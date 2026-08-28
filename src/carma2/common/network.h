#ifndef GUARD_NETWORK_H
#define GUARD_NETWORK_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern int C2_HOOK_FASTCALL NetInitialise(void);

extern int C2_HOOK_FASTCALL NetShutdown(void);

extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

// DisableNetService

// ReenableNetService

// PermitNetServiceReentrancy

// HaltNetServiceReentrancy

// NetSendHeadupToAllPlayers

// NetSendHeadupToEverybody

// NetSendHeadupToPlayer

// InitialisePlayerStati

extern void C2_HOOK_FASTCALL LeaveTempGame(void);

extern void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void);

extern void C2_HOOK_FASTCALL DoNextJoinPoll(void);

// NetStartProducingJoinList

// NetEndJoinList

// NetDisposePIDGameInfo

// NetDisposeGameDetails

extern tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void);

extern void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void);

extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

// NetSetPlayerSystemInfo

// FillInThisPlayer

// LoadCarN

// DisposeCarN

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

extern int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName);

// NetJoinGame

extern void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

// NetExtractGameID

// NetExtractPlayerID

extern int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

// NetReallySendMessageToPlayer

// NetSendMessageToPlayer

// NetSendMessageToHost

// NetSendMessageToAllPlayers

// NetGetContentsSize

// NetGetMessageSize

extern tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);

// NetBuildGuaranteedMessage

// NetGetToPlayerContentsSize

// NetGetToPlayerContents

// NetGetToHostContents

// NetStartBroadcastContents

// NetBroadcastContents

extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);

extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);

extern void C2_HOOK_FASTCALL NetFreeExcessMemory(void);

extern void C2_HOOK_FASTCALL DisposeExcessMemory(void);

extern int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage);

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

extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);

extern void C2_HOOK_FASTCALL BroadcastStatus(void);

extern void C2_HOOK_FASTCALL CheckForDisappearees(void);

extern void C2_HOOK_FASTCALL CheckForPendingStartRace(void);

extern void C2_HOOK_FASTCALL NetService(int pIn_race);

// NetFinishRace

extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

// NetGetPlayerStatus

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// NetGuaranteedSendMessageToHost

// AddToGuaranteeList

// NetGuaranteedSendMessageToPlayer

// NetGuaranteedSendMessageToAddress

extern void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);

// NetWaitForGuaranteeReplies

// NetPlayerFromID

// NetCarFromPlayerID

// NetPlayerFromCar

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket

#define CARPOCALYPSE2_NETWORK_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"


extern br_pixelmap* gDigits_pix;

extern tMin_message* gMin_messages;

extern tMid_message* gMid_messages;

extern tMax_message* gMax_messages;

extern int gNet_service_disable;

extern int gNet_storage_space_initialized;

extern int gScore_winner;

extern int gReceived_game_scores;

extern int gMessage_header_size;

extern int gIn_net_service;

extern int gOnly_receive_guarantee_replies;

extern tU32 gUINT_0074a690;

extern tU32 gUINT_0074a718;

extern tU16 gGuarantee_number;

extern int gNext_guarantee;

extern int gDont_allow_joiners;

extern tNet_message_memory* gMessage_to_free;

extern int gJoin_list_mode;

extern int gBastard_has_answered;

extern int gTime_for_next_one;

extern int gJoin_poll_index;

extern tNet_game_details* gCurrent_join_poll_game;

extern tAddToJoinListProc* gAdd_proc;

extern tU32 gLast_status_broadcast;

extern tU32 gLast_flush_message;

extern tU32 gAsk_time;

extern tPlayer_ID gLocal_net_ID;

extern tGuaranteed_message gGuarantee_list[200];

extern void C2_HOOK_FASTCALL InitNetHeadups(void);

extern void C2_HOOK_FASTCALL DisposeNetHeadups(void);

extern void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);

extern void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);

extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

extern void C2_HOOK_FASTCALL GetCheckSum(tNet_message* pMessage);

extern void C2_HOOK_FASTCALL DisableNetService(void);

extern void C2_HOOK_FASTCALL ReenableNetService(void);

extern tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(int pType, int pOption);

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);

extern void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason);

extern void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there);

extern void C2_HOOK_FASTCALL NetEndJoinList(void);

extern void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails);

extern void C2_HOOK_FASTCALL StopAllThatJoinyStuffThisInstant(void);

extern tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index);

extern int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index);

extern void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc);

extern void C2_HOOK_FASTCALL InitNetStorageSpace(void);

extern void C2_HOOK_FASTCALL DisposeNetStorageSpace(void);

extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);

extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address);

extern void C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time);

extern void C2_HOOK_FASTCALL CheckCheckSum(tNet_message* pMessage);

extern void C2_HOOK_FASTCALL StatReceivePacket(void);

extern void C2_HOOK_FASTCALL NetReallyDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, undefined4 *pArg3);

extern int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id);

extern void* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pNet_player, int pSize);

extern tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void);

extern tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider);

extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pArg3);

#endif // GUARD_NETWORK_H
