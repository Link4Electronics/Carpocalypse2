#ifndef sdl3_shaders_h
#define sdl3_shaders_h

#include <stddef.h>

/*
 * One shader stage in every format the driver can feed SDL3 GPU. Exactly one
 * field is used at runtime, selected by the device's shader format (see
 * video.c SDL3GPUREND_CreateShader). Fields for formats this build did not
 * produce are NULL/0 (see sdl3gpurend_shader_formats.h and the CMake toolchain).
 */
typedef struct SDL3GPUREND_ShaderSource {
    const char* spirv;
    size_t spirv_size;
    const char* msl;
    size_t msl_size;
    const char* dxil;
    size_t dxil_size;
} SDL3GPUREND_ShaderSource;

/* Index of a stage within a shader pair. */
#define SDL3GPUREND_STAGE_VERTEX 0
#define SDL3GPUREND_STAGE_FRAGMENT 1

/* Embedded shader pairs: { vertex, fragment }. default_shaders aliases the
 * brender pair (the driver's 2D content uses the same vertex/fragment).
 * text_shaders is the glyph-quad pair used by device BrPixelmapText. */
extern const SDL3GPUREND_ShaderSource brender_shaders[2];
extern const SDL3GPUREND_ShaderSource overlay_shaders[2];
extern const SDL3GPUREND_ShaderSource default_shaders[2];
extern const SDL3GPUREND_ShaderSource text_shaders[2];

#endif
