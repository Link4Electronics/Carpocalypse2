#include "sdl3.h"

#include <SDL3/SDL.h>

#include "sdl3_input.h"
#include "sdl3_ssdx.h"

#include "25-grafdata.h"
#include "05-drmem.h"
#include "52-errors.h"
#include "globvars.h"
#include "18-graphics2.h"
#include "02-init.h"
#include "42-input.h"
#include "platform.h"
#include "41-utility.h"

#include <stdlib.h>

#include "carpocalypse2_macros.h"
#ifdef SDL_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <libgen.h>
#include <sys/stat.h>
#endif

#if defined(C2_WIN32_DEBUG)
#define DR_DPRINTF(...) dr_dprintf(__VA_ARGS__)
#else
#define DR_DPRINTF(...)
#endif

#define DELTA_DOUBLE_CLICK_NS (300 * 1000 * 1000)

int gDefault_spec_index;
int gDefault_data_index;
tGraf_spec gGraf_specs[2] = {
    { 8, 1, 0, 320, 200, 0, 0, "32X20X8", "320x200 init string", 320, 320, 200, 0 },
    { 8, 1, 0, 640, 480, 0, 0, "64X48X8", "640x480 init string", 640, 640, 480, 0 },
};
int gNbPixelBits;
char gPathNetworkIni[256];
bool gPerformanceCounterInitialized;
Uint64 g_PerformanceFrequency;
Uint64 gPerformanceCounterFrequency_ms;
Uint64 gPerformanceCounterFrequency_us;
SDL_Window *g_SDL_Window;
SDL_Palette *g_SDL_Palette;
void *gPDActionReplayBuffer;
int gPDActionReplayBufferSize;
int gKeyboardBufferLength;
unsigned char gKeyboardBuffer[20];
Uint64 last_left_button_down;
Uint64 last_right_button_down;
void *gHWnd;
bool gMouseLButtonDown;
bool gMouseRButtonDown;
float gPD_mouse_position_x;
float gCursorPos_LastClick_x;
float gPD_mouse_position_y;
float gCursorPos_LastClick_y;
bool gCursorPos_LastClick_Valid;
bool gWindowShown;
char gFatalErrorMessage[512];
bool gIsFatalError;
int gExit_code;
br_diaghandler gPD_error_handler;
int gGraf_spec_index;
/* The software renderer device registered by BrDrv1SoftRendBegin (see
 * drivers/softrend/driver.c). */
/* The SDLREND output device from sdl3_rend.c (memory-backed screen). */
char* gRenderer = "SDLREND";
int gUnknown_int_0074ca94;
int gUnknown_int_0074cf48;
tJoystick_info g_Joystick_infos[16];
int g_Count_Joystick_infos;

char gExtendedAsciiToNormalAscii[128] = {
    ' ', ' ',  ' ',  ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '\'', '\'', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~',  'c',  '$', '*',  'Y', '|', ' ', '"', 'c', 'a', '<', '-', '-', 'R', '-',
    'o', ' ',  '2',  '3', '\'', 'u', ' ', '.', ',', '1', 'o', '>', ' ', ' ', ' ', '{',
    'A', 'A',  'A',  'A', 'A',  'A', 'A', 'C', 'E', 'E', 'E', 'E', 'I', 'I', 'I', 'I',
    'D', '}',  'O',  'O', 'O',  'O', 'O', 'x', '0', 'U', 'U', 'U', 'U', 'Y', 'b', 'B',
    'a', 'a',  'a',  'a', 'a',  'a', 'a', 'C', 'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i',
    'd', '}',  'o',  'o', 'o',  'o', 'o', '/', '0', 'u', 'u', 'u', 'u', 'y', 'b', 'y',
};

static int find_gamepad_index(SDL_JoystickID which) {
    int i;

    for (i = 0; i < g_Count_Joystick_infos; i++) {
        if (g_Joystick_infos[i].which == which) {
            return i;
        }
    }
    return -1;
}

void SDL3ServiceMessages(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            gPD_mouse_position_x = event.motion.x;
            gPD_mouse_position_y = event.motion.y;
        } else if (event.type == SDL_EVENT_QUIT) {
            SDL_DestroyWindow(g_SDL_Window);
            g_SDL_Window = NULL;
            gHWnd = NULL;
            PDShutdownSystem();
            abort();
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            bool doubleclicked = false;
            switch (event.button.button) {
            case 1:
                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    gMouseLButtonDown = true;
                    if (last_left_button_down - event.button.timestamp < DELTA_DOUBLE_CLICK_NS) {
                        doubleclicked = true;
                    }
                    last_left_button_down = event.button.timestamp;
                } else {
                    gMouseLButtonDown = false;
                }
                break;
            case 3:
                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    gMouseRButtonDown = true;
                    if (last_right_button_down - event.button.timestamp < DELTA_DOUBLE_CLICK_NS) {
                        doubleclicked = true;
                    }
                    last_right_button_down = event.button.timestamp;
                } else {
                    gMouseRButtonDown = false;
                }
                break;
            }
            if (doubleclicked) {
                gPD_mouse_position_x = gCursorPos_LastClick_x = event.button.x;
                gPD_mouse_position_y = gCursorPos_LastClick_y = event.button.y;
                gCursorPos_LastClick_Valid = true;
            }
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            if (0x1f < event.key.key  && event.key.key != 0x7f) {
                char buffer[256];
                if (gKeyboardBufferLength >= SDL_arraysize(gKeyboardBuffer)) {
                    memmove(gKeyboardBuffer, &gKeyboardBuffer[1], SDL_arraysize(gKeyboardBuffer) - 1);
                    gKeyboardBufferLength = SDL_arraysize(gKeyboardBuffer) - 1;
                }
                gKeyboardBuffer[gKeyboardBufferLength] = event.key.key;
                gKeyboardBufferLength++;
                strncpy(buffer, (char*)gKeyboardBuffer, gKeyboardBufferLength);
                buffer[gKeyboardBufferLength] = '\0';
                DR_DPRINTF("KEY PRESSED, BUFFER NOW IS: '%s'", buffer);
            }
        } else if (event.type == SDL_EVENT_WINDOW_HIDDEN || event.type == SDL_EVENT_WINDOW_SHOWN) {
            gWindowShown = event.type == SDL_EVENT_WINDOW_SHOWN;
        } else if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            if (g_Count_Joystick_infos < SDL_arraysize(g_Joystick_infos)) {
                SDL_memset(&g_Joystick_infos[g_Count_Joystick_infos], 0, sizeof(tJoystick_info));
                g_Count_Joystick_infos += 1;
                g_Joystick_infos[g_Count_Joystick_infos].which = event.gdevice.which;
            }
        } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            int index = find_gamepad_index(event.gdevice.which);
            if (index >= 0) {
                SDL_memmove(&g_Joystick_infos[index], &g_Joystick_infos[index + 1], sizeof(tJoystick_info) * (g_Count_Joystick_infos - index - 1));
                g_Count_Joystick_infos -= 1;
            }
        } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
            int index = find_gamepad_index(event.gdevice.which);
            if (index >= 0 && event.gbutton.button < 4 * sizeof(g_Joystick_infos[index].buttons)) {
                g_Joystick_infos[index].buttons |= 0x1 << event.gbutton.button;
            }
        } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
            int index = find_gamepad_index(event.gbutton.which);
            if (index >= 0 && event.gbutton.button < 4 * sizeof(g_Joystick_infos[index].buttons)) {
                g_Joystick_infos[index].buttons &= ~(0x1 << event.gbutton.button);
            }
        } else if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
            int index = find_gamepad_index(event.gaxis.which);
            if (index >= 0) {
                g_Joystick_infos[index].axis_motions[event.gaxis.axis] = event.gaxis.value;
            }
        }
    }
}

void PDReallyAllocateActionReplayBuffer(void) {
    gPDActionReplayBufferSize = 8000000;
    gPDActionReplayBuffer = SDL_malloc(gPDActionReplayBufferSize);
    if (gPDActionReplayBuffer == NULL) {
        gPDActionReplayBufferSize = 0;
    }
}

void C2_HOOK_FASTCALL PDInitialiseSystem(void) {
    gBack_screen = NULL;
    gScreen = NULL;
    SDL_HideCursor();
    KeyBegin();
    Win32InitInputDevice();
}

void PDSetFileVariables(void) {
#ifdef SDL_PLATFORM_WINDOWS
    gDir_separator = "\\";
#else
    gDir_separator = "/";
#endif
}

void PDBuildAppPath(char* pThe_path) {
    char *curdir = SDL_GetCurrentDirectory();
    SDL_snprintf(pThe_path, 256, "%s/", curdir);
    SDL_free(curdir);
    dr_dprintf("Application path '%s'", pThe_path);
}

void PDFatalError(const char *pThe_str) {
    dr_dprintf("FATAL ERROR: %s", pThe_str);
    if (pThe_str == NULL) {
        pThe_str = "NULL str1";
    }
    gIsFatalError = 1;
    SDL_snprintf(gFatalErrorMessage, SDL_arraysize(gFatalErrorMessage), "%s\n%s", pThe_str, "");

    gExit_code = 700;
    if (gBack_screen != NULL) {
        if (gBack_screen->pixels != NULL) {
            gExit_code = 700;
            PDUnlockRealBackScreen();
        }
    }
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
}

void PDNonFatalError(const char *pThe_str) {
    dr_dprintf("*** ERROR...");
    dr_dprintf(pThe_str);
}

int PDIsWindowInactive(void) {
    return !gWindowShown;
}

char PDConvertToASCIILessThan128(char pChar) {
    if ((unsigned char)pChar > 0x7f) {
        char c = gExtendedAsciiToNormalAscii[(unsigned char)pChar - 128];
        dr_dprintf("PDConvertToASCIILessThan128() Returning %d", c);
        return c;
    }
    dr_dprintf("PDConvertToASCIILessThan128() Returning %d", pChar);
    return pChar;
}

int PDGetKeyboardCharacter(void) {
    int key;
    SDL3ServiceMessages();
    if (gKeyboardBufferLength == 0) {
        return 0;
    }
    key = gKeyboardBuffer[0];
    if (gKeyboardBufferLength > 1) {
        SDL_memmove(&gKeyboardBuffer[1], &gKeyboardBuffer[0], gKeyboardBufferLength - 1);
    }
    gKeyboardBufferLength -= 1;
    dr_dprintf("KEY RETURNED %d", key);
    return key;
}

void PDClearKeyboardBuffer(void) {
    dr_dprintf("KEYBOARD BUFFER CLEARED");
    gKeyboardBufferLength = 0;
}

C2_NORETURN void PDShutdownSystem(void) {
    static bool been_here = false;

    gBack_screen = NULL;
    if (been_here) {
        PDDisposeActionReplayBuffer();
        exit(127);
    } else {
        been_here = 1;
        dr_dprintf("PDShutdownSystem()");
        SSDXStop();
        SSDXRelease();
        CloseDirectInput();
        SDL_ShowCursor();
        if (g_SDL_Window != NULL) {
            dr_dprintf("Resizing main window...");
            SDL_SetWindowFullscreen(g_SDL_Window, false);
        }
        dr_dprintf("Servicing messages...");
        SDL3ServiceMessages();
        dr_dprintf("Sending WM_SHOWWINDOW broadcast message...");
        SDL_ShowWindow(g_SDL_Window);
        if (gIsFatalError) {
            dr_dprintf("Displaying fatal error...");
            if (getenv("CARPOCALYPSE2_NO_DIALOG") == NULL) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Carmageddon Fatal error", gFatalErrorMessage, g_SDL_Window);
            }
        }
        if (g_SDL_Window != NULL) {
            dr_dprintf("Destroying window...");
            SDL_DestroyWindow(g_SDL_Window);
            g_SDL_Window = NULL;
            gHWnd = NULL;
        }
        dr_dprintf("End of PDShutdownSystem()");
        CloseDiagnostics();
        PDDisposeActionReplayBuffer();
        SDL_Quit();
        exit(gExit_code);
    }
}

void PDLockRealBackScreen(void) {
}

void PDUnlockRealBackScreen(void) {
}

int PDGetASCIIFromKey(int pKey) {
    if (PDKeyDown(0)) {
        return gASCII_shift_table[pKey];
    } else {
        return gASCII_table[pKey];
    }
}

int PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size) {
    char the_path[256];
    const char* lookup;
    SDL_PathInfo info;

    if (pFile_name != NULL) {
        PathCat(the_path, pThe_path, pFile_name);
    } else {
        strcpy(the_path, pThe_path);
    }
    if (the_path[0] && the_path[1] == ':' && the_path[2] == '\0') {
        strcat(the_path, gDir_separator);
    }
#ifndef _WIN32
    /* Linux is case-sensitive; the game data is not. */
    {
        extern const char* carpocalypse2_fix_path_case(const char* pPath);
        lookup = carpocalypse2_fix_path_case(the_path);
    }
#else
    lookup = the_path;
#endif
    if (!SDL_GetPathInfo(lookup, &info)) {
        return 0;
    }
    return info.type == SDL_PATHTYPE_FILE && info.size >= pMin_size;
}

int PDReadSourceLocation(tPath_name pPath) {
    /* No support for CD */
    return 0;
}

int PDDoWeLeadAnAustereExistance(void) {
    return 0;
}

void PDSetPaletteEntries(br_pixelmap *pPalette, int pFirst_colour, int pCount) {
    int i;
    SDL_Color colours[256];
    tU32* pixels = pPalette->pixels;

    for (i = 0; i < pCount; i++) {
        tU32 c;

        c = pixels[pFirst_colour + i];
        colours[i].a = 0;
        colours[i].r = c >> 16;
        colours[i].g = c >> 8;
        colours[i].b = c >> 0;
    }
    SDL_SetPaletteColors(g_SDL_Palette, colours, pFirst_colour, pCount);
    BrPixelmapDoubleBuffer(gScreen, gBack_screen);
}

void PDSetPalette(br_pixelmap *pixelmap) {
    BrPixelmapPaletteSet(gScreen, pixelmap);
}

int PDServiceSystem(tU32 pTime_since_last_call) {
    SDL3ServiceMessages();
    return 0;
}

void PDScreenBufferSwap(int pRendering_area_only) {
    BrPixelmapDoubleBuffer(gScreen, gBack_screen);
}

void PDInitTimer(void) {
    gPerformanceCounterInitialized = false;
    g_PerformanceFrequency = SDL_GetPerformanceFrequency();
    gPerformanceCounterFrequency_ms = g_PerformanceFrequency / 1000;
    gPerformanceCounterFrequency_us = g_PerformanceFrequency / 1000000;
    gPerformanceCounterInitialized = true;
}

tU32 PDGetTotalTime(void) {
    if (gPerformanceCounterInitialized) {
        return (tU32) (SDL_GetPerformanceCounter() / g_PerformanceFrequency);
    } return 0;
}

int PDGetMicroseconds(void) {
    if (gPerformanceCounterInitialized) {
        return (tU32)(SDL_GetPerformanceCounter() / gPerformanceCounterFrequency_us);
    }
    return 0;
}

void PDEnterDebugger(const char *pStr) {
    dr_dprintf("PDEnterDebugger(): %s", pStr);
    SDL_ShowCursor();
    SDL_TriggerBreakpoint();
    SDL_HideCursor();
}

int PDFileUnlock(const char *pThe_path) {
#ifdef SDL_PLATFORM_WINDOWS
    DWORD dwAttributes;

    dwAttributes = GetFileAttributesA(pThe_path);
    if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
        return SetFileAttributesA(pThe_path, dwAttributes & ~FILE_ATTRIBUTE_READONLY);
    }
    return 0;
#else
    struct stat s;
    if (stat(pThe_path, &s) != 0) {
        return 1;
    }
    if (chmod(pThe_path, s.st_mode | S_IWUSR | S_IRGRP) != 0) {
        return 1;
    }
    return 0;
#endif
}

void PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data) {
    int count;
    int i;
    char **files;

    files = SDL_GlobDirectory(path, "*.*", 0, &count);
    for (i = 0; i < count; i++) {
        tPath_name found_path;
        SDL_PathInfo info;
        size_t l = SDL_strlen(files[i]);
        if (l >= 4 && SDL_strcasecmp(files[i] + l - 4, ".lnk") == 0) {
            continue;
        }
        PathCat(found_path, path, files[i]);
        if (!SDL_GetPathInfo(found_path, &info)) {
            continue;
        }
        if (info.type != SDL_PATHTYPE_FILE) {
            continue;
        }
        if (pCallback(found_path, data) != 0) {
            break;
        }
    }
    SDL_free(files);
}

int PDmkdir(const char *path) {
    return mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}

void MAMSUnlock(void **pPtr) {
    void *mem = *pPtr;
    if (mem == NULL) {
        FatalError(kFatalError_OOM_S, "NULL POINTER BEING FREED");
    }
    SDL_free(mem);
    *pPtr = NULL;
}

void MAMSLock(void **pPtr) {
}

void C2_HOOK_FASTCALL PDForEveryFileRecurse(const char *pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    char **files;
    int count;
    int i;

    dr_dprintf("*PDForEveryFileRecurse() - NEW DIRECTORY '%s'", pThe_path);
    files = SDL_GlobDirectory(pThe_path, "*", 0, &count);
    for (i = 0; i < count; i++) {
        SDL_PathInfo info;
        tPath_name found_path;

        PathCat(found_path, pThe_path, files[i]);
        if (!SDL_GetPathInfo(found_path, &info)) {
            continue;
        }
        if (info.type == SDL_PATHTYPE_DIRECTORY) {
            PDForEveryFileRecurse(found_path, pAction_routine);
        } else {
            dr_dprintf("PDForEveryFileRecurse() - Calling action proc with file '%s' in dir '%s'", found_path, pThe_path);
            pAction_routine(found_path);
        }
    }
    SDL_free(files);
}

void C2_HOOK_FASTCALL PDForEveryFile(const char *pThe_path, tPDForEveryFile_cbfn pAction_routine) {
    char **files;
    int count;
    int i;

    files = SDL_GlobDirectory(pThe_path, "*.???", 0, &count);
    for (i = 0; i < count; i++) {
        tPath_name found_path;

        PathCat(found_path, pThe_path, files[i]);
        pAction_routine(found_path);
    }
    SDL_free(files);
}

void C2_HOOK_FASTCALL PDGetCurrentDate(char *pTimeStr) {
    SDL_Time time_now = 0;
    SDL_DateTime date_time;

    SDL_GetCurrentTime(&time_now);
    SDL_TimeToDateTime(time_now, &date_time, true);

    /* FIXME: use DL_GetDateTimeLocalePreferences to get the format preference? */

    SDL_snprintf(pTimeStr, 30, "%d-%d-%d", date_time.day, date_time.month, date_time.year);
}

void PDGetCurrentTime(char *pTimeStr) {
    SDL_Time time_now = 0;
    SDL_DateTime date_time;

    SDL_GetCurrentTime(&time_now);
    SDL_TimeToDateTime(time_now, &date_time, true);

    /* FIXME: use DL_GetDateTimeLocalePreferences to get the format preference? */

    SDL_snprintf(pTimeStr, 30, "%d:%d", date_time.hour, date_time.minute);
}

int C2_HOOK_FASTCALL PDGetMouseClickPosition(int *pX_coord, int *pY_coord) {
    gCursorPos_LastClick_Valid = 0;
    if (!gCursorPos_LastClick_Valid) {
        return 0;
    }
    *pX_coord = gCursorPos_LastClick_x;
    *pY_coord = gCursorPos_LastClick_y;
    return 1;
}

static void OnWarnCallback(char* text) {

    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender WARNING: '%s'", text);
    dr_dprintf("*******************************************************************************");
}

static void OnErrorCallback(char* text) {
    const char *msg;

    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender FAILURE: '%s'", text);
    dr_dprintf("*******************************************************************************");
    msg = "BRender error detected:";
    if (msg == NULL) {
        msg = "NULL str1";
    }
    if (text == NULL) {
        msg = "NULL str2";
    }
    gIsFatalError = 1;
    sprintf(gFatalErrorMessage, "%s\n%s", msg, text);
    gExit_code = 700;
    if (gBack_screen != NULL && gBack_screen->pixels != NULL) {
        PossibleUnlock(1);
    }
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
}

void PDInstallErrorHandlers(void) {

    gPD_error_handler.identifier = "LlantisilioBlahBlahBlahOgOgOch";
    gPD_error_handler.warning = OnWarnCallback;
    gPD_error_handler.failure = OnErrorCallback;
    BrDiagHandlerSet(&gPD_error_handler);
}

int PDInitScreenVars(int pArgc, const char **pArgv) {
    int i;

    /* Hardware edition defaults to the largest available mode; the low-res
     * software specs lack the PIX16 asset variants on partial installs. */
    {
        int best_area = -1;
        int best = gDefault_spec_index;
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gGraf_specs); i++) {
            int area = gGraf_specs[i].total_width * gGraf_specs[i].total_height;
            if (area > best_area) {
                best_area = area;
                best = i;
            }
        }
        gGraf_spec_index = best;
    }
    gGraf_data_index = gDefault_data_index;
    return 1;
}

void PDSaveOriginalPalette(void) {
}

void PDInitScreen(void) {
}

void PDAllocateScreenAndBack(void) {
    dr_dprintf("PDAllocateScreenAndBack() - START...");

    dr_dprintf("Setting up DirectSound stuff...");

    SSDXStart(gHWnd, 0, 1, SSDX_InitDirectSound);

    gNbPixelBits = 16;
    {
        extern void carpocalypse2_RegisterSDLRenderer(void);
        br_error dev_r;

        carpocalypse2_RegisterSDLRenderer();

        dev_r = BrDevBeginVar(&gScreen, gRenderer,
            BRT_WINDOW_FULLSCREEN_B, 1,
            BRT_WINDOW_HANDLE_H, gHWnd,
            BRT_WIDTH_I32, 640,
            BRT_HEIGHT_I32, 480,
            BRT_PIXEL_BITS_I32, 16,
            BRT_PIXEL_TYPE_U8, BR_PMT_RGB_565,
            BR_NULL_TOKEN
        );
        dr_dprintf("BrDevBeginVar('%s') -> %d", gRenderer, dev_r);
    }
    if (gScreen == NULL) {
        BrFailure("Unable to allocate Main Front Screen");
    }

    if (SDL_strcmp(gScreen->identifier, "Voodoo Graphics") == 0) {
        dr_dprintf("%s: lock seldom", gScreen->identifier);
    } else {
        dr_dprintf("%s: lock often", gScreen->identifier);
        gLock_often = 1;
    }
    gShadow_workaround = 1; /* Not 100% sure this is shadow related */
    gEnable_texture_interpolation = 1;
    gEnable_texture_antialiasing = 1;
    gTexture_power_of_2 = 1;
    gUnknown_int_0074ca94 = 8;
    gUnknown_int_0074cf48 = 256;
    gAdapt_sky_model_for_cockpit = 1;
    gNo_fog = 1; /* Enables lut for horizon material */
    gRenderer_fixup_basename = "VOODOO";
    gRenderer_fixup_extension = ".TXT";
    gNo_render_indent = 1;
    gNo_2d_effects = 1; /* maybe? */
    gEnable_perspective_maps = 1;
    gNoTransients = 1;
    gDevious_2d = 1;

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    gBack_screen = BrPixelmapMatch(gScreen, BR_PMMATCH_OFFSCREEN);
    if (gBack_screen == NULL) {
        BrFailure("Unable to allocate Back Screen");
    }

    gPixelFlags = kPixelFlags_unknown_0x4 | (gNbPixelBits == 16 ? kPixelFlags_16bbp : 0);

    gDouble_back_screen = BrPixelmapMatch(gBack_screen, BR_PMMATCH_OFFSCREEN);
    gHas_double_back_screen = 1;

    SDL_memset(gBack_screen->pixels, 0, gBack_screen->height * gBack_screen->row_bytes);

    BrPixelmapDoubleBuffer(gScreen, gBack_screen);

    dr_dprintf("PDAllocateScreenAndBack() - END.");
}

int PDGetRegisterSourceLocation(char *buffer, int *buffer_size) {
    return 0;
}

void C2_HOOK_FASTCALL PDGetMousePosition(int *pX, int *pY) {
    /* OG win32 calls GetCursorPos + ScreenToClient: does it get an updated position from the message pump? */
    *pX = gPD_mouse_position_x;
    *pY = gPD_mouse_position_y;
}

void PDRevertPalette(void) {
}

void PDMouseButtons(int *pLeftButtonDown, int *pRightButtonDown) {
    *pLeftButtonDown = gMouseLButtonDown;
    if (pRightButtonDown != NULL) {
        *pRightButtonDown = gMouseRButtonDown;
    }
}

void PDExtractFilename(char *pDest, const char *pPath) {
#ifdef SDL_PLATFORM_WINDOWS
    char filename[256];
    char extension[256];

    _splitpath(pPath, NULL, NULL, filename, extension);
    sprintf(pDest, "%s%s", filename, extension);
#else
    tPath_name buffer;

    SDL_strlcpy(buffer, pPath, SDL_arraysize(buffer));
    basename(buffer);
    SDL_strlcpy(pDest, buffer, 256);
#endif
}

void C2_HOOK_FASTCALL PDExtractDirectory(char *pDest, const char *pPath) {
#ifdef SDL_PLATFORM_WINDOWS
    char dirname [256];
    char drive[4];

    _splitpath(pPath, drive, dirname, NULL, NULL);
    sprintf(pDest, "%s%s", drive, dirname);
#else
    tPath_name buffer;

    SDL_strlcpy(buffer, pPath, SDL_arraysize(buffer));
    dirname(buffer);
    SDL_strlcpy(pDest, buffer, 256);
#endif
}

void PDPageInProcessMemory(void) {
}

void PDFileDelete(const char *pPath, int pIgnore_read_only) {

    if (pIgnore_read_only) {
#ifdef SDL_PLATFORM_WINDOWS
        DWORD flags = GetFileAttributesA(pPath);
        if (flags != INVALID_FILE_ATTRIBUTES) {
            SetFileAttributesA(pPath, flags & ~FILE_ATTRIBUTE_READONLY);
        }
#else
        struct stat s;
        if (stat(pPath, &s) != 0) {
            if (!(s.st_mode & S_IWUSR)) {
                chmod(pPath, s.st_mode | S_IWUSR);
            }
        }
#endif
    }
    SDL_RemovePath(pPath);
}

void PDDisposeActionReplayBuffer(void) {
}

int PDmemicmp(const char* str1, const char* str2, size_t count) {

    return memcmp(str1, str2, count);
}

/* ==== Minimal presentation for Linux boot ==== */

#include "globvars.h"
#include "25-grafdata.h"

int carpocalypse2_ShouldQuit(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return 1;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            return 1;
        }
    }
    return 0;
}

void carpocalypse2_PresentFrame(void) {
    SDL_Surface* surface;

    if (g_SDL_Window == NULL || gBack_screen == NULL || gBack_screen->pixels == NULL) {
        SDL_Delay(16);
        return;
    }

    surface = SDL_GetWindowSurface(g_SDL_Window);
    if (surface == NULL) {
        SDL_Delay(16);
        return;
    }

    {
        int x, y;
        int src_w = gBack_screen->width;
        int src_h = gBack_screen->height;
        int dst_w = surface->w;
        int dst_h = surface->h;
        br_uint_16* src_base = (br_uint_16*)gBack_screen->pixels;
        int src_stride = gBack_screen->row_bytes / 2;

        /* Nearest-neighbour scale from back_screen (640×480) to window size */
        for (y = 0; y < dst_h; y++) {
            Uint32* drow = (Uint32*)((char*)surface->pixels + y * surface->pitch);
            int sy = y * src_h / dst_h;
            if (sy >= src_h) sy = src_h - 1;
            br_uint_16* srow = (br_uint_16*)((char*)src_base + sy * gBack_screen->row_bytes);

            for (x = 0; x < dst_w; x++) {
                int sx = x * src_w / dst_w;
                if (sx >= src_w) sx = src_w - 1;
                br_uint_16 px = srow[sx];
                unsigned r = ((px >> 11) & 0x1F) << 3;
                unsigned g = ((px >> 5) & 0x3F) << 2;
                unsigned b = (px & 0x1F) << 3;
                drow[x] = SDL_MapSurfaceRGB(surface, (Uint8)r, (Uint8)g, (Uint8)b);
            }
        }
        SDL_UpdateWindowSurface(g_SDL_Window);
    }
}

void carpocalypse2_ClearBackScreen(void) {
    if (gBack_screen != NULL && gBack_screen->pixels != NULL) {
        BrPixelmapFill(gBack_screen, 0);
    }
    SDL_Delay(16); /* ~60 fps cap */
}

/* ==== Minimal menu input + rendering for Linux boot ==== */

#include "carpocalypse2_types.h"

static int menu_key_pressed;
static int menu_key_value;
static int menu_quit_requested;

int carpocalypse2_PollMenuInput(int* pCurrent, int count_items) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            menu_key_pressed = 0;
            menu_quit_requested = 1;
            return -2; /* quit */
        }
        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
            case SDLK_UP:
            case SDLK_W:
                *pCurrent = (*pCurrent - 1 + count_items) % count_items;
                break;
            case SDLK_DOWN:
            case SDLK_S:
                *pCurrent = (*pCurrent + 1) % count_items;
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
            case SDLK_SPACE:
                menu_key_pressed = 1;
                return 1; /* activate */
            case SDLK_ESCAPE:
                return -2; /* quit */
            default:
                break;
            }
        }
    }
    return 0;
}

void carpocalypse2_DrawMenuItem(br_pixelmap* pm, int x, int y, const char* text, int highlighted) {
    /* Software bitmap font: 5×7 glyphs, scale 2× per pixel → visible text */
    static const unsigned char font[26][7] = {
        { 0x7E,0x11,0x11,0x11,0x7E }, { 0x7F,0x49,0x49,0x49,0x36 },
        { 0x3E,0x41,0x41,0x41,0x22 }, { 0x7F,0x41,0x41,0x22,0x1C },
        { 0x7F,0x49,0x49,0x49,0x41 }, { 0x7F,0x09,0x09,0x09,0x01 },
        { 0x3E,0x41,0x49,0x49,0x3A }, { 0x7F,0x08,0x08,0x08,0x7F },
        { 0x00,0x41,0x7F,0x41,0x00 }, { 0x20,0x40,0x41,0x3F,0x01 },
        { 0x7F,0x08,0x14,0x22,0x41 }, { 0x7F,0x40,0x40,0x40,0x40 },
        { 0x7F,0x02,0x04,0x02,0x7F }, { 0x7F,0x04,0x08,0x10,0x7F },
        { 0x3E,0x41,0x41,0x41,0x3E }, { 0x7F,0x09,0x09,0x09,0x06 },
        { 0x3E,0x41,0x51,0x21,0x5E }, { 0x7F,0x09,0x19,0x29,0x46 },
        { 0x46,0x49,0x49,0x49,0x31 }, { 0x01,0x01,0x7F,0x01,0x01 },
        { 0x3F,0x40,0x40,0x40,0x3F }, { 0x1F,0x20,0x40,0x20,0x1F },
        { 0x3F,0x40,0x38,0x40,0x3F }, { 0x63,0x14,0x08,0x14,0x63 },
        { 0x07,0x08,0x70,0x08,0x07 }, { 0x61,0x51,0x49,0x45,0x43 },
    };
    br_uint_16 colour;
    br_uint_16 shadow_colour;
    int cx, cy, px, py;

    if (highlighted) {
        /* Draw selection bar */
        int bar_x = x - 20, bar_y = y - 6;
        int bar_w = 300, bar_h = 34;
        int bx, by;
        for (by = 0; by < bar_h; by++) {
            for (bx = 0; bx < bar_w; bx++) {
                int ix = bar_x + bx, iy = bar_y + by;
                if (ix >= 0 && ix < pm->width && iy >= 0 && iy < pm->height) {
                    br_uint_16* p = &((br_uint_16*)pm->pixels)[iy * (pm->row_bytes / 2) + ix];
                    if (by < 2 || by >= bar_h - 2 || bx < 2 || bx >= bar_w - 2) {
                        *p = 0xFFE0; /* yellow border */
                    } else {
                        *p = 0x0842; /* dark navy fill */
                    }
                }
            }
        }
        colour = 0xFFFF;   /* white text */
        shadow_colour = 0x0000;
    } else {
        colour = 0xAD55;   /* grey text */
        shadow_colour = 0x0000;
    }

    /* Draw shadow first */
    for (cy = 0; text[cy]; cy++) {
        char ch = text[cy];
        int ci;
        if (ch >= 'A' && ch <= 'Z') ci = ch - 'A';
        else if (ch >= 'a' && ch <= 'z') ci = ch - 'a';
        else continue;

        for (py = 0; py < 7; py++) {
            unsigned char row = font[ci][py];
            for (px = 0; px < 5; px++) {
                if ((row << px) & 0x80) {
                    int fx_base = x + cy * 16 + px * 2 + 1;
                    int fy_base = y + py * 3 + 1;
                    int sx, sy;
                    for (sy = 0; sy < 3; sy++)
                        for (sx = 0; sx < 2; sx++) {
                            int fx = fx_base + sx, fy = fy_base + sy;
                            if (fx >= 0 && fx < pm->width && fy >= 0 && fy < pm->height)
                                ((br_uint_16*)pm->pixels)[fy * (pm->row_bytes/2) + fx] = shadow_colour;
                        }
                }
            }
        }
    }

    /* Draw text */
    for (cy = 0; text[cy]; cy++) {
        char ch = text[cy];
        int ci;
        if (ch >= 'A' && ch <= 'Z') ci = ch - 'A';
        else if (ch >= 'a' && ch <= 'z') ci = ch - 'a';
        else continue;

        for (py = 0; py < 7; py++) {
            unsigned char row = font[ci][py];
            for (px = 0; px < 5; px++) {
                if ((row << px) & 0x80) {
                    int fx_base = x + cy * 16 + px * 2;
                    int fy_base = y + py * 3;
                    int sx, sy;
                    for (sy = 0; sy < 3; sy++)
                        for (sx = 0; sx < 2; sx++) {
                            int fx = fx_base + sx, fy = fy_base + sy;
                            if (fx >= 0 && fx < pm->width && fy >= 0 && fy < pm->height)
                                ((br_uint_16*)pm->pixels)[fy * (pm->row_bytes/2) + fx] = colour;
                        }
                }
            }
        }
    }
}

void carpocalypse2_DrawBackground(br_pixelmap* pm) {
    int x, y;
    int w = pm->width, h = pm->height;

    /* Dark gradient: dark blue at top → darker at bottom */
    for (y = 0; y < h; y++) {
        br_uint_16* row = (br_uint_16*)((char*)pm->pixels + y * pm->row_bytes);
        /* RGB565: interpolate from (8,8,32) to (2,2,8) */
        unsigned r = 8 - (y * 6 / h);
        unsigned g = 8 - (y * 6 / h);
        unsigned b = 40 - (y * 30 / h);
        br_uint_16 base = (br_uint_16)(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F));
        for (x = 0; x < w; x++) {
            row[x] = base;
        }
    }

    /* Grid pattern overlay for visual interest */
    for (y = 0; y < h; y += 4) {
        br_uint_16* row = (br_uint_16*)((char*)pm->pixels + y * pm->row_bytes);
        for (x = 0; x < w; x += 4) {
            row[x] = 0x0842; /* slightly lighter dot */
        }
    }

    /* Title bar: dark strip at top */
    {
        int bar_h = h / 10;
        for (y = 0; y < bar_h; y++) {
            br_uint_16* row = (br_uint_16*)((char*)pm->pixels + y * pm->row_bytes);
            for (x = 0; x < w; x++) {
                row[x] = 0x0000; /* black title bar */
            }
        }
        /* Yellow separator line under title bar */
        if (bar_h + 2 < h) {
            br_uint_16* row = (br_uint_16*)((char*)pm->pixels + (bar_h+2) * pm->row_bytes);
            for (x = 0; x < w; x++) {
                row[x] = 0xFFE0;
            }
        }
    }

    /* Bottom status bar */
    {
        int y0 = h - h / 12;
        for (y = y0; y < h; y++) {
            br_uint_16* row = (br_uint_16*)((char*)pm->pixels + y * pm->row_bytes);
            for (x = 0; x < w; x++) {
                row[x] = 0x0000;
            }
        }
    }
}

void carpocalypse2_DrawText(br_pixelmap* pm, int x, int y, const char* text, br_uint_16 colour) {
    static const unsigned char font[26][5] = {
        { 0x7E,0x11,0x11,0x11,0x7E }, { 0x7F,0x49,0x49,0x49,0x36 },
        { 0x3E,0x41,0x41,0x41,0x22 }, { 0x7F,0x41,0x41,0x22,0x1C },
        { 0x7F,0x49,0x49,0x49,0x41 }, { 0x7F,0x09,0x09,0x09,0x01 },
        { 0x3E,0x41,0x49,0x49,0x3A }, { 0x7F,0x08,0x08,0x08,0x7F },
        { 0x00,0x41,0x7F,0x41,0x00 }, { 0x20,0x40,0x41,0x3F,0x01 },
        { 0x7F,0x08,0x14,0x22,0x41 }, { 0x7F,0x40,0x40,0x40,0x40 },
        { 0x7F,0x02,0x04,0x02,0x7F }, { 0x7F,0x04,0x08,0x10,0x7F },
        { 0x3E,0x41,0x41,0x41,0x3E }, { 0x7F,0x09,0x09,0x09,0x06 },
        { 0x3E,0x41,0x51,0x21,0x5E }, { 0x7F,0x09,0x19,0x29,0x46 },
        { 0x46,0x49,0x49,0x49,0x31 }, { 0x01,0x01,0x7F,0x01,0x01 },
        { 0x3F,0x40,0x40,0x40,0x3F }, { 0x1F,0x20,0x40,0x20,0x1F },
        { 0x3F,0x40,0x38,0x40,0x3F }, { 0x63,0x14,0x08,0x14,0x63 },
        { 0x07,0x08,0x70,0x08,0x07 }, { 0x61,0x51,0x49,0x45,0x43 },
    };
    int cx, px, py;

    for (cx = 0; text[cx]; cx++) {
        char ch = text[cx];
        int ci;
        if (ch >= 'A' && ch <= 'Z') ci = ch - 'A';
        else if (ch >= 'a' && ch <= 'z') ci = ch - 'a';
        else if (ch >= '0' && ch <= '9') ci = 26;
        else continue;

        for (py = 0; py < 5; py++) {
            unsigned char row = font[ci % 26][py];
            for (px = 0; px < 5; px++) {
                if ((row << px) & 0x80) {
                    int fx = x + cx * 12 + px * 2;
                    int fy = y + py * 2;
                    int sx, sy;
                    for (sy = 0; sy < 2; sy++)
                        for (sx = 0; sx < 2; sx++) {
                            int ix = fx + sx, iy = fy + sy;
                            if (ix >= 0 && ix < pm->width && iy >= 0 && iy < pm->height)
                                ((br_uint_16*)pm->pixels)[iy * (pm->row_bytes/2) + ix] = colour;
                        }
                }
            }
        }
    }
}

/* ==== Menu screen renderer ==== */

void carpocalypse2_DrawMenuScreen(br_pixelmap* pm, tFrontend_spec* pSpec, int current_item) {
    int i;

    carpocalypse2_ClearBackScreen();
    carpocalypse2_DrawBackground(pm);

    /* Title */
    {
        /* Draw "CARMAGEDDON II" using large block letters */
        static const unsigned char f_C[7] = {0x3E,0x41,0x40,0x40,0x40,0x41,0x3E};
        static const unsigned char f_A[7] = {0x18,0x24,0x42,0x7E,0x42,0x42,0x42};
        static const unsigned char f_R[7] = {0x7C,0x42,0x42,0x7C,0x44,0x42,0x42};
        static const unsigned char f_M[7] = {0x42,0x66,0x5A,0x42,0x42,0x42,0x42};
        static const unsigned char f_G[7] = {0x3E,0x40,0x40,0x4E,0x41,0x41,0x3E};
        static const unsigned char f_E[7] = {0x7E,0x40,0x40,0x78,0x40,0x40,0x7E};
        static const unsigned char f_D[7] = {0x7C,0x42,0x42,0x42,0x42,0x42,0x7C};
        static const unsigned char f_O[7] = {0x3E,0x41,0x41,0x41,0x41,0x41,0x3E};
        static const unsigned char f_N[7] = {0x42,0x62,0x52,0x4A,0x46,0x42,0x42};
        const unsigned char* title[] = {f_C,f_A,f_R,f_M,f_A,f_G,f_E,f_D,f_D,f_O,f_N};
        int tx = 100, ty = 20;
        int gi, px, py, sx, sy;
        br_uint_16 colour = 0xFD20; /* orange */

        for (gi = 0; gi < 11; gi++) {
            for (py = 0; py < 7; py++) {
                unsigned char row = title[gi][py];
                for (px = 0; px < 8; px++) {
                    if (row & (0x80 >> px)) {
                        for (sy = 0; sy < 4; sy++)
                            for (sx = 0; sx < 3; sx++) {
                                int ix = tx + gi * 30 + px * 3 + sx;
                                int iy = ty + py * 6 + sy;
                                if (ix >= 0 && ix < pm->width && iy >= 0 && iy < pm->height)
                                    ((br_uint_16*)pm->pixels)[iy * (pm->row_bytes/2) + ix] = colour;
                            }
                    }
                }
            }
        }

        /* "II" subtitle */
        {
            static const unsigned char f_I[7] = {0x7E,0x10,0x10,0x10,0x10,0x10,0x7E};
            tx = 420; ty = 20;
            for (gi = 0; gi < 2; gi++) {
                for (py = 0; py < 7; py++) {
                    unsigned char row = f_I[py];
                    for (px = 0; px < 8; px++) {
                        if (row & (0x80 >> px)) {
                            for (sy = 0; sy < 4; sy++)
                                for (sx = 0; sx < 3; sx++) {
                                    int ix = tx + gi * 30 + px * 3 + sx;
                                    int iy = ty + py * 6 + sy;
                                    if (ix >= 0 && ix < pm->width && iy >= 0 && iy < pm->height)
                                        ((br_uint_16*)pm->pixels)[iy * (pm->row_bytes/2) + ix] = 0xFFFF;
                                }
                        }
                    }
                }
            }
        }
    }

    /* Menu items */
    for (i = 0; i < pSpec->count_items; i++) {
        tFrontend_item_spec* item = &pSpec->items[i];
        if (item->visible) {
            carpocalypse2_DrawMenuItem(pm, item->x, item->y,
                item->text[0] ? item->text : "MENU", (i == current_item));
        }
    }
}
