#include "07-structur.h"

// GLOBAL: CARMA2_HW 0x00655e48
int gHud_tinted1 = -1;

// GLOBAL: CARMA2_HW 0x00655e4c
int gHud_tinted2 = -1;

// GLOBAL: CARMA2_HW 0x00655e50
int gHud_tinted3 = -1;

// GLOBAL: CARMA2_HW 0x00762140
int gCredits_checkpoint[3];

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

// FUNCTION: CARMA2_HW 0x00503c50
void C2_HOOK_FASTCALL DoProgram(void) {
#ifdef CARPOCALYPSE2_MATCHING
    NOT_IMPLEMENTED();
#else
    /*
     * Minimal boot loop: keeps the SDL window alive and renders a test
     * pattern into the back screen each frame. A proper state machine
     * (logos → frontend → race) replaces this once those systems work.
     */
    extern void carpocalypse2_PresentFrame(void);
    extern int carpocalypse2_ShouldQuit(void);
    int frame;

    for (frame = 0; frame < 1800; frame++) {
        if (carpocalypse2_ShouldQuit()) {
            break;
        }
        carpocalypse2_PresentFrame();
    }
#endif
}

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

