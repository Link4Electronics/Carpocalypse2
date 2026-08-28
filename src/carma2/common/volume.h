#ifndef GUARD_VOLUME_H
#define GUARD_VOLUME_H

#include "carpocalypse2_types.h"
#include "c2_hooks.h"

#include <stdio.h>

extern tSpecial_volume gDefault_default_water_spec_vol;

// GetDefaultSpecialVolumeForWater

// FindSpecialVolume

// MungeSpecialVolume

// ResetCarSpecialVolume

// TestAutoSpecialVolume

extern void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx);

// SaveSpecialVolumes

// FindInverseAndWorldBox

// ReadSpecialVolumes

#endif // GUARD_VOLUME_H
