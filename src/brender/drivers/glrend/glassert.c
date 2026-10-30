#include "drv.h"

// todo assert or warn based on a flag
void GL_AssertOrWarnIfError(char *file, int line) {
    int e;
    const char *kind;
    e = glGetError();
    if (e == GL_NO_ERROR) {
        return;
    }
#ifdef __vita__
    /* A per-group fault must remain diagnosable without synchronous card I/O
     * dominating every draw. Retain the first occurrence and exponential counts. */
    {
        static struct {const char* file; int line, error; unsigned int count;} sites[32];
        int i;
        for (i=0; i<32; ++i) {
            if (sites[i].file == NULL || (sites[i].file == file && sites[i].line == line && sites[i].error == e)) {
                unsigned int count;
                sites[i].file=file; sites[i].line=line; sites[i].error=e;
                count=++sites[i].count;
                if ((count & (count-1)) != 0) return;
                break;
            }
        }
        if (i==32) return;
    }
#endif
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
