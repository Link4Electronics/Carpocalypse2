/*
 * Geometry format for version 1 models
 */
#include "brassert.h"
#include "drv.h"
#include "commonrend.h"
#if defined(__vita__) && defined(BREND_DRIVER_GL)
#include "vita_geometry_cache.h"
#endif
#if defined(__vita__) && defined(BREND_DRIVER_GL)
#include "../../../carma2/platform/sdl3/vita_perf.h"
#define VPB(x) do { if (C2VitaPerfDetail) C2VitaPerfBegin(x); } while (0)
#define VPE(x) do { if (C2VitaPerfDetail) C2VitaPerfEnd(x); } while (0)
#else
#define VPB(x) ((void)0)
#define VPE(x) ((void)0)
#endif

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_geometry_v1_model_dispatch geometryV1ModelDispatch;

/*
 * Geometry format info. template
 */
#define F(f) offsetof(struct br_geometry_v1_model, f)
#define A(a) ((br_uintptr_t)(a))

static struct br_tv_template_entry geometryV1ModelTemplateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER },
    { BRT(RENDERER_FACILITY_O), F(renderer_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER },
    { BRT(FACILITY_O), F(renderer_facility), BRTV_QUERY, BRTV_CONV_POINTER },
    { BRT(STORED_ONLY_B), A(BR_TRUE), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY },
};
#undef F
#undef A

/*
 * Allocate a geometry format
 */
br_geometry_v1_model* BREND_FN(GeometryV1Model, Allocate)(br_renderer_facility* type, const char* id) {
    br_geometry_v1_model* self;

    self = BrResAllocate(type, sizeof(*self), BR_MEMORY_OBJECT);
    if (self == NULL)
        return NULL;

    self->dispatch = &geometryV1ModelDispatch;
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    ObjectContainerAddFront(type, (br_object*)self);
    return self;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), free)(br_object* _self) {
    br_geometry_v1_model* self = (br_geometry_v1_model*)_self;

#if defined(__vita__) && defined(BREND_DRIVER_GL)
    GLVitaGeometryCacheClearOwner(self);
#endif
    ObjectContainerRemove(self->renderer_facility, (br_object*)self);

    BrResFreeNoCallback(self);
}

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), identifier)(br_object* self) {
    return ((br_geometry_v1_model*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), type)(br_object* self) {
    (void)self;
    return BRT_GEOMETRY_V1_MODEL;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), isType)(br_object* self, br_token t) {
    (void)self;
    return (t == BRT_GEOMETRY_V1_MODEL) || (t == BRT_GEOMETRY) || (t == BRT_OBJECT);
}

static br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), device)(br_object* self) {
    return ((br_geometry_v1_model*)self)->device;
}

static br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), space)(br_object* self) {
    (void)self;
    return sizeof(br_geometry_v1_model);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), templateQuery)(br_object* _self) {
    br_geometry_v1_model* self = (br_geometry_v1_model*)_self;

    if (self->device->templates.geometryV1ModelTemplate == NULL) {
        self->device->templates.geometryV1ModelTemplate = BrTVTemplateAllocate(
            self->device, (br_tv_template_entry*)geometryV1ModelTemplateEntries, BR_ASIZE(geometryV1ModelTemplateEntries));
    }

    return self->device->templates.geometryV1ModelTemplate;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), storedAvail)(struct br_geometry_v1_model* self,
    br_int_32* psize, br_token_value* tv) {
    (void)self;
    (void)psize;
    (void)tv;
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_),
    storedNew)(struct br_geometry_v1_model* self, struct br_renderer* renderer,
    struct br_geometry_stored** psg, struct v11model_f* model, br_token type,
    br_token_value* tv) {
    (void)tv;
    ASSERT(type == BRT_TRIANGLE);

    if ((*psg = BREND_FN(GeometryStored, Allocate)(self, "MODEL", renderer, (struct v11model*)model)) == NULL)
        return BRE_FAIL;

    return BRE_OK;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_v1_model_), render)(struct br_geometry_v1_model* self,
    struct br_renderer* renderer, struct v11model_f* model,
    struct br_renderer_state_stored* default_state, br_token type) {
    /*
     * Immediate mode (our 1.2-era core renders updateable models, e.g. crushed cars and
     * pedestrians, straight from the prepared v11 data): upload into a temporary stored
     * geometry, draw it and free it. GL lines use the prepared edge IDs.
     */
    typedef br_error BR_METHOD stored_render_fn(struct br_geometry_stored*, struct br_renderer*, struct br_renderer_state_stored*);
    struct br_geometry_stored* sg;
    br_error r;
#if defined(__vita__) && defined(BREND_DRIVER_GL)
    int cached;
#endif

    if (model == NULL) return BRE_OK;
#if defined(BREND_DRIVER_GL)
    if (type != BRT_TRIANGLE && type != BRT_LINE) return BRE_OK;
#else
    if (type != BRT_TRIANGLE) return BRE_OK;
#endif

#if defined(__vita__) && defined(BREND_DRIVER_GL)
    C2VitaPerfCount(C2VPC_MODELS);
    VPB(C2VP_G_LOOKUP);
    sg=GLVitaGeometryCacheGet(self,renderer,(struct v11model*)model,type);
    VPE(C2VP_G_LOOKUP);
    cached=sg!=NULL;
    if (!cached) C2VitaPerfCount(C2VPC_CACHE_MISSES);
    if (!cached)
#endif
    if ((sg = BREND_FN(GeometryStored, Allocate)(self, "IMMEDIATE", renderer, (struct v11model*)model)) == NULL)
        return BRE_FAIL;

#if defined(BREND_DRIVER_GL)
#ifdef __vita__
    if (!cached && type == BRT_LINE) StoredGLUseEdges(sg);
#else
    if (type == BRT_LINE) StoredGLUseEdges(sg);
#endif
#endif
    VPB(C2VP_G_RENDER);
    r = ((stored_render_fn*)sg->dispatch->_render)(sg, renderer, default_state);
    VPE(C2VP_G_RENDER);
#if defined(__vita__) && defined(BREND_DRIVER_GL)
    if (!cached)
#endif
    sg->dispatch->_free((br_object*)sg);
    return r;
}

/*
 * Default dispatch table for renderer type
 */
static const struct br_geometry_v1_model_dispatch geometryV1ModelDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._renderF = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), render),
    ._renderOnScreenF = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), render),
    ._storedNewF = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), storedNew),

    ._storedAvail = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_v1_model_), storedAvail),
};
