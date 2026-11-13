#include "genrend.h"

#include "rend.h"
#include "setup.h"

#include "core/fw/scratch.h"

#ifdef CARPOCALYPSE2_MATCHING
extern br_scalar gScreenZOffset;
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
extern static_cache_soft scache;
#endif
// FUNCTION: CARMA2_HW 0x00547810
void C2_NAKED C2_HOOK_CDECL Vertex_TransformProjectOutcodeBounds(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    brp_vertex* tvp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        C2_HOOK_BUG_ON((OUTCODE_RIGHT | OUTCODE_N_RIGHT) != 0x20002);
        C2_HOOK_BUG_ON((OUTCODE_LEFT | OUTCODE_N_LEFT) != 0x10001);
        C2_HOOK_BUG_ON((OUTCODE_TOP | OUTCODE_N_TOP) != 0x40004);
        C2_HOOK_BUG_ON((OUTCODE_BOTTOM | OUTCODE_N_BOTTOM) != 0x80008);
        C2_HOOK_BUG_ON((OUTCODE_HITHER | OUTCODE_N_HITHER) != 0x100010);
        C2_HOOK_BUG_ON((OUTCODE_YON | OUTCODE_N_YON) != 0x200020);
        C2_HOOK_BUG_ON((OUTCODE_USER | OUTCODE_N_USER) != 0x400040);

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &rend.vertices[v].p, &scache.model_to_screen);
        OUTCODE_POINT(tvp->flags, (br_vector4 *)(tvp->comp + C_X));

        if (!(tvp->flags & OUTCODES_ALL)) {
            PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
            UPDATE_BOUNDS(tvp);
        }
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x38]
            mov edx, dword ptr [rend+0xc]
            push ebp
            xor ebp, ebp
            test eax, eax
            jle L00547ad2
            push edi
            push esi
            push ebx
            mov ebx, dword ptr [esp + 0x18]
            xor esi, esi
    L0054782f:
            mov eax, dword ptr [rend+0x8]
            cmp byte ptr [ebp + eax], 0
            je L00547abb
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(1)
            fmul dword ptr [scache+0x10]
            fxch st(2)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [edx + 4]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fxch st(2)
            fmul dword ptr [scache+0x4]
            fxch st(1)
            fmul dword ptr [scache+0x14]
            fxch st(2)
            fmul dword ptr [scache+0x24]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [edx + 4]
            fxch st(2)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [edx + 8]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fxch st(2)
            fmul dword ptr [scache+0x8]
            fxch st(1)
            fmul dword ptr [scache+0x18]
            fxch st(2)
            fmul dword ptr [scache+0x28]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [edx + 0xc]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(1)
            fmul dword ptr [scache+0x1c]
            fxch st(2)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fxch st(1)
            fcomp st(1)
            fstp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 1
            jne L00547937
            xor dword ptr [edx], 0x20002
    L00547937:
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L00547950
            xor dword ptr [edx], 0x10001
    L00547950:
            fld dword ptr [edx + 8]
            fcomp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 1
            jne L00547963
            xor dword ptr [edx], 0x40004
    L00547963:
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L0054797c
            xor dword ptr [edx], 0x80008
    L0054797c:
            fld dword ptr [edx + 0xc]
            fcomp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 1
            jne L0054798f
            xor dword ptr [edx], 0x100010
    L0054798f:
            fld dword ptr [edx + 0xc]
            fld dword ptr [edx + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L005479a8
            xor dword ptr [edx], 0x200020
    L005479a8:
            mov eax, dword ptr [scache+0x110]
            test eax, eax
            je L00547a06
            xor ecx, ecx
            lea edi, [ebx + 0x438]
    L005479b9:
            cmp dword ptr [edi - 8], 0xe1
            jne L005479fd
            fld dword ptr [edi - 4]
            fld dword ptr [edx + 4]
            fld dword ptr [edi + 4]
            fld dword ptr [edx + 0xc]
            fld dword ptr [edi + 8]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [edx + 0x10]
            _emit 0xd9
        _emit 0x07
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [edx + 8]
            faddp st(1), st(0)
            fcomp dword ptr [rendererStateStoredDispatch+0x98]
            fnstsw ax
            test ah, 1
            je L005479fd
            mov eax, 0x400040
            shl eax, cl
            xor dword ptr [edx], eax
    L005479fd:
            inc ecx
            add edi, 0x14
            cmp ecx, 6
            jl L005479b9
    L00547a06:
            test dword ptr [edx], 0xfff
            jne L00547abb
            fld dword ptr [ebx + 0x5f8]
            fmul dword ptr [edx + 4]
            fdiv dword ptr [edx + 0x10]
            fadd dword ptr [ebx + 0x5b8]
            fstp dword ptr [edx + 0x14]
            fld dword ptr [ebx + 0x5fc]
            fmul dword ptr [edx + 8]
            fdiv dword ptr [edx + 0x10]
            fadd dword ptr [ebx + 0x5bc]
            fstp dword ptr [edx + 0x18]
            fld dword ptr [ebx + 0x600]
            fmul dword ptr [edx + 0xc]
            fdiv dword ptr [edx + 0x10]
            fadd dword ptr [ebx + 0x5c0]
            fld dword ptr [edx + 0x14]
            fxch st(1)
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [edx + 0x1c]
            fcomp dword ptr [scache+0xec]
            fnstsw ax
            test ah, 0x41
            jne L00547a72
            mov ecx, dword ptr [edx + 0x14]
            mov dword ptr [scache+0xec], ecx
    L00547a72:
            fld dword ptr [edx + 0x14]
            fcomp dword ptr [scache+0xe4]
            fnstsw ax
            test ah, 1
            je L00547a8a
            mov eax, dword ptr [edx + 0x14]
            mov dword ptr [scache+0xe4], eax
    L00547a8a:
            fld dword ptr [edx + 0x18]
            fcomp dword ptr [scache+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L00547aa3
            mov ecx, dword ptr [edx + 0x18]
            mov dword ptr [scache+0xf0], ecx
    L00547aa3:
            fld dword ptr [edx + 0x18]
            fcomp dword ptr [scache+0xe8]
            fnstsw ax
            test ah, 1
            je L00547abb
            mov eax, dword ptr [edx + 0x18]
            mov dword ptr [scache+0xe8], eax
    L00547abb:
            mov eax, dword ptr [rend+0x38]
            inc ebp
            add esi, 0x20
            add edx, 0x40
            cmp ebp, eax
            jl L0054782f
            pop ebx
            pop esi
            pop edi
    L00547ad2:
            pop ebp
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

// FUNCTION: CARMA2_HW 0x00547ae0
void C2_NAKED C2_HOOK_CDECL Vertex_TransformProjectOutcode(br_geometry *self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    brp_vertex* tvp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &rend.vertices[v].p, &scache.model_to_screen);
        OUTCODE_POINT(tvp->flags, (br_vector4*)(tvp->comp + C_X));

        if (!(tvp->flags & OUTCODES_ALL)) {
            PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        }
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x38]
            mov edx, dword ptr [rend+0xc]
            push ebp
            xor ebp, ebp
            test eax, eax
            jle L00547d3a
            push edi
            push esi
            push ebx
            mov ebx, dword ptr [esp + 0x18]
            xor esi, esi
    L00547aff:
            mov eax, dword ptr [rend+0x8]
            cmp byte ptr [ebp + eax], 0
            je L00547d23
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fld dword ptr [eax + esi]
            fxch st(2)
            fmul dword ptr [scache+0x10]
            fxch st(1)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [edx + 4]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fld dword ptr [eax + esi]
            fxch st(2)
            fmul dword ptr [scache+0x14]
            fxch st(1)
            fmul dword ptr [scache+0x24]
            fxch st(2)
            fmul dword ptr [scache+0x4]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [edx + 4]
            fxch st(2)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [edx + 8]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fld dword ptr [eax + esi]
            fxch st(2)
            fmul dword ptr [scache+0x18]
            fxch st(1)
            fmul dword ptr [scache+0x28]
            fxch st(2)
            fmul dword ptr [scache+0x8]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [edx + 0xc]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + esi + 4]
            fld dword ptr [eax + esi + 8]
            fld dword ptr [eax + esi]
            fxch st(2)
            fmul dword ptr [scache+0x1c]
            fxch st(1)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fxch st(1)
            fcomp st(1)
            fstp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 1
            jne L00547c07
            xor dword ptr [edx], 0x20002
    L00547c07:
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L00547c20
            xor dword ptr [edx], 0x10001
    L00547c20:
            fld dword ptr [edx + 8]
            fcomp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 1
            jne L00547c33
            xor dword ptr [edx], 0x40004
    L00547c33:
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L00547c4c
            xor dword ptr [edx], 0x80008
    L00547c4c:
            fld dword ptr [edx + 0xc]
            fcomp dword ptr [edx + 0x10]
            fnstsw ax
            test ah, 1
            jne L00547c5f
            xor dword ptr [edx], 0x100010
    L00547c5f:
            fld dword ptr [edx + 0xc]
            fld dword ptr [edx + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L00547c78
            xor dword ptr [edx], 0x200020
    L00547c78:
            mov eax, dword ptr [scache+0x110]
            test eax, eax
            je L00547cd6
            xor ecx, ecx
            lea edi, [ebx + 0x438]
    L00547c89:
            cmp dword ptr [edi - 8], 0xe1
            jne L00547ccd
            fld dword ptr [edi - 4]
            fld dword ptr [edx + 4]
            _emit 0xd9
        _emit 0x07
            fld dword ptr [edx + 8]
            fld dword ptr [edi + 8]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [edx + 0x10]
            fld dword ptr [edi + 4]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [edx + 0xc]
            faddp st(1), st(0)
            fcomp dword ptr [rendererStateStoredDispatch+0x98]
            fnstsw ax
            test ah, 1
            je L00547ccd
            mov eax, 0x400040
            shl eax, cl
            xor dword ptr [edx], eax
    L00547ccd:
            inc ecx
            add edi, 0x14
            cmp ecx, 6
            jl L00547c89
    L00547cd6:
            test dword ptr [edx], 0xfff
            jne L00547d23
            fld dword ptr [ebx + 0x5f8]
            fmul dword ptr [edx + 4]
            fdiv dword ptr [edx + 0x10]
            fadd dword ptr [ebx + 0x5b8]
            fstp dword ptr [edx + 0x14]
            fld dword ptr [ebx + 0x5fc]
            fmul dword ptr [edx + 8]
            fdiv dword ptr [edx + 0x10]
            fadd dword ptr [ebx + 0x5bc]
            fstp dword ptr [edx + 0x18]
            fld dword ptr [ebx + 0x600]
            fmul dword ptr [edx + 0xc]
            fdiv dword ptr [edx + 0x10]
            fadd dword ptr [ebx + 0x5c0]
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [edx + 0x1c]
    L00547d23:
            mov eax, dword ptr [rend+0x38]
            inc ebp
            add esi, 0x20
            add edx, 0x40
            cmp ebp, eax
            jl L00547aff
            pop ebx
            pop esi
            pop edi
    L00547d3a:
            pop ebp
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00547d40
void C2_NAKED C2_HOOK_CDECL Vertex_OS_TransformProjectBounds(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    brp_vertex* tvp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &rend.vertices[v].p, &scache.model_to_screen);
        PROJECT_VERTEX(tvp,tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);
    }
#else
    __asm {
            mov ecx, dword ptr [rend+0x38]
            mov eax, dword ptr [rend+0xc]
            push edi
            xor edi, edi
            test ecx, ecx
            jle L00547f1a
            push esi
            mov esi, dword ptr [esp + 0x10]
            lea ecx, [eax + 8]
            xor edx, edx
    L00547d60:
            mov eax, dword ptr [rend+0x8]
            cmp byte ptr [edi + eax], 0
            je L00547f05
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + edx + 4]
            fld dword ptr [eax + edx + 8]
            fld dword ptr [eax + edx]
            fxch st(2)
            fmul dword ptr [scache+0x10]
            fxch st(1)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [ecx - 4]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + edx + 4]
            fld dword ptr [eax + edx + 8]
            fld dword ptr [eax + edx]
            fxch st(2)
            fmul dword ptr [scache+0x14]
            fxch st(1)
            fmul dword ptr [scache+0x24]
            fxch st(2)
            fmul dword ptr [scache+0x4]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [ecx - 4]
            fxch st(2)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            _emit 0xd9
        _emit 0x19
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + edx + 4]
            fld dword ptr [eax + edx + 8]
            fld dword ptr [eax + edx]
            fxch st(2)
            fmul dword ptr [scache+0x18]
            fxch st(1)
            fmul dword ptr [scache+0x28]
            fxch st(2)
            fmul dword ptr [scache+0x8]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [ecx + 4]
            mov eax, dword ptr [rend+0x28]
            fld dword ptr [eax + edx + 4]
            fld dword ptr [eax + edx + 8]
            fld dword ptr [eax + edx]
            fxch st(2)
            fmul dword ptr [scache+0x1c]
            fxch st(1)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fld st(0)
            fld st(1)
            fxch st(1)
            fstp dword ptr [ecx + 8]
            fld dword ptr [esi + 0x5f8]
            fxch st(3)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            fadd dword ptr [esi + 0x5b8]
            fstp dword ptr [ecx + 0xc]
            fstp st(0)
            fld dword ptr [esi + 0x5fc]
            _emit 0xd8
        _emit 0x09
            fdiv dword ptr [ecx + 8]
            fadd dword ptr [esi + 0x5bc]
            fstp dword ptr [ecx + 0x10]
            fld dword ptr [esi + 0x600]
            fmul dword ptr [ecx + 4]
            fdiv dword ptr [ecx + 8]
            fadd dword ptr [esi + 0x5c0]
            fld dword ptr [ecx + 0xc]
            fxch st(1)
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [ecx + 0x14]
            fcomp dword ptr [scache+0xec]
            fnstsw ax
            test ah, 0x41
            jne L00547ebd
            mov eax, dword ptr [ecx + 0xc]
            mov dword ptr [scache+0xec], eax
    L00547ebd:
            fld dword ptr [ecx + 0xc]
            fcomp dword ptr [scache+0xe4]
            fnstsw ax
            test ah, 1
            je L00547ed5
            mov eax, dword ptr [ecx + 0xc]
            mov dword ptr [scache+0xe4], eax
    L00547ed5:
            fld dword ptr [ecx + 0x10]
            fcomp dword ptr [scache+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L00547eed
            mov eax, dword ptr [ecx + 0x10]
            mov dword ptr [scache+0xf0], eax
    L00547eed:
            fld dword ptr [ecx + 0x10]
            fcomp dword ptr [scache+0xe8]
            fnstsw ax
            test ah, 1
            je L00547f05
            mov eax, dword ptr [ecx + 0x10]
            mov dword ptr [scache+0xe8], eax
    L00547f05:
            mov eax, dword ptr [rend+0x38]
            inc edi
            add edx, 0x20
            add ecx, 0x40
            cmp edi, eax
            jl L00547d60
            pop esi
    L00547f1a:
            pop edi
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00547f20
void C2_NAKED C2_HOOK_CDECL Vertex_OS_TransformProject(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    brp_vertex* tvp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &rend.vertices[v].p, &scache.model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
    }
#else
    __asm {
            mov ecx, dword ptr [rend+0x38]
            mov eax, dword ptr [rend+0xc]
            push edi
            xor edi, edi
            test ecx, ecx
            jle L0054809a
            mov edx, dword ptr [esp + 0xc]
            push esi
            lea ecx, [eax + 4]
            xor eax, eax
    L00547f40:
            mov esi, dword ptr [rend+0x8]
            cmp byte ptr [edi + esi], 0
            je L00548084
            mov esi, dword ptr [rend+0x28]
            fld dword ptr [esi + eax]
            fld dword ptr [esi + eax + 4]
            fld dword ptr [esi + eax + 8]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(1)
            fmul dword ptr [scache+0x10]
            fxch st(2)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            _emit 0xd9
        _emit 0x19
            mov esi, dword ptr [rend+0x28]
            fld dword ptr [esi + eax]
            fld dword ptr [esi + eax + 4]
            fld dword ptr [esi + eax + 8]
            fxch st(2)
            fmul dword ptr [scache+0x4]
            fxch st(1)
            fmul dword ptr [scache+0x14]
            fxch st(2)
            fmul dword ptr [scache+0x24]
            fxch st(2)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [ecx + 4]
            mov esi, dword ptr [rend+0x28]
            fld dword ptr [esi + eax]
            fld dword ptr [esi + eax + 4]
            fld dword ptr [esi + eax + 8]
            fxch st(2)
            fmul dword ptr [scache+0x8]
            fxch st(1)
            fmul dword ptr [scache+0x18]
            fxch st(2)
            fmul dword ptr [scache+0x28]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [ecx + 4]
            fxch st(2)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [ecx + 8]
            mov esi, dword ptr [rend+0x28]
            fld dword ptr [esi + eax]
            fld dword ptr [esi + eax + 4]
            fld dword ptr [esi + eax + 8]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(1)
            fmul dword ptr [scache+0x1c]
            fxch st(2)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fst dword ptr [ecx + 0xc]
            fld dword ptr [edx + 0x5f8]
            fxch st(3)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            fadd dword ptr [edx + 0x5b8]
            fstp dword ptr [ecx + 0x10]
            fld dword ptr [edx + 0x5fc]
            fxch st(1)
            fmulp st(1), st(0)
            fdiv dword ptr [ecx + 0xc]
            fadd dword ptr [edx + 0x5bc]
            fstp dword ptr [ecx + 0x14]
            fld dword ptr [edx + 0x600]
            fmul dword ptr [ecx + 8]
            fdiv dword ptr [ecx + 0xc]
            fadd dword ptr [edx + 0x5c0]
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [ecx + 0x18]
    L00548084:
            mov esi, dword ptr [rend+0x38]
            inc edi
            add eax, 0x20
            add ecx, 0x40
            cmp edi, esi
            jl L00547f40
            pop esi
    L0054809a:
            pop edi
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005480a0
void C2_NAKED C2_HOOK_CDECL Vertex_OS_TransformProjectBoundsSurf(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &scache.model_to_screen);
        PROJECT_VERTEX(tvp,tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, scache.colour, tvp->comp);
        }
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [rend+0x38]
            push esi
            mov esi, dword ptr [rend+0xc]
            xor ecx, ecx
            push edi
            mov edi, dword ptr [rend+0x28]
            test eax, eax
            mov dword ptr [esp + 8], ecx
            jle L005482ad
            push ebp
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
    L005480c8:
            mov eax, dword ptr [rend+0x8]
            cmp byte ptr [ecx + eax], 0
            je L00548293
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            _emit 0xd9
        _emit 0x07
            fxch st(2)
            fmul dword ptr [scache+0x10]
            fxch st(1)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [esi + 4]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            _emit 0xd9
        _emit 0x07
            fxch st(2)
            fmul dword ptr [scache+0x14]
            fxch st(1)
            fmul dword ptr [scache+0x24]
            fld dword ptr [esi + 4]
            fxch st(3)
            fmul dword ptr [scache+0x4]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [esi + 8]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            _emit 0xd9
        _emit 0x07
            fxch st(2)
            fmul dword ptr [scache+0x18]
            fxch st(1)
            fmul dword ptr [scache+0x28]
            fld dword ptr [esi + 8]
            fxch st(3)
            fmul dword ptr [scache+0x8]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [esi + 0xc]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            _emit 0xd9
        _emit 0x07
            fxch st(2)
            fmul dword ptr [scache+0x1c]
            fxch st(1)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fst dword ptr [esi + 0x10]
            fld dword ptr [ebx + 0x5f8]
            fxch st(3)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            fadd dword ptr [ebx + 0x5b8]
            fstp dword ptr [esi + 0x14]
            fld dword ptr [ebx + 0x5fc]
            fxch st(1)
            fmulp st(1), st(0)
            fdiv dword ptr [esi + 0x10]
            fadd dword ptr [ebx + 0x5bc]
            fstp dword ptr [esi + 0x18]
            fld dword ptr [ebx + 0x600]
            fmul dword ptr [esi + 0xc]
            fdiv dword ptr [esi + 0x10]
            fadd dword ptr [ebx + 0x5c0]
            fld dword ptr [esi + 0x14]
            fxch st(1)
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [esi + 0x1c]
            fcomp dword ptr [scache+0xec]
            fnstsw ax
            test ah, 0x41
            jne L00548200
            mov edx, dword ptr [esi + 0x14]
            mov dword ptr [scache+0xec], edx
    L00548200:
            fld dword ptr [esi + 0x14]
            fcomp dword ptr [scache+0xe4]
            fnstsw ax
            test ah, 1
            je L00548218
            mov eax, dword ptr [esi + 0x14]
            mov dword ptr [scache+0xe4], eax
    L00548218:
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [scache+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L00548231
            mov edx, dword ptr [esi + 0x18]
            mov dword ptr [scache+0xf0], edx
    L00548231:
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [scache+0xe8]
            fnstsw ax
            test ah, 1
            je L00548249
            mov eax, dword ptr [esi + 0x18]
            mov dword ptr [scache+0xe8], eax
    L00548249:
            mov eax, dword ptr [ebx + 0x724]
            mov dword ptr [esp + 0x1c], 0
            test eax, eax
            jle L00548293
            lea ebp, [ebx + 0x728]
    L00548261:
            mov ecx, dword ptr [scache+0xe0]
            push esi
            lea eax, [edi + 0x14]
            push ecx
            push eax
            lea eax, [edi + 0xc]
            push eax
            push edi
            push ebx
            call dword ptr [ebp]
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [ebx + 0x724]
            add esp, 0x18
            inc eax
            add ebp, 4
            cmp eax, ecx
            mov dword ptr [esp + 0x1c], eax
            jl L00548261
            mov ecx, dword ptr [esp + 0x10]
    L00548293:
            mov eax, dword ptr [rend+0x38]
            inc ecx
            add edi, 0x20
            add esi, 0x40
            cmp ecx, eax
            mov dword ptr [esp + 0x10], ecx
            jl L005480c8
            pop ebx
            pop ebp
    L005482ad:
            pop edi
            pop esi
            pop ecx
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

// FUNCTION: CARMA2_HW 0x005482c0
void C2_NAKED C2_HOOK_CDECL Vertex_OS_TransformProjectSurf(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &scache.model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, scache.colour, tvp->comp);
        }
    }
#else
    __asm {
            push ecx
            mov ecx, dword ptr [rend+0x38]
            push esi
            mov esi, dword ptr [rend+0x28]
            xor eax, eax
            push edi
            mov edi, dword ptr [rend+0xc]
            test ecx, ecx
            mov dword ptr [esp + 8], eax
            jle L0054846c
            push ebp
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
    L005482e9:
            mov ecx, dword ptr [rend+0x8]
            cmp byte ptr [eax + ecx], 0
            je L00548451
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x10]
            fxch st(1)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [edi + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x14]
            fxch st(1)
            fmul dword ptr [scache+0x24]
            fld dword ptr [edi + 4]
            fxch st(3)
            fmul dword ptr [scache+0x4]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [edi + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x18]
            fxch st(1)
            fmul dword ptr [scache+0x28]
            fld dword ptr [edi + 8]
            fxch st(3)
            fmul dword ptr [scache+0x8]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [edi + 0xc]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x1c]
            fxch st(1)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fst dword ptr [edi + 0x10]
            fld dword ptr [ebx + 0x5f8]
            fxch st(3)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            mov dword ptr [esp + 0x1c], 0
            fadd dword ptr [ebx + 0x5b8]
            fstp dword ptr [edi + 0x14]
            fld dword ptr [ebx + 0x5fc]
            fxch st(1)
            fmulp st(1), st(0)
            fdiv dword ptr [edi + 0x10]
            fadd dword ptr [ebx + 0x5bc]
            fstp dword ptr [edi + 0x18]
            fld dword ptr [ebx + 0x600]
            fmul dword ptr [edi + 0xc]
            fdiv dword ptr [edi + 0x10]
            fadd dword ptr [ebx + 0x5c0]
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [edi + 0x1c]
            mov ecx, dword ptr [ebx + 0x724]
            test ecx, ecx
            jle L00548451
            lea ebp, [ebx + 0x728]
    L0054841f:
            mov edx, dword ptr [scache+0xe0]
            push edi
            lea eax, [esi + 0x14]
            push edx
            push eax
            lea eax, [esi + 0xc]
            push eax
            push esi
            push ebx
            call dword ptr [ebp]
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [ebx + 0x724]
            add esp, 0x18
            inc eax
            add ebp, 4
            cmp eax, ecx
            mov dword ptr [esp + 0x1c], eax
            jl L0054841f
            mov eax, dword ptr [esp + 0x10]
    L00548451:
            mov ecx, dword ptr [rend+0x38]
            inc eax
            add esi, 0x20
            add edi, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L005482e9
            pop ebx
            pop ebp
    L0054846c:
            pop edi
            pop esi
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00548470
void C2_NAKED C2_HOOK_CDECL Vertex_OS_TransformProjectBoundsGeom(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &scache.model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, rend.vertex_colours[v], tvp->comp);
        }
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [rend+0x38]
            push esi
            mov esi, dword ptr [rend+0x28]
            xor ecx, ecx
            push edi
            mov edi, dword ptr [rend+0xc]
            test eax, eax
            mov dword ptr [esp + 8], ecx
            jle L00548680
            push ebp
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
    L00548498:
            mov eax, dword ptr [rend+0x8]
            cmp byte ptr [ecx + eax], 0
            je L00548666
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x10]
            fxch st(1)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [edi + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x14]
            fxch st(1)
            fmul dword ptr [scache+0x24]
            fld dword ptr [edi + 4]
            fxch st(3)
            fmul dword ptr [scache+0x4]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [edi + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x18]
            fxch st(1)
            fmul dword ptr [scache+0x28]
            fld dword ptr [edi + 8]
            fxch st(3)
            fmul dword ptr [scache+0x8]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [edi + 0xc]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x1c]
            fxch st(1)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fst dword ptr [edi + 0x10]
            fld dword ptr [ebx + 0x5f8]
            fxch st(3)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            fadd dword ptr [ebx + 0x5b8]
            fstp dword ptr [edi + 0x14]
            fld dword ptr [ebx + 0x5fc]
            fxch st(1)
            fmulp st(1), st(0)
            fdiv dword ptr [edi + 0x10]
            fadd dword ptr [ebx + 0x5bc]
            fstp dword ptr [edi + 0x18]
            fld dword ptr [ebx + 0x600]
            fmul dword ptr [edi + 0xc]
            fdiv dword ptr [edi + 0x10]
            fadd dword ptr [ebx + 0x5c0]
            fld dword ptr [edi + 0x14]
            fxch st(1)
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [edi + 0x1c]
            fcomp dword ptr [scache+0xec]
            fnstsw ax
            test ah, 0x41
            jne L005485d0
            mov edx, dword ptr [edi + 0x14]
            mov dword ptr [scache+0xec], edx
    L005485d0:
            fld dword ptr [edi + 0x14]
            fcomp dword ptr [scache+0xe4]
            fnstsw ax
            test ah, 1
            je L005485e8
            mov eax, dword ptr [edi + 0x14]
            mov dword ptr [scache+0xe4], eax
    L005485e8:
            fld dword ptr [edi + 0x18]
            fcomp dword ptr [scache+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L00548601
            mov edx, dword ptr [edi + 0x18]
            mov dword ptr [scache+0xf0], edx
    L00548601:
            fld dword ptr [edi + 0x18]
            fcomp dword ptr [scache+0xe8]
            fnstsw ax
            test ah, 1
            je L00548619
            mov eax, dword ptr [edi + 0x18]
            mov dword ptr [scache+0xe8], eax
    L00548619:
            mov eax, dword ptr [ebx + 0x724]
            mov dword ptr [esp + 0x1c], 0
            test eax, eax
            jle L00548666
            lea ebp, [ebx + 0x728]
    L00548631:
            mov edx, dword ptr [rend+0x30]
            push edi
            mov eax, dword ptr [edx + ecx*4]
            push eax
            lea eax, [esi + 0x14]
            push eax
            lea eax, [esi + 0xc]
            push eax
            push esi
            push ebx
            call dword ptr [ebp]
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [ebx + 0x724]
            add esp, 0x18
            inc eax
            add ebp, 4
            cmp eax, ecx
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x1c], eax
            jl L00548631
    L00548666:
            mov eax, dword ptr [rend+0x38]
            inc ecx
            add esi, 0x20
            add edi, 0x40
            cmp ecx, eax
            mov dword ptr [esp + 0x10], ecx
            jl L00548498
            pop ebx
            pop ebp
    L00548680:
            pop edi
            pop esi
            pop ecx
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

// FUNCTION: CARMA2_HW 0x00548690
void C2_NAKED C2_HOOK_CDECL Vertex_OS_TransformProjectGeom(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &scache.model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, rend.vertex_colours[v], tvp->comp);
        }
    }
#else
    __asm {
            push ecx
            mov ecx, dword ptr [rend+0x38]
            push esi
            mov esi, dword ptr [rend+0x28]
            xor eax, eax
            push edi
            mov edi, dword ptr [rend+0xc]
            test ecx, ecx
            mov dword ptr [esp + 8], eax
            jle L0054883f
            push ebp
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
    L005486b9:
            mov ecx, dword ptr [rend+0x8]
            cmp byte ptr [eax + ecx], 0
            je L00548824
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x10]
            fxch st(1)
            fmul dword ptr [scache+0x20]
            fxch st(2)
            fmul dword ptr [scache]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x30]
            fstp dword ptr [edi + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x14]
            fxch st(1)
            fmul dword ptr [scache+0x24]
            fld dword ptr [edi + 4]
            fxch st(3)
            fmul dword ptr [scache+0x4]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x34]
            fstp dword ptr [edi + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x18]
            fxch st(1)
            fmul dword ptr [scache+0x28]
            fld dword ptr [edi + 8]
            fxch st(3)
            fmul dword ptr [scache+0x8]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x38]
            fstp dword ptr [edi + 0xc]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmul dword ptr [scache+0x1c]
            fxch st(1)
            fmul dword ptr [scache+0x2c]
            fxch st(2)
            fmul dword ptr [scache+0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [scache+0x3c]
            fst dword ptr [edi + 0x10]
            fld dword ptr [ebx + 0x5f8]
            fxch st(3)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            mov dword ptr [esp + 0x1c], 0
            fadd dword ptr [ebx + 0x5b8]
            fstp dword ptr [edi + 0x14]
            fld dword ptr [ebx + 0x5fc]
            fxch st(1)
            fmulp st(1), st(0)
            fdiv dword ptr [edi + 0x10]
            fadd dword ptr [ebx + 0x5bc]
            fstp dword ptr [edi + 0x18]
            fld dword ptr [ebx + 0x600]
            fmul dword ptr [edi + 0xc]
            fdiv dword ptr [edi + 0x10]
            fadd dword ptr [ebx + 0x5c0]
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [edi + 0x1c]
            mov ecx, dword ptr [ebx + 0x724]
            test ecx, ecx
            jle L00548824
            lea ebp, [ebx + 0x728]
    L005487ef:
            mov edx, dword ptr [rend+0x30]
            push edi
            mov eax, dword ptr [edx + eax*4]
            push eax
            lea eax, [esi + 0x14]
            push eax
            lea eax, [esi + 0xc]
            push eax
            push esi
            push ebx
            call dword ptr [ebp]
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [ebx + 0x724]
            add esp, 0x18
            inc eax
            add ebp, 4
            mov dword ptr [esp + 0x1c], eax
            cmp eax, ecx
            mov eax, dword ptr [esp + 0x10]
            jl L005487ef
    L00548824:
            mov ecx, dword ptr [rend+0x38]
            inc eax
            add esi, 0x20
            add edi, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L005486b9
            pop ebx
            pop ebp
    L0054883f:
            pop edi
            pop esi
            pop ecx
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

// FUNCTION: CARMA2_HW 0x00548850
void C2_NAKED C2_HOOK_CDECL Vertex_SurfaceComponentsSurf(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &scache.model_to_screen);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, scache.colour, tvp->comp);
        }
    }
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [rend+0xc]
            mov ecx, dword ptr [rend+0x38]
            mov dword ptr [esp], eax
            xor eax, eax
            push ebp
            mov ebp, dword ptr [rend+0x28]
            test ecx, ecx
            mov dword ptr [esp + 8], eax
            jle L005488e4
            push edi
            mov edi, dword ptr [esp + 0x18]
            push esi
            push ebx
    L0054887a:
            mov ecx, dword ptr [rend+0x8]
            cmp byte ptr [eax + ecx], 0
            je L005488c4
            mov ecx, dword ptr [edi + 0x724]
            xor ebx, ebx
            test ecx, ecx
            jle L005488c4
            lea esi, [edi + 0x728]
    L00548898:
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [scache+0xe0]
            push edx
            push eax
            lea eax, [ebp + 0x14]
            push eax
            lea eax, [ebp + 0xc]
            push eax
            push ebp
            push edi
            call dword ptr [esi]
            mov eax, dword ptr [edi + 0x724]
            add esp, 0x18
            inc ebx
            add esi, 4
            cmp ebx, eax
            jl L00548898
            mov eax, dword ptr [esp + 0x14]
    L005488c4:
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [rend+0x38]
            inc eax
            add ebp, 0x20
            add edx, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], edx
            jl L0054887a
            pop ebx
            pop esi
            pop edi
    L005488e4:
            pop ebp
            add esp, 8
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

// FUNCTION: CARMA2_HW 0x005488f0
void C2_NAKED C2_HOOK_CDECL Vertex_SurfaceComponentsTwoSidedSurf(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;
    br_vector3 rev_normal;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        C2_HOOK_BUG_ON(TVDIR_FRONT != 0x80000000);

        if (tvp->flags & TVDIR_FRONT) {
            for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, scache.colour, tvp->comp);
            }
        } else {
            BrVector3Negate(&rev_normal, &vp->n);

            for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &rev_normal, scache.colour, tvp->comp);
            }
        }
    }
#else
    __asm {
            sub esp, 0x14
            mov eax, dword ptr [rend+0xc]
            mov ecx, dword ptr [rend+0x38]
            mov dword ptr [esp], eax
            xor eax, eax
            push edi
            mov edi, dword ptr [rend+0x28]
            test ecx, ecx
            mov dword ptr [esp + 8], eax
            jle L005489f9
            push esi
            mov esi, dword ptr [esp + 0x24]
            push ebp
            push ebx
    L0054891e:
            mov ecx, dword ptr [rend+0x8]
            cmp byte ptr [eax + ecx], 0
            je L005489d1
            mov edx, dword ptr [esp + 0x10]
            test dword ptr [edx], 0x80000000
            je L0054897b
            mov eax, dword ptr [esi + 0x724]
            xor ebp, ebp
            test eax, eax
            jle L005489d1
            lea ebx, [esi + 0x728]
    L00548950:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [scache+0xe0]
            push eax
            lea eax, [edi + 0x14]
            push ecx
            push eax
            lea eax, [edi + 0xc]
            push eax
            push edi
            push esi
            call dword ptr [ebx]
            mov eax, dword ptr [esi + 0x724]
            add esp, 0x18
            inc ebp
            add ebx, 4
            cmp ebp, eax
            jl L00548950
            jmp L005489d1
    L0054897b:
            fld dword ptr [edi + 0x14]
            fchs
            fstp dword ptr [esp + 0x18]
            fld dword ptr [edi + 0x18]
            mov eax, dword ptr [esi + 0x724]
            xor ebp, ebp
            fchs
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [edi + 0x1c]
            fchs
            fstp dword ptr [esp + 0x20]
            test eax, eax
            jle L005489d1
            lea ebx, [esi + 0x728]
    L005489a8:
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [scache+0xe0]
            push edx
            push eax
            lea ecx, [esp + 0x20]
            lea eax, [edi + 0xc]
            push ecx
            push eax
            push edi
            push esi
            call dword ptr [ebx]
            mov eax, dword ptr [esi + 0x724]
            add esp, 0x18
            inc ebp
            add ebx, 4
            cmp ebp, eax
            jl L005489a8
    L005489d1:
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [rend+0x38]
            inc eax
            add edi, 0x20
            add edx, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], edx
            jl L0054891e
            pop ebx
            pop ebp
            pop esi
    L005489f9:
            pop edi
            add esp, 0x14
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00548a00
void C2_NAKED C2_HOOK_CDECL Vertex_SurfaceComponentsGeom(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    brp_vertex* tvp;
    fmt_vertex* vp;
    int v;
    int i;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &scache.model_to_screen);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, rend.vertex_colours[v], tvp->comp);
        }
    }
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [rend+0xc]
            mov ecx, dword ptr [rend+0x38]
            mov dword ptr [esp + 4], eax
            xor eax, eax
            push ebp
            mov ebp, dword ptr [rend+0x28]
            test ecx, ecx
            mov dword ptr [esp + 4], eax
            jle L00548a98
            push edi
            mov edi, dword ptr [esp + 0x18]
            push esi
            push ebx
    L00548a2a:
            mov ecx, dword ptr [rend+0x8]
            cmp byte ptr [eax + ecx], 0
            je L00548a78
            mov ecx, dword ptr [edi + 0x724]
            xor ebx, ebx
            test ecx, ecx
            jle L00548a78
            lea esi, [edi + 0x728]
    L00548a48:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [rend+0x30]
            push edx
            mov edx, dword ptr [ecx + eax*4]
            lea eax, [ebp + 0x14]
            push edx
            push eax
            lea eax, [ebp + 0xc]
            push eax
            push ebp
            push edi
            call dword ptr [esi]
            mov eax, dword ptr [edi + 0x724]
            add esp, 0x18
            inc ebx
            add esi, 4
            cmp ebx, eax
            mov eax, dword ptr [esp + 0x10]
            jl L00548a48
    L00548a78:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [rend+0x38]
            inc eax
            add ebp, 0x20
            add edx, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], edx
            jl L00548a2a
            pop ebx
            pop esi
            pop edi
    L00548a98:
            pop ebp
            add esp, 8
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00548aa0
void C2_NAKED C2_HOOK_CDECL Vertex_SurfaceComponentsTwoSidedGeom(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    brp_vertex* tvp;
    fmt_vertex* vp;
    int v;
    int i;
    br_vector3 rev_normal;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];
        vp = &rend.vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        if (tvp->flags & TVDIR_FRONT) {
            for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, rend.vertex_colours[v], tvp->comp);
            }
        } else {
            BrVector3Negate(&rev_normal, &vp->n);

            for (i= 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &rev_normal, rend.vertex_colours[v], tvp->comp);
            }
        }
    }
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [rend+0xc]
            mov ecx, dword ptr [rend+0x38]
            mov dword ptr [esp], eax
            xor eax, eax
            push edi
            mov edi, dword ptr [rend+0x28]
            cmp ecx, eax
            mov dword ptr [esp + 0xc], eax
            jle L00548bc7
            push esi
            mov esi, dword ptr [esp + 0x28]
            push ebp
            push ebx
            mov dword ptr [esp + 0x14], eax
    L00548ad2:
            mov ecx, dword ptr [rend+0x8]
            cmp byte ptr [eax + ecx], 0
            je L00548b94
            mov edx, dword ptr [esp + 0x10]
            test dword ptr [edx], 0x80000000
            je L00548b36
            mov eax, dword ptr [esi + 0x724]
            xor ebp, ebp
            test eax, eax
            jle L00548b94
            lea ebx, [esi + 0x728]
    L00548b04:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [rend+0x30]
            mov edx, dword ptr [esp + 0x14]
            push eax
            mov eax, dword ptr [ecx + edx]
            push eax
            lea eax, [edi + 0x14]
            push eax
            lea eax, [edi + 0xc]
            push eax
            push edi
            push esi
            call dword ptr [ebx]
            mov eax, dword ptr [esi + 0x724]
            add esp, 0x18
            inc ebp
            add ebx, 4
            cmp ebp, eax
            jl L00548b04
            jmp L00548b94
    L00548b36:
            fld dword ptr [edi + 0x14]
            fchs
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [edi + 0x18]
            mov eax, dword ptr [esi + 0x724]
            xor ebp, ebp
            fchs
            fstp dword ptr [esp + 0x20]
            fld dword ptr [edi + 0x1c]
            fchs
            fstp dword ptr [esp + 0x24]
            test eax, eax
            jle L00548b94
            lea ebx, [esi + 0x728]
    L00548b63:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [rend+0x30]
            mov eax, dword ptr [esp + 0x14]
            push ecx
            mov ecx, dword ptr [edx + eax]
            lea edx, [esp + 0x20]
            push ecx
            lea eax, [edi + 0xc]
            push edx
            push eax
            push edi
            push esi
            call dword ptr [ebx]
            mov eax, dword ptr [esi + 0x724]
            add esp, 0x18
            inc ebp
            add ebx, 4
            cmp ebp, eax
            jl L00548b63
    L00548b94:
            mov eax, dword ptr [esp + 0x18]
            mov ebp, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [rend+0x38]
            inc eax
            add ebp, 4
            add edi, 0x20
            add edx, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x14], ebp
            mov dword ptr [esp + 0x10], edx
            jl L00548ad2
            pop ebx
            pop ebp
            pop esi
    L00548bc7:
            pop edi
            add esp, 0x18
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00548bd0
void C2_NAKED C2_HOOK_CDECL Vertex_ForceFront(br_geometry* self, br_soft_renderer* renderer) {
#ifndef CARPOCALYPSE2_MATCHING
    brp_vertex* tvp;
    int v;

    for (v = 0; v < rend.nvertices; v++) {
        tvp = &rend.temp_vertices[v];

        if (rend.vertex_counts[v] == 0) {
            continue;
        }

        tvp->comp[C_SZ] = 0.f;
        tvp->comp[C_Z] = tvp->comp[C_W] - BR_SCALAR_EPSILON;
    }
#else
    __asm {
            mov eax, dword ptr [rend+0xc]
            push esi
            mov esi, dword ptr [rend+0x38]
            xor edx, edx
            xor ecx, ecx
            cmp esi, edx
            jle L00548c0e
            add eax, 0x10
    L00548be7:
            mov esi, dword ptr [rend+0x8]
            cmp byte ptr [ecx + esi], dl
            je L00548c00
            _emit 0xd9
        _emit 0x00
            fsub dword ptr [rendererStateStoredDispatch+0x9c]
            mov dword ptr [eax + 0xc], edx
            fstp dword ptr [eax - 4]
    L00548c00:
            mov esi, dword ptr [rend+0x38]
            inc ecx
            add eax, 0x40
            cmp ecx, esi
            jl L00548be7
    L00548c0e:
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005476a0
void C2_HOOK_CDECL ScratchFree(br_geometry* self, br_soft_renderer* renderer) {

    BrScratchFree(rend.scratch);
    rend.scratch = NULL;
}

// FUNCTION: CARMA2_HW 0x005476c0
void C2_HOOK_CDECL Vertex_ClearFlags(br_geometry* self, br_soft_renderer* renderer) {
    int v;

    for (v = 0; v < rend.nvertices; v++) {
        rend.temp_vertices[v].flags = OUTCODES_NOT;
    }
}

// FUNCTION: CARMA2_HW 0x005476f0
void C2_HOOK_STDCALL VertexGeometryFns(br_soft_renderer* renderer, geometry_fn* prim_outcode) {

    C2_HOOK_BUG_ON(ENBL_BOUNDS != 0x10);
    if (renderer->state.enable.flags & ENBL_BOUNDS) {
        GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_TransformProjectOutcodeBounds);
    } else {
        GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_TransformProjectOutcode);
    }

    if (prim_outcode != NULL) {
        GeometryFunctionAdd(renderer, prim_outcode);
    }

    if (renderer->state.cache.nvertex_fns != 0) {
        if (renderer->state.cull.type == BRT_TWO_SIDED) {
            if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedGeom);
            } else {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedSurf);
            }
        } else {
            if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsGeom);
            } else {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsSurf);
            }
        }
    }

    if (renderer->state.cache.nvertex_fns != 0) {
        if (renderer->state.cull.type == BRT_TWO_SIDED) {
            if (renderer->state.enable.flags & ENBL_BOUNDS) {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBounds);
            } else {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProject);
            }

            if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedGeom);
            } else {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedSurf);
            }
        } else {
            if (renderer->state.enable.flags & ENBL_BOUNDS) {
                if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBoundsGeom);
                } else {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBoundsSurf);
                }
            } else {
                if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectGeom);
                } else {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectSurf);
                }
            }
        }
    } else {
        if (renderer->state.enable.flags & ENBL_BOUNDS) {
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBounds);
        } else {
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProject);
        }
    }

    if (renderer->state.surface.force_front) {
        GeometryFunctionBothAdd(renderer, (geometry_fn*)Vertex_ForceFront);
    }
}