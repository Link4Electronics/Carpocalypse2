#include "win32_sound.h"

#include "platform.h"
#include "s3/internal/audio.h"
#include "carpocalypse2_macros.h"

// GLOBAL: CARMA2_HW 0x006aaa1c
LPDIRECTSOUND gDirectSound;

// GLOBAL: CARMA2_HW 0x006b2d9c
LPDIRECTSOUND gPD_S3_direct_sound;

// GLOBAL: CARMA2_HW 0x007a00e0
int gPDS3_volume_factors[256];

// GLOBAL: CARMA2_HW 0x007a0080
tPD_S3_config gPD_S3_config;

// GLOBAL: CARMA2_HW 0x007a0500
MCI_OPEN_PARMS gPDS3_mci_open_parms;

// GLOBAL: CARMA2_HW 0x0079fec0
MCI_SET_PARMS gPDS3_mci_set_parms;

// GLOBAL: CARMA2_HW 0x007a00a0
MCI_SET_PARMS gPDS3_mci_audio_parms;

// GLOBAL: CARMA2_HW 0x007a00c0
MCI_SEQ_SET_PARMS gPDS3_mci_seq_set_parms;

// GLOBAL: CARMA2_HW 0x006b2d98
int gPDS3_cda_initialized;

// GLOBAL: CARMA2_HW 0x006b2da0
int gS3_working_directory_initialized;

// GLOBAL: CARMA2_HW 0x006b2c90
char gS3_working_directory[260];

// GLOBAL: CARMA2_HW 0x006b2da4
int gPDS3_midi_playing;

// GLOBAL: CARMA2_HW 0x006b2da8
int gPDS3_midi_restart_required;

// GLOBAL: CARMA2_HW 0x006b2dac
int gPDS3_cda_media_present;

// GLOBAL: CARMA2_HW 0x006b2db0
int gPDS3_cda_is_playing;

// GLOBAL: CARMA2_HW 0x006b2db4
int gPDS3_cda_paused;

// GLOBAL: CARMA2_HW 0x006b2db8
int gPDS3_cda_track;

// GLOBAL: CARMA2_HW 0x007a0520
MCI_STATUS_PARMS gPDS3_cda_status_parms;

// GLOBAL: CARMA2_HW 0x007a00ac
int gPDS3_Number_of_tracks;

// GLOBAL: CARMA2_HW 0x007a0070
MCI_STATUS_PARMS gPDS3_mci_midi_status_parms;

// GLOBAL: CARMA2_HW 0x006b2dc4
tU32 gPDS3_last_cda_status_update;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type);
extern void C2_HOOK_FASTCALL S3MemFree(void* p);
extern tS3_callbacks gS3_callbacks;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume);
extern int C2_HOOK_FASTCALL S3ServiceChannel(tS3_channel *pChannel);
extern void __cdecl _ftol();
extern void* gHWnd;
extern int gS3_CDA_enabled;
extern tS3_sample_filter gS3_sample_filter_disable_func;
extern tS3_state gS3_state;
extern MCI_OPEN_PARMS mci_open_params;
extern MCI_PLAY_PARMS mci_play_parms;
extern char rendererStateStoredDispatch[];
#endif
// FUNCTION: CARMA2_HW 0x0056992f
int C2_HOOK_FASTCALL PDS3Init(void) {

    C2_HOOK_BUG_ON(sizeof(gPD_S3_config) != 0x20);

    strcpy(gS3_path_separator, "\\");
    strcpy(gS3_sound_folder_name, "SOUND");
    memset(&gPD_S3_config, 0, sizeof(gPD_S3_config));
    if (!PDS3DDXInit()) {
        return 0;
    }
    PDS3InitCDA();
    PDS3Return0();
    gPD_S3_config.field_0x04 = 0;
    gPD_S3_config.free_direct_sound = 1;
    gPD_S3_config.independent_pitch = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569a2c
int C2_HOOK_FASTCALL PDS3DDXInit(void) {
#ifdef DSOUND_ENABLED
    int i;

    if (FAILED(DirectSoundCreate(0, &gPD_S3_direct_sound, NULL))) {
        return 0;
    }
    if (FAILED(IDirectSound_SetCooperativeLevel(gPD_S3_direct_sound, gHWnd, DSSCL_EXCLUSIVE))) {
        return 0;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPDS3_volume_factors); i++) {

        gPDS3_volume_factors[i] = (int)(-(9999.0 - (log10(((double)i + 1.0) * 9999.0 / 256.0) * 9999.0 / 3.0 - 4300.0)));
    }
    S3Enable();
    return 1;
#else
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x005699a3
int C2_HOOK_FASTCALL PDS3Return0(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x005699aa
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

    gPDS3_mci_open_parms.lpstrDeviceType = (LPCSTR)(uintptr_t)MCI_DEVTYPE_CD_AUDIO;
    if (mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR)&gPDS3_mci_open_parms) != 0
         && mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&gPDS3_mci_open_parms) != 0) {

        return 0;
    }
    gPDS3_mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&gPDS3_mci_set_parms);
    S3EnableCDA();
    gPDS3_cda_initialized = 1;
    return 1;
}

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer) {
    LPDIRECTSOUNDBUFFER buffer;
    DSBUFFERDESC buffer_desc;
    void* buffer_data;
    DWORD buffer_data_size;

    C2_HOOK_BUG_ON(sizeof(DSBUFFERDESC) != 0x14);
    memset(&buffer_desc, 0, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
    buffer_desc.dwBufferBytes = pWav_info->sample_size;
    buffer_desc.lpwfxFormat = (LPWAVEFORMATEX)pWav_info->wav_info_header;
    if (FAILED(IDirectSound_CreateSoundBuffer(gPD_S3_direct_sound, &buffer_desc, &buffer, NULL))) {
        return NULL;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, pWav_info->sample_size, &buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        IDirectSoundBuffer_Release(buffer);
        return NULL;
    }
    memmove(buffer_data, pWav_info->samples, buffer_data_size);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return buffer;
}

// FUNCTION: CARMA2_HW 0x005693d9
void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {
    LPDIRECTSOUNDBUFFER buffer = pSound_descriptor->pd_handle;
    if (buffer != NULL) {
        IDirectSoundBuffer_Release(buffer);
    }
}

// FUNCTION: CARMA2_HW 0x00569401
int C2_NAKED C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING
    tU8* buffer_data;
    DWORD buffer_data_size;
    DWORD i;
    tU8* tmp_buffer;
    DWORD sample_size;

    LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
    if (buffer == NULL) {
        return 1;
    }
    sample_size = pChannel->descriptor->buffer_description->sample_size;
    if (sample_size > 2000000) {
        return 0;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, sample_size, (void**)&buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        return 0;
    }
    tmp_buffer = BrMemAllocate(sample_size, kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample);
    if (tmp_buffer == NULL) {
        IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
        return 1;
    }
    memmove(tmp_buffer, buffer_data, buffer_data_size);
    for (i = 0; i < buffer_data_size; i++) {
        buffer_data[i] = tmp_buffer[sample_size - 1 - i];
    }
    BrMemFree(tmp_buffer);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x24
            mov dword ptr [ebp - 0x24], ecx
            mov dword ptr [ebp - 0xc], 0
            mov eax, dword ptr [ebp - 0x24]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x48]
            mov dword ptr [ebp - 0x14], edx
            cmp dword ptr [ebp - 0x14], 0
            jne L0056942d
            mov eax, 1
            jmp L00569503
    L0056942d:
            mov eax, dword ptr [ebp - 0x24]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x40]
            mov dword ptr [ebp - 8], edx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0x20], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx]
            mov dword ptr [ebp - 4], eax
            push 0
            push 0
            push 0
            lea ecx, [ebp - 0x18]
            push ecx
            lea edx, [ebp - 0xc]
            push edx
            mov eax, dword ptr [ebp - 4]
            push eax
            push 0
            mov ecx, dword ptr [ebp - 0x14]
            push ecx
            mov edx, dword ptr [ebp - 0x14]
            mov eax, dword ptr [edx]
            call dword ptr [eax + 0x2c]
            test eax, eax
            jne L00569501
            cmp dword ptr [ebp - 4], 0x1e8480
            jbe L00569482
            xor eax, eax
            jmp L00569503
    L00569482:
            mov dl, 0xf0
            mov ecx, dword ptr [ebp - 4]
            call S3MemAllocate
            mov dword ptr [ebp - 0x1c], eax
            cmp dword ptr [ebp - 0x1c], 0
            jne L0056949c
            mov eax, 1
            jmp L00569503
    L0056949c:
            mov ecx, dword ptr [ebp - 0x18]
            push ecx
            mov edx, dword ptr [ebp - 0xc]
            push edx
            mov eax, dword ptr [ebp - 0x1c]
            push eax
            call memmove
            add esp, 0xc
            mov dword ptr [ebp - 0x10], 0
            jmp L005694c2
    L005694b9:
            mov ecx, dword ptr [ebp - 0x10]
            add ecx, 1
            mov dword ptr [ebp - 0x10], ecx
    L005694c2:
            mov edx, dword ptr [ebp - 0x10]
            cmp edx, dword ptr [ebp - 0x18]
            jge L005694e1
            mov eax, dword ptr [ebp - 4]
            sub eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 0xc]
            add ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ebp - 0x1c]
            mov al, byte ptr [edx + eax - 1]
            mov byte ptr [ecx], al
            jmp L005694b9
    L005694e1:
            mov ecx, dword ptr [ebp - 0x1c]
            call S3MemFree
            push 0
            push 0
            mov ecx, dword ptr [ebp - 0x18]
            push ecx
            mov edx, dword ptr [ebp - 0xc]
            push edx
            mov eax, dword ptr [ebp - 0x14]
            push eax
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ecx]
            call dword ptr [edx + 0x4c]
    L00569501:
            xor eax, eax
    L00569503:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00569b29
int C2_HOOK_FASTCALL PDS3Return0_B(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569b30
const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {

    if (!gS3_working_directory_initialized) {
        DWORD len_res;
        size_t len;

        C2_HOOK_BUG_ON(sizeof(gS3_working_directory) != 0x104);

        gS3_working_directory[0] = '\0';
        len_res = GetCurrentDirectoryA(sizeof(gS3_working_directory), gS3_working_directory);
        len = strlen(gS3_working_directory);
        if (len_res != len) {
            gS3_working_directory[0] = '\0';
        }
        gS3_working_directory_initialized = 1;
    }
    return gS3_working_directory;
}

// FUNCTION: CARMA2_HW 0x00569cb1
int C2_NAKED C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gPDS3_midi_playing) {
        gPDS3_midi_playing = 0;
        mciSendCommandA(pChannel->mciDevice, MCI_CLOSE, 0, 0);
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dword ptr [ebp - 8], ecx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [ebp - 4], ecx
            cmp dword ptr [gPDS3_midi_playing], 0
            je L00569ce9
            mov dword ptr [gPDS3_midi_playing], 0
            push 0
            push 0
            push 0x804
            mov edx, dword ptr [ebp - 4]
            push edx
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
    L00569ce9:
            mov eax, dword ptr [ebp - 8]
            cmp dword ptr [eax], 0
            je L00569cfb
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 8], 1
    L00569cfb:
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00569d01
int C2_HOOK_FASTCALL FUN_00569d01(tS3_channel* pChannel) {
    MCIDEVICEID mci_device;

    mci_device = (MCIDEVICEID)pChannel->mciDevice;
    if (gPDS3_midi_playing != 0) {
        gPDS3_midi_playing = 0;
        mciSendCommandA(mci_device, MCI_CLOSE, 0, 0);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569de5
int C2_HOOK_FASTCALL PDS3MuteMidiChannel(tS3_channel* pChannel) {
    MCIDEVICEID mci_device;

    mci_device = (MCIDEVICEID)pChannel->mciDevice;
    if (gPDS3_midi_playing != 0) {
        gPDS3_mci_audio_parms.dwAudio = MCI_SET_AUDIO_ALL;
        mciSendCommandA(mci_device, MCI_SET, MCI_SET_AUDIO | MCI_SET_OFF, (DWORD_PTR)&gPDS3_mci_audio_parms);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a0e1
void C2_HOOK_FASTCALL PDS3CheckCDAMedia(void) {
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MEDIA_PRESENT;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (!gPDS3_cda_media_present && gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 1;
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
        gPDS3_Number_of_tracks = MIN((int)gPDS3_cda_status_parms.dwReturn, 99);
    } else if (gPDS3_cda_media_present && !gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 0;
        gPDS3_cda_paused = 0;
    }

}

// FUNCTION: CARMA2_HW 0x0056a093
int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel) {

    PDS3CheckCDAMedia();
    if (gPDS3_cda_media_present) {
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STOP, 0, 0);
    }
    gPDS3_cda_is_playing = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005697ef
int C2_NAKED C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type && pChannel->descriptor->pd_handle != NULL) {
        LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
        IDirectSoundBuffer_Stop(buffer);
        if (gS3_callbacks.on_sample_channel_stop != NULL && pChannel->active) {
            gS3_callbacks.on_sample_channel_stop(pChannel);
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dword ptr [ebp - 8], ecx
            mov eax, dword ptr [ebp - 8]
            cmp dword ptr [eax + 0x60], 0
            je L0056984a
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx + 0x60]
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [edx + 4]
            cmp ecx, dword ptr [eax + 0x20]
            jne L0056984a
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [eax + 0x48]
            mov dword ptr [ebp - 4], ecx
            cmp dword ptr [ebp - 4], 0
            je L0056984a
            mov edx, dword ptr [ebp - 4]
            push edx
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x48]
            cmp dword ptr [gS3_callbacks+0x4], 0
            je L0056984a
            mov edx, dword ptr [ebp - 8]
            cmp dword ptr [edx], 0
            je L0056984a
            mov ecx, dword ptr [ebp - 8]
            call dword ptr [gS3_callbacks+0x4]
    L0056984a:
            mov eax, dword ptr [ebp - 8]
            cmp dword ptr [eax], 0
            je L0056985c
            mov ecx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 8], 1
    L0056985c:
            mov eax, 1
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00569507
int C2_NAKED C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pChannel->type == 0) {
        if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
            LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;

            if (buffer != NULL) {
                int volume = (int)(pChannel->owner_outlet->volume / 255.f * pChannel->volume_multiplier * 11.76470588235294f);
                int ds_volume = volume - 3000;
                ds_volume = MAX(ds_volume, -9999);
                ds_volume = MIN(ds_volume, 0);

                if (SUCCEEDED(IDirectSoundBuffer_SetVolume(buffer, ds_volume))) {
                    if (gS3_callbacks.on_sample_channel_set_volume != NULL) {
                        gS3_callbacks.on_sample_channel_set_volume(pChannel, volume);
                    }
                    if (ds_volume != 0 && pChannel->spatial_sound) {
                        int pan = (int)(pChannel->field_0x28 * 9999.f);
                        pan = MAX(pan, -9999);
                        pan = MIN(pan,  9999);
                        if (gS3_callbacks.on_sample_channel_set_pan != NULL) {
                            gS3_callbacks.on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
                        }
                        IDirectSoundBuffer_SetPan(buffer, pan);
                    }
                }
            }
        }
    } else if (pChannel->type == 1) {

    } else if (pChannel->type == 2) {
        PDS3UpdateCDAVolume(pChannel, pChannel->owner_outlet->volume);
    }
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x18
            mov dword ptr [ebp - 0x18], ecx
            mov eax, dword ptr [ebp - 0x18]
            cmp dword ptr [eax + 0x20], 0
            jne L00569650
            mov ecx, dword ptr [ebp - 0x18]
            fild dword ptr [ecx + 0x24]
            fstp dword ptr [ebp - 0x10]
            mov edx, dword ptr [ebp - 0x18]
            mov eax, dword ptr [edx + 0x5c]
            fild dword ptr [eax + 8]
            fstp dword ptr [ebp - 0x14]
            mov ecx, dword ptr [ebp - 0x18]
            cmp dword ptr [ecx + 0x60], 0
            je L0056964e
            mov edx, dword ptr [ebp - 0x18]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [eax + 4]
            cmp edx, dword ptr [ecx + 0x20]
            jne L0056964e
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x48]
            mov dword ptr [ebp - 0xc], edx
            cmp dword ptr [ebp - 0xc], 0
            je L0056964e
            fld dword ptr [ebp - 0x10]
            fld dword ptr [ebp - 0x14]
            fdiv qword ptr [rendererStateStoredDispatch+0xc90]
            fmulp st(1), st(0)
            fmul qword ptr [rendererStateStoredDispatch+0xc98]
            call _ftol
            mov dword ptr [ebp - 8], eax
            mov eax, dword ptr [ebp - 8]
            sub eax, 0xbb8
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 8], 0xffffd8f1
            jge L005695a1
            mov dword ptr [ebp - 8], 0xffffd8f1
    L005695a1:
            cmp dword ptr [ebp - 8], 0
            jle L005695ae
            mov dword ptr [ebp - 8], 0
    L005695ae:
            mov ecx, dword ptr [ebp - 8]
            push ecx
            mov edx, dword ptr [ebp - 0xc]
            push edx
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x3c]
            test eax, eax
            jl L0056964e
            cmp dword ptr [gS3_callbacks+0x8], 0
            je L005695e2
            fld dword ptr [ebp - 0x10]
            call _ftol
            mov edx, eax
            mov ecx, dword ptr [ebp - 0x18]
            call dword ptr [gS3_callbacks+0x8]
    L005695e2:
            cmp dword ptr [ebp - 8], 0
            je L0056964e
            mov edx, dword ptr [ebp - 0x18]
            cmp dword ptr [edx + 0x14], 0
            je L0056964e
            mov eax, dword ptr [ebp - 0x18]
            fld dword ptr [eax + 0x28]
            fmul qword ptr [rendererStateStoredDispatch+0xca0]
            call _ftol
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0xffffd8f1
            jge L00569615
            mov dword ptr [ebp - 4], 0xffffd8f1
    L00569615:
            cmp dword ptr [ebp - 4], 0x270f
            jle L00569625
            mov dword ptr [ebp - 4], 0x270f
    L00569625:
            cmp dword ptr [gS3_callbacks+0xc], 0
            je L0056963e
            mov ecx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [ecx + 0x28]
            push edx
            mov ecx, dword ptr [ebp - 0x18]
            call dword ptr [gS3_callbacks+0xc]
    L0056963e:
            mov eax, dword ptr [ebp - 4]
            push eax
            mov ecx, dword ptr [ebp - 0xc]
            push ecx
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx]
            call dword ptr [eax + 0x40]
    L0056964e:
            jmp L00569675
    L00569650:
            mov ecx, dword ptr [ebp - 0x18]
            cmp dword ptr [ecx + 0x20], 1
            jne L0056965b
            jmp L00569675
    L0056965b:
            mov edx, dword ptr [ebp - 0x18]
            cmp dword ptr [edx + 0x20], 2
            jne L00569675
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 0x5c]
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [ebp - 0x18]
            call PDS3UpdateCDAVolume
    L00569675:
            mov eax, 1
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00568ed0
void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {
}

// FUNCTION: CARMA2_HW 0x00569ae2
void C2_HOOK_FASTCALL PDS3Stop(void) {

    if (gPD_S3_config.free_direct_sound) {

        IDirectSound_Release(gPD_S3_direct_sound);
        gPD_S3_direct_sound = NULL;
    }
    if (gPDS3_mci_open_parms.wDeviceID != 0) {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_CLOSE, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00569d67
int C2_HOOK_FASTCALL PDS3UpdateMidiChannelPitch(tS3_channel* pChannel, int pPitch) {
    MCIDEVICEID device;

    device = pChannel->mciDevice;
    gPDS3_mci_midi_status_parms.dwItem = MCI_SEQ_STATUS_TEMPO;
    mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    gPDS3_mci_seq_set_parms.dwTempo = gPDS3_mci_midi_status_parms.dwReturn * (int)ldexp((double)pPitch, -16);
    mciSendCommandA(device, MCI_SET, MCI_SEQ_SET_TEMPO, (DWORD_PTR)&gPDS3_mci_seq_set_parms);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569e6d
int C2_HOOK_FASTCALL PDS3PauseMidiChannel(tS3_channel* pChannel) {
    MCIDEVICEID device;

    device = pChannel->mciDevice;
    if (gPDS3_midi_restart_required == 0) {
        gPDS3_mci_midi_status_parms.dwItem = MCI_STATUS_MODE;
        mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
        if (gPDS3_mci_midi_status_parms.dwReturn == MCI_MODE_PLAY) {
            gPDS3_midi_restart_required = 1;
            mciSendCommandA(device, MCI_PAUSE, 0, 0);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569eda
int C2_HOOK_FASTCALL PDS3ResumeMidiChannel(tS3_channel* pChannel) {
    MCIDEVICEID device;

    device = pChannel->mciDevice;
    if (gPDS3_midi_restart_required != 0) {
        gPDS3_midi_restart_required = 0;
        mciSendCommandA(device, MCI_PLAY, 0, (DWORD_PTR)&mci_play_parms);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569e29
int C2_HOOK_FASTCALL PDS3UnmuteMidiChannel(tS3_channel* pChannel) {
    MCIDEVICEID device;

    device = pChannel->mciDevice;
    if (gPDS3_midi_playing != 0) {
        gPDS3_mci_audio_parms.dwAudio = 0;
        mciSendCommandA(device, MCI_SET, MCI_SET_AUDIO | MCI_SET_ON, (DWORD_PTR)&gPDS3_mci_audio_parms);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569f1b
int C2_NAKED C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel *pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    gPDS3_mci_midi_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(pChannel->mciDevice, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    if (gPDS3_mci_midi_status_parms.dwReturn == MCI_MODE_PLAY) {
        return 0;
    }
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dword ptr [ebp - 8], ecx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [ebp - 4], ecx
            mov dword ptr [gPDS3_mci_midi_status_parms+0x8], 4
            push offset gPDS3_mci_midi_status_parms
            push 0x102
            push 0x814
            mov edx, dword ptr [ebp - 4]
            push edx
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            cmp dword ptr [gPDS3_mci_midi_status_parms+0x4], 0x20e
            jne L00569f60
            xor eax, eax
            jmp L00569f65
    L00569f60:
            mov eax, 1
    L00569f65:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a1af
int C2_HOOK_FASTCALL PDS3PauseCDA(volatile undefined4 pUnused_arg) {

    if (gPDS3_cda_media_present != 0 && gPDS3_cda_paused == 0) {
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
        if (gPDS3_cda_status_parms.dwReturn == MCI_MODE_PLAY) {
            gPDS3_cda_paused = 1;
            mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PAUSE, 0, 0);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a21f
int C2_HOOK_FASTCALL PDS3ResumeCDA(tS3_channel* pChannel) {

    if (gPDS3_cda_media_present != 0 && gPDS3_cda_paused != 0) {
        gPDS3_cda_paused = 0;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, 0, (DWORD_PTR)&gPDS3_cda_play_parms);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a260
int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void) {
    if (gS3_CDA_enabled != 0) {
        if (gPDS3_cda_media_present != 0) {
            return gPDS3_cda_is_playing;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL PDS3IsSamplePlaying(tS3_channel* pChannel) {
    DWORD status;

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        LPDIRECTSOUNDBUFFER buffer;

        buffer = pChannel->descriptor->pd_handle;
        if (buffer != NULL) {
            if (!FAILED(IDirectSoundBuffer_GetStatus(buffer, &status))) {
                if (status & DSBSTATUS_PLAYING) {
                    return 1;
                }
            }
        }
    }
    PDS3StopSampleChannel(pChannel);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056967e
int C2_NAKED C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pChannel->type == 0 && pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer* sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            IDirectSoundBuffer_SetFrequency(sound_buffer, MIN(pChannel->rate, 100000));
        }
    }
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x10
            mov dword ptr [ebp - 0xc], ecx
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x20], 0
            jne L005696f0
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 0x60], 0
            je L005696f0
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [eax + 4]
            cmp edx, dword ptr [ecx + 0x20]
            jne L005696f0
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x48]
            mov dword ptr [ebp - 8], edx
            cmp dword ptr [ebp - 8], 0
            je L005696f0
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0x10], 0x186a0
            jge L005696d3
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [ebp - 0x10], edx
            jmp L005696da
    L005696d3:
            mov dword ptr [ebp - 0x10], 0x186a0
    L005696da:
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [ebp - 4], eax
            mov ecx, dword ptr [ebp - 4]
            push ecx
            mov edx, dword ptr [ebp - 8]
            push edx
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x44]
    L005696f0:
            mov eax, 1
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

int C2_HOOK_FASTCALL PDS3PlaySample(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer *sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            int looping = pChannel->repetitions == 0;
            IDirectSoundBuffer_Play(sound_buffer, 0, 0, looping ? DSBPLAY_LOOPING : 0);
            if (!looping) {
                IDirectSoundBuffer_SetCurrentPosition(sound_buffer, 0);
            }
        }
        PDS3UpdateChannelVolume(pChannel);
        PDS3UpdateChannelFrequency(pChannel);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569b93
tS3_error_codes C2_NAKED C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel* pChannel) {
#ifndef CARPOCALYPSE2_MATCHING
    char path[256];
    MCIERROR err;
    MCIDEVICEID device;

    path[0] = '\0';
    strcpy(path, pChannel->descriptor->path);
    strcat(path, ".MID");
    mci_open_params.lpstrElementName = path;
    mci_open_params.lpstrDeviceType = "sequencer";
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_open_params);
    device = mci_open_params.wDeviceID;
    if (err != 0) {
        return eS3_error_start_song;
    }
    gPDS3_mci_midi_status_parms.dwItem = MCI_VD_STATUS_FORWARD;
    err = mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    err = mciSendCommandA(device, MCI_PLAY, 0, (DWORD_PTR)&mci_play_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    pChannel->mciDevice = device;
    gPDS3_midi_playing = 1;
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x64
            mov dword ptr [ebp - 0x64], ecx
            mov eax, dword ptr [ebp - 0x64]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x44]
            mov dword ptr [ebp - 0x5c], edx
            mov byte ptr [ebp - 0x50], 0
            mov eax, dword ptr [ebp - 0x5c]
            push eax
            lea ecx, [ebp - 0x50]
            push ecx
            call strcpy
            add esp, 8
            push offset gS3_CDA_enabled+0x180
            lea edx, [ebp - 0x50]
            push edx
            call strcat
            add esp, 8
            lea eax, [ebp - 0x50]
            mov dword ptr [ebp - 0x58], eax
            mov dword ptr [mci_open_params+0x8], 0x67368c
            mov ecx, dword ptr [ebp - 0x58]
            mov dword ptr [mci_open_params+0xc], ecx
            push offset mci_open_params
            push 0x2200
            push 0x803
            push 0
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            mov dword ptr [ebp - 0x60], eax
            cmp dword ptr [ebp - 0x60], 0
            je L00569c10
            mov eax, 0xf
            jmp L00569cad
    L00569c10:
            mov edx, dword ptr [mci_open_params+0x4]
            mov dword ptr [ebp - 0x54], edx
            mov dword ptr [gPDS3_mci_midi_status_parms+0x8], 0x4003
            push offset gPDS3_mci_midi_status_parms
            push 0x100
            push 0x814
            mov eax, dword ptr [ebp - 0x54]
            push eax
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            mov dword ptr [ebp - 0x60], eax
            cmp dword ptr [ebp - 0x60], 0
            je L00569c5f
            push 0
            push 0
            push 0x804
            mov ecx, dword ptr [ebp - 0x54]
            push ecx
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            mov eax, 0xf
            jmp L00569cad
    L00569c5f:
            push offset mci_play_parms
            push 0
            push 0x806
            mov edx, dword ptr [ebp - 0x54]
            push edx
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            mov dword ptr [ebp - 0x60], eax
            cmp dword ptr [ebp - 0x60], 0
            je L00569c98
            push 0
            push 0
            push 0x804
            mov eax, dword ptr [ebp - 0x54]
            push eax
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            mov eax, 0xf
            jmp L00569cad
    L00569c98:
            mov ecx, dword ptr [ebp - 0x64]
            mov edx, dword ptr [ebp - 0x54]
            mov dword ptr [ecx + 0x18], edx
            mov dword ptr [gPDS3_midi_playing], 1
            xor eax, eax
    L00569cad:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00569f69
int C2_HOOK_FASTCALL PDS3StopMidiChannelWrapper(tS3_channel* pChannel, int pArg2, int pArg3, int pArg4, int pArg5, int pArg6) {
    PDS3StopMidiChannel(pChannel);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569f85
int C2_HOOK_FASTCALL PDS3Return0_C(tS3_channel* pChannel) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569f92
tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel* pChannel) {
    char* endptr;

    PDS3CheckCDAMedia();
    if (gPDS3_cda_media_present) {
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = strtoul(pChannel->descriptor->path, &endptr, 10);
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_cda_status_parms);
        if (gPDS3_cda_status_parms.dwReturn != MCI_MODE_NOT_READY
                && gPDS3_cda_status_parms.dwReturn != MCI_MODE_OPEN) {
            gPDS3_cda_play_parms.dwFrom = (unsigned char)gPDS3_cda_track;
            if (gPDS3_cda_track + 1 < gPDS3_Number_of_tracks) {
                gPDS3_cda_play_parms.dwTo = (unsigned char)(gPDS3_cda_track + 1);
                mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD_PTR)&gPDS3_cda_play_parms);
            } else {
                mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM, (DWORD_PTR)&gPDS3_cda_play_parms);
            }
            gPDS3_cda_is_playing = 1;
            return eS3_error_none;
        }
    }
    return eS3_error_start_cda;
}

// FUNCTION: CARMA2_HW 0x0056a28d
void C2_NAKED C2_HOOK_FASTCALL PDS3ServiceCDA(tS32 pDelta_time) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 now;
    tS3_outlet* o;
    tS3_channel* c;

    o = gS3_state.outlets;
    for (; o != NULL; o = o->next) {
        c = o->channel_list;
        for (; c != NULL; c = c->next) {
            S3ServiceChannel(c);
            ;
        }

    }

    if (gPDS3_cda_initialized) {
        now = PDGetTotalTime();
        if (now > gPDS3_last_cda_status_update + 3000) {
            gPDS3_last_cda_status_update = now;
            gPDS3_cda_status_parms.dwCallback = (DWORD_PTR)gHWnd;
            gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
            mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_NOTIFY, (DWORD_PTR)&gPDS3_cda_status_parms);
            gPDS3_cda_is_playing = gPDS3_cda_status_parms.dwReturn == MCI_MODE_PLAY;
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x10
            mov dword ptr [ebp - 0x10], ecx
            mov eax, dword ptr [gS3_state+0xc]
            mov dword ptr [ebp - 8], eax
            jmp L0056a2a9
    L0056a2a0:
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [ebp - 8], edx
    L0056a2a9:
            cmp dword ptr [ebp - 8], 0
            je L0056a2d5
            mov eax, dword ptr [ebp - 8]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ebp - 0xc], ecx
            jmp L0056a2c3
    L0056a2ba:
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x64]
            mov dword ptr [ebp - 0xc], eax
    L0056a2c3:
            cmp dword ptr [ebp - 0xc], 0
            je L0056a2d3
            mov ecx, dword ptr [ebp - 0xc]
            call S3ServiceChannel
            jmp L0056a2ba
    L0056a2d3:
            jmp L0056a2a0
    L0056a2d5:
            cmp dword ptr [gPDS3_cda_initialized], 0
            je L0056a345
            call PDGetTotalTime
            mov dword ptr [ebp - 4], eax
            mov ecx, dword ptr [gPDS3_last_cda_status_update]
            add ecx, 0xbb8
            cmp dword ptr [ebp - 4], ecx
            jbe L0056a345
            mov edx, dword ptr [ebp - 4]
            mov dword ptr [gPDS3_last_cda_status_update], edx
            mov eax, dword ptr [gHWnd]
            mov dword ptr [gPDS3_cda_status_parms], eax
            mov dword ptr [gPDS3_cda_status_parms+0x8], 4
            push offset gPDS3_cda_status_parms
            push 0x101
            push 0x814
            mov ecx, dword ptr [gPDS3_mci_open_parms+0x4]
            push ecx
            call dword ptr [gS3_sample_filter_disable_func+0x1994]
            xor edx, edx
            cmp dword ptr [gPDS3_cda_status_parms+0x4], 0x20e
            sete dl
            mov dword ptr [gPDS3_cda_is_playing], edx
    L0056a345:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}
