#ifndef CARPOCALYPSE2_VIDEO_H
#define CARPOCALYPSE2_VIDEO_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

#include <Movies.h>

extern int gRecording_paused;
extern tU32 gRecording_time;
extern int gRecording_mouse_disabled;
extern br_uint_16* gQuicktime_frame_pixels;
extern br_uint_16* gQuicktime_frame_pixel_conversion_table;
extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;
extern undefined4 gNext_replay_record_state;
extern short gQuicktime_movie_file;
struct FSSpec;
extern struct FSSpec gQt_file_spec;

undefined4 C2_HOOK_FASTCALL FUN_004e2100(void);

void C2_HOOK_CDECL InitQuickTimeStuff(void);

void C2_HOOK_FASTCALL KillStatusMessage(void);

void C2_HOOK_FASTCALL WriteBannerFrame(void);

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2);

void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

void C2_HOOK_FASTCALL FreeOffQTshite(void);

int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void);

int C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

int C2_HOOK_FASTCALL GetMovieWidth(void);

int C2_HOOK_FASTCALL GetMovieHeight(void);

short C2_HOOK_FASTCALL InitMovie(void);

int C2_HOOK_FASTCALL CreateMovie(void);

void* C2_HOOK_CDECL FUN_0056fde0(int pArg1, int pArg2);
Track C2_HOOK_CDECL FUN_0056c640(Movie pMovie, Fixed pWidth, Fixed pHeight, short pVolume);
unsigned int C2_HOOK_CDECL FUN_0056a900(void);
unsigned int C2_HOOK_CDECL FUN_0056ac00(void);
int C2_HOOK_CDECL FUN_0056a920(void);
Media C2_HOOK_CDECL FUN_0056c6e0(Track pTrack, int pMedia_type, int pTime_scale, int pArg4, int pArg5);
int C2_HOOK_CDECL FUN_0056b340(Media pMedia);
Handle C2_HOOK_CDECL FUN_00570600(int pSize);
int C2_HOOK_CDECL FUN_005705c0(void);
int C2_HOOK_CDECL FUN_0056d7e0(Handle pPixmap, struct Rect* pRect, int pArg3, int pQuality, int pCodec_type, int pArg6, int* pMax_size);
void C2_HOOK_CDECL FUN_00570a40(Handle pHandle);
void C2_HOOK_CDECL FUN_005707e0(Handle pHandle);
int C2_HOOK_CDECL FUN_0056d8a0(int* pImage_sequence, Handle pPixmap, int pArg3, struct Rect* pSource_rect, struct Rect* pDest_rect, int pArg6, int pCodec_type, int pArg8, int pSpatial_quality, int pTemporal_quality, int pArg11, int pArg12, int pArg13, Handle pImage_description);

void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen);

short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);

short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);

short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);

void C2_HOOK_FASTCALL TrapS3SoundOutput(void);

void C2_HOOK_FASTCALL FreeS3SoundOutput(void);

int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap *pScreen, int pFrame_period);

int C2_HOOK_FASTCALL GetNextReplayRecordState(void);

extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;
extern undefined4 gNext_replay_record_state;

#endif //CARPOCALYPSE2_VIDEO_H
