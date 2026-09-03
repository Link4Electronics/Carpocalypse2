#include "funks.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
#include "c2_string.h"
#include "world.h"
// GLOBAL: CARMA2_HW 0x0068b840
br_scalar gSight_distance_squared;

// CalculateTextureBitsValue

// MapSawToTriangle

// FUNCTION: CARMA2_HW 0x00474880
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)CARPOCALYPSE2_SQR(pYon * 1.02);
}

// AddFunkGrooveBinding

// ControlBoundFunkGroove

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

// FUNCTION: CARMA2_HW 0x00474950
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tFunkotronic_spec* the_funk;

    for (i = 0; i < gFunkotronics_array_size; i++) {
        the_funk = &gFunkotronics_array[i];
        if (the_funk->owner != pOwner) {
            continue;
        }
        if (the_funk->proximity_array != NULL) {
            BrMemFree(the_funk->proximity_array);
        }
        if (the_funk->matrix_mod_data.texture_info.data != NULL) {
            BrMemFree(the_funk->matrix_mod_data.texture_info.data);
        }
        if (the_funk->lighting_animation_data.texture_info.data != NULL) {
            BrMemFree(the_funk->lighting_animation_data.texture_info.data);
        }
        if (the_funk->texture_animation_data.frames_info.texture_info.data != NULL) {
            BrMemFree(the_funk->texture_animation_data.frames_info.texture_info.data);
        }
        if (the_funk->texture_animation_data.flic_info.flic_data != NULL) {
            BrMemFree(the_funk->texture_animation_data.flic_info.flic_data);
        }
        memset(the_funk, 0, sizeof(tFunkotronic_spec));
        the_funk->owner = -999;
    }
#else
    NOT_IMPLEMENTED();
#endif
}

// CalcProximities

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

// FUNCTION: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tGroovidelic_spec* the_groove;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        the_groove = &gGroovidelics_array[i];
        if (the_groove->owner != pOwner) {
            continue;
        }
        if (the_groove->path_data.straight_info.texture_info.data != NULL) {
            BrMemFree(the_groove->path_data.straight_info.texture_info.data);
        }
        if (the_groove->path_data.circular_info.texture_info.data != NULL) {
            BrMemFree(the_groove->path_data.circular_info.texture_info.data);
        }
        if (the_groove->object_data.spin_info.texture_info.data != NULL) {
            BrMemFree(the_groove->object_data.spin_info.texture_info.data);
        }
        if (the_groove->object_data.rock_info.texture_info.data != NULL) {
            BrMemFree(the_groove->object_data.rock_info.texture_info.data);
        }
        memset(the_groove, 0, sizeof(tGroovidelic_spec));
        the_groove->owner = -999;
    }
#else
    NOT_IMPLEMENTED();
#endif
}

// AddNewGroovidelic

// AddGroovidelics

// KillGroovadelic

// KillFunkotronic

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

// GrooveThisDelic

// GrooveThoseDelics

// StopGroovidelic

// DisableGroovidelic

// EnableGroovidelic

// SetGrooveInterrupt

// ResetGrooveFlags

// DisableFunkotronic

// EnableFunkotronic

// MasterDisableFunkotronic

// MasterEnableFunkotronic

// AdjustFunkEnable

// ActorsGroove