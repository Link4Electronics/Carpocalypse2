#include "timer.h"

#include "font.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006b7818
int gTimers_stack_size;

// GLOBAL: CARMA2_HW 0x006b7600
tTimer gTimers[19];

// GLOBAL: CARMA2_HW 0x006b7814
int gTimers_max_index;

// GLOBAL: CARMA2_HW 0x006aaa40
int gTimers_draw_x;

// GLOBAL: CARMA2_HW 0x006aaa44
int gTimers_draw_y_stride;

// GLOBAL: CARMA2_HW 0x006aaa48
int gTimers_enough_samples;

// GLOBAL: CARMA2_HW 0x006aaa4c
int gTimers_frame_count;

// GLOBAL: CARMA2_HW 0x006aaa50
tU32 gTimers_frame_end_time;

// GLOBAL: CARMA2_HW 0x006aaa54
int gTimers_draw_y;

// GLOBAL: CARMA2_HW 0x006aaa58
tU32 gTimers_frame_start_time;

// GLOBAL: CARMA2_HW 0x006aaa5c
tU32 gTimers_tolerance;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL PDGetMicroseconds(void);
extern tRace_over_reason gRace_over_reason;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int gLoad_last_save_game;
#endif
// FUNCTION: CARMA2_HW 0x00504300
void C2_HOOK_FASTCALL Timers_Init(void) {
    int i;
    int j;

    gTimers_stack_size = 0;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTimers); i++) {
        for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(gTimers[i].durations); j++) {
            gTimers[i].durations[j] = 0;
        }
        gTimers[i].longest_duration = 0;
    }

    gTimers_frame_count = 0;
    gTimers_enough_samples = 0;
    gTimers_max_index = CARPOCALYPSE2_ASIZE(gTimers) - 1;
    gTimers_draw_x = 34;
    gTimers_draw_y = gBack_screen->height > 250 ? 150 : 0;
    gTimers_draw_y_stride = 9;

#ifdef CARPOCALYPSE2_FIX_BUGS
#define LEN_IDENTIFIER(I) CARPOCALYPSE2_ASIZE(gTimers[I].identifier)
#else
#define LEN_IDENTIFIER(I) (CARPOCALYPSE2_ASIZE(gTimers[I].identifier) - 1)
#endif
#define TIMER_COLOUR_NAME(I, NAME, R5, G6, B5)                          \
    do {                                                                \
        gTimers[(I)].colour = RGB565_TO_BACKSCREEN_COLOUR(R5, G6, B5);  \
        strncpy(gTimers[(I)].identifier, (NAME), LEN_IDENTIFIER(I));    \
    } while (0)

    TIMER_COLOUR_NAME(TIMER_MNG, "MNG", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_ROB, "ROB", 0x00, 0x00, 0x15);
    TIMER_COLOUR_NAME(TIMER_RWL, "RWL", 0x00, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_RQQ, "R??", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_PHS, "PHS", 0x00, 0x26, 0x1f);
    TIMER_COLOUR_NAME(TIMER_PED, "PED", 0x1f, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_DRN, "DRN", 0x07, 0x2e, 0x07);
    TIMER_COLOUR_NAME(TIMER_OPP, "OPP", 0x1f, 0x3e, 0x1f);
    TIMER_COLOUR_NAME(TIMER_CRS, "CRS", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_LLR, "LLR", 0x00, 0x26, 0x1f);
    TIMER_COLOUR_NAME(TIMER_CRC, "CRC", 0x00, 0x00, 0x15);
    TIMER_COLOUR_NAME(TIMER_SRC, "SRC", 0x00, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_PRC, "PRC", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_CHK, "CHK", 0x1f, 0x3e, 0x1f);
    TIMER_COLOUR_NAME(TIMER_MAN, "MAN", 0x1f, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_GUA, "GUA", 0x00, 0x00, 0x15);
    TIMER_COLOUR_NAME(TIMER_REC, "REC", 0x00, 0x26, 0x1f);
    TIMER_COLOUR_NAME(TIMER_SER, "SER", 0x1f, 0x3e, 0x1f);
    TIMER_COLOUR_NAME(TIMER_OQQ, "O??", 0x06, 0x2a, 0x06);

#undef TIMER_COLOUR_NAME
#undef LEN_IDENTIFIER
}

// FUNCTION: CARMA2_HW 0x00504700
void C2_NAKED C2_HOOK_FASTCALL Timers_StartFrame(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTimers); i++) {
        gTimers[i].durations[gTimers[i].index] = 0;
    }
    gTimers[TIMER_OQQ].start_time = PDGetMicroseconds();
    gTimers_frame_start_time = PDGetMicroseconds();
#else
    __asm {
            push esi
            xor esi, esi
            xor ecx, ecx
            mov eax, offset gTimers+0x8
    L0050470a:
            xor edx, edx
            mov dl, byte ptr [eax]
            add eax, 0x1c
            add edx, ecx
            add ecx, 7
            cmp eax, offset gRace_over_reason
            mov dword ptr [edx*4 + gTimers+0x4], esi
            jl L0050470a
            call PDGetMicroseconds
            mov dword ptr [gTimers+0x204], eax
            call PDGetMicroseconds
            mov dword ptr [gTimers_frame_start_time], eax
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00504740
void C2_NAKED C2_HOOK_FASTCALL Timers_EndFrame(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    tU32 total_duration;

    if (gTimers_stack_size != 0) {
        PDFatalError("Timers_EndFrame(): Timer stack mismatch.");
    }
    gTimers[TIMER_OQQ].durations[gTimers[TIMER_OQQ].index] += PDGetMicroseconds() - gTimers[TIMER_OQQ].start_time;
    gTimers_frame_end_time = PDGetMicroseconds();
    gTimers_frame_count += 1;
    if (!gTimers_enough_samples && gTimers_frame_count > 64) {
        gTimers_enough_samples = 1;
    }

    total_duration = 0;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTimers); i++) {
        gTimers[i].total_duration = 0;
        for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(gTimers[i].durations); j++) {
            gTimers[i].total_duration += gTimers[i].durations[j];
        }
        gTimers[i].index = 0;
        total_duration += gTimers[i].total_duration;
        if (gTimers_enough_samples && gTimers[i].total_duration > gTimers[i].longest_duration) {
            gTimers[i].longest_duration = gTimers[i].total_duration;
        }
    }
    gTimers_tolerance = abs(total_duration - (gTimers_frame_end_time - gTimers_frame_start_time));
#else
    __asm {
            mov eax, dword ptr [gTimers_stack_size]
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            cmp eax, ebx
            push edi
            je L00504759
            mov ecx, offset gLoad_last_save_game+0x164
            call PDFatalError
    L00504759:
            mov eax, dword ptr [gTimers+0x200]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x1fc]
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers+0x204]
            mov edx, dword ptr [esi]
            sub eax, edi
            add edx, eax
            mov dword ptr [esi], edx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_frame_count]
            mov edi, dword ptr [gTimers_enough_samples]
            inc ecx
            cmp edi, ebx
            mov dword ptr [gTimers_frame_end_time], eax
            mov dword ptr [gTimers_frame_count], ecx
            jne L005047b0
            mov eax, ecx
            cmp eax, 0x40
            jle L005047b0
            mov edi, 1
            mov dword ptr [gTimers_enough_samples], edi
    L005047b0:
            xor esi, esi
            mov eax, offset gTimers
    L005047b7:
            mov dword ptr [eax], ebx
            lea ecx, [eax + 4]
            mov edx, 1
    L005047c1:
            mov ebp, dword ptr [ecx]
            add ecx, 4
            add dword ptr [eax], ebp
            dec edx
            jne L005047c1
            mov edx, dword ptr [eax]
            mov byte ptr [eax + 8], bl
            add esi, edx
            cmp edi, ebx
            je L005047e2
            mov ecx, dword ptr [eax]
            mov edx, dword ptr [eax + 0x10]
            cmp ecx, edx
            jbe L005047e2
            mov dword ptr [eax + 0x10], ecx
    L005047e2:
            add eax, 0x1c
            cmp eax, offset gTimers_max_index
            jl L005047b7
            mov ecx, dword ptr [gTimers_frame_start_time]
            mov edi, dword ptr [gTimers_frame_end_time]
            lea eax, [esi + ecx]
            sub eax, edi
            pop edi
            cdq
            xor eax, edx
            pop esi
            sub eax, edx
            pop ebp
            mov dword ptr [gTimers_tolerance], eax
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00504810
void C2_HOOK_FASTCALL Timers_Draw(br_pixelmap* pScreen) {
    tU32 longest_duration;
    int longest_duration_timer;
    tU32 total_duration;
    float draw_factor;
    char duration_str[16];
    char tolerance_str[64];
    int i;

    total_duration = 0;
    longest_duration_timer = -1;
    longest_duration = 0;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTimers); i++) {
        total_duration += gTimers[i].total_duration;
    }

    draw_factor = (float)(pScreen->width - 2 *  gTimers_draw_x) / (float)total_duration;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTimers); i++) {

        DRPixelmapText(pScreen,
            10,
            i * gTimers_draw_y_stride - 3 + gTimers_draw_y,
            &gFonts[1],
            gTimers[i].identifier,
            0);

        BrPixelmapLine(pScreen,
            gTimers_draw_x,
            i * gTimers_draw_y_stride + gTimers_draw_y + 0,
            (int)(gTimers[i].total_duration * draw_factor + (float)gTimers_draw_x),
            i * gTimers_draw_y_stride + gTimers_draw_y + 0,
            gTimers[i].colour);
        BrPixelmapLine(pScreen,
            gTimers_draw_x,
            i * gTimers_draw_y_stride + gTimers_draw_y + 1,
            (int)(gTimers[i].total_duration * draw_factor + (float)gTimers_draw_x),
            i * gTimers_draw_y_stride + gTimers_draw_y + 1,
            gTimers[i].colour);
        BrPixelmapLine(pScreen,
            gTimers_draw_x,
            i * gTimers_draw_y_stride + gTimers_draw_y + 2,
            (int)(gTimers[i].total_duration * draw_factor + (float)gTimers_draw_x),
            i * gTimers_draw_y_stride + gTimers_draw_y + 2,
            gTimers[i].colour);

        sprintf(duration_str, "%i", gTimers[i].total_duration);
        DRPixelmapText(pScreen,
            (int)(gTimers[i].total_duration * draw_factor + (float)gTimers_draw_x + 4.0f),
            i * gTimers_draw_y_stride - 3 + gTimers_draw_y,
            &gFonts[1],
            duration_str,
            0);
        if (gTimers[i].total_duration > longest_duration) {
            longest_duration = gTimers[i].total_duration;
            longest_duration_timer = i;
        }
    }
    if (longest_duration_timer != -1) {
        BrPixelmapRectangle(pScreen,
            gTimers_draw_x - 2,
            gTimers_draw_y + longest_duration_timer * gTimers_draw_y_stride - 2,
            (int)(gTimers[longest_duration_timer].total_duration * draw_factor - 5.0f),
            6,
            RGB565_TO_BACKSCREEN_COLOUR(0x1f, 0x3e, 0x1f));
    }
    BrPixelmapLine(pScreen,
        (int)(gTimers_draw_x + draw_factor * 10000.0),
        gTimers_draw_y - 1,
        (int)(gTimers_draw_x + draw_factor * 10000.0),
        gTimers_draw_y + (CARPOCALYPSE2_ASIZE(gTimers) - 1) * gTimers_draw_y_stride + 3,
        RGB565_TO_BACKSCREEN_COLOUR(0x1f, 0x00, 0x00));
    sprintf(tolerance_str, "8.38ms, tollerance = %i", gTimers_tolerance);
    DRPixelmapText(pScreen,
        (int)(((double)gTimers_draw_x + draw_factor * 10000.0) - 13.0),
        gTimers_draw_y + 7 + gTimers_draw_y_stride * (CARPOCALYPSE2_ASIZE(gTimers) - 1),
        &gFonts[1],
        tolerance_str,
        0);
}