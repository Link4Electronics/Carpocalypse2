#include "render.h"

#include "actsupt.h"
#include "dbsetup.h"
#include "enables.h"
#include "modrend.h"
#include "otable.h"
#include "prepmesh.h"

#include "../core/math/matrix34.h"
#include "../core/math/transfrm.h"

#include "core/fw/diag.h"

#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern br_order_table* C2_HOOK_CDECL BrZsOrderTableClear();
extern char IID_IDirectInputDevice2A[];
extern void C2_HOOK_STDCALL InsertOrderTableList(br_order_table* order_table);
extern int C2_HOOK_STDCALL IsMaterialTransparent(const br_material* material);
extern void C2_HOOK_STDCALL RenderOrderTableList(void);
extern void C2_HOOK_STDCALL RenderPrimaryOrderTable(void);
extern render_style_cbfn RenderStyleCalls[8];
extern void C2_HOOK_STDCALL SetOrderTableBounds(br_bounds* bounds, br_order_table* order_table);
extern void __cdecl _ftol();
extern br_filesystem gZlib_filesystem;
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x00521890
void C2_NAKED C2_HOOK_CDECL BrDbModelRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen, int use_custom) {
#ifndef CARPOCALYPSE2_MATCHING
    br_token_value tv[5] = {
        { BRT_V1INSERT_FUNCTION_P },
        { BRT_V1INSERT_ARG1_P },
        { BRT_V1INSERT_ARG2_P },
        { BRT_V1INSERT_ARG3_P },
        { BR_NULL_TOKEN },
    };

    v1db.model_to_screen_valid = 0;
    if (use_custom && (model->flags & BR_MODF_CUSTOM)) {
        model->custom(actor, model, material, render_data, style, on_screen);
        return;
    }
    if (model->prepared == NULL && model->stored == NULL) {
        BrFailure("Tried to render un-prepared model %s", model->identifier != NULL ? model->identifier : "<NULL>");
        return;
    }
    if ((v1db.rendering == 2
        || (v1db.rendering == 3
            && ((model->flags & BR_MODF_USED_PREPARED_USER)
                || ((model->flags & _BR_MODF_RESERVED) && IsMaterialTransparent(material)))))
        && render_data != NULL) {
        br_order_table* order_table = render_data;
        SetOrderTableBounds(&model->bounds, order_table);
        if (order_table->visits == 0) {
            BrZsOrderTableClear(order_table);
            InsertOrderTableList(order_table);
        }
        order_table->visits += 1;
        v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (uintptr_t)order_table);
        if (v1db.primitive_call != NULL) {
            tv[0].v.p = v1db.primitive_call;
            tv[1].v.p = actor;
            tv[2].v.p = model;
            tv[3].v.p = material;
            v1db.renderer->dispatch->_partSetMany(v1db.renderer, BRT_HIDDEN_SURFACE, 0, tv, &use_custom);
        }
    }
    if (v1db.bounds_call == NULL) {
        RenderStyleCalls[style](actor, model, material, render_data, style, on_screen);
    } else {
        char bounds_buffer[2 * sizeof(br_vector2)];
        br_int_32 r;
        br_int_32 int_bounds[4];
        br_token_value tv2[3] = {
            { BRT_MIN_V2_F },
            { BRT_MAX_V2_F },
            { BR_NULL_TOKEN },
        };
        v1db.renderer->dispatch->_stateDefault(v1db.renderer, 0x20);
        RenderStyleCalls[style](actor, model, material, render_data, style, on_screen);
        v1db.renderer->dispatch->_partQueryMany(v1db.renderer, BRT_BOUNDS, 0, tv2, bounds_buffer, sizeof(bounds_buffer), &r);
        int_bounds[0] = (br_int_32)tv[0].v.v2_f->v[0];
        int_bounds[1] = (br_int_32)tv[0].v.v2_f->v[1];
        int_bounds[2] = (br_int_32)tv[1].v.v2_f->v[0];
        int_bounds[3] = (br_int_32)tv[1].v.v2_f->v[1];

        if (int_bounds[0] < 0) {
            int_bounds[0] = 0;
        }
        if (int_bounds[1] < 0) {
            int_bounds[1] = 0;
        }
        if (int_bounds[2] < 0) {
            int_bounds[2] = 0;
        }
        if (int_bounds[3] < 0) {
            int_bounds[3] = 0;
        }

        if (int_bounds[0] >= v1db.colour_buffer->width) {
            int_bounds[0] = v1db.colour_buffer->width - 1;
        }
        if (int_bounds[1] >= v1db.colour_buffer->height) {
            int_bounds[1] = v1db.colour_buffer->height - 1;
        }
        if (int_bounds[2] >= v1db.colour_buffer->width) {
            int_bounds[2] = v1db.colour_buffer->width - 1;
        }
        if (int_bounds[3] >= v1db.colour_buffer->height) {
            int_bounds[3] = v1db.colour_buffer->height - 1;
        }
        int_bounds[0] -= v1db.colour_buffer->origin_x;
        int_bounds[1] -= v1db.colour_buffer->origin_y;
        int_bounds[2] -= v1db.colour_buffer->origin_x;
        int_bounds[3] -= v1db.colour_buffer->origin_y;
        if (int_bounds[0] <= int_bounds[2] && int_bounds[1] <= int_bounds[3]) {
            v1db.bounds_call(actor, model, material, render_data, style, &v1db.model_to_screen, int_bounds);
        }
    }
#else
    __asm {
            sub esp, 0x60
            mov eax, dword ptr [esp + 0x7c]
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x74]
            push edi
            xor edi, edi
            mov dword ptr [esp + 0x48], 0x104
            cmp eax, edi
            mov dword ptr [esp + 0x4c], edi
            mov dword ptr [esp + 0x50], 0x105
            mov dword ptr [esp + 0x54], edi
            mov dword ptr [esp + 0x58], 0x106
            mov dword ptr [esp + 0x5c], edi
            mov dword ptr [esp + 0x60], 0x107
            mov dword ptr [esp + 0x64], edi
            mov dword ptr [esp + 0x68], edi
            mov dword ptr [esp + 0x6c], edi
            mov dword ptr [v1db+0x90], edi
            je L0052191d
            test byte ptr [esi + 0x20], 0x20
            je L0052191d
            mov eax, dword ptr [esp + 0x88]
            mov ecx, dword ptr [esp + 0x84]
            mov edx, dword ptr [esp + 0x80]
            push eax
            mov eax, dword ptr [esp + 0x80]
            push ecx
            mov ecx, dword ptr [esp + 0x7c]
            push edx
            push eax
            push esi
            push ecx
            call dword ptr [esi + 0x24]
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x60
            ret
    L0052191d:
            cmp dword ptr [esi + 0x4c], edi
            jne L00521941
            cmp dword ptr [esi + 0x50], edi
            jne L00521941
            mov eax, dword ptr [esi + 4]
            cmp eax, edi
            jne L00521933
            mov eax, offset gZlib_filesystem+0x1d0
    L00521933:
            push eax
            push offset gZlib_filesystem+0x1d8
            call BrFailure
            add esp, 8
    L00521941:
            mov eax, dword ptr [v1db+0x8]
            mov ebp, dword ptr [esp + 0x80]
            cmp eax, 2
            je L00521981
            cmp eax, 3
            jne L00521a0a
            mov ax, word ptr [esi + 0x20]
            test ah, 0x10
            jne L00521981
            test ah, 0x80
            je L00521a0a
            mov ebx, dword ptr [esp + 0x7c]
            push ebx
            call IsMaterialTransparent
            test eax, eax
            je L00521a0e
            jmp L00521985
    L00521981:
            mov ebx, dword ptr [esp + 0x7c]
    L00521985:
            cmp ebp, edi
            je L00521a0e
            lea edx, [esi + 0x34]
            push ebp
            push edx
            call SetOrderTableBounds
            cmp word ptr [ebp + 0x22], di
            jne L005219ac
            push ebp
            call BrZsOrderTableClear
            add esp, 4
            push ebp
            call InsertOrderTableList
    L005219ac:
            inc word ptr [ebp + 0x22]
            mov eax, dword ptr [v1db+0xc]
            push ebp
            push 0x102
            mov ecx, dword ptr [eax]
            push edi
            push 0x7b
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0x4dc]
            add esp, 0x14
            cmp eax, edi
            je L00521a0e
            mov edx, dword ptr [esp + 0x74]
            mov dword ptr [esp + 0x4c], eax
            mov eax, dword ptr [v1db+0xc]
            mov dword ptr [esp + 0x54], edx
            lea edx, [esp + 0x8c]
            mov dword ptr [esp + 0x5c], esi
            push edx
            lea edx, [esp + 0x4c]
            push edx
            mov dword ptr [esp + 0x6c], ebx
            mov ecx, dword ptr [eax]
            push edi
            push 0x7b
            push eax
            call dword ptr [ecx + 0x84]
            add esp, 0x14
            jmp L00521a0e
    L00521a0a:
            mov ebx, dword ptr [esp + 0x7c]
    L00521a0e:
            cmp dword ptr [v1db+0x4e0], edi
            je L00521bba
            mov eax, dword ptr [v1db+0xc]
            mov dword ptr [esp + 0x30], 0xe6
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x38], 0xe9
            mov dword ptr [esp + 0x3c], edi
            mov dword ptr [esp + 0x40], edi
            mov dword ptr [esp + 0x44], edi
            mov ecx, dword ptr [eax]
            push 0x20
            push eax
            call dword ptr [ecx + 0xcc]
            mov ecx, dword ptr [esp + 0x90]
            mov eax, dword ptr [esp + 0x8c]
            add esp, 8
            mov edx, eax
            and edx, 0xff
            push ecx
            push eax
            mov eax, dword ptr [esp + 0x7c]
            push ebp
            push ebx
            push esi
            push eax
            call dword ptr [edx*4 + RenderStyleCalls]
            mov eax, dword ptr [v1db+0xc]
            lea edx, [esp + 0x80]
            push edx
            lea edx, [esp + 0x24]
            mov ecx, dword ptr [eax]
            push 0x10
            push edx
            lea edx, [esp + 0x3c]
            push edx
            push edi
            push 0x7a
            push eax
            call dword ptr [ecx + 0x90]
            mov ebx, dword ptr [esp + 0x50]
            add esp, 0x1c
            _emit 0xd9
        _emit 0x03
            call _ftol
            mov esi, eax
            mov dword ptr [esp + 0x10], esi
            fld dword ptr [ebx + 4]
            call _ftol
            mov ebx, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x14], eax
            _emit 0xd9
        _emit 0x03
            call _ftol
            mov dword ptr [esp + 0x18], eax
            fld dword ptr [ebx + 4]
            call _ftol
            cmp esi, edi
            mov dword ptr [esp + 0x1c], eax
            jge L00521ade
            xor esi, esi
            mov dword ptr [esp + 0x10], esi
    L00521ade:
            cmp dword ptr [esp + 0x14], edi
            jge L00521ae8
            mov dword ptr [esp + 0x14], edi
    L00521ae8:
            cmp dword ptr [esp + 0x18], edi
            jge L00521af2
            mov dword ptr [esp + 0x18], edi
    L00521af2:
            cmp eax, edi
            jge L00521afc
            xor eax, eax
            mov dword ptr [esp + 0x1c], eax
    L00521afc:
            mov ecx, dword ptr [v1db+0x4fc]
            xor edx, edx
            mov dx, word ptr [ecx + 0x34]
            cmp esi, edx
            jl L00521b13
            lea esi, [edx - 1]
            mov dword ptr [esp + 0x10], esi
    L00521b13:
            mov edi, dword ptr [esp + 0x14]
            xor edx, edx
            mov dx, word ptr [ecx + 0x36]
            cmp edi, edx
            jl L00521b26
            dec edx
            mov dword ptr [esp + 0x14], edx
    L00521b26:
            mov edi, dword ptr [esp + 0x18]
            xor edx, edx
            mov dx, word ptr [ecx + 0x34]
            cmp edi, edx
            jl L00521b39
            dec edx
            mov dword ptr [esp + 0x18], edx
    L00521b39:
            xor edx, edx
            mov dx, word ptr [ecx + 0x36]
            cmp eax, edx
            jl L00521b4a
            lea eax, [edx - 1]
            mov dword ptr [esp + 0x1c], eax
    L00521b4a:
            movsx edx, word ptr [ecx + 0x38]
            mov edi, dword ptr [esp + 0x14]
            sub esi, edx
            mov dword ptr [esp + 0x10], esi
            movsx edx, word ptr [ecx + 0x3a]
            sub edi, edx
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x14], edi
            movsx edi, word ptr [ecx + 0x38]
            sub edx, edi
            mov dword ptr [esp + 0x18], edx
            movsx ecx, word ptr [ecx + 0x3a]
            sub eax, ecx
            cmp esi, edx
            mov dword ptr [esp + 0x1c], eax
            jg L00521be1
            cmp dword ptr [esp + 0x14], eax
            jg L00521be1
            mov eax, dword ptr [esp + 0x84]
            mov ecx, dword ptr [esp + 0x7c]
            lea edx, [esp + 0x10]
            push edx
            mov edx, dword ptr [esp + 0x7c]
            push offset v1db+0x20
            push eax
            mov eax, dword ptr [esp + 0x80]
            push ebp
            push ecx
            push edx
            push eax
            call dword ptr [v1db+0x4e0]
            add esp, 0x1c
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x60
            ret
    L00521bba:
            mov edx, dword ptr [esp + 0x88]
            mov eax, dword ptr [esp + 0x84]
            push edx
            push eax
            mov ecx, eax
            mov eax, dword ptr [esp + 0x7c]
            push ebp
            push ebx
            and ecx, 0xff
            push esi
            push eax
            call dword ptr [ecx*4 + RenderStyleCalls]
    L00521be1:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x60
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00521bf0
br_uint_32 C2_HOOK_CDECL BrOnScreenCheck(br_bounds3* bounds) {
    br_token r;

    v1db.renderer->dispatch->_boundsTestF(v1db.renderer, &r, (br_bounds3_f*)bounds);
    return r;
}

br_uint_16 C2_HOOK_STDCALL prependActorTransform(br_actor* ap, br_uint_16 t) {
    br_matrix34 mt;
    br_uint_16 combine;

    if (ap->t.type <= BR_TRANSFORM_MATRIX34_LP) {
        v1db.renderer->dispatch->_modelMulF(v1db.renderer, (br_matrix34_f*)&ap->t.t.mat);
    } else {
        BrTransformToMatrix34(&mt, &ap->t);
        v1db.renderer->dispatch->_modelMulF(v1db.renderer, (br_matrix34_f*)&mt);
    }
    combine = BrTransformCombineTypes(t, ap->t.type);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_HINT_T, combine ? BRT_LENGTH_PRESERVING : BRT_NONE);
    return combine;
}

// FUNCTION: CARMA2_HW 0x005221e0
void C2_HOOK_STDCALL actorRender(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t) {
    br_material* this_material;
    br_model* this_model;
    void* this_render_data;
    br_actor* a;
    br_token s;

    if (ap->children == NULL && ap->type != BR_ACTOR_MODEL) {
        return;
    }
    if (ap->render_style != BR_RSTYLE_DEFAULT) {
        style = ap->render_style;
    }
    if (style == BR_RSTYLE_NONE) {
        return;
    }
    this_material = ap->material != NULL ? ap->material : material;
    this_model = ap->model != NULL ? ap->model : model;
    this_render_data = ap->render_data != NULL ? ap->render_data : render_data;
    if (ap->t.type == BR_TRANSFORM_IDENTITY) {
        switch (ap->type) {
        case BR_ACTOR_MODEL:
            s = BrOnScreenCheck(&this_model->bounds);
            if (s != BRT_REJECT) {
                BrDbModelRender(ap, this_model, this_material, this_render_data, style, s, 1);
            }
            break;
        case BR_ACTOR_BOUNDS:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            if (s == BRT_REJECT) {
                return;
            }
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            switch (s) {
            case BRT_ACCEPT:
                for (a = ap->children; a != NULL; a = a->next) {
                    actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
                }
                return;
            case BRT_REJECT:
                return;
#ifdef BRENDER_FIX_BUGS
            default:
                break;
#endif
            }
            break;
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRender(a, this_model, this_material, this_render_data, style, t);
        }
    } else {
        v1db.renderer->dispatch->_statePush(v1db.renderer, 0x2);
        t = prependActorTransform(ap, t);
        switch (ap->type) {
        case BR_ACTOR_MODEL:
            s = BrOnScreenCheck(&this_model->bounds);
            if (s != BRT_REJECT) {
                BrDbModelRender(ap, this_model, this_material, this_render_data, style, s, 1);
            }
            break;
        case BR_ACTOR_BOUNDS:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            if (s == BRT_REJECT) {
                v1db.renderer->dispatch->_statePop(v1db.renderer, 0x2);
                return;
            }
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            switch (s) {
            case BRT_ACCEPT:
                for (a = ap->children; a != NULL; a = a->next) {
                    actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
                }
                break;
            case BRT_REJECT:
                break;
            default:
                goto render_children;
            }
            v1db.renderer->dispatch->_statePop(v1db.renderer, 0x2);
            return;
        }
    render_children:
        for (a = ap->children; a != NULL; a = a->next) {
            actorRender(a, this_model, this_material, this_render_data, style, t);
        }
        v1db.renderer->dispatch->_statePop(v1db.renderer, 0x2);
    }
}

// FUNCTION: CARMA2_HW 0x00522510
void C2_HOOK_STDCALL actorRenderOnScreen(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t) {
    br_material* this_material;
    br_model* this_model;
    void* this_render_data;
    br_actor* a;

    if (ap->render_style != BR_RSTYLE_DEFAULT) {
        style = ap->render_style;
    }
    if (style == BR_RSTYLE_NONE) {
        return;
    }
    this_material = ap->material != NULL ? ap->material : material;
    this_model = ap->model != NULL ? ap->model : model;
    this_render_data = ap->render_data != NULL ? ap->render_data : render_data;
    if (ap->t.type == BR_TRANSFORM_IDENTITY) {
        if (ap->type == BR_ACTOR_MODEL) {
            BrDbModelRender(ap, this_model, this_material, this_render_data, style, BRT_ACCEPT, 1);
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
        }
    } else {
        v1db.renderer->dispatch->_statePush(v1db.renderer, 0x2);
        t = prependActorTransform(ap, t);
        if (ap->type == BR_ACTOR_MODEL) {
            BrDbModelRender(ap, this_model, this_material, this_render_data, style, BRT_ACCEPT, 1);
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
        }
        v1db.renderer->dispatch->_statePop(v1db.renderer, 0x2);
    }
}

void C2_HOOK_STDCALL sceneRenderWorld(br_actor* world) {
    br_model* model;
    br_material* material;
    void* render_data;
    br_uint_8 style;
    br_actor* a;

    model = world->model != NULL ? world->model : v1db.default_model;
    material = world->material != NULL ? world->material : v1db.default_material;
    render_data = world->render_data != NULL ? world->render_data : v1db.default_render_data;
    style = world->render_style != BR_RSTYLE_DEFAULT ? world->render_style : BR_RSTYLE_DEFAULT;
    for (a = world->children; a != NULL; a = a->next) {
        actorRender(a, model, material, render_data, style, v1db.ttype);
    }
}

// FUNCTION: CARMA2_HW 0x00521fe0
void C2_HOOK_STDCALL sceneRenderAdd(br_actor* tree) {
    br_int_32 t;
    void* render_data;
    br_actor* a;
    br_material* material;
    br_model* model;
    br_uint_8 style;
    br_matrix34 m;

    material = NULL;
    model = NULL;
    render_data = NULL;
    style = BR_RSTYLE_DEFAULT;
    if (tree->parent == NULL) {
        actorRender(tree, v1db.default_model, v1db.default_material, v1db.default_render_data, BR_RSTYLE_DEFAULT, v1db.ttype);
        return;
    }
    t = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&m);
    for (a = tree->parent; a != NULL; a = a->parent) {
        if (material == NULL && a->material != NULL) {
            material = a->material;
        }
        if (model == NULL && a->model != NULL) {
            model = a->model;
        }
        if (render_data == NULL && a->render_data != NULL) {
            render_data = a->render_data;
        }
        if (a->render_style != BR_RSTYLE_DEFAULT) {
            style = a->render_style;
        }
        if (a == v1db.render_root) {
            break;
        }
        if (a->t.type != BR_TRANSFORM_IDENTITY) {
            BrMatrix34PostTransform(&m, &a->t);
            t = _CombineTransforms[t][a->t.type];
        }
    }
    if (material == NULL) {
        material = v1db.default_material;
    }
    if (model == NULL) {
        model = v1db.default_model;
    }
    if (render_data == NULL) {
        render_data = v1db.default_render_data;
    }
    if (t == BR_TRANSFORM_IDENTITY) {
        actorRender(tree, model, material, render_data, style, v1db.ttype);
    } else {
        v1db.renderer->dispatch->_statePush(v1db.renderer, 0x2);
        v1db.renderer->dispatch->_modelMulF(v1db.renderer, (br_matrix34_f*)&m);
        t = _CombineTransforms[(br_uint_16)v1db.ttype][(br_uint_16)t];
        v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_HINT_T, (t == BR_TRANSFORM_MATRIX34_LP) ? BRT_LENGTH_PRESERVING : BRT_NONE);
        actorRender(tree, model, material, render_data, style, t);
        v1db.renderer->dispatch->_statePop(v1db.renderer, 0x2);
    }
}

// FUNCTION: CARMA2_HW 0x00521c10
void C2_HOOK_CDECL BrDbSceneRenderBegin(br_actor* world, br_actor* camera) {
    br_matrix34 tfm;
    br_matrix4 vtos;
    br_actor* a;
    int i;
    br_token vtos_type;
    br_uint_32 dummy;

    vtos_type = CameraToScreenMatrix4(&vtos, camera);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_MATRIX, 0, BRT_VIEW_TO_SCREEN_M4_F, (uintptr_t)&vtos);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_MATRIX, 0, BRT_VIEW_TO_SCREEN_HINT_T, vtos_type);
    for (i = 0; i < (int)BR_ASIZE(v1db.camera_path); i++) {
        v1db.camera_path[i].a = NULL;
    }
    i = camera->depth;
    BrMatrix34Identity(&v1db.camera_path[i].m);
    v1db.camera_path[i].transform_type = BR_TRANSFORM_IDENTITY;
    for (a = camera; i > 0 && a != world; i--, a = a->parent) {
        BrTransformToMatrix34(&tfm, &a->t);
        BrMatrix34Mul(&v1db.camera_path[i - 1].m, &v1db.camera_path[i].m, &tfm);
        v1db.camera_path[i - 1].transform_type = _CombineTransforms[v1db.camera_path[i].transform_type][a->t.type];
        v1db.camera_path[i].a = a;
    }
    if (world != a) {
        BrFailure("camera is not in world hierachy");
    }
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_M34_F, (uintptr_t)&v1db.camera_path[i].m);
    v1db.ttype = v1db.camera_path[i].transform_type;
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_HINT_T, v1db.ttype != BR_TRANSFORM_MATRIX34 ? BRT_LENGTH_PRESERVING : BRT_NONE);
    v1db.renderer->dispatch->_modelInvert(v1db.renderer);
    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)&tfm, sizeof(tfm), BRT_MODEL_TO_VIEW_M34_F);
    BrSetupLights(world, &tfm, v1db.ttype);
    BrSetupClipPlanes(world, &tfm, v1db.ttype, &vtos);
    BrSetupEnvironment(world, &tfm, v1db.ttype);
    BrSetupHorizons(world,&tfm, v1db.ttype);
#ifndef CARPOCALYPSE2_MATCHING
    /*
     * Port-only: the hardware drivers implement per-scene setup (SDL3 GPU
     * render pass/viewport/push uniforms, GL program/FBO binding) in their
     * _sceneBegin method. The original 1.2 core never dispatched it, so the
     * ported drivers' sceneBegin never ran; call it here like dethrace does.
     */
    if (v1db.renderer->dispatch->_sceneBegin != NULL) {
        v1db.renderer->dispatch->_sceneBegin(v1db.renderer);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00521e10
br_renderbounds_cbfn* C2_HOOK_CDECL BrDbSetRenderBoundsCallback(br_renderbounds_cbfn* new_cbfn) {
    br_renderbounds_cbfn* old_cbfn;

    old_cbfn = v1db.bounds_call;
    v1db.bounds_call = new_cbfn;
    if (v1db.renderer != NULL) {
        v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_ENABLE, 0, BRT_BOUNDS_B, new_cbfn != NULL);
    }
    return old_cbfn;
}

void C2_HOOK_STDCALL SetOrigin(br_pixelmap* buffer) {

    v1db.origin.v[0] = (br_scalar)(buffer->origin_x - buffer->width / 2) / (buffer->width / 2);
    v1db.origin.v[1] = -((br_scalar)(buffer->origin_y - buffer->height / 2) / (buffer->height / 2));
}

void C2_HOOK_STDCALL SetViewport(br_pixelmap* buffer) {

    v1db.vp_ox = (br_scalar)(buffer->base_x + buffer->width / 2) + 0.5f;
    v1db.vp_width = (br_scalar)(buffer->width / 2);
    v1db.vp_oy = (br_scalar)(buffer->height / 2) + 0.5f;
    v1db.vp_height = -(br_scalar)(buffer->height / 2);
}

// FUNCTION: CARMA2_HW 0x00521e50
void C2_NAKED C2_HOOK_CDECL BrZbSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
#ifndef CARPOCALYPSE2_MATCHING

    v1db.rendering = BR_ZB_RENDERING;
    v1db.render_root = world;
    v1db.colour_buffer = colour_buffer;
    SetOrigin(colour_buffer);
    SetViewport(colour_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)depth_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, 1);
    v1db.default_render_data = NULL;
    BrDbSceneRenderBegin(world, camera);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push esi
            mov esi, dword ptr [esp + 8]
            mov dword ptr [v1db+0x8], 1
            mov dword ptr [v1db+0x98], esi
            mov dword ptr [v1db+0x4fc], eax
            mov cx, word ptr [eax + 0x34]
            push edi
            movsx edx, word ptr [eax + 0x38]
            mov edi, ecx
            and edi, 0xffff
            shr edi, 1
            sub edx, edi
            mov dword ptr [esp + 0xc], edx
            fild dword ptr [esp + 0xc]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fdivp st(1), st(0)
            fstp dword ptr [v1db+0x4e4]
            mov cx, word ptr [eax + 0x36]
            movsx edx, word ptr [eax + 0x3a]
            mov edi, ecx
            and edi, 0xffff
            shr edi, 1
            sub edx, edi
            mov dword ptr [esp + 0xc], edx
            xor edx, edx
            fild dword ptr [esp + 0xc]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            xor ecx, ecx
            fild dword ptr [esp + 0xc]
            fdivp st(1), st(0)
            fchs
            fstp dword ptr [v1db+0x4e8]
            mov dx, word ptr [eax + 0x34]
            mov cx, word ptr [eax + 0x30]
            shr edx, 1
            add ecx, edx
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4ec]
            mov cx, word ptr [eax + 0x34]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fstp dword ptr [v1db+0x4f4]
            mov dx, word ptr [eax + 0x36]
            shr dx, 1
            and edx, 0xffff
            mov dword ptr [esp + 0xc], edx
            fild dword ptr [esp + 0xc]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4f0]
            mov cx, word ptr [eax + 0x36]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            mov ecx, dword ptr [v1db+0xc]
            fild dword ptr [esp + 0xc]
            fchs
            fstp dword ptr [v1db+0x4f8]
            mov edx, dword ptr [ecx]
            push eax
            push 0xf9
            push 0
            push 0x7d
            push ecx
            call dword ptr [edx + 0x80]
            mov edx, dword ptr [esp + 0x2c]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov ecx, dword ptr [eax]
            push edx
            push 0xfa
            push 0
            push 0x7d
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov ecx, dword ptr [eax]
            push 1
            push 0xac
            push 0
            push 0x7b
            push eax
            call dword ptr [ecx + 0x80]
            mov edx, dword ptr [esp + 0x24]
            add esp, 0x14
            mov dword ptr [v1db+0x4c0], 0
            push edx
            push esi
            call BrDbSceneRenderBegin
            add esp, 8
            pop edi
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00521fd0
void C2_HOOK_CDECL BrZbSceneRenderAdd(br_actor* tree) {

    sceneRenderAdd(tree);
}

// FUNCTION: CARMA2_HW 0x005226a0
void C2_HOOK_CDECL BrZbSceneRenderEnd(void) {

    v1db.renderer->dispatch->_flush(v1db.renderer, 0);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port-only: see BrDbSceneRenderBegin. */
    if (v1db.renderer->dispatch->_sceneEnd != NULL) {
        v1db.renderer->dispatch->_sceneEnd(v1db.renderer);
    }
#endif
    v1db.rendering = 0;
    v1db.render_root = NULL;
}

// FUNCTION: CARMA2_HW 0x005226d0
void C2_NAKED C2_HOOK_CDECL BrZbSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* child;
    br_model* model;
    br_material* material;
    void* render_data;
    br_uint_8 style;
    br_uint_16 ttype;

    if (v1db.renderer == NULL) {
        return;
    }

    v1db.rendering = BR_ZB_RENDERING;
    v1db.render_root = world;
    v1db.colour_buffer = colour_buffer;

    v1db.origin.v[0] = (br_scalar)(colour_buffer->origin_x - colour_buffer->width / 2) / (br_scalar)(colour_buffer->width / 2);
    v1db.origin.v[1] = -(br_scalar)(colour_buffer->origin_y - colour_buffer->height / 2) / (br_scalar)(colour_buffer->height / 2);
    v1db.vp_ox = (br_scalar)(colour_buffer->base_x + colour_buffer->width / 2) + 0.5f;
    v1db.vp_width = (br_scalar)(colour_buffer->width / 2);
    v1db.vp_oy = (br_scalar)(colour_buffer->height / 2) + 0.5f;
    v1db.vp_height = -(br_scalar)(colour_buffer->height / 2);

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)depth_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, 1);
    v1db.default_render_data = NULL;

    BrDbSceneRenderBegin(world, camera);

    model = v1db.default_model;
    if (world->model) {
        model = world->model;
    }
    material = v1db.default_material;
    if (world->material) {
        material = world->material;
    }
    render_data = v1db.default_render_data;
    if (world->render_data) {
        render_data = world->render_data;
    }
    style = 0;
    if (world->render_style) {
        style = world->render_style;
    }
    ttype = v1db.ttype;

    child = world->children;
    while (child) {
        actorRender(child, model, material, render_data, style, ttype);
        child = child->next;
    }

    v1db.renderer->dispatch->_flush(v1db.renderer, 0);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port-only: see BrDbSceneRenderBegin. */
    if (v1db.renderer->dispatch->_sceneEnd != NULL) {
        v1db.renderer->dispatch->_sceneEnd(v1db.renderer);
    }
#endif

    v1db.rendering = 0;
    v1db.render_root = NULL;
#else
    __asm {
            mov edx, dword ptr [v1db+0xc]
            test edx, edx
            je L005228d4
            mov eax, dword ptr [esp + 0xc]
            push edi
            push esi
            mov esi, dword ptr [esp + 0xc]
            mov dword ptr [v1db+0x8], 1
            mov dword ptr [v1db+0x98], esi
            mov dword ptr [v1db+0x4fc], eax
            mov cx, word ptr [eax + 0x34]
            movsx edi, word ptr [eax + 0x38]
            push ebp
            push ebx
            mov ebx, ecx
            and ebx, 0xffff
            shr ebx, 1
            sub edi, ebx
            mov dword ptr [esp + 0x14], edi
            fild dword ptr [esp + 0x14]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0x14], ecx
            fild dword ptr [esp + 0x14]
            fdivp st(1), st(0)
            fstp dword ptr [v1db+0x4e4]
            mov cx, word ptr [eax + 0x36]
            movsx edi, word ptr [eax + 0x3a]
            mov ebx, ecx
            and ebx, 0xffff
            shr ebx, 1
            sub edi, ebx
            mov dword ptr [esp + 0x14], edi
            xor edi, edi
            fild dword ptr [esp + 0x14]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0x14], ecx
            xor ecx, ecx
            fild dword ptr [esp + 0x14]
            fdivp st(1), st(0)
            fchs
            fstp dword ptr [v1db+0x4e8]
            mov di, word ptr [eax + 0x34]
            mov cx, word ptr [eax + 0x30]
            shr edi, 1
            add ecx, edi
            mov dword ptr [esp + 0x14], ecx
            fild dword ptr [esp + 0x14]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4ec]
            mov cx, word ptr [eax + 0x34]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0x14], ecx
            fild dword ptr [esp + 0x14]
            fstp dword ptr [v1db+0x4f4]
            mov cx, word ptr [eax + 0x36]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0x14], ecx
            fild dword ptr [esp + 0x14]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4f0]
            mov cx, word ptr [eax + 0x36]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0x14], ecx
            fild dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [v1db+0x4f8]
            mov ecx, dword ptr [edx]
            push eax
            push 0xf9
            push 0
            push 0x7d
            push edx
            call dword ptr [ecx + 0x80]
            mov ecx, dword ptr [esp + 0x34]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov edx, dword ptr [eax]
            push ecx
            push 0xfa
            push 0
            push 0x7d
            push eax
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov edx, dword ptr [eax]
            push 1
            push 0xac
            push 0
            push 0x7b
            push eax
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [esp + 0x2c]
            add esp, 0x14
            mov dword ptr [v1db+0x4c0], 0
            push eax
            push esi
            call BrDbSceneRenderBegin
            mov eax, dword ptr [esi + 0x18]
            mov ebx, dword ptr [v1db+0x4b8]
            mov ebp, dword ptr [v1db+0x4bc]
            mov edi, dword ptr [v1db+0x4c0]
            add esp, 8
            mov byte ptr [esp + 0x14], 0
            test eax, eax
            je L00522871
            mov ebx, eax
    L00522871:
            mov eax, dword ptr [esi + 0x1c]
            test eax, eax
            je L0052287a
            mov ebp, eax
    L0052287a:
            mov eax, dword ptr [esi + 0x24]
            test eax, eax
            je L00522883
            mov edi, eax
    L00522883:
            mov al, byte ptr [esi + 0x20]
            test al, al
            je L0052288e
            mov byte ptr [esp + 0x14], al
    L0052288e:
            mov esi, dword ptr [esi + 8]
            test esi, esi
            je L005228b1
    L00522895:
            mov cx, word ptr [v1db+0x94]
            mov edx, dword ptr [esp + 0x14]
            push ecx
            push edx
            push edi
            push ebp
            push ebx
            push esi
            call actorRender
            mov esi, dword ptr [esi]
            test esi, esi
            jne L00522895
    L005228b1:
            mov eax, dword ptr [v1db+0xc]
            push 0
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0xfc]
            add esp, 8
            xor eax, eax
            mov dword ptr [v1db+0x8], eax
            mov dword ptr [v1db+0x98], eax
            pop ebx
            pop ebp
            pop esi
            pop edi
    L005228d4:
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

// FUNCTION: CARMA2_HW 0x005228e0
void C2_NAKED C2_HOOK_CDECL BrZsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* camera_data;

    v1db.rendering = BR_ZS_RENDERING;
    v1db.render_root = world;
    v1db.colour_buffer = colour_buffer;

    SetOrigin(colour_buffer);
    SetViewport(colour_buffer);

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)NULL);
    v1db.heap.current = v1db.heap.base;
    camera_data = camera->type_data;
    v1db.default_order_table->max_z = camera_data->yon_z;
    v1db.default_order_table->min_z = camera_data->hither_z;
    v1db.default_order_table->visits = 0;
    v1db.order_table_list = NULL;
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (uintptr_t)&v1db.default_order_table);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1PRIMITIVE_HEAP_P, (uintptr_t)&v1db.heap);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_BUCKET_SORT);
    v1db.default_render_data = v1db.default_order_table;
    BrDbSceneRenderBegin(world, camera);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push esi
            mov esi, dword ptr [esp + 8]
            mov dword ptr [v1db+0x8], 2
            mov dword ptr [v1db+0x98], esi
            mov dword ptr [v1db+0x4fc], eax
            mov cx, word ptr [eax + 0x34]
            push edi
            movsx edx, word ptr [eax + 0x38]
            mov edi, ecx
            and edi, 0xffff
            shr edi, 1
            sub edx, edi
            mov dword ptr [esp + 0xc], edx
            fild dword ptr [esp + 0xc]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fdivp st(1), st(0)
            fstp dword ptr [v1db+0x4e4]
            mov cx, word ptr [eax + 0x36]
            movsx edx, word ptr [eax + 0x3a]
            mov edi, ecx
            and edi, 0xffff
            shr edi, 1
            sub edx, edi
            mov dword ptr [esp + 0xc], edx
            xor edx, edx
            fild dword ptr [esp + 0xc]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            xor ecx, ecx
            fild dword ptr [esp + 0xc]
            fdivp st(1), st(0)
            fchs
            fstp dword ptr [v1db+0x4e8]
            mov dx, word ptr [eax + 0x34]
            mov cx, word ptr [eax + 0x30]
            shr edx, 1
            add ecx, edx
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4ec]
            mov cx, word ptr [eax + 0x34]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fstp dword ptr [v1db+0x4f4]
            mov dx, word ptr [eax + 0x36]
            shr dx, 1
            and edx, 0xffff
            mov dword ptr [esp + 0xc], edx
            fild dword ptr [esp + 0xc]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4f0]
            mov cx, word ptr [eax + 0x36]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            mov ecx, dword ptr [v1db+0xc]
            fild dword ptr [esp + 0xc]
            fchs
            fstp dword ptr [v1db+0x4f8]
            mov edx, dword ptr [ecx]
            push eax
            push 0xf9
            push 0
            push 0x7d
            push ecx
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov ecx, dword ptr [eax]
            push 0
            push 0xfa
            push 0
            push 0x7d
            push eax
            call dword ptr [ecx + 0x80]
            mov edx, dword ptr [v1db+0x4d0]
            mov edi, dword ptr [esp + 0x24]
            mov dword ptr [v1db+0x4d8], edx
            mov edx, dword ptr [v1db+0x4c4]
            mov eax, dword ptr [edi + 0x5c]
            add esp, 0x14
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [edx + 0x10], ecx
            mov ecx, dword ptr [v1db+0x4c4]
            mov eax, dword ptr [eax + 8]
            mov dword ptr [ecx + 0xc], eax
            mov edx, dword ptr [v1db+0x4c4]
            mov word ptr [edx + 0x22], 0
            mov edx, dword ptr [v1db+0x4c4]
            mov eax, dword ptr [v1db+0xc]
            push edx
            push 0x102
            mov dword ptr [v1db+0x4cc], 0
            mov ecx, dword ptr [eax]
            push 0
            push 0x7b
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov ecx, dword ptr [eax]
            push offset v1db+0x4d0
            push 0x103
            push 0
            push 0x7b
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov edx, dword ptr [eax]
            push 0x101
            push 0xac
            push 0
            push 0x7b
            push eax
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [v1db+0x4c4]
            add esp, 0x14
            mov dword ptr [v1db+0x4c0], eax
            push edi
            push esi
            call BrDbSceneRenderBegin
            add esp, 8
            pop edi
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00522ae0
void C2_HOOK_CDECL BrZsSceneRenderAdd(br_actor* tree) {

    sceneRenderAdd(tree);
}

// FUNCTION: CARMA2_HW 0x00522af0
void C2_HOOK_CDECL BrZsSceneRenderEnd(void) {

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)v1db.colour_buffer);
    if (v1db.format_buckets == NULL) {
        BrFailure("Renderer does not support buckets");
    }
    if (v1db.primary_order_table != NULL) {
        RenderPrimaryOrderTable();
    } else {
        RenderOrderTableList();
    }
    v1db.renderer->dispatch->_flush(v1db.renderer, 0);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port-only: see BrDbSceneRenderBegin. */
    if (v1db.renderer->dispatch->_sceneEnd != NULL) {
        v1db.renderer->dispatch->_sceneEnd(v1db.renderer);
    }
#endif
    v1db.rendering = 0;
    v1db.render_root = NULL;
}

// FUNCTION: CARMA2_HW 0x00522b70
void C2_NAKED C2_HOOK_CDECL BrZsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* child;
    br_model* model;
    br_material* material;
    void* render_data;
    br_uint_8 style;
    br_uint_16 ttype;

    if (v1db.renderer == NULL) {
        return;
    }
    BrZsSceneRenderBegin(world, camera, colour_buffer);

    model = v1db.default_model;
    if (world->model) {
        model = world->model;
    }
    material = v1db.default_material;
    if (world->material) {
        material = world->material;
    }
    render_data = v1db.default_render_data;
    if (world->render_data) {
        render_data = world->render_data;
    }
    style = 0;
    if (world->render_style) {
        style = world->render_style;
    }
    ttype = v1db.ttype;

    child = world->children;
    while (child) {
        actorRender(child, model, material, render_data, style, ttype);
        child = child->next;
    }

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)v1db.colour_buffer);
    if (v1db.format_buckets == NULL) {
        BrFailure("Renderer does not support buckets");
    }
    if (v1db.primary_order_table != NULL) {
        RenderPrimaryOrderTable();
    } else {
        RenderOrderTableList();
    }
    v1db.renderer->dispatch->_flush(v1db.renderer, 0);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port-only: see BrDbSceneRenderBegin. */
    if (v1db.renderer->dispatch->_sceneEnd != NULL) {
        v1db.renderer->dispatch->_sceneEnd(v1db.renderer);
    }
#endif
    v1db.rendering = 0;
    v1db.render_root = NULL;
#else
    __asm {
            mov eax, dword ptr [v1db+0xc]
            test eax, eax
            je L00522c6f
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            push edi
            push esi
            mov esi, dword ptr [esp + 0xc]
            push ebp
            push ebx
            push eax
            push ecx
            push esi
            call BrZsSceneRenderBegin
            mov eax, dword ptr [esi + 0x18]
            mov edx, dword ptr [v1db+0x4b8]
            mov ebx, dword ptr [v1db+0x4bc]
            mov ebp, dword ptr [v1db+0x4c0]
            add esp, 0xc
            mov dword ptr [esp + 0x18], edx
            test eax, eax
            mov byte ptr [esp + 0x1c], 0
            je L00522bbe
            mov dword ptr [esp + 0x18], eax
    L00522bbe:
            mov eax, dword ptr [esi + 0x1c]
            test eax, eax
            je L00522bc7
            mov ebx, eax
    L00522bc7:
            mov eax, dword ptr [esi + 0x24]
            test eax, eax
            je L00522bd0
            mov ebp, eax
    L00522bd0:
            mov al, byte ptr [esi + 0x20]
            test al, al
            je L00522bdb
            mov byte ptr [esp + 0x1c], al
    L00522bdb:
            mov esi, dword ptr [esi + 8]
            test esi, esi
            je L00522c01
            mov edi, dword ptr [esp + 0x1c]
    L00522be6:
            mov ax, word ptr [v1db+0x94]
            mov ecx, dword ptr [esp + 0x18]
            push eax
            push edi
            push ebp
            push ebx
            push ecx
            push esi
            call actorRender
            mov esi, dword ptr [esi]
            test esi, esi
            jne L00522be6
    L00522c01:
            mov ecx, dword ptr [v1db+0x4fc]
            mov eax, dword ptr [v1db+0xc]
            push ecx
            xor esi, esi
            mov edx, dword ptr [eax]
            push 0xf9
            push esi
            push 0x7d
            push eax
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [v1db+0x18]
            add esp, 0x14
            cmp eax, esi
            jne L00522c39
            push offset gZlib_filesystem+0x220
            call BrFailure
            add esp, 4
    L00522c39:
            cmp dword ptr [v1db+0x4c8], esi
            je L00522c48
            call RenderPrimaryOrderTable
            jmp L00522c4d
    L00522c48:
            call RenderOrderTableList
    L00522c4d:
            mov eax, dword ptr [v1db+0xc]
            push esi
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 0xfc]
            add esp, 8
            mov dword ptr [v1db+0x8], esi
            mov dword ptr [v1db+0x98], esi
            pop ebx
            pop ebp
            pop esi
            pop edi
    L00522c6f:
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00522c70
br_primitive_cbfn* C2_HOOK_CDECL BrZsPrimitiveCallbackSet(br_primitive_cbfn* new_cbfn) {
    br_primitive_cbfn* old_cbfn;

    old_cbfn = v1db.primitive_call;
    v1db.primitive_call = new_cbfn;
    return old_cbfn;
}

// FUNCTION: CARMA2_HW 0x00522c80
void C2_NAKED C2_HOOK_CDECL BrZbsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* camera_data;

    v1db.rendering = BR_ZB_RENDERING | BR_ZS_RENDERING;
    v1db.render_root = world;
    v1db.colour_buffer = colour_buffer;

    SetOrigin(colour_buffer);
    SetViewport(colour_buffer);

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)NULL);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port (from the Vita/hardware port): the Zbs path is the hardware scene
     * path. Give the renderer the scene's depth buffer so depth testing works
     * in glrend/sdl3gpurend; the 1.2 code always cleared it because softrend
     * hid surfaces on the CPU. */
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)depth_buffer);
#endif
    v1db.heap.current = v1db.heap.base;
    camera_data = camera->type_data;
    v1db.default_order_table->max_z = camera_data->yon_z;
    v1db.default_order_table->min_z = camera_data->hither_z;
    v1db.default_order_table->visits = 0;
    v1db.order_table_list = NULL;
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (uintptr_t)v1db.default_order_table);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1PRIMITIVE_HEAP_P, (uintptr_t)&v1db.heap);
    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_BUCKET_AND_BUFFER);
    v1db.default_render_data = v1db.default_order_table;
    BrDbSceneRenderBegin(world, camera);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push esi
            mov esi, dword ptr [esp + 8]
            mov dword ptr [v1db+0x8], 3
            mov dword ptr [v1db+0x98], esi
            mov dword ptr [v1db+0x4fc], eax
            mov cx, word ptr [eax + 0x34]
            push edi
            movsx edx, word ptr [eax + 0x38]
            mov edi, ecx
            and edi, 0xffff
            shr edi, 1
            sub edx, edi
            mov dword ptr [esp + 0xc], edx
            fild dword ptr [esp + 0xc]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fdivp st(1), st(0)
            fstp dword ptr [v1db+0x4e4]
            mov cx, word ptr [eax + 0x36]
            movsx edx, word ptr [eax + 0x3a]
            mov edi, ecx
            and edi, 0xffff
            shr edi, 1
            sub edx, edi
            mov dword ptr [esp + 0xc], edx
            xor edx, edx
            fild dword ptr [esp + 0xc]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            xor ecx, ecx
            fild dword ptr [esp + 0xc]
            fdivp st(1), st(0)
            fchs
            fstp dword ptr [v1db+0x4e8]
            mov dx, word ptr [eax + 0x34]
            mov cx, word ptr [eax + 0x30]
            shr edx, 1
            add ecx, edx
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4ec]
            mov cx, word ptr [eax + 0x34]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            fild dword ptr [esp + 0xc]
            fstp dword ptr [v1db+0x4f4]
            mov dx, word ptr [eax + 0x36]
            shr dx, 1
            and edx, 0xffff
            mov dword ptr [esp + 0xc], edx
            fild dword ptr [esp + 0xc]
            fsub dword ptr [IID_IDirectInputDevice2A+0x440]
            fstp dword ptr [v1db+0x4f0]
            mov cx, word ptr [eax + 0x36]
            shr cx, 1
            and ecx, 0xffff
            mov dword ptr [esp + 0xc], ecx
            mov ecx, dword ptr [v1db+0xc]
            fild dword ptr [esp + 0xc]
            fchs
            fstp dword ptr [v1db+0x4f8]
            mov edx, dword ptr [ecx]
            push eax
            push 0xf9
            push 0
            push 0x7d
            push ecx
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov ecx, dword ptr [eax]
            push 0
            push 0xfa
            push 0
            push 0x7d
            push eax
            call dword ptr [ecx + 0x80]
            mov ecx, dword ptr [esp + 0x2c]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov edx, dword ptr [eax]
            push ecx
            push 0xfa
            push 0
            push 0x7d
            push eax
            call dword ptr [edx + 0x80]
            mov edx, dword ptr [v1db+0x4d0]
            mov edi, dword ptr [esp + 0x24]
            mov dword ptr [v1db+0x4d8], edx
            mov edx, dword ptr [v1db+0x4c4]
            mov eax, dword ptr [edi + 0x5c]
            add esp, 0x14
            mov ecx, dword ptr [eax + 0xc]
            mov dword ptr [edx + 0x10], ecx
            mov ecx, dword ptr [v1db+0x4c4]
            mov eax, dword ptr [eax + 8]
            mov dword ptr [ecx + 0xc], eax
            mov edx, dword ptr [v1db+0x4c4]
            mov word ptr [edx + 0x22], 0
            mov edx, dword ptr [v1db+0x4c4]
            mov eax, dword ptr [v1db+0xc]
            push edx
            push 0x102
            mov dword ptr [v1db+0x4cc], 0
            mov ecx, dword ptr [eax]
            push 0
            push 0x7b
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov ecx, dword ptr [eax]
            push offset v1db+0x4d0
            push 0x103
            push 0
            push 0x7b
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x14
            mov edx, dword ptr [eax]
            push 0x1b3
            push 0xac
            push 0
            push 0x7b
            push eax
            call dword ptr [edx + 0x80]
            mov eax, dword ptr [v1db+0x4c4]
            add esp, 0x14
            mov dword ptr [v1db+0x4c0], eax
            push edi
            push esi
            call BrDbSceneRenderBegin
            add esp, 8
            pop edi
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00522ea0
void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor* tree) {

    sceneRenderAdd(tree);
}

// FUNCTION: CARMA2_HW 0x00522eb0
void C2_HOOK_CDECL BrZbsSceneRenderEnd(void) {

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)v1db.colour_buffer);
    if (v1db.format_buckets == NULL) {
        BrFailure("Renderer does not support buckets");
    }
    if (v1db.primary_order_table != NULL) {
        RenderPrimaryOrderTable();
    } else {
        RenderOrderTableList();
    }
    v1db.renderer->dispatch->_flush(v1db.renderer, 0);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port-only: see BrDbSceneRenderBegin. */
    if (v1db.renderer->dispatch->_sceneEnd != NULL) {
        v1db.renderer->dispatch->_sceneEnd(v1db.renderer);
    }
#endif
    v1db.rendering = 0;
    v1db.render_root = NULL;
}

// FUNCTION: CARMA2_HW 0x00522f30
void C2_NAKED C2_HOOK_CDECL BrZbsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* child;
    br_model* model;
    br_material* material;
    void* render_data;
    br_uint_8 style;
    br_uint_16 ttype;

    if (v1db.renderer == NULL) {
        return;
    }
    BrZbsSceneRenderBegin(world, camera, colour_buffer, depth_buffer);

    model = v1db.default_model;
    if (world->model) {
        model = world->model;
    }
    material = v1db.default_material;
    if (world->material) {
        material = world->material;
    }
    render_data = v1db.default_render_data;
    if (world->render_data) {
        render_data = world->render_data;
    }
    style = 0;
    if (world->render_style) {
        style = world->render_style;
    }
    ttype = v1db.ttype;

    child = world->children;
    while (child) {
        actorRender(child, model, material, render_data, style, ttype);
        child = child->next;
    }

    v1db.renderer->dispatch->_partSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)v1db.colour_buffer);
    if (v1db.format_buckets == NULL) {
        BrFailure("Renderer does not support buckets");
    }
    if (v1db.primary_order_table != NULL) {
        RenderPrimaryOrderTable();
    } else {
        RenderOrderTableList();
    }
    v1db.renderer->dispatch->_flush(v1db.renderer, 0);
#ifndef CARPOCALYPSE2_MATCHING
    /* Port-only: see BrDbSceneRenderBegin. */
    if (v1db.renderer->dispatch->_sceneEnd != NULL) {
        v1db.renderer->dispatch->_sceneEnd(v1db.renderer);
    }
#endif
    v1db.rendering = 0;
    v1db.render_root = NULL;
#else
    __asm {
            mov eax, dword ptr [v1db+0xc]
            test eax, eax
            je L00523034
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 8]
            push edi
            push esi
            mov esi, dword ptr [esp + 0xc]
            push ebp
            push ebx
            push eax
            push ecx
            push edx
            push esi
            call BrZbsSceneRenderBegin
            mov eax, dword ptr [v1db+0x4b8]
            mov ebx, dword ptr [v1db+0x4bc]
            mov ebp, dword ptr [v1db+0x4c0]
            mov dword ptr [esp + 0x2c], eax
            mov eax, dword ptr [esi + 0x18]
            add esp, 0x10
            test eax, eax
            mov byte ptr [esp + 0x20], 0
            je L00522f82
            mov dword ptr [esp + 0x1c], eax
    L00522f82:
            mov eax, dword ptr [esi + 0x1c]
            test eax, eax
            je L00522f8b
            mov ebx, eax
    L00522f8b:
            mov eax, dword ptr [esi + 0x24]
            test eax, eax
            je L00522f94
            mov ebp, eax
    L00522f94:
            mov al, byte ptr [esi + 0x20]
            test al, al
            je L00522f9f
            mov byte ptr [esp + 0x20], al
    L00522f9f:
            mov esi, dword ptr [esi + 8]
            test esi, esi
            je L00522fc6
            mov edi, dword ptr [esp + 0x20]
    L00522faa:
            mov cx, word ptr [v1db+0x94]
            mov edx, dword ptr [esp + 0x1c]
            push ecx
            push edi
            push ebp
            push ebx
            push edx
            push esi
            call actorRender
            mov esi, dword ptr [esi]
            test esi, esi
            jne L00522faa
    L00522fc6:
            mov edx, dword ptr [v1db+0x4fc]
            mov eax, dword ptr [v1db+0xc]
            push edx
            xor esi, esi
            mov ecx, dword ptr [eax]
            push 0xf9
            push esi
            push 0x7d
            push eax
            call dword ptr [ecx + 0x80]
            mov eax, dword ptr [v1db+0x18]
            add esp, 0x14
            cmp eax, esi
            jne L00522ffe
            push offset gZlib_filesystem+0x244
            call BrFailure
            add esp, 4
    L00522ffe:
            cmp dword ptr [v1db+0x4c8], esi
            je L0052300d
            call RenderPrimaryOrderTable
            jmp L00523012
    L0052300d:
            call RenderOrderTableList
    L00523012:
            mov eax, dword ptr [v1db+0xc]
            push esi
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0xfc]
            add esp, 8
            mov dword ptr [v1db+0x8], esi
            mov dword ptr [v1db+0x98], esi
            pop ebx
            pop ebp
            pop esi
            pop edi
    L00523034:
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

// FUNCTION: CARMA2_HW 0x00523040
void C2_HOOK_CDECL BrZbModelRender(br_actor* actor, br_model* model, br_material* material, br_uint_8 style, int on_screen, int use_custom) {

    BrDbModelRender(actor, model, material, NULL, style, on_screen, use_custom);
}

// FUNCTION: CARMA2_HW 0x00523070
void C2_HOOK_CDECL BrZsModelRender(br_actor* actor, br_model* model, br_material* material, br_order_table* order_table, br_uint_8 style, int on_screen, int use_custom) {

    BrDbModelRender(actor, model, material, order_table, style, on_screen, use_custom);
}

// FUNCTION: CARMA2_HW 0x005230a0
br_renderbounds_cbfn* C2_HOOK_CDECL BrZbRenderBoundsCallbackSet(br_renderbounds_cbfn* new_cbfn) {

    if (!(v1db.zx_active & BR_ZX_ZB_ACTIVE_MASK)) {
        BrFailure("BrZbSetRenderBoundsCallback called before BrZbBegin");
        return NULL;
    }
    return BrDbSetRenderBoundsCallback(new_cbfn);
}

// FUNCTION: CARMA2_HW 0x00523100
br_renderbounds_cbfn* C2_HOOK_CDECL BrZsRenderBoundsCallbackSet(br_renderbounds_cbfn* new_cbfn) {

    if ((v1db.zx_active & BR_ZX_ZS_ACTIVE_MASK) == 0) {
        BrFailure("BrZsSetRenderBoundsCallback called before BrZsBegin");
        return NULL;
    }
    return BrDbSetRenderBoundsCallback(new_cbfn);
}
