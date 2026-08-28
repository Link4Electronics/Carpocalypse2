#ifndef GUARD_NETGAME_H
#define GUARD_NETGAME_H

#include "carpocalypse2_types.h"


#include "c2_hooks.h"

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

// ReceivedCopInfo

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

extern void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);

extern void C2_HOOK_FASTCALL DoNetScores(void);

extern void C2_HOOK_FASTCALL InitNetHeadups(void);

extern void C2_HOOK_FASTCALL DisposeNetHeadups(void);

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

extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

// ReceivedGameplay

// SendGameplay

// SendGameplayToAllPlayers

// SendGameplayToHost

// InitNetGameplayStuff

extern void C2_HOOK_FASTCALL DefaultNetName(void);

// AddPlayerToShapeStatusLists

// RemovePlayerFromShapeStatusLists

#define CARPOCALYPSE2_NETGAME_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"


extern int gInitialised_grid;

extern int gIt_or_fox;

extern tNet_game_player_info* gLast_lepper;

extern tU32 gLast_it_change;

extern int gNot_shown_race_type_headup;

extern int gWinner_declared;

extern tU32 gTime_for_punishment;

extern int gINT_0068d920;

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);

void C2_HOOK_FASTCALL InitPlayers(void);

void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

void C2_HOOK_FASTCALL CalcPlayerScores(void);

void C2_HOOK_FASTCALL SendPlayerScores(void);

void C2_HOOK_FASTCALL DoNetGameManagement(void);

void C2_HOOK_FASTCALL SetUpNetCarPositions(void);

void C2_HOOK_FASTCALL SignalToStartRace(void);

void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits);

void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time);

#endif // GUARD_NETGAME_H
