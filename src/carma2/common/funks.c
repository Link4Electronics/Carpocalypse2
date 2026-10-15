#include "funks.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
#include "c2_string.h"
#include "flicplay.h"
#include "utility.h"
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
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array == NULL || gFunkotronics_array_size <= 0) {
        return;
    }
    the_funk = gFunkotronics_array;
    for (i = 0; i < gFunkotronics_array_size; i++, the_funk++) {
        PossibleService();
        if (the_funk->owner != pOwner) {
            continue;
        }
        the_funk->owner = -999;
        if (the_funk->proximity_array != NULL) {
            BrMemFree(the_funk->proximity_array);
        }
        if (the_funk->texture_animation_type == eTexture_animation_frames) {
            if (the_funk->texture_animation_data.frames_info.mode == eMove_texturebits) {
                BrMemFree(the_funk->texture_animation_data.frames_info.texture_info.data);
            }
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic) {
            BrMemFree(the_funk->texture_animation_data.flic_info.flic_data);
            EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
            BrMemFree(the_funk->material->colour_map->pixels);
            the_funk->material->colour_map->pixels = NULL;
            BrPixelmapFree(the_funk->material->colour_map);
            the_funk->material->colour_map = NULL;
        } else if (the_funk->texture_animation_type == eTexture_animation_camera) {
            int j;

            for (j = 0; j < the_funk->texture_animation_data.camera_info.count; j++) {
                BrActorRemove(the_funk->texture_animation_data.camera_info.actors[j]);
                BrActorFree(the_funk->texture_animation_data.camera_info.actors[j]);
            }
        } else if (the_funk->texture_animation_type == eTexture_animation_mirror) {
            if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                BrActorRemove(the_funk->texture_animation_data.mirror_info.actor);
                BrActorFree(the_funk->texture_animation_data.mirror_info.actor);
            }
        }
    }
}

// CalcProximities

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial


// FUNCTION: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    int i;
    tGroovidelic_spec* the_groove;

    if (gGroovidelics_array == NULL || gGroovidelics_array_size <= 0) {
        return;
    }
    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        PossibleService();
        if (the_groove->owner == pOwner) {
            the_groove->owner = -999;
        }
    }
}

// AddNewGroovidelic

// AddGroovidelics

// FUNCTION: CARMA2_HW 0x00477160
void C2_HOOK_FASTCALL KillGroovadelic(int pOwner) {
    int i;
    tGroovidelic_spec* the_groove;

    if (gGroovidelics_array == NULL) {
        return;
    }
    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->owner != pOwner) {
            continue;
        }
        if (the_groove->path_mode == eMove_controlled || the_groove->path_mode == eMove_absolute) {
            continue;
        }
        if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
            continue;
        }
        the_groove->owner = -999;
    }
}

// FUNCTION: CARMA2_HW 0x004771c0
void C2_HOOK_FASTCALL KillFunkotronic(int pOwner) {
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array == NULL) {
        return;
    }
    the_funk = gFunkotronics_array;
    for (i = 0; i < gFunkotronics_array_size; i++, the_funk++) {
        if (the_funk->owner == pOwner
            && the_funk->matrix_mode != eMove_controlled
            && the_funk->matrix_mode != eMove_absolute
            && the_funk->lighting_animation_type != eMove_controlled
            && the_funk->lighting_animation_type != eMove_absolute
            && (the_funk->texture_animation_data.frames_info.mode != eMove_controlled
                && the_funk->texture_animation_data.frames_info.mode != eMove_texturebits
                || the_funk->texture_animation_type != 0)) {
            the_funk->owner = -999;
        }
    }
}

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

// GrooveThisDelic

// GrooveThoseDelics

// StopGroovidelic

// DisableGroovidelic

// FUNCTION: CARMA2_HW 0x0047b130
void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags |= pBlock_flags;
            return;
        }
    }
}

// EnableGroovidelic

// FUNCTION: CARMA2_HW 0x0047b1a0
void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value) {
    tGroovidelic_spec* the_groove;
    br_actor* actor;

    the_groove = &gGroovidelics_array[pIndex];
    actor = the_groove->actor;
    the_groove->path_interrupt_status = pPath_interrupt_status;
    the_groove->object_interrupt_status = pObject_interrupt_status;
    the_groove->path_resumption_value = pPath_resumption_value;
    the_groove->object_resumption_value = pObject_resumption_value;
    BrMatrix34Copy(&actor->t.t.mat, pMatrix);
}

// ResetGrooveFlags

// DisableFunkotronic

// EnableFunkotronic

// MasterDisableFunkotronic

// MasterEnableFunkotronic

// FUNCTION: CARMA2_HW 0x0047b310
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        EnableFunkotronic(pIndex);
    } else {
        DisableFunkotronic(pIndex);
    }
}

// ActorsGroove