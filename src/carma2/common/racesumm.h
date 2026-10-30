#ifndef GUARD_RACESUMM_H
#define GUARD_RACESUMM_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

extern int C2_HOOK_FASTCALL AnyBuyableCars(void);

// RecordAPOAndCredits

// PickRaceWhenGameCompleted

// MungeRaceProgression

#define CARPOCALYPSE2_RACESUMM_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern int gPlayer_lookup_racesumm[14];

void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed);

#endif // GUARD_RACESUMM_H
