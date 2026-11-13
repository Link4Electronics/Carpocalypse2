#include "faceops.h"

#include "clip.h"
#include "setup.h"
#include "subdiv.h"

#include "c2_string.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL averageVertices();
extern void C2_HOOK_CDECL averageVerticesOnScreen();
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
extern char rendererStateStoredTemplateEntries[];
extern static_cache_soft scache;
extern void C2_HOOK_STDCALL triangleSubdivideOnScreen(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);
#endif
// FUNCTION: CARMA2_HW 0x00545120
void C2_HOOK_CDECL OpTriangleClip(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    brp_vertex clip_in[3];
    int nclipped;
    brp_vertex* clipped;

    C2_HOOK_BUG_ON(sizeof(brp_vertex) != 0x40);

    memcpy(&clip_in[0], v0, sizeof(brp_vertex));
    memcpy(&clip_in[1], v1, sizeof(brp_vertex));//
    memcpy(&clip_in[2], v2, sizeof(brp_vertex));

    clipped = FaceClip(rend.renderer, clip_in, rend.renderer->state.cache.clip_slots, *fp_edges, 3, &nclipped);
    if (clipped != NULL) {
        ClippedRenderTriangles(rend.renderer, block->chain, clipped, nclipped, fp_vertices, fp_edges);
    }
}

// FUNCTION: CARMA2_HW 0x005451c0
void C2_HOOK_CDECL OpTriangleClipConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, temp_face_soft* tfp) {
    brp_vertex clip_in[3];
    int nclipped,i;
    brp_vertex* clipped;
    fmt_vertex* vp;
    br_colour colour = scache.colour;
    br_vector3 rev_normal;

    clip_in[0] = *v0;
    clip_in[1] = *v1;
    clip_in[2] = *v2;

    clipped = FaceClip(rend.renderer, clip_in, rend.renderer->state.cache.clip_slots, tfp->codes, 3, &nclipped);
    if (clipped != NULL) {
        vp = &rend.vertices[face->vertices[0]];

        if (rend.renderer->state.surface.colour_source == BRT_GEOMETRY) {
            colour = rend.face_colours[face - rend.faces];
        }
        if (tfp->flag & TFF_REVERSED) {
            BrVector3Negate(&rev_normal, &face->eqn);

            for (i = 0; i < rend.renderer->state.cache.nconstant_fns; i++) {
                rend.renderer->state.cache.constant_fns[i]((br_renderer*)rend.renderer, &vp->p, &vp->map, &rev_normal, colour, clipped->comp);
            }

        } else {
            for (i = 0; i < rend.renderer->state.cache.nconstant_fns; i++) {
                rend.renderer->state.cache.constant_fns[i]((br_renderer*)rend.renderer, &vp->p, &vp->map, (br_vector3*)&face->eqn, colour, clipped->comp);
            }
        }

        /* FIXME: are last argument types correct? */
        ClippedRenderTriangles(rend.renderer, block->chain, clipped, nclipped, face->vertices, &tfp->codes);
    }
}

// FUNCTION: CARMA2_HW 0x00545370
void C2_HOOK_CDECL OpTriangleConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, br_uint_16* fp_vertices) {
    br_colour colour;
    fmt_vertex* vp;
    int i;

    colour = scache.colour;
    vp = &rend.vertices[face->vertices[0]];

    if (rend.renderer->state.surface.colour_source == BRT_GEOMETRY) {
        colour = rend.face_colours[face - rend.faces];
    }

    for (i = 0; i < rend.renderer->state.cache.nconstant_fns; i++) {
        rend.renderer->state.cache.constant_fns[i]((br_renderer*)rend.renderer, &vp->p, &vp->map, (br_vector3*)&face->eqn, colour, v0->comp);
    }

    /* FIXME: what is type of last argument? */
    block->chain->render(block->chain, v0, v1, v2, face, fp_vertices);
}

// FUNCTION: CARMA2_HW 0x00545430
void C2_HOOK_CDECL OpTriangleTwoSidedConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, temp_face_soft* tfp) {
    fmt_vertex* vp;
    br_colour colour;
    br_vector3 rev_normal;
    int i;

    vp = &rend.vertices[face->vertices[0]];
    colour = scache.colour;

    if (rend.renderer->state.surface.colour_source == BRT_GEOMETRY) {
        colour = rend.face_colours[face - rend.faces];
    }
    if (tfp->flag & TFF_REVERSED) {
        BrVector3Negate(&rev_normal, &face->eqn);

        for (i = 0; i < rend.renderer->state.cache.nconstant_fns; i++) {
            rend.renderer->state.cache.constant_fns[i]((br_renderer*)rend.renderer, &vp->p, &vp->map, &rev_normal, colour, v0->comp);
        }

    } else {
        for (i = 0; i < rend.renderer->state.cache.nconstant_fns; i++) {
            rend.renderer->state.cache.constant_fns[i]((br_renderer*)rend.renderer, &vp->p, &vp->map, (br_vector3*)&face->eqn, colour, v0->comp);
        }
    }
    block->chain->render(block->chain, v0, v1, v2, face, tfp);
}

// FUNCTION: CARMA2_HW 0x00545560
void C2_HOOK_CDECL OpTriangleMappingWrapFix(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, undefined4* param_5, undefined4* param_6) {
    br_scalar scale;
    br_scalar half;
    br_scalar d0, d1, d2;
    brp_vertex fixed[3];

    scale = (br_scalar)fabs(rend.renderer->state.cache.comp_scales[C_U]);
    half = scale / 2.f;
    d0 = v1->comp[C_U] - v0->comp[C_U];
    d1 = v2->comp[C_U] - v1->comp[C_U];
    d2 = v0->comp[C_U] - v2->comp[C_U];

    if (d0 > half || d0 < -half || d1 > half || d1 < -half || d2 > half || d2 < -half) {
        if (d0 > half || d2 <-half) {
            fixed[0] = *v0;
            fixed[0].comp[C_U] += scale;
            v0 = &fixed[0];
        }
        if (d0 < -half || d1 > half) {
            fixed[1] = *v1;
            fixed[1].comp[C_U] += scale;
            v1 = &fixed[1];
        }
        if (d1 < -half || d2 > half) {
            fixed[2] = *v2;
            fixed[2].comp[C_U] += scale;
            v2 = &fixed[2];
        }
    }

    /* FIXME: what are the last arguments? */
    block->chain->render(block->chain, v0, v1, v2, param_5, param_6);
}

// FUNCTION: CARMA2_HW 0x00545720
void C2_NAKED C2_HOOK_CDECL OpTriangleRelightTwoSided(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, temp_face_soft* tfp) {
#ifndef CARPOCALYPSE2_MATCHING
    int i, v;
    brp_vertex tv[3];
    br_vector3 rev_normal;
    br_colour colour;

    colour = scache.colour;
    if (tfp->flag & TFF_REVERSED) {
        tv[0] = *v0;
        tv[1] = *v1;
        tv[2] = *v2;

        for (v = 0; v < 3; v++) {

            if ((tv[v].flags & (TVDIR_FRONT | TVDIR_BACK)) == (TVDIR_FRONT | TVDIR_BACK)) {
                fmt_vertex* vp = &rend.vertices[fp_vertices[v]];

                if (rend.renderer->state.surface.colour_source == BRT_GEOMETRY) {
                    colour = rend.vertex_colours[fp_vertices[v]];
                }
                BrVector3Negate(&rev_normal, &vp->n);

                for (i = 0; i < rend.renderer->state.cache.nvertex_fns; i++) {
                    rend.renderer->state.cache.vertex_fns[i]((br_renderer*)rend.renderer, &vp->p, &vp->map, &rev_normal, colour, tv[v].comp);
                }
            }
        }
        block->chain->render(block->chain, &tv[0], &tv[1], &tv[2], fp_vertices, tfp);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, tfp);
    }
#else
    __asm {
            sub esp, 0xdc
            mov eax, dword ptr [scache+0xe0]
            mov ecx, dword ptr [esp + 0xf4]
            mov dword ptr [esp + 8], eax
            test byte ptr [ecx + 2], 1
            je L005458a5
            push edi
            push esi
            mov esi, dword ptr [esp + 0xec]
            mov ecx, 0x10
            lea edi, [esp + 0x24]
            mov edx, dword ptr [esp + 0xf8]
            rep movsd
            mov esi, dword ptr [esp + 0xf0]
            mov ecx, 0x10
            lea edi, [esp + 0x64]
            mov eax, dword ptr [rend+0x58]
            rep movsd
            mov esi, dword ptr [esp + 0xf4]
            mov ecx, 0x10
            lea edi, [esp + 0xa4]
            push ebp
            rep movsd
            lea ecx, [esp + 0x28]
            push ebx
            mov dword ptr [esp + 0x10], ecx
            mov ecx, dword ptr [rend+0x28]
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x1c], 3
    L005457a3:
            mov edx, dword ptr [esp + 0x10]
            mov edx, dword ptr [edx]
            and edx, 0xc0000000
            cmp edx, 0xc0000000
            jne L00545843
            mov edx, dword ptr [esp + 0x14]
            xor esi, esi
            mov si, word ptr [edx]
            mov edx, dword ptr [eax + 0x44]
            shl esi, 5
            add esi, ecx
            cmp edx, 0x2e
            jne L005457e5
            mov edi, dword ptr [rend+0x30]
            mov edx, esi
            sub edx, ecx
            sar edx, 5
            mov edx, dword ptr [edi + edx*4]
            mov dword ptr [esp + 0x18], edx
    L005457e5:
            fld dword ptr [esi + 0x14]
            fchs
            fstp dword ptr [esp + 0x20]
            fld dword ptr [esi + 0x18]
            fchs
            fstp dword ptr [esp + 0x24]
            fld dword ptr [esi + 0x1c]
            fchs
            fstp dword ptr [esp + 0x28]
            mov edx, dword ptr [eax + 0x724]
            xor ebx, ebx
            test edx, edx
            jle L00545843
            lea ebp, [esi + 0xc]
            mov edi, 0x728
    L00545814:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x18]
            push ecx
            lea ecx, [esp + 0x24]
            push edx
            push ecx
            push ebp
            push esi
            push eax
            call dword ptr [eax + edi]
            mov eax, dword ptr [rend+0x58]
            add esp, 0x18
            inc ebx
            add edi, 4
            cmp ebx, dword ptr [eax + 0x724]
            jl L00545814
            mov ecx, dword ptr [rend+0x28]
    L00545843:
            mov edi, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x1c]
            add edi, 2
            add esi, 0x40
            dec edx
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x1c], edx
            jne L005457a3
            mov edx, dword ptr [esp + 0xf0]
            mov ecx, dword ptr [esp + 0x104]
            push ecx
            lea ecx, [esp + 0xb0]
            mov eax, dword ptr [edx + 4]
            mov edx, dword ptr [esp + 0x104]
            push edx
            push ecx
            lea edx, [esp + 0x78]
            lea ecx, [esp + 0x38]
            push edx
            push ecx
            push eax
            call dword ptr [eax]
            add esp, 0x18
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0xdc
            ret
    L005458a5:
            mov edx, dword ptr [esp + 0xe0]
            push ecx
            mov ecx, dword ptr [esp + 0xf4]
            mov eax, dword ptr [edx + 4]
            mov edx, dword ptr [esp + 0xf0]
            push ecx
            mov ecx, dword ptr [esp + 0xf0]
            push edx
            mov edx, dword ptr [esp + 0xf0]
            push ecx
            push edx
            push eax
            call dword ptr [eax]
            add esp, 0x18
            add esp, 0xdc
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005458e0
void C2_HOOK_CDECL OpTriangleToLines(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* fp) {

    if (!rend.edge_flags[fp->edges[0]]) {
        block->chain->render(block->chain, v0, v1);
        rend.edge_flags[fp->edges[0]] = 1;
    }

    if (!rend.edge_flags[fp->edges[1]]) {
        block->chain->render(block->chain, v1, v2);
        rend.edge_flags[fp->edges[1]] = 1;
    }

    if (!rend.edge_flags[fp->edges[2]]) {
        block->chain->render(block->chain, v2, v0);
        rend.edge_flags[fp->edges[2]] = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00545990
void C2_HOOK_CDECL OpTriangleReplicateConstant(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    br_uint_32 m;
    int c;

    m = rend.block->constant_mask;

    for (c = 0; m != 0; c++, m >>= 1) {
        if (m & 1) {
            v1->comp[c] = v2->comp[c] = v0->comp[c];
        }
    }

    block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

// FUNCTION: CARMA2_HW 0x005459f0
void C2_HOOK_CDECL OpTriangleReplicateConstantI(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {

    v1->comp[C_I] = v2->comp[C_I] = v0->comp[C_I];

    block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

// FUNCTION: CARMA2_HW 0x00545a30
void C2_HOOK_CDECL OpTriangleReplicateConstantRGB(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    v1->comp[C_R] = v2->comp[C_R] = v0->comp[C_R];
    v1->comp[C_G] = v2->comp[C_G] = v0->comp[C_G];
    v1->comp[C_B] = v2->comp[C_B] = v0->comp[C_B];

    block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

// FUNCTION: CARMA2_HW 0x00545a80
void C2_HOOK_CDECL OpTriangleToPoints(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices) {

    if (!rend.vertex_flags[fp_vertices[0]] && !(v0->flags & OUTCODES_ALL)) {
        block->chain->render(block->chain, v0);
        rend.vertex_flags[fp_vertices[0]] = 1;
    }

    if (!rend.vertex_flags[fp_vertices[1]] && !(v1->flags & OUTCODES_ALL)) {
        block->chain->render(block->chain, v1);
        rend.vertex_flags[fp_vertices[1]] = 1;
    }

    if (!rend.vertex_flags[fp_vertices[2]] && !(v2->flags & OUTCODES_ALL)) {
        block->chain->render(block->chain, v2);
        rend.vertex_flags[fp_vertices[2]] = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00545b40
void C2_HOOK_CDECL OpTriangleToPoints_OS(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices) {

    if (!rend.vertex_flags[fp_vertices[0]]) {
        block->chain->render(block->chain, v0);
        rend.vertex_flags[fp_vertices[0]] = 1;
    }

    if (!rend.vertex_flags[fp_vertices[1]]) {
        block->chain->render(block->chain, v1);
        rend.vertex_flags[fp_vertices[1]] = 1;
    }

    if (!rend.vertex_flags[fp_vertices[2]]) {
        block->chain->render(block->chain, v2);
        rend.vertex_flags[fp_vertices[2]] = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00545be0
void C2_HOOK_CDECL OpLineClip(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
    brp_vertex clipped[2];

    if (ClipLine(rend.renderer, clipped, v0, v1, rend.renderer->state.cache.clip_slots, v0->flags | v1->flags)) {
        ClippedRenderLine(rend.renderer, block->chain, clipped);
    }
}

// FUNCTION: CARMA2_HW 0x00545c40
void C2_NAKED C2_HOOK_STDCALL SubdivideSetThreshold(br_int_32 subdivide_tolerance) {
#ifndef CARPOCALYPSE2_MATCHING

    rend.subdivide_threshold = 1.f / (1.25f - 0.002f * (float)subdivide_tolerance);
#else
    __asm {
            fild dword ptr [esp + 4]
            fmul dword ptr [rendererStateStoredDispatch+0x74]
            fsub dword ptr [rendererStateStoredDispatch+0x78]
            fdivr qword ptr [rendererStateStoredDispatch+0x80]
            fstp dword ptr [rend+0x60]
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054c766
br_boolean C2_NAKED C2_HOOK_CDECL subdivideCheck(brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar z0,z1,z2,zt;

    z0 = (br_scalar)fabs(v0->comp[C_Z]);
    z1 = (br_scalar)fabs(v1->comp[C_Z]);
    z2 = (br_scalar)fabs(v2->comp[C_Z]);

    if (z0 > z1) {
        zt = z0;
        z0 = z1;
        z1 = zt;
    }

    if (z0 > z2) {
        zt = z0;
        z0 = z2;
        z2 = zt;
    }

    if (z1 > z2) {
        zt = z1;
        z1 = z2;
        z2 = zt;
    }

    if (z0 > (br_scalar)fabs(rend.subdivide_threshold * z2)) {
        return 0;
    }

    return 1;
#else
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push edi
            push esi
            mov eax, dword ptr [ebp + 8]
            mov ebx, dword ptr [ebp + 0xc]
            mov ecx, dword ptr [ebp + 0x10]
            mov eax, dword ptr [eax + 0xc]
            mov ebx, dword ptr [ebx + 0xc]
            mov ecx, dword ptr [ecx + 0xc]
            and eax, 0x7fffffff
            and ebx, 0x7fffffff
            and ecx, 0x7fffffff
            xor edx, edx
            cmp ebx, eax
            rcl edx, 1
            cmp ecx, eax
            rcl edx, 1
            cmp ecx, ebx
            rcl edx, 1
            fld dword ptr [rend+0x60]
            mov eax, dword ptr [edx*4 + rendererStateStoredTemplateEntries+0x148]
            mov ebx, dword ptr [edx*4 + rendererStateStoredTemplateEntries+0x13c]
            mov eax, dword ptr ss:[eax + ebp + 8]
            mov ebx, dword ptr ss:[ebx + ebp + 8]
            fmul dword ptr [eax + 0xc]
            fstp dword ptr [rendererStateStoredTemplateEntries+0x168]
            mov eax, dword ptr [rendererStateStoredTemplateEntries+0x168]
            mov ebx, dword ptr [ebx + 0xc]
            and eax, 0x7fffffff
            and ebx, 0x7fffffff
            cmp eax, ebx
            jge L0054c7e5
            mov eax, 0
            pop esi
            pop edi
            pop ebx
            pop ebp
            ret
    L0054c7e5:
            mov eax, 1
            pop esi
            pop edi
            pop ebx
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00545de0
void C2_NAKED C2_HOOK_STDCALL triangleSubdivide(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 combined_codes;

    combined_codes = v0->flags | v1->flags | v2->flags;

    if ((combined_codes & OUTCODES_NOT) != OUTCODES_NOT) {
        return;
    }

    if (combined_codes & OUTCODES_ALL) {
        triangleSubdivideCheck(depth, block, v0,v1,v2, fp_vertices, fp_edges);
    } else{
        triangleSubdivideOnScreen(depth, block, v0, v1, v2, fp_vertices, fp_edges);
    }
#else
    __asm {
    L00545de0:
            sub esp, 0xc0
            push ebx
            mov ebx, dword ptr [esp + 0xd0]
            push ebp
            mov ebp, dword ptr [esp + 0xdc]
            mov edx, dword ptr [ebx]
            push esi
            mov ecx, dword ptr [ebp]
            push edi
            mov edi, dword ptr [esp + 0xe0]
            mov eax, dword ptr [edi]
            or eax, edx
            or eax, ecx
            mov ecx, eax
            and ecx, 0xfff0000
            cmp ecx, 0xfff0000
            jne L00545f5c
            test eax, 0xfff
            je L00545f34
            mov esi, dword ptr [esp + 0xd4]
            test esi, esi
            jle L00545f04
            push ebp
            push edi
            push ebx
            call subdivideCheck
            add esp, 0xc
            test eax, eax
            je L00545f04
            push ebp
            push edi
            lea edx, [esp + 0x58]
            push ebx
            lea eax, [esp + 0x9c]
            push edx
            mov edx, dword ptr [rend+0x58]
            lea ecx, [esp + 0x20]
            push eax
            push ecx
            push edx
            call averageVertices
            mov edi, dword ptr [esp + 0x108]
            mov ebx, dword ptr [esp + 0x104]
            mov ebp, dword ptr [esp + 0xf4]
            add esp, 0x1c
            lea eax, [esp + 0x90]
            lea ecx, [esp + 0x10]
            push edi
            push ebx
            push eax
            lea edx, [esp + 0x5c]
            push ecx
            dec esi
            push edx
            push ebp
            push esi
            call L00545de0
            push edi
            lea eax, [esp + 0x94]
            mov edx, dword ptr [esp + 0xe0]
            push ebx
            lea ecx, [esp + 0x58]
            push eax
            push ecx
            push edx
            push ebp
            push esi
            call L00545de0
            push edi
            lea eax, [esp + 0x54]
            mov edx, dword ptr [esp + 0xe4]
            push ebx
            lea ecx, [esp + 0x18]
            push eax
            push ecx
            push edx
            push ebp
            push esi
            call L00545de0
            push edi
            lea eax, [esp + 0x14]
            mov edx, dword ptr [esp + 0xe8]
            push ebx
            lea ecx, [esp + 0x98]
            push eax
            push ecx
            push edx
            push ebp
            push esi
            call L00545de0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret 0x1c
    L00545f04:
            mov ecx, dword ptr [esp + 0xec]
            mov edx, dword ptr [esp + 0xe8]
            mov eax, dword ptr [esp + 0xd8]
            push ecx
            push edx
            push ebp
            mov eax, dword ptr [eax + 4]
            push edi
            push ebx
            push eax
            call dword ptr [eax]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret 0x1c
    L00545f34:
            mov eax, dword ptr [esp + 0xec]
            mov ecx, dword ptr [esp + 0xe8]
            mov edx, dword ptr [esp + 0xd8]
            push eax
            mov eax, dword ptr [esp + 0xd8]
            push ecx
            push ebp
            push edi
            push ebx
            push edx
            push eax
            call triangleSubdivideOnScreen
    L00545f5c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret 0x1c
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00545f70
void C2_NAKED C2_HOOK_STDCALL triangleSubdivideOnScreen(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
#ifndef CARPOCALYPSE2_MATCHING
    brp_vertex mid0, mid1, mid2;

    if (depth > 0 && subdivideCheck(v0, v1, v2)) {

        averageVerticesOnScreen(rend.renderer, &mid1, &mid2, &mid0, v0, v1, v2);
        triangleSubdivideOnScreen(depth - 1, block, &mid0, &mid1, &mid2, fp_vertices, fp_edges);
        triangleSubdivideOnScreen(depth - 1, block, v0,    &mid0, &mid2, fp_vertices, fp_edges);
        triangleSubdivideOnScreen(depth - 1, block, v1,    &mid1, &mid0, fp_vertices, fp_edges);
        triangleSubdivideOnScreen(depth - 1, block, v2,    &mid2, &mid1, fp_vertices, fp_edges);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
    }
#else
    __asm {
    L00545f70:
            sub esp, 0xc0
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0xd0]
            push edi
            mov edi, dword ptr [esp + 0xe4]
            test esi, esi
            jle L00546069
            mov ebx, dword ptr [esp + 0xe0]
            mov ebp, dword ptr [esp + 0xdc]
            push edi
            push ebx
            push ebp
            call subdivideCheck
            add esp, 0xc
            test eax, eax
            je L00546077
            push edi
            push ebx
            lea eax, [esp + 0x58]
            push ebp
            lea ecx, [esp + 0x9c]
            push eax
            mov eax, dword ptr [rend+0x58]
            lea edx, [esp + 0x20]
            push ecx
            push edx
            push eax
            call averageVerticesOnScreen
            mov edi, dword ptr [esp + 0x108]
            mov ebx, dword ptr [esp + 0x104]
            mov ebp, dword ptr [esp + 0xf4]
            add esp, 0x1c
            lea ecx, [esp + 0x90]
            lea edx, [esp + 0x10]
            push edi
            push ebx
            push ecx
            lea eax, [esp + 0x5c]
            push edx
            dec esi
            push eax
            push ebp
            push esi
            call L00545f70
            push edi
            lea ecx, [esp + 0x94]
            mov eax, dword ptr [esp + 0xe0]
            push ebx
            lea edx, [esp + 0x58]
            push ecx
            push edx
            push eax
            push ebp
            push esi
            call L00545f70
            push edi
            lea ecx, [esp + 0x54]
            mov eax, dword ptr [esp + 0xe4]
            push ebx
            lea edx, [esp + 0x18]
            push ecx
            push edx
            push eax
            push ebp
            push esi
            call L00545f70
            push edi
            lea ecx, [esp + 0x14]
            mov eax, dword ptr [esp + 0xe8]
            push ebx
            lea edx, [esp + 0x98]
            push ecx
            push edx
            push eax
            push ebp
            push esi
            call L00545f70
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret 0x1c
    L00546069:
            mov ebx, dword ptr [esp + 0xe0]
            mov ebp, dword ptr [esp + 0xdc]
    L00546077:
            mov ecx, dword ptr [esp + 0xd8]
            mov edx, dword ptr [esp + 0xec]
            push edx
            mov eax, dword ptr [ecx + 4]
            mov ecx, dword ptr [esp + 0xec]
            push ecx
            push edi
            push ebx
            push ebp
            push eax
            call dword ptr [eax]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret 0x1c
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

void C2_HOOK_STDCALL triangleSubdivideCheck(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    brp_vertex mid0, mid1, mid2;

    if (depth > 0 && subdivideCheck(v0, v1, v2)) {

        averageVertices(rend.renderer, &mid1, &mid2, &mid0, v0, v1, v2);
        triangleSubdivide(depth - 1, block, &mid0, &mid1, &mid2, fp_vertices, fp_edges);
        triangleSubdivide(depth - 1, block, v0,    &mid0, &mid2, fp_vertices, fp_edges);
        triangleSubdivide(depth - 1, block, v1,    &mid1, &mid0, fp_vertices, fp_edges);
        triangleSubdivide(depth - 1, block, v2,    &mid2, &mid1, fp_vertices, fp_edges);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
    }
}

// FUNCTION: CARMA2_HW 0x00545c60
void C2_NAKED C2_HOOK_CDECL OpTriangleSubdivide(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
#ifndef CARPOCALYPSE2_MATCHING

    triangleSubdivide(6, block, v0, v1, v2, fp_vertices, fp_edges);
#else
    __asm {
            sub esp, 0xc0
            push ebx
            mov ebx, dword ptr [esp + 0xcc]
            push ebp
            push esi
            mov esi, dword ptr [esp + 0xdc]
            mov ecx, dword ptr [ebx]
            push edi
            mov edi, dword ptr [esp + 0xdc]
            mov eax, dword ptr [esi]
            or eax, dword ptr [edi]
            or eax, ecx
            mov ecx, eax
            and ecx, 0xfff0000
            cmp ecx, 0xfff0000
            jne L00545dca
            test eax, 0xfff
            je L00545da8
            push esi
            push edi
            push ebx
            call subdivideCheck
            add esp, 0xc
            test eax, eax
            je L00545d7a
            push esi
            push edi
            lea edx, [esp + 0x58]
            push ebx
            lea eax, [esp + 0x9c]
            push edx
            mov edx, dword ptr [rend+0x58]
            lea ecx, [esp + 0x20]
            push eax
            push ecx
            push edx
            call averageVertices
            mov ebp, dword ptr [esp + 0x104]
            mov eax, dword ptr [esp + 0x100]
            add esp, 0x1c
            lea ecx, [esp + 0x90]
            lea edx, [esp + 0x10]
            push ebp
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0xe0]
            lea eax, [esp + 0x5c]
            push edx
            push eax
            push ecx
            push 5
            call triangleSubdivide
            mov edx, dword ptr [esp + 0xe4]
            push ebp
            lea eax, [esp + 0x94]
            push edx
            lea ecx, [esp + 0x58]
            push eax
            push ecx
            push ebx
            mov ebx, dword ptr [esp + 0xe8]
            push ebx
            push 5
            call triangleSubdivide
            mov edx, dword ptr [esp + 0xe4]
            push ebp
            lea eax, [esp + 0x54]
            push edx
            lea ecx, [esp + 0x18]
            push eax
            push ecx
            push edi
            push ebx
            push 5
            call triangleSubdivide
            mov edx, dword ptr [esp + 0xe4]
            push ebp
            lea eax, [esp + 0x14]
            push edx
            lea ecx, [esp + 0x98]
            push eax
            push ecx
            push esi
            push ebx
            push 5
            call triangleSubdivide
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret
    L00545d7a:
            mov edx, dword ptr [esp + 0xd4]
            mov ecx, dword ptr [esp + 0xe8]
            push ecx
            mov eax, dword ptr [edx + 4]
            mov edx, dword ptr [esp + 0xe8]
            push edx
            push esi
            push edi
            push ebx
            push eax
            call dword ptr [eax]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret
    L00545da8:
            mov eax, dword ptr [esp + 0xe8]
            mov ecx, dword ptr [esp + 0xe4]
            mov edx, dword ptr [esp + 0xd4]
            push eax
            push ecx
            push esi
            push edi
            push ebx
            push edx
            push 6
            call triangleSubdivideOnScreen
    L00545dca:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc0
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005460b0
void C2_HOOK_CDECL OpTriangleSubdivideOnScreen(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {

    triangleSubdivideOnScreen(6, block, v0, v1, v2, fp_vertices, fp_edges);
}
