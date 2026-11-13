#include "clip.h"

#include "rend.h"
#include "setup.h"

int ClipFaceToPlane(brp_vertex* vp, brp_vertex* verts_out, int num_in, br_vector4* plane, int cmask) {
    brp_vertex* wp = verts_out;
    int num_out = 0;

    brp_vertex *up;
    br_scalar t, tu, tv;
    int m;
    br_scalar* usp;
    br_scalar* vsp;
    br_scalar* wsp;

    up = &vp[num_in - 1];

    tu = -BR_MAC4(
        plane->v[0], up->comp[C_X],
        plane->v[1], up->comp[C_Y],
        plane->v[2], up->comp[C_Z],
        plane->v[3], up->comp[C_W]);

    for ( ; num_in-- ; up = vp, tu = tv, vp++) {
        tv = -BR_MAC4(
            plane->v[0], vp->comp[C_X],
            plane->v[1], vp->comp[C_Y],
            plane->v[2], vp->comp[C_Z],
            plane->v[3], vp->comp[C_W]);
        if (tv <= 0.f) {
            if (tu <= 0.f) {
                *wp++ = *vp;
                num_out++;
                continue;
            }
            if (tv < 0.f) {
                t = tv / (tv - tu);

                usp = up->comp;
                vsp = vp->comp;
                wsp = wp->comp;

                for (m = cmask ; m != 0 ; m >>= 1, usp++, vsp++, wsp++) {
                    if (m & 1) {
                        *wsp = *vsp + t * (*usp - *vsp);
                    }
                }

                wp->flags = TV_CLIPPED;
                wp++;
                num_out++;
            }
            *wp++ = *vp;
            num_out++;
        } else {
            if (tu >= 0.f) {
                continue;
            }
            t = tu / (tu-tv);
            usp = up->comp;
            vsp = vp->comp;
            wsp = wp->comp;
            for (m = cmask ; m != 0 ; m >>=1, usp++, vsp++, wsp++) {
                if (m & 1) {
                    *wsp = *usp + t * (*vsp - *usp);
                }
            }

            wp->flags = TV_CLIPPED;
            wp++;
            num_out++;
        }
    }

    return num_out;
}

#ifdef CARPOCALYPSE2_MATCHING
extern br_scalar gScreenZOffset;
extern char rendererStateStoredDispatch[];
extern static_cache_soft scache;
#endif
// FUNCTION: CARMA2_HW 0x0054bb70
int C2_HOOK_STDCALL ClipFaceToPlus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask) {
    brp_vertex* wp = verts_out;
    int num_out = 0;
    brp_vertex* up;
    br_scalar t, tu, tv;
    int m;
    br_scalar* usp;
    br_scalar* vsp;
    br_scalar* wsp;

    up = &vp[num_in - 1];
    tu = up->comp[axis] - up->comp[C_W];

    for ( ; num_in != 0 ; num_in--, up = vp, tu = tv, vp++) {

        tv = vp->comp[axis] - vp->comp[C_W];

        if (tv <= 0.f) {
            if (tu <= 0.f) {
                *wp++ = *vp;
                num_out++;
                continue;
            }

            if (tv < 0.f) {
                t = tv /(tv - tu);
                usp = up->comp;
                vsp = vp->comp;
                wsp = wp->comp;
                for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
                    if (m & 1) {
                        *wsp = *vsp + t * (*usp - *vsp);
                    }
                }

                wp->comp[axis] = wp->comp[C_W];
                wp->flags = TV_CLIPPED;
                wp++;
                num_out++;
            }
            *wp++ = *vp;
            num_out++;

        } else {
            if (tu >= 0.f) {
                continue;
            }

            t = tu / (tu - tv);

            usp = up->comp;
            vsp = vp->comp;
            wsp = wp->comp;

            for (m = cmask ; m != 0 ; m >>= 1, usp++, vsp++, wsp++) {
                if (m & 1) {
                    *wsp = *usp + t * (*vsp - *usp);
                }
            }

            wp->comp[axis] = wp->comp[C_W];
            wp->flags = TV_CLIPPED;
            wp++;
            num_out++;
        }
    }

    return num_out;
}

// FUNCTION: CARMA2_HW 0x0054bd00
int C2_HOOK_STDCALL ClipFaceToMinus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask) {
    brp_vertex* wp = verts_out;
    int num_out = 0;
    brp_vertex* up;
    br_scalar t, tu, tv;
    int m;
    br_scalar *usp;
    br_scalar* vsp;
    br_scalar* wsp;

    up = &vp[num_in - 1];
    tu = -up->comp[axis] - up->comp[C_W];

    for ( ; num_in != 0 ; num_in--, up = vp, tu = tv, vp++) {

        tv = - vp->comp[axis] - vp->comp[C_W];

        if (tv <= 0.f) {
            if (tu <= 0.f) {
                *wp++ = *vp;
                num_out++;
                continue;
            }
            if (tv < 0.f) {
                t = tv /(tv-tu);
                usp = up->comp;
                vsp = vp->comp;
                wsp = wp->comp;

                for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
                    if (m & 1) {
                        *wsp = *vsp + t * (*usp - *vsp);
                    }
                }

                wp->comp[axis] = -wp->comp[C_W] + 2 * BR_SCALAR_EPSILON;
                wp->flags = TV_CLIPPED;
                wp++;
                num_out++;
            }
            *wp++ = *vp;
            num_out++;
        } else {
            if (tu >= 0.f) {
                continue;
            }
            t = tu /(tu-tv);
            usp = up->comp;
            vsp = vp->comp;
            wsp = wp->comp;
            for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
                if (m & 1) {
                    *wsp = *usp + t * (*vsp - *usp);
                }
            }

            wp->comp[axis] = -wp->comp[C_W] + 2 * BR_SCALAR_EPSILON;
            wp->flags = TV_CLIPPED;
            wp++;
            num_out++;
        }
    }

    return num_out;
}

#define CLIP_TOGGLE do {                \
        toggle = !toggle;               \
        if (toggle) {                   \
            cp_in = clip_poly_2;   \
            cp_out = clip_poly_1;  \
        } else {                        \
            cp_in = clip_poly_1;   \
            cp_out = clip_poly_2;  \
        }                               \
    } while (0)

// FUNCTION: CARMA2_HW 0x0054b730
brp_vertex* C2_HOOK_STDCALL FaceClip(br_soft_renderer* self, brp_vertex* clip_in, br_uint_32 mask, br_uint_32 codes, int n, int* n_out) {
    // GLOBAL: CARMA2_HW 0x006b0a60
    static  brp_vertex clip_poly_1[16];
    // GLOBAL: CARMA2_HW 0x006b0e60
    static  brp_vertex clip_poly_2[16];
    brp_vertex* cp_in = clip_in;
    brp_vertex* cp_out = clip_poly_1;
    int c;
    br_boolean toggle = 1;

    if (codes & OUTCODE_HITHER) {
        n = ClipFaceToPlus1(cp_in, cp_out, n, C_Z, mask & ~(1 << C_Z));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_YON) {
        n = ClipFaceToMinus1(cp_in, cp_out, n, C_Z, mask & ~(1 << C_Z));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_RIGHT) {
        n = ClipFaceToPlus1(cp_in, cp_out, n, C_X, mask & ~(1 << C_X));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_LEFT) {
        n = ClipFaceToMinus1(cp_in, cp_out, n, C_X,mask & ~(1 << C_X));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_TOP) {
        n = ClipFaceToPlus1(cp_in, cp_out, n, C_Y, mask & ~(1 << C_Y));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_BOTTOM) {
        n = ClipFaceToMinus1(cp_in, cp_out, n, C_Y,mask & ~(1 << C_Y));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (scache.user_clip_active) {
        for (c = 0; c < MAX_STATE_CLIP_PLANES; c++) {

            if (self->state.clip[c].type != BRT_PLANE) {
                continue;
            }

            if (!(codes & (OUTCODE_USER << c))) {
                continue;
            }

            n = ClipFaceToPlane(cp_in, cp_out, n, &self->state.clip[c].plane, mask);
            if (n < 3) {
                return NULL;
            }
            CLIP_TOGGLE;
        }
    }

    *n_out = n;
    return cp_in;
}

// FUNCTION: CARMA2_HW 0x0054bea0
void C2_NAKED C2_HOOK_STDCALL ClippedRenderTriangles(br_soft_renderer* renderer, brp_block* block, brp_vertex* cp_in, int n, br_uint_16 *fp_vertices, br_uint_16 *fp_edges) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    brp_vertex* tvp;

    for (i = 0; i < n; i++) {
        tvp = &cp_in[i];

        if (tvp->flags & (TV_CLIPPED | OUTCODES_ALL)) {
            PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
            UPDATE_BOUNDS(tvp);
        }
    }

    for (i = 2; i < n; i++) {
        block->render(block, &cp_in[0], &cp_in[i - 1], &cp_in[i], fp_vertices, fp_edges);
    }
#else
    __asm {
            push ebx
            mov ebx, dword ptr [esp + 0x14]
            push ebp
            push esi
            test ebx, ebx
            push edi
            jle L0054bf7b
            mov eax, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x14]
            mov esi, ebx
            mov edi, 0x20000fff
            lea ecx, [eax + 0x10]
    L0054bec2:
            test dword ptr [ecx - 0x10], edi
            je L0054bf71
            fld dword ptr [edx + 0x5f8]
            fld dword ptr [ecx - 0xc]
            fmulp st(1), st(0)
            _emit 0xd8
        _emit 0x31
            fadd dword ptr [edx + 0x5b8]
            fstp dword ptr [ecx + 4]
            fld dword ptr [edx + 0x5fc]
            fmul dword ptr [ecx - 8]
            _emit 0xd8
        _emit 0x31
            fadd dword ptr [edx + 0x5bc]
            fstp dword ptr [ecx + 8]
            fld dword ptr [edx + 0x600]
            fmul dword ptr [ecx - 4]
            _emit 0xd8
        _emit 0x31
            fadd dword ptr [edx + 0x5c0]
            fld dword ptr [ecx + 4]
            fxch st(1)
            fadd dword ptr [gScreenZOffset]
            fstp dword ptr [ecx + 0xc]
            fcomp dword ptr [scache+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054bf29
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [scache+0xec], eax
    L0054bf29:
            fld dword ptr [ecx + 4]
            fcomp dword ptr [scache+0xe4]
            fnstsw ax
            test ah, 1
            je L0054bf41
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [scache+0xe4], eax
    L0054bf41:
            fld dword ptr [ecx + 8]
            fcomp dword ptr [scache+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L0054bf59
            mov eax, dword ptr [ecx + 8]
            mov dword ptr [scache+0xf0], eax
    L0054bf59:
            fld dword ptr [ecx + 8]
            fcomp dword ptr [scache+0xe8]
            fnstsw ax
            test ah, 1
            je L0054bf71
            mov eax, dword ptr [ecx + 8]
            mov dword ptr [scache+0xe8], eax
    L0054bf71:
            add ecx, 0x40
            dec esi
            jne L0054bec2
    L0054bf7b:
            cmp ebx, 2
            jle L0054bfb1
            mov ecx, dword ptr [esp + 0x1c]
            mov ebp, dword ptr [esp + 0x28]
            mov edi, dword ptr [esp + 0x18]
            add ebx, -2
            lea esi, [ecx + 0x80]
    L0054bf95:
            mov edx, dword ptr [esp + 0x24]
            mov ecx, dword ptr [esp + 0x1c]
            push ebp
            push edx
            lea eax, [esi - 0x40]
            push esi
            push eax
            push ecx
            push edi
            call dword ptr [edi]
            add esp, 0x18
            add esi, 0x40
            dec ebx
            jne L0054bf95
    L0054bfb1:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x18
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

// FUNCTION: CARMA2_HW 0x0054bfc0
br_boolean C2_NAKED C2_HOOK_STDCALL ClipLineToPlane(brp_vertex* in, brp_vertex* out, br_vector4* plane, int cmask) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar t, tu, tv;
    int m;
    br_scalar* usp;
    br_scalar* vsp;
    br_scalar* wsp;

    tu = -BR_MAC4(
        plane->v[0], in[0].comp[C_X],
        plane->v[1], in[0].comp[C_Y],
        plane->v[2], in[0].comp[C_Z],
        plane->v[3], in[0].comp[C_W]);

    tv = -BR_MAC4(
        plane->v[0], in[1].comp[C_X],
        plane->v[1], in[1].comp[C_Y],
        plane->v[2], in[1].comp[C_Z],
        plane->v[3], in[1].comp[C_W]);

    out[0] = in[0];
    out[1] = in[1];

    if (tu <= 0.f) {
        if (tv <= 0.f) {
            out[1] = in[1];
            return 1;
        }
        t = tu / (tu - tv);
        usp = in[0].comp;
        vsp = in[1].comp;
        wsp = out[1].comp;
        for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
            if (m & 1) {
                *wsp = *usp + t * (*vsp - *usp);
            }
        }
    } else {
        if (tv > 0.f) {
            return 0;
        }
        t = tv / (tv - tu);
        usp = in[0].comp;
        vsp = in[1].comp;
        wsp = out[0].comp;
        for (m = cmask ; m != 0 ; m >>= 1, usp++, vsp++, wsp++) {
            if (m & 1) {
                *wsp = *vsp + t * (*usp - *vsp);
            }
        }
    }

    return 1;
#else
    __asm {
            mov edx, dword ptr [esp + 4]
            push ebx
            mov ebx, dword ptr [esp + 0x10]
            push ebp
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 0xc]
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            fld dword ptr [edx + 0x10]
            fxch st(2)
            fmulp st(4), st(0)
            mov ebp, dword ptr [esp + 0x10]
            push esi
            push edi
            mov ecx, 0x10
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            mov esi, edx
            mov edi, ebp
            fmul dword ptr [ebx + 0xc]
            fld dword ptr [edx + 4]
            _emit 0xd8
        _emit 0x0b
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [edx + 0x48]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ebx + 4]
            fxch st(1)
            fchs
            fst dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0x134]
            fld dword ptr [edx + 0x4c]
            fmul dword ptr [ebx + 8]
            fnstsw ax
            faddp st(1), st(0)
            fld dword ptr [edx + 0x50]
            fld dword ptr [edx + 0x44]
            fld dword ptr [ebx + 0xc]
            fmulp st(2), st(0)
            test ah, 0x41
            _emit 0xd8
        _emit 0x0b
            fxch st(1)
            faddp st(2), st(0)
            rep movsd
            faddp st(1), st(0)
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x134]
            lea ebx, [edx + 0x40]
            lea edi, [ebp + 0x40]
            mov ecx, 0x10
            mov esi, ebx
            mov dword ptr [esp + 0x18], edi
            rep movsd
            fnstsw ax
            je L0054c0c2
            test ah, 0x41
            je L0054c07d
            mov edi, dword ptr [esp + 0x18]
            mov ecx, 0x10
            mov esi, ebx
            mov eax, 1
            rep movsd
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L0054c07d:
            fld dword ptr [esp + 0x1c]
            fsub dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x20]
            mov eax, edx
            test ecx, ecx
            fdivr dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x20]
            je L0054c114
            sub ebp, edx
    L0054c099:
            test cl, 1
            je L0054c0ad
            fld dword ptr [eax + 0x40]
            _emit 0xd8
        _emit 0x20
            fmul dword ptr [esp + 0x20]
            _emit 0xd8
        _emit 0x00
            fstp dword ptr [eax + ebp + 0x40]
    L0054c0ad:
            sar ecx, 1
            add eax, 4
            test ecx, ecx
            jne L0054c099
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L0054c0c2:
            test ah, 0x41
            jne L0054c0d0
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x10
    L0054c0d0:
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x20]
            mov ecx, ebx
            test eax, eax
            fdivr dword ptr [esp + 0x14]
            mov ebx, eax
            fstp dword ptr [esp + 0x20]
            je L0054c114
            lea eax, [ecx + ebp]
            sub eax, edx
            sub eax, 0x40
            sub edx, ebp
    L0054c0f6:
            test bl, 1
            je L0054c108
            fld dword ptr [edx + eax]
            _emit 0xd8
        _emit 0x21
            fmul dword ptr [esp + 0x20]
            _emit 0xd8
        _emit 0x01
            _emit 0xd9
        _emit 0x18
    L0054c108:
            sar ebx, 1
            add ecx, 4
            add eax, 4
            test ebx, ebx
            jne L0054c0f6
    L0054c114:
            pop edi
            pop esi
            pop ebp
            mov eax, 1
            pop ebx
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054c120
br_boolean C2_HOOK_STDCALL ClipLine(br_soft_renderer* self, brp_vertex* out, brp_vertex* v0, brp_vertex* v1, br_uint_32 mask, br_uint_32 codes) {
    // GLOBAL: CARMA2_HW 0x00670758
    static br_vector4 plane_px = { { -1.f,  0.f,  0.f, 1.f } };
    // GLOBAL: CARMA2_HW 0x00670768
    static br_vector4 plane_nx = { {  1.f,  0.f,  0.f, 1.f } };
    // GLOBAL: CARMA2_HW 0x00670778
    static br_vector4 plane_py = { {  0.f, -1.f,  0.f, 1.f } };
    // GLOBAL: CARMA2_HW 0x00670788
    static br_vector4 plane_ny = { {  0.f,  1.f,  0.f, 1.f } };
    // GLOBAL: CARMA2_HW 0x00670798
    static br_vector4 plane_pz = { {  0.f,  0.f, -1.f, 1.f } };
    // GLOBAL: CARMA2_HW 0x006707a8
    static br_vector4 plane_nz = { {  0.f,  0.f,  1.f, 1.f } };

    brp_vertex cv0[2];
    brp_vertex cv1[2];
    brp_vertex* cp_in = cv0;
    brp_vertex* cp_out = cv1;
    brp_vertex* cp_tmp;
    int c;

    cp_in[0] = *v0;
    cp_in[1] = *v1;

    if (codes & OUTCODE_LEFT) {
        if (!ClipLineToPlane(cp_in, cp_out, &plane_nx, mask)) {
            return 0;
        }
        cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
    }

    if(codes & OUTCODE_RIGHT) {
        if (!ClipLineToPlane(cp_in, cp_out, &plane_px, mask)) {
            return 0;
        }
        cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
    }

    if (codes & OUTCODE_TOP) {
        if (!ClipLineToPlane(cp_in, cp_out, &plane_py, mask)) {
            return 0;
        }
        cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
    }

    if (codes & OUTCODE_BOTTOM) {
        if (!ClipLineToPlane(cp_in, cp_out, &plane_ny, mask)) {
            return 0;
        }
        cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
    }

    if(codes & OUTCODE_HITHER) {
        if (!ClipLineToPlane(cp_in, cp_out, &plane_pz, mask)) {
            return 0;
        }
        cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
    }

    if (codes & OUTCODE_YON) {
        if (!ClipLineToPlane(cp_in, cp_out, &plane_nz, mask)) {
            return 0;
        }
        cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
    }
    if (scache.user_clip_active) {
        for (c = 0; c < MAX_STATE_CLIP_PLANES; c++) {
            if (self->state.clip[c].type != BRT_PLANE) {
                continue;
            }
            if (!(codes & (OUTCODE_USER << c))) {
                continue;
            }
            if (!ClipLineToPlane(cp_in, cp_out, &self->state.clip[c].plane, mask)) {
                return 0;
            }
            cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
        }
    }
    out[0] = cp_in[0];
    out[1] = cp_in[1];
    return 1;
}

// FUNCTION: CARMA2_HW 0x0054c320
void C2_NAKED C2_HOOK_STDCALL ClippedRenderLine(br_soft_renderer* renderer, brp_block* block, brp_vertex* cp_in) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    brp_vertex* tvp;

    for (i = 0, tvp = cp_in; i < 2; i++, tvp++) {
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);
    }

    block->render(block, &cp_in[0], &cp_in[1]);
#else
    __asm {
            mov edx, dword ptr [esp + 4]
            push esi
            push edi
            mov edi, dword ptr [esp + 0x14]
            mov esi, 2
            lea ecx, [edi + 0x10]
    L0054c332:
            fld dword ptr [ecx - 0xc]
            fmul dword ptr [edx + 0x5f8]
            _emit 0xd8
        _emit 0x31
            fadd dword ptr [edx + 0x5b8]
            fstp dword ptr [ecx + 4]
            fld dword ptr [edx + 0x5fc]
            fmul dword ptr [ecx - 8]
            _emit 0xd8
        _emit 0x31
            fadd dword ptr [edx + 0x5bc]
            fld dword ptr [ecx - 4]
            fxch st(1)
            fstp dword ptr [ecx + 8]
            fmul dword ptr [edx + 0x600]
            _emit 0xd8
        _emit 0x31
            fadd dword ptr [gScreenZOffset]
            fld dword ptr [ecx + 4]
            fxch st(1)
            fadd dword ptr [edx + 0x5c0]
            fstp dword ptr [ecx + 0xc]
            fcomp dword ptr [scache+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054c390
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [scache+0xec], eax
    L0054c390:
            fld dword ptr [ecx + 4]
            fcomp dword ptr [scache+0xe4]
            fnstsw ax
            test ah, 1
            je L0054c3a8
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [scache+0xe4], eax
    L0054c3a8:
            fld dword ptr [ecx + 8]
            fcomp dword ptr [scache+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L0054c3c0
            mov eax, dword ptr [ecx + 8]
            mov dword ptr [scache+0xf0], eax
    L0054c3c0:
            fld dword ptr [ecx + 8]
            fcomp dword ptr [scache+0xe8]
            fnstsw ax
            test ah, 1
            je L0054c3d8
            mov eax, dword ptr [ecx + 8]
            mov dword ptr [scache+0xe8], eax
    L0054c3d8:
            add ecx, 0x40
            dec esi
            jne L0054c332
            mov eax, dword ptr [esp + 0x10]
            lea ecx, [edi + 0x40]
            push ecx
            push edi
            push eax
            call dword ptr [eax]
            add esp, 0xc
            pop edi
            pop esi
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
