#include "heap.h"

#include "convert.h"
#include "setup.h"
#include "zsrmacro.h"

#include "c2_string.h"

static br_boolean heapCheck(br_primitive_heap* heap, br_size_t s) {

    return heap->current + s <= heap->base + heap->size;
}

static br_primitive *heapPrimitiveAdd(br_primitive_heap* heap, br_token type) {
    br_primitive* p;

    p = (br_primitive *)heap->current;

    C2_HOOK_BUG_ON(sizeof(br_primitive) != 0x18);
    heap->current += sizeof(br_primitive);

    p->type = type;
    p->stored = rend.renderer->last_restored;

    return p;
}

static brp_vertex *heapVertexAdd(br_primitive_heap* heap, brp_vertex* src, br_boolean share) {
    brp_vertex* v;
    int n;

    C2_HOOK_BUG_ON(sizeof(brp_vertex) != 0x40);

    if (share) {
        if (src >= rend.temp_vertices) {
            n = src - rend.temp_vertices;
            if (n < rend.nvertices) {
                if (rend.vertex_heap_pointers[n] == NULL) {
                    rend.vertex_heap_pointers[n] = (brp_vertex*)heap->current;
                    memcpy(rend.vertex_heap_pointers[n], src, sizeof(brp_vertex));
                    heap->current += sizeof(brp_vertex);
                }
                return rend.vertex_heap_pointers[n];
            }
        }
    }

    v = (brp_vertex*)heap->current;
    memcpy(v, src, sizeof(brp_vertex));
    *v = *src;
    heap->current += sizeof(brp_vertex);

    return v;
}

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src);
extern void __cdecl _ftol();
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
#endif
// FUNCTION: CARMA2_HW 0x00546860
void C2_NAKED C2_HOOK_CDECL OpHeapAddTriangle(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_primitive* p;
    br_scalar zprim;

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 3 * sizeof(brp_vertex) != 0xd8);
    if (!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 3 * sizeof(brp_vertex))) {
        return;
    }

    if (!rend.renderer->state.surface.force_front) {
        SORT_VALUE_TRIANGLE(rend.renderer->state.hidden.order_table->type, v0, v1, v2);
    } else {
        zprim = 0.f;
    }

    p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_TRIANGLE);
    p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, v0, rend.block->constant_components == 0);
    p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, v1, !~(rend.block->flags & 0x2));
    p->v[2] = heapVertexAdd(rend.renderer->state.hidden.heap, v2, !~(rend.block->flags & 0x2));

    if (rend.renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[3];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);
        z[2] = VIEW_Z(v2);

        rend.renderer->state.hidden.insert_fn(p,
            rend.renderer->state.hidden.insert_arg1,
            rend.renderer->state.hidden.insert_arg2,
            rend.renderer->state.hidden.insert_arg3,
            rend.renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x58]
            sub esp, 0xc
            mov ecx, dword ptr [eax + 0x588]
            push ebx
            push ebp
            push esi
            mov edx, dword ptr [ecx + 4]
            mov ebp, dword ptr [ecx]
            mov esi, dword ptr [ecx + 8]
            add edx, ebp
            add esi, 0xd8
            push edi
            cmp esi, edx
            ja L00546bd1
            mov edx, dword ptr [eax + 0x40]
            test edx, edx
            jne L00546925
            mov edx, dword ptr [esp + 0x24]
            fld dword ptr [edx + 0x10]
            mov edx, dword ptr [eax + 0x584]
            xor eax, eax
            mov ax, word ptr [edx + 0x20]
            dec eax
            je L0054690d
            dec eax
            je L005468dd
            dec eax
            jne L0054692b
            mov edx, dword ptr [esp + 0x28]
            fld dword ptr [edx + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L005468c6
            fstp st(0)
            fld dword ptr [edx + 0x10]
    L005468c6:
            mov edx, dword ptr [esp + 0x2c]
            fld dword ptr [edx + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0054692b
            fstp st(0)
            fld dword ptr [edx + 0x10]
            jmp L0054692b
    L005468dd:
            mov edx, dword ptr [esp + 0x28]
            fld dword ptr [edx + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L005468f2
            fstp st(0)
            fld dword ptr [edx + 0x10]
    L005468f2:
            mov eax, dword ptr [esp + 0x2c]
            fld dword ptr [eax + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L0054692b
            mov edx, dword ptr [esp + 0x2c]
            fstp st(0)
            fld dword ptr [edx + 0x10]
            jmp L0054692b
    L0054690d:
            mov eax, dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x2c]
            fld dword ptr [eax + 0x10]
            fadd dword ptr [edx + 0x10]
            faddp st(1), st(0)
            fmul dword ptr [rendererStateStoredDispatch+0x8c]
            jmp L0054692b
    L00546925:
            fld dword ptr [rendererStateStoredDispatch+0x90]
    L0054692b:
            mov ebx, dword ptr [ecx + 8]
            lea eax, [ebx + 0x18]
            mov dword ptr [ecx + 8], eax
            mov dword ptr [ebx + 4], 0x10b
            mov ecx, dword ptr [rend+0x58]
            mov edx, dword ptr [ecx + 0x79c]
            mov dword ptr [ebx + 8], edx
            mov ecx, dword ptr [rend+0x4c]
            mov eax, dword ptr [rend+0x58]
            mov edx, dword ptr [ecx + 0x18]
            mov eax, dword ptr [eax + 0x588]
            test edx, edx
            jne L005469c2
            mov edx, dword ptr [rend+0xc]
            mov esi, dword ptr [esp + 0x24]
            cmp esi, edx
            jb L005469c6
            mov ecx, esi
            sub ecx, edx
            mov edx, dword ptr [rend+0x38]
            sar ecx, 6
            cmp ecx, edx
            jge L005469c6
            lea edx, [ecx*4]
            mov ecx, dword ptr [rend+0x10]
            mov ebp, 0x40
            cmp dword ptr [ecx + edx], 0
            jne L005469bd
            mov edi, dword ptr [eax + 8]
            mov dword ptr [ecx + edx], edi
            mov edi, dword ptr [rend+0x10]
            mov ecx, 0x10
            mov edi, dword ptr [edi + edx]
            rep movsd
            mov ecx, dword ptr [eax + 8]
            add ecx, ebp
            mov dword ptr [eax + 8], ecx
            mov ecx, dword ptr [rend+0x10]
    L005469bd:
            mov edx, dword ptr [ecx + edx]
            jmp L005469da
    L005469c2:
            mov esi, dword ptr [esp + 0x24]
    L005469c6:
            mov edx, dword ptr [eax + 8]
            mov ecx, 0x10
            mov edi, edx
            mov ebp, 0x40
            rep movsd
            add dword ptr [eax + 8], ebp
    L005469da:
            mov dword ptr [ebx + 0xc], edx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [rend+0x4c]
            mov eax, dword ptr [edx + 0x588]
            mov edx, dword ptr [ecx + 0x14]
            not edx
            test dl, 2
            je L00546a54
            mov edx, dword ptr [rend+0xc]
            mov esi, dword ptr [esp + 0x28]
            cmp esi, edx
            jb L00546a58
            mov ecx, esi
            sub ecx, edx
            mov edx, dword ptr [rend+0x38]
            sar ecx, 6
            cmp ecx, edx
            jge L00546a58
            lea edx, [ecx*4]
            mov ecx, dword ptr [rend+0x10]
            cmp dword ptr [ecx + edx], 0
            jne L00546a4f
            mov edi, dword ptr [eax + 8]
            mov dword ptr [ecx + edx], edi
            mov edi, dword ptr [rend+0x10]
            mov ecx, 0x10
            mov edi, dword ptr [edi + edx]
            rep movsd
            mov ecx, dword ptr [eax + 8]
            add ecx, ebp
            mov dword ptr [eax + 8], ecx
            mov ecx, dword ptr [rend+0x10]
    L00546a4f:
            mov edx, dword ptr [ecx + edx]
            jmp L00546a67
    L00546a54:
            mov esi, dword ptr [esp + 0x28]
    L00546a58:
            mov edx, dword ptr [eax + 8]
            mov ecx, 0x10
            mov edi, edx
            rep movsd
            add dword ptr [eax + 8], ebp
    L00546a67:
            mov dword ptr [ebx + 0x10], edx
            mov ecx, dword ptr [rend+0x4c]
            mov eax, dword ptr [rend+0x58]
            mov edx, dword ptr [ecx + 0x14]
            mov eax, dword ptr [eax + 0x588]
            not edx
            test dl, 2
            je L00546ae0
            mov edx, dword ptr [rend+0xc]
            mov esi, dword ptr [esp + 0x2c]
            cmp esi, edx
            jb L00546ae4
            mov ecx, esi
            sub ecx, edx
            mov edx, dword ptr [rend+0x38]
            sar ecx, 6
            cmp ecx, edx
            jge L00546ae4
            lea edx, [ecx*4]
            mov ecx, dword ptr [rend+0x10]
            cmp dword ptr [ecx + edx], 0
            jne L00546adb
            mov edi, dword ptr [eax + 8]
            mov dword ptr [ecx + edx], edi
            mov edi, dword ptr [rend+0x10]
            mov ecx, 0x10
            mov edi, dword ptr [edi + edx]
            rep movsd
            mov ecx, dword ptr [eax + 8]
            add ecx, ebp
            mov dword ptr [eax + 8], ecx
            mov ecx, dword ptr [rend+0x10]
    L00546adb:
            mov edx, dword ptr [ecx + edx]
            jmp L00546af3
    L00546ae0:
            mov esi, dword ptr [esp + 0x2c]
    L00546ae4:
            mov edx, dword ptr [eax + 8]
            mov ecx, 0x10
            mov edi, edx
            rep movsd
            add dword ptr [eax + 8], ebp
    L00546af3:
            mov dword ptr [ebx + 0x14], edx
            mov eax, dword ptr [rend+0x58]
            mov ecx, dword ptr [eax + 0x58c]
            test ecx, ecx
            je L00546b5b
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x28]
            fstp st(0)
            fld dword ptr [ecx + 0x10]
            mov ecx, dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [edx + 0x10]
            fstp dword ptr [esp + 0x14]
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax + 0x584]
            lea edx, [esp + 0x10]
            push edx
            mov edx, dword ptr [eax + 0x598]
            push ecx
            mov ecx, dword ptr [eax + 0x594]
            push edx
            mov edx, dword ptr [eax + 0x590]
            push ecx
            push edx
            push ebx
            call dword ptr [eax + 0x58c]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L00546b5b:
            mov esi, dword ptr [eax + 0x584]
            test byte ptr [esi + 0x1c], 4
            je L00546b87
            mov eax, dword ptr [esi]
            fstp st(0)
            mov ecx, dword ptr [eax]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x584]
            mov ecx, dword ptr [eax]
            mov dword ptr [ecx], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L00546b87:
            fsub dword ptr [esi + 0xc]
            fcom dword ptr [rendererStateStoredDispatch+0x90]
            fnstsw ax
            test ah, 1
            je L00546b9d
            fstp st(0)
            xor eax, eax
            jmp L00546bb6
    L00546b9d:
            fld dword ptr [esi + 0x18]
            fmul st(0), st(1)
            call _ftol
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            cmp eax, ecx
            fstp st(0)
            jl L00546bb6
            lea eax, [ecx - 1]
    L00546bb6:
            mov edx, dword ptr [esi]
            shl eax, 2
            mov ecx, dword ptr [eax + edx]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [edx + 0x584]
            mov edx, dword ptr [ecx]
            mov dword ptr [edx + eax], ebx
    L00546bd1:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
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

// FUNCTION: CARMA2_HW 0x00546be0
void C2_NAKED C2_HOOK_CDECL OpHeapAddLine(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
#ifndef CARPOCALYPSE2_MATCHING
    br_primitive *p;
    br_scalar zprim;

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 2 * sizeof(brp_vertex) != 0x98);
    if (!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 2 * sizeof(brp_vertex))) {
        return;
    }

    if (rend.renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_EDGE(rend.renderer->state.hidden.order_table->type, v0, v1);
    }

    p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_LINE);
    p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, v0, !rend.block->constant_components);
    p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, v1, !~(rend.block->flags & 0x2));

    if (rend.renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[2];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);

        rend.renderer->state.hidden.insert_fn(p,
            rend.renderer->state.hidden.insert_arg1,
            rend.renderer->state.hidden.insert_arg2,
            rend.renderer->state.hidden.insert_arg3,
            rend.renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x58]
            sub esp, 8
            mov ecx, dword ptr [eax + 0x588]
            push ebx
            push esi
            push edi
            mov edx, dword ptr [ecx + 4]
            mov esi, dword ptr [ecx]
            add edx, esi
            mov esi, dword ptr [ecx + 8]
            add esi, 0x98
            cmp esi, edx
            ja L00546e7a
            mov edx, dword ptr [eax + 0x40]
            test edx, edx
            jne L00546c6d
            mov edx, dword ptr [esp + 0x1c]
            fld dword ptr [edx + 0x10]
            mov edx, dword ptr [eax + 0x584]
            xor eax, eax
            mov ax, word ptr [edx + 0x20]
            dec eax
            je L00546c5e
            dec eax
            je L00546c43
            dec eax
            jne L00546c73
            mov edx, dword ptr [esp + 0x20]
            fld dword ptr [edx + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L00546c73
            fstp st(0)
            fld dword ptr [edx + 0x10]
            jmp L00546c73
    L00546c43:
            mov eax, dword ptr [esp + 0x20]
            fld dword ptr [eax + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00546c73
            mov edx, dword ptr [esp + 0x20]
            fstp st(0)
            fld dword ptr [edx + 0x10]
            jmp L00546c73
    L00546c5e:
            mov eax, dword ptr [esp + 0x20]
            fadd dword ptr [eax + 0x10]
            fmul dword ptr [rendererStateStoredDispatch+0x94]
            jmp L00546c73
    L00546c6d:
            fld dword ptr [rendererStateStoredDispatch+0x90]
    L00546c73:
            mov ebx, dword ptr [ecx + 8]
            lea edx, [ebx + 0x18]
            mov dword ptr [ecx + 8], edx
            mov dword ptr [ebx + 4], 0x10a
            mov eax, dword ptr [rend+0x58]
            mov ecx, dword ptr [eax + 0x79c]
            mov dword ptr [ebx + 8], ecx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [rend+0x4c]
            mov eax, dword ptr [edx + 0x588]
            mov edx, dword ptr [ecx + 0x18]
            test edx, edx
            jne L00546d06
            mov edx, dword ptr [rend+0xc]
            mov esi, dword ptr [esp + 0x1c]
            cmp esi, edx
            jb L00546d0a
            mov ecx, esi
            sub ecx, edx
            mov edx, dword ptr [rend+0x38]
            sar ecx, 6
            cmp ecx, edx
            jge L00546d0a
            lea edx, [ecx*4]
            mov ecx, dword ptr [rend+0x10]
            cmp dword ptr [ecx + edx], 0
            jne L00546d01
            mov edi, dword ptr [eax + 8]
            mov dword ptr [ecx + edx], edi
            mov edi, dword ptr [rend+0x10]
            mov ecx, 0x10
            mov edi, dword ptr [edi + edx]
            rep movsd
            mov ecx, dword ptr [eax + 8]
            add ecx, 0x40
            mov dword ptr [eax + 8], ecx
            mov ecx, dword ptr [rend+0x10]
    L00546d01:
            mov edx, dword ptr [ecx + edx]
            jmp L00546d1a
    L00546d06:
            mov esi, dword ptr [esp + 0x1c]
    L00546d0a:
            mov edx, dword ptr [eax + 8]
            mov ecx, 0x10
            mov edi, edx
            rep movsd
            add dword ptr [eax + 8], 0x40
    L00546d1a:
            mov dword ptr [ebx + 0xc], edx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [rend+0x4c]
            mov eax, dword ptr [edx + 0x588]
            mov edx, dword ptr [ecx + 0x14]
            not edx
            test dl, 2
            je L00546d95
            mov edx, dword ptr [rend+0xc]
            mov esi, dword ptr [esp + 0x20]
            cmp esi, edx
            jb L00546d99
            mov ecx, esi
            sub ecx, edx
            mov edx, dword ptr [rend+0x38]
            sar ecx, 6
            cmp ecx, edx
            jge L00546d99
            lea edx, [ecx*4]
            mov ecx, dword ptr [rend+0x10]
            cmp dword ptr [ecx + edx], 0
            jne L00546d90
            mov edi, dword ptr [eax + 8]
            mov dword ptr [ecx + edx], edi
            mov edi, dword ptr [rend+0x10]
            mov ecx, 0x10
            mov edi, dword ptr [edi + edx]
            rep movsd
            mov ecx, dword ptr [eax + 8]
            add ecx, 0x40
            mov dword ptr [eax + 8], ecx
            mov ecx, dword ptr [rend+0x10]
    L00546d90:
            mov edx, dword ptr [ecx + edx]
            jmp L00546da9
    L00546d95:
            mov esi, dword ptr [esp + 0x20]
    L00546d99:
            mov edx, dword ptr [eax + 8]
            mov ecx, 0x10
            mov edi, edx
            rep movsd
            add dword ptr [eax + 8], 0x40
    L00546da9:
            mov dword ptr [ebx + 0x10], edx
            mov eax, dword ptr [rend+0x58]
            mov ecx, dword ptr [eax + 0x58c]
            test ecx, ecx
            je L00546e05
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x20]
            fstp st(0)
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0xc]
            fld dword ptr [edx + 0x10]
            fstp dword ptr [esp + 0x10]
            mov edx, dword ptr [eax + 0x584]
            lea ecx, [esp + 0xc]
            push ecx
            mov ecx, dword ptr [eax + 0x598]
            push edx
            mov edx, dword ptr [eax + 0x594]
            push ecx
            mov ecx, dword ptr [eax + 0x590]
            push edx
            push ecx
            push ebx
            call dword ptr [eax + 0x58c]
            add esp, 0x18
            pop edi
            pop esi
            pop ebx
            add esp, 8
            ret
    L00546e05:
            mov esi, dword ptr [eax + 0x584]
            test byte ptr [esi + 0x1c], 4
            je L00546e30
            mov edx, dword ptr [esi]
            fstp st(0)
            mov eax, dword ptr [edx]
            mov dword ptr [ebx], eax
            mov ecx, dword ptr [rend+0x58]
            mov edx, dword ptr [ecx + 0x584]
            mov eax, dword ptr [edx]
            mov dword ptr [eax], ebx
            pop edi
            pop esi
            pop ebx
            add esp, 8
            ret
    L00546e30:
            fsub dword ptr [esi + 0xc]
            fcom dword ptr [rendererStateStoredDispatch+0x90]
            fnstsw ax
            test ah, 1
            je L00546e46
            fstp st(0)
            xor eax, eax
            jmp L00546e5f
    L00546e46:
            fld dword ptr [esi + 0x18]
            fmul st(0), st(1)
            call _ftol
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            cmp eax, ecx
            fstp st(0)
            jl L00546e5f
            lea eax, [ecx - 1]
    L00546e5f:
            mov ecx, dword ptr [esi]
            shl eax, 2
            mov edx, dword ptr [eax + ecx]
            mov dword ptr [ebx], edx
            mov ecx, dword ptr [rend+0x58]
            mov edx, dword ptr [ecx + 0x584]
            mov ecx, dword ptr [edx]
            mov dword ptr [ecx + eax], ebx
    L00546e7a:
            pop edi
            pop esi
            pop ebx
            add esp, 8
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
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00546e90
void C2_NAKED C2_HOOK_CDECL OpHeapAddPoint(brp_block* block, brp_vertex* v0) {
#ifndef CARPOCALYPSE2_MATCHING
    br_primitive* p;
    br_scalar zprim;

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 1 * sizeof(brp_vertex) != 0x58);
    if (!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 1 * sizeof(brp_vertex))) {
        return;
    }

    if (rend.renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        zprim = VIEW_Z(v0);
    }

    p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_POINT);
    p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, v0, 1);

    if (rend.renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[1];
        z[0] = VIEW_Z(v0);

        rend.renderer->state.hidden.insert_fn(p,
            rend.renderer->state.hidden.insert_arg1,
            rend.renderer->state.hidden.insert_arg2,
            rend.renderer->state.hidden.insert_arg3,
            rend.renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
    }
#else
    __asm {
            mov ecx, dword ptr [rend+0x58]
            push ebx
            push ebp
            push esi
            mov eax, dword ptr [ecx + 0x588]
            push edi
            mov edx, dword ptr [eax + 4]
            mov ebp, dword ptr [eax]
            mov esi, dword ptr [eax + 8]
            add edx, ebp
            add esi, 0x58
            cmp esi, edx
            ja L00547020
            mov edx, dword ptr [ecx + 0x40]
            mov ebp, dword ptr [esp + 0x18]
            test edx, edx
            jne L00546ec5
            fld dword ptr [ebp + 0x10]
            jmp L00546ecb
    L00546ec5:
            fld dword ptr [rendererStateStoredDispatch+0x90]
    L00546ecb:
            mov ebx, dword ptr [eax + 8]
            lea ecx, [ebx + 0x18]
            mov dword ptr [eax + 8], ecx
            mov dword ptr [ebx + 4], 0xd1
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x79c]
            mov dword ptr [ebx + 8], eax
            mov ecx, dword ptr [rend+0x58]
            mov edx, dword ptr [rend+0xc]
            mov eax, dword ptr [ecx + 0x588]
            cmp ebp, edx
            jb L00546f50
            mov ecx, ebp
            sub ecx, edx
            mov edx, dword ptr [rend+0x38]
            sar ecx, 6
            cmp ecx, edx
            jge L00546f50
            lea edx, [ecx*4]
            mov ecx, dword ptr [rend+0x10]
            cmp dword ptr [ecx + edx], 0
            jne L00546f4b
            mov esi, dword ptr [eax + 8]
            mov dword ptr [ecx + edx], esi
            mov edi, dword ptr [rend+0x10]
            mov ecx, 0x10
            mov esi, ebp
            mov edi, dword ptr [edi + edx]
            rep movsd
            mov ecx, dword ptr [eax + 8]
            add ecx, 0x40
            mov dword ptr [eax + 8], ecx
            mov ecx, dword ptr [rend+0x10]
    L00546f4b:
            mov edx, dword ptr [ecx + edx]
            jmp L00546f62
    L00546f50:
            mov edx, dword ptr [eax + 8]
            mov ecx, 0x10
            mov esi, ebp
            mov edi, edx
            rep movsd
            add dword ptr [eax + 8], 0x40
    L00546f62:
            mov dword ptr [ebx + 0xc], edx
            mov eax, dword ptr [rend+0x58]
            mov ecx, dword ptr [eax + 0x58c]
            test ecx, ecx
            je L00546fad
            fstp st(0)
            fld dword ptr [ebp + 0x10]
            fstp dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax + 0x584]
            lea edx, [esp + 0x18]
            push edx
            mov edx, dword ptr [eax + 0x598]
            push ecx
            mov ecx, dword ptr [eax + 0x594]
            push edx
            mov edx, dword ptr [eax + 0x590]
            push ecx
            push edx
            push ebx
            call dword ptr [eax + 0x58c]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00546fad:
            mov esi, dword ptr [eax + 0x584]
            test byte ptr [esi + 0x1c], 4
            je L00546fd6
            mov eax, dword ptr [esi]
            fstp st(0)
            mov ecx, dword ptr [eax]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x584]
            mov ecx, dword ptr [eax]
            mov dword ptr [ecx], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00546fd6:
            fsub dword ptr [esi + 0xc]
            fcom dword ptr [rendererStateStoredDispatch+0x90]
            fnstsw ax
            test ah, 1
            je L00546fec
            fstp st(0)
            xor eax, eax
            jmp L00547005
    L00546fec:
            fld dword ptr [esi + 0x18]
            fmul st(0), st(1)
            call _ftol
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            cmp eax, ecx
            fstp st(0)
            jl L00547005
            lea eax, [ecx - 1]
    L00547005:
            mov edx, dword ptr [esi]
            shl eax, 2
            mov ecx, dword ptr [eax + edx]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [edx + 0x584]
            mov edx, dword ptr [ecx]
            mov dword ptr [edx + eax], ebx
    L00547020:
            pop edi
            pop esi
            pop ebp
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00547030
void C2_NAKED C2_HOOK_CDECL OpHeapAddTriangleConvert(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_primitive* p;
    br_scalar zprim;
    brp_vertex outv[3];

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 3 * sizeof(brp_vertex) != 0xd8);
    if (!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 3 * sizeof(brp_vertex))) {
        return;
    }

    if (rend.renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_TRIANGLE(rend.renderer->state.hidden.order_table->type, v0, v1, v2);
    }

    ConvertVertex(&outv[0], v0);
    ConvertVertex(&outv[1], v1);
    ConvertVertex(&outv[2], v2);

    p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_TRIANGLE);
    p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, &outv[0], 0);
    p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, &outv[1], 0);
    p->v[2] = heapVertexAdd(rend.renderer->state.hidden.heap, &outv[2], 0);

    if (rend.renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[3];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);
        z[2] = VIEW_Z(v2);

        rend.renderer->state.hidden.insert_fn(p,
            rend.renderer->state.hidden.insert_arg1,
            rend.renderer->state.hidden.insert_arg2,
            rend.renderer->state.hidden.insert_arg3,
            rend.renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x58]
            sub esp, 0xd0
            mov ecx, dword ptr [eax + 0x588]
            push ebx
            push esi
            push edi
            mov edx, dword ptr [ecx + 4]
            mov ebx, dword ptr [ecx]
            mov ecx, dword ptr [ecx + 8]
            add edx, ebx
            add ecx, 0xd8
            cmp ecx, edx
            ja L005472f9
            mov ecx, dword ptr [eax + 0x40]
            test ecx, ecx
            jne L0054712c
            mov edx, dword ptr [esp + 0xe4]
            mov ecx, dword ptr [eax + 0x584]
            xor eax, eax
            fld dword ptr [edx + 0x10]
            mov ax, word ptr [ecx + 0x20]
            fstp dword ptr [esp + 0xc]
            dec eax
            je L00547108
            dec eax
            je L005470cc
            dec eax
            jne L00547134
            mov ecx, dword ptr [esp + 0xe8]
            fld dword ptr [ecx + 0x10]
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L005470ae
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0xc]
    L005470ae:
            mov ecx, dword ptr [esp + 0xec]
            fld dword ptr [ecx + 0x10]
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L00547134
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0xc]
            jmp L00547134
    L005470cc:
            mov ecx, dword ptr [esp + 0xe8]
            fld dword ptr [ecx + 0x10]
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 1
            je L005470e8
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0xc]
    L005470e8:
            mov edx, dword ptr [esp + 0xec]
            fld dword ptr [edx + 0x10]
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 1
            je L00547134
            mov eax, edx
            fld dword ptr [eax + 0x10]
            fstp dword ptr [esp + 0xc]
            jmp L00547134
    L00547108:
            mov ecx, dword ptr [esp + 0xec]
            mov edx, dword ptr [esp + 0xe8]
            fld dword ptr [ecx + 0x10]
            fadd dword ptr [edx + 0x10]
            fadd dword ptr [esp + 0xc]
            fmul dword ptr [rendererStateStoredDispatch+0x8c]
            fstp dword ptr [esp + 0xc]
            jmp L00547134
    L0054712c:
            mov dword ptr [esp + 0xc], 0
    L00547134:
            mov eax, dword ptr [esp + 0xe4]
            lea ecx, [esp + 0x1c]
            push eax
            push ecx
            call ConvertVertex
            mov edx, dword ptr [esp + 0xe8]
            lea eax, [esp + 0x5c]
            push edx
            push eax
            call ConvertVertex
            mov ecx, dword ptr [esp + 0xec]
            lea edx, [esp + 0x9c]
            push ecx
            push edx
            call ConvertVertex
            mov eax, dword ptr [rend+0x58]
            lea esi, [esp + 0x1c]
            mov eax, dword ptr [eax + 0x588]
            mov ebx, dword ptr [eax + 8]
            lea ecx, [ebx + 0x18]
            mov dword ptr [eax + 8], ecx
            mov dword ptr [ebx + 4], 0x10b
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x79c]
            mov dword ptr [ebx + 8], eax
            mov ecx, dword ptr [rend+0x58]
            mov eax, dword ptr [ecx + 0x588]
            mov ecx, 0x10
            mov edx, dword ptr [eax + 8]
            mov edi, edx
            rep movsd
            mov ecx, dword ptr [eax + 8]
            lea esi, [esp + 0x5c]
            add ecx, 0x40
            mov dword ptr [eax + 8], ecx
            mov dword ptr [ebx + 0xc], edx
            mov edx, dword ptr [rend+0x58]
            mov ecx, 0x10
            mov eax, dword ptr [edx + 0x588]
            mov edx, dword ptr [eax + 8]
            mov edi, edx
            rep movsd
            mov edi, dword ptr [eax + 8]
            mov ecx, 0x10
            add edi, 0x40
            lea esi, [esp + 0x9c]
            mov dword ptr [eax + 8], edi
            mov dword ptr [ebx + 0x10], edx
            mov eax, dword ptr [rend+0x58]
            mov eax, dword ptr [eax + 0x588]
            mov edx, dword ptr [eax + 8]
            mov edi, edx
            rep movsd
            mov esi, dword ptr [eax + 8]
            add esi, 0x40
            mov dword ptr [eax + 8], esi
            mov dword ptr [ebx + 0x14], edx
            mov eax, dword ptr [rend+0x58]
            mov ecx, dword ptr [eax + 0x58c]
            test ecx, ecx
            je L0054727f
            mov ecx, dword ptr [esp + 0xe4]
            mov edx, dword ptr [esp + 0xe8]
            fld dword ptr [ecx + 0x10]
            mov ecx, dword ptr [esp + 0xec]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [edx + 0x10]
            fstp dword ptr [esp + 0x14]
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax + 0x584]
            lea edx, [esp + 0x10]
            push edx
            mov edx, dword ptr [eax + 0x598]
            push ecx
            mov ecx, dword ptr [eax + 0x594]
            push edx
            mov edx, dword ptr [eax + 0x590]
            push ecx
            push edx
            push ebx
            call dword ptr [eax + 0x58c]
            add esp, 0x18
            pop edi
            pop esi
            pop ebx
            add esp, 0xd0
            ret
    L0054727f:
            mov esi, dword ptr [eax + 0x584]
            test byte ptr [esi + 0x1c], 4
            je L005472ab
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x584]
            mov ecx, dword ptr [eax]
            mov dword ptr [ecx], ebx
            pop edi
            pop esi
            pop ebx
            add esp, 0xd0
            ret
    L005472ab:
            fld dword ptr [esi + 0xc]
            fsubr dword ptr [esp + 0xc]
            fcom dword ptr [rendererStateStoredDispatch+0x90]
            fnstsw ax
            test ah, 1
            je L005472c5
            fstp st(0)
            xor eax, eax
            jmp L005472de
    L005472c5:
            fld dword ptr [esi + 0x18]
            fmul st(0), st(1)
            call _ftol
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            cmp eax, ecx
            fstp st(0)
            jl L005472de
            lea eax, [ecx - 1]
    L005472de:
            mov edx, dword ptr [esi]
            shl eax, 2
            mov ecx, dword ptr [eax + edx]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [edx + 0x584]
            mov edx, dword ptr [ecx]
            mov dword ptr [edx + eax], ebx
    L005472f9:
            pop edi
            pop esi
            pop ebx
            add esp, 0xd0
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00547310
void C2_NAKED C2_HOOK_CDECL OpHeapAddLineConvert(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
#ifndef CARPOCALYPSE2_MATCHING
    br_primitive* p;
    br_scalar zprim;
    brp_vertex outv[2];

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 2 * sizeof(brp_vertex) != 0x98);
    if (!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 2 * sizeof(brp_vertex))) {
        return;
    }

    if (rend.renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_EDGE(rend.renderer->state.hidden.order_table->type, v0, v1);
    }

    ConvertVertex(&outv[0], v0);
    ConvertVertex(&outv[1], v1);

    p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_LINE);
    p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, &outv[0], 0);
    p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, &outv[1], 0);

    if (rend.renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[2];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);

        rend.renderer->state.hidden.insert_fn(p,
            rend.renderer->state.hidden.insert_arg1,
            rend.renderer->state.hidden.insert_arg2,
            rend.renderer->state.hidden.insert_arg3,
            rend.renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x58]
            sub esp, 0x88
            mov ecx, dword ptr [eax + 0x588]
            push ebx
            push ebp
            push esi
            mov edx, dword ptr [ecx + 4]
            mov ebx, dword ptr [ecx]
            mov ecx, dword ptr [ecx + 8]
            add edx, ebx
            add ecx, 0x98
            push edi
            cmp ecx, edx
            ja L00547524
            mov ecx, dword ptr [eax + 0x40]
            mov ebp, dword ptr [esp + 0xa4]
            test ecx, ecx
            jne L005473af
            mov edx, dword ptr [esp + 0xa0]
            mov ecx, dword ptr [eax + 0x584]
            xor eax, eax
            fld dword ptr [edx + 0x10]
            mov ax, word ptr [ecx + 0x20]
            fstp dword ptr [esp + 0x10]
            dec eax
            je L0054739c
            dec eax
            je L00547385
            dec eax
            jne L005473b7
            fld dword ptr [ebp + 0x10]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L005473b7
            fld dword ptr [ebp + 0x10]
            fstp dword ptr [esp + 0x10]
            jmp L005473b7
    L00547385:
            fld dword ptr [ebp + 0x10]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L005473b7
            fld dword ptr [ebp + 0x10]
            fstp dword ptr [esp + 0x10]
            jmp L005473b7
    L0054739c:
            fld dword ptr [ebp + 0x10]
            fadd dword ptr [esp + 0x10]
            fmul dword ptr [rendererStateStoredDispatch+0x94]
            fstp dword ptr [esp + 0x10]
            jmp L005473b7
    L005473af:
            mov dword ptr [esp + 0x10], 0
    L005473b7:
            mov edx, dword ptr [esp + 0xa0]
            lea eax, [esp + 0x18]
            push edx
            push eax
            call ConvertVertex
            lea ecx, [esp + 0x58]
            push ebp
            push ecx
            call ConvertVertex
            mov edx, dword ptr [rend+0x58]
            lea esi, [esp + 0x18]
            mov eax, dword ptr [edx + 0x588]
            mov ebx, dword ptr [eax + 8]
            lea ecx, [ebx + 0x18]
            mov dword ptr [eax + 8], ecx
            mov dword ptr [ebx + 4], 0x10a
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x79c]
            mov dword ptr [ebx + 8], eax
            mov ecx, dword ptr [rend+0x58]
            mov eax, dword ptr [ecx + 0x588]
            mov ecx, 0x10
            mov edx, dword ptr [eax + 8]
            mov edi, edx
            rep movsd
            mov edi, dword ptr [eax + 8]
            mov ecx, 0x10
            add edi, 0x40
            lea esi, [esp + 0x58]
            mov dword ptr [eax + 8], edi
            mov dword ptr [ebx + 0xc], edx
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x588]
            mov edx, dword ptr [eax + 8]
            mov edi, edx
            rep movsd
            mov esi, dword ptr [eax + 8]
            add esi, 0x40
            mov dword ptr [eax + 8], esi
            mov dword ptr [ebx + 0x10], edx
            mov eax, dword ptr [rend+0x58]
            mov ecx, dword ptr [eax + 0x58c]
            test ecx, ecx
            je L005474a9
            mov ecx, dword ptr [esp + 0xa0]
            lea edx, [esp + 0x10]
            push edx
            fld dword ptr [ecx + 0x10]
            fstp dword ptr [esp + 0x14]
            fld dword ptr [ebp + 0x10]
            fstp dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax + 0x584]
            mov edx, dword ptr [eax + 0x598]
            push ecx
            mov ecx, dword ptr [eax + 0x594]
            push edx
            mov edx, dword ptr [eax + 0x590]
            push ecx
            push edx
            push ebx
            call dword ptr [eax + 0x58c]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x88
            ret
    L005474a9:
            mov esi, dword ptr [eax + 0x584]
            test byte ptr [esi + 0x1c], 4
            je L005474d6
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov eax, dword ptr [edx + 0x584]
            mov ecx, dword ptr [eax]
            mov dword ptr [ecx], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x88
            ret
    L005474d6:
            fld dword ptr [esi + 0xc]
            fsubr dword ptr [esp + 0x10]
            fcom dword ptr [rendererStateStoredDispatch+0x90]
            fnstsw ax
            test ah, 1
            je L005474f0
            fstp st(0)
            xor eax, eax
            jmp L00547509
    L005474f0:
            fld dword ptr [esi + 0x18]
            fmul st(0), st(1)
            call _ftol
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            cmp eax, ecx
            fstp st(0)
            jl L00547509
            lea eax, [ecx - 1]
    L00547509:
            mov edx, dword ptr [esi]
            shl eax, 2
            mov ecx, dword ptr [eax + edx]
            mov dword ptr [ebx], ecx
            mov edx, dword ptr [rend+0x58]
            mov ecx, dword ptr [edx + 0x584]
            mov edx, dword ptr [ecx]
            mov dword ptr [edx + eax], ebx
    L00547524:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x88
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00547530
void C2_HOOK_CDECL OpHeapAddPointConvert(brp_block* block, brp_vertex* v0) {
    br_primitive* p;
    br_scalar zprim;
    brp_vertex outv[1];

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 1 * sizeof(brp_vertex) != 0x58);
    if (!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 1 * sizeof(brp_vertex))) {
        return;
    }

    if (rend.renderer->state.surface.force_front) {
        zprim = BR_SCALAR(0.0);
    } else {
        SORT_VALUE_POINT(rend.renderer->state.hidden.order_table->type, v0);
    }

    ConvertVertex(&outv[0], v0);

    p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_POINT);
    p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, &outv[0], 0);

    if (rend.renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[1];
        z[0] = VIEW_Z(v0);

        rend.renderer->state.hidden.insert_fn(p,
            rend.renderer->state.hidden.insert_arg1,
            rend.renderer->state.hidden.insert_arg2,
            rend.renderer->state.hidden.insert_arg3,
            rend.renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
    }
}
