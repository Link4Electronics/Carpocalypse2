/*
 * Output type methods
 */
#include "drv.h"
#include "commonrend.h"

/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_output_facility_dispatch outputFacilityDispatch;

/*
 * Output Type info. template
 */
#define F(f) offsetof(br_output_facility, f)
#define A(a) ((br_uintptr_t)(a))

static struct br_tv_template_entry outputFacilityTemplateEntries[] = {
    { BRT(WIDTH_I32), A(-1), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(WIDTH_MIN_I32), A(1), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(HEIGHT_I32), A(-1), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(HEIGHT_MIN_I32), A(1), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(PIXEL_BITS_I32), A(-1), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(PIXEL_TYPE_U8), A(BR_PMT_MAX), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(INDEXED_B), A(0), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(RENDERER_FACILITY_O), F(renderer_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },

    /*
     * We don't use these, but we need BrDevBeginVar to accept them.
     * These are passed to pixelmapNew, which will handle them.
     */
    { BRT(MSAA_SAMPLES_I32), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
    { BRT(WINDOW_HANDLE_H), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
#if defined(BREND_DRIVER_GL)
    { BRT(OPENGL_CALLBACKS_P), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
    { BRT(OPENGL_VERTEX_SHADER_STR), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
    { BRT(OPENGL_FRAGMENT_SHADER_STR), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
#else
    { BRT(SDL3GPU_CALLBACKS_P), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
    { BRT(SDL3GPU_DEBUG_MODE), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
#endif
    { BRT_CLUT_MAX_I32, 0, 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, 0 },
};

#undef F
#undef A

br_output_facility* BREND_FN(OutputFacility, Init)(br_device* dev, br_renderer_facility* rendfcty) {
    br_output_facility* self;

    self = BrResAllocate(dev, sizeof(br_output_facility), BR_MEMORY_OBJECT);
#if defined(BREND_DRIVER_GL)
    self->identifier = "OpenGL";
#else
    self->identifier = "SDL3 GPU";
#endif
    self->dispatch = &outputFacilityDispatch;
    self->device = dev;
    self->object_list = BrObjectListAllocate(self);
    self->renderer_facility = rendfcty;

    ObjectContainerAddFront(dev, (br_object*)self);
    return self;
}

/*
 * Common object methods
 */
static void BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), free)(br_object* _self) {
    br_output_facility* self = (br_output_facility*)_self;

    ObjectContainerRemove(self->device, (br_object*)self);

    /*
     * Remove attached objects
     */
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), identifier)(br_object* self) {
    return ((br_output_facility*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), type)(br_object* self) {
    (void)self;
    return BRT_OUTPUT_FACILITY;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), isType)(br_object* self, br_token t) {
    (void)self;
    return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), device)(br_object* self) {
    return ((br_output_facility*)self)->device;
}

static br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), space)(br_object* self) {
    (void)self;
    return sizeof(br_output_facility);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), queryTemplate)(br_object* _self) {
    br_output_facility* self = (br_output_facility*)_self;

    if (self->device->templates.outputFacilityTemplate == NULL)
        self->device->templates.outputFacilityTemplate = BrTVTemplateAllocate(self->device, outputFacilityTemplateEntries,
            BR_ASIZE(outputFacilityTemplateEntries));

    return self->device->templates.outputFacilityTemplate;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), validSource)(br_output_facility* self, br_boolean* bp, br_object* h) {
    (void)self;
    (void)bp;
    (void)h;
    return BRE_OK;
}

/*
 * Instantiate an output pixelmap from the output type
 */
static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), pixelmapNew)(br_output_facility* self,
    br_device_pixelmap** ppmap, br_token_value* tv) {
    br_device_pixelmap* pm;

    /*
     * Create a device pixelmap structure representing display memory
     */
    pm = BREND_FN(DevicePixelmap, AllocateFront)(self->device, self, tv);

    if (pm == NULL)
        return BRE_FAIL;

    *ppmap = pm;

    return BRE_OK;
}

/*
 * Cannot create new CLUTs, stuck with the single hardware one
 */
static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), clutNew)(br_output_facility* self, br_device_clut** pclut,
    br_token_value* tv) {
    (void)self;
    (void)pclut;
    (void)tv;
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), queryCapability)(br_output_facility* self, br_token_value* buffer_in,
    br_token_value* buffer_out, br_size_t size_buffer_out) {
    (void)self;
    (void)buffer_in;
    (void)buffer_out;
    (void)size_buffer_out;
    return BRE_FAIL;
}

static void* BREND_CMETHOD_DECL(BREND_CLASS(br_output_facility_), listQuery)(struct br_object_container* self) {
    return ((br_output_facility*)self)->object_list;
}

/*
 * Output facility dispatch table
 */
static const struct br_output_facility_dispatch outputFacilityDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), queryTemplate),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._listQuery = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), listQuery),
    ._tokensMatchBegin = BR_CMETHOD_REF(br_object_container, tokensMatchBegin),
    ._tokensMatch = BR_CMETHOD_REF(br_object_container, tokensMatch),
    ._tokensMatchEnd = BR_CMETHOD_REF(br_object_container, tokensMatchEnd),
    ._addFront = BR_CMETHOD_REF(br_object_container, addFront),
    ._removeFront = BR_CMETHOD_REF(br_object_container, removeFront),
    ._remove = BR_CMETHOD_REF(br_object_container, remove),
    ._find = BR_CMETHOD_REF(br_object_container, find),
    ._findMany = BR_CMETHOD_REF(br_object_container, findMany),
    ._count = BR_CMETHOD_REF(br_object_container, count),

    ._validSource = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), validSource),
    ._pixelmapNew = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), pixelmapNew),
    ._clutNew = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), clutNew),
    ._queryCapability = BREND_CMETHOD_REF(BREND_CLASS(br_output_facility_), queryCapability),
};
