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
#include "../../brender/core/v1db/dbsetup.h"
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

// GLOBAL: CARMA2_HW 0x006a2488
tSaved_table gSaved_shade_tables[100];

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

// GLOBAL: CARMA2_HW 0x0068d8c0
br_actor* gLit_op_stat_actor;


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

// GLOBAL: CARMA2_HW 0x006a2128
void* gLollipop_queue[100];

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

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrPixelmapCopy();
extern void C2_HOOK_CDECL BrPixelmapLine();
extern void C2_HOOK_CDECL BrPixelmapPixelSet();
extern tU16 C2_HOOK_FASTCALL Colour24BitTo16Bit(br_colour pColour);
extern void C2_HOOK_FASTCALL DRPixelmapCopy(br_pixelmap* dst, br_pixelmap* src);
extern void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);
extern int C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
extern void C2_HOOK_FASTCALL ReinitialiseForwardCamera(void);
extern void C2_HOOK_FASTCALL ReinitialiseRenderStuff(void);
extern void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height);
extern void __cdecl _ftol();
extern br_uint_8 bl;
extern const int gArrows[2][4][60];
extern int gBack_original_base_x;
extern int gBack_original_base_y;
extern int gBack_original_origin_x;
extern int gBack_original_origin_y;
extern br_pixelmap* gBack_screen;
extern tGraf_data* gCurrent_graf_data;
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern int gGraf_data_index;
extern int gHeight;
extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted4;
extern int gINT_0068d6f4;
extern int gMap_view;
extern br_pixelmap* gMini_map_glowing_line_palettes[3];
extern tNet_mode gNet_mode;
extern tProgram_state gProgram_state;
extern br_pixelmap* gRender_palette;
extern br_pixelmap* gRender_screen;
extern tCar_spec* gTarget_lock_car_1;
extern tCar_spec* gTarget_lock_car_2;
extern int gWidth;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrClipPlaneDisable();
extern void C2_HOOK_CDECL BrClipPlaneEnable();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34Mul();
extern void C2_HOOK_CDECL BrMatrix34TApplyV();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelRemove();
extern void C2_HOOK_CDECL BrPixelmapFill();
extern void C2_HOOK_CDECL BrSetScreenZOffset();
extern void C2_HOOK_CDECL BrTableUpdate();
extern void C2_HOOK_CDECL BrZbsSceneRenderAdd();
extern void C2_HOOK_CDECL BrZbsSceneRenderBegin();
extern void C2_HOOK_CDECL BrZbsSceneRenderEnd();
extern void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);
extern int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);
extern void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);
extern void C2_HOOK_FASTCALL DisableLights(void);
extern void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world);
extern void C2_HOOK_FASTCALL EnableLights(void);
extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);
extern int C2_HOOK_FASTCALL GetOilSpillCount(void);
extern void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize);
extern int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs);
extern int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);
extern void C2_HOOK_FASTCALL PossibleLock(int pValue);
extern void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex);
extern void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);
extern void C2_HOOK_FASTCALL RenderElectroBastardRays(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);
extern void C2_HOOK_FASTCALL RenderLimbs(void);
extern void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);
extern void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);
extern void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);
extern void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);
extern void C2_HOOK_FASTCALL RenderTintedPolys(void);
extern void C2_HOOK_FASTCALL SetSkyColour(br_colour c);
extern void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial);
extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);
extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);
extern void __cdecl _chkstk();
extern int gAFE;
extern br_actor* gCamera;
extern char gConst_kangeroo_force_factor[];
extern float gCop_speed_factor;
extern tSpecial_volume* gDAT_0079ec44;
extern br_pixelmap* gDepth_buffer;
extern br_pixelmap* gDepth_shade_table;
extern char gDouble_0058a068[];
extern tU32 gFrame_period;
extern int gHeadup_map_half_height;
extern int gHeadup_map_half_width;
extern int gINT_0074ab88;
extern int gINT_0074ab8c;
extern int gIt_or_fox;
extern tLighting_data gLighting_data;
extern tU32 gMap_time;
extern int gMini_map_visible;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_net_players;
extern br_actor* gOther_selfs[4];
extern br_pixelmap* gPalette_0074a5fc;
extern br_pixelmap* gPalette_0074a600;
extern br_pixelmap* gPalette_0074a604;
extern int gPed_676914;
extern int gSaved_table_count;
extern int gShadow_workaround;
extern tDepth_effect_type gSwap_depth_effect_type;
extern br_actor* gUniverse_actor;
#endif
// FUNCTION: CARMA2_HW 0x004b5460
void C2_HOOK_FASTCALL FadePaletteDown(void) {
}

// FUNCTION: CARMA2_HW 0x004e9960
void C2_HOOK_FASTCALL ToggleShadow(void) {

    if (gShadow_level++ == eShadow_everyone) {
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

// FUNCTION: CARMA2_HW 0x0047c670
void C2_NAKED C2_HOOK_FASTCALL FUN_0047c670(br_pixelmap* pDest, br_pixelmap* pSrc, int pWidth, int pHeight, int pArg5, int pDest_y) {
#ifndef CARPOCALYPSE2_MATCHING
    int dest_width;
    int dest_step;
    int src_step;
    int src_pixels_per_row;
    int pixels_left;
    tU16 pixel;
    tU8 pixel_high;
    tU8 pixel_low;
    tU8* dest_top;
    tU8* dest_bottom;
    int src_row;

    dest_width = (short)pDest->row_bytes;
    dest_step = dest_width - pWidth;
    dest_step *= 2;
    src_step = ((short)pSrc->row_bytes - pWidth) / 2;
    src_row = (int)pSrc->pixels + ((pHeight * (short)pSrc->row_bytes) / 2 - src_step) * 2;
    dest_top = (tU8*)((pDest_y - 1 + pHeight * 2) * dest_width + (int)pDest->pixels + pWidth * 2);
    dest_bottom = dest_top - dest_width;
    src_pixels_per_row = pWidth / 2;
    if (pHeight > 0) {
        pWidth = pHeight;
        do {
            pixels_left = src_pixels_per_row;
            if (pixels_left > 0) {
                do {
                    pixel = *(tU16*)(src_row - 2);
                    src_row = src_row - 2;
                    pixel_high = (tU8)(pixel >> 8);
                    dest_top--;
                    dest_bottom--;
                    *dest_top = pixel_high;
                    dest_top--;
                    *dest_bottom = pixel_high;
                    dest_bottom--;
                    *dest_top = pixel_high;
                    dest_top--;
                    *dest_bottom = pixel_high;
                    dest_bottom--;
                    pixel_low = (tU8)pixel;
                    *dest_top = pixel_low;
                    dest_top--;
                    *dest_bottom = pixel_low;
                    dest_bottom--;
                    *dest_top = pixel_low;
                    pixels_left--;
                    *dest_bottom = pixel_low;
                } while (pixels_left != 0);
            }
            dest_top -= dest_step;
            dest_bottom -= dest_step;
            src_row += src_step * -2;
            pWidth--;
        } while (pWidth != 0);
    }
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            movsx ebp, word ptr [ecx + 0x28]
            push esi
            mov esi, edx
            mov edx, dword ptr [esp + 0x18]
            push edi
            movsx edi, word ptr [esi + 0x28]
            mov eax, edi
            mov dword ptr [esp + 0x10], ebp
            sub eax, edx
            sub ebp, edx
            cdq
            sub eax, edx
            mov ebx, eax
            mov eax, dword ptr [esp + 0x20]
            imul eax, edi
            cdq
            sub eax, edx
            mov edx, dword ptr [esi + 8]
            sar ebx, 1
            sar eax, 1
            sub eax, ebx
            mov dword ptr [esp + 0x14], ebx
            shl ebp, 1
            lea edi, [edx + eax*2]
            mov eax, dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x20]
            lea eax, [eax + edx*2 - 1]
            mov edx, dword ptr [ecx + 8]
            imul eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x1c]
            add eax, edx
            lea esi, [eax + ecx*2]
            mov eax, dword ptr [esp + 0x10]
            mov ecx, esi
            sub ecx, eax
            mov eax, dword ptr [esp + 0x1c]
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x20]
            sar eax, 1
            test edx, edx
            mov dword ptr [esp + 0x28], eax
            jle L0047c736
            mov dword ptr [esp + 0x1c], edx
    L0047c6ee:
            test eax, eax
            jle L0047c720
    L0047c6f2:
            mov dx, word ptr [edi - 2]
            sub edi, 2
            xor ebx, ebx
            dec esi
            mov bl, dh
            dec ecx
            mov byte ptr [esi], bl
            dec esi
            mov byte ptr [ecx], bl
            dec ecx
            mov byte ptr [esi], bl
            dec esi
            mov byte ptr [ecx], bl
            dec ecx
            mov byte ptr [esi], dl
            dec esi
            mov byte ptr [ecx], dl
            dec ecx
            mov byte ptr [esi], dl
            dec eax
            mov byte ptr [ecx], dl
            jne L0047c6f2
            mov ebx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x28]
    L0047c720:
            mov edx, ebx
            sub esi, ebp
            neg edx
            sub ecx, ebp
            lea edi, [edi + edx*2]
            mov edx, dword ptr [esp + 0x1c]
            dec edx
            mov dword ptr [esp + 0x1c], edx
            jne L0047c6ee
    L0047c736:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e4b40
void C2_NAKED C2_HOOK_FASTCALL AdjustRenderScreenSize(void) {
#ifndef CARPOCALYPSE2_MATCHING

    ReinitialiseRenderStuff();
    if (gMap_view == 2) {
        gRender_screen->base_x = (br_uint_16)gMap_render_x_i;
        gRender_screen->base_y = (br_uint_16)gMap_render_y_i;
        gRender_screen->width = (br_uint_16)gMap_render_width_i;
        gRender_screen->height = (br_uint_16)gMap_render_height_i;
    } else {
        gRender_screen->base_x = (br_uint_16)gProgram_state.current_render_left;
        gRender_screen->base_y = (br_uint_16)gProgram_state.current_render_top;
        gRender_screen->height = (br_uint_16)(gProgram_state.current_render_bottom - gProgram_state.current_render_top);
        gRender_screen->width = (br_uint_16)(gProgram_state.current_render_right - gProgram_state.current_render_left);
    }
    if (gRender_screen->row_bytes != gRender_screen->width) {
        gRender_screen->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
    } else {
        gRender_screen->flags |= BR_PMF_ROW_WHOLEPIXELS;
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
#else
    __asm {
            call ReinitialiseRenderStuff
            cmp dword ptr [gMap_view], 2
            jne L004e4b91
            mov eax, dword ptr [gRender_screen]
            mov cx, word ptr [gMap_render_x_i]
            mov word ptr [eax + 0x30], cx
            mov edx, dword ptr [gRender_screen]
            mov ax, word ptr [gMap_render_y_i]
            mov word ptr [edx + 0x32], ax
            mov ecx, dword ptr [gRender_screen]
            mov dx, word ptr [gMap_render_width_i]
            mov word ptr [ecx + 0x34], dx
            mov eax, dword ptr [gRender_screen]
            mov cx, word ptr [gMap_render_height_i]
            mov word ptr [eax + 0x36], cx
            jmp L004e4bdf
    L004e4b91:
            mov edx, dword ptr [gRender_screen]
            mov ax, word ptr [gProgram_state+0x54]
            mov word ptr [edx + 0x30], ax
            mov edx, dword ptr [gRender_screen]
            mov cx, word ptr [gProgram_state+0x58]
            mov word ptr [edx + 0x32], cx
            mov eax, dword ptr [gProgram_state+0x60]
            mov ecx, dword ptr [gProgram_state+0x58]
            sub eax, ecx
            mov ecx, dword ptr [gRender_screen]
            mov word ptr [ecx + 0x36], ax
            mov edx, dword ptr [gProgram_state+0x5c]
            mov eax, dword ptr [gProgram_state+0x54]
            sub edx, eax
            mov eax, dword ptr [gRender_screen]
            mov word ptr [eax + 0x34], dx
    L004e4bdf:
            mov eax, dword ptr [gRender_screen]
            xor ecx, ecx
            movsx edx, word ptr [eax + 0x28]
            mov cx, word ptr [eax + 0x34]
            cmp ecx, edx
            mov cl, byte ptr [eax + 0x2d]
            je L004e4bfa
            and cl, 0xfb
            jmp L004e4bfd
    L004e4bfa:
            or cl, 4
    L004e4bfd:
            mov byte ptr [eax + 0x2d], cl
            mov eax, dword ptr [gRender_screen]
            mov cx, word ptr [eax + 0x34]
            shr cx, 1
            mov word ptr [eax + 0x38], cx
            mov eax, dword ptr [gRender_screen]
            xor ecx, ecx
            mov dx, word ptr [eax + 0x36]
            shr dx, 1
            mov word ptr [eax + 0x3a], dx
            mov eax, dword ptr [gRender_screen]
            xor edx, edx
            mov cx, word ptr [eax + 0x34]
            mov dword ptr [gWidth], ecx
            mov dx, word ptr [eax + 0x36]
            mov dword ptr [gHeight], edx
            call ReinitialiseForwardCamera
            mov eax, dword ptr [gRender_screen]
            xor ecx, ecx
            xor edx, edx
            mov cx, word ptr [eax + 0x36]
            mov dx, word ptr [eax + 0x34]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [eax + 0x32]
            push edx
            xor edx, edx
            push ecx
            mov dx, word ptr [eax + 0x30]
            mov ecx, dword ptr [gHud_tinted1]
            call SetTintedPolySize
            mov eax, dword ptr [gRender_screen]
            xor edx, edx
            xor ecx, ecx
            mov dx, word ptr [eax + 0x36]
            mov cx, word ptr [eax + 0x34]
            push edx
            xor edx, edx
            mov dx, word ptr [eax + 0x32]
            push ecx
            mov ecx, dword ptr [gHud_tinted2]
            push edx
            xor edx, edx
            mov dx, word ptr [eax + 0x30]
            call SetTintedPolySize
            mov eax, dword ptr [gRender_screen]
            xor ecx, ecx
            xor edx, edx
            mov cx, word ptr [eax + 0x36]
            mov dx, word ptr [eax + 0x34]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [eax + 0x32]
            push edx
            xor edx, edx
            push ecx
            mov dx, word ptr [eax + 0x30]
            mov ecx, dword ptr [gHud_tinted4]
            call SetTintedPolySize
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
    br_pixelmap* pal = gRender_palette;

    gLast_palette_change = 0;
    gPalette_index = 0;
    ((br_int_32*)pal->pixels)[0] = 0;
    memcpy(gCurrent_palette_pixels, pal->pixels, 4 * 256);
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pal);
    }
    gPalette_munged |= (pal != gRender_palette);
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
                mod_angle = fmod((double)time_going / gWobble_array[i].period, TAU);
                if (mod_angle > 1.5 * PI) {
                    cosine_over_angle = gCosine_array[(unsigned int)((TAU - mod_angle) / PI * 128.)];
                } else if (mod_angle > PI) {
                    cosine_over_angle = -gCosine_array[(unsigned int)((mod_angle - PI) / PI * 128.)];
                } else if (mod_angle > .5 * PI) {
                    cosine_over_angle = -gCosine_array[(unsigned int)((PI - mod_angle) / PI * 128.)];
                } else {
                    cosine_over_angle = gCosine_array[(unsigned int)(mod_angle / PI * 128.)];
                }
                angle = cosine_over_angle / (1. + (double)time_going * .0035);
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

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        br_actor* actor = gPolyfont_glyph_actors[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gCount_polyfont_glyph_actors = 0;
    gHud_actor_storage_size = 0;
    gRender_poly_text = 0;
    MapStuffBeforeRender();
    if (!gAction_replay_mode && gProgram_state.new_view == eView_undefined) {
        int j;
        tU32 time_going;
        double mod_angle;
        double cosine_over_angle;

        gScreen_wobble_x = 0;
        gScreen_wobble_y = 0;
        for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(gWobble_array); j++) {
            if (gWobble_array[j].time_started != 0) {
                time_going = the_time - gWobble_array[j].time_started;
                if (time_going > 1000) {
                    gWobble_array[j].time_started = 0;
                } else {
                    mod_angle = fmod((double)time_going / gWobble_array[j].period, TAU);
                    if (mod_angle > 1.5 * PI) {
                        cosine_over_angle = gCosine_array[(unsigned int)((TAU - mod_angle) / PI * 128.)];
                    } else if (mod_angle > PI) {
                        cosine_over_angle = -gCosine_array[(unsigned int)((mod_angle - PI) / PI * 128.)];
                    } else if (mod_angle > .5 * PI) {
                        cosine_over_angle = -gCosine_array[(unsigned int)((PI - mod_angle) / PI * 128.)];
                    } else {
                        cosine_over_angle = gCosine_array[(unsigned int)(mod_angle / PI * 128.)];
                    }
                    gScreen_wobble_x = (int)((double)gScreen_wobble_x + (double)gWobble_array[j].amplitude_x * cosine_over_angle / (1. + (double)time_going * .0035));
                    gScreen_wobble_y = (int)((double)gScreen_wobble_y + (double)gWobble_array[j].amplitude_y * cosine_over_angle / (1. + (double)time_going * .0035));
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
    if (cockpit_on) {
        if (-gScreen_wobble_x > gX_offset) {
            x_shift = -gX_offset;
        } else if (gBack_screen->width < gX_offset + gScreen_wobble_x + gRender_screen->width) {
            x_shift = gBack_screen->width - gRender_screen->width - gX_offset;
        } else {
            x_shift = gScreen_wobble_x;
        }
        if (-gScreen_wobble_y > gY_offset) {
            y_shift = -gY_offset;
        } else if (gBack_screen->height < gY_offset + gScreen_wobble_y + gRender_screen->height) {
            y_shift = gBack_screen->height - gRender_screen->height - gY_offset;
        } else {
            y_shift = gScreen_wobble_y;
        }
    } else {
        x_shift = 0;
        y_shift = 0;
    }
    old_camera_matrix = gCamera->t.t.mat;
    if (gMirror_on__graphics) {
        old_mirror_cam_matrix = gRearview_camera->t.t.mat;
    }
    if (cockpit_on) {
        gSheer_mat.m[2][1] = y_shift / (float)gRender_screen->height;
        gSheer_mat.m[2][0] = (float)-x_shift / (float)gRender_screen->width;
        BrMatrix34Pre(&gCamera->t.t.mat, &gSheer_mat);
        gCamera->t.t.translate.t.v[0] -= gScreen_wobble_x * 1.5f / gRender_screen->width / WORLD_SCALE;
        gCamera->t.t.translate.t.v[1] = gScreen_wobble_y * 1.5f / gRender_screen->width / WORLD_SCALE + gCamera->t.t.translate.t.v[1];
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
    br_pixelmap* pal;

    memcpy(gRender_palette->pixels, gOrig_render_palette->pixels, 4 * 256);
    pal = gRender_palette;
    memcpy(gCurrent_palette_pixels, pal->pixels, 4 * 256);
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pal);
    }
    gPalette_munged |= (pal != gRender_palette);
}

// FUNCTION: CARMA2_HW 0x004e9680
void C2_NAKED C2_HOOK_FASTCALL MungeClipPlane(br_vector3* pLight, tCar_spec* pCar, br_vector3* p1, br_vector3* p2, br_vector3* pOffset) {
#ifndef CARPOCALYPSE2_MATCHING
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
    if (fabs(v4.v[0]) >= 0.01 || fabs(v4.v[1]) >= 0.01 || fabs(v4.v[2]) >= 0.01) {
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
        length = (v1.v[2] - v2.v[2]) * (v1.v[2] - v2.v[2]) + (v1.v[0] - v2.v[0]) * (v1.v[0] - v2.v[0]);
        gShadow_clip_planes[gShadow_clip_plane_count].length = length;
        gShadow_clip_plane_count += 1;
    }
#else
    __asm {
            sub esp, 0x30
            push ebx
            push ebp
            mov ebp, edx
            push esi
            push edi
            mov edi, dword ptr [esp + 0x44]
            mov edx, dword ptr [ebp + 0x10]
            lea eax, [esp + 0x28]
            add edx, 0x2c
            mov esi, ecx
            push edx
            push edi
            push eax
            call BrMatrix34ApplyP
            mov eax, dword ptr [ebp + 0x10]
            mov ebx, dword ptr [esp + 0x54]
            add esp, 0xc
            add eax, 0x2c
            lea ecx, [esp + 0x34]
            push eax
            push ebx
            push ecx
            call BrMatrix34ApplyP
            fld dword ptr [ebx + 8]
            fld dword ptr [ebx + 4]
            fsub dword ptr [edi + 4]
            fxch st(1)
            fsub dword ptr [edi + 8]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fxch st(3)
            fstp dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(2)
            fmul st(0), st(1)
            _emit 0xd9
        _emit 0x03
            _emit 0xd8
        _emit 0x27
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld st(3)
            fld dword ptr [esp + 0x2c]
            fxch st(4)
            fstp dword ptr [esp + 0x28]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            _emit 0xd8
        _emit 0x0e
            fxch st(5)
            fabs
            fxch st(3)
            _emit 0xd8
        _emit 0x0e
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e9766
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e9766
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e9928
    L004e9766:
            fld dword ptr [edi + 4]
            mov eax, dword ptr [esp + 0x4c]
            _emit 0xd9
        _emit 0x07
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [edi + 8]
            fsub dword ptr [eax + 8]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e97d3
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e97d3:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e9834
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x24]
            jmp L004e984e
    L004e9834:
            fstp st(0)
            mov dword ptr [esp + 0x1c], 0x3f800000
            mov dword ptr [esp + 0x20], 0
            mov dword ptr [esp + 0x24], 0
    L004e984e:
            mov edx, dword ptr [ebp + 0x10]
            lea eax, [esp + 0x1c]
            add edx, 0x2c
            lea ecx, [esp + 0x10]
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x3c]
            mov edx, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x34]
            fxch st(1)
            fsub dword ptr [esp + 0x3c]
            fxch st(2)
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            mov ecx, dword ptr [eax + 0x5c]
            _emit 0xd9
        _emit 0x19
            mov edx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            mov ecx, dword ptr [eax + 0x5c]
            fstp dword ptr [ecx + 4]
            mov edx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            mov ecx, dword ptr [eax + 0x5c]
            fstp dword ptr [ecx + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            faddp st(2), st(0)
            mov edx, dword ptr [gShadow_clip_plane_count]
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov ecx, dword ptr [eax + 0x5c]
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fchs
            fstp dword ptr [ecx + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
    L004e9928:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047b910
br_scalar C2_NAKED C2_HOOK_FASTCALL DistanceFromPlane(br_vector3* pPos, br_scalar arg2, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD) {
#ifndef CARPOCALYPSE2_MATCHING

    return fabsf((pPos->v[0] * pA + pPos->v[1] * pB + pPos->v[2] * pC + pD) / (pA * pA + pB * pB + pC * pC));
#else
    __asm {
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx]
            fmul dword ptr [esp + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [esp + 4]
            fld dword ptr [esp + 8]
            fxch st(4)
            fmul dword ptr [esp + 8]
            faddp st(3), st(0)
            fmul dword ptr [esp + 4]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            faddp st(3), st(0)
            fxch st(3)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fadd dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            fabs
            ret 0x10
            nop
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor) {
#ifndef CARPOCALYPSE2_MATCHING
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
            BrZbsSceneRenderEnd();

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
#else
    __asm {
            mov eax, 0x2a94
            call _chkstk
            push ebx
            mov ebx, ecx
            push ebp
            xor ebp, ebp
            mov edx, dword ptr [ebx + 0xe0c]
            push esi
            push edi
            mov dword ptr [esp + 0x84], ebx
            mov eax, dword ptr [edx + 0x60]
            lea ecx, [esp + 0x2c]
            mov dword ptr [esp + 0x70], ebp
            mov eax, dword ptr [eax]
            fld dword ptr [eax + 0x40]
            fld dword ptr [eax + 0x38]
            fld dword ptr [eax + 0x44]
            mov edx, dword ptr [eax + 0x34]
            fld dword ptr [eax + 0x3c]
            fld dword ptr [eax + 0x48]
            mov dword ptr [esp + 0x60], edx
            fld dword ptr [esp + 0x60]
            fxch st(5)
            fst dword ptr [esp + 0x38]
            fxch st(4)
            fst dword ptr [esp + 0x6c]
            fxch st(3)
            fst dword ptr [esp + 0x64]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x5c]
            fxch st(3)
            fstp dword ptr [gShadow_points]
            mov eax, dword ptr [esp + 0x5c]
            mov edx, dword ptr [esp + 0x38]
            fstp dword ptr [gShadow_points+0x4]
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x60]
            fxch st(4)
            fstp dword ptr [gShadow_points+0x14]
            fld dword ptr [esp + 0x64]
            fxch st(5)
            fstp dword ptr [gShadow_points+0x18]
            fld dword ptr [esp + 0x5c]
            fxch st(2)
            fstp dword ptr [gShadow_points+0x1c]
            fld dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [gShadow_points+0x20]
            fld dword ptr [esp + 0x5c]
            fxch st(4)
            fstp dword ptr [gShadow_points+0x24]
            fld dword ptr [esp + 0x38]
            fxch st(5)
            fstp dword ptr [gShadow_points+0x28]
            fld dword ptr [esp + 0x6c]
            fxch st(2)
            fstp dword ptr [gShadow_points+0x2c]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [gShadow_points+0x30]
            fld dword ptr [esp + 0x60]
            fxch st(3)
            fstp dword ptr [gShadow_points+0x34]
            fld dword ptr [esp + 0x6c]
            fxch st(4)
            fstp dword ptr [gShadow_points+0x38]
            fld dword ptr [esp + 0x1c]
            fxch st(5)
            fstp dword ptr [gShadow_points+0x3c]
            fld dword ptr [esp + 0x60]
            fxch st(2)
            fstp dword ptr [gShadow_points+0x40]
            fld dword ptr [esp + 0x6c]
            fxch st(1)
            mov dword ptr [gShadow_points+0x8], eax
            mov eax, dword ptr [esp + 0x64]
            mov dword ptr [gShadow_points+0xc], edx
            mov dword ptr [gShadow_points+0x10], eax
            fstp dword ptr [gShadow_points+0x44]
            fld dword ptr [esp + 0x5c]
            fxch st(3)
            fstp dword ptr [gShadow_points+0x48]
            fxch st(3)
            fstp dword ptr [gShadow_points+0x4c]
            fxch st(3)
            fstp dword ptr [gShadow_points+0x50]
            fxch st(2)
            fstp dword ptr [gShadow_points+0x54]
            mov dword ptr [gShadow_clip_plane_count], ebp
            fstp dword ptr [gShadow_points+0x58]
            fstp dword ptr [gShadow_points+0x5c]
            mov edx, dword ptr [ebx + 0x10]
            add edx, 0x2c
            push edx
            push offset gShadow_light_ray
            push ecx
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x44]
            fadd dword ptr [esp + 0x6c]
            add esp, 0xc
            mov esi, 6
            fmul dword ptr [gConst_kangeroo_force_factor+0x16c]
            fstp dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x64]
            fadd dword ptr [esp + 0x6c]
            fxch st(1)
            fadd dword ptr [esp + 0x1c]
            fld st(2)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x16c]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x16c]
            fxch st(1)
            fstp dword ptr [esp + 0x54]
            fnstsw ax
            fstp dword ptr [esp + 0x58]
            test ah, 1
            jne L004e7858
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e787c
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e785a
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e787c
            jmp L004e785a
    L004e7858:
            fstp st(0)
    L004e785a:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e787c
            lea eax, [esp + 0x50]
            mov edx, ebx
            push eax
            push offset gShadow_points+0x24
            push offset gShadow_points
            lea ecx, [esp + 0x38]
            call MungeClipPlane
    L004e787c:
            fld dword ptr [esp + 0x34]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fld st(0)
            fmul dword ptr [esp + 0x30]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e78c6
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e78ee
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e78ca
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e78f0
            jmp L004e78ca
    L004e78c6:
            fstp st(0)
            fstp st(0)
    L004e78ca:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e78f0
            lea ecx, [esp + 0x50]
            mov edx, ebx
            push ecx
            push offset gShadow_points+0x18
            push offset gShadow_points+0xc
            lea ecx, [esp + 0x38]
            call MungeClipPlane
            jmp L004e78f0
    L004e78ee:
            fstp st(0)
    L004e78f0:
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fmul st(0), st(1)
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e7948
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e7970
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e794c
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e7972
            jmp L004e794c
    L004e7948:
            fstp st(0)
            fstp st(0)
    L004e794c:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e7972
            lea edx, [esp + 0x50]
            lea ecx, [esp + 0x2c]
            push edx
            push offset gShadow_points+0x3c
            push offset gShadow_points+0x48
            mov edx, ebx
            call MungeClipPlane
            jmp L004e7972
    L004e7970:
            fstp st(0)
    L004e7972:
            fld dword ptr [esp + 0x30]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fst dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x34]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e79c2
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e79e6
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e79c4
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e79e6
            jmp L004e79c4
    L004e79c2:
            fstp st(0)
    L004e79c4:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e79e6
            lea eax, [esp + 0x50]
            mov edx, ebx
            push eax
            push offset gShadow_points+0x30
            push offset gShadow_points+0x54
            lea ecx, [esp + 0x38]
            call MungeClipPlane
    L004e79e6:
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x2c]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e7a2c
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e7a50
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e7a2e
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e7a50
            jmp L004e7a2e
    L004e7a2c:
            fstp st(0)
    L004e7a2e:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e7a50
            lea ecx, [esp + 0x50]
            mov edx, ebx
            push ecx
            push offset gShadow_points
            push offset gShadow_points+0xc
            lea ecx, [esp + 0x38]
            call MungeClipPlane
    L004e7a50:
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fld st(0)
            fmul dword ptr [esp + 0x30]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e7aa2
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e7d73
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e7aa6
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e7d75
            jmp L004e7aa6
    L004e7aa2:
            fstp st(0)
            fstp st(0)
    L004e7aa6:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e7d75
            mov edx, dword ptr [ebx + 0x10]
            lea eax, [esp + 0x3c]
            add edx, 0x2c
            push edx
            push offset gShadow_points+0x18
            push eax
            call BrMatrix34ApplyP
            mov ecx, dword ptr [ebx + 0x10]
            add esp, 0xc
            add ecx, 0x2c
            lea edx, [esp + 0x74]
            push ecx
            push offset gShadow_points+0x24
            push edx
            call BrMatrix34ApplyP
            fld dword ptr [gShadow_points+0x2c]
            fld dword ptr [gShadow_points+0x28]
            fsub dword ptr [gShadow_points+0x1c]
            fxch st(1)
            fsub dword ptr [gShadow_points+0x20]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [gShadow_points+0x24]
            fsub dword ptr [gShadow_points+0x18]
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld st(3)
            fld dword ptr [esp + 0x30]
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x38]
            fxch st(5)
            fabs
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e7ba9
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e7ba9
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e7d75
    L004e7ba9:
            fld dword ptr [gShadow_points+0x1c]
            fld dword ptr [gShadow_points+0x18]
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [gShadow_points+0x20]
            fsub dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e7c20
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e7c20:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e7c81
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            jmp L004e7c9b
    L004e7c81:
            fstp st(0)
            mov dword ptr [esp + 0x20], 0x3f800000
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e7c9b:
            mov eax, dword ptr [ebx + 0x10]
            lea ecx, [esp + 0x20]
            add eax, 0x2c
            lea edx, [esp + 0x10]
            push eax
            push ecx
            push edx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x50]
            mov eax, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fsub dword ptr [esp + 0x7c]
            fxch st(2)
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            _emit 0xd9
        _emit 0x1a
            mov eax, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            fstp dword ptr [edx + 4]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            fstp dword ptr [edx + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            mov eax, dword ptr [gShadow_clip_plane_count]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov edx, dword ptr [ecx + 0x5c]
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fchs
            fstp dword ptr [edx + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
            jmp L004e7d75
    L004e7d73:
            fstp st(0)
    L004e7d75:
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fmul st(0), st(1)
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e7dd5
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e80aa
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e7dd9
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e80ac
            jmp L004e7dd9
    L004e7dd5:
            fstp st(0)
            fstp st(0)
    L004e7dd9:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e80ac
            mov eax, dword ptr [ebx + 0x10]
            lea ecx, [esp + 0x3c]
            add eax, 0x2c
            push eax
            push offset gShadow_points+0x54
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [ebx + 0x10]
            add esp, 0xc
            add edx, 0x2c
            lea eax, [esp + 0x74]
            push edx
            push offset gShadow_points+0x48
            push eax
            call BrMatrix34ApplyP
            fld dword ptr [gShadow_points+0x50]
            fld dword ptr [gShadow_points+0x4c]
            fsub dword ptr [gShadow_points+0x58]
            fxch st(1)
            fsub dword ptr [gShadow_points+0x5c]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [gShadow_points+0x48]
            fsub dword ptr [gShadow_points+0x54]
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld st(3)
            fld dword ptr [esp + 0x30]
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x38]
            fxch st(5)
            fabs
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e7edc
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e7edc
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e80ac
    L004e7edc:
            fld dword ptr [gShadow_points+0x58]
            fld dword ptr [gShadow_points+0x54]
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [gShadow_points+0x5c]
            fsub dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e7f53
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e7f53:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e7fb4
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            jmp L004e7fce
    L004e7fb4:
            fstp st(0)
            mov dword ptr [esp + 0x20], 0x3f800000
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e7fce:
            mov ecx, dword ptr [ebx + 0x10]
            lea edx, [esp + 0x20]
            add ecx, 0x2c
            lea eax, [esp + 0x10]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x50]
            mov ecx, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fsub dword ptr [esp + 0x7c]
            fxch st(2)
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            mov eax, dword ptr [edx + 0x5c]
            _emit 0xd9
        _emit 0x18
            mov ecx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            mov eax, dword ptr [edx + 0x5c]
            fstp dword ptr [eax + 4]
            mov ecx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            mov eax, dword ptr [edx + 0x5c]
            fstp dword ptr [eax + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            mov ecx, dword ptr [gShadow_clip_plane_count]
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov eax, dword ptr [edx + 0x5c]
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fchs
            fstp dword ptr [eax + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
            jmp L004e80ac
    L004e80aa:
            fstp st(0)
    L004e80ac:
            fld dword ptr [esp + 0x30]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fst dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x2c]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e80fc
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e8120
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e80fe
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e8120
            jmp L004e80fe
    L004e80fc:
            fstp st(0)
    L004e80fe:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e8120
            lea ecx, [esp + 0x50]
            mov edx, ebx
            push ecx
            push offset gShadow_points+0x3c
            push offset gShadow_points+0x30
            lea ecx, [esp + 0x38]
            call MungeClipPlane
    L004e8120:
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x2c]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e816e
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e843b
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e8170
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e843b
            jmp L004e8170
    L004e816e:
            fstp st(0)
    L004e8170:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e843b
            mov edx, dword ptr [ebx + 0x10]
            lea eax, [esp + 0x3c]
            add edx, 0x2c
            push edx
            push offset gShadow_points+0x30
            push eax
            call BrMatrix34ApplyP
            mov ecx, dword ptr [ebx + 0x10]
            add esp, 0xc
            add ecx, 0x2c
            lea edx, [esp + 0x74]
            push ecx
            push offset gShadow_points
            push edx
            call BrMatrix34ApplyP
            fld dword ptr [gShadow_points+0x8]
            fld dword ptr [gShadow_points+0x4]
            fsub dword ptr [gShadow_points+0x34]
            fxch st(1)
            fsub dword ptr [gShadow_points+0x38]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [gShadow_points]
            fsub dword ptr [gShadow_points+0x30]
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld st(3)
            fld dword ptr [esp + 0x30]
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x38]
            fxch st(5)
            fabs
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e8273
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e8273
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e843b
    L004e8273:
            fld dword ptr [gShadow_points+0x34]
            fld dword ptr [gShadow_points+0x30]
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [gShadow_points+0x38]
            fsub dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e82ea
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e82ea:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e834b
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            jmp L004e8365
    L004e834b:
            fstp st(0)
            mov dword ptr [esp + 0x20], 0x3f800000
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e8365:
            mov eax, dword ptr [ebx + 0x10]
            lea ecx, [esp + 0x20]
            add eax, 0x2c
            lea edx, [esp + 0x10]
            push eax
            push ecx
            push edx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x50]
            mov eax, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fsub dword ptr [esp + 0x7c]
            fxch st(2)
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            _emit 0xd9
        _emit 0x1a
            mov eax, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            fstp dword ptr [edx + 4]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            fstp dword ptr [edx + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            mov eax, dword ptr [gShadow_clip_plane_count]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov edx, dword ptr [ecx + 0x5c]
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fchs
            fstp dword ptr [edx + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
    L004e843b:
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fld st(0)
            fmul dword ptr [esp + 0x34]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e848d
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e8762
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e8491
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e8764
            jmp L004e8491
    L004e848d:
            fstp st(0)
            fstp st(0)
    L004e8491:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e8764
            mov eax, dword ptr [ebx + 0x10]
            lea ecx, [esp + 0x3c]
            add eax, 0x2c
            push eax
            push offset gShadow_points+0x24
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [ebx + 0x10]
            add esp, 0xc
            add edx, 0x2c
            lea eax, [esp + 0x74]
            push edx
            push offset gShadow_points+0x54
            push eax
            call BrMatrix34ApplyP
            fld dword ptr [gShadow_points+0x5c]
            fld dword ptr [gShadow_points+0x58]
            fsub dword ptr [gShadow_points+0x28]
            fxch st(1)
            fsub dword ptr [gShadow_points+0x2c]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [gShadow_points+0x54]
            fsub dword ptr [gShadow_points+0x24]
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld st(3)
            fld dword ptr [esp + 0x30]
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x38]
            fxch st(5)
            fabs
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e8594
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e8594
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e8764
    L004e8594:
            fld dword ptr [gShadow_points+0x28]
            fld dword ptr [gShadow_points+0x24]
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [gShadow_points+0x2c]
            fsub dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e860b
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e860b:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e866c
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            jmp L004e8686
    L004e866c:
            fstp st(0)
            mov dword ptr [esp + 0x20], 0x3f800000
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e8686:
            mov ecx, dword ptr [ebx + 0x10]
            lea edx, [esp + 0x20]
            add ecx, 0x2c
            lea eax, [esp + 0x10]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x50]
            mov ecx, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fsub dword ptr [esp + 0x7c]
            fxch st(2)
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            mov eax, dword ptr [edx + 0x5c]
            _emit 0xd9
        _emit 0x18
            mov ecx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            mov eax, dword ptr [edx + 0x5c]
            fstp dword ptr [eax + 4]
            mov ecx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            mov eax, dword ptr [edx + 0x5c]
            fstp dword ptr [eax + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            mov ecx, dword ptr [gShadow_clip_plane_count]
            mov edx, dword ptr [ecx*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov eax, dword ptr [edx + 0x5c]
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fchs
            fstp dword ptr [eax + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
            jmp L004e8764
    L004e8762:
            fstp st(0)
    L004e8764:
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fmul st(0), st(1)
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e87c4
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e8a99
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e87c8
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e8a9b
            jmp L004e87c8
    L004e87c4:
            fstp st(0)
            fstp st(0)
    L004e87c8:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e8a9b
            mov ecx, dword ptr [ebx + 0x10]
            lea edx, [esp + 0x3c]
            add ecx, 0x2c
            push ecx
            push offset gShadow_points+0x18
            push edx
            call BrMatrix34ApplyP
            mov eax, dword ptr [ebx + 0x10]
            add esp, 0xc
            add eax, 0x2c
            lea ecx, [esp + 0x74]
            push eax
            push offset gShadow_points+0x48
            push ecx
            call BrMatrix34ApplyP
            fld dword ptr [gShadow_points+0x50]
            fld dword ptr [gShadow_points+0x4c]
            fsub dword ptr [gShadow_points+0x1c]
            fxch st(1)
            fsub dword ptr [gShadow_points+0x20]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [gShadow_points+0x48]
            fsub dword ptr [gShadow_points+0x18]
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld st(3)
            fld dword ptr [esp + 0x30]
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x38]
            fxch st(5)
            fabs
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e88cb
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e88cb
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e8a9b
    L004e88cb:
            fld dword ptr [gShadow_points+0x1c]
            fld dword ptr [gShadow_points+0x18]
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [gShadow_points+0x20]
            fsub dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e8942
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e8942:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e89a3
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            jmp L004e89bd
    L004e89a3:
            fstp st(0)
            mov dword ptr [esp + 0x20], 0x3f800000
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e89bd:
            mov edx, dword ptr [ebx + 0x10]
            lea eax, [esp + 0x20]
            add edx, 0x2c
            lea ecx, [esp + 0x10]
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x50]
            mov edx, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fsub dword ptr [esp + 0x7c]
            fxch st(2)
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            mov ecx, dword ptr [eax + 0x5c]
            _emit 0xd9
        _emit 0x19
            mov edx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            mov ecx, dword ptr [eax + 0x5c]
            fstp dword ptr [ecx + 4]
            mov edx, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            mov ecx, dword ptr [eax + 0x5c]
            fstp dword ptr [ecx + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            mov edx, dword ptr [gShadow_clip_plane_count]
            mov eax, dword ptr [edx*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov ecx, dword ptr [eax + 0x5c]
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fchs
            fstp dword ptr [ecx + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
            jmp L004e8a9b
    L004e8a99:
            fstp st(0)
    L004e8a9b:
            fld dword ptr [esp + 0x34]
            fmul dword ptr [gConst_kangeroo_force_factor+0x174]
            fst dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x2c]
            fcom dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e8af3
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x40
            je L004e8dc0
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            je L004e8af5
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            jne L004e8dc0
            jmp L004e8af5
    L004e8af3:
            fstp st(0)
    L004e8af5:
            cmp dword ptr [gShadow_clip_plane_count], esi
            jge L004e8dc0
            mov edx, dword ptr [ebx + 0x10]
            lea eax, [esp + 0x3c]
            add edx, 0x2c
            push edx
            push offset gShadow_points+0x3c
            push eax
            call BrMatrix34ApplyP
            mov ecx, dword ptr [ebx + 0x10]
            add esp, 0xc
            add ecx, 0x2c
            lea edx, [esp + 0x74]
            push ecx
            push offset gShadow_points+0xc
            push edx
            call BrMatrix34ApplyP
            fld dword ptr [gShadow_points+0x14]
            fld dword ptr [gShadow_points+0x10]
            fsub dword ptr [gShadow_points+0x40]
            fxch st(1)
            fsub dword ptr [gShadow_points+0x44]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [gShadow_points+0xc]
            fsub dword ptr [gShadow_points+0x3c]
            fxch st(1)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld st(3)
            fld dword ptr [esp + 0x30]
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(5)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x38]
            fxch st(5)
            fabs
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            add esp, 0xc
            test ah, 1
            fstp st(0)
            je L004e8bf8
            fld dword ptr [esp + 0x14]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            je L004e8bf8
            fld dword ptr [esp + 0x18]
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x178]
            fnstsw ax
            test ah, 1
            jne L004e8dc0
    L004e8bf8:
            fld dword ptr [gShadow_points+0x40]
            fld dword ptr [gShadow_points+0x3c]
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [gShadow_points+0x44]
            fsub dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e8c6f
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
    L004e8c6f:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x180]
            fnstsw ax
            test ah, 0x41
            jne L004e8cd0
            fdivr qword ptr [gConst_kangeroo_force_factor+0x188]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            jmp L004e8cea
    L004e8cd0:
            fstp st(0)
            mov dword ptr [esp + 0x20], 0x3f800000
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e8cea:
            mov eax, dword ptr [ebx + 0x10]
            lea ecx, [esp + 0x20]
            add eax, 0x2c
            lea edx, [esp + 0x10]
            push eax
            push ecx
            push edx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x50]
            mov eax, dword ptr [gShadow_clip_plane_count]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fsub dword ptr [esp + 0x7c]
            fxch st(2)
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            _emit 0xd9
        _emit 0x1a
            mov eax, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x14]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            fstp dword ptr [edx + 4]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fld dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            mov edx, dword ptr [ecx + 0x5c]
            fstp dword ptr [edx + 8]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            mov eax, dword ptr [gShadow_clip_plane_count]
            mov ecx, dword ptr [eax*8 + gShadow_clip_planes]
            faddp st(2), st(0)
            mov edx, dword ptr [ecx + 0x5c]
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fchs
            fstp dword ptr [edx + 0xc]
            mov eax, dword ptr [gShadow_clip_plane_count]
            fstp dword ptr [eax*8 + gShadow_clip_planes+0x4]
            inc eax
            fstp st(0)
            mov dword ptr [gShadow_clip_plane_count], eax
    L004e8dc0:
            mov eax, dword ptr [gShadow_clip_plane_count]
            xor esi, esi
            cmp eax, ebp
            jle L004e8de8
            mov edi, offset gShadow_clip_planes
    L004e8dd0:
            mov eax, dword ptr [edi]
            push eax
            call BrClipPlaneEnable
            mov eax, dword ptr [gShadow_clip_plane_count]
            add esp, 4
            inc esi
            add edi, 8
            cmp esi, eax
            jl L004e8dd0
    L004e8de8:
            cmp dword ptr [gPed_676914], ebp
            jne L004e8e1e
            cmp dword ptr [ebx + 0x12e8], 3
            jl L004e8e1e
            lea edx, [esp + 0x80]
            mov ecx, ebx
            call GetPrecalculatedFacesUnderCar
            mov edx, eax
            cmp edx, ebp
            mov dword ptr [esp + 0x48], edx
            je L004e8e1e
            mov dword ptr [esp + 0x38], 0
            jmp L004e9009
    L004e8e1e:
            mov dword ptr [esp + 0x9c], 0x447a0000
            mov dword ptr [esp + 0xa0], 0x447a0000
            mov dword ptr [esp + 0xa4], 0x447a0000
            mov dword ptr [esp + 0xa8], 0xc47a0000
            mov dword ptr [esp + 0xac], 0xc47a0000
            mov dword ptr [esp + 0xb0], 0xc47a0000
            xor edi, edi
    L004e8e62:
            mov ecx, dword ptr [ebx + 0x10]
            lea edx, [edi + gShadow_points]
            add ecx, 0x2c
            lea esi, [esp + edi + 0xec]
            push ecx
            push edx
            push esi
            call BrMatrix34ApplyP
            _emit 0xd9
        _emit 0x06
            fcomp dword ptr [esp + 0xa8]
            add esp, 0xc
            fnstsw ax
            test ah, 1
            je L004e8e9b
            mov eax, dword ptr [esi]
            mov dword ptr [esp + 0x9c], eax
            jmp L004e8eb4
    L004e8e9b:
            _emit 0xd9
        _emit 0x06
            fcomp dword ptr [esp + 0xa8]
            fnstsw ax
            test ah, 0x41
            jne L004e8eb4
            mov ecx, dword ptr [esi]
            mov dword ptr [esp + 0xa8], ecx
    L004e8eb4:
            fld dword ptr [esp + edi + 0xf0]
            fcomp dword ptr [esp + 0xa0]
            fnstsw ax
            test ah, 1
            je L004e8ed9
            mov edx, dword ptr [esp + edi + 0xf0]
            mov dword ptr [esp + 0xa0], edx
            jmp L004e8efc
    L004e8ed9:
            fld dword ptr [esp + edi + 0xf0]
            fcomp dword ptr [esp + 0xac]
            fnstsw ax
            test ah, 0x41
            jne L004e8efc
            mov eax, dword ptr [esp + edi + 0xf0]
            mov dword ptr [esp + 0xac], eax
    L004e8efc:
            fld dword ptr [esp + edi + 0xf4]
            fcomp dword ptr [esp + 0xa4]
            fnstsw ax
            test ah, 1
            je L004e8f21
            mov ecx, dword ptr [esp + edi + 0xf4]
            mov dword ptr [esp + 0xa4], ecx
            jmp L004e8f44
    L004e8f21:
            fld dword ptr [esp + edi + 0xf4]
            fcomp dword ptr [esp + 0xb0]
            fnstsw ax
            test ah, 0x41
            jne L004e8f44
            mov edx, dword ptr [esp + edi + 0xf4]
            mov dword ptr [esp + 0xb0], edx
    L004e8f44:
            add edi, 0xc
            cmp edi, 0x60
            jl L004e8e62
            fld dword ptr [esp + 0xa0]
            fsub dword ptr [gConst_kangeroo_force_factor+0x19c]
            push ebp
            push 0x64
            lea edx, [esp + 0xb6c]
            lea ecx, [esp + 0xa0]
            mov dword ptr [esp + 0xa0], 0x655e00
            fstp dword ptr [esp + 0xa8]
            call FindFacesInBox
            fld dword ptr [esp + 0xac]
            fsub dword ptr [esp + 0xa0]
            fld dword ptr [gConst_kangeroo_force_factor+0x170]
            fld dword ptr [gConst_kangeroo_force_factor+0x170]
            fxch st(2)
            fst dword ptr [esp + 0x5c]
            mov ecx, dword ptr [ebx + 0x10]
            lea edx, [esp + 0xb64]
            fchs
            fxch st(1)
            fstp dword ptr [esp + 0xdc]
            fstp dword ptr [esp + 0xe0]
            mov dword ptr [esp + 0x80], edx
            mov dword ptr [esp + 0x38], 0x447a0000
            fstp dword ptr [esp + 0xe4]
            mov edx, dword ptr [ecx + 0x50]
            fld dword ptr [esp + 0xac]
            mov dword ptr [esp + 0x8c], edx
            mov edx, dword ptr [ecx + 0x54]
            mov dword ptr [esp + 0x90], edx
            mov ecx, dword ptr [ecx + 0x58]
            fstp dword ptr [esp + 0x90]
            mov edx, eax
            mov dword ptr [esp + 0x94], ecx
            mov dword ptr [esp + 0x48], edx
    L004e9009:
            cmp edx, ebp
            je L004e9633
            cmp edx, ebp
            mov dword ptr [esp + 0x1c], 0xc47a0000
            jle L004e9168
            mov eax, dword ptr [esp + 0x80]
            mov esi, edx
            mov edi, 0x1000
            lea ecx, [eax + 0xc]
    L004e9032:
            fld dword ptr [ecx + 0x2c]
            fcomp qword ptr [gConst_kangeroo_force_factor+0x1a0]
            fnstsw ax
            test ah, 1
            je L004e90f3
            mov eax, dword ptr [ecx - 0xc]
            cmp eax, ebp
            je L004e9056
            test dword ptr [eax + 0x20], edi
            jne L004e90f3
    L004e9056:
            fld dword ptr [ecx + 0x14]
            fadd dword ptr [ecx - 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0xc]
            fxch st(2)
            fadd dword ptr [ecx + 8]
            fxch st(1)
            fadd dword ptr [ecx - 8]
            fxch st(2)
            _emit 0xd8
        _emit 0x01
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x1a8]
            fxch st(2)
            fadd dword ptr [ecx + 0x10]
            fxch st(1)
            fadd dword ptr [ecx + 0x18]
            fld st(2)
            fcomp dword ptr [esp + 0x1c]
            fxch st(1)
            fst dword ptr [esp + 0x3c]
            fxch st(1)
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x1a8]
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x1a8]
            fnstsw ax
            test ah, 0x41
            jne L004e90eb
            mov eax, dword ptr [ebx + 0x10]
            fld st(2)
            fxch st(2)
            fsub dword ptr [eax + 0x50]
            fxch st(2)
            fsub dword ptr [eax + 0x54]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [ecx + 0x2c]
            fld st(3)
            fsub dword ptr [eax + 0x58]
            fxch st(2)
            fmulp st(5), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(3), st(0)
            fmul dword ptr [ecx + 0x30]
            faddp st(2), st(0)
            fxch st(1)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004e90ef
            fstp dword ptr [esp + 0x1c]
            jmp L004e9152
    L004e90eb:
            fstp st(0)
            fstp st(0)
    L004e90ef:
            fstp st(0)
            jmp L004e9152
    L004e90f3:
            fld dword ptr [ecx + 0x2c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            jne L004e9152
            mov eax, dword ptr [ecx - 0xc]
            cmp eax, ebp
            je L004e911b
            mov eax, dword ptr [eax + 4]
            cmp eax, ebp
            je L004e911b
            mov al, byte ptr [eax]
            cmp al, 0x21
            je L004e9152
            cmp al, 0x3e
            je L004e9152
    L004e911b:
            fld dword ptr [ecx - 4]
            fcomp dword ptr [ebx + 0x50]
            fnstsw ax
            test ah, 0x41
            je L004e9142
            fld dword ptr [ecx + 8]
            fcomp dword ptr [ebx + 0x50]
            fnstsw ax
            test ah, 0x41
            je L004e9142
            fld dword ptr [ecx + 0x14]
            fcomp dword ptr [ebx + 0x50]
            fnstsw ax
            test ah, 0x41
            jne L004e915e
    L004e9142:
            fld dword ptr [ecx + 0x2c]
            fcomp qword ptr [gConst_kangeroo_force_factor+0x1b0]
            fnstsw ax
            test ah, 1
            je L004e915e
    L004e9152:
            fld dword ptr [ecx + 0x38]
            fsub dword ptr [gConst_kangeroo_force_factor+0x1ac]
            fstp dword ptr [ecx + 0x38]
    L004e915e:
            add ecx, 0x50
            dec esi
            jne L004e9032
    L004e9168:
            cmp edx, ebp
            mov dword ptr [esp + 0x60], ebp
            jle L004e9351
            mov ecx, dword ptr [esp + 0x80]
            lea edi, [esp + 0x3e8]
            lea ebp, [esp + 0x16c]
            lea esi, [ecx + 0x44]
    L004e918c:
            _emit 0xd9
        _emit 0x06
            fcomp dword ptr [gConst_kangeroo_force_factor+0x1b8]
            fnstsw ax
            test ah, 1
            jne L004e91aa
            _emit 0xd9
        _emit 0x06
            fsub dword ptr [gConst_kangeroo_force_factor+0x1bc]
            _emit 0xd9
        _emit 0x1e
            jmp L004e9339
    L004e91aa:
            fld dword ptr [esi - 0x3c]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 1
            je L004e91d8
            fld dword ptr [esi - 0x30]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 1
            je L004e91d8
            fld dword ptr [esi - 0x24]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 1
            jne L004e9339
    L004e91d8:
            mov eax, dword ptr [gFancy_shadow]
            test eax, eax
            jne L004e91ec
            mov edx, dword ptr [gShadow_material]
            mov dword ptr [ebp], edx
            jmp L004e923d
    L004e91ec:
            mov eax, dword ptr [esi - 0x44]
            mov dword ptr [ebp], eax
            mov eax, dword ptr [gShadow_workaround]
            test eax, eax
            je L004e9215
            fld dword ptr [gLighting_data+0xc]
            fmul qword ptr [gConst_kangeroo_force_factor+0x1c0]
            mov ecx, dword ptr [esi - 0x44]
            push 4
            fstp dword ptr [ecx + 0x10]
            mov edx, dword ptr [esi - 0x44]
            push edx
            jmp L004e9235
    L004e9215:
            mov ecx, dword ptr [esi - 0x44]
            test ecx, ecx
            je L004e923d
            mov eax, dword ptr [ecx + 0x40]
            test eax, eax
            je L004e923d
            mov eax, dword ptr [ecx + 0x20]
            test al, 1
            jne L004e923d
            or al, 5
            push 2
            mov dword ptr [ecx + 0x20], eax
            mov eax, dword ptr [esi - 0x44]
            push eax
    L004e9235:
            call BrMaterialUpdate
            add esp, 8
    L004e923d:
            mov ecx, dword ptr [esi - 0x40]
            add ebp, 0x28
            mov dword ptr [edi - 4], ecx
            mov edx, dword ptr [esi - 0x3c]
            mov dword ptr [edi], edx
            mov eax, dword ptr [esi - 0x38]
            mov dword ptr [edi + 4], eax
            mov ecx, dword ptr [esi - 0x34]
            mov dword ptr [edi + 0x24], ecx
            mov edx, dword ptr [esi - 0x30]
            mov dword ptr [edi + 0x28], edx
            mov eax, dword ptr [esi - 0x2c]
            mov dword ptr [edi + 0x2c], eax
            mov ecx, dword ptr [esi - 0x28]
            mov dword ptr [edi + 0x4c], ecx
            mov edx, dword ptr [esi - 0x24]
            mov dword ptr [edi + 0x50], edx
            mov eax, dword ptr [esi - 0x20]
            mov dword ptr [edi + 0x54], eax
            mov ecx, dword ptr [esi - 0x1c]
            fld dword ptr [esp + 0x38]
            mov edx, dword ptr [ecx]
            mov dword ptr [edi + 8], edx
            mov eax, dword ptr [esi - 0x1c]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [edi + 0xc], ecx
            mov edx, dword ptr [esi - 0x18]
            add edi, 0x78
            mov eax, dword ptr [edx]
            mov dword ptr [edi - 0x48], eax
            mov ecx, dword ptr [esi - 0x18]
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [edi - 0x44], edx
            mov eax, dword ptr [esi - 0x14]
            mov ecx, dword ptr [eax]
            mov dword ptr [edi - 0x20], ecx
            mov edx, dword ptr [esi - 0x14]
            mov ecx, dword ptr [esp + 0x70]
            mov eax, dword ptr [edx + 4]
            mov dword ptr [edi - 0x1c], eax
            lea eax, [ecx + ecx*2]
            mov word ptr [ebp - 0x30], ax
            lea edx, [eax + 1]
            add eax, 2
            mov word ptr [ebp - 0x2e], dx
            mov word ptr [ebp - 0x2c], ax
            fnstsw ax
            inc ecx
            test ah, 0x41
            mov dword ptr [esp + 0x70], ecx
            jne L004e9332
            lea eax, [esp + 0x158]
            lea ecx, [esp + 0x68]
            push eax
            lea edx, [esp + 0x150]
            push ecx
            lea eax, [esp + 0xe4]
            push edx
            push eax
            lea edx, [esp + 0x9c]
            lea ecx, [esi - 0x44]
            call CheckSingleFace
            fld dword ptr [esp + 0x68]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x190]
            fnstsw ax
            test ah, 1
            je L004e9332
            fld dword ptr [esp + 0x5c]
            fmul dword ptr [esp + 0x68]
            fcom dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 1
            je L004e9330
            fstp dword ptr [esp + 0x38]
            jmp L004e9332
    L004e9330:
            fstp st(0)
    L004e9332:
            cmp dword ptr [esp + 0x70], 0x10
            jge L004e9351
    L004e9339:
            mov eax, dword ptr [esp + 0x60]
            mov ecx, dword ptr [esp + 0x48]
            inc eax
            add esi, 0x50
            cmp eax, ecx
            mov dword ptr [esp + 0x60], eax
            jl L004e918c
    L004e9351:
            fld dword ptr [esp + 0x64]
            fsub dword ptr [esp + 0x6c]
            fsubr dword ptr [esp + 0x38]
            fst dword ptr [esp + 0x38]
            fcomp qword ptr [gConst_kangeroo_force_factor+0x1c8]
            fnstsw ax
            test ah, 0x41
            jne L004e9378
            mov dword ptr [esp + 0x38], 0x400ccccd
            jmp L004e9391
    L004e9378:
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x170]
            fnstsw ax
            test ah, 1
            je L004e9391
            mov dword ptr [esp + 0x38], 0
    L004e9391:
            mov eax, dword ptr [gFancy_shadow]
            test eax, eax
            je L004e9416
            fld dword ptr [esp + 0x38]
            fsubr qword ptr [gConst_kangeroo_force_factor+0x1c8]
            fmul qword ptr [gConst_kangeroo_force_factor+0x1d8]
            fmul qword ptr [gConst_kangeroo_force_factor+0x1e0]
            fsubr qword ptr [gConst_kangeroo_force_factor+0x1e8]
            call _ftol
            mov ecx, dword ptr [gSaved_table_count]
            mov dword ptr [gShadow_dim_amount], eax
            test ecx, ecx
            mov edi, 0
            jle L004e9416
            mov esi, offset gSaved_shade_tables
            mov ebp, 1
    L004e93d9:
            mov ecx, dword ptr [esi]
            push 0x7fff
            mov word ptr [ecx + 0x36], bp
            mov eax, dword ptr [gDepth_shade_table]
            movsx edx, word ptr [eax + 0x28]
            imul edx, dword ptr [gShadow_dim_amount]
            mov ecx, dword ptr [eax + 8]
            mov eax, dword ptr [esi]
            add edx, ecx
            mov dword ptr [eax + 8], edx
            mov ecx, dword ptr [esi]
            push ecx
            call BrTableUpdate
            mov eax, dword ptr [gSaved_table_count]
            add esp, 8
            inc edi
            add esi, 8
            cmp edi, eax
            jl L004e93d9
    L004e9416:
            fld dword ptr [esp + 0x38]
            fsubr qword ptr [gConst_kangeroo_force_factor+0x1c8]
            mov eax, dword ptr [gShadow_clip_plane_count]
            mov ebp, 0
            test eax, eax
            fmul qword ptr [gConst_kangeroo_force_factor+0x1f0]
            fmul qword ptr [gConst_kangeroo_force_factor+0x1e0]
            fsubr qword ptr [gConst_kangeroo_force_factor+0x1f8]
            jle L004e94b4
            fsubr qword ptr [gConst_kangeroo_force_factor+0x188]
            mov edi, offset gShadow_clip_planes
            fstp qword ptr [esp + 0x48]
    L004e944f:
            mov edx, dword ptr [edi]
            mov ecx, dword ptr [ebx + 0x10]
            add ecx, 0x50
            mov esi, dword ptr [edx + 0x5c]
            mov eax, dword ptr [esi + 0xc]
            mov edx, dword ptr [esi + 8]
            mov ebx, dword ptr [esi]
            push eax
            mov eax, dword ptr [esi + 4]
            push edx
            push eax
            push ebx
            call DistanceFromPlane
            mov ecx, dword ptr [edi]
            inc ebp
            fst dword ptr [esp + 0x68]
            fmul qword ptr [esp + 0x48]
            mov ebx, dword ptr [esp + 0x84]
            add edi, 8
            _emit 0xd8
        _emit 0x0e
            fstp dword ptr [ecx + 0x50]
            fld dword ptr [esp + 0x68]
            fmul qword ptr [esp + 0x48]
            mov edx, dword ptr [edi - 8]
            fmul dword ptr [esi + 4]
            fstp dword ptr [edx + 0x54]
            fld dword ptr [esp + 0x68]
            fmul qword ptr [esp + 0x48]
            mov eax, dword ptr [edi - 8]
            fmul dword ptr [esi + 8]
            fstp dword ptr [eax + 0x58]
            cmp ebp, dword ptr [gShadow_clip_plane_count]
            jl L004e944f
            jmp L004e94b6
    L004e94b4:
            fstp st(0)
    L004e94b6:
            mov esi, dword ptr [esp + 0x70]
            test esi, esi
            je L004e9633
            call DisableLights
            mov ecx, dword ptr [gDepth_buffer]
            mov edx, dword ptr [gRender_screen]
            mov eax, dword ptr [gCamera]
            push ecx
            mov ecx, dword ptr [gUniverse_actor]
            push edx
            push eax
            push ecx
            call BrZbsSceneRenderBegin
            add esp, 0x10
            call EnableLights
            push 2
            call BrSetScreenZOffset
            mov eax, dword ptr [gShadow_model]
            lea edx, [esp + 0x3e8]
            lea ecx, [esp + 0x168]
            add esp, 4
            mov dword ptr [eax + 8], edx
            mov edx, dword ptr [gShadow_model]
            mov dword ptr [edx + 0xc], ecx
            mov eax, dword ptr [gShadow_model]
            lea ecx, [esi + esi*2]
            mov word ptr [eax + 0x12], si
            mov edx, dword ptr [gShadow_model]
            mov word ptr [edx + 0x10], cx
            mov eax, dword ptr [gShadow_actor]
            mov byte ptr [eax + 0x20], 4
            mov ecx, dword ptr [gShadow_model]
            push ecx
            call BrModelAdd
            mov edx, dword ptr [gShadow_actor]
            add esp, 4
            push edx
            call BrZbsSceneRenderAdd
            mov eax, dword ptr [gShadow_model]
            add esp, 4
            push eax
            call BrModelRemove
            mov eax, dword ptr [ebx + 0x1594]
            add esp, 4
            test eax, eax
            je L004e95c8
            call GetOilSpillCount
            mov edi, eax
            xor esi, esi
            test edi, edi
            jle L004e95c8
    L004e957a:
            mov eax, dword ptr [ebx + 0x1594]
            mov edx, 1
            mov ecx, esi
            shl edx, cl
            test edx, eax
            je L004e95c3
            lea eax, [esp + 0xe8]
            lea edx, [esp + 0x88]
            push eax
            call GetOilSpillDetails
            mov eax, dword ptr [esp + 0x88]
            test eax, eax
            je L004e95c3
            mov ecx, esi
            call MungeIndexedOilsHeightAboveGround
            mov ecx, dword ptr [esp + 0x88]
            push ecx
            call BrZbsSceneRenderAdd
            add esp, 4
    L004e95c3:
            inc esi
            cmp esi, edi
            jl L004e957a
    L004e95c8:
            push 0
            call BrSetScreenZOffset
            add esp, 4
            call BrZbsSceneRenderEnd
            mov esi, dword ptr [esp + 0x70]
            test esi, esi
            jle L004e9633
            mov edi, dword ptr [esp + 0x70]
            xor esi, esi
    L004e95e5:
            mov eax, dword ptr [gFancy_shadow]
            test eax, eax
            je L004e962d
            mov edx, dword ptr [gShadow_model]
            mov eax, dword ptr [edx + 0xc]
            mov ecx, dword ptr [eax + esi + 8]
            mov eax, dword ptr [gShadow_workaround]
            test eax, eax
            je L004e960b
            call SmoothificateWorldMaterial
            jmp L004e962d
    L004e960b:
            test ecx, ecx
            je L004e962d
            mov eax, dword ptr [ecx + 0x40]
            test eax, eax
            je L004e962d
            mov eax, dword ptr [ecx + 0x20]
            test al, 1
            je L004e962d
            and al, 0xf8
            push 2
            push ecx
            mov dword ptr [ecx + 0x20], eax
            call BrMaterialUpdate
            add esp, 8
    L004e962d:
            add esi, 0x28
            dec edi
            jne L004e95e5
    L004e9633:
            mov ecx, dword ptr [gShadow_actor]
            xor esi, esi
            mov byte ptr [ecx + 0x20], 1
            mov eax, dword ptr [gShadow_clip_plane_count]
            test eax, eax
            jle L004e9665
            mov edi, offset gShadow_clip_planes
    L004e964d:
            mov edx, dword ptr [edi]
            push edx
            call BrClipPlaneDisable
            mov eax, dword ptr [gShadow_clip_plane_count]
            add esp, 4
            inc esi
            add edi, 8
            cmp esi, eax
            jl L004e964d
    L004e9665:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2a94
            ret 0x10
    }
#endif
}

void C2_HOOK_FASTCALL InitNearestCar(void) {

    gINT_0068d6f4 = -1;
    if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy) {
        gTarget_lock_car_2 = NULL;
    } else if (gPrevious_opponent_status == eOpponent_status_Wasted && gTarget_lock_car_1 != NULL) {
        if (PDGetTotalTime() - gTime_oppobar_target_wasted <= 2500) {
            gFLOAT_0074ab90 = 0.f;
        } else {
            gTarget_lock_car_2 = NULL;
            gTarget_lock_car_1 = NULL;
            if (gTarget_lock_enabled) {
                gTarget_lock_enabled = 0;
            }
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
        if (gGraf_data_index != 0) {
            PossibleUnlock(1);
            FRONTEND_Redraw(gBack_screen,
                -gBack_screen->origin_x,
                -gBack_screen->origin_y,
                gCurrent_race.map_image,
                gCurrent_race.map_image->origin_x,
                gCurrent_race.map_image->origin_y,
                640, 480);
        } else {
            DRPixelmapCopy(gBack_screen, gCurrent_race.map_image);
        }
    }
    DimRectangleClipped(gBack_screen,
        gMap_render_x_i - gCurrent_graf_data->map_render_x_marg,
        gMap_render_y_i - gCurrent_graf_data->map_render_y_marg,
        gMap_render_width_i + gMap_render_x_i + gCurrent_graf_data->map_render_x_marg,
        gMap_render_height_i + gMap_render_y_i + gCurrent_graf_data->map_render_y_marg,
        1);
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00496be0
void C2_NAKED C2_HOOK_FASTCALL MapStuffBeforeRender(void) {
#ifndef CARPOCALYPSE2_MATCHING
    InitNearestCar();
    if (gCurrent_race.map_image != NULL && gMap_view == 2) {
        StartMap();
        CopyMapToScreen();
    }
#else
    __asm {
            mov eax, dword ptr [gNet_mode]
            push esi
            xor esi, esi
            push edi
            cmp eax, esi
            mov dword ptr [gINT_0068d6f4], 0xffffffff
            je L00496c02
            mov eax, dword ptr [gCurrent_net_game]
            cmp dword ptr [eax + 0x74], 7
            je L00496c62
    L00496c02:
            cmp dword ptr [gPrevious_opponent_status], 8
            jne L00496c4e
            cmp dword ptr [gTarget_lock_car_1], esi
            je L00496c4e
            call PDGetTotalTime
            sub eax, dword ptr [gTime_oppobar_target_wasted]
            cmp eax, 0x9c4
            ja L00496c31
            mov dword ptr [gFLOAT_0074ab90], 0
            jmp L00496c68
    L00496c31:
            mov eax, dword ptr [gTarget_lock_enabled]
            mov dword ptr [gTarget_lock_car_2], esi
            cmp eax, esi
            mov dword ptr [gTarget_lock_car_1], esi
            je L00496c68
            mov dword ptr [gTarget_lock_enabled], esi
            jmp L00496c68
    L00496c4e:
            cmp dword ptr [gTarget_lock_enabled], esi
            je L00496c62
            mov dword ptr [gFLOAT_0074ab90], 0
            jmp L00496c68
    L00496c62:
            mov dword ptr [gTarget_lock_car_2], esi
    L00496c68:
            cmp dword ptr [gCurrent_race+0xf04], esi
            je L00496d74
            cmp dword ptr [gMap_view], 2
            jne L00496d74
            mov eax, dword ptr [gBack_screen]
            movsx ecx, word ptr [eax + 0x38]
            mov dword ptr [gBack_original_origin_x], ecx
            xor ecx, ecx
            movsx edx, word ptr [eax + 0x3a]
            mov dword ptr [gBack_original_origin_y], edx
            mov cx, word ptr [eax + 0x30]
            xor edx, edx
            mov dword ptr [gBack_original_base_x], ecx
            mov dx, word ptr [eax + 0x32]
            mov dword ptr [gBack_original_base_y], edx
            mov word ptr [eax + 0x38], si
            mov eax, dword ptr [gBack_screen]
            mov word ptr [eax + 0x3a], si
            mov ecx, dword ptr [gBack_screen]
            mov word ptr [ecx + 0x30], si
            mov edx, dword ptr [gBack_screen]
            mov word ptr [edx + 0x32], si
            mov edx, dword ptr [gCurrent_race+0xf04]
            cmp edx, esi
            je L00496d30
            cmp dword ptr [gGraf_data_index], esi
            je L00496d25
            mov ecx, 1
            call PossibleUnlock
            mov eax, dword ptr [gCurrent_race+0xf04]
            push 0x1e0
            push 0x280
            mov cx, word ptr [eax + 0x3a]
            mov dx, word ptr [eax + 0x38]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            push eax
            mov ax, word ptr [ecx + 0x3a]
            mov dx, word ptr [ecx + 0x38]
            neg ax
            push eax
            neg dx
            call FRONTEND_Redraw
            jmp L00496d30
    L00496d25:
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapCopy
    L00496d30:
            mov eax, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [gMap_render_height_i]
            mov edi, dword ptr [gMap_render_width_i]
            push 1
            mov ecx, dword ptr [eax + 0x3ec]
            mov esi, dword ptr [eax + 0x3e8]
            mov eax, dword ptr [gMap_render_y_i]
            add edx, eax
            sub eax, ecx
            add edx, ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            mov edx, dword ptr [gMap_render_x_i]
            add edi, edx
            sub edx, esi
            add edi, esi
            push edi
            push eax
            call DimRectangleClipped
    L00496d74:
            pop edi
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
    }
#endif
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004e5680
void C2_NAKED C2_HOOK_FASTCALL DoARenderPass(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, float pYon_factor, int pShadows, int pEffects, int pEffects_extra) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* camera_data = (br_camera*)pCamera->type_data;
    br_scalar original_yon = camera_data->yon_z;
    int count;
    int i;
    int j;
    tCar_spec* fox_car;
    br_pixelmap* sky_pixelmap;
    int bgnd_col;
    camera_data->yon_z *= pYon_factor;
    BrPixelmapFill(pDepth, 0xffffffff);
    if (!gAFE) {
        count = 0;
        if (gNet_mode != eNet_mode_none && (gCurrent_net_game->type == eNet_game_type_6 || gCurrent_net_game->type == eNet_game_type_foxy)) {
            BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
            for (i = 0; i < gNumber_of_net_players; i++) {
                tNet_game_player_info* net_player = &gNet_players[i];
                if ((net_player->field_0x80 || i == gIt_or_fox) && !IsCarCloaked(net_player->car)) {
                    fox_car = net_player->car;

                    if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                        br_actor** wheel_actors;

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
                            br_actor** wheel_actors;

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
    sky_pixelmap = (pColour->width == gBack_screen->width) ? gBack_screen : pColour;
    if (gProgram_state.current_depth_effect.sky_texture != NULL && (gLast_camera_special_volume == NULL || gLast_camera_special_volume->sky_col <= -1)) {
        BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
        DepthEffectSky(pColour, pDepth, pCamera, pMat34);
        BrZbsSceneRenderEnd();
    } else {
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
            BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
            DepthEffectSky(pColour, pDepth, pCamera, pMat34);
            BrZbsSceneRenderEnd();
        } else {
            if (sky_pixelmap->type != BR_PMT_INDEX_8) {
                bgnd_col = PaletteEntry16Bit(gRender_palette, bgnd_col);
                bgnd_col = (bgnd_col << 16) | bgnd_col;
            }
            BrPixelmapFill(sky_pixelmap, bgnd_col);
        }
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
    BrZbsSceneRenderAdd(gNon_track_actor);
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
#else
    __asm {
            sub esp, 0x14
            fld dword ptr [esp + 0x20]
            push ebx
            mov ebx, edx
            push ebp
            push esi
            mov eax, dword ptr [ebx + 0x5c]
            mov esi, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x10], ecx
            push edi
            fmul dword ptr [eax + 0xc]
            mov ecx, dword ptr [eax + 0xc]
            push -1
            push esi
            mov dword ptr [esp + 0x20], ebx
            mov dword ptr [esp + 0x28], eax
            mov dword ptr [esp + 0x24], ecx
            fstp dword ptr [eax + 0xc]
            call BrPixelmapFill
            mov eax, dword ptr [gAFE]
            xor edi, edi
            add esp, 8
            cmp eax, edi
            jne L004e586b
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [esp + 0x30], edi
            cmp eax, edi
            je L004e586b
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x74]
            cmp eax, 6
            je L004e56ef
            cmp eax, 7
            jne L004e586b
    L004e56ef:
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [gUniverse_actor]
            push esi
            push eax
            push ebx
            push ecx
            call BrZbsSceneRenderBegin
            mov eax, dword ptr [gNumber_of_net_players]
            add esp, 0x10
            cmp eax, edi
            jle L004e5780
            mov esi, offset gNet_players+0xc8
    L004e5713:
            mov eax, dword ptr [esi - 0x48]
            test eax, eax
            jne L004e5722
            cmp edi, dword ptr [gIt_or_fox]
            jne L004e576c
    L004e5722:
            mov ecx, dword ptr [esi]
            call IsCarCloaked
            test eax, eax
            jne L004e576c
            mov ebp, dword ptr [esi]
            mov dword ptr [esp + 0x10], ebp
            mov edx, dword ptr [ebp + 0x10]
            cmp byte ptr [edx + 0x20], 1
            je L004e576c
            mov eax, dword ptr [esp + 0x30]
            lea ecx, [ebp + 0x12ec]
            inc eax
            mov edx, 6
            mov dword ptr [esp + 0x30], eax
    L004e5750:
            mov eax, dword ptr [ecx]
            test eax, eax
            je L004e575a
            mov byte ptr [eax + 0x20], 1
    L004e575a:
            add ecx, 4
            dec edx
            jne L004e5750
            mov eax, dword ptr [ebp + 0x10]
            push eax
            call BrZbsSceneRenderAdd
            add esp, 4
    L004e576c:
            mov eax, dword ptr [gNumber_of_net_players]
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L004e5713
            mov esi, dword ptr [esp + 0x2c]
    L004e5780:
            call BrZbsSceneRenderEnd
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L004e586b
            mov eax, dword ptr [gHud_tinted4]
            cmp eax, -1
            jne L004e57bb
            push 0
            push 0
            push 2
            push 3
            push 0x1e0
            push 0x280
            xor edx, edx
            xor ecx, ecx
            call CreateTintedPoly
            mov dword ptr [gHud_tinted4], eax
    L004e57bb:
            mov ecx, eax
            call TurnTintedPolyOn
            mov ecx, dword ptr [gHud_tinted4]
            call ProcessTintedPoly
            call RenderTintedPolys
            mov ecx, dword ptr [gHud_tinted4]
            call TurnTintedPolyOff
            mov ecx, dword ptr [esp + 0x28]
            mov edx, dword ptr [gUniverse_actor]
            push esi
            push ecx
            push ebx
            push edx
            call BrZbsSceneRenderBegin
            mov eax, dword ptr [gNumber_of_net_players]
            add esp, 0x10
            xor ebp, ebp
            test eax, eax
            jle L004e585b
            mov ebx, offset gNet_players+0xc8
    L004e5803:
            mov eax, dword ptr [ebx - 0x48]
            test eax, eax
            jne L004e5812
            cmp ebp, dword ptr [gIt_or_fox]
            jne L004e5847
    L004e5812:
            mov esi, dword ptr [ebx]
            mov dword ptr [esp + 0x10], esi
            mov eax, dword ptr [esi + 0x10]
            cmp byte ptr [eax + 0x20], 1
            je L004e5847
            add esi, 0x12ec
            mov edi, 6
    L004e582c:
            mov eax, dword ptr [esi]
            test eax, eax
            je L004e5841
            mov byte ptr [eax + 0x20], 0
            mov ecx, dword ptr [esi]
            push ecx
            call BrZbsSceneRenderAdd
            add esp, 4
    L004e5841:
            add esi, 4
            dec edi
            jne L004e582c
    L004e5847:
            mov eax, dword ptr [gNumber_of_net_players]
            inc ebp
            add ebx, 0xd4
            cmp ebp, eax
            jl L004e5803
            mov ebx, dword ptr [esp + 0x18]
    L004e585b:
            call BrZbsSceneRenderEnd
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edx + 0x10]
            mov byte ptr [eax + 0x20], 1
    L004e586b:
            mov edi, dword ptr [esp + 0x28]
            mov eax, dword ptr [gBack_screen]
            mov esi, edi
            mov cx, word ptr [edi + 0x34]
            cmp cx, word ptr [eax + 0x34]
            jne L004e5882
            mov esi, eax
    L004e5882:
            mov eax, dword ptr [gProgram_state+0x1bf8]
            test eax, eax
            mov eax, dword ptr [gDAT_0079ec44]
            je L004e589e
            test eax, eax
            je L004e590b
            mov ecx, dword ptr [eax + 0x9c]
            test ecx, ecx
            jl L004e590b
    L004e589e:
            mov edx, dword ptr [gProgram_state+0x1be0]
            cmp edx, 1
            je L004e58d2
            mov ecx, dword ptr [gSwap_depth_effect_type]
            cmp ecx, 1
            je L004e58d2
            test edx, edx
            je L004e58ce
            test ecx, ecx
            je L004e58ce
            test eax, eax
            je L004e58ce
            mov edx, dword ptr [eax + 0x9c]
            test edx, edx
            jge L004e58d7
            xor edx, edx
            jmp L004e58d7
    L004e58ce:
            xor edx, edx
            jmp L004e58d7
    L004e58d2:
            mov edx, 0xff
    L004e58d7:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004e5a35
            mov eax, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [eax + 0x74]
            cmp eax, 6
            je L004e58fa
            cmp eax, 7
            jne L004e5a35
    L004e58fa:
            mov ecx, dword ptr [gRender_palette]
            mov eax, dword ptr [ecx + 8]
            mov ecx, dword ptr [eax + edx*4]
            call SetSkyColour
    L004e590b:
            mov esi, dword ptr [esp + 0x2c]
            mov edx, dword ptr [gUniverse_actor]
            push esi
            push edi
            push ebx
            push edx
            call BrZbsSceneRenderBegin
            mov ebp, dword ptr [esp + 0x24]
            add esp, 0x10
            mov edx, esi
            mov ecx, edi
            push ebp
            push ebx
            call DepthEffectSky
            call BrZbsSceneRenderEnd
    L004e5935:
            mov edx, ebp
            mov ecx, ebx
            call DoSpecialCameraEffect
            mov eax, dword ptr [esp + 0x34]
            test eax, eax
            je L004e5958
            mov ecx, dword ptr [gUniverse_actor]
            push ebp
            push ebx
            mov edx, offset gProgram_state+0x1ba0
            call RenderShadows
    L004e5958:
            mov eax, dword ptr [gUniverse_actor]
            push esi
            push edi
            push ebx
            push eax
            call BrZbsSceneRenderBegin
            add esp, 0x10
            mov edi, 4
            mov esi, offset gOther_selfs+0xc
    L004e5973:
            cmp esi, offset gOther_selfs+0x4
            je L004e598f
            push edi
            call BrSetScreenZOffset
            mov ecx, dword ptr [esi]
            add esp, 4
            push ecx
            call BrZbsSceneRenderAdd
            add esp, 4
    L004e598f:
            sub esi, 4
            dec edi
            cmp esi, offset gCop_speed_factor+0xc
            jg L004e5973
            push 0
            call BrSetScreenZOffset
            mov ecx, dword ptr [gUniverse_actor]
            add esp, 4
            mov edx, offset gProgram_state+0x1ba0
            push ebp
            push ebx
            call ProcessTrack
            mov edx, dword ptr [gNon_track_actor]
            push edx
            call BrZbsSceneRenderAdd
            add esp, 4
            call RenderLimbs
            mov esi, dword ptr [esp + 0x2c]
            mov edi, dword ptr [esp + 0x28]
            push ebp
            push ebx
            mov edx, esi
            mov ecx, edi
            call RenderLollipops
            mov eax, dword ptr [esp + 0x38]
            test eax, eax
            je L004e5a1b
            mov eax, dword ptr [gFrame_period]
            mov edx, esi
            push eax
            push ebp
            push ebx
            mov ecx, edi
            call RenderSparks
            mov ecx, dword ptr [gFrame_period]
            mov edx, esi
            push ecx
            push ebp
            push ebx
            mov ecx, edi
            call RenderSmoke
            mov edx, dword ptr [gFrame_period]
            mov ecx, edi
            push edx
            push ebp
            push ebx
            mov edx, esi
            call RenderElectroBastardRays
    L004e5a1b:
            call BrZbsSceneRenderEnd
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x1c]
            pop edi
            pop esi
            pop ebp
            mov dword ptr [ecx + 0xc], eax
            pop ebx
            add esp, 0x14
            ret 0x18
    L004e5a35:
            cmp byte ptr [esi + 0x2c], 3
            je L004e5a54
            mov ecx, dword ptr [gRender_palette]
            call PaletteEntry16Bit
            and eax, 0xffff
            mov edx, eax
            mov ecx, edx
            shl ecx, 0x10
            or edx, ecx
    L004e5a54:
            push edx
            push esi
            call BrPixelmapFill
            mov ebp, dword ptr [esp + 0x1c]
            mov esi, dword ptr [esp + 0x34]
            add esp, 8
            jmp L004e5935
            nop
            nop
            nop
            nop
            nop
    }
#endif
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
            gRear_pixelmap->height = extra_render->material->colour_map->height;
            if (gRear_pixelmap->width != gRear_pixelmap->row_bytes) {
                gRear_pixelmap->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
            } else {
                gRear_pixelmap->flags |= BR_PMF_ROW_WHOLEPIXELS;
            }
            BrActorToActorMatrix34(&mat, extra_render->actor, gUniverse_actor);
            DoARenderPass(&mat, extra_render->actor, gRear_pixelmap, gDepth_buffer, 1.f, 0, 0, 0);
            DRPixelmapCopyMapBlack(extra_render->material->colour_map, gRear_pixelmap);
            BrMaterialUpdate(extra_render->material, BR_MATU_COLOURMAP);
        }
    }
    DoARenderPass(pCamera_to_world, pCamera, pScreen, pDepth, 1.f, !pMirror, !pMirror, !pMirror);
    gRendering_mirror = 0;
}

// FUNCTION: CARMA2_HW 0x00495ba0
void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev) {
    int i;
    int checkpoint;
    int checkpoints[10];
    int checkpoint_flags;

    C2_HOOK_ASSERT(gCurrent_race.check_point_count <= CARPOCALYPSE2_ASIZE(checkpoints));
    for (i = 0; i < gCurrent_race.check_point_count; i++) {
        checkpoints[i] = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
            checkpoint_flags = gNet_players[gThis_net_player_index].score2;
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (checkpoint_flags & 0x1) {
                    pCallback(pMap, i, pTime, 1);
                    checkpoints[i] = 1;
                }
                checkpoint_flags >>= 1;
            }
        } else if (gCurrent_net_game->type == eNet_game_type_5 || gCurrent_net_game->type == eNet_game_type_4) {
            if (gNet_players[gThis_net_player_index].score2 >= 0) {
                checkpoint = gNet_players[gThis_net_player_index].score2 % gCurrent_race.check_point_count;
                pCallback(pMap, checkpoint, pTime, 1);
                checkpoints[checkpoint] = 1;
            }
        }
    } else {
        pCallback(pMap, gCheckpoint - 1, pTime, 1);
        checkpoints[gCheckpoint - 1] = 1;
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
void C2_NAKED C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 p;

    BrMatrix34ApplyP(&p, &gCurrent_race.checkpoints[pCheckpoint].pos, &gCurrent_race.map_transformation);
    BrVector2Copy(&gCurrent_race.checkpoints[pCheckpoint].map_position, &p);
#else
    __asm {
            lea eax, [edx + edx*2]
            sub esp, 0xc
            shl eax, 3
            sub eax, edx
            push esi
            push offset gCurrent_race+0xed4
            lea edx, [esp + 8]
            lea eax, [eax + eax*2]
            lea esi, [eax*4 + gCurrent_race+0x21c]
            lea ecx, [esi + 0x100]
            push ecx
            push edx
            call BrMatrix34ApplyP
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            add esp, 0xc
            mov dword ptr [esi + 0x10c], eax
            mov dword ptr [esi + 0x110], ecx
            pop esi
            add esp, 0xc
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
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
    tRace_checkpoint_spec* checkpoint;

    checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
    BrMatrix34ApplyP(&pos_mapspace, &checkpoint->pos, &gCurrent_race.map_transformation);
    BrVector2Set(&p, pos_mapspace.v[0] - gOrigin_map.v[0] + gOrigin_headup_map.v[0], pos_mapspace.v[1] - gOrigin_map.v[1] + gOrigin_headup_map.v[1]);
    BrMatrix23TApplyV(&checkpoint->map_position, &p, &gMatrix23_0068c880);
    checkpoint->map_position.v[0] += (float)gINT_0074abd4;
    checkpoint->map_position.v[1] += (float)gINT_0074abd0;
}

// FUNCTION: CARMA2_HW 0x004967c0
void C2_NAKED C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0xc
            test dh, 1
            push ebx
            push ebp
            push esi
            mov esi, ecx
            je L004967d1
            xor ebx, ebx
            jmp L004967d5
    L004967d1:
            mov ebx, dword ptr [esp + 0x20]
    L004967d5:
            mov eax, dword ptr [esp + 0x1c]
            push offset gCurrent_race+0xed4
            lea ecx, [esp + 0x10]
            push eax
            push ecx
            call BrMatrix34ApplyP
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gDouble_0058a068+0x2e8]
            add esp, 0xc
            fnstsw ax
            test ah, 1
            jne L004968d8
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            mov dword ptr [esp + 0x20], edx
            fild dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L004968d8
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gDouble_0058a068+0x2e8]
            fnstsw ax
            test ah, 1
            jne L004968d8
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            mov dword ptr [esp + 0x20], eax
            fild dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L004968d8
            cmp dword ptr [gMap_view], 2
            jne L00496863
            mov ecx, 1
            call PossibleLock
    L00496863:
            mov ecx, dword ptr [gBack_screen]
            cmp byte ptr [ecx + 0x2c], 3
            je L004968a5
            mov ecx, dword ptr [gRender_palette]
            mov edx, ebx
            call PaletteEntry16Bit
            fld dword ptr [esp + 0x10]
            movsx ebp, word ptr [esi + 0x28]
            mov bx, ax
            call _ftol
            imul ebp, eax
            fld dword ptr [esp + 0xc]
            call _ftol
            lea edx, [ebp + eax*2]
            mov eax, dword ptr [esi + 8]
            mov word ptr [edx + eax], bx
            jmp L004968c5
    L004968a5:
            fld dword ptr [esp + 0x10]
            movsx ebp, word ptr [esi + 0x28]
            call _ftol
            imul ebp, eax
            fld dword ptr [esp + 0xc]
            add ebp, dword ptr [esi + 8]
            call _ftol
            mov byte ptr [ebp + eax], bl
    L004968c5:
            cmp dword ptr [gMap_view], 2
            jne L004968d8
            mov ecx, 1
            call PossibleUnlock
    L004968d8:
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 8
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3 *pPos, br_uint_32 pColour) {
#ifndef CARPOCALYPSE2_MATCHING
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
        colours[0] = 0;
        colours[1] = 0x84;
    } else {
        colours[0] = pColour;
        if ((int)pColour < 0xe0) {
            if ((int)(pColour & 0x7) < 4) {
                colours[1] = 0xff;
            } else {
                colours[1] = 0;
            }
        } else {
            if ((int)(pColour & 0xf) < 8) {
                colours[1] = 0xff;
            } else {
                colours[1] = 0;
            }
        }
    }
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        colours[0] = PaletteEntry16Bit(gRender_palette, colours[0]);
        colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        if (pColour == 0xffffffff) {
            colours[1] = Colour24BitTo16Bit(0xff00ff);
        }
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(colours); i++) {
        arrow_ptr = &gArrows[i][pArrow_index & 0x3][0];
        point_count = *arrow_ptr++;
        for (j = 0; j < point_count; j++) {
            colour = colours[i];
            if (pArrow_index & 0x8) {
                x = -*arrow_ptr++;
                y = -*arrow_ptr++;
            } else {
                x = *arrow_ptr++;
                y = *arrow_ptr++;
            }
            if (pArrow_index & 0x4) {
                temp = x;
                x = -y;
                y = temp;
            }
            BrPixelmapPixelSet(pScreen, (int)(pPos->v[0] + x), (int)(pPos->v[1] + y), colour);
        }
    }
#else
    __asm {
            sub esp, 0x24
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x38]
            mov ebx, edx
            cmp esi, -1
            push edi
            mov dword ptr [esp + 0x28], ebx
            mov dword ptr [esp + 0x24], ecx
            jne L00495a27
            xor ebp, ebp
            mov edi, 0x84
            mov dword ptr [esp + 0x2c], ebp
            jmp L00495a60
    L00495a27:
            mov ebp, esi
            cmp esi, 0xe0
            mov dword ptr [esp + 0x2c], ebp
            jge L00495a4b
            mov eax, esi
            xor ecx, ecx
            and al, 7
            cmp al, 4
            setge cl
            dec ecx
            and ecx, 0xff
            mov edi, ecx
            jmp L00495a60
    L00495a4b:
            mov edx, esi
            xor eax, eax
            and dl, 0xf
            cmp dl, 8
            setge al
            dec eax
            and eax, 0xff
            mov edi, eax
    L00495a60:
            mov ecx, dword ptr [gBack_screen]
            mov dword ptr [esp + 0x30], edi
            cmp byte ptr [ecx + 0x2c], 3
            je L00495ab4
            mov ecx, dword ptr [gRender_palette]
            mov edx, ebp
            call PaletteEntry16Bit
            mov ecx, dword ptr [gRender_palette]
            and eax, 0xffff
            mov edx, edi
            mov dword ptr [esp + 0x2c], eax
            call PaletteEntry16Bit
            and eax, 0xffff
            cmp esi, -1
            mov dword ptr [esp + 0x30], eax
            jne L00495ab4
            mov ecx, 0xff00ff
            call Colour24BitTo16Bit
            and eax, 0xffff
            mov dword ptr [esp + 0x30], eax
    L00495ab4:
            mov eax, ebx
            lea edx, [esp + 0x2c]
            and eax, 3
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x1c], 2
            lea eax, [eax + eax*2]
            lea ecx, [eax + eax*4]
            shl ecx, 4
            add ecx, offset gArrows
            mov dword ptr [esp + 0x18], ecx
    L00495adc:
            mov eax, dword ptr [ecx]
            lea esi, [ecx + 4]
            test eax, eax
            jle L00495b70
            mov ecx, dword ptr [esp + 0x14]
            mov edi, ebx
            and edi, 8
            and ebx, 4
            mov edx, dword ptr [ecx]
            mov ebp, eax
            mov dword ptr [esp + 0x20], edx
    L00495afd:
            mov eax, dword ptr [esi]
            test edi, edi
            je L00495b13
            mov ecx, dword ptr [esi + 4]
            add esi, 4
            neg eax
            mov dword ptr [esp + 0x10], eax
            neg ecx
            jmp L00495b1d
    L00495b13:
            mov ecx, dword ptr [esi + 4]
            add esi, 4
            mov dword ptr [esp + 0x10], eax
    L00495b1d:
            add esi, 4
            mov dword ptr [esp + 0x3c], ecx
            test ebx, ebx
            je L00495b32
            neg ecx
            mov dword ptr [esp + 0x3c], eax
            mov dword ptr [esp + 0x10], ecx
    L00495b32:
            fild dword ptr [esp + 0x3c]
            mov ecx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x20]
            push eax
            fadd dword ptr [ecx + 4]
            call _ftol
            fild dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x3c]
            push eax
            fadd dword ptr [edx]
            call _ftol
            push eax
            mov eax, dword ptr [esp + 0x30]
            push eax
            call BrPixelmapPixelSet
            add esp, 0x10
            dec ebp
            jne L00495afd
            mov ebx, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x18]
    L00495b70:
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x1c]
            add ecx, 0x3c0
            add edx, 4
            dec eax
            mov dword ptr [esp + 0x18], ecx
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x1c], eax
            jne L00495adc
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret 8
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00496270
void C2_NAKED C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x1c
            push ebx
            mov ebx, dword ptr [esp + 0x24]
            push ebp
            mov ebp, ecx
            mov ecx, dword ptr [gMap_time]
            mov eax, ebx
            sub eax, ecx
            push esi
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x34]
            push edi
            push offset gCurrent_race+0xed4
            lea ecx, [esp + 0x24]
            push eax
            mov esi, edx
            push ecx
            call BrMatrix34ApplyP
            add esp, 0xc
            mov dword ptr [esp + 0x28], 0
            test esi, esi
            mov edi, 6
            je L00496438
            mov eax, dword ptr [esi + 0xc]
            cmp eax, edi
            je L004962d1
            test esi, esi
            je L00496438
            cmp eax, 7
            jne L00496438
    L004962d1:
            mov eax, dword ptr [esi + 0x1d4]
            test eax, eax
            je L004962e7
            cmp dword ptr [gTarget_lock_car_1], esi
            jne L00496438
    L004962e7:
            mov edx, dword ptr [gCurrent_race+0x98]
            cmp dword ptr [edx + 0x210], 1
            jne L00496304
            mov eax, dword ptr [esi + 0x1c0]
            test eax, eax
            je L00496438
    L00496304:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L0049635f
            mov eax, dword ptr [gCurrent_net_game]
            cmp dword ptr [eax + 0x74], edi
            jne L00496328
            mov ecx, esi
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            je L0049635f
    L00496328:
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, dword ptr [ecx + 0x74]
            cmp edx, 7
            jne L0049634e
            mov eax, dword ptr [gIt_or_fox]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            cmp esi, dword ptr [ecx*4 + gNet_players+0xc8]
            je L0049635f
    L0049634e:
            cmp edx, edi
            je L00496438
            cmp edx, 7
            je L00496438
    L0049635f:
            mov edx, dword ptr [gHeadup_map_half_width]
            mov eax, dword ptr [gINT_0068d890]
            mov ecx, dword ptr [gHeadup_map_half_height]
            mov edi, dword ptr [gINT_0074ab88]
            add edx, eax
            mov eax, dword ptr [gINT_0068c858]
            add ecx, eax
            mov eax, dword ptr [gINT_0074ab8c]
            add edx, eax
            add ecx, edi
            mov dword ptr [esp + 0x38], edx
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x38]
            fild dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fsub dword ptr [esp + 0x24]
            fxch st(2)
            fchs
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            mov ecx, dword ptr [gTarget_lock_car_2]
            test ecx, ecx
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x38]
            je L00496401
            mov edx, dword ptr [gTarget_lock_enabled]
            test edx, edx
            jne L004963fd
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [gFLOAT_0074ab90]
            fnstsw ax
            test ah, 1
            jne L00496401
            test edx, edx
            je L00496438
    L004963fd:
            cmp ecx, esi
            jne L00496438
    L00496401:
            mov eax, dword ptr [esi + 0x1d4]
            test eax, eax
            jne L00496438
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [gVector2_0068d6d8], ecx
            mov ecx, dword ptr [esp + 0x38]
            mov dword ptr [gTarget_lock_car_2], esi
            mov dword ptr [gVector2_0068d6d8+0x4], edx
            mov dword ptr [gFLOAT_0068d6e0], eax
            mov dword ptr [gFLOAT_0074ab90], ecx
    L00496438:
            mov eax, dword ptr [gMini_map_visible]
            test eax, eax
            jne L0049644e
            cmp dword ptr [gMap_view], 2
            jne L004967b4
    L0049644e:
            mov edx, dword ptr [esp + 0x34]
            push offset gCurrent_race+0xed4
            push edx
            push offset gCar_in_map_space
            call BrMatrix34Mul
            mov eax, dword ptr [gCar_in_map_space+0x1c]
            mov ecx, dword ptr [gCar_in_map_space+0x18]
            add esp, 0xc
            push eax
            push ecx
            call FastScalarArcTan2
            fsubr dword ptr [gDouble_0058a068+0x320]
            fsub qword ptr [gDouble_0058a068+0x328]
            call _ftol
            mov dword ptr [esp + 0x38], eax
            fild dword ptr [esp + 0x38]
            fmul qword ptr [gDouble_0058a068+0x330]
            call _ftol
            cdq
            xor eax, edx
            mov edi, dword ptr [esp + 0x3c]
            sub eax, edx
            and eax, 0xf
            xor eax, edx
            sub eax, edx
            test esi, esi
            je L004964ba
            cmp dword ptr [esi + 0xc], 8
            jne L004964ba
            test bh, 1
            je L004964c9
    L004964ba:
            lea edx, [esp + 0x20]
            push edi
            push edx
            mov edx, eax
            mov ecx, ebp
            call DrawArrow
    L004964c9:
            cmp edi, 0xe0
            mov dword ptr [esp + 0x10], edi
            jge L004964df
            mov eax, edi
            xor ebx, ebx
            and al, 7
            cmp al, 4
            jmp L004964e9
    L004964df:
            mov ecx, edi
            xor ebx, ebx
            and cl, 0xf
            cmp cl, 8
    L004964e9:
            mov edx, dword ptr [gBack_screen]
            setge bl
            mov al, byte ptr [edx + 0x2c]
            dec ebx
            and ebx, 0xff
            cmp al, 3
            mov dword ptr [esp + 0x14], ebx
            je L00496530
            mov ecx, dword ptr [gRender_palette]
            mov edx, edi
            call PaletteEntry16Bit
            mov ecx, dword ptr [gRender_palette]
            and eax, 0xffff
            mov edx, ebx
            mov dword ptr [esp + 0x10], eax
            call PaletteEntry16Bit
            and eax, 0xffff
            mov dword ptr [esp + 0x14], eax
    L00496530:
            mov eax, dword ptr [esp + 0x30]
            and eax, 0x100
            mov edi, dword ptr [esp + eax*4 + 0x10]
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00496669
            mov eax, dword ptr [gCurrent_net_game]
            cmp dword ptr [eax + 0x74], 7
            jne L00496669
            mov eax, dword ptr [gIt_or_fox]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            cmp esi, dword ptr [ecx*4 + gNet_players+0xc8]
            jne L00496669
            fld dword ptr [esp + 0x24]
            fsub dword ptr [gDouble_0058a068+0x338]
            call _ftol
            fld dword ptr [esp + 0x20]
            fsub dword ptr [gDouble_0058a068+0x2fc]
            mov ebx, eax
            push edi
            push ebx
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub dword ptr [gDouble_0058a068+0x338]
            push eax
            push ebx
            call _ftol
            push eax
            push ebp
            call BrPixelmapLine
            fld dword ptr [esp + 0x3c]
            fsub dword ptr [gDouble_0058a068+0x2fc]
            add esp, 0x18
            call _ftol
            fld dword ptr [esp + 0x20]
            fsub dword ptr [gDouble_0058a068+0x2fc]
            mov ebx, eax
            push edi
            push ebx
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub dword ptr [gDouble_0058a068+0x338]
            push eax
            push ebx
            call _ftol
            push eax
            push ebp
            call BrPixelmapLine
            fld dword ptr [esp + 0x38]
            fsub dword ptr [gDouble_0058a068+0x338]
            add esp, 0x18
            call _ftol
            fld dword ptr [esp + 0x24]
            fsub dword ptr [gDouble_0058a068+0x2fc]
            mov ebx, eax
            push edi
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub dword ptr [gDouble_0058a068+0x338]
            push eax
            push ebx
            call _ftol
            push eax
            push ebx
            push ebp
            call BrPixelmapLine
            fld dword ptr [esp + 0x38]
            fsub dword ptr [gDouble_0058a068+0x2fc]
            add esp, 0x18
            call _ftol
            fld dword ptr [esp + 0x24]
            fsub dword ptr [gDouble_0058a068+0x2fc]
            mov ebx, eax
            push edi
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub dword ptr [gDouble_0058a068+0x338]
            push eax
            push ebx
            call _ftol
            push eax
            push ebx
            push ebp
            call BrPixelmapLine
            add esp, 0x18
    L00496669:
            mov ecx, dword ptr [esp + 0x1c]
            cmp ecx, 0x1f4
            ja L004967b4
            test esi, esi
            je L004967b4
            cmp dword ptr [esi + 0xc], 8
            jne L004967b4
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            mov ecx, 0x88b8
            fld dword ptr [esp + 0x24]
            lea edx, [eax + eax*4]
            mov eax, 0x10624dd3
            shl edx, 1
            sub ecx, edx
            mul ecx
            fsub qword ptr [gDouble_0058a068+0x340]
            mov esi, edx
            shr esi, 5
            call _ftol
            fld dword ptr [esp + 0x20]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            mov ebx, eax
            push edi
            sub ebx, esi
            push ebx
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub qword ptr [gDouble_0058a068+0x340]
            add eax, esi
            push eax
            push ebx
            call _ftol
            sub eax, esi
            push eax
            push ebp
            call BrPixelmapLine
            fld dword ptr [esp + 0x3c]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            add esp, 0x18
            call _ftol
            fld dword ptr [esp + 0x20]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            lea ebx, [eax + esi]
            push edi
            push ebx
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub qword ptr [gDouble_0058a068+0x340]
            add eax, esi
            push eax
            push ebx
            call _ftol
            sub eax, esi
            push eax
            push ebp
            call BrPixelmapLine
            fld dword ptr [esp + 0x38]
            fsub qword ptr [gDouble_0058a068+0x340]
            add esp, 0x18
            call _ftol
            fld dword ptr [esp + 0x24]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            mov ebx, eax
            push edi
            sub ebx, esi
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub qword ptr [gDouble_0058a068+0x340]
            add eax, esi
            push eax
            push ebx
            call _ftol
            sub eax, esi
            push eax
            push ebx
            push ebp
            call BrPixelmapLine
            fld dword ptr [esp + 0x38]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            add esp, 0x18
            call _ftol
            fld dword ptr [esp + 0x24]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            lea ebx, [eax + esi]
            push edi
            call _ftol
            fld dword ptr [esp + 0x28]
            fsub qword ptr [gDouble_0058a068+0x340]
            add eax, esi
            push eax
            push ebx
            call _ftol
            sub eax, esi
            push eax
            push ebx
            push ebp
            call BrPixelmapLine
            add esp, 0x18
    L004967b4:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x10
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047d260
void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette) {
    br_uint_32 colour;

    (void)pPalette;
    colour = pColour;
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        colour = PaletteEntry16Bit(gRender_palette, colour);
    }
    BrPixelmapLine(pMap, pX1, pY1, pX2, pY2, colour);
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
void C2_NAKED C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap *pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pMap->type == BR_PMT_INDEX_8) {
        DR8BitFancyDrawLine(pMap, pX1, pY1, pX2, pY2, pColour, pTime_period);
    } else {
        BrPixelmapLine(pMap, pX1, pY1, pX2, pY2, PaletteEntry16Bit(gRender_palette, pColour));
    }
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov ebx, edx
            mov al, byte ptr [esi + 0x2c]
            mov dword ptr [esp + 0x10], esi
            cmp al, 3
            jne L0047d42f
            mov edi, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x20]
            mov ebp, dword ptr [esp + 0x1c]
            mov ecx, edi
            sub eax, ebx
            sub ecx, ebp
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x1c], ecx
            call PDGetTotalTime
            xor edx, edx
            mov ecx, 5
            div dword ptr [esp + 0x2c]
            xor edx, edx
            mov esi, dword ptr [esp + 0x28]
            div ecx
            mov eax, esi
            mov dword ptr [esp + 0x24], edx
            mov edx, dword ptr [gBack_screen]
            cmp byte ptr [edx + 0x2c], 3
            je L0047d320
            mov ecx, dword ptr [gRender_palette]
            mov edx, esi
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047d320:
            mov ecx, dword ptr [esp + 0x10]
            push eax
            mov eax, dword ptr [esp + 0x24]
            push edi
            push eax
            push ebp
            push ebx
            push ecx
            call BrPixelmapLine
            fild dword ptr [esp + 0x34]
            fild dword ptr [esp + 0x2c]
            add esp, 0x18
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x28]
            fcompp
            fnstsw ax
            test ah, 1
            je L0047d3d5
            mov edx, dword ptr [gBack_screen]
            mov eax, esi
            cmp byte ptr [edx + 0x2c], 3
            je L0047d375
            mov ecx, dword ptr [gRender_palette]
            mov edx, esi
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047d375:
            push eax
            lea eax, [edi - 1]
            mov ecx, dword ptr [esp + 0x14]
            push eax
            mov eax, dword ptr [esp + 0x28]
            push eax
            lea eax, [ebp - 1]
            push eax
            push ebx
            push ecx
            call BrPixelmapLine
            mov edx, dword ptr [gBack_screen]
            add esp, 0x18
            inc edi
            inc ebp
            mov cl, byte ptr [edx + 0x2c]
            mov eax, esi
            cmp cl, 3
            je L0047d3b5
            mov ecx, dword ptr [gRender_palette]
            mov edx, esi
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047d3b5:
            mov ecx, dword ptr [esp + 0x10]
            push eax
            mov eax, dword ptr [esp + 0x24]
            push edi
            push eax
            push ebp
            push ebx
            push ecx
            call BrPixelmapLine
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x14
    L0047d3d5:
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [edx*4 + gMini_map_glowing_line_animation_indices]
            mov edx, dword ptr [esp + 0x20]
            dec edx
            mov ecx, dword ptr [eax*4 + gMini_map_glowing_line_palettes]
            push ecx
            mov ecx, dword ptr [esp + 0x14]
            push esi
            push edi
            push edx
            push ebp
            lea edx, [ebx - 1]
            call DRPixelmapBlendedLine
            mov eax, dword ptr [esp + 0x24]
            mov ecx, dword ptr [eax*4 + gMini_map_glowing_line_animation_indices]
            mov eax, dword ptr [esp + 0x20]
            inc eax
            mov edx, dword ptr [ecx*4 + gMini_map_glowing_line_palettes]
            mov ecx, dword ptr [esp + 0x10]
            push edx
            push esi
            push edi
            push eax
            push ebp
            lea edx, [ebx + 1]
            call DRPixelmapBlendedLine
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x14
    L0047d42f:
            mov edx, dword ptr [esp + 0x28]
            mov ecx, dword ptr [gRender_palette]
            call PaletteEntry16Bit
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x20]
            and eax, 0xffff
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            push edx
            push eax
            push ebx
            push esi
            call BrPixelmapLine
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x14
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

// FUNCTION: CARMA2_HW 0x0047d0d0
void C2_NAKED C2_HOOK_FASTCALL DR16BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height) {
#ifndef CARPOCALYPSE2_MATCHING
    float base_x;
    float src_x0;
    float base_y;
    float height_half;
    float src_y0;
    int width_div_2;
    int height_div_2;
    int dy;
    int dx;
    tU16 *ptr_write;
    float src_f_x;
    float src_f_y;

    width_div_2 = pSrc_width / 2;
    height_div_2 = pSrc_height / 2;
    src_x0 = (float)pSrc_x;
    src_y0 = (float)pSrc_y;
    height_half = (float)pSrc_height * 0.5;
    base_x = -(pMat->m[0][0] * (float)width_div_2);
    base_y = -(pMat->m[1][0] * (float)width_div_2);
    for (dy = -height_div_2; dy < height_div_2; dy++) {
        float dyf = (float)dy;
        src_f_x = base_x - pMat->m[0][1] * dyf + src_x0;
        ptr_write = (tU16*)((tU8*)pDest->pixels + (pDest_y + dy + height_div_2) * pDest->row_bytes) + pDest_x;
        src_f_y = pMat->m[1][1] * dyf - base_y + height_half + src_y0;
        for (dx = -width_div_2; dx < width_div_2; dx++, ptr_write++) {
            *ptr_write = *((tU16*)((tU8*)pSrc->pixels + ((int)src_f_y * pSrc->row_bytes)) + width_div_2 + (int)src_f_x);
            src_f_x += pMat->m[0][0];
            src_f_y -= pMat->m[1][0];
        }
    }
#else
    __asm {
            sub esp, 0x1c
            movsx eax, word ptr [esp + 0x34]
            mov dword ptr [esp], edx
            push ebx
            cdq
            sub eax, edx
            push edi
            mov ebx, eax
            mov edi, ecx
            movsx eax, word ptr [esp + 0x40]
            movsx ecx, word ptr [esp + 0x34]
            mov dword ptr [esp + 0x40], eax
            mov dword ptr [esp + 0x3c], ecx
            fild dword ptr [esp + 0x3c]
            cdq
            fstp dword ptr [esp + 0x10]
            sub eax, edx
            movsx edx, word ptr [esp + 0x38]
            mov dword ptr [esp + 0x3c], edx
            fild dword ptr [esp + 0x3c]
            sar ebx, 1
            mov dword ptr [esp + 0xc], ebx
            fstp dword ptr [esp + 0x1c]
            fild dword ptr [esp + 0x40]
            sar eax, 1
            mov ecx, eax
            mov dword ptr [esp + 0x20], eax
            fmul qword ptr [gDouble_0058a068+0x180]
            neg ecx
            cmp ecx, eax
            mov dword ptr [esp + 0x3c], ecx
            fstp dword ptr [esp + 0x18]
            fild dword ptr [esp + 0xc]
            _emit 0xd9
        _emit 0x07
            fmul st(0), st(1)
            fchs
            fstp dword ptr [esp + 0xc]
            fmul dword ptr [edi + 8]
            fchs
            fstp dword ptr [esp + 0x14]
            jge L0047d24e
            movsx edx, word ptr [esp + 0x28]
            shl edx, 1
            add ecx, eax
            mov dword ptr [esp + 0x38], edx
            movsx eax, word ptr [esp + 0x2c]
            mov edx, ebx
            push esi
            neg edx
            add ecx, eax
            push ebp
            mov ebp, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x3c], ecx
    L0047d17a:
            mov edx, dword ptr [esp + 0x10]
            mov eax, ecx
            fild dword ptr [esp + 0x44]
            movsx edx, word ptr [edx + 0x28]
            fst dword ptr [esp + 0x48]
            imul eax, edx
            fmul dword ptr [edi + 4]
            fsubr dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x48]
            fmul dword ptr [edi + 0xc]
            shl eax, 1
            cdq
            fsub dword ptr [esp + 0x1c]
            sub eax, edx
            mov esi, eax
            mov eax, dword ptr [esp + 0x40]
            sar esi, 1
            fadd dword ptr [esp + 0x20]
            add esi, eax
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [eax + 8]
            fadd dword ptr [esp + 0x24]
            add esi, edx
            mov edx, dword ptr [esp + 0x30]
            cmp edx, ebx
            jge L0047d256
            mov eax, ebx
            sub eax, edx
            mov dword ptr [esp + 0x48], eax
    L0047d1d9:
            fld st(0)
            call _ftol
            movsx ecx, word ptr [ebp + 0x28]
            fld st(1)
            imul eax, ecx
            cdq
            sub eax, edx
            mov ebp, eax
            sar ebp, 1
            add ebp, ebx
            call _ftol
            add ebp, eax
            add esi, 2
            mov dword ptr [esp + 0x34], ebp
            mov ebp, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x34]
            mov edx, dword ptr [ebp + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x07
            fxch st(1)
            fsub dword ptr [edi + 8]
            mov cx, word ptr [edx + eax*2]
            lea eax, [edx + eax*2]
            mov eax, dword ptr [esp + 0x48]
            mov word ptr [esi - 2], cx
            dec eax
            mov dword ptr [esp + 0x48], eax
            jne L0047d1d9
            mov ecx, dword ptr [esp + 0x3c]
            fstp st(0)
            fstp st(0)
    L0047d232:
            mov eax, dword ptr [esp + 0x44]
            mov edx, dword ptr [esp + 0x28]
            inc eax
            inc ecx
            cmp eax, edx
            mov dword ptr [esp + 0x44], eax
            mov dword ptr [esp + 0x3c], ecx
            jl L0047d17a
            pop ebp
            pop esi
    L0047d24e:
            pop edi
            pop ebx
            add esp, 0x1c
            ret 0x20
    L0047d256:
            fstp st(0)
            fstp st(0)
            jmp L0047d232
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047cd40
void C2_NAKED C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans) {
#ifndef CARPOCALYPSE2_MATCHING
    int width_div_2;
    int height_div_2;
    int width_near1;
    int width_near2;
    int width_near3;
    int height_near1;
    int height_near2;
    int height_near3;
    int src_y;
    int src_x;
    int dx_bit0;
    int dy_bit1;
    tU8 src_pix;
    float src_f_x;
    float src_f_y;
    tU8* ptr_write;
    int dy;
    int dx;

    if (pDest->type == BR_PMT_INDEX_8) {
        int width_full = pSrc_width;
        int height_full = pSrc_height;
        float height_half;
        float f_src_x;
        float f_src_y;
        float xbase;
        float ybase;

        width_div_2 = width_full / 2;
        height_div_2 = height_full / 2;
        width_near1 = width_div_2 - width_full / 24;
        width_near2 = width_div_2 - width_full / 12;
        width_near3 = width_div_2 - width_full * 3 / 24;
        height_near1 = height_div_2 - height_full / 24;
        height_near2 = height_div_2 - height_full / 12;
        height_near3 = height_div_2 - height_full * 3 / 24;
        f_src_x = (float)pSrc_x;
        f_src_y = (float)pSrc_y;
        height_half = (float)height_full * 0.5f;
        xbase = -(pMat->m[0][0] * (float)width_div_2);
        ybase = -(pMat->m[1][0] * (float)width_div_2);

        for (dy = -height_div_2; dy < height_div_2; dy++) {
            ptr_write = (tU8*)pDest->pixels + (pDest_y + dy + height_div_2) * pDest->row_bytes + pDest_x;
            src_f_x = xbase - pMat->m[0][1] * (float)dy + f_src_x;
            src_f_y = pMat->m[1][1] * (float)dy - ybase + height_half + f_src_y;
            for (dx = -width_div_2; dx < width_div_2; dx++, ptr_write++) {
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
                        if (-width_near1 <= dx_bit0 && dx_bit0 <= width_near1
                                && -height_near1 <= dy_bit1 && dy_bit1 <= height_near1) {
                            if (-width_near2 <= dx_bit0 && dx_bit0 <= width_near2
                                    && -height_near2 <= dy_bit1 && dy_bit1 <= height_near2) {
                                if (-width_near3 <= dx_bit0 && dx_bit0 <= width_near3
                                        && -height_near3 <= dy_bit1 && dy_bit1 <= height_near3) {
                                    *ptr_write = src_pix;
                                } else {
                                    *ptr_write = ((tU8*)gPalette_0074a5fc->pixels)[*ptr_write + 256 * src_pix];
                                }
                            } else {
                                *ptr_write = ((tU8*)gPalette_0074a600->pixels)[*ptr_write + 256 * src_pix];
                            }
                        } else {
                            *ptr_write = ((tU8*)gPalette_0074a604->pixels)[*ptr_write + 256 * src_pix];
                        }
                    }
                }
            }
        }
    } else {
        DR16BitPixelmapRotatedAndFeatheredCopy(pMat, pDest, pDest_x, pDest_y, pSrc, pSrc_x, pSrc_y, pSrc_width, pSrc_height);
    }
#else
    __asm {
            sub esp, 0x3c
            mov al, byte ptr [edx + 0x2c]
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            cmp al, 3
            push edi
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x28], ebp
            jne L0047d087
            movsx esi, word ptr [esp + 0x64]
            movsx edi, word ptr [esp + 0x68]
            mov eax, esi
            mov dword ptr [esp + 0x2c], edi
            cdq
            sub eax, edx
            mov ecx, eax
            mov eax, edi
            cdq
            sub eax, edx
            mov ebx, eax
            mov eax, 0x2aaaaaab
            imul esi
            sar edx, 2
            mov eax, edx
            shr eax, 0x1f
            sar ecx, 1
            add edx, eax
            mov eax, ecx
            sub eax, edx
            mov dword ptr [esp + 0x64], ecx
            mov dword ptr [esp + 0x14], eax
            mov eax, 0x2aaaaaab
            imul esi
            sar edx, 1
            mov eax, edx
            lea esi, [esi + esi*2]
            shr eax, 0x1f
            add edx, eax
            mov eax, ecx
            sub eax, edx
            mov dword ptr [esp + 0x1c], eax
            mov eax, 0x2aaaaaab
            imul esi
            sar edx, 2
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, 0x2aaaaaab
            sub ecx, edx
            imul edi
            sar edx, 2
            mov dword ptr [esp + 0x24], ecx
            mov ecx, edx
            sar ebx, 1
            shr ecx, 0x1f
            add edx, ecx
            mov eax, ebx
            sub eax, edx
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 0x18], eax
            mov eax, 0x2aaaaaab
            imul edi
            sar edx, 1
            mov eax, edx
            lea edi, [edi + edi*2]
            shr eax, 0x1f
            add edx, eax
            mov eax, ebx
            sub eax, edx
            mov dword ptr [esp + 0x20], eax
            mov eax, 0x2aaaaaab
            imul edi
            sar edx, 2
            mov ecx, edx
            mov eax, ebx
            shr ecx, 0x1f
            add edx, ecx
            sub eax, edx
            movsx edx, word ptr [esp + 0x5c]
            mov dword ptr [esp + 0x68], edx
            mov dword ptr [esp + 0x48], eax
            fild dword ptr [esp + 0x68]
            movsx eax, word ptr [esp + 0x60]
            fstp dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x68], eax
            fild dword ptr [esp + 0x68]
            fstp dword ptr [esp + 0x44]
            fild dword ptr [esp + 0x2c]
            fmul qword ptr [gDouble_0058a068+0x180]
            fstp dword ptr [esp + 0x3c]
            fild dword ptr [esp + 0x64]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            mov ecx, ebx
            fmul st(0), st(1)
            neg ecx
            cmp ecx, ebx
            mov dword ptr [esp + 0x68], ecx
            fchs
            fstp dword ptr [esp + 0x38]
            fmul dword ptr [ebp + 8]
            fchs
            fstp dword ptr [esp + 0x34]
            jge L0047d0b6
            movsx eax, word ptr [esp + 0x54]
            mov edx, dword ptr [esp + 0x64]
            lea edi, [ebx + ecx]
            neg edx
            add edi, eax
            mov dword ptr [esp + 0x2c], edx
            mov dword ptr [esp + 0x5c], edi
            jmp L0047ce96
    L0047ce8e:
            mov edi, dword ptr [esp + 0x5c]
            mov edx, dword ptr [esp + 0x2c]
    L0047ce96:
            mov esi, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x60], edx
            fild dword ptr [esp + 0x68]
            movsx eax, word ptr [esi + 0x28]
            fld st(0)
            fmul dword ptr [ebp + 0xc]
            imul edi, eax
            fxch st(1)
            fmul dword ptr [ebp + 4]
            fxch st(1)
            fsub dword ptr [esp + 0x34]
            fxch st(1)
            fsubr dword ptr [esp + 0x38]
            fxch st(1)
            fadd dword ptr [esp + 0x3c]
            fxch st(1)
            fadd dword ptr [esp + 0x40]
            fxch st(1)
            fadd dword ptr [esp + 0x44]
            movsx eax, word ptr [esp + 0x50]
            add edi, eax
            mov eax, dword ptr [esi + 8]
            add edi, eax
            mov eax, dword ptr [esp + 0x64]
            cmp edx, eax
            jge L0047d0c0
            jmp L0047ceef
    L0047ceeb:
            mov ebp, dword ptr [esp + 0x28]
    L0047ceef:
            fld st(0)
            call _ftol
            mov ebx, dword ptr [esp + 0x58]
            mov esi, eax
            mov eax, dword ptr [esp + 0x64]
            movsx ecx, word ptr [ebx + 0x28]
            fld st(1)
            imul esi, ecx
            add esi, eax
            call _ftol
            mov ecx, dword ptr [ebx + 8]
            add esi, eax
            mov eax, dword ptr [esp + 0x6c]
            add esi, ecx
            fxch st(1)
            _emit 0xd8
        _emit 0x45
        _emit 0x00
            fxch st(1)
            fsub dword ptr [ebp + 8]
            test eax, eax
            je L0047cf46
            mov eax, dword ptr [gPalette_0074a600]
            xor edx, edx
            mov dl, byte ptr [esi]
            mov ecx, dword ptr [eax + 8]
            shl edx, 8
            add edx, ecx
            xor ecx, ecx
            mov cl, byte ptr [edi]
            mov dl, byte ptr [edx + ecx]
            jmp L0047d03f
    L0047cf46:
            mov bl, byte ptr [esi]
            mov esi, dword ptr [esp + 0x60]
            mov edx, ebx
            xor eax, eax
            mov al, byte ptr [edi]
            and edx, 0xff
            mov ecx, edx
            mov ebp, eax
            and eax, 1
            and ecx, 1
            sub ecx, eax
            mov eax, edx
            add ecx, esi
            mov esi, ebp
            shr esi, 1
            shr eax, 1
            and esi, 1
            and eax, 1
            sub eax, esi
            mov esi, dword ptr [esp + 0x68]
            add eax, esi
            mov esi, dword ptr [esp + 0x64]
            neg esi
            cmp ecx, esi
            jl L0047d041
            cmp ecx, dword ptr [esp + 0x64]
            jg L0047d041
            mov esi, dword ptr [esp + 0x10]
            neg esi
            cmp eax, esi
            jl L0047d041
            cmp eax, dword ptr [esp + 0x10]
            jg L0047d041
            mov esi, dword ptr [esp + 0x14]
            neg esi
            cmp ecx, esi
            jl L0047d02f
            cmp ecx, dword ptr [esp + 0x14]
            jg L0047d02f
            mov esi, dword ptr [esp + 0x18]
            neg esi
            cmp eax, esi
            jl L0047d02f
            cmp eax, dword ptr [esp + 0x18]
            jg L0047d02f
            mov esi, dword ptr [esp + 0x1c]
            neg esi
            cmp ecx, esi
            jl L0047d025
            cmp ecx, dword ptr [esp + 0x1c]
            jg L0047d025
            mov esi, dword ptr [esp + 0x20]
            neg esi
            cmp eax, esi
            jl L0047d025
            cmp eax, dword ptr [esp + 0x20]
            jg L0047d025
            mov esi, dword ptr [esp + 0x24]
            neg esi
            cmp ecx, esi
            jl L0047d010
            cmp ecx, dword ptr [esp + 0x24]
            jg L0047d010
            mov ecx, dword ptr [esp + 0x48]
            mov esi, ecx
            neg esi
            cmp eax, esi
            jl L0047d010
            cmp eax, ecx
            jg L0047d010
            mov byte ptr [edi], bl
            jmp L0047d041
    L0047d010:
            mov ecx, dword ptr [gPalette_0074a5fc]
            shl edx, 8
            mov eax, dword ptr [ecx + 8]
            add eax, edx
            mov cl, byte ptr [eax + ebp]
            mov byte ptr [edi], cl
            jmp L0047d041
    L0047d025:
            mov eax, dword ptr [gPalette_0074a600]
            shl edx, 8
            jmp L0047d037
    L0047d02f:
            mov eax, dword ptr [gPalette_0074a604]
            shl edx, 8
    L0047d037:
            mov ecx, dword ptr [eax + 8]
            add ecx, edx
            mov dl, byte ptr [ecx + ebp]
    L0047d03f:
            mov byte ptr [edi], dl
    L0047d041:
            mov eax, dword ptr [esp + 0x60]
            mov ecx, dword ptr [esp + 0x64]
            inc eax
            inc edi
            cmp eax, ecx
            mov dword ptr [esp + 0x60], eax
            jl L0047ceeb
            mov ecx, dword ptr [esp + 0x68]
            mov ebx, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x28]
            fstp st(0)
            fstp st(0)
    L0047d067:
            mov edx, dword ptr [esp + 0x5c]
            inc ecx
            inc edx
            cmp ecx, ebx
            mov dword ptr [esp + 0x68], ecx
            mov dword ptr [esp + 0x5c], edx
            jl L0047ce8e
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret 0x20
    L0047d087:
            mov eax, dword ptr [esp + 0x6c]
            mov ecx, dword ptr [esp + 0x68]
            push eax
            mov eax, dword ptr [esp + 0x68]
            push ecx
            mov ecx, dword ptr [esp + 0x68]
            push eax
            mov eax, dword ptr [esp + 0x68]
            push ecx
            mov ecx, dword ptr [esp + 0x68]
            push eax
            mov eax, dword ptr [esp + 0x68]
            push ecx
            mov ecx, dword ptr [esp + 0x68]
            push eax
            push ecx
            mov ecx, ebp
            call DR16BitPixelmapRotatedAndFeatheredCopy
    L0047d0b6:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret 0x20
    L0047d0c0:
            fstp st(0)
            fstp st(0)
            jmp L0047d067
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047d5b0
void C2_NAKED C2_HOOK_FASTCALL DRPixelmapCopyMapBlack(br_pixelmap* pDest, br_pixelmap* pSrc) {
#ifndef CARPOCALYPSE2_MATCHING
    if (pDest->type == BR_PMT_INDEX_8) {
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
    } else {
        BrPixelmapCopy(pDest, pSrc);
    }
#else
    __asm {
            mov al, byte ptr [ecx + 0x2c]
            sub esp, 0x14
            cmp al, 3
            push ebx
            push ebp
            push esi
            push edi
            mov ebp, edx
            jne L0047d682
            movsx ebx, word ptr [ecx + 0x28]
            movsx edx, word ptr [ebp + 0x28]
            mov esi, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x14], ebx
            xor ebx, ebx
            xor edi, edi
            mov bx, word ptr [ecx + 0x34]
            mov di, word ptr [ebp + 0x34]
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [ebp + 8]
            sub edx, edi
            sub ecx, ebx
            shr edi, 2
            mov dword ptr [esp + 0x20], ecx
            mov ecx, edi
            xor edi, edi
            mov dword ptr [esp + 0x18], edx
            cmp word ptr [ebp + 0x36], di
            mov dword ptr [esp + 0x1c], ecx
            jbe L0047d68c
    L0047d607:
            test ecx, ecx
            jle L0047d667
            mov dword ptr [esp + 0x14], ecx
    L0047d60f:
            mov cl, byte ptr [eax]
            mov dl, byte ptr [eax + 1]
            inc eax
            inc eax
            mov bl, byte ptr [eax]
            inc eax
            mov byte ptr [esp + 0x13], bl
            mov bl, byte ptr [eax]
            inc eax
            test cl, cl
            je L0047d628
            mov byte ptr [esi], cl
            jmp L0047d62b
    L0047d628:
            mov byte ptr [esi], 0xf0
    L0047d62b:
            inc esi
            test dl, dl
            je L0047d634
            mov byte ptr [esi], dl
            jmp L0047d637
    L0047d634:
            mov byte ptr [esi], 0xf0
    L0047d637:
            mov cl, byte ptr [esp + 0x13]
            inc esi
            test cl, cl
            je L0047d644
            mov byte ptr [esi], cl
            jmp L0047d647
    L0047d644:
            mov byte ptr [esi], 0xf0
    L0047d647:
            inc esi
            test bl, bl
            je L0047d650
            mov byte ptr [esi], bl
            jmp L0047d653
    L0047d650:
            mov byte ptr [esi], 0xf0
    L0047d653:
            mov ecx, dword ptr [esp + 0x14]
            inc esi
            dec ecx
            mov dword ptr [esp + 0x14], ecx
            jne L0047d60f
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x1c]
    L0047d667:
            mov ebx, dword ptr [esp + 0x20]
            add eax, edx
            add esi, ebx
            xor ebx, ebx
            mov bx, word ptr [ebp + 0x36]
            inc edi
            cmp edi, ebx
            jl L0047d607
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L0047d682:
            push ebp
            push ecx
            call BrPixelmapCopy
            add esp, 8
    L0047d68c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
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

// FUNCTION: CARMA2_HW 0x004e74d0
void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    int cat;
    int i;
    int car_count;
    tCar_spec* the_car;
    br_vector3 camera_to_car;
    br_scalar distance_factor;

    if (gShadow_level == eShadow_none) {
        return;
    }
    for (cat = eVehicle_self;; ++cat) {
        int max;
        if (gShadow_level == eShadow_everyone) {
            max = eVehicle_drone;
        } else {
            max = (gShadow_level == eShadow_us_and_opponents ? eVehicle_rozzer : eVehicle_self);
        }
        if (cat > max) {
            break;
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
            if (!gAction_replay_mode) {
                BrVector3Sub(&camera_to_car, (br_vector3*)gCamera_to_world.m[3], &the_car->car_master_actor->t.t.translate.t);
                distance_factor = BrVector3LengthSquared(&camera_to_car);
                if (distance_factor > SHADOW_MAX_RENDER_DISTANCE) {
                    continue;
                }
            }
            ProcessShadow(the_car, gUniverse_actor, &gProgram_state.track_spec, gCamera, &gCamera_to_world, distance_factor);
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
// FUNCTION: CARMA2_HW 0x0047d470
void C2_HOOK_FASTCALL DrawBevelledRectangle(br_pixelmap* pMap, int pLeft, int pTop, int pWidth, int pHeight, br_uint_32 pColour) {
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapRectangleFill(pMap, pLeft, pTop + 1, pWidth, pHeight - 2, pColour);
    BrPixelmapLine(pMap, pLeft, pTop, pLeft + pWidth - 1, pTop, pColour + 2);
    BrPixelmapLine(pMap, pLeft, pTop + pHeight - 1, pLeft + pWidth - 1, pTop + pHeight - 1, pColour - 2);
}

// FUNCTION: CARMA2_HW 0x0047d510
void C2_HOOK_FASTCALL RectangleFill_LeftRightLine(br_pixelmap* pMap, int pX, int pY, int pWidth, int pHeight, br_colour pColour) {
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapRectangleFill(pMap, pX + 1, pY, pWidth - 2, pHeight, pColour);
    BrPixelmapLine(pMap, pX, pY, pX, pY + pHeight - 1, pColour + 2);
    BrPixelmapLine(pMap, pX + pWidth - 1, pY, pX + pWidth - 1, pY + pHeight - 1, pColour - 2);
}

// FUNCTION: CARMA2_HW 0x004e4910
int C2_HOOK_FASTCALL AddToLollipopQueue(void* pItem, int pIndex) {
    if (pIndex < 0) {
        if (gNumber_of_lollipops < 100) {
            gLollipop_queue[gNumber_of_lollipops] = pItem;
            gNumber_of_lollipops++;
            return gNumber_of_lollipops - 1;
        }
        return -1;
    }
    gLollipop_queue[pIndex] = pItem;
    return pIndex;
}

// FUNCTION: CARMA2_HW 0x00518590
br_actor* C2_HOOK_FASTCALL CreateBoundsActor(br_actor* pParent, const br_bounds* pBounds) {
    br_actor* actor;

    actor = BrActorAllocate(1, NULL);
    actor->model = BrModelAllocate("Bounds", 3, 1);
    actor->model->faces->vertices[0] = 0;
    actor->model->faces->vertices[1] = 1;
    actor->model->faces->vertices[2] = 2;
    actor->model->vertices[0].p.v[0] = 0.f;
    actor->model->vertices[0].p.v[1] = 0.f;
    actor->model->vertices[0].p.v[2] = 1.f;
    actor->model->vertices[1].p.v[0] = 0.f;
    actor->model->vertices[1].p.v[1] = 1.f;
    actor->model->vertices[1].p.v[2] = 1.f;
    actor->model->vertices[2].p.v[0] = 1.f;
    actor->model->vertices[2].p.v[1] = 0.f;
    actor->model->vertices[2].p.v[2] = 1.f;
    BrModelAdd(actor->model);
    actor->model->bounds = *pBounds;
    actor->render_style = 6;
    BrActorAdd(pParent, actor);
    return actor;
}

// FUNCTION: CARMA2_HW 0x00518660
void C2_HOOK_FASTCALL DisposeActorAndModel(br_actor* pActor) {
    BrModelRemove(pActor->model);
    BrModelFree(pActor->model);
    BrActorRemove(pActor);
    BrActorFree(pActor);
}
