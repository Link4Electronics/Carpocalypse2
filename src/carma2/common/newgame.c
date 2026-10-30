#include "newgame.h"

#include "globvars.h"
#include "globvrpb.h"
#include "init.h"
#include "loading.h"
#include "network.h"
#include "structur.h"
#include "utility.h"

#include "carpocalypse2_types.h"

#include "carpocalypse2_macros.h"
#include <stdio.h>
#include "newgame.h"

#include "loading1.h"
#include "loading2.h"
#include "utility.h"
#include "packfile.h"
#include "globvars.h"

#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x0059c7e8
int gNet_grid_starts[10] = {
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
};

// GLOBAL: CARMA2_HW 0x00764e98
int gRace_index;

// GLOBAL: CARMA2_HW 0x00688760
int gLast_graph_sel;

// GLOBAL: CARMA2_HW 0x00688718
tJoinable_game gGames_to_join[6];

// GLOBAL: CARMA2_HW 0x00763960
tNet_game_options gNet_settings[9];

// FUNCTION: CARMA2_HW 0x00468140
void C2_HOOK_FASTCALL SetOptions(tNet_game_type pGame_type, tNet_game_options* pGame_options) {

    pGame_options->show_players_on_map = gNet_settings[0].show_players_on_map;
    pGame_options->enable_text_messages = gNet_settings[0].enable_text_messages;
    pGame_options->show_powerups_on_map = gNet_settings[0].show_powerups_on_map;
    pGame_options->powerup_respawn = gNet_settings[0].powerup_respawn;
    pGame_options->open_game = gNet_settings[pGame_type + 1].open_game;
    pGame_options->starting_credits = gNet_settings[0].starting_credits;
    pGame_options->grid_start = gNet_settings[pGame_type + 1].grid_start || gNet_grid_starts[pGame_type + 1];
    pGame_options->waste_to_transfer = gNet_settings[pGame_type + 1].waste_to_transfer;
    pGame_options->random_car_choice = gNet_settings[pGame_type + 1].random_car_choice;
    pGame_options->race_sequence_type = gNet_settings[0].race_sequence_type;
    pGame_options->car_choice = gNet_settings[0].car_choice;
    pGame_options->starting_target = gNet_settings[pGame_type + 1].starting_target;

}

void C2_HOOK_FASTCALL SetGameTarget(tNet_game_type* pGame_type, tNet_game_options* pGame_options) {

}

void C2_HOOK_FASTCALL ReadNetGameChoices(tNet_game_type* pGame_type, tNet_game_options* pGame_options, int* pRace_index) {

    SetOptions(*pGame_type, pGame_options);
    SetGameTarget(pGame_type, pGame_options);
    *pRace_index = PickNetRace(-1, pGame_options->race_sequence_type);
}

void C2_HOOK_FASTCALL DisposeJoinList(int pExemption) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gGames_to_join); i++) {
        if (i == pExemption) {
            continue;
        }
        if (gGames_to_join[i].game != NULL) {
            DisposeJoinableGame(i);
        }
    }
}

void C2_HOOK_FASTCALL DisposeJoinableGame(int pIndex) {

    NetDisposeGameDetails(gGames_to_join[pIndex].game);
    gGames_to_join[pIndex].game = NULL;
}

// GLOBAL: CARMA2_HW 0x0074c6cc
tNet_game_options* gChosen_net_game_options;

// FUNCTION: CARMA2_HW 0x00468d70
int C2_HOOK_FASTCALL ChooseNetCar(tNet_game_details* pNet_game, tNet_game_options* pOptions, int* pCar_index, int pIm_the_host_so_fuck_off) {

    if (!pIm_the_host_so_fuck_off) {
        return 1;
    }
    gChosen_net_game_options = pOptions;
    SetNetAvailability(pOptions);
    *pCar_index = PickARandomCar();
    if (pIm_the_host_so_fuck_off && *pCar_index >= 0) {
        gCar_details[*pCar_index].ownership = eCar_owner_someone;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004681d0
void C2_HOOK_FASTCALL AddToJoinList(tNet_game_details* pGame) {
    int i;
    int slot_to_use;
    tU32 this_game_ID;

    slot_to_use = -1;
    this_game_ID = NetExtractGameID(pGame);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gGames_to_join); i++) {
        if (gGames_to_join[i].game != NULL) {
            if (NetExtractGameID(gGames_to_join[i].game) == this_game_ID) {
                DisposeJoinableGame(i);
                slot_to_use = i;
                break;
            }
        }
    }
    if (slot_to_use < 0) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gGames_to_join); i++) {
            if (gGames_to_join[i].game == NULL) {
                slot_to_use = i;
                break;
            }
        }
    }
    if (slot_to_use >= 0) {
        gGames_to_join[slot_to_use].game = pGame;
        gGames_to_join[slot_to_use].time = PDGetTotalTime();
    }
}

void C2_HOOK_FASTCALL SetUpOtherNetThings(tNet_game_details* pGame_details) {

    gProgram_state.skill_level = 1;
    InitGame(pGame_details->start_race);
}

void C2_HOOK_FASTCALL InitGamesToJoin(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gGames_to_join); i++) {

        gGames_to_join[i].game = NULL;
    }
}
// SetGameTarget

// SetOptions

// ReadNetGameChoices

// FUNCTION: CARMA2_HW 0x004662c0
int C2_HOOK_FASTCALL PickARandomCar(void) {
    int i;
    int cars[2 * CARPOCALYPSE2_ASIZE(gCar_details)];
    int array_size;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(cars) != 120);

    array_size = 0;
    for (i = 0; i < gNumber_of_racers; i++) {
        if (gCar_details[i].ownership == eCar_owner_none) {
            cars[array_size] = i;
            array_size++;
            if (gOpponents[i].network_availability == eNet_avail_all) {
                cars[array_size] = i;
                array_size++;
            }
        }
    }
    return cars[IRandomBetween(0, array_size - 1)];
}

// FUNCTION: CARMA2_HW 0x00466330
void C2_HOOK_FASTCALL PollCarDetails(tNet_game_details* pNet_game) {
    tNet_message* message;

    if (gReceived_car_details) {
        gReceived_car_details = 0;
        if (gNet_mode == eNet_mode_none) {
            gNet_mode = eNet_mode_thinking_about_it;
        }
        message = NetBuildGuaranteedMessage(6, 0);
        NetGuaranteedSendMessageToHost(pNet_game, message, NULL);
    }
}

// RequestCarDetails

// FUNCTION: CARMA2_HW 0x00466370
void C2_HOOK_FASTCALL SetNetAvailability(tNet_game_options* pOptions) {
    int i;

    for (i = 0; i < gNumber_of_racers; i++) {
        switch (gOpponents[i].network_availability) {
        case eNet_avail_never:
            gCar_details[i].ownership = eCar_owner_not_allowed;
            break;
        case eNet_avail_eagle:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_eagle) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        case eNet_avail_all:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_novelty) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004663e0
void C2_HOOK_FASTCALL DefaultNetSettings(void) {
    FILE* file;
    tNet_game_options* pOptions;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "NETDEFLT.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        return;
    }
    ReadNetworkSettings(file, &gNet_settings[0]);
    PFrewind(file);
    for (i = 1; i < (int)CARPOCALYPSE2_ASIZE(gNet_settings); i++) {
        pOptions = &gNet_settings[i];
        ReadNetworkSettings(file, pOptions);
    }
    PFfclose(file);
}

// DisposeJoinableGame

// InitGamesToJoin

// DisposeJoinList

// AddToJoinList

// FillInRaceDescription