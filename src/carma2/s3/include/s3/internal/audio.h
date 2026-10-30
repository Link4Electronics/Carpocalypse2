#ifndef S3_AUDIO_H
#define S3_AUDIO_H

#include <s3/s3.h>

extern char gS3_path_separator[2];
extern int gS3_enabled;
extern int gS3_CDA_enabled;
extern int gS3_soundbank_buffer_len;
extern tS3_state gS3_state;
extern char* gS3_soundbank_buffer;
extern tS3_callbacks gS3_callbacks;
extern int gS3_effects_enabled;
extern int gS3_opened_output_devices;
extern br_uint_32 gS3_last_service_time;
extern tS3_channel* gS3_unbound_channels;
extern tS3_channel* gS3_last_unbound_channel;
extern int gS3_enable_midi;
extern br_uint_32 gS3_tag_seed;
extern tS3_sample_filter gS3_sample_filter_func;
extern tS3_sample_filter gS3_sample_filter_disable_func;
extern tS3_channel gS3_channel_template;
extern int gS3_delta_time;
extern int gS3_inside_cockpit;
extern br_uint_32 gS3_last_service_time_spatial;

extern int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path);

extern void C2_HOOK_FASTCALL S3Enable(void);

extern void C2_HOOK_FASTCALL S3Disable(void);

extern int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3GetCountChannels(int pCount_channels_1, int pCount_channels_2);

extern tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2);

extern tS3_error_codes C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet);

extern int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

extern int C2_HOOK_FASTCALL S3StopAllOutletSounds(void);

extern int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);

extern int C2_HOOK_FASTCALL S3CountActiveChannels(tS3_outlet* pOutlet);

extern int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src);

extern tS3_error_codes C2_HOOK_FASTCALL S3StopSound(int pTag);

extern void C2_HOOK_FASTCALL S3UpdateListenerVectors(void);

extern void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1);

extern int C2_HOOK_FASTCALL S3DisableSound(void);

extern void C2_HOOK_FASTCALL S3EnableCDA(void);

extern void* C2_HOOK_FASTCALL S3LoadSoundBankFile(const char* pPath);

extern int C2_HOOK_FASTCALL S3LoadSoundbank(const char* pPath, int pLow_memory_mode);

extern int C2_HOOK_FASTCALL S3CreateOutletChannels(tS3_outlet* outlet, int pChannel_count);

extern void C2_HOOK_FASTCALL S3SoundBankReaderSkipWhitespace(tS3_soundbank_read_ctx* pContext);

extern int C2_HOOK_FASTCALL S3SoundBankReadEntry(tS3_soundbank_read_ctx *pContext, const char* pDir_name, int pLow_memory_mode);

extern void C2_HOOK_FASTCALL S3SoundBankReaderNextLine(tS3_soundbank_read_ctx* pContext);

extern void C2_HOOK_FASTCALL S3SoundBankReaderSkipToNewline(tS3_soundbank_read_ctx* pContext);

extern void C2_HOOK_FASTCALL S3SoundBankReaderAdvance(tS3_soundbank_read_ctx* pContext, int pAmount);

extern tS3_descriptor* C2_HOOK_FASTCALL S3CreateDescriptor(void);

extern int C2_HOOK_FASTCALL S3SoundBankReaderReadFilename(char** pPath, tS3_soundbank_read_ctx* pContext, const char* pDir_name);

extern void C2_HOOK_FASTCALL S3StopMidi(void);

extern void C2_HOOK_FASTCALL S3StopCDA(void);
extern void C2_HOOK_FASTCALL S3ToggleCDA(void);

extern void C2_HOOK_FASTCALL S3StopMidiInternal(void);

extern int C2_HOOK_FASTCALL S3StopMIDIChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3StopCDAInternal(void);

extern int C2_HOOK_FASTCALL S3StopCDAChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3UnbindChannels(tS3_outlet* pOutlet);

extern tS3_channel* C2_HOOK_FASTCALL S3GetChannelForTag(int pTag);
extern int C2_HOOK_FASTCALL S3ResumeMidiChannel(int pTag);
extern int C2_HOOK_FASTCALL S3ResumeCDAChannel(int pTag);

extern void C2_HOOK_FASTCALL FUN_00564d4a(int pUnused, int pTag);

extern tS3_error_codes C2_HOOK_FASTCALL S3ClearBufferOfMidiChannel(int pTag);

extern int C2_HOOK_FASTCALL S3ReleaseSound(int pSound_id);
extern void C2_HOOK_FASTCALL S3ReleaseSoundList(int* pSound_ids, int pSound_count);

extern void C2_HOOK_FASTCALL S3LoadSampleList(int* pSample_ids, int pCount);
extern void C2_HOOK_FASTCALL S3UpdateSampleList(int* pOld_sample_ids, int pOld_count, int* pNew_sample_ids, int pNew_count);

extern double C2_HOOK_STDCALL S3FRandomBetween(double pMin, double pMax);
extern double C2_HOOK_STDCALL S3FRandomBetween2(double pMin, double pMax);

extern int C2_HOOK_FASTCALL S3IRandomBetween(int pMin, int pMax, int pDefault);

extern int C2_HOOK_FASTCALL S3IRandomBetween__dup(int pMin, int pMax, int pDefault);

extern int C2_HOOK_FASTCALL S3IRandomBetweenLog(int pMin, int pMax, int pDefault);

extern int C2_HOOK_FASTCALL S3FreeUnboundChannels(void);

extern int C2_HOOK_FASTCALL S3SoundStillPlaying(int pTag);

extern int C2_HOOK_FASTCALL S3SetVolume(int pVolume);

extern int C2_HOOK_FASTCALL S3ServiceChannel(tS3_channel *pChannel);
extern tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSource(void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet);


extern int C2_HOOK_FASTCALL S3ServiceSpatialSound(tS3_channel* pChannel);


extern tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceInternal(void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet);

extern tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceBR(br_vector3* pPosition, br_vector3* pVelocity, tS3_outlet* pBound_outlet);

extern int C2_HOOK_FASTCALL S3IsCDAEnabled(void);

extern void C2_HOOK_FASTCALL S3CalculateRandomizedFields(tS3_channel* chan, tS3_descriptor* desc);

extern int C2_HOOK_FASTCALL S3CalculatePriority(int pPriority, int pVolumeFactor);

extern int C2_HOOK_FASTCALL S3GenerateTag(tS3_outlet* outlet);

extern int C2_HOOK_FASTCALL S3GetLastError(void);

extern tS3_channel* C2_HOOK_FASTCALL S3AllocateChannel(tS3_outlet* pOutlet, int pPriority);

extern int C2_HOOK_FASTCALL S3MIDILoadSong2(tS3_channel* pChannel);

extern tS3_error_codes C2_HOOK_FASTCALL S3MIDILoadSong(tS3_channel* pChannel);

extern void C2_HOOK_FASTCALL S3ReverseSample(tS3_effect_tag pEffect, tS3_sound_tag pTag);

extern int C2_HOOK_FASTCALL S3SetSampleFilterCallbacks(tS3_sample_filter pEnable, tS3_sample_filter pDisable);

extern int C2_HOOK_FASTCALL S3ExecuteSampleFilterFuncs(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3RegisterSampleFilters(tS3_sample_filter pFilter, tS3_sample_filter pDisable_filter);

extern tS3_error_codes C2_HOOK_FASTCALL S3PlayMIDI(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3SetMIDIVolume2(tS3_channel* pChannel, int pVolume);

extern int C2_HOOK_FASTCALL S3SetMIDIVolume(tS3_channel* pChannel, int pVolume);

extern int C2_HOOK_FASTCALL FUN_0056a6cd(int pTag, int pPitch);

extern tS3_error_codes C2_HOOK_FASTCALL S3PlayCDA(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

extern int C2_HOOK_FASTCALL FUN_005667af(int pSound_id, tS3_sound_source* pSource);

extern int C2_HOOK_FASTCALL FUN_00566a54(int pSound_id, tS3_sound_source* pSource, int pVolume, int pPitch, int pSpeed);

extern int C2_HOOK_FASTCALL S3SetChannelVolume(int pChannel_tag, int pVolume);

extern tS3_error_codes C2_HOOK_FASTCALL S3SetChannelPitch(tS3_sound_tag pChannel_tag, int pPitch);

extern int C2_HOOK_FASTCALL S3ChangePitchSpeed(tS3_sound_tag pTag, int pPitch);

extern tS3_error_codes C2_HOOK_FASTCALL FUN_00564a56(tS3_sound_tag pChannel_tag, int pPitch);

extern int C2_HOOK_FASTCALL S3SetChannelVelocity(tS3_sound_tag pChannel_tag, const tS3_vector3* pVelocity);

extern int C2_HOOK_FASTCALL S3StartSound2(tS3_outlet* pOutlet, tS3_sound_id pSound, unsigned int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch);

extern tS3_error_codes C2_HOOK_FASTCALL S3MIDITagHasStoppedPlaying(int pTag);

extern int C2_HOOK_FASTCALL S3UnmuteMidiChannel(int pTag);

extern int C2_HOOK_FASTCALL S3ServiceMIDIChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL S3IsCDAPlaying(void);

extern int C2_HOOK_FASTCALL S3UpdateCDAVolume(tS3_channel* pChannel, int pVolume);

extern void* C2_HOOK_CDECL LoadQuickTimeModule(void);

extern short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);

extern short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);

#endif // S3_AUDIO_H
