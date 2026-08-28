#include "07-structur.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_types.h"

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"
#else
#include "19-font.h"
#include "18-graphics2.h"
#endif

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
// FUNCTION: CARMA2_HW 0x00503c50
void C2_HOOK_FASTCALL DoProgram(void) {
#ifdef CARPOCALYPSE2_MATCHING
    NOT_IMPLEMENTED();
#else
    extern int DoMainScreen(void);

    /* InitialiseProgramState (inlined by retail compiler) */
    gProgram_state.prog_status = eProg_idling;
    gProgram_state.last_slot = 0;
    gProgram_state.racing = 0;
    gProgram_state.cut_scene = 0;
    gProgram_state.saving = 0;
    gProgram_state.loading = 0;
    gProgram_state.dont_save_or_load = 0;
    gProgram_state.dont_load = 0;
    gProgram_state.mirror_on = 1;
    gProgram_state.prat_cam_on = 1;
    gProgram_state.cockpit_on = 1;
    gProgram_state.frank_or_anniness = eFrankie;

    do {
        switch (gProgram_state.prog_status) {
        case eProg_intro:
        case eProg_opening:
            gProgram_state.prog_status = eProg_idling;
            break;

        case eProg_idling:
            if (!DoMainScreen()) {
                gProgram_state.prog_status = eProg_quit;
            }
            break;

        case eProg_demo:
        case eProg_game_ongoing:
            gProgram_state.prog_status = eProg_quit;
            break;

        case eProg_game_starting:
            SplashScreenWith("LOADSCRN.PIX");
            extern void carpocalypse2_LoadingScreenHold(int pMilliseconds);
            carpocalypse2_LoadingScreenHold(4000);
            gProgram_state.prog_status = eProg_quit;
            break;

        default:
            break;
        }
    } while (gProgram_state.prog_status != eProg_quit);
#endif
}

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

