#include "light8.h"

#include "lightmac.h"
#include "setup.h"

#ifdef CARPOCALYPSE2_MATCHING
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34TApplyV();
extern static_cache_soft scache;
#endif
// FUNCTION: CARMA2_HW 0x0054a230
void C2_HOOK_CDECL SurfaceIndexZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_I] = self->state.cache.comp_offsets[C_I];
}

// FUNCTION: CARMA2_HW 0x0054a250
void C2_HOOK_CDECL SurfaceIndexUnlit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_I] = self->state.cache.comp_scales[C_I] * (((float)BR_ALPHA(colour)) / 256.f) + self->state.cache.comp_offsets[C_I];
}

// FUNCTION: CARMA2_HW 0x0054a290
void C2_NAKED C2_HOOK_CDECL SurfaceIndexLit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    active_light* alp = scache.lights;
    br_vector3 vp;
    br_vector3 vn;
    br_vector3 fvn;

    if (scache.light_1md) {
        br_scalar l, dot;

        dot = BrVector3Dot(n, &alp->direction);

        if (dot <= 0.0f) {
            comp[C_I] = self->state.surface.ka;
            CLAMP_SCALE(C_I);
            return;
        }

        l = dot * self->state.surface.kd;

        if (self->state.surface.ks != 0.0f) {
            dot = BrVector3Dot(n, &alp->half);

            if (dot > SPECULARPOW_CUTOFF) {
                l += SPECULAR_POWER(self->state.surface.ks * alp->intensity);
            }
        }

        comp[C_I] = l + self->state.surface.ka;

        CLAMP_SCALE(C_I);
        return;
    }

    comp[C_I] = self->state.surface.ka;

    rend.eye_l = scache.eye_m_normalised;

    for (i = 0; i < scache.nlights_model; i++, alp++) {
        alp->accumulate_index(self, p, n, alp, comp);
    }

    if (scache.nlights_view != 0) {
        BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
        BrMatrix34TApplyV(&vn, n, &scache.view_to_model);
        BrVector3Normalise(&fvn, &vn);

        BrVector3Set(&rend.eye_l, 0.f, 0.f, 1.f);

        for (i = 0; i < scache.nlights_view; i++, alp++) {
            alp->accumulate_index(self, &vp, &fvn, alp, comp);
        }
    }

    CLAMP_SCALE(C_I);
#else
    __asm {
            mov eax, dword ptr [scache+0x114]
            sub esp, 0x24
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov esi, offset scache+0x118
            je L0054a436
            fld dword ptr [scache+0x130]
            mov edx, dword ptr [esp + 0x44]
            fld dword ptr [scache+0x12c]
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fld dword ptr [scache+0x128]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            je L0054a346
            mov ecx, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x4c]
            fstp st(0)
            mov eax, dword ptr [ecx + 0x2c]
            mov dword ptr [esp + 0x38], eax
            mov dword ptr [edx + 0x28], eax
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [rendererStateStoredDispatch+0xe0]
            fnstsw ax
            test ah, 1
            je L0054a3dc
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            je L0054a32b
            mov ecx, dword ptr [ecx + 0x5cc]
            mov dword ptr [edx + 0x28], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a32b:
            fld dword ptr [ecx + 0x60c]
            fmul dword ptr [esp + 0x38]
            fadd dword ptr [ecx + 0x5cc]
            fstp dword ptr [edx + 0x28]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a346:
            mov ecx, dword ptr [esp + 0x38]
            fld dword ptr [ecx + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fmul dword ptr [ecx + 0x30]
            fnstsw ax
            test ah, 0x40
            jne L0054a3b5
            fld dword ptr [scache+0x13c]
            fld dword ptr [edx + 8]
            fld dword ptr [scache+0x138]
            fld dword ptr [scache+0x134]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [edx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xe8]
            fnstsw ax
            test ah, 0x41
            jne L0054a3b3
            fld dword ptr [ecx + 0x38]
            fld dword ptr [scache+0x140]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul dword ptr [ecx + 0x34]
            fxch st(1)
            fsubr dword ptr [ecx + 0x38]
            fadd st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fdivp st(1), st(0)
            faddp st(2), st(0)
    L0054a3b3:
            fstp st(0)
    L0054a3b5:
            fld dword ptr [ecx + 0x2c]
            mov edx, dword ptr [esp + 0x4c]
            fadd st(0), st(1)
            fstp dword ptr [esp + 0x4c]
            mov eax, dword ptr [esp + 0x4c]
            fstp st(0)
            fld dword ptr [esp + 0x4c]
            fcomp dword ptr [rendererStateStoredDispatch+0xe0]
            mov dword ptr [edx + 0x28], eax
            fnstsw ax
            test ah, 1
            jne L0054a3f9
    L0054a3dc:
            fld dword ptr [ecx + 0x5cc]
            fadd dword ptr [ecx + 0x60c]
            fsub dword ptr [rendererStateStoredDispatch+0xe4]
            fstp dword ptr [edx + 0x28]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a3f9:
            fld dword ptr [esp + 0x4c]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            je L0054a41b
            mov ecx, dword ptr [ecx + 0x5cc]
            mov dword ptr [edx + 0x28], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a41b:
            fld dword ptr [ecx + 0x60c]
            fmul dword ptr [esp + 0x4c]
            fadd dword ptr [ecx + 0x5cc]
            fstp dword ptr [edx + 0x28]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a436:
            mov edi, dword ptr [esp + 0x38]
            mov ebp, dword ptr [esp + 0x4c]
            xor ebx, ebx
            mov edx, dword ptr [edi + 0x2c]
            mov dword ptr [ebp + 0x28], edx
            mov eax, dword ptr [scache+0x104]
            mov ecx, dword ptr [scache+0x108]
            mov edx, dword ptr [scache+0x10c]
            mov dword ptr [rend+0x40], eax
            mov eax, dword ptr [scache+0x498]
            test eax, eax
            mov dword ptr [rend+0x44], ecx
            mov dword ptr [rend+0x48], edx
            jle L0054a48e
    L0054a471:
            mov eax, dword ptr [esp + 0x44]
            mov ecx, dword ptr [esp + 0x3c]
            push ebp
            push esi
            push eax
            push ecx
            push edi
            call dword ptr [esi + 0x2c]
            mov eax, dword ptr [scache+0x498]
            inc ebx
            add esi, 0x38
            cmp ebx, eax
            jl L0054a471
    L0054a48e:
            mov eax, dword ptr [scache+0x49c]
            test eax, eax
            je L0054a584
            mov eax, dword ptr [esp + 0x3c]
            lea edx, [edi + 0x4ac]
            push edx
            lea ecx, [esp + 0x2c]
            push eax
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [esp + 0x50]
            add esp, 0xc
            lea eax, [esp + 0x10]
            push offset scache+0x40
            push edx
            push eax
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            add esp, 0xc
            faddp st(1), st(0)
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [rendererStateStoredDispatch+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054a524
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x24]
            jmp L0054a53e
    L0054a524:
            fstp st(0)
            mov dword ptr [esp + 0x1c], 0x3f800000
            mov dword ptr [esp + 0x20], 0
            mov dword ptr [esp + 0x24], 0
    L0054a53e:
            mov eax, dword ptr [scache+0x49c]
            xor ebx, ebx
            test eax, eax
            mov dword ptr [rend+0x40], 0
            mov dword ptr [rend+0x44], 0
            mov dword ptr [rend+0x48], 0x3f800000
            jle L0054a584
    L0054a567:
            push ebp
            lea ecx, [esp + 0x20]
            push esi
            lea edx, [esp + 0x30]
            push ecx
            push edx
            push edi
            call dword ptr [esi + 0x2c]
            mov eax, dword ptr [scache+0x49c]
            inc ebx
            add esi, 0x38
            cmp ebx, eax
            jl L0054a567
    L0054a584:
            fld dword ptr [ebp + 0x28]
            fcomp dword ptr [rendererStateStoredDispatch+0xe0]
            fnstsw ax
            test ah, 1
            jne L0054a5b1
            fld dword ptr [edi + 0x5cc]
            fadd dword ptr [edi + 0x60c]
            fsub dword ptr [rendererStateStoredDispatch+0xe4]
            fstp dword ptr [ebp + 0x28]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a5b1:
            fld dword ptr [ebp + 0x28]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            je L0054a5d2
            mov eax, dword ptr [edi + 0x5cc]
            mov dword ptr [ebp + 0x28], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L0054a5d2:
            fld dword ptr [edi + 0x60c]
            fmul dword ptr [ebp + 0x28]
            fadd dword ptr [edi + 0x5cc]
            pop edi
            pop esi
            fstp dword ptr [ebp + 0x28]
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054a7b0
void C2_HOOK_STDCALL lightingIndexNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

}

// FUNCTION: CARMA2_HW 0x0054a7c0
void C2_NAKED C2_HOOK_STDCALL lightingIndexDirect(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_scalar l;

    dot = BrVector3Dot(n, &alp->direction);
    if (dot <= 0.f) {
        return;
    }
    l = dot * self->state.surface.kd;
    if (self->state.surface.ks != 0.f) {
        dot = BrVector3Dot(&alp->half, n);
        if (dot > SPECULARPOW_CUTOFF) {
            l += alp->intensity * SPECULAR_POWER(self->state.surface.ks);
        }
    }

    comp[C_I] += l;
#else
    __asm {
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0xc]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x18]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [ecx + 0x10]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            jne L0054a864
            push esi
            mov esi, dword ptr [esp + 8]
            fld dword ptr [esi + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fmul dword ptr [esi + 0x30]
            fnstsw ax
            test ah, 0x40
            jne L0054a856
            fld dword ptr [ecx + 0x20]
            fld dword ptr [edx + 4]
            fld dword ptr [ecx + 0x24]
            fld dword ptr [ecx + 0x1c]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [edx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xe8]
            fnstsw ax
            test ah, 0x41
            jne L0054a854
            fld dword ptr [esi + 0x38]
            fld dword ptr [ecx + 0x28]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul dword ptr [esi + 0x34]
            fxch st(1)
            fsubr dword ptr [esi + 0x38]
            fadd st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fdivp st(1), st(0)
            faddp st(2), st(0)
    L0054a854:
            fstp st(0)
    L0054a856:
            mov eax, dword ptr [esp + 0x18]
            pop esi
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
            ret 0x14
    L0054a864:
            fstp st(0)
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054a870
void C2_NAKED C2_HOOK_STDCALL lightingIndexPoint(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 dirn;
    br_scalar l;
    br_scalar dot;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn, &dirn);
    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    l = dot * self->state.surface.kd;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            l += SPECULAR_POWER(self->state.surface.ks);
        }
    }

    comp[C_I] += alp->intensity * l;
#else
    __asm {
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 8]
            sub esp, 0xc
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
            push esi
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
            fcom dword ptr [rendererStateStoredDispatch+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054a8e2
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 4]
            fxch st(1)
            fstp dword ptr [esp + 8]
            fstp dword ptr [esp + 0xc]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L0054a902
    L0054a8e2:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 4], 0x3f800000
            mov dword ptr [esp + 8], 0
            mov dword ptr [esp + 0xc], 0
    L0054a902:
            mov edx, dword ptr [esp + 0x1c]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            _emit 0xd8
        _emit 0x0a
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xdc]
            fst dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 0x41
            jne L0054a9d9
            mov esi, dword ptr [esp + 0x14]
            fld dword ptr [esi + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fmul dword ptr [esi + 0x30]
            fnstsw ax
            test ah, 0x40
            jne L0054a9ca
            fld dword ptr [esp + 0x20]
            fadd st(0), st(0)
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0a
            fxch st(1)
            fsub dword ptr [esp + 8]
            fxch st(2)
            fsub dword ptr [esp + 0xc]
            fxch st(1)
            fsub dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [rend+0x44]
            fxch st(1)
            fmul dword ptr [rend+0x48]
            fxch st(2)
            fmul dword ptr [rend+0x40]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [rendererStateStoredDispatch+0xe8]
            fnstsw ax
            test ah, 0x41
            jne L0054a9c8
            fld dword ptr [esi + 0x38]
            fmul st(0), st(1)
            fld dword ptr [esi + 0x34]
            fxch st(1)
            fsubr dword ptr [esi + 0x38]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fdivp st(1), st(0)
            faddp st(2), st(0)
    L0054a9c8:
            fstp st(0)
    L0054a9ca:
            fld dword ptr [ecx + 0x28]
            mov eax, dword ptr [esp + 0x24]
            fmul st(0), st(1)
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
    L0054a9d9:
            fstp st(0)
            pop esi
            add esp, 0xc
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054a9f0
void C2_NAKED C2_HOOK_STDCALL lightingIndexLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 pos_l;
    br_scalar len2;

    BrVector3Sub(&pos_l, &alp->position, p);
    len2 = BrVector3LengthSquared(&pos_l);
    if (len2 <= alp->s->attenuation_q && len2 >= BR_SCALAR_EPSILON) {
        comp[C_I] += self->state.surface.kd * (alp->s->attenuation_q - len2) / alp->s->attenuation_q * alp->s->attenuation_c;
    }
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x14]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            _emit 0xd8
        _emit 0x21
            fxch st(1)
            fsub dword ptr [ecx + 4]
            fld dword ptr [eax + 0xc]
            fsub dword ptr [ecx + 8]
            fxch st(2)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fst dword ptr [esp + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [esp + 4]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            mov ecx, dword ptr [eax + 0x34]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x1c]
            fstp st(0)
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [ecx + 0x34]
            fnstsw ax
            test ah, 0x41
            je L0054aa7a
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0xe4]
            fnstsw ax
            test ah, 1
            jne L0054aa7a
            fld dword ptr [ecx + 0x34]
            fsub dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x10]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [eax + 0x30]
            fmulp st(2), st(0)
            mov eax, dword ptr [esp + 0x20]
            fdivp st(1), st(0)
            fmul dword ptr [ecx + 0x30]
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
    L0054aa7a:
            add esp, 0xc
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054aa80
void C2_NAKED C2_HOOK_STDCALL lightingIndexLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 pos_l;
    br_scalar len2;

    BrVector3Sub(&pos_l, &alp->position, p);
    len2 = BrVector3LengthSquared(&pos_l);
    if (len2 <= alp->s->attenuation_q && len2 >= BR_SCALAR_EPSILON) {
        br_scalar dot;

        dot = BrVector3Dot(&pos_l, n);
        if (dot >= 0.f) {
            comp[C_I] += self->state.surface.kd * dot * (alp->s->attenuation_q - len2) / (alp->s->attenuation_q * (br_scalar)sqrt(len2)) * alp->s->attenuation_c;
        }
    }
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x14]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            _emit 0xd8
        _emit 0x21
            fxch st(1)
            fsub dword ptr [ecx + 4]
            fld dword ptr [eax + 0xc]
            fsub dword ptr [ecx + 8]
            fxch st(2)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fst dword ptr [esp + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fstp dword ptr [esp + 8]
            mov ecx, dword ptr [eax + 0x34]
            fmul dword ptr [esp + 4]
            fld dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x1c]
            fcomp dword ptr [ecx + 0x34]
            fnstsw ax
            test ah, 0x41
            je L0054ab5f
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0xe4]
            fnstsw ax
            test ah, 1
            jne L0054ab5f
            fld dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x18]
            fsqrt
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fmul dword ptr [esp + 4]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            _emit 0xd8
        _emit 0x08
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 1
            jne L0054ab5b
            mov eax, dword ptr [esp + 0x10]
            fld dword ptr [eax + 0x30]
            fld dword ptr [ecx + 0x34]
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fmul st(0), st(2)
            fld dword ptr [ecx + 0x34]
            fmul st(0), st(4)
            fxch st(1)
            fmulp st(2), st(0)
            fdivp st(1), st(0)
            mov eax, dword ptr [esp + 0x20]
            fmul dword ptr [ecx + 0x30]
            fstp st(2)
            fstp st(0)
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
            add esp, 0xc
            ret 0x14
    L0054ab5b:
            fstp st(0)
            fstp st(0)
    L0054ab5f:
            add esp, 0xc
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054ab70
void C2_NAKED C2_HOOK_STDCALL lightingIndexPointAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar attn, dot, l, dist, dist2;
	br_vector3 dirn, dirn_norm;

	BrVector3Sub(&dirn, &alp->position, p);
    dist = BrVector3Length(&dirn);
    if (dist <= BR_SCALAR_EPSILON) {
        return;
    }
    dist2 = dist < 180.f ? dist * dist : 32767.f;
    BrVector3InvScale(&dirn_norm, &dirn, dist);
    dot = BrVector3Dot(&dirn_norm, n);
    if (dot <= 0.f) {
        return;
    }
    l = dot * self->state.surface.kd;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn_norm);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            l += SPECULAR_POWER(self->state.surface.ks);
        }
    }
    attn = 1.f / (alp->s->attenuation_q * dist2 + alp->s->attenuation_l * dist + alp->s->attenuation_c);
    comp[C_I] += l * attn;
#else
    __asm {
            sub esp, 0xc
            mov ecx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x14]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
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
            fst dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054ad0c
            fld dword ptr [esp + 0x1c]
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0xfc]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fstp dword ptr [esp + 4]
            fstp dword ptr [esp + 8]
            fnstsw ax
            fstp st(0)
            fstp st(0)
            test ah, 1
            fstp st(0)
            jne L0054ac04
            fld dword ptr [rendererStateStoredDispatch+0x100]
            jmp L0054ac0c
    L0054ac04:
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
    L0054ac0c:
            mov eax, dword ptr [ecx + 0x34]
            mov ecx, dword ptr [esp + 0x18]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [eax + 0x34]
            fmulp st(2), st(0)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 8]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [ecx + 4]
            fxch st(2)
            fadd dword ptr [eax + 0x30]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(3)
            fmul dword ptr [esp + 4]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xdc]
            fst dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 0x41
            jne L0054ad0e
            mov edx, dword ptr [esp + 0x10]
            fld dword ptr [edx + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fmul dword ptr [edx + 0x30]
            fnstsw ax
            test ah, 0x40
            jne L0054acf8
            fld dword ptr [esp + 0x1c]
            fadd st(0), st(0)
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fsub dword ptr [esp + 4]
            fxch st(2)
            fsub dword ptr [esp + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x64
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fmul dword ptr [rend+0x44]
            fxch st(1)
            fmul dword ptr [rend+0x48]
            fxch st(2)
            fmul dword ptr [rend+0x40]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [rendererStateStoredDispatch+0xe8]
            fnstsw ax
            test ah, 0x41
            jne L0054acf6
            fld dword ptr [edx + 0x38]
            fmul st(0), st(1)
            fld dword ptr [edx + 0x34]
            fxch st(1)
            fsubr dword ptr [edx + 0x38]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fdivp st(1), st(0)
            faddp st(2), st(0)
    L0054acf6:
            fstp st(0)
    L0054acf8:
            mov eax, dword ptr [esp + 0x20]
            fmul st(0), st(1)
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
            fstp st(0)
            add esp, 0xc
            ret 0x14
    L0054ad0c:
            fstp st(0)
    L0054ad0e:
            fstp st(0)
            fstp st(0)
            add esp, 0xc
            ret 0x14
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

// FUNCTION: CARMA2_HW 0x0054ad20
void C2_NAKED C2_HOOK_STDCALL lightingIndexSpot(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot_spot, dot, l, attn;
    br_vector3 dirn, dirn_norm;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn_norm, &dirn);

    dot_spot = BrVector3Dot(&dirn_norm, &alp->direction);
    if (dot_spot < alp->s->spot_outer) {
        return;
    }
    dot = BrVector3Dot(&dirn_norm, n);
    if (dot <= 0.f) {
        return;
    }
    attn = (dot_spot < alp->s->spot_inner) ? alp->intensity * (alp->s->spot_outer - dot_spot) / (alp->s->spot_outer - alp->s->spot_inner) : alp->intensity;
    l = dot * self->state.surface.kd;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn_norm);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            l += SPECULAR_POWER(self->state.surface.ks);
        }
    }
    comp[C_I] += attn * l;
#else
    __asm {
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 8]
            sub esp, 0xc
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
            push esi
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
            fcom dword ptr [rendererStateStoredDispatch+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054ad92
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 4]
            fxch st(1)
            fstp dword ptr [esp + 8]
            fstp dword ptr [esp + 0xc]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L0054adb2
    L0054ad92:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 4], 0x3f800000
            mov dword ptr [esp + 8], 0
            mov dword ptr [esp + 0xc], 0
    L0054adb2:
            fld dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [esp + 4]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            mov edx, dword ptr [ecx + 0x34]
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [edx + 0x24]
            fnstsw ax
            test ah, 1
            jne L0054aef3
            mov esi, dword ptr [esp + 0x1c]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x18]
            fst dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            jne L0054aef3
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [edx + 0x28]
            fnstsw ax
            test ah, 1
            je L0054ae4f
            fld dword ptr [edx + 0x24]
            fsub dword ptr [esp + 0x20]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [edx + 0x24]
            fsub dword ptr [edx + 0x28]
            fxch st(1)
            fmulp st(2), st(0)
            fdivp st(1), st(0)
            jmp L0054ae52
    L0054ae4f:
            fld dword ptr [ecx + 0x28]
    L0054ae52:
            mov ecx, dword ptr [esp + 0x14]
            fld dword ptr [ecx + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fld dword ptr [ecx + 0x30]
            fmul dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 0x40
            jne L0054aee3
            fld dword ptr [esp + 0x18]
            fadd st(0), st(0)
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fxch st(1)
            fsub dword ptr [esp + 0xc]
            fxch st(2)
            fsub dword ptr [esp + 8]
            fxch st(1)
            fsub dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [rend+0x48]
            fxch st(1)
            fmul dword ptr [rend+0x44]
            fxch st(2)
            fmul dword ptr [rend+0x40]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [rendererStateStoredDispatch+0xe8]
            fnstsw ax
            test ah, 0x41
            jne L0054aee1
            fld dword ptr [ecx + 0x38]
            fmul st(0), st(1)
            fld dword ptr [ecx + 0x34]
            fxch st(1)
            fsubr dword ptr [ecx + 0x38]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fdivp st(1), st(0)
            faddp st(2), st(0)
    L0054aee1:
            fstp st(0)
    L0054aee3:
            mov eax, dword ptr [esp + 0x24]
            fxch st(1)
            fmul st(0), st(1)
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
            fstp st(0)
    L0054aef3:
            pop esi
            add esp, 0xc
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

// FUNCTION: CARMA2_HW 0x0054af00
void C2_NAKED C2_HOOK_STDCALL lightingIndexSpotAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dist, dist2, dot_spot, dot, l, attn;
    br_vector3 dirn, dirn_norm;

    BrVector3Sub(&dirn, &alp->position, p);
    dist = BrVector3Length(&dirn);
    if (dist <= BR_SCALAR_EPSILON) {
        return;
    }
    BrVector3InvScale(&dirn_norm, &dirn, dist);
    dot_spot = BrVector3Dot(&alp->direction, &dirn_norm);
    if (dot_spot < alp->s->spot_outer) {
        return;
    }
    dot = BrVector3Dot(&dirn_norm, n);
    if (dot <= 0.f) {
        return;
    }
    dist2 = dist < 180.f ? dist * dist : 32767.f;
    attn = 1.f / (alp->s->attenuation_q * dist2 + alp->s->attenuation_l * dist + alp->s->attenuation_c);
    if (alp->s->spot_inner > dot_spot) {
        attn *= (alp->s->spot_outer - dot_spot) / (alp->s->spot_outer - alp->s->spot_inner);
    }
    l = dot * self->state.surface.kd;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn_norm);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            l += SPECULAR_POWER(self->state.surface.ks);
        }
    }
    comp[C_I] += attn * l;
#else
    __asm {
            sub esp, 0x10
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x18]
            push esi
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
            fxch st(1)
            fld st(2)
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 0x24]
            fcomp dword ptr [rendererStateStoredDispatch+0xec]
            fnstsw ax
            test ah, 0x41
            jne L0054b101
            fld dword ptr [esp + 0x24]
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            mov edx, dword ptr [ecx + 0x34]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            fmul st(0), st(4)
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x1c]
            fstp st(0)
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [edx + 0x24]
            fnstsw ax
            test ah, 1
            jne L0054b103
            mov esi, dword ptr [esp + 0x20]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x20]
            fst dword ptr [esp + 4]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fnstsw ax
            test ah, 0x41
            jne L0054b103
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [rendererStateStoredDispatch+0xfc]
            fnstsw ax
            test ah, 1
            jne L0054b022
            fld dword ptr [rendererStateStoredDispatch+0x100]
            jmp L0054b02a
    L0054b022:
            fld dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x24]
    L0054b02a:
            fld dword ptr [edx + 0x2c]
            fld dword ptr [edx + 0x34]
            fmulp st(2), st(0)
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(2), st(0)
            fcomp dword ptr [edx + 0x28]
            fadd dword ptr [edx + 0x30]
            fnstsw ax
            fdivr qword ptr [rendererStateStoredDispatch+0xf0]
            test ah, 1
            je L0054b064
            fld dword ptr [edx + 0x24]
            fsub dword ptr [esp + 0x1c]
            fld dword ptr [edx + 0x24]
            fsub dword ptr [edx + 0x28]
            fxch st(1)
            fmulp st(2), st(0)
            fdivp st(1), st(0)
    L0054b064:
            mov ecx, dword ptr [esp + 0x18]
            fld dword ptr [ecx + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xdc]
            fld dword ptr [ecx + 0x30]
            fmul dword ptr [esp + 4]
            fnstsw ax
            test ah, 0x40
            jne L0054b0f5
            fld dword ptr [esp + 0x20]
            fadd st(0), st(0)
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fxch st(1)
            fsub dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0xc]
            fxch st(1)
            fsub dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [rend+0x48]
            fxch st(1)
            fmul dword ptr [rend+0x44]
            fxch st(2)
            fmul dword ptr [rend+0x40]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [rendererStateStoredDispatch+0xe8]
            fnstsw ax
            test ah, 0x41
            jne L0054b0f3
            fld dword ptr [ecx + 0x38]
            fmul st(0), st(1)
            fld dword ptr [ecx + 0x34]
            fxch st(1)
            fsubr dword ptr [ecx + 0x38]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fdivp st(1), st(0)
            faddp st(2), st(0)
    L0054b0f3:
            fstp st(0)
    L0054b0f5:
            mov eax, dword ptr [esp + 0x28]
            fmul st(0), st(1)
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x28]
    L0054b101:
            fstp st(0)
    L0054b103:
            pop esi
            add esp, 0x10
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

// FUNCTION: CARMA2_HW 0x0054a5f0
void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp) {
    switch (alp->type) {
    case BRT_DIRECT:
        alp->accumulate_index = lightingIndexDirect;
        break;
    case BRT_POINT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_index = lightingIndexPoint;
        } else {
            alp->accumulate_index = lightingIndexPointAttn;
        }
        break;
    case BRT_SPOT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_index = lightingIndexSpot;
        } else {
            alp->accumulate_index = lightingIndexSpotAttn;
        }
        break;
    case BRT_POINT_LOCAL_1:
        alp->accumulate_index = lightingIndexLocal1;
        break;
    case BRT_POINT_LOCAL_2:
        alp->accumulate_index = lightingIndexLocal2;
        break;
    default:
        alp->accumulate_index = lightingIndexNull;
        break;
    }
}
