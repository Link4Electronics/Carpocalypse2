/*
 * Stored renderer state
 */
#include "drv.h"
#include "commonrend.h"

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_state_stored_dispatch rendererStateStoredDispatch;

/*
 * Geometry format info. template
 */
#define F(f) offsetof(struct br_renderer_state_stored, f)

static struct br_tv_template_entry rendererStateStoredTemplateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(RENDERER_O), F(renderer), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(PARTS_U32), F(state.valid), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
};
#undef F

/*
 * Allocate a stored state
 */
#include <string.h>
br_renderer_state_stored* BREND_FN(RendererStateStored, Allocate)(br_renderer* renderer, state_stack* base_state,
    br_uint_32 m, br_token_value* tv) {
    br_renderer_state_stored* self;

    self = BrResAllocate(renderer, sizeof(*self), BR_MEMORY_OBJECT);
    self->dispatch = &rendererStateStoredDispatch;
    self->identifier = "Renderer-State-Stored";
    self->device = ObjectDevice(renderer);
    self->renderer = renderer;

    BREND_FN(State, Copy)(&self->state, base_state, m);
    ObjectContainerAddFront(renderer, (br_object*)self);
    return self;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), free)(br_object* _self) {
    br_renderer_state_stored* self = (br_renderer_state_stored*)_self;

    ObjectContainerRemove(self->renderer, (br_object*)self);

    /*
     * Any associated primitive state will have been attached as a resource
     */
    BrResFreeNoCallback(self);
}

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), identifier)(br_object* self) {
    return ((br_renderer_state_stored*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), type)(br_object* self) {
    return BRT_RENDERER_STATE_STORED;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), isType)(br_object* self, br_token t) {
    return (t == BRT_RENDERER_STATE_STORED) || (t == BRT_OBJECT);
}

static br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), device)(br_object* self) {
    return ((br_renderer_state_stored*)self)->device;
}

static br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), space)(br_object* self) {
    return sizeof(br_renderer_state_stored);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_state_stored_), templateQuery)(br_object* _self) {
    br_renderer_state_stored* self = (br_renderer_state_stored*)_self;

    if (self->device->templates.rendererStateStoredTemplate == NULL) {
        self->device->templates.rendererStateStoredTemplate = BrTVTemplateAllocate(
            self->device, (br_tv_template_entry*)rendererStateStoredTemplateEntries,
            BR_ASIZE(rendererStateStoredTemplateEntries));
    }

    return self->device->templates.rendererStateStoredTemplate;
}

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_state_stored_dispatch rendererStateStoredDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_state_stored_), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),
};
