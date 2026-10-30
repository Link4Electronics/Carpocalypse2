#ifndef GUARD_WIN95NET_H
#define GUARD_WIN95NET_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

// PDNetShutdown

// StartLookup

// PDNetStartProducingJoinList

// PDNetEndJoinList

// PDNetGetNextJoinGame

// CreateNBPAddress

extern int C2_HOOK_FASTCALL PDNetHostGame(tNet_game_details* pDetails, char* pHost_name, void** pHost_address);

// PDNetJoinGame

// PDNetLeaveGame

// PDNetHostFinishGame

tU32 C2_HOOK_FASTCALL PDNetExtractGameID(tNet_game_details* pDetails);

tPlayer_ID C2_HOOK_FASTCALL PDNetExtractPlayerID(tNet_game_details* pDetails);

void C2_HOOK_FASTCALL PDNetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address);

// PDNetObtainSystemUserName

// PDNetSendMessageToAddress

// PDNetGetNextMessage

// SetForRecieve

// PDNetGetHeaderSize

#endif // GUARD_WIN95NET_H
