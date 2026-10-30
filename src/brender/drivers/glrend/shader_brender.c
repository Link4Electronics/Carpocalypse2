#include "drv.h"

#include "brassert.h"
#ifdef __vita__
#include "vita_shaders.h"
#define BRENDER_FRAG_GLSL VITA_BRENDER_FRAG
#define BRENDER_VERT_GLSL VITA_BRENDER_VERT
_Static_assert(offsetof(shader_data_model, surface_colour) == 448, "Cg model colours");
_Static_assert(offsetof(shader_data_model, ka) == 496, "Cg model lighting");
_Static_assert(offsetof(shader_data_model, lighting) == 512, "Cg model flags");
_Static_assert(offsetof(shader_data_model, fog_enabled) == 528, "Cg model fog flag");
_Static_assert(offsetof(shader_data_model, depth_bias) == 532, "Cg model depth bias");
_Static_assert(offsetof(shader_data_model, fog_colour) == 544, "Cg model fog colour");
_Static_assert(sizeof(shader_data_model) == 576, "Cg model buffer size");
_Static_assert(offsetof(shader_data_scene, num_clip_planes) == 112, "Cg scene flags");
_Static_assert(offsetof(shader_data_scene, lights) == 128, "Cg scene buffer prefix");
#else
#include "brender.frag.glsl.h"
#include "brender.vert.glsl.h"
#endif

static void VIDEOI_GetShaderVariables(HVIDEO hVideo) {
    glGenBuffers(1, &hVideo->brenderProgram.uboScene);
    glBindBuffer(GL_UNIFORM_BUFFER, hVideo->brenderProgram.uboScene);
    glUniformBlockBinding(hVideo->brenderProgram.program, hVideo->brenderProgram.blockIndexScene,
        hVideo->brenderProgram.blockBindingScene);
    glBindBufferBase(GL_UNIFORM_BUFFER, hVideo->brenderProgram.blockBindingScene, hVideo->brenderProgram.uboScene);
    glBufferData(GL_UNIFORM_BUFFER, GL_SCENE_UNIFORM_BYTES, NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &hVideo->brenderProgram.uboModel);
    glBindBuffer(GL_UNIFORM_BUFFER, hVideo->brenderProgram.uboModel);
    glUniformBlockBinding(hVideo->brenderProgram.program, hVideo->brenderProgram.blockIndexModel,
        hVideo->brenderProgram.blockBindingModel);
    glBindBufferBase(GL_UNIFORM_BUFFER, hVideo->brenderProgram.blockBindingModel, hVideo->brenderProgram.uboModel);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data_model), NULL, GL_DYNAMIC_DRAW);

    hVideo->brenderProgram.attributes.aPosition = glGetAttribLocation(hVideo->brenderProgram.program, "aPosition");
    hVideo->brenderProgram.attributes.aUV = glGetAttribLocation(hVideo->brenderProgram.program, "aUV");
    hVideo->brenderProgram.attributes.aNormal = glGetAttribLocation(hVideo->brenderProgram.program, "aNormal");
    hVideo->brenderProgram.attributes.aColour = glGetAttribLocation(hVideo->brenderProgram.program, "aColour");
    hVideo->brenderProgram.uniforms.main_texture = glGetUniformLocation(hVideo->brenderProgram.program, "main_texture");
    GL_CHECK_ERROR();
}

#ifdef __vita__
/* Lazily built specialised BRender programs (vita_shaders.h VITA_BRENDER_VARIANT_*), selected per
 * draw by vita_gl.c. Failures are not fatal: the draw keeps the uber program. */
#include <stdio.h>
#include <string.h>
extern void C2VitaLog(const char* format, ...);
extern void C2VitaGLSetBrenderVariants(GLuint main_program, GLuint (*get)(unsigned key));
enum { VITA_VARIANT_KEYS = 128 };
static GLuint vita_variant_program[VITA_VARIANT_KEYS];
static unsigned char vita_variant_state[VITA_VARIANT_KEYS]; /* 0 untried, 1 ok, 2 failed */
static HVIDEO vita_variant_video;

static GLuint VitaCompileVariantShader(GLenum type, const char* defines, const char* body) {
    static char source[sizeof(VITA_BRENDER_VARIANT_FRAG) + sizeof(VITA_BRENDER_VARIANT_VERT) + 256];
    const GLchar* text = source;
    GLint length, status = GL_FALSE;
    GLuint s;
    length = snprintf(source, sizeof(source), "%s%s", defines, body);
    if (length <= 0 || length >= (GLint)sizeof(source)) return 0;
    s = glCreateShader(type);
    glShaderSource(s, 1, &text, &length);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char log[512] = "";
        GLint written = 0;
        glGetShaderInfoLog(s, sizeof(log), &written, log);
        log[sizeof(log) - 1] = 0;
        C2VitaLog("VITA SHADER variant %s compile FAILED: %s\n", type == GL_VERTEX_SHADER ? "vert" : "frag", log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

static GLuint VitaBrenderVariant(unsigned key) {
    HVIDEO hVideo = vita_variant_video;
    char defines[256];
    GLuint vert, frag, program;
    GLint status = GL_FALSE, previous = 0;
    GLuint index;
    GLint location;
    static const char* const names[4] = { "aPosition", "aUV", "aNormal", "aColour" };
    GLint wanted[4];
    int i;

    if (key >= VITA_VARIANT_KEYS || hVideo == NULL) return 0;
    if (vita_variant_state[key] == 1) return vita_variant_program[key];
    if (vita_variant_state[key] == 2) return 0;
    vita_variant_state[key] = 2;

    snprintf(defines, sizeof(defines),
        "#define C2_CLIP %d\n#define C2_ENV %d\n#define C2_TEX %d\n#define C2_KEY %d\n"
        "#define C2_ALPHA %d\n#define C2_FOG %d\n#define C2_MOD %d\n",
        !!(key & 1), !!(key & 2), !!(key & 4), !!(key & 8), !!(key & 16), !!(key & 32), !!(key & 64));
    vert = VitaCompileVariantShader(GL_VERTEX_SHADER, defines, VITA_BRENDER_VARIANT_VERT);
    if (!vert) return 0;
    frag = VitaCompileVariantShader(GL_FRAGMENT_SHADER, defines, VITA_BRENDER_VARIANT_FRAG);
    if (!frag) { glDeleteShader(vert); return 0; }
    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    /* the same fixed slots as VIDEOI_CreateAndCompileProgram, so model VAOs work unchanged */
    glBindAttribLocation(program, 0, "aPosition");
    glBindAttribLocation(program, 1, "aColour");
    glBindAttribLocation(program, 2, "aUV");
    glBindAttribLocation(program, 3, "aNormal");
    glLinkProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        C2VitaLog("VITA SHADER variant key=%02x link FAILED\n", key);
        glDeleteProgram(program);
        return 0;
    }
    wanted[0] = hVideo->brenderProgram.attributes.aPosition;
    wanted[1] = hVideo->brenderProgram.attributes.aUV;
    wanted[2] = hVideo->brenderProgram.attributes.aNormal;
    wanted[3] = hVideo->brenderProgram.attributes.aColour;
    for (i = 0; i < 4; ++i) {
        location = glGetAttribLocation(program, names[i]);
        if (location != -1 && location != wanted[i]) {
            C2VitaLog("VITA SHADER variant key=%02x attribute %s at %d, main at %d: rejected\n",
                key, names[i], location, wanted[i]);
            glDeleteProgram(program);
            return 0;
        }
    }
    index = glGetUniformBlockIndex(program, "br_scene_state");
    if (index != GL_INVALID_INDEX) glUniformBlockBinding(program, index, hVideo->brenderProgram.blockBindingScene);
    index = glGetUniformBlockIndex(program, "br_model_state");
    if (index != GL_INVALID_INDEX) glUniformBlockBinding(program, index, hVideo->brenderProgram.blockBindingModel);
    location = glGetUniformLocation(program, "main_texture");
    if (location >= 0) {
        glGetIntegerv(GL_CURRENT_PROGRAM, &previous);
        glUseProgram(program);
        glUniform1i(location, hVideo->brenderProgram.mainTextureBinding);
        glUseProgram((GLuint)previous);
    }
    vita_variant_program[key] = program;
    vita_variant_state[key] = 1;
    C2VitaLog("VITA SHADER variant key=%02x ready program=%u\n", key, program);
    return program;
}
#endif

br_boolean VIDEOI_CompileBRenderShader(HVIDEO hVideo, const char* vertPath, const char* fragPath) {
    GLuint vert, frag;

    hVideo->brenderProgram.mainTextureBinding = 0;
    hVideo->brenderProgram.blockBindingScene = 1;
    hVideo->brenderProgram.blockBindingModel = 2;

#ifndef __vita__
    {
#define _MAX(a, b) ((a) > (b) ? (a) : (b))
        int neededSize = _MAX(sizeof(shader_data_scene), sizeof(shader_data_model));
#undef _MAX
        if (hVideo->maxUniformBlockSize < neededSize) {
            BR_FATAL2("VIDEO: GL_MAX_UNIFORM_BLOCK_SIZE too small, got %d, needed %d.", hVideo->maxUniformBlockSize,
                neededSize);
            return BR_FALSE;
        }
    }

    if (hVideo->maxUniformBufferBindings < 2) {
        BR_FATAL1("VIDEO: GL_MAX_UNIFORM_BUFFER_BINDINGS too small, got %d, needed 2.", hVideo->maxUniformBufferBindings);
        return BR_FALSE;
    }

    /* br_model_state */
    if (hVideo->maxVertexUniformBlocks < 1) {
        BR_FATAL1("VIDEO: GL_MAX_VERTEX_UNIFORM_BLOCKS too small, got %d, needed 1.", hVideo->maxVertexUniformBlocks);
        return BR_FALSE;
    }

    /* br_model_state, br_scene_state */
    if (hVideo->maxFragmentUniformBlocks < 2) {
        BR_FATAL1("VIDEO: GL_MAX_FRAGMENT_UNIFORM_BLOCKS too small, got %d, needed 2.", hVideo->maxFragmentUniformBlocks);
        return BR_FALSE;
    }

#endif
    vert = VIDEOI_CreateAndCompileShader("brender.vert", GL_VERTEX_SHADER, BRENDER_VERT_GLSL, sizeof(BRENDER_VERT_GLSL));
    if (!vert)
        return BR_FALSE;

    frag = VIDEOI_CreateAndCompileShader("brender.frag", GL_FRAGMENT_SHADER, BRENDER_FRAG_GLSL, sizeof(BRENDER_FRAG_GLSL));
    if (!frag) {
        glDeleteShader(vert);
        return BR_FALSE;
    }

    if (!(hVideo->brenderProgram.program = VIDEOI_CreateAndCompileProgram(vert, frag))) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return BR_FALSE;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    if (hVideo->brenderProgram.program) {
        hVideo->brenderProgram.blockIndexScene = glGetUniformBlockIndex(hVideo->brenderProgram.program,
            "br_scene_state");
        if (hVideo->brenderProgram.blockIndexScene == GL_INVALID_INDEX) {
            BR_FATAL("VIDEO: Unable to retrieve block index for uniform block 'br_scene_state'.");
            return BR_FALSE;
        }

        hVideo->brenderProgram.blockIndexModel = glGetUniformBlockIndex(hVideo->brenderProgram.program,
            "br_model_state");
        if (hVideo->brenderProgram.blockIndexModel == GL_INVALID_INDEX) {
            BR_FATAL("VIDEO: Unable to retrieve block index for uniform block 'br_model_state'.");
            return BR_FALSE;
        }

        VIDEOI_GetShaderVariables(hVideo);
#ifdef __vita__
        vita_variant_video = hVideo;
        C2VitaGLSetBrenderVariants(hVideo->brenderProgram.program, VitaBrenderVariant);
        {
            /* build the common textured combinations (key/alpha/fog/modulate) during loading,
             * so the race does not hitch on first use; the rest stay lazy */
            unsigned k;
            for (k = 0; k < 16; ++k)
                VitaBrenderVariant(4u | ((k & 1) ? 8u : 0u) | ((k & 2) ? 16u : 0u) | ((k & 4) ? 32u : 0u) | ((k & 8) ? 64u : 0u));
        }
#endif
    }

    GL_CHECK_ERROR();
    return hVideo->brenderProgram.program != 0;
}
