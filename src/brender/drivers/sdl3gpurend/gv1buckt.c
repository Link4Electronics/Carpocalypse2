/*
 * Geometry format for rendering the order table buckets spat out by a prior
 * hidden surface render. This mirrors glrend/gv1buckt.c; the only difference
 * is that replayed groups go through StoredSDL3GPURENDRenderGroup().
 *
 * With the current core the deferred (order-table) path is disabled, so the
 * bucket tables are empty by the time this runs. The object still has to
 * exist: BrZbsSceneRender() fails with "Renderer does not support buckets"
 * when no BRT_GEOMETRY_V1_BUCKETS format was created by the facility.
 */
#include "drv.h"
#include "brassert.h"

static const struct br_geometry_v1_buckets_dispatch geometryV1BucketsDispatch;

#define F(f) offsetof(struct br_geometry_v1_buckets, f)

static struct br_tv_template_entry geometryV1BucketsTemplateEntries[] = {
    {BRT(IDENTIFIER_CSTR),     F(identifier),        BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER},
    {BRT(RENDERER_FACILITY_O), F(renderer_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER},
    {BRT(FACILITY_O),          F(renderer_facility), BRTV_QUERY,            BRTV_CONV_POINTER},
};
#undef F

br_geometry_v1_buckets* GeometryV1BucketsSDL3GPURENDAllocate(br_renderer_facility* type, const char* id)
{
    br_geometry_v1_buckets* self;

    self                    = BrResAllocate(type, sizeof(*self), BR_MEMORY_OBJECT);
    self->dispatch          = &geometryV1BucketsDispatch;
    self->identifier        = id;
    self->device            = type->device;
    self->renderer_facility = type;

    ObjectContainerAddFront(type, (br_object*)self);
    return self;
}

static void BR_CMETHOD_DECL(br_geometry_v1_buckets_sdl3gpurend, free)(br_object* _self)
{
    br_geometry_v1_buckets* self = (br_geometry_v1_buckets*)_self;

    ObjectContainerRemove(self->renderer_facility, (br_object*)self);

    BrResFreeNoCallback(self);
}

static const char* BR_CMETHOD(br_geometry_v1_buckets_sdl3gpurend, identifier)(br_object* self)
{
    return ((br_geometry_v1_buckets*)self)->identifier;
}

static br_token BR_CMETHOD_DECL(br_geometry_v1_buckets_sdl3gpurend, type)(br_object* self)
{
    return BRT_GEOMETRY_V1_BUCKETS;
}

static br_boolean BR_CMETHOD_DECL(br_geometry_v1_buckets_sdl3gpurend, isType)(br_object* self, br_token t)
{
    (void)self;
    return (t == BRT_GEOMETRY_V1_BUCKETS) || (t == BRT_GEOMETRY) || (t == BRT_OBJECT);
}

static br_device* BR_CMETHOD(br_geometry_v1_buckets_sdl3gpurend, device)(br_object* self)
{
    return ((br_geometry_v1_buckets*)self)->device;
}

static br_size_t BR_CMETHOD_DECL(br_geometry_v1_buckets_sdl3gpurend, space)(br_object* self)
{
    (void)self;
    return sizeof(br_geometry_v1_buckets);
}

static struct br_tv_template* BR_CMETHOD_DECL(br_geometry_v1_buckets_sdl3gpurend, templateQuery)(br_object* _self)
{
    br_geometry_v1_buckets* self = (br_geometry_v1_buckets*)_self;

    if (self->device->templates.geometryV1BucketsTemplate == NULL) {
        self->device->templates.geometryV1BucketsTemplate = BrTVTemplateAllocate(
            self->device, (br_tv_template_entry*)geometryV1BucketsTemplateEntries, BR_ASIZE(geometryV1BucketsTemplateEntries));
    }

    return self->device->templates.geometryV1BucketsTemplate;
}

br_error BR_CMETHOD_DECL(br_geometry_v1_buckets_sdl3gpurend, render)(br_geometry_v1_buckets* self, br_renderer* renderer,
                                                                     br_primitive** buckets, br_int_32 nbuckets)
{
    br_primitive* p;
    br_renderer_state_stored* stored;

    (void)self;

    if (nbuckets <= 0)
        return BRE_OK;

    /*
     * Render bucket table from last to first
     */
    for (buckets += nbuckets - 1; nbuckets--; buckets--) {
        for (p = *buckets; p; p = p->next) {
            stored = (br_renderer_state_stored*)p->stored;
            ASSERT(stored == NULL || ObjectIsType((br_object*)stored, BRT_RENDERER_STATE_STORED));

            if (p->type == BRT_GEOMETRY_STORED) {
                br_geometry_stored* geom = (br_geometry_stored*)p->v[0];
                state_stack* state = (state_stack*)p->v[1];
                sdl3_groupinfo* groupinfo = (sdl3_groupinfo*)p->v[2];

                ASSERT(ObjectIsType((br_object*)geom, BRT_GEOMETRY_STORED));

                *renderer->state.current = *state;
                BrPoolBlockFree(renderer->state_pool, p->v[1]);

                StoredSDL3GPURENDRenderGroup(geom, renderer, groupinfo);
            } else {
                /* We can't handle other things yet. */
                continue;
            }
        }
    }

    return BRE_OK;
}

static const struct br_geometry_v1_buckets_dispatch geometryV1BucketsDispatch = {
    ._reserved0 = NULL,
    ._reserved1 = NULL,
    ._reserved2 = NULL,
    ._reserved3 = NULL,
    ._free       = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, free),
    ._identifier = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, identifier),
    ._type       = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, type),
    ._isType     = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, isType),
    ._device     = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, device),
    ._space      = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, space),

    ._templateQuery = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, templateQuery),
    ._query         = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer   = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany     = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll      = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize  = BR_CMETHOD_REF(br_object, queryAllSize),

    ._render         = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, render),
    ._renderOnScreen = BR_CMETHOD_REF(br_geometry_v1_buckets_sdl3gpurend, render),
};
