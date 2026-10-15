#include "finteray.h"

#include "brucetrk.h"
#include "globvars.h"
#include "spark.h"
#include "world.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

#include "c2_math.h"
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
        if (rd->v[i] >= -2.3841858e-7f) {
            if (rd->v[i] <= 2.3841858e-7f) {
                if (b->max.v[i] < rp->v[i] || rp->v[i] < b->min.v[i]) {
                    return 0;
                }
            } else {
                s = (-1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
                if (s < BR_SCALAR_MIN) {
                    t_far = BR_SCALAR_MIN;
                } else if (s < t_far) {
                    t_far = s;
                }
                t = (-1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
                if (t > BR_SCALAR_MAX) {
                    t_near = BR_SCALAR_MAX;
                } else if (t > t_near) {
                    t_near = t;
                }
            }
        } else {
            s = (-1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (-1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        }
    }
    if (t_far < t_near) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
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

    r = 0;
    t_near = 0.0f;
    t_far = 1.0f;
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
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model != NULL) {
            if (PickBoundsTestRay__finteray(&this_model->bounds, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
                t_near = 0.f;
                t_far = gNearest_T > 1.f ? 1.f : gNearest_T;
                r = callback(ap, this_model, this_material, pPosition, pDir, t_near, t_far, arg);
                if (r) {
                    return r;
                }
            }
            if (r) {
                return r;
            }
        }
    } else if (ap->type >= BR_ACTOR_BOUNDS && ap->type <= BR_ACTOR_BOUNDS_CORRECT) {
        if (!PickBoundsTestRay__finteray((br_bounds*)ap->type_data, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
            return 0;
        }
        for (a = ap->children; a != NULL; a = a->next) {
            r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
            if (r) {
                return r;
            }
        }
        return 0;
    }
    for (a = ap->children; a != NULL; a = a->next) {
        r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
        if (r) {
            return r;
        }
    }
    return 0;
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
        if (-numerator < 0.f || -numerator > 0.003f + d) {
            return;
        }
    } else {
        if ( numerator < 0.f ||  numerator > 0.003f - d) {
            return;
        }
    }
    t = -(numerator / d);
    if (t > 1.f) {
        t = 1.f;
    }
    BrVector3Scale(coll_pos, ray_dir, t);
    BrVector3Accumulate(coll_pos, ray_pos);
    axis_m = fabs(pFace->normal.v[0]) >= fabs(pFace->normal.v[1]) ? 0 : 1;
    if (fabsf(pFace->normal.v[2]) > fabs(pFace->normal.v[axis_m])) {
        axis_m = 2;
    }
    if (axis_m == 0) {
        axis_0 = 1;
        axis_1 = 2;
    } else if (axis_m == 1) {
        axis_0 = 0;
        axis_1 = 2;
    } else if (axis_m == 2) {
        axis_0 = 0;
        axis_1 = 1;
    }
    v0i1 = pFace->v[0].v[axis_0];
    v0i2 = pFace->v[0].v[axis_1];
    u0 = pFace->v[1].v[axis_0] - v0i1;
    u1 = pFace->v[1].v[axis_1] - v0i2;
    v0 = pFace->v[2].v[axis_0] - v0i1;
    v1 = pFace->v[2].v[axis_1] - v0i2;
    u2 = coll_pos->v[axis_0] - v0i1;
    v2 = coll_pos->v[axis_1] - v0i2;

    f_d = u0 * v1 - u1 * v0;
    alpha = u0 * v2 - u1 * u2;
    if (fabsf(alpha) > 1.0001f * fabsf(f_d) || f_d == 0.f || alpha / f_d < -0.0001f) {
        return;
    }
    beta = v0 * v2 - v1 * u2;
    if (fabsf(beta) > 1.0001f * fabsf(f_d) || f_d == 0.f || beta / f_d >= -0.0001f || alpha / f_d - beta / f_d > 1.0001f) {
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
void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pVertex, br_vector3* pSphere_start, br_vector3* pSphere_end, br_vector3* pSphere_direction, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    double x;
    double y;
    double z;
    double direction_x;
    double direction_y;
    double direction_z;
    double projection;
    double distance;
    double time;
    double length;
    double scale;
    double point_z;
    volatile br_vector3 delta;
    volatile br_scalar direction_squared;
    volatile br_scalar projection_scalar;
    volatile br_scalar distance_squared;
    volatile br_scalar time_scalar;

    (void)pSphere_end;
    direction_y = FinterayShapeLoadScalar(&pSphere_direction->v[1]);
    direction_z = FinterayShapeLoadScalar(&pSphere_direction->v[2]);
    y = FinterayShapeLoadScalar(&pVertex->v[1]) - FinterayShapeLoadScalar(&pSphere_start->v[1]);
    z = FinterayShapeLoadScalar(&pVertex->v[2]) - FinterayShapeLoadScalar(&pSphere_start->v[2]);
    direction_x = FinterayShapeLoadScalar(&pSphere_direction->v[0]);
    x = FinterayShapeLoadScalar(&pVertex->v[0]) - FinterayShapeLoadScalar(&pSphere_start->v[0]);
    delta.v[0] = FinterayEdgeScalar(x);
    delta.v[1] = FinterayEdgeScalar(y);
    delta.v[2] = FinterayEdgeScalar(z);
    direction_squared = FinterayEdgeScalar((direction_y * direction_y + direction_z * direction_z) + direction_x * direction_x);
    projection = ((direction_z * delta.v[2] + FinterayShapeLoadScalar(&pSphere_direction->v[1]) * delta.v[1])
        + FinterayShapeLoadScalar(&pSphere_direction->v[0]) * delta.v[0]) / direction_squared;
    projection_scalar = FinterayEdgeScalar(projection);
    distance = ((x * delta.v[0] + y * delta.v[1]) + z * delta.v[2]) - (projection * projection_scalar) * direction_squared;
    distance_squared = FinterayEdgeScalar(distance);
    if (distance_squared > FinterayShapeLoadScalar(&pSphere->radius_squared)) {
        return;
    }
    time = projection_scalar - sqrt((FinterayShapeLoadScalar(&pSphere->radius_squared) - distance_squared) / direction_squared);
    time_scalar = FinterayEdgeScalar(time);
    if (!(time >= 0.0) || time_scalar > 1.0f) {
        return;
    }
    FinterayBoxCopyScalar(&pPoint->v[0], &pVertex->v[0]);
    FinterayBoxCopyScalar(&pPoint->v[1], &pVertex->v[1]);
    point_z = FinterayShapeLoadScalar(&pVertex->v[2]);
    pPoint->v[2] = FinterayEdgeScalar(point_z);
    direction_y = FinterayShapeLoadScalar(&pSphere_direction->v[1]) * time_scalar;
    direction_x = FinterayShapeLoadScalar(&pSphere_direction->v[0]) * time_scalar;
    direction_z = FinterayShapeLoadScalar(&pSphere_direction->v[2]) * time_scalar;
    direction_x += FinterayShapeLoadScalar(&pSphere_start->v[0]);
    direction_y += FinterayShapeLoadScalar(&pSphere_start->v[1]);
    direction_z += FinterayShapeLoadScalar(&pSphere_start->v[2]);
    x = FinterayShapeLoadScalar(&pPoint->v[0]) - direction_x;
    y = FinterayShapeLoadScalar(&pPoint->v[1]) - direction_y;
    z = point_z - direction_z;
    length = sqrt((x * x + y * y) + z * z);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        pNormal->v[0] = FinterayEdgeScalar(scale * x);
        pNormal->v[1] = FinterayEdgeScalar(scale * y);
        pNormal->v[2] = FinterayEdgeScalar(scale * z);
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
    double closest;
    double difference;
    double distance = 0.0;

    for (axis = 0; axis < 3; axis++) {
        if (!(pPoint->v[axis] > pBounds->min.v[axis])) {
            closest = pBounds->min.v[axis];
        } else if (!(pPoint->v[axis] >= pBounds->max.v[axis])) {
            closest = pPoint->v[axis];
        } else {
            closest = pBounds->max.v[axis];
        }
        pClosest->v[axis] = FinterayScalar(closest);
        difference = (double)pPoint->v[axis] - closest;
        distance = distance + difference * difference;
    }
    return !(distance > pRadius_squared);
}


// FUNCTION: CARMA2_HW  0x00460e10
int C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    int axis;
    double length;
    double scale;
    double x;
    double y;
    double z;

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
        if (pPoint->v[axis] == pBounds->max.v[axis] || FinterayBoxNaN(pPoint->v[axis]) || FinterayBoxNaN(pBounds->max.v[axis])) {
            pNormal->v[axis] = -1.0f;
        } else if (pPoint->v[axis] == pBounds->min.v[axis] || FinterayBoxNaN(pPoint->v[axis]) || FinterayBoxNaN(pBounds->min.v[axis])) {
            pNormal->v[axis] = 1.0f;
        }
    }
    x = pNormal->v[0];
    y = pNormal->v[1];
    z = pNormal->v[2];
    length = sqrt((y * y + z * z) + x * x);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        pNormal->v[0] = FinterayScalar(scale * pNormal->v[0]);
        pNormal->v[1] = FinterayScalar(y * scale);
        pNormal->v[2] = FinterayScalar(z * scale);
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


// FUNCTION: CARMA2_HW  0x00461850
int C2_HOOK_FASTCALL ShapeRayCastWithSphere(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        return LineBoxCollWithSphere(pStart, pEnd, &pShape->common.bb, pPoint, pNormal, pT);
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        return LinePolyCollWithSphere(pStart, pEnd, &pShape->polyhedron, pPoint, pNormal, pT);
    case kCollisionShapeType_Wireframe:
        return 0;
    case kCollisionShapeType_Sphere:
        return LineSphereCollWithSphere(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, &gPhysics_empty_message);
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

