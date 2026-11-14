#include "displays.h"

#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "font.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "polyfont.h"
#include "powerups.h"
#include "smashing.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "c2_math.h"
#include "displays.h"

#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x00589d28
float gFLOAT_00589d28 = 6.9f;

#include "netgame.h"
#include "loading1.h"
#include "graphics1.h"
#include "graphics2.h"
#include "font.h"
#include "grafdata.h"
#include "depth.h"
#include "utility.h"
#include "loading3.h"
#include "packfile.h"
#include "c2_string.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_macros.h"

#define HELLO _Pragma("error")
// GLOBAL: CARMA2_HW 0x00686178
br_material* gAcc_poly_material;

// GLOBAL: CARMA2_HW 0x00686174
br_actor* gAcc_poly_actor;

// GLOBAL: CARMA2_HW 0x0068617c
br_model* gAcc_poly_model;

// GLOBAL: CARMA2_HW 0x00686180
int gRender_acc_poly_actor;

// GLOBAL: CARMA2_HW 0x005962f8
br_token_value gAccent_poly_prims[] = {
    {
        BRT_BLEND_B,
        { 1 },
    }, {
        BRT_OPACITY_X,
        { 0x4b0000 },
    }, {
        BR_NULL_TOKEN,
        { 0 },
    },
};

// GLOBAL: CARMA2_HW 0x0068c880
br_matrix23 gMatrix23_0068c880;

// GLOBAL: CARMA2_HW 0x0068c868
br_vector3 gOrigin_map;

// GLOBAL: CARMA2_HW 0x00659b40
const int gCar_map_colours[4] = {
    4, 0, 0x34, 0x84,
};

// GLOBAL: CARMA2_HW 0x0068d6f4
int gINT_0068d6f4;

// GLOBAL: CARMA2_HW 0x0074abc4
float gFLOAT_0074abc4;

// GLOBAL: CARMA2_HW 0x0068d8b8
int gINT_0068d8b8;

// GLOBAL: CARMA2_HW 0x0068d8b0
br_vector2 gVector2_0068d8b0;

// GLOBAL: CARMA2_HW 0x0068c874
int gINT_0068c874;

// GLOBAL: CARMA2_HW 0x0068c878
int gINT_0068c878;

// GLOBAL: CARMA2_HW 0x0068d898
br_vector2 gOrigin_headup_map;

// GLOBAL: CARMA2_HW 0x0067c500
tHeadup gHeadups[37] = { 0 };

// GLOBAL: CARMA2_HW 0x0067f874
tU32 gLast_earn_time = 0;

// GLOBAL: CARMA2_HW 0x0067f890
tQueued_headup gQueued_headups[4];

// GLOBAL: CARMA2_HW 0x0067f888
int gQueued_headup_count;

// GLOBAL: CARMA2_HW 0x0067fcf8
tU32 gLast_centre_headup;

// GLOBAL: CARMA2_HW 0x0079eaa8
int gCredits_won_headup;

// GLOBAL: CARMA2_HW 0x0079eaf4
int gPed_kill_count_headup;

// GLOBAL: CARMA2_HW 0x0079eaf8
int gCar_kill_count_headup;

// GLOBAL: CARMA2_HW 0x00589d78
float gFLOAT_00589d78 = 0.1f;

// GLOBAL: CARMA2_HW 0x00589d7c
float gFLOAT_00589d7c = 1.0f;

// GLOBAL: CARMA2_HW 0x00589d80
double gDOUBLE_00589d80 = 0.5;

// GLOBAL: CARMA2_HW 0x0079eaec
int gTimer_headup;

// GLOBAL: CARMA2_HW 0x0079eb40
int gTime_awarded_headup;

// GLOBAL: CARMA2_HW 0x0079eaf0
int gLaps_headup;

// GLOBAL: CARMA2_HW 0x0079eae8
int gNet_cash_headup;

// GLOBAL: CARMA2_HW 0x0079eaa4
int gNet_ped_headup;

// GLOBAL: CARMA2_HW 0x005913d4
int gLast_fancy_index = -1;

// GLOBAL: CARMA2_HW 0x005913d8
int gLast_credit_headup__displays = -1;

// GLOBAL: CARMA2_HW 0x005913dc
int gLast_time_credit_headup = -1;

// GLOBAL: CARMA2_HW 0x0067fd34
tU32 gLast_fancy_time;

// GLOBAL: CARMA2_HW 0x0067fcc4
tU32 gLast_time_earn_time;

// GLOBAL: CARMA2_HW 0x0067f880
int gLast_time_credit_amount;

// GLOBAL: CARMA2_HW 0x0079eac0
int gOld_times[10];

// GLOBAL: CARMA2_HW 0x0067fcd8
br_font* gBR_fonts[4];

// GLOBAL: CARMA2_HW 0x0067fd00
br_pixelmap* gRevs[13];

// GLOBAL: CARMA2_HW 0x0074cf84
br_pixelmap* gArmour_colour_map;

// GLOBAL: CARMA2_HW 0x0067fcd0
br_pixelmap* gPowerbar;

// GLOBAL: CARMA2_HW 0x0074cab4
br_pixelmap* gPower_colour_map;

// GLOBAL: CARMA2_HW 0x0074cf5c
br_pixelmap* gOffensive_colour_map;

// GLOBAL: CARMA2_HW 0x006815bc
br_pixelmap* gIcon_test;

// GLOBAL: CARMA2_HW 0x0074ca1c
br_pixelmap* gCurrent_rev;

// GLOBAL: CARMA2_HW 0x0067c4f8
br_pixelmap* gHud_curve1;

// GLOBAL: CARMA2_HW 0x0067fcfc
br_pixelmap* gHud_curve2;

// GLOBAL: CARMA2_HW 0x0067f87c
br_pixelmap* gHud_curve3;

// GLOBAL: CARMA2_HW 0x0067fcf4
br_pixelmap* gGrey_top5;

// GLOBAL: CARMA2_HW 0x0074cf00
br_pixelmap* gDamage_hud;

// GLOBAL: CARMA2_HW 0x0074ca80
br_pixelmap* gTimer_lhs;

// GLOBAL: CARMA2_HW 0x0074ca44
br_pixelmap* gHud_timer_rhs;

// GLOBAL: CARMA2_HW 0x006815b8
br_pixelmap* gIcon_armour;

// GLOBAL: CARMA2_HW 0x006815b4
br_pixelmap* gIcon_power;

// GLOBAL: CARMA2_HW 0x0067c4f0
br_pixelmap* gIcon_offense;

// GLOBAL: CARMA2_HW 0x0067c4f4
br_pixelmap* gIcon_greyBloc1;

// GLOBAL: CARMA2_HW 0x0067fce8
br_pixelmap* gIcon_litBloc1;

// GLOBAL: CARMA2_HW 0x0067fcd4
br_pixelmap* gIcon_grnBlock1;

// GLOBAL: CARMA2_HW 0x0067fd40
tHud_message gHud_messages[46];

// GLOBAL: CARMA2_HW 0x00705be0
int gCredit_multiplier;

// GLOBAL: CARMA2_HW 0x0067f878
int gLast_credit_amount;

// GLOBAL: CARMA2_HW 0x0067fcc0
tU32 gLast_fancy_headup;

// GLOBAL: CARMA2_HW 0x005913ec
int gPrev_ps_apo_levels[3] = { -1, -1, -1};

// GLOBAL: CARMA2_HW 0x005913f8
int gPrev_ps_drawn_levels[3] = { -1, -1, -1};

// GLOBAL: CARMA2_HW 0x005913e0
int gPS_apo_level_changed[3] = { 1, 1, 1};

// GLOBAL: CARMA2_HW 0x0068d6ec
int gBack_original_origin_x;

// GLOBAL: CARMA2_HW 0x0068d6e8
int gBack_original_origin_y;

// GLOBAL: CARMA2_HW 0x0068d88c
int gBack_original_base_x;

// GLOBAL: CARMA2_HW 0x0068d888
int gBack_original_base_y;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMapUpdate();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrModelUpdate();
extern br_uint_16 C2_HOOK_CDECL BrPixelmapTextWidth();
extern void C2_HOOK_CDECL BrZbsSceneRender();
extern void C2_HOOK_FASTCALL DRPixelmapCopy(br_pixelmap* dst, br_pixelmap* src);
extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);
extern int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText);
extern int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void __cdecl _ftol();
extern br_actor* g2d_camera;
extern br_actor* gArmour_actor;
extern br_material* gArmour_material;
extern br_pixelmap* gDepth_buffer;
extern br_model* gDim_model;
extern tDR_font gFonts[28];
extern br_material* gOffence_material;
extern br_actor* gOffense_actor;
extern br_actor* gPower_actor;
extern br_material* gPower_material;
extern tProgram_state gProgram_state;
extern int gScreen_wobble_x;
extern int gScreen_wobble_y;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern void C2_HOOK_CDECL BrMatrix23LPNormalise();
extern void C2_HOOK_CDECL BrMatrix23TApplyV();
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34Mul();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrPixelmapFill();
extern void C2_HOOK_CDECL BrPixelmapLine();
extern void C2_HOOK_CDECL BrPixelmapPixelSet();
extern void C2_HOOK_CDECL BrPixelmapRectangleCopy();
extern void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);
extern void C2_HOOK_FASTCALL DoOpponentStatusHeadup(void);
extern void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev);
extern void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3 *pPos, br_uint_32 pColour);
extern void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);
extern void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour);
extern void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour);
extern void C2_HOOK_FASTCALL DrawNumberAt(const br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue, int pDigit_count, int pLeading_zeroes);
extern void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap *pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern int C2_HOOK_FASTCALL GetPedCount(void);
extern int C2_HOOK_FASTCALL GetPedPosition(int pIndex, br_vector3* pPos);
extern int C2_HOOK_FASTCALL GetPowerupCount(void);
extern void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition);
extern void C2_HOOK_FASTCALL InitMap(void);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);
extern tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry);
extern void C2_HOOK_FASTCALL PolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDestn, int pX1, float pAngle, int pY1, int pX2, int pY2, int pColour);
extern void C2_HOOK_FASTCALL PossibleLock(int pValue);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
extern br_pixelmap* gBack_screen;
extern br_matrix34 gCar_in_map_space;
extern tCar_spec* gCar_to_view;
extern int gCheckpoint_finder_enabled;
extern float gCosine_array[64];
extern int gCount_smashable_race_targets;
extern tGraf_data* gCurrent_graf_data;
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern char gDouble_0058a068[];
extern float gFLOAT_0074ab90;
extern float gFLOAT_0074abb8;
extern int gHeadup_detail_level;
extern int gHeadup_map_h;
extern int gHeadup_map_half_height;
extern int gHeadup_map_half_width;
extern int gHeadup_map_w;
extern int gHeadup_map_x;
extern int gHeadup_map_y;
extern int gINT_0068c858;
extern int gINT_0068d890;
extern int gINT_0074ab88;
extern int gINT_0074ab8c;
extern int gINT_0074ab94;
extern int gINT_0074abd0;
extern int gINT_0074abd4;
extern int gINT_0074abec;
extern int gMap_trans;
extern int gMap_view;
extern float gMini_map_arrow_z;
extern int gMini_map_visible;
extern tNet_mode gNet_mode;
extern int gPed_676914;
extern tPedestrian* gPedestrian_array;
extern br_actor* gPlayer_car_master_actor;
extern tU32 gRace_finished;
extern br_pixelmap* gRender_palette;
extern int gShow_opponents;
extern int gShow_peds_on_map;
extern tSmashable_race_target gSmashable_race_targets[300];
extern br_pixelmap* gSmashy_dot;
extern tCar_spec* gTarget_lock_car_2;
extern int gUNK_00596310;
extern br_vector2 gVector2_0068d6d8;
extern int C2_HOOK_FASTCALL sign(int pNumber);
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type);
extern void C2_HOOK_CDECL BrMatrix34Translate();
extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
extern void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);
extern int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state);
extern int C2_HOOK_FASTCALL GetAverageCrushLevel(tCar_spec* pCar);
extern const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor);
extern char gUNK_00768f68[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tCar_icon gCar_icons[128];
extern br_pixelmap* gDepth_shade_table;
extern int gDevious_2d;
extern tFrontend_spec gFrontend_QUIT;
extern br_pixelmap* gLit_op_stat;
extern tOppo_status_messages gOppo_status_messages[9];
extern tOpponent_Status gPrevious_opponent_status;
extern tCar_spec* gTarget_lock_car_1;
extern int gTarget_lock_enabled;
extern br_pixelmap* gTexture_maps[1024];
extern tU32 gTime_oppobar_target_wasted;
#endif
// FUNCTION: CARMA2_HW 0x0045aa60
void C2_NAKED C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gAcc_poly_material == NULL) {
        gAcc_poly_material = BrMaterialAllocate("Acc Poly Mat");
        gAcc_poly_material->colour = 0x80;
        gAcc_poly_material->flags = BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gAcc_poly_material);
        gAcc_poly_material->extra_prim = gAccent_poly_prims;
        gAccent_poly_prims[1].v.x = 0x800000;
        BrMaterialUpdate(gAcc_poly_material, BR_MATU_EXTRA_PRIM);
    }
    if (gAcc_poly_actor == NULL) {
        gAcc_poly_model = BrModelAllocate("Tint Poly", 6, 2);
        BrVector3Set(&gAcc_poly_model->vertices[0].p, pX,          -pY,                -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[1].p, pX + pWidth, -pY,                -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[2].p, pX,          -(pY + pHeight),    -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[3].p, pX + pWidth, -pY,                -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[4].p, pX + pWidth, -(pY + pHeight),    -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[5].p, pX,          -(pY + pHeight),    -1.01f);
        gAcc_poly_model->faces[0].vertices[0] = 0;
        gAcc_poly_model->faces[0].vertices[1] = 1;
        gAcc_poly_model->faces[0].vertices[2] = 2;
        gAcc_poly_model->faces[1].vertices[0] = 3;
        gAcc_poly_model->faces[1].vertices[1] = 4;
        gAcc_poly_model->faces[1].vertices[2] = 5;
        gAcc_poly_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gAcc_poly_actor->material = gAcc_poly_material;
        gAcc_poly_model->faces[0].material = gAcc_poly_material;
        gAcc_poly_model->faces[1].material = gAcc_poly_material;
        gAcc_poly_actor->model = gAcc_poly_model;
        BrModelAdd(gAcc_poly_model);
    }
    if (!gRender_acc_poly_actor) {
        gRender_acc_poly_actor = 1;
        BrActorAdd(g2d_camera, gAcc_poly_actor);
    }
#else
    __asm {
            mov eax, dword ptr [gAcc_poly_material]
            test eax, eax
            jne L0045aaca
            push offset gUNK_00596310+0x118
            call BrMaterialAllocate
            mov dword ptr [gAcc_poly_material], eax
            mov dword ptr [eax + 8], 0x80
            mov eax, dword ptr [gAcc_poly_material]
            add esp, 4
            mov dword ptr [eax + 0x20], 0x800
            mov ecx, dword ptr [gAcc_poly_material]
            push ecx
            call BrMaterialAdd
            mov edx, dword ptr [gAcc_poly_material]
            add esp, 4
            mov dword ptr [edx + 0x58], 0x5962f8
            mov eax, dword ptr [gAcc_poly_material]
            push 0x40
            mov ecx, dword ptr [eax + 0x58]
            mov dword ptr [ecx + 0xc], 0x800000
            mov edx, dword ptr [gAcc_poly_material]
            push edx
            call BrMaterialUpdate
            add esp, 8
    L0045aaca:
            mov eax, dword ptr [gAcc_poly_actor]
            test eax, eax
            jne L0045acc2
            push 2
            push 6
            push offset gUNK_00596310+0x10c
            call BrModelAllocate
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [gAcc_poly_model], eax
            mov eax, dword ptr [eax + 8]
            add esp, 0xc
            fld dword ptr [esp + 8]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [gAcc_poly_model]
            fchs
            mov eax, dword ptr [edx + 8]
            fst dword ptr [eax + 4]
            mov ecx, dword ptr [gAcc_poly_model]
            mov eax, 0xbf8147ae
            fld dword ptr [esp + 0xc]
            mov edx, dword ptr [ecx + 8]
            fadd dword ptr [esp + 4]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [ecx + 8]
            fst dword ptr [esp + 0xc]
            fst dword ptr [edx + 0x28]
            mov ecx, dword ptr [gAcc_poly_model]
            fld st(1)
            mov edx, dword ptr [ecx + 8]
            fstp dword ptr [edx + 0x2c]
            mov ecx, dword ptr [gAcc_poly_model]
            fld dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx + 8]
            fadd dword ptr [esp + 8]
            mov dword ptr [edx + 0x30], eax
            mov ecx, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 4]
            mov dword ptr [edx + 0x50], ecx
            mov edx, dword ptr [gAcc_poly_model]
            fchs
            mov ecx, dword ptr [edx + 8]
            fst dword ptr [esp + 8]
            fstp dword ptr [ecx + 0x54]
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0x58], eax
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            fstp dword ptr [ecx + 0x78]
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            fstp dword ptr [ecx + 0x7c]
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0x80], eax
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + 0xc]
            mov dword ptr [ecx + 0xa0], edx
            mov ecx, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 8]
            mov dword ptr [edx + 0xa4], ecx
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0xa8], eax
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + 4]
            mov dword ptr [ecx + 0xc8], edx
            mov ecx, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 8]
            mov dword ptr [edx + 0xcc], ecx
            mov edx, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0xd0], eax
            mov edx, dword ptr [gAcc_poly_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax], 0
            mov ecx, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [ecx + 0xc]
            push 0
            push 1
            mov word ptr [edx + 2], 1
            mov eax, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [eax + 0xc]
            mov word ptr [ecx + 4], 2
            mov edx, dword ptr [gAcc_poly_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax + 0x28], 3
            mov ecx, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [ecx + 0xc]
            mov word ptr [edx + 0x2a], 4
            mov eax, dword ptr [gAcc_poly_model]
            mov ecx, dword ptr [eax + 0xc]
            mov word ptr [ecx + 0x2c], 5
            call BrActorAllocate
            mov edx, dword ptr [gAcc_poly_material]
            mov dword ptr [gAcc_poly_actor], eax
            mov dword ptr [eax + 0x1c], edx
            mov eax, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [gAcc_poly_material]
            add esp, 8
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [gAcc_poly_model]
            mov edx, dword ptr [gAcc_poly_material]
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ecx + 0x30], edx
            mov ecx, dword ptr [gAcc_poly_actor]
            mov eax, dword ptr [gAcc_poly_model]
            mov dword ptr [ecx + 0x18], eax
            mov edx, dword ptr [gAcc_poly_model]
            push edx
            call BrModelAdd
            add esp, 4
    L0045acc2:
            mov eax, dword ptr [gRender_acc_poly_actor]
            test eax, eax
            jne L0045acea
            mov eax, dword ptr [gAcc_poly_actor]
            mov ecx, dword ptr [g2d_camera]
            push eax
            push ecx
            mov dword ptr [gRender_acc_poly_actor], 1
            call BrActorAdd
            add esp, 8
    L0045acea:
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044b300
void C2_NAKED C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text) {
#ifndef CARPOCALYPSE2_MATCHING
    char s[256];
    int original_amount;
    tU32 the_time;

    if (gRace_finished) {
        return;
    }
    the_time = GetTotalTime();
    if (pAmount == 0) {
        return;
    }
    if (gCredit_multiplier != 0) {
        pAmount *= gCredit_multiplier;
    }
    original_amount = pAmount;
    if (gLast_credit_headup__displays >= 0 && the_time - gLast_earn_time < 2000) {
        pAmount += gLast_credit_amount;
    }
    gLast_credit_amount = pAmount;
    if (pAmount >= 2) {
        sprintf(s, "%s%d %s", pPrefix_text, pAmount, GetMiscString(eMiscString_credits));
    } else if (pAmount == 1) {
        sprintf(s, "%s1 %s", pPrefix_text, GetMiscString(eMiscString_credit));
    } else if (pAmount == -1) {
        sprintf(s, "%s%s 1 %s", pPrefix_text, GetMiscString(eMiscString_lost_credits_prefix), GetMiscString(eMiscString_credit));
    } else {
        sprintf(s, "%s%s %d %s", GetMiscString(eMiscString_lost_credits_prefix), pPrefix_text, -pAmount, GetMiscString(eMiscString_credits));
    }
    gProgram_state.credits += original_amount;
    gLast_credit_headup__displays = NewTextHeadupSlot(4, 0, 2000, -4, s);
    gLast_earn_time = the_time;
#else
    __asm {
            mov eax, dword ptr [gRace_finished]
            sub esp, 0x100
            test eax, eax
            push ebx
            push ebp
            push esi
            mov ebx, edx
            mov esi, ecx
            jne L0044b429
            call GetTotalTime
            test esi, esi
            mov ebp, eax
            je L0044b429
            mov eax, dword ptr [gCredit_multiplier]
            push edi
            test eax, eax
            je L0044b336
            imul esi, eax
    L0044b336:
            mov eax, dword ptr [gLast_credit_headup__displays]
            mov edi, esi
            test eax, eax
            jl L0044b358
            mov edx, dword ptr [gLast_earn_time]
            mov eax, ebp
            sub eax, edx
            cmp eax, 0x7d0
            jae L0044b358
            add esi, dword ptr [gLast_credit_amount]
    L0044b358:
            cmp esi, 1
            mov dword ptr [gLast_credit_amount], esi
            jle L0044b377
            mov ecx, 0xc
            call GetMiscString
            push eax
            push esi
            push ebx
            push offset gPrev_ps_drawn_levels+0x20c
            jmp L0044b3ea
    L0044b377:
            test esi, esi
            jle L0044b39b
            mov ecx, 0xd
            call GetMiscString
            lea edx, [esp + 0x10]
            push eax
            push ebx
            push offset gPrev_ps_drawn_levels+0x204
            push edx
            call sprintf
            add esp, 0x10
            jmp L0044b3f7
    L0044b39b:
            cmp esi, -1
            jge L0044b3ce
            mov ecx, 0xc
            call GetMiscString
            neg esi
            push eax
            push esi
            push ebx
            mov ecx, 0xe
            call GetMiscString
            push eax
            lea eax, [esp + 0x20]
            push offset gPrev_ps_drawn_levels+0x1f8
            push eax
            call sprintf
            add esp, 0x18
            jmp L0044b3f7
    L0044b3ce:
            mov ecx, 0xd
            call GetMiscString
            mov ecx, 0xe
            push eax
            call GetMiscString
            push eax
            push ebx
            push offset gPrev_ps_drawn_levels+0x1ec
    L0044b3ea:
            lea ecx, [esp + 0x20]
            push ecx
            call sprintf
            add esp, 0x14
    L0044b3f7:
            mov eax, dword ptr [gProgram_state]
            lea edx, [esp + 0x10]
            push 1
            push edx
            add eax, edi
            push -4
            push 0x7d0
            xor edx, edx
            mov ecx, 4
            mov dword ptr [gProgram_state], eax
            call NewTextHeadupSlot2
            mov dword ptr [gLast_credit_headup__displays], eax
            mov dword ptr [gLast_earn_time], ebp
            pop edi
    L0044b429:
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044b440
void C2_HOOK_FASTCALL EarnCredits(int pAmount) {
    EarnCredits2(pAmount, "");
}

// FUNCTION: CARMA2_HW 0x0044b450
void C2_HOOK_FASTCALL EarnCredits100(int pAmount) {
    EarnCredits2((pAmount + 50) - (pAmount + 50) % 100, "");
}

// FUNCTION: CARMA2_HW 0x0044b470
int C2_HOOK_FASTCALL SpendCredits(int pAmount) {
    int result;

    result = gProgram_state.credits - pAmount;
    gProgram_state.credits = result;
    if (result < 0) {
        gProgram_state.credits = 0;
    } else {
        return 0;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x0044b490
void C2_HOOK_FASTCALL AwardTime(tU32 pTime) {
    char s[256];
    tU32 original_amount;
    tU32 the_time;
    int i;

    if (gRace_finished || gFreeze_timer || gNet_mode != eNet_mode_none || (*(tGraf_data**)((char*)&gCurrent_race + 0x98))->field_0x18c) {
        return;
    }
    the_time = GetTotalTime();
    if (pTime != 0) {
        for (i = 9; i > 0; i--) {
            gOld_times[i] = gOld_times[i - 1];
        }
        gOld_times[0] = pTime;
        original_amount = pTime;
        if (gLast_time_credit_headup >= 0 && (the_time - gLast_time_earn_time) < 2000) {
            pTime += gLast_time_credit_amount;
        }
        gLast_time_credit_amount = pTime;
        gTimer += original_amount * 1000;
        s[0] = '+';
        TimerString(1000 * pTime, &s[1], 0, 0, 0);
        gLast_time_credit_headup = NewTextHeadupSlot2(0xb, 0, 2000, -2, s, 1);
        gLast_time_earn_time = the_time;
    }
}

// FUNCTION: CARMA2_HW 0x0044b590
void C2_HOOK_FASTCALL AwardTime5(tU32 pTime) {
    char s[256];
    tU32 original_amount;
    tU32 the_time;
    int i;
    tU32 rounded_time;

    rounded_time = pTime - (pTime + 2) % 5 + 2;
    if (gRace_finished || gFreeze_timer || gNet_mode != eNet_mode_none || (*(tGraf_data**)((char*)&gCurrent_race + 0x98))->field_0x18c) {
        return;
    }
    the_time = GetTotalTime();
    if (rounded_time != 0) {
        for (i = 9; i > 0; i--) {
            gOld_times[i] = gOld_times[i - 1];
        }
        gOld_times[0] = rounded_time;
        original_amount = rounded_time;
        if (gLast_time_credit_headup >= 0 && (the_time - gLast_time_earn_time) < 2000) {
            rounded_time += gLast_time_credit_amount;
        }
        gLast_time_credit_amount = rounded_time;
        gTimer += original_amount * 1000;
        s[0] = '+';
        TimerString(1000 * rounded_time, &s[1], 0, 0, 0);
        gLast_time_credit_headup = NewTextHeadupSlot2(0xb, 0, 2000, -2, s, 1);
        gLast_time_earn_time = the_time;
    }
}

// FUNCTION: CARMA2_HW 0x0044b170
void C2_HOOK_FASTCALL ChangingView(void) {
    tU32 the_time;

    the_time = PDGetTotalTime() - gProgram_state.view_change_start;
    if (gProgram_state.new_view == eView_undefined) {
        return;
    }
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
    if (the_time > 175 && gProgram_state.which_view == gProgram_state.new_view) {
        if (gProgram_state.pending_view != eView_undefined) {
            switch (gProgram_state.pending_view) {
            case eView_left:
                LookLeft();
                break;
            case eView_forward:
                LookForward();
                break;
            case eView_right:
                LookRight();
                break;
            default:
                break;
            }
        } else {
            gProgram_state.new_view = eView_undefined;
        }
    } else if (the_time > 87) {
        gProgram_state.which_view = gProgram_state.new_view;
        switch (gProgram_state.new_view) {
        case eView_left:
            gProgram_state.cockpit_image_index = 1;
            break;
        case eView_forward:
            gProgram_state.cockpit_image_index = 0;
            break;
        case eView_right:
            gProgram_state.cockpit_image_index = 2;
            break;
        default:
            break;
        }
        AdjustRenderScreenSize();
        if (gProgram_state.old_view < gProgram_state.new_view) {
            gScreen_wobble_x = (int)((double)(175 - the_time) * (double)gCurrent_graf_data->cock_margin_x * (2. / 175.));
        } else {
            gScreen_wobble_x = (int)((double)(175 - the_time) * (double)gCurrent_graf_data->cock_margin_x * (-2. / 175.));
        }
    } else {
        if (gProgram_state.old_view < gProgram_state.new_view) {
            gScreen_wobble_x = (int)((double)the_time * (double)gCurrent_graf_data->cock_margin_x * (-2. / 175.));
        } else {
            gScreen_wobble_x = (int)((double)the_time * (double)gCurrent_graf_data->cock_margin_x * (2. / 175.));
        }
    }
}

br_uint_32 C2_HOOK_FASTCALL CarArrowColour(tCar_spec *pCar, tVehicle_type pCategory) {
    tNet_game_player_info* net_player;

    if (gNet_mode != eNet_mode_none && pCar->shrapnel_materials[0] != NULL) {
        net_player = NetPlayerFromCar(pCar);
        if (net_player->field_0x80) {
            return 0xffffffff;
        } else {
            return pCar->shrapnel_materials[0]->index_base;
        }
    } else {
        if (pCar->knackered) {
            return 0x0;
        } else {
            return gCar_map_colours[pCategory];
        }
    }
}

void C2_HOOK_FASTCALL DrawCarArrow(br_pixelmap* pScreen, tCar_spec *pCar, tU32 pTime, const br_vector3* pPosition, tVehicle_type category) {

    DrawMapBlip(pScreen, pCar, pTime, &pCar->car_master_actor->t.t.mat, pPosition, CarArrowColour(pCar, category));
}

void C2_HOOK_FASTCALL DrawSmashBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3* pPosition, int pIndex) {
    br_vector3 map_pos;
    br_vector3 dir;
    br_scalar dot;

    BrMatrix34ApplyP(&map_pos, pPosition, &gCurrent_race.map_transformation);
    BrVector3Set(&dir,
        (float)(gHeadup_map_half_width + gINT_0068d890 + gINT_0074ab8c) - map_pos.v[0],
        (float)(gHeadup_map_half_height + gINT_0068c858 + gINT_0074ab88) - map_pos.v[1],
        0.f);
    dot = BrVector3LengthSquared(&dir);
    if (gINT_0068d6f4 == -1 || dot < gFLOAT_0074abc4) {
        gINT_0068d8b8 = 0;
        gINT_0068d6f4 = pIndex;
        gVector2_0068d8b0.v[0] = dir.v[0];
        gVector2_0068d8b0.v[1] = dir.v[1];
        gFLOAT_0074abc4 = dot;
    }
    if (gNet_mode == eNet_mode_none && (pTime & 0x100)) {
        if (gMap_view == 2) {
            PossibleLock(1);
        }
        DRPixelmapRectangleMaskedCopy(pScreen,
            (int)(map_pos.v[0] - gSmashy_dot->width / 2),
            (int)(map_pos.v[1] - gSmashy_dot->height / 2),
            gSmashy_dot,
            0,
            0,
            gSmashy_dot->width,
            gSmashy_dot->height);
        if (gMap_view == 2) {
            PossibleUnlock(1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00495e10
void C2_NAKED C2_HOOK_FASTCALL DoMapOverlays(br_pixelmap* pScreen) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 the_time;
    int category;
    int car_count;
    tCar_spec* car;
    int ped_count;
    int powerup_count;
    tNet_game_player_info* net_player;
    int i;
    br_vector3 ped_position;
    br_vector3* smash_position;
    br_actor* actor;

    the_time = PDGetTotalTime();
    if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
        if (gMap_view == 2) {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint, 1);
            DoSomeThingsToCheckpoints(pScreen, the_time, DrawCheckpoint, 1);
        }
        else {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint2, 1);
        }
    }
    if ((gShow_peds_on_map || gCurrent_race.race_spec->race_type == kRaceType_Peds)
            || (gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)) {

        ped_count = GetPedCount();
        for (i = 0; i < ped_count; i++) {
            if (GetPedPosition(i, &ped_position)) {
                DrawMapSmallBlip(pScreen, the_time, &ped_position, 52);
            }
        }
    }

    if ((gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)
            || gCurrent_race.race_spec->expansion) {

        powerup_count = GetPowerupCount();
        for (i = 0; i < powerup_count; i++) {
            GetPowerupPosition(i, &ped_position);
            DrawMapSmallBlip(pScreen, the_time, &ped_position, 4);
        }
    }
    for (category = gShow_opponents ? eVehicle_opponent : eVehicle_self; category >= (gMap_view == 1 ? eVehicle_net_player : eVehicle_self); category -= 1) {
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (i = 0; i < car_count; i++) {
            if (category == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(category, i);
            }
            if (gNet_mode != eNet_mode_none) {
                net_player = NetPlayerFromCar(car);
            }
            if (gNet_mode == eNet_mode_none
                    || (!car->knackered && !net_player->wasted && net_player->player_status >= ePlayer_status_racing)) {

                if (category == eVehicle_self) {
                    actor = gPlayer_car_master_actor;
                } else {
                    actor = GetCarSpec(category, i)->car_master_actor;
                }
                DrawCarArrow(pScreen, car, the_time, &actor->t.t.translate.t, category);
            }
        }
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        for (i = 0; i < gCount_smashable_race_targets; i++) {
            tSmashable_race_target* smash_target = &gSmashable_race_targets[i];

            if ((smash_target->model != NULL && smash_target->actor->render_style != BR_RSTYLE_NONE && smash_target->actor->model == smash_target->model)
                    || (smash_target->field_0x8 >= 0 && gPedestrian_array[smash_target->field_0x8].hit_points > 0)) {
                if (smash_target->field_0x8 >= 0) {
                    smash_position = &gPedestrian_array[smash_target->field_0x8].pos;
                } else {
                    smash_position = &smash_target->actor->t.t.translate.t;
                }

                DrawSmashBlip(pScreen, the_time, smash_position, i);
            }
        }
    }
#else
    __asm {
            sub esp, 0x34
            push ebx
            push ebp
            push esi
            mov ebx, ecx
            push edi
            mov dword ptr [esp + 0x14], ebx
            call PDGetTotalTime
            mov ebp, eax
            mov eax, dword ptr [gCurrent_race+0x98]
            mov dword ptr [esp + 0x18], ebp
            mov eax, dword ptr [eax + 0x210]
            test eax, eax
            je L00495e3c
            cmp eax, 3
            jne L00495e6d
    L00495e3c:
            mov eax, dword ptr [gMap_view]
            push 1
            cmp eax, 2
            jne L00495e5f
            push offset CalcMapCheckpoint
            mov edx, ebp
            mov ecx, ebx
            call DoSomeThingsToCheckpoints
            push 1
            push offset DrawCheckpoint
            jmp L00495e64
    L00495e5f:
            push offset CalcMapCheckpoint2
    L00495e64:
            mov edx, ebp
            mov ecx, ebx
            call DoSomeThingsToCheckpoints
    L00495e6d:
            mov eax, dword ptr [gShow_peds_on_map]
            test eax, eax
            jne L00495e9b
            mov ecx, dword ptr [gCurrent_race+0x98]
            cmp dword ptr [ecx + 0x210], 2
            je L00495e9b
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00495ee2
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x4c]
            test eax, eax
            je L00495ecc
    L00495e9b:
            call GetPedCount
            mov edi, eax
            xor esi, esi
            test edi, edi
            jle L00495ecc
    L00495ea8:
            lea edx, [esp + 0x38]
            mov ecx, esi
            call GetPedPosition
            test eax, eax
            je L00495ec7
            lea eax, [esp + 0x38]
            push 0x34
            push eax
            mov edx, ebp
            mov ecx, ebx
            call DrawMapSmallBlip
    L00495ec7:
            inc esi
            cmp esi, edi
            jl L00495ea8
    L00495ecc:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00495ee2
            mov ecx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [ecx + 0x4c]
            test eax, eax
            jne L00495ef2
    L00495ee2:
            mov edx, dword ptr [gCurrent_race+0x98]
            mov eax, dword ptr [edx + 0x188]
            test eax, eax
            je L00495f1f
    L00495ef2:
            call GetPowerupCount
            mov edi, eax
            xor esi, esi
            test edi, edi
            jle L00495f1f
    L00495eff:
            lea edx, [esp + 0x38]
            mov ecx, esi
            call GetPowerupPosition
            lea eax, [esp + 0x38]
            mov edx, ebp
            push 4
            push eax
            mov ecx, ebx
            call DrawMapSmallBlip
            inc esi
            cmp esi, edi
            jl L00495eff
    L00495f1f:
            mov ebp, dword ptr [gShow_opponents]
            mov edx, dword ptr [gMap_view]
            neg ebp
            sbb ebp, ebp
            xor ecx, ecx
            and ebp, 2
            cmp edx, 1
            sete cl
            cmp ebp, ecx
            jl L00496072
    L00495f42:
            test ebp, ebp
            jne L00495f50
            mov dword ptr [esp + 0x10], 1
            jmp L00495f5b
    L00495f50:
            mov ecx, ebp
            call GetCarCount
            mov dword ptr [esp + 0x10], eax
    L00495f5b:
            mov eax, dword ptr [esp + 0x10]
            xor ebx, ebx
            test eax, eax
            jle L00496057
    L00495f69:
            test ebp, ebp
            jne L00495f74
            mov esi, offset gProgram_state+0xac
            jmp L00495f7f
    L00495f74:
            mov edx, ebx
            mov ecx, ebp
            call GetCarSpec
            mov esi, eax
    L00495f7f:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00495f95
            mov ecx, esi
            call NetPlayerFromCar
            mov dword ptr [esp + 0x1c], eax
            jmp L00495f99
    L00495f95:
            mov eax, dword ptr [esp + 0x1c]
    L00495f99:
            mov ecx, dword ptr [gNet_mode]
            test ecx, ecx
            je L00495fc6
            mov ecx, dword ptr [esi + 0x1d4]
            test ecx, ecx
            jne L0049604a
            mov ecx, dword ptr [eax + 0x6c]
            test ecx, ecx
            jne L0049604a
            cmp dword ptr [eax + 0x48], 6
            jl L0049604a
    L00495fc6:
            test ebp, ebp
            jne L00495fd5
            mov edx, dword ptr [gPlayer_car_master_actor]
            lea edi, [edx + 0x50]
            jmp L00495fe4
    L00495fd5:
            mov edx, ebx
            mov ecx, ebp
            call GetCarSpec
            mov edi, dword ptr [eax + 0x10]
            add edi, 0x50
    L00495fe4:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L0049601c
            mov eax, dword ptr [esi + 0x84]
            test eax, eax
            je L0049601c
            mov ecx, esi
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            je L0049600d
            or eax, 0xffffffff
            jmp L00496031
    L0049600d:
            mov eax, dword ptr [esi + 0x84]
            xor ecx, ecx
            mov cl, byte ptr [eax + 0x3c]
            mov eax, ecx
            jmp L00496031
    L0049601c:
            mov eax, dword ptr [esi + 0x1d4]
            test eax, eax
            je L0049602a
            xor eax, eax
            jmp L00496031
    L0049602a:
            mov eax, dword ptr [ebp*4 + gCar_map_colours]
    L00496031:
            mov edx, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            add edx, 0x2c
            push edi
            push edx
            push eax
            mov edx, esi
            call DrawMapBlip
    L0049604a:
            mov eax, dword ptr [esp + 0x10]
            inc ebx
            cmp ebx, eax
            jl L00495f69
    L00496057:
            mov edx, dword ptr [gMap_view]
            dec ebp
            xor ecx, ecx
            cmp edx, 1
            sete cl
            cmp ebp, ecx
            jge L00495f42
            mov ebx, dword ptr [esp + 0x14]
    L00496072:
            mov edx, dword ptr [gCurrent_race+0x98]
            cmp dword ptr [edx + 0x210], 4
            jl L00496262
            mov eax, dword ptr [gCount_smashable_race_targets]
            xor ebp, ebp
            test eax, eax
            jle L00496262
            mov edi, offset gSmashable_race_targets+0x8
    L00496099:
            mov ecx, dword ptr [edi - 8]
            test ecx, ecx
            je L004960ae
            mov eax, dword ptr [edi - 4]
            cmp byte ptr [eax + 0x20], 1
            je L004960ae
            cmp dword ptr [eax + 0x18], ecx
            je L004960d6
    L004960ae:
            mov ecx, dword ptr [edi]
            test ecx, ecx
            jl L00496251
            lea eax, [ecx*8]
            sub eax, ecx
            mov ecx, dword ptr [gPedestrian_array]
            lea eax, [eax + eax*2]
            mov dl, byte ptr [ecx + eax*4 + 4]
            test dl, dl
            jle L00496251
    L004960d6:
            mov ecx, dword ptr [edi]
            test ecx, ecx
            jl L004960f3
            lea eax, [ecx*8]
            sub eax, ecx
            lea edx, [eax + eax*2]
            mov eax, dword ptr [gPedestrian_array]
            lea eax, [eax + edx*4 + 0x1c]
            jmp L004960f9
    L004960f3:
            mov eax, dword ptr [edi - 4]
            add eax, 0x50
    L004960f9:
            push offset gCurrent_race+0xed4
            lea ecx, [esp + 0x30]
            push eax
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [gHeadup_map_half_width]
            mov eax, dword ptr [gINT_0068d890]
            mov ecx, dword ptr [gHeadup_map_half_height]
            mov esi, dword ptr [gINT_0074ab88]
            add edx, eax
            mov eax, dword ptr [gINT_0068c858]
            add ecx, eax
            mov eax, dword ptr [gINT_0074ab8c]
            add edx, eax
            add ecx, esi
            mov dword ptr [esp + 0x28], edx
            mov dword ptr [esp + 0x20], ecx
            fild dword ptr [esp + 0x28]
            fild dword ptr [esp + 0x20]
            fld dword ptr [gDouble_0058a068+0x2e8]
            fxch st(2)
            fsub dword ptr [esp + 0x38]
            fxch st(1)
            fsub dword ptr [esp + 0x3c]
            fld dword ptr [gDouble_0058a068+0x2e8]
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fst dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(3)
            fstp dword ptr [esp + 0x40]
            mov eax, dword ptr [gINT_0068d6f4]
            add esp, 0xc
            fxch st(2)
            faddp st(2), st(0)
            cmp eax, -1
            fstp dword ptr [esp + 0x28]
            je L00496196
            fcom dword ptr [gFLOAT_0074abc4]
            fnstsw ax
            test ah, 1
            je L004961c1
    L00496196:
            mov ecx, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x28]
            mov dword ptr [gVector2_0068d8b0], ecx
            fstp dword ptr [gFLOAT_0074abc4]
            mov dword ptr [gVector2_0068d8b0+0x4], edx
            mov dword ptr [gINT_0068d8b8], eax
            mov dword ptr [gINT_0068d6f4], ebp
            jmp L004961c3
    L004961c1:
            fstp st(0)
    L004961c3:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            jne L00496251
            mov eax, dword ptr [esp + 0x18]
            test ah, 1
            je L00496251
            cmp dword ptr [gMap_view], 2
            jne L004961ec
            mov ecx, 1
            call PossibleLock
    L004961ec:
            mov ecx, dword ptr [gSmashy_dot]
            mov ax, word ptr [ecx + 0x36]
            mov si, word ptr [ecx + 0x34]
            push eax
            push esi
            shr ax, 1
            and eax, 0xffff
            push 0
            mov dword ptr [esp + 0x28], eax
            push 0
            fild dword ptr [esp + 0x2c]
            push ecx
            fsubr dword ptr [esp + 0x44]
            call _ftol
            shr si, 1
            and esi, 0xffff
            push eax
            mov dword ptr [esp + 0x34], esi
            fild dword ptr [esp + 0x34]
            fsubr dword ptr [esp + 0x44]
            call _ftol
            mov edx, eax
            mov ecx, ebx
            call DRPixelmapRectangleMaskedCopy
            cmp dword ptr [gMap_view], 2
            jne L00496251
            mov ecx, 1
            call PossibleUnlock
    L00496251:
            mov eax, dword ptr [gCount_smashable_race_targets]
            inc ebp
            add edi, 0xc
            cmp ebp, eax
            jl L00496099
    L00496262:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x34
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00494f40
void C2_HOOK_FASTCALL BuildOppoBar(int pX, int pY) {
    br_material* material;
    br_model* model;
    br_actor* actor;
    int x;

    x = pX;

    material = BrMaterialAllocate("OppoBar Material");
    if (material == NULL) {
        return;
    }
    material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE;
    model = BrModelAllocate("OppoBar Model", 4, 2);
    if (model == NULL) {
        return;
    }
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;

    model->vertices[0].p.v[0] = (br_scalar)x;
    model->vertices[0].p.v[1] = (br_scalar)-pY;
    model->vertices[0].p.v[2] = -1.5f;
    model->vertices[1].p.v[0] = (br_scalar)(x + 20);
    model->vertices[1].p.v[1] = (br_scalar)-pY;
    model->vertices[1].p.v[2] = -1.5f;
    model->vertices[2].p.v[0] = (br_scalar)x;
    model->vertices[2].p.v[1] = (br_scalar)(-7 - pY);
    model->vertices[2].p.v[2] = -1.5f;
    model->vertices[3].p.v[0] = (br_scalar)(x + 20);
    model->vertices[3].p.v[1] = (br_scalar)(-7 - pY);
    model->vertices[3].p.v[2] = -1.5f;

    model->vertices[0].red = 0xff;
    model->vertices[0].grn = 0;
    model->vertices[0].blu = 0;
    model->vertices[2].red = 0xff;
    model->vertices[2].grn = 0;
    model->vertices[2].blu = 0;

    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrModelAdd(model);
    BrMaterialAdd(material);
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gLit_op_stat_actor = actor;
    if (actor != NULL) {
        actor->material = material;
        gLit_op_stat_actor->model = model;
    }
}

// FUNCTION: CARMA2_HW 0x004af640
tOpponent_Status C2_HOOK_FASTCALL GetOpponentStatus(tCar_spec* pCar) {
    tOpponent_spec* opponent;
    tOpponent_spec* op;
    int i;
    int count;

    opponent = NULL;
    if ((pCar->car_ID & 0xff00) == 0x200) {
        count = gProgram_state.AI_vehicles.number_of_opponents;
        op = gProgram_state.AI_vehicles.opponents;
        for (i = 0; i < count; i++, op++) {
            if (op->car_spec == pCar) {
                opponent = op;
                break;
            }
        }
    } else if ((pCar->car_ID & 0xff00) == 0x300) {
        count = gNumber_of_cops_before_faffage;
        op = gProgram_state.AI_vehicles.cops;
        for (i = 0; i < count; i++, op++) {
            if (op->car_spec == pCar) {
                opponent = op;
                break;
            }
        }
    }

    if (*(int*)((char*)pCar + 0x1d4) != 0) {
        return eOpponent_status_Wasted;
    }
    if (gCountdown != 0) {
        return eOpponent_status_On_grid;
    }
    if (gTime_stamp_for_this_munging < opponent->stun_time_ends) {
        return eOpponent_status_Stunned;
    }
    if (*(int*)((char*)opponent + 0xbc) != 0 && opponent->current_objective != eOOT_get_near_player) {
        return eOpponent_status_Confused;
    }
    switch (opponent->current_objective) {
        case eOOT_complete_race:
            return eOpponent_status_Racing;
        case eOOT_pursue_and_twat:
            return eOpponent_status_Trying_to_kill_you;
        case eOOT_run_away:
            return eOpponent_status_Running_away;
        case eOOT_get_near_player:
            return eOpponent_status_Frozen;
        default:
            return eOpponent_status_Trying_to_get_to_you;
    }
}

// Local helper; the original function at 0x575bf0 is the CRT's _strchr.
char* C2_HOOK_CDECL LookForChar(const char* pString, int pChar) {
    while (*pString != '\0') {
        if (*pString == (char)pChar) {
            return (char*)pString;
        }
        pString++;
    }
    return NULL;
}

// GLOBAL: CARMA2_HW 0x0068d900
tU32 gOppo_stat_flash_change_1;
// GLOBAL: CARMA2_HW 0x0068d904
tU32 gOppo_stat_flash_change_2;
// GLOBAL: CARMA2_HW 0x0068d908
int gOppo_stat_flash_state_1;
// GLOBAL: CARMA2_HW 0x0068d90c
int gOppo_stat_flash_state_2;

// GLOBAL: CARMA2_HW 0x00659b24
int gOppo_stat_message_index = -1;

// GLOBAL: CARMA2_HW 0x00659b24
int gOppo_stat_message_index;

extern int C2_HOOK_CDECL sub_576c00(int pValue, int pDivisor);

// FUNCTION: CARMA2_HW 0x00494900
void C2_NAKED C2_HOOK_FASTCALL DoOpponentStatusHeadup(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int damage;
    int status;
    int choice;
    int is_girl;
    char* p;
    char text[0x100];
    char copy[0x100];

    br_pixelmap* pm = gLit_op_stat;
    if (pm == NULL) {
        pm = LoadPixelmap("litopstat.tif");
        gLit_op_stat = pm;
    }
    BRPM_convert(pm, gBack_screen->type);

    Flash(250, &gOppo_stat_flash_change_1, &gOppo_stat_flash_state_1);

    if (gTarget_lock_enabled && Flash(500, &gOppo_stat_flash_change_2, &gOppo_stat_flash_state_2)) {
        DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x4d8, gCurrent_graf_data->field_0x4dc, &gFonts[23],
            GetMiscString(0xfd), gBack_screen->width);
    } else {
        const char* stat_text =
            GetMiscString((gTarget_lock_car_2 == NULL || gTarget_lock_car_2->driver == eDriver_oppo) ? 0xf6 : 0xf7);
        DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x4d8, gCurrent_graf_data->field_0x4dc, &gFonts[23],
            stat_text, gBack_screen->width);
    }

    DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x4e0, gCurrent_graf_data->field_0x4e4, &gFonts[23],
        GetMiscString(0xf8), gBack_screen->width);

    if (gNet_mode == eNet_mode_none) {
        DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x4e8, gCurrent_graf_data->field_0x4ec, &gFonts[23],
            GetMiscString(0xf9), gBack_screen->width);
    }

    if (gTarget_lock_car_2 == NULL) {
        return;
    }

    if (gNet_mode != eNet_mode_none) {
        strcpy(text, NetPlayerFromCar(gTarget_lock_car_2)->player_name);
    } else {
        unsigned int cid = gTarget_lock_car_2->car_ID;
        strcpy(text, GetDriverName(cid >> 8, cid & 0xff));
    }
    DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x4f0, gCurrent_graf_data->field_0x4f4, &gFonts[23],
        text, gBack_screen->width);

    BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat,
        (br_scalar)(float)gCurrent_graf_data->field_0x4f8, (br_scalar)(float)-gCurrent_graf_data->field_0x4fc, 0.0f);
    gCar_icons_model_actor->material->colour_map =
        gTexture_maps[gCar_icons[gTarget_lock_car_2->index].index];
    BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
    gCar_icons_model_actor->model = gCar_icons[gTarget_lock_car_2->index].model;
    RenderThisHeadup(gCar_icons_model_actor);

    if (gTarget_lock_car_2 == NULL) {
        return;
    }

    damage = GetAverageCrushLevel(gTarget_lock_car_2);
    sprintf(text, "%d%%", damage);
    DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x500, gCurrent_graf_data->field_0x504, &gFonts[23],
        text, gBack_screen->width);
    sub_576c00(damage, 10);

    if (gLit_op_stat_actor == NULL) {
        BuildOppoBar(gCurrent_graf_data->field_0x508, gCurrent_graf_data->field_0x50c + 2);
    }

    if (damage > 0) {
        br_model* model;
        if (damage > 100) {
            damage = 100;
        }
        model = gLit_op_stat_actor->model;
        model->vertices[1].p.v[0] = model->vertices[0].p.v[0] + (br_scalar)damage;
        model->vertices[3].p.v[0] = model->vertices[2].p.v[0] + (br_scalar)damage;
        {
            int rgb = (int)(damage * 255 / 100);
            model->vertices[1].red = (br_uint_8)(0xff - rgb);
            model->vertices[1].grn = (br_uint_8)rgb;
            model->vertices[1].blu = 0;
            model->vertices[3].red = (br_uint_8)(0xff - rgb);
            model->vertices[3].grn = (br_uint_8)rgb;
            model->vertices[3].blu = 0;
        }
        BrModelUpdate(model, BR_MODU_VERTICES);
        RenderThisHeadup(gLit_op_stat_actor);
    }

    if (gTarget_lock_car_2 == NULL) {
        return;
    }
    if (gTarget_lock_car_2->driver != eDriver_oppo) {
        return;
    }

    status = GetOpponentStatus(gTarget_lock_car_2);
    if (gTarget_lock_car_1 == gTarget_lock_car_2 && gPrevious_opponent_status == status) {
        choice = gOppo_stat_message_index;
    } else {
        choice = IRandomBetween(0, gOppo_status_messages[status].count - 1);
    }
    if (status == eOpponent_status_Wasted && gTarget_lock_car_1 != gTarget_lock_car_2) {
        gTime_oppobar_target_wasted = PDGetTotalTime();
    } else if (gPrevious_opponent_status != status) {
        gTime_oppobar_target_wasted = PDGetTotalTime();
    }
    gOppo_stat_message_index = choice;
    gPrevious_opponent_status = (tOpponent_Status)status;
    gTarget_lock_car_1 = gTarget_lock_car_2;

    if (status == eOpponent_status_Wasted && gOppo_stat_flash_state_1 == 0) {
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        return;
    }

    is_girl = gTarget_lock_car_2->is_girl;
    strcpy(text, gOppo_status_messages[status].messages[choice]);
    p = strchr(text, '%');
    while (p != NULL) {
        int index;
        strcpy(copy, p + 1);
        index = 0x100;
        if (is_girl) {
            index = 0x101;
        }
        strcpy(p, GetMiscString(index));
        strcat(text, copy);
        p = strchr(text, '%');
    }
    p = strchr(text, '&');
    while (p != NULL) {
        int index;
        strcpy(copy, p + 1);
        index = 0x102;
        if (is_girl) {
            index = 0x103;
        }
        strcpy(p, GetMiscString(index));
        strcat(text, copy);
        p = strchr(text, '&');
    }
    DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x510, gCurrent_graf_data->field_0x514, &gFonts[23],
        text, gBack_screen->width);
#else
    __asm {
            mov eax, dword ptr [gLit_op_stat]
            sub esp, 0x204
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L00494922
            mov ecx, offset gCar_map_colours+0x18
            call LoadPixelmap
            mov dword ptr [gLit_op_stat], eax
    L00494922:
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            mov dl, byte ptr [ecx + 0x2c]
            mov ecx, eax
            call BRPM_convert
            mov edx, offset gOppo_stat_flash_change_1
            mov ecx, 0xfa
            push offset gOppo_stat_flash_state_1
            call Flash
            mov eax, dword ptr [gTarget_lock_enabled]
            mov ebx, 6
            test eax, eax
            je L004949a4
            push offset gOppo_stat_flash_state_2
            mov edx, offset gOppo_stat_flash_change_2
            mov ecx, 0x1f4
            call Flash
            test eax, eax
            je L004949a4
            mov eax, dword ptr [gBack_screen]
            xor edx, edx
            mov ecx, 0xfd
            mov dx, word ptr [eax + 0x34]
            push edx
            call GetMiscString
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov ecx, dword ptr [eax + 0x4dc]
            mov edx, dword ptr [eax + 0x4d8]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            jmp L004949e8
    L004949a4:
            mov eax, dword ptr [gTarget_lock_car_2]
            test eax, eax
            je L004949b9
            mov ecx, dword ptr [eax + 0xc]
            cmp ecx, ebx
            mov ecx, 0xf7
            jne L004949be
    L004949b9:
            mov ecx, 0xf6
    L004949be:
            call GetMiscString
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            mov dx, word ptr [ecx + 0x34]
            push edx
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov edx, dword ptr [eax + 0x4dc]
            push edx
            mov edx, dword ptr [eax + 0x4d8]
    L004949e8:
            call DRPixelmapText
            mov ecx, dword ptr [gBack_screen]
            xor eax, eax
            mov ax, word ptr [ecx + 0x34]
            mov ecx, 0xf8
            push eax
            call GetMiscString
            mov ecx, dword ptr [gBack_screen]
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov edx, dword ptr [eax + 0x4e4]
            push edx
            mov edx, dword ptr [eax + 0x4e0]
            call DRPixelmapText
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            jne L00494a6a
            mov ecx, dword ptr [gBack_screen]
            xor eax, eax
            mov ax, word ptr [ecx + 0x34]
            mov ecx, 0xf9
            push eax
            call GetMiscString
            mov ecx, dword ptr [gBack_screen]
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov edx, dword ptr [eax + 0x4ec]
            push edx
            mov edx, dword ptr [eax + 0x4e8]
            call DRPixelmapText
    L00494a6a:
            mov ecx, dword ptr [gTarget_lock_car_2]
            test ecx, ecx
            je L00494f30
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00494a91
            call NetPlayerFromCar
            mov edi, eax
            lea edx, [esp + 0x14]
            add edi, 0x28
            jmp L00494ab2
    L00494a91:
            xor eax, eax
            mov ax, word ptr [ecx + 0x80]
            mov edx, eax
            mov ecx, eax
            and edx, 0xff
            shr ecx, 8
            call GetDriverName
            lea edx, [esp + 0x14]
            mov edi, eax
    L00494ab2:
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            xor edx, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            lea eax, [esp + 0x14]
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gBack_screen]
            mov dx, word ptr [ecx + 0x34]
            push edx
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov edx, dword ptr [eax + 0x4f4]
            push edx
            mov edx, dword ptr [eax + 0x4f0]
            call DRPixelmapText
            mov eax, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [gCar_icons_model_actor]
            push 0
            add edx, 0x2c
            mov ecx, dword ptr [eax + 0x4fc]
            neg ecx
            mov dword ptr [esp + 0x14], ecx
            push ecx
            fild dword ptr [esp + 0x18]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            fild dword ptr [eax + 0x4f8]
            push ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push edx
            call BrMatrix34Translate
            mov eax, dword ptr [gTarget_lock_car_2]
            add esp, 0x10
            mov ecx, dword ptr [eax]
            mov eax, dword ptr [gCar_icons_model_actor]
            push 8
            mov edx, dword ptr [ecx*8 + gCar_icons+0x4]
            mov ecx, dword ptr [eax + 0x1c]
            mov edx, dword ptr [edx*4 + gTexture_maps]
            mov dword ptr [ecx + 0x40], edx
            mov eax, dword ptr [gCar_icons_model_actor]
            mov ecx, dword ptr [eax + 0x1c]
            push ecx
            call BrMaterialUpdate
            mov edx, dword ptr [gTarget_lock_car_2]
            add esp, 8
            mov eax, dword ptr [edx]
            mov edx, dword ptr [gCar_icons_model_actor]
            mov ecx, dword ptr [eax*8 + gCar_icons]
            mov dword ptr [edx + 0x18], ecx
            mov ecx, dword ptr [gCar_icons_model_actor]
            call RenderThisHeadup
            mov ecx, dword ptr [gTarget_lock_car_2]
            test ecx, ecx
            je L00494f30
            call GetAverageCrushLevel
            mov esi, eax
            lea eax, [esp + 0x14]
            push esi
            push offset gFrontend_QUIT+0xbd4c
            push eax
            mov dword ptr [esp + 0x1c], esi
            call sprintf
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            add esp, 0xc
            lea eax, [esp + 0x14]
            mov dx, word ptr [ecx + 0x34]
            push edx
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov edx, dword ptr [eax + 0x504]
            push edx
            mov edx, dword ptr [eax + 0x500]
            call DRPixelmapText
            push 0xa
            push esi
            call sub_576c00
            mov eax, dword ptr [gLit_op_stat_actor]
            add esp, 8
            test eax, eax
            jne L00494c11
            mov eax, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [eax + 0x50c]
            mov ecx, dword ptr [eax + 0x508]
            add edx, 2
            call BuildOppoBar
    L00494c11:
            test esi, esi
            jle L00494cab
            cmp esi, 0x64
            jle L00494c27
            mov esi, 0x64
            mov dword ptr [esp + 0x10], esi
    L00494c27:
            mov eax, dword ptr [gLit_op_stat_actor]
            mov edx, esi
            fild dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax + 0x18]
            push 0x40
            shl edx, 8
            mov eax, dword ptr [ecx + 8]
            sub edx, esi
            push ecx
            _emit 0xd9
        _emit 0x00
            fadd st(0), st(1)
            fstp dword ptr [eax + 0x28]
            mov eax, dword ptr [ecx + 8]
            fld dword ptr [eax + 0x50]
            fadd st(0), st(1)
            fstp dword ptr [eax + 0x78]
            mov eax, 0x51eb851f
            mov esi, dword ptr [ecx + 8]
            imul edx
            fstp st(0)
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            or al, 0xff
            sub al, dl
            mov byte ptr [esi + 0x3d], al
            mov esi, dword ptr [ecx + 8]
            mov byte ptr [esi + 0x3e], dl
            mov esi, dword ptr [ecx + 8]
            mov byte ptr [esi + 0x3f], 0
            mov esi, dword ptr [ecx + 8]
            mov byte ptr [esi + 0x8d], al
            mov eax, dword ptr [ecx + 8]
            mov byte ptr [eax + 0x8e], dl
            mov edx, dword ptr [ecx + 8]
            mov byte ptr [edx + 0x8f], 0
            call BrModelUpdate
            mov ecx, dword ptr [gLit_op_stat_actor]
            add esp, 8
            call RenderThisHeadup
    L00494cab:
            mov ecx, dword ptr [gTarget_lock_car_2]
            test ecx, ecx
            je L00494f30
            cmp dword ptr [ecx + 0xc], ebx
            jne L00494f30
            call GetOpponentStatus
            mov ecx, dword ptr [gTarget_lock_car_1]
            mov esi, eax
            mov eax, dword ptr [gTarget_lock_car_2]
            cmp ecx, eax
            jne L00494ce8
            cmp dword ptr [gPrevious_opponent_status], esi
            jne L00494ce8
            mov edi, dword ptr [gOppo_stat_message_index]
            jmp L00494d04
    L00494ce8:
            lea eax, [esi + esi*4]
            lea ecx, [esi + eax*2]
            mov edx, dword ptr [ecx*4 + gOppo_status_messages]
            xor ecx, ecx
            dec edx
            call IRandomBetween
            mov edi, eax
            mov eax, dword ptr [gTarget_lock_car_2]
    L00494d04:
            cmp esi, 8
            jne L00494d11
            cmp dword ptr [gTarget_lock_car_1], eax
            jne L00494d19
    L00494d11:
            cmp dword ptr [gPrevious_opponent_status], esi
            je L00494d28
    L00494d19:
            call PDGetTotalTime
            mov dword ptr [gTime_oppobar_target_wasted], eax
            mov eax, dword ptr [gTarget_lock_car_2]
    L00494d28:
            cmp esi, 8
            mov dword ptr [gOppo_stat_message_index], edi
            mov dword ptr [gPrevious_opponent_status], esi
            mov dword ptr [gTarget_lock_car_1], eax
            jne L00494d4c
            mov ecx, dword ptr [gOppo_stat_flash_state_1]
            test ecx, ecx
            je L00494f30
    L00494d4c:
            mov ecx, dword ptr [gNet_mode]
            test ecx, ecx
            jne L00494f30
            mov ebx, dword ptr [eax + 0x18c4]
            lea edx, [esi + esi*4]
            add edi, esi
            or ecx, 0xffffffff
            push 0x25
            lea eax, [edi + edx*2]
            lea edx, [esp + 0x18]
            mov edi, dword ptr [eax*4 + gOppo_status_messages+0x4]
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
            lea ecx, [esp + 0x18]
            push ecx
            call strchr
            mov ebp, eax
            add esp, 8
            test ebp, ebp
            je L00494e49
    L00494da9:
            lea edi, [ebp + 1]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x114]
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
            test ebx, ebx
            mov ecx, 0x100
            je L00494dde
            mov ecx, 0x101
    L00494dde:
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            push 0x25
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, ebp
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x18]
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x118]
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
            mov ecx, ebp
            lea eax, [esp + 0x18]
            and ecx, 3
            push eax
            rep movsb
            call strchr
            mov ebp, eax
            add esp, 8
            test ebp, ebp
            jne L00494da9
    L00494e49:
            lea ecx, [esp + 0x14]
            push 0x26
            push ecx
            call strchr
            mov ebp, eax
            add esp, 8
            test ebp, ebp
            je L00494f02
    L00494e62:
            lea edi, [ebp + 1]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x114]
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
            test ebx, ebx
            mov ecx, 0x102
            je L00494e97
            mov ecx, 0x103
    L00494e97:
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            push 0x26
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, ebp
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x18]
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x118]
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
            mov ecx, ebp
            lea eax, [esp + 0x18]
            and ecx, 3
            push eax
            rep movsb
            call strchr
            mov ebp, eax
            add esp, 8
            test ebp, ebp
            jne L00494e62
    L00494f02:
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            lea eax, [esp + 0x14]
            mov dx, word ptr [ecx + 0x34]
            push edx
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push offset gUNK_00768f68+0x27d8
            mov edx, dword ptr [eax + 0x514]
            push edx
            mov edx, dword ptr [eax + 0x510]
            call DRPixelmapText
    L00494f30:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x204
            ret
    }
#endif
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00496b10
void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    int p2_x;
    int p2_y;
    tRace_checkpoint_spec* checkpoint;

    if (pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] < 0.f) {
            p2_x = 8;
            p2_y = (int)((checkpoint->map_position.v[1] - (float)gINT_0074abd0)
                * ((float)gINT_0074abd4 / ((float)gINT_0074abd4 - checkpoint->map_position.v[0]))
                + (float)gINT_0074abd0);
        } else {
            p2_x = (int)(checkpoint->map_position.v[0] + 8.f);
            p2_y = (int)checkpoint->map_position.v[1];
        }
        FancyDrawLine(pMap, gINT_0074abd4 + 8, gINT_0074abd0, p2_x, p2_y, 132, 100);
    }
}

// FUNCTION: CARMA2_HW 0x00495d00
void C2_HOOK_FASTCALL DrawOffsetCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    // GLOBAL: CARMA2_HW 0x0068d8f0
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d8f4
    static int flash_state;
    tRace_checkpoint_spec* checkpoint;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count
            && gTrack_version > 0 && pTarget == 0) || Flash(300, &last_flash, &flash_state)) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] >= 0.0f
                && checkpoint->map_position.v[0] < (float)pMap->width
                && checkpoint->map_position.v[1] >= 0.0f
                && checkpoint->map_position.v[1] < (float)pMap->height) {
            DRPixelmapRectangleMaskedCopy(pMap,
                (br_int_16)(int)(checkpoint->map_position.v[0] + 8.0f - (float)gCheckpoint_digit_center_x),
                (br_int_16)(int)(checkpoint->map_position.v[1] - (float)(int)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0,
                (br_int_16)(pCheckpoint * gCheckpoint_digit_height),
                gCheckpoint_numbers->width,
                (br_int_16)gCheckpoint_digit_height);
        }
    }
}

void C2_HOOK_FASTCALL DoDirectionFinderStuff(br_pixelmap* pMap) {
    tU32 the_time;
    const br_vector2* ptr_v;
    br_scalar tmp;
    br_vector2 tv;

    the_time = PDGetTotalTime();
    if ((gMini_map_visible || gMap_view == 2)
            && gCheckpoint_finder_enabled
            && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 ||
                gCurrent_race.race_spec->race_type == kRaceType_Checkpoints)) {
        DoSomeThingsToCheckpoints(pMap, the_time, CheckpointLine, 0);
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        if (gINT_0068d6f4 < 0) {
            return;
        }
        ptr_v = &gVector2_0068d8b0;
        tmp = gFLOAT_0074abc4;
    } else if ((gNet_mode != eNet_mode_none
            || gCurrent_race.race_spec->race_type == kRaceType_Carma1
            || gCurrent_race.race_spec->race_type == kRaceType_Cars) && gTarget_lock_car_2 != NULL) {
        ptr_v = &gVector2_0068d6d8;
        tmp = gFLOAT_0074ab90;
    } else {
        return;
    }
    if (tmp > gFLOAT_0074abb8) {
        BrMatrix23TApplyV(&tv, ptr_v, &gMatrix23_0068c880);
        tmp = sqrtf(tmp) - (float)gCurrent_graf_data->field_0x51c;
        BrVector2Normalise(&tv, &tv);
        if (fabsf(tmp * tv.v[0]) > (float)gINT_0074abd4) {
            tmp *= (float)gINT_0074abd4 / fabsf(tmp * tv.v[0]);
        }
        if (gMini_map_visible || gMap_view == 2) {
            FancyDrawLine(pMap,
                gINT_0074abd4 + 8, gINT_0074abd0,
                (int)((float)gINT_0074abd4 - tmp * tv.v[0] + 8.f),
                (int)((float)gINT_0074abd0 - tmp * tv.v[1]),
                52, 75);
        }
    }
}

void C2_HOOK_FASTCALL DoNearestOpponentRelativeheightMarkerStuff(br_pixelmap* pMap) {
    int half_height;
    int border_height;
    br_scalar delta_height;
    int ym;
    int yt;

    if ((gMini_map_visible || gMap_view == 2)
            && gTarget_lock_car_2 != NULL) {
//        y = (int)(((gPlayer_car_master_actor->t.t.translate.t.v[1] - gTarget_lock_car_2->pos.v[1]) *
//                (float)gCurrent_graf_data->field_0x518 * 0.5f * 0.46f);
        half_height = pMap->height / 2;
        border_height = (int)(pMap->height * 0.05f);
        BrPixelmapLine(pMap, 1, border_height, 3, border_height, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 1, pMap->height - border_height, 3, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 0, border_height + 1, 0, pMap->height - (border_height + 1), RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 4, border_height + 1, 4, pMap->height - (border_height + 1), RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 0, half_height, 4, half_height, RGB565_TO_BACKSCREEN_COLOUR(31, 62, 31));

        BrPixelmapPixelSet(pMap, 0, border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 4, border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 0, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 4, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));

        delta_height = gPlayer_car_master_actor->t.t.translate.t.v[1] - gTarget_lock_car_2->pos.v[1];
        if (fabsf(delta_height) > 14.9999997f / WORLD_SCALE) {
            delta_height = (float)sign((int)delta_height) * 14.9999997f / WORLD_SCALE;
        }
        ym = half_height;
        if (fabsf(delta_height * 0.46f) >= 0.05f) {
            ym = (int)((float)(half_height - border_height) * delta_height * 0.46f + (float)half_height);
        }
        yt = border_height + 1;
        if (ym > border_height) {
            yt = ym;
            if (ym >= pMap->height - border_height) {
                yt = pMap->height - border_height - 1;
            }
        }
        if (yt < half_height) {
            BrPixelmapLine(pMap, 1, yt, 1, half_height - 1, RGB565_TO_BACKSCREEN_COLOUR(0, 38, 31));
            BrPixelmapLine(pMap, 2, yt, 2, half_height - 1, RGB565_TO_BACKSCREEN_COLOUR(6, 0, 31));
            BrPixelmapLine(pMap, 3, yt, 3, half_height - 1, 0x15);
        } else if (yt > half_height) {
            BrPixelmapLine(pMap, 1, half_height + 1, 1, yt, RGB565_TO_BACKSCREEN_COLOUR(0, 38, 31));
            BrPixelmapLine(pMap, 2, half_height + 1, 2, yt, RGB565_TO_BACKSCREEN_COLOUR(6, 0, 31));
            BrPixelmapLine(pMap, 3, half_height + 1, 3, yt, 0x15);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004950b0
void C2_NAKED C2_HOOK_FASTCALL MapOverlay(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 the_time;
    br_uint_16 original_base_x;
    br_matrix23 mat23;
    br_vector3 player_pos;

    BrMatrix34ApplyP(&gOrigin_map, &gPlayer_car_master_actor->t.t.translate.t,  &gCurrent_race.map_transformation);
    gOrigin_headup_map.v[0] = gOrigin_map.v[0] - (float)(gINT_0074ab94 / 2);
    gOrigin_headup_map.v[1] = gOrigin_map.v[1] - (float)(gINT_0074abec / 2);
    gINT_0068c878 = (int)(gOrigin_headup_map.v[0] + .5);
    gINT_0068c874 = (int)(gOrigin_headup_map.v[1] + .5);
    if (gINT_0068c878 < 0 || gINT_0068c874 < 0
            || gINT_0068c878 > gCurrent_race.map_image->width
            || gINT_0068c874 > gCurrent_race.map_image->height) {
        BrPixelmapFill(gMap_overlay, 0);
    }
    if (gINT_0074ab94 > gINT_0074abec) {
        gINT_0068c858 = (gINT_0074ab94 - gINT_0074abec) / 2;
        gINT_0068d890 = 0;
        BrPixelmapRectangleCopy(gMap_overlay,
            0, 0,
            gCurrent_race.map_image,
            gINT_0068c878, gINT_0068c874 - gINT_0068c858,
            gINT_0074ab94, gINT_0074ab94);
    } else {
        gINT_0068c858 = 0;
        gINT_0068d890 = (gINT_0074abec - gINT_0074ab94) / 2;
        BrPixelmapRectangleCopy(gMap_overlay,
            0, 0,
            gCurrent_race.map_image,
            gINT_0068c878 - gINT_0068d890, gINT_0068c874,
            gINT_0074abec, gINT_0074abec);
    }
    gCurrent_race.map_transformation.m[3][0] -= gOrigin_headup_map.v[0] - (float)gINT_0068d890;
    gCurrent_race.map_transformation.m[3][1] -= gOrigin_headup_map.v[1] - (float)gINT_0068c858;
    BrMatrix34Mul(&gCar_in_map_space,
        &gProgram_state.current_car.car_master_actor->t.t.mat,
        &gCurrent_race.map_transformation);
    mat23.m[0][0] = gCar_in_map_space.m[0][0];
    mat23.m[0][1] = gCar_in_map_space.m[0][1];
    mat23.m[1][0] = gCar_in_map_space.m[2][0];
    mat23.m[1][1] = gCar_in_map_space.m[2][1];
    mat23.m[2][0] = 0.f;
    mat23.m[2][1] = 0.f;
    BrMatrix23LPNormalise(&gMatrix23_0068c880, &mat23);
    DoMapOverlays(gMap_overlay);
    gCurrent_race.map_transformation.m[3][0] += gOrigin_headup_map.v[0] - (float)gINT_0068d890;
    gCurrent_race.map_transformation.m[3][1] += gOrigin_headup_map.v[1] - (float)gINT_0068c858;
    the_time = PDGetTotalTime();
    PossibleLock(1);
    if (gMini_map_visible && gMap_overlay != NULL) {
        DRPixelmapRotatedAndFeatheredCopy(&gMatrix23_0068c880,
            gBack_screen,
            gHeadup_map_x + 8, gHeadup_map_y,
            gMap_overlay,
            gINT_0068d890 + gINT_0074ab8c, gINT_0068c858 + gINT_0074ab88,
            gHeadup_map_w, gHeadup_map_h,
            gMap_trans);
    }
    InitMap();

    original_base_x = gMini_map->base_x;
    gMini_map->base_x = gMini_map->base_x / 2;
    DoDirectionFinderStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    if (gMini_map_visible) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1
                || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {

            DoSomeThingsToCheckpoints(gMini_map, the_time, DrawOffsetCheckpoint, 1);
        }
        if (gMini_map->type == BR_PMT_INDEX_8) {
            gMini_map->base_x = 0;
        } else {
            gMini_map->base_x /= 2;
        }
        player_pos.v[0] = (float)(gHeadup_map_w / 2 + gHeadup_map_x + 8);
        player_pos.v[1] = (float)(gHeadup_map_h / 2 + gHeadup_map_y);
        player_pos.v[2] = gMini_map_arrow_z;
        DrawArrow(gBack_screen, 0, &player_pos, CarArrowColour(&gProgram_state.current_car, eVehicle_self));
    }
    DoNearestOpponentRelativeheightMarkerStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    PossibleUnlock(1);
    if (gHeadup_detail_level > 2) {
        DoOpponentStatusHeadup();
    }
#else
    __asm {
            sub esp, 0x38
            mov eax, dword ptr [gPlayer_car_master_actor]
            push ebx
            push ebp
            push esi
            push edi
            add eax, 0x50
            push offset gCurrent_race+0xed4
            push eax
            push offset gOrigin_map
            call BrMatrix34ApplyP
            mov edi, dword ptr [gINT_0074ab94]
            mov ebx, dword ptr [gINT_0074abec]
            mov eax, edi
            add esp, 0xc
            cdq
            sub eax, edx
            sar eax, 1
            mov dword ptr [esp + 0x10], eax
            mov eax, ebx
            fild dword ptr [esp + 0x10]
            cdq
            fsubr dword ptr [gOrigin_map]
            sub eax, edx
            sar eax, 1
            fstp dword ptr [gOrigin_headup_map]
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fsubr dword ptr [gOrigin_map+0x4]
            fstp dword ptr [gOrigin_headup_map+0x4]
            fld dword ptr [gOrigin_headup_map]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            call _ftol
            fld dword ptr [gOrigin_headup_map+0x4]
            fsub qword ptr [gDouble_0058a068+0x2e0]
            mov esi, eax
            mov dword ptr [gINT_0068c878], esi
            call _ftol
            mov ecx, eax
            test esi, esi
            mov dword ptr [gINT_0068c874], ecx
            jl L00495168
            test ecx, ecx
            jl L00495168
            mov ebp, dword ptr [gCurrent_race+0xf04]
            xor edx, edx
            mov dx, word ptr [ebp + 0x34]
            cmp esi, edx
            jg L00495168
            xor eax, eax
            mov ax, word ptr [ebp + 0x36]
            cmp ecx, eax
            jle L00495197
    L00495168:
            mov ecx, dword ptr [gMap_overlay]
            push 0
            push ecx
            call BrPixelmapFill
            mov ebp, dword ptr [gCurrent_race+0xf04]
            mov edi, dword ptr [gINT_0074ab94]
            mov ebx, dword ptr [gINT_0074abec]
            mov esi, dword ptr [gINT_0068c878]
            mov ecx, dword ptr [gINT_0068c874]
            add esp, 8
    L00495197:
            cmp edi, ebx
            jle L004951c7
            mov eax, edi
            push edi
            sub eax, ebx
            push edi
            cdq
            sub eax, edx
            mov edx, dword ptr [gMap_overlay]
            sar eax, 1
            sub ecx, eax
            mov dword ptr [gINT_0068d890], 0
            push ecx
            push esi
            push ebp
            push 0
            push 0
            mov dword ptr [gINT_0068c858], eax
            push edx
            jmp L004951f0
    L004951c7:
            mov eax, ebx
            push ebx
            sub eax, edi
            push ebx
            cdq
            sub eax, edx
            push ecx
            sar eax, 1
            sub esi, eax
            mov dword ptr [gINT_0068d890], eax
            mov eax, dword ptr [gMap_overlay]
            push esi
            push ebp
            push 0
            push 0
            mov dword ptr [gINT_0068c858], 0
            push eax
    L004951f0:
            call BrPixelmapRectangleCopy
            fild dword ptr [gINT_0068c858]
            fild dword ptr [gINT_0068d890]
            mov ecx, dword ptr [gProgram_state+0xbc]
            add esp, 0x20
            add ecx, 0x2c
            fsubr dword ptr [gOrigin_headup_map]
            fxch st(1)
            fsubr dword ptr [gOrigin_headup_map+0x4]
            fxch st(1)
            fsubr dword ptr [gCurrent_race+0xef8]
            fxch st(1)
            fsubr dword ptr [gCurrent_race+0xefc]
            fxch st(1)
            fstp dword ptr [gCurrent_race+0xef8]
            push offset gCurrent_race+0xed4
            push ecx
            fstp dword ptr [gCurrent_race+0xefc]
            push offset gCar_in_map_space
            call BrMatrix34Mul
            fld dword ptr [gCar_in_map_space]
            fld dword ptr [gCar_in_map_space+0x4]
            fld dword ptr [gCar_in_map_space+0x18]
            fld dword ptr [gCar_in_map_space+0x1c]
            fld dword ptr [gDouble_0058a068+0x2e8]
            fld dword ptr [gDouble_0058a068+0x2e8]
            fxch st(5)
            fstp dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x40]
            fxch st(1)
            fstp dword ptr [esp + 0x44]
            add esp, 0xc
            lea edx, [esp + 0x30]
            fstp dword ptr [esp + 0x3c]
            push edx
            push offset gMatrix23_0068c880
            fstp dword ptr [esp + 0x48]
            fstp dword ptr [esp + 0x4c]
            call BrMatrix23LPNormalise
            mov ecx, dword ptr [gMap_overlay]
            add esp, 8
            call DoMapOverlays
            fild dword ptr [gINT_0068c858]
            fild dword ptr [gINT_0068d890]
            fsubr dword ptr [gOrigin_headup_map]
            fxch st(1)
            fsubr dword ptr [gOrigin_headup_map+0x4]
            fxch st(1)
            fadd dword ptr [gCurrent_race+0xef8]
            fxch st(1)
            fadd dword ptr [gCurrent_race+0xefc]
            fxch st(1)
            fstp dword ptr [gCurrent_race+0xef8]
            fstp dword ptr [gCurrent_race+0xefc]
            call PDGetTotalTime
            mov ecx, 1
            mov edi, eax
            call PossibleLock
            mov eax, dword ptr [gMini_map_visible]
            test eax, eax
            je L0049535d
            mov eax, dword ptr [gMap_overlay]
            test eax, eax
            je L0049535d
            mov ecx, dword ptr [gMap_trans]
            mov dx, word ptr [gHeadup_map_h]
            push ecx
            mov cx, word ptr [gHeadup_map_w]
            push edx
            mov edx, dword ptr [gINT_0068c858]
            push ecx
            mov ecx, dword ptr [gINT_0074ab88]
            add edx, ecx
            mov ecx, dword ptr [gINT_0074ab8c]
            push edx
            mov edx, dword ptr [gINT_0068d890]
            add edx, ecx
            mov ecx, offset gMatrix23_0068c880
            push edx
            mov dx, word ptr [gHeadup_map_y]
            push eax
            mov eax, dword ptr [gHeadup_map_x]
            add eax, 8
            push edx
            mov edx, dword ptr [gBack_screen]
            push eax
            call DRPixelmapRotatedAndFeatheredCopy
    L0049535d:
            call InitMap
            mov eax, dword ptr [gMini_map]
            mov bx, word ptr [eax + 0x30]
            mov cx, bx
            mov dword ptr [esp + 0x20], ebx
            shr cx, 1
            mov word ptr [eax + 0x30], cx
            mov esi, dword ptr [gMini_map]
            call PDGetTotalTime
            mov ecx, dword ptr [gMini_map_visible]
            test ecx, ecx
            jne L00495397
            cmp dword ptr [gMap_view], 2
            jne L004953c6
    L00495397:
            mov ecx, dword ptr [gCheckpoint_finder_enabled]
            test ecx, ecx
            je L004953c6
            mov edx, dword ptr [gCurrent_race+0x98]
            mov ecx, dword ptr [edx + 0x210]
            test ecx, ecx
            je L004953b6
            cmp ecx, 3
            jne L004953c6
    L004953b6:
            push 0
            push offset CheckpointLine
            mov edx, eax
            mov ecx, esi
            call DoSomeThingsToCheckpoints
    L004953c6:
            mov eax, dword ptr [gCurrent_race+0x98]
            mov eax, dword ptr [eax + 0x210]
            cmp eax, 4
            jl L004953f0
            mov eax, dword ptr [gINT_0068d6f4]
            test eax, eax
            jl L00495549
            fld dword ptr [gFLOAT_0074abc4]
            mov ecx, offset gVector2_0068d8b0
            jmp L0049541f
    L004953f0:
            mov ecx, dword ptr [gNet_mode]
            test ecx, ecx
            jne L00495407
            test eax, eax
            je L00495407
            cmp eax, 1
            jne L00495549
    L00495407:
            mov eax, dword ptr [gTarget_lock_car_2]
            test eax, eax
            je L00495549
            fld dword ptr [gFLOAT_0074ab90]
            mov ecx, offset gVector2_0068d6d8
    L0049541f:
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0074abb8]
            fnstsw ax
            test ah, 0x41
            jne L00495549
            push offset gMatrix23_0068c880
            lea edx, [esp + 0x1c]
            push ecx
            push edx
            call BrMatrix23TApplyV
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fsqrt
            mov eax, dword ptr [gCurrent_graf_data]
            add esp, 0xc
            fxch st(2)
            faddp st(1), st(0)
            fild dword ptr [eax + 0x51c]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fsqrt
            fxch st(1)
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gDouble_0058a068+0x2ec]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fsubp st(1), st(0)
            fnstsw ax
            test ah, 0x41
            jne L004954b3
            fld dword ptr [esp + 0x10]
            fdivr qword ptr [gDouble_0058a068+0x2f0]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            jmp L004954bf
    L004954b3:
            fld dword ptr [gDouble_0058a068+0x2f8]
            fld dword ptr [gDouble_0058a068+0x2e8]
    L004954bf:
            fld st(2)
            fmul st(0), st(2)
            fild dword ptr [gINT_0074abd4]
            fxch st(1)
            fabs
            fxch st(1)
            fld st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L004954e0
            fdivrp st(1), st(0)
            fmulp st(3), st(0)
            jmp L004954e4
    L004954e0:
            fstp st(0)
            fstp st(0)
    L004954e4:
            mov eax, dword ptr [gMini_map_visible]
            test eax, eax
            jne L004954f6
            cmp dword ptr [gMap_view], 2
            jne L00495543
    L004954f6:
            fild dword ptr [gINT_0074abd0]
            fld st(3)
            fmul st(0), st(2)
            push 0x4b
            push 0x34
            fsubp st(1), st(0)
            call _ftol
            fstp st(0)
            fild dword ptr [gINT_0074abd4]
            fxch st(2)
            fmul st(0), st(1)
            push eax
            fsubp st(2), st(0)
            fxch st(1)
            fsub dword ptr [gDouble_0058a068+0x2fc]
            call _ftol
            mov ecx, dword ptr [gINT_0074abd0]
            mov edx, dword ptr [gINT_0074abd4]
            push eax
            push ecx
            add edx, 8
            mov ecx, esi
            fstp st(0)
            call FancyDrawLine
            jmp L00495549
    L00495543:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L00495549:
            mov eax, dword ptr [gMini_map]
            mov word ptr [eax + 0x30], bx
            mov eax, dword ptr [gMini_map_visible]
            test eax, eax
            je L00495645
            mov ecx, dword ptr [gCurrent_race+0x98]
            mov eax, dword ptr [ecx + 0x210]
            test eax, eax
            je L00495574
            cmp eax, 3
            jne L00495588
    L00495574:
            mov ecx, dword ptr [gMini_map]
            push 1
            push offset DrawOffsetCheckpoint
            mov edx, edi
            call DoSomeThingsToCheckpoints
    L00495588:
            mov eax, dword ptr [gMini_map]
            cmp byte ptr [eax + 0x2c], 3
            jne L0049559b
            mov word ptr [eax + 0x30], 0
            jmp L0049559f
    L0049559b:
            shr word ptr [eax + 0x30], 1
    L0049559f:
            mov eax, dword ptr [gHeadup_map_w]
            mov ecx, dword ptr [gHeadup_map_y]
            cdq
            sub eax, edx
            mov edx, dword ptr [gHeadup_map_x]
            sar eax, 1
            lea eax, [eax + edx + 8]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [gHeadup_map_h]
            fild dword ptr [esp + 0x14]
            cdq
            fstp dword ptr [esp + 0x24]
            sub eax, edx
            sar eax, 1
            add eax, ecx
            mov ecx, dword ptr [gMini_map_arrow_z]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [gNet_mode]
            fild dword ptr [esp + 0x14]
            test eax, eax
            mov dword ptr [esp + 0x2c], ecx
            fstp dword ptr [esp + 0x28]
            je L0049561f
            mov eax, dword ptr [gProgram_state+0x130]
            test eax, eax
            je L0049561f
            mov ecx, offset gProgram_state+0xac
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            je L00495612
            or eax, 0xffffffff
            jmp L00495632
    L00495612:
            mov edx, dword ptr [gProgram_state+0x130]
            xor eax, eax
            mov al, byte ptr [edx + 0x3c]
            jmp L00495632
    L0049561f:
            mov eax, dword ptr [gProgram_state+0x280]
            mov ecx, dword ptr [gCar_map_colours]
            neg eax
            sbb eax, eax
            not eax
            and eax, ecx
    L00495632:
            mov ecx, dword ptr [gBack_screen]
            push eax
            lea eax, [esp + 0x28]
            xor edx, edx
            push eax
            call DrawArrow
    L00495645:
            mov eax, dword ptr [gMini_map_visible]
            mov esi, dword ptr [gMini_map]
            test eax, eax
            jne L00495661
            cmp dword ptr [gMap_view], 2
            jne L004959c5
    L00495661:
            mov eax, dword ptr [gTarget_lock_car_2]
            test eax, eax
            je L004959c5
            mov ecx, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [ecx + 0x518]
            mov ecx, dword ptr [gPlayer_car_master_actor]
            mov dword ptr [esp + 0x14], edx
            fld dword ptr [ecx + 0x54]
            fsub dword ptr [eax + 0x50]
            fimul dword ptr [esp + 0x14]
            fmul dword ptr [gDouble_0058a068+0x300]
            fmul qword ptr [gDouble_0058a068+0x308]
            call _ftol
            mov dword ptr [esp + 0x18], eax
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x14]
            shr eax, 1
            mov ebp, eax
            fmul qword ptr [gDouble_0058a068+0x310]
            mov dword ptr [esp + 0x10], ebp
            call _ftol
            mov edx, dword ptr [gBack_screen]
            mov edi, eax
            mov al, byte ptr [edx + 0x2c]
            sub al, 5
            neg al
            sbb eax, eax
            and eax, 0xffffe120
            add eax, 0x3dc7
            push eax
            push edi
            push 3
            push edi
            push 1
            push esi
            call BrPixelmapLine
            mov ecx, dword ptr [gBack_screen]
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            add esp, 0x18
            mov dl, byte ptr [ecx + 0x2c]
            sub eax, edi
            sub dl, 5
            neg dl
            sbb edx, edx
            and edx, 0xffffe120
            add edx, 0x3dc7
            push edx
            push eax
            push 3
            push eax
            push 1
            push esi
            call BrPixelmapLine
            mov eax, dword ptr [gBack_screen]
            add esp, 0x18
            lea ebx, [edi + 1]
            mov cl, byte ptr [eax + 0x2c]
            mov dword ptr [esp + 0x18], ebx
            sub cl, 5
            neg cl
            sbb ecx, ecx
            xor edx, edx
            mov dx, word ptr [esi + 0x36]
            and ecx, 0xffffe120
            sub edx, edi
            add ecx, 0x3dc7
            dec edx
            push ecx
            push edx
            push 0
            push ebx
            push 0
            push esi
            call BrPixelmapLine
            mov eax, dword ptr [gBack_screen]
            add esp, 0x18
            mov cl, byte ptr [eax + 0x2c]
            sub cl, 5
            neg cl
            sbb ecx, ecx
            xor edx, edx
            and ecx, 0xffffe120
            mov dx, word ptr [esi + 0x36]
            add ecx, 0x3dc7
            sub edx, edi
            push ecx
            dec edx
            push edx
            push 4
            push ebx
            push 4
            push esi
            call BrPixelmapLine
            mov eax, dword ptr [gBack_screen]
            add esp, 0x18
            mov cl, byte ptr [eax + 0x2c]
            sub cl, 5
            neg cl
            sbb ecx, ecx
            and ecx, 0xffff8020
            add ecx, 0xffdf
            push ecx
            push ebp
            push 4
            push ebp
            push 0
            push esi
            call BrPixelmapLine
            mov edx, dword ptr [gBack_screen]
            add esp, 0x18
            mov al, byte ptr [edx + 0x2c]
            sub al, 5
            neg al
            sbb eax, eax
            and eax, 0xffffe560
            add eax, 0x3546
            push eax
            push edi
            push 0
            push esi
            call BrPixelmapPixelSet
            mov ecx, dword ptr [gBack_screen]
            add esp, 0x10
            mov dl, byte ptr [ecx + 0x2c]
            sub dl, 5
            neg dl
            sbb edx, edx
            and edx, 0xffffe560
            add edx, 0x3546
            push edx
            push edi
            push 4
            push esi
            call BrPixelmapPixelSet
            mov eax, dword ptr [gBack_screen]
            add esp, 0x10
            mov cl, byte ptr [eax + 0x2c]
            sub cl, 5
            neg cl
            sbb ecx, ecx
            xor edx, edx
            mov dx, word ptr [esi + 0x36]
            and ecx, 0xffffe560
            add ecx, 0x3546
            sub edx, edi
            push ecx
            push edx
            push 0
            push esi
            call BrPixelmapPixelSet
            mov eax, dword ptr [gBack_screen]
            add esp, 0x10
            mov cl, byte ptr [eax + 0x2c]
            sub cl, 5
            neg cl
            sbb ecx, ecx
            and ecx, 0xffffe560
            add ecx, 0x3546
            xor edx, edx
            mov dx, word ptr [esi + 0x36]
            push ecx
            sub edx, edi
            push edx
            push 4
            push esi
            call BrPixelmapPixelSet
            mov eax, dword ptr [gPlayer_car_master_actor]
            mov ecx, dword ptr [gTarget_lock_car_2]
            add esp, 0x10
            fld dword ptr [eax + 0x54]
            fsub dword ptr [ecx + 0x50]
            fld st(0)
            fabs
            fcomp qword ptr [gDouble_0058a068+0x318]
            fnstsw ax
            test ah, 0x41
            jne L004958a8
            call _ftol
            mov ecx, eax
            call sign
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x14]
            fmul qword ptr [gDouble_0058a068+0x318]
    L004958a8:
            fmul qword ptr [gDouble_0058a068+0x308]
            fld st(0)
            fabs
            fcomp qword ptr [gDouble_0058a068+0x310]
            fnstsw ax
            test ah, 1
            je L004958c5
            fstp st(0)
            mov ebx, ebp
            jmp L004958e0
    L004958c5:
            mov edx, ebp
            sub edx, edi
            mov dword ptr [esp + 0x14], edx
            fild dword ptr [esp + 0x14]
            fmul st(0), st(1)
            fiadd dword ptr [esp + 0x10]
            call _ftol
            fstp st(0)
            mov ebx, eax
    L004958e0:
            cmp ebx, edi
            jg L004958ea
            mov ebx, dword ptr [esp + 0x18]
            jmp L004958f9
    L004958ea:
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            sub eax, edi
            cmp ebx, eax
            jl L004958f9
            lea ebx, [eax - 1]
    L004958f9:
            cmp ebx, ebp
            jge L0049595a
            mov eax, dword ptr [gBack_screen]
            dec ebp
            mov cl, byte ptr [eax + 0x2c]
            sub cl, 5
            neg cl
            sbb ecx, ecx
            and ecx, 0xfffffda0
            add ecx, 0x4df
            push ecx
            push ebp
            push 1
            push ebx
            push 1
            push esi
            call BrPixelmapLine
            mov edx, dword ptr [gBack_screen]
            add esp, 0x18
            mov al, byte ptr [edx + 0x2c]
            sub al, 5
            neg al
            sbb eax, eax
            and eax, 0xffffe800
            add eax, 0x301f
            push eax
            push ebp
            push 2
            push ebx
            push 2
            push esi
            call BrPixelmapLine
            add esp, 0x18
            push 0x15
            push ebp
            push 3
            push ebx
            jmp L004959ba
    L0049595a:
            jle L004959c5
            mov ecx, dword ptr [gBack_screen]
            inc ebp
            mov dl, byte ptr [ecx + 0x2c]
            sub dl, 5
            neg dl
            sbb edx, edx
            and edx, 0xfffffda0
            add edx, 0x4df
            push edx
            push ebx
            push 1
            push ebp
            push 1
            push esi
            call BrPixelmapLine
            mov eax, dword ptr [gBack_screen]
            add esp, 0x18
            mov cl, byte ptr [eax + 0x2c]
            sub cl, 5
            neg cl
            sbb ecx, ecx
            and ecx, 0xffffe800
            add ecx, 0x301f
            push ecx
            push ebx
            push 2
            push ebp
            push 2
            push esi
            call BrPixelmapLine
            add esp, 0x18
            push 0x15
            push ebx
            push 3
            push ebp
    L004959ba:
            push 3
            push esi
            call BrPixelmapLine
            add esp, 0x18
    L004959c5:
            mov eax, dword ptr [gMini_map]
            mov dx, word ptr [esp + 0x20]
            mov ecx, 1
            mov word ptr [eax + 0x30], dx
            call PossibleUnlock
            cmp dword ptr [gHeadup_detail_level], 2
            jle L004959eb
            call DoOpponentStatusHeadup
    L004959eb:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    }
#endif
}

void C2_HOOK_FASTCALL FinishMap(void) {

    gBack_screen->origin_x = gBack_original_origin_x;
    gBack_screen->origin_y = gBack_original_origin_y;
    gBack_screen->base_x = gBack_original_base_x;
    gBack_screen->base_y = gBack_original_base_y;
}

// FUNCTION: CARMA2_HW 0x00496d80
void C2_HOOK_FASTCALL MapStuffAfterRender(void) {
    if (gCurrent_race.map_image == NULL) {
        return;
    }
    if (gMap_view == 2) {
        BrVector2Set(&gOrigin_headup_map, 0.f, 0.f);
        DoMapOverlays(gBack_screen);
        FinishMap();
    }
    else if (gMap_view == 1) {
      if (!gPed_676914
                && !(gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox)) {

            MapOverlay();
        }
    }
}
void C2_HOOK_FASTCALL LoadHeadupMessageFile(void) {
    FILE* f;
    tPath_name path;
    int i;
    int count;

    PathCat(path, gApplication_path, "SCROLLERS.TXT");
    f = DRfopen(path, "rt");
    if (f == NULL) {
        BrFailure("Couldn't open ", path);
    }
    /*
     * # of Headup scrolly messages
     * Format:  Font index, Message string
     * N.B use underscore "_" where you want a space in the string
     **/
    count = GetAnInt(f);

    C2_HOOK_BUG_ON(sizeof(tHud_message) != 0x88);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gHud_messages) != 46);

    if (count > CARPOCALYPSE2_ASIZE(gHud_messages)) {
        BrFailure("Error - Too many Headup messages ", path);
    }
    for (i = 0; i < count; i++) {
        char s[256];
        char *str;
        tHud_message *hud_message;
        int len;
        int j;

        hud_message = &gHud_messages[i];
        GetALineAndDontArgue(f, s);

        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &hud_message->font1);

        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &hud_message->font2);

        str = strtok(NULL, "\t ,/");
        strcpy(hud_message->message, str);

        len = strlen(hud_message->message);
        for (j = 0; j < len; j++) {
            if (hud_message->message[j] == '_') {
                hud_message->message[j] = ' ';
            }
        }
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00449090
void C2_HOOK_FASTCALL InitHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gHeadups) != 37);
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gHeadups); i++) {
        gHeadups[i].type = eHeadup_unused;
    }

    gBR_fonts[0] = BrFontProp4x6;
    gBR_fonts[1] = BrFontProp7x9;
    gBR_fonts[2] = gFont_7;
    gBR_fonts[3] = gHeadup_font;
    gRevs[ 0] = LoadPixelmap("rev1.tif");
    gRevs[ 1] = LoadPixelmap("rev2.tif");
    gRevs[ 2] = LoadPixelmap("rev3.tif");
    gRevs[ 3] = LoadPixelmap("rev4.tif");
    gRevs[ 4] = LoadPixelmap("rev5.tif");
    gRevs[ 5] = LoadPixelmap("rev6.tif");
    gRevs[ 6] = LoadPixelmap("rev7.tif");
    gRevs[ 7] = LoadPixelmap("rev8.tif");
    gRevs[ 8] = LoadPixelmap("rev9.tif");
    gRevs[ 9] = LoadPixelmap("rev10.tif");
    gRevs[10] = LoadPixelmap("rev11.tif");
    gRevs[11] = LoadPixelmap("rev12.tif");
    gRevs[12] = LoadPixelmap("rev13.tif");
    gPowerbar = LoadPixelmap("powerbar.tif");

    gArmour_colour_map = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        gPowerbar->width, gPowerbar->height,
        NULL, 0);
    gArmour_colour_map->identifier = BrResStrDup(gArmour_colour_map, "Armour_area");
    BrMapAdd(gArmour_colour_map);

    gPower_colour_map = LoadPixelmap("powerbar.tif");
    BrMapAdd(gPower_colour_map);

    gOffensive_colour_map = LoadPixelmap("powerbar.tif");
    BrMapAdd(gOffensive_colour_map);

    gIcon_test = LoadPixelmap("icontest.tif");

    gCurrent_rev = gRevs[0];
    BrMapAdd(gRevs[0]);

    gHud_curve1 = LoadPixelmap("hudcurve1.tif");
    BrMapAdd(gHud_curve1);
    gHud_curve2 = LoadPixelmap("hudcurve2.tif");
    BrMapAdd(gHud_curve2);
    gHud_curve3 = LoadPixelmap("hudcurve3.tif");
    BrMapAdd(gHud_curve3);

    gGrey_top5 = LoadPixelmap("greytop5.tif");
    gDamage_hud = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        gGrey_top5->width, gGrey_top5->height,
        NULL, 0);
    BrMapAdd(gDamage_hud);
    BrMapUpdate(gDamage_hud, BR_MAPU_ALL);

    gStatbarHUD5_material->colour_map = gDamage_hud;
    BrMaterialUpdate(gStatbarHUD5_material, BR_MATU_COLOURMAP);

    gTimer_lhs = LoadPixelmap("timerlhs.tif");
    BrMapAdd(gTimer_lhs);
    gTimerLeftHUD_material->colour_map = gTimer_lhs;
    BrMaterialUpdate(gTimerLeftHUD_material, BR_MATU_COLOURMAP);
    gTimerLeftHUD_model->vertices[1].p.v[0] = (float)gTimerLeftHUD_dim_x;
    gTimerLeftHUD_model->vertices[0].p.v[0] = gTimerLeftHUD_model->vertices[1].p.v[0];
    gTimerLeftHUD_model->vertices[3].p.v[1] = -(float)gTimerLeftHUD_dim_y;
    gTimerLeftHUD_model->vertices[0].p.v[1] = gTimerLeftHUD_model->vertices[3].p.v[1];
    gTimerLeftHUD_model->vertices[3].p.v[0] = (float)gTimerLeftHUD_dim_w + gTimerLeftHUD_model->vertices[1].p.v[0];
    gTimerLeftHUD_model->vertices[2].p.v[0] = gTimerLeftHUD_model->vertices[3].p.v[0];
    gTimerLeftHUD_model->vertices[2].p.v[1] = gTimerLeftHUD_model->vertices[3].p.v[1] - (float)gTimerLeftHUD_dim_h;
    gTimerLeftHUD_model->vertices[1].p.v[1] = gTimerLeftHUD_model->vertices[2].p.v[1];
    BrModelUpdate(gTimerLeftHUD_model, BR_MODU_VERTEX_POSITIONS);

    gHud_timer_rhs = LoadPixelmap("timerrhs.tif");
    BrMapAdd(gHud_timer_rhs);
    gTimerRightHUD_material->colour_map = gHud_timer_rhs;
    BrMaterialUpdate(gTimerRightHUD_material, BR_MATU_COLOURMAP);

    gTimerRightHUD_model->vertices[1].p.v[0] = (float)gTimerRightHUD_dim_x;
    gTimerRightHUD_model->vertices[0].p.v[0] = gTimerRightHUD_model->vertices[1].p.v[0];
    gTimerRightHUD_model->vertices[3].p.v[1] = -(float)gTimerRightHUD_dim_y;
    gTimerRightHUD_model->vertices[0].p.v[1] = gTimerRightHUD_model->vertices[3].p.v[1];
    gTimerRightHUD_model->vertices[3].p.v[0] = (float)gTimerRightHUD_dim_w + gTimerRightHUD_model->vertices[1].p.v[0];
    gTimerRightHUD_model->vertices[2].p.v[0] = gTimerRightHUD_model->vertices[3].p.v[0];
    gTimerRightHUD_model->vertices[2].p.v[1] = gTimerRightHUD_model->vertices[3].p.v[1] - (float)gTimerRightHUD_dim_h;
    gTimerRightHUD_model->vertices[1].p.v[1] = gTimerRightHUD_model->vertices[2].p.v[1];
    BrModelUpdate(gTimerRightHUD_model, BR_MODU_VERTEX_POSITIONS);

    gIcon_armour = LoadPixelmap("iconarmour.tif");
    gIcon_power = LoadPixelmap("iconpower.tif");
    gIcon_offense = LoadPixelmap("iconoffense.tif");
    gIcon_greyBloc1 = LoadPixelmap("greybloc1.tif");
    gIcon_litBloc1 = LoadPixelmap("litbloc1.tif");
    gIcon_grnBlock1 = LoadPixelmap("grnblock1.tif");

    {
        FILE* f;
        tPath_name path;
        int i;
        int count;

        PathCat(path, gApplication_path, "SCROLLERS.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            BrFailure("Couldn't open ", path);
        }
        /*
         * # of Headup scrolly messages
         * Format:  Font index, Message string
         * N.B use underscore "_" where you want a space in the string
         **/
        count = GetAnInt(f);

        C2_HOOK_BUG_ON(sizeof(tHud_message) != 0x88);
        C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gHud_messages) != 46);

        if (count > (int)CARPOCALYPSE2_ASIZE(gHud_messages)) {
            BrFailure("Error - Too many Headup messages ", path);
        }
        for (i = 0; i < count; i++) {
            char s[256];
            char *str;
            tHud_message *hud_message;
            int len;
            int j;

            hud_message = &gHud_messages[i];
            GetALineAndDontArgue(f, s);

            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &hud_message->font1);

            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &hud_message->font2);

            str = strtok(NULL, "\t ,/");
            strcpy(hud_message->message, str);

            len = strlen(hud_message->message);
            for (j = 0; j < len; j++) {
                if (hud_message->message[j] == '_') {
                    hud_message->message[j] = ' ';
                }
            }
        }
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x00449630
void C2_HOOK_FASTCALL ClearHeadup(int pIndex) {
    gHeadups[pIndex].type = eHeadup_unused;
}

// FUNCTION: CARMA2_HW 0x00449650
void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index) {
    int i;
    tHeadup* the_headup;

    the_headup = gHeadups;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gHeadups); i++, the_headup++) {
        if (the_headup->type != eHeadup_unused && the_headup->slot_index == pSlot_index) {
            ClearHeadup(i);
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00449690
void C2_HOOK_FASTCALL ClearHeadups(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gHeadups); i++) {
        if (gHeadups[i].type != eHeadup_unused) {
            ClearHeadup(i);
        }
    }
    gLast_fancy_index = -1;
    gLast_credit_headup__displays = -1;
    gLast_time_credit_headup = -1;
    gLast_earn_time = 0;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOld_times); i++) {
        gOld_times[i] = 0;
    }
    gLast_fancy_time = 0;
    gLast_time_earn_time = 0;
    gQueued_headup_count = 0;
    gLast_centre_headup = 0;
}

// FUNCTION: CARMA2_HW 0x00449fa0
int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    the_headup = gHeadups;
    empty_one = -1;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gHeadups); i++, the_headup++) {
        if (pSlot_index >= 0 && the_headup->slot_index == pSlot_index) {
            return i;
        } else if (the_headup->type == eHeadup_unused) {
            empty_one = i;
        }
    }
    return empty_one;
}

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {

    C2_HOOK_BUG_ON(sizeof(tQueued_headup) != 0x10c);

    gQueued_headup_count -= 1;
    memmove(&gQueued_headups[0], &gQueued_headups[1], gQueued_headup_count * sizeof(tQueued_headup));
}

// FUNCTION: CARMA2_HW 0x004497b0
void C2_NAKED C2_HOOK_FASTCALL ClearQueuedHeadups(void) {
#ifndef CARPOCALYPSE2_MATCHING
    while (gQueued_headup_count != 0) {
        KillOldestQueuedHeadup();
    }
#else
    __asm {
            mov eax, dword ptr [gQueued_headup_count]
            push ebx
            mov ecx, eax
            push esi
            shl ecx, 5
            add ecx, eax
            push edi
            lea edx, [eax + ecx*2]
            shl edx, 2
            test eax, eax
            je L004497ee
    L004497c9:
            sub edx, 0x10c
            mov esi, offset gQueued_headups+0x10c
            mov ecx, edx
            mov edi, offset gQueued_headups
            mov ebx, ecx
            dec eax
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            test eax, eax
            rep movsb
            jne L004497c9
    L004497ee:
            pop edi
            pop esi
            mov dword ptr [gQueued_headup_count], eax
            pop ebx
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

// FUNCTION: CARMA2_HW 0x00449830
void C2_NAKED C2_HOOK_FASTCALL DoTestPowerHeadup(int pLevel, const char* pAPO_str) {
#ifndef CARPOCALYPSE2_MATCHING
    int changed;
    br_pixelmap* dest_pm;
    br_pixelmap* icon;
    br_pixelmap* block;
    int count_slots;
    int level;
    int i;
    int j;
    int delta_x = 2;

    switch (pAPO_str[0]) {
    case 'A':
        dest_pm = gArmour_colour_map;
        icon = gIcon_armour;
        count_slots = gProgram_state.current_car.power_up_slots[0];
        changed = pLevel != gPrev_ps_apo_levels[0] || count_slots != gPrev_ps_drawn_levels[0];
        gPS_apo_level_changed[0] = changed;
        gPrev_ps_apo_levels[0] = pLevel;
        gPrev_ps_drawn_levels[0] = count_slots;
        break;
    case 'P':
        dest_pm = gPower_colour_map;
        icon = gIcon_power;
        count_slots = gProgram_state.current_car.power_up_slots[1];
        changed = pLevel != gPrev_ps_apo_levels[1] || count_slots != gPrev_ps_drawn_levels[1];
        gPS_apo_level_changed[1] = changed;
        gPrev_ps_apo_levels[1] = pLevel;
        gPrev_ps_drawn_levels[1] = count_slots;
        break;
    case 'O':
        dest_pm = gOffensive_colour_map;
        icon = gIcon_offense;
        count_slots = gProgram_state.current_car.power_up_slots[2];
        /* Offense redraws use the cached power slot count. */
        changed = pLevel != gPrev_ps_apo_levels[2] || count_slots != gPrev_ps_drawn_levels[1];
        gPS_apo_level_changed[2] = changed;
        gPrev_ps_drawn_levels[2] = count_slots;
        gPrev_ps_apo_levels[2] = pLevel;
        break;
    default:
        return;
    }

    if (changed) {
        DRPixelmapCopy(dest_pm, gPowerbar);
        DRPixelmapRectangleMaskedCopy(dest_pm, (br_int_16)delta_x, 0, icon, 0, 0,
            (br_int_16)icon->width, (br_int_16)icon->height);
        count_slots -= pLevel;
        level = pLevel;
        for (i = 0; i < 15; i += 5) {
            for (j = 0; j < 10; ++j) {
                if (level > 0) {
                    block = gIcon_litBloc1;
                } else if (count_slots > 0) {
                    block = gIcon_grnBlock1;
                } else {
                    block = gIcon_greyBloc1;
                }
                DRPixelmapRectangleMaskedCopy(
                    dest_pm,
                    (br_int_16)(j * block->width + delta_x + icon->width + 2),
                    (br_int_16)i,
                    block, 0, 0, (br_int_16)block->width, (br_int_16)block->height);
                if (level > 0) {
                    --level;
                } else if (count_slots > 0) {
                    --count_slots;
                }
            }
            delta_x -= 2;
        }

        switch (pAPO_str[0]) {
        case 'A':
            BrMapUpdate(gArmour_colour_map, BR_MAPU_ALL);
            gArmour_material->colour_map = gArmour_colour_map;
            BrMaterialUpdate(gArmour_material, BR_MATU_COLOURMAP);
            break;
        case 'P':
            BrMapUpdate(gPower_colour_map, BR_MAPU_ALL);
            gPower_material->colour_map = gPower_colour_map;
            BrMaterialUpdate(gPower_material, BR_MATU_COLOURMAP);
            break;
        case 'O':
            BrMapUpdate(gOffensive_colour_map, BR_MAPU_ALL);
            gOffence_material->colour_map = gOffensive_colour_map;
            BrMaterialUpdate(gOffence_material, BR_MATU_COLOURMAP);
            break;
        }
    }
    gArmour_actor->render_style = BR_RSTYLE_FACES;
    gPower_actor->render_style = BR_RSTYLE_FACES;
    gOffense_actor->render_style = BR_RSTYLE_FACES;
#else
    __asm {
            sub esp, 0x10
            movsx eax, byte ptr [edx]
            push ebx
            push ebp
            push esi
            sub eax, 0x41
            push edi
            mov dword ptr [esp + 0x1c], edx
            mov esi, ecx
            je L004498ef
            sub eax, 0xe
            je L004498a3
            dec eax
            jne L0044993a
            mov eax, dword ptr [gPower_colour_map]
            mov ecx, dword ptr [gProgram_state+0x1964]
            mov ebp, dword ptr [gIcon_power]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [gPrev_ps_apo_levels+0x4]
            mov ebx, 2
            cmp esi, eax
            mov dword ptr [esp + 0x10], ecx
            jne L00449888
            cmp ecx, dword ptr [gPrev_ps_drawn_levels+0x4]
            jne L00449888
            xor eax, eax
            jmp L0044988d
    L00449888:
            mov eax, 1
    L0044988d:
            mov dword ptr [gPS_apo_level_changed+0x4], eax
            mov dword ptr [gPrev_ps_apo_levels+0x4], esi
            mov dword ptr [gPrev_ps_drawn_levels+0x4], ecx
            jmp L00449946
    L004498a3:
            mov eax, dword ptr [gPrev_ps_apo_levels+0x8]
            mov ecx, dword ptr [gProgram_state+0x1968]
            mov edx, dword ptr [gOffensive_colour_map]
            mov ebp, dword ptr [gIcon_offense]
            cmp esi, eax
            mov ebx, 2
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x14], edx
            jne L004498d7
            cmp ecx, dword ptr [gPrev_ps_drawn_levels+0x4]
            jne L004498d7
            xor eax, eax
            jmp L004498dc
    L004498d7:
            mov eax, 1
    L004498dc:
            mov dword ptr [gPS_apo_level_changed+0x8], eax
            mov dword ptr [gPrev_ps_drawn_levels+0x8], ecx
            mov dword ptr [gPrev_ps_apo_levels+0x8], esi
            jmp L00449946
    L004498ef:
            mov eax, dword ptr [gArmour_colour_map]
            mov ecx, dword ptr [gProgram_state+0x1960]
            mov ebp, dword ptr [gIcon_armour]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [gPrev_ps_apo_levels]
            mov ebx, 2
            cmp esi, eax
            mov dword ptr [esp + 0x10], ecx
            jne L00449922
            cmp ecx, dword ptr [gPrev_ps_drawn_levels]
            jne L00449922
            xor eax, eax
            jmp L00449927
    L00449922:
            mov eax, 1
    L00449927:
            mov dword ptr [gPS_apo_level_changed], eax
            mov dword ptr [gPrev_ps_apo_levels], esi
            mov dword ptr [gPrev_ps_drawn_levels], ecx
            jmp L00449946
    L0044993a:
            mov ebp, dword ptr [esp + 0x1c]
            mov ebx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x1c]
    L00449946:
            test eax, eax
            je L00449ae8
            mov edi, dword ptr [esp + 0x14]
            mov edx, dword ptr [gPowerbar]
            mov ecx, edi
            call DRPixelmapCopy
            mov cx, word ptr [ebp + 0x36]
            mov dx, word ptr [ebp + 0x34]
            push ecx
            push edx
            push 0
            push 0
            push ebp
            push 0
            mov edx, ebx
            mov ecx, edi
            call DRPixelmapRectangleMaskedCopy
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x18], esi
            sub eax, esi
            xor edi, edi
            mov dword ptr [esp + 0x10], eax
    L00449989:
            xor esi, esi
    L0044998b:
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            jle L004499c5
            mov eax, dword ptr [gIcon_litBloc1]
            mov dx, word ptr [eax + 0x36]
            mov cx, word ptr [eax + 0x34]
            push edx
            mov edx, esi
            imul edx, ecx
            add edx, ebx
            push ecx
            add dx, word ptr [ebp + 0x34]
            mov ecx, dword ptr [esp + 0x1c]
            push 0
            push 0
            push eax
            push edi
            add edx, 2
            call DRPixelmapRectangleMaskedCopy
            dec dword ptr [esp + 0x18]
            jmp L00449a2b
    L004499c5:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            jle L004499ff
            mov eax, dword ptr [gIcon_grnBlock1]
            mov dx, word ptr [eax + 0x36]
            mov cx, word ptr [eax + 0x34]
            push edx
            mov edx, esi
            imul edx, ecx
            add edx, ebx
            push ecx
            add dx, word ptr [ebp + 0x34]
            mov ecx, dword ptr [esp + 0x1c]
            push 0
            push 0
            push eax
            push edi
            add edx, 2
            call DRPixelmapRectangleMaskedCopy
            dec dword ptr [esp + 0x10]
            jmp L00449a2b
    L004499ff:
            mov eax, dword ptr [gIcon_greyBloc1]
            mov dx, word ptr [eax + 0x36]
            mov cx, word ptr [eax + 0x34]
            push edx
            mov edx, esi
            imul edx, ecx
            add edx, ebx
            push ecx
            add dx, word ptr [ebp + 0x34]
            mov ecx, dword ptr [esp + 0x1c]
            push 0
            push 0
            push eax
            push edi
            add edx, 2
            call DRPixelmapRectangleMaskedCopy
    L00449a2b:
            inc esi
            cmp esi, 0xa
            jl L0044998b
            add edi, 5
            sub ebx, 2
            cmp edi, 0xf
            jl L00449989
            mov eax, dword ptr [esp + 0x1c]
            movsx eax, byte ptr [eax]
            sub eax, 0x41
            je L00449ab6
            sub eax, 0xe
            je L00449a89
            dec eax
            jne L00449ae8
            mov ecx, dword ptr [gPower_colour_map]
            push 0xfff
            push ecx
            call BrMapUpdate
            mov edx, dword ptr [gPower_material]
            mov eax, dword ptr [gPower_colour_map]
            add esp, 8
            mov dword ptr [edx + 0x40], eax
            mov ecx, dword ptr [gPower_material]
            push 8
            push ecx
            jmp L00449ae0
    L00449a89:
            mov edx, dword ptr [gOffensive_colour_map]
            push 0xfff
            push edx
            call BrMapUpdate
            mov eax, dword ptr [gOffence_material]
            mov ecx, dword ptr [gOffensive_colour_map]
            add esp, 8
            mov dword ptr [eax + 0x40], ecx
            mov edx, dword ptr [gOffence_material]
            push 8
            push edx
            jmp L00449ae0
    L00449ab6:
            mov eax, dword ptr [gArmour_colour_map]
            push 0xfff
            push eax
            call BrMapUpdate
            mov ecx, dword ptr [gArmour_material]
            mov edx, dword ptr [gArmour_colour_map]
            add esp, 8
            mov dword ptr [ecx + 0x40], edx
            mov eax, dword ptr [gArmour_material]
            push 8
            push eax
    L00449ae0:
            call BrMaterialUpdate
            add esp, 8
    L00449ae8:
            mov ecx, dword ptr [gArmour_actor]
            mov al, 4
            pop edi
            pop esi
            mov byte ptr [ecx + 0x20], al
            mov edx, dword ptr [gPower_actor]
            pop ebp
            pop ebx
            mov byte ptr [edx + 0x20], al
            mov ecx, dword ptr [gOffense_actor]
            mov byte ptr [ecx + 0x20], al
            add esp, 0x10
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00449800
void C2_HOOK_FASTCALL DoPSPowerupHeadups(void) {
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[0], "A");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[1], "P");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[2], "O");
}

// FUNCTION: CARMA2_HW 0x004496f0
void C2_HOOK_FASTCALL DimAFewBits(void) {
    int i;

#define DIM_INDEX gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0
    for (i = 0; i < gProgram_state.current_car.dim_count[DIM_INDEX]; i++) {
        DimRectangle(
            gBack_screen,
            gProgram_state.current_car.dim_left[DIM_INDEX][i],
            gProgram_state.current_car.dim_top[DIM_INDEX][i],
            gProgram_state.current_car.dim_right[DIM_INDEX][i],
            gProgram_state.current_car.dim_bottom[DIM_INDEX][i],
            1);
    }
#undef DIM_INDEX
}

// FUNCTION: CARMA2_HW 0x0047cbd0
void C2_NAKED C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
#ifndef CARPOCALYPSE2_MATCHING
    tU8* ptr;
    tU8* depth_table_ptr;
    tU8* right_ptr;
    int y;
    int line_skip;
    int width;

    if (gDevious_2d) {
        DeviouslyDimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
        return;
    }
    ptr = (tU8*)pPixelmap->pixels + pLeft + pPixelmap->row_bytes * pTop;
    line_skip = pPixelmap->row_bytes - pRight + pLeft;
    depth_table_ptr = (tU8*)gDepth_shade_table->pixels + gDepth_shade_table->row_bytes * gDim_amount;
    width = pRight - pLeft;
    if (pKnock_out_corners) {
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
        ptr += line_skip + 1;
        for (y = pTop + 1; y < pBottom - 1; y++, ptr += line_skip) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
        }
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
    } else {
        for (y = pTop; y < pBottom; y++) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
            ptr += line_skip;
        }
    }
#else
    __asm {
            mov eax, dword ptr [gDevious_2d]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            je L0047cc03
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x20]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esp + 0x1c]
            push edx
            mov edx, dword ptr [esp + 0x24]
            push eax
            call DeviouslyDimRectangle
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0047cc03:
            mov edx, dword ptr [esp + 0x14]
            mov ebp, dword ptr [esp + 0x1c]
            mov eax, ebp
            mov ebx, dword ptr [gDepth_shade_table]
            movsx ecx, word ptr [edx + 0x28]
            imul eax, ecx
            mov esi, dword ptr [edx + 8]
            mov edx, dword ptr [esp + 0x20]
            add eax, esi
            mov esi, dword ptr [esp + 0x18]
            mov edi, esi
            add eax, esi
            sub edi, edx
            sub edx, esi
            add edi, ecx
            mov esi, dword ptr [esp + 0x28]
            movsx ecx, word ptr [ebx + 0x28]
            imul ecx, dword ptr [gHeadup_images+0xb4]
            add ecx, dword ptr [ebx + 8]
            test esi, esi
            je L0047ccab
            inc eax
            lea esi, [edx + eax - 2]
            cmp eax, esi
            jae L0047cc5f
    L0047cc50:
            xor ebx, ebx
            mov bl, byte ptr [eax]
            inc eax
            cmp eax, esi
            mov bl, byte ptr [ebx + ecx]
            mov byte ptr [eax - 1], bl
            jb L0047cc50
    L0047cc5f:
            mov esi, dword ptr [esp + 0x24]
            lea ebx, [ebp + 1]
            dec esi
            lea eax, [eax + edi + 1]
            cmp ebx, esi
            jge L0047cc8e
            sub esi, ebx
            mov ebp, esi
    L0047cc73:
            lea esi, [edx + eax]
            cmp eax, esi
            jae L0047cc89
    L0047cc7a:
            xor ebx, ebx
            mov bl, byte ptr [eax]
            inc eax
            cmp eax, esi
            mov bl, byte ptr [ebx + ecx]
            mov byte ptr [eax - 1], bl
            jb L0047cc7a
    L0047cc89:
            add eax, edi
            dec ebp
            jne L0047cc73
    L0047cc8e:
            inc eax
            lea esi, [edx + eax - 2]
            cmp eax, esi
            jae L0047ccd2
    L0047cc97:
            xor edx, edx
            mov dl, byte ptr [eax]
            inc eax
            cmp eax, esi
            mov dl, byte ptr [edx + ecx]
            mov byte ptr [eax - 1], dl
            jb L0047cc97
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0047ccab:
            mov esi, dword ptr [esp + 0x24]
            cmp ebp, esi
            jge L0047ccd2
            sub esi, ebp
            mov ebp, esi
    L0047ccb7:
            lea esi, [edx + eax]
            cmp eax, esi
            jae L0047cccd
    L0047ccbe:
            xor ebx, ebx
            mov bl, byte ptr [eax]
            inc eax
            cmp eax, esi
            mov bl, byte ptr [ebx + ecx]
            mov byte ptr [eax - 1], bl
            jb L0047ccbe
    L0047cccd:
            add eax, edi
            dec ebp
            jne L0047ccb7
    L0047ccd2:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047cce0
void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    if (pLeft < 0) {
        pLeft = 0;
    }
    if (pTop < 0) {
        pTop = 0;
    }
    if (pRight > pPixelmap->width) {
        pRight = pPixelmap->width;
    }
    if (pBottom > pPixelmap->height) {
        pBottom = pPixelmap->height;
    }
    if (pLeft < pRight && pTop < pBottom) {
        DimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
    }
}

// FUNCTION: CARMA2_HW 0x0047ca50
void C2_HOOK_FASTCALL DrawRectangleOutline(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, br_uint_32 pColour) {
    BrPixelmapLine(pPixelmap, pLeft + 1, pTop, pRight - 1, pTop, pColour);
    BrPixelmapLine(pPixelmap, pLeft + 1, pBottom, pRight - 1, pBottom, pColour);
    BrPixelmapLine(pPixelmap, pLeft, pTop + 1, pLeft, pBottom - 1, pColour);
    BrPixelmapLine(pPixelmap, pRight, pTop + 1, pRight, pBottom - 1, pColour);
}

// FUNCTION: CARMA2_HW 0x0047cad0
void C2_NAKED C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
#ifndef CARPOCALYPSE2_MATCHING

    gDim_model->vertices[1].p.v[0] = (float)pLeft;
    gDim_model->vertices[0].p.v[0] = gDim_model->vertices[1].p.v[0];
    gDim_model->vertices[3].p.v[0] = (float)pRight;
    gDim_model->vertices[2].p.v[0] = gDim_model->vertices[3].p.v[0];
    gDim_model->vertices[3].p.v[1] = -(float)pTop;
    gDim_model->vertices[0].p.v[1] = gDim_model->vertices[3].p.v[1];
    gDim_model->vertices[2].p.v[1] = -(float)pBottom;
    gDim_model->vertices[1].p.v[1] = gDim_model->vertices[2].p.v[1];
    BrModelUpdate(gDim_model, BR_MODU_VERTEX_POSITIONS);
    gDim_actor->render_style = BR_RSTYLE_FACES;
    if (gDim_actor->prev == NULL) {
        BrActorAdd(g2d_camera, gDim_actor);
    }
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    if (gDim_actor->parent != NULL) {
        BrActorRemove(gDim_actor);
    }
    gDim_actor->render_style = BR_RSTYLE_NONE;
#else
    __asm {
            push ecx
            mov eax, dword ptr [gDim_model]
            mov dword ptr [esp], edx
            _emit 0xdb
        _emit 0x44
        _emit 0x24
        _emit 0x00
            mov ecx, dword ptr [eax + 8]
            push 1
            fstp dword ptr [ecx + 0x28]
            mov edx, dword ptr [gDim_model]
            fild dword ptr [esp + 0x10]
            mov eax, dword ptr [edx + 8]
            mov ecx, dword ptr [eax + 0x28]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [gDim_model]
            mov eax, dword ptr [edx + 8]
            fstp dword ptr [eax + 0x78]
            mov ecx, dword ptr [gDim_model]
            fild dword ptr [esp + 0xc]
            mov eax, dword ptr [ecx + 8]
            mov edx, dword ptr [eax + 0x78]
            mov dword ptr [eax + 0x50], edx
            mov eax, dword ptr [gDim_model]
            mov ecx, dword ptr [eax + 8]
            fchs
            fstp dword ptr [ecx + 0x7c]
            mov edx, dword ptr [gDim_model]
            fild dword ptr [esp + 0x14]
            mov eax, dword ptr [edx + 8]
            mov ecx, dword ptr [eax + 0x7c]
            mov dword ptr [eax + 4], ecx
            mov edx, dword ptr [gDim_model]
            mov eax, dword ptr [edx + 8]
            fchs
            fstp dword ptr [eax + 0x54]
            mov ecx, dword ptr [gDim_model]
            mov eax, dword ptr [ecx + 8]
            mov edx, dword ptr [eax + 0x54]
            mov dword ptr [eax + 0x2c], edx
            mov eax, dword ptr [gDim_model]
            push eax
            call BrModelUpdate
            mov ecx, dword ptr [gDim_actor]
            add esp, 8
            mov byte ptr [ecx + 0x20], 4
            mov eax, dword ptr [gDim_actor]
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            jne L0047cb88
            mov edx, dword ptr [g2d_camera]
            push eax
            push edx
            call BrActorAdd
            add esp, 8
    L0047cb88:
            mov eax, dword ptr [gDepth_buffer]
            mov ecx, dword ptr [gBack_screen]
            push eax
            mov eax, dword ptr [g2d_camera]
            push ecx
            push eax
            push eax
            call BrZbsSceneRender
            mov eax, dword ptr [gDim_actor]
            add esp, 0x10
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L0047cbbe
            push eax
            call BrActorRemove
            mov eax, dword ptr [gDim_actor]
            add esp, 4
    L0047cbbe:
            mov byte ptr [eax + 0x20], 1
            pop ecx
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044a9d0
void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time) {
    int i;
    int y_pitch;
    int the_step;
    int the_wobble_x;
    int the_wobble_y;
    int the_num_damage_units;
    br_pixelmap* the_image;
    tDamage_unit* the_damage;

    if (&gProgram_state.current_car != gCar_to_view) {
        return;
    }
    if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
        if (gProgram_state.which_view != eView_forward) {
            return;
        }
        the_wobble_x = gScreen_wobble_x;
        the_wobble_y = gScreen_wobble_y;
    } else {
        the_wobble_x = gProgram_state.current_car.damage_x_offset;
        the_wobble_y = gProgram_state.current_car.damage_y_offset;
    }

    DRPixelmapCopy(gDamage_hud, gGrey_top5);

    the_num_damage_units = CARPOCALYPSE2_ASIZE(gProgram_state.current_car.damage_units);

    for (i = 0; i < the_num_damage_units; i++) {
        the_damage = &gProgram_state.current_car.damage_units[i];
        if (i != eDamage_driver) {
            the_image = the_damage->images;
            the_step = 5 * the_damage->damage_level / 100;
            y_pitch = (the_image->height / 2) / 5;
            DRPixelmapRectangleMaskedCopy(
                gDamage_hud,
                the_wobble_x + gProgram_state.current_car.damage_units[i].x_coord,
                the_wobble_y + gProgram_state.current_car.damage_units[i].y_coord,
                the_image,
                0,
                y_pitch * (2 * the_step + ((pThe_time / the_damage->periods[the_step]) & 1)),
                the_image->width,
                y_pitch);
        }
    }
#ifndef CARPOCALYPSE2_MATCHING
    /* The soft rasterizer caches prepared textures, so flag the damage bar
     * map dirty after repainting it every frame (same as the APO bars);
     * otherwise the quad keeps showing its stale initial contents.
     * Matching builds are unaffected. */
    BrMapUpdate(gDamage_hud, BR_MAPU_ALL);
    gStatbarHUD5_material->colour_map = gDamage_hud;
    BrMaterialUpdate(gStatbarHUD5_material, BR_MATU_COLOURMAP);
#endif
}

int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText) {

    while (*pText) {
        if (*(signed char*)pText < 0) {
            return 1;
        }
        pText++;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0044a220
int C2_NAKED C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {
#ifndef CARPOCALYPSE2_MATCHING
    int width;

    width = 0;
    if (pHeadup->type == eHeadup_box_text) {
        return 0;
    } else if (pHeadup->type == eHeadup_coloured_text) {
        pHeadup->clever = IsHeadupTextClever(pHeadup->data.text_info.text);
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    } else if (pHeadup->type == eHeadup_fancy) {
        return 0;
    } else {
        pHeadup->clever = 0;
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    }
    return width;
#else
    __asm {
            push esi
            mov esi, ecx
            push edi
            xor edi, edi
            mov eax, dword ptr [esi]
            cmp eax, 5
            jne L0044a232
            xor eax, eax
            pop edi
            pop esi
            ret
    L0044a232:
            cmp eax, 2
            jne L0044a2e3
            lea edx, [esi + 0x4c]
            mov ecx, edx
            mov al, byte ptr [ecx]
            test al, al
            je L0044a255
    L0044a246:
            inc ecx
            test al, al
            jl L0044a374
            mov al, byte ptr [ecx]
            test al, al
            jne L0044a246
    L0044a255:
            xor ecx, ecx
    L0044a257:
            mov eax, dword ptr [esi + 0x40]
            mov dword ptr [esi + 0x30], ecx
            sub eax, 0
            je L0044a2d8
            dec eax
            je L0044a2a8
            dec eax
            jne L0044a36f
            test ecx, ecx
            mov ecx, dword ptr [esi + 0x148]
            je L0044a28f
            call DRTextCleverWidth
            mov ecx, dword ptr [esi + 0xc]
            mov edi, eax
            cdq
            sub eax, edx
            sar eax, 1
            sub ecx, eax
            mov eax, edi
            mov dword ptr [esi + 4], ecx
            pop edi
            pop esi
            ret
    L0044a28f:
            call DRTextWidth
            mov edi, eax
            cdq
            sub eax, edx
            mov edx, dword ptr [esi + 0xc]
            sar eax, 1
            sub edx, eax
            mov eax, edi
            mov dword ptr [esi + 4], edx
            pop edi
            pop esi
            ret
    L0044a2a8:
            test ecx, ecx
            mov ecx, dword ptr [esi + 0x148]
            je L0044a2c6
            call DRTextCleverWidth
            mov edi, eax
            mov eax, dword ptr [esi + 0xc]
            sub eax, edi
            mov dword ptr [esi + 4], eax
            mov eax, edi
            pop edi
            pop esi
            ret
    L0044a2c6:
            call DRTextWidth
            mov ecx, dword ptr [esi + 0xc]
            mov edi, eax
            sub ecx, edi
            mov dword ptr [esi + 4], ecx
            pop edi
            pop esi
            ret
    L0044a2d8:
            mov edx, dword ptr [esi + 0xc]
            mov eax, edi
            mov dword ptr [esi + 4], edx
            pop edi
            pop esi
            ret
    L0044a2e3:
            cmp eax, 4
            jne L0044a2ed
            xor eax, eax
            pop edi
            pop esi
            ret
    L0044a2ed:
            mov eax, dword ptr [esi + 0x40]
            mov dword ptr [esi + 0x30], 0
            sub eax, 0
            je L0044a369
            dec eax
            je L0044a339
            dec eax
            jne L0044a36f
            mov ecx, dword ptr [esi + 0x14c]
            mov edx, dword ptr [gBack_screen]
            lea eax, [esi + 0x4c]
            push eax
            push ecx
            push edx
            call BrPixelmapTextWidth
            mov di, ax
            mov ecx, dword ptr [esi + 0xc]
            and edi, 0xffff
            add esp, 0xc
            mov eax, edi
            cdq
            sub eax, edx
            sar eax, 1
            sub ecx, eax
            mov eax, edi
            mov dword ptr [esi + 4], ecx
            pop edi
            pop esi
            ret
    L0044a339:
            mov eax, dword ptr [esi + 0x14c]
            mov ecx, dword ptr [gBack_screen]
            lea edx, [esi + 0x4c]
            push edx
            push eax
            push ecx
            call BrPixelmapTextWidth
            mov edx, dword ptr [esi + 0xc]
            mov di, ax
            and edi, 0xffff
            add esp, 0xc
            sub edx, edi
            mov eax, edi
            mov dword ptr [esi + 4], edx
            pop edi
            pop esi
            ret
    L0044a369:
            mov eax, dword ptr [esi + 0xc]
            mov dword ptr [esi + 4], eax
    L0044a36f:
            mov eax, edi
            pop edi
            pop esi
            ret
    L0044a374:
            mov ecx, 1
            jmp L0044a257
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00449fd0
int C2_NAKED C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it) {
#ifndef CARPOCALYPSE2_MATCHING
    int index;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;
    tU32 time;

    time = PDGetTotalTime();
    if (pQueue_it && pSlot_index == 4 && time - gLast_centre_headup < 1000) {
        if (gQueued_headup_count == 4) {
            KillOldestQueuedHeadup();
        }
        gQueued_headups[gQueued_headup_count].flash_rate = pFlash_rate;
        gQueued_headups[gQueued_headup_count].lifetime = pLifetime;
        gQueued_headups[gQueued_headup_count].font_index = pFont_index;
        strcpy(gQueued_headups[gQueued_headup_count].text, pText);
        gQueued_headup_count++;
        return -1;
    }
    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index < 0) {
        return index;
    }
    if (pSlot_index == 4) {
        gLast_centre_headup = time;
    }
    headup_slot = &gProgram_state.current_car.headup_slots[gProgram_state.cockpit_on][pSlot_index];
    the_headup = &gHeadups[index];
    the_headup->data.coloured_text_info.coloured_font = &gFonts[-pFont_index];
    if (pSlot_index == 4) {
        the_headup->type = eHeadup_box_text;
    } else {
        the_headup->type = eHeadup_coloured_text;
    }
    strcpy(the_headup->data.coloured_text_info.text, pText);

    the_headup->slot_index = pSlot_index;
    the_headup->justification = headup_slot->justification;
    if (pSlot_index < 0) {
        the_headup->cockpit_anchored = 0;
    } else {
        the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
    }
    the_headup->field_0x3c = headup_slot->field_0x28;
    the_headup->dimmed_background = headup_slot->dimmed_background;
    the_headup->dim_left = headup_slot->dim_left;
    the_headup->dim_top = headup_slot->dim_top;
    the_headup->dim_right = headup_slot->dim_right;
    the_headup->dim_bottom = headup_slot->dim_bottom;
    the_headup->original_x = headup_slot->x;
    the_headup->right_edge = MungeHeadupWidth(the_headup) + the_headup->x;
    the_headup->y = headup_slot->y;
    if (pFlash_rate) {
        the_headup->flash_period = 1000 / pFlash_rate;
    } else {
        the_headup->flash_period = 0;
    }
    the_headup->last_flash = 0;
    the_headup->flash_state = 0;
    if (pLifetime) {
        the_headup->end_time = GetTotalTime() + pLifetime;
    } else {
        the_headup->end_time = 0;
    }
    return index;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            mov ebx, edx
            mov esi, ecx
            push edi
            mov dword ptr [esp + 0x14], ebx
            mov dword ptr [esp + 0x10], esi
            call PDGetTotalTime
            mov ecx, dword ptr [esp + 0x28]
            test ecx, ecx
            je L0044a0a4
            cmp esi, 4
            jne L0044a0a4
            mov edi, dword ptr [gLast_centre_headup]
            mov ecx, eax
            sub ecx, edi
            cmp ecx, 0x3e8
            jae L0044a0a4
            mov edx, dword ptr [gQueued_headup_count]
            cmp edx, esi
            jne L0044a039
            mov edx, 3
            mov ecx, 0xc9
            mov esi, offset gQueued_headups+0x10c
            mov edi, offset gQueued_headups
            mov dword ptr [gQueued_headup_count], edx
            rep movsd
    L0044a039:
            mov eax, edx
            mov ecx, dword ptr [esp + 0x1c]
            shl eax, 5
            add eax, edx
            mov edi, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x28], 0xffffffff
            lea eax, [edx + eax*2]
            shl eax, 2
            mov dword ptr [eax + gQueued_headups], ebx
            mov dword ptr [eax + gQueued_headups+0x4], ecx
            mov ecx, dword ptr [esp + 0x20]
            lea ebx, [eax + gQueued_headups+0xc]
            mov dword ptr [eax + gQueued_headups+0x8], ecx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x28]
            and ecx, 3
            inc edx
            rep movsb
            mov dword ptr [gQueued_headup_count], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x10
    L0044a0a4:
            or edi, 0xffffffff
            mov edx, offset gHeadups
            xor ecx, ecx
    L0044a0ae:
            test esi, esi
            jl L0044a0bb
            cmp dword ptr [edx + 0x18], esi
            je L0044a216
    L0044a0bb:
            cmp dword ptr [edx], 0
            jne L0044a0c2
            mov edi, ecx
    L0044a0c2:
            inc ecx
            add edx, 0x164
            cmp ecx, 0x25
            jl L0044a0ae
            mov dword ptr [esp + 0x28], edi
    L0044a0d2:
            mov ecx, dword ptr [esp + 0x28]
            test ecx, ecx
            jl L0044a208
            cmp esi, 4
            jne L0044a0e8
            mov dword ptr [gLast_centre_headup], eax
    L0044a0e8:
            mov eax, dword ptr [gProgram_state+0x4c]
            mov edi, dword ptr [esp + 0x24]
            lea ecx, [eax + eax*4]
            lea eax, [esi + ecx*4]
            sub esi, 4
            lea edx, [eax + eax*4]
            lea eax, [eax + edx*2]
            lea ebp, [eax*4 + gProgram_state+0x7d4]
            mov eax, dword ptr [esp + 0x28]
            lea ecx, [eax + eax*4]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov eax, dword ptr [esp + 0x20]
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea ebx, [ecx*4 + gHeadups]
            mov ecx, offset gFonts
            lea eax, [eax + edx*4]
            lea edx, [ebx + 0x4c]
            shl eax, 5
            sub ecx, eax
            neg esi
            sbb esi, esi
            mov dword ptr [ebx + 0x148], ecx
            and esi, 0xfffffffd
            or ecx, 0xffffffff
            add esi, 5
            xor eax, eax
            mov dword ptr [ebx], esi
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x10]
            and ecx, 3
            rep movsb
            mov dword ptr [ebx + 0x18], eax
            mov ecx, dword ptr [ebp + 0x24]
            test eax, eax
            mov dword ptr [ebx + 0x40], ecx
            jl L0044a17f
            mov edx, dword ptr [ebp + 0xc]
            mov dword ptr [ebx + 0x34], edx
            jmp L0044a186
    L0044a17f:
            mov dword ptr [ebx + 0x34], 0
    L0044a186:
            mov eax, dword ptr [ebp + 0x28]
            mov dword ptr [ebx + 0x3c], eax
            mov ecx, dword ptr [ebp + 0x10]
            mov dword ptr [ebx + 0x1c], ecx
            mov edx, dword ptr [ebp + 0x14]
            mov dword ptr [ebx + 0x20], edx
            mov eax, dword ptr [ebp + 0x18]
            mov dword ptr [ebx + 0x24], eax
            mov ecx, dword ptr [ebp + 0x1c]
            mov dword ptr [ebx + 0x28], ecx
            mov edx, dword ptr [ebp + 0x20]
            mov dword ptr [ebx + 0x2c], edx
            mov eax, dword ptr [ebp]
            mov ecx, ebx
            mov dword ptr [ebx + 0xc], eax
            call MungeHeadupWidth
            mov ecx, dword ptr [ebx + 4]
            xor edi, edi
            add eax, ecx
            mov dword ptr [ebx + 0x10], eax
            mov ecx, dword ptr [ebp + 4]
            mov dword ptr [ebx + 8], ecx
            mov ecx, dword ptr [esp + 0x14]
            cmp ecx, edi
            je L0044a1dc
            mov eax, 0x3e8
            cdq
            idiv ecx
            mov dword ptr [ebx + 0x14], eax
            jmp L0044a1df
    L0044a1dc:
            mov dword ptr [ebx + 0x14], edi
    L0044a1df:
            mov esi, dword ptr [esp + 0x1c]
            mov dword ptr [ebx + 0x48], edi
            cmp esi, edi
            mov dword ptr [ebx + 0x38], edi
            je L0044a205
            call GetTotalTime
            add eax, esi
            mov dword ptr [ebx + 0x44], eax
            mov eax, dword ptr [esp + 0x28]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x10
    L0044a205:
            mov dword ptr [ebx + 0x44], edi
    L0044a208:
            mov eax, dword ptr [esp + 0x28]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x10
    L0044a216:
            mov dword ptr [esp + 0x28], ecx
            jmp L0044a0d2
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044a380
int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText) {
    return NewTextHeadupSlot2(pSlot_index, pFlash_rate, pLifetime, pFont_index, pText, 1);
}

// FUNCTION: CARMA2_HW 0x0044a3a0
int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index) {
    int index;
    int width;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;

    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index >= 0) {
        headup_slot = &gProgram_state.current_car.headup_slots[gProgram_state.cockpit_on][pSlot_index];
        the_headup = &gHeadups[index];
        the_headup->type = eHeadup_image;
        the_headup->slot_index = pSlot_index;
        the_headup->justification = headup_slot->justification;
        if (pSlot_index >= 0) {
            the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
        } else {
            the_headup->cockpit_anchored = 0;
        }
        the_headup->dimmed_background = headup_slot->dimmed_background;
        the_headup->dim_left = headup_slot->dim_left;
        the_headup->dim_top = headup_slot->dim_top;
        the_headup->dim_right = headup_slot->dim_right;
        the_headup->dim_bottom = headup_slot->dim_bottom;
        the_headup->original_x = headup_slot->x;

        switch (headup_slot->justification) {
        case eJust_left:
            the_headup->x = the_headup->original_x;
            break;
        case eJust_right:
            width = DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            the_headup->x = the_headup->original_x + width;
            break;
        case eJust_centre:
            width = DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            the_headup->x = the_headup->original_x + width / 2;
            break;
        }
        the_headup->y = headup_slot->y;
        if (pFlash_rate != 0) {
            the_headup->flash_period = 1000 / pFlash_rate;
        } else {
            the_headup->flash_period = 0;
        }
        the_headup->last_flash = 0;
        the_headup->flash_state = 0;
        if (pLifetime != 0) {
            the_headup->end_time = GetTotalTime() + pLifetime;
        } else {
            the_headup->end_time = 0;
        }
        strcpy(the_headup->data.image_info.text, gHud_messages[pImage_index].message);
        the_headup->data.image_info.font_index = gHud_messages[pImage_index].font2;
        the_headup->data.image_info.font = &gFonts[gHud_messages[pImage_index].font1];
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.image_info.field_0x104 = 0;
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = -width;
            break;
        case eJust_right:
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x104 = width;
            the_headup->data.image_info.field_0x10c = 0;
            break;
        case eJust_centre:
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x104 = width / 2;
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = -(width / 2);
            break;
        }
    }
    return index;
}

// FUNCTION: CARMA2_HW 0x0044a600
void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex) {
    tU32 the_time;
    int temp_ref;
    int center;
    int width;
    tHeadup* the_headup;

    center = gCurrent_graf_data->width / 2;
    the_time = GetTotalTime();
    if (gMap_view == 2) {
        return;
    }
    if (gLast_fancy_index >= 0 && the_time - gLast_fancy_time <= 2000 && gLast_fancy_index > pIndex) {
        return;
    }
    temp_ref = NewImageHeadupSlot(6, 0, 2000, pIndex + 10);
    if (temp_ref >= 0) {
        gLast_fancy_index = pIndex;
        gLast_fancy_headup = temp_ref;
        gLast_fancy_time = the_time;

        the_headup = &gHeadups[temp_ref];
        the_headup->type = eHeadup_fancy;
        the_headup->data.fancy_info.fancy_stage = 0;
        the_headup->data.fancy_info.field_0x108 = 0;
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.fancy_info.offset = center;
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = -width;
            break;
        case eJust_right:
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = center;
            the_headup->data.fancy_info.offset = center + width;
            break;
        case eJust_centre:
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.offset = center + width / 2;
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = -center - width / 2;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044a860
void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pX, int pY) {
    tHeadup* the_headup;
    int delta_x;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        delta_x = the_headup->x - the_headup->original_x;
        the_headup->original_x = pX;
        the_headup->x = delta_x + pX;
        the_headup->y = pY;
    }
}

// FUNCTION: CARMA2_HW 0x0044a8a0
void C2_HOOK_FASTCALL FUN_0044a8a0(int pHeadup_index, int pNew_x, int pNew_y) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        the_headup->justification = eJust_left;
        the_headup->original_x = pNew_x;
        the_headup->x = pNew_x;
        the_headup->y = pNew_y;
    }
}

// FUNCTION: CARMA2_HW 0x00449b10
void C2_HOOK_FASTCALL DoHeadups(tU32 pThe_time) {
    int i;
    int x_offset;
    int y_offset;
    tHeadup* the_headup;

    if (gNet_mode != eNet_mode_none) {
        DoNetScores();
    }
    if (gQueued_headup_count && PDGetTotalTime() - gLast_centre_headup >= 1000) {
        NewTextHeadupSlot2(4,
            gQueued_headups[0].flash_rate,
            gQueued_headups[0].lifetime,
            gQueued_headups[0].font_index,
            gQueued_headups[0].text,
            1);
        if (gQueued_headup_count) {
            gQueued_headup_count -= 1;
            memmove(&gQueued_headups[0], &gQueued_headups[1], gQueued_headup_count * sizeof(tQueued_headup));
        }
    }
    if (gHeadup_detail_level == 0 || gHeadup_detail_level == 3) {
        if (gTimer_headup >= 0) {
            the_headup = &gHeadups[gTimer_headup];
            x_offset = the_headup->x - the_headup->original_x;
            the_headup->original_x = 634;
            the_headup->y = 2;
            the_headup->x = x_offset + 634;
        }
    } else {
        if (gTimer_headup >= 0) {
            the_headup = &gHeadups[gTimer_headup];
            x_offset = the_headup->x - the_headup->original_x;
            the_headup->original_x = 389;
            the_headup->y = 13;
            the_headup->x = x_offset + 389;
        }
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gHeadups); i++) {
        the_headup = &gHeadups[i];
        if (the_headup->type != eHeadup_unused
                && (gProgram_state.which_view == eView_forward || !the_headup->cockpit_anchored)
                && (the_headup->type == eHeadup_image
                    || the_headup->type == eHeadup_fancy
                    || (the_headup->type == eHeadup_text && the_headup->data.text_info.text[0] != '\0')
                    || ((the_headup->type == eHeadup_coloured_text || the_headup->type == eHeadup_box_text)
                        && the_headup->data.text_info.text[0] != '\0'))) {
            if (the_headup->type == eHeadup_fancy || the_headup->end_time == 0 || pThe_time < the_headup->end_time) {
                if (the_headup->flash_period == 0
                        || Flash(the_headup->flash_period, &the_headup->last_flash, &the_headup->flash_state)) {

                    switch (the_headup->type) {
                    case eHeadup_text:
                        if (the_headup->cockpit_anchored) {
                            y_offset = gScreen_wobble_y;
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = gScreen_wobble_x;
                        } else {
                            x_offset = 0;
                        }
                        BrPixelmapText(
                            gRender_screen,
                            x_offset + the_headup->x,
                            y_offset + the_headup->y,
                            the_headup->data.text_info.colour,
                            the_headup->data.text_info.font,
                            the_headup->data.text_info.text);
                        break;
                    case eHeadup_coloured_text:
                        if (gHeadup_detail_level == 2 || gHeadup_detail_level == 5
                                || (i != gCredits_won_headup
                                    && i != gPed_kill_count_headup
                                    && i != gCar_kill_count_headup
                                    && i != gLaps_headup)) {
                            if (the_headup->clever) {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = gScreen_wobble_y;
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = gScreen_wobble_x;
                                } else {
                                    x_offset = 0;
                                }
                                DRPixelmapCleverText(
                                        gRender_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            } else {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = gScreen_wobble_y;
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = gScreen_wobble_x;
                                } else {
                                    x_offset = 0;
                                }
                                DRPixelmapText(
                                        gRender_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            }
                        }
                        break;
                    case eHeadup_image:
                        PolyFontText(the_headup->data.image_info.text,
                             the_headup->original_x + the_headup->data.image_info.field_0x104,
                             the_headup->y,
                             the_headup->data.image_info.font_index, eJust_centre, 0);
                        break;

                    case eHeadup_fancy:
                        switch (the_headup->data.fancy_info.fancy_stage) {
                        case 0:
                            the_headup->data.fancy_info.offset -= 325 * gFrame_period / 1000;
                            if (the_headup->data.fancy_info.offset <= 0) {
                                the_headup->data.fancy_info.offset = 0;
                                the_headup->data.fancy_info.fancy_stage = 1;
                                the_headup->data.fancy_info.start_time = GetTotalTime();
                            }
                            PolyFontText(the_headup->data.fancy_info.text,
                                the_headup->original_x + the_headup->data.fancy_info.offset,
                                the_headup->y,
                                the_headup->data.fancy_info.font_index,
                                eJust_centre,
                                0);
                            break;
                        case 1:
                            if (pThe_time - the_headup->data.fancy_info.start_time > 1000) {
                                the_headup->data.fancy_info.fancy_stage = 2;
                                the_headup->data.fancy_info.start_time = GetTotalTime();
                            }
                            PolyFontText(the_headup->data.fancy_info.text,
                                the_headup->original_x,
                                the_headup->y,
                                the_headup->data.fancy_info.font_index,
                                eJust_centre,
                                0);
                            break;
                        case 2:
                            the_headup->data.fancy_info.offset -= 325 * gFrame_period / 1000;
                            if (the_headup->data.fancy_info.offset <= the_headup->data.fancy_info.end_offset) {
                                ClearHeadup(i);
                            } else {
                                PolyFontText(the_headup->data.fancy_info.text,
                                    the_headup->original_x + the_headup->data.fancy_info.offset,
                                    the_headup->y,
                                    the_headup->data.fancy_info.font_index,
                                    eJust_centre,
                                    0);
                            }
                            break;
                        default:
                            break;
                        }
                        break;

                    case eHeadup_box_text:
                        if (the_headup->cockpit_anchored) {
                            y_offset = gScreen_wobble_y;
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = gScreen_wobble_x;
                        } else {
                            x_offset = 0;
                        }
                        OoerrIveGotTextInMeBoxMissus(
                            the_headup->data.coloured_text_info.coloured_font - gFonts,
                            the_headup->data.coloured_text_info.text,
                            gRender_screen,
                            gRender_screen->width / 10,
                            y_offset + the_headup->y,
                            9 * gRender_screen->width / 10,
                            y_offset + the_headup->y + 60,
                            1);
                        break;
                    default:
                        break;
                    }
                }
            } else {
                ClearHeadup(i);
            }
        }
    }
}

#ifndef CARPOCALYPSE2_MATCHING
/* Port builds draw coloured/plain HUD text straight into pixels (see the
 * software DRPixelmapText), which has no depth buffer, so the opaque HUD
 * quads rendered by StopRenderingHeadups cover it wherever they overlap
 * (notably the timer digits under the timer panels). The original routed
 * this text through glyph actors that depth-test nearer than the quads.
 * Redraw the stateless pixel-text slots once more after the HUD pass so
 * they end up on top, matching the original look. Matching builds are
 * unaffected. Image/fancy/box slots are skipped here: image glyphs were
 * already rendered inside the pass, and fancy/box carry per-frame state. */
void C2_HOOK_FASTCALL RedrawHeadupTexts(tU32 pThe_time) {
    int i;
    int x_offset;
    int y_offset;
    tHeadup* the_headup;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gHeadups); i++) {
        the_headup = &gHeadups[i];
        if ((the_headup->type == eHeadup_text || the_headup->type == eHeadup_coloured_text)
                && (gProgram_state.which_view == eView_forward || !the_headup->cockpit_anchored)
                && the_headup->data.text_info.text[0] != '\0') {
            if (the_headup->type == eHeadup_fancy || the_headup->end_time == 0 || pThe_time < the_headup->end_time) {
                if (the_headup->flash_period == 0
                        || Flash(the_headup->flash_period, &the_headup->last_flash, &the_headup->flash_state)) {
                    if (the_headup->type == eHeadup_text) {
                        if (the_headup->cockpit_anchored) {
                            y_offset = gScreen_wobble_y;
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = gScreen_wobble_x;
                        } else {
                            x_offset = 0;
                        }
                        BrPixelmapText(
                            gRender_screen,
                            x_offset + the_headup->x,
                            y_offset + the_headup->y,
                            the_headup->data.text_info.colour,
                            the_headup->data.text_info.font,
                            the_headup->data.text_info.text);
                    } else {
                        if (gHeadup_detail_level == 2 || gHeadup_detail_level == 5
                                || (i != gCredits_won_headup
                                    && i != gPed_kill_count_headup
                                    && i != gCar_kill_count_headup
                                    && i != gLaps_headup)) {
                            if (the_headup->clever) {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = gScreen_wobble_y;
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = gScreen_wobble_x;
                                } else {
                                    x_offset = 0;
                                }
                                DRPixelmapCleverText(
                                        gRender_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            } else {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = gScreen_wobble_y;
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = gScreen_wobble_x;
                                } else {
                                    x_offset = 0;
                                }
                                DRPixelmapText(
                                        gRender_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            }
                        }
                    }
                }
            }
        }
    }
}

/* gFrame_rate is CalculateFrameRate's 31-frame average in tenths of a frame per second;
 * the shown value is refreshed twice a second so it stays readable. */

#endif

// FUNCTION: CARMA2_HW 0x0044aad0
void C2_NAKED C2_HOOK_FASTCALL DoInstruments(tU32 pThe_time) {
#ifndef CARPOCALYPSE2_MATCHING
    char buffer[8];
    br_pixelmap* speedo_image;
    int the_wobble_x;
    int the_wobble_y;
    int gear;
    int gear_height;
    double the_angle;
    double the_angle2;
    double sin_angle;
    double cos_angle;
    double speed_mph;

    if (gCar_to_view != NULL && gCar_to_view->driver > 5) {
        speed_mph = gCar_to_view->speedo_speed * WORLD_SCALE / 1600.0f * 1000.f * 3600.0f;
        if (speed_mph < 0.0f) {
            speed_mph = 0.0f;
        }
        if (gHeadup_detail_level == 0 || gHeadup_detail_level == 3) {
            if (!gAction_replay_mode) {
                sprintf(buffer, "%03i", (int)speed_mph);
                PolyFontText(buffer, 1, 1, kPolyfont_ingame_medium_green, eJust_left, 0);
            }
        } else {
            if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
                if (gProgram_state.which_view != eView_forward) {
                    return;
                }
                the_wobble_x = gScreen_wobble_x;
                the_wobble_y = gScreen_wobble_y;
            } else {
                the_wobble_x = 0;
                the_wobble_y = 0;
            }
            if (gProgram_state.current_car.tacho_radius_2[gProgram_state.cockpit_on] >= 0) {
                int rev;

                rev = div((int)gCar_to_view->revs, div(gCar_to_view->red_line, 11).quot).quot;
                if (rev < 0) {
                    rev = 0;
                } else if (rev > 11) {
                    rev = 11;
                }
                gCurrent_rev = gRevs[rev];
            } else {
                speedo_image = gRevs[div((int)gCar_to_view->revs, 11).quot];
                BrPixelmapRectangleCopy(gCurrent_rev,
                    gProgram_state.current_car.tacho_x[gProgram_state.cockpit_on] + the_wobble_x,
                    gProgram_state.current_car.tacho_y[gProgram_state.cockpit_on] + the_wobble_y,
                    speedo_image, 0, 0, speedo_image->width, speedo_image->height);

            }
            if (!gProgram_state.cockpit_on || gProgram_state.cockpit_image_index < 0 || gProgram_state.which_view == eView_forward) {

                gear = gCar_to_view->gear;
                if (gear < 0) {
                    gear = -1;
                }
                gear_height = gProgram_state.current_car.gears_image->height / 8;
                DRPixelmapRectangleMaskedCopy(gCurrent_rev,
                    gProgram_state.current_car.gear_x[gProgram_state.cockpit_on] + the_wobble_x,
                    gProgram_state.current_car.gear_y[gProgram_state.cockpit_on] + the_wobble_y,
                    gProgram_state.current_car.gears_image,
                    0, (gear + 1) * gear_height,
                    gProgram_state.current_car.gears_image->width,
                    gear_height);
            }
            speedo_image = gProgram_state.current_car.speedo_image[gProgram_state.cockpit_on];
            if (gProgram_state.current_car.speedo_radius_2[gProgram_state.cockpit_on] >= 0) {
                if (speedo_image != NULL && (!gProgram_state.cockpit_on || gProgram_state.cockpit_image_index < 0)) {
                    DRPixelmapRectangleMaskedCopy(gCurrent_rev,
                        gProgram_state.current_car.speedo_x[gProgram_state.cockpit_on] + the_wobble_x,
                        gProgram_state.current_car.speedo_y[gProgram_state.cockpit_on] + the_wobble_y,
                        speedo_image, 0, 0, speedo_image->width, speedo_image->height);
                }
                if (speed_mph <= gProgram_state.current_car.max_speed) {
                    the_angle = DEG_TO_RAD(gProgram_state.current_car.speedo_start_angle[gProgram_state.cockpit_on]
                        + ((float)gProgram_state.current_car.speedo_end_angle[gProgram_state.cockpit_on] - (float)gProgram_state.current_car.speedo_start_angle[gProgram_state.cockpit_on]) * speed_mph
                            / (float)gProgram_state.current_car.max_speed);
                } else {
                    the_angle = DEG_TO_RAD((float)gProgram_state.current_car.speedo_end_angle[gProgram_state.cockpit_on]);
                }
                if (the_angle < 0.0) {
                    the_angle += CARPOCALYPSE2_TAU;
                } else if (the_angle >= CARPOCALYPSE2_TAU) {
                    the_angle -= CARPOCALYPSE2_TAU;
                }
                the_angle2 = CARPOCALYPSE2_PI_OVER_2 - the_angle;
                if (the_angle2 < 0.0) {
                    the_angle2 += CARPOCALYPSE2_TAU;
                }
                if (the_angle2 > CARPOCALYPSE2_3PI_OVER_2) {
                    cos_angle = gCosine_array[(unsigned int)((CARPOCALYPSE2_TAU - the_angle2) / CARPOCALYPSE2_PI * 128.0)];
                } else if (the_angle2 > CARPOCALYPSE2_PI) {
                    cos_angle = -gCosine_array[(unsigned int)((the_angle2 - CARPOCALYPSE2_PI) / CARPOCALYPSE2_PI * 128.0)];
                } else if (the_angle2 > CARPOCALYPSE2_PI_OVER_2) {
                    cos_angle = -gCosine_array[(unsigned int)((CARPOCALYPSE2_PI - the_angle2) / CARPOCALYPSE2_PI * 128.0)];
                } else {
                    cos_angle = gCosine_array[(unsigned int)(the_angle2 / CARPOCALYPSE2_PI * 128.0)];
                }

                if (the_angle > CARPOCALYPSE2_3PI_OVER_2) {
                    sin_angle = gCosine_array[(unsigned int)((CARPOCALYPSE2_TAU - the_angle) / CARPOCALYPSE2_PI * 128.0)];
                } else if (the_angle > CARPOCALYPSE2_PI) {
                    sin_angle = -gCosine_array[(unsigned int)((the_angle - CARPOCALYPSE2_PI) / CARPOCALYPSE2_PI * 128.0)];
                } else if (the_angle > CARPOCALYPSE2_PI_OVER_2) {
                    sin_angle = -gCosine_array[(unsigned int)((CARPOCALYPSE2_PI - the_angle) / CARPOCALYPSE2_PI * 128.0)];
                } else {
                    sin_angle = gCosine_array[(unsigned int)(the_angle / CARPOCALYPSE2_PI * 128.0)];
                }

                PoshDrawLine(
                    gBack_screen,
                    (int)((double)gProgram_state.current_car.speedo_radius_1[gProgram_state.cockpit_on] * sin_angle
                    + (double)the_wobble_x
                    + (double)gProgram_state.current_car.speedo_centre_x[gProgram_state.cockpit_on]),
                    (float)the_angle,
                    (int)((double)gProgram_state.current_car.speedo_centre_y[gProgram_state.cockpit_on]
                    - (double)gProgram_state.current_car.speedo_radius_1[gProgram_state.cockpit_on] * cos_angle
                    + (double)the_wobble_y),
                    (int)((double)gProgram_state.current_car.speedo_radius_2[gProgram_state.cockpit_on] * sin_angle
                    + (double)the_wobble_x
                    + (double)gProgram_state.current_car.speedo_centre_x[gProgram_state.cockpit_on]),
                    (int)((double)gProgram_state.current_car.speedo_centre_y[gProgram_state.cockpit_on]
                    + (double)the_wobble_y
                    - (double)gProgram_state.current_car.speedo_radius_2[gProgram_state.cockpit_on] * cos_angle),
                    gProgram_state.current_car.speedo_needle_colour[gProgram_state.cockpit_on]);
            } else if (speedo_image != NULL) {
                DrawNumberAt(speedo_image,
                    gCurrent_rev,
                    gProgram_state.current_car.speedo_x[gProgram_state.cockpit_on] + the_wobble_x,
                    gProgram_state.current_car.speedo_y[gProgram_state.cockpit_on] + the_wobble_y,
                    gProgram_state.current_car.speedo_x_pitch[gProgram_state.cockpit_on],
                    gProgram_state.current_car.speedo_y_pitch[gProgram_state.cockpit_on],
                    (int)speed_mph, 3, 1);
            }
        }
    }
#else
    __asm {
            mov ecx, dword ptr [gCar_to_view]
            sub esp, 0x38
            test ecx, ecx
            push ebx
            push ebp
            push esi
            push edi
            je L0044b03c
            cmp dword ptr [ecx + 0xc], 5
            jle L0044b03c
            fld dword ptr [ecx + 0x4a4]
            fmul dword ptr [gFLOAT_00589d28]
            fmul qword ptr [gFLOAT_00589d28+0x8]
            fst qword ptr [esp + 0x30]
            fcomp qword ptr [gFLOAT_00589d28+0x10]
            fnstsw ax
            test ah, 1
            je L0044ab22
            mov dword ptr [esp + 0x30], 0
            mov dword ptr [esp + 0x34], 0
    L0044ab22:
            mov eax, dword ptr [gHeadup_detail_level]
            test eax, eax
            je L0044b001
            cmp eax, 3
            je L0044b001
            mov eax, dword ptr [gProgram_state+0x4c]
            mov edi, 2
            test eax, eax
            je L0044ab6e
            mov edx, dword ptr [gProgram_state+0x50]
            test edx, edx
            jl L0044ab6e
            cmp dword ptr [gProgram_state+0x90], edi
            jne L0044b03c
            mov ebx, dword ptr [gScreen_wobble_x]
            mov ebp, dword ptr [gScreen_wobble_y]
            mov dword ptr [esp + 0x20], ebx
            jmp L0044ab76
    L0044ab6e:
            xor ebx, ebx
            xor ebp, ebp
            mov dword ptr [esp + 0x20], ebx
    L0044ab76:
            mov edx, dword ptr [eax*4 + gProgram_state+0x368]
            mov dword ptr [esp + 0x18], ebp
            test edx, edx
            push 0xb
            jge L0044abe4
            fld dword ptr [ecx + 0x1340]
            call _ftol
            push eax
            call sub_576c00
            mov eax, dword ptr [eax*4 + gRevs]
            mov dword ptr [esp + 0x44], edx
            xor edx, edx
            add esp, 8
            mov dx, word ptr [eax + 0x36]
            mov ecx, dword ptr [gProgram_state+0x4c]
            push edx
            xor edx, edx
            mov dx, word ptr [eax + 0x34]
            push edx
            mov edx, dword ptr [gCurrent_rev]
            push 0
            push 0
            push eax
            mov eax, dword ptr [ecx*4 + gProgram_state+0x338]
            mov ecx, dword ptr [ecx*4 + gProgram_state+0x330]
            add eax, ebp
            add ecx, ebx
            push eax
            push ecx
            push edx
            call BrPixelmapRectangleCopy
            add esp, 0x20
            jmp L0044ac35
    L0044abe4:
            mov eax, dword ptr [ecx + 0x2ec]
            push eax
            call sub_576c00
            mov ecx, dword ptr [gCar_to_view]
            add esp, 8
            mov dword ptr [esp + 0x3c], edx
            fld dword ptr [ecx + 0x1340]
            push eax
            call _ftol
            push eax
            call sub_576c00
            add esp, 8
            mov dword ptr [esp + 0x3c], edx
            test eax, eax
            jge L0044ac1e
            xor eax, eax
            jmp L0044ac28
    L0044ac1e:
            cmp eax, 0xb
            jle L0044ac28
            mov eax, 0xb
    L0044ac28:
            mov edx, dword ptr [eax*4 + gRevs]
            mov dword ptr [gCurrent_rev], edx
    L0044ac35:
            mov esi, dword ptr [gProgram_state+0x4c]
            test esi, esi
            je L0044ac50
            mov eax, dword ptr [gProgram_state+0x50]
            test eax, eax
            jl L0044ac50
            cmp dword ptr [gProgram_state+0x90], edi
            jne L0044aca6
    L0044ac50:
            mov eax, dword ptr [gCar_to_view]
            mov edx, dword ptr [eax + 0x135c]
            test edx, edx
            jge L0044ac62
            or edx, 0xffffffff
    L0044ac62:
            mov ecx, dword ptr [gProgram_state+0x254]
            mov ax, word ptr [ecx + 0x36]
            mov di, word ptr [ecx + 0x34]
            shr ax, 3
            inc edx
            push eax
            imul edx, eax
            push edi
            push edx
            mov dx, word ptr [esi*4 + gProgram_state+0x388]
            push 0
            push ecx
            mov cx, word ptr [esi*4 + gProgram_state+0x390]
            add cx, bp
            add dx, bx
            push ecx
            mov ecx, dword ptr [gCurrent_rev]
            call DRPixelmapRectangleMaskedCopy
            mov esi, dword ptr [gProgram_state+0x4c]
    L0044aca6:
            mov eax, dword ptr [esi*4 + gProgram_state+0x310]
            mov edi, dword ptr [esi*4 + gProgram_state+0x204]
            test eax, eax
            jge L0044ad07
            test edi, edi
            je L0044b03c
            fld qword ptr [esp + 0x30]
            push 1
            push 3
            call _ftol
            mov edx, dword ptr [esi*4 + gProgram_state+0x300]
            mov ecx, dword ptr [esi*4 + gProgram_state+0x2e0]
            push eax
            mov eax, dword ptr [esi*4 + gProgram_state+0x2f8]
            push edx
            mov edx, dword ptr [esi*4 + gProgram_state+0x2d8]
            add ecx, ebp
            push eax
            add edx, ebx
            push ecx
            push edx
            mov edx, dword ptr [gCurrent_rev]
            mov ecx, edi
            call DrawNumberAt
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    L0044ad07:
            test edi, edi
            je L0044ad4f
            test esi, esi
            je L0044ad18
            mov eax, dword ptr [gProgram_state+0x50]
            test eax, eax
            jge L0044ad4f
    L0044ad18:
            mov ax, word ptr [edi + 0x36]
            mov cx, word ptr [edi + 0x34]
            mov dx, word ptr [esi*4 + gProgram_state+0x2e0]
            push eax
            push ecx
            mov ecx, dword ptr [gCurrent_rev]
            push 0
            push 0
            add dx, bp
            push edi
            push edx
            mov dx, word ptr [esi*4 + gProgram_state+0x2d8]
            add dx, bx
            call DRPixelmapRectangleMaskedCopy
            mov esi, dword ptr [gProgram_state+0x4c]
    L0044ad4f:
            fild dword ptr [gProgram_state+0x410]
            fld qword ptr [esp + 0x30]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0044ad6f
            shl esi, 2
            fstp st(0)
            fild dword ptr [esi + gProgram_state+0x320]
            jmp L0044ad98
    L0044ad6f:
            shl esi, 2
            mov eax, dword ptr [esi + gProgram_state+0x318]
            mov ecx, dword ptr [esi + gProgram_state+0x320]
            mov dword ptr [esp + 0x28], eax
            sub ecx, eax
            fild dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            fmul qword ptr [esp + 0x30]
            fdivrp st(2), st(0)
            faddp st(1), st(0)
    L0044ad98:
            fmul qword ptr [gFLOAT_00589d28+0x18]
            fcom qword ptr [gFLOAT_00589d28+0x10]
            fnstsw ax
            test ah, 1
            je L0044adb3
            fsub qword ptr [gFLOAT_00589d28+0x20]
            jmp L0044adc6
    L0044adb3:
            fcom qword ptr [gFLOAT_00589d28+0x28]
            fnstsw ax
            test ah, 1
            jne L0044adc6
            fsub qword ptr [gFLOAT_00589d28+0x28]
    L0044adc6:
            fld qword ptr [gFLOAT_00589d28+0x30]
            fsub st(0), st(1)
            fld st(0)
            fcomp qword ptr [gFLOAT_00589d28+0x10]
            fnstsw ax
            test ah, 1
            je L0044ade3
            fsub qword ptr [gFLOAT_00589d28+0x20]
    L0044ade3:
            fcom qword ptr [gFLOAT_00589d28+0x38]
            fnstsw ax
            test ah, 0x41
            jne L0044ae13
            fld qword ptr [gFLOAT_00589d28+0x28]
            fsub st(0), st(1)
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            shl eax, 2
            mov edx, offset gCosine_array
            sub edx, eax
            fstp st(0)
            _emit 0xd9
        _emit 0x02
            jmp L0044ae8a
    L0044ae13:
            fcom qword ptr [gFLOAT_00589d28+0x48]
            fnstsw ax
            test ah, 0x41
            jne L0044ae41
            fsub qword ptr [gFLOAT_00589d28+0x48]
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            mov ecx, offset gCosine_array
            shl eax, 2
            sub ecx, eax
            _emit 0xd9
        _emit 0x01
            fchs
            jmp L0044ae8a
    L0044ae41:
            fcom qword ptr [gFLOAT_00589d28+0x30]
            fnstsw ax
            test ah, 0x41
            jne L0044ae73
            fld qword ptr [gFLOAT_00589d28+0x48]
            fsub st(0), st(1)
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            mov edx, offset gCosine_array
            shl eax, 2
            sub edx, eax
            fstp st(0)
            _emit 0xd9
        _emit 0x02
            fchs
            jmp L0044ae8a
    L0044ae73:
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            shl eax, 2
            mov ecx, offset gCosine_array
            sub ecx, eax
            _emit 0xd9
        _emit 0x01
    L0044ae8a:
            fld st(1)
            fcomp qword ptr [gFLOAT_00589d28+0x38]
            fnstsw ax
            test ah, 0x41
            jne L0044aec1
            fld qword ptr [gFLOAT_00589d28+0x28]
            fsub st(0), st(2)
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            shl eax, 2
            mov edx, offset gCosine_array
            sub edx, eax
            mov eax, dword ptr [edx]
            mov dword ptr [esp + 0x10], eax
            jmp L0044af48
    L0044aec1:
            fld st(1)
            fcomp qword ptr [gFLOAT_00589d28+0x48]
            fld st(1)
            fnstsw ax
            test ah, 0x41
            jne L0044aef7
            fsub qword ptr [gFLOAT_00589d28+0x48]
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            mov ecx, offset gCosine_array
            shl eax, 2
            sub ecx, eax
            _emit 0xd9
        _emit 0x01
            fchs
            fstp dword ptr [esp + 0x10]
            jmp L0044af48
    L0044aef7:
            fcomp qword ptr [gFLOAT_00589d28+0x30]
            fnstsw ax
            test ah, 0x41
            jne L0044af2b
            fld qword ptr [gFLOAT_00589d28+0x48]
            fsub st(0), st(2)
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            mov edx, offset gCosine_array
            shl eax, 2
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            fchs
            fstp dword ptr [esp + 0x10]
            jmp L0044af48
    L0044af2b:
            fld st(1)
            fmul qword ptr [gFLOAT_00589d28+0x40]
            call _ftol
            shl eax, 2
            mov ecx, offset gCosine_array
            sub ecx, eax
            mov edx, dword ptr [ecx]
            mov dword ptr [esp + 0x10], edx
    L0044af48:
            fild dword ptr [esi + gProgram_state+0x310]
            fild dword ptr [esi + gProgram_state+0x2f0]
            fld st(2)
            fmul st(0), st(2)
            fxch st(1)
            fstp qword ptr [esp + 0x30]
            fild dword ptr [esp + 0x18]
            fxch st(1)
            fsubr qword ptr [esp + 0x30]
            fild dword ptr [esi + gProgram_state+0x2e8]
            fild dword ptr [esi + gProgram_state+0x308]
            fld dword ptr [esp + 0x10]
            fxch st(4)
            fstp qword ptr [esp + 0x28]
            fxch st(2)
            fadd qword ptr [esp + 0x28]
            fild dword ptr [esp + 0x20]
            fxch st(4)
            fstp qword ptr [esp + 0x20]
            mov eax, dword ptr [esi + gProgram_state+0x328]
            fxch st(1)
            fstp qword ptr [esp + 0x38]
            fxch st(2)
            fstp qword ptr [esp + 0x10]
            push eax
            fstp qword ptr [esp + 0x1c]
            call _ftol
            fld qword ptr [esp + 0x24]
            fmul st(0), st(1)
            push eax
            fadd qword ptr [esp + 0x18]
            fadd qword ptr [esp + 0x40]
            call _ftol
            fstp st(0)
            fmul qword ptr [esp + 0x20]
            push eax
            fsubr qword ptr [esp + 0x3c]
            fadd qword ptr [esp + 0x34]
            call _ftol
            fld qword ptr [esp + 0x2c]
            fmul qword ptr [esp + 0x24]
            push eax
            fadd qword ptr [esp + 0x20]
            fadd qword ptr [esp + 0x48]
            call _ftol
            push ecx
            mov ecx, dword ptr [gBack_screen]
            mov edx, eax
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            call PoshDrawLine
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    L0044b001:
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L0044b03c
            fld qword ptr [esp + 0x30]
            call _ftol
            push eax
            lea ecx, [esp + 0x44]
            push offset gPrev_ps_drawn_levels+0x1e4
            push ecx
            call sprintf
            add esp, 0xc
            mov edx, 1
            lea ecx, [esp + 0x40]
            push 0
            push 0
            push 0x14
            push 1
            call PolyFontText
    L0044b03c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047c740
void C2_NAKED C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDest, int pX1, br_scalar pAngle, int pY1, int pX2, int pY2, int pColour) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pColour < 0) {
        /* NEG and coordinate offsets wrap as original 32-bit integer operations. */
        if (pAngle >= 0.785 && pAngle <= 5.498 && (pAngle <= 2.356 || pAngle >= 3.926)) {
            if ((pAngle <= 0.785 || pAngle >= 1.57) && (pAngle <= 3.926 || pAngle >= 4.712)) {
                DRDrawLine(pDest, (int)((tU32)pX1 - 1u), pY1, (int)((tU32)pX2 - 1u), pY2, (int)(0u - (tU32)pColour - 1u));
                DRDrawLine(pDest, (int)((tU32)pX1 + 1u), pY1, (int)((tU32)pX2 + 1u), pY2, (int)(1u - (tU32)pColour));
            } else {
                DRDrawLine(pDest, (int)((tU32)pX1 - 1u), pY1, (int)((tU32)pX2 - 1u), pY2, (int)(1u - (tU32)pColour));
                DRDrawLine(pDest, (int)((tU32)pX1 + 1u), pY1, (int)((tU32)pX2 + 1u), pY2, (int)(0u - (tU32)pColour - 1u));
            }
        } else {
            DRDrawLine(pDest, pX1, (int)((tU32)pY1 + 1u), pX2, (int)((tU32)pY2 + 1u), (int)(0u - (tU32)pColour - 1u));
            DRDrawLine(pDest, pX1, (int)((tU32)pY1 - 1u), pX2, (int)((tU32)pY2 - 1u), (int)(1u - (tU32)pColour));
        }
        DRDrawLine(pDest, pX1, pY1, pX2, pY2, (int)(0u - (tU32)pColour));
    } else {
        DRDrawLine(pDest, pX1, pY1, pX2, pY2, pColour);
    }
#else
    __asm {
            push ecx
            push ebx
            mov ebx, edx
            mov edx, dword ptr [esp + 0x1c]
            push esi
            mov esi, ecx
            test edx, edx
            mov dword ptr [esp + 8], esi
            jl L0047c791
            mov ecx, dword ptr [gBack_screen]
            mov eax, edx
            cmp byte ptr [ecx + 0x2c], 3
            je L0047c771
            mov ecx, dword ptr [gRender_palette]
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047c771:
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x14]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push edx
            push eax
            push ecx
            push ebx
            push esi
            call BrPixelmapLine
            add esp, 0x18
            pop esi
            pop ebx
            pop ecx
            ret 0x14
    L0047c791:
            fld dword ptr [esp + 0x10]
            fcomp qword ptr [gDouble_0058a068+0x150]
            push edi
            push ebp
            fnstsw ax
            test ah, 1
            jne L0047c8f1
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x158]
            fnstsw ax
            test ah, 0x41
            je L0047c8f1
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x160]
            fnstsw ax
            test ah, 0x41
            jne L0047c7e3
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x168]
            fnstsw ax
            test ah, 1
            jne L0047c8f1
    L0047c7e3:
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x150]
            fnstsw ax
            test ah, 0x41
            jne L0047c805
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x170]
            fnstsw ax
            test ah, 1
            jne L0047c827
    L0047c805:
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x168]
            fnstsw ax
            test ah, 0x41
            jne L0047c875
            fld dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a068+0x178]
            fnstsw ax
            test ah, 1
            je L0047c875
    L0047c827:
            mov eax, 1
            sub eax, edx
            mov edx, dword ptr [gBack_screen]
            cmp byte ptr [edx + 0x2c], 3
            je L0047c84c
            mov ecx, dword ptr [gRender_palette]
            mov edx, eax
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047c84c:
            mov edi, dword ptr [esp + 0x20]
            mov esi, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x10]
            push eax
            lea eax, [edi - 1]
            push esi
            push eax
            lea ecx, [ebx - 1]
            push ebp
            push ecx
            push edx
            call BrPixelmapLine
            add esp, 0x18
            or edx, 0xffffffff
            jmp L0047c8c1
    L0047c875:
            mov ecx, dword ptr [gBack_screen]
            or eax, 0xffffffff
            sub eax, edx
            cmp byte ptr [ecx + 0x2c], 3
            je L0047c898
            mov ecx, dword ptr [gRender_palette]
            mov edx, eax
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047c898:
            mov edi, dword ptr [esp + 0x20]
            mov esi, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x10]
            push eax
            lea edx, [edi - 1]
            push esi
            push edx
            lea eax, [ebx - 1]
            push ebp
            push eax
            push ecx
            call BrPixelmapLine
            add esp, 0x18
            mov edx, 1
    L0047c8c1:
            mov eax, dword ptr [esp + 0x28]
            sub edx, eax
            mov eax, dword ptr [gBack_screen]
            cmp byte ptr [eax + 0x2c], 3
            je L0047c8e4
            mov ecx, dword ptr [gRender_palette]
            call PaletteEntry16Bit
            and eax, 0xffff
            mov edx, eax
    L0047c8e4:
            push edx
            lea ecx, [edi + 1]
            push esi
            push ecx
            lea edx, [ebx + 1]
            push ebp
            push edx
            jmp L0047c96b
    L0047c8f1:
            mov ecx, dword ptr [gBack_screen]
            or eax, 0xffffffff
            sub eax, edx
            cmp byte ptr [ecx + 0x2c], 3
            je L0047c914
            mov ecx, dword ptr [gRender_palette]
            mov edx, eax
            call PaletteEntry16Bit
            and eax, 0xffff
    L0047c914:
            mov esi, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x1c]
            mov edi, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esi + 1]
            push eax
            push edx
            lea eax, [ebp + 1]
            push edi
            push eax
            push ebx
            push ecx
            call BrPixelmapLine
            mov eax, dword ptr [esp + 0x40]
            mov edx, 1
            sub edx, eax
            mov eax, dword ptr [gBack_screen]
            add esp, 0x18
            cmp byte ptr [eax + 0x2c], 3
            je L0047c960
            mov ecx, dword ptr [gRender_palette]
            call PaletteEntry16Bit
            and eax, 0xffff
            mov edx, eax
    L0047c960:
            lea ecx, [esi - 1]
            push edx
            push ecx
            lea edx, [ebp - 1]
            push edi
            push edx
            push ebx
    L0047c96b:
            mov eax, dword ptr [esp + 0x24]
            push eax
            call BrPixelmapLine
            mov ecx, dword ptr [gBack_screen]
            mov edx, dword ptr [esp + 0x40]
            add esp, 0x18
            mov al, byte ptr [ecx + 0x2c]
            neg edx
            cmp al, 3
            je L0047c99d
            mov ecx, dword ptr [gRender_palette]
            call PaletteEntry16Bit
            and eax, 0xffff
            mov edx, eax
    L0047c99d:
            push edx
            mov edx, dword ptr [esp + 0x14]
            push esi
            push edi
            push ebp
            push ebx
            push edx
            call BrPixelmapLine
            add esp, 0x18
            pop ebp
            pop edi
            pop esi
            pop ebx
            pop ecx
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044ba60
int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth) {
    int result;

    if (gDevious_2d == 0 || gTexture_power_of_2 == 0) {
        return pWidth;
    }
    result = 1;
    while (result < pWidth) {
        result <<= 1;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x0044ba90
int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight) {
    int result;

    if (gDevious_2d == 0 || gTexture_power_of_2 == 0) {
        return pHeight;
    }
    result = 1;
    while (result < pHeight) {
        result <<= 1;
    }
    return result;
}

// DrawThisCarIconNow


// FUNCTION: CARMA2_HW 0x0044a920
void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont) {
    if (pHeadup_index >= 0) {
        gHeadups[pHeadup_index].data.image_info.font = &gFonts[-pFont];
    }
}

extern br_colour gColours[9];

// FUNCTION: CARMA2_HW 0x0044a9b0
void C2_HOOK_FASTCALL ChangeHeadupColour(int pHeadup_index, int pNew_colour) {
    if (pHeadup_index < 0) {
    } else {
        gHeadups[pHeadup_index].data.text_info.colour = gColours[pNew_colour];
    }
}

// FUNCTION: CARMA2_HW 0x0044a8d0
void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        strcpy(the_headup->data.text_info.text, pNew_text);
        MungeHeadupWidth(the_headup);
    }
}

// FUNCTION: CARMA2_HW 0x00449070
void C2_HOOK_FASTCALL TimerStringGlobalTimer(char* pStr, undefined4 pArg3) {
    TimerString(gTimer, pStr, pArg3, 0, 0);
}

extern br_pixelmap* gHeadup_images[45];

// FUNCTION: CARMA2_HW 0x0044a950
void C2_HOOK_FASTCALL ChangeHeadupImage(int pHeadup_index, int pImage_index) {
    tHeadup* headup;
    br_pixelmap* image;
    int justification;

    if (pHeadup_index >= 0) {
        image = gHeadup_images[pImage_index];
        headup = &gHeadups[pHeadup_index];
        justification = headup->justification;
        *(br_pixelmap**)headup->data.image_info.text = image;
        switch (justification) {
        case eJust_left:
            headup->x = headup->original_x;
            break;
        case eJust_right:
            headup->x = headup->original_x - image->width;
            return;
        case eJust_centre:
            headup->x = headup->original_x - (image->width >> 1);
            return;
        default:
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047c9c0
void C2_HOOK_FASTCALL DrawRectangleOutlineWithPalette(br_pixelmap* pMap, int pLeft, int pTop, int pRight, int pBottom, br_uint_32 pColour) {
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapLine(pMap, pLeft, pTop, pRight, pTop, pColour);
    BrPixelmapLine(pMap, pLeft, pBottom, pRight, pBottom, pColour);
    BrPixelmapLine(pMap, pLeft, pTop, pLeft, pBottom, pColour);
    BrPixelmapLine(pMap, pRight, pTop, pRight, pBottom, pColour);
}

// FUNCTION: CARMA2_HW 0x00576850
int C2_HOOK_CDECL FUN_00576850(int pValue) {
    return pValue + 0x20;
}

// FUNCTION: CARMA2_HW 0x0044a740
void C2_NAKED C2_HOOK_FASTCALL UpdateHeadupPositions(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int cockpit_on;
    int cockpit_image_index;
    int count;
    tHeadup* headup;
    tHeadup_slot* headup_slots;

    cockpit_on = gProgram_state.cockpit_on;
    cockpit_image_index = gProgram_state.cockpit_image_index;
    headup_slots = &gProgram_state.current_car.headup_slots[0][0];
    for (headup = &gHeadups[0], count = 37; count != 0; count--, headup++) {
        int side_a;
        int side_b;
        int x_delta;
        int y_delta;

        if (headup->type == eHeadup_unused) {
            continue;
        }
        if (cockpit_on != 0 && cockpit_image_index >= 0) {
            side_a = 1;
            side_b = 0;
        } else {
            side_a = 0;
            side_b = 1;
        }
        x_delta = headup_slots[headup->slot_index + side_a * 20].x
                - headup_slots[headup->slot_index + side_b * 20].x;
        y_delta = headup_slots[headup->slot_index + side_a * 20].y
                - headup_slots[headup->slot_index + side_b * 20].y;
        headup->original_x += x_delta;
        headup->x += x_delta;
        headup->y += y_delta;
        headup->dim_left += x_delta;
        headup->dim_top += y_delta;
        headup->dim_right += x_delta;
        headup->dim_bottom += y_delta;
    }
#else
    __asm {
            push ecx
            push ebx
            mov ebx, dword ptr [gProgram_state+0x4c]
            push ebp
            mov ebp, dword ptr [gProgram_state+0x50]
            push esi
            push edi
            mov eax, offset gHeadups+0x18
            mov dword ptr [esp + 0x10], 0x25
    L0044a75e:
            mov ecx, dword ptr [eax - 0x18]
            test ecx, ecx
            je L0044a83f
            test ebx, ebx
            je L0044a778
            test ebp, ebp
            jl L0044a778
            mov esi, 1
            jmp L0044a77a
    L0044a778:
            xor esi, esi
    L0044a77a:
            test ebx, ebx
            je L0044a786
            test ebp, ebp
            jl L0044a786
            xor ecx, ecx
            jmp L0044a78b
    L0044a786:
            mov ecx, 1
    L0044a78b:
            mov edx, dword ptr [eax]
            lea ecx, [ecx + ecx*4]
            lea ecx, [edx + ecx*4]
            lea edi, [ecx + ecx*4]
            lea edi, [ecx + edi*2]
            lea ecx, [esi + esi*4]
            lea ecx, [edx + ecx*4]
            mov esi, dword ptr [edi*4 + gProgram_state+0x7d4]
            lea edx, [ecx + ecx*4]
            lea ecx, [ecx + edx*2]
            mov ecx, dword ptr [ecx*4 + gProgram_state+0x7d4]
            sub ecx, esi
            test ebx, ebx
            je L0044a7c4
            test ebp, ebp
            jl L0044a7c4
            mov edi, 1
            jmp L0044a7c6
    L0044a7c4:
            xor edi, edi
    L0044a7c6:
            test ebx, ebx
            je L0044a7d2
            test ebp, ebp
            jl L0044a7d2
            xor edx, edx
            jmp L0044a7d7
    L0044a7d2:
            mov edx, 1
    L0044a7d7:
            mov esi, dword ptr [eax]
            lea edx, [edx + edx*4]
            lea edx, [esi + edx*4]
            lea ebp, [edx + edx*4]
            lea ebp, [edx + ebp*2]
            lea edx, [edi + edi*4]
            mov edi, dword ptr [eax - 0xc]
            lea edx, [esi + edx*4]
            add edi, ecx
            mov dword ptr [eax - 0xc], edi
            mov edi, dword ptr [eax + 0xc]
            lea esi, [edx + edx*4]
            lea edx, [edx + esi*2]
            mov esi, dword ptr [ebp*4 + gProgram_state+0x7d8]
            mov ebp, dword ptr [eax - 0x14]
            mov edx, dword ptr [edx*4 + gProgram_state+0x7d8]
            add ebp, ecx
            sub edx, esi
            mov esi, dword ptr [eax - 0x10]
            add esi, edx
            mov dword ptr [eax - 0x14], ebp
            mov ebp, dword ptr [eax + 8]
            mov dword ptr [eax - 0x10], esi
            mov esi, dword ptr [eax + 0x10]
            add ebp, ecx
            add esi, ecx
            mov ecx, dword ptr [eax + 0x14]
            add edi, edx
            add ecx, edx
            mov dword ptr [eax + 8], ebp
            mov ebp, dword ptr [gProgram_state+0x50]
            mov dword ptr [eax + 0xc], edi
            mov dword ptr [eax + 0x10], esi
            mov dword ptr [eax + 0x14], ecx
    L0044a83f:
            mov ecx, dword ptr [esp + 0x10]
            add eax, 0x164
            dec ecx
            mov dword ptr [esp + 0x10], ecx
            jne L0044a75e
            pop edi
            pop esi
            pop ebp
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044b050
void C2_NAKED C2_HOOK_FASTCALL DrawCockpitHands(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_pixelmap* image;

    if (gProgram_state.current_car_index != gProgram_state.current_car.index
            || !gProgram_state.cockpit_on
            || gProgram_state.cockpit_image_index < 0
            || gProgram_state.which_view != eView_forward) {
        return;
    }

    i = (int)floor((double)(gProgram_state.current_car.number_of_hands_images
        * (gFLOAT_00589d7c - gProgram_state.current_car.steering_angle * gFLOAT_00589d78))
        * gDOUBLE_00589d80);
    if (i < 0) {
        i = 0;
    } else if (i >= gProgram_state.current_car.number_of_hands_images) {
        i = gProgram_state.current_car.number_of_hands_images - 1;
    }

    image = gProgram_state.current_car.lhands_images[i];
    if (image != NULL) {
        DRPixelmapRectangleMaskedCopy(gBack_screen,
            (br_int_16)(gProgram_state.current_car.lhands_x[i] + gScreen_wobble_x),
            (br_int_16)(gProgram_state.current_car.lhands_y[i] + gScreen_wobble_y),
            image, 0, 0, image->width, image->height);
    }

    image = gProgram_state.current_car.rhands_images[i];
    if (image != NULL) {
        DRPixelmapRectangleMaskedCopy(gBack_screen,
            (br_int_16)(gProgram_state.current_car.rhands_x[i] + gScreen_wobble_x),
            (br_int_16)(gProgram_state.current_car.rhands_y[i] + gScreen_wobble_y),
            image, 0, 0, image->width, image->height);
    }
#else
    __asm {
            mov eax, dword ptr [gProgram_state+0x8c]
            mov ecx, dword ptr [gProgram_state+0xac]
            cmp eax, ecx
            push esi
            jne L0044b160
            mov eax, dword ptr [gProgram_state+0x4c]
            test eax, eax
            je L0044b160
            mov eax, dword ptr [gProgram_state+0x50]
            test eax, eax
            jl L0044b160
            cmp dword ptr [gProgram_state+0x90], 2
            jne L0044b160
            fld dword ptr [gProgram_state+0x54c]
            fmul dword ptr [gFLOAT_00589d78]
            fild dword ptr [gProgram_state+0x40c]
            fxch st(1)
            fsubr dword ptr [gFLOAT_00589d7c]
            fxch st(1)
            sub esp, 8
            fmulp st(1), st(0)
            fmul qword ptr [gDOUBLE_00589d80]
            fstp qword ptr [esp]
            call floor
            add esp, 8
            call _ftol
            mov esi, eax
            test esi, esi
            jge L0044b0cc
            xor esi, esi
            jmp L0044b0d8
    L0044b0cc:
            mov eax, dword ptr [gProgram_state+0x40c]
            cmp esi, eax
            jl L0044b0d8
            lea esi, [eax - 1]
    L0044b0d8:
            mov eax, dword ptr [esi*4 + gProgram_state+0x218]
            test eax, eax
            je L0044b11c
            mov cx, word ptr [eax + 0x36]
            mov dx, word ptr [eax + 0x34]
            push ecx
            push edx
            mov dx, word ptr [esi*4 + gProgram_state+0x39c]
            push 0
            add dx, word ptr [gScreen_wobble_x]
            mov ecx, dword ptr [gBack_screen]
            push 0
            push eax
            mov ax, word ptr [esi*4 + gProgram_state+0x3b8]
            add ax, word ptr [gScreen_wobble_y]
            push eax
            call DRPixelmapRectangleMaskedCopy
    L0044b11c:
            mov eax, dword ptr [esi*4 + gProgram_state+0x234]
            test eax, eax
            je L0044b160
            mov cx, word ptr [eax + 0x36]
            mov dx, word ptr [eax + 0x34]
            push ecx
            push edx
            mov dx, word ptr [esi*4 + gProgram_state+0x3d4]
            push 0
            add dx, word ptr [gScreen_wobble_x]
            mov ecx, dword ptr [gBack_screen]
            push 0
            push eax
            mov ax, word ptr [esi*4 + gProgram_state+0x3f0]
            add ax, word ptr [gScreen_wobble_y]
            push eax
            call DRPixelmapRectangleMaskedCopy
    L0044b160:
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
            nop
            nop
    }
#endif
}
