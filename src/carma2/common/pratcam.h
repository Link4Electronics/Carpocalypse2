#ifndef GUARD_PRATCAM_H
#define GUARD_PRATCAM_H

#include "c2_hooks.h"

// ChangeAmbientPratcam

// PratcamEvent

// InitPratcam

extern void C2_HOOK_FASTCALL DisposePratcam(void);

#define CARPOCALYPSE2_PRATCAM_H

#include "c2_hooks.h"


void C2_HOOK_FASTCALL InitPratcam(void);

void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);

void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

#endif // GUARD_PRATCAM_H
