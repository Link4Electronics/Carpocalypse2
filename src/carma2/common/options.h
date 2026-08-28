#ifndef CARPOCALYPSE2_OPTIONS_H
#define CARPOCALYPSE2_OPTIONS_H

#include "c2_hooks.h"

extern char* gKey_names[153];
extern int gOrig_key_map_index;


void C2_HOOK_FASTCALL DoOptions(void);

void C2_HOOK_FASTCALL StripControls(char* pStr);

void C2_HOOK_FASTCALL LoadKeyNames(void);

void C2_HOOK_FASTCALL DisposeKeyNames(void);

void C2_HOOK_FASTCALL BackupKeyMappings(void);

void C2_HOOK_FASTCALL Joystick_BackupSettings(void);

#endif //CARPOCALYPSE2_OPTIONS_H
