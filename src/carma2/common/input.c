#include "input.h"

#include "globvars.h"
#include "grafdata.h"
#include "utility.h"

#include "platform.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"

#define NBR_ROLLING_LETTERS 500
#define ROLLING_LETTER_LOOP_RANDOM 96
#include "input.h"
#include "carpocalypse2_types.h"
#include "carpocalypse2_macros.h"
#include "platform.h"
// GLOBAL: CARMA2_HW 0x006571f8
tMouse_coord gCurrent_mouse_position = {-1, -1};

// GLOBAL: CARMA2_HW 0x00657200
int gGo_ahead_keys[3] = { 51, 52, 106 };

// GLOBAL: CARMA2_HW 0x0065720c
int gLast_key_down = -1000;

// GLOBAL: CARMA2_HW 0x0068c1d0
tU32 gLast_key_down_time;

// GLOBAL: CARMA2_HW 0x0068c1c4
extern int gEdge_trigger_mode;
// GLOBAL: CARMA2_HW 0x0068bed4
extern int gKey_poll_counter;

// GLOBAL: CARMA2_HW 0x0068c144
tU32 gLast_roll;

// GLOBAL: CARMA2_HW 0x0068c140
int gCurrent_cursor;

// GLOBAL: CARMA2_HW 0x0068be88
tRolling_letter* gRolling_letters;

// GLOBAL: CARMA2_HW 0x0068be90
int gLetter_x_coords[15];

// GLOBAL: CARMA2_HW 0x0068bed0
int gVisible_length;

// GLOBAL: CARMA2_HW 0x0068be48
int gLetter_y_coords[15];

// GLOBAL: CARMA2_HW 0x0068be84
int gThe_length;

// GLOBAL: CARMA2_HW 0x0068c150
char gCurrent_typing[110];

// GLOBAL: CARMA2_HW 0x0068c1c0
int gCurrent_position;

// GLOBAL: CARMA2_HW 0x0068c1c8
tU32 gLast_poll_keys;

// GLOBAL: CARMA2_HW 0x006621e4
int gINT_006621e4 = 1;

// GLOBAL: CARMA2_HW 0x006621e8
int gINT_006621e8 = 1;

// GLOBAL: CARMA2_HW 0x006621ec
int gINT_006621ec = 1;

// GLOBAL: CARMA2_HW 0x006621f0
int gINT_006621f0 = 1;

// GLOBAL: CARMA2_HW 0x0068be44
int gJoy1_x;

// GLOBAL: CARMA2_HW 0x0068c13c
int gJoy1_y;

// GLOBAL: CARMA2_HW 0x0068be40
int gJoy2_x;

// GLOBAL: CARMA2_HW 0x0068becc
int gJoy2_y;

// GLOBAL: CARMA2_HW 0x0068bed8
int gModifiers_down;

extern tU32 gKeys_pressed;

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

// GLOBAL: CARMA2_HW 0x006571f8
int gLast_mouse_x_coord;
// GLOBAL: CARMA2_HW 0x006571fc
int gLast_mouse_y_coord;

int C2_HOOK_FASTCALL AnyModifiersDown(void) {
    int i;

    for (i = 0; i <= 10; i++) {
        if (gKey_array[i] != 0) {
            return 1;
        }
    }
    return 0;
}

#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark);
extern void C2_HOOK_FASTCALL PDSetKeysFromJoystick(int *keys);
extern void C2_HOOK_FASTCALL SetJoystickArrays(int* pKeys, int pMark);
extern br_actor* gActor_array[100];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void);
extern int C2_HOOK_FASTCALL PDGetJoy1X(void);
extern int C2_HOOK_FASTCALL PDGetJoy1Y(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void);
extern int C2_HOOK_FASTCALL PDGetJoy2X(void);
extern int C2_HOOK_FASTCALL PDGetJoy2Y(void);
extern void C2_HOOK_FASTCALL PDReadJoysticks(void);
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL CheckQuit(void);
extern void C2_HOOK_FASTCALL CyclePollKeys(void);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern int C2_HOOK_FASTCALL PDGetMouseClickPosition(int* pX_coord, int* pY_coord);
extern void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int gJoystick_deadzone;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_0068c1e4[];
extern char gData_0074b384[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_0068c1e4[];
extern char gMatchData_0074b384[];
#endif
// FUNCTION: CARMA2_HW 0x00482590
tKey_down_result C2_NAKED C2_HOOK_FASTCALL EdgeTriggeryKey(int pKey_index, int pReset) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 now;

    if (pReset) {
        gLast_key_down = -1;
        gLast_key_down_time = 0;
        return tKey_down_no;
    }
    CheckKeysForMouldiness();
    if (!gEdge_trigger_mode) {
        return gKey_array[pKey_index];
    }
    if (gKey_array[pKey_index] == 0) {
        if (pKey_index == gLast_key_down) {
            gLast_key_down_time = 0;
            gLast_key_down = -1000;
        }
        return tKey_down_no;
    }
    now = PDGetTotalTime();
    if (pKey_index == gLast_key_down) {
        if (gModifiers_down == AnyModifiersDown()) {
            if (now - gLast_key_down_time < 300) {
                return tKey_down_still;
            } else {
                gLast_key_down_time = now;
                return tKey_down_repeat;
            }
        } else {
            gLast_key_down_time = 0;
            gLast_key_down = -gLast_key_down;
            return tKey_down_no;
        }
    }
    if (pKey_index == -gLast_key_down) {
        return tKey_down_no;
    }
    if (pKey_index < 11) {
        gLast_key_down_time = 0;
        gLast_key_down = -1000;
        return tKey_down_yes;
    }
    gLast_key_down = pKey_index;
    gLast_key_down_time = now;
    gModifiers_down = AnyModifiersDown();
    return tKey_down_yes;
#else
    __asm {
            push ebx
            xor ebx, ebx
            push esi
            cmp edx, ebx
            push edi
            mov esi, ecx
            je L004825b1
            mov dword ptr [gLast_key_down], 0xffffffff
            mov dword ptr [gLast_key_down_time], ebx
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    L004825b1:
            call PDGetTotalTime
            sub eax, dword ptr [gLast_poll_keys]
            cmp eax, 0x1f4
            jbe L00482650
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            mov edx, dword ptr [gKey_poll_counter]
            rep stosd
            mov ecx, 8
            or eax, 0xffffffff
            mov edi, offset gJoy_array
            rep stosd
            xor ecx, ecx
    L004825ec:
            cmp dword ptr [ecx + gKey_array], edx
            jle L00482608
            cmp ecx, 0x23c
            mov dword ptr [ecx + gKey_array], ebx
            jle L00482608
            mov dword ptr [ecx + gMatchData_0074b384], eax
    L00482608:
            add ecx, 4
            cmp ecx, 0x25c
            jl L004825ec
            mov edx, 1
            mov ecx, offset gKey_array
            mov dword ptr [gKey_poll_counter], 1
            call PDSetKeyArray
            mov edx, dword ptr [gKey_poll_counter]
            mov ecx, offset gKey_array
            call SetJoystickArrays
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov dword ptr [gLast_poll_keys], eax
    L00482650:
            mov eax, dword ptr [gEdge_trigger_mode]
            cmp eax, ebx
            mov eax, dword ptr [esi*4 + gKey_array]
            je L00482754
            cmp eax, ebx
            je L0048273a
            call PDGetTotalTime
            mov edx, dword ptr [gLast_key_down]
            cmp esi, edx
            jne L004826da
            mov ecx, offset gKey_array
    L00482680:
            cmp dword ptr [ecx], ebx
            jne L00482758
            add ecx, 4
            cmp ecx, offset gKey_array+0x28
            jle L00482680
            xor ecx, ecx
    L00482695:
            cmp dword ptr [gModifiers_down], ecx
            je L004826b1
            neg edx
            mov dword ptr [gLast_key_down], edx
            mov dword ptr [gLast_key_down_time], ebx
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    L004826b1:
            mov esi, dword ptr [gLast_key_down_time]
            mov ecx, eax
            sub ecx, esi
            cmp ecx, 0x12c
            jb L004826d1
            mov dword ptr [gLast_key_down_time], eax
            mov eax, 3
            pop edi
            pop esi
            pop ebx
            ret
    L004826d1:
            mov eax, 2
            pop edi
            pop esi
            pop ebx
            ret
    L004826da:
            neg edx
            cmp esi, edx
            je L00482752
            cmp esi, 0xa
            jle L00482721
            mov dword ptr [gLast_key_down_time], eax
            mov dword ptr [gLast_key_down], esi
            mov eax, offset gKey_array
    L004826f5:
            cmp dword ptr [eax], ebx
            jne L00482713
            add eax, 4
            cmp eax, offset gKey_array+0x28
            jle L004826f5
            xor eax, eax
            mov dword ptr [gModifiers_down], eax
            mov eax, 1
            pop edi
            pop esi
            pop ebx
            ret
    L00482713:
            mov eax, 1
            mov dword ptr [gModifiers_down], eax
            pop edi
            pop esi
            pop ebx
            ret
    L00482721:
            mov dword ptr [gLast_key_down_time], ebx
            mov dword ptr [gLast_key_down], 0xfffffc18
            mov eax, 1
            pop edi
            pop esi
            pop ebx
            ret
    L0048273a:
            cmp esi, dword ptr [gLast_key_down]
            jne L00482752
            mov dword ptr [gLast_key_down_time], ebx
            mov dword ptr [gLast_key_down], 0xfffffc18
    L00482752:
            xor eax, eax
    L00482754:
            pop edi
            pop esi
            pop ebx
            ret
    L00482758:
            mov ecx, 1
            jmp L00482695
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004833a0
int C2_HOOK_FASTCALL KeyIsDownNoMouldiness(int pKey_index) {

    return gKey_array[gKey_mapping[pKey_index]];
}

// FUNCTION: CARMA2_HW 0x00483cf0
int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type) {
    tRolling_letter* let;
    int i;

    for (let = gRolling_letters, i = 0; i < NBR_ROLLING_LETTERS; i++, let++) {
        if (let->number_of_letters < 0) {
            goto letter_found;
        }
    }
    return -1;

letter_found:
    let->rolling_type = rolling_type;
    let->x_coord = pX;
    let->y_coord = pY;
    if (rolling_type == eRT_looping_random) {
        let->number_of_letters = 9;
    } else if (rolling_type == eRT_looping_single) {
        let->number_of_letters = 2;
    } else {
        let->number_of_letters = IRandomBetween(3, 9);
    }
    let->current_offset = (float)(let->number_of_letters * gCurrent_graf_data->save_slot_letter_height);
    for (i = let->number_of_letters - 1; i > (rolling_type != eRT_looping_random ? 0 : -1); i--) {
        if (rolling_type != eRT_numeric) {
            let->letters[i] = IRandomBetween('A', 'Z' + 1);
        } else {
            let->letters[i] = (tS8)pChar;
        }
    }
    if (rolling_type != eRT_looping_random) {
        let->letters[0] = (tS8)pChar;
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x00483dd0
void C2_HOOK_FASTCALL AddRollingString(char* pStr, int pX, int pY, tRolling_type pRolling_type) {
    int i;

    for (i = 0; (size_t)i < strlen(pStr); i++) {
        AddRollingLetter(pStr[i], pX + gCurrent_graf_data->rolling_letter_x_pitch * i, pY, pRolling_type);
    }
}

// FUNCTION: CARMA2_HW 0x00484000
int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char) {
    int x_coord;
    int y_coord;
    int i;
    tRolling_letter* let;
    tRolling_type new_type;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, save_slot_letter_height, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, rolling_letter_x_pitch, 0x10);

    if (pChar_index >= gVisible_length || pChar_index < 0) {
        return -1;
    }
    y_coord = gLetter_y_coords[pSlot_index];
    x_coord = gLetter_x_coords[pSlot_index] + pChar_index * gCurrent_graf_data->rolling_letter_x_pitch;
    if (pNew_char == ROLLING_LETTER_LOOP_RANDOM) {
        new_type = eRT_looping_random;
    } else if (pNew_char >= '0' && pNew_char <= '9') {
        new_type = eRT_numeric;
    } else {
        new_type = eRT_alpha;
    }
    for (let = gRolling_letters, i = 0; i < NBR_ROLLING_LETTERS; i++, let++) {
        if (let->number_of_letters >= 0 && x_coord == let->x_coord && y_coord == let->y_coord) {
            goto letter_found;
        }
    }
    return AddRollingLetter(pNew_char, x_coord, y_coord, new_type);

letter_found:
    if (pNew_char != ROLLING_LETTER_LOOP_RANDOM) {
        let->letters[0] = (tS8)pNew_char;
    }
    if (pNew_char == ' ') {
        let->letters[0] = ' ';
    }
    let->current_offset = (float)(gCurrent_graf_data->save_slot_letter_height * let->number_of_letters);
    let->rolling_type = new_type;
    return i;
}

// FUNCTION: CARMA2_HW 0x00482770
__inline void C2_HOOK_FASTCALL SetJoystickArrays(int* pKeys, int pMark) {
    int i;

    for (i = 0; i < 44; i++) {
        pKeys[107 + i] = 0;
    }
    PDReadJoysticks();
    for (i = 0; i < 8; i++) {
        gJoy_array[i] = -1;
    }
    for (i = 0; i < 36; i++) {
        pKeys[107 + i] = 0;
    }
    if (gINT_006621e4 || gINT_006621e8) {
        if (PDGetJoy1Button1()) {
            pKeys[107] = pMark;
        }
        if (PDGetJoy1Button2()) {
            pKeys[108] = pMark;
        }
        if (PDGetJoy1Button3()) {
            pKeys[109] = pMark;
        }
        if (PDGetJoy1Button4()) {
            pKeys[110] = pMark;
        }
    }
    if (gINT_006621ec || gINT_006621f0) {
        if (PDGetJoy2Button1()) {
            pKeys[139] = pMark;
        }
        if (PDGetJoy2Button2()) {
            pKeys[140] = pMark;
        }
        if (PDGetJoy2Button3()) {
            pKeys[141] = pMark;
        }
        if (PDGetJoy2Button4()) {
            pKeys[142] = pMark;
        }
    }
    if (gINT_006621e4) {
        int new_joy1_x = PDGetJoy1X();
        if (new_joy1_x != -1) {
            if (new_joy1_x < 0x8000) {
                if (new_joy1_x < 0x4000 && gJoy1_x >= 0x4000) {
                    pKeys[143] = pMark;
                }
                gJoy_array[0] = 2 * (0x8000 - new_joy1_x);
            } else {
                if (new_joy1_x > 0xc000 && gJoy1_x <= 0xc000) {
                    pKeys[144] = pMark;
                }
                gJoy_array[1] = 2 * (new_joy1_x - 0x8000);
            }
        }
        gJoy1_x = new_joy1_x;
    }
    if (gINT_006621e8) {
        int new_joy1_y = PDGetJoy1Y();
        if (new_joy1_y != -1) {
            if (new_joy1_y < 0x8000) {
                if (new_joy1_y < 0x4000 && gJoy1_y >= 0x4000) {
                    pKeys[145] = pMark;
                }
                gJoy_array[2] = 2 * (0x8000 - new_joy1_y);
            }
            else {
                if (new_joy1_y > 0xc000 && gJoy1_y <= 0xc000) {
                    pKeys[146] = pMark;
                }
                gJoy_array[3] = 2 * (new_joy1_y - 0x8000);
            }
        }
        gJoy1_y = new_joy1_y;
    }
    if (gINT_006621ec) {
        int new_joy2_x = PDGetJoy2X();
        if (new_joy2_x != -1) {
            if (new_joy2_x < 0x8000) {
                if (new_joy2_x < 0x4000 && gJoy2_x >= 0x4000) {
                    pKeys[147] = pMark;
                }
                gJoy_array[4] = 2 * (0x8000 - new_joy2_x);
            } else {
                if (new_joy2_x > 0xc000 && gJoy2_x <= 0xc000) {
                    pKeys[148] = pMark;
                }
                gJoy_array[5] = 2 * (new_joy2_x - 0x8000);
            }
        }
        gJoy2_x = new_joy2_x;
    }
    if (gINT_006621f0) {
        int new_joy2_y = PDGetJoy2Y();
        if (new_joy2_y != -1) {
            if (new_joy2_y < 0x8000) {
                if (new_joy2_y < 0x4000 && gJoy2_y >= 0x4000) {
                    pKeys[149] = pMark;
                }
                gJoy_array[6] = 2 * (0x8000 - new_joy2_y);
            } else {
                if (new_joy2_y > gJoystick_deadzone + 0x8000) {
                    if (new_joy2_y > 0xc000 && gJoy2_y <= 0xc000) {
                        pKeys[150] = pMark;
                    }
                    gJoy_array[7] = 2 * (new_joy2_y - 0x8000);
                }
            }
        }
        gJoy2_y = new_joy2_y;
    }
}
// SetJoystickArrays

// FUNCTION: CARMA2_HW 0x00481eb0
void C2_HOOK_FASTCALL PollKeys(void) {

    gKey_poll_counter += 1;
    PDSetKeyArray(gKey_array, gKey_poll_counter);
    SetJoystickArrays(gKey_array, gKey_poll_counter);
    PDSetKeysFromJoystick(gKey_array);
    gLast_poll_keys = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00482160
void C2_NAKED C2_HOOK_FASTCALL CyclePollKeys(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gKey_array); i++) {
        if (gKey_array[i] > gKey_poll_counter) {
            gKey_array[i] = 0;
            if (i > 143) {
                gJoy_array[i - 143] = -1;
            }
        }
    }
    gKey_poll_counter = 0;
#else
    __asm {
            mov ecx, dword ptr [gKey_poll_counter]
            push esi
            xor edx, edx
            xor eax, eax
            or esi, 0xffffffff
    L0048216e:
            cmp dword ptr [eax + gKey_array], ecx
            jle L00482189
            cmp eax, 0x23c
            mov dword ptr [eax + gKey_array], edx
            jle L00482189
            mov dword ptr [eax + gMatchData_0074b384], esi
    L00482189:
            add eax, 4
            cmp eax, 0x25c
            jl L0048216e
            mov dword ptr [gKey_poll_counter], edx
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004821a0
void C2_HOOK_FASTCALL ResetPollKeys(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gKey_array) != 151);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gJoy_array) != 8);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_array); i++) {
        gKey_array[i] = 0;
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gJoy_array); i++) {
        gJoy_array[i] = -1;
    }
}

// FUNCTION: CARMA2_HW 0x004821c0
void C2_NAKED C2_HOOK_FASTCALL CheckKeysForMouldiness(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (PDGetTotalTime() - gLast_poll_keys > 500) {
        ResetPollKeys();
        CyclePollKeys();
        PollKeys();
    }
#else
    __asm {
            call PDGetTotalTime
            sub eax, dword ptr [gLast_poll_keys]
            cmp eax, 0x1f4
            jbe L004824bd
            push edi
            push esi
            push ebp
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            push ebx
            or ebx, 0xffffffff
            rep stosd
            mov ecx, 8
            mov eax, ebx
            mov edi, offset gJoy_array
            xor ebp, ebp
            rep stosd
            mov ecx, dword ptr [gKey_poll_counter]
            xor eax, eax
    L00482203:
            cmp dword ptr [eax + gKey_array], ecx
            jle L0048221e
            cmp eax, 0x23c
            mov dword ptr [eax + gKey_array], ebp
            jle L0048221e
            mov dword ptr [eax + gMatchData_0074b384], ebx
    L0048221e:
            add eax, 4
            cmp eax, 0x25c
            jl L00482203
            mov edx, 1
            mov ecx, offset gKey_array
            mov dword ptr [gKey_poll_counter], edx
            call PDSetKeyArray
            mov esi, dword ptr [gKey_poll_counter]
            mov ecx, 0x2c
            xor eax, eax
            mov edi, offset gKey_array+0x1ac
            rep stosd
            call PDReadJoysticks
            mov ecx, 8
            mov eax, ebx
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, 0x24
            xor eax, eax
            mov edi, offset gKey_array+0x1ac
            rep stosd
            cmp dword ptr [gINT_006621e4], ebp
            jne L00482282
            cmp dword ptr [gINT_006621e8], ebp
            je L004822be
    L00482282:
            call PDGetJoy1Button1
            test eax, eax
            je L00482291
            mov dword ptr [gKey_array+0x1ac], esi
    L00482291:
            call PDGetJoy1Button2
            test eax, eax
            je L004822a0
            mov dword ptr [gKey_array+0x1b0], esi
    L004822a0:
            call PDGetJoy1Button3
            test eax, eax
            je L004822af
            mov dword ptr [gKey_array+0x1b4], esi
    L004822af:
            call PDGetJoy1Button4
            test eax, eax
            je L004822be
            mov dword ptr [gKey_array+0x1b8], esi
    L004822be:
            cmp dword ptr [gINT_006621ec], ebp
            jne L004822ce
            cmp dword ptr [gINT_006621f0], ebp
            je L0048230a
    L004822ce:
            call PDGetJoy2Button1
            test eax, eax
            je L004822dd
            mov dword ptr [gKey_array+0x22c], esi
    L004822dd:
            call PDGetJoy2Button2
            test eax, eax
            je L004822ec
            mov dword ptr [gKey_array+0x230], esi
    L004822ec:
            call PDGetJoy2Button3
            test eax, eax
            je L004822fb
            mov dword ptr [gKey_array+0x234], esi
    L004822fb:
            call PDGetJoy2Button4
            test eax, eax
            je L0048230a
            mov dword ptr [gKey_array+0x238], esi
    L0048230a:
            mov eax, dword ptr [gINT_006621e4]
            mov edi, 0x4000
            cmp eax, ebp
            mov ebx, 0xc000
            je L00482375
            call PDGetJoy1X
            cmp eax, -1
            je L00482370
            cmp eax, 0x8000
            jge L00482351
            cmp eax, edi
            jge L00482340
            cmp dword ptr [gJoy1_x], edi
            jl L00482340
            mov dword ptr [gKey_array+0x23c], esi
    L00482340:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array], ecx
            jmp L00482370
    L00482351:
            cmp eax, ebx
            jle L00482363
            cmp dword ptr [gJoy1_x], ebx
            jg L00482363
            mov dword ptr [gKey_array+0x240], esi
    L00482363:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x4], edx
    L00482370:
            mov dword ptr [gJoy1_x], eax
    L00482375:
            cmp dword ptr [gINT_006621e8], ebp
            je L004823d5
            call PDGetJoy1Y
            cmp eax, -1
            je L004823d0
            cmp eax, 0x8000
            jge L004823b1
            cmp eax, edi
            jge L004823a0
            cmp dword ptr [gJoy1_y], edi
            jl L004823a0
            mov dword ptr [gKey_array+0x244], esi
    L004823a0:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x8], ecx
            jmp L004823d0
    L004823b1:
            cmp eax, ebx
            jle L004823c3
            cmp dword ptr [gJoy1_y], ebx
            jg L004823c3
            mov dword ptr [gKey_array+0x248], esi
    L004823c3:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0xc], edx
    L004823d0:
            mov dword ptr [gJoy1_y], eax
    L004823d5:
            cmp dword ptr [gINT_006621ec], ebp
            je L00482435
            call PDGetJoy2X
            cmp eax, -1
            je L00482430
            cmp eax, 0x8000
            jge L00482411
            cmp eax, edi
            jge L00482400
            cmp dword ptr [gJoy2_x], edi
            jl L00482400
            mov dword ptr [gKey_array+0x24c], esi
    L00482400:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x10], ecx
            jmp L00482430
    L00482411:
            cmp eax, ebx
            jle L00482423
            cmp dword ptr [gJoy2_x], ebx
            jg L00482423
            mov dword ptr [gKey_array+0x250], esi
    L00482423:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x14], edx
    L00482430:
            mov dword ptr [gJoy2_x], eax
    L00482435:
            cmp dword ptr [gINT_006621f0], ebp
            je L004824a5
            call PDGetJoy2Y
            cmp eax, -1
            je L004824a0
            cmp eax, 0x8000
            jge L00482471
            cmp eax, edi
            jge L00482460
            cmp dword ptr [gJoy2_y], edi
            jl L00482460
            mov dword ptr [gKey_array+0x254], esi
    L00482460:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x18], ecx
            jmp L004824a0
    L00482471:
            mov edx, dword ptr [gJoystick_deadzone]
            add edx, 0x8000
            cmp eax, edx
            jle L004824a0
            cmp eax, ebx
            jle L00482493
            cmp dword ptr [gJoy2_y], ebx
            jg L00482493
            mov dword ptr [gKey_array+0x258], esi
    L00482493:
            lea ecx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x1c], ecx
    L004824a0:
            mov dword ptr [gJoy2_y], eax
    L004824a5:
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov dword ptr [gLast_poll_keys], eax
            pop ebx
            pop ebp
            pop esi
            pop edi
    L004824bd:
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004824c0
int C2_HOOK_FASTCALL EitherMouseButtonDown(void) {
    tMouse_coord pos;
    tMouse_coord click_pos;
    int buttons[2];
    int prev_x;
    int prev_y;

    prev_x = gCurrent_mouse_position.x;
    prev_y = gLast_mouse_y_coord;
    GetMousePosition(&pos.x, &pos.y);
    if (prev_x == pos.x && prev_y == pos.y) {
        int ok = PDGetMouseClickPosition(&click_pos.x, &click_pos.y);
        if (ok && pos.x == click_pos.x && pos.y == click_pos.y) {
            return 2;
        }
    }
    PDMouseButtons(&buttons[0], &buttons[1]);
    if (buttons[0] || buttons[1]) {
        return 1;
    }
    return 0;
}

// AnyModifiersDown

// EdgeTriggeryKey

// FUNCTION: CARMA2_HW 0x00482550
int C2_HOOK_FASTCALL PDKeyDown(int pKey_index) {
    tKey_down_result result;

    result = EdgeTriggeryKey(pKey_index, 0);
    if (!gEdge_trigger_mode || pKey_index <= 10) {
        return result != tKey_down_no;
    }
    return result == tKey_down_yes || result == tKey_down_repeat;
}

// PDKeyDown3

// FUNCTION: CARMA2_HW 0x00482a00
int C2_NAKED C2_HOOK_FASTCALL PDAnyKeyDown(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    CheckKeysForMouldiness();
    for (i = 142; i >= 0; --i) {
        if (gKey_array[i] && i != 4) {
            if (!gEdge_trigger_mode) {
                return i;
            }
            switch (EdgeTriggeryKey(i, 0)) {
                case tKey_down_no:
                case tKey_down_still:
                    return -1;
                case tKey_down_yes:
                case tKey_down_repeat:
                    return i;
            }
        }
    }
    if (gEdge_trigger_mode) {
        EdgeTriggeryKey(0, 1);
    }
    return -1;
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            call PDGetTotalTime
            mov edx, dword ptr [gLast_poll_keys]
            xor ebp, ebp
            sub eax, edx
            cmp eax, 0x1f4
            jbe L00482cfb
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            or ebx, 0xffffffff
            rep stosd
            mov ecx, 8
            mov eax, ebx
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, dword ptr [gKey_poll_counter]
            xor eax, eax
    L00482a45:
            cmp dword ptr [eax + gKey_array], ecx
            jle L00482a60
            cmp eax, 0x23c
            mov dword ptr [eax + gKey_array], ebp
            jle L00482a60
            mov dword ptr [eax + gMatchData_0074b384], ebx
    L00482a60:
            add eax, 4
            cmp eax, 0x25c
            jl L00482a45
            mov edx, 1
            mov ecx, offset gKey_array
            mov dword ptr [gKey_poll_counter], edx
            call PDSetKeyArray
            mov esi, dword ptr [gKey_poll_counter]
            mov ecx, 0x2c
            xor eax, eax
            mov edi, offset gKey_array+0x1ac
            rep stosd
            call PDReadJoysticks
            mov ecx, 8
            mov eax, ebx
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, 0x24
            xor eax, eax
            mov edi, offset gKey_array+0x1ac
            rep stosd
            cmp dword ptr [gINT_006621e4], ebp
            jne L00482ac4
            cmp dword ptr [gINT_006621e8], ebp
            je L00482b00
    L00482ac4:
            call PDGetJoy1Button1
            test eax, eax
            je L00482ad3
            mov dword ptr [gKey_array+0x1ac], esi
    L00482ad3:
            call PDGetJoy1Button2
            test eax, eax
            je L00482ae2
            mov dword ptr [gKey_array+0x1b0], esi
    L00482ae2:
            call PDGetJoy1Button3
            test eax, eax
            je L00482af1
            mov dword ptr [gKey_array+0x1b4], esi
    L00482af1:
            call PDGetJoy1Button4
            test eax, eax
            je L00482b00
            mov dword ptr [gKey_array+0x1b8], esi
    L00482b00:
            cmp dword ptr [gINT_006621ec], ebp
            jne L00482b10
            cmp dword ptr [gINT_006621f0], ebp
            je L00482b4c
    L00482b10:
            call PDGetJoy2Button1
            test eax, eax
            je L00482b1f
            mov dword ptr [gKey_array+0x22c], esi
    L00482b1f:
            call PDGetJoy2Button2
            test eax, eax
            je L00482b2e
            mov dword ptr [gKey_array+0x230], esi
    L00482b2e:
            call PDGetJoy2Button3
            test eax, eax
            je L00482b3d
            mov dword ptr [gKey_array+0x234], esi
    L00482b3d:
            call PDGetJoy2Button4
            test eax, eax
            je L00482b4c
            mov dword ptr [gKey_array+0x238], esi
    L00482b4c:
            mov eax, dword ptr [gINT_006621e4]
            mov edi, 0x4000
            cmp eax, ebp
            mov ebx, 0xc000
            je L00482bb7
            call PDGetJoy1X
            cmp eax, -1
            je L00482bb2
            cmp eax, 0x8000
            jge L00482b93
            cmp eax, edi
            jge L00482b82
            cmp dword ptr [gJoy1_x], edi
            jl L00482b82
            mov dword ptr [gKey_array+0x23c], esi
    L00482b82:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array], ecx
            jmp L00482bb2
    L00482b93:
            cmp eax, ebx
            jle L00482ba5
            cmp dword ptr [gJoy1_x], ebx
            jg L00482ba5
            mov dword ptr [gKey_array+0x240], esi
    L00482ba5:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x4], edx
    L00482bb2:
            mov dword ptr [gJoy1_x], eax
    L00482bb7:
            cmp dword ptr [gINT_006621e8], ebp
            je L00482c17
            call PDGetJoy1Y
            cmp eax, -1
            je L00482c12
            cmp eax, 0x8000
            jge L00482bf3
            cmp eax, edi
            jge L00482be2
            cmp dword ptr [gJoy1_y], edi
            jl L00482be2
            mov dword ptr [gKey_array+0x244], esi
    L00482be2:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x8], ecx
            jmp L00482c12
    L00482bf3:
            cmp eax, ebx
            jle L00482c05
            cmp dword ptr [gJoy1_y], ebx
            jg L00482c05
            mov dword ptr [gKey_array+0x248], esi
    L00482c05:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0xc], edx
    L00482c12:
            mov dword ptr [gJoy1_y], eax
    L00482c17:
            cmp dword ptr [gINT_006621ec], ebp
            je L00482c77
            call PDGetJoy2X
            cmp eax, -1
            je L00482c72
            cmp eax, 0x8000
            jge L00482c53
            cmp eax, edi
            jge L00482c42
            cmp dword ptr [gJoy2_x], edi
            jl L00482c42
            mov dword ptr [gKey_array+0x24c], esi
    L00482c42:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x10], ecx
            jmp L00482c72
    L00482c53:
            cmp eax, ebx
            jle L00482c65
            cmp dword ptr [gJoy2_x], ebx
            jg L00482c65
            mov dword ptr [gKey_array+0x250], esi
    L00482c65:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x14], edx
    L00482c72:
            mov dword ptr [gJoy2_x], eax
    L00482c77:
            cmp dword ptr [gINT_006621f0], ebp
            je L00482ce7
            call PDGetJoy2Y
            cmp eax, -1
            je L00482ce2
            cmp eax, 0x8000
            jge L00482cb3
            cmp eax, edi
            jge L00482ca2
            cmp dword ptr [gJoy2_y], edi
            jl L00482ca2
            mov dword ptr [gKey_array+0x254], esi
    L00482ca2:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x18], ecx
            jmp L00482ce2
    L00482cb3:
            mov edx, dword ptr [gJoystick_deadzone]
            add edx, 0x8000
            cmp eax, edx
            jle L00482ce2
            cmp eax, ebx
            jle L00482cd5
            cmp dword ptr [gJoy2_y], ebx
            jg L00482cd5
            mov dword ptr [gKey_array+0x258], esi
    L00482cd5:
            lea ecx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x1c], ecx
    L00482ce2:
            mov dword ptr [gJoy2_y], eax
    L00482ce7:
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov dword ptr [gLast_poll_keys], eax
    L00482cfb:
            mov edi, 0x8e
            mov esi, offset gKey_array+0x238
    L00482d05:
            cmp dword ptr [esi], ebp
            je L00482d2e
            cmp esi, offset gKey_array+0x10
            je L00482d2e
            cmp dword ptr [gEdge_trigger_mode], ebp
            je L00482d56
            xor edx, edx
            mov ecx, edi
            call EdgeTriggeryKey
            cmp eax, 3
            ja L00482d2e
            jmp dword ptr [eax*4 + 0x482d60]
    L00482d2e:
            sub esi, 4
            dec edi
            cmp esi, offset gKey_array
            jge L00482d05
            cmp dword ptr [gEdge_trigger_mode], ebp
            je L00482d4e
            mov edx, 1
            xor ecx, ecx
            call EdgeTriggeryKey
    L00482d4e:
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00482d56:
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            lea ecx, [ecx]
            _emit 0x4e
            _emit 0x2d
            _emit 0x48
            _emit 0x00
            _emit 0x56
            _emit 0x2d
            _emit 0x48
            _emit 0x00
            _emit 0x4e
            _emit 0x2d
            _emit 0x48
            _emit 0x00
            _emit 0x56
            _emit 0x2d
            _emit 0x48
            _emit 0x00
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00482d70
int C2_NAKED C2_HOOK_FASTCALL AnyKeyDown(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int the_key;

    the_key = PDAnyKeyDown();
    return (the_key != -1 && the_key != 4) || EitherMouseButtonDown();
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            push esi
            push edi
            call PDGetTotalTime
            mov edx, dword ptr [gLast_poll_keys]
            or ebp, 0xffffffff
            sub eax, edx
            xor ebx, ebx
            cmp eax, 0x1f4
            jbe L00482e16
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            rep stosd
            mov ecx, 8
            mov eax, ebp
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, dword ptr [gKey_poll_counter]
            xor eax, eax
    L00482db8:
            cmp dword ptr [eax + gKey_array], ecx
            jle L00482dd3
            cmp eax, 0x23c
            mov dword ptr [eax + gKey_array], ebx
            jle L00482dd3
            mov dword ptr [eax + gMatchData_0074b384], ebp
    L00482dd3:
            add eax, 4
            cmp eax, 0x25c
            jl L00482db8
            mov edx, 1
            mov ecx, offset gKey_array
            mov dword ptr [gKey_poll_counter], edx
            call PDSetKeyArray
            mov edx, dword ptr [gKey_poll_counter]
            mov ecx, offset gKey_array
            call SetJoystickArrays
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov dword ptr [gLast_poll_keys], eax
    L00482e16:
            mov edi, 0x8e
            mov esi, offset gKey_array+0x238
    L00482e20:
            cmp dword ptr [esi], ebx
            je L00482e49
            cmp esi, offset gKey_array+0x10
            je L00482e49
            cmp dword ptr [gEdge_trigger_mode], ebx
            je L00482e6b
            xor edx, edx
            mov ecx, edi
            call EdgeTriggeryKey
            cmp eax, 3
            ja L00482e49
            jmp dword ptr [eax*4 + 0x482ef4]
    L00482e49:
            sub esi, 4
            dec edi
            cmp esi, offset gKey_array
            jge L00482e20
            cmp dword ptr [gEdge_trigger_mode], ebx
            je L00482e69
            mov edx, 1
            xor ecx, ecx
            call EdgeTriggeryKey
    L00482e69:
            mov edi, ebp
    L00482e6b:
            cmp edi, ebp
            je L00482e74
            cmp edi, 4
            jne L00482ee5
    L00482e74:
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 0x14]
            lea ecx, [esp + 0x10]
            call GetMousePosition
            cmp esi, dword ptr [esp + 0x10]
            jne L00482ec2
            cmp edi, dword ptr [esp + 0x14]
            jne L00482ec2
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x18]
            call PDGetMouseClickPosition
            test eax, eax
            je L00482ec2
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x18]
            cmp eax, ecx
            jne L00482ec2
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x1c]
            cmp ecx, eax
            je L00482ee5
    L00482ec2:
            lea edx, [esp + 0x24]
            lea ecx, [esp + 0x20]
            call PDMouseButtons
            cmp dword ptr [esp + 0x20], ebx
            jne L00482ee5
            cmp dword ptr [esp + 0x24], ebx
            jne L00482ee5
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L00482ee5:
            pop edi
            pop esi
            pop ebp
            mov eax, 1
            pop ebx
            add esp, 0x18
            ret
            mov edi, edi
            _emit 0x69
            _emit 0x2e
            _emit 0x48
            _emit 0x00
            _emit 0x6b
            _emit 0x2e
            _emit 0x48
            _emit 0x00
            _emit 0x69
            _emit 0x2e
            _emit 0x48
            _emit 0x00
            _emit 0x6b
            _emit 0x2e
            _emit 0x48
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00482f10
C2_NAKED tU32* C2_HOOK_FASTCALL KevKeyService(void) {
#ifndef CARPOCALYPSE2_MATCHING
    // GLOBAL: CARMA2_HW 0x0068c1d4
    static tU32 sum;
    // GLOBAL: CARMA2_HW 0x0068c1d8
    static tU32 code;
    // GLOBAL: CARMA2_HW 0x0068c1dc
    static tU32 code2;
    // GLOBAL: CARMA2_HW 0x00657210
    static int last_key = -1;
    // GLOBAL: CARMA2_HW 0x00657214
    static int last_single_key = -1;
    // GLOBAL: CARMA2_HW 0x0068be8c
    static tU32 last_time;
    // GLOBAL: CARMA2_HW 0x0068c1e0
    static tU32 return_val[2];
    tU32 keys;

    keys = gKeys_pressed;
    return_val[0] = 0;
    return_val[1] = 0;
    if (keys < 0x6b) {
        last_single_key = (int)keys;
    } else {
        if (keys > 0x6b00
            || ((keys & 0xff) != (tU32)last_single_key && (keys >> 8) != (tU32)last_single_key)) {
            sum = 0;
            code = 0;
            return return_val;
        }
        // Keep the newly pressed member of a two-key chord.
        if ((keys & 0xff) == (tU32)last_single_key) {
            keys >>= 8;
        } else {
            keys &= 0xff;
        }
    }
    if (keys != 0 && keys != (tU32)last_key) {
        sum += keys;
        code += keys << 11;
        code = (code >> 17) + (code << 4);
        code2 = (code2 >> 29) + keys * keys + (code2 << 3);
        last_time = PDGetTotalTime();
    } else if (PDGetTotalTime() > last_time + 1000u) {
        return_val[0] = (code >> 11) + (sum << 21);
        return_val[1] = code2;
        code = 0;
        code2 = 0;
        sum = 0;
    }
    last_key = (int)keys;
    return return_val;
#else
    __asm {
            push esi
            mov esi, dword ptr [gKeys_pressed]
            push edi
            xor edi, edi
            cmp esi, 0x6b
            _emit 0x89
        _emit 0x3d
        _emit 0xe0
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            mov dword ptr [gMatchData_0068c1e4], edi
            jb L00482f75
            cmp esi, 0x6b00
            ja L00482f61
            _emit 0x8b
        _emit 0x0d
        _emit 0x14
        _emit 0x72
        _emit 0x65
        _emit 0x00
            mov eax, esi
            and eax, 0xff
            cmp ecx, eax
            je L00482f51
            mov edx, esi
            shr edx, 8
            cmp ecx, edx
            jne L00482f61
            cmp ecx, eax
            jne L00482f56
    L00482f51:
            shr esi, 8
            jmp L00482f7b
    L00482f56:
            shr esi, 8
            cmp ecx, esi
            jne L00482f61
            mov esi, eax
            jmp L00482f7b
    L00482f61:
            _emit 0x89
        _emit 0x3d
        _emit 0xd4
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0x89
        _emit 0x3d
        _emit 0xd8
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            mov eax, 0x68c1e0
            pop edi
            pop esi
            ret
    L00482f75:
            _emit 0x89
        _emit 0x35
        _emit 0x14
        _emit 0x72
        _emit 0x65
        _emit 0x00
    L00482f7b:
            _emit 0x3b
        _emit 0x35
        _emit 0x10
        _emit 0x72
        _emit 0x65
        _emit 0x00
            je L00482fdf
            cmp esi, edi
            jbe L00482fdf
            _emit 0xa1
        _emit 0xd4
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            mov ecx, esi
            add eax, esi
            _emit 0xa3
        _emit 0xd4
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0xa1
        _emit 0xd8
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            shl ecx, 0xb
            add eax, ecx
            mov ecx, esi
            imul ecx, esi
            mov edx, eax
            shl edx, 4
            shr eax, 0x11
            add edx, eax
            _emit 0xa1
        _emit 0xdc
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0x89
        _emit 0x15
        _emit 0xd8
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            lea edx, [ecx + eax*8]
            shr eax, 0x1d
            add edx, eax
            _emit 0x89
        _emit 0x15
        _emit 0xdc
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            call PDGetTotalTime
            _emit 0xa3
        _emit 0x8c
        _emit 0xbe
        _emit 0x68
        _emit 0x00
            _emit 0x89
        _emit 0x35
        _emit 0x10
        _emit 0x72
        _emit 0x65
        _emit 0x00
            mov eax, 0x68c1e0
            pop edi
            pop esi
            ret
    L00482fdf:
            call PDGetTotalTime
            _emit 0x8b
        _emit 0x0d
        _emit 0x8c
        _emit 0xbe
        _emit 0x68
        _emit 0x00
            add ecx, 0x3e8
            cmp eax, ecx
            jbe L0048302b
            _emit 0x8b
        _emit 0x15
        _emit 0xd4
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0xa1
        _emit 0xd8
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0x8b
        _emit 0x0d
        _emit 0xdc
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0x89
        _emit 0x3d
        _emit 0xd8
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            shl edx, 0x15
            shr eax, 0xb
            add edx, eax
            mov dword ptr [gMatchData_0068c1e4], ecx
            _emit 0x89
        _emit 0x15
        _emit 0xe0
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0x89
        _emit 0x3d
        _emit 0xdc
        _emit 0xc1
        _emit 0x68
        _emit 0x00
            _emit 0x89
        _emit 0x3d
        _emit 0xd4
        _emit 0xc1
        _emit 0x68
        _emit 0x00
    L0048302b:
            _emit 0x89
        _emit 0x35
        _emit 0x10
        _emit 0x72
        _emit 0x65
        _emit 0x00
            pop edi
            mov eax, 0x68c1e0
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00483040
int C2_NAKED C2_HOOK_FASTCALL KeyIsDown(int pKey_index) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    CheckKeysForMouldiness();
    if (pKey_index == -2) {
        return 1;
    } else if (pKey_index == -1) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gGo_ahead_keys); i++) {
            if (gKey_array[gGo_ahead_keys[i]]) {
                return 1;
            }
        }
        return 0;
    } else {
        return gKey_array[gKey_mapping[pKey_index]];
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov dword ptr [esp + 0x10], esi
            call PDGetTotalTime
            mov edx, dword ptr [gLast_poll_keys]
            xor ebp, ebp
            sub eax, edx
            cmp eax, 0x1f4
            jbe L00483346
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            or ebx, 0xffffffff
            rep stosd
            mov ecx, 8
            mov eax, ebx
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, dword ptr [gKey_poll_counter]
            xor eax, eax
    L0048308c:
            cmp dword ptr [eax + gKey_array], ecx
            jle L004830a7
            cmp eax, 0x23c
            mov dword ptr [eax + gKey_array], ebp
            jle L004830a7
            mov dword ptr [eax + gMatchData_0074b384], ebx
    L004830a7:
            add eax, 4
            cmp eax, 0x25c
            jl L0048308c
            mov edx, 1
            mov ecx, offset gKey_array
            mov dword ptr [gKey_poll_counter], edx
            call PDSetKeyArray
            mov esi, dword ptr [gKey_poll_counter]
            mov ecx, 0x2c
            xor eax, eax
            mov edi, offset gKey_array+0x1ac
            rep stosd
            call PDReadJoysticks
            mov ecx, 8
            mov eax, ebx
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, 0x24
            xor eax, eax
            mov edi, offset gKey_array+0x1ac
            rep stosd
            cmp dword ptr [gINT_006621e4], ebp
            jne L0048310b
            cmp dword ptr [gINT_006621e8], ebp
            je L00483147
    L0048310b:
            call PDGetJoy1Button1
            test eax, eax
            je L0048311a
            mov dword ptr [gKey_array+0x1ac], esi
    L0048311a:
            call PDGetJoy1Button2
            test eax, eax
            je L00483129
            mov dword ptr [gKey_array+0x1b0], esi
    L00483129:
            call PDGetJoy1Button3
            test eax, eax
            je L00483138
            mov dword ptr [gKey_array+0x1b4], esi
    L00483138:
            call PDGetJoy1Button4
            test eax, eax
            je L00483147
            mov dword ptr [gKey_array+0x1b8], esi
    L00483147:
            cmp dword ptr [gINT_006621ec], ebp
            jne L00483157
            cmp dword ptr [gINT_006621f0], ebp
            je L00483193
    L00483157:
            call PDGetJoy2Button1
            test eax, eax
            je L00483166
            mov dword ptr [gKey_array+0x22c], esi
    L00483166:
            call PDGetJoy2Button2
            test eax, eax
            je L00483175
            mov dword ptr [gKey_array+0x230], esi
    L00483175:
            call PDGetJoy2Button3
            test eax, eax
            je L00483184
            mov dword ptr [gKey_array+0x234], esi
    L00483184:
            call PDGetJoy2Button4
            test eax, eax
            je L00483193
            mov dword ptr [gKey_array+0x238], esi
    L00483193:
            mov eax, dword ptr [gINT_006621e4]
            mov edi, 0x4000
            cmp eax, ebp
            mov ebx, 0xc000
            je L004831fe
            call PDGetJoy1X
            cmp eax, -1
            je L004831f9
            cmp eax, 0x8000
            jge L004831da
            cmp eax, edi
            jge L004831c9
            cmp dword ptr [gJoy1_x], edi
            jl L004831c9
            mov dword ptr [gKey_array+0x23c], esi
    L004831c9:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array], ecx
            jmp L004831f9
    L004831da:
            cmp eax, ebx
            jle L004831ec
            cmp dword ptr [gJoy1_x], ebx
            jg L004831ec
            mov dword ptr [gKey_array+0x240], esi
    L004831ec:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x4], edx
    L004831f9:
            mov dword ptr [gJoy1_x], eax
    L004831fe:
            cmp dword ptr [gINT_006621e8], ebp
            je L0048325e
            call PDGetJoy1Y
            cmp eax, -1
            je L00483259
            cmp eax, 0x8000
            jge L0048323a
            cmp eax, edi
            jge L00483229
            cmp dword ptr [gJoy1_y], edi
            jl L00483229
            mov dword ptr [gKey_array+0x244], esi
    L00483229:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x8], ecx
            jmp L00483259
    L0048323a:
            cmp eax, ebx
            jle L0048324c
            cmp dword ptr [gJoy1_y], ebx
            jg L0048324c
            mov dword ptr [gKey_array+0x248], esi
    L0048324c:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0xc], edx
    L00483259:
            mov dword ptr [gJoy1_y], eax
    L0048325e:
            cmp dword ptr [gINT_006621ec], ebp
            je L004832be
            call PDGetJoy2X
            cmp eax, -1
            je L004832b9
            cmp eax, 0x8000
            jge L0048329a
            cmp eax, edi
            jge L00483289
            cmp dword ptr [gJoy2_x], edi
            jl L00483289
            mov dword ptr [gKey_array+0x24c], esi
    L00483289:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x10], ecx
            jmp L004832b9
    L0048329a:
            cmp eax, ebx
            jle L004832ac
            cmp dword ptr [gJoy2_x], ebx
            jg L004832ac
            mov dword ptr [gKey_array+0x250], esi
    L004832ac:
            lea edx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x14], edx
    L004832b9:
            mov dword ptr [gJoy2_x], eax
    L004832be:
            cmp dword ptr [gINT_006621f0], ebp
            je L0048332e
            call PDGetJoy2Y
            cmp eax, -1
            je L00483329
            cmp eax, 0x8000
            jge L004832fa
            cmp eax, edi
            jge L004832e9
            cmp dword ptr [gJoy2_y], edi
            jl L004832e9
            mov dword ptr [gKey_array+0x254], esi
    L004832e9:
            mov ecx, 0x8000
            sub ecx, eax
            shl ecx, 1
            mov dword ptr [gJoy_array+0x18], ecx
            jmp L00483329
    L004832fa:
            mov edx, dword ptr [gJoystick_deadzone]
            add edx, 0x8000
            cmp eax, edx
            jle L00483329
            cmp eax, ebx
            jle L0048331c
            cmp dword ptr [gJoy2_y], ebx
            jg L0048331c
            mov dword ptr [gKey_array+0x258], esi
    L0048331c:
            lea ecx, [eax + eax - 0x10000]
            mov dword ptr [gJoy_array+0x1c], ecx
    L00483329:
            mov dword ptr [gJoy2_y], eax
    L0048332e:
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov esi, dword ptr [esp + 0x10]
            mov dword ptr [gLast_poll_keys], eax
    L00483346:
            cmp esi, -2
            jne L00483356
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00483356:
            cmp esi, -1
            jne L00483388
            mov eax, offset gGo_ahead_keys
    L00483360:
            mov edx, dword ptr [eax]
            cmp dword ptr [edx*4 + gKey_array], ebp
            jne L0048337d
            add eax, 4
            cmp eax, offset gLast_key_down
            jb L00483360
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L0048337d:
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00483388:
            mov eax, dword ptr [esi*4 + gKey_mapping]
            pop edi
            pop esi
            pop ebp
            mov eax, dword ptr [eax*4 + gKey_array]
            pop ebx
            pop ecx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// KeyIsDownNoMouldiness

// FUNCTION: CARMA2_HW 0x004833b0
void C2_NAKED C2_HOOK_FASTCALL WaitForNoKeys(void) {
#ifndef CARPOCALYPSE2_MATCHING

    do {
        CheckQuit();
    } while (AnyKeyDown() || EitherMouseButtonDown());
    CheckQuit();
#else
    __asm {
            sub esp, 0x30
            push ebx
            push ebp
            push esi
            or ebp, 0xffffffff
            push edi
            xor ebx, ebx
    L004833bc:
            call CheckQuit
            call PDGetTotalTime
            sub eax, dword ptr [gLast_poll_keys]
            cmp eax, 0x1f4
            jbe L00483459
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            rep stosd
            mov ecx, 8
            mov eax, ebp
            mov edi, offset gJoy_array
            rep stosd
            mov ecx, dword ptr [gKey_poll_counter]
            xor eax, eax
    L004833fb:
            cmp dword ptr [eax + gKey_array], ecx
            jle L00483416
            cmp eax, 0x23c
            mov dword ptr [eax + gKey_array], ebx
            jle L00483416
            mov dword ptr [eax + gMatchData_0074b384], ebp
    L00483416:
            add eax, 4
            cmp eax, 0x25c
            jl L004833fb
            mov edx, 1
            mov ecx, offset gKey_array
            mov dword ptr [gKey_poll_counter], edx
            call PDSetKeyArray
            mov edx, dword ptr [gKey_poll_counter]
            mov ecx, offset gKey_array
            call SetJoystickArrays
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov dword ptr [gLast_poll_keys], eax
    L00483459:
            mov edi, 0x8e
            mov esi, offset gKey_array+0x238
    L00483463:
            cmp dword ptr [esi], ebx
            je L0048348c
            cmp esi, offset gKey_array+0x10
            je L0048348c
            cmp dword ptr [gEdge_trigger_mode], ebx
            je L004834ae
            xor edx, edx
            mov ecx, edi
            call EdgeTriggeryKey
            cmp eax, 3
            ja L0048348c
            jmp dword ptr [eax*4 + 0x4835bc]
    L0048348c:
            sub esi, 4
            dec edi
            cmp esi, offset gKey_array
            jge L00483463
            cmp dword ptr [gEdge_trigger_mode], ebx
            je L004834ac
            mov edx, 1
            xor ecx, ecx
            call EdgeTriggeryKey
    L004834ac:
            mov edi, ebp
    L004834ae:
            cmp edi, ebp
            je L004834b7
            cmp edi, 4
            jne L00483522
    L004834b7:
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 0x14]
            lea ecx, [esp + 0x10]
            call GetMousePosition
            cmp esi, dword ptr [esp + 0x10]
            jne L00483505
            cmp edi, dword ptr [esp + 0x14]
            jne L00483505
            lea edx, [esp + 0x24]
            lea ecx, [esp + 0x20]
            call PDGetMouseClickPosition
            test eax, eax
            je L00483505
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x20]
            cmp eax, ecx
            jne L00483505
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x24]
            cmp ecx, eax
            je L00483522
    L00483505:
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x28]
            call PDMouseButtons
            cmp dword ptr [esp + 0x28], ebx
            jne L00483522
            cmp dword ptr [esp + 0x2c], ebx
            jne L00483522
            xor eax, eax
            jmp L00483527
    L00483522:
            mov eax, 1
    L00483527:
            cmp eax, ebx
            jne L004833bc
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x18]
            call GetMousePosition
            cmp esi, dword ptr [esp + 0x18]
            jne L00483584
            cmp edi, dword ptr [esp + 0x1c]
            jne L00483584
            lea edx, [esp + 0x34]
            lea ecx, [esp + 0x30]
            call PDGetMouseClickPosition
            test eax, eax
            je L00483584
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x30]
            cmp edx, eax
            jne L00483584
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x34]
            cmp eax, ecx
            jne L00483584
            mov eax, 2
            jmp L004835a6
    L00483584:
            lea edx, [esp + 0x3c]
            lea ecx, [esp + 0x38]
            call PDMouseButtons
            cmp dword ptr [esp + 0x38], ebx
            jne L004835a1
            cmp dword ptr [esp + 0x3c], ebx
            jne L004835a1
            xor eax, eax
            jmp L004835a6
    L004835a1:
            mov eax, 1
    L004835a6:
            cmp eax, ebx
            jne L004833bc
            call CheckQuit
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
            nop
            _emit 0xac
            _emit 0x34
            _emit 0x48
            _emit 0x00
            _emit 0xae
            _emit 0x34
            _emit 0x48
            _emit 0x00
            _emit 0xac
            _emit 0x34
            _emit 0x48
            _emit 0x00
            _emit 0xae
            _emit 0x34
            _emit 0x48
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

#pragma auto_inline(off)
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
    } else if (*pY > gGraf_specs[gGraf_spec_index].total_height) {
        *pY = gGraf_specs[gGraf_spec_index].total_height;
    }
    gCurrent_mouse_position.x = *pX;
    gLast_mouse_y_coord = *pY;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00483c90
void C2_HOOK_FASTCALL InitRollingLetters(void) {
    int i;

    gLast_roll = 0;
    gCurrent_cursor = -1;
    gRolling_letters = BrMemAllocate(NBR_ROLLING_LETTERS * sizeof(tRolling_letter), kMem_misc);
    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        gRolling_letters[i].number_of_letters = -1;
    }
}

// FUNCTION: CARMA2_HW 0x00483ce0
void C2_HOOK_FASTCALL EndRollingLetters(void) {
    BrMemFree(gRolling_letters);
}

// AddRollingLetter

// ChangeCharTo

// FUNCTION: CARMA2_HW 0x00483fd0
void C2_HOOK_FASTCALL BlankSlot(int pIndex, int pName_length, int pVisible_length) {
    int i;

    gVisible_length = pVisible_length;
    for (i = 0; i < pName_length; i++) {
        ChangeCharTo(pIndex, i, ' ');
    }
}

// FUNCTION: CARMA2_HW 0x00484120
void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str) {
    int i;

    for (i = 0; i < gThe_length; i++) {
        ChangeCharTo(pSlot_index, i, (char)((size_t)i < strlen(pRevert_str) ? pRevert_str[i] : ' '));
    }
}

// FUNCTION: CARMA2_HW 0x00484170
void C2_HOOK_FASTCALL StartTyping(int pSlot_index, char* pText, int pVisible_length) {

    gThe_length = pVisible_length;
    strcpy(gCurrent_typing, pText);
    gCurrent_position = (int)strlen(gCurrent_typing);
    gVisible_length = pVisible_length;
    gCurrent_cursor = ChangeCharTo(pSlot_index, gCurrent_position, ROLLING_LETTER_LOOP_RANDOM);
}

// FUNCTION: CARMA2_HW 0x00484510
void C2_HOOK_FASTCALL SetSlotXY(int pSlot_index, int pX_coord, int pY_coord) {
    gLetter_x_coords[pSlot_index] = pX_coord;
    gLetter_y_coords[pSlot_index] = pY_coord;
}

// FUNCTION: CARMA2_HW 0x00484530
void C2_HOOK_FASTCALL GetTypedName(char* pDestn, int pMax_length) {

    if (strlen(gCurrent_typing) > (size_t)pMax_length) {
        memcpy(pDestn, gCurrent_typing, pMax_length);
        pDestn[pMax_length] = '\0';
    } else {
        strcpy(pDestn, gCurrent_typing);
    }
}

// FUNCTION: CARMA2_HW 0x00484590
void C2_HOOK_FASTCALL KillCursor(int pSlot_index) {
    int x_coord;
    int y_coord;
    int j;
    tRolling_letter* let;

    if (gCurrent_position < gVisible_length && gCurrent_position >= 0) {
        y_coord = gLetter_y_coords[pSlot_index];
        x_coord = gCurrent_graf_data->rolling_letter_x_pitch * gCurrent_position + gLetter_x_coords[pSlot_index];
        for (let = gRolling_letters, j = 0; j < NBR_ROLLING_LETTERS; j++, let++) {
            if (let->number_of_letters >= 0 && x_coord == let->x_coord && y_coord == let->y_coord) {
                gRolling_letters[j].number_of_letters = -1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00484600
void C2_HOOK_FASTCALL EdgeTriggerModeOn(void) {

    gEdge_trigger_mode = 1;
}


// FUNCTION: CARMA2_HW 0x00484610
void C2_HOOK_FASTCALL EdgeTriggerModeOff(void) {

    gEdge_trigger_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004829f0
int C2_HOOK_FASTCALL FUN_004829f0(int pKey_index) {

    return gKey_array[pKey_index];
}

// FUNCTION: CARMA2_HW 0x00483e20
void C2_HOOK_FASTCALL AddRollingNumber(tU32 pNumber, int pWidth, int pX, int pY) {
    char the_string[32];
    unsigned int i;

    sprintf(the_string, "%0.*d", pWidth, pNumber);
    for (i = 0; i < strlen(the_string); i++) {
        AddRollingLetter(the_string[i], i * gCurrent_graf_data->rolling_letter_x_pitch + pX, pY, eRT_numeric);
    }
}

// FUNCTION: CARMA2_HW 0x004835d0
void C2_NAKED C2_HOOK_FASTCALL FUN_004835d0(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int bVar1;
    int iVar2;
    int iVar3;
    int iVar4;
    int uVar5;
    int* piVar6;
    int* puVar7;
    /* Shared stack slots in binary order, from local_30 through local_4. */
    int localVars[12];

    for (;;) {
        CheckQuit();
        iVar4 = PDGetTotalTime();
        if (500 < (tU32)(iVar4 - gLast_poll_keys)) {
            puVar7 = gKey_array;
            for (iVar4 = 0x97; iVar4 != 0; iVar4--) {
                *puVar7++ = 0;
            }
            puVar7 = gJoy_array;
            for (iVar4 = 8; iVar4 != 0; iVar4--) {
                *puVar7++ = -1;
            }
            CyclePollKeys();
            gKey_poll_counter++;
            PDSetKeyArray(gKey_array, gKey_poll_counter);
            SetJoystickArrays(gKey_array, gKey_poll_counter);
            PDSetKeysFromJoystick(gKey_array);
            gLast_poll_keys = PDGetTotalTime();
        }
        iVar4 = 0x8e;
        piVar6 = &gKey_array[0x8e];
        do {
            if (*piVar6 != 0 && piVar6 != &gKey_array[4]) {
                if (gEdge_trigger_mode == 0) {
                    goto key_found_first;
                }
                uVar5 = EdgeTriggeryKey(iVar4, 0);
                switch (uVar5) {
                case 0:
                case 2:
                    goto key_scan_next_first;
                case 1:
                case 3:
                    goto key_found_first;
                }
            }
key_scan_next_first:
            piVar6--;
            iVar4--;
        } while ((tS32)piVar6 >= (tS32)gKey_array);
        if (gEdge_trigger_mode != 0) {
            EdgeTriggeryKey(0, 1);
        }
        iVar4 = -1;
key_found_first:
        if (iVar4 != -1 && iVar4 != 4) {
            break;
        }
        iVar2 = gCurrent_mouse_position.x;
        iVar3 = gCurrent_mouse_position.y;
        GetMousePosition(&localVars[0], &localVars[1]);
        if (iVar2 == localVars[0] && iVar3 == localVars[1]) {
            iVar4 = PDGetMouseClickPosition(&localVars[4], &localVars[5]);
            if (iVar4 != 0 && localVars[0] == localVars[4] && localVars[1] == localVars[5]) {
                break;
            }
        }
        PDMouseButtons(&localVars[6], &localVars[7]);
        if (localVars[6] != 0 || localVars[7] != 0) {
            break;
        }
        iVar2 = gCurrent_mouse_position.x;
        iVar3 = gCurrent_mouse_position.y;
        GetMousePosition(&localVars[2], &localVars[3]);
        if (iVar2 == localVars[2] && iVar3 == localVars[3]) {
            iVar4 = PDGetMouseClickPosition(&localVars[8], &localVars[9]);
            if (iVar4 != 0 && localVars[2] == localVars[8] && localVars[3] == localVars[9]) {
                break;
            }
        }
        PDMouseButtons(&localVars[10], &localVars[11]);
        if (localVars[10] != 0 || localVars[11] != 0) {
            break;
        }
    }
    CheckQuit();
    do {
        CheckQuit();
        iVar4 = PDGetTotalTime();
        if (500 < (tU32)(iVar4 - gLast_poll_keys)) {
            puVar7 = gKey_array;
            for (iVar4 = 0x97; iVar4 != 0; iVar4--) {
                *puVar7++ = 0;
            }
            puVar7 = gJoy_array;
            for (iVar4 = 8; iVar4 != 0; iVar4--) {
                *puVar7++ = -1;
            }
            CyclePollKeys();
            PollKeys();
        }
        iVar4 = 0x8e;
        piVar6 = &gKey_array[0x8e];
        do {
            if (*piVar6 != 0 && piVar6 != &gKey_array[4]) {
                if (gEdge_trigger_mode == 0) {
                    goto key_found_second;
                }
                uVar5 = EdgeTriggeryKey(iVar4, 0);
                switch (uVar5) {
                case 0:
                case 2:
                    goto key_scan_next_second;
                case 1:
                case 3:
                    goto key_found_second;
                }
            }
key_scan_next_second:
            piVar6--;
            iVar4--;
        } while ((tS32)piVar6 >= (tS32)gKey_array);
        if (gEdge_trigger_mode != 0) {
            EdgeTriggeryKey(0, 1);
        }
        iVar4 = -1;
key_found_second:
        if (iVar4 == -1 || iVar4 == 4) {
            iVar2 = gCurrent_mouse_position.x;
            iVar3 = gCurrent_mouse_position.y;
            GetMousePosition(&localVars[3], &localVars[2]);
            if (iVar2 == localVars[3] && iVar3 == localVars[2]) {
                iVar4 = PDGetMouseClickPosition(&localVars[11], &localVars[10]);
                if (iVar4 != 0 && localVars[3] == localVars[11] && localVars[2] == localVars[10]) {
                    goto mouse_activity_found;
                }
            }
            PDMouseButtons(&localVars[9], &localVars[8]);
            if (localVars[9] != 0 || localVars[8] != 0) {
                goto mouse_activity_found;
            }
            bVar1 = 0;
        } else {
mouse_activity_found:
            bVar1 = 1;
        }
        if (!bVar1) {
            iVar2 = gCurrent_mouse_position.x;
            iVar3 = gCurrent_mouse_position.y;
            GetMousePosition(&localVars[1], &localVars[0]);
            if (iVar2 == localVars[1] && iVar3 == localVars[0] &&
                    (iVar4 = PDGetMouseClickPosition(&localVars[7], &localVars[6]), iVar4 != 0 &&
                     localVars[1] == localVars[7] && localVars[0] == localVars[6])) {
                iVar4 = 2;
            } else {
                PDMouseButtons(&localVars[5], &localVars[4]);
                if (localVars[5] == 0 && localVars[4] == 0) {
                    iVar4 = 0;
                } else {
                    iVar4 = 1;
                }
            }
            if (iVar4 == 0) {
                CheckQuit();
                return;
            }
        }
    } while (1);
#else
    __asm {
            sub esp, 0x30
            push esi
            push edi
    L004835d5:
            call CheckQuit
            call PDGetTotalTime
            sub eax, dword ptr [gLast_poll_keys]
            cmp eax, 0x1f4
            jbe L00483649
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            rep stosd
            mov ecx, 8
            or eax, 0xffffffff
            mov edi, offset gJoy_array
            rep stosd
            call CyclePollKeys
            mov edx, dword ptr [gKey_poll_counter]
            mov ecx, offset gKey_array
            inc edx
            mov dword ptr [gKey_poll_counter], edx
            call PDSetKeyArray
            mov edx, dword ptr [gKey_poll_counter]
            mov ecx, offset gKey_array
            call SetJoystickArrays
            mov ecx, offset gKey_array
            call PDSetKeysFromJoystick
            call PDGetTotalTime
            mov dword ptr [gLast_poll_keys], eax
    L00483649:
            mov edi, 0x8e
            mov esi, offset gKey_array+0x238
    L00483653:
            cmp dword ptr [esi], 0
            je L0048367e
            cmp esi, offset gKey_array+0x10
            je L0048367e
            mov eax, dword ptr [gEdge_trigger_mode]
            test eax, eax
            je L004836a2
            xor edx, edx
            mov ecx, edi
            call EdgeTriggeryKey
            cmp eax, 3
            ja L0048367e
            jmp dword ptr [eax*4 + 0x483950]
    L0048367e:
            sub esi, 4
            dec edi
            cmp esi, offset gKey_array
            jge L00483653
            mov eax, dword ptr [gEdge_trigger_mode]
            test eax, eax
            je L0048369f
            mov edx, 1
            xor ecx, ecx
            call EdgeTriggeryKey
    L0048369f:
            or edi, 0xffffffff
    L004836a2:
            cmp edi, -1
            je L004836b0
            cmp edi, 4
            jne L0048379f
    L004836b0:
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 0xc]
            lea ecx, [esp + 8]
            call GetMousePosition
            cmp esi, dword ptr [esp + 8]
            jne L00483702
            cmp edi, dword ptr [esp + 0xc]
            jne L00483702
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x18]
            call PDGetMouseClickPosition
            test eax, eax
            je L00483702
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0x18]
            cmp eax, ecx
            jne L00483702
            mov ecx, dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 0x1c]
            cmp ecx, eax
            je L0048379f
    L00483702:
            lea edx, [esp + 0x24]
            lea ecx, [esp + 0x20]
            call PDMouseButtons
            mov eax, dword ptr [esp + 0x20]
            test eax, eax
            jne L0048379f
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            jne L0048379f
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 0x14]
            lea ecx, [esp + 0x10]
            call GetMousePosition
            cmp esi, dword ptr [esp + 0x10]
            jne L00483771
            cmp edi, dword ptr [esp + 0x14]
            jne L00483771
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x28]
            call PDGetMouseClickPosition
            test eax, eax
            je L00483771
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x28]
            cmp edx, eax
            jne L00483771
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x2c]
            cmp eax, ecx
            je L0048379f
    L00483771:
            lea edx, [esp + 0x34]
            lea ecx, [esp + 0x30]
            call PDMouseButtons
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            jne L00483792
            mov eax, dword ptr [esp + 0x34]
            test eax, eax
            je L004835d5
    L00483792:
            mov eax, 1
            test eax, eax
            je L004835d5
    L0048379f:
            call CheckQuit
    L004837a4:
            call CheckQuit
            call PDGetTotalTime
            sub eax, dword ptr [gLast_poll_keys]
            cmp eax, 0x1f4
            jbe L004837e2
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gKey_array
            rep stosd
            mov ecx, 8
            or eax, 0xffffffff
            mov edi, offset gJoy_array
            rep stosd
            call CyclePollKeys
            call PollKeys
    L004837e2:
            mov edi, 0x8e
            mov esi, offset gKey_array+0x238
    L004837ec:
            cmp dword ptr [esi], 0
            je L00483817
            cmp esi, offset gKey_array+0x10
            je L00483817
            mov eax, dword ptr [gEdge_trigger_mode]
            test eax, eax
            je L0048383b
            xor edx, edx
            mov ecx, edi
            call EdgeTriggeryKey
            cmp eax, 3
            ja L00483817
            jmp dword ptr [eax*4 + 0x483960]
    L00483817:
            sub esi, 4
            dec edi
            cmp esi, offset gKey_array
            jge L004837ec
            mov eax, dword ptr [gEdge_trigger_mode]
            test eax, eax
            je L00483838
            mov edx, 1
            xor ecx, ecx
            call EdgeTriggeryKey
    L00483838:
            or edi, 0xffffffff
    L0048383b:
            cmp edi, -1
            je L00483845
            cmp edi, 4
            jne L004838b4
    L00483845:
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 0x10]
            lea ecx, [esp + 0x14]
            call GetMousePosition
            cmp esi, dword ptr [esp + 0x14]
            jne L00483893
            cmp edi, dword ptr [esp + 0x10]
            jne L00483893
            lea edx, [esp + 0x30]
            lea ecx, [esp + 0x34]
            call PDGetMouseClickPosition
            test eax, eax
            je L00483893
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x34]
            cmp ecx, eax
            jne L00483893
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x30]
            cmp edx, eax
            je L004838b4
    L00483893:
            lea edx, [esp + 0x28]
            lea ecx, [esp + 0x2c]
            call PDMouseButtons
            mov eax, dword ptr [esp + 0x2c]
            test eax, eax
            jne L004838b4
            mov eax, dword ptr [esp + 0x28]
            test eax, eax
            jne L004838b4
            xor eax, eax
            jmp L004838b9
    L004838b4:
            mov eax, 1
    L004838b9:
            test eax, eax
            jne L004837a4
            mov esi, dword ptr [gCurrent_mouse_position]
            mov edi, dword ptr [gLast_mouse_y_coord]
            lea edx, [esp + 8]
            lea ecx, [esp + 0xc]
            call GetMousePosition
            cmp esi, dword ptr [esp + 0xc]
            jne L00483916
            cmp edi, dword ptr [esp + 8]
            jne L00483916
            lea edx, [esp + 0x20]
            lea ecx, [esp + 0x24]
            call PDGetMouseClickPosition
            test eax, eax
            je L00483916
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 0x24]
            cmp eax, ecx
            jne L00483916
            mov ecx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 0x20]
            cmp ecx, eax
            jne L00483916
            mov eax, 2
            jmp L0048393c
    L00483916:
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x1c]
            call PDMouseButtons
            mov eax, dword ptr [esp + 0x1c]
            test eax, eax
            jne L00483937
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            jne L00483937
            xor eax, eax
            jmp L0048393c
    L00483937:
            mov eax, 1
    L0048393c:
            test eax, eax
            jne L004837a4
            call CheckQuit
            pop edi
            pop esi
            add esp, 0x30
            ret
            nop
            _emit 0x9f
            _emit 0x36
            _emit 0x48
            _emit 0x00
            _emit 0xa2
            _emit 0x36
            _emit 0x48
            _emit 0x00
            _emit 0x9f
            _emit 0x36
            _emit 0x48
            _emit 0x00
            _emit 0xa2
            _emit 0x36
            _emit 0x48
            _emit 0x00
            _emit 0x38
            _emit 0x38
            _emit 0x48
            _emit 0x00
            _emit 0x3b
            _emit 0x38
            _emit 0x48
            _emit 0x00
            _emit 0x38
            _emit 0x38
            _emit 0x48
            _emit 0x00
            _emit 0x3b
            _emit 0x38
            _emit 0x48
            _emit 0x00
            _emit 0x51
            _emit 0x53
            _emit 0x55
            _emit 0x56
            _emit 0x57
            _emit 0x89
            _emit 0x54
            _emit 0x24
            _emit 0x10
            _emit 0x8b
            _emit 0xf1
            _emit 0xe8
            _emit 0x90
            _emit 0x9a
            _emit 0x09
            _emit 0x00
            _emit 0x8b
            _emit 0x15
            _emit 0xc8
            _emit 0xc1
            _emit 0x68
            _emit 0x00
            _emit 0x83
            _emit 0xcd
            _emit 0xff
            _emit 0x2b
            _emit 0xc2
            _emit 0x33
            _emit 0xdb
            _emit 0x3d
            _emit 0xf4
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x0f
            _emit 0x86
            _emit 0x82
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xb9
            _emit 0x97
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x33
            _emit 0xc0
            _emit 0xbf
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xf3
            _emit 0xab
            _emit 0xb9
            _emit 0x08
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x8b
            _emit 0xc5
            _emit 0xbf
            _emit 0xc0
            _emit 0xb5
            _emit 0x74
            _emit 0x00
            _emit 0xf3
            _emit 0xab
            _emit 0x8b
            _emit 0x0d
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x33
            _emit 0xc0
            _emit 0x39
            _emit 0x88
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x7e
            _emit 0x13
            _emit 0x3d
            _emit 0x3c
            _emit 0x02
            _emit 0x00
            _emit 0x00
            _emit 0x89
            _emit 0x98
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x7e
            _emit 0x06
            _emit 0x89
            _emit 0xa8
            _emit 0x84
            _emit 0xb3
            _emit 0x74
            _emit 0x00
            _emit 0x83
            _emit 0xc0
            _emit 0x04
            _emit 0x3d
            _emit 0x5c
            _emit 0x02
            _emit 0x00
            _emit 0x00
            _emit 0x7c
            _emit 0xdb
            _emit 0xba
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x89
            _emit 0x15
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0xfa
            _emit 0x94
            _emit 0x09
            _emit 0x00
            _emit 0x8b
            _emit 0x15
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0x6a
            _emit 0xed
            _emit 0xff
            _emit 0xff
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0x50
            _emit 0x79
            _emit 0xfd
            _emit 0xff
            _emit 0xe8
            _emit 0xfb
            _emit 0x99
            _emit 0x09
            _emit 0x00
            _emit 0xa3
            _emit 0xc8
            _emit 0xc1
            _emit 0x68
            _emit 0x00
            _emit 0x83
            _emit 0xfe
            _emit 0xfe
            _emit 0x75
            _emit 0x07
            _emit 0xb8
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xeb
            _emit 0x37
            _emit 0x3b
            _emit 0xf5
            _emit 0x75
            _emit 0x25
            _emit 0xb8
            _emit 0x00
            _emit 0x72
            _emit 0x65
            _emit 0x00
            _emit 0x8b
            _emit 0x08
            _emit 0x39
            _emit 0x1c
            _emit 0x8d
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x75
            _emit 0x0e
            _emit 0x83
            _emit 0xc0
            _emit 0x04
            _emit 0x3d
            _emit 0x0c
            _emit 0x72
            _emit 0x65
            _emit 0x00
            _emit 0x72
            _emit 0xeb
            _emit 0x33
            _emit 0xc0
            _emit 0xeb
            _emit 0x15
            _emit 0xb8
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xeb
            _emit 0x0e
            _emit 0x8b
            _emit 0x14
            _emit 0xb5
            _emit 0xe0
            _emit 0xb5
            _emit 0x74
            _emit 0x00
            _emit 0x8b
            _emit 0x04
            _emit 0x95
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x3b
            _emit 0xc3
            _emit 0x0f
            _emit 0x85
            _emit 0x95
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0xe8
            _emit 0xa6
            _emit 0x99
            _emit 0x09
            _emit 0x00
            _emit 0x2b
            _emit 0x05
            _emit 0xc8
            _emit 0xc1
            _emit 0x68
            _emit 0x00
            _emit 0x3d
            _emit 0xf4
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x0f
            _emit 0x86
            _emit 0x82
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xb9
            _emit 0x97
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x33
            _emit 0xc0
            _emit 0xbf
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xf3
            _emit 0xab
            _emit 0xb9
            _emit 0x08
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x8b
            _emit 0xc5
            _emit 0xbf
            _emit 0xc0
            _emit 0xb5
            _emit 0x74
            _emit 0x00
            _emit 0xf3
            _emit 0xab
            _emit 0x8b
            _emit 0x0d
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x33
            _emit 0xc0
            _emit 0x39
            _emit 0x88
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x7e
            _emit 0x13
            _emit 0x3d
            _emit 0x3c
            _emit 0x02
            _emit 0x00
            _emit 0x00
            _emit 0x89
            _emit 0x98
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x7e
            _emit 0x06
            _emit 0x89
            _emit 0xa8
            _emit 0x84
            _emit 0xb3
            _emit 0x74
            _emit 0x00
            _emit 0x83
            _emit 0xc0
            _emit 0x04
            _emit 0x3d
            _emit 0x5c
            _emit 0x02
            _emit 0x00
            _emit 0x00
            _emit 0x7c
            _emit 0xdb
            _emit 0xba
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x89
            _emit 0x15
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0x17
            _emit 0x94
            _emit 0x09
            _emit 0x00
            _emit 0x8b
            _emit 0x15
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0x87
            _emit 0xec
            _emit 0xff
            _emit 0xff
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0x6d
            _emit 0x78
            _emit 0xfd
            _emit 0xff
            _emit 0xe8
            _emit 0x18
            _emit 0x99
            _emit 0x09
            _emit 0x00
            _emit 0xa3
            _emit 0xc8
            _emit 0xc1
            _emit 0x68
            _emit 0x00
            _emit 0xa1
            _emit 0xfc
            _emit 0xb5
            _emit 0x74
            _emit 0x00
            _emit 0x39
            _emit 0x1c
            _emit 0x85
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x0f
            _emit 0x84
            _emit 0xe3
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xe8
            _emit 0xfc
            _emit 0x98
            _emit 0x09
            _emit 0x00
            _emit 0x2b
            _emit 0x05
            _emit 0xc8
            _emit 0xc1
            _emit 0x68
            _emit 0x00
            _emit 0x3d
            _emit 0xf4
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x0f
            _emit 0x86
            _emit 0x82
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xb9
            _emit 0x97
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x33
            _emit 0xc0
            _emit 0xbf
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xf3
            _emit 0xab
            _emit 0xb9
            _emit 0x08
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x8b
            _emit 0xc5
            _emit 0xbf
            _emit 0xc0
            _emit 0xb5
            _emit 0x74
            _emit 0x00
            _emit 0xf3
            _emit 0xab
            _emit 0x8b
            _emit 0x0d
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x33
            _emit 0xc0
            _emit 0x39
            _emit 0x88
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x7e
            _emit 0x13
            _emit 0x3d
            _emit 0x3c
            _emit 0x02
            _emit 0x00
            _emit 0x00
            _emit 0x89
            _emit 0x98
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x7e
            _emit 0x06
            _emit 0x89
            _emit 0xa8
            _emit 0x84
            _emit 0xb3
            _emit 0x74
            _emit 0x00
            _emit 0x83
            _emit 0xc0
            _emit 0x04
            _emit 0x3d
            _emit 0x5c
            _emit 0x02
            _emit 0x00
            _emit 0x00
            _emit 0x7c
            _emit 0xdb
            _emit 0xba
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x89
            _emit 0x15
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0x6d
            _emit 0x93
            _emit 0x09
            _emit 0x00
            _emit 0x8b
            _emit 0x15
            _emit 0xd4
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0xdd
            _emit 0xeb
            _emit 0xff
            _emit 0xff
            _emit 0xb9
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0xe8
            _emit 0xc3
            _emit 0x77
            _emit 0xfd
            _emit 0xff
            _emit 0xe8
            _emit 0x6e
            _emit 0x98
            _emit 0x09
            _emit 0x00
            _emit 0xa3
            _emit 0xc8
            _emit 0xc1
            _emit 0x68
            _emit 0x00
            _emit 0x8b
            _emit 0x44
            _emit 0x24
            _emit 0x10
            _emit 0x83
            _emit 0xf8
            _emit 0xfe
            _emit 0x75
            _emit 0x07
            _emit 0xb8
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xeb
            _emit 0x37
            _emit 0x3b
            _emit 0xc5
            _emit 0x75
            _emit 0x25
            _emit 0xb8
            _emit 0x00
            _emit 0x72
            _emit 0x65
            _emit 0x00
            _emit 0x8b
            _emit 0x08
            _emit 0x39
            _emit 0x1c
            _emit 0x8d
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x75
            _emit 0x0e
            _emit 0x83
            _emit 0xc0
            _emit 0x04
            _emit 0x3d
            _emit 0x0c
            _emit 0x72
            _emit 0x65
            _emit 0x00
            _emit 0x72
            _emit 0xeb
            _emit 0x33
            _emit 0xc0
            _emit 0xeb
            _emit 0x15
            _emit 0xb8
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0xeb
            _emit 0x0e
            _emit 0x8b
            _emit 0x14
            _emit 0x85
            _emit 0xe0
            _emit 0xb5
            _emit 0x74
            _emit 0x00
            _emit 0x8b
            _emit 0x04
            _emit 0x95
            _emit 0xe0
            _emit 0xbe
            _emit 0x68
            _emit 0x00
            _emit 0x3b
            _emit 0xc3
            _emit 0x75
            _emit 0x08
            _emit 0x33
            _emit 0xc0
            _emit 0x5f
            _emit 0x5e
            _emit 0x5d
            _emit 0x5b
            _emit 0x59
            _emit 0xc3
            _emit 0x5f
            _emit 0x5e
            _emit 0x5d
            _emit 0xb8
            _emit 0x01
            _emit 0x00
            _emit 0x00
            _emit 0x00
            _emit 0x5b
            _emit 0x59
            _emit 0xc3
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00483e90
void C2_HOOK_FASTCALL InputNoOp(void) {
}

// FUNCTION: CARMA2_HW 0x004840d0
void C2_HOOK_FASTCALL FUN_004840d0(int pSlot_index) {
    unsigned i;

    i = 0;
    if (0 < gThe_length) {
        do {
            ChangeCharTo(pSlot_index, i, i < strlen(gCurrent_typing) ? gCurrent_typing[i] : ' ');
            i += 1;
        } while ((int)i < gThe_length);
    }
}
