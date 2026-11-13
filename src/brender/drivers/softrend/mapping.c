#include "mapping.h"

#include "setup.h"

#define APPLY_UV(du, dv, su, sv) do {                       \
        du = self->state.cache.map_transform.m[0][0] * su   \
            + self->state.cache.map_transform.m[1][0] * sv  \
            + self->state.cache.map_transform.m[2][0];      \
        dv = self->state.cache.map_transform.m[0][1] * su   \
            + self->state.cache.map_transform.m[1][1] * sv  \
            + self->state.cache.map_transform.m[2][1];      \
    } while (0)

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void __cdecl _ftol();
extern char rendererStateStoredDispatch[];
extern static_cache_soft scache;
#endif
// FUNCTION: CARMA2_HW 0x0054b150
void C2_NAKED C2_HOOK_CDECL SurfaceMapEnvironmentInfinite(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 r, wr;
    br_scalar d, cu, cv;

    d = 2.f * BrVector3Dot(&scache.eye_m_normalised, normal);
    BrVector3Scale(&r, normal, d);
    BrVector3Sub(&r, &r, &scache.eye_m_normalised);

    if (self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
        BrMatrix34ApplyV(&wr, &r, &scache.model_to_environment);
        BrVector3Normalise(&wr, &wr);
    } else {
        wr = r;
    }

    cu = BrFixedToFloat(BR_ATAN2(wr.v[0], -wr.v[2]));

    cv = -wr.v[1] / 2 + 0.5f;

    APPLY_UV(comp[C_U], comp[C_V], cu, cv);
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [esp + 0x28]
            push esi
            mov esi, dword ptr [esp + 0x20]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fmul dword ptr [scache+0x10c]
            fxch st(1)
            fmul dword ptr [scache+0x108]
            fld dword ptr [eax + 4]
            fxch st(3)
            fmul dword ptr [scache+0x104]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            faddp st(3), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fsub dword ptr [scache+0x104]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fsub dword ptr [scache+0x108]
            fxch st(3)
            fst dword ptr [esp + 0x18]
            fsub dword ptr [scache+0x10c]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            mov eax, dword ptr [esi + 0x554]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            cmp eax, 0xf6
            fstp dword ptr [esp + 0x18]
            fstp st(0)
            je L0054b269
            lea eax, [esp + 0x10]
            push offset scache+0x70
            lea ecx, [esp + 8]
            push eax
            push ecx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            add esp, 0xc
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [rendererStateStoredDispatch+0x10c]
            fnstsw ax
            test ah, 0x41
            jne L0054b251
            fdivr qword ptr [rendererStateStoredDispatch+0x110]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 4]
            jmp L0054b289
    L0054b251:
            fstp st(0)
            fld dword ptr [rendererStateStoredDispatch+0x11c]
            fld dword ptr [rendererStateStoredDispatch+0x11c]
            mov dword ptr [esp + 4], 0x3f800000
            jmp L0054b289
    L0054b269:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 8], eax
            fld dword ptr [esp + 8]
            mov dword ptr [esp + 0xc], ecx
            mov dword ptr [esp + 4], edx
            fld dword ptr [esp + 0xc]
    L0054b289:
            fld dword ptr [esp + 4]
            fxch st(1)
            fchs
            fpatan
            fmul qword ptr [rendererStateStoredDispatch+0x120]
            call _ftol
            and eax, 0xffff
            mov dword ptr [esp + 0x2c], eax
            mov eax, dword ptr [esp + 0x34]
            fild dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0x130]
            fld dword ptr [esi + 0x774]
            fxch st(2)
            fmul qword ptr [rendererStateStoredDispatch+0x128]
            fxch st(1)
            fsubr dword ptr [rendererStateStoredDispatch+0x130]
            fld dword ptr [esi + 0x76c]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(3)
            faddp st(1), st(0)
            fadd dword ptr [esi + 0x77c]
            fstp dword ptr [eax + 0x20]
            fld dword ptr [esi + 0x770]
            fld dword ptr [esi + 0x778]
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(3)
            faddp st(1), st(0)
            fadd dword ptr [esi + 0x780]
            fstp dword ptr [eax + 0x24]
            pop esi
            fstp st(0)
            fstp st(0)
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b320
void C2_NAKED C2_HOOK_CDECL SurfaceMapEnvironmentLocal(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 eye;
    br_vector3 r, wr;
    br_scalar d, cu, cv;

    BrVector3Sub(&eye, &scache.eye_m, p);
    BrVector3Normalise(&eye, &eye);

    d = 2.f * BrVector3Dot(&eye, normal);
    BrVector3Scale(&r, normal, d);
    BrVector3Sub(&r, &r, &eye);

    if (self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
        BrMatrix34ApplyV(&wr, &r, &scache.model_to_environment);
        BrVector3Normalise(&wr, &wr);
    } else {
        wr = r;
    }

    cu = BrFixedToFloat(BR_ATAN2(wr.v[0], -wr.v[2]));

    cv = -wr.v[1] / 2 + 0.5f;

    APPLY_UV(comp[C_U], comp[C_V], cu, cv);
#else
    __asm {
            fld dword ptr [scache+0xf8]
            mov eax, dword ptr [esp + 8]
            sub esp, 0x18
            fld dword ptr [scache+0xf4]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [scache+0xfc]
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
            fcom dword ptr [rendererStateStoredDispatch+0x10c]
            fnstsw ax
            test ah, 0x41
            jne L0054b397
            fdivr qword ptr [rendererStateStoredDispatch+0x110]
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
            jmp L0054b3b7
    L0054b397:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 4], 0x3f800000
            mov dword ptr [esp + 8], 0
            mov dword ptr [esp + 0xc], 0
    L0054b3b7:
            mov eax, dword ptr [esp + 0x2c]
            mov esi, dword ptr [esp + 0x20]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [eax + 4]
            fxch st(3)
            fmul dword ptr [esp + 4]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            faddp st(3), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fsub dword ptr [esp + 4]
            fxch st(2)
            fsub dword ptr [esp + 8]
            fxch st(3)
            fsub dword ptr [esp + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            mov eax, dword ptr [esi + 0x554]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            cmp eax, 0xf6
            fstp dword ptr [esp + 0x18]
            fstp st(0)
            je L0054b4b4
            lea eax, [esp + 0x10]
            push offset scache+0x70
            lea ecx, [esp + 8]
            push eax
            push ecx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            add esp, 0xc
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [rendererStateStoredDispatch+0x10c]
            fnstsw ax
            test ah, 0x41
            jne L0054b49c
            fdivr qword ptr [rendererStateStoredDispatch+0x110]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 4]
            jmp L0054b4d4
    L0054b49c:
            fstp st(0)
            fld dword ptr [rendererStateStoredDispatch+0x11c]
            fld dword ptr [rendererStateStoredDispatch+0x11c]
            mov dword ptr [esp + 4], 0x3f800000
            jmp L0054b4d4
    L0054b4b4:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 8], eax
            fld dword ptr [esp + 8]
            mov dword ptr [esp + 0xc], ecx
            mov dword ptr [esp + 4], edx
            fld dword ptr [esp + 0xc]
    L0054b4d4:
            fld dword ptr [esp + 4]
            fxch st(1)
            fchs
            fpatan
            fmul qword ptr [rendererStateStoredDispatch+0x120]
            call _ftol
            and eax, 0xffff
            mov dword ptr [esp + 0x24], eax
            mov eax, dword ptr [esp + 0x34]
            fild dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [rendererStateStoredDispatch+0x130]
            fld dword ptr [esi + 0x774]
            fxch st(2)
            fmul qword ptr [rendererStateStoredDispatch+0x128]
            fxch st(1)
            fsubr dword ptr [rendererStateStoredDispatch+0x130]
            fld dword ptr [esi + 0x76c]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(3)
            faddp st(1), st(0)
            fadd dword ptr [esi + 0x77c]
            fstp dword ptr [eax + 0x20]
            fld dword ptr [esi + 0x770]
            fld dword ptr [esi + 0x778]
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(3)
            faddp st(1), st(0)
            fadd dword ptr [esi + 0x780]
            fstp dword ptr [eax + 0x24]
            pop esi
            fstp st(0)
            fstp st(0)
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b560
void C2_NAKED C2_HOOK_CDECL SurfaceMapGeometryX(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING

    APPLY_UV(comp[C_U], comp[C_V], p->v[1], p->v[2]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 0x18]
            fld dword ptr [eax + 0x774]
            fld dword ptr [eax + 0x76c]
            fld dword ptr [ecx + 8]
            fmulp st(2), st(0)
            fmul dword ptr [ecx + 4]
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x77c]
            fstp dword ptr [edx + 0x20]
            fld dword ptr [eax + 0x770]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x778]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [ecx + 8]
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x780]
            fstp dword ptr [edx + 0x24]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b5b0
void C2_NAKED C2_HOOK_CDECL SurfaceMapGeometryY(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING

    APPLY_UV(comp[C_U], comp[C_V], p->v[2], p->v[0]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 0x18]
            fld dword ptr [eax + 0x76c]
            fld dword ptr [eax + 0x774]
            fld dword ptr [ecx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x77c]
            fstp dword ptr [edx + 0x20]
            fld dword ptr [eax + 0x770]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x778]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x780]
            fstp dword ptr [edx + 0x24]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b600
void C2_NAKED C2_HOOK_CDECL SurfaceMapGeometryZ(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING

    APPLY_UV(comp[C_U], comp[C_V], p->v[0], p->v[1]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 0x18]
            fld dword ptr [eax + 0x774]
            fld dword ptr [eax + 0x76c]
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x77c]
            fstp dword ptr [edx + 0x20]
            fld dword ptr [eax + 0x778]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x770]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x780]
            fstp dword ptr [edx + 0x24]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b650
void C2_NAKED C2_HOOK_CDECL SurfaceMapGeometryMap(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING

    APPLY_UV(comp[C_U], comp[C_V], map->v[0], map->v[1]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 0x18]
            fld dword ptr [eax + 0x774]
            fld dword ptr [eax + 0x76c]
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x77c]
            fstp dword ptr [edx + 0x20]
            fld dword ptr [eax + 0x778]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x770]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x780]
            fstp dword ptr [edx + 0x24]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b6a0
void C2_HOOK_CDECL SurfaceMapGeometryMapScale(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_U] = self->state.cache.map_transform.m[0][0] * map->v[0];
    comp[C_V] = self->state.cache.map_transform.m[1][1] * map->v[1];
}

// FUNCTION: CARMA2_HW 0x0054b6d0
void C2_NAKED C2_HOOK_CDECL SurfaceMapGeometryMapScaleTranslate(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
#ifndef CARPOCALYPSE2_MATCHING

    comp[C_U] = self->state.cache.map_transform.m[0][0] * map->v[0] + self->state.cache.map_transform.m[2][0];
    comp[C_V] = self->state.cache.map_transform.m[1][1] * map->v[1] + self->state.cache.map_transform.m[2][1];
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 0x18]
            fld dword ptr [eax + 0x76c]
            _emit 0xd8
        _emit 0x09
            fadd dword ptr [eax + 0x77c]
            fstp dword ptr [edx + 0x20]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x778]
            fmulp st(1), st(0)
            fadd dword ptr [eax + 0x780]
            fstp dword ptr [edx + 0x24]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0054b710
void C2_HOOK_CDECL SurfaceMapGeometryMapCopy(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_U] = map->v[0];
    comp[C_V] = map->v[1];
}