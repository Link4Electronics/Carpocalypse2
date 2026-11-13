#include "light24.h"

#include "lightmac.h"
#include "setup.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34TApplyV();
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
extern static_cache_soft scache;
#endif
// FUNCTION: CARMA2_HW 0x00548c10
void C2_HOOK_CDECL SurfaceColourZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_R] = self->state.cache.comp_offsets[C_R];
    comp[C_G] = self->state.cache.comp_offsets[C_G];
    comp[C_B] = self->state.cache.comp_offsets[C_B];
}

// FUNCTION: CARMA2_HW 0x00548c40
void C2_NAKED C2_HOOK_CDECL SurfaceColourUnlit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING

    comp[C_R] = self->state.cache.comp_scales[C_R] * (float)BR_RED(colour) / 256.f + self->state.cache.comp_offsets[C_R];
    comp[C_G] = self->state.cache.comp_scales[C_G] * (float)BR_GRN(colour) / 256.f + self->state.cache.comp_offsets[C_G];
    comp[C_B] = self->state.cache.comp_scales[C_B] * (float)BR_BLU(colour) / 256.f + self->state.cache.comp_offsets[C_B];
#else
    __asm {
            sub esp, 0x10
            mov ecx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 4], 0
            mov eax, ecx
            mov edx, ecx
            shr eax, 0x10
            and eax, 0xff
            mov dword ptr [esp + 0xc], 0
            mov dword ptr [esp], eax
            mov eax, dword ptr [esp + 0x14]
            _emit 0xdf
        _emit 0x6c
        _emit 0x24
        _emit 0x00
            shr edx, 8
            and edx, 0xff
            and ecx, 0xff
            fmul dword ptr [rendererStateStoredDispatch+0xa0]
            mov dword ptr [esp + 8], edx
            mov edx, dword ptr [esp + 0x28]
            fmul dword ptr [eax + 0x610]
            fild qword ptr [esp + 8]
            fxch st(1)
            fadd dword ptr [eax + 0x5d0]
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xa0]
            fxch st(1)
            fstp dword ptr [edx + 0x2c]
            fmul dword ptr [eax + 0x614]
            mov dword ptr [esp + 8], ecx
            mov dword ptr [esp + 0xc], 0
            fild qword ptr [esp + 8]
            fxch st(1)
            fadd dword ptr [eax + 0x5d4]
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xa0]
            fxch st(1)
            fstp dword ptr [edx + 0x30]
            fmul dword ptr [eax + 0x618]
            fadd dword ptr [eax + 0x5d8]
            fstp dword ptr [edx + 0x34]
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00548cf0
void C2_NAKED C2_HOOK_CDECL SurfaceColourLit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    active_light* alp;
    br_vector3 vp;
    br_vector3 vn;
    br_vector3 fvn;
    br_scalar red, green, blue;

    red = BrFixedToScalar(BR_RED(colour) << 8);
    green = BrFixedToScalar(BR_GRN(colour) << 8);
    blue = BrFixedToScalar(BR_BLU(colour) << 8);

    comp[C_R] = self->state.surface.ka * red;
    comp[C_G] = self->state.surface.ka * green;
    comp[C_B] = self->state.surface.ka * blue;

    BrVector3Copy(&rend.eye_l, &scache.eye_m_normalised);

    alp = scache.lights;

    for (i = 0; i < scache.nlights_model; i++, alp++) {
        alp->accumulate_colour(self, p, n, colour, alp, comp);
    }

    if (scache.nlights_view != 0) {
        BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
        BrMatrix34TApplyV(&vn, n, &scache.view_to_model);
        BrVector3Normalise(&fvn, &vn);

        BrVector3Set(&rend.eye_l, 0.f, 0.f, 1.f);

        for (i = 0; i < scache.nlights_view; i++, alp++) {
            alp->accumulate_colour(self, &vp, &fvn, colour, alp, comp);
        }
    }

    CLAMP_SCALE(C_R);
    CLAMP_SCALE(C_G);
    CLAMP_SCALE(C_B);
#else
    __asm {
            sub esp, 0x24
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x40]
            xor eax, eax
            mov ecx, ebp
            mov dword ptr [esp + 0xc], eax
            shr ecx, 8
            and ecx, 0xff00
            mov edx, ebp
            mov dword ptr [esp + 8], ecx
            and edx, 0xff00
            fild qword ptr [esp + 8]
            mov dword ptr [esp + 8], edx
            mov dword ptr [esp + 0xc], eax
            mov ebx, dword ptr [esp + 0x44]
            push esi
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fild qword ptr [esp + 0xc]
            push edi
            mov edi, dword ptr [esp + 0x38]
            mov ecx, ebp
            mov dword ptr [esp + 0x14], eax
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(1)
            fmul dword ptr [edi + 0x2c]
            and ecx, 0xff
            mov dword ptr [esp + 0x48], eax
            shl ecx, 8
            fstp dword ptr [ebx + 0x2c]
            fld dword ptr [edi + 0x2c]
            mov dword ptr [esp + 0x10], ecx
            mov esi, offset scache+0x118
            fild qword ptr [esp + 0x10]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            fstp dword ptr [ebx + 0x30]
            fmul dword ptr [edi + 0x2c]
            fstp dword ptr [ebx + 0x34]
            mov edx, dword ptr [scache+0x104]
            mov eax, dword ptr [scache+0x108]
            mov ecx, dword ptr [scache+0x10c]
            cmp dword ptr [scache+0x498], 0
            mov dword ptr [rend+0x40], edx
            mov dword ptr [rend+0x44], eax
            mov dword ptr [rend+0x48], ecx
            jle L00548dce
    L00548da7:
            mov edx, dword ptr [esp + 0x44]
            mov eax, dword ptr [esp + 0x3c]
            push ebx
            push esi
            push ebp
            push edx
            push eax
            push edi
            call dword ptr [esi + 0x30]
            mov eax, dword ptr [esp + 0x48]
            mov ecx, dword ptr [scache+0x498]
            inc eax
            add esi, 0x38
            cmp eax, ecx
            mov dword ptr [esp + 0x48], eax
            jl L00548da7
    L00548dce:
            mov eax, dword ptr [scache+0x49c]
            test eax, eax
            je L00548ed4
            mov edx, dword ptr [esp + 0x3c]
            lea ecx, [edi + 0x4ac]
            push ecx
            lea eax, [esp + 0x2c]
            push edx
            push eax
            call BrMatrix34ApplyP
            mov ecx, dword ptr [esp + 0x50]
            add esp, 0xc
            lea edx, [esp + 0x10]
            push offset scache+0x40
            push ecx
            push edx
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
            fcom dword ptr [rendererStateStoredDispatch+0xa8]
            fnstsw ax
            test ah, 0x41
            jne L00548e64
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
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
            jmp L00548e7e
    L00548e64:
            fstp st(0)
            mov dword ptr [esp + 0x1c], 0x3f800000
            mov dword ptr [esp + 0x20], 0
            mov dword ptr [esp + 0x24], 0
    L00548e7e:
            mov eax, dword ptr [scache+0x49c]
            mov dword ptr [rend+0x40], 0
            test eax, eax
            mov dword ptr [rend+0x44], 0
            mov dword ptr [rend+0x48], 0x3f800000
            mov dword ptr [esp + 0x48], 0
            jle L00548ed4
    L00548ead:
            push ebx
            push esi
            lea eax, [esp + 0x24]
            push ebp
            lea ecx, [esp + 0x34]
            push eax
            push ecx
            push edi
            call dword ptr [esi + 0x30]
            mov eax, dword ptr [esp + 0x48]
            mov ecx, dword ptr [scache+0x49c]
            inc eax
            add esi, 0x38
            cmp eax, ecx
            mov dword ptr [esp + 0x48], eax
            jl L00548ead
    L00548ed4:
            fld dword ptr [ebx + 0x2c]
            fcomp dword ptr [rendererStateStoredDispatch+0xb8]
            fnstsw ax
            test ah, 1
            jne L00548ef8
            fld dword ptr [edi + 0x5d0]
            fadd dword ptr [edi + 0x610]
            fsub dword ptr [rendererStateStoredDispatch+0xc0]
            jmp L00548f22
    L00548ef8:
            fld dword ptr [ebx + 0x2c]
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fnstsw ax
            test ah, 0x41
            je L00548f13
            mov edx, dword ptr [edi + 0x5d0]
            mov dword ptr [ebx + 0x2c], edx
            jmp L00548f25
    L00548f13:
            fld dword ptr [edi + 0x610]
            fmul dword ptr [ebx + 0x2c]
            fadd dword ptr [edi + 0x5d0]
    L00548f22:
            fstp dword ptr [ebx + 0x2c]
    L00548f25:
            fld dword ptr [ebx + 0x30]
            fcomp dword ptr [rendererStateStoredDispatch+0xb8]
            fnstsw ax
            test ah, 1
            jne L00548f49
            fld dword ptr [edi + 0x5d4]
            fadd dword ptr [edi + 0x614]
            fsub dword ptr [rendererStateStoredDispatch+0xc0]
            jmp L00548f73
    L00548f49:
            fld dword ptr [ebx + 0x30]
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fnstsw ax
            test ah, 0x41
            je L00548f64
            mov eax, dword ptr [edi + 0x5d4]
            mov dword ptr [ebx + 0x30], eax
            jmp L00548f76
    L00548f64:
            fld dword ptr [edi + 0x614]
            fmul dword ptr [ebx + 0x30]
            fadd dword ptr [edi + 0x5d4]
    L00548f73:
            fstp dword ptr [ebx + 0x30]
    L00548f76:
            fld dword ptr [ebx + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xb8]
            fnstsw ax
            test ah, 1
            jne L00548fa3
            fld dword ptr [edi + 0x5d8]
            fadd dword ptr [edi + 0x618]
            fsub dword ptr [rendererStateStoredDispatch+0xc0]
            fstp dword ptr [ebx + 0x34]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L00548fa3:
            fld dword ptr [ebx + 0x34]
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fnstsw ax
            test ah, 0x41
            je L00548fc4
            mov ecx, dword ptr [edi + 0x5d8]
            mov dword ptr [ebx + 0x34], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    L00548fc4:
            fld dword ptr [edi + 0x618]
            fmul dword ptr [ebx + 0x34]
            fadd dword ptr [edi + 0x5d8]
            pop edi
            pop esi
            pop ebp
            fstp dword ptr [ebx + 0x34]
            pop ebx
            add esp, 0x24
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005491a0
void C2_HOOK_STDCALL lightingColourNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

}

// FUNCTION: CARMA2_HW 0x005491b0
void C2_NAKED C2_HOOK_STDCALL lightingColourDirectSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_scalar red, grn, blu;

    dot = BrVector3Dot(&alp->direction, n);
    if (dot <= 0.f) {
        return;
    }

    red = BrFixedToFloat(BR_RED(colour) << 8) * dot;
    grn = BrFixedToFloat(BR_GRN(colour) << 8) * dot;
    blu = BrFixedToFloat(BR_BLU(colour) << 8) * dot;

    if (self->state.surface.ks != 0.f) {
        dot = BrVector3Dot(&alp->half, n);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = alp->intensity * SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
#else
    __asm {
            mov ecx, dword ptr [esp + 0x14]
            sub esp, 0x18
            fld dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 0x14]
            push edi
            mov edi, dword ptr [esp + 0x28]
            fld dword ptr [edi + 8]
            fld dword ptr [edi + 4]
            fld dword ptr [ecx + 0x10]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x0f
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xbc]
            fnstsw ax
            test ah, 0x41
            jne L0054936f
            mov eax, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x20]
            push esi
            mov esi, eax
            fld dword ptr [edx + 0x30]
            shr esi, 8
            and esi, 0xff00
            push ebx
            mov dword ptr [esp + 0xc], esi
            xor esi, esi
            mov ebx, eax
            mov dword ptr [esp + 0x10], esi
            fild qword ptr [esp + 0xc]
            and eax, 0xff00
            and ebx, 0xff
            mov dword ptr [esp + 0xc], eax
            mov dword ptr [esp + 0x10], esi
            fild qword ptr [esp + 0xc]
            shl ebx, 8
            mov dword ptr [esp + 0xc], ebx
            mov dword ptr [esp + 0x10], esi
            fild qword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(3)
            fmulp st(4), st(0)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fld dword ptr [edx + 0x34]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(2)
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp st(3)
            fstp dword ptr [esp + 0x38]
            fnstsw ax
            fstp dword ptr [esp + 0x30]
            test ah, 0x40
            jne L005492e4
            fld dword ptr [ecx + 0x24]
            fld dword ptr [edi + 8]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 0x1c]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [edi + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x0f
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xc4]
            fnstsw ax
            test ah, 0x41
            jne L005492e2
            fld dword ptr [edx + 0x38]
            fld dword ptr [ecx + 0x28]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul dword ptr [edx + 0x34]
            fxch st(1)
            fsubr dword ptr [edx + 0x38]
            fadd st(0), st(2)
            fxch st(1)
            fmulp st(2), st(0)
            fdivp st(1), st(0)
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x30]
            fxch st(3)
            fadd st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fxch st(3)
            fadd st(0), st(2)
            fxch st(1)
            fxch st(3)
            fstp dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x30]
    L005492e2:
            fstp st(0)
    L005492e4:
            mov edx, dword ptr [ecx + 0x34]
            mov dword ptr [esp + 0x10], esi
            mov eax, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x18], esi
            mov edx, dword ptr [edx + 0x20]
            mov dword ptr [esp + 0x20], esi
            shr edx, 8
            and edx, 0xff00
            pop ebx
            mov dword ptr [esp + 8], edx
            pop esi
            fild qword ptr [esp + 4]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmul st(0), st(1)
            fadd dword ptr [eax + 0x2c]
            fstp dword ptr [eax + 0x2c]
            mov edx, dword ptr [ecx + 0x34]
            fstp st(0)
            mov edx, dword ptr [edx + 0x20]
            and edx, 0xff00
            mov dword ptr [esp + 0xc], edx
            fild qword ptr [esp + 0xc]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmul dword ptr [esp + 0x30]
            fadd dword ptr [eax + 0x30]
            fstp dword ptr [eax + 0x30]
            mov ecx, dword ptr [ecx + 0x34]
            mov edx, dword ptr [ecx + 0x20]
            and edx, 0xff
            shl edx, 8
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x14]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmul dword ptr [esp + 0x28]
            fadd dword ptr [eax + 0x34]
            fstp dword ptr [eax + 0x34]
            pop edi
            add esp, 0x18
            ret 0x18
    L0054936f:
            fstp st(0)
            pop edi
            add esp, 0x18
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

// FUNCTION: CARMA2_HW 0x00549380
void C2_NAKED C2_HOOK_STDCALL lightingColourLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_scalar dist2;
	br_vector3 dirn;

    BrVector3Sub(&dirn, &alp->position, p);
    dist2 = BrVector3LengthSquared(&dirn);
    if (dist2 <= alp->s->attenuation_q && dist2 >= BR_SCALAR_EPSILON) {
        dot = BrVector3Dot(&dirn, n);
        if (dot >= 0.f) {
            br_scalar attn;

            attn = self->state.surface.kd * dot * (alp->s->attenuation_q - dist2) / (alp->s->attenuation_q * (br_scalar)sqrt(dist2));
            comp[C_R] += attn * BrFixedToFloat(BR_RED(colour)) * BR_RED(alp->s->colour);
            comp[C_G] += attn * BrFixedToFloat(BR_GRN(colour)) * BR_GRN(alp->s->colour);
            comp[C_B] += attn * BrFixedToFloat(BR_BLU(colour)) * BR_BLU(alp->s->colour);
        }
    }
#else
    __asm {
            sub esp, 0x1c
            push ebx
            mov eax, dword ptr [esp + 0x28]
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x40]
            fld dword ptr [edi + 8]
            fld dword ptr [edi + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [edi + 0xc]
            fsub dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            mov ecx, dword ptr [edi + 0x34]
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x40]
            fcomp dword ptr [ecx + 0x34]
            fnstsw ax
            test ah, 0x41
            je L005494fa
            fld dword ptr [esp + 0x40]
            fcomp dword ptr [rendererStateStoredDispatch+0xc0]
            fnstsw ax
            test ah, 1
            jne L005494fa
            fld dword ptr [esp + 0x40]
            mov eax, dword ptr [esp + 0x38]
            fsqrt
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            _emit 0xd8
        _emit 0x08
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xbc]
            fnstsw ax
            test ah, 1
            jne L005494f6
            mov eax, dword ptr [esp + 0x30]
            xor edx, edx
            mov dl, byte ptr [esp + 0x3e]
            mov dword ptr [esp + 0x14], 0
            fld dword ptr [eax + 0x30]
            fld dword ptr [ecx + 0x34]
            fsub dword ptr [esp + 0x40]
            fxch st(1)
            fmul st(0), st(2)
            fld dword ptr [ecx + 0x34]
            xor eax, eax
            mov esi, dword ptr [esp + 0x44]
            mov al, byte ptr [ecx + 0x22]
            xor ebx, ebx
            imul edx, eax
            fmul st(0), st(4)
            fxch st(1)
            fmulp st(2), st(0)
            mov dword ptr [esp + 0x10], edx
            xor ecx, ecx
            fdivp st(1), st(0)
            mov cl, byte ptr [esp + 0x3d]
            mov dword ptr [esp + 0x1c], ebx
            mov dword ptr [esp + 0x24], ebx
            mov ebp, dword ptr [esp + 0x3c]
            and ebp, 0xff
            mov dword ptr [esp + 0x3c], ebp
            fild qword ptr [esp + 0x10]
            fxch st(1)
            fstp st(3)
            fmul st(0), st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xc8]
            fsubr dword ptr [esi + 0x2c]
            fstp dword ptr [esi + 0x2c]
            mov edx, dword ptr [edi + 0x34]
            fstp st(0)
            mov bl, byte ptr [edx + 0x21]
            imul ecx, ebx
            mov dword ptr [esp + 0x18], ecx
            fild qword ptr [esp + 0x18]
            fmul st(0), st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xc8]
            fsubr dword ptr [esi + 0x30]
            fstp dword ptr [esi + 0x30]
            mov eax, dword ptr [edi + 0x34]
            mov ecx, dword ptr [eax + 0x20]
            mov eax, ebp
            and ecx, 0xff
            imul eax, ecx
            mov dword ptr [esp + 0x20], eax
            fild qword ptr [esp + 0x20]
            fmul st(0), st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xc8]
            fsubr dword ptr [esi + 0x34]
            fstp dword ptr [esi + 0x34]
            fstp st(0)
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x18
    L005494f6:
            fstp st(0)
            fstp st(0)
    L005494fa:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x18
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

// FUNCTION: CARMA2_HW 0x00549510
void C2_NAKED C2_HOOK_STDCALL lightingColourLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dist2;
	br_vector3 dirn;

    BrVector3Sub(&dirn, &alp->position, p);
    dist2 = BrVector3LengthSquared(&dirn);
    if (dist2 <= alp->s->attenuation_q && dist2 >= BR_SCALAR_EPSILON) {
        br_scalar attn;

        attn = self->state.surface.kd * (alp->s->attenuation_q - dist2) / alp->s->attenuation_q ;
        comp[C_R] += attn * BrFixedToFloat(BR_RED(colour)) * BR_RED(alp->s->colour);
        comp[C_G] += attn * BrFixedToFloat(BR_GRN(colour)) * BR_GRN(alp->s->colour);
        comp[C_B] += attn * BrFixedToFloat(BR_BLU(colour)) * BR_BLU(alp->s->colour);
    }
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [esp + 0x14]
            push esi
            mov esi, dword ptr [esp + 0x24]
            push edi
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [esi + 0xc]
            fsub dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            mov edi, dword ptr [esi + 0x34]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x28]
            fstp st(0)
            fld dword ptr [esp + 0x28]
            fcomp dword ptr [edi + 0x34]
            fnstsw ax
            test ah, 0x41
            je L00549623
            fld dword ptr [esp + 0x28]
            fcomp dword ptr [rendererStateStoredDispatch+0xc0]
            fnstsw ax
            test ah, 1
            jne L00549623
            fld dword ptr [edi + 0x34]
            fsub dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x18]
            xor ecx, ecx
            mov cl, byte ptr [esp + 0x26]
            xor edx, edx
            fmul dword ptr [eax + 0x30]
            mov dl, byte ptr [edi + 0x22]
            mov eax, dword ptr [esp + 0x2c]
            imul ecx, edx
            fdiv dword ptr [edi + 0x34]
            xor edi, edi
            mov dword ptr [esp + 8], ecx
            mov dword ptr [esp + 0xc], edi
            mov ecx, dword ptr [esp + 0x24]
            push ebp
            push ebx
            xor edx, edx
            xor ebx, ebx
            mov dl, ch
            and ecx, 0xff
            fild qword ptr [esp + 0x10]
            mov dword ptr [esp + 0x14], edi
            fmul st(0), st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xc8]
            fsubr dword ptr [eax + 0x2c]
            fstp dword ptr [eax + 0x2c]
            mov ebp, dword ptr [esi + 0x34]
            mov bl, byte ptr [ebp + 0x21]
            imul edx, ebx
            mov dword ptr [esp + 0x10], edx
            pop ebx
            fild qword ptr [esp + 0xc]
            mov dword ptr [esp + 0x10], edi
            pop ebp
            fmul st(0), st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xc8]
            fsubr dword ptr [eax + 0x30]
            fstp dword ptr [eax + 0x30]
            mov edx, dword ptr [esi + 0x34]
            mov edx, dword ptr [edx + 0x20]
            and edx, 0xff
            imul ecx, edx
            mov dword ptr [esp + 8], ecx
            fild qword ptr [esp + 8]
            fmul st(0), st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xc8]
            fsubr dword ptr [eax + 0x34]
            fstp dword ptr [eax + 0x34]
            fstp st(0)
    L00549623:
            pop edi
            pop esi
            add esp, 0xc
            ret 0x18
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00549630
void C2_NAKED C2_HOOK_STDCALL lightingColourPointSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn, &dirn);

    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    red = BrFixedToScalar(BR_RED(colour) << 8) * self->state.surface.kd * dot;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * self->state.surface.kd * dot;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * self->state.surface.kd * dot;

    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp , &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * alp->intensity * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * alp->intensity * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * alp->intensity * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
#else
    __asm {
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 8]
            sub esp, 0x14
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
            push ebx
            push esi
            push edi
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
            fcom dword ptr [rendererStateStoredDispatch+0xa8]
            fnstsw ax
            test ah, 0x41
            jne L005496a4
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x1c]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L005496c4
    L005496a4:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x14], 0x3f800000
            mov dword ptr [esp + 0x18], 0
            mov dword ptr [esp + 0x1c], 0
    L005496c4:
            mov edi, dword ptr [esp + 0x2c]
            fld dword ptr [edi + 8]
            fld dword ptr [edi + 4]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            _emit 0xd8
        _emit 0x0f
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xbc]
            fst dword ptr [esp + 0x2c]
            fnstsw ax
            test ah, 0x41
            jne L005498d7
            mov eax, dword ptr [esp + 0x30]
            mov esi, dword ptr [esp + 0x24]
            mov edx, eax
            mov ebx, eax
            fld dword ptr [esi + 0x30]
            shr edx, 8
            and edx, 0xff00
            and eax, 0xff00
            mov dword ptr [esp + 0xc], edx
            xor edx, edx
            mov dword ptr [esp + 0x10], edx
            and ebx, 0xff
            fild qword ptr [esp + 0xc]
            mov dword ptr [esp + 0xc], eax
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0xc]
            shl ebx, 8
            mov dword ptr [esp + 0xc], ebx
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(3)
            fmulp st(4), st(0)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fld dword ptr [esi + 0x34]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(2)
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp st(3)
            fstp dword ptr [esp + 0x34]
            fnstsw ax
            fstp dword ptr [esp + 0x28]
            test ah, 0x40
            jne L00549831
            fld dword ptr [esp + 0x2c]
            fadd st(0), st(0)
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0f
            fxch st(1)
            fsub dword ptr [esp + 0x18]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fsub dword ptr [esp + 0x14]
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
            fcom dword ptr [rendererStateStoredDispatch+0xc4]
            fnstsw ax
            test ah, 0x41
            jne L0054982f
            fld dword ptr [esi + 0x38]
            fmul st(0), st(1)
            fld dword ptr [esi + 0x34]
            fxch st(1)
            fsubr dword ptr [esi + 0x38]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            fdivp st(3), st(0)
            fxch st(2)
            fstp st(3)
            fxch st(3)
            fadd st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fxch st(3)
            fadd st(0), st(2)
            fxch st(1)
            fxch st(3)
            fxch st(1)
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x28]
    L0054982f:
            fstp st(0)
    L00549831:
            mov eax, dword ptr [ecx + 0x34]
            mov dword ptr [esp + 0x10], edx
            fld dword ptr [ecx + 0x28]
            mov esi, dword ptr [eax + 0x20]
            mov eax, dword ptr [esp + 0x38]
            shr esi, 8
            and esi, 0xff00
            mov dword ptr [esp + 0x18], edx
            mov dword ptr [esp + 0xc], esi
            fild qword ptr [esp + 0xc]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(1), st(0)
            mov dword ptr [esp + 0x10], edx
            fadd dword ptr [eax + 0x2c]
            fstp dword ptr [eax + 0x2c]
            mov edx, dword ptr [ecx + 0x34]
            fstp st(0)
            mov edx, dword ptr [edx + 0x20]
            fld dword ptr [ecx + 0x28]
            and edx, 0xff00
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(1), st(0)
            fadd dword ptr [eax + 0x30]
            fstp dword ptr [eax + 0x30]
            mov edx, dword ptr [ecx + 0x34]
            fld dword ptr [ecx + 0x28]
            mov ecx, dword ptr [edx + 0x20]
            and ecx, 0xff
            shl ecx, 8
            mov dword ptr [esp + 0xc], ecx
            fild qword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(1), st(0)
            fadd dword ptr [eax + 0x34]
            fstp dword ptr [eax + 0x34]
            pop edi
            pop esi
            pop ebx
            add esp, 0x14
            ret 0x18
    L005498d7:
            pop edi
            pop esi
            fstp st(0)
            pop ebx
            add esp, 0x14
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005498f0
void C2_NAKED C2_HOOK_STDCALL lightingColourPointAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_scalar dist, dist2;
    br_scalar attn;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    dist = BrVector3Length(&dirn);
    if (dist <= BR_SCALAR_EPSILON) {
        return;
    }
    dist2 = (dist < 180.f) ? dist * dist : 32767.f;
    BrVector3InvScale(&dirn, &dirn, dist);
    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    red = BrFixedToScalar(BR_RED(colour) << 8) * self->state.surface.kd * dot;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * self->state.surface.kd * dot;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * self->state.surface.kd * dot;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > 0.f) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }
    attn = 1.f / (alp->s->attenuation_q * dist2 + alp->s->attenuation_l * dist + alp->s->attenuation_c);

    comp[C_R] += red * attn * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * attn * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * attn * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
#else
    __asm {
            sub esp, 0x14
            mov ecx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x1c]
            push ebx
            push ebp
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
            push esi
            push edi
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
            fst dword ptr [esp + 0x38]
            fcomp dword ptr [rendererStateStoredDispatch+0xa8]
            fnstsw ax
            test ah, 0x41
            jne L00549bc4
            fld dword ptr [esp + 0x38]
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [rendererStateStoredDispatch+0xd0]
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
            fnstsw ax
            fstp st(0)
            fstp st(0)
            test ah, 1
            fstp st(0)
            jne L00549988
            fld dword ptr [rendererStateStoredDispatch+0xd4]
            jmp L00549990
    L00549988:
            fld dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x38]
    L00549990:
            mov esi, dword ptr [ecx + 0x34]
            mov ebx, dword ptr [esp + 0x30]
            fld dword ptr [esi + 0x34]
            fld dword ptr [esi + 0x2c]
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fmul st(0), st(2)
            fld dword ptr [ebx + 4]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [ebx + 8]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fadd dword ptr [esi + 0x30]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            _emit 0xd8
        _emit 0x0b
            fxch st(3)
            faddp st(2), st(0)
            fstp st(3)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            fcom dword ptr [rendererStateStoredDispatch+0xbc]
            fst dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 0x41
            jne L00549bc8
            mov eax, dword ptr [esp + 0x34]
            mov edi, dword ptr [esp + 0x28]
            mov edx, eax
            mov ebp, eax
            fld dword ptr [edi + 0x30]
            shr edx, 8
            and edx, 0xff00
            and eax, 0xff00
            mov dword ptr [esp + 0x10], edx
            xor edx, edx
            mov dword ptr [esp + 0x14], edx
            and ebp, 0xff
            fild qword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x10]
            shl ebp, 8
            mov dword ptr [esp + 0x10], ebp
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(3)
            fmulp st(4), st(0)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fld dword ptr [edi + 0x34]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(2)
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp st(3)
            fnstsw ax
            test ah, 0x40
            fxch st(1)
            jne L00549b23
            fld dword ptr [esp + 0x38]
            fadd st(0), st(0)
            fld dword ptr [ebx + 8]
            fld dword ptr [ebx + 4]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0b
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fsub dword ptr [esp + 0x18]
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
            fst dword ptr [esp + 0x38]
            fstp st(1)
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fnstsw ax
            test ah, 0x41
            jne L00549b23
            fld dword ptr [edi + 0x38]
            fmul dword ptr [esp + 0x38]
            fld dword ptr [edi + 0x34]
            fxch st(1)
            fsubr dword ptr [edi + 0x38]
            fxch st(1)
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fadd dword ptr [esp + 0x38]
            fld st(2)
            fxch st(1)
            fdivp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(3)
            fadd dword ptr [esp + 0x38]
            fxch st(2)
            fadd dword ptr [esp + 0x38]
            fxch st(3)
            fadd dword ptr [esp + 0x38]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fstp st(1)
    L00549b23:
            mov esi, dword ptr [esi + 0x20]
            mov dword ptr [esp + 0x14], edx
            shr esi, 8
            and esi, 0xff00
            mov eax, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x1c], edx
            fild qword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x2c]
            fxch st(3)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            mov dword ptr [esp + 0x14], edx
            fmulp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [eax + 0x2c]
            fld st(1)
            fxch st(1)
            fstp dword ptr [eax + 0x2c]
            mov edx, dword ptr [ecx + 0x34]
            fmul dword ptr [esp + 0x2c]
            mov edx, dword ptr [edx + 0x20]
            and edx, 0xff00
            mov dword ptr [esp + 0x18], edx
            fild qword ptr [esp + 0x18]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(3), st(0)
            fxch st(2)
            fadd dword ptr [eax + 0x30]
            fstp dword ptr [eax + 0x30]
            mov ecx, dword ptr [ecx + 0x34]
            mov edx, dword ptr [ecx + 0x20]
            and edx, 0xff
            shl edx, 8
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0x10]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(2), st(0)
            fxch st(1)
            fadd dword ptr [eax + 0x34]
            fstp dword ptr [eax + 0x34]
            fstp st(0)
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0x18
    L00549bc4:
            fstp st(0)
            fstp st(0)
    L00549bc8:
            pop edi
            pop esi
            pop ebp
            pop ebx
            fstp st(0)
            add esp, 0x14
            ret 0x18
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

// FUNCTION: CARMA2_HW 0x00549be0
void C2_NAKED C2_HOOK_STDCALL lightingColourSpotSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_scalar dot_spot;
    br_scalar spot;
    br_scalar l;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn, &dirn);
    dot_spot = BrVector3Dot(&dirn, &alp->direction);
    if (dot_spot < alp->s->spot_outer) {
        return;
    }
    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    spot = (dot_spot >= alp->s->spot_inner) ? alp->intensity : alp->intensity * (alp->s->spot_outer - dot_spot) / (alp->s->spot_outer - alp->s->spot_inner);

    l = dot * self->state.surface.kd;
    red = BrFixedToScalar(BR_RED(colour) << 8) * l;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * l;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * l;

    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * spot * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * spot * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * spot * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
#else
    __asm {
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 8]
            sub esp, 0x14
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [eax + 8]
            push ebx
            push ebp
            push esi
            push edi
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
            fcom dword ptr [rendererStateStoredDispatch+0xa8]
            fnstsw ax
            test ah, 0x41
            jne L00549c55
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
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
            jmp L00549c75
    L00549c55:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x18], 0x3f800000
            mov dword ptr [esp + 0x1c], 0
            mov dword ptr [esp + 0x20], 0
    L00549c75:
            fld dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            mov edi, dword ptr [ecx + 0x34]
            fst dword ptr [esp + 0x38]
            fcomp dword ptr [edi + 0x24]
            fnstsw ax
            test ah, 1
            jne L00549eda
            mov ebx, dword ptr [esp + 0x30]
            fld dword ptr [ebx + 8]
            fld dword ptr [ebx + 4]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            _emit 0xd8
        _emit 0x0b
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xbc]
            fst dword ptr [esp + 0x2c]
            fnstsw ax
            test ah, 0x41
            jne L00549ed8
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [edi + 0x28]
            fnstsw ax
            test ah, 1
            je L00549d0e
            fld dword ptr [edi + 0x24]
            fsub dword ptr [esp + 0x38]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [edi + 0x24]
            fsub dword ptr [edi + 0x28]
            fxch st(1)
            fmulp st(2), st(0)
            fdivp st(1), st(0)
            jmp L00549d11
    L00549d0e:
            fld dword ptr [ecx + 0x28]
    L00549d11:
            mov eax, dword ptr [esp + 0x34]
            mov esi, dword ptr [esp + 0x28]
            mov edx, eax
            mov ebp, eax
            shr edx, 8
            and edx, 0xff00
            and eax, 0xff00
            fstp dword ptr [esp + 0x38]
            fld dword ptr [esi + 0x30]
            mov dword ptr [esp + 0x10], edx
            xor edx, edx
            mov dword ptr [esp + 0x14], edx
            and ebp, 0xff
            fild qword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x10]
            shl ebp, 8
            mov dword ptr [esp + 0x10], ebp
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(3)
            fmulp st(4), st(0)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fld dword ptr [esi + 0x34]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(2)
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp st(3)
            fnstsw ax
            test ah, 0x40
            fxch st(1)
            jne L00549e43
            fld dword ptr [esp + 0x2c]
            fadd st(0), st(0)
            fld dword ptr [ebx + 8]
            fld dword ptr [ebx + 4]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0b
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fsub dword ptr [esp + 0x18]
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
            fst dword ptr [esp + 0x2c]
            fstp st(1)
            fcomp dword ptr [rendererStateStoredDispatch+0xc4]
            fnstsw ax
            test ah, 0x41
            jne L00549e43
            fld dword ptr [esi + 0x38]
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [esi + 0x34]
            fxch st(1)
            fsubr dword ptr [esi + 0x38]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fadd dword ptr [esp + 0x2c]
            fld st(2)
            fxch st(1)
            fdivp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            fxch st(3)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [esp + 0x2c]
            fxch st(3)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fstp st(1)
    L00549e43:
            mov esi, dword ptr [edi + 0x20]
            mov dword ptr [esp + 0x14], edx
            shr esi, 8
            and esi, 0xff00
            mov eax, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x1c], edx
            fild qword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            mov dword ptr [esp + 0x14], edx
            fmulp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x38]
            fxch st(2)
            fadd dword ptr [eax + 0x2c]
            fld st(1)
            fxch st(1)
            fstp dword ptr [eax + 0x2c]
            mov edx, dword ptr [ecx + 0x34]
            fmul dword ptr [esp + 0x38]
            mov edx, dword ptr [edx + 0x20]
            and edx, 0xff00
            mov dword ptr [esp + 0x18], edx
            fild qword ptr [esp + 0x18]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(3), st(0)
            fxch st(2)
            fadd dword ptr [eax + 0x30]
            fstp dword ptr [eax + 0x30]
            mov ecx, dword ptr [ecx + 0x34]
            mov edx, dword ptr [ecx + 0x20]
            and edx, 0xff
            shl edx, 8
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0x10]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(2), st(0)
            fxch st(1)
            fadd dword ptr [eax + 0x34]
            fstp dword ptr [eax + 0x34]
    L00549ed8:
            fstp st(0)
    L00549eda:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00549ef0
void C2_NAKED C2_HOOK_STDCALL lightingColourSpotAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar dot;
    br_scalar dot_spot;
    br_scalar dist, dist2;
    br_scalar l;
    br_scalar attn;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    dist = BrVector3Length(&dirn);
    if (dist <= BR_SCALAR_EPSILON) {
        return;
    }
    BrVector3InvScale(&dirn, &dirn, dist);
    dot_spot = BrVector3Dot(&dirn, &alp->position);
    if (dot_spot < alp->s->spot_outer) {
        return;
    }
    dot = BrVector3Dot(&dirn, n);
    dist2 = (dist < 180.f) ? dist * dist : 32767.f;

    l = self->state.surface.kd * dot;
    red = BrFixedToScalar(BR_RED(colour) << 8) * l;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * l;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * l;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }
    attn = 1.f / (alp->s->attenuation_q * dist2 + alp->s->attenuation_l * dist + alp->s->attenuation_c);
    if (dot_spot < alp->s->spot_inner) {
        attn = (alp->s->spot_outer - dot_spot) * attn / (alp->s->spot_outer - alp->s->spot_inner);
    }

    comp[C_R] += red * attn * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * attn * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * attn * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
#else
    __asm {
            sub esp, 0x14
            mov ecx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x1c]
            push ebx
            push ebp
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
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            push esi
            push edi
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 0x38]
            fcomp dword ptr [rendererStateStoredDispatch+0xa8]
            fnstsw ax
            test ah, 0x41
            jne L0054a222
            fld dword ptr [esp + 0x38]
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
            mov esi, dword ptr [ecx + 0x34]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fxch st(4)
            fstp dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x2c]
            fstp st(0)
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [esi + 0x24]
            fnstsw ax
            test ah, 1
            jne L0054a224
            mov ebx, dword ptr [esp + 0x30]
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            _emit 0xd8
        _emit 0x0b
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [rendererStateStoredDispatch+0xbc]
            fst dword ptr [esp + 0x30]
            fnstsw ax
            test ah, 0x41
            jne L0054a222
            fld dword ptr [esp + 0x38]
            fcomp dword ptr [rendererStateStoredDispatch+0xd0]
            fnstsw ax
            test ah, 1
            jne L0054a011
            fld dword ptr [rendererStateStoredDispatch+0xd4]
            jmp L0054a019
    L0054a011:
            fld dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x38]
    L0054a019:
            fld dword ptr [esi + 0x2c]
            fld dword ptr [esi + 0x34]
            fmulp st(2), st(0)
            fmul dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            fcomp dword ptr [esi + 0x28]
            fadd dword ptr [esi + 0x30]
            fnstsw ax
            fdivr qword ptr [rendererStateStoredDispatch+0xb0]
            test ah, 1
            fstp dword ptr [esp + 0x38]
            je L0054a05f
            fld dword ptr [esi + 0x24]
            fsub dword ptr [esp + 0x2c]
            fld dword ptr [esi + 0x24]
            fsub dword ptr [esi + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fdivp st(1), st(0)
            fstp dword ptr [esp + 0x38]
    L0054a05f:
            mov eax, dword ptr [esp + 0x34]
            mov edi, dword ptr [esp + 0x28]
            mov edx, eax
            mov ebp, eax
            fld dword ptr [edi + 0x30]
            shr edx, 8
            and edx, 0xff00
            and eax, 0xff00
            mov dword ptr [esp + 0x10], edx
            xor edx, edx
            mov dword ptr [esp + 0x14], edx
            and ebp, 0xff
            fild qword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x10]
            shl ebp, 8
            mov dword ptr [esp + 0x10], ebp
            mov dword ptr [esp + 0x14], edx
            fild qword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(3)
            fmulp st(4), st(0)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fld dword ptr [edi + 0x34]
            fxch st(2)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fxch st(2)
            fcomp dword ptr [rendererStateStoredDispatch+0xbc]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fstp st(3)
            fnstsw ax
            test ah, 0x40
            fxch st(1)
            jne L0054a18d
            fld dword ptr [esp + 0x30]
            fadd st(0), st(0)
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            _emit 0xd8
        _emit 0x0b
            fxch st(1)
            fsub dword ptr [esp + 0x1c]
            fxch st(2)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fsub dword ptr [esp + 0x18]
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
            fst dword ptr [esp + 0x2c]
            fstp st(1)
            fcomp dword ptr [rendererStateStoredDispatch+0xc4]
            fnstsw ax
            test ah, 0x41
            jne L0054a18d
            fld dword ptr [edi + 0x38]
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [edi + 0x34]
            fxch st(1)
            fsubr dword ptr [edi + 0x38]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fadd dword ptr [esp + 0x2c]
            fld st(2)
            fxch st(1)
            fdivp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            fxch st(3)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [esp + 0x2c]
            fxch st(3)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fstp st(1)
    L0054a18d:
            mov esi, dword ptr [esi + 0x20]
            mov dword ptr [esp + 0x14], edx
            shr esi, 8
            and esi, 0xff00
            mov eax, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x1c], edx
            fild qword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            mov dword ptr [esp + 0x14], edx
            fmulp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x38]
            fxch st(2)
            fadd dword ptr [eax + 0x2c]
            fld st(1)
            fxch st(1)
            fstp dword ptr [eax + 0x2c]
            mov edx, dword ptr [ecx + 0x34]
            fmul dword ptr [esp + 0x38]
            mov edx, dword ptr [edx + 0x20]
            and edx, 0xff00
            mov dword ptr [esp + 0x18], edx
            fild qword ptr [esp + 0x18]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(3), st(0)
            fxch st(2)
            fadd dword ptr [eax + 0x30]
            fstp dword ptr [eax + 0x30]
            mov ecx, dword ptr [ecx + 0x34]
            mov edx, dword ptr [ecx + 0x20]
            and edx, 0xff
            shl edx, 8
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0x10]
            fmul dword ptr [rendererStateStoredDispatch+0xa4]
            fmulp st(2), st(0)
            fxch st(1)
            fadd dword ptr [eax + 0x34]
            fstp dword ptr [eax + 0x34]
    L0054a222:
            fstp st(0)
    L0054a224:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00548fe0
void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp) {

    switch (alp->type) {
    case BRT_DIRECT:
        alp->accumulate_colour = lightingColourDirectSpecular;
        break;
    case BRT_POINT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_colour = lightingColourPointSpecular;
        } else {
            alp->accumulate_colour = lightingColourPointAttnSpecular;
        }
        break;
    case BRT_SPOT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_colour = lightingColourSpotSpecular;
        } else {
            alp->accumulate_colour = lightingColourSpotAttnSpecular;
        }
        break;
    case BRT_POINT_LOCAL_1:
        alp->accumulate_colour = lightingColourLocal1;
        break;
    case BRT_POINT_LOCAL_2:
        alp->accumulate_colour = lightingColourLocal2;
        break;
    default:
        alp->accumulate_colour = lightingColourNull;
        break;
    }
}
