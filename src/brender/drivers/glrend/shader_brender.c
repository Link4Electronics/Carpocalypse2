#include "drv.h"

#include "brassert.h"
#include "brender.frag.glsl.h"
#include "brender.vert.glsl.h"

static void VIDEOI_GetShaderVariables(HVIDEO hVideo) {
    glGenBuffers(1, &hVideo->brenderProgram.uboScene);
    glBindBuffer(GL_UNIFORM_BUFFER, hVideo->brenderProgram.uboScene);
    glUniformBlockBinding(hVideo->brenderProgram.program, hVideo->brenderProgram.blockIndexScene,
        hVideo->brenderProgram.blockBindingScene);
    glBindBufferBase(GL_UNIFORM_BUFFER, hVideo->brenderProgram.blockBindingScene, hVideo->brenderProgram.uboScene);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data_scene), NULL, GL_DYNAMIC_DRAW);

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

br_boolean VIDEOI_CompileBRenderShader(HVIDEO hVideo, const char* vertPath, const char* fragPath) {
    GLuint vert, frag;

    hVideo->brenderProgram.mainTextureBinding = 0;
    hVideo->brenderProgram.blockBindingScene = 1;
    hVideo->brenderProgram.blockBindingModel = 2;

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
    }

    GL_CHECK_ERROR();
    return hVideo->brenderProgram.program != 0;
}
