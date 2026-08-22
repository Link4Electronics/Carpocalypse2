/*
 * VIDEO structures
 */
#ifndef VIDEO_H_
#define VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "shader_data.h"

struct br_font;

/* Maximum number of font atlases cached in the VIDEO instance. */
#define TEXT_ATLAS_CACHE_MAX 8

typedef struct {
    struct br_font* font;
    GLuint texture;
    int atlasWidth;
    int atlasHeight;
} text_atlas_cache_entry;

typedef struct _VIDEO {
    GLint maxUniformBlockSize;
    GLint maxUniformBufferBindings;
    GLint maxVertexUniformBlocks;
    GLint maxFragmentUniformBlocks;
    GLint maxSamples;
    GLfloat maxAnisotropy;

    struct {
        GLuint program;
        GLint aPosition;     /* Position, vec3 */
        GLint aColour;       /* Colour, vec3 */
        GLint aUV;           /* UV, vec2 */
        GLint uSampler;      /* Sampler, sampler2D */
        GLint uMVP;          /* Model-View-Projection Matrix, mat4 */
        GLint uFlipVertically;
        GLint uDiscardPurplePixels;
    } defaultProgram;

    struct {
        GLuint program;

        struct {
            GLint aPosition; /* Vectex Position, vec3 */
            GLint aUV;       /* UV, vec2 */
            GLint aNormal;   /* Vertex Normal, vec3 */
            GLint aColour;   /* Vertex colour, vec4 */
        } attributes;

        struct {
            GLint main_texture; /* sampler2D */
        } uniforms;

        GLuint uboScene;
        GLuint blockIndexScene;
        GLuint blockBindingScene;

        GLuint uboModel;
        GLuint blockIndexModel;
        GLuint blockBindingModel;

        GLint mainTextureBinding;
    } brenderProgram;

    struct {
        GLuint program;

        GLint aPosition;     /* Position, vec2 */
        GLint aUV;           /* UV, vec2 */
        GLint uSampler;      /* Sampler, sampler2D */
        GLint uTextColour;   /* Text colour, vec4 */
    } textProgram;

    /* Lazily-built font atlases (16x16 glyph grid). */
    text_atlas_cache_entry textAtlas[TEXT_ATLAS_CACHE_MAX];
    int textAtlasCount;
    int textAtlasReplace;
} VIDEO, *HVIDEO;

#ifdef __cplusplus
};
#endif

/* UBO data must be in native byte order (OpenGL spec). No-op on all platforms. */
static inline void byteswap_ubo(void *buf, size_t size_bytes) {
    (void)buf;
    (void)size_bytes;
}

#endif /* VIDEO_H_ */
