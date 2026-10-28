#include "win95net.h"

#include "errors.h"

#include <string.h>

extern char gLocal_address[];

// PDNetShutdown

// StartLookup

// PDNetStartProducingJoinList

// PDNetEndJoinList

// PDNetGetNextJoinGame

// CreateNBPAddress

// FUNCTION: CARMA2_HW 0x0051a080
int C2_HOOK_FASTCALL PDNetHostGame(tNet_game_details* pDetails, char* pHost_name, void** pHost_address) {
    dr_dprintf("PDNetHostGame()");
    *pHost_address = &gLocal_address;
    return 1;
}

// PDNetJoinGame

// PDNetLeaveGame

// PDNetHostFinishGame

// PDNetExtractGameID

// PDNetExtractPlayerID

// PDNetObtainSystemUserName

// PDNetSendMessageToAddress

// PDNetGetNextMessage

// SetForRecieve

// FUNCTION: CARMA2_HW 0x0051a5f0
void C2_HOOK_FASTCALL PDNetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address) {
    dr_dprintf("PDNetSetPlayerSystemInfo()");
    memcpy(pPlayer->pd_net_info.addr, pSender_address, 14);
}

// PDNetGetHeaderSize