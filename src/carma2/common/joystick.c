#include "joystick.h"

#include "controls.h"
#include "displays.h"
#include "globvars.h"
#include "input.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include <math.h>

#include "c2_string.h"
#include "joystick.h"

#include "loading1.h"
#include "utility.h"
#include "platform.h"
#include "globvars.h"
#include "packfile.h"
#include "input.h"
#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
#include "c2_string.h"
#include "c2_stdio.h"
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
#include "win32/win32_dinput.h"
#endif
#include <string.h>

#include <stdio.h>
extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pText);
extern void C2_HOOK_FASTCALL FUN_0045a0c0(void);
extern const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void);
extern int C2_HOOK_FASTCALL PDIsJoystickAvailable(int pIndex);
extern int C2_HOOK_FASTCALL InsertHeadupText(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSortAlphabetically);

// GLOBAL: CARMA2_HW 0x00595f84
int gUNK_00595f84 = 7500;

// GLOBAL: CARMA2_HW 0x00595f88
int gJoystick_index = -1;

// GLOBAL: CARMA2_HW 0x00595f8c
int gUNK_00595f8c = -1;

// GLOBAL: CARMA2_HW 0x00596310
int gUNK_00596310 = 50;

// GLOBAL: CARMA2_HW 0x00595f90
float gJoystick_x_steering = 1.f;

// GLOBAL: CARMA2_HW 0x00595f94
float gJoystick_y_throttle = 1.f;

// GLOBAL: CARMA2_HW 0x00595f9c
int gUNK_00595f9c = 1;

// GLOBAL: CARMA2_HW 0x00686184
int gJoystick_preferences_backup_valid;

// GLOBAL: CARMA2_HW 0x0079d8c0
tButtonJoystickInfo gJoystick_preferences_backup;

typedef struct tJoystick_preference_node {
    tButtonJoystickInfo* info;
    struct tJoystick_preference_node* next;
} tJoystick_preference_node;


// GLOBAL: CARMA2_HW 0x00655e5c
float Force_feedback_lower_limit = 1.5f;

// GLOBAL: CARMA2_HW 0x00655e58
float gForce_feedback_upper_limit = 2.7f;

// GLOBAL: CARMA2_HW 0x00589f80
const double gJoystick_collision_force_scale = 10000.0;

// GLOBAL: CARMA2_HW 0x00589f88
const float gJoystick_collision_force_min_length = 0.0000002384185791015625f;

// GLOBAL: CARMA2_HW 0x00589f90
const double gJoystick_collision_force_one = 1.0;

// GLOBAL: CARMA2_HW 0x00589f98
const float gJoystick_collision_force_one_f = 1.0f;

// GLOBAL: CARMA2_HW 0x00589f9c
const float gJoystick_collision_force_zero_f = 0.0f;

// GLOBAL: CARMA2_HW 0x00589fa0
const double gJoystick_collision_force_radians_to_degrees = 57.29577951308;

// GLOBAL: CARMA2_HW 0x0079d8a0
tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;

// GLOBAL: CARMA2_HW 0x00596250
tJoystick_headup_text gJoystick_headup_texts[21] = {
    { "Recover", 60 },
    { "Reapir", 45 },
    { "Hand Brake", 54 },
    { "Use Powerup", 61 },
    { "Next Powerup", 64 },
    { "Prev Powerup", 63 },
    { "Wheel Spin", 56 },
    { "Map", 57 },
    { "Cockpit View", 46 },
    { "Buy Armour", 67 },
    { "Buy Power", 68 },
    { "Buy Offensive", 69 },
    { "Target Lock", 72 },
    { "Cycle Target", 73 },
    { "Camera Mode", 75 },
    { "Doors", 74 },
    { "Horn", 59 },
    { "Accelerate", 49 },
    { "Brake", 50 },
    { "Left", 47 },
    { "Right", 48 },
};

// GLOBAL: CARMA2_HW 0x00688700
int gOrig_joystick_index;

extern int gINT_0068b8e4;
extern int gINT_0068b8e8;
extern int gRace_head_ups[22];

float gOriginal_joystick_x;
float gOriginal_joystick_y;
int gOriginal_joystick_fbb;
int gOriginal_joystick_dpad;
// GLOBAL: CARMA2_HW 0x00596308
int gINT_00596308 = -1;

// GLOBAL: CARMA2_HW 0x0059630c
int gCurrent_ffb_effect_index = -1;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(long pMagnitude, long pDirection);
extern char _CIacos[];
extern void __cdecl _ftol();
extern br_token_value gAccent_poly_prims[3];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL DRstrlwr(char* s);
extern tButtonJoystickInfo* C2_HOOK_FASTCALL PDGetCurrentJoystickData(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL StringTransformToLower(char* pStr);
extern int gCancel_recover;
extern int gDisable_sound;
#endif
// FUNCTION: CARMA2_HW 0x0045b7e0
int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name) {
    fpos_t position;
    char joystick_name[256];
    char s[256];
    long start;
    size_t i;

    rewind(pF);
    start = ftell(pF);
    fseek(pF, 0, SEEK_END);
    if (ftell(pF) - start < 32) {
        return -1;
    }
    strcpy(joystick_name, pJoystick_name);
    for (i = 0; i < strlen(joystick_name); i++) {
        if (joystick_name[i] == ' ') {
            joystick_name[i] = '_';
        }
    }
    rewind(pF);
    while (!feof(pF)) {
        if (fgetpos(pF, &position) != 0) {
            PDFatalError("getpos error");
        }
        GetAString(pF, s);
        if (strstr(s, joystick_name) != NULL) {
            if (fsetpos(pF, &position) != 0) {
                PDFatalError("setpos error");
            }
            return 1;
        }
    }
    rewind(pF);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045b920
int C2_HOOK_FASTCALL WriteJoystickPreferences(FILE* pF, const tButtonJoystickInfo* pInfo) {
    char s[256];
    char joystick_name[256];
    size_t i;

    strcpy(joystick_name, pInfo->productName);
    for (i = 0; i < strlen(joystick_name); i++) {
        if (joystick_name[i] == ' ') {
            joystick_name[i] = '_';
        }
    }
    sprintf(s, "&%s\n", joystick_name);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", 32);
    fwrite(s, 1, strlen(s), pF);
    for (i = 0; i < 32; i++) {
        sprintf(s, "%d\n", pInfo->buttons[i]);
        fwrite(s, 1, strlen(s), pF);
    }
    sprintf(s, "%d\n", pInfo->field_0xd4);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%.3f, %.3f\n", pInfo->field_0xd8, pInfo->field_0xdc);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe0);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe4);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe8);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "\n");
    fwrite(s, 1, strlen(s), pF);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045bbc0
int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo) {
    char s[256];
    size_t count_buttons;
    size_t i;

    GetAString(pF, s);
    for (i = 0; i < strlen(s); i++) {
        if (s[i] == '_') {
            s[i] = ' ';
        }
    }
    strcpy(pInfo->productName, s + 1);
    count_buttons = GetAnInt(pF);
    for (i = 0; i < 32; i++) {
        if (i < count_buttons) {
            pInfo->buttons[i] = GetAnInt(pF);
        } else {
            pInfo->buttons[i] = -1;
        }
    }
    pInfo->field_0xd4 = GetAnInt(pF);
    GetPairOfScalars(pF, &pInfo->field_0xd8, &pInfo->field_0xdc);
    pInfo->field_0xe0 = GetAnInt(pF);
    pInfo->field_0xe4 = GetAnInt(pF);
    pInfo->field_0xe8 = GetAnInt(pF);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045bcc0
int C2_HOOK_FASTCALL CountJoystickPreferences(FILE* pF) {
    char s[256];
    int count = 0;
    long start;

    rewind(pF);
    start = ftell(pF);
    fseek(pF, 0, SEEK_END);
    if (ftell(pF) - start < 32) {
        return 0;
    }
    rewind(pF);
    while (!feof(pF)) {
        GetAString(pF, s);
        if (s[0] == '&') {
            count++;
        }
    }
    rewind(pF);
    return count;
}

// FUNCTION: CARMA2_HW 0x0045c0b0
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {
    tPath_name path;
    tButtonJoystickInfo* info;
    FILE* f;

    info = PDGetCurrentJoystickData();
    if (info == NULL) {
        return 0;
    }
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "rt");
    if (f == NULL) {
        return 0;
    }
    if (FindJoystickPreferences(f, GetCurrentJoystickName()) > 0) {
        ReadJoystickPreferences(f, info);
        fclose(f);
    } else {
        fclose(f);
        remove(path);
    }
    gJoystick_x_steering = info->field_0xd8;
    gJoystick_y_throttle = info->field_0xdc;
    gUNK_00595f9c = info->field_0xd4;
    gUNK_00595f84 = info->field_0xe4;
    gUNK_00596310 += 25;
    if (gUNK_00596310 > 100) {
        gUNK_00596310 = 0;
    }
    SetAllFFBEffectsGain(gUNK_00596310);
    if (info->field_0xe8 != 0) {
        EnableJoysticks();
    } else {
        DisableJoysticks();
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045bd90
int C2_HOOK_FASTCALL SaveAllJoystickData(void) {
    tPath_name path;
    char error[256];
    int original_index;
    int count;
    int i;
    int result;
    FILE* f;
    tJoystick_preference_node* list;
    tJoystick_preference_node* node;
    tJoystick_preference_node* tail;
    tJoystick_preference_node* match;
    tButtonJoystickInfo* info;

    original_index = gJoystick_index;
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "rt");
    if (f == NULL) {
        f = fopen(path, "wt");
        if (f != NULL) {
            for (i = 0; i < 16; i++) {
                if (PDIsJoystickAvailable(i)) {
                    gJoystick_index = i;
                    info = PDGetCurrentJoystickData();
                    if (info != NULL) {
                        WriteJoystickPreferences(f, info);
                    }
                }
            }
            fclose(f);
        }
        gJoystick_index = original_index;
        return 1;
    }
    count = CountJoystickPreferences(f);
    if (count == 0) {
        return 1;
    }
    list = malloc(sizeof(*list));
    if (list == NULL) {
        fclose(f);
        gJoystick_index = original_index;
        return 0;
    }
    node = list;
    for (i = 0; i < count; i++) {
        node->next = NULL;
        node->info = malloc(sizeof(*node->info));
        if (node->info != NULL) {
            ReadJoystickPreferences(f, node->info);
        }
        if (i != count - 1) {
            node->next = malloc(sizeof(*node));
            node = node->next;
        }
        tail = node;
    }
    fclose(f);
    for (i = 0; i < 16; i++) {
        if (PDIsJoystickAvailable(i)) {
            gJoystick_index = i;
            info = PDGetCurrentJoystickData();
            if (info == NULL) {
                continue;
            }
            match = NULL;
            for (node = list; node != NULL; node = node->next) {
                if (match != NULL) {
                    break;
                }
                if (strcmp(node->info->productName, GetCurrentJoystickName()) == 0) {
                    match = node;
                }
            }
            if (match != NULL) {
                *match->info = *info;
            } else {
                node = malloc(sizeof(*node));
                if (node == NULL) {
                    return 0;
                }
                tail->next = node;
                tail = node;
                node->info = malloc(sizeof(*node->info));
                if (node->info == NULL) {
                    return 0;
                }
                node->next = NULL;
                *node->info = *info;
            }
        }
    }
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "wt");
    if (f == NULL) {
        sprintf(error, "Cant open %s", path);
        return 0;
    }
    node = list;
    do {
        if (node->info != NULL) {
            WriteJoystickPreferences(f, node->info);
        }
        tail = node;
        node = node->next;
        if (tail->info != NULL) {
            free(tail->info);
        }
        if (tail != NULL) {
            free(tail);
        }
    } while (node != NULL);
    result = fclose(f);
    gJoystick_index = original_index;
    return result;
}

// FUNCTION: CARMA2_HW 0x0045c1e0
void C2_HOOK_FASTCALL BackupJoystickPreferences(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        gJoystick_preferences_backup = *info;
        gJoystick_preferences_backup_valid = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0045c210
void C2_HOOK_FASTCALL RestoreJoystickPreferences(void) {
    tButtonJoystickInfo* info;

    if (gJoystick_preferences_backup_valid) {
        info = PDGetCurrentJoystickData();
        if (info != NULL) {
            *info = gJoystick_preferences_backup;
        }
    }
}

/* ported from common/joystick.c */
void C2_HOOK_FASTCALL SetupFFBValues(void) {
    tPath_name path;
    FILE *f;
    char s[256];

    PathCat(path, gApplication_path, "FFB.TXT");
    f = DRfopen(path, "rt");
    GetALineAndDontArgue(f, s);
    sscanf(strtok(s, "\t ,/"), "%f", &gForce_feedback_upper_limit);
    GetALineAndDontArgue(f, s);
    sscanf(strtok(s, "\t ,/"), "%f", &Force_feedback_lower_limit);
    PFfclose(f);
}

void C2_HOOK_FASTCALL InitJoysticks(void) {

    PDInitJoysticks();
}

typedef struct {
    const char* name;
    int key;
} tJoystick_action_entry;

// GLOBAL: CARMA2_HW 0x00596250
tJoystick_action_entry gJoystick_actions[21] = {
    { "Recover", 0x3c },
    { "Reapir", 0x2d },
    { "Hand Brake", 0x36 },
    { "Use Powerup", 0x3d },
    { "Next Powerup", 0x40 },
    { "Prev Powerup", 0x3f },
    { "Wheel Spin", 0x38 },
    { "Map", 0x39 },
    { "Cockpit View", 0x2e },
    { "Buy Armour", 0x43 },
    { "Buy Power", 0x44 },
    { "Buy Offensive", 0x45 },
    { "Target Lock", 0x48 },
    { "Cycle Target", 0x49 },
    { "Camera Mode", 0x4b },
    { "Doors", 0x4a },
    { "Horn", 0x3b },
    { "Accelerate", 0x31 },
    { "Brake", 0x32 },
    { "Left", 0x2f },
    { "Right", 0x30 },
};

// FUNCTION: CARMA2_HW 0x0045c8d0
float C2_HOOK_FASTCALL GetJoystickX(void) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        return joystick_info->field_0xd8;
    }
    return gJoystick_x_steering;
}

// FUNCTION: CARMA2_HW 0x0045c8f0
float C2_HOOK_FASTCALL GetJoystickY(void) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        return joystick_info->field_0xdc;
    }
    return gJoystick_y_throttle;
}

// FUNCTION: CARMA2_HW 0x0045c910
void C2_NAKED C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, const br_vector3* pPosition, br_vector3* pForce, float pMagnitude) {
#ifndef CARPOCALYPSE2_MATCHING
    float force_length;
    float inverse_force_length;
    br_vector3 normalized_position;
    register int angle_degrees;
    int magnitude;
    float force_x;
    float force_y;
    float force_z;

    (void)pForce;
    (void)pCar;

    if (pMagnitude > gForce_feedback_upper_limit) {
        pMagnitude = gForce_feedback_upper_limit;
    }
    if (pMagnitude < Force_feedback_lower_limit) {
        pMagnitude = Force_feedback_lower_limit;
    }
    magnitude = (int)(pMagnitude * (gJoystick_collision_force_scale / gForce_feedback_upper_limit));

    force_x = pPosition->v[0];
    force_y = pPosition->v[1];
    force_z = pPosition->v[2];
    force_length = sqrt(force_x * force_x + force_y * force_y + force_z * force_z);
    if (force_length > gJoystick_collision_force_min_length) {
        inverse_force_length = gJoystick_collision_force_one / force_length;
        normalized_position.v[0] = force_x * inverse_force_length;
        normalized_position.v[1] = force_y * inverse_force_length;
        normalized_position.v[2] = force_z * inverse_force_length;
    }
    else {
        normalized_position.v[0] = gJoystick_collision_force_one_f;
        normalized_position.v[1] = gJoystick_collision_force_zero_f;
        normalized_position.v[2] = gJoystick_collision_force_zero_f;
    }

    angle_degrees = (int)(acos(-(double)normalized_position.v[2]) * gJoystick_collision_force_radians_to_degrees);
    if (angle_degrees < 45) {
        angle_degrees = 90 - angle_degrees;
    }
    if (angle_degrees > 135) {
        angle_degrees = 270 - angle_degrees;
    }
    if (normalized_position.v[0] <= 0.0f) {
        angle_degrees = 360 - angle_degrees;
    }

    SetJoystickFFBCollisionForce(magnitude, angle_degrees * 100);
#else
    __asm {
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fcomp dword ptr [gForce_feedback_upper_limit]
            sub esp, 0xc
            push esi
            push edi
            fnstsw ax
            mov esi, edx
            test ah, 0x41
            jne L0045c934
            fstp st(0)
            fld dword ptr [gForce_feedback_upper_limit]
    L0045c934:
            fcom dword ptr [Force_feedback_lower_limit]
            fnstsw ax
            test ah, 1
            je L0045c949
            fstp st(0)
            fld dword ptr [Force_feedback_lower_limit]
    L0045c949:
            fld dword ptr [gForce_feedback_upper_limit]
            fdivr qword ptr [gJoystick_collision_force_scale]
            fxch st(1)
            fmulp st(1), st(0)
            call _ftol
            _emit 0xd9
        _emit 0x06
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            mov edi, eax
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gJoystick_collision_force_min_length]
            fnstsw ax
            test ah, 0x41
            jne L0045c9b1
            fdivr qword ptr [gJoystick_collision_force_one]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmulp st(5), st(0)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            fstp st(0)
            jmp L0045c9cf
    L0045c9b1:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gJoystick_collision_force_one_f]
            mov dword ptr [esp + 0xc], 0
            mov dword ptr [esp + 0x10], 0
    L0045c9cf:
            fld dword ptr [gJoystick_collision_force_zero_f]
            fxch st(1)
            fadd st(0), st(1)
            fld dword ptr [esp + 0x10]
            fmul dword ptr [gJoystick_collision_force_one_f]
            fxch st(1)
            fcomp dword ptr [gJoystick_collision_force_zero_f]
            fsubr st(0), st(1)
            fnstsw ax
            fstp dword ptr [esp + 0x1c]
            test ah, 0x41
            fstp st(0)
            jne L0045c9fe
            xor esi, esi
            jmp L0045ca03
    L0045c9fe:
            mov esi, 1
    L0045ca03:
            fld dword ptr [esp + 0x1c]
            call _CIacos
            fmul qword ptr [gJoystick_collision_force_radians_to_degrees]
            call _ftol
            cmp eax, 0x2d
            jge L0045ca25
            mov ecx, 0x5a
            sub ecx, eax
            mov eax, ecx
    L0045ca25:
            cmp eax, 0x87
            jle L0045ca35
            mov edx, 0x10e
            sub edx, eax
            mov eax, edx
    L0045ca35:
            test esi, esi
            je L0045ca42
            mov ecx, 0x168
            sub ecx, eax
            mov eax, ecx
    L0045ca42:
            lea eax, [eax + eax*4]
            mov ecx, edi
            lea edx, [eax + eax*4]
            shl edx, 2
            call SetJoystickFFBCollisionForce
            pop edi
            pop esi
            add esp, 0xc
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045c7f0
int C2_HOOK_FASTCALL GetJoystickFBBGain(void) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        return joystick_info->field_0xe4;
    }
    return gUNK_00596310;
}

// FUNCTION: CARMA2_HW 0x0045c810
void C2_HOOK_FASTCALL SetJoystickX(float pValue) {
    tButtonJoystickInfo* joystick;

    joystick = PDGetCurrentJoystickData();
    if (joystick != NULL) {
        joystick->field_0xd8 = pValue;
        if (joystick->field_0xd8 > 2.0) {
            joystick->field_0xd8 = 2.0f;
        }
        if (joystick->field_0xd8 < 0.0) {
            joystick->field_0xd8 = 0.0f;
        }
        gJoystick_x_steering = joystick->field_0xd8;
    }
}

// FUNCTION: CARMA2_HW 0x0045c870
void C2_HOOK_FASTCALL SetJoystickY(float pValue) {
    tButtonJoystickInfo* joystick;

    joystick = PDGetCurrentJoystickData();
    if (joystick != NULL) {
        joystick->field_0xdc = pValue;
        if (joystick->field_0xdc > 2.0) {
            joystick->field_0xdc = 2.0f;
        }
        if (joystick->field_0xdc < 0.0) {
            joystick->field_0xdc = 0.0f;
        }
        gJoystick_y_throttle = joystick->field_0xdc;
    }
}

// FUNCTION: CARMA2_HW 0x0045c7b0
void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue) {
    tButtonJoystickInfo* data;

    data = PDGetCurrentJoystickData();
    if (data != NULL) {
        if (pValue < 0) {
            pValue = 0;
        }
        if (pValue > 100) {
            pValue = 100;
        }
        data->field_0xe4 = pValue;
        gUNK_00596310 = pValue;
        SetAllFFBEffectsGain(pValue);
    }
}

extern void C2_HOOK_FASTCALL SelectNextDInputJoystick(void);

// FUNCTION: CARMA2_HW 0x0045c570
void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        joystick_info->field_0xe0 = pEnabled;
    }
}

// FUNCTION: CARMA2_HW 0x0045c6b0
void C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2) {
#ifdef CARPOCALYPSE2_PLATFORM_WIN32
    int index;
    tJoystick_force_effect* effect;

    if (!gForceFeedbackAvailable) {
        return;
    }
    index = PDFindJoystickEffect(pEffect_name);
    if (index == -1) {
        return;
    }
    if (gCurrent_ffb_effect_index != -1 && gCurrent_ffb_effect_index != index) {
        StopDinputEffect(gCurrent_ffb_effect_index);
        UnloadDinputFFBEffectAtIndex(gCurrent_ffb_effect_index);
        gCurrent_ffb_effect_index = -1;
    }
    if (gCurrent_ffb_effect_index != -1) {
        effect = GetJoystickEffect(gCurrent_ffb_effect_index);
        if (pArg2 == 0 && effect != NULL && effect->di_effect.dwDuration == -1) {
            return;
        }
    }
    gCurrent_ffb_effect_index = index;
    StartDinputEffect(index);
#endif
}

void C2_HOOK_FASTCALL Joystick_BackupSettings(void) {

    gOrig_joystick_index = gJoystick_index;
    gOriginal_joystick_x = GetJoystickX();
    gOriginal_joystick_y = GetJoystickY();
    gOriginal_joystick_fbb = GetJoystickFBBGain();
    gOriginal_joystick_dpad = PDIsJoystickDPadEnabled();
    gJoystick_index = gOrig_joystick_index;
}

int C2_HOOK_FASTCALL InsertHeadupText(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSortAlphabetically);

// FUNCTION: CARMA2_HW 0x0045bd50
void C2_HOOK_FASTCALL EnableJoysticks(void) {
    tButtonJoystickInfo* info;

    PDEnableJoysticks();
    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe8 = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0045bd70
void C2_HOOK_FASTCALL DisableJoysticks(void) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        joystick_info->field_0xe8 = 0;
    }
    FUN_0045a0c0();
}

// FUNCTION: CARMA2_HW 0x0045b0a0
void C2_NAKED C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pText_buffer) {
#ifndef CARPOCALYPSE2_MATCHING
    tHeadup_text* node;
    tHeadup_text* current;
    int index;
    int i;

    node = pText_buffer->texts;
    if (node == NULL) {
        return;
    }
    if (pText_buffer->field_0x4 == NULL) {
        pText_buffer->field_0x4 = pText_buffer->texts;
        if (pText_buffer->count - pText_buffer->pos_last_add < pText_buffer->field_0x10) {
            node = pText_buffer->texts;
            for (i = pText_buffer->count - pText_buffer->field_0x10; i > 0; i--) {
                node = node->next;
            }
            pText_buffer->field_0x4 = node;
            return;
        }
    }
    current = pText_buffer->field_0x4;
    index = 0;
    for (node = pText_buffer->texts; node != current; node = node->next) {
        index++;
    }
    if (pText_buffer->pos_last_add >= index + pText_buffer->field_0x10
            || pText_buffer->pos_last_add <= index) {
        if (pText_buffer->pos_last_add == index + pText_buffer->field_0x10) {
            if (pText_buffer->count - index < pText_buffer->field_0x10) {
                return;
            }
            pText_buffer->field_0x4 = current->next;
            return;
        }
        if (pText_buffer->count - pText_buffer->pos_last_add < pText_buffer->field_0x10) {
            node = pText_buffer->texts;
            for (i = pText_buffer->count - pText_buffer->field_0x10; i > 0; i--) {
                node = node->next;
            }
            pText_buffer->field_0x4 = node;
            return;
        }
        pText_buffer->field_0x4 = pText_buffer->text_last_add;
    }
#else
    __asm {
            push ebx
            push ebp
            mov ebp, dword ptr [ecx]
            push esi
            test ebp, ebp
            push edi
            je L0045b13f
            mov eax, dword ptr [ecx + 4]
            test eax, eax
            jne L0045b0df
            mov eax, dword ptr [ecx + 8]
            mov ebx, dword ptr [ecx + 0xc]
            mov edx, dword ptr [ecx + 0x10]
            mov esi, eax
            sub esi, ebx
            mov dword ptr [ecx + 4], ebp
            cmp esi, edx
            jge L0045b0df
            sub eax, edx
            mov edx, ebp
            test eax, eax
            jle L0045b131
    L0045b0d1:
            mov edx, dword ptr [edx + 0x38]
            dec eax
            jne L0045b0d1
            mov dword ptr [ecx + 4], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0045b0df:
            mov edi, dword ptr [ecx + 4]
            mov eax, ebp
            xor edx, edx
            cmp eax, edi
            je L0045b0f2
    L0045b0ea:
            mov eax, dword ptr [eax + 0x38]
            inc edx
            cmp eax, edi
            jne L0045b0ea
    L0045b0f2:
            mov esi, dword ptr [ecx + 0x10]
            mov ebx, dword ptr [ecx + 0xc]
            lea eax, [edx + esi]
            cmp ebx, eax
            jge L0045b105
            cmp ebx, edx
            jg L0045b13f
            cmp ebx, eax
    L0045b105:
            mov eax, dword ptr [ecx + 8]
            jne L0045b11b
            sub eax, edx
            cmp eax, esi
            jl L0045b13f
            mov edx, dword ptr [edi + 0x38]
            mov dword ptr [ecx + 4], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0045b11b:
            mov edx, eax
            sub edx, ebx
            cmp edx, esi
            jge L0045b139
            sub eax, esi
            mov edx, ebp
            test eax, eax
            jle L0045b131
    L0045b12b:
            mov edx, dword ptr [edx + 0x38]
            dec eax
            jne L0045b12b
    L0045b131:
            mov dword ptr [ecx + 4], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0045b139:
            mov eax, dword ptr [ecx + 0x18]
            mov dword ptr [ecx + 4], eax
    L0045b13f:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
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

// UNLABELED: CARMA2_HW 0x0045a920
int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSorted) {
    tHeadup_text* node;
    tHeadup_text* previous;
    int index;

    node = pText_buffer->texts;
    index = 0;
    if (node == NULL) {
        pText_buffer->texts = pText;
        pText->next = NULL;
        pText_buffer->count = 1;
        pText_buffer->pos_last_add = 0;
        pText_buffer->text_last_add = pText;
        return 0;
    }
    if (!pSorted) {
        while (node->next != NULL) {
            node = node->next;
            index++;
        }
        pText->next = NULL;
        node->next = pText;
        pText_buffer->count++;
        pText_buffer->pos_last_add = index;
        pText_buffer->text_last_add = node;
        return index;
    }
    previous = NULL;
    while (node != NULL) {
        if (strcmp(node->text, pText->text) > 0) {
            break;
        }
        previous = node;
        node = node->next;
        index++;
    }
    pText->next = node;
    if (previous == NULL) {
        pText_buffer->texts = pText;
    } else {
        previous->next = pText;
    }
    pText_buffer->count++;
    if (node != NULL) {
        pText_buffer->pos_last_add = index;
        pText_buffer->text_last_add = previous != NULL ? previous : node;
    } else {
        pText_buffer->pos_last_add = index - 1;
        pText_buffer->text_last_add = previous;
    }
    return pText_buffer->pos_last_add;
}

// FUNCTION: CARMA2_HW 0x0045b3b0
void C2_NAKED C2_HOOK_FASTCALL MungeJoystickHeadups(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tButtonJoystickInfo* joystick_info;
    tHeadup_text* headup_text;
    tHeadup_text* text;
    tU32 the_time;
    int last_index;
    int i;
    size_t j;
    char s[64];

    last_index = -1;
    joystick_info = NULL;
    if (gINT_0068b8e8) {
        the_time = PDGetTotalTime();
        if (PDKeyDown(65)) {
            if (gINT_00596308 != -1) {
                gINT_00596308 = -1;
            } else {
                joystick_info = PDGetCurrentJoystickData();
                if (joystick_info == NULL) {
                    goto munge;
                }
                i = 0;
                for (text = gJoystick_headup_buffer_0079d8a0.texts; text != NULL; text = text->next, i++) {
                    if (text == gJoystick_headup_buffer_0079d8a0.text_last_add) {
                        last_index = i;
                    }
                }
                for (j = 0; j < joystick_info->count_buttons; j++) {
                    if (joystick_info->buttons[j] == last_index && joystick_info->buttons[j] != -1) {
                        joystick_info->buttons[j] = -1;
                    }
                }
            }
            while (PDKeyDown(65)) {
            }
        }
        if (gINT_00596308 != -1) {
            goto munge;
        }
        if (PDKeyDown(64)) {
            if (gINT_00596308 == -1) {
                i = 0;
                for (text = gJoystick_headup_buffer_0079d8a0.texts; text != NULL; text = text->next, i++) {
                    if (text == gJoystick_headup_buffer_0079d8a0.text_last_add) {
                        last_index = i;
                    }
                }
                gINT_00596308 = last_index;
            }
            while (PDKeyDown(64)) {
            }
        }
        if (PDKeyDown(66)) {
            if (gJoystick_headup_buffer_0079d8a0.texts != NULL
                    && gJoystick_headup_buffer_0079d8a0.text_last_add != NULL
                    && gJoystick_headup_buffer_0079d8a0.text_last_add != gJoystick_headup_buffer_0079d8a0.texts) {
                gJoystick_headup_buffer_0079d8a0.pos_last_add -= 1;
                gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.texts;
                for (i = 0; i < gJoystick_headup_buffer_0079d8a0.pos_last_add; i++) {
                    gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.text_last_add->next;
                }
            }
            while (PDGetTotalTime() - the_time < 100) {
            }
        }
        if (PDKeyDown(67)) {
            if (gJoystick_headup_buffer_0079d8a0.texts != NULL
                    && gJoystick_headup_buffer_0079d8a0.text_last_add != NULL
                    && gJoystick_headup_buffer_0079d8a0.text_last_add->next != NULL) {
                gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.text_last_add->next;
                gJoystick_headup_buffer_0079d8a0.pos_last_add += 1;
            }
            while (PDGetTotalTime() - the_time < 100) {
            }
        }
        if (PDKeyDown(68)) {
            for (i = gJoystick_headup_buffer_0079d8a0.field_0x10 - 1; i > 0; i--) {
                if (gJoystick_headup_buffer_0079d8a0.text_last_add != NULL
                        && gJoystick_headup_buffer_0079d8a0.text_last_add != gJoystick_headup_buffer_0079d8a0.texts) {
                    gJoystick_headup_buffer_0079d8a0.pos_last_add -= 1;
                    gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.texts;
                    for (j = 0; j < (size_t)gJoystick_headup_buffer_0079d8a0.pos_last_add; j++) {
                        gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.text_last_add->next;
                    }
                }
            }
            while (PDKeyDown(68)) {
            }
        }
        if (PDKeyDown(69)) {
            for (i = gJoystick_headup_buffer_0079d8a0.field_0x10 - 1; i > 0; i--) {
                if (gJoystick_headup_buffer_0079d8a0.text_last_add != NULL
                        && gJoystick_headup_buffer_0079d8a0.text_last_add->next != NULL) {
                    gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.text_last_add->next;
                    gJoystick_headup_buffer_0079d8a0.pos_last_add += 1;
                }
            }
            while (PDKeyDown(69)) {
            }
        }
munge:
        AdjustHeadupTextBuffer(&gJoystick_headup_buffer_0079d8a0);
        headup_text = gJoystick_headup_buffer_0079d8a0.field_0x4;
        joystick_info = PDGetCurrentJoystickData();
        if (joystick_info == NULL) {
            return;
        }
    } else if (gINT_0068b8e4) {
        return;
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRace_head_ups); i++) {

        if (gINT_0068b8e8 && headup_text != NULL) {
            int font;

            if (i == gINT_00596308) {
                font = -3;
            } else if (headup_text == gJoystick_headup_buffer_0079d8a0.text_last_add) {
                font = -1;
            } else {
                font = -2;
            }
            ChangeHeadupFont(gRace_head_ups[i], font);
            if (i == gINT_00596308) {
                sprintf(s, "%s      ???", headup_text->text);
            } else {
                sprintf(s, "%s", headup_text->text);
                if (joystick_info != NULL) {
                    for (j = 0; j < joystick_info->count_buttons; j++) {
                        if (i == joystick_info->buttons[j]) {
                            sprintf(s, "%s      #%02d", headup_text->text, (int)j);
                        }
                    }
                }
            }
            ChangeHeadupText(gRace_head_ups[i], s);
            headup_text = headup_text->next;
        } else {
            ChangeHeadupText(gRace_head_ups[i], "");
        }
    }
#else
    __asm {
            mov eax, dword ptr [gINT_0068b8e8]
            sub esp, 0x58
            push ebx
            push ebp
            xor ebp, ebp
            or ebx, 0xffffffff
            push esi
            push edi
            test eax, eax
            je L0045b66f
            call PDGetTotalTime
            mov ecx, 0x41
            mov ebp, eax
            call PDKeyDown
            test eax, eax
            je L0045b45c
            cmp dword ptr [gINT_00596308], ebx
            je L0045b3ee
            mov dword ptr [gINT_00596308], ebx
            jmp L0045b440
    L0045b3ee:
            call PDGetCurrentJoystickData
            test eax, eax
            je L0045b64e
            mov ecx, dword ptr [gJoystick_headup_buffer_0079d8a0]
            xor edx, edx
            test ecx, ecx
            mov edi, ebx
            je L0045b41d
            mov esi, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
    L0045b40f:
            cmp ecx, esi
            jne L0045b415
            mov edi, edx
    L0045b415:
            mov ecx, dword ptr [ecx + 0x38]
            inc edx
            test ecx, ecx
            jne L0045b40f
    L0045b41d:
            mov ecx, dword ptr [eax + 0x50]
            xor esi, esi
            test ecx, ecx
            jbe L0045b440
            lea ecx, [eax + 0x54]
    L0045b429:
            mov edx, dword ptr [ecx]
            cmp edx, edi
            jne L0045b435
            cmp edx, ebx
            je L0045b435
            mov dword ptr [ecx], ebx
    L0045b435:
            mov edx, dword ptr [eax + 0x50]
            inc esi
            add ecx, 4
            cmp esi, edx
            jb L0045b429
    L0045b440:
            mov ecx, 0x41
            call PDKeyDown
            test eax, eax
            je L0045b45c
    L0045b44e:
            mov ecx, 0x41
            call PDKeyDown
            test eax, eax
            jne L0045b44e
    L0045b45c:
            cmp dword ptr [gINT_00596308], ebx
            jne L0045b64e
            mov ecx, 0x40
            call PDKeyDown
            test eax, eax
            je L0045b4c1
            cmp dword ptr [gINT_00596308], ebx
            jne L0045b4c1
            mov eax, dword ptr [gJoystick_headup_buffer_0079d8a0]
            xor ecx, ecx
            test eax, eax
            mov edx, ebx
            je L0045b49f
            mov esi, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
    L0045b491:
            cmp eax, esi
            jne L0045b497
            mov edx, ecx
    L0045b497:
            mov eax, dword ptr [eax + 0x38]
            inc ecx
            test eax, eax
            jne L0045b491
    L0045b49f:
            mov ecx, 0x40
            mov dword ptr [gINT_00596308], edx
            call PDKeyDown
            test eax, eax
            je L0045b4c1
    L0045b4b3:
            mov ecx, 0x40
            call PDKeyDown
            test eax, eax
            jne L0045b4b3
    L0045b4c1:
            mov ecx, 0x42
            call PDKeyDown
            test eax, eax
            je L0045b51f
            mov eax, offset gJoystick_headup_buffer_0079d8a0
            test eax, eax
            je L0045b50c
            mov ecx, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
            test ecx, ecx
            je L0045b50c
            mov eax, dword ptr [gJoystick_headup_buffer_0079d8a0]
            cmp ecx, eax
            je L0045b50c
            mov ecx, dword ptr [gJoystick_headup_buffer_0079d8a0+0xc]
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0x18], eax
            dec ecx
            test ecx, ecx
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0xc], ecx
            jle L0045b50c
    L0045b501:
            mov eax, dword ptr [eax + 0x38]
            dec ecx
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0x18], eax
            jne L0045b501
    L0045b50c:
            xor cl, cl
            cmp cl, 0x64
            jae L0045b51f
    L0045b513:
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x64
            jb L0045b513
    L0045b51f:
            mov ecx, 0x43
            call PDKeyDown
            test eax, eax
            je L0045b568
            mov edx, offset gJoystick_headup_buffer_0079d8a0
            test edx, edx
            je L0045b556
            mov eax, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
            test eax, eax
            je L0045b556
            mov eax, dword ptr [eax + 0x38]
            test eax, eax
            je L0045b556
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0x18], eax
            mov eax, dword ptr [gJoystick_headup_buffer_0079d8a0+0xc]
            inc eax
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0xc], eax
    L0045b556:
            xor al, al
            cmp al, 0x64
            jae L0045b568
    L0045b55c:
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x64
            jb L0045b55c
    L0045b568:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            je L0045b5e5
            mov ecx, dword ptr [gJoystick_headup_buffer_0079d8a0+0x10]
            lea eax, [ecx - 1]
            test eax, eax
            jle L0045b5c9
            mov esi, dword ptr [gJoystick_headup_buffer_0079d8a0]
            mov edx, dword ptr [gJoystick_headup_buffer_0079d8a0+0xc]
            mov edi, eax
            mov eax, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
    L0045b596:
            mov ecx, offset gJoystick_headup_buffer_0079d8a0
            test ecx, ecx
            je L0045b5c6
            test eax, eax
            je L0045b5c6
            cmp eax, esi
            je L0045b5c6
            dec edx
            mov eax, esi
            test edx, edx
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0xc], edx
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0x18], eax
            jle L0045b5c6
            mov ecx, edx
    L0045b5bb:
            mov eax, dword ptr [eax + 0x38]
            dec ecx
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0x18], eax
            jne L0045b5bb
    L0045b5c6:
            dec edi
            jne L0045b596
    L0045b5c9:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            je L0045b5e5
    L0045b5d7:
            mov ecx, 0x44
            call PDKeyDown
            test eax, eax
            jne L0045b5d7
    L0045b5e5:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            je L0045b64e
            mov edx, dword ptr [gJoystick_headup_buffer_0079d8a0+0x10]
            lea eax, [edx - 1]
            test eax, eax
            jle L0045b632
            mov ecx, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
            mov edx, eax
    L0045b608:
            mov eax, offset gJoystick_headup_buffer_0079d8a0
            test eax, eax
            je L0045b62f
            test ecx, ecx
            je L0045b62f
            mov eax, dword ptr [ecx + 0x38]
            test eax, eax
            je L0045b62f
            mov ecx, eax
            mov eax, dword ptr [gJoystick_headup_buffer_0079d8a0+0xc]
            inc eax
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0x18], ecx
            mov dword ptr [gJoystick_headup_buffer_0079d8a0+0xc], eax
    L0045b62f:
            dec edx
            jne L0045b608
    L0045b632:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            je L0045b64e
    L0045b640:
            mov ecx, 0x45
            call PDKeyDown
            test eax, eax
            jne L0045b640
    L0045b64e:
            mov ecx, offset gJoystick_headup_buffer_0079d8a0
            call AdjustHeadupTextBuffer
            mov ebx, dword ptr [gJoystick_headup_buffer_0079d8a0+0x4]
            call PDGetCurrentJoystickData
            mov ebp, eax
            test ebp, ebp
            je L0045b77d
            jmp L0045b680
    L0045b66f:
            mov eax, dword ptr [gINT_0068b8e4]
            test eax, eax
            jne L0045b77d
            mov ebx, dword ptr [esp + 0x14]
    L0045b680:
            mov dword ptr [esp + 0x14], 0x74d040
            mov dword ptr [esp + 0x10], 0
            mov edi, dword ptr [esp + 0x14]
    L0045b694:
            mov eax, dword ptr [gINT_0068b8e8]
            test eax, eax
            je L0045b755
            test ebx, ebx
            je L0045b755
            mov esi, dword ptr [esp + 0x10]
            mov eax, dword ptr [gINT_00596308]
            cmp eax, esi
            jne L0045b6bd
            mov edx, 0xfffffffd
            jmp L0045b6cf
    L0045b6bd:
            cmp ebx, dword ptr [gJoystick_headup_buffer_0079d8a0+0x18]
            jne L0045b6ca
            or edx, 0xffffffff
            jmp L0045b6cf
    L0045b6ca:
            mov edx, 0xfffffffe
    L0045b6cf:
            mov ecx, dword ptr [edi]
            call ChangeHeadupFont
            cmp dword ptr [gINT_00596308], esi
            jne L0045b6eb
            push ebx
            lea ecx, [esp + 0x1c]
            push offset gUNK_00596310+0x148
            push ecx
            jmp L0045b73d
    L0045b6eb:
            push ebx
            lea edx, [esp + 0x1c]
            test ebp, ebp
            push offset gCancel_recover+0xbc
            push edx
            je L0045b73d
            call sprintf
            mov eax, dword ptr [ebp + 0x50]
            add esp, 0xc
            xor esi, esi
            test eax, eax
            jbe L0045b745
            lea edi, [ebp + 0x54]
    L0045b70e:
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [esp + 0x10]
            cmp eax, ecx
            jne L0045b72c
            push esi
            push ebx
            lea ecx, [esp + 0x20]
            push offset gUNK_00596310+0x138
            push ecx
            call sprintf
            add esp, 0x10
    L0045b72c:
            mov eax, dword ptr [ebp + 0x50]
            inc esi
            add edi, 4
            cmp esi, eax
            jb L0045b70e
            mov edi, dword ptr [esp + 0x14]
            jmp L0045b745
    L0045b73d:
            call sprintf
            add esp, 0xc
    L0045b745:
            mov ecx, dword ptr [edi]
            lea edx, [esp + 0x18]
            call ChangeHeadupText
            mov ebx, dword ptr [ebx + 0x38]
            jmp L0045b761
    L0045b755:
            mov ecx, dword ptr [edi]
            mov edx, offset gDisable_sound+0x8
            call ChangeHeadupText
    L0045b761:
            mov edx, dword ptr [esp + 0x10]
            add edi, 4
            inc edx
            cmp edi, offset gRace_head_ups+0x58
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x14], edi
            jl L0045b694
    L0045b77d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x58
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045a300
int C2_HOOK_FASTCALL ReadUntilCharacter(FILE* pF, char* pBuffer, int pBuffer_size, char pLimit_char) {
    int i;
    char s[2];

    strcpy(pBuffer, "");
    s[1] = '\0';
    for (i = 0; (s[0] = fgetc(pF)) != pLimit_char && i < pBuffer_size && !feof(pF); i++) {
        strcat(pBuffer, s);
    }
    if (feof(pF)) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0045a0f0
int C2_HOOK_FASTCALL HasCurrentJoystick(void) {
    return gJoystick_index != -1;
}

// FUNCTION: CARMA2_HW 0x0045a2e0
int C2_HOOK_FASTCALL PeekChar(FILE* pFile) {
    char c;

    c = fgetc(pFile);
    return ungetc(c, pFile);
}

// FUNCTION: CARMA2_HW 0x0045c590
unsigned int C2_HOOK_FASTCALL GetJoystickButtonIndexForKey(int pKey) {
    tButtonJoystickInfo* joystick_info;
    unsigned int i;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        for (i = 0; i < joystick_info->count_buttons; i++) {
            if (gJoystick_headup_texts[joystick_info->buttons[i]].key == pKey) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0045b790
void C2_NAKED C2_HOOK_FASTCALL FUN_0045b790(void* pThis) {
#ifndef CARPOCALYPSE2_MATCHING
    tJoystick_headup_text* entry;
    tHeadup_text* dest;

    entry = gJoystick_headup_texts;
    do {
        dest = malloc(sizeof(tHeadup_text));
        if (dest != NULL) {
            memset(dest, 0, sizeof(tHeadup_text));
            strncpy(dest->text, entry->text, sizeof(dest->text));
            FUN_0045a920((tHeadup_text_buffer*)pThis, dest, 0);
        }
        entry++;
    } while ((int)entry < (int)(gJoystick_headup_texts + CARPOCALYPSE2_ASIZE(gJoystick_headup_texts)));
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov ebx, ecx
            mov edi, offset gJoystick_headup_texts
            xor ebp, ebp
    L0045b79d:
            push 0x3c
            call malloc
            mov esi, eax
            add esp, 4
            cmp esi, ebp
            je L0045b7c8
            mov dword ptr [esi + 0x38], ebp
            mov eax, dword ptr [edi]
            push 0x28
            push eax
            push esi
            call strncpy
            add esp, 0xc
            mov edx, esi
            mov ecx, ebx
            push ebp
            call FUN_0045a920
    L0045b7c8:
            add edi, 8
            cmp edi, offset gAccent_poly_prims
            jl L0045b79d
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
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

// FUNCTION: CARMA2_HW 0x0045a7f0
int C2_NAKED C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2) {
#ifndef CARPOCALYPSE2_MATCHING
    char local_100[256];
    char* substr;
    void* this_ptr;
    tHeadup_text* mem;

    this_ptr = (void*)pArg2[0];
    substr = (char*)pArg2[1];
    memcpy(local_100, pFilename, strlen(pFilename) + 1);
    DRstrlwr(local_100);
    if (strstr(local_100, "POWERUP") == NULL && strstr(local_100, "CPOINT") == NULL && strstr(local_100, "TRACKX") == NULL) {
        if (strstr(local_100, substr) == NULL) {
            if (strcmp(substr, ".*") != 0) {
                return 0;
            }
        }
        mem = malloc(sizeof(tHeadup_text));
        if (mem != NULL) {
            memset(mem, 0, sizeof(tHeadup_text));
            strncpy(mem->text, local_100, sizeof(mem->text));
            StringTransformToLower(mem->text);
            mem->text[0] = (char)toupper((int)mem->text[0]);
            FUN_0045a920(this_ptr, mem, 1);
        }
    }
    return 0;
#else
    __asm {
            sub esp, 0x100
            xor eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ebp, dword ptr [edx]
            mov ebx, dword ptr [edx + 4]
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x10]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x10]
            call DRstrlwr
            lea ecx, [esp + 0x10]
            push offset gUNK_00596310+0x104
            push ecx
            call strstr
            add esp, 8
            test eax, eax
            jne L0045a90a
            lea edx, [esp + 0x10]
            push offset gUNK_00596310+0xfc
            push edx
            call strstr
            add esp, 8
            test eax, eax
            jne L0045a90a
            lea eax, [esp + 0x10]
            push offset gUNK_00596310+0xf4
            push eax
            call strstr
            add esp, 8
            test eax, eax
            jne L0045a90a
            lea ecx, [esp + 0x10]
            push ebx
            push ecx
            call strstr
            add esp, 8
            test eax, eax
            jne L0045a8c3
            mov esi, offset gUNK_00596310+0xf0
            mov eax, ebx
    L0045a892:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L0045a8ba
            test cl, cl
            je L0045a8b6
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L0045a8ba
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0045a892
    L0045a8b6:
            xor eax, eax
            jmp L0045a8bf
    L0045a8ba:
            sbb eax, eax
            sbb eax, -1
    L0045a8bf:
            test eax, eax
            jne L0045a90a
    L0045a8c3:
            push 0x3c
            call malloc
            mov esi, eax
            add esp, 4
            test esi, esi
            je L0045a90a
            lea eax, [esp + 0x10]
            push 0x28
            push eax
            push esi
            mov dword ptr [esi + 0x38], 0
            call strncpy
            add esp, 0xc
            mov ecx, esi
            call StringTransformToLower
            movsx ecx, byte ptr [esi]
            push ecx
            call toupper
            add esp, 4
            mov edx, esi
            mov ecx, ebp
            mov byte ptr [esi], al
            push 1
            call L0045a920
    L0045a90a:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x100
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    L0045a920:
            sub esp, 0x10
            push ebx
            mov ebx, ecx
            push ebp
            push edi
            mov ebp, dword ptr [ebx]
            xor ecx, ecx
            xor edi, edi
            cmp ebp, ecx
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x18], ebx
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0xc], edi
            jne L0045a95f
            mov dword ptr [ebx], edx
            mov dword ptr [edx + 0x38], ecx
            mov dword ptr [ebx + 8], 1
            mov dword ptr [ebx + 0xc], ecx
            mov dword ptr [ebx + 0x18], edx
            xor eax, eax
            pop edi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 4
    L0045a95f:
            push esi
            mov esi, dword ptr [esp + 0x24]
            cmp esi, ecx
            mov eax, ebp
            jne L0045a98a
            mov esi, dword ptr [eax + 0x38]
            cmp esi, ecx
            je L0045a97f
    L0045a971:
            mov eax, esi
            inc edi
            mov esi, dword ptr [eax + 0x38]
            cmp esi, ecx
            jne L0045a971
            mov dword ptr [esp + 0x10], edi
    L0045a97f:
            mov dword ptr [edx + 0x38], ecx
            mov dword ptr [eax + 0x38], edx
            jmp L0045aa42
    L0045a98a:
            mov edi, edx
            mov esi, eax
    L0045a98e:
            mov dl, byte ptr [esi]
            mov bl, dl
            cmp dl, byte ptr [edi]
            jne L0045a9b2
            cmp bl, cl
            je L0045a9ae
            mov dl, byte ptr [esi + 1]
            mov bl, dl
            cmp dl, byte ptr [edi + 1]
            jne L0045a9b2
            add esi, 2
            add edi, 2
            cmp bl, cl
            jne L0045a98e
    L0045a9ae:
            xor esi, esi
            jmp L0045a9b7
    L0045a9b2:
            sbb esi, esi
            sbb esi, -1
    L0045a9b7:
            cmp esi, ecx
            jle L0045a9ca
            mov ebx, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [ebx], ecx
            mov dword ptr [ecx + 0x38], ebp
            jmp L0045aa42
    L0045a9ca:
            mov eax, ebp
            mov dword ptr [esp + 0x24], ecx
            mov ebp, dword ptr [eax + 0x38]
            cmp ebp, ecx
            je L0045aa30
    L0045a9d7:
            cmp dword ptr [esp + 0x18], ecx
            jne L0045aa30
            mov edi, dword ptr [esp + 0x14]
            mov esi, ebp
    L0045a9e3:
            mov dl, byte ptr [esi]
            mov bl, dl
            cmp dl, byte ptr [edi]
            jne L0045aa07
            cmp bl, cl
            je L0045aa03
            mov dl, byte ptr [esi + 1]
            mov bl, dl
            cmp dl, byte ptr [edi + 1]
            jne L0045aa07
            add esi, 2
            add edi, 2
            cmp bl, cl
            jne L0045a9e3
    L0045aa03:
            xor esi, esi
            jmp L0045aa0c
    L0045aa07:
            sbb esi, esi
            sbb esi, -1
    L0045aa0c:
            cmp esi, ecx
            jle L0045aa1e
            mov dword ptr [esp + 0x18], 1
            mov dword ptr [esp + 0x24], ebp
            jmp L0045aa20
    L0045aa1e:
            mov eax, ebp
    L0045aa20:
            mov esi, dword ptr [esp + 0x10]
            mov ebp, dword ptr [eax + 0x38]
            inc esi
            cmp ebp, ecx
            mov dword ptr [esp + 0x10], esi
            jne L0045a9d7
    L0045aa30:
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x24]
            mov ebx, dword ptr [esp + 0x1c]
            mov dword ptr [ecx + 0x38], edx
            mov dword ptr [eax + 0x38], ecx
    L0045aa42:
            mov ecx, dword ptr [ebx + 8]
            pop esi
            inc ecx
            pop edi
            mov dword ptr [ebx + 8], ecx
            mov ecx, dword ptr [esp + 8]
            mov dword ptr [ebx + 0xc], ecx
            mov dword ptr [ebx + 0x18], eax
            pop ebp
            mov eax, ecx
            pop ebx
            add esp, 0x10
            ret 4
    }
#endif
}
