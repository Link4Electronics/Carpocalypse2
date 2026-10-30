/* Alpha surfaces in a combined bucket/Z-buffer scene must follow opaque geometry.
 * Snapshot geometry and resolved uniforms: effects reuse/update/free source models
 * and materials between RenderAdd calls. Never retain their mutable pointers.
 * The old BRender 1.4 bucket adaptation remains disabled.
 */
#include "drv.h"
#include "transparency.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef struct transparent_vertex {
    float p[3], uv[2], normal[3], colour[4];
} transparent_vertex;

typedef struct transparent_gl_state {
    GLint texture, min_filter, mag_filter, wrap_s, wrap_t;
    GLint src_rgb, dst_rgb, src_alpha, dst_alpha, depth_func, cull_mode;
    GLint viewport[4], framebuffer;
    GLfloat anisotropy, offset_factor, offset_units;
    GLboolean blend, depth, depth_write, cull, offset, colour_write[4];
} transparent_gl_state;

typedef struct gl_transparent_draw {
    struct gl_transparent_draw* next;
    float depth;
    GLsizei vertex_count, index_count;
    shader_data_model model;
#ifdef __vita__
    /* Native Cg reads only this128byte prefix, not the4608byte light array.
     * Thousands of transparent groups otherwise exhaust the Vita game heap. */
    unsigned char scene[offsetof(shader_data_scene, lights)];
#else
    shader_data_scene scene;
#endif
    transparent_gl_state gl;
    /* Followed by vertex_count vertices, then index_count ushort indices. */
} gl_transparent_draw;

static void capture_gl(transparent_gl_state* s) {
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &s->texture);
    if (s->texture != 0) {
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &s->min_filter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &s->mag_filter);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &s->wrap_s);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &s->wrap_t);
        if (GLAD_GL_EXT_texture_filter_anisotropic)
            glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &s->anisotropy);
    }
    glGetIntegerv(GL_BLEND_SRC_RGB, &s->src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &s->dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &s->src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &s->dst_alpha);
    glGetIntegerv(GL_DEPTH_FUNC, &s->depth_func);
    glGetIntegerv(GL_CULL_FACE_MODE, &s->cull_mode);
    glGetIntegerv(GL_VIEWPORT, s->viewport);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &s->framebuffer);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &s->offset_factor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &s->offset_units);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &s->depth_write);
    glGetBooleanv(GL_COLOR_WRITEMASK, s->colour_write);
    s->blend = glIsEnabled(GL_BLEND);
    s->depth = glIsEnabled(GL_DEPTH_TEST);
    s->cull = glIsEnabled(GL_CULL_FACE);
    s->offset = glIsEnabled(GL_POLYGON_OFFSET_FILL);
}

static void enable(GLenum cap, GLboolean value) {
    if (value) glEnable(cap); else glDisable(cap);
}

static void apply_gl(const transparent_gl_state* s) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s->framebuffer);
    glViewport(s->viewport[0], s->viewport[1], s->viewport[2], s->viewport[3]);
    glBindTexture(GL_TEXTURE_2D, s->texture);
    if (s->texture != 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, s->min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, s->mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s->wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, s->wrap_t);
        if (GLAD_GL_EXT_texture_filter_anisotropic)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, s->anisotropy);
    }
    enable(GL_BLEND, s->blend);
    enable(GL_DEPTH_TEST, s->depth);
    enable(GL_CULL_FACE, s->cull);
    enable(GL_POLYGON_OFFSET_FILL, s->offset);
    glBlendFuncSeparate(s->src_rgb, s->dst_rgb, s->src_alpha, s->dst_alpha);
    glDepthFunc(s->depth_func);
    glDepthMask(s->depth_write);
    glCullFace(s->cull_mode);
    glPolygonOffset(s->offset_factor, s->offset_units);
    glColorMask(s->colour_write[0], s->colour_write[1], s->colour_write[2], s->colour_write[3]);
}

br_boolean GLQueueTransparent(br_renderer* renderer, const struct v11group* group, const shader_data_model* model) {
    const state_stack* state = renderer->state.current;
    gl_transparent_draw* draw;
    gl_transparent_draw** link;
    transparent_vertex* vertices;
    br_uint_16* indices;
    float z = 0.f;
    br_size_t bytes;
    int i, j;

    /* Pure Z-buffer/UI/shadow passes keep their immediate ordering. */
    if (state->hidden.type != BRT_BUCKET_AND_BUFFER && state->hidden.type != BRT_BUCKET_SORT)
        return BR_FALSE;
    if (state->output.depth == NULL || state->surface.force_front || state->surface.force_back)
        return BR_FALSE;
    if (!(state->prim.flags & PRIMF_BLEND) &&
        !(state->prim.colour_map != NULL && state->prim.colour_map->blended))
        return BR_FALSE;
    if (group->nvertices == 0 || group->nfaces == 0)
        return BR_FALSE;

    bytes = sizeof(*draw) + group->nvertices * sizeof(*vertices) + group->nfaces * 3 * sizeof(*indices);
    draw = BrResAllocate(renderer, bytes, BR_MEMORY_OBJECT_DATA);
    if (draw == NULL) BrFailure("Out of memory snapshotting transparent geometry");
    draw->vertex_count = group->nvertices;
    draw->index_count = group->nfaces * 3;
    draw->model = *model;
    memcpy(&draw->scene, &renderer->state.cache.scene, sizeof(draw->scene));
    capture_gl(&draw->gl);
    /* Opaque depth is complete at replay; sorted alpha must not occlude itself. */
    draw->gl.depth_write = GL_FALSE;
    vertices = (transparent_vertex*)(draw + 1);
    indices = (br_uint_16*)(vertices + group->nvertices);
    for (i = 0; i < group->nvertices; i++) {
        const struct fmt_vertex* v = &group->vertices[i];
        br_colour c = group->vertex_colours[i];
        for (j = 0; j < 3; j++) {
            vertices[i].p[j] = v->p.v[j];
            vertices[i].normal[j] = v->n.v[j];
        }
        vertices[i].uv[0] = v->map.v[0];
        vertices[i].uv[1] = v->map.v[1];
        vertices[i].colour[0] = BR_RED(c) / 255.f;
        vertices[i].colour[1] = BR_GRN(c) / 255.f;
        vertices[i].colour[2] = BR_BLU(c) / 255.f;
        vertices[i].colour[3] = BR_ALPHA(c) / 255.f;
        z += v->p.v[0] * model->model_view.m[0][2]
            + v->p.v[1] * model->model_view.m[1][2]
            + v->p.v[2] * model->model_view.m[2][2] + model->model_view.m[3][2];
    }
    for (i = 0; i < group->nfaces; i++)
        for (j = 0; j < 3; j++) indices[3 * i + j] = group->faces[i].vertices[j];
    draw->depth = -z / group->nvertices;
    /* Stable back-to-front insertion, including groups from different actors. */
    link = &renderer->transparent_head;
    while (*link != NULL && (*link)->depth >= draw->depth) link = &(*link)->next;
    draw->next = *link;
    *link = draw;
    return BR_TRUE;
}

static void init_buffers(br_renderer* renderer, HVIDEO video) {
    const GLint attributes[] = {video->brenderProgram.attributes.aPosition, video->brenderProgram.attributes.aUV,
        video->brenderProgram.attributes.aNormal, video->brenderProgram.attributes.aColour};
    const GLint sizes[] = {3, 2, 3, 4};
    const size_t offsets[] = {offsetof(transparent_vertex, p), offsetof(transparent_vertex, uv),
        offsetof(transparent_vertex, normal), offsetof(transparent_vertex, colour)};
    int i;
    if (renderer->transparent_vao != 0) return;
    glGenVertexArrays(1, &renderer->transparent_vao);
    glGenBuffers(1, &renderer->transparent_vbo);
    glGenBuffers(1, &renderer->transparent_ibo);
    glBindVertexArray(renderer->transparent_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->transparent_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->transparent_ibo);
    for (i = 0; i < 4; i++) {
        if (attributes[i] < 0) continue;
        glEnableVertexAttribArray(attributes[i]);
        glVertexAttribPointer(attributes[i], sizes[i], GL_FLOAT, GL_FALSE, sizeof(transparent_vertex), (void*)offsets[i]);
    }
}

void GLFlushTransparent(br_renderer* renderer) {
    HVIDEO video;
    gl_transparent_draw* draw;
    transparent_gl_state saved;
    GLint vao, array_buffer, uniform_buffer;
    unsigned int groups = 0, bytes = 0;
    if (renderer->transparent_head == NULL) return;
    video = &renderer->pixelmap->screen->asFront.video;
    capture_gl(&saved);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &array_buffer);
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &uniform_buffer);
    init_buffers(renderer, video);
    glBindVertexArray(renderer->transparent_vao);
    while ((draw = renderer->transparent_head) != NULL) {
        const transparent_vertex* vertices = (const transparent_vertex*)(draw + 1);
        const br_uint_16* indices = (const br_uint_16*)(vertices + draw->vertex_count);
        groups++;
        bytes += sizeof(*draw) + draw->vertex_count * sizeof(*vertices) + draw->index_count * sizeof(*indices);
        apply_gl(&draw->gl);
        glBindBuffer(GL_ARRAY_BUFFER, renderer->transparent_vbo);
        glBufferData(GL_ARRAY_BUFFER, draw->vertex_count * sizeof(*vertices), vertices, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw->index_count * sizeof(*indices), indices, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, video->brenderProgram.uboScene);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(draw->scene), &draw->scene);
        glBindBuffer(GL_UNIFORM_BUFFER, video->brenderProgram.uboModel);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(draw->model), &draw->model);
        glDrawElements(GL_TRIANGLES, draw->index_count, GL_UNSIGNED_SHORT, NULL);
        renderer->transparent_head = draw->next;
        BrResFree(draw);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, video->brenderProgram.uboScene);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, GL_SCENE_UNIFORM_BYTES, &renderer->state.cache.scene);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
    apply_gl(&saved);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    {
        static int logging = -1;
        static unsigned int scenes;
        if (logging < 0) logging = getenv("C2_OBJLOG") != NULL;
        if (logging && (++scenes % 120) == 1)
            BrWarning("ALPHALOG groups=%u snapshot_bytes=%u", groups, bytes);
    }
}

void GLFreeTransparent(br_renderer* renderer) {
    while (renderer->transparent_head != NULL) {
        gl_transparent_draw* draw = renderer->transparent_head;
        renderer->transparent_head = draw->next;
        BrResFree(draw);
    }
    if (renderer->transparent_vao != 0) glDeleteVertexArrays(1, &renderer->transparent_vao);
    if (renderer->transparent_vbo != 0) glDeleteBuffers(1, &renderer->transparent_vbo);
    if (renderer->transparent_ibo != 0) glDeleteBuffers(1, &renderer->transparent_ibo);
    renderer->transparent_vao = renderer->transparent_vbo = renderer->transparent_ibo = 0;
}
