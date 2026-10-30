#ifndef C2_WIN32_NET_H
#define C2_WIN32_NET_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

#include <brender/brender.h>
#include <winsock.h>

extern char gPathNetworkIni[240];
extern int gPathNetworkIniValid;

void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr);

int C2_HOOK_FASTCALL BroadcastMessage(void);

int C2_HOOK_FASTCALL PDNetGetHeaderSize(void);

int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName);

void C2_HOOK_FASTCALL PDNetHostFinishGame(void);

void C2_HOOK_FASTCALL PDNetLeaveGame(void);

tU32 C2_HOOK_FASTCALL PDNetExtractGameID(tNet_game_details* pDetails);

tPlayer_ID C2_HOOK_FASTCALL PDNetExtractPlayerID(tNet_game_details* pDetails);

tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** gRemote_addr);

int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

int C2_HOOK_FASTCALL PDNetSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer_id);

int C2_HOOK_FASTCALL PDNetSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage);

#endif // C2_WIN32_NET_H