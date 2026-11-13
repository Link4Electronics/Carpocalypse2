#include "audio.h"

#include "../include/s3/internal/3d.h"
#include "resource.h"
#include "../include/s3/internal/s3sound.h"
#include "sample.h"
#include "platform.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <Movies.h>

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include <ctype.h>
#include "c2_io.h"
#include "c2_math.h"
#include "c2_sys_stat.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#ifdef CARPOCALYPSE2_PLATFORM_WIN32
#include "../../platform/win32/win32_sound.h"
#endif

#include <fcntl.h>
#include <stddef.h>

extern int C2_HOOK_FASTCALL PDS3UpdateMidiChannelPitch(tS3_channel* pChannel, int pPitch);


// GLOBAL: CARMA2_HW 0x007a06c0
int gS3_enabled;

// GLOBAL: CARMA2_HW 0x00673504
int gS3_CDA_enabled = 1;

// GLOBAL: CARMA2_HW 0x007a0554
char gS3_path_separator[2];

// GLOBAL: CARMA2_HW 0x007a0558
char gS3_sound_folder_name[6];

// GLOBAL: CARMA2_HW 0x007a0580
tS3_state gS3_state;

// GLOBAL: CARMA2_HW 0x006b2c80
int gS3_soundbank_buffer_len;

// GLOBAL: CARMA2_HW 0x006b2c84
char* gS3_soundbank_buffer;

// GLOBAL: CARMA2_HW 0x006b2dc8
tS3_callbacks gS3_callbacks;

// GLOBAL: CARMA2_HW 0x007a0758
int gS3_effects_enabled;

// GLOBAL: CARMA2_HW 0x006b2c18
int gS3_opened_output_devices;

// GLOBAL: CARMA2_HW 0x007a0568
br_uint_32 gS3_last_service_time;

// GLOBAL: CARMA2_HW 0x007a056c
tS3_channel* gS3_unbound_channels;

// GLOBAL: CARMA2_HW 0x007a059c
tS3_channel* gS3_last_unbound_channel;

// GLOBAL: CARMA2_HW 0x0079feb8
int gS3_enable_midi;

// GLOBAL: CARMA2_HW 0x007a0564
br_uint_32 gS3_tag_seed;

// GLOBAL: CARMA2_HW 0x007a0760
tS3_sample_filter gS3_sample_filter_func;

// GLOBAL: CARMA2_HW 0x007a0764
tS3_sample_filter gS3_sample_filter_disable_func;

// GLOBAL: CARMA2_HW 0x007a06e0
tS3_channel gS3_channel_template;

// GLOBAL: CARMA2_HW 0x007a075c
int gS3_delta_time;

// GLOBAL: CARMA2_HW 0x007a0560
int gS3_inside_cockpit;

// GLOBAL: CARMA2_HW 0x006b2c14
br_uint_32 gS3_last_service_time_spatial;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void);
extern int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel* pChannel);
extern void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor);
extern void C2_HOOK_FASTCALL PDS3ServiceCDA(tS32 pDelta_time);
extern void C2_HOOK_FASTCALL PDS3Stop(void);
extern int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel);
extern tS3_channel* C2_HOOK_FASTCALL S3AllocateChannel(tS3_outlet* pOutlet, int pPriority);
extern int C2_HOOK_FASTCALL S3Calculate3D(tS3_channel* pChannel, int pAmbient);
extern int C2_HOOK_FASTCALL S3CalculatePriority(int pPriority, int pVolumeFactor);
extern void C2_HOOK_FASTCALL S3CalculateRandomizedFields(tS3_channel* chan, tS3_descriptor* desc);
extern void C2_HOOK_FASTCALL S3CopyVector3(void* pDest, void* pSrc, int pIs_BRender);
extern int C2_HOOK_FASTCALL S3CreateOutletChannels(tS3_outlet* outlet, int pChannel_count);
extern void C2_HOOK_FASTCALL S3Disable(void);
extern int C2_HOOK_FASTCALL S3ExecuteSampleFilterFuncs(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL S3FreeUnboundChannels(void);
extern int C2_HOOK_FASTCALL S3GenerateTag(tS3_outlet* outlet);
extern tS3_channel* C2_HOOK_FASTCALL S3GetChannelForTag(int pTag);
extern tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id);
extern int C2_HOOK_FASTCALL S3IRandomBetweenLog(int pMin, int pMax, int pDefault);
extern int C2_HOOK_FASTCALL S3IRandomBetweenLog__dup(int pMin, int pMax, int pDefault);
extern int C2_HOOK_FASTCALL S3IRandomBetween__dup(int pMin, int pMax, int pDefault);
extern tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);
extern void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type);
extern void C2_HOOK_FASTCALL S3MemFree(void* p);
extern int C2_HOOK_FASTCALL S3PlaySample(tS3_channel* pChannel, tS3_descriptor* pDescriptor);
extern int C2_HOOK_FASTCALL S3ReleaseSound(int pSound_id);
extern int C2_HOOK_FASTCALL S3Service3D(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL S3ServiceMIDIChannel(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL S3ServiceSpatialSound(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL S3SoundStillPlaying(int pTag);
extern int C2_HOOK_FASTCALL S3StopAllOutletSounds(void);
extern void C2_HOOK_FASTCALL S3StopCDA(void);
extern void C2_HOOK_FASTCALL S3StopMidi(void);
extern void C2_HOOK_FASTCALL S3StopSoundSource(tS3_sound_source* src);
extern int C2_HOOK_FASTCALL S3UnbindChannels(tS3_outlet* pOutlet);
extern void C2_HOOK_FASTCALL S3UpdateListenerVectors(void);
extern void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);
extern int C2_HOOK_FASTCALL S3UpdateSourceVectors(void);
extern void __cdecl _ftol();
extern tPD_S3_config gPD_S3_config;
extern tS3_error_codes gS3_last_error;
extern void C2_HOOK_CDECL s3_debug_disable_sound();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern FARPROC gQuickTime_call_component_with_storage;
extern void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath);
extern void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);
extern const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void);
extern int C2_HOOK_FASTCALL PDS3Init(void);
extern int C2_HOOK_FASTCALL S3GetFileSize(FILE *pF);
extern void* C2_HOOK_FASTCALL S3LoadSoundBankFile(const char* pPath);
extern int C2_HOOK_FASTCALL S3LoadSoundbank(const char* pPath, int pLow_memory_mode);
extern int C2_HOOK_FASTCALL S3SoundBankReadEntry(tS3_soundbank_read_ctx *pContext, const char* pDir_name, int pLow_memory_mode);
extern void C2_HOOK_FASTCALL S3SoundBankReaderSkipWhitespace(tS3_soundbank_read_ctx* pContext);
extern FILE* C2_HOOK_FASTCALL S3_low_memory_fopen(const char* pPath, const char* pMode);
extern int gS3_low_memory_mode;
extern char gS3_sound_dirname[256];
extern char hitherYonTable[];
extern char rendererStateStoredDispatch[];
extern void C2_HOOK_CDECL s3_debug_init();
#endif
// FUNCTION: CARMA2_HW 0x005651d0
int C2_NAKED C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_dirname) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    char dirpath[256];

    s3_debug_init();

    C2_HOOK_BUG_ON(sizeof(gS3_state) != 0x1c);
    memset(&gS3_state, 0, sizeof(gS3_state));

    C2_HOOK_BUG_ON(sizeof(gS3_callbacks) != 0x10);
    memset(&gS3_callbacks, 0, sizeof(gS3_callbacks));

    S3Disable();
    S3StopMidi();
    S3StopCDA();
    gS3_effects_enabled = 0;
    gS3_sample_filter_func = NULL;
    gS3_sample_filter_disable_func = NULL;
    if (!PDS3Init()) {
        return eS3_error_digi_init;
    }
    gS3_opened_output_devices = 1;

    C2_HOOK_BUG_ON(sizeof(tS3_descriptor) != 0x4c);

    descriptor = S3MemAllocate(sizeof(tS3_descriptor), kMem_S3_sentinel);
    if (descriptor == NULL) {
        return eS3_error_memory;
    }
    memset(descriptor, 0, sizeof(tS3_descriptor));
    descriptor->sample_id = 0x261269;
    gS3_state.root_descriptor = descriptor;
    gS3_state.descriptors = gS3_state.root_descriptor;
    gS3_sound_dirname[0] = '\0';
    if (pSound_dirname != NULL) {

        PDExtractDirectory(dirpath, pPath);
        sprintf(gS3_sound_dirname, "%s%s", dirpath, pSound_dirname);
    }
    if (S3LoadSoundbank(pPath, pLow_memory_mode)) {
        return eS3_error_soundbank;
    }
    gS3_last_service_time = PDGetTotalTime();
    gS3_unbound_channels = NULL;
    gS3_last_unbound_channel = NULL;
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x10c
            mov dword ptr [ebp - 0x10c], edx
            mov dword ptr [ebp - 0x108], ecx
            call s3_debug_init
            push 0x1c
            push 0
            push offset gS3_state
            call memset
            add esp, 0xc
            push 0x10
            push 0
            push offset gS3_callbacks
            call memset
            add esp, 0xc
            call S3Disable
            call S3StopMidi
            call S3StopCDA
            mov dword ptr [gS3_effects_enabled], 0
            mov dword ptr [gS3_sample_filter_func], 0
            mov dword ptr [gS3_sample_filter_disable_func], 0
            call PDS3Init
            test eax, eax
            jne L0056524c
            mov eax, 1
            jmp L00565327
    L0056524c:
            mov dword ptr [gS3_opened_output_devices], 1
            mov dl, 0xe4
            mov ecx, 0x4c
            call S3MemAllocate
            mov dword ptr [ebp - 0x104], eax
            cmp dword ptr [ebp - 0x104], 0
            jne L0056527b
            mov eax, 3
            jmp L00565327
    L0056527b:
            push 0x4c
            push 0
            mov eax, dword ptr [ebp - 0x104]
            push eax
            call memset
            add esp, 0xc
            mov ecx, dword ptr [ebp - 0x104]
            mov dword ptr [ecx], 0x261269
            mov edx, dword ptr [ebp - 0x104]
            mov dword ptr [gS3_state+0x18], edx
            mov eax, dword ptr [gS3_state+0x18]
            mov dword ptr [gS3_state+0x14], eax
            mov byte ptr [gS3_sound_dirname], 0
            cmp dword ptr [ebp + 8], 0
            je L005652eb
            mov edx, dword ptr [ebp - 0x108]
            lea ecx, [ebp - 0x100]
            call PDExtractDirectory
            mov ecx, dword ptr [ebp + 8]
            push ecx
            lea edx, [ebp - 0x100]
            push edx
            push offset hitherYonTable+0x2c6c
            push offset gS3_sound_dirname
            call sprintf
            add esp, 0x10
    L005652eb:
            mov edx, dword ptr [ebp - 0x10c]
            mov ecx, dword ptr [ebp - 0x108]
            call S3LoadSoundbank
            test eax, eax
            je L00565307
            mov eax, 5
            jmp L00565327
    L00565307:
            call PDGetTotalTime
            mov dword ptr [gS3_last_service_time], eax
            mov dword ptr [gS3_unbound_channels], 0
            mov dword ptr [gS3_last_unbound_channel], 0
            xor eax, eax
    L00565327:
            mov esp, ebp
            pop ebp
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00564ce7
void C2_HOOK_FASTCALL S3Enable(void) {
    gS3_enabled = 1;
}

// FUNCTION: CARMA2_HW 0x00564cf6
void C2_HOOK_FASTCALL S3Disable(void) {
    S3StopAllOutletSounds();
    gS3_enabled = 0;
}

// FUNCTION: CARMA2_HW 0x00564bfd
int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* pChannel) {
    C2_HOOK_BUG_ON(sizeof(tS3_channel) != 120);

    if (pChannel->tag == 0) {
        return eS3_error_bad_stag;
    }
    pChannel->termination_reason = eS3_tr_stopped;
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    if (pChannel->type == 0) {
        if (pChannel->sound_source_ptr != NULL) {
            pChannel->sound_source_ptr->tag = 0;
            pChannel->sound_source_ptr->channel = 0;
            pChannel->sound_source_ptr->volume = 0;
        }
        if (PDS3StopSampleChannel(pChannel) == 0) {
            return eS3_error_function_failed;
        }
    } else if (pChannel->type == 1) {
        if (S3StopMIDIChannel(pChannel) != 0) {
            return eS3_error_function_failed;
        }
    } else if (pChannel->type == 2) {
        if (S3StopCDAChannel(pChannel) != 0) {
            return eS3_error_function_failed;
        }
    }

    if ((pChannel->descriptor->flags & 0x2) != 0) {
        S3ReleaseSound(pChannel->descriptor->sample_id);
    }
    pChannel->repetitions = 1;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x005698fc
int C2_HOOK_FASTCALL S3GetCountChannels(int pCount_channels_1, int pCount_channels_2) {
    return pCount_channels_2;
}

// FUNCTION: CARMA2_HW 0x005653c7
C2_NAKED tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_outlet* o;
    tS3_outlet* next_outlet;
    int nchannels;
    tS3_outlet* outlet;
    int channels_remaining;

    (void) next_outlet;
    nchannels = S3GetCountChannels(pCount_channels_1, pCount_channels_2);

    if (nchannels == 0) {
        gS3_last_error = eS3_error_channel_alloc;
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(tS3_outlet) != 0x20);

    outlet = S3MemAllocate(sizeof(tS3_outlet), kMem_S3_outlet);
    if (outlet == NULL) {
        gS3_last_error = eS3_error_memory;
        return NULL;
    }
    memset(outlet, 0, sizeof(tS3_outlet));
    channels_remaining = S3CreateOutletChannels(outlet, nchannels);
    if (channels_remaining == nchannels) {
        S3MemFree(outlet);
        return NULL;
    }

    if (gS3_state.outlets == NULL) {
        gS3_state.outlets = outlet;
    } else {
        o = gS3_state.outlets;
        for (; o->next != NULL;) {
            o = o->next;
        }
        o->next = outlet;
        outlet->prev = o;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, volume, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, max_channels, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, id, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, independent_pitch, 0xc);

    outlet->volume = 0xff;
    outlet->max_channels = nchannels - channels_remaining;
    outlet->id = gS3_state.next_outlet_id;
    gS3_state.next_outlet_id += 1;
    outlet->independent_pitch = gPD_S3_config.independent_pitch;
    gS3_state.count_outlets += 1;
    return outlet;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x1c
            mov dword ptr [ebp - 0x1c], edx
            mov dword ptr [ebp - 0x18], ecx
            mov edx, dword ptr [ebp - 0x1c]
            mov ecx, dword ptr [ebp - 0x18]
            call S3GetCountChannels
            mov dword ptr [ebp - 0xc], eax
            cmp dword ptr [ebp - 0xc], 0
            jne L005653f8
            mov dword ptr [gS3_last_error], 0xa
            xor eax, eax
            jmp L005654de
    L005653f8:
            mov dl, 0xe5
            mov ecx, 0x20
            call S3MemAllocate
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 8], 0
            jne L0056541e
            mov dword ptr [gS3_last_error], 3
            xor eax, eax
            jmp L005654de
    L0056541e:
            push 0x20
            push 0
            mov eax, dword ptr [ebp - 8]
            push eax
            call memset
            add esp, 0xc
            mov edx, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [ebp - 8]
            call S3CreateOutletChannels
            mov dword ptr [ebp - 4], eax
            mov ecx, dword ptr [ebp - 4]
            cmp ecx, dword ptr [ebp - 0xc]
            jne L00565453
            mov ecx, dword ptr [ebp - 8]
            call S3MemFree
            xor eax, eax
            jmp L005654de
    L00565453:
            mov edx, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 0x14], edx
            cmp dword ptr [ebp - 0x14], 0
            jne L0056546c
            mov eax, dword ptr [ebp - 8]
            mov dword ptr [gS3_state+0xc], eax
            jmp L00565492
    L0056546c:
            mov ecx, dword ptr [ebp - 0x14]
            cmp dword ptr [ecx + 0x18], 0
            je L00565480
            mov edx, dword ptr [ebp - 0x14]
            mov eax, dword ptr [edx + 0x18]
            mov dword ptr [ebp - 0x14], eax
            jmp L0056546c
    L00565480:
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x18], edx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0x14]
            mov dword ptr [eax + 0x14], ecx
    L00565492:
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [edx + 8], 0xff
            mov eax, dword ptr [ebp - 0xc]
            sub eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [gS3_state+0x8]
            mov dword ptr [edx], eax
            mov ecx, dword ptr [gS3_state+0x8]
            add ecx, 1
            mov dword ptr [gS3_state+0x8], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [gPD_S3_config+0x1c]
            mov dword ptr [edx + 0xc], eax
            mov ecx, dword ptr [gS3_state]
            add ecx, 1
            mov dword ptr [gS3_state], ecx
            mov eax, dword ptr [ebp - 8]
    L005654de:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005654e2
tS3_error_codes C2_NAKED C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pOutlet == NULL) {
        return eS3_error_bad_id;
    }
    S3UnbindChannels(pOutlet);
    if (pOutlet->prev != NULL) {
        pOutlet->prev->next = pOutlet->next;
    } else {
        gS3_state.outlets = pOutlet->next;
    }
    if (pOutlet->next != NULL) {
        pOutlet->next->prev = pOutlet->prev;
    }
    if (gS3_state.count_outlets != 0) {
        gS3_state.count_outlets -= 1;
        if (gS3_state.count_outlets == 0) {
            gS3_state.outlets = NULL;
        }
    }
    S3MemFree(pOutlet);
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [ebp - 0x14], 0
            jne L005654f8
            mov eax, 6
            jmp L0056556d
    L005654f8:
            mov ecx, dword ptr [ebp - 0x14]
            call S3UnbindChannels
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [eax + 0x14]
            mov dword ptr [ebp - 4], ecx
            mov edx, dword ptr [ebp - 0x14]
            mov eax, dword ptr [edx + 0x18]
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 4], 0
            je L00565523
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x18], edx
            jmp L0056552b
    L00565523:
            mov eax, dword ptr [ebp - 8]
            mov dword ptr [gS3_state+0xc], eax
    L0056552b:
            cmp dword ptr [ebp - 8], 0
            je L0056553a
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 4]
            mov dword ptr [ecx + 0x14], edx
    L0056553a:
            cmp dword ptr [gS3_state], 0
            je L00565563
            mov eax, dword ptr [gS3_state]
            sub eax, 1
            mov dword ptr [gS3_state], eax
            cmp dword ptr [gS3_state], 0
            jne L00565563
            mov dword ptr [gS3_state+0xc], 0
    L00565563:
            mov ecx, dword ptr [ebp - 0x14]
            call S3MemFree
            xor eax, eax
    L0056556d:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056494f
int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {
    tS3_channel* channel;

    if (gS3_enabled) {
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (pVolume < 10) {
            pVolume = 10;
        }
        if (pOutlet == NULL) {
            return 0;
        }
        pOutlet->volume = pVolume;
        channel = pOutlet->channel_list;
        for (; channel != NULL; ) {
            if (channel->active) {
                PDS3UpdateChannelVolume(channel);
            }
            channel = channel->next;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056575b
int C2_NAKED C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel* c;

    if (!gS3_enabled) {
        return 0;
    }
    for (c = pOutlet->channel_list; c != NULL; c = c->next) {
        // FUN_006a280(c);
        if (c->active) {
            c->spatial_sound = 0;
            S3StopChannel(c);
            c->needs_service = 1;
        }
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dword ptr [ebp - 8], ecx
            cmp dword ptr [gS3_enabled], 0
            je L005657b7
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 4], ecx
    L00565776:
            cmp dword ptr [ebp - 4], 0
            je L005657b3
            mov ecx, dword ptr [ebp - 4]
            call S3ServiceSpatialSound
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx], 0
            je L005657a8
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [eax + 0x14], 0
            mov ecx, dword ptr [ebp - 4]
            call S3StopChannel
            mov ecx, dword ptr [ebp - 4]
            mov dword ptr [ecx + 8], 1
    L005657a8:
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x64]
            mov dword ptr [ebp - 4], eax
            jmp L00565776
    L005657b3:
            xor eax, eax
            jmp L005657b9
    L005657b7:
            xor eax, eax
    L005657b9:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056671e
int C2_NAKED C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_sound_source* prev;
    tS3_sound_source* next;

    C2_HOOK_BUG_ON(sizeof(tS3_sound_source) != 68);

    if (!gS3_enabled) {
        return 0;
    }

    if (src == NULL) {
        return 0;
    }
    prev = src->prev;
    next = src->next;
    if (prev != NULL) {
        prev->next = next;
    } else {
        gS3_state.sources = src->next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    if (gS3_state.count_sources != 0) {
        gS3_state.count_sources--;
        if (gS3_state.count_sources == 0) {
            gS3_state.sources = NULL;
        }
    }
    S3StopSoundSource(src);
    S3MemFree(src);
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0xc
            mov dword ptr [ebp - 0xc], ecx
            cmp dword ptr [gS3_enabled], 0
            je L005667a9
            cmp dword ptr [ebp - 0xc], 0
            je L005667a9
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x14]
            mov dword ptr [ebp - 8], ecx
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x18]
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 8], 0
            je L00566759
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 4]
            mov dword ptr [ecx + 0x18], edx
            jmp L00566761
    L00566759:
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [gS3_state+0x10], eax
    L00566761:
            cmp dword ptr [ebp - 4], 0
            je L00566770
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x14], edx
    L00566770:
            cmp dword ptr [gS3_state+0x4], 0
            je L00566799
            mov eax, dword ptr [gS3_state+0x4]
            sub eax, 1
            mov dword ptr [gS3_state+0x4], eax
            cmp dword ptr [gS3_state+0x4], 0
            jne L00566799
            mov dword ptr [gS3_state+0x10], 0
    L00566799:
            mov ecx, dword ptr [ebp - 0xc]
            call S3StopSoundSource
            mov ecx, dword ptr [ebp - 0xc]
            call S3MemFree
    L005667a9:
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005667af
int C2_NAKED C2_HOOK_FASTCALL FUN_005667af(int pSound_id, tS3_sound_source* pSource) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    int priority;
    tS3_outlet* outlet;
    tS3_channel* channel;

    if (gS3_enabled != 0) {
        if (pSource == NULL) {
            return 0;
        }
        outlet = pSource->bound_outlet;
        descriptor = S3GetDescriptorByID(pSound_id);
        if (descriptor == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return 0;
        }
        if (descriptor->buffer_description == NULL) {
            return eS3_error_nonexistant_source;
        }
        if (descriptor->type != 0) {
            return 0;
        }
        memset(&gS3_channel_template, 0, sizeof(gS3_channel_template));
        gS3_channel_template.sound_source_ptr = pSource;
        if (pSource->velocity_ptr == NULL) {
            S3CopyVector3(&gS3_channel_template.prev_position, pSource->position_ptr, pSource->brender_vector);
        }
        S3CalculateRandomizedFields(&gS3_channel_template, descriptor);
        gS3_channel_template.max_distance_squared = 150.0f;
        gS3_channel_template.source_volume = gS3_channel_template.source_volume;
        gS3_channel_template.source_rate = gS3_channel_template.rate;
        if (S3Calculate3D(&gS3_channel_template, 0) != 0) {
            priority = S3CalculatePriority(gS3_channel_template.volume_multiplier, descriptor->priority);
            channel = S3AllocateChannel(outlet, priority);
            if (channel == NULL) {
                gS3_last_error = eS3_error_channel_alloc;
                return 0;
            }
            channel->volume_multiplier = gS3_channel_template.volume_multiplier;
            channel->field_0x28 = gS3_channel_template.field_0x28;
            channel->rate = gS3_channel_template.rate;
            channel->position = gS3_channel_template.position;
            channel->velocity = gS3_channel_template.velocity;
            channel->prev_position = gS3_channel_template.prev_position;
            channel->source_volume = gS3_channel_template.source_volume;
            channel->source_rate = gS3_channel_template.source_rate;
            if ((descriptor->buffer_description == NULL || (descriptor->flags & 0x2) != 0)
                    && S3LoadSample(pSound_id) == 0) {
                gS3_last_error = eS3_error_load_sound;
                channel->needs_service = 1;
                return 0;
            }
            channel->max_distance_squared = 150.0f;
            channel->spatial_sound = 1;
            channel->sound_source_ptr = pSource;
            channel->descriptor = descriptor;
            channel->repetitions = descriptor->repeat_rate <= 0 ? 0 : descriptor->repeat_rate;
            channel->needs_service = 0;
            channel->termination_reason = 0;
            channel->tag = S3GenerateTag(outlet);
            S3ExecuteSampleFilterFuncs(channel);
            if (((int (C2_HOOK_FASTCALL *)(tS3_channel*, tS3_descriptor*))S3PlaySample)(channel, descriptor) == 0) {
                channel->needs_service = 1;
                gS3_last_error = eS3_error_start_sound;
                return 0;
            }
            return channel->tag;
        }
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x1c
            mov dword ptr [ebp - 0x18], edx
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00566a4e
            cmp dword ptr [ebp - 0x18], 0
            jne L005667d5
            xor eax, eax
            jmp L00566a50
    L005667d5:
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0xc], ecx
            mov ecx, dword ptr [ebp - 0x14]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00566800
            mov dword ptr [gS3_last_error], 6
            xor eax, eax
            jmp L00566a50
    L00566800:
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 0x40], 0
            jne L00566813
            mov eax, 0x13
            jmp L00566a50
    L00566813:
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 4], 0
            je L00566823
            xor eax, eax
            jmp L00566a50
    L00566823:
            push 0x78
            push 0
            push offset gS3_channel_template
            call memset
            add esp, 0xc
            mov ecx, dword ptr [ebp - 0x18]
            mov dword ptr [gS3_channel_template+0x74], ecx
            mov edx, dword ptr [ebp - 0x18]
            cmp dword ptr [edx + 0xc], 0
            jne L0056685d
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 4]
            push ecx
            mov edx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [edx + 8]
            mov ecx, offset gS3_channel_template+0x44
            call S3CopyVector3
    L0056685d:
            mov edx, dword ptr [ebp - 4]
            mov ecx, offset gS3_channel_template
            call S3CalculateRandomizedFields
            mov dword ptr [gS3_channel_template+0x58], 0x43160000
            mov eax, dword ptr [gS3_channel_template+0x50]
            mov dword ptr [gS3_channel_template+0x50], eax
            mov ecx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [gS3_channel_template+0x54], ecx
            xor edx, edx
            mov ecx, offset gS3_channel_template
            call S3Calculate3D
            test eax, eax
            je L00566a4e
            mov edx, dword ptr [ebp - 4]
            mov edx, dword ptr [edx + 8]
            mov ecx, dword ptr [gS3_channel_template+0x24]
            call S3CalculatePriority
            mov dword ptr [ebp - 8], eax
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0xc]
            call S3AllocateChannel
            mov dword ptr [ebp - 0x10], eax
            cmp dword ptr [ebp - 0x10], 0
            jne L005668d7
            mov dword ptr [gS3_last_error], 0xa
            xor eax, eax
            jmp L00566a50
    L005668d7:
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [gS3_channel_template+0x24]
            mov dword ptr [eax + 0x24], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [gS3_channel_template+0x28]
            mov dword ptr [edx + 0x28], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [ebp - 0x10]
            add eax, 0x2c
            mov ecx, dword ptr [gS3_channel_template+0x2c]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [gS3_channel_template+0x30]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [gS3_channel_template+0x34]
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [ebp - 0x10]
            add edx, 0x38
            mov eax, dword ptr [gS3_channel_template+0x38]
            mov dword ptr [edx], eax
            mov ecx, dword ptr [gS3_channel_template+0x3c]
            mov dword ptr [edx + 4], ecx
            mov eax, dword ptr [gS3_channel_template+0x40]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp - 0x10]
            add ecx, 0x44
            mov edx, dword ptr [gS3_channel_template+0x44]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [gS3_channel_template+0x48]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [gS3_channel_template+0x4c]
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [gS3_channel_template+0x50]
            mov dword ptr [eax + 0x50], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [gS3_channel_template+0x54]
            mov dword ptr [edx + 0x54], eax
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            je L00566984
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L005669ab
    L00566984:
            mov ecx, dword ptr [ebp - 0x14]
            call S3LoadSample
            test eax, eax
            jne L005669ab
            mov dword ptr [gS3_last_error], 8
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 1
            xor eax, eax
            jmp L00566a50
    L005669ab:
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x58], 0x43160000
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 0x14], 1
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ebp - 0x18]
            mov dword ptr [ecx + 0x74], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 4]
            mov dword ptr [eax + 0x60], ecx
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 0xc], 0
            jg L005669e3
            mov dword ptr [ebp - 0x1c], 0
            jmp L005669ec
    L005669e3:
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ebp - 0x1c], ecx
    L005669ec:
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp - 0x1c]
            mov dword ptr [edx + 0xc], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 0
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 4], 0
            mov ecx, dword ptr [ebp - 0xc]
            call S3GenerateTag
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 0x1c], eax
            mov ecx, dword ptr [ebp - 0x10]
            call S3ExecuteSampleFilterFuncs
            mov edx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlaySample
            test eax, eax
            jne L00566a46
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 8], 1
            mov dword ptr [gS3_last_error], 9
            xor eax, eax
            jmp L00566a50
    L00566a46:
            mov eax, dword ptr [ebp - 0x10]
            mov eax, dword ptr [eax + 0x1c]
            jmp L00566a50
    L00566a4e:
            xor eax, eax
    L00566a50:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00566a54
int C2_NAKED C2_HOOK_FASTCALL FUN_00566a54(int pSound_id, tS3_sound_source* pSource, int pVolume, int pPitch, int pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    int priority;
    tS3_outlet* outlet;
    tS3_channel* channel;
    int repetitions;
    float rate_as_float;
    float scale_as_float;

    if (!gS3_enabled) {
        return 0;
    }
    if (pSource == NULL) {
        return 0;
    }
    outlet = pSource->bound_outlet;
    descriptor = S3GetDescriptorByID(pSound_id);
    if (descriptor == NULL) {
        gS3_last_error = eS3_error_bad_id;
        return 0;
    }
    if (descriptor->buffer_description == NULL) {
        return eS3_error_nonexistant_source;
    }
    if (descriptor->type != 0) {
        return 0;
    }
    if (pVolume > 0xff) {
        pVolume = 0xff;
    }
    if (pVolume < 0) {
        pVolume = S3IRandomBetween__dup(descriptor->min_volume, descriptor->max_volume, 0x80);
    }

    memset(&gS3_channel_template, 0, 0x78);
    gS3_channel_template.source_volume = pVolume;
    gS3_channel_template.volume_multiplier = pVolume;
    gS3_channel_template.sound_source_ptr = pSource;
    if (pSource->velocity_ptr == NULL) {
        S3CopyVector3(&gS3_channel_template.prev_position,
                pSource->position_ptr,
                pSource->brender_vector);
    }
    gS3_channel_template.rate = S3IRandomBetweenLog(descriptor->min_pitch,
            descriptor->max_pitch,
            descriptor->buffer_description->sample_rate);
    if (pPitch == -1) {
        pPitch = 0x10000;
    }
    if (pSpeed == -1) {
        pSpeed = 0x10000;
    }
    rate_as_float = (float)gS3_channel_template.rate;
    scale_as_float = (float)ldexp((double)pPitch, -16);
    gS3_channel_template.rate = (int)(scale_as_float * rate_as_float);
    if (!outlet->independent_pitch) {
        rate_as_float = (float)gS3_channel_template.rate;
        scale_as_float = (float)ldexp((double)pSpeed, -16);
        gS3_channel_template.rate = (int)(scale_as_float * rate_as_float);
    }
    gS3_channel_template.source_rate = gS3_channel_template.rate;
    gS3_channel_template.max_distance_squared = 150.0f;

    if (S3Calculate3D(&gS3_channel_template, 0)) {
        priority = S3CalculatePriority(gS3_channel_template.volume_multiplier, descriptor->priority);
        channel = S3AllocateChannel(outlet, priority);
        if (channel == NULL) {
            gS3_last_error = eS3_error_channel_alloc;
            return 0;
        }
        if ((descriptor->buffer_description == NULL || (descriptor->flags & 0x2) != 0)
                && S3LoadSample(pSound_id) == 0) {
            gS3_last_error = eS3_error_load_sound;
            channel->needs_service = 1;
            return 0;
        }
        channel->volume_multiplier = gS3_channel_template.volume_multiplier;
        channel->field_0x28 = gS3_channel_template.field_0x28;
        channel->rate = gS3_channel_template.rate;
        channel->position = gS3_channel_template.position;
        channel->velocity = gS3_channel_template.velocity;
        channel->prev_position = gS3_channel_template.prev_position;
        channel->source_volume = gS3_channel_template.source_volume;
        channel->source_rate = gS3_channel_template.source_rate;
        channel->max_distance_squared = gS3_channel_template.max_distance_squared;
        channel->spatial_sound = 1;
        channel->sound_source_ptr = pSource;
        channel->descriptor = descriptor;
        channel->tag = S3GenerateTag(outlet);
        if (descriptor->repeat_rate > 0) {
            repetitions = descriptor->repeat_rate;
        } else {
            repetitions = 0;
        }
        channel->repetitions = repetitions;
        channel->needs_service = 0;
        channel->termination_reason = 0;
        S3ExecuteSampleFilterFuncs(channel);
        if (((int (C2_HOOK_FASTCALL *)(tS3_channel*, tS3_descriptor*))S3PlaySample)(channel, descriptor) == 0) {
            channel->needs_service = 1;
            gS3_last_error = eS3_error_start_sound;
            return 0;
        }
        return channel->tag;
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x2c
            mov dword ptr [ebp - 0x18], edx
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00566db9
            cmp dword ptr [ebp - 0x18], 0
            jne L00566a7a
            xor eax, eax
            jmp L00566dbb
    L00566a7a:
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0xc], ecx
            mov ecx, dword ptr [ebp - 0x14]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00566aa5
            mov dword ptr [gS3_last_error], 6
            xor eax, eax
            jmp L00566dbb
    L00566aa5:
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 0x40], 0
            jne L00566ab8
            mov eax, 0x13
            jmp L00566dbb
    L00566ab8:
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 4], 0
            je L00566ac8
            xor eax, eax
            jmp L00566dbb
    L00566ac8:
            cmp dword ptr [ebp + 8], 0xff
            jle L00566ad8
            mov dword ptr [ebp + 8], 0xff
    L00566ad8:
            cmp dword ptr [ebp + 8], 0
            jge L00566af7
            push 0x80
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x14]
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x10]
            call S3IRandomBetween__dup
            mov dword ptr [ebp + 8], eax
    L00566af7:
            push 0x78
            push 0
            push offset gS3_channel_template
            call memset
            add esp, 0xc
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [gS3_channel_template+0x50], ecx
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [gS3_channel_template+0x24], edx
            mov eax, dword ptr [ebp - 0x18]
            mov dword ptr [gS3_channel_template+0x74], eax
            mov ecx, dword ptr [ebp - 0x18]
            cmp dword ptr [ecx + 0xc], 0
            jne L00566b42
            mov edx, dword ptr [ebp - 0x18]
            mov eax, dword ptr [edx + 4]
            push eax
            mov ecx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [ecx + 8]
            mov ecx, offset gS3_channel_template+0x44
            call S3CopyVector3
    L00566b42:
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x40]
            mov ecx, dword ptr [eax + 4]
            push ecx
            mov edx, dword ptr [ebp - 4]
            mov edx, dword ptr [edx + 0x1c]
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x18]
            call S3IRandomBetweenLog
            mov dword ptr [gS3_channel_template+0x10], eax
            cmp dword ptr [ebp + 0xc], -1
            jne L00566b6f
            mov dword ptr [ebp + 0xc], 0x10000
    L00566b6f:
            cmp dword ptr [ebp + 0x10], -1
            jne L00566b7c
            mov dword ptr [ebp + 0x10], 0x10000
    L00566b7c:
            fild dword ptr [gS3_channel_template+0x10]
            fstp dword ptr [ebp - 0x1c]
            push -0x10
            fild dword ptr [ebp + 0xc]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fst dword ptr [ebp - 0x20]
            fmul dword ptr [ebp - 0x1c]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 0xc], 0
            jne L00566bdd
            fild dword ptr [gS3_channel_template+0x10]
            fstp dword ptr [ebp - 0x24]
            push -0x10
            fild dword ptr [ebp + 0x10]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fst dword ptr [ebp - 0x28]
            fmul dword ptr [ebp - 0x24]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
    L00566bdd:
            mov edx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [gS3_channel_template+0x54], edx
            mov dword ptr [gS3_channel_template+0x58], 0x43160000
            xor edx, edx
            mov ecx, offset gS3_channel_template
            call S3Calculate3D
            test eax, eax
            je L00566db9
            mov eax, dword ptr [ebp - 4]
            mov edx, dword ptr [eax + 8]
            mov ecx, dword ptr [gS3_channel_template+0x24]
            call S3CalculatePriority
            mov dword ptr [ebp - 8], eax
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0xc]
            call S3AllocateChannel
            mov dword ptr [ebp - 0x10], eax
            cmp dword ptr [ebp - 0x10], 0
            jne L00566c40
            mov dword ptr [gS3_last_error], 0xa
            xor eax, eax
            jmp L00566dbb
    L00566c40:
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            je L00566c56
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L00566c7d
    L00566c56:
            mov ecx, dword ptr [ebp - 0x14]
            call S3LoadSample
            test eax, eax
            jne L00566c7d
            mov dword ptr [gS3_last_error], 8
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 1
            xor eax, eax
            jmp L00566dbb
    L00566c7d:
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [gS3_channel_template+0x24]
            mov dword ptr [edx + 0x24], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [gS3_channel_template+0x28]
            mov dword ptr [ecx + 0x28], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [eax + 0x10], ecx
            mov edx, dword ptr [ebp - 0x10]
            add edx, 0x2c
            mov eax, dword ptr [gS3_channel_template+0x2c]
            mov dword ptr [edx], eax
            mov ecx, dword ptr [gS3_channel_template+0x30]
            mov dword ptr [edx + 4], ecx
            mov eax, dword ptr [gS3_channel_template+0x34]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp - 0x10]
            add ecx, 0x38
            mov edx, dword ptr [gS3_channel_template+0x38]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [gS3_channel_template+0x3c]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [gS3_channel_template+0x40]
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [ebp - 0x10]
            add eax, 0x44
            mov ecx, dword ptr [gS3_channel_template+0x44]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [gS3_channel_template+0x48]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [gS3_channel_template+0x4c]
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [gS3_channel_template+0x50]
            mov dword ptr [edx + 0x50], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [gS3_channel_template+0x54]
            mov dword ptr [ecx + 0x54], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [gS3_channel_template+0x58]
            mov dword ptr [eax + 0x58], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x14], 1
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 0x18]
            mov dword ptr [eax + 0x74], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [edx + 0x60], eax
            mov ecx, dword ptr [ebp - 0xc]
            call S3GenerateTag
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 0x1c], eax
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 0xc], 0
            jg L00566d5c
            mov dword ptr [ebp - 0x2c], 0
            jmp L00566d65
    L00566d5c:
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ebp - 0x2c], ecx
    L00566d65:
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp - 0x2c]
            mov dword ptr [edx + 0xc], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 0
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 4], 0
            mov ecx, dword ptr [ebp - 0x10]
            call S3ExecuteSampleFilterFuncs
            mov edx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlaySample
            test eax, eax
            jne L00566db1
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 8], 1
            mov dword ptr [gS3_last_error], 9
            xor eax, eax
            jmp L00566dbb
    L00566db1:
            mov ecx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ecx + 0x1c]
            jmp L00566dbb
    L00566db9:
            xor eax, eax
    L00566dbb:
            mov esp, ebp
            pop ebp
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005657bd
int C2_NAKED C2_HOOK_FASTCALL S3StopAllOutletSounds(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_outlet* o;

    if (!gS3_enabled) {
        return 0;
    }

    for (o = gS3_state.outlets; o != NULL; o = o->next) {
        S3StopOutletSound(o);
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            push ecx
            cmp dword ptr [gS3_enabled], 0
            je L005657ef
            mov eax, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 4], eax
    L005657d2:
            cmp dword ptr [ebp - 4], 0
            je L005657eb
            mov ecx, dword ptr [ebp - 4]
            call S3StopOutletSound
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [ebp - 4], edx
            jmp L005657d2
    L005657eb:
            xor eax, eax
            jmp L005657f1
    L005657ef:
            xor eax, eax
    L005657f1:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00564ae2
tS3_error_codes C2_HOOK_FASTCALL S3StopSound(int pTag) {
    tS3_channel* channel;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, source_volume, 0x50);

    if (gS3_enabled) {
        if (pTag == 0) {
            return eS3_error_bad_stag;
        }
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        channel->termination_reason = 1;
        channel->source_volume = 0;
        if (channel->active) {
            channel->needs_service = 1;
        }
        if (channel->type == 0) {
            if (channel->sound_source_ptr != NULL) {
                channel->sound_source_ptr->tag = 0;
                channel->sound_source_ptr->channel = NULL;
                channel->sound_source_ptr->volume = 0;
            }
            if (PDS3StopSampleChannel(channel) == 0) {
                return eS3_error_function_failed;
            }
        } else if (channel->type == 1) {
            if (S3StopMIDIChannel(channel) != 0) {
                return eS3_error_function_failed;
            }
        } else if (channel->type == 2) {
            if (S3StopCDAChannel(channel) != 0) {
                return eS3_error_function_failed;
            }
        }
        if (channel->descriptor->flags & 0x2) {
            S3ReleaseSound(channel->descriptor->sample_id);
        }
        channel->repetitions = 1;
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00564358
void C2_NAKED C2_HOOK_FASTCALL S3Service(int pInside_cockpit, int pThings_moved) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 now;
    int spatial_serviced = 0;
    tS3_outlet* o;
    tS3_channel* c;

    gS3_inside_cockpit = pInside_cockpit;
    if (gS3_enabled) {
        now = PDGetTotalTime();
        gS3_delta_time = now - gS3_last_service_time;
        gS3_last_service_time = now;
        PDS3ServiceCDA(gS3_delta_time);
        if (pThings_moved == 1) {
            S3UpdateListenerVectors();
            S3UpdateSourceVectors();
        }
        for (o = gS3_state.outlets; o != NULL; o = o->next) {
            for (c = o->channel_list; c != NULL; c = c->next) {
                if (c->needs_service) {
                    c->needs_service = 0;
                    if (c->descriptor != NULL && c->descriptor->flags == 2) {
                        S3ReleaseSound(c->descriptor->sample_id);
                    }
                    c->active = 0;
                    if (c->type != 1) {
                        c->tag = 0;
                    }
                } else {
                    if (c->spatial_sound && c->active) {
                        if (S3Service3D(c) == 0) {
                            if (c->sound_source_ptr != NULL) {
                                if (c->sound_source_ptr->ambient) {
                                    S3UpdateSoundSource(NULL, -1, c->sound_source_ptr, -1.f, -1, -1, 0, -1, -1);
                                }
                            } else {
                                S3StopChannel(c);
                            }
                        }
                        else if (c->sound_source_ptr != NULL && c->sound_source_ptr->ambient && !S3SoundStillPlaying(c->tag)) {
                            S3UpdateSoundSource(NULL, -1, c->sound_source_ptr, -1.f, -1, -1, 0, -1, -1);
                        }
                    } else if (c->type == 1 && c->active) {
                        S3ServiceMIDIChannel(c);
                    }
                }
                if (pThings_moved < 2 && gS3_last_service_time_spatial < gS3_last_service_time)  {
                    spatial_serviced = 1;
                    if (!c->active && c->spatial_sound == 2) {
                        S3ServiceSpatialSound(c);
                    }
                }
            }
        }
        if (spatial_serviced) {
            gS3_last_service_time_spatial = gS3_last_service_time;
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x18
            mov dword ptr [ebp - 0x18], edx
            mov dword ptr [ebp - 0x14], ecx
            mov dword ptr [ebp - 4], 0
            mov eax, dword ptr [ebp - 0x14]
            mov dword ptr [gS3_inside_cockpit], eax
            cmp dword ptr [gS3_enabled], 0
            je L00564561
            call PDGetTotalTime
            mov dword ptr [ebp - 0x10], eax
            mov ecx, dword ptr [ebp - 0x10]
            sub ecx, dword ptr [gS3_last_service_time]
            mov dword ptr [gS3_delta_time], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [gS3_last_service_time], edx
            mov ecx, dword ptr [gS3_delta_time]
            call PDS3ServiceCDA
            cmp dword ptr [ebp - 0x18], 1
            jne L005643bb
            call S3UpdateListenerVectors
            call S3UpdateSourceVectors
    L005643bb:
            mov eax, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 8], eax
            jmp L005643ce
    L005643c5:
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [ebp - 8], edx
    L005643ce:
            cmp dword ptr [ebp - 8], 0
            je L0056454f
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0xc], ecx
            jmp L005643ec
    L005643e3:
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x64]
            mov dword ptr [ebp - 0xc], eax
    L005643ec:
            cmp dword ptr [ebp - 0xc], 0
            je L0056454a
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 8], 0
            je L0056444c
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 8], 0
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x60], 0
            je L0056442b
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x60]
            cmp dword ptr [edx + 0x30], 2
            jne L0056442b
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x60]
            mov ecx, dword ptr [ecx]
            call S3ReleaseSound
    L0056442b:
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx], 0
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x20], 1
            je L00564447
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ecx + 0x1c], 0
    L00564447:
            jmp L00564511
    L0056444c:
            mov edx, dword ptr [ebp - 0xc]
            cmp dword ptr [edx + 0x14], 0
            je L005644f8
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax], 0
            je L005644f8
            mov ecx, dword ptr [ebp - 0xc]
            call S3Service3D
            test eax, eax
            jne L005644b2
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 0x74], 0
            je L005644a8
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x74]
            cmp dword ptr [eax + 0x1c], 0
            je L005644a6
            push -1
            push -1
            push 0
            push -1
            push -1
            push 0xbf800000
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x74]
            push edx
            or edx, 0xffffffff
            xor ecx, ecx
            call S3UpdateSoundSource
    L005644a6:
            jmp L005644b0
    L005644a8:
            mov ecx, dword ptr [ebp - 0xc]
            call S3StopChannel
    L005644b0:
            jmp L005644f6
    L005644b2:
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x74], 0
            je L005644f6
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x74]
            cmp dword ptr [edx + 0x1c], 0
            je L005644f6
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x1c]
            call S3SoundStillPlaying
            test eax, eax
            jne L005644f6
            push -1
            push -1
            push 0
            push -1
            push -1
            push 0xbf800000
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x74]
            push edx
            or edx, 0xffffffff
            xor ecx, ecx
            call S3UpdateSoundSource
    L005644f6:
            jmp L00564511
    L005644f8:
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x20], 1
            jne L00564511
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx], 0
            je L00564511
            mov ecx, dword ptr [ebp - 0xc]
            call S3ServiceMIDIChannel
    L00564511:
            cmp dword ptr [ebp - 0x18], 2
            jge L00564545
            mov edx, dword ptr [gS3_last_service_time_spatial]
            cmp edx, dword ptr [gS3_last_service_time]
            jae L00564545
            mov dword ptr [ebp - 4], 1
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax], 0
            jne L00564545
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 0x14], 2
            jne L00564545
            mov ecx, dword ptr [ebp - 0xc]
            call S3ServiceSpatialSound
    L00564545:
            jmp L005643e3
    L0056454a:
            jmp L005643c5
    L0056454f:
            cmp dword ptr [ebp - 4], 0
            je L00564561
            mov edx, dword ptr [gS3_last_service_time]
            mov dword ptr [gS3_last_service_time_spatial], edx
    L00564561:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056532d
int C2_NAKED C2_HOOK_FASTCALL S3DisableSound(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    tS3_outlet* outlet;
    tS3_descriptor* next_descriptor;
    tS3_outlet* next_outlet;

    S3StopAllOutletSounds();
    S3StopMidi();
    S3StopCDA();
    if (gS3_enabled) {

        S3Disable();

        descriptor = gS3_state.descriptors;
        while (descriptor != NULL) {
            next_descriptor = descriptor->next;
            S3ReleaseSound(descriptor->sample_id);
            S3MemFree(descriptor);
            descriptor = next_descriptor;
        }

        outlet = gS3_state.outlets;
        while (outlet != NULL) {
            next_outlet = outlet->next;
            S3ReleaseOutlet(outlet);
            outlet = next_outlet;
        }

        S3FreeUnboundChannels();
    }
    if (gS3_opened_output_devices) {
        PDS3Stop();
    }
    s3_debug_disable_sound();
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            call S3StopAllOutletSounds
            call S3StopMidi
            call S3StopCDA
            cmp dword ptr [gS3_enabled], 0
            je L005653ae
            call S3Disable
            mov eax, dword ptr [gS3_state+0x14]
            mov dword ptr [ebp - 4], eax
    L00565358:
            cmp dword ptr [ebp - 4], 0
            je L00565381
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x2c]
            mov dword ptr [ebp - 8], edx
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax]
            call S3ReleaseSound
            mov ecx, dword ptr [ebp - 4]
            call S3MemFree
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [ebp - 4], ecx
            jmp L00565358
    L00565381:
            mov edx, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 0x10], edx
    L0056538a:
            cmp dword ptr [ebp - 0x10], 0
            je L005653a9
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [ebp - 0xc], ecx
            mov ecx, dword ptr [ebp - 0x10]
            call S3ReleaseOutlet
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [ebp - 0x10], edx
            jmp L0056538a
    L005653a9:
            call S3FreeUnboundChannels
    L005653ae:
            cmp dword ptr [gS3_opened_output_devices], 0
            je L005653bc
            call PDS3Stop
    L005653bc:
            call s3_debug_disable_sound
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00565b70
void C2_HOOK_FASTCALL S3EnableCDA(void) {
    gS3_CDA_enabled = 1;
}

// FUNCTION: CARMA2_HW 0x00565571
int C2_HOOK_FASTCALL S3CreateOutletChannels(tS3_outlet* outlet, int pChannel_count) {
    tS3_channel* chan;
    tS3_channel* last_chan;

    last_chan = NULL;
    while (pChannel_count != 0) {
        C2_HOOK_BUG_ON(sizeof(tS3_channel) != 0x78);

        chan = S3MemAllocate(sizeof(tS3_channel), kMem_S3_channel_00593a58);
        if (chan == NULL) {
            return pChannel_count;
        }
        memset(chan, 0, sizeof(tS3_channel));
        chan->owner_outlet = outlet;

        if (!S3CreateTypeStructs(chan)) {
            S3MemFree(chan);
            return pChannel_count;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, next, 0x64);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, channel_list, 0x10);

        if (last_chan == NULL) {
            outlet->channel_list = chan;
        } else {
            last_chan->next = chan;
        }
        last_chan = chan;
        pChannel_count--;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00568c2c
C2_NAKED void* C2_HOOK_FASTCALL S3LoadSoundBankFile(const char* pPath) {
#ifndef CARPOCALYPSE2_MATCHING
    char path[512];
    char lowmem_path[512];
    char line[512];
    int bytes_read;
    char* buffer;
    int file_size;
    FILE* f;
    struct_c2_stat32 stat;

    if (gS3_low_memory_mode) {

        gS3_last_error = eS3_error_readfile;
        sprintf(lowmem_path, "DATA%sSOUNDS%s%s", gS3_path_separator, gS3_path_separator, pPath);
        f = S3_low_memory_fopen(lowmem_path, "rb");
        if (f == NULL) {
            return NULL;
        }
        file_size = S3GetFileSize(f);
        buffer = S3MemAllocate(file_size + 1, kMem_S3_sample);
        if (buffer == NULL) {
            fclose(f);
            gS3_last_error = eS3_error_memory;
            return NULL;
        }
        buffer[file_size] = '\0';
        bytes_read = fread(buffer, 1, file_size, f);
        if (bytes_read != file_size) {
#ifdef CARPOCALYPSE2_FIX_BUGS
            fclose(f);
#endif
            gS3_last_error = eS3_error_readfile;
            return NULL;
        }
        gS3_soundbank_buffer = buffer;
        gS3_soundbank_buffer_len = file_size;
        fclose(f);
    } else {
        int fd;

        C2_HOOK_BUG_ON(_O_BINARY != 0x8000);
        if (gS3_sound_dirname[0] != '\0') {

            PDExtractFilename(path, pPath);
            sprintf(line, "%s%s%s", gS3_sound_dirname, gS3_path_separator, path);
            fd = c2_open(line, _O_BINARY);
        } else {
            fd = c2_open(pPath, _O_BINARY);
        }
        if (fd == -1) {
            gS3_last_error = eS3_error_readfile;
            return NULL;
        }
        if (c2_fstat32(fd, &stat) != 0) {
#ifdef CARPOCALYPSE2_FIX_BUGS
            c2_close(fd);
#endif
            gS3_last_error = eS3_error_readfile;
            return NULL;
        }
        buffer = S3MemAllocate(stat.st_size + 1, kMem_S3_sample);
        if (buffer == NULL) {
            c2_close(fd);
            gS3_last_error = eS3_error_memory;
            return NULL;
        }
        buffer[stat.st_size] = '\0';
        bytes_read = c2_read(fd, buffer, stat.st_size);
        if (bytes_read != stat.st_size) {
#ifdef CARPOCALYPSE2_FIX_BUGS
            c2_close(fd);
#endif
            gS3_last_error = eS3_error_readfile;
            return NULL;
        }
        gS3_soundbank_buffer = buffer;
        gS3_soundbank_buffer_len = stat.st_size;
        c2_close(fd);
    }
    return buffer;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x638
            mov dword ptr [ebp - 0x638], ecx
            cmp dword ptr [gS3_low_memory_mode], 0
            je L00568d60
            mov dword ptr [gS3_last_error], 4
            mov eax, dword ptr [ebp - 0x638]
            push eax
            push offset gS3_path_separator
            push offset gS3_path_separator
            push offset gS3_CDA_enabled+0xb4
            lea ecx, [ebp - 0x400]
            push ecx
            call sprintf
            add esp, 0x14
            mov edx, offset gS3_CDA_enabled+0xc8
            lea ecx, [ebp - 0x400]
            call S3_low_memory_fopen
            mov dword ptr [ebp - 0x634], eax
            cmp dword ptr [ebp - 0x634], 0
            jne L00568c9d
            xor eax, eax
            jmp L00568ec7
    L00568c9d:
            mov ecx, dword ptr [ebp - 0x634]
            call S3GetFileSize
            mov dword ptr [ebp - 0x630], eax
            mov dl, 0xe1
            mov ecx, dword ptr [ebp - 0x630]
            add ecx, 1
            call S3MemAllocate
            mov dword ptr [ebp - 0x628], eax
            cmp dword ptr [ebp - 0x628], 0
            jne L00568ced
            mov edx, dword ptr [ebp - 0x634]
            push edx
            call fclose
            add esp, 4
            mov dword ptr [gS3_last_error], 3
            xor eax, eax
            jmp L00568ec7
    L00568ced:
            mov eax, dword ptr [ebp - 0x628]
            add eax, dword ptr [ebp - 0x630]
            mov byte ptr [eax], 0
            mov ecx, dword ptr [ebp - 0x634]
            push ecx
            mov edx, dword ptr [ebp - 0x630]
            push edx
            push 1
            mov eax, dword ptr [ebp - 0x628]
            push eax
            call fread
            add esp, 0x10
            cmp eax, dword ptr [ebp - 0x630]
            je L00568d34
            mov dword ptr [gS3_last_error], 4
            xor eax, eax
            jmp L00568ec7
    L00568d34:
            mov ecx, dword ptr [ebp - 0x628]
            mov dword ptr [gS3_soundbank_buffer], ecx
            mov edx, dword ptr [ebp - 0x630]
            mov dword ptr [gS3_soundbank_buffer_len], edx
            mov eax, dword ptr [ebp - 0x634]
            push eax
            call fclose
            add esp, 4
            jmp L00568ec1
    L00568d60:
            movsx ecx, byte ptr [gS3_sound_dirname]
            test ecx, ecx
            je L00568dbd
            mov edx, dword ptr [ebp - 0x638]
            lea ecx, [ebp - 0x200]
            call PDExtractFilename
            lea edx, [ebp - 0x200]
            push edx
            push offset gS3_path_separator
            push offset gS3_sound_dirname
            push offset gS3_CDA_enabled+0xcc
            lea eax, [ebp - 0x600]
            push eax
            call sprintf
            add esp, 0x14
            push 0x8000
            lea ecx, [ebp - 0x600]
            push ecx
            call _open
            add esp, 8
            mov dword ptr [ebp - 0x62c], eax
            jmp L00568dd7
    L00568dbd:
            push 0x8000
            mov edx, dword ptr [ebp - 0x638]
            push edx
            call _open
            add esp, 8
            mov dword ptr [ebp - 0x62c], eax
    L00568dd7:
            cmp dword ptr [ebp - 0x62c], -1
            jne L00568df1
            mov dword ptr [gS3_last_error], 4
            xor eax, eax
            jmp L00568ec7
    L00568df1:
            lea eax, [ebp - 0x624]
            push eax
            mov ecx, dword ptr [ebp - 0x62c]
            push ecx
            call _fstat
            add esp, 8
            test eax, eax
            je L00568e1c
            mov dword ptr [gS3_last_error], 4
            xor eax, eax
            jmp L00568ec7
    L00568e1c:
            mov dl, 0xe1
            mov ecx, dword ptr [ebp - 0x610]
            add ecx, 1
            call S3MemAllocate
            mov dword ptr [ebp - 0x628], eax
            cmp dword ptr [ebp - 0x628], 0
            jne L00568e58
            mov edx, dword ptr [ebp - 0x62c]
            push edx
            call _close
            add esp, 4
            mov dword ptr [gS3_last_error], 3
            xor eax, eax
            jmp L00568ec7
    L00568e58:
            mov eax, dword ptr [ebp - 0x628]
            add eax, dword ptr [ebp - 0x610]
            mov byte ptr [eax], 0
            mov ecx, dword ptr [ebp - 0x610]
            push ecx
            mov edx, dword ptr [ebp - 0x628]
            push edx
            mov eax, dword ptr [ebp - 0x62c]
            push eax
            call read
            add esp, 0xc
            cmp eax, dword ptr [ebp - 0x610]
            je L00568e9a
            mov dword ptr [gS3_last_error], 4
            xor eax, eax
            jmp L00568ec7
    L00568e9a:
            mov ecx, dword ptr [ebp - 0x628]
            mov dword ptr [gS3_soundbank_buffer], ecx
            mov edx, dword ptr [ebp - 0x610]
            mov dword ptr [gS3_soundbank_buffer_len], edx
            mov eax, dword ptr [ebp - 0x62c]
            push eax
            call _close
            add esp, 4
    L00568ec1:
            mov eax, dword ptr [ebp - 0x628]
    L00568ec7:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00567fe0
int C2_NAKED C2_HOOK_FASTCALL S3LoadSoundbank(const char* pPath, int pLow_memory_mode) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_soundbank_read_ctx read_ctx;
    char soundbank_filename[512];
    void *buffer;
    char dir_name[512];
    const char* cur_dir;

    if (!gS3_enabled) {
        return 0;
    }
    if (pLow_memory_mode == 0x123456) {
        gS3_low_memory_mode = 1;
        strcpy(soundbank_filename, pPath);
        dir_name[0] = '\0';
        buffer = S3LoadSoundBankFile(soundbank_filename);
        pLow_memory_mode = 0;
    } else {
        dir_name[0] = '\0';
        soundbank_filename[0] = '\0';
        cur_dir = PDS3GetWorkingDirectory();
        strcpy(dir_name, cur_dir);
        strcat(dir_name, gS3_path_separator);
        strcat(dir_name, "DATA");
        strcat(dir_name, gS3_path_separator);
        strcat(dir_name, gS3_sound_folder_name);
        strcat(dir_name, gS3_path_separator);
        strcpy(soundbank_filename, pPath);
        buffer = S3LoadSoundBankFile(soundbank_filename);
    }
    if (buffer == NULL) {
        return gS3_last_error;
    }
    read_ctx.nlines = 0;
    read_ctx.data_len = gS3_soundbank_buffer_len;
    read_ctx.data = buffer;
    S3SoundBankReaderSkipWhitespace(&read_ctx);
    while (S3SoundBankReadEntry(&read_ctx, dir_name, pLow_memory_mode) != 0) {
    }
    S3MemFree(buffer);
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x420
            mov dword ptr [ebp - 0x420], edx
            mov dword ptr [ebp - 0x41c], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00568189
            cmp dword ptr [ebp - 0x420], 0x123456
            jne L00568068
            mov dword ptr [gS3_low_memory_mode], 1
            mov dword ptr [ebp - 0x420], 0
            mov eax, dword ptr [ebp - 0x41c]
            push eax
            lea ecx, [ebp - 0x418]
            push ecx
            call strcpy
            add esp, 8
            mov byte ptr [ebp - 0x200], 0
            lea ecx, [ebp - 0x418]
            call S3LoadSoundBankFile
            mov dword ptr [ebp - 0x218], eax
            cmp dword ptr [ebp - 0x218], 0
            jne L00568063
            mov eax, dword ptr [gS3_last_error]
            jmp L0056818b
    L00568063:
            jmp L00568134
    L00568068:
            mov byte ptr [ebp - 0x200], 0
            mov byte ptr [ebp - 0x418], 0
            xor ecx, ecx
            call PDS3GetWorkingDirectory
            mov dword ptr [ebp - 0x204], eax
            mov edx, dword ptr [ebp - 0x204]
            push edx
            lea eax, [ebp - 0x200]
            push eax
            call strcpy
            add esp, 8
            push offset gS3_path_separator
            lea ecx, [ebp - 0x200]
            push ecx
            call strcat
            add esp, 8
            push offset gS3_CDA_enabled+0x10
            lea edx, [ebp - 0x200]
            push edx
            call strcat
            add esp, 8
            push offset gS3_path_separator
            lea eax, [ebp - 0x200]
            push eax
            call strcat
            add esp, 8
            push offset gS3_sound_folder_name
            lea ecx, [ebp - 0x200]
            push ecx
            call strcat
            add esp, 8
            push offset gS3_path_separator
            lea edx, [ebp - 0x200]
            push edx
            call strcat
            add esp, 8
            mov eax, dword ptr [ebp - 0x41c]
            push eax
            lea ecx, [ebp - 0x418]
            push ecx
            call strcpy
            add esp, 8
            lea ecx, [ebp - 0x418]
            call S3LoadSoundBankFile
            mov dword ptr [ebp - 0x218], eax
            cmp dword ptr [ebp - 0x218], 0
            jne L00568134
            mov eax, dword ptr [gS3_last_error]
            jmp L0056818b
    L00568134:
            mov edx, dword ptr [ebp - 0x218]
            mov dword ptr [ebp - 0x208], edx
            mov eax, dword ptr [gS3_soundbank_buffer_len]
            mov dword ptr [ebp - 0x214], eax
            mov dword ptr [ebp - 0x20c], 0
            lea ecx, [ebp - 0x214]
            call S3SoundBankReaderSkipWhitespace
    L00568160:
            mov ecx, dword ptr [ebp - 0x420]
            push ecx
            lea edx, [ebp - 0x200]
            lea ecx, [ebp - 0x214]
            call S3SoundBankReadEntry
            test eax, eax
            je L0056817e
            jmp L00568160
    L0056817e:
            mov ecx, dword ptr [ebp - 0x218]
            call S3MemFree
    L00568189:
            xor eax, eax
    L0056818b:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056818f
void C2_HOOK_FASTCALL S3SoundBankReaderSkipWhitespace(tS3_soundbank_read_ctx* pContext) {

    while (pContext->data_len != 0) {
        if (!isalnum(*pContext->data) && *pContext->data != '-') {
            S3SoundBankReaderSkipToNewline(pContext);
            continue;
        }
        return;
    }
}

// FUNCTION: CARMA2_HW 0x0056828c
int C2_HOOK_FASTCALL S3SoundBankReadEntry(tS3_soundbank_read_ctx *pContext, const char* pDir_name, int pLow_memory_mode) {
    tS3_descriptor* descriptor;
    int i;
    int n;
    const char* dir_name;
    double f1, f2;
    int count;
    int alternative;

    // GLOBAL: CARMA2_HW 0x006b2c8c
    static char cda_dir_name[4];

    descriptor = S3CreateDescriptor();
    if (descriptor == NULL) {
        return gS3_last_error;
    }

    /* Sound ID */
    if (sscanf(pContext->data, "%i%n", &descriptor->sample_id, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* Type, Flags */
    if (sscanf(pContext->data, "%i,%i%n", &descriptor->type, &descriptor->flags, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* File name */
    dir_name = pDir_name;
    if (descriptor->type == 2) {
        dir_name = cda_dir_name;
        cda_dir_name[0] = '\0';
    }
    if (S3SoundBankReaderReadFilename(&descriptor->path, pContext, dir_name) == 0) {
        return 0;
    }
    S3SoundBankReaderNextLine(pContext);

    /* Priority */
    if (sscanf(pContext->data, "%i%n", &descriptor->priority, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* Repeat rate */
    if (sscanf(pContext->data, "%i%n", &descriptor->repeat_rate, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* MinVol, MaxVol */
    if (sscanf(pContext->data, "%i,%i%n", &descriptor->min_volume, &descriptor->max_volume, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* MinPitch, MaxPitch */
    if (sscanf(pContext->data, "%lf,%lf%n", &f1, &f2, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);
    if (f1 == 0.0) {
        f1 = 1.875;
    }
    if (f2 == 0.0) {
        f2 = 1.875;
    }
    descriptor->min_pitch = (int)ldexp(f1, 16);
    descriptor->max_pitch = (int)ldexp(f2, 16);

    /* MinSpeed, MaxSpeed */
    if (sscanf(pContext->data, "%lf,%lf%n", &f1, &f2, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);
    if (f1 == 0.0) {
        f1 = 1.875;
    }
    if (f2 == 0.0) {
        f2 = 1.875;
    }
    descriptor->min_speed = (int)ldexp(f1, 16);
    descriptor->max_speed = (int)ldexp(f2, 16);

    /* Special FX index */
    if (sscanf(pContext->data, "%i%n", &descriptor->effects_enabled, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* Number of low memory alternatives */
    if (sscanf(pContext->data, "%d%n", &count, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);
    descriptor->low_memory_alternative = -1;
    for (i = 0; i < count; i++) {

        if (sscanf(pContext->data, "%d%n", &alternative, &n) != 1) {
            return 0;
        }
        if (i + 1 == pLow_memory_mode) {
            descriptor->low_memory_alternative = alternative;
        }
        S3SoundBankReaderAdvance(pContext, n);
        S3SoundBankReaderNextLine(pContext);
    }

    if ((descriptor->flags & 0x1) && descriptor->low_memory_alternative == -1) {
        if (descriptor->type == 1) {
            descriptor->buffer_description = NULL;
        } else {
            if (S3LoadSample(descriptor->sample_id) != eS3_error_none) {
                printf("\nSound bank file: couldn't load '%s'\n", descriptor->path);
                pContext->data_len = 1;
                return 0;
            }
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x005686e9
void C2_HOOK_FASTCALL S3SoundBankReaderNextLine(tS3_soundbank_read_ctx* pContext) {
    S3SoundBankReaderSkipToNewline(pContext);
    S3SoundBankReaderSkipWhitespace(pContext);
}

// FUNCTION: CARMA2_HW 0x00568205
void C2_HOOK_FASTCALL S3SoundBankReaderSkipToNewline(tS3_soundbank_read_ctx* pContext) {
    char* newline_ptr;

    newline_ptr = memchr(pContext->data, '\n', pContext->data_len);
    if (newline_ptr != NULL) {
        S3SoundBankReaderAdvance(pContext, newline_ptr + 1 - pContext->data);
        pContext->nlines += 1;
    } else {
        pContext->data_len = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00568260
void C2_HOOK_FASTCALL S3SoundBankReaderAdvance(tS3_soundbank_read_ctx* pContext, int pAmount) {
    pContext->data += pAmount;
    pContext->data_len -= pAmount;
}

// FUNCTION: CARMA2_HW 0x00565a8a
C2_NAKED tS3_descriptor* C2_HOOK_FASTCALL S3CreateDescriptor(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;

    C2_HOOK_BUG_ON(sizeof(tS3_descriptor) != 0x4c);

    descriptor = S3MemAllocate(sizeof(tS3_descriptor), kMem_S3_descriptor);
    if (descriptor == NULL) {
        gS3_last_error = eS3_error_memory;
        return NULL;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, prev, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, next, 0x2c);

    memset(descriptor, 0, sizeof(tS3_descriptor));
    gS3_state.root_descriptor->next = descriptor;
    descriptor->prev = gS3_state.root_descriptor;
    gS3_state.root_descriptor = descriptor;
    return descriptor;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dl, 0xe7
            mov ecx, 0x4c
            call S3MemAllocate
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00565ab3
            mov dword ptr [gS3_last_error], 3
            xor eax, eax
            jmp L00565ae9
    L00565ab3:
            push 0x4c
            push 0
            mov eax, dword ptr [ebp - 4]
            push eax
            call memset
            add esp, 0xc
            mov ecx, dword ptr [gS3_state+0x18]
            mov dword ptr [ebp - 8], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [edx + 0x2c], eax
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x28], edx
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [gS3_state+0x18], eax
            mov eax, dword ptr [ebp - 4]
    L00565ae9:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00568704
int C2_NAKED C2_HOOK_FASTCALL S3SoundBankReaderReadFilename(char** pPath, tS3_soundbank_read_ctx* pContext, const char* pDir_name) {
#ifndef CARPOCALYPSE2_MATCHING
    unsigned int dir_name_len;
    char* data_start;
    unsigned int bytes_read;

    data_start = pContext->data;
    dir_name_len = strlen(pDir_name);
    while (pContext->data_len != 0) {
        if (!isspace(*pContext->data)) {
            S3SoundBankReaderAdvance(pContext, 1);
            continue;
        }
        break;
    }
    bytes_read = pContext->data - data_start;
    if (bytes_read == 0) {
        return 0;
    }
    *pPath = S3MemAllocate(bytes_read + dir_name_len + 1, kMem_S3_scan_name);
    if (*pPath == NULL) {
        return 0;
    }
    strcpy(*pPath, pDir_name);
    memmove(&(*pPath)[dir_name_len], data_start, bytes_read);
    (*pPath)[bytes_read + dir_name_len] = '\0';
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x18
            mov dword ptr [ebp - 0x14], edx
            mov dword ptr [ebp - 0x10], ecx
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [ebp - 0xc], ecx
            mov edx, dword ptr [ebp + 8]
            push edx
            call strlen
            add esp, 4
            mov dword ptr [ebp - 4], eax
    L00568728:
            mov eax, dword ptr [ebp - 0x14]
            cmp dword ptr [eax], 0
            je L00568782
            cmp dword ptr [gQuickTime_call_component_with_storage+0x5a0], 1
            jle L00568752
            push 8
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ecx + 0xc]
            movsx eax, byte ptr [edx]
            push eax
            call _isctype
            add esp, 8
            mov dword ptr [ebp - 0x18], eax
            jmp L0056876d
    L00568752:
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ecx + 0xc]
            movsx eax, byte ptr [edx]
            mov ecx, dword ptr [gQuickTime_call_component_with_storage+0x394]
            xor edx, edx
            mov dx, word ptr [ecx + eax*2]
            and edx, 8
            mov dword ptr [ebp - 0x18], edx
    L0056876d:
            cmp dword ptr [ebp - 0x18], 0
            jne L00568782
            mov edx, 1
            mov ecx, dword ptr [ebp - 0x14]
            call S3SoundBankReaderAdvance
            jmp L00568728
    L00568782:
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [eax + 0xc]
            sub ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ebp - 8], ecx
            cmp dword ptr [ebp - 8], 0
            jne L00568798
            xor eax, eax
            jmp L005687f9
    L00568798:
            mov dl, 0xdf
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 4]
            lea ecx, [ecx + eax + 1]
            call S3MemAllocate
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx], eax
            mov eax, dword ptr [ebp - 0x10]
            cmp dword ptr [eax], 0
            jne L005687ba
            xor eax, eax
            jmp L005687f9
    L005687ba:
            mov ecx, dword ptr [ebp + 8]
            push ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [edx]
            push eax
            call strcpy
            add esp, 8
            mov ecx, dword ptr [ebp - 8]
            push ecx
            mov edx, dword ptr [ebp - 0xc]
            push edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax]
            add ecx, dword ptr [ebp - 4]
            push ecx
            call memmove
            add esp, 0xc
            mov edx, dword ptr [ebp - 4]
            add edx, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax]
            mov byte ptr [ecx + edx], 0
            mov eax, 1
    L005687f9:
            mov esp, ebp
            pop ebp
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a41f
void C2_HOOK_FASTCALL S3StopMidi(void) {
    S3StopMidiInternal();
    gS3_enable_midi = 0;
}


// FUNCTION: CARMA2_HW 0x00565b7f
void C2_HOOK_FASTCALL S3StopCDA(void) {
    S3StopCDAInternal();
    gS3_CDA_enabled = 0;
}

// FUNCTION: CARMA2_HW 0x0056a7bd
void C2_HOOK_FASTCALL S3StopMidiInternal(void) {
    if (gS3_enable_midi) {
        tS3_outlet* outlet;
        for (outlet = gS3_state.outlets; outlet != NULL; outlet = outlet->next) {
            tS3_channel *channel;

            for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                if (channel->type == 1) {
                    S3StopMIDIChannel(channel);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0056a4cd
int C2_HOOK_FASTCALL S3StopMIDIChannel(tS3_channel* pChannel) {
    if (gS3_enable_midi && pChannel->active && pChannel->type == 1) {
        PDS3StopMidiChannel(pChannel);
        pChannel->active = 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00565c22
int C2_HOOK_FASTCALL S3StopCDAInternal(void) {
    if (gS3_CDA_enabled) {
        tS3_outlet* outlet;

        for (outlet = gS3_state.outlets; outlet != NULL; outlet = outlet->next) {
            tS3_channel* channel;
            for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                if (channel->type == 2) {
                    PDS3StopCDAChannel(channel);
                }
            }
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00565bfe
int C2_NAKED C2_HOOK_FASTCALL S3StopCDAChannel(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    if (!gS3_CDA_enabled) {
        return 0;
    }
    return PDS3StopCDAChannel(pChannel);
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dword ptr [ebp - 8], ecx
            cmp dword ptr [gS3_CDA_enabled], 0
            je L00565c1b
            mov ecx, dword ptr [ebp - 8]
            call PDS3StopCDAChannel
            mov dword ptr [ebp - 4], eax
    L00565c1b:
            mov eax, dword ptr [ebp - 4]
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056991f
int C2_HOOK_FASTCALL S3NotifyUnbindChannel(tS3_channel* pChannel) {
    return 1;
}

// FUNCTION: CARMA2_HW 0x00565607
int C2_NAKED C2_HOOK_FASTCALL S3UnbindChannels(tS3_outlet* pOutlet) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel* next;
    tS3_channel* c;

    c = pOutlet->channel_list;
    for (; c != NULL;) {
        if (c->active) {
            c->termination_reason = 1;
            S3StopChannel(c);
            c->needs_service = 1;
        }
        next = c->next;
        S3NotifyUnbindChannel(c);
        if (gS3_unbound_channels == NULL) {
            gS3_unbound_channels = c;
            gS3_last_unbound_channel = c;
        } else {
            gS3_last_unbound_channel->next = c;
            gS3_last_unbound_channel = c;
        }

        C2_HOOK_BUG_ON(sizeof(*c) != 0x78);
        memset(c, 0, sizeof(*c));
        c = next;
    }
    pOutlet->channel_list = NULL;
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0xc
            mov dword ptr [ebp - 0xc], ecx
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 8], ecx
    L00565619:
            cmp dword ptr [ebp - 8], 0
            je L005656a4
            mov edx, dword ptr [ebp - 8]
            cmp dword ptr [edx], 0
            je L00565647
            mov eax, dword ptr [ebp - 8]
            mov dword ptr [eax + 4], 1
            mov ecx, dword ptr [ebp - 8]
            call S3StopChannel
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 8], 1
    L00565647:
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx + 0x64]
            mov dword ptr [ebp - 4], eax
            mov ecx, dword ptr [ebp - 8]
            call S3NotifyUnbindChannel
            cmp dword ptr [gS3_unbound_channels], 0
            jne L00565675
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [gS3_unbound_channels], ecx
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [gS3_last_unbound_channel], edx
            jmp L00565689
    L00565675:
            mov eax, dword ptr [gS3_last_unbound_channel]
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [eax + 0x64], ecx
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [gS3_last_unbound_channel], edx
    L00565689:
            push 0x78
            push 0
            mov eax, dword ptr [ebp - 8]
            push eax
            call memset
            add esp, 0xc
            mov ecx, dword ptr [ebp - 4]
            mov dword ptr [ebp - 8], ecx
            jmp L00565619
    L005656a4:
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 0x10], 0
            mov eax, 1
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00565888
C2_NAKED tS3_channel* C2_HOOK_FASTCALL S3GetChannelForTag(int pTag) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel* channel;
    tS3_outlet* outlet;
    int tag_id;

    if (pTag == 0) {
        return NULL;
    }
    tag_id = pTag & 0xff;
    outlet = gS3_state.outlets;
    for (; outlet != NULL && outlet->id != tag_id; ) {
        outlet = outlet->next;
    }
    if (outlet == NULL) {
        return NULL;
    }
    channel = outlet->channel_list;
    for (; channel != NULL; ) {
        if (channel->tag == pTag) {
            return channel;
        }
        channel = channel->next;
    }
    return NULL;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x10
            mov dword ptr [ebp - 0x10], ecx
            cmp dword ptr [ebp - 0x10], 0
            jne L0056589b
            xor eax, eax
            jmp L00565900
    L0056589b:
            mov eax, dword ptr [ebp - 0x10]
            and eax, 0xff
            mov dword ptr [ebp - 8], eax
            mov ecx, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 4], ecx
    L005658af:
            cmp dword ptr [ebp - 4], 0
            je L005658ca
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx]
            cmp eax, dword ptr [ebp - 8]
            je L005658ca
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [ebp - 4], edx
            jmp L005658af
    L005658ca:
            cmp dword ptr [ebp - 4], 0
            jne L005658d4
            xor eax, eax
            jmp L00565900
    L005658d4:
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0xc], ecx
    L005658dd:
            cmp dword ptr [ebp - 0xc], 0
            je L005658fe
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x1c]
            cmp eax, dword ptr [ebp - 0x10]
            jne L005658f3
            mov eax, dword ptr [ebp - 0xc]
            jmp L00565900
    L005658f3:
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x64]
            mov dword ptr [ebp - 0xc], edx
            jmp L005658dd
    L005658fe:
            xor eax, eax
    L00565900:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a818
tS3_error_codes C2_HOOK_FASTCALL S3ClearBufferOfMidiChannel(int pTag) {
    if (gS3_enable_midi) {
        tS3_channel* channel;

        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 1) {
            PDS3Return0_C(channel);
            channel->descriptor->buffer_description = NULL;
        }
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00568929
int C2_NAKED C2_HOOK_FASTCALL S3ReleaseSound(int pSound_id) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_buffer_desc *description;
    tS3_outlet *outlet;
    tS3_channel *channel;
    tS3_descriptor* descriptor;

    if (gS3_enabled) {
        descriptor = S3GetDescriptorByID(pSound_id);
        if (descriptor == NULL) {
            return eS3_error_bad_id;
        }
        if (descriptor->type == 1) {

            for (outlet = gS3_state.outlets; outlet != NULL; outlet = outlet->next) {

                for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                    if (channel->descriptor != NULL && channel->descriptor->sample_id == pSound_id) {
                        S3ClearBufferOfMidiChannel(channel->tag);
                    }
                }
            }
        } else if (descriptor->type == 0) {


            if (descriptor->buffer_description == NULL) {
                return 0;
            }
            description = descriptor->buffer_description;
            PDS3ReleaseSound(descriptor);
            if (description->field_0x14 != NULL) {
                S3MemFree(description->field_0x14);
            }
            S3MemFree(description);
            descriptor->buffer_description = NULL;
        }
    }
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00568a0b
            mov ecx, dword ptr [ebp - 0x14]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 8], 0
            jne L0056895a
            mov eax, 6
            jmp L00568a0d
    L0056895a:
            mov eax, dword ptr [ebp - 8]
            cmp dword ptr [eax + 4], 1
            jne L005689be
            mov ecx, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 0xc], ecx
            jmp L00568977
    L0056896e:
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x18]
            mov dword ptr [ebp - 0xc], eax
    L00568977:
            cmp dword ptr [ebp - 0xc], 0
            je L005689bc
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [ebp - 0x10], edx
            jmp L00568991
    L00568988:
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax + 0x64]
            mov dword ptr [ebp - 0x10], ecx
    L00568991:
            cmp dword ptr [ebp - 0x10], 0
            je L005689ba
            mov edx, dword ptr [ebp - 0x10]
            cmp dword ptr [edx + 0x60], 0
            je L005689b8
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx]
            cmp edx, dword ptr [ebp - 0x14]
            jne L005689b8
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax + 0x1c]
            call S3ClearBufferOfMidiChannel
    L005689b8:
            jmp L00568988
    L005689ba:
            jmp L0056896e
    L005689bc:
            jmp L00568a0b
    L005689be:
            mov ecx, dword ptr [ebp - 8]
            cmp dword ptr [ecx + 4], 0
            jne L00568a0b
            mov edx, dword ptr [ebp - 8]
            cmp dword ptr [edx + 0x40], 0
            jne L005689d4
            xor eax, eax
            jmp L00568a0d
    L005689d4:
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x40]
            mov dword ptr [ebp - 4], ecx
            mov ecx, dword ptr [ebp - 8]
            call PDS3ReleaseSound
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 0x14], 0
            je L005689f9
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x14]
            call S3MemFree
    L005689f9:
            mov ecx, dword ptr [ebp - 4]
            call S3MemFree
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x40], 0
    L00568a0b:
            xor eax, eax
    L00568a0d:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00566454
double C2_HOOK_STDCALL S3FRandomBetween(double pMin, double pMax) {

    return (pMax - pMin) * (double)rand() / 32767.0 + pMin;
}

// FUNCTION: CARMA2_HW 0x00564278
int C2_HOOK_FASTCALL S3IRandomBetween(int pMin, int pMax, int pDefault) {
    if (pMin == -1) {
        return pDefault;
    }
    if (pMin < pMax) {
        return pMin + rand() % (pMax - pMin);
    } else {
        return pMin;
    }
}

// FUNCTION: CARMA2_HW 0x005663a0
int C2_HOOK_FASTCALL S3IRandomBetween__dup(int pMin, int pMax, int pDefault) {
    if (pMin == -1) {
        return pDefault;
    }
    if (pMin < pMax) {
        return pMin + rand() % (pMax - pMin);
    } else {
        return pMin;
    }
}

// FUNCTION: CARMA2_HW 0x005663dd
int C2_HOOK_FASTCALL S3IRandomBetweenLog(int pMin, int pMax, int pDefault) {
    double dbl;

    if (pMin == -1 || pMin >= pMax) {
            return pDefault;
        } else {
            dbl = exp(S3FRandomBetween(log(pMin), log(pMax)));
            return (int)(ldexp(dbl, -16) * pDefault);
        }
    // } else {
    //     return pDefault;
    // }
}

// FUNCTION: CARMA2_HW 0x005656b7
int C2_NAKED C2_HOOK_FASTCALL S3FreeUnboundChannels(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel *channel;

    channel = gS3_unbound_channels;
    while (channel != NULL) {
        tS3_channel* next;

        next = channel->next;
        S3MemFree(channel);
        channel = next;
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov eax, dword ptr [gS3_unbound_channels]
            mov dword ptr [ebp - 8], eax
    L005656c5:
            cmp dword ptr [ebp - 8], 0
            je L005656e4
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx + 0x64]
            mov dword ptr [ebp - 4], edx
            mov ecx, dword ptr [ebp - 8]
            call S3MemFree
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [ebp - 8], eax
            jmp L005656c5
    L005656e4:
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005657f5
int C2_HOOK_FASTCALL S3SoundStillPlaying(int pTag) {
    tS3_channel *channel;

    if (gS3_enabled) {
        if (pTag == 0) {
            return 0;
        }

        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return 0;
        }

        if (S3ServiceChannel(channel) != 0) {
            return 1;
        }
    }

    return 0;
}

// FUNCTION: CARMA2_HW 0x005649c8
int C2_HOOK_FASTCALL S3SetVolume(int pVolume) {
    tS3_outlet* outlet;

    if (gS3_enabled) {
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (pVolume < 0) {
            pVolume = 0;
        }
        outlet = gS3_state.outlets;
        for (; outlet != NULL; ) {
            S3SetOutletVolume(outlet, pVolume);
            outlet = outlet->next;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569752
int C2_NAKED C2_HOOK_FASTCALL S3ServiceChannel(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pChannel->type == 0) {
        return PDS3IsSamplePlaying(pChannel);
    } else if (pChannel->type == 1) {
        return !PDS3IsMIDIStopped(pChannel);
    } else if (pChannel->type == 2) {
        return PDS3IsCDAPlaying();
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0xc
            mov dword ptr [ebp - 0xc], ecx
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x20], 0
            jne L005697c1
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 0x60], 0
            je L005697b5
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [eax + 4]
            cmp edx, dword ptr [ecx + 0x20]
            jne L005697b5
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x48]
            mov dword ptr [ebp - 4], edx
            cmp dword ptr [ebp - 4], 0
            je L005697b5
            lea eax, [ebp - 8]
            push eax
            mov ecx, dword ptr [ebp - 4]
            push ecx
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx]
            call dword ptr [eax + 0x24]
            test eax, eax
            jl L005697b5
            mov ecx, dword ptr [ebp - 8]
            and ecx, 1
            test ecx, ecx
            je L005697b5
            mov eax, 1
            jmp L005697eb
    L005697b5:
            mov ecx, dword ptr [ebp - 0xc]
            call PDS3StopSampleChannel
            xor eax, eax
            jmp L005697eb
    L005697c1:
            mov edx, dword ptr [ebp - 0xc]
            cmp dword ptr [edx + 0x20], 1
            jne L005697d9
            mov ecx, dword ptr [ebp - 0xc]
            call PDS3IsMIDIStopped
            neg eax
            sbb eax, eax
            inc eax
            jmp L005697eb
    L005697d9:
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x20], 2
            jne L005697e9
            call PDS3IsCDAPlaying
            jmp L005697eb
    L005697e9:
            xor eax, eax
    L005697eb:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a280
int C2_HOOK_FASTCALL S3ServiceSpatialSound(tS3_channel* pChannel) {
    (void) pChannel;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005665d4
C2_NAKED tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceInternal(void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_sound_source* src;

    C2_HOOK_BUG_ON(sizeof(tS3_sound_source) != 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_sound_source, next, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_sound_source, prev, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_sound_source, bound_outlet, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_sound_source, position_ptr, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_sound_source, velocity_ptr, 0xc);

    src = S3MemAllocate(sizeof(tS3_sound_source), kMem_S3_source);
    if (src == NULL) {
        gS3_last_error = eS3_error_memory;
        return NULL;
    }
    memset(src, 0, sizeof(tS3_sound_source));
    src->bound_outlet = pBound_outlet;
    src->position_ptr = pPosition;
    src->velocity_ptr = pVelocity;
    if (gS3_state.sources != NULL) {
        tS3_sound_source* s;
        s = gS3_state.sources;
        while (s->next != NULL) {
            s = s->next;
        }
        s->next = src;
        src->prev = s;
    } else {
        gS3_state.sources = src;
    }
    gS3_state.count_sources += 1;
    return src;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x10
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 0xc], ecx
            mov dl, 0xe9
            mov ecx, 0x44
            call S3MemAllocate
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 8], 0
            jne L00566603
            mov dword ptr [gS3_last_error], 3
            xor eax, eax
            jmp L00566680
    L00566603:
            push 0x44
            push 0
            mov eax, dword ptr [ebp - 8]
            push eax
            call memset
            add esp, 0xc
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0xc], eax
            mov ecx, dword ptr [gS3_state+0x10]
            mov dword ptr [ebp - 4], ecx
            cmp dword ptr [ebp - 4], 0
            jne L00566648
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [gS3_state+0x10], edx
            jmp L0056666e
    L00566648:
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 0x18], 0
            je L0056665c
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [ebp - 4], edx
            jmp L00566648
    L0056665c:
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [eax + 0x18], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [edx + 0x14], eax
    L0056666e:
            mov ecx, dword ptr [gS3_state+0x4]
            add ecx, 1
            mov dword ptr [gS3_state+0x4], ecx
            mov eax, dword ptr [ebp - 8]
    L00566680:
            mov esp, ebp
            pop ebp
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00566590
tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceBR(br_vector3* pPosition, br_vector3* pVelocity, tS3_outlet* pBound_outlet) {
    tS3_sound_source* source;

    if (gS3_enabled) {
        source = S3CreateSoundSourceInternal(pPosition, pVelocity, pBound_outlet);
        if (source != NULL) {
            source->brender_vector = 1;
        }
        return source;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00565bbc
int C2_HOOK_FASTCALL S3IsCDAEnabled(void) {
    if (gS3_CDA_enabled) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00564200
void C2_NAKED C2_HOOK_FASTCALL S3CalculateRandomizedFields(tS3_channel* chan, tS3_descriptor* desc) {
#ifndef CARPOCALYPSE2_MATCHING
    int vol;

    vol = S3IRandomBetween(desc->min_volume, desc->max_volume, 128);
    chan->source_volume = vol;
    chan->volume_multiplier = vol;
    if (desc->type == 0 && desc->buffer_description != NULL) {
        chan->rate = S3IRandomBetweenLog(desc->min_pitch, desc->max_pitch, desc->buffer_description->sample_rate);
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0xc
            mov dword ptr [ebp - 0xc], edx
            mov dword ptr [ebp - 8], ecx
            push 0x80
            mov eax, dword ptr [ebp - 0xc]
            mov edx, dword ptr [eax + 0x14]
            mov ecx, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [ecx + 0x10]
            call S3IRandomBetween
            mov dword ptr [ebp - 4], eax
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [edx + 0x50], eax
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 4]
            mov dword ptr [ecx + 0x24], edx
            mov eax, dword ptr [ebp - 8]
            mov dword ptr [eax + 0x28], 0
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 4], 0
            jne L00564274
            mov edx, dword ptr [ebp - 0xc]
            cmp dword ptr [edx + 0x40], 0
            je L00564274
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x40]
            mov edx, dword ptr [ecx + 4]
            push edx
            mov eax, dword ptr [ebp - 0xc]
            mov edx, dword ptr [eax + 0x1c]
            mov ecx, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [ecx + 0x18]
            call S3IRandomBetweenLog__dup
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [edx + 0x10], eax
    L00564274:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00565b4b
int C2_HOOK_FASTCALL S3CalculatePriority(int pPriority, int pVolumeFactor) {
    return pVolumeFactor / 40 + pPriority;
}

// FUNCTION: CARMA2_HW 0x00565864
int C2_HOOK_FASTCALL S3GenerateTag(tS3_outlet* outlet) {
    gS3_tag_seed += 256;
    return gS3_tag_seed | outlet->id;
}

// FUNCTION: CARMA2_HW 0x00565904
C2_NAKED tS3_channel* C2_HOOK_FASTCALL S3AllocateChannel(tS3_outlet* pOutlet, int pPriority) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel* c;
    int lowest_priority;
    int this_priority;
    tS3_channel* lowest_priority_chan;

    lowest_priority_chan = pOutlet->channel_list;
    c = pOutlet->channel_list;
    if (lowest_priority_chan == NULL) {
        return NULL;
    }
    while (c != NULL) {
        if (!c->active || c->needs_service) {
            if (!c->needs_service) {
                c->active = 1;
                return c;
            }
        } else {
            if (lowest_priority_chan->descriptor != NULL) {
                lowest_priority = S3CalculatePriority(lowest_priority_chan->descriptor->priority,
                                                      lowest_priority_chan->volume_multiplier);
            } else {
                lowest_priority = 0;
            }
            if (c->descriptor != NULL) {
                this_priority = S3CalculatePriority(c->descriptor->priority, c->volume_multiplier);;
            } else {
                this_priority = 0;
            }
            if (this_priority <= lowest_priority) {
                lowest_priority_chan = c;
            }
        }
        c = c->next;
    }
    if (lowest_priority_chan->descriptor == NULL || lowest_priority_chan->needs_service) {
        lowest_priority = 0;
    } else {
        lowest_priority = S3CalculatePriority(lowest_priority_chan->descriptor->priority,
                                              lowest_priority_chan->volume_multiplier);
    }
    if (pPriority > lowest_priority && !lowest_priority_chan->needs_service) {
        lowest_priority_chan->termination_reason = 2;
        S3StopChannel(lowest_priority_chan);
        lowest_priority_chan->active = 1;
    }
    return NULL;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x1c
            mov dword ptr [ebp - 0x18], edx
            mov dword ptr [ebp - 0x14], ecx
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0x10], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [ebp - 4], edx
            cmp dword ptr [ebp - 0x10], 0
            je L00565a37
    L00565929:
            cmp dword ptr [ebp - 0x10], 0
            je L005659d9
            mov eax, dword ptr [ebp - 0x10]
            cmp dword ptr [eax], 0
            je L005659b4
            mov ecx, dword ptr [ebp - 0x10]
            cmp dword ptr [ecx + 8], 0
            jne L005659b4
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 0x60], 0
            je L00565966
            mov eax, dword ptr [ebp - 4]
            mov edx, dword ptr [eax + 0x24]
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ecx + 0x60]
            mov ecx, dword ptr [eax + 8]
            call S3CalculatePriority
            mov dword ptr [ebp - 0xc], eax
            jmp L0056596d
    L00565966:
            mov dword ptr [ebp - 0xc], 0
    L0056596d:
            mov ecx, dword ptr [ebp - 0x10]
            cmp dword ptr [ecx + 0x60], 0
            je L0056598f
            mov edx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [edx + 0x24]
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax + 0x60]
            mov ecx, dword ptr [ecx + 8]
            call S3CalculatePriority
            mov dword ptr [ebp - 8], eax
            jmp L00565996
    L0056598f:
            mov dword ptr [ebp - 8], 0
    L00565996:
            mov edx, dword ptr [ebp - 0xc]
            cmp edx, dword ptr [ebp - 8]
            jge L005659a6
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [ebp - 0x1c], eax
            jmp L005659ac
    L005659a6:
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ebp - 0x1c], ecx
    L005659ac:
            mov edx, dword ptr [ebp - 0x1c]
            mov dword ptr [ebp - 4], edx
            jmp L005659cb
    L005659b4:
            mov eax, dword ptr [ebp - 0x10]
            cmp dword ptr [eax + 8], 0
            jne L005659cb
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx], 1
            mov eax, dword ptr [ebp - 0x10]
            jmp L00565a39
    L005659cb:
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [edx + 0x64]
            mov dword ptr [ebp - 0x10], eax
            jmp L00565929
    L005659d9:
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x60], 0
            je L00565a04
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 8], 0
            jne L00565a04
            mov eax, dword ptr [ebp - 4]
            mov edx, dword ptr [eax + 0x24]
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ecx + 0x60]
            mov ecx, dword ptr [eax + 8]
            call S3CalculatePriority
            mov dword ptr [ebp - 0xc], eax
            jmp L00565a0b
    L00565a04:
            mov dword ptr [ebp - 0xc], 0
    L00565a0b:
            mov ecx, dword ptr [ebp - 0x18]
            cmp ecx, dword ptr [ebp - 0xc]
            jle L00565a37
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 8], 0
            jne L00565a37
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [eax + 4], 2
            mov ecx, dword ptr [ebp - 4]
            call S3StopChannel
            mov ecx, dword ptr [ebp - 4]
            mov dword ptr [ecx], 1
    L00565a37:
            xor eax, eax
    L00565a39:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00569b86
int C2_HOOK_FASTCALL S3MIDILoadSong2(tS3_channel* pChannel) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a473
tS3_error_codes C2_HOOK_FASTCALL S3MIDILoadSong(tS3_channel* pChannel) {
    if (S3MIDILoadSong2(pChannel) != 0) {
        return eS3_error_load_song;
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00564d4a
void C2_NAKED C2_HOOK_FASTCALL S3ReverseSample(tS3_effect_tag pEffect, tS3_sound_tag pTag) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel* channel;

    channel = S3GetChannelForTag(pTag);
    if (channel != NULL) {
        PDReverseAudio(channel);
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x20
            mov dword ptr [ebp - 0x20], edx
            mov dword ptr [ebp - 0x1c], ecx
            mov ecx, dword ptr [ebp - 0x20]
            call S3GetChannelForTag
            mov dword ptr [ebp - 0x14], eax
            cmp dword ptr [ebp - 0x14], 0
            jne L00564d69
            jmp L00564d71
    L00564d69:
            mov ecx, dword ptr [ebp - 0x14]
            call PDReverseAudio
    L00564d71:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00564d75
int C2_HOOK_FASTCALL S3SetSampleFilterCallbacks(tS3_sample_filter pEnable, tS3_sample_filter pDisable) {
    if (pEnable != NULL) {
        gS3_sample_filter_func = pEnable;
    }
    if (pDisable != NULL) {
        gS3_sample_filter_disable_func = pDisable;
    } else if (pDisable == NULL) {
        gS3_sample_filter_disable_func = gS3_sample_filter_func;
    }
    if (pEnable == NULL || (pEnable == NULL && pDisable == NULL)) {
        gS3_effects_enabled = 0;
    } else {
        gS3_effects_enabled = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00564de0
int C2_HOOK_FASTCALL S3ExecuteSampleFilterFuncs(tS3_channel* pChannel) {
    if (gS3_effects_enabled & (!pChannel->descriptor->effects_enabled)) {
        gS3_sample_filter_func(1, pChannel->tag);
        pChannel->descriptor->effects_enabled = 1;
    } else if ((!gS3_effects_enabled) & pChannel->descriptor->effects_enabled) {
        gS3_sample_filter_disable_func(1, pChannel->tag);
        pChannel->descriptor->effects_enabled = 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a493
tS3_error_codes C2_HOOK_FASTCALL S3PlayMIDI(tS3_channel* pChannel) {
    if (gS3_enable_midi && pChannel->type == 1) {
        PDS3StopMidiChannel(pChannel);
        pChannel->active = 1;
        return PDS3StartMidiChannel(pChannel);
    } else {
        return eS3_error_none;
    }
}

// FUNCTION: CARMA2_HW 0x00569d3f
int C2_HOOK_FASTCALL S3SetMIDIVolume2(tS3_channel* pChannel, int pVolume) {
    uintptr_t mciDevice;

    mciDevice = pChannel->mciDevice;
    (void) mciDevice;

    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a505
int C2_HOOK_FASTCALL S3SetMIDIVolume(tS3_channel* pChannel, int pVolume) {
    if (pVolume < 0) {
        pVolume = 0;
    }
    if (pVolume > 255) {
        pVolume = 255;
    }
    if (gS3_enable_midi) {
        S3SetMIDIVolume2(pChannel, pVolume);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00565bd3
tS3_error_codes C2_HOOK_FASTCALL S3PlayCDA(tS3_channel* pChannel) {
    tS3_error_codes result;

    result = eS3_error_none;
    if (gS3_CDA_enabled) {
        result = PDS3PlayCDAChannel(pChannel);
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00564565
int C2_HOOK_FASTCALL S3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    tS3_channel* channel;
    tS3_descriptor* desc;
    int priority;

    if (gS3_enabled) {
        if (pOutlet == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return 0;
        }
        desc = S3GetDescriptorByID(pSound);
        if (desc == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return 0;
        }

        C2_HOOK_BUG_ON(sizeof(gS3_channel_template) != 0x78);

        memset(&gS3_channel_template, 0, sizeof(gS3_channel_template));
        S3CalculateRandomizedFields(&gS3_channel_template, desc);
        priority = S3CalculatePriority(gS3_channel_template.volume_multiplier, desc->priority);
        channel = S3AllocateChannel(pOutlet, priority);
        if (channel == NULL) {
            gS3_last_error = eS3_error_channel_alloc;
            return 0;
        }
        channel->source_volume = gS3_channel_template.source_volume;
        channel->volume_multiplier = gS3_channel_template.volume_multiplier;
        channel->field_0x28 = gS3_channel_template.field_0x28;
        channel->rate = gS3_channel_template.rate;

        if (desc->type == 2) {
        } else if (desc->type == 0 && !(desc->buffer_description != NULL && desc->flags != 2)) {
            if (!S3LoadSample(pSound)) {
                channel->needs_service = 1;
                gS3_last_error = eS3_error_load_sound;
                return 0;
            }
        }
        if (channel->descriptor != NULL && channel->descriptor->type == 1 && channel->descriptor->sample_id != pSound) {
            S3ClearBufferOfMidiChannel(channel->tag);
        }
        channel->spatial_sound = 0;
        channel->sound_source_ptr = NULL;
        channel->descriptor = desc;
        channel->type = desc->type;
        channel->repetitions = desc->repeat_rate <= 0 ? 0 : desc->repeat_rate;
        channel->needs_service = 0;
        channel->termination_reason = eS3_tr_natural;
        channel->tag = S3GenerateTag(pOutlet);
        if (desc->type == 1 && desc->buffer_description == NULL && S3MIDILoadSong(channel) != 0) {
            channel->needs_service = 1;
            return 0;
        } else if (channel->type == 0) {
            S3ExecuteSampleFilterFuncs(channel);
            if (S3PlaySample(channel, desc) == 0) {
                gS3_last_error = eS3_error_start_sound;
                channel->needs_service = 1;
                return 0;
            }
        } else if (channel->type == 1) {
            if (S3PlayMIDI(channel) != 0) {
                channel->needs_service = 1;
                gS3_last_error = eS3_error_start_song;
                return 0;
            }
            S3SetMIDIVolume(channel, channel->volume_multiplier);
        } else if (channel->type == 2 && S3PlayCDA(channel) != 0) {
            channel->needs_service = 1;
            gS3_last_error = eS3_error_start_cda;
            return 0;
        }
        return channel->tag;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00564807
int C2_HOOK_FASTCALL S3SetChannelVolume(int pChannel_tag, int pVolume) {
    tS3_channel* channel;
    if (gS3_enabled) {

        channel = S3GetChannelForTag(pChannel_tag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (pVolume < 0) {
            pVolume = 0x80;
        }
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (channel->type == 0) {
            channel->volume_multiplier = pVolume;
            if (!PDS3UpdateChannelVolume(channel)) {
                return eS3_error_function_failed;
            }
        } else if (channel->type == 1) {
            S3SetMIDIVolume(channel, pVolume);
        } else if (channel->type == 2) {
            S3UpdateCDAVolume(channel, pVolume);
        }
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00564e9b
int C2_NAKED C2_HOOK_FASTCALL S3StartSound2(tS3_outlet* pOutlet, tS3_sound_id pSound, unsigned int pRepeats, int pLeft_volume, int pRight_volume,  tS32 pLeft_pitch, tS32 pRight_pitch) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    tS3_channel *chan;
    float normalized_delta_volume;
    int priority;

    if (gS3_enabled) {
        descriptor = S3GetDescriptorByID(pSound);
        if (descriptor == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return 0;
        }
        if (pLeft_volume < 0) {
            pLeft_volume = 0x80;
        }
        if (pRight_volume < 0) {
            pRight_volume = 0x80;
        }
        if (0xff < pLeft_volume) {
            pLeft_volume = 0xff;
        }
        if (0xff < pRight_volume) {
            pRight_volume = 0xff;
        }
        normalized_delta_volume = (float)(pRight_volume - pLeft_volume) / 255.0f;

        priority = S3CalculatePriority((pLeft_volume + pRight_volume) / 2, descriptor->priority);
        chan = S3AllocateChannel(pOutlet, priority);
        if (chan == NULL) {
            gS3_last_error = eS3_error_channel_alloc;
            return 0;
        }
        if (descriptor->type != 2) {
            if (descriptor->type == 0) {
                if (descriptor->buffer_description == NULL) {
                    if (!S3LoadSample(pSound) || (descriptor->flags & 0x2)) {
                        chan->needs_service = 1;
                        gS3_last_error = eS3_error_load_sound;
                        return 0;
                    }
                }
            }
        }
        if (chan->descriptor != NULL && chan->descriptor->type == 1 && chan->descriptor->sample_id != pSound) {
            S3ClearBufferOfMidiChannel(chan->tag);
        }
        chan->spatial_sound = 0;
        chan->descriptor = descriptor;
        chan->needs_service = 0;
        chan->termination_reason = 0;
        chan->type = descriptor->type;
        chan->sound_source_ptr = NULL;
        chan->repetitions = pRepeats <= 0 ? 0 : pRepeats;
        S3CalculateRandomizedFields(chan, descriptor);
        chan->volume_multiplier = (pLeft_volume + pRight_volume) / 2;
        chan->field_0x28 = normalized_delta_volume;
        chan->tag = S3GenerateTag(pOutlet);
        if (pLeft_pitch == -1) {
            pLeft_pitch = 0x10000;
        }
        if (pRight_pitch == -1) {
            pRight_pitch = 0x10000;
        }
        chan->rate = (int)((double)chan->rate * ldexp((double)pLeft_pitch, -16));
        if (!pOutlet->independent_pitch) {
            chan->rate = (int)((double)chan->rate * ldexp((double)pRight_pitch, -16));
        }
        if (descriptor->type == 1 && descriptor->buffer_description == NULL && S3MIDILoadSong(chan) != eS3_error_none) {
            chan->needs_service = 1;
            return 0;
        } else if (chan->type == 0) {
            S3ExecuteSampleFilterFuncs(chan);
            if (!S3PlaySample(chan, descriptor)) {
                chan->needs_service = 1;
                gS3_last_error = eS3_error_start_sound;
                return 0;
            }
        } else if (chan->type == 1) {
            if (S3PlayMIDI(chan)) {
                chan->needs_service = 1;
                gS3_last_error = eS3_error_start_song;
                return 0;
            }
            S3SetMIDIVolume(chan, chan->volume_multiplier);
        } else if (chan->type == 2 && S3PlayCDA(chan)) {
            chan->needs_service = 1;
            gS3_last_error = eS3_error_start_cda;
            return 0;
        }
        return chan->tag;
    } else {
        return 0;
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x2c
            push esi
            mov dword ptr [ebp - 0x18], edx
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [gS3_enabled], 0
            je L005651bb
            mov ecx, dword ptr [ebp - 0x18]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 8], 0
            jne L00564ed7
            mov dword ptr [gS3_last_error], 6
            xor eax, eax
            jmp L005651bd
    L00564ed7:
            cmp dword ptr [ebp + 0xc], 0
            jge L00564ee4
            mov dword ptr [ebp + 0xc], 0x80
    L00564ee4:
            cmp dword ptr [ebp + 0x10], 0
            jge L00564ef1
            mov dword ptr [ebp + 0x10], 0x80
    L00564ef1:
            cmp dword ptr [ebp + 0xc], 0xff
            jle L00564f01
            mov dword ptr [ebp + 0xc], 0xff
    L00564f01:
            cmp dword ptr [ebp + 0x10], 0xff
            jle L00564f11
            mov dword ptr [ebp + 0x10], 0xff
    L00564f11:
            mov eax, dword ptr [ebp + 0x10]
            sub eax, dword ptr [ebp + 0xc]
            mov dword ptr [ebp - 0x1c], eax
            fild dword ptr [ebp - 0x1c]
            fdiv dword ptr [rendererStateStoredDispatch+0xc38]
            fstp dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 8]
            mov esi, dword ptr [ecx + 8]
            mov eax, dword ptr [ebp + 0xc]
            add eax, dword ptr [ebp + 0x10]
            cdq
            sub eax, edx
            mov ecx, eax
            sar ecx, 1
            mov edx, esi
            call S3CalculatePriority
            mov dword ptr [ebp - 0xc], eax
            mov edx, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [ebp - 0x14]
            call S3AllocateChannel
            mov dword ptr [ebp - 0x10], eax
            cmp dword ptr [ebp - 0x10], 0
            jne L00564f68
            mov dword ptr [gS3_last_error], 0xa
            xor eax, eax
            jmp L005651bd
    L00564f68:
            mov edx, dword ptr [ebp - 8]
            cmp dword ptr [edx + 4], 2
            jne L00564f73
            jmp L00564fb9
    L00564f73:
            mov eax, dword ptr [ebp - 8]
            cmp dword ptr [eax + 4], 0
            jne L00564fb9
            mov ecx, dword ptr [ebp - 8]
            cmp dword ptr [ecx + 0x40], 0
            jne L00564fb9
            mov ecx, dword ptr [ebp - 0x18]
            call S3LoadSample
            test eax, eax
            je L00564f9e
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L00564fb9
    L00564f9e:
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 1
            mov dword ptr [gS3_last_error], 8
            xor eax, eax
            jmp L005651bd
    L00564fb9:
            mov edx, dword ptr [ebp - 0x10]
            cmp dword ptr [edx + 0x60], 0
            je L00564fe6
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [eax + 0x60]
            cmp dword ptr [ecx + 4], 1
            jne L00564fe6
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [eax]
            cmp ecx, dword ptr [ebp - 0x18]
            je L00564fe6
            mov edx, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [edx + 0x1c]
            call S3ClearBufferOfMidiChannel
    L00564fe6:
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 0x14], 0
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x60], edx
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 8], 0
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 4], 0
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [edx + 0x20], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x74], 0
            xor eax, eax
            cmp dword ptr [ebp + 8], 0
            setle al
            dec eax
            and eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 0xc], eax
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0x10]
            call S3CalculateRandomizedFields
            mov eax, dword ptr [ebp + 0xc]
            add eax, dword ptr [ebp + 0x10]
            cdq
            sub eax, edx
            sar eax, 1
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x24], eax
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 4]
            mov dword ptr [eax + 0x28], ecx
            mov ecx, dword ptr [ebp - 0x14]
            call S3GenerateTag
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x1c], eax
            cmp dword ptr [ebp + 0x14], -1
            jne L00565076
            mov dword ptr [ebp + 0x14], 0x10000
    L00565076:
            cmp dword ptr [ebp + 0x18], -1
            jne L00565083
            mov dword ptr [ebp + 0x18], 0x10000
    L00565083:
            mov eax, dword ptr [ebp - 0x10]
            fild dword ptr [eax + 0x10]
            fstp qword ptr [ebp - 0x24]
            push -0x10
            fild dword ptr [ebp + 0x14]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            call ldexp
            add esp, 0xc
            fmul qword ptr [ebp - 0x24]
            call _ftol
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 0x10], eax
            mov edx, dword ptr [ebp - 0x14]
            cmp dword ptr [edx + 0xc], 0
            jne L005650e0
            mov eax, dword ptr [ebp - 0x10]
            fild dword ptr [eax + 0x10]
            fstp qword ptr [ebp - 0x2c]
            push -0x10
            fild dword ptr [ebp + 0x18]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            call ldexp
            add esp, 0xc
            fmul qword ptr [ebp - 0x2c]
            call _ftol
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 0x10], eax
    L005650e0:
            mov edx, dword ptr [ebp - 8]
            cmp dword ptr [edx + 4], 1
            jne L0056510f
            mov eax, dword ptr [ebp - 8]
            cmp dword ptr [eax + 0x40], 0
            jne L0056510f
            mov ecx, dword ptr [ebp - 0x10]
            call S3MIDILoadSong
            test eax, eax
            je L0056510f
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 1
            xor eax, eax
            jmp L005651bd
    L0056510f:
            mov edx, dword ptr [ebp - 0x10]
            cmp dword ptr [edx + 0x20], 0
            jne L00565149
            mov ecx, dword ptr [ebp - 0x10]
            call S3ExecuteSampleFilterFuncs
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlaySample
            test eax, eax
            jne L00565147
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 8], 1
            mov dword ptr [gS3_last_error], 9
            xor eax, eax
            jmp L005651bd
    L00565147:
            jmp L005651b3
    L00565149:
            mov ecx, dword ptr [ebp - 0x10]
            cmp dword ptr [ecx + 0x20], 1
            jne L00565186
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlayMIDI
            test eax, eax
            je L00565176
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 8], 1
            mov dword ptr [gS3_last_error], 0xf
            xor eax, eax
            jmp L005651bd
    L00565176:
            mov eax, dword ptr [ebp - 0x10]
            mov edx, dword ptr [eax + 0x24]
            mov ecx, dword ptr [ebp - 0x10]
            call S3SetMIDIVolume
            jmp L005651b3
    L00565186:
            mov ecx, dword ptr [ebp - 0x10]
            cmp dword ptr [ecx + 0x20], 2
            jne L005651b3
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlayCDA
            test eax, eax
            je L005651b3
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 8], 1
            mov dword ptr [gS3_last_error], 0x10
            xor eax, eax
            jmp L005651bd
    L005651b3:
            mov eax, dword ptr [ebp - 0x10]
            mov eax, dword ptr [eax + 0x1c]
            jmp L005651bd
    L005651bb:
            xor eax, eax
    L005651bd:
            pop esi
            mov esp, ebp
            pop ebp
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00566dc1
tS3_sound_tag C2_NAKED C2_HOOK_FASTCALL S3StartSoundFromSource3(tS3_sound_id pSound, tS3_sound_source* pSource, tS3_repeats pRepeats, tS3_volume pVolume, tS3_pitch pPitch, tS3_speed pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    int priority;
    tS3_outlet* outlet;
    tS3_channel* channel;

    if (gS3_enabled) {
        if (pSource == NULL) {
            return 0;
        }
        outlet = pSource->bound_outlet;
        descriptor = S3GetDescriptorByID(pSound);
        if (descriptor == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return 0;
        }
        if (descriptor->type != 0) {
            return 0;
        }
        if (descriptor->buffer_description == NULL) {
            return eS3_error_nonexistant_source;
        }
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (pVolume < 0) {
            pVolume = S3IRandomBetween__dup(descriptor->min_volume, descriptor->max_volume, 128);
        }
        memset(&gS3_channel_template, 0, 0x78);
        gS3_channel_template.source_volume = pVolume;
        gS3_channel_template.volume_multiplier = pVolume;
        gS3_channel_template.rate = S3IRandomBetweenLog(descriptor->min_pitch, descriptor->max_pitch, descriptor->buffer_description->sample_rate);
        if (pPitch == -1) {
            pPitch = 0x10000;
        }
        if (pSpeed == -1) {
            pSpeed = 0x10000;
        }
        gS3_channel_template.rate = (int)((float)gS3_channel_template.rate * (float)ldexp((double)pPitch, -16));
        if (!outlet->independent_pitch) {
            gS3_channel_template.rate = (int)((float)gS3_channel_template.rate * (float)ldexp((double)pSpeed, -16));
        }
        gS3_channel_template.source_rate = gS3_channel_template.rate;
        gS3_channel_template.sound_source_ptr = pSource;
        if (pSource->velocity_ptr == NULL) {
            S3CopyVector3(&gS3_channel_template.prev_position, pSource->position_ptr, pSource->brender_vector);
        }
        gS3_channel_template.max_distance_squared = 150.0f;
        if (S3Calculate3D(&gS3_channel_template, 0)) {
            priority = S3CalculatePriority(gS3_channel_template.volume_multiplier, descriptor->priority);
            channel = S3AllocateChannel(outlet, priority);
            if (channel == NULL) {
                gS3_last_error = eS3_error_channel_alloc;
                return 0;
            }
            if ((descriptor->buffer_description == NULL || (descriptor->flags & 2) != 0)
                    && S3LoadSample(pSound) == eS3_error_none) {
                gS3_last_error = eS3_error_load_sound;
                channel->needs_service = 1;
                return 0;
            }
            channel->volume_multiplier = gS3_channel_template.volume_multiplier;
            channel->field_0x28 = gS3_channel_template.field_0x28;
            channel->rate = gS3_channel_template.rate;
            channel->position.x = gS3_channel_template.position.x;
            channel->position.y = gS3_channel_template.position.y;
            channel->position.z = gS3_channel_template.position.z;
            channel->velocity.x = gS3_channel_template.velocity.x;
            channel->velocity.y = gS3_channel_template.velocity.y;
            channel->velocity.z = gS3_channel_template.velocity.z;
            channel->prev_position.x = gS3_channel_template.prev_position.x;
            channel->prev_position.y = gS3_channel_template.prev_position.y;
            channel->prev_position.z = gS3_channel_template.prev_position.z;
            channel->source_volume = gS3_channel_template.source_volume;
            channel->source_rate = gS3_channel_template.source_rate;
            channel->max_distance_squared = gS3_channel_template.max_distance_squared;
            channel->spatial_sound = 1;
            channel->sound_source_ptr = pSource;
            channel->descriptor = descriptor;
            channel->tag = S3GenerateTag(outlet);
            channel->repetitions = pRepeats <= 0 ? 0 : pRepeats;
            channel->needs_service = 0;
            channel->termination_reason = eS3_tr_natural;
            S3ExecuteSampleFilterFuncs(channel);
            if (S3PlaySample(channel, channel->descriptor) == 0) {
                channel->needs_service = 1;
                gS3_last_error = eS3_error_start_sound;
                return 0;
            }
            return channel->tag;
        }
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x28
            mov dword ptr [ebp - 0x18], edx
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00567114
            cmp dword ptr [ebp - 0x18], 0
            jne L00566de7
            xor eax, eax
            jmp L00567116
    L00566de7:
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0xc], ecx
            mov ecx, dword ptr [ebp - 0x14]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00566e12
            mov dword ptr [gS3_last_error], 6
            xor eax, eax
            jmp L00567116
    L00566e12:
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 4], 0
            je L00566e22
            xor eax, eax
            jmp L00567116
    L00566e22:
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 0x40], 0
            jne L00566e35
            mov eax, 0x13
            jmp L00567116
    L00566e35:
            cmp dword ptr [ebp + 0xc], 0xff
            jle L00566e45
            mov dword ptr [ebp + 0xc], 0xff
    L00566e45:
            cmp dword ptr [ebp + 0xc], 0
            jge L00566e64
            push 0x80
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x14]
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x10]
            call S3IRandomBetween__dup
            mov dword ptr [ebp + 0xc], eax
    L00566e64:
            push 0x78
            push 0
            push offset gS3_channel_template
            call memset
            add esp, 0xc
            mov ecx, dword ptr [ebp + 0xc]
            mov dword ptr [gS3_channel_template+0x50], ecx
            mov edx, dword ptr [ebp + 0xc]
            mov dword ptr [gS3_channel_template+0x24], edx
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x40]
            mov edx, dword ptr [ecx + 4]
            push edx
            mov eax, dword ptr [ebp - 4]
            mov edx, dword ptr [eax + 0x1c]
            mov ecx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ecx + 0x18]
            call S3IRandomBetweenLog
            mov dword ptr [gS3_channel_template+0x10], eax
            cmp dword ptr [ebp + 0x10], -1
            jne L00566eb4
            mov dword ptr [ebp + 0x10], 0x10000
    L00566eb4:
            cmp dword ptr [ebp + 0x14], -1
            jne L00566ec1
            mov dword ptr [ebp + 0x14], 0x10000
    L00566ec1:
            fild dword ptr [gS3_channel_template+0x10]
            fstp dword ptr [ebp - 0x1c]
            push -0x10
            fild dword ptr [ebp + 0x10]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fst dword ptr [ebp - 0x20]
            fmul dword ptr [ebp - 0x1c]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
            mov edx, dword ptr [ebp - 0xc]
            cmp dword ptr [edx + 0xc], 0
            jne L00566f22
            fild dword ptr [gS3_channel_template+0x10]
            fstp dword ptr [ebp - 0x24]
            push -0x10
            fild dword ptr [ebp + 0x14]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fst dword ptr [ebp - 0x28]
            fmul dword ptr [ebp - 0x24]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
    L00566f22:
            mov eax, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [gS3_channel_template+0x54], eax
            mov ecx, dword ptr [ebp - 0x18]
            mov dword ptr [gS3_channel_template+0x74], ecx
            mov edx, dword ptr [ebp - 0x18]
            cmp dword ptr [edx + 0xc], 0
            jne L00566f55
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 4]
            push ecx
            mov edx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [edx + 8]
            mov ecx, offset gS3_channel_template+0x44
            call S3CopyVector3
    L00566f55:
            mov dword ptr [gS3_channel_template+0x58], 0x43160000
            xor edx, edx
            mov ecx, offset gS3_channel_template
            call S3Calculate3D
            test eax, eax
            je L00567114
            mov eax, dword ptr [ebp - 4]
            mov edx, dword ptr [eax + 8]
            mov ecx, dword ptr [gS3_channel_template+0x24]
            call S3CalculatePriority
            mov dword ptr [ebp - 8], eax
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0xc]
            call S3AllocateChannel
            mov dword ptr [ebp - 0x10], eax
            cmp dword ptr [ebp - 0x10], 0
            jne L00566fac
            mov dword ptr [gS3_last_error], 0xa
            xor eax, eax
            jmp L00567116
    L00566fac:
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            je L00566fc2
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L00566fe9
    L00566fc2:
            mov ecx, dword ptr [ebp - 0x14]
            call S3LoadSample
            test eax, eax
            jne L00566fe9
            mov dword ptr [gS3_last_error], 8
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 1
            xor eax, eax
            jmp L00567116
    L00566fe9:
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [gS3_channel_template+0x24]
            mov dword ptr [edx + 0x24], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [gS3_channel_template+0x28]
            mov dword ptr [ecx + 0x28], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [eax + 0x10], ecx
            mov edx, dword ptr [ebp - 0x10]
            add edx, 0x2c
            mov eax, dword ptr [gS3_channel_template+0x2c]
            mov dword ptr [edx], eax
            mov ecx, dword ptr [gS3_channel_template+0x30]
            mov dword ptr [edx + 4], ecx
            mov eax, dword ptr [gS3_channel_template+0x34]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp - 0x10]
            add ecx, 0x38
            mov edx, dword ptr [gS3_channel_template+0x38]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [gS3_channel_template+0x3c]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [gS3_channel_template+0x40]
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [ebp - 0x10]
            add eax, 0x44
            mov ecx, dword ptr [gS3_channel_template+0x44]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [gS3_channel_template+0x48]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [gS3_channel_template+0x4c]
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [gS3_channel_template+0x50]
            mov dword ptr [edx + 0x50], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [gS3_channel_template+0x54]
            mov dword ptr [ecx + 0x54], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [gS3_channel_template+0x58]
            mov dword ptr [eax + 0x58], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x14], 1
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 0x18]
            mov dword ptr [eax + 0x74], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [edx + 0x60], eax
            mov ecx, dword ptr [ebp - 0xc]
            call S3GenerateTag
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 0x1c], eax
            xor edx, edx
            cmp dword ptr [ebp + 8], 0
            setle dl
            dec edx
            and edx, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 0xc], edx
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 8], 0
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 4], 0
            mov ecx, dword ptr [ebp - 0x10]
            call S3ExecuteSampleFilterFuncs
            mov edx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlaySample
            test eax, eax
            jne L0056710c
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 8], 1
            mov dword ptr [gS3_last_error], 9
            xor eax, eax
            jmp L00567116
    L0056710c:
            mov ecx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ecx + 0x1c]
            jmp L00567116
    L00567114:
            xor eax, eax
    L00567116:
            mov esp, ebp
            pop ebp
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a687
tS3_error_codes C2_HOOK_FASTCALL S3MIDITagHasStoppedPlaying(int pTag) {
    tS3_channel* channel;

    if (gS3_enable_midi) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 1) {
            return PDS3IsMIDIStopped(channel);
        }
    }
    return eS3_error_digi_init;
}

// FUNCTION: CARMA2_HW 0x0056a866
int C2_HOOK_FASTCALL S3ServiceMIDIChannel(tS3_channel* pChannel) {
    if (gS3_enable_midi && pChannel->type == 1) {
        if (S3MIDITagHasStoppedPlaying(pChannel->tag)) {
            S3PlayMIDI(pChannel);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00565d1a
int C2_HOOK_FASTCALL S3IsCDAPlaying(void) {
    return PDS3IsCDAPlaying();
}

// FUNCTION: CARMA2_HW 0x00565d24
int C2_HOOK_FASTCALL S3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {
    if (pVolume < 0) {
        pVolume = 0;
    }
    if (pVolume > 255) {
        pVolume = 255;
    }
    if (gS3_CDA_enabled) {
        PDS3UpdateCDAVolume(pChannel, pVolume);
    }
    return 0;
}

#ifdef _WIN32

/* The QuickTime trap globals are defined in external/dummyqt/src/dummy.c so the
 * STUB trap wrappers there share the loader state.  Keep the historic local
 * spellings as aliases. */
typedef void (C2_HOOK_CDECL *tQuickTime_trap_cbfn)(void);
extern tQuickTime_trap_cbfn gQuickTime_trap_callback;
extern tQuickTime_trap_cbfn gQuickTime_trap_dispatcher;
#define gQuickTime_dispatcher gQuickTime_trap_callback
#define gQuickTime_dispatch gQuickTime_trap_dispatcher

// GLOBAL: CARMA2_HW 0x006736b0
HMODULE gQuickTime_module;

// GLOBAL: CARMA2_HW 0x006736b8
FARPROC gQuickTime_call_component;

// GLOBAL: CARMA2_HW 0x006736bc
FARPROC gQuickTime_call_component_with_storage;

#pragma optimize("gty", on)

// C2PORT-NONMATCHING: best 31.33% | difftest PASS 2000 trials (spec difftest/specs/00575730_FUN_00575730.json) | reason register-allocation/branch-layout
// FUNCTION: CARMA2_HW 0x00575730
void C2_HOOK_CDECL FUN_00575730(void) {
    register int loaded_module;

    loaded_module = 0;
    if (gQuickTime_module == NULL) {
        gQuickTime_module = LoadQuickTimeModule();
        if (gQuickTime_module == NULL) {
            return;
        }
        loaded_module = 1;
    }
    gQuickTime_call_component_with_storage = GetProcAddress(gQuickTime_module, "_CallComponentFunctionWithStorage");
    if (gQuickTime_call_component_with_storage != NULL &&
        (gQuickTime_call_component = GetProcAddress(gQuickTime_module, "_CallComponent")) != NULL &&
        (gQuickTime_dispatch = (tQuickTime_trap_cbfn)GetProcAddress(gQuickTime_module, "theQuickTimeDispatcher")) != NULL) {
        gQuickTime_dispatcher = NULL;
    }
    if (loaded_module) {
        FreeLibrary(gQuickTime_module);
        gQuickTime_module = NULL;
    }
}
#pragma intrinsic(strlen, strcpy, strcat)
#pragma auto_inline(off)
// C2PORT-NONMATCHING: best 47.47% | difftest PASS 2000 trials (spec difftest/specs/005757d0_LoadQuickTimeModule.json) | reason vc5-string-intrinsic-codegen
// FUNCTION: CARMA2_HW 0x005757d0
void* C2_HOOK_CDECL LoadQuickTimeModule(void) {
    register HMODULE module;
    register char* path_separator;
    char path[0x104];

    module = GetModuleHandleA("QuickTime.qts");
    if (module != NULL) {
        if (GetModuleFileNameA(module, path, sizeof(path)) != 0) {
            return LoadLibraryA(path);
        }
    }

    if (GetModuleFileNameA(NULL, path, sizeof(path)) != 0) {
        path_separator = path + strlen(path) - 1;
        while (path_separator >= path && *path_separator != '\\' && *path_separator != '/') {
            path_separator--;
        }
        if (path_separator >= path) {
            path_separator[1] = '\0';
            strcat(path, "QuickTime.qts");
            module = LoadLibraryA(path);
            if (module != NULL) {
                return module;
            }
        }
    }

    module = NULL;
    if (GetSystemDirectoryA(path, sizeof(path)) != 0) {
        strcat(path, "\\");
        strcat(path, "QuickTime.qts");
        module = LoadLibraryA(path);
    }
    return module;
}
#pragma auto_inline(on)
#pragma optimize("", on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0056fde0
C2_NAKED void* C2_HOOK_CDECL FUN_0056fde0(int pArg1, int pArg2) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((void* (C2_HOOK_CDECL *)(unsigned int, int, int))gQuickTime_dispatch)(0x100001, pArg1, pArg2);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056fdef
            call dword ptr [gQuickTime_trap_callback]
    L0056fdef:
            mov eax, 0x100001
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056fe00
void C2_NAKED C2_HOOK_CDECL FUN_0056fe00(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x100002);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056fe0f
            call dword ptr [gQuickTime_trap_callback]
    L0056fe0f:
            mov eax, 0x100002
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c640
Track C2_NAKED C2_HOOK_CDECL FUN_0056c640(Movie pMovie, Fixed pWidth, Fixed pHeight, short pVolume) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Track (C2_HOOK_CDECL *)(unsigned int, Movie, Fixed, Fixed, short))gQuickTime_dispatch)(
        0x20188, pMovie, pWidth, pHeight, pVolume);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c64f
            call dword ptr [gQuickTime_trap_callback]
    L0056c64f:
            mov eax, 0x20188
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c660
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056c660(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x2018a);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c66f
            call dword ptr [gQuickTime_trap_callback]
    L0056c66f:
            mov eax, 0x2018a
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c700
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056c700(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20190);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c70f
            call dword ptr [gQuickTime_trap_callback]
    L0056c70f:
            mov eax, 0x20190
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c740
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056c740(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20192);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c74f
            call dword ptr [gQuickTime_trap_callback]
    L0056c74f:
            mov eax, 0x20192
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}
// FUNCTION: CARMA2_HW 0x0056c820
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056c820(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x2019e);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c82f
            call dword ptr [gQuickTime_trap_callback]
    L0056c82f:
            mov eax, 0x2019e
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}
// FUNCTION: CARMA2_HW 0x0056a900
unsigned int C2_HOOK_CDECL FUN_0056a900(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20002);
}

// FUNCTION: CARMA2_HW 0x0056ac00
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056ac00(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20026);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056ac0f
            call dword ptr [gQuickTime_trap_callback]
    L0056ac0f:
            mov eax, 0x20026
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a920
int C2_NAKED C2_HOOK_CDECL FUN_0056a920(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return GetMoviesError();
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056a92f
            call dword ptr [gQuickTime_trap_callback]
    L0056a92f:
            mov eax, 0x20003
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c6e0
Media C2_NAKED C2_HOOK_CDECL FUN_0056c6e0(Track pTrack, int pMedia_type, int pTime_scale, int pArg4, int pArg5) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Media (C2_HOOK_CDECL *)(unsigned int, Track, int, int, int, int))gQuickTime_dispatch)(
        0x2018e, pTrack, pMedia_type, pTime_scale, pArg4, pArg5);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c6ef
            call dword ptr [gQuickTime_trap_callback]
    L0056c6ef:
            mov eax, 0x2018e
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056b340
int C2_HOOK_CDECL FUN_0056b340(Media pMedia) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return BeginMediaEdits(pMedia);
}

// FUNCTION: CARMA2_HW 0x00570600
Handle C2_NAKED C2_HOOK_CDECL FUN_00570600(int pSize) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Handle (C2_HOOK_CDECL *)(unsigned int, int))gQuickTime_dispatch)(0x150008, pSize);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0057060f
            call dword ptr [gQuickTime_trap_callback]
    L0057060f:
            mov eax, 0x150008
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005705c0
int C2_NAKED C2_HOOK_CDECL FUN_005705c0(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150006);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L005705cf
            call dword ptr [gQuickTime_trap_callback]
    L005705cf:
            mov eax, 0x150006
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005705e0
void C2_NAKED C2_HOOK_CDECL FUN_005705e0(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150007);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L005705ef
            call dword ptr [gQuickTime_trap_callback]
    L005705ef:
            mov eax, 0x150007
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00570620
void C2_NAKED C2_HOOK_CDECL FUN_00570620(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150009);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0057062f
            call dword ptr [gQuickTime_trap_callback]
    L0057062f:
            mov eax, 0x150009
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056d7e0
int C2_NAKED C2_HOOK_CDECL FUN_0056d7e0(Handle pPixmap, struct Rect* pRect, int pArg3, int pQuality, int pCodec_type, int pArg6, int* pMax_size) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((int (C2_HOOK_CDECL *)(unsigned int, Handle, struct Rect*, int, int, int, int, int*))gQuickTime_dispatch)(
        0x30004, pPixmap, pRect, pArg3, pQuality, pCodec_type, pArg6, pMax_size);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056d7ef
            call dword ptr [gQuickTime_trap_callback]
    L0056d7ef:
            mov eax, 0x30004
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00570a40
void C2_NAKED C2_HOOK_CDECL FUN_00570a40(Handle pHandle) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int, Handle))gQuickTime_dispatch)(0x15002e, pHandle);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L00570a4f
            call dword ptr [gQuickTime_trap_callback]
    L00570a4f:
            mov eax, 0x15002e
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00570a60
void C2_NAKED C2_HOOK_CDECL FUN_00570a60(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x15002f);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L00570a6f
            call dword ptr [gQuickTime_trap_callback]
    L00570a6f:
            mov eax, 0x15002f
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00570ae0
void C2_NAKED C2_HOOK_CDECL FUN_00570ae0(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150033);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L00570aef
            call dword ptr [gQuickTime_trap_callback]
    L00570aef:
            mov eax, 0x150033
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005707e0
void C2_NAKED C2_HOOK_CDECL FUN_005707e0(Handle pHandle) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int, Handle))gQuickTime_dispatch)(0x150019, pHandle);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L005707ef
            call dword ptr [gQuickTime_trap_callback]
    L005707ef:
            mov eax, 0x150019
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00570800
void C2_NAKED C2_HOOK_CDECL FUN_00570800(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x15001a);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0057080f
            call dword ptr [gQuickTime_trap_callback]
    L0057080f:
            mov eax, 0x15001a
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056d8a0
int C2_NAKED C2_HOOK_CDECL FUN_0056d8a0(int* pImage_sequence, Handle pPixmap, int pArg3, struct Rect* pSource_rect, struct Rect* pDest_rect, int pArg6, int pCodec_type, int pArg8, int pSpatial_quality, int pTemporal_quality, int pArg11, int pArg12, int pArg13, Handle pImage_description) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((int (C2_HOOK_CDECL *)(unsigned int, int*, Handle, int, struct Rect*, struct Rect*, int, int, int, int, int, int, int, int, Handle))gQuickTime_dispatch)(
        0x3000a, pImage_sequence, pPixmap, pArg3, pSource_rect, pDest_rect, pArg6, pCodec_type, pArg8,
        pSpatial_quality, pTemporal_quality, pArg11, pArg12, pArg13, pImage_description);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056d8af
            call dword ptr [gQuickTime_trap_callback]
    L0056d8af:
            mov eax, 0x3000a
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056d8c0
short C2_NAKED C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return (short)((short (C2_HOOK_CDECL *)(unsigned int, int, void*, void*, int, void*, int*, void*, void*))gQuickTime_dispatch)(
        0x3000b, pSequence, pFrame_pixmap, pFrame_rect, pFlags, pCompressed_frame, pFrame_size, pCodec_info, pUnused8);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056d8cf
            call dword ptr [gQuickTime_trap_callback]
    L0056d8cf:
            mov eax, 0x3000b
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056d8e0
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056d8e0(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x3000c);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056d8ef
            call dword ptr [gQuickTime_trap_callback]
    L0056d8ef:
            mov eax, 0x3000c
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056dae0
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056dae0(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x3001c);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056daef
            call dword ptr [gQuickTime_trap_callback]
    L0056daef:
            mov eax, 0x3001c
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056e760
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056e760(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x1d0008);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056e76f
            call dword ptr [gQuickTime_trap_callback]
    L0056e76f:
            mov eax, 0x1d0008
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056e780
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056e780(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x1d0009);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056e78f
            call dword ptr [gQuickTime_trap_callback]
    L0056e78f:
            mov eax, 0x1d0009
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056b440
short C2_NAKED C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return (short)((unsigned int (C2_HOOK_CDECL *)(unsigned int, void*, void*, int, int, int, void*, int, int, int))gQuickTime_dispatch)(
        0x2007c, pMedia, pCompressed_frame, pOffset, pSize, pDuration,
        pImage_description, pSample_count, pSample_flags, pSample_time);
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056b44f
            call dword ptr [gQuickTime_trap_callback]
    L0056b44f:
            mov eax, 0x2007c
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}
#pragma auto_inline(on)

#else
/* QuickTime.qts is a Windows DLL. Movie recording is unavailable on these
 * platforms (InitializeQTML already reports failure); fail safely if called.
 * Game cutscenes use the separate Smacker decoder. */
void C2_HOOK_CDECL FUN_00575730(void) {
}

void* C2_HOOK_CDECL LoadQuickTimeModule(void) {
    return NULL;
}

void* C2_HOOK_CDECL FUN_0056fde0(int pArg1, int pArg2) {
    return NULL;
}

void C2_HOOK_CDECL FUN_0056fe00(void) {
}

Track C2_HOOK_CDECL FUN_0056c640(Movie pMovie, Fixed pWidth, Fixed pHeight, short pVolume) {
    return NULL;
}

unsigned int C2_HOOK_CDECL FUN_0056c660(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056c700(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056c740(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056c820(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056a900(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056ac00(void) {
    return 0;
}

int C2_HOOK_CDECL FUN_0056a920(void) {
    return 1; /* QuickTime unavailable. */
}

Media C2_HOOK_CDECL FUN_0056c6e0(Track pTrack, int pMedia_type, int pTime_scale, int pArg4, int pArg5) {
    return NULL;
}

int C2_HOOK_CDECL FUN_0056b340(Media pMedia) {
    return 1; /* QuickTime unavailable. */
}

Handle C2_HOOK_CDECL FUN_00570600(int pSize) {
    return NULL;
}

int C2_HOOK_CDECL FUN_005705c0(void) {
    return 1; /* QuickTime unavailable. */
}

void C2_HOOK_CDECL FUN_005705e0(void) {
}

void C2_HOOK_CDECL FUN_00570620(void) {
}

int C2_HOOK_CDECL FUN_0056d7e0(Handle pPixmap, struct Rect* pRect, int pArg3, int pQuality, int pCodec_type, int pArg6, int* pMax_size) {
    return 1; /* QuickTime unavailable. */
}

void C2_HOOK_CDECL FUN_00570a40(Handle pHandle) {
}

void C2_HOOK_CDECL FUN_00570a60(void) {
}

void C2_HOOK_CDECL FUN_00570ae0(void) {
}

void C2_HOOK_CDECL FUN_005707e0(Handle pHandle) {
}

void C2_HOOK_CDECL FUN_00570800(void) {
}

int C2_HOOK_CDECL FUN_0056d8a0(int* pImage_sequence, Handle pPixmap, int pArg3, struct Rect* pSource_rect, struct Rect* pDest_rect, int pArg6, int pCodec_type, int pArg8, int pSpatial_quality, int pTemporal_quality, int pArg11, int pArg12, int pArg13, Handle pImage_description) {
    return 1; /* QuickTime unavailable. */
}

short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    return 1; /* QuickTime unavailable. */
}

unsigned int C2_HOOK_CDECL FUN_0056d8e0(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056dae0(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056e760(void) {
    return 0;
}

unsigned int C2_HOOK_CDECL FUN_0056e780(void) {
    return 0;
}

short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    return 1; /* QuickTime unavailable. */
}
#endif

// FUNCTION: CARMA2_HW 0x0056432c
double C2_HOOK_STDCALL S3FRandomBetween2(double pMin, double pMax) {

    return (pMax - pMin) * (double)rand() / 32767.0 + pMin;
}

// FUNCTION: CARMA2_HW 0x005642b5
int C2_HOOK_FASTCALL S3IRandomBetweenLog__dup(int pMin, int pMax, int pDefault) {
    double dbl;

    if (pMin == -1 || pMin >= pMax) {
        return pDefault;
    } else {
        dbl = exp(S3FRandomBetween2(log(pMin), log(pMax)));
        return (int)(ldexp(dbl, -16) * pDefault);
    }
}

// FUNCTION: CARMA2_HW 0x00564d0a
void C2_HOOK_FASTCALL S3ToggleEnabled(void) {
    if (gS3_enabled) {
        S3StopAllOutletSounds();
        gS3_enabled = 0;
    } else {
        gS3_enabled = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00564d33
int C2_HOOK_FASTCALL FUN_00564d33(void) {
    if (gS3_enabled != 0) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005656ea
int C2_HOOK_FASTCALL S3GetTerminationReason(tS3_channel* pChannel) {
    return pChannel->termination_reason;
}

// FUNCTION: CARMA2_HW 0x005656fb
int C2_HOOK_FASTCALL S3GetRepetitions(tS3_channel* pChannel) {
    return pChannel->repetitions;
}

// FUNCTION: CARMA2_HW 0x0056570c
int C2_HOOK_FASTCALL S3CountActiveChannels(tS3_outlet* pOutlet) {
    int count;
    tS3_channel* channel;

    count = 0;
    if (gS3_enabled != 0) {
        channel = pOutlet->channel_list;
        while (channel != NULL) {
            if (channel->active != 0) {
                count += 1;
            }
            channel = channel->next;
        }
        return count;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056583f
int C2_HOOK_FASTCALL S3GetLastError(void) {
    return gS3_last_error;
}

// FUNCTION: CARMA2_HW 0x00565849
int C2_HOOK_FASTCALL S3SetChannelTag(tS3_channel* pChannel, tS3_sound_tag pTag) {
    pChannel->tag = pTag;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00565b93
void C2_HOOK_FASTCALL S3ToggleCDA(void) {
    if (gS3_CDA_enabled) {
        S3StopCDAInternal();
        gS3_CDA_enabled = 0;
    } else {
        gS3_CDA_enabled = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00566480
int C2_HOOK_FASTCALL S3SetChannelPosition(tS3_sound_tag pChannel_tag, const tS3_vector3* pPosition) {
    tS3_channel* channel;

    if (gS3_enabled != 0) {
        channel = S3GetChannelForTag(pChannel_tag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->spatial_sound == 0) {
            return 11;
        }
        channel->position.x = pPosition->x;
        channel->position.y = pPosition->y;
        channel->position.z = pPosition->z;
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x005664e6
int C2_HOOK_FASTCALL S3SetChannelVelocity(tS3_sound_tag pChannel_tag, const tS3_vector3* pVelocity) {
    tS3_channel* channel;

    if (gS3_enabled != 0) {
        channel = S3GetChannelForTag(pChannel_tag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->spatial_sound == 0) {
            return 11;
        }
        channel->velocity.x = pVelocity->x;
        channel->velocity.y = pVelocity->y;
        channel->velocity.z = pVelocity->z;
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00566686
int C2_HOOK_FASTCALL S3SetSoundSourceBinding(tS3_sound_source* pSource, void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet) {
    tS3_sound_source* iter;
    tS3_sound_source* src;
    int found;

    found = 0;
    if (pSource == NULL) {
        return eS3_error_nonexistant_source;
    }
    iter = gS3_state.sources;
    if (iter == pSource) {
        found = 1;
    } else {
        while (iter->next != NULL && !found) {
            iter = iter->next;
            if (iter == pSource) {
                found = 1;
            }
        }
    }
    if (!found) {
        return eS3_error_nonexistant_source;
    }
    src = pSource;
    src->bound_outlet = pBound_outlet;
    src->position_ptr = pPosition;
    src->velocity_ptr = pVelocity;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00568a11
void C2_HOOK_FASTCALL S3LoadSampleList(int* pSample_ids, int pCount) {
    int sample_id;

    if (gS3_enabled != 0) {
        while (pCount != 0) {
            sample_id = *pSample_ids;
            S3LoadSample(sample_id);
            pSample_ids++;
            pCount--;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00568a54
void C2_HOOK_FASTCALL S3ReleaseSoundList(int* pSound_ids, int pSound_count) {
    int sound_id;

    if (gS3_enabled != 0) {
        while (pSound_count != 0) {
            sound_id = *pSound_ids;
            S3ReleaseSound(sound_id);
            pSound_ids++;
            pSound_count = pSound_count - 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00568a97
void C2_NAKED C2_HOOK_FASTCALL S3UpdateSampleList(int* pOld_sample_ids, int pOld_count, int* pNew_sample_ids, int pNew_count) {
#ifndef CARPOCALYPSE2_MATCHING
    int old_sample_id;
    int new_count_remaining;
    int old_count_remaining;
    int new_sample_id;
    int found;

    if (gS3_enabled != 0) {
        old_count_remaining = pOld_count;
        old_sample_id = pOld_sample_ids[old_count_remaining - 1];
        while (old_count_remaining != 0) {
            new_count_remaining = pNew_count;
            new_sample_id = pNew_sample_ids[new_count_remaining - 1];
            found = 0;
            while (new_count_remaining != 0 && found == 0) {
                if (old_sample_id == new_sample_id) {
                    found = 1;
                }
                new_count_remaining--;
                new_sample_id = pNew_sample_ids[new_count_remaining - 1];
            }
            if (found == 0) {
                S3ReleaseSound(pOld_sample_ids[old_count_remaining - 1]);
            }
            old_count_remaining--;
            old_sample_id = pOld_sample_ids[old_count_remaining - 1];
        }
        new_count_remaining = pNew_count;
        new_sample_id = pNew_sample_ids[new_count_remaining - 1];
        while (new_count_remaining != 0) {
            old_sample_id = pOld_sample_ids[pOld_count - 1];
            found = 0;
            old_count_remaining = pOld_count;
            while (old_count_remaining != 0 && found == 0) {
                if (new_sample_id == old_sample_id) {
                    found = 1;
                }
                old_count_remaining--;
                old_sample_id = pOld_sample_ids[old_count_remaining - 1];
            }
            if (found == 0) {
                S3LoadSample(pNew_sample_ids[new_count_remaining - 1]);
            }
            new_count_remaining--;
            new_sample_id = pNew_sample_ids[new_count_remaining - 1];
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x1c
            mov dword ptr [ebp - 0x1c], edx
            mov dword ptr [ebp - 0x18], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00568bd6
            mov eax, dword ptr [ebp - 0x1c]
            mov dword ptr [ebp - 8], eax
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 0x18]
            mov eax, dword ptr [edx + ecx*4 - 4]
            mov dword ptr [ebp - 0x14], eax
    L00568ac3:
            cmp dword ptr [ebp - 8], 0
            je L00568b43
            mov ecx, dword ptr [ebp + 0xc]
            mov dword ptr [ebp - 0x10], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + edx*4 - 4]
            mov dword ptr [ebp - 4], ecx
            mov dword ptr [ebp - 0xc], 0
    L00568ae3:
            cmp dword ptr [ebp - 0x10], 0
            je L00568b16
            cmp dword ptr [ebp - 0xc], 0
            jne L00568b16
            mov edx, dword ptr [ebp - 0x14]
            cmp edx, dword ptr [ebp - 4]
            jne L00568afe
            mov dword ptr [ebp - 0xc], 1
    L00568afe:
            mov eax, dword ptr [ebp - 0x10]
            sub eax, 1
            mov dword ptr [ebp - 0x10], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + ecx*4 - 4]
            mov dword ptr [ebp - 4], eax
            jmp L00568ae3
    L00568b16:
            cmp dword ptr [ebp - 0xc], 0
            jne L00568b2b
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [edx + ecx*4 - 4]
            call S3ReleaseSound
    L00568b2b:
            mov eax, dword ptr [ebp - 8]
            sub eax, 1
            mov dword ptr [ebp - 8], eax
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 0x18]
            mov eax, dword ptr [edx + ecx*4 - 4]
            mov dword ptr [ebp - 0x14], eax
            jmp L00568ac3
    L00568b43:
            mov ecx, dword ptr [ebp + 0xc]
            mov dword ptr [ebp - 0x10], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + edx*4 - 4]
            mov dword ptr [ebp - 4], ecx
    L00568b56:
            cmp dword ptr [ebp - 0x10], 0
            je L00568bd6
            mov edx, dword ptr [ebp - 0x1c]
            mov dword ptr [ebp - 8], edx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [ecx + eax*4 - 4]
            mov dword ptr [ebp - 0x14], edx
            mov dword ptr [ebp - 0xc], 0
    L00568b76:
            cmp dword ptr [ebp - 8], 0
            je L00568ba9
            cmp dword ptr [ebp - 0xc], 0
            jne L00568ba9
            mov eax, dword ptr [ebp - 4]
            cmp eax, dword ptr [ebp - 0x14]
            jne L00568b91
            mov dword ptr [ebp - 0xc], 1
    L00568b91:
            mov ecx, dword ptr [ebp - 8]
            sub ecx, 1
            mov dword ptr [ebp - 8], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + edx*4 - 4]
            mov dword ptr [ebp - 0x14], ecx
            jmp L00568b76
    L00568ba9:
            cmp dword ptr [ebp - 0xc], 0
            jne L00568bbe
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + edx*4 - 4]
            call S3LoadSample
    L00568bbe:
            mov ecx, dword ptr [ebp - 0x10]
            sub ecx, 1
            mov dword ptr [ebp - 0x10], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + edx*4 - 4]
            mov dword ptr [ebp - 4], ecx
            jmp L00568b56
    L00568bd6:
            mov esp, ebp
            pop ebp
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00569d5a
int C2_HOOK_FASTCALL FUN_00569d5a(int pVolume) {
    (void) pVolume;

    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a410
void C2_HOOK_FASTCALL S3EnableMidi(void) {
    gS3_enable_midi = 1;
}

// FUNCTION: CARMA2_HW 0x0056a433
void C2_HOOK_FASTCALL S3ToggleMidiEnabled(void) {
    if (gS3_enable_midi) {
        S3StopMidiInternal();
        gS3_enable_midi = 0;
    } else {
        gS3_enable_midi = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0056a45c
int C2_HOOK_CDECL S3IsMidiEnabled(void) {
    if (gS3_enable_midi) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a548
int C2_HOOK_FASTCALL FUN_0056a548(int pVolume) {
    if (pVolume < 0) {
        pVolume = 0;
    }
    if (pVolume > 255) {
        pVolume = 255;
    }
    if (gS3_enable_midi != 0) {
        FUN_00569d5a(pVolume);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a8a0
void C2_HOOK_FASTCALL S3SetCallbacks(const tS3_callbacks* pCallbacks) {
    gS3_callbacks.field_0x0 = pCallbacks->field_0x0;
    gS3_callbacks.on_sample_channel_stop = pCallbacks->on_sample_channel_stop;
    gS3_callbacks.on_sample_channel_set_volume = pCallbacks->on_sample_channel_set_volume;
    gS3_callbacks.on_sample_channel_set_pan = pCallbacks->on_sample_channel_set_pan;
}

tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceInternal(void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet);
int C2_HOOK_FASTCALL FUN_0056a6cd(int pTag, int pPitch);
tS3_error_codes C2_HOOK_FASTCALL FUN_00564a56(tS3_sound_tag pChannel_tag, int pPitch);
int C2_HOOK_FASTCALL S3ChangePitchSpeed(tS3_sound_tag pTag, int pPitch);

// FUNCTION: CARMA2_HW 0x0056654c
tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSource(void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet) {
    tS3_sound_source* source;

    if (gS3_enabled != 0) {
        source = S3CreateSoundSourceInternal(pPosition, pVelocity, pBound_outlet);
        if (source != NULL) {
            source->brender_vector = 0;
        }
        return source;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005648c3
tS3_error_codes C2_HOOK_FASTCALL S3SetChannelPitch(tS3_sound_tag pChannel_tag, int pPitch) {
    tS3_channel* channel;

    if (gS3_enabled) {
        if (pPitch == -1) {
            pPitch = 0x10000;
        }
        channel = S3GetChannelForTag(pChannel_tag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 0) {
            channel->rate = (int)((double)channel->descriptor->buffer_description->sample_rate
                    * ldexp((double)pPitch, -16));
            if (!PDS3UpdateChannelFrequency(channel)) {
                return eS3_error_function_failed;
            }
        }
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00564a21
int C2_HOOK_FASTCALL FUN_00564a21(tS3_sound_tag pTag, int pPitch) {
    if (gS3_enabled) {
        if (pPitch == -1) {
            pPitch = 0x10000;
        }
        return S3ChangePitchSpeed(pTag, pPitch);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00564a56
tS3_error_codes C2_HOOK_FASTCALL FUN_00564a56(tS3_sound_tag pChannel_tag, int pPitch) {
    tS3_channel* channel;

    if (gS3_enabled) {
        if (pPitch == -1) {
            pPitch = 0x10000;
        }
        channel = S3GetChannelForTag(pChannel_tag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 0) {
            channel->rate = (int)((double)channel->descriptor->buffer_description->sample_rate
                    * ldexp((double)pPitch, -16));
            if (!PDS3UpdateChannelFrequency(channel)) {
                return eS3_error_function_failed;
            }
        }
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00565c82
int C2_HOOK_FASTCALL FUN_00565c82(int pTag) {
    tS3_channel* channel;
    /* name order decides the VC5 stack slots (zresult at ebp-8) */
    int zresult;

    zresult = 0;
    if (gS3_CDA_enabled != 0) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return 7;
        } else if (channel->type == 2) {
            zresult = PDS3PauseCDA((undefined4)(uintptr_t)channel);
        }
    }
    return zresult;
}

// FUNCTION: CARMA2_HW 0x00565cce
int C2_HOOK_FASTCALL S3ResumeCDAChannel(int pTag) {
    struct {
        int result;
        tS3_channel* channel;
    } locals;

    locals.result = 0;
    if (gS3_CDA_enabled) {
        locals.channel = S3GetChannelForTag(pTag);
        if (locals.channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (locals.channel->type == 2) {
            locals.result = PDS3ResumeCDA(locals.channel);
        }
    }
    return locals.result;
}

// FUNCTION: CARMA2_HW 0x005696f9
int C2_HOOK_FASTCALL S3ChangePitchSpeed(tS3_sound_tag pTag, int pPitch) {
    tS3_channel* channel;

    channel = S3GetChannelForTag(pTag);
    if (channel == NULL) {
        return eS3_error_bad_stag;
    }
    if (channel->type == 0) {
        return FUN_00564a56(pTag, pPitch);
    }
    if (channel->type == 1) {
        return FUN_0056a6cd(pTag, pPitch);
    }
    return eS3_error_digi_init;
}

// FUNCTION: CARMA2_HW 0x0056a583
int C2_HOOK_FASTCALL FUN_0056a583(int pTag) {
    tS3_channel* channel;

    if (gS3_enable_midi != 0) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return 7;
        }
        if (channel->type == 1) {
            PDS3MuteMidiChannel(channel);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a5c4
int C2_HOOK_FASTCALL S3UnmuteMidiChannel(int pTag) {
    tS3_channel* channel;

    if (gS3_enable_midi) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 1) {
            PDS3UnmuteMidiChannel(channel);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a605
int C2_HOOK_FASTCALL FUN_0056a605(int pTag) {
    tS3_channel* channel;

    if (gS3_enable_midi != 0) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return 7;
        }
        if (channel->type == 1) {
            PDS3PauseMidiChannel(channel);
        }
    }
    return 0;
}

extern int C2_HOOK_FASTCALL PDS3StopMidiChannelWrapper(tS3_channel* pChannel, int pArg2, int pArg3, int pArg4, int pArg5, int pArg6);

// FUNCTION: CARMA2_HW 0x0056a646
int C2_HOOK_FASTCALL S3ResumeMidiChannel(int pTag) {
    tS3_channel* channel;

    if (gS3_enable_midi) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 1) {
            PDS3ResumeMidiChannel(channel);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a6cd
int C2_HOOK_FASTCALL FUN_0056a6cd(int pTag, int pPitch) {
    tS3_channel* channel;

    if (gS3_enable_midi != 0) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 1 && PDS3UpdateMidiChannelPitch(channel, pPitch) == 0) {
            return eS3_error_function_failed;
        }
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x0056a71f
int C2_HOOK_FASTCALL FUN_0056a71f(int pTag, int pArg2, int pArg3, int pArg4, int pArg5) {
    int arg6;
    tS3_channel* channel;

    arg6 = 0x14;
    if (gS3_enable_midi != 0) {
        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return 7;
        }
        if (pArg2 == 2) {
            channel->needs_service = 1;
            if (channel->type == 1 && (channel->descriptor->flags & 2) != 0) {
                return S3StopMIDIChannel(channel);
            }
        }
        if (channel->type == 1) {
            if (PDS3StopMidiChannelWrapper(channel, pArg2, pArg3, pArg4, pArg5, arg6) == 0) {
                return 0xc;
            }
        }
    }
    return 0;
}
