#ifndef GUARD_FUNKS_H
#define GUARD_FUNKS_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "carpocalypse2_types.h"

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

extern void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

// EnableGroovidelic

extern void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value);

// ResetGrooveFlags

// DisableFunkotronic

// EnableFunkotronic

// FUNCTION: CARMA2_HW 0x0047b2b0
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pFunk_index);

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index);

extern void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags);

// ActorsGroove

#endif // GUARD_FUNKS_H
