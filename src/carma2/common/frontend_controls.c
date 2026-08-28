#include "frontend_controls.h"

#include "frontend.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "main.h"
#include "options.h"
#include "platform.h"
#include "sound.h"

#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x006864f8
extern int gFrontend_controls_count_keys;