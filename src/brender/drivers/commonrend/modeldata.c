#include <stdlib.h>
#include "drv.h"
#include "commonrend.h"

/*
 * Fill the backend-agnostic shader_data_model material fields from the
 * current (or stored) renderer state. Backend texture binding and the
 * disable_texture/disable_colour_key flags (which depend on the backend's
 * texture handles) stay in each driver.
 */
void BREND_FN(State, FillModel)(state_stack* state, uint32_t states, shader_data_model* model) {
    /* Only use the states we want (if valid). */
    states = state->valid & states;

    if (states & MASK_STATE_SURFACE) {
        if (state->surface.colour_source == BRT_SURFACE) {
            br_uint_32 colour = state->surface.colour;
            float r = BR_RED(colour) / 255.0f;
            float g = BR_GRN(colour) / 255.0f;
            float b = BR_BLU(colour) / 255.0f;
            BrVector4Set(&model->surface_colour, r, g, b, state->surface.opacity);
        } else {
            /* Geometry already supplies RGB; use a neutral modulation colour. */
            BrVector4Set(&model->surface_colour, 1.0f, 1.0f, 1.0f, state->surface.opacity);
        }

        model->ka = state->surface.ka;
        model->ks = state->surface.ks;
        model->kd = state->surface.kd;
        model->power = state->surface.power;

        switch (state->surface.mapping_source) {
        case BRT_GEOMETRY_MAP:
        default:
            model->uv_source = 0;
            break;

        case BRT_ENVIRONMENT_LOCAL:
            model->uv_source = 1;
            break;

        case BRT_ENVIRONMENT_INFINITE:
            model->uv_source = 2;
            break;
        }

        BrMatrix4Copy23(&model->map_transform, &state->surface.map_transform);

        /* BR_MATF_PRELIT selects geometry colour and disables dynamic lighting. */
        model->prelit = state->surface.prelighting || state->surface.colour_source == BRT_GEOMETRY;
        model->lighting = state->surface.lighting;
        { static int nolight = -1; /* port debug: C2_GL_NOLIGHT=1 */
            if (nolight < 0) { const char* e = getenv("C2_GL_NOLIGHT"); nolight = e != NULL && e[0] == '1'; }
            if (nolight) model->lighting = 0;
        }
    }

    if (states & MASK_STATE_PRIMITIVE) {
        /* Materials only ever send BRT_FOG_T (prepmatl.c): BRT_LINEAR when the
         * material has BR_MATF_FOG_LOCAL, BRT_NONE otherwise. BRT_FOG_B is kept
         * for completeness but no material sets it, so keying fog_enabled off
         * the boolean alone left distance fog disabled for everything. */
        model->fog_enabled = state->prim.fog_type == BRT_LINEAR || state->prim.fog_enabled;
        BrVector4Set(&model->fog_colour, BR_RED(state->prim.fog_colour) / 255.0f,
            BR_GRN(state->prim.fog_colour) / 255.0f, BR_BLU(state->prim.fog_colour) / 255.0f, 0.0f);
        model->fog_min = state->prim.fog_min;
        model->fog_max = state->prim.fog_max;
        { static int nofog = -1; /* port debug switch: C2_NO_FOG=1 turns fog back off */
            if (nofog < 0) { const char* e = getenv("C2_NO_FOG"); nofog = e != NULL && e[0] == '1'; }
            if (nofog) model->fog_enabled = 0;
        }
    }
}

/*
 * Shared material-texture decision used by StoredGLApplyProperties /
 * StoredSDL3GPURENDApplyProperties. Computes the texture-related model fields
 * (alpha, disable_texture/disable_colour_key), the effective colour map, the
 * filter mode, and the paletted-texture dirty/revision state. Texture binding,
 * palette upload and sampler choice stay in each driver.
 */
void BREND_FN(State, FillModelTexture)(state_stack* state, uint32_t states, shader_data_model* model,
    struct br_buffer_stored** colour_map, br_boolean* filter_linear, br_boolean* palette_dirty,
    const br_uint_32** palette_entries) {
    /* Only use the states we want (if valid). */
    states = state->valid & states;

    /* port: alpha_val is a float (default 1); the game also sets it on a 0..255 scale
     * (headup panels); material opacity comes with the surface part */
    model->alpha = (state->prim.alpha_val > 1.0f ? state->prim.alpha_val / 255.0f : state->prim.alpha_val) * state->surface.opacity;

    *colour_map = (states & MASK_STATE_PRIMITIVE) ? state->prim.colour_map : NULL;

    *filter_linear = (state->prim.filter == BRT_LINEAR);

    *palette_dirty = BR_FALSE;
    *palette_entries = NULL;
    if (*colour_map != NULL && (*colour_map)->paletted_source_dirty == BR_TRUE)
        *palette_dirty = BR_TRUE;
    if (*colour_map != NULL && (*colour_map)->palette_pointer != NULL &&
        (*colour_map)->palette_revision != (*colour_map)->palette_pointer->revision)
        *palette_dirty = BR_TRUE;
    if (*palette_dirty && *colour_map != NULL && (*colour_map)->palette_pointer != NULL)
        *palette_entries = (*colour_map)->palette_pointer->entries;

    if (*colour_map != NULL) {
        model->disable_texture = 0;
        model->disable_colour_key = !(state->prim.flags & PRIMF_COLOUR_KEY);
    } else {
        model->disable_texture = 1;
        model->disable_colour_key = 1;
    }
}
