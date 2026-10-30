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
extern void C2_HOOK_FASTCALL Darken(void);
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

// GLOBAL: CARMA2_HW 0x00690230
tAddToJoinListProc* gAdd_proc;

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

// GLOBAL: CARMA2_HW 0x00762430
int gINT_00762430;

// GLOBAL: CARMA2_HW 0x00763088
int gINT_00763088[48];

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

tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider);
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);

// FUNCTION: CARMA2_HW 0x0049d780

void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage) {
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
}

// FUNCTION: CARMA2_HW 0x0049d430
void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {
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
            SendGuaranteedMessageToPlayer(gCurrent_net_game, message, pPlayer_id, 0);
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

// FUNCTION: CARMA2_HW 0x0049f860
tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(tU8 pType, int pOption) {
    int i;
    int chunk_size;
    int offset;
    tNet_message* message;
    tNet_message_chunk* chunk;
    tNet_game_player_info* player;
    tPlayer_ID key;

    key = gNet_players[0].ID;
    chunk_size = NetGetContentsSize(pType, pOption);
    i = 0;
    if (gNumber_of_net_players <= 0) {
        return NULL;
    }
    for (; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == key) {
            goto found_player;
        }
    }
    return NULL;

found_player:
    player = &gNet_players[i];
    if (player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = player->field_0xcc;
    if (message != NULL && message->header.field_0x16 + chunk_size > 0x200) {
        tPlayer_ID player_id = player->ID;
        tNet_game_details* game = gCurrent_net_game;

        if (gNet_mode != eNet_mode_none) {
            message->header.field_0x8 = gLocal_net_ID;
            message->header.field_0x10 = PDGetTotalTime();
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == player_id) {
                    goto send_message;
                }
            }
            goto clear_message;

send_message:
            PDNetSendMessageToAddress(game, message, &gNet_players[i]);
        }
clear_message:
        player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        message = NetAllocateMessage(0x200);
        player->field_0xcc = message;
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    chunk = (tNet_message_chunk*)((tU8*)message + offset);
    message->header.field_0x16 += (tU16)chunk_size;
    message->header.field_0x14 += 1;
    chunk->raw.header.type = pType;
    chunk->raw.header.size = (tU8)chunk_size;
    return chunk;
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
void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason) {
    int i;
    int size_a;
    int size_b;
    int size_c;
    int total_size;
    int fill;
    int result;
    tNet_message* message;
    tNet_game_details* game;

    gINT_0074c6d4 = 0;
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
            result |= SendGuaranteedMessageToPlayer(game, message, gNet_players[i].ID, 0);
        }
    }
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
    Darken();
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
void C2_HOOK_FASTCALL NetEndJoinList(void) {

    if (gJoin_list_mode) {
        gJoin_list_mode = 0;
        DisposeCurrentJoinPollGame();
        LeaveTempGame();
        PDNetEndJoinList();
    }
}

// FUNCTION: CARMA2_HW 0x0049dc80
void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

// FUNCTION: CARMA2_HW 0x0049e210
tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049e970
int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index) {

    NOT_IMPLEMENTED();
    return 0;
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
tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {
    int size_a;
    int size_b;
    int size_c;
    int total_size;
    tNet_message* message;

    size_a = NetGetContentsSize(pNet_message_type, pOption);
    size_b = NetGetContentsSize(5, 0);
    size_c = NetGetContentsSize(pNet_message_type, pOption);
    total_size = size_c + size_b + sizeof(tNet_message_header);
    message = NetAllocateMessage(total_size);
    if (message != NULL) {
        size_a &= 0xff;
        message->header.field_0x14 = 0xff;
        message->header.field_0x16 = (tU16)total_size;
        message->contents.raw.header.size = (tU8)size_a;
        message->contents.raw.header.type = pNet_message_type;
        ((tU8*)message)[0x19 + size_a] = 5;
        ((tU8*)message)[0x18 + message->contents.raw.header.size] = (tU8)size_b;
    }
    return message;
}

// FUNCTION: CARMA2_HW 0x004a5c80
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {

    SendGuaranteedMessageToPlayer(pDetails, pMessage, gNet_players[0].ID, (undefined4)pNotifyFail);
}

// FUNCTION: CARMA2_HW 0x0049ff70
tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address) {
    tNet_message* message;

    Timers_Push(TIMER_LLR);
    message = PDNetGetNextMessage(pDetails, pSender_address);
    Timers_Pop(TIMER_LLR);
    return message;
}

// FUNCTION: CARMA2_HW 0x004a0260
void C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time) {

    NOT_IMPLEMENTED();
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
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, undefined4 *pArg3) {
    int i;
    int result;

    if (gNumber_of_net_players == 1) {
        pMessage->contents.raw.header.type = 0xff;
        return 0;
    }
    result = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            result |= SendGuaranteedMessageToPlayer(pDetails, pMessage, gNet_players[i].ID, (undefined4)pArg3);
        }
    }
    return result;
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
void* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pNet_player, int pSize) {
    tNet_message* message;
    int offset;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);

    if (pNet_player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = pNet_player->field_0xcc;
    if (pNet_player->field_0xcc != NULL && pNet_player->field_0xcc->header.field_0x16 + pSize > 512) {
        NetReallySendMessageToPlayer(gCurrent_net_game, pNet_player->field_0xcc, pNet_player->ID);
        pNet_player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        pNet_player->field_0xcc = message = NetAllocateMessage(512);
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    message->header.field_0x16 += (tU16)pSize;
    message->header.field_0x14 += 1;
    return (tU8*)message + offset;
}

// FUNCTION: CARMA2_HW 0x004a5b30
tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void) {

    return gNet_players[gThis_net_player_index].player_status;
}

// FUNCTION: CARMA2_HW 0x0049f9c0
tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider) {
    int i;
    int size;
    int offset;
    tNet_message* message;
    tNet_message_chunk* chunk;
    tNet_game_player_info* player;
    tPlayer_ID key;

    if (gINT_00659d34 >= 0) {
        PDFatalError("Broadcast stacks messed up");
    }
    i = 0;
    if (gNumber_of_net_players <= 0) {
        return NULL;
    }
    for (; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            goto found_player;
        }
    }
    return NULL;

found_player:
    gINT_00659d34 = i;
    key = gNet_players[i].ID;
    size = NetGetContentsSize(pType, pSize_decider);
    if (gNumber_of_net_players <= 0) {
        return NULL;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == key) {
            goto found_target;
        }
    }
    return NULL;

found_target:
    player = &gNet_players[i];
    if (player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = player->field_0xcc;
    if (message != NULL && size + message->header.field_0x16 > 0x200) {
        tPlayer_ID player_id = player->ID;
        tNet_game_details* game = gCurrent_net_game;

        if (gNet_mode != eNet_mode_none) {
            message->header.field_0x8 = gLocal_net_ID;
            message->header.field_0x10 = PDGetTotalTime();
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == player_id) {
                    goto send_message;
                }
            }
            goto clear_message;

send_message:
            PDNetSendMessageToAddress(game, message, &gNet_players[i]);
        }
clear_message:
        player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        player->field_0xcc = message = NetAllocateMessage(0x200);
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    chunk = (tNet_message_chunk*)((tU8*)message + offset);
    message->header.field_0x16 += (tU16)size;
    message->header.field_0x14 += 1;
    chunk->raw.header.type = pType;
    chunk->raw.header.size = (tU8)size;
    return chunk;
}

// FUNCTION: CARMA2_HW 0x0049fb70
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage) {
    tNet_message* message;
    int size;
    tU8* dest;
    int i;
    int j;
    tPlayer_ID player_id;
    tNet_game_details* game;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && i != gINT_00659d34) {
            size = pMessage->raw.header.size;
            if (&gNet_players[i] == &gNet_players[gThis_net_player_index]) {
                PDFatalError("Trying to send network message to self");
            }
            message = gNet_players[i].field_0xcc;
            if (message != NULL && message->header.field_0x16 + size > 0x200) {
                player_id = gNet_players[i].ID;
                game = gCurrent_net_game;
                if (gNet_mode != eNet_mode_none) {
                    message->header.field_0x8 = gLocal_net_ID;
                    message->header.field_0x10 = PDGetTotalTime();
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        if (gNet_players[j].ID == player_id) {
                            goto send_message;
                        }
                    }
                    goto clear_message;

send_message:
                    PDNetSendMessageToAddress(game, message, &gNet_players[j]);
                }
clear_message:
                gNet_players[i].field_0xcc = message = NULL;
            }
            if (message == NULL) {
                message = NetAllocateMessage(0x200);
                gNet_players[i].field_0xcc = message;
                message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
                message->header.field_0x14 = 0;
            }
            dest = (tU8*)message;
            dest += message->header.field_0x16;
            message->header.field_0x16 = size + message->header.field_0x16;
            message->header.field_0x14 += 1;
            memcpy(dest, pMessage, pMessage->raw.header.size);
        }
    }
    gINT_00659d34 = -1;
}

// FUNCTION: CARMA2_HW 0x004a5bc0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pArg3) {
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
            result |= SendGuaranteedMessageToPlayer(pNet_game, pMessage, gNet_players[i].ID, pArg3);
        }
    }
    return result;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004a5e20
int C2_HOOK_FASTCALL SendGuaranteedMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pPlayer, undefined4 pArg4) {
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
void C2_HOOK_FASTCALL DoNextJoinPoll(void) {

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
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
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
}

// NetSetPlayerSystemInfo

// FUNCTION: CARMA2_HW 0x0049e3a0
void C2_HOOK_FASTCALL FillInThisPlayer(tNet_game_details* pGame, tNet_game_player_info* pPlayer, int pCar_index, int pHost) {
    int car_choice;

    pPlayer->host = pHost;
    pPlayer->ID = PDNetExtractPlayerID(pGame);
    strcpy(pPlayer->player_name, gProgram_state.player_name);
    pPlayer->this_players_time_stamp = PDGetTotalTime();
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

// C2PORT-NONMATCHING: best 55.83% | difftest PASS 2000 trials (spec difftest/specs/0049e450_NetPlayersChanged.json) | reason vc5-register-allocation
// FUNCTION: CARMA2_HW 0x0049e450
void C2_HOOK_FASTCALL NetPlayersChanged(int pPlayer_count, tNet_game_player_info* pPlayers) {
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
int C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
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
    int j;
    int contents_size;
    tPlayer_ID player_id;
    tNet_game_details* game;
    tNet_game_player_info* player;
    tNet_message* message;
    tU8* destination;

    contents_size = pMessage->header.field_0x16 - sizeof(tNet_message_header);
    pMessage->contents.raw.header.size = (tU8)contents_size;
    i = 0;
    player = gNet_players;
    if (0 < gNumber_of_net_players) {
        do {
            if (i != gThis_net_player_index) {
                if (&player->field_0xcc == &gNet_players[gThis_net_player_index].field_0xcc) {
                    PDFatalError("Trying to send network message to self");
                }
                message = player->field_0xcc;
                if (message != NULL && message->header.field_0x16 + contents_size > 0x200) {
                    player_id = player->ID;
                    game = gCurrent_net_game;
                    if (gNet_mode != eNet_mode_none) {
                        message->header.field_0x8 = gLocal_net_ID;
                        message->header.field_0x10 = PDGetTotalTime();
                        for (j = 0; j < gNumber_of_net_players; j++) {
                            if (gNet_players[j].ID == player_id) {
                                PDNetSendMessageToAddress(game, message, &gNet_players[j].pd_net_info);
                                break;
                            }
                        }
                    }
                    player->field_0xcc = NULL;
                    message = NULL;
                }
                if (message == NULL) {
                    message = NetAllocateMessage(0x200);
                    player->field_0xcc = message;
                    message->header.field_0x16 = sizeof(tNet_message_header);
                    message->header.field_0x14 = 0;
                }
                destination = (tU8*)message + message->header.field_0x16;
                message->header.field_0x14++;
                message->header.field_0x16 += contents_size;
                if (destination != NULL) {
                    memcpy(destination, &pMessage->contents, contents_size);
                }
            }
            i++;
            player++;
        } while (i < gNumber_of_net_players);
    }
    if (pMessage->header.field_0x14 != 0xff) {
        pMessage->contents.raw.header.type = 0xff;
    }
    return 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0049f440
int C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, int pOption) {
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
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tU8 pType, int pSize_decider, tPlayer_ID pPlayer_id) {
    int i;
    int chunk_size;
    int offset;
    tNet_message* message;
    tNet_message_chunk* chunk;
    tNet_game_player_info* player;

    chunk_size = NetGetContentsSize(pType, pSize_decider);
    i = 0;
    if (gNumber_of_net_players <= 0) {
        return NULL;
    }
    for (; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer_id) {
            goto found_player;
        }
    }
    return NULL;

found_player:
    player = &gNet_players[i];
    if (player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = player->field_0xcc;
    if (message != NULL && message->header.field_0x16 + chunk_size > 0x200) {
        tPlayer_ID player_id = player->ID;
        tNet_game_details* game = gCurrent_net_game;

        if (gNet_mode != eNet_mode_none) {
            message->header.field_0x8 = gLocal_net_ID;
            message->header.field_0x10 = PDGetTotalTime();
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == player_id) {
                    goto send_message;
                }
            }
            goto clear_message;

send_message:
            PDNetSendMessageToAddress(game, message, &gNet_players[i]);
        }
clear_message:
        player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        message = NetAllocateMessage(0x200);
        player->field_0xcc = message;
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    chunk = (tNet_message_chunk*)((tU8*)message + offset);
    message->header.field_0x16 += (tU16)chunk_size;
    message->header.field_0x14 += 1;
    chunk->raw.header.type = pType;
    chunk->raw.header.size = (tU8)chunk_size;
    return chunk;
}

// NetGetToHostContents

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
    static int rr_min;
    static int rr_mid;
    static int rr_max;
    tU8* pool;
    tU8* msg;
    int i;
    tNet_message_memory* block;
    tNet_message_memory* tail;

    pool = NULL;
    if (pSize <= (int)sizeof(tMin_message) - 4) {
        for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
            msg = (tU8*)gMin_messages + rr_min * sizeof(tMin_message) + gMessage_header_size;
            if (msg[0x19] == eNetMsg_none) {
                pool = (tU8*)gMin_messages + rr_min * sizeof(tMin_message);
                break;
            }
            rr_min++;
            if (rr_min >= MIN_MESSAGES_CAPACITY) {
                rr_min = 0;
            }
        }
    }
    if (pool == NULL && pSize <= (int)sizeof(tMid_message) - 4) {
        for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
            msg = (tU8*)gMid_messages + rr_mid * sizeof(tMid_message) + gMessage_header_size;
            if (msg[0x19] == eNetMsg_none) {
                pool = (tU8*)gMid_messages + rr_mid * sizeof(tMid_message);
                break;
            }
            rr_mid++;
            if (rr_mid >= MID_MESSAGES_CAPACITY) {
                rr_mid = 0;
            }
        }
    }
    if (pool == NULL && pSize <= (int)sizeof(tMax_message) - 4) {
        for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
            msg = (tU8*)gMax_messages + rr_max * sizeof(tMax_message) + gMessage_header_size;
            if (msg[0x19] == eNetMsg_none) {
                pool = (tU8*)gMax_messages + rr_max * sizeof(tMax_message);
                break;
            }
            rr_max++;
            if (rr_max >= MAX_MESSAGES_CAPACITY) {
                rr_max = 0;
            }
        }
    }
    if (pool == NULL) {
        block = BrMemAllocate(gMessage_header_size + pSize + sizeof(void*), kMem_dynamic_message);
        if (block == NULL) {
            return NULL;
        }
        block->next = NULL;
        if (gDynamic_messages != NULL) {
            tail = gDynamic_messages;
            while (tail->next != NULL) {
                tail = tail->next;
            }
            tail->next = block;
        } else {
            gDynamic_messages = block;
        }
        pool = (tU8*)block + sizeof(void*);
    }
    msg = pool + gMessage_header_size;
    ((tNet_message*)msg)->header.field_0xc = 1;
    ((tNet_message*)msg)->header.field_0x4 = MESSAGE_MAGIC_NUMBER;
    return (tNet_message*)msg;
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

// C2PORT-NONMATCHING: best 71.51% | difftest PASS 2000 trials (spec difftest/specs/004a4660_RemoveNetPlayer.json) | reason vc5-register-allocation
// FUNCTION: CARMA2_HW 0x004a4660
void C2_HOOK_FASTCALL RemoveNetPlayer(register tPlayer_ID pPlayer_id) {
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
                        SendGuaranteedMessageToPlayer(current_game, message, gNet_players[recipient].ID, 0);
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
            gINT_0074c6d4 = 1;
            player = &gNet_players[1];
            for (i = 1; i < gNumber_of_net_players; i++, player++) {
                player->field_0x60 = 1;
            }
        }
    }
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
void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void) {
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
}

// FUNCTION: CARMA2_HW 0x004a4ac0
void C2_HOOK_FASTCALL BroadcastStatus(void) {
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
}

// FUNCTION: CARMA2_HW 0x004a4da0
void C2_HOOK_FASTCALL CheckForDisappearees(void) {
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
}

// FUNCTION: CARMA2_HW 0x004a5240
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (gNet_mode != eNet_mode_host || gStart_race_sent || gPending_race == 0) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].player_status != ePlayer_status_ready) {
            return;
        }
    }
    SignalToStartRace();
    gStart_race_sent = 1;
#else
    int i;

    if (gNet_mode != eNet_mode_host || gStart_race_sent || gPending_race == 0) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].player_status != ePlayer_status_ready) {
            return;
        }
    }
    SignalToStartRace();
    gStart_race_sent = 1;
#endif
}

// FUNCTION: CARMA2_HW 0x004a5280
void C2_HOOK_FASTCALL NetService(int pIn_race) {
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
}

// NetFinishRace

// FUNCTION: CARMA2_HW 0x004a57e0
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
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
}

// NetGetPlayerStatus

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// NetGuaranteedSendMessageToHost

// AddToGuaranteeList
// FUNCTION: CARMA2_HW 0x004a5ca0
void C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, tPlayer_ID pPlayer, void* pNotifyFail) {
    int i;
    int n;
    tU32 time;
    tU8 size;
    int idx;

    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guaranteed message not built correctly");
    }
    if (gNext_guarantee >= 200) {
        for (i = 0; i < 10; i++) {
            gGuarantee_list[i].message->contents.raw.header.type = eNetMsg_none;
        }
        n = 10;
        if (gGuarantee_list[9].message == gGuarantee_list[10].message) {
            /* count consecutive duplicates starting at index 10 */
            {
                tU8* p = (tU8*)&gGuarantee_list[10];
                tU8* end = (tU8*)&gGuarantee_list[200];
                while (p < end) {
                    tNet_message* ca = *(tNet_message**)p;
                    tNet_message* cb = *(tNet_message**)(p + sizeof(tGuaranteed_message));
                    p += sizeof(tGuaranteed_message);
                    n++;
                    if (ca != cb) {
                        break;
                    }
                }
            }
        }
        memmove(&gGuarantee_list[0], &gGuarantee_list[n], (200 - n) * sizeof(tGuaranteed_message));
        gNext_guarantee -= n;
        if (gNext_guarantee >= 200) {
            return;
        }
    }
    size = pMessage->contents.raw.header.size;
    *(tU16*)((tU8*)pMessage + size + 0x1a) = gGuarantee_number;
    idx = gNext_guarantee;
    gGuarantee_list[idx].field_0x2c = gGuarantee_number;
    gGuarantee_number++;
    if (gGuarantee_number == 0) {
        gGuarantee_number = 1;
    }
    gGuarantee_list[idx].message = pMessage;
    time = PDGetTotalTime();
    gGuarantee_list[idx].send_time = time;
    gGuarantee_list[idx].next_resend_time = time + 100;
    gGuarantee_list[idx].resend_period = 100;
    if (pAddress == NULL) {
        gGuarantee_list[idx].field_0x30 = 0;
    } else {
        ((tU32*)gGuarantee_list[idx].pd_address)[0] = ((tU32*)pAddress)[0];
        ((tU32*)gGuarantee_list[idx].pd_address)[1] = ((tU32*)pAddress)[1];
        ((tU32*)gGuarantee_list[idx].pd_address)[2] = ((tU32*)pAddress)[2];
        *(tU16*)&gGuarantee_list[idx].pd_address[12] = *(tU16*)((tU8*)pAddress + 12);
        gGuarantee_list[idx].field_0x30 = 1;
    }
    gGuarantee_list[idx].local_id = pPlayer;
    gGuarantee_list[idx].NotifyFail = pNotifyFail;
    gGuarantee_list[idx].recieved = 0;
    gNext_guarantee = idx + 1;
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
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
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
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049a410
static void C2_HOOK_FASTCALL FUN_0049a410(void) {
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
}

#include <limits.h>

extern int gUNK_0075bc24;
extern void C2_HOOK_FASTCALL DisposeCarN(int pPlayer_index);
extern char* gCurrent_palette_pixels;
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);
extern void C2_HOOK_FASTCALL Darken(void);
extern void C2_HOOK_FASTCALL FadePaletteUp(void);
extern void C2_HOOK_FASTCALL FUN_004a48d0(void* pPayload, void* pMessage);
extern void C2_HOOK_FASTCALL FUN_0049bad0(void* pPayload, void* pMessage, int pReceive_time);
extern void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents);
extern void C2_HOOK_FASTCALL ReceivedCrush(tU8* pContents);
extern void C2_HOOK_FASTCALL FUN_00498bb0(void);
extern void C2_HOOK_FASTCALL FUN_0043c940(void* pPayload);
extern void C2_HOOK_FASTCALL FUN_0043cd60(void* pPayload);
extern void C2_HOOK_FASTCALL ReceiveMineUpdate(tNet_message_chunk_mine_explode* pChunk);

// GLOBAL: CARMA2_HW 0x0058fd6c
const char gSTRING_0058fd6c[] = "%s %d";

// GLOBAL: CARMA2_HW 0x0065a2e0
const char gSTRING_0065a2e0[] = "PLAYER";

// GLOBAL: CARMA2_HW 0x0065a2d4
const char gSTRING_0065a2d4[] = "%s %s %s %s";

// GLOBAL: CARMA2_HW 0x0065a2c8
const char gSTRING_0065a2c8[] = "%s %s %s";

// GLOBAL: CARMA2_HW 0x0065a148
const char gSTRING_0065a148[] = "Trying to send network message to self";

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
int C2_HOOK_CDECL SortNetScoresAscending(const void* pFirst, const void* pSecond) {
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
}

// FUNCTION: CARMA2_HW 0x0049a3a0
int C2_HOOK_CDECL SortNetScoresDescending(const void* pFirst, const void* pSecond) {
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
}

static void C2_HOOK_FASTCALL FUN_0049a410(void);

// FUNCTION: CARMA2_HW 0x004a64b0
int C2_HOOK_FASTCALL FUN_004a64b0(unsigned int pArg) {
    return pArg >= 10000;
}

// FUNCTION: CARMA2_HW 0x004a69f0
void C2_HOOK_FASTCALL NetworkNoOp(void) {
}

// C2PORT-NONMATCHING: best 74.56% | difftest PASS 2000 trials (spec difftest/specs/004a48d0_FUN_004a48d0.json) | reason VC5 register allocation and global-table operand layout
// FUNCTION: CARMA2_HW 0x004a48d0
void C2_HOOK_FASTCALL FUN_004a48d0(void* pPayload, void* pMessage) {
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
        gINT_00762430 = gNumber_of_net_players;
        if (gStart_race_sent != 0 && (iVar4 = 0, 0 < gNumber_of_net_players)) {
            piVar5 = (int*)&gNet_players[0].opponent_list_index;
            do {
                iVar1 = *piVar5;
                piVar5 += 0x35;
                gINT_00763088[iVar1 * 4] = iVar4;
                iVar4++;
            } while (iVar4 < iVar3);
        }
    }
}

/* Rebuild after the Timers_Pop stack index correction. */
