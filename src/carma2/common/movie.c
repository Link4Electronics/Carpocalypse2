#include "movie.h"

#include "utility.h"
#include "globvars.h"

#include "globvrpb.h"

#include "sound.h"

#include "video.h"

#include "graphics.h"

#include "platform.h"

#include <QTML.h>
#include <Movies.h>

#include <FileTypesAndCreators.h>

#include <GXMath.h>

#include <Script.h>

#include <TextUtils.h>

#include "c2_string.h"

extern Movie gQuicktime_movie;
extern ImageSequence gQuicktime_image_sequence;
extern Media gQuicktime_media;
extern Track gQuicktime_track;
extern int gQuicktime_movie_counter;
extern Handle gQuicktime_frame_pixmap_handle;
extern Handle gQuicktime_image_description_handle;
extern Handle gQuicktime_compressed_frame_handle;
extern void (C2_HOOK_CDECL *gQuickTime_trap_callback)();
extern void (C2_HOOK_CDECL *gQuickTime_trap_dispatcher)();

// GLOBAL: CARMA2_HW 0x006a0c24
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x00705440
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
char gQuick_time_banner_texture_name[256];

// GLOBAL: CARMA2_HW 0x0065ef28
int gQuickTime_quality_code = -1;

// GLOBAL: CARMA2_HW 0x0065ed30
const tQuickTime_name_code gQuickTime_compressor_options[8] = {
    { "animation", 0x726c6520 },
    { "video", 0x72707a61 },
    { "jpeg", 0x6a706567 },
    { "raw", 0x72617720 },
    { "flic", 0x666c6963 },
    { "gif", 0x67696620 },
    { "cinepak", 0x63766964 },
    { "compactvideo", 0x63647663 },
};

// GLOBAL: CARMA2_HW 0x0065ee50
const tQuickTime_name_code gQuickTime_quality_options[6] = {
    { "normal", 0x200 },
    { "min", 0x0 },
    { "low", 0x100 },
    { "high", 0x300 },
    { "max", 0x3ff },
    { "lossless", 0x400 },
};

// GLOBAL: CARMA2_HW 0x006a0c4c
int gQuickTime_compressor_code;

char gDefault_quick_time_quality[] = "normal";

char gDefault_quick_time_compressor[] = "animation";

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

// WriteBannerFrame

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

// CreateMovie

// MovieStartNewMovie

// MovieStopRecordingAndSave

// FreeOffQTshite

int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void) {
    // Implementation should be in video.c
    return 1;
}



#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern br_uint_8 bl;
extern tS3_outlet* gEffects_outlet;
extern undefined4 gNext_replay_record_state;
extern struct FSSpec gQt_file_spec;
extern char gQuick_time_compressor[32];
extern char gQuick_time_quality[32];
extern short gQuicktime_movie_file;
extern int gRecording_paused;
extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern unsigned int C2_HOOK_CDECL FUN_00575340();
#endif
// FUNCTION: CARMA2_HW 0x004e1700
void C2_HOOK_CDECL InitQuickTimeStuff(void) {
    OSErr err;

    gQuickTime_initialized = 0;
    err = InitializeQTML(0);
    if (err == noErr) {
        err = EnterMovies();
        if (err == noErr) {
            gQuickTime_initialized = 1;
        }
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e1740
void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (gQuick_time_quality[0] == '\0') {
        strcpy(gQuick_time_quality, gQuickTime_quality_options[0].name);
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, gQuickTime_compressor_options[0].name);
    }
    if (gQuick_time_temp_path[0] == '\0') {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    }
    if (gQuick_time_movie_path_stub[0] == '\0') {
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    }
    if (gQuick_time_banner_texture_name[0] == '\0') {
        strcpy(gQuick_time_banner_texture_name, "ARBANNER.PIX");
    }
}

// FUNCTION: CARMA2_HW 0x004e1810
void C2_NAKED C2_HOOK_FASTCALL MovieRecordButtonPressed(void) {
#ifndef CARPOCALYPSE2_MATCHING
    const tQuickTime_name_code* pOption;
    int i;

    if (gQuickTime_initialized != 0) {
        if (gNext_replay_record_state == 4) {
            gRecording_paused = gRecording_paused == 0;
            if (gRecording_paused != 0) {
                DRS3StartSound(gEffects_outlet, 0x232b);
                DRS3StartSound(gEffects_outlet, 0x157c);
                gShow_status_message = 1;
                gStatus_message_1 = 0x119;
                gStatus_message_2 = 0;
                return;
            }
            DRS3StartSound(gEffects_outlet, 0x2333);
            gShow_status_message = 1;
            gStatus_message_1 = 0x117;
            gStatus_message_2 = 0;
            return;
        }
        if (gNext_replay_record_state == 0) {
            gStatus_message_1 = 0x116;
            gNext_replay_record_state = 1;
            gShow_status_message = 1;
            gStatus_message_2 = 0x118;
            gQuicktime_movie_file = 0;
            gRecording_paused = 0;
            if (gQuickTime_quality_code == -1) {
                for (i = 0, pOption = gQuickTime_quality_options;
                    pOption < gQuickTime_quality_options + sizeof(gQuickTime_quality_options) / sizeof(gQuickTime_quality_options[0]);
                    i++, pOption++) {
                    if (strcmp(pOption->name, gQuick_time_quality) == 0) {
                        goto quality_found;
                    }
                }
                gQuickTime_quality_code = 0x100;
                goto quality_continue;
            }
quality_continue:
            if (gQuickTime_compressor_code == 0) {
                for (i = 0, pOption = gQuickTime_compressor_options;
                    pOption < gQuickTime_compressor_options + sizeof(gQuickTime_compressor_options) / sizeof(gQuickTime_compressor_options[0]);
                    i++, pOption++) {
                    if (strcmp(pOption->name, gQuick_time_compressor) == 0) {
                        goto compressor_found;
                    }
                }
                gQuickTime_compressor_code = 0x726c6520;
                goto compressor_done;
compressor_found:
                gQuickTime_compressor_code = gQuickTime_compressor_options[i].code;
compressor_done:
                ;
            }
            gQt_file_spec.vRefNum = 0;
            gQt_file_spec.parID = 0;
            strcpy((char*)gQt_file_spec.name, gQuick_time_temp_path);
            c2pstr((char*)gQt_file_spec.name);
        }
    }
    return;
quality_found:
    gQuickTime_quality_code = gQuickTime_quality_options[i].code;
    goto quality_continue;
#else
    __asm {
            mov eax, dword ptr [gQuickTime_initialized]
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            cmp eax, ebx
            push edi
            je L004e19f9
            mov eax, dword ptr [gNext_replay_record_state]
            cmp eax, 4
            jne L004e18b5
            mov edx, dword ptr [gRecording_paused]
            xor eax, eax
            cmp edx, ebx
            sete al
            cmp eax, ebx
            mov dword ptr [gRecording_paused], eax
            je L004e1886
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0x232b
            call DRS3StartSound
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0x157c
            call DRS3StartSound
            mov dword ptr [gShow_status_message], 1
            mov dword ptr [gStatus_message_1], 0x119
            mov dword ptr [gStatus_message_2], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L004e1886:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0x2333
            call DRS3StartSound
            mov dword ptr [gShow_status_message], 1
            mov dword ptr [gStatus_message_1], 0x117
            mov dword ptr [gStatus_message_2], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L004e18b5:
            cmp eax, ebx
            jne L004e19f9
            mov eax, 1
            mov dword ptr [gStatus_message_1], 0x116
            mov dword ptr [gNext_replay_record_state], eax
            mov dword ptr [gShow_status_message], eax
            mov eax, dword ptr [gQuickTime_quality_code]
            mov dword ptr [gStatus_message_2], 0x118
            cmp eax, -1
            mov word ptr [gQuicktime_movie_file], bx
            mov dword ptr [gRecording_paused], ebx
            jne L004e194c
            xor ebp, ebp
            mov edi, offset gQuickTime_quality_options
    L004e18fe:
            mov esi, offset gQuick_time_quality
            mov eax, edi
    L004e1905:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L004e1929
            cmp cl, bl
            je L004e1925
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L004e1929
            add eax, 2
            add esi, 2
            cmp cl, bl
            jne L004e1905
    L004e1925:
            xor eax, eax
            jmp L004e192e
    L004e1929:
            sbb eax, eax
            sbb eax, -1
    L004e192e:
            cmp eax, ebx
            je L004e19fe
            add edi, 0x24
            inc ebp
            cmp edi, offset gQuickTime_quality_code
            jb L004e18fe
            mov dword ptr [gQuickTime_quality_code], 0x100
    L004e194c:
            cmp dword ptr [gQuickTime_compressor_code], ebx
            jne L004e19ba
            xor ebp, ebp
            mov edi, offset gQuickTime_compressor_options
    L004e195b:
            mov esi, offset gQuick_time_compressor
            mov eax, edi
    L004e1962:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L004e1986
            cmp cl, bl
            je L004e1982
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L004e1986
            add eax, 2
            add esi, 2
            cmp cl, bl
            jne L004e1962
    L004e1982:
            xor eax, eax
            jmp L004e198b
    L004e1986:
            sbb eax, eax
            sbb eax, -1
    L004e198b:
            cmp eax, ebx
            je L004e199d
            add edi, 0x24
            inc ebp
            cmp edi, offset gQuickTime_quality_options
            jb L004e195b
            jmp L004e19b0
    L004e199d:
            lea eax, [ebp + ebp*8]
            mov ecx, dword ptr [eax*4 + gQuickTime_compressor_options+0x20]
            mov dword ptr [gQuickTime_compressor_code], ecx
            jmp L004e19ba
    L004e19b0:
            mov dword ptr [gQuickTime_compressor_code], 0x726c6520
    L004e19ba:
            mov edi, offset gQuick_time_temp_path
            or ecx, 0xffffffff
            xor eax, eax
            mov word ptr [gQt_file_spec], bx
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gQt_file_spec+0x2], ebx
            mov edx, ecx
            mov esi, edi
            mov edi, offset gQt_file_spec+0x6
            push offset gQt_file_spec+0x6
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            call FUN_00575340
            add esp, 4
    L004e19f9:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L004e19fe:
            lea eax, [ebp + ebp*8]
            mov ecx, dword ptr [eax*4 + gQuickTime_quality_options+0x20]
            mov dword ptr [gQuickTime_quality_code], ecx
            jmp L004e194c
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a940
void C2_NAKED C2_HOOK_STDCALL FUN_0056a940(Movie pMovie) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    DisposeMovie(pMovie);
#else
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056a94f
            call dword ptr [gQuickTime_trap_callback]
    L0056a94f:
            mov eax, 0x20004
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056abe0
OSErr C2_HOOK_CDECL FUN_0056abe0(short pResource_file) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return CloseMovieFile(pResource_file);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// FUNCTION: CARMA2_HW 0x0056b240
TimeValue C2_HOOK_STDCALL FUN_0056b240(Media pMedia) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return GetMediaDuration(pMedia);
#else
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x0056b260
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056b260(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x2006b);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056b26f
            call dword ptr [gQuickTime_trap_callback]
    L0056b26f:
            mov eax, 0x2006b
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056b360
OSErr C2_HOOK_CDECL FUN_0056b360(Media pMedia) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return EndMediaEdits(pMedia);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// FUNCTION: CARMA2_HW 0x0056b380
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056b380(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x20074);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056b38f
            call dword ptr [gQuickTime_trap_callback]
    L0056b38f:
            mov eax, 0x20074
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056b460
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056b460(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x2007d);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056b46f
            call dword ptr [gQuickTime_trap_callback]
    L0056b46f:
            mov eax, 0x2007d
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056baa0
OSErr C2_HOOK_CDECL FUN_0056baa0(short pResource_file) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return CloseMovieFile(pResource_file);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// FUNCTION: CARMA2_HW 0x0056bac0
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056bac0(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x200d7);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056bacf
            call dword ptr [gQuickTime_trap_callback]
    L0056bacf:
            mov eax, 0x200d7
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c600
OSErr C2_HOOK_CDECL FUN_0056c600(Track pTrack, TimeValue pTrack_start, TimeValue pMedia_time, TimeValue pMedia_duration, Fixed pMedia_rate) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return InsertMediaIntoTrack(pTrack, pTrack_start, pMedia_time, pMedia_duration, pMedia_rate);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// FUNCTION: CARMA2_HW 0x0056c620
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056c620(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x20187);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056c62f
            call dword ptr [gQuickTime_trap_callback]
    L0056c62f:
            mov eax, 0x20187
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056c800
Movie C2_HOOK_CDECL FUN_0056c800(Movie pMovie, long pFlatten_flags, const FSSpec* pFile, OSType pCreator, ScriptCode pScript, long pCreate_flags) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((Movie (C2_HOOK_CDECL *)(unsigned int, Movie, long, const FSSpec*, OSType, ScriptCode, long))gQuickTime_trap_dispatcher)(
        0x2019c, pMovie, pFlatten_flags, pFile, pCreator, pScript, pCreate_flags);
#else
    return NULL;
#endif
}

// FUNCTION: CARMA2_HW 0x0056d800
unsigned int C2_NAKED C2_HOOK_CDECL FUN_0056d800(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x30005);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0056d80f
            call dword ptr [gQuickTime_trap_callback]
    L0056d80f:
            mov eax, 0x30005
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056dac0
OSErr C2_HOOK_CDECL FUN_0056dac0(ImageSequence pSequence) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((OSErr (C2_HOOK_CDECL *)(unsigned int, ImageSequence))gQuickTime_trap_dispatcher)(0x3001b, pSequence);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// FUNCTION: CARMA2_HW 0x00570ac0
void C2_HOOK_CDECL FUN_00570ac0(Handle pHandle) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int, Handle))gQuickTime_trap_dispatcher)(0x150032, pHandle);
#else
#endif
}

// FUNCTION: CARMA2_HW 0x00575340
unsigned int C2_HOOK_CDECL FUN_00575340(void) {
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x280017);
#else
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x00575360
unsigned int C2_NAKED C2_HOOK_CDECL FUN_00575360(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef _WIN32
    if (gQuickTime_trap_callback != NULL) {
        gQuickTime_trap_callback();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_trap_dispatcher)(0x280018);
#else
    return 0;
#endif
#else
    __asm {
            mov eax, dword ptr [gQuickTime_trap_callback]
            or eax, eax
            je L0057536f
            call dword ptr [gQuickTime_trap_callback]
    L0057536f:
            mov eax, 0x280018
            jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#endif
}

// MovieRecordButtonPressed

// FUNCTION: CARMA2_HW 0x004e1a20
int C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {
    MovieStopRecordingAndSave();
    KillStatusMessage();
    return 1;
}

// GenerateOneMovieFrame

// MovieRecordFrame

// MovieRecording

// KillStatusMessage

// ShowStatusMessage

// GetMovieWidth

// GetMovieHeight

// TidyMovieARDisplay

// TrapS3SoundOutput

// FreeS3SoundOutput

// FUNCTION: CARMA2_HW 0x004e1c30
void C2_HOOK_FASTCALL NoOp_004e1c30(void) {
}

// FUNCTION: CARMA2_HW 0x004e1c40
void C2_HOOK_FASTCALL NoOp_004e1c40(void) {
}

// FUNCTION: CARMA2_HW 0x004e1c50
void C2_HOOK_FASTCALL NoOp_004e1c50(void) {
}

// FUNCTION: CARMA2_HW 0x004e1c60
void C2_HOOK_FASTCALL NoOp_004e1c60(undefined4 pArg1, undefined4 pArg2, undefined4 pArg3) {
}
