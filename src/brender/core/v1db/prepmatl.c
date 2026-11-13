#include "prepmatl.h"

#include "dbsetup.h"
#ifndef CARPOCALYPSE2_MATCHING
#include "../fw/brprintf.h"
#include "prepmap.h"
#endif

/*
 * Mask of renderer state parts that materials are mapped to
 */
#define PARTS_MASK ((unsigned)(BR_STATE_SURFACE | BR_STATE_PRIMITIVE | BR_STATE_CULL))

#ifdef CARPOCALYPSE2_MATCHING
// GLOBAL: CARMA2_HW 0x0058b7a0
const float gFloat_0058b7a0 = 0.003921568859368563f;
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x00520e70
void C2_NAKED C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags) {
#ifndef CARPOCALYPSE2_MATCHING
    br_token_value tva[32];
    br_token_value* tvp;
    br_token t;
    br_int_32 c;

    C2_HOOK_BUG_ON(sizeof(br_token_value) != 8);
    C2_HOOK_BUG_ON(PARTS_MASK != 0x80000041u);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_material, subdivide_tolerance, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateStoredNew, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _partSet, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _partSetMany, 0x84);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _statePush, 0xb8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _statePop, 0xbc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateSave, 0xc0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateRestore, 0xc4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateDefault, 0xcc);

    if (v1db.renderer == NULL) {
        return;
    }

    v1db.renderer->dispatch->_statePush(v1db.renderer, PARTS_MASK);
    v1db.renderer->dispatch->_stateDefault(v1db.renderer, PARTS_MASK);
    if (mat->stored != NULL) {
        v1db.renderer->dispatch->_stateRestore(v1db.renderer, mat->stored, PARTS_MASK);
    }

    if (flags & BR_MATU_MAP_TRANSFORM) {
        v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_SURFACE, 0, BRT_MAP_MATRIX_M23_F, (uintptr_t)&mat->map_transform);
    }

    if (flags & BR_MATU_RENDERING) {
        tvp = tva;

        tvp->t = BRT_COLOUR_RGB;
        tvp->v.rgb = mat->colour;
        tvp++;

        tvp->t = BRT_OPACITY_F;
        tvp->v.f = (float)mat->opacity * (1.f / 255.f);
        tvp++;

        tvp->t = BRT_MODULATE_B;
        tvp->v.b = !!(mat->flags & (BR_MATF_LIGHT | BR_MATF_PRELIT));
        tvp++;

        tvp->t = BRT_BLEND_B;
        tvp->v.b = mat->opacity != 255;
        tvp++;

        tvp->t = BRT_SMOOTH_B;
        tvp->v.b = !!(mat->flags & BR_MATF_SMOOTH);
        tvp++;

        tvp->t = BRT_BLEND_SUMMED;
        tvp->v.b = !!(mat->flags & BR_MATF_DIM_BLENDING);
        tvp++;

        if (mat->flags & BR_MATF_SUBDIVIDE) {
            tvp->t = BRT_PERSPECTIVE_T;
            tvp->v.t = BRT_SUBDIVIDE;
            tvp++;

            tvp->t = BRT_SUBDIVIDE_TOLERANCE_I32;
            tvp->v.i32 = mat->subdivide_tolerance;
            tvp++;
        } else {
            tvp->t = BRT_PERSPECTIVE_B;
            tvp->v.t = !!(mat->flags & BR_MATF_PERSPECTIVE);
            tvp++;
        }

        tvp->t = BRT_DECAL_B;
        tvp->v.b = !!(mat->flags & BR_MATF_DECAL);
        tvp++;

        tvp->t = BRT_DITHER_MAP_B;
        tvp->v.b = !!(mat->flags & BR_MATF_DITHER);
        tvp++;

        tvp->t = BRT_DITHER_COLOUR_B;
        tvp->v.b = !!(mat->flags & BR_MATF_DITHER);
        tvp++;

        tvp->t = BRT_INDEX_BASE_I32;
        tvp->v.i32 = mat->index_base;
        tvp++;

        tvp->t = BRT_INDEX_RANGE_I32;
        tvp->v.i32 = mat->index_range;
        tvp++;

        tvp->t = BRT_ANTIALIASING_T;
        tvp->v.t = (mat->mode & BR_MATM_ANTIALIASING) ? BRT_DEFAULT : BRT_NONE;
        tvp++;

        tvp->t = BRT_MAP_ANTIALIASING_T;
        tvp->v.t = (mat->flags & BR_MATF_MAP_ANTIALIASING) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        tvp->t = BRT_MAP_INTERPOLATION_T;
        tvp->v.t = (mat->flags & BR_MATF_MAP_INTERPOLATION) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        tvp->t = BRT_MIP_INTERPOLATION_T;
        tvp->v.t = (mat->flags & BR_MATF_MIP_INTERPOLATION) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        tvp->t = BRT_MAP_WIDTH_LIMIT_T;
        tvp->v.t = BRT_WRAP;
        if (mat->mode & BR_MATM_MAP_WIDTH_LIMIT_MIRROR) {
            tvp->v.t = BRT_MIRROR;
        }
        if (mat->mode & BR_MATM_MAP_WIDTH_LIMIT_CLAMP) {
            tvp->v.t = BRT_CLAMP;
        }
        tvp++;

        tvp->t = BRT_MAP_HEIGHT_LIMIT_T;
        tvp->v.t = BRT_WRAP;
        if (mat->mode & BR_MATM_MAP_HEIGHT_LIMIT_MIRROR) {
            tvp->v.t = BRT_MIRROR;
        }
        if (mat->mode & BR_MATM_MAP_HEIGHT_LIMIT_CLAMP) {
            tvp->v.t = BRT_CLAMP;
        }
        tvp++;

        tvp->t = BRT_FOG_T;
        tvp->v.t = (mat->flags & BR_MATF_FOG_LOCAL) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        if (mat->flags & BR_MATF_FOG_LOCAL) {
            tvp->t = BRT_FOG_MIN_F;
            tvp->v.f = mat->fog_min;
            tvp++;

            tvp->t = BRT_FOG_MAX_F;
            tvp->v.f = mat->fog_max;
            tvp++;

            tvp->t = BRT_FOG_RGB;
            tvp->v.rgb = mat->fog_colour;
            tvp++;
        }

        tvp->t = BRT_NEW_BLEND_B;
        tvp->v.b = !!(mat->flags & BR_MATF_QUAD_MAPPING);
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);

        t = BRT_ONE_SIDED;
        if (mat->flags & BR_MATF_ALWAYS_VISIBLE) {
            t = BRT_NONE;
        }
        if (mat->flags & BR_MATF_TWO_SIDED) {
            t = BRT_TWO_SIDED;
        }
        v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_CULL, 0, BRT_TYPE_T, t);
    }

    if (flags & BR_MATU_LIGHTING) {
        tvp = tva;

        tvp->t = BRT_COLOUR_RGB;
        tvp->v.rgb = mat->colour;
        tvp++;

        tvp->t = BRT_OPACITY_F;
        tvp->v.f = (float)mat->opacity * (1.0f / 255.0f);
        tvp++;

        tvp->t = BRT_AMBIENT_F;
        tvp->v.f = mat->ka;
        tvp++;

        tvp->t = BRT_DIFFUSE_F;
        tvp->v.f = mat->kd;
        tvp++;

        tvp->t = BRT_SPECULAR_F;
        tvp->v.f = mat->ks;
        tvp++;

        tvp->t = BRT_SPECULAR_POWER_F;
        tvp->v.f = mat->power;
        tvp++;

        tvp->t = BRT_LIGHTING_B;
        if (mat->flags & BR_MATF_PRELIT) {
            tvp->v.b = 0;
        } else {
            tvp->v.b = !!(mat->flags & BR_MATF_LIGHT);
        }
        tvp++;

        tvp->t = BRT_FORCE_FRONT_B;
        tvp->v.b = !!(mat->flags & BR_MATF_FORCE_FRONT);
        tvp++;

        tvp->t = BRT_COLOUR_SOURCE_T;
        tvp->v.t = (mat->flags & BR_MATF_PRELIT) ? BRT_GEOMETRY : BRT_SURFACE;
        tvp++;

        tvp->t = BRT_MAPPING_SOURCE_T;
        tvp->v.t = BRT_GEOMETRY_MAP;
        if (mat->flags & BR_MATF_ENVIRONMENT_L) {
            tvp->v.t = BRT_ENVIRONMENT_LOCAL;
        }
        if (mat->flags & BR_MATF_ENVIRONMENT_I) {
            tvp->v.t = BRT_ENVIRONMENT_INFINITE;
        }
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_SURFACE, 0, tva, &c);
    }

    if (flags & BR_MATU_COLOURMAP) {
#ifndef CARPOCALYPSE2_MATCHING
        /* Port: hardware drivers reference the colour map's stored buffer
         * object. Some materials (notably the frontend/poly fonts) are built
         * lazily and their pixelmaps were never updated through BrMapUpdate,
         * so the stored object is missing and the material would end up
         * untextured. Create/refresh it on demand. */
        if (mat->colour_map != NULL && mat->colour_map->stored == NULL) {
            BrMapUpdate(mat->colour_map, BR_MAPU_ALL);
        }
#endif
        tvp = tva;

        tvp->t = BRT_COLOUR_MAP_O;
        tvp->v.o = (mat->colour_map != NULL) ? (br_object*)mat->colour_map->stored : NULL;
        tvp++;

        tvp->t = BRT_INDEX_SHADE_O;
        tvp->v.o = (mat->index_shade != NULL) ? (br_object*)mat->index_shade->stored : NULL;
        tvp++;

        tvp->t = BRT_INDEX_BLEND_O;
        tvp->v.o = (mat->index_blend != NULL) ? (br_object*)mat->index_blend->stored : NULL;
        tvp++;

        tvp->t = BRT_INDEX_FOG_O;
        tvp->v.o = (mat->index_fog != NULL) ? (br_object*)mat->index_fog->stored : NULL;
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);
    }

    if (flags & BR_MATU_SCREENDOOR) {
        tvp = tva;

        tvp->t = BRT_SCREEN_DOOR_O;
        tvp->v.o = (mat->screendoor != NULL) ? (br_object*)mat->screendoor->stored : NULL;
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);
    }

    if ((flags & BR_MATU_EXTRA_SURF) && mat->extra_surf != NULL) {
        v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_SURFACE, 0, mat->extra_surf, &c);
    }

    if ((flags & BR_MATU_EXTRA_PRIM) && mat->extra_prim != NULL) {
        v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_PRIMITIVE, 0, mat->extra_prim, &c);
    }

    if (mat->stored != NULL) {
        v1db.renderer->dispatch->_stateSave(v1db.renderer, mat->stored, PARTS_MASK);
    } else {
        v1db.renderer->dispatch->_stateStoredNew(v1db.renderer, &mat->stored, PARTS_MASK, 0);
    }
    v1db.renderer->dispatch->_statePop(v1db.renderer, PARTS_MASK);
#else
    __asm {
            mov eax, dword ptr [v1db+0xc]
            sub esp, 0x10c
            push edi
            xor edi, edi
            cmp eax, edi
            je L00521452
            mov ecx, dword ptr [eax]
            push esi
            push ebp
            push ebx
            push 0x80000041
            push eax
            call dword ptr [ecx + 0xb8]
            mov eax, dword ptr [v1db+0xc]
            add esp, 8
            mov edx, dword ptr [eax]
            push 0x80000041
            push eax
            call dword ptr [edx + 0xcc]
            mov esi, dword ptr [esp + 0x128]
            add esp, 8
            mov eax, dword ptr [esi + 0x98]
            lea ebp, [esi + 0x98]
            cmp eax, edi
            mov dword ptr [esp + 0x14], ebp
            je L00520ee3
            mov ecx, dword ptr [v1db+0xc]
            push 0x80000041
            push eax
            push ecx
            mov edx, dword ptr [ecx]
            call dword ptr [edx + 0xc4]
            add esp, 0xc
    L00520ee3:
            mov bl, byte ptr [esp + 0x124]
            test bl, 1
            je L00520f0c
            mov eax, dword ptr [v1db+0xc]
            lea edx, [esi + 0x24]
            push edx
            push 0xc8
            mov ecx, dword ptr [eax]
            push edi
            push 0x75
            push eax
            call dword ptr [ecx + 0x80]
            add esp, 0x14
    L00520f0c:
            test bl, 2
            je L005211ce
            mov eax, dword ptr [esi + 8]
            mov dword ptr [esp + 0x1c], 0xe
            mov dword ptr [esp + 0x20], eax
            mov al, byte ptr [esi + 0xc]
            mov ecx, eax
            mov dword ptr [esp + 0x24], 0xbf
            and ecx, 0xff
            mov dword ptr [esp + 0x2c], 0x86
            mov dword ptr [esp + 0x18], ecx
            mov ecx, dword ptr [esi + 0x20]
            mov edx, ecx
            mov dword ptr [esp + 0x34], 0x85
            and dl, 3
            mov dword ptr [esp + 0x3c], 0x7e
            fild dword ptr [esp + 0x18]
            neg dl
            sbb edx, edx
            mov dword ptr [esp + 0x44], 0x1bb
            neg edx
            cmp al, 0xff
            mov dword ptr [esp + 0x30], edx
            sbb eax, eax
            mov edx, ecx
            neg eax
            mov dword ptr [esp + 0x38], eax
            mov eax, ecx
            fmul dword ptr [gFloat_0058b7a0]
            shr edx, 2
            shr eax, 0x15
            and edx, 1
            and eax, 1
            fstp dword ptr [esp + 0x28]
            test ecx, 0x100000
            mov dword ptr [esp + 0x40], edx
            mov dword ptr [esp + 0x48], eax
            je L00520fcd
            mov edx, dword ptr [esi + 0x90]
            mov dword ptr [esp + 0x4c], 0x121
            mov dword ptr [esp + 0x50], 0x17d
            mov dword ptr [esp + 0x54], 0x17e
            mov dword ptr [esp + 0x58], edx
            lea eax, [esp + 0x5c]
            jmp L00520fe5
    L00520fcd:
            mov eax, ecx
            mov dword ptr [esp + 0x4c], 0x7f
            shr eax, 5
            and eax, 1
            mov dword ptr [esp + 0x50], eax
            lea eax, [esp + 0x54]
    L00520fe5:
            mov edx, ecx
            mov dword ptr [eax], 0x80
            shr edx, 6
            and edx, 1
            mov ebx, 0x135
            mov dword ptr [eax + 4], edx
            mov edx, ecx
            shr edx, 0xe
            add eax, 8
            and edx, 1
            mov dword ptr [eax], 0x82
            mov dword ptr [eax + 4], edx
            add eax, 8
            mov dword ptr [eax + 4], edx
            mov dword ptr [eax], 0x81
            xor edx, edx
            add eax, 8
            mov dl, byte ptr [esi + 0x3c]
            mov dword ptr [eax + 4], edx
            mov dword ptr [eax], 0x8b
            xor edx, edx
            add eax, 8
            mov dl, byte ptr [esi + 0x3d]
            mov dword ptr [eax + 4], edx
            mov edx, dword ptr [esi + 0x68]
            mov edi, edx
            mov dword ptr [eax], 0x8c
            and edi, 0x100
            add eax, 8
            neg edi
            sbb edi, edi
            mov dword ptr [eax], 0x12a
            and edi, 0x89
            add eax, 8
            inc edi
            mov dword ptr [eax - 4], edi
            mov edi, ecx
            and edi, 0x10000
            mov dword ptr [eax], 0x8d
            neg edi
            sbb edi, edi
            add eax, 8
            and edi, 0x92
            inc edi
            mov dword ptr [eax - 4], edi
            mov edi, ecx
            and edi, 0x20000
            mov dword ptr [eax], 0x91
            neg edi
            sbb edi, edi
            add eax, 8
            and edi, 0x92
            inc edi
            mov dword ptr [eax - 4], edi
            mov edi, ecx
            and edi, 0x40000
            mov dword ptr [eax], 0x94
            neg edi
            sbb edi, edi
            add eax, 8
            and edi, 0x92
            inc edi
            mov dword ptr [eax - 4], edi
            mov edi, 0x133
            test dl, 1
            mov dword ptr [eax], 0x131
            mov dword ptr [eax + 4], edi
            je L005210d5
            mov dword ptr [eax + 4], ebx
    L005210d5:
            test dl, 2
            mov ebp, 0x134
            je L005210e2
            mov dword ptr [eax + 4], ebp
    L005210e2:
            add eax, 8
            test dl, 4
            mov dword ptr [eax], 0x136
            mov dword ptr [eax + 4], edi
            je L005210f6
            mov dword ptr [eax + 4], ebx
    L005210f6:
            test dl, 8
            je L005210fe
            mov dword ptr [eax + 4], ebp
    L005210fe:
            mov edx, ecx
            add eax, 8
            and edx, 0x80000
            mov edi, edx
            mov dword ptr [eax], 0x95
            neg edi
            sbb edi, edi
            add eax, 8
            and edi, 0x92
            inc edi
            mov dword ptr [eax - 4], edi
            test edx, edx
            je L00521153
            mov edx, dword ptr [esi + 0x5c]
            mov dword ptr [eax], 0x97
            mov dword ptr [eax + 4], edx
            mov edx, dword ptr [esi + 0x60]
            add eax, 8
            mov dword ptr [eax], 0x99
            mov dword ptr [eax + 4], edx
            mov edx, dword ptr [esi + 0x64]
            add eax, 8
            mov dword ptr [eax], 0x96
            mov dword ptr [eax + 4], edx
            add eax, 8
    L00521153:
            shr ecx, 0x16
            and ecx, 1
            mov dword ptr [eax], 0x1af
            mov dword ptr [eax + 4], ecx
            add eax, 8
            lea edx, [esp + 0x10]
            push edx
            mov dword ptr [eax], 0
            mov eax, dword ptr [v1db+0xc]
            lea edx, [esp + 0x20]
            push edx
            push 0
            mov ecx, dword ptr [eax]
            push 0x7c
            push eax
            call dword ptr [ecx + 0x84]
            mov eax, dword ptr [esi + 0x20]
            add esp, 0x14
            test ah, 8
            mov ecx, 0xad
            je L0052119c
            mov ecx, 1
    L0052119c:
            test ah, 0x10
            je L005211a6
            mov ecx, 0xae
    L005211a6:
            mov eax, dword ptr [v1db+0xc]
            push ecx
            push 0xac
            push 0
            mov edx, dword ptr [eax]
            push 0x74
            push eax
            call dword ptr [edx + 0x80]
            mov ebp, dword ptr [esp + 0x28]
            mov bl, byte ptr [esp + 0x138]
            add esp, 0x14
            xor edi, edi
    L005211ce:
            test bl, 4
            je L005212db
            xor eax, eax
            mov ecx, dword ptr [esi + 0x20]
            mov al, byte ptr [esi + 0xc]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [esp + 0x14], eax
            mov eax, ecx
            fild dword ptr [esp + 0x14]
            fld dword ptr [esi + 0x10]
            fld dword ptr [esi + 0x14]
            fld dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x1c]
            fxch st(4)
            fmul dword ptr [gFloat_0058b7a0]
            and eax, 2
            mov dword ptr [esp + 0x20], edx
            fstp dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x1c], 0xe
            mov dword ptr [esp + 0x24], 0xbf
            fstp dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x40]
            fstp dword ptr [esp + 0x48]
            mov dword ptr [esp + 0x2c], 0xc1
            mov dword ptr [esp + 0x34], 0xc3
            mov dword ptr [esp + 0x3c], 0xc5
            mov dword ptr [esp + 0x44], 0xc7
            mov dword ptr [esp + 0x4c], 0xb2
            je L0052125d
            mov dword ptr [esp + 0x50], edi
            jmp L00521266
    L0052125d:
            mov edx, ecx
            and edx, 1
            mov dword ptr [esp + 0x50], edx
    L00521266:
            mov edx, ecx
            mov dword ptr [esp + 0x54], 0xb3
            shr edx, 0xd
            and edx, 1
            mov dword ptr [esp + 0x5c], 0xb4
            neg eax
            sbb eax, eax
            mov dword ptr [esp + 0x58], edx
            and al, 0xb9
            mov dword ptr [esp + 0x64], 0xb6
            add eax, 0x75
            mov dword ptr [esp + 0x68], 0xb9
            test cl, 0x10
            mov dword ptr [esp + 0x60], eax
            je L005212ac
            mov dword ptr [esp + 0x68], 0xb7
    L005212ac:
            test cl, 8
            je L005212b9
            mov dword ptr [esp + 0x68], 0xb8
    L005212b9:
            mov eax, dword ptr [v1db+0xc]
            lea edx, [esp + 0x10]
            push edx
            lea edx, [esp + 0x20]
            push edx
            mov dword ptr [esp + 0x74], edi
            mov ecx, dword ptr [eax]
            push edi
            push 0x75
            push eax
            call dword ptr [ecx + 0x84]
            add esp, 0x14
    L005212db:
            test bl, 8
            je L0052136e
            mov eax, dword ptr [esi + 0x40]
            mov dword ptr [esp + 0x1c], 0xa5
            cmp eax, edi
            je L005212f8
            mov eax, dword ptr [eax + 0x40]
            jmp L005212fa
    L005212f8:
            xor eax, eax
    L005212fa:
            mov dword ptr [esp + 0x20], eax
            mov eax, dword ptr [esi + 0x48]
            cmp eax, edi
            mov dword ptr [esp + 0x24], 0xa6
            je L00521312
            mov eax, dword ptr [eax + 0x40]
            jmp L00521314
    L00521312:
            xor eax, eax
    L00521314:
            mov dword ptr [esp + 0x28], eax
            mov eax, dword ptr [esi + 0x4c]
            cmp eax, edi
            mov dword ptr [esp + 0x2c], 0xa7
            je L0052132c
            mov eax, dword ptr [eax + 0x40]
            jmp L0052132e
    L0052132c:
            xor eax, eax
    L0052132e:
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esi + 0x50]
            cmp eax, edi
            mov dword ptr [esp + 0x34], 0x18a
            je L00521346
            mov eax, dword ptr [eax + 0x40]
            jmp L00521348
    L00521346:
            xor eax, eax
    L00521348:
            lea edx, [esp + 0x10]
            mov dword ptr [esp + 0x38], eax
            mov eax, dword ptr [v1db+0xc]
            push edx
            lea edx, [esp + 0x20]
            mov dword ptr [esp + 0x40], edi
            mov ecx, dword ptr [eax]
            push edx
            push edi
            push 0x7c
            push eax
            call dword ptr [ecx + 0x84]
            add esp, 0x14
    L0052136e:
            test bl, 0x10
            je L005213af
            mov eax, dword ptr [esi + 0x44]
            mov dword ptr [esp + 0x1c], 0xa8
            cmp eax, edi
            je L00521387
            mov eax, dword ptr [eax + 0x40]
            jmp L00521389
    L00521387:
            xor eax, eax
    L00521389:
            lea edx, [esp + 0x10]
            mov dword ptr [esp + 0x20], eax
            mov eax, dword ptr [v1db+0xc]
            push edx
            lea edx, [esp + 0x20]
            mov dword ptr [esp + 0x28], edi
            mov ecx, dword ptr [eax]
            push edx
            push edi
            push 0x7c
            push eax
            call dword ptr [ecx + 0x84]
            add esp, 0x14
    L005213af:
            test bl, 0x20
            je L005213dd
            mov eax, dword ptr [esi + 0x54]
            cmp eax, edi
            je L005213dd
            mov ecx, dword ptr [v1db+0xc]
            lea ebx, [esp + 0x10]
            push ebx
            push eax
            mov edx, dword ptr [ecx]
            push edi
            push 0x75
            push ecx
            call dword ptr [edx + 0x84]
            mov bl, byte ptr [esp + 0x138]
            add esp, 0x14
    L005213dd:
            test bl, 0x40
            je L00521403
            mov esi, dword ptr [esi + 0x58]
            cmp esi, edi
            je L00521403
            mov eax, dword ptr [v1db+0xc]
            lea edx, [esp + 0x10]
            push edx
            push esi
            mov ecx, dword ptr [eax]
            push edi
            push 0x7c
            push eax
            call dword ptr [ecx + 0x84]
            add esp, 0x14
    L00521403:
            mov eax, dword ptr [ebp]
            cmp eax, edi
            je L00521424
            mov ecx, dword ptr [v1db+0xc]
            push 0x80000041
            push eax
            push ecx
            mov edx, dword ptr [ecx]
            call dword ptr [edx + 0xc0]
            add esp, 0xc
            jmp L00521439
    L00521424:
            mov eax, dword ptr [v1db+0xc]
            push edi
            push 0x80000041
            push ebp
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x70]
            add esp, 0x10
    L00521439:
            mov eax, dword ptr [v1db+0xc]
            push 0x80000041
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 0xbc]
            add esp, 8
            pop ebx
            pop ebp
            pop esi
    L00521452:
            pop edi
            add esp, 0x10c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00521460
void C2_HOOK_STDCALL BrMaterialClear(br_material* mat) {
    br_object* stored = (br_object*)mat->stored;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_state_stored_dispatch, _free, 0x10);

    if (stored != NULL) {
        stored->dispatch->_free(stored);
        mat->stored = NULL;
    }
}
