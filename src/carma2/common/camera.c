#include "camera.h"

#include <brender/brender.h>

#include "car.h"
#include "drone.h"
#include "globvars.h"
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

// ReallyViewOpponent

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

// MungeCarMaterials

// SetCameraType

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
    if ((gAction_replay_mode != 0 && gCamera_type_allowed_replay[mode] == 0) ||
        (gAction_replay_mode == 0 && gCamera_type_allowed_gameplay[mode] == 0) ||
        mode >= 9 ||
        (mode == 6 && !OKToViewDrones()) ||
        (mode == 5 && gPed_count == 0)) {
        ChangeCameraType();
    }
    if (gAction_replay_mode != 0) {
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

// ToggleCockpit

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

// FreezeCamera

// GetAverageGridPosition

// FindACar

// FindAPed

// MouseClickInActionReplay