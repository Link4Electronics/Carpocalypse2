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

// PDNetHostGame

// PDNetJoinGame

// PDNetLeaveGame

// PDNetHostFinishGame

tU32 C2_HOOK_FASTCALL PDNetExtractGameID(tNet_game_details* pDetails);

tPlayer_ID C2_HOOK_FASTCALL PDNetExtractPlayerID(tNet_game_details* pDetails);

// PDNetObtainSystemUserName

// PDNetSendMessageToAddress

// PDNetGetNextMessage

// SetForRecieve

// PDNetSetPlayerSystemInfo

// PDNetGetHeaderSize

#endif // GUARD_WIN95NET_H
