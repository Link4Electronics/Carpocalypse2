#include "../include/s3/internal/sample.h"

#include "../include/s3/internal/audio.h"
#include "resource.h"

#include "platform.h"

#include <brender/brender.h>
#include "carpocalypse2_types.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x006b2c88
int gS3_low_memory_mode;

// GLOBAL: CARMA2_HW 0x007a05a0
tS3_error_codes gS3_last_error;

// GLOBAL: CARMA2_HW 0x007a05c0
char gS3_sound_dirname[256];

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel);
extern int C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel);
extern tS3_state gS3_state;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);
extern void* C2_HOOK_FASTCALL S3BufferWav(const char* pPath, tS3_buffer_desc* pBuffer_desc);
extern tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id);
extern int C2_HOOK_FASTCALL S3GetFileSize(FILE *pF);
extern void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type);
extern void C2_HOOK_FASTCALL S3MemFree(void* p);
extern FILE* C2_HOOK_FASTCALL S3_low_memory_fopen(const char* pPath, const char* pMode);
extern int gS3_CDA_enabled;
extern int gS3_enabled;
extern char gS3_path_separator[2];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gPD_S3_direct_sound[];
#endif
// FUNCTION: CARMA2_HW 0x005687ff
tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id) {
    tS3_descriptor* descriptor;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, buffer_description, 0x40);

    if (!gS3_enabled || (descriptor = S3GetDescriptorByID(pSample_id)) == NULL) {
        return NULL;
    }
    return descriptor->buffer_description;
}

// FUNCTION: CARMA2_HW 0x00568830
tS3_error_codes C2_NAKED C2_HOOK_FASTCALL S3LoadSample(int pSample_id) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor* descriptor;
    char path[512];
    tS3_buffer_desc* buffer_description;

    if (gS3_enabled) {
        descriptor = S3GetDescriptorByID(pSample_id);
        if (descriptor == NULL) {
            return eS3_error_bad_id;
        }
        if (descriptor->type == 0) {
            if (descriptor->buffer_description != NULL) {
                return eS3_error_none;
            }
            path[0] = '\0';
            strcpy(path, descriptor->path);

            C2_HOOK_BUG_ON(sizeof(tS3_buffer_desc) != 0x18);

            buffer_description = S3MemAllocate(sizeof(tS3_buffer_desc), kMem_S3_sound_header);
            if (buffer_description == NULL) {
                return eS3_error_memory;
            }
            memset(buffer_description, 0, sizeof(*buffer_description));
            descriptor->pd_handle = S3BufferWav(path, buffer_description);
            if (descriptor->pd_handle == NULL) {
                BrFailure("Cound not load sample:%s", path);
                S3MemFree(buffer_description);
                return gS3_last_error;
            }
            descriptor->effects_enabled = 0;
            descriptor->buffer_description = buffer_description;
        }
    }
    return eS3_error_none;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x214
            mov dword ptr [ebp - 0x214], ecx
            cmp dword ptr [gS3_enabled], 0
            je L00568923
            mov ecx, dword ptr [ebp - 0x214]
            call S3GetDescriptorByID
            mov dword ptr [ebp - 0xc], eax
            cmp dword ptr [ebp - 0xc], 0
            jne L0056886a
            mov eax, 6
            jmp L00568925
    L0056886a:
            mov eax, dword ptr [ebp - 0xc]
            cmp dword ptr [eax + 4], 0
            jne L00568923
            mov ecx, dword ptr [ebp - 0xc]
            cmp dword ptr [ecx + 0x40], 0
            je L00568887
            xor eax, eax
            jmp L00568925
    L00568887:
            mov byte ptr [ebp - 0x20c], 0
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [edx + 0x44]
            push eax
            lea ecx, [ebp - 0x20c]
            push ecx
            call strcpy
            add esp, 8
            mov dl, 0xe0
            mov ecx, 0x18
            call S3MemAllocate
            mov dword ptr [ebp - 4], eax
            cmp dword ptr [ebp - 4], 0
            jne L005688c0
            mov eax, 3
            jmp L00568925
    L005688c0:
            push 0x18
            push 0
            mov edx, dword ptr [ebp - 4]
            push edx
            call memset
            add esp, 0xc
            mov edx, dword ptr [ebp - 4]
            lea ecx, [ebp - 0x20c]
            call S3BufferWav
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ecx + 0x48], eax
            mov edx, dword ptr [ebp - 0xc]
            cmp dword ptr [edx + 0x48], 0
            jne L00568910
            lea eax, [ebp - 0x20c]
            push eax
            push offset gS3_CDA_enabled+0x98
            call BrFailure
            add esp, 8
            mov ecx, dword ptr [ebp - 4]
            call S3MemFree
            mov eax, dword ptr [gS3_last_error]
            jmp L00568925
    L00568910:
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [ecx + 0x38], 0
            mov edx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [edx + 0x40], eax
    L00568923:
            xor eax, eax
    L00568925:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00565a3d
C2_NAKED tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id) {
#ifndef CARPOCALYPSE2_MATCHING
    tS3_descriptor *desc;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, sample_id, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, next, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, low_memory_alternative, 0x3c);

    for (desc = gS3_state.descriptors; desc != NULL; desc = desc->next) {
        if (desc->sample_id == pSample_id) {
            if (desc->low_memory_alternative >= 0) {
                return S3GetDescriptorByID(desc->low_memory_alternative);
            }
            return desc;
        }
    }
    return NULL;
#else
    __asm {
    L00565a3d:
            push ebp
            mov ebp, esp
            sub esp, 8
            mov dword ptr [ebp - 8], ecx
            mov eax, dword ptr [gS3_state+0x14]
            mov dword ptr [ebp - 4], eax
    L00565a4e:
            cmp dword ptr [ebp - 4], 0
            je L00565a84
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ecx]
            cmp edx, dword ptr [ebp - 8]
            jne L00565a79
            mov eax, dword ptr [ebp - 4]
            cmp dword ptr [eax + 0x3c], 0
            jl L00565a74
            mov ecx, dword ptr [ebp - 4]
            mov ecx, dword ptr [ecx + 0x3c]
            call L00565a3d
            jmp L00565a86
    L00565a74:
            mov eax, dword ptr [ebp - 4]
            jmp L00565a86
    L00565a79:
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx + 0x2c]
            mov dword ptr [ebp - 4], eax
            jmp L00565a4e
    L00565a84:
            xor eax, eax
    L00565a86:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00565b34
void C2_HOOK_CDECL s3_debug_disable_sound(void) {
}

// FUNCTION: CARMA2_HW 0x00565b39
void C2_HOOK_CDECL s3_debug_init(void) {
    tPath_name path;
    int result;

    (void) path;
    (void) result;
}

// FUNCTION: CARMA2_HW 0x00565b46
void C2_HOOK_CDECL s3_dprintf(const char* pFormat, ...) {
}

// FUNCTION: CARMA2_HW 0x00568ee0
int C2_NAKED C2_HOOK_FASTCALL S3CheckWavHeader(tS3_wav_file* pWav_buffer, tS3_wav_chunk_info_header** pWav_format_info_header, void** pSamples, int* pSample_size) {
#ifndef CARPOCALYPSE2_MATCHING
    void* chunk_start;
    void* file_end;

    if (pWav_format_info_header != NULL) {
        *pWav_format_info_header = NULL;
    }
    if (pSamples != NULL) {
        *pSamples = NULL;
    }
    if (pSample_size != NULL) {
        *pSample_size = 0;
    }
    if (strncmp(pWav_buffer->riff_header.magic, "RIFF", 4) != 0 || strncmp(pWav_buffer->wave_magic, "WAVE", 4) != 0) {
        return 0;
    }
    file_end = (br_uint_8*)pWav_buffer->wave_magic + pWav_buffer->riff_header.chunk_size;
    chunk_start = &pWav_buffer->fmt_chunk;
    while (chunk_start < file_end) {
        tS3_riff_chunk_header* chunk_header = chunk_start;
        if (strncmp(chunk_header->magic, "fmt ", 4) == 0) {
            tS3_riff_fmt_chunk* fmt_chunk = chunk_start;
            if (pWav_format_info_header != NULL && *pWav_format_info_header == NULL) {
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_wav_chunk_info_header, bits_per_sample, 0xe);
                if (chunk_header->chunk_size < offsetof(tS3_wav_chunk_info_header, bits_per_sample)) {
                    return 0;
                }
                *pWav_format_info_header = &fmt_chunk->wav_info_header;
                if ((pSamples == NULL || *pSamples != NULL) && (pSample_size == NULL || *pSample_size != 0)) {
                    return 1;
                }
            }
        } else if (strncmp(chunk_header->magic, "data", 4) == 0) {
            tS3_riff_data_chunk* data_chunk = chunk_start;
            if ((pSamples != NULL && *pSamples == NULL) || (pSample_size != NULL && *pSample_size == 0)) {
                if (pSamples != NULL) {
                    *pSamples = data_chunk->data;
                }
                if (pSample_size != NULL) {
                    *pSample_size = data_chunk->header.chunk_size;
                }
                if (pWav_format_info_header == NULL || *pWav_format_info_header != NULL) {
                    return 1;
                }
            }
        }
        chunk_start = (br_uint_8*)&chunk_header[1] + chunk_header->chunk_size;
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x20
            mov dword ptr [ebp - 0x1c], edx
            mov dword ptr [ebp - 0x18], ecx
            cmp dword ptr [ebp - 0x1c], 0
            je L00568efb
            mov eax, dword ptr [ebp - 0x1c]
            mov dword ptr [eax], 0
    L00568efb:
            cmp dword ptr [ebp + 8], 0
            je L00568f0a
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [ecx], 0
    L00568f0a:
            cmp dword ptr [ebp + 0xc], 0
            je L00568f19
            mov edx, dword ptr [ebp + 0xc]
            mov dword ptr [edx], 0
    L00568f19:
            mov eax, dword ptr [ebp - 0x18]
            mov dword ptr [ebp - 8], eax
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx]
            mov dword ptr [ebp - 0x10], edx
            mov eax, dword ptr [ebp - 8]
            add eax, 4
            mov dword ptr [ebp - 8], eax
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx]
            mov dword ptr [ebp - 0xc], edx
            mov eax, dword ptr [ebp - 8]
            add eax, 4
            mov dword ptr [ebp - 8], eax
            mov ecx, dword ptr [ebp - 8]
            mov edx, dword ptr [ecx]
            mov dword ptr [ebp - 4], edx
            mov eax, dword ptr [ebp - 8]
            add eax, 4
            mov dword ptr [ebp - 8], eax
            cmp dword ptr [ebp - 0x10], 0x46464952
            je L00568f62
            xor eax, eax
            jmp L00569076
    L00568f62:
            cmp dword ptr [ebp - 4], 0x45564157
            je L00568f72
            xor eax, eax
            jmp L00569076
    L00568f72:
            mov ecx, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ebp - 8]
            lea eax, [edx + ecx - 4]
            mov dword ptr [ebp - 0x14], eax
    L00568f7f:
            mov ecx, dword ptr [ebp - 8]
            cmp ecx, dword ptr [ebp - 0x14]
            jae L00569074
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx]
            mov dword ptr [ebp - 4], eax
            mov ecx, dword ptr [ebp - 8]
            add ecx, 4
            mov dword ptr [ebp - 8], ecx
            mov edx, dword ptr [ebp - 8]
            mov eax, dword ptr [edx]
            mov dword ptr [ebp - 0xc], eax
            mov ecx, dword ptr [ebp - 8]
            add ecx, 4
            mov dword ptr [ebp - 8], ecx
            mov edx, dword ptr [ebp - 4]
            mov dword ptr [ebp - 0x20], edx
            cmp dword ptr [ebp - 0x20], 0x20746d66
            je L00568fca
            cmp dword ptr [ebp - 0x20], 0x61746164
            je L00569012
            jmp L0056905f
    L00568fca:
            cmp dword ptr [ebp - 0x1c], 0
            je L00569010
            mov eax, dword ptr [ebp - 0x1c]
            cmp dword ptr [eax], 0
            jne L00569010
            cmp dword ptr [ebp - 0xc], 0xe
            jae L00568fe5
            xor eax, eax
            jmp L00569076
    L00568fe5:
            mov ecx, dword ptr [ebp - 0x1c]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx], edx
            cmp dword ptr [ebp + 8], 0
            je L00568ffb
            mov eax, dword ptr [ebp + 8]
            cmp dword ptr [eax], 0
            je L00569010
    L00568ffb:
            cmp dword ptr [ebp + 0xc], 0
            je L00569009
            mov ecx, dword ptr [ebp + 0xc]
            cmp dword ptr [ecx], 0
            je L00569010
    L00569009:
            mov eax, 1
            jmp L00569076
    L00569010:
            jmp L0056905f
    L00569012:
            cmp dword ptr [ebp + 8], 0
            je L00569020
            mov edx, dword ptr [ebp + 8]
            cmp dword ptr [edx], 0
            je L0056902e
    L00569020:
            cmp dword ptr [ebp + 0xc], 0
            je L0056905f
            mov eax, dword ptr [ebp + 0xc]
            cmp dword ptr [eax], 0
            jne L0056905f
    L0056902e:
            cmp dword ptr [ebp + 8], 0
            je L0056903c
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp - 8]
            mov dword ptr [ecx], edx
    L0056903c:
            cmp dword ptr [ebp + 0xc], 0
            je L0056904a
            mov eax, dword ptr [ebp + 0xc]
            mov ecx, dword ptr [ebp - 0xc]
            mov dword ptr [eax], ecx
    L0056904a:
            cmp dword ptr [ebp - 0x1c], 0
            je L00569058
            mov edx, dword ptr [ebp - 0x1c]
            cmp dword ptr [edx], 0
            je L0056905f
    L00569058:
            mov eax, 1
            jmp L00569076
    L0056905f:
            mov eax, dword ptr [ebp - 0xc]
            add eax, 1
            and al, 0xfe
            mov ecx, dword ptr [ebp - 8]
            add ecx, eax
            mov dword ptr [ebp - 8], ecx
            jmp L00568f7f
    L00569074:
            xor eax, eax
    L00569076:
            mov esp, ebp
            pop ebp
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056907c
C2_NAKED void* C2_HOOK_FASTCALL S3BufferWav(const char* pPath, tS3_buffer_desc* pBuffer_desc) {
#ifndef CARPOCALYPSE2_MATCHING
    char wav_path[512];
    FILE* f;
    int file_size;
    tS3_wav_info wav_info;
    tS3_wav_file* wav_buffer;
    void* pd_handle;

    if (gS3_low_memory_mode) {
        sprintf(wav_path, "DATA%sSOUND%s%s", gS3_path_separator, gS3_path_separator, pPath);
        f = S3_low_memory_fopen(wav_path, "rb");
    } else {
        if (gS3_sound_dirname[0] != '\0') {
            char filename[512];

            PDExtractFilename(filename, pPath);
            sprintf(wav_path, "%s\\%s", gS3_sound_dirname, filename);
            f = fopen(wav_path, "rb");
        } else {
            f = fopen(pPath, "rb");
        }
    }
    if (f == NULL) {
        gS3_last_error = eS3_error_readfile;
        return NULL;
    }
    file_size = S3GetFileSize(f);
    if (file_size == 0) {
        fclose(f);
        gS3_last_error = eS3_error_readfile;
        return NULL;
    }
    wav_buffer = S3MemAllocate(file_size, kMem_S3_Windows_95_load_WAV_file);
    if (wav_buffer == NULL) {
        fclose(f);
        gS3_last_error = eS3_error_memory;
        return NULL;
    }
    fread(wav_buffer, 1, file_size, f);
    fclose(f);

    C2_HOOK_BUG_ON(sizeof(tS3_wav_info) != 0xc);

    memset(&wav_info, 0, sizeof(tS3_wav_info));
    if (!S3CheckWavHeader(wav_buffer, &wav_info.wav_info_header, &wav_info.samples, &wav_info.sample_size)) {
        gS3_last_error = eS3_error_readfile;
        s3_dprintf("ERROR: .WAV file '%s'is crap", wav_path);
        return NULL;
    }
    pBuffer_desc->field_0x14 = 0;
    pBuffer_desc->field_0x10 = 0;
    pBuffer_desc->sample_size = wav_info.sample_size;
    pBuffer_desc->sample_rate = wav_info.wav_info_header->sample_rate;
    pBuffer_desc->bytes_per_second = wav_info.wav_info_header->avg_bytes_per_second;
    pBuffer_desc->count_channels = wav_info.wav_info_header->number_of_channels;
    pd_handle = PDS3CreateSoundBuffer(&wav_info, wav_buffer);
    if (pd_handle != NULL) {
        /* FIXME: unconditionally free wav buffer */
        S3MemFree(wav_buffer);
    }
    return pd_handle;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x444
            mov dword ptr [ebp - 0x444], edx
            mov dword ptr [ebp - 0x440], ecx
            mov dword ptr [ebp - 0x204], 0
            mov dword ptr [ebp - 0x438], 0
            mov dword ptr [ebp - 0x230], 0
            cmp dword ptr [gS3_low_memory_mode], 0
            je L005690f5
            mov eax, dword ptr [ebp - 0x440]
            push eax
            push offset gS3_path_separator
            push offset gS3_path_separator
            push offset gS3_CDA_enabled+0x12c
            lea ecx, [ebp - 0x200]
            push ecx
            call sprintf
            add esp, 0x14
            mov edx, offset gS3_CDA_enabled+0x140
            lea ecx, [ebp - 0x200]
            call S3_low_memory_fopen
            mov dword ptr [ebp - 0x43c], eax
            jmp L00569167
    L005690f5:
            movsx edx, byte ptr [gS3_sound_dirname]
            test edx, edx
            je L0056914d
            mov edx, dword ptr [ebp - 0x440]
            lea ecx, [ebp - 0x430]
            call PDExtractFilename
            lea eax, [ebp - 0x430]
            push eax
            push offset gS3_sound_dirname
            push offset gS3_CDA_enabled+0x144
            lea ecx, [ebp - 0x200]
            push ecx
            call sprintf
            add esp, 0x10
            push offset gS3_CDA_enabled+0x14c
            lea edx, [ebp - 0x200]
            push edx
            call fopen
            add esp, 8
            mov dword ptr [ebp - 0x43c], eax
            jmp L00569167
    L0056914d:
            push offset gS3_CDA_enabled+0x150
            mov eax, dword ptr [ebp - 0x440]
            push eax
            call fopen
            add esp, 8
            mov dword ptr [ebp - 0x43c], eax
    L00569167:
            cmp dword ptr [ebp - 0x43c], 0
            jne L00569181
            mov dword ptr [gS3_last_error], 4
            xor eax, eax
            jmp L005693d5
    L00569181:
            mov ecx, dword ptr [ebp - 0x43c]
            call S3GetFileSize
            mov dword ptr [ebp - 0x434], eax
            cmp dword ptr [ebp - 0x434], 0
            jne L005691bb
            mov ecx, dword ptr [ebp - 0x43c]
            push ecx
            call fclose
            add esp, 4
            mov dword ptr [gS3_last_error], 4
            xor eax, eax
            jmp L005693d5
    L005691bb:
            mov dl, 0xef
            mov ecx, dword ptr [ebp - 0x434]
            call S3MemAllocate
            mov dword ptr [ebp - 0x22c], eax
            cmp dword ptr [ebp - 0x22c], 0
            jne L005691f7
            mov edx, dword ptr [ebp - 0x43c]
            push edx
            call fclose
            add esp, 4
            mov dword ptr [gS3_last_error], 3
            xor eax, eax
            jmp L005693d5
    L005691f7:
            mov eax, dword ptr [ebp - 0x43c]
            push eax
            mov ecx, dword ptr [ebp - 0x434]
            push ecx
            push 1
            mov edx, dword ptr [ebp - 0x22c]
            push edx
            call fread
            add esp, 0x10
            mov eax, dword ptr [ebp - 0x43c]
            push eax
            call fclose
            add esp, 4
            push 0xc
            push 0
            lea ecx, [ebp - 0x210]
            push ecx
            call memset
            add esp, 0xc
            lea edx, [ebp - 0x210]
            push edx
            lea eax, [ebp - 0x208]
            push eax
            lea edx, [ebp - 0x20c]
            mov ecx, dword ptr [ebp - 0x22c]
            call S3CheckWavHeader
            test eax, eax
            jne L00569280
            mov dword ptr [gS3_last_error], 4
            lea ecx, [ebp - 0x200]
            push ecx
            push offset gS3_CDA_enabled+0x154
            call s3_dprintf
            add esp, 8
            xor eax, eax
            jmp L005693d5
    L00569280:
            mov edx, dword ptr [ebp - 0x444]
            mov dword ptr [edx + 0x14], 0
            mov eax, dword ptr [ebp - 0x444]
            mov dword ptr [eax + 0x10], 0
            mov ecx, dword ptr [ebp - 0x444]
            mov edx, dword ptr [ebp - 0x210]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [ebp - 0x444]
            mov ecx, dword ptr [ebp - 0x20c]
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + 4], edx
            mov eax, dword ptr [ebp - 0x444]
            mov ecx, dword ptr [ebp - 0x20c]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [eax + 8], edx
            mov eax, dword ptr [ebp - 0x20c]
            xor ecx, ecx
            mov cx, word ptr [eax + 2]
            mov edx, dword ptr [ebp - 0x444]
            mov dword ptr [edx + 0xc], ecx
            push 0x14
            push 0
            lea eax, [ebp - 0x224]
            push eax
            call memset
            add esp, 0xc
            mov dword ptr [ebp - 0x224], 0x14
            mov dword ptr [ebp - 0x220], 0x100e2
            mov ecx, dword ptr [ebp - 0x210]
            mov dword ptr [ebp - 0x21c], ecx
            mov edx, dword ptr [ebp - 0x20c]
            mov dword ptr [ebp - 0x214], edx
            push 0
            lea eax, [ebp - 0x228]
            push eax
            lea ecx, [ebp - 0x224]
            push ecx
            mov edx, dword ptr [gPD_S3_direct_sound]
            push edx
            mov eax, dword ptr [gPD_S3_direct_sound]
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0xc]
            test eax, eax
            jge L0056934c
            xor eax, eax
            jmp L005693d5
    L0056934c:
            push 0
            push 0
            push 0
            lea edx, [ebp - 0x230]
            push edx
            lea eax, [ebp - 0x204]
            push eax
            mov ecx, dword ptr [ebp - 0x210]
            push ecx
            push 0
            mov edx, dword ptr [ebp - 0x228]
            push edx
            mov eax, dword ptr [ebp - 0x228]
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x2c]
            test eax, eax
            jge L00569383
            xor eax, eax
            jmp L005693d5
    L00569383:
            mov edx, dword ptr [ebp - 0x230]
            push edx
            mov eax, dword ptr [ebp - 0x208]
            push eax
            mov ecx, dword ptr [ebp - 0x204]
            push ecx
            call memmove
            add esp, 0xc
            mov ecx, dword ptr [ebp - 0x22c]
            call S3MemFree
            push 0
            push 0
            mov edx, dword ptr [ebp - 0x230]
            push edx
            mov eax, dword ptr [ebp - 0x204]
            push eax
            mov ecx, dword ptr [ebp - 0x228]
            push ecx
            mov edx, dword ptr [ebp - 0x228]
            mov eax, dword ptr [edx]
            call dword ptr [eax + 0x4c]
            mov eax, dword ptr [ebp - 0x228]
    L005693d5:
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00564e5e
int C2_HOOK_FASTCALL FUN_00564e5e(int pSample_id) {
    struct {
        tS3_descriptor* descriptor;
        tS3_buffer_desc* buffer_description;
    } volatile locals;

    locals.descriptor = S3GetDescriptorByID(pSample_id);
    if (locals.descriptor == NULL) {
        return 0;
    }
    locals.buffer_description = locals.descriptor->buffer_description;
    return locals.buffer_description->sample_size * 1000 / locals.buffer_description->sample_rate;
}

// FUNCTION: CARMA2_HW 0x00569865
int C2_NAKED C2_HOOK_FASTCALL S3PlaySample(tS3_channel* pChannel, tS3_descriptor* pDescriptor) {
#ifndef CARPOCALYPSE2_MATCHING
    int unused;
    void* sound_buffer;
    int looping;

    (void)pDescriptor;
    unused = 0;
    looping = 0;
#if defined(CARPOCALYPSE2_PLATFORM_WIN32)
    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            if (pChannel->repetitions == 0) {
                looping = 1;
            }
            ((int (C2_HOOK_STDCALL *)(void*, unsigned int, int, unsigned int))(*(void***)sound_buffer)[12])(sound_buffer, 0, 0, looping);
            if (looping == 0) {
                ((int (C2_HOOK_STDCALL *)(void*, unsigned int))(*(void***)sound_buffer)[13])(sound_buffer, 0);
            }
        }
    }
#else
    PDS3PlaySample(pChannel);
#endif
    PDS3UpdateChannelVolume(pChannel);
    PDS3UpdateChannelFrequency(pChannel);
    (void)unused;
    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            mov dword ptr [ebp - 0x14], edx
            mov dword ptr [ebp - 0x10], ecx
            mov dword ptr [ebp - 8], 0
            mov dword ptr [ebp - 0xc], 0
            mov eax, dword ptr [ebp - 0x10]
            cmp dword ptr [eax + 0x60], 0
            je L005698e3
            mov ecx, dword ptr [ebp - 0x10]
            mov edx, dword ptr [ecx + 0x60]
            mov eax, dword ptr [ebp - 0x10]
            mov ecx, dword ptr [edx + 4]
            cmp ecx, dword ptr [eax + 0x20]
            jne L005698e3
            mov edx, dword ptr [ebp - 0x10]
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [eax + 0x48]
            mov dword ptr [ebp - 4], ecx
            cmp dword ptr [ebp - 4], 0
            je L005698e3
            mov edx, dword ptr [ebp - 0x10]
            cmp dword ptr [edx + 0xc], 0
            jne L005698bb
            mov dword ptr [ebp - 0xc], 1
    L005698bb:
            mov eax, dword ptr [ebp - 0xc]
            push eax
            push 0
            push 0
            mov ecx, dword ptr [ebp - 4]
            push ecx
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx]
            call dword ptr [eax + 0x30]
            cmp dword ptr [ebp - 0xc], 0
            jne L005698e3
            push 0
            mov ecx, dword ptr [ebp - 4]
            push ecx
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [edx]
            call dword ptr [eax + 0x34]
    L005698e3:
            mov ecx, dword ptr [ebp - 0x10]
            call PDS3UpdateChannelVolume
            mov ecx, dword ptr [ebp - 0x10]
            call PDS3UpdateChannelFrequency
            mov eax, 1
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}
