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
#include <string.h>
extern void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pText);
// GLOBAL: CARMA2_HW 0x00595f88
int gJoystick_index = -1;

// GLOBAL: CARMA2_HW 0x00595f90
float gJoystick_x_steering = 1.f;

// GLOBAL: CARMA2_HW 0x00595f94
float gJoystick_y_throttle = 1.f;


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
int gINT_00596308 = -1;

// STUB: CARMA2_HW 0x0045c0b0
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {

    NOT_IMPLEMENTED();
    return 0;
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

float C2_HOOK_FASTCALL GetJoystickX(void) {

    NOT_IMPLEMENTED();
}

float C2_HOOK_FASTCALL GetJoystickY(void) {

    NOT_IMPLEMENTED();
}

int C2_HOOK_FASTCALL GetJoystickFBBGain(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SetJoystickX(float pValue) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SetJoystickY(float pValue) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL Joystick_BackupSettings(void) {

    gOrig_joystick_index = gJoystick_index;
    gOriginal_joystick_x = GetJoystickX();
    gOriginal_joystick_y = GetJoystickY();
    gOriginal_joystick_fbb = GetJoystickFBBGain();
    gOriginal_joystick_dpad = PDIsJoystickDPadEnabled();
    gJoystick_index = gOrig_joystick_index;
}

void C2_HOOK_FASTCALL EnableJoysticks(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL DisableJoysticks(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL FUN_0045b0a0(tHeadup_text_buffer* pText_buffer) {

    NOT_IMPLEMENTED();
}

int C2_HOOK_FASTCALL FUN_CheckJoystickHeadupButtons(tButtonJoystickInfo** pJoystick_info) {

    if (!PDKeyDown(65)) {
        return 1;
    }
    if (gINT_00596308 != -1) {
        gINT_00596308 = -1;
    } else {
        tHeadup_text* text;
        int last_index;
        size_t i;

        *pJoystick_info = PDGetCurrentJoystickData();
        if (*pJoystick_info == NULL) {
            return 0;
        }

        i = 0;
        last_index = -1;
        for (text = gJoystick_headup_buffer_0079d8a0.texts; text != NULL; text = text->next, i++) {
            if (text == gJoystick_headup_buffer_0079d8a0.text_last_add) {
                last_index = i;
            }
        }
        for (i = 0; i < (*pJoystick_info)->count_buttons; i++) {
            if ((*pJoystick_info)->buttons[i] == last_index && (*pJoystick_info)->buttons[i] != -1) {
                (*pJoystick_info)->buttons[i] = -1;
            }
        }
    }
    while (PDKeyDown(65)) {
        /* brr */
        /* FIXME: replace with udelay */
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045b3b0
void C2_HOOK_FASTCALL MungeJoystickHeadups(void) {
    tButtonJoystickInfo* joystick_info;
    tHeadup_text* headup_text;
    int i;

    joystick_info = NULL;
    headup_text = NULL;
    if (gINT_0068b8e8) {
        tU32 the_time;

        the_time = PDGetTotalTime();
        if (FUN_CheckJoystickHeadupButtons(&joystick_info) && gINT_00596308 == -1) {

            if (PDKeyDown(64) && gINT_00596308 == -1) {
                int i;
                tHeadup_text* headup_text2;

                i = 0;
                for (headup_text2 = gJoystick_headup_buffer_0079d8a0.texts; headup_text2 != NULL; i++, headup_text2 = headup_text2->next) {
                    if (headup_text2 == gJoystick_headup_buffer_0079d8a0.text_last_add) {
                        gINT_00596308 = i;
                    }
                }
                while (PDKeyDown(64)) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(66)) {
                if (gJoystick_headup_buffer_0079d8a0.text_last_add != NULL
                        && gJoystick_headup_buffer_0079d8a0.text_last_add != gJoystick_headup_buffer_0079d8a0.texts) {
                    int i;

                    gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.texts;
                    gJoystick_headup_buffer_0079d8a0.pos_last_add -= 1;
                    for (i = 0; i < gJoystick_headup_buffer_0079d8a0.pos_last_add; i++) {
                        gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.text_last_add->next;
                    }
                }
                while (PDGetTotalTime() - the_time < 100) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(67)) {
                if (gJoystick_headup_buffer_0079d8a0.text_last_add != NULL
                        && gJoystick_headup_buffer_0079d8a0.text_last_add->next != NULL) {
                    gJoystick_headup_buffer_0079d8a0.pos_last_add += 1;
                    gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.text_last_add->next;
                }
                while (PDGetTotalTime() - the_time < 100) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(68)) {
                int i;
                tHeadup_text* last_text;
                int last_pos;

                last_text = gJoystick_headup_buffer_0079d8a0.text_last_add;
                last_pos = gJoystick_headup_buffer_0079d8a0.pos_last_add;

                for (i = 0; i < gJoystick_headup_buffer_0079d8a0.field_0x10 - 1; i++) {
                    if (last_text != NULL && last_text != gJoystick_headup_buffer_0079d8a0.texts) {
                        int j;
                        tHeadup_text* headup_text2;

                        last_pos -= 1;
                        gJoystick_headup_buffer_0079d8a0.text_last_add = gJoystick_headup_buffer_0079d8a0.texts;
                        headup_text2 = gJoystick_headup_buffer_0079d8a0.texts;
                        gJoystick_headup_buffer_0079d8a0.pos_last_add = last_pos;

                        for (j = 0; j < last_pos; j++) {
                            headup_text2 = headup_text2->next;
                            gJoystick_headup_buffer_0079d8a0.text_last_add = headup_text2;
                        }
                    }
                }
                while (PDKeyDown(68)) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(69)) {
                int i;
                tHeadup_text* headup_text2;

                headup_text2 = gJoystick_headup_buffer_0079d8a0.text_last_add;

                for (i = 0; i < gJoystick_headup_buffer_0079d8a0.field_0x10 - 1; i++) {
                    if (headup_text2 != NULL && headup_text2->next != NULL) {
                        gJoystick_headup_buffer_0079d8a0.pos_last_add += 1;
                        headup_text2 = headup_text2->next;
                        gJoystick_headup_buffer_0079d8a0.text_last_add = headup_text2;
                    }
                }
                while (PDKeyDown(69)) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
        }
        FUN_0045b0a0(&gJoystick_headup_buffer_0079d8a0);
        headup_text = gJoystick_headup_buffer_0079d8a0.field_0x4;
        joystick_info = PDGetCurrentJoystickData();
        if (joystick_info == NULL) {
            return;
        }
    } else if (gINT_0068b8e4) {
        return;
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gRace_head_ups); i++) {

        if (gINT_0068b8e8 && headup_text != NULL) {
            int font;
            char s[80];

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
            } else if (joystick_info == NULL) {
                sprintf(s, "%s", headup_text->text);
            } else {
                size_t j;

                sprintf(s, "%s", headup_text->text);
                for (j = 0; j < joystick_info->count_buttons; j++) {
                    if (i == joystick_info->buttons[j]) {
                        sprintf(s, "%s      #%02d", headup_text->text, (int)j);
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

int C2_HOOK_FASTCALL HasCurrentJoystick(void) {

    return gJoystick_index != -1;
}