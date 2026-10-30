#ifndef CARPOCALYPSE2_SDL3_HWREND_H
#define CARPOCALYPSE2_SDL3_HWREND_H

#include "carpocalypse2_types.h"

#include <SDL3/SDL.h>

#include <brender/brender.h>

/*
 * BRender hardware output for the SDL3 platform.
 *
 * The retail game only knows the 3dfx path: PDAllocateScreenAndBack() opens a
 * BRender device and then assumes an accelerated, Voodoo-like screen. This
 * module supplies that screen from BRender hardware drivers instead of the
 * memory-backed sdl3_plib software device:
 *
 *   C2_RENDERER=sdl3gpu (default)  drivers/sdl3gpurend  (SDL3 GPU / Vulkan)
 *   C2_RENDERER=gl                 drivers/glrend       (OpenGL 3.2 core)
 *   C2_RENDERER=software           softrend + SDLREND  (legacy, broken)
 *
 * Both hardware drivers are built from the shared drivers/commonrend stack.
 */

typedef enum {
    C2_RENDERER_SOFTWARE = 0,
    C2_RENDERER_GL,
    C2_RENDERER_SDL3GPU,
} carpocalypse2_renderer;

/* Selected renderer (C2_RENDERER, cached after the first call). */
carpocalypse2_renderer carpocalypse2_GetRenderer(void);

/* Non-zero when the selected renderer is a BRender hardware driver. */
int carpocalypse2_UseHardwareRenderer(void);

/* Non-zero for glrend; kept as a separate predicate because the game-side
 * palette/swap hooks need to know whether they talk to GL or to SDL3 GPU. */
int carpocalypse2_UseGL(void);

/* Non-zero for sdl3gpurend. */
int carpocalypse2_UseSDL3GPU(void);

/* Extra SDL window flags needed before SDL_CreateWindow() (GL context). */
SDL_WindowFlags carpocalypse2_RendererWindowFlags(void);

/* Opens the selected hardware device into *screen. Returns a BRender error. */
br_error carpocalypse2_OpenHardwareScreen(br_pixelmap** screen);

/* Flush hook used by the game before it draws the 3D scene: uploads any CPU
 * pixel writes (palette/headup text) that the driver composites as an overlay. */
void PDDisplayFlush(br_pixelmap* pm);

#endif /* CARPOCALYPSE2_SDL3_HWREND_H */
