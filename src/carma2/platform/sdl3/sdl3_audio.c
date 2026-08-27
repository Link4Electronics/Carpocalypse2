#include "sdl3_ssdx.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stb/stb_vorbis.c"

static const int gS3_audio_max_volume = 255;

int PDS3StopSampleChannel(tS3_channel* pChannel);
int PDS3UpdateChannelVolume(tS3_channel* pChannel);
int PDS3UpdateChannelFrequency(tS3_channel* pChannel);
int PDS3IsCDAPlaying(void);

typedef struct {
    Uint8* data;
    Uint32 size;
    SDL_AudioSpec spec;
} tS3_audio_sample;

typedef struct {
    SDL_AudioStream* stream;
    SDL_AudioSpec spec;
    Uint8* owned_data;
    Uint32 owned_len;
    int looping;
    Uint32 end_time;
    int pan;
} tS3_audio_voice;

typedef struct {
    SDL_AudioStream* stream;
    Uint8* data;
    Uint32 data_len;
    int playing;
} tS3_cda_state;

static tS3_cda_state gS3_cda;

static tS3_audio_voice* PDS3_GetVoice(tS3_channel* pChannel) {
    tS3_audio_voice* voice;

    if (pChannel == NULL) {
        return NULL;
    }
    voice = (tS3_audio_voice*)pChannel->type_struct_sample;
    if (voice == NULL) {
        voice = (tS3_audio_voice*)malloc(sizeof(tS3_audio_voice));
        if (voice == NULL) {
            return NULL;
        }
        memset(voice, 0, sizeof(*voice));
        pChannel->type_struct_sample = (char*)voice;
    }
    return voice;
}

static void PDS3_CloseVoice(tS3_audio_voice* voice) {
    if (voice == NULL) {
        return;
    }
    if (voice->stream != NULL) {
        SDL_DestroyAudioStream(voice->stream);
        voice->stream = NULL;
    }
    if (voice->owned_data != NULL) {
        free(voice->owned_data);
        voice->owned_data = NULL;
        voice->owned_len = 0;
    }
    voice->looping = 0;
}

static void PDS3_CloseCDA(void) {
    if (gS3_cda.stream != NULL) {
        SDL_DestroyAudioStream(gS3_cda.stream);
        gS3_cda.stream = NULL;
    }
    if (gS3_cda.data != NULL) {
        free(gS3_cda.data);
        gS3_cda.data = NULL;
    }
    gS3_cda.data_len = 0;
    gS3_cda.playing = 0;
}

static SDL_AudioFormat S3FormatFromBits(int pBits_per_sample) {
    if (pBits_per_sample >= 16) {
        return SDL_AUDIO_S16;
    }
    return SDL_AUDIO_U8;
}

int PDS3Init(void) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        fprintf(stderr, "SDL3 audio init error: %s\n", SDL_GetError());
        return 0;
    }
    strcpy(gS3_path_separator, "/");
    strcpy(gS3_sound_folder_name, "SOUND");
    S3Enable();
    return 1;
}

void PDS3Stop(void) {
    PDS3_CloseCDA();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void PDS3InitCDA(void) {
}

const char* PDS3GetWorkingDirectory(void) {
    return ".";
}

void* PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer) {
    tS3_audio_sample* sample;

    (void)pWav_buffer;

    if (pWav_info == NULL || pWav_info->samples == NULL || pWav_info->wav_info_header == NULL
            || pWav_info->sample_size <= 0) {
        return NULL;
    }
    sample = (tS3_audio_sample*)malloc(sizeof(tS3_audio_sample));
    if (sample == NULL) {
        return NULL;
    }
    memset(sample, 0, sizeof(*sample));
    sample->size = (Uint32)pWav_info->sample_size;
    sample->data = (Uint8*)malloc(sample->size);
    if (sample->data == NULL) {
        free(sample);
        return NULL;
    }
    memcpy(sample->data, pWav_info->samples, sample->size);
    SDL_zero(sample->spec);
    sample->spec.freq = (int)pWav_info->wav_info_header->sample_rate;
    sample->spec.channels = (Uint8)pWav_info->wav_info_header->number_of_channels;
    sample->spec.format = S3FormatFromBits((int)pWav_info->wav_info_header->bits_per_sample);
    return sample;
}

void PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {
    tS3_audio_sample* sample;

    if (pSound_descriptor == NULL || pSound_descriptor->pd_handle == NULL) {
        return;
    }
    sample = (tS3_audio_sample*)pSound_descriptor->pd_handle;
    if (sample->data != NULL) {
        free(sample->data);
    }
    free(sample);
    pSound_descriptor->pd_handle = NULL;
}

int PDReverseAudio(tS3_channel* pChannel) {
    tS3_audio_sample* sample;
    tS3_audio_voice* voice;
    Uint8* tmp_buffer;
    Uint32 i;

    if (pChannel == NULL || pChannel->descriptor == NULL) {
        return 1;
    }
    sample = (tS3_audio_sample*)pChannel->descriptor->pd_handle;
    if (sample == NULL || sample->data == NULL || sample->size == 0) {
        return 1;
    }
    tmp_buffer = (Uint8*)malloc(sample->size);
    if (tmp_buffer == NULL) {
        return 1;
    }
    memcpy(tmp_buffer, sample->data, sample->size);
    for (i = 0; i < sample->size; i++) {
        sample->data[i] = tmp_buffer[sample->size - 1 - i];
    }
    free(tmp_buffer);
    voice = (tS3_audio_voice*)pChannel->type_struct_sample;
    if (voice != NULL && voice->stream != NULL) {
        SDL_ClearAudioStream(voice->stream);
        SDL_PutAudioStreamData(voice->stream, sample->data, (int)sample->size);
    }
    return 0;
}

int PDS3PlaySample(tS3_channel* pChannel) {
    tS3_audio_sample* sample;
    tS3_audio_voice* voice;
    Uint8 bytes_per_sample;

    if (pChannel == NULL || pChannel->descriptor == NULL) {
        return 1;
    }
    if (pChannel->descriptor->type != 0) {
        return 1;
    }
    sample = (tS3_audio_sample*)pChannel->descriptor->pd_handle;
    if (sample == NULL || sample->data == NULL || sample->size == 0) {
        return 1;
    }
    voice = PDS3_GetVoice(pChannel);
    if (voice == NULL) {
        return 1;
    }

    PDS3_CloseVoice(voice);
    voice->spec = sample->spec;
    voice->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &voice->spec, NULL, NULL);
    if (voice->stream == NULL) {
        return 1;
    }
    if (!SDL_PutAudioStreamData(voice->stream, sample->data, (int)sample->size)) {
        PDS3_CloseVoice(voice);
        return 1;
    }
    SDL_ResumeAudioStreamDevice(voice->stream);

    voice->looping = (pChannel->repetitions == 0);
    if (voice->looping) {
        voice->owned_data = (Uint8*)malloc(sample->size);
        if (voice->owned_data != NULL) {
            memcpy(voice->owned_data, sample->data, sample->size);
            voice->owned_len = sample->size;
        }
    } else {
        bytes_per_sample = (Uint8)SDL_AUDIO_BYTESIZE(voice->spec.format);
        if (voice->spec.channels > 0 && voice->spec.freq > 0 && bytes_per_sample > 0) {
            Uint64 duration_ms = (Uint64)sample->size * 1000
                    / ((Uint64)voice->spec.channels * (Uint64)voice->spec.freq * (Uint64)bytes_per_sample);
            voice->end_time = SDL_GetTicks() + (Uint32)duration_ms + 500;
        } else {
            voice->end_time = SDL_GetTicks() + 500;
        }
    }

    PDS3UpdateChannelVolume(pChannel);
    PDS3UpdateChannelFrequency(pChannel);
    return 1;
}

int PDS3IsSamplePlaying(tS3_channel* pChannel) {
    tS3_audio_voice* voice;

    if (pChannel == NULL) {
        return 0;
    }
    voice = (tS3_audio_voice*)pChannel->type_struct_sample;
    if (voice == NULL || voice->stream == NULL) {
        PDS3StopSampleChannel(pChannel);
        return 0;
    }
    if (voice->looping) {
        int queued = SDL_GetAudioStreamQueued(voice->stream);
        if (voice->owned_data != NULL && voice->owned_len > 0 && queued < (int)voice->owned_len / 2) {
            SDL_PutAudioStreamData(voice->stream, voice->owned_data, (int)voice->owned_len);
        }
        return 1;
    }
    if (SDL_GetTicks() < voice->end_time) {
        return 1;
    }
    if (SDL_GetAudioStreamQueued(voice->stream) > 0 || SDL_GetAudioStreamAvailable(voice->stream) > 0) {
        return 1;
    }
    PDS3StopSampleChannel(pChannel);
    return 0;
}

int PDS3StopSampleChannel(tS3_channel* pChannel) {
    tS3_audio_voice* voice;

    if (pChannel == NULL) {
        return 1;
    }
    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type
            && pChannel->descriptor->pd_handle != NULL) {
        voice = (tS3_audio_voice*)pChannel->type_struct_sample;
        if (voice != NULL) {
            PDS3_CloseVoice(voice);
        }
        if (gS3_callbacks.on_sample_channel_stop != NULL && pChannel->active) {
            gS3_callbacks.on_sample_channel_stop(pChannel);
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
}

int PDS3UpdateChannelVolume(tS3_channel* pChannel) {
    tS3_audio_voice* voice;
    int volume;
    int pan;
    float gain;

    if (pChannel == NULL) {
        return 1;
    }
    if (pChannel->type != 0) {
        return 1;
    }
    if (pChannel->descriptor == NULL || pChannel->descriptor->type != pChannel->type
            || pChannel->owner_outlet == NULL) {
        return 1;
    }
    voice = (tS3_audio_voice*)pChannel->type_struct_sample;
    if (voice == NULL || voice->stream == NULL) {
        return 1;
    }
    volume = (int)(pChannel->owner_outlet->volume / 255.0f * pChannel->volume_multiplier);
    if (volume < 0) {
        volume = 0;
    }
    if (volume > gS3_audio_max_volume) {
        volume = gS3_audio_max_volume;
    }
    gain = volume / 255.0f;
    if (gain < 0.05f && volume != 0) {
        gain = 0.05f;
    }
    SDL_SetAudioStreamGain(voice->stream, gain);
    if (volume != 0 && pChannel->spatial_sound) {
        pan = (int)(pChannel->field_0x28 * 9999.0f);
        if (pan < -9999) {
            pan = -9999;
        }
        if (pan > 9999) {
            pan = 9999;
        }
        voice->pan = pan;
        if (gS3_callbacks.on_sample_channel_set_pan != NULL) {
            gS3_callbacks.on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
        }
    }
    return 1;
}

int PDS3UpdateChannelFrequency(tS3_channel* pChannel) {
    tS3_audio_voice* voice;
    tS3_audio_sample* sample;

    if (pChannel == NULL) {
        return 1;
    }
    if (pChannel->type != 0 || pChannel->descriptor == NULL || pChannel->descriptor->type != pChannel->type) {
        return 1;
    }
    voice = (tS3_audio_voice*)pChannel->type_struct_sample;
    if (voice == NULL || voice->stream == NULL) {
        return 1;
    }
    sample = (tS3_audio_sample*)pChannel->descriptor->pd_handle;
    if (sample == NULL || sample->spec.freq <= 0) {
        return 1;
    }
    SDL_SetAudioStreamFrequencyRatio(voice->stream, (float)pChannel->rate / (float)sample->spec.freq);
    return 1;
}

void PDS3StopMidiChannel(tS3_channel* pChannel) {
    (void)pChannel;
}

tS3_error_codes PDS3StartMidiChannel(tS3_channel* pChannel) {
    (void)pChannel;
    return eS3_error_memory;
}

int PDS3IsMIDIStopped(tS3_channel* pChannel) {
    (void)pChannel;
    return 1;
}

int PDS3StopCDAChannel(tS3_channel* pChannel) {
    (void)pChannel;
    PDS3_CloseCDA();
    return 0;
}

tS3_error_codes PDS3PlayCDAChannel(tS3_channel* pChannel) {
    char path[256];
    char* endptr;
    int track;
    int channels;
    int sample_rate;
    int sample_count;
    short* decoded;
    SDL_AudioSpec spec;

    if (pChannel == NULL || pChannel->descriptor == NULL || pChannel->descriptor->path == NULL) {
        return eS3_error_start_cda;
    }
    track = (int)strtoul(pChannel->descriptor->path, &endptr, 10);
    if (endptr == pChannel->descriptor->path) {
        return eS3_error_start_cda;
    }
    snprintf(path, sizeof(path), "MUSIC/Track%02d.ogg", track);
    if (access(path, F_OK) != 0) {
        return eS3_error_start_cda;
    }

    decoded = NULL;
    sample_count = stb_vorbis_decode_filename(path, &channels, &sample_rate, &decoded);
    if (sample_count <= 0 || decoded == NULL || channels <= 0 || sample_rate <= 0) {
        if (decoded != NULL) {
            free(decoded);
        }
        return eS3_error_start_cda;
    }

    PDS3_CloseCDA();

    SDL_zero(spec);
    spec.freq = sample_rate;
    spec.channels = (Uint8)channels;
    spec.format = SDL_AUDIO_S16;
    gS3_cda.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (gS3_cda.stream == NULL) {
        free(decoded);
        return eS3_error_start_cda;
    }
    gS3_cda.data = (Uint8*)decoded;
    gS3_cda.data_len = (Uint32)((Uint64)sample_count * (Uint64)channels * 2u);
    if (!SDL_PutAudioStreamData(gS3_cda.stream, gS3_cda.data, (int)gS3_cda.data_len)) {
        PDS3_CloseCDA();
        return eS3_error_start_cda;
    }
    SDL_ResumeAudioStreamDevice(gS3_cda.stream);
    gS3_cda.playing = 1;
    return eS3_error_none;
}

void PDS3ServiceCDA(int pDelta_time) {
    (void)pDelta_time;
    PDS3IsCDAPlaying();
}

void PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {
    float gain;

    (void)pChannel;
    if (gS3_cda.stream == NULL) {
        return;
    }
    gain = pVolume / (float)gS3_audio_max_volume;
    SDL_SetAudioStreamGain(gS3_cda.stream, gain);
}

int PDS3IsCDAPlaying(void) {
    int queued;

    if (gS3_cda.stream == NULL || !gS3_cda.playing) {
        return 0;
    }
    if (gS3_cda.data != NULL && gS3_cda.data_len > 0) {
        queued = SDL_GetAudioStreamQueued(gS3_cda.stream);
        if (queued < (int)gS3_cda.data_len / 2) {
            SDL_PutAudioStreamData(gS3_cda.stream, gS3_cda.data, (int)gS3_cda.data_len);
        }
    }
    return 1;
}
