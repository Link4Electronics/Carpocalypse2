/*
 * BRender hardware output for the SDL3 platform.
 *
 * The retail game's screen setup (PDAllocateScreenAndBack) is the 3dfx path:
 * it opens a BRender device and then assumes an accelerated Voodoo-like
 * screen. Until now this port satisfied it with softrend plus sdl3_plib, a
 * memory screen whose primitives are emulated in software and which is known
 * to be broken. This module opens a real BRender hardware driver instead:
 *
 *   C2_RENDERER=sdl3gpu (default)  drivers/sdl3gpurend
 *   C2_RENDERER=gl                 drivers/glrend
 *   C2_RENDERER=software           softrend + SDLREND (legacy fallback)
 *
 * Both hardware drivers share drivers/commonrend and expose the same
 * presentation contract: a front screen device pixelmap, offscreen pixelmaps
 * with a CPU overlay, and a swap callback once a frame has been presented.
 */
#include "sdl3_hwrend.h"

#include "sdl3.h"

#include <SDL3/SDL.h>

#include <brender/brender.h>
#include "drivers/ddi/brender14.h"

/* glrend entry point / callbacks (src/brender/drivers/glrend/include). */
#ifndef BR_EXPORT
#define BR_EXPORT C2_HOOK_CDECL
#endif
#include "brglrend.h"
#include "brgl_display.h"

/* sdl3gpurend entry point / callbacks (src/brender/drivers/sdl3gpurend). */
#include "brsdl3gpurend.h"

#include "core/fw/devlist.h"
#include "core/fw/devsetup.h"

extern SDL_Window* g_SDL_Window;

/* Read by glrend: wireframe debug view and texture anisotropy (0 = off). */
int g_wireframe_mode = 0;
int gAnisotropy_level = 8;
/* Back buffer scale; 0 lets the platform derive it from the window size. */
int gGLRenderScale = 0;

static carpocalypse2_renderer g_renderer = (carpocalypse2_renderer)-1;
/* Set once a hardware device has actually been opened. Until then the
 * predicates below report "software" so the game keeps the memory-screen
 * paths if the hardware driver fails to start. */
static int g_hardware_active = 0;
static SDL_GLContext g_gl_context;

static const char* renderer_name(carpocalypse2_renderer r) {
    switch (r) {
    case C2_RENDERER_GL:
        return "glrend";
    case C2_RENDERER_SDL3GPU:
        return "sdl3gpurend";
    default:
        return "softrend";
    }
}

carpocalypse2_renderer carpocalypse2_GetRenderer(void) {
    if (g_renderer != (carpocalypse2_renderer)-1) {
        return g_renderer;
    }

    {
        const char* e = SDL_getenv("C2_RENDERER");

        if (e == NULL || e[0] == '\0') {
            /* The software screen is the broken path; default to hardware. */
            g_renderer = C2_RENDERER_SDL3GPU;
        } else if (SDL_strcasecmp(e, "gl") == 0 || SDL_strcasecmp(e, "opengl") == 0) {
            g_renderer = C2_RENDERER_GL;
        } else if (SDL_strcasecmp(e, "sdl3gpu") == 0 || SDL_strcasecmp(e, "gpu") == 0
                || SDL_strcasecmp(e, "vulkan") == 0 || SDL_strcasecmp(e, "3dfx") == 0
                || SDL_strcasecmp(e, "d3d") == 0) {
            g_renderer = C2_RENDERER_SDL3GPU;
        } else {
            g_renderer = C2_RENDERER_SOFTWARE;
        }
    }

    SDL_Log("carpocalypse2: renderer '%s' (C2_RENDERER=%s)", renderer_name(g_renderer),
        g_renderer == C2_RENDERER_SOFTWARE ? "software" : (g_renderer == C2_RENDERER_GL ? "gl" : "sdl3gpu"));
    return g_renderer;
}

int carpocalypse2_UseHardwareRenderer(void) {
    return g_hardware_active;
}

int carpocalypse2_UseGL(void) {
    return g_hardware_active && g_renderer == C2_RENDERER_GL;
}

int carpocalypse2_UseSDL3GPU(void) {
    return g_hardware_active && g_renderer == C2_RENDERER_SDL3GPU;
}

SDL_WindowFlags carpocalypse2_RendererWindowFlags(void) {
    if (carpocalypse2_GetRenderer() != C2_RENDERER_GL) {
        return 0;
    }

#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_IOS)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    return SDL_WINDOW_OPENGL;
}

/* C2_TEST_FPS=n caps the frame rate (e.g. 30 to match a slow target). */
static void renderer_frame_limit(void) {
    static int fps = -1;
    static Uint64 last;

    if (fps < 0) {
        const char* e = SDL_getenv("C2_TEST_FPS");
        fps = e != NULL ? SDL_atoi(e) : 0;
    }
    if (fps > 0) {
        Uint64 frame = 1000 / (Uint64)fps;
        Uint64 now = SDL_GetTicks();
        if (last != 0 && now - last < frame) {
            SDL_Delay((Uint32)(frame - (now - last)));
        }
        last = SDL_GetTicks();
    }
}

/* Present the expanded world; UI is proportionally mapped inside this area. */
static void C2_HOOK_CDECL renderer_get_viewport(int* x, int* y, float* width_multiplier, float* height_multiplier) {
    int w = 640, h = 480;

    if (g_SDL_Window != NULL) {
        SDL_GetWindowSizeInPixels(g_SDL_Window, &w, &h);
    }
    {
        int vw, vh;
        C2GLFitOutput(w, h, x, y, &vw, &vh);
        gGLPresentationAspect = (float)vw / (float)vh;
        *width_multiplier = (float)vw / 640.f;
        *height_multiplier = (float)vh / 480.f;
    }
}

static void renderer_swap_done(void) {
    gGLFrameHasWorld = 0;
    gGLFrameHasWide = 0;
}

/* ---------------------------------------------------------------- glrend --- */

static void* renderer_gl_get_proc_address(const char* name) {
    return (void*)SDL_GL_GetProcAddress(name);
}

static void C2_HOOK_CDECL renderer_gl_swap_buffers(br_pixelmap* pm) {
    (void)pm;
    if (g_SDL_Window != NULL) {
        SDL_GL_SwapWindow(g_SDL_Window);
    }
    renderer_swap_done();
    renderer_frame_limit();
}

static br_device_gl_callback_procs g_gl_callbacks = {
    renderer_gl_get_proc_address,
    renderer_get_viewport,
    renderer_gl_swap_buffers,
};

static br_error open_gl_screen(br_pixelmap** screen) {
    br_error r;

    g_gl_context = SDL_GL_CreateContext(g_SDL_Window);
    if (g_gl_context == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext failed (%s)", SDL_GetError());
        return 0x1002; /* BRE_FAIL */
    }
    SDL_GL_MakeCurrent(g_SDL_Window, g_gl_context);
    SDL_GL_SetSwapInterval(1);

    /* Render the 3D at desktop resolution so a maximised/fullscreen window
     * stays sharp. C2_GL_SCALE overrides the automatic choice. */
    {
        const char* e = SDL_getenv("C2_GL_SCALE");
        const SDL_DisplayMode* dm = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
        gGLRenderScale = dm != NULL ? SDL_max((dm->h + 479) / 480, (dm->w + 639) / 640) : 2;
        if (e != NULL && SDL_atoi(e) > 0) {
            gGLRenderScale = SDL_atoi(e);
        }
    }

    BrDevAddStatic(NULL, BrDrv1GLBegin, NULL);
    r = BrDevBeginVar(screen, "glrend",
        BRT_WIDTH_I32, 640,
        BRT_HEIGHT_I32, 480,
        BRT_PIXEL_BITS_I32, 16,
        BRT_PIXEL_TYPE_U8, BR_PMT_RGB_565,
        BRT_OPENGL_CALLBACKS_P, &g_gl_callbacks,
        BR_NULL_TOKEN);
    SDL_Log("glrend: BrDevBeginVar -> %d (GL %s / %s)", (int)r,
        SDL_GL_GetCurrentContext() != NULL ? "context ok" : "no context",
        SDL_GetCurrentVideoDriver());
    return r;
}

/* ------------------------------------------------------------ sdl3gpurend --- */

static void C2_HOOK_CDECL renderer_gpu_swap_buffers(br_pixelmap* pm) {
    (void)pm;
    /* sdl3gpurend already blitted into the swapchain and submitted; the
     * callback only runs the shared presentation bookkeeping. */
    renderer_swap_done();
    renderer_frame_limit();
}

static void C2_HOOK_CDECL renderer_gpu_get_window_size(int* width, int* height) {
    if (g_SDL_Window != NULL) {
        SDL_GetWindowSize(g_SDL_Window, width, height);
    } else {
        *width = 640;
        *height = 480;
    }
}

static void* C2_HOOK_CDECL renderer_gpu_get_window(void) {
    return g_SDL_Window;
}

static br_device_sdl3gpu_callback_procs g_gpu_callbacks = {
    NULL, /* get_proc_address: linked build binds SDL3 directly */
    renderer_get_viewport,
    renderer_gpu_swap_buffers,
    NULL, /* free */
    renderer_gpu_get_window_size,
    renderer_gpu_get_window,
    NULL, /* sdl3_handle: driver uses the SDL3 symbols it was linked against */
};

static br_error open_sdl3gpu_screen(br_pixelmap** screen) {
    br_error r;

    BrDevAddStatic(NULL, BrDrv1SDL3GPURENDBegin, NULL);
    r = BrDevBeginVar(screen, "sdl3gpurend",
        BRT_WIDTH_I32, 640,
        BRT_HEIGHT_I32, 480,
        BRT_PIXEL_BITS_I32, 16,
        BRT_PIXEL_TYPE_U8, BR_PMT_RGB_565,
        BRT_SDL3GPU_CALLBACKS_P, &g_gpu_callbacks,
        BR_NULL_TOKEN);
    SDL_Log("sdl3gpurend: BrDevBeginVar -> %d", (int)r);
    return r;
}

/* ------------------------------------------------------------------ API --- */

br_error carpocalypse2_OpenHardwareScreen(br_pixelmap** screen) {
    br_error r;

    switch (carpocalypse2_GetRenderer()) {
    case C2_RENDERER_GL:
        r = open_gl_screen(screen);
        break;
    case C2_RENDERER_SDL3GPU:
        r = open_sdl3gpu_screen(screen);
        break;
    default:
        return 0x1002;
    }

    if (r == 0 && *screen != NULL) {
        g_hardware_active = 1;
    } else {
        /* Never leave a half-open device behind: the caller falls back to the
         * software screen and its palette/swap paths. */
        *screen = NULL;
        if (r == 0) {
            r = 0x1002;
        }
    }
    return r;
}

void PDDisplayFlush(br_pixelmap* pm) {
    if (carpocalypse2_UseHardwareRenderer() && pm != NULL) {
        BrPixelmapFlush(pm);
    }
}
