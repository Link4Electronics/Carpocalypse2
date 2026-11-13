#include "v1model.h"

#include "convert.h"
#include "faceops.h"
#include "genrend.h"
#include "heap.h"
#include "rndstate.h"
#include "setup.h"

#include "core/fw/scratch.h"

#include "c2_string.h"

#ifdef CARPOCALYPSE2_MATCHING
extern br_error C2_HOOK_STDCALL CacheUpdate(br_soft_renderer* self);
extern br_error C2_HOOK_STDCALL CheckPrimitiveState(br_soft_renderer* self);
extern void C2_HOOK_STDCALL GeometryFunctionAdd(br_soft_renderer* renderer, geometry_fn* fn);
extern void C2_HOOK_STDCALL GeometryFunctionBothAdd(br_soft_renderer* renderer, geometry_fn* fn);
extern void C2_HOOK_STDCALL GeometryFunctionOnScreenAdd(br_soft_renderer* renderer, geometry_fn* fn);
extern void C2_HOOK_STDCALL GeometryFunctionReset(br_soft_renderer* renderer);
extern void C2_HOOK_CDECL OpHeapAddLine();
extern void C2_HOOK_CDECL OpHeapAddLineConvert();
extern void C2_HOOK_CDECL OpHeapAddPoint();
extern void C2_HOOK_CDECL OpHeapAddPointConvert();
extern void C2_HOOK_CDECL OpHeapAddTriangle();
extern void C2_HOOK_CDECL OpHeapAddTriangleConvert();
extern void C2_HOOK_CDECL OpLineClip();
extern void C2_HOOK_CDECL OpTriangleClip();
extern void C2_HOOK_CDECL OpTriangleClipConstantSurf();
extern void C2_HOOK_CDECL OpTriangleConstantSurf();
extern void C2_HOOK_CDECL OpTriangleMappingWrapFix();
extern void C2_HOOK_CDECL OpTriangleRelightTwoSided();
extern void C2_HOOK_CDECL OpTriangleReplicateConstant();
extern void C2_HOOK_CDECL OpTriangleReplicateConstantI();
extern void C2_HOOK_CDECL OpTriangleReplicateConstantRGB();
extern void C2_HOOK_CDECL OpTriangleSubdivide();
extern void C2_HOOK_CDECL OpTriangleSubdivideOnScreen();
extern void C2_HOOK_CDECL OpTriangleToLines();
extern void C2_HOOK_CDECL OpTriangleToPoints();
extern void C2_HOOK_CDECL OpTriangleToPoints_OS();
extern void C2_HOOK_CDECL OpTriangleTwoSidedConstantSurf();
extern void C2_HOOK_STDCALL PrimBlockAdd(br_soft_renderer* renderer, brp_render_fn* fn);
extern void C2_HOOK_STDCALL PrimBlockAddBoth(br_soft_renderer* renderer, brp_render_fn* fn);
extern void C2_HOOK_STDCALL PrimBlockOnScreenAdd(br_soft_renderer* renderer, brp_render_fn* fn);
extern void C2_HOOK_STDCALL PrimBlockReset(br_soft_renderer* renderer);
extern void C2_HOOK_CDECL RenderConvert1();
extern void C2_HOOK_CDECL RenderConvert2();
extern void C2_HOOK_CDECL RenderConvert3();
extern void C2_HOOK_CDECL ScratchFree();
extern void C2_HOOK_STDCALL StaticCacheUpdate_PerModel(br_soft_renderer *self);
extern void C2_HOOK_STDCALL StaticCacheUpdate_PerScene(br_soft_renderer* self);
extern void C2_HOOK_STDCALL SubdivideSetThreshold(br_int_32 subdivide_tolerance);
extern void C2_HOOK_STDCALL VertexGeometryFns(br_soft_renderer* renderer, geometry_fn* prim_outcode);
extern void C2_HOOK_CDECL Vertex_ClearFlags();
extern void __cdecl _ftol();
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
extern static_cache_soft scache;
#endif
// FUNCTION: CARMA2_HW 0x00542f90
void C2_HOOK_CDECL V1Faces_ScratchAllocate(br_geometry* self, br_soft_renderer* renderer) {
    char *sp;
    br_size_t scratch_size;

    C2_HOOK_BUG_ON(sizeof(*rend.vertex_counts) != 1);
    C2_HOOK_BUG_ON(sizeof(*rend.temp_faces) != 4);
    C2_HOOK_BUG_ON(sizeof(*rend.temp_vertices) != 0x40);

    scratch_size  = SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_counts));
    scratch_size += SCRATCH_ALIGN(rend.nfaces    * sizeof(*rend.temp_faces));
    scratch_size += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.temp_vertices));

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, type, 0x10);
    C2_HOOK_BUG_ON(sizeof(*rend.edge_flags) != 1);
    if (rend.block->type == BRT_LINE) {
        scratch_size += SCRATCH_ALIGN(rend.nedges * sizeof(*rend.edge_flags));
    }

    C2_HOOK_BUG_ON(sizeof(*rend.vertex_flags) != 1);
    if (rend.block->type == BRT_POINT) {
        scratch_size += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_flags));
    }

    C2_HOOK_BUG_ON(sizeof(*rend.vertex_heap_pointers) != 4);
    if (renderer->state.hidden.type == BRT_BUCKET_SORT || renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER) {
        scratch_size += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_heap_pointers));
    }

    rend.scratch = BrScratchAllocate(scratch_size + SCRATCH_BOUNDARY);

    sp = rend.scratch;

    rend.vertex_counts = (void*)sp;
    sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_counts));

    rend.temp_faces = (void*)sp;
    sp += SCRATCH_ALIGN(rend.nfaces * sizeof(*rend.temp_faces));

    rend.temp_vertices = (void*)sp;
    sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.temp_vertices));

    if (rend.block->type == BRT_LINE) {
        rend.edge_flags = (void*)sp;
        sp += SCRATCH_ALIGN(rend.nedges * sizeof(*rend.edge_flags));
        memset(rend.edge_flags, 0, rend.nedges * sizeof(*rend.edge_flags));
        rend.edge_flags[0] = 1;
    }

    if(rend.block->type == BRT_POINT) {
        rend.vertex_flags = (void*)sp;
        sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_flags));
        memset(rend.vertex_flags, 0, rend.nvertices * sizeof(*rend.vertex_flags));
    }

    if (renderer->state.hidden.type == BRT_BUCKET_SORT || renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER) {
        rend.vertex_heap_pointers = (void*)sp;
        sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_heap_pointers));
        memset(rend.vertex_heap_pointers, 0, rend.nvertices * sizeof(*rend.vertex_heap_pointers));
    }

    memset(rend.vertex_counts, 0, rend.nvertices * sizeof(*rend.vertex_counts));
}

// FUNCTION: CARMA2_HW 0x00543110
void C2_HOOK_CDECL V1Face_CullNone(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    C2_HOOK_BUG_ON(sizeof(temp_face_soft) != 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(temp_face_soft, flag, 0x2);
    C2_HOOK_BUG_ON(TFF_VISIBLE != 0x4);
    C2_HOOK_BUG_ON(sizeof(v11face) != 0x1c);

    for (f = 0, tfp = rend.temp_faces, fp = rend.faces; f < rend.nfaces; f++, tfp++, fp++) {
        tfp->flag = TFF_VISIBLE;

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;
    }
    rend.nvisible_faces = rend.nfaces;
}

// FUNCTION: CARMA2_HW 0x00543190
void C2_NAKED C2_HOOK_CDECL V1Face_OS_CullNone(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    temp_face_soft* tfp;

    for (f = 0; f < rend.nfaces; f++) {

        tfp = &rend.temp_faces[f];
        tfp->flag = TFF_VISIBLE;
    }

    memset(rend.vertex_counts, 1, rend.nvertices);

    rend.nvisible_faces = rend.nfaces;
#else
    __asm {
            mov edx, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x4]
            xor eax, eax
            push edi
            test edx, edx
            jle L005431b7
            add ecx, 2
    L005431a6:
            mov byte ptr [ecx], 4
            mov edx, dword ptr [rend+0x34]
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L005431a6
    L005431b7:
            mov ecx, dword ptr [rend+0x38]
            mov edi, dword ptr [rend+0x8]
            mov edx, ecx
            mov eax, 0x1010101
            shr ecx, 2
            rep stosd
            mov ecx, edx
            and ecx, 3
            rep stosb
            mov eax, dword ptr [rend+0x34]
            pop edi
            mov dword ptr [rend+0x1c], eax
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005431f0
void C2_HOOK_CDECL V1Face_CullOneSided(br_geometry* self, br_soft_renderer* renderer) {

    switch (renderer->state.matrix.view_to_screen_hint) {
    case BRT_PERSPECTIVE:
        V1Face_CullOneSidedPerspective(self, renderer);
        break;
    case BRT_PARALLEL:
        V1Face_CullOneSidedParallel(self, renderer);
        break;
    default:
        V1Face_CullNone(self, renderer);
        break;
    }
}

// FUNCTION: CARMA2_HW 0x005432b0
void C2_NAKED C2_HOOK_CDECL V1Face_CullOneSidedPerspective(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    rend.nvisible_faces = 0;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < fp->eqn.v[3]) {
            tfp->flag = 0;
            continue;
        }

        tfp->flag = TFF_VISIBLE;

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.nvisible_faces++;
    }
#else
    __asm {
            mov edx, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x24]
            mov eax, dword ptr [rend+0x4]
            push esi
            xor esi, esi
            mov dword ptr [rend+0x1c], 0
            test edx, edx
            jle L00543372
            push edi
            push ebx
            lea edx, [eax + 2]
    L005432db:
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fxch st(2)
            fmul dword ptr [scache+0xf4]
            fxch st(1)
            fmul dword ptr [scache+0xf8]
            fxch st(2)
            fmul dword ptr [scache+0xfc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [ecx + 0x18]
            fnstsw ax
            test ah, 1
            je L00543313
            mov byte ptr [edx], 0
            jmp L0054335c
    L00543313:
            xor eax, eax
            mov byte ptr [edx], 4
            mov ax, word ptr [ecx]
            mov edi, dword ptr [rend+0x8]
            add eax, edi
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov edi, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx + 2]
            add eax, edi
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov edi, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx + 4]
            add eax, edi
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov eax, dword ptr [rend+0x1c]
            inc eax
            mov dword ptr [rend+0x1c], eax
    L0054335c:
            mov eax, dword ptr [rend+0x34]
            inc esi
            add ecx, 0x1c
            add edx, 4
            cmp esi, eax
            jl L005432db
            pop ebx
            pop edi
    L00543372:
            pop esi
            ret
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

// FUNCTION: CARMA2_HW 0x00543380
void C2_NAKED C2_HOOK_CDECL V1Face_CullOneSidedParallel(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    rend.nvisible_faces = 0;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < 0.f) {
            tfp->flag = 0;
            continue;
        }

        tfp->flag = TFF_VISIBLE;

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.nvisible_faces++;
    }
#else
    __asm {
            mov edx, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x24]
            mov eax, dword ptr [rend+0x4]
            push esi
            xor esi, esi
            mov dword ptr [rend+0x1c], 0
            test edx, edx
            jle L00543445
            push edi
            push ebx
            lea edx, [eax + 2]
    L005433ab:
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0xc]
            fxch st(2)
            fmul dword ptr [scache+0xfc]
            fxch st(1)
            fmul dword ptr [scache+0xf8]
            fxch st(2)
            fmul dword ptr [scache+0xf4]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [rendererStateStoredDispatch+0x48]
            fnstsw ax
            test ah, 1
            je L005433e6
            mov byte ptr [edx], 0
            jmp L0054342f
    L005433e6:
            xor eax, eax
            mov byte ptr [edx], 4
            mov ax, word ptr [ecx]
            mov edi, dword ptr [rend+0x8]
            add eax, edi
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov edi, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx + 2]
            add eax, edi
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov edi, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx + 4]
            add eax, edi
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov eax, dword ptr [rend+0x1c]
            inc eax
            mov dword ptr [rend+0x1c], eax
    L0054342f:
            mov eax, dword ptr [rend+0x34]
            inc esi
            add ecx, 0x1c
            add edx, 4
            cmp esi, eax
            jl L005433ab
            pop ebx
            pop edi
    L00543445:
            pop esi
            ret
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

// FUNCTION: CARMA2_HW 0x00543450
void C2_NAKED C2_HOOK_CDECL V1Face_OS_CullOneSided(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (renderer->state.matrix.view_to_screen_hint) {
    case BRT_PERSPECTIVE:
        V1Face_CullOneSidedPerspective(self, renderer);
        break;
    case BRT_PARALLEL:
        V1Face_CullOneSidedParallel(self, renderer);
        break;
    default:
        V1Face_OS_CullNone(self, renderer);
        break;
    }
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            push edi
            mov eax, dword ptr [ecx + 0x550]
            sub eax, 0xf1
            je L005434c6
            dec eax
            je L005434b6
            mov edx, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x4]
            xor eax, eax
            test edx, edx
            jle L0054348b
            add ecx, 2
    L0054347a:
            mov byte ptr [ecx], 4
            mov edx, dword ptr [rend+0x34]
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L0054347a
    L0054348b:
            mov ecx, dword ptr [rend+0x38]
            mov edi, dword ptr [rend+0x8]
            mov edx, ecx
            mov eax, 0x1010101
            shr ecx, 2
            rep stosd
            mov ecx, edx
            and ecx, 3
            rep stosb
            mov eax, dword ptr [rend+0x34]
            mov dword ptr [rend+0x1c], eax
            pop edi
            ret
    L005434b6:
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            push ecx
            call V1Face_CullOneSidedParallel
            add esp, 8
            pop edi
            ret
    L005434c6:
            mov edx, dword ptr [esp + 8]
            push ecx
            push edx
            call V1Face_CullOneSidedPerspective
            add esp, 8
            pop edi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005434e0
void C2_HOOK_CDECL V1Face_CullTwoSided(br_geometry* self, br_soft_renderer* renderer) {

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.matrix.view_to_screen_hint, 0x550);
#endif
    switch (renderer->state.matrix.view_to_screen_hint) {
        case BRT_PERSPECTIVE:
            V1Face_CullTwoSidedPerspective(self, renderer);
            break;

        case BRT_PARALLEL:
            V1Face_CullTwoSidedParallel(self, renderer);
            break;

        default:
            V1Face_CullNone(self, renderer);
            break;
    }
}

// FUNCTION: CARMA2_HW 0x005435a0
void C2_NAKED C2_HOOK_CDECL V1Face_CullTwoSidedPerspective(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    int df;
    temp_face_soft* tfp;
    v11face* fp;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        tfp->flag = TFF_VISIBLE;
        df = TVDIR_FRONT;

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < fp->eqn.v[3]) {
            tfp->flag |= TFF_REVERSED;
            df = TVDIR_BACK;
        }

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.temp_vertices[fp->vertices[0]].flags |= df;
        rend.temp_vertices[fp->vertices[1]].flags |= df;
        rend.temp_vertices[fp->vertices[2]].flags |= df;
    }
    rend.nvisible_faces = rend.nfaces;
#else
    __asm {
            mov eax, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x24]
            mov edx, dword ptr [rend+0x4]
            push esi
            xor esi, esi
            test eax, eax
            jle L0054369a
            push edi
            push ebx
            lea edi, [edx + 2]
    L005435c1:
            mov byte ptr [edi], 4
            mov edx, 0x80000000
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x10]
            fxch st(2)
            fmul dword ptr [scache+0xf4]
            fxch st(1)
            fmul dword ptr [scache+0xfc]
            fxch st(2)
            fmul dword ptr [scache+0xf8]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [ecx + 0x18]
            fnstsw ax
            test ah, 1
            je L00543604
            mov byte ptr [edi], 5
            mov edx, 0x40000000
    L00543604:
            mov ebx, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx]
            add ecx, 0x1c
            add eax, ebx
            add edi, 4
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov ebx, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx - 0x1a]
            add eax, ebx
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov ebx, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx - 0x18]
            add eax, ebx
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov ebx, dword ptr [rend+0xc]
            xor eax, eax
            mov ax, word ptr [ecx - 0x1c]
            shl eax, 6
            add eax, ebx
            mov ebx, dword ptr [eax]
            or ebx, edx
            mov dword ptr [eax], ebx
            mov ebx, dword ptr [rend+0xc]
            xor eax, eax
            mov ax, word ptr [ecx - 0x1a]
            shl eax, 6
            add eax, ebx
            mov ebx, dword ptr [eax]
            or ebx, edx
            mov dword ptr [eax], ebx
            mov ebx, dword ptr [rend+0xc]
            xor eax, eax
            mov ax, word ptr [ecx - 0x18]
            shl eax, 6
            add eax, ebx
            mov ebx, dword ptr [eax]
            or ebx, edx
            inc esi
            mov dword ptr [eax], ebx
            mov eax, dword ptr [rend+0x34]
            cmp esi, eax
            jl L005435c1
            pop ebx
            pop edi
    L0054369a:
            mov dword ptr [rend+0x1c], eax
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005436b0
void C2_NAKED C2_HOOK_CDECL V1Face_CullTwoSidedParallel(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    int df;
    temp_face_soft* tfp;
    v11face* fp;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        tfp->flag = TFF_VISIBLE;
        df = TVDIR_FRONT;

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < 0.f) {
            tfp->flag |= TFF_REVERSED;
            df = TVDIR_BACK;
        }

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.temp_vertices[fp->vertices[0]].flags |= df;
        rend.temp_vertices[fp->vertices[1]].flags |= df;
        rend.temp_vertices[fp->vertices[2]].flags |= df;
    }
    rend.nvisible_faces = rend.nfaces;
#else
    __asm {
            mov eax, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x24]
            mov edx, dword ptr [rend+0x4]
            push esi
            xor esi, esi
            test eax, eax
            jle L005437ad
            push edi
            push ebx
            lea edi, [edx + 2]
    L005436d1:
            mov byte ptr [edi], 4
            mov edx, 0x80000000
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fxch st(2)
            fmul dword ptr [scache+0xf4]
            fxch st(1)
            fmul dword ptr [scache+0xf8]
            fxch st(2)
            fmul dword ptr [scache+0xfc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [rendererStateStoredDispatch+0x48]
            fnstsw ax
            test ah, 1
            je L00543717
            mov byte ptr [edi], 5
            mov edx, 0x40000000
    L00543717:
            mov ebx, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx]
            add ecx, 0x1c
            add eax, ebx
            add edi, 4
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov ebx, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx - 0x1a]
            add eax, ebx
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov ebx, dword ptr [rend+0x8]
            xor eax, eax
            mov ax, word ptr [ecx - 0x18]
            add eax, ebx
            mov bl, byte ptr [eax]
            inc bl
            mov byte ptr [eax], bl
            mov ebx, dword ptr [rend+0xc]
            xor eax, eax
            mov ax, word ptr [ecx - 0x1c]
            shl eax, 6
            add eax, ebx
            mov ebx, dword ptr [eax]
            or ebx, edx
            mov dword ptr [eax], ebx
            mov ebx, dword ptr [rend+0xc]
            xor eax, eax
            mov ax, word ptr [ecx - 0x1a]
            shl eax, 6
            add eax, ebx
            mov ebx, dword ptr [eax]
            or ebx, edx
            mov dword ptr [eax], ebx
            mov ebx, dword ptr [rend+0xc]
            xor eax, eax
            mov ax, word ptr [ecx - 0x18]
            shl eax, 6
            add eax, ebx
            mov ebx, dword ptr [eax]
            or ebx, edx
            inc esi
            mov dword ptr [eax], ebx
            mov eax, dword ptr [rend+0x34]
            cmp esi, eax
            jl L005436d1
            pop ebx
            pop edi
    L005437ad:
            mov dword ptr [rend+0x1c], eax
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005437c0
void C2_NAKED C2_HOOK_CDECL V1Face_OS_CullTwoSided(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (renderer->state.matrix.view_to_screen_hint) {
    case BRT_PERSPECTIVE:
        V1Face_CullTwoSidedPerspective(self, renderer);
        break;
    case BRT_PARALLEL:
        V1Face_CullTwoSidedParallel(self, renderer);
        break;
    default:
        V1Face_OS_CullNone(self, renderer);
        break;
    }
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            push edi
            mov eax, dword ptr [ecx + 0x550]
            sub eax, 0xf1
            je L00543836
            dec eax
            je L00543826
            mov edx, dword ptr [rend+0x34]
            mov ecx, dword ptr [rend+0x4]
            xor eax, eax
            test edx, edx
            jle L005437fb
            add ecx, 2
    L005437ea:
            mov byte ptr [ecx], 4
            mov edx, dword ptr [rend+0x34]
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L005437ea
    L005437fb:
            mov ecx, dword ptr [rend+0x38]
            mov edi, dword ptr [rend+0x8]
            mov edx, ecx
            mov eax, 0x1010101
            shr ecx, 2
            rep stosd
            mov ecx, edx
            and ecx, 3
            rep stosb
            mov eax, dword ptr [rend+0x34]
            mov dword ptr [rend+0x1c], eax
            pop edi
            ret
    L00543826:
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            push ecx
            call V1Face_CullTwoSidedParallel
            add esp, 8
            pop edi
            ret
    L00543836:
            mov edx, dword ptr [esp + 8]
            push ecx
            push edx
            call V1Face_CullTwoSidedPerspective
            add esp, 8
            pop edi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00543850
void C2_NAKED C2_HOOK_CDECL V1Face_Outcode(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    br_uint_32 combined_codes;

    C2_HOOK_BUG_ON(OUTCODES_NOT != 0xfff0000);
    C2_HOOK_BUG_ON(OUTCODES_ALL != 0xfff);

    rend.faces_clipped = 0;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (!tfp->flag) {
            continue;
        }

        combined_codes = rend.temp_vertices[fp->vertices[0]].flags |
            rend.temp_vertices[fp->vertices[1]].flags |
            rend.temp_vertices[fp->vertices[2]].flags;

        if ((combined_codes & OUTCODES_NOT) != OUTCODES_NOT) {
            rend.nvisible_faces -= 1;
            tfp->flag = 0;
            rend.vertex_counts[fp->vertices[0]] -= 1;
            rend.vertex_counts[fp->vertices[1]] -= 1;
            rend.vertex_counts[fp->vertices[2]] -= 1;
        } else if (combined_codes & OUTCODES_ALL) {
            tfp->flag |= TFF_CLIPPED;
            tfp->codes = (br_uint_16)(combined_codes & OUTCODES_ALL);
            rend.faces_clipped = 1;
        }
    }
#else
    __asm {
            mov ecx, dword ptr [rend+0x34]
            mov eax, dword ptr [rend+0x24]
            mov edx, dword ptr [rend+0x4]
            push ebx
            push edi
            xor ebx, ebx
            xor edi, edi
            cmp ecx, ebx
            mov dword ptr [rend+0x20], ebx
            jle L00543938
            push esi
            mov esi, dword ptr [rend+0xc]
            push ebp
    L0054387d:
            cmp byte ptr [edx + 2], bl
            je L00543921
            xor ecx, ecx
            xor ebp, ebp
            mov cx, word ptr [eax + 4]
            mov bp, word ptr [eax + 2]
            shl ecx, 6
            shl ebp, 6
            mov ecx, dword ptr [ecx + esi]
            or ecx, dword ptr [ebp + esi]
            xor ebp, ebp
            mov bp, word ptr [eax]
            shl ebp, 6
            or ecx, dword ptr [ebp + esi]
            mov ebp, ecx
            and ebp, 0xfff0000
            cmp ebp, 0xfff0000
            je L005438fc
            mov ebp, dword ptr [rend+0x1c]
            xor ecx, ecx
            dec ebp
            mov dword ptr [rend+0x1c], ebp
            mov byte ptr [edx + 2], bl
            mov cx, word ptr [eax]
            mov esi, dword ptr [rend+0x8]
            add ecx, esi
            dec byte ptr [ecx]
            mov esi, dword ptr [rend+0x8]
            xor ecx, ecx
            mov cx, word ptr [eax + 2]
            add ecx, esi
            dec byte ptr [ecx]
            mov esi, dword ptr [rend+0x8]
            xor ecx, ecx
            mov cx, word ptr [eax + 4]
            add ecx, esi
            dec byte ptr [ecx]
            jmp L0054391b
    L005438fc:
            test ecx, 0xfff
            je L00543921
            or byte ptr [edx + 2], 2
            and ecx, 0xfff
            mov word ptr [edx], cx
            mov dword ptr [rend+0x20], 1
    L0054391b:
            mov esi, dword ptr [rend+0xc]
    L00543921:
            mov ecx, dword ptr [rend+0x34]
            inc edi
            add eax, 0x1c
            add edx, 4
            cmp edi, ecx
            jl L0054387d
            pop ebp
            pop esi
    L00543938:
            pop edi
            pop ebx
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00543940
void C2_NAKED C2_HOOK_CDECL V1Face_Render(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;
    brp_block* clipped = renderer->state.cache.face_blocks[renderer->state.cache.nface_blocks].chain;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (!(tfp->flag & TFF_VISIBLE)) {
            continue;
        }

        if (tfp->flag & TFF_CLIPPED) {
            clipped->render(clipped,
                &rend.temp_vertices[fp->vertices[0]],
                &rend.temp_vertices[fp->vertices[1]],
                &rend.temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        } else {
            unclipped->render(unclipped,
                &rend.temp_vertices[fp->vertices[0]],
                &rend.temp_vertices[fp->vertices[1]],
                &rend.temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        }
    }
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            push ebx
            push ebp
            push esi
            movsx ecx, byte ptr [eax + 0x71d]
            movsx edx, byte ptr [eax + 0x71c]
            mov ebp, dword ptr [eax + ecx*8 + 0x6e8]
            mov esi, dword ptr [rend+0x24]
            mov ecx, dword ptr [eax + edx*8 + 0x6b0]
            mov eax, dword ptr [rend+0x34]
            xor ebx, ebx
            push edi
            mov edi, dword ptr [rend+0x4]
            mov dword ptr [esp + 0x18], ecx
            test eax, eax
            jle L00543a0a
            jmp L00543989
    L00543985:
            mov ecx, dword ptr [esp + 0x18]
    L00543989:
            mov al, byte ptr [edi + 2]
            test al, 4
            je L005439f6
            test al, 2
            push edi
            push esi
            je L005439c5
            xor eax, eax
            mov ax, word ptr [esi + 4]
            mov edx, eax
            mov eax, dword ptr [rend+0xc]
            shl edx, 6
            add edx, eax
            push edx
            xor edx, edx
            mov dx, word ptr [esi + 2]
            shl edx, 6
            add edx, eax
            push edx
            xor edx, edx
            mov dx, word ptr [esi]
            shl edx, 6
            add edx, eax
            push edx
            push ecx
            call dword ptr [ecx]
            jmp L005439f3
    L005439c5:
            xor eax, eax
            xor edx, edx
            mov ax, word ptr [esi + 4]
            mov dx, word ptr [esi + 2]
            mov ecx, eax
            mov eax, dword ptr [rend+0xc]
            shl ecx, 6
            add ecx, eax
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi]
            shl edx, 6
            shl ecx, 6
            add edx, eax
            add ecx, eax
            push edx
            push ecx
            push ebp
            call dword ptr [ebp]
    L005439f3:
            add esp, 0x18
    L005439f6:
            mov eax, dword ptr [rend+0x34]
            inc ebx
            add esi, 0x1c
            add edi, 4
            cmp ebx, eax
            jl L00543985
    L00543a0a:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00542830
void C2_HOOK_CDECL V1Face_OS_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;

    for (f = 0, tfp = rend.temp_faces, fp = rend.faces; f < rend.nfaces; f++, tfp++, fp++) {
        if (tfp->flag & TFF_VISIBLE) {
            unclipped->render(unclipped,
                &rend.temp_vertices[fp->vertices[0]],
                &rend.temp_vertices[fp->vertices[1]],
                &rend.temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005428b0
void C2_HOOK_CDECL V1Face_OSV_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;

    for (f = 0, tfp = rend.temp_faces, fp = rend.faces; f < rend.nfaces; f++, tfp++, fp++) {
        unclipped->render(unclipped,
            &rend.temp_vertices[fp->vertices[0]],
            &rend.temp_vertices[fp->vertices[1]],
            &rend.temp_vertices[fp->vertices[2]],
            fp,
            tfp);
    }
}

static void C2_HOOK_STDCALL AddReplicateConstant(br_geometry_v1_model_soft* self, br_soft_renderer* renderer) {

    if (rend.block->constant_mask == (1 << C_I)) {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleReplicateConstantI);
#if 0
    /* VERIFYME: this is different from Carmageddon II and BRender sources */
    } else if (rend.block->constant_mask == ((1 << C_R) | (1 << C_G) | (1 << C_B))) {
#else
    } else if (rend.block->constant_mask == (1 << C_R) | (1 << C_G) | (1 << C_B)) {
#endif
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleReplicateConstantRGB);
    } else {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleReplicateConstant);
    }
}

static void C2_HOOK_STDCALL V1Faces_GeometryFnsUpdate(br_geometry_v1_model_soft* self, br_soft_renderer* renderer, br_boolean divert) {

    GeometryFunctionReset(renderer);
    PrimBlockReset(renderer);

    GeometryFunctionBothAdd(renderer, (geometry_fn*)V1Faces_ScratchAllocate);
    GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_ClearFlags);

    if (renderer->state.cull.type == BRT_TWO_SIDED) {
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_ClearFlags);
    }

    switch (renderer->state.cull.type) {
        case BRT_ONE_SIDED:
            GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_CullOneSided);
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_CullOneSided);
            break;
        case BRT_TWO_SIDED:
            GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_CullTwoSided);
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_CullTwoSided);
            break;
        default:
            GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_CullNone);
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_CullNone);
            break;
    }

    VertexGeometryFns(renderer, (geometry_fn*)V1Face_Outcode);
    GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_Render);

    if (renderer->state.cull.type == BRT_ONE_SIDED) {
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_Render);
    } else {
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OSV_Render);
    }

    GeometryFunctionBothAdd(renderer, (geometry_fn*)ScratchFree);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_x, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_i, 0x28);

    if (divert) {
        if (rend.block->convert_mask_i != 0 || rend.block->convert_mask_x != 0) {
            switch (rend.block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddPointConvert);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddLineConvert);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddTriangleConvert);
                break;
            default:
                break;
            }
        } else {
            switch (rend.block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddPoint);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddLine);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddTriangle);
                break;
            default:
                break;
            }
        }
    } else {
        if (rend.block->convert_mask_i != 0 || rend.block->convert_mask_x != 0) {
            switch (rend.block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, (brp_render_fn*)RenderConvert1);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)RenderConvert2);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)RenderConvert3);
                break;
            default:
                break;
            }
        }
    }

    switch (rend.block->type) {
    case BRT_POINT:
        PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleToPoints);
        PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleToPoints_OS);

        if (renderer->state.cache.nconstant_fns != 0) {
            AddReplicateConstant(self, renderer);
            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleConstantSurf);
            }
        }
        break;

    case BRT_LINE:
        PrimBlockAdd(renderer, (brp_render_fn*)OpLineClip);
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleToLines);

        if (renderer->state.cache.nconstant_fns != 0) {
            AddReplicateConstant(self, renderer);
            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleConstantSurf);
            }
        }
        break;

    case BRT_TRIANGLE:
        if (renderer->state.cache.nconstant_fns != 0) {
            C2_HOOK_BUG_ON(BR_PRIMF_CONST_DUPLICATE != 0x2);
            if (rend.block->flags & BR_PRIMF_CONST_DUPLICATE) {
                AddReplicateConstant(self, renderer);
            }
            PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleClipConstantSurf);

            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleConstantSurf);
            }
        } else {
            PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleClip);
        }

        C2_HOOK_BUG_ON(BR_PRIMF_SUBDIVIDE != 0x4);

        if (rend.block->flags & BR_PRIMF_SUBDIVIDE) {
            PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleSubdivide);
            PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleSubdivideOnScreen);
            SubdivideSetThreshold(rend.block->subdivide_tolerance);
        }
        break;
    default:
        break;
    }

    if ((renderer->state.cull.type == BRT_TWO_SIDED) && renderer->state.cache.nvertex_fns != 0) {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleRelightTwoSided);
    }

    if (renderer->state.surface.mapping_source == BRT_ENVIRONMENT_INFINITE ||
        renderer->state.surface.mapping_source == BRT_ENVIRONMENT_LOCAL) {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleMappingWrapFix);
    }

    renderer->state.cache.format = self;
}

// FUNCTION: CARMA2_HW 0x00542960
br_error C2_NAKED C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_soft_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen) {
#ifndef CARPOCALYPSE2_MATCHING
    int i, g;
    br_renderer_state_stored_soft* state;
    br_error r;
    br_boolean z_sort, z_sort_blended, divert;

    CheckPrimitiveState(renderer);

    rend.geometry = self;
    rend.renderer = renderer;

    scache.min = renderer->state.bounds.min;
    scache.max = renderer->state.bounds.max;

    if (!scache.valid_per_scene) {
        StaticCacheUpdate_PerScene(renderer);
        scache.valid_per_scene = 1;
    }

    if (!scache.valid_per_model) {
        StaticCacheUpdate_PerModel(renderer);
        scache.valid_per_model = 1;
    }

    for (g = 0; g < model->ngroups; g++) {

        rend.faces = model->groups[g].faces;
        rend.vertices = model->groups[g].vertices;
        rend.face_colours = model->groups[g].face_colours.colours;
        rend.vertex_colours = model->groups[g].vertex_colours;
        rend.nfaces = model->groups[g].nfaces;
        rend.nvertices = model->groups[g].nvertices;
        rend.nedges = model->groups[g].nedges;

        /* First restore material's stored state FULLY (including cull from material flags) */
        if (default_state != NULL) {
            renderer->dispatch->_stateRestore((br_renderer*)renderer, (br_renderer_state_stored*)default_state, BR_STATE_ALL);
        }

        /* Then restore model group's stored state WITHOUT cull (don't override material's cull) */
        state = model->groups[g].stored;
        if (state != NULL) {
            renderer->dispatch->_stateRestore((br_renderer*)renderer, (br_renderer_state_stored*)state, BR_STATE_ALL & ~BR_STATE_CULL);
        }

        z_sort = renderer->state.hidden.type == BRT_BUCKET_SORT &&
                 renderer->state.hidden.order_table != NULL &&
                 renderer->state.hidden.heap != NULL;

        r = renderer->state.pstate->dispatch->_renderBegin(renderer->state.pstate,
            &rend.block, &rend.block_changed, &rend.range_changed, z_sort, type);

        if (r != 0) {
            return r;
        }

        z_sort_blended = (rend.block->flags & BR_PRIMF_BLENDED) &&
            renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER &&
            renderer->state.hidden.order_table != NULL &&
            renderer->state.hidden.heap != NULL;

        z_sort |= z_sort_blended;

        scache.colour = renderer->state.surface.colour & 0xffffff;
        scache.colour |= ((int)(renderer->state.surface.opacity * 256.f)) << 24;

        renderer->state.cache.face_blocks[0].chain = rend.block;
        renderer->state.cache.face_blocks_onscreen[0].chain = rend.block;

        if (rend.block_changed || rend.range_changed || !renderer->state.cache.valid) {
            CacheUpdate(renderer);

            V1Faces_GeometryFnsUpdate(self, renderer, z_sort);

            renderer->state.cache.valid = 1;
        }

        if (on_screen) {
            for (i = 0; i < renderer->state.cache.ngeometry_fns_onscreen; i++) {
                renderer->state.cache.geometry_fns_onscreen[i]((br_geometry*)self, (br_renderer*)renderer);
            }
        } else {
            for (i = 0; i < renderer->state.cache.ngeometry_fns; i++) {
                renderer->state.cache.geometry_fns[i]((br_geometry*)self, (br_renderer*)renderer);
            }
        }

        if (state != NULL) {
            renderer->dispatch->_stateSave((br_renderer*)renderer, (br_renderer_state_stored*)state, BR_STATE_CACHE);
            state->cache.valid = 1;
        }

        renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, rend.block);
    }

    renderer->state.bounds.min = scache.min;
    renderer->state.bounds.max = scache.max;

    return 0;
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x18]
            push edi
            push esi
            call CheckPrimitiveState
            mov dword ptr [rend+0x58], esi
            mov edi, 1
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [rend+0x5c], eax
            mov ecx, dword ptr [esi + 0x568]
            mov dword ptr [scache+0xe4], ecx
            mov edx, dword ptr [esi + 0x56c]
            mov dword ptr [scache+0xe8], edx
            mov eax, dword ptr [esi + 0x570]
            mov dword ptr [scache+0xec], eax
            mov eax, dword ptr [scache+0x4ac]
            mov ecx, dword ptr [esi + 0x574]
            test eax, eax
            mov dword ptr [scache+0xf0], ecx
            jne L005429c7
            push esi
            call StaticCacheUpdate_PerScene
            mov dword ptr [scache+0x4ac], edi
    L005429c7:
            mov eax, dword ptr [scache+0x4a8]
            test eax, eax
            jne L005429dc
            push esi
            call StaticCacheUpdate_PerModel
            mov dword ptr [scache+0x4a8], edi
    L005429dc:
            mov ecx, dword ptr [esp + 0x20]
            xor eax, eax
            mov dword ptr [esp + 0x1c], eax
            cmp word ptr [ecx + 8], ax
            jbe L00542f50
            jmp L005429fa
    L005429f2:
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x20]
    L005429fa:
            mov edx, dword ptr [ecx + 0x18]
            lea eax, [eax + eax*8]
            shl eax, 2
            xor edi, edi
            mov edx, dword ptr [edx + eax + 4]
            mov dword ptr [rend+0x24], edx
            mov edx, dword ptr [ecx + 0x18]
            mov edx, dword ptr [edx + eax + 0x10]
            mov dword ptr [rend+0x28], edx
            mov edx, dword ptr [ecx + 0x18]
            mov edx, dword ptr [edx + eax + 8]
            mov dword ptr [rend+0x2c], edx
            mov edx, dword ptr [ecx + 0x18]
            mov edx, dword ptr [edx + eax + 0x14]
            mov dword ptr [rend+0x30], edx
            mov edx, dword ptr [ecx + 0x18]
            mov di, word ptr [edx + eax + 0x1c]
            mov dword ptr [rend+0x34], edi
            mov edx, dword ptr [ecx + 0x18]
            xor edi, edi
            mov di, word ptr [edx + eax + 0x1e]
            mov dword ptr [rend+0x38], edi
            mov edx, dword ptr [ecx + 0x18]
            xor edi, edi
            mov di, word ptr [edx + eax + 0x20]
            mov dword ptr [rend+0x3c], edi
            mov ecx, dword ptr [ecx + 0x18]
            mov eax, dword ptr [ecx + eax]
            test eax, eax
            jne L00542a72
            mov eax, dword ptr [esp + 0x24]
    L00542a72:
            test eax, eax
            mov dword ptr [esp + 0x10], eax
            je L00542a89
            mov edx, dword ptr [esi]
            push -1
            push eax
            push esi
            call dword ptr [edx + 0xc4]
            add esp, 0xc
    L00542a89:
            cmp dword ptr [esi + 0x580], 0x101
            jne L00542ab0
            mov eax, dword ptr [esi + 0x584]
            test eax, eax
            je L00542ab0
            mov eax, dword ptr [esi + 0x588]
            test eax, eax
            je L00542ab0
            mov ebx, 1
            jmp L00542ab2
    L00542ab0:
            xor ebx, ebx
    L00542ab2:
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esi + 0x794]
            push edx
            push ebx
            mov ecx, dword ptr [eax]
            push offset rend+0x54
            push offset rend+0x50
            push offset rend+0x4c
            push eax
            call dword ptr [ecx + 0x70]
            add esp, 0x18
            test eax, eax
            jne L00542f81
            mov ebp, dword ptr [rend+0x4c]
            test byte ptr [ebp + 0x14], 8
            je L00542b11
            cmp dword ptr [esi + 0x580], 0x1b3
            jne L00542b11
            mov eax, dword ptr [esi + 0x584]
            test eax, eax
            je L00542b11
            mov eax, dword ptr [esi + 0x588]
            test eax, eax
            je L00542b11
            mov eax, 1
            jmp L00542b13
    L00542b11:
            xor eax, eax
    L00542b13:
            mov edi, dword ptr [esi + 0x24]
            or ebx, eax
            and edi, 0xffffff
            mov dword ptr [scache+0xe0], edi
            fld dword ptr [esi + 0x28]
            fmul dword ptr [rendererStateStoredDispatch+0x44]
            call _ftol
            shl eax, 0x18
            or edi, eax
            mov dword ptr [scache+0xe0], edi
            mov dword ptr [esi + 0x6b0], ebp
            mov eax, dword ptr [rend+0x4c]
            mov dword ptr [esi + 0x6e8], eax
            mov eax, dword ptr [rend+0x50]
            test eax, eax
            jne L00542b6e
            mov eax, dword ptr [rend+0x54]
            test eax, eax
            jne L00542b6e
            mov eax, dword ptr [esi + 0x788]
            test eax, eax
            jne L00542e99
    L00542b6e:
            push esi
            call CacheUpdate
            push esi
            call GeometryFunctionReset
            push esi
            call PrimBlockReset
            push offset V1Faces_ScratchAllocate
            push esi
            call GeometryFunctionBothAdd
            push offset Vertex_ClearFlags
            push esi
            call GeometryFunctionAdd
            mov eax, dword ptr [esi + 0x18]
            mov edi, 0xae
            cmp eax, edi
            jne L00542bad
            push offset Vertex_ClearFlags
            push esi
            call GeometryFunctionOnScreenAdd
    L00542bad:
            mov eax, dword ptr [esi + 0x18]
            sub eax, 0xad
            je L00542bde
            dec eax
            je L00542bcc
            push offset V1Face_CullNone
            push esi
            call GeometryFunctionAdd
            push offset V1Face_OS_CullNone
            jmp L00542bee
    L00542bcc:
            push offset V1Face_CullTwoSided
            push esi
            call GeometryFunctionAdd
            push offset V1Face_OS_CullTwoSided
            jmp L00542bee
    L00542bde:
            push offset V1Face_CullOneSided
            push esi
            call GeometryFunctionAdd
            push offset V1Face_OS_CullOneSided
    L00542bee:
            push esi
            call GeometryFunctionOnScreenAdd
            push offset V1Face_Outcode
            push esi
            call VertexGeometryFns
            push offset V1Face_Render
            push esi
            call GeometryFunctionAdd
            cmp dword ptr [esi + 0x18], 0xad
            jne L00542c1a
            push offset V1Face_OS_Render
            jmp L00542c1f
    L00542c1a:
            push offset V1Face_OSV_Render
    L00542c1f:
            push esi
            call GeometryFunctionOnScreenAdd
            push offset ScratchFree
            push esi
            call GeometryFunctionBothAdd
            mov eax, dword ptr [rend+0x4c]
            test ebx, ebx
            mov ecx, dword ptr [eax + 0x28]
            je L00542c95
            test ecx, ecx
            jne L00542c6e
            mov ecx, dword ptr [eax + 0x24]
            test ecx, ecx
            jne L00542c6e
            mov eax, dword ptr [eax + 0x10]
            sub eax, 0xd1
            je L00542c67
            sub eax, 0x39
            je L00542c60
            dec eax
            jne L00542ccb
            push offset OpHeapAddTriangle
            jmp L00542cc5
    L00542c60:
            push offset OpHeapAddLine
            jmp L00542cc5
    L00542c67:
            push offset OpHeapAddPoint
            jmp L00542cc5
    L00542c6e:
            mov eax, dword ptr [eax + 0x10]
            sub eax, 0xd1
            je L00542c8e
            sub eax, 0x39
            je L00542c87
            dec eax
            jne L00542ccb
            push offset OpHeapAddTriangleConvert
            jmp L00542cc5
    L00542c87:
            push offset OpHeapAddLineConvert
            jmp L00542cc5
    L00542c8e:
            push offset OpHeapAddPointConvert
            jmp L00542cc5
    L00542c95:
            test ecx, ecx
            jne L00542ca0
            mov ecx, dword ptr [eax + 0x24]
            test ecx, ecx
            je L00542ccb
    L00542ca0:
            mov eax, dword ptr [eax + 0x10]
            sub eax, 0xd1
            je L00542cc0
            sub eax, 0x39
            je L00542cb9
            dec eax
            jne L00542ccb
            push offset RenderConvert3
            jmp L00542cc5
    L00542cb9:
            push offset RenderConvert2
            jmp L00542cc5
    L00542cc0:
            push offset RenderConvert1
    L00542cc5:
            push esi
            call PrimBlockAddBoth
    L00542ccb:
            mov ecx, dword ptr [rend+0x4c]
            mov eax, dword ptr [ecx + 0x10]
            sub eax, 0xd1
            je L00542ddd
            sub eax, 0x39
            je L00542da2
            dec eax
            jne L00542e4f
            mov eax, dword ptr [esi + 0x748]
            test eax, eax
            je L00542d5e
            test byte ptr [ecx + 0x14], 2
            je L00542d34
            mov eax, dword ptr [ecx + 0x2c]
            cmp eax, 0x400
            jne L00542d10
            push offset OpTriangleReplicateConstantI
            jmp L00542d2e
    L00542d10:
            xor ecx, ecx
            cmp eax, 0x800
            sete cl
            or ecx, 0x3000
            je L00542d29
            push offset OpTriangleReplicateConstantRGB
            jmp L00542d2e
    L00542d29:
            push offset OpTriangleReplicateConstant
    L00542d2e:
            push esi
            call PrimBlockAddBoth
    L00542d34:
            push offset OpTriangleClipConstantSurf
            push esi
            call PrimBlockAdd
            cmp dword ptr [esi + 0x18], edi
            jne L00542d51
            push offset OpTriangleTwoSidedConstantSurf
            push esi
            call PrimBlockOnScreenAdd
            jmp L00542d69
    L00542d51:
            push offset OpTriangleConstantSurf
            push esi
            call PrimBlockOnScreenAdd
            jmp L00542d69
    L00542d5e:
            push offset OpTriangleClip
            push esi
            call PrimBlockAdd
    L00542d69:
            mov edx, dword ptr [rend+0x4c]
            test byte ptr [edx + 0x14], 4
            je L00542e4f
            push offset OpTriangleSubdivide
            push esi
            call PrimBlockAdd
            push offset OpTriangleSubdivideOnScreen
            push esi
            call PrimBlockOnScreenAdd
            mov eax, dword ptr [rend+0x4c]
            mov ecx, dword ptr [eax + 0x30]
            push ecx
            call SubdivideSetThreshold
            jmp L00542e4f
    L00542da2:
            push offset OpLineClip
            push esi
            call PrimBlockAdd
            push offset OpTriangleToLines
            push esi
            call PrimBlockAddBoth
            mov eax, dword ptr [esi + 0x748]
            test eax, eax
            je L00542e4f
            mov edx, dword ptr [rend+0x4c]
            mov eax, dword ptr [edx + 0x2c]
            cmp eax, 0x400
            jne L00542e14
            push offset OpTriangleReplicateConstantI
            jmp L00542e32
    L00542ddd:
            push offset OpTriangleToPoints
            push esi
            call PrimBlockAdd
            push offset OpTriangleToPoints_OS
            push esi
            call PrimBlockOnScreenAdd
            mov eax, dword ptr [esi + 0x748]
            test eax, eax
            je L00542e4f
            mov edx, dword ptr [rend+0x4c]
            mov eax, dword ptr [edx + 0x2c]
            cmp eax, 0x400
            jne L00542e14
            push offset OpTriangleReplicateConstantI
            jmp L00542e32
    L00542e14:
            xor ecx, ecx
            cmp eax, 0x800
            sete cl
            or ecx, 0x3000
            je L00542e2d
            push offset OpTriangleReplicateConstantRGB
            jmp L00542e32
    L00542e2d:
            push offset OpTriangleReplicateConstant
    L00542e32:
            push esi
            call PrimBlockAddBoth
            cmp dword ptr [esi + 0x18], edi
            jne L00542e44
            push offset OpTriangleTwoSidedConstantSurf
            jmp L00542e49
    L00542e44:
            push offset OpTriangleConstantSurf
    L00542e49:
            push esi
            call PrimBlockAddBoth
    L00542e4f:
            cmp dword ptr [esi + 0x18], edi
            jne L00542e69
            mov eax, dword ptr [esi + 0x724]
            test eax, eax
            je L00542e69
            push offset OpTriangleRelightTwoSided
            push esi
            call PrimBlockAddBoth
    L00542e69:
            mov eax, dword ptr [esi + 0x48]
            cmp eax, 0xb8
            je L00542e7a
            cmp eax, 0xb7
            jne L00542e85
    L00542e7a:
            push offset OpTriangleMappingWrapFix
            push esi
            call PrimBlockAddBoth
    L00542e85:
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [esi + 0x788], 1
            mov dword ptr [esi + 0x720], edx
    L00542e99:
            mov eax, dword ptr [esp + 0x2c]
            test eax, eax
            je L00542ecf
            mov eax, dword ptr [esi + 0x668]
            xor edi, edi
            test eax, eax
            jle L00542efb
            mov ebx, dword ptr [esp + 0x18]
            lea ebp, [esi + 0x66c]
    L00542eb7:
            push esi
            push ebx
            call dword ptr [ebp]
            mov eax, dword ptr [esi + 0x668]
            add esp, 8
            inc edi
            add ebp, 4
            cmp edi, eax
            jl L00542eb7
            jmp L00542efb
    L00542ecf:
            mov eax, dword ptr [esi + 0x624]
            xor edi, edi
            test eax, eax
            jle L00542efb
            lea ebp, [esi + 0x628]
    L00542ee1:
            mov eax, dword ptr [esp + 0x18]
            push esi
            push eax
            call dword ptr [ebp]
            mov eax, dword ptr [esi + 0x624]
            add esp, 8
            inc edi
            add ebp, 4
            cmp edi, eax
            jl L00542ee1
    L00542efb:
            mov edi, dword ptr [esp + 0x10]
            test edi, edi
            je L00542f1f
            mov ecx, dword ptr [esi]
            push 0x100
            push edi
            push esi
            call dword ptr [ecx + 0xc0]
            add esp, 0xc
            mov dword ptr [edi + 0x250], 1
    L00542f1f:
            mov eax, dword ptr [esi + 0x794]
            mov ecx, dword ptr [rend+0x4c]
            push ecx
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 0x74]
            mov ecx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x24]
            xor edx, edx
            add esp, 8
            mov dx, word ptr [ecx + 8]
            inc eax
            cmp eax, edx
            mov dword ptr [esp + 0x1c], eax
            jl L005429f2
    L00542f50:
            mov edx, dword ptr [scache+0xe4]
            mov dword ptr [esi + 0x568], edx
            mov eax, dword ptr [scache+0xe8]
            mov dword ptr [esi + 0x56c], eax
            mov ecx, dword ptr [scache+0xec]
            mov dword ptr [esi + 0x570], ecx
            mov edx, dword ptr [scache+0xf0]
            mov dword ptr [esi + 0x574], edx
            xor eax, eax
    L00542f81:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 0x18
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

// FUNCTION: CARMA2_HW 0x00542930
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, (br_soft_renderer*)renderer, model, default_state, type, 0);

    return r;
}

// FUNCTION: CARMA2_HW 0x00543a10
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreen(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, (br_soft_renderer*)renderer, model, default_state, type, 1);

    return r;
}
