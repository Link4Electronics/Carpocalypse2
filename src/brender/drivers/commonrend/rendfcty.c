/*
 * Renderer type methods
 */
#include "drv.h"
#include "commonrend.h"
#include <stddef.h>

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_facility_dispatch rendererFacilityDispatch;

/*
 * Renderer Type info. template
 */
#define F(f) offsetof(struct br_renderer_facility, f)

static struct br_tv_template_entry rendererFacilityTemplateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, 0 },
    { BRT(RENDERER_MAX_I32), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, 1 },
};

#undef F

br_renderer_facility* BREND_FN(RendererFacility, Init)(br_device* device) {
    br_renderer_facility* self;

    self = BrResAllocate(device, sizeof(*self), BR_MEMORY_OBJECT);
    self->dispatch = &rendererFacilityDispatch;
#if defined(BREND_DRIVER_GL)
    self->identifier = "OpenGL Renderer facility";
#else
    self->identifier = "SDL3 GPU Renderer facility";
#endif
    self->device = device;
    self->object_list = BrObjectListAllocate(self);

    /* Create geometry objects */
    if (GeometryPrimitivesNullAllocate(self, "Primitives") == NULL || GeometryLightingNullAllocate(self, "Lighting") == NULL || BREND_FN(GeometryV1Model, Allocate)(self, "V1Model") == NULL) {
#if defined(BREND_DRIVER_GL)
        BR_ERROR("GLREND: Error creating geometry objects.");
#else
        BR_ERROR("SDL3GPU: Error creating geometry objects.");
#endif
        BrResFree(self);
        return NULL;
    }

    ObjectContainerAddFront(device, (br_object*)self);
    return (br_renderer_facility*)self;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), free)(br_object* _self) {
    br_renderer_facility* self = (br_renderer_facility*)_self;

    /*
     * Detach renderer from device
     */
    ObjectContainerRemove(self->device, (br_object*)self);

    /*
     * Remove attached objects
     */
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), identifier)(br_object* self) {
    return ((br_renderer_facility*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), type)(br_object* self) {
    (void)self;
    return BRT_RENDERER_FACILITY;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), isType)(br_object* self, br_token t) {
    (void)self;
    return (t == BRT_RENDERER_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static struct br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), device)(br_object* self) {
    return ((br_renderer_facility*)self)->device;
}

static br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), space)(br_object* self) {
    (void)self;
    return sizeof(br_renderer_facility);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), templateQuery)(br_object* _self) {
    br_renderer_facility* self = (br_renderer_facility*)_self;

    if (self->device->templates.rendererFacilityTemplate == NULL) {
        self->device->templates.rendererFacilityTemplate = BrTVTemplateAllocate(
            self->device, (br_tv_template_entry*)rendererFacilityTemplateEntries, BR_ASIZE(rendererFacilityTemplateEntries));
    }

    return self->device->templates.rendererFacilityTemplate;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), validDestination)(br_renderer_facility* self, br_boolean* bp,
    br_object* h) {
    (void)self;
    (void)bp;
    (void)h;
    return BRE_OK;
}

/*
 * Structure used to unpack the rendererNew arguments
 */
struct newRendererTokens {
    br_object* dest;
};

#define F(f) offsetof(struct newRendererTokens, f)

static struct br_tv_template_entry rendererNewTemplateEntries[] = {
    { BRT(DESTINATION_O), F(dest), BRTV_SET, BRTV_CONV_COPY },
    { BRT(OUTPUT_FACILITY_O), F(dest), BRTV_SET, BRTV_CONV_COPY },
};

#undef F

/*
 * Create a new renderer
 */
static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), rendererNew)(br_renderer_facility* self,
    struct br_renderer** prenderer, br_token_value* tv) {
    struct newRendererTokens rt = { NULL };
    br_int_32 count;
    br_renderer* renderer;

    /*
     * Process any options
     */
    if (self->device->templates.rendererNewTemplate == NULL) {
        self->device->templates.rendererNewTemplate = BrTVTemplateAllocate(self->device, rendererNewTemplateEntries,
            BR_ASIZE(rendererNewTemplateEntries));
    }

    BrTokenValueSetMany(&rt, &count, NULL, tv, self->device->templates.rendererNewTemplate);

    /*
     * Check that the destination is valid
     */
    if (rt.dest == NULL || ObjectDevice(rt.dest) != self->device || !ObjectIsType(rt.dest, BRT_DEVICE_PIXELMAP))
        return BRE_UNSUPPORTED;

    /*
     * Create Renderer
     */
    renderer = BREND_FN(Renderer, Allocate)(self->device, self, (br_device_pixelmap*)rt.dest);

    if (renderer == NULL)
        return BRE_FAIL;

    *prenderer = renderer;
    return BRE_OK;
}

static void* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer_facility_), listQuery)(br_object_container* self) {
    return ((br_renderer_facility*)self)->object_list;
}

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_facility_dispatch rendererFacilityDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._listQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), listQuery),
    ._tokensMatchBegin = BR_CMETHOD_REF(br_object_container, tokensMatchBegin),
    ._tokensMatch = BR_CMETHOD_REF(br_object_container, tokensMatch),
    ._tokensMatchEnd = BR_CMETHOD_REF(br_object_container, tokensMatchEnd),
    ._addFront = BR_CMETHOD_REF(br_object_container, addFront),
    ._removeFront = BR_CMETHOD_REF(br_object_container, removeFront),
    ._remove = BR_CMETHOD_REF(br_object_container, remove),
    ._find = BR_CMETHOD_REF(br_object_container, find),
    ._findMany = BR_CMETHOD_REF(br_object_container, findMany),
    ._count = BR_CMETHOD_REF(br_object_container, count),

    ._validDestination = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), validDestination),

    ._rendererNew = BREND_CMETHOD_REF(BREND_CLASS(br_renderer_facility_), rendererNew),
};
