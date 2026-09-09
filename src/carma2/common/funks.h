#ifndef GUARD_FUNKS_H
#define GUARD_FUNKS_H

#include "c2_hooks.h"
#include "brender/brender.h"

// CalculateTextureBitsValue

// MapSawToTriangle

extern void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon);

// AddFunkGrooveBinding

// ControlBoundFunkGroove

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

extern void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner);

// CalcProximities

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

extern void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner);

// AddNewGroovidelic

// AddGroovidelics

extern void C2_HOOK_FASTCALL KillGroovadelic(int pOwner);

extern void C2_HOOK_FASTCALL KillFunkotronic(int pOwner);

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

// FUNCTION: CARMA2_HW 0x0047b2b0
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pFunk_index);

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index);

// AdjustFunkEnable

// ActorsGroove

#endif // GUARD_FUNKS_H
