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

extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pText);
extern void C2_HOOK_FASTCALL FUN_0045a0c0(void);
extern const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void);

// GLOBAL: CARMA2_HW 0x00595f84
int gUNK_00595f84 = 7500;
int C2_HOOK_FASTCALL InsertHeadupText(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSortAlphabetically);

#include <stdio.h>
extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pText);
extern void C2_HOOK_FASTCALL FUN_0045a0c0(void);
extern const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void);
extern int C2_HOOK_FASTCALL PDIsJoystickAvailable(int pIndex);
// GLOBAL: CARMA2_HW 0x00595f84
int gINT_00595f84 = 7500;

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
    gINT_00595f84 = info->field_0xe4;
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

// C2PORT-NONMATCHING: best 50.55% | difftest PASS 2000 trials (spec difftest/specs/0045c910_FUN_0045c910.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x0045c910
void C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, const br_vector3* pPosition, br_vector3* pForce, float pMagnitude) {
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
void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pText_buffer) {
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
void C2_HOOK_FASTCALL MungeJoystickHeadups(void) {
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
void C2_HOOK_FASTCALL FUN_0045b790(void* pThis) {
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
}

// FUNCTION: CARMA2_HW 0x0045a7f0
int C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2) {
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
}
