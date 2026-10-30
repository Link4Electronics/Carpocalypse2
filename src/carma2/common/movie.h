#ifndef GUARD_MOVIE_H
#define GUARD_MOVIE_H

#include "c2_hooks.h"

typedef struct tQuickTime_name_code {
    char name[32];
    unsigned int code;
} tQuickTime_name_code;

extern const tQuickTime_name_code gQuickTime_compressor_options[8];
extern const tQuickTime_name_code gQuickTime_quality_options[6];
extern int gQuickTime_compressor_code;
extern int gQuickTime_quality_code;

extern char gQuick_time_temp_path[256];
extern char gQuick_time_movie_path_stub[256];
extern char gQuick_time_banner_texture_name[256];

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

extern void C2_HOOK_CDECL InitQuickTimeStuff(void);
extern int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void);



extern void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

void C2_HOOK_FASTCALL MovieRecordButtonPressed(void);

int C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

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

void C2_HOOK_FASTCALL NoOp_004e1c30(void);

#endif // GUARD_MOVIE_H
