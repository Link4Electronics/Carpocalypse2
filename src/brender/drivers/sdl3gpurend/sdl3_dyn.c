/* SDL3 symbol resolution for the sdl3gpurend driver. */
#include "sdl3_dyn.h"

#include <stdio.h>

#if defined(SDL3GPUREND_SDL_DYNAMIC)

/* Resolve SDL3 at runtime from a host-provided handle, loading the SDL3
 * library ourselves if none is given. The library being loaded is SDL itself,
 * so this deliberately uses the OS loader (dlopen / LoadLibrary) rather than
 * SDL_LoadObject. */
#ifdef _WIN32
#include <windows.h>

static void* LoadObject(const char* name) {
    return LoadLibraryA(name);
}

static void* LoadFunction(void* obj, const char* name) {
    return (void*)GetProcAddress((HMODULE)obj, name);
}
#else
#include <dlfcn.h>

static void* LoadObject(const char* name) {
    return dlopen(name, RTLD_NOW | RTLD_LOCAL);
}

static void* LoadFunction(void* obj, const char* name) {
    return dlsym(obj, name);
}
#endif

static const char* const possible_locations[] = {
#ifdef _WIN32
    "SDL3.dll",
#elif defined(__APPLE__)
    "libSDL3.dylib",
#else
    "libSDL3.so.0",
    "libSDL3.so",
#endif
    NULL,
};

#endif /* SDL3GPUREND_SDL_DYNAMIC */

#define SDL3GPUREND_X_DEFINE(name, ret, args) tSDL3GPUREND_##name##_fn* SDL3_##name;
FOREACH_SDL3GPUREND_SYM(SDL3GPUREND_X_DEFINE)

int SDL3GPUREND_LoadSDLSymbols(void* handle) {
#if defined(SDL3GPUREND_SDL_DYNAMIC)
    if (handle == NULL) {
        for (int i = 0; possible_locations[i] != NULL; i++) {
            handle = LoadObject(possible_locations[i]);
            if (handle != NULL) {
                break;
            }
        }
    }
    if (handle == NULL) {
        fprintf(stderr, "sdl3gpurend: unable to load the SDL3 shared library\n");
        return 1;
    }
#define SDL3GPUREND_X_LOAD(name, ret, args)                                    \
    SDL3_##name = (tSDL3GPUREND_##name##_fn*)LoadFunction(handle, "SDL_" #name); \
    if (SDL3_##name == NULL) {                                              \
        fprintf(stderr, "sdl3gpurend: missing SDL3 symbol SDL_" #name "\n");   \
        return 1;                                                           \
    }
#else
    (void)handle;
    /* Linked against SDL3: bind the pointers straight to the functions. */
#define SDL3GPUREND_X_LOAD(name, ret, args) SDL3_##name = SDL_##name;
#endif

    FOREACH_SDL3GPUREND_SYM(SDL3GPUREND_X_LOAD)
    return 0;
}
