#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <signal.h>
#if defined(SDL_PLATFORM_ANDROID)
#include <unistd.h>
#endif

#include "sdl3.h"
#include "sdl3_platform.h"

#include "errors.h"
#include "globvars.h"
#include "main.h"
#include "pedestrn.h"

extern int carpocalypse2_MenuQuitRequested(void);
extern void carpocalypse2_RequestQuit(void);

static void carpocalypse2_SignalHandler(int sig) {
    (void)sig;
    carpocalypse2_RequestQuit();
}

#if defined(SDL_PLATFORM_ANDROID)
// Android starts an app with "/" as the working directory, which is
// read-only, and the engine resolves everything against the working
// directory: PDBuildAppPath() below is SDL_GetCurrentDirectory() + "DATA",
// MUSIC/TrackNN.ogg is opened relative to it, and DATA/OPTIONS.TXT,
// DATA/SAVEDGAMES.ARS and DIAGNOST.TXT are written back into the same tree.
// So the process has to chdir into storage it owns before GameMain runs.
//
// Prefer the app's own external files directory
// (/storage/emulated/0/Android/data/com.carpocalypse2.game/files): it needs
// no storage permission and it is where the APK's install notes tell the
// player to push DATA/ and MUSIC/ with adb. Fall back to the internal files
// directory if the external one cannot be entered (unmounted card, locked
// profile).
//
// SDL_CreateDirectory() makes exactly one level and reports an
// already-present directory as a failure, so walk the path creating each
// level and ignore those returns: chdir() is what decides.
static int carpocalypse2_TryDataDir(const char *root) {
    char buf[1024];
    char *p;
    size_t len;

    len = SDL_strlen(root);
    if (len == 0 || len >= sizeof(buf)) {
        return 0;
    }
    SDL_memcpy(buf, root, len + 1);
    for (p = buf + 1; *p != '\0'; p++) {
        if (*p == '/') {
            *p = '\0';
            SDL_CreateDirectory(buf);
            *p = '/';
        }
    }
    SDL_CreateDirectory(buf);
    return chdir(buf) == 0;
}

static int carpocalypse2_EnterAndroidDataDir(void) {
    const char *root;

    // Called after SDL_Init: both accessors go through SDL's JNI binding.
    root = SDL_GetAndroidExternalStoragePath();
    if (root != NULL && carpocalypse2_TryDataDir(root)) {
        return 1;
    }
    root = SDL_GetAndroidInternalStoragePath();
    if (root != NULL && carpocalypse2_TryDataDir(root)) {
        return 1;
    }
    return 0;
}
#endif

int main(int argc, char *argv[]) {
    int i;
    char *path;

    signal(SIGINT, carpocalypse2_SignalHandler);
    signal(SIGTERM, carpocalypse2_SignalHandler);

    gAFE = 0;
    SetDefaultPedFolderNames();
    for (i = 1; i < argc; i++) {
        if (SDL_strcmp(argv[i], "-NOCUTSCENE") == 0 || SDL_strcmp(argv[i], "-NOCUTSCENES") == 0) {
            gNoCutscenes = 1;
        } else if (SDL_strcmp(argv[i], "-ZOMBIE") == 0) {
            SetZombiePedFolderNames();
        } else if (SDL_strcmp(argv[i], "-BLOOD") == 0) {
            SetBloodPedFolderNames();
        } else if (SDL_strcmp(argv[i], "-ALIEN") == 0) {
            SetAlienPedFolderNames();
        } else if (SDL_strcmp(argv[i], "-AFE") == 0) {
            gAFE = 1;
//        } else if (SDL_strcmp(argv[i], "-SCALEMOUSE") == 0) {
//            gScaleMouse = 1;
        } else {
            dr_dprintf("Unknown argument: \"%s\"", argv[i]);
            return 1;
        }
    }

    // Before the first path lookup: on Android this chdirs into the data
    // directory, and SDL_GetCurrentDirectory() below must report where the
    // game actually runs from afterwards.
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init failed", SDL_GetError(), NULL);
        return 1;
    }

#if defined(SDL_PLATFORM_ANDROID)
    if (!carpocalypse2_EnterAndroidDataDir()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No writable data directory (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "carpocalypse2",
            "No writable data directory on this device.", NULL);
        SDL_Quit();
        return 1;
    }
#endif

    path = SDL_GetCurrentDirectory();
    if (path == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetCurrentDirectory() failed (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_GetCurrentDirectory failed", SDL_GetError(), NULL);
        SDL_Quit();
        return 1;
    }
    SDL_strlcpy(gPathNetworkIni, path, SDL_arraysize(gPathNetworkIni));
    SDL_free(path);

    g_PerformanceFrequency = SDL_GetPerformanceFrequency();

    g_SDL_Window = SDL_CreateWindow("carpocalypse2", 640, 480, SDL_WINDOW_RESIZABLE);
    if (g_SDL_Window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow failed", SDL_GetError(), NULL);
        SDL_Quit();
        return 1;
    }
    gHWnd = g_SDL_Window;

    const char* args[] = { "carpocalypse2", NULL };

    GameMain(1, args);

    SDL_Quit();
    return 0;
}
