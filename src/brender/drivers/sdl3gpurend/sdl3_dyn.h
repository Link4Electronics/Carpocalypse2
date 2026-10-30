#ifndef sdl3gpurend_dyn_h
#define sdl3gpurend_dyn_h

/* SDL3 symbol table for the sdl3gpurend driver: one typedef and one extern
 * function pointer (SDL3_<name>) per SDL3 function, generated from
 * sdl3_syms.h. The driver calls SDL3 only through these pointers, so it works
 * both linked against SDL3 (standalone BRender builds) and with SDL3 resolved
 * at runtime from a host-provided library handle (dethrace single-executable
 * builds, where the SDL3 symbols live in the harness and are resolved with
 * dlsym()/GetProcAddress()).
 *
 * SDL3GPUREND_LoadSDLSymbols() (sdl3_dyn.c) must be called before any SDL3 use;
 * the driver does this in SDL3GPUREND_VideoOpen(), passing the sdl3_handle field
 * of the br_device_sdl3gpu_callback_procs structure.
 */

#include "sdl3_syms.h"

#define SDL3GPUREND_X_TYPEDEF(name, ret, args) \
    typedef ret SDLCALL tSDL3GPUREND_##name##_fn args;
#define SDL3GPUREND_X_EXTERN(name, ret, args) \
    extern tSDL3GPUREND_##name##_fn* SDL3_##name;

FOREACH_SDL3GPUREND_SYM(SDL3GPUREND_X_TYPEDEF)
FOREACH_SDL3GPUREND_SYM(SDL3GPUREND_X_EXTERN)

/* Resolves every SDL3_<name> pointer. `handle` is a handle to the already
 * loaded SDL3 library (an HMODULE on Windows, the dlopen() handle on POSIX);
 * if NULL the SDL3 library is loaded itself. Returns 0 on success, non-zero
 * if any symbol could not be resolved. When built without
 * SDL3GPUREND_SDL_DYNAMIC this binds the pointers directly to the SDL3 functions
 * and `handle` is ignored. */
int SDL3GPUREND_LoadSDLSymbols(void* handle);

#endif /* sdl3gpurend_dyn_h */
