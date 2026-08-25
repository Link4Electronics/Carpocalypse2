#include "42-input.h"
#include "carpocalypse2_types.h"
#include "carpocalypse2_macros.h"
#include "platform.h"
extern int gKeys_pressed;

// GLOBAL: CARMA2_HW 0x0074b5c0
tJoy_array gJoy_array;

// GLOBAL: CARMA2_HW 0x0068bee0
tKey_array gKey_array;

// GLOBAL: CARMA2_HW 0x0068bed4
int gKey_poll_counter;

// GLOBAL: CARMA2_HW 0x0074b5e0
int gKey_mapping[77];

// GLOBAL: CARMA2_HW 0x0068c1c4
int gEdge_trigger_mode;

// SetJoystickArrays

// FUNCTION: CARMA2_HW 0x00481eb0
void C2_HOOK_FASTCALL PollKeys(void) {

    gKey_poll_counter += 1;
    PDSetKeyArray(gKey_array, gKey_poll_counter);
}

// CyclePollKeys

// ResetPollKeys

// CheckKeysForMouldiness

// GLOBAL: CARMA2_HW 0x006571f8
int gLast_mouse_x_coord;
// GLOBAL: CARMA2_HW 0x006571fc
int gLast_mouse_y_coord;

// FUNCTION: CARMA2_HW 0x004824c0
int C2_HOOK_FASTCALL EitherMouseButtonDown(void) {
    int x_coord;
    int y_coord;
    int click_x_coord;
    int click_y_coord;
    int left_button_down;
    int right_button_down;
    int previous_x;
    int previous_y;

    previous_x = gLast_mouse_x_coord;
    previous_y = gLast_mouse_y_coord;
    GetMousePosition(&x_coord, &y_coord);
    if (previous_x == x_coord && previous_y == y_coord && PDGetMouseClickPosition(&click_x_coord, &click_y_coord) != 0
            && x_coord == click_x_coord && y_coord == click_y_coord) {
        return 2;
    }
    PDMouseButtons(&left_button_down, &right_button_down);
    return (left_button_down || right_button_down) ? 1 : 0;
}

// AnyModifiersDown

// EdgeTriggeryKey

// STUB: CARMA2_HW 0x00482550
int C2_HOOK_FASTCALL PDKeyDown(int pKey_index) {
#ifndef CARPOCALYPSE2_MATCHING
    return 0;
#else
    NOT_IMPLEMENTED();
    return 0;
#endif
}

// PDKeyDown3

// FUNCTION: CARMA2_HW 0x00482a00
int C2_HOOK_FASTCALL PDAnyKeyDown(void) {
    int i;

    PollKeys();
    for (i = CARPOCALYPSE2_ASIZE(gKey_array) - 1; i >= 0; i--) {
        if (gKey_array[i] != 0) {
            return i;
        }
    }
    return -1;
}

// STUB: CARMA2_HW 0x00482d70
int C2_HOOK_FASTCALL AnyKeyDown(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00482f10
tU32* C2_HOOK_FASTCALL KevKeyService(void) {
    static tU32 sum = 0;
    static tU32 code = 0;
    static tU32 code2 = 0;
    static int last_key = -1;
    static int last_single_key = -1;
    static tU32 last_time = 0;
    static tU32 return_val[2];
    tU32 keys;

    return_val[0] = 0;
    return_val[1] = 0;
    keys = gKeys_pressed;

    if (keys < 0x6B) {
        last_single_key = gKeys_pressed;
    } else {
        if (keys > 0x6b00) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if ((keys & 0xff) != last_single_key && keys >> 8 != last_single_key) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if (keys >> 8 != last_single_key) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if ((keys & 0xff) == last_single_key) {
            keys = keys >> 8;
        }
        keys = keys & 0xff;
    }

    if (keys != 0 && keys != last_key) {
        sum += keys;
        code += keys << 11;
        code = (code >> 17) + (code << 4);
        code2 = (code2 >> 29) + keys * keys + (code2 << 3);
        last_time = PDGetTotalTime();
    } else if ((tU32)PDGetTotalTime() > last_time + 1000) {
        return_val[0] = (code >> 11) + (sum << 21);
        return_val[1] = code2;
        // printf("final value: code=%lx, code2=%lx\n", return_val[0], return_val[1]);
        code = 0;
        code2 = 0;
        sum = 0;
    }
    last_key = keys;
    return return_val;
}

// STUB: CARMA2_HW 0x00483040
int C2_HOOK_FASTCALL KeyIsDown(int pKey_index) {
    NOT_IMPLEMENTED();
}

// KeyIsDownNoMouldiness

// STUB: CARMA2_HW 0x004833b0
void C2_HOOK_FASTCALL WaitForNoKeys(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00483c10
void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY) {

    PDGetMousePosition(pX, pY);
    if (*pX < 0) {
        *pX = 0;
    } else if (*pX > gGraf_specs[gGraf_spec_index].total_width) {
        *pX = gGraf_specs[gGraf_spec_index].total_width;
    }
    if (*pY < 0) {
        *pY = 0;
        gLast_mouse_x_coord = *pX;
        gLast_mouse_y_coord = *pY;
        return;
    } else if (*pY > gGraf_specs[gGraf_spec_index].total_height) {
        *pY = gGraf_specs[gGraf_spec_index].total_height;
    }
    gLast_mouse_x_coord = *pX;
    gLast_mouse_y_coord = *pY;
}

// STUB: CARMA2_HW 0x00483c90
void C2_HOOK_FASTCALL InitRollingLetters(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// STUB: CARMA2_HW 0x00483ce0
void C2_HOOK_FASTCALL EndRollingLetters(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// AddRollingLetter

// ChangeCharTo

// STUB: CARMA2_HW 0x00484120
void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00484600
void C2_HOOK_FASTCALL EdgeTriggerModeOn(void) {

    gEdge_trigger_mode = 1;
}


// FUNCTION: CARMA2_HW 0x00484610
void C2_HOOK_FASTCALL EdgeTriggerModeOff(void) {

    gEdge_trigger_mode = 0;
}

