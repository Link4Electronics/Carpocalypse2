#include "racestrt.h"

#include "network.h"
#include "globvars.h"
#include "globvrpb.h"

#include "grafdata.h"

#include "netgame.h"

#include "platform.h"
#include "utility.h"

#include "carpocalypse2_types.h"

#include "c2_stdlib.h"

#include "c2_string.h"

#include "racestrt.h"

// SortGridFunction

// SortOpponents

// DoGridPosition

// ReceivedViewCar
// GLOBAL: CARMA2_HW 0x0065fb18
const char gNet_synch_host_label_prefix[] = " -";

// GLOBAL: CARMA2_HW 0x006a0c74
int gOur_starting_position;

// FUNCTION: CARMA2_HW 0x004e2b50
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

void C2_HOOK_FASTCALL DoGridPosition(void) {

    SortOpponents();
}

// FUNCTION: CARMA2_HW 0x004e2b70
int C2_HOOK_FASTCALL SortOpponents(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        if (gCurrent_race.opponent_list[i].index < 0) {
            return 3;
        }
    }

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);
    qsort(gCurrent_race.opponent_list, gCurrent_race.number_of_racers, sizeof(tOpp_spec), SortGridFunction);
    gOur_starting_position = IRandomBetween(0, gCurrent_race.number_of_racers - 1);


    for (i = gCurrent_race.number_of_racers; i > gOur_starting_position; i--) {
        gCurrent_race.opponent_list[i] = gCurrent_race.opponent_list[i - 1];
    }
    gCurrent_race.opponent_list[gOur_starting_position].index = -1;
    gCurrent_race.opponent_list[gOur_starting_position].ranking = 0;
    gCurrent_race.opponent_list[gOur_starting_position].car_spec = &gProgram_state.current_car;
    gCurrent_race.number_of_racers++;
    return 3;
}

void C2_HOOK_FASTCALL CheckPlayersAreResponding(void) {
    tU32 now;
    int i;

    now = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i == gThis_net_player_index) {
            continue;
        }
        if (now - gNet_players[i].last_heard_from_him > 60000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
    if (gNet_mode == eNet_mode_client && gLast_host_query == 0) {
        tNet_message* msg;

        msg = NetBuildGuaranteedMessage(eNet_message_chunk_type_netsynch, 0);
        NetGuaranteedSendMessageToHost(gCurrent_net_game, msg, NULL);
        gLast_host_query = now;
    }
}

// FUNCTION: CARMA2_HW 0x004e2cb0
void C2_HOOK_FASTCALL NetSynchStartDraw(void) {
    int count_ready;
    int i;
    char s[256];

    count_ready = 0;
    CheckPlayersAreResponding();
    NetPlayerStatusChanged(ePlayer_status_ready);
    for (i = 0; i < 12; i++) {
        BrPixelmapRectangleFill(gBack_screen,
            gCurrent_graf_data->start_synch_x_0,
            gCurrent_graf_data->start_synch_y_pitch * i + gCurrent_graf_data->start_synch_top,
            gCurrent_graf_data->start_synch_x_r - gCurrent_graf_data->start_synch_x_1,
            gFont_7->glyph_y,
            0);
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        strcpy(s, gNet_players[i].player_name);
        if (gNet_players[i].host) {
            strcat(s, gNet_synch_host_label_prefix);
            strcat(s, GetMiscString(0x58));
            strcat(s, "-");
        }
        if (gNet_players[i].player_status == ePlayer_status_ready) {
            count_ready++;
        }
    }
    if (gNet_mode == eNet_mode_host
            && count_ready == gNumber_of_net_players
            && gNumber_of_net_players > 1
            && (!gNo_races_yet || gNumber_of_net_players == 12)) {
        SignalToStartRace();
        gStart_race_sent = 1;
    }
}
