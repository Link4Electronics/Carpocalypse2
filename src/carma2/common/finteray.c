#include "finteray.h"

#include "brucetrk.h"

#include "crush.h"

#include "errors.h"

#include "globvars.h"
#include "physics.h"

#include "raycast.h"

#include "spark.h"
#include "world.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

#include "c2_math.h"

extern const br_scalar gFLOAT_0058996c;
extern const char gEmpty_string_00676920[];
extern const double gDouble_00589a08;

/*
 * Face-intersection raycasting (ported from finteray.c — real implementation).
 */
#include "finteray.h"

#include "brucetrk.h"
#include "world3.h"
#include "globvars.h"
#include "spark.h"
int C2_HOOK_FASTCALL BadDiv__finteray(br_scalar a, br_scalar b);
extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);
extern br_material* gBlack_material;
extern const br_scalar gFLOAT_00589968;
extern const br_scalar gFLOAT_0058996c;
extern const br_scalar gFLOAT_00589978;
extern const br_scalar gFLOAT_005899a8;
extern const double gDouble_005899b0;
extern const char gEmpty_string_00676920[];

// GLOBAL: CARMA2_HW 0x005964c0
int gPling_materials = 1;

// GLOBAL: CARMA2_HW 0x006861c8
int gTemp_group;

// GLOBAL: CARMA2_HW 0x00686188
int gNearest_face;

// GLOBAL: CARMA2_HW 0x0068618c
br_model* gNearest_model;

// GLOBAL: CARMA2_HW 0x006861cc
br_actor* gNearest_actor;

// GLOBAL: CARMA2_HW 0x00686190
br_scalar gNearest_T;

// GLOBAL: CARMA2_HW 0x00686194
int gNearest_face_group;

// GLOBAL: CARMA2_HW 0x00686198
br_matrix34 gPick_model_to_view__finteray;

// GLOBAL: CARMA2_HW 0x0079d858
tFace_ref* gPling_face;

// GLOBAL: CARMA2_HW 0x005964c4
int gActorBoxPick_StopGroovidelics = 1;

// GLOBAL: CARMA2_HW 0x005896d4
br_scalar gFloat_005896d4 = 0.14492753148078918f;

// GLOBAL: CARMA2_HW 0x00589a08
const double gDouble_00589a08 = 0.707;

// GLOBAL: CARMA2_HW 0x00589a10
const float gFLOAT_00589a10 = 0.005f;

// GLOBAL: CARMA2_HW 0x00589a14
const float gFLOAT_00589a14 = -0.005f;

// GLOBAL: CARMA2_HW 0x00589a1c
const float gFLOAT_00589a1c = 0.5f;

// GLOBAL: CARMA2_HW 0x00589a20
const float gFLOAT_00589a20 = 0.2f;

// GLOBAL: CARMA2_HW 0x00589fac
const float gUNK_00589fac = 0.0f;

// GLOBAL: CARMA2_HW 0x00589fc4
const float gUNK_00589fc4 = 2.384185791015625e-7f;

// GLOBAL: CARMA2_HW 0x00589fd0
const float gFloat_00589fd0 = 2.0f;

// GLOBAL: CARMA2_HW 0x00589fd8
const double gDouble_00589fd8 = 0.000001;

// GLOBAL: CARMA2_HW 0x00589fe0
const double gDouble_00589fe0 = -0.002;

// GLOBAL: CARMA2_HW 0x00589fe8
const double gDouble_00589fe8 = 1.0;

// GLOBAL: CARMA2_HW 0x00589ff4
const float gFLOAT_00589ff4 = 0.002f;

// GLOBAL: CARMA2_HW 0x00589ff8
const float gFLOAT_00589ff8 = -2.0f;

// GLOBAL: CARMA2_HW 0x0058a000
const double gDouble_0058a000 = 0.002;

// GLOBAL: CARMA2_HW 0x0058a038
const float gUNK_0058a038 = -0.002f;

// GLOBAL: CARMA2_HW 0x0058f7f0
const tPolyhedron_edge_indexes gBox_edges_0058f7f0[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

#include "world2.h"
#include "structur.h"
#include "physics.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

#include "c2_math.h"

#include <string.h>

/* FP matching helpers ported from upstream 34-finteray.c (REC2). */
static float C2_HOOK_FASTCALL FinterayScalar(double value) {
    volatile float scalar = (float)value;
    return scalar;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static br_scalar C2_HOOK_FASTCALL FinterayEdgeScalar(double value) {
    br_scalar scalar = FinterayScalar(value);
    br_uint_32 bits;
    volatile br_uint_32 stored;
    memcpy(&bits, &scalar, sizeof(bits));
    stored = bits;
    bits = stored;
    memcpy(&scalar, &bits, sizeof(scalar));
    return scalar;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif
static double C2_HOOK_FASTCALL FinterayEdgeDifference(const br_scalar* a, const br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1\n\tfsubs %2\n\tfstpl %0" : "=m"(result) : "m"(*a), "m"(*b) : "st");
#else
    result = (double)*a - *b;
#endif
    return result;
}
static int C2_HOOK_FASTCALL FinterayBoxNaN(double value) {
    union { double value; tU32 words[2]; } bits;
    bits.value = value;
    return (bits.words[1] & 0x7ff00000u) == 0x7ff00000u && ((bits.words[1] & 0xfffffu) != 0 || bits.words[0] != 0);
}
static void C2_HOOK_FASTCALL FinterayBoxCopyScalar(br_scalar* output, const br_scalar* input) {
    tU32 value;
    memcpy(&value, input, sizeof(value));
    memcpy(output, &value, sizeof(value));
}
static void C2_HOOK_FASTCALL FinterayBoxStoreScalar(br_scalar* output, const br_scalar* input) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, output
        mov ecx, input
        fld dword ptr [ecx]
        fstp dword ptr [eax]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fstps %0" : "=m" (*output) : "m" (*input) : "st");
#else
    volatile double value = *input;
    *output = (br_scalar)value;
#endif
}
static double C2_HOOK_FASTCALL FinterayShapeLoadScalar(const br_scalar* input) {
    return *(const volatile br_scalar*)input;
}
static br_vector4* C2_HOOK_FASTCALL FinterayShapePlanes(tCollision_shape_polyhedron_data* poly) {
    return *(br_vector4* volatile*)&poly->planes;
}
static int C2_HOOK_FASTCALL FinterayShapePlaneCount(tCollision_shape_polyhedron_data* poly) {
    return *(volatile int*)&poly->count_planes;
}


// FUNCTION: CARMA2_HW 0x0045d5c0
void C2_HOOK_FASTCALL EnablePlingMaterials(void) {

    gPling_materials = 1;
}

// FUNCTION: CARMA2_HW 0x0045d5d0
void C2_HOOK_FASTCALL DisablePlingMaterials(void) {

    gPling_materials = 0;
}

int C2_HOOK_FASTCALL BadDiv__finteray(br_scalar a, br_scalar b) {

    return fabsf(b) < 1.0f && fabsf(b) * BR_SCALAR_MAX < fabsf(a);
}

#ifdef CARPOCALYPSE2_FIX_BUGS
/* v11group.face_colours normally points at a br_colour array; game code stores
 * the group's material through *face_colours.materials once the model has been
 * prepared. Only trust the slot when it agrees with the group's real face
 * material, otherwise fall back like the retail (32-bit) read does. */
static br_material* DrGroupMaterial(br_model* model, v11group* group, br_material* fallback) {
    br_material* face_material;

    if (model == NULL || model->faces == NULL || group->face_user == NULL) {
        return fallback;
    }
    face_material = model->faces[group->face_user[0]].material;
    if (face_material == NULL) {
        return fallback;
    }
    if (*group->face_colours.materials == face_material) {
        return face_material;
    }
    return fallback;
}
#endif

// FUNCTION: CARMA2_HW 0x0045f0d0
void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt) {
    int i;
    br_scalar t[4];
    br_scalar numerator;
    br_scalar d;
    br_vector3 p[4];
    br_vector3 tv;
    int axis_m;
    int axis_0;
    int axis_1;
    br_scalar u0[4];
    br_scalar u1;
    br_scalar u2;
    br_scalar v0[4];
    br_scalar v1;
    br_scalar v2;
    br_scalar v0i1;
    br_scalar v0i2;
    br_material* this_material;

    C2_HOOK_BUG_ON(sizeof(tFace_ref) != 80);

    this_material = pFace->material;
    d = BrVector3Dot(ray_dir, &pFace->normal);
    for (i = 0; i < pNum_rays; ++i) {
        rt[i] = 100.f;
    }
    if (this_material == NULL || (((this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) != 0 || d <= 0.f)
        && (this_material == NULL || this_material->identifier == NULL || (this_material->identifier[0] != '!' && this_material->identifier[0] != '>' && this_material->identifier[0] != '?') || !gPling_materials)
        && fabsf(d) >= 2.384186e-7f)) {
        for (i = 0; i < pNum_rays; i++) {

            BrVector3Sub(&tv, &ray_pos[i], &pFace->v[0]);
            numerator = BrVector3Dot(&pFace->normal, &tv);
            if (BadDiv__finteray(numerator, d)) {
                return;
            }
            if (d > 0.0f) {
                if (-numerator < -0.001 || -numerator > d + 0.003) {
                    t[i] = 100.0f;
                    continue;
                }
            } else if (numerator < -0.001 || 0.003 - d < numerator) {
                t[i] = 100.0f;
                continue;
            }
            t[i] = -(numerator / d);
            if (t[i] > 1.0f) {
                t[i] = 1.0f;
            }
            BrVector3Scale(&p[i], ray_dir, t[i]);
            BrVector3Accumulate(&p[i], &ray_pos[i]);
        }
        axis_m = fabsf(pFace->normal.v[0]) < fabsf(pFace->normal.v[1]);
        if (fabsf(pFace->normal.v[2]) > fabsf(pFace->normal.v[axis_m])) {
            axis_m = 2;
        }
        if (axis_m == 0) {
            axis_0 = 1;
            axis_1 = 2;
        } else if (axis_m == 1) {
            axis_0 = 0;
            axis_1 = 2;
        } else {
            axis_0 = 0;
            axis_1 = 1;
        }
        v0i1 = pFace->v[0].v[axis_0];
        v0i2 = pFace->v[0].v[axis_1];
        u1 = pFace->v[1].v[axis_0] - v0i1;
        u2 = pFace->v[2].v[axis_0] - v0i1;
        v1 = pFace->v[1].v[axis_1] - v0i2;
        v2 = pFace->v[2].v[axis_1] - v0i2;
        for (i = 0; i < pNum_rays; i++) {
            if (t[i] != 100.0f) {
                br_scalar s1, s2, s3, s4;

                u0[i] = p[i].v[axis_0] - v0i1;
                v0[i] = p[i].v[axis_1] - v0i2;
                s1 = (br_scalar)(u1 * v0[i] - v1 * u0[i]);
                s2 = (br_scalar)(v2 * u1 - v1 * u2);
                if (fabsf(s1) <= 1.0001 * fabsf(s2) && s2 != 0.f) {
                    s1 = s1 / s2;
                    if (s1 >= -0.0001) {
                        s3 = (br_scalar)(u2 * v0[i] - v2 * u0[i]);
                        s4 = -s2;
                        if (fabsf(s3) <= 1.0001 * fabsf(s4) && s4 != 0.f) {
                            s3 = s3 / s4;
                            if (s3 >= -0.0001 && s1 + s3 <= 1.0001) {
                                rt[i] = t[i];
                                *normal = pFace->normal;
                                if (d > 0.f) {
                                    BrVector3Negate(normal, &pFace->normal);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f5b0
void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
    br_vector3 a;
    br_vector3 c[3];
    int i;
    int j;

    BrMatrix34ApplyP(&b2->min, &b1->min, m);
    BrVector3Copy(&b2->max, &b2->min);
    BrVector3Sub(&a, &b1->max, &b1->min);
    for (j = 0; j < 3; j++) {
        BrVector3Scale(&c[j], (br_vector3*)m->m[j], a.v[j]);
    }
    for (j = 0; j < 3; ++j) {
        for (i = 0; i < 3; ++i) {
            if (c[i].v[j] < 0.f) {
                b2->min.v[j] += c[i].v[j];
            } else {
                b2->max.v[j] += c[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045cd90
int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] < -2.3841858e-7f) {
            s = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] > 2.3841858e-7f) {
            s = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->max.v[i]);
            if (s < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (s < t_far) {
                t_far = s;
            }
            t = (-1.0f / (rd->v[i] * 1.0f)) * (rp->v[i] - b->min.v[i]);
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else if (rp->v[i] > b->max.v[i] || rp->v[i] < b->min.v[i]) {
            return 0;
        }
    }
    if (t_near <= t_far) {
        *new_t_near = t_near;
        *new_t_far = t_far;
        return 1;
    }
    return 0;
}

void C2_HOOK_FASTCALL DRVector2AccumulateScale__finteray(br_vector2* a, const br_vector2* b, br_scalar s) {

    a->v[0] = b->v[0] * s + a->v[0];
    a->v[1] = b->v[1] * s + a->v[1];
}

// FUNCTION: CARMA2_HW 0x0045cf60
int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg) {
    DR_FACE* fp;
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    br_scalar t;
    br_scalar d;
    br_vector3 p;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    br_scalar v0i1;
    br_scalar v0i2;
    float alpha;
    float beta;
    float f_d;
    float f_n;
    br_scalar s_alpha;
    br_scalar s_beta;
    br_vector2 map;
    int v;
    int e;
    int r;
    br_material* this_material;
    br_scalar numerator;
    int group;

    t_near -= 1e-5f;
    t_far += 1e-5f;
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        for (f = 0; f < V11MODEL(model)->groups[group].nfaces; f++) {
            fp = &V11MODEL(model)->groups[group].faces[f];
#ifdef CARPOCALYPSE2_FIX_BUGS
            this_material = DrGroupMaterial(model, &V11MODEL(model)->groups[group], material);
#else
            if (V11MODEL(model)->groups[group].face_colours.materials != NULL) {
                this_material = *V11MODEL(model)->groups[group].face_colours.materials;
            } else {
                this_material = material;
            }
#endif
            d = BrVector3Dot(&fp->eqn, ray_dir);
            if (fabsf(d) < 2.3841858e-7f) {
                continue;
            }
            if (this_material != NULL && this_material->identifier != NULL && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?') && gPling_materials) {
                continue;
            }
            if (this_material != NULL && (this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
                continue;
            }
            numerator = BrVector3Dot(&fp->eqn, ray_pos) - fp->eqn.v[3];
            if (BadDiv__finteray(numerator, d)) {
                continue;
            }
            t = -(numerator / d);
            if (t < t_near || t > t_far) {
                continue;
            }
            BrVector3Scale(&p, ray_dir, t);
            BrVector3Accumulate(&p, ray_pos);
            axis_m = fabsf(fp->eqn.v[0]) < fabsf(fp->eqn.v[1]);
            if (fabsf(fp->eqn.v[2]) > fabsf(fp->eqn.v[axis_m])) {
                axis_m = 2;
            }
            if (axis_m == 0) {
                axis_0 = 1;
                axis_1 = 2;
            } else {
                axis_0 = 0;
                if (axis_m == 1) {
                    axis_1 = 2;
                } else {
                    axis_1 = 1;
                }
            }

            v0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_0];
            u0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_1];
            v1 = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_0] - v0;
            u1 = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_1] - u0;
            v2 = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_0] - v0;
            u2 = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_1] - u0;

            v0i1 = p.v[axis_0] - v0;
            v0i2 = p.v[axis_1] - u0;

            f_d = v0i2 * v1 - u1 * v0i1;
            f_n = u2 * v1 - u1 * v2;
            if (fabsf(f_d) > fabs(f_n)) {
                continue;
            }
            if (f_n == 0.f) {
                continue;
            }
            beta = f_d / f_n;
            if (beta < 0.f) {
                continue;
            }
            alpha = v0i2 * v2 - u2 * v0i1;
            if (fabsf(alpha) > fabsf(f_n)) { /* Verifies fabsf(alpha / f_n) < 1.f */
                continue;
            }
            alpha = alpha / -f_n;
            if (alpha < 0.f) {
                continue;
            }
            if (alpha + beta > 1.f) {
                continue;
            }
            s_alpha = alpha;
            s_beta = beta;
            map.v[0] = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[0] * s_alpha;
            map.v[1] = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[1] * s_alpha;
            DRVector2AccumulateScale__finteray(&map,
                &V11MODEL(model)->groups[group].vertices[fp->vertices[2]].map,
                s_beta);
            DRVector2AccumulateScale__finteray(&map,
                &V11MODEL(model)->groups[group].vertices[fp->vertices[0]].map,
                1.f - s_alpha - s_beta);

            if (s_alpha <= s_beta) {
                e = (1.f - s_beta) / 2.f <= s_alpha;
                v = s_alpha > 1.f - 2.f * s_beta;
            } else {
                e = (1.f - 2.f * s_beta <= s_alpha) ? 1 : 2;
                v = (alpha <= (1.f - s_beta) / 2.f) ? 0 : 2;
            }

            gTemp_group = group;
            r = callback(model, actor, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
            if (r != 0) {
                return r;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045cb70
int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_matrix34 mat;
    br_matrix34 invmat;
    br_vector3 pos;
    br_vector3 dir;
    void* arg;

    t_near = 0.f;
    t_far = 1.f;
    r = 0;
    arg = NULL;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        BrTransformToMatrix34(&mat, &ap->t);
        BrMatrix34Inverse(&invmat, &mat);
        BrMatrix34ApplyP(&pos, pPosition, &invmat);
        BrMatrix34ApplyV(&dir, pDir, &invmat);
        pPosition = &pos;
        pDir = &dir;
    }
    switch (ap->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            if (PickBoundsTestRay__finteray(&this_model->bounds, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
                t_near = 0.f;
                if (gNearest_T > 1.f) {
                    t_far = 1.f;
                } else {
                    t_far = gNearest_T;
                }
                r = callback(ap, this_model, this_material, pPosition, pDir, t_near, t_far, arg);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
        /* fall through */
    default:
        for (a = ap->children; a != NULL; a = a->next) {
            r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
            if (r) {
                break;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        if (PickBoundsTestRay__finteray((br_bounds*)ap->type_data, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
            for (a = ap->children; a != NULL; a = a->next) {
                r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
                if (r) {
                    break;
                }
            }
        }
        break;
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0045d570
int C2_HOOK_CDECL FindHighestPolyCallBack__finteray(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData) {

    if (pT < gNearest_T) {
        gNearest_T = pT;
        gNearest_model = pModel;
        gNearest_actor = pActor;
        gNearest_face = pF;
        gNearest_face_group = gTemp_group;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045cf20
int C2_HOOK_CDECL FindHighestCallBack__finteray(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    DRModelPick2D__finteray(pModel, pMaterial, pActor, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__finteray, pArg);
    return 0;
}

int C2_HOOK_FASTCALL DRSceneRayPick2D(br_actor* pWorld, br_vector3* pPosition, br_vector3* pDir, dr_pick2d_cbfn* pCallback) {

    BrMatrix34Inverse(&gPick_model_to_view__finteray, &pWorld->t.t.mat);
    return ActorRayPick2D(pWorld, pPosition, pDir, NULL, NULL, pCallback);
}

// FUNCTION: CARMA2_HW 0x0045ca60
void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* nor, br_scalar* t, br_material** material, br_actor** actor) {
    int group;

    gNearest_T = 100.0f;
    DRSceneRayPick2D(pWorld, pPosition, pDir, FindHighestCallBack__finteray);
    *t = gNearest_T;
    if (gNearest_T < 100.0f) {
        group = gNearest_face_group;
        BrVector3Copy(nor, &V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn);
#ifdef CARPOCALYPSE2_FIX_BUGS
        *material = DrGroupMaterial(gNearest_model, &V11MODEL(gNearest_model)->groups[group], NULL);
#else
        *material = *V11MODEL(gNearest_model)->groups[group].face_colours.materials;
#endif
        if (actor != NULL) {
            *actor = gNearest_actor;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041e340
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

// FUNCTION: CARMA2_HW 0x0045ecc0
void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos) {
    br_scalar t;
    br_scalar numerator;
    br_scalar d;
    br_vector3 p;
    int axis_m;
    int axis_0;
    int axis_1;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    br_scalar v0i1;
    br_scalar v0i2;
    float alpha;
    float beta;
    float f_d;
    br_material* this_material;

    this_material = pFace->material;
    *rt = 100.f;

    d = BrVector3Dot(&pFace->normal, ray_dir);
    if (this_material != NULL && (this_material->flags & (BR_MATF_TWO_SIDED | BR_MATF_ALWAYS_VISIBLE)) == 0 && d > 0.f) {
        return;
    }
    if (this_material != NULL && this_material->identifier != NULL && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?') && gPling_materials) {
        return;
    }
    if (fabsf(d) < 2.384186e-7f) {
        return;
    }
    BrVector3Sub(&p, ray_pos, &pFace->v[0]);
    numerator = BrVector3Dot(&pFace->normal, &p);
    if (BadDiv__finteray(numerator, d)) {
        return;
    }

    if (d > 0.f) {
        if (-numerator < 0.f || -numerator > 0.003 + d) {
            return;
        }
    } else {
        if ( numerator < 0.f ||  numerator > 0.003 - d) {
            return;
        }
    }
    t = -(numerator / d);
    if (t > 1.f) {
        t = 1.f;
    }
    BrVector3Scale(coll_pos, ray_dir, t);
    BrVector3Accumulate(coll_pos, ray_pos);
    axis_m = 0;
    if (fabs(pFace->normal.v[0]) < fabs(pFace->normal.v[1])) {
        axis_m = 1;
    }
    if (fabsf(pFace->normal.v[2]) > fabs(pFace->normal.v[axis_m])) {
        axis_m = 2;
    }
    switch (axis_m) {
    case 0:
        axis_0 = 1;
        axis_1 = 2;
        break;
    case 1:
        axis_0 = 0;
        axis_1 = 2;
        break;
    case 2:
        axis_0 = 0;
        axis_1 = 1;
        break;
    default:
        return;
    }
    v0i2 = pFace->v[0].v[axis_1];
    v0i1 = pFace->v[0].v[axis_0];
    u0 = pFace->v[1].v[axis_0] - v0i1;
    u1 = pFace->v[1].v[axis_1] - v0i2;
    v0 = pFace->v[2].v[axis_0] - v0i1;
    v1 = pFace->v[2].v[axis_1] - v0i2;
    u2 = coll_pos->v[axis_0] - v0i1;
    v2 = coll_pos->v[axis_1] - v0i2;

    f_d = u0 * v1 - u1 * v0;
    alpha = u0 * v2 - u1 * u2;
    if (fabsf(alpha) > 1.0001 * fabsf(f_d) || f_d == 0.f || alpha / f_d < -0.0001) {
        return;
    }
    beta = v0 * v2 - v1 * u2;
    if (fabsf(beta) > 1.0001 * fabsf(f_d) || f_d == 0.f || beta / f_d >= -0.0001 || alpha / f_d - beta / f_d > 1.0001) {
        return;
    }
    *rt = t;
    BrVector3Copy(normal, &pFace->normal);
    if (d > 0.f) {
        BrVector3Negate(normal, normal);
    }
}

// FUNCTION: CARMA2_HW 0x0045f690
void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds) {
    br_vector3 a;
    br_vector3 c[3];
    int i;

    BrVector3Add(&a, &bnds->original_bounds.min, &bnds->original_bounds.max);
    BrVector3Scale(&a, &a, 0.5f);
    BrMatrix34ApplyP(&bnds->box_centre, &a, bnds->mat);
    BrVector3Sub(&a, &bnds->original_bounds.max, &bnds->original_bounds.min);
    bnds->radius = BrVector3Length(&a) / 2.f;
    BrMatrix34ApplyP(&bnds->real_bounds.min, &bnds->original_bounds.min, bnds->mat);
    BrVector3Copy(&bnds->real_bounds.max, &bnds->real_bounds.min);
    for (i = 0; i < 3; ++i) {
        c[i].v[0] = bnds->mat->m[i][0] * a.v[i];
        c[i].v[1] = bnds->mat->m[i][1] * a.v[i];
        c[i].v[2] = bnds->mat->m[i][2] * a.v[i];
    }
    for (i = 0; i < 3; ++i) {
        bnds->real_bounds.min.v[i] += (float)(c[2].v[i] < 0.f) * c[2].v[i]
            + (float)(c[1].v[i] < 0.f) * c[1].v[i]
            + (float)(c[0].v[i] < 0.f) * c[0].v[i];
        bnds->real_bounds.max.v[i] += (float)(c[0].v[i] > 0.f) * c[0].v[i]
            + (float)(c[2].v[i] > 0.f) * c[2].v[i]
            + (float)(c[1].v[i] > 0.f) * c[1].v[i];
    }
}

// FUNCTION: CARMA2_HW 0x00425ba0
int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2) {
    int i;
    if (b1->min.v[2] > b2->max.v[2]
           || b2->min.v[2] > b1->max.v[2]
           || b1->min.v[1] > b2->max.v[1]
           || b2->min.v[1] > b1->max.v[1]
           || b1->min.v[0] > b2->max.v[0]
           || b2->min.v[0] > b1->max.v[0]) {

        return 0;
    }

    for (i = 0; i < 3; i++) {
        if (notfinite(b1->min.v[i]) || notfinite(b1->max.v[i])
                || notfinite(b2->min.v[i]) || notfinite(b2->max.v[i])) {
            return 0;
        }
    }
    return 1;
}

// Segment-vs-AABB overlap (Liang-Barsky slab test). Returns nonzero when the
// segment from pA to pB touches the box.
// NOTE: helper (not original 0x00460bd0); original LineBoxColl is below.
int C2_HOOK_CDECL TestSegmentBox(const br_vector3* pA, const br_vector3* pB, const br_bounds3* pBox) {
    float t_enter;
    float t_exit;
    float d;
    float t0;
    float t1;
    int i;

    t_enter = 0.f;
    t_exit = 1.f;
    for (i = 0; i < 3; i++) {
        d = pB->v[i] - pA->v[i];
        if (d == 0.f) {
            if (pA->v[i] < pBox->min.v[i] || pA->v[i] > pBox->max.v[i]) {
                return 0;
            }
        } else {
            if (d > 0.f) {
                t0 = (pBox->min.v[i] - pA->v[i]) / d;
                t1 = (pBox->max.v[i] - pA->v[i]) / d;
            } else {
                t0 = (pBox->max.v[i] - pA->v[i]) / d;
                t1 = (pBox->min.v[i] - pA->v[i]) / d;
            }
            if (t0 > t_enter) {
                t_enter = t0;
            }
            if (t1 < t_exit) {
                t_exit = t1;
            }
            if (t_enter > t_exit) {
                return 0;
            }
        }
    }
    return 1;
}

static void C2_HOOK_FASTCALL ClipToPlaneGE(br_scalar limit, br_vector3* p, int* nv, int i) {
    int last_vertex;
    int j;
    int vertex;
    int k;
    br_vector3 p2[12];

    last_vertex = *nv - 1;
    j = 0;
    for (vertex = 0; vertex < *nv; vertex++) {
        if ((p[last_vertex].v[i] > limit) != (p[vertex].v[i] > limit)) {
            for (k = 0; k < 3; k++) {
                if (i != k) {
                    p2[j].v[k] = (p[vertex].v[k] - p[last_vertex].v[k])
                                 * (limit - p[last_vertex].v[i])
                                 / (p[vertex].v[i] - p[last_vertex].v[i])
                                 + p[last_vertex].v[k];
                }
            }
            p2[j++].v[i] = limit;
        }
        if (p[vertex].v[i] >= limit) {
            BrVector3Copy(&p2[j], &p[vertex]);
            j++;
        }
        last_vertex = vertex;
    }
    *nv = j;
    for (k = 0; k < j; k++) {
        BrVector3Copy(&p[k], &p2[k]);
    }
}

static void C2_HOOK_FASTCALL ClipToPlaneLE(br_scalar limit, br_vector3* p, int* nv, int i) {
    int last_vertex;
    int j;
    int vertex;
    int k;
    br_vector3 p2[12];

    last_vertex = *nv - 1;
    j = 0;
    for (vertex = 0; vertex < *nv; vertex++) {
        if ((p[vertex].v[i] > limit) != (p[last_vertex].v[i] > limit)) {
            for (k = 0; k < 3; k++) {
                if (k != i) {
                    p2[j].v[k] = (p[vertex].v[k] - p[last_vertex].v[k])
                                 * (limit - p[last_vertex].v[i])
                                 / (p[vertex].v[i] - p[last_vertex].v[i])
                                 + p[last_vertex].v[k];
                }
            }
            p2[j++].v[i] = limit;
        }
        if (p[vertex].v[i] <= (double)limit) {
            BrVector3Copy(&p2[j], &p[vertex]);
            j++;
        }
        last_vertex = vertex;
    }
    *nv = j;
    for (k = 0; k < j; k++) {
        BrVector3Copy(&p[k], &p2[k]);
    }
}

// FUNCTION: CARMA2_HW 0x0045ff90
int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
    int f;
    int i;
    int n;
    int group;
    int v1;
    int v2;
    int v3;
    br_vector3 polygon[12];
    br_vector3 a;
    br_vector3 tv;
    br_scalar t;
    v11model* prepared;

    prepared = model->prepared;
    if (max_face <= 0) {
        return 0;
    }
    for (group = 0; group < prepared->ngroups; group++) {
        v11group* v11g;

        v11g = &prepared->groups[group];
        for (f = 0; f < v11g->nfaces; f++) {
            v11face* v11f;

            v11f = &v11g->faces[f];
            v1 = v11f->vertices[0];
            BrVector3Sub(&a, &v11g->vertices[v1].p, &bnds->box_centre);
            t = BrVector3Dot(&v11f->eqn, &a);
            if (fabsf(t) > bnds->radius) {
                continue;
            }
            v2 = v11f->vertices[1];
            v3 = v11f->vertices[2];

            t = bnds->real_bounds.min.v[0];
            if (t > v11g->vertices[v1].p.v[0]
                && t > v11g->vertices[v2].p.v[0]
                && t > v11g->vertices[v3].p.v[0]) {
                continue;
            }
            t = bnds->real_bounds.max.v[0];
            if (t < v11g->vertices[v1].p.v[0]
                && t < v11g->vertices[v2].p.v[0]
                && t < v11g->vertices[v3].p.v[0]) {
                continue;
            }
            t = bnds->real_bounds.min.v[1];
            if (t > v11g->vertices[v1].p.v[1]
                && t > v11g->vertices[v2].p.v[1]
                && t > v11g->vertices[v3].p.v[1]) {
                continue;
            }
            t = bnds->real_bounds.max.v[1];
            if (t < v11g->vertices[v1].p.v[1]
                && t < v11g->vertices[v2].p.v[1]
                && t < v11g->vertices[v3].p.v[1]) {
                continue;
            }
            t = bnds->real_bounds.min.v[2];
            if (t > v11g->vertices[v1].p.v[2]
                && t > v11g->vertices[v2].p.v[2]
                && t > v11g->vertices[v3].p.v[2]) {
                continue;
            }
            t = bnds->real_bounds.max.v[2];
            if (t < v11g->vertices[v1].p.v[2]
                && t < v11g->vertices[v2].p.v[2]
                && t < v11g->vertices[v3].p.v[2]) {
                continue;
            }
            BrVector3Sub(&polygon[1], &v11g->vertices[v1].p, (br_vector3*)bnds->mat->m[3]);
            BrVector3Sub(&polygon[2], &v11g->vertices[v2].p, (br_vector3*)bnds->mat->m[3]);
            BrVector3Sub(&polygon[3], &v11g->vertices[v3].p, (br_vector3*)bnds->mat->m[3]);
            BrMatrix34TApplyV(&polygon[0], &polygon[1], bnds->mat);
            BrMatrix34TApplyV(&polygon[1], &polygon[2], bnds->mat);
            BrMatrix34TApplyV(&polygon[2], &polygon[3], bnds->mat);
            n = 3;
            for (i = 0; i < 3; i++) {
                ClipToPlaneGE(bnds->original_bounds.min.v[i], polygon, &n, i);
                if (n < 3) {
                    break;
                }
                ClipToPlaneLE(bnds->original_bounds.max.v[i], polygon, &n, i);
                if (n < 3) {
                    break;
                }
            }
            if (n >= 3) {
                if (pMat != NULL) {
                    BrMatrix34ApplyP(&face_list->v[0], &v11g->vertices[v1].p, pMat);
                    BrMatrix34ApplyP(&face_list->v[1], &v11g->vertices[v2].p, pMat);
                    BrMatrix34ApplyP(&face_list->v[2], &v11g->vertices[v3].p, pMat);
                    BrVector3Copy(&tv, &v11f->eqn);
                    BrMatrix34ApplyV(&face_list->normal, &tv, pMat);
                } else {
                    BrVector3Copy(&face_list->v[0], &v11g->vertices[v1].p);
                    BrVector3Copy(&face_list->v[1], &v11g->vertices[v2].p);
                    BrVector3Copy(&face_list->v[2], &v11g->vertices[v3].p);
                    BrVector3Copy(&face_list->normal, &v11f->eqn);
                }
#ifdef CARPOCALYPSE2_FIX_BUGS
                face_list->material = DrGroupMaterial(model, v11g, model_material);
#else
                if (*v11g->face_colours.materials != NULL) {
                    face_list->material = *v11g->face_colours.materials;
                } else {
                    face_list->material = model_material;
                }
#endif
                if (pMat != NULL) {
                    face_list->d = BrVector3Dot(&face_list->v[0], &face_list->normal);
                } else {
                    face_list->d = v11f->eqn.v[3];
                }
                face_list->a = actor;
                if (model->flags & BR_MODF_UPDATEABLE) {
                    tS16 ff;

                    face_list->g = -1;
                    ff = v11g->face_user[f];
                    face_list->f = ff;
                    face_list->flags = model->faces[ff].flags;
                    face_list->map[0] = &model->vertices[model->faces[ff].vertices[0]].map;
                    face_list->map[1] = &model->vertices[model->faces[ff].vertices[1]].map;
                    face_list->map[2] = &model->vertices[model->faces[ff].vertices[2]].map;
                } else {
                    face_list->g = group;
                    face_list->f = f;
                    face_list->flags = 0;
                    if (v11f->edges[0] == 0) {
                        face_list->flags |= 0x1;
                    }
                    if (v11f->edges[1] == 0) {
                        face_list->flags |= 0x2;
                    }
                    if (v11f->edges[2] == 0) {
                        face_list->flags |= 0x4;
                    }
                    face_list->map[0] = &v11g->vertices[v1].map;
                    face_list->map[1] = &v11g->vertices[v2].map;
                    face_list->map[2] = &v11g->vertices[v3].map;
                }
                if (face_list->material != NULL
                        && face_list->material->identifier != NULL
                        && face_list->material->identifier[0] == '!') {
                    gPling_face = face_list;
                }
                face_list++;
                max_face--;
                if (max_face == 0) {
                    break;
                }
            }
        }
        if (max_face == 0) {
            break;
        }
    }
    return max_face;
}

static int C2_HOOK_FASTCALL BoundsTransformTest(br_bounds* b1, br_bounds* b2, br_matrix34* M) {
    br_scalar val;
    br_vector3 o;

    BrVector3Sub(&o, &b1->max, &b1->min);
    val = M->m[0][0] * b1->min.v[0] + M->m[1][0] * b1->min.v[1] + M->m[2][0] * b1->min.v[2] + M->m[3][0];

    if ((M->m[0][0] <= 0.0f ? 0.0f : M->m[0][0] * o.v[0])
            + (M->m[1][0] <= 0.0f ? 0.0f : M->m[1][0] * o.v[1])
            + (M->m[2][0] <= 0.0f ? 0.0f : M->m[2][0] * o.v[2])
            + val
            < b2->min.v[0]) {
        return 0;
    }
    if ((M->m[0][0] < 0.0f ? M->m[0][0] * o.v[0] : 0.0f)
            + (M->m[1][0] < 0.0f ? M->m[1][0] * o.v[1] : 0.0f)
            + (M->m[2][0] < 0.0f ? M->m[2][0] * o.v[2] : 0.0f)
            + val
            > b2->max.v[0]) {
        return 0;
    }

    val = M->m[0][2] * b1->min.v[0] + M->m[1][2] * b1->min.v[1] + M->m[2][2] * b1->min.v[2] + M->m[3][2];
    if ((M->m[0][2] <= 0.0f ? 0.0f : M->m[0][2] * o.v[0])
            + (M->m[1][2] <= 0.0f ? 0.0f : M->m[1][2] * o.v[1])
            + (M->m[2][2] <= 0.0f ? 0.0f : M->m[2][2] * o.v[2])
            + val
            < b2->min.v[2]) {
        return 0;
    }
    if ((M->m[0][2] < 0.0f ? M->m[0][2] * o.v[0] : 0.0f)
            + (M->m[1][2] < 0.0f ? M->m[1][2] * o.v[1] : 0.0f)
            + (M->m[2][2] < 0.0f ? M->m[2][2] * o.v[2] : 0.0f)
            + val
            > b2->max.v[2]) {
        return 0;
    }

    val = M->m[0][1] * b1->min.v[0] + M->m[1][1] * b1->min.v[1] + M->m[2][1] * b1->min.v[2] + M->m[3][1];
    if ((M->m[0][1] <= 0.0f ? 0.0f : M->m[0][1] * o.v[0])
            + (M->m[1][1] <= 0.0f ? 0.0f : M->m[1][1] * o.v[1])
            + (M->m[2][1] <= 0.0f ? 0.0f : M->m[2][1] * o.v[2])
            + val
            < b2->min.v[1]) {
        return 0;
    }
    if ((M->m[0][1] < 0.0 ? M->m[0][1] * o.v[0] : 0.0)
            + (M->m[1][1] < 0.0 ? M->m[1][1] * o.v[1] : 0.0)
            + (M->m[2][1] < 0.0 ? M->m[2][1] * o.v[2] : 0.0)
            + val
            > b2->max.v[1]) {
        return 0;
    }

    return 1;
}

// FUNCTION: CARMA2_HW 0x0045f8c0
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    br_model* this_model;
    br_material* this_material;
    int i;
    int n;
    int test_children;
    br_actor* a;
    br_actor* next_a;
    br_matrix34 mat;
    br_matrix34 mat2;
    br_matrix34 invmat;
    br_matrix34 box_to_actor;
    tBounds new_bounds;
    br_bounds br_bnds;

    i = 0;
    test_children = 1;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return max_face;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        if (this_model == NULL) {
            return max_face;
        }
        if (ap->children == NULL) {
            if (ap->type != BR_ACTOR_MODEL) {
                return max_face;
            }
            if (!BoundsTransformTest(&this_model->bounds, &bnds->real_bounds, &ap->t.t.mat)) {
                return max_face;
            }
        }
        if (pMat != NULL) {
            BrMatrix34Mul(&mat, &ap->t.t.mat, pMat);
            pMat = &mat;
        } else {
            pMat = &ap->t.t.mat;
        }
        BrMatrix34LPInverse(&invmat, &ap->t.t.mat);
        BrMatrix34Mul(&mat2, bnds->mat, &invmat);
        new_bounds.mat = &mat2;
        BrVector3Copy(&new_bounds.original_bounds.min, &bnds->original_bounds.min);
        BrVector3Copy(&new_bounds.original_bounds.max, &bnds->original_bounds.max);
        BrMatrix34ApplyP(&new_bounds.box_centre, &bnds->box_centre, &invmat);
        new_bounds.radius = bnds->radius;
        GetNewBoundingBox(&new_bounds.real_bounds, &new_bounds.original_bounds, new_bounds.mat);
        if (ap->identifier[1] >= '0' && ap->identifier[1] <= '9') {
            if (!BoundsOverlapTest__finteray(&new_bounds.real_bounds, &this_model->bounds)) {
                return max_face;
            }
            BrMatrix34LPInverse(&invmat, bnds->mat);
            BrMatrix34Mul(&box_to_actor, &ap->t.t.mat, &invmat);
            GetNewBoundingBox(&br_bnds, &ap->model->bounds, &box_to_actor);
            if (!BoundsOverlapTest__finteray(&br_bnds, &bnds->original_bounds)) {
                return max_face;
            }
            if (pWorld_callbacks != NULL && pWorld_callbacks->pull_actor_from_world != NULL) {
                if (pWorld_callbacks->pull_actor_from_world(ap) != NULL) {
                    return max_face;
                }
            }
        }
        bnds = &new_bounds;
    }
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model == NULL) {
            return max_face;
        }
        if (gActorBoxPick_StopGroovidelics && BoundsOverlapTest__finteray(&bnds->real_bounds, &this_model->bounds)) {
            n = ModelPickBox(ap, bnds, this_model, this_material, &face_list[i], max_face, pMat);
            if (pMat != NULL && max_face != n) {
                if (pWorld_callbacks != NULL && pWorld_callbacks->stop_groovidelic != NULL) {
                    pWorld_callbacks->stop_groovidelic(ap);
                }
            }
            i = max_face - n;
            max_face = n;
        }
    } else if (ap->type == BR_ACTOR_BOUNDS || ap->type == BR_ACTOR_BOUNDS_CORRECT) {
        test_children = BoundsOverlapTest__finteray(&bnds->real_bounds, (br_bounds*)ap->type_data);
    }
    if (test_children) {
        for (a = ap->children; a != NULL; a = next_a) {
            next_a = a->next;
            n = ActorBoxPick(bnds, a, this_model, this_material, &face_list[i], max_face, pMat, pWorld_callbacks);
            i += max_face - n;
            max_face = n;
        }
    }
    return max_face;
}

// FUNCTION: CARMA2_HW 0x004b57d0
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks) {
    int j;
    int x;
    int z;
    tU8 cx_min;
    tU8 cx_max;
    tU8 cz_min;
    tU8 cz_max;
    tTrack_spec* track_spec;

    j = 0;
    track_spec = &gProgram_state.track_spec;
    FillInBounds(bnds);
    XZToColumnXZ(&cx_min, &cz_min, bnds->real_bounds.min.v[0], bnds->real_bounds.min.v[2], track_spec);
    XZToColumnXZ(&cx_max, &cz_max, bnds->real_bounds.max.v[0], bnds->real_bounds.max.v[2], track_spec);
    if (cx_min > 0) {
        cx_min -= 1;
    }
    if (cz_min > 0) {
        cz_min -= 1;
    }
    if (cx_max + 1 < track_spec->ncolumns_x) {
        cx_max += 1;
    }
    if (cz_max + 1 < track_spec->ncolumns_z) {
        cz_max += 1;
    }
    for (x = cx_min; x <= cx_max; x++) {
        for (z = cz_min; z <= cz_max; z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                j = max_face - ActorBoxPick(bnds, track_spec->columns[z][x].actor_0x0, NULL, gBlack_material, &face_list[j], max_face - j, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        j = max_face - ActorBoxPick(bnds, gAdditional_actors, NULL, gBlack_material, &face_list[j], max_face - j, NULL, pWorld_callbacks);
    }
    return j;
}

typedef struct {
    int type;
    float max_xyz[3];
    float min_xyz[3];
    undefined4 field_0x1c[9];
    br_vector3* vectors_a;
    br_vector3* vectors_b;
} tFinteray_corner_source;

static int C2_HOOK_FASTCALL FinteraySphereFacePoint(tFace_ref* face, const volatile br_vector3* normal, const volatile br_vector3* point) {
    int axis;
    int axis_0;
    int axis_1;
    volatile br_scalar u0;
    volatile br_scalar v0;
    volatile br_scalar v1;
    volatile br_scalar v2;
    volatile br_scalar u2_scalar;
    volatile br_scalar du_scalar;
    volatile br_scalar dv;
    volatile br_scalar product_1;
    volatile br_scalar product_2;
    volatile br_scalar numerator;
    volatile br_scalar denominator;
    volatile br_scalar beta;
    double u1;
    double u2;
    double du;
    double n;
    double d;
    double ratio;

    axis = !(fabs(normal->v[0]) >= fabs(normal->v[1]));
    if (!(fabs(normal->v[axis]) >= fabs(normal->v[2]))) {
        axis = 2;
    }
    axis_0 = axis == 0 ? 1 : 0;
    axis_1 = axis == 2 ? 1 : 2;
    u0 = face->v[0].v[axis_1];
    v0 = face->v[0].v[axis_0];
    u2 = (double)face->v[2].v[axis_1] - u0;
    u1 = (double)face->v[1].v[axis_1] - u0;
    v1 = FinterayEdgeScalar((double)face->v[1].v[axis_0] - v0);
    v2 = FinterayEdgeScalar((double)face->v[2].v[axis_0] - v0);
    u2_scalar = FinterayEdgeScalar(u2);
    product_2 = FinterayEdgeScalar(u2 * v1);
    product_1 = FinterayEdgeScalar(u1 * v2);
    du = (double)point->v[axis_1] - u0;
    du_scalar = FinterayEdgeScalar(du);
    dv = FinterayEdgeScalar((double)point->v[axis_0] - v0);
    d = u2 * v1 - product_1;
    denominator = FinterayEdgeScalar(d);
    n = du * v1 - u1 * dv;
    numerator = FinterayEdgeScalar(n);
    if (!(fabs(n) <= fabs(d) * 1.0001) || FinterayBoxNaN(denominator) || denominator == 0.0f) {
        return 0;
    }
    ratio = (double)numerator / denominator;
    beta = FinterayEdgeScalar(ratio);
    if (!(ratio >= -0.0001)) {
        return 0;
    }
    d = (double)product_1 - product_2;
    denominator = FinterayEdgeScalar(d);
    n = (double)du_scalar * v2 - u2_scalar * (double)dv;
    numerator = FinterayEdgeScalar(n);
    if (!(fabs(n) <= fabs(d) * 1.0001) || FinterayBoxNaN(denominator) || denominator == 0.0f) {
        return 0;
    }
    ratio = (double)numerator / denominator;
    return ratio >= -0.0001 && !FinterayBoxNaN(ratio) && !(beta + ratio > 1.0001);
}



// FUNCTION: CARMA2_HW  0x0045dea0
void C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    double x;
    double y;
    double z;
    double projection;
    double product;
    double point_x;
    double limit;
    double length;
    double radius_squared;
    volatile br_scalar ratio;
    br_scalar normal_x;
    br_scalar normal_y;
    volatile br_scalar squared;

    x = FinterayShapeLoadScalar(&pSphere_end->v[0]) - FinterayShapeLoadScalar(&pEdge_start->v[0]);
    y = FinterayShapeLoadScalar(&pSphere_end->v[1]) - FinterayShapeLoadScalar(&pEdge_start->v[1]);
    z = FinterayShapeLoadScalar(&pSphere_end->v[2]) - FinterayShapeLoadScalar(&pEdge_start->v[2]);
    projection = ((x * FinterayShapeLoadScalar(&pEdge_direction->v[0])
        + FinterayShapeLoadScalar(&pEdge_direction->v[1]) * y) + z * FinterayShapeLoadScalar(&pEdge_direction->v[2])) / pEdge_length_squared;
    ratio = FinterayEdgeScalar(projection);
    if (FinterayBoxNaN(projection) || !(projection > 0.0)) {
        FinterayBoxCopyScalar(&pPoint->v[0], &pEdge_start->v[0]);
        FinterayBoxCopyScalar(&pPoint->v[1], &pEdge_start->v[1]);
        FinterayBoxCopyScalar(&pPoint->v[2], &pEdge_start->v[2]);
    } else if (ratio < 1.0f || FinterayBoxNaN(ratio)) {
        pPoint->v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_direction->v[0]) * ratio);
        product = FinterayShapeLoadScalar(&pEdge_direction->v[1]) * ratio;
        point_x = FinterayShapeLoadScalar(&pPoint->v[0]);
        pPoint->v[1] = FinterayEdgeScalar(product);
        pPoint->v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_direction->v[2]) * ratio);
        pPoint->v[0] = FinterayEdgeScalar(point_x + FinterayShapeLoadScalar(&pEdge_start->v[0]));
        pPoint->v[1] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_start->v[1]) + FinterayShapeLoadScalar(&pPoint->v[1]));
        pPoint->v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_start->v[2]) + FinterayShapeLoadScalar(&pPoint->v[2]));
    } else {
        FinterayBoxCopyScalar(&pPoint->v[0], &pEdge_end->v[0]);
        FinterayBoxCopyScalar(&pPoint->v[1], &pEdge_end->v[1]);
        FinterayBoxCopyScalar(&pPoint->v[2], &pEdge_end->v[2]);
    }
    pNormal->v[0] = FinterayEdgeScalar(FinterayEdgeDifference(&pPoint->v[0], &pSphere_end->v[0]));
    product = FinterayEdgeDifference(&pPoint->v[1], &pSphere_end->v[1]);
    FinterayBoxCopyScalar(&normal_x, &pNormal->v[0]);
    pNormal->v[1] = FinterayEdgeScalar(product);
    z = FinterayEdgeDifference(&pPoint->v[2], &pSphere_end->v[2]);
    normal_y = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[1]));
    squared = FinterayEdgeScalar((normal_y * (double)normal_y + z * z) + normal_x * (double)normal_x);
    pNormal->v[2] = FinterayEdgeScalar(z);
    radius_squared = FinterayShapeLoadScalar(&pSphere->radius_squared);
    if (!FinterayBoxNaN(squared) && !FinterayBoxNaN(radius_squared) && squared > radius_squared) {
        limit = FinterayShapeLoadScalar(&pSphere->radius) * 0.002f;
        limit = FinterayShapeLoadScalar(&pSphere->radius_squared) - limit * -2.0f;
        if (squared < limit || FinterayBoxNaN(limit)) {
            length = sqrt((double)squared);
            *pT = 1.0f;
            pNormal->v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[0]) / length);
            pNormal->v[1] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[1]) / length);
            pNormal->v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[2]) / length);
        }
    }
}


// FUNCTION: CARMA2_HW  0x0045e020
void C2_HOOK_FASTCALL FUN_0045e020(br_vector3* pVertex, br_vector3* pSphere_start, br_vector3* pSphere_end, br_vector3* pSphere_direction, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_scalar x;
    br_scalar y;
    br_scalar z;
    br_scalar direction_x;
    br_scalar direction_y;
    br_scalar direction_z;
    br_scalar projection;
    br_scalar distance;
    br_scalar time;
    br_scalar length;
    br_scalar scale;
    br_scalar point_z;
    br_vector3 delta;
    br_scalar direction_squared;
    br_scalar projection_scalar;
    br_scalar distance_squared;
    br_scalar time_scalar;

    (void)pSphere_end;
    direction_y = pSphere_direction->v[1];
    direction_z = pSphere_direction->v[2];
    y = pVertex->v[1] - pSphere_start->v[1];
    z = pVertex->v[2] - pSphere_start->v[2];
    direction_x = pSphere_direction->v[0];
    x = pVertex->v[0] - pSphere_start->v[0];
    delta.v[0] = x;
    delta.v[1] = y;
    delta.v[2] = z;
    direction_squared = (direction_y * direction_y + direction_z * direction_z) + direction_x * direction_x;
    projection = ((direction_z * delta.v[2] + pSphere_direction->v[1] * delta.v[1])
        + pSphere_direction->v[0] * delta.v[0]) / direction_squared;
    projection_scalar = projection;
    distance = ((x * delta.v[0] + y * delta.v[1]) + z * delta.v[2]) - (projection * projection_scalar) * direction_squared;
    distance_squared = distance;
    if (distance_squared > pSphere->radius_squared) {
        return;
    }
    time = projection_scalar - sqrtf((pSphere->radius_squared - distance_squared) / direction_squared);
    time_scalar = time;
    if (!(time >= 0.0f) || time_scalar > 1.0f) {
        return;
    }
    pPoint->v[0] = pVertex->v[0];
    pPoint->v[1] = pVertex->v[1];
    point_z = pVertex->v[2];
    pPoint->v[2] = point_z;
    direction_y = pSphere_direction->v[1] * time_scalar;
    direction_x = pSphere_direction->v[0] * time_scalar;
    direction_z = pSphere_direction->v[2] * time_scalar;
    direction_x += pSphere_start->v[0];
    direction_y += pSphere_start->v[1];
    direction_z += pSphere_start->v[2];
    x = pPoint->v[0] - direction_x;
    y = pPoint->v[1] - direction_y;
    z = point_z - direction_z;
    length = sqrtf((x * x + y * y) + z * z);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        pNormal->v[0] = scale * x;
        pNormal->v[1] = scale * y;
        pNormal->v[2] = scale * z;
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    *pT = time_scalar;
}


// FUNCTION: CARMA2_HW  0x0045e850
void C2_HOOK_FASTCALL SphereFaceCollide(tFace_ref* pFace, br_vector3* pStart, br_vector3* pEnd, br_scalar pRadius, void* pUnused_point, br_vector3* pNormal, br_scalar* pT) {
    br_material* material;
    volatile br_vector3 normal;
    volatile br_vector3 center;
    volatile br_vector3 point;
    volatile br_scalar start_scalar;
    volatile br_scalar time;
    volatile br_scalar plane_offset;
    double distance;
    double end_distance;
    double ratio;
    double x;
    double y;
    double z;

    (void)pUnused_point;
    *pT = 100.0f;
    material = pFace->material;
    if (material != NULL && material->identifier != NULL
        && (material->identifier[0] == '!' || material->identifier[0] == '>' || material->identifier[0] == '?')) {
        return;
    }
    distance = ((FinterayShapeLoadScalar(&pFace->normal.v[0]) * pStart->v[0]
        + (double)pStart->v[1] * pFace->normal.v[1]) + (double)pStart->v[2] * pFace->normal.v[2]) - pFace->d;
    start_scalar = FinterayEdgeScalar(distance);
    if (!(distance >= 0.0)) {
        if (start_scalar > -(double)pRadius && !FinterayBoxNaN(start_scalar) && !FinterayBoxNaN(pRadius)) {
            return;
        }
        if (material == NULL || !(material->flags & BR_MATF_TWO_SIDED)) {
            return;
        }
        normal.v[1] = FinterayEdgeScalar(-FinterayShapeLoadScalar(&pFace->normal.v[1]));
        normal.v[2] = FinterayEdgeScalar(-FinterayShapeLoadScalar(&pFace->normal.v[2]));
        normal.v[0] = FinterayEdgeScalar(-FinterayShapeLoadScalar(&pFace->normal.v[0]));
        end_distance = (((double)pEnd->v[2] * normal.v[2] + (double)pEnd->v[1] * normal.v[1])
            + normal.v[0] * (double)pEnd->v[0]) + pFace->d;
    } else {
        if (!(start_scalar >= pRadius)) {
            return;
        }
        normal.v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pFace->normal.v[2]));
        normal.v[1] = pFace->normal.v[1];
        normal.v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pFace->normal.v[0]));
        end_distance = (((double)pEnd->v[2] * normal.v[2] + (double)pEnd->v[1] * normal.v[1])
            + normal.v[0] * (double)pEnd->v[0]) - pFace->d;
    }
    if (FinterayBoxNaN(end_distance) || FinterayBoxNaN(pRadius) || !(end_distance <= (double)pRadius + 0.002)) {
        return;
    }
    ratio = ((double)pRadius - start_scalar) / (end_distance - start_scalar);
    time = FinterayEdgeScalar(ratio);
    if (!FinterayBoxNaN(ratio) && ratio > 1.0) {
        time = 1.0f;
    }
    if (time < 0.0f || FinterayBoxNaN(time)) {
        time = 0.0f;
    }
    x = ((double)pEnd->v[0] - pStart->v[0]) * time + pStart->v[0];
    y = ((double)pEnd->v[1] - pStart->v[1]) * time + pStart->v[1];
    z = ((double)pEnd->v[2] - pStart->v[2]) * time + pStart->v[2];
    center.v[0] = FinterayEdgeScalar(x);
    center.v[1] = FinterayEdgeScalar(y);
    center.v[2] = FinterayEdgeScalar(z);
    plane_offset = FinterayEdgeScalar((((double)pFace->v[0].v[0] - center.v[0]) * normal.v[0]
        + ((double)pFace->v[0].v[1] - center.v[1]) * normal.v[1]) + ((double)pFace->v[0].v[2] - center.v[2]) * normal.v[2]);
    point.v[0] = FinterayEdgeScalar(normal.v[0] * (double)plane_offset + center.v[0]);
    point.v[1] = FinterayEdgeScalar(normal.v[1] * (double)plane_offset + center.v[1]);
    point.v[2] = FinterayEdgeScalar(normal.v[2] * (double)plane_offset + center.v[2]);
    if (FinteraySphereFacePoint(pFace, &normal, &point)) {
        *pT = time;
        FinterayBoxCopyScalar(&pNormal->v[1], (const br_scalar*)&normal.v[1]);
        pNormal->v[0] = FinterayEdgeScalar(normal.v[0]);
        FinterayBoxCopyScalar(&pNormal->v[2], (const br_scalar*)&normal.v[2]);
    }
}


// FUNCTION: CARMA2_HW  0x00460da0
int C2_HOOK_FASTCALL SphereBoxIntersection(br_bounds* pBounds, br_vector3* pPoint, br_scalar pRadius_squared, br_vector3* pClosest) {
    int axis;
    br_scalar closest;
    br_scalar difference;
    br_scalar distance = 0.0f;

    for (axis = 0; axis < 3; axis++) {
        if (!(pPoint->v[axis] > pBounds->min.v[axis])) {
            closest = pBounds->min.v[axis];
        } else if (pPoint->v[axis] >= pBounds->max.v[axis]) {
            closest = pBounds->max.v[axis];
        } else {
            closest = pPoint->v[axis];
        }
        pClosest->v[axis] = (br_scalar)closest;
        difference = pPoint->v[axis] - closest;
        distance += difference * difference;
    }
    return !(distance > pRadius_squared);
}


// FUNCTION: CARMA2_HW  0x00460e10
int C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    int axis;
    br_scalar length;
    br_scalar scale;
    br_scalar x;
    br_scalar y;
    br_scalar z;

    result = LineBoxColl(pStart, pEnd, pBounds, pPoint, pT);
    if (result != 0) {
        pNormal->v[0] = 0.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
        pNormal->v[(result - 1) & 3] = (result & 4) ? 1.0f : -1.0f;
        return result;
    }
    if (!SphereBoxIntersection(pBounds, pEnd, 2.5e-5f, pPoint)) {
        return 0;
    }
    *pT = 1.0f;
    pNormal->v[0] = 0.0f;
    pNormal->v[1] = 0.0f;
    pNormal->v[2] = 0.0f;
    for (axis = 0; axis < 3; axis++) {
        if (pPoint->v[axis] == pBounds->max.v[axis]) {
            pNormal->v[axis] = -1.0f;
        } else if (pPoint->v[axis] == pBounds->min.v[axis]) {
            pNormal->v[axis] = 1.0f;
        }
    }
    x = pNormal->v[0];
    y = pNormal->v[1];
    z = pNormal->v[2];
    length = sqrt((y * y + z * z) + x * x);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        pNormal->v[0] = scale * pNormal->v[0];
        pNormal->v[1] = y * scale;
        pNormal->v[2] = z * scale;
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    return 0x40;
}


// FUNCTION: CARMA2_HW  0x00460f60
int C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_vector3 offset;
    br_vector3 direction;
    double x;
    double y;
    double z;
    double squared;
    float offset_squared;
    float direction_squared;
    double projection;
    double perpendicular;
    float t;
    double point_x;
    double point_y;
    double point_z;
    double length;
    double scale;

    x = (double)pSphere->center.v[0] - pStart->v[0];
    y = (double)pSphere->center.v[1] - pStart->v[1];
    z = (double)pSphere->center.v[2] - pStart->v[2];
    offset.v[0] = FinterayScalar(x);
    offset.v[1] = FinterayScalar(y);
    offset.v[2] = FinterayScalar(z);
    squared = (x * FinterayShapeLoadScalar(&offset.v[0]) + y * FinterayShapeLoadScalar(&offset.v[1])) + FinterayShapeLoadScalar(&offset.v[2]) * FinterayShapeLoadScalar(&offset.v[2]);
    offset_squared = FinterayScalar(squared);
    if (!(squared >= pSphere->radius_squared)) {
        return -1;
    }
    x = (double)pEnd->v[0] - pStart->v[0];
    y = (double)pEnd->v[1] - pStart->v[1];
    z = (double)pEnd->v[2] - pStart->v[2];
    direction.v[0] = FinterayScalar(x);
    direction.v[1] = FinterayScalar(y);
    direction.v[2] = FinterayScalar(z);
    squared = (x * FinterayShapeLoadScalar(&direction.v[0]) + y * FinterayShapeLoadScalar(&direction.v[1])) + FinterayShapeLoadScalar(&direction.v[2]) * FinterayShapeLoadScalar(&direction.v[2]);
    direction_squared = FinterayScalar(squared);
    if (!(squared >= 1e-6)) {
        return 0;
    }
    projection = ((FinterayShapeLoadScalar(&offset.v[0]) * FinterayShapeLoadScalar(&direction.v[0]) + FinterayShapeLoadScalar(&offset.v[1]) * FinterayShapeLoadScalar(&direction.v[1])) + FinterayShapeLoadScalar(&offset.v[2]) * FinterayShapeLoadScalar(&direction.v[2])) / FinterayShapeLoadScalar(&direction_squared);
    perpendicular = FinterayShapeLoadScalar(&offset_squared) - projection * projection * FinterayShapeLoadScalar(&direction_squared);
    if (perpendicular > pSphere->radius_squared) {
        return 0;
    }
    t = FinterayScalar(projection - sqrt(((double)pSphere->radius_squared - perpendicular) / FinterayShapeLoadScalar(&direction_squared)));
    FinterayBoxStoreScalar(pT, &t);
    if (!(t >= 0.0f) || t > 1.0f) {
        return 0;
    }
    point_x = FinterayShapeLoadScalar(&direction.v[0]) * t + pStart->v[0];
    point_y = FinterayShapeLoadScalar(&direction.v[1]) * t;
    point_z = FinterayShapeLoadScalar(&direction.v[2]) * t;
    pPoint->v[0] = FinterayScalar(point_x);
    pPoint->v[1] = FinterayScalar(point_y + pStart->v[1]);
    pPoint->v[2] = FinterayScalar(point_z + pStart->v[2]);
    if (pNormal != NULL) {
        pNormal->v[0] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[0]) - FinterayShapeLoadScalar(&pPoint->v[0]));
        pNormal->v[1] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[1]) - FinterayShapeLoadScalar(&pPoint->v[1]));
        z = FinterayShapeLoadScalar(&pSphere->center.v[2]) - FinterayShapeLoadScalar(&pPoint->v[2]);
        x = FinterayShapeLoadScalar(&pNormal->v[0]);
        y = FinterayShapeLoadScalar(&pNormal->v[1]);
        length = sqrt((x * x + y * y) + z * FinterayScalar(z));
        pNormal->v[2] = FinterayScalar(z);
        if (length > 2.3841858e-7f) {
            scale = 1.0 / length;
            y = FinterayShapeLoadScalar(&pNormal->v[1]);
            z = FinterayShapeLoadScalar(&pNormal->v[2]);
            pNormal->v[0] = FinterayScalar(scale * FinterayShapeLoadScalar(&pNormal->v[0]));
            pNormal->v[1] = FinterayScalar(y * scale);
            pNormal->v[2] = FinterayScalar(z * scale);
        } else {
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
        }
    }
    return 1;
}


// FUNCTION: CARMA2_HW  0x004611b0
int C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    double z;
    double squared;
    float squared_scalar;
    double length;
    double scale;
    double y;
    double normal_z;
    double point_y;
    double point_z;
    float point_x;

    result = LineSphereColl(pStart, pEnd, pSphere, pPoint, pNormal, pT);
    if (result != 0) {
        return result;
    }
    pNormal->v[0] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[0]) - pEnd->v[0]);
    pNormal->v[1] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[1]) - pEnd->v[1]);
    z = FinterayShapeLoadScalar(&pSphere->center.v[2]) - pEnd->v[2];
    squared = ((double)FinterayShapeLoadScalar(&pNormal->v[0]) * FinterayShapeLoadScalar(&pNormal->v[0]) + (double)FinterayShapeLoadScalar(&pNormal->v[1]) * FinterayShapeLoadScalar(&pNormal->v[1])) + z * FinterayScalar(z);
    pNormal->v[2] = FinterayScalar(z);
    squared_scalar = FinterayScalar(squared);
    if (squared > FinterayShapeLoadScalar(&pSphere->radius_squared) - (FinterayShapeLoadScalar(&pSphere->radius_squared) * 0.002f) * -2.0f) {
        return 0;
    }
    if (!(squared >= FinterayShapeLoadScalar(&pSphere->radius_squared))) {
        return 0;
    }
    length = sqrt((double)squared_scalar);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        y = FinterayShapeLoadScalar(&pNormal->v[1]);
        normal_z = FinterayShapeLoadScalar(&pNormal->v[2]);
        pNormal->v[0] = FinterayScalar(scale * FinterayShapeLoadScalar(&pNormal->v[0]));
        pNormal->v[1] = FinterayScalar(y * scale);
        pNormal->v[2] = FinterayScalar(normal_z * scale);
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    pPoint->v[0] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->radius) * FinterayShapeLoadScalar(&pNormal->v[0]));
    point_x = (br_scalar)FinterayShapeLoadScalar(&pPoint->v[0]);
    point_y = (double)FinterayShapeLoadScalar(&pNormal->v[1]) * FinterayShapeLoadScalar(&pSphere->radius);
    pPoint->v[1] = FinterayScalar(point_y);
    point_y = FinterayShapeLoadScalar(&pPoint->v[1]);
    point_z = (double)FinterayShapeLoadScalar(&pNormal->v[2]) * FinterayShapeLoadScalar(&pSphere->radius);
    pPoint->v[2] = FinterayScalar(point_z);
    point_z = FinterayShapeLoadScalar(&pPoint->v[2]);
    pPoint->v[0] = FinterayScalar((double)point_x + FinterayShapeLoadScalar(&pSphere->center.v[0]));
    pPoint->v[1] = FinterayScalar(point_y + FinterayShapeLoadScalar(&pSphere->center.v[1]));
    pPoint->v[2] = FinterayScalar(point_z + FinterayShapeLoadScalar(&pSphere->center.v[2]));
    *pT = 1.0f;
    return 2;
}


// FUNCTION: CARMA2_HW  0x00461310
int C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT) {
    br_vector3 direction;
    volatile br_scalar candidate[64];
    volatile br_scalar denominators[64];
    int count;
    int plane;
    int selected = -1;
    int inside = 1;
    br_vector4* equation;
    double denominator;
    double distance;
    double best_t = 0.0;
    double point_x;
    double point_y;
    double point_z;
    float dot;

    *pT = 2.0f;
    direction.v[0] = FinterayScalar((double)pEnd->v[0] - pStart->v[0]);
    count = pPoly->count_planes;
    direction.v[1] = FinterayScalar((double)pEnd->v[1] - pStart->v[1]);
    direction.v[2] = FinterayScalar((double)pEnd->v[2] - pStart->v[2]);
    equation = pPoly->planes;
    for (plane = 0; plane < count; plane++, equation++) {
        denominator = ((FinterayShapeLoadScalar(&equation->v[2]) * FinterayShapeLoadScalar(&direction.v[2])
            + FinterayShapeLoadScalar(&equation->v[1]) * FinterayShapeLoadScalar(&direction.v[1]))
            + FinterayShapeLoadScalar(&equation->v[0]) * FinterayShapeLoadScalar(&direction.v[0]));
        denominators[plane] = FinterayScalar(denominator);
        dot = FinterayScalar(((FinterayShapeLoadScalar(&equation->v[0]) * pStart->v[0]
            + (double)pStart->v[1] * equation->v[1]) + FinterayShapeLoadScalar(&equation->v[2]) * pStart->v[2]) + pRadius);
        distance = FinterayShapeLoadScalar(&equation->v[3]) + FinterayShapeLoadScalar(&dot);
        if (!(distance >= 0.0)) {
            inside = 0;
        }
        if (denominator == 0.0 || FinterayBoxNaN(denominator)) {
            if (!(distance >= 0.0)) {
                return 0;
            }
        } else {
            candidate[plane] = FinterayScalar(-(distance / denominator));
        }
        if (denominator > 0.0 && !(best_t > candidate[plane])) {
            if (candidate[plane] > 1.0f) {
                return 0;
            }
            best_t = candidate[plane];
            selected = plane;
        }
    }
    if (inside) {
        return -1;
    }
    if (selected < 0) {
        return 0;
    }
    for (plane = 0; plane < count; plane++) {
        if (!(denominators[plane] >= 0.0f) && best_t > candidate[plane]) {
            return 0;
        }
    }
    point_x = best_t * FinterayShapeLoadScalar(&direction.v[0]);
    point_y = best_t * FinterayShapeLoadScalar(&direction.v[1]);
    point_z = best_t * FinterayShapeLoadScalar(&direction.v[2]);
    pPoint->v[0] = FinterayScalar(point_x);
    pPoint->v[1] = FinterayScalar(point_y);
    pPoint->v[2] = FinterayScalar(point_z);
    pPoint->v[0] = FinterayScalar((double)pStart->v[0] + point_x);
    pPoint->v[1] = FinterayScalar(point_y + pStart->v[1]);
    pPoint->v[2] = FinterayScalar(point_z + pStart->v[2]);
    *pT = FinterayScalar(best_t);
    return selected + 1;
}


// FUNCTION: CARMA2_HW  0x00461520
int C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_scalar distance[64];
    tCollision_shape_polyhedron_data* poly = &pShape->polyhedron;
    br_vector4* equation;
    int result;
    int count;
    int plane;
    int selected;
    double extended;
    double x;
    double y;
    double z;
    double length;
    double scale;

    result = LinePolyColl(pStart, pEnd, poly, pPoint, 0.0f, pT);
    if (result < 0) {
        BrVector3SetFloat(pNormal, 1.0f, 0.0f, 0.0f);
        return result;
    }
    if (result != 0) {
        selected = (result & 0x3f) - 1;
        FinterayBoxCopyScalar(&pNormal->v[0], &FinterayShapePlanes(poly)[selected].v[0]);
        FinterayBoxCopyScalar(&pNormal->v[1], &FinterayShapePlanes(poly)[selected].v[1]);
        FinterayBoxCopyScalar(&pNormal->v[2], &FinterayShapePlanes(poly)[selected].v[2]);
        return result;
    }
    count = FinterayShapePlaneCount(poly);
    equation = FinterayShapePlanes(poly);
    for (plane = 0; plane < count; plane++, equation++) {
        extended = ((FinterayShapeLoadScalar(&equation->v[0]) * pEnd->v[0]
            + (double)pEnd->v[2] * equation->v[2]) + FinterayShapeLoadScalar(&equation->v[1]) * pEnd->v[1]) + equation->v[3];
        distance[plane] = FinterayScalar(extended);
        if (!(extended >= -0.005f)) {
            return 0;
        }
    }
    pNormal->v[0] = 0.0f;
    pNormal->v[1] = 0.0f;
    pNormal->v[2] = 0.0f;
    selected = 0;
    for (plane = 0; plane < FinterayShapePlaneCount(poly); plane++) {
        if (!(FinterayShapeLoadScalar(&distance[plane]) >= 0.0)) {
            selected++;
            pNormal->v[0] = FinterayScalar(FinterayShapeLoadScalar(&FinterayShapePlanes(poly)[plane].v[0]) + pNormal->v[0]);
            pNormal->v[1] = FinterayScalar(FinterayShapeLoadScalar(&FinterayShapePlanes(poly)[plane].v[1]) + pNormal->v[1]);
            pNormal->v[2] = FinterayScalar(FinterayShapeLoadScalar(&FinterayShapePlanes(poly)[plane].v[2]) + pNormal->v[2]);
        }
    }
    if (selected > 1) {
        x = FinterayShapeLoadScalar(&pNormal->v[0]);
        y = FinterayShapeLoadScalar(&pNormal->v[1]);
        z = FinterayShapeLoadScalar(&pNormal->v[2]);
        length = sqrt((y * y + z * z) + x * x);
        if (length > 2.3841858e-7f) {
            scale = 1.0 / length;
            pNormal->v[0] = FinterayScalar(scale * FinterayShapeLoadScalar(&pNormal->v[0]));
            pNormal->v[1] = FinterayScalar(y * scale);
            pNormal->v[2] = FinterayScalar(z * scale);
        } else {
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
        }
    }
    FinterayBoxCopyScalar(&pPoint->v[0], &pEnd->v[0]);
    FinterayBoxCopyScalar(&pPoint->v[1], &pEnd->v[1]);
    FinterayBoxCopyScalar(&pPoint->v[2], &pEnd->v[2]);
    *pT = 1.0f;
    return 0x40;
}


// FUNCTION: CARMA2_HW 0x004618e0
tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pCount, br_scalar pOffset, br_vector3* pCollision_position) {
    br_vector3 direction;
    br_vector3 position;
    br_vector3 normal;
    br_scalar t;
    tFace_ref* face;

    BrVector3Set(&direction, 0.f, -1.f, 0.f);
    face = RayCastThruFaceList(pPosition, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    if (face == NULL && pOffset != 0.f) {
        BrVector3Set(&position, pPosition->v[0], pPosition->v[1] + pOffset, pPosition->v[2]);
        face = RayCastThruFaceList(&position, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    }
    return face;
}

// FUNCTION: CARMA2_HW  0x00461850
int C2_HOOK_FASTCALL ShapeRayCastWithSphere(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        return LineBoxCollWithSphere(pStart, pEnd, &pShape->common.bb, pPoint, pNormal, pT);
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        return LinePolyCollWithSphere(pStart, pEnd, &pShape->polyhedron, pPoint, pNormal, pT);
    case kCollisionShapeType_Sphere:
        return LineSphereCollWithSphere(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
    case kCollisionShapeType_Wireframe:
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00460bd0
int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT) {
    br_vector3 direction;
    br_vector3 plane;
    br_vector3 candidate;
    int quadrant[3];
    int inside = 1;
    int axis;
    int selected = 0;
    double coordinate;

    for (axis = 0; axis < 3; axis++) {
        direction.v[axis] = FinterayScalar((double)pEnd->v[axis] - pStart->v[axis]);
    }
    for (axis = 0; axis < 3; axis++) {
        if (!(pStart->v[axis] >= pBounds->min.v[axis])) {
            quadrant[axis] = 1;
            FinterayBoxCopyScalar(&plane.v[axis], &pBounds->min.v[axis]);
            inside = 0;
        } else if (pStart->v[axis] > pBounds->max.v[axis]) {
            quadrant[axis] = 0;
            FinterayBoxCopyScalar(&plane.v[axis], &pBounds->max.v[axis]);
            inside = 0;
        } else {
            quadrant[axis] = 2;
        }
    }
    if (inside) {
        for (axis = 0; axis < 3; axis++) {
            FinterayBoxCopyScalar(&pPoint->v[axis], &pStart->v[axis]);
        }
        return -1;
    }
    for (axis = 0; axis < 3; axis++) {
        if (quadrant[axis] == 2 || direction.v[axis] == 0.0f || FinterayBoxNaN(direction.v[axis])) {
            candidate.v[axis] = -1.0f;
        } else {
            candidate.v[axis] = FinterayScalar(((double)plane.v[axis] - pStart->v[axis]) / direction.v[axis]);
        }
    }
    for (axis = 1; axis < 3; axis++) {
        if (candidate.v[axis] > candidate.v[selected]) {
            selected = axis;
        }
    }
    if (!(candidate.v[selected] >= 0.0f) || candidate.v[selected] > 1.0f) {
        return 0;
    }
    for (axis = 0; axis < 3; axis++) {
        if (axis == selected) {
            FinterayBoxCopyScalar(&pPoint->v[axis], &plane.v[axis]);
        } else {
            coordinate = (double)direction.v[axis] * candidate.v[selected] + pStart->v[axis];
            pPoint->v[axis] = FinterayScalar(coordinate);
            if (!(coordinate >= pBounds->min.v[axis]) || coordinate > pBounds->max.v[axis]) {
                return 0;
            }
        }
    }
    FinterayBoxCopyScalar(pT, &candidate.v[selected]);
    return selected + 1 + 4 * quadrant[selected];
}

// FUNCTION: CARMA2_HW 0x0041e360
void C2_HOOK_FASTCALL FUN_0041e360(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pNor, br_scalar* pT) {
    br_vector3 scaled_ray_pos;
    br_vector3 scaled_ray_dir;
    br_material* material;

    scaled_ray_pos.v[1] = pRay_pos->v[1] * gFloat_005896d4;
    scaled_ray_pos.v[2] = pRay_pos->v[2] * gFloat_005896d4;
    scaled_ray_pos.v[0] = pRay_pos->v[0] * gFloat_005896d4;
    scaled_ray_dir.v[0] = pRay_dir->v[0] * gFloat_005896d4;
    scaled_ray_dir.v[1] = pRay_dir->v[1] * gFloat_005896d4;
    scaled_ray_dir.v[2] = pRay_dir->v[2] * gFloat_005896d4;
    ActorFindFace(&scaled_ray_pos, &scaled_ray_dir, gTrack_actor, pNor, pT, &material, NULL);
}

// FUNCTION: CARMA2_HW 0x0041e3f0
void C2_HOOK_FASTCALL FUN_0041e3f0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, void* pRay_context) {
    br_vector3 scaled_ray_pos;
    br_vector3 scaled_ray_dir;

    scaled_ray_pos.v[0] = pRay_pos->v[0] * gFloat_005896d4;
    scaled_ray_dir.v[0] = pRay_dir->v[0] * gFloat_005896d4;
    scaled_ray_pos.v[1] = pRay_pos->v[1] * gFloat_005896d4;
    scaled_ray_pos.v[2] = pRay_pos->v[2] * gFloat_005896d4;
    scaled_ray_dir.v[1] = pRay_dir->v[1] * gFloat_005896d4;
    scaled_ray_dir.v[2] = pRay_dir->v[2] * gFloat_005896d4;
    RayCastInBox(&scaled_ray_pos, &scaled_ray_dir, pHit_pos, pT, *(tPhysics_object**)((tU8*)pRay_context + 8));
}

// FUNCTION: CARMA2_HW 0x004227a0
tFace_ref* C2_HOOK_FASTCALL FUN_004227a0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pBest_t, br_vector3* pHit_pos, tFace_ref** pFaces, int pFace_count) {
	br_vector3 hit_normal;
	br_vector3 hit_pos;
	br_scalar t;
	br_vector3* output_pos;
	int face_count;

	*pBest_t = 2.0f;
	if (pFace_count > 0) {
		face_count = pFace_count;
		output_pos = pHit_pos;
		do {
			CheckSingleFace(*pFaces, pRay_pos, pRay_dir, &hit_normal, &t, &hit_pos);
			if (t < *pBest_t) {
				*pBest_t = t;
				pHit_pos = (br_vector3*)*pFaces;
				pHit_normal->v[0] = hit_normal.v[0];
				pHit_normal->v[1] = hit_normal.v[1];
				pHit_normal->v[2] = hit_normal.v[2];
				output_pos->v[0] = hit_pos.v[0];
				output_pos->v[1] = hit_pos.v[1];
				output_pos->v[2] = hit_pos.v[2];
			}
			pFaces++;
			face_count--;
		} while (face_count != 0);
	}
	if (*pBest_t < gFLOAT_00589968) {
		return (tFace_ref*)pHit_pos;
	}
	return NULL;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00425ca0
int C2_HOOK_FASTCALL FUN_00425ca0(tPhysics_object* pObject1, tPhysics_object* pObject2, void* pRecords, int pRecord_count) {
    tU16 point_counts1[66];
    tU16 point_counts2[18];
    br_vector3 vertices1[128][2];
    br_vector3 vertices2[128][2];
    br_matrix34 inverse_object1;
    br_matrix34 object2_to_object1;
    br_matrix34 inverse_actor1;
    br_matrix34 actor2_to_actor1;
    br_vector3 point;
    br_vector3 translated_point;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    br_vector3* vertices1_for_shape;
    br_vector3* vertices2_for_shape;
    tU16* point_count1;
    tU16* point_count2;
    tU16* point_count1_for_shape;
    tU16* point_count2_for_shape;
    int vertex_count1;
    int vertex_count2;
    int point_count;
    int i;
    int contact_count;
    float collision_factor;


    if (pRecord_count > 0x24) {
        return 0;
    }

    BrMatrix34LPInverse(&inverse_object1, &pObject1->transform_matrix);
    BrMatrix34Mul(&object2_to_object1, &pObject2->transform_matrix, &inverse_object1);
    BrMatrix34LPInverse(&inverse_actor1, &pObject1->actor->t.t.mat);
    BrMatrix34Mul(&actor2_to_actor1, &pObject2->actor->t.t.mat, &inverse_actor1);

    /* Object 2 vertices are expressed in object 1 space; object 1 vertices
     * use the inverse relative transform to enter object 2 space. */
    point_count2 = point_counts2;
    vertices2_for_shape = &vertices2[0][0];
    for (shape2 = pObject2->shape; shape2 != NULL; shape2 = shape2->common.next) {
        switch (shape2->common.type) {
        case kCollisionShapeType_Box:
            point_count = 8;
            for (i = 0; i < 8; i++) {
                point.v[0] = (i & 2) ? shape2->box.common.bb.min.v[0] : shape2->box.common.bb.max.v[0];
                point.v[1] = (i & 1) ? shape2->box.common.bb.min.v[1] : shape2->box.common.bb.max.v[1];
                point.v[2] = (i & 4) ? shape2->box.common.bb.min.v[2] : shape2->box.common.bb.max.v[2];
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2], &point, &object2_to_object1);
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2 + 1], &point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            point_count = shape2->polyhedron.polyhedron.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape2->polyhedron.polyhedron.points[i];
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2], &point, &object2_to_object1);
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2 + 1], &point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Wireframe:
            point_count = shape2->wireframe.wireframe.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape2->wireframe.wireframe.points[i];
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2], &point, &object2_to_object1);
                BrMatrix34ApplyP(&vertices2_for_shape[i * 2 + 1], &point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Sphere:
            point_count = 1;
            point = shape2->sphere.sphere.center;
            BrMatrix34ApplyP(&vertices2_for_shape[0], &point, &object2_to_object1);
            BrMatrix34ApplyP(&vertices2_for_shape[1], &point, &actor2_to_actor1);
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, NULL);
            point_count = 0;
            break;
        }
        *point_count2++ = (tU16)point_count;
        vertices2_for_shape += point_count * 2;
    }

    point_count1 = point_counts1;
    vertices1_for_shape = &vertices1[0][0];
    for (shape1 = pObject1->shape; shape1 != NULL; shape1 = shape1->common.next) {
        switch (shape1->common.type) {
        case kCollisionShapeType_Box:
            point_count = 8;
            for (i = 0; i < 8; i++) {
                point.v[0] = (i & 2) ? shape1->box.common.bb.min.v[0] : shape1->box.common.bb.max.v[0];
                point.v[1] = (i & 1) ? shape1->box.common.bb.min.v[1] : shape1->box.common.bb.max.v[1];
                point.v[2] = (i & 4) ? shape1->box.common.bb.min.v[2] : shape1->box.common.bb.max.v[2];
                translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
                translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
                translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2], &translated_point, &object2_to_object1);
                translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
                translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
                translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2 + 1], &translated_point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            point_count = shape1->polyhedron.polyhedron.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape1->polyhedron.polyhedron.points[i];
                translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
                translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
                translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2], &translated_point, &object2_to_object1);
                translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
                translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
                translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2 + 1], &translated_point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Wireframe:
            point_count = shape1->wireframe.wireframe.count_points;
            for (i = 0; i < point_count; i++) {
                point = shape1->wireframe.wireframe.points[i];
                translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
                translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
                translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2], &translated_point, &object2_to_object1);
                translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
                translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
                translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
                BrMatrix34TApplyV(&vertices1_for_shape[i * 2 + 1], &translated_point, &actor2_to_actor1);
            }
            break;
        case kCollisionShapeType_Sphere:
            point_count = 1;
            point = shape1->sphere.sphere.center;
            translated_point.v[0] = point.v[0] - object2_to_object1.m[3][0];
            translated_point.v[1] = point.v[1] - object2_to_object1.m[3][1];
            translated_point.v[2] = point.v[2] - object2_to_object1.m[3][2];
            BrMatrix34TApplyV(&vertices1_for_shape[0], &translated_point, &object2_to_object1);
            translated_point.v[0] = point.v[0] - actor2_to_actor1.m[3][0];
            translated_point.v[1] = point.v[1] - actor2_to_actor1.m[3][1];
            translated_point.v[2] = point.v[2] - actor2_to_actor1.m[3][2];
            BrMatrix34TApplyV(&vertices1_for_shape[1], &translated_point, &actor2_to_actor1);
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, NULL);
            point_count = 0;
            break;
        }
        *point_count1++ = (tU16)point_count;
        vertices1_for_shape += point_count * 2;
    }

    contact_count = 0;
    point_count1_for_shape = point_counts1;
    vertices1_for_shape = &vertices1[0][0];
    for (shape1 = pObject1->shape; shape1 != NULL; shape1 = shape1->common.next) {
        vertex_count1 = *point_count1_for_shape;
        point_count2_for_shape = point_counts2;
        vertices2_for_shape = &vertices2[0][0];
        for (shape2 = pObject2->shape; shape2 != NULL; shape2 = shape2->common.next) {
            vertex_count2 = *point_count2_for_shape;
            if (shape1->common.type == kCollisionShapeType_Sphere) {
                if (shape2->common.type == kCollisionShapeType_Sphere) {
                    collision_factor = 100.0f;
                    FUN_0045d5e0(&shape1->sphere.sphere, &shape2->sphere.sphere,
                            &vertices2_for_shape[0], &vertices2_for_shape[1],
                            &point, &collision_factor);
                    if (collision_factor <= gFLOAT_0058996c) {
                        br_vector3 contact1;
                        br_vector3 contact2;
                        br_vector3 normal_start;
                        br_vector3 normal_end;
                        br_vector3 world_normal;
                        br_vector3 local_normal_start;
                        br_vector3 local_normal_end;
                        br_vector3 local_normal;
                        float remaining;

                        /* Contact positions stay in their owner's local space. The normal
                         * is interpolated between the physics and actor orientations. */
                        BrVector3Scale(&contact1, &point, -shape1->sphere.sphere.radius);
                        BrVector3Accumulate(&contact1, &shape1->sphere.sphere.center);
                        BrMatrix34ApplyV(&normal_start, &point, &pObject1->transform_matrix);
                        BrMatrix34ApplyV(&normal_end, &point, &pObject1->actor->t.t.mat);
                        remaining = gFLOAT_0058996c - collision_factor;
                        for (i = 0; i < 3; i++) {
                            normal_start.v[i] *= remaining;
                            normal_end.v[i] *= collision_factor;
                            world_normal.v[i] = normal_start.v[i] + normal_end.v[i];
                        }
                        BrMatrix34TApplyV(&local_normal_start, &world_normal, &pObject2->transform_matrix);
                        BrMatrix34TApplyV(&local_normal_end, &world_normal, &pObject2->actor->t.t.mat);
                        for (i = 0; i < 3; i++) {
                            local_normal_start.v[i] *= remaining;
                            local_normal_end.v[i] *= collision_factor;
                            local_normal.v[i] = local_normal_start.v[i] + local_normal_end.v[i];
                        }
                        BrVector3Scale(&contact2, &local_normal, shape2->sphere.sphere.radius);
                        BrVector3Accumulate(&contact2, &shape2->sphere.sphere.center);
                        contact_count = AddObjectObjectForce((float*)((tU8*)pRecords + pRecord_count * 0x60),
                                &contact1, collision_factor, &contact2, &world_normal,
                                contact_count, pObject1, pObject2, 7, NULL, 0.0f);
                    }
                } else {
                    contact_count = FUN_004269c0(pObject1, pObject2, shape1, shape2,
                            vertices1_for_shape, vertices2_for_shape,
                            vertex_count1, vertex_count2,
                            (tU8*)pRecords + pRecord_count * 0x60,
                            contact_count, 0x28 - pRecord_count);
                }
            } else if (shape2->common.type == kCollisionShapeType_Sphere) {
                contact_count = FUN_004269c0(pObject2, pObject1, shape2, shape1,
                        vertices2_for_shape, vertices1_for_shape,
                        vertex_count2, vertex_count1,
                        (tU8*)pRecords + pRecord_count * 0x60,
                        contact_count, 0x28 - pRecord_count);
            } else {
                contact_count = FUN_00427870(pObject1, pObject2, shape1, shape2,
                        vertices1_for_shape, vertices2_for_shape,
                        vertex_count1, vertex_count2,
                        (tU8*)pRecords + pRecord_count * 0x60,
                        contact_count, 0x28 - pRecord_count);
            }
            vertices2_for_shape += vertex_count2 * 2;
            point_count2_for_shape++;
        }
        vertices1_for_shape += vertex_count1 * 2;
        point_count1_for_shape++;
    }

    if (contact_count != 0) {
        float* record;
        float normal_dot;
        int result;

        pObject1->omega = pObject1->rotate_omega;
        pObject2->omega = pObject2->rotate_omega;
        result = 1;
        if (contact_count > 1) {
            record = (float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x94);
            do {
                if (*(tPhysics_object**)(record - 12) == *(tPhysics_object**)((tU8*)pRecords + pRecord_count * 0x60 + 4)) {
                    normal_dot = record[0] * *(float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x34)
                            + record[-1] * *(float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x30)
                            + record[-2] * *(float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x2c);
                } else {
                    normal_dot = -(record[0] * *(float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x40)
                            + record[-1] * *(float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x3c)
                            + record[-2] * *(float*)((tU8*)pRecords + pRecord_count * 0x60 + 0x38));
                }
                if (normal_dot < (float)gDouble_00589a08) {
                    return result;
                }
                result++;
                record += 0x18;
            } while (result < contact_count);
        }
        return result;
    }
    return 0;
}

extern br_scalar gFloat_005896d4;
extern const br_scalar gFLOAT_00589968;
extern const br_scalar gFLOAT_0058996c;
extern const br_scalar gFLOAT_00589978;
extern const br_scalar gFLOAT_005899a8;
extern const double gDouble_005899b0;

#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004269c0
int C2_HOOK_FASTCALL FUN_004269c0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, void* pRecords, int pRecord_count, int pRemaining) {
    tU8 outcodes[64];
    tU8 outcodes_and;
    br_vector3 box_points[8];
    const br_vector3* points;
    const tPolyhedron_edge_indexes* edges;
    int edge_count;
    int i;
    int hit;
    int bit0;
    int bit1;
    int bit2;
    int bit3;
    int bit4;
    int bit5;
    tU8 code;
    tU8 code1;
    tU8 code2;
    const float* vertex;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float factor;
    float weight;
    float length;
    float scale;
    float dot;
    br_vector3 hit_normal;
    br_vector3 hit_position;
    br_vector3 normal;
    br_vector3 blend;
    br_vector3 sphere_point;
    br_vector3 relative;
    br_vector3 temp1;
    br_vector3 temp2;
    br_vector3 temp3;
    br_vector3 temp4;
    br_vector3 local_b;
    br_vector3 local_c;
    br_vector3 local_d;
    br_vector3 local_e;
    br_vector3 local_f;
    br_vector3 local_g;
    br_vector3 local_h;
    br_vector3 local_i;
    br_vector3 local_j;
    br_vector3 local_k;
    br_vector3 local_l;

    (void)pCount1;
    (void)pRemaining;
    i = 0;
    outcodes_and = 0xff;
    if (pCount2 > 0) {
        fVar1 = pShape1->common.bb.min.v[0] - gFLOAT_00589a10;
        fVar2 = pShape1->common.bb.min.v[1] - gFLOAT_00589a10;
        fVar3 = pShape1->common.bb.min.v[2] - gFLOAT_00589a10;
        fVar4 = pShape1->common.bb.max.v[0] - gFLOAT_00589a14;
        fVar5 = pShape1->common.bb.max.v[1] - gFLOAT_00589a14;
        fVar6 = pShape1->common.bb.max.v[2] - gFLOAT_00589a14;
        vertex = (const float*)((const tU8*)pVertices2 + 4);
        do {
            bit0 = vertex[-1] < fVar1 && vertex[2] < fVar1;
            bit1 = vertex[0] < fVar2 && vertex[3] < fVar2;
            bit2 = vertex[1] < fVar3 && vertex[4] < fVar3;
            bit3 = vertex[-1] > fVar4 && vertex[2] > fVar4;
            bit4 = vertex[0] > fVar5 && vertex[3] > fVar5;
            bit5 = vertex[1] > fVar6 && vertex[4] > fVar6;
            code = (((((bit5 * 2 + bit4) * 2 + bit3) * 2 + bit2) * 2 + bit1) * 2 + bit0);
            vertex += 6;
            outcodes[i] = code;
            outcodes_and &= code;
            i++;
        } while (i < pCount2);
    }
    if (outcodes_and != 0) {
        return pRecord_count;
    }

    points = NULL;
    switch (pShape2->common.type) {
    case kCollisionShapeType_Box:
        for (i = 0; i < 8; i++) {
            box_points[i].v[0] = (i & 2) ? pShape2->common.bb.min.v[0] : pShape2->common.bb.max.v[0];
            box_points[i].v[1] = (i & 1) ? pShape2->common.bb.min.v[1] : pShape2->common.bb.max.v[1];
            box_points[i].v[2] = (i & 4) ? pShape2->common.bb.min.v[2] : pShape2->common.bb.max.v[2];
        }
        points = box_points;
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        points = pShape2->polyhedron.polyhedron.points;
        break;
    case kCollisionShapeType_Wireframe:
        points = pShape2->wireframe.wireframe.points;
        break;
    default:
        break;
    }

    for (i = 0; i < pCount2; i++) {
        if (outcodes[i] != 0) {
            continue;
        }
        hit = LineSphereCollWithSphere(&pVertices2[i * 2], &pVertices2[i * 2 + 1], &pShape1->sphere.sphere, &hit_position, &hit_normal, &factor);
        if (hit == 0) {
            continue;
        }
        if (hit == -1) {
            outcodes[i] = 0xff;
            continue;
        }
        BrMatrix34ApplyV(&normal, &hit_normal, &pObject1->actor->t.t.mat);
        BrMatrix34ApplyV(&blend, &hit_normal, &pObject1->transform_matrix);
        weight = gFLOAT_0058996c - factor;
        blend.v[0] = blend.v[0] * weight;
        blend.v[1] = blend.v[1] * weight;
        blend.v[2] = blend.v[2] * weight;
        fVar1 = factor * normal.v[0] + blend.v[0];
        fVar2 = factor * normal.v[1] + blend.v[1];
        fVar3 = factor * normal.v[2] + blend.v[2];
        length = sqrtf(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
        if (length > gFLOAT_005899a8) {
            scale = (float)(gDouble_005899b0 / length);
            normal.v[0] = scale * fVar1;
            normal.v[1] = scale * fVar2;
            normal.v[2] = scale * fVar3;
        } else {
            normal.v[0] = 1.0f;
            normal.v[1] = 0.0f;
            normal.v[2] = 0.0f;
        }
        pRecord_count = AddObjectObjectForce((float*)pRecords, &hit_position, factor, (br_vector3*)&points[i], &normal, pRecord_count, pObject1, pObject2, 5, NULL, 0.0f);
    }

    switch (pShape2->common.type) {
    case kCollisionShapeType_Box:
        FUN_0045e4e0(&pShape1->sphere.sphere.radius, (float*)pShape2, (float*)&pVertices1[0], (float*)&pVertices1[1], hit_position.v, hit_normal.v, &factor);
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        FUN_0045e200(&pShape1->sphere.sphere.radius, &pShape2->polyhedron.polyhedron, &pVertices1[0], &pVertices1[1], &hit_position, &hit_normal, &factor);
        break;
    default:
        factor = 100.0f;
        break;
    }

    if (factor <= gFLOAT_0058996c) {
        BrMatrix34ApplyV(&normal, &hit_normal, &pObject2->actor->t.t.mat);
        BrMatrix34ApplyV(&blend, &hit_normal, &pObject2->transform_matrix);
        weight = gFLOAT_0058996c - factor;
        blend.v[0] = blend.v[0] * weight;
        blend.v[1] = blend.v[1] * weight;
        blend.v[2] = blend.v[2] * weight;
        fVar1 = factor * normal.v[0] + blend.v[0];
        fVar2 = factor * normal.v[1] + blend.v[1];
        fVar3 = factor * normal.v[2] + blend.v[2];
        length = sqrtf(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
        if (length > gFLOAT_005899a8) {
            scale = (float)(gDouble_005899b0 / length);
            normal.v[0] = scale * fVar1;
            normal.v[1] = scale * fVar2;
            normal.v[2] = scale * fVar3;
        } else {
            normal.v[0] = 1.0f;
            normal.v[1] = 0.0f;
            normal.v[2] = 0.0f;
        }
        BrMatrix34TApplyV(&blend, &normal, &pObject1->transform_matrix);
        BrMatrix34TApplyV(&sphere_point, &normal, &pObject1->actor->t.t.mat);
        weight = gFLOAT_0058996c - factor;
        blend.v[0] = blend.v[0] * weight;
        blend.v[1] = blend.v[1] * weight;
        blend.v[2] = blend.v[2] * weight;
        fVar1 = factor * sphere_point.v[0] + blend.v[0];
        fVar2 = factor * sphere_point.v[1] + blend.v[1];
        fVar3 = factor * sphere_point.v[2] + blend.v[2];
        length = sqrtf(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
        if (length > gFLOAT_005899a8) {
            scale = (float)(gDouble_005899b0 / length);
            fVar1 = scale * fVar1;
            fVar2 = scale * fVar2;
            fVar3 = scale * fVar3;
        } else {
            fVar1 = gFLOAT_0058996c;
            fVar2 = gFLOAT_00589978;
            fVar3 = gFLOAT_00589978;
        }
        sphere_point.v[0] = pShape1->sphere.sphere.radius * fVar1 + pShape1->sphere.sphere.center.v[0];
        sphere_point.v[1] = pShape1->sphere.sphere.center.v[1] + pShape1->sphere.sphere.radius * fVar2;
        sphere_point.v[2] = pShape1->sphere.sphere.center.v[2] + pShape1->sphere.sphere.radius * fVar3;
        BrMatrix34ApplyP(&blend, &pShape1->sphere.sphere.center, &pObject1->transform_matrix);
        relative.v[0] = blend.v[0] - pObject2->transform_matrix.m[3][0];
        relative.v[1] = blend.v[1] - pObject2->transform_matrix.m[3][1];
        relative.v[2] = blend.v[2] - pObject2->transform_matrix.m[3][2];
        BrMatrix34TApplyV(&local_b, &relative, &pObject2->transform_matrix);
        BrMatrix34ApplyP(&blend, &pShape1->sphere.sphere.center, &pObject1->actor->t.t.mat);
        relative.v[0] = blend.v[0] - pObject2->actor->t.t.mat.m[3][0];
        relative.v[1] = blend.v[1] - pObject2->actor->t.t.mat.m[3][1];
        relative.v[2] = blend.v[2] - pObject2->actor->t.t.mat.m[3][2];
        BrMatrix34TApplyV(&local_c, &relative, &pObject2->actor->t.t.mat);
        local_c.v[0] = local_c.v[0] - local_b.v[0];
        local_c.v[1] = local_c.v[1] - local_b.v[1];
        local_c.v[2] = local_c.v[2] - local_b.v[2];
        dot = local_c.v[0] * hit_normal.v[0] + local_c.v[1] * hit_normal.v[1] + local_c.v[2] * hit_normal.v[2];
        pRecord_count = AddObjectObjectForce((float*)pRecords, &hit_position, factor, &sphere_point, &normal, pRecord_count, pObject2, pObject1, 8, NULL, dot);
    }

    switch (pShape2->common.type) {
    case kCollisionShapeType_Box:
        edges = gBox_edges_0058f7f0;
        edge_count = 12;
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        edges = pShape2->polyhedron.polyhedron.edges;
        edge_count = pShape2->polyhedron.polyhedron.count_edges;
        break;
    case kCollisionShapeType_Wireframe:
        edges = pShape2->wireframe.wireframe.lines;
        edge_count = pShape2->wireframe.wireframe.count_lines;
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, gEmpty_string_00676920);
        edges = NULL;
        edge_count = 0;
        break;
    }

    for (; edge_count > 0; edge_count--, edges++) {
        code1 = outcodes[edges->index1];
        code2 = outcodes[edges->index2];
        if ((code1 & code2 & 0x3f) != 0) {
            continue;
        }
        code1 |= 0x80;
        outcodes[edges->index1] = code1;
        code2 |= 0x80;
        outcodes[edges->index2] = code2;
        FUN_0045d920((br_scalar*)&points[edges->index1], (br_scalar*)&points[edges->index2], (br_scalar*)&pVertices1[0], (br_scalar*)&pVertices1[1], &pShape1->sphere.sphere.radius, hit_position.v, hit_normal.v, &factor, code1, code2);
        if (factor > gFLOAT_0058996c) {
            continue;
        }
        BrMatrix34ApplyV(&normal, &hit_normal, &pObject2->actor->t.t.mat);
        BrMatrix34ApplyV(&blend, &hit_normal, &pObject2->transform_matrix);
        weight = gFLOAT_0058996c - factor;
        blend.v[0] = blend.v[0] * weight;
        blend.v[1] = blend.v[1] * weight;
        blend.v[2] = blend.v[2] * weight;
        fVar1 = factor * normal.v[0] + blend.v[0];
        fVar2 = factor * normal.v[1] + blend.v[1];
        fVar3 = factor * normal.v[2] + blend.v[2];
        length = sqrtf(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
        if (length > gFLOAT_005899a8) {
            scale = (float)(gDouble_005899b0 / length);
            normal.v[0] = scale * fVar1;
            normal.v[1] = scale * fVar2;
            normal.v[2] = scale * fVar3;
        } else {
            normal.v[0] = 1.0f;
            normal.v[1] = 0.0f;
            normal.v[2] = 0.0f;
        }
        BrMatrix34TApplyV(&blend, &normal, &pObject1->transform_matrix);
        BrMatrix34TApplyV(&sphere_point, &normal, &pObject1->actor->t.t.mat);
        weight = gFLOAT_0058996c - factor;
        blend.v[0] = blend.v[0] * weight;
        blend.v[1] = blend.v[1] * weight;
        blend.v[2] = blend.v[2] * weight;
        fVar1 = factor * sphere_point.v[0] + blend.v[0];
        fVar2 = factor * sphere_point.v[1] + blend.v[1];
        fVar3 = factor * sphere_point.v[2] + blend.v[2];
        length = sqrtf(fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1);
        if (length > gFLOAT_005899a8) {
            scale = (float)(gDouble_005899b0 / length);
            fVar1 = scale * fVar1;
            fVar2 = scale * fVar2;
            fVar3 = scale * fVar3;
        } else {
            fVar1 = gFLOAT_0058996c;
            fVar2 = gFLOAT_00589978;
            fVar3 = gFLOAT_00589978;
        }
        sphere_point.v[0] = pShape1->sphere.sphere.radius * fVar1 + pShape1->sphere.sphere.center.v[0];
        sphere_point.v[1] = pShape1->sphere.sphere.center.v[1] + pShape1->sphere.sphere.radius * fVar2;
        sphere_point.v[2] = pShape1->sphere.sphere.center.v[2] + pShape1->sphere.sphere.radius * fVar3;
        BrMatrix34ApplyP(&blend, &pShape1->sphere.sphere.center, &pObject1->transform_matrix);
        relative.v[0] = blend.v[0] - pObject2->transform_matrix.m[3][0];
        relative.v[1] = blend.v[1] - pObject2->transform_matrix.m[3][1];
        relative.v[2] = blend.v[2] - pObject2->transform_matrix.m[3][2];
        BrMatrix34TApplyV(&local_b, &relative, &pObject2->transform_matrix);
        BrMatrix34ApplyP(&blend, &pShape1->sphere.sphere.center, &pObject1->actor->t.t.mat);
        relative.v[0] = blend.v[0] - pObject2->actor->t.t.mat.m[3][0];
        relative.v[1] = blend.v[1] - pObject2->actor->t.t.mat.m[3][1];
        relative.v[2] = blend.v[2] - pObject2->actor->t.t.mat.m[3][2];
        BrMatrix34TApplyV(&local_c, &relative, &pObject2->actor->t.t.mat);
        local_c.v[0] = local_c.v[0] - local_b.v[0];
        local_c.v[1] = local_c.v[1] - local_b.v[1];
        local_c.v[2] = local_c.v[2] - local_b.v[2];
        dot = local_c.v[0] * hit_normal.v[0] + local_c.v[1] * hit_normal.v[1] + local_c.v[2] * hit_normal.v[2];
        pRecord_count = AddObjectObjectForce((float*)pRecords, &hit_position, factor, &sphere_point, &normal, pRecord_count, pObject2, pObject1, 8, NULL, dot);
        if (pRecord_count == 500) {
            BrMatrix34ApplyP(&local_d, &hit_position, &pObject2->transform_matrix);
            BrMatrix34ApplyP(&local_e, &hit_position, &pObject2->actor->t.t.mat);
            BrMatrix34ApplyP(&local_f, &sphere_point, &pObject1->transform_matrix);
            BrMatrix34ApplyP(&local_g, &sphere_point, &pObject1->actor->t.t.mat);
            temp1.v[0] = local_d.v[0] - pObject1->transform_matrix.m[3][0];
            temp1.v[1] = local_d.v[1] - pObject1->transform_matrix.m[3][1];
            temp1.v[2] = local_d.v[2] - pObject1->transform_matrix.m[3][2];
            BrMatrix34TApplyV(&local_h, &temp1, &pObject1->transform_matrix);
            temp2.v[0] = local_e.v[0] - pObject1->actor->t.t.mat.m[3][0];
            temp2.v[1] = local_e.v[1] - pObject1->actor->t.t.mat.m[3][1];
            temp2.v[2] = local_e.v[2] - pObject1->actor->t.t.mat.m[3][2];
            BrMatrix34TApplyV(&local_i, &temp2, &pObject1->actor->t.t.mat);
            BrMatrix34TApplyV(&local_j, &normal, &pObject1->transform_matrix);
            temp3.v[0] = local_f.v[0] - pObject2->transform_matrix.m[3][0];
            temp3.v[1] = local_f.v[1] - pObject2->transform_matrix.m[3][1];
            temp3.v[2] = local_f.v[2] - pObject2->transform_matrix.m[3][2];
            BrMatrix34TApplyV(&local_k, &temp3, &pObject2->transform_matrix);
            temp4.v[0] = local_g.v[0] - pObject2->actor->t.t.mat.m[3][0];
            temp4.v[1] = local_g.v[1] - pObject2->actor->t.t.mat.m[3][1];
            temp4.v[2] = local_g.v[2] - pObject2->actor->t.t.mat.m[3][2];
            BrMatrix34TApplyV(&local_l, &temp4, &pObject2->actor->t.t.mat);
            BrMatrix34TApplyV(&local_j, &normal, &pObject2->transform_matrix);
        }
    }
    return pRecord_count;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00427870
int C2_HOOK_FASTCALL FUN_00427870(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, void* pRecords, int pRecord_count, undefined4 pArg11) {
    int count_before;
    int count_after;
    int i;
    int collided;
    tU8 local_80[64];
    tU8 local_40[64];
    tU8 common1;
    tU8 common2;
    char bit0;
    char bit1;
    char bit2;
    char bit3;
    char bit4;
    char bit5;
    tU8 code;
    const float* vertex;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    br_vector3 center1;
    br_vector3 center2;
    br_vector3 transformed1;
    br_vector3 transformed_vector;
    br_vector3 normal;
    br_vector3 world_normal;
    float length;
    float scale;
    union {
        const br_vector3* pointer;
        float scalar;
    } vertices_arg;

    common1 = 0xff;
    common2 = 0xff;
    i = 0;
    if (pCount2 > 0) {
        fVar1 = pShape1->common.bb.min.v[0] - gFLOAT_00589a10;
        fVar2 = pShape1->common.bb.min.v[1] - gFLOAT_00589a10;
        fVar3 = pShape1->common.bb.min.v[2] - gFLOAT_00589a10;
        fVar4 = pShape1->common.bb.max.v[0] - gFLOAT_00589a14;
        fVar5 = pShape1->common.bb.max.v[1] - gFLOAT_00589a14;
        fVar6 = pShape1->common.bb.max.v[2] - gFLOAT_00589a14;
        vertex = (const float*)((const tU8*)pVertices2 + 4);
        do {
            if (fVar1 <= vertex[-1] || fVar1 <= vertex[2]) bit0 = 0; else bit0 = 1;
            if (fVar2 <= vertex[0] || fVar2 <= vertex[3]) bit1 = 0; else bit1 = 1;
            if (fVar3 <= vertex[1] || fVar3 <= vertex[4]) bit2 = 0; else bit2 = 1;
            if (vertex[-1] <= fVar4 || vertex[2] <= fVar4) bit3 = 0; else bit3 = 1;
            if (vertex[0] <= fVar5 || vertex[3] <= fVar5) bit4 = 0; else bit4 = 1;
            if (vertex[1] <= fVar6 || vertex[4] <= fVar6) bit5 = 0; else bit5 = 1;
            code = (((((bit5 * 2 + bit4) * 2 + bit3) * 2 + bit2) * 2 + bit1) * 2 + bit0);
            local_80[i] = code;
            common2 &= code;
            vertex += 6;
            i++;
        } while (i < pCount2);
    }
    if (common2 != 0) return pRecord_count;

    i = 0;
    if (pCount1 > 0) {
        fVar1 = pShape2->common.bb.min.v[0] - gFLOAT_00589a10;
        fVar2 = pShape2->common.bb.min.v[1] - gFLOAT_00589a10;
        fVar3 = pShape2->common.bb.min.v[2] - gFLOAT_00589a10;
        fVar4 = pShape2->common.bb.max.v[0] - gFLOAT_00589a14;
        fVar5 = pShape2->common.bb.max.v[1] - gFLOAT_00589a14;
        fVar6 = pShape2->common.bb.max.v[2] - gFLOAT_00589a14;
        vertex = (const float*)((const tU8*)pVertices1 + 4);
        do {
            if (fVar1 <= vertex[-1] || fVar1 <= vertex[2]) bit0 = 0; else bit0 = 1;
            if (fVar2 <= vertex[0] || fVar2 <= vertex[3]) bit1 = 0; else bit1 = 1;
            if (fVar3 <= vertex[1] || fVar3 <= vertex[4]) bit2 = 0; else bit2 = 1;
            if (vertex[-1] <= fVar4 || vertex[2] <= fVar4) bit3 = 0; else bit3 = 1;
            if (vertex[0] <= fVar5 || vertex[3] <= fVar5) bit4 = 0; else bit4 = 1;
            if (vertex[1] <= fVar6 || vertex[4] <= fVar6) bit5 = 0; else bit5 = 1;
            code = (((((bit5 * 2 + bit4) * 2 + bit3) * 2 + bit2) * 2 + bit1) * 2 + bit0);
            local_40[i] = code;
            common1 &= code;
            vertex += 6;
            i++;
        } while (i < pCount1);
    }
    if (common1 != 0) return pRecord_count;

    count_before = pRecord_count;
    if (((pObject1->flags | pObject2->flags) & 0x100) != 0) {
        center1.v[0] = (pObject1->bb1.min.v[0] + pObject1->bb1.max.v[0]) * gFLOAT_00589a1c;
        center1.v[1] = (pObject1->bb1.min.v[1] + pObject1->bb1.max.v[1]) * gFLOAT_00589a1c;
        center1.v[2] = (pObject1->bb1.min.v[2] + pObject1->bb1.max.v[2]) * gFLOAT_00589a1c;
        BrMatrix34ApplyP(&transformed1, &center1, &pObject1->transform_matrix);
        center2.v[0] = transformed1.v[0] - pObject2->transform_matrix.m[3][0];
        center2.v[1] = transformed1.v[1] - pObject2->transform_matrix.m[3][1];
        center2.v[2] = transformed1.v[2] - pObject2->transform_matrix.m[3][2];
        BrMatrix34TApplyV(&transformed_vector, &center2, &pObject2->transform_matrix);
        normal.v[0] = transformed_vector.v[0] - (pObject2->bb1.min.v[0] + pObject2->bb1.max.v[0]) * gFLOAT_00589a1c;
        normal.v[1] = transformed_vector.v[1] - (pObject2->bb1.min.v[1] + pObject2->bb1.max.v[1]) * gFLOAT_00589a1c;
        normal.v[2] = transformed_vector.v[2] - (pObject2->bb1.min.v[2] + pObject2->bb1.max.v[2]) * gFLOAT_00589a1c;
        length = sqrtf(normal.v[0] * normal.v[0] + normal.v[1] * normal.v[1] + normal.v[2] * normal.v[2]);
        if (length <= gFLOAT_005899a8) {
            normal.v[0] = 1.0f;
            normal.v[1] = 0.0f;
            normal.v[2] = 0.0f;
        } else {
            scale = (float)(gDouble_005899b0 / length);
            normal.v[0] *= scale;
            normal.v[1] *= scale;
            normal.v[2] *= scale;
        }
        collided = 0;
        count_after = FUN_00427fd0(pObject1, pObject2, pShape1, pShape2, (int)pVertices1, (float*)pRecords, pRecord_count, pArg11, local_40, &collided, normal.v);
        BrMatrix34ApplyV(&world_normal, &normal, &pObject1->transform_matrix);
        BrMatrix34TApplyV(&normal, &world_normal, &pObject2->transform_matrix);
        normal.v[0] = -normal.v[0];
        normal.v[1] = -normal.v[1];
        normal.v[2] = -normal.v[2];
        count_after = FUN_00427fd0(pObject2, pObject1, pShape2, pShape1, (int)pVertices2, (float*)pRecords, count_after, pArg11, local_80, &collided, normal.v);
        if (collided != 0) {
            pObject1->flags |= 0x200;
            pObject2->flags |= 0x200;
            if (pObject1->parent != NULL && pObject1->parent == pObject2->parent) return count_before;
            return count_after;
        }
    }

    if (pShape2->common.type != 2) {
        vertices_arg.pointer = pVertices1;
        pRecord_count = FUN_00428530(pObject1, pObject2, pShape1, pShape2, vertices_arg.scalar, pCount1, (float*)pRecords, pRecord_count, pArg11, local_40);
    }
    if (pShape1->common.type != 2) {
        vertices_arg.pointer = pVertices2;
        pRecord_count = FUN_00428530(pObject2, pObject1, pShape2, pShape1, vertices_arg.scalar, pCount2, (float*)pRecords, pRecord_count, pArg11, local_80);
    }
    if (pRecord_count != 0 && pObject1 != *(tPhysics_object**)((tU8*)pRecords + count_before * 0x60 + 4)) {
        return FUN_004287a0(pObject2, pObject1, pShape2, pShape1, pVertices2, pVertices1, pRecords, pRecord_count, pArg11, local_80, local_40);
    }
    return FUN_004287a0(pObject1, pObject2, pShape1, pShape2, pVertices1, pVertices2, pRecords, pRecord_count, pArg11, local_40, local_80);
}

// FUNCTION: CARMA2_HW 0x00427fd0
int C2_HOOK_FASTCALL FUN_00427fd0(tPhysics_object* pObj1, tPhysics_object* pObj2, tPhysics_shape* pEdgeShape, tPhysics_shape* pRayShape, int pVertices, float* pRecords, int pRecordCount, undefined4 pUnused, const tU8* pEdge_visibility, int* pCollided, float* pNormal) {
    float fVar1;
    float fVar2;
    float fVar3;
    tU32 uVar4;
    tU32 uVar5;
    const tU8* pbVar6;
    float local_58;
    int local_54;
    int local_50;
    float local_4c;
    br_vector3 local_48;
    br_vector3 local_3c;
    br_vector3 local_30;
    br_vector3 local_24;
    br_vector3 local_18;
    br_vector3 local_c;

    switch (pEdgeShape->common.type) {
    case kCollisionShapeType_Box:
        pbVar6 = (const tU8*)gBox_edges_0058f7f0;
        local_54 = 0xc;
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        local_54 = pEdgeShape->polyhedron.polyhedron.count_edges;
        pbVar6 = (const tU8*)pEdgeShape->polyhedron.polyhedron.edges;
        break;
    case kCollisionShapeType_Wireframe:
        local_54 = pEdgeShape->wireframe.wireframe.count_lines;
        pbVar6 = (const tU8*)pEdgeShape->wireframe.wireframe.lines;
        break;
    default:
        PhysicsError(1, gEmpty_string_00676920);
        pbVar6 = (const tU8*)pRayShape;
        break;
    }
    local_50 = 0;
    if (0 < local_54) {
        do {
            if ((pEdge_visibility[pbVar6[1]] & pEdge_visibility[pbVar6[0]]) == 0) {
                uVar4 = ShapeRayCast((const br_vector3*)(pVertices + (tU32)pbVar6[0] * 0x18),
                    (const br_vector3*)(pVertices + (tU32)pbVar6[1] * 0x18), pRayShape,
                    &local_18, &local_58, &local_30);
                if (uVar4 != 0) {
                    *pCollided = 1;
                    if ((uVar4 & 0x40) == 0) {
                        uVar5 = ShapeRayCast((const br_vector3*)(pVertices + (tU32)pbVar6[1] * 0x18),
                            (const br_vector3*)(pVertices + (tU32)pbVar6[0] * 0x18), pRayShape,
                            &local_48, &local_4c, &local_3c);
                        if ((uVar5 & 0x40) == 0) {
                            goto LAB_00428440;
                        }
                        uVar4 = uVar4 & 0xff;
                    } else {
                        uVar4 = ShapeRayCast((const br_vector3*)(pVertices + (tU32)pbVar6[1] * 0x18),
                            (const br_vector3*)(pVertices + (tU32)pbVar6[0] * 0x18), pRayShape,
                            &local_18, &local_58, &local_30);
                        local_58 = gFLOAT_0058996c - local_58;
                    }
                    if ((uVar4 & 0x40) == 0) {
                        local_30.v[1] = -local_30.v[1];
                        local_30.v[2] = -local_30.v[2];
                        local_30.v[0] = -local_30.v[0];
                        if (gFLOAT_00589a20 <= local_30.v[0] * pNormal[0]
                                + pNormal[1] * local_30.v[1]
                                + pNormal[2] * local_30.v[2]) {
                            local_48.v[0] = local_18.v[0] - pObj2->cmpos.v[0];
                            local_48.v[1] = local_18.v[1] - pObj2->cmpos.v[1];
                            local_48.v[2] = local_18.v[2] - pObj2->cmpos.v[2];
                            local_3c.v[0] = pObj2->velocity_car_space.v[0]
                                + (pObj2->rotate_omega.v[1] * local_48.v[2] - pObj2->rotate_omega.v[2] * local_48.v[1]);
                            local_3c.v[1] = pObj2->velocity_car_space.v[1]
                                + (pObj2->rotate_omega.v[2] * local_48.v[0] - pObj2->rotate_omega.v[0] * local_48.v[2]);
                            local_3c.v[2] = pObj2->velocity_car_space.v[2]
                                + (pObj2->rotate_omega.v[0] * local_48.v[1] - pObj2->rotate_omega.v[1] * local_48.v[0]);
                            fVar3 = local_3c.v[0] * local_30.v[0];
                            fVar1 = local_3c.v[1] * local_30.v[1];
                            fVar2 = local_3c.v[2] * local_30.v[2];
                            GetShapePoint(&local_48, (tPhysics_shape*)pEdgeShape, pbVar6[0]);
                            GetShapePoint(&local_3c, (tPhysics_shape*)pEdgeShape, pbVar6[1]);
                            local_24.v[0] = local_48.v[0] + local_58 * (local_3c.v[0] - local_48.v[0]);
                            local_24.v[1] = local_48.v[1] + local_58 * (local_3c.v[1] - local_48.v[1]);
                            local_24.v[2] = local_48.v[2] + local_58 * (local_3c.v[2] - local_48.v[2]);
                            local_48.v[0] = local_24.v[0] - pObj1->cmpos.v[0];
                            local_48.v[1] = local_24.v[1] - pObj1->cmpos.v[1];
                            local_48.v[2] = local_24.v[2] - pObj1->cmpos.v[2];
                            local_3c.v[0] = pObj1->velocity_car_space.v[0]
                                + (pObj1->rotate_omega.v[1] * local_48.v[2] - pObj1->rotate_omega.v[2] * local_48.v[1]);
                            local_3c.v[1] = pObj1->velocity_car_space.v[1]
                                + (pObj1->rotate_omega.v[2] * local_48.v[0] - pObj1->rotate_omega.v[0] * local_48.v[2]);
                            local_3c.v[2] = pObj1->velocity_car_space.v[2]
                                + (pObj1->rotate_omega.v[0] * local_48.v[1] - pObj1->rotate_omega.v[1] * local_48.v[0]);
                            BrMatrix34ApplyV(&local_c, &local_30, &pObj2->transform_matrix);
                            BrMatrix34TApplyV(&local_48, &local_c, &pObj1->transform_matrix);
                            if (gFLOAT_00589978 < (fVar2 + fVar1 + fVar3)
                                    - (local_3c.v[2] * local_48.v[2] + local_3c.v[1] * local_48.v[1] + local_3c.v[0] * local_48.v[0])) {
                                pRecordCount = AddObjectObjectForce(pRecords, &local_24, 0.0f,
                                    &local_18, &local_c, pRecordCount,
                                    pObj1, pObj2, 6, (br_vector3*)(size_t)local_50, 0.0f);
                            }
                        }
                    }
                }
            }
LAB_00428440:
            local_50 = local_50 + 1;
            pbVar6 = pbVar6 + 2;
        } while (local_50 < local_54);
    }
    return pRecordCount;
}

// FUNCTION: CARMA2_HW 0x00428530
int C2_HOOK_FASTCALL FUN_00428530(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pCorner_shape, tPhysics_shape* pShape, float pT, int pEdge_count, float* pRecords, int pRecord_count, undefined4 pArg9, tU8* pEdge_visibility) {
    float fVar1;
    float fVar2;
    tU32 uVar3;
    int iVar4;
    tFinteray_ray_cast_locals local;

    uVar3 = pShape->common.type;
    uVar3 -= 2;
    if (uVar3 == 0) {
        return pRecord_count;
    }
    iVar4 = 0;
    if (0 < pEdge_count) {
        local.edge_record = (br_vector3*)(uintptr_t)*(tU32*)&pT;
        do {
            if (pEdge_visibility[iVar4] == 0
                    && (uVar3 = ShapeRayCastWithSphere(local.edge_record,
                        (br_vector3*)((tU8*)local.edge_record + 0xc), pShape,
                        &local.hit_position_and_corner[0], &local.shape_normal, &pT)) != 0) {
                if (uVar3 == 0xFFFFFFFF) {
                    pEdge_visibility[iVar4] = 0xFF;
                    pObject1->flags |= 0x200;
                    pObject2->flags |= 0x200;
                } else {
                    GetShapePoint(&local.hit_position_and_corner[1], (tPhysics_shape*)pCorner_shape, iVar4);
                    if ((uVar3 & 0x40) != 0) {
                        pT = 1.05f;
                        BrMatrix34ApplyV(&local.actor_normal, &local.shape_normal, &pObject2->actor->t.t.mat);
                        local.actor_normal.v[0] = -local.actor_normal.v[0];
                        local.actor_normal.v[1] = -local.actor_normal.v[1];
                        local.actor_normal.v[2] = -local.actor_normal.v[2];
                    } else {
                        BrMatrix34ApplyV(&local.actor_normal, &local.shape_normal, &pObject2->actor->t.t.mat);
                        BrMatrix34ApplyV(&local.object_normal, &local.shape_normal, &pObject2->transform_matrix);
                        fVar1 = -(gFLOAT_0058996c - pT);
                        local.object_normal.v[0] = local.object_normal.v[0] * fVar1;
                        local.object_normal.v[1] = local.object_normal.v[1] * fVar1;
                        fVar2 = -pT;
                        local.object_normal.v[2] = local.object_normal.v[2] * fVar1;
                        local.actor_normal.v[0] = local.actor_normal.v[0] * fVar2 + local.object_normal.v[0];
                        local.actor_normal.v[1] = local.actor_normal.v[1] * fVar2 + local.object_normal.v[1];
                        fVar1 = local.actor_normal.v[2] * fVar2 + local.object_normal.v[2];
                        local.actor_normal.v[2] = sqrtf(fVar1 * fVar1
                            + local.actor_normal.v[1] * local.actor_normal.v[1]
                            + local.actor_normal.v[0] * local.actor_normal.v[0]);
                        if (local.actor_normal.v[2] <= gFLOAT_005899a8) {
                            local.actor_normal.v[0] = 1.0f;
                            local.actor_normal.v[1] = 0.0f;
                            local.actor_normal.v[2] = 0.0f;
                        } else {
                            local.actor_normal.v[2] = (float)(gDouble_005899b0 / local.actor_normal.v[2]);
                            local.actor_normal.v[0] = local.actor_normal.v[2] * local.actor_normal.v[0];
                            local.actor_normal.v[1] = local.actor_normal.v[2] * local.actor_normal.v[1];
                            local.actor_normal.v[2] = local.actor_normal.v[2] * fVar1;
                        }
                    }
                    pRecord_count = AddObjectObjectForce(pRecords, &local.hit_position_and_corner[1], pT,
                        &local.hit_position_and_corner[0], &local.actor_normal, pRecord_count,
                        pObject1, pObject2, 5, NULL, 0.0f);
                }
            }
            iVar4 = iVar4 + 1;
            local.edge_record = (br_vector3*)((tU8*)local.edge_record + 0x18);
        } while (iVar4 < pEdge_count);
    }
    return pRecord_count;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004287a0
int C2_HOOK_FASTCALL FUN_004287a0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, void* pRecords, int pRecord_count, undefined4 pArg11, const tU8* pVisibility1, const tU8* pVisibility2) {
    int iVar1;
    float uVar2;
    float fVar3;
    float fVar4;
    tU8 bVar5;
    tU8 bVar6;
    int iVar7;
    tU8* pbVar8;
    int iVar9;
    tU8* local_12ec;
    int local_12e8;
    tU8* local_12e4;
    int local_12e0;
    int local_12dc;
    int local_12cc;
    br_vector3 shape_endpoint;
    br_vector3 edge_direction;
    br_vector3 edge_motion;
    br_vector3 vertex_motion;
    br_vector3 hit_position;
    float ray_factor;
    tPhysics_joint_data candidate;
    tU8 edge_indices[188];
    br_vector3 edge_starts[186];
    br_vector3 edge_deltas[186];

    /* Retail: 186 starts/deltas and one contiguous 0x60-byte candidate. */
    C2_HOOK_BUG_ON(sizeof(candidate) != 0x60);
    (void)pArg11;
    switch (pShape1->common.type) {
    case kCollisionShapeType_Box:
        local_12ec = (tU8*)gBox_edges_0058f7f0;
        local_12e8 = 0xc;
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        local_12ec = (tU8*)pShape1->polyhedron.polyhedron.edges;
        local_12e8 = pShape1->polyhedron.polyhedron.count_edges;
        break;
    case kCollisionShapeType_Wireframe:
        local_12ec = (tU8*)pShape1->wireframe.wireframe.lines;
        local_12e8 = pShape1->wireframe.wireframe.count_lines;
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, gEmpty_string_00676920);
        local_12ec = (tU8*)pShape1;
        local_12e8 = 0;
        break;
    }
    *(float*)&candidate.field_0x00 = 2.0f;
    switch (pShape2->common.type) {
    case kCollisionShapeType_Box:
        local_12e4 = (tU8*)gBox_edges_0058f7f0;
        local_12dc = 0xc;
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        local_12e4 = (tU8*)pShape2->polyhedron.polyhedron.edges;
        local_12dc = pShape2->polyhedron.polyhedron.count_edges;
        break;
    case kCollisionShapeType_Wireframe:
        local_12e4 = (tU8*)pShape2->wireframe.wireframe.lines;
        local_12dc = pShape2->wireframe.wireframe.count_lines;
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, gEmpty_string_00676920);
        local_12e4 = (tU8*)pShape2;
        local_12dc = 0;
        break;
    }

    iVar9 = 0;
    local_12e0 = 0;
    local_12cc = 0;
    pbVar8 = local_12e4;
    if (0 < local_12dc) {
        do {
            if ((pVisibility2[pbVar8[1]] & pVisibility2[pbVar8[0]]) == 0) {
                switch (pShape2->common.type) {
                case kCollisionShapeType_Box:
                    uVar2 = (pbVar8[0] & 2) == 0 ? pShape2->box.common.bb.max.v[0] : pShape2->box.common.bb.min.v[0];
                    *(float*)((tU8*)edge_starts + iVar9) = uVar2;
                    uVar2 = (pbVar8[0] & 1) == 0 ? pShape2->box.common.bb.max.v[1] : pShape2->box.common.bb.min.v[1];
                    *(float*)((tU8*)edge_starts + iVar9 + 4) = uVar2;
                    uVar2 = (pbVar8[0] & 4) == 0 ? pShape2->box.common.bb.max.v[2] : pShape2->box.common.bb.min.v[2];
                    *(float*)((tU8*)edge_starts + iVar9 + 8) = uVar2;
                    bVar5 = pbVar8[1];
                    shape_endpoint.v[0] = (bVar5 & 2) == 0 ? pShape2->box.common.bb.max.v[0] : pShape2->box.common.bb.min.v[0];
                    shape_endpoint.v[1] = (bVar5 & 1) == 0 ? pShape2->box.common.bb.max.v[1] : pShape2->box.common.bb.min.v[1];
                    shape_endpoint.v[2] = (bVar5 & 4) == 0 ? pShape2->box.common.bb.max.v[2] : pShape2->box.common.bb.min.v[2];
                    break;
                case kCollisionShapeType_Polyhedron:
                case kCollisionShapeType_Wireframe_Polyhedron:
                    iVar7 = (int)(tU32)pShape2->polyhedron.polyhedron.points;
                    goto copy_shape_points;
                case kCollisionShapeType_Wireframe:
                    iVar7 = (int)(tU32)pShape2->wireframe.wireframe.points;
copy_shape_points:
                    *(tU32*)((tU8*)edge_starts + iVar9) = *(tU32*)(iVar7 + (tU32)pbVar8[0] * 0xc);
                    *(tU32*)((tU8*)edge_starts + iVar9 + 4) = *(tU32*)(iVar7 + 4 + (tU32)pbVar8[0] * 0xc);
                    *(tU32*)((tU8*)edge_starts + iVar9 + 8) = *(tU32*)(iVar7 + 8 + (tU32)pbVar8[0] * 0xc);
                    iVar1 = iVar7 + (tU32)pbVar8[1] * 0xc;
                    shape_endpoint.v[0] = *(float*)(iVar7 + (tU32)pbVar8[1] * 0xc);
                    shape_endpoint.v[1] = *(float*)(iVar1 + 4);
                    shape_endpoint.v[2] = *(float*)(iVar1 + 8);
                    break;
                default:
                    PhysicsError(ePhysicsError_UnknownShapeType, gEmpty_string_00676920);
                    break;
                }
                fVar3 = *(float*)((tU8*)edge_starts + iVar9 + 4);
                fVar4 = *(float*)((tU8*)edge_starts + iVar9 + 8);
                *(float*)((tU8*)edge_deltas + iVar9) = shape_endpoint.v[0] - *(float*)((tU8*)edge_starts + iVar9);
                *(float*)((tU8*)edge_deltas + iVar9 + 4) = shape_endpoint.v[1] - fVar3;
                edge_indices[local_12e0] = (tU8)local_12cc;
                local_12e0++;
                *(float*)((tU8*)edge_deltas + iVar9 + 8) = shape_endpoint.v[2] - fVar4;
                iVar9 += 0xc;
            }
            local_12cc++;
            pbVar8 += 2;
        } while (local_12cc < local_12dc);
    }

    if (0 < local_12e8) {
        do {
            bVar5 = pVisibility1[local_12ec[1]];
            bVar6 = pVisibility1[local_12ec[0]];
            if ((bVar5 & bVar6) == 0 && bVar6 != 0xff && bVar5 != 0xff) {
                iVar9 = 0;
                edge_direction.v[0] = ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18))[0]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[0];
                edge_direction.v[1] = ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18))[1]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[1];
                edge_direction.v[2] = ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18))[2]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[2];
                edge_motion.v[0] = (((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18))[3]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[3]) - edge_direction.v[0];
                edge_motion.v[1] = (((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18))[4]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[4]) - edge_direction.v[1];
                edge_motion.v[2] = (((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18))[5]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[5]) - edge_direction.v[2];
                vertex_motion.v[0] = ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[3]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[0];
                vertex_motion.v[1] = ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[4]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[1];
                vertex_motion.v[2] = ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[5]
                    - ((const float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18))[2];
                if (0 < local_12e0) {
                    do {
                        iVar7 = FUN_00424990((float*)((tU8*)edge_starts + iVar9),
                                (float*)((tU8*)edge_deltas + iVar9),
                                (float*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18),
                                edge_direction.v, vertex_motion.v, edge_motion.v, (float*)&candidate, 0,
                                (int)(tU32)pObject1, (int)(tU32)pObject2);
                        if (iVar7 != 0) {
                            bVar5 = edge_indices[iVar9 / 0xc];
                            iVar7 = ShapeRayCast(
                                (const br_vector3*)((const tU8*)pVertices1 + (tU32)local_12ec[0] * 0x18),
                                (const br_vector3*)((const tU8*)pVertices1 + (tU32)local_12ec[1] * 0x18),
                                pShape2, &hit_position, &ray_factor, NULL);
                            if (iVar7 == 0) {
                                iVar7 = ShapeRayCast(
                                    (const br_vector3*)((const tU8*)pVertices2 + (tU32)local_12e4[(tU32)bVar5 * 2] * 0x18),
                                    (const br_vector3*)((const tU8*)pVertices2 + (tU32)local_12e4[(tU32)bVar5 * 2 + 1] * 0x18),
                                    pShape1, &hit_position, &ray_factor, NULL);
                                if (iVar7 == 0) {
                                    pRecord_count = AddObjectObjectForce((float*)pRecords, &candidate.anchor1,
                                            *(float*)&candidate.field_0x00, &candidate.anchor2,
                                            &candidate.axis1, pRecord_count, pObject1, pObject2,
                                            6, NULL, 0.0f);
                                    *(float*)&candidate.field_0x00 = 2.0f;
                                }
                            }
                        }
                        iVar9 += 0xc;
                    } while (iVar9 / 0xc < local_12e0);
                }
            }
            local_12ec += 2;
            local_12e8--;
        } while (local_12e8 != 0);
    }
    return pRecord_count;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0045d5e0
void C2_HOOK_FASTCALL FUN_0045d5e0(tCollision_shape_sphere_data* pSphere1, tCollision_shape_sphere_data* pSphere2, br_vector3* pStart, br_vector3* pEnd, br_vector3* pNormal, float* pFactor) {
    float* start_values;
    float* end_values;
    float delta_x;
    float delta_y;
    float delta_z;
    float delta_length_squared;
    float center_delta_x;
    float center_delta_y;
    float center_delta_z;
    float radius_sum;
    float projection;
    float perpendicular_distance_squared;
    float normal_length;
    float scale;

    start_values = pStart->v;
    end_values = pEnd->v;
    *pFactor = 100.0f;
    delta_x = end_values[0] - start_values[0];
    delta_y = end_values[1] - start_values[1];
    delta_z = end_values[2] - start_values[2];
    delta_length_squared = delta_z * delta_z + delta_y * delta_y + delta_x * delta_x;
    center_delta_x = start_values[0] - pSphere1->center.v[0];
    center_delta_y = start_values[1] - pSphere1->center.v[1];
    center_delta_z = start_values[2] - pSphere1->center.v[2];
    radius_sum = pSphere1->radius + pSphere2->radius;

    if (gDouble_00589fd8 <= delta_length_squared) {
        projection = -((center_delta_z * delta_z + center_delta_y * delta_y + center_delta_x * delta_x)
            / delta_length_squared);
        perpendicular_distance_squared = center_delta_z * center_delta_z
            + center_delta_y * center_delta_y + center_delta_x * center_delta_x
            - projection * projection * delta_length_squared;
        if (!((radius_sum - gDouble_00589fe0) * (radius_sum - gDouble_00589fe0) >= perpendicular_distance_squared)) {
            return;
        }
        if (perpendicular_distance_squared > radius_sum * radius_sum) {
            goto point_collision;
        }
        projection -= sqrtf((radius_sum * radius_sum - perpendicular_distance_squared) / delta_length_squared);
        if (!(gUNK_00589fac <= projection && projection <= gUNK_00589fb0)) {
            goto point_collision;
        }
        center_delta_x = delta_x * -projection - center_delta_x;
        center_delta_y = delta_y * -projection - center_delta_y;
        center_delta_z = delta_z * -projection - center_delta_z;
        normal_length = sqrtf(center_delta_x * center_delta_x
            + center_delta_z * center_delta_z + center_delta_y * center_delta_y);
        *pFactor = projection;
        pNormal->v[0] = center_delta_x;
        pNormal->v[1] = center_delta_y;
        pNormal->v[2] = center_delta_z;
        if (gUNK_00589fc4 < normal_length) {
            scale = (float)(gDouble_00589fe8 / normal_length);
            pNormal->v[0] = scale * center_delta_x;
            pNormal->v[1] = scale * center_delta_y;
            pNormal->v[2] = scale * center_delta_z;
        } else {
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
        }
        return;
    }
point_collision:
    PointSphereCollision(&pSphere1->center, pEnd, radius_sum, pNormal, pFactor);
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x0045d860
void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pPoint, br_vector3* pSphere_center, br_scalar pRadius, br_vector3* pNormal, br_scalar* pCollision_t) {
    br_scalar dx;
    br_scalar dy;
    br_scalar dz;
    volatile br_scalar distance_squared;
    br_scalar radius_squared;
    br_scalar scale;

    dx = pPoint->v[0] - pSphere_center->v[0];
    dy = pPoint->v[1] - pSphere_center->v[1];
    dz = pPoint->v[2] - pSphere_center->v[2];
    distance_squared = dx * dx + dy * dy + dz * dz;
    if ((pRadius - -0.002f) * (pRadius - -0.002f) > distance_squared) {
        radius_squared = pRadius * pRadius;
        if (radius_squared < distance_squared) {
            scale = (br_scalar)(1.0 / sqrt(distance_squared));
            pNormal->v[0] = scale * dx;
            pNormal->v[1] = scale * dy;
            pNormal->v[2] = scale * dz;
            *pCollision_t = 1.0f;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045d920
void C2_HOOK_FASTCALL FUN_0045d920(br_scalar* pStart, br_scalar* pEnd, br_scalar* pEdge_start, br_scalar* pEdge_end, br_scalar* pSphere, br_scalar* pCollision_pos, br_scalar* pNormal, br_scalar* pT, tU32 pStart_flags, tU32 pEnd_flags) {
    volatile br_vector3 ray_delta;
    volatile br_vector3 edge_delta;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    volatile float local_3c;
    volatile float local_38;
    volatile float local_34;
    volatile float local_30;
    volatile float local_2c;
    volatile float local_28;
    volatile float local_24;
    volatile float local_20;
    volatile float local_1c;
    volatile float local_c;
    volatile float local_8;
    volatile float local_4;

    *pT = 100.0f;
    ray_delta.v[0] = pEnd[0] - pStart[0];
    ray_delta.v[1] = pEnd[1] - pStart[1];
    edge_delta.v[0] = pEdge_end[0] - pEdge_start[0];
    edge_delta.v[1] = pEdge_end[1] - pEdge_start[1];
    ray_delta.v[2] = pEnd[2] - pStart[2];
    edge_delta.v[2] = pEdge_end[2] - pEdge_start[2];
    fVar1 = ray_delta.v[2] * ray_delta.v[2] + ray_delta.v[1] * ray_delta.v[1] + ray_delta.v[0] * ray_delta.v[0];
    local_4 = (edge_delta.v[2] * ray_delta.v[2] + edge_delta.v[1] * ray_delta.v[1] + edge_delta.v[0] * ray_delta.v[0]) / fVar1;
    local_c = ray_delta.v[0] * local_4;
    local_3c = edge_delta.v[0] - local_c;
    local_8 = ray_delta.v[1] * local_4;
    local_38 = edge_delta.v[1] - local_8;
    local_4 = ray_delta.v[2] * local_4;
    local_24 = pStart[0] - pEdge_start[0];
    local_34 = edge_delta.v[2] - local_4;
    local_20 = pStart[1] - pEdge_start[1];
    local_1c = pStart[2] - pEdge_start[2];
    fVar3 = local_34 * local_34 + local_38 * local_38 + local_3c * local_3c;
    fVar2 = -((local_1c * ray_delta.v[2] + local_20 * ray_delta.v[1] + local_24 * ray_delta.v[0]) / fVar1);
    local_30 = ray_delta.v[0] * fVar2 + local_24;
    local_2c = ray_delta.v[1] * fVar2 + local_20;
    local_28 = ray_delta.v[2] * fVar2 + local_1c;
    if (fVar3 < (float)gDouble_00589fd8) {
        StationarySphereEdgeCollide((const br_vector3*)pStart, (const br_vector3*)pEnd, (br_vector3*)&ray_delta, fVar1, (const br_vector3*)pEdge_end, (const tCollision_shape_sphere_data*)pSphere, (br_vector3*)pCollision_pos, (br_vector3*)pNormal, pT);
        return;
    }
    fVar2 = (local_28 * local_34 + local_2c * local_38 + local_30 * local_3c) / fVar3;
    local_3c = local_3c * fVar2;
    local_38 = local_38 * fVar2;
    fVar4 = (local_28 * local_28 + local_2c * local_2c + local_30 * local_30)
        - (local_34 * fVar2 * local_34 * fVar2 + local_38 * local_38 + local_3c * local_3c);
    if (pSphere[4] - pSphere[0] * gFLOAT_00589ff4 * gFLOAT_00589ff8 < fVar4) {
        return;
    }
    if (fVar4 <= pSphere[4]) {
        fVar2 = fVar2 - sqrtf((pSphere[4] - fVar4) / fVar3);
        local_c = (local_c * fVar2 + local_30) - local_24;
        local_8 = (local_8 * fVar2 + local_2c) - local_20;
        local_4 = (local_4 * fVar2 + local_28) - local_1c;
        fVar3 = (local_4 * ray_delta.v[2] + local_8 * ray_delta.v[1] + local_c * ray_delta.v[0]) / fVar1;
        if (gUNK_00589fac <= fVar3) {
            if (fVar3 <= gUNK_00589fb0) {
                if (gUNK_00589fac <= fVar2 && fVar2 <= gUNK_00589fb0) {
                    pCollision_pos[0] = pStart[0] + local_c;
                    pCollision_pos[1] = local_8 + pStart[1];
                    fVar1 = pStart[2];
                    pCollision_pos[2] = local_4 + fVar1;
                    fVar5 = pCollision_pos[0] - (pEdge_start[0] + (pEdge_end[0] - pEdge_start[0]) * fVar2);
                    fVar3 = pCollision_pos[1] - ((pEdge_end[1] - pEdge_start[1]) * fVar2 + pEdge_start[1]);
                    fVar4 = (local_4 + fVar1) - ((pEdge_end[2] - pEdge_start[2]) * fVar2 + pEdge_start[2]);
                    fVar1 = sqrtf(fVar4 * fVar4 + fVar3 * fVar3 + fVar5 * fVar5);
                    if (gUNK_00589fc4 < fVar1) {
                        fVar1 = (float)(gDouble_00589fe8 / fVar1);
                        pNormal[0] = fVar1 * fVar5;
                        pNormal[1] = fVar1 * fVar3;
                        pNormal[2] = fVar1 * fVar4;
                        *pT = fVar2;
                        return;
                    }
                    pNormal[0] = 1.0f;
                    pNormal[1] = 0.0f;
                    pNormal[2] = 0.0f;
                    *pT = fVar2;
                    return;
                }
                goto collision_fallback;
            }
            if ((pEnd_flags & 0x80) == 0) {
                FUN_0045e020((const br_vector3*)pEnd, (const br_vector3*)pEdge_start, (const br_vector3*)pEdge_end, (const br_vector3*)&edge_delta, (const tCollision_shape_sphere_data*)pSphere, (br_vector3*)pCollision_pos, (br_vector3*)pNormal, pT);
            }
        } else if ((pStart_flags & 0x80) == 0) {
            FUN_0045e020((const br_vector3*)pStart, (const br_vector3*)pEdge_start, (const br_vector3*)pEdge_end, (const br_vector3*)&edge_delta, (const tCollision_shape_sphere_data*)pSphere, (br_vector3*)pCollision_pos, (br_vector3*)pNormal, pT);
            if (*pT < gFloat_00589fd0) {
                return;
            }
            goto collision_fallback;
        }
        if (*pT < gFloat_00589fd0) {
            return;
        }
    }
collision_fallback:
    StationarySphereEdgeCollide((const br_vector3*)pStart, (const br_vector3*)pEnd, (br_vector3*)&ray_delta, fVar1, (const br_vector3*)pEdge_end, (const tCollision_shape_sphere_data*)pSphere, (br_vector3*)pCollision_pos, (br_vector3*)pNormal, pT);
}

// FUNCTION: CARMA2_HW 0x0045e200
void C2_HOOK_FASTCALL FUN_0045e200(br_scalar* pClearance, tCollision_shape_polyhedron_data* pShape, const br_vector3* pStart, const br_vector3* pEnd, br_vector3* pHit_position, br_vector3* pNormal, br_scalar* pFactor) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float fVar7;
    float* pfVar8;
    int iVar9;
    int iVar10;
    int iVar11;
    int local_400;
    int local_3f4;
    float afStack_3e0[124];
    float afStack_1f0[124];
    iVar9 = 0;
    *pFactor = 100.0f;
    iVar11 = pShape->count_planes;
    if (0 < iVar11) {
        pfVar8 = (float*)pShape->planes;
        do {
            fVar1 = pStart->v[2] * pfVar8[2];
            fVar2 = pStart->v[1] * pfVar8[1];
            fVar3 = *pfVar8 * pStart->v[0];
            fVar4 = pEnd->v[2] * pfVar8[2];
            fVar5 = pEnd->v[1] * pfVar8[1];
            fVar6 = *pfVar8 * pEnd->v[0];
            fVar7 = fVar3 + fVar2 + fVar1 + *pClearance + pfVar8[3];
            afStack_1f0[iVar9] = fVar7;
            fVar1 = fVar6 + fVar5 + fVar4 + *pClearance + pfVar8[3];
            afStack_3e0[iVar9] = fVar1;
            if (fVar7 < gDouble_00589fe0 && fVar1 < gDouble_00589fe0) {
                return;
            }
            iVar9++;
            pfVar8 += 4;
        } while (iVar9 < iVar11);
    }
    local_400 = 0;
    if (iVar11 <= 0) {
        return;
    }
    local_3f4 = 0;
    do {
        fVar1 = gUNK_00589fb0;
        if (gDouble_00589fe0 < afStack_3e0[local_400] && afStack_1f0[local_400] < gUNK_00589fac) {
            if (afStack_3e0[local_400] < gUNK_00589fac) {
                fVar1 = gUNK_00589fb0;
                pHit_position->v[0] = pEnd->v[0];
                pHit_position->v[1] = pEnd->v[1];
                pHit_position->v[2] = pEnd->v[2];
            } else {
                fVar1 = -afStack_1f0[local_400] / (afStack_3e0[local_400] - afStack_1f0[local_400]);
                fVar2 = gUNK_00589fb0 - fVar1;
                pHit_position->v[0] = pStart->v[0] * fVar2;
                pHit_position->v[1] = pStart->v[1] * fVar2;
                pHit_position->v[2] = pStart->v[2] * fVar2;
                fVar2 = pEnd->v[2];
                fVar3 = pEnd->v[0];
                pHit_position->v[1] = pEnd->v[1] * fVar1 + pHit_position->v[1];
                pHit_position->v[2] = fVar2 * fVar1 + pHit_position->v[2];
                pHit_position->v[0] = fVar3 * fVar1 + pHit_position->v[0];
            }
            iVar9 = 0;
            pfVar8 = (float*)((char*)pShape->planes + local_3f4);
            fVar2 = *pfVar8;
            fVar5 = pfVar8[1] * *pClearance + pHit_position->v[1];
            fVar4 = pfVar8[2] * *pClearance + pHit_position->v[2];
            fVar3 = *pClearance;
            pHit_position->v[1] = fVar5;
            pHit_position->v[0] = fVar2 * fVar3 + pHit_position->v[0];
            pHit_position->v[2] = fVar4;
            iVar11 = pShape->count_planes;
            if (0 < iVar11) {
                iVar10 = 0;
                do {
                    if (iVar9 != local_400) {
                        pfVar8 = (float*)((char*)pShape->planes + iVar10);
                        if (pfVar8[2] * fVar4 + pfVar8[1] * fVar5 + *pfVar8 * pHit_position->v[0] + pfVar8[3] <= gUNK_00589fac) {
                            break;
                        }
                    }
                    iVar9++;
                    iVar10 += 0x10;
                } while (iVar9 < iVar11);
            }
            if (iVar9 == iVar11) {
                *pFactor = fVar1;
                local_400 *= 0x10;
                pNormal->v[0] = ((br_vector4*)((char*)pShape->planes + local_400))->v[0];
                pNormal->v[1] = ((br_vector4*)((char*)pShape->planes + local_400))->v[1];
                pNormal->v[2] = ((br_vector4*)((char*)pShape->planes + local_400))->v[2];
                return;
            }
        }
        local_400++;
        local_3f4 += 0x10;
        if (iVar11 <= local_400) {
            return;
        }
    } while (1);
}

// FUNCTION: CARMA2_HW 0x0045e4e0
void C2_HOOK_FASTCALL FUN_0045e4e0(float* param_1, float* param_2, float* param_3, float* param_4, float* param_5, float* param_6, float* param_7) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float* pfVar5;
    int iVar6;
    int iVar7;
    int iVar8;
    float* pfVar9;
    float* pfVar10;
    float* local_18;

    *param_7 = 100.0f;
    fVar3 = *param_1;
    iVar7 = 0;
    pfVar5 = param_4;
    pfVar9 = param_2 + 1;
    do {
        fVar1 = (float)((pfVar9[3] + fVar3) - gDouble_00589fe0);
        if ((fVar1 < *(float*)(((int)param_3 - (int)param_4) + (int)pfVar5)) && (fVar1 < *pfVar5)) {
            return;
        }
        fVar1 = (float)((*pfVar9 - fVar3) - gDouble_0058a000);
        if ((*(float*)(((int)param_3 - (int)param_4) + (int)pfVar5) < fVar1) && (*pfVar5 < fVar1)) {
            return;
        }
        iVar7 = iVar7 + 1;
        pfVar5 = pfVar5 + 1;
        pfVar9 = pfVar9 + 1;
    } while (iVar7 < 3);
    iVar8 = 0;
    param_2 = param_2 + 4;
    iVar7 = (int)param_4 - (int)param_3;
    pfVar9 = param_3;
    local_18 = param_2;
    do {
        fVar4 = gUNK_00589fb0;
        fVar1 = local_18[-3] - fVar3;
        if ((*pfVar9 < fVar1) && (fVar1 - gDouble_0058a000 < *(float*)(iVar7 + (int)pfVar9))) {
            if (fVar1 < *(float*)(iVar7 + (int)pfVar9)) {
                fVar4 = (fVar1 - *pfVar9) / (*(float*)(iVar7 + (int)pfVar9) - *pfVar9);
                fVar1 = gUNK_00589fb0 - fVar4;
                *param_5 = *param_3 * fVar1;
                param_5[1] = param_3[1] * fVar1;
                param_5[2] = param_3[2] * fVar1;
                fVar1 = param_4[2];
                fVar2 = *param_4;
                param_5[1] = param_4[1] * fVar4 + param_5[1];
                param_5[2] = fVar1 * fVar4 + param_5[2];
                *param_5 = fVar2 * fVar4 + *param_5;
                param_5[iVar8] = fVar3 + param_5[iVar8];
            } else {
                *param_5 = *param_4;
                param_5[1] = param_4[1];
                param_5[2] = param_4[2];
            }
            iVar6 = 0;
            pfVar5 = param_2;
            pfVar10 = param_5;
            do {
                if ((iVar6 != iVar8) && ((*pfVar10 < pfVar5[-3] || (*pfVar5 < *pfVar10)))) {
                    break;
                }
                iVar6 = iVar6 + 1;
                pfVar10 = pfVar10 + 1;
                pfVar5 = pfVar5 + 1;
            } while (iVar6 < 3);
            if (iVar6 == 3) {
                *param_7 = fVar4;
                BrVector3SetFloat((br_vector3*)param_6, 0.0f, 0.0f, 0.0f);
                param_6[iVar8] = 1.0f;
                return;
            }
        }
        fVar4 = gUNK_00589fb0;
        fVar1 = fVar3 + *local_18;
        if ((fVar1 < *pfVar9) && (*(float*)(iVar7 + (int)pfVar9) < fVar1 - gDouble_00589fe0)) {
            if (*(float*)(iVar7 + (int)pfVar9) < fVar1) {
                fVar4 = (fVar1 - *pfVar9) / (*(float*)(iVar7 + (int)pfVar9) - *pfVar9);
                fVar1 = gUNK_00589fb0 - fVar4;
                *param_5 = *param_3 * fVar1;
                param_5[1] = param_3[1] * fVar1;
                param_5[2] = param_3[2] * fVar1;
                fVar1 = param_4[2];
                fVar2 = *param_4;
                param_5[1] = param_4[1] * fVar4 + param_5[1];
                param_5[2] = fVar1 * fVar4 + param_5[2];
                *param_5 = fVar2 * fVar4 + *param_5;
                param_5[iVar8] = param_5[iVar8] - fVar3;
            } else {
                *param_5 = *param_4;
                param_5[1] = param_4[1];
                param_5[2] = param_4[2];
            }
            iVar6 = 0;
            pfVar5 = param_2;
            pfVar10 = param_5;
            do {
                if ((iVar6 != iVar8) && ((*pfVar10 < pfVar5[-3] || (*pfVar5 < *pfVar10)))) {
                    break;
                }
                iVar6 = iVar6 + 1;
                pfVar10 = pfVar10 + 1;
                pfVar5 = pfVar5 + 1;
            } while (iVar6 < 3);
            if (iVar6 == 3) {
                *param_7 = fVar4;
                BrVector3SetFloat((br_vector3*)param_6, 0.0f, 0.0f, 0.0f);
                param_6[iVar8] = -1.0f;
                return;
            }
        }
        iVar8 = iVar8 + 1;
        local_18 = local_18 + 1;
        pfVar9 = pfVar9 + 1;
        if (2 < iVar8) {
            return;
        }
    } while (1);
}
