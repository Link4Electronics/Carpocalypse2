#include "drv.h"
#include "brassert.h"
#include "default.vert.glsl.h"
#include "text.frag.glsl.h"


br_boolean VIDEOI_CompileTextShader(HVIDEO hVideo) {
    GLuint vert = VIDEOI_CreateAndCompileShader("text.vert", GL_VERTEX_SHADER, DEFAULT_VERT_GLSL, sizeof(DEFAULT_VERT_GLSL));
    if (!vert)
        return BR_FALSE;
    GLuint frag = VIDEOI_CreateAndCompileShader("text.frag", GL_FRAGMENT_SHADER, TEXT_FRAG_GLSL, sizeof(TEXT_FRAG_GLSL));
    if (!frag) {
        glDeleteShader(vert);
        return BR_FALSE;
    }
    hVideo->textProgram.program = VIDEOI_CreateAndCompileProgram(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    if (hVideo->textProgram.program) {
        hVideo->textProgram.aPosition = glGetAttribLocation(hVideo->textProgram.program, "aPosition");
        hVideo->textProgram.aUV = glGetAttribLocation(hVideo->textProgram.program, "aUV");

        hVideo->textProgram.uSampler = glGetUniformLocation(hVideo->textProgram.program, "uSampler");
        hVideo->textProgram.uTextColour = glGetUniformLocation(hVideo->textProgram.program, "uTextColour");
        glUseProgram(hVideo->textProgram.program);
    }

    GL_CHECK_ERROR();
    return hVideo->textProgram.program != 0;
}
