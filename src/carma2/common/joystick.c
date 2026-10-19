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
#include <string.h>
#include <stdio.h>
extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pText);
extern void C2_HOOK_FASTCALL FUN_0045a0c0(void);
extern const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void);
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


// GLOBAL: CARMA2_HW 0x00655e5c
float Force_feedback_lower_limit = 1.5f;
// GLOBAL: CARMA2_HW 0x0079d8a0
tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;
// GLOBAL: CARMA2_HW 0x00688700
int gOrig_joystick_index;

extern int gINT_0068b8e4;
extern int gINT_0068b8e8;
extern int gRace_head_ups[22];

float gForce_feedback_upper_limit = 2.7f;
float gOriginal_joystick_x;
float gOriginal_joystick_y;
int gOriginal_joystick_fbb;
int gOriginal_joystick_dpad;
int gOrig_joystick_index;
// GLOBAL: CARMA2_HW 0x00596308
int gINT_00596308 = -1;

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

int C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004575b0
void C2_HOOK_FASTCALL InitJoysticks(void) {

    PDInitJoysticks();
}

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

// UNLABELED: CARMA2_HW 0x0045c910
void C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, float pImpulse) {

    NOT_IMPLEMENTED();
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

// FUNCTION: CARMA2_HW 0x0045c570
void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        joystick_info->field_0xe0 = pEnabled;
    }
}

void C2_HOOK_FASTCALL Joystick_BackupSettings(void) {

    gOrig_joystick_index = gJoystick_index;
    gOriginal_joystick_x = GetJoystickX();
    gOriginal_joystick_y = GetJoystickY();
    gOriginal_joystick_fbb = GetJoystickFBBGain();
    gOriginal_joystick_dpad = PDIsJoystickDPadEnabled();
    gJoystick_index = gOrig_joystick_index;
}

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
void C2_HOOK_FASTCALL FUN_0045b0a0(tHeadup_text_buffer* pText_buffer) {
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
        FUN_0045b0a0(&gJoystick_headup_buffer_0079d8a0);
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

// FUNCTION: CARMA2_HW 0x0045a0f0
int C2_HOOK_FASTCALL HasCurrentJoystick(void) {

    return gJoystick_index != -1;
}
