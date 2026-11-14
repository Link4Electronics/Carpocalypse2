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


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34LPInverse();
extern void C2_HOOK_CDECL BrMatrix34Mul();
extern void C2_HOOK_CDECL BrMatrix34TApplyV();
extern void C2_HOOK_CDECL BrVector3SetFloat();
extern int C2_HOOK_FASTCALL FUN_00427fd0(tPhysics_object* pObj1, tPhysics_object* pObj2, tPhysics_shape* pEdgeShape, tPhysics_shape* pRayShape, int pVertices, float* pRecords, int pRecordCount, undefined4 pUnused, const tU8* pEdge_visibility, int* pCollided, float* pNormal);
extern int C2_HOOK_FASTCALL FUN_00428530(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pCorner_shape, tPhysics_shape* pShape, float pT, int pEdge_count, float* pRecords, int pRecord_count, undefined4 pArg9, tU8* pEdge_visibility);
extern int C2_HOOK_FASTCALL FUN_004287a0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, void* pRecords, int pRecord_count, undefined4 pArg11, const tU8* pVisibility1, const tU8* pVisibility2);
extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);
extern int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT);
extern void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pPoint, br_vector3* pSphere_center, br_scalar pRadius, br_vector3* pNormal, br_scalar* pCollision_t);
extern int C2_HOOK_FASTCALL RayCastInBox(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, tPhysics_object* pObject);
extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);
extern br_uint_8 bl;
extern char gDouble_0058a008[];
extern char gDouble_0058a010[];
extern char gDouble_0058a018[];
extern tFace_ref gFace_list__car[300];
extern br_actor* gTrack_actor;
extern float gUNK_00589fb0;
extern float gUNK_00589fb8;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
#endif
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
void C2_NAKED C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0xa0
            fld dword ptr [edx + 0x38]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0xb0]
            mov ebx, ecx
            fld dword ptr [ebp + 4]
            fld dword ptr [edx + 0x34]
            fld dword ptr [ebp + 8]
            fld dword ptr [edx + 0x3c]
            fxch st(2)
            _emit 0xd8
        _emit 0x4d
        _emit 0x00
            fxch st(3)
            fmulp st(4), st(0)
            fxch st(3)
            faddp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            push esi
            mov esi, dword ptr [edx]
            test ebx, ebx
            faddp st(1), st(0)
            push edi
            mov dword ptr [esp + 0x2c], ebx
            fstp dword ptr [esp + 0x20]
            jle L0045f123
            mov edi, dword ptr [esp + 0xc0]
            mov eax, 0x42c80000
            rep stosd
    L0045f123:
            test esi, esi
            je L0045f16a
            mov eax, dword ptr [esi + 0x20]
            test ah, 0x18
            jne L0045f144
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            je L0045f5a0
    L0045f144:
            test esi, esi
            je L0045f16a
            mov esi, dword ptr [esi + 4]
            test esi, esi
            je L0045f16a
            mov al, byte ptr [esi]
            cmp al, 0x21
            je L0045f15d
            cmp al, 0x3e
            je L0045f15d
            cmp al, 0x3f
            jne L0045f16a
    L0045f15d:
            mov eax, dword ptr [gPling_materials]
            test eax, eax
            jne L0045f5a0
    L0045f16a:
            fld dword ptr [esp + 0x20]
            fabs
            fst dword ptr [esp + 0x14]
            fcomp dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 1
            jne L0045f5a0
            test ebx, ebx
            mov dword ptr [esp + 0x1c], 0
            jle L0045f2ff
            mov eax, dword ptr [esp + 0xb4]
            lea ebx, [esp + 0x60]
            lea edi, [esp + 0x50]
            lea esi, [esp + 0x64]
            lea ecx, [eax + 8]
            sub ebx, eax
    L0045f1ad:
            fld dword ptr [ecx - 4]
            _emit 0xd9
        _emit 0x01
            fsub dword ptr [edx + 0xc]
            fxch st(1)
            fsub dword ptr [edx + 8]
            fld dword ptr [edx + 0x3c]
            fld dword ptr [edx + 0x38]
            fld dword ptr [ecx - 8]
            fsub dword ptr [edx + 4]
            fxch st(2)
            fmulp st(4), st(0)
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fmulp st(3), st(0)
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            fcomp dword ptr [gUNK_00589fb0]
            fmul dword ptr [edx + 0x34]
            fnstsw ax
            faddp st(1), st(0)
            test ah, 1
            fstp dword ptr [esp + 0x10]
            je L0045f20f
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [gUNK_00589fb8+0x4]
            fxch st(1)
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f5a0
    L0045f20f:
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            je L0045f253
            fld dword ptr [esp + 0x10]
            fcomp qword ptr [gDouble_0058a018+0x18]
            fnstsw ax
            test ah, 1
            jne L0045f2d7
            fld dword ptr [esp + 0x20]
            fsubr qword ptr [gDouble_0058a018+0x8]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L0045f279
            jmp L0045f2d7
    L0045f253:
            fld dword ptr [esp + 0x10]
            fchs
            fcom qword ptr [gDouble_0058a018+0x18]
            fnstsw ax
            test ah, 1
            jne L0045f2d5
            fld dword ptr [esp + 0x20]
            fsub qword ptr [gDouble_0058a018+0x10]
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f2d7
    L0045f279:
            fld dword ptr [esp + 0x10]
            fdiv dword ptr [esp + 0x20]
            fchs
            fcom dword ptr [gUNK_00589fb0]
            _emit 0xd9
        _emit 0x1f
            fnstsw ax
            test ah, 0x41
            jne L0045f298
            mov dword ptr [edi], 0x3f800000
    L0045f298:
            _emit 0xd9
        _emit 0x07
            _emit 0xd9
        _emit 0x07
            fld dword ptr [ebp + 4]
            _emit 0xd9
        _emit 0x07
            fld dword ptr [ebp + 8]
            fxch st(4)
            _emit 0xd8
        _emit 0x4d
        _emit 0x00
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(3)
            fmulp st(3), st(0)
            fstp dword ptr [esi - 4]
            _emit 0xd9
        _emit 0x1e
            fstp dword ptr [ebx + ecx]
            fld dword ptr [esi - 4]
            fadd dword ptr [ecx - 8]
            fld dword ptr [ecx - 4]
            fxch st(1)
            fstp dword ptr [esi - 4]
            _emit 0xd8
        _emit 0x06
            _emit 0xd9
        _emit 0x1e
            fld dword ptr [ebx + ecx]
            _emit 0xd8
        _emit 0x01
            fstp dword ptr [ebx + ecx]
            jmp L0045f2dd
    L0045f2d5:
            fstp st(0)
    L0045f2d7:
            mov dword ptr [edi], 0x42c80000
    L0045f2dd:
            mov eax, dword ptr [esp + 0x1c]
            add esi, 0xc
            inc eax
            add ecx, 0xc
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0x2c]
            add edi, 4
            cmp dword ptr [esp + 0x1c], eax
            jl L0045f1ad
            mov ebx, eax
    L0045f2ff:
            fld dword ptr [edx + 0x34]
            fld dword ptr [edx + 0x38]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x18]
            fcompp
            xor ecx, ecx
            fnstsw ax
            test ah, 1
            je L0045f31f
            mov ecx, 1
    L0045f31f:
            fld dword ptr [edx + ecx*4 + 0x34]
            fld dword ptr [edx + 0x3c]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x18]
            fcompp
            fnstsw ax
            test ah, 1
            je L0045f33e
            mov ecx, 2
    L0045f33e:
            sub ecx, 0
            je L0045f35b
            dec ecx
            je L0045f352
            dec ecx
            jne L0045f367
            xor eax, eax
            mov edi, 1
            jmp L0045f36f
    L0045f352:
            xor eax, eax
            mov edi, 2
            jmp L0045f36f
    L0045f35b:
            mov eax, 1
            mov edi, 2
            jmp L0045f36f
    L0045f367:
            mov eax, dword ptr [esp + 0x18]
            mov edi, dword ptr [esp + 0x18]
    L0045f36f:
            fld dword ptr [edx + eax*4 + 4]
            fld dword ptr [edx + edi*4 + 4]
            fld dword ptr [edx + eax*4 + 0x10]
            fld dword ptr [edx + edi*4 + 0x10]
            fld dword ptr [edx + eax*4 + 0x1c]
            fld dword ptr [edx + edi*4 + 0x1c]
            fxch st(5)
            fstp dword ptr [esp + 0x1c]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fxch st(3)
            fstp dword ptr [esp + 0x24]
            fsub dword ptr [esp + 0x24]
            fxch st(3)
            fsub dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(2)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x4c]
            test ebx, ebx
            fstp dword ptr [esp + 0x48]
            mov ecx, 0
            jle L0045f5a0
            mov ebp, dword ptr [esp + 0xc0]
            lea edi, [esp + edi*4 + 0x60]
            mov dword ptr [esp + 0x28], edi
            mov edi, dword ptr [esp + 0xbc]
            lea esi, [esp + 0x50]
            lea ebx, [esp + eax*4 + 0x60]
    L0045f3e8:
            cmp dword ptr [esi], 0x42c80000
            je L0045f57f
            fld dword ptr [esp + 0x48]
            fmul dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            mov eax, dword ptr [esp + 0x28]
            _emit 0xd9
        _emit 0x03
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fmul dword ptr [esp + 0x4c]
            fxch st(1)
            fsub dword ptr [esp + 0x1c]
            fxch st(2)
            fsub dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fxch st(4)
            fstp dword ptr [esp + 0x30]
            fxch st(3)
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x30]
            fstp dword ptr [esp + ecx*4 + 0xa0]
            fxch st(4)
            fst dword ptr [esp + 0x44]
            fxch st(2)
            fstp dword ptr [esp + 0x40]
            fxch st(1)
            fsub dword ptr [esp + 0x40]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(2)
            fsubp st(1), st(0)
            fxch st(1)
            fabs
            fmul qword ptr [gDouble_0058a018]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fabs
            fxch st(1)
            fcompp
            fld dword ptr [esp + 0x34]
            fstp dword ptr [esp + ecx*4 + 0x90]
            fnstsw ax
            test ah, 1
            jne L0045f57f
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045f57f
            fld dword ptr [esp + 0x14]
            fdiv dword ptr [esp + 0x10]
            fst dword ptr [esp + 0x18]
            fcomp qword ptr [gDouble_0058a010]
            fnstsw ax
            test ah, 1
            jne L0045f57f
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fsub dword ptr [esp + 0x44]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fsubp st(2), st(0)
            fabs
            fmul qword ptr [gDouble_0058a018]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f57f
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045f57f
            fld dword ptr [esp + 0x14]
            fdiv dword ptr [esp + 0x10]
            fcom qword ptr [gDouble_0058a010]
            fnstsw ax
            test ah, 1
            jne L0045f57d
            fld dword ptr [esp + 0x18]
            fadd st(0), st(1)
            fcomp qword ptr [gDouble_0058a018]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L0045f57f
            mov eax, dword ptr [esi]
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fac]
            mov dword ptr [ebp], eax
            mov eax, dword ptr [edx + 0x34]
            mov dword ptr [edi], eax
            mov eax, dword ptr [edx + 0x38]
            mov dword ptr [edi + 4], eax
            mov eax, dword ptr [edx + 0x3c]
            mov dword ptr [edi + 8], eax
            fnstsw ax
            test ah, 0x41
            jne L0045f57f
            fld dword ptr [edx + 0x34]
            fchs
            _emit 0xd9
        _emit 0x1f
            fld dword ptr [edx + 0x38]
            fchs
            fstp dword ptr [edi + 4]
            fld dword ptr [edx + 0x3c]
            fchs
            fstp dword ptr [edi + 8]
            jmp L0045f57f
    L0045f57d:
            fstp st(0)
    L0045f57f:
            mov eax, dword ptr [esp + 0x28]
            inc ecx
            add eax, 0xc
            add ebx, 0xc
            mov dword ptr [esp + 0x28], eax
            mov eax, dword ptr [esp + 0x2c]
            add esi, 4
            add ebp, 4
            cmp ecx, eax
            jl L0045f3e8
    L0045f5a0:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xa0
            ret 0x10
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045f5b0
void C2_NAKED C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x30
            push ebx
            mov ebx, dword ptr [esp + 0x38]
            push esi
            push edi
            mov esi, edx
            mov edi, ecx
            push ebx
            push esi
            push edi
            call BrMatrix34ApplyP
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [edi + 4]
            mov edx, dword ptr [edi + 8]
            mov dword ptr [edi + 0xc], eax
            mov dword ptr [edi + 0x10], ecx
            mov dword ptr [edi + 0x14], edx
            fld dword ptr [esi + 0xc]
            fsub dword ptr [esi]
            add esp, 0xc
            lea eax, [ebx + 8]
            lea ecx, [esp + 0xc]
            lea edx, [esp + 0x1c]
            fstp dword ptr [esp + 0xc]
            fld dword ptr [esi + 0x10]
            fsub dword ptr [esi + 4]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esi + 0x14]
            fsub dword ptr [esi + 8]
            lea esi, [esp + 0x18]
            sub esi, ebx
            mov ebx, 3
            fstp dword ptr [esp + 0x14]
    L0045f60d:
            fld dword ptr [ecx]
            fld dword ptr [eax - 8]
            fld dword ptr [eax - 4]
            fld dword ptr [ecx]
            fld dword ptr [eax]
            fxch st(4)
            fmulp st(3), st(0)
            fxch st(1)
            fmul dword ptr [ecx]
            fxch st(2)
            fstp dword ptr [edx - 4]
            fxch st(1)
            fstp dword ptr [edx]
            fmulp st(1), st(0)
            add edx, 0xc
            add ecx, 4
            add eax, 0xc
            dec ebx
            fstp dword ptr [esi + eax - 0xc]
            jne L0045f60d
            lea ebx, [esp + 0x18]
            mov ecx, edi
            sub ebx, edi
            mov edi, 3
    L0045f649:
            lea edx, [ebx + ecx]
            mov esi, 3
    L0045f651:
            fld dword ptr [edx]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045f668
            fld dword ptr [ecx]
            fadd dword ptr [edx]
            fstp dword ptr [ecx]
            jmp L0045f670
    L0045f668:
            fld dword ptr [ecx + 0xc]
            fadd dword ptr [edx]
            fstp dword ptr [ecx + 0xc]
    L0045f670:
            add edx, 0xc
            dec esi
            jne L0045f651
            add ecx, 4
            dec edi
            jne L0045f649
            pop edi
            pop esi
            pop ebx
            add esp, 0x30
            ret 4
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045cd90
int C2_NAKED C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_scalar inverse;
    br_scalar t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] < -2 * BR_SCALAR_EPSILON) {
            inverse = rd->v[i];
            inverse = -1.f / inverse;
            t = (rp->v[i] - b->max.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
            t = (rp->v[i] - b->min.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] > 2 * BR_SCALAR_EPSILON) {
            inverse = rd->v[i];
            inverse = -1.f / inverse;
            t = (rp->v[i] - b->max.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
            t = (rp->v[i] - b->min.v[i]) * inverse;
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
    } else {
        return 0;
    }
#else
    __asm {
            fld dword ptr [esp + 8]
            push esi
            mov esi, dword ptr [esp + 8]
            push edi
            xor edi, edi
            sub esi, edx
            sub ecx, edx
    L0045cda0:
            fld dword ptr [esi + edx]
            fcomp dword ptr [gUNK_00589fb0+0x4]
            fld dword ptr [esi + edx]
            fnstsw ax
            test ah, 1
            je L0045ce38
            fdivr dword ptr [gUNK_00589fb8]
            _emit 0xd9
        _emit 0x02
            fsub dword ptr [ecx + edx + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fst dword ptr [esp + 0xc]
            fcomp dword ptr [gUNK_00589fb8+0x4]
            fnstsw ax
            test ah, 0x41
            jne L0045cde8
            fstp st(0)
            fld dword ptr [gUNK_00589fb8+0x4]
            jmp L0045cdfb
    L0045cde8:
            fld dword ptr [esp + 0xc]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0045cdfb
            fstp st(0)
            fld dword ptr [esp + 0xc]
    L0045cdfb:
            _emit 0xd9
        _emit 0x02
            fsub dword ptr [ecx + edx]
            fmul dword ptr [esp + 0x10]
            fcom dword ptr [gUNK_00589fb8+0x8]
            fnstsw ax
            test ah, 1
            je L0045ce20
            fstp st(0)
            mov dword ptr [esp + 0x14], 0xff7fffff
            jmp L0045cedc
    L0045ce20:
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            je L0045ceda
            fstp dword ptr [esp + 0x14]
            jmp L0045cedc
    L0045ce38:
            fcomp dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 0x41
            jne L0045cebf
            fld dword ptr [esi + edx]
            fdivr dword ptr [gUNK_00589fb8]
            _emit 0xd9
        _emit 0x02
            fsub dword ptr [ecx + edx + 0xc]
            fmul st(0), st(1)
            fcom dword ptr [gUNK_00589fb8+0x8]
            fnstsw ax
            test ah, 1
            je L0045ce6f
            fstp st(0)
            mov dword ptr [esp + 0x14], 0xff7fffff
            jmp L0045ce82
    L0045ce6f:
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            je L0045ce80
            fstp dword ptr [esp + 0x14]
            jmp L0045ce82
    L0045ce80:
            fstp st(0)
    L0045ce82:
            _emit 0xd9
        _emit 0x02
            fsub dword ptr [ecx + edx]
            fmul st(0), st(1)
            fstp dword ptr [esp + 0xc]
            fstp st(0)
            fld dword ptr [esp + 0xc]
            fcomp dword ptr [gUNK_00589fb8+0x4]
            fnstsw ax
            test ah, 0x41
            jne L0045ceaa
            fstp st(0)
            fld dword ptr [gUNK_00589fb8+0x4]
            jmp L0045cedc
    L0045ceaa:
            fld dword ptr [esp + 0xc]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0045cedc
            fstp st(0)
            fld dword ptr [esp + 0xc]
            jmp L0045cedc
    L0045cebf:
            _emit 0xd9
        _emit 0x02
            fcomp dword ptr [ecx + edx + 0xc]
            fnstsw ax
            test ah, 0x41
            je L0045cf0e
            _emit 0xd9
        _emit 0x02
            fcomp dword ptr [ecx + edx]
            fnstsw ax
            test ah, 1
            jne L0045cf0e
            jmp L0045cedc
    L0045ceda:
            fstp st(0)
    L0045cedc:
            inc edi
            add edx, 4
            cmp edi, 3
            jl L0045cda0
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 0x41
            je L0045cf0e
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x1c]
            _emit 0xd9
        _emit 0x18
            fld dword ptr [esp + 0x14]
            mov eax, 1
            _emit 0xd9
        _emit 0x19
            pop edi
            pop esi
            ret 0x14
    L0045cf0e:
            pop edi
            xor eax, eax
            fstp st(0)
            pop esi
            ret 0x14
    }
#endif
}

void C2_HOOK_FASTCALL DRVector2AccumulateScale__finteray(br_vector2* a, const br_vector2* b, br_scalar s) {

    a->v[0] = b->v[0] * s + a->v[0];
    a->v[1] = b->v[1] * s + a->v[1];
}

// FUNCTION: CARMA2_HW 0x0045cf60
int C2_NAKED C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg) {
#ifndef CARPOCALYPSE2_MATCHING
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
            if (fabsf(alpha) > fabsf(f_n)) {
 /* Verifies fabsf(alpha / f_n) < 1.f */
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
#else
    __asm {
            sub esp, 0x60
            fld dword ptr [esp + 0x70]
            fsub dword ptr [gUNK_00589fc4+0x4]
            mov eax, dword ptr [ecx + 0x4c]
            push ebx
            push ebp
            push esi
            push edi
            xor edi, edi
            fstp dword ptr [esp + 0x80]
            fld dword ptr [esp + 0x84]
            fsub dword ptr [gUNK_00589fc4+0x8]
            cmp word ptr [eax + 8], di
            mov dword ptr [esp + 0x58], edx
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edi
            fstp dword ptr [esp + 0x84]
            jbe L0045d55c
            mov ebp, dword ptr [esp + 0x7c]
            mov ebx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x2c], edi
    L0045cfb3:
            lea edx, [eax + 0x18]
            mov dword ptr [esp + 0x28], 0
            mov dword ptr [esp + 0x54], edx
            mov eax, dword ptr [edx]
            add eax, edi
            cmp word ptr [eax + 0x1c], 0
            mov esi, dword ptr [eax + 4]
            jbe L0045d53b
    L0045cfd4:
            mov ecx, dword ptr [eax + 8]
            mov ecx, dword ptr [ecx]
            test ecx, ecx
            jne L0045cfe1
            mov ecx, dword ptr [esp + 0x58]
    L0045cfe1:
            fld dword ptr [ebp + 4]
            fld dword ptr [esi + 0xc]
            fld dword ptr [esi + 0x10]
            fld dword ptr [ebp + 8]
            fxch st(2)
            _emit 0xd8
        _emit 0x4d
        _emit 0x00
            fxch st(3)
            fmulp st(1), st(0)
            mov dword ptr [esp + 0x50], ecx
            faddp st(2), st(0)
            fmul dword ptr [esi + 0x14]
            faddp st(1), st(0)
            fst dword ptr [esp + 0x18]
            fabs
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 1
            jne L0045d507
            test ecx, ecx
            je L0045d063
            mov eax, dword ptr [ecx + 4]
            test eax, eax
            je L0045d042
            mov al, byte ptr [eax]
            cmp al, 0x21
            je L0045d035
            cmp al, 0x3e
            je L0045d035
            cmp al, 0x3f
            jne L0045d042
    L0045d035:
            mov eax, dword ptr [gPling_materials]
            test eax, eax
            jne L0045d507
    L0045d042:
            test ecx, ecx
            je L0045d063
            mov eax, dword ptr [ecx + 0x20]
            test ah, 0x18
            jne L0045d063
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            je L0045d507
    L0045d063:
            fld dword ptr [esi + 0xc]
            mov ecx, dword ptr [esp + 0x78]
            fld dword ptr [esi + 0x14]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [esi + 0x10]
            fxch st(4)
            _emit 0xd8
        _emit 0x09
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fb0]
            fsub dword ptr [esi + 0x18]
            fnstsw ax
            test ah, 1
            je L0045d0bd
            fld st(0)
            fld dword ptr [esp + 0x20]
            fmul dword ptr [gUNK_00589fb8+0x4]
            fxch st(1)
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045d568
    L0045d0bd:
            fdiv dword ptr [esp + 0x18]
            fchs
            fst dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x80]
            fnstsw ax
            test ah, 1
            jne L0045d507
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x84]
            fnstsw ax
            test ah, 0x41
            je L0045d507
            fld dword ptr [ebp + 4]
            fld dword ptr [ebp + 8]
            fld dword ptr [esp + 0x18]
            _emit 0xd8
        _emit 0x4d
        _emit 0x00
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            _emit 0xd8
        _emit 0x01
            fxch st(1)
            fst dword ptr [esp + 0x68]
            fadd dword ptr [ecx + 4]
            fxch st(2)
            fst dword ptr [esp + 0x6c]
            fadd dword ptr [ecx + 8]
            fxch st(1)
            fstp dword ptr [esp + 0x64]
            fxch st(1)
            fstp dword ptr [esp + 0x68]
            xor ecx, ecx
            fstp dword ptr [esp + 0x6c]
            fld dword ptr [esi + 0xc]
            fld dword ptr [esi + 0x10]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x4c]
            fcompp
            fnstsw ax
            test ah, 1
            je L0045d14e
            mov ecx, 1
    L0045d14e:
            fld dword ptr [esi + ecx*4 + 0xc]
            fld dword ptr [esi + 0x14]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x4c]
            fcompp
            fnstsw ax
            test ah, 1
            je L0045d16d
            mov ecx, 2
    L0045d16d:
            sub ecx, 0
            je L0045d18c
            dec ecx
            je L0045d188
            dec ecx
            jne L0045d19d
            xor ebx, ebx
            mov dword ptr [esp + 0x1c], 1
            mov dword ptr [esp + 0x24], ebx
            jmp L0045d19d
    L0045d188:
            xor ebx, ebx
            jmp L0045d191
    L0045d18c:
            mov ebx, 1
    L0045d191:
            mov dword ptr [esp + 0x24], ebx
            mov dword ptr [esp + 0x1c], 2
    L0045d19d:
            mov ecx, dword ptr [edx]
            xor ebp, ebp
            mov bp, word ptr [esi + 4]
            xor eax, eax
            mov ax, word ptr [esi]
            mov ecx, dword ptr [edi + ecx + 0x10]
            mov edi, ebp
            mov ebp, dword ptr [esp + 0x1c]
            shl eax, 3
            add ebp, eax
            xor edx, edx
            mov dx, word ptr [esi + 2]
            add eax, ebx
            fld dword ptr [ecx + ebp*4]
            mov ebp, dword ptr [esp + 0x1c]
            lea ebx, [ebx + edx*8]
            fld dword ptr [ecx + eax*4]
            mov eax, dword ptr [esp + 0x1c]
            shl edi, 3
            add ebp, edi
            fld dword ptr [ecx + ebp*4]
            fld dword ptr [ecx + ebx*4]
            fxch st(3)
            fstp dword ptr [esp + 0x20]
            mov ebx, dword ptr [esp + 0x24]
            lea ebp, [eax + edx*8]
            add edi, ebx
            fld dword ptr [ecx + ebp*4]
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            fld dword ptr [ecx + edi*4]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fxch st(3)
            fsub dword ptr [esp + 0x14]
            fxch st(2)
            fsub dword ptr [esp + 0x14]
            fld dword ptr [esp + eax*4 + 0x64]
            fld dword ptr [esp + ebx*4 + 0x64]
            fxch st(4)
            fstp dword ptr [esp + 0x10]
            fsub dword ptr [esp + 0x20]
            fxch st(3)
            fsub dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0x10]
            fld st(4)
            fxch st(2)
            fstp dword ptr [esp + 0x40]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fxch st(3)
            fst dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 0x48]
            fxch st(3)
            fmul dword ptr [esp + 0x48]
            fxch st(3)
            fst dword ptr [esp + 0x4c]
            fxch st(2)
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fsub dword ptr [esp + 0x44]
            fxch st(2)
            fsubp st(1), st(0)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fabs
            fxch st(1)
            fabs
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045d4ff
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045d4ff
            fld dword ptr [esp + 0x20]
            fdiv dword ptr [esp + 0x14]
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045d4ff
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [esp + 0x48]
            fxch st(2)
            fsub dword ptr [esp + 0x4c]
            fxch st(2)
            fsubp st(1), st(0)
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fabs
            fxch st(1)
            fabs
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045d4ff
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045d4ff
            fld dword ptr [esp + 0x20]
            fdiv dword ptr [esp + 0x14]
            fst dword ptr [esp + 0x14]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045d4ff
            fld dword ptr [esp + 0x10]
            fadd dword ptr [esp + 0x14]
            fcom dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L0045d4fd
            shl edx, 5
            mov edi, dword ptr [esp + 0x2c]
            xor eax, eax
            fld dword ptr [edx + ecx + 0xc]
            mov ecx, dword ptr [esp + 0x54]
            xor edx, edx
            fmul dword ptr [esp + 0x14]
            xor ebx, ebx
            mov dword ptr [esp + 0x48], 0
            mov dword ptr [esp + 0x4c], 1
            fstp dword ptr [esp + 0x5c]
            mov ebp, dword ptr [ecx]
            mov ax, word ptr [esi + 2]
            fld dword ptr [gUNK_00589fb0]
            mov ebp, dword ptr [edi + ebp + 0x10]
            shl eax, 5
            fld dword ptr [ebp + eax + 0x10]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fsubrp st(2), st(0)
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x60]
            mov eax, dword ptr [ecx]
            mov dx, word ptr [esi + 4]
            shl edx, 5
            mov eax, dword ptr [edi + eax + 0x10]
            fld dword ptr [eax + edx + 0xc]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fcomp dword ptr [esp + 0x10]
            lea edx, [eax + edx + 0xc]
            fadd dword ptr [esp + 0x5c]
            fnstsw ax
            fstp dword ptr [esp + 0x5c]
            fld dword ptr [edx + 4]
            fmul dword ptr [esp + 0x10]
            fadd dword ptr [esp + 0x60]
            fstp dword ptr [esp + 0x60]
            mov ecx, dword ptr [ecx]
            mov bx, word ptr [esi]
            shl ebx, 5
            mov edx, dword ptr [edi + ecx + 0x10]
            test ah, 0x41
            fld dword ptr [edx + ebx + 0xc]
            lea eax, [edx + ebx + 0xc]
            fmul st(0), st(1)
            fadd dword ptr [esp + 0x5c]
            fstp dword ptr [esp + 0x5c]
            fld dword ptr [eax + 4]
            fmul st(0), st(1)
            fadd dword ptr [esp + 0x60]
            fstp dword ptr [esp + 0x60]
            fstp st(0)
            fld dword ptr [esp + 0x10]
            jne L0045d455
            fadd st(0), st(0)
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fsubr dword ptr [gUNK_00589fb0]
            fxch st(1)
            fcompp
            mov ecx, 2
            fnstsw ax
            test ah, 1
            jne L0045d42d
            mov ecx, dword ptr [esp + 0x4c]
    L0045d42d:
            fld dword ptr [esp + 0x10]
            fmul dword ptr [gFloat_00589fd0+0x4]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fsubr dword ptr [gFloat_00589fd0+0x4]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L0045d49e
            mov eax, 2
            jmp L0045d4a2
    L0045d455:
            fmul dword ptr [gFloat_00589fd0+0x4]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fsubr dword ptr [gFloat_00589fd0+0x4]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L0045d476
            xor ecx, ecx
            jmp L0045d47a
    L0045d476:
            mov ecx, dword ptr [esp + 0x4c]
    L0045d47a:
            fld dword ptr [esp + 0x10]
            fadd st(0), st(0)
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fsubr dword ptr [gUNK_00589fb0]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L0045d49e
            mov eax, 1
            jmp L0045d4a2
    L0045d49e:
            mov eax, dword ptr [esp + 0x48]
    L0045d4a2:
            mov edx, dword ptr [esp + 0x34]
            mov ebp, dword ptr [esp + 0x7c]
            mov dword ptr [gTemp_group], edx
            mov edx, dword ptr [esp + 0x8c]
            push edx
            lea edx, [esp + 0x60]
            push edx
            lea edx, [esp + 0x6c]
            push edx
            mov edx, dword ptr [esp + 0x84]
            push eax
            mov eax, dword ptr [esp + 0x38]
            push ecx
            mov ecx, dword ptr [esp + 0x2c]
            push eax
            mov eax, dword ptr [esp + 0x68]
            push ecx
            mov ecx, dword ptr [esp + 0x90]
            push ebp
            push edx
            mov edx, dword ptr [esp + 0x54]
            push eax
            push ecx
            push edx
            call dword ptr [esp + 0xb8]
            add esp, 0x30
            test eax, eax
            jne L0045d55e
            mov ebx, dword ptr [esp + 0x24]
            jmp L0045d507
    L0045d4fd:
            fstp st(0)
    L0045d4ff:
            mov ebp, dword ptr [esp + 0x7c]
            mov edi, dword ptr [esp + 0x2c]
    L0045d507:
            mov eax, dword ptr [esp + 0x30]
            mov ecx, dword ptr [esp + 0x28]
            inc ecx
            add esi, 0x1c
            mov eax, dword ptr [eax + 0x4c]
            mov dword ptr [esp + 0x28], ecx
            mov ecx, dword ptr [eax + 0x18]
            lea edx, [eax + 0x18]
            mov dword ptr [esp + 0x54], edx
            lea eax, [edi + ecx]
            xor ecx, ecx
            mov cx, word ptr [eax + 0x1c]
            cmp dword ptr [esp + 0x28], ecx
            jl L0045cfd4
            mov ecx, dword ptr [esp + 0x30]
    L0045d53b:
            mov eax, dword ptr [ecx + 0x4c]
            mov edx, dword ptr [esp + 0x34]
            xor esi, esi
            inc edx
            mov si, word ptr [eax + 8]
            add edi, 0x24
            cmp edx, esi
            mov dword ptr [esp + 0x34], edx
            mov dword ptr [esp + 0x2c], edi
            jl L0045cfb3
    L0045d55c:
            xor eax, eax
    L0045d55e:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x60
            ret 0x1c
    L0045d568:
            fstp st(0)
            jmp L0045d507
    }
#endif
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
                if (gNearest_T > gUNK_00589fb0) {
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
void C2_NAKED C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x20
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x34]
            push esi
            mov esi, dword ptr [ecx]
            push edi
            mov edi, dword ptr [esp + 0x34]
            mov dword ptr [ebp], 0x42c80000
            fld dword ptr [ecx + 0x34]
            fld dword ptr [edi + 8]
            fld dword ptr [ecx + 0x3c]
            fld dword ptr [ecx + 0x38]
            fxch st(3)
            _emit 0xd8
        _emit 0x0f
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [edi + 4]
            test esi, esi
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x3c]
            je L0045ed1c
            mov eax, dword ptr [esi + 0x20]
            test ah, 0x18
            jne L0045ed1c
            fld dword ptr [esp + 0x3c]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            je L0045f0c1
    L0045ed1c:
            test esi, esi
            je L0045ed42
            mov esi, dword ptr [esi + 4]
            test esi, esi
            je L0045ed42
            mov al, byte ptr [esi]
            cmp al, 0x21
            je L0045ed35
            cmp al, 0x3e
            je L0045ed35
            cmp al, 0x3f
            jne L0045ed42
    L0045ed35:
            mov eax, dword ptr [gPling_materials]
            test eax, eax
            jne L0045f0c1
    L0045ed42:
            fld dword ptr [esp + 0x3c]
            fabs
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 1
            jne L0045f0c1
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fsub dword ptr [ecx + 8]
            fxch st(1)
            fsub dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x38]
            fld dword ptr [ecx + 0x3c]
            _emit 0xd9
        _emit 0x02
            fsub dword ptr [ecx + 4]
            fxch st(2)
            fmulp st(4), st(0)
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fmulp st(3), st(0)
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            fcomp dword ptr [gUNK_00589fb0]
            fmul dword ptr [ecx + 0x34]
            fnstsw ax
            faddp st(1), st(0)
            test ah, 1
            fstp dword ptr [esp + 0x34]
            je L0045edbf
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [gUNK_00589fb8+0x4]
            fxch st(1)
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f0c1
    L0045edbf:
            fld dword ptr [esp + 0x3c]
            fcomp dword ptr [gUNK_00589fac]
            fld dword ptr [esp + 0x34]
            fnstsw ax
            test ah, 0x41
            je L0045ee08
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045f0c1
            fld dword ptr [esp + 0x3c]
            fsubr qword ptr [gDouble_0058a018+0x8]
            fld dword ptr [esp + 0x34]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L0045ee32
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret 0x10
    L0045ee08:
            fchs
            fcom dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045f0bf
            fld dword ptr [esp + 0x3c]
            fsub qword ptr [gDouble_0058a018+0x10]
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f0c1
    L0045ee32:
            fld dword ptr [esp + 0x34]
            fdiv dword ptr [esp + 0x3c]
            fchs
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            jne L0045ee55
            mov dword ptr [esp + 0x10], 0x3f800000
    L0045ee55:
            fld dword ptr [esp + 0x10]
            _emit 0xd8
        _emit 0x0f
            mov esi, dword ptr [esp + 0x40]
            xor ebx, ebx
            _emit 0xd9
        _emit 0x1e
            fld dword ptr [edi + 4]
            fmul dword ptr [esp + 0x10]
            _emit 0xd9
        _emit 0x06
            fxch st(1)
            fstp dword ptr [esi + 4]
            fld dword ptr [edi + 8]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esi + 4]
            fxch st(1)
            fstp dword ptr [esi + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x02
            _emit 0xd9
        _emit 0x1e
            fadd dword ptr [edx + 4]
            fstp dword ptr [esi + 4]
            fld dword ptr [edx + 8]
            fadd dword ptr [esi + 8]
            fstp dword ptr [esi + 8]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [ecx + 0x38]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x34]
            fcompp
            fnstsw ax
            test ah, 1
            je L0045eeb3
            mov ebx, 1
    L0045eeb3:
            fld dword ptr [ecx + ebx*4 + 0x34]
            fld dword ptr [ecx + 0x3c]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x34]
            fcompp
            fnstsw ax
            test ah, 1
            je L0045eed2
            mov ebx, 2
    L0045eed2:
            sub ebx, 0
            je L0045eeef
            dec ebx
            je L0045eee6
            dec ebx
            jne L0045eefb
            xor eax, eax
            mov edx, 1
            jmp L0045ef03
    L0045eee6:
            xor eax, eax
            mov edx, 2
            jmp L0045ef03
    L0045eeef:
            mov eax, 1
            mov edx, 2
            jmp L0045ef03
    L0045eefb:
            mov eax, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x34]
    L0045ef03:
            fld dword ptr [ecx + edx*4 + 4]
            fld dword ptr [ecx + eax*4 + 4]
            fld dword ptr [ecx + edx*4 + 0x1c]
            fld dword ptr [ecx + eax*4 + 0x10]
            fld dword ptr [ecx + edx*4 + 0x10]
            fxch st(4)
            fstp dword ptr [esp + 0x40]
            fxch st(1)
            fsub dword ptr [esp + 0x40]
            fxch st(3)
            fsub dword ptr [esp + 0x40]
            fld dword ptr [ecx + eax*4 + 0x1c]
            fxch st(3)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fsub dword ptr [esp + 0x34]
            fxch st(2)
            fsub dword ptr [esp + 0x34]
            fld dword ptr [esi + edx*4]
            fxch st(4)
            fst dword ptr [esp + 0x1c]
            fld dword ptr [esi + eax*4]
            fxch st(4)
            fstp dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld st(2)
            fxch st(2)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(3)
            fsub dword ptr [esp + 0x34]
            fxch st(4)
            fsub dword ptr [esp + 0x40]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fst dword ptr [esp + 0x34]
            fxch st(2)
            fsubp st(1), st(0)
            fxch st(1)
            fabs
            fmul qword ptr [gDouble_0058a018]
            fxch st(1)
            fst dword ptr [esp + 0x40]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f0c1
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045f0c1
            fld dword ptr [esp + 0x40]
            fdiv dword ptr [esp + 0x34]
            fst dword ptr [esp + 0x2c]
            fcomp qword ptr [gDouble_0058a010]
            fnstsw ax
            test ah, 1
            jne L0045f0c1
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x34]
            fxch st(1)
            fsubp st(2), st(0)
            fabs
            fmul qword ptr [gDouble_0058a018]
            fxch st(1)
            fst dword ptr [esp + 0x40]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045f0c1
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045f0c1
            fld dword ptr [esp + 0x40]
            fdiv dword ptr [esp + 0x34]
            fcom qword ptr [gDouble_0058a010]
            fnstsw ax
            test ah, 1
            jne L0045f0bf
            fld dword ptr [esp + 0x2c]
            fadd st(0), st(1)
            fcomp qword ptr [gDouble_0058a018]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L0045f0c1
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fcomp dword ptr [gUNK_00589fac]
            mov dword ptr [ebp], eax
            mov eax, dword ptr [ecx + 0x34]
            mov dword ptr [edx], eax
            mov eax, dword ptr [ecx + 0x38]
            mov dword ptr [edx + 4], eax
            mov ecx, dword ptr [ecx + 0x3c]
            fnstsw ax
            mov dword ptr [edx + 8], ecx
            test ah, 0x41
            jne L0045f0c1
            _emit 0xd9
        _emit 0x02
            fchs
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [edx + 4]
            fchs
            fstp dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fchs
            fstp dword ptr [edx + 8]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret 0x10
    L0045f0bf:
            fstp st(0)
    L0045f0c1:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045f690
void C2_NAKED C2_HOOK_FASTCALL FillInBounds(tBounds* pBounds) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 tv;
    br_vector3 c[3];
    int i;

    BrVector3Add(&tv, &pBounds->original_bounds.min, &pBounds->original_bounds.max);
    BrVector3InvScale(&tv, &tv, 2.0f);
    BrMatrix34ApplyP(&pBounds->box_centre, &tv, pBounds->mat);
    BrVector3Sub(&tv, &pBounds->original_bounds.max, &pBounds->original_bounds.min);
    pBounds->radius = BrVector3Length(&tv) / 2.0f;
    BrMatrix34ApplyP(&pBounds->real_bounds.min, &pBounds->original_bounds.min, pBounds->mat);
    BrVector3Copy(&pBounds->real_bounds.max, &pBounds->real_bounds.min);
    for (i = 0; i < 3; ++i) {
        BrVector3Scale(&c[i], (br_vector3*)pBounds->mat->m[i], tv.v[i]);
    }
    for (i = 0; i < 3; ++i) {
        pBounds->real_bounds.min.v[i] += c[0].v[i] * (float)(c[0].v[i] < 0.0f)
            + c[1].v[i] * (float)(c[1].v[i] < 0.0f)
            + c[2].v[i] * (float)(c[2].v[i] < 0.0f);
        pBounds->real_bounds.max.v[i] += c[0].v[i] * (float)(c[0].v[i] > 0.0f)
            + c[1].v[i] * (float)(c[1].v[i] > 0.0f)
            + c[2].v[i] * (float)(c[2].v[i] > 0.0f);
    }
#else
    __asm {
            sub esp, 0x3c
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            lea eax, [esp + 0x1c]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 0xc]
            fxch st(2)
            fadd dword ptr [esi + 0x10]
            fxch st(1)
            fadd dword ptr [esi + 0x14]
            fxch st(2)
            fadd dword ptr [esi + 0x18]
            fxch st(1)
            fmul dword ptr [gFloat_00589fd0+0x4]
            fxch st(2)
            fmul dword ptr [gFloat_00589fd0+0x4]
            fxch st(1)
            fmul dword ptr [gFloat_00589fd0+0x4]
            fxch st(2)
            fstp dword ptr [esp + 0x1c]
            lea edi, [esi + 4]
            mov ecx, dword ptr [esi]
            fstp dword ptr [esp + 0x20]
            lea edx, [esi + 0x1c]
            push ecx
            push eax
            push edx
            fstp dword ptr [esp + 0x30]
            call BrMatrix34ApplyP
            fld dword ptr [esi + 0x14]
            fld dword ptr [esi + 0x10]
            _emit 0xd8
        _emit 0x27
            fxch st(1)
            fsub dword ptr [esi + 8]
            fld dword ptr [esi + 0x18]
            fsub dword ptr [esi + 0xc]
            fxch st(2)
            fst dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(2)
            fst dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            mov eax, dword ptr [esi]
            add esp, 0xc
            lea ebx, [esi + 0x2c]
            fsqrt
            push eax
            push edi
            push ebx
            fmul dword ptr [gFloat_00589fd0+0x4]
            fstp dword ptr [esi + 0x28]
            call BrMatrix34ApplyP
            mov ecx, dword ptr [ebx]
            mov eax, dword ptr [esi]
            mov ebp, dword ptr [esi + 0x34]
            mov edi, dword ptr [esi + 0x30]
            lea ebx, [esi + 0x38]
            mov dword ptr [esi + 0x40], ebp
            lea edx, [eax + 8]
            mov dword ptr [esp + 0x24], eax
            lea ebp, [esp + 0x38]
            add esp, 0xc
            mov dword ptr [ebx], ecx
            mov dword ptr [esi + 0x3c], edi
            mov edi, dword ptr [esp + 0x18]
            lea ecx, [esp + 0x1c]
            lea eax, [esp + 0x2c]
            sub edi, ebp
            mov esi, 3
    L0045f775:
            _emit 0xd9
        _emit 0x01
            fld dword ptr [edi + eax]
            fld dword ptr [edx - 4]
            _emit 0xd9
        _emit 0x01
            fxch st(3)
            fmulp st(2), st(0)
            add ecx, 4
            add edx, 0xc
            add eax, 0xc
            dec esi
            fmul dword ptr [ecx - 4]
            fxch st(1)
            fstp dword ptr [eax - 0x10]
            fstp dword ptr [eax - 0xc]
            fmul dword ptr [edx - 0xc]
            fstp dword ptr [eax - 8]
            jne L0045f775
            mov ecx, ebx
            lea edx, [esp + 0x34]
            mov ebx, 3
            xor ebp, ebp
    L0045f7ad:
            fld dword ptr [edx - 0xc]
            fcomp dword ptr [gUNK_00589fac]
            lea edi, [edx - 0xc]
            mov dword ptr [esp + 0x14], 1
            fnstsw ax
            test ah, 1
            jne L0045f7cc
            mov dword ptr [esp + 0x14], ebp
    L0045f7cc:
            _emit 0xd9
        _emit 0x02
            fcomp dword ptr [gUNK_00589fac]
            mov dword ptr [esp + 0x18], 1
            fnstsw ax
            test ah, 1
            jne L0045f7e7
            mov dword ptr [esp + 0x18], ebp
    L0045f7e7:
            fld dword ptr [edx + 0xc]
            fcomp dword ptr [gUNK_00589fac]
            lea esi, [edx + 0xc]
            mov dword ptr [esp + 0x10], 1
            fnstsw ax
            test ah, 1
            jne L0045f806
            mov dword ptr [esp + 0x10], ebp
    L0045f806:
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x14]
            fild dword ptr [esp + 0x18]
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            _emit 0xd9
        _emit 0x07
            fxch st(2)
            _emit 0xd8
        _emit 0x0f
            fxch st(2)
            fcomp dword ptr [gUNK_00589fac]
            fxch st(2)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            faddp st(2), st(0)
            mov dword ptr [esp + 0x14], 1
            fnstsw ax
            faddp st(1), st(0)
            test ah, 0x41
            fadd dword ptr [ecx - 0xc]
            fstp dword ptr [ecx - 0xc]
            je L0045f847
            mov dword ptr [esp + 0x14], ebp
    L0045f847:
            _emit 0xd9
        _emit 0x02
            fcomp dword ptr [gUNK_00589fac]
            mov dword ptr [esp + 0x18], 1
            fnstsw ax
            test ah, 0x41
            je L0045f862
            mov dword ptr [esp + 0x18], ebp
    L0045f862:
            _emit 0xd9
        _emit 0x06
            fcomp dword ptr [gUNK_00589fac]
            mov dword ptr [esp + 0x10], 1
            fnstsw ax
            test ah, 0x41
            je L0045f87d
            mov dword ptr [esp + 0x10], ebp
    L0045f87d:
            fild dword ptr [esp + 0x18]
            fild dword ptr [esp + 0x14]
            fild dword ptr [esp + 0x10]
            fxch st(2)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            _emit 0xd8
        _emit 0x0f
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            add edx, 4
            add ecx, 4
            dec ebx
            faddp st(1), st(0)
            fadd dword ptr [ecx - 4]
            fstp dword ptr [ecx - 4]
            jne L0045f7ad
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret
    }
#endif
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
int C2_NAKED C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x22c
            mov eax, dword ptr [esp + 0x230]
            mov dword ptr [esp + 0x68], ecx
            mov dword ptr [esp + 0x60], edx
            mov ecx, dword ptr [eax + 0x4c]
            mov eax, dword ptr [esp + 0x23c]
            test eax, eax
            mov dword ptr [esp + 0x64], ecx
            jg L0045ffc2
            xor eax, eax
            add esp, 0x22c
            ret 0x14
    L0045ffc2:
            cmp word ptr [ecx + 8], 0
            push edi
            push esi
            push ebp
            push ebx
            mov dword ptr [esp + 0x60], 0
            jbe L00460bb9
            xor ebx, ebx
            mov dword ptr [esp + 0x44], ebx
    L0045ffdf:
            mov edx, dword ptr [ecx + 0x18]
            add ecx, 0x18
            mov dword ptr [esp + 0x54], 0
            cmp word ptr [ebx + edx + 0x1c], 0
            lea eax, [ebx + edx]
            mov edx, dword ptr [ebx + edx + 4]
            mov dword ptr [esp + 0x3c], edx
            jbe L00460b86
            mov edx, dword ptr [esp + 0x248]
            mov dword ptr [esp + 0x5c], 0
            add edx, 0x10
            mov dword ptr [esp + 0x4c], edx
    L0046001a:
            mov ebp, dword ptr [esp + 0x3c]
            mov edx, dword ptr [eax + 0x10]
            xor esi, esi
            mov edi, dword ptr [esp + 0x70]
            mov si, word ptr [ebp]
            shl esi, 5
            mov dword ptr [esp + 0x68], esi
            fld dword ptr [edx + esi + 8]
            fld dword ptr [edx + esi + 4]
            fsub dword ptr [edi + 0x20]
            fxch st(1)
            fsub dword ptr [edi + 0x24]
            fld dword ptr [ebp + 0x10]
            fld dword ptr [ebp + 0x14]
            fld dword ptr [edx + esi]
            fsub dword ptr [edi + 0x1c]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ebp + 0xc]
            faddp st(1), st(0)
            fabs
            fcomp dword ptr [edi + 0x28]
            fnstsw ax
            test ah, 0x41
            je L00460b45
            xor eax, eax
            mov ax, word ptr [ebp + 2]
            xor ebp, ebp
            fld dword ptr [edx + esi]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [esp + 0x3c]
            mov bp, word ptr [eax + 4]
            mov eax, dword ptr [edi + 0x2c]
            mov dword ptr [esp + 0x10], eax
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004600c2
            mov eax, dword ptr [esp + 0x14]
            shl eax, 5
            fld dword ptr [eax + edx]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004600c2
            mov eax, ebp
            shl eax, 5
            fld dword ptr [eax + edx]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            jne L00460b45
    L004600c2:
            mov eax, dword ptr [edi + 0x38]
            fld dword ptr [edx + esi]
            mov dword ptr [esp + 0x10], eax
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L00460103
            mov eax, dword ptr [esp + 0x14]
            shl eax, 5
            fld dword ptr [eax + edx]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L00460103
            mov eax, ebp
            shl eax, 5
            fld dword ptr [eax + edx]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            je L00460b45
    L00460103:
            mov eax, dword ptr [edi + 0x30]
            fld dword ptr [edx + esi + 4]
            mov dword ptr [esp + 0x10], eax
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L00460147
            mov eax, dword ptr [esp + 0x14]
            shl eax, 5
            fld dword ptr [eax + edx + 4]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L00460147
            mov eax, ebp
            shl eax, 5
            fld dword ptr [eax + edx + 4]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            jne L00460b45
    L00460147:
            mov eax, dword ptr [edi + 0x3c]
            fld dword ptr [edx + esi + 4]
            mov dword ptr [esp + 0x10], eax
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L0046018b
            mov eax, dword ptr [esp + 0x14]
            shl eax, 5
            fld dword ptr [eax + edx + 4]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L0046018b
            mov eax, ebp
            shl eax, 5
            fld dword ptr [eax + edx + 4]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            je L00460b45
    L0046018b:
            mov eax, dword ptr [edi + 0x34]
            fld dword ptr [edx + esi + 8]
            mov dword ptr [esp + 0x10], eax
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004601cf
            mov eax, dword ptr [esp + 0x14]
            shl eax, 5
            fld dword ptr [eax + edx + 8]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004601cf
            mov eax, ebp
            shl eax, 5
            fld dword ptr [eax + edx + 8]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            jne L00460b45
    L004601cf:
            mov eax, dword ptr [edi + 0x40]
            fld dword ptr [edx + esi + 8]
            mov dword ptr [esp + 0x10], eax
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L00460213
            mov eax, dword ptr [esp + 0x14]
            shl eax, 5
            fld dword ptr [eax + edx + 8]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L00460213
            mov eax, ebp
            shl eax, 5
            fld dword ptr [eax + edx + 8]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            je L00460b45
    L00460213:
            mov eax, dword ptr [edi]
            fld dword ptr [edx + esi]
            fsub dword ptr [eax + 0x24]
            shl ebp, 5
            push eax
            mov dword ptr [esp + 0x70], ebp
            fstp dword ptr [esp + 0x9c]
            mov edx, dword ptr [ecx]
            mov edx, dword ptr [ebx + edx + 0x10]
            fld dword ptr [edx + esi + 4]
            fsub dword ptr [eax + 0x28]
            fstp dword ptr [esp + 0xa0]
            mov edx, dword ptr [ecx]
            mov edx, dword ptr [ebx + edx + 0x10]
            fld dword ptr [edx + esi + 8]
            fsub dword ptr [eax + 0x2c]
            mov edx, dword ptr [esp + 0x18]
            shl edx, 5
            mov dword ptr [esp + 0x68], edx
            fstp dword ptr [esp + 0xa4]
            mov esi, dword ptr [ecx]
            mov esi, dword ptr [ebx + esi + 0x10]
            fld dword ptr [esi + edx]
            fsub dword ptr [eax + 0x24]
            fstp dword ptr [esp + 0xa8]
            mov esi, dword ptr [ecx]
            mov esi, dword ptr [ebx + esi + 0x10]
            fld dword ptr [esi + edx + 4]
            fsub dword ptr [eax + 0x28]
            fstp dword ptr [esp + 0xac]
            mov esi, dword ptr [ecx]
            mov esi, dword ptr [ebx + esi + 0x10]
            fld dword ptr [esi + edx + 8]
            fsub dword ptr [eax + 0x2c]
            fstp dword ptr [esp + 0xb0]
            mov edx, dword ptr [ecx]
            mov edx, dword ptr [ebx + edx + 0x10]
            fld dword ptr [edx + ebp]
            fsub dword ptr [eax + 0x24]
            fstp dword ptr [esp + 0xb4]
            mov edx, dword ptr [ecx]
            mov edx, dword ptr [ebx + edx + 0x10]
            fld dword ptr [edx + ebp + 4]
            fsub dword ptr [eax + 0x28]
            fstp dword ptr [esp + 0xb8]
            mov ecx, dword ptr [ecx]
            mov edx, dword ptr [ebx + ecx + 0x10]
            lea ecx, [esp + 0x90]
            fld dword ptr [edx + ebp + 8]
            fsub dword ptr [eax + 0x2c]
            lea eax, [esp + 0x9c]
            push eax
            push ecx
            fstp dword ptr [esp + 0xc4]
            call BrMatrix34TApplyV
            mov edx, dword ptr [edi]
            add esp, 0xc
            lea eax, [esp + 0xa4]
            lea ecx, [esp + 0x98]
            push edx
            push eax
            push ecx
            call BrMatrix34TApplyV
            mov edx, dword ptr [edi]
            add esp, 0xc
            lea eax, [esp + 0xb0]
            lea ecx, [esp + 0xa4]
            push edx
            push eax
            push ecx
            call BrMatrix34TApplyV
            add esp, 0xc
            xor esi, esi
            add edi, 0x10
            mov dword ptr [esp + 0x18], 3
            mov dword ptr [esp + 0x50], esi
            mov dword ptr [esp + 0x58], edi
            jmp L0046033c
    L00460338:
            mov ebx, dword ptr [esp + 0x44]
    L0046033c:
            mov edx, dword ptr [esp + 0x58]
            xor ecx, ecx
            mov dword ptr [esp + 0x14], ecx
            mov eax, dword ptr [edx - 0xc]
            mov dword ptr [esp + 0x48], eax
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            lea edx, [eax - 1]
            jle L00460542
            lea ecx, [esp + 0x124]
            xor eax, eax
            mov dword ptr [esp + 0x2c], ecx
            lea ecx, [esp + 0x120]
            mov dword ptr [esp + 0x30], ecx
            lea ecx, [esp + 0x11c]
            lea edx, [edx + edx*2]
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [esp + 0x24], eax
            mov dword ptr [esp + 0x20], ecx
            mov ecx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x38], eax
            shl edx, 2
            lea ebp, [esp + esi + 0x8c]
            lea eax, [esp + esi + 0x11c]
            mov edi, edx
            mov dword ptr [esp + 0x34], ebp
            mov dword ptr [esp + 0x28], eax
            mov dword ptr [esp + 0x18], ecx
    L004603b1:
            fld dword ptr [ebp]
            fcomp dword ptr [esp + 0x48]
            fnstsw ax
            test ah, 0x41
            jne L004603c6
            mov ecx, 1
            jmp L004603c8
    L004603c6:
            xor ecx, ecx
    L004603c8:
            lea eax, [esi + edi]
            fld dword ptr [esp + eax + 0x8c]
            fcomp dword ptr [esp + 0x48]
            fnstsw ax
            test ah, 0x41
            jne L004603e4
            mov eax, 1
            jmp L004603e6
    L004603e4:
            xor eax, eax
    L004603e6:
            cmp ecx, eax
            je L004604a7
            mov eax, dword ptr [esp + 0x1c]
            xor ecx, ecx
            lea edx, [esp + edx + 0x8c]
            lea ebx, [esp + eax + 0x8c]
    L00460402:
            cmp ecx, dword ptr [esp + 0x50]
            je L0046044a
            fld dword ptr [esp + 0x48]
            fld dword ptr [ebx]
            fsub dword ptr [edx]
            fxch st(1)
            lea eax, [esi + edi]
            lea ebp, [esi + edi]
            mov dword ptr [esp + 0x7c], eax
            mov eax, ebp
            mov ebp, dword ptr [esp + 0x34]
            fsub dword ptr [esp + eax + 0x8c]
            fld dword ptr [ebp]
            fsub dword ptr [esp + eax + 0x8c]
            fxch st(2)
            fmulp st(1), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            mov eax, dword ptr [esp + 0x24]
            add eax, ecx
            fadd dword ptr [edx]
            fstp dword ptr [esp + eax*4 + 0x11c]
    L0046044a:
            inc ecx
            add ebx, 4
            add edx, 4
            cmp ecx, 3
            jl L00460402
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x48]
            mov edx, dword ptr [esp + 0x14]
            mov edi, dword ptr [esp + 0x20]
            mov dword ptr [eax], ecx
            mov ecx, dword ptr [esp + 0x24]
            add ecx, 3
            inc edx
            mov dword ptr [esp + 0x24], ecx
            mov ecx, 0xc
            mov ebx, dword ptr [esp + 0x44]
            add eax, ecx
            mov dword ptr [esp + 0x14], edx
            mov edx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x28], eax
            mov eax, dword ptr [esp + 0x2c]
            add edi, ecx
            add edx, ecx
            add eax, ecx
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x2c], eax
            jmp L004604ab
    L004604a7:
            mov ecx, dword ptr [esp + 0x14]
    L004604ab:
            fld dword ptr [ebp]
            fcomp dword ptr [esp + 0x48]
            fnstsw ax
            test ah, 1
            jne L00460512
            mov edx, dword ptr [esp + 0x1c]
            mov edi, dword ptr [esp + 0x20]
            inc ecx
            lea eax, [esp + edx + 0x8c]
            mov dword ptr [esp + 0x14], ecx
            mov edx, dword ptr [eax]
            mov dword ptr [edi], edx
            mov edx, dword ptr [esp + 0x30]
            mov edi, dword ptr [eax + 4]
            mov dword ptr [edx], edi
            mov edi, dword ptr [eax + 8]
            mov eax, dword ptr [esp + 0x2c]
            add edx, 0xc
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [eax], edi
            mov edi, dword ptr [esp + 0x24]
            add edi, 3
            mov dword ptr [esp + 0x24], edi
            mov edi, dword ptr [esp + 0x28]
            add edi, 0xc
            mov dword ptr [esp + 0x28], edi
            mov edi, dword ptr [esp + 0x20]
            add edi, 0xc
            add eax, 0xc
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x2c], eax
    L00460512:
            mov eax, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x1c]
            mov edi, eax
            add eax, 0xc
            mov dword ptr [esp + 0x38], eax
            mov eax, edx
            add eax, 0xc
            add ebp, 0xc
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0x18]
            dec eax
            mov dword ptr [esp + 0x34], ebp
            mov dword ptr [esp + 0x18], eax
            jne L004603b1
    L00460542:
            test ecx, ecx
            mov dword ptr [esp + 0x18], ecx
            jle L0046057e
            xor eax, eax
            mov edx, ecx
    L0046054e:
            mov edi, dword ptr [esp + eax + 0x11c]
            mov dword ptr [esp + eax + 0x8c], edi
            mov edi, dword ptr [esp + eax + 0x120]
            mov dword ptr [esp + eax + 0x90], edi
            mov edi, dword ptr [esp + eax + 0x124]
            mov dword ptr [esp + eax + 0x94], edi
            add eax, 0xc
            dec edx
            jne L0046054e
    L0046057e:
            cmp ecx, 3
            jl L004607e3
            mov eax, dword ptr [esp + 0x58]
            xor edi, edi
            test ecx, ecx
            mov edx, dword ptr [eax]
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x40], edx
            lea edx, [ecx - 1]
            jle L0046077e
            lea eax, [esp + 0x1b4]
            lea edx, [edx + edx*2]
            mov dword ptr [esp + 0x18], eax
            lea eax, [esp + 0x1b0]
            mov dword ptr [esp + 0x28], eax
            lea eax, [esp + 0x1ac]
            mov dword ptr [esp + 0x30], eax
            lea eax, [esp + esi + 0x1ac]
            xor ecx, ecx
            mov dword ptr [esp + 0x2c], eax
            mov eax, dword ptr [esp + 0x14]
            lea ebx, [esp + esi + 0x8c]
            shl edx, 2
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x1c], ecx
            mov dword ptr [esp + 0x34], ecx
            mov edi, edx
            mov dword ptr [esp + 0x38], ebx
            mov dword ptr [esp + 0x14], eax
    L004605f7:
            fld dword ptr [ebx]
            fcomp dword ptr [esp + 0x40]
            fnstsw ax
            test ah, 0x41
            jne L0046060b
            mov ebp, 1
            jmp L0046060d
    L0046060b:
            xor ebp, ebp
    L0046060d:
            lea eax, [esi + edi]
            fld dword ptr [esp + eax + 0x8c]
            fcomp dword ptr [esp + 0x40]
            fnstsw ax
            test ah, 0x41
            jne L00460629
            mov eax, 1
            jmp L0046062b
    L00460629:
            xor eax, eax
    L0046062b:
            cmp ebp, eax
            je L004606e6
            mov eax, dword ptr [esp + 0x24]
            xor ecx, ecx
            lea edx, [esp + edx + 0x8c]
            lea ebp, [esp + eax + 0x8c]
    L00460647:
            cmp ecx, dword ptr [esp + 0x50]
            je L0046068f
            fld dword ptr [esp + 0x40]
            fld dword ptr [ebp]
            fsub dword ptr [edx]
            fxch st(1)
            lea eax, [esi + edi]
            lea ebx, [esi + edi]
            mov dword ptr [esp + 0x7c], eax
            mov eax, ebx
            mov ebx, dword ptr [esp + 0x38]
            fsub dword ptr [esp + eax + 0x8c]
            fld dword ptr [ebx]
            fsub dword ptr [esp + eax + 0x8c]
            fxch st(2)
            fmulp st(1), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            mov eax, dword ptr [esp + 0x1c]
            add eax, ecx
            fadd dword ptr [edx]
            fstp dword ptr [esp + eax*4 + 0x1ac]
    L0046068f:
            inc ecx
            add ebp, 4
            add edx, 4
            cmp ecx, 3
            jl L00460647
            mov eax, dword ptr [esp + 0x2c]
            mov ecx, dword ptr [esp + 0x40]
            mov edx, dword ptr [esp + 0x20]
            mov edi, dword ptr [esp + 0x30]
            mov dword ptr [eax], ecx
            mov ecx, dword ptr [esp + 0x1c]
            add ecx, 3
            inc edx
            mov dword ptr [esp + 0x1c], ecx
            mov ecx, 0xc
            add eax, ecx
            mov dword ptr [esp + 0x20], edx
            mov edx, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x2c], eax
            mov eax, dword ptr [esp + 0x18]
            add edi, ecx
            add edx, ecx
            add eax, ecx
            mov ecx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x28], edx
            mov dword ptr [esp + 0x18], eax
    L004606e6:
            fld dword ptr [ebx]
            fcomp dword ptr [esp + 0x40]
            fnstsw ax
            test ah, 0x41
            je L0046074e
            mov eax, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + ecx + 0x8c]
            mov edi, dword ptr [esp + ecx + 0x90]
            mov ebp, dword ptr [esp + ecx + 0x94]
            mov dword ptr [eax], edx
            mov edx, dword ptr [esp + 0x28]
            add eax, 0xc
            mov dword ptr [edx], edi
            mov edi, dword ptr [esp + 0x18]
            add edx, 0xc
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [edi], ebp
            mov ebp, dword ptr [esp + 0x20]
            inc ebp
            mov dword ptr [esp + 0x28], edx
            mov dword ptr [esp + 0x20], ebp
            mov ebp, dword ptr [esp + 0x1c]
            add ebp, 3
            mov dword ptr [esp + 0x1c], ebp
            mov ebp, dword ptr [esp + 0x2c]
            add ebp, 0xc
            add edi, 0xc
            mov dword ptr [esp + 0x2c], ebp
            mov dword ptr [esp + 0x18], edi
    L0046074e:
            mov eax, dword ptr [esp + 0x34]
            mov edx, ecx
            mov edi, eax
            add eax, 0xc
            mov dword ptr [esp + 0x34], eax
            mov eax, dword ptr [esp + 0x14]
            add ebx, 0xc
            add ecx, 0xc
            dec eax
            mov dword ptr [esp + 0x38], ebx
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x14], eax
            jne L004605f7
            mov edi, dword ptr [esp + 0x20]
    L0046077e:
            test edi, edi
            mov dword ptr [esp + 0x18], edi
            jle L004607ba
            xor eax, eax
            mov ecx, edi
    L0046078a:
            mov edx, dword ptr [esp + eax + 0x1ac]
            mov dword ptr [esp + eax + 0x8c], edx
            mov edx, dword ptr [esp + eax + 0x1b0]
            mov dword ptr [esp + eax + 0x90], edx
            mov edx, dword ptr [esp + eax + 0x1b4]
            mov dword ptr [esp + eax + 0x94], edx
            add eax, 0xc
            dec ecx
            jne L0046078a
    L004607ba:
            cmp edi, 3
            jl L004607df
            mov edi, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x58]
            add esi, 4
            inc edi
            add ecx, 4
            cmp esi, 0xc
            mov dword ptr [esp + 0x50], edi
            mov dword ptr [esp + 0x58], ecx
            jl L00460338
    L004607df:
            mov ebx, dword ptr [esp + 0x44]
    L004607e3:
            cmp dword ptr [esp + 0x18], 3
            jl L00460b45
            mov eax, dword ptr [esp + 0x250]
            test eax, eax
            je L004608aa
            mov edi, dword ptr [esp + 0x240]
            mov esi, dword ptr [esp + 0x4c]
            mov ebp, dword ptr [esp + 0x250]
            mov ecx, dword ptr [edi + 0x4c]
            lea eax, [esi - 0xc]
            push ebp
            mov dword ptr [esp + 0x38], eax
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + ebx + 0x10]
            mov edx, dword ptr [esp + 0x6c]
            add ecx, edx
            push ecx
            push eax
            call BrMatrix34ApplyP
            mov edx, dword ptr [edi + 0x4c]
            add esp, 0xc
            mov eax, dword ptr [edx + 0x18]
            push ebp
            mov ecx, dword ptr [eax + ebx + 0x10]
            mov eax, dword ptr [esp + 0x68]
            add ecx, eax
            push ecx
            push esi
            call BrMatrix34ApplyP
            mov edx, dword ptr [edi + 0x4c]
            add esp, 0xc
            mov eax, dword ptr [edx + 0x18]
            push ebp
            lea edx, [esi + 0xc]
            mov ecx, dword ptr [eax + ebx + 0x10]
            mov eax, dword ptr [esp + 0x70]
            add ecx, eax
            push ecx
            push edx
            call BrMatrix34ApplyP
            mov eax, dword ptr [esp + 0x48]
            add esp, 0xc
            mov ecx, dword ptr [eax + 0xc]
            push ebp
            mov dword ptr [esp + 0x84], ecx
            mov edx, dword ptr [eax + 0x10]
            mov dword ptr [esp + 0x88], edx
            mov eax, dword ptr [eax + 0x14]
            mov dword ptr [esp + 0x8c], eax
            lea ecx, [esp + 0x84]
            lea eax, [esi + 0x24]
            push ecx
            push eax
            mov dword ptr [esp + 0x44], eax
            call BrMatrix34ApplyV
            add esp, 0xc
            jmp L00460988
    L004608aa:
            mov ecx, dword ptr [esp + 0x240]
            mov eax, dword ptr [esp + 0x4c]
            mov ebp, dword ptr [esp + 0x250]
            mov edx, dword ptr [ecx + 0x4c]
            lea esi, [eax - 0xc]
            mov dword ptr [esp + 0x34], esi
            mov edx, dword ptr [edx + 0x18]
            mov edi, dword ptr [edx + ebx + 0x10]
            mov edx, dword ptr [esp + 0x68]
            mov edi, dword ptr [edi + edx]
            mov dword ptr [esi], edi
            mov esi, dword ptr [ecx + 0x4c]
            mov edi, dword ptr [esp + 0x240]
            mov esi, dword ptr [esi + 0x18]
            mov esi, dword ptr [esi + ebx + 0x10]
            mov esi, dword ptr [esi + edx + 4]
            mov dword ptr [eax - 8], esi
            mov esi, dword ptr [ecx + 0x4c]
            mov esi, dword ptr [esi + 0x18]
            mov esi, dword ptr [esi + ebx + 0x10]
            mov edx, dword ptr [esi + edx + 8]
            mov dword ptr [eax - 4], edx
            mov edx, dword ptr [ecx + 0x4c]
            mov edx, dword ptr [edx + 0x18]
            mov esi, dword ptr [edx + ebx + 0x10]
            mov edx, dword ptr [esp + 0x64]
            mov esi, dword ptr [esi + edx]
            mov dword ptr [eax], esi
            mov esi, dword ptr [ecx + 0x4c]
            mov esi, dword ptr [esi + 0x18]
            mov esi, dword ptr [esi + ebx + 0x10]
            mov esi, dword ptr [esi + edx + 4]
            mov dword ptr [eax + 4], esi
            mov esi, dword ptr [ecx + 0x4c]
            mov esi, dword ptr [esi + 0x18]
            mov esi, dword ptr [esi + ebx + 0x10]
            mov edx, dword ptr [esi + edx + 8]
            mov dword ptr [eax + 8], edx
            mov edx, dword ptr [ecx + 0x4c]
            mov edx, dword ptr [edx + 0x18]
            mov esi, dword ptr [edx + ebx + 0x10]
            mov edx, dword ptr [esp + 0x6c]
            mov esi, dword ptr [esi + edx]
            mov dword ptr [eax + 0xc], esi
            mov esi, dword ptr [ecx + 0x4c]
            mov esi, dword ptr [esi + 0x18]
            mov esi, dword ptr [esi + ebx + 0x10]
            mov esi, dword ptr [esi + edx + 4]
            mov dword ptr [eax + 0x10], esi
            mov ecx, dword ptr [ecx + 0x4c]
            mov ecx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [ecx + ebx + 0x10]
            mov edx, dword ptr [ecx + edx + 8]
            mov ecx, dword ptr [esp + 0x3c]
            mov dword ptr [eax + 0x14], edx
            lea edx, [eax + 0x24]
            mov esi, dword ptr [ecx + 0xc]
            mov dword ptr [esp + 0x38], edx
            mov dword ptr [edx], esi
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [eax + 0x28], edx
            mov ecx, dword ptr [ecx + 0x14]
            mov dword ptr [eax + 0x2c], ecx
            mov esi, eax
    L00460988:
            mov edx, dword ptr [edi + 0x4c]
            mov eax, dword ptr [edx + 0x18]
            mov ecx, dword ptr [eax + ebx + 8]
            mov eax, dword ptr [ecx]
            test eax, eax
            jne L0046099f
            mov eax, dword ptr [esp + 0x244]
    L0046099f:
            mov edx, dword ptr [esp + 0x248]
            test ebp, ebp
            mov dword ptr [edx], eax
            jne L004609b5
            mov eax, dword ptr [esp + 0x3c]
            fld dword ptr [eax + 0x18]
            jmp L004609d7
    L004609b5:
            fld dword ptr [esi + 0x2c]
            fld dword ptr [esi - 4]
            fld dword ptr [esi + 0x28]
            mov ecx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x38]
            fld dword ptr [esi - 8]
            fld dword ptr [ecx]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax]
            faddp st(1), st(0)
    L004609d7:
            mov ecx, dword ptr [esp + 0x78]
            fstp dword ptr [esi + 0x34]
            mov dword ptr [esi + 0x38], ecx
            mov al, byte ptr [edi + 0x20]
            test al, 0x80
            je L00460a6e
            mov word ptr [esi + 0x3c], 0xffff
            mov eax, dword ptr [edi + 0x4c]
            mov ecx, dword ptr [eax + 0x18]
            mov eax, dword ptr [ecx + ebx + 0xc]
            mov ecx, dword ptr [esp + 0x54]
            mov ax, word ptr [eax + ecx*2]
            mov word ptr [esi + 0x3e], ax
            mov ecx, dword ptr [edi + 0xc]
            and eax, 0xffff
            lea ebp, [eax + eax*4]
            xor eax, eax
            mov al, byte ptr [ecx + ebp*8 + 0x10]
            xor ecx, ecx
            mov dword ptr [esi + 0x30], eax
            mov ebp, dword ptr [edi + 0xc]
            xor eax, eax
            mov ax, word ptr [esi + 0x3e]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov cx, word ptr [eax + ebp]
            mov ebp, dword ptr [edi + 8]
            lea ecx, [ecx + ecx*4]
            lea ecx, [ebp + ecx*8 + 0xc]
            mov dword ptr [esi + 0x18], ecx
            mov ebp, dword ptr [edi + 0xc]
            xor ecx, ecx
            mov cx, word ptr [eax + ebp + 2]
            mov ebp, dword ptr [edi + 8]
            lea ecx, [ecx + ecx*4]
            lea ecx, [ebp + ecx*8 + 0xc]
            mov dword ptr [esi + 0x1c], ecx
            mov ebp, dword ptr [edi + 0xc]
            xor ecx, ecx
            mov cx, word ptr [eax + ebp + 4]
            lea eax, [ecx + ecx*4]
            mov ecx, dword ptr [edi + 8]
            lea eax, [ecx + eax*8 + 0xc]
            jmp L00460b08
    L00460a6e:
            mov cx, word ptr [esp + 0x60]
            mov ax, word ptr [esp + 0x54]
            mov word ptr [esi + 0x3c], cx
            mov ecx, dword ptr [esp + 0x74]
            mov word ptr [esi + 0x3e], ax
            mov dword ptr [esi + 0x30], 0
            mov eax, dword ptr [ecx + 0x18]
            mov ebp, dword ptr [ebx + eax + 4]
            mov eax, dword ptr [esp + 0x5c]
            cmp word ptr [ebp + eax + 6], 0
            jne L00460aa2
            or dword ptr [esi + 0x30], 1
    L00460aa2:
            mov ebp, dword ptr [ecx + 0x18]
            mov ebp, dword ptr [ebx + ebp + 4]
            cmp word ptr [ebp + eax + 8], 0
            jne L00460ab5
            or dword ptr [esi + 0x30], 2
    L00460ab5:
            mov ecx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [ebx + ecx + 4]
            cmp word ptr [ecx + eax + 0xa], 0
            jne L00460acc
            mov eax, dword ptr [esi + 0x30]
            or al, 4
            mov dword ptr [esi + 0x30], eax
    L00460acc:
            mov eax, dword ptr [edi + 0x4c]
            mov ecx, dword ptr [eax + 0x18]
            mov eax, dword ptr [ecx + ebx + 0x10]
            mov ecx, dword ptr [esp + 0x68]
            lea eax, [eax + ecx + 0xc]
            mov dword ptr [esi + 0x18], eax
            mov ecx, dword ptr [edi + 0x4c]
            mov eax, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [eax + ebx + 0x10]
            mov eax, dword ptr [esp + 0x64]
            lea ecx, [ecx + eax + 0xc]
            mov dword ptr [esi + 0x1c], ecx
            mov eax, dword ptr [edi + 0x4c]
            mov ecx, dword ptr [eax + 0x18]
            mov eax, dword ptr [ecx + ebx + 0x10]
            mov ecx, dword ptr [esp + 0x6c]
            lea eax, [eax + ecx + 0xc]
    L00460b08:
            mov dword ptr [esi + 0x20], eax
            mov eax, dword ptr [edx]
            test eax, eax
            je L00460b23
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L00460b23
            cmp byte ptr [eax], 0x21
            jne L00460b23
            mov dword ptr [gPling_face], edx
    L00460b23:
            mov eax, dword ptr [esp + 0x24c]
            add edx, 0x50
            add esi, 0x50
            dec eax
            mov dword ptr [esp + 0x248], edx
            mov dword ptr [esp + 0x4c], esi
            mov dword ptr [esp + 0x24c], eax
            je L00460b86
    L00460b45:
            mov ecx, dword ptr [esp + 0x5c]
            mov eax, 0x1c
            add ecx, eax
            mov ebp, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x5c], ecx
            mov ecx, dword ptr [esp + 0x240]
            mov edx, dword ptr [esp + 0x54]
            add ebp, eax
            mov ecx, dword ptr [ecx + 0x4c]
            xor esi, esi
            add ecx, 0x18
            inc edx
            mov dword ptr [esp + 0x54], edx
            mov dword ptr [esp + 0x3c], ebp
            mov eax, dword ptr [ecx]
            add eax, ebx
            mov si, word ptr [eax + 0x1c]
            cmp edx, esi
            jl L0046001a
    L00460b86:
            mov eax, dword ptr [esp + 0x24c]
            test eax, eax
            je L00460bb9
            mov ecx, dword ptr [esp + 0x240]
            mov eax, dword ptr [esp + 0x60]
            xor edx, edx
            inc eax
            mov ecx, dword ptr [ecx + 0x4c]
            add ebx, 0x24
            mov dword ptr [esp + 0x60], eax
            mov dword ptr [esp + 0x44], ebx
            mov dx, word ptr [ecx + 8]
            cmp eax, edx
            jl L0045ffdf
    L00460bb9:
            mov eax, dword ptr [esp + 0x24c]
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x22c
            ret 0x14
            nop
            nop
            nop
    }
#endif
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
int C2_NAKED C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
    L0045f8c0:
            sub esp, 0x12c
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            xor ebp, ebp
            mov ebx, ecx
            mov dword ptr [esp + 0x30], 1
            mov eax, dword ptr [edi + 0x18]
            test eax, eax
            je L0045f8e5
            mov dword ptr [esp + 0x10], eax
            jmp L0045f8f0
    L0045f8e5:
            mov eax, dword ptr [esp + 0x140]
            mov dword ptr [esp + 0x10], eax
    L0045f8f0:
            mov ecx, dword ptr [edi + 0x1c]
            test ecx, ecx
            jne L0045f8fe
            mov ecx, dword ptr [esp + 0x144]
    L0045f8fe:
            mov dword ptr [esp + 0x14], ecx
            mov cl, byte ptr [edi + 0x20]
            cmp cl, 1
            jne L0045f91e
            mov eax, dword ptr [esp + 0x14c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045f91e:
            mov ecx, dword ptr [edi + 0x14]
            test ecx, ecx
            je L0045fe6d
            cmp byte ptr [ecx], 0x26
            jne L0045fe6d
            test eax, eax
            jne L0045f94a
            mov eax, dword ptr [esp + 0x14c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045f94a:
            mov ecx, dword ptr [edi + 8]
            test ecx, ecx
            jne L0045fcf0
            cmp byte ptr [edi + 0x12], 1
            jne L0045fef8
            fld dword ptr [eax + 0x34]
            fld dword ptr [eax + 0x3c]
            fld dword ptr [edi + 0x44]
            fxch st(2)
            fmul dword ptr [edi + 0x2c]
            lea ecx, [eax + 0x34]
            fld dword ptr [ecx + 4]
            fld dword ptr [edi + 0x38]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [ecx]
            fstp dword ptr [esp + 0x18]
            fld dword ptr [ecx + 0x10]
            fsub dword ptr [ecx + 4]
            fstp dword ptr [esp + 0x1c]
            fxch st(4)
            fmulp st(3), st(0)
            fld dword ptr [edi + 0x2c]
            fld dword ptr [ecx + 0x14]
            fsub dword ptr [ecx + 8]
            fstp dword ptr [esp + 0x20]
            fxch st(3)
            faddp st(2), st(0)
            fxch st(3)
            fmulp st(3), st(0)
            fxch st(1)
            fcomp dword ptr [gUNK_00589fac]
            fxch st(1)
            faddp st(1), st(0)
            fnstsw ax
            fadd dword ptr [edi + 0x50]
            test ah, 0x41
            jne L0045f9c1
            fld dword ptr [esp + 0x18]
            fmul dword ptr [edi + 0x2c]
            jmp L0045f9c7
    L0045f9c1:
            fld dword ptr [gUNK_00589fac]
    L0045f9c7:
            fld dword ptr [edi + 0x38]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045f9e0
            fld dword ptr [edi + 0x38]
            fmul dword ptr [esp + 0x1c]
            jmp L0045f9e6
    L0045f9e0:
            fld dword ptr [gUNK_00589fac]
    L0045f9e6:
            fld dword ptr [edi + 0x44]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045f9ff
            fld dword ptr [edi + 0x44]
            fmul dword ptr [esp + 0x20]
            jmp L0045fa05
    L0045f9ff:
            fld dword ptr [gUNK_00589fac]
    L0045fa05:
            fld st(3)
            fadd st(0), st(1)
            fadd st(0), st(2)
            fadd st(0), st(3)
            fcomp dword ptr [ebx + 0x2c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            jne L0045fcda
            fld dword ptr [edi + 0x2c]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fa3a
            fld dword ptr [esp + 0x18]
            fmul dword ptr [edi + 0x2c]
            jmp L0045fa40
    L0045fa3a:
            fld dword ptr [gUNK_00589fac]
    L0045fa40:
            fld dword ptr [edi + 0x38]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fa59
            fld dword ptr [edi + 0x38]
            fmul dword ptr [esp + 0x1c]
            jmp L0045fa5f
    L0045fa59:
            fld dword ptr [gUNK_00589fac]
    L0045fa5f:
            fld dword ptr [edi + 0x44]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fa78
            fld dword ptr [edi + 0x44]
            fmul dword ptr [esp + 0x20]
            jmp L0045fa7e
    L0045fa78:
            fld dword ptr [gUNK_00589fac]
    L0045fa7e:
            fxch st(3)
            fadd st(0), st(3)
            fadd st(0), st(1)
            fadd st(0), st(2)
            fcomp dword ptr [ebx + 0x38]
            fxch st(2)
            fstp st(0)
            fxch st(1)
            fstp st(0)
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L0045fef8
            fld dword ptr [ecx + 8]
            fld dword ptr [edi + 0x34]
            fld dword ptr [edi + 0x4c]
            fld dword ptr [ecx + 4]
            fld dword ptr [edi + 0x40]
            fxch st(3)
            fmul dword ptr [ecx]
            fxch st(4)
            fmulp st(2), st(0)
            fmulp st(2), st(0)
            faddp st(2), st(0)
            fld dword ptr [edi + 0x34]
            fcomp dword ptr [gUNK_00589fac]
            faddp st(1), st(0)
            fnstsw ax
            fadd dword ptr [edi + 0x58]
            test ah, 0x41
            jne L0045fad7
            fld dword ptr [edi + 0x34]
            fmul dword ptr [esp + 0x18]
            jmp L0045fadd
    L0045fad7:
            fld dword ptr [gUNK_00589fac]
    L0045fadd:
            fld dword ptr [edi + 0x40]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045faf6
            fld dword ptr [edi + 0x40]
            fmul dword ptr [esp + 0x1c]
            jmp L0045fafc
    L0045faf6:
            fld dword ptr [gUNK_00589fac]
    L0045fafc:
            fld dword ptr [edi + 0x4c]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045fb15
            fld dword ptr [edi + 0x4c]
            fmul dword ptr [esp + 0x20]
            jmp L0045fb1b
    L0045fb15:
            fld dword ptr [gUNK_00589fac]
    L0045fb1b:
            fld st(3)
            fadd st(0), st(1)
            fadd st(0), st(2)
            fadd st(0), st(3)
            fcomp dword ptr [ebx + 0x34]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            jne L0045fcda
            fld dword ptr [edi + 0x34]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fb50
            fld dword ptr [edi + 0x34]
            fmul dword ptr [esp + 0x18]
            jmp L0045fb56
    L0045fb50:
            fld dword ptr [gUNK_00589fac]
    L0045fb56:
            fld dword ptr [edi + 0x40]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fb6f
            fld dword ptr [edi + 0x40]
            fmul dword ptr [esp + 0x1c]
            jmp L0045fb75
    L0045fb6f:
            fld dword ptr [gUNK_00589fac]
    L0045fb75:
            fld dword ptr [edi + 0x4c]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fb8e
            fld dword ptr [edi + 0x4c]
            fmul dword ptr [esp + 0x20]
            jmp L0045fb94
    L0045fb8e:
            fld dword ptr [gUNK_00589fac]
    L0045fb94:
            fxch st(3)
            fadd st(0), st(3)
            fadd st(0), st(1)
            fadd st(0), st(2)
            fcomp dword ptr [ebx + 0x40]
            fxch st(2)
            fstp st(0)
            fxch st(1)
            fstp st(0)
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L0045fef8
            fld dword ptr [ecx + 8]
            fld dword ptr [edi + 0x30]
            fld dword ptr [edi + 0x48]
            fld dword ptr [ecx + 4]
            fld dword ptr [edi + 0x3c]
            fxch st(3)
            fmul dword ptr [ecx]
            fxch st(4)
            fmulp st(2), st(0)
            fmulp st(2), st(0)
            faddp st(2), st(0)
            fld dword ptr [edi + 0x30]
            fcomp dword ptr [gUNK_00589fac]
            faddp st(1), st(0)
            fnstsw ax
            fadd dword ptr [edi + 0x54]
            test ah, 0x41
            jne L0045fbed
            fld dword ptr [edi + 0x30]
            fmul dword ptr [esp + 0x18]
            jmp L0045fbf3
    L0045fbed:
            fld dword ptr [gUNK_00589fac]
    L0045fbf3:
            fld dword ptr [edi + 0x3c]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045fc0c
            fld dword ptr [edi + 0x3c]
            fmul dword ptr [esp + 0x1c]
            jmp L0045fc12
    L0045fc0c:
            fld dword ptr [gUNK_00589fac]
    L0045fc12:
            fld dword ptr [edi + 0x48]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045fc2b
            fld dword ptr [edi + 0x48]
            fmul dword ptr [esp + 0x20]
            jmp L0045fc31
    L0045fc2b:
            fld dword ptr [gUNK_00589fac]
    L0045fc31:
            fld st(3)
            fadd st(0), st(1)
            fadd st(0), st(2)
            fadd st(0), st(3)
            fcomp dword ptr [ebx + 0x30]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            jne L0045fcda
            fld dword ptr [edi + 0x30]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fc66
            fld dword ptr [edi + 0x30]
            fmul dword ptr [esp + 0x18]
            jmp L0045fc6c
    L0045fc66:
            fld dword ptr [gUNK_00589fac]
    L0045fc6c:
            fld dword ptr [edi + 0x3c]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fc85
            fld dword ptr [edi + 0x3c]
            fmul dword ptr [esp + 0x1c]
            jmp L0045fc8b
    L0045fc85:
            fld dword ptr [gUNK_00589fac]
    L0045fc8b:
            fld dword ptr [edi + 0x48]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045fca4
            fld dword ptr [edi + 0x48]
            fmul dword ptr [esp + 0x20]
            jmp L0045fcaa
    L0045fca4:
            fld dword ptr [gUNK_00589fac]
    L0045fcaa:
            fxch st(3)
            fadd st(0), st(3)
            fadd st(0), st(1)
            fadd st(0), st(2)
            fcomp dword ptr [ebx + 0x3c]
            fxch st(2)
            fstp st(0)
            fxch st(1)
            fstp st(0)
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L0045fcf0
            mov eax, dword ptr [esp + 0x14c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045fcda:
            fstp st(0)
            mov eax, dword ptr [esp + 0x14c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045fcf0:
            mov eax, dword ptr [esp + 0x150]
            lea esi, [edi + 0x2c]
            test eax, eax
            jne L0045fd07
            mov dword ptr [esp + 0x150], esi
            jmp L0045fd27
    L0045fd07:
            push eax
            lea edx, [esp + 0xb0]
            push esi
            push edx
            call BrMatrix34Mul
            lea eax, [esp + 0xb8]
            add esp, 0xc
            mov dword ptr [esp + 0x150], eax
    L0045fd27:
            lea ecx, [esp + 0x7c]
            push esi
            push ecx
            call BrMatrix34LPInverse
            mov eax, dword ptr [ebx]
            add esp, 8
            lea edx, [esp + 0x7c]
            lea ecx, [esp + 0x10c]
            push edx
            push eax
            push ecx
            call BrMatrix34Mul
            mov ecx, dword ptr [ebx + 8]
            lea eax, [ebx + 4]
            lea edx, [esp + 0x118]
            mov dword ptr [esp + 0x40], eax
            mov eax, dword ptr [eax]
            mov dword ptr [esp + 0x44], edx
            mov edx, dword ptr [ebx + 0xc]
            mov dword ptr [esp + 0x48], eax
            mov eax, dword ptr [ebx + 0x10]
            mov dword ptr [esp + 0x4c], ecx
            mov ecx, dword ptr [ebx + 0x14]
            mov dword ptr [esp + 0x50], edx
            mov edx, dword ptr [ebx + 0x18]
            add esp, 0xc
            mov dword ptr [esp + 0x48], eax
            mov dword ptr [esp + 0x4c], ecx
            lea eax, [esp + 0x7c]
            mov dword ptr [esp + 0x50], edx
            lea ecx, [ebx + 0x1c]
            push eax
            lea edx, [esp + 0x58]
            push ecx
            push edx
            call BrMatrix34ApplyP
            mov ecx, dword ptr [esp + 0x44]
            mov eax, dword ptr [ebx + 0x28]
            add esp, 0xc
            lea edx, [esp + 0x3c]
            mov dword ptr [esp + 0x60], eax
            push ecx
            lea ecx, [esp + 0x68]
            call GetNewBoundingBox
            mov edx, dword ptr [edi + 0x14]
            mov al, byte ptr [edx + 1]
            cmp al, 0x30
            jl L0045fe65
            cmp al, 0x39
            jg L0045fe65
            mov eax, dword ptr [esp + 0x10]
            lea ecx, [esp + 0x64]
            lea edx, [eax + 0x34]
            call BoundsOverlapTest__finteray
            test eax, eax
            je L0045fef8
            mov ecx, dword ptr [ebx]
            lea edx, [esp + 0x7c]
            push ecx
            push edx
            call BrMatrix34LPInverse
            add esp, 8
            lea eax, [esp + 0x7c]
            lea ecx, [esp + 0xdc]
            push eax
            push esi
            push ecx
            call BrMatrix34Mul
            add esp, 0xc
            lea edx, [esp + 0xdc]
            lea ecx, [esp + 0x18]
            push edx
            mov edx, dword ptr [edi + 0x18]
            add edx, 0x34
            call GetNewBoundingBox
            lea ecx, [esp + 0x18]
            mov edx, dword ptr [esp + 0x34]
            call BoundsOverlapTest__finteray
            test eax, eax
            je L0045fef8
            mov eax, dword ptr [esp + 0x154]
            test eax, eax
            je L0045fe65
            mov eax, dword ptr [eax + 0x10]
            test eax, eax
            je L0045fe65
            mov ecx, edi
            call eax
            test eax, eax
            je L0045fe65
            mov eax, dword ptr [esp + 0x14c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045fe65:
            mov eax, dword ptr [esp + 0x10]
            lea ebx, [esp + 0x38]
    L0045fe6d:
            mov cl, byte ptr [edi + 0x12]
            cmp cl, 1
            je L0045fef4
            cmp cl, 4
            jbe L0045fe8e
            cmp cl, 6
            ja L0045fe8e
            mov edx, dword ptr [edi + 0x5c]
            lea ecx, [ebx + 0x2c]
            call BoundsOverlapTest__finteray
            mov dword ptr [esp + 0x30], eax
    L0045fe8e:
            mov esi, dword ptr [esp + 0x14c]
    L0045fe95:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L0045fee5
            mov edx, dword ptr [edi + 8]
            test edx, edx
            je L0045fee5
    L0045fea4:
            mov eax, dword ptr [esp + 0x154]
            mov ecx, dword ptr [esp + 0x150]
            push eax
            lea eax, [ebp + ebp*4]
            push ecx
            mov ecx, dword ptr [esp + 0x150]
            mov edi, dword ptr [edx]
            push esi
            shl eax, 4
            add eax, ecx
            mov ecx, dword ptr [esp + 0x20]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            push eax
            mov ecx, ebx
            call L0045f8c0
            sub esi, eax
            mov edx, edi
            add ebp, esi
            mov esi, eax
            test edi, edi
            jne L0045fea4
    L0045fee5:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045fef4:
            test eax, eax
            jne L0045ff0c
    L0045fef8:
            mov eax, dword ptr [esp + 0x14c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret 0x18
    L0045ff0c:
            mov eax, dword ptr [gActorBoxPick_StopGroovidelics]
            test eax, eax
            je L0045fe8e
            mov esi, dword ptr [esp + 0x10]
            lea ecx, [ebx + 0x2c]
            lea edx, [esi + 0x34]
            call BoundsOverlapTest__finteray
            test eax, eax
            je L0045fe8e
            mov ecx, dword ptr [esp + 0x150]
            mov ebp, dword ptr [esp + 0x14c]
            mov edx, dword ptr [esp + 0x148]
            mov eax, dword ptr [esp + 0x14]
            push ecx
            push ebp
            push edx
            push eax
            push esi
            mov edx, ebx
            mov ecx, edi
            call ModelPickBox
            mov esi, eax
            mov eax, dword ptr [esp + 0x150]
            test eax, eax
            je L0045ff80
            mov ecx, ebp
            sub ecx, esi
            je L0045ff80
            mov eax, dword ptr [esp + 0x154]
            test eax, eax
            je L0045ff80
            mov eax, dword ptr [eax + 0x14]
            test eax, eax
            je L0045ff80
            mov ecx, edi
            call eax
    L0045ff80:
            sub ebp, esi
            jmp L0045fe95
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push esi
            push edi
            mov edi, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0xc]
            fld dword ptr [edi + 4]
            fld dword ptr [edi]
            fsub dword ptr [ecx]
            fxch st(1)
            fsub dword ptr [ecx + 4]
            fld dword ptr [edi + 8]
            fld dword ptr [esi + 4]
            fxch st(1)
            fsub dword ptr [ecx + 8]
            fxch st(3)
            fmul dword ptr [esi]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [esi + 8]
            faddp st(1), st(0)
            fdiv dword ptr [esp + 0x10]
            fst dword ptr [esp + 0x14]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            je L0045deff
            mov eax, dword ptr [esp + 0x1c]
            mov edx, dword ptr [ecx]
            mov dword ptr [eax], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [eax + 8], ecx
            jmp L0045df60
    L0045deff:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 1
            jne L0045df26
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [edx]
            mov dword ptr [eax], ecx
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [eax + 4], ecx
            mov edx, dword ptr [edx + 8]
            mov dword ptr [eax + 8], edx
            jmp L0045df60
    L0045df26:
            fld dword ptr [esi]
            fmul dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x1c]
            fstp dword ptr [eax]
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [eax]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fld dword ptr [esi + 8]
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [eax + 8]
            fadd dword ptr [ecx]
            fstp dword ptr [eax]
            fld dword ptr [ecx + 4]
            fadd dword ptr [eax + 4]
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fadd dword ptr [eax + 8]
            fstp dword ptr [eax + 8]
    L0045df60:
            fld dword ptr [eax]
            fsub dword ptr [edi]
            mov ecx, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x18]
            fstp dword ptr [ecx]
            fld dword ptr [eax + 4]
            fsub dword ptr [edi + 4]
            mov esi, dword ptr [ecx]
            mov dword ptr [esp + 0xc], esi
            fstp dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            fsub dword ptr [edi + 8]
            fld dword ptr [ecx + 4]
            fstp dword ptr [esp + 0x1c]
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fstp dword ptr [ecx + 8]
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x1c]
            fstp st(0)
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L0045e017
            fld dword ptr [edx]
            fmul dword ptr [gFLOAT_00589ff4]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [gFLOAT_00589ff8]
            fsubr dword ptr [edx + 0x10]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L0045e017
            fld dword ptr [esp + 0x1c]
            fsqrt
            mov eax, dword ptr [esp + 0x24]
            mov dword ptr [eax], 0x3f800000
            fld dword ptr [ecx]
            fdiv st(0), st(1)
            fstp dword ptr [ecx]
            fld dword ptr [ecx + 4]
            fdiv st(0), st(1)
            fstp dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fdiv st(0), st(1)
            fstp dword ptr [ecx + 8]
            fstp st(0)
    L0045e017:
            pop edi
            pop esi
            ret 0x1c
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x0045e020
void C2_NAKED C2_HOOK_FASTCALL FUN_0045e020(br_vector3* pVertex, br_vector3* pSphere_start, br_vector3* pSphere_end, br_vector3* pSphere_direction, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x10
            push esi
            mov esi, dword ptr [esp + 0x1c]
            push edi
            mov edi, dword ptr [esp + 0x24]
            fld dword ptr [esi + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [esi + 8]
            fxch st(2)
            fsub dword ptr [edx + 4]
            fld dword ptr [ecx]
            fxch st(2)
            fsub dword ptr [edx + 8]
            fld dword ptr [esi]
            fxch st(3)
            fsub dword ptr [edx]
            fst dword ptr [esp + 0xc]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(5)
            fst dword ptr [esp + 0x20]
            fxch st(4)
            fst dword ptr [esp + 0x24]
            fld st(3)
            fxch st(5)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esi + 8]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            faddp st(1), st(0)
            fxch st(5)
            fmul st(0), st(4)
            faddp st(5), st(0)
            fmul dword ptr [esp + 0x14]
            fxch st(4)
            fstp dword ptr [esp + 0x20]
            fld dword ptr [esi]
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + 0x10]
            faddp st(5), st(0)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            faddp st(4), st(0)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fdiv dword ptr [esp + 0x20]
            fst dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 8]
            fstp st(0)
            fld dword ptr [esp + 8]
            fcomp dword ptr [edi + 0x10]
            fnstsw ax
            test ah, 0x41
            je L0045e1f4
            fld dword ptr [edi + 0x10]
            fsub dword ptr [esp + 8]
            fdiv dword ptr [esp + 0x20]
            fsqrt
            fsubr dword ptr [esp + 0x24]
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045e1f4
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L0045e1f4
            mov eax, dword ptr [esp + 0x28]
            mov edi, dword ptr [ecx]
            mov dword ptr [eax], edi
            mov edi, dword ptr [ecx + 4]
            mov dword ptr [eax + 4], edi
            fld dword ptr [ecx + 8]
            fst dword ptr [eax + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esi + 8]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fadd dword ptr [edx]
            fxch st(2)
            fadd dword ptr [edx + 4]
            fxch st(1)
            fadd dword ptr [edx + 8]
            fxch st(2)
            fsubr dword ptr [eax]
            fxch st(1)
            fsubr dword ptr [eax + 4]
            fxch st(2)
            fsubr st(0), st(3)
            fxch st(1)
            fstp st(3)
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 0x41
            jne L0045e1d0
            fdivr qword ptr [gDouble_00589fe8]
            mov eax, dword ptr [esp + 0x2c]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [eax]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fstp dword ptr [eax + 8]
            mov eax, dword ptr [esp + 0x30]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [esp + 0x20]
            fstp dword ptr [eax]
            pop edi
            pop esi
            add esp, 0x10
            ret 0x18
    L0045e1d0:
            mov eax, dword ptr [esp + 0x2c]
            xor ecx, ecx
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [esp + 0x20]
            mov dword ptr [eax], 0x3f800000
            mov dword ptr [eax + 4], ecx
            mov dword ptr [eax + 8], ecx
            mov eax, dword ptr [esp + 0x30]
            fstp dword ptr [eax]
    L0045e1f4:
            pop edi
            pop esi
            add esp, 0x10
            ret 0x18
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x0045e850
void C2_NAKED C2_HOOK_FASTCALL SphereFaceCollide(tFace_ref* pFace, br_vector3* pStart, br_vector3* pEnd, br_scalar pRadius, void* pUnused_point, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x34
            push ebx
            mov ebx, dword ptr [esp + 0x4c]
            push esi
            push edi
            mov dword ptr [ebx], 0x42c80000
            mov esi, dword ptr [ecx]
            test esi, esi
            je L0045e887
            mov edi, dword ptr [esi + 4]
            test edi, edi
            je L0045e887
            mov al, byte ptr [edi]
            cmp al, 0x21
            je L0045ecb1
            cmp al, 0x3e
            je L0045ecb1
            cmp al, 0x3f
            je L0045ecb1
    L0045e887:
            fld dword ptr [ecx + 0x38]
            fld dword ptr [edx + 4]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [ecx + 0x3c]
            fld dword ptr [edx + 8]
            fxch st(2)
            fmul dword ptr [edx]
            fxch st(3)
            fmulp st(4), st(0)
            fxch st(3)
            faddp st(2), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsub dword ptr [ecx + 0x44]
            fst dword ptr [esp + 0x54]
            fcomp dword ptr [gUNK_00589fac]
            fld dword ptr [esp + 0x54]
            fnstsw ax
            test ah, 1
            je L0045e931
            fld dword ptr [esp + 0x48]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            je L0045ecb1
            test esi, esi
            je L0045ecb1
            mov eax, dword ptr [esi + 0x20]
            test ah, 0x10
            je L0045ecb1
            fld dword ptr [ecx + 0x38]
            fld dword ptr [ecx + 0x3c]
            mov esi, dword ptr [esp + 0x44]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fxch st(4)
            fchs
            fxch st(3)
            fchs
            fxch st(3)
            fstp dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fchs
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fmul dword ptr [esi]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [ecx + 0x44]
            jmp L0045e978
    L0045e931:
            fcomp dword ptr [esp + 0x48]
            fnstsw ax
            test ah, 1
            jne L0045ecb1
            fld dword ptr [ecx + 0x3c]
            mov esi, dword ptr [esp + 0x44]
            mov eax, dword ptr [ecx + 0x38]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x2c], eax
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fmul dword ptr [esi]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsub dword ptr [ecx + 0x44]
    L0045e978:
            fld dword ptr [esp + 0x48]
            fsub qword ptr [gDouble_00589fe0]
            fld st(1)
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045ecaf
            fld dword ptr [esp + 0x48]
            fsub dword ptr [esp + 0x54]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fdivp st(1), st(0)
            fst dword ptr [esp + 0x48]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            jne L0045e9bc
            mov dword ptr [esp + 0x48], 0x3f800000
    L0045e9bc:
            fld dword ptr [esp + 0x48]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045e9d5
            mov dword ptr [esp + 0x48], 0
    L0045e9d5:
            fld dword ptr [esi + 4]
            fld dword ptr [esi]
            fsub dword ptr [edx]
            fxch st(1)
            fsub dword ptr [edx + 4]
            fld dword ptr [esi + 8]
            fsub dword ptr [edx + 8]
            fxch st(2)
            fmul dword ptr [esp + 0x48]
            fxch st(1)
            fmul dword ptr [esp + 0x48]
            fxch st(2)
            fmul dword ptr [esp + 0x48]
            fld st(1)
            fxch st(3)
            fst dword ptr [esp + 0x38]
            fxch st(3)
            fadd dword ptr [edx]
            fxch st(3)
            fadd dword ptr [edx + 4]
            fxch st(1)
            fst dword ptr [esp + 0x3c]
            fadd dword ptr [edx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fxch st(5)
            fstp dword ptr [esp + 0x34]
            fsub dword ptr [esp + 0x34]
            fxch st(2)
            fstp dword ptr [esp + 0x38]
            fxch st(3)
            fsub dword ptr [esp + 0x38]
            fld dword ptr [ecx + 0xc]
            fxch st(4)
            fstp dword ptr [esp + 0x3c]
            fxch st(3)
            fsub dword ptr [esp + 0x3c]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(3)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x28]
            fxch st(3)
            faddp st(5), st(0)
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fxch st(3)
            faddp st(6), st(0)
            fld dword ptr [esp + 0x30]
            fxch st(6)
            fstp dword ptr [esp + 0x44]
            fmul dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(5)
            fmul dword ptr [esp + 0x44]
            fxch st(2)
            fadd dword ptr [esp + 0x34]
            fxch st(5)
            fadd dword ptr [esp + 0x38]
            fxch st(2)
            fadd dword ptr [esp + 0x3c]
            fxch st(5)
            fstp dword ptr [esp + 0x34]
            xor edx, edx
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fabs
            fst dword ptr [esp + 0x44]
            fxch st(3)
            fstp dword ptr [esp + 0x3c]
            fabs
            fst dword ptr [esp + 0x44]
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fcompp
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0045eaca
            mov edx, 1
    L0045eaca:
            fld dword ptr [esp + edx*4 + 0x28]
            fld dword ptr [esp + 0x30]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x44]
            fcompp
            fnstsw ax
            test ah, 1
            je L0045eaea
            mov edx, 2
    L0045eaea:
            sub edx, 0
            je L0045eb07
            dec edx
            je L0045eafe
            dec edx
            jne L0045eb13
            xor eax, eax
            mov edx, 1
            jmp L0045eb1b
    L0045eafe:
            xor eax, eax
            mov edx, 2
            jmp L0045eb1b
    L0045eb07:
            mov eax, 1
            mov edx, 2
            jmp L0045eb1b
    L0045eb13:
            mov eax, dword ptr [esp + 0x48]
            mov edx, dword ptr [esp + 0x48]
    L0045eb1b:
            fld dword ptr [ecx + edx*4 + 4]
            fld dword ptr [ecx + eax*4 + 4]
            fld dword ptr [ecx + edx*4 + 0x1c]
            fld dword ptr [ecx + eax*4 + 0x10]
            fld dword ptr [ecx + edx*4 + 0x10]
            fxch st(4)
            fstp dword ptr [esp + 0x54]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fxch st(3)
            fsub dword ptr [esp + 0x54]
            fld dword ptr [ecx + eax*4 + 0x1c]
            fxch st(3)
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fsub dword ptr [esp + 0x44]
            fxch st(2)
            fsub dword ptr [esp + 0x44]
            fld dword ptr [esp + edx*4 + 0x34]
            fxch st(4)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + eax*4 + 0x34]
            fxch st(4)
            fstp dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fld st(2)
            fxch st(2)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            fsub dword ptr [esp + 0x44]
            fxch st(4)
            fsub dword ptr [esp + 0x54]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0xc]
            fxch st(3)
            fstp dword ptr [esp + 0xc]
            fsub dword ptr [esp + 0xc]
            fxch st(3)
            fstp dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x44]
            fxch st(2)
            fsubp st(1), st(0)
            fxch st(1)
            fabs
            fmul qword ptr [gDouble_0058a008]
            fxch st(1)
            fst dword ptr [esp + 0x54]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045ecb1
            fld dword ptr [esp + 0x44]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045ecb1
            fld dword ptr [esp + 0x54]
            fdiv dword ptr [esp + 0x44]
            fst dword ptr [esp + 0x24]
            fcomp qword ptr [gDouble_0058a010]
            fnstsw ax
            test ah, 1
            jne L0045ecb1
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fsub dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fsubp st(2), st(0)
            fabs
            fmul qword ptr [gDouble_0058a008]
            fxch st(1)
            fst dword ptr [esp + 0x54]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L0045ecb1
            fld dword ptr [esp + 0x44]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L0045ecb1
            fld dword ptr [esp + 0x54]
            fdiv dword ptr [esp + 0x44]
            fcom qword ptr [gDouble_0058a010]
            fnstsw ax
            test ah, 1
            jne L0045ecaf
            fld dword ptr [esp + 0x24]
            fadd st(0), st(1)
            fcomp qword ptr [gDouble_0058a018]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L0045ecb1
            mov ecx, dword ptr [esp + 0x48]
            mov eax, dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x2c]
            mov dword ptr [ebx], ecx
            mov ecx, dword ptr [esp + 0x30]
            mov dword ptr [eax + 4], edx
            fstp dword ptr [eax]
            mov dword ptr [eax + 8], ecx
            pop edi
            pop esi
            pop ebx
            add esp, 0x34
            ret 0x14
    L0045ecaf:
            fstp st(0)
    L0045ecb1:
            pop edi
            pop esi
            pop ebx
            add esp, 0x34
            ret 0x14
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x00460da0
int C2_NAKED C2_HOOK_FASTCALL SphereBoxIntersection(br_bounds* pBounds, br_vector3* pPoint, br_scalar pRadius_squared, br_vector3* pClosest) {
#ifndef CARPOCALYPSE2_MATCHING
    int axis;
    br_scalar closest;
    br_scalar distance;

    distance = 0.f;
    for (axis = 0; axis < 3; axis++) {
        if (pPoint->v[axis] <= pBounds->min.v[axis]) {
            closest = pBounds->min.v[axis];
        } else if (pPoint->v[axis] >= pBounds->max.v[axis]) {
            closest = pBounds->max.v[axis];
        } else {
            closest = pPoint->v[axis];
        }
        pClosest->v[axis] = closest;
        distance += (pPoint->v[axis] - closest) * (pPoint->v[axis] - closest);
    }
    return distance <= pRadius_squared;
#else
    __asm {
            fld dword ptr [gUNK_00589fac]
            push esi
            mov esi, dword ptr [esp + 0xc]
            push edi
            sub esi, ecx
            sub edx, ecx
            mov edi, 3
    L00460db5:
            fld dword ptr [edx + ecx]
            fcomp dword ptr [ecx]
            fnstsw ax
            test ah, 0x41
            je L00460dc5
            fld dword ptr [ecx]
            jmp L00460dda
    L00460dc5:
            fld dword ptr [edx + ecx]
            fcomp dword ptr [ecx + 0xc]
            fnstsw ax
            test ah, 1
            jne L00460dd7
            fld dword ptr [ecx + 0xc]
            jmp L00460dda
    L00460dd7:
            fld dword ptr [edx + ecx]
    L00460dda:
            fst dword ptr [esi + ecx]
            fsubr dword ptr [edx + ecx]
            fld st(0)
            fmul st(0), st(1)
            add ecx, 4
            dec edi
            faddp st(2), st(0)
            fstp st(0)
            jne L00460db5
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 0x41
            je L00460e03
            mov eax, 1
            pop edi
            pop esi
            ret 8
    L00460e03:
            pop edi
            xor eax, eax
            pop esi
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x00460e10
int C2_NAKED C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x18]
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            mov edi, dword ptr [esp + 0x18]
            push ebp
            push edi
            mov ebx, edx
            push esi
            call LineBoxColl
            xor edx, edx
            cmp eax, edx
            je L00460e66
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [ecx], edx
            mov dword ptr [ecx + 4], edx
            mov dword ptr [ecx + 8], edx
            lea edx, [eax - 1]
            and edx, 3
            test al, 4
            je L00460e56
            fld dword ptr [gUNK_00589fb0]
            fstp dword ptr [ecx + edx*4]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L00460e56:
            fld dword ptr [gUNK_00589fb8]
            fstp dword ptr [ecx + edx*4]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L00460e66:
            push edi
            push 0x37d1b717
            mov edx, ebx
            mov ecx, esi
            call SphereBoxIntersection
            test eax, eax
            je L00460f57
            mov edx, dword ptr [esp + 0x1c]
            mov ebx, edi
            mov dword ptr [ebp], 0x3f800000
            mov edi, edx
            xor ebp, ebp
            sub ebx, esi
            mov ecx, esi
            sub edi, esi
            mov dword ptr [edx], ebp
            mov dword ptr [edx + 4], ebp
            mov dword ptr [edx + 8], ebp
            mov esi, 3
    L00460ea1:
            fld dword ptr [ebx + ecx]
            fcomp dword ptr [ecx + 0xc]
            fnstsw ax
            test ah, 0x40
            je L00460eb7
            mov dword ptr [edi + ecx], 0xbf800000
            jmp L00460eca
    L00460eb7:
            fld dword ptr [ebx + ecx]
            fcomp dword ptr [ecx]
            fnstsw ax
            test ah, 0x40
            je L00460eca
            mov dword ptr [edi + ecx], 0x3f800000
    L00460eca:
            add ecx, 4
            dec esi
            jne L00460ea1
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edx]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 0x41
            jne L00460f3d
            fdivr qword ptr [gDouble_00589fe8]
            mov eax, 0x40
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fld st(2)
            fmul dword ptr [edx]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [edx]
            fxch st(1)
            fstp dword ptr [edx + 4]
            fstp dword ptr [edx + 8]
            fstp st(0)
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L00460f3d:
            fstp st(0)
            mov dword ptr [edx], 0x3f800000
            mov dword ptr [edx + 4], ebp
            mov dword ptr [edx + 8], ebp
            mov eax, 0x40
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L00460f57:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret 0x10
    }
#endif
}


// FUNCTION: CARMA2_HW  0x00460f60
int C2_NAKED C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x1c
            push esi
            mov esi, dword ptr [esp + 0x24]
            push edi
            fld dword ptr [esi + 4]
            fsub dword ptr [ecx]
            fld dword ptr [esi + 8]
            fsub dword ptr [ecx + 4]
            fld dword ptr [esi + 0xc]
            fsub dword ptr [ecx + 8]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fst dword ptr [esp + 8]
            fcomp dword ptr [esi + 0x10]
            fnstsw ax
            test ah, 1
            je L00460fbd
            or eax, 0xffffffff
            pop edi
            pop esi
            add esp, 0x1c
            ret 0x10
    L00460fbd:
            fld dword ptr [edx + 4]
            fld dword ptr [edx]
            fsub dword ptr [ecx]
            fxch st(1)
            fsub dword ptr [ecx + 4]
            fld dword ptr [edx + 8]
            fsub dword ptr [ecx + 8]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x28]
            fcomp qword ptr [gDouble_00589fd8]
            fnstsw ax
            test ah, 1
            jne L0046119c
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fdiv dword ptr [esp + 0x28]
            fld st(0)
            fmul st(0), st(1)
            fmul dword ptr [esp + 0x28]
            fsubr dword ptr [esp + 8]
            fcom dword ptr [esi + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L0046105a
            fstp st(0)
            fstp st(0)
            xor eax, eax
            pop edi
            pop esi
            add esp, 0x1c
            ret 0x10
    L0046105a:
            fsubr dword ptr [esi + 0x10]
            mov eax, dword ptr [esp + 0x34]
            fdiv dword ptr [esp + 0x28]
            fsqrt
            fsubr st(0), st(1)
            fstp dword ptr [esp + 0x28]
            fstp st(0)
            fld dword ptr [esp + 0x28]
            fcomp dword ptr [gUNK_00589fac]
            fld dword ptr [esp + 0x28]
            fstp dword ptr [eax]
            fnstsw ax
            test ah, 1
            jne L0046119c
            fld dword ptr [esp + 0x28]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L0046119c
            fld dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fadd dword ptr [ecx]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x2c]
            xor edx, edx
            fxch st(1)
            fstp dword ptr [eax]
            fadd dword ptr [ecx + 4]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fadd dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 0x30]
            cmp ecx, edx
            fstp dword ptr [eax + 8]
            je L0046118f
            fld dword ptr [esi + 4]
            fsub dword ptr [eax]
            fstp dword ptr [ecx]
            fld dword ptr [esi + 8]
            fsub dword ptr [eax + 4]
            mov edi, dword ptr [ecx]
            mov dword ptr [esp + 0x2c], edi
            fstp dword ptr [ecx + 4]
            fld dword ptr [esi + 0xc]
            fsub dword ptr [eax + 8]
            fld dword ptr [ecx + 4]
            fstp dword ptr [esp + 0x28]
            fst dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x28]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x34]
            fxch st(3)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fstp dword ptr [ecx + 8]
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 0x41
            jne L00461181
            fdivr qword ptr [gDouble_00589fe8]
            mov eax, 1
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld st(2)
            fmul dword ptr [ecx]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [ecx]
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            fstp st(0)
            pop edi
            pop esi
            add esp, 0x1c
            ret 0x10
    L00461181:
            fstp st(0)
            mov dword ptr [ecx], 0x3f800000
            mov dword ptr [ecx + 4], edx
            mov dword ptr [ecx + 8], edx
    L0046118f:
            mov eax, 1
            pop edi
            pop esi
            add esp, 0x1c
            ret 0x10
    L0046119c:
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x1c
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x004611b0
int C2_NAKED C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, dword ptr [esp + 0x10]
            push ebx
            mov ebx, dword ptr [esp + 0xc]
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x18]
            push edi
            mov edi, dword ptr [esp + 0x14]
            push eax
            push esi
            push ebx
            mov ebp, edx
            push edi
            call LineSphereColl
            xor ecx, ecx
            cmp eax, ecx
            jne L00461304
            fld dword ptr [edi + 4]
            fsub dword ptr [ebp]
            fstp dword ptr [esi]
            fld dword ptr [edi + 8]
            fsub dword ptr [ebp + 4]
            mov edx, dword ptr [esi]
            mov dword ptr [esp + 0x14], edx
            fstp dword ptr [esi + 4]
            fld dword ptr [edi + 0xc]
            fsub dword ptr [ebp + 8]
            fld dword ptr [esi + 4]
            fstp dword ptr [esp + 0x1c]
            fst dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esi + 8]
            fld dword ptr [edi + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [gFLOAT_00589ff4]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [gFLOAT_00589ff8]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fsubr dword ptr [edi + 0x10]
            fxch st(1)
            fstp st(2)
            fld st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L00461260
            fstp st(0)
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L00461260:
            fcomp dword ptr [edi + 0x10]
            fnstsw ax
            test ah, 1
            je L00461273
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L00461273:
            fld dword ptr [esp + 0x1c]
            fsqrt
            fcom dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 0x41
            jne L004612ae
            fdivr qword ptr [gDouble_00589fe8]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld st(2)
            fmul dword ptr [esi]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi + 8]
            fstp st(0)
            jmp L004612bc
    L004612ae:
            fstp st(0)
            mov dword ptr [esi], 0x3f800000
            mov dword ptr [esi + 4], ecx
            mov dword ptr [esi + 8], ecx
    L004612bc:
            fld dword ptr [edi]
            fmul dword ptr [esi]
            mov ecx, dword ptr [esp + 0x20]
            mov eax, 2
            fstp dword ptr [ebx]
            fld dword ptr [esi + 4]
            fmul dword ptr [edi]
            fld dword ptr [ebx]
            fxch st(1)
            fstp dword ptr [ebx + 4]
            fld dword ptr [esi + 8]
            fmul dword ptr [edi]
            fld dword ptr [ebx + 4]
            fxch st(1)
            fstp dword ptr [ebx + 8]
            fxch st(1)
            fadd dword ptr [edi + 4]
            fld dword ptr [ebx + 8]
            fxch st(1)
            fstp dword ptr [ebx]
            fxch st(1)
            fadd dword ptr [edi + 8]
            fstp dword ptr [ebx + 4]
            fadd dword ptr [edi + 0xc]
            fstp dword ptr [ebx + 8]
            mov dword ptr [ecx], 0x3f800000
    L00461304:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x00461310
int C2_NAKED C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x214
            mov eax, dword ptr [esp + 0x224]
            push ebx
            fld dword ptr [gUNK_00589fac]
            mov dword ptr [eax], 0x40000000
            mov eax, dword ptr [esp + 0x21c]
            fld dword ptr [edx]
            fsub dword ptr [ecx]
            mov ebx, dword ptr [eax + 8]
            push ebp
            push esi
            push edi
            or edi, 0xffffffff
            xor esi, esi
            fstp dword ptr [esp + 0x10]
            fld dword ptr [edx + 4]
            fsub dword ptr [ecx + 4]
            test ebx, ebx
            mov ebp, 1
            mov dword ptr [esp + 0x1c], edi
            fstp dword ptr [esp + 0x14]
            fld dword ptr [edx + 8]
            fsub dword ptr [ecx + 8]
            fstp dword ptr [esp + 0x18]
            jle L00461439
            mov edx, dword ptr [eax + 0x14]
    L0046136c:
            fld dword ptr [edx]
            fld dword ptr [ecx + 4]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 8]
            fmul dword ptr [esp + 0x18]
            fld dword ptr [edx + 4]
            fmul dword ptr [esp + 0x14]
            faddp st(1), st(0)
            fld dword ptr [edx]
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fst dword ptr [esp + esi*4 + 0x124]
            fxch st(4)
            fmul dword ptr [ecx]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 8]
            fld dword ptr [edx + 0xc]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fadd dword ptr [esp + 0x230]
            fstp dword ptr [esp + 0x20]
            fadd dword ptr [esp + 0x20]
            fcom dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L004613cc
            xor ebp, ebp
    L004613cc:
            fld st(1)
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L004613e5
            fdiv st(0), st(1)
            fchs
            fstp dword ptr [esp + esi*4 + 0x24]
            jmp L004613f6
    L004613e5:
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L00461503
    L004613f6:
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L00461429
            fcom dword ptr [esp + esi*4 + 0x24]
            lea edi, [esp + esi*4 + 0x24]
            fnstsw ax
            test ah, 0x41
            je L00461429
            fstp st(0)
            fld dword ptr [edi]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L00461492
            fld dword ptr [edi]
            mov dword ptr [esp + 0x1c], esi
    L00461429:
            inc esi
            add edx, 0x10
            cmp esi, ebx
            jl L0046136c
            mov edi, dword ptr [esp + 0x1c]
    L00461439:
            test ebp, ebp
            je L0046144f
            fstp st(0)
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x214
            ret 0x10
    L0046144f:
            test edi, edi
            jge L00461464
            fstp st(0)
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x214
            ret 0x10
    L00461464:
            xor edx, edx
            test ebx, ebx
            jle L004614a1
    L0046146a:
            fld dword ptr [esp + edx*4 + 0x124]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L00461489
            fcom dword ptr [esp + edx*4 + 0x24]
            fnstsw ax
            test ah, 0x41
            je L00461490
    L00461489:
            inc edx
            cmp edx, ebx
            jl L0046146a
            jmp L004614a1
    L00461490:
            fstp st(0)
    L00461492:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x214
            ret 0x10
    L004614a1:
            fld st(0)
            fld st(1)
            fld st(2)
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x22c]
            lea eax, [edi + 1]
            fld st(1)
            fld st(3)
            fld st(2)
            fxch st(2)
            fstp dword ptr [edx]
            fstp dword ptr [edx + 4]
            fstp dword ptr [edx + 8]
            fld dword ptr [ecx]
            fadd st(0), st(2)
            fstp dword ptr [edx]
            fxch st(2)
            fadd dword ptr [ecx + 4]
            fstp dword ptr [edx + 4]
            fxch st(1)
            fadd dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 0x234]
            fstp dword ptr [edx + 8]
            fxch st(1)
            fstp dword ptr [ecx]
            fstp st(0)
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x214
            ret 0x10
    L00461503:
            pop edi
            pop esi
            fstp st(0)
            pop ebp
            xor eax, eax
            fstp st(0)
            pop ebx
            add esp, 0x214
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


// FUNCTION: CARMA2_HW  0x00461520
int C2_NAKED C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, dword ptr [esp + 0x10]
            sub esp, 0x108
            push ebx
            mov ebx, dword ptr [esp + 0x110]
            push ebp
            push esi
            mov ebp, edx
            push edi
            mov edx, dword ptr [esp + 0x120]
            push eax
            lea esi, [ebx + 0x38]
            push 0
            push edx
            push esi
            mov edx, ebp
            call LinePolyColl
            mov edi, eax
            xor edx, edx
            cmp edi, edx
            je L004615b8
            jge L0046157d
            mov eax, dword ptr [esp + 0x124]
            push edx
            push edx
            push 0x3f800000
            push eax
            call BrVector3SetFloat
            add esp, 0x10
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 0x10
    L0046157d:
            mov ecx, dword ptr [ebx + 0x4c]
            mov eax, edi
            and eax, 0x3f
            dec eax
            shl eax, 4
            mov edx, dword ptr [eax + ecx]
            mov ecx, dword ptr [esp + 0x124]
            mov dword ptr [ecx], edx
            mov edx, dword ptr [ebx + 0x4c]
            mov edx, dword ptr [eax + edx + 4]
            mov dword ptr [ecx + 4], edx
            mov edx, dword ptr [ebx + 0x4c]
            mov eax, dword ptr [eax + edx + 8]
            mov dword ptr [ecx + 8], eax
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 0x10
    L004615b8:
            mov ebx, dword ptr [esi + 8]
            cmp ebx, edx
            jle L00461608
            mov ecx, dword ptr [esi + 0x14]
            lea edi, [esp + 0x18]
    L004615c6:
            fld dword ptr [ecx]
            fld dword ptr [ecx + 8]
            fld dword ptr [ebp + 8]
            fld dword ptr [ecx + 4]
            fxch st(3)
            fmul dword ptr [ebp]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [ebp + 4]
            faddp st(1), st(0)
            fadd dword ptr [ecx + 0xc]
            fcom dword ptr [gUNK_0058a038]
            fstp dword ptr [edi]
            fnstsw ax
            test ah, 1
            jne L00461723
            inc edx
            add ecx, 0x10
            add edi, 4
            cmp edx, ebx
            jl L004615c6
            xor edx, edx
    L00461608:
            mov ecx, dword ptr [esp + 0x124]
            xor edi, edi
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [ecx], edx
            mov dword ptr [ecx + 4], edx
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [esi + 8]
            cmp eax, edx
            jle L00461672
            lea ebx, [esp + 0x18]
    L00461628:
            fld dword ptr [ebx]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L00461664
            mov eax, dword ptr [esp + 0x10]
            inc eax
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [esi + 0x14]
            fld dword ptr [edx + eax]
            fadd dword ptr [ecx]
            fstp dword ptr [ecx]
            mov eax, dword ptr [esi + 0x14]
            fld dword ptr [edx + eax + 4]
            fadd dword ptr [ecx + 4]
            fstp dword ptr [ecx + 4]
            mov eax, dword ptr [esi + 0x14]
            fld dword ptr [edx + eax + 8]
            fadd dword ptr [ecx + 8]
            fstp dword ptr [ecx + 8]
    L00461664:
            mov eax, dword ptr [esi + 8]
            inc edi
            add ebx, 4
            add edx, 0x10
            cmp edi, eax
            jl L00461628
    L00461672:
            cmp dword ptr [esp + 0x10], 1
            jle L004616ec
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gUNK_00589fc4]
            fnstsw ax
            test ah, 0x41
            jne L004616dc
            fdivr qword ptr [gDouble_00589fe8]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld st(2)
            fmul dword ptr [ecx]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [ecx]
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            fstp st(0)
            jmp L004616ec
    L004616dc:
            xor eax, eax
            mov dword ptr [ecx], 0x3f800000
            fstp st(0)
            mov dword ptr [ecx + 4], eax
            mov dword ptr [ecx + 8], eax
    L004616ec:
            mov eax, dword ptr [esp + 0x120]
            mov ecx, dword ptr [ebp]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [ebp + 4]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [esp + 0x128]
            mov dword ptr [eax + 8], ecx
            mov eax, 0x40
            mov dword ptr [edx], 0x3f800000
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 0x10
    L00461723:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x108
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
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
    if (face == NULL && pOffset != gUNK_00589fac) {
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
int C2_NAKED C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x34
            fld dword ptr [edx]
            fsub dword ptr [ecx]
            push ebx
            push ebp
            mov eax, ecx
            push esi
            lea ebp, [esp + 0x34]
            lea esi, [esp + 0x28]
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [edx + 4]
            fsub dword ptr [ecx + 4]
            push edi
            mov edi, 1
            mov ebx, 3
            fstp dword ptr [esp + 0x24]
            fld dword ptr [edx + 8]
            fsub dword ptr [ecx + 8]
            mov edx, dword ptr [esp + 0x48]
            sub eax, edx
            sub ebp, edx
            mov dword ptr [esp + 0x10], eax
            sub esi, edx
            fstp dword ptr [esp + 0x28]
            jmp L00460c1b
    L00460c17:
            mov eax, dword ptr [esp + 0x10]
    L00460c1b:
            fld dword ptr [eax + edx]
            fcomp dword ptr [edx]
            fnstsw ax
            test ah, 1
            je L00460c38
            mov eax, dword ptr [edx]
            mov dword ptr [esi + edx], 1
            mov dword ptr [ebp + edx], eax
            xor edi, edi
            jmp L00460c62
    L00460c38:
            mov eax, dword ptr [esp + 0x10]
            fld dword ptr [eax + edx]
            fcomp dword ptr [edx + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L00460c5b
            mov eax, dword ptr [edx + 0xc]
            mov dword ptr [esi + edx], 0
            mov dword ptr [ebp + edx], eax
            xor edi, edi
            jmp L00460c62
    L00460c5b:
            mov dword ptr [esi + edx], 2
    L00460c62:
            add edx, 4
            dec ebx
            jne L00460c17
            test edi, edi
            je L00460c8d
            mov eax, dword ptr [esp + 0x4c]
            mov edx, dword ptr [ecx]
            mov dword ptr [eax], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [eax + 8], ecx
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x34
            ret 0xc
    L00460c8d:
            xor edx, edx
            mov esi, 0xbf800000
    L00460c94:
            cmp dword ptr [esp + edx + 0x2c], 2
            je L00460cbd
            fld dword ptr [esp + edx + 0x20]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x40
            jne L00460cbd
            fld dword ptr [esp + edx + 0x38]
            fsub dword ptr [ecx + edx]
            fdiv dword ptr [esp + edx + 0x20]
            fstp dword ptr [esp + edx + 0x14]
            jmp L00460cc1
    L00460cbd:
            mov dword ptr [esp + edx + 0x14], esi
    L00460cc1:
            add edx, 4
            cmp edx, 0xc
            jl L00460c94
            xor ebx, ebx
            mov edx, 1
            xor edi, edi
    L00460cd2:
            lea esi, [edx*4]
            fld dword ptr [esp + esi + 0x14]
            fcomp dword ptr [esp + edi + 0x14]
            fnstsw ax
            test ah, 0x41
            jne L00460cec
            mov ebx, edx
            mov edi, esi
    L00460cec:
            inc edx
            cmp edx, 3
            jl L00460cd2
            fld dword ptr [esp + ebx*4 + 0x14]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L00460d8a
            fld dword ptr [esp + ebx*4 + 0x14]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L00460d8a
            mov edx, dword ptr [esp + 0x48]
            lea edi, [esp + 0x20]
            sub edi, ecx
            mov ecx, dword ptr [esp + 0x4c]
            xor esi, esi
            sub ecx, edx
    L00460d2a:
            cmp ebx, esi
            je L00460d5d
            mov eax, dword ptr [esp + 0x10]
            add eax, edx
            fld dword ptr [eax + edi]
            fmul dword ptr [esp + ebx*4 + 0x14]
            fadd dword ptr [eax]
            fld st(0)
            fxch st(1)
            fld st(1)
            fxch st(1)
            fstp dword ptr [ecx + edx]
            fcomp dword ptr [edx]
            fnstsw ax
            test ah, 1
            jne L00460d88
            fcomp dword ptr [edx + 0xc]
            fnstsw ax
            test ah, 0x41
            je L00460d8a
            jmp L00460d63
    L00460d5d:
            mov eax, dword ptr [edx + ebp]
            mov dword ptr [ecx + edx], eax
    L00460d63:
            inc esi
            add edx, 4
            cmp esi, 3
            jl L00460d2a
            mov edx, dword ptr [esp + 0x50]
            mov eax, dword ptr [esp + ebx*4 + 0x2c]
            mov ecx, dword ptr [esp + ebx*4 + 0x14]
            mov dword ptr [edx], ecx
            lea eax, [ebx + eax*4 + 1]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x34
            ret 0xc
    L00460d88:
            fstp st(0)
    L00460d8a:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x34
            ret 0xc
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0041e360
void C2_NAKED C2_HOOK_FASTCALL FUN_0041e360(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pNor, br_scalar* pT) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x18
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edx]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx]
            fmul dword ptr [gFloat_005896d4]
            mov ecx, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x1c]
            lea eax, [esp + 0x20]
            push 0
            fstp dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [gFloat_005896d4]
            fxch st(1)
            fmul dword ptr [gFloat_005896d4]
            fxch st(2)
            fmul dword ptr [gFloat_005896d4]
            fxch st(4)
            fmul dword ptr [gFloat_005896d4]
            fxch st(3)
            fmul dword ptr [gFloat_005896d4]
            fxch st(2)
            fstp dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [esp + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            push eax
            mov eax, dword ptr [gTrack_actor]
            fstp dword ptr [esp + 0x18]
            push ecx
            push edx
            fstp dword ptr [esp + 0x24]
            push eax
            lea edx, [esp + 0x20]
            lea ecx, [esp + 0x14]
            call ActorFindFace
            add esp, 0x18
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0041e3f0
void C2_NAKED C2_HOOK_FASTCALL FUN_0041e3f0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, void* pRay_context) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 scaled_ray_pos;
    br_vector3 scaled_ray_dir;

    scaled_ray_pos.v[0] = pRay_pos->v[0] * gFloat_005896d4;
    scaled_ray_dir.v[0] = pRay_dir->v[0] * gFloat_005896d4;
    scaled_ray_pos.v[1] = pRay_pos->v[1] * gFloat_005896d4;
    scaled_ray_pos.v[2] = pRay_pos->v[2] * gFloat_005896d4;
    scaled_ray_dir.v[1] = pRay_dir->v[1] * gFloat_005896d4;
    scaled_ray_dir.v[2] = pRay_dir->v[2] * gFloat_005896d4;
    RayCastInBox(&scaled_ray_pos, &scaled_ray_dir, pHit_pos, pT, *(tPhysics_object**)((tU8*)pRay_context + 8));
#else
    __asm {
            sub esp, 0x18
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            _emit 0xd9
        _emit 0x02
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fmul dword ptr [gFloat_005896d4]
            mov eax, dword ptr [esp + 0x24]
            mov ecx, dword ptr [esp + 0x1c]
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fmul dword ptr [gFloat_005896d4]
            fxch st(1)
            fmul dword ptr [gFloat_005896d4]
            fxch st(2)
            fmul dword ptr [gFloat_005896d4]
            fxch st(4)
            fmul dword ptr [gFloat_005896d4]
            fxch st(3)
            fmul dword ptr [gFloat_005896d4]
            fxch st(2)
            fstp dword ptr [esp + 4]
            mov edx, dword ptr [eax + 8]
            mov eax, dword ptr [esp + 0x20]
            fxch st(3)
            fstp dword ptr [esp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0xc]
            push edx
            push eax
            fstp dword ptr [esp + 0x18]
            push ecx
            lea edx, [esp + 0x18]
            fstp dword ptr [esp + 0x20]
            lea ecx, [esp + 0xc]
            call RayCastInBox
            add esp, 0x18
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004227a0
C2_NAKED tFace_ref* C2_HOOK_FASTCALL FUN_004227a0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pBest_t, br_vector3* pHit_pos, tFace_ref** pFaces, int pFace_count) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x20
            mov eax, dword ptr [esp + 0x34]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x30]
            push esi
            push edi
            test eax, eax
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [ebp], 0x40000000
            jle L00422846
            mov edi, dword ptr [esp + 0x3c]
            mov ebx, dword ptr [esp + 0x34]
            mov esi, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x44], eax
    L004227d6:
            lea eax, [esp + 0x24]
            lea ecx, [esp + 0x38]
            push eax
            mov eax, dword ptr [esp + 0x14]
            lea edx, [esp + 0x1c]
            push ecx
            mov ecx, dword ptr [esi]
            push edx
            mov edx, dword ptr [esp + 0x20]
            push eax
            call CheckSingleFace
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [ebp]
            fnstsw ax
            test ah, 1
            je L00422838
            mov ecx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [ebp], ecx
            mov edx, dword ptr [esi]
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x3c], edx
            mov edx, dword ptr [esp + 0x20]
            mov dword ptr [ebx], eax
            mov eax, dword ptr [esp + 0x24]
            mov dword ptr [ebx + 4], ecx
            mov ecx, dword ptr [esp + 0x28]
            mov dword ptr [ebx + 8], edx
            mov edx, dword ptr [esp + 0x2c]
            mov dword ptr [edi], eax
            mov dword ptr [edi + 4], ecx
            mov dword ptr [edi + 8], edx
    L00422838:
            mov eax, dword ptr [esp + 0x44]
            add esi, 4
            dec eax
            mov dword ptr [esp + 0x44], eax
            jne L004227d6
    L00422846:
            fld dword ptr [ebp]
            fcomp dword ptr [gFLOAT_00589968]
            fnstsw ax
            test ah, 1
            je L00422864
            mov eax, dword ptr [esp + 0x3c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret 0x14
    L00422864:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x20
            ret 0x14
            push esi
            mov esi, dword ptr [esp + 0x14]
            push edi
            xor eax, eax
            mov ax, word ptr [esi + 0x174]
            xor edi, edi
            mov di, word ptr [esi + 0x176]
            sub edi, eax
            lea eax, [eax + eax*4]
            shl eax, 4
            add eax, offset gFace_list__car
            push edi
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push eax
            call RayCastThruFaceList
            pop edi
            pop esi
            ret 0x10
    }
#endif
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
        PhysicsError(ePhysicsError_UnknownShapeType, "");
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
int C2_NAKED C2_HOOK_FASTCALL FUN_00427870(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, void* pRecords, int pRecord_count, undefined4 pArg11) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0xc8
            push ebx
            push ebp
            mov dword ptr [esp + 0x10], edx
            mov al, 0xff
            push esi
            push edi
            mov edx, ecx
            mov byte ptr [esp + 0x13], al
            mov byte ptr [esp + 0x12], al
            mov eax, dword ptr [esp + 0xf0]
            xor edi, edi
            xor ecx, ecx
            cmp eax, edi
            mov dword ptr [esp + 0x14], edx
            mov ebp, 1
            jle L00427a14
            mov eax, dword ptr [esp + 0xdc]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x14]
            fld dword ptr [eax + 0x18]
            fxch st(5)
            fsub dword ptr [gFLOAT_00589a10]
            fxch st(4)
            fsub dword ptr [gFLOAT_00589a10]
            fxch st(3)
            fsub dword ptr [gFLOAT_00589a10]
            fxch st(2)
            fsub dword ptr [gFLOAT_00589a14]
            fxch st(1)
            fsub dword ptr [gFLOAT_00589a14]
            fxch st(5)
            fsub dword ptr [gFLOAT_00589a14]
            fxch st(4)
            fxch st(5)
            fxch st(3)
            fxch st(4)
            fxch st(2)
            fxch st(3)
            fxch st(1)
            fxch st(2)
            mov eax, dword ptr [esp + 0xe8]
            lea esi, [eax + 4]
            jmp L0042790d
    L0042790b:
            xor edi, edi
    L0042790d:
            fld dword ptr [esi - 4]
            fcomp st(6)
            fnstsw ax
            test ah, 1
            je L00427929
            fld dword ptr [esi + 8]
            fcomp st(6)
            mov dword ptr [esp + 0x20], ebp
            fnstsw ax
            test ah, 1
            jne L0042792d
    L00427929:
            mov dword ptr [esp + 0x20], edi
    L0042792d:
            fld dword ptr [esi]
            fcomp st(5)
            fnstsw ax
            test ah, 1
            je L00427948
            fld dword ptr [esi + 0xc]
            fcomp st(5)
            mov dword ptr [esp + 0x24], ebp
            fnstsw ax
            test ah, 1
            jne L0042794c
    L00427948:
            mov dword ptr [esp + 0x24], edi
    L0042794c:
            fld dword ptr [esi + 4]
            fcomp st(4)
            fnstsw ax
            test ah, 1
            je L00427968
            fld dword ptr [esi + 0x10]
            fcomp st(4)
            mov dword ptr [esp + 0x1c], ebp
            fnstsw ax
            test ah, 1
            jne L0042796c
    L00427968:
            mov dword ptr [esp + 0x1c], edi
    L0042796c:
            fld dword ptr [esi - 4]
            fcomp st(3)
            fnstsw ax
            test ah, 0x41
            jne L00427988
            fld dword ptr [esi + 8]
            fcomp st(3)
            fnstsw ax
            test ah, 0x41
            jne L00427988
            mov ebx, ebp
            jmp L0042798a
    L00427988:
            xor ebx, ebx
    L0042798a:
            fld dword ptr [esi]
            fcomp st(2)
            fnstsw ax
            test ah, 0x41
            jne L004279a5
            fld dword ptr [esi + 0xc]
            fcomp st(2)
            fnstsw ax
            test ah, 0x41
            jne L004279a5
            mov edx, ebp
            jmp L004279a7
    L004279a5:
            xor edx, edx
    L004279a7:
            fcom dword ptr [esi + 4]
            fnstsw ax
            test ah, 1
            je L004279bf
            fcom dword ptr [esi + 0x10]
            fnstsw ax
            test ah, 1
            je L004279bf
            mov eax, ebp
            jmp L004279c1
    L004279bf:
            xor eax, eax
    L004279c1:
            shl al, 1
            add al, dl
            mov dl, byte ptr [esp + 0x20]
            shl al, 1
            add al, bl
            mov bl, byte ptr [esp + 0x1c]
            shl al, 1
            add al, bl
            mov bl, byte ptr [esp + 0x24]
            shl al, 1
            add al, bl
            mov bl, byte ptr [esp + 0x12]
            shl al, 1
            add al, dl
            add esi, 0x18
            mov byte ptr [esp + ecx + 0x58], al
            and bl, al
            mov eax, dword ptr [esp + 0xf0]
            inc ecx
            cmp ecx, eax
            mov byte ptr [esp + 0x12], bl
            jl L0042790b
            fstp st(0)
            fstp st(0)
            mov edx, dword ptr [esp + 0x14]
            xor edi, edi
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L00427a14:
            mov al, byte ptr [esp + 0x12]
            test al, al
            je L00427a30
            mov eax, dword ptr [esp + 0xf8]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x24
    L00427a30:
            mov eax, dword ptr [esp + 0xec]
            xor ecx, ecx
            cmp eax, edi
            jle L00427bb0
            mov eax, dword ptr [esp + 0xe0]
            mov edx, dword ptr [esp + 0xe4]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x14]
            fld dword ptr [eax + 0x18]
            fxch st(5)
            fsub dword ptr [gFLOAT_00589a10]
            fxch st(4)
            fsub dword ptr [gFLOAT_00589a10]
            fxch st(3)
            fsub dword ptr [gFLOAT_00589a10]
            fxch st(2)
            fsub dword ptr [gFLOAT_00589a14]
            fxch st(1)
            fsub dword ptr [gFLOAT_00589a14]
            fxch st(5)
            fsub dword ptr [gFLOAT_00589a14]
            fxch st(4)
            fxch st(5)
            fxch st(3)
            fxch st(4)
            fxch st(2)
            fxch st(3)
            fxch st(1)
            fxch st(2)
            lea esi, [edx + 4]
            jmp L00427aa8
    L00427aa6:
            xor edi, edi
    L00427aa8:
            fld dword ptr [esi - 4]
            fcomp st(6)
            fnstsw ax
            test ah, 1
            je L00427ac4
            fld dword ptr [esi + 8]
            fcomp st(6)
            mov dword ptr [esp + 0x1c], ebp
            fnstsw ax
            test ah, 1
            jne L00427ac8
    L00427ac4:
            mov dword ptr [esp + 0x1c], edi
    L00427ac8:
            fld dword ptr [esi]
            fcomp st(5)
            fnstsw ax
            test ah, 1
            je L00427ae3
            fld dword ptr [esi + 0xc]
            fcomp st(5)
            mov dword ptr [esp + 0x24], ebp
            fnstsw ax
            test ah, 1
            jne L00427ae7
    L00427ae3:
            mov dword ptr [esp + 0x24], edi
    L00427ae7:
            fld dword ptr [esi + 4]
            fcomp st(4)
            fnstsw ax
            test ah, 1
            je L00427b03
            fld dword ptr [esi + 0x10]
            fcomp st(4)
            mov dword ptr [esp + 0x20], ebp
            fnstsw ax
            test ah, 1
            jne L00427b07
    L00427b03:
            mov dword ptr [esp + 0x20], edi
    L00427b07:
            fld dword ptr [esi - 4]
            fcomp st(3)
            fnstsw ax
            test ah, 0x41
            jne L00427b23
            fld dword ptr [esi + 8]
            fcomp st(3)
            fnstsw ax
            test ah, 0x41
            jne L00427b23
            mov ebx, ebp
            jmp L00427b25
    L00427b23:
            xor ebx, ebx
    L00427b25:
            fld dword ptr [esi]
            fcomp st(2)
            fnstsw ax
            test ah, 0x41
            jne L00427b40
            fld dword ptr [esi + 0xc]
            fcomp st(2)
            fnstsw ax
            test ah, 0x41
            jne L00427b40
            mov edx, ebp
            jmp L00427b42
    L00427b40:
            xor edx, edx
    L00427b42:
            fcom dword ptr [esi + 4]
            fnstsw ax
            test ah, 1
            je L00427b5a
            fcom dword ptr [esi + 0x10]
            fnstsw ax
            test ah, 1
            je L00427b5a
            mov eax, ebp
            jmp L00427b5c
    L00427b5a:
            xor eax, eax
    L00427b5c:
            shl al, 1
            add al, dl
            mov dl, byte ptr [esp + 0x1c]
            shl al, 1
            add al, bl
            mov bl, byte ptr [esp + 0x20]
            shl al, 1
            add al, bl
            mov bl, byte ptr [esp + 0x24]
            shl al, 1
            add al, bl
            mov bl, byte ptr [esp + 0x13]
            shl al, 1
            add al, dl
            add esi, 0x18
            mov byte ptr [esp + ecx + 0x98], al
            and bl, al
            mov eax, dword ptr [esp + 0xec]
            inc ecx
            cmp ecx, eax
            mov byte ptr [esp + 0x13], bl
            jl L00427aa6
            fstp st(0)
            fstp st(0)
            mov edx, dword ptr [esp + 0x14]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L00427bb0:
            mov al, byte ptr [esp + 0x13]
            test al, al
            je L00427bcc
            mov eax, dword ptr [esp + 0xf8]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x24
    L00427bcc:
            mov esi, dword ptr [esp + 0xf8]
            mov ecx, dword ptr [edx + 0x19c]
            mov ebx, dword ptr [esp + 0xfc]
            mov edi, dword ptr [esp + 0xf4]
            mov eax, 0x100
            mov ebp, esi
            test ecx, eax
            mov dword ptr [esp + 0x20], ebp
            jne L00427c06
            mov ecx, dword ptr [esp + 0x18]
            test dword ptr [ecx + 0x19c], eax
            je L00427eb7
    L00427c06:
            fld dword ptr [edx + 0x30]
            fld dword ptr [edx + 0x34]
            fld dword ptr [edx + 0x38]
            fxch st(2)
            fadd dword ptr [edx + 0x24]
            fxch st(1)
            fadd dword ptr [edx + 0x28]
            fxch st(2)
            fadd dword ptr [edx + 0x2c]
            fld st(1)
            fmul dword ptr [gFLOAT_00589a1c]
            fxch st(3)
            fst dword ptr [esp + 0x38]
            fmul dword ptr [gFLOAT_00589a1c]
            fxch st(1)
            fst dword ptr [esp + 0x3c]
            fmul dword ptr [gFLOAT_00589a1c]
            fxch st(3)
            fstp dword ptr [esp + 0x34]
            lea eax, [edx + 0x8c]
            lea edx, [esp + 0x34]
            fstp dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x24], eax
            push eax
            lea eax, [esp + 0x44]
            push edx
            push eax
            mov dword ptr [esp + 0x28], 0
            fstp st(0)
            call BrMatrix34ApplyP
            fld dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x24]
            add esp, 0xc
            fld dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fsub dword ptr [ecx + 0xb0]
            fxch st(1)
            fsub dword ptr [ecx + 0xb4]
            fxch st(2)
            fsub dword ptr [ecx + 0xb8]
            fxch st(1)
            fstp dword ptr [esp + 0x4c]
            lea ebp, [ecx + 0x8c]
            lea edx, [esp + 0x4c]
            fxch st(1)
            fstp dword ptr [esp + 0x50]
            push ebp
            lea eax, [esp + 0x38]
            fstp dword ptr [esp + 0x58]
            push edx
            push eax
            call BrMatrix34TApplyV
            mov edx, dword ptr [esp + 0x24]
            fld dword ptr [edx + 0x34]
            fld dword ptr [edx + 0x30]
            fadd dword ptr [edx + 0x24]
            fxch st(1)
            fadd dword ptr [edx + 0x28]
            fld dword ptr [edx + 0x38]
            fadd dword ptr [edx + 0x2c]
            fxch st(2)
            fmul dword ptr [gFLOAT_00589a1c]
            fxch st(1)
            fmul dword ptr [gFLOAT_00589a1c]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x44]
            fxch st(4)
            fmul dword ptr [gFLOAT_00589a1c]
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fsubrp st(4), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x38]
            fxch st(3)
            fsub dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fxch st(2)
            fsub dword ptr [esp + 0x3c]
            add esp, 0xc
            fld st(1)
            fmul st(0), st(2)
            fld st(3)
            fmul st(0), st(4)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFLOAT_005899a8]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fnstsw ax
            test ah, 0x41
            jne L00427d73
            fdivr qword ptr [gDouble_005899b0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x30]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L00427d93
    L00427d73:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x28], 0x3f800000
            mov dword ptr [esp + 0x2c], 0
            mov dword ptr [esp + 0x30], 0
    L00427d93:
            lea ecx, [esp + 0x28]
            lea eax, [esp + 0x1c]
            push ecx
            lea ecx, [esp + 0x9c]
            push eax
            mov eax, dword ptr [esp + 0xec]
            push ecx
            mov ecx, dword ptr [esp + 0xec]
            push ebx
            push esi
            push edi
            push eax
            mov eax, dword ptr [esp + 0xf8]
            push ecx
            mov ecx, dword ptr [esp + 0x34]
            push eax
            call FUN_00427fd0
            mov esi, eax
            lea edx, [esp + 0x28]
            mov ecx, dword ptr [esp + 0x24]
            lea eax, [esp + 0x40]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyV
            add esp, 0xc
            lea ecx, [esp + 0x40]
            lea edx, [esp + 0x28]
            push ebp
            push ecx
            push edx
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x34]
            fchs
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            add esp, 0xc
            lea eax, [esp + 0x28]
            lea ecx, [esp + 0x1c]
            lea edx, [esp + 0x58]
            push eax
            mov eax, dword ptr [esp + 0xec]
            push ecx
            mov ecx, dword ptr [esp + 0xe4]
            fchs
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            push edx
            mov edx, dword ptr [esp + 0xec]
            push ebx
            push esi
            push edi
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x38]
            fchs
            fstp dword ptr [esp + 0x50]
            push edx
            mov edx, dword ptr [esp + 0x38]
            call FUN_00427fd0
            mov esi, eax
            mov eax, dword ptr [esp + 0x1c]
            test eax, eax
            je L00427eaf
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x18]
            mov edx, 0x200
            mov ebx, dword ptr [eax + 0x19c]
            or ebx, edx
            mov dword ptr [eax + 0x19c], ebx
            mov edi, dword ptr [ecx + 0x19c]
            or edi, edx
            mov dword ptr [ecx + 0x19c], edi
            mov eax, dword ptr [eax + 0x228]
            test eax, eax
            je L00427ea0
            cmp eax, dword ptr [ecx + 0x228]
            jne L00427ea0
            mov eax, dword ptr [esp + 0x20]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x24
    L00427ea0:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x24
    L00427eaf:
            mov ebp, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x14]
    L00427eb7:
            mov eax, dword ptr [esp + 0xe0]
            cmp dword ptr [eax], 2
            je L00427f01
            mov edx, dword ptr [esp + 0xec]
            lea ecx, [esp + 0x98]
            push ecx
            mov ecx, dword ptr [esp + 0xe8]
            push ebx
            push esi
            push edi
            push edx
            mov edx, dword ptr [esp + 0xf0]
            push ecx
            mov ecx, dword ptr [esp + 0x2c]
            push eax
            push edx
            mov edx, dword ptr [esp + 0x38]
            call FUN_00428530
            mov esi, eax
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0xe0]
    L00427f01:
            mov ecx, dword ptr [esp + 0xdc]
            cmp dword ptr [ecx], 2
            je L00427f4b
            lea ecx, [esp + 0x58]
            push ecx
            mov ecx, dword ptr [esp + 0xf4]
            push ebx
            push esi
            push edi
            push ecx
            mov ecx, dword ptr [esp + 0xfc]
            push ecx
            mov ecx, dword ptr [esp + 0xf4]
            push ecx
            mov ecx, dword ptr [esp + 0x34]
            push eax
            call FUN_00428530
            mov esi, eax
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0xe0]
            mov ecx, dword ptr [esp + 0xdc]
    L00427f4b:
            test esi, esi
            je L00427f94
            lea ebp, [ebp + ebp*2]
            shl ebp, 5
            cmp edx, dword ptr [ebp + edi + 4]
            je L00427f94
            lea ebp, [esp + 0x98]
            push ebp
            lea ebp, [esp + 0x5c]
            push ebp
            push ebx
            push esi
            mov esi, dword ptr [esp + 0xf4]
            push edi
            push esi
            mov esi, dword ptr [esp + 0x100]
            push esi
            push ecx
            mov ecx, dword ptr [esp + 0x38]
            push eax
            call FUN_004287a0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x24
    L00427f94:
            lea edx, [esp + 0x58]
            push edx
            lea edx, [esp + 0x9c]
            push edx
            mov edx, dword ptr [esp + 0xf0]
            push ebx
            push esi
            push edi
            push edx
            mov edx, dword ptr [esp + 0xfc]
            push edx
            mov edx, dword ptr [esp + 0x34]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x38]
            call FUN_004287a0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x24
    }
#endif
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
        PhysicsError(1, "");
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
int C2_NAKED C2_HOOK_FASTCALL FUN_00428530(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pCorner_shape, tPhysics_shape* pShape, float pT, int pEdge_count, float* pRecords, int pRecord_count, undefined4 pArg9, tU8* pEdge_visibility) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            sub esp, 0x44
            mov eax, dword ptr [eax]
            push ebp
            push esi
            sub eax, 2
            mov esi, edx
            mov ebp, ecx
            je L00428791
            mov eax, dword ptr [esp + 0x5c]
            push edi
            xor edi, edi
            test eax, eax
            jle L00428784
            mov ecx, dword ptr [esp + 0x5c]
            push ebx
            mov dword ptr [esp + 0x14], ecx
    L00428560:
            mov edx, dword ptr [esp + 0x74]
            cmp byte ptr [edi + edx], 0
            jne L0042876b
            lea eax, [esp + 0x60]
            lea edx, [esp + 0x30]
            push eax
            push edx
            mov edx, dword ptr [esp + 0x64]
            lea eax, [esp + 0x44]
            push eax
            push edx
            lea edx, [ecx + 0xc]
            call ShapeRayCastWithSphere
            mov ebx, eax
            test ebx, ebx
            je L0042876b
            cmp ebx, -1
            jne L004285c7
            mov eax, dword ptr [esp + 0x74]
            mov byte ptr [edi + eax], 0xff
            mov edx, dword ptr [ebp + 0x19c]
            mov eax, 0x200
            or edx, eax
            mov dword ptr [ebp + 0x19c], edx
            mov ecx, dword ptr [esi + 0x19c]
            or ecx, eax
            mov dword ptr [esi + 0x19c], ecx
            jmp L0042876b
    L004285c7:
            mov edx, dword ptr [esp + 0x58]
            push edi
            lea ecx, [esp + 0x4c]
            call GetShapePoint
            test bl, 0x40
            je L0042861d
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            add ecx, 0x2c
            lea eax, [esp + 0x18]
            push ecx
            push edx
            push eax
            mov dword ptr [esp + 0x6c], 0x3f866666
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x24]
            fchs
            fstp dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fchs
            fstp dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fchs
            fstp dword ptr [esp + 0x2c]
            add esp, 0xc
            jmp L0042873b
    L0042861d:
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            add ecx, 0x2c
            lea eax, [esp + 0x18]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyV
            add esp, 0xc
            lea ecx, [esi + 0x8c]
            lea edx, [esp + 0x30]
            lea eax, [esp + 0x24]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyV
            fld dword ptr [gFLOAT_0058996c]
            fsub dword ptr [esp + 0x6c]
            fld dword ptr [esp + 0x6c]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fxch st(5)
            fchs
            fstp dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            fchs
            fxch st(2)
            fmul st(0), st(2)
            fld dword ptr [esp + 0x2c]
            fxch st(5)
            fmul st(0), st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(4)
            fstp dword ptr [esp + 0x34]
            fxch st(4)
            fmul st(0), st(2)
            fxch st(4)
            fadd dword ptr [esp + 0x30]
            fxch st(1)
            fadd dword ptr [esp + 0x34]
            fxch st(3)
            fstp dword ptr [esp + 0x38]
            fxch st(3)
            fadd dword ptr [esp + 0x38]
            fxch st(3)
            add esp, 0xc
            fstp st(1)
            fld st(0)
            fmul st(0), st(1)
            fld st(2)
            fmul st(0), st(3)
            fld st(4)
            fmul st(0), st(5)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFLOAT_005899a8]
            fxch st(1)
            fxch st(3)
            fxch st(1)
            fnstsw ax
            test ah, 0x41
            jne L0042871b
            fdivr qword ptr [gDouble_005899b0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x20]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L0042873b
    L0042871b:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x18], 0x3f800000
            mov dword ptr [esp + 0x1c], 0
            mov dword ptr [esp + 0x20], 0
    L0042873b:
            mov ecx, dword ptr [esp + 0x6c]
            push 0
            push 0
            push 5
            push esi
            push ebp
            lea edx, [esp + 0x2c]
            push ecx
            mov ecx, dword ptr [esp + 0x78]
            lea eax, [esp + 0x54]
            push edx
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x8c]
            lea edx, [esp + 0x6c]
            call AddObjectObjectForce
            mov dword ptr [esp + 0x6c], eax
    L0042876b:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x64]
            inc edi
            add ecx, 0x18
            cmp edi, eax
            mov dword ptr [esp + 0x14], ecx
            jl L00428560
            pop ebx
    L00428784:
            mov eax, dword ptr [esp + 0x68]
            pop edi
            pop esi
            pop ebp
            add esp, 0x44
            ret 0x20
    L00428791:
            mov eax, dword ptr [esp + 0x64]
            pop esi
            pop ebp
            add esp, 0x44
            ret 0x20
            nop
            nop
            nop
    }
#endif
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
        PhysicsError(ePhysicsError_UnknownShapeType, "");
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
        PhysicsError(ePhysicsError_UnknownShapeType, "");
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
                    PhysicsError(ePhysicsError_UnknownShapeType, "");
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
void C2_NAKED C2_HOOK_FASTCALL FUN_0045d5e0(tCollision_shape_sphere_data* pSphere1, tCollision_shape_sphere_data* pSphere2, br_vector3* pStart, br_vector3* pEnd, br_vector3* pNormal, float* pFactor) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x1c
            push esi
            mov esi, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x24]
            push edi
            mov edi, edx
            mov dword ptr [esi], 0x42c80000
            mov edx, dword ptr [esp + 0x2c]
            add ecx, 4
            fld dword ptr [edx + 4]
            fld dword ptr [edx]
            fsub dword ptr [eax]
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [edx + 8]
            fsub dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [eax]
            fld dword ptr [eax + 4]
            fxch st(2)
            faddp st(3), st(0)
            fld dword ptr [eax + 8]
            fld dword ptr [ecx - 4]
            fxch st(4)
            fst dword ptr [esp + 0x28]
            fcomp qword ptr [gDouble_00589fd8]
            fxch st(2)
            fsub dword ptr [ecx + 4]
            fxch st(2)
            fsub dword ptr [ecx + 8]
            fxch st(3)
            fadd dword ptr [edi]
            fxch st(1)
            fsub dword ptr [ecx]
            fnstsw ax
            test ah, 1
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x34]
            jne L0045d71e
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x1c]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fdiv dword ptr [esp + 0x28]
            fchs
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fld dword ptr [esp + 0x34]
            fxch st(3)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            fsub qword ptr [gDouble_00589fe0]
            fxch st(2)
            faddp st(4), st(0)
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            faddp st(3), st(0)
            fld st(0)
            fmul st(0), st(1)
            fxch st(2)
            fsubp st(3), st(0)
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L0045d72e
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x2c]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0045d736
            fstp st(0)
    L0045d71e:
            mov eax, dword ptr [esp + 0x30]
            push esi
            push eax
            mov eax, dword ptr [esp + 0x3c]
            push eax
            call PointSphereCollision
    L0045d72e:
            pop edi
            pop esi
            add esp, 0x1c
            ret 0x10
    L0045d736:
            fsub dword ptr [esp + 0x2c]
            fdiv dword ptr [esp + 0x28]
            fsqrt
            fsubr dword ptr [esp + 8]
            fst dword ptr [esp + 0x28]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045d71e
            fld dword ptr [esp + 0x28]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L0045d71e
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fchs
            fxch st(1)
            fmul st(0), st(1)
            fld dword ptr [esp + 0xc]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            fld st(1)
            fld st(3)
            fxch st(2)
            fsub dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x34]
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x34]
            fxch st(2)
            faddp st(3), st(0)
            mov ecx, dword ptr [esp + 0x30]
            fld st(3)
            fld st(5)
            fxch st(4)
            fsqrt
            fst dword ptr [esp + 0x30]
            fst dword ptr [esp + 0x30]
            fcomp dword ptr [gUNK_00589fc4]
            fxch st(1)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [ecx]
            fnstsw ax
            fstp dword ptr [ecx + 4]
            test ah, 0x41
            fstp dword ptr [ecx + 8]
            fxch st(1)
            jne L0045d83b
            fld dword ptr [esp + 0x30]
            fdivr qword ptr [gDouble_00589fe8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [ecx]
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            fstp st(0)
            fstp st(0)
            pop edi
            pop esi
            add esp, 0x1c
            ret 0x10
    L0045d83b:
            fstp st(0)
            xor eax, eax
            pop edi
            fstp st(0)
            mov dword ptr [ecx], 0x3f800000
            mov dword ptr [ecx + 4], eax
            mov dword ptr [ecx + 8], eax
            pop esi
            add esp, 0x1c
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
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
            scale = (br_scalar)(gDouble_00589fe8 / sqrt(distance_squared));
            pNormal->v[0] = scale * dx;
            pNormal->v[1] = scale * dy;
            pNormal->v[2] = scale * dz;
            *pCollision_t = 1.0f;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045d920
void C2_NAKED C2_HOOK_FASTCALL FUN_0045d920(br_scalar* pStart, br_scalar* pEnd, br_scalar* pEdge_start, br_scalar* pEdge_end, br_scalar* pSphere, br_scalar* pCollision_pos, br_scalar* pNormal, br_scalar* pT, tU32 pStart_flags, tU32 pEnd_flags) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x4c
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x6c]
            mov ebx, edx
            push esi
            push edi
            mov edi, dword ptr [esp + 0x64]
            mov dword ptr [ebp], 0x42c80000
            fld dword ptr [ebx]
            fld dword ptr [ebx + 4]
            fld dword ptr [edi]
            fld dword ptr [edi + 4]
            fxch st(3)
            mov edx, dword ptr [esp + 0x60]
            mov esi, ecx
            fsub dword ptr [esi]
            fxch st(2)
            fsub dword ptr [esi + 4]
            fxch st(1)
            fsub dword ptr [edx]
            fld dword ptr [ebx + 8]
            fxch st(4)
            fsub dword ptr [edx + 4]
            fld dword ptr [edi + 8]
            fxch st(4)
            fst dword ptr [esp + 0x14]
            fxch st(3)
            fst dword ptr [esp + 0x18]
            fxch st(5)
            fsub dword ptr [esi + 8]
            fst dword ptr [esp + 0x1c]
            fxch st(2)
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fst dword ptr [esp + 0x48]
            fxch st(4)
            fsub dword ptr [edx + 8]
            fst dword ptr [esp + 0x4c]
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(6)
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            faddp st(2), st(0)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(4)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fdiv st(0), st(3)
            fstp dword ptr [esp + 0x64]
            fxch st(5)
            fmul dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x4c]
            fxch st(1)
            fstp dword ptr [esp + 0x50]
            fxch st(5)
            fsub dword ptr [esp + 0x50]
            fxch st(4)
            fmul dword ptr [esp + 0x64]
            fld dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esp + 0x54]
            fxch st(1)
            fsub dword ptr [esp + 0x54]
            fxch st(3)
            fmul dword ptr [esp + 0x64]
            fxch st(4)
            fstp dword ptr [esp + 0x20]
            fsub dword ptr [edx]
            fxch st(3)
            fstp dword ptr [esp + 0x58]
            fxch st(3)
            fsub dword ptr [esp + 0x58]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fld dword ptr [esi + 4]
            fsub dword ptr [edx + 4]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fld dword ptr [esi + 8]
            fsub dword ptr [edx + 8]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x40]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x74]
            fld dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fdiv dword ptr [esp + 0x74]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fxch st(2)
            fxch st(1)
            faddp st(1), st(0)
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(4)
            fchs
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fcomp qword ptr [gDouble_00589fd8]
            fxch st(2)
            fstp dword ptr [esp + 0x64]
            fmul dword ptr [esp + 0x64]
            fxch st(1)
            fmul dword ptr [esp + 0x64]
            fxch st(2)
            fmul dword ptr [esp + 0x64]
            fxch st(1)
            fadd dword ptr [esp + 0x38]
            fxch st(2)
            fadd dword ptr [esp + 0x3c]
            fxch st(1)
            fadd dword ptr [esp + 0x40]
            fxch st(2)
            fstp dword ptr [esp + 0x2c]
            fnstsw ax
            fstp dword ptr [esp + 0x30]
            test ah, 1
            fstp dword ptr [esp + 0x34]
            je L0045db1f
            mov eax, dword ptr [esp + 0x70]
            mov ecx, dword ptr [esp + 0x6c]
            mov edx, dword ptr [esp + 0x68]
            push ebp
            push eax
            mov eax, dword ptr [esp + 0x7c]
            push ecx
            push edx
            push edi
            lea ecx, [esp + 0x28]
            push eax
            push ecx
            mov edx, ebx
            mov ecx, esi
            call StationarySphereEdgeCollide
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4c
            ret 0x20
    L0045db1f:
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            faddp st(3), st(0)
            fld dword ptr [esp + 0x2c]
            fxch st(3)
            fdiv dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x68]
            fstp dword ptr [esp + 0x64]
            fxch st(3)
            fmul dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x30]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(4)
            fmul dword ptr [esp + 0x64]
            fst dword ptr [esp + 0x24]
            fxch st(2)
            fmul dword ptr [esp + 0x64]
            fld st(0)
            fxch st(4)
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x34]
            fxch st(3)
            fmul dword ptr [esp + 0x30]
            faddp st(1), st(0)
            fxch st(5)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x24]
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fld dword ptr [ecx]
            fxch st(1)
            faddp st(5), st(0)
            fxch st(3)
            fmul st(0), st(1)
            faddp st(2), st(0)
            fxch st(2)
            fmul dword ptr [gFLOAT_00589ff4]
            fmul dword ptr [gFLOAT_00589ff8]
            fxch st(1)
            fsubp st(3), st(0)
            fsubr dword ptr [ecx + 0x10]
            fxch st(2)
            fst dword ptr [esp + 0x60]
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fcompp
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L0045de8d
            fld dword ptr [esp + 0x60]
            fcomp dword ptr [ecx + 0x10]
            fnstsw ax
            test ah, 0x41
            je L0045de6a
            fld dword ptr [ecx + 0x10]
            fsub dword ptr [esp + 0x60]
            fld dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x54]
            fxch st(2)
            fdiv dword ptr [esp + 0x10]
            fsqrt
            fld st(0)
            fsubr dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x58]
            fxch st(1)
            fstp dword ptr [esp + 0x64]
            fxch st(2)
            fmul dword ptr [esp + 0x64]
            fxch st(3)
            fmul dword ptr [esp + 0x64]
            fxch st(2)
            fmul dword ptr [esp + 0x64]
            fxch st(3)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [esp + 0x30]
            fxch st(3)
            fadd dword ptr [esp + 0x34]
            fxch st(2)
            fsub dword ptr [esp + 0x38]
            fxch st(3)
            fsub dword ptr [esp + 0x3c]
            fxch st(2)
            fsub dword ptr [esp + 0x40]
            fxch st(3)
            fst dword ptr [esp + 0x50]
            fxch st(2)
            fst dword ptr [esp + 0x54]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esp + 0x58]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x58]
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(3), st(0)
            faddp st(2), st(0)
            fxch st(1)
            fdiv dword ptr [esp + 0x74]
            fstp st(1)
            fcom dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045dcdf
            test byte ptr [esp + 0x78], 0x80
            fstp st(0)
            jne L0045dd10
            mov eax, dword ptr [esp + 0x70]
            push ebp
            push eax
            mov eax, dword ptr [esp + 0x74]
            push eax
            push ecx
            lea ecx, [esp + 0x54]
            push ecx
            push edi
            mov ecx, esi
            call FUN_0045e020
            fld dword ptr [ebp]
            fcomp dword ptr [gFloat_00589fd0]
            mov ecx, dword ptr [esp + 0x68]
            fnstsw ax
            test ah, 1
            jne L0045de8d
            mov edx, dword ptr [esp + 0x70]
            mov eax, dword ptr [esp + 0x6c]
            push ebp
            push edx
            push eax
            jmp L0045de75
    L0045dcdf:
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            jne L0045dd34
            test byte ptr [esp + 0x7c], 0x80
            jne L0045dd10
            mov eax, dword ptr [esp + 0x70]
            push ebp
            push eax
            mov eax, dword ptr [esp + 0x74]
            push eax
            push ecx
            lea ecx, [esp + 0x54]
            push ecx
            push edi
            mov ecx, ebx
            call FUN_0045e020
            mov ecx, dword ptr [esp + 0x68]
    L0045dd10:
            fld dword ptr [ebp]
            fcomp dword ptr [gFloat_00589fd0]
            fnstsw ax
            test ah, 1
            jne L0045de8d
            mov edx, dword ptr [esp + 0x70]
            mov eax, dword ptr [esp + 0x6c]
            push ebp
            push edx
            push eax
            jmp L0045de75
    L0045dd34:
            fld dword ptr [esp + 0x64]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            jne L0045de6a
            fld dword ptr [esp + 0x64]
            fcomp dword ptr [gUNK_00589fb0]
            fnstsw ax
            test ah, 0x41
            je L0045de6a
            fld dword ptr [esi]
            fadd dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x58]
            fxch st(2)
            mov eax, dword ptr [esp + 0x6c]
            fstp dword ptr [eax]
            fadd dword ptr [esi + 4]
            fstp dword ptr [eax + 4]
            fadd dword ptr [esi + 8]
            fst dword ptr [esp + 0x78]
            fstp dword ptr [eax + 8]
            fld dword ptr [edi]
            fld dword ptr [edi + 4]
            fsub dword ptr [edx + 4]
            fxch st(1)
            fsub dword ptr [edx]
            fld dword ptr [edi + 8]
            fsub dword ptr [edx + 8]
            fxch st(2)
            fmul dword ptr [esp + 0x64]
            fxch st(1)
            fmul dword ptr [esp + 0x64]
            fxch st(2)
            fmul dword ptr [esp + 0x64]
            fld dword ptr [edx]
            fld st(2)
            fadd dword ptr [edx + 4]
            fxch st(4)
            fstp dword ptr [esp + 0x20]
            fadd dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fadd dword ptr [edx + 8]
            fxch st(1)
            fsubr dword ptr [eax]
            fxch st(3)
            fsubr dword ptr [eax + 4]
            fxch st(1)
            fsubr dword ptr [esp + 0x78]
            fxch st(3)
            fstp st(2)
            fld st(1)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            fld st(4)
            fmul st(0), st(5)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gUNK_00589fc4]
            fxch st(2)
            fxch st(3)
            fxch st(1)
            fxch st(2)
            fnstsw ax
            test ah, 0x41
            jne L0045de3f
            fdivr qword ptr [gDouble_00589fe8]
            mov eax, dword ptr [esp + 0x70]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [eax]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fstp dword ptr [eax + 8]
            mov eax, dword ptr [esp + 0x64]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [ebp], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4c
            ret 0x20
    L0045de3f:
            mov eax, dword ptr [esp + 0x70]
            xor ecx, ecx
            fstp st(0)
            fstp st(0)
            mov dword ptr [eax], 0x3f800000
            mov dword ptr [eax + 4], ecx
            mov dword ptr [eax + 8], ecx
            mov eax, dword ptr [esp + 0x64]
            fstp st(0)
            fstp st(0)
            mov dword ptr [ebp], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4c
            ret 0x20
    L0045de6a:
            mov edx, dword ptr [esp + 0x70]
            mov eax, dword ptr [esp + 0x6c]
            push ebp
            push edx
            push eax
    L0045de75:
            push ecx
            mov ecx, dword ptr [esp + 0x84]
            push edi
            lea edx, [esp + 0x28]
            push ecx
            push edx
            mov edx, ebx
            mov ecx, esi
            call StationarySphereEdgeCollide
    L0045de8d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4c
            ret 0x20
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045e200
void C2_NAKED C2_HOOK_FASTCALL FUN_0045e200(br_scalar* pClearance, tCollision_shape_polyhedron_data* pShape, const br_vector3* pStart, const br_vector3* pEnd, br_vector3* pHit_position, br_vector3* pNormal, br_scalar* pFactor) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x400
            mov eax, edx
            push ebx
            mov edx, ecx
            push ebp
            mov ecx, dword ptr [esp + 0x41c]
            push esi
            push edi
            xor ebx, ebx
            mov dword ptr [ecx], 0x42c80000
            mov edi, dword ptr [eax + 8]
            mov ecx, dword ptr [esp + 0x418]
            mov dword ptr [esp + 0x14], eax
            test edi, edi
            jle L0045e2c3
            mov esi, dword ptr [eax + 0x14]
    L0045e236:
            mov ebp, dword ptr [esp + 0x414]
            fld dword ptr [esi]
            fld dword ptr [ebp + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [ebp + 8]
            fxch st(4)
            fmul dword ptr [ebp]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fld dword ptr [esi]
            fmul dword ptr [ecx]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [esi + 8]
            fxch st(1)
            fadd dword ptr [edx]
            fld dword ptr [ecx + 8]
            fmulp st(2), st(0)
            fadd dword ptr [esi + 0xc]
            fld dword ptr [esi + 4]
            fxch st(2)
            faddp st(3), st(0)
            fcom qword ptr [gDouble_00589fe0]
            fld dword ptr [ecx + 4]
            fmulp st(2), st(0)
            fxch st(1)
            fnstsw ax
            faddp st(2), st(0)
            test ah, 1
            fstp dword ptr [esp + ebx*4 + 0x220]
            fadd dword ptr [edx]
            fadd dword ptr [esi + 0xc]
            fst dword ptr [esp + ebx*4 + 0x30]
            je L0045e2b3
            fcomp qword ptr [gDouble_00589fe0]
            fnstsw ax
            test ah, 1
            jne L0045e4c4
            jmp L0045e2b5
    L0045e2b3:
            fstp st(0)
    L0045e2b5:
            inc ebx
            add esi, 0x10
            cmp ebx, edi
            jl L0045e236
            jmp L0045e2ca
    L0045e2c3:
            mov ebp, dword ptr [esp + 0x414]
    L0045e2ca:
            xor eax, eax
            cmp edi, eax
            mov dword ptr [esp + 0x10], eax
            jle L0045e4c4
            mov esi, dword ptr [esp + 0x41c]
            mov dword ptr [esp + 0x1c], eax
    L0045e2e3:
            mov eax, dword ptr [esp + 0x10]
            lea ebx, [eax*4]
            fld dword ptr [esp + ebx + 0x30]
            fcomp qword ptr [gDouble_00589fe0]
            lea eax, [esp + ebx + 0x30]
            mov dword ptr [esp + 0x18], eax
            fnstsw ax
            test ah, 0x41
            jne L0045e464
            fld dword ptr [esp + ebx + 0x220]
            fcomp dword ptr [gUNK_00589fac]
            lea ebx, [esp + ebx + 0x220]
            fnstsw ax
            test ah, 1
            je L0045e464
            mov edi, dword ptr [esp + 0x18]
            fld dword ptr [edi]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 1
            je L0045e355
            mov eax, dword ptr [ecx]
            fld dword ptr [gUNK_00589fb0]
            mov dword ptr [esi], eax
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [esi + 4], eax
            mov eax, dword ptr [ecx + 8]
            mov dword ptr [esi + 8], eax
            jmp L0045e3b8
    L0045e355:
            fld dword ptr [ebx]
            fld dword ptr [edi]
            fsub dword ptr [ebx]
            fxch st(1)
            fchs
            fxch st(1)
            fdivp st(1), st(0)
            fld dword ptr [gUNK_00589fb0]
            fsub st(0), st(1)
            fld dword ptr [ebp]
            fmul st(0), st(1)
            fstp dword ptr [esi]
            fld dword ptr [ebp + 4]
            fmul st(0), st(1)
            fstp dword ptr [esi + 4]
            fld dword ptr [ebp + 8]
            fmul st(0), st(1)
            fstp dword ptr [esi + 8]
            fstp st(0)
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fxch st(1)
            fadd dword ptr [esi + 4]
            fxch st(1)
            fadd dword ptr [esi + 8]
            fxch st(2)
            fadd dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 4]
            fxch st(1)
            fstp dword ptr [esi + 8]
            fstp dword ptr [esi]
    L0045e3b8:
            mov eax, dword ptr [esp + 0x14]
            mov edi, dword ptr [esp + 0x1c]
            xor ebx, ebx
            mov eax, dword ptr [eax + 0x14]
            add eax, edi
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            fmul dword ptr [edx]
            fxch st(1)
            fmul dword ptr [edx]
            fld dword ptr [eax]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [esi + 4]
            fxch st(2)
            fadd dword ptr [esi + 8]
            fxch st(1)
            fmul dword ptr [edx]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fxch st(2)
            fadd dword ptr [esi]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x14]
            fxch st(2)
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi]
            fstp dword ptr [esi + 8]
            mov edi, dword ptr [eax + 8]
            test edi, edi
            jle L0045e45e
            xor ebp, ebp
    L0045e410:
            cmp ebx, dword ptr [esp + 0x10]
            je L0045e44f
            mov eax, dword ptr [esp + 0x14]
            mov eax, dword ptr [eax + 0x14]
            add eax, ebp
            fld dword ptr [eax]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(2)
            fmul dword ptr [esi]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0xc]
            fcomp dword ptr [gUNK_00589fac]
            fnstsw ax
            test ah, 0x41
            jne L0045e457
    L0045e44f:
            inc ebx
            add ebp, 0x10
            cmp ebx, edi
            jl L0045e410
    L0045e457:
            mov ebp, dword ptr [esp + 0x414]
    L0045e45e:
            cmp ebx, edi
            je L0045e48d
            fstp st(0)
    L0045e464:
            mov eax, dword ptr [esp + 0x10]
            mov ebx, dword ptr [esp + 0x1c]
            inc eax
            add ebx, 0x10
            cmp eax, edi
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x1c], ebx
            jl L0045e2e3
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x400
            ret 0x14
    L0045e48d:
            mov ecx, dword ptr [esp + 0x424]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x10]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [edx + 0x14]
            shl eax, 4
            mov esi, dword ptr [eax + ecx]
            mov ecx, dword ptr [esp + 0x420]
            mov dword ptr [ecx], esi
            mov esi, dword ptr [edx + 0x14]
            mov esi, dword ptr [eax + esi + 4]
            mov dword ptr [ecx + 4], esi
            mov edx, dword ptr [edx + 0x14]
            mov eax, dword ptr [eax + edx + 8]
            mov dword ptr [ecx + 8], eax
    L0045e4c4:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x400
            ret 0x14
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045e4e0
void C2_NAKED C2_HOOK_FASTCALL FUN_0045e4e0(float* param_1, float* param_2, float* param_3, float* param_4, float* param_5, float* param_6, float* param_7) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x1c
            mov eax, dword ptr [esp + 0x30]
            push ebx
            push ebp
            push esi
            mov dword ptr [esp + 0x18], edx
            lea esi, [edx + 4]
            mov edx, dword ptr [esp + 0x2c]
            mov dword ptr [eax], 0x42c80000
            mov ecx, dword ptr [ecx]
            push edi
            mov edi, dword ptr [esp + 0x34]
            mov ebx, edx
            mov dword ptr [esp + 0x10], ecx
            xor ebp, ebp
            mov ecx, edi
            sub ebx, edi
    L0045e50e:
            fld dword ptr [esi + 0xc]
            fadd dword ptr [esp + 0x10]
            fld dword ptr [ebx + ecx]
            fxch st(1)
            fsub qword ptr [gDouble_00589fe0]
            fxch st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0045e53a
            fcomp dword ptr [ecx]
            fnstsw ax
            test ah, 1
            jne L0045e83b
            jmp L0045e53c
    L0045e53a:
            fstp st(0)
    L0045e53c:
            fld dword ptr [esi]
            fsub dword ptr [esp + 0x10]
            fld dword ptr [ebx + ecx]
            fxch st(1)
            fsub qword ptr [gDouble_0058a000]
            fxch st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L0045e567
            fcomp dword ptr [ecx]
            fnstsw ax
            test ah, 0x41
            je L0045e83b
            jmp L0045e569
    L0045e567:
            fstp st(0)
    L0045e569:
            inc ebp
            add esi, 4
            add ecx, 4
            cmp ebp, 3
            jl L0045e50e
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x38]
            mov ebx, edi
            xor ebp, ebp
            add eax, 0x10
            sub ebx, edx
            mov esi, edx
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x1c], ebx
    L0045e594:
            mov eax, dword ptr [esp + 0x14]
            fld dword ptr [eax - 0xc]
            fsub dword ptr [esp + 0x10]
            fld dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fcomp dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 1
            je L0045e6bb
            fld dword ptr [esp + 0x38]
            fsub qword ptr [gDouble_0058a000]
            fld dword ptr [ebx + esi]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L0045e6bb
            fld dword ptr [ebx + esi]
            fcomp dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 0x41
            je L0045e5f8
            mov edx, dword ptr [edi]
            fld dword ptr [gUNK_00589fb0]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [edi + 4]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [edi + 8]
            mov dword ptr [ecx + 8], edx
            jmp L0045e675
    L0045e5f8:
            fld dword ptr [ebx + esi]
            fld dword ptr [esp + 0x38]
            fsub dword ptr [esi]
            fxch st(1)
            fsub dword ptr [esi]
            fld dword ptr [gUNK_00589fb0]
            fxch st(1)
            fdivp st(2), st(0)
            fsub st(0), st(1)
            fld dword ptr [edx]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x38]
            fstp dword ptr [ecx]
            fld dword ptr [edx + 4]
            fmul dword ptr [esp + 0x38]
            fstp dword ptr [ecx + 4]
            fld dword ptr [edx + 8]
            fmul dword ptr [esp + 0x38]
            fstp dword ptr [ecx + 8]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            fld dword ptr [edi]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fst dword ptr [esp + 0x24]
            fxch st(2)
            fst dword ptr [esp + 0x28]
            fxch st(2)
            fadd dword ptr [ecx + 4]
            fxch st(2)
            fadd dword ptr [ecx + 8]
            fxch st(1)
            fadd dword ptr [ecx]
            fxch st(2)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            fstp dword ptr [ecx]
            fadd dword ptr [ecx + ebp*4]
            fstp dword ptr [ecx + ebp*4]
    L0045e675:
            mov ebx, dword ptr [esp + 0x18]
            xor edx, edx
            mov edi, ecx
    L0045e67d:
            cmp edx, ebp
            je L0045e698
            fld dword ptr [edi]
            fcomp dword ptr [ebx - 0xc]
            fnstsw ax
            test ah, 1
            jne L0045e6a4
            fld dword ptr [edi]
            fcomp dword ptr [ebx]
            fnstsw ax
            test ah, 0x41
            je L0045e6a4
    L0045e698:
            inc edx
            add edi, 4
            add ebx, 4
            cmp edx, 3
            jl L0045e67d
    L0045e6a4:
            cmp edx, 3
            je L0045e7f1
            mov edx, dword ptr [esp + 0x30]
            mov edi, dword ptr [esp + 0x34]
            mov ebx, dword ptr [esp + 0x1c]
            fstp st(0)
    L0045e6bb:
            mov eax, dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fadd dword ptr [eax]
            fld dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fcomp dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 0x41
            jne L0045e7cf
            fld dword ptr [esp + 0x38]
            fsub qword ptr [gDouble_00589fe0]
            fld dword ptr [ebx + esi]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L0045e7cf
            fld dword ptr [ebx + esi]
            fcomp dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 1
            jne L0045e71e
            mov edx, dword ptr [edi]
            fld dword ptr [gUNK_00589fb0]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [edi + 4]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [edi + 8]
            mov dword ptr [ecx + 8], edx
            jmp L0045e78d
    L0045e71e:
            fld dword ptr [ebx + esi]
            fld dword ptr [esp + 0x38]
            fsub dword ptr [esi]
            fxch st(1)
            fsub dword ptr [esi]
            fld dword ptr [gUNK_00589fb0]
            fxch st(1)
            fdivp st(2), st(0)
            fsub st(0), st(1)
            fld dword ptr [edx]
            fmul st(0), st(1)
            fstp dword ptr [ecx]
            fld dword ptr [edx + 4]
            fmul st(0), st(1)
            fstp dword ptr [ecx + 4]
            fld dword ptr [edx + 8]
            fmul st(0), st(1)
            fstp dword ptr [ecx + 8]
            fstp st(0)
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            fld dword ptr [edi]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fst dword ptr [esp + 0x28]
            fxch st(1)
            fadd dword ptr [ecx + 4]
            fxch st(1)
            fadd dword ptr [ecx + 8]
            fxch st(2)
            fadd dword ptr [ecx]
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fxch st(1)
            fstp dword ptr [ecx + 8]
            fstp dword ptr [ecx]
            fld dword ptr [ecx + ebp*4]
            fsub dword ptr [esp + 0x10]
            fstp dword ptr [ecx + ebp*4]
    L0045e78d:
            mov ebx, dword ptr [esp + 0x18]
            xor edx, edx
            mov edi, ecx
    L0045e795:
            cmp edx, ebp
            je L0045e7b0
            fld dword ptr [edi]
            fcomp dword ptr [ebx - 0xc]
            fnstsw ax
            test ah, 1
            jne L0045e7bc
            fld dword ptr [edi]
            fcomp dword ptr [ebx]
            fnstsw ax
            test ah, 0x41
            je L0045e7bc
    L0045e7b0:
            inc edx
            add edi, 4
            add ebx, 4
            cmp edx, 3
            jl L0045e795
    L0045e7bc:
            cmp edx, 3
            je L0045e81b
            mov edx, dword ptr [esp + 0x30]
            mov edi, dword ptr [esp + 0x34]
            mov ebx, dword ptr [esp + 0x1c]
            fstp st(0)
    L0045e7cf:
            mov eax, dword ptr [esp + 0x14]
            inc ebp
            add eax, 4
            add esi, 4
            cmp ebp, 3
            mov dword ptr [esp + 0x14], eax
            jl L0045e594
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x14
    L0045e7f1:
            mov eax, dword ptr [esp + 0x40]
            mov esi, dword ptr [esp + 0x3c]
            push 0
            push 0
            fstp dword ptr [eax]
            push 0
            push esi
            call BrVector3SetFloat
            add esp, 0x10
            mov dword ptr [esi + ebp*4], 0x3f800000
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x14
    L0045e81b:
            mov ecx, dword ptr [esp + 0x40]
            mov esi, dword ptr [esp + 0x3c]
            push 0
            push 0
            fstp dword ptr [ecx]
            push 0
            push esi
            call BrVector3SetFloat
            add esp, 0x10
            mov dword ptr [esi + ebp*4], 0xbf800000
    L0045e83b:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x14
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}
