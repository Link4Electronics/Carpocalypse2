/*
 * Shared stored-buffer (texture) support for the glrend/sdl3gpurend drivers:
 * template entries, identity methods, dispatch table, and the allocate
 * skeleton. The backend-specific update/free paths stay in each driver's
 * sbuffer.c (BufferStoredGL/SDL3GPUREND InitFields/update/free).
 */
#include <stddef.h>

#include "brassert.h"
#include "drv.h"

/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch;

/* Per-driver hooks, defined in each driver's sbuffer.c. */
extern void BREND_FN(BufferStored, InitFields)(struct br_buffer_stored* self);
extern void BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), free)(br_object* _self);
extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), update)(struct br_buffer_stored* self,
    struct br_device_pixelmap* pm, br_token_value* tv);

/*
 * Primitive state info. template
 */
#define F(f) offsetof(struct br_buffer_stored, f)

static struct br_tv_template_entry bufferStoredTemplateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
#if defined(BREND_DRIVER_GL)
    { DEV(OPENGL_TEXTURE_U32), F(gl_tex), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY }
#endif
};

#undef F

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), identifier)(br_object* self) {
    return ((br_buffer_stored*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), type)(br_object* self) {
    (void)self;
    return BRT_BUFFER_STORED;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), isType)(br_object* self, br_token t) {
    (void)self;
    return (t == BRT_BUFFER_STORED) || (t == BRT_OBJECT);
}

static br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), device)(br_object* self) {
    return ((br_buffer_stored*)self)->device;
}

static br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), space)(br_object* self) {
    return BrResSizeTotal(self);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), templateQuery)(br_object* _self) {
    return ((br_buffer_stored*)_self)->templates;
}

/*
 * Set up a static device object
 */
struct br_buffer_stored* BREND_FN(BufferStored, Allocate)(br_renderer* renderer, br_token use,
    struct br_device_pixelmap* pm, br_token_value* tv) {
    struct br_buffer_stored* self;
    char* ident;

    switch (use) {

    case BRT_TEXTURE_O:
    case BRT_COLOUR_MAP_O:
        ident = "Colour-Map";
        break;

        // case BRT_INDEX_SHADE_O:
        //     ident = "Shade-Table";
        //     break;

        // case BRT_INDEX_BLEND_O:
        //     ident = "Blend-Table";
        //     break;

        // case BRT_SCREEN_DOOR_O:
        //     ident = "Screendoor-Table";
        //     break;

        // case BRT_INDEX_LIGHT_O:
        //     ident = "Lighting-Table";
        //     break;

        // case BRT_BUMP_O:
        //     ident = "Bump-Map";
        //     break;

        // case BRT_UNKNOWN:
        //     ident = "Unknown";
        //     break;

    default:
        return NULL;
    }

    self = BrResAllocate(renderer, sizeof(*self), BR_MEMORY_OBJECT);
    if (self == NULL)
        return NULL;

    self->dispatch = &bufferStoredDispatch;
    self->identifier = ident;
    self->device = ObjectDevice(renderer);
    self->renderer = renderer;
    BREND_FN(BufferStored, InitFields)(self);
    self->templates = BrTVTemplateAllocate(self, (br_tv_template_entry*)bufferStoredTemplateEntries,
        BR_ASIZE(bufferStoredTemplateEntries));

    br_error r = self->dispatch->_update(self, pm, tv);
    (void)r;
#if defined(BREND_DRIVER_SDL3GPUREND)
    if (r != BRE_OK) {
        BrResFreeNoCallback(self);
        return NULL;
    }
#endif

    ObjectContainerAddFront(renderer, (br_object*)self);

    return self;
}

/*
 * Default dispatch table for device
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._update = BREND_CMETHOD_REF(BREND_CLASS(br_buffer_stored_), update),
};
