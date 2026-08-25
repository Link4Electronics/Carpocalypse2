#include "15-displays.h"
#include "globvars.h"
#include "19-font.h"
#include "c2_string.h"

tHeadup gHeadups[37];


// GLOBAL: CARMA2_HW 0x00655e54
int gHeadup_detail_level = 5;

// LoadHeadupMessageFile

// STUB: CARMA2_HW 0x00449090
void C2_HOOK_FASTCALL InitHeadups(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// ClearHeadup

// ClearHeadupSlot

// ClearHeadups

// DimAFewBits

// KillOldestQueuedHeadup

// DoTestPowerHeadup

// DoPSPowerupHeadups

// DoHeadups

// FindAHeadupHoleWoofBarkSoundsABitRude

// FUNCTION: CARMA2_HW 0x0044a1a0
int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText) {

    while (*pText) {
        if (*(signed char*)pText < 0) {
            return 1;
        }
        pText++;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0044a220
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {
    int width;

    C2_HOOK_BUG_ON(sizeof(tHeadup) != 356);

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_BUG_ON((int)&((tHeadup*)0)->data.text_info.text != 0x4c);
    C2_HOOK_BUG_ON((int)&((tHeadup*)0)->data.coloured_text_info.coloured_font != 0x148);
#endif

    width = 0;
    if (pHeadup->type == eHeadup_box_text) {
        return 0;
    } else if (pHeadup->type == eHeadup_coloured_text) {

        pHeadup->clever = IsHeadupTextClever(pHeadup->data.text_info.text);
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    } else if (pHeadup->type ==eHeadup_fancy) {
        return 0;
    } else {
        pHeadup->clever = 0;
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    }
    return width;
}

// NewTextHeadupSlot2

// NewTextHeadupSlot

// NewImageHeadupSlot

// DoFancyHeadup

// MoveHeadupTo

// ChangeHeadupText

// DoDamageScreen

// DoInstruments

// ChangingView

// EarnCredits2

// EarnCredits

// EarnCredits100

// SpendCredits

// AwardTime

// AwardTime5

// DoTestHeadup

// FUNCTION: CARMA2_HW 0x0044ba60
int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth) {
    int result;

    if (gDevious_2d == 0 || gTexture_power_of_2 == 0) {
        return pWidth;
    }
    result = 1;
    while (result < pWidth) {
        result *= 2;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x0044ba90
int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight) {
    int result;

    if (gDevious_2d == 0 || gTexture_power_of_2 == 0) {
        return pHeight;
    }
    result = 1;
    while (result < pHeight) {
        result *= 2;
    }
    return result;
}

// DrawThisCarIconNow


// FUNCTION: CARMA2_HW 0x0044a920
void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont) {

    if (pHeadup_index >= 0) {
        gHeadups[pHeadup_index].data.image_info.font = &gFonts[-pFont];
    }
}

// FUNCTION: CARMA2_HW 0x0044a8d0
void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        strcpy(the_headup->data.text_info.text, pNew_text);
        MungeHeadupWidth(the_headup);
    }
}
