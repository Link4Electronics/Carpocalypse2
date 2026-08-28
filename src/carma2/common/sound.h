#ifndef GUARD_SOUND_H
#define GUARD_SOUND_H

#include "c2_stdio.h"


#include "carpocalypse2_types.h"
#include "brender/brender.h"
#include "s3/s3.h"

#include "c2_hooks.h"

#include <stdio.h>

extern tS3_outlet* gEffects_outlet;

extern int gCD_fully_installed;

extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size);

extern void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

extern void C2_HOOK_FASTCALL InitSound(void);

extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

extern int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound);

extern int C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet *pOutlet, tS3_sound_id pSound, int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch);

extern int C2_HOOK_FASTCALL DRS3ChangeVolume(int pChannel_id, int pVolume);

extern int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag);

extern void C2_HOOK_FASTCALL DRS3Service(void);

extern int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);

extern void C2_HOOK_FASTCALL DRS3ShutDown(void);

extern int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

extern int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);

extern int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void);

extern void C2_HOOK_FASTCALL ToggleSoundEnable(void);

extern void C2_HOOK_FASTCALL SoundService(void);

extern void C2_HOOK_FASTCALL InitSoundSources(void);

extern void C2_HOOK_FASTCALL DisposeSoundSources(void);

extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

extern void C2_HOOK_FASTCALL MungeEngineNoise(void);

extern void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);

extern tS3_outlet* C2_HOOK_FASTCALL GetOutletFromIndex(int pIndex);

extern int C2_HOOK_FASTCALL GetIndexFromOutlet(tS3_outlet* pOutlet);

extern int C2_HOOK_FASTCALL DRS3StartCDA(int pSound);

extern void C2_HOOK_FASTCALL DRS3StopCDA(void);

// StartMusicTrack

extern void C2_HOOK_FASTCALL StartMusic(void);

extern void C2_HOOK_FASTCALL StopMusic(void);

extern void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel);

// ReallySetSoundDetailLevel

// GetSoundDetailLevel

extern void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

// TryToSetEnvironmentalSound

// DoAnEnvironmentalSound

// DoEnvSound

// FoundSoundSource

// MungeSoundGenerators

// MungeEnvironmentalSound

extern void C2_HOOK_FASTCALL BuggerModelName(tTrack_spec* pTrack_spec, const char* pName, int pIndex);

extern void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF);

extern void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

extern void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void);

#define CARPOCALYPSE2_SOUND_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "carpocalypse2_types.h"


extern int gSound_detail_level;

extern int gSound_sources_inited;

extern tS3_outlet* gEngine_outlet;

extern int gMusic_available;

extern int gINT_00684554;

extern int gINT_00684568;

extern int gVirgin_pass;

extern int gOld_sound_detail_level;

extern int gCD_is_disabled;

extern tS3_outlet* gDriver_outlet;

extern tS3_outlet* gMusic_outlet;

extern tS3_outlet* gCar_outlet;

extern tS3_outlet* gPedestrians_outlet;

extern tS3_outlet* gXXX_outlet;

extern tS3_outlet* gIndexed_outlets[6];

extern br_vector3 gCamera_left;

extern br_vector3 gCamera_position;

extern br_vector3 gCamera_velocity;

extern tU32 gNext_sound_generator_munging;

extern int gCount_environmental_sound_sources;

extern int gEnvironmental_sound_sources_buffer_index;

extern tEnvironment_sound_source gEnvironment_sound_sources[5];

extern br_vector3 gZero_v__car;

extern br_vector3 gOld_camera_position;

void C2_HOOK_FASTCALL DoEnvSound(tSpecial_volume* pVolume, br_vector3* pP, tSpecial_volume_soundfx_type pType, tSpecial_volume_soundfx_data* pSound_data, float pSound_dist, br_vector3 *pSound_dir);

void C2_HOOK_FASTCALL DoAnEnvironmentalSound(void* pSrc_object, tEnvironment_sound_generator_info* pEnv_info, int pVolume, br_vector3* pPos);

int C2_HOOK_FASTCALL S3IsCDAPlaying(void);

void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track);

int C2_HOOK_FASTCALL DRS3SetVolume(int pVolume);

intptr_t C2_HOOK_FASTCALL FoundSoundSource(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL MungeSoundGenerators(void);

void C2_HOOK_FASTCALL MungeEnvironmentalSound(void);

#endif // GUARD_SOUND_H
