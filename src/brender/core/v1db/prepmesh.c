#include "prepmesh.h"

#include "dbsetup.h"

#include "core/fw/brqsort.h"
#include "core/fw/diag.h"
#include "core/fw/resource.h"
#include "core/fw/scratch.h"

#include "core/math/angles.h"
#include "core/math/plane.h"
#include "core/math/vector.h"

#include "core/std/brmath.h"
#include "core/std/brstdlib.h"

#include <assert.h>

#define MODU_FACE_COPY_FLAGS ( 0 \
    | BR_MODU_FACE_COLOURS       \
    )

#define MODU_VERTEX_COPY_FLAGS ( 0 \
    | BR_MODU_VERTEX_POSITIONS     \
    | BR_MODU_VERTEX_COLOURS       \
    | BR_MODU_VERTEX_MAPPING       \
    | BR_MODU_VERTEX_NORMALS       \
    )


// GLOBAL: CARMA2_HW 0x006ad4e0
br_model* compareModel;

// GLOBAL: CARMA2_HW 0x006ad4e4
static pm_temp_edge** pm_edge_scratch;

// GLOBAL: CARMA2_HW 0x006ad4dc
static struct pm_temp_edge * pm_edge_table;

// GLOBAL: CARMA2_HW 0x006ad4e8
static pm_temp_edge** pm_edge_hash;

// GLOBAL: CARMA2_HW 0x006ad4ec
static int num_edges;

#define PREP_BOUNDARY 32
#define PREP_ALIGN(x) (((x)+(PREP_BOUNDARY-1)) & ~(PREP_BOUNDARY-1))

int C2_HOOK_STDCALL addEdge(br_uint_16 first, br_uint_16 last) {
    struct pm_temp_edge *tep;

    for (tep = pm_edge_scratch[last]; tep != NULL; tep = tep->next) {

        if (tep->last == first && tep->other == 0) {
            tep->other = 1;
            return tep - pm_edge_table;
        }
    }

    tep = &pm_edge_table[num_edges];

    tep->first = first;
    tep->last = last;
    tep->other = 0;
    tep->next = pm_edge_scratch[first];
    pm_edge_scratch[first] = tep;

    return num_edges++;
}

#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern float C2_HOOK_CDECL BrFloatSqrt();
extern void* C2_HOOK_CDECL BrMemSet();
extern br_int_32 C2_HOOK_CDECL BrPlaneEquation();
extern void C2_HOOK_CDECL BrQsort();
extern void* C2_HOOK_CDECL BrResAllocate();
extern void C2_HOOK_CDECL BrResFree();
extern void* C2_HOOK_CDECL BrScratchAllocate();
extern void C2_HOOK_CDECL BrScratchFree();
extern char IID_IDirectInputDevice2A[];
extern br_filesystem gZlib_filesystem;
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x00520560
void C2_HOOK_STDCALL prepareEdges(v11group* group, br_model* model) {
    br_size_t scratch_size;
    br_face* mfp;
    v11face* fp;
    int f;

    C2_HOOK_BUG_ON(sizeof(*pm_edge_table) != 12);
    C2_HOOK_BUG_ON(sizeof(*pm_edge_hash) != 4);

    scratch_size = (group->nfaces * 9 + group->nvertices) * sizeof(*pm_edge_hash);

    pm_edge_hash = (struct pm_temp_edge**)BrScratchAllocate(scratch_size);
    pm_edge_scratch = pm_edge_hash;
    pm_edge_table = (struct pm_temp_edge*)((char*)pm_edge_scratch + group->nvertices * sizeof(*pm_edge_hash));

    BrMemSet(pm_edge_hash, 0, group->nvertices * sizeof(*pm_edge_hash));

    num_edges = 0;
	fp = group->faces;

    for (f = 0; f < group->nfaces; f++, fp++) {
        mfp = &model->faces[group->face_user[f]];

        if (!(mfp->flags & BR_FACEF_COPLANAR_0)) {
            fp->edges[0] = addEdge(fp->vertices[0], fp->vertices[1]) + 1;
        } else {
            fp->edges[0] = 0;
        }

        if (!(mfp->flags & BR_FACEF_COPLANAR_1)) {
            fp->edges[1] = addEdge(fp->vertices[1], fp->vertices[2]) + 1;
        } else {
            fp->edges[1] = 0;
        }

        if (!(mfp->flags & BR_FACEF_COPLANAR_2)) {
            fp->edges[2] = addEdge(fp->vertices[2], fp->vertices[0]) + 1;
        } else {
            fp->edges[2] = 0;
        }
    }

    group->nedges = num_edges + 1;

    BrScratchFree(pm_edge_hash);
}

void C2_HOOK_STDCALL BrPrepareEdges(br_model* model) {
    int g;
    struct v11model *v11m;

    v11m = model->prepared;

    if (v11m == NULL) {
        return;
    }

    for (g = 0; g < v11m->ngroups; g++) {
        prepareEdges(&v11m->groups[g], model);
    }
}

// FUNCTION: CARMA2_HW 0x00520840
int C2_HOOK_CDECL FacesCompare(const void* p1, const void* p2) {
    br_face* f1;
    br_face* f2;

    f1 = *(br_face**)p1;
    f2 = *(br_face**)p2;

    if (f1->material > f2->material) {
        return 1;
    }
    if (f1->material < f2->material) {
        return -1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00520860
int C2_HOOK_CDECL TVCompare_XYZ(const void* p1, const void* p2) {
    prep_vertex* tv1;
    prep_vertex* tv2;
    br_vertex* v1;
    br_vertex* v2;
    int i;

    tv1 = *(prep_vertex**)p1;
    tv2 = *(prep_vertex**)p2;

    assert(compareModel != NULL);

    if (tv1->v == tv2->v) {
        return 0;
    }

    v1 = &compareModel->vertices[tv1->v];
    v2 = &compareModel->vertices[tv2->v];

    for (i = 0; i < 3; i++) {
        if (v1->p.v[i] > v2->p.v[i]) {
            return 1;
        }
        if (v1->p.v[i] < v2->p.v[i]) {
            return -1;
        }
    }

    return 0;
}

// FUNCTION: CARMA2_HW 0x005208e0
int C2_HOOK_CDECL TVCompare_MXYZUVN(const void* p1, const void* p2) {
    prep_vertex* tv1;
    prep_vertex* tv2;
    br_vertex* v1;
    br_vertex* v2;
    int i;

    tv1 = *(struct prep_vertex**)p1;
    tv2 = *(struct prep_vertex**)p2;

    assert(compareModel != NULL);

    if (compareModel->faces[tv1->f].material > compareModel->faces[tv2->f].material) {
        return 1;
    }
    if (compareModel->faces[tv1->f].material < compareModel->faces[tv2->f].material) {
        return -1;
    }

    if (tv1->v != tv2->v) {
        v1 = &compareModel->vertices[tv1->v];
        v2 = &compareModel->vertices[tv2->v];

        for (i = 0; i < 3; i++) {
            if (v1->p.v[i] > v2->p.v[i]) {
                return 1;
            }
            if (v1->p.v[i] < v2->p.v[i]) {
                return -1;
            }
        }

        for (i = 0; i < 2; i++) {
            if (v1->map.v[i] > v2->map.v[i]) {
                return 1;
            }
            if (v1->map.v[i] < v2->map.v[i]) {
                return -1;
            }
        }
    }

    for (i = 0; i < 3; i++) {
        if (tv1->n.v[i] > tv2->n.v[i]) {
            return 1;
        }
        if (tv1->n.v[i] < tv2->n.v[i]) {
            return -1;
        }
    }

    return 0;
}

// FUNCTION: CARMA2_HW 0x005209f0
int C2_HOOK_CDECL TVCompare_MVN(const void* p1, const void* p2) {
    prep_vertex* tv1;
    prep_vertex* tv2;
    int i;

    tv1 = *(prep_vertex**)p1;
    tv2 = *(prep_vertex**)p2;

    // if (compareModel->faces[tv1->f].material != NULL) {
    //     LOG_DEBUG("%s, %s", compareModel->faces[tv1->f].material->identifier, compareModel->faces[tv2->f].material->identifier);
    // }
    if (compareModel->faces[tv1->f].material > compareModel->faces[tv2->f].material)
        return 1;
    if (compareModel->faces[tv1->f].material < compareModel->faces[tv2->f].material)
        return -1;

    if (tv1->v > tv2->v)
        return 1;
    if (tv1->v < tv2->v)
        return -1;

    for (i = 0; i < 3; i++) {
        if (tv1->n.v[i] > tv2->n.v[i])
            return 1;
        if (tv1->n.v[i] < tv2->n.v[i])
            return -1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051f5f0
br_fraction C2_HOOK_STDCALL BrScalarToFractionClamp(br_scalar s) {
    if (s <= -1.0f) {
        return -0.99999988f;
    }
    if (s >= 1.0f) {
        return 1.0f;
    }
    return s;
}

void C2_HOOK_STDCALL PrepareFaceNormals(br_model* model) {
    br_vector4 v4;
    br_face* fp;
    int f;

    for (f = 0; f < model->nfaces; f++) {
        fp = &model->faces[f];
        BrPlaneEquation(&v4, &model->vertices[fp->vertices[0]].p, &model->vertices[fp->vertices[1]].p, &model->vertices[fp->vertices[2]].p);
        fp->n.v[0] = BrScalarToFractionClamp(v4.v[0]);
        fp->n.v[1] = BrScalarToFractionClamp(v4.v[1]);
        fp->n.v[2] = BrScalarToFractionClamp(v4.v[2]);
        fp->d = v4.v[3];
    }
}

// FUNCTION: CARMA2_HW 0x00520a90
void C2_NAKED C2_HOOK_STDCALL Smoothing(br_model* model, br_scalar crease_limit, prep_vertex** start, prep_vertex** end) {
#ifndef CARPOCALYPSE2_MATCHING
    prep_vertex** outer;
    prep_vertex** inner;

    for (outer = start; outer < end; outer++) {
        for (inner = start; inner < end; inner++) {
            if (inner == outer || (model->faces[(*outer)->f].smoothing & model->faces[(*inner)->f].smoothing) != 0) {
                BrVector3Accumulate(&(*outer)->n, (br_vector3*)&model->faces[(*inner)->f].n);
            }
        }
    }
#else
    __asm {
            mov ecx, dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 0x10]
            cmp ecx, eax
            mov edx, ecx
            jae L00520b3f
            push edi
            push esi
            mov esi, dword ptr [esp + 0xc]
            push ebp
            push ebx
            jmp L00520ab0
    L00520aac:
            mov ecx, dword ptr [esp + 0x1c]
    L00520ab0:
            cmp ecx, edx
            je L00520ad9
            mov ebx, dword ptr [ecx]
            mov ebp, dword ptr [edx]
            xor eax, eax
            mov edi, dword ptr [esi + 0xc]
            mov ax, word ptr [ebx + 0xe]
            lea ebx, [eax + eax*4]
            xor eax, eax
            mov ax, word ptr [ebp + 0xe]
            mov bx, word ptr [edi + ebx*8 + 6]
            lea eax, [eax + eax*4]
            test word ptr [edi + eax*8 + 6], bx
            je L00520b25
    L00520ad9:
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [edx]
            xor eax, eax
            mov ax, word ptr [ebx + 0xe]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [ebx + eax*8 + 0x18]
            _emit 0xd8
        _emit 0x07
            xor eax, eax
            _emit 0xd9
        _emit 0x1f
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [edx]
            mov ax, word ptr [ebx + 0xe]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [ebx + eax*8 + 0x1c]
            fadd dword ptr [edi + 4]
            xor eax, eax
            fstp dword ptr [edi + 4]
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [edx]
            mov ax, word ptr [ebx + 0xe]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [ebx + eax*8 + 0x20]
            fadd dword ptr [edi + 8]
            fstp dword ptr [edi + 8]
    L00520b25:
            mov eax, dword ptr [esp + 0x20]
            add ecx, 4
            cmp ecx, eax
            jb L00520ab0
            add edx, 4
            cmp edx, eax
            jb L00520aac
            pop ebx
            pop ebp
            pop esi
            pop edi
    L00520b3f:
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00520b50
void C2_NAKED C2_HOOK_STDCALL SmoothingCreased(br_model* model, br_scalar crease_limit, prep_vertex** start, prep_vertex** end) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 o_n;
    prep_vertex** outer;
    prep_vertex** inner;

    for (outer = start; outer < end; outer++) {
        o_n.v[0] = model->faces[(*outer)->f].n.v[0];
        o_n.v[1] = model->faces[(*outer)->f].n.v[1];
        o_n.v[2] = model->faces[(*outer)->f].n.v[2];
        for (inner = start; inner < end; inner++) {
            if ((inner == outer) || ((model->faces[(*outer)->f].smoothing & model->faces[(*inner)->f].smoothing) && (BrVector3Dot((br_vector3*)&model->faces[(*inner)->f].n, &o_n) > crease_limit))) {
                BrVector3Accumulate(&(*outer)->n, (br_vector3*)&model->faces[(*inner)->f].n);
            }
        }
    }
#else
    __asm {
            mov ecx, dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 0x10]
            cmp ecx, eax
            mov edx, ecx
            jae L00520c50
            push edi
            push esi
            mov esi, dword ptr [esp + 0xc]
            push ebp
            push ebx
            jmp L00520b70
    L00520b6c:
            mov ecx, dword ptr [esp + 0x1c]
    L00520b70:
            mov edi, dword ptr [edx]
            xor eax, eax
            mov ax, word ptr [edi + 0xe]
            mov edi, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [edi + eax*8 + 0x18]
            fld dword ptr [edi + eax*8 + 0x1c]
            fld dword ptr [edi + eax*8 + 0x20]
            lea eax, [edi + eax*8]
    L00520b8d:
            cmp ecx, edx
            je L00520be0
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [esi + 0xc]
            xor eax, eax
            mov ebp, dword ptr [edx]
            mov ax, word ptr [ebx + 0xe]
            lea eax, [eax + eax*4]
            lea ebx, [edi + eax*8]
            xor eax, eax
            mov ax, word ptr [ebp + 0xe]
            lea eax, [eax + eax*4]
            mov ax, word ptr [edi + eax*8 + 6]
            test word ptr [ebx + 6], ax
            je L00520c2c
            fld dword ptr [ebx + 0x20]
            fld dword ptr [ebx + 0x1c]
            fld dword ptr [ebx + 0x18]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 0x41
            jne L00520c2c
    L00520be0:
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [edx]
            xor eax, eax
            mov ax, word ptr [ebx + 0xe]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [ebx + eax*8 + 0x18]
            _emit 0xd8
        _emit 0x07
            xor eax, eax
            _emit 0xd9
        _emit 0x1f
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [edx]
            mov ax, word ptr [ebx + 0xe]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [ebx + eax*8 + 0x1c]
            fadd dword ptr [edi + 4]
            xor eax, eax
            fstp dword ptr [edi + 4]
            mov ebx, dword ptr [ecx]
            mov edi, dword ptr [edx]
            mov ax, word ptr [ebx + 0xe]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            fld dword ptr [ebx + eax*8 + 0x20]
            fadd dword ptr [edi + 8]
            fstp dword ptr [edi + 8]
    L00520c2c:
            mov eax, dword ptr [esp + 0x20]
            add ecx, 4
            cmp ecx, eax
            jb L00520b8d
            fstp st(0)
            add edx, 4
            fstp st(0)
            cmp edx, eax
            fstp st(0)
            jb L00520b6c
            pop ebx
            pop ebp
            pop esi
            pop edi
    L00520c50:
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00520c60
void C2_NAKED C2_HOOK_STDCALL CopyVertex(v11group* group, int v, prep_vertex* src, br_model* model) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vertex* srcv;

    srcv = &model->vertices[src->v];

    group->vertices[v].p.v[0] = srcv->p.v[0] - model->pivot.v[0];
    group->vertices[v].p.v[1] = srcv->p.v[1] - model->pivot.v[1];
    group->vertices[v].p.v[2] = srcv->p.v[2] - model->pivot.v[2];

    group->vertices[v].map = srcv->map;

    /* FIXME: use inlined function */
    BrVector3Normalise(&group->vertices[v].n, &src->n);

    group->vertex_colours[v] = BR_COLOUR_RGBA(srcv->red, srcv->grn, srcv->blu, srcv->index);
    group->vertex_user[v] = src->v;
#else
    __asm {
            push ebx
            mov ebx, dword ptr [esp + 0x14]
            push ebp
            push esi
            mov edx, dword ptr [ebx + 8]
            push edi
            mov edi, dword ptr [esp + 0x1c]
            xor eax, eax
            mov esi, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x18]
            mov ax, word ptr [edi + 0xc]
            mov ebp, dword ptr [esi + 0x10]
            lea eax, [eax + eax*4]
            shl ecx, 5
            fld dword ptr [edx + eax*8]
            fsub dword ptr [ebx + 0x14]
            lea edx, [edx + eax*8]
            fstp dword ptr [ecx + ebp]
            fld dword ptr [edx + 4]
            fsub dword ptr [ebx + 0x18]
            mov eax, dword ptr [esi + 0x10]
            fstp dword ptr [ecx + eax + 4]
            fld dword ptr [edx + 8]
            fsub dword ptr [ebx + 0x1c]
            mov eax, dword ptr [esi + 0x10]
            fstp dword ptr [ecx + eax + 8]
            mov eax, dword ptr [esi + 0x10]
            mov ebx, dword ptr [edx + 0xc]
            mov dword ptr [ecx + eax + 0xc], ebx
            mov ebx, dword ptr [edx + 0x10]
            mov dword ptr [ecx + eax + 0x10], ebx
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            _emit 0xd9
        _emit 0x07
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
            fcom dword ptr [IID_IDirectInputDevice2A+0x3ec]
            fnstsw ax
            test ah, 0x41
            jne L00520d25
            fdivr qword ptr [IID_IDirectInputDevice2A+0x3f0]
            mov eax, dword ptr [esi + 0x10]
            _emit 0xd9
        _emit 0x07
            fmul st(0), st(1)
            fstp dword ptr [ecx + eax + 0x14]
            fld dword ptr [edi + 4]
            mov eax, dword ptr [esi + 0x10]
            fmul st(0), st(1)
            fstp dword ptr [ecx + eax + 0x18]
            fld dword ptr [edi + 8]
            mov eax, dword ptr [esi + 0x10]
            fmul st(0), st(1)
            fstp dword ptr [ecx + eax + 0x1c]
            fstp st(0)
            jmp L00520d42
    L00520d25:
            mov eax, dword ptr [esi + 0x10]
            fstp st(0)
            mov dword ptr [ecx + eax + 0x14], 0x3f800000
            mov ebx, dword ptr [esi + 0x10]
            xor eax, eax
            mov dword ptr [ecx + ebx + 0x18], eax
            mov ebx, dword ptr [esi + 0x10]
            mov dword ptr [ecx + ebx + 0x1c], eax
    L00520d42:
            xor ecx, ecx
            xor eax, eax
            mov ch, byte ptr [edx + 0x14]
            mov al, byte ptr [edx + 0x16]
            mov cl, byte ptr [edx + 0x15]
            shl ecx, 8
            or ecx, eax
            xor eax, eax
            mov al, byte ptr [edx + 0x17]
            mov edx, dword ptr [esi + 0x14]
            shl ecx, 8
            or ecx, eax
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [edx + eax*4], ecx
            mov dx, word ptr [edi + 0xc]
            mov ecx, dword ptr [esi + 0x18]
            pop edi
            pop esi
            pop ebp
            mov word ptr [ecx + eax*2], dx
            pop ebx
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00520d80
void C2_NAKED C2_HOOK_STDCALL CopyFace(v11group* group, int f, br_face* src, br_model* model) {
#ifndef CARPOCALYPSE2_MATCHING

    group->faces[f].vertices[0] = src->vertices[0];
    group->faces[f].vertices[1] = src->vertices[1];
    group->faces[f].vertices[2] = src->vertices[2];
    group->face_colours.colours[f] = BR_COLOUR_RGBA(src->red, src->grn, src->blu, src->index);
    group->faces[f].eqn.v[0] = src->n.v[0];
    group->faces[f].eqn.v[1] = src->n.v[1];
    group->faces[f].eqn.v[2] = src->n.v[2];
    group->faces[f].eqn.v[3] = src->d;
    group->face_user[f] = src - model->faces;
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push ebp
            mov ebp, dword ptr [esp + 0xc]
            push esi
            mov dx, word ptr [eax]
            mov esi, ebp
            push edi
            mov edi, dword ptr [esp + 0x10]
            shl esi, 3
            mov ecx, dword ptr [edi + 4]
            sub esi, ebp
            shl esi, 2
            mov word ptr [ecx + esi], dx
            mov ecx, dword ptr [edi + 4]
            mov dx, word ptr [eax + 2]
            mov word ptr [ecx + esi + 2], dx
            mov ecx, dword ptr [edi + 4]
            mov dx, word ptr [eax + 4]
            mov word ptr [ecx + esi + 4], dx
            xor ecx, ecx
            mov ch, byte ptr [eax + 0xc]
            xor edx, edx
            mov cl, byte ptr [eax + 0xd]
            mov dl, byte ptr [eax + 0xe]
            shl ecx, 8
            or ecx, edx
            xor edx, edx
            mov dl, byte ptr [eax + 0xf]
            shl ecx, 8
            or ecx, edx
            mov edx, dword ptr [edi + 8]
            mov dword ptr [edx + ebp*4], ecx
            mov ecx, dword ptr [edi + 4]
            fld dword ptr [eax + 0x18]
            fstp dword ptr [ecx + esi + 0xc]
            fld dword ptr [eax + 0x1c]
            mov edx, dword ptr [edi + 4]
            fstp dword ptr [edx + esi + 0x10]
            fld dword ptr [eax + 0x20]
            mov ecx, dword ptr [edi + 4]
            fstp dword ptr [ecx + esi + 0x14]
            mov edx, dword ptr [edi + 4]
            mov ecx, dword ptr [eax + 0x24]
            mov dword ptr [edx + esi + 0x18], ecx
            mov edx, dword ptr [esp + 0x1c]
            sub eax, dword ptr [edx + 0xc]
            mov ecx, eax
            mov eax, 0x66666667
            imul ecx
            mov ecx, dword ptr [edi + 0xc]
            pop edi
            sar edx, 4
            mov eax, edx
            pop esi
            shr eax, 0x1f
            add edx, eax
            mov word ptr [ecx + ebp*2], dx
            pop ebp
            ret 0x10
    }
#endif
}

void C2_HOOK_STDCALL PrepareGroups(br_model* model) {
    br_qsort_cbfn* vertex_compare_groups;
    void (C2_HOOK_STDCALL*smoothing_fn)(br_model*, br_scalar, prep_vertex**, prep_vertex**);
    br_size_t block_size;
    prep_vertex* temp_verts;
    prep_vertex* gtvp;
    prep_vertex** sorted_vertices;
    br_face* fp;
    int g;
    int f;
    int v;
    int i;
    int ntemps;
    int count;
    int nf;
    int nv;
    int ng;
    int old_count;
    br_scalar crease_limit;
    v11model* v11m;
    v11group* v11g;
    v11face* v11f;
    fmt_vertex* v11v;
    br_colour* v11fcolours;
    br_colour* v11vcolours;
    br_uint_16* v11fuser;
    br_uint_16* v11vuser;
    br_face** sorted_faces;
    br_uint_8* cp;

    if (model->flags & BR_MODF_DONT_WELD) {
        vertex_compare_groups = TVCompare_MVN;
    } else {
        vertex_compare_groups = TVCompare_MXYZUVN;
    }

    if (model->flags & BR_MODF_CREASE) {
        crease_limit = BR_COS(model->crease_angle);
        smoothing_fn = SmoothingCreased;
    } else {
        crease_limit = 0.f;
        smoothing_fn = Smoothing;
    }

    compareModel = model;

    C2_HOOK_BUG_ON(3 * (sizeof(*temp_verts) + sizeof(*sorted_vertices)) + sizeof(sorted_faces) != 64);

    ntemps = model->nfaces * 3;
    block_size = ntemps * (sizeof(*temp_verts) + sizeof(*sorted_vertices)) + model->nfaces * sizeof(sorted_faces);
    temp_verts = BrScratchAllocate(block_size);
    BrMemSet(temp_verts, 0, block_size);

    sorted_vertices = (struct prep_vertex **)(temp_verts + ntemps);
    sorted_faces = (struct br_face **)(sorted_vertices + ntemps);

    gtvp = temp_verts;
    for (i = 0, f = 0; f < model->nfaces; f++) {
        fp = &model->faces[f];
        sorted_faces[f] = fp;

        for (v = 0; v < 3; v++, i++) {
            gtvp->v = fp->vertices[v];
            gtvp->f = f;

            sorted_vertices[i] = gtvp;
            gtvp++;
        }
    }

    BrQsort(sorted_faces, model->nfaces, sizeof(*sorted_faces), FacesCompare);

    BrQsort(sorted_vertices, ntemps, sizeof(*sorted_vertices), TVCompare_XYZ);

    for (v = 0, i = 0; v < ntemps - 1; v++) {

        if (TVCompare_XYZ(&sorted_vertices[v], &sorted_vertices[v + 1])) {
            smoothing_fn(model, crease_limit, &sorted_vertices[i], &sorted_vertices[v + 1]);
            i = v + 1;
        }
    }

    smoothing_fn(model, crease_limit, &sorted_vertices[i], &sorted_vertices[ntemps]);

    BrQsort(sorted_vertices, ntemps, sizeof(*sorted_vertices), vertex_compare_groups);

    nv = 1;
    for (v = 0; v < ntemps - 1; v++) {
        if (vertex_compare_groups(&sorted_vertices[v], &sorted_vertices[v + 1])) {
            nv++;
        }
    }

    ng = 1;
    for (f = 1; f < model->nfaces; f++) {
        if (sorted_faces[f]->material != sorted_faces[f - 1]->material) {
            ng++;
        }
    }

    nf = model->nfaces;

    block_size = 0
        + PREP_ALIGN(sizeof(v11model))
        + PREP_ALIGN(ng * sizeof(v11group))
        + PREP_ALIGN(nf * sizeof(v11face))
        + PREP_ALIGN(nv * sizeof(*v11g->vertices))
        + PREP_ALIGN(nv * sizeof(br_colour))
        + PREP_ALIGN(nf * sizeof(br_colour))
        + nv * sizeof(br_int_16)
        + nf * sizeof(br_int_16);

    if (model->prepared != NULL && block_size > model->prepared->size) {
        BrResFree(model->prepared);
        model->prepared = NULL;
    }

    if (model->prepared == NULL) {
        model->prepared = BrResAllocate(model, block_size, BR_MEMORY_PREPARED_MODEL);
        model->prepared->size = block_size;
    }

    cp = (void*)model->prepared;

    v11m = (void*)cp; cp += PREP_ALIGN(sizeof(v11model));
    v11g = (void*)cp; cp += PREP_ALIGN(ng * sizeof(v11group));
    v11f = (void*)cp; cp += PREP_ALIGN(nf * sizeof(v11face));

    v11v = (void*)cp; cp += PREP_ALIGN(nv * sizeof(*v11g->vertices));

    v11vcolours = (void*)cp; cp += PREP_ALIGN(nv * sizeof(br_colour));
    v11fcolours = (void*)cp; cp += PREP_ALIGN(nf * sizeof(br_colour));

    v11vuser = (void*)cp; cp += nv * sizeof(br_uint_16);
    v11fuser = (void*)cp; cp += nf * sizeof(br_uint_16);

    v11m->groups = v11g;
    v11m->ngroups = ng;

    model->flags &= ~BR_MODF_USED_PREPARED_USER;

    for (f = 0, g = 0; f < model->nfaces; g++) {
        v11group* gp = &v11g[g];

        gp->faces = &v11f[f];
        gp->face_colours.colours = &v11fcolours[f];
        gp->face_user = &v11fuser[f];
        if (sorted_faces[f]->material == NULL) {
            gp->stored = NULL;
        } else {
            gp->stored = sorted_faces[f]->material->stored;
            if (IsMaterialTransparent(sorted_faces[f]->material)) {
                model->flags |= BR_MODF_USED_PREPARED_USER;
            }
        }
        gp->nfaces = 0;
        for (i = f; i < model->nfaces; i++) {

            if (sorted_faces[i]->material != sorted_faces[f]->material) {
                break;
            }
            CopyFace(gp, gp->nfaces, sorted_faces[i], model);
            gp->nfaces++;
        }
        f = i;
    }

    v11g[0].vertices = v11v;
    v11g[0].vertex_colours = v11vcolours;
    v11g[0].vertex_user = v11vuser;
    v11g[0].nvertices = 1;

    CopyVertex(v11g, 0, sorted_vertices[0], model);

    for (v = 0, g = 0, count = 0; v < ntemps - 1; v++) {

        if (model->faces[sorted_vertices[v]->f].material != model->faces[sorted_vertices[v + 1]->f].material) {
            g++;
            v11g[g].vertices = &v11v[count + 1];
            v11g[g].vertex_colours = &v11vcolours[count + 1];
            v11g[g].vertex_user = &v11vuser[count + 1];
            v11g[g].nvertices = 0;
        }

        old_count = count;
        if (vertex_compare_groups(&sorted_vertices[v], &sorted_vertices[v + 1])) {
            count++;
            sorted_vertices[v]->v = count;
            CopyVertex(&v11g[g], v11g[g].nvertices, sorted_vertices[v + 1], model);
            v11g[g].nvertices++;
        }

        sorted_vertices[v]->v = old_count;
    }

    sorted_vertices[v]->v = count;

    for (g = 0; g < ng; g++) {
        for (f = 0; f < v11g[g].nfaces; f++) {
            i = v11g[g].vertices - v11v;
            v = v11g[g].face_user[f] * 3;

            v11g[g].faces[f].vertices[0] = temp_verts[v + 0].v - i;
            v11g[g].faces[f].vertices[1] = temp_verts[v + 1].v - i;
            v11g[g].faces[f].vertices[2] = temp_verts[v + 2].v - i;
        }
    }

    BrScratchFree(temp_verts);
}

void C2_HOOK_STDCALL PrepareBoundingRadius(br_model* model) {
    float d;
    float max = 0.0f;
    int v;
    br_vertex* vp;

    for (v = 0; v < model->nvertices; v++) {
        vp = &model->vertices[v];
        d = BrVector3Dot(&vp->p, &vp->p);

        if (d > max) {
            max = d;
        }
    }

    model->radius = BrFloatSqrt(max);
}

void C2_HOOK_STDCALL PrepareBoundingBox(br_model* model) {
    int axis;
    int v;
    br_vertex* vp;
    br_scalar x;

    for (axis = 0; axis < 3; axis++) {
        model->bounds.min.v[axis] = model->bounds.max.v[axis] = model->vertices[0].p.v[axis];
    }

    for (v = 1; v < model->nvertices; v++) {
        vp = &model->vertices[v];

        for (axis = 0; axis < 3; axis++) {

            x = vp->p.v[axis];

            if (x > model->bounds.max.v[axis]) {
                model->bounds.max.v[axis] = x;
            }

            if (x < model->bounds.min.v[axis]) {
                model->bounds.min.v[axis] = x;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051f630
int C2_NAKED C2_HOOK_STDCALL IsMaterialTransparent(const br_material* material) {
#ifndef CARPOCALYPSE2_MATCHING
    br_token_value* tvp;

    if (material == NULL) {
        return 0;
    }
    if (material->colour_map != NULL) {
        br_uint_8 type = material->colour_map->type;

        switch (type) {
        case BR_PMT_ALPHA_8:
        case BR_PMT_INDEXA_88:
        case BR_PMT_RGBA_4444:
        case BR_PMT_ALPHA_4:
        case BR_PMT_INDEXA_44:
        case BR_PMT_DEPTH_15:
        case BR_PMT_ARGB_1555:
            return 1;
        }
        if (material->index_blend != NULL) {
            return 1;
        }
    }

    tvp = material->extra_prim;
    if (tvp != NULL) {
        while (tvp->t != BR_NULL_TOKEN) {
            if (tvp->t == BRT_OPACITY_X || tvp->t == BRT_OPACITY_F) {
                return 1;
            }
            tvp++;
        }
    }
    return 0;
#else
    __asm {
            mov ecx, dword ptr [esp + 4]
            test ecx, ecx
            je L0051f697
            mov eax, dword ptr [ecx + 0x40]
            test eax, eax
            je L0051f65e
            mov al, byte ptr [eax + 0x2c]
            cmp al, 0xd
            je L0051f68f
            cmp al, 0xe
            je L0051f68f
            cmp al, 0x12
            je L0051f68f
            cmp al, 0x18
            je L0051f68f
            cmp al, 0x19
            je L0051f68f
            cmp al, 0x1a
            je L0051f68f
            cmp al, 0x1f
            je L0051f68f
    L0051f65e:
            mov eax, dword ptr [ecx + 0x4c]
            test eax, eax
            jne L0051f68f
            mov ecx, dword ptr [ecx + 0x58]
            test ecx, ecx
            je L0051f697
            mov eax, dword ptr [ecx]
            test eax, eax
            je L0051f68a
    L0051f672:
            cmp eax, 0xbe
            je L0051f68a
            cmp eax, 0xbf
            je L0051f68a
            mov eax, dword ptr [ecx + 8]
            add ecx, 8
            test eax, eax
            jne L0051f672
    L0051f68a:
            cmp dword ptr [ecx], 0
            je L0051f697
    L0051f68f:
            mov eax, 1
            ret 4
    L0051f697:
            xor eax, eax
            ret 4
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051f6a0
void C2_NAKED C2_HOOK_STDCALL RegenerateFaceNormals(v11model* v11m) {
#ifndef CARPOCALYPSE2_MATCHING
    int g;
    int f;
    v11face* fp;

    for (g = 0; g < v11m->ngroups; g++) {
        for (f = 0; f < v11m->groups[g].nfaces; f++) {
            fp = &v11m->groups[g].faces[f];
            BrPlaneEquation(&fp->eqn,
                &v11m->groups[g].vertices[fp->vertices[0]].p,
                &v11m->groups[g].vertices[fp->vertices[1]].p,
                &v11m->groups[g].vertices[fp->vertices[2]].p);
        }
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x10]
            xor ebx, ebx
            mov dword ptr [esp + 8], ebx
            cmp word ptr [ebp + 8], bx
            jbe L0051f722
            push edi
            push esi
    L0051f6b5:
            mov eax, dword ptr [ebp + 0x18]
            xor edi, edi
            add eax, ebx
            cmp word ptr [eax + 0x1c], di
            mov esi, dword ptr [eax + 4]
            jbe L0051f70a
    L0051f6c5:
            mov eax, dword ptr [eax + 0x10]
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            xor edx, edx
            mov dx, word ptr [esi + 2]
            shl ecx, 5
            add ecx, eax
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi]
            shl edx, 5
            add edx, eax
            shl ecx, 5
            push edx
            add ecx, eax
            lea edx, [esi + 0xc]
            push ecx
            push edx
            call BrPlaneEquation
            mov eax, dword ptr [ebp + 0x18]
            xor ecx, ecx
            add eax, ebx
            add esp, 0x10
            inc edi
            add esi, 0x1c
            mov cx, word ptr [eax + 0x1c]
            cmp edi, ecx
            jl L0051f6c5
    L0051f70a:
            mov eax, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [ebp + 8]
            inc eax
            add ebx, 0x24
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L0051f6b5
            pop esi
            pop edi
    L0051f722:
            pop ebp
            pop ebx
            pop ecx
            ret 4
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

// FUNCTION: CARMA2_HW 0x0051f730
void C2_NAKED C2_HOOK_STDCALL RegenerateVertexNormals(v11model* v11m) {
#ifndef CARPOCALYPSE2_MATCHING
    int g;
    int v;
    int f;
    v11face* fp;
    fmt_vertex* vp;
    br_vector3* normals;

    for (g = 0; g < v11m->ngroups; g++) {
        normals = BrScratchAllocate(v11m->groups[g].nvertices * sizeof(br_vector3));
        BrMemSet(normals, 0, v11m->groups[g].nvertices * sizeof(br_vector3));
        for (f = 0; f < v11m->groups[g].nfaces; f++) {
            fp = &v11m->groups[g].faces[f];
            normals[fp->vertices[0]].v[0] += fp->eqn.v[0];
            normals[fp->vertices[0]].v[1] += fp->eqn.v[1];
            normals[fp->vertices[0]].v[2] += fp->eqn.v[2];
            normals[fp->vertices[1]].v[0] += fp->eqn.v[0];
            normals[fp->vertices[1]].v[1] += fp->eqn.v[1];
            normals[fp->vertices[1]].v[2] += fp->eqn.v[2];
            normals[fp->vertices[2]].v[0] += fp->eqn.v[0];
            normals[fp->vertices[2]].v[1] += fp->eqn.v[1];
            normals[fp->vertices[2]].v[2] += fp->eqn.v[2];
        }
        for (v = 0; v < v11m->groups[g].nvertices; v++) {
            vp = &v11m->groups[g].vertices[v];
            // FIXME: use inlined variant? BR_VECTOR3_NORMALISE (unsure about exact naming)
            BrVector3Normalise(&vp->n, &normals[v]);
        }

        BrScratchFree(normals);
    }
#else
    __asm {
            push ecx
            push ebx
            mov ebx, dword ptr [esp + 0xc]
            push edi
            xor edi, edi
            cmp word ptr [ebx + 8], di
            mov dword ptr [esp + 8], edi
            jbe L0051f944
            push esi
            push ebp
    L0051f749:
            mov ecx, dword ptr [ebx + 0x18]
            xor eax, eax
            mov ax, word ptr [edi + ecx + 0x1e]
            lea edx, [eax + eax*2]
            shl edx, 2
            push edx
            call BrScratchAllocate
            mov ecx, dword ptr [ebx + 0x18]
            mov esi, eax
            xor eax, eax
            add esp, 4
            mov ax, word ptr [edi + ecx + 0x1e]
            lea edx, [eax + eax*2]
            shl edx, 2
            push edx
            push 0
            push esi
            call BrMemSet
            mov eax, dword ptr [ebx + 0x18]
            xor edx, edx
            add esp, 0xc
            cmp word ptr [edi + eax + 0x1c], dx
            lea ecx, [edi + eax]
            mov eax, dword ptr [edi + eax + 4]
            jbe L0051f86c
    L0051f797:
            xor ecx, ecx
            xor ebp, ebp
            mov cx, word ptr [eax]
            fld dword ptr [eax + 0xc]
            lea ecx, [ecx + ecx*2]
            add eax, 0x1c
            fadd dword ptr [esi + ecx*4]
            lea ecx, [esi + ecx*4]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x1c]
            fld dword ptr [eax - 0xc]
            lea ecx, [ecx + ecx*2]
            fadd dword ptr [esi + ecx*4 + 4]
            lea ecx, [esi + ecx*4 + 4]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x1c]
            lea ecx, [ecx + ecx*2]
            fld dword ptr [esi + ecx*4 + 8]
            fadd dword ptr [eax - 8]
            lea ecx, [esi + ecx*4 + 8]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x1a]
            lea ecx, [ecx + ecx*2]
            fld dword ptr [esi + ecx*4]
            fadd dword ptr [eax - 0x10]
            lea ecx, [esi + ecx*4]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x1a]
            lea ecx, [ecx + ecx*2]
            fld dword ptr [esi + ecx*4 + 4]
            fadd dword ptr [eax - 0xc]
            lea ecx, [esi + ecx*4 + 4]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x1a]
            lea ecx, [ecx + ecx*2]
            fld dword ptr [esi + ecx*4 + 8]
            fadd dword ptr [eax - 8]
            lea ecx, [esi + ecx*4 + 8]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x18]
            lea ecx, [ecx + ecx*2]
            fld dword ptr [esi + ecx*4]
            fadd dword ptr [eax - 0x10]
            lea ecx, [esi + ecx*4]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            mov cx, word ptr [eax - 0x18]
            fld dword ptr [eax - 0xc]
            lea ecx, [ecx + ecx*2]
            fadd dword ptr [esi + ecx*4 + 4]
            lea ecx, [esi + ecx*4 + 4]
            _emit 0xd9
        _emit 0x19
            xor ecx, ecx
            inc edx
            mov cx, word ptr [eax - 0x18]
            fld dword ptr [eax - 8]
            lea ecx, [ecx + ecx*2]
            fadd dword ptr [esi + ecx*4 + 8]
            lea ecx, [esi + ecx*4 + 8]
            _emit 0xd9
        _emit 0x19
            mov ecx, dword ptr [ebx + 0x18]
            mov bp, word ptr [edi + ecx + 0x1c]
            cmp edx, ebp
            jl L0051f797
    L0051f86c:
            mov edx, dword ptr [ebx + 0x18]
            xor ebp, ebp
            cmp word ptr [edi + edx + 0x1e], bp
            mov ecx, dword ptr [edi + edx + 0x10]
            lea eax, [edi + edx]
            jbe L0051f91f
            add ecx, 0x18
            lea edx, [esi + 8]
    L0051f889:
            fld dword ptr [edx - 8]
            fld dword ptr [edx - 4]
            _emit 0xd9
        _emit 0x02
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x3ec]
            fnstsw ax
            test ah, 0x41
            jne L0051f8e8
            fdivr qword ptr [IID_IDirectInputDevice2A+0x3f0]
            fld dword ptr [edx - 8]
            fmul st(0), st(1)
            fstp dword ptr [ecx - 4]
            fld dword ptr [edx - 4]
            fmul st(0), st(1)
            _emit 0xd9
        _emit 0x19
            _emit 0xd9
        _emit 0x02
            fmul st(0), st(1)
            fstp dword ptr [ecx + 4]
            fstp st(0)
            jmp L0051f8fe
    L0051f8e8:
            fstp st(0)
            mov dword ptr [ecx - 4], 0x3f800000
            mov dword ptr [ecx], 0
            mov dword ptr [ecx + 4], 0
    L0051f8fe:
            inc ebp
            xor eax, eax
            mov dword ptr [esp + 0x18], ebp
            mov ebp, dword ptr [ebx + 0x18]
            add edx, 0xc
            add ecx, 0x20
            mov ax, word ptr [edi + ebp + 0x1e]
            mov ebp, dword ptr [esp + 0x18]
            cmp ebp, eax
            jl L0051f889
    L0051f91f:
            push esi
            call BrScratchFree
            mov eax, dword ptr [esp + 0x14]
            xor ecx, ecx
            mov cx, word ptr [ebx + 8]
            add esp, 4
            inc eax
            add edi, 0x24
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L0051f749
            pop ebp
            pop esi
    L0051f944:
            pop edi
            pop ebx
            pop ecx
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051f950
void C2_NAKED C2_HOOK_CDECL BrModelUpdate(br_model* model, br_uint_16 flags) {
#ifndef CARPOCALYPSE2_MATCHING
    int g, f, v;
    struct v11model *v11m;
    struct fmt_vertex *fvp;
    br_face *fp;
    br_vertex *vp;

    if (model->flags & BR_MODF_PREPREPARED) {
        return;
    }

    if (model->faces == NULL || model->vertices == NULL) {
        BrFailure("BrModelUpdate: model has no faces or vertices (%s)",
                  model->identifier != NULL ? model->identifier : "<NULL>");
    }

    if (flags & BR_MODU_PIVOT) {
        flags |= BR_MODU_VERTEX_POSITIONS;
    }

    if (model->flags & (BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL)) {
        model->flags |= BR_MODF_UPDATEABLE;
    }

    if (!(model->flags & BR_MODF_CUSTOM_BOUNDS) && (flags & (BR_MODU_VERTICES | BR_MODU_VERTEX_POSITIONS))) {
        PrepareBoundingRadius(model);
        PrepareBoundingBox(model);
    }

    if (model->prepared != NULL && !(flags & ~(MODU_VERTEX_COPY_FLAGS | MODU_FACE_COPY_FLAGS))) {
        v11m = model->prepared;

        if (model->vertices != NULL && (flags & MODU_VERTEX_COPY_FLAGS)) {
            for (g = 0; g < v11m->ngroups; g++) {
                for (v = 0; v < v11m->groups[g].nvertices; v++) {
                    fvp = &v11m->groups[g].vertices[v];

                    vp = &model->vertices[v11m->groups[g].vertex_user[v]];

                    if (flags & BR_MODU_VERTEX_POSITIONS) {
                        fvp->p.v[0] = vp->p.v[0] - model->pivot.v[0];
                        fvp->p.v[1] = vp->p.v[1] - model->pivot.v[1];
                        fvp->p.v[2] = vp->p.v[2] - model->pivot.v[2];
                    }

                    if (flags & BR_MODU_VERTEX_COLOURS) {
                        v11m->groups[g].vertex_colours[v] = BR_COLOUR_RGBA(vp->red, vp->grn, vp->blu, vp->index);
                    }

                    if (flags & BR_MODU_VERTEX_MAPPING) {
                        fvp->map.v[0] = vp->map.v[0];
                        fvp->map.v[1] = vp->map.v[1];
                    }

                    if ((flags & BR_MODU_VERTEX_NORMALS) && (model->flags & BR_MODF_CUSTOM_NORMALS)) {
                        fvp->n.v[0] = vp->n.v[0];
                        fvp->n.v[1] = vp->n.v[1];
                        fvp->n.v[2] = vp->n.v[2];
                    }
                }
            }
        }

        if (model->faces != NULL && (flags & MODU_FACE_COPY_FLAGS)) {

            for (g = 0; g < v11m->ngroups; g++) {
                for (f = 0; f < v11m->groups[g].nfaces; f++) {

                    fp = model->faces + v11m->groups[g].face_user[f];

                    if (flags & BR_MODU_FACE_COLOURS) {
                        v11m->groups[g].face_colours.colours[f] = BR_COLOUR_RGBA(fp->red, fp->grn, fp->blu, fp->index);
                    }
                }
            }
        }

        if (flags & BR_MODU_VERTEX_POSITIONS) {
            if (model->flags & BR_MODF_CUSTOM_NORMALS) {
                RegenerateFaceNormals(v11m);
            } else {
                RegenerateVertexNormals(v11m);
            }
        }
    } else {

        if (model->faces == NULL || model->vertices == NULL) {
            return;
        }

        PrepareFaceNormals(model);
        model->flags &= ~_BR_MODF_RESERVED;

        for (f = 0; f < model->nfaces; f++) {
            fp = &model->faces[f];

            if (fp->material == NULL) {
                model->flags |= _BR_MODF_RESERVED;
            }

            for (v = 0; v < 3; v++) {
                if (fp->vertices[v] >= model->nvertices) {
                    BrFailure("face references invalid vertex f=%d v=%d", f, v);
                }
            }
        }
        PrepareGroups(model);
        BrPrepareEdges(model);
    }

    if (!(model->flags & BR_MODF_UPDATEABLE)) {
        if (model->faces != NULL) {
            BrResFree(model->faces);
        }

        if (model->vertices != NULL) {
            BrResFree(model->vertices);
        }

        model->faces = NULL;
        model->vertices = NULL;

        model->nfaces = 0;
        model->nvertices = 0;
    }
    if (v1db.renderer != NULL && v1db.format_model != NULL &&
        !(model->flags & BR_MODF_UPDATEABLE) && (model->flags & BR_MODF_FACES_ONLY)) {

        br_error r;
        br_geometry_stored *sg;
        br_boolean b;
        br_token_value tv[] = {
            { BRT_CAN_SHARE_B, { 1 } },
            { 0 },
        };

        if (model->stored != NULL) {
            model->stored->dispatch->_free(model->stored);
            model->stored = NULL;
        }

        r = ((br_geometry_v1_model*)v1db.format_model)->dispatch->_storedNewF((br_geometry_v1_model*)v1db.format_model, v1db.renderer, &sg, model->prepared, BRT_TRIANGLE, tv);

        if (r == 0 && sg != NULL) {
            model->stored = (br_object*)sg;

            r = sg->dispatch->_query((br_object*)sg, (br_uint_32 *)&b, BRT_SHARED_B);

            if (r == 0 && !b) {
                BrResFree(model->prepared);
                model->prepared = NULL;
            }
        }
    }
#else
    __asm {
            sub esp, 0x40
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x50]
            push edi
            test byte ptr [esi + 0x20], 0x40
            jne L00520550
            mov eax, dword ptr [esi + 0xc]
            test eax, eax
            je L0051f973
            mov eax, dword ptr [esi + 8]
            test eax, eax
            jne L0051f98d
    L0051f973:
            mov eax, dword ptr [esi + 4]
            test eax, eax
            jne L0051f97f
            mov eax, offset gZlib_filesystem+0x11c
    L0051f97f:
            push eax
            push offset gZlib_filesystem+0x124
            call BrFailure
            add esp, 8
    L0051f98d:
            mov ebx, dword ptr [esp + 0x58]
            test bh, 1
            je L0051f99d
            or ebx, 1
            mov dword ptr [esp + 0x58], ebx
    L0051f99d:
            mov ax, word ptr [esi + 0x20]
            test al, 6
            je L0051f9ab
            or al, 0x80
            mov word ptr [esi + 0x20], ax
    L0051f9ab:
            test byte ptr [esi + 0x21], 4
            jne L0051faa2
            test bl, 0x41
            je L0051faa2
            mov ecx, dword ptr [esi + 8]
            xor eax, eax
            mov ax, word ptr [esi + 0x10]
            mov dword ptr [esp + 0x54], 0
            test eax, eax
            jle L0051fa1a
            mov edx, eax
    L0051f9d5:
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [esp + 0x54]
            fnstsw ax
            test ah, 0x41
            jne L0051fa12
            fstp dword ptr [esp + 0x54]
            jmp L0051fa14
    L0051fa12:
            fstp st(0)
    L0051fa14:
            add ecx, 0x28
            dec edx
            jne L0051f9d5
    L0051fa1a:
            mov eax, dword ptr [esp + 0x54]
            push eax
            call BrFloatSqrt
            mov ecx, 0xffffffcc
            add esp, 4
            fstp dword ptr [esi + 0x30]
            lea eax, [esi + 0x34]
            sub ecx, esi
            mov edx, 3
    L0051fa39:
            mov ebx, dword ptr [esi + 8]
            lea edi, [ecx + eax]
            add eax, 4
            dec edx
            fld dword ptr [edi + ebx]
            fst dword ptr [eax + 8]
            fstp dword ptr [eax - 4]
            jne L0051fa39
            mov ebp, dword ptr [esi + 8]
            mov ebx, 1
            add ebp, 0x28
            cmp word ptr [esi + 0x10], bx
            jbe L0051fa9e
    L0051fa5f:
            mov edx, ebp
            lea ecx, [esi + 0x34]
            mov edi, 3
    L0051fa69:
            _emit 0xd9
        _emit 0x02
            fcom dword ptr [ecx + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L0051fa78
            fst dword ptr [ecx + 0xc]
    L0051fa78:
            _emit 0xd8
        _emit 0x11
            fnstsw ax
            test ah, 1
            je L0051fa85
            _emit 0xd9
        _emit 0x19
            jmp L0051fa87
    L0051fa85:
            fstp st(0)
    L0051fa87:
            add edx, 4
            add ecx, 4
            dec edi
            jne L0051fa69
            xor ecx, ecx
            inc ebx
            mov cx, word ptr [esi + 0x10]
            add ebp, 0x28
            cmp ebx, ecx
            jl L0051fa5f
    L0051fa9e:
            mov ebx, dword ptr [esp + 0x58]
    L0051faa2:
            mov edi, dword ptr [esi + 0x4c]
            xor ebp, ebp
            cmp edi, ebp
            mov dword ptr [esp + 0x10], edi
            je L0051fccf
            mov edx, ebx
            and edx, 0xffff
            test edx, 0xffffffd0
            jne L0051fccf
            cmp dword ptr [esi + 8], ebp
            je L0051fc0c
            test bl, 0xf
            je L0051fc0c
            cmp word ptr [edi + 8], bp
            mov dword ptr [esp + 0x54], ebp
            jbe L0051fc0c
    L0051fae7:
            mov eax, dword ptr [edi + 0x18]
            xor edx, edx
            cmp word ptr [ebp + eax + 0x1e], dx
            jbe L0051fbf2
            mov ecx, dword ptr [esp + 0x58]
            and ebx, 1
            and ecx, 2
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x34], ecx
            mov ecx, dword ptr [esp + 0x58]
            and ecx, 4
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x2c], ecx
            mov ecx, dword ptr [esp + 0x58]
            and ecx, 8
            mov dword ptr [esp + 0x28], ecx
    L0051fb23:
            mov ecx, dword ptr [ebp + eax + 0x10]
            mov edi, dword ptr [esp + 0x14]
            add ecx, edi
            mov edi, dword ptr [ebp + eax + 0x18]
            xor eax, eax
            mov ax, word ptr [edi + edx*2]
            mov edi, dword ptr [esi + 8]
            test bx, bx
            lea eax, [eax + eax*4]
            lea edi, [edi + eax*8]
            je L0051fb5e
            _emit 0xd9
        _emit 0x07
            fsub dword ptr [esi + 0x14]
            _emit 0xd9
        _emit 0x19
            fld dword ptr [edi + 4]
            fsub dword ptr [esi + 0x18]
            fstp dword ptr [ecx + 4]
            fld dword ptr [edi + 8]
            fsub dword ptr [esi + 0x1c]
            fstp dword ptr [ecx + 8]
    L0051fb5e:
            cmp word ptr [esp + 0x34], 0
            je L0051fb94
            xor eax, eax
            xor ebx, ebx
            mov al, byte ptr [edi + 0x15]
            mov bl, byte ptr [edi + 0x16]
            mov ah, byte ptr [edi + 0x14]
            shl eax, 8
            or eax, ebx
            xor ebx, ebx
            mov bl, byte ptr [edi + 0x17]
            shl eax, 8
            or eax, ebx
            mov ebx, dword ptr [esp + 0x10]
            mov ebx, dword ptr [ebx + 0x18]
            mov ebx, dword ptr [ebp + ebx + 0x14]
            mov dword ptr [ebx + edx*4], eax
            mov ebx, dword ptr [esp + 0x30]
    L0051fb94:
            cmp word ptr [esp + 0x2c], 0
            je L0051fba8
            mov eax, dword ptr [edi + 0xc]
            mov dword ptr [ecx + 0xc], eax
            mov eax, dword ptr [edi + 0x10]
            mov dword ptr [ecx + 0x10], eax
    L0051fba8:
            cmp word ptr [esp + 0x28], 0
            je L0051fbc8
            test byte ptr [esi + 0x21], 2
            je L0051fbc8
            fld dword ptr [edi + 0x1c]
            fstp dword ptr [ecx + 0x14]
            fld dword ptr [edi + 0x20]
            fstp dword ptr [ecx + 0x18]
            fld dword ptr [edi + 0x24]
            fstp dword ptr [ecx + 0x1c]
    L0051fbc8:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x10]
            add eax, 0x20
            inc edx
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [ecx + 0x18]
            xor ecx, ecx
            mov cx, word ptr [ebp + eax + 0x1e]
            cmp edx, ecx
            jl L0051fb23
            mov edi, dword ptr [esp + 0x10]
            mov ebx, dword ptr [esp + 0x58]
    L0051fbf2:
            mov eax, dword ptr [esp + 0x54]
            xor edx, edx
            mov dx, word ptr [edi + 8]
            inc eax
            add ebp, 0x24
            cmp eax, edx
            mov dword ptr [esp + 0x54], eax
            jl L0051fae7
    L0051fc0c:
            mov eax, dword ptr [esi + 0xc]
            xor edx, edx
            cmp eax, edx
            je L0051fcaf
            and ebx, 0x20
            cmp bx, dx
            mov dword ptr [esp + 0x34], ebx
            je L0051fcab
            cmp word ptr [edi + 8], dx
            mov dword ptr [esp + 0x54], edx
            jbe L0051fcab
    L0051fc33:
            mov ecx, dword ptr [edi + 0x18]
            xor ebp, ebp
            cmp word ptr [edx + ecx + 0x1c], bp
            jbe L0051fc91
    L0051fc3f:
            mov edi, dword ptr [edx + ecx + 0xc]
            xor eax, eax
            test bx, bx
            mov ax, word ptr [edi + ebp*2]
            mov edi, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            lea edi, [edi + eax*8]
            je L0051fc7e
            xor eax, eax
            xor ebx, ebx
            mov al, byte ptr [edi + 0xd]
            mov bl, byte ptr [edi + 0xe]
            mov ah, byte ptr [edi + 0xc]
            mov ecx, dword ptr [edx + ecx + 8]
            shl eax, 8
            or eax, ebx
            xor ebx, ebx
            mov bl, byte ptr [edi + 0xf]
            shl eax, 8
            or eax, ebx
            mov ebx, dword ptr [esp + 0x34]
            mov dword ptr [ecx + ebp*4], eax
    L0051fc7e:
            mov eax, dword ptr [esp + 0x10]
            inc ebp
            mov ecx, dword ptr [eax + 0x18]
            xor eax, eax
            mov ax, word ptr [edx + ecx + 0x1c]
            cmp ebp, eax
            jl L0051fc3f
    L0051fc91:
            mov edi, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x54]
            xor ecx, ecx
            inc eax
            mov cx, word ptr [edi + 8]
            add edx, 0x24
            cmp eax, ecx
            mov dword ptr [esp + 0x54], eax
            jl L0051fc33
    L0051fcab:
            mov ebx, dword ptr [esp + 0x58]
    L0051fcaf:
            test bl, 1
            je L0052045c
            test byte ptr [esi + 0x21], 2
            jne L0051fcc4
            push edi
            call RegenerateVertexNormals
    L0051fcc4:
            push edi
            call RegenerateFaceNormals
            jmp L0052045c
    L0051fccf:
            mov edi, dword ptr [esi + 0xc]
            cmp edi, ebp
            je L00520550
            mov ebx, dword ptr [esi + 8]
            cmp ebx, ebp
            je L00520550
            xor ebp, ebp
            cmp word ptr [esi + 0x12], bp
            jbe L0051fde8
    L0051fcf1:
            xor eax, eax
            mov ax, word ptr [edi + 4]
            lea edx, [eax + eax*4]
            lea eax, [ebx + edx*8]
            push eax
            xor eax, eax
            mov ax, word ptr [edi + 2]
            lea ecx, [eax + eax*4]
            xor eax, eax
            mov ax, word ptr [edi]
            lea edx, [ebx + ecx*8]
            lea eax, [eax + eax*4]
            push edx
            lea edx, [esp + 0x48]
            lea ecx, [ebx + eax*8]
            push ecx
            push edx
            call BrPlaneEquation
            fld dword ptr [esp + 0x50]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x3e0]
            add esp, 0x10
            fnstsw ax
            test ah, 0x41
            je L0051fd3d
            fld dword ptr [IID_IDirectInputDevice2A+0x3e4]
            jmp L0051fd5a
    L0051fd3d:
            fld dword ptr [esp + 0x40]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x3e8]
            fnstsw ax
            test ah, 1
            jne L0051fd56
            fld dword ptr [IID_IDirectInputDevice2A+0x3e8]
            jmp L0051fd5a
    L0051fd56:
            fld dword ptr [esp + 0x40]
    L0051fd5a:
            fstp dword ptr [edi + 0x18]
            fld dword ptr [esp + 0x44]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x3e0]
            fnstsw ax
            test ah, 0x41
            je L0051fd76
            fld dword ptr [IID_IDirectInputDevice2A+0x3e4]
            jmp L0051fd93
    L0051fd76:
            fld dword ptr [esp + 0x44]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x3e8]
            fnstsw ax
            test ah, 1
            jne L0051fd8f
            fld dword ptr [IID_IDirectInputDevice2A+0x3e8]
            jmp L0051fd93
    L0051fd8f:
            fld dword ptr [esp + 0x44]
    L0051fd93:
            fstp dword ptr [edi + 0x1c]
            fld dword ptr [esp + 0x48]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x3e0]
            fnstsw ax
            test ah, 0x41
            je L0051fdaf
            fld dword ptr [IID_IDirectInputDevice2A+0x3e4]
            jmp L0051fdcc
    L0051fdaf:
            fld dword ptr [esp + 0x48]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x3e8]
            fnstsw ax
            test ah, 1
            jne L0051fdc8
            fld dword ptr [IID_IDirectInputDevice2A+0x3e8]
            jmp L0051fdcc
    L0051fdc8:
            fld dword ptr [esp + 0x48]
    L0051fdcc:
            fstp dword ptr [edi + 0x20]
            mov eax, dword ptr [esp + 0x4c]
            xor ecx, ecx
            mov dword ptr [edi + 0x24], eax
            mov cx, word ptr [esi + 0x12]
            inc ebp
            add edi, 0x28
            cmp ebp, ecx
            jl L0051fcf1
    L0051fde8:
            mov ebx, dword ptr [esi + 0xc]
            and word ptr [esi + 0x20], 0x7fff
            xor ebp, ebp
            mov dword ptr [esp + 0x58], ebx
            cmp word ptr [esi + 0x12], bp
            jbe L0051fe41
    L0051fdfd:
            mov eax, dword ptr [ebx + 8]
            test eax, eax
            jne L0051fe08
            or byte ptr [esi + 0x21], 0x80
    L0051fe08:
            xor edi, edi
    L0051fe0a:
            mov dx, word ptr [ebx]
            cmp dx, word ptr [esi + 0x10]
            jb L0051fe22
            push edi
            push ebp
            push offset gZlib_filesystem+0x158
            call BrFailure
            add esp, 0xc
    L0051fe22:
            inc edi
            add ebx, 2
            cmp edi, 3
            jl L0051fe0a
            mov ebx, dword ptr [esp + 0x58]
            xor eax, eax
            mov ax, word ptr [esi + 0x12]
            inc ebp
            add ebx, 0x28
            cmp ebp, eax
            mov dword ptr [esp + 0x58], ebx
            jl L0051fdfd
    L0051fe41:
            mov ax, word ptr [esi + 0x20]
            mov dword ptr [esp + 0x20], 0x5209f0
            test al, 1
            jne L0051fe59
            mov dword ptr [esp + 0x20], 0x5208e0
    L0051fe59:
            test ah, 1
            je L0051fe82
            xor ecx, ecx
            mov dword ptr [esp + 0x18], 0x520b50
            mov cx, word ptr [esi + 0x2c]
            mov dword ptr [esp + 0x58], ecx
            fild dword ptr [esp + 0x58]
            fmul qword ptr [IID_IDirectInputDevice2A+0x400]
            fcos
            fstp dword ptr [esp + 0x14]
            jmp L0051fe92
    L0051fe82:
            mov dword ptr [esp + 0x14], 0
            mov dword ptr [esp + 0x18], 0x520a90
    L0051fe92:
            xor eax, eax
            mov dword ptr [compareModel], esi
            mov ax, word ptr [esi + 0x12]
            lea ebp, [eax + eax*2]
            mov edi, ebp
            mov dword ptr [esp + 0x30], ebp
            lea edx, [eax + ebp*4]
            add edi, edx
            shl edi, 2
            push edi
            call BrScratchAllocate
            add esp, 4
            mov dword ptr [esp + 0x28], eax
            push edi
            push 0
            push eax
            call BrMemSet
            mov eax, dword ptr [esp + 0x34]
            mov ebx, ebp
            shl ebx, 4
            add ebx, eax
            xor edi, edi
            add esp, 0xc
            cmp word ptr [esi + 0x12], di
            lea ecx, [ebx + ebp*4]
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 0x58], ecx
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [esp + 0x24], ecx
            jbe L0051ff4f
            mov edx, dword ptr [esp + 0x58]
            mov ebp, ebx
            mov dword ptr [esp + 0x54], edx
            jmp L0051fefd
    L0051fef9:
            mov ebp, dword ptr [esp + 0x34]
    L0051fefd:
            mov edx, dword ptr [esp + 0x54]
            mov ebx, 3
            mov dword ptr [edx], ecx
            mov edx, ebp
            add ebp, 0xc
            mov dword ptr [esp + 0x34], ebp
    L0051ff11:
            mov bp, word ptr [ecx]
            mov word ptr [eax + 0xe], di
            mov word ptr [eax + 0xc], bp
            mov dword ptr [edx], eax
            add ecx, 2
            add edx, 4
            add eax, 0x10
            dec ebx
            jne L0051ff11
            mov ecx, dword ptr [esp + 0x54]
            xor edx, edx
            mov dx, word ptr [esi + 0x12]
            add ecx, 4
            mov dword ptr [esp + 0x54], ecx
            mov ecx, dword ptr [esp + 0x24]
            inc edi
            add ecx, 0x28
            cmp edi, edx
            mov dword ptr [esp + 0x24], ecx
            jl L0051fef9
            mov ebx, dword ptr [esp + 0x10]
    L0051ff4f:
            mov ecx, dword ptr [esp + 0x58]
            xor eax, eax
            mov ax, word ptr [esi + 0x12]
            push offset FacesCompare
            push 4
            push eax
            push ecx
            call BrQsort
            mov ebp, dword ptr [esp + 0x40]
            add esp, 0x10
            push offset TVCompare_XYZ
            push 4
            push ebp
            push ebx
            call BrQsort
            add esp, 0x10
            xor edi, edi
            dec ebp
            mov dword ptr [esp + 0x2c], edi
            test ebp, ebp
            mov dword ptr [esp + 0x1c], ebp
            jle L0051ffe9
            mov dword ptr [esp + 0x24], edi
            mov dword ptr [esp + 0x54], 4
            lea ebp, [ebx + 4]
    L0051ff9d:
            lea edx, [ebp - 4]
            push ebp
            push edx
            call TVCompare_XYZ
            add esp, 8
            test eax, eax
            je L0051ffd0
            mov eax, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x14]
            push ebp
            lea ecx, [ebx + eax]
            push ecx
            push edx
            push esi
            call dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x54]
            lea eax, [edi + 1]
            mov dword ptr [esp + 0x2c], eax
            mov dword ptr [esp + 0x24], ecx
    L0051ffd0:
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x1c]
            inc edi
            add edx, 4
            add ebp, 4
            cmp edi, eax
            mov dword ptr [esp + 0x54], edx
            jl L0051ff9d
            mov ebp, eax
    L0051ffe9:
            mov eax, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x58]
            push edx
            mov edx, dword ptr [esp + 0x18]
            lea ecx, [ebx + eax*4]
            push ecx
            push edx
            push esi
            call dword ptr [esp + 0x28]
            mov edi, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x30]
            push edi
            push 4
            push eax
            push ebx
            call BrQsort
            add esp, 0x10
            mov dword ptr [esp + 0x54], 1
            test ebp, ebp
            jle L00520044
            mov dword ptr [esp + 0x30], ebp
    L00520025:
            lea ebp, [ebx + 4]
            push ebp
            push ebx
            call edi
            add esp, 8
            test eax, eax
            je L00520037
            inc dword ptr [esp + 0x54]
    L00520037:
            mov eax, dword ptr [esp + 0x30]
            mov ebx, ebp
            dec eax
            mov dword ptr [esp + 0x30], eax
            jne L00520025
    L00520044:
            xor eax, eax
            mov ebx, 1
            mov ax, word ptr [esi + 0x12]
            mov dword ptr [esp + 0x14], ebx
            cmp eax, ebx
            jle L00520079
            mov ecx, dword ptr [esp + 0x58]
            lea edx, [eax - 1]
            add ecx, 4
    L00520061:
            mov edi, dword ptr [ecx]
            mov ebp, dword ptr [ecx - 4]
            mov edi, dword ptr [edi + 8]
            cmp edi, dword ptr [ebp + 8]
            je L0052006f
            inc ebx
    L0052006f:
            add ecx, 4
            dec edx
            jne L00520061
            mov dword ptr [esp + 0x14], ebx
    L00520079:
            mov ecx, dword ptr [esp + 0x54]
            mov edx, eax
            lea ebx, [ebx + ebx*8]
            lea ebp, [eax*4 + 0x1f]
            shl edx, 3
            sub edx, eax
            lea ebx, [ebx*4 + 0x1f]
            mov dword ptr [esp + 0x30], ebx
            mov ebx, ecx
            lea edi, [edx*4 + 0x1f]
            lea edx, [ecx*4 + 0x1f]
            shl ebx, 5
            mov dword ptr [esp + 0x18], edx
            add ebx, 0x1f
            add ecx, eax
            and edx, 0xffffffe0
            mov dword ptr [esp + 0x2c], ebx
            mov eax, edi
            lea ebx, [edx + ecx*2 + 0x20]
            mov ecx, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x2c]
            and al, 0xe0
            add ebx, eax
            mov eax, dword ptr [esi + 0x4c]
            and ecx, 0xffffffe0
            mov dword ptr [esp + 0x38], ebp
            add ebx, ecx
            and ebp, 0xffffffe0
            add ebx, ebp
            and edx, 0xffffffe0
            add ebx, edx
            test eax, eax
            je L005200fc
            cmp ebx, dword ptr [eax]
            jbe L005200fc
            push eax
            call BrResFree
            add esp, 4
            mov dword ptr [esi + 0x4c], 0
    L005200fc:
            mov eax, dword ptr [esi + 0x4c]
            test eax, eax
            jne L00520114
            push 0x2a
            push ebx
            push esi
            call BrResAllocate
            mov dword ptr [esi + 0x4c], eax
            add esp, 0xc
            mov dword ptr [eax], ebx
    L00520114:
            mov ebp, dword ptr [esi + 0x4c]
            mov ecx, dword ptr [esp + 0x30]
            mov eax, ebp
            add ebp, 0x20
            and ecx, 0xffffffe0
            mov edx, ebp
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x2c]
            and edi, 0xffffffe0
            mov dword ptr [esp + 0x3c], ebp
            add ebp, edi
            and ecx, 0xffffffe0
            mov dword ptr [esp + 0x24], ebp
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x30], ebp
            and ecx, 0xffffffe0
            mov dword ptr [eax + 0x18], edx
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x38]
            and ecx, 0xffffffe0
            mov dword ptr [esp + 0x2c], ebp
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x54]
            xor ebx, ebx
            mov dword ptr [esp + 0x34], edx
            lea ecx, [ebp + ecx*2]
            mov dword ptr [esp + 0x38], ecx
            mov cx, word ptr [esp + 0x14]
            mov word ptr [eax + 8], cx
            and word ptr [esi + 0x20], 0xefff
            cmp word ptr [esi + 0x12], bx
            jbe L00520250
            lea edi, [edx + 0x1c]
    L00520188:
            mov eax, dword ptr [esp + 0x3c]
            mov edx, ebx
            shl edx, 3
            sub edx, ebx
            lea ecx, [eax + edx*4]
            mov edx, dword ptr [esp + 0x2c]
            mov dword ptr [edi - 0x18], ecx
            mov ecx, dword ptr [esp + 0x38]
            lea eax, [edx + ebx*4]
            mov dword ptr [edi - 0x14], eax
            mov eax, dword ptr [esp + 0x58]
            lea edx, [ecx + ebx*2]
            lea eax, [eax + ebx*4]
            mov dword ptr [edi - 0x10], edx
            mov dword ptr [esp + 0x54], eax
            mov ecx, dword ptr [eax]
            mov eax, dword ptr [ecx + 8]
            test eax, eax
            je L005201e3
            mov edx, dword ptr [eax + 0x98]
            mov eax, dword ptr [esp + 0x54]
            mov dword ptr [edi - 0x1c], edx
            mov ecx, dword ptr [eax]
            mov edx, dword ptr [ecx + 8]
            push edx
            call IsMaterialTransparent
            test eax, eax
            je L005201ea
            or byte ptr [esi + 0x21], 0x10
            jmp L005201ea
    L005201e3:
            mov dword ptr [edi - 0x1c], 0
    L005201ea:
            xor eax, eax
            mov word ptr [edi], 0
            mov ax, word ptr [esi + 0x12]
            cmp ebx, eax
            jge L0052023f
            mov ecx, dword ptr [esp + 0x54]
            mov dword ptr [esp + 0x18], ecx
    L00520201:
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x54]
            mov eax, dword ptr [edx]
            mov edx, dword ptr [ecx]
            mov ecx, dword ptr [eax + 8]
            cmp ecx, dword ptr [edx + 8]
            jne L0052023f
            xor edx, edx
            push esi
            mov dx, word ptr [edi]
            push eax
            lea eax, [edi - 0x1c]
            push edx
            push eax
            call CopyFace
            inc word ptr [edi]
            mov edx, dword ptr [esp + 0x18]
            xor eax, eax
            inc ebx
            mov ax, word ptr [esi + 0x12]
            add edx, 4
            cmp ebx, eax
            mov dword ptr [esp + 0x18], edx
            jl L00520201
    L0052023f:
            xor ecx, ecx
            add edi, 0x24
            mov cx, word ptr [esi + 0x12]
            cmp ebx, ecx
            jl L00520188
    L00520250:
            mov eax, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x24]
            mov ecx, dword ptr [esp + 0x30]
            push esi
            mov dword ptr [eax + 0x10], edx
            mov edx, dword ptr [esp + 0x14]
            lea edi, [eax + 0x1e]
            mov dword ptr [eax + 0x14], ecx
            mov dword ptr [eax + 0x18], ebp
            xor ebx, ebx
            mov word ptr [edi], 1
            mov ecx, dword ptr [edx]
            push ecx
            push ebx
            push eax
            call CopyVertex
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x2c], ebx
            cmp eax, ebx
            mov dword ptr [esp + 0x58], ebx
            jle L00520377
            mov edx, dword ptr [esp + 0x30]
            mov ecx, dword ptr [esp + 0x24]
            mov ebx, dword ptr [esp + 0x10]
            add ebp, 2
            add edx, 4
            add ecx, 0x20
            mov dword ptr [esp + 0x18], ebp
            mov dword ptr [esp + 0x1c], edx
            mov dword ptr [esp + 0x54], ecx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x2c], eax
    L005202bb:
            mov edx, dword ptr [ebx]
            xor eax, eax
            lea ebp, [ebx + 4]
            mov ax, word ptr [edx + 0xe]
            mov edx, dword ptr [ebp]
            lea ecx, [eax + eax*4]
            xor eax, eax
            mov ax, word ptr [edx + 0xe]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [esi + 0xc]
            mov ecx, dword ptr [eax + ecx*8 + 8]
            cmp ecx, dword ptr [eax + edx*8 + 8]
            je L005202ff
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x18]
            add edi, 0x24
            mov dword ptr [edi - 0xe], edx
            mov dword ptr [edi - 0xa], eax
            mov dword ptr [edi - 6], ecx
            mov word ptr [edi], 0
    L005202ff:
            mov edx, dword ptr [esp + 0x58]
            push ebp
            push ebx
            mov dword ptr [esp + 0x44], edx
            call dword ptr [esp + 0x28]
            add esp, 8
            test eax, eax
            je L0052035b
            mov edx, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x58]
            add edx, 0x20
            add ecx, 4
            mov dword ptr [esp + 0x54], edx
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x1c], ecx
            mov ecx, dword ptr [ebx]
            inc eax
            add edx, 2
            mov dword ptr [esp + 0x58], eax
            mov word ptr [ecx + 0xc], ax
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [ebp]
            xor eax, eax
            mov ax, word ptr [edi]
            push esi
            push edx
            lea ecx, [edi - 0x1e]
            push eax
            push ecx
            call CopyVertex
            inc word ptr [edi]
    L0052035b:
            mov edx, dword ptr [ebx]
            mov ax, word ptr [esp + 0x3c]
            mov ebx, ebp
            mov word ptr [edx + 0xc], ax
            mov eax, dword ptr [esp + 0x30]
            dec eax
            mov dword ptr [esp + 0x30], eax
            jne L005202bb
    L00520377:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x2c]
            mov eax, dword ptr [ecx + edx*4]
            mov cx, word ptr [esp + 0x58]
            mov word ptr [eax + 0xc], cx
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            jle L00520422
            mov edx, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x54], eax
            lea ecx, [edx + 0xc]
    L005203a2:
            xor ebx, ebx
            cmp word ptr [ecx + 0x10], bx
            mov dword ptr [esp + 0x58], ebx
            jbe L00520414
            xor edi, edi
    L005203b0:
            mov eax, dword ptr [ecx + 4]
            mov edx, dword ptr [esp + 0x24]
            mov ebp, dword ptr [ecx]
            sub eax, edx
            xor edx, edx
            add edi, 0x1c
            mov dx, word ptr [ebp + ebx*2]
            mov ebx, dword ptr [esp + 0x28]
            mov ebp, dword ptr [ecx - 8]
            lea edx, [edx + edx*2]
            shl edx, 4
            add edx, ebx
            sar eax, 5
            mov bx, word ptr [edx + 0xc]
            sub bx, ax
            mov word ptr [edi + ebp - 0x1c], bx
            mov bx, word ptr [edx + 0x1c]
            mov ebp, dword ptr [ecx - 8]
            sub bx, ax
            mov word ptr [edi + ebp - 0x1a], bx
            mov dx, word ptr [edx + 0x2c]
            mov ebx, dword ptr [esp + 0x58]
            sub dx, ax
            mov eax, dword ptr [ecx - 8]
            inc ebx
            mov dword ptr [esp + 0x58], ebx
            mov word ptr [edi + eax - 0x18], dx
            xor edx, edx
            mov dx, word ptr [ecx + 0x10]
            cmp ebx, edx
            jl L005203b0
    L00520414:
            mov eax, dword ptr [esp + 0x54]
            add ecx, 0x24
            dec eax
            mov dword ptr [esp + 0x54], eax
            jne L005203a2
    L00520422:
            mov eax, dword ptr [esp + 0x28]
            push eax
            call BrScratchFree
            mov edi, dword ptr [esi + 0x4c]
            add esp, 4
            test edi, edi
            je L0052045c
            xor ebx, ebx
            cmp word ptr [edi + 8], bx
            jbe L0052045c
            xor ebp, ebp
    L00520440:
            mov edx, dword ptr [edi + 0x18]
            mov ecx, ebp
            add ecx, edx
            push esi
            push ecx
            call prepareEdges
            xor edx, edx
            inc ebx
            mov dx, word ptr [edi + 8]
            add ebp, 0x24
            cmp ebx, edx
            jl L00520440
    L0052045c:
            test byte ptr [esi + 0x20], 0x80
            jne L00520494
            mov eax, dword ptr [esi + 0xc]
            xor edi, edi
            cmp eax, edi
            je L00520474
            push eax
            call BrResFree
            add esp, 4
    L00520474:
            mov eax, dword ptr [esi + 8]
            cmp eax, edi
            je L00520484
            push eax
            call BrResFree
            add esp, 4
    L00520484:
            mov dword ptr [esi + 0xc], edi
            mov dword ptr [esi + 8], edi
            mov word ptr [esi + 0x12], di
            mov word ptr [esi + 0x10], di
            jmp L00520496
    L00520494:
            xor edi, edi
    L00520496:
            cmp dword ptr [v1db+0xc], edi
            je L00520550
            mov ecx, dword ptr [v1db+0x14]
            cmp ecx, edi
            je L00520550
            mov ax, word ptr [esi + 0x20]
            test al, 0x80
            jne L00520550
            test ah, 8
            je L00520550
            mov eax, dword ptr [esi + 0x50]
            mov dword ptr [esp + 0x40], 0x6e
            cmp eax, edi
            mov dword ptr [esp + 0x44], 1
            mov dword ptr [esp + 0x48], edi
            mov dword ptr [esp + 0x4c], edi
            je L005204f6
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x10]
            mov dword ptr [esi + 0x50], edi
            mov ecx, dword ptr [v1db+0x14]
            add esp, 4
    L005204f6:
            lea eax, [esp + 0x40]
            mov edx, dword ptr [ecx]
            push eax
            mov eax, dword ptr [esi + 0x4c]
            push 0x10b
            push eax
            lea eax, [esp + 0x64]
            push eax
            mov eax, dword ptr [v1db+0xc]
            push eax
            push ecx
            call dword ptr [edx + 0x54]
            add esp, 0x18
            cmp eax, edi
            jne L00520550
            mov eax, dword ptr [esp + 0x58]
            cmp eax, edi
            je L00520550
            lea edx, [esp + 0x54]
            mov dword ptr [esi + 0x50], eax
            mov ecx, dword ptr [eax]
            push 0x6d
            push edx
            push eax
            call dword ptr [ecx + 0x2c]
            add esp, 0xc
            cmp eax, edi
            jne L00520550
            cmp dword ptr [esp + 0x54], edi
            jne L00520550
            mov eax, dword ptr [esi + 0x4c]
            push eax
            call BrResFree
            add esp, 4
            mov dword ptr [esi + 0x4c], edi
    L00520550:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x40
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00520e30
void C2_HOOK_STDCALL BrModelClear(br_model* model) {

    // remove prepared mesh
    if (model->prepared != NULL) {
        BrResFree(model->prepared);
        model->prepared = NULL;
    }
    if (model->stored != NULL) {
        ((br_object*)model->stored)->dispatch->_free((br_object*)model->stored);
        model->stored = NULL;
    }
}
