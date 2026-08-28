#ifndef GUARD_15_DISPLAYS_H
#define GUARD_15_DISPLAYS_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "carpocalypse2_types.h"

extern int gHeadup_detail_level;

extern tQueued_headup gQueued_headups[4];
extern int gQueued_headup_count;
extern tU32 gLast_centre_headup;

extern int gDRFont_to_polyfont_mapping[24];

extern int gCredits_won_headup;
extern int gPed_kill_count_headup;
extern int gCar_kill_count_headup;
extern int gTimer_headup;
extern int gTime_awarded_headup;
extern int gLaps_headup;
extern int gNet_cash_headup;
extern int gNet_ped_headup;

extern int gLast_fancy_index;
extern int gLast_credit_headup__displays;
extern int gLast_time_credit_headup;
extern tU32 gLast_earn_time;
extern tU32 gLast_fancy_time;
extern tU32 gLast_time_earn_time;
extern tU32 gLast_fancy_headup;
extern int gOld_times[10];

extern br_font* gBR_fonts[4];
extern br_pixelmap* gRevs[13];
extern br_pixelmap* gArmour_colour_map;
extern br_pixelmap* gPowerbar;
extern br_pixelmap* gPower_colour_map;
extern br_pixelmap* gOffensive_colour_map;
extern br_pixelmap* gIcon_test;
extern br_pixelmap* gCurrent_rev;
extern br_pixelmap* gHud_curve1;
extern br_pixelmap* gHud_curve2;
extern br_pixelmap* gHud_curve3;
extern br_pixelmap* gGrey_top5;
extern br_pixelmap* gDamage_hud;
extern br_pixelmap* gTimer_lhs;
extern br_pixelmap* gHud_timer_rhs;
extern br_pixelmap* gIcon_armour;
extern br_pixelmap* gIcon_power;
extern br_pixelmap* gIcon_offense;
extern br_pixelmap* gIcon_greyBloc1;
extern br_pixelmap* gIcon_litBloc1;
extern br_pixelmap* gIcon_grnBlock1;
extern tHud_message gHud_messages[46];
extern int gCredit_multiplier;
extern int gLast_credit_amount;
extern int gPrev_ps_apo_levels[3];
extern int gPrev_ps_drawn_levels[3];
extern int gPS_apo_level_changed[3];

extern int gBack_original_origin_x;
extern int gBack_original_origin_y;
extern int gBack_original_base_x;
extern int gBack_original_base_y;

extern void C2_HOOK_FASTCALL InitHeadups(void);

extern void C2_HOOK_FASTCALL ClearHeadup(int pIndex);

extern void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index);

extern void C2_HOOK_FASTCALL ClearHeadups(void);

extern void C2_HOOK_FASTCALL DimAFewBits(void);

extern void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void);

extern void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

extern void C2_HOOK_FASTCALL DoPSPowerHeadup(int pLevel, const char* pAPO_str);

extern void C2_HOOK_FASTCALL DoPSPowerupHeadups(void);

extern int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index);

extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);

extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);

extern int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index);

extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);

extern void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pX, int pY);

// IsHeadupTextClever

// MungeHeadupWidth

extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text);

extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);

extern void C2_HOOK_FASTCALL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time);

extern void C2_HOOK_FASTCALL DoHeadups(tU32 pThe_time);

extern void C2_HOOK_FASTCALL DoInstruments(tU32 pThe_time);

extern void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDestn, int pX1, float pAngle, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL DoTestHeadup(void);

// ChangingView

// EarnCredits2

// EarnCredits

// EarnCredits100

// SpendCredits

// AwardTime

// AwardTime5

extern int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth);

extern int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight);

// DrawThisCarIconNow

#endif // GUARD_15_DISPLAYS_H