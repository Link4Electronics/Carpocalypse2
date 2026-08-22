/*
 * Support routines for rendering models
 */
#include "brassert.h"
#include "drv.h"
#include "commonrend.h"
#include "pixconv.h"
#include <string.h>

static void apply_blend_mode(state_stack* self) {
    /* C_result = (C_source * F_Source) + (C_dest * F_dest) */

    /* NB: srcAlpha and dstAlpha are all GL_ONE and GL_ZERO respectively. */
    switch (self->prim.blend_mode) {
    default:
        /* fallthrough */
    case BRT_BLEND_STANDARD:
        /* fallthrough */
    case BRT_BLEND_DIMMED:
        /*
         * 3dfx blending mode = 1
         * Colour = (alpha * src) + ((1 - alpha) * dest)
         * Alpha  = (1     * src) + (0           * dest)
         */
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        break;

    case BRT_BLEND_SUMMED:
        /*
         * 3fdx blending mode = 4
         * Colour = (alpha * src) + (1 * dest)
         * Alpha  = (1     * src) + (0 * dest)
         */
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);
        break;

    case BRT_BLEND_PREMULTIPLIED:
        /*
         * 3dfx qblending mode = 2
         * Colour = (1 * src) + ((1 - alpha) * dest)
         * Alpha  = (1 * src) + (0           * dest)
         */
        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        break;
    }
    GL_CHECK_ERROR();
}

static void apply_depth_properties(state_stack* state, uint32_t states) {
    br_boolean depth_valid = BR_TRUE; /* Defaulting to BR_TRUE to keep existing behaviour. */
    GLenum depth_test = GL_NONE;

    /* Only use the states we want (if valid). */
    states = state->valid & states;

    if (states & MASK_STATE_OUTPUT) {
        depth_valid = state->output.depth != NULL;
    }

    if (states & MASK_STATE_SURFACE) {
        if (state->surface.force_front || state->surface.force_back)
            depth_test = GL_FALSE;
        else
            depth_test = GL_TRUE;
    }

    if (depth_valid == BR_TRUE) {
        if (depth_test == GL_TRUE)
            glEnable(GL_DEPTH_TEST);
        else if (depth_test == GL_FALSE)
            glDisable(GL_DEPTH_TEST);
    }

    if (states & MASK_STATE_PRIMITIVE) {
        if (state->prim.flags & PRIMF_DEPTH_WRITE)
            glDepthMask(GL_TRUE);
        else
            glDepthMask(GL_FALSE);

        GLenum depthFunc;
        switch (state->prim.depth_test) {
        case BRT_LESS:
            depthFunc = GL_LESS;
            break;
        case BRT_GREATER:
            depthFunc = GL_GREATER;
            break;
        case BRT_LESS_OR_EQUAL:
            depthFunc = GL_LEQUAL;
            break;
        case BRT_GREATER_OR_EQUAL:
            depthFunc = GL_GEQUAL;
            break;
        case BRT_EQUAL:
            depthFunc = GL_EQUAL;
            break;
        case BRT_NOT_EQUAL:
            depthFunc = GL_NOTEQUAL;
            break;
        case BRT_NEVER:
            depthFunc = GL_NEVER;
            break;
        case BRT_ALWAYS:
            depthFunc = GL_ALWAYS;
            break;
        default:
            depthFunc = GL_LESS;
        }
        glDepthFunc(depthFunc);
    }
    GL_CHECK_ERROR();
}

// take a pixelmap and palette and convert 8 bit to 32 bit just in time
static void update_paletted_texture(br_pixelmap *src, const br_uint_32 *palette) {
    uint32_t* buffer = BrScratchAllocate(sizeof(uint32_t) * src->width * src->height);
    if (BREND_FN(Pixelmap, ExpandToRGBA8888)(src->pixels, src->width, src->height, src->row_bytes, BR_PMT_INDEX_8, buffer, palette) != BRE_OK) {
        BrScratchFree(buffer);
        return;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, src->width, src->height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    BrScratchFree(buffer);
    GL_CHECK_ERROR();
}

void StoredGLApplyProperties(HVIDEO hVideo, state_stack* state, uint32_t states, shader_data_model* model, GLuint tex_default) {
    br_boolean blending_on;
    br_buffer_stored* colour_map;
    br_boolean filter_linear;
    br_boolean palette_dirty;
    const br_uint_32* palette_entries;

    /* Only use the states we want (if valid). */
    states = state->valid & states;

    BREND_FN(State, FillModel)(state, states, model);
    BREND_FN(State, FillModelTexture)(state, states, model, &colour_map, &filter_linear, &palette_dirty, &palette_entries);

    if (states & MASK_STATE_CULL) {
        /*
         * Apply culling states. These are a bit confusing:
         * BRT_ONE_SIDED - Simple, cull back faces. From BRT_ONE_SIDED.
         *
         * BRT_TWO_SIDED - This means the face is two-sided, not to cull
         *                 both sides. From BR_MATF_TWO_SIDED. In the .3ds file
         *                 format, the "two sided" flag means the material is
         *                 visible from the back, or "not culled". fmt/load3ds.c
         *                 sets BR_MATF_TWO_SIDED if this is set, so assume this is
         *                 the correct behaviour.
         *
         * BRT_NONE      - Confusing, this is set if the material has
         *                 BR_MATF_ALWAYS_VISIBLE, but is overridden if
         *                 BR_MATF_TWO_SIDED is set. Assume it means the same
         *                 as BR_MATF_TWO_SIDED.
         */
        switch (state->cull.type) {
        case BRT_ONE_SIDED:
        default: /* Default BRender policy, so default. */
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;

        case BRT_TWO_SIDED:
        case BRT_NONE:
            glDisable(GL_CULL_FACE);
            break;
        }
    }

    if (states & MASK_STATE_SURFACE) {
        glActiveTexture(GL_TEXTURE0);
    }

    if (states & MASK_STATE_PRIMITIVE) {

        if (state->prim.flags & PRIMF_COLOUR_WRITE)
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        else
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        if (colour_map) {

            glBindTexture(GL_TEXTURE_2D, BufferStoredGLGetTexture(colour_map));

            // has the 8 bit color source or palette changed?
            if (palette_dirty) {
                update_paletted_texture(colour_map->source, palette_entries);
                colour_map->paletted_source_dirty = BR_FALSE;
                colour_map->palette_revision = colour_map->palette_pointer->revision;
            }

        } else {

            glBindTexture(GL_TEXTURE_2D, tex_default);
        }

        GLenum minFilter, magFilter;
        GLfloat maxAnisotropy;
        extern int gAnisotropy_level;
        if (gAnisotropy_level > 0 && GLAD_GL_EXT_texture_filter_anisotropic) {
            maxAnisotropy = (GLfloat)gAnisotropy_level;
            if (maxAnisotropy > hVideo->maxAnisotropy)
                maxAnisotropy = hVideo->maxAnisotropy;
        } else {
            maxAnisotropy = 1.0f;
        }
        if (filter_linear && state->prim.mip_filter == BRT_LINEAR) {
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
            magFilter = GL_LINEAR;
        } else if (filter_linear && state->prim.mip_filter == BRT_NONE) {
            minFilter = GL_LINEAR;
            magFilter = GL_LINEAR;
        } else if (!filter_linear && state->prim.mip_filter == BRT_LINEAR) {
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
            magFilter = GL_NEAREST;
        } else if (!filter_linear && state->prim.mip_filter == BRT_NONE) {
            minFilter = GL_NEAREST;
            magFilter = GL_NEAREST;
        } else {
            assert(0);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magFilter);

        if (GLAD_GL_EXT_texture_filter_anisotropic)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

        blending_on = (state->prim.flags & PRIMF_BLEND) || (colour_map != NULL && colour_map->blended);
        if (blending_on) {
            glEnable(GL_BLEND);
            apply_blend_mode(state);
        } else {
            glDisable(GL_BLEND);
        }
    }
    apply_depth_properties(state, states);
    GL_CHECK_ERROR();
}
