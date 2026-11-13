#include "setup.h"

#include "alpha.h"
#include "light24.h"
#include "light8.h"
#include "mapping.h"


// GLOBAL: CARMA2_HW 0x0079f980
rend_block_soft rend;


// GLOBAL: CARMA2_HW 0x0079fa00
static_cache_soft scache;

static void eyeInModel(br_soft_renderer* self) {
    br_matrix4 s2m;

    if (self->state.matrix.model_to_view_hint == BRT_LENGTH_PRESERVING) {
        if (self->state.matrix.view_to_screen_hint == BRT_PERSPECTIVE) {
            scache.eye_m.v[0] = -BR_MAC3(
                self->state.matrix.model_to_view.m[3][0], self->state.matrix.model_to_view.m[0][0],
                self->state.matrix.model_to_view.m[3][1], self->state.matrix.model_to_view.m[0][1],
                self->state.matrix.model_to_view.m[3][2], self->state.matrix.model_to_view.m[0][2]);
            scache.eye_m.v[1] = -BR_MAC3(
                self->state.matrix.model_to_view.m[3][0], self->state.matrix.model_to_view.m[1][0],
                self->state.matrix.model_to_view.m[3][1], self->state.matrix.model_to_view.m[1][1],
                self->state.matrix.model_to_view.m[3][2], self->state.matrix.model_to_view.m[1][2]);
            scache.eye_m.v[2] = -BR_MAC3(
                self->state.matrix.model_to_view.m[3][0], self->state.matrix.model_to_view.m[2][0],
                self->state.matrix.model_to_view.m[3][1], self->state.matrix.model_to_view.m[2][1],
                self->state.matrix.model_to_view.m[3][2], self->state.matrix.model_to_view.m[2][2]);
            scache.eye_m.v[3] = 1.f;
            return;
        }
        if (self->state.matrix.view_to_screen_hint == BRT_PARALLEL) {
            BrVector3Copy(&scache.eye_m, (br_vector3*)scache.view_to_model.m[2]);
            scache.eye_m.v[3] = 0.f;
            return;
        }
    } else {
        if (self->state.matrix.view_to_screen_hint == BRT_PERSPECTIVE) {
            BrVector3Copy(&scache.eye_m, (br_vector3*)scache.view_to_model.m[3]);
            scache.eye_m.v[3] = 1.f;
            return;
        }

        if (self->state.matrix.view_to_screen_hint == BRT_PARALLEL) {
            BrVector3Copy(&scache.eye_m, (br_vector3*)scache.view_to_model.m[2]);
            scache.eye_m.v[3] = 0.f;
            return;
        }
    }
    BrMatrix4Mul34(&scache.model_to_screen, &self->state.matrix.model_to_view, &self->state.matrix.view_to_screen);
    BrMatrix4Inverse(&s2m, &scache.model_to_screen);

    scache.eye_m.v[0] = s2m.m[2][0];
    scache.eye_m.v[1] = s2m.m[2][1];
    scache.eye_m.v[2] = s2m.m[2][2];
    scache.eye_m.v[3] = s2m.m[2][3];
}

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp);
extern void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp);
extern br_int_32 C2_HOOK_STDCALL GenerateSurfaceFunctions(br_soft_renderer *self, surface_fn **fns, br_uint_32 mask);
extern char rendererStateStoredDispatch[];
extern char rendererStateStoredTemplateEntries[];
#endif
// FUNCTION: CARMA2_HW 0x00543ec0
void C2_HOOK_STDCALL ViewToModelUpdate(br_soft_renderer* self) {

    BrMatrix34Inverse(&scache.view_to_model, &self->state.matrix.model_to_view);
}

// FUNCTION: CARMA2_HW 0x00543ee0
void C2_HOOK_STDCALL ModelToScreenUpdate(br_soft_renderer* self) {

    BrMatrix4Mul34(&scache.model_to_screen,
        &self->state.matrix.model_to_view,
        &self->state.matrix.view_to_screen);
}

// FUNCTION: CARMA2_HW 0x00543a40
void C2_HOOK_STDCALL StaticCacheUpdate_PerScene(br_soft_renderer* self) {
    int i;

    scache.user_clip_active = 0;

    C2_HOOK_BUG_ON(MAX_STATE_CLIP_PLANES != 6);
    for (i = 0; i < MAX_STATE_CLIP_PLANES; i++) {
        if (self->state.clip[i].type == BRT_PLANE)
            scache.user_clip_active = 1;
    }
    ActiveLightsFind(self);
}

// FUNCTION: CARMA2_HW 0x00543a80
void C2_HOOK_STDCALL StaticCacheUpdate_PerModel(br_soft_renderer* self) {

#if COUNTERS
    CounterPerModel++;
#endif

    if (!scache.valid_v2m) {
        ViewToModelUpdate(self);
        scache.valid_v2m = 1;
    }

    if (!scache.valid_m2s) {
        ModelToScreenUpdate(self);
        scache.valid_m2s = 1;
    }

    eyeInModel(self);
    BrVector3Normalise(&scache.eye_m_normalised, &scache.eye_m);

    if (self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
        BrMatrix34Mul(&scache.model_to_environment,
            &self->state.matrix.model_to_view,
            &self->state.matrix.view_to_environment);
    }

    ActiveLightsUpdate(self);
}

// FUNCTION: CARMA2_HW 0x00543d80
br_error C2_NAKED C2_HOOK_STDCALL CacheUpdate(br_soft_renderer* self) {
#ifndef CARPOCALYPSE2_MATCHING

    if (!rend.block_changed && !rend.range_changed && self->state.cache.valid) {
        return 0;
    }

    C2_HOOK_BUG_ON(NUM_COMPONENTS != 16);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, constant_components, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, vertex_components, 0x1c);

    self->state.cache.clip_slots = ComponentMaskToSlots(CM_W | CM_Z | COMP_S2V(rend.block->vertex_components));

    self->state.pstate->dispatch->_rangesQueryF(self->state.pstate,
        self->state.cache.comp_offsets, self->state.cache.comp_scales, NUM_COMPONENTS);

    self->state.cache.map_transform.m[0][0] = self->state.surface.map_transform.m[0][0] * self->state.cache.comp_scales[C_U];
    self->state.cache.map_transform.m[0][1] = self->state.surface.map_transform.m[0][1] * self->state.cache.comp_scales[C_V];
    self->state.cache.map_transform.m[1][0] = self->state.surface.map_transform.m[1][0] * self->state.cache.comp_scales[C_U];
    self->state.cache.map_transform.m[1][1] = self->state.surface.map_transform.m[1][1] * self->state.cache.comp_scales[C_V];

    self->state.cache.map_transform.m[2][0] = self->state.surface.map_transform.m[2][0] * self->state.cache.comp_scales[C_U] + self->state.cache.comp_offsets[C_U];
    self->state.cache.map_transform.m[2][1] = self->state.surface.map_transform.m[2][1] * self->state.cache.comp_scales[C_V] + self->state.cache.comp_offsets[C_V];

    self->state.cache.nvertex_fns = GenerateSurfaceFunctions(self, self->state.cache.vertex_fns, rend.block->vertex_components);

    self->state.cache.nconstant_fns = GenerateSurfaceFunctions(self, self->state.cache.constant_fns, rend.block->constant_components);

    return 0;
#else
    __asm {
            mov eax, dword ptr [rend+0x50]
            push esi
            mov esi, dword ptr [esp + 8]
            push edi
            test eax, eax
            jne L00543da6
            mov eax, dword ptr [rend+0x54]
            test eax, eax
            jne L00543da6
            mov eax, dword ptr [esi + 0x788]
            test eax, eax
            jne L00543eaa
    L00543da6:
            mov eax, dword ptr [rend+0x4c]
            mov edx, 0
            mov ecx, dword ptr [eax + 0x1c]
            mov eax, ecx
            and ecx, 0x30
            or cl, 0xc0
            and al, 0x8f
            shr ecx, 4
            or eax, ecx
            je L00543dd6
            mov ecx, offset rendererStateStoredTemplateEntries+0x90
    L00543dc9:
            test al, 1
            je L00543dcf
            or edx, dword ptr [ecx]
    L00543dcf:
            add ecx, 4
            shr eax, 1
            jne L00543dc9
    L00543dd6:
            mov eax, dword ptr [esi + 0x794]
            lea ecx, [esi + 0x5e4]
            push 0x10
            lea edi, [esi + 0x5a4]
            mov dword ptr [esi + 0x5a0], edx
            mov edx, dword ptr [eax]
            push ecx
            push edi
            push eax
            call dword ptr [edx + 0x78]
            fld dword ptr [esi + 0x5c]
            fld dword ptr [esi + 0x60]
            fld dword ptr [esi + 0x604]
            fld dword ptr [esi + 0x4c]
            fmul dword ptr [esi + 0x604]
            fld dword ptr [esi + 0x50]
            fxch st(1)
            fstp dword ptr [esi + 0x76c]
            fmul dword ptr [esi + 0x608]
            fld dword ptr [esi + 0x54]
            fxch st(1)
            fstp dword ptr [esi + 0x770]
            fmul dword ptr [esi + 0x604]
            fld dword ptr [esi + 0x58]
            fxch st(1)
            fstp dword ptr [esi + 0x774]
            fmul dword ptr [esi + 0x608]
            fxch st(1)
            fmulp st(3), st(0)
            add esp, 0x10
            lea eax, [esi + 0x728]
            fstp dword ptr [esi + 0x778]
            fxch st(1)
            fadd dword ptr [esi + 0x5c4]
            fxch st(1)
            fmul dword ptr [esi + 0x608]
            fxch st(1)
            fstp dword ptr [esi + 0x77c]
            fadd dword ptr [esi + 0x5c8]
            fstp dword ptr [esi + 0x780]
            mov ecx, dword ptr [rend+0x4c]
            mov edx, dword ptr [ecx + 0x1c]
            push edx
            push eax
            push esi
            call GenerateSurfaceFunctions
            mov dword ptr [esi + 0x724], eax
            mov eax, dword ptr [rend+0x4c]
            lea edx, [esi + 0x74c]
            mov ecx, dword ptr [eax + 0x18]
            push ecx
            push edx
            push esi
            call GenerateSurfaceFunctions
            mov dword ptr [esi + 0x748], eax
    L00543eaa:
            pop edi
            xor eax, eax
            pop esi
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00543f10
void C2_NAKED C2_HOOK_STDCALL ActiveLightsFind(br_soft_renderer* self) {
#ifndef CARPOCALYPSE2_MATCHING
    int l;
    soft_state_light* lp;
    active_light* alp;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(static_cache_soft, nlights_model, 0x498);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(static_cache_soft, nlights_view, 0x49c);
    C2_HOOK_BUG_ON(BR_ASIZE(scache.lights) != 16);
    C2_HOOK_BUG_ON(BR_ASIZE(self->state.light) != 16);

    scache.nlights_model = 0;
    scache.nlights_view = 0;

    alp = &scache.lights[0];

    for (l = 0; l < BR_ASIZE(scache.lights); l++) {
        lp = &self->state.light[l];

        if (lp->type == BRT_NONE || lp->lighting_space != BRT_MODEL) {
            continue;
        }

        alp->s = lp;
        alp->intensity = 1.f / lp->attenuation_c;
        alp->type = lp->type;

        ActiveLightAccumulateIndexSet(alp);
        ActiveLightAccumulateColourSet(alp);
        alp++;
        scache.nlights_model += 1;
    }

    for (l = 0; l < BR_ASIZE(self->state.light); l++) {
        lp = &self->state.light[l];

        if (lp->type == BRT_NONE || lp->lighting_space != BRT_VIEW) {
            continue;
        }

        alp->s = lp;
        alp->intensity = 1.f / lp->attenuation_c;
        alp->type = lp->type;
        BrVector3Copy(&alp->position, &lp->position);

        if (lp->type == BRT_DIRECT) {
            BrVector3Scale(&alp->direction, &lp->direction, alp->intensity);
        } else {
            BrVector3Copy(&alp->direction, &lp->direction);
        }

        ActiveLightAccumulateIndexSet(alp);
        ActiveLightAccumulateColourSet(alp);

        alp++;
        scache.nlights_view += 1;
    }
#else
    __asm {
            xor eax, eax
            push ebx
            mov dword ptr [scache+0x498], eax
            mov dword ptr [scache+0x49c], eax
            mov eax, dword ptr [esp + 8]
            push ebp
            push esi
            push edi
            lea edi, [eax + 0x6c]
            mov esi, offset scache+0x118
            mov ebx, edi
            mov ebp, 0x10
    L00543f33:
            cmp dword ptr [ebx], 1
            je L00543f6e
            cmp dword ptr [ebx + 4], 0xb1
            jne L00543f6e
            mov dword ptr [esi + 0x34], ebx
            push esi
            fld dword ptr [ebx + 0x30]
            fdivr qword ptr [rendererStateStoredDispatch+0x60]
            fstp dword ptr [esi + 0x28]
            mov ecx, dword ptr [ebx]
            mov dword ptr [esi], ecx
            call ActiveLightAccumulateIndexSet
            push esi
            call ActiveLightAccumulateColourSet
            mov eax, dword ptr [scache+0x498]
            add esi, 0x38
            inc eax
            mov dword ptr [scache+0x498], eax
    L00543f6e:
            add ebx, 0x3c
            dec ebp
            jne L00543f33
            mov ebx, 0x10
    L00543f79:
            cmp dword ptr [edi], 1
            je L00544011
            cmp dword ptr [edi + 4], 0xb0
            jne L00544011
            lea eax, [esi + 4]
            lea ecx, [edi + 8]
            mov dword ptr [eax + 0x30], edi
            fld dword ptr [edi + 0x30]
            fdivr qword ptr [rendererStateStoredDispatch+0x60]
            fstp dword ptr [esi + 0x28]
            mov edx, dword ptr [edi]
            mov dword ptr [esi], edx
            mov ebp, dword ptr [ecx]
            mov edx, eax
            mov dword ptr [edx], ebp
            mov ebp, dword ptr [ecx + 4]
            mov dword ptr [edx + 4], ebp
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [edx + 8], ecx
            mov ecx, dword ptr [edi]
            cmp ecx, 0xd0
            jne L00543fe1
            fld dword ptr [edi + 0x14]
            fmul dword ptr [esi + 0x28]
            fstp dword ptr [esi + 0x10]
            fld dword ptr [edi + 0x18]
            fmul dword ptr [esi + 0x28]
            fstp dword ptr [eax + 0x10]
            fld dword ptr [edi + 0x1c]
            fmul dword ptr [esi + 0x28]
            fstp dword ptr [esi + 0x18]
            jmp L00543ff7
    L00543fe1:
            lea edx, [edi + 0x14]
            lea eax, [esi + 0x10]
            mov ecx, dword ptr [edx]
            mov dword ptr [eax], ecx
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [eax + 4], ecx
            mov edx, dword ptr [edx + 8]
            mov dword ptr [eax + 8], edx
    L00543ff7:
            push esi
            call ActiveLightAccumulateIndexSet
            push esi
            call ActiveLightAccumulateColourSet
            mov eax, dword ptr [scache+0x49c]
            add esi, 0x38
            inc eax
            mov dword ptr [scache+0x49c], eax
    L00544011:
            add edi, 0x3c
            dec ebx
            jne L00543f79
            mov eax, dword ptr [scache+0x49c]
            test eax, eax
            jne L00544046
            mov ecx, dword ptr [scache+0x498]
            mov eax, 1
            cmp ecx, eax
            jne L00544046
            mov ecx, dword ptr [scache+0x118]
            mov dword ptr [scache+0x114], eax
            cmp ecx, 0xd0
            je L00544050
    L00544046:
            mov dword ptr [scache+0x114], 0
    L00544050:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00544060
void C2_HOOK_STDCALL ActiveLightsUpdate(br_soft_renderer* self) {
    active_light* alp;
    int l;

    alp = scache.lights;
    for (l = 0; l < scache.nlights_model; l++, alp++) {
        switch (alp->type) {
        case BRT_DIRECT:
            BrMatrix34TApplyV(&alp->direction, &alp->s->direction, &self->state.matrix.model_to_view);
            BrVector3Normalise(&alp->direction, &alp->direction);

            BrVector3Add(&alp->half, &alp->direction, &scache.eye_m_normalised);
            BrVector3Normalise(&alp->half, &alp->half);
            BrVector3Scale(&alp->direction, &alp->direction, alp->intensity);
            break;
        case BRT_SPOT:
            BrMatrix34TApplyV(&alp->direction, &alp->s->direction, &self->state.matrix.model_to_view);
            BrVector3Normalise(&alp->direction, &alp->direction);
            break;
        case BRT_POINT:
        case BRT_POINT_LOCAL_1:
        case BRT_POINT_LOCAL_2:
            BrMatrix34ApplyP(&alp->position, &alp->s->position, &scache.view_to_model);
            break;
        default:
            break;
        }
    }

    for (l = 0; l < scache.nlights_view; l++, alp++) {
        switch (alp->type) {
        case BRT_DIRECT:
            BrVector3Copy(&alp->half, &alp->s->direction);
            alp->half.v[2] += 1.f;
            BrVector3Normalise(&alp->half, &alp->half);
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005444a0
br_int_32 C2_HOOK_STDCALL GenerateSurfaceFunctions(br_soft_renderer *self, surface_fn **fns, br_uint_32 mask) {
    br_int_32 f = 0;
    br_matrix23* m;

    if (mask & (CM_U | CM_V)) {
        switch (self->state.surface.mapping_source) {
        case BRT_ENVIRONMENT_LOCAL:
            fns[f++] = (surface_fn*)SurfaceMapEnvironmentLocal;
            break;

        case BRT_ENVIRONMENT_INFINITE:
            fns[f++] = (surface_fn*)SurfaceMapEnvironmentInfinite;
            break;

        case BRT_GEOMETRY_MAP:
            fns[f++] = (surface_fn*)SurfaceMapGeometryMap;

            m = &self->state.cache.map_transform;

            if (m->m[0][1] == 0.f && m->m[1][0] == 0.f) {
                fns[f - 1] = (surface_fn*)SurfaceMapGeometryMapScaleTranslate;
                if (m->m[2][0] == 0.f && m->m[2][1] == 0.f) {
                    fns[f - 1] = (surface_fn*)SurfaceMapGeometryMapScale;
                }

                if (m->m[0][0] == 1.f && m->m[1][1] == 1.f
                        && m->m[2][0] == 0.f && m->m[2][1] == 0.f) {
                    fns[f - 1] = (surface_fn*)SurfaceMapGeometryMapCopy;
                }
            }
            break;
        case BRT_GEOMETRY_X:
            fns[f++] = (surface_fn*)SurfaceMapGeometryX;
            break;
        case BRT_GEOMETRY_Y:
            fns[f++] = (surface_fn*)SurfaceMapGeometryY;
            break;
        case BRT_GEOMETRY_Z:
        default:
            fns[f++] = (surface_fn*)SurfaceMapGeometryZ;
            break;
        }
    }

    if (mask & CM_I) {
        if (self->state.surface.lighting) {
            fns[f++] = (surface_fn*)SurfaceIndexLit;
        } else if (self->state.surface.colour_source == BRT_GEOMETRY) {
            fns[f++] = (surface_fn*)SurfaceIndexUnlit;
        } else {
            fns[f++] = (surface_fn*)SurfaceIndexZero;
        }
    } else if (mask & CM_UI) {
        if (self->state.surface.colour_source == BRT_GEOMETRY) {
            fns[f++] = (surface_fn*)SurfaceIndexUnlit;
        } else {
            fns[f++] = (surface_fn*)SurfaceIndexZero;
        }
    }

    if (mask & (CM_R | CM_G | CM_B)) {
        if (self->state.surface.lighting) {
            fns[f++] = (surface_fn*)SurfaceColourLit;
        } else {
            fns[f++] = (surface_fn*)SurfaceColourUnlit;
        }
    } else if (mask & (CM_UR | CM_UG | CM_UB)) {
        fns[f++] = (surface_fn*)SurfaceColourUnlit;
    }

    if (mask & CM_A) {
        fns[f++] = (surface_fn*)SurfaceAlpha;
    }

    fns[f] = NULL;

    return f;
}

// FUNCTION: CARMA2_HW 0x00544650
br_uint_32 C2_HOOK_STDCALL ComponentMaskToSlots(br_uint_32 cm) {
    br_uint_32 m;
    int i;
    // GLOBAL: CARMA2_HW 0x00670720
    static const br_uint_32 bits[14] = {
        1 << C_X,
        1 << C_Y,
        1 << C_Z,
        1 << C_W,
        1 << C_SX,
        1 << C_SY,
        1 << C_SZ,
        1 << C_U,
        1 << C_V,
        1 << C_I,
        1 << C_A,
        1 << C_R,
        1 << C_G,
        1 << C_B,
    };

    m = 0;

    for (i = 0; cm != 0; i++, cm /= 2) {
        if (cm & 1) {
            m |= bits[i];
        }
    }

    return m;
}

// FUNCTION: CARMA2_HW 0x00544670
void C2_HOOK_STDCALL GeometryFunctionReset(br_soft_renderer* renderer) {

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.cache.ngeometry_fns, 0x624);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.cache.ngeometry_fns_onscreen, 0x668);
#endif

    renderer->state.cache.ngeometry_fns = 0;
    renderer->state.cache.ngeometry_fns_onscreen = 0;
}

// FUNCTION: CARMA2_HW 0x00544690
void C2_HOOK_STDCALL GeometryFunctionAdd(br_soft_renderer* renderer, geometry_fn* fn) {

    renderer->state.cache.geometry_fns[renderer->state.cache.ngeometry_fns] = fn;
    renderer->state.cache.ngeometry_fns += 1;
}

// FUNCTION: CARMA2_HW 0x005446c0
void C2_HOOK_STDCALL GeometryFunctionOnScreenAdd(br_soft_renderer* renderer, geometry_fn* fn) {

    renderer->state.cache.geometry_fns_onscreen[renderer->state.cache.ngeometry_fns_onscreen] = fn;
    renderer->state.cache.ngeometry_fns_onscreen += 1;
}

// FUNCTION: CARMA2_HW 0x005446f0
void C2_HOOK_STDCALL GeometryFunctionBothAdd(br_soft_renderer* renderer, geometry_fn* fn) {

    GeometryFunctionAdd(renderer, fn);
    GeometryFunctionOnScreenAdd(renderer, fn);
}

// FUNCTION: CARMA2_HW 0x00544730
void C2_HOOK_STDCALL PrimBlockReset(br_soft_renderer* renderer) {

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.cache.nface_blocks, 0x71c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.cache.nface_blocks_onscreen, 0x71d);
#endif

    renderer->state.cache.nface_blocks = 0;
    renderer->state.cache.nface_blocks_onscreen = 0;
}

// FUNCTION: CARMA2_HW 0x00544750
void C2_HOOK_STDCALL PrimBlockAdd(br_soft_renderer* renderer, brp_render_fn* fn) {
    int f;

    f = renderer->state.cache.nface_blocks;
    renderer->state.cache.face_blocks[f].render = fn;
    renderer->state.cache.face_blocks[f + 1].chain = (brp_block*)&renderer->state.cache.face_blocks[f];
    renderer->state.cache.nface_blocks = f + 1;
}

// FUNCTION: CARMA2_HW 0x00544790
void C2_HOOK_STDCALL PrimBlockOnScreenAdd(br_soft_renderer* renderer, brp_render_fn* fn) {
    int f;

    f = renderer->state.cache.nface_blocks_onscreen;
    renderer->state.cache.face_blocks_onscreen[f].render = fn;
    renderer->state.cache.face_blocks_onscreen[f + 1].chain = (brp_block*)&renderer->state.cache.face_blocks_onscreen[f];
    renderer->state.cache.nface_blocks_onscreen = f + 1;
}

// FUNCTION: CARMA2_HW 0x005447d0
void C2_HOOK_STDCALL PrimBlockAddBoth(br_soft_renderer* renderer, brp_render_fn* fn) {

    PrimBlockAdd(renderer, fn);
    PrimBlockOnScreenAdd(renderer, fn);
}
