#include "camera.h"

#include <brender/brender.h>

#include "car.h"
#include "drone.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "pedestrn.h"
#include "replay.h"

// GLOBAL: CARMA2_HW 0x007634ac
float gCamera_cockpit_hither;

// GLOBAL: CARMA2_HW 0x0079efa8
tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// GetRaceLeader

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// ViewOpponent

// FUNCTION: CARMA2_HW 0x0040e680
void C2_HOOK_FASTCALL ReallyViewOpponent(void) {
    int i;

    if (gNet_mode == eNet_mode_client) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == gCar_to_view_id) {
                gCar_to_view_id = 0;
                gCamera_yaw = 0;
                gCar_to_view = gNet_players[i].car;
                InitialiseExternalCamera();
                PositionExternalCamera(gCar_to_view, 200);
                return;
            }
        }
    }
}

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

// MungeCarMaterials

// FUNCTION: CARMA2_HW 0x0040e790
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

    gAction_replay_camera_mode = pCamPos;
    switch (pCamPos) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        gAction_replay_manual_camera_target_type = 0;
        break;
    default:
        break;
    }
    MungeCarMaterials(&gProgram_state.current_car, gAction_replay_camera_mode == kActionReplayCameraMode_Internal);
}

// ChangeCameraType

// FUNCTION: CARMA2_HW 0x0040e7f0
void C2_HOOK_FASTCALL ChangeCameraType(void) {
    tActionReplayCameraMode mode;

    if (KeyIsDown(8)) {
        mode = gAction_replay_camera_mode;
        gAction_replay_camera_mode = mode - 1;
        if (gAction_replay_camera_mode >= 9 || gAction_replay_camera_mode < 0) {
            gAction_replay_camera_mode = 8;
        }
    } else {
        mode = gAction_replay_camera_mode;
        gAction_replay_camera_mode = mode + 1;
        if (gAction_replay_camera_mode >= 9) {
            gAction_replay_camera_mode = 0;
        }
    }
    mode = gAction_replay_camera_mode;
    if ((gPed_676914 != 0 && gCamera_type_allowed_replay[mode] == 0) ||
        (gPed_676914 == 0 && gCamera_type_allowed_gameplay[mode] == 0) ||
        mode >= 9 ||
        (mode == 6 && !OKToViewDrones()) ||
        (mode == 5 && gPed_count == 0)) {
        ChangeCameraType();
    }
    if (gPed_676914 != 0) {
        gAR_camera_type = gAction_replay_camera_mode;
    } else {
        gCamera_type = gAction_replay_camera_mode;
    }
    switch (gAction_replay_camera_mode) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        gAction_replay_manual_camera_target_type = 0;
        break;
    default:
        break;
    }
    MungeCarMaterials(&gProgram_state.current_car, gAction_replay_camera_mode == kActionReplayCameraMode_Internal);
}

// FUNCTION: CARMA2_HW 0x0040e900
void C2_HOOK_FASTCALL ToggleCockpit(void) {

    if ((gCar_to_view != &gProgram_state.current_car && !gProgram_state.cockpit_on) || gMap_view == 2) {
        return;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        tActionReplayCameraMode old_camera_mode = gCameraTypeBeforeCockpit;

        if ((gAction_replay_mode && !gCamera_type_allowed_replay[gCameraTypeBeforeCockpit])
                || (!gAction_replay_mode && !gCamera_type_allowed_gameplay[gCameraTypeBeforeCockpit])
                || gCameraTypeBeforeCockpit >= 9
                || (gCameraTypeBeforeCockpit == kActionReplayCameraMode_Drone && !OKToViewDrones())
                || (old_camera_mode == kActionReplayCameraMode_Peds && !gPed_count)) {
            SetCameraType(kActionReplayCameraMode_Standard);
        } else {
            SetCameraType(gCameraTypeBeforeCockpit);
        }
    } else {
        gCameraTypeBeforeCockpit = gAction_replay_camera_mode;
        SetCameraType(kActionReplayCameraMode_Internal);
    }
}

// PositionCarMountedCamera

// PositionExternalCamera

// CameraBugFix

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

// IncidentCam

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

// SaveCameraPosition

// RestoreCameraPosition

// DrVector3RotateY

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

// PointCameraAtCar

// PointCamera

// CollideCamera2

// CollideCameraWithOtherCars

// InitialiseExternalCamera

// FUNCTION: CARMA2_HW 0x00413770
void C2_HOOK_FASTCALL FreezeCamera(void) {

    gCamera_frozen = 1;
}

// GetAverageGridPosition

// FindACar

// FindAPed

// MouseClickInActionReplay