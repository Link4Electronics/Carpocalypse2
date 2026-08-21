#ifndef CARPOCALYPSE2_PRATCAM_H
#define CARPOCALYPSE2_PRATCAM_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitPratcam(void);

void C2_HOOK_FASTCALL DisposePratcam(void);

void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);

void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

#endif //CARPOCALYPSE2_PRATCAM_H