#ifndef GUARD_STRUCTUR_H
#define GUARD_STRUCTUR_H

#include "carpocalypse2_types.h"


#include "c2_hooks.h"

extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted3;
extern int gCredits_checkpoint[3];

// NumberOfOpponentsLeft

// RaceCompleted

// Checkpoint

// IncrementCheckpoint

// IncrementLap

// RayHitFace

// WrongCheckpoint

// CheckCheckpoints

// TotalRepair

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// ChooseOpponent

// SelectOpponents

// PickNetRace

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

extern void C2_HOOK_FASTCALL carpocalypse2_TestRenderTrack(void);

extern void C2_HOOK_FASTCALL DoProgram(void);

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

#define CARPOCALYPSE2_STRUCTUR_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"


extern int gMirror_on__structur;

extern int gLoad_last_save_game;

extern int gMoney_pre_race;

extern int gAPO_pre_race[3];

extern tRace_over_reason gRace_over_reason;

extern tU32 gLast_checkpoint_time;

extern int gLast_wrong_checkpoint;

void C2_HOOK_FASTCALL RecordAPOAndCredits(void);

int C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result);

void C2_HOOK_FASTCALL InitialiseProgramState(void);

void C2_HOOK_FASTCALL DoLogos(void);

void C2_HOOK_FASTCALL DoProgramDemo(void);

void C2_HOOK_FASTCALL DoProgOpeningAnimation(void);

void C2_HOOK_FASTCALL SwapNetCarsLoad(void);

void C2_HOOK_FASTCALL DoGame(void);

int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness);

int C2_HOOK_FASTCALL NumberOfOpponentsStillRunning(void);

void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info);

int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);

void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound);

void C2_HOOK_FASTCALL IncrementCheckpoint(void);

void C2_HOOK_FASTCALL IncrementLap(void);

void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index);

void C2_HOOK_FASTCALL CheckCheckpoints(void);

void C2_HOOK_FASTCALL TotalRepair(void);

int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);

void C2_HOOK_FASTCALL JumpTheStart(void);

void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);

void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);

void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

#endif // GUARD_STRUCTUR_H
