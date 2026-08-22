#include "72-interface-main.h"

#include "72-interface.h"

#ifndef CARPOCALYPSE2_MATCHING

static int OnePlayerSetup(tFrontend_spec* pFrontend) {
    return 1;
}

static int MultiplayerSetup(tFrontend_spec* pFrontend) {
    return 0;
}

static int QuitGameItem(tFrontend_spec* pFrontend) {
    return -1;
}

// GLOBAL: CARMA2_HW 0x005a80f0
tFrontend_spec gFrontend_MAIN = {
    "Main",
    0,
    3,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    0,
    0,
    2,
    2,
    1,
    {
        { 0, OnePlayerSetup,   NULL, 0, 17, 18, 220, 180, 200, 24, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 }, "START GAME" },
        { 0, MultiplayerSetup, NULL, 0, 17, 18, 220, 220, 200, 24, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 }, "MULTIPLAYER" },
        { 0, QuitGameItem,     NULL, 0, 17, 18, 220, 260, 200, 24, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 }, "QUIT" },
    },
    0,
    { { 0 } },
    0,
    { { 0 } },
    0,
    0,
    { { 0 } },
    "",
    { { 0 } },
    0,
    "",
    0,
    0,
};

#else
tFrontend_spec gFrontend_MAIN = { 0 };
#endif
