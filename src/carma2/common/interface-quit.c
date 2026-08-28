#include "interface-quit.h"

#include "interface.h"
#include "interface-main.h"
extern int C2_HOOK_FASTCALL Options_Quit(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend);
// GLOBAL: CARMA2_HW 0x00649df0
tFrontend_spec gFrontend_QUIT = {
    "Quit",
    0,
    3,
    Generic_Infunc,
    Generic_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    12,
    0,
    0,
    {
        { 0x101,    temp,           NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     Options_Quit,   NULL,       2, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,           (void*)1,   0, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
};
