#include "drv.h"
#include "commonrend.h"
#include <stddef.h>
#include <string.h>

#if defined(__vita__) && defined(BREND_DRIVER_GL)
void StateGLTouchLights(state_stack* state) {
    static uint64_t next_revision;
    state->light_revision = ++next_revision;
}
#endif

void BREND_FN(State, Init)(state_all* state, void* res) {
    state->res = res;
#if defined(__vita__) && defined(BREND_DRIVER_GL)
    state->default_.light_revision = 0;
    for (unsigned i = 0; i < MAX_STATE_STACK; ++i) state->stack[i].light_revision = 0;
#endif

    BREND_FN(State, InitMatrix)(state);
    BREND_FN(State, InitClip)(state);
    BREND_FN(State, InitCull)(state);
    BREND_FN(State, InitSurface)(state);
    BREND_FN(State, InitPrimitive)(state);
    BREND_FN(State, InitOutput)(state);
    BREND_FN(State, InitHidden)(state);
    BREND_FN(State, InitLight)(state);

    state->current = state->stack;
    BREND_FN(State, Copy)(state->current, &state->default_, ~0u);
}

void BREND_FN(State, Copy)(state_stack* dst, state_stack* src, uint32_t mask) {
    /* Restrict the copy mask to the valid parts. */
    mask &= src->valid;

    /* Merge the valid mask into the destination. */
    dst->valid |= mask;

    if (mask & MASK_STATE_MATRIX)
        dst->matrix = src->matrix;

    if (mask & MASK_STATE_CLIP) {
        for (int i = 0; i < MAX_STATE_CLIP_PLANES; ++i)
            dst->clip[i] = src->clip[i];
    }

    if (mask & MASK_STATE_CULL)
        dst->cull = src->cull;

    if (mask & MASK_STATE_SURFACE)
        dst->surface = src->surface;

    if (mask & MASK_STATE_PRIMITIVE)
        dst->prim = src->prim;

    if (mask & MASK_STATE_OUTPUT)
        dst->output = src->output;

    if (mask & MASK_STATE_LIGHT) {
        for (int i = 0; i < MAX_STATE_LIGHTS; ++i)
            dst->light[i] = src->light[i];
#if defined(__vita__) && defined(BREND_DRIVER_GL)
        dst->light_revision = src->light_revision;
#endif
    }
}

br_boolean BREND_FN(State, Push)(state_all* state, uint32_t mask) {
#if defined(__vita__) && defined(BREND_DRIVER_GL)
    if (state->top >= MAX_STATE_STACK - 1)
#else
    if (state->top >= MAX_STATE_STACK)
#endif
        return BR_FALSE;

    state_stack* old = state->current;
    ++state->top;
    ++state->current;

    // BREND_FN(State, Copy)(state->current, old, mask);
#if defined(__vita__) && defined(BREND_DRIVER_GL)
    /* The child still inherits EVERY state part, irrespective of push mask.
     * Reused stack slots already hold the same48 lights in the usual case.
     * Copy both other ranges unconditionally, and only elide lights when a
     * nonzero revision proves their complete contents match. */
    static unsigned pushes, light_copies;
    if (old->light_revision == 0 || state->current->light_revision != old->light_revision) {
        memcpy(state->current->light, old->light, sizeof(old->light));
        ++light_copies;
    }
    memcpy(state->current, old, offsetof(state_stack, light));
    memcpy((char*)state->current + offsetof(state_stack, hidden),
        (char*)old + offsetof(state_stack, hidden), sizeof(*old) - offsetof(state_stack, hidden));
    if (++pushes % 8192 == 0) {
        extern void C2VitaLog(const char*, ...);
        C2VitaLog("VITA STATE pushes=%u light_copies=%u light_bytes_avoided=%llu\n",
            pushes, light_copies, (unsigned long long)(pushes-light_copies)*sizeof(old->light));
    }
#else
    *state->current = *old;
#endif
    return BR_TRUE;
}

br_boolean BREND_FN(State, Pop)(state_all* state, uint32_t mask) {
    if (state->top <= 0)
        return BR_FALSE;

    state_stack* old = state->current;
    --state->top;
    --state->current;

    // BREND_FN(State, Copy)(&state->current, sp, mask);
    return BR_TRUE;
}

void BREND_FN(State, Default)(state_all* state, uint32_t mask) {
    BREND_FN(State, Copy)(state->current, &state->default_, mask);
}

#if defined(BREND_DRIVER_GL)
// d3drend:state.c:452 void TemplateActions(struct state_all *state, br_token part, br_int_32 index, br_uint_32 mask)
void BREND_FN(State, TemplateActions)(state_all* state, uint32_t mask) {
    // if(mask & TM_CLEAR_M2V_HINT)
    //     state->current.matrix.model_to_view_hint = BRT_NONE;
    //
    // if(mask & TM_CLEAR_V2S_HINT)
    //     state->current.matrix.view_to_screen_hint = BRT_NONE;
}
#endif

// d3drend:state.c:351 struct br_tv_template * FindStateTemplate(struct br_renderer *self, struct state_all **state,
// br_token part, br_int_32 index)
struct br_tv_template* BREND_FN(State, GetStateTemplate)(state_all* state, br_token part, br_int_32 index) {
    switch (part) {
        case BRT_MATRIX:
            return state->templates.matrix;

        case BRT_CLIP:
            if (index >= MAX_STATE_CLIP_PLANES)
                return NULL;

            return state->templates.clip[index];

        case BRT_CULL:
            return state->templates.cull;

        case BRT_SURFACE:
            return state->templates.surface;

        case BRT_PRIMITIVE:
            return state->templates.prim;

        case BRT_HIDDEN_SURFACE:
            return state->templates.hidden;
        case BRT_LIGHT:
            if (index >= MAX_STATE_LIGHTS)
                return NULL;

            return state->templates.light[index];

        case BRT_OUTPUT:
            return state->templates.output;

        case BRT_ENABLE:
        case BRT_BOUNDS:
        default:
            break;
    }

    return NULL;
}
