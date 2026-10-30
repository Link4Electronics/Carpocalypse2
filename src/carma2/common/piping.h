#ifndef GUARD_PIPING_H
#define GUARD_PIPING_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"
#include <s3/s3.h>

// ARReplayForwards

// ARReplayIsReallyPaused

// ARReplayPauseState

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern int C2_HOOK_FASTCALL ARReplayPauseState(void);



// ARSetReplayRate

// ARGetReplayDirection

// ARSetReplayDirection

// ARResetPiping

// ARInitialise

// ARDispose

// LengthOfChunk

// LengthOfSession

// ARAdvanceChunkPtr
void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tPipe_chunk** pChunk_ptr, tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);



// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

// ARSomeReplayLeft

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

// EndPipingSession2

// AREndPipingSession

// ARAddDataToSession

// ARAddVariedDataToSession

// ARDoSingleSession

// ARDoSingleVariedSession
extern void C2_HOOK_FASTCALL FUN_004c8710(uintptr_t pOwner, tU16 pValue_0, tU16 pValue_2, tU16 pValue_4, br_vector3* pVector, br_scalar pValue_20);
extern int C2_HOOK_FASTCALL ARSomeReplayLeft(void);



void C2_HOOK_FASTCALL FUN_004c86b0(tU32 pArg1, br_actor* pActor);

extern void C2_HOOK_FASTCALL FUN_004c86f0(int pOwner, void* pData, tU32 pArg3);

// ARResetPipePlayToEnd

// ARGetPipePlayPtr

// ARSetPipePlayPtr

// ApplyPipedSession

// MoveSessionPointerBackOne

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

// SoundTimeout

// ScanAndPlaySoundsToBe

// ARGetStartTime

// ARDisableAdvance

// AREnableAdvance

// FindNextChunk

// MoveReplayBuffer

// ARMoveToEndOfReplay

// ARMoveToStartOfReplay

typedef void C2_HOOK_FASTCALL tARDoActionReplay_step_callback(int pStep_count);
extern void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr);


extern int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pSession);

extern int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime);

extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);

typedef void C2_HOOK_FASTCALL tARDoActionReplay_frame_callback(tU32 pFrame_period);

void C2_HOOK_FASTCALL FUN_00403210(tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);

void C2_HOOK_FASTCALL ARDoActionReplay(int pStep_count, tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);

// ARDoActionReplay

// FUNCTION: CARMA2_HW 0x00403210

// FUNCTION: CARMA2_HW 0x00403c70
void C2_HOOK_FASTCALL FUN_00403c70(tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);

// FUNCTION: CARMA2_HW 0x00403260

extern void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag);

extern int C2_HOOK_FASTCALL ARGoBackwards(void);

extern int C2_HOOK_FASTCALL ARGoForwards(void);

// ARService

// ARMainLoopStart

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// PipeObjectPosition

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

// CheckCar

// CarTimeout

// ScanCarsPositions

// InitialiseActionReplay

extern void C2_HOOK_FASTCALL DisposeActionReplay(void);

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

// AddSparkToPipingSession

// AddShrapnelToPipingSession

// AddNonCarToPipingSession

// AddSmokeToPipingSession

// AddSmokeColumnToPipingSession

// AddFlameToPipingSession

// AddSplashToPipingSession
void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pCollision);
void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);



// AddExtendedSplashToPipingSession

void C2_HOOK_FASTCALL FUN_004c71e0(uintptr_t pOwner, void* pData, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, void* pArg8, undefined4 pArg9);

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession
void C2_HOOK_FASTCALL AddSoundToPipingSession(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

void C2_HOOK_FASTCALL AddFlapToPipingSession(uintptr_t pOwner, int pDelta);
void C2_HOOK_FASTCALL WorkerFlap(br_actor* pActor, tS16 pDelta);

// AddModelMashToPipingSession

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

// PipeSingleCrush

// PipeSingleScreenWobble

// PipeSingleGrooveStop

// PipeSingleNonCar

// PipeSingleSmoke

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

void C2_HOOK_FASTCALL PipeSingleCar(int pCar_id, br_matrix34* pMatrix, br_vector3* pV, float pSpeedo_speed, float pRevs, int pGear, int pFrame_collision_flag, undefined4 pArg7);

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// PipeSingleFlap

// PipeSingleRelink

// PipeSingleIdentity

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// PipeSingleExplosion

// PipeSingleNonCarCreation

extern void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);
extern void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle);


extern void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor);


// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// PipeSingleSmashTextureChange

// PipeSinglePedStatus

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// PipeSinglePHILObject

// PipeSingleGibShower

// PipeSingleBloodSpurt

// PipeSingleGrooveOnOff

// PipeSinglePowerupGot

// PipeSinglePowerupLose

void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar);

void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar);

// PipeSingleFunkEnable

// PipeSingleVanishedDismembered

// PipeSingleDSModel

// PipeSingleVector3

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

// PipeSingleTransformType

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// ApplyScreenWobble

// ApplyGrooveStop

// ApplyNonCar

// ApplySmoke

// ApplySmokeColumn

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

// ApplyFrameBoundary

// ApplySound

// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

// ApplyRelink

// ApplyIdentity

// ApplySplitWeld

// ApplyBend

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

// ApplySmashModelChange

// ApplySmashRemoveFaces

// ApplySmashDecal

// ApplySmashTextureChange

// ApplyRepulseRay

// ApplyActorTrans

// ApplyPedStatus

// ApplyPedDir

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

// ApplyPedFPChange

// ApplyPedPos

// ApplyPedModelChange

// ApplyPHILObject

// ApplyGibShower

// ApplyBloodSpurt

// ApplyGrooveOnOff

// ApplyPowerupGot

// ApplyPowerupLose

// ApplyFunkEnable

// ApplyVanishDismembered

// ApplyDSModel

// ApplyPedDiagnostics

// ApplyVector3

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

// ApplyTransformType

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

// UndoDamage

// UndoSpecial

// UndoScreenWobble

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

// UndoFlap

// UndoModelMash

// UndoRelink

// UndoSplitWeld

// UndoBend

// UndoUnBend

// UndoShrapnelShower

// UndoNonCarCreation

// UndoSmashModelChange

// UndoSmashRemoveFaces

// UndoSmashDecal

// UndoSmashTextureChange

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

// UndoPedDismember

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

// UndoGrooveOnOff

// UndoPowerupGot

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

// UndoVector3

// UndoNapalmBolt

// UndoPowerupRespawn

// UndoShitMine

// UndoEndShitMine

// UndoTransformType

// CalcCrushLength

// CalcShrapnelLength

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength

#define CARPOCALYPSE2_PIPING_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern undefined4* gCrush_space;

extern tU8* gPipe_play_ptr;

extern int gPlay_direction;

extern tU8* gPipe_record_ptr;

extern tU8* gPipe_buffer_oldest;

extern tU8* gLocal_buffer;

extern tU8* gPipe_buffer_start;
extern int gReplay_pause_state;


extern int gPipe_undo_enabled;

void C2_HOOK_FASTCALL SetPipeUndoFlag(void);

void C2_HOOK_FASTCALL ClearPipeUndoFlag(void);

extern tU8* gPipe_buffer_phys_end;

extern tU8* gPipe_buffer_working_end;

extern float gReplay_rate;
extern tU32 gPipe_sound_time;


extern int gDisable_sound;

extern tU32 gYoungest_time;

extern tU32 gOldest_time;

extern br_vector3 gCar_pos;

extern br_scalar gMax_distance;

extern br_vector3 gReference_pos;

extern tCar_spec* gCar_ptr;

extern tU32 gTrigger_time;

extern volatile br_scalar gUNK_0069410c;
#define gUNK_0069410c_u32 (*(volatile tU32*)&gUNK_0069410c)

extern tPipe_smudge_data* gSmudge_space;

extern const tReplay_callback* gPipe_callbacks;

extern int gPipe_count_callbacks;

extern tU32 gPipe_buffer_size;

int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);

void C2_HOOK_FASTCALL ARResetPiping(void);

void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** buffer, tU32* size);

void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks);

int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);

int C2_HOOK_FASTCALL ARIsPlayPointerMovable(void);

void C2_HOOK_FASTCALL DisposePiping(void);

void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy);

void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);

void C2_HOOK_FASTCALL AREndPipingSession(void);

tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner);

tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession);
tPipe_chunk_data* C2_HOOK_FASTCALL FUN_00403800(tPipe_chunk* pSession, tPipe_chunk_type pType, tU32 pOwner);

void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);

void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void *pData, int pSize);

int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType);


void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);
void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);


void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);
void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2);
void C2_HOOK_FASTCALL FUN_004c70f0(uintptr_t pOwner, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6);
void C2_HOOK_FASTCALL FUN_004c8500(uintptr_t pOwner, const br_vector3* pVector);

void C2_HOOK_FASTCALL ClearCarActiveFlags(void);

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void);

void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);
void C2_HOOK_FASTCALL AddNonCarToPipingSession(tPhysics_object* pObject, br_actor* pActor);


void C2_HOOK_FASTCALL AddNonCarActorToPipingSession(int pCar_ID, br_actor* pActor);

void C2_HOOK_FASTCALL FUN_0043bca0(tCompressed_vector3* pDest, tU16* pDest2, br_model* pModel, br_vector3* pSrc, const float* pScalars);

int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext);

void C2_HOOK_FASTCALL AddCarToPipingSession(int pCar_id, br_matrix34* pMatrix, br_vector3* pV, float pSpeedo_speed, float pRevs, int pGear, int pFrame_collision_flag, undefined4 pArg7);

void C2_HOOK_FASTCALL AddDamageToPipingSession(int pCar_id, tU8 *pDamage_deltas);

void C2_HOOK_FASTCALL PipeCarPositions(void);

tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

int C2_HOOK_FASTCALL ARGetReplayDirection(void);

void C2_HOOK_STDCALL FUN_004023c0(float pDirection);

int C2_HOOK_FASTCALL FindPipeSessionZeroValue(void);

int C2_HOOK_FASTCALL ARReplayForwards(void);

// FUNCTION: CARMA2_HW 0x00402540
int C2_HOOK_FASTCALL ARReplayPlaying(void);

// FUNCTION: CARMA2_HW 0x004c6c90
int C2_HOOK_FASTCALL GetPipeCarStateAtTime(tS32 pOffset, tS32* pMode, br_scalar* pTowards, undefined4* pData, tS32* pTime);

int C2_HOOK_FASTCALL GetPipeCarStateAtTimeCB(tPipe_chunk* pChunk, int pChunk_count, tU32 pTime);

int C2_HOOK_FASTCALL CarTimeout(tU32 pTime);

int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime);

int C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);

void C2_HOOK_FASTCALL ScanAndPlaySoundsToBe(tPipe_chunk* pPtr, tU32 pOldest_time, tU32 pYoungest_time);

void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned);

void C2_HOOK_FASTCALL PipeSingleSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);

void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, undefined4 pArg5,undefined4 pArg6, int pArg7, undefined4 pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13);

void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat);

void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir);

void C2_HOOK_FASTCALL PipeSinglePedMoveStatus(tPedestrian* pPed, float pValue, float pArg3);

void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);

void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pField_0x70, tCompressed_matrix34* pMatrix);

void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates);

void C2_HOOK_FASTCALL PipeSingleDSModel(int pArg1, void* pThing);

void C2_HOOK_FASTCALL FUN_004c7fa0(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates);

void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor *pActor, int pBlock_flags);

void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3 *pDest, br_model* pModel, br_vector3* pSrc);

void C2_HOOK_FASTCALL FUN_0043c240(tCar_spec* pCar_spec, float* pPlane);

void C2_HOOK_FASTCALL ReceivedCrushDetach(tU8* pContents);

void C2_HOOK_FASTCALL ReceivedFullDetachBit(tU8* pParam);

void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash);

void C2_HOOK_FASTCALL FUN_004c6e40(br_model* pModel, br_vector3* pDirection, const float* pScalars);

void C2_HOOK_FASTCALL FUN_004c8650(br_model* pModel, br_vector3* pMash);

void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);

void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags);

void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType);

void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3);

void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable);

void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, undefined pArg2, undefined pArg3, undefined pArg4, undefined pArg5, tS8 pArg6, tU8 pArg7, undefined pArg8, tU8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12);

void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity);

void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, undefined4 pArg2, undefined4 pArg3);

void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6);

void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar);
void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce);


void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor);

void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject);

void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial);

void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, tSmoke_column* pSmoke_column, int pVertex, int pColour);

void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id);

void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime);

void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y);

void C2_HOOK_FASTCALL PipeSingleFrameFinish(void);

void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);

void C2_HOOK_FASTCALL SaveReducedPos(tReduced_pos* p, br_vector3* v);

void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime);

void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);

void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

void C2_HOOK_FASTCALL PipeSingleMatrix34Type66(br_actor* pActor);

void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor);


void C2_HOOK_STDCALL PipeSingleLocalCarIncident(br_scalar pArg);

void C2_HOOK_FASTCALL FUN_004c8960(int pOwner, tU32 pArg2, tU32 pArg3);


extern int gDisable_piped_sounds;

extern tU32 gPipe_sound_time_window;

extern tU32 gPipe_sound_start_time;

extern tU32 gPipe_sound_time;

extern int gReplay_pause_state;

#endif // GUARD_PIPING_H
