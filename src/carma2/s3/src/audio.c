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

// FUNCTION: CARMA2_HW 0x005651d0
int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_dirname) {
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
tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2) {
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
}

// FUNCTION: CARMA2_HW 0x005654e2
tS3_error_codes C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet) {

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
int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet) {
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
}

// FUNCTION: CARMA2_HW 0x0056671e
int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src) {
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
}

// FUNCTION: CARMA2_HW 0x005667af
int C2_HOOK_FASTCALL FUN_005667af(int pSound_id, tS3_sound_source* pSource) {
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
}

// C2PORT-NONMATCHING: best 77.12% | difftest PASS 2000 trials (spec difftest/specs/00566a54_FUN_00566a54.json) | reason stack-layout
// FUNCTION: CARMA2_HW 0x00566a54
int C2_HOOK_FASTCALL FUN_00566a54(int pSound_id, tS3_sound_source* pSource, int pVolume, int pPitch, int pSpeed) {
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
}

// FUNCTION: CARMA2_HW 0x005657bd
int C2_HOOK_FASTCALL S3StopAllOutletSounds(void) {
    tS3_outlet* o;

    if (!gS3_enabled) {
        return 0;
    }

    for (o = gS3_state.outlets; o != NULL; o = o->next) {
        S3StopOutletSound(o);
    }
    return 0;
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
void C2_HOOK_FASTCALL S3Service(int pInside_cockpit, int pThings_moved) {
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
}

// FUNCTION: CARMA2_HW 0x0056532d
int C2_HOOK_FASTCALL S3DisableSound(void) {
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
void* C2_HOOK_FASTCALL S3LoadSoundBankFile(const char* pPath) {
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
}

// FUNCTION: CARMA2_HW 0x00567fe0
int C2_HOOK_FASTCALL S3LoadSoundbank(const char* pPath, int pLow_memory_mode) {
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
tS3_descriptor* C2_HOOK_FASTCALL S3CreateDescriptor(void) {
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
}

// FUNCTION: CARMA2_HW 0x00568704
int C2_HOOK_FASTCALL S3SoundBankReaderReadFilename(char** pPath, tS3_soundbank_read_ctx* pContext, const char* pDir_name) {
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
int C2_HOOK_FASTCALL S3StopCDAChannel(tS3_channel* pChannel) {

    if (!gS3_CDA_enabled) {
        return 0;
    }
    return PDS3StopCDAChannel(pChannel);
}

// FUNCTION: CARMA2_HW 0x0056991f
int C2_HOOK_FASTCALL S3NotifyUnbindChannel(tS3_channel* pChannel) {

    return 1;
}

// FUNCTION: CARMA2_HW 0x00565607
int C2_HOOK_FASTCALL S3UnbindChannels(tS3_outlet* pOutlet) {
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
}

// FUNCTION: CARMA2_HW 0x00565888
tS3_channel* C2_HOOK_FASTCALL S3GetChannelForTag(int pTag) {
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
            /* nop0_FUN_00569f85(); */
            channel->descriptor->buffer_description = NULL;
        }
    }
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00568929
int C2_HOOK_FASTCALL S3ReleaseSound(int pSound_id) {
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
int C2_HOOK_FASTCALL S3FreeUnboundChannels(void) {
    tS3_channel *channel;

    channel = gS3_unbound_channels;
    while (channel != NULL) {
        tS3_channel* next;

        next = channel->next;
        S3MemFree(channel);
        channel = next;
    }
    return 0;
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
int C2_HOOK_FASTCALL S3ServiceChannel(tS3_channel* pChannel) {

    if (pChannel->type == 0) {
        return PDS3IsSamplePlaying(pChannel);
    } else if (pChannel->type == 1) {
        return !PDS3IsMIDIStopped(pChannel);
    } else if (pChannel->type == 2) {
        return PDS3IsCDAPlaying();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a280
int C2_HOOK_FASTCALL S3ServiceSpatialSound(tS3_channel* pChannel) {

    (void) pChannel;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005665d4
tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceInternal(void* pPosition, void* pVelocity, tS3_outlet* pBound_outlet) {
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
void C2_HOOK_FASTCALL S3CalculateRandomizedFields(tS3_channel* chan, tS3_descriptor* desc) {
    int vol;

    vol = S3IRandomBetween(desc->min_volume, desc->max_volume, 128);
    chan->source_volume = vol;
    chan->volume_multiplier = vol;
    if (desc->type == 0 && desc->buffer_description != NULL) {
        chan->rate = S3IRandomBetweenLog(desc->min_pitch, desc->max_pitch, desc->buffer_description->sample_rate);
    }
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
tS3_channel* C2_HOOK_FASTCALL S3AllocateChannel(tS3_outlet* pOutlet, int pPriority) {
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
void C2_HOOK_FASTCALL S3ReverseSample(tS3_effect_tag pEffect, tS3_sound_tag pTag) {
    tS3_channel* channel;

    channel = S3GetChannelForTag(pTag);
    if (channel != NULL) {
        PDReverseAudio(channel);
    }
}

// FUNCTION: CARMA2_HW 0x00564d75
int C2_HOOK_FASTCALL S3SetSampleFilterCallbacks(tS3_sample_filter pEnable, tS3_sample_filter pDisable) {
    if (pEnable != NULL) {
        gS3_sample_filter_func = pEnable;
    }
    if (pDisable != NULL) {
        gS3_sample_filter_disable_func = pDisable;
    }
    if (pDisable == NULL) {
        gS3_sample_filter_disable_func = gS3_sample_filter_func;
    }
    if (pEnable == NULL && pDisable == NULL) {
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
            if (S3PlaySample(channel, channel->descriptor) == 0) {
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
int C2_HOOK_FASTCALL S3StartSound2(tS3_outlet* pOutlet, tS3_sound_id pSound, unsigned int pRepeats, int pLeft_volume, int pRight_volume,  tS32 pLeft_pitch, tS32 pRight_pitch) {
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
            if (!S3PlaySample(chan, chan->descriptor)) {
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
}

// FUNCTION: CARMA2_HW 0x00566dc1
tS3_sound_tag C2_HOOK_FASTCALL S3StartSoundFromSource3(tS3_sound_id pSound, tS3_sound_source* pSource, tS3_repeats pRepeats, tS3_volume pVolume, tS3_pitch pPitch, tS3_speed pSpeed) {
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
}

// FUNCTION: CARMA2_HW 0x0056a687
tS3_error_codes C2_HOOK_FASTCALL S3MIDITagHasStoppedPlaying(int pTag) {
    tS3_channel* channel;

    if (!gS3_enable_midi) {
        return eS3_error_digi_init;
    }
    channel = S3GetChannelForTag(pTag);
    if (channel == NULL) {
        return eS3_error_bad_stag;
    }
    if (channel->type != 1) {
        return eS3_error_digi_init;
    }
    return PDS3IsMIDIStopped(channel);
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
// GLOBAL: CARMA2_HW 0x00662528
const char gBackslash_00662528[] = "\\";

// GLOBAL: CARMA2_HW 0x006736a8
FARPROC gQuickTime_dispatcher;

// GLOBAL: CARMA2_HW 0x006736b0
HMODULE gQuickTime_module;

// GLOBAL: CARMA2_HW 0x006736b4
FARPROC gQuickTime_dispatch;

// GLOBAL: CARMA2_HW 0x006736b8
FARPROC gQuickTime_call_component;

// GLOBAL: CARMA2_HW 0x006736bc
FARPROC gQuickTime_call_component_with_storage;

// GLOBAL: CARMA2_HW 0x0067370c
const char gQuickTime_qts_0067370c[] = "QuickTime.qts";

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
        (gQuickTime_dispatch = GetProcAddress(gQuickTime_module, "theQuickTimeDispatcher")) != NULL) {
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

    module = GetModuleHandleA(gQuickTime_qts_0067370c);
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
            strcat(path, gQuickTime_qts_0067370c);
            module = LoadLibraryA(path);
            if (module != NULL) {
                return module;
            }
        }
    }

    module = NULL;
    if (GetSystemDirectoryA(path, sizeof(path)) != 0) {
        strcat(path, gBackslash_00662528);
        strcat(path, gQuickTime_qts_0067370c);
        module = LoadLibraryA(path);
    }
    return module;
}
#pragma auto_inline(on)
#pragma optimize("", on)

#pragma auto_inline(off)
// C2PORT-NONMATCHING: best 20.00% | difftest PASS 2000 trials (spec difftest/specs/0056fde0_FUN_0056fde0.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056fde0
void* C2_HOOK_CDECL FUN_0056fde0(int pArg1, int pArg2) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((void* (C2_HOOK_CDECL *)(unsigned int, int, int))gQuickTime_dispatch)(0x100001, pArg1, pArg2);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056fe00_FUN_0056fe00.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x0056fe00
void C2_HOOK_CDECL FUN_0056fe00(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x100002);
}

// C2PORT-NONMATCHING: best 16.67% | difftest PASS 2000 trials (spec difftest/specs/0056c640_FUN_0056c640.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c640
Track C2_HOOK_CDECL FUN_0056c640(Movie pMovie, Fixed pWidth, Fixed pHeight, short pVolume) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Track (C2_HOOK_CDECL *)(unsigned int, Movie, Fixed, Fixed, short))gQuickTime_dispatch)(
        0x20188, pMovie, pWidth, pHeight, pVolume);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056c660_FUN_0056c660.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c660
unsigned int C2_HOOK_CDECL FUN_0056c660(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x2018a);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056c700_FUN_0056c700.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c700
unsigned int C2_HOOK_CDECL FUN_0056c700(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20190);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056c740_FUN_0056c740.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c740
unsigned int C2_HOOK_CDECL FUN_0056c740(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20192);
}
// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056c820_FUN_0056c820.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c820
unsigned int C2_HOOK_CDECL FUN_0056c820(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x2019e);
}
// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056a900_FUN_0056a900.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056a900
unsigned int C2_HOOK_CDECL FUN_0056a900(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20002);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056ac00_FUN_0056ac00.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056ac00
unsigned int C2_HOOK_CDECL FUN_0056ac00(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20026);
}

// C2PORT-NONMATCHING: best 26.67% | difftest PASS 2000 trials (spec difftest/specs/0056a920_FUN_0056a920.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056a920
int C2_HOOK_CDECL FUN_0056a920(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return GetMoviesError();
}

// C2PORT-NONMATCHING: best 15.38% | difftest PASS 2000 trials (spec difftest/specs/0056c6e0_FUN_0056c6e0.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c6e0
Media C2_HOOK_CDECL FUN_0056c6e0(Track pTrack, int pMedia_type, int pTime_scale, int pArg4, int pArg5) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Media (C2_HOOK_CDECL *)(unsigned int, Track, int, int, int, int))gQuickTime_dispatch)(
        0x2018e, pTrack, pMedia_type, pTime_scale, pArg4, pArg5);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056b340_FUN_0056b340.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b340
int C2_HOOK_CDECL FUN_0056b340(Media pMedia) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return BeginMediaEdits(pMedia);
}

// C2PORT-NONMATCHING: best 22.22% | difftest PASS 2000 trials (spec difftest/specs/00570600_FUN_00570600.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x00570600
Handle C2_HOOK_CDECL FUN_00570600(int pSize) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Handle (C2_HOOK_CDECL *)(unsigned int, int))gQuickTime_dispatch)(0x150008, pSize);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/005705c0_FUN_005705c0.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x005705c0
int C2_HOOK_CDECL FUN_005705c0(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150006);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/005705e0_FUN_005705e0.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x005705e0
void C2_HOOK_CDECL FUN_005705e0(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150007);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/00570620_FUN_00570620.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x00570620
void C2_HOOK_CDECL FUN_00570620(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150009);
}

// C2PORT-NONMATCHING: best 13.33% | difftest PASS 2000 trials (spec difftest/specs/0056d7e0_FUN_0056d7e0.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056d7e0
int C2_HOOK_CDECL FUN_0056d7e0(Handle pPixmap, struct Rect* pRect, int pArg3, int pQuality, int pCodec_type, int pArg6, int* pMax_size) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((int (C2_HOOK_CDECL *)(unsigned int, Handle, struct Rect*, int, int, int, int, int*))gQuickTime_dispatch)(
        0x30004, pPixmap, pRect, pArg3, pQuality, pCodec_type, pArg6, pMax_size);
}

// C2PORT-NONMATCHING: best 22.22% | difftest PASS 2000 trials (spec difftest/specs/00570a40_FUN_00570a40.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x00570a40
void C2_HOOK_CDECL FUN_00570a40(Handle pHandle) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int, Handle))gQuickTime_dispatch)(0x15002e, pHandle);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/00570a60_FUN_00570a60.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x00570a60
void C2_HOOK_CDECL FUN_00570a60(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x15002f);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/00570ae0_FUN_00570ae0.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x00570ae0
void C2_HOOK_CDECL FUN_00570ae0(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x150033);
}

// C2PORT-NONMATCHING: best 22.22% | difftest PASS 2000 trials (spec difftest/specs/005707e0_FUN_005707e0.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x005707e0
void C2_HOOK_CDECL FUN_005707e0(Handle pHandle) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int, Handle))gQuickTime_dispatch)(0x150019, pHandle);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/00570800_FUN_00570800.json) | reason implicit-EAX QuickTime selector
// FUNCTION: CARMA2_HW 0x00570800
void C2_HOOK_CDECL FUN_00570800(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x15001a);
}

// C2PORT-NONMATCHING: best 9.09% | difftest PASS 2000 trials (spec difftest/specs/0056d8a0_FUN_0056d8a0.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056d8a0
int C2_HOOK_CDECL FUN_0056d8a0(int* pImage_sequence, Handle pPixmap, int pArg3, struct Rect* pSource_rect, struct Rect* pDest_rect, int pArg6, int pCodec_type, int pArg8, int pSpatial_quality, int pTemporal_quality, int pArg11, int pArg12, int pArg13, Handle pImage_description) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((int (C2_HOOK_CDECL *)(unsigned int, int*, Handle, int, struct Rect*, struct Rect*, int, int, int, int, int, int, int, int, Handle))gQuickTime_dispatch)(
        0x3000a, pImage_sequence, pPixmap, pArg3, pSource_rect, pDest_rect, pArg6, pCodec_type, pArg8,
        pSpatial_quality, pTemporal_quality, pArg11, pArg12, pArg13, pImage_description);
}

// C2PORT-NONMATCHING: best 12.50% | difftest PASS 2000 trials (spec difftest/specs/0056d8c0_FUN_0056d8c0.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056d8c0
short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return (short)((short (C2_HOOK_CDECL *)(unsigned int, int, void*, void*, int, void*, int*, void*, void*))gQuickTime_dispatch)(
        0x3000b, pSequence, pFrame_pixmap, pFrame_rect, pFlags, pCompressed_frame, pFrame_size, pCodec_info, pUnused8);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056d8e0_FUN_0056d8e0.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056d8e0
unsigned int C2_HOOK_CDECL FUN_0056d8e0(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x3000c);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056dae0_FUN_0056dae0.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056dae0
unsigned int C2_HOOK_CDECL FUN_0056dae0(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x3001c);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056e760_FUN_0056e760.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056e760
unsigned int C2_HOOK_CDECL FUN_0056e760(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x1d0008);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056e780_FUN_0056e780.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056e780
unsigned int C2_HOOK_CDECL FUN_0056e780(void) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x1d0009);
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056b440_FUN_0056b440.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b440
short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return (short)((unsigned int (C2_HOOK_CDECL *)(unsigned int, void*, void*, int, int, int, void*, int, int, int))gQuickTime_dispatch)(
        0x2007c, pMedia, pCompressed_frame, pOffset, pSize, pDuration,
        pImage_description, pSample_count, pSample_flags, pSample_time);
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

// C2PORT-NONMATCHING: best 75.00% | difftest PASS 2000 trials (spec difftest/specs/00568a97_S3UpdateSampleList.json) | reason vc5-regalloc-stack-layout
// FUNCTION: CARMA2_HW 0x00568a97
void C2_HOOK_FASTCALL S3UpdateSampleList(int* pOld_sample_ids, int pOld_count, int* pNew_sample_ids, int pNew_count) {
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
