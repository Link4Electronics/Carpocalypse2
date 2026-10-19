#ifndef GUARD_SMASH_H
#define GUARD_SMASH_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern tExplosion gExplosions[50];

extern void C2_HOOK_FASTCALL InitExplosions(void);

// ResetExplosions

// RemoveAllExplosionettes

// MungeExplosions

// CopyExplosionData

// DoExplosionette

// Explode

extern void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec);

// CalcRandomPointBetween

// ApplyInitialMovement

// ApplyTimeSpec

// FindShrapnelFragment

// SetShrapnelFragment

// ShatterObject

// MungeInternalCarGlass

// ActuallySmashPart

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// SetInitPosition

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

// DoSpillage

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

// MungeDelayedSideEffects

// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

// DoPowerupRespawnSmash

// DoConnotationsMultiLevel

// SmashItIntoVerySmallPiecesIndeed

// InitSmashQueue

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// ReadShrapnelMovementSpec

// ReadShrapnelInitialPosition

// ReadShrapnelTimeInfo

// NoteChildren

// ReadSmokeFireChance

// LoadActorFromRace

// ReadShrapnelSpec

// ReadSpillData

// ReadExplosion

// FreeExplosion

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

// ReadSmashLevels

// ReadSmashableData

// ActuallyRepairSmash

// KillFragment

extern void C2_HOOK_FASTCALL AddRepairAnimationToQueue(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable);

extern void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data);

// TotallyRepairSmash

// InitGlassFragments

// InitDecals

// RemoveAllDecals

// ReadPowerupSmash

extern void C2_HOOK_FASTCALL InitSmashing(void);

// VerySimpleRotate

// DoFragMovement

// MungeGlassFragments2

// MungeAnimationRepairs

// MungeGlassFragments

// KillAllGlassFragments

extern void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

// DisposeSmashData
void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data);

// DoBackwardShrapnel

// InitRepairAnimations

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// DisposeSmashEnvNonCars

// CloneMemory

// CloneEnvSmash

// FreeConnotations

// FreeEnvSmash

// DisposeSmashableEnvironment

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

// ExtrapolateGeometryCoordinates

// ExtractDeTiledFaces

// AddDecal

// InsertQuad

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

// SmashEnvironment

// BashObject

// NonCarSnapSmashCallback

// SmashFaceMustBeUpdateable

// MungeMaterialCB

// MungeMaterial

// ReinitSmashing

// LinkSmashies

// RepairModels (static in smash.c)

// FUNCTION: CARMA2_HW 0x004f5a00
extern intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec);

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

// LoadWastedExplosion

// ChangeSmashModel

// TurnOnSmashModel

// TurnOffSmashModel

// SmashRemoveSomeFaces

extern void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

extern void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, tS16 pCount);

// SmashRestoreSomeFaces

// TurnOnSmashDecal

// TurnOffSmashDecal

void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex);

void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex);

// ChangeSmashTexture

void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash);

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers

#endif // GUARD_SMASH_H
