#include "drv.h"

// todo assert or warn based on a flag
void GL_AssertOrWarnIfError(char *file, int line) {
    int e;
    const char *kind;
    e = glGetError();
    if (e == GL_NO_ERROR) {
        return;
    }
    switch (e) {
        case GL_INVALID_ENUM:                   kind = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                  kind = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:              kind = "GL_INVALID_OPERATION"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  kind = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                  kind = "GL_OUT_OF_MEMORY"; break;
        default:                                kind = "<unknown>"; break;
    }
    BrWarning("GL error: %s:%d %s (0x%x)\n", file, line, kind, e);
}
