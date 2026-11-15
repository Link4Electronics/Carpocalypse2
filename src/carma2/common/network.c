#include "network.h"

#include "controls.h"

#include "car.h"

#include "crush.h"

#include "crush2.h"

#include "pratcam.h"

#include "structur.h"

#include "smashing.h"

#include "smash.h"

#include "piping.h"

#include "displays.h"
#include "errors.h"

#include "explosions.h"

#include "font.h"

#include "globvrme.h"

#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "netgame.h"
#include "newgame.h"

#include "oil.h"

#include "opponent.h"

#include "pedestrn.h"

#include "phil.h"
#include "platform.h"

#include "physics.h"

#include "powerups.h"

#include "replay.h"
#include "spark.h"
#include "timers.h"
#include "utility.h"
#include "world.h"

#include "win95net.h"

#include "polyfont.h"

#include "c2_string.h"

#include "c2_stdio.h"

#include "brender/brender.h"
#include "network.h"

#include "netgame.h"
#include "spark.h"
#include "controls.h"
#include "platform.h"
#include "globvars.h"
#include "globvrpb.h"

#include "carpocalypse2_macros.h"

#include <stdlib.h>

extern char* gCurrent_palette_pixels;
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);
extern void C2_HOOK_FASTCALL EnsurePaletteUp(void);
extern void C2_HOOK_FASTCALL FadePaletteUp(void);

#define MIN_MESSAGES_CAPACITY 200
#define MID_MESSAGES_CAPACITY 200
#define MAX_MESSAGES_CAPACITY 200

#define MESSAGE_MAGIC_NUMBER 0x763a5059

#define MAX_GUARANTEED_MESSAGES 200
#define NET_MSG_STATUS 0x05
#define NET_MSG_LEAVE 0x0b
// GLOBAL: CARMA2_HW 0x0068d96c
extern int gNet_initialised;
// GLOBAL: CARMA2_HW 0x00690c48
extern tNet_message_memory* gDynamic_messages;
// GLOBAL: CARMA2_HW 0x00688b1c
int gNet_storage_space_initialized;

// GLOBAL: CARMA2_HW 0x00659c2c
int gScore_winner = 20;

// GLOBAL: CARMA2_HW 0x00659d34
int gINT_00659d34 = -1;

// GLOBAL: CARMA2_HW 0x00659d38
int gRace_only_flags[52] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0074a694
int gReceived_game_scores;

// GLOBAL: CARMA2_HW 0x0074a698
tReceived_message_cache_entry gReceived_message_cache[16];

// GLOBAL: CARMA2_HW 0x0068d968
int gINT_0068d968;

// GLOBAL: CARMA2_HW 0x00690c50
int gINT_00690c50;

// GLOBAL: CARMA2_HW 0x00690c6c
tU32 gUINT_00690c6c;

// GLOBAL: CARMA2_HW 0x00690c70
tU32 gUINT_00690c70;

// GLOBAL: CARMA2_HW 0x00690c74
tU32 gUINT_00690c74;

// GLOBAL: CARMA2_HW 0x00690c78
undefined4* gPTR_00690c78;

// GLOBAL: CARMA2_HW 0x00690c7c
undefined4* gPTR_00690c7c;

// GLOBAL: CARMA2_HW 0x00690c48
tNet_message_memory* gMessage_to_free;

// GLOBAL: CARMA2_HW 0x0068d990
tGuaranteed_message gGuarantee_list[200];

// GLOBAL: CARMA2_HW 0x0068d95c
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x0068d978
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x0068d984
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0068d98c
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c80
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690238
int gTime_for_next_one;

// GLOBAL: CARMA2_HW 0x00690c34
tNet_game_details* gCurrent_join_poll_game;

// GLOBAL: CARMA2_HW 0x00690c38
int gNet_service_disable;

// GLOBAL: CARMA2_HW 0x00690c3c
int gIn_net_service;

// GLOBAL: CARMA2_HW 0x00690c48
tNet_message_memory* gDynamic_messages;

// GLOBAL: CARMA2_HW 0x00690c4c
tU32 gLast_flush_message;

// GLOBAL: CARMA2_HW 0x00690c54
int gJoin_list_mode;

// GLOBAL: CARMA2_HW 0x0068d964
int gINT_0068d964;

// GLOBAL: CARMA2_HW 0x00690c40
tU32 gUINT_00690c40;

// GLOBAL: CARMA2_HW 0x00690230
tAddToJoinListProc* gAdd_proc;

// GLOBAL: CARMA2_HW 0x00690c30
int gINT_00690c30;

// GLOBAL: CARMA2_HW 0x0068d974
int gINT_0068d974;

// GLOBAL: CARMA2_HW 0x0068d988
int gINT_0068d988;

// GLOBAL: CARMA2_HW 0x00690234
int gINT_00690234;

// GLOBAL: CARMA2_HW 0x00690240
tNet_game_player_info gReceived_net_player_list[12];

/* 0x00762430 and 0x00763088 are inside gCurrent_race:
   number_of_racers and opponent_list[].net_player_index respectively. */

// GLOBAL: CARMA2_HW 0x0068d93c
int gNet_score_scene_initialised;

// GLOBAL: CARMA2_HW 0x0074a71c
br_actor* gNet_score_root;

// GLOBAL: CARMA2_HW 0x0074a720
br_material* gNet_score_materials[4];

// GLOBAL: CARMA2_HW 0x0074a730
br_actor* gNet_score_camera;

// GLOBAL: CARMA2_HW 0x0074a73c
br_model* gNet_score_model;

// GLOBAL: CARMA2_HW 0x0074a740
br_actor* gNet_score_actors[12];

// GLOBAL: CARMA2_HW 0x0068d968
int gINT_0068d968;

// GLOBAL: CARMA2_HW 0x0074b7dc
tPlayer_ID gLocal_net_ID;

// GLOBAL: CARMA2_HW 0x00690c44
int gOnly_receive_guarantee_replies;

// GLOBAL: CARMA2_HW 0x0068d97c
tMin_message* gMin_messages;

// GLOBAL: CARMA2_HW 0x0068d970
tMid_message* gMid_messages;

// GLOBAL: CARMA2_HW 0x0068d980
tMax_message* gMax_messages;

// GLOBAL: CARMA2_HW 0x0068d96c
int gNet_initialised;

// GLOBAL: CARMA2_HW 0x0068d960
int gDont_allow_joiners;

// GLOBAL: CARMA2_HW 0x0074a690
tU32 gUINT_0074a690;

// GLOBAL: CARMA2_HW 0x0074a718
tU32 gUINT_0074a718;

// GLOBAL: CARMA2_HW 0x00659d30
tU16 gGuarantee_number = 1;

// GLOBAL: CARMA2_HW 0x0068d958
int gNext_guarantee;

tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tU32 pSize_decider);
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers);
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrMemFree();
extern void C2_HOOK_FASTCALL CheckForVampireWinner(void);
extern void C2_HOOK_FASTCALL FUN_004ddde0(tCar_spec* pCar);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);
extern tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);
extern tU32 C2_HOOK_FASTCALL GetRaceTime(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space);
extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);
extern int C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pPlayer, undefined4 pArg4);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PDNetEndJoinList(void);
extern void C2_HOOK_FASTCALL PDNetHostFinishGame(void);
extern int C2_HOOK_FASTCALL PDNetInitClient(void);
extern void C2_HOOK_FASTCALL PDNetLeaveGame(void);
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL RemovePlayerFromShapeStatusLists(tPlayer_ID pPlayer_id, int pPlayer_count, tNet_game_player_info* pPlayers);
extern void C2_HOOK_FASTCALL RemoveRepulseLinksForCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);
extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);
extern br_uint_8 bl;
extern tCar_detail_info gCar_details[60];
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern int gINT_0074b780;
extern int gInitialised_grid;
extern int gIt_or_fox;
extern tBrender_storage gNet_cars_storage_space;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_net_players;
extern tOpponent* gOpponents;
extern tProgram_state gProgram_state;
extern int gStart_race_sent;
extern int gSynch_race_start;
extern int gThis_net_player_index;
#endif
/* Defined later in this file; referenced from inline asm above it. */
void C2_HOOK_FASTCALL FUN_0049a410(void);
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, tPlayer_ID pPlayer, void* pNotifyFail);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_FASTCALL CheckForDisappearees(void);
extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);
extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);
extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL DisposeCarN(int pPlayer_index);
extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address);
extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);
extern void C2_HOOK_FASTCALL NetService(int pIn_race);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PDGetMicroseconds(void);
extern void C2_HOOK_FASTCALL PDNetDisposePlayer(void);
extern int C2_HOOK_FASTCALL PDNetGetNextJoinGame(tNet_game_details* pGame, int pIndex);
extern tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** gRemote_addr);
extern int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName);
extern int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);
extern void C2_HOOK_FASTCALL PDNetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address);
extern void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);
extern void C2_HOOK_FASTCALL SignalToStartRace(void);
extern void __cdecl _ftol();
extern void C2_HOOK_CDECL dr_dprintf();
extern char gCamera_incident_direction[];
extern char gCurrent_dropped_actor_index[];
extern char gFLOAT_0058a420[];
extern tBrender_storage gOur_car_storage_space;
extern br_pixelmap* gScreen;
extern tTimer gTimers[19];
extern int gTimers_max_index;
extern int gTimers_stack[19];
extern int gTimers_stack_size;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int gNet_score_flash_state;
extern tU32 gNet_score_last_flash;
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34Translate();
extern void C2_HOOK_CDECL BrZbSceneRender();
extern void C2_HOOK_CDECL BrZbsSceneRender();
extern void C2_HOOK_FASTCALL BroadcastStatus(void);
extern void C2_HOOK_FASTCALL CancelPendingCunningStunt(void);
extern void C2_HOOK_FASTCALL CheckForPendingStartRace(void);
extern int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);
extern void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);
extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);
extern void C2_HOOK_FASTCALL DoNextJoinPoll(void);
extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
extern void C2_HOOK_FASTCALL ExpandMatrix34(br_matrix34* pMatrix, const tCompressed_matrix3* pCompressed, int pInactive);
extern void C2_HOOK_FASTCALL ExplodeMeCarUpMeOldMatey(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL FUN_0043cd60(void* pPayload);
extern void C2_HOOK_FASTCALL FUN_0043d5f0(const tU8* pMessage);
extern void C2_HOOK_FASTCALL FUN_0043d810(tU8* pMessage);
extern void C2_HOOK_FASTCALL FUN_00497860(tNet_message* pMessage, void* pSender_address);
extern void C2_HOOK_FASTCALL FUN_004984c0(tNet_message_chunk_car_reposition* pContents);
extern void C2_HOOK_FASTCALL FUN_0049b7e0(tU32 pPlayer_id);
extern void C2_HOOK_FASTCALL FUN_0049bad0(void* pPayload, void* pMessage, int pReceive_time);
extern void C2_HOOK_FASTCALL FUN_004a48d0(void* pPayload, void* pMessage);
extern void C2_HOOK_FASTCALL FUN_004f6320(tSmashable_noncar_action* pAction);
extern void C2_HOOK_FASTCALL FillInThisPlayer(tNet_game_details* pGame, tNet_game_player_info* pPlayer, int pCar_index, int pHost);
extern int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush);
extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);
extern void C2_HOOK_FASTCALL NetPlayersChanged(int pPlayer_count, tNet_game_player_info* pPlayers);
extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);
extern int C2_HOOK_FASTCALL PDNetHostGame(tNet_game_details* pDetails, char* pHost_name, void** pHost_address);
extern int C2_HOOK_FASTCALL PickARandomCar(void);
extern void C2_HOOK_FASTCALL PolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);
extern void C2_HOOK_FASTCALL QueueNetSmash(tQueued_net_smash* pSmash);
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern void C2_HOOK_FASTCALL ReceivedCarData(tNet_message_chunk_car_data* pContents);
extern void C2_HOOK_FASTCALL ReceivedCopInfo(void);
extern void C2_HOOK_FASTCALL ReceivedCrush(tU8* pContents);
extern void C2_HOOK_FASTCALL ReceivedCrushDetach(tU8* pContents);
extern void C2_HOOK_FASTCALL ReceivedCrushReattachBit(tNet_message_chunk_crush_reattach_bit* pContents);
extern void C2_HOOK_FASTCALL ReceivedCrushSync(tNet_message_chunk_car_crush_sync* pContents);
extern void C2_HOOK_FASTCALL ReceivedDetachedNonCarData(tU8* pMessage);
extern void C2_HOOK_FASTCALL ReceivedFullDetachBit(tU8* pParam);
extern void C2_HOOK_FASTCALL ReceivedNonCar(tNet_message_chunk_non_car* pContents);
extern void C2_HOOK_FASTCALL ReceivedNonCarPosition(tNet_message_chunk_non_car_position* pContents);
extern void C2_HOOK_FASTCALL ReceivedOilSpill(tNet_message_chunk* pContents);
extern void C2_HOOK_FASTCALL ReceivedPowerup(tNet_message_chunk_powerup* pMessage);
extern void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents);
extern int C2_HOOK_FASTCALL ReceivedRepulseOpponents(tNet_message_chunk_powerup* pMessage);
extern void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ReceiveMineUpdate(tNet_message_chunk_mine_explode* pChunk);
extern void C2_HOOK_FASTCALL RemoveNetPlayer(tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL SendCurrentPowerups(void);
extern void C2_HOOK_FASTCALL SetNetAvailability(tNet_game_options* pOptions);
extern int C2_HOOK_CDECL SortNetScoresAscending();
extern int C2_HOOK_CDECL SortNetScoresDescending();
extern void C2_HOOK_FASTCALL TimerString(tU32 pTime, char* pStr, undefined4 pArg3, int pFudge_colon, int pFloat);
extern void C2_HOOK_FASTCALL ToggleFlapsFromNetwork(tNet_message_chunk* pChunk);
extern br_pixelmap* gBack_screen;
extern tCar_icon gCar_icons[128];
extern br_actor* gCar_icons_model_actor;
extern tNet_game_options* gChosen_net_game_options;
extern br_pixelmap* gDepth_buffer;
extern int gInterface_within_race_mode;
extern int gNumber_of_racers;
extern int gPending_race;
extern tU32 gRace_finished;
extern tU32 gRace_start;
extern int gReceived_car_details;
extern br_pixelmap* gRender_screen;
extern br_pixelmap* gTexture_maps[1024];
extern tU32 gWait_for_it;
#endif
#ifdef CARPOCALYPSE2_MATCHING
const char gAsmString_0058f418[] = "%d";
const char gAsmString_0058fd6c[] = "%s %d";
const char gAsmString_0059101c[] = "%s";
const char gAsmString_00655b3c[] = "%d%%";
const char gAsmString_00659c74[] = "Net HUD Camera";
const char gAsmString_00659c84[] = "NetScore Material";
const char gAsmString_00659c98[] = "Cannot find headup graphic %s";
const char gAsmString_00659cb8[] = "NETBOX%i.PIX";
const char gAsmString_00659cc8[] = "NetScore Model";
const char gAsmString_00659cd8[] = "Out of memory for NetPlay";
const char gAsmString_00659cf4[] = "%s -%d-";
const char gAsmString_00659cfc[] = "%d-%d";
const char gAsmString_00659d04[] = "%d left";
const char gAsmString_0065a148[] = "Trying to send network message to self";
const char gAsmString_0065a170[] = "Broadcast stacks messed up";
const char gAsmString_0065a18c[] = "NetLeaveGame() Point 11";
const char gAsmString_0065a1a4[] = "NetLeaveGame() Point 10";
const char gAsmString_0065a1bc[] = "NetLeaveGame() Point 9";
const char gAsmString_0065a1d4[] = "NetLeaveGame() Point 8";
const char gAsmString_0065a1ec[] = "NetLeaveGame() Point 7";
const char gAsmString_0065a204[] = "NetLeaveGame() Point 6";
const char gAsmString_0065a21c[] = "NetLeaveGame() Point 5";
const char gAsmString_0065a234[] = "NetLeaveGame() Point 4";
const char gAsmString_0065a24c[] = "NetLeaveGame() Point 3";
const char gAsmString_0065a264[] = "NetLeaveGame() Point 2";
const char gAsmString_0065a27c[] = "NetLeaveGame() Point 1";
const char gAsmString_0065a294[] = "NetLeaveGame() START";
const char gAsmString_0065a2ac[] = "HOST";
// GLOBAL: CARMA2_HW 0x0065a2b4
const char gAsmString_0065a2b4[] = "!TEMP!";
const char gAsmString_0065a2bc[] = " type %d";
const char gAsmString_0065a2c8[] = "%s %s %s";
const char gAsmString_0065a2d4[] = "%s %s %s %s";
const char gAsmString_0065a2e0[] = "PLAYER";
const char gAsmString_0065a2e8[] = "Guarantee message not built proper";
const char gAsmString_0065a30c[] = "Guaranteed message not built correctly";
const char gAsmString_0065a334[] = "Guaranteed message not built properly";
const char gAsmString_0065a35c[] = "oops - guaranteed messages";
const char gAsmString_0065a378[] = "Dodgy message in guarantee list";
#endif
// FUNCTION: CARMA2_HW 0x0049d780

void C2_NAKED C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message_chunk* chunk;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (gProgram_state.racing) {
        NewTextHeadupSlot(4, 0, 3000, -4, pMessage);
    }
    chunk = NetStartBroadcastContents(0xd, 0);
    if (chunk == NULL) {
        return;
    }
    strcpy((char*)chunk + sizeof(tNet_message_chunk_header), pMessage);
    NetBroadcastContents(chunk);
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gNet_mode]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            test eax, eax
            mov dword ptr [esp + 0x14], edi
            je L0049d981
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L0049d7b7
            push edi
            push -4
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L0049d7b7:
            mov eax, dword ptr [gINT_00659d34]
            test eax, eax
            jl L0049d7ca
            mov ecx, offset gAsmString_0065a170
            call PDFatalError
    L0049d7ca:
            mov ecx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test ecx, ecx
            jle L0049d7e9
            mov edx, dword ptr [gThis_net_player_index]
    L0049d7dc:
            cmp eax, edx
            jne L0049d989
            inc eax
            cmp eax, ecx
            jl L0049d7dc
    L0049d7e9:
            xor ebp, ebp
    L0049d7eb:
            test ebp, ebp
            je L0049d981
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebp + 2]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            mov dword ptr [esp + 0x10], eax
            rep movsb
            mov ecx, dword ptr [gNumber_of_net_players]
            test ecx, ecx
            jle L0049d977
            mov ecx, dword ptr [gThis_net_player_index]
            mov ebx, offset gNet_players
    L0049d832:
            cmp eax, ecx
            je L0049d95e
            cmp eax, dword ptr [gINT_00659d34]
            je L0049d95e
            lea eax, [ecx + ecx*2]
            xor edx, edx
            mov dl, byte ptr [ebp]
            lea eax, [eax + eax*8]
            mov edi, edx
            shl eax, 1
            sub eax, ecx
            mov dword ptr [esp + 0x18], edi
            lea ecx, [eax*4 + gNet_players]
            cmp ebx, ecx
            jne L0049d870
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L0049d870:
            mov esi, dword ptr [ebx + 0xcc]
            test esi, esi
            je L0049d903
            xor edx, edx
            mov dx, word ptr [esi + 0x16]
            add edx, edi
            cmp edx, 0x200
            jle L0049d8ff
            mov eax, dword ptr [gCurrent_net_game]
            mov edi, dword ptr [ebx + 0x24]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L0049d8f3
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L0049d8f3
            mov ecx, offset gNet_players+0x24
    L0049d8c5:
            cmp dword ptr [ecx], edi
            je L0049d8d6
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049d8c5
            jmp L0049d8f3
    L0049d8d6:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, dword ptr [esp + 0x14]
            shl edx, 1
            sub edx, eax
            lea eax, [edx*4 + gNet_players]
            mov edx, esi
            push eax
            call PDNetSendMessageToAddress
    L0049d8f3:
            mov dword ptr [ebx + 0xcc], 0
            xor esi, esi
    L0049d8ff:
            test esi, esi
            jne L0049d921
    L0049d903:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [ebx + 0xcc], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L0049d921:
            mov ax, word ptr [esi + 0x16]
            mov edx, dword ptr [esp + 0x18]
            mov ecx, eax
            mov edi, esi
            and ecx, 0xffff
            add edx, eax
            add edi, ecx
            inc word ptr [esi + 0x14]
            xor ecx, ecx
            mov word ptr [esi + 0x16], dx
            mov cl, byte ptr [ebp]
            mov esi, ebp
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x10]
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gThis_net_player_index]
    L0049d95e:
            mov edx, dword ptr [gNumber_of_net_players]
            inc eax
            add ebx, 0xd4
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L0049d832
    L0049d977:
            mov dword ptr [gINT_00659d34], 0xffffffff
    L0049d981:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L0049d989:
            lea ecx, [eax + eax*2]
            xor edx, edx
            mov dword ptr [gINT_00659d34], eax
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov esi, dword ptr [ecx*4 + gNet_players+0x24]
            mov cl, 0xd
            call NetGetContentsSize
            mov edx, dword ptr [gNumber_of_net_players]
            mov ebx, eax
            xor eax, eax
            mov dword ptr [esp + 0x10], ebx
            test edx, edx
            jle L0049dae6
            mov ecx, offset gNet_players+0x24
    L0049d9c3:
            cmp dword ptr [ecx], esi
            je L0049d9db
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049d9c3
            xor eax, eax
            mov ebp, eax
            jmp L0049d7eb
    L0049d9db:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ebp, [edx*4 + gNet_players]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            cmp ebp, edx
            jne L0049da10
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L0049da10:
            mov esi, dword ptr [ebp + 0xcc]
            test esi, esi
            je L0049daa1
            xor eax, eax
            mov ax, word ptr [esi + 0x16]
            add eax, ebx
            cmp eax, 0x200
            jle L0049da9d
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [ebp + 0x24]
            mov ebx, dword ptr [gCurrent_net_game]
            test eax, eax
            je L0049da8d
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L0049da8d
            mov ecx, offset gNet_players+0x24
    L0049da61:
            cmp dword ptr [ecx], edi
            je L0049da72
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049da61
            jmp L0049da8d
    L0049da72:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, ebx
            shl edx, 1
            sub edx, eax
            lea eax, [edx*4 + gNet_players]
            mov edx, esi
            push eax
            call PDNetSendMessageToAddress
    L0049da8d:
            mov ebx, dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x14]
            xor esi, esi
            mov dword ptr [ebp + 0xcc], esi
    L0049da9d:
            test esi, esi
            jne L0049dabf
    L0049daa1:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [ebp + 0xcc], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L0049dabf:
            mov cx, word ptr [esi + 0x16]
            mov eax, esi
            mov edx, ecx
            add ecx, ebx
            and edx, 0xffff
            mov word ptr [esi + 0x16], cx
            add eax, edx
            inc word ptr [esi + 0x14]
            mov ebp, eax
            mov byte ptr [eax + 1], 0xd
            mov byte ptr [eax], bl
            jmp L0049d7eb
    L0049dae6:
            xor eax, eax
            mov ebp, eax
            jmp L0049d7eb
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049d430
void C2_NAKED C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message_chunk* chunk;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    chunk = NetStartBroadcastContents(0xd, 0);
    if (chunk == NULL) {
        return;
    }
    strcpy((char*)chunk + sizeof(tNet_message_chunk_header), pMessage);
    NetBroadcastContents(chunk);
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gNet_mode]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x14], ecx
            je L0049d616
            mov eax, dword ptr [gINT_00659d34]
            test eax, eax
            jl L0049d45b
            mov ecx, offset gAsmString_0065a170
            call PDFatalError
    L0049d45b:
            mov ecx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test ecx, ecx
            jle L0049d47a
            mov edx, dword ptr [gThis_net_player_index]
    L0049d46d:
            cmp eax, edx
            jne L0049d61e
            inc eax
            cmp eax, ecx
            jl L0049d46d
    L0049d47a:
            xor ebp, ebp
    L0049d47c:
            test ebp, ebp
            je L0049d616
            mov edi, dword ptr [esp + 0x14]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 2]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            mov dword ptr [esp + 0x10], eax
            rep movsb
            mov ecx, dword ptr [gNumber_of_net_players]
            test ecx, ecx
            jle L0049d60c
            mov ecx, dword ptr [gThis_net_player_index]
            mov ebx, offset gNet_players
    L0049d4c7:
            cmp eax, ecx
            je L0049d5f3
            cmp eax, dword ptr [gINT_00659d34]
            je L0049d5f3
            lea eax, [ecx + ecx*2]
            xor edx, edx
            mov dl, byte ptr [ebp]
            lea eax, [eax + eax*8]
            mov edi, edx
            shl eax, 1
            sub eax, ecx
            mov dword ptr [esp + 0x18], edi
            lea ecx, [eax*4 + gNet_players]
            cmp ebx, ecx
            jne L0049d505
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L0049d505:
            mov esi, dword ptr [ebx + 0xcc]
            test esi, esi
            je L0049d598
            xor edx, edx
            mov dx, word ptr [esi + 0x16]
            add edx, edi
            cmp edx, 0x200
            jle L0049d594
            mov eax, dword ptr [gCurrent_net_game]
            mov edi, dword ptr [ebx + 0x24]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L0049d588
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L0049d588
            mov ecx, offset gNet_players+0x24
    L0049d55a:
            cmp dword ptr [ecx], edi
            je L0049d56b
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049d55a
            jmp L0049d588
    L0049d56b:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, dword ptr [esp + 0x14]
            shl edx, 1
            sub edx, eax
            lea eax, [edx*4 + gNet_players]
            mov edx, esi
            push eax
            call PDNetSendMessageToAddress
    L0049d588:
            mov dword ptr [ebx + 0xcc], 0
            xor esi, esi
    L0049d594:
            test esi, esi
            jne L0049d5b6
    L0049d598:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [ebx + 0xcc], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L0049d5b6:
            mov ax, word ptr [esi + 0x16]
            mov edx, dword ptr [esp + 0x18]
            mov ecx, eax
            mov edi, esi
            and ecx, 0xffff
            add edx, eax
            add edi, ecx
            inc word ptr [esi + 0x14]
            xor ecx, ecx
            mov word ptr [esi + 0x16], dx
            mov cl, byte ptr [ebp]
            mov esi, ebp
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x10]
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gThis_net_player_index]
    L0049d5f3:
            mov edx, dword ptr [gNumber_of_net_players]
            inc eax
            add ebx, 0xd4
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L0049d4c7
    L0049d60c:
            mov dword ptr [gINT_00659d34], 0xffffffff
    L0049d616:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L0049d61e:
            lea ecx, [eax + eax*2]
            xor edx, edx
            mov dword ptr [gINT_00659d34], eax
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov esi, dword ptr [ecx*4 + gNet_players+0x24]
            mov cl, 0xd
            call NetGetContentsSize
            mov edx, dword ptr [gNumber_of_net_players]
            mov ebx, eax
            xor eax, eax
            mov dword ptr [esp + 0x10], ebx
            test edx, edx
            jle L0049d773
            mov ecx, offset gNet_players+0x24
    L0049d658:
            cmp dword ptr [ecx], esi
            je L0049d670
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049d658
            xor eax, eax
            mov ebp, eax
            jmp L0049d47c
    L0049d670:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ebp, [edx*4 + gNet_players]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            cmp ebp, edx
            jne L0049d6a5
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L0049d6a5:
            mov esi, dword ptr [ebp + 0xcc]
            test esi, esi
            je L0049d72e
            xor eax, eax
            mov ax, word ptr [esi + 0x16]
            add eax, ebx
            cmp eax, 0x200
            jle L0049d72a
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [ebp + 0x24]
            mov ebx, dword ptr [gCurrent_net_game]
            test eax, eax
            je L0049d71e
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L0049d71e
            mov ecx, offset gNet_players+0x24
    L0049d6f2:
            cmp dword ptr [ecx], edi
            je L0049d703
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049d6f2
            jmp L0049d71e
    L0049d703:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, ebx
            shl edx, 1
            sub edx, eax
            lea eax, [edx*4 + gNet_players]
            mov edx, esi
            push eax
            call PDNetSendMessageToAddress
    L0049d71e:
            mov ebx, dword ptr [esp + 0x10]
            xor esi, esi
            mov dword ptr [ebp + 0xcc], esi
    L0049d72a:
            test esi, esi
            jne L0049d74c
    L0049d72e:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [ebp + 0xcc], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L0049d74c:
            mov cx, word ptr [esi + 0x16]
            mov eax, esi
            mov edx, ecx
            add ecx, ebx
            and edx, 0xffff
            mov word ptr [esi + 0x16], cx
            add eax, edx
            inc word ptr [esi + 0x14]
            mov ebp, eax
            mov byte ptr [eax + 1], 0xd
            mov byte ptr [eax], bl
            jmp L0049d47c
    L0049d773:
            xor eax, eax
            mov ebp, eax
            jmp L0049d47c
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049daf0
void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id) {
    if (gNet_mode != eNet_mode_none) {
        if (pPlayer_id == gLocal_net_ID) {
            if (gProgram_state.racing) {
                NewTextHeadupSlot(4, 0, 3000, -4, pMessage);
                return;
            }
        } else {
            int size_headup;
            int size_guaranteed;
            int size_headup_again;
            int full_size;
            tNet_message* message;

            size_headup = NetGetContentsSize(0xd, 0);
            size_guaranteed = NetGetContentsSize(5, 0);
            size_headup_again = NetGetContentsSize(0xd, 0);
            full_size = size_headup_again + 0x18 + size_guaranteed;
            message = NetAllocateMessage(full_size);
            if (message != NULL) {
                message->contents.raw.header.size = (tU8)size_headup;
                message->header.field_0x14 = 0xff;
                message->header.field_0x16 = (tU16)full_size;
                message->contents.raw.header.type = 0xd;
                ((tU8*)message)[0x19 + (tU8)size_headup] = 5;
                ((tU8*)message)[0x18 + message->contents.raw.header.size] = (tU8)size_guaranteed;
            }
            strcpy((char*)message + 0x1a, pMessage);
            NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, pPlayer_id, 0);
        }
    }
}

void C2_HOOK_FASTCALL GetCheckSum(tNet_message* pMessage) {

}

// FUNCTION: CARMA2_HW 0x0049d3e0
void C2_HOOK_FASTCALL DisableNetService(void) {
    gNet_service_disable = 1;
}

// FUNCTION: CARMA2_HW 0x0049d3f0
void C2_HOOK_FASTCALL ReenableNetService(void) {
    gNet_service_disable = 0;
}

// FUNCTION: CARMA2_HW 0x004a69b0
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].car == pCar) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a5710
void C2_NAKED C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int size_a;
    int size_b;
    int size_c;
    int total_size;
    int fill;
    int result;
    tNet_message* message;
    tNet_game_details* game;

    gSynch_race_start = 0;
    size_a = NetGetContentsSize(0xa, 0);
    size_b = NetGetContentsSize(5, 0);
    size_c = NetGetContentsSize(0xa, 0);
    total_size = size_c + size_b + sizeof(tNet_message_header);
    message = NetAllocateMessage(total_size);
    fill = 0xff;
    if (message != NULL) {
        size_a &= fill;
        message->header.field_0x14 = fill;
        message->header.field_0x16 = (tU16)total_size;
        message->contents.raw.header.size = (tU8)size_a;
        message->contents.raw.header.type = 0xa;
        ((tU8*)message)[0x19 + size_a] = 5;
        ((tU8*)message)[0x18 + message->contents.raw.header.size] = (tU8)size_b;
    }
    *(tU32*)&message->contents.raw.data[2] = pReason;
    game = gCurrent_net_game;
    result = 0;
    if (gNumber_of_net_players == 1) {
        ((tU8*)message)[0x19] = (tU8)fill;
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            result |= NetGuaranteedSendMessageToPlayer(game, message, gNet_players[i].ID, 0);
        }
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            mov ebp, edx
            push esi
            push edi
            xor edx, edx
            mov cl, 0xa
            mov dword ptr [gSynch_race_start], 0
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            xor edx, edx
            mov cl, 0xa
            mov dword ptr [esp + 0x10], eax
            call NetGetContentsSize
            mov ecx, dword ptr [esp + 0x10]
            lea edi, [eax + ecx + 0x18]
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            mov ecx, 0xff
            test esi, esi
            je L004a577f
            mov al, byte ptr [esp + 0x10]
            mov byte ptr [esi + 0x18], bl
            and ebx, ecx
            mov word ptr [esi + 0x14], cx
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 0xa
            xor edx, edx
            mov byte ptr [esi + ebx + 0x19], 5
            mov dl, byte ptr [esi + 0x18]
            mov byte ptr [esi + edx + 0x18], al
    L004a577f:
            mov dword ptr [esi + 0x1c], ebp
            mov eax, dword ptr [gNumber_of_net_players]
            mov edx, dword ptr [gCurrent_net_game]
            xor ebp, ebp
            cmp eax, 1
            mov dword ptr [esp + 0x10], edx
            jne L004a57a1
            mov byte ptr [esi + 0x19], cl
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004a57a1:
            xor ebx, ebx
            test eax, eax
            jle L004a57d6
            mov edi, offset gNet_players+0x24
    L004a57ac:
            cmp ebx, dword ptr [gThis_net_player_index]
            je L004a57cb
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [esp + 0x10]
            push 0
            push eax
            mov edx, esi
            call NetGuaranteedSendMessageToPlayer
            or ebp, eax
            mov eax, dword ptr [gNumber_of_net_players]
    L004a57cb:
            inc ebx
            add edi, 0xd4
            cmp ebx, eax
            jl L004a57ac
    L004a57d6:
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a0080
void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there) {
    tU32 start_time;
    const char* s;
    int size;
    int restore_screen;
    char* pixels_copy;
    char* palette_copy;

    if (!(pLeave_it_up_there || (gProgram_state.racing && !gInterface_within_race_mode))) {
        size = gBack_screen->height * gBack_screen->row_bytes;
        pixels_copy = BrMemAllocate(size, 0xb0);
        palette_copy = BrMemAllocate(0x400, 0xb1);
        memcpy(pixels_copy, gBack_screen->pixels, size);
        memcpy(palette_copy, gCurrent_palette_pixels, 0x400);
        restore_screen = 1;
    } else {
        restore_screen = 0;
    }
    FadePaletteDown();
    LoadFont(4);
    ClearEntireScreen();
    s = GetMiscString(pStr_index);
    OoerrIveGotTextInMeBoxMissus(4, s, gBack_screen,
        gGraf_specs[gGraf_spec_index].total_width / 10,
        gGraf_specs[gGraf_spec_index].total_height / 2 - gFonts[4].height,
        gGraf_specs[gGraf_spec_index].total_width * 9 / 10,
        gGraf_specs[gGraf_spec_index].total_height - 64, 1);
    PDScreenBufferSwap(0);
    EnsureRenderPalette();
    EnsurePaletteUp();
    if (pLeave_it_up_there) {
    } else {
        start_time = PDGetTotalTime();
        while (PDGetTotalTime() - start_time < 3000) {
        }
        FadePaletteDown();
        if (restore_screen) {
            memcpy(gBack_screen->pixels, pixels_copy, size);
            memcpy(gCurrent_palette_pixels, palette_copy, 0x400);
            if (pixels_copy != NULL) {
                BrMemFree(pixels_copy);
            }
            if (palette_copy != NULL) {
                BrMemFree(palette_copy);
            }
            PDScreenBufferSwap(0);
            FadePaletteUp();
        } else {
            ClearEntireScreen();
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049dc00
void C2_NAKED C2_HOOK_FASTCALL NetEndJoinList(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gJoin_list_mode) {
        gJoin_list_mode = 0;
        DisposeCurrentJoinPollGame();
        LeaveTempGame();
        PDNetEndJoinList();
    }
#else
    __asm {
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L0049dc66
            mov ecx, dword ptr [gCurrent_join_poll_game]
            mov dword ptr [gJoin_list_mode], 0
            test ecx, ecx
            je L0049dc4d
            push ecx
            call BrMemFree
            xor ecx, ecx
            add esp, 4
            test ecx, ecx
            mov dword ptr [gCurrent_join_poll_game], ecx
            je L0049dc4d
            cmp dword ptr [gNet_mode], 2
            jne L0049dc48
            mov ecx, dword ptr [gCurrent_net_game]
            call PDNetHostFinishGame
            jmp L0049dc4d
    L0049dc48:
            call PDNetLeaveGame
    L0049dc4d:
            mov dword ptr [gTime_for_next_one], 1
            mov dword ptr [gCurrent_join_poll_game], 0
            jmp PDNetEndJoinList
    L0049dc66:
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

// FUNCTION: CARMA2_HW 0x0049dc80
void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {
    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

// FUNCTION: CARMA2_HW 0x0049e210
C2_NAKED tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
    return NULL;
#else
    __asm {
            sub esp, 0xe0
            push ebx
            push ebp
            push esi
            push edi
            push 0xb5
            mov dword ptr [esp + 0x1c], edx
            mov dword ptr [esp + 0x14], ecx
            push 0x78
            call BrMemAllocate
            mov edi, dword ptr [esp + 0x100]
            mov ebp, eax
            xor ebx, ebx
            add esp, 8
            cmp byte ptr [edi], bl
            jne L0049e253
            push offset gAsmString_0065a2ac
            push offset gAsmString_0059101c
            push edi
            call sprintf
            add esp, 0xc
    L0049e253:
            lea eax, [esp + 0x14]
            mov edx, edi
            push eax
            mov ecx, ebp
            mov dword ptr [gNet_service_disable], 1
            call PDNetHostGame
            test eax, eax
            je L0049e376
            or ecx, 0xffffffff
            xor eax, eax
            mov dword ptr [gCurrent_net_game], ebp
            mov dword ptr [gSynch_race_start], ebx
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebp + 0xe]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, ebp
            _emit 0xe8
        _emit 0x8a
        _emit 0xbe
        _emit 0x07
        _emit 0x00
            mov esi, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0xf4]
            mov dword ptr [ebp + 0x30], eax
            mov eax, 1
            lea edi, [ebp + 0x44]
            mov ecx, 0xc
            mov dword ptr [ebp + 0x34], eax
            push eax
            rep movsd
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [ebp + 0x40], ebx
            mov dword ptr [ebp + 0x74], ecx
            mov dword ptr [ebp + 0x38], edx
            mov edx, dword ptr [esp + 0x100]
            mov dword ptr [ebp + 0x3c], eax
            mov dword ptr [gINT_00690c30], ebx
            mov dword ptr [gINT_0068d968], ebx
            mov dword ptr [gNumber_of_net_players], ebx
            mov dword ptr [gThis_net_player_index], ebx
            mov ecx, dword ptr [ebp + 0x30]
            push edx
            mov dword ptr [gLocal_net_ID], ecx
            lea edx, [esp + 0x24]
            mov ecx, ebp
            call FillInThisPlayer
            lea ecx, [esp + 0x1c]
            mov edx, dword ptr [esp + 0x14]
            call PDNetSetPlayerSystemInfo
            lea edx, [esp + 0x1c]
            mov ecx, 1
            call NetPlayersChanged
            mov esi, offset gNet_players+0x48
    L0049e32b:
            call PDGetTotalTime
            mov dword ptr [esi - 0x34], eax
            mov dword ptr [esi], 2
            add esi, 0xd4
            cmp esi, offset gViewable_car_list+0x18
            jl L0049e32b
            mov eax, dword ptr [gNet_players+0xc8]
            mov dword ptr [gNet_mode], 2
            mov ecx, dword ptr [eax + 0x18d4]
            call InitNetworkShapesStuff
            mov dword ptr [gDont_allow_joiners], ebx
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xe0
            ret 0xc
    L0049e376:
            cmp ebp, ebx
            je L0049e383
            push ebp
            call BrMemFree
            add esp, 4
    L0049e383:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0xe0
            ret 0xc
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

// FUNCTION: CARMA2_HW 0x0049e970
int C2_NAKED C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
    return 0;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            call PDNetJoinGame
            xor esi, esi
            mov dword ptr [esp + 0x10], eax
            cmp eax, esi
            jne L0049ec56
            mov ecx, edi
            mov dword ptr [gNet_service_disable], 1
            mov dword ptr [gINT_00690c30], esi
            mov dword ptr [gNet_mode], 3
            mov dword ptr [gCurrent_net_game], edi
            _emit 0xe8
        _emit 0x7d
        _emit 0xb7
        _emit 0x07
        _emit 0x00
            xor edx, edx
            mov cl, 2
            mov dword ptr [gLocal_net_ID], eax
            mov dword ptr [gNumber_of_net_players], esi
            mov dword ptr [gINT_0068d974], esi
            mov dword ptr [gINT_0068d988], esi
            mov dword ptr [gINT_00690234], esi
            mov dword ptr [gINT_0068d968], esi
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            xor edx, edx
            mov cl, 2
            mov dword ptr [esp + 0x14], eax
            call NetGetContentsSize
            mov ecx, dword ptr [esp + 0x14]
            lea ebp, [eax + ecx + 0x18]
            mov ecx, ebp
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L0049ea35
            mov al, byte ptr [esp + 0x14]
            mov byte ptr [esi + 0x18], bl
            and ebx, 0xff
            mov word ptr [esi + 0x14], 0xff
            mov word ptr [esi + 0x16], bp
            mov byte ptr [esi + 0x19], 2
            xor edx, edx
            mov byte ptr [ebx + esi + 0x19], 5
            mov dl, byte ptr [esi + 0x18]
            mov byte ptr [esi + edx + 0x18], al
    L0049ea35:
            mov ecx, dword ptr [esp + 0x1c]
            push 0
            push ecx
            lea edx, [esi + 0x1c]
            mov ecx, edi
            call FillInThisPlayer
            mov eax, dword ptr [gNet_mode]
            xor ebx, ebx
            cmp eax, ebx
            mov dword ptr [gNet_service_disable], ebx
            jne L0049ea5f
            cmp dword ptr [gJoin_list_mode], ebx
            je L0049ea85
    L0049ea5f:
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], edx
            call PDGetTotalTime
            mov edx, edi
            mov ecx, esi
            push ebx
            push ebx
            mov dword ptr [esi + 0x10], eax
            call AddToGuaranteeList
            mov edx, esi
            mov ecx, edi
            push edi
            call PDNetSendMessageToAddress
    L0049ea85:
            call PDGetTotalTime
            mov ebp, eax
    L0049ea8c:
            cmp dword ptr [gIn_net_service], ebx
            jne L0049eb9f
            cmp dword ptr [gNet_service_disable], ebx
            jne L0049eb9f
            call PDGetTotalTime
            mov edi, eax
            mov eax, dword ptr [gJoin_list_mode]
            cmp eax, ebx
            mov dword ptr [gIn_net_service], 1
            je L0049eaf6
            mov eax, dword ptr [gDynamic_messages]
            cmp eax, ebx
            je L0049eaea
    L0049eac7:
            mov ecx, dword ptr [gMessage_header_size]
            cmp byte ptr [eax + ecx + 0x1d], 0xff
            jne L0049eaea
            mov esi, dword ptr [eax]
            push eax
            call BrMemFree
            add esp, 4
            mov eax, esi
            cmp esi, ebx
            mov dword ptr [gDynamic_messages], eax
            jne L0049eac7
    L0049eaea:
            call DoNextJoinPoll
            call NetReceiveAndProcessMessages
            jmp L0049eb67
    L0049eaf6:
            cmp dword ptr [gNet_mode], ebx
            je L0049eb67
            mov eax, dword ptr [gDynamic_messages]
            cmp eax, ebx
            je L0049eb2a
    L0049eb07:
            mov edx, dword ptr [gMessage_header_size]
            cmp byte ptr [eax + edx + 0x1d], 0xff
            jne L0049eb2a
            mov esi, dword ptr [eax]
            push eax
            call BrMemFree
            add esp, 4
            mov eax, esi
            cmp esi, ebx
            mov dword ptr [gDynamic_messages], eax
            jne L0049eb07
    L0049eb2a:
            call NetReceiveAndProcessMessages
            mov edx, dword ptr [gLast_status_broadcast]
            mov eax, edi
            sub eax, edx
            cmp eax, 0x3e8
            jbe L0049eb4f
            call PDGetTotalTime
            mov dword ptr [gLast_status_broadcast], eax
            call BroadcastStatus
    L0049eb4f:
            call CheckForDisappearees
            cmp dword ptr [gNet_mode], 2
            jne L0049eb67
            call CheckForPendingStartRace
            call CheckForNeedyEnvironmentRecipients
    L0049eb67:
            cmp dword ptr [gJoin_list_mode], ebx
            jne L0049eb77
            cmp dword ptr [gNet_mode], ebx
            je L0049eb99
    L0049eb77:
            call ResendGuaranteedMessages
            mov ecx, dword ptr [gLast_flush_message]
            add ecx, 0xc8
            cmp edi, ecx
            ja L0049eb94
            cmp dword ptr [gProgram_state+0x28], ebx
            jne L0049eb99
    L0049eb94:
            call NetSendMessageStacks
    L0049eb99:
            mov dword ptr [gIn_net_service], ebx
    L0049eb9f:
            cmp dword ptr [gNumber_of_net_players], ebx
            jne L0049ebc9
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x7530
            jae L0049ebc9
            cmp dword ptr [gINT_0068d988], ebx
            jne L0049ebc9
            cmp dword ptr [gINT_0068d968], ebx
            je L0049ea8c
    L0049ebc9:
            mov dword ptr [gNet_service_disable], 1
            mov esi, offset gNet_players+0x48
    L0049ebd8:
            call PDGetTotalTime
            mov dword ptr [esi - 0x34], eax
            mov dword ptr [esi], 2
            add esi, 0xd4
            cmp esi, offset gViewable_car_list+0x18
            jl L0049ebd8
            cmp dword ptr [gNumber_of_net_players], ebx
            jne L0049ec56
            mov eax, dword ptr [gINT_0068d988]
            mov dword ptr [gNet_service_disable], ebx
            cmp eax, ebx
            je L0049ec29
            cmp dword ptr [gINT_00690234], ebx
            je L0049ec29
            mov dword ptr [esp + 0x10], 0xfffffffc
            mov eax, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 4
    L0049ec29:
            mov ecx, dword ptr [gCurrent_net_game]
            mov dword ptr [gNet_mode], ebx
            cmp ecx, ebx
            je L0049ec47
            push ecx
            call BrMemFree
            mov eax, dword ptr [gINT_0068d988]
            add esp, 4
    L0049ec47:
            neg eax
            sbb eax, eax
            mov dword ptr [gCurrent_net_game], ebx
            dec eax
            mov dword ptr [esp + 0x10], eax
    L0049ec56:
            mov eax, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049dbd0
void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc) {
    gAdd_proc = pAdd_proc;
    gJoin_list_mode = 1;
    gBastard_has_answered = 0;
    gTime_for_next_one = 1;
    gJoin_poll_index = 0;
    gCurrent_join_poll_game = NULL;
    PDNetStartProducingJoinList();
}

// FUNCTION: CARMA2_HW 0x00468750
void C2_HOOK_FASTCALL InitNetStorageSpace(void) {
    int i;

    for (i = 0; i < kMax_netplayers; i++) {
        gCurrent_race.opponent_list[i].car_spec = NULL;
    }
    gNet_storage_space_initialized = 1;
    InitialiseStorageSpace(0, &gNet_cars_storage_space, 2000, 50, 2000, 2000, 50);
}

void C2_HOOK_FASTCALL DisposeNetStorageSpace(void) {

    if (gNet_storage_space_initialized) {
        DisposeStorageSpace(&gNet_cars_storage_space);
    }
    gNet_storage_space_initialized = 0;
}

// FUNCTION: CARMA2_HW 0x0049f680
C2_NAKED tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message* the_message;
    tU32 contents_size;
    tU32 guarantee_size;
    tU32 the_size;

    C2_HOOK_BUG_ON(offsetof(tNet_message, contents) != 0x18);

    contents_size = NetGetContentsSize(pNet_message_type, pOption);
    guarantee_size = NetGetContentsSize(eNet_message_chunk_type_5, 0);
    the_size = NetGetContentsSize(pNet_message_type, pOption) + offsetof(tNet_message, contents) + guarantee_size;
    the_message = NetAllocateMessage(the_size);
    if (the_message != NULL) {
        the_message->contents.raw.header.size = (tU8)contents_size;
        the_message->header.field_0x14 = 0xff;
        the_message->header.field_0x16 = (tU16)the_size;
        the_message->contents.raw.header.type = pNet_message_type;
        ((tNet_message_guaranteed*)((tU8*)&the_message->contents + the_message->contents.raw.header.size))->header.type = 5;
        ((tNet_message_guaranteed*)((tU8*)&the_message->contents + the_message->contents.raw.header.size))->header.size = (tU8)guarantee_size;
    }
    return the_message;
#else
    __asm {
            sub esp, 8
            push ebx
            push esi
            mov esi, edx
            mov byte ptr [esp + 0xb], cl
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            mov cl, byte ptr [esp + 0xb]
            mov edx, esi
            mov dword ptr [esp + 0xc], eax
            call NetGetContentsSize
            mov ecx, dword ptr [esp + 0xc]
            lea esi, [eax + ecx + 0x18]
            mov ecx, esi
            call NetAllocateMessage
            test eax, eax
            je L0049f6e9
            mov dl, byte ptr [esp + 0xb]
            mov byte ptr [eax + 0x18], bl
            and ebx, 0xff
            mov word ptr [eax + 0x14], 0xff
            mov word ptr [eax + 0x16], si
            mov byte ptr [eax + 0x19], dl
            mov dl, byte ptr [esp + 0xc]
            xor ecx, ecx
            mov byte ptr [eax + ebx + 0x19], 5
            mov cl, byte ptr [eax + 0x18]
            mov byte ptr [eax + ecx + 0x18], dl
    L0049f6e9:
            pop esi
            pop ebx
            add esp, 8
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a5c80
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    NetGuaranteedSendMessageToPlayer(pDetails, pMessage, gNet_players[0].ID, (undefined4)pNotifyFail);
}

// FUNCTION: CARMA2_HW 0x0049ff70
C2_NAKED tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message* message;

    Timers_Push(TIMER_LLR);
    message = PDNetGetNextMessage(pDetails, pSender_address);
    Timers_Pop(TIMER_LLR);
    return message;
#else
    __asm {
            mov eax, dword ptr [gTimers_stack_size]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov ebx, edx
            mov ebp, ecx
            mov dword ptr [eax*4 + gTimers_stack], 9
            je L0049ffb0
            mov eax, dword ptr [eax*4 + 0x6b75dc]
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor ecx, ecx
            mov cl, byte ptr [esi + gTimers+0x8]
            jmp L0049ffcc
    L0049ffb0:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
    L0049ffcc:
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov edx, ebx
            inc edi
            mov ecx, ebp
            mov dword ptr [gTimers+0x108], eax
            mov dword ptr [gTimers_stack_size], edi
            call PDNetGetNextMessage
            mov edi, eax
            mov eax, dword ptr [gTimers+0x104]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x100]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x108]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a004e
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            jmp L004a0059
    L004a004e:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
    L004a0059:
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov eax, dword ptr [gTimers_stack_size]
            dec eax
            mov dword ptr [gTimers_stack_size], eax
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
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

// FUNCTION: CARMA2_HW 0x004a0260
void C2_NAKED C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x11c
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            mov dword ptr [esp + 0x20], edx
            cmp word ptr [ebp + 0x14], 0xff
            lea ebx, [ebp + 0x18]
            mov dword ptr [esp + 0x14], ebp
            mov dword ptr [esp + 0x10], ebx
            jne L004a0289
            mov word ptr [ebp + 0x14], 2
    L004a0289:
            mov eax, dword ptr [gTimers_stack_size]
            test eax, eax
            mov dword ptr [eax*4 + gTimers_stack], 0xf
            je L004a02c1
            mov eax, dword ptr [eax*4 + 0x6b75dc]
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor ecx, ecx
            mov cl, byte ptr [esi + gTimers+0x8]
            jmp L004a02dd
    L004a02c1:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
    L004a02dd:
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov dword ptr [gTimers+0x1b0], eax
            mov eax, dword ptr [gTimers_stack_size]
            inc eax
            mov esi, ebx
            xor edx, edx
            mov dword ptr [gTimers_stack_size], eax
            mov dl, byte ptr [esi]
            mov ax, word ptr [ebp + 0x14]
            add edx, esi
            cmp ax, 1
            jbe L004a03f2
            cmp ax, 1
            mov dword ptr [esp + 0x1c], 1
            jbe L004a03f2
            mov ecx, dword ptr [gINT_00690c50]
    L004a033e:
            cmp byte ptr [edx + 1], 5
            jne L004a03cf
            mov di, word ptr [edx + 2]
            xor ebp, ebp
            mov eax, offset gReceived_message_cache+0x8
    L004a0353:
            cmp di, word ptr [eax + 4]
            jne L004a0373
            mov ebx, dword ptr [esp + 0x14]
            mov ebx, dword ptr [ebx + 8]
            cmp ebx, dword ptr [eax]
            jne L004a036f
            movzx bx, byte ptr [esi + 1]
            cmp bx, word ptr [eax + 6]
            je L004a037f
    L004a036f:
            mov ebx, dword ptr [esp + 0x10]
    L004a0373:
            add eax, 8
            cmp eax, offset gUINT_0074a718
            jl L004a0353
            jmp L004a0388
    L004a037f:
            mov ebx, dword ptr [esp + 0x10]
            mov ebp, 1
    L004a0388:
            test ebp, ebp
            je L004a0398
            mov byte ptr [esi + 1], 0xfe
            mov ecx, dword ptr [gINT_00690c50]
            jmp L004a03cf
    L004a0398:
            mov eax, dword ptr [esp + 0x14]
            mov word ptr [ecx*8 + gReceived_message_cache+0xc], di
            inc ecx
            mov eax, dword ptr [eax + 8]
            cmp ecx, 0xf
            mov dword ptr [ecx*8 + gReceived_message_cache], eax
            movzx ax, byte ptr [esi + 1]
            mov word ptr [ecx*8 + gReceived_message_cache+0x6], ax
            mov dword ptr [gINT_00690c50], ecx
            jl L004a03cf
            xor ecx, ecx
            mov dword ptr [gINT_00690c50], ecx
    L004a03cf:
            mov ebp, dword ptr [esp + 0x14]
            xor eax, eax
            mov al, byte ptr [edx]
            mov esi, edx
            add edx, eax
            mov eax, dword ptr [esp + 0x1c]
            xor edi, edi
            inc eax
            mov di, word ptr [ebp + 0x14]
            mov dword ptr [esp + 0x1c], eax
            cmp eax, edi
            jl L004a033e
    L004a03f2:
            mov ecx, dword ptr [gTimers+0x1ac]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x1a8]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x1b0]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a044a
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a0469
    L004a044a:
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_max_index]
            mov ecx, edx
            shl ecx, 3
            sub ecx, edx
            mov dword ptr [ecx*4 + gTimers+0xc], eax
            mov ecx, dword ptr [gTimers_stack_size]
    L004a0469:
            dec ecx
            mov dword ptr [esp + 0x28], 0
            mov dword ptr [gTimers_stack_size], ecx
            cmp word ptr [ebp + 0x14], 0
            jbe L004a44e7
    L004a0483:
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            jne L004a04a4
            xor eax, eax
            mov al, byte ptr [ebx + 1]
            mov esi, dword ptr [eax*4 + gRace_only_flags]
            test esi, esi
            jne L004a44c2
            mov ebp, dword ptr [esp + 0x14]
    L004a04a4:
            mov eax, dword ptr [gOnly_receive_guarantee_replies]
            test eax, eax
            je L004a04b7
            cmp byte ptr [ebx + 1], 4
            jne L004a44c2
    L004a04b7:
            cmp byte ptr [ebx + 1], 8
            jb L004a050f
            mov eax, dword ptr [gNumber_of_net_players]
            mov edi, dword ptr [ebp + 8]
            xor esi, esi
            test eax, eax
            jle L004a44c2
            mov eax, offset gNet_players+0x24
    L004a04d4:
            cmp dword ptr [eax], edi
            je L004a04ed
            mov ebp, dword ptr [gNumber_of_net_players]
            inc esi
            add eax, 0xd4
            cmp esi, ebp
            jl L004a04d4
            jmp L004a44c2
    L004a04ed:
            call PDGetTotalTime
            lea ecx, [esi + esi*2]
            mov edx, dword ptr [gTimers_max_index]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, esi
            mov dword ptr [ecx*4 + gNet_players+0x14], eax
            mov ecx, dword ptr [gTimers_stack_size]
    L004a050f:
            xor eax, eax
            mov al, byte ptr [ebx + 1]
            cmp eax, 0x33
            ja L004a44c2
            jmp dword ptr [eax*4 + 0x4a4584]
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a0562
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a0584
    L004a0562:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a0584:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gDont_allow_joiners]
            inc ecx
            test eax, eax
            mov dword ptr [gTimers_stack_size], ecx
            jne L004a062c
            xor edx, edx
            mov cl, 1
            call NetGetContentsSize
            mov esi, eax
            add esi, 0x18
            mov ecx, esi
            call NetAllocateMessage
            mov ebp, eax
            test ebp, ebp
            je L004a05e0
            mov word ptr [ebp + 0x14], 1
            mov word ptr [ebp + 0x16], si
            mov byte ptr [ebp + 0x19], 1
    L004a05e0:
            mov ecx, dword ptr [gCurrent_net_game]
            lea edi, [ebp + 0x2a]
            lea esi, [ecx + 0xe]
            mov ecx, 0x1a
            rep movsd
            movsw
            mov eax, dword ptr [gNet_mode]
            mov esi, dword ptr [gCurrent_net_game]
            test eax, eax
            jne L004a060d
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L004a062c
    L004a060d:
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ebp + 8], edx
            call PDGetTotalTime
            mov dword ptr [ebp + 0x10], eax
            mov eax, dword ptr [esp + 0x20]
            push eax
            mov edx, ebp
            mov ecx, esi
            call PDNetSendMessageToAddress
    L004a062c:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            jmp L004a4474
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a06a3
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a06c5
    L004a06a3:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a06c5:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gCurrent_join_poll_game]
            inc edx
            test eax, eax
            mov dword ptr [gTimers_stack_size], edx
            je L004a0711
            lea esi, [ebx + 0x12]
            lea edi, [eax + 0xe]
            mov ecx, 0x1a
            mov dword ptr [gBastard_has_answered], 1
            rep movsd
            movsw
    L004a0711:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a07ae
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a07d0
    L004a07ae:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a07d0:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gNet_mode]
            inc ecx
            mov dword ptr [gTimers_stack_size], ecx
            mov ecx, dword ptr [esp + 0x14]
            cmp eax, 2
            mov edx, dword ptr [ecx + 8]
            jne L004a0b6c
            mov eax, dword ptr [gCurrent_net_game]
            test eax, eax
            je L004a0b6c
            mov ebp, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test ebp, ebp
            jle L004a0840
            mov ecx, offset gNet_players+0x24
    L004a082d:
            cmp dword ptr [ecx], edx
            je L004a0b6c
            inc eax
            add ecx, 0xd4
            cmp eax, ebp
            jl L004a082d
    L004a0840:
            lea eax, [ebp + 1]
            push 0xb8
            mov dword ptr [esp + 0x1c], eax
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            shl edx, 2
            push edx
            call BrMemAllocate
            lea ecx, [ebp + ebp*2]
            mov esi, offset gNet_players
            mov edi, eax
            add esp, 8
            lea ecx, [ecx + ecx*8]
            mov dword ptr [esp + 0x1c], eax
            shl ecx, 1
            sub ecx, ebp
            shl ecx, 2
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, dword ptr [ecx + 0x54]
            test edx, edx
            jne L004a08a4
            mov edx, dword ptr [gProgram_state+0x28]
            test edx, edx
            jne L004a0b05
    L004a08a4:
            cmp dword ptr [ecx + 0x34], 0xc
            jge L004a0b05
            mov ecx, dword ptr [gDont_allow_joiners]
            test ecx, ecx
            jne L004a0b05
            xor ecx, ecx
            test ebp, ebp
            jle L004a08da
            mov edx, dword ptr [ebx + 0x28]
            add eax, 0x24
    L004a08c8:
            cmp dword ptr [eax], edx
            je L004a0b6c
            inc ecx
            add eax, 0xd4
            cmp ecx, ebp
            jl L004a08c8
    L004a08da:
            mov esi, dword ptr [ebx + 0x50]
            test esi, esi
            jge L004a44f4
            call PickARandomCar
            mov dword ptr [ebx + 0x50], eax
    L004a08ed:
            mov eax, dword ptr [ebx + 0x50]
            test eax, eax
            jl L004a0902
            lea edx, [eax + eax*4]
            mov dword ptr [edx*4 + gCar_details], 1
    L004a0902:
            mov ecx, dword ptr [esp + 0x1c]
            lea eax, [ebp + ebp*2]
            mov edx, dword ptr [esp + 0x10]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ebp
            lea esi, [edx + 4]
            lea ebx, [ecx + eax*4]
            mov ecx, 0x35
            mov edi, ebx
            rep movsd
            mov dword ptr [ebx + 0x48], 2
            call PDGetTotalTime
            mov dword ptr [ebx + 0x14], eax
            xor eax, eax
            mov dword ptr [ebx + 0x54], eax
            mov dword ptr [ebx + 0x58], eax
            mov cl, byte ptr [ebx + 0x28]
            lea eax, [ebx + 0x28]
            test cl, cl
            jne L004a0959
            push ebp
            push offset gAsmString_0065a2e0
            push offset gAsmString_0058fd6c
            push eax
            call sprintf
            add esp, 0x10
    L004a0959:
            mov edx, dword ptr [esp + 0x20]
            mov ecx, ebx
            call PDNetSetPlayerSystemInfo
            mov esi, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x18]
            mov edx, esi
            call NetPlayersChanged
            push esi
            call BrMemFree
            mov ecx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [gNumber_of_net_players]
            add esp, 4
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [ecx + 0x34], eax
            mov eax, dword ptr [gNumber_of_net_players]
            test eax, eax
            jle L004a0aae
            mov dword ptr [esp + 0x1c], offset gNet_players
    L004a09a7:
            mov edx, 1
            mov cl, 3
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            mov edx, 1
            mov cl, 3
            mov dword ptr [esp + 0x18], eax
            call NetGetContentsSize
            mov edx, dword ptr [esp + 0x18]
            lea esi, [eax + edx + 0x18]
            mov ecx, esi
            call NetAllocateMessage
            mov ebp, eax
            test ebp, ebp
            je L004a0a0c
            mov cl, byte ptr [esp + 0x18]
            mov byte ptr [ebp + 0x18], bl
            and ebx, 0xff
            mov word ptr [ebp + 0x14], 0xff
            mov word ptr [ebp + 0x16], si
            mov byte ptr [ebp + 0x19], 3
            xor eax, eax
            mov byte ptr [ebp + ebx + 0x19], 5
            mov al, byte ptr [ebp + 0x18]
            mov byte ptr [ebp + eax + 0x18], cl
    L004a0a0c:
            mov edx, dword ptr [gNumber_of_net_players]
            mov eax, dword ptr [esp + 0x24]
            mov esi, dword ptr [esp + 0x1c]
            mov dword ptr [ebp + 0x1c], edx
            mov dword ptr [ebp + 0x20], eax
            mov ecx, dword ptr [gUINT_00690c40]
            mov dword ptr [ebp + 0x24], ecx
            lea edi, [ebp + 0x28]
            mov ecx, 0x35
            xor ebx, ebx
            rep movsd
            xor edi, edi
            mov dword ptr [ebp + 0xf4], edi
            mov eax, dword ptr [gNumber_of_net_players]
            mov edx, dword ptr [gCurrent_net_game]
            cmp eax, 1
            mov dword ptr [esp + 0x18], edx
            jne L004a0a5c
            mov byte ptr [ebp + 0x19], 0xff
            mov eax, dword ptr [gNumber_of_net_players]
            jmp L004a0a8f
    L004a0a5c:
            cmp eax, edi
            jle L004a0a8f
            mov esi, offset gNet_players+0x24
    L004a0a65:
            cmp edi, dword ptr [gThis_net_player_index]
            je L004a0a84
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [esp + 0x18]
            push 0
            push eax
            mov edx, ebp
            call NetGuaranteedSendMessageToPlayer
            or ebx, eax
            mov eax, dword ptr [gNumber_of_net_players]
    L004a0a84:
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L004a0a65
    L004a0a8f:
            mov ecx, dword ptr [esp + 0x24]
            mov esi, dword ptr [esp + 0x1c]
            inc ecx
            add esi, 0xd4
            cmp ecx, eax
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x1c], esi
            jl L004a09a7
    L004a0aae:
            mov ecx, dword ptr [gUINT_00690c40]
            mov eax, dword ptr [gInitialised_grid]
            inc ecx
            test eax, eax
            mov dword ptr [gUINT_00690c40], ecx
            je L004a0b6c
            call SetUpNetCarPositions
            mov eax, dword ptr [gINT_0074b780]
            test eax, eax
            je L004a0b6c
            mov ecx, dword ptr [gNumber_of_net_players]
            mov dword ptr [gSynch_race_start], 1
            cmp ecx, 1
            jle L004a0b6c
            mov eax, offset gNet_players+0x134
            dec ecx
    L004a0af5:
            mov dword ptr [eax], 1
            add eax, 0xd4
            dec ecx
            jne L004a0af5
            jmp L004a0b6c
    L004a0b05:
            xor edx, edx
            mov cl, 3
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L004a0b2e
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 3
    L004a0b2e:
            mov dword ptr [esi + 0x1c], 0
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            jne L004a0b4d
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L004a0b6c
    L004a0b4d:
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov edx, dword ptr [esp + 0x20]
            mov dword ptr [esi + 0x10], eax
            push edx
    L004a0b63:
            mov edx, esi
            mov ecx, edi
            call PDNetSendMessageToAddress
    L004a0b6c:
            mov eax, dword ptr [gTimers+0x190]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a2e18
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ebx, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            dec ecx
            mov dword ptr [esi*4 + gTimers+0xc], eax
            mov dword ptr [gTimers_stack_size], ecx
            jmp L004a44c2
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a0c12
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a0c34
    L004a0c12:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a0c34:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [esp + 0x14]
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            mov ecx, dword ptr [ecx + 8]
            call RemoveNetPlayer
            mov edx, dword ptr [gTimers+0x190]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a0d07
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a0d29
    L004a0d07:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a0d29:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gINT_0068d968]
            inc ecx
            test eax, eax
            mov dword ptr [gTimers_stack_size], ecx
            jne L004a0d7c
            xor edx, edx
            mov ecx, 0x49
            mov dword ptr [gINT_0068d968], 1
            call NetFullScreenMessage
            mov dword ptr [gProgram_state+0xa4], 2
    L004a0d7c:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a0e19
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a0e3b
    L004a0e19:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a0e3b:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [esp + 0x14]
            inc edi
            mov ecx, ebx
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_004a48d0
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a0f0d
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a0f2f
    L004a0f0d:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a0f2f:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gProgram_state+0x28]
            inc ecx
            test eax, eax
            mov dword ptr [gTimers_stack_size], ecx
            mov dword ptr [gRace_finished], 0
            je L004a0f83
            cmp dword ptr [gNet_mode], 3
            je L004a0f7b
            cmp dword ptr [ebx + 4], 8
            jl L004a0f83
    L004a0f7b:
            mov ecx, dword ptr [ebx + 4]
            call RaceCompleted
    L004a0f83:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a1020
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1042
    L004a1020:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1042:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            inc edx
            xor eax, eax
            mov dword ptr [gTimers_stack_size], edx
            mov edx, dword ptr [gNumber_of_net_players]
            test edx, edx
            jle L004a10fc
            mov ecx, dword ptr [esp + 0x14]
            mov esi, dword ptr [ecx + 8]
            mov ecx, offset gNet_players+0x24
    L004a1088:
            cmp dword ptr [ecx], esi
            je L004a1099
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a1088
            jmp L004a10fc
    L004a1099:
            lea esi, [eax + eax*2]
            xor edx, edx
            mov dx, word ptr [ebx + 2]
            lea esi, [esi + esi*8]
            shl esi, 1
            sub esi, eax
            shl esi, 2
            mov dword ptr [esi + gNet_players+0x48], edx
            call PDGetTotalTime
            mov dword ptr [esi + gNet_players+0x14], eax
            mov eax, dword ptr [esi + gNet_players+0x48]
            cmp eax, 6
            jl L004a10f1
            cmp eax, 8
            je L004a10ec
            mov eax, dword ptr [esi + gNet_players+0xc8]
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            je L004a10df
            call SendCurrentPowerups
    L004a10df:
            mov ecx, dword ptr [esi + gNet_players+0xc8]
            call EnableCar
            jmp L004a10fc
    L004a10ec:
            cmp eax, 6
            jge L004a10fc
    L004a10f1:
            mov ecx, dword ptr [esi + gNet_players+0xc8]
            call DisableCar
    L004a10fc:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a1199
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a11bb
    L004a1199:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a11bb:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gStart_race_sent]
            inc ecx
            test eax, eax
            mov dword ptr [gTimers_stack_size], ecx
            jne L004a134e
            mov cl, byte ptr [ebx + 2]
            xor eax, eax
            test cl, cl
            jle L004a130d
            mov esi, offset gCurrent_race+0xcf0
            lea edi, [ebx + 6]
    L004a1207:
            cmp eax, dword ptr [gNumber_of_net_players]
            jge L004a130d
            lea ebp, [eax + 1]
            or eax, 0xffffffff
            mov dword ptr [gCurrent_race+0x90], ebp
            mov dword ptr [esi - 0xc], eax
            mov dword ptr [esi - 4], eax
            mov al, byte ptr [edi]
            and eax, 0x7f
            xor edx, edx
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov eax, dword ptr [ecx*4 + gNet_players+0xc8]
            cmp eax, edx
            mov dword ptr [esi], eax
            je L004a12df
            mov al, byte ptr [edi]
            and eax, 0x7f
            mov dword ptr [esi - 8], eax
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov dword ptr [ecx*4 + gNet_players+0x1c], edx
            mov ecx, eax
            lea eax, [ecx + ecx*2]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ecx
            shl eax, 2
            mov dword ptr [eax + gNet_players+0x70], edx
            mov dword ptr [eax + gNet_players+0x80], edx
            cmp dword ptr [gProgram_state+0x28], edx
            je L004a128f
            mov eax, dword ptr [esi]
            cmp eax, edx
            je L004a128f
            cmp dword ptr [eax + 0xc], 8
            je L004a12af
    L004a128f:
            mov dl, byte ptr [edi]
            mov eax, dword ptr [esi]
            and edx, 0x80
            mov ecx, dword ptr [eax + 0x10]
            push edx
            lea edx, [edi + 2]
            add ecx, 0x2c
            call ExpandMatrix34
            mov ecx, dword ptr [esi]
            call InitialiseCar
    L004a12af:
            mov al, byte ptr [edi]
            add esi, 0x10
            and eax, 0x7f
            movsx edx, byte ptr [edi + 1]
            lea ecx, [eax + eax*2]
            add edi, 0x14
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov eax, ebp
            mov dword ptr [ecx*4 + gNet_players+0x8c], edx
            movsx edx, byte ptr [ebx + 2]
            cmp eax, edx
            jl L004a1207
            jmp L004a130d
    L004a12df:
            cmp dword ptr [gINT_0068d968], edx
            jne L004a138a
            xor edx, edx
            mov ecx, 0x59
            mov dword ptr [gINT_0068d968], 1
            call NetFullScreenMessage
            mov dword ptr [gProgram_state+0xa4], 2
            jmp L004a138a
    L004a130d:
            movsx ecx, byte ptr [ebx + 4]
            mov dword ptr [gPending_race], ecx
            mov dword ptr [gCurrent_race+0x90], eax
            mov dword ptr [gStart_race_sent], 1
            mov al, byte ptr [ebx + 3]
            test al, al
            je L004a1342
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            jne L004a1342
            mov dword ptr [gWait_for_it], 0x3e8
            jmp L004a138a
    L004a1342:
            mov dword ptr [gWait_for_it], 0
            jmp L004a138a
    L004a134e:
            mov al, byte ptr [ebx + 2]
            xor esi, esi
            test al, al
            jle L004a138a
            mov edi, dword ptr [gNumber_of_net_players]
            lea edx, [ebx + 6]
    L004a1360:
            cmp esi, edi
            jge L004a138a
            mov al, byte ptr [edx]
            and eax, 0x7f
            movsx ebp, byte ptr [edx + 1]
            lea ecx, [eax + eax*2]
            add edx, 0x14
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            inc esi
            mov dword ptr [ecx*4 + gNet_players+0x8c], ebp
            movsx eax, byte ptr [ebx + 2]
            cmp esi, eax
            jl L004a1360
    L004a138a:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xf
            je L004a1427
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1449
    L004a1427:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1449:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [esp + 0x14]
            inc edx
            mov dword ptr [gTimers+0x1b0], eax
            mov dword ptr [gTimers_stack_size], edx
            mov esi, dword ptr [ecx + 8]
            xor edx, edx
            mov cl, 4
            call NetGetContentsSize
            mov edx, dword ptr [gNumber_of_net_players]
            mov ebx, eax
            xor eax, eax
            mov dword ptr [esp + 0x18], ebx
            test edx, edx
            jle L004a15b0
            mov ecx, offset gNet_players+0x24
    L004a149e:
            cmp dword ptr [ecx], esi
            je L004a14b2
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a149e
            jmp L004a15b0
    L004a14b2:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea edi, [edx*4 + gNet_players]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            cmp edi, edx
            jne L004a14e7
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L004a14e7:
            mov esi, dword ptr [edi + 0xcc]
            test esi, esi
            je L004a1570
            xor eax, eax
            mov ax, word ptr [esi + 0x16]
            add eax, ebx
            cmp eax, 0x200
            jle L004a156c
            mov eax, dword ptr [gNet_mode]
            mov ebp, dword ptr [edi + 0x24]
            mov ebx, dword ptr [gCurrent_net_game]
            test eax, eax
            je L004a1560
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a1560
            mov ecx, offset gNet_players+0x24
    L004a1534:
            cmp dword ptr [ecx], ebp
            je L004a1545
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a1534
            jmp L004a1560
    L004a1545:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, ebx
            shl edx, 1
            sub edx, eax
            lea eax, [edx*4 + gNet_players]
            mov edx, esi
            push eax
            call PDNetSendMessageToAddress
    L004a1560:
            mov ebx, dword ptr [esp + 0x18]
            xor esi, esi
            mov dword ptr [edi + 0xcc], esi
    L004a156c:
            test esi, esi
            jne L004a158e
    L004a1570:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [edi + 0xcc], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L004a158e:
            mov cx, word ptr [esi + 0x16]
            mov eax, esi
            mov edx, ecx
            add ecx, ebx
            and edx, 0xffff
            mov word ptr [esi + 0x16], cx
            add eax, edx
            inc word ptr [esi + 0x14]
            mov byte ptr [eax + 1], 4
            mov byte ptr [eax], bl
            jmp L004a15b2
    L004a15b0:
            xor eax, eax
    L004a15b2:
            test eax, eax
            je L004a15c4
            mov edx, dword ptr [esp + 0x10]
            mov cx, word ptr [edx + 2]
            mov word ptr [eax + 2], cx
            jmp L004a1630
    L004a15c4:
            xor edx, edx
            mov cl, 4
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L004a15ed
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 4
    L004a15ed:
            mov edx, dword ptr [esp + 0x10]
            mov ax, word ptr [edx + 2]
            mov word ptr [esi + 0x1a], ax
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            jne L004a1611
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L004a1630
    L004a1611:
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov edx, dword ptr [esp + 0x20]
            mov ecx, edi
            push edx
            mov edx, esi
            mov dword ptr [esi + 0x10], eax
            call PDNetSendMessageToAddress
    L004a1630:
            mov eax, dword ptr [gTimers+0x1ac]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x1a8]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x1b0]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a2e18
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ebx, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            dec ecx
            mov dword ptr [esi*4 + gTimers+0xc], eax
            mov dword ptr [gTimers_stack_size], ecx
            jmp L004a44c2
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xf
            je L004a16d6
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a16f8
    L004a16d6:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a16f8:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x1b0], eax
            inc ecx
            mov dword ptr [gTimers_stack_size], ecx
            mov ecx, dword ptr [gNext_guarantee]
            test ecx, ecx
            jle L004a1747
            mov eax, offset gGuarantee_list+0x10
    L004a1731:
            mov dx, word ptr [eax + 0x1c]
            cmp dx, word ptr [ebx + 2]
            jne L004a1741
            mov dword ptr [eax], 1
    L004a1741:
            add eax, 0x34
            dec ecx
            jne L004a1731
    L004a1747:
            mov eax, dword ptr [gTimers+0x1ac]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x1a8]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x1b0]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a17e2
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1804
    L004a17e2:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1804:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gProgram_state+0x28]
            inc ecx
            test eax, eax
            mov dword ptr [gTimers_stack_size], ecx
            je L004a184e
            lea ecx, [ebx + 2]
            xor edx, edx
            push ecx
            push -4
            push 0xbb8
            mov ecx, 4
            call NewTextHeadupSlot
    L004a184e:
            mov edx, dword ptr [gTimers+0x190]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a18eb
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a190d
    L004a18eb:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a190d:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov cl, 0xf
            inc edx
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edx
            xor edx, edx
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            xor edx, edx
            mov cl, 0xf
            mov dword ptr [esp + 0x18], eax
            call NetGetContentsSize
            mov ecx, dword ptr [esp + 0x18]
            lea esi, [eax + ecx + 0x18]
            mov ecx, esi
            call NetAllocateMessage
            test eax, eax
            je L004a1994
            mov cl, byte ptr [esp + 0x18]
            mov byte ptr [eax + 0x18], bl
            and ebx, 0xff
            mov word ptr [eax + 0x14], 0xff
            mov word ptr [eax + 0x16], si
            mov byte ptr [eax + 0x19], 0xf
            xor edx, edx
            mov byte ptr [ebx + eax + 0x19], 5
            mov dl, byte ptr [eax + 0x18]
            mov byte ptr [eax + edx + 0x18], cl
    L004a1994:
            mov edx, dword ptr [gProgram_state+0x28]
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [eax + 0x1c], edx
            mov ecx, dword ptr [gProgram_state+0x78]
            mov dword ptr [eax + 0x20], ecx
            mov edx, dword ptr [gPending_race]
            mov dword ptr [eax + 0x24], edx
            mov ecx, dword ptr [esi + 8]
            push 0
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, eax
            call NetGuaranteedSendMessageToPlayer
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L004a1a15
            call SignalToStartRace
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a1a0c
            mov esi, dword ptr [esi + 8]
            mov ecx, offset gNet_players+0x24
    L004a19e8:
            cmp dword ptr [ecx], esi
            je L004a19f9
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a19e8
            jmp L004a1a0c
    L004a19f9:
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            lea eax, [edx*4 + gNet_players]
            jmp L004a1a0e
    L004a1a0c:
            xor eax, eax
    L004a1a0e:
            mov dword ptr [eax + 0x58], 1
    L004a1a15:
            mov eax, dword ptr [gTimers+0x190]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a2e18
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ebx, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            dec ecx
            mov dword ptr [esi*4 + gTimers+0xc], eax
            mov dword ptr [gTimers_stack_size], ecx
            jmp L004a44c2
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a1abb
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1add
    L004a1abb:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1add:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gProgram_state+0x78]
            inc ecx
            mov dword ptr [gTimers_stack_size], ecx
            mov ecx, dword ptr [ebx + 8]
            cmp ecx, eax
            je L004a1b36
            mov ecx, dword ptr [gCurrent_net_game]
            call NetLeaveGame
            xor edx, edx
            mov ecx, 0x80
            call NetFullScreenMessage
            mov dword ptr [gProgram_state+0xa4], 2
            jmp L004a1b6a
    L004a1b36:
            mov eax, dword ptr [ebx + 4]
            test eax, eax
            je L004a1b6a
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x54]
            test eax, eax
            jne L004a1b62
            xor edx, edx
            mov ecx, 0x59
            call NetFullScreenMessage
            mov dword ptr [gProgram_state+0xa4], 2
            jmp L004a1b6a
    L004a1b62:
            mov eax, dword ptr [ebx + 0xc]
            mov dword ptr [gPending_race], eax
    L004a1b6a:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xc
            je L004a1c07
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1c29
    L004a1c07:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1c29:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x15c], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedCarData
            mov ecx, dword ptr [gTimers+0x158]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x154]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x15c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xc
            je L004a1cf7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1d19
    L004a1cf7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1d19:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x15c], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedNonCar
            mov ecx, dword ptr [gTimers+0x158]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x154]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x15c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a1de7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1e09
    L004a1de7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1e09:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov esi, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x130]
            inc esi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], esi
            mov eax, dword ptr [ecx + 0x10]
            mov ecx, dword ptr [ebx + 4]
            mov esi, eax
            lea edi, [edx + 0xa]
            sub esi, ecx
            cmp edi, esi
            jae L004a1e5f
            add edx, ecx
            sub edx, eax
            mov eax, dword ptr [gRace_start]
            add eax, edx
            mov dword ptr [gRace_start], eax
    L004a1e5f:
            mov edx, dword ptr [gTimers+0x190]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a1efc
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a1f1e
    L004a1efc:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a1f1e:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            inc edx
            xor eax, eax
            mov dword ptr [gTimers_stack_size], edx
            mov edx, dword ptr [gNumber_of_net_players]
            test edx, edx
            jle L004a1f82
            mov esi, dword ptr [ebx + 4]
            mov ecx, offset gNet_players+0x24
    L004a1f5c:
            cmp dword ptr [ecx], esi
            je L004a1f6d
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a1f5c
            jmp L004a1f82
    L004a1f6d:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov dword ptr [ecx*4 + gNet_players+0x60], 0
    L004a1f82:
            mov edx, dword ptr [gTimers+0x190]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a201f
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2041
    L004a201f:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2041:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers+0x190]
            mov edi, dword ptr [gTimers_stack_size]
            and ecx, 0xff
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2108
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a212a
    L004a2108:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a212a:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers+0x190]
            mov edi, dword ptr [gTimers_stack_size]
            and ecx, 0xff
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a21f1
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2213
    L004a21f1:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2213:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedPowerup
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a22e1
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2303
    L004a22e1:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2303:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedRecover
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a23d1
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a23f3
    L004a23d1:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a23f3:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            inc ecx
            mov dword ptr [gTimers_stack_size], ecx
            mov ecx, dword ptr [ebx + 4]
            call FUN_0049b7e0
            mov edx, dword ptr [gNumber_of_net_players]
            test edx, edx
            jle L004a2447
            mov ecx, offset gNet_players+0x64
            lea eax, [ebx + 8]
    L004a2437:
            mov esi, dword ptr [eax]
            add eax, 4
            mov dword ptr [ecx], esi
            add ecx, 0xd4
            dec edx
            jne L004a2437
    L004a2447:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a24e4
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2506
    L004a24e4:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2506:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            inc edx
            xor eax, eax
            mov dword ptr [gTimers_stack_size], edx
            mov edx, dword ptr [gNumber_of_net_players]
            test edx, edx
            jle L004a2568
            mov esi, dword ptr [ebx + 4]
            mov ecx, offset gNet_players+0x24
    L004a2544:
            cmp dword ptr [ecx], esi
            je L004a2555
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a2544
            jmp L004a2568
    L004a2555:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edi, [ecx*4 + gNet_players]
            jmp L004a256a
    L004a2568:
            xor edi, edi
    L004a256a:
            test edi, edi
            je L004a27a8
            mov ecx, dword ptr [edi + 0xc8]
            mov ebp, 1
            mov dword ptr [ecx + 0x1d4], ebp
            mov eax, dword ptr [gNet_mode]
            cmp eax, 2
            je L004a2592
            call ExplodeMeCarUpMeOldMatey
    L004a2592:
            mov edx, dword ptr [ebx + 4]
            mov eax, dword ptr [gLocal_net_ID]
            cmp edx, eax
            jne L004a25eb
            mov eax, dword ptr [gCurrent_net_game]
            mov ecx, dword ptr [eax + 0x74]
            test ecx, ecx
            je L004a25db
            call PDGetTotalTime
            mov edx, dword ptr [gUINT_00690c70]
            mov dword ptr [gUINT_00690c6c], eax
            sub eax, edx
            cmp eax, 0x3e8
            jg L004a25cf
            mov ecx, 0x19
            call DoFancyHeadup
            jmp L004a25eb
    L004a25cf:
            mov ecx, 0x18
            call DoFancyHeadup
            jmp L004a25eb
    L004a25db:
            mov ecx, 0x1c
            call DoFancyHeadup
            mov dword ptr [gRace_finished], ebp
    L004a25eb:
            mov esi, dword ptr [ebx + 8]
            cmp esi, -1
            jne L004a260b
            mov eax, dword ptr [edi + 0x1c]
            test eax, eax
            jne L004a27ad
            call GetTotalTime
            mov dword ptr [edi + 0x1c], eax
            jmp L004a27ad
    L004a260b:
            mov eax, dword ptr [edi + 0x70]
            test eax, eax
            jne L004a27ad
            cmp esi, -2
            je L004a2650
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a2650
            mov ecx, offset gNet_players+0x24
    L004a262c:
            cmp dword ptr [ecx], esi
            je L004a263d
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a262c
            jmp L004a2650
    L004a263d:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea esi, [ecx*4 + gNet_players]
            jmp L004a2652
    L004a2650:
            xor esi, esi
    L004a2652:
            test esi, esi
            je L004a2698
            cmp dword ptr [gNet_mode], 2
            jne L004a2698
            mov ecx, dword ptr [gCurrent_net_game]
            cmp dword ptr [ecx + 0x74], ebp
            jne L004a2698
            mov eax, dword ptr [esi + 0x64]
            inc eax
            mov dword ptr [esi + 0x64], eax
            mov edx, dword ptr [ecx + 0x60]
            cmp eax, edx
            jl L004a2698
            mov ecx, esi
            mov eax, 0x4d4873ed
            sub ecx, offset gNet_players
            imul ecx
            sar edx, 6
            mov eax, edx
            mov ecx, edx
            shr eax, 0x1f
            add ecx, eax
            call DeclareWinner
    L004a2698:
            call GetTotalTime
            mov dword ptr [edi + 0x1c], eax
            mov eax, dword ptr [gPTR_00690c78]
            cmp edi, eax
            mov dword ptr [edi + 0x70], ebp
            jne L004a26fc
            cmp esi, dword ptr [gPTR_00690c7c]
            jne L004a26fc
            test esi, esi
            je L004a2705
            call PDGetTotalTime
            sub eax, dword ptr [gUINT_00690c74]
            cmp eax, 0x3e8
            jge L004a26fc
            mov ecx, 0x7f
            call GetMiscString
            lea ecx, [esi + 0x28]
            push eax
            push ecx
            mov ecx, 0x7e
            call GetMiscString
            lea edx, [edi + 0x28]
            push eax
            push edx
            lea eax, [esp + 0x3c]
            push offset gAsmString_0065a2d4
            push eax
            call sprintf
            add esp, 0x18
            jmp L004a2731
    L004a26fc:
            test esi, esi
            je L004a2705
            lea eax, [esi + 0x28]
            jmp L004a270f
    L004a2705:
            mov ecx, 0xd8
            call GetMiscString
    L004a270f:
            push eax
            mov ecx, 0xab
            call GetMiscString
            lea ecx, [edi + 0x28]
            lea edx, [esp + 0x30]
            push eax
            push ecx
            push offset gAsmString_0065a2c8
            push edx
            call sprintf
            add esp, 0x14
    L004a2731:
            lea eax, [esp + 0x2c]
            push 0
            push eax
            push -4
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot2
            call PDGetTotalTime
            mov dword ptr [gUINT_00690c74], eax
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [gPTR_00690c78], esi
            mov dword ptr [gPTR_00690c7c], edi
            mov ecx, dword ptr [ebx + 8]
            cmp ecx, eax
            jne L004a27ad
            mov ecx, 0x20
            call PratcamEvent
            call PDGetTotalTime
            mov edx, dword ptr [gUINT_00690c6c]
            mov dword ptr [gUINT_00690c70], eax
            sub eax, edx
            cmp eax, 0x3e8
            jg L004a279c
            mov ecx, 0x19
            call DoFancyHeadup
            jmp L004a27ad
    L004a279c:
            mov ecx, 0x14
            call DoFancyHeadup
            jmp L004a27ad
    L004a27a8:
            mov ebp, 1
    L004a27ad:
            mov edx, dword ptr [gTimers+0x190]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, ebp
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2849
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a286b
    L004a2849:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a286b:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ebp, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [esp + 0x130]
            mov edx, dword ptr [esp + 0x14]
            inc ebp
            push ecx
            mov ecx, ebx
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], ebp
            call FUN_0049bad0
            mov edx, dword ptr [gTimers+0x190]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xc
            je L004a2945
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2967
    L004a2945:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2967:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x15c], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedNonCarPosition
            mov ecx, dword ptr [gTimers+0x158]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x154]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x15c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2a35
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2a57
    L004a2a35:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2a57:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedCopInfo
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2b25
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2b47
    L004a2b25:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2b47:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            xor edx, edx
            inc ecx
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], ecx
            mov cl, 7
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            xor edx, edx
            mov cl, 7
            mov dword ptr [esp + 0x18], eax
            call NetGetContentsSize
            mov ecx, dword ptr [esp + 0x18]
            lea edi, [eax + ecx + 0x18]
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L004a2bd0
            mov al, byte ptr [esp + 0x18]
            mov byte ptr [esi + 0x18], bl
            and ebx, 0xff
            mov word ptr [esi + 0x14], 0xff
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 7
            xor edx, edx
            mov byte ptr [ebx + esi + 0x19], 5
            mov dl, byte ptr [esi + 0x18]
            mov byte ptr [esi + edx + 0x18], al
    L004a2bd0:
            mov ecx, dword ptr [gNumber_of_net_players]
            xor edx, edx
            mov dword ptr [esi + 0x1c], ecx
            mov eax, dword ptr [gNumber_of_net_players]
            test eax, eax
            jle L004a2c22
            mov ecx, offset gNet_players+0x28
            lea eax, [esi + 0x24]
    L004a2bec:
            mov edi, dword ptr [ecx + 0x24]
            mov ebx, eax
            mov dword ptr [eax - 4], edi
            mov edi, ecx
            inc edx
            add ecx, 0xd4
            mov ebp, dword ptr [edi]
            add eax, 0x14
            mov dword ptr [ebx], ebp
            mov ebp, dword ptr [edi + 4]
            mov dword ptr [ebx + 4], ebp
            mov ebp, dword ptr [edi + 8]
            mov dword ptr [ebx + 8], ebp
            mov edi, dword ptr [edi + 0xc]
            mov dword ptr [ebx + 0xc], edi
            mov byte ptr [eax - 5], 0
            cmp edx, dword ptr [gNumber_of_net_players]
            jl L004a2bec
    L004a2c22:
            mov eax, dword ptr [gNet_mode]
            mov ebx, dword ptr [gCurrent_net_game]
            test eax, eax
            jne L004a2c3a
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L004a2c66
    L004a2c3a:
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], edx
            call PDGetTotalTime
            mov edi, dword ptr [esp + 0x20]
            mov ecx, esi
            push 0
            push 0
            mov edx, edi
            mov dword ptr [esi + 0x10], eax
            call AddToGuaranteeList
            mov edx, esi
            mov ecx, ebx
            push edi
            call PDNetSendMessageToAddress
    L004a2c66:
            mov eax, dword ptr [gTimers+0x190]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            mov dword ptr [esi], ecx
            cmp eax, 1
            jmp L004a2de0
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2cd5
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2cf7
    L004a2cd5:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2cf7:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            inc ecx
            mov dword ptr [gTimers_stack_size], ecx
            mov ecx, dword ptr [gChosen_net_game_options]
            call SetNetAvailability
            mov eax, dword ptr [gNumber_of_racers]
            mov dword ptr [esp + 0x1c], 0
            test eax, eax
            jle L004a2da8
            mov ebp, offset gCar_details+0x4
    L004a2d42:
            mov eax, dword ptr [esp + 0x10]
            xor ebx, ebx
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            jle L004a2d92
            lea edx, [eax + 0xc]
    L004a2d52:
            mov ecx, dword ptr [esp + 0x1c]
            mov esi, dword ptr [edx - 4]
            cmp esi, ecx
            jne L004a2d87
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            mov dword ptr [ebp - 4], 1
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebp
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x10]
            and ecx, 3
            rep movsb
    L004a2d87:
            mov ecx, dword ptr [eax + 4]
            inc ebx
            add edx, 0x14
            cmp ebx, ecx
            jl L004a2d52
    L004a2d92:
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [gNumber_of_racers]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x1c], eax
            jl L004a2d42
    L004a2da8:
            mov ecx, dword ptr [gTimers+0x190]
            mov edi, 1
            and ecx, 0xff
            mov dword ptr [gReceived_car_details], edi
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            mov dword ptr [esi], ecx
            cmp eax, edi
    L004a2de0:
            jle L004a2e18
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ebx, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            dec ecx
            mov dword ptr [esi*4 + gTimers+0xc], eax
            mov dword ptr [gTimers_stack_size], ecx
            jmp L004a44c2
    L004a2e18:
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_max_index]
            mov ebx, dword ptr [esp + 0x10]
            mov ecx, edx
            shl ecx, 3
            sub ecx, edx
            mov dword ptr [ecx*4 + gTimers+0xc], eax
            mov ecx, dword ptr [gTimers_stack_size]
            dec ecx
            mov dword ptr [gTimers_stack_size], ecx
            jmp L004a44c2
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2e85
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2ea7
    L004a2e85:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2ea7:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [gNumber_of_net_players]
            inc ecx
            mov edi, 1
            test edx, edx
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], ecx
            mov dword ptr [gReceived_game_scores], edi
            jle L004a2f0b
            mov ecx, offset gNet_players+0x88
            lea eax, [ebx + 8]
    L004a2eee:
            mov esi, dword ptr [eax - 4]
            add eax, 0xc
            mov dword ptr [ecx - 0xc], esi
            mov esi, dword ptr [eax - 0xc]
            mov dword ptr [ecx], esi
            mov esi, dword ptr [eax - 8]
            mov dword ptr [ecx + 8], esi
            add ecx, 0xd4
            dec edx
            jne L004a2eee
    L004a2f0b:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, edi
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a2fa7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a2fc9
    L004a2fa7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a2fc9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedOilSpill
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3097
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a30b9
    L004a3097:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a30b9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedCrush
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xb
            je L004a3187
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a31a9
    L004a3187:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a31a9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x140], eax
            mov dword ptr [gTimers_stack_size], edi
            call QueueNetSmash
            mov ecx, dword ptr [gTimers+0x13c]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x138]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x140]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xb
            je L004a3277
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3299
    L004a3277:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3299:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x140], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_004f6320
            mov ecx, dword ptr [gTimers+0x13c]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x138]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x140]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a3367
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3389
    L004a3367:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3389:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedRepulseOpponents
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3457
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3479
    L004a3457:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3479:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedCrushDetach
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3547
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3569
    L004a3547:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3569:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_0043c940
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3637
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3659
    L004a3637:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3659:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedFullDetachBit
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3727
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3749
    L004a3727:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3749:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_0043cd60
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3817
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3839
    L004a3817:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3839:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedDetachedNonCarData
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3907
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3929
    L004a3907:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3929:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_0043d5f0
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a39f7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3a19
    L004a39f7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3a19:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_0043d810
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3ae7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3b09
    L004a3ae7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3b09:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedCrushSync
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3bd7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3bf9
    L004a3bd7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3bf9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceivedCrushReattachBit
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xa
            je L004a3cc7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3ce9
    L004a3cc7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3ce9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x124], eax
            mov dword ptr [gTimers_stack_size], edi
            call ToggleFlapsFromNetwork
            mov ecx, dword ptr [gTimers+0x120]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x11c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x124]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xc
            je L004a3db7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3dd9
    L004a3db7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3dd9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x15c], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_004984c0
            mov ecx, dword ptr [gTimers+0x158]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x154]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x15c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a3ea7
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a3ec9
    L004a3ea7:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a3ec9:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            inc ecx
            xor esi, esi
            mov dword ptr [gTimers_stack_size], ecx
            call PDGetTotalTime
            mov edx, dword ptr [gNumber_of_net_players]
            mov ebp, eax
            test edx, edx
            jle L004a3f19
            mov eax, offset gNet_players+0x24
            mov ecx, edx
    L004a3f0d:
            mov edi, dword ptr [eax]
            add eax, 0xd4
            add esi, edi
            dec ecx
            jne L004a3f0d
    L004a3f19:
            cmp esi, dword ptr [ebx + 4]
            jne L004a3f98
            xor ecx, ecx
            mov cl, byte ptr [ebx + 8]
            cmp edx, ecx
            jne L004a3f98
            xor edi, edi
            test edx, edx
            jle L004a3fa8
            mov esi, offset gNet_players+0x48
    L004a3f32:
            cmp edi, dword ptr [gThis_net_player_index]
            je L004a3f8b
            xor eax, eax
            mov al, byte ptr [edi + ebx + 9]
            cmp eax, 5
            mov dword ptr [esi], eax
            je L004a3f8b
            mov dword ptr [esi - 0x34], ebp
            mov eax, dword ptr [esi]
            cmp eax, 6
            jl L004a3f7a
            cmp eax, 8
            je L004a3f75
            mov edx, dword ptr [esi + 0x80]
            mov eax, dword ptr [edx + 4]
            test eax, eax
            je L004a3f68
            call SendCurrentPowerups
    L004a3f68:
            mov ecx, dword ptr [esi + 0x80]
            call EnableCar
            jmp L004a3f85
    L004a3f75:
            cmp eax, 6
            jge L004a3f8b
    L004a3f7a:
            mov ecx, dword ptr [esi + 0x80]
            call DisableCar
    L004a3f85:
            mov edx, dword ptr [gNumber_of_net_players]
    L004a3f8b:
            inc edi
            add esi, 0xd4
            cmp edi, edx
            jl L004a3f32
            jmp L004a3fa8
    L004a3f98:
            xor eax, eax
            mov al, byte ptr [ebx + 9]
            mov dword ptr [gNet_players+0x14], ebp
            mov dword ptr [gNet_players+0x48], eax
    L004a3fa8:
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            je L004a4045
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a4067
    L004a4045:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a4067:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [esp + 0x14]
            inc edi
            mov ecx, ebx
            mov dword ptr [gTimers+0x194], eax
            mov dword ptr [gTimers_stack_size], edi
            call FUN_00497860
            mov ecx, dword ptr [gTimers+0x190]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xc
            je L004a4139
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a415b
    L004a4139:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a415b:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x15c], eax
            mov eax, dword ptr [gProgram_state+0x1bbc]
            inc ecx
            mov dword ptr [gTimers_stack_size], ecx
            mov ecx, dword ptr [ebx + 4]
            cmp ecx, eax
            jae L004a41f3
            mov eax, dword ptr [gProgram_state+0x1bc0]
            xor edx, edx
            mov dx, word ptr [ebx + 2]
            mov esi, dword ptr [eax + edx*4]
            test esi, esi
            je L004a41f3
            mov eax, dword ptr [esi + 0x5c]
            test eax, eax
            je L004a41c3
            cmp dword ptr [eax + 0xc], 4
            jne L004a41bd
            xor edx, edx
            mov dx, word ptr [eax + 0x80]
            cmp edx, ecx
            je L004a41bf
    L004a41bd:
            xor eax, eax
    L004a41bf:
            test eax, eax
            jne L004a41e1
    L004a41c3:
            mov ecx, dword ptr [esi + 0x14]
            mov cl, byte ptr [ecx + 1]
            cmp cl, 0x30
            jl L004a41dd
            cmp cl, 0x39
            jg L004a41dd
            mov ecx, esi
            call DoPullActorFromWorld
            mov eax, dword ptr [esi + 0x5c]
    L004a41dd:
            test eax, eax
            je L004a41f3
    L004a41e1:
            mov edx, dword ptr [eax + 0x100]
            and dl, 1
            or edx, 1
            mov dword ptr [eax + 0x100], edx
    L004a41f3:
            mov eax, dword ptr [gTimers+0x158]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x154]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x15c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            test ecx, ecx
            mov dword ptr [ecx*4 + gTimers_stack], 0xc
            je L004a428e
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a42b0
    L004a428e:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a42b0:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0x15c], eax
            mov dword ptr [gTimers_stack_size], edi
            call ReceiveMineUpdate
            mov ecx, dword ptr [gTimers+0x158]
            and ecx, 0xff
            lea esi, [ecx*4 + gTimers+0x154]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x15c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
            xor ebp, ebp
            mov dword ptr [ecx*4 + gTimers_stack], 0xe
            cmp ecx, ebp
            je L004a4380
            mov ecx, dword ptr [ecx*4 + 0x6b75dc]
            xor edx, edx
            lea esi, [ecx*8]
            lea eax, [ecx*8]
            sub esi, ecx
            sub eax, ecx
            shl esi, 2
            mov dl, byte ptr [esi + gTimers+0x8]
            add edx, eax
            lea edi, [edx*4 + gTimers+0x4]
            jmp L004a43a2
    L004a4380:
            mov esi, edx
            mov eax, edx
            shl esi, 3
            sub esi, edx
            xor ecx, ecx
            shl esi, 2
            shl eax, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub eax, edx
            add ecx, eax
            lea edi, [ecx*4 + gTimers+0x4]
    L004a43a2:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x194], eax
            mov eax, dword ptr [gProgram_state+0x28]
            inc ecx
            cmp eax, ebp
            mov dword ptr [gTimers_stack_size], ecx
            je L004a4446
            xor ecx, ecx
            mov edx, dword ptr [ebx + 8]
            mov cl, byte ptr [ebx + 0xc]
            push edx
            mov edi, ecx
            lea edx, [ebx + 0xe]
            lea esi, [edi + edi*2]
            lea esi, [esi + esi*8]
            shl esi, 1
            sub esi, edi
            shl esi, 2
            mov eax, dword ptr [esi + gNet_players+0xc8]
            mov ecx, dword ptr [eax + 0x10]
            add ecx, 0x2c
            call ExpandMatrix34
            mov ecx, dword ptr [esi + gNet_players+0xc8]
            call ReinitialiseCar
            cmp edi, dword ptr [gThis_net_player_index]
            jne L004a443a
            cmp dword ptr [gInterface_within_race_mode], ebp
            jne L004a4421
            call FadePaletteDown
    L004a4421:
            call CancelPendingCunningStunt
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, dword ptr [ecx + 0x58]
            mov dword ptr [gProgram_state], edx
            call InitialiseExternalCamera
    L004a443a:
            mov dword ptr [esi + gNet_players+0x1c], ebp
            mov dword ptr [esi + gNet_players+0x70], ebp
    L004a4446:
            mov eax, dword ptr [gTimers+0x190]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x18c]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x194]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L004a449c
    L004a4474:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov edx, dword ptr [ecx*4 + 0x6b75dc]
            mov esi, edx
            shl esi, 3
            sub esi, edx
            mov edx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004a44bb
    L004a449c:
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_max_index]
            mov ecx, edx
            shl ecx, 3
            sub ecx, edx
            mov dword ptr [ecx*4 + gTimers+0xc], eax
            mov ecx, dword ptr [gTimers_stack_size]
    L004a44bb:
            dec ecx
            mov dword ptr [gTimers_stack_size], ecx
    L004a44c2:
            mov ebp, dword ptr [esp + 0x14]
            xor eax, eax
            mov al, byte ptr [ebx]
            xor esi, esi
            mov si, word ptr [ebp + 0x14]
            add ebx, eax
            mov eax, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x10], ebx
            inc eax
            cmp eax, esi
            mov dword ptr [esp + 0x28], eax
            jl L004a0483
    L004a44e7:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x11c
            ret 4
    L004a44f4:
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a08ed
            mov ecx, offset gNet_players+0x4c
    L004a4509:
            cmp dword ptr [ecx], esi
            je L004a451d
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a4509
            jmp L004a08ed
    L004a451d:
            xor edx, edx
            mov cl, 3
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L004a4546
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 3
    L004a4546:
            mov dword ptr [esi + 0x1c], 0xffffffff
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            jne L004a4569
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L004a0b6c
    L004a4569:
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov ecx, dword ptr [esp + 0x20]
            mov dword ptr [esi + 0x10], eax
            push ecx
            jmp L004a0b63
            nop
            _emit 0x24
            _emit 0x05
            _emit 0x4a
            _emit 0x00
            _emit 0x65
            _emit 0x06
            _emit 0x4a
            _emit 0x00
            _emit 0x70
            _emit 0x07
            _emit 0x4a
            _emit 0x00
            _emit 0xdb
            _emit 0x0d
            _emit 0x4a
            _emit 0x00
            _emit 0x98
            _emit 0x16
            _emit 0x4a
            _emit 0x00
            _emit 0xe9
            _emit 0x13
            _emit 0x4a
            _emit 0x00
            _emit 0xe7
            _emit 0x2a
            _emit 0x4a
            _emit 0x00
            _emit 0x97
            _emit 0x2c
            _emit 0x4a
            _emit 0x00
            _emit 0xd4
            _emit 0x0b
            _emit 0x4a
            _emit 0x00
            _emit 0xc9
            _emit 0x0c
            _emit 0x4a
            _emit 0x00
            _emit 0xcf
            _emit 0x0e
            _emit 0x4a
            _emit 0x00
            _emit 0xe2
            _emit 0x0f
            _emit 0x4a
            _emit 0x00
            _emit 0x5b
            _emit 0x11
            _emit 0x4a
            _emit 0x00
            _emit 0xa4
            _emit 0x17
            _emit 0x4a
            _emit 0x00
            _emit 0xad
            _emit 0x18
            _emit 0x4a
            _emit 0x00
            _emit 0x7d
            _emit 0x1a
            _emit 0x4a
            _emit 0x00
            _emit 0xc9
            _emit 0x1b
            _emit 0x4a
            _emit 0x00
            _emit 0xb9
            _emit 0x1c
            _emit 0x4a
            _emit 0x00
            _emit 0xa9
            _emit 0x1d
            _emit 0x4a
            _emit 0x00
            _emit 0xbe
            _emit 0x1e
            _emit 0x4a
            _emit 0x00
            _emit 0xe1
            _emit 0x1f
            _emit 0x4a
            _emit 0x00
            _emit 0xca
            _emit 0x20
            _emit 0x4a
            _emit 0x00
            _emit 0xb3
            _emit 0x21
            _emit 0x4a
            _emit 0x00
            _emit 0xa3
            _emit 0x22
            _emit 0x4a
            _emit 0x00
            _emit 0x93
            _emit 0x23
            _emit 0x4a
            _emit 0x00
            _emit 0xa6
            _emit 0x24
            _emit 0x4a
            _emit 0x00
            _emit 0xc2
            _emit 0x44
            _emit 0x4a
            _emit 0x00
            _emit 0x0b
            _emit 0x28
            _emit 0x4a
            _emit 0x00
            _emit 0x07
            _emit 0x29
            _emit 0x4a
            _emit 0x00
            _emit 0xf7
            _emit 0x29
            _emit 0x4a
            _emit 0x00
            _emit 0x47
            _emit 0x2e
            _emit 0x4a
            _emit 0x00
            _emit 0x69
            _emit 0x2f
            _emit 0x4a
            _emit 0x00
            _emit 0x59
            _emit 0x30
            _emit 0x4a
            _emit 0x00
            _emit 0x49
            _emit 0x31
            _emit 0x4a
            _emit 0x00
            _emit 0x39
            _emit 0x32
            _emit 0x4a
            _emit 0x00
            _emit 0x29
            _emit 0x33
            _emit 0x4a
            _emit 0x00
            _emit 0x09
            _emit 0x35
            _emit 0x4a
            _emit 0x00
            _emit 0xf9
            _emit 0x35
            _emit 0x4a
            _emit 0x00
            _emit 0xe9
            _emit 0x36
            _emit 0x4a
            _emit 0x00
            _emit 0xd9
            _emit 0x37
            _emit 0x4a
            _emit 0x00
            _emit 0xc9
            _emit 0x38
            _emit 0x4a
            _emit 0x00
            _emit 0xb9
            _emit 0x39
            _emit 0x4a
            _emit 0x00
            _emit 0xa9
            _emit 0x3a
            _emit 0x4a
            _emit 0x00
            _emit 0x99
            _emit 0x3b
            _emit 0x4a
            _emit 0x00
            _emit 0x89
            _emit 0x3c
            _emit 0x4a
            _emit 0x00
            _emit 0x79
            _emit 0x3d
            _emit 0x4a
            _emit 0x00
            _emit 0x69
            _emit 0x3e
            _emit 0x4a
            _emit 0x00
            _emit 0x07
            _emit 0x40
            _emit 0x4a
            _emit 0x00
            _emit 0xfb
            _emit 0x40
            _emit 0x4a
            _emit 0x00
            _emit 0x50
            _emit 0x42
            _emit 0x4a
            _emit 0x00
            _emit 0x40
            _emit 0x43
            _emit 0x4a
            _emit 0x00
            _emit 0x19
            _emit 0x34
            _emit 0x4a
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

#undef true
#undef false

void C2_HOOK_FASTCALL CheckCheckSum(tNet_message* pMessage) {
}

void C2_HOOK_FASTCALL StatReceivePacket(void) {
}

void C2_HOOK_FASTCALL NetReallyDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {
    pMessage->contents.raw.header.type = eNetMsg_none;
}

// FUNCTION: CARMA2_HW 0x004a5b50
int C2_NAKED C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int err;

    err = 0;
    if (gNumber_of_net_players == 1) {
        pMessage->contents.raw.header.type = eNetMsg_none;
        return err;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gThis_net_player_index != i) {
            err |= NetGuaranteedSendMessageToPlayer(pDetails, pMessage, gNet_players[i].ID, pNotifyFail);
        }
    }
    return err;
#else
    __asm {
            push ecx
            mov eax, dword ptr [gNumber_of_net_players]
            push ebx
            xor ebx, ebx
            push ebp
            cmp eax, 1
            mov ebp, edx
            mov dword ptr [esp + 8], ecx
            jne L004a5b71
            mov byte ptr [ebp + 0x19], 0xff
            xor eax, eax
            pop ebp
            pop ebx
            pop ecx
            ret 4
    L004a5b71:
            push edi
            xor edi, edi
            test eax, eax
            jle L004a5bac
            push esi
            mov esi, offset gNet_players+0x24
    L004a5b7e:
            cmp edi, dword ptr [gThis_net_player_index]
            je L004a5ba0
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esi]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x18]
            mov edx, ebp
            call NetGuaranteedSendMessageToPlayer
            or ebx, eax
            mov eax, dword ptr [gNumber_of_net_players]
    L004a5ba0:
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L004a5b7e
            pop esi
    L004a5bac:
            pop edi
            mov eax, ebx
            pop ebp
            pop ebx
            pop ecx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049eea0
int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer_id) {
            return PDNetSendMessageToAddress(pNet_game, pMessage, &gNet_players[i].pd_net_info);
        }
    }
    return -3;
}

// FUNCTION: CARMA2_HW 0x004a5140
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pPlayer, int pContents_size) {
    tNet_message* message;
    tNet_message_chunk* contents;

    if (pPlayer == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = pPlayer->field_0xcc;
    if (message != NULL && (tS32)(message->header.field_0x16 + pContents_size) > (tS32)(sizeof(tMax_message) - sizeof(void*))) {
        NetReallySendMessageToPlayer(gCurrent_net_game, message, pPlayer->ID);
        pPlayer->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        message = pPlayer->field_0xcc = NetAllocateMessage(sizeof(tMax_message) - sizeof(void*));
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    contents = (tNet_message_chunk*)((tU8*)message + message->header.field_0x16);
    message->header.field_0x16 += (tU16)pContents_size;
    message->header.field_0x14++;
    return contents;
}

// FUNCTION: CARMA2_HW 0x004a5b30
tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void) {
    return gNet_players[gThis_net_player_index].player_status;
}

// FUNCTION: CARMA2_HW 0x0049f9c0
tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tU32 pSize_decider) {
    int i;

    if (gINT_00659d34 >= 0) {
        PDFatalError("Broadcast stacks messed up");
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            gINT_00659d34 = i;
            return NetGetToPlayerContents(pType, pSize_decider, gNet_players[i].ID);
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049fb70
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage) {
    tNet_message_chunk* contents;
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && i != gINT_00659d34) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], pMessage->raw.header.size);
            memcpy(contents, pMessage, pMessage->raw.header.size);
        }
    }
    gINT_00659d34 = -1;
}

// FUNCTION: CARMA2_HW 0x004a5bc0
int C2_NAKED C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pArg3) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int result;

    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guarantee message not built proper");
    }
    pMessage->header.field_0x14 = 1;
    ReceivedMessage(pMessage, &gNet_players[gThis_net_player_index], GetRaceTime());
    pMessage->header.field_0x14 = 0xff;
    if (gNumber_of_net_players == 1) {
        ((tU8*)pMessage)[0x19] = 0xff;
        return 0;
    }
    result = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            result |= NetGuaranteedSendMessageToPlayer(pNet_game, pMessage, gNet_players[i].ID, pArg3);
        }
    }
    return result;
#else
    __asm {
            push ecx
            mov eax, dword ptr [gLocal_net_ID]
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov ebx, 0xff
            mov dword ptr [esi + 0x10], eax
            cmp word ptr [esi + 0x14], bx
            je L004a5bf0
            mov ecx, offset gAsmString_0065a2e8
            call PDFatalError
    L004a5bf0:
            mov word ptr [esi + 0x14], 1
            call GetRaceTime
            push eax
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            mov ecx, esi
            call ReceivedMessage
            mov word ptr [esi + 0x14], bx
            mov eax, dword ptr [gNumber_of_net_players]
            xor ebp, ebp
            cmp eax, 1
            jne L004a5c36
            mov byte ptr [esi + 0x19], bl
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 4
    L004a5c36:
            xor ebx, ebx
            test eax, eax
            jle L004a5c6e
            mov edi, offset gNet_players+0x24
    L004a5c41:
            cmp ebx, dword ptr [gThis_net_player_index]
            je L004a5c63
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [esp + 0x10]
            push edx
            push eax
            mov edx, esi
            call NetGuaranteedSendMessageToPlayer
            or ebp, eax
            mov eax, dword ptr [gNumber_of_net_players]
    L004a5c63:
            inc ebx
            add edi, 0xd4
            cmp ebx, eax
            jl L004a5c41
    L004a5c6e:
            pop edi
            mov eax, ebp
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 4
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

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004a5e20
int C2_NAKED C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pPlayer, undefined4 pArg4) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int idx;
    tNet_game_player_info* target;
    tNet_message* stack;
    tU8* src;
    tU8* dst;
    int total;
    int old_size;
    tU8* p;
    int k;

    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guaranteed message not built properly");
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == (tPlayer_ID)pPlayer) {
            goto found;
        }
    }
    return -1;

found:
    idx = i;
    if ((tPlayer_ID)pPlayer == gLocal_net_ID) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();
        pMessage->header.field_0x14 = 1;
        ReceivedMessage(pMessage, &gNet_players[idx], GetRaceTime());
        if (pMessage->header.field_0x14 != 0xff) {
            ((tU8*)pMessage)[0x19] = 0xff;
        }
        return 0;
    }
    AddToGuaranteeList(pMessage, NULL, (tPlayer_ID)pPlayer, (void*)pArg4);
    src = (tU8*)pMessage + sizeof(tNet_message_header);
    total = 0;
    p = src;
    for (k = 0; k < 2; k++) {
        total += *p;
        p += *p;
    }
    target = &gNet_players[idx];
    if (target == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    stack = target->field_0xcc;
    if (stack != NULL && stack->header.field_0x16 + total > 0x200) {
        tPlayer_ID tid = target->ID;
        tNet_game_details* game = gCurrent_net_game;

        if (gNet_mode != eNet_mode_none) {
            stack->header.field_0x8 = gLocal_net_ID;
            stack->header.field_0x10 = PDGetTotalTime();
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == tid) {
                    goto send_stack;
                }
            }
            goto clear_stack;

send_stack:
            PDNetSendMessageToAddress(game, stack, &gNet_players[i]);
        }
clear_stack:
        target->field_0xcc = stack = NULL;
    }
    if (stack == NULL) {
        stack = NetAllocateMessage(0x200);
        target->field_0xcc = stack;
        stack->header.field_0x16 = sizeof(tNet_message_header);
        stack->header.field_0x14 = 0;
    }
    old_size = stack->header.field_0x16;
    dst = (tU8*)stack + old_size;
    stack->header.field_0x14++;
    stack->header.field_0x16 = (tU16)(old_size + total);
    target->field_0xcc->header.field_0x14++;
    memcpy(dst, src, total);
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, edx
            mov ebx, 0xff
            push edi
            cmp word ptr [esi + 0x14], bx
            je L004a5e3b
            mov ecx, offset gAsmString_0065a334
            call PDFatalError
    L004a5e3b:
            mov ecx, dword ptr [gNumber_of_net_players]
            xor edi, edi
            test ecx, ecx
            jle L004a6013
            mov edx, dword ptr [esp + 0x14]
            mov eax, offset gNet_players+0x24
    L004a5e54:
            cmp dword ptr [eax], edx
            je L004a5e6c
            inc edi
            add eax, 0xd4
            cmp edi, ecx
            jl L004a5e54
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
    L004a5e6c:
            mov eax, dword ptr [gLocal_net_ID]
            cmp edx, eax
            jne L004a5eba
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov word ptr [esi + 0x14], 1
            call GetRaceTime
            mov ecx, esi
            push eax
            lea eax, [edi + edi*2]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, edi
            lea edx, [eax*4 + gNet_players]
            call ReceivedMessage
            cmp word ptr [esi + 0x14], bx
            je L004a600a
            mov byte ptr [esi + 0x19], bl
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
    L004a5eba:
            mov ecx, dword ptr [esp + 0x18]
            push ecx
            push edx
            xor edx, edx
            mov ecx, esi
            call AddToGuaranteeList
            lea ecx, [esi + 0x18]
            xor ebp, ebp
            mov dword ptr [esp + 0x18], ecx
            mov edx, 2
    L004a5ed7:
            xor eax, eax
            mov al, byte ptr [ecx]
            add ebp, eax
            add ecx, eax
            dec edx
            jne L004a5ed7
            lea eax, [edi + edi*2]
            lea esi, [eax + eax*8]
            mov eax, dword ptr [gThis_net_player_index]
            shl esi, 1
            lea ecx, [eax + eax*2]
            sub esi, edi
            shl esi, 2
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            lea edi, [esi + gNet_players]
            lea eax, [edx*4 + gNet_players]
            cmp edi, eax
            jne L004a5f19
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L004a5f19:
            mov ebx, dword ptr [edi + 0xcc]
            test ebx, ebx
            je L004a5fb2
            xor ecx, ecx
            mov cx, word ptr [ebx + 0x16]
            add ecx, ebp
            cmp ecx, 0x200
            jle L004a5fae
            mov eax, dword ptr [gNet_mode]
            mov edx, dword ptr [gCurrent_net_game]
            mov edi, dword ptr [edi + 0x24]
            mov dword ptr [esp + 0x14], edx
            test eax, eax
            je L004a5f9c
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [ebx + 8], eax
            call PDGetTotalTime
            mov dword ptr [ebx + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a5f9c
            mov ecx, offset gNet_players+0x24
    L004a5f6e:
            cmp dword ptr [ecx], edi
            je L004a5f7f
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a5f6e
            jmp L004a5f9c
    L004a5f7f:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            mov ecx, dword ptr [esp + 0x14]
            push edx
            mov edx, ebx
            call PDNetSendMessageToAddress
    L004a5f9c:
            lea edi, [esi + gNet_players]
            xor ebx, ebx
            mov dword ptr [edi + 0xcc], 0
    L004a5fae:
            test ebx, ebx
            jne L004a5fd0
    L004a5fb2:
            mov ecx, 0x200
            call NetAllocateMessage
            mov ebx, eax
            mov dword ptr [edi + 0xcc], eax
            mov word ptr [ebx + 0x16], 0x18
            mov word ptr [ebx + 0x14], 0
    L004a5fd0:
            mov ax, word ptr [ebx + 0x16]
            mov edi, ebx
            mov ecx, eax
            and ecx, 0xffff
            lea edx, [ebp + eax]
            add edi, ecx
            inc word ptr [ebx + 0x14]
            mov word ptr [ebx + 0x16], dx
            mov eax, dword ptr [esi + gNet_players+0xcc]
            mov esi, dword ptr [esp + 0x18]
            mov ecx, ebp
            inc word ptr [eax + 0x14]
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
    L004a600a:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
    L004a6013:
            pop edi
            pop esi
            pop ebp
            or eax, 0xffffffff
            pop ebx
            ret 8
            nop
            nop
            nop
    }
#endif
}
#pragma auto_inline(on)
// FUNCTION: CARMA2_HW 0x0049d210
int C2_HOOK_FASTCALL NetInitialise(void) {
    int i;

    InitAbuseomatic();
    gNet_service_disable = 0;
    gIn_net_service = 0;
    gMessage_header_size = PDNetGetHeaderSize();
    gOnly_receive_guarantee_replies = 0;
    gMin_messages = BrMemAllocate(MIN_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMin_message)), kMem_net_min_messages);
    gMid_messages = BrMemAllocate(MID_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMid_message)), kMem_net_mid_messages);
    gMax_messages = BrMemAllocate(MAX_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMax_message)), kMem_net_max_messages);

    for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMin_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMid_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMax_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    gNet_initialised = PDNetInitialise() == 0;
    if (gNet_initialised) {
        InitNetHeadups();
    }
    GenerateItFoxShadeTable();
    gDont_allow_joiners = 0;
    gUINT_0074a690 = PDGetTotalTime();
    gUINT_0074a718 = PDGetTotalTime();
    gNext_guarantee = 0;
    gDynamic_messages = NULL;
    gGuarantee_number = ((tU16)PDGetTotalTime() & 0x3ff) + 1;
    return !gNet_initialised;
}

int C2_HOOK_FASTCALL NetShutdown(void) {
    int err;

    err = PDNetShutdown();
    DisposeAbuseomatic();
    BrMemFree(gMin_messages);
    BrMemFree(gMid_messages);
    BrMemFree(gMax_messages);
    DisposeExcessMemory();
    DisposeNetHeadups();
    return err;
}

// FUNCTION: CARMA2_HW 0x0049d370
void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {
    if (gNet_initialised) {
        NetShutdown();
        gNet_initialised = 0;
    }
}

// DisableNetService

// ReenableNetService

// FUNCTION: CARMA2_HW 0x0049d400
int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void) {
    if (gIn_net_service) {
        gIn_net_service = 0;
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049d420
void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void) {
    gIn_net_service = 1;
}

// NetSendHeadupToAllPlayers

// NetSendHeadupToEverybody

// NetSendHeadupToPlayer

// InitialisePlayerStati

void C2_HOOK_FASTCALL LeaveTempGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        NetLeaveGameLowLevel();
    }
    gTime_for_next_one = 1;
    gCurrent_join_poll_game = NULL;
}

void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        BrMemFree(gCurrent_join_poll_game);
        gCurrent_join_poll_game = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0049ec70
void C2_NAKED C2_HOOK_FASTCALL DoNextJoinPoll(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gTime_for_next_one) {
        gCurrent_join_poll_game = NetAllocatePIDGameDetails();
        if (gCurrent_join_poll_game == NULL) {
            return;
        }
        if (PDNetGetNextJoinGame(gCurrent_join_poll_game, gJoin_poll_index)) {
            if (!NetJoinGameLowLevel(gCurrent_join_poll_game, "!TEMP!")) {
                tNet_message* message;
                gTime_for_next_one = 0;
                message = NetBuildMessage(0, 0);
                NetSendMessageToAddress(gCurrent_join_poll_game, message, &gCurrent_join_poll_game->pd_net_info);
                gBastard_has_answered = 0;
                gAsk_time = PDGetTotalTime();
            } else {
                DisposeCurrentJoinPollGame();
            }
            gJoin_poll_index += 1;
            return;
        } else {
            gJoin_poll_index = 0;
            DisposeCurrentJoinPollGame();
            return;
        }
    } else {
        if (gBastard_has_answered) {
            gAdd_proc(gCurrent_join_poll_game);
            LeaveTempGame();
        } else {
            if (PDGetTotalTime() - gAsk_time > 10000) {
                LeaveTempGame();
                DisposeCurrentJoinPollGame();
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [gTime_for_next_one]
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            cmp eax, ebx
            push edi
            je L0049ed97
            push 0xb5
            push 0x78
            call BrMemAllocate
            add esp, 8
            cmp eax, ebx
            mov dword ptr [gCurrent_join_poll_game], eax
            je L0049ee12
            mov edx, dword ptr [gJoin_poll_index]
            mov ecx, eax
            call PDNetGetNextJoinGame
            test eax, eax
            je L0049ed70
            mov ecx, dword ptr [gCurrent_join_poll_game]
            mov edx, offset gAsmString_0065a2b4
            call PDNetJoinGame
            test eax, eax
            jne L0049ed4d
            xor edx, edx
            xor cl, cl
            mov dword ptr [gTime_for_next_one], ebx
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            cmp esi, ebx
            je L0049ecfa
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], bl
    L0049ecfa:
            mov eax, dword ptr [gCurrent_join_poll_game]
            mov edi, eax
            mov ebp, eax
            cmp dword ptr [gNet_mode], ebx
            jne L0049ed13
            cmp dword ptr [gJoin_list_mode], ebx
            je L0049ed2d
    L0049ed13:
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov edx, esi
            mov ecx, ebp
            push edi
            mov dword ptr [esi + 0x10], eax
            call PDNetSendMessageToAddress
    L0049ed2d:
            mov dword ptr [gBastard_has_answered], ebx
            call PDGetTotalTime
            mov dword ptr [gAsk_time], eax
            mov eax, dword ptr [gJoin_poll_index]
            inc eax
            mov dword ptr [gJoin_poll_index], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0049ed4d:
            mov eax, dword ptr [gCurrent_join_poll_game]
            cmp eax, ebx
            je L0049ed65
            push eax
            call BrMemFree
            add esp, 4
            mov dword ptr [gCurrent_join_poll_game], ebx
    L0049ed65:
            inc dword ptr [gJoin_poll_index]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0049ed70:
            mov eax, dword ptr [gCurrent_join_poll_game]
            mov dword ptr [gJoin_poll_index], ebx
            cmp eax, ebx
            je L0049ee12
            push eax
            call BrMemFree
            add esp, 4
            mov dword ptr [gCurrent_join_poll_game], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0049ed97:
            cmp dword ptr [gBastard_has_answered], ebx
            je L0049edcb
            mov ecx, dword ptr [gCurrent_join_poll_game]
            call dword ptr [gAdd_proc]
            mov ecx, dword ptr [gCurrent_join_poll_game]
            cmp ecx, ebx
            je L0049ee02
            cmp dword ptr [gNet_mode], 2
            jne L0049edfd
            mov ecx, dword ptr [gCurrent_net_game]
            call PDNetHostFinishGame
            jmp L0049ee02
    L0049edcb:
            call PDGetTotalTime
            sub eax, dword ptr [gAsk_time]
            cmp eax, 0x2710
            jbe L0049ee12
            mov ecx, dword ptr [gCurrent_join_poll_game]
            cmp ecx, ebx
            je L0049ee02
            cmp dword ptr [gNet_mode], 2
            jne L0049edfd
            mov ecx, dword ptr [gCurrent_net_game]
            call PDNetHostFinishGame
            jmp L0049ee02
    L0049edfd:
            call PDNetLeaveGame
    L0049ee02:
            mov dword ptr [gTime_for_next_one], 1
            mov dword ptr [gCurrent_join_poll_game], ebx
    L0049ee12:
            pop edi
            pop esi
            pop ebp
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

// NetStartProducingJoinList

// NetEndJoinList

// FUNCTION: CARMA2_HW 0x0049dc70
void C2_HOOK_FASTCALL NetDisposePIDGameInfo(tNet_game_details* pDetails) {
    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

// NetDisposeGameDetails

tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);

    return BrMemAllocate(sizeof(tNet_game_details), kMem_net_pid_details);
}

void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void) {

    if (gNet_mode == eNet_mode_host) {
        PDNetHostFinishGame();
    } else {
        PDNetLeaveGame();
    }
}

// FUNCTION: CARMA2_HW 0x0049dc90
void C2_NAKED C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message* message;
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (i != gThis_net_player_index) {
                message = NetBuildMessage(NET_MSG_LEAVE, 0);
                NetReallySendMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID);
            }
        }
        NetLeaveGameLowLevel();
    }
    gNet_mode = eNet_mode_none;
    gNet_mode_of_last_game = eNet_mode_none;
    gNumber_of_net_players = 0;
    gThis_net_player_index = 0;
    DisposeNetStorageSpace();
    NetDisposeGameDetails(pNet_game);
    gCurrent_net_game = NULL;
#else
    tNet_message* message;
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (i != gThis_net_player_index) {
                message = NetBuildMessage(NET_MSG_LEAVE, 0);
                NetReallySendMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID);
            }
        }
        NetLeaveGameLowLevel();
    }
    gNet_mode = eNet_mode_none;
    gNet_mode_of_last_game = eNet_mode_none;
    gNumber_of_net_players = 0;
    gThis_net_player_index = 0;
    DisposeNetStorageSpace();
    NetDisposeGameDetails(pNet_game);
    gCurrent_net_game = NULL;
#endif
#else
    __asm {
            sub esp, 0x108
            push ebp
            push edi
            mov edi, ecx
            push offset gAsmString_0065a294
            mov dword ptr [esp + 0x10], edi
            call dr_dprintf
            mov eax, dword ptr [gNet_mode]
            xor ebp, ebp
            add esp, 4
            cmp eax, ebp
            je L0049e0d0
            push esi
            mov esi, 1
            cmp eax, 2
            push ebx
            mov dword ptr [gOnly_receive_guarantee_replies], esi
            jne L0049de42
            push offset gAsmString_0065a27c
            call dr_dprintf
            add esp, 4
            mov dword ptr [gDont_allow_joiners], esi
            call PDGetTotalTime
            mov dword ptr [gLast_flush_message], eax
            mov eax, dword ptr [gNumber_of_net_players]
            xor edi, edi
            cmp eax, ebp
            jle L0049dd27
            mov esi, offset gNet_players+0xcc
    L0049dcfd:
            mov edx, dword ptr [esi]
            cmp edx, ebp
            je L0049dd17
            mov eax, dword ptr [esi - 0xa8]
            mov ecx, dword ptr [gCurrent_net_game]
            push eax
            call NetReallySendMessageToPlayer
            mov dword ptr [esi], ebp
    L0049dd17:
            mov eax, dword ptr [gNumber_of_net_players]
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L0049dcfd
    L0049dd27:
            push offset gAsmString_0065a264
            call dr_dprintf
            add esp, 4
            xor edx, edx
            mov cl, 9
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            xor edx, edx
            mov cl, 9
            mov dword ptr [esp + 0x10], eax
            call NetGetContentsSize
            mov ecx, dword ptr [esp + 0x10]
            lea edi, [eax + ecx + 0x18]
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            cmp esi, ebp
            je L0049dd93
            mov al, byte ptr [esp + 0x10]
            mov byte ptr [esi + 0x18], bl
            and ebx, 0xff
            mov word ptr [esi + 0x14], 0xff
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 9
            xor edx, edx
            mov byte ptr [esi + ebx + 0x19], 5
            mov dl, byte ptr [esi + 0x18]
            mov byte ptr [esi + edx + 0x18], al
    L0049dd93:
            push offset gAsmString_0065a24c
            call dr_dprintf
            mov eax, dword ptr [gIn_net_service]
            add esp, 4
            cmp eax, ebp
            je L0049ddb9
            mov dword ptr [gIn_net_service], ebp
            mov dword ptr [esp + 0x10], 1
            jmp L0049ddbd
    L0049ddb9:
            mov dword ptr [esp + 0x10], ebp
    L0049ddbd:
            push offset gAsmString_0065a234
            call dr_dprintf
            mov eax, dword ptr [gNumber_of_net_players]
            add esp, 4
            cmp eax, 1
            jne L0049ddda
            mov byte ptr [esi + 0x19], 0xff
            jmp L0049de0f
    L0049ddda:
            xor ebx, ebx
            test eax, eax
            jle L0049de0f
            mov edi, offset gNet_players+0x24
    L0049dde5:
            cmp ebx, dword ptr [gThis_net_player_index]
            je L0049de04
            mov ecx, dword ptr [edi]
            push 0
            push ecx
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, esi
            call NetGuaranteedSendMessageToPlayer
            or ebp, eax
            mov eax, dword ptr [gNumber_of_net_players]
    L0049de04:
            inc ebx
            add edi, 0xd4
            cmp ebx, eax
            jl L0049dde5
    L0049de0f:
            push offset gAsmString_0065a21c
            call dr_dprintf
            mov eax, dword ptr [esp + 0x14]
            add esp, 4
            test eax, eax
            je L0049de2e
            mov dword ptr [gIn_net_service], 1
    L0049de2e:
            push offset gAsmString_0065a204
            call dr_dprintf
            add esp, 4
            xor ebp, ebp
            jmp L0049df88
    L0049de42:
            cmp dword ptr [gINT_0068d968], ebp
            jne L0049df88
            xor edx, edx
            mov cl, 8
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            xor edx, edx
            mov cl, 8
            mov dword ptr [esp + 0x10], eax
            call NetGetContentsSize
            mov edx, dword ptr [esp + 0x10]
            lea esi, [eax + edx + 0x18]
            mov ecx, esi
            call NetAllocateMessage
            cmp eax, ebp
            je L0049deab
            mov dl, byte ptr [esp + 0x10]
            mov byte ptr [eax + 0x18], bl
            and ebx, 0xff
            mov word ptr [eax + 0x14], 0xff
            mov word ptr [eax + 0x16], si
            mov byte ptr [eax + 0x19], 8
            xor ecx, ecx
            mov byte ptr [eax + ebx + 0x19], 5
            mov cl, byte ptr [eax + 0x18]
            mov byte ptr [eax + ecx + 0x18], dl
    L0049deab:
            mov ecx, dword ptr [gNet_players+0x24]
            push ebp
            push ecx
            mov edx, eax
            mov ecx, edi
            call NetGuaranteedSendMessageToPlayer
            mov edi, offset gProgram_state+0x1a10
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x18]
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
            mov ecx, 0x4a
            call GetMiscString
            mov edx, eax
            mov edi, 0x58f640
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea ebx, [esp + 0x18]
            mov esi, edi
            mov edi, ebx
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea ebx, [esp + 0x18]
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gNet_mode]
            mov ecx, edx
            and ecx, 3
            cmp eax, ebp
            rep movsb
            je L0049df88
            xor edx, edx
            mov cl, 0xd
            call NetStartBroadcastContents
            mov edx, eax
            cmp edx, ebp
            je L0049df88
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            lea ebx, [edx + 2]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, edx
            call NetBroadcastContents
    L0049df88:
            push offset gAsmString_0065a1ec
            call dr_dprintf
            mov eax, dword ptr [gNumber_of_net_players]
            add esp, 4
            xor esi, esi
            cmp eax, ebp
            jle L0049dfb1
    L0049dfa0:
            mov ecx, esi
            call DisposeCarN
            mov eax, dword ptr [gNumber_of_net_players]
            inc esi
            cmp esi, eax
            jl L0049dfa0
    L0049dfb1:
            push offset gAsmString_0065a1d4
            call dr_dprintf
            add esp, 4
            mov ecx, offset gOur_car_storage_space
            call ClearOutStorageSpace
            push offset gAsmString_0065a1bc
            call dr_dprintf
            add esp, 4
            mov ecx, offset gNet_cars_storage_space
            call ClearOutStorageSpace
            push offset gAsmString_0065a1a4
            call dr_dprintf
            mov eax, dword ptr [gIn_net_service]
            add esp, 4
            cmp eax, ebp
            je L0049e002
            mov dword ptr [gIn_net_service], ebp
            mov ebx, 1
            jmp L0049e004
    L0049e002:
            xor ebx, ebx
    L0049e004:
            call PDGetTotalTime
            mov dword ptr [gLast_flush_message], eax
            mov eax, dword ptr [gNumber_of_net_players]
            xor edi, edi
            cmp eax, ebp
            jle L0049e048
            mov esi, offset gNet_players+0xcc
    L0049e01e:
            mov edx, dword ptr [esi]
            cmp edx, ebp
            je L0049e038
            mov ecx, dword ptr [esi - 0xa8]
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetReallySendMessageToPlayer
            mov dword ptr [esi], ebp
    L0049e038:
            mov eax, dword ptr [gNumber_of_net_players]
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L0049e01e
    L0049e048:
            call PDGetTotalTime
            mov esi, eax
            mov eax, dword ptr [gNext_guarantee]
            cmp eax, ebp
            je L0049e075
    L0049e058:
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x1388
            jae L0049e075
            xor ecx, ecx
            call NetService
            cmp dword ptr [gNext_guarantee], ebp
            jne L0049e058
    L0049e075:
            mov eax, dword ptr [gNet_mode]
            mov ecx, dword ptr [gCurrent_net_game]
            mov esi, 2
            cmp eax, esi
            jne L0049e090
            call PDNetHostFinishGame
            jmp L0049e095
    L0049e090:
            call PDNetLeaveGame
    L0049e095:
            cmp ebx, ebp
            je L0049e0a3
            mov dword ptr [gIn_net_service], 1
    L0049e0a3:
            push offset gAsmString_0065a18c
            mov dword ptr [gCurrent_net_game], ebp
            mov dword ptr [gNet_mode], ebp
            mov dword ptr [gNumber_of_net_players], ebp
            mov dword ptr [gProgram_state+0xa4], esi
            mov dword ptr [gOnly_receive_guarantee_replies], ebp
            call dr_dprintf
            add esp, 4
            pop ebx
            pop esi
    L0049e0d0:
            pop edi
            pop ebp
            add esp, 0x108
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jmp PDNetSetPlayerSystemInfo
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
            jmp PDNetDisposePlayer
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

// NetSetPlayerSystemInfo

// FUNCTION: CARMA2_HW 0x0049e3a0
void C2_NAKED C2_HOOK_FASTCALL FillInThisPlayer(tNet_game_details* pGame, tNet_game_player_info* pPlayer, int pCar_index, int pHost) {
#ifndef CARPOCALYPSE2_MATCHING
    int car_choice;

    pPlayer->host = pHost;
    pPlayer->ID = PDNetExtractPlayerID(pGame);
    strcpy(pPlayer->player_name, gProgram_state.player_name);
    pPlayer->last_heard_from_him = PDGetTotalTime();
    pPlayer->car_index = pCar_index;
    pPlayer->reposition_time = 0;
    pPlayer->last_waste_message = 0;
    pPlayer->wasteage_attributed = 0;
    pPlayer->field_0x58 = 0;
    pPlayer->name_not_clipped = 1;
    pPlayer->field_0x7c = 0;
    pPlayer->field_0x88 = 0;
    pPlayer->score = 0;
    pPlayer->field_0x80 = 0;
    pPlayer->field_0x84 = 0;
    pPlayer->field_0xcc = 0;
    pPlayer->field_0xd0 = 0;
    car_choice = pGame->options.car_choice;
    if (car_choice == eNet_car_all || car_choice == eNet_car_novelty) {
        pPlayer->next_car_index = -1;
    }
    InitialisePlayerScore(pPlayer);
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            push ebx
            push ebp
            mov ebx, edx
            push esi
            push edi
            mov ebp, ecx
            mov dword ptr [ebx + 0x20], eax
            _emit 0xe8
        _emit 0x7c
        _emit 0xbd
        _emit 0x07
        _emit 0x00
            mov dword ptr [ebx + 0x24], eax
            mov edi, offset gProgram_state+0x1a10
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebx + 0x28]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            call PDGetTotalTime
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [ebx + 0x10], eax
            xor eax, eax
            mov dword ptr [ebx + 0x4c], ecx
            mov dword ptr [ebx + 0x18], eax
            mov dword ptr [ebx + 0x1c], eax
            mov dword ptr [ebx + 0x70], eax
            mov dword ptr [ebx + 0x58], eax
            mov dword ptr [ebx + 0x74], 1
            mov dword ptr [ebx + 0x7c], eax
            mov dword ptr [ebx + 0x88], eax
            mov dword ptr [ebx + 0x90], eax
            mov dword ptr [ebx + 0x80], eax
            mov dword ptr [ebx + 0x84], eax
            mov dword ptr [ebx + 0xcc], eax
            mov dword ptr [ebx + 0xd0], eax
            mov ebp, dword ptr [ebp + 0x70]
            cmp ebp, 2
            je L0049e434
            cmp ebp, 1
            jne L0049e43e
    L0049e434:
            mov dword ptr [ebx + 0x8c], 0xffffffff
    L0049e43e:
            mov ecx, ebx
            call InitialisePlayerScore
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049e100
void C2_HOOK_FASTCALL DisposeCarN(int pPlayer_index) {
    int i;
    int j;

    if (gNet_players[pPlayer_index].car != NULL) {
        for (i = 0; i < gCurrent_race.number_of_racers; i++) {
            if (gCurrent_race.opponent_list[i].car_spec == gNet_players[pPlayer_index].car) {
                gCurrent_race.number_of_racers--;
                for (j = i; j < gCurrent_race.number_of_racers; j++) {
                    gCurrent_race.opponent_list[j] = gCurrent_race.opponent_list[j + 1];
                }
            }
        }
        PHILRemoveObject(gNet_players[pPlayer_index].car->collision_info);
        if (gProgram_state.racing != 0) {
            RemoveCarFromEffects(gNet_players[pPlayer_index].car);
        }
        DisposeCar(gNet_players[pPlayer_index].car, gNet_players[pPlayer_index].car_index);
        if (pPlayer_index != gThis_net_player_index) {
            BrMemFree(gNet_players[pPlayer_index].car);
            gNet_players[pPlayer_index].car = NULL;
        }
        if (gAction_replay_mode != 0) {
            ToggleReplay(NULL, NULL);
        }
        ARResetPiping();
    }
}

// FUNCTION: CARMA2_HW 0x0049e450
void C2_NAKED C2_HOOK_FASTCALL NetPlayersChanged(int pPlayer_count, tNet_game_player_info* pPlayers) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    int k;
    int found;
    int opponent_index;
    tCar_spec* car;
    tCar_spec* other_car;
    tNet_game_player_info* player;
    tPlayer_ID it_player_id;

    gNet_service_disable = 1;
    if (gCurrent_net_game->type == 7) {
        it_player_id = gNet_players[gIt_or_fox].ID;
    }
    for (i = 0; i < pPlayer_count; i++) {
        player = &pPlayers[i];
        found = 0;
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (player->ID == gNet_players[j].ID) {
                found = 1;
                player->last_waste_message = gNet_players[j].last_waste_message;
                player->player_status = gNet_players[j].player_status;
                player->car_index = gNet_players[j].car_index;
                player->score2 = gNet_players[j].score2;
                player->credits = gNet_players[j].credits;
                player->wasted = gNet_players[j].wasted;
                player->wasteage_attributed = gNet_players[j].wasteage_attributed;
                player->field_0x58 = gNet_players[j].field_0x58;
                player->next_car_index = gNet_players[j].next_car_index;
                player->car = gNet_players[j].car;
                break;
            }
        }
        if (!found) {
            if (player->ID == gLocal_net_ID) {
                car = &gProgram_state.current_car;
                LoadCar(gOpponents[player->car_index].car_file_name,
                    eDriver_local_human,
                    car,
                    player->car_index,
                    player->player_name,
                    &gNet_cars_storage_space);
                player->car = car;
                if (player->player_status < 6) {
                    car->disabled = 1;
                }
            } else {
                car = BrMemAllocate(0x1964, kMem_net_car_spec);
                player->car = car;
                LoadCar(gOpponents[player->car_index].car_file_name,
                    eDriver_net_human,
                    car,
                    player->car_index,
                    player->player_name,
                    &gNet_cars_storage_space);
                InitialiseCar(car);
                if (player->player_status < 6) {
                    car->disabled = 1;
                }
            }
            SetCarStorageTexturingLevel(&gNet_cars_storage_space, GetCarTexturingLevel(), eCTL_full);
            player->opponent_list_index = i;
            gCurrent_race.opponent_list[i].index = -1;
            gCurrent_race.opponent_list[i].net_player_index = i;
            gCurrent_race.opponent_list[i].ranking = IRandomBetween(0, 99);
            gCurrent_race.opponent_list[i].car_spec = player->car;
            gCurrent_race.number_of_racers = pPlayer_count;
            if (gNet_mode == 2) {
                AddPlayerToShapeStatusLists(player->ID, pPlayer_count, pPlayers);
            }
        }
    }

    for (i = 0; i < gNumber_of_net_players; i++) {
        found = 0;
        for (j = 0; j < pPlayer_count; j++) {
            if (gNet_players[i].ID == pPlayers[j].ID) {
                found = 1;
                break;
            }
        }
        if (!found) {
            opponent_index = gNumber_of_net_players;
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (gCurrent_race.opponent_list[j].net_player_index == i) {
                    opponent_index = j;
                    break;
                }
            }
            if (opponent_index < gNumber_of_net_players) {
                for (k = opponent_index; k < gNumber_of_net_players - 1; k++) {
                    gCurrent_race.opponent_list[k] = gCurrent_race.opponent_list[k + 1];
                }
            }
            for (j = 0; j < pPlayer_count; j++) {
                if (pPlayers[j].opponent_list_index > opponent_index) {
                    pPlayers[j].opponent_list_index--;
                }
            }

            car = gNet_players[i].car;
            if (car != NULL) {
                for (j = 0; j < gNumber_of_net_players; j++) {
                    other_car = gNet_players[j].car;
                    if (other_car != NULL) {
                        if (*(tCar_spec**)((tU8*)other_car + 0x1560) == car) {
                            *(tCar_spec**)((tU8*)other_car + 0x1560) = NULL;
                        }
                        if (*(tCar_spec**)((tU8*)other_car + 0x1564) == car) {
                            *(tCar_spec**)((tU8*)other_car + 0x1564) = NULL;
                        }
                        if (*(tCar_spec**)((tU8*)other_car + 0xc8) == car) {
                            *(tCar_spec**)((tU8*)other_car + 0xc8) = NULL;
                        }
                        if (*(tCar_spec**)((tU8*)other_car + 0x13ec) == car) {
                            *(tCar_spec**)((tU8*)other_car + 0x13ec) = NULL;
                        }
                        if (*(tCar_spec**)((tU8*)other_car + 0x13f0) == car) {
                            *(tCar_spec**)((tU8*)other_car + 0x13f0) = NULL;
                        }
                    }
                }
                RemoveFromCloakingList(car);
                PHILRemoveObject(car->collision_info);
                RemoveCarsOilSlicks(car);
                RemoveExplosions(car);
                FUN_004ddde0(car);
                RemoveRepulseLinksForCar(car);
            }

            if (gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty) {
                if (gNet_players[i].car_index >= 0) {
                    gCar_details[gNet_players[i].car_index].ownership = eCar_owner_none;
                }
                if (gNet_players[i].next_car_index >= 0) {
                    gCar_details[gNet_players[i].next_car_index].ownership = eCar_owner_none;
                }
            }
            DisposeCarN(i);
            if (gInitialised_grid != 0) {
                for (i = 0; i < pPlayer_count; i++) {
                    opponent_index = pPlayers[i].opponent_list_index;
                    gCurrent_race.opponent_list[opponent_index].net_player_index = i;
                }
            }
            if (gNet_mode == 2) {
                RemovePlayerFromShapeStatusLists(pPlayers[i].ID, pPlayer_count, pPlayers);
            }
        }
    }

    gNumber_of_net_players = pPlayer_count;
    memcpy(gNet_players, pPlayers, pPlayer_count * sizeof(tNet_game_player_info));
    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[i].this_players_time_stamp = PDGetTotalTime();
    }
    ForceRebuildActiveCarList();
    if (gCurrent_net_game->type == 7) {
        gIt_or_fox = -1;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (it_player_id == gNet_players[i].ID) {
                gIt_or_fox = i;
                gNet_service_disable = 0;
                return;
            }
        }
    } else if (gCurrent_net_game->type == 6) {
        CheckForVampireWinner();
    }
    gNet_service_disable = 0;
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gCurrent_net_game]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov dword ptr [gNet_service_disable], 1
            mov ecx, dword ptr [eax + 0x74]
            cmp ecx, 7
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], edi
            jne L0049e492
            mov eax, dword ptr [gIt_or_fox]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov eax, dword ptr [ecx*4 + gNet_players+0x24]
            mov dword ptr [esp + 0x18], eax
    L0049e492:
            xor ebp, ebp
            test edi, edi
            jle L0049e660
            mov ebx, offset gCurrent_race+0xcec
            lea esi, [edx + 0x64]
    L0049e4a4:
            mov eax, dword ptr [gNumber_of_net_players]
            xor ecx, ecx
            test eax, eax
            mov edx, 1
            jle L0049e540
            mov edi, dword ptr [esi - 0x40]
            mov eax, offset gNet_players+0x24
    L0049e4c0:
            cmp edi, dword ptr [eax]
            je L0049e4d4
            inc ecx
            add eax, 0xd4
            cmp ecx, dword ptr [gNumber_of_net_players]
            jl L0049e4c0
            jmp L0049e53c
    L0049e4d4:
            lea eax, [ecx + ecx*2]
            xor edx, edx
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ecx
            shl eax, 2
            mov ecx, dword ptr [eax + gNet_players+0x1c]
            mov dword ptr [esi - 0x48], ecx
            mov ecx, dword ptr [eax + gNet_players+0x48]
            mov dword ptr [esi - 0x1c], ecx
            mov ecx, dword ptr [eax + gNet_players+0x4c]
            mov dword ptr [esi - 0x18], ecx
            mov ecx, dword ptr [eax + gNet_players+0x64]
            mov dword ptr [esi], ecx
            mov ecx, dword ptr [eax + gNet_players+0x68]
            mov dword ptr [esi + 4], ecx
            mov ecx, dword ptr [eax + gNet_players+0x6c]
            mov dword ptr [esi + 8], ecx
            mov ecx, dword ptr [eax + gNet_players+0x70]
            mov dword ptr [esi + 0xc], ecx
            mov ecx, dword ptr [eax + gNet_players+0x58]
            mov dword ptr [esi - 0xc], ecx
            mov ecx, dword ptr [eax + gNet_players+0x8c]
            mov dword ptr [esi + 0x28], ecx
            mov eax, dword ptr [eax + gNet_players+0xc8]
            mov dword ptr [esi + 0x64], eax
    L0049e53c:
            mov edi, dword ptr [esp + 0x10]
    L0049e540:
            test edx, edx
            je L0049e64e
            mov ecx, dword ptr [esi - 0x40]
            mov eax, dword ptr [gLocal_net_ID]
            cmp ecx, eax
            jne L0049e5a0
            mov eax, dword ptr [esi - 0x18]
            lea edx, [esi - 0x3c]
            push offset gNet_cars_storage_space
            push edx
            lea ecx, [eax*8]
            push eax
            sub ecx, eax
            mov eax, dword ptr [gOpponents]
            shl ecx, 6
            push offset gProgram_state+0xac
            mov edx, 8
            lea ecx, [ecx + eax + 0x50]
            call LoadCar
            mov dword ptr [esi + 0x64], offset gProgram_state+0xac
            mov eax, dword ptr [esi - 0x1c]
            cmp eax, 6
            jge L0049e5fb
            mov dword ptr [gProgram_state+0xb0], 1
            jmp L0049e5fb
    L0049e5a0:
            push 0xb6
            push 0x1964
            call BrMemAllocate
            mov ecx, dword ptr [esi - 0x18]
            add esp, 8
            lea edx, [esi - 0x3c]
            mov dword ptr [esi + 0x64], eax
            push offset gNet_cars_storage_space
            push edx
            push ecx
            push eax
            lea eax, [ecx*8]
            mov edx, 7
            sub eax, ecx
            mov ecx, dword ptr [gOpponents]
            shl eax, 6
            lea ecx, [eax + ecx + 0x50]
            call LoadCar
            mov ecx, dword ptr [esi + 0x64]
            call InitialiseCar
            cmp dword ptr [esi - 0x1c], 6
            jge L0049e5fb
            mov edx, dword ptr [esi + 0x64]
            mov dword ptr [edx + 4], 1
    L0049e5fb:
            push 2
            call GetCarTexturingLevel
            mov edx, eax
            mov ecx, offset gNet_cars_storage_space
            call SetCarStorageTexturingLevel
            mov edx, 0x63
            xor ecx, ecx
            mov dword ptr [ebx - 8], 0xffffffff
            call IRandomBetween
            mov dword ptr [ebx], eax
            mov eax, dword ptr [esi + 0x64]
            mov dword ptr [ebx + 4], eax
            mov dword ptr [ebx - 4], ebp
            mov dword ptr [esi - 8], ebp
            mov eax, dword ptr [gNet_mode]
            cmp eax, 2
            mov dword ptr [gCurrent_race+0x90], edi
            jne L0049e64e
            mov ecx, dword ptr [esp + 0x14]
            mov edx, edi
            push ecx
            mov ecx, dword ptr [esi - 0x40]
            call AddPlayerToShapeStatusLists
    L0049e64e:
            inc ebp
            add ebx, 0x10
            add esi, 0xd4
            cmp ebp, edi
            jl L0049e4a4
    L0049e660:
            mov eax, dword ptr [gNumber_of_net_players]
            xor ebx, ebx
            test eax, eax
            jle L0049e87c
    L0049e66f:
            xor esi, esi
            xor ecx, ecx
            test edi, edi
            jle L0049e6a4
            lea eax, [ebx + ebx*2]
            lea edx, [eax + eax*8]
            mov eax, dword ptr [esp + 0x14]
            shl edx, 1
            sub edx, ebx
            add eax, 0x24
            mov edx, dword ptr [edx*4 + gNet_players+0x24]
    L0049e68f:
            cmp edx, dword ptr [eax]
            je L0049e69f
            inc ecx
            add eax, 0xd4
            cmp ecx, edi
            jl L0049e68f
            jmp L0049e6a4
    L0049e69f:
            mov esi, 1
    L0049e6a4:
            test esi, esi
            jne L0049e86e
            mov eax, dword ptr [gNumber_of_net_players]
            xor edx, edx
            test eax, eax
            jle L0049e71f
            mov eax, offset gCurrent_race+0xce8
    L0049e6bc:
            cmp dword ptr [eax], ebx
            je L0049e6d0
            mov ecx, dword ptr [gNumber_of_net_players]
            inc edx
            add eax, 0x10
            cmp edx, ecx
            jl L0049e6bc
            jmp L0049e71f
    L0049e6d0:
            mov ecx, dword ptr [gNumber_of_net_players]
            mov eax, edx
            sub ecx, edx
            shl eax, 4
            dec ecx
            shl ecx, 4
            lea esi, [eax + gCurrent_race+0xcf4]
            lea edi, [eax + gCurrent_race+0xce4]
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x10]
            and ecx, 3
            test eax, eax
            rep movsb
            jle L0049e71f
            mov ecx, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0x10]
            lea eax, [ecx + 0x5c]
    L0049e70e:
            mov ecx, dword ptr [eax]
            cmp ecx, edx
            jle L0049e717
            dec ecx
            mov dword ptr [eax], ecx
    L0049e717:
            add eax, 0xd4
            dec esi
            jne L0049e70e
    L0049e71f:
            lea eax, [ebx + ebx*2]
            xor ebp, ebp
            lea edi, [eax + eax*8]
            shl edi, 1
            sub edi, ebx
            shl edi, 2
            mov esi, dword ptr [edi + gNet_players+0xc8]
            cmp esi, ebp
            je L0049e7d3
            mov eax, dword ptr [gNumber_of_net_players]
            xor edx, edx
            cmp eax, ebp
            jle L0049e7a8
            mov ecx, offset gNet_players+0xc8
    L0049e74c:
            mov eax, dword ptr [ecx]
            cmp eax, ebp
            je L0049e798
            cmp dword ptr [eax + 0x1560], esi
            jne L0049e760
            mov dword ptr [eax + 0x1560], ebp
    L0049e760:
            cmp dword ptr [eax + 0x1564], esi
            jne L0049e76e
            mov dword ptr [eax + 0x1564], ebp
    L0049e76e:
            cmp dword ptr [eax + 0xc8], esi
            jne L0049e77c
            mov dword ptr [eax + 0xc8], ebp
    L0049e77c:
            cmp dword ptr [eax + 0x13ec], esi
            jne L0049e78a
            mov dword ptr [eax + 0x13ec], ebp
    L0049e78a:
            cmp dword ptr [eax + 0x13f0], esi
            jne L0049e798
            mov dword ptr [eax + 0x13f0], ebp
    L0049e798:
            mov eax, dword ptr [gNumber_of_net_players]
            inc edx
            add ecx, 0xd4
            cmp edx, eax
            jl L0049e74c
    L0049e7a8:
            mov ecx, esi
            call RemoveFromCloakingList
            mov ecx, dword ptr [esi + 8]
            call PHILRemoveObject
            mov ecx, esi
            call RemoveCarsOilSlicks
            mov ecx, esi
            call RemoveExplosions
            mov ecx, esi
            call FUN_004ddde0
            mov ecx, esi
            call RemoveRepulseLinksForCar
    L0049e7d3:
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x70]
            cmp eax, 2
            je L0049e7e6
            cmp eax, 1
            jne L0049e80e
    L0049e7e6:
            mov eax, dword ptr [edi + gNet_players+0x4c]
            cmp eax, ebp
            jl L0049e7fa
            lea eax, [eax + eax*4]
            mov dword ptr [eax*4 + gCar_details], ebp
    L0049e7fa:
            mov eax, dword ptr [edi + gNet_players+0x8c]
            cmp eax, ebp
            jl L0049e80e
            lea ecx, [eax + eax*4]
            mov dword ptr [ecx*4 + gCar_details], ebp
    L0049e80e:
            mov ecx, ebx
            call DisposeCarN
            cmp dword ptr [gInitialised_grid], ebp
            je L0049e845
            mov edx, dword ptr [esp + 0x10]
            xor ebx, ebx
            cmp edx, ebp
            jle L0049e849
            mov eax, dword ptr [esp + 0x14]
            add eax, 0x5c
    L0049e82e:
            mov ecx, dword ptr [eax]
            add eax, 0xd4
            shl ecx, 4
            mov dword ptr [ecx + gCurrent_race+0xce8], ebx
            inc ebx
            cmp ebx, edx
            jl L0049e82e
            jmp L0049e849
    L0049e845:
            mov edx, dword ptr [esp + 0x10]
    L0049e849:
            cmp dword ptr [gNet_mode], 2
            jne L0049e86a
            mov ecx, dword ptr [esp + 0x14]
            lea eax, [ebx + ebx*2]
            push ecx
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ebx
            mov ecx, dword ptr [ecx + eax*4 + 0x24]
            call RemovePlayerFromShapeStatusLists
    L0049e86a:
            mov edi, dword ptr [esp + 0x10]
    L0049e86e:
            mov eax, dword ptr [gNumber_of_net_players]
            inc ebx
            cmp ebx, eax
            jl L0049e66f
    L0049e87c:
            lea eax, [edi + edi*2]
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [gNumber_of_net_players], edi
            lea ecx, [eax + eax*8]
            mov eax, dword ptr [esp + 0x10]
            shl ecx, 1
            sub ecx, edi
            mov edi, offset gNet_players
            shl ecx, 2
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            xor esi, esi
            test eax, eax
            jle L0049e8cc
            mov edi, offset gNet_players+0x14
    L0049e8b5:
            call PDGetTotalTime
            mov dword ptr [edi], eax
            mov eax, dword ptr [gNumber_of_net_players]
            inc esi
            add edi, 0xd4
            cmp esi, eax
            jl L0049e8b5
    L0049e8cc:
            call ForceRebuildActiveCarList
            mov eax, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [eax + 0x74]
            cmp eax, 7
            jne L0049e937
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            mov dword ptr [gIt_or_fox], 0xffffffff
            jle L0049e941
            mov ecx, offset gNet_players+0x24
    L0049e8f9:
            mov esi, dword ptr [esp + 0x18]
            mov edi, dword ptr [ecx]
            cmp esi, edi
            je L0049e920
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049e8f9
            mov dword ptr [gNet_service_disable], 0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L0049e920:
            mov dword ptr [gIt_or_fox], eax
            mov dword ptr [gNet_service_disable], 0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L0049e937:
            cmp eax, 6
            jne L0049e941
            call CheckForVampireWinner
    L0049e941:
            pop edi
            pop esi
            pop ebp
            mov dword ptr [gNet_service_disable], 0
            pop ebx
            add esp, 0xc
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
            jmp PDNetInitClient
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

// LoadCarN

// DisposeCarN

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName) {

    return PDNetJoinGame(pGame_details, pName);
}

// NetJoinGame

// FUNCTION: CARMA2_HW 0x0049ee20
void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}

// NetExtractGameID
tU32 C2_HOOK_FASTCALL NetExtractGameID(tNet_game_details* pDetails) {

    return PDNetExtractGameID(pDetails);
}

// NetExtractPlayerID
tPlayer_ID C2_HOOK_FASTCALL NetExtractPlayerID(tNet_game_details* pDetails) {

    return PDNetExtractPlayerID(pDetails);
}

// FUNCTION: CARMA2_HW 0x0049ee50
int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {
    if (gNet_mode == eNet_mode_none && !gJoin_list_mode) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
}

// NetReallySendMessageToPlayer

// FUNCTION: CARMA2_HW 0x0049ef20
int C2_NAKED C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int contents_size;
    tNet_message* message;
    tU8* destination;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID != pPlayer_id) {
            continue;
        }
        contents_size = pMessage->header.field_0x16 - sizeof(tNet_message_header);
        pMessage->contents.raw.header.size = (tU8)contents_size;
        if (&gNet_players[i] == &gNet_players[gThis_net_player_index]) {
            PDFatalError("Trying to send network message to self");
        }
        message = gNet_players[i].field_0xcc;
        if (message != NULL && message->header.field_0x16 + contents_size > 0x200) {
            NetReallySendMessageToPlayer(pNet_game, message, gNet_players[i].ID);
            gNet_players[i].field_0xcc = message = NULL;
        }
        if (message == NULL) {
            message = NetAllocateMessage(0x200);
            gNet_players[i].field_0xcc = message;
            message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
            message->header.field_0x14 = 0;
        }
        destination = (tU8*)message + message->header.field_0x16;
        message->header.field_0x14++;
        message->header.field_0x16 += (tU16)contents_size;
        memcpy(destination, (tU8*)pMessage + sizeof(tNet_message_header), contents_size);
        if (pMessage->header.field_0x14 != 0xff) {
            pMessage->contents.raw.header.type = 0xff;
        }
        return 0;
    }
    if (pMessage->header.field_0x14 != 0xff) {
        pMessage->contents.raw.header.type = 0xff;
    }
    return -3;
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gNumber_of_net_players]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], 0
            jle L0049f074
            mov ebp, offset gNet_players+0x24
    L0049ef45:
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [ebp]
            cmp ecx, eax
            jne L0049f057
            xor ebx, ebx
            mov bx, word ptr [edx + 0x16]
            sub ebx, 0x18
            mov byte ptr [edx + 0x18], bl
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea eax, [ebp - 0x24]
            lea edx, [ecx*4 + gNet_players]
            cmp eax, edx
            jne L0049ef87
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L0049ef87:
            mov esi, dword ptr [ebp + 0xa8]
            test esi, esi
            je L0049f016
            xor ecx, ecx
            mov cx, word ptr [esi + 0x16]
            add ecx, ebx
            cmp ecx, 0x200
            jle L0049f012
            mov eax, dword ptr [gNet_mode]
            mov edx, dword ptr [gCurrent_net_game]
            mov edi, dword ptr [ebp]
            mov dword ptr [esp + 0x18], edx
            test eax, eax
            je L0049f00a
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L0049f00a
            mov ecx, offset gNet_players+0x24
    L0049efdc:
            cmp dword ptr [ecx], edi
            je L0049efed
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L0049efdc
            jmp L0049f00a
    L0049efed:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            mov ecx, dword ptr [esp + 0x18]
            push edx
            mov edx, esi
            call PDNetSendMessageToAddress
    L0049f00a:
            xor esi, esi
            mov dword ptr [ebp + 0xa8], esi
    L0049f012:
            test esi, esi
            jne L0049f034
    L0049f016:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [ebp + 0xa8], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L0049f034:
            mov ax, word ptr [esi + 0x16]
            mov edi, esi
            mov ecx, eax
            and ecx, 0xffff
            lea edx, [ebx + eax]
            add edi, ecx
            inc word ptr [esi + 0x14]
            test edi, edi
            mov word ptr [esi + 0x16], dx
            jne L0049f091
            mov edx, dword ptr [esp + 0x14]
    L0049f057:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNumber_of_net_players]
            inc eax
            add ebp, 0xd4
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L0049ef45
    L0049f074:
            mov ecx, 0xff
            cmp word ptr [edx + 0x14], cx
            je L0049f082
            mov byte ptr [edx + 0x19], cl
    L0049f082:
            mov eax, 0xfffffffd
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 4
    L0049f091:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, ebx
            mov edx, ecx
            lea esi, [eax + 0x18]
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov ecx, 0xff
            cmp word ptr [eax + 0x14], cx
            je L0049f0b6
            mov byte ptr [eax + 0x19], cl
    L0049f0b6:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0xc
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

// FUNCTION: CARMA2_HW 0x0049f0d0
int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (gNet_mode != eNet_mode_none) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();
        return PDNetSendMessageToAddress(pDetails, pMessage, &pDetails->pd_net_info);
    } else {
        return -3;
    }
}

// FUNCTION: CARMA2_HW 0x0049f2c0
int C2_HOOK_FASTCALL NetSendMessageToAllPlayers(tNet_game_details* pNet_game, tNet_message* pMessage) {
    int i;
    int size;
    void* contents;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    size = pMessage->header.field_0x16 - sizeof(tNet_message_header);
    pMessage->contents.raw.header.size = size;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], size);
            if (contents != NULL) {
                memcpy(contents, &pMessage->contents, size);
            }
        }
    }
    NetDisposeMessage(pNet_game, pMessage);
    return 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0049f440
int C2_NAKED C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, int pOption) {
#ifndef CARPOCALYPSE2_MATCHING
    unsigned int size;
    char buf[256];

    switch (pType) {
    case 0x01:
        size = 0x7c;
        break;
    case 0x02:
        size = 0xd8;
        break;
    case 0x03:
        size = 0xe4;
        break;
    case 0x0c:
        size = 0xf6;
        break;
    case 0x05:
        size = pOption + 4;
        break;
    case 0x0d:
        size = 0x82;
        break;
    case 0x1a:
        switch (pOption) {
        case 0:
            size = 0x18;
            break;
        case 1:
            size = 0x24;
            break;
        case 2:
            size = 0x3c;
            break;
        default:
            size = *(int*)&pType;
            break;
        }
        break;
    case 0x1c:
        size = 0x40;
        break;
    case 0x1d:
        size = 0x68;
        break;
    case 0x00:
    case 0x06:
    case 0x08:
    case 0x09:
    case 0x0e:
        size = 2;
        break;
    case 0x07:
        size = 0xf8;
        break;
    case 0x1e:
        size = 0x94;
        break;
    case 0x18:
    case 0x21:
        size = 0x38;
        break;
    case 0x22:
        size = 0x64;
        break;
    case 0x33:
        size = 0x1c;
        break;
    case 0x20:
    case 0x24:
    case 0x25:
        size = 0x30;
        break;
    case 0x0f:
    case 0x26:
        size = 0x10;
        break;
    case 0x28:
        size = 0x3c;
        break;
    case 0x17:
    case 0x19:
    case 0x23:
    case 0x2b:
        size = 0xc;
        break;
    case 0x16:
    case 0x1f:
    case 0x2d:
        size = 0x14;
        break;
    case 0x2e:
        size = pOption + 0xc;
        break;
    case 0x0a:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x2c:
    case 0x2f:
        size = 8;
        break;
    case 0x10:
    case 0x11:
    case 0x27:
    case 0x29:
    case 0x2a:
    case 0x31:
        size = pOption;
        break;
    case 0x32:
        size = 0x20;
        break;
    default:
        size = 4;
        break;
    }
    size += size & 1;
    if (size > 0xff) {
        sprintf(buf, " type %d", pType);
        FatalError(0x72, buf);
    }
    return size;
#else
    __asm {
            sub esp, 0x104
            mov byte ptr [esp], cl
            push esi
            mov eax, dword ptr [esp + 4]
            and eax, 0xff
            cmp eax, 0x33
            ja L0049f53a
            jmp dword ptr [eax*4 + 0x49f580]
            mov esi, 0x7c
            jmp L0049f545
            mov esi, 0xd8
            jmp L0049f545
            mov esi, 0xe4
            jmp L0049f545
            mov esi, 0xf6
            jmp L0049f545
            lea esi, [edx + 4]
            jmp L0049f545
            mov esi, 0x82
            jmp L0049f545
            sub edx, 0
            je L0049f4c1
            dec edx
            je L0049f4b7
            dec edx
            jne L0049f541
            mov esi, 0x3c
            jmp L0049f545
    L0049f4b7:
            mov esi, 0x24
            jmp L0049f545
    L0049f4c1:
            mov esi, 0x18
            jmp L0049f545
            mov esi, 0x40
            jmp L0049f545
            mov esi, 0x68
            jmp L0049f545
            mov esi, 2
            jmp L0049f545
            mov esi, 0xf8
            jmp L0049f545
            mov esi, 0x94
            jmp L0049f545
            mov esi, 0x38
            jmp L0049f545
            mov esi, 0x64
            jmp L0049f545
            mov esi, 0x1c
            jmp L0049f545
            mov esi, 0x30
            jmp L0049f545
            mov esi, 0x10
            jmp L0049f545
            mov esi, 0x3c
            jmp L0049f545
            mov esi, 0xc
            jmp L0049f545
            mov esi, 0x14
            jmp L0049f545
            lea esi, [edx + 0xc]
            jmp L0049f545
            mov esi, 8
            jmp L0049f545
            mov esi, edx
            jmp L0049f545
            mov esi, 0x20
            jmp L0049f545
    L0049f53a:
            mov esi, 4
            jmp L0049f545
    L0049f541:
            mov esi, dword ptr [esp + 4]
    L0049f545:
            mov ecx, esi
            and ecx, 1
            add esi, ecx
            cmp esi, 0xff
            jbe L0049f576
            push eax
            lea edx, [esp + 0xc]
            push offset gAsmString_0065a2bc
            push edx
            call sprintf
            add esp, 0xc
            lea eax, [esp + 8]
            push eax
            push 0x72
            call FatalError
            add esp, 8
    L0049f576:
            mov eax, esi
            pop esi
            add esp, 0x104
            ret
            _emit 0xd6
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x64
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x6e
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x78
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x3a
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x8c
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xd6
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xdd
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xd6
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xd6
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x3a
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x82
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x94
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xd6
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x07
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x2f
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x2f
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x1c
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x15
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0xeb
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x15
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x9e
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xc1
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xc8
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xcf
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xe4
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x1c
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x00
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0xeb
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0xf2
            _emit 0xf4
            _emit 0x49
            _emit 0x00
            _emit 0x15
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x00
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x00
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x07
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x2f
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x0e
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x2f
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x2f
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x15
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x1c
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x23
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x28
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x3a
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x2f
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0x33
            _emit 0xf5
            _emit 0x49
            _emit 0x00
            _emit 0xf9
            _emit 0xf4
            _emit 0x49
            _emit 0x00
    }
#endif
}
#pragma auto_inline(on)

// NetGetMessageSize

// FUNCTION: CARMA2_HW 0x0049f650
tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message* message;

    message = NetAllocateMessage(sizeof(tNet_message_header) + 1);
    message->contents.raw.header.type = pArg1;
    message->header.field_0x14 = 0;
    message->header.field_0x16 = sizeof(tNet_message_header) + 1;
    return message;
#else
    int size;
    tNet_message* message;

    size = NetGetContentsSize(pArg1, pArg2) + sizeof(tNet_message_header);
    message = NetAllocateMessage(size);
    if (message != NULL) {
        message->header.field_0x14 = 1;
        message->header.field_0x16 = (tU16)size;
        message->contents.raw.header.type = pArg1;
    }
    return message;
#endif
}

// NetBuildGuaranteedMessage

// NetGetToPlayerContentsSize

// FUNCTION: CARMA2_HW 0x0049f6f0
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tNet_message_type pType, tU32 pSize_decider, tPlayer_ID pPlayer) {
    tNet_message_chunk* contents;
    tU32 content_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);
    C2_HOOK_BUG_ON(sizeof(tMax_message) - sizeof(void*) != 0x200);

    content_size = NetGetContentsSize(pType, pSize_decider);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], content_size);
            contents->raw.header.type = pType;
            contents->raw.header.size = (tU8)content_size;
            return contents;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049f860
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tNet_message_type pType, tU32 pSize_decider) {

    return NetGetToPlayerContents(pType, pSize_decider, gNet_players[0].ID);
}

// NetStartBroadcastContents

// NetBroadcastContents

// FUNCTION: CARMA2_HW 0x0049fcf0
void C2_HOOK_FASTCALL NetSendMessageStacks(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && gNet_players[i].field_0xcc != NULL) {
            NetReallySendMessageToPlayer(gCurrent_net_game, gNet_players[i].field_0xcc, gNet_players[i].ID);
            gNet_players[i].field_0xcc = NULL;
        }
    }
#else
    int i;

    gLast_flush_message = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].field_0xcc != NULL) {
            NetReallySendMessageToPlayer(gCurrent_net_game, gNet_players[i].field_0xcc, gNet_players[i].ID);
            gNet_players[i].field_0xcc = NULL;
        }
    }
#endif
}

// GLOBAL: CARMA2_HW 0x00690c60
static int rr_min;

// GLOBAL: CARMA2_HW 0x00690c64
static int rr_mid;

// GLOBAL: CARMA2_HW 0x00690c68
static int rr_max;

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0049fdb0
tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize) {
    void* pointer;
    tNet_message_memory* new_message;
    tNet_message_memory* last_message;
    tNet_message* message;
    tU8* test;
    int i;

    C2_HOOK_BUG_ON(sizeof(tMin_message) - sizeof(void*) != 0x58);
    C2_HOOK_BUG_ON(sizeof(tMid_message) - sizeof(void*) != 0x80);
    C2_HOOK_BUG_ON(sizeof(tMax_message) - sizeof(void*) != 0x200);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_memory, message, 0x4);

    pointer = NULL;
    if (pSize <= sizeof(tMin_message) - sizeof(void*)) {
        for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
            test = gMin_messages[rr_min].buffer;
            test += gMessage_header_size;
            if (((tNet_message*)test)->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMin_messages[rr_min];
                break;
            }
            rr_min++;
            if (rr_min >= MIN_MESSAGES_CAPACITY) {
                rr_min = 0;
            }
        }
    }
    if (pointer == NULL && pSize <= (int)(sizeof(tMid_message) - sizeof(void*))) {
        for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
            test = gMid_messages[rr_mid].buffer;
            test += gMessage_header_size;
            if (((tNet_message*)test)->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMid_messages[rr_mid];
                break;
            }
            rr_mid++;
            if (rr_mid >= MID_MESSAGES_CAPACITY) {
                rr_mid = 0;
            }
        }
    }
    if (pointer == NULL && pSize <= (int)(sizeof(tMax_message) - sizeof(void*))) {
        for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
            test = gMax_messages[rr_max].buffer;
            test += gMessage_header_size;
            if (((tNet_message*)test)->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMax_messages[rr_max];
                break;
            }
            rr_max++;
            if (rr_max >= MAX_MESSAGES_CAPACITY) {
                rr_max = 0;
            }
        }
    }
    if (pointer == NULL) {
        pointer = new_message = BrMemAllocate(pSize + offsetof(tNet_message_memory, message) + gMessage_header_size, kMem_dynamic_message);
        if (new_message != NULL) {
            new_message->next = NULL;
            if (gDynamic_messages != NULL) {
                for (last_message = gDynamic_messages; last_message->next != NULL; last_message = last_message->next) {
                }
                last_message->next = new_message;
            } else {
                gDynamic_messages = new_message;
            }
            pointer = &new_message->message;
        }
    }
    if (pointer != NULL) {
        message = (tNet_message*)((tU8*)pointer + gMessage_header_size);
        message->header.field_0xc = 1;
        message->header.field_0x4 = MESSAGE_MAGIC_NUMBER;
        return message;
    } else {
        return NULL;
    }
}
#pragma auto_inline(on)

void C2_HOOK_FASTCALL NetFreeExcessMemory(void) {

    while (gDynamic_messages != NULL && *(&gDynamic_messages->message.contents.raw.header.type + gMessage_header_size) == eNetMsg_none) {
        tNet_message_memory* next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

void C2_HOOK_FASTCALL DisposeExcessMemory(void) {
    tNet_message_memory* next;

    while (gDynamic_messages != NULL) {
        next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

// FUNCTION: CARMA2_HW 0x0049ff50
int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {
    if (pMessage->header.field_0x14 == 0xff) {
        return -1;
    }
    pMessage->contents.raw.header.type = eNetMsg_none;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004a4660
void C2_NAKED C2_HOOK_FASTCALL RemoveNetPlayer(register tPlayer_ID pPlayer_id) {
#ifndef CARPOCALYPSE2_MATCHING
    int player_count;
    int player_index;
    int i;
    int move_count;
    int message_size;
    int contents_size;
    int guarantee_size;
    int trailing_contents_size;
    tNet_game_player_info* volatile player_list;
    tNet_game_player_info* player;
    tNet_message* message;
    tNet_game_player_info* message_player;
    tNet_game_details* volatile current_game;
    tU8* message_bytes;
    tU32* shift_destination;

    player_count = gNumber_of_net_players;
    player_list = BrMemAllocate(player_count * sizeof(tNet_game_player_info), kMem_player_list_leave);
    memcpy(player_list, gNet_players, player_count * sizeof(tNet_game_player_info));
    player = player_list;
    player_index = 0;
    if (player_count > 0) {
        do {
            if (pPlayer_id == player->ID) {
                if (player->car_index >= 0) {
                    gCar_details[player->car_index].ownership = 0;
                }
                if (player->next_car_index >= 0
                        && (gCurrent_net_game->options.car_choice == eNet_car_all
                            || gCurrent_net_game->options.car_choice == eNet_car_novelty)) {
                    gCar_details[player->next_car_index].ownership = 0;
                }
                player_count--;
                if (player_index < player_count) {
                    move_count = player_count - player_index;
                    shift_destination = (tU32*)player;
                    do {
                        move_count--;
                        memcpy(shift_destination, shift_destination + 0x35, sizeof(tNet_game_player_info));
                        shift_destination += 0x35;
                    } while (move_count != 0);
                }
                break;
            }
            player++;
            player_index++;
        } while (player_index < player_count);
    }
    NetPlayersChanged(player_count, player_list);
    BrMemFree(player_list);
    i = 0;
    gCurrent_net_game->num_players = gNumber_of_net_players;
    if (gNumber_of_net_players > 0) {
        player = gNet_players;
        do {
            contents_size = NetGetContentsSize(3, 1);
            guarantee_size = NetGetContentsSize(5, 0);
            trailing_contents_size = NetGetContentsSize(3, 1);
            message_size = trailing_contents_size + 0x18 + guarantee_size;
            message = NetAllocateMessage(message_size);
            if (message != NULL) {
                message_bytes = (tU8*)message;
                message->contents.raw.header.size = (tU8)contents_size;
                message->header.field_0x14 = 0xff;
                message->header.field_0x16 = (tU16)message_size;
                message->contents.raw.header.type = 3;
                message_bytes[0x19 + (tU8)contents_size] = 5;
                message_bytes[0x18 + message->contents.raw.header.size] = (tU8)guarantee_size;
            }
            message_bytes = (tU8*)message;
            *(int*)(message_bytes + 0x1c) = gNumber_of_net_players;
            *(int*)(message_bytes + 0x20) = i;
            *(int*)(message_bytes + 0x24) = gUINT_00690c40;
            message_player = (tNet_game_player_info*)(message_bytes + 0x28);
            memcpy(message_player, player, sizeof(tNet_game_player_info));
            message_player->field_0xcc = NULL;
            current_game = gCurrent_net_game;
            if (gNumber_of_net_players == 1) {
                message->contents.raw.header.type = 0xff;
            } else {
                int recipient;

                for (recipient = 0; recipient < gNumber_of_net_players; recipient++) {
                    if (recipient != gThis_net_player_index) {
                        NetGuaranteedSendMessageToPlayer(current_game, message, gNet_players[recipient].ID, 0);
                    }
                }
            }
            i++;
            player++;
        } while (i < gNumber_of_net_players);
    }
    gUINT_00690c40++;
    if (gInitialised_grid != 0) {
        SetUpNetCarPositions();
        if (gINT_0074b780 != 0) {
            gSynch_race_start = 1;
            player = &gNet_players[1];
            for (i = 1; i < gNumber_of_net_players; i++, player++) {
                player->awaiting_confirmation = 1;
            }
        }
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            mov ebx, dword ptr [gNumber_of_net_players]
            push ebp
            push esi
            lea eax, [ebx + ebx*2]
            push edi
            push 0xb9
            mov ebp, ecx
            lea esi, [eax + eax*8]
            shl esi, 1
            sub esi, ebx
            shl esi, 2
            push esi
            call BrMemAllocate
            mov ecx, esi
            mov edx, eax
            mov eax, ecx
            mov esi, offset gNet_players
            mov edi, edx
            add esp, 8
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            mov dword ptr [esp + 0x14], edx
            test ebx, ebx
            rep movsb
            jle L004a4734
            lea ecx, [edx + 0x24]
    L004a46b4:
            cmp ebp, dword ptr [ecx]
            je L004a46c5
            inc eax
            add ecx, 0xd4
            cmp eax, ebx
            jl L004a46b4
            jmp L004a4734
    L004a46c5:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edi, [edx + ecx*4]
            mov ecx, dword ptr [edx + ecx*4 + 0x4c]
            test ecx, ecx
            jl L004a46e8
            lea ecx, [ecx + ecx*4]
            mov dword ptr [ecx*4 + gCar_details], 0
    L004a46e8:
            mov ecx, dword ptr [edi + 0x8c]
            test ecx, ecx
            jl L004a4713
            mov esi, dword ptr [gCurrent_net_game]
            mov esi, dword ptr [esi + 0x70]
            cmp esi, 2
            je L004a4705
            cmp esi, 1
            jne L004a4713
    L004a4705:
            lea ecx, [ecx + ecx*4]
            mov dword ptr [ecx*4 + gCar_details], 0
    L004a4713:
            dec ebx
            cmp eax, ebx
            jge L004a4734
            mov edx, ebx
            sub edx, eax
    L004a471c:
            lea eax, [edi + 0xd4]
            mov ecx, 0x35
            mov esi, eax
            dec edx
            rep movsd
            mov edi, eax
            jne L004a471c
            mov edx, dword ptr [esp + 0x14]
    L004a4734:
            mov ecx, ebx
            call NetPlayersChanged
            mov edx, dword ptr [esp + 0x14]
            push edx
            call BrMemFree
            mov ecx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [gNumber_of_net_players]
            add esp, 4
            mov dword ptr [esp + 0x10], 0
            mov dword ptr [ecx + 0x34], eax
            mov eax, dword ptr [gNumber_of_net_players]
            test eax, eax
            jle L004a487a
            mov dword ptr [esp + 0x14], offset gNet_players
    L004a4773:
            mov edx, 1
            mov cl, 3
            call NetGetContentsSize
            xor edx, edx
            mov cl, 5
            mov ebx, eax
            call NetGetContentsSize
            mov edx, 1
            mov cl, 3
            mov dword ptr [esp + 0x18], eax
            call NetGetContentsSize
            mov edx, dword ptr [esp + 0x18]
            lea esi, [eax + edx + 0x18]
            mov ecx, esi
            call NetAllocateMessage
            mov ebp, eax
            test ebp, ebp
            je L004a47d8
            mov cl, byte ptr [esp + 0x18]
            mov byte ptr [ebp + 0x18], bl
            and ebx, 0xff
            mov word ptr [ebp + 0x14], 0xff
            mov word ptr [ebp + 0x16], si
            mov byte ptr [ebp + 0x19], 3
            xor eax, eax
            mov byte ptr [ebp + ebx + 0x19], 5
            mov al, byte ptr [ebp + 0x18]
            mov byte ptr [ebp + eax + 0x18], cl
    L004a47d8:
            mov edx, dword ptr [gNumber_of_net_players]
            mov eax, dword ptr [esp + 0x10]
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [ebp + 0x1c], edx
            mov dword ptr [ebp + 0x20], eax
            mov ecx, dword ptr [gUINT_00690c40]
            mov dword ptr [ebp + 0x24], ecx
            lea edi, [ebp + 0x28]
            mov ecx, 0x35
            xor ebx, ebx
            rep movsd
            xor edi, edi
            mov dword ptr [ebp + 0xf4], edi
            mov eax, dword ptr [gNumber_of_net_players]
            mov edx, dword ptr [gCurrent_net_game]
            cmp eax, 1
            mov dword ptr [esp + 0x18], edx
            jne L004a4828
            mov byte ptr [ebp + 0x19], 0xff
            mov eax, dword ptr [gNumber_of_net_players]
            jmp L004a485b
    L004a4828:
            cmp eax, edi
            jle L004a485b
            mov esi, offset gNet_players+0x24
    L004a4831:
            cmp edi, dword ptr [gThis_net_player_index]
            je L004a4850
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [esp + 0x18]
            push 0
            push eax
            mov edx, ebp
            call NetGuaranteedSendMessageToPlayer
            or ebx, eax
            mov eax, dword ptr [gNumber_of_net_players]
    L004a4850:
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L004a4831
    L004a485b:
            mov ecx, dword ptr [esp + 0x10]
            mov esi, dword ptr [esp + 0x14]
            inc ecx
            add esi, 0xd4
            cmp ecx, eax
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x14], esi
            jl L004a4773
    L004a487a:
            mov ecx, dword ptr [gUINT_00690c40]
            mov eax, dword ptr [gInitialised_grid]
            inc ecx
            test eax, eax
            mov dword ptr [gUINT_00690c40], ecx
            je L004a48c7
            call SetUpNetCarPositions
            mov eax, dword ptr [gINT_0074b780]
            test eax, eax
            je L004a48c7
            mov ecx, dword ptr [gNumber_of_net_players]
            mov dword ptr [gSynch_race_start], 1
            cmp ecx, 1
            jle L004a48c7
            mov eax, offset gNet_players+0x134
            dec ecx
    L004a48b9:
            mov dword ptr [eax], 1
            add eax, 0xd4
            dec ecx
            jne L004a48b9
    L004a48c7:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
            nop
    }
#endif
}

// NetReallyDisposeMessage

// NetGetNextMessage

// ReceivedSendMeDetails

// ReceivedDetails

// SendOutPlayerList

// ReceivedJoin

// KickPlayerOut

// ReceivedLeave

// NetFullScreenMessage

// HostHasBittenTheDust

// ReceivedHosticide

// ReceivedNewPlayerList

// ReceivedRaceOver

// ReceivedStatusReport

// ReceivedGroupStatus

// ReceivedRepositionCar

// ReceivedStartRace

// ReceivedGuaranteeReply

// ReceivedHeadup

// ReceivedHostQuery

// ReceivedHostReply

// ReceivedGuaranteedMessage

// PlayerIsInList

// ReceivedTimeSync

// ReceivedConfirm

// ReceivedDisableCar

// ReceivedEnableCar

// ReceivedActivateLift

// ReceivedScores

// ReceivedWasted

// ReceivedCarDetailsReq

// ReceivedCarDetails

// ReceivedGameScores

// ProcessGuaranteeStuff

// ReceivedMessage

// FUNCTION: CARMA2_HW 0x004a4a40
void C2_NAKED C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void) {
#ifndef CARPOCALYPSE2_MATCHING
#pragma inline_depth(0)
    int old_net_service;

    if (gNet_mode != eNet_mode_none || gJoin_list_mode) {
        old_net_service = gIn_net_service;
        gIn_net_service = 1;
        for (;;) {
            tNet_message* message;
            void* sender_address;

            message = NetGetNextMessage(gCurrent_net_game, &sender_address);
            if (message == NULL) {
                break;
            }
            if (message->header.field_0x14 != 0) {
                tU32 receive_time;

                receive_time = GetRaceTime();
                if (message->header.field_0x4 == 0x763a5059) {
                    ReceivedMessage(message, sender_address, receive_time);
                }
                ((tU8*)message)[0x19] = 0xff;
            }
        }
        gIn_net_service = old_net_service;
    }
#pragma inline_depth()
#else
    __asm {
            push ecx
            mov eax, dword ptr [gNet_mode]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            jne L004a4a57
            mov eax, dword ptr [gJoin_list_mode]
            test eax, eax
            je L004a4aad
    L004a4a57:
            mov ebp, dword ptr [gIn_net_service]
            mov dword ptr [gIn_net_service], 1
            mov edi, 0x763a5059
            mov bl, 0xff
    L004a4a6e:
            mov ecx, dword ptr [gCurrent_net_game]
            lea edx, [esp + 0x10]
            call NetGetNextMessage
            mov esi, eax
            test esi, esi
            je L004a4aa7
            cmp word ptr [esi + 0x14], 0
            je L004a4aa3
            call GetRaceTime
            cmp dword ptr [esi + 4], edi
            jne L004a4aa0
            mov edx, dword ptr [esp + 0x10]
            push eax
            mov ecx, esi
            call ReceivedMessage
    L004a4aa0:
            mov byte ptr [esi + 0x19], bl
    L004a4aa3:
            test esi, esi
            jne L004a4a6e
    L004a4aa7:
            mov dword ptr [gIn_net_service], ebp
    L004a4aad:
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
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a4ac0
void C2_NAKED C2_HOOK_FASTCALL BroadcastStatus(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message* message;
    tNet_game_player_info* player;
    int i;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    player = &gNet_players[gThis_net_player_index];
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i == gThis_net_player_index) {
            continue;
        }
        message = NetBuildMessage(NET_MSG_STATUS, 0);
        message->contents.raw.data[0] = (tU8)player->player_status;
        NetReallySendMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID);
    }
#else
    tNet_message* message;
    tNet_game_player_info* player;
    int i;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    player = &gNet_players[gThis_net_player_index];
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i == gThis_net_player_index) {
            continue;
        }
        message = NetBuildMessage(NET_MSG_STATUS, 0);
        message->contents.raw.data[0] = (tU8)player->player_status;
        NetReallySendMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID);
    }
#endif
#else
    __asm {
            mov eax, dword ptr [gNet_mode]
            sub esp, 0xc
            sub eax, 2
            push ebx
            push ebp
            push esi
            push edi
            je L004a4b4e
            dec eax
            jne L004a4cdc
            xor edx, edx
            mov cl, 0xb
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L004a4b01
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 0xb
    L004a4b01:
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov dx, word ptr [ecx*4 + gNet_players+0x48]
            mov word ptr [esi + 0x1a], dx
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            je L004a4cdc
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov edx, esi
            mov ecx, edi
            push edi
            mov dword ptr [esi + 0x10], eax
            call PDNetSendMessageToAddress
            jmp L004a4cdc
    L004a4b4e:
            mov edx, dword ptr [gNumber_of_net_players]
            mov cl, 0x2e
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            mov dword ptr [esp + 0x18], esi
            je L004a4b7f
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 0x2e
    L004a4b7f:
            mov dword ptr [esi + 0x1c], 0
            mov cl, byte ptr [gNumber_of_net_players]
            mov byte ptr [esi + 0x20], cl
            mov ecx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test ecx, ecx
            jle L004a4bc2
            mov ecx, offset gNet_players+0x48
    L004a4ba0:
            mov edx, dword ptr [ecx - 0x24]
            mov ebp, dword ptr [esi + 0x1c]
            add ebp, edx
            inc eax
            mov dword ptr [esi + 0x1c], ebp
            mov dl, byte ptr [ecx]
            mov byte ptr [eax + esi + 0x20], dl
            mov edx, dword ptr [gNumber_of_net_players]
            add ecx, 0xd4
            cmp eax, edx
            jl L004a4ba0
    L004a4bc2:
            xor ebx, ebx
            lea eax, [esi + 0x18]
            mov bx, word ptr [esi + 0x16]
            mov dword ptr [esp + 0x14], eax
            sub ebx, 0x18
            xor ecx, ecx
            mov byte ptr [eax], bl
            mov eax, dword ptr [gNumber_of_net_players]
            test eax, eax
            mov dword ptr [esp + 0x10], ecx
            jle L004a4cce
            mov eax, dword ptr [gThis_net_player_index]
            mov ebp, offset gNet_players+0xcc
    L004a4bf1:
            cmp ecx, eax
            je L004a4cb1
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea eax, [ebp - 0xcc]
            lea edx, [ecx*4 + gNet_players]
            cmp eax, edx
            jne L004a4c1e
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L004a4c1e:
            mov edx, dword ptr [ebp]
            test edx, edx
            je L004a4c54
            xor ecx, ecx
            mov cx, word ptr [edx + 0x16]
            add ecx, ebx
            cmp ecx, 0x200
            jle L004a4c50
            mov eax, dword ptr [ebp - 0xa8]
            mov ecx, dword ptr [gCurrent_net_game]
            push eax
            call NetReallySendMessageToPlayer
            mov dword ptr [ebp], 0
            xor edx, edx
    L004a4c50:
            test edx, edx
            jne L004a4c6f
    L004a4c54:
            mov ecx, 0x200
            call NetAllocateMessage
            mov edx, eax
            mov dword ptr [ebp], eax
            mov word ptr [edx + 0x16], 0x18
            mov word ptr [edx + 0x14], 0
    L004a4c6f:
            mov ax, word ptr [edx + 0x16]
            mov edi, edx
            mov ecx, eax
            add eax, ebx
            and ecx, 0xffff
            mov word ptr [edx + 0x16], ax
            add edi, ecx
            inc word ptr [edx + 0x14]
            test edi, edi
            je L004a4cac
            mov esi, dword ptr [esp + 0x14]
            mov ecx, ebx
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gThis_net_player_index]
            mov esi, dword ptr [esp + 0x18]
            jmp L004a4cb1
    L004a4cac:
            mov eax, dword ptr [gThis_net_player_index]
    L004a4cb1:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [gNumber_of_net_players]
            inc ecx
            add ebp, 0xd4
            cmp ecx, edx
            mov dword ptr [esp + 0x10], ecx
            jl L004a4bf1
    L004a4cce:
            mov eax, 0xff
            cmp word ptr [esi + 0x14], ax
            je L004a4cdc
            mov byte ptr [esi + 0x19], al
    L004a4cdc:
            call PDGetTotalTime
            mov edx, dword ptr [gNumber_of_net_players]
            mov dword ptr [gLast_flush_message], eax
            test edx, edx
            mov dword ptr [esp + 0x10], 0
            jle L004a4d8e
            mov ebp, offset gNet_players+0xcc
    L004a4d01:
            mov esi, dword ptr [ebp]
            test esi, esi
            je L004a4d77
            mov eax, dword ptr [gNet_mode]
            mov ebx, dword ptr [ebp - 0xa8]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            je L004a4d70
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a4d70
            mov ecx, offset gNet_players+0x24
    L004a4d3e:
            cmp dword ptr [ecx], ebx
            je L004a4d4f
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a4d3e
            jmp L004a4d70
    L004a4d4f:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            mov ecx, edi
            push edx
            mov edx, esi
            call PDNetSendMessageToAddress
            mov edx, dword ptr [gNumber_of_net_players]
    L004a4d70:
            mov dword ptr [ebp], 0
    L004a4d77:
            mov eax, dword ptr [esp + 0x10]
            add ebp, 0xd4
            inc eax
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L004a4d01
    L004a4d8e:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a4da0
void C2_NAKED C2_HOOK_FASTCALL CheckForDisappearees(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    tU32 now;
    int i;

    now = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && now - gNet_players[i].last_heard_from_him > 60000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
#else
    tU32 now;
    int i;

    now = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && now - gNet_players[i].last_heard_from_him > 60000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
#endif
#else
    __asm {
            sub esp, 0x110
            push ebx
            push ebp
            push esi
            push edi
            call PDGetTotalTime
            mov ecx, eax
            mov eax, dword ptr [gNet_mode]
            mov esi, 2
            mov dword ptr [esp + 0x1c], ecx
            cmp eax, esi
            jne L004a5063
            mov eax, dword ptr [gNumber_of_net_players]
            mov dword ptr [esp + 0x14], 0
            test eax, eax
            jle L004a50a4
            mov ebx, offset gNet_players+0x14
            mov dword ptr [esp + 0x10], ebx
            jmp L004a4deb
    L004a4de7:
            mov ecx, dword ptr [esp + 0x1c]
    L004a4deb:
            mov eax, dword ptr [ebx + 0xc]
            test eax, eax
            jne L004a5037
            mov eax, dword ptr [ebx]
            test eax, eax
            je L004a5037
            sub ecx, eax
            cmp ecx, 0xea60
            jl L004a5037
            lea edi, [ebx + 0x14]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x20]
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
            mov ecx, 0x5a
            call GetMiscString
            mov edx, eax
            mov edi, 0x58f640
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea ebp, [esp + 0x20]
            mov esi, edi
            mov edi, ebp
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebp, [esp + 0x20]
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, ebp
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gNet_mode]
            mov ecx, edx
            and ecx, 3
            test eax, eax
            rep movsb
            je L004a500e
            mov eax, dword ptr [gINT_00659d34]
            test eax, eax
            jl L004a4eb5
            mov ecx, offset gAsmString_0065a170
            call PDFatalError
    L004a4eb5:
            mov ecx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test ecx, ecx
            jle L004a4ed4
            mov edx, dword ptr [gThis_net_player_index]
    L004a4ec7:
            cmp eax, edx
            jne L004a50af
            inc eax
            cmp eax, ecx
            jl L004a4ec7
    L004a4ed4:
            xor ebp, ebp
    L004a4ed6:
            test ebp, ebp
            je L004a500e
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 2]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            mov dword ptr [esp + 0x18], eax
            rep movsb
            mov ecx, dword ptr [gNumber_of_net_players]
            test ecx, ecx
            jle L004a5004
            mov ecx, dword ptr [gThis_net_player_index]
            mov ebx, offset gNet_players+0xcc
    L004a4f21:
            cmp eax, ecx
            je L004a4fe7
            cmp eax, dword ptr [gINT_00659d34]
            je L004a4fe7
            lea eax, [ecx + ecx*2]
            xor edx, edx
            mov dl, byte ptr [ebp]
            lea eax, [eax + eax*8]
            mov esi, edx
            shl eax, 1
            sub eax, ecx
            lea edx, [ebx - 0xcc]
            lea ecx, [eax*4 + gNet_players]
            cmp edx, ecx
            jne L004a4f61
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L004a4f61:
            mov edx, dword ptr [ebx]
            test edx, edx
            je L004a4f94
            xor eax, eax
            mov ax, word ptr [edx + 0x16]
            add eax, esi
            cmp eax, 0x200
            jle L004a4f90
            mov ecx, dword ptr [ebx - 0xa8]
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetReallySendMessageToPlayer
            mov dword ptr [ebx], 0
            xor edx, edx
    L004a4f90:
            test edx, edx
            jne L004a4fae
    L004a4f94:
            mov ecx, 0x200
            call NetAllocateMessage
            mov edx, eax
            mov dword ptr [ebx], eax
            mov word ptr [edx + 0x16], 0x18
            mov word ptr [edx + 0x14], 0
    L004a4fae:
            mov ax, word ptr [edx + 0x16]
            mov edi, edx
            mov ecx, eax
            add esi, eax
            and ecx, 0xffff
            mov word ptr [edx + 0x16], si
            add edi, ecx
            inc word ptr [edx + 0x14]
            xor ecx, ecx
            mov esi, ebp
            mov cl, byte ptr [ebp]
            mov eax, dword ptr [esp + 0x18]
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gThis_net_player_index]
    L004a4fe7:
            mov edx, dword ptr [gNumber_of_net_players]
            inc eax
            add ebx, 0xd4
            cmp eax, edx
            mov dword ptr [esp + 0x18], eax
            jl L004a4f21
            mov ebx, dword ptr [esp + 0x10]
    L004a5004:
            mov dword ptr [gINT_00659d34], 0xffffffff
    L004a500e:
            mov ecx, dword ptr [ebx + 0x10]
            call RemoveNetPlayer
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L004a5037
            lea eax, [esp + 0x20]
            xor edx, edx
            push eax
            push -4
            push 0xbb8
            mov ecx, 4
            call NewTextHeadupSlot
    L004a5037:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gNumber_of_net_players]
            inc eax
            add ebx, 0xd4
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], ebx
            jl L004a4de7
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x110
            ret
    L004a5063:
            mov eax, dword ptr [gINT_0068d968]
            test eax, eax
            jne L004a50a4
            mov eax, dword ptr [gNumber_of_net_players]
            test eax, eax
            je L004a50a4
            mov eax, dword ptr [gNet_players+0x14]
            test eax, eax
            je L004a50a4
            sub ecx, eax
            cmp ecx, 0xea60
            jl L004a50a4
            xor edx, edx
            mov ecx, 0x5b
            mov dword ptr [gINT_0068d968], 1
            call NetFullScreenMessage
            mov dword ptr [gProgram_state+0xa4], esi
    L004a50a4:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x110
            ret
    L004a50af:
            lea ecx, [eax + eax*2]
            xor edx, edx
            mov dword ptr [gINT_00659d34], eax
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov esi, dword ptr [ecx*4 + gNet_players+0x24]
            mov cl, 0xd
            call NetGetContentsSize
            mov edx, dword ptr [gNumber_of_net_players]
            mov ebx, eax
            xor eax, eax
            test edx, edx
            jle L004a5126
            mov ecx, offset gNet_players+0x24
    L004a50e1:
            cmp dword ptr [ecx], esi
            je L004a50fd
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a50e1
            mov ebx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ebp, eax
            jmp L004a4ed6
    L004a50fd:
            lea ecx, [eax + eax*2]
            mov edx, ebx
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea ecx, [ecx*4 + gNet_players]
            call NetGetToPlayerContentsSize
            mov byte ptr [eax], bl
            mov ebx, dword ptr [esp + 0x10]
            mov byte ptr [eax + 1], 0xd
            mov ebp, eax
            jmp L004a4ed6
    L004a5126:
            mov ebx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ebp, eax
            jmp L004a4ed6
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

// FUNCTION: CARMA2_HW 0x004a5240
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, awaiting_confirmation, 0x60);

    if (gSynch_race_start) {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].awaiting_confirmation) {
                return;
            }
        }
        SignalToStartRace();
    }
}

// FUNCTION: CARMA2_HW 0x004a5280
void C2_NAKED C2_HOOK_FASTCALL NetService(int pIn_race) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 time;

    if (gIn_net_service || gNet_service_disable) {
        return;
    }
    time = PDGetTotalTime();
    gIn_net_service = 1;
    if (gJoin_list_mode) {
        NetFreeExcessMemory();
        DoNextJoinPoll();
        NetReceiveAndProcessMessages();
    } else {
        if (gNet_mode != eNet_mode_none) {
            NetFreeExcessMemory();
            if (!pIn_race) {
                NetReceiveAndProcessMessages();
            }
            if (time - gLast_status_broadcast > 1000) {
                gLast_status_broadcast = PDGetTotalTime();
                BroadcastStatus();
            }
            CheckForDisappearees();
            if (gNet_mode == eNet_mode_host) {
                CheckForPendingStartRace();
                CheckForNeedyEnvironmentRecipients();
            }
        }
    }
    if (gJoin_list_mode || gNet_mode != eNet_mode_none) {
        ResendGuaranteedMessages();
        if (time > gLast_flush_message + 200u || !gProgram_state.racing) {
            NetSendMessageStacks();
        }
    }
    gIn_net_service = 0;
#else
    __asm {
            mov eax, dword ptr [gIn_net_service]
            sub esp, 8
            push ebp
            xor ebp, ebp
            push edi
            cmp eax, ebp
            mov edi, ecx
            jne L004a570a
            cmp dword ptr [gNet_service_disable], ebp
            jne L004a570a
            push esi
            push ebx
            call PDGetTotalTime
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [gJoin_list_mode]
            cmp eax, ebp
            mov dword ptr [gIn_net_service], 1
            je L004a54c8
            mov eax, dword ptr [gDynamic_messages]
            mov bl, 0xff
            cmp eax, ebp
            je L004a52f1
    L004a52cf:
            mov ecx, dword ptr [gMessage_header_size]
            cmp byte ptr [eax + ecx + 0x1d], bl
            jne L004a52f1
            mov esi, dword ptr [eax]
            push eax
            call BrMemFree
            add esp, 4
            mov eax, esi
            cmp esi, ebp
            mov dword ptr [gDynamic_messages], eax
            jne L004a52cf
    L004a52f1:
            cmp dword ptr [gTime_for_next_one], ebp
            je L004a53de
            push 0xb5
            push 0x78
            call BrMemAllocate
            add esp, 8
            cmp eax, ebp
            mov dword ptr [gCurrent_join_poll_game], eax
            je L004a5459
            mov edx, dword ptr [gJoin_poll_index]
            mov ecx, eax
            call PDNetGetNextJoinGame
            test eax, eax
            je L004a53c0
            mov ecx, dword ptr [gCurrent_join_poll_game]
            mov edx, offset gAsmString_0065a2b4
            call PDNetJoinGame
            test eax, eax
            jne L004a539d
            xor edx, edx
            xor cl, cl
            mov dword ptr [gTime_for_next_one], ebp
            call NetGetContentsSize
            mov esi, eax
            add esi, 0x18
            mov ecx, esi
            call NetAllocateMessage
            cmp eax, ebp
            je L004a536f
            mov word ptr [eax + 0x14], 1
            mov word ptr [eax + 0x16], si
            mov byte ptr [eax + 0x19], 0
    L004a536f:
            mov ecx, dword ptr [gCurrent_join_poll_game]
            mov edx, eax
            push ecx
            call NetSendMessageToAddress
            mov dword ptr [gBastard_has_answered], ebp
            call PDGetTotalTime
            mov dword ptr [gAsk_time], eax
            mov eax, dword ptr [gJoin_poll_index]
            inc eax
            mov dword ptr [gJoin_poll_index], eax
            jmp L004a5459
    L004a539d:
            mov eax, dword ptr [gCurrent_join_poll_game]
            cmp eax, ebp
            je L004a53b5
            push eax
            call BrMemFree
            add esp, 4
            mov dword ptr [gCurrent_join_poll_game], ebp
    L004a53b5:
            inc dword ptr [gJoin_poll_index]
            jmp L004a5459
    L004a53c0:
            mov eax, dword ptr [gCurrent_join_poll_game]
            mov dword ptr [gJoin_poll_index], ebp
            cmp eax, ebp
            je L004a5459
            push eax
            call BrMemFree
            add esp, 4
            jmp L004a5453
    L004a53de:
            cmp dword ptr [gBastard_has_answered], ebp
            je L004a5412
            mov ecx, dword ptr [gCurrent_join_poll_game]
            call dword ptr [gAdd_proc]
            mov ecx, dword ptr [gCurrent_join_poll_game]
            cmp ecx, ebp
            je L004a5449
            cmp dword ptr [gNet_mode], 2
            jne L004a5444
            mov ecx, dword ptr [gCurrent_net_game]
            call PDNetHostFinishGame
            jmp L004a5449
    L004a5412:
            call PDGetTotalTime
            sub eax, dword ptr [gAsk_time]
            cmp eax, 0x2710
            jbe L004a5459
            mov ecx, dword ptr [gCurrent_join_poll_game]
            cmp ecx, ebp
            je L004a5449
            cmp dword ptr [gNet_mode], 2
            jne L004a5444
            mov ecx, dword ptr [gCurrent_net_game]
            call PDNetHostFinishGame
            jmp L004a5449
    L004a5444:
            call PDNetLeaveGame
    L004a5449:
            mov dword ptr [gTime_for_next_one], 1
    L004a5453:
            mov dword ptr [gCurrent_join_poll_game], ebp
    L004a5459:
            cmp dword ptr [gNet_mode], ebp
            jne L004a546d
            cmp dword ptr [gJoin_list_mode], ebp
            je L004a5697
    L004a546d:
            mov edi, dword ptr [gIn_net_service]
            mov dword ptr [gIn_net_service], 1
            mov ebp, 0x763a5059
    L004a5482:
            mov ecx, dword ptr [gCurrent_net_game]
            lea edx, [esp + 0x14]
            call NetGetNextMessage
            mov esi, eax
            test esi, esi
            je L004a54bb
            cmp word ptr [esi + 0x14], 0
            je L004a54b7
            call GetRaceTime
            cmp dword ptr [esi + 4], ebp
            jne L004a54b4
            mov edx, dword ptr [esp + 0x14]
            push eax
            mov ecx, esi
            call ReceivedMessage
    L004a54b4:
            mov byte ptr [esi + 0x19], bl
    L004a54b7:
            test esi, esi
            jne L004a5482
    L004a54bb:
            mov dword ptr [gIn_net_service], edi
            xor ebp, ebp
            jmp L004a568f
    L004a54c8:
            cmp dword ptr [gNet_mode], ebp
            je L004a568f
            mov eax, dword ptr [gDynamic_messages]
            mov bl, 0xff
            cmp eax, ebp
            je L004a5501
    L004a54df:
            mov edx, dword ptr [gMessage_header_size]
            cmp byte ptr [eax + edx + 0x1d], bl
            jne L004a5501
            mov esi, dword ptr [eax]
            push eax
            call BrMemFree
            add esp, 4
            mov eax, esi
            cmp esi, ebp
            mov dword ptr [gDynamic_messages], eax
            jne L004a54df
    L004a5501:
            cmp edi, ebp
            jne L004a556b
            cmp dword ptr [gNet_mode], ebp
            jne L004a5515
            cmp dword ptr [gJoin_list_mode], ebp
            je L004a556b
    L004a5515:
            mov edi, dword ptr [gIn_net_service]
            mov dword ptr [gIn_net_service], 1
            mov ebp, 0x763a5059
    L004a552a:
            mov ecx, dword ptr [gCurrent_net_game]
            lea edx, [esp + 0x14]
            call NetGetNextMessage
            mov esi, eax
            test esi, esi
            je L004a5563
            cmp word ptr [esi + 0x14], 0
            je L004a555f
            call GetRaceTime
            cmp dword ptr [esi + 4], ebp
            jne L004a555c
            mov edx, dword ptr [esp + 0x14]
            push eax
            mov ecx, esi
            call ReceivedMessage
    L004a555c:
            mov byte ptr [esi + 0x19], bl
    L004a555f:
            test esi, esi
            jne L004a552a
    L004a5563:
            mov dword ptr [gIn_net_service], edi
            xor ebp, ebp
    L004a556b:
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [gLast_status_broadcast]
            sub eax, edx
            cmp eax, 0x3e8
            jbe L004a564c
            call PDGetTotalTime
            mov dword ptr [gLast_status_broadcast], eax
            mov eax, dword ptr [gNet_mode]
            sub eax, 2
            je L004a55ee
            dec eax
            jne L004a5647
            xor edx, edx
            mov cl, 0xb
            call NetBuildMessage
            mov esi, eax
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov dx, word ptr [ecx*4 + gNet_players+0x48]
            mov word ptr [esi + 0x1a], dx
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [gCurrent_net_game]
            cmp eax, ebp
            je L004a5647
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov edx, esi
            mov ecx, edi
            push edi
            mov dword ptr [esi + 0x10], eax
            call PDNetSendMessageToAddress
            jmp L004a5647
    L004a55ee:
            mov edx, dword ptr [gNumber_of_net_players]
            mov cl, 0x2e
            call NetBuildMessage
            mov dword ptr [eax + 0x1c], ebp
            mov cl, byte ptr [gNumber_of_net_players]
            mov byte ptr [eax + 0x20], cl
            mov edx, dword ptr [gNumber_of_net_players]
            xor ecx, ecx
            cmp edx, ebp
            jle L004a563a
            mov esi, offset gNet_players+0x48
    L004a5618:
            mov edx, dword ptr [esi - 0x24]
            mov edi, dword ptr [eax + 0x1c]
            add edi, edx
            inc ecx
            mov dword ptr [eax + 0x1c], edi
            mov dl, byte ptr [esi]
            mov byte ptr [ecx + eax + 0x20], dl
            mov edx, dword ptr [gNumber_of_net_players]
            add esi, 0xd4
            cmp ecx, edx
            jl L004a5618
    L004a563a:
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, eax
            call NetSendMessageToAllPlayers
    L004a5647:
            call NetSendMessageStacks
    L004a564c:
            call CheckForDisappearees
            cmp dword ptr [gNet_mode], 2
            jne L004a568f
            cmp dword ptr [gSynch_race_start], ebp
            je L004a568a
            mov edx, dword ptr [gNumber_of_net_players]
            mov eax, 1
            cmp edx, eax
            jle L004a5685
            mov ecx, offset gNet_players+0x134
    L004a5676:
            cmp dword ptr [ecx], ebp
            jne L004a568a
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a5676
    L004a5685:
            call SignalToStartRace
    L004a568a:
            call CheckForNeedyEnvironmentRecipients
    L004a568f:
            cmp dword ptr [gJoin_list_mode], ebp
            jne L004a569f
    L004a5697:
            cmp dword ptr [gNet_mode], ebp
            je L004a5702
    L004a569f:
            call ResendGuaranteedMessages
            mov eax, dword ptr [gLast_flush_message]
            mov ecx, dword ptr [esp + 0x10]
            add eax, 0xc8
            cmp ecx, eax
            ja L004a56be
            cmp dword ptr [gProgram_state+0x28], ebp
            jne L004a5702
    L004a56be:
            call PDGetTotalTime
            mov dword ptr [gLast_flush_message], eax
            mov eax, dword ptr [gNumber_of_net_players]
            xor edi, edi
            cmp eax, ebp
            jle L004a5702
            mov esi, offset gNet_players+0xcc
    L004a56d8:
            mov edx, dword ptr [esi]
            cmp edx, ebp
            je L004a56f2
            mov ecx, dword ptr [esi - 0xa8]
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetReallySendMessageToPlayer
            mov dword ptr [esi], ebp
    L004a56f2:
            mov eax, dword ptr [gNumber_of_net_players]
            inc edi
            add esi, 0xd4
            cmp edi, eax
            jl L004a56d8
    L004a5702:
            pop ebx
            mov dword ptr [gIn_net_service], ebp
            pop esi
    L004a570a:
            pop edi
            pop ebp
            add esp, 8
            ret
    }
#endif
}

// NetFinishRace

// FUNCTION: CARMA2_HW 0x004a57e0
void C2_NAKED C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (gNet_players[gThis_net_player_index].player_status == pNew_status) {
        return;
    }
    gNet_players[gThis_net_player_index].player_status = pNew_status;
    BroadcastStatus();
#else
    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (gNet_players[gThis_net_player_index].player_status == pNew_status) {
        return;
    }
    gNet_players[gThis_net_player_index].player_status = pNew_status;
    BroadcastStatus();
#endif
#else
    __asm {
            sub esp, 0x10
            mov edx, dword ptr [gNet_mode]
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            test edx, edx
            mov dword ptr [esp + 0x14], ebp
            je L004a5b27
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea eax, [ecx*4 + gNet_players+0x48]
            mov ecx, dword ptr [ecx*4 + gNet_players+0x48]
            cmp ecx, ebp
            je L004a5b27
            mov dword ptr [eax], ebp
            mov eax, edx
            sub eax, 2
            je L004a58a7
            dec eax
            jne L004a5a39
            xor edx, edx
            mov cl, 0xb
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            je L004a5859
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 0xb
    L004a5859:
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov ax, word ptr [edx*4 + gNet_players+0x48]
            mov word ptr [esi + 0x1a], ax
            mov eax, dword ptr [gNet_mode]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            je L004a5a39
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], ecx
            call PDGetTotalTime
            mov edx, esi
            mov ecx, edi
            push edi
            mov dword ptr [esi + 0x10], eax
            call PDNetSendMessageToAddress
            jmp L004a5a39
    L004a58a7:
            mov edx, dword ptr [gNumber_of_net_players]
            mov cl, 0x2e
            call NetGetContentsSize
            mov edi, eax
            add edi, 0x18
            mov ecx, edi
            call NetAllocateMessage
            mov esi, eax
            test esi, esi
            mov dword ptr [esp + 0x1c], esi
            je L004a58d8
            mov word ptr [esi + 0x14], 1
            mov word ptr [esi + 0x16], di
            mov byte ptr [esi + 0x19], 0x2e
    L004a58d8:
            mov dword ptr [esi + 0x1c], 0
            mov dl, byte ptr [gNumber_of_net_players]
            mov byte ptr [esi + 0x20], dl
            mov ecx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test ecx, ecx
            jle L004a591b
            mov ecx, offset gNet_players+0x48
    L004a58f9:
            mov edx, dword ptr [ecx - 0x24]
            mov edi, dword ptr [esi + 0x1c]
            add edi, edx
            inc eax
            mov dword ptr [esi + 0x1c], edi
            mov dl, byte ptr [ecx]
            mov byte ptr [eax + esi + 0x20], dl
            mov edx, dword ptr [gNumber_of_net_players]
            add ecx, 0xd4
            cmp eax, edx
            jl L004a58f9
    L004a591b:
            xor ebx, ebx
            lea eax, [esi + 0x18]
            mov bx, word ptr [esi + 0x16]
            mov dword ptr [esp + 0x18], eax
            sub ebx, 0x18
            xor ecx, ecx
            mov byte ptr [eax], bl
            mov eax, dword ptr [gNumber_of_net_players]
            test eax, eax
            mov dword ptr [esp + 0x10], ecx
            jle L004a5a2b
            mov eax, dword ptr [gThis_net_player_index]
            mov ebp, offset gNet_players+0xcc
    L004a594a:
            cmp ecx, eax
            je L004a5a0a
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea eax, [ebp - 0xcc]
            lea edx, [ecx*4 + gNet_players]
            cmp eax, edx
            jne L004a5977
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L004a5977:
            mov edx, dword ptr [ebp]
            test edx, edx
            je L004a59ad
            xor ecx, ecx
            mov cx, word ptr [edx + 0x16]
            add ecx, ebx
            cmp ecx, 0x200
            jle L004a59a9
            mov eax, dword ptr [ebp - 0xa8]
            mov ecx, dword ptr [gCurrent_net_game]
            push eax
            call NetReallySendMessageToPlayer
            mov dword ptr [ebp], 0
            xor edx, edx
    L004a59a9:
            test edx, edx
            jne L004a59c8
    L004a59ad:
            mov ecx, 0x200
            call NetAllocateMessage
            mov edx, eax
            mov dword ptr [ebp], eax
            mov word ptr [edx + 0x16], 0x18
            mov word ptr [edx + 0x14], 0
    L004a59c8:
            mov ax, word ptr [edx + 0x16]
            mov edi, edx
            mov ecx, eax
            add eax, ebx
            and ecx, 0xffff
            mov word ptr [edx + 0x16], ax
            add edi, ecx
            inc word ptr [edx + 0x14]
            test edi, edi
            je L004a5a05
            mov esi, dword ptr [esp + 0x18]
            mov ecx, ebx
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gThis_net_player_index]
            mov esi, dword ptr [esp + 0x1c]
            jmp L004a5a0a
    L004a5a05:
            mov eax, dword ptr [gThis_net_player_index]
    L004a5a0a:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [gNumber_of_net_players]
            inc ecx
            add ebp, 0xd4
            cmp ecx, edx
            mov dword ptr [esp + 0x10], ecx
            jl L004a594a
            mov ebp, dword ptr [esp + 0x14]
    L004a5a2b:
            mov eax, 0xff
            cmp word ptr [esi + 0x14], ax
            je L004a5a39
            mov byte ptr [esi + 0x19], al
    L004a5a39:
            call PDGetTotalTime
            mov edx, dword ptr [gNumber_of_net_players]
            mov dword ptr [gLast_flush_message], eax
            test edx, edx
            mov dword ptr [esp + 0x10], 0
            jle L004a5aef
            mov ebp, offset gNet_players+0xcc
    L004a5a5e:
            mov esi, dword ptr [ebp]
            test esi, esi
            je L004a5ad4
            mov eax, dword ptr [gNet_mode]
            mov ebx, dword ptr [ebp - 0xa8]
            mov edi, dword ptr [gCurrent_net_game]
            test eax, eax
            je L004a5acd
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], eax
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a5acd
            mov ecx, offset gNet_players+0x24
    L004a5a9b:
            cmp dword ptr [ecx], ebx
            je L004a5aac
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a5a9b
            jmp L004a5acd
    L004a5aac:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            mov ecx, edi
            push edx
            mov edx, esi
            call PDNetSendMessageToAddress
            mov edx, dword ptr [gNumber_of_net_players]
    L004a5acd:
            mov dword ptr [ebp], 0
    L004a5ad4:
            mov eax, dword ptr [esp + 0x10]
            add ebp, 0xd4
            inc eax
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L004a5a5e
            mov ebp, dword ptr [esp + 0x14]
    L004a5aef:
            mov eax, dword ptr [gProgram_state+0xb0]
            test eax, eax
            je L004a5b18
            cmp ebp, 6
            jl L004a5b14
            cmp ebp, 8
            je L004a5b14
            mov ecx, offset gProgram_state+0xac
            call EnableCar
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004a5b14:
            test eax, eax
            jne L004a5b27
    L004a5b18:
            cmp ebp, 6
            jge L004a5b27
            mov ecx, offset gProgram_state+0xac
            call DisableCar
    L004a5b27:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
            nop
    }
#endif
}

// NetGetPlayerStatus

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// NetGuaranteedSendMessageToHost

// AddToGuaranteeList
// FUNCTION: CARMA2_HW 0x004a5ca0
void C2_NAKED C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, tPlayer_ID pPlayer, void* pNotifyFail) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guaranteed message not built correctly");
    }
    if (gNext_guarantee >= 200) {
        for (i = 0; i < 10; i++) {
            NetReallyDisposeMessage(NULL, gGuarantee_list[i].message);
        }
        while (gGuarantee_list[i - 1].message == gGuarantee_list[i].message && i < 200) {
            i++;
        }
        memmove(gGuarantee_list, &gGuarantee_list[i], (200 - i) * sizeof(gGuarantee_list[0]));
        gNext_guarantee -= i;
    }
    if (gNext_guarantee < 200) {
        *(tU16*)&pMessage->contents.raw.data[pMessage->contents.raw.header.size] = gGuarantee_number;
        gGuarantee_list[gNext_guarantee].field_0x2c = gGuarantee_number;
        gGuarantee_number++;
        if (gGuarantee_number == 0) {
            gGuarantee_number = 1;
        }
        gGuarantee_list[gNext_guarantee].message = pMessage;
        gGuarantee_list[gNext_guarantee].send_time = PDGetTotalTime();
        gGuarantee_list[gNext_guarantee].next_resend_time = gGuarantee_list[gNext_guarantee].send_time + 100;
        gGuarantee_list[gNext_guarantee].resend_period = 100;
        if (pAddress != NULL) {
            memcpy(gGuarantee_list[gNext_guarantee].pd_address, pAddress, 14);
            gGuarantee_list[gNext_guarantee].field_0x30 = 1;
        } else {
            gGuarantee_list[gNext_guarantee].field_0x30 = 0;
        }
        gGuarantee_list[gNext_guarantee].local_id = pPlayer;
        gGuarantee_list[gNext_guarantee].NotifyFail = pNotifyFail;
        gGuarantee_list[gNext_guarantee].recieved = 0;
        gNext_guarantee++;
    }
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov ebx, 0xff
            mov ebp, edx
            cmp word ptr [edi + 0x14], bx
            je L004a5cbd
            mov ecx, offset gAsmString_0065a30c
            call PDFatalError
    L004a5cbd:
            cmp dword ptr [gNext_guarantee], 0xc8
            jl L004a5d46
            mov eax, offset gGuarantee_list
            mov esi, 0xa
    L004a5cd3:
            mov ecx, dword ptr [eax]
            add eax, 0x34
            cmp eax, offset gGuarantee_list+0x208
            mov byte ptr [ecx + 0x19], bl
            jl L004a5cd3
            mov edx, dword ptr [gGuarantee_list+0x1d4]
            mov eax, dword ptr [gGuarantee_list+0x208]
            cmp edx, eax
            jne L004a5d0a
            mov eax, offset gGuarantee_list+0x208
    L004a5cf6:
            cmp eax, offset gAdd_proc
            jge L004a5d0a
            mov ecx, dword ptr [eax]
            mov edx, dword ptr [eax + 0x34]
            add eax, 0x34
            inc esi
            cmp ecx, edx
            je L004a5cf6
    L004a5d0a:
            lea edx, [esi + esi*2]
            mov ecx, 0x28a0
            lea eax, [esi + edx*4]
            shl eax, 2
            sub ecx, eax
            lea edx, [eax + gGuarantee_list]
            push ecx
            push edx
            push offset gGuarantee_list
            call memmove
            mov eax, dword ptr [gNext_guarantee]
            add esp, 0xc
            sub eax, esi
            cmp eax, 0xc8
            mov dword ptr [gNext_guarantee], eax
            jge L004a5e15
    L004a5d46:
            mov cx, word ptr [gGuarantee_number]
            xor eax, eax
            mov al, byte ptr [edi + 0x18]
            mov esi, 1
            mov word ptr [eax + edi + 0x1a], cx
            mov eax, dword ptr [gNext_guarantee]
            mov cx, word ptr [gGuarantee_number]
            lea edx, [eax + eax*2]
            lea eax, [eax + edx*4]
            shl eax, 2
            mov word ptr [eax + gGuarantee_list+0x2c], cx
            inc cx
            mov word ptr [gGuarantee_number], cx
            jne L004a5d8a
            mov word ptr [gGuarantee_number], si
    L004a5d8a:
            mov dword ptr [eax + gGuarantee_list], edi
            call PDGetTotalTime
            mov edx, dword ptr [gNext_guarantee]
            lea ecx, [edx + edx*2]
            lea ecx, [edx + ecx*4]
            shl ecx, 2
            mov dword ptr [ecx + gGuarantee_list+0x4], eax
            add eax, 0x64
            mov dword ptr [ecx + gGuarantee_list+0x8], eax
            xor eax, eax
            cmp ebp, eax
            mov dword ptr [ecx + gGuarantee_list+0xc], 0x64
            je L004a5dee
            mov ebx, dword ptr [ebp]
            lea edi, [ecx + gGuarantee_list+0x14]
            mov dword ptr [ecx + gGuarantee_list+0x14], ebx
            mov ebx, dword ptr [ebp + 4]
            mov dword ptr [edi + 4], ebx
            mov ebx, dword ptr [ebp + 8]
            mov dword ptr [edi + 8], ebx
            mov bx, word ptr [ebp + 0xc]
            mov word ptr [edi + 0xc], bx
            mov dword ptr [ecx + gGuarantee_list+0x30], esi
            jmp L004a5df4
    L004a5dee:
            mov dword ptr [ecx + gGuarantee_list+0x30], eax
    L004a5df4:
            mov esi, dword ptr [esp + 0x14]
            inc edx
            mov dword ptr [ecx + gGuarantee_list+0x24], esi
            mov esi, dword ptr [esp + 0x18]
            mov dword ptr [ecx + gGuarantee_list+0x28], esi
            mov dword ptr [ecx + gGuarantee_list+0x10], eax
            mov dword ptr [gNext_guarantee], edx
    L004a5e15:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
            nop
            nop
            nop
            nop
    }
#endif
}

// NetGuaranteedSendMessageToPlayer

// FUNCTION: CARMA2_HW 0x004a6020
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress, void* pNotifyFail) {
    if (gNet_mode == eNet_mode_none && !gJoin_list_mode) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    AddToGuaranteeList(pMessage, pAddress, 0, pNotifyFail);
    return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
}

// FUNCTION: CARMA2_HW 0x004a6080
void C2_NAKED C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    tU32 time;

    C2_HOOK_BUG_ON(sizeof(tGuaranteed_message) != 0x34);

    i = 0;
    time = PDGetTotalTime();
    for (j = 0; j < gNext_guarantee; j++) {
        if (i != j) {
            memcpy(&gGuarantee_list[i], &gGuarantee_list[j], sizeof(tGuaranteed_message));
        }
        if (!gGuarantee_list[i].recieved) {
            if (gGuarantee_list[i].message->header.field_0x14 != 0xff) {
                PDFatalError("Dodgy message in guarantee list");
            }
            if (gGuarantee_list[i].NotifyFail != NULL) {
                gGuarantee_list[i].recieved |= gGuarantee_list[i].NotifyFail(time - gGuarantee_list[i].send_time, gGuarantee_list[i].message);
            } else {
                if (time - gGuarantee_list[i].send_time > 10000) {
                    gGuarantee_list[i].recieved = 1;
                }
            }
        }
        if (!gGuarantee_list[j].recieved) {
            if (time > gGuarantee_list[i].next_resend_time) {
                /* guarantee number is appended to message (and not part of the header) */
                *(tU16*)(&gGuarantee_list[j].message->contents.raw.data[gGuarantee_list[j].message->contents.raw.header.size]) = gGuarantee_list[i].field_0x2c;
                if (gGuarantee_list[i].field_0x30 == 1) {
                    GetCheckSum(gGuarantee_list[i].message);
                    PDNetSendMessageToAddress(gCurrent_net_game, gGuarantee_list[i].message, &gGuarantee_list[i].pd_address);
                } else if (!gGuarantee_list[i].field_0x30) {
                    int k;

                    for (k = 0; i < gNumber_of_net_players; k++) {
                        if (gNet_players[k].ID == gGuarantee_list[i].local_id) {
                            break;
                        }
                    }
                    if (k != gNumber_of_net_players) {
                        int guarantee_size = 0;
                        void* message_data;
                        tNet_message_chunk* message_chunk;

                        message_chunk = &gGuarantee_list[i].message->contents;
                        for (k = 0; k < 2; k++) {
                            guarantee_size += message_chunk->raw.header.size;
                            message_chunk = (tNet_message_chunk*)((tU8*)message_chunk + message_chunk->raw.header.size);
                        }
                        message_data = NetGetToPlayerContentsSize(&gNet_players[k], guarantee_size);
                        memcpy(message_data, &gGuarantee_list[i].message->contents, guarantee_size);
                    } else {
                        i++;
                        continue;
                    }
                } else {
                    PDFatalError("oops - guaranteed messages");
                }
                gGuarantee_list[i].resend_period = (tU32)(gGuarantee_list[i].resend_period * 1.2f);
                gGuarantee_list[i].next_resend_time += gGuarantee_list[i].resend_period;
                i++;
            }
        } else if ((i <= 0 || gGuarantee_list[i - 1].message != gGuarantee_list[i].message)
                   && (gNext_guarantee <= i + 1 || gGuarantee_list[j + 1].message != gGuarantee_list[i].message)) {
            gGuarantee_list[i].message->header.field_0x14 = 0;
            NetDisposeMessage(gCurrent_net_game, gGuarantee_list[j].message);
        }
    }
    gNext_guarantee = j;
#else
    __asm {
            sub esp, 0x44
            mov dword ptr [esp + 0xc], 0
            call PDGetTotalTime
            mov ecx, dword ptr [gNext_guarantee]
            mov dword ptr [esp + 0x2c], eax
            xor eax, eax
            test ecx, ecx
            mov dword ptr [esp + 0x30], eax
            jle L004a6497
            push edi
            push esi
            push ebp
            mov ebp, offset gGuarantee_list+0x10
            push ebx
            mov ebx, offset gGuarantee_list
            mov dword ptr [esp + 0x38], offset gGuarantee_list+0x34
            mov dword ptr [esp + 0x14], offset gGuarantee_list+0x8
            mov dword ptr [esp + 0x34], offset gGuarantee_list+0xc
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x2c], offset gGuarantee_list+0x24
            mov dword ptr [esp + 0x28], offset gGuarantee_list+0x30
            mov dword ptr [esp + 0x24], offset gGuarantee_list+0x14
            mov dword ptr [esp + 0x20], offset gGuarantee_list+0x2c
            mov dword ptr [esp + 0x10], offset gGuarantee_list+0x4
            mov dword ptr [esp + 0x18], offset gGuarantee_list+0x28
    L004a6102:
            cmp eax, dword ptr [esp + 0x1c]
            je L004a6118
            mov eax, dword ptr [esp + 0x38]
            mov ecx, 0xd
            mov edi, ebx
            lea esi, [eax - 0x34]
            rep movsd
    L004a6118:
            cmp dword ptr [ebp], 0
            jne L004a642c
            mov ecx, dword ptr [ebx]
            cmp word ptr [ecx + 0x14], 0xff
            je L004a6136
            mov ecx, offset gAsmString_0065a378
            call PDFatalError
    L004a6136:
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [edx]
            test eax, eax
            je L004a6153
            mov esi, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x3c]
            mov edx, dword ptr [ebx]
            sub ecx, dword ptr [esi]
            call eax
            or dword ptr [ebp], eax
            jmp L004a616b
    L004a6153:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x3c]
            sub eax, dword ptr [ecx]
            cmp eax, 0x2710
            jbe L004a616b
            mov dword ptr [ebp], 1
    L004a616b:
            cmp dword ptr [ebp], 0
            jne L004a642c
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x3c]
            cmp edx, dword ptr [eax]
            jbe L004a63c4
            mov eax, dword ptr [ebx]
            mov edx, dword ptr [esp + 0x20]
            xor ecx, ecx
            mov cl, byte ptr [eax + 0x18]
            mov dx, word ptr [edx]
            mov word ptr [ecx + eax + 0x1a], dx
            mov eax, dword ptr [esp + 0x28]
            mov eax, dword ptr [eax]
            cmp eax, 1
            jne L004a61ba
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [ebx]
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call PDNetSendMessageToAddress
            jmp L004a639b
    L004a61ba:
            test eax, eax
            jne L004a6391
            mov ecx, dword ptr [gNumber_of_net_players]
            mov ebp, offset gNet_players
            test ecx, ecx
            jle L004a61e7
            mov edx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [edx]
    L004a61d7:
            cmp dword ptr [ebp + 0x24], edx
            je L004a61e7
            inc eax
            add ebp, 0xd4
            cmp eax, ecx
            jl L004a61d7
    L004a61e7:
            cmp eax, ecx
            jne L004a6262
            mov esi, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x18]
            mov ebp, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x30]
            mov ecx, 0x34
            inc esi
            add edx, ecx
            mov edi, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x1c], esi
            mov esi, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [esp + 0x28]
            add ebp, ecx
            add esi, ecx
            add edx, ecx
            mov dword ptr [esp + 0x10], ebp
            mov ebp, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x24], esi
            mov esi, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x28], edx
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [eax], 1
            add ebp, ecx
            add edi, ecx
            add eax, ecx
            add esi, ecx
            add edx, ecx
            mov dword ptr [esp + 0x2c], ebp
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], esi
            mov dword ptr [esp + 0x14], edx
            add ebx, ecx
            mov ebp, eax
            jmp L004a6472
    L004a6262:
            mov eax, dword ptr [ebx]
            xor edi, edi
            add eax, 0x18
            mov esi, 2
    L004a626e:
            xor ecx, ecx
            mov cl, byte ptr [eax]
            add edi, ecx
            add eax, ecx
            dec esi
            jne L004a626e
            mov eax, dword ptr [gThis_net_player_index]
            mov dword ptr [esp + 0x48], edi
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            cmp ebp, edx
            jne L004a62a1
            mov ecx, offset gAsmString_0065a148
            call PDFatalError
    L004a62a1:
            mov esi, dword ptr [ebp + 0xcc]
            test esi, esi
            je L004a6334
            xor eax, eax
            mov ax, word ptr [esi + 0x16]
            add eax, edi
            cmp eax, 0x200
            jle L004a6330
            mov eax, dword ptr [gNet_mode]
            mov ecx, dword ptr [gCurrent_net_game]
            mov edi, dword ptr [ebp + 0x24]
            mov dword ptr [esp + 0x44], ecx
            test eax, eax
            je L004a6324
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 8], edx
            call PDGetTotalTime
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test edx, edx
            jle L004a6324
            mov ecx, offset gNet_players+0x24
    L004a62f6:
            cmp dword ptr [ecx], edi
            je L004a6307
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a62f6
            jmp L004a6324
    L004a6307:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            lea edx, [ecx*4 + gNet_players]
            mov ecx, dword ptr [esp + 0x44]
            push edx
            mov edx, esi
            call PDNetSendMessageToAddress
    L004a6324:
            mov dword ptr [ebp + 0xcc], 0
            xor esi, esi
    L004a6330:
            test esi, esi
            jne L004a6352
    L004a6334:
            mov ecx, 0x200
            call NetAllocateMessage
            mov esi, eax
            mov dword ptr [ebp + 0xcc], eax
            mov word ptr [esi + 0x16], 0x18
            mov word ptr [esi + 0x14], 0
    L004a6352:
            mov ax, word ptr [esi + 0x16]
            mov ecx, dword ptr [esp + 0x48]
            mov edi, eax
            add eax, ecx
            and edi, 0xffff
            mov word ptr [esi + 0x16], ax
            add edi, esi
            inc word ptr [esi + 0x14]
            mov eax, dword ptr [ebp + 0xcc]
            mov edx, ecx
            shr ecx, 2
            inc word ptr [eax + 0x14]
            mov esi, dword ptr [ebx]
            add esi, 0x18
            mov ebp, dword ptr [esp + 0x30]
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            jmp L004a639b
    L004a6391:
            mov ecx, offset gAsmString_0065a35c
            call PDFatalError
    L004a639b:
            mov esi, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x50], 0
            mov eax, dword ptr [esi]
            mov dword ptr [esp + 0x4c], eax
            fild qword ptr [esp + 0x4c]
            fmul qword ptr ds:[0x58a428]
            call _ftol
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [esi], eax
            add dword ptr [ecx], eax
    L004a63c4:
            mov esi, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x10]
            mov eax, 0x34
            inc esi
            add edx, eax
            mov edi, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x1c], esi
            mov esi, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [esp + 0x28]
            add ecx, eax
            add esi, eax
            add edx, eax
            mov dword ptr [esp + 0x10], ecx
            mov ecx, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x24], esi
            mov esi, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x28], edx
            mov edx, dword ptr [esp + 0x14]
            add edi, eax
            add ecx, eax
            add ebp, eax
            add esi, eax
            add edx, eax
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x2c], ecx
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x34], esi
            mov dword ptr [esp + 0x14], edx
            add ebx, eax
            jmp L004a646d
    L004a642c:
            cmp dword ptr [esp + 0x18], offset gGuarantee_list+0x5c
            jl L004a643f
            mov ecx, dword ptr [ebx]
            mov eax, dword ptr [ebx - 0x34]
            cmp ecx, eax
            je L004a646d
    L004a643f:
            mov edx, dword ptr [esp + 0x40]
            mov eax, dword ptr [gNext_guarantee]
            inc edx
            cmp edx, eax
            jge L004a6457
            mov ecx, dword ptr [esp + 0x38]
            mov eax, dword ptr [ebx]
            cmp eax, dword ptr [ecx]
            je L004a646d
    L004a6457:
            mov edx, dword ptr [ebx]
            mov word ptr [edx + 0x14], 0
            mov eax, dword ptr [ebx]
            cmp word ptr [eax + 0x14], 0xff
            je L004a646d
            mov byte ptr [eax + 0x19], 0xff
    L004a646d:
            mov ecx, 0x34
    L004a6472:
            mov edx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x40]
            add edx, ecx
            mov ecx, dword ptr [gNext_guarantee]
            inc eax
            mov dword ptr [esp + 0x38], edx
            cmp eax, ecx
            mov dword ptr [esp + 0x40], eax
            jl L004a6102
            pop ebx
            pop ebp
            pop esi
            pop edi
    L004a6497:
            mov eax, dword ptr [esp + 0xc]
            mov dword ptr [gNext_guarantee], eax
            add esp, 0x44
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

// NetWaitForGuaranteeReplies

// FUNCTION: CARMA2_HW 0x004a6920
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a6960
tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer) {
    tNet_game_player_info* player;

    player = NetPlayerFromID(pPlayer);
    if (player != NULL) {
        return player->car;
    } else {
        return NULL;
    }
}

// NetPlayerFromCar

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket
// FUNCTION: CARMA2_HW 0x00499a00
void C2_NAKED C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x1d8
            mov eax, dword ptr [gCurrent_net_game]
            push ebx
            push ebp
            xor ebx, ebx
            mov eax, dword ptr [eax + 0x74]
            push esi
            cmp eax, 3
            push edi
            mov dword ptr [esp + 0x34], ecx
            je L00499a26
            cmp eax, 6
            mov dword ptr [esp + 0x14], ebx
            jne L00499a2e
    L00499a26:
            mov dword ptr [esp + 0x14], 1
    L00499a2e:
            push offset gNet_score_flash_state
            mov edx, offset gNet_score_last_flash
            mov ecx, 0xc8
            call Flash
            mov esi, dword ptr [gNumber_of_net_players]
            xor edi, edi
            cmp esi, ebx
            jle L00499acc
            lea ecx, [esp + 0x15c]
            mov ebp, offset gNet_players+0x64
    L00499a5a:
            cmp dword ptr [ebp - 0x1c], 6
            jl L00499a94
            mov esi, dword ptr [ebp]
            mov dword ptr [ecx - 4], edi
            mov eax, esi
            mov dword ptr [ecx], esi
            cdq
            xor eax, edx
            sub eax, edx
            cmp eax, 0xf4240
            je L00499a8b
            cmp esi, ebx
            jge L00499a86
            mov edx, dword ptr [gCurrent_net_game]
            cmp dword ptr [edx + 0x74], 1
            jne L00499a8b
    L00499a86:
            mov dword ptr [ecx + 4], ebx
            jmp L00499ab8
    L00499a8b:
            mov eax, dword ptr [ebp + 0x30]
            inc eax
            mov dword ptr [ecx + 4], eax
            jmp L00499ab8
    L00499a94:
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [ecx - 4], 0xffffffff
            neg edx
            sbb edx, edx
            and edx, 0x1e8482
            add edx, 0xfff0bdbf
            mov dword ptr [ecx], edx
            mov dword ptr [ecx + 4], 0x3e8
    L00499ab8:
            mov esi, dword ptr [gNumber_of_net_players]
            inc edi
            add ebp, 0xd4
            add ecx, 0xc
            cmp edi, esi
            jl L00499a5a
    L00499acc:
            cmp esi, 0xc
            jge L00499b0c
            mov ecx, dword ptr [esp + 0x14]
            lea eax, [esi + esi*2]
            neg ecx
            sbb ecx, ecx
            mov edx, 0xc
            and ecx, 0x1e8482
            lea eax, [esp + eax*4 + 0x15c]
            add ecx, 0xfff0bdbf
            sub edx, esi
    L00499af6:
            mov dword ptr [eax - 4], 0xffffffff
            mov dword ptr [eax], ecx
            mov dword ptr [eax + 4], 0x3e8
            add eax, 0xc
            dec edx
            jne L00499af6
    L00499b0c:
            mov eax, dword ptr [esp + 0x14]
            cmp eax, ebx
            mov eax, offset SortNetScoresAscending
            jne L00499b1e
            mov eax, offset SortNetScoresDescending
    L00499b1e:
            push eax
            push 0xc
            lea ecx, [esp + 0x160]
            push 0xc
            push ecx
            call qsort
            mov edx, dword ptr [gNumber_of_net_players]
            add esp, 0x10
            xor eax, eax
            cmp edx, ebx
            jle L00499b5e
            mov esi, dword ptr [gThis_net_player_index]
            lea ecx, [esp + 0x158]
    L00499b4c:
            cmp dword ptr [ecx], esi
            je L00499b5a
            inc eax
            add ecx, 0xc
            cmp eax, edx
            jl L00499b4c
            jmp L00499b5e
    L00499b5a:
            mov dword ptr [esp + 0x30], eax
    L00499b5e:
            xor eax, eax
            lea edx, [esp + 0x158]
            mov dword ptr [esp + 0x2c], eax
            mov dword ptr [esp + 0x1c], offset gNet_score_actors
            mov dword ptr [esp + 0x28], 1
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x14], 0xffffffff
    L00499b87:
            mov ecx, dword ptr [esp + 0x24]
            mov edi, dword ptr [ecx]
            cmp edi, ebx
            mov dword ptr [esp + 0x20], edi
            jl L00499ba6
            lea ecx, [edi + edi*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, edi
            mov dword ptr [edx*4 + gNet_players+0x94], eax
    L00499ba6:
            cmp dword ptr [esp + 0x34], ebx
            jne L0049a2e7
            cmp edi, ebx
            jl L0049a2e7
            mov ecx, dword ptr [esp + 0x30]
            cmp ecx, eax
            mov eax, dword ptr [gNet_score_scene_initialised]
            jne L00499c61
            cmp eax, ebx
            jne L00499bd2
            call FUN_0049a410
    L00499bd2:
            cmp dword ptr [gNet_score_scene_initialised], ebx
            je L00499c54
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [gNet_score_materials+0x4]
            push ebx
            push ecx
            mov esi, dword ptr [eax]
            mov eax, dword ptr [esp + 0x1c]
            add eax, 0xf0
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esi + 0x1c], ecx
            fild dword ptr [esp + 0x18]
            mov edx, dword ptr [gNet_score_model]
            lea ecx, [esi + 0x2c]
            mov dword ptr [esi + 0x18], edx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0x435c0000
            push ecx
            call BrMatrix34Translate
            mov edx, dword ptr [gNet_score_root]
            add esp, 0x10
            push esi
            push edx
            call BrActorAdd
            mov eax, dword ptr [gDepth_buffer]
            mov ecx, dword ptr [gRender_screen]
            mov edx, dword ptr [gNet_score_camera]
            add esp, 8
            push eax
            mov eax, dword ptr [gNet_score_root]
            push ecx
            push edx
            push eax
            call BrZbSceneRender
            add esp, 0x10
            push esi
            call BrActorRemove
            add esp, 4
    L00499c54:
            mov dword ptr [esp + 0x10], 0x13
            jmp L00499fd4
    L00499c61:
            cmp eax, ebx
            jne L00499f51
            push ebx
            push ebx
            call BrActorAllocate
            add esp, 8
            mov dword ptr [gNet_score_root], eax
            mov esi, offset gNet_score_actors
    L00499c7d:
            push ebx
            push 1
            call BrActorAllocate
            add esp, 8
            cmp eax, ebx
            mov dword ptr [esi], eax
            jne L00499c9b
            push offset gAsmString_00659cd8
            call BrFailure
            add esp, 4
    L00499c9b:
            mov ecx, dword ptr [esi]
            add esi, 4
            cmp esi, 0x74a770
            mov byte ptr [ecx + 0x20], 4
            jl L00499c7d
            mov esi, 2
            push esi
            push 4
            push offset gAsmString_00659cc8
            call BrModelAllocate
            mov dword ptr [gNet_score_model], eax
            mov edx, dword ptr [eax + 0xc]
            add esp, 0xc
            mov word ptr [edx], bx
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 0xc]
            mov word ptr [ecx + 2], 1
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax + 4], si
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 0xc]
            mov word ptr [edx + 0x28], 1
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 0xc]
            mov word ptr [ecx + 0x2a], 3
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax + 0x2c], si
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 0xc]
            mov dword ptr [edx + 8], ebx
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ecx + 0x30], ebx
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax], ebx
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx + 4], ebx
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 8]
            mov eax, 0xc0000000
            mov dword ptr [ecx + 8], eax
            mov edx, dword ptr [gNet_score_model]
            mov ecx, 0x42c80000
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x28], ecx
            mov edx, dword ptr [gNet_score_model]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x2c], ebx
            mov edx, dword ptr [gNet_score_model]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x30], eax
            mov edx, dword ptr [gNet_score_model]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x50], ebx
            mov edx, dword ptr [gNet_score_model]
            mov edi, dword ptr [edx + 8]
            mov edx, 0xc2180000
            mov dword ptr [edi + 0x54], edx
            mov edi, dword ptr [gNet_score_model]
            mov edi, dword ptr [edi + 8]
            mov dword ptr [edi + 0x58], eax
            mov edi, dword ptr [gNet_score_model]
            mov edi, dword ptr [edi + 8]
            mov dword ptr [edi + 0x78], ecx
            mov ecx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [ecx + 0x7c], edx
            mov edx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0x80], eax
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax + 0xc], ebx
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx + 0x10], ebx
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 8]
            mov eax, 0x3f480000
            mov dword ptr [ecx + 0x34], eax
            mov edx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0x38], ebx
            mov edx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 0x5c], ebx
            mov edx, dword ptr [gNet_score_model]
            mov ecx, 0x3e980000
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x60], ecx
            mov edx, dword ptr [gNet_score_model]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x84], eax
            mov eax, dword ptr [gNet_score_model]
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x88], ecx
            mov eax, dword ptr [gNet_score_model]
            or word ptr [eax + 0x20], si
            mov eax, dword ptr [gNet_score_model]
            push eax
            call BrModelAdd
            add esp, 4
            xor eax, eax
            mov edi, offset gNet_score_materials
    L00499e54:
            inc eax
            lea ecx, [esp + 0x38]
            push eax
            push offset gAsmString_00659cb8
            push ecx
            mov dword ptr [esp + 0x1c], eax
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x38]
            call LoadPixelmap
            mov ebp, eax
            cmp ebp, ebx
            jne L00499e8d
            lea edx, [esp + 0x38]
            push edx
            push offset gAsmString_00659c98
            call BrFailure
            add esp, 8
    L00499e8d:
            push ebp
            call BrMapAdd
            add esp, 4
            push offset gAsmString_00659c84
            call BrMaterialAllocate
            add esp, 4
            mov esi, eax
            push esi
            mov dword ptr [esi + 8], ebx
            mov dword ptr [esi + 0x40], ebp
            mov dword ptr [esi + 0x20], 0x2800
            call BrMaterialAdd
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [edi], esi
            add edi, 4
            add esp, 4
            cmp edi, offset gNet_score_camera
            jl L00499e54
            push ebx
            push 3
            call BrActorAllocate
            mov dword ptr [gNet_score_camera], eax
            mov dword ptr [eax + 0x14], offset gAsmString_00659c74
            mov eax, dword ptr [gNet_score_camera]
            xor ecx, ecx
            add esp, 8
            mov eax, dword ptr [eax + 0x5c]
            mov byte ptr [eax + 4], bl
            mov dword ptr [eax + 8], 0x3f800000
            mov dword ptr [eax + 0xc], 0x40400000
            mov edx, dword ptr [gScreen]
            mov cx, word ptr [edx + 0x34]
            mov dword ptr [esp + 0x10], ecx
            xor ecx, ecx
            fild dword ptr [esp + 0x10]
            fstp dword ptr [eax + 0x14]
            mov edx, dword ptr [gScreen]
            mov cx, word ptr [edx + 0x36]
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            fstp dword ptr [eax + 0x18]
            mov eax, dword ptr [gNet_score_camera]
            mov ecx, dword ptr [gNet_score_root]
            push eax
            push ecx
            call BrActorAdd
            mov edi, dword ptr [esp + 0x28]
            mov eax, 1
            add esp, 8
            cmp eax, ebx
            mov dword ptr [gNet_score_scene_initialised], eax
            je L00499fcc
    L00499f51:
            mov edx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [gNet_score_materials]
            push ebx
            mov esi, dword ptr [edx]
            mov edx, dword ptr [esp + 0x18]
            add edx, 0xf0
            mov dword ptr [esi + 0x1c], eax
            mov ecx, dword ptr [gNet_score_model]
            mov dword ptr [esp + 0x14], edx
            push ecx
            fild dword ptr [esp + 0x18]
            lea eax, [esi + 0x2c]
            mov dword ptr [esi + 0x18], ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0x435c0000
            push eax
            call BrMatrix34Translate
            mov ecx, dword ptr [gNet_score_root]
            add esp, 0x10
            push esi
            push ecx
            call BrActorAdd
            mov edx, dword ptr [gDepth_buffer]
            mov eax, dword ptr [gRender_screen]
            mov ecx, dword ptr [gNet_score_camera]
            add esp, 8
            push edx
            mov edx, dword ptr [gNet_score_root]
            push eax
            push ecx
            push edx
            call BrZbSceneRender
            add esp, 0x10
            push esi
            call BrActorRemove
            add esp, 4
    L00499fcc:
            mov dword ptr [esp + 0x10], 0x12
    L00499fd4:
            mov esi, dword ptr [gNet_score_flash_state]
            cmp esi, ebx
            jne L00499ff1
            mov eax, dword ptr [gCurrent_net_game]
            cmp dword ptr [eax + 0x74], 7
            jne L00499ff1
            cmp edi, dword ptr [gIt_or_fox]
            je L0049a06d
    L00499ff1:
            lea esi, [edi + edi*2]
            lea esi, [esi + esi*8]
            shl esi, 1
            sub esi, edi
            shl esi, 2
            cmp dword ptr [esi + gNet_players+0x74], ebx
            je L0049a048
            lea ebp, [esi + gNet_players+0x28]
            mov ecx, offset gFonts+0x15c0
            mov edx, ebp
            call DRTextWidth
            cmp eax, 0x4d
            jle L0049a042
    L0049a01d:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, ebp
            repne scasb
            not ecx
            dec ecx
            mov byte ptr [ecx + ebp - 1], bl
            mov ecx, offset gFonts+0x15c0
            call DRTextWidth
            cmp eax, 0x4d
            jg L0049a01d
            mov edi, dword ptr [esp + 0x20]
    L0049a042:
            mov dword ptr [esi + gNet_players+0x74], ebx
    L0049a048:
            mov edx, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x10]
            push ebx
            push ebx
            add edx, 4
            push ecx
            push edx
            mov edx, 0x223
            lea ecx, [esi + gNet_players+0x28]
            call PolyFontText
            mov esi, dword ptr [gNet_score_flash_state]
    L0049a06d:
            lea ebp, [edi + edi*2]
            lea ebp, [ebp + ebp*8]
            shl ebp, 1
            sub ebp, edi
            shl ebp, 2
            mov ecx, dword ptr [ebp + gNet_players+0x64]
            mov eax, ecx
            cdq
            xor eax, edx
            sub eax, edx
            cmp eax, 0xf4240
            jne L0049a0c9
            cmp esi, ebx
            je L0049a1cf
            mov ecx, 0xad
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x58]
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
            jmp L0049a1d3
    L0049a0c9:
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x74]
            cmp eax, 7
            ja L0049a1d3
            jmp dword ptr [eax*4 + 0x49a30c]
            cmp ecx, ebx
            jne L0049a0f8
            mov ecx, 0xc0
            call GetMiscString
            push eax
            push offset gAsmString_0059101c
            jmp L0049a128
    L0049a0f8:
            cmp esi, ebx
            je L0049a1cf
            mov ecx, 0xc1
            jmp L0049a1b5
            push ecx
            lea edx, [esp + 0x5c]
            push offset gAsmString_00655b3c
            push edx
            call sprintf
            add esp, 0xc
            jmp L0049a1d3
            push ecx
            push offset gAsmString_0058f418
    L0049a128:
            lea eax, [esp + 0x60]
            push eax
            call sprintf
            add esp, 0xc
            jmp L0049a1d3
            sar ecx, 0x10
            push ecx
            push offset gAsmString_00659d04
            jmp L0049a1c0
            cmp edi, dword ptr [gIt_or_fox]
            jne L0049a151
            cmp esi, ebx
            je L0049a1cf
    L0049a151:
            push ebx
            push 1
            push ebx
            lea edx, [esp + 0x64]
            call TimerString
            jmp L0049a1d3
            mov esi, dword ptr [gCurrent_race+0x8]
            mov eax, ecx
            cdq
            idiv esi
            mov eax, ecx
            inc edx
            push edx
            cdq
            idiv esi
            lea edx, [esp + 0x5c]
            inc eax
            push eax
            push offset gAsmString_00659cfc
            push edx
            call sprintf
            add esp, 0x10
            jmp L0049a1d3
            cmp ecx, ebx
            jl L0049a1ac
            push ecx
            mov ecx, 0x5c
            call GetMiscString
            push eax
            lea eax, [esp + 0x60]
            push offset gAsmString_00659cf4
            push eax
            call sprintf
            add esp, 0x10
            jmp L0049a1d3
    L0049a1ac:
            cmp esi, ebx
            je L0049a1cf
            mov ecx, 0x5d
    L0049a1b5:
            call GetMiscString
            push eax
            push offset gAsmString_0059101c
    L0049a1c0:
            lea ecx, [esp + 0x60]
            push ecx
            call sprintf
            add esp, 0xc
            jmp L0049a1d3
    L0049a1cf:
            mov byte ptr [esp + 0x58], bl
    L0049a1d3:
            mov dl, byte ptr [esp + 0x2c]
            mov esi, dword ptr [esp + 0x28]
            add dl, 0x30
            push ebx
            push ebx
            lea eax, [esi + 3]
            mov byte ptr [esp + 0x22], dl
            push 0x19
            push eax
            mov edx, 0x270
            lea ecx, [esp + 0x2a]
            mov byte ptr [esp + 0x2b], bl
            call PolyFontText
            mov edx, dword ptr [gCar_icons_model_actor]
            mov edi, dword ptr [esp + 0x14]
            push ebx
            add edx, 0x2c
            lea ecx, [edi - 0x14]
            mov dword ptr [esp + 0x24], ecx
            push ecx
            fild dword ptr [esp + 0x28]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0x44188000
            push edx
            call BrMatrix34Translate
            mov eax, dword ptr [ebp + gNet_players+0x4c]
            mov edx, dword ptr [gCar_icons_model_actor]
            add esp, 0x10
            mov ecx, dword ptr [eax*8 + gCar_icons+0x4]
            mov eax, dword ptr [edx + 0x1c]
            push 8
            mov ecx, dword ptr [ecx*4 + gTexture_maps]
            mov dword ptr [eax + 0x40], ecx
            mov edx, dword ptr [gCar_icons_model_actor]
            mov eax, dword ptr [edx + 0x1c]
            push eax
            call BrMaterialUpdate
            mov ecx, dword ptr [ebp + gNet_players+0x4c]
            mov eax, dword ptr [gCar_icons_model_actor]
            add esp, 8
            mov edx, dword ptr [ecx*8 + gCar_icons]
            mov dword ptr [eax + 0x18], edx
            mov ecx, dword ptr [gCar_icons_model_actor]
            mov edx, dword ptr [gNet_score_camera]
            push ecx
            push edx
            call BrActorAdd
            mov eax, dword ptr [gDepth_buffer]
            mov ecx, dword ptr [gBack_screen]
            add esp, 8
            push eax
            mov eax, dword ptr [gNet_score_camera]
            push ecx
            push eax
            push eax
            call BrZbsSceneRender
            mov edx, dword ptr [gCar_icons_model_actor]
            add esp, 0x10
            push edx
            call BrActorRemove
            mov eax, dword ptr [esp + 0x14]
            add esp, 4
            lea ecx, [esi + 0x12]
            mov edx, 0x223
            push ebx
            push ebx
            push eax
            push ecx
            lea ecx, [esp + 0x68]
            call PolyFontText
            sub edi, 0x28
            add esi, 0x28
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x14], edi
            add eax, 4
            mov dword ptr [esp + 0x28], esi
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0x2c]
    L0049a2e7:
            mov edx, dword ptr [esp + 0x24]
            inc eax
            add edx, 0xc
            cmp eax, 0xc
            mov dword ptr [esp + 0x2c], eax
            mov dword ptr [esp + 0x24], edx
            jl L00499b87
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1d8
            ret
            nop
            _emit 0x0a
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0x22
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0x22
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0x3a
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0x88
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0x60
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0xe2
            _emit 0xa0
            _emit 0x49
            _emit 0x00
            _emit 0x45
            _emit 0xa1
            _emit 0x49
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049a410
void C2_NAKED C2_HOOK_FASTCALL FUN_0049a410(void) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor** actor_ptr;
    br_camera* camera;
    br_material* material;
    br_pixelmap* pixelmap;
    int material_index;
    char name_buffer[32];

    gNet_score_root = BrActorAllocate(0, NULL);
    actor_ptr = gNet_score_actors;
    do {
        *actor_ptr = BrActorAllocate(1, NULL);
        if (*actor_ptr == NULL) {
            BrFailure("Out of memory for NetPlay");
        }
        (*actor_ptr)->render_style = 4;
        actor_ptr++;
    } while ((int)actor_ptr < (int)(gNet_score_actors + 12));

    gNet_score_model = BrModelAllocate("NetScore Model", 4, 2);
    gNet_score_model->faces[0].vertices[0] = 0;
    gNet_score_model->faces[0].vertices[1] = 1;
    gNet_score_model->faces[0].vertices[2] = 2;
    gNet_score_model->faces[1].vertices[0] = 1;
    gNet_score_model->faces[1].vertices[1] = 3;
    gNet_score_model->faces[1].vertices[2] = 2;
    gNet_score_model->faces[0].material = NULL;
    gNet_score_model->faces[1].material = NULL;
    gNet_score_model->vertices[0].p.v[0] = 0.0f;
    gNet_score_model->vertices[0].p.v[1] = 0.0f;
    gNet_score_model->vertices[0].p.v[2] = -2.0f;
    gNet_score_model->vertices[1].p.v[0] = 100.0f;
    gNet_score_model->vertices[1].p.v[1] = 0.0f;
    gNet_score_model->vertices[1].p.v[2] = -2.0f;
    gNet_score_model->vertices[2].p.v[0] = 0.0f;
    gNet_score_model->vertices[2].p.v[1] = -38.0f;
    gNet_score_model->vertices[2].p.v[2] = -2.0f;
    gNet_score_model->vertices[3].p.v[0] = 100.0f;
    gNet_score_model->vertices[3].p.v[1] = -38.0f;
    gNet_score_model->vertices[3].p.v[2] = -2.0f;
    gNet_score_model->vertices[0].map.v[0] = 0.0f;
    gNet_score_model->vertices[0].map.v[1] = 0.0f;
    gNet_score_model->vertices[1].map.v[0] = 0.78125f;
    gNet_score_model->vertices[1].map.v[1] = 0.0f;
    gNet_score_model->vertices[2].map.v[0] = 0.0f;
    gNet_score_model->vertices[2].map.v[1] = 0.296875f;
    gNet_score_model->vertices[3].map.v[0] = 0.78125f;
    gNet_score_model->vertices[3].map.v[1] = 0.296875f;
    gNet_score_model->flags |= 2;
    BrModelAdd(gNet_score_model);

    for (material_index = 0; material_index < 4; material_index++) {
        sprintf(name_buffer, "NETBOX%i.PIX", material_index + 1);
        pixelmap = LoadPixelmap(name_buffer);
        if (pixelmap == NULL) {
            BrFailure("Cannot find headup graphic %s", name_buffer);
        }
        BrMapAdd(pixelmap);
        material = BrMaterialAllocate("NetScore Material");
        material->colour = 0;
        material->colour_map = pixelmap;
        material->flags = 0x2800;
        BrMaterialAdd(material);
        gNet_score_materials[material_index] = material;
    }

    gNet_score_camera = BrActorAllocate(3, NULL);
    gNet_score_camera->identifier = "Net HUD Camera";
    camera = (br_camera*)gNet_score_camera->type_data;
    camera->type = 0;
    camera->hither_z = 1.0f;
    camera->yon_z = 3.0f;
    camera->width = (float)gScreen->width;
    camera->height = (float)gScreen->height;
    BrActorAdd(gNet_score_root, gNet_score_camera);
    gNet_score_scene_initialised = 1;
#else
    __asm {
            sub esp, 0x24
            push ebx
            push ebp
            push esi
            xor esi, esi
            push edi
            push esi
            push esi
            call BrActorAllocate
            add esp, 8
            mov dword ptr [gNet_score_root], eax
            mov edi, offset gNet_score_actors
    L0049a42d:
            push esi
            push 1
            call BrActorAllocate
            add esp, 8
            cmp eax, esi
            mov dword ptr [edi], eax
            jne L0049a44b
            push offset gAsmString_00659cd8
            call BrFailure
            add esp, 4
    L0049a44b:
            mov eax, dword ptr [edi]
            add edi, 4
            cmp edi, 0x74a770
            mov byte ptr [eax + 0x20], 4
            jl L0049a42d
            mov edi, 2
            push edi
            push 4
            push offset gAsmString_00659cc8
            call BrModelAllocate
            mov dword ptr [gNet_score_model], eax
            mov ecx, dword ptr [eax + 0xc]
            add esp, 0xc
            mov word ptr [ecx], si
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax + 2], 1
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 0xc]
            mov word ptr [edx + 4], di
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 0xc]
            mov word ptr [ecx + 0x28], 1
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax + 0x2a], 3
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 0xc]
            mov word ptr [edx + 0x2c], di
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ecx + 8], esi
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 0xc]
            mov dword ptr [eax + 0x30], esi
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx], esi
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 8]
            mov eax, 0xc0000000
            mov dword ptr [ecx + 4], esi
            mov edx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ecx + 8], eax
            mov edx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [edx + 8]
            mov edx, 0x42c80000
            mov dword ptr [ecx + 0x28], edx
            mov ecx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [ecx + 0x2c], esi
            mov ecx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [ecx + 0x30], eax
            mov ecx, dword ptr [gNet_score_model]
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [ecx + 0x50], esi
            mov ecx, dword ptr [gNet_score_model]
            mov ebx, dword ptr [ecx + 8]
            mov ecx, 0xc2180000
            mov dword ptr [ebx + 0x54], ecx
            mov ebx, dword ptr [gNet_score_model]
            mov ebx, dword ptr [ebx + 8]
            mov dword ptr [ebx + 0x58], eax
            mov ebx, dword ptr [gNet_score_model]
            mov ebx, dword ptr [ebx + 8]
            mov dword ptr [ebx + 0x78], edx
            mov edx, dword ptr [gNet_score_model]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x7c], ecx
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx + 0x80], eax
            mov eax, dword ptr [gNet_score_model]
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0xc], esi
            mov edx, dword ptr [gNet_score_model]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax + 0x10], esi
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 8]
            mov ecx, 0x3f480000
            mov dword ptr [edx + 0x34], ecx
            mov eax, dword ptr [gNet_score_model]
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x38], esi
            mov eax, dword ptr [gNet_score_model]
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x5c], esi
            mov eax, dword ptr [gNet_score_model]
            mov edx, dword ptr [eax + 8]
            mov eax, 0x3e980000
            mov dword ptr [edx + 0x60], eax
            mov edx, dword ptr [gNet_score_model]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + 0x84], ecx
            mov ecx, dword ptr [gNet_score_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx + 0x88], eax
            mov eax, dword ptr [gNet_score_model]
            or word ptr [eax + 0x20], di
            mov eax, dword ptr [gNet_score_model]
            push eax
            call BrModelAdd
            add esp, 4
            xor ebx, ebx
            mov edi, offset gNet_score_materials
            jmp L0049a607
    L0049a605:
            xor esi, esi
    L0049a607:
            inc ebx
            lea ecx, [esp + 0x14]
            push ebx
            push offset gAsmString_00659cb8
            push ecx
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x14]
            call LoadPixelmap
            mov ebp, eax
            cmp ebp, esi
            jne L0049a63c
            lea edx, [esp + 0x14]
            push edx
            push offset gAsmString_00659c98
            call BrFailure
            add esp, 8
    L0049a63c:
            push ebp
            call BrMapAdd
            add esp, 4
            push offset gAsmString_00659c84
            call BrMaterialAllocate
            add esp, 4
            mov esi, eax
            push esi
            mov dword ptr [esi + 8], 0
            mov dword ptr [esi + 0x40], ebp
            mov dword ptr [esi + 0x20], 0x2800
            call BrMaterialAdd
            mov dword ptr [edi], esi
            add edi, 4
            add esp, 4
            cmp edi, offset gNet_score_camera
            jl L0049a605
            push 0
            push 3
            call BrActorAllocate
            mov dword ptr [gNet_score_camera], eax
            mov dword ptr [eax + 0x14], offset gAsmString_00659c74
            mov eax, dword ptr [gNet_score_camera]
            xor ecx, ecx
            add esp, 8
            mov eax, dword ptr [eax + 0x5c]
            mov byte ptr [eax + 4], 0
            mov dword ptr [eax + 8], 0x3f800000
            mov dword ptr [eax + 0xc], 0x40400000
            mov edx, dword ptr [gScreen]
            mov cx, word ptr [edx + 0x34]
            mov dword ptr [esp + 0x10], ecx
            xor ecx, ecx
            fild dword ptr [esp + 0x10]
            fstp dword ptr [eax + 0x14]
            mov edx, dword ptr [gScreen]
            mov cx, word ptr [edx + 0x36]
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            fstp dword ptr [eax + 0x18]
            mov eax, dword ptr [gNet_score_camera]
            mov ecx, dword ptr [gNet_score_root]
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
            mov dword ptr [gNet_score_scene_initialised], 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
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

#include <limits.h>

extern void C2_HOOK_FASTCALL DisposeCarN(int pPlayer_index);
extern char* gCurrent_palette_pixels;
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);
extern void C2_HOOK_FASTCALL EnsurePaletteUp(void);
extern void C2_HOOK_FASTCALL FadePaletteUp(void);
extern void C2_HOOK_FASTCALL FUN_004a48d0(void* pPayload, void* pMessage);
extern void C2_HOOK_FASTCALL FUN_0049bad0(void* pPayload, void* pMessage, int pReceive_time);
extern void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents);
extern void C2_HOOK_FASTCALL ReceivedCrush(tU8* pContents);
extern void C2_HOOK_FASTCALL FUN_00498bb0(void);
extern void C2_HOOK_FASTCALL FUN_0043c940(void* pPayload);
extern void C2_HOOK_FASTCALL FUN_0043cd60(void* pPayload);
extern void C2_HOOK_FASTCALL ReceiveMineUpdate(tNet_message_chunk_mine_explode* pChunk);

typedef struct tNet_score_entry {
    int player_index;
    int score;
    int out_of_game;
} tNet_score_entry;

// GLOBAL: CARMA2_HW 0x0068d94c
tU32 gNet_score_last_flash;

// GLOBAL: CARMA2_HW 0x0068d950
int gNet_score_flash_state;

// FUNCTION: CARMA2_HW 0x0049a330
int C2_NAKED C2_HOOK_CDECL SortNetScoresAscending(const void* pFirst, const void* pSecond) {
#ifndef CARPOCALYPSE2_MATCHING
    const tNet_score_entry* first;
    const tNet_score_entry* second;

    first = (const tNet_score_entry*)pFirst;
    second = (const tNet_score_entry*)pSecond;
    if (first->out_of_game != 0) {
        if (second->out_of_game != 0) {
            return first->out_of_game - second->out_of_game;
        }
        return INT_MAX;
    }
    if (second->out_of_game != 0) {
        return INT_MIN + 1;
    }
    if (first->score != second->score) {
        return first->score - second->score;
    }
    return gNet_players[first->player_index].last_score_index - gNet_players[second->player_index].last_score_index;
#else
    __asm {
            mov edx, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            push esi
            mov eax, dword ptr [edx + 8]
            test eax, eax
            je L0049a34e
            mov ecx, dword ptr [ecx + 8]
            test ecx, ecx
            jne L0049a390
            mov eax, 0x7fffffff
            pop esi
            ret
    L0049a34e:
            mov eax, dword ptr [ecx + 8]
            test eax, eax
            je L0049a35c
            mov eax, 0x80000001
            pop esi
            ret
    L0049a35c:
            mov eax, dword ptr [edx + 4]
            mov esi, dword ptr [ecx + 4]
            cmp eax, esi
            je L0049a36a
            sub eax, esi
            pop esi
            ret
    L0049a36a:
            mov eax, dword ptr [ecx]
            lea ecx, [eax + eax*2]
            lea esi, [ecx + ecx*8]
            shl esi, 1
            sub esi, eax
            mov eax, dword ptr [edx]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, dword ptr [esi*4 + gNet_players+0x94]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [edx*4 + gNet_players+0x94]
    L0049a390:
            sub eax, ecx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049a3a0
int C2_NAKED C2_HOOK_CDECL SortNetScoresDescending(const void* pFirst, const void* pSecond) {
#ifndef CARPOCALYPSE2_MATCHING
    const tNet_score_entry* first;
    const tNet_score_entry* second;

    first = (const tNet_score_entry*)pFirst;
    second = (const tNet_score_entry*)pSecond;
    if (first->out_of_game != 0) {
        if (second->out_of_game != 0) {
            return first->out_of_game - second->out_of_game;
        }
        return INT_MAX;
    }
    if (second->out_of_game != 0) {
        return INT_MIN + 1;
    }
    if (first->score != second->score) {
        return second->score - first->score;
    }
    return gNet_players[first->player_index].last_score_index - gNet_players[second->player_index].last_score_index;
#else
    __asm {
            mov edx, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            push esi
            mov eax, dword ptr [edx + 8]
            test eax, eax
            je L0049a3be
            mov ecx, dword ptr [ecx + 8]
            test ecx, ecx
            jne L0049a400
            mov eax, 0x7fffffff
            pop esi
            ret
    L0049a3be:
            mov eax, dword ptr [ecx + 8]
            test eax, eax
            je L0049a3cc
            mov eax, 0x80000001
            pop esi
            ret
    L0049a3cc:
            mov esi, dword ptr [edx + 4]
            mov eax, dword ptr [ecx + 4]
            cmp esi, eax
            je L0049a3da
            sub eax, esi
            pop esi
            ret
    L0049a3da:
            mov eax, dword ptr [ecx]
            lea ecx, [eax + eax*2]
            lea esi, [ecx + ecx*8]
            shl esi, 1
            sub esi, eax
            mov eax, dword ptr [edx]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, dword ptr [esi*4 + gNet_players+0x94]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [edx*4 + gNet_players+0x94]
    L0049a400:
            sub eax, ecx
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
    }
#endif
}

void C2_HOOK_FASTCALL FUN_0049a410(void);

// FUNCTION: CARMA2_HW 0x004a64b0
int C2_HOOK_FASTCALL FUN_004a64b0(unsigned int pArg) {
    return pArg >= 10000;
}

// FUNCTION: CARMA2_HW 0x004a69f0
void C2_HOOK_FASTCALL NetworkNoOp(void) {
}

// FUNCTION: CARMA2_HW 0x004a48d0
void C2_NAKED C2_HOOK_FASTCALL FUN_004a48d0(void* pPayload, void* pMessage) {
#ifndef CARPOCALYPSE2_MATCHING
    register int iVar1;
    register int iVar3;
    register int iVar4;
    register volatile int* piVar5;
    tNet_game_player_info* player;

    if (gCurrent_net_game != NULL) {
        iVar3 = gINT_0068d964;
        while (1) {
            if (*(int*)((tU8*)pPayload + 4) <= 0) {
                gINT_0068d988 = 1;
                if (*(int*)((tU8*)pPayload + 4) >= 0) {
                    return;
                }
                gINT_00690234 = 1;
                return;
            }
            if (iVar3 > *(int*)((tU8*)pPayload + 0xc)) {
                return;
            }
            if (gINT_00690c30 == 0) {
                gINT_0068d974 = *(int*)((tU8*)pMessage + 0x10);
                piVar5 = (int*)&gReceived_net_player_list[0].car_index;
                do {
                    *piVar5 = -1;
                    piVar5 += 0x35;
                } while ((int)piVar5 < (int)((tU8*)gReceived_net_player_list + 0xa3c));
                gINT_00690c30 = 1;
                iVar3 = *(int*)((tU8*)pPayload + 0xc);
                gINT_0068d964 = iVar3;
            }
            if (iVar3 >= *(int*)((tU8*)pPayload + 0xc)) {
                break;
            }
            gINT_00690c30 = 0;
        }
        memcpy((tU8*)gReceived_net_player_list + *(int*)((tU8*)pPayload + 8) * 0xd4,
            (tU8*)pPayload + 0x10, sizeof(tNet_game_player_info));
        iVar3 = 0;
        if (0 < *(int*)((tU8*)pPayload + 4)) {
            piVar5 = (int*)&gReceived_net_player_list[0].car_index;
            do {
                if (*piVar5 < 0) {
                    return;
                }
                iVar3++;
                piVar5 += 0x35;
            } while (iVar3 < *(int*)((tU8*)pPayload + 4));
        }
        gINT_00690c30 = 0;
        NetPlayersChanged(*(int*)((tU8*)pPayload + 4), gReceived_net_player_list);
        gThis_net_player_index = -1;
        iVar4 = 0;
        if (0 < gNumber_of_net_players) {
            player = gNet_players;
            do {
                if (player->ID == gLocal_net_ID) {
                    gThis_net_player_index = iVar4;
                    break;
                }
                iVar4++;
                player++;
            } while (iVar4 < gNumber_of_net_players);
        }
        if (gThis_net_player_index < 0) {
            FatalError(0x69);
        }
        gNet_players[0].this_players_time_stamp = PDGetTotalTime();
        iVar3 = gNumber_of_net_players;
        gCurrent_race.number_of_racers = gNumber_of_net_players;
        if (gStart_race_sent != 0 && (iVar4 = 0, 0 < gNumber_of_net_players)) {
            piVar5 = (int*)&gNet_players[0].opponent_list_index;
            do {
                iVar1 = *piVar5;
                piVar5 += 0x35;
                gCurrent_race.opponent_list[iVar1].net_player_index = iVar4;
                iVar4++;
            } while (iVar4 < iVar3);
        }
    }
#else
    __asm {
            mov eax, dword ptr [gCurrent_net_game]
            push ebx
            push esi
            push edi
            test eax, eax
            mov ebx, ecx
            je L004a4a3b
            mov eax, dword ptr [gINT_0068d964]
            mov ecx, dword ptr [gINT_00690c30]
            mov esi, 1
    L004a48f2:
            mov edi, dword ptr [ebx + 4]
            test edi, edi
            jle L004a4942
            cmp eax, dword ptr [ebx + 0xc]
            jg L004a4a3b
            test ecx, ecx
            jne L004a4933
            mov eax, dword ptr [edx + 0x10]
            mov dword ptr [gINT_0068d974], eax
            mov eax, offset gReceived_net_player_list+0x4c
    L004a4913:
            mov dword ptr [eax], 0xffffffff
            add eax, 0xd4
            cmp eax, offset gPTR_00690c7c
            jl L004a4913
            mov dword ptr [gINT_00690c30], esi
            mov eax, dword ptr [ebx + 0xc]
            mov dword ptr [gINT_0068d964], eax
    L004a4933:
            cmp eax, dword ptr [ebx + 0xc]
            jge L004a495d
            xor ecx, ecx
            mov dword ptr [gINT_00690c30], ecx
            jmp L004a48f2
    L004a4942:
            mov dword ptr [gINT_0068d988], esi
            mov eax, dword ptr [ebx + 4]
            test eax, eax
            jge L004a4a3b
            mov dword ptr [gINT_00690234], esi
            pop edi
            pop esi
            pop ebx
            ret
    L004a495d:
            mov eax, dword ptr [ebx + 8]
            lea esi, [ebx + 0x10]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            xor eax, eax
            lea edi, [ecx*4 + gReceived_net_player_list]
            mov ecx, 0x35
            rep movsd
            mov edx, dword ptr [ebx + 4]
            test edx, edx
            jle L004a499d
            mov ecx, offset gReceived_net_player_list+0x4c
    L004a4989:
            cmp dword ptr [ecx], 0
            jl L004a4a3b
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a4989
    L004a499d:
            mov dword ptr [gINT_00690c30], 0
            mov ecx, dword ptr [ebx + 4]
            mov edx, offset gReceived_net_player_list
            call NetPlayersChanged
            mov edx, dword ptr [gNumber_of_net_players]
            or edi, 0xffffffff
            xor eax, eax
            mov dword ptr [gThis_net_player_index], edi
            test edx, edx
            jle L004a49ed
            mov esi, dword ptr [gLocal_net_ID]
            mov ecx, offset gNet_players+0x24
    L004a49d4:
            cmp dword ptr [ecx], esi
            je L004a49e5
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004a49d4
            jmp L004a49ed
    L004a49e5:
            mov edi, eax
            mov dword ptr [gThis_net_player_index], edi
    L004a49ed:
            test edi, edi
            jge L004a49fb
            push 0x69
            call FatalError
            add esp, 4
    L004a49fb:
            call PDGetTotalTime
            mov edx, dword ptr [gNumber_of_net_players]
            mov dword ptr [gNet_players+0x14], eax
            mov eax, dword ptr [gStart_race_sent]
            mov dword ptr [gCurrent_race+0x90], edx
            test eax, eax
            je L004a4a3b
            xor eax, eax
            test edx, edx
            jle L004a4a3b
            mov ecx, offset gNet_players+0x5c
    L004a4a25:
            mov esi, dword ptr [ecx]
            add ecx, 0xd4
            shl esi, 4
            mov dword ptr [esi + gCurrent_race+0xce8], eax
            inc eax
            cmp eax, edx
            jl L004a4a25
    L004a4a3b:
            pop edi
            pop esi
            pop ebx
            ret
            nop
    }
#endif
}

/* Rebuild after the Timers_Pop stack index correction. */
