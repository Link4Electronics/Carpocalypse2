#ifndef GUARD_PEDESTRN_H
#define GUARD_PEDESTRN_H

#include "c2_stdio.h"
#include "carpocalypse2_types.h"
#include <brender/brender.h>

#include "c2_hooks.h"

#ifndef CARPOCALYPSE2_MATCHING
void RefreshPedestrianAfterInterpolation(tPhysics_object* object);
int PedestrianOwnsPassiveRoot(tPhysics_object* object);
#endif

void C2_HOOK_FASTCALL FUN_004d2320(tPhysics_object* pObject1, tPhysics_object* pObject2, void* pCollision_data, float pImpact, br_vector3* pPosition);

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

// PedApplyPedDir

// SetCharacterMoveAR

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

// SetCharacterPhysicsLevelAR

// PedApplyPedPhysics

// PedApplyPedPos

// PedApplyVanishDismembered

// SetCharacterPositionAR

// SetCharacterBoneModelAR

// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

// PedUndoDismember

// ReadPedSpecs

// GetPedPos

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

// InitNapalmNolts

extern int gBOOL_00744804;

extern void C2_HOOK_FASTCALL InitPolyPedSystem(void);

extern int gBOOL_00744804;

// DisposePedSpawnSpec

// DisposeCharacterInstances

extern void C2_HOOK_FASTCALL DisposePedStuff(void);

// BonerOpenCharacterForm

// BonerOpenDefaultMoves

// BonerOpenPersonality

// BonerOpenMoves

// BonerOpenRemaps

// BonerReadPersonalityModels

// MakeActorRenderable

// StopActorBeingRenderable

// IsActorRenderworthy

// SetPedXZDirection

// StopPedNoise

// MakePedNoise

// SetPedMove

// StartPedRunning

// AcceleratePed

// MungePedHeadAnim

// SetPedHeadAnim

// ClearPedHeadAnim

// ScarePedestrian

// PedAnimCausesMovement

// StopSmoothTurning

// SetKnockedOverMove

// DoPostElectricution

// CBMoveCompleted

// CBLoadForm

// CBDisposeForm

// CBLoadPersonality

// CBDisposePersonality

// CBFillInObject

// DoGibShower

// PedApplyGibShower

// PedUndoGibShower

// DoGiblets

// DoBloodSpurt

// DoSpurt

// PedApplyBloodSpurt

// PedUndoBloodSpurt

// OneLessPed

// ScoreForKilledPedestrian

// KillPedestrian

// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

// DoToPeds

// DoPedAnnihilator

// RepulseIt

// DoPedestrianRepulsifictor

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

// RescanPedProximity

// RecacheAndSetFace

// PedScanForObjects

// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

// ActiveHalted

// CBActiveHalted

// RecordPedSlide

// PedProcessForces

// CBMovedByPhysics

// GetPedCount

// GetPedPosition

// TurnLimbsOnAndOff

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

// FinishUpLoadingPeds

// MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

// SetThisPedPhysicing

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

// RenderLimbs

// MungePedestrians

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

// ARResetNapalmBolts

// AdjustNapalmBolt

// InitPedsForRace

// RenderElectroBastardRays

// AdjustProxRay

// ResetProxRay

// GetTestPed

// NextPedCam

// PrevPedCam

// FindNearestPed

// ResetPedNearness

// PedPreCollisionStuff

// UpdateIfBackwardsAR

// ChangePedScaling

// ChangeHeadScaling

// MakeMaterialBlendy

// UnmakeMaterialBlendy

// TurnOffCollisions

// TurnOnCollisions

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

// MutatePed

// FlushAllPedCaches

extern void C2_HOOK_FASTCALL SavePedStuff(void);
extern void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions);



extern void C2_HOOK_FASTCALL ComparePedStuff(void);

extern void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4);

// CalmDownAllPeds

extern void C2_HOOK_FASTCALL FUN_004d6da0(void);

extern void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void);

void C2_HOOK_FASTCALL AddChangedPoint(const br_vector3* pPoint);

// GetHowMuchBloodAndSnotToSmearAbout

// AreWeGoingToOutrageAnimalLoversAroundTheGlobe

// IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense

// IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells

int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

void C2_HOOK_FASTCALL FUN_004d35e0(tPedestrian* pPed);

extern void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

extern void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

extern void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on);

extern void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

// ClearOutMorphs

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

// ScaleModelXYZ

// DRScaleModel

// DisposeMove
void C2_HOOK_FASTCALL DisposeMove(tPed_move* pMove);

// DisposeAllLoadedMoves

// ReadRemap

// DisposeRemap

// DisposeAllLoadedRemaps

// FindOrOpenRemap

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

// SetUpCharacterForm

// DisposeCharacterForm

// DisposeAllLoadedForms

// DisposeCharacterInstance

// FindOrOpenForm

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

// FindOrOpenPersonality

// BuildCharacterInstance

// GetCharacterMatrixPtr

// GetBoneMatrixPtr

// GetCharacterActorPtr

// SetCharacterBonePositions

// MakeCharacterRenderable2

// MakeCharacterRenderable

// CharacterNoLongerRenderable

// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

// MakeCharacterCollideworthy2

// MakeCharacterCollideworthy

// GetRootObject

// CharacterNoLongerCollideworthy

// SetObjectV

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// MakeCharacterPhysicworthy

// RecalculateOrientationOfRoot

// BonerActiveHalted2

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

// StopCharacterMorphing

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

// SetCharacterMove

// DRMatrix34RotateCos

// DropPointOntoPlane

// SetCharacterDirection

// SetCharacterPosition

// DisposeClonedMaterials

// DisposeAllBonerData

// SetCharacterBoneModel

// SetCharacterBoneModelP

// SetCharacterAllBonesModel

// GetCharacterBoneModel

// GetCharacterModelSet

// BonerPassiveCollision

// BonerActiveHalted

// BonerPedMovedByPhysics

// DismemberCharacter

// GetClearanceFromCharacterInstance

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

// MungeAllCharacterMaterials

// MungeAllCharacterObjects

// MungeAllCharacterModels

// DisableOverallMovement

// EnableOverallMovement

// EnableRetainRootMode

// DisableRetainRootMode

#define CARPOCALYPSE2_PEDESTRN_H

#include "c2_stdio.h"

#include "c2_hooks.h"

#include <brender/brender.h>
#include "carpocalypse2_types.h"

extern tPedestrian* gPedestrian_array;

extern int gPed_count;
extern int gPed_676914;


extern int gPed_nearness;

extern int gSelected_ped;

extern int gCount_killed_peds;

extern int gTotal_count_smash_peds;

extern int gCount_active_peds;

extern int gPeds_already_munged;

extern tPedestrian* gPedestrians_in_sight;

extern tU32 gPed_last_munging;

extern tU32 gLast_scare_time;

extern tPed_cache_006944c0 gPed_cache_00694328;

extern br_actor* gLimbs_actor;

extern int gPed_count_limbed_actors;

extern br_actor* gPed_limbed_actors[30];

extern tProximity_ray gProximity_rays[20];

int C2_HOOK_FASTCALL GetPedCount(void);

int C2_HOOK_FASTCALL GetPedPosition(int pIndex, br_vector3* pPos);

void C2_HOOK_FASTCALL ClearOutMorphs(void);

void C2_HOOK_FASTCALL InitBoner(tPedForms_vtable* pTable);

void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale);

void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pFile);
void C2_HOOK_FASTCALL ReadSettingsFile(void);


void C2_HOOK_FASTCALL InitNapalmNolts(void);

int C2_HOOK_FASTCALL AddPedToDistanceList(tPedestrian* pPed, tCar_spec* pCar, float pDistance, tU32 pNow, tPedestrian_distances* pList);

int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);

void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);

tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pGroup_name, br_matrix34* pMat34);

br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance *pCharacter);

void C2_HOOK_FASTCALL SetPedRetainRootMode(void);

void C2_HOOK_FASTCALL ClearPedRetainRootMode(void);

void C2_HOOK_FASTCALL PedNoOp(void);

void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2);

void C2_HOOK_FASTCALL MakeRotationMatrix(br_matrix34* pMat, tU8 pAngle_x, tU32 pAngle_y, tU32 pAngle_z);

void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9);

void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);

extern const int gPow2_array[32];

void C2_HOOK_FASTCALL ResetAnimation(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL SetOnGroundVector(tPed_character_instance* pPed, br_vector3* pPos, br_vector3* pNew_pos);

void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);

int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pPed,int pMove_action, float pSpeed, int pArg4, int pArg5, undefined4 pArg6);

void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pAction, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pArg7);

tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance *pPed);

void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest);

void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);

void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

int C2_HOOK_FASTCALL OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle);

void C2_HOOK_FASTCALL AssertRootObjectsMatrix(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp);

br_scalar C2_HOOK_FASTCALL AreaOfTriangle(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

void C2_HOOK_FASTCALL MakeRandomPointInTriangle(br_vector3* pDest, br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

void C2_HOOK_FASTCALL SetModelCallbacks(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec);

void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face *pFace, br_model *pModel);

void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);

void C2_HOOK_FASTCALL InitPedsForRace(void);

void C2_HOOK_FASTCALL RenderElectroBastardRays(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

void C2_HOOK_FASTCALL ResetPedNearness(void);

void C2_HOOK_FASTCALL PossiblePedSmear(tPedestrian* pPed);

void C2_HOOK_FASTCALL LastChanceForPedEffects(void);

void C2_HOOK_FASTCALL MungeNapalm(void);

void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter);

void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel, undefined4 pArg3);

undefined4 C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);

void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed);

void C2_HOOK_FAKE_THISCALL ScoreForKilledPedestrian(tPedestrian* pPed, undefined4 pArg2, float pHeight);

int C2_HOOK_CDECL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, int pLevel, tU32 pArg3);

void C2_HOOK_FASTCALL MakePedVanish(tPedestrian* pPed);

int C2_HOOK_FASTCALL PedFallingForever(tPedestrian* pPed);

void C2_HOOK_FASTCALL KillNapalmBolt(tNapalm_bolt* pBolt);

void C2_HOOK_FASTCALL SetNextRandomTurn(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL InitProcessData(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL AcceleratePed(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL SetCharacterPositionAR(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);

void C2_HOOK_FASTCALL StopSmoothTurning(tPedestrian* pPed);

void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed);

int C2_HOOK_FASTCALL PedAnimCausesMovement(tPedestrian* pPed);

void C2_HOOK_FASTCALL SmoothTurnPedestrian(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL RandomWander(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL ResetScanDirection(tPedestrian* pPed);

void C2_HOOK_FASTCALL CheckForAvoidingAction(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL SetRandomOmega(tPhysics_object* pObject, float pMax);

void C2_HOOK_FASTCALL CheckPowerupMoveSubstitution(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL StillifyCorpse(tPedestrian* pPed, tU32 pTime, undefined4 pArg3);

void C2_HOOK_FASTCALL MakeEmBleed(tPedestrian* pPed, tU32 pTime);

void C2_HOOK_FASTCALL SetPedHeadAnim(tPedestrian *pPed, const tPed_anim_seq* pSeq, tU32 pTime);

void C2_HOOK_FASTCALL ScarePedestrian(tPedestrian* pPed, tU32 pTime, int pArg3, int pArg4);

void C2_HOOK_FASTCALL MungePedestrians(void);

void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);

void C2_HOOK_FASTCALL FlushAllPedCaches(void);

undefined4 C2_HOOK_FASTCALL CBPassiveCollision(tPed_character_instance* pCharacter, tPhysics_object* pObject, tPhysics_object* pCollision_info);

int C2_HOOK_FASTCALL CBActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pPhysics);

int C2_HOOK_FASTCALL CBActiveHaltedInternal(tPed_character_instance* pCharacter, tPhysics_object* pPhysics, int pMode);

void C2_HOOK_FASTCALL CBMovedByPhysics(tPed_character_instance* pCharacter, tPhysics_object* pPhysics, undefined4 pArg3);

int C2_HOOK_FASTCALL CBMoveCompleted(tPed_character_instance* pCharacter);

void C2_HOOK_FASTCALL PedProcessContact(tPedestrian* pPed, tPhysics_object* pCollision_info);

int C2_HOOK_FASTCALL SetPedFall(tPedestrian* pPed, int pSpeed, int pMove1, int pMove2, tPhysics_object* pCollision_info);

int C2_HOOK_FASTCALL FUN_004cd9e0(tPedestrian* pPed, int pBoneIndex);

void C2_HOOK_FASTCALL CBFillInObject(undefined4* pArg1, undefined4 pArg2);

void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF);

void C2_HOOK_FASTCALL CBDisposeForm(undefined4* pArg1);

void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF);

void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality);

FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);

tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char *pFile_name);

FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);

FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);

FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);

int C2_HOOK_FASTCALL DRVector3NonZero(const br_vector3* pV);

void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName);

void C2_HOOK_FASTCALL RemapVector(br_vector3* pV, const tPed_remap_bone* pRemap);

tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName);

void C2_HOOK_FASTCALL Flip3DStoBRaxes(br_matrix34* pMat);

tPed_move* C2_HOOK_FASTCALL ReadMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags);

tPed_move* C2_HOOK_FASTCALL FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 flags);

tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);

FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);

tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName);

void C2_HOOK_FASTCALL RemapModelAxis(br_model* pModel, const tPed_remap_bone* pRemap);

br_model* C2_HOOK_FASTCALL BonerCloneModel(br_model *pModel, int pIndex, int pUpdateable);

float C2_HOOK_FASTCALL CalcBoundsRadius(const br_bounds3* pBounds);

tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);

tPed_personality* C2_HOOK_FASTCALL FindOrOpenPersonality(const char* pName);

tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);

void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pIndex, tPed_character_instance* pCharacter);

void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor);

int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor);

undefined4 C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);

undefined4 C2_HOOK_FASTCALL CharacterNoLongerCollideworthy(tPed_character_instance* pCharacter);

int C2_HOOK_FASTCALL MakeCharacterPhysicworthy(tPed_character_instance* pCharacter);

undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);

undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter);

int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4);

int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance *pCharacter, int pArg2, undefined4 pArg3);

tPhysics_object* C2_HOOK_FASTCALL MakeCharacterPhysicsForBone(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, undefined4 pArg4);
int C2_HOOK_FASTCALL MakeCharacterBonePhysicworthy(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pOmega);

int C2_HOOK_FASTCALL MakeCharacterPhysics(tPed_character_instance* pCharacter, tPhysics_object* obj, br_actor* actor, void* frame, br_vector3* bone_pos, int kind, undefined4 pArg3, int flag, int bone_index, br_vector3* bone_vec, br_vector3* bone_ptr);

void C2_HOOK_FASTCALL DisableOverallMovement(void);

void C2_HOOK_FASTCALL EnableOverallMovement(void);

br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed);

void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info);

void C2_HOOK_FASTCALL DoPostElectricution(tPedestrian* pPed, tU32 pTime, float pF1, float pF2);

void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info);

void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime);

tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime);

void C2_HOOK_FASTCALL CachePedestrianFaces(tPedestrian* pPed, int pArg2, int pCache_key);

float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum);

void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);

void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pArg3, tU32 pTime);

float C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, br_vector3* pArg2);

void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime);

int C2_HOOK_FASTCALL FUN_00409c20(tPed_character_instance* pCharacter, undefined4 pTime);

void C2_HOOK_FASTCALL MungeCharacterAnimation(tPed_character_instance* pCharacter, tU32 pTime, undefined4 pArg3);

void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4);

br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index);

void C2_HOOK_FASTCALL FUN_004ccb10(tU32 pFragment_owner, tU32 pTime, tU32 pSeed, float pForce, int pStarting_size, br_vector3* pInitial_position, br_vector3* pInitial_velocity, br_vector3* volatile pDirection);

void C2_HOOK_FASTCALL DoGiblets(tPedestrian* pPed, tPhysics_object* pObject, tPhysics_object* pOptional_speed_object, float pForce, br_vector3* pInitial_velocity, int pStarting_size);

void C2_HOOK_FASTCALL DoBloodSpurt(uintptr_t pArg1, int pArg2, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

void C2_HOOK_FASTCALL DoBloodSpurtUndo(int pCurrent_time, uintptr_t pOwner, int pSpurt_time, tU32 pSeed, br_vector3* pPosition, br_vector3* pDirection, br_vector3* pRandom_direction);

void C2_HOOK_FASTCALL DoVanishDismembered(uintptr_t pOwner, tS32 value);

void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4);

br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index);

int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter);

int C2_HOOK_FASTCALL GetCharacterBoneIndexByName(tPed_character_instance* pCharacter, const char* pBone_name);

int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4);

void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pArg2, int pArg3);

void C2_HOOK_FASTCALL FUN_0040b500(tPed_character_instance* pCharacter, int pBone_index, br_model* pModel, tS8 pModel_set);

void C2_HOOK_FASTCALL SetPedStartRun(tPedestrian* pPed);

void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3);

br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter);

void C2_HOOK_FASTCALL RenderLimbs(void);

void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

int C2_HOOK_FASTCALL CalmDownAllPeds(void);

void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, br_vector3* pV);

int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2);

int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter);

int C2_HOOK_FASTCALL BonerPassiveCollision(tPed_character_instance* pCharacter, undefined4 pArg2);

void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2);

extern float gConst_replay_rate_zero;

extern const int gPow2_array[32];



void C2_HOOK_FASTCALL PedNoOp2(void);

void C2_HOOK_FASTCALL DisposePedForm(tPed_form* pForm);

void C2_HOOK_FASTCALL DisposePedForms(void);

void C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2);

// UNLABELED: CARMA2_HW 0x004ccb10
void C2_HOOK_FASTCALL DoGibShower(int pId, int pTime, int pSeed, float pForce, int pSize_index, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

void C2_HOOK_FASTCALL InitOtherPedStuff(void);

void C2_HOOK_FASTCALL InitFaceCaches(void);


extern int gPed_676914;

#endif // GUARD_PEDESTRN_H
