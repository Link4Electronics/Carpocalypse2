#ifndef CARPOCALYPSE2_JOYSTICK_H
#define CARPOCALYPSE2_JOYSTICK_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern int gJoystick_index;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;


extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

#endif /* CARPOCALYPSE2_JOYSTICK_H */
