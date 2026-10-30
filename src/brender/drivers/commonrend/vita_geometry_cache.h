/* Optional exact-byte cache for Vita immediate geometry. Model data can be
 * edited without BrModelUpdate: compare the complete contiguous prepared block
 * on every draw, not a dirty flag or hash. Foreign/noncontiguous models bypass. */
#ifndef GL_VITA_GEOMETRY_CACHE_H
#define GL_VITA_GEOMETRY_CACHE_H
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#ifdef __ARM_NEON
#include <arm_neon.h>
#endif
#define VITA_GEO_SLOTS 512
#define VITA_GEO_BUDGET (2u*1024u*1024u)
#define VITA_GEO_MAX_MODEL (128u*1024u)
/* C2VitaStaticGeometry (set around RenderTrack) is kept for the logs; trust now uses the per-model
 * update stamp below for all geometry. */
int C2VitaStaticGeometry;
extern unsigned long C2VitaModelUpdateGeneration;
unsigned long long C2VitaGeometryStaticTrusted;
#define VITA_GEO_STATIC_NS 1000000000ull
extern unsigned long long C2VitaGLTicksNS(void);
typedef struct {
    br_geometry_v1_model* owner;
    br_renderer* renderer;
    const struct v11model* model;
    br_token type;
    size_t size;
    unsigned char* snapshot;
    br_geometry_stored* geometry;
    unsigned long long age;
    unsigned long verified_generation;
    unsigned long long verified_ns;
} vita_geo_entry;
static vita_geo_entry vita_geo_entries[VITA_GEO_SLOTS];
/* Direct-mapped hint: model pointer -> slot index of its last entry (validated on use). */
#define VITA_GEO_HINTS 1024
static unsigned short vita_geo_hint[VITA_GEO_HINTS];
static unsigned vita_geo_hash(const void* model) { return (unsigned)(((uintptr_t)model >> 4) ^ ((uintptr_t)model >> 14)) & (VITA_GEO_HINTS - 1); }
unsigned long long C2VitaGeometryBytesCompared;
/* Exact equality (same answer as memcmp(...)==0) without newlib's generic byte loop:
 * the common case is a full scan of equal data. Snapshots are malloc-aligned, prepared models
 * are at least word aligned; fall back to memcmp otherwise. */
static int vita_geo_equal(const void* a, const void* b, size_t n);
/* Per-model update stamp written by BrModelUpdate into the v11model padding after ngroups
 * (prepmesh.c C2VitaStampPrepared). 0 = never updated (pre-prepared models edited in place). */
static unsigned short vita_geo_stamp(const struct v11model* m) {
    unsigned short stamp;
    memcpy(&stamp, (const char*)m + offsetof(struct v11model, ngroups) + sizeof(m->ngroups), sizeof(stamp));
    return stamp;
}
/* An entry verified (exact compare or fresh snapshot) less than VITA_GEO_TRUST_NS ago, whose model
 * still carries the same non-zero stamp, is trusted without comparing. Safety net for a writer that
 * edits prepared data without BrModelUpdate: one in 64 trusted hits still compares and counts a
 * stamp miss (VITA DRAW stamp_misses), and nothing is trusted for longer than the window. */
#define VITA_GEO_TRUST_NS 1000000000ull
unsigned long long C2VitaGeometryStampMisses;
static unsigned vita_geo_sample;
static int vita_geo_current(vita_geo_entry* e, const struct v11model* model) {
    unsigned long long now;
    unsigned short stamp;
    if (e->size != model->size) return 0;
    stamp = vita_geo_stamp(model);
    now = C2VitaGLTicksNS();
    if (stamp != 0 && e->verified_generation == stamp && now - e->verified_ns < VITA_GEO_TRUST_NS) {
        if ((++vita_geo_sample & 63) != 0 || vita_geo_equal(e->snapshot, model, model->size)) {
            ++C2VitaGeometryStaticTrusted;
            return 1;
        }
        ++C2VitaGeometryStampMisses;
        return 0;
    }
    if (!vita_geo_equal(e->snapshot, model, model->size)) return 0;
    e->verified_generation = stamp;
    e->verified_ns = now;
    return 1;
}
static int vita_geo_equal(const void* a, const void* b, size_t n) {
    const uint32_t* x;
    const uint32_t* y;
    uint32_t d = 0;
    C2VitaGeometryBytesCompared += n;
    if ((((uintptr_t)a | (uintptr_t)b) & 3) != 0) return memcmp(a, b, n) == 0;
    x = (const uint32_t*)a;
    y = (const uint32_t*)b;
#ifdef __ARM_NEON
    /* Equal data is the common case, so reduce in NEON over 512 bytes before
     * one lane transfer (NEON->ARM moves stall the Cortex-A9), and prefetch
     * both streams ~384 bytes ahead, one 32-byte line per PLD. */
    while (n >= 512) {
        uint32x4_t d0 = vdupq_n_u32(0), d1 = d0;
        unsigned k;
        for (k = 0; k < 8; ++k) {
            __builtin_prefetch((const char*)x + 384, 0, 0);
            __builtin_prefetch((const char*)x + 416, 0, 0);
            __builtin_prefetch((const char*)y + 384, 0, 0);
            __builtin_prefetch((const char*)y + 416, 0, 0);
            d0 = vorrq_u32(d0, veorq_u32(vld1q_u32(x), vld1q_u32(y)));
            d1 = vorrq_u32(d1, veorq_u32(vld1q_u32(x + 4), vld1q_u32(y + 4)));
            d0 = vorrq_u32(d0, veorq_u32(vld1q_u32(x + 8), vld1q_u32(y + 8)));
            d1 = vorrq_u32(d1, veorq_u32(vld1q_u32(x + 12), vld1q_u32(y + 12)));
            x += 16; y += 16;
        }
        d0 = vorrq_u32(d0, d1);
        {
            uint32x2_t r = vorr_u32(vget_low_u32(d0), vget_high_u32(d0));
            if (vget_lane_u32(r, 0) | vget_lane_u32(r, 1)) return 0;
        }
        n -= 512;
    }
#endif
    while (n >= 32) {
        d |= (x[0] ^ y[0]) | (x[1] ^ y[1]) | (x[2] ^ y[2]) | (x[3] ^ y[3])
           | (x[4] ^ y[4]) | (x[5] ^ y[5]) | (x[6] ^ y[6]) | (x[7] ^ y[7]);
        if (d) return 0;
        x += 8; y += 8; n -= 32;
    }
    while (n >= 4) { d |= *x++ ^ *y++; n -= 4; }
    if (d) return 0;
    return n == 0 || memcmp(x, y, n) == 0;
}
static size_t vita_geo_bytes;
static unsigned long long vita_geo_clock, vita_geo_hits, vita_geo_misses;
static int vita_geo_inside(const struct v11model* m, const void* p, size_t n) {
    uintptr_t base=(uintptr_t)m, addr=(uintptr_t)p;
    return addr>=base && addr-base<=m->size && n<=m->size-(addr-base);
}
static int vita_geo_eligible(const struct v11model* m) {
    if (m->size<sizeof(*m) || m->size>VITA_GEO_MAX_MODEL || !m->ngroups
            || !vita_geo_inside(m,m->groups,(size_t)m->ngroups*sizeof(*m->groups))) return 0;
    for (unsigned i=0;i<m->ngroups;i++) {
        const struct v11group* g=m->groups+i;
        if (!vita_geo_inside(m,g->vertices,(size_t)g->nvertices*sizeof(*g->vertices))
                || !vita_geo_inside(m,g->faces,(size_t)g->nfaces*sizeof(*g->faces))
                || !vita_geo_inside(m,g->vertex_colours,(size_t)g->nvertices*sizeof(*g->vertex_colours))) return 0;
    }
    return 1;
}
/* Stored objects can also be destroyed by their facility's object-container
 * teardown. Forget their snapshots there, so owner cleanup cannot double-free. */
void GLVitaGeometryCacheForget(br_geometry_stored* geometry) {
    for (unsigned i=0;i<VITA_GEO_SLOTS;i++) if (vita_geo_entries[i].geometry==geometry) {
        vita_geo_bytes-=vita_geo_entries[i].size;
        free(vita_geo_entries[i].snapshot);
        memset(vita_geo_entries+i,0,sizeof(vita_geo_entries[i]));
    }
}
static void vita_geo_drop(vita_geo_entry* e) {
    br_geometry_stored* geometry=e->geometry;
    if (!geometry) return;
    /* Clear bookkeeping before the destructor's Forget callback. */
    vita_geo_bytes-=e->size;free(e->snapshot);memset(e,0,sizeof(*e));
    geometry->dispatch->_free((br_object*)geometry);
}
static void GLVitaGeometryCacheClearOwner(br_geometry_v1_model* owner) {
    for (unsigned i=0;i<VITA_GEO_SLOTS;i++) if(vita_geo_entries[i].owner==owner) vita_geo_drop(vita_geo_entries+i);
}
static vita_geo_entry* vita_geo_oldest(void) {
    vita_geo_entry* oldest=NULL;
    for (unsigned i=0;i<VITA_GEO_SLOTS;i++) if (vita_geo_entries[i].geometry
            && (!oldest || vita_geo_entries[i].age<oldest->age)) oldest=vita_geo_entries+i;
    return oldest;
}
static br_geometry_stored* GLVitaGeometryCacheGet(br_geometry_v1_model* owner,
        br_renderer* renderer, struct v11model* model, br_token type) {
    vita_geo_entry* slot=NULL;
    if (!vita_geo_eligible(model)) return NULL;
    ++vita_geo_clock;
    {   /* fast path: the slot this model used last time */
        unsigned h=vita_geo_hash(model);
        vita_geo_entry* e=vita_geo_entries+vita_geo_hint[h];
        if(e->geometry && e->owner==owner && e->renderer==renderer && e->model==model && e->type==type
                && vita_geo_current(e,model)) {
            e->age=vita_geo_clock;++vita_geo_hits;
            return e->geometry;
        }
    }
    for (unsigned i=0;i<VITA_GEO_SLOTS;i++) {
        vita_geo_entry* e=vita_geo_entries+i;
        if (!e->geometry) {if (!slot) slot=e;continue;}
        if(e->owner==owner && e->renderer==renderer && e->model==model && e->type==type) {
            if(vita_geo_current(e,model)) {
                e->age=vita_geo_clock;++vita_geo_hits;
                vita_geo_hint[vita_geo_hash(model)]=(unsigned short)i;
                if (vita_geo_clock%1000==0) {
                    extern void C2VitaLog(const char*,...);
                    C2VitaLog("VITA GEOMETRY lookups=%llu hits=%llu misses=%llu snapshot_bytes=%u\n",
                        vita_geo_clock,vita_geo_hits,vita_geo_misses,(unsigned)vita_geo_bytes);
                }
                return e->geometry;
            }
            vita_geo_drop(e);slot=e;break;
        }
    }
    ++vita_geo_misses;
    if (!slot) {slot=vita_geo_oldest();vita_geo_drop(slot);}
    while(vita_geo_bytes+model->size>VITA_GEO_BUDGET) vita_geo_drop(vita_geo_oldest());
    unsigned char* snapshot=malloc(model->size);
    if(!snapshot) return NULL; /* optional: ordinary immediate rendering remains */
    memcpy(snapshot,model,model->size);
    br_geometry_stored* geometry=BREND_FN(GeometryStored, Allocate)(owner,"VITA-CACHED",renderer,model);
    if(!geometry) {free(snapshot);return NULL;}
    if(type==BRT_LINE) StoredGLUseEdges(geometry);
    *slot=(vita_geo_entry){owner,renderer,model,type,model->size,snapshot,geometry,vita_geo_clock,
        vita_geo_stamp(model), C2VitaGLTicksNS()};
    vita_geo_hint[vita_geo_hash(model)]=(unsigned short)(slot-vita_geo_entries);
    vita_geo_bytes+=model->size;
    return geometry;
}
#endif
