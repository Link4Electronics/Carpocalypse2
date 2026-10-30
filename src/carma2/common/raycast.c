#include "raycast.h"

extern const char gEmpty_string_00676920[];

extern const br_scalar gFLOAT_00589978;
extern const double gDouble_00589fe0;

#include "brucetrk.h"
#include "car.h"
#include "crush.h"
#include "finteray.h"
#include "globvars.h"
#include "physics.h"
#include "spark.h"

#include <math.h>
#include <brender/brender.h>

#include "carpocalypse2_macros.h"
#include "raycast.h"

// GLOBAL: CARMA2_HW 0x00676920
const char gEmpty_string_00676920[] = "";

// DRActorToRoot

// GLOBAL: CARMA2_HW 0x00589968
const br_scalar gFLOAT_00589968 = 2.0f;

// GLOBAL: CARMA2_HW 0x005899a0
const br_scalar gFLOAT_005899a0 = 0.005f;

// GLOBAL: CARMA2_HW 0x005899a4
const br_scalar gFLOAT_005899a4 = -0.005f;

// GLOBAL: CARMA2_HW 0x0058996c
const br_scalar gFLOAT_0058996c = 1.0f;

// GLOBAL: CARMA2_HW 0x00589978
const br_scalar gFLOAT_00589978 = 0.0f;

// GLOBAL: CARMA2_HW 0x00589970
const double gDouble_00589970 = 0.95;

// GLOBAL: CARMA2_HW 0x0058a008
const double gDouble_0058a008 = 1.001;

// GLOBAL: CARMA2_HW 0x0058a010
const double gDouble_0058a010 = -0.0001;

// GLOBAL: CARMA2_HW 0x0058a018
const double gDouble_0058a018 = 1.0001;

// GLOBAL: CARMA2_HW 0x00589980
const double gDouble_00589980 = -0.01;

// GLOBAL: CARMA2_HW 0x00589988
const double gDouble_00589988 = 0.0045;

// GLOBAL: CARMA2_HW 0x00589990
const double gDouble_00589990 = 0.0005;

// GLOBAL: CARMA2_HW 0x00589998
const double gDouble_00589998 = -0.95;

// GLOBAL: CARMA2_HW 0x005899a8
const br_scalar gFLOAT_005899a8 = 2.384186e-07f;

// GLOBAL: CARMA2_HW 0x005899b0
const double gDouble_005899b0 = 1.0;

// GLOBAL: CARMA2_HW 0x005899b8
const br_scalar gFLOAT_005899b8 = 0.01f;

// GLOBAL: CARMA2_HW 0x005899bc
const br_scalar gFLOAT_005899bc = 0.05f;

// GLOBAL: CARMA2_HW 0x005899c0
const br_scalar gFLOAT_005899c0 = 0.0010434783f;

// GLOBAL: CARMA2_HW 0x005899e8
const double gDouble_005899e8 = 0.0000001;

// GLOBAL: CARMA2_HW 0x005899dc
const br_scalar gFLOAT_005899dc = 4.0f;

// GLOBAL: CARMA2_HW 0x005899e0
const br_scalar gFLOAT_005899e0 = 1000.0f;

// GLOBAL: CARMA2_HW 0x005899f0
const br_scalar gFLOAT_005899f0 = -1.3333334f;

// GLOBAL: CARMA2_HW 0x005899f8
const double gDouble_005899f8 = 0.002;

// GLOBAL: CARMA2_HW 0x00589a00
const double gDouble_00589a00 = 0.001;

// GLOBAL: CARMA2_HW 0x005899c8
const double gDouble_005899c8 = 0.005;

// GLOBAL: CARMA2_HW 0x005899d0
const double gDouble_005899d0 = -0.005;

// GLOBAL: CARMA2_HW 0x00679430
int gCollision_face_mask__raycast;

// GLOBAL: CARMA2_HW 0x00679434
int gINT_00679434;

// GLOBAL: CARMA2_HW 0x0058f7f0
const tPolyhedron_edge_indexes gBox_edges__raycast[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

// GLOBAL: CARMA2_HW 0x006a20d0
br_actor* gY_picking_camera;

// GLOBAL: CARMA2_HW 0x006a20f0
br_matrix34 gPick_model_to_view__raycast;

// GLOBAL: CARMA2_HW 0x006a2120
br_scalar gHighest_y_below;

// GLOBAL: CARMA2_HW 0x0070518c
br_material* gMaterial_below;

// GLOBAL: CARMA2_HW 0x006a20dc
br_scalar gLowest_y_above;

// GLOBAL: CARMA2_HW 0x006a20e0
br_scalar gCurrent_y;

// GLOBAL: CARMA2_HW 0x006a20e4
br_model* gAbove_model;

// GLOBAL: CARMA2_HW 0x006a20d4
br_model* gBelow_model;

// GLOBAL: CARMA2_HW 0x006a20e8
int gAbove_face_index;

// GLOBAL: CARMA2_HW 0x006a20d8
int gBelow_face_index;


// FUNCTION: CARMA2_HW 0x004e3a30
int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] > 2.384186e-07f) {
            s = (rp->v[i] - b->max.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (rp->v[i] - b->min.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] < -2.384186e-07f) {
            s = (rp->v[i] - b->max.v[i]) * (1.0f / (rd->v[i] * 1.0f));
            if (s < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (s < t_far) {
                t_far = s;
            }
            t = (rp->v[i] - b->min.v[i]) * (1.0f / (rd->v[i] * 1.0f));
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

// FUNCTION: CARMA2_HW 0x004e3840
int C2_HOOK_FASTCALL ActorPick2D(br_actor* ap, br_model* model, br_material* material, dr_pick2d_cbfn* callback, void* arg) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_matrix34 m_to_v;
    br_matrix34 v_to_m;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_vector3 dir;

    r = 0;
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
    m_to_v = gPick_model_to_view__raycast;

    BrMatrix34PreTransform(&gPick_model_to_view__raycast, &ap->t);
    switch (ap->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
            if (PickBoundsTestRay__raycast(&this_model->bounds,
                    (br_vector3 *) v_to_m.m[3], (br_vector3 *) v_to_m.m[2],
                    0.f, BR_SCALAR_MAX, &t_near, &t_far) != 0) {
                BrVector3Negate(&dir, (br_vector3 *) v_to_m.m[2]);
                r = callback(
                        ap, this_model, this_material, (br_vector3 *) v_to_m.m[3],
                        &dir, t_near, t_far, arg);
                if (r != 0) {
                    goto done;
                }
            }
            if (r != 0) {
                goto done;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
        if (PickBoundsTestRay__raycast((br_bounds*)ap->type_data,
                (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2],
                0.0f, BR_SCALAR_MAX, &t_near, &t_far) != 0) {
            for (a = ap->children; a != NULL; a = a->next) {
                r = ActorPick2D(a, this_model, this_material, callback, arg);
                if (r != 0) {
                    break;
                }
            }
        }
        goto done;
    }
    for (a = ap->children; a != NULL; a = a->next) {
        r = ActorPick2D(a, this_model, this_material, callback, arg);
        if (r != 0) {
            break;
        }
    }
done:
    gPick_model_to_view__raycast = m_to_v;
    return r;
}

int C2_HOOK_FASTCALL DRActorToRoot(br_actor* a, br_actor* world, br_matrix34* m) {

    if (a == world) {
        BrMatrix34Identity(m);
        return 1;
    } else {
        BrTransformToMatrix34(m, &a->t);
        for (a = a->parent; a != NULL && a != world; a = a->parent) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(m, &a->t);
            }
        }
        return a == world;
    }
}

// FUNCTION: CARMA2_HW 0x004e36f0
int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg) {
    br_matrix34 camera_tfm;
    br_scalar scale;
    br_scalar cos_angle;
    br_scalar sin_angle;
    br_angle view_over_2;
    br_camera* camera_data;

    camera_data = (br_camera*)camera->type_data;
    DRActorToRoot(camera, world, &camera_tfm);
    BrMatrix34Inverse(&gPick_model_to_view__raycast, &camera_tfm);
    view_over_2 = camera_data->field_of_view / 2;
    cos_angle = BR_COS(view_over_2);
    sin_angle = BR_SIN(view_over_2);
    scale = cos_angle / sin_angle;
    BrMatrix34PostScale(&gPick_model_to_view__raycast, scale / camera_data->aspect, scale, 1.f);
    BrMatrix34PostShearZ(&gPick_model_to_view__raycast,
                         (float)(2 * pick_x) / (float)viewport->width,
                         (float)(-2 * pick_y) / (float)viewport->height);
    return ActorPick2D(world, NULL, gBlack_material, callback, arg);
}

// FUNCTION: CARMA2_HW 0x004e45b0
int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext) {
    br_scalar the_y;

    if (pMaterial == NULL || pMaterial->identifier == NULL
            || (pMaterial->identifier[0] != '!'
            && pMaterial->identifier[0] != '>')) {
        the_y = pPoint->v[1];
        if (the_y > gHighest_y_below) {
            gHighest_y_below = the_y;
            gMaterial_below = pMaterial;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL BadDiv_raycast(br_scalar a, br_scalar b) {

    return fabsf(b) < 1.0f && fabs(a) > fabs(b) * BR_SCALAR_MAX;
}

void C2_HOOK_FASTCALL DRVector2AccumulateScale_raycast(br_vector2* a, br_vector2* b, br_scalar s) {

    a->v[0] = b->v[0] * s + a->v[0];
    a->v[1] = b->v[1] * s + a->v[1];
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

// FUNCTION: CARMA2_HW 0x004e3d90
int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* model, br_material* material, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_raycast_cbfn* callback, void* arg) {
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    int group;
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
    v11face* v11f;
    v11group* v11g;
    br_vector4* eqn;

    t_near -= 0.001f;
    t_far += 0.001f;
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        v11g = &V11MODEL(model)->groups[group];
        for (f = 0; f < v11g->nfaces; f++) {
            v11f = &v11g->faces[f];
            eqn = &v11f->eqn;
#ifdef CARPOCALYPSE2_FIX_BUGS
            this_material = DrGroupMaterial(model, v11g, material);
#else
            if (v11g->face_colours.materials[0] != NULL) {
                this_material = v11g->face_colours.materials[0];
            } else {
                this_material = material;
            }
#endif
            d = BrVector3Dot(eqn, ray_dir);
            if ((fabsf(d) >= 2.3841858e-7f && this_material != NULL && (this_material->flags & (BR_MATF_TWO_SIDED | BR_MATF_ALWAYS_VISIBLE)) != 0) || d <= 0.0) {
                numerator = BrVector3Dot(eqn, ray_pos) - eqn->v[3];
                if (!BadDiv_raycast(numerator, d)) {
                    t = -(numerator / d);
                    if (t >= t_near && t <= t_far) {
                        BrVector3Scale(&p, ray_dir, t);
                        BrVector3Accumulate(&p, ray_pos);
                        axis_m = (fabsf(eqn->v[1]) > fabsf(eqn->v[0])) ? 1 : 0;
                        if (fabsf(eqn->v[2]) > fabsf(eqn->v[axis_m])) {
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

                        v0 = v11g->vertices[v11f->vertices[0]].p.v[axis_0];
                        u0 = v11g->vertices[v11f->vertices[0]].p.v[axis_1];

                        v1 = v11g->vertices[v11f->vertices[1]].p.v[axis_0] - v0;
                        u1 = v11g->vertices[v11f->vertices[1]].p.v[axis_1] - u0;
                        v2 = v11g->vertices[v11f->vertices[2]].p.v[axis_0] - v0;
                        u2 = v11g->vertices[v11f->vertices[2]].p.v[axis_1] - u0;

                        v0i1 = p.v[axis_0] - v0;
                        v0i2 = p.v[axis_1] - u0;
                        if (fabs(v1) > 2.3841858e-7f) {
                            f_d = v0i2 * v1 - u1 * v0i1;
                            f_n = u2 * v1 - u1 * v2;
                            if (f_n == 0.) {
                                continue;
                            }
                            beta = f_d / f_n;
                            alpha = (v0i1 - beta * v2) / v1;
                        } else {
                            beta = v0i1 / v2;
                            alpha = (v0i2 - beta * u2) / u1;
                        }

                        if (alpha >= 0.0 && beta >= 0.0 && beta + alpha <= 1.0) {
                            s_alpha = alpha;
                            s_beta = beta;
                            BrVector2Scale(&map, &v11g->vertices[v11f->vertices[1]].map, s_alpha);
                            DRVector2AccumulateScale_raycast(
                                &map,
                                &v11g->vertices[v11f->vertices[2]].map,
                                s_beta);
                            DRVector2AccumulateScale_raycast(
                                &map,
                                &v11g->vertices[v11f->vertices[0]].map,
                                1.0f - (s_alpha + s_beta));
                            v = 0;
                            e = 1;
                            if (s_alpha <= s_beta) {
                                if (0.5f - s_beta / 2.0f > s_alpha) {
                                    e = 0;
                                }
                                if (1.0f - s_beta * 2.0f < s_alpha) {
                                    v = 1;
                                }
                            } else {
                                if (1.0f - s_beta * 2.0f > s_alpha) {
                                    e = 2;
                                }
                                if (0.5f - s_beta / 2.0f < s_alpha) {
                                    v = 2;
                                }
                            }
                            r = callback(model, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
                            if (r != 0) {
                                return r;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e4570
int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindYVerticallyBelowPolyCallBack, pArg);
    }
    return 0;
}

int C2_HOOK_FASTCALL DRScenePick2D(br_actor* world, br_actor* camera, dr_pick2d_cbfn* callback, void* arg) {
    br_matrix34 camera_tfm;
    br_scalar scale;
    br_scalar angle;
    br_scalar aspect;
    br_scalar cos_angle;
    br_scalar sin_angle;
    br_angle view_over_2;
    br_camera* camera_data;

    camera_data = (br_camera*)camera->type_data;
    DRActorToRoot(camera, world, &camera_tfm);
    BrMatrix34Inverse(&gPick_model_to_view__raycast, &camera_tfm);
    view_over_2 = camera_data->field_of_view / 2;
    aspect = camera_data->aspect;
    angle = BrAngleToRadian(view_over_2);
    cos_angle = cosf(angle);
    sin_angle = sinf(angle);
    scale = cos_angle / sin_angle;

    BrMatrix34PostScale(&gPick_model_to_view__raycast, scale / aspect, scale, 1.0f);
    return ActorPick2D(world, NULL, gBlack_material, callback, arg);
}

// FUNCTION: CARMA2_HW 0x004e4370
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition) {
    tU8 cx;
    tU8 cz;
    tU8 x;
    tU8 z;
    tTrack_spec* track_spec;

    track_spec = &gProgram_state.track_spec;
    XZToColumnXZ(&cx, &cz, pPosition->v[0], pPosition->v[2], track_spec);
    gHighest_y_below = BR_SCALAR_MIN;
    gY_picking_camera->t.t.translate.t = *pPosition;
    for (x = MAX(0, cx - 1); x < MIN(track_spec->ncolumns_x, cx + 2); x++) {
        for (z = MAX(0, cz - 1); z < MIN(track_spec->ncolumns_z, cz + 2); z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                DRScenePick2D(track_spec->columns[z][x].actor_0x0, gY_picking_camera, FindYVerticallyBelowCallBack, NULL);
            }
        }
    }
    return gHighest_y_below;
}

// FUNCTION: CARMA2_HW 0x004e4600
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point) {
    br_scalar result;
    int number_of_attempts;
    br_vector3 cast_point;

    number_of_attempts = 0;
    BrVector3Copy(&cast_point, pCast_point);
    do {
        result = FindYVerticallyBelow(&cast_point);
        cast_point.v[1] += .2f;
    } while (result < -100.f && number_of_attempts++ < 10);
    return result;
}

// FUNCTION: CARMA2_HW 0x004e4300
int C2_HOOK_CDECL FindHighestPolyCallBack__raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pArg) {
    br_scalar the_y;

    the_y = pPoint->v[1];
    if (the_y <= gCurrent_y) {
        if (the_y > gHighest_y_below) {
            gHighest_y_below = the_y;
            gBelow_face_index = pF;
            gBelow_model = pModel;
        }
    } else {
        if (the_y < gLowest_y_above) {
            gLowest_y_above = the_y;
            gAbove_face_index = pF;
            gAbove_model = pModel;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e3d50
int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__raycast, pArg);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e3bc0
void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index) {

    gLowest_y_above = 30000.0;
    gHighest_y_below = -30000.0;
    gCurrent_y = pPosition->v[1] + 1.192093e-5f;
    gY_picking_camera->t.t.translate.t = *pPosition;
    gY_picking_camera->t.t.mat.m[3][1] += pStarting_height;
    DRScenePick2D(pWorld, gY_picking_camera, FindHighestCallBack__raycast, NULL);
    *pNearest_y_above = gLowest_y_above;
    *pNearest_y_below = gHighest_y_below;
    *pNearest_above_model = gAbove_model;
    *pNearest_below_model = gBelow_model;
    *pNearest_above_face_index = gAbove_face_index;
    *pNearest_below_face_index = gBelow_face_index;
}

// FUNCTION: CARMA2_HW 0x004225b0
void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* pDest, const br_vector3* pOrigin, const br_matrix34* pMatrix) {
    br_vector3 tv;

    BrVector3Sub(&tv, pOrigin, (br_vector3*)pMatrix->m[3]);
    BrMatrix34TApplyV(pDest, &tv, pMatrix);
}
// FUNCTION: CARMA2_HW 0x004e3660
void C2_HOOK_FASTCALL InitRayCasting(void) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* cam;

    gY_picking_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    cam = gY_picking_camera->type_data;
    cam->type = BR_CAMERA_PERSPECTIVE_FOV;
    cam->field_of_view = BrDegreeToAngle(70);
    cam->hither_z = 0.001f;
    cam->yon_z = 1000.0f;
    cam->aspect = 1.0f;

    gY_picking_camera->t.t.mat.m[0][0] = 1.0f;
    gY_picking_camera->t.t.mat.m[0][1] = 0.0f;
    gY_picking_camera->t.t.mat.m[0][2] = 0.0f;
    gY_picking_camera->t.t.mat.m[1][0] = 0.0f;
    gY_picking_camera->t.t.mat.m[1][1] = 0.0f;
    gY_picking_camera->t.t.mat.m[1][2] = -1.0f;
    gY_picking_camera->t.t.mat.m[2][0] = 0.0f;
    gY_picking_camera->t.t.mat.m[2][1] = 1.0f;
    gY_picking_camera->t.t.mat.m[2][2] = 0.0f;
#else
    br_camera* cam;

    gY_picking_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    cam = gY_picking_camera->type_data;
    cam->type = BR_CAMERA_PERSPECTIVE_FOV;
    cam->field_of_view = BrDegreeToAngle(70);
    cam->hither_z = 0.001f;
    cam->yon_z = 1000.0f;
    cam->aspect = 1.0f;

    gY_picking_camera->t.t.mat.m[0][0] = 1.0f;
    gY_picking_camera->t.t.mat.m[0][1] = 0.0f;
    gY_picking_camera->t.t.mat.m[0][2] = 0.0f;
    gY_picking_camera->t.t.mat.m[1][0] = 0.0f;
    gY_picking_camera->t.t.mat.m[1][1] = 0.0f;
    gY_picking_camera->t.t.mat.m[1][2] = -1.0f;
    gY_picking_camera->t.t.mat.m[2][0] = 0.0f;
    gY_picking_camera->t.t.mat.m[2][1] = 1.0f;
    gY_picking_camera->t.t.mat.m[2][2] = 0.0f;
#endif
}

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

// ActorPick2D

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FUNCTION: CARMA2_HW 0x004226d0
tFace_ref* C2_HOOK_FASTCALL FUN_004226d0(const br_vector3* pRay_pos, const br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pDistance, br_vector3* pHit_position, tFace_ref* pFaces, int pFace_count) {
    br_scalar t;
    br_vector3 coll_pos;
    br_vector3 normal;
    br_vector3* pNormal_output;
    tFace_ref* current_face;
    int remaining_faces;

    *pDistance = 2.0f;
    if (pFace_count > 0) {
        current_face = pFaces;
        remaining_faces = pFace_count;
        pNormal_output = pHit_position;
        do {
            CheckSingleFace(current_face, (br_vector3*)pRay_pos, (br_vector3*)pRay_dir, &normal, &t, &coll_pos);
            if (t < *pDistance) {
                *pDistance = t;
                pHit_normal->v[0] = normal.v[0];
                pHit_normal->v[1] = normal.v[1];
                pHit_normal->v[2] = normal.v[2];
                pNormal_output->v[0] = coll_pos.v[0];
                pNormal_output->v[1] = coll_pos.v[1];
                pNormal_output->v[2] = coll_pos.v[2];
                pHit_position = (br_vector3*)current_face;
            }
            current_face++;
            remaining_faces--;
        } while (remaining_faces != 0);
    }
    if (*pDistance < gFLOAT_00589968) {
        return (tFace_ref*)pHit_position;
    }
    return NULL;
}

// FindBestY

// FindYVerticallyBelowPolyCallBack

// FindYVerticallyBelowCallBack

// FindYVerticallyBelow

// FindYVerticallyBelow2

// FUNCTION: CARMA2_HW 0x004228b0
int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* pObject, void* pCollision_buffer, int pIndex) {
    int collision_count;
    int has_collision;
    int has_any_collision;
    int point_index;
    register int result_index;
    register tPhysics_shape* pShape;
    tFace_ref* closest_face;
    tCollision_shape_test_data shape_data;
    br_vector3 local_point;
    br_vector3 object_center;
    br_vector3 scratch[8];
    br_vector3 closest_point;
    br_vector3 closest_normal;
    br_vector3 world_point;
    br_scalar distance;
    br_scalar closest_distance;
    br_matrix34 inverse_actor_matrix;
    br_matrix34 object_to_actor_matrix;
    br_matrix34* pShape_transform;

    result_index = 1;
    collision_count = 0;
    has_any_collision = 0;
    pShape = pObject->shape;
    if (pObject->box_face_start == pObject->box_face_end) {
        return 0;
    }
    if (pIndex > 0x24) {
        return 0;
    }
    BrMatrix34LPInverse(&inverse_actor_matrix, &pObject->actor->t.t.mat);
    BrMatrix34Mul(&object_to_actor_matrix, &pObject->transform_matrix, &inverse_actor_matrix);

    for (; pShape != NULL; pShape = pShape->common.next) {
        if (!FUN_00423320(pObject, pShape, &object_to_actor_matrix)) {
            continue;
        }
        if (gSensible_place_collect_active != 0) {
            pShape_transform = &pObject->transform_matrix;
        } else {
            pShape_transform = &pObject->actor->t.t.mat;
        }
        shape_data.point_count = 0;
        shape_data.edge_count = 0;
        shape_data.edges = NULL;
        switch (pShape->common.type) {
        case kCollisionShapeType_Box:
            shape_data.point_count = 8;
            shape_data.edge_count = 12;
            shape_data.edges = (tPolyhedron_edge_indexes*)gBox_edges__raycast;
            for (point_index = 0; point_index < shape_data.point_count; point_index++) {
                local_point.v[0] = (point_index & 2) != 0 ? pShape->box.common.bb.min.v[0] : pShape->box.common.bb.max.v[0];
                local_point.v[1] = (point_index & 1) != 0 ? pShape->box.common.bb.min.v[1] : pShape->box.common.bb.max.v[1];
                local_point.v[2] = (point_index & 4) != 0 ? pShape->box.common.bb.min.v[2] : pShape->box.common.bb.max.v[2];
                /* both from the local point: [i] at the physics transform, [64 + i] at the shape transform */
                BrMatrix34ApplyP(&shape_data.points[point_index], &local_point, &pObject->transform_matrix);
                BrMatrix34ApplyP(&shape_data.points[point_index + 64], &local_point, pShape_transform);
                shape_data.point_flags[point_index] = 0;
            }
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            shape_data.point_count = (tU16)pShape->polyhedron.polyhedron.count_points;
            shape_data.edge_count = (tU16)pShape->polyhedron.polyhedron.count_edges;
            shape_data.edges = pShape->polyhedron.polyhedron.edges;
            for (point_index = 0; point_index < shape_data.point_count; point_index++) {
                BrMatrix34ApplyP(&shape_data.points[point_index], &pShape->polyhedron.polyhedron.points[point_index], &pObject->transform_matrix);
                BrMatrix34ApplyP(&shape_data.points[point_index + 64], &pShape->polyhedron.polyhedron.points[point_index], pShape_transform);
                shape_data.point_flags[point_index] = 0;
            }
            break;
        case kCollisionShapeType_Wireframe:
            shape_data.point_count = (tU16)pShape->wireframe.wireframe.count_points;
            shape_data.edge_count = (tU16)pShape->wireframe.wireframe.count_lines;
            shape_data.edges = pShape->wireframe.wireframe.lines;
            for (point_index = 0; point_index < shape_data.point_count; point_index++) {
                BrMatrix34ApplyP(&shape_data.points[point_index], &pShape->wireframe.wireframe.points[point_index], &pObject->transform_matrix);
                BrMatrix34ApplyP(&shape_data.points[point_index + 64], &pShape->wireframe.wireframe.points[point_index], pShape_transform);
                shape_data.point_flags[point_index] = 0;
            }
            break;
        case kCollisionShapeType_Sphere:
            shape_data.point_count = 1;
            BrMatrix34ApplyP(&shape_data.points[0], &pShape->sphere.sphere.center, &pObject->transform_matrix);
            BrMatrix34ApplyP(&shape_data.points[64], &pShape->sphere.sphere.center, pShape_transform);
            break;
        default:
            ((void (C2_HOOK_FASTCALL *)(tPhysicsError, const char*))PhysicsError)(ePhysicsError_UnknownShapeType, gEmpty_string_00676920);
            break;
        }

        if (gSensible_place_collect_active != 0) {
            BrMatrix34ApplyP(&object_center, &pObject->cmpos, &pObject->transform_matrix);
            for (point_index = 1; point_index < shape_data.point_count; point_index++) {
                shape_data.points[64 + point_index] = object_center;
            }
        }

        if (pShape->common.type == kCollisionShapeType_Sphere) {
            closest_distance = 2.0f;
            closest_face = NULL;
            for (point_index = pObject->box_face_start; point_index < pObject->box_face_end; point_index++) {
                SphereFaceCollide(&gFace_list__car[point_index], &shape_data.points[0], &shape_data.points[64],
                        pShape->sphere.sphere.radius, &closest_normal, &closest_point, &distance);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_face = &gFace_list__car[point_index];
                    object_center = closest_point;
                }
            }
            if (closest_distance <= gFLOAT_0058996c) {
                BrMatrix34TApplyV(&world_point, &object_center, pShape_transform);
                BrMatrix34TApplyV(&local_point, &object_center, &pObject->actor->t.t.mat);
                closest_normal.v[0] = pShape->sphere.sphere.center.v[0] - pShape->sphere.sphere.radius * world_point.v[0];
                closest_normal.v[1] = pShape->sphere.sphere.center.v[1] - pShape->sphere.sphere.radius * world_point.v[1];
                closest_normal.v[2] = pShape->sphere.sphere.center.v[2] - pShape->sphere.sphere.radius * world_point.v[2];
                collision_count = FUN_00422fe0((float*)((tU8*)pCollision_buffer + pIndex * 0x60), (float*)&closest_normal,
                        closest_distance, (float*)&object_center, 0, collision_count, pObject, 3,
                        closest_face, NULL);
            }
            has_collision = collision_count;
            result_index = FUN_00423c00(pObject, &pShape->sphere.sphere.radius,
                    (tU8*)pCollision_buffer + pIndex * 0x60, collision_count, 0x28 - pIndex,
                    &shape_data.points[0], &shape_data.points[64], scratch);
            collision_count = has_collision + (has_collision < result_index);
        } else {
            has_collision = 0;
            if (pObject->field_0xed != 0) {
                collision_count = FUN_004236a0(pObject, pShape, (tU8*)pCollision_buffer + pIndex * 0x60, collision_count,
                        0x28 - pIndex, &shape_data, scratch, &has_collision);
            }
            has_any_collision |= has_collision;
            collision_count = FUN_00423ed0(pObject, pShape, (tU8*)pCollision_buffer + pIndex * 0x60, collision_count,
                    0x28 - pIndex, &shape_data, scratch);
            if (has_collision == 0) {
                collision_count = FUN_00424140(pObject, pShape, (tPhysics_shape*)((tU8*)pCollision_buffer + pIndex * 0x60),
                        collision_count, 0x28 - pIndex, (tU8*)&shape_data, (undefined4)scratch);
            }
        }
    }

    pObject->field_0xed = (tU8)has_any_collision;
    if (collision_count == 0) {
        return 0;
    }
    if (collision_count > 1) {
        tU8* pCollision_slot = (tU8*)pCollision_buffer + pIndex * 0x60;
        float* pNext_normal = (float*)(pCollision_slot + 0x90);

        do {
            if (pNext_normal[1] * *(float*)(pCollision_slot + 0x34) +
                    *(float*)(pCollision_slot + 0x30) * pNext_normal[0] +
                    *(float*)(pCollision_slot + 0x2c) * pNext_normal[-1] <= gDouble_00589970) {
                break;
            }
            result_index++;
            pNext_normal += 0x18;
        } while (result_index < collision_count);
    }
    pObject->omega.v[0] = pObject->rotate_omega.v[0];
    pObject->omega.v[1] = pObject->rotate_omega.v[1];
    pObject->omega.v[2] = pObject->rotate_omega.v[2];
    return result_index;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00422fe0
int C2_HOOK_FASTCALL FUN_00422fe0(float* pCollision_list, float* pContact_pos, float pT, float* pNormal, undefined4 pUnk5, int pCollision_count, tPhysics_object* pOwner, int pType, void* pCollision_data, float* pOptional_normal) {
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    int iVar6;
    register float* pfVar7;
    register int iVar8;
    register int iVar9;
    register float* pfVar10;
    register float* pfVar11;
    register float* pfVar12;

    (void)pUnk5;
    (void)pUnk5;
    if (pOwner->parent == NULL) {
        tPhysics_joint* joint;
        float local_18[6];

        joint = pOwner->physics_joint2;
        if (joint != NULL && joint->type != 0) {
            iVar8 = 0;
            local_18[3] = pOwner->bb1.max.v[0] - joint->field_0x08.v[0];
            local_18[4] = pOwner->bb1.max.v[1] - joint->field_0x08.v[1];
            local_18[5] = pOwner->bb1.max.v[2] - joint->field_0x08.v[2];
            local_18[0] = pOwner->bb1.min.v[0] - joint->field_0x08.v[0];
            local_18[1] = pOwner->bb1.min.v[1] - joint->field_0x08.v[1];
            local_18[2] = pOwner->bb1.min.v[2] - joint->field_0x08.v[2];
            fVar5 = gFLOAT_00589978;
            do {
                fVar2 = (float)fabs(*(float*)((tU8*)local_18 + iVar8 + 0xc));
                if ((float)fabs(*(float*)((tU8*)local_18 + iVar8)) <= fVar2) {
                    fVar2 = (float)fabs(*(float*)((tU8*)local_18 + iVar8));
                }
                if (fVar5 < fVar2) {
                    fVar5 = fVar2;
                }
                iVar8 = iVar8 + 4;
            } while (iVar8 < 0xc);
            fVar5 = (float)((double)fVar5 - gDouble_00589980);
            fVar2 = (float)fabs(pContact_pos[1] - joint->field_0x08.v[1]);
            if ((float)fabs(pContact_pos[0] - joint->field_0x08.v[0]) < fVar5 && fVar2 < fVar5 &&
                    fVar2 + fVar2 < pOwner->bb1.max.v[1] - pOwner->bb1.min.v[1] &&
                    (float)fabs(pContact_pos[2] - joint->field_0x08.v[2]) < fVar5) {
                return pCollision_count;
            }
        }
    }
    iVar8 = 0;
    if (0 < pCollision_count) {
        pfVar7 = pCollision_list + 6;
        do {
            fVar4 = pfVar7[-1] - (pContact_pos[0] - pOwner->cmpos.v[0]);
            fVar2 = *pfVar7 - (pContact_pos[1] - pOwner->cmpos.v[1]);
            fVar3 = pfVar7[1] - (pContact_pos[2] - pOwner->cmpos.v[2]);
            fVar2 = fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4;
            if (fVar2 < gDouble_00589988) {
                fVar3 = pfVar7[5] * *pNormal + pNormal[1] * pfVar7[6] + pfVar7[7] * pNormal[2];
                if (gDouble_00589970 < fVar3 && fVar2 < gDouble_00589990) {
                    return pCollision_count;
                }
                if (fVar3 < gDouble_00589998) {
                    return pCollision_count;
                }
            }
            iVar8 = iVar8 + 1;
            pfVar7 = pfVar7 + 0x18;
        } while (iVar8 < pCollision_count);
    }
    iVar8 = pCollision_count;
    if (pCollision_count < 4) {
        iVar8 = pCollision_count + 1;
        pCollision_list[pCollision_count * 0x18] = 2.0f;
    }
    iVar9 = 0;
    pfVar7 = pCollision_list;
    if (0 < iVar8) {
        while (*pfVar7 <= pT) {
            iVar9 = iVar9 + 1;
            pfVar7 = pfVar7 + 0x18;
            if (iVar8 <= iVar9) {
                return iVar8;
            }
        }
        iVar6 = 3;
        if (iVar8 < 4) {
            iVar6 = iVar8;
        }
        if (iVar9 <= iVar6) {
            pCollision_count = (iVar6 - iVar9) + 1;
            pfVar11 = pCollision_list + iVar6 * 0x18;
            do {
                pfVar10 = pfVar11 - 0x18;
                pfVar12 = pfVar11;
                for (iVar9 = 0x18; iVar9 != 0; iVar9 = iVar9 - 1) {
                    *pfVar12 = *pfVar10;
                    pfVar10 = pfVar10 + 1;
                    pfVar12 = pfVar12 + 1;
                }
                pCollision_count = pCollision_count - 1;
                pfVar11 = pfVar11 - 0x18;
            } while (pCollision_count != 0);
        }
        *pfVar7 = pT;
        *(tU32*)&pfVar7[1] = (tU32)pOwner;
        pfVar7[2] = 0.0f;
        *(tU32*)&pfVar7[3] = (tU32)pCollision_data;
        pfVar7[0xb] = *pNormal;
        pfVar7[0xc] = pNormal[1];
        pfVar7[0xd] = pNormal[2];
        pfVar7[5] = *pContact_pos - pOwner->cmpos.v[0];
        pfVar7[6] = pContact_pos[1] - pOwner->cmpos.v[1];
        pfVar7[7] = pContact_pos[2] - pOwner->cmpos.v[2];
        if (pOptional_normal != NULL) {
            pfVar7[8] = *pOptional_normal;
            pfVar7[9] = pOptional_normal[1];
            pfVar7[10] = pOptional_normal[2];
        }
        *(tU32*)&pfVar7[0x17] = pType;
    }
    return iVar8;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00423320
int C2_HOOK_FASTCALL FUN_00423320(tPhysics_object* pCar, tPhysics_shape* pPart, br_matrix34* pPart_matrix) {
    int face_count;
    br_vector3 center_offset;
    float axis_extent[9];
    float plane_min;
    float plane_max;
    float plane_offset;
    float axis_projection;
    float vertex_projection;
    float* transformed_bounds_values;
    float* original_bounds_values;
    float* orientation;
    br_bounds* pTransformed_bounds;
    br_bounds* pOriginal_bounds;
    br_material* material;
    char* material_identifier;
    tFace_ref* face;
    tU8* pActor;
    int face_start;
    int face_end;
    int face_index;
    int axis;

    pActor = *(tU8**)(tU8*)pCar;
    pOriginal_bounds = (br_bounds*)((tU8*)pPart + 4);
    pTransformed_bounds = (br_bounds*)((tU8*)pPart + 0x1c);
    transformed_bounds_values = (float*)pTransformed_bounds;
    original_bounds_values = (float*)pOriginal_bounds;
    face_count = 0;
    GetNewBoundingBox(pTransformed_bounds, pOriginal_bounds, pPart_matrix);
    for (axis = 0; axis < 3; axis++) {
        if (original_bounds_values[axis] < transformed_bounds_values[axis]) {
            transformed_bounds_values[axis] = original_bounds_values[axis];
        }
        if (transformed_bounds_values[axis + 3] < original_bounds_values[axis + 3]) {
            transformed_bounds_values[axis + 3] = original_bounds_values[axis + 3];
        }
    }
    BrMatrix34ApplyV(&center_offset, &pTransformed_bounds->min, (br_matrix34*)(pActor + 0x2c));
    orientation = (float*)(pActor + 0x2c);
    for (axis = 0; axis < 3; axis++) {
        plane_offset = pTransformed_bounds->max.v[axis] - pTransformed_bounds->min.v[axis];
        axis_extent[axis * 3] = orientation[axis * 3] * plane_offset;
        axis_extent[axis * 3 + 1] = orientation[axis * 3 + 1] * plane_offset;
        axis_extent[axis * 3 + 2] = orientation[axis * 3 + 2] * plane_offset;
    }
    face_start = *(tU16*)((tU8*)pCar + 0x174);
    face_end = *(tU16*)((tU8*)pCar + 0x176);
    if (face_end > face_start) {
        face = &gFace_list__car[face_start];
        for (face_index = 0; face_index < (int)(*(tU16*)((tU8*)pCar + 0x176) - *(tU16*)((tU8*)pCar + 0x174)); face_index++, face++) {
            plane_min = ((center_offset.v[1] + *(float*)(pActor + 0x54)) - face->v[0].v[1]) * face->normal.v[1]
                + face->normal.v[2] * ((center_offset.v[2] + *(float*)(pActor + 0x58)) - face->v[0].v[2])
                + face->normal.v[0] * ((center_offset.v[0] + *(float*)(pActor + 0x50)) - face->v[0].v[0]);
            plane_max = plane_min;
            for (axis = 0; axis < 3; axis++) {
                plane_offset = axis_extent[axis * 3 + 2] * face->normal.v[2]
                    + face->normal.v[1] * axis_extent[axis * 3 + 1]
                    + face->normal.v[0] * axis_extent[axis * 3];
                if (gFLOAT_00589978 <= plane_offset) {
                    plane_max += plane_offset;
                } else {
                    plane_min += plane_offset;
                }
            }
            material = face->material;
            material_identifier = *(char**)((tU8*)material + 4);
            if ((gFLOAT_005899a0 < plane_max && gFLOAT_005899a0 < plane_min)
                || (plane_max < gFLOAT_005899a4 && plane_min < gFLOAT_005899a4)
                || *material_identifier == '!') {
                face->flags |= 0x80;
                continue;
            }
            if (*material_identifier == '>') {
                face->flags |= 0x80;
                continue;
            }
            for (axis = 0; axis < 3; axis++) {
                float* axis_orientation;
                axis_orientation = orientation + axis * 3;
                axis_projection = *(float*)(pActor + 0x58) * axis_orientation[2]
                    + *(float*)(pActor + 0x50) * axis_orientation[0]
                    + *(float*)(pActor + 0x54) * axis_orientation[1];
                vertex_projection = face->v[0].v[2] * axis_orientation[2]
                    + face->v[0].v[0] * axis_orientation[0]
                    + face->v[0].v[1] * axis_orientation[1];
                plane_min = (pTransformed_bounds->min.v[axis] - gFLOAT_005899a0) + axis_projection;
                if (plane_min <= vertex_projection) {
                    plane_max = (pTransformed_bounds->max.v[axis] + axis_projection) - gFLOAT_005899a4;
                    if (plane_max < vertex_projection
                        && plane_max < face->v[1].v[2] * axis_orientation[2]
                            + face->v[1].v[0] * axis_orientation[0]
                            + face->v[1].v[1] * axis_orientation[1]
                        && plane_max < face->v[2].v[2] * axis_orientation[2]
                            + face->v[2].v[0] * axis_orientation[0]
                            + face->v[2].v[1] * axis_orientation[1]) {
                        face->flags |= 0x80;
                    }
                } else if (face->v[1].v[2] * axis_orientation[2]
                        + face->v[1].v[0] * axis_orientation[0]
                        + face->v[1].v[1] * axis_orientation[1] < plane_min
                    && face->v[2].v[2] * axis_orientation[2]
                        + face->v[2].v[0] * axis_orientation[0]
                        + face->v[2].v[1] * axis_orientation[1] < plane_min) {
                    face->flags |= 0x80;
                }
            }
            face->flags &= ~0x80;
            face_count++;
        }
    }
    return face_count;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004236a0
int C2_HOOK_FASTCALL FUN_004236a0(tPhysics_object* pObject, tPhysics_shape* pShape, void* pCollision_slot, int pCollision_count, int pMax_collisions, tCollision_shape_test_data* pShape_data, void* pScratch, int* pHas_collision) {
    br_vector3 object_center;
    tFace_ref* faces;
    int face_count;
    int edge_index;

    (void)pMax_collisions;
    (void)pScratch;
    if (pShape->common.type == kCollisionShapeType_Sphere) {
        return pCollision_count;
    }

    faces = &gFace_list__car[pObject->box_face_start];
    face_count = pObject->box_face_end - pObject->box_face_start;
    BrMatrix34ApplyP(&object_center, &pObject->cmpos, &pObject->transform_matrix);
    for (edge_index = 0; edge_index < pShape_data->edge_count; edge_index++) {
        tPolyhedron_edge_indexes* edge;
        br_vector3* edge_start;
        br_vector3* edge_end;
        br_vector3 forward;
        br_vector3 reverse;
        br_vector3 forward_normal;
        br_vector3 reverse_normal;
        br_vector3 forward_hit;
        br_vector3 reverse_hit;
        br_vector3* hit;
        br_vector3* normal;
        br_vector3 relative_hit;
        br_vector3 object_local_hit;
        br_vector3 world_hit;
        br_vector3 world_normal;
        br_scalar forward_t;
        br_scalar reverse_t;
        br_scalar center_distance;
        br_scalar forward_dot;
        br_scalar reverse_dot;
        br_scalar dx;
        br_scalar dy;
        br_scalar dz;
        br_scalar length;
        tFace_ref* forward_face;
        tFace_ref* reverse_face;
        tFace_ref* hit_face;

        edge = &pShape_data->edges[edge_index];
        edge_start = &pShape_data->points[edge->index1];
        edge_end = &pShape_data->points[edge->index2];
        forward.v[0] = edge_end->v[0] - edge_start->v[0];
        forward.v[1] = edge_end->v[1] - edge_start->v[1];
        forward.v[2] = edge_end->v[2] - edge_start->v[2];
        forward_face = RayCastThruFaceList(edge_start, &forward, &forward_normal, &forward_t, &forward_hit, faces, face_count);
        reverse.v[0] = edge_start->v[0] - edge_end->v[0];
        reverse.v[1] = edge_start->v[1] - edge_end->v[1];
        reverse.v[2] = edge_start->v[2] - edge_end->v[2];
        if (forward_face == NULL) {
            reverse_face = RayCastThruFaceList(edge_end, &reverse, &forward_normal, &forward_t, &forward_hit, faces, face_count);
            if (reverse_face == NULL) {
                continue;
            }
            hit = &forward_hit;
            normal = &forward_normal;
            hit_face = reverse_face;
            dx = object_center.v[0] - hit->v[0];
            dy = object_center.v[1] - hit->v[1];
            dz = object_center.v[2] - hit->v[2];
            length = (br_scalar)sqrt(dx * dx + dy * dy + dz * dz);
            if (gFLOAT_005899a8 < length) {
                length = (br_scalar)(gDouble_005899b0 / length);
                dx *= length;
                dy *= length;
                dz *= length;
            } else {
                dx = gFLOAT_0058996c;
                dy = gFLOAT_00589978;
                dz = gFLOAT_00589978;
            }
            if (gFLOAT_005899b8 <= dz * normal->v[2] + dy * normal->v[1] + dx * normal->v[0]) {
                pShape_data->point_flags[edge->index1] = 1;
            } else {
                continue;
            }
        } else {
            dx = object_center.v[0] - forward_hit.v[0];
            dy = object_center.v[1] - forward_hit.v[1];
            dz = object_center.v[2] - forward_hit.v[2];
            length = (br_scalar)sqrt(dx * dx + dy * dy + dz * dz);
            if (gFLOAT_005899a8 < length) {
                length = (br_scalar)(gDouble_005899b0 / length);
                dx *= length;
                dy *= length;
                dz *= length;
            } else {
                dx = gFLOAT_0058996c;
                dy = gFLOAT_00589978;
                dz = gFLOAT_00589978;
            }
            forward_dot = dz * forward_normal.v[2] + dy * forward_normal.v[1] + dx * forward_normal.v[0];
            reverse_face = RayCastThruFaceList(edge_end, &reverse, &reverse_normal, &reverse_t, &reverse_hit, faces, face_count);
            if (reverse_face == NULL) {
                if (gFLOAT_005899b8 < forward_dot) {
                    pShape_data->point_flags[edge->index2] = 1;
                    hit = &forward_hit;
                    normal = &forward_normal;
                    hit_face = forward_face;
                } else {
                    continue;
                }
            } else {
                dx = object_center.v[0] - reverse_hit.v[0];
                dy = object_center.v[1] - reverse_hit.v[1];
                dz = object_center.v[2] - reverse_hit.v[2];
                length = (br_scalar)sqrt(dx * dx + dy * dy + dz * dz);
                if (gFLOAT_005899a8 < length) {
                    length = (br_scalar)(gDouble_005899b0 / length);
                    dx *= length;
                    dy *= length;
                    dz *= length;
                } else {
                    dx = gFLOAT_0058996c;
                    dy = gFLOAT_00589978;
                    dz = gFLOAT_00589978;
                }
                reverse_dot = dz * reverse_normal.v[2] + dy * reverse_normal.v[1] + dx * reverse_normal.v[0];
                if (gFLOAT_005899bc <= forward_dot || gFLOAT_005899bc <= reverse_dot) {
                    if (reverse_dot <= forward_dot) {
                        pShape_data->point_flags[edge->index2] = 1;
                        hit = &forward_hit;
                        normal = &forward_normal;
                        hit_face = forward_face;
                    } else {
                        pShape_data->point_flags[edge->index1] = 1;
                        hit = &reverse_hit;
                        normal = &reverse_normal;
                        hit_face = reverse_face;
                    }
                } else {
                    continue;
                }
            }
        }

        relative_hit.v[0] = hit->v[0] - pObject->transform_matrix.m[3][0];
        relative_hit.v[1] = hit->v[1] - pObject->transform_matrix.m[3][1];
        relative_hit.v[2] = hit->v[2] - pObject->transform_matrix.m[3][2];
        BrMatrix34TApplyV(&object_local_hit, &relative_hit, &pObject->transform_matrix);
        BrMatrix34ApplyP(&world_hit, &object_local_hit, (br_matrix34*)((tU8*)pObject->actor + 0x2c));
        center_distance = (hit->v[0] - world_hit.v[0]) * normal->v[0]
            + (hit->v[1] - world_hit.v[1]) * normal->v[1]
            + (hit->v[2] - world_hit.v[2]) * normal->v[2];
        if (gFLOAT_00589978 < center_distance) {
            BrMatrix34TApplyV(&world_normal, normal, &pObject->actor->t.t.mat);
            pCollision_count = FUN_00422fe0((float*)pCollision_slot, (float*)&object_local_hit, gFLOAT_00589978,
                    (float*)&world_normal, *(undefined4*)&center_distance, pCollision_count, pObject, 1, hit_face, NULL);
        }
        *pHas_collision = 1;
    }
    return pCollision_count;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00423c00
int C2_HOOK_FASTCALL FUN_00423c00(tPhysics_object* pOwner, br_scalar* pSphere, void* pCollision_list, int pCollision_count, int pUnused5, br_vector3* pEdge_start, br_vector3* pEdge_end, void* pUnused8) {
    int i;
    tFace_ref* pFace;

    i = pOwner->box_face_start;
    pFace = &gFace_list__car[i];
    if (i < pOwner->box_face_end) {
        do {
            if ((pFace->flags & 0x80) == 0) {
                if ((pFace->flags & 1) == 0) {
                    pCollision_count = FUN_00423ce0(&pFace->v[0], &pFace->v[1], pOwner, pSphere,
                            pCollision_list, pCollision_count, pEdge_start, pEdge_end, pFace);
                }
                if ((pFace->flags & 2) == 0) {
                    pCollision_count = FUN_00423ce0(&pFace->v[1], &pFace->v[2], pOwner, pSphere,
                            pCollision_list, pCollision_count, pEdge_start, pEdge_end, pFace);
                }
                if ((pFace->flags & 4) == 0) {
                    pCollision_count = FUN_00423ce0(&pFace->v[2], &pFace->v[0], pOwner, pSphere,
                            pCollision_list, pCollision_count, pEdge_start, pEdge_end, pFace);
                }
            }
            i++;
            pFace++;
        } while (i < pOwner->box_face_end);
    }
    return pCollision_count;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00423ce0
int C2_HOOK_FASTCALL FUN_00423ce0(br_vector3* pStart, br_vector3* pEnd, tPhysics_object* pOwner, br_scalar* pSphere, void* pCollision_list, int pCollision_count, br_vector3* pEdge_start, br_vector3* pEdge_end, void* pCollision_data) {
    float fVar1;
    float fVar2;
    float fVar3;
    float* pfVar4;
    int uVar5;
    float scratch[18];

    pfVar4 = (float*)pSphere;
    FUN_0045d920((br_scalar*)pStart, (br_scalar*)pEnd, (br_scalar*)pEdge_start, (br_scalar*)pEdge_end,
            pSphere, &scratch[15], &scratch[9], (br_scalar*)&pSphere, 0, 0);
    if (*(br_scalar*)&pSphere <= gFLOAT_0058996c) {
        BrMatrix34TApplyV((br_vector3*)&scratch[0], (br_vector3*)&scratch[9], &pOwner->transform_matrix);
        BrMatrix34TApplyV((br_vector3*)&scratch[3], (br_vector3*)&scratch[9], &pOwner->actor->t.t.mat);
        fVar1 = gFLOAT_0058996c - *(br_scalar*)&pSphere;
        scratch[3] = *(br_scalar*)&pSphere * scratch[3];
        scratch[4] = *(br_scalar*)&pSphere * scratch[4];
        scratch[5] = *(br_scalar*)&pSphere * scratch[5];
        fVar3 = scratch[0] * fVar1 + scratch[3];
        fVar2 = scratch[1] * fVar1 + scratch[4];
        scratch[14] = scratch[2] * fVar1 + scratch[5];
        fVar1 = sqrtf(scratch[14] * scratch[14] + fVar2 * fVar2 + fVar3 * fVar3);
        if (fVar1 <= gFLOAT_005899a8) {
            scratch[14] = 0.0f;
            fVar2 = gFLOAT_00589978;
            fVar1 = gFLOAT_0058996c;
        } else {
            fVar1 = (float)(gDouble_005899b0 / fVar1);
            scratch[14] *= fVar1;
            fVar2 *= fVar1;
            fVar1 *= fVar3;
        }
        scratch[6] = pfVar4[1] + pfVar4[0] * fVar1;
        scratch[7] = pfVar4[2] + pfVar4[0] * fVar2;
        scratch[8] = pfVar4[3] + pfVar4[0] * scratch[14];
        scratch[0] = -scratch[0];
        scratch[1] = -scratch[1];
        scratch[2] = -scratch[2];
        uVar5 = FUN_00422fe0((float*)pCollision_list, &scratch[6], *(br_scalar*)&pSphere, &scratch[0], 0, pCollision_count, pOwner, 4, pCollision_data, NULL);
        return uVar5;
    }
    return pCollision_count;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00423ed0
int C2_HOOK_FASTCALL FUN_00423ed0(tPhysics_object* pObject, tPhysics_shape* pShape, void* pCollision_slot, int pCollision_count, int pMax_collisions, tCollision_shape_test_data* pShape_data, void* pScratch) {
    int point_index;
    register br_matrix34* object_matrix;

    (void)pShape;
    (void)pScratch;

    if (pMax_collisions < 4) {
        return 0;
    }
    object_matrix = &pObject->transform_matrix;
    gINT_00679434 = 0;
    for (point_index = 0; point_index < pShape_data->point_count; point_index++) {
        if (pShape_data->point_flags[point_index] == 0) {
            br_vector3 direction;
            br_vector3 hit_normal;
            br_vector3 hit_position;
            br_vector3 world_normal;
            float normal_and_direction[3];
            br_scalar best_t;
            br_scalar length;
            br_scalar normal_dot;
            br_vector3* point;
            tFace_ref* face;

            point = &pShape_data->points[point_index];
            direction.v[0] = pShape_data->points[64 + point_index].v[0] - point->v[0];
            direction.v[1] = pShape_data->points[64 + point_index].v[1] - point->v[1];
            direction.v[2] = pShape_data->points[64 + point_index].v[2] - point->v[2];
            length = sqrtf(direction.v[0] * direction.v[0] + direction.v[1] * direction.v[1] + direction.v[2] * direction.v[2]);
            if (length <= gFLOAT_005899a8) {
                normal_and_direction[0] = gFLOAT_005899c0;
                normal_and_direction[1] = 0.0f;
                normal_and_direction[2] = 0.0f;
            } else {
                br_scalar inverse_length;

                inverse_length = (br_scalar)(gDouble_005899b0 / length);
                normal_and_direction[0] = inverse_length * direction.v[0] * gFLOAT_005899c0;
                normal_and_direction[1] = inverse_length * direction.v[1] * gFLOAT_005899c0;
                normal_and_direction[2] = inverse_length * direction.v[2] * gFLOAT_005899c0;
            }
            direction.v[0] += normal_and_direction[0];
            direction.v[1] += normal_and_direction[1];
            direction.v[2] += normal_and_direction[2];

            face = RayCastThruFaceList(point, &direction, &hit_normal, &best_t, &hit_position,
                    &gFace_list__car[pObject->box_face_start], pObject->box_face_end - pObject->box_face_start);
            if (face != NULL) {
                br_scalar t;
                br_vector3 relative_position;

                BrMatrix34TApplyV(&world_normal, &hit_normal, object_matrix);
                normal_dot = world_normal.v[0] * direction.v[0] + world_normal.v[1] * direction.v[1] + world_normal.v[2] * direction.v[2];
                t = best_t;
                relative_position.v[0] = point->v[0] - object_matrix->m[3][0];
                relative_position.v[1] = point->v[1] - object_matrix->m[3][1];
                relative_position.v[2] = point->v[2] - object_matrix->m[3][2];
                BrMatrix34TApplyV(&hit_position, &relative_position, object_matrix);
                pCollision_count = FUN_00422fe0((float*)pCollision_slot, (float*)&hit_position, t,
                        (float*)&world_normal, *(undefined4*)&normal_dot, pCollision_count, pObject, 1, face, NULL);
            }
        }
    }
    if (pCollision_count > 3) {
        pCollision_count = 4;
    }
    return pCollision_count;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00424140
int C2_HOOK_FASTCALL FUN_00424140(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7) {
    br_vector3 actor_translation;
    int face_index;
    tFace_ref* face;

    gCollision_face_mask__raycast = 0;
    BrMatrix34TApplyV(&actor_translation, (const br_vector3*)&pObject->actor->t.t.mat.m[3], &pObject->actor->t.t.mat);
    pBounds_shape->common.field_0x1c.min.v[0] += actor_translation.v[0];
    pBounds_shape->common.field_0x1c.min.v[1] += actor_translation.v[1];
    pBounds_shape->common.field_0x1c.min.v[2] += actor_translation.v[2];
    pBounds_shape->common.field_0x1c.max.v[0] += actor_translation.v[0];
    pBounds_shape->common.field_0x1c.max.v[1] += actor_translation.v[1];
    pBounds_shape->common.field_0x1c.max.v[2] += actor_translation.v[2];

    face_index = (int)pObject->box_face_start;
    face = &gFace_list__car[face_index];
    if (face_index < pObject->box_face_end) {
        do {
            if ((face->flags & 0x80) == 0) {
                if ((face->flags & 1) == 0) {
                    pCollision_count = FUN_00424270((float*)&face->v[0], (float*)&face->v[1], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
                if ((face->flags & 2) == 0) {
                    pCollision_count = FUN_00424270((float*)&face->v[1], (float*)&face->v[2], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
                if ((face->flags & 4) == 0) {
                    pCollision_count = FUN_00424270((float*)&face->v[2], (float*)&face->v[0], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
            }
            face_index++;
            face++;
        } while (face_index < pObject->box_face_end);
    }
    return pCollision_count;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00424270
int C2_HOOK_FASTCALL FUN_00424270(float* pStart, float* pEnd, int* volatile param_3, int param_4, undefined4 param_5, int param_6, int param_7, int param_8) {
    /* original stack frame layout (0x170..0x0 below the frame top): the raw code addresses vectors across
     * neighbouring locals, so they must stay contiguous and in this order */
    float pfx_frame[123];
#define local_170 (pfx_frame[0])
#define local_16c (pfx_frame[1])
#define local_168 (*(tU32*)&pfx_frame[2])
#define local_164 (*(tU32*)&pfx_frame[3])
#define local_160 (pfx_frame[4])
#define local_15c (pfx_frame[5])
#define local_158 (pfx_frame[6])
#define local_154 (pfx_frame[7])
#define local_150 (pfx_frame[8])
#define local_14c (pfx_frame[9])
#define local_148 (pfx_frame[10])
#define local_144 (pfx_frame[11])
#define local_140 (pfx_frame[12])
#define local_13c (pfx_frame[13])
#define local_138 (pfx_frame[14])
#define local_134 (pfx_frame[15])
#define local_130 (*(float**)&pfx_frame[16])
#define local_12c (pfx_frame[17])
#define local_128 (pfx_frame[18])
#define local_124 (pfx_frame[19])
#define local_120 (&pfx_frame[20])
#define local_114 (pfx_frame[23])
#define local_110 (pfx_frame[24])
#define local_10c (pfx_frame[25])
#define local_108 (&pfx_frame[26])
#define local_fc (&pfx_frame[29])
#define local_f0 (&pfx_frame[32])
#define local_e4 (&pfx_frame[35])
#define local_d8 (&pfx_frame[38])
#define local_cc (&pfx_frame[41])
#define local_c0 (*(int*)&pfx_frame[44])
#define local_ac (&pfx_frame[49])
#define local_a0 (&pfx_frame[52])
#define local_6c (pfx_frame[65])
#define local_60 (*(int*)&pfx_frame[68])
#define local_4c (&pfx_frame[73])
#define local_40 (&pfx_frame[76])
#define local_c (&pfx_frame[89])
    float fVar1;
    float fVar2;
    tU32 uVar3;
    int uVar4;
    float* pfVar5;
    tU32 uVar6;
    int iVar7;
    tU32 uVar8;
    float* pfVar9;






































    local_168 = 1;
    local_164 = 1;
    pfVar9 = (float*)(param_4 + 0x28);
    iVar7 = 0;
    pfVar5 = (float*)(*(int*)param_3 + 0x34);
    while (1) {
        fVar1 = pStart[1] * pfVar5[-1] + pStart[2] * *pfVar5 + pfVar5[-2] * *pStart;
        fVar2 = pfVar9[-3] - gDouble_005899c8;
        local_16c = pEnd[1] * pfVar5[-1] + pEnd[2] * *pfVar5 + pfVar5[-2] * *pEnd;
        if (fVar1 < fVar2 && local_16c < fVar2) {
            return param_6;
        }
        local_170 = *pfVar9 - gDouble_005899d0;
        local_168 = local_168 | (tU32)(fVar1 < fVar2) | (tU32)(fVar1 < local_170);
        local_164 = local_164 | (tU32)(local_16c < fVar2) | (tU32)(local_16c < local_170);
        if (local_170 < fVar1 && local_170 < local_16c) {
            break;
        }
        iVar7++;
        pfVar9++;
        pfVar5 += 3;
        if (iVar7 > 2) {
            local_130 = pEnd;
            if (*(char*)((tU8*)param_3 + 0xed) == 0) {
                if (local_168 != 0) {
                    local_148 = *pStart - *(float*)((tU8*)param_3 + 0xb0);
                    local_144 = pStart[1] - *(float*)((tU8*)param_3 + 0xb4);
                    local_140 = pStart[2] - *(float*)((tU8*)param_3 + 0xb8);
                    BrMatrix34TApplyV((br_vector3*)local_fc, (br_vector3*)&local_148, (br_matrix34*)((tU8*)param_3 + 0x8c));
                    local_160 = *pStart - *(float*)(*(int*)param_3 + 0x50);
                    local_15c = pStart[1] - *(float*)(*(int*)param_3 + 0x54);
                    local_158 = pStart[2] - *(float*)(*(int*)param_3 + 0x58);
                    BrMatrix34TApplyV((br_vector3*)local_f0, (br_vector3*)&local_160, (br_matrix34*)(*(int*)param_3 + 0x2c));
                    iVar7 = ShapeRayCastWithSphere((br_vector3*)local_fc, (br_vector3*)local_f0, (tPhysics_shape*)param_4, (br_vector3*)local_120, (br_vector3*)local_108, &local_170);
                    if (0 < iVar7) {
                        if (0x40 < iVar7) {
                            local_170 = 1.1f;
                        }
                        param_6 = FUN_00422fe0((float*)param_5, local_120, local_170, local_108, 0, param_6, (void*)param_3, 2, (void*)param_7, NULL);
                    }
                }
                if (local_164 != 0) {
                    local_160 = *pEnd - *(float*)((tU8*)param_3 + 0xb0);
                    local_15c = pEnd[1] - *(float*)((tU8*)param_3 + 0xb4);
                    local_158 = pEnd[2] - *(float*)((tU8*)param_3 + 0xb8);
                    BrMatrix34TApplyV((br_vector3*)local_108, (br_vector3*)&local_160, (br_matrix34*)((tU8*)param_3 + 0x8c));
                    local_148 = *pEnd - *(float*)(*(int*)param_3 + 0x50);
                    local_144 = pEnd[1] - *(float*)(*(int*)param_3 + 0x54);
                    local_140 = pEnd[2] - *(float*)(*(int*)param_3 + 0x58);
                    BrMatrix34TApplyV((br_vector3*)local_120, (br_vector3*)&local_148, (br_matrix34*)(*(int*)param_3 + 0x2c));
                    iVar7 = ShapeRayCastWithSphere((br_vector3*)local_108, (br_vector3*)local_120, (tPhysics_shape*)param_4, (br_vector3*)local_f0, (br_vector3*)local_fc, &local_170);
                    if (0 < iVar7) {
                        if (0x40 < iVar7) {
                            local_170 = 1.1f;
                        }
                        param_6 = FUN_00422fe0((float*)param_5, local_f0, local_170, local_fc, 0, param_6, (void*)param_3, 2, (void*)param_7, NULL);
                    }
                }
            }
            local_114 = pEnd[0] - *pStart;
            local_110 = pEnd[1] - pStart[1];
            local_10c = pEnd[2] - pStart[2];
            uVar8 = 0;
            local_c0 = param_7;
            local_60 = param_7;
            local_cc[0] = gFLOAT_00589968;
            *(tU32*)&local_16c = 0xffffffff;
            local_170 = 0.0f;
            local_6c = gFLOAT_00589968;
            if (*(tU16*)((tU8*)param_8 + 0x702) != 0) {
                do {
                    uVar3 = (tU32)*(tU8*)(*(tU32*)((tU8*)param_8 + 0x704) + (int)local_170 * 2);
                    uVar6 = (tU32)*(tU8*)(*(tU32*)((tU8*)param_8 + 0x704) + (int)local_170 * 2 + 1);
                    iVar7 = uVar3 + 0x40;
                    pfVar9 = (float*)((tU8*)param_8 + uVar3 * 0xc);
                    local_12c = *(float*)((tU8*)param_8 + iVar7 * 0xc) - *pfVar9;
                    pfVar5 = (float*)((tU8*)param_8 + uVar6 * 0xc);
                    local_128 = pfVar9[0xc1] - pfVar9[1];
                    local_124 = pfVar9[0xc2] - pfVar9[2];
                    local_13c = *pfVar5 - *pfVar9;
                    local_138 = pfVar5[1] - pfVar9[1];
                    local_134 = pfVar5[2] - pfVar9[2];
                    local_154 = (*(float*)((tU8*)param_8 + (uVar6 + 0x40) * 0xc) - *(float*)((tU8*)param_8 + iVar7 * 0xc)) - local_13c;
                    local_150 = (pfVar5[0xc1] - pfVar9[0xc1]) - local_138;
                    local_14c = (pfVar5[0xc2] - pfVar9[0xc2]) - local_134;
                    iVar7 = FUN_00424990(pStart, &local_114, pfVar9, &local_13c, &local_12c, &local_154, (float*)((tU8*)local_cc + uVar8 * 0x60), param_6, (int)param_3, 0);
                    if (iVar7 != 0) {
                        local_16c = local_170;
                        if (local_cc[uVar8 * 0x18] < local_cc[(tU32)(uVar8 == 0) * 0x18]) {
                            uVar8 = (tU32)(uVar8 == 0);
                        }
                    }
                    local_170 = (float)((int)local_170 + 1);
                    pEnd = local_130;
                } while ((int)local_170 < (int)(tU32)*(tU16*)((tU8*)param_8 + 0x702));
            }
            if (gFLOAT_00589968 <= local_cc[0]) {
                return param_6;
            }
            if (((float*)(param_7 + 0x34))[0] * ((float*)local_cc)[14] + ((float*)(param_7 + 0x38))[0] * ((float*)local_cc)[15] + ((float*)(param_7 + 0x3c))[0] * ((float*)local_cc)[16] < gFLOAT_00589978) {
                local_160 = *pStart - *(float*)((tU8*)param_3 + 0xb0);
                local_15c = pStart[1] - *(float*)((tU8*)param_3 + 0xb4);
                local_158 = pStart[2] - *(float*)((tU8*)param_3 + 0xb8);
                BrMatrix34TApplyV((br_vector3*)local_d8, (br_vector3*)&local_160, (br_matrix34*)((tU8*)param_3 + 0x8c));
                local_160 = *pEnd - *(float*)((tU8*)param_3 + 0xb0);
                local_15c = pEnd[1] - *(float*)((tU8*)param_3 + 0xb4);
                local_158 = pEnd[2] - *(float*)((tU8*)param_3 + 0xb8);
                BrMatrix34TApplyV((br_vector3*)local_e4, (br_vector3*)&local_160, (br_matrix34*)((tU8*)param_3 + 0x8c));
                iVar7 = ShapeRayCast((br_vector3*)local_d8, (br_vector3*)local_e4, (tPhysics_shape*)param_4, (br_vector3*)local_c, (float*)&local_168, NULL);
                if (iVar7 != 0) {
                    return param_6;
                }
            }
            gCollision_face_mask__raycast = gCollision_face_mask__raycast | 1 << ((int)local_16c & 0x1f);
            /* Temporary contact records store point at +0x14 and normal at +0x2c.
             * param_5 is the output list; param_8 is the shape-test scratch data. */
            uVar4 = FUN_00422fe0((float*)param_5, local_cc + 5, local_cc[0], local_a0, 0, param_6, (void*)param_3, 0, (void*)param_7, (float*)local_ac);
            if (local_6c < gFLOAT_00589968) {
                uVar4 = FUN_00422fe0((float*)param_5, (float*)&local_6c + 5, local_6c, local_40, 0, uVar4, (void*)param_3, 0, (void*)param_7, local_4c);
            }
            return uVar4;
        }
    }
    return param_6;
#undef local_170
#undef local_16c
#undef local_168
#undef local_164
#undef local_160
#undef local_15c
#undef local_158
#undef local_154
#undef local_150
#undef local_14c
#undef local_148
#undef local_144
#undef local_140
#undef local_13c
#undef local_138
#undef local_134
#undef local_130
#undef local_12c
#undef local_128
#undef local_124
#undef local_120
#undef local_114
#undef local_110
#undef local_10c
#undef local_108
#undef local_fc
#undef local_f0
#undef local_e4
#undef local_d8
#undef local_cc
#undef local_c0
#undef local_ac
#undef local_a0
#undef local_6c
#undef local_60
#undef local_4c
#undef local_40
#undef local_c
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00424990
int C2_HOOK_FASTCALL FUN_00424990(float* p1, float* p2, float* p3, float* p4, float* p5, float* p6, float* p7, undefined4 p8, int p9, int p10) {
    br_vector3 cross1;
    br_vector3 cross2;
    br_vector3 normal;
    br_vector3 temp;
    float local_18[6];
    float diff_x;
    float diff_y;
    float diff_z;
    float sep_x;
    float sep_y;
    float sep_z;
    float a;
    float b;
    float c;
    float bb;
    float cc;
    float root;
    float two_a;
    float t;
    float s;
    float u;
    float length;
    float f;
    float dot;
    float tx;
    float ty;
    float tz;
    float sx;
    float sy;
    float sz;
    float cu;
    float cw;
    int axis;

    cross1.v[0] = p2[2] * p4[1] - p4[2] * p2[1];
    cross1.v[1] = p4[2] * p2[0] - p2[2] * p4[0];
    cross1.v[2] = p2[1] * p4[0] - p4[1] * p2[0];
    cross2.v[0] = p2[2] * p6[1] - p2[1] * p6[2];
    cross2.v[1] = p6[2] * p2[0] - p2[2] * p6[0];
    cross2.v[2] = p2[1] * p6[0] - p6[1] * p2[0];
    diff_x = p3[0] - p1[0];
    diff_y = p3[1] - p1[1];
    a = cross2.v[0] * p5[0] + p5[2] * cross2.v[2] + p5[1] * cross2.v[1];
    b = (cross1.v[0] * p5[0] + p5[2] * cross1.v[2] + p5[1] * cross1.v[1]
            + cross2.v[0] * p3[0] + p3[1] * cross2.v[1] + p3[2] * cross2.v[2])
        - (cross1.v[0] * p2[0] + p2[1] * cross1.v[1] + p2[2] * cross1.v[2]
            + cross2.v[0] * p1[0] + p1[1] * cross2.v[1] + p1[2] * cross2.v[2]);
    diff_z = p3[2] - p1[2];
    c = diff_z * cross1.v[2] + diff_y * cross1.v[1] + diff_x * cross1.v[0];
    if (a < gFLOAT_00589978) {
        a = -a;
        b = -b;
        c = -c;
    }
    bb = b * b;
    cc = c * a * gFLOAT_005899dc;
    if (bb < fabsf(cc) * gFLOAT_005899e0) {
        bb = bb - cc;
        if (bb < gFLOAT_00589978) {
            goto fallback;
        }
        root = sqrtf(bb);
        two_a = a + a;
        t = (-b - root) / two_a;
        if (gFLOAT_0058996c < t) {
            goto fallback;
        }
        if (gFLOAT_00589978 <= t) {
            normal.v[0] = t * cross2.v[0] + cross1.v[0];
            normal.v[1] = t * cross2.v[1] + cross1.v[1];
            normal.v[2] = t * cross2.v[2] + cross1.v[2];
            if (normal.v[2] * normal.v[2] + normal.v[1] * normal.v[1] + normal.v[0] * normal.v[0] < gDouble_005899e8) {
                goto fallback;
            }
        }
        if (t < gFLOAT_00589978) {
            t = (root - b) / two_a;
            goto check_t;
        }
    } else {
        if (fabsf(b) <= gDouble_005899e8) {
            goto fallback;
        }
        t = -(c / b) - ((c * c * a) / (bb * b)) * gFLOAT_005899f0;
        if (t < gFLOAT_00589978 || gFLOAT_0058996c < t || gSensible_place_collect_active != 0) {
            t = -(b / a) - t;
        }
check_t:
        if (t < gFLOAT_00589978 || gFLOAT_0058996c < t) {
            goto fallback;
        }
        normal.v[0] = t * cross2.v[0] + cross1.v[0];
        normal.v[1] = t * cross2.v[1] + cross1.v[1];
        normal.v[2] = t * cross2.v[2] + cross1.v[2];
        if (normal.v[2] * normal.v[2] + normal.v[1] * normal.v[1] + normal.v[0] * normal.v[0] < gDouble_005899e8) {
            goto fallback;
        }
    }
    if (p7[0] <= t) {
        return 0;
    }
    length = sqrtf(normal.v[0] * normal.v[0] + normal.v[1] * normal.v[1] + normal.v[2] * normal.v[2]);
    if (length > gFLOAT_005899a8) {
        length = (float)gDouble_005899b0 / length;
        normal.v[0] = length * normal.v[0];
        normal.v[1] = length * normal.v[1];
        normal.v[2] = length * normal.v[2];
    } else {
        normal.v[0] = 1.0f;
        normal.v[1] = 0.0f;
        normal.v[2] = 0.0f;
    }
    local_18[0] = p6[0] * t + p4[0];
    local_18[1] = p6[1] * t + p4[1];
    local_18[2] = p6[2] * t + p4[2];
    local_18[3] = t * p5[0] + p3[0];
    local_18[4] = p5[1] * t + p3[1];
    local_18[5] = p5[2] * t + p3[2];
    dot = local_18[0] * p2[0] + p2[2] * local_18[2] + p2[1] * local_18[1];
    cu = -(p2[0] * p2[0] + p2[2] * p2[2] + p2[1] * p2[1]);
    tx = local_18[0] * cu + dot * p2[0];
    sep_x = p1[0] - local_18[3];
    sep_y = p1[1] - local_18[4];
    ty = local_18[1] * cu + p2[1] * dot;
    sep_z = p1[2] - local_18[5];
    tz = local_18[2] * cu + p2[2] * dot;
    s = (sep_z * tz + sep_y * ty + sep_x * tx) / (local_18[2] * tz + local_18[1] * ty + local_18[0] * tx);
    if (gFLOAT_00589978 <= s && s <= gFLOAT_0058996c) {
        axis = fabsf(p2[0]) < fabsf(p2[1]);
        if (fabsf(p2[axis]) < fabsf(p2[2])) {
            axis = 2;
        }
        u = ((local_18[axis] * s + local_18[axis + 3]) - p1[axis]) / p2[axis];
        if (gFLOAT_00589978 <= u && u <= gFLOAT_0058996c) {
            if (diff_z * cross1.v[2] + diff_y * cross1.v[1] + diff_x * cross1.v[0] < gFLOAT_00589978) {
                normal.v[0] = -normal.v[0];
                normal.v[1] = -normal.v[1];
                normal.v[2] = -normal.v[2];
            }
            if (p10 != 0) {
                BrMatrix34ApplyV((br_vector3*)&p7[0xb], &normal, &((tPhysics_object*)p10)->transform_matrix);
                BrMatrix34ApplyV(&temp, &normal, &((tPhysics_object*)p10)->actor->t.t.mat);
                f = gFLOAT_0058996c - t;
                sx = temp.v[0] * f + p7[0xb] * t;
                sy = temp.v[1] * f + p7[0xc] * t;
                sz = temp.v[2] * f + p7[0xd] * t;
                length = sqrtf(sz * sz + sy * sy + sx * sx);
                if (length > gFLOAT_005899a8) {
                    length = (float)gDouble_005899b0 / length;
                    p7[0xb] = length * sx;
                    p7[0xc] = length * sy;
                    p7[0xd] = length * sz;
                } else {
                    p7[0xb] = 1.0f;
                    p7[0xc] = 0.0f;
                    p7[0xd] = 0.0f;
                }
                p7[8] = u * p2[0] + p1[0];
                p7[9] = p2[1] * u + p1[1];
                p7[10] = p2[2] * u + p1[2];
                temp.v[0] = s * p4[0] + p3[0];
                temp.v[1] = p4[1] * s + p3[1];
                temp.v[2] = p4[2] * s + p3[2];
                BrMatrix34ApplyP((br_vector3*)&local_18[0], &temp, &((tPhysics_object*)p10)->transform_matrix);
                local_18[3] = local_18[0] - ((tPhysics_object*)p9)->transform_matrix.m[3][0];
                local_18[4] = local_18[1] - ((tPhysics_object*)p9)->transform_matrix.m[3][1];
                local_18[5] = local_18[2] - ((tPhysics_object*)p9)->transform_matrix.m[3][2];
                BrMatrix34TApplyV((br_vector3*)&p7[5], (br_vector3*)&local_18[3], &((tPhysics_object*)p9)->transform_matrix);
                p7[0] = t;
                return 1;
            }
            BrMatrix34TApplyV((br_vector3*)&p7[0xb], &normal, &((tPhysics_object*)p9)->transform_matrix);
            p7[0xe] = normal.v[0];
            p7[0xf] = normal.v[1];
            p7[0x10] = normal.v[2];
            temp.v[0] = s * p4[0] + p3[0];
            temp.v[1] = p4[1] * s + p3[1];
            temp.v[2] = p4[2] * s + p3[2];
            local_18[3] = temp.v[0] - ((tPhysics_object*)p9)->transform_matrix.m[3][0];
            local_18[4] = temp.v[1] - ((tPhysics_object*)p9)->transform_matrix.m[3][1];
            local_18[5] = temp.v[2] - ((tPhysics_object*)p9)->transform_matrix.m[3][2];
            BrMatrix34TApplyV((br_vector3*)&p7[5], (br_vector3*)&local_18[3], &((tPhysics_object*)p9)->transform_matrix);
            p7[8] = u * p2[0];
            p7[9] = p2[1] * u;
            p7[10] = p2[2] * u;
            p7[8] = p1[0] + p7[8];
            p7[9] = p7[9] + p1[1];
            cw = p1[2];
            p7[0] = t;
            p7[10] = p7[10] + cw;
            return 1;
        }
    }

fallback:
    return FUN_00425630(&cross1, &cross2, (const br_vector3*)p1, (const br_vector3*)p2, (const br_vector3*)p3, (const br_vector3*)p4,
        (const br_vector3*)p5, (const br_vector3*)p6, p7, p8, (const tPhysics_object*)p9, (const tPhysics_object*)p10);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00425630
int C2_HOOK_FASTCALL FUN_00425630(const br_vector3* param_1, const br_vector3* param_2, register const br_vector3* param_3, register const br_vector3* param_4, const br_vector3* param_5, const br_vector3* param_6, const br_vector3* param_7, const br_vector3* param_8, register float* param_9, undefined4 param_10, const tPhysics_object* param_11, const tPhysics_object* param_12) {
    // The x87 code stores several values as float AND keeps the un-rounded value in an FPU register
    // for later use. The double locals below model those register copies (the CRT runs the FPU at 53 bit).
    float sum_dir[3];     // p1 + p2
    double sum_dir_ext[3];
    float len;
    double len_ext;
    float dir[3];         // (p5 + p7) - p3
    double dir_ext[3];
    float normal[3];      // sum_dir / len
    double normal_ext[3];
    float rel[3];         // p8 + p6
    double rel_x_ext;
    double cross[3];
    double denom;
    volatile float t;
    volatile float u; // must be rounded to float before use (the original reloads it from its stack slot)
    float qy;
    volatile float w_y;
    volatile float w_z;
    double dot;
    float point[3];
    float local_pos[3];
    float world_pos[3];
    int i;

    if (!(gDouble_005899b0 < *param_9)) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        sum_dir_ext[i] = param_1->v[i] + param_2->v[i];
        sum_dir[i] = (float)sum_dir_ext[i];
    }
    len_ext = sqrt(sum_dir_ext[0] * sum_dir[0] + sum_dir_ext[1] * sum_dir[1] + sum_dir_ext[2] * sum_dir[2]);
    len = (float)len_ext;
    if (!(gDouble_005899e8 < len_ext)) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        dir_ext[i] = (param_5->v[i] + param_7->v[i]) - param_3->v[i];
        dir[i] = (float)dir_ext[i];
    }
    if (!(fabs((dir_ext[0] * sum_dir[0] + dir_ext[1] * sum_dir[1] + dir_ext[2] * sum_dir[2]) / len) < gDouble_005899f8)) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        normal_ext[i] = sum_dir[i] / len;
        normal[i] = (float)normal_ext[i];
    }
    rel_x_ext = param_8->v[0] + param_6->v[0];
    rel[0] = (float)rel_x_ext;
    rel[1] = param_8->v[1] + param_6->v[1];
    rel[2] = param_8->v[2] + param_6->v[2];
    cross[0] = param_4->v[1] * normal[2] - param_4->v[2] * normal[1];
    cross[1] = param_4->v[2] * normal[0] - normal_ext[2] * param_4->v[0];
    cross[2] = normal_ext[1] * param_4->v[0] - param_4->v[1] * normal[0];
    denom = rel_x_ext * cross[0] + rel[1] * cross[1] + rel[2] * cross[2];
    if (!(gDouble_005899e8 < fabs(denom))) {
        return 0;
    }
    t = (float)-((cross[0] * dir[0] + cross[1] * dir[1] + cross[2] * dir[2]) / denom);
    if (t < gFLOAT_00589978) {
        return 0;
    }
    if (t > gFLOAT_0058996c) {
        return 0;
    }
    w_y = (float)(t * rel[1] + dir[1]);
    w_z = (float)(t * rel[2] + dir[2]);
    u = (float)((param_4->v[1] * w_y + param_4->v[2] * w_z + (t * rel[0] + dir[0]) * param_4->v[0])
        / (param_4->v[1] * param_4->v[1] + param_4->v[2] * param_4->v[2] + param_4->v[0] * param_4->v[0]));
    if (u < gFLOAT_00589978) {
        return 0;
    }
    if (u > gFLOAT_0058996c) {
        return 0;
    }
    *param_9 = 1.0f;
    qy = (float)(param_5->v[1] - param_3->v[1]);
    if (param_1->v[1] * qy + param_1->v[2] * (param_5->v[2] - param_3->v[2]) + (param_5->v[0] - param_3->v[0]) * param_1->v[0] < gFLOAT_00589978) {
        normal[0] = -normal[0];
        normal[1] = -normal[1];
        normal[2] = -normal[2];
    }
    dot = dir_ext[0] * normal[0] + dir_ext[1] * normal[1] + dir_ext[2] * normal[2];
    if (dot < gFLOAT_00589978) {
        normal[0] = -normal[0];
        normal[1] = -normal[1];
        normal[2] = -normal[2];
    }
    if (param_12 != NULL) {
        point[0] = t * param_6->v[0] + param_5->v[0];
        point[1] = param_6->v[1] * t + param_5->v[1];
        point[2] = param_6->v[2] * t + param_5->v[2];
        BrMatrix34ApplyP((br_vector3*)world_pos, (br_vector3*)point, &param_12->transform_matrix);
        local_pos[0] = world_pos[0] - param_11->transform_matrix.m[3][0];
        local_pos[1] = world_pos[1] - param_11->transform_matrix.m[3][1];
        local_pos[2] = world_pos[2] - param_11->transform_matrix.m[3][2];
        BrMatrix34TApplyV((br_vector3*)&param_9[5], (br_vector3*)local_pos, &param_11->transform_matrix);
        BrMatrix34ApplyV((br_vector3*)&param_9[11], (br_vector3*)normal, &param_12->transform_matrix);
    } else {
        if (fabs(dot) < gDouble_00589a00) {
            *param_9 = 1.1f;
        }
        // Here the x87 code keeps the un-rounded t * p6 + p5 in a register for the subtraction.
        local_pos[0] = (t * param_6->v[0] + param_5->v[0]) - param_11->transform_matrix.m[3][0];
        local_pos[1] = (param_6->v[1] * t + param_5->v[1]) - param_11->transform_matrix.m[3][1];
        local_pos[2] = (param_6->v[2] * t + param_5->v[2]) - param_11->transform_matrix.m[3][2];
        BrMatrix34TApplyV((br_vector3*)&param_9[5], (br_vector3*)local_pos, &param_11->transform_matrix);
        BrMatrix34TApplyV((br_vector3*)&param_9[11], (br_vector3*)normal, &param_11->transform_matrix);
        param_9[14] = normal[0];
        param_9[15] = normal[1];
        param_9[16] = normal[2];
    }
    param_9[8] = u * param_4->v[0] + param_3->v[0];
    param_9[9] = param_4->v[1] * u + param_3->v[1];
    param_9[10] = param_4->v[2] * u + param_3->v[2];
    return 1;
}
#pragma auto_inline(on)
