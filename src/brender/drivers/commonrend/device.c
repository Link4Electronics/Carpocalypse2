/*
 * Device methods
 */
#include "drv.h"
#include "brassert.h"
#include "commonrend.h"

#if defined(BREND_DRIVER_GL)
#define DEVICE_TITLE "OpenGL v3.2"
#define DEVICE_CREATOR "Zane van Iperen"
#define DEVICE_PRODUCT "OpenGL"
#else
#define DEVICE_TITLE "SDL3 GPU"
#define DEVICE_CREATOR "dethrace"
#define DEVICE_PRODUCT "SDL3 GPU"
#endif
#define DEVICE_VERSION BR_VERSION(1, 0, 0)

/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_device_dispatch deviceDispatch;

/*
 * Device info. template
 */
static const char deviceTitle[] = DEVICE_TITLE;

static const char deviceCreator[] = DEVICE_CREATOR;

static const char deviceProduct[] = DEVICE_PRODUCT;

#define F(f) offsetof(br_device, f)
#define A(a) ((br_uintptr_t)(a))

static struct br_tv_template_entry deviceTemplateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, 0 },
    {
        BRT(CLUT_O),
        0,
        F(clut),
        BRTV_QUERY | BRTV_ALL,
        BRTV_CONV_COPY,
    },
    { BRT(VERSION_U32), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, DEVICE_VERSION },
    { BRT(BRENDER_VERSION_U32), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, __BRENDER__ },
    { BRT(DDI_VERSION_U32), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, __BRENDER_DDI__ },
    { BRT(CREATOR_CSTR), A(deviceCreator), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY, 0 },
    { BRT(TITLE_CSTR), A(deviceTitle), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY, 0 },
    { BRT(PRODUCT_CSTR), A(deviceProduct), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY, 0 },

#if defined(BREND_DRIVER_GL)
    /*
     * Minimum version of OpenGL supported by this driver.
     * Other devices (e.g. SDL) may query these to create appropriate windows.
     */
    { BRT(OPENGL_VERSION_MAJOR_U8), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, 3 },
    { BRT(OPENGL_VERSION_MINOR_U8), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, 2 },
    { BRT(OPENGL_PROFILE_T), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, BRT_OPENGL_PROFILE_CORE },
#endif
};
#undef F
#undef A

/*
 * List of tokens which are not significant in matching (for output facilities)
 */
// clang-format off
static const br_token insignificantMatchTokens[] = {
    BRT_WIDTH_I32,
    BRT_HEIGHT_I32,
    BRT_PIXEL_BITS_I32,
    BRT_PIXEL_TYPE_U8,
    BRT_WINDOW_MONITOR_I32,
    BRT_MSAA_SAMPLES_I32,
    BRT_WINDOW_HANDLE_H,
#if defined(BREND_DRIVER_GL)
    BRT_OPENGL_CALLBACKS_P,
    BRT_OPENGL_VERTEX_SHADER_STR,
    BRT_OPENGL_FRAGMENT_SHADER_STR,
#else
    BRT_SDL3GPU_CALLBACKS_P,
    BRT_SDL3GPU_DEBUG_MODE,
#endif
    BR_NULL_TOKEN,
};
// clang-format on

/*
 * Default token matching does nothing other than make all tokens match
 *
 * makes a copy of token/value list
 */
struct token_match {
    br_token_value* original;
    br_token_value* query;
    br_int_32 n;
    void* extra;
    br_size_t extra_size;
};

br_device* BREND_FN(Device, Allocate)(const char* identifier, const char* arguments) {
    br_device* self;

    /*
     * Set up device block and resource anchor
     */
    self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT);
    self->res = BrResAllocate(self, 0, BR_MEMORY_DRIVER);
    self->identifier = identifier;
    self->dispatch = &deviceDispatch;
    self->device = self;
    self->object_list = BrObjectListAllocate(self);

    if ((self->renderer_facility = BREND_FN(RendererFacility, Init)(self)) == NULL) {
        BrResFreeNoCallback(self);
        return NULL;
    }

    if ((self->output_facility = BREND_FN(OutputFacility, Init)(self, self->renderer_facility)) == NULL) {
        BrResFreeNoCallback(self);
        return NULL;
    }

    /*
     * Build CLUT object
     */
    self->clut = BREND_FN(DeviceClut, Allocate)(self, "Pseudo-CLUT");

    return self;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_device_), free)(struct br_object* _self) {
    br_device* self = (br_device*)_self;

    /*
     * Remove attached objects
     */
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);

    /*
     * Remove resources
     */
    BrResFreeNoCallback(self);
}

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_device_), identifier)(struct br_object* self) {
    return ((br_device*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_device_), type)(struct br_object* self) {
    (void)self;
    return BRT_DEVICE;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_device_), isType)(struct br_object* self, br_token t) {
    (void)self;
    return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_device_), device)(struct br_object* self) {
    return ((br_device*)self)->device;
}

static br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_device_), space)(struct br_object* self) {
    (void)self;
    return sizeof(br_device);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_device_), templateQuery)(struct br_object* _self) {
    br_device* self = (br_device*)_self;

    if (self->templates.deviceTemplate == NULL) {
        self->templates.deviceTemplate = BrTVTemplateAllocate(self, deviceTemplateEntries, BR_ASIZE(deviceTemplateEntries));
    }

    return self->templates.deviceTemplate;
}

static void* BREND_CMETHOD_DECL(BREND_CLASS(br_device_), listQuery)(struct br_object_container* self) {
    return ((br_device*)self)->object_list;
}

void* BREND_CMETHOD_DECL(BREND_CLASS(br_device_), tokensMatchBegin)(struct br_device* self, br_token t, br_token_value* tv) {
    struct token_match* tm;
    br_int_32 i;

    if (tv == NULL)
        return NULL;

    tm = BrResAllocate(self->res, sizeof(*tm), BR_MEMORY_APPLICATION);
    tm->original = tv;

    for (i = 0; tv[i].t != BR_NULL_TOKEN; i++)
        ;

    tm->n = i + 1;
    tm->query = BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
    BrMemCpy(tm->query, tv, i * sizeof(br_token_value));
    return (void*)tm;
}

br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_device_), tokensMatch)(struct br_object_container* self, br_object* h, void* arg) {
    struct token_match* tm = arg;
    br_size_t s;
    br_int_32 n;

    if (arg == NULL)
        return BR_TRUE;

    /*
     * Make a query on the object and then compare with the original tokens
     */
    ObjectQueryManySize(h, &s, tm->query);

    if (s > tm->extra_size) {
        if (tm->extra)
            BrResFree(tm->extra);
        tm->extra = BrResAllocate(tm, s, BR_MEMORY_APPLICATION);
        tm->extra_size = s;
    }

    ObjectQueryMany(h, tm->query, tm->extra, tm->extra_size, &n);

    /*
     * Ensure that all tokens were found
     */
    if (tm->query[n].t != BR_NULL_TOKEN)
        return BR_FALSE;

    /*
     * Compare the two token lists
     */
    /* BrTokenValueComparePartial reads (never writes) the insignificant list,
     * so the const cast is safe. */
    return BrTokenValueComparePartial(tm->original, tm->query, (br_token *)insignificantMatchTokens);
}

void BREND_CMETHOD_DECL(BREND_CLASS(br_device_), tokensMatchEnd)(struct br_object_container* self, void* arg) {
    if (arg)
        BrResFree(arg);
}

/*
 * Default dispatch table for device
 */
static const struct br_device_dispatch deviceDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_device_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_device_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_device_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_device_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_device_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_device_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_device_), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._listQuery = BREND_CMETHOD_REF(BREND_CLASS(br_device_), listQuery),
    ._tokensMatchBegin = BREND_CMETHOD_REF(BREND_CLASS(br_device_), tokensMatchBegin),
    ._tokensMatch = BREND_CMETHOD_REF(BREND_CLASS(br_device_), tokensMatch),
    ._tokensMatchEnd = BREND_CMETHOD_REF(BREND_CLASS(br_device_), tokensMatchEnd),
    ._addFront = BR_CMETHOD_REF(br_object_container, addFront),
    ._removeFront = BR_CMETHOD_REF(br_object_container, removeFront),
    ._remove = BR_CMETHOD_REF(br_object_container, remove),
    ._find = BR_CMETHOD_REF(br_object_container, find),
    ._findMany = BR_CMETHOD_REF(br_object_container, findMany),
    ._count = BR_CMETHOD_REF(br_object_container, count),
};
