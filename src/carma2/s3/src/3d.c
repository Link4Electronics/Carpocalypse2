#include "../include/s3/internal/3d.h"

#include "audio.h"
#include "../include/s3/internal/sample.h"

#include "platform.h"

#include "carpocalypse2_macros.h"

#include "c2_math.h"
#include <stddef.h>
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x006b2c6c
float gFLOAT_006b2c6c;

// GLOBAL: CARMA2_HW 0x006b2c4c
float gFLOAT_006b2c4c;

// GLOBAL: CARMA2_HW 0x006b2c68
float gFLOAT_006b2c68;

// GLOBAL: CARMA2_HW 0x006b2c20
br_vector3 gS3_listener_position_now;

// GLOBAL: CARMA2_HW 0x006b2c58
br_vector3 gS3_listener_vel_now;

// GLOBAL: CARMA2_HW 0x006b2c30
br_vector3 gS3_listener_left_now;

// GLOBAL: CARMA2_HW 0x006b2c50
int gS3_listener_pos_is_brender;

// GLOBAL: CARMA2_HW 0x006b2c70
br_vector3* gS3_listener_pos_ptr;

// GLOBAL: CARMA2_HW 0x006b2c40
br_vector3 gS3_listener_position_old;

// GLOBAL: CARMA2_HW 0x006b2c74
br_vector3* gS3_listener_vel_ptr;

// GLOBAL: CARMA2_HW 0x006b2c64
int gS3_listener_vel_is_brender;

// GLOBAL: CARMA2_HW 0x006b2c78
br_vector3* gS3_listener_left_ptr;

// GLOBAL: CARMA2_HW 0x006b2c54
int gS3_listener_left_is_brender;


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tS3_channel* C2_HOOK_FASTCALL S3AllocateChannel(tS3_outlet* pOutlet, int pPriority);
extern int C2_HOOK_FASTCALL S3Calculate3D(tS3_channel* pChannel, int pAmbient);
extern int C2_HOOK_FASTCALL S3CalculatePriority(int pPriority, int pVolumeFactor);
extern int C2_HOOK_FASTCALL S3ExecuteSampleFilterFuncs(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL S3GenerateTag(tS3_outlet* outlet);
extern tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id);
extern int C2_HOOK_FASTCALL S3IRandomBetweenLog(int pMin, int pMax, int pDefault);
extern int C2_HOOK_FASTCALL S3IRandomBetween__dup(int pMin, int pMax, int pDefault);
extern tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);
extern int C2_HOOK_FASTCALL S3PlaySample(tS3_channel* pChannel, tS3_descriptor* pDescriptor);
extern int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* pChannel);
extern void __cdecl _ftol();
extern tS3_channel gS3_channel_template;
extern int gS3_enabled;
extern tS3_error_codes gS3_last_error;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int gS3_delta_time;
extern int gS3_inside_cockpit;
extern char rendererStateStoredDispatch[];
#endif
// FUNCTION: CARMA2_HW 0x00567ab8
void C2_HOOK_FASTCALL S3StopSoundSource(tS3_sound_source* src) {
    if (gS3_enabled) {
        if (src == NULL) {
            return;
        }
        if (src->channel != NULL && src->tag != 0 && src->channel->tag == src->tag) {
            src->channel->termination_reason = eS3_tr_stopped;
            src->channel->spatial_sound = 0;
            S3StopChannel(src->channel);
            src->channel->sound_source_ptr = NULL;
        }
        src->ambient = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00567875
void C2_NAKED C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_channel* channel;
    int is_sample;

    is_sample = 0;
    if (!gS3_enabled) {
        return;
    }
    if (src == NULL) {
        return;
    }
    channel = src->channel;
    if (tag != -1 && src->sound_id != tag) {
        tS3_descriptor* desc = S3GetDescriptorByID(tag);
        if (desc == NULL) {
            return;
        }
        if (desc->type == 0) {
            src->sound_id = tag;
            if (desc->buffer_description == NULL || (desc->flags & 0x2)) {
                if (S3LoadSample(tag) == eS3_error_none) {
                    return;
                }
            }
            is_sample = 1;
        }
    }
    if (pSpeed != -1) {
        if (pSpeed <= 0) {
            pSpeed = 0x10000;
        }
        src->speed = pSpeed;
    }
    if (pPitch != -1) {
        if (pPitch <= 0) {
            pPitch = 0x10000;
        }
        src->pitch = pPitch;
        if (channel != NULL && channel->descriptor != NULL && channel->descriptor->type == 0) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, source_rate, 0x54);

            channel->source_rate = S3IRandomBetweenLog(channel->descriptor->min_pitch, channel->descriptor->max_pitch, channel->descriptor->buffer_description->sample_rate);
            channel->source_rate = (int)((float)channel->source_rate * ldexpf((float)src->pitch, -16));
        }
    }
    if (pAmbient_repeats != -1) {
        src->ambient_repeats = MAX(0, pAmbient_repeats);
        if (channel != NULL) {
            channel->repetitions = src->ambient_repeats;
        }
    }

    if (pMax_distance_squared != -1.0f) {
        src->max_distance_sq = pMax_distance_squared;
        if (channel != NULL) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, max_distance_squared, 0x58);

            channel->max_distance_squared = pMax_distance_squared;
        }
    }
    if (pPeriod != -1) {
        src->period = pPeriod;
    }
    if (pVolume != -1) {
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (pVolume < 0) {
            pVolume = 0;
        }
        if (src->volume != pVolume) {
            src->volume = pVolume;
        }
        if (channel != NULL) {
            channel->volume_multiplier = pVolume;
        }
    }
    if (channel != NULL) {
        if (channel->sound_source_ptr != src) {
            S3StopChannel(channel);
        }
        if (pVolume == 0 || is_sample) {
            channel->spatial_sound = 0;
            channel->needs_service = 1;
            S3StopChannel(channel);
            src->channel = NULL;
            src->tag = 0;
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x1c
            mov dword ptr [ebp - 0x14], edx
            mov dword ptr [ebp - 0x10], ecx
            mov dword ptr [ebp - 4], 0
            cmp dword ptr [gS3_enabled], 0
            je L00567ab2
            cmp dword ptr [ebp + 8], 0
            jne L005678a0
            jmp L00567ab2
    L005678a0:
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax]
            mov dword ptr [ebp - 0xc], ecx
            cmp dword ptr [ebp - 0x14], -1
            je L0056790f
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + 0x28]
            cmp eax, dword ptr [ebp - 0x14]
            je L0056790f
            mov ecx, dword ptr [ebp - 0x14]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 8], 0
            jne L005678cf
            jmp L00567ab2
    L005678cf:
            mov ecx, dword ptr [ebp - 8]
            cmp dword ptr [ecx + 4], 0
            jne L0056790f
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp - 0x14]
            mov dword ptr [edx + 0x28], eax
            mov ecx, dword ptr [ebp - 8]
            cmp dword ptr [ecx + 0x40], 0
            je L005678f7
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L00567908
    L005678f7:
            mov ecx, dword ptr [ebp - 0x14]
            call S3LoadSample
            test eax, eax
            jne L00567908
            jmp L00567ab2
    L00567908:
            mov dword ptr [ebp - 4], 1
    L0056790f:
            cmp dword ptr [ebp + 0x20], -1
            je L0056792b
            cmp dword ptr [ebp + 0x20], 1
            jge L00567922
            mov dword ptr [ebp + 0x20], 0x10000
    L00567922:
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp + 0x20]
            mov dword ptr [ecx + 0x3c], edx
    L0056792b:
            cmp dword ptr [ebp + 0x1c], -1
            je L005679bd
            cmp dword ptr [ebp + 0x1c], 1
            jge L00567942
            mov dword ptr [ebp + 0x1c], 0x10000
    L00567942:
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebp + 0x1c]
            mov dword ptr [eax + 0x38], ecx
            cmp dword ptr [ebp - 0xc], 0
            je L005679bd
            mov edx, dword ptr [ebp - 0xc]
            cmp dword ptr [edx + 0x60], 0
            je L005679bd
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x60]
            cmp dword ptr [ecx + 4], 0
            jne L005679bd
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [eax + 0x40]
            mov edx, dword ptr [ecx + 4]
            push edx
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x60]
            mov edx, dword ptr [ecx + 0x1c]
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [eax + 0x60]
            mov ecx, dword ptr [ecx + 0x18]
            call S3IRandomBetweenLog
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 0x54], eax
            mov eax, dword ptr [ebp - 0xc]
            fild dword ptr [eax + 0x54]
            fstp qword ptr [ebp - 0x1c]
            push -0x10
            mov ecx, dword ptr [ebp + 8]
            fild dword ptr [ecx + 0x38]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fmul qword ptr [ebp - 0x1c]
            call _ftol
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 0x54], eax
    L005679bd:
            cmp dword ptr [ebp + 0x14], -1
            je L005679e8
            xor eax, eax
            cmp dword ptr [ebp + 0x14], 0
            setle al
            dec eax
            and eax, dword ptr [ebp + 0x14]
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [ecx + 0x20], eax
            cmp dword ptr [ebp - 0xc], 0
            je L005679e8
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x20]
            mov dword ptr [edx + 0xc], ecx
    L005679e8:
            cmp dword ptr [ebp + 0xc], 0xbf800000
            je L00567a09
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0xc]
            mov dword ptr [edx + 0x2c], eax
            cmp dword ptr [ebp - 0xc], 0
            je L00567a09
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ebp + 0xc]
            mov dword ptr [ecx + 0x58], edx
    L00567a09:
            cmp dword ptr [ebp + 0x10], -1
            je L00567a18
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebp + 0x10]
            mov dword ptr [eax + 0x30], ecx
    L00567a18:
            cmp dword ptr [ebp + 0x18], -1
            je L00567a5e
            cmp dword ptr [ebp + 0x18], 0xff
            jle L00567a2e
            mov dword ptr [ebp + 0x18], 0xff
    L00567a2e:
            cmp dword ptr [ebp + 0x18], 0
            jge L00567a3b
            mov dword ptr [ebp + 0x18], 0
    L00567a3b:
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + 0x34]
            cmp eax, dword ptr [ebp + 0x18]
            je L00567a4f
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp + 0x18]
            mov dword ptr [ecx + 0x34], edx
    L00567a4f:
            cmp dword ptr [ebp - 0xc], 0
            je L00567a5e
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [ebp + 0x18]
            mov dword ptr [eax + 0x24], ecx
    L00567a5e:
            cmp dword ptr [ebp - 0xc], 0
            je L00567ab2
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x74]
            cmp eax, dword ptr [ebp + 8]
            je L00567a77
            mov ecx, dword ptr [ebp - 0xc]
            call S3StopChannel
    L00567a77:
            cmp dword ptr [ebp + 0x18], 0
            je L00567a83
            cmp dword ptr [ebp - 4], 0
            je L00567ab2
    L00567a83:
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ecx + 0x14], 0
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 8], 1
            mov ecx, dword ptr [ebp - 0xc]
            call S3StopChannel
            mov eax, dword ptr [ebp + 8]
            mov dword ptr [eax], 0
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [ecx + 0x40], 0
    L00567ab2:
            mov esp, ebp
            pop ebp
            ret 0x1c
    }
#endif
}

#define S3_FUN_00565e39_MACRO(A1, A2, A3) \
    if (A1 == -1.f) { \
        A1 = .25f; \
    } \
    if (A2 == -1.f) { \
        A2 = 1.2f; \
    } \
    if (A3 == -1.f) { \
        A3 = 130000.f; \
    } \
    gFLOAT_006b2c6c = A1; \
    gFLOAT_006b2c4c = (float)sqrt(A3 / A2); \
    gFLOAT_006b2c68 = gFLOAT_006b2c4c * gFLOAT_006b2c6c;

// FUNCTION: CARMA2_HW 0x00565e39
int C2_HOOK_STDCALL FUN_00565e39(float a1, float a2, float a3) {
    S3_FUN_00565e39_MACRO(a1, a2, a3);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00565d70
int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3) {
    S3_FUN_00565e39_MACRO(a1, a2, a3);
    gS3_listener_position_now.v[0] = gS3_listener_position_now.v[1] = gS3_listener_position_now.v[2] = 0.0f;
    gS3_listener_vel_now.v[0] =  gS3_listener_vel_now.v[1] = gS3_listener_vel_now.v[2] = 0.0f;
    gS3_listener_left_now.v[0] = gS3_listener_left_now.v[1] = gS3_listener_left_now.v[2] = 0.0f;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00565ec7
void C2_HOOK_FASTCALL S3CopyBrVector3(br_vector3* pDest, br_vector3* pSrc) {
    pDest->v[0] = pSrc->v[0];
    pDest->v[1] = pSrc->v[1];
    pDest->v[2] = pSrc->v[2];
}

// FUNCTION: CARMA2_HW 0x0056770a
void C2_HOOK_FASTCALL S3CopyS3Vector3(tS3_vector3* pDest, tS3_vector3* pSrc) {
    pDest->x = pSrc->x;
    pDest->y = pSrc->y;
    pDest->z = pSrc->z;
}

// FUNCTION: CARMA2_HW 0x00565faa
void C2_HOOK_FASTCALL S3BindListenerPositionBRender(br_vector3* pos) {
    gS3_listener_pos_ptr = pos;
    gS3_listener_pos_is_brender = 1;
    S3CopyBrVector3(&gS3_listener_position_old, pos);
}

// FUNCTION: CARMA2_HW 0x00565ff1
void C2_HOOK_FASTCALL S3BindListenerVelocityBRender(br_vector3* vel) {
    gS3_listener_vel_ptr = vel;
    gS3_listener_vel_is_brender = 1;
}

// FUNCTION: CARMA2_HW 0x0056602b
void C2_HOOK_FASTCALL S3BindListenerLeftBRender(br_vector3* left) {
    gS3_listener_left_ptr = left;
    gS3_listener_left_is_brender = 1;
}

// FUNCTION: CARMA2_HW 0x00566048
int C2_NAKED C2_HOOK_FASTCALL S3StartSound3D(tS3_outlet *pOutlet, int pSound, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    tS3_channel* channel;
    float rate;

    if (gS3_enabled) {
        descriptor = S3GetDescriptorByID(pSound);
        if (descriptor == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return 0;
        }
        if (descriptor->type != 0) {
            return 0;
        }
        if (descriptor->buffer_description == NULL || (descriptor->flags & 0x2)) {
            if (!S3LoadSample(pSound)) {
                gS3_last_error = eS3_error_load_sound;
                return 0;
            }
        }
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (pVolume < 0) {
            pVolume = S3IRandomBetween__dup(descriptor->min_volume, descriptor->max_volume, 128);
        }
        memset(&gS3_channel_template, 0, sizeof(gS3_channel_template));
        gS3_channel_template.sound_source_ptr = NULL;
        gS3_channel_template.max_distance_squared = 150.0f;
        gS3_channel_template.volume_multiplier = pVolume;
        gS3_channel_template.source_volume = pVolume;
        gS3_channel_template.rate = S3IRandomBetweenLog(descriptor->min_pitch,descriptor->max_pitch,
        descriptor->buffer_description->sample_rate);
        if (pPitch == -1) {
            pPitch = BR_FIXED_INT(1);
        }
        if (pSpeed == -1) {
            pSpeed = BR_FIXED_INT(1);
        }
        rate = (float)(gS3_channel_template.rate * ldexp((double)pPitch, -16));
        gS3_channel_template.rate = (int)rate;
        if (!pOutlet->independent_pitch) {
            gS3_channel_template.rate = (int)((float)ldexp((double)pSpeed, -16) * rate);
        }
        gS3_channel_template.field_0x28 = 0.0f;
        gS3_channel_template.source_rate = gS3_channel_template.rate;
        gS3_channel_template.position.x = pInitial_position->v[0];
        gS3_channel_template.position.y = pInitial_position->v[1];
        gS3_channel_template.position.z = pInitial_position->v[2];
        gS3_channel_template.velocity.x = pInitial_velocity->v[0];
        gS3_channel_template.velocity.y = pInitial_velocity->v[1];
        gS3_channel_template.velocity.z = pInitial_velocity->v[2];
        if (S3Calculate3D(&gS3_channel_template, 0)) {
            channel = S3AllocateChannel(pOutlet, S3CalculatePriority(gS3_channel_template.volume_multiplier, descriptor->priority));
            if (channel == NULL) {
                gS3_last_error = eS3_error_channel_alloc;
                return 0;
            }
            channel->volume_multiplier = gS3_channel_template.volume_multiplier;
            channel->field_0x28 = gS3_channel_template.field_0x28;
            channel->rate = gS3_channel_template.rate;
            channel->spatial_sound = 1;
            channel->sound_source_ptr = NULL;
            channel->descriptor = descriptor;
            channel->needs_service = 0;
            channel->termination_reason = 0;
            channel->position.x = gS3_channel_template.position.x;
            channel->position.y = gS3_channel_template.position.y;
            channel->position.z = gS3_channel_template.position.z;
            channel->prev_position.x = gS3_channel_template.prev_position.x;
            channel->prev_position.y = gS3_channel_template.prev_position.y;
            channel->prev_position.z = gS3_channel_template.prev_position.z;
            channel->velocity.x = gS3_channel_template.velocity.x;
            channel->velocity.y = gS3_channel_template.velocity.y;
            channel->velocity.z = gS3_channel_template.velocity.z;
            channel->repetitions = (pRepeats <= 0) ? 0 : pRepeats;
            channel->tag = S3GenerateTag(pOutlet);
            channel->source_volume = gS3_channel_template.source_volume;
            channel->source_rate = gS3_channel_template.source_rate;
            channel->max_distance_squared = gS3_channel_template.max_distance_squared;
            S3ExecuteSampleFilterFuncs(channel);
            if (!S3PlaySample(channel, channel->descriptor)) {
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
            sub esp, 0x24
            mov dword ptr [ebp - 0x14], edx
            mov dword ptr [ebp - 0x10], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00566398
            mov ecx, dword ptr [ebp - 0x14]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00566083
            mov dword ptr [gS3_last_error], 6
            xor eax, eax
            jmp L0056639a
    L00566083:
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 4], 0
            je L00566093
            xor eax, eax
            jmp L0056639a
    L00566093:
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            je L005660a9
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L005660c6
    L005660a9:
            mov ecx, dword ptr [ebp - 0x14]
            call S3LoadSample
            test eax, eax
            jne L005660c6
            mov dword ptr [gS3_last_error], 8
            xor eax, eax
            jmp L0056639a
    L005660c6:
            cmp dword ptr [ebp + 0x14], 0xff
            jle L005660d6
            mov dword ptr [ebp + 0x14], 0xff
    L005660d6:
            cmp dword ptr [ebp + 0x14], 0
            jge L005660f5
            push 0x80
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 0x14]
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [eax + 0x10]
            call S3IRandomBetween__dup
            mov dword ptr [ebp + 0x14], eax
    L005660f5:
            push 0x78
            push 0
            push offset gS3_channel_template
            call memset
            add esp, 0xc
            mov dword ptr [gS3_channel_template+0x74], 0
            mov dword ptr [gS3_channel_template+0x58], 0x43160000
            mov ecx, dword ptr [ebp + 0x14]
            mov dword ptr [gS3_channel_template+0x24], ecx
            mov edx, dword ptr [ebp + 0x14]
            mov dword ptr [gS3_channel_template+0x50], edx
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
            cmp dword ptr [ebp + 0x18], -1
            jne L00566159
            mov dword ptr [ebp + 0x18], 0x10000
    L00566159:
            cmp dword ptr [ebp + 0x1c], -1
            jne L00566166
            mov dword ptr [ebp + 0x1c], 0x10000
    L00566166:
            fild dword ptr [gS3_channel_template+0x10]
            fstp dword ptr [ebp - 0x18]
            push -0x10
            fild dword ptr [ebp + 0x18]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fst dword ptr [ebp - 0x1c]
            fmul dword ptr [ebp - 0x18]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
            mov edx, dword ptr [ebp - 0x10]
            cmp dword ptr [edx + 0xc], 0
            jne L005661c7
            fild dword ptr [gS3_channel_template+0x10]
            fstp dword ptr [ebp - 0x20]
            push -0x10
            fild dword ptr [ebp + 0x1c]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fst dword ptr [ebp - 0x24]
            fmul dword ptr [ebp - 0x20]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
    L005661c7:
            mov dword ptr [gS3_channel_template+0x28], 0
            mov eax, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [gS3_channel_template+0x54], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ecx]
            mov dword ptr [gS3_channel_template+0x2c], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [gS3_channel_template+0x30], ecx
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [gS3_channel_template+0x34], eax
            mov ecx, dword ptr [ebp + 0xc]
            mov edx, dword ptr [ecx]
            mov dword ptr [gS3_channel_template+0x38], edx
            mov eax, dword ptr [ebp + 0xc]
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [gS3_channel_template+0x3c], ecx
            mov edx, dword ptr [ebp + 0xc]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [gS3_channel_template+0x40], eax
            xor edx, edx
            mov ecx, offset gS3_channel_template
            call S3Calculate3D
            test eax, eax
            je L00566398
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [gS3_channel_template+0x24]
            call S3CalculatePriority
            mov dword ptr [ebp - 8], eax
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0x10]
            call S3AllocateChannel
            mov dword ptr [ebp - 0xc], eax
            cmp dword ptr [ebp - 0xc], 0
            jne L0056626c
            mov dword ptr [gS3_last_error], 0xa
            xor eax, eax
            jmp L0056639a
    L0056626c:
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [gS3_channel_template+0x24]
            mov dword ptr [edx + 0x24], eax
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [gS3_channel_template+0x28]
            mov dword ptr [ecx + 0x28], edx
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [eax + 0x10], ecx
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 0x14], 1
            mov eax, dword ptr [ebp - 0xc]
            mov dword ptr [eax + 0x74], 0
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ebp - 4]
            mov dword ptr [ecx + 0x60], edx
            mov eax, dword ptr [ebp - 0xc]
            mov dword ptr [eax + 8], 0
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ecx + 4], 0
            mov edx, dword ptr [ebp - 0xc]
            add edx, 0x2c
            mov eax, dword ptr [gS3_channel_template+0x2c]
            mov dword ptr [edx], eax
            mov ecx, dword ptr [gS3_channel_template+0x30]
            mov dword ptr [edx + 4], ecx
            mov eax, dword ptr [gS3_channel_template+0x34]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp - 0xc]
            add ecx, 0x44
            mov edx, dword ptr [gS3_channel_template+0x44]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [gS3_channel_template+0x48]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [gS3_channel_template+0x4c]
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [ebp - 0xc]
            add eax, 0x38
            mov ecx, dword ptr [gS3_channel_template+0x38]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [gS3_channel_template+0x3c]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [gS3_channel_template+0x40]
            mov dword ptr [eax + 8], ecx
            xor edx, edx
            cmp dword ptr [ebp + 0x10], 0
            setle dl
            dec edx
            and edx, dword ptr [ebp + 0x10]
            mov eax, dword ptr [ebp - 0xc]
            mov dword ptr [eax + 0xc], edx
            mov ecx, dword ptr [ebp - 0x10]
            call S3GenerateTag
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ecx + 0x1c], eax
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [gS3_channel_template+0x50]
            mov dword ptr [edx + 0x50], eax
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [gS3_channel_template+0x54]
            mov dword ptr [ecx + 0x54], edx
            mov eax, dword ptr [ebp - 0xc]
            mov ecx, dword ptr [gS3_channel_template+0x58]
            mov dword ptr [eax + 0x58], ecx
            mov ecx, dword ptr [ebp - 0xc]
            call S3ExecuteSampleFilterFuncs
            mov edx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 0xc]
            call S3PlaySample
            test eax, eax
            jne L00566390
            mov edx, dword ptr [ebp - 0xc]
            mov dword ptr [edx + 8], 1
            mov dword ptr [gS3_last_error], 9
            xor eax, eax
            jmp L0056639a
    L00566390:
            mov eax, dword ptr [ebp - 0xc]
            mov eax, dword ptr [eax + 0x1c]
            jmp L0056639a
    L00566398:
            xor eax, eax
    L0056639a:
            mov esp, ebp
            pop ebp
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056773c
int C2_NAKED C2_HOOK_FASTCALL S3BindAmbientSoundToOutlet(tS3_outlet* pOutlet, int pSound, tS3_sound_source* source, float pMax_distance, int pPeriod, int pRepeats, int pVolume, int pPitch, int pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* desc;

    if (gS3_enabled) {
        if (source == NULL) {
            return eS3_error_nonexistant_source;
        }
        desc = S3GetDescriptorByID(pSound);
        if (desc == NULL) {
            return eS3_error_bad_id;
        }
        if (desc->type == 0) {
            if ((desc->buffer_description == NULL || (desc->flags & 2) != 0) && S3LoadSample(pSound) == eS3_error_none) {
                return eS3_error_load_sound;
            }
            if (pVolume > 255) {
                pVolume = 255;
            }
            if (pVolume < 0) {
                pVolume = 128;
            }
            if (pPitch < 0) {
                pPitch = BR_FIXED_INT(1);
            }
            if (pSpeed < 0) {
                pSpeed = BR_FIXED_INT(1);
            }
            source->bound_outlet = pOutlet;
            source->sound_id = pSound;
            source->volume = pVolume;
            source->max_distance_sq = pMax_distance;
            source->period = pPeriod;
            source->pitch = pPitch;
            source->speed = pSpeed;
            source->ambient = 1;

            source->ambient_repeats = pRepeats <= 0 ? 0 : pRepeats;
            source->time_since_last_played = pPeriod;
            source->channel = NULL;
            source->tag = 0;
        }
    }
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0xc
            mov dword ptr [ebp - 0xc], edx
            mov dword ptr [ebp - 8], ecx
            cmp dword ptr [gS3_enabled], 0
            je L0056786d
            cmp dword ptr [ebp + 8], 0
            jne L00567765
            mov eax, 0x13
            jmp L0056786f
    L00567765:
            mov ecx, dword ptr [ebp - 0xc]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00567780
            mov eax, 6
            jmp L0056786f
    L00567780:
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 4], 0
            jne L005677b7
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            je L0056779f
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L005677b5
    L0056779f:
            mov ecx, dword ptr [ebp - 0xc]
            call S3LoadSample
            test eax, eax
            jne L005677b5
            mov eax, 8
            jmp L0056786f
    L005677b5:
            jmp L005677be
    L005677b7:
            xor eax, eax
            jmp L0056786f
    L005677be:
            cmp dword ptr [ebp + 0x18], 0xff
            jle L005677ce
            mov dword ptr [ebp + 0x18], 0xff
    L005677ce:
            cmp dword ptr [ebp + 0x18], 0
            jge L005677db
            mov dword ptr [ebp + 0x18], 0x80
    L005677db:
            cmp dword ptr [ebp + 0x1c], 0
            jge L005677e8
            mov dword ptr [ebp + 0x1c], 0x10000
    L005677e8:
            cmp dword ptr [ebp + 0x20], 0
            jge L005677f5
            mov dword ptr [ebp + 0x20], 0x10000
    L005677f5:
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [eax + 0x28], ecx
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x18]
            mov dword ptr [edx + 0x34], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp + 0xc]
            mov dword ptr [ecx + 0x2c], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebp + 0x10]
            mov dword ptr [eax + 0x30], ecx
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x1c]
            mov dword ptr [edx + 0x38], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp + 0x20]
            mov dword ptr [ecx + 0x3c], edx
            mov eax, dword ptr [ebp + 8]
            mov dword ptr [eax + 0x1c], 1
            xor ecx, ecx
            cmp dword ptr [ebp + 0x14], 0
            setle cl
            dec ecx
            and ecx, dword ptr [ebp + 0x14]
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [edx + 0x20], ecx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebp + 0x10]
            mov dword ptr [eax + 0x24], ecx
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [edx], 0
            mov eax, dword ptr [ebp + 8]
            mov dword ptr [eax + 0x40], 0
    L0056786d:
            xor eax, eax
    L0056786f:
            mov esp, ebp
            pop ebp
            ret 0x1c
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005676da
void C2_HOOK_FASTCALL S3CopyVector3(void* pDest, void* pSrc, int pIs_BRender) {
    if (pIs_BRender) {
        S3CopyBrVector3(pDest, pSrc);
    } else {
        S3CopyS3Vector3(pDest, pSrc);
    }
}

// FUNCTION: CARMA2_HW 0x005675ce
void C2_NAKED C2_HOOK_FASTCALL S3UpdateListenerVectors(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gS3_listener_pos_ptr != NULL) {
        S3CopyVector3(&gS3_listener_position_now, gS3_listener_pos_ptr, gS3_listener_pos_is_brender);
    }
    if (gS3_listener_vel_ptr != NULL) {
        S3CopyVector3(&gS3_listener_vel_now, gS3_listener_vel_ptr, gS3_listener_vel_is_brender);
    } else {
        gS3_listener_vel_now.v[0] = (gS3_listener_position_now.v[0] - gS3_listener_position_old.v[0]) * (float)gS3_delta_time / 1000.f;
        gS3_listener_vel_now.v[1] = (gS3_listener_position_now.v[1] - gS3_listener_position_old.v[1]) * (float)gS3_delta_time / 1000.f;
        gS3_listener_vel_now.v[2] = (gS3_listener_position_now.v[2] - gS3_listener_position_old.v[2]) * (float)gS3_delta_time / 1000.f;
        BrVector3Copy(&gS3_listener_position_old, &gS3_listener_position_now);
    }
    if (gS3_listener_left_ptr != NULL) {
        S3CopyVector3(&gS3_listener_left_now, gS3_listener_left_ptr, gS3_listener_left_is_brender);
    } else {
        gS3_listener_left_now.v[0] = gS3_listener_position_now.v[0] - 1.f;
        gS3_listener_left_now.v[1] = gS3_listener_position_now.v[1];
        gS3_listener_left_now.v[2] = gS3_listener_position_now.v[2];
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            cmp dword ptr [gS3_listener_pos_ptr], 0
            je L005675f0
            mov eax, dword ptr [gS3_listener_pos_is_brender]
            push eax
            mov edx, dword ptr [gS3_listener_pos_ptr]
            mov ecx, offset gS3_listener_position_now
            call S3CopyVector3
    L005675f0:
            cmp dword ptr [gS3_listener_vel_ptr], 0
            je L00567612
            mov ecx, dword ptr [gS3_listener_vel_is_brender]
            push ecx
            mov edx, dword ptr [gS3_listener_vel_ptr]
            mov ecx, offset gS3_listener_vel_now
            call S3CopyVector3
            jmp L0056768e
    L00567612:
            fld dword ptr [gS3_listener_position_now]
            fsub dword ptr [gS3_listener_position_old]
            fimul dword ptr [gS3_delta_time]
            fdiv dword ptr [rendererStateStoredDispatch+0xc48]
            fstp dword ptr [gS3_listener_vel_now]
            fld dword ptr [gS3_listener_position_now+0x4]
            fsub dword ptr [gS3_listener_position_old+0x4]
            fimul dword ptr [gS3_delta_time]
            fdiv dword ptr [rendererStateStoredDispatch+0xc48]
            fstp dword ptr [gS3_listener_vel_now+0x4]
            fld dword ptr [gS3_listener_position_now+0x8]
            fsub dword ptr [gS3_listener_position_old+0x8]
            fimul dword ptr [gS3_delta_time]
            fdiv dword ptr [rendererStateStoredDispatch+0xc48]
            fstp dword ptr [gS3_listener_vel_now+0x8]
            mov edx, dword ptr [gS3_listener_position_now]
            mov dword ptr [gS3_listener_position_old], edx
            mov eax, dword ptr [gS3_listener_position_now+0x4]
            mov dword ptr [gS3_listener_position_old+0x4], eax
            mov ecx, dword ptr [gS3_listener_position_now+0x8]
            mov dword ptr [gS3_listener_position_old+0x8], ecx
    L0056768e:
            cmp dword ptr [gS3_listener_left_ptr], 0
            je L005676b0
            mov edx, dword ptr [gS3_listener_left_is_brender]
            push edx
            mov edx, dword ptr [gS3_listener_left_ptr]
            mov ecx, offset gS3_listener_left_now
            call S3CopyVector3
            jmp L005676d8
    L005676b0:
            fld dword ptr [gS3_listener_position_now]
            fsub dword ptr [rendererStateStoredDispatch+0xc74]
            fstp dword ptr [gS3_listener_left_now]
            mov eax, dword ptr [gS3_listener_position_now+0x4]
            mov dword ptr [gS3_listener_left_now+0x4], eax
            mov ecx, dword ptr [gS3_listener_position_now+0x8]
            mov dword ptr [gS3_listener_left_now+0x8], ecx
    L005676d8:
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00567b2d
int C2_NAKED C2_HOOK_FASTCALL S3UpdateSourcePosition(tS3_sound_source *pSource) {
#ifndef CARPOCALYPSE2_MATCHING
    if (pSource == NULL) {
        return 0;
    }
    if (gS3_enabled && pSource->ambient) {
        tS3_outlet* outlet;
        tS3_descriptor* descriptor;

        outlet = pSource->bound_outlet;
        descriptor = S3GetDescriptorByID(pSource->sound_id);
        if (descriptor == NULL) {
            gS3_last_error = eS3_error_bad_id;
            return eS3_error_none;
        }
        if (descriptor->buffer_description == NULL) {
            return eS3_error_nonexistant_source;
        }
        if (descriptor->type != 0) {
            return eS3_error_none;
        }
        memset(&gS3_channel_template, 0, sizeof(tS3_channel));
        gS3_channel_template.source_volume = pSource->volume;
        gS3_channel_template.rate = S3IRandomBetweenLog(descriptor->min_pitch,
                descriptor->max_pitch,
                descriptor->buffer_description->sample_rate);
        if (pSource->pitch < 0) {
            pSource->pitch = BR_FIXED_INT(1);
        }
        if (pSource->speed < 0) {
            pSource->speed = BR_FIXED_INT(1);
        }
        gS3_channel_template.rate = (int)(ldexpf((float)pSource->pitch, -16) * (float)gS3_channel_template.rate);
        if (!outlet->independent_pitch) {
            gS3_channel_template.rate = (int)(ldexpf((float)pSource->speed, -16) * (float)gS3_channel_template.rate);
        }
        gS3_channel_template.source_rate = gS3_channel_template.rate;
        gS3_channel_template.sound_source_ptr = pSource;
        if (pSource->velocity_ptr == NULL) {
            S3CopyVector3(&gS3_channel_template.prev_position, pSource->position_ptr, pSource->brender_vector);
        }
        gS3_channel_template.max_distance_squared = pSource->max_distance_sq;
        if (S3Calculate3D(&gS3_channel_template, 1) != 0) {
            tS3_channel* channel;

            channel = S3AllocateChannel(outlet, S3CalculatePriority(gS3_channel_template.volume_multiplier, descriptor->priority));
            if (channel == NULL) {
                gS3_last_error = eS3_error_channel_alloc;
                pSource->tag = 0;
                pSource->channel = NULL;
                return eS3_error_none;
            }
            if ((descriptor->buffer_description == NULL || (descriptor->flags & 0x2)) && S3LoadSample(pSource->sound_id) == 0) {
                gS3_last_error = eS3_error_load_sound;
                channel->needs_service = 1;
                return eS3_error_none;
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
            channel->spatial_sound = 2;
            channel->sound_source_ptr = pSource;
            channel->descriptor = descriptor;
            channel->tag = S3GenerateTag(outlet);
            channel->repetitions = pSource->ambient_repeats;
            channel->needs_service = 0;
            channel->termination_reason = 0;
            S3ExecuteSampleFilterFuncs(channel);
            if (S3PlaySample(channel, channel->descriptor) == 0) {
                channel->needs_service = 1;
                gS3_last_error = eS3_error_start_sound;
                return 0;
            }
            pSource->tag = channel->tag;
            pSource->channel = channel;
            return channel->tag;
        }
    }
    pSource->tag = 0;
    pSource->channel = NULL;
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x24
            mov dword ptr [ebp - 0x14], ecx
            cmp dword ptr [ebp - 0x14], 0
            jne L00567b43
            xor eax, eax
            jmp L00567ea1
    L00567b43:
            cmp dword ptr [gS3_enabled], 0
            je L00567e8c
            mov eax, dword ptr [ebp - 0x14]
            cmp dword ptr [eax + 0x1c], 0
            je L00567e8c
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [ebp - 0xc], edx
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [eax + 0x28]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L00567b8b
            mov dword ptr [gS3_last_error], 6
            xor eax, eax
            jmp L00567ea1
    L00567b8b:
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            jne L00567b9e
            mov eax, 0x13
            jmp L00567ea1
    L00567b9e:
            mov edx, dword ptr [ebp - 4]
            cmp dword ptr [edx + 4], 0
            je L00567bae
            xor eax, eax
            jmp L00567ea1
    L00567bae:
            push 0x78
            push 0
            push offset gS3_channel_template
            call memset
            add esp, 0xc
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [eax + 0x34]
            mov dword ptr [gS3_channel_template+0x50], ecx
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
            mov ecx, dword ptr [ebp - 0x14]
            cmp dword ptr [ecx + 0x38], 0
            jge L00567bfe
            mov edx, dword ptr [ebp - 0x14]
            mov dword ptr [edx + 0x38], 0x10000
    L00567bfe:
            mov eax, dword ptr [ebp - 0x14]
            cmp dword ptr [eax + 0x3c], 0
            jge L00567c11
            mov ecx, dword ptr [ebp - 0x14]
            mov dword ptr [ecx + 0x3c], 0x10000
    L00567c11:
            fild dword ptr [gS3_channel_template+0x10]
            fstp qword ptr [ebp - 0x1c]
            push -0x10
            mov edx, dword ptr [ebp - 0x14]
            fild dword ptr [edx + 0x38]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fmul qword ptr [ebp - 0x1c]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 0xc], 0
            jne L00567c72
            fild dword ptr [gS3_channel_template+0x10]
            fstp qword ptr [ebp - 0x24]
            push -0x10
            mov ecx, dword ptr [ebp - 0x14]
            fild dword ptr [ecx + 0x3c]
            sub esp, 8
            fstp qword ptr [esp]
            call ldexp
            add esp, 0xc
            fmul qword ptr [ebp - 0x24]
            call _ftol
            mov dword ptr [gS3_channel_template+0x10], eax
    L00567c72:
            mov edx, dword ptr [gS3_channel_template+0x10]
            mov dword ptr [gS3_channel_template+0x54], edx
            mov eax, dword ptr [ebp - 0x14]
            mov dword ptr [gS3_channel_template+0x74], eax
            mov ecx, dword ptr [ebp - 0x14]
            cmp dword ptr [ecx + 0xc], 0
            jne L00567ca6
            mov edx, dword ptr [ebp - 0x14]
            mov eax, dword ptr [edx + 4]
            push eax
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ecx + 8]
            mov ecx, offset gS3_channel_template+0x44
            call S3CopyVector3
    L00567ca6:
            mov edx, dword ptr [ebp - 0x14]
            mov eax, dword ptr [edx + 0x2c]
            mov dword ptr [gS3_channel_template+0x58], eax
            mov edx, 1
            mov ecx, offset gS3_channel_template
            call S3Calculate3D
            test eax, eax
            je L00567e8c
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [gS3_channel_template+0x24]
            call S3CalculatePriority
            mov dword ptr [ebp - 8], eax
            mov edx, dword ptr [ebp - 8]
            mov ecx, dword ptr [ebp - 0xc]
            call S3AllocateChannel
            mov dword ptr [ebp - 0x10], eax
            cmp dword ptr [ebp - 0x10], 0
            jne L00567d14
            mov dword ptr [gS3_last_error], 0xa
            mov edx, dword ptr [ebp - 0x14]
            mov dword ptr [edx + 0x40], 0
            mov eax, dword ptr [ebp - 0x14]
            mov dword ptr [eax], 0
            xor eax, eax
            jmp L00567ea1
    L00567d14:
            mov ecx, dword ptr [ebp - 4]
            cmp dword ptr [ecx + 0x40], 0
            je L00567d2a
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x30]
            and eax, 2
            test eax, eax
            je L00567d54
    L00567d2a:
            mov ecx, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [ecx + 0x28]
            call S3LoadSample
            test eax, eax
            jne L00567d54
            mov dword ptr [gS3_last_error], 8
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 8], 1
            xor eax, eax
            jmp L00567ea1
    L00567d54:
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
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [gS3_channel_template+0x58]
            mov dword ptr [ecx + 0x58], edx
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 0x14], 2
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ebp - 0x14]
            mov dword ptr [ecx + 0x74], edx
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 4]
            mov dword ptr [eax + 0x60], ecx
            mov ecx, dword ptr [ebp - 0xc]
            call S3GenerateTag
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 0x1c], eax
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ecx + 0x20]
            mov dword ptr [eax + 0xc], edx
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [eax + 8], 0
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [ecx + 4], 0
            mov ecx, dword ptr [ebp - 0x10]
            call S3ExecuteSampleFilterFuncs
            mov edx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp - 0x10]
            call S3PlaySample
            test eax, eax
            jne L00567e70
            mov edx, dword ptr [ebp - 0x10]
            mov dword ptr [edx + 8], 1
            mov dword ptr [gS3_last_error], 9
            xor eax, eax
            jmp L00567ea1
    L00567e70:
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ecx + 0x1c]
            mov dword ptr [eax + 0x40], edx
            mov eax, dword ptr [ebp - 0x14]
            mov ecx, dword ptr [ebp - 0x10]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [edx + 0x1c]
            jmp L00567ea1
    L00567e8c:
            mov eax, dword ptr [ebp - 0x14]
            mov dword ptr [eax + 0x40], 0
            mov ecx, dword ptr [ebp - 0x14]
            mov dword ptr [ecx], 0
            xor eax, eax
    L00567ea1:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00567ea5
int C2_HOOK_FASTCALL S3UpdateSourceVectors(void) {
    tS3_sound_source* source;

    source = gS3_state.sources;
    while (source != NULL) {
        if (source->ambient) {
            if (source->period > 0) {
                source->time_since_last_played += gS3_delta_time;
            }
            if (source->channel != NULL && source->tag != source->channel->tag) {
                S3StopChannel(source->channel);
                source->channel = NULL;
                source->tag = 0;
            }
            if (source->channel == NULL) {
                if (source->time_since_last_played > source->period && source->period != 0 && source->tag == 0) {
                    if (source->volume > 0) {
                        if (S3UpdateSourcePosition(source) == 0) {
                            source->channel = NULL;
                            source->tag = 0;
                        }
                    }
                    source->time_since_last_played = 0;
                } else if ((!source->ambient_repeats || source->period == 0) && source->tag == 0) {
                    if (source->volume > 0) {
                        if (S3UpdateSourcePosition(source) == 0) {
                            source->channel = NULL;
                            source->tag = 0;
                        }
                    }
                    source->time_since_last_played = 0;
                }
            }
        }
        source = source->next;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056711c
int C2_NAKED C2_HOOK_FASTCALL S3Calculate3D(tS3_channel* pChannel, int pAmbient) {
#ifndef CARPOCALYPSE2_MATCHING
    float dist_squared;
    float dist;
    float sound_volume_factor;
    br_vector3 dir_pos_listener;
    br_vector3 dir_left;

    if (pChannel->sound_source_ptr != NULL) {
        tS3_sound_source* sound_source = pChannel->sound_source_ptr;

        if (sound_source->position_ptr != NULL) {
            S3CopyVector3(&pChannel->position, sound_source->position_ptr, sound_source->brender_vector);
        }
        if (sound_source->velocity_ptr != NULL) {
            S3CopyVector3(&pChannel->velocity, sound_source->velocity_ptr, sound_source->brender_vector);
        } else {
            pChannel->velocity.x = (pChannel->position.x - pChannel->prev_position.x) * (float)gS3_delta_time / 1000.f;
            pChannel->velocity.y = (pChannel->position.y - pChannel->prev_position.y) * (float)gS3_delta_time / 1000.f;
            pChannel->velocity.z = (pChannel->position.z - pChannel->prev_position.z) * (float)gS3_delta_time / 1000.f;
            pChannel->prev_position.x = pChannel->position.x;
            pChannel->prev_position.y = pChannel->position.y;
            pChannel->prev_position.z = pChannel->position.z;
        }
    }
    dist_squared =
              (pChannel->position.x - gS3_listener_position_now.v[0]) * (pChannel->position.x - gS3_listener_position_now.v[0])
            + (pChannel->position.y - gS3_listener_position_now.v[1]) * (pChannel->position.y - gS3_listener_position_now.v[1])
            + (pChannel->position.z - gS3_listener_position_now.v[2]) * (pChannel->position.z - gS3_listener_position_now.v[2]);

    if (dist_squared < 0.f) {
        dist_squared = -dist_squared;
    }
    if (dist_squared > pChannel->max_distance_squared) {
        return 0;
    }
    dist = dist_squared != 0.f ? (float)sqrt(dist_squared) : 0.f;
    if (pAmbient && dist != 0.f) {
        float doppler;

        doppler = 1.f -
                ( (pChannel->velocity.x - gS3_listener_vel_now.v[0]) * (pChannel->position.x - gS3_listener_position_now.v[0])
                + (pChannel->velocity.y - gS3_listener_vel_now.v[1]) * (pChannel->position.y - gS3_listener_position_now.v[1])
                + (pChannel->velocity.z - gS3_listener_vel_now.v[2]) * (pChannel->position.z - gS3_listener_position_now.v[2])) / dist / gFLOAT_006b2c68;
        if (doppler > 2.f) {
            doppler = 2.f;
        } else if (doppler < .5f) {
            doppler = .5f;
        }
        pChannel->rate = (int)((float)pChannel->source_rate * doppler);
    } else {
        pChannel->rate = pChannel->source_rate;
    }
    sound_volume_factor = 1.f / (1.f + dist / 6.f);
    if (!gS3_inside_cockpit) {
        sound_volume_factor *= 1.3f;
    }
    dir_pos_listener.v[0] = pChannel->position.x - gS3_listener_position_now.v[0];
    dir_pos_listener.v[1] = pChannel->position.y - gS3_listener_position_now.v[1];
    dir_pos_listener.v[2] = pChannel->position.z - gS3_listener_position_now.v[2];
    dir_left.v[0] = gS3_listener_left_now.v[0];
    dir_left.v[1] = gS3_listener_left_now.v[1];
    dir_left.v[2] = gS3_listener_left_now.v[2];
    BrVector3Normalise(&dir_left, &dir_left);
    BrVector3Normalise(&dir_pos_listener, &dir_pos_listener);
    pChannel->field_0x28 = -BrVector3Dot(&dir_left, &dir_pos_listener);
    pChannel->volume_multiplier = (int)((float)pChannel->source_volume * sound_volume_factor);
    if (pChannel->volume_multiplier < 0) {
        pChannel->volume_multiplier = 0;
    }
    if (pChannel->volume_multiplier > 255) {
        pChannel->volume_multiplier = 255;
    }
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x60
            mov dword ptr [ebp - 0x50], edx
            mov dword ptr [ebp - 0x4c], ecx
            mov dword ptr [ebp - 0x40], 0x3f800000
            mov dword ptr [ebp - 0xc], 0
            mov dword ptr [ebp - 8], 0xbf800000
            mov dword ptr [ebp - 4], 0
            mov eax, dword ptr [ebp - 0x4c]
            mov ecx, dword ptr [eax + 0x74]
            mov dword ptr [ebp - 0x18], ecx
            cmp dword ptr [ebp - 0x18], 0
            je L00567219
            mov edx, dword ptr [ebp - 0x18]
            cmp dword ptr [edx + 8], 0
            je L00567178
            mov eax, dword ptr [ebp - 0x18]
            mov ecx, dword ptr [eax + 4]
            push ecx
            mov edx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [edx + 8]
            mov ecx, dword ptr [ebp - 0x4c]
            add ecx, 0x2c
            call S3CopyVector3
    L00567178:
            mov eax, dword ptr [ebp - 0x18]
            cmp dword ptr [eax + 0xc], 0
            je L0056719b
            mov ecx, dword ptr [ebp - 0x18]
            mov edx, dword ptr [ecx + 4]
            push edx
            mov eax, dword ptr [ebp - 0x18]
            mov edx, dword ptr [eax + 0xc]
            mov ecx, dword ptr [ebp - 0x4c]
            add ecx, 0x38
            call S3CopyVector3
            jmp L00567219
    L0056719b:
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x2c]
            fsub dword ptr [edx + 0x44]
            fimul dword ptr [gS3_delta_time]
            fdiv dword ptr [rendererStateStoredDispatch+0xc48]
            mov eax, dword ptr [ebp - 0x4c]
            fstp dword ptr [eax + 0x38]
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x30]
            fsub dword ptr [edx + 0x48]
            fimul dword ptr [gS3_delta_time]
            fdiv dword ptr [rendererStateStoredDispatch+0xc48]
            mov eax, dword ptr [ebp - 0x4c]
            fstp dword ptr [eax + 0x3c]
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x34]
            fsub dword ptr [edx + 0x4c]
            fimul dword ptr [gS3_delta_time]
            fdiv dword ptr [rendererStateStoredDispatch+0xc48]
            mov eax, dword ptr [ebp - 0x4c]
            fstp dword ptr [eax + 0x40]
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x4c]
            mov eax, dword ptr [edx + 0x2c]
            mov dword ptr [ecx + 0x44], eax
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x4c]
            mov eax, dword ptr [edx + 0x30]
            mov dword ptr [ecx + 0x48], eax
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x4c]
            mov eax, dword ptr [edx + 0x34]
            mov dword ptr [ecx + 0x4c], eax
    L00567219:
            mov ecx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x2c]
            fsub dword ptr [gS3_listener_position_now]
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [edx + 0x2c]
            fsub dword ptr [gS3_listener_position_now]
            fmulp st(1), st(0)
            mov eax, dword ptr [ebp - 0x4c]
            fld dword ptr [eax + 0x30]
            fsub dword ptr [gS3_listener_position_now+0x4]
            mov ecx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x30]
            fsub dword ptr [gS3_listener_position_now+0x4]
            fmulp st(1), st(0)
            faddp st(1), st(0)
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [edx + 0x34]
            fsub dword ptr [gS3_listener_position_now+0x8]
            mov eax, dword ptr [ebp - 0x4c]
            fld dword ptr [eax + 0x34]
            fsub dword ptr [gS3_listener_position_now+0x8]
            fmulp st(1), st(0)
            faddp st(1), st(0)
            fst dword ptr [ebp - 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0xc4c]
            fnstsw ax
            test ah, 1
            je L00567283
            fld dword ptr [ebp - 0x14]
            fchs
            fstp dword ptr [ebp - 0x14]
    L00567283:
            mov ecx, dword ptr [ebp - 0x4c]
            fld dword ptr [ebp - 0x14]
            fcomp dword ptr [ecx + 0x58]
            fnstsw ax
            test ah, 0x41
            jne L0056729a
            xor eax, eax
            jmp L00567571
    L0056729a:
            fld dword ptr [ebp - 0x14]
            fcomp qword ptr [rendererStateStoredDispatch+0xc50]
            fnstsw ax
            test ah, 0x40
            jne L005672c0
            fld dword ptr [ebp - 0x14]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            call sqrt
            add esp, 8
            fstp dword ptr [ebp - 0x10]
            jmp L005672c7
    L005672c0:
            mov dword ptr [ebp - 0x10], 0
    L005672c7:
            cmp dword ptr [ebp - 0x50], 0
            je L0056738f
            fld dword ptr [ebp - 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0xc4c]
            fnstsw ax
            test ah, 0x40
            jne L0056738f
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [edx + 0x2c]
            fsub dword ptr [gS3_listener_position_now]
            mov eax, dword ptr [ebp - 0x4c]
            fld dword ptr [eax + 0x38]
            fsub dword ptr [gS3_listener_vel_now]
            fmulp st(1), st(0)
            mov ecx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x30]
            fsub dword ptr [gS3_listener_position_now+0x4]
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [edx + 0x3c]
            fsub dword ptr [gS3_listener_vel_now+0x4]
            fmulp st(1), st(0)
            faddp st(1), st(0)
            mov eax, dword ptr [ebp - 0x4c]
            fld dword ptr [eax + 0x34]
            fsub dword ptr [gS3_listener_position_now+0x8]
            mov ecx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x40]
            fsub dword ptr [gS3_listener_vel_now+0x8]
            fmulp st(1), st(0)
            faddp st(1), st(0)
            fdiv dword ptr [ebp - 0x10]
            fst dword ptr [ebp - 0x3c]
            fdiv dword ptr [gFLOAT_006b2c68]
            fsubr qword ptr [rendererStateStoredDispatch+0xc58]
            fst dword ptr [ebp - 0x40]
            fcomp dword ptr [rendererStateStoredDispatch+0xc60]
            fnstsw ax
            test ah, 0x41
            jne L00567362
            mov dword ptr [ebp - 0x40], 0x40000000
            jmp L00567379
    L00567362:
            fld dword ptr [ebp - 0x40]
            fcomp qword ptr [rendererStateStoredDispatch+0xc68]
            fnstsw ax
            test ah, 1
            je L00567379
            mov dword ptr [ebp - 0x40], 0x3f000000
    L00567379:
            mov edx, dword ptr [ebp - 0x4c]
            fild dword ptr [edx + 0x54]
            fmul dword ptr [ebp - 0x40]
            call _ftol
            mov ecx, dword ptr [ebp - 0x4c]
            mov dword ptr [ecx + 0x10], eax
            jmp L0056739b
    L0056738f:
            mov edx, dword ptr [ebp - 0x4c]
            mov eax, dword ptr [ebp - 0x4c]
            mov ecx, dword ptr [eax + 0x54]
            mov dword ptr [edx + 0x10], ecx
    L0056739b:
            fld dword ptr [ebp - 0x10]
            fdiv dword ptr [rendererStateStoredDispatch+0xc70]
            fadd dword ptr [rendererStateStoredDispatch+0xc74]
            fdivr dword ptr [rendererStateStoredDispatch+0xc74]
            fstp dword ptr [ebp - 0x38]
            cmp dword ptr [gS3_inside_cockpit], 0
            jne L005673c8
            fld dword ptr [ebp - 0x38]
            fmul dword ptr [rendererStateStoredDispatch+0xc78]
            fstp dword ptr [ebp - 0x38]
    L005673c8:
            mov edx, dword ptr [ebp - 0x4c]
            fld dword ptr [edx + 0x2c]
            fsub dword ptr [gS3_listener_position_now]
            fstp dword ptr [ebp - 0x30]
            mov eax, dword ptr [ebp - 0x4c]
            fld dword ptr [eax + 0x30]
            fsub dword ptr [gS3_listener_position_now+0x4]
            fstp dword ptr [ebp - 0x2c]
            mov ecx, dword ptr [ebp - 0x4c]
            fld dword ptr [ecx + 0x34]
            fsub dword ptr [gS3_listener_position_now+0x8]
            fstp dword ptr [ebp - 0x28]
            mov edx, dword ptr [gS3_listener_left_now]
            mov dword ptr [ebp - 0x24], edx
            mov eax, dword ptr [gS3_listener_left_now+0x4]
            mov dword ptr [ebp - 0x20], eax
            mov ecx, dword ptr [gS3_listener_left_now+0x8]
            mov dword ptr [ebp - 0x1c], ecx
    L0056740f:
            fld dword ptr [ebp - 0x24]
            fmul dword ptr [ebp - 0x24]
            fld dword ptr [ebp - 0x20]
            fmul dword ptr [ebp - 0x20]
            faddp st(1), st(0)
            fld dword ptr [ebp - 0x1c]
            fmul dword ptr [ebp - 0x1c]
            faddp st(1), st(0)
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            call sqrt
            add esp, 8
            fst dword ptr [ebp - 0x54]
            fst dword ptr [ebp - 0x44]
            fcomp dword ptr [rendererStateStoredDispatch+0xc7c]
            fnstsw ax
            test ah, 0x41
            jne L00567472
            fld dword ptr [ebp - 0x44]
            fdivr qword ptr [rendererStateStoredDispatch+0xc58]
            fst dword ptr [ebp - 0x58]
            fstp dword ptr [ebp - 0x44]
            fld dword ptr [ebp - 0x24]
            fmul dword ptr [ebp - 0x44]
            fstp dword ptr [ebp - 0x24]
            fld dword ptr [ebp - 0x20]
            fmul dword ptr [ebp - 0x44]
            fstp dword ptr [ebp - 0x20]
            fld dword ptr [ebp - 0x1c]
            fmul dword ptr [ebp - 0x44]
            fstp dword ptr [ebp - 0x1c]
            jmp L00567487
    L00567472:
            mov dword ptr [ebp - 0x24], 0x3f800000
            mov dword ptr [ebp - 0x20], 0
            mov dword ptr [ebp - 0x1c], 0
    L00567487:
            xor edx, edx
            test edx, edx
            jne L0056740f
    L0056748d:
            fld dword ptr [ebp - 0x30]
            fmul dword ptr [ebp - 0x30]
            fld dword ptr [ebp - 0x2c]
            fmul dword ptr [ebp - 0x2c]
            faddp st(1), st(0)
            fld dword ptr [ebp - 0x28]
            fmul dword ptr [ebp - 0x28]
            faddp st(1), st(0)
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            call sqrt
            add esp, 8
            fst dword ptr [ebp - 0x5c]
            fst dword ptr [ebp - 0x48]
            fcomp dword ptr [rendererStateStoredDispatch+0xc7c]
            fnstsw ax
            test ah, 0x41
            jne L005674f0
            fld dword ptr [ebp - 0x48]
            fdivr qword ptr [rendererStateStoredDispatch+0xc58]
            fst dword ptr [ebp - 0x60]
            fstp dword ptr [ebp - 0x48]
            fld dword ptr [ebp - 0x30]
            fmul dword ptr [ebp - 0x48]
            fstp dword ptr [ebp - 0x30]
            fld dword ptr [ebp - 0x2c]
            fmul dword ptr [ebp - 0x48]
            fstp dword ptr [ebp - 0x2c]
            fld dword ptr [ebp - 0x28]
            fmul dword ptr [ebp - 0x48]
            fstp dword ptr [ebp - 0x28]
            jmp L00567505
    L005674f0:
            mov dword ptr [ebp - 0x30], 0x3f800000
            mov dword ptr [ebp - 0x2c], 0
            mov dword ptr [ebp - 0x28], 0
    L00567505:
            xor eax, eax
            test eax, eax
            jne L0056748d
            fld dword ptr [ebp - 0x24]
            fmul dword ptr [ebp - 0x30]
            fld dword ptr [ebp - 0x20]
            fmul dword ptr [ebp - 0x2c]
            faddp st(1), st(0)
            fld dword ptr [ebp - 0x1c]
            fmul dword ptr [ebp - 0x28]
            faddp st(1), st(0)
            fchs
            fstp dword ptr [ebp - 0x34]
            mov ecx, dword ptr [ebp - 0x4c]
            mov edx, dword ptr [ebp - 0x34]
            mov dword ptr [ecx + 0x28], edx
            mov eax, dword ptr [ebp - 0x4c]
            fild dword ptr [eax + 0x50]
            fmul dword ptr [ebp - 0x38]
            call _ftol
            mov ecx, dword ptr [ebp - 0x4c]
            mov dword ptr [ecx + 0x24], eax
            mov edx, dword ptr [ebp - 0x4c]
            cmp dword ptr [edx + 0x24], 0
            jge L00567556
            mov eax, dword ptr [ebp - 0x4c]
            mov dword ptr [eax + 0x24], 0
    L00567556:
            mov ecx, dword ptr [ebp - 0x4c]
            cmp dword ptr [ecx + 0x24], 0xff
            jle L0056756c
            mov edx, dword ptr [ebp - 0x4c]
            mov dword ptr [edx + 0x24], 0xff
    L0056756c:
            mov eax, 1
    L00567571:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00567575
int C2_HOOK_FASTCALL S3Service3D(tS3_channel* pChannel) {
    int updated;

    updated = S3Calculate3D(pChannel, pChannel->sound_source_ptr != NULL && !!pChannel->sound_source_ptr->ambient);
    if (updated) {
        PDS3UpdateChannelVolume(pChannel);
        PDS3UpdateChannelFrequency(pChannel);
    }
    return updated;
}

// FUNCTION: CARMA2_HW 0x00565ea0
void C2_HOOK_FASTCALL S3SetVector3(tS3_vector3* pVec, float pX, float pY, float pZ) {
    pVec->x = pX;
    pVec->y = pY;
    pVec->z = pZ;
}

// FUNCTION: CARMA2_HW 0x00565ef9
void C2_HOOK_FASTCALL S3SetListenerPositionNow(br_vector3* pPos) {
    gS3_listener_position_now.v[0] = pPos->v[0];
    gS3_listener_position_now.v[1] = pPos->v[1];
    gS3_listener_position_now.v[2] = pPos->v[2];
}

// FUNCTION: CARMA2_HW 0x00565f26
void C2_HOOK_FASTCALL FUN_00565f26(br_vector3* pVec) {
    gS3_listener_vel_now.v[0] = pVec->v[0];
    gS3_listener_vel_now.v[1] = pVec->v[1];
    gS3_listener_vel_now.v[2] = pVec->v[2];
}

// FUNCTION: CARMA2_HW 0x00565f53
void C2_HOOK_FASTCALL S3SetListenerLeftNow(br_vector3* pLeft) {
    gS3_listener_left_now.v[0] = pLeft->v[0];
    gS3_listener_left_now.v[1] = pLeft->v[1];
    gS3_listener_left_now.v[2] = pLeft->v[2];
}

// FUNCTION: CARMA2_HW 0x00565f80
void C2_HOOK_FASTCALL S3BindListenerPosition(tS3_vector3* pos) {
    gS3_listener_pos_ptr = (br_vector3*)pos;
    gS3_listener_pos_is_brender = 0;
    S3CopyS3Vector3((tS3_vector3*)&gS3_listener_position_old, pos);
}

// FUNCTION: CARMA2_HW 0x00565fd4
void C2_HOOK_FASTCALL S3BindListenerVelocity(br_vector3* vel) {
    gS3_listener_vel_ptr = vel;
    gS3_listener_vel_is_brender = 0;
}

// FUNCTION: CARMA2_HW 0x0056600e
void C2_HOOK_FASTCALL S3BindListenerLeft(br_vector3* left) {
    gS3_listener_left_ptr = left;
    gS3_listener_left_is_brender = 0;
}
