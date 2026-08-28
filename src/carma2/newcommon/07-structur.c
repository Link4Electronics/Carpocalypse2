#include "07-structur.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_types.h"
#include "brender/brender.h"
#include "28-world3.h"
#include "42-input.h"
#include <stdlib.h>

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"
#endif
#include "19-font.h"
#include "18-graphics2.h"

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

void C2_HOOK_FASTCALL carpocalypse2_TestRenderTrack(void) {
    br_bounds bounds;
    br_vector3 center;
    br_vector3 eye;
    br_vector3 up;
    br_vector3 target;
    br_vector3 forward;
    br_vector3 right;
    br_vector3 temp;
    br_scalar half_width;
    br_scalar half_height;
    br_scalar half_depth;

    const char* tn = getenv("CARMA2_TRACK");
    if (!tn) tn = "NEWCITY2.TWT";
    LoadTrack(tn, &gProgram_state.track_spec, &gCurrent_race);

    BrActorToBounds(&bounds, gTrack_actor);

    center.v[0] = (bounds.min.v[0] + bounds.max.v[0]) * 0.5f;
    center.v[1] = (bounds.min.v[1] + bounds.max.v[1]) * 0.5f;
    center.v[2] = (bounds.min.v[2] + bounds.max.v[2]) * 0.5f;

    half_width = (bounds.max.v[0] - bounds.min.v[0]) * 0.5f;
    half_height = (bounds.max.v[1] - bounds.min.v[1]) * 0.5f;
    half_depth = (bounds.max.v[2] - bounds.min.v[2]) * 0.5f;

    if (half_width <= 0.5f && half_depth <= 0.5f && half_height <= 0.5f) {
        half_width = 2000.f;
        half_height = 2000.f;
        half_depth = 2000.f;
        center.v[0] = 0.f;
        center.v[1] = 0.f;
        center.v[2] = 0.f;
    }

    eye.v[0] = center.v[0];
    eye.v[1] = center.v[1] + half_height * 2.f + 200.f;
    eye.v[2] = center.v[2] + (half_depth * 2.f + half_width * 2.f) * 0.5f + 500.f;

    up.v[0] = 0.f;
    up.v[1] = 1.f;
    up.v[2] = 0.f;

    target.v[0] = center.v[0];
    target.v[1] = center.v[1] + half_height * 0.5f;
    target.v[2] = center.v[2];

    BrVector3Sub(&forward, &eye, &target);
    BrVector3Normalise(&forward, &forward);
    BrVector3Cross(&right, &up, &forward);
    BrVector3Normalise(&right, &right);
    BrVector3Cross(&temp, &forward, &right);

    gCamera->t.t.mat.m[0][0] = right.v[0];
    gCamera->t.t.mat.m[1][0] = right.v[1];
    gCamera->t.t.mat.m[2][0] = right.v[2];
    gCamera->t.t.mat.m[0][1] = temp.v[0];
    gCamera->t.t.mat.m[1][1] = temp.v[1];
    gCamera->t.t.mat.m[2][1] = temp.v[2];
    gCamera->t.t.mat.m[0][2] = forward.v[0];
    gCamera->t.t.mat.m[1][2] = forward.v[1];
    gCamera->t.t.mat.m[2][2] = forward.v[2];
    gCamera->t.t.translate.t.v[0] = eye.v[0];
    gCamera->t.t.translate.t.v[1] = eye.v[1];
    gCamera->t.t.translate.t.v[2] = eye.v[2];

    BrZbsSceneRender(gUniverse_actor, gCamera, gBack_screen, gDepth_buffer);
    PDScreenBufferSwap(0);

    BrActorRemove(gTrack_actor);
    ClearOutStorageSpace(&gTrack_storage_space);
    BrResFree(gTrack_actor);
}

void C2_HOOK_FASTCALL carpocalypse2_RaceSkeleton(void) {
    br_bounds bounds;
    br_vector3 center;
    br_vector3 eye;
    br_vector3 up;
    br_vector3 target;
    br_vector3 forward;
    br_vector3 right;
    br_vector3 temp;
    br_scalar half_width;
    br_scalar half_height;
    br_scalar half_depth;
    extern int carpocalypse2_MenuQuitRequested(void);

    const char* tn = getenv("CARMA2_TRACK");
    if (!tn) tn = "NEWCITY2.TWT";
    LoadTrack(tn, &gProgram_state.track_spec, &gCurrent_race);

    BrActorToBounds(&bounds, gTrack_actor);

    center.v[0] = (bounds.min.v[0] + bounds.max.v[0]) * 0.5f;
    center.v[1] = (bounds.min.v[1] + bounds.max.v[1]) * 0.5f;
    center.v[2] = (bounds.min.v[2] + bounds.max.v[2]) * 0.5f;

    half_width = (bounds.max.v[0] - bounds.min.v[0]) * 0.5f;
    half_height = (bounds.max.v[1] - bounds.min.v[1]) * 0.5f;
    half_depth = (bounds.max.v[2] - bounds.min.v[2]) * 0.5f;

    if (half_width <= 0.5f && half_depth <= 0.5f && half_height <= 0.5f) {
        half_width = 2000.f;
        half_height = 2000.f;
        half_depth = 2000.f;
        center.v[0] = 0.f;
        center.v[1] = 0.f;
        center.v[2] = 0.f;
    }

    eye.v[0] = center.v[0];
    eye.v[1] = center.v[1] + half_height * 2.f + 200.f;
    eye.v[2] = center.v[2] + (half_depth * 2.f + half_width * 2.f) * 0.5f + 500.f;

    up.v[0] = 0.f;
    up.v[1] = 1.f;
    up.v[2] = 0.f;

    target.v[0] = center.v[0];
    target.v[1] = center.v[1] + half_height * 0.5f;
    target.v[2] = center.v[2];

    BrVector3Sub(&forward, &eye, &target);
    BrVector3Normalise(&forward, &forward);
    BrVector3Cross(&right, &up, &forward);
    BrVector3Normalise(&right, &right);
    BrVector3Cross(&temp, &forward, &right);

    gCamera->t.t.mat.m[0][0] = right.v[0];
    gCamera->t.t.mat.m[1][0] = right.v[1];
    gCamera->t.t.mat.m[2][0] = right.v[2];
    gCamera->t.t.mat.m[0][1] = temp.v[0];
    gCamera->t.t.mat.m[1][1] = temp.v[1];
    gCamera->t.t.mat.m[2][1] = temp.v[2];
    gCamera->t.t.mat.m[0][2] = forward.v[0];
    gCamera->t.t.mat.m[1][2] = forward.v[1];
    gCamera->t.t.mat.m[2][2] = forward.v[2];
    gCamera->t.t.translate.t.v[0] = eye.v[0];
    gCamera->t.t.translate.t.v[1] = eye.v[1];
    gCamera->t.t.translate.t.v[2] = eye.v[2];

    {
        br_actor* direct;
        br_light* light;

        direct = BrActorAllocate(BR_ACTOR_LIGHT, NULL);
        light = direct->type_data;
        light->type = BR_LIGHT_DIRECT;
        light->colour = BR_COLOUR_RGB(255, 255, 255);
        light->attenuation_c = 1.0f;
        BrMatrix34RotateX(&direct->t.t.mat, BR_ANGLE_DEG(-60));
        BrMatrix34PostRotateY(&direct->t.t.mat, BR_ANGLE_DEG(30));
        BrActorAdd(gUniverse_actor, direct);
        BrLightEnable(direct);
    }

    LoadFont(1);

    while (!carpocalypse2_MenuQuitRequested()) {
        PollKeys();
        if (PDAnyKeyDown() == 63) {
            break;
        }
        BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        BrZbsSceneRender(gUniverse_actor, gCamera, gBack_screen, gDepth_buffer);
        if (gFonts[1].images != NULL) {
            DRPixelmapText(gBack_screen, 16, 16, &gFonts[1], "HUD TEST", 0);
        }
        PDScreenBufferSwap(0);
    }

    BrActorRemove(gTrack_actor);
    ClearOutStorageSpace(&gTrack_storage_space);
    BrResFree(gTrack_actor);
}

void C2_HOOK_FASTCALL DoProgram(void) {
#ifdef CARPOCALYPSE2_MATCHING
    NOT_IMPLEMENTED();
#else
    extern int DoMainScreen(void);

    if (getenv("CARMA2_TEST_TRACK")) {
        carpocalypse2_TestRenderTrack();
        gProgram_state.prog_status = eProg_quit;
        return;
    }

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
            carpocalypse2_LoadingScreenHold(2000);
            carpocalypse2_RaceSkeleton();
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

