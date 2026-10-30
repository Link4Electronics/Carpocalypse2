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
extern OSErr (C2_HOOK_CDECL *gQuickTime_dispatcher)();
extern unsigned int (C2_HOOK_CDECL *gQuickTime_dispatch)();

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



// FUNCTION: CARMA2_HW 0x004e1700
void C2_HOOK_CDECL InitQuickTimeStuff(void) {

    gQuickTime_initialized = 0;
    if (InitializeQTML(0) == noErr && EnterMovies() == noErr) {
        gQuickTime_initialized = 1;
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e1740
void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (gQuick_time_quality[0] == '\0') {
        strcpy(gQuick_time_quality, gDefault_quick_time_quality);
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, gDefault_quick_time_compressor);
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

// C2PORT-NONMATCHING: best 88.54% | difftest PASS 2000 trials (spec difftest/specs/004e1810_MovieRecordButtonPressed.json) | reason vc5-loop-layout
// FUNCTION: CARMA2_HW 0x004e1810
void C2_HOOK_FASTCALL MovieRecordButtonPressed(void) {
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
}

// C2PORT-NONMATCHING: best 16.67% | difftest PASS 2000 trials (spec difftest/specs/0056a940_FUN_0056a940.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056a940
void C2_HOOK_STDCALL FUN_0056a940(Movie pMovie) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    DisposeMovie(pMovie);
#else
#endif
}

// C2PORT-NONMATCHING: best 18.18% | difftest PASS 2000 trials (spec difftest/specs/0056abe0_CloseMovieFile.json) | reason QuickTime dispatcher tail-call codegen
// FUNCTION: CARMA2_HW 0x0056abe0
OSErr C2_HOOK_CDECL FUN_0056abe0(short pResource_file) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return CloseMovieFile(pResource_file);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// C2PORT-NONMATCHING: best 16.67% | difftest PASS 2000 trials (spec difftest/specs/0056b240_FUN_0056b240.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b240
TimeValue C2_HOOK_STDCALL FUN_0056b240(Media pMedia) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return GetMediaDuration(pMedia);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056b260_FUN_0056b260.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b260
unsigned int C2_HOOK_CDECL FUN_0056b260(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x2006b);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 18.18% | difftest PASS 2000 trials (spec difftest/specs/0056b360_FUN_0056b360.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b360
OSErr C2_HOOK_CDECL FUN_0056b360(Media pMedia) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return EndMediaEdits(pMedia);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056b380_FUN_0056b380.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b380
unsigned int C2_HOOK_CDECL FUN_0056b380(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20074);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056b460_FUN_0056b460.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056b460
unsigned int C2_HOOK_CDECL FUN_0056b460(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x2007d);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 18.18% | difftest PASS 2000 trials (spec 0056baa0_FUN_0056baa0.json) | reason QuickTime trap dispatch modeled by stubs
// FUNCTION: CARMA2_HW 0x0056baa0
OSErr C2_HOOK_CDECL FUN_0056baa0(short pResource_file) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return CloseMovieFile(pResource_file);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// C2PORT-NONMATCHING: best 25.00% | difftest PASS 2000 trials (spec difftest/specs/0056bac0_FUN_0056bac0.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056bac0
unsigned int C2_HOOK_CDECL FUN_0056bac0(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x200d7);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 10.00% | difftest PASS 2000 trials (spec difftest/specs/0056c600_FUN_0056c600.json) | reason QuickTime trap dispatch ABI
// FUNCTION: CARMA2_HW 0x0056c600
OSErr C2_HOOK_CDECL FUN_0056c600(Track pTrack, TimeValue pTrack_start, TimeValue pMedia_time, TimeValue pMedia_duration, Fixed pMedia_rate) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return InsertMediaIntoTrack(pTrack, pTrack_start, pMedia_time, pMedia_duration, pMedia_rate);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// C2PORT-NONMATCHING: best 14.29% | difftest PASS 2000 trials (spec difftest/specs/0056c620_FUN_0056c620.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c620
unsigned int C2_HOOK_CDECL FUN_0056c620(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x20187);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 7.69% | difftest PASS 2000 trials (spec difftest/specs/0056c800_FUN_0056c800.json) | reason QuickTime tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056c800
Movie C2_HOOK_CDECL FUN_0056c800(Movie pMovie, long pFlatten_flags, const FSSpec* pFile, OSType pCreator, ScriptCode pScript, long pCreate_flags) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((Movie (C2_HOOK_CDECL *)(unsigned int, Movie, long, const FSSpec*, OSType, ScriptCode, long))gQuickTime_dispatch)(
        0x2019c, pMovie, pFlatten_flags, pFile, pCreator, pScript, pCreate_flags);
#else
    return NULL;
#endif
}

// C2PORT-NONMATCHING: best 14.29% | difftest PASS 2000 trials (spec difftest/specs/0056d800_FUN_0056d800.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056d800
unsigned int C2_HOOK_CDECL FUN_0056d800(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x30005);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 12.50% | difftest PASS 2000 trials (spec difftest/specs/0056dac0_FUN_0056dac0.json) | reason QuickTime trap tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x0056dac0
OSErr C2_HOOK_CDECL FUN_0056dac0(ImageSequence pSequence) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((OSErr (C2_HOOK_CDECL *)(unsigned int, ImageSequence))gQuickTime_dispatch)(0x3001b, pSequence);
#else
    return 1; /* QuickTime recording is unsupported. */
#endif
}

// C2PORT-NONMATCHING: best 12.50% | difftest PASS 2000 trials (spec difftest/specs/00570ac0_DisposeHandle.json) | reason QuickTime EAX selector tail-dispatch
// FUNCTION: CARMA2_HW 0x00570ac0
void C2_HOOK_CDECL FUN_00570ac0(Handle pHandle) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    ((void (C2_HOOK_CDECL *)(unsigned int, Handle))gQuickTime_dispatch)(0x150032, pHandle);
#else
#endif
}

// C2PORT-NONMATCHING: best 14.29% | difftest PASS 2000 trials (spec difftest/specs/00575340_FUN_00575340.json) | reason QuickTime EAX selector tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x00575340
unsigned int C2_HOOK_CDECL FUN_00575340(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x280017);
#else
    return 0;
#endif
}

// C2PORT-NONMATCHING: best 14.29% | difftest PASS 2000 trials (spec difftest/specs/00575360_FUN_00575360.json) | reason QuickTime EAX selector tail-dispatch codegen
// FUNCTION: CARMA2_HW 0x00575360
unsigned int C2_HOOK_CDECL FUN_00575360(void) {
#ifdef _WIN32
    if (gQuickTime_dispatcher != NULL) {
        gQuickTime_dispatcher();
    }
    return ((unsigned int (C2_HOOK_CDECL *)(unsigned int))gQuickTime_dispatch)(0x280018);
#else
    return 0;
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
