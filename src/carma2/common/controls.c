#include "controls.h"

#include "brucetrk.h"
#include "camera.h"
#include "car.h"
#include "crush.h"
#include "depth.h"
#include "displays.h"
#include "drone.h"
#include "errors.h"
#include "finteray.h"

#include "frontend_controls.h"

#include "globvars.h"
#include "globvrkm.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "joystick.h"
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
#include "win32/win32_dinput.h"
#endif
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "polyfont.h"
#include "powerup.h"
#include "powerups.h"
#include "pratcam.h"
#include "raycast.h"
#include "replay.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include "brender/brender.h"

#include "c2_string.h"
#include "c2_stdlib.h"

#include "carpocalypse2_macros.h"
#include "controls.h"

#include "globvars.h"
#include "platform.h"
#include "sound.h"
#include "loading1.h"
#include "input.h"

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"

/* The retail compiler inlined this single-call-site helper into PollCarControls. */
#if defined(_MSC_VER)
#define C2_MATCHING_INLINE __inline
#else
#define C2_MATCHING_INLINE inline
#endif
#else
#define C2_MATCHING_INLINE
#endif
extern tU32* C2_HOOK_FASTCALL KevKeyService(void);
/* VC5 float-to-int runtime helper, called directly from transcribed asm. */
// GLOBAL: CARMA2_HW 0x0067c474
int gEntering_message;

// GLOBAL: CARMA2_HW 0x00590970
tCheat gKev_keys[] = {
    { 0xa11ee75d, 0xa11ee75d, SetFlag,    0xa11ee75d },
    { 0x398da28c, 0x398da28c, SetFlag,    0x564e78b9 },
    { 0x7dc510f3, 0x7dc510f3, SetFlag2,   0x00000001 },
    { 0x309e4f55, 0x309e4f55, FinishRace, 0x00000000 },
    { 0x1bcbe148, 0x1bcbe148, GetPowerup, 0x00000000 },
    { 0x1d5e7725, 0x1d5e7725, GetPowerup, 0x00000001 },
    { 0x22c65063, 0x22c65063, GetPowerup, 0x00000002 },
    { 0x1a37d28a, 0x1a37d28a, GetPowerup, 0x00000003 },
    { 0x1dcba360, 0x1dcba360, GetPowerup, 0x00000004 },
    { 0x24c99afb, 0x24c99afb, GetPowerup, 0x00000005 },
    { 0x200c1bd4, 0x200c1bd4, GetPowerup, 0x00000006 },
    { 0x252a2e6b, 0x252a2e6b, GetPowerup, 0x00000007 },
    { 0x218f555c, 0x218f555c, GetPowerup, 0x00000008 },
    { 0x1fc7655b, 0x1fc7655b, GetPowerup, 0x00000009 },
    { 0x2b2e6891, 0x2b2e6891, GetPowerup, 0x0000000a },
    { 0x2db8b34a, 0x2db8b34a, GetPowerup, 0x0000000b },
    { 0x3001467e, 0x3001467e, GetPowerup, 0x0000000c },
    { 0x23968eda, 0x23968eda, GetPowerup, 0x0000000d },
    { 0x1f3baa55, 0x1f3baa55, GetPowerup, 0x0000000f },
    { 0x214a2558, 0x214a2558, GetPowerup, 0x00000010 },
    { 0x373ae69a, 0x373ae69a, GetPowerup, 0x00000011 },
    { 0x327ebd75, 0x327ebd75, GetPowerup, 0x00000012 },
    { 0x350c0384, 0x350c0384, GetPowerup, 0x00000013 },
    { 0x17f03c24, 0x17f03c24, GetPowerup, 0x00000014 },
    { 0x32aeca21, 0x32aeca21, GetPowerup, 0x00000015 },
    { 0x191841aa, 0x191841aa, GetPowerup, 0x00000016 },
    { 0x26026896, 0x26026896, GetPowerup, 0x00000017 },
    { 0x2440ca1b, 0x2440ca1b, GetPowerup, 0x00000018 },
    { 0x37a11b1b, 0x37a11b1b, GetPowerup, 0x00000019 },
    { 0x2f2ea509, 0x2f2ea509, GetPowerup, 0x0000001a },
    { 0x28f522f1, 0x28f522f1, GetPowerup, 0x0000001b },
    { 0x26c15553, 0x26c15553, GetPowerup, 0x00000020 },
    { 0x3964b52b, 0x3964b52b, GetPowerup, 0x00000021 },
    { 0x18bf123a, 0x18bf123a, GetPowerup, 0x00000022 },
    { 0x2a439e13, 0x2a439e13, GetPowerup, 0x00000023 },
    { 0x28769902, 0x28769902, GetPowerup, 0x00000024 },
    { 0x2d5aa4e5, 0x2d5aa4e5, GetPowerup, 0x00000025 },
    { 0x1e73b354, 0x1e73b354, GetPowerup, 0x00000026 },
    { 0x1cac0a7c, 0x1cac0a7c, GetPowerup, 0x00000027 },
    { 0x1e3c613a, 0x1e3c613a, GetPowerup, 0x00000028 },
    { 0x2f4c3519, 0x2f4c3519, GetPowerup, 0x00000029 },
    { 0x21f0d261, 0x21f0d261, GetPowerup, 0x0000002a },
    { 0x1c727344, 0x1c727344, GetPowerup, 0x0000002b },
    { 0x2f574845, 0x2f574845, GetPowerup, 0x0000002c },
    { 0x1f0601e3, 0x1f0601e3, GetPowerup, 0x0000002d },
    { 0x26219ff3, 0x26219ff3, GetPowerup, 0x0000002f },
    { 0x26afbb31, 0x26afbb31, GetPowerup, 0x00000030 },
    { 0x25205546, 0x25205546, GetPowerup, 0x00000031 },
    { 0x1a0a8e5b, 0x1a0a8e5b, GetPowerup, 0x00000033 },
    { 0x1bdea925, 0x1bdea925, GetPowerup, 0x00000034 },
    { 0x2d4dd2a9, 0x2d4dd2a9, GetPowerup, 0x00000035 },
    { 0x2e7a7505, 0x2e7a7505, GetPowerup, 0x00000036 },
    { 0x17290940, 0x17290940, GetPowerup, 0x00000037 },
    { 0x1d4e7a9c, 0x1d4e7a9c, GetPowerup, 0x00000038 },
    { 0x3579d64a, 0x3579d64a, GetPowerup, 0x00000039 },
    { 0x28f4d49c, 0x28f4d49c, GetPowerup, 0x0000003a },
    { 0x1d6ba9c3, 0x1d6ba9c3, GetPowerup, 0x0000003b },
    { 0x310971ab, 0x310971ab, GetPowerup, 0x0000003c },
    { 0x28451eeb, 0x28451eeb, GetPowerup, 0x0000003d },
    { 0x1ebfa5ba, 0x1ebfa5ba, GetPowerup, 0x0000003e },
    { 0x27079773, 0x27079773, GetPowerup, 0x0000003f },
    { 0x388de72c, 0x388de72c, GetPowerup, 0x00000040 },
    { 0x1a0da9fc, 0x1a0da9fc, GetPowerup, 0x00000041 },
    { 0x2975a10c, 0x2975a10c, GetPowerup, 0x00000042 },
    { 0x1e5cc6ca, 0x1e5cc6ca, GetPowerup, 0x00000043 },
    { 0x250c6f99, 0x250c6f99, GetPowerup, 0x00000044 },
    { 0x33950e49, 0x33950e49, GetPowerup, 0x00000045 },
    { 0x4e5f487a, 0x4e5f487a, GetPowerup, 0x00000046 },
    { 0x3815584c, 0x3815584c, GetPowerup, 0x00000047 },
    { 0x459732b2, 0x459732b2, GetPowerup, 0x00000048 },
    { 0x62871003, 0x62871003, GetPowerup, 0x00000049 },
    { 0x2d72ebb4, 0x2d72ebb4, GetPowerup, 0x0000004a },
    { 0x2c3be2aa, 0x2c3be2aa, GetPowerup, 0x0000004b },
    { 0x2b2be28b, 0x2b2be28b, GetPowerup, 0x0000004c },
    { 0x29be089d, 0x29be089d, GetPowerup, 0x0000004d },
    { 0x4897982d, 0x4897982d, GetPowerup, 0x0000004e },
    { 0x44d50f49, 0x44d50f49, GetPowerup, 0x0000004f },
    { 0x403afae5, 0x403afae5, GetPowerup, 0x00000050 },
    { 0x45c19e5e, 0x45c19e5e, GetPowerup, 0x00000051 },
    { 0x2f790ebd, 0x2f790ebd, GetPowerup, 0x00000052 },
    { 0x244f60c9, 0x244f60c9, GetPowerup, 0x00000053 },
    { 0x2b0794d3, 0x2b0794d3, GetPowerup, 0x00000054 },
    { 0x2a44b628, 0x2a44b628, GetPowerup, 0x00000055 },
    { 0x2998e46d, 0x2998e46d, GetPowerup, 0x00000056 },
    { 0x23248728, 0x23248728, GetPowerup, 0x00000057 },
    { 0x289c1822, 0x289c1822, GetPowerup, 0x00000058 },
    { 0x35abb7d0, 0x35abb7d0, GetPowerup, 0x0000005a },
    { 0x1c1fdd92, 0x1c1fdd92, GetPowerup, 0x0000005b },
    { 0x253069c1, 0x253069c1, GetPowerup, 0x0000005c },
    { 0x33ca4873, 0x33ca4873, GetPowerup, 0x0000005d },
    { 0x1f56cde5, 0x1f56cde5, GetPowerup, 0x0000005e },
    { 0x1784995b, 0x1784995b, GetPowerup, 0x00000060 },
    { 0x3003eccb, 0x3003eccb, GetPowerup, 0x00000061 },
    { 0x4b054b60, 0x4b054b60, RichardsSphere, 0x00000000 },
    { 0x00000000, 0x00000000, NULL,       0x00000000 },
};

// GLOBAL: CARMA2_HW 0x005900a0
tToggle_element gToggle_array[44] = {
    { 0x23, -2, 1, 1, 0, ToggleMiniMap },
    { 0x4a, -2, 1, 1, 0, ToggleFlaps },
    { 0x39, -2, 1, 1, 0, ToggleMap },
    { 0x39, 8, 1, 1, 0, ToggleHeadupMap },
    { 0x39, 7, 1, 1, 0, CycleMapOptions },
    { 0x39, 9, 1, 1, 0, ToggleMapTransparency },
    { 0x3c, -2, 1, 1, 0, SetRecovery },
    { 0x04, 7, 1, 0, 0, AbortRace },
    { 0x25, -2, 0, 1, 0, ToggleSoundEnable },
    { 0x05, 8, 0, 1, 0, PrintScreen },
    { 0x4c, -2, 1, 0, 0, ToggleHeadupLevel },
    { 0x12, -2, 0, 0, 0, F4Key },
    { 0x13, -2, 1, 0, 0, F5Key },
    { 0x14, -2, 1, 0, 0, F6Key },
    { 0x15, -2, 1, 0, 0, F7Key },
    { 0x16, -2, 1, 0, 0, F8Key },
    { 0x17, -2, 1, 0, 0, F10Key },
    { 0x18, -2, 1, 0, 0, F11Key },
    { 0x19, -2, 1, 0, 0, F12Key },
    { 0x0f, -2, 1, 0, 0, NumberKey0 },
    { 0x26, -2, 1, 0, 0, NumberKey1 },
    { 0x27, -2, 1, 0, 0, NumberKey2 },
    { 0x28, -2, 1, 0, 0, NumberKey3 },
    { 0x29, -2, 1, 0, 0, NumberKey4 },
    { 0x2a, -2, 1, 0, 0, NumberKey5 },
    { 0x2b, -2, 1, 0, 0, NumberKey6 },
    { 0x2c, -2, 1, 0, 0, NumberKey7 },
    { 0x10, -2, 1, 0, 0, NumberKey8 },
    { 0x11, -2, 1, 0, 0, NumberKey9 },
    { 0x0b, 7, 1, 1, 0, ToggleFlying },
    { 0x41, -2, 1, 0, 0, ScreenSmaller },
    { 0x42, -2, 1, 0, 0, ScreenLarger },
    { 0x43, -2, 1, 0, 0, BuyArmour },
    { 0x44, -2, 1, 0, 0, BuyPower },
    { 0x45, -2, 1, 0, 0, BuyOffense },
    { 0x46, -2, 1, 0, 0, ViewNetPlayer },
    { 0x47, -2, 1, 0, 0, UserSendMessage },
    { 0x48, -2, 1, 0, 0, ToggleTargetLock },
    { 0x49, -2, 1, 0, 0, CycleTargetLock },
    { 0x3e, -2, 1, 0, 0, ToggleInventory },
    { 0x3f, -2, 1, 0, 0, LeftInventory },
    { 0x40, -2, 1, 0, 0, RightInventory },
    { 0x48, -2, 1, 0, 0, ChangeCameraTypeInGame },
    { 0x2e, -2, 1, 0, 0, ToggleCockpit },
};

// GLOBAL: CARMA2_HW 0x0067c3f8
int gRecovery_voucher_count;

// GLOBAL: CARMA2_HW 0x006923d8
tPhysics_object* gUnknown_006923d8;

// GLOBAL: CARMA2_HW 0x0079d860
tHeadup_text_buffer gHeadup_text_buffer_0079d860;

extern int C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2);
extern const char* gSelected_actor_name;
extern void C2_HOOK_CDECL MurderLastActor(void);

// GLOBAL: CARMA2_HW 0x0079d880
tHeadup_text_buffer gHeadup_text_buffer_0079d880;

extern const char* gSelected_actor_name;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time);
extern void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);
extern void C2_HOOK_FASTCALL CheckKeysForMouldiness(void);
extern void C2_HOOK_FASTCALL CheckOtherRacingKeys(void);
extern int C2_HOOK_FASTCALL CheckQuit(void);
extern int C2_HOOK_FASTCALL CheckRecoverCost(void);
extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tU32 C2_HOOK_FASTCALL GetRaceTime(void);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern int C2_HOOK_FASTCALL KeyIsDownNoMouldiness(int pKey_index);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap);
extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
extern int C2_HOOK_FASTCALL SpendCredits(int pAmount);
extern void C2_HOOK_FASTCALL UnloadDinputFFBEffectAtIndex(int index);
extern void __cdecl _ftol();
extern br_angle gCamera_yaw;
extern tCar_spec* gCar_to_view;
extern tU32 gCar_to_view_id;
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern int gINT_006a0a5c;
extern int gINT_006a0a6c;
extern int gInventory[20];
extern int gInventory_selected;
extern int gIt_or_fox;
extern tPlayer_ID gLocal_net_ID;
extern tU32 gMap_time;
extern int gMap_view;
extern tNet_mode gNet_mode;
extern int gNet_player_to_view_index;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_icons;
extern int gNumber_of_net_players;
extern int gOpponent_viewing_mode;
extern int gPed_676914;
extern int gPed_count;
extern int gPed_nearness;
extern tPedestrian* gPedestrian_array;
extern tHeadup_icon gPickedup_powerups[20];
extern tPowerup* gPowerup_array;
extern tProgram_state gProgram_state;
extern tU32 gRace_finished;
extern int gRecover_car;
extern tFloat_bunch_info gRecovery_cost;
extern int gRender_indent;
extern int gSelected_ped;
extern int gThis_net_player_index;
extern tSlot_info gTrade_in_value_APO;
extern int gTyping;
extern tU32 gWait_for_it;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime);
extern void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pText_buffer);
extern void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34Rotate();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_CDECL BrVector3SetFloat();
extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text);
extern void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);
extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern void C2_HOOK_FASTCALL DRstrlwr(char* s);
extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL DropActor(int pIndex, int pPowerup);
extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL FUN_0044a8a0(int pHeadup_index, int pX, int pY);
extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSorted);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);
extern void C2_HOOK_FASTCALL FlipUpCollisionInfo(tPhysics_object* pCollision_info);
extern int C2_HOOK_FASTCALL HasCurrentJoystick(void);
extern void C2_HOOK_FASTCALL JumpTheStart(void);
extern int C2_HOOK_FASTCALL PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data);
extern int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void);
extern void C2_HOOK_FASTCALL PDPlayFFBEffect(const char* effectName);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object *pCollision_info);
extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);
extern void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object *pCollision_info, tU32 pDisable);
extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec *pCar_spec, br_vector3 *pVec3);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern br_actor* gAcc_poly_actor;
extern int gCar_flying;
extern int gCountdown;
extern int gCredit_multiplier;
extern char gDOUBLE_0058ac18[];
extern char gDefault_drone_direction[];
extern int gDisable_sound;
extern char gFLOAT_0058ac0c[];
extern char gFLOAT_0058ac10[];
extern char gFLOAT_0058ac20[];
extern char gFloat_00589c40[];
extern tJoy_array gJoy_array;
extern int gKey_mapping[77];
extern br_actor* gNon_track_actor;
extern char gPowerup_respawn_specs[];
extern int gRace_head_ups[22];
extern char gRace_path[256];
extern int gRender_acc_poly_actor;
extern tRepulse_link gRepulse_links[6];
extern int gUNK_00596310;
extern tS3_outlet* gXXX_outlet;
extern br_vector3 y_unit_vector;
#endif
// FUNCTION: CARMA2_HW 0x00444700
void C2_HOOK_FASTCALL AddVouchers(int amount) {
    gRecovery_voucher_count += amount;
}

// GLOBAL: CARMA2_HW 0x0079ec54
int gAuto_repair;

// GLOBAL: CARMA2_HW 0x0079ec50
int gInstant_handbrake;

// GLOBAL: CARMA2_HW 0x0067c47c
tU32 gToo_poor_for_recovery_timeout;

// GLOBAL: CARMA2_HW 0x00659b28
int gCheckpoint_finder_enabled = 1;


// GLOBAL: CARMA2_HW 0x0067c460
int gHad_auto_recover;

// GLOBAL: CARMA2_HW 0x0067c478
tU32 gPalette_fade_time;

// GLOBAL: CARMA2_HW 0x0067c3bc
int gRecover_timer;

// GLOBAL: CARMA2_HW 0x0067c3b8
int gToo_late;

// GLOBAL: CARMA2_HW 0x0067c470
int gINT_0067c470;

// GLOBAL: CARMA2_HW 0x0067c468
int gWhich_edit_mode; /* FIXME: enum tEdit_mode */

// GLOBAL: CARMA2_HW 0x005904f0
tEdit_func* gEdit_funcs[2][18][8] = {
    {
        {
            NULL, /* [0][ 0][0] */
            NULL, /* [0][ 0][1] */
            NULL, /* [0][ 0][2] */
            NULL, /* [0][ 0][3] */
            NULL, /* [0][ 0][4] */
            NULL, /* [0][ 0][5] */
            NULL, /* [0][ 0][6] */
            NULL, /* [0][ 0][7] */
        },
        {
            NULL, /* [0][ 1][0] */
            NULL, /* [0][ 1][1] */
            NULL, /* [0][ 1][2] */
            NULL, /* [0][ 1][3] */
            NULL, /* [0][ 1][4] */
            NULL, /* [0][ 1][5] */
            NULL, /* [0][ 1][6] */
            NULL, /* [0][ 1][7] */
        },
        {
            NULL, /* [0][ 2][0] */
            NULL, /* [0][ 2][1] */
            NULL, /* [0][ 2][2] */
            NULL, /* [0][ 2][3] */
            NULL, /* [0][ 2][4] */
            NULL, /* [0][ 2][5] */
            NULL, /* [0][ 2][6] */
            NULL, /* [0][ 2][7] */
        },
        {
            NULL, /* [0][ 3][0] */
            NULL, /* [0][ 3][1] */
            NULL, /* [0][ 3][2] */
            NULL, /* [0][ 3][3] */
            NULL, /* [0][ 3][4] */
            NULL, /* [0][ 3][5] */
            NULL, /* [0][ 3][6] */
            NULL, /* [0][ 3][7] */
        },
        {
            NULL, /* [0][ 4][0] */
            NULL, /* [0][ 4][1] */
            NULL, /* [0][ 4][2] */
            NULL, /* [0][ 4][3] */
            NULL, /* [0][ 4][4] */
            NULL, /* [0][ 4][5] */
            NULL, /* [0][ 4][6] */
            NULL, /* [0][ 4][7] */
        },
        {
            NULL, /* [0][ 5][0] */
            NULL, /* [0][ 5][1] */
            NULL, /* [0][ 5][2] */
            NULL, /* [0][ 5][3] */
            NULL, /* [0][ 5][4] */
            NULL, /* [0][ 5][5] */
            NULL, /* [0][ 5][6] */
            NULL, /* [0][ 5][7] */
        },
        {
            NULL, /* [0][ 6][0] */
            NULL, /* [0][ 6][1] */
            NULL, /* [0][ 6][2] */
            NULL, /* [0][ 6][3] */
            NULL, /* [0][ 6][4] */
            NULL, /* [0][ 6][5] */
            NULL, /* [0][ 6][6] */
            NULL, /* [0][ 6][7] */
        },
        {
            NULL, /* [0][ 7][0] */
            NULL, /* [0][ 7][1] */
            NULL, /* [0][ 7][2] */
            NULL, /* [0][ 7][3] */
            NULL, /* [0][ 7][4] */
            NULL, /* [0][ 7][5] */
            NULL, /* [0][ 7][6] */
            NULL, /* [0][ 7][7] */
        },
        {
            CycleCarSimplificationLevel, /* [0][ 8][0] */
            NULL, /* [0][ 8][1] */
            NULL, /* [0][ 8][2] */
            NULL, /* [0][ 8][3] */
            NULL, /* [0][ 8][4] */
            NULL, /* [0][ 8][5] */
            NULL, /* [0][ 8][6] */
            NULL, /* [0][ 8][7] */
        },
        {
            ToggleShadow, /* [0][ 9][0] */
            NULL, /* [0][ 9][1] */
            NULL, /* [0][ 9][2] */
            NULL, /* [0][ 9][3] */
            NULL, /* [0][ 9][4] */
            NULL, /* [0][ 9][5] */
            NULL, /* [0][ 9][6] */
            NULL, /* [0][ 9][7] */
        },
        {
            NULL, /* [0][10][0] */
            NULL, /* [0][10][1] */
            NULL, /* [0][10][2] */
            NULL, /* [0][10][3] */
            NULL, /* [0][10][4] */
            NULL, /* [0][10][5] */
            NULL, /* [0][10][6] */
            NULL, /* [0][10][7] */
        },
        {
            ToggleSky, /* [0][11][0] */
            NULL, /* [0][11][1] */
            NULL, /* [0][11][2] */
            NULL, /* [0][11][3] */
            ToggleDepthCueing, /* [0][11][4] */
            NULL, /* [0][11][5] */
            NULL, /* [0][11][6] */
            NULL, /* [0][11][7] */
        },
        {
            CycleYonFactor, /* [0][12][0] */
            NULL, /* [0][12][1] */
            NULL, /* [0][12][2] */
            NULL, /* [0][12][3] */
            ToggleAccessoryRendering, /* [0][12][4] */
            NULL, /* [0][12][5] */
            NULL, /* [0][12][6] */
            NULL, /* [0][12][7] */
        },
        {
            DecreaseYon, /* [0][13][0] */
            NULL, /* [0][13][1] */
            NULL, /* [0][13][2] */
            NULL, /* [0][13][3] */
            IncreaseYon, /* [0][13][4] */
            NULL, /* [0][13][5] */
            NULL, /* [0][13][6] */
            NULL, /* [0][13][7] */
        },
        {
            CycleSoundDetailLevel, /* [0][14][0] */
            NULL, /* [0][14][1] */
            NULL, /* [0][14][2] */
            NULL, /* [0][14][3] */
            NULL, /* [0][14][4] */
            NULL, /* [0][14][5] */
            NULL, /* [0][14][6] */
            NULL, /* [0][14][7] */
        },
        {
            ShowCurrentJoystickName, /* [0][15][0] */
            ToggleJoystickHeadup, /* [0][15][1] */
            ToggleJoystickYAxis, /* [0][15][2] */
            ToggleJoystickDPad, /* [0][15][3] */
            CycleJoystick, /* [0][15][4] */
            ToggleJoystickEnable, /* [0][15][5] */
            IncreaseJoystickFFB, /* [0][15][6] */
            NULL, /* [0][15][7] */
        },
        {
            NULL, /* [0][16][0] */
            NULL, /* [0][16][1] */
            NULL, /* [0][16][2] */
            NULL, /* [0][16][3] */
            NULL, /* [0][16][4] */
            NULL, /* [0][16][5] */
            NULL, /* [0][16][6] */
            NULL, /* [0][16][7] */
        },
        {
            NULL, /* [0][17][0] */
            NULL, /* [0][17][1] */
            NULL, /* [0][17][2] */
            NULL, /* [0][17][3] */
            NULL, /* [0][17][4] */
            NULL, /* [0][17][5] */
            NULL, /* [0][17][6] */
            NULL, /* [0][17][7] */
        },
    },
    {
        {
            TotalRepair, /* [1][ 0][0] */
            MaxOutAPO, /* [1][ 0][1] */
            NULL, /* [1][ 0][2] */
            NULL, /* [1][ 0][3] */
            FUN_00447330, /* [1][ 0][4] */
            FUN_00447340, /* [1][ 0][5] */
            NULL, /* [1][ 0][6] */
            NULL, /* [1][ 0][7] */
        },
        {
            CycleInvulnerability, /* [1][ 1][0] */
            NULL, /* [1][ 1][1] */
            NULL, /* [1][ 1][2] */
            NULL, /* [1][ 1][3] */
            ResetMan, /* [1][ 1][4] */
            NULL, /* [1][ 1][5] */
            NULL, /* [1][ 1][6] */
            NULL, /* [1][ 1][7] */
        },
        {
            NextPedCam, /* [1][ 2][0] */
            FindNearestPed, /* [1][ 2][1] */
            NULL, /* [1][ 2][2] */
            NULL, /* [1][ 2][3] */
            PrevPedCam, /* [1][ 2][4] */
            FindNearestPed, /* [1][ 2][5] */
            NULL, /* [1][ 2][6] */
            NULL, /* [1][ 2][7] */
        },
        {
            ToggleTimerFreeze, /* [1][ 3][0] */
            ShadowMode, /* [1][ 3][1] */
            NULL, /* [1][ 3][2] */
            NULL, /* [1][ 3][3] */
            ToggleShadow, /* [1][ 3][4] */
            NULL, /* [1][ 3][5] */
            NULL, /* [1][ 3][6] */
            NULL, /* [1][ 3][7] */
        },
        {
            IncrementLap, /* [1][ 4][0] */
            NULL, /* [1][ 4][1] */
            NULL, /* [1][ 4][2] */
            NULL, /* [1][ 4][3] */
            IncrementCheckpoint, /* [1][ 4][4] */
            NULL, /* [1][ 4][5] */
            NULL, /* [1][ 4][6] */
            NULL, /* [1][ 4][7] */
        },
        {
            EarnDosh, /* [1][ 5][0] */
            NULL, /* [1][ 5][1] */
            NULL, /* [1][ 5][2] */
            NULL, /* [1][ 5][3] */
            LoseDosh, /* [1][ 5][4] */
            NULL, /* [1][ 5][5] */
            NULL, /* [1][ 5][6] */
            NULL, /* [1][ 5][7] */
        },
        {
            ViewOpponent, /* [1][ 6][0] */
            ViewOpponent, /* [1][ 6][1] */
            ViewOpponent, /* [1][ 6][2] */
            ViewOpponent, /* [1][ 6][3] */
            ViewOpponent, /* [1][ 6][4] */
            ViewOpponent, /* [1][ 6][5] */
            ViewOpponent, /* [1][ 6][6] */
            ViewOpponent, /* [1][ 6][7] */
        },
        {
            GotPowerup0, /* [1][ 7][0] */
            GotPowerup0, /* [1][ 7][1] */
            GotPowerup0, /* [1][ 7][2] */
            GotPowerup0, /* [1][ 7][3] */
            GotPowerup0, /* [1][ 7][4] */
            GotPowerup0, /* [1][ 7][5] */
            GotPowerup0, /* [1][ 7][6] */
            GotPowerup0, /* [1][ 7][7] */
        },
        {
            GotPowerup1, /* [1][ 8][0] */
            GotPowerup1, /* [1][ 8][1] */
            GotPowerup1, /* [1][ 8][2] */
            GotPowerup1, /* [1][ 8][3] */
            GotPowerup1, /* [1][ 8][4] */
            GotPowerup1, /* [1][ 8][5] */
            GotPowerup1, /* [1][ 8][6] */
            GotPowerup1, /* [1][ 8][7] */
        },
        {
            GotPowerup2, /* [1][ 9][0] */
            GotPowerup2, /* [1][ 9][1] */
            GotPowerup2, /* [1][ 9][2] */
            GotPowerup2, /* [1][ 9][3] */
            GotPowerup2, /* [1][ 9][4] */
            GotPowerup2, /* [1][ 9][5] */
            GotPowerup2, /* [1][ 9][6] */
            GotPowerup2, /* [1][ 9][7] */
        },
        {
            GotPowerup3, /* [1][10][0] */
            GotPowerup3, /* [1][10][1] */
            GotPowerup3, /* [1][10][2] */
            GotPowerup3, /* [1][10][3] */
            GotPowerup3, /* [1][10][4] */
            GotPowerup3, /* [1][10][5] */
            GotPowerup3, /* [1][10][6] */
            GotPowerup3, /* [1][10][7] */
        },
        {
            GotPowerup4, /* [1][11][0] */
            GotPowerup4, /* [1][11][1] */
            GotPowerup4, /* [1][11][2] */
            GotPowerup4, /* [1][11][3] */
            GotPowerup4, /* [1][11][4] */
            GotPowerup4, /* [1][11][5] */
            GotPowerup4, /* [1][11][6] */
            GotPowerup4, /* [1][11][7] */
        },
        {
            GotPowerup5, /* [1][12][0] */
            GotPowerup5, /* [1][12][1] */
            GotPowerup5, /* [1][12][2] */
            GotPowerup5, /* [1][12][3] */
            GotPowerup5, /* [1][12][4] */
            GotPowerup5, /* [1][12][5] */
            GotPowerup5, /* [1][12][6] */
            GotPowerup5, /* [1][12][7] */
        },
        {
            GotPowerup6, /* [1][13][0] */
            GotPowerup6, /* [1][13][1] */
            GotPowerup6, /* [1][13][2] */
            GotPowerup6, /* [1][13][3] */
            GotPowerup6, /* [1][13][4] */
            GotPowerup6, /* [1][13][5] */
            GotPowerup6, /* [1][13][6] */
            GotPowerup6, /* [1][13][7] */
        },
        {
            GotPowerup7, /* [1][14][0] */
            GotPowerup7, /* [1][14][1] */
            GotPowerup7, /* [1][14][2] */
            GotPowerup7, /* [1][14][3] */
            GotPowerup7, /* [1][14][4] */
            GotPowerup7, /* [1][14][5] */
            GotPowerup7, /* [1][14][6] */
            GotPowerup7, /* [1][14][7] */
        },
        {
            GotPowerup8, /* [1][15][0] */
            GotPowerup8, /* [1][15][1] */
            GotPowerup8, /* [1][15][2] */
            GotPowerup8, /* [1][15][3] */
            GotPowerup8, /* [1][15][4] */
            GotPowerup8, /* [1][15][5] */
            GotPowerup8, /* [1][15][6] */
            GotPowerup8, /* [1][15][7] */
        },
        {
            GotPowerup9, /* [1][16][0] */
            GotPowerup9, /* [1][16][1] */
            GotPowerup9, /* [1][16][2] */
            GotPowerup9, /* [1][16][3] */
            GotPowerup9, /* [1][16][4] */
            GotPowerup9, /* [1][16][5] */
            GotPowerup9, /* [1][16][6] */
            GotPowerup9, /* [1][16][7] */
        },
        {
            NULL, /* [1][17][0] */
            NULL, /* [1][17][1] */
            NULL, /* [1][17][2] */
            NULL, /* [1][17][3] */
            NULL, /* [1][17][4] */
            NULL, /* [1][17][5] */
            NULL, /* [1][17][6] */
            NULL, /* [1][17][7] */
        },
    },
};

// GLOBAL: CARMA2_HW 0x0067c46c
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x005904c0
const char * gEdit_mode_names[12] = {
    "Options",
    "Cheat",
    "Accessories",
    "Powerups",
    "Special Volumes",
    "Pedestrians",
    "Drones",
    "Opponents",
    "Prat-cam",
    "Depth effects",
    "Damage",
    "Crush Test",
};

// GLOBAL: CARMA2_HW 0x0068d8c4
tCar_spec* gTarget_lock_car_1;

// GLOBAL: CARMA2_HW 0x0068d6f0
tCar_spec* gTarget_lock_car_2;

// GLOBAL: CARMA2_HW 0x006a0940
int gInventory_cycling;

// GLOBAL: CARMA2_HW 0x006a0954
tU32 gInventory_timeout;

// GLOBAL: CARMA2_HW 0x0067c498
int gInvulnerability;

// GLOBAL: CARMA2_HW 0x00590f60
int gCancel_recover = 1;

// GLOBAL: CARMA2_HW 0x0067c45c
int gRepair_last_time;

// GLOBAL: CARMA2_HW 0x0067c464
tU32 gLast_repair_time;


void (C2_HOOK_FASTCALL * RichardsSphere_original)(int pNum);
// GLOBAL: CARMA2_HW 0x0068b8e4
int gINT_0068b8e4;

// GLOBAL: CARMA2_HW 0x0068b8e8
int gINT_0068b8e8;

// GLOBAL: CARMA2_HW 0x00659b2c
float gMap_render_x = 80.f;

// GLOBAL: CARMA2_HW 0x00659b30
float gMap_render_y = 6.f;

// GLOBAL: CARMA2_HW 0x00659b34
float gMap_render_width = 128.f;

// GLOBAL: CARMA2_HW 0x00659b38
float gMap_render_height = 80.f;

// GLOBAL: CARMA2_HW 0x0068d8d4
int gMap_trans;

// GLOBAL: CARMA2_HW 0x0074abac
int gHeadup_map_x;

// GLOBAL: CARMA2_HW 0x0074abb0
int gHeadup_map_y;

// GLOBAL: CARMA2_HW 0x0074aba8
int gHeadup_map_w;

// GLOBAL: CARMA2_HW 0x0074abb4
int gHeadup_map_h;

// GLOBAL: CARMA2_HW 0x0067c400
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c3c8
char* gAbuse_text[10];

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00456960
void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel) {
    if (gSound_enabled) {
        tS3_outlet* pOutlet;
        pOutlet = gXXX_outlet;
        if (gSound_enabled) {
            S3StopOutletSound(pOutlet);
        }
        pOutlet = gEngine_outlet;
        if (gSound_enabled) {
            S3StopOutletSound(pOutlet);
        }
        pOutlet = gCar_outlet;
        if (gSound_enabled) {
            S3StopOutletSound(pOutlet);
        }
        pOutlet = gEffects_outlet;
        if (gSound_enabled) {
            S3StopOutletSound(pOutlet);
        }
        pOutlet = gPedestrians_outlet;
        if (gSound_enabled) {
            S3StopOutletSound(pOutlet);
        }
    }
    DisposeSoundSources();
    gSound_detail_level = pLevel;
    InitSound();
    InitSoundSources();
}
#pragma auto_inline(on)

// Key: 'm'
// FUNCTION: CARMA2_HW 0x004420e0
void C2_HOOK_FASTCALL ToggleMiniMap(void) {
    if (gMap_view != 2) {
        gMini_map_visible = !gMini_map_visible;
        if (!gMini_map_visible) {
            NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_off));
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_on));
        }
        UpdateMapAndSaveOptions();
    }
}

// Key: 'Enter'
// FUNCTION: CARMA2_HW 0x0042dd50
void C2_HOOK_FASTCALL ToggleFlaps(void) {
    if (gCountdown || gToo_poor_for_recovery_timeout != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        PhysicsObjectRecurse(gProgram_state.current_car.collision_info,
            ToggleFlapsCB,
            &gProgram_state.current_car);
    } else {
        tNet_message_chunk* chunk = NetGetToHostContents(eNet_message_chunk_type_toggle_doors, 0);
        tNet_game_player_info *player_info = NetPlayerFromCar(&gProgram_state.current_car);
        if (player_info != NULL) {
            chunk->toggle_doors.id = player_info->ID;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042ddb0
int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data) {
    tCar_spec *pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleDoorsActorCallback, pCar_spec);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ddc0
intptr_t C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data) {
    tCar_spec* pCar = data;
    tUser_crush_data* user_data;
    tCar_crush_buffer_entry* crush_buffer;
    int i;

    user_data = pActor->user;
    if (user_data == NULL) {
        return 0;
    }
    crush_buffer = user_data->crush_data;
    if (crush_buffer == NULL) {
        return 0;
    }
    if (crush_buffer->flap_data == NULL || !crush_buffer->flap_data->kev_o_flap) {
        return 0;
    }

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gToggled_doors) != 16);

    if (crush_buffer->flap_data->field_0x0) {
        if (gCount_toggled_doors != CARPOCALYPSE2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 0;
            gToggled_doors[gCount_toggled_doors].field_0xc = 0.f;
            gCount_toggled_doors += 1;
        }
    } else {
        if (gCount_toggled_doors != CARPOCALYPSE2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 1;
            gToggled_doors[gCount_toggled_doors].field_0xc = 20.f;
            gCount_toggled_doors += 1;
        }
    }
    return 0;
}

// GLOBAL: CARMA2_HW 0x0067c458
static int old_indent;

void C2_HOOK_FASTCALL ToggleMap2(void) {
    int original_indent;

    original_indent = gRender_indent;
    if (gMap_view == 2) {
        gMap_view = 1;
        gRender_indent = old_indent;
    } else {
        if (gAction_replay_mode) {
            return;
        }
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
            return;
        }
        gRender_indent = 0;
        old_indent = original_indent;
        gMap_view = 2;
        gMap_time = PDGetTotalTime();
    }
    if (gCurrent_race.map_image != NULL) {
        // nop_FUN005191f0(gCurrent_race.map_image);
    }
}

// Key: 'tab'
// FUNCTION: CARMA2_HW 0x00444610
void C2_NAKED C2_HOOK_FASTCALL ToggleMap(void) {
#ifndef CARPOCALYPSE2_MATCHING

    ToggleMap2();
    AdjustRenderScreenSize();
#else
    __asm {
            mov ecx, dword ptr [gMap_view]
            mov eax, 2
            cmp ecx, eax
            jne L00444643
            mov ecx, dword ptr [gCurrent_race+0xf04]
            mov eax, dword ptr [old_indent]
            test ecx, ecx
            mov dword ptr [gMap_view], 1
            mov dword ptr [gRender_indent], eax
            je L004446c7
            jmp L004446c2
    L00444643:
            mov ecx, dword ptr [gPed_676914]
            test ecx, ecx
            jne L004446c7
            mov ecx, dword ptr [gNet_mode]
            test ecx, ecx
            je L00444693
            mov ecx, dword ptr [gCurrent_net_game]
            cmp dword ptr [ecx + 0x74], 7
            jne L00444693
            mov edx, dword ptr [gThis_net_player_index]
            mov ecx, dword ptr [gIt_or_fox]
            cmp edx, ecx
            jne L00444693
            mov ecx, 0xd6
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x3e8
            call NewTextHeadupSlot
            jmp L004446c7
    L00444693:
            mov ecx, dword ptr [gRender_indent]
            mov dword ptr [gRender_indent], 0
            mov dword ptr [old_indent], ecx
            mov dword ptr [gMap_view], eax
            call PDGetTotalTime
            mov ecx, dword ptr [gCurrent_race+0xf04]
            mov dword ptr [gMap_time], eax
            test ecx, ecx
            je L004446c7
    L004446c2:
            call PixelmapSwapByteOrder
    L004446c7:
            jmp AdjustRenderScreenSize
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00444600
void C2_HOOK_FASTCALL ToggleHeadupMap(void) {
}

// Key: 'ctrl+tab'
// FUNCTION: CARMA2_HW 0x00494840
void C2_NAKED C2_HOOK_FASTCALL CycleMapOptions(void) {
#ifndef CARPOCALYPSE2_MATCHING

    gCheckpoint_finder_enabled = !gCheckpoint_finder_enabled;
    NewTextHeadupSlot2(4, 0, 1500, -4, gCheckpoint_finder_enabled ? GetMiscString(eMiscString_checkpoint_finder_turned_on) : GetMiscString(eMiscString_checkpoint_finder_turned_off), 0);
#else
    __asm {
            mov edx, dword ptr [gCheckpoint_finder_enabled]
            xor eax, eax
            test edx, edx
            sete al
            test eax, eax
            mov dword ptr [gCheckpoint_finder_enabled], eax
            push 0
            mov ecx, 0xff
            jne L00494862
            mov ecx, 0xfe
    L00494862:
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x5dc
            call NewTextHeadupSlot2
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// key: 'alt+tab' ('alt' in map mode)
// FUNCTION: CARMA2_HW 0x00494880
void C2_HOOK_FASTCALL ToggleMapTransparency(void) {
    gMap_trans = !gMap_trans;
    SaveOptions();
}

// Key: 'r'
// FUNCTION: CARMA2_HW 0x00442300
void C2_NAKED C2_HOOK_FASTCALL SetRecovery(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gINT_0068b8e4
        || gINT_0068b8e8
        || gRace_finished
        || gProgram_state.current_car.knackered
        || gWait_for_it
        || gHad_auto_recover
        || gPalette_fade_time != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        gRecover_car = 1;
        gRecover_timer = 0;
    } else if (gProgram_state.current_car.time_to_recover == 0) {
        gCancel_recover = 1;
        if (CheckRecoverCost()) {
            if (gCurrent_net_game->type == eNet_game_type_foxy) {
                if (gThis_net_player_index == gIt_or_fox) {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 5000;
                } else {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 1000;
                }
            } else {
                if (gCurrent_net_game->type == 6 &&
                    gNet_players[gThis_net_player_index].field_0x80 != 0) {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
                } else {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 3000;
                }
            }
        }
    }
    else if (gCancel_recover) {
        if (GetRaceTime() + 600 < gProgram_state.current_car.time_to_recover) {
            gProgram_state.current_car.time_to_recover = 0;
            if (!gINT_0067c470) {
                float cost = (gNet_mode == eNet_mode_none) ? gRecovery_cost.initial[gProgram_state.skill_level] : gRecovery_cost.initial_network[gCurrent_net_game->type];
                EarnCredits((int)cost);
            }
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_recovery_cancelled), 0);
        } else {
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_too_late_to_cancel), 1);
            gToo_late = 1;
        }
    }
#else
    __asm {
            mov eax, dword ptr [gINT_0068b8e4]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gINT_0068b8e8]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gRace_finished]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gProgram_state+0x280]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gWait_for_it]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gHad_auto_recover]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gPalette_fade_time]
            test eax, eax
            jne L004424f6
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004424e2
            mov eax, dword ptr [gProgram_state+0x194c]
            test eax, eax
            je L00442428
            mov eax, dword ptr [gCancel_recover]
            test eax, eax
            je L004424f6
            call GetRaceTime
            mov ecx, dword ptr [gProgram_state+0x194c]
            add eax, 0x258
            cmp ecx, eax
            jbe L004423fd
            mov eax, dword ptr [gINT_0067c470]
            mov dword ptr [gProgram_state+0x194c], 0
            test eax, eax
            jne L004423dc
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004423c3
            mov eax, dword ptr [gCurrent_net_game]
            mov ecx, dword ptr [eax + 0x74]
            fld dword ptr [ecx*4 + gRecovery_cost+0xc]
            jmp L004423d0
    L004423c3:
            mov edx, dword ptr [gProgram_state+0x20]
            fld dword ptr [edx*4 + gRecovery_cost]
    L004423d0:
            call _ftol
            mov ecx, eax
            call EarnCredits
    L004423dc:
            push 0
            mov ecx, 0x7d
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x7d0
            call NewTextHeadupSlot2
            ret
    L004423fd:
            push 1
            mov ecx, 0xf2
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x7d0
            call NewTextHeadupSlot2
            mov dword ptr [gToo_late], 1
            ret
    L00442428:
            mov dword ptr [gCancel_recover], 1
            call CheckRecoverCost
            test eax, eax
            je L004424f6
            mov eax, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [eax + 0x74]
            cmp eax, 7
            jne L0044248f
            mov ecx, dword ptr [gThis_net_player_index]
            mov eax, dword ptr [gIt_or_fox]
            cmp ecx, eax
            jne L00442475
            call GetRaceTime
            add eax, 0x1388
            mov dword ptr [gToo_late], 0
            mov dword ptr [gProgram_state+0x194c], eax
            ret
    L00442475:
            call GetRaceTime
            add eax, 0x3e8
            mov dword ptr [gToo_late], 0
            mov dword ptr [gProgram_state+0x194c], eax
            ret
    L0044248f:
            cmp eax, 6
            jne L004424c8
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [edx*4 + gNet_players+0x80]
            test eax, eax
            je L004424c8
            call GetRaceTime
            add eax, 0x1f4
            mov dword ptr [gToo_late], 0
            mov dword ptr [gProgram_state+0x194c], eax
            ret
    L004424c8:
            call GetRaceTime
            add eax, 0xbb8
            mov dword ptr [gToo_late], 0
            mov dword ptr [gProgram_state+0x194c], eax
            ret
    L004424e2:
            mov dword ptr [gRecover_car], 1
            mov dword ptr [gRecover_timer], 0
    L004424f6:
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

// FUNCTION: CARMA2_HW 0x00442750
int C2_NAKED C2_HOOK_FASTCALL CheckRecoverCost(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int recovery_cost;

    if (gNet_mode == eNet_mode_none) {
        return 1;
    }

    if (gProgram_state.current_car.knackered
        || gRecovery_voucher_count != 0
        || (NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80 != 0)) {

        gINT_0067c470 = 1;
        return 1;
    }

    gINT_0067c470 = 0;
    recovery_cost = (int)(gNet_mode == eNet_mode_none ? gRecovery_cost.initial[gProgram_state.skill_level] : gRecovery_cost.initial[gCurrent_net_game->type]);

    if (recovery_cost > gProgram_state.credits) {
        int remaining_cost = recovery_cost;
        while (remaining_cost > 0
                && gProgram_state.current_car.power_up_levels[0] != 0
                && gProgram_state.current_car.power_up_levels[1] != 0
                && gProgram_state.current_car.power_up_levels[2] != 0) {
            int which;

            which = IRandomBetween(0, 2);
            if (gProgram_state.current_car.power_up_levels[which] != 0) {
                int part_value;

                gProgram_state.current_car.power_up_levels[which] -= 1;
                part_value = (gNet_mode == eNet_mode_none ? gTrade_in_value_APO.initial[gProgram_state.skill_level] : gTrade_in_value_APO.initial[gCurrent_net_game->type]);
                remaining_cost -= part_value;
                gProgram_state.credits += part_value;
            }
        }
    }
    if (recovery_cost > gProgram_state.credits) {
        gToo_poor_for_recovery_timeout = PDGetTotalTime() + 2000;
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_press_return_to_commit_suicide));
        return 0;
    }
    SpendCredits(recovery_cost);
    return 1;
#else
    __asm {
            mov eax, dword ptr [gNet_mode]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            je L00442962
            mov eax, dword ptr [gProgram_state+0x280]
            test eax, eax
            jne L00442958
            mov eax, dword ptr [gRecovery_voucher_count]
            test eax, eax
            jne L00442958
            mov ecx, offset gProgram_state+0xac
            call NetPlayerFromCar
            test eax, eax
            je L004427a1
            mov ecx, offset gProgram_state+0xac
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            jne L00442958
    L004427a1:
            mov esi, dword ptr [gNet_mode]
            mov edi, dword ptr [gProgram_state+0x20]
            mov ebp, dword ptr [gCurrent_net_game]
            mov dword ptr [gINT_0067c470], 0
            test esi, esi
            je L004427cd
            mov eax, dword ptr [ebp + 0x74]
            fld dword ptr [eax*4 + gRecovery_cost+0xc]
            jmp L004427d4
    L004427cd:
            fld dword ptr [edi*4 + gRecovery_cost]
    L004427d4:
            fild dword ptr [gProgram_state]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004428bb
            test esi, esi
            je L004427f9
            mov ecx, dword ptr [ebp + 0x74]
            fld dword ptr [ecx*4 + gRecovery_cost+0xc]
            jmp L00442800
    L004427f9:
            fld dword ptr [edi*4 + gRecovery_cost]
    L00442800:
            call _ftol
            mov ebx, eax
            test ebx, ebx
            jle L004428bb
    L0044280f:
            mov eax, dword ptr [gProgram_state+0x1954]
            test eax, eax
            jne L0044282e
            mov eax, dword ptr [gProgram_state+0x1958]
            test eax, eax
            jne L0044282e
            mov eax, dword ptr [gProgram_state+0x195c]
            test eax, eax
            je L004428bb
    L0044282e:
            mov edx, 2
            xor ecx, ecx
            call IRandomBetween
            mov ecx, dword ptr [eax*4 + gProgram_state+0x1954]
            mov esi, dword ptr [gNet_mode]
            mov ebp, dword ptr [gCurrent_net_game]
            test ecx, ecx
            je L004428ad
            dec ecx
            mov dword ptr [eax*4 + gProgram_state+0x1954], ecx
            mov edi, dword ptr [gProgram_state+0x20]
            test esi, esi
            je L0044286f
            mov edx, dword ptr [ebp + 0x74]
            mov eax, dword ptr [edx*4 + gTrade_in_value_APO+0xc]
            jmp L00442876
    L0044286f:
            mov eax, dword ptr [edi*4 + gTrade_in_value_APO]
    L00442876:
            sub ebx, eax
            test esi, esi
            je L00442896
            mov eax, dword ptr [ebp + 0x74]
            mov ecx, dword ptr [gProgram_state]
            mov eax, dword ptr [eax*4 + gTrade_in_value_APO+0xc]
            add ecx, eax
            mov dword ptr [gProgram_state], ecx
            jmp L004428b3
    L00442896:
            mov eax, dword ptr [edi*4 + gTrade_in_value_APO]
            mov ecx, dword ptr [gProgram_state]
            add ecx, eax
            mov dword ptr [gProgram_state], ecx
            jmp L004428b3
    L004428ad:
            mov edi, dword ptr [gProgram_state+0x20]
    L004428b3:
            test ebx, ebx
            jg L0044280f
    L004428bb:
            test esi, esi
            je L004428cb
            mov ecx, dword ptr [ebp + 0x74]
            fld dword ptr [ecx*4 + gRecovery_cost+0xc]
            jmp L004428d2
    L004428cb:
            fld dword ptr [edi*4 + gRecovery_cost]
    L004428d2:
            fild dword ptr [gProgram_state]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00442917
            call PDGetTotalTime
            add eax, 0x7d0
            mov ecx, 0x11e
            mov dword ptr [gToo_poor_for_recovery_timeout], eax
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x3e8
            call NewTextHeadupSlot
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00442917:
            test esi, esi
            je L0044293b
            mov edx, dword ptr [ebp + 0x74]
            fld dword ptr [edx*4 + gRecovery_cost+0xc]
            call _ftol
            mov ecx, eax
            call SpendCredits
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0044293b:
            fld dword ptr [edi*4 + gRecovery_cost]
            call _ftol
            mov ecx, eax
            call SpendCredits
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00442958:
            mov dword ptr [gINT_0067c470], 1
    L00442962:
            pop edi
            pop esi
            pop ebp
            mov eax, 1
            pop ebx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// Key: 'ctrl+a'
// FUNCTION: CARMA2_HW 0x00441490
void C2_HOOK_FASTCALL AbortRace(void) {
    if (!gRace_finished) {
        gAbandon_game = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00444f40
void C2_HOOK_FASTCALL ToggleHeadupLevel(void) {
    if (PDKeyDown(0)) {
        gHeadup_detail_level -= 1;
        if (gHeadup_detail_level < 0) {
            gHeadup_detail_level = kMax_headup_detail_level;
        }
    } else {
        gHeadup_detail_level += 1;
        if (gHeadup_detail_level > kMax_headup_detail_level) {
            gHeadup_detail_level = 0;
        }
    }
    NewTextHeadupSlot2(4, 0, 250, -4, GetMiscString(eMiscString_no_HUD + (gHeadup_detail_level % 3)), 0);
}

// FUNCTION: CARMA2_HW 0x00441580
void C2_HOOK_FASTCALL SetFlag(int i) {

    if (gI_am_cheating == i) {
        gI_am_cheating = 0;
        NewTextHeadupSlot(4, 0, 3000, -4, "Cheating Off");
    } else {
        gI_am_cheating = i;
    }
    F4Key();
}

// FUNCTION: CARMA2_HW 0x00444350
void C2_HOOK_FASTCALL SetFlag2(int pArg) {
    (void)pArg;
    gAllow_car_flying = 1;
    ToggleFlying();
    gAllow_car_flying = gCar_flying;
}

// Key: 'f4'
#pragma auto_inline( off )
// FUNCTION: CARMA2_HW 0x004414b0
void C2_HOOK_FASTCALL F4Key(void) {

    if (gI_am_cheating == 0xa11ee75d || (gI_am_cheating != 0x564e78b9 && gNet_mode != eNet_mode_none)) {
        char s[256];

        if (gINT_0068b8e4 == 1) {
            FUN_0045a670();
        }
        if (gINT_0068b8e4 == 2) {
            FUN_0045acf0();
        }
        if (gINT_0068b8e8) {
            ToggleJoystickHeadup();
        }
        if (PDKeyDown(0)) {
            gWhich_edit_mode -= 1;
            if (gWhich_edit_mode < 0) {
                gWhich_edit_mode = CARPOCALYPSE2_ASIZE(gEdit_funcs) - 1;
            }
        } else {
            gWhich_edit_mode += 1;
            if (gWhich_edit_mode >= CARPOCALYPSE2_ASIZE(gEdit_funcs)) {
                gWhich_edit_mode = 0;
            }
        }
        sprintf(s, "Edit mode: %s", gEdit_mode_names[gWhich_edit_mode]);
        NewTextHeadupSlot2(4, 0, 2000, -4, s, 0);
    } else {
        gWhich_edit_mode = 0;
    }
}
#pragma auto_inline( on )

// FUNCTION: CARMA2_HW 0x004415e0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {
}

// FUNCTION: CARMA2_HW 0x004415f0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {
}

void C2_HOOK_FASTCALL DoEditModeKey(int pIndex) {
    int modifiers;

    if (gI_am_cheating == 0xa11ee75d || (gI_am_cheating == 0x564e78b9 && gNet_mode == eNet_mode_none)) {
        modifiers = 0;
        modifiers |= PDKeyDown(0) ? 4 : 0;
        modifiers |= PDKeyDown(1) ? 2 : 0;
        modifiers |= PDKeyDown(2) ? 1 : 0;
        if (gEdit_funcs[gWhich_edit_mode][pIndex][modifiers] != NULL) {
            gEdit_funcs[gWhich_edit_mode][pIndex][modifiers]();
        }
    } else {
        gWhich_edit_mode = 0;
    }
}

// Key: 'f5'
// FUNCTION: CARMA2_HW 0x00441600
void C2_HOOK_FASTCALL F5Key(void) {
    DoEditModeKey(0);
}

// Key: 'f6'
// FUNCTION: CARMA2_HW 0x00441680
void C2_HOOK_FASTCALL F6Key(void) {
    DoEditModeKey(1);
}

// Key: 'f7'
// FUNCTION: CARMA2_HW 0x00441700
void C2_HOOK_FASTCALL F7Key(void) {
    DoEditModeKey(2);
}

// Key: 'f8'
// FUNCTION: CARMA2_HW 0x00441780
void C2_HOOK_FASTCALL F8Key(void) {
    DoEditModeKey(3);
}

// Key: 'f10'
// FUNCTION: CARMA2_HW 0x00441800
void C2_HOOK_FASTCALL F10Key(void) {
    DoEditModeKey(4);
}

// Key: 'f11'
// FUNCTION: CARMA2_HW 0x00441880
void C2_HOOK_FASTCALL F11Key(void) {
    DoEditModeKey(5);
}

// Key: 'f12'
// FUNCTION: CARMA2_HW 0x00441900
void C2_HOOK_FASTCALL F12Key(void) {
    /* Do nothing */
}

// Key: '0'
// FUNCTION: CARMA2_HW 0x00441910
void C2_HOOK_FASTCALL NumberKey0(void) {
    DoEditModeKey(7);
}

// Key: '1'
// FUNCTION: CARMA2_HW 0x00441990
void C2_HOOK_FASTCALL NumberKey1(void) {
    DoEditModeKey(8);
}

// Key: '2'
// FUNCTION: CARMA2_HW 0x00441a10
void C2_HOOK_FASTCALL NumberKey2(void) {
    DoEditModeKey(9);
}

// Key: '3'
// FUNCTION: CARMA2_HW 0x00441a90
void C2_HOOK_FASTCALL NumberKey3(void) {
    DoEditModeKey(10);
}

// Key: '4'
// FUNCTION: CARMA2_HW 0x00441b10
void C2_HOOK_FASTCALL NumberKey4(void) {
    DoEditModeKey(11);
}

// Key: '5'
// FUNCTION: CARMA2_HW 0x00441b90
void C2_HOOK_FASTCALL NumberKey5(void) {
    DoEditModeKey(12);
}

// Key: '6'
// FUNCTION: CARMA2_HW 0x00441c10
void C2_HOOK_FASTCALL NumberKey6(void) {
    DoEditModeKey(13);
}

// Key: '7'
// FUNCTION: CARMA2_HW 0x00441c90
void C2_HOOK_FASTCALL NumberKey7(void) {
    DoEditModeKey(14);
}

// Key: '8'
// FUNCTION: CARMA2_HW 0x00441d10
void C2_HOOK_FASTCALL NumberKey8(void) {
    DoEditModeKey(15);
}

// Key: '9'
// FUNCTION: CARMA2_HW 0x00441d90
void C2_HOOK_FASTCALL NumberKey9(void) {
    DoEditModeKey(16);
}

// Key: 'kp_1'
// FUNCTION: CARMA2_HW 0x004443c0
void C2_HOOK_FASTCALL ToggleFlying(void) {

    if (gAllow_car_flying && gNet_mode == eNet_mode_none) {
        gCar_flying = !gCar_flying;
        if (gCar_flying) {
            NewTextHeadupSlot(4, 0, 500, -4, "We have lift off!!");
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, "Back down to Earth");
        }
    } else {
        gCar_flying = 0;
    }
}

// Key: '-'
// FUNCTION: CARMA2_HW 0x004e4cd0
void C2_HOOK_FASTCALL ScreenSmaller(void) {
    if (gMap_view != 2) {
        gRender_indent += 1;
        if (gRender_indent > 8) {
            gRender_indent = 8;
        }
        AdjustRenderScreenSize();
    }
}

// Key: '='
// FUNCTION: CARMA2_HW 0x004e4d00
void C2_HOOK_FASTCALL ScreenLarger(void) {
    if (gMap_view != 2) {
        gRender_indent -= 1;
        if (gRender_indent < 0) {
            gRender_indent = 0;
        }
        AdjustRenderScreenSize();
    }
}

// Key: 'Del'
// FUNCTION: CARMA2_HW 0x00502e50
void C2_HOOK_FASTCALL BuyArmour(void) {
    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        BuyPSPowerup(0);
    }
}

// Key: 'End'
// FUNCTION: CARMA2_HW 0x00502fd0
void C2_HOOK_FASTCALL BuyPower(void) {
    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
        } else {
            BuyPSPowerup(1);
        }
    }
}

// Key: 'PgDn'
// FUNCTION: CARMA2_HW 0x00503030
void C2_HOOK_FASTCALL BuyOffense(void) {
    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        BuyPSPowerup(2);
    }
}

// Key: ' '
// FUNCTION: CARMA2_HW 0x0040e430
void C2_NAKED C2_HOOK_FASTCALL ViewNetPlayer(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_spec* car;
    tNet_game_player_info* net_player;
    tNet_message* message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, last_score_index, 0x94);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_view_car, player, 0x4);

    if (!gOpponent_viewing_mode) {
        return;
    }
    gNet_player_to_view_index++;
    if (gNet_player_to_view_index >= gNumber_of_net_players) {
        gNet_player_to_view_index = -1;
    }
    if (gNet_player_to_view_index >= 0) {
        car = gNet_players[gNet_player_to_view_index].car;
    } else {
        int best;
        int idx;

        car = gRace_board[0].car;
        best = gRace_board[0].meter;
        if (gCurrent_net_game->type == eNet_game_type_foxy) {
            idx = gIt_or_fox;
            if (idx >= 0 && idx < gNumber_of_net_players) {
                car = gRace_board[idx].car;
                goto leader_done;
            }
        }
        if (gNumber_of_net_players > 1) {
            for (idx = 1; idx < gNumber_of_net_players; idx++) {
                if (gRace_board[idx].meter < best) {
                    best = gRace_board[idx].meter;
                    car = gRace_board[idx].car;
                }
            }
        }
    leader_done:
        ;
    }
    net_player = NetPlayerFromCar(car);
    if (gNet_mode == eNet_mode_host) {
        gCar_to_view = car;
        gCamera_yaw = 0;
        InitialiseExternalCamera();
        PositionExternalCamera(gCar_to_view, 200);
    } else if (gNet_mode == eNet_mode_client && gCar_to_view_id != net_player->ID) {
        gCar_to_view_id = net_player->ID;
        message = NetBuildGuaranteedMessage(eNet_message_chunk_type_view_car, 0);
        message->contents.view_car.player = gCar_to_view_id;
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
    }
#else
    __asm {
            mov eax, dword ptr [gOpponent_viewing_mode]
            push esi
            test eax, eax
            push edi
            je L0040e528
            mov eax, dword ptr [gNet_player_to_view_index]
            mov ecx, dword ptr [gNumber_of_net_players]
            inc eax
            cmp eax, ecx
            mov dword ptr [gNet_player_to_view_index], eax
            jl L0040e45c
            or eax, 0xffffffff
            mov dword ptr [gNet_player_to_view_index], eax
    L0040e45c:
            test eax, eax
            jge L0040e484
            mov eax, dword ptr [gCurrent_net_game]
            mov edx, dword ptr [gNet_players+0x94]
            mov esi, dword ptr [gNet_players+0xc8]
            cmp dword ptr [eax + 0x74], 7
            jne L0040e497
            mov eax, dword ptr [gIt_or_fox]
            test eax, eax
            jl L0040e497
            cmp eax, ecx
            jge L0040e497
    L0040e484:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov esi, dword ptr [ecx*4 + gNet_players+0xc8]
            jmp L0040e4b7
    L0040e497:
            cmp ecx, 1
            jle L0040e4b7
            mov eax, offset gNet_players+0x168
            lea edi, [ecx - 1]
    L0040e4a4:
            mov ecx, dword ptr [eax]
            cmp ecx, edx
            jge L0040e4af
            mov esi, dword ptr [eax + 0x34]
            mov edx, ecx
    L0040e4af:
            add eax, 0xd4
            dec edi
            jne L0040e4a4
    L0040e4b7:
            mov ecx, esi
            call NetPlayerFromCar
            mov ecx, dword ptr [gNet_mode]
            cmp ecx, 2
            jne L0040e4f0
            mov dword ptr [gCar_to_view], esi
            mov word ptr [gCamera_yaw], 0
            call InitialiseExternalCamera
            mov ecx, dword ptr [gCar_to_view]
            mov edx, 0xc8
            call PositionExternalCamera
            pop edi
            pop esi
            ret
    L0040e4f0:
            cmp ecx, 3
            jne L0040e528
            mov eax, dword ptr [eax + 0x24]
            mov ecx, dword ptr [gCar_to_view_id]
            cmp ecx, eax
            je L0040e528
            xor edx, edx
            mov cl, 0x2f
            mov dword ptr [gCar_to_view_id], eax
            call NetBuildGuaranteedMessage
            mov edx, dword ptr [gCar_to_view_id]
            mov dword ptr [eax + 0x1c], edx
            mov ecx, dword ptr [gCurrent_net_game]
            push 0
            mov edx, eax
            call NetGuaranteedSendMessageToHost
    L0040e528:
            pop edi
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// Key: '`'
// FUNCTION: CARMA2_HW 0x004448f0
void C2_HOOK_FASTCALL UserSendMessage(void) {
    if (gNet_mode != eNet_mode_none && gCurrent_net_game->options.enable_text_messages) {
        gEntering_message = 1;
    }
}

// Key: 't'
// FUNCTION: CARMA2_HW 0x004945f0
void C2_HOOK_FASTCALL ToggleTargetLock(void) {
    char name[256];
    char s[256];

    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_foxy) {
            return;
        }
        if (!gCurrent_net_game->options.show_players_on_map) {
            return;
        }
    }
    if (gTarget_lock_car_1 == NULL) {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_no_target_to_lock_onto), 0);
    } else {
        gTarget_lock_enabled = !gTarget_lock_enabled;
        if (gTarget_lock_enabled) {
            if (gNet_mode != eNet_mode_none) {
                strcpy(name, NetPlayerFromCar(gTarget_lock_car_2)->player_name);
            } else {
                strcpy(name, GetDriverName((tU16)gTarget_lock_car_2->car_ID >> 8, (tU16)gTarget_lock_car_2->car_ID & 0xff));
            }
            sprintf(s, "%s %s", GetMiscString(eMiscString_locked_onto), name);
            NewTextHeadupSlot2(4, 0, 1500, -4, s, 0);
        } else {
            NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_target_lock_off), 0);
        }
    }
}

// Key: 'y'
// FUNCTION: CARMA2_HW 0x00494700
void C2_HOOK_FASTCALL CycleTargetLock(void) {
    tVehicle_type category;
    int i;
    int car_index;
    int car_count;
    tCar_spec* car;

    if (gNet_mode == eNet_mode_none || (gCurrent_net_game->type != eNet_game_type_foxy && gCurrent_net_game->options.show_players_on_map)) {
        category = gNet_mode != eNet_mode_none ? eVehicle_net_player : eVehicle_opponent;
        if (gTarget_lock_car_1 == NULL) {
            if (category == eVehicle_self) {
                gTarget_lock_car_1 = &gProgram_state.current_car;
            } else {
                gTarget_lock_car_1 = GetCarSpec(category, 0);
            }
        }
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
            if (car == gTarget_lock_car_1) {
                if (i == car_count - 1) {
                    car_index = 0;
                } else {
                    car_index = i + 1;
                }
                do {
                    if (category == eVehicle_self) {
                        car = &gProgram_state.current_car;
                    } else {
                        car = GetCarSpec(category, car_index);
                    }
                    if (car_index == car_count - 1) {
                        car_index = 0;
                    } else {
                        car_index++;
                    }
                } while ((car != gTarget_lock_car_1 && car->knackered)
                    || (gNet_mode == eNet_mode_none && gCurrent_race.race_spec->race_type == kRaceType_Cars && !car->is_race_goal)
                    || (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_6 && NetPlayerFromCar(car)->field_0x80));
                gTarget_lock_car_1 = car;
                gTarget_lock_car_2 = car;
                gTarget_lock_enabled = 0;
                ToggleTargetLock();
                break;
            }
        }
    }
}

// Key: ';'
// FUNCTION: CARMA2_HW 0x004da9d0
void C2_HOOK_FASTCALL ToggleInventory(void) {
    gInventory_cycling = !gInventory_cycling;
    if (gInventory_cycling) {
        gInventory_timeout = GetTotalTime() + 3000;
    }
}

// Key: '['
// FUNCTION: CARMA2_HW 0x004dab80
void C2_NAKED C2_HOOK_FASTCALL LeftInventory(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int old_index;
    int new_index;
    tPowerup* powerup;
    tNet_message* msg;
    int i;

    gInventory_timeout = GetTotalTime() + 3000;
    old_index = gInventory_selected;
    gInventory_cycling = 1;
    if (old_index < 0) {
        return;
    }
    new_index = old_index;
    for (;;) {
        new_index++;
        if (new_index >= (int)CARPOCALYPSE2_ASIZE(gInventory)) {
            new_index = 0;
        }
        if (gInventory[new_index] < 0) {
            if (new_index == old_index) {
                return;
            }
            continue;
        }
        powerup = &gPowerup_array[gInventory[old_index]];
        if (powerup->initial_value >= 0 && gINT_006a0a6c == 0) {
            goto select_new;
        }
        if (powerup->got_time != 0 && powerup->lose_proc != NULL) {
            powerup->lose_proc(powerup, powerup->car);
        }
        if (powerup->net_type & 0x80) {
            ARPowerupLose(powerup, powerup->car);
        }
        if (gNet_mode != eNet_mode_none) {
            msg = NetBuildGuaranteedMessage(0x16, 0);
            *(int*)((tU8*)msg + 0x28) = 2;
            *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
            *(int*)((tU8*)msg + 0x20) = powerup - gPowerup_array;
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
        }
        gProgram_state.current_car.powerups[powerup - gPowerup_array] = 0;
        if (powerup->initial_value == 0) {
            for (i = 0; i < gNumber_of_icons; i++) {
                if (gPickedup_powerups[i].powerup == powerup) {
                    gPickedup_powerups[i].fizzle_stage = 3;
                    gPickedup_powerups[i].fizzle_direction = -1;
                    gPickedup_powerups[i].fizzle_start = GetTotalTime();
                    break;
                }
            }
        }
        powerup->got_time = 0;
select_new:
        gINT_006a0a5c = 1;
        gInventory_selected = new_index;
        return;
    }
#else
    __asm {
            push ebp
            call GetTotalTime
            add eax, 0xbb8
            xor ebp, ebp
            mov dword ptr [gInventory_timeout], eax
            mov eax, dword ptr [gInventory_selected]
            cmp eax, ebp
            mov dword ptr [gInventory_cycling], 1
            jl L004dacf1
            push edi
            push esi
            push ebx
            mov ebx, eax
    L004dabae:
            inc ebx
            cmp ebx, 0x14
            jl L004dabb6
            xor ebx, ebx
    L004dabb6:
            cmp dword ptr [ebx*4 + gInventory], ebp
            jl L004dace6
            mov ecx, dword ptr [eax*4 + gInventory]
            mov edx, dword ptr [gPowerup_array]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edx + ecx*4 + 0x34]
            cmp eax, ebp
            lea esi, [edx + ecx*4]
            jl L004dabf4
            cmp dword ptr [gINT_006a0a6c], ebp
            je L004dacd5
    L004dabf4:
            cmp dword ptr [esi + 4], ebp
            je L004dac07
            mov eax, dword ptr [esi + 0x40]
            cmp eax, ebp
            je L004dac07
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004dac07:
            test byte ptr [esi + 0x38], 0x80
            je L004dac17
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004dac17:
            cmp dword ptr [gNet_mode], ebp
            je L004dac65
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            mov edx, esi
            push ebp
            mov dword ptr [ecx + 0x28], 2
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], eax
            mov edi, dword ptr [gPowerup_array]
            sub edx, edi
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004dac65:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi + 0x34]
            cmp eax, ebp
            jne L004dacd2
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004dacd2
            mov ecx, offset gPickedup_powerups
    L004dac9f:
            cmp esi, dword ptr [ecx]
            je L004dacad
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004dac9f
            jmp L004dacd2
    L004dacad:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
    L004dacd2:
            mov dword ptr [esi + 4], ebp
    L004dacd5:
            mov eax, ebx
            mov dword ptr [gINT_006a0a5c], 1
            mov dword ptr [gInventory_selected], eax
    L004dace6:
            cmp ebx, eax
            jne L004dabae
            pop ebx
            pop esi
            pop edi
    L004dacf1:
            pop ebp
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

// Key: ']'
// FUNCTION: CARMA2_HW 0x004daa00
void C2_NAKED C2_HOOK_FASTCALL RightInventory(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int old_index;
    int new_index;
    tPowerup* powerup;
    tNet_message* msg;
    int i;

    gInventory_timeout = GetTotalTime() + 3000;
    old_index = gInventory_selected;
    gInventory_cycling = 1;
    if (old_index < 0) {
        return;
    }
    new_index = old_index;
    for (;;) {
        new_index--;
        if (new_index < 0) {
            new_index = (int)CARPOCALYPSE2_ASIZE(gInventory) - 1;
        }
        if (gInventory[new_index] < 0) {
            if (new_index == old_index) {
                return;
            }
            continue;
        }
        powerup = &gPowerup_array[gInventory[old_index]];
        if (powerup->initial_value >= 0 && gINT_006a0a6c == 0) {
            goto select_new;
        }
        if (powerup->got_time != 0 && powerup->lose_proc != NULL) {
            powerup->lose_proc(powerup, powerup->car);
        }
        if (powerup->net_type & 0x80) {
            ARPowerupLose(powerup, powerup->car);
        }
        if (gNet_mode != eNet_mode_none) {
            msg = NetBuildGuaranteedMessage(0x16, 0);
            *(int*)((tU8*)msg + 0x28) = 2;
            *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
            *(int*)((tU8*)msg + 0x20) = powerup - gPowerup_array;
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
        }
        gProgram_state.current_car.powerups[powerup - gPowerup_array] = 0;
        if (powerup->initial_value == 0) {
            for (i = 0; i < gNumber_of_icons; i++) {
                if (gPickedup_powerups[i].powerup == powerup) {
                    gPickedup_powerups[i].fizzle_stage = 3;
                    gPickedup_powerups[i].fizzle_direction = -1;
                    gPickedup_powerups[i].fizzle_start = GetTotalTime();
                    break;
                }
            }
        }
        powerup->got_time = 0;
select_new:
        gINT_006a0a5c = 1;
        gInventory_selected = new_index;
        return;
    }
#else
    __asm {
            push ebp
            call GetTotalTime
            add eax, 0xbb8
            xor ebp, ebp
            mov dword ptr [gInventory_timeout], eax
            mov eax, dword ptr [gInventory_selected]
            cmp eax, ebp
            mov dword ptr [gInventory_cycling], 1
            jl L004dab71
            push edi
            push esi
            push ebx
            mov ebx, eax
    L004daa2e:
            dec ebx
            jns L004daa36
            mov ebx, 0x13
    L004daa36:
            cmp dword ptr [ebx*4 + gInventory], ebp
            jl L004dab66
            mov ecx, dword ptr [eax*4 + gInventory]
            mov edx, dword ptr [gPowerup_array]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edx + ecx*4 + 0x34]
            cmp eax, ebp
            lea esi, [edx + ecx*4]
            jl L004daa74
            cmp dword ptr [gINT_006a0a6c], ebp
            je L004dab55
    L004daa74:
            cmp dword ptr [esi + 4], ebp
            je L004daa87
            mov eax, dword ptr [esi + 0x40]
            cmp eax, ebp
            je L004daa87
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004daa87:
            test byte ptr [esi + 0x38], 0x80
            je L004daa97
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004daa97:
            cmp dword ptr [gNet_mode], ebp
            je L004daae5
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            mov edx, esi
            push ebp
            mov dword ptr [ecx + 0x28], 2
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], eax
            mov edi, dword ptr [gPowerup_array]
            sub edx, edi
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004daae5:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi + 0x34]
            cmp eax, ebp
            jne L004dab52
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004dab52
            mov ecx, offset gPickedup_powerups
    L004dab1f:
            cmp esi, dword ptr [ecx]
            je L004dab2d
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004dab1f
            jmp L004dab52
    L004dab2d:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
    L004dab52:
            mov dword ptr [esi + 4], ebp
    L004dab55:
            mov eax, ebx
            mov dword ptr [gINT_006a0a5c], 1
            mov dword ptr [gInventory_selected], eax
    L004dab66:
            cmp ebx, eax
            jne L004daa2e
            pop ebx
            pop esi
            pop edi
    L004dab71:
            pop ebp
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

// Key: '*' (keypad)
// FUNCTION: CARMA2_HW 0x00444ed0
void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void) {
    if (!gAction_replay_mode) {

        ChangeCameraType();
        NewTextHeadupSlot2(4, 0, 1000, -4, GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode), 0);
    }
}

// Key: 'c'
// GLOBAL: CARMA2_HW 0x00679324
tActionReplayCameraMode gCameraTypeBeforeCockpit;

// FUNCTION: CARMA2_HW 0x00443c90
void C2_HOOK_FASTCALL CheckKevKeys(void) {
    int i;
    tU32* value;
    char s[128];

    value = KevKeyService();
    if (value[0] == 0) {
        return;
    }

    for (i = 0; gKev_keys[i].action_proc != NULL; i++) {
        if (gKev_keys[i].code == value[0] && gKev_keys[i].code2 == value[1]) {
            break;
        }
    }

    if (gKev_keys[i].action_proc == NULL) {
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        if (gKev_keys[i].num != 0xa11ee75d) {
            strcpy(s, gNet_players[gThis_net_player_index].player_name);
            strcat(s, " ");
            strcat(s, GetMiscString(224));
            NetSendHeadupToAllPlayers(s);
        } else {
            strcpy(s, gNet_players[gThis_net_player_index].player_name);
            strcat(s, " ");
            strcat(s, GetMiscString(225));
            NetSendHeadupToEverybody(s);
            gKev_keys[i].action_proc(gKev_keys[i].num);
        }
        return;
    }
    gKev_keys[i].action_proc(gKev_keys[i].num);
}

// FUNCTION: CARMA2_HW 0x00442e90
void C2_NAKED C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 start_menu_time;

    start_menu_time = PDGetTotalTime();
    CheckQuit();
    AddLostTime(PDGetTotalTime() - start_menu_time);
    CheckToggles(pRacing);
    if (pRacing & !gAction_replay_mode) {
        CheckOtherRacingKeys();
    }
#else
    __asm {
            push ebx
            push esi
            push edi
            mov ebx, ecx
            call PDGetTotalTime
            mov esi, eax
            call CheckQuit
            call PDGetTotalTime
            mov ecx, eax
            sub ecx, esi
            call AddLostTime
            call CheckKeysForMouldiness
            mov esi, offset gToggle_array
            mov edi, 0xfffffffe
    L00442ebe:
            mov eax, dword ptr [esi + 8]
            test eax, eax
            je L00442ecd
            test ebx, ebx
            je L00442f62
    L00442ecd:
            mov eax, dword ptr [gTyping]
            test eax, eax
            jne L00442edf
            mov eax, dword ptr [gEntering_message]
            test eax, eax
            je L00442ee4
    L00442edf:
            cmp dword ptr [esi + 4], edi
            je L00442f62
    L00442ee4:
            mov ecx, dword ptr [esi]
            cmp ecx, edi
            je L00442ef3
            call KeyIsDownNoMouldiness
            test eax, eax
            je L00442f51
    L00442ef3:
            mov ecx, dword ptr [esi + 4]
            cmp ecx, edi
            jne L00442f41
            mov eax, dword ptr [esi + 0xc]
            test eax, eax
            je L00442f41
            xor ecx, ecx
            call PDKeyDown
            test eax, eax
            jne L00442f3d
            mov ecx, 1
            call PDKeyDown
            test eax, eax
            jne L00442f3d
            mov ecx, 2
            call PDKeyDown
            test eax, eax
            jne L00442f3d
            mov ecx, 3
            call PDKeyDown
            test eax, eax
            jne L00442f3d
            mov eax, 1
            jmp L00442f46
    L00442f3d:
            xor eax, eax
            jmp L00442f46
    L00442f41:
            call KeyIsDown
    L00442f46:
            test eax, eax
            je L00442f51
            mov eax, 1
            jmp L00442f53
    L00442f51:
            xor eax, eax
    L00442f53:
            cmp dword ptr [esi + 0x10], eax
            je L00442f62
            test eax, eax
            mov dword ptr [esi + 0x10], eax
            je L00442f62
            call dword ptr [esi + 0x14]
    L00442f62:
            add esi, 0x18
            cmp esi, offset gEdit_mode_names
            jb L00442ebe
            test ebx, ebx
            je L00442f83
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00442f83
            call CheckOtherRacingKeys
    L00442f83:
            pop edi
            pop esi
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

void C2_HOOK_FASTCALL CheckToggles(int pRacing) {
    int i;
    int new_state;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gToggle_array); i++) {
        if (!pRacing && gToggle_array[i].in_game_only) {
            continue;
        }
        if ((gTyping || gEntering_message) && gToggle_array[i].key2 == -2) {
            continue;
        }
        new_state = 0;
        if (gToggle_array[i].key1 == -2 || KeyIsDownNoMouldiness(gToggle_array[i].key1)) {
            if (gToggle_array[i].key2 == -2 && gToggle_array[i].exact_modifiers) {
                if (!PDKeyDown(0) && !PDKeyDown(1) && !PDKeyDown(2) && !PDKeyDown(3)) {
                    new_state = 1;
                }
            } else {
                if (KeyIsDown(gToggle_array[i].key2)) {
                    new_state = 1;
                }
            }
        }
        if (gToggle_array[i].on_last_time != new_state) {
            gToggle_array[i].on_last_time = new_state;
            if (new_state) {
                gToggle_array[i].action_proc();
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00443ba0
void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar) {
    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound3D(gEffects_outlet, eSoundId_Horn,
                                                &pCar->car_master_actor->t.t.translate.t, &pCar->vel,
                                                0, 0xff, ((pCar->car_ID & 0x7) + 0xc) << 12, 0x10000);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag) != 0) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00442140
int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around) {
    br_vector3 car_pos;
    br_vector3 offset_c;
    br_vector3 offset_w;
    int result;
    int i;

    if (gCar_flying && pCar != NULL && pCar->driver == eDriver_local_human) {
        return 0;
    }
    if (gAction_replay_mode) {
        return 0;
    }
    if (pCar->number_of_wheels_on_ground != 0) {
        for (i = pCar->collision_info->box_face_start; i < pCar->collision_info->box_face_end; i++) {
            if (gFace_list__car[i].material != NULL
                    && gFace_list__car[i].material->identifier != NULL
                    && gFace_list__car[i].material->identifier[0] == '^') {
                return 1;
            }
        }
        return 0;
    }
    BrVector3Copy(&car_pos, &pCar->car_master_actor->t.t.translate.t);
    car_pos.v[1] += 0.5f;
    if (FindYVerticallyBelow2(&car_pos) < -100.f) {
        BrVector3Set(&offset_c, 0.f, 1.f, 0.f);
        BrMatrix34ApplyV(&offset_w, &offset_c, &pCar->car_master_actor->t.t.mat);
        BrVector3Add(&car_pos, &pCar->car_master_actor->t.t.translate.t, &offset_w);
        if (FindYVerticallyBelow2(&car_pos) < -100.f) {
            if (pCheck_around) {
                pCar->car_master_actor->t.t.translate.t.v[0] += 0.05f;
                result = CarWorldOffFallenCheckThingy(pCar, 0);
                pCar->car_master_actor->t.t.translate.t.v[0] -= 0.05f;
                if (!result) {
                    return 0;
                }
                pCar->car_master_actor->t.t.translate.t.v[2] += 0.05f;
                result = CarWorldOffFallenCheckThingy(pCar, 0);
                pCar->car_master_actor->t.t.translate.t.v[2] -= 0.05f;
                return result;
            } else {
                return 1;
            }
        }
    } else if (gMaterial_below->identifier != NULL && gMaterial_below->identifier[0] == '^') {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00442130
int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar) {
    return CarWorldOffFallenCheckThingy(pCar, 1);
}

void C2_HOOK_FASTCALL CheckHornLocal(tCar_spec* pCar) {

    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound(gEffects_outlet, eSoundId_Horn);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag) != 0) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}

void C2_HOOK_FASTCALL CheckHorns(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            CheckHorn3D(gNet_players[i].car);
        }
    } else {
        CheckHornLocal(&gProgram_state.current_car);
    }
}

void C2_HOOK_FASTCALL CheckMapRenderMove(void) {
    int amount;
    float old_x;
    float old_y;

    old_y = gMap_render_y;
    old_x = gMap_render_x;
    if (gMap_view == 2) {
        amount = (int)(gFrame_period / 10.f);
        KeyIsDown(8);
        if (KeyIsDown(31)) {
            gMap_render_y -= (float)amount;
        } else if (KeyIsDown(32)) {
            gMap_render_y += (float)amount;
        }
        if (KeyIsDown(33)) {
            gMap_render_x -= (float)amount;
        } else if (KeyIsDown(34)) {
            gMap_render_x += (float)amount;
        }
        if (gMap_render_x != old_x || gMap_render_y != old_y) {
            SetIntegerMapRenders();
            if (gMap_render_x_i < gCurrent_graf_data->map_render_x_marg) {
                gMap_render_x = (float)((gCurrent_graf_data->map_render_x_marg + 3) & ~3);
            }
            if (gMap_render_y_i < gCurrent_graf_data->map_render_y_marg) {
                gMap_render_y = (float)((gCurrent_graf_data->map_render_y_marg + 1) & ~1);
            }
            if (gBack_screen->width - gCurrent_graf_data->map_render_x_marg - gMap_render_width_i < gMap_render_x_i) {
                gMap_render_x = (float)((gBack_screen->width - gCurrent_graf_data->map_render_x_marg - gMap_render_width_i) & ~3);
            }
            if (gBack_screen->height - gCurrent_graf_data->map_render_y_marg - gMap_render_height_i < gMap_render_y_i) {
                gMap_render_y = (float)((gBack_screen->height - gCurrent_graf_data->map_render_y_marg - gMap_render_height_i) & ~1);
            }
            SetIntegerMapRenders();
            AdjustRenderScreenSize();
        }
    }
}

int C2_HOOK_FASTCALL TradeInAPO(tS32 cost) {

    if (cost > 0) {
        while (cost > 0
                && (gProgram_state.current_car.power_up_levels[0] != 0
                        || gProgram_state.current_car.power_up_levels[1] != 0
                        || gProgram_state.current_car.power_up_levels[2] != 0)) {
            int type;
            int value;

            type = IRandomBetween(0, 2);
            if (gProgram_state.current_car.power_up_levels[type] != 0) {
                gProgram_state.current_car.power_up_levels[type] -= 1;

                if (gNet_mode == eNet_mode_none) {
                    value = gTrade_in_value_APO.initial[gProgram_state.skill_level];
                }
                else {
                    value = gTrade_in_value_APO.initial_network[gCurrent_net_game->type];
                }
                cost -= value;
                if (gNet_mode == eNet_mode_none) {
                    value = gTrade_in_value_APO.initial[gProgram_state.skill_level];
                }
                else {
                    value = gTrade_in_value_APO.initial_network[gCurrent_net_game->type];
                }
                gProgram_state.credits += value;
            }
        }
    }
    if (cost < 0) {
        cost = 0;
    }
    return cost;
}

void C2_HOOK_FASTCALL RecoverCar(void) {

    if (gNet_mode == eNet_mode_none || gPalette_fade_time == 0) {
        gRecover_car = 1;
    }
    gProgram_state.current_car.time_to_recover = 0;
}

void C2_HOOK_FASTCALL SetQuickRecovery(void) {

    if (gNet_mode == eNet_mode_none) {
        RecoverCar();
    } else {
        if (gProgram_state.current_car.time_to_recover == 0 && !CheckRecoverCost()) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_cant_afford_to_recover));
            DoFancyHeadup(27);
            KnackerThisCar(&gProgram_state.current_car);
            SendGameplayToHost(3, 0, 0, 0, 0);
        } else {
            if (gProgram_state.current_car.time_to_recover == 0 || GetRaceTime() + 500 < gProgram_state.current_car.time_to_recover) {
                gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
                gCancel_recover = 0;
            }
        }
    }
}

void C2_HOOK_FASTCALL CheckForBeingOutOfThisWorld(void) {
    // GLOBAL: CARMA2_HW 0x0067c3f4
    static tU32 the_time;
    // GLOBAL: CARMA2_HW 0x0067c480
    static tU32 sLast_check;

    the_time = PDGetTotalTime();
    if (gProgram_state.current_car.disabled) {
        gRecover_timer = 0;
        return;
    }

    if (gRecover_timer != 0) {
        if ((!gProgram_state.current_car.frame_collision_flag && gProgram_state.current_car.number_of_wheels_on_ground == 0) || IsCarInTheSea()) {
            gRecover_timer -= gFrame_period;
            if (gRecover_timer <= 0 || IsCarInTheSea() == 2) {
                gRecover_timer = 0;
                SetQuickRecovery();
                gHad_auto_recover = 1;
            }
            return;
        } else {
            gRecover_timer = 0;
        }
    }

    if (the_time - sLast_check > 200) {
        sLast_check = the_time;
        if (HasCarFallenOffWorld(&gProgram_state.current_car)) {
            gRecover_timer = 3000;
        }
    }
    if (IsCarInTheSea() && gRecover_timer == 0) {
        gRecover_timer = 3000;
    }
}

void C2_HOOK_FASTCALL SortOutRecover(tCar_spec* pCar) {
    int the_time;
    int val;
    // GLOBAL: CARMA2_HW 0x0067c494
    static int old_time;

    the_time = GetRaceTime() - gPalette_fade_time;
    if (the_time < 0) {
        gPalette_fade_time = 0;
        old_time = 0;
    }
    if (the_time < 500) {
        val = 256 - (the_time * 256) / 500;
    } else {
        if (old_time < 500) {
            FlipUpCar(pCar);
            PipeSingleSpecial(ePipe_special_fade);
        }
        val = ((the_time - 1000) * 256) / 500;
        if (val >= 256) {
            val = 256;
            gPalette_fade_time = 0;
            old_time = 0;
            SetCollisionInfoChildsDoNothing(pCar->collision_info, 0);
        } else if (!pCar->collision_info->disable_move_rotate) {
            SetCollisionInfoChildsDoNothing(pCar->collision_info, 1);
        }
    }
    if (val <= 0) {
        val = 0;
    }
    SetFadedPalette(val);
    old_time = the_time;
}

// FUNCTION: CARMA2_HW 0x00442970
void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar) {
    if (gNet_mode != eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        DisableCar(pCar);
        gPalette_fade_time = GetRaceTime();
        NetPlayerStatusChanged(ePlayer_status_recovering);
    } else {
        FlipUpCar(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x00442f90
void C2_HOOK_FASTCALL CheckOtherRacingKeys(void) {
    int i;
    int new_level;
    int old_level;
    char s[256];
    tU32 cost;
    br_scalar ts;
    tCar_spec* car;
    float bodywork_repair_amount;
    float cost_float;
    // GLOBAL: CARMA2_HW 0x0067c484
    static tU32 total_repair_cost;
    // GLOBAL: CARMA2_HW 0x0067c488
    static tS3_sound_tag sound_tag;
    // GLOBAL: CARMA2_HW 0x0067c3fc
    static br_scalar amount;
    // GLOBAL: CARMA2_HW 0x0067c48c
    static int NeedToExpandBoundingBox;
    // GLOBAL: CARMA2_HW 0x0067c3c0
    static int total_difference;
    // GLOBAL: CARMA2_HW 0x0067c490
    static int stopped_repairing;

    (void)total_difference;

    car = GetCarSpec(eVehicle_self, 0);
    CheckMapRenderMove();
    CheckHorns();
    CheckForBeingOutOfThisWorld();
    if (gPalette_fade_time != 0) {
        SortOutRecover(car);
    } else if (gNet_mode != eNet_mode_none && NetGetPlayerStatus() == ePlayer_status_recovering) {
        NetPlayerStatusChanged(ePlayer_status_racing);
    }

    if ((gAuto_repair || KeyIsDown(45)) && !gRace_finished && !gProgram_state.current_car.knackered && !gWait_for_it && !gEntering_message) {
        if (gProgram_state.current_car.car_crush_spec != NULL && gProgram_state.current_car.car_crush_spec->field_0x4b8 != 0) {
            FlipUpCar(&gProgram_state.current_car);
        }
        if (!gAuto_repair && gRepair_last_time == 0 && GetTotalTime() - gLast_repair_time < 1200) {
            gAuto_repair = 1;
        }
        gLast_repair_time = GetTotalTime();
        gRepair_last_time = 1;
        if (!NeedToExpandBoundingBox) {
            bodywork_repair_amount = RepairCar(gProgram_state.current_car.car_ID, gFrame_period, &amount);
            NeedToExpandBoundingBox = bodywork_repair_amount > 0.f;
            cost = 0;
            for (i = 0; i < CARPOCALYPSE2_ASIZE(gProgram_state.current_car.damage_units); i++) {
                tDamage_unit* unit = &gProgram_state.current_car.damage_units[i];

                old_level = unit->damage_level;
                if (amount == 0.0f) {
                    new_level = 0;
                } else {
                    new_level = (int)((float)unit->damage_level - floorf((float)unit->damage_level * bodywork_repair_amount / amount));
                }
                if (new_level < 0) {
                    unit->damage_level = 0;
                } else if (new_level >= 100) {
                    unit->damage_level = 99;
                } else {
                    unit->damage_level = new_level;
                }
                unit->smoke_last_level = unit->damage_level;
                if (gNet_mode == eNet_mode_none) {
                    ts = gRepair_cost.initial[gProgram_state.skill_level];
                } else {
                    ts = gRepair_cost.initial_network[gCurrent_net_game->type];
                }
                cost = (int)((float)cost + (float)(old_level - unit->damage_level) * ts);
                total_difference += old_level - new_level;
            }
            total_difference %= 100;
            cost = 10 * (cost / 10);
            if (((total_repair_cost == 0 && cost != 0) || bodywork_repair_amount != 0.0f) && sound_tag == 0) {
                sound_tag = DRS3StartSound(gXXX_outlet, eSoundId_DamageRepair);
            }
            StopCarSmoking(&gProgram_state.current_car);
            if (cost == 0 && bodywork_repair_amount == 0.0) {
                gAuto_repair = 0;
            }
            if (!gFree_repairs) {
                if (cost > (tU32)gProgram_state.credits) {
                    TradeInAPO(cost);
                }
                SpendCredits(cost);
            }
            total_repair_cost += cost;
            if (total_repair_cost) {
                if (gFree_repairs) {
                    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_repairing_for_free));
                } else {
                    sprintf(s, "%s %d", GetMiscString(eMiscString_repair_cost_colon), total_repair_cost);
                    NewTextHeadupSlot(4, 0, 1000, -4, s);
                }
            }
        }
    } else {
        (void)stopped_repairing;

        gRepair_last_time = 0;
        stopped_repairing = 0;
        total_repair_cost = 0;
        total_difference = 0;
        if (sound_tag != 0) {
            while (S3SoundStillPlaying(sound_tag)) {
                DRS3StopSound(sound_tag);
            }
            sound_tag = 0;
        }
    }
    if (NeedToExpandBoundingBox) {
        NeedToExpandBoundingBox = gProgram_state.current_car.car_crush_spec->expand_bounding_box;
    }
    if (gToo_poor_for_recovery_timeout) {
        if (PDGetTotalTime() < gToo_poor_for_recovery_timeout) {
            if (PDKeyDown(51)) {
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_cant_afford_to_recover));
                DoFancyHeadup(27);
                KnackerThisCar(&gProgram_state.current_car);
                SendGameplayToHost(3, 0, 0, 0, 0);
            }
        } else {
            gToo_poor_for_recovery_timeout = 0;
        }
    }

    if (!gRecover_car || gProgram_state.current_car.knackered) {
        gHad_auto_recover = 0;
    } else {
        gRecover_timer = 0;
        SetFlipUpCar(car);
        if (gNet_mode != eNet_mode_none) {
            NewTextHeadupSlot(4, 0, 1500, -4, " ");
        }
        if (gRecovery_voucher_count != 0
                || (gNet_mode != eNet_mode_none && NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80)) {
            if (gRecovery_voucher_count != 0) {
                gRecovery_voucher_count -= 1;
            }
            sprintf(s, "%s", GetMiscString(eMiscString_recovering_for_free));
            NewTextHeadupSlot(4, 0, 1500, -4, s);
        } else {
            if (gNet_mode == eNet_mode_none) {

                if (!gProgram_state.current_car.knackered) {

                    if (gNet_mode == eNet_mode_none) {
                        cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
                    } else {
                        cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
                    }
                    if (cost_float > (float)gProgram_state.credits) {
                        if (gNet_mode == eNet_mode_none) {
                            cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
                        } else {
                            cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
                        }
                        TradeInAPO((int)cost_float);
                    }
                }
                if (gNet_mode == eNet_mode_none) {
                    cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
                } else {
                    cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
                }
                SpendCredits((int)cost_float);
            }
            if (gNet_mode == eNet_mode_none) {
                cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
            } else {
                cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
            }
            sprintf(s, "%s %d", GetMiscString(eMiscString_recovery_cost_colon), (int)cost_float);
            NewTextHeadupSlot(4, 0, 1500, -4, s);
        }
        CancelPendingCunningStunt();
        PipeSingleSpecial(ePipe_special_fade);
    }
    gRecover_car = 0;
}

// FUNCTION: CARMA2_HW 0x004429b0
void C2_NAKED C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    int new_pos;
    int i;
    int j;
    int k;

    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gNet_mode == eNet_mode_none) {
        FadePaletteDown();
        while (KeyIsDown(0x3c)) {
        }
    }

    if (pCar_spec->car_crush_spec != NULL && pCar_spec->car_crush_spec->field_0x4b8) {
        WeldCar(pCar_spec);
    }
    SetCollisionInfoChildsDoNothing(pCar_spec->collision_info, 0);
    EnableCar(pCar_spec);

    new_pos = 1;
    for (i = 0; i < 4; i++) {
        if (pCar_spec->susp_height[i / 2] <= pCar_spec->oldd[i]) {
            new_pos = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        br_scalar t;
        br_vector3 dir;
        br_vector3 nor;
        br_vector3 p;
        br_material* mat;

        BrVector3Sub(&nor, &pCar_spec->car_master_actor->t.t.translate.t, (br_vector3*)pCar_spec->last_safe_positions[0].m[3]);
        if (BrVector3LengthSquared(&nor) > 8.3015966f) {
            new_pos = 0;
        }
        BrMatrix34Copy(&pCar_spec->car_master_actor->t.t.mat, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->collision_info->transform_matrix, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->old_frame_mat, &pCar_spec->last_safe_positions[new_pos]);
        BrVector3Set(&dir, 0.f, 0.2898551f, 0.f);
        FindFace(&pCar_spec->car_master_actor->t.t.translate.t, &dir, &nor, &t, &mat);
        if (t > 1.f) {
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &dir);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
        BrVector3Set(&nor, 0.f, 0.f, -0.0002f);
        BrMatrix34ApplyV(&pCar_spec->collision_info->v, &nor, &pCar_spec->car_master_actor->t.t.mat);
        BrVector3Set(&pCar_spec->collision_info->omega, 0.f, 0.f, 0.f);
        BrVector3Negate(&pCar_spec->direction, (br_vector3*)pCar_spec->collision_info->transform_matrix.m[2]);

        for (j = 0; j <= new_pos; j++) {
            for (k = 0; k < CARPOCALYPSE2_ASIZE(pCar_spec->last_safe_positions) - 1; k++) {
                BrMatrix34Copy(&pCar_spec->last_safe_positions[k], &pCar_spec->last_safe_positions[k + 1]);
            }
        }
        FlipUpCollisionInfo(pCar_spec->collision_info);
        PositionChildren(pCar_spec->collision_info);
        for (j = 0; !TestForCarInSensiblePlace(pCar_spec, &p) && j < 10; j++) {
            BrVector3InvScale(&p, &p, 2.f * WORLD_SCALE);
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &p);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
    }
    BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3],
        &pCar_spec->car_master_actor->t.t.translate.t);
    pCar_spec->curvature = 0.f;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->oldd); i++) {
        pCar_spec->oldd[i] = pCar_spec->ride_height;
    }
    pCar_spec->revs = 0.f;
    pCar_spec->gear = 0;
    pCar_spec->collision_info->auto_special_volume = NULL;
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human) {
        InitialiseExternalCamera();
        PositionExternalCamera(pCar_spec, 100);
    }
#else
    __asm {
            sub esp, 0x38
            push ebx
            push ebp
            push esi
            mov esi, ecx
            xor ebp, ebp
            push edi
            cmp esi, ebp
            mov dword ptr [esp + 0x14], ebp
            je L004429f2
            cmp dword ptr [esi + 0xc], 8
            jne L004429f2
            cmp dword ptr [gNet_mode], ebp
            jne L004429f2
            call FadePaletteDown
            mov ecx, 0x3c
            call KeyIsDown
            test eax, eax
            je L004429f2
    L004429e4:
            mov ecx, 0x3c
            call KeyIsDown
            test eax, eax
            jne L004429e4
    L004429f2:
            mov eax, dword ptr [esi + 0x18d4]
            cmp eax, ebp
            je L00442a0d
            mov cl, byte ptr [eax + 0x4b8]
            test cl, cl
            je L00442a0d
            mov ecx, esi
            call WeldCar
    L00442a0d:
            mov ecx, dword ptr [esi + 8]
            xor edx, edx
            call SetCollisionInfoChildsDoNothing
            mov ecx, esi
            call EnableCar
            mov ebx, 1
            xor ecx, ecx
            mov dword ptr [esp + 0x10], ebx
            lea edx, [esi + 0x1264]
    L00442a2f:
            _emit 0xd9
        _emit 0x02
            mov eax, ecx
            sar eax, 1
            fcomp dword ptr [esi + eax*4 + 0x1218]
            fnstsw ax
            test ah, 1
            jne L00442a49
            mov dword ptr [esp + 0x10], ebp
            mov ebx, ebp
    L00442a49:
            inc ecx
            add edx, 4
            cmp ecx, 4
            jl L00442a2f
            jmp L00442a5a
    L00442a54:
            mov ebx, dword ptr [esp + 0x10]
            xor ebp, ebp
    L00442a5a:
            mov ecx, dword ptr [esi + 0x10]
            fld dword ptr [ecx + 0x50]
            fsub dword ptr [esi + 0xe5c]
            fstp dword ptr [esp + 0x24]
            fld dword ptr [ecx + 0x54]
            fsub dword ptr [esi + 0xe60]
            fld dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fld dword ptr [ecx + 0x58]
            fsub dword ptr [esi + 0xe64]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gFloat_00589c40+0x38]
            fnstsw ax
            test ah, 0x41
            jne L00442ab5
            mov dword ptr [esp + 0x10], ebp
            mov ebx, ebp
    L00442ab5:
            lea edx, [ebx + ebx*2]
            add ecx, 0x2c
            shl edx, 4
            lea edi, [edx + esi + 0xe38]
            push edi
            push ecx
            call BrMatrix34Copy
            mov eax, dword ptr [esi + 8]
            add esp, 8
            add eax, 0x8c
            push edi
            push eax
            call BrMatrix34Copy
            add esp, 8
            lea eax, [esi + 0x1c]
            push edi
            push eax
            call BrMatrix34Copy
            add esp, 8
            lea ecx, [esp + 0x20]
            lea edx, [esp + 0x1c]
            lea eax, [esp + 0x24]
            push ecx
            mov ecx, dword ptr [esi + 0x10]
            push edx
            push eax
            lea edx, [esp + 0x48]
            add ecx, 0x50
            mov dword ptr [esp + 0x48], 0
            mov dword ptr [esp + 0x4c], 0x3e9467e2
            mov dword ptr [esp + 0x50], 0
            call FindFace
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gFloat_00589c40+0x14]
            fnstsw ax
            test ah, 0x41
            jne L00442b9e
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x3c]
            fadd dword ptr [eax + 0x50]
            fstp dword ptr [eax + 0x50]
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x40]
            fadd dword ptr [eax + 0x54]
            fstp dword ptr [eax + 0x54]
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x44]
            fadd dword ptr [eax + 0x58]
            fstp dword ptr [eax + 0x58]
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x50]
            mov dword ptr [edx + 0xb0], eax
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x54]
            mov dword ptr [edx + 0xb4], eax
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x58]
            mov dword ptr [edx + 0xb8], eax
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [eax + 0x50]
            mov dword ptr [esi + 0x40], ecx
            mov edx, dword ptr [eax + 0x54]
            mov dword ptr [esi + 0x44], edx
            mov eax, dword ptr [eax + 0x58]
            mov dword ptr [esi + 0x48], eax
    L00442b9e:
            mov ecx, dword ptr [esi + 0x10]
            mov eax, dword ptr [esi + 8]
            add ecx, 0x2c
            lea edx, [esp + 0x24]
            push ecx
            add eax, 0x68
            push edx
            push eax
            mov dword ptr [esp + 0x30], 0
            mov dword ptr [esp + 0x34], 0
            mov dword ptr [esp + 0x38], 0xb951b717
            call BrMatrix34ApplyV
            mov ecx, dword ptr [esi + 8]
            add esp, 0xc
            cmp ebx, ebp
            mov dword ptr [ecx + 0x74], ebp
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x78], ebp
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x7c], ebp
            mov eax, dword ptr [esi + 8]
            fld dword ptr [eax + 0xa4]
            fchs
            fstp dword ptr [esi + 0x70]
            fld dword ptr [eax + 0xa8]
            fchs
            fstp dword ptr [esi + 0x74]
            fld dword ptr [eax + 0xac]
            fchs
            fstp dword ptr [esi + 0x78]
            jl L00442c2c
            inc ebx
    L00442c0c:
            lea eax, [esi + 0xe38]
            mov ebp, 0x13
    L00442c17:
            lea edi, [eax + 0x30]
            push edi
            push eax
            call BrMatrix34Copy
            add esp, 8
            dec ebp
            mov eax, edi
            jne L00442c17
            dec ebx
            jne L00442c0c
    L00442c2c:
            mov ebp, dword ptr [esi + 8]
            mov edi, dword ptr [ebp + 0x224]
            test edi, edi
            je L00442c9b
    L00442c39:
            lea eax, [edi + 0x8c]
            mov dword ptr [esp + 0x18], 3
    L00442c47:
            mov ecx, ebp
            mov edx, 3
            sub ecx, edi
    L00442c50:
            mov ebx, dword ptr [ecx + eax]
            mov dword ptr [eax], ebx
            add eax, 4
            dec edx
            jne L00442c50
            mov ecx, dword ptr [esp + 0x18]
            dec ecx
            mov dword ptr [esp + 0x18], ecx
            jne L00442c47
            push 0
            push 0
            lea ecx, [edi + 0x68]
            push 0
            push ecx
            call BrVector3SetFloat
            add esp, 0x10
            lea edx, [edi + 0x74]
            push 0
            push 0
            push 0
            push edx
            call BrVector3SetFloat
            add esp, 0x10
            mov ecx, edi
            call FlipUpCollisionInfo
            mov edi, dword ptr [edi + 0x220]
            test edi, edi
            jne L00442c39
    L00442c9b:
            mov ecx, dword ptr [esi + 8]
            call PositionChildren
            lea edx, [esp + 0x30]
            mov ecx, esi
            xor edi, edi
            call TestForCarInSensiblePlace
            test eax, eax
            jne L00442d68
    L00442cb8:
            cmp edi, 0xa
            jge L00442d68
            fld dword ptr [esp + 0x30]
            fmul dword ptr [gFloat_00589c40+0x44]
            mov eax, dword ptr [esi + 0x10]
            inc edi
            fstp dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [gFloat_00589c40+0x44]
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            fmul dword ptr [gFloat_00589c40+0x44]
            fstp dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x30]
            fadd dword ptr [eax + 0x50]
            fstp dword ptr [eax + 0x50]
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x34]
            fadd dword ptr [eax + 0x54]
            fstp dword ptr [eax + 0x54]
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x38]
            fadd dword ptr [eax + 0x58]
            fstp dword ptr [eax + 0x58]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [eax + 0x50]
            mov dword ptr [ecx + 0xb0], edx
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [eax + 0x54]
            mov dword ptr [ecx + 0xb4], edx
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [eax + 0x58]
            mov dword ptr [ecx + 0xb8], edx
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [eax + 0x50]
            mov dword ptr [esi + 0x40], ecx
            mov edx, dword ptr [eax + 0x54]
            mov dword ptr [esi + 0x44], edx
            mov eax, dword ptr [eax + 0x58]
            lea edx, [esp + 0x30]
            mov ecx, esi
            mov dword ptr [esi + 0x48], eax
            call TestForCarInSensiblePlace
            test eax, eax
            je L00442cb8
    L00442d68:
            mov ecx, dword ptr [esp + 0x14]
            inc ecx
            cmp edi, 0xa
            mov dword ptr [esp + 0x14], ecx
            jne L00442d81
            mov eax, ecx
            cmp eax, 3
            jl L00442a54
    L00442d81:
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x50]
            mov dword ptr [edx + 0xb0], eax
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x54]
            mov dword ptr [edx + 0xb4], eax
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x58]
            mov ecx, 4
            mov dword ptr [edx + 0xb8], eax
            xor edx, edx
            mov dword ptr [esi + 0x1254], edx
            lea eax, [esi + 0x1264]
    L00442dc1:
            mov edi, dword ptr [esi + 0x1220]
            mov dword ptr [eax], edi
            add eax, 4
            dec ecx
            jne L00442dc1
            mov ecx, dword ptr [esi + 8]
            mov dword ptr [esi + 0x1340], edx
            mov dword ptr [esi + 0x135c], edx
            cmp esi, edx
            mov dword ptr [ecx + 0x25c], edx
            je L00442dff
            cmp dword ptr [esi + 0xc], 8
            jne L00442dff
            call InitialiseExternalCamera
            mov edx, 0x64
            mov ecx, esi
            call PositionExternalCamera
    L00442dff:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00442e10
void C2_HOOK_FASTCALL FlipUpCollisionInfo(tPhysics_object* pCollision_info) {
    tPhysics_object *child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, v, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, omega, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, transform_matrix, 0x8c);

    for (child = pCollision_info->child; child != NULL; child = child->next) {
        int i;
        int j;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                child->transform_matrix.m[i][j] = pCollision_info->transform_matrix.m[i][j];
            }
        }
        BrVector3SetFloat(&child->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&child->omega, 0.f, 0.f, 0.f);
        FlipUpCollisionInfo(child);
    }
}

// FUNCTION: CARMA2_HW 0x00444710
void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {
    gRecovery_voucher_count = 0;
}

// FUNCTION: CARMA2_HW 0x00444910
void C2_HOOK_FASTCALL EnterUserMessage(void) {
    // GLOBAL: CARMA2_HW 0x00590f64
    static int last_key = -1;
    // GLOBAL: CARMA2_HW 0x0067c49c
    static int about_to_die;
    // GLOBAL: CARMA2_HW 0x0067c454
    static tU32 next_time;
    char* the_message;
    int len;
    int the_key;
    int abuse_num;

    if (!gEntering_message) {
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (!gCurrent_net_game->options.enable_text_messages) {
        return;
    }
    the_key = PDAnyKeyDown();
    if (gEntering_message == 1) {
        if (the_key != -1) {
            return;
        }
        gEntering_message = 2;
        PDClearKeyboardBuffer();
    }
    if (about_to_die) {
        if (the_key != -1) {
            return;
        }
        gEntering_message = 0;
        about_to_die = 0;
        return;
    }
    if (the_key == last_key) {
        if (next_time < PDGetTotalTime()) {
            next_time += 100;
        } else {
            the_key = -1000;
        }
    } else {
        last_key = the_key;
        next_time = PDGetTotalTime() + 500;
    }
    switch (the_key) {
    case -1:
    case 0:
        break;
    case 2:
    case 3:
    case 53:
    case 63:
        about_to_die = 1;
        break;
    case 50:
    case 65:
    case 70:
        len = strlen(&gString[20]);
        if (len > 0) {
            gString[20 + len - 1] = '\0';
        }
        break;
    case 51:
    case 52:
        len = strlen(gNet_players[gThis_net_player_index].player_name);
        if (len <= 18) {
            the_message = gString + 18 - len;
            strcpy(the_message, gNet_players[gThis_net_player_index].player_name);
            the_message[len + 0] = ':';
            the_message[len + 1] = ' ';
            gString[CARPOCALYPSE2_ASIZE(gString) - 1] = '\0';
            NetSendHeadupToAllPlayers(the_message);
            gString[20] = '\0';
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_message_sent));
            about_to_die = 1;
        }
        break;
    default:
        if (gKey_mapping[71] == the_key) {
            about_to_die = 1;
        } else if (the_key <= 74 || the_key >= 106) {
            for (;;) {
                int raw_ch = PDGetKeyboardCharacter();
                int ch;
                if (raw_ch == '\0') {
                    break;
                }
                ch = PDConvertToASCIILessThan128(raw_ch);
                len = strlen(&gString[20]);
                if (len < 64 - 1) {
                    gString[20 + len] = ch;
                    if (ch < gFonts[4].offset || ch >= gFonts[4].offset + gFonts[4].num_entries) {
                        gString[20 + len] = '\0';
                    }
                    gString[20 + len + 1] = '\0';
                    dr_dprintf("NETMESSAGE: '%s'", &gString[20]);
                }
            }
        } else if (the_key >= 81 && the_key <= 90) {
            if (the_key == 81) {
                abuse_num = 9;
            } else {
                abuse_num = the_key - 82;
            }
            if (gAbuse_text[abuse_num] != NULL) {
                strcpy(&gString[20], gAbuse_text[abuse_num]);
            }
        } else {
            gEntering_message = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00444270
void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period) {
    int left;
    int right;

    left = PDKeyDown(0);
    right = PDKeyDown(1);
    if (left || right) {
        CheckMapMoveKeys(left, right);
        gCamera_key_flags.field_0x0_bit1 = 0;
        gCamera_key_flags.field_0x0_bit2 = 0;
        gCamera_key_flags.field_0x0_bit3 = 0;
        gCamera_key_flags.field_0x0_bit4 = 0;
    } else {
        gCamera_key_flags.field_0x0_bit1 = KeyIsDownNoMouldiness(31);
        gCamera_key_flags.field_0x0_bit2 = KeyIsDownNoMouldiness(32);
        gCamera_key_flags.field_0x0_bit3 = KeyIsDownNoMouldiness(33);
        gCamera_key_flags.field_0x0_bit4 = KeyIsDownNoMouldiness(34);
        gCamera_key_flags.field_0x0_bit0 = PDKeyDown(2);
    }
}

void C2_HOOK_FASTCALL ExplodeCar(tCar_spec* pCar) {
    br_vector3 tv;
    br_vector3 v;
    br_vector3 pos;

    pCar->collision_info->field_0x49c = 0;
    BrVector3Set(&tv,
        pCar->collision_info->cmpos.v[0],
        pCar->collision_info->cmpos.v[1],
        pCar->collision_info->bb2.min.v[2] + .3f * (pCar->collision_info->bb2.max.v[2] - pCar->collision_info->bb2.min.v[2]));
    BrVector3Scale(&v, &pCar->collision_info->v, WORLD_SCALE);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    CreatePuffOfSmoke(&pos, &v, 1.f, 1.f, 7);

    tv.v[2] = pCar->collision_info->bb2.min.v[2] + .7f * (pCar->collision_info->bb2.max.v[2] - pCar->collision_info->bb2.min.v[2]);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    CreatePuffOfSmoke(&pos, &v, 1.f, 1.f, 7);
    if (pCar->car_crush_spec->field_0x4b8) {
        WeldCar(pCar);
    }
    DisableCar(pCar);
}

// FUNCTION: CARMA2_HW 0x00442500
void C2_NAKED C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gProgram_state.current_car.time_to_recover != 0) {
        if (gProgram_state.current_car.knackered) {
            gProgram_state.current_car.time_to_recover = 0;
        } else {
            int time;
            char str[256];

            time = (gProgram_state.current_car.time_to_recover - pEndFrameTime + 1000) / 1000;
            sprintf(str, "%s %d %s",
                GetMiscString(eMiscString_recovery_in),
                time,
                GetMiscString(time > 1 ? eMiscString_seconds : eMiscString_second));
            if (!gToo_late && gCancel_recover) {
                NewTextHeadupSlot2(4, 0, 2000, -1, str, 0);
            }
            if (pEndFrameTime >= gProgram_state.current_car.time_to_recover) {
                RecoverCar();
            }
        }
    }
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {
            if (i != gThis_net_player_index && gNet_players[i].car->time_to_recover != 0 && pEndFrameTime >= gNet_players[i].car->time_to_recover) {
                gNet_players[i].player_status = ePlayer_status_recovering;
                gNet_players[i].car->collision_info->field_0x261 = 0xff;
                gNet_players[i].car->collision_info->message_time = pEndFrameTime;
                ExplodeCar(gNet_players[i].car);
                gNet_players[i].car->time_to_recover = 0;
            }
        }
    }
#else
    __asm {
            sub esp, 0x128
            push ebx
            push ebp
            mov ebp, ecx
            xor ebx, ebx
            mov ecx, dword ptr [gProgram_state+0x194c]
            push esi
            cmp ecx, ebx
            push edi
            je L004425c1
            cmp dword ptr [gProgram_state+0x280], ebx
            jne L004425bb
            sub ecx, ebp
            mov eax, 0x10624dd3
            add ecx, 0x3e8
            mul ecx
            mov esi, edx
            mov ecx, 0x63
            shr esi, 6
            cmp esi, 1
            jg L0044254b
            mov ecx, 0x62
    L0044254b:
            call GetMiscString
            mov ecx, 0x61
            push eax
            push esi
            call GetMiscString
            push eax
            lea eax, [esp + 0x44]
            push offset gCancel_recover+0xb0
            push eax
            call sprintf
            mov eax, dword ptr [gToo_late]
            add esp, 0x14
            cmp eax, ebx
            jne L00442599
            cmp dword ptr [gCancel_recover], ebx
            je L00442599
            lea ecx, [esp + 0x38]
            push ebx
            push ecx
            push -4
            push 0x7d0
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot2
    L00442599:
            cmp dword ptr [gProgram_state+0x194c], ebp
            ja L004425c1
            cmp dword ptr [gNet_mode], ebx
            je L004425b1
            cmp dword ptr [gPalette_fade_time], ebx
            jne L004425bb
    L004425b1:
            mov dword ptr [gRecover_car], 1
    L004425bb:
            mov dword ptr [gProgram_state+0x194c], ebx
    L004425c1:
            cmp dword ptr [gNet_mode], ebx
            je L00442742
            mov eax, dword ptr [gNumber_of_net_players]
            xor ecx, ecx
            cmp eax, ebx
            mov dword ptr [esp + 0x10], ecx
            jle L00442742
            mov edi, offset gNet_players+0xc8
    L004425e5:
            cmp ecx, dword ptr [gThis_net_player_index]
            je L0044272a
            mov edx, dword ptr [edi]
            mov eax, dword ptr [edx + 0x18a0]
            cmp eax, ebx
            je L0044272a
            cmp eax, ebp
            ja L0044272a
            mov dword ptr [edi - 0x80], 8
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [eax + 8]
            mov byte ptr [ecx + 0x261], 0xff
            mov edx, dword ptr [edi]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax + 0x268], ebp
            mov esi, dword ptr [edi]
            mov ecx, dword ptr [esi + 8]
            mov dword ptr [ecx + 0x49c], ebx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [eax + 0x14]
            mov dword ptr [esp + 0x14], edx
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [esp + 0x18], ecx
            mov edx, dword ptr [eax + 0x1c]
            mov dword ptr [esp + 0x1c], edx
            lea ecx, [esp + 0x14]
            fld dword ptr [eax + 0x68]
            fmul qword ptr [gFloat_00589c40+0x20]
            fstp dword ptr [esp + 0x20]
            fld dword ptr [eax + 0x6c]
            fmul qword ptr [gFloat_00589c40+0x20]
            fstp dword ptr [esp + 0x24]
            fld dword ptr [eax + 0x70]
            fmul qword ptr [gFloat_00589c40+0x20]
            fstp dword ptr [esp + 0x28]
            fld dword ptr [eax + 0x44]
            fld dword ptr [eax + 0x50]
            fsub dword ptr [eax + 0x44]
            mov eax, dword ptr [esi + 0x10]
            lea edx, [esp + 0x2c]
            add eax, 0x2c
            fmul qword ptr [gFloat_00589c40+0x28]
            push eax
            push ecx
            push edx
            fsubp st(1), st(0)
            fstp dword ptr [esp + 0x28]
            call BrMatrix34ApplyP
            add esp, 0xc
            lea edx, [esp + 0x20]
            lea ecx, [esp + 0x2c]
            push 7
            push 0x3f800000
            push 0x3f800000
            call CreatePuffOfSmoke
            mov eax, dword ptr [esi + 8]
            lea ecx, [esp + 0x14]
            lea edx, [esp + 0x2c]
            fld dword ptr [eax + 0x44]
            fld dword ptr [eax + 0x50]
            fsub dword ptr [eax + 0x44]
            mov eax, dword ptr [esi + 0x10]
            add eax, 0x2c
            fmul qword ptr [gFloat_00589c40+0x30]
            push eax
            push ecx
            push edx
            fsubp st(1), st(0)
            fstp dword ptr [esp + 0x28]
            call BrMatrix34ApplyP
            add esp, 0xc
            lea edx, [esp + 0x20]
            lea ecx, [esp + 0x2c]
            push 7
            push 0x3f800000
            push 0x3f800000
            call CreatePuffOfSmoke
            mov eax, dword ptr [esi + 0x18d4]
            cmp byte ptr [eax + 0x4b8], bl
            je L00442717
            mov ecx, esi
            call WeldCar
    L00442717:
            mov ecx, esi
            call DisableCar
            mov ecx, dword ptr [edi]
            mov dword ptr [ecx + 0x18a0], ebx
            mov ecx, dword ptr [esp + 0x10]
    L0044272a:
            mov eax, dword ptr [gNumber_of_net_players]
            inc ecx
            add edi, 0xd4
            cmp ecx, eax
            mov dword ptr [esp + 0x10], ecx
            jl L004425e5
    L00442742:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x128
            ret
    }
#endif
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL BrakeInstantly(void) {
    int i;
    br_scalar speed_squared;

    gProgram_state.current_car.revs = 0.f;
    speed_squared = BrVector3LengthSquared(&gProgram_state.current_car.collision_info->v);
    if (gProgram_state.current_car.number_of_wheels_on_ground != 0 && speed_squared > .0001f) {
        PratcamEvent(41);
        for (i = 0; i < 5; i++) {
            DRS3StartSound(gXXX_outlet, eSoundId_TyreScreeching + i);
        }
    }
    if (speed_squared > .1f) {
        PDPlayFFBEffect("Instant_Handbrake");
    }
    BrVector3Set(&gProgram_state.current_car.collision_info->v, 0.f, 0.f, 0.f);
}

// FUNCTION: CARMA2_HW 0x00443e80
void C2_NAKED C2_HOOK_FASTCALL PollCarControls(tU32 pTime_difference) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_controls keys;
    tJoystick joystick;
    tCar_spec* c;

    c = &gProgram_state.current_car;
    memset(&keys, 0, sizeof(tCar_controls));
    joystick.left = -1;
    joystick.right = -1;
    joystick.acc = -1;
    joystick.dec = -1;
    CheckKeysForMouldiness();
    if (gEntering_message) {
        c->keys = keys;
        c->joystick = joystick;
        return;
    }
    if (gKey_mapping[47] < 143 && gKey_mapping[48] < 143) {
        if (KeyIsDownNoMouldiness(47)) {
            keys.left = 1;
        }
        if (KeyIsDownNoMouldiness(48)) {
            keys.right = 1;
        }
    }
    if (!(keys.left || keys.right)) {
        if (PDIsJoystickDPadEnabled()) {
            if (gJoy_array[0] > 10) {
                keys.left = 1;
            }
            if (gJoy_array[1] > 10) {
                keys.right = 1;
            }
        } else {
            joystick.left = gJoy_array[0];
            joystick.right = gJoy_array[1];
            if (gJoy_array[0] < 0 && gJoy_array[1] < 0) {
                joystick.left = 0;
            }
        }
    }
    if (KeyIsDownNoMouldiness(13)) {
        keys.holdw = 1;
    }
    if (KeyIsDownNoMouldiness(54) || gRace_finished) {
        if (gInstant_handbrake && !gRace_finished) {
            BrakeInstantly();
        } else {
            keys.brake = 1;
        }
    }
    if (KeyIsDownNoMouldiness(49) && !gRace_finished && !c->knackered && !gWait_for_it) {
        keys.acc = 1;
    }
    if (!keys.acc) {
        if (PDIsJoystickDPadEnabled()) {
            if (gJoy_array[2] > 10) {
                keys.acc = 1;
            }
        } else {
            if (HasCurrentJoystick()) {
                joystick.acc = gJoy_array[2];
            }
        }
    }
    if (KeyIsDownNoMouldiness(50) && !gRace_finished && !c->knackered && !gWait_for_it) {
        keys.dec = 1;
    }
    if (!keys.dec) {
        if (PDIsJoystickDPadEnabled()) {
            if (gJoy_array[3] > 10) {
                keys.dec = 1;
            }
        } else {
            if (HasCurrentJoystick()) {
                joystick.dec = gJoy_array[3];
            }
        }
    }

    if (gCredit_multiplier) {

        int temp;

        temp = joystick.acc;
        joystick.acc = joystick.dec;
        joystick.dec = temp;

        temp = joystick.left;
        joystick.left = joystick.right;
        joystick.right = temp;

        {
            int acc = keys.acc;
            int dec = keys.dec;
            keys.dec = acc;
            keys.acc = dec;
        }

        {
            int l = keys.left;
            int r = keys.right;
            keys.left = r;
            keys.right = l;
        }
    }

    if (KeyIsDownNoMouldiness(56) && c->gear >= 0) {
        keys.change_down = 1;
        c->just_changed_gear = 1;
        if (keys.acc || joystick.acc > 32000) {
            c->traction_control = 0;
        } else if (c->gear > 1) {
            c->gear -= !c->keys.change_down;
        }
        if (gCountdown != 0 && !c->keys.change_down) {
            JumpTheStart();
        }
    }
    if (gCar_flying) {
        if (KeyIsDownNoMouldiness(14)) {
            keys.up = 1;
        }
        if (KeyIsDownNoMouldiness(12)) {
            keys.down = 1;
        }
    }
    if (KeyIsDownNoMouldiness(59)) {
        if (!gEntering_message) {
            keys.horn = 1;
        }
    }
    c->keys = keys;
    c->joystick = joystick;
#else
    __asm {
            sub esp, 0x14
            push ebx
            push ebp
            push esi
            push edi
            or edi, 0xffffffff
            xor esi, esi
            mov ebx, edi
            mov ebp, edi
            mov dword ptr [esp + 0x1c], edi
            mov dword ptr [esp + 0x20], edi
            call CheckKeysForMouldiness
            mov eax, dword ptr [gEntering_message]
            test eax, eax
            je L00443eb1
            mov dword ptr [gProgram_state+0x137c], esi
            jmp L00444244
    L00443eb1:
            mov ecx, dword ptr [gKey_mapping+0xbc]
            mov eax, 0x8f
            cmp ecx, eax
            jge L00443eef
            cmp dword ptr [gKey_mapping+0xc0], eax
            jge L00443eef
            mov ecx, 0x2f
            call KeyIsDownNoMouldiness
            test eax, eax
            je L00443edb
            mov esi, 0x10000
    L00443edb:
            mov ecx, 0x30
            call KeyIsDownNoMouldiness
            test eax, eax
            je L00443eef
            or esi, 0x20000
    L00443eef:
            test esi, 0x30000
            jne L00443f3b
            call PDIsJoystickDPadEnabled
            test eax, eax
            je L00443f25
            mov ecx, dword ptr [gJoy_array]
            mov eax, 0xa
            cmp ecx, eax
            jle L00443f15
            or esi, 0x10000
    L00443f15:
            cmp dword ptr [gJoy_array+0x4], eax
            jle L00443f3b
            or esi, 0x20000
            jmp L00443f3b
    L00443f25:
            mov ebx, dword ptr [gJoy_array]
            mov ebp, dword ptr [gJoy_array+0x4]
            test ebx, ebx
            jge L00443f3b
            test ebp, ebp
            jge L00443f3b
            xor ebx, ebx
    L00443f3b:
            mov ecx, 0xd
            call KeyIsDownNoMouldiness
            test eax, eax
            je L00443f4f
            or esi, 0x800000
    L00443f4f:
            mov ecx, 0x36
            call KeyIsDownNoMouldiness
            test eax, eax
            mov eax, dword ptr [gRace_finished]
            jne L00443f6a
            test eax, eax
            je L00444047
    L00443f6a:
            mov ecx, dword ptr [gInstant_handbrake]
            test ecx, ecx
            je L00444041
            test eax, eax
            jne L00444041
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov dword ptr [gProgram_state+0x13ec], eax
            mov eax, dword ptr [gProgram_state+0x1394]
            fld dword ptr [ecx + 0x68]
            fld dword ptr [ecx + 0x6c]
            fld dword ptr [ecx + 0x70]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            test eax, eax
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            je L00444007
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFloat_00589c40+0x50]
            fnstsw ax
            test ah, 0x41
            jne L00444007
            mov ecx, 0x29
            call PratcamEvent
            xor edi, edi
    L00443fe6:
            mov ecx, dword ptr [gXXX_outlet]
            lea edx, [edi + 0x2328]
            call DRS3StartSound
            inc edi
            cmp edi, 5
            jl L00443fe6
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov edi, dword ptr [esp + 0x1c]
    L00444007:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFloat_00589c40+0x54]
            fnstsw ax
            test ah, 0x41
            jne L00444028
            mov ecx, offset gCancel_recover+0xc0
            call PDPlayFFBEffect
            mov ecx, dword ptr [gProgram_state+0xb4]
    L00444028:
            xor eax, eax
            mov dword ptr [ecx + 0x68], eax
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov dword ptr [ecx + 0x6c], eax
            mov edx, dword ptr [gProgram_state+0xb4]
            mov dword ptr [edx + 0x70], eax
            jmp L00444047
    L00444041:
            or esi, 0x100000
    L00444047:
            mov ecx, 0x31
            call KeyIsDownNoMouldiness
            test eax, eax
            je L00444076
            mov eax, dword ptr [gRace_finished]
            test eax, eax
            jne L00444076
            mov eax, dword ptr [gProgram_state+0x280]
            test eax, eax
            jne L00444076
            mov eax, dword ptr [gWait_for_it]
            test eax, eax
            jne L00444076
            or esi, 0x40000
    L00444076:
            test esi, 0x40000
            jne L004440ac
            call PDIsJoystickDPadEnabled
            test eax, eax
            je L00444098
            cmp dword ptr [gJoy_array+0x8], 0xa
            jle L004440ac
            or esi, 0x40000
            jmp L004440ac
    L00444098:
            call HasCurrentJoystick
            test eax, eax
            je L004440ac
            mov eax, dword ptr [gJoy_array+0x8]
            mov dword ptr [esp + 0x1c], eax
            mov edi, eax
    L004440ac:
            mov ecx, 0x32
            call KeyIsDownNoMouldiness
            test eax, eax
            je L004440db
            mov eax, dword ptr [gRace_finished]
            test eax, eax
            jne L004440db
            mov eax, dword ptr [gProgram_state+0x280]
            test eax, eax
            jne L004440db
            mov eax, dword ptr [gWait_for_it]
            test eax, eax
            jne L004440db
            or esi, 0x80000
    L004440db:
            test esi, 0x80000
            jne L00444110
            call PDIsJoystickDPadEnabled
            test eax, eax
            je L004440fd
            cmp dword ptr [gJoy_array+0xc], 0xa
            jle L00444110
            or esi, 0x80000
            jmp L00444110
    L004440fd:
            call HasCurrentJoystick
            test eax, eax
            je L00444110
            mov ecx, dword ptr [gJoy_array+0xc]
            mov dword ptr [esp + 0x20], ecx
    L00444110:
            mov eax, dword ptr [gCredit_multiplier]
            test eax, eax
            je L0044417b
            mov eax, dword ptr [esp + 0x20]
            mov ecx, esi
            mov dword ptr [esp + 0x1c], eax
            mov eax, ebx
            mov ebx, ebp
            mov ebp, eax
            mov eax, esi
            and esi, 0x40000
            shr eax, 0x13
            and eax, 1
            and ecx, 0xfff7ffff
            shl esi, 1
            or ecx, esi
            and eax, 1
            and ecx, 0xfffbffff
            mov dword ptr [esp + 0x20], edi
            shl eax, 0x12
            or ecx, eax
            mov edi, dword ptr [esp + 0x1c]
            mov eax, ecx
            mov edx, ecx
            shr eax, 0x10
            and eax, 1
            and edx, 0xfffcffff
            and eax, 1
            shl eax, 0x11
            shr ecx, 1
            or edx, eax
            and ecx, 0x10000
            or edx, ecx
            mov esi, edx
    L0044417b:
            mov ecx, 0x38
            call KeyIsDownNoMouldiness
            test eax, eax
            je L004441f0
            mov eax, dword ptr [gProgram_state+0x1408]
            test eax, eax
            jl L004441f0
            mov ecx, dword ptr [gProgram_state+0x137c]
            or esi, 0x4000000
            test esi, 0x40000
            mov dword ptr [gProgram_state+0x140c], 1
            jne L004441d0
            cmp edi, 0x7d00
            jg L004441d0
            cmp eax, 1
            jle L004441da
            mov edx, ecx
            not edx
            shr edx, 0x1a
            and edx, 1
            sub eax, edx
            mov dword ptr [gProgram_state+0x1408], eax
            jmp L004441da
    L004441d0:
            mov dword ptr [gProgram_state+0x1370], 0
    L004441da:
            mov eax, dword ptr [gCountdown]
            test eax, eax
            je L004441f0
            test ecx, 0x4000000
            jne L004441f0
            call JumpTheStart
    L004441f0:
            mov eax, dword ptr [gCar_flying]
            test eax, eax
            je L00444221
            mov ecx, 0xe
            call KeyIsDownNoMouldiness
            test eax, eax
            je L0044420d
            or esi, 0x200000
    L0044420d:
            mov ecx, 0xc
            call KeyIsDownNoMouldiness
            test eax, eax
            je L00444221
            or esi, 0x400000
    L00444221:
            mov ecx, 0x3b
            call KeyIsDownNoMouldiness
            test eax, eax
            je L0044423e
            mov eax, dword ptr [gEntering_message]
            test eax, eax
            jne L0044423e
            or esi, 0x8000000
    L0044423e:
            mov dword ptr [gProgram_state+0x137c], esi
    L00444244:
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [gProgram_state+0x1388], edi
            pop edi
            mov dword ptr [gProgram_state+0x1384], ebp
            pop esi
            mov dword ptr [gProgram_state+0x1380], ebx
            pop ebp
            mov dword ptr [gProgram_state+0x138c], eax
            pop ebx
            add esp, 0x14
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045a670
void C2_NAKED C2_HOOK_FASTCALL FUN_0045a670(void) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            mov eax, dword ptr [gINT_0068b8e4]
            sub esp, 0x118
            push ebx
            xor ebx, ebx
            push esi
            cmp eax, ebx
            push edi
            je L0045a6bc
            mov eax, dword ptr [gAcc_poly_actor]
            mov dword ptr [gINT_0068b8e4], ebx
            cmp eax, ebx
            je L0045a7e1
            cmp dword ptr [gRender_acc_poly_actor], ebx
            je L0045a7e1
            push eax
            call BrActorRemove
            add esp, 4
            mov dword ptr [gRender_acc_poly_actor], ebx
            pop edi
            pop esi
            pop ebx
            add esp, 0x118
            ret
    L0045a6bc:
            mov ecx, dword ptr [gProgram_state+0x64]
            push 0x15e
            mov edx, 0xa
            mov dword ptr [gINT_0068b8e4], 1
            call FUN_0044a8a0
            mov edi, offset gRace_head_ups
            mov esi, 0x78
    L0045a6e5:
            mov ecx, dword ptr [edi]
            push esi
            mov edx, 0xa
            call FUN_0044a8a0
            mov ecx, dword ptr [edi]
            mov edx, 0xfffffffe
            call ChangeHeadupFont
            add esi, 0xa
            add edi, 4
            cmp esi, 0x154
            jl L0045a6e5
            push 0x43600000
            push 0x43020000
            push 0x42ec0000
            push 0x41000000
            call CreateAccentPolyActor
            cmp dword ptr [gHeadup_text_buffer_0079d860+0x8], ebx
            jne L0045a7e1
            push offset gDisable_sound+0x8
            mov edx, offset gRace_path
            lea ecx, [esp + 0x28]
            call PathCat
            cmp dword ptr [gHeadup_text_buffer_0079d860+0x14], 5
            jge L0045a757
            mov dword ptr [gHeadup_text_buffer_0079d860+0x14], 0xa
    L0045a757:
            mov edi, offset gDefault_drone_direction+0x6dc
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0xc]
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gHeadup_text_buffer_0079d860], ebx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov dword ptr [gHeadup_text_buffer_0079d860+0x8], ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov dword ptr [gHeadup_text_buffer_0079d860+0xc], ebx
            and ecx, 3
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], ebx
            rep movsb
            lea ecx, [esp + 0xc]
            mov dword ptr [gHeadup_text_buffer_0079d860+0x4], ebx
            mov dword ptr [gHeadup_text_buffer_0079d860+0x10], 0x16
            mov dword ptr [esp + 0x18], 0x79d860
            call DRstrlwr
            lea ecx, [esp + 0xc]
            lea edx, [esp + 0x18]
            mov dword ptr [esp + 0x1c], ecx
            push edx
            mov edx, offset FUN_0045a7f0
            lea ecx, [esp + 0x28]
            call PDEnumPath
            mov eax, dword ptr [gHeadup_text_buffer_0079d860]
            mov dword ptr [gHeadup_text_buffer_0079d860+0xc], ebx
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], eax
    L0045a7e1:
            pop edi
            pop esi
            pop ebx
            add esp, 0x118
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045acf0
void C2_NAKED C2_HOOK_FASTCALL FUN_0045acf0(void) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            mov eax, dword ptr [gINT_0068b8e4]
            push ebx
            xor ebx, ebx
            push esi
            cmp eax, ebx
            push edi
            je L0045ad30
            mov eax, dword ptr [gAcc_poly_actor]
            mov dword ptr [gINT_0068b8e4], ebx
            cmp eax, ebx
            je L0045ae12
            cmp dword ptr [gRender_acc_poly_actor], ebx
            je L0045ae12
            push eax
            call BrActorRemove
            add esp, 4
            mov dword ptr [gRender_acc_poly_actor], ebx
            pop edi
            pop esi
            pop ebx
            ret
    L0045ad30:
            mov ecx, dword ptr [gProgram_state+0x64]
            push 0x15e
            mov edx, 0xa
            mov dword ptr [gINT_0068b8e4], 2
            call FUN_0044a8a0
            mov edi, offset gRace_head_ups
            mov esi, 0x78
    L0045ad59:
            mov ecx, dword ptr [edi]
            push esi
            mov edx, 0xa
            call FUN_0044a8a0
            mov ecx, dword ptr [edi]
            mov edx, 0xfffffffe
            call ChangeHeadupFont
            add esi, 0xa
            add edi, 4
            cmp esi, 0x154
            jl L0045ad59
            push 0x43600000
            push 0x43020000
            push 0x42ec0000
            push 0x41000000
            call CreateAccentPolyActor
            cmp dword ptr [gHeadup_text_buffer_0079d880+0x8], ebx
            jne L0045ae12
            cmp dword ptr [gHeadup_text_buffer_0079d880+0x14], 5
            jge L0045adb4
            mov dword ptr [gHeadup_text_buffer_0079d880+0x14], 0xa
    L0045adb4:
            mov dword ptr [gHeadup_text_buffer_0079d880], ebx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x8], ebx
            mov dword ptr [gHeadup_text_buffer_0079d880+0xc], ebx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], ebx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x4], ebx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x10], 0x16
            xor edi, edi
    L0045adde:
            push 0x3c
            call malloc
            mov esi, eax
            add esp, 4
            cmp esi, ebx
            je L0045ae0c
            push edi
            push offset gUNK_00596310+0x128
            push esi
            mov dword ptr [esi], ebx
            call sprintf
            add esp, 0xc
            mov edx, esi
            mov ecx, offset gHeadup_text_buffer_0079d880
            push ebx
            call FUN_0045a920
    L0045ae0c:
            inc edi
            cmp edi, 0xa
            jl L0045adde
    L0045ae12:
            pop edi
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045ae20
void C2_NAKED C2_HOOK_FASTCALL FUN_0045ae20(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 start_time;
    int i;
    int j;
    int font;
    tHeadup_text* text;
    tHeadup_text* current;
    int* headup;

    if (gINT_0068b8e4 == 0) {
        return;
    }
    start_time = PDGetTotalTime();
    if (PDKeyDown(0x42)) {
        if (&gHeadup_text_buffer_0079d860 != NULL
                && gHeadup_text_buffer_0079d860.text_last_add != NULL
                && gHeadup_text_buffer_0079d860.text_last_add != gHeadup_text_buffer_0079d860.texts) {
            gHeadup_text_buffer_0079d860.text_last_add = gHeadup_text_buffer_0079d860.texts;
            gHeadup_text_buffer_0079d860.pos_last_add--;
            i = gHeadup_text_buffer_0079d860.pos_last_add;
            if (i > 0) {
                do {
                    gHeadup_text_buffer_0079d860.text_last_add = gHeadup_text_buffer_0079d860.text_last_add->next;
                    i--;
                } while (i != 0);
            }
        }
        do {
            i = PDGetTotalTime();
        } while ((tU32)(i - start_time) < 100U);
    }
    if (PDKeyDown(0x43)) {
        if (&gHeadup_text_buffer_0079d860 != NULL
                && gHeadup_text_buffer_0079d860.text_last_add != NULL
                && gHeadup_text_buffer_0079d860.text_last_add->next != NULL) {
            gHeadup_text_buffer_0079d860.text_last_add = gHeadup_text_buffer_0079d860.text_last_add->next;
            gHeadup_text_buffer_0079d860.pos_last_add++;
        }
        do {
            i = PDGetTotalTime();
        } while ((tU32)(i - start_time) < 100U);
    }
    if (PDKeyDown(0x44)) {
        i = gHeadup_text_buffer_0079d860.field_0x10 - 1;
        if (i > 0) {
            do {
                if (&gHeadup_text_buffer_0079d860 != NULL
                        && gHeadup_text_buffer_0079d860.text_last_add != NULL
                        && gHeadup_text_buffer_0079d860.text_last_add != gHeadup_text_buffer_0079d860.texts) {
                    gHeadup_text_buffer_0079d860.pos_last_add--;
                    gHeadup_text_buffer_0079d860.text_last_add = gHeadup_text_buffer_0079d860.texts;
                    j = gHeadup_text_buffer_0079d860.pos_last_add;
                    if (j > 0) {
                        do {
                            gHeadup_text_buffer_0079d860.text_last_add = gHeadup_text_buffer_0079d860.text_last_add->next;
                            j--;
                        } while (j != 0);
                    }
                }
                i--;
            } while (i != 0);
        }
        while (PDKeyDown(0x44)) {
        }
    }
    if (PDKeyDown(0x45)) {
        i = gHeadup_text_buffer_0079d860.field_0x10 - 1;
        current = gHeadup_text_buffer_0079d860.text_last_add;
        if (i > 0) {
            do {
                if (&gHeadup_text_buffer_0079d860 != NULL && current != NULL && current->next != NULL) {
                    current = current->next;
                    gHeadup_text_buffer_0079d860.text_last_add = current;
                    gHeadup_text_buffer_0079d860.pos_last_add++;
                }
                i--;
            } while (i != 0);
        }
        while (PDKeyDown(0x45)) {
        }
    }
    if (PDKeyDown(0x40)) {
        gSelected_actor_name = (const char*)gHeadup_text_buffer_0079d860.text_last_add;
        DropActor(0, 0);
        gSelected_actor_name = NULL;
        while (PDKeyDown(0x40)) {
        }
    }
    if (PDKeyDown(0x41)) {
        MurderLastActor();
        while (PDKeyDown(0x41)) {
        }
    }
    AdjustHeadupTextBuffer(&gHeadup_text_buffer_0079d860);
    text = gHeadup_text_buffer_0079d860.field_0x4;
    headup = gRace_head_ups;
    do {
        if (text != NULL) {
            font = -1;
            if (text != gHeadup_text_buffer_0079d860.text_last_add) {
                font = -2;
            }
            ChangeHeadupFont(*headup, font);
            ChangeHeadupText(*headup, text->text);
            text = text->next;
        } else {
            ChangeHeadupText(*headup, "");
        }
        headup++;
    } while ((int)headup < (int)&gRace_head_ups[22]);
#else
    __asm {
            mov eax, dword ptr [gINT_0068b8e4]
            test eax, eax
            je L0045b092
            push edi
            push esi
            call PDGetTotalTime
            mov ecx, 0x42
            mov esi, eax
            call PDKeyDown
            test eax, eax
            je L0045ae94
            mov eax, offset gHeadup_text_buffer_0079d860
            test eax, eax
            je L0045ae81
            mov ecx, dword ptr [gHeadup_text_buffer_0079d860+0x18]
            test ecx, ecx
            je L0045ae81
            mov eax, dword ptr [gHeadup_text_buffer_0079d860]
            cmp ecx, eax
            je L0045ae81
            mov ecx, dword ptr [gHeadup_text_buffer_0079d860+0xc]
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], eax
            dec ecx
            test ecx, ecx
            mov dword ptr [gHeadup_text_buffer_0079d860+0xc], ecx
            jle L0045ae81
    L0045ae76:
            mov eax, dword ptr [eax + 0x38]
            dec ecx
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], eax
            jne L0045ae76
    L0045ae81:
            xor cl, cl
            cmp cl, 0x64
            jae L0045ae94
    L0045ae88:
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x64
            jb L0045ae88
    L0045ae94:
            mov ecx, 0x43
            call PDKeyDown
            test eax, eax
            je L0045aedd
            mov edx, offset gHeadup_text_buffer_0079d860
            test edx, edx
            je L0045aecb
            mov eax, dword ptr [gHeadup_text_buffer_0079d860+0x18]
            test eax, eax
            je L0045aecb
            mov eax, dword ptr [eax + 0x38]
            test eax, eax
            je L0045aecb
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], eax
            mov eax, dword ptr [gHeadup_text_buffer_0079d860+0xc]
            inc eax
            mov dword ptr [gHeadup_text_buffer_0079d860+0xc], eax
    L0045aecb:
            xor al, al
            cmp al, 0x64
            jae L0045aedd
    L0045aed1:
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x64
            jb L0045aed1
    L0045aedd:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            je L0045af5a
            mov ecx, dword ptr [gHeadup_text_buffer_0079d860+0x10]
            lea eax, [ecx - 1]
            test eax, eax
            jle L0045af3e
            mov esi, dword ptr [gHeadup_text_buffer_0079d860]
            mov edx, dword ptr [gHeadup_text_buffer_0079d860+0xc]
            mov edi, eax
            mov eax, dword ptr [gHeadup_text_buffer_0079d860+0x18]
    L0045af0b:
            mov ecx, offset gHeadup_text_buffer_0079d860
            test ecx, ecx
            je L0045af3b
            test eax, eax
            je L0045af3b
            cmp eax, esi
            je L0045af3b
            dec edx
            mov eax, esi
            test edx, edx
            mov dword ptr [gHeadup_text_buffer_0079d860+0xc], edx
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], eax
            jle L0045af3b
            mov ecx, edx
    L0045af30:
            mov eax, dword ptr [eax + 0x38]
            dec ecx
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], eax
            jne L0045af30
    L0045af3b:
            dec edi
            jne L0045af0b
    L0045af3e:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            je L0045af5a
    L0045af4c:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            jne L0045af4c
    L0045af5a:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            je L0045afc3
            mov edx, dword ptr [gHeadup_text_buffer_0079d860+0x10]
            lea eax, [edx - 1]
            test eax, eax
            jle L0045afa7
            mov ecx, dword ptr [gHeadup_text_buffer_0079d860+0x18]
            mov edx, eax
    L0045af7d:
            mov eax, offset gHeadup_text_buffer_0079d860
            test eax, eax
            je L0045afa4
            test ecx, ecx
            je L0045afa4
            mov eax, dword ptr [ecx + 0x38]
            test eax, eax
            je L0045afa4
            mov ecx, eax
            mov eax, dword ptr [gHeadup_text_buffer_0079d860+0xc]
            inc eax
            mov dword ptr [gHeadup_text_buffer_0079d860+0x18], ecx
            mov dword ptr [gHeadup_text_buffer_0079d860+0xc], eax
    L0045afa4:
            dec edx
            jne L0045af7d
    L0045afa7:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            je L0045afc3
    L0045afb5:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            jne L0045afb5
    L0045afc3:
            mov ecx, 0x40
            call PDKeyDown
            test eax, eax
            je L0045b00a
            mov eax, dword ptr [gHeadup_text_buffer_0079d860+0x18]
            xor edx, edx
            xor ecx, ecx
            mov dword ptr [gSelected_actor_name], eax
            call DropActor
            mov ecx, 0x40
            mov dword ptr [gSelected_actor_name], 0
            call PDKeyDown
            test eax, eax
            je L0045b00a
    L0045affc:
            mov ecx, 0x40
            call PDKeyDown
            test eax, eax
            jne L0045affc
    L0045b00a:
            mov ecx, 0x41
            call PDKeyDown
            test eax, eax
            je L0045b039
            call MurderLastActor
            mov ecx, 0x41
            call PDKeyDown
            test eax, eax
            je L0045b039
    L0045b02b:
            mov ecx, 0x41
            call PDKeyDown
            test eax, eax
            jne L0045b02b
    L0045b039:
            mov ecx, offset gHeadup_text_buffer_0079d860
            call AdjustHeadupTextBuffer
            mov edi, dword ptr [gHeadup_text_buffer_0079d860+0x4]
            mov esi, offset gRace_head_ups
    L0045b04e:
            test edi, edi
            je L0045b079
            cmp edi, dword ptr [gHeadup_text_buffer_0079d860+0x18]
            jne L0045b05f
            or edx, 0xffffffff
            jmp L0045b064
    L0045b05f:
            mov edx, 0xfffffffe
    L0045b064:
            mov ecx, dword ptr [esi]
            call ChangeHeadupFont
            mov ecx, dword ptr [esi]
            mov edx, edi
            call ChangeHeadupText
            mov edi, dword ptr [edi + 0x38]
            jmp L0045b085
    L0045b079:
            mov ecx, dword ptr [esi]
            mov edx, offset gDisable_sound+0x8
            call ChangeHeadupText
    L0045b085:
            add esi, 4
            cmp esi, offset gRace_head_ups+0x58
            jl L0045b04e
            pop esi
            pop edi
    L0045b092:
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045b150
void C2_NAKED C2_HOOK_FASTCALL FUN_0045b150(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 start_time;
    tHeadup_text* text;
    tHeadup_text* current;
    int* headup;
    int i;
    int j;
    int font;

    if (gINT_0068b8e4 == 0) {
        return;
    }
    start_time = PDGetTotalTime();
    if (PDKeyDown(0x42)) {
        if (&gHeadup_text_buffer_0079d880 != NULL
                && gHeadup_text_buffer_0079d880.text_last_add != NULL
                && gHeadup_text_buffer_0079d880.text_last_add != gHeadup_text_buffer_0079d880.texts) {
            gHeadup_text_buffer_0079d880.text_last_add = gHeadup_text_buffer_0079d880.texts;
            gHeadup_text_buffer_0079d880.pos_last_add--;
            i = gHeadup_text_buffer_0079d880.pos_last_add;
            if (i > 0) {
                do {
                    gHeadup_text_buffer_0079d880.text_last_add = gHeadup_text_buffer_0079d880.text_last_add->next;
                    i--;
                } while (i != 0);
            }
        }
        do {
            i = PDGetTotalTime();
        } while ((tU32)(i - start_time) < 100U);
    }
    if (PDKeyDown(0x43)) {
        if (&gHeadup_text_buffer_0079d880 != NULL
                && gHeadup_text_buffer_0079d880.text_last_add != NULL
                && gHeadup_text_buffer_0079d880.text_last_add->next != NULL) {
            gHeadup_text_buffer_0079d880.text_last_add = gHeadup_text_buffer_0079d880.text_last_add->next;
            gHeadup_text_buffer_0079d880.pos_last_add++;
        }
        do {
            i = PDGetTotalTime();
        } while ((tU32)(i - start_time) < 100U);
    }
    if (PDKeyDown(0x44)) {
        i = gHeadup_text_buffer_0079d880.field_0x10 - 1;
        if (i > 0) {
            do {
                if (&gHeadup_text_buffer_0079d880 != NULL
                        && gHeadup_text_buffer_0079d880.text_last_add != NULL
                        && gHeadup_text_buffer_0079d880.text_last_add != gHeadup_text_buffer_0079d880.texts) {
                    gHeadup_text_buffer_0079d880.pos_last_add--;
                    gHeadup_text_buffer_0079d880.text_last_add = gHeadup_text_buffer_0079d880.texts;
                    j = gHeadup_text_buffer_0079d880.pos_last_add;
                    if (j > 0) {
                        do {
                            gHeadup_text_buffer_0079d880.text_last_add = gHeadup_text_buffer_0079d880.text_last_add->next;
                            j--;
                        } while (j != 0);
                    }
                }
                i--;
            } while (i != 0);
        }
        while (PDKeyDown(0x44)) {
        }
    }
    if (PDKeyDown(0x45)) {
        i = gHeadup_text_buffer_0079d880.field_0x10 - 1;
        current = gHeadup_text_buffer_0079d880.text_last_add;
        if (i > 0) {
            do {
                if (&gHeadup_text_buffer_0079d880 != NULL && current != NULL && current->next != NULL) {
                    current = current->next;
                    gHeadup_text_buffer_0079d880.text_last_add = current;
                    gHeadup_text_buffer_0079d880.pos_last_add++;
                }
                i--;
            } while (i != 0);
        }
        while (PDKeyDown(0x45)) {
        }
    }
    AdjustHeadupTextBuffer(&gHeadup_text_buffer_0079d880);
    text = gHeadup_text_buffer_0079d880.field_0x4;
    headup = gRace_head_ups;
    do {
        if (text == NULL) {
            ChangeHeadupText(*headup, "");
        } else {
            font = -1;
            if (text != gHeadup_text_buffer_0079d880.text_last_add) {
                font = -2;
            }
            ChangeHeadupFont(*headup, font);
            ChangeHeadupText(*headup, text->text);
            text = text->next;
        }
        headup++;
    } while (headup < &gRace_head_ups[22]);
#else
    __asm {
            mov eax, dword ptr [gINT_0068b8e4]
            test eax, eax
            je L0045b34c
            push edi
            push esi
            call PDGetTotalTime
            mov ecx, 0x42
            mov esi, eax
            call PDKeyDown
            test eax, eax
            je L0045b1c4
            mov eax, offset gHeadup_text_buffer_0079d880
            test eax, eax
            je L0045b1b1
            mov ecx, dword ptr [gHeadup_text_buffer_0079d880+0x18]
            test ecx, ecx
            je L0045b1b1
            mov eax, dword ptr [gHeadup_text_buffer_0079d880]
            cmp ecx, eax
            je L0045b1b1
            mov ecx, dword ptr [gHeadup_text_buffer_0079d880+0xc]
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], eax
            dec ecx
            test ecx, ecx
            mov dword ptr [gHeadup_text_buffer_0079d880+0xc], ecx
            jle L0045b1b1
    L0045b1a6:
            mov eax, dword ptr [eax + 0x38]
            dec ecx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], eax
            jne L0045b1a6
    L0045b1b1:
            xor cl, cl
            cmp cl, 0x64
            jae L0045b1c4
    L0045b1b8:
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x64
            jb L0045b1b8
    L0045b1c4:
            mov ecx, 0x43
            call PDKeyDown
            test eax, eax
            je L0045b20d
            mov edx, offset gHeadup_text_buffer_0079d880
            test edx, edx
            je L0045b1fb
            mov eax, dword ptr [gHeadup_text_buffer_0079d880+0x18]
            test eax, eax
            je L0045b1fb
            mov eax, dword ptr [eax + 0x38]
            test eax, eax
            je L0045b1fb
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], eax
            mov eax, dword ptr [gHeadup_text_buffer_0079d880+0xc]
            inc eax
            mov dword ptr [gHeadup_text_buffer_0079d880+0xc], eax
    L0045b1fb:
            xor al, al
            cmp al, 0x64
            jae L0045b20d
    L0045b201:
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x64
            jb L0045b201
    L0045b20d:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            je L0045b28a
            mov ecx, dword ptr [gHeadup_text_buffer_0079d880+0x10]
            lea eax, [ecx - 1]
            test eax, eax
            jle L0045b26e
            mov esi, dword ptr [gHeadup_text_buffer_0079d880]
            mov edx, dword ptr [gHeadup_text_buffer_0079d880+0xc]
            mov edi, eax
            mov eax, dword ptr [gHeadup_text_buffer_0079d880+0x18]
    L0045b23b:
            mov ecx, offset gHeadup_text_buffer_0079d880
            test ecx, ecx
            je L0045b26b
            test eax, eax
            je L0045b26b
            cmp eax, esi
            je L0045b26b
            dec edx
            mov eax, esi
            test edx, edx
            mov dword ptr [gHeadup_text_buffer_0079d880+0xc], edx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], eax
            jle L0045b26b
            mov ecx, edx
    L0045b260:
            mov eax, dword ptr [eax + 0x38]
            dec ecx
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], eax
            jne L0045b260
    L0045b26b:
            dec edi
            jne L0045b23b
    L0045b26e:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            je L0045b28a
    L0045b27c:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            jne L0045b27c
    L0045b28a:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            je L0045b2f3
            mov edx, dword ptr [gHeadup_text_buffer_0079d880+0x10]
            lea eax, [edx - 1]
            test eax, eax
            jle L0045b2d7
            mov ecx, dword ptr [gHeadup_text_buffer_0079d880+0x18]
            mov edx, eax
    L0045b2ad:
            mov eax, offset gHeadup_text_buffer_0079d880
            test eax, eax
            je L0045b2d4
            test ecx, ecx
            je L0045b2d4
            mov eax, dword ptr [ecx + 0x38]
            test eax, eax
            je L0045b2d4
            mov ecx, eax
            mov eax, dword ptr [gHeadup_text_buffer_0079d880+0xc]
            inc eax
            mov dword ptr [gHeadup_text_buffer_0079d880+0x18], ecx
            mov dword ptr [gHeadup_text_buffer_0079d880+0xc], eax
    L0045b2d4:
            dec edx
            jne L0045b2ad
    L0045b2d7:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            je L0045b2f3
    L0045b2e5:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            jne L0045b2e5
    L0045b2f3:
            mov ecx, offset gHeadup_text_buffer_0079d880
            call AdjustHeadupTextBuffer
            mov edi, dword ptr [gHeadup_text_buffer_0079d880+0x4]
            mov esi, offset gRace_head_ups
    L0045b308:
            test edi, edi
            je L0045b333
            cmp edi, dword ptr [gHeadup_text_buffer_0079d880+0x18]
            jne L0045b319
            or edx, 0xffffffff
            jmp L0045b31e
    L0045b319:
            mov edx, 0xfffffffe
    L0045b31e:
            mov ecx, dword ptr [esi]
            call ChangeHeadupFont
            mov ecx, dword ptr [esi]
            mov edx, edi
            call ChangeHeadupText
            mov edi, dword ptr [edi + 0x38]
            jmp L0045b33f
    L0045b333:
            mov ecx, dword ptr [esi]
            mov edx, offset gDisable_sound+0x8
            call ChangeHeadupText
    L0045b33f:
            add esi, 4
            cmp esi, offset gRace_head_ups+0x58
            jl L0045b308
            pop esi
            pop edi
    L0045b34c:
            ret
            nop
            nop
            nop
            mov eax, dword ptr [edx*4 + gKey_mapping]
            mov dword ptr [ecx + eax*4], 1
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045c760
void C2_HOOK_FASTCALL FUN_0045c760(void) {
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
    if (gCurrent_ffb_effect_index != -1) {
        StopDinputEffect(gCurrent_ffb_effect_index);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00502e70
void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex) {
    char text[256];
    char price_text[256];

    if (gRace_finished || gProgram_state.current_car.knackered || gWait_for_it) {
        return;
    }

    if (gProgram_state.current_car.power_up_levels[pIndex] < gProgram_state.current_car.power_up_slots[pIndex]) {
        if (gProgram_state.credits < (gNet_mode != eNet_mode_none
                ? gCost_APO.initial_network[gCurrent_net_game->type]
                : gCost_APO.initial[gProgram_state.skill_level])) {
            strcpy(text, GetMiscString(0xb3));
            sprintf(price_text, "%d", (gNet_mode != eNet_mode_none
                ? gCost_APO.initial_network[gCurrent_net_game->type]
                : gCost_APO.initial[gProgram_state.skill_level]));
            SubsStringJob(text, price_text);
            NewTextHeadupSlot(4, 0, 3000, -4, text);
        } else {
            SpendCredits(gNet_mode != eNet_mode_none
                ? gCost_APO.initial_network[gCurrent_net_game->type]
                : gCost_APO.initial[gProgram_state.skill_level]);
            gProgram_state.current_car.power_up_levels[pIndex]++;
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(pIndex + 0xaf));
        }
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(0xae));
    }
}

// FUNCTION: CARMA2_HW 0x004415c0
void C2_HOOK_FASTCALL FinishRace(int i) {
    gRace_over_reason = eRace_over_0;
    gAbandon_game = 2;
}

// FUNCTION: CARMA2_HW 0x00442e80
void C2_HOOK_FASTCALL GetPowerup(int pNum) {
    GotPowerup(&gProgram_state.current_car, pNum);
}

void C2_HOOK_FASTCALL SteelBollock(void) {

    SphericizeCar(gCar_to_view);
    NewTextHeadupSlot(4, 0, 2000, -4, "STEEL GONAD O' DEATH");
}

// FUNCTION: CARMA2_HW 0x00444f10
void C2_HOOK_FASTCALL RichardsSphere(int pNum) {

    TotallyRepairACar(gCar_to_view);
    SteelBollock();
}

// FUNCTION: CARMA2_HW 0x00444820
void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void) {
    const char* src;
    char* dst;

    gCar_simplification_level++;
    gCar_simplification_level = gCar_simplification_level % 5u;
    src = GetMiscString(eMiscString_car_simplification_level_D);
    dst = BrMemAllocate(strlen(src) + 21, kMem_simp_level);
    sprintf(dst, src, gCar_simplification_level);
    NewTextHeadupSlot(4, 0, 2000, -4, dst);
    BrMemFree(dst);
}

// FUNCTION: CARMA2_HW 0x00444720
void C2_HOOK_FASTCALL CycleYonFactor(void) {
    float factor;

    factor = 0.5f * GetYonFactor();
    if (factor < 0.1f) {
        factor = 1.0f;
    }
    SetYonFactor(factor);
    if (factor > 0.75f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_quickly));
    } else if (factor > 0.375f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_quickly));
    } else if (factor > 0.187f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_late));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_late));
    }
}

// FUNCTION: CARMA2_HW 0x004448a0
void C2_HOOK_FASTCALL ToggleAccessoryRendering(void) {
    int on;

    if (gNet_mode != eNet_mode_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_net_games_always_accessorized));
    } else {
        on = !GetAccessoryRendering();
        SetAccessoryRendering(on);
        if (on) {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_on));
        } else {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_off));
        }
    }
}

// FUNCTION: CARMA2_HW 0x004447c0
void C2_HOOK_FASTCALL CycleSoundDetailLevel(void) {
    int new_level;

    new_level = (gSound_detail_level + 1) % 3;
    ReallySetSoundDetailLevel(new_level);
    SetSoundDetailLevel(new_level);
    switch (gSound_detail_level) {
        case 0:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_fewest_sounds));
            break;
        case 1:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_partial_sound));
            break;
        case 2:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_all_sounds));
            break;
        default:
            break;
    }
}

// FUNCTION: CARMA2_HW 0x004599f0
void C2_HOOK_FASTCALL ShowCurrentJoystickName(void) {
/* DirectInput joystick names exist only on the win32 platform: the
 * gDirectInput* globals live in win32/win32_dinput.c, which a Windows build
 * of the SDL3 platform does not compile. */
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
    char buffer[256];

    if (gJoystick_index == -1 || gDirectInputJoystickDevices[gJoystick_index] == NULL) {
        return;
    }
    sprintf(buffer, "Joystick %d: %s %d", gJoystick_index,
            gDirectInputJoystickInfos[gJoystick_index].productName,
            (gJoystickFFB & (1 << gJoystick_index)) != 0);
    NewTextHeadupSlot(4, 0, 3000, -1, buffer);
#else
    (void)0;
#endif
}

// UNLABELED: CARMA2_HW 0x0045c240
void C2_HOOK_FASTCALL FUN_0045c240(void) {
    tHeadup_text* text;
    int i;

    if (gINT_0068b8e8) {
        gINT_0068b8e8 = 0;
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor != 0) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
    } else {
        gINT_0068b8e8 = 1;
        FUN_0044a8a0(gProgram_state.frame_rate_headup, 10, 350);
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRace_head_ups); i++) {
            FUN_0044a8a0(gRace_head_ups[i], 10, 120 + i * 10);
            ChangeHeadupFont(gRace_head_ups[i], -2);
        }
        CreateAccentPolyActor(8.0f, 118.0f, 130.0f, 224.0f);
        if (gJoystick_headup_buffer_0079d8a0.count == 0) {
            if (gJoystick_headup_buffer_0079d8a0.field_0x14 < 5) {
                gJoystick_headup_buffer_0079d8a0.field_0x14 = 10;
            }
            gJoystick_headup_buffer_0079d8a0.texts = NULL;
            gJoystick_headup_buffer_0079d8a0.count = 0;
            gJoystick_headup_buffer_0079d8a0.pos_last_add = 0;
            gJoystick_headup_buffer_0079d8a0.text_last_add = NULL;
            gJoystick_headup_buffer_0079d8a0.field_0x4 = NULL;
            gJoystick_headup_buffer_0079d8a0.field_0x10 = 22;
            for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gJoystick_headup_texts); i++) {
                text = malloc(sizeof(tHeadup_text));
                if (text != NULL) {
                    text->next = NULL;
                    strncpy(text->text, gJoystick_headup_texts[i].text, 0x28);
                    FUN_0045a920(&gJoystick_headup_buffer_0079d8a0, text, 0);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00511240
void C2_HOOK_FASTCALL ToggleJoystickHeadup(void) {

    if (gINT_0068b8e8) {
        SaveAllJoystickData();
    }
    FUN_0045c240();
    if (!gINT_0068b8e8) {
        LoadJoystickPreferences();
    }
}

// FUNCTION: CARMA2_HW 0x00511280
void C2_HOOK_FASTCALL ToggleJoystickYAxis(void) {
    char buffer[256];

    if (ToggleJoystickYInvert()) {
        sprintf(buffer, "Y Axis Enabled");
    } else {
        sprintf(buffer, "Y Axis Disabled");
    }
    NewTextHeadupSlot(4, 0, 3000, -4, buffer);
}

// FUNCTION: CARMA2_HW 0x00511330
void C2_HOOK_FASTCALL ToggleJoystickDPad(void) {
    char buffer[256];

    if (PDIsJoystickDPadEnabled()) {
        SetJoystickDPadEnabled(0);
        sprintf(buffer, "D-Pad Mode Disabled");
    } else {
        SetJoystickDPadEnabled(1);
        sprintf(buffer, "D-Pad Mode Enabled");
    }
    NewTextHeadupSlot(4, 0, 3000, -4, buffer);
}

// FUNCTION: CARMA2_HW 0x00511270
void C2_HOOK_FASTCALL CycleJoystick(void) {
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
    SaveAllJoystickData();
    FUN_0045c420();
    LoadJoystickPreferences();
#else
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x005112d0
void C2_HOOK_FASTCALL ToggleJoystickEnable(void) {
    char buffer[256];

    if (HasCurrentJoystick()) {
        FUN_0045a0c0();
        sprintf(buffer, "Joystick Disabled");
    } else {
        PDEnableJoysticks();
        sprintf(buffer, "Joystick Enabled");
    }
    NewTextHeadupSlot(4, 0, 3000, -4, buffer);
}

// FUNCTION: CARMA2_HW 0x00511390
void C2_HOOK_FASTCALL IncreaseJoystickFFB(void) {
    char buffer[256];

    sprintf(buffer, "FFB Gain Set to %d%%", IncreaseJoystickFFBGain());
    NewTextHeadupSlot(4, 0, 3000, -4, buffer);
}

// FUNCTION: CARMA2_HW 0x00447330
void C2_HOOK_FASTCALL FUN_00447330(void) {
}

// FUNCTION: CARMA2_HW 0x00447340
void C2_HOOK_FASTCALL FUN_00447340(void) {
}

// FUNCTION: CARMA2_HW 0x00444420
void C2_HOOK_FASTCALL CycleInvulnerability(void) {
    char message[256];

    gInvulnerability = (gInvulnerability + 1) % 8;
    gCar_to_view->invulnerable_no_damage = gInvulnerability & 1;
    gCar_to_view->invulnerable_no_crushage = gInvulnerability & 2;
    gCar_to_view->invulnerable_no_wastage = gInvulnerability & 4;
    message[0] = '\0';
    if (gInvulnerability == 0) {
        strcpy(message, "FULLY VULNERABLE");
    }
    if (gCar_to_view->invulnerable_no_damage) {
        strcat(message, " NO DAMAGE");
    }
    if (gCar_to_view->invulnerable_no_crushage != 0) {
        strcat(message, " NO CRUSHAGE");
    }
    if (gCar_to_view->invulnerable_no_wastage != 0) {
        strcat(message, " NO WASTAGE");
    }
    NewTextHeadupSlot(4, 0, 1000, -4, message);
}

// FUNCTION: CARMA2_HW 0x004d6240
void C2_HOOK_FASTCALL NextPedCam(void) {
    int original_ped;

    if (gPed_count != 0) {
        original_ped = gSelected_ped;
        do {
            gSelected_ped++;
            if (gSelected_ped >= gPed_count) {
                gSelected_ped = 0;
            }
        } while (!(gPedestrian_array[gSelected_ped].flags & 0x1) && original_ped != gSelected_ped);
    }
}

// FUNCTION: CARMA2_HW 0x004d6290
void C2_HOOK_FASTCALL PrevPedCam(void) {
    int count;
    int i;
    int start;

    count = gPed_count;
    if (count == 0) {
        return;
    }
    i = gSelected_ped;
    start = i;
    do {
        i -= 1;
        gSelected_ped = i;
        if (i < 0) {
            i = count - 1;
            gSelected_ped = i;
        }
    } while (!(gPedestrian_array[i].flags & 1) && start != i);
}

br_vector3 gPredist_point;

// FUNCTION: CARMA2_HW 0x004d62e0
void C2_NAKED C2_HOOK_FASTCALL FindNearestPed(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int count;
    int i;
    int best;
    tU32 predist;
    tPedestrian* p;

    count = gPed_count;
    if (count == 0) {
        return;
    }
    i = 0;
    predist = 0x4b189680;
    if (i < count) {
        best = predist;
        do {
            float dist;

            p = &gPedestrian_array[i];
            dist = (p->pos.v[0] - gProgram_state.current_car.pos.v[0]) * (p->pos.v[0] - gProgram_state.current_car.pos.v[0])
                 + (p->pos.v[1] - gProgram_state.current_car.pos.v[1]) * (p->pos.v[1] - gProgram_state.current_car.pos.v[1])
                 + (p->pos.v[2] - gProgram_state.current_car.pos.v[2]) * (p->pos.v[2] - gProgram_state.current_car.pos.v[2]);
            if (dist < *(float*)&predist) {
                *(float*)&predist = dist;
                best = i;
            }
            i += 1;
        } while (i < count);
    }
    gSelected_ped = best;
    gPed_nearness = 1;
#else
    __asm {
            sub esp, 8
            push esi
            mov esi, dword ptr [gPed_count]
            test esi, esi
            je L004d6385
            xor ecx, ecx
            push edi
            test esi, esi
            mov dword ptr [esp + 0xc], 0x4b189680
            jle L004d6370
            mov eax, dword ptr [gPedestrian_array]
            mov edi, dword ptr [esp + 0xc]
            lea edx, [eax + 0x20]
    L004d630d:
            fld dword ptr [edx]
            fld dword ptr [edx - 4]
            fsub dword ptr [gProgram_state+0xf8]
            fxch st(1)
            fsub dword ptr [gProgram_state+0xfc]
            fld dword ptr [edx + 4]
            fsub dword ptr [gProgram_state+0x100]
            fxch st(2)
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fstp st(1)
            fcom dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 1
            je L004d6364
            fstp dword ptr [esp + 0xc]
            mov edi, ecx
            jmp L004d6366
    L004d6364:
            fstp st(0)
    L004d6366:
            inc ecx
            add edx, 0x54
            cmp ecx, esi
            jl L004d630d
            jmp L004d6374
    L004d6370:
            mov edi, dword ptr [esp + 0xc]
    L004d6374:
            mov dword ptr [gSelected_ped], edi
            mov dword ptr [gPed_nearness], 1
            pop edi
    L004d6385:
            pop esi
            add esp, 8
            ret
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00444590
void C2_HOOK_FASTCALL ToggleTimerFreeze(void) {

    gFreeze_timer = !gFreeze_timer;
    if (gFreeze_timer) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_timer_frozen));
    } else {
        NewTextHeadupSlot(4, 0, 1000, -4, "Timer thawed out");
    }
}

// FUNCTION: CARMA2_HW 0x004445e0
void C2_HOOK_FASTCALL EarnDosh(void) {
    EarnCredits(5000);
}

// FUNCTION: CARMA2_HW 0x004445f0
void C2_HOOK_FASTCALL LoseDosh(void) {
    EarnCredits(-5000);
}

// FUNCTION: CARMA2_HW 0x0040e530
void C2_HOOK_FASTCALL FUN_0040e530(br_actor* pActor) {
    if (pActor == NULL) {
        gCar_to_view = &gProgram_state.current_car;
    } else {
        if (pActor->parent != gNon_track_actor) {
            gCar_to_view = (tCar_spec*)DoPullActorFromWorld(pActor);
            if (gCar_to_view == NULL) {
                gCar_to_view = &gProgram_state.current_car;
            }
        } else {
            gCar_to_view = (tCar_spec*)pActor->type_data;
        }
    }
    gCamera_yaw = 0;
    InitialiseExternalCamera();
    PositionExternalCamera(gCar_to_view, 0xc8);
}

// GLOBAL: CARMA2_HW 0x0058f628
int gView_opponent_index = -1;

// FUNCTION: CARMA2_HW 0x0040e590
void C2_HOOK_FASTCALL ViewOpponent(void) {
    const char* name;

    gView_opponent_index++;
    if (gNet_mode != eNet_mode_none) {
        if (gView_opponent_index >= gNumber_of_net_players) {
            gView_opponent_index = 0;
        }
        if (gNet_mode == eNet_mode_host) {
            gCar_to_view = gRace_board[gView_opponent_index].car;
        } else if (gCar_to_view_id == 0) {
            tNet_message* message;

            gCar_to_view_id = gNet_players[gView_opponent_index].ID;
            message = NetBuildGuaranteedMessage(0x2f, 0);
            *(tPlayer_ID*)((tU8*)message + 0x1c) = gCar_to_view_id;
            NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
            return;
        }
        name = gNet_players[gView_opponent_index].player_name;
    } else {
        if (gView_opponent_index >= gNum_viewable_cars) {
            gView_opponent_index = 0;
        }
        gCar_to_view = gViewable_car_list[gView_opponent_index];
        name = gCar_to_view->driver_name;
    }
    NewTextHeadupSlot(4, 0, 2000, -3, name);
    gCamera_yaw = 0;
    InitialiseExternalCamera();
    PositionExternalCamera(gCar_to_view, 200);
}

// FUNCTION: CARMA2_HW 0x004db8e0
void C2_NAKED C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    ARStartPipingSession(ePipe_chunk_repulse_ray);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gRepulse_links); i++) {
        tRepulse_link* link = &gRepulse_links[i];
        tU32 delta_time;
        br_model *model;
        br_vector3 delta;
        br_vector3 dir;
        br_vector3 delta_orth;
        br_vector3 delta_unit;
        br_vector3 y_unit;
        br_vector3 orth_dirs[5];
        br_vector3 orth_pieces1[5];
        br_vector3 orth_pieces2[5];
        br_vector3 p;
        int blend;
        int j;
        float percentage;

        if (link->time == 0) {
            continue;
        }
        delta_time = pTime - link->time;
        if (delta_time >= 700) {
            link->time = 0;
            if (link->actor->parent == gNon_track_actor) {
                BrActorRemove(link->actor);
            }
            continue;
        }
        BrVector3Sub(&delta, link->pos_victim, link->pos_origin);
        model = link->actor->model;
        if (delta.v[0] == 0.f && delta.v[2] == 0.f) {
            continue;
        }
        BrVector3Scale(&delta_unit, &delta, 1.f / (float)(model->nvertices / 2 - 1));
        BrVector3Normalise(&dir, &delta);
        BrVector3Set(&y_unit, 0.f, 1.f, 0.f);
        BrVector3Cross(&delta_orth, &delta, &y_unit);
        BrVector3Copy(&p, link->pos_origin);
        BrVector3Normalise(&orth_dirs[0], &delta_orth);
        for (j = 1; j < 5; j++) {
            br_matrix34 mat;

            BrMatrix34Rotate(&mat, (i * (br_angle)0x10000) / 5, &dir);
            BrMatrix34ApplyV(&orth_dirs[i], &orth_dirs[0], &mat);
        }
        for (j = 0; j < 5; j++) {
            BrVector3Scale(&orth_pieces1[i], &orth_dirs[i], link->field_0x18);
            BrVector3Scale(&orth_pieces2[i], &orth_dirs[i], link->field_0x1c);
        }
        for (j = 0; j < model->nvertices / 2; j++) {
            BrVector3Accumulate(&p, &delta_unit);
            BrVector3Add(&model->vertices[j].p, &p, &orth_pieces1[j % 5]);
            BrVector3Add(&model->vertices[j + model->nvertices / 2].p, &p, &orth_pieces2[j % 5]);
        }
        percentage = (float)delta_time / 700.f;
        if (percentage > .9f) {
            blend = 25;
        } else if (percentage > .8f) {
            blend = 50;
        } else if (percentage > .7f) {
            blend = 75;
        } else {
            blend = 100;
        }
        if (blend != link->field_0x10) {
            BlendifyMaterial(link->material, blend);
            BrMaterialUpdate(link->material, BR_MATU_ALL);
            link->field_0x10 = blend;
        }
        BrModelUpdate(model, BR_MODU_ALL);
        if (link->actor->parent != gNon_track_actor) {
            BrActorAdd(gNon_track_actor, link->actor);
        }
        AddRepulseRayToPipingSession(i, link->pos_origin, link->pos_victim, link->time);
    }
    AREndPipingSession();
#else
    __asm {
            sub esp, 0x22c
            push ebx
            push ebp
            mov dword ptr [esp + 0x40], ecx
            push esi
            push edi
            mov ecx, 0x25
            call ARStartPipingSession
            mov dword ptr [esp + 0x20], 0
            mov esi, offset gRepulse_links+0x14
    L004db905:
            mov ecx, dword ptr [esi - 0x14]
            test ecx, ecx
            je L004dbd81
            mov eax, dword ptr [esp + 0x48]
            sub eax, ecx
            cmp eax, 0x2bc
            mov dword ptr [esp + 0x50], eax
            jb L004db94a
            mov edx, dword ptr [gNon_track_actor]
            mov dword ptr [esi - 0x14], 0
            mov eax, dword ptr [esi - 8]
            mov ecx, dword ptr [eax + 0xc]
            cmp ecx, edx
            jne L004dbd81
            push eax
            call BrActorRemove
            add esp, 4
            jmp L004dbd81
    L004db94a:
            mov eax, dword ptr [esi - 0xc]
            mov ecx, dword ptr [esi - 0x10]
            mov edx, dword ptr [esi - 8]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x21
            fld dword ptr [eax + 4]
            fsub dword ptr [ecx + 4]
            mov ebp, dword ptr [edx + 0x18]
            mov dword ptr [esp + 0x4c], ebp
            fstp dword ptr [esp + 0x18]
            fld dword ptr [eax + 8]
            fsub dword ptr [ecx + 8]
            fstp dword ptr [esp + 0x1c]
            fcom dword ptr [gFLOAT_0058ac0c]
            fnstsw ax
            test ah, 0x40
            je L004db994
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gFLOAT_0058ac0c]
            fnstsw ax
            test ah, 0x40
            jne L004dbd7f
    L004db994:
            fld st(0)
            fld dword ptr [esp + 0x18]
            fxch st(1)
            fmul st(0), st(2)
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            xor eax, eax
            mov ax, word ptr [ebp + 0x10]
            faddp st(1), st(0)
            shr eax, 1
            dec eax
            fsqrt
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fld st(2)
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fdiv st(0), st(3)
            fxch st(1)
            fdiv st(0), st(3)
            fxch st(2)
            fdiv st(0), st(3)
            fxch st(4)
            fst dword ptr [esp + 0x10]
            fstp st(3)
            fld st(2)
            fcomp dword ptr [gFLOAT_0058ac10]
            fstp dword ptr [esp + 0x3c]
            fnstsw ax
            fstp dword ptr [esp + 0x40]
            fxch st(1)
            fstp dword ptr [esp + 0x44]
            test ah, 0x41
            jne L004dba30
            fdivr qword ptr [gDOUBLE_0058ac18]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x38]
            jmp L004dba4a
    L004dba30:
            fstp st(0)
            mov dword ptr [esp + 0x30], 0x3f800000
            mov dword ptr [esp + 0x34], 0
            mov dword ptr [esp + 0x38], 0
    L004dba4a:
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x1c]
            fld st(3)
            fxch st(3)
            fmul dword ptr [y_unit_vector+0x8]
            fxch st(2)
            fmul dword ptr [y_unit_vector+0x4]
            fld dword ptr [esp + 0x18]
            fxch st(1)
            fsubp st(3), st(0)
            fxch st(1)
            fmul dword ptr [y_unit_vector]
            fxch st(2)
            fxch st(4)
            fxch st(2)
            fld st(4)
            fmul st(0), st(5)
            fxch st(4)
            fmul dword ptr [y_unit_vector+0x8]
            fxch st(3)
            fmul dword ptr [y_unit_vector+0x4]
            fxch st(2)
            fmul dword ptr [y_unit_vector]
            fxch st(3)
            fsubp st(1), st(0)
            mov eax, dword ptr [ecx + 4]
            mov edx, dword ptr [ecx]
            fld st(0)
            fxch st(3)
            fsubp st(2), st(0)
            fxch st(2)
            fmul st(0), st(2)
            fld st(1)
            fmul st(0), st(2)
            fxch st(1)
            faddp st(4), st(0)
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x28], eax
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x2c], ecx
            faddp st(3), st(0)
            fxch st(2)
            fsqrt
            fcom dword ptr [gFLOAT_0058ac10]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fnstsw ax
            test ah, 0x41
            jne L004dbb0b
            fdivr qword ptr [gDOUBLE_0058ac18]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x5c]
            fxch st(1)
            fstp dword ptr [esp + 0x60]
            fstp dword ptr [esp + 0x64]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L004dbb2b
    L004dbb0b:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x5c], 0x3f800000
            mov dword ptr [esp + 0x60], 0
            mov dword ptr [esp + 0x64], 0
    L004dbb2b:
            mov edi, 0x10000
            lea ebx, [esp + 0x68]
    L004dbb34:
            lea edx, [esp + 0x30]
            mov eax, 0x66666667
            push edx
            imul edi
            sar edx, 1
            mov eax, edx
            lea ecx, [esp + 0x180]
            shr eax, 0x1f
            add edx, eax
            push edx
            push ecx
            call BrMatrix34Rotate
            add esp, 0xc
            lea edx, [esp + 0x17c]
            lea eax, [esp + 0x5c]
            push edx
            push eax
            push ebx
            call BrMatrix34ApplyV
            add edi, 0x10000
            add esp, 0xc
            add ebx, 0xc
            cmp edi, 0x50000
            jl L004dbb34
            xor eax, eax
    L004dbb83:
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 8]
            fld dword ptr [esp + eax + 0x5c]
            fld dword ptr [esp + eax + 0x60]
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + eax + 0x5c]
            fstp dword ptr [esp + eax + 0xec]
            fld dword ptr [esp + eax + 0x64]
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + eax + 0x60]
            add eax, 0xc
            fstp dword ptr [esp + eax + 0xe4]
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + eax + 0x58]
            cmp eax, 0x3c
            fstp dword ptr [esp + eax + 0xe8]
            fxch st(2)
            fmulp st(5), st(0)
            fxch st(4)
            fstp dword ptr [esp + eax + 0x1a0]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fstp dword ptr [esp + eax + 0x1a4]
            fstp dword ptr [esp + eax + 0x1a8]
            jl L004dbb83
            mov ecx, dword ptr [ebp + 8]
            xor eax, eax
            mov ax, word ptr [ebp + 0x10]
            xor ebx, ebx
            shr eax, 1
            test eax, eax
            lea edx, [eax + eax*4]
            lea edi, [ecx + edx*8]
            jle L004dbcc5
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x24]
    L004dbc10:
            mov eax, ebx
            mov ebp, 5
            cdq
            fld dword ptr [esp + 0x2c]
            fxch st(1)
            fadd dword ptr [esp + 0x3c]
            fxch st(2)
            fadd dword ptr [esp + 0x40]
            fxch st(1)
            fadd dword ptr [esp + 0x44]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            idiv ebp
            xor ebp, ebp
            add ecx, 0x28
            mov dword ptr [esp + 0x10], ebp
            mov ebp, dword ptr [esp + 0x4c]
            add edi, 0x28
            lea eax, [edx + edx*2]
            shl eax, 2
            inc ebx
            fld dword ptr [esp + eax + 0xec]
            fld dword ptr [esp + eax + 0xf0]
            fld dword ptr [esp + eax + 0xf4]
            fld dword ptr [esp + eax + 0x1ac]
            fld dword ptr [esp + eax + 0x1b0]
            fld dword ptr [esp + eax + 0x1b4]
            fxch st(5)
            fadd st(0), st(6)
            mov eax, dword ptr [esp + 0x10]
            fstp dword ptr [ecx - 0x28]
            fxch st(3)
            fadd st(0), st(6)
            fxch st(2)
            fadd dword ptr [esp + 0x2c]
            fxch st(1)
            fadd st(0), st(5)
            fxch st(3)
            fadd st(0), st(6)
            fxch st(4)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [ecx - 0x24]
            fstp dword ptr [ecx - 0x20]
            fxch st(1)
            fstp dword ptr [edi - 0x28]
            fxch st(1)
            fstp dword ptr [edi - 0x24]
            fstp dword ptr [edi - 0x20]
            mov ax, word ptr [ebp + 0x10]
            shr eax, 1
            cmp ebx, eax
            jl L004dbc10
            fstp st(0)
            fstp st(0)
    L004dbcc5:
            mov ecx, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x58], 0
            mov dword ptr [esp + 0x54], ecx
            fild qword ptr [esp + 0x54]
            fmul dword ptr [gFLOAT_0058ac20+0x4]
            fcom qword ptr [gFLOAT_0058ac20+0x8]
            fnstsw ax
            test ah, 0x41
            jne L004dbcf5
            fstp st(0)
            mov edi, 0x19
            jmp L004dbd22
    L004dbcf5:
            fcom qword ptr [gFLOAT_0058ac20+0x10]
            fnstsw ax
            test ah, 0x41
            jne L004dbd0b
            fstp st(0)
            mov edi, 0x32
            jmp L004dbd22
    L004dbd0b:
            fcomp qword ptr [gFLOAT_0058ac20+0x18]
            mov edi, 0x4b
            fnstsw ax
            test ah, 0x41
            je L004dbd22
            mov edi, 0x64
    L004dbd22:
            cmp edi, dword ptr [esi - 4]
            je L004dbd43
            mov ecx, dword ptr [esi]
            mov edx, edi
            call BlendifyMaterial
            mov edx, dword ptr [esi]
            push 0x7fff
            push edx
            call BrMaterialUpdate
            add esp, 8
            mov dword ptr [esi - 4], edi
    L004dbd43:
            push 0x7fff
            push ebp
            call BrModelUpdate
            mov eax, dword ptr [esi - 8]
            mov ecx, dword ptr [gNon_track_actor]
            add esp, 8
            cmp dword ptr [eax + 0xc], ecx
            je L004dbd69
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
    L004dbd69:
            mov eax, dword ptr [esi - 0x14]
            mov ecx, dword ptr [esi - 0xc]
            mov edx, dword ptr [esi - 0x10]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x28]
            call AddRepulseRayToPipingSession
            jmp L004dbd81
    L004dbd7f:
            fstp st(0)
    L004dbd81:
            mov edx, dword ptr [esp + 0x20]
            add esi, 0x28
            inc edx
            cmp esi, offset gPowerup_respawn_specs
            mov dword ptr [esp + 0x20], edx
            jl L004db905
            call AREndPipingSession
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x22c
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

void C2_HOOK_FASTCALL GotPowerupN(int pN) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + pN, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbdb0
void C2_HOOK_FASTCALL GotPowerup0(void) {
    GotPowerupN(0);
}

// FUNCTION: CARMA2_HW 0x004dbe10
void C2_HOOK_FASTCALL GotPowerup1(void) {
    GotPowerupN(1);
}

// FUNCTION: CARMA2_HW 0x004dbe70
void C2_HOOK_FASTCALL GotPowerup2(void) {
    GotPowerupN(2);
}

// FUNCTION: CARMA2_HW 0x004dbed0
void C2_HOOK_FASTCALL GotPowerup3(void) {
    GotPowerupN(3);
}

// FUNCTION: CARMA2_HW 0x004dbf30
void C2_HOOK_FASTCALL GotPowerup4(void) {
    GotPowerupN(4);
}

// FUNCTION: CARMA2_HW 0x004dbf90
void C2_HOOK_FASTCALL GotPowerup5(void) {
    GotPowerupN(5);
}

// FUNCTION: CARMA2_HW 0x004dbff0
void C2_HOOK_FASTCALL GotPowerup6(void) {
    GotPowerupN(6);
}

// FUNCTION: CARMA2_HW 0x004dc050
void C2_HOOK_FASTCALL GotPowerup7(void) {
    GotPowerupN(7);
}

// FUNCTION: CARMA2_HW 0x004dc0b0
void C2_HOOK_FASTCALL GotPowerup8(void) {
    GotPowerupN(8);
}

// FUNCTION: CARMA2_HW 0x004dc110
void C2_HOOK_FASTCALL GotPowerup9(void) {
    GotPowerupN(9);
}

// FUNCTION: CARMA2_HW 0x00441e10
void C2_HOOK_FASTCALL LookLeft(void) {
    PratcamEvent(27);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_left) {
        LookForward();
    } else if (gProgram_state.which_view == eView_right) {
        LookForward();
        gProgram_state.pending_view = eView_left;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_left;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441ec0
void C2_HOOK_FASTCALL LookForward(void) {
    if (gProgram_state.which_view == eView_right) {
        PratcamEvent(27);
    } else if (gProgram_state.which_view == eView_left) {
        PratcamEvent(28);
    }
    if (gProgram_state.which_view != eView_forward) {
        gProgram_state.old_view = gProgram_state.which_view;
        ClearWobbles();
        gProgram_state.new_view = eView_forward;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441f20
void C2_HOOK_FASTCALL LookRight(void) {
    PratcamEvent(28);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_right) {
        LookForward();
    } else if (gProgram_state.which_view == eView_left) {
        LookForward();
        gProgram_state.pending_view = eView_right;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_right;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441fd0
void C2_HOOK_FASTCALL DamageTest(void) {
}

// FUNCTION: CARMA2_HW 0x00441fe0
void C2_HOOK_FASTCALL TDamageEngine(void) {
    DamageEngine(21);
}

// FUNCTION: CARMA2_HW 0x00441ff0
void C2_HOOK_FASTCALL TDamageDriver(void) {
    if (gProgram_state.current_car.damage_units[eDamage_driver].damage_level < 80) {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 80 - gProgram_state.current_car.damage_units[eDamage_driver].damage_level);
    } else {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 2);
    }
}

// FUNCTION: CARMA2_HW 0x00442030
void C2_HOOK_FASTCALL TDamageTrans(void) {
    DamageTrans(21);
}

// FUNCTION: CARMA2_HW 0x00442050
void C2_HOOK_FASTCALL TDamageLFWheel(void) {
    DamageLFWheel(21);
}

// FUNCTION: CARMA2_HW 0x00442060
void C2_HOOK_FASTCALL TDamageLFBrake(void) {
    DamageLFBrake(21);
}

// FUNCTION: CARMA2_HW 0x00442070
void C2_HOOK_FASTCALL TDamageLRBrake(void) {
    DamageLRBrake(21);
}

// FUNCTION: CARMA2_HW 0x00442080
void C2_HOOK_FASTCALL TDamageLRWheel(void) {
    DamageLRWheel(21);
}

// FUNCTION: CARMA2_HW 0x00442090
void C2_HOOK_FASTCALL TDamageRFWheel(void) {
    DamageRFWheel(21);
}

// FUNCTION: CARMA2_HW 0x004420a0
void C2_HOOK_FASTCALL TDamageRFBrake(void) {
    DamageRFBrake(21);
}

// FUNCTION: CARMA2_HW 0x004420b0
void C2_HOOK_FASTCALL TDamageRRBrake(void) {
    DamageRRBrake(21);
}

// FUNCTION: CARMA2_HW 0x004420c0
void C2_HOOK_FASTCALL TDamageRRWheel(void) {
    DamageRRWheel(21);
}

// FUNCTION: CARMA2_HW 0x00444c00
void C2_HOOK_FASTCALL DisplayUserMessage(void) {
    char* the_message;
    int len;
    tDR_font* font;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, net_message_enter_y, 0x4b4);

    font = &gFonts[6];
    the_message = &gString[20];
    if (!gEntering_message || gNet_mode == eNet_mode_none) {
        return;
    }

    len = strlen(the_message);
    if (len < 63 && (PDGetTotalTime() & 0x200)) {
        the_message[len] = '_';
        the_message[len + 1] = '\0';
    }
    DimRectangle(gBack_screen,
        15 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y - font->height,
        85 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        1);

    DRPixelmapText(gBack_screen, 20 * gBack_screen->width / 100, gCurrent_graf_data->net_message_enter_y, font, GetMiscString(eMiscString_enter_message), 100);
    OoerrIveGotTextInMeBoxMissus(
        6,
        the_message,
        gBack_screen,
        20 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 2 * font->height,
        80 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        0);
    the_message[len] = '\0';
}
#include "packfile.h"
#include "platform.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"

#include <stdio.h>

// CompleteRace

// AbortRace

// F4Key

// SetFlag

// FinishRace

// EnsureSpecialVolumesHidden

// ShowSpecialVolumesIfRequ

// DoEditModeKey

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

// F12Key

// NumberKey0

// NumberKey1

// NumberKey2

// NumberKey3

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

// LookLeft

// LookForward

// LookRight

// ToggleMiniMap

// CheckToggles

// CarWorldOffFallenCheckThingy

// HasCarFallenOffWorld

// CheckForBeingOutOfThisWorld

// CheckHornLocal

// CheckHorn3D

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

// CheckRecoverCost

// SortOutRecover

// SetFlipUpCar

// AlignChildren

// FlipUpCar

// GetPowerup

// CheckSystemKeys

// FUNCTION: CARMA2_HW 0x00443c50
void C2_HOOK_FASTCALL CheckForCheatingGits(void) {
    tU32* keys;

    keys = KevKeyService();
    if (keys[0] == 0x616fb8e4 && keys[1] == 0x7c6100a8) {
        ToggleSoundEnable();
        gProgram_state.game_completed = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_FlaskGone);
    }
}

// CheckKevKeys

// BrakeInstantly

// PollCarControls

// PollCameraControls

// SetFlag2

// ToggleFlying

// CycleInvulnerability

// ToggleTimerFreeze

// EarnDosh

// LoseDosh

// ToggleHeadupMap

// HackyMapFixForMac3dfx

// ToggleMap

// FUNCTION: CARMA2_HW 0x004446f0
int C2_HOOK_FASTCALL GetRecoverVoucherCount(void) {
    return gRecovery_voucher_count;
}

// AddVouchers

// ResetRecoveryVouchers

// CycleYonFactor

// CycleSoundDetailLevel

// CycleCarSimplificationLevel

// ToggleAccessoryRendering

// UserSendMessage

// EnterUserMessage

// DisplayUserMessage

// FUNCTION: CARMA2_HW 0x00444d70
void C2_HOOK_FASTCALL InitAbuseomatic(void) {
    char path[256];
    char s[256];
    FILE* f;
    int i;
    int len;

    gString[20] = '\0';
    PDBuildAppPath(path);
    strcat(path, "ABUSE.TXT");
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gAbuse_text); i++) {
        gAbuse_text[i] = NULL;
    }
    f = PFfopen(path, "rt");
    if (f == NULL) {
        return;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gAbuse_text); i++) {
        if (PFfgets(s, CARPOCALYPSE2_ASIZE(s) - 1, f) == NULL) {
            break;
        }
        // len = strlen(s);
        if (strlen(s) > 63) {
            s[63] = '\0';
        }
        len = strlen(s);
        while (len != 0 && s[len - 1] < ' ') {
            s[--len] = '\0';
        }
        gAbuse_text[i] = BrMemAllocate(strlen(s) + 1, kMem_abuse_text);
        strcpy(gAbuse_text[i], s);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00444ea0
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004446d0
tU32 C2_HOOK_FASTCALL GetKeyStateBit27(void) {
    return (*(tU32*)&gProgram_state.current_car.keys >> 27) & 1;
}

// FUNCTION: CARMA2_HW 0x00444fa0
void C2_HOOK_FASTCALL ToggleShellModel(void) {
    gCar_to_view->use_shell_model = !gCar_to_view->use_shell_model;
    if (gCar_to_view->use_shell_model) {
        NewTextHeadupSlot(4, 0, 2000, -4, "SHELL MODEL");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "COMPLEX MODELS");
    }
}

// ChangeCameraTypeInGame

// FUNCTION: CARMA2_HW 0x0045c5d0
void C2_NAKED C2_HOOK_FASTCALL FUN_0045c5d0(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gAccent_poly_prims[2].v.i32 != -1) {
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
        UnloadDinputFFBEffectAtIndex(gAccent_poly_prims[2].v.i32);
#endif
    }
    gAccent_poly_prims[2].v.i32 = -1;
#else
    __asm {
            mov ecx, dword ptr [gCurrent_ffb_effect_index]
            cmp ecx, -1
            je L0045c5e0
            call UnloadDinputFFBEffectAtIndex
    L0045c5e0:
            mov dword ptr [gCurrent_ffb_effect_index], 0xffffffff
            ret
    }
#endif
}

// SteelBollock

// RichardsSphere

// ToggleHeadupLevel
