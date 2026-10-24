#include "netgame.h"

#include "displays.h"
#include "globvars.h"
#include "globvrpb.h"
#include "network.h"
#include "structur.h"
#include "opponent.h"
#include "physics.h"
#include "platform.h"
#include "utility.h"
#include "car.h"
#include "racestrt.h"
#include "controls.h"
#include "crush.h"
#include "crush2.h"
#include "powerups.h"
#include "spark.h"
#include "racemem.h"
#include "smashing.h"
#include "errors.h"
#include "carpocalypse2_macros.h"
#include "c2_stdlib.h"

#include <brender/brender.h>
#include "netgame.h"

#include "network.h"
#include "loading3.h"
#include "globvars.h"
// GLOBAL: CARMA2_HW 0x0074a738
int gInitialised_grid;

// GLOBAL: CARMA2_HW 0x0074a734
int gIt_or_fox;

// GLOBAL: CARMA2_HW 0x0068d924
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
tU32 gLast_it_change;

// GLOBAL: CARMA2_HW 0x0068d934
int gNot_shown_race_type_headup;

// GLOBAL: CARMA2_HW 0x0068d938
int gWinner_declared;

// GLOBAL: CARMA2_HW 0x0068d92c
tU32 gTime_for_punishment;

// GLOBAL: CARMA2_HW 0x0068d920
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0075b8f8
br_pixelmap* gDigits_pix;
// FUNCTION: CARMA2_HW 0x00499260

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (!pCar->disabled) {
            pCar->disabled = 1;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] < 500.0f) {
            BrVector3Accumulate(&pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004992e0
void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (pCar->disabled) {
            pCar->disabled = 0;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] > 500.0f) {
            BrVector3Sub(&pCar->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b690
void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);
    C2_HOOK_BUG_ON(sizeof(tNet_game_options) != 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, start_race, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, options, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, type, 0x74);
    C2_HOOK_BUG_ON(sizeof(tNet_game_player_info) != 0xd4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, reposition_time, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, score2, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, credits, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, wasted, 0x6c);

    PossibleService();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_fight_to_death:
        pPlayer->score2 = 100;
        break;
    case eNet_game_type_1:
    case eNet_game_type_2:
    case eNet_game_type_4:
    case eNet_game_type_5:
    case eNet_game_type_6:
        pPlayer->score2 = 0;
        break;
    case eNet_game_type_checkpoint:
        pPlayer->score2 = 0xffff;
        break;
    case eNet_game_type_foxy:
        pPlayer->score2 = 0;
        break;
    default:
        abort();
    }
    pPlayer->credits = gCurrent_net_game->options.starting_credits;
    pPlayer->wasted = 0;
    pPlayer->reposition_time = 0;
}

// FUNCTION: CARMA2_HW 0x0049b720
void C2_HOOK_FASTCALL InitPlayers(void) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        InitialisePlayerScore(&gNet_players[i]);
    }
    if (gNet_mode == eNet_mode_host) {
        gLast_it_change = 0;
        gLast_lepper = NULL;
    }
    gTime_for_punishment = 0;
    gNot_shown_race_type_headup = 1;
    gIt_or_fox = -1;
    gWinner_declared = 0;
}

// FUNCTION: CARMA2_HW 0x0049bd00
void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {

}

// FUNCTION: CARMA2_HW 0x0049a760
void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049bc20
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* message = NetBuildGuaranteedMessage(0x1b, 0);

    *(int *)((tU8 *)message + 0x20) = pParam_1;
    *(int *)((tU8 *)message + 0x24) = pParam_2;
    *(int *)((tU8 *)message + 0x1c) = pMess;
    *(int *)((tU8 *)message + 0x28) = pParam_3;
    *(int *)((tU8 *)message + 0x2c) = pParam_4;
    SendGuaranteedMessageToPlayer(gCurrent_net_game, message, pPlayer, 0);
}

// FUNCTION: CARMA2_HW 0x0049bc70
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* message = NetBuildGuaranteedMessage(0x1b, 0);

    *(int *)((tU8 *)message + 0x24) = pParam_2;
    *(int *)((tU8 *)message + 0x1c) = pMess;
    *(int *)((tU8 *)message + 0x20) = pParam_1;
    *(int *)((tU8 *)message + 0x28) = pParam_3;
    *(int *)((tU8 *)message + 0x2c) = pParam_4;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
}

void C2_HOOK_FASTCALL PlayerIsItTag(tNet_game_player_info* pPlayer);

void C2_HOOK_FASTCALL LoseAllLocalPowerups(tCar_spec* pCar);

void C2_HOOK_FASTCALL SendCurrentPowerups(void);

void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer) {

    NOT_IMPLEMENTED();
}

// UNLABELED: CARMA2_HW 0x0049a9e0
void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    if (pPlayer - gNet_players == gIt_or_fox) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        StopCarBeingIt(gNet_players[i].car);
    }
    if (gCurrent_net_game->type == eNet_game_type_foxy) {
        pPlayer->car->power_up_levels[1] = 0;
    }
    DisposeKevStuffCar(pPlayer->car);
    gIt_or_fox = pPlayer - gNet_players;
    sprintf(s, "%s", GetMiscString(0xbc));
    NetSendHeadupToPlayer(s, pPlayer->ID);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (&gNet_players[i] != pPlayer) {
            sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(0xbb));
            NetSendHeadupToPlayer(s, gNet_players[i].ID);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049abf0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    int i;
    int j;
    int knock_out_bit;
    int cars_left;
    int car_left;
    int flags;
    int score;
    int highest;
    int next_highest;
    int lowest_score;
    tNet_game_player_info* lowest_score_player;
    int player_left;
    tCar_spec* car;
    tNet_message* message;
    tS32 time;
    char s[256];

    time = GetTotalTime();

    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        highest = 0;
        next_highest = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 > highest) {
                next_highest = highest;
                highest = gNet_players[i].score2;
            } else if (gNet_players[i].score2 > next_highest) {
                next_highest = gNet_players[i].score2;
            }
        }
        gINT_0068d920 = (next_highest + highest + gCount_smashable_race_targets - GetRuntimeVariable(1)) / 2 + 1;
        if (gINT_0068d920 > gCurrent_net_game->options.starting_target) {
            gINT_0068d920 = gCurrent_net_game->options.starting_target;
        }
        break;
    case eNet_game_type_foxy:
        if (gIt_or_fox < 0) {
            gLast_lepper = NULL;
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (i != -1 && gRace_board[i].meter == gNumber_of_net_players - 1) {
                    goto select_initial_it;
                }
            }
            do {
                i = IRandomBetween(0, gNumber_of_net_players - 1);
            } while (i == -1 && !gRace_board[i].car->knackered);
select_initial_it:
            PlayerIsIt(&gNet_players[i]);
        }
        if (gLast_lepper != NULL && gIt_or_fox >= 0) {
            br_vector3 difference;

            BrVector3Sub(&difference, &gLast_lepper->car->pos, &gRace_board[gIt_or_fox].car->pos);
            if (BrVector3LengthSquared(&difference) >= 4.0f) {
                gLast_lepper = NULL;
            }
        }
        break;
    default:
        break;
    }

    lowest_score = 9999;
    lowest_score_player = NULL;
    cars_left = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        car = gRace_board[i].car;
        if (gNet_players[i].reposition_time != 0
                && time >= (int)gNet_players[i].reposition_time
                && (!gRace_finished || gRace_over_reason == eRace_not_over_yet)) {
            gNet_players[i].wasted = 0;
            gNet_players[i].reposition_time = 0;
            if (gCurrent_net_game->type == eNet_game_type_2
                    || gCurrent_net_game->type == eNet_game_type_checkpoint
                    || gCurrent_net_game->type == eNet_game_type_4) {
                BrMatrix34Copy(&car->car_master_actor->t.t.mat, (br_matrix34*)gRace_board[i].field_0x04);
            } else {
                SetInitialPosition(&gCurrent_race, gNet_players[i].opponent_list_index, gNet_players[i].grid_index);
            }
            StopCarSmokingInstantly(car);
            LoseAllLocalPowerups(car);
            InitialiseCar(car);
            TotallyRepairACar(car);
            if (car->driver == eDriver_local_human) {
                gTime_for_punishment = PDGetTotalTime() + 2000;
                EmptyInventory();
            }
            SetFlipUpCar(car);
            car->collision_info->field_0x49c = GetRaceTime() + 100;
            gINT_0074c6d4 = 0;
            gINT_0074b780 = 1;
            message = NetBuildGuaranteedMessage(0x32, 0);
            *(int *)((tU8 *)message + 0x1c) = gProgram_state.racing;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            ((tU8 *)message)[0x24] = (tU8)i;
            CompressMatrix34((tCompressed_matrix3 *)((tU8 *)message + 0x26), (int *)((tU8 *)message + 0x20),
                &gCurrent_race.opponent_list[gNet_players[i].opponent_list_index].car_spec->car_master_actor->t.t.mat);
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
            if (gProgram_state.racing) {
                SendCurrentPowerups();
            }
        }
        if (gNet_players[i].last_waste_message != 0
                && !gNet_players[i].wasteage_attributed
                && time - gNet_players[i].last_waste_message > 500) {
            sprintf(s, "%s %s", gNet_players[i].player_name, GetMiscString(0xac));
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            if (gCurrent_net_game->type == eNet_game_type_1) {
                gNet_players[i].score2--;
            }
            NetSendHeadupToEverybody(s);
        }
        SetKnackeredFlag(car);
        if (car->knackered && !gNet_players[i].wasted) {
            gNet_players[i].wasted = 1;
            message = NetBuildGuaranteedMessage(0x19, 0);
            *(int *)((tU8 *)message + 0x20) = -1;
            *(int *)((tU8 *)message + 0x1c) = gNet_players[i].ID;
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
            switch (gCurrent_net_game->type) {
            case eNet_game_type_fight_to_death:
                cars_left = 0;
                for (j = 0; j < gNumber_of_net_players; j++) {
                    if (!gNet_players[j].wasted) {
                        cars_left++;
                        car_left = j;
                    }
                }
                gNet_players[i].score += (&gScore_winner)[-cars_left];
                if (cars_left == 1) {
                    DeclareWinner(car_left);
                } else if (cars_left <= 0) {
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        gNet_players[j].field_0x7c++;
                        message = NetBuildGuaranteedMessage(0xa, 0);
                        *(int *)((tU8 *)message + 0x1c) = 9;
                        SendGuaranteedMessageToPlayer(gCurrent_net_game, message, gNet_players[j].ID, 0);
                    }
                }
                break;
            case eNet_game_type_1:
            case eNet_game_type_4:
            case eNet_game_type_5:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_2:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                break;
            case eNet_game_type_checkpoint:
                if ((gNet_players[i].score2 >> 16) != gCurrent_race.check_point_count) {
                    knock_out_bit = IRandomBetween(0, gCurrent_race.check_point_count - 1);
                    while ((gNet_players[i].score2 & (1 << knock_out_bit)) != 0) {
                        knock_out_bit++;
                        if (knock_out_bit >= gCurrent_race.check_point_count) {
                            knock_out_bit = 0;
                        }
                    }
                    gNet_players[i].score2 |= 1 << knock_out_bit;
                }
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_6:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                if (gNet_players[i].field_0x80 == 0) {
                    PlayerIsItTag(&gNet_players[i]);
                }
                break;
            case eNet_game_type_foxy:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                if (gNumber_of_net_players > 1 && gIt_or_fox == i) {
                    gLast_lepper = NULL;
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        if (j != i && gRace_board[j].meter == gNumber_of_net_players - 1) {
                            goto select_wasted_it;
                        }
                    }
                    do {
                        j = IRandomBetween(0, gNumber_of_net_players - 1);
                    } while (j == i && !gRace_board[j].car->knackered);
select_wasted_it:
                    PlayerIsIt(&gNet_players[j]);
                }
                break;
            }
        }
        switch (gCurrent_net_game->type) {
        case eNet_game_type_fight_to_death:
            if (car->knackered) {
                gNet_players[i].score2 = -1000000;
            } else {
                gNet_players[i].score2 = GetAverageCrushLevel(car);
            }
            break;
        case eNet_game_type_2:
            if (gNet_players[i].score2 >= gINT_0068d920 && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_checkpoint:
            score = 0;
            gNet_players[i].score2 = gNet_players[i].score2 & 0xffff;
            flags = gNet_players[i].score2;
            for (j = 0; j < gCurrent_race.check_point_count; j++) {
                if ((flags & 1) != 0) {
                    score++;
                }
                flags >>= 1;
            }
            gNet_players[i].score2 |= score << 16;
            if (!score && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_4:
            if (gNet_players[i].score2 >= 0) {
                if (gNet_players[i].score2 < lowest_score) {
                    lowest_score = gNet_players[i].score2;
                    lowest_score_player = &gNet_players[i];
                } else if (gNet_players[i].score2 == lowest_score) {
                    lowest_score_player = NULL;
                }
            }
            break;
        case eNet_game_type_foxy:
            if (i == gIt_or_fox && !gCountdown && !gRace_finished) {
                gNet_players[i].score2 += gFrame_period;
                if (gNet_players[i].score2 >= gCurrent_net_game->options.starting_target) {
                    DeclareWinner(i);
                }
            }
            break;
        }
    }
    if (gCurrent_net_game->type == eNet_game_type_4
            && lowest_score_player != NULL
            && lowest_score_player->score2 >= 0) {
        player_left = -1;
        cars_left = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 >= 0 && &gNet_players[i] != lowest_score_player) {
                cars_left++;
                if (player_left == -1) {
                    player_left = i;
                } else {
                    player_left = -2;
                }
            }
        }
        if (cars_left) {
            lowest_score_player->car->knackered = 1;
            lowest_score_player->wasted = 1;
            lowest_score_player->score2 += (&gScore_winner)[-cars_left];
            lowest_score_player->score2 = -1;
            if (player_left == -1) {
                for (i = 0; i < gNumber_of_net_players; i++) {
                    gNet_players[i].field_0x7c++;
                    message = NetBuildGuaranteedMessage(0xa, 0);
                    *(int *)((tU8 *)message + 0x1c) = 9;
                    SendGuaranteedMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID, 0);
                }
            } else if (player_left >= 0) {
                DeclareWinner(player_left);
            } else {
                SendGameplay(lowest_score_player->ID, (tNet_gameplay_mess)2, 0, 0, 0, 0);
                sprintf(s, "%s %s", lowest_score_player->player_name, GetMiscString(0xb8));
                NetSendHeadupToEverybody(s);
            }
        }
    }
}

// UNLABELED: CARMA2_HW 0x0049b540
void C2_HOOK_FASTCALL PlayerIsItTag(tNet_game_player_info* pPlayer) {
    tCar_spec* car;
    int i;
    int count;
    int winner;
    char s[256];

    if (pPlayer->field_0x80 != 0) {
        return;
    }
    car = pPlayer->car;
    DisposeKevStuffCar(car);
    car->invulnerable_no_crushage = 1;
    car->invulnerable_no_damage = 1;
    car->invulnerable_no_wastage = 1;
    pPlayer->field_0x80 = 1;
    car->power_up_levels[0] = car->power_up_slots[0];
    car->power_up_levels[1] = car->power_up_slots[1];
    if (gNet_mode == eNet_mode_host) {
        sprintf(s, "%s", GetMiscString(0xba));
        NetSendHeadupToPlayer(s, pPlayer->ID);
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (&gNet_players[i] != pPlayer) {
                sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(0xb9));
                NetSendHeadupToPlayer(s, gNet_players[i].ID);
            }
        }
    }
    pPlayer->score2 = -1 - GetTotalTime();
    if (gProgram_state.racing && gNet_mode == eNet_mode_host) {
        count = 0;
        winner = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].field_0x80 == 0) {
                count++;
                winner = i;
            }
        }
        if (count <= 1) {
            DeclareWinner(winner);
        }
    }
}

void C2_HOOK_FASTCALL LoseAllLocalPowerups(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SendCurrentPowerups(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SendPlayerScores(void) {
    tNet_message_chunk* the_contents;
    int i;

    the_contents = NetStartBroadcastContents(24, 0);
    if (the_contents == NULL) {
        return;
    }
    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        the_contents->scores.general_score = gINT_0068d920;
        break;
    case eNet_game_type_6:
        the_contents->scores.general_score = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].field_0x80) {
                the_contents->scores.general_score |= 1 << i;
            }
        }
        break;
    case eNet_game_type_foxy:
        the_contents->scores.general_score = gNet_players[gIt_or_fox].ID;
        break;
    default:
        break;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        the_contents->scores.scores[i] = gNet_players[i].score2;
    }
    NetBroadcastContents(the_contents);
}

// FUNCTION: CARMA2_HW 0x0049ab00
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    static tU32 last_playerscores_sent = -1;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            SendPlayerScores();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00499000
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    int i;
    int j;
    int k;
    int grid_index;
    int racer_count;

    DisableNetService();
    if (!gInitialised_grid) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gNet_players[i].grid_position_set = 0;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
        }
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        gCurrent_race.opponent_list[i].index = -1;
        gCurrent_race.opponent_list[i].ranking = IRandomBetween(0, 99);
        gCurrent_race.opponent_list[i].car_spec = gNet_players[i].car;
        gCurrent_race.opponent_list[i].net_player_index = i;
        gNet_players[i].opponent_list_index = i;
    }
    racer_count = gNumber_of_net_players;
    if (!gInitialised_grid && gCurrent_net_game->options.grid_start) {
        qsort(gCurrent_race.opponent_list, gNumber_of_net_players, sizeof(tOpp_spec), SortGridFunction);
    }
    gCurrent_race.number_of_racers = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[gCurrent_race.opponent_list[i].net_player_index].opponent_list_index = i;
    }
    for (i = 0; i < racer_count; i++) {
        if ((gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver == eDriver_oppo
                && !gInitialised_grid)
            || (gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver >= eDriver_net_human
                && !gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set)) {
            grid_index = -1;
            for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(gNet_players) && grid_index < 0; j++) {
                grid_index = j;
                for (k = 0; k < racer_count; k++) {
                    if (k != i
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_position_set
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_index == j) {
                        grid_index = -1;
                        break;
                    }
                }
            }
            if (grid_index < 0) {
                FatalError(kFatalError_NetworkCodeSelfCheck);
            }
            SetInitialPosition(&gCurrent_race, i, grid_index);
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_index = grid_index;
            if (!gInitialised_grid) {
                gCurrent_race.number_of_racers = i + 1;
            } else {
                InitialiseCar2(gCurrent_race.opponent_list[i].car_spec, 0);
            }
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set = 1;
        }
    }
    gCurrent_race.number_of_racers = racer_count;
    gInitialised_grid = 1;
    ReenableNetService();
}

// FUNCTION: CARMA2_HW 0x00498e70
void C2_HOOK_FASTCALL SignalToStartRace(void) {
    tNet_message* message;
    tU8* dst;
    int i;
    int j;
    int inactive;
    tCar_spec* car;
    int player_idx;
    int next_car;

    gCurrent_net_game->no_races_yet = 0;
    gINT_0074c6d4 = 0;
    gINT_0074b780 = 1;
    message = NetBuildGuaranteedMessage(0xc, 0);
    ((tU8*)message)[0x1b] = (tU8)gProgram_state.racing;
    ((tU8*)message)[0x1a] = (tU8)gCurrent_race.number_of_racers;
    dst = (tU8*)message + 0x1e;
    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        car = gCurrent_race.opponent_list[i].car_spec;
        CompressMatrix34((tCompressed_matrix3*)(dst + 2), &inactive, &car->car_master_actor->t.t.mat);
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (gRace_board[j].car == car) {
                break;
            }
        }
        if (j < gNumber_of_net_players) {
            dst[0] = (tU8)j;
            if (inactive) {
                dst[0] |= 0x80;
            }
        }
        player_idx = dst[0] & 0x7f;
        if (gCurrent_net_game->options.car_choice == 2 || gCurrent_net_game->options.car_choice == 1) {
            next_car = gNet_players[player_idx].next_car_index;
            if (next_car < 0) {
                next_car = 0;
                gNet_players[player_idx].next_car_index = next_car;
            }
            gCar_details[next_car].ownership = 1;
            dst[1] = (tU8)gNet_players[player_idx].next_car_index;
        } else {
            dst[1] = (tU8)gNet_players[player_idx].next_car_index;
        }
        dst += 0x14;
    }
    if (gPending_race < 0) {
        gPending_race = PickNetRace(gProgram_state.current_race_index, gCurrent_net_game->options.race_sequence_type);
    }
    ((tU8*)message)[0x1c] = (tU8)gPending_race;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
}

// FUNCTION: CARMA2_HW 0x00499360
void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits) {
    char s[256];
    char s2[256];
    static tU32 last_flash;
    static int flash_state;

    if (gNot_shown_race_type_headup) {
        gNot_shown_race_type_headup = 0;
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(gCurrent_net_game->type + 59));
    }
    if (gTime_for_punishment != 0 && PDGetTotalTime() >= gTime_for_punishment) {
        gTime_for_punishment = 0;
        switch (gCurrent_net_game->type) {
        case 2:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(219));
            break;
        case 3:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(220));
            break;
        case 4:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(221));
            break;
        case 7:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(222));
            break;
        default:
            break;
        }
    }
    if (gNet_mode == eNet_mode_none
        || (float)gProgram_state.credits >= gRecovery_cost.initial_network[gCurrent_net_game->type]
        || Flash(200, &last_flash, &flash_state)) {
        sprintf(s, "\xf8%d\xfa %s", pCredits, GetMiscString(94));
        ChangeHeadupText(gNet_cash_headup, s);
    } else {
        ChangeHeadupText(gNet_cash_headup, "");
    }
    switch (gCurrent_net_game->type) {
    case 1:
        sprintf(s, "\xf8%d\xfa %s", gCurrent_net_game->options.starting_target, GetMiscString(241));
        break;
    case 2:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(180), gINT_0068d920);
        break;
    case 5:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(181), gCurrent_net_game->options.starting_target);
        break;
    case 7:
        TimerString(gCurrent_net_game->options.starting_target, s2, 1, 1, 0);
        sprintf(s, "%s \xf8%s\xfa", GetMiscString(182), s2);
        break;
    default:
        s[0] = '\0';
        break;
    }
    ChangeHeadupText(gNet_ped_headup, s);
    switch (gHeadup_detail_level) {
    case 0:
    case 3:
        MoveHeadupTo(gNet_cash_headup, 0x50, 0x19);
        MoveHeadupTo(gNet_ped_headup, 0x50, 0);
        break;
    case 1:
    case 4:
        MoveHeadupTo(gNet_cash_headup, 0x1e0, 0x19);
        MoveHeadupTo(gNet_ped_headup, 0x1e0, 0);
        break;
    case 2:
    case 5:
        MoveHeadupTo(gNet_cash_headup, 0x1ae, 0x3c);
        MoveHeadupTo(gNet_ped_headup, 0x1ae, 0x14);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00497900
void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time) {

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    NOT_IMPLEMENTED();
}
// SendShapeNumbers

// ClearShapeStatusFlag

// SendNonCar

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// CarShapeNeedsSendingToPlayer

// AddShapeStuffToMechanicsMessage

// TimeToSendData

// SendCarData

// ReceivedRecover

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// FUNCTION: CARMA2_HW 0x00498bb0
void C2_HOOK_FASTCALL ReceivedCopInfo(void) {
}

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

// SignalToStartRace

// SignalToRepositionCar

// SetUpNetCarPositions

// ReinitialiseCar

// RepositionPlayer

// DisableCar

// EnableCar

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

// CreateBoxes

// DrawScoreBoxes

// FUNCTION: CARMA2_HW 0x004999f0
void C2_HOOK_FASTCALL DoNetScores(void) {

    DoNetScores2(0);
}

// FUNCTION: CARMA2_HW 0x0049a710
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

// EverybodysLost

// DeclareWinner

// PlayerIsIt

// CheckForVampireWinner

// PlayerIsInfected

// FarEnoughAway

// CarInContactWithFox

// SelectRandomFox

// CalcPlayerScores

// SendPlayerScores

// DoNetGameManagement

// InitialisePlayerScore

// InitPlayers

// UseGeneralScore

// NetSendEnvironmentChanges

// FUNCTION: CARMA2_HW 0x0049b9a0
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && gNet_players[i].player_status >= ePlayer_status_ready
                && gNet_players[i].player_status < ePlayer_status_racing) {
            SendCarData(PDGetTotalTime() + 1000);
        }
    }
#else
    int i;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && gNet_players[i].player_status >= ePlayer_status_ready
                && gNet_players[i].player_status < ePlayer_status_racing) {
            SendCarData(PDGetTotalTime() + 1000);
        }
    }
#endif
}

// ReceivedGameplay

// SendGameplay

// SendGameplayToAllPlayers

// SendGameplayToHost

// InitNetGameplayStuff

// FUNCTION: CARMA2_HW 0x0049bd10
void C2_HOOK_FASTCALL DefaultNetName(void) {

    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// AddPlayerToShapeStatusLists

// RemovePlayerFromShapeStatusLists
