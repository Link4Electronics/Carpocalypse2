#ifndef CARPOCALYPSE2_VIDEO_H
#define CARPOCALYPSE2_VIDEO_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern int gRecording_paused;
extern tU32 gRecording_time;
extern int gRecording_mouse_disabled;

extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;
extern undefined4 gNext_replay_record_state;

int C2_HOOK_FASTCALL GetNextReplayRecordState(void);

void C2_HOOK_CDECL InitQuickTimeStuff(void);

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2);

void C2_HOOK_FASTCALL FreeOffQTshite(void);


int C2_HOOK_FASTCALL GetMovieWidth(void);

int C2_HOOK_FASTCALL GetMovieHeight(void);

short C2_HOOK_FASTCALL InitMovie(void);

int C2_HOOK_FASTCALL CreateMovie(void);

int C2_HOOK_FASTCALL GenerateOneMovieFrame(void);

void C2_HOOK_FASTCALL TrapS3SoundOutput(void);

void C2_HOOK_FASTCALL FreeS3SoundOutput(void);

int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap *pScreen, int pFrame_period);

#endif //CARPOCALYPSE2_VIDEO_H
