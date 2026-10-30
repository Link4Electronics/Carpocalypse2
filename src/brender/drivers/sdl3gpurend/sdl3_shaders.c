#include "sdl3_shaders.h"

#include "sdl3gpurend_shader_formats.h"

#include "brender.vert.spv.h"
#include "brender.frag.spv.h"
#include "overlay.vert.spv.h"
#include "overlay.frag.spv.h"
#include "text.vert.spv.h"
#include "text.frag.spv.h"

#if SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE
#include "brender.vert.metal.h"
#include "brender.frag.metal.h"
#include "overlay.vert.metal.h"
#include "overlay.frag.metal.h"
#include "text.vert.metal.h"
#include "text.frag.metal.h"
#endif

#if SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE
#include "brender.vert.dxil.h"
#include "brender.frag.dxil.h"
#include "overlay.vert.dxil.h"
#include "overlay.frag.dxil.h"
#include "text.vert.dxil.h"
#include "text.frag.dxil.h"
#endif

/* The msl/dxil members of SDL3GPUREND_ShaderSource for a data-array prefix (e.g.
 * BRENDER_VERT), or NULL/0 when that format was not produced in this build. */
#if SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE
#define SDL3GPUREND_MSL(name) name##_METAL, sizeof(name##_METAL),
#else
#define SDL3GPUREND_MSL(name) NULL, 0,
#endif

#if SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE
#define SDL3GPUREND_DXIL(name) name##_DXIL, sizeof(name##_DXIL),
#else
#define SDL3GPUREND_DXIL(name) NULL, 0,
#endif

/* Expands to a complete SDL3GPUREND_ShaderSource initializer for a shader stage
 * data-array prefix (e.g. BRENDER_VERT). Initializers are inlined (instead of
 * referencing static struct objects) so the arrays are valid MSVC C11: MSVC
 * rejects static initializers that refer to non-constant objects (C2099). */
#define SDL3GPUREND_SOURCE(name)                          \
    {                                                  \
        name##_SPV, sizeof(name##_SPV),                \
        SDL3GPUREND_MSL(name) SDL3GPUREND_DXIL(name)         \
    }

const SDL3GPUREND_ShaderSource brender_shaders[2] = {
    SDL3GPUREND_SOURCE(BRENDER_VERT),
    SDL3GPUREND_SOURCE(BRENDER_FRAG),
};

const SDL3GPUREND_ShaderSource overlay_shaders[2] = {
    SDL3GPUREND_SOURCE(OVERLAY_VERT),
    SDL3GPUREND_SOURCE(OVERLAY_FRAG),
};

/* The driver's 2D content uses the same vertex/fragment pair. */
const SDL3GPUREND_ShaderSource default_shaders[2] = {
    SDL3GPUREND_SOURCE(BRENDER_VERT),
    SDL3GPUREND_SOURCE(BRENDER_FRAG),
};

/* Glyph-quad pair for device BrPixelmapText. */
const SDL3GPUREND_ShaderSource text_shaders[2] = {
    SDL3GPUREND_SOURCE(TEXT_VERT),
    SDL3GPUREND_SOURCE(TEXT_FRAG),
};
