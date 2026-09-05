#include "aiworld.h"

#include "drmem.h"
#include "drone.h"
#include "loading.h"
#include "opponent.h"

// MakeVertexAndOffsetIt

// MakeFaceAndTextureIt

// FUNCTION: CARMA2_HW 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {
#ifndef CARPOCALYPSE2_MATCHING
    gCount_drones = 0;
    gNum_of_opponents_pursuing = 0;
    gNum_of_opponents_getting_near = 0;
    gNum_of_opponents_completing_race = 0;
#else
    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
#endif
}

// LoadAIWorldTrackInfo

// LoadAIWorldForRace

// InitialiseAIWorldForRace

// FUNCTION: CARMA2_HW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {
    DisposeOpponents();
    DisposeDronesRaceStuff();
}

// MungeAIWorld

// PointActorAlongThisBloodyVector

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere