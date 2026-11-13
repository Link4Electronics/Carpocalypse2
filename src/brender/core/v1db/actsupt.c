#include "actsupt.h"

#include "dbsetup.h"
#include "enables.h"

#include "core/fw/brlists.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/pattern.h"
#include "core/fw/resource.h"

#include "core/math/matrix34.h"
#include "core/math/matrix4.h"
#include "core/math/transfrm.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34PreTransform();
extern br_boolean C2_HOOK_CDECL BrNamePatternMatch();
extern char IID_IDirectInputDevice2A[];
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x0051ded0
br_uint_32 C2_HOOK_CDECL BrActorEnum(br_actor* parent, br_actor_enum_cbfn* callback, void* arg) {
    br_actor* a;
    br_actor* next;
    br_uint_32 r;

    a = parent->children;

    while (a != NULL) {
        next = a->next;
        r = callback(a, arg);
        if (r != 0) {
            return r;
        }
        a = next;
    }

    return 0;
}

// FUNCTION: CARMA2_HW 0x0051df00
br_uint_32 C2_NAKED C2_HOOK_CDECL BrActorSearchMany(br_actor* root, char* pattern, br_actor** actors, int max) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* a;
    char* sub;
    int n;
    int remaining;

    sub = NULL;
    if (pattern != NULL) {
        while (*pattern == '/') {
            pattern++;
        }
        sub = pattern;
        while (*sub != '\0' && *sub != '/') {
            sub++;
        }
        while (*sub == '/') {
            sub++;
        }
    }
    a = root->children;
    remaining = max;
    while (a != NULL && remaining > 0) {
        if (BrNamePatternMatch(pattern, a->identifier) != 0) {
            if (sub == NULL || *sub == '\0') {
                *actors = a;
                actors++;
                remaining--;
            } else {
                n = BrActorSearchMany(a, sub, actors, remaining);
                actors += n;
                remaining -= n;
            }
        }
        a = a->next;
    }
    return max - remaining;
#else
    __asm {
    L0051df00:
            mov ecx, dword ptr [esp + 8]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x18]
            push esi
            push edi
            xor edi, edi
            test ecx, ecx
            je L0051df44
            cmp byte ptr [ecx], 0x2f
            jne L0051df23
    L0051df17:
            mov al, byte ptr [ecx + 1]
            inc ecx
            cmp al, 0x2f
            je L0051df17
            mov dword ptr [esp + 0x18], ecx
    L0051df23:
            mov al, byte ptr [ecx]
            mov edi, ecx
            test al, al
            je L0051df37
    L0051df2b:
            cmp al, 0x2f
            je L0051df37
            mov al, byte ptr [edi + 1]
            inc edi
            test al, al
            jne L0051df2b
    L0051df37:
            cmp byte ptr [edi], 0x2f
            jne L0051df44
    L0051df3c:
            mov al, byte ptr [edi + 1]
            inc edi
            cmp al, 0x2f
            je L0051df3c
    L0051df44:
            mov eax, dword ptr [esp + 0x14]
            mov ebx, dword ptr [eax + 8]
            test ebx, ebx
            je L0051df96
            mov esi, dword ptr [esp + 0x1c]
            jmp L0051df59
    L0051df55:
            mov ecx, dword ptr [esp + 0x18]
    L0051df59:
            test ebp, ebp
            jle L0051df96
            mov edx, dword ptr [ebx + 0x14]
            push edx
            push ecx
            call BrNamePatternMatch
            add esp, 8
            test eax, eax
            je L0051df90
            test edi, edi
            je L0051df8a
            cmp byte ptr [edi], 0
            je L0051df8a
            push ebp
            push esi
            push edi
            push ebx
            call L0051df00
            add esp, 0x10
            lea esi, [esi + eax*4]
            sub ebp, eax
            jmp L0051df90
    L0051df8a:
            mov dword ptr [esi], ebx
            add esi, 4
            dec ebp
    L0051df90:
            mov ebx, dword ptr [ebx]
            test ebx, ebx
            jne L0051df55
    L0051df96:
            mov eax, dword ptr [esp + 0x20]
            pop edi
            sub eax, ebp
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051dfb0
br_actor* C2_HOOK_CDECL BrActorSearch(br_actor* root, char* pattern) {
    br_actor* a;

    if (BrActorSearchMany(root, pattern, &a, 1) == 1) {
        return a;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0051e040
void C2_HOOK_STDCALL RenumberActor(br_actor* a, int d) {
    br_actor* ac;

    a->depth = d;
    ac = a->children;
    while (ac != NULL) {
        RenumberActor(ac, d + 1);
        ac = ac->next;
    }
}

// FUNCTION: CARMA2_HW 0x0051dfe0
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent, br_actor* a) {
    br_actor* ac;

    BrSimpleAddHead((br_simple_list*)&parent->children, (br_simple_node*)a);
    a->parent = parent;

    a->depth = parent->depth + 1;
    for (ac = a->children; ac != NULL; ac = ac->next) {
        RenumberActor(ac, a->depth + 1);
    }
    return a;
}

// FUNCTION: CARMA2_HW 0x0051e070
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* a) {
    br_actor* ac;

    BrSimpleRemove((br_simple_node*)a);
    a->parent = NULL;
    a->depth = 0;

    for (ac = a->children; ac != NULL; ac = ac->next) {
        RenumberActor(ac, 1);
    }
    return a;
}

// FUNCTION: CARMA2_HW 0x0051e0d0
void C2_HOOK_CDECL BrActorRelink(br_actor* parent, br_actor* a) {
    br_matrix34 mat;

    if (a->parent == parent) {
        return;
    }

    BrActorToActorMatrix34(&mat, a, parent);
    BrMatrix34ToTransform(&a->t, &mat);
    BrActorAdd(parent, BrActorRemove(a));
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x0040ca40
void C2_HOOK_CDECL BrActorRelink_library(br_actor* parent, br_actor* a) {
    br_matrix34 mat;

    if (a->parent != parent) {
        BrActorToActorMatrix34_Legacy(&mat, a, parent);
        BrMatrix34ToTransform(&a->t, &mat);
        BrActorAdd(parent, BrActorRemove(a));
    }
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x0051e1a0
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type, void* type_data) {
    br_actor* a;
    br_light* light;
    br_camera* camera;
    br_bounds* bounds;
    br_vector4* clip_plane;

    C2_HOOK_BUG_ON(sizeof(br_actor) != 100);
    a = BrResAllocate(v1db.res, sizeof(br_actor), BR_MEMORY_ACTOR);
    BrSimpleNewList((br_simple_list*)&a->children);
    a->type = type;
    a->depth = 0;
    a->t.type = BR_TRANSFORM_MATRIX34;
    BrMatrix34Identity(&a->t.t.mat);

    if (type_data != NULL) {
        a->type_data = type_data;
    } else {
        switch (type) {
        case BR_ACTOR_LIGHT:
            C2_HOOK_BUG_ON(sizeof(br_light) != 32);
            light = BrResAllocate(a, sizeof(br_light), BR_MEMORY_LIGHT);
            a->type_data = light;
            light->type = BR_LIGHT_DIRECT;
            light->colour = BR_COLOUR_RGB(0xff, 0xff, 0xff);
            light->attenuation_c = 1.0f;
            light->cone_outer = BR_ANGLE_DEG(15.0f);
            light->cone_inner = BR_ANGLE_DEG(10.0f);
            break;
        case BR_ACTOR_CAMERA:
            C2_HOOK_BUG_ON(sizeof(br_camera) != 36);
            camera = (br_camera*)BrResAllocate(a, sizeof(br_camera), BR_MEMORY_CAMERA);
            a->type_data = camera;
            camera->type = BR_CAMERA_PERSPECTIVE_FOV;
            camera->field_of_view = BR_ANGLE_DEG(45.0f);
            camera->hither_z = 1.0f;
            camera->yon_z = 10.0f;
            camera->aspect = 1.0f;
            break;
        case BR_ACTOR_BOUNDS:
        case BR_ACTOR_BOUNDS_CORRECT:
            C2_HOOK_BUG_ON(sizeof(br_bounds) != 24);
            bounds = BrResAllocate(a, sizeof(br_bounds), BR_MEMORY_BOUNDS);
            a->type_data = bounds;
            break;
        case BR_ACTOR_CLIP_PLANE:
            clip_plane = BrResAllocate(a, sizeof(br_vector4), BR_MEMORY_BOUNDS);
            clip_plane->v[0] = 0.0f;
            clip_plane->v[1] = 0.0f;
            clip_plane->v[2] = 1.0f;
            clip_plane->v[3] = 0.0f;
            a->type_data = clip_plane;
            break;
        }
    }
    return a;
}

// FUNCTION: CARMA2_HW 0x0051e300
void C2_HOOK_STDCALL InternalActorFree(br_actor* a) {

    while (a->children != NULL) {
        InternalActorFree((br_actor*)BrSimpleRemove((br_simple_node*)a->children));
    }
    BrActorEnableCheck(a);
    BrResFree(a);
}

// FUNCTION: CARMA2_HW 0x0051e2c0
void C2_HOOK_CDECL BrActorFree(br_actor* a) {

    InternalActorFree(a);
}

// FUNCTION: CARMA2_HW 0x0051e340
br_boolean C2_HOOK_STDCALL ActorToRoot(br_actor* a, br_actor* world, br_matrix34* m) {
    if (a == world) {
        BrMatrix34Identity(m);
        return 1;
    } else {
        BrTransformToMatrix34(m, &a->t);
        for (a = a->parent; a != NULL && a != world; a = a->parent) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(m, &a->t);
            }
        }
        return a == world;
    }
}

// FUNCTION: CARMA2_HW 0x0051e3b0
br_boolean C2_HOOK_STDCALL ActorToRootTyped(br_actor* a, br_actor* world, br_matrix34* m, br_int_32* type) {
    br_int_32 t;

	if (a == world) {
		BrMatrix34Identity(m);
		*type = BR_TRANSFORM_IDENTITY;
		return 1;
	}
    BrTransformToMatrix34(m, &a->t);
	t = a->t.type;
	for (a = a->parent; a != NULL && a != world; a = a->parent) {
		if (a->t.type != BR_TRANSFORM_IDENTITY) {
            BrMatrix34PostTransform(m, &a->t);
			t = BrTransformCombineTypes(t, a->t.type);
		}
	}
	*type = t;
	return a == world;
}

// FUNCTION: CARMA2_HW 0x0051e450
void C2_NAKED C2_HOOK_STDCALL Matrix4PerspectiveNew(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon, br_scalar origin_x, br_scalar origin_y) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar scale;

    scale = BR_COS(field_of_view / 2) / BR_SIN(field_of_view / 2);

    mat->m[0][0] = scale / aspect;
    mat->m[0][1] = 0.f;
    mat->m[0][2] = 0.f;
    mat->m[0][3] = 0.f;
    mat->m[1][0] = 0.f;
    mat->m[1][1] = scale;
    mat->m[1][2] = 0.f;
    mat->m[1][3] = 0.f;
    mat->m[2][0] = -origin_x;
    mat->m[2][1] = -origin_y;
    mat->m[2][2] = (yon + hither) / (yon - hither);
    mat->m[2][3] = -1.f;
    mat->m[3][0] = 0.f;
    mat->m[3][1] = 0.f;
    mat->m[3][2] = -2.f * (yon * hither) / (yon - hither);
    mat->m[3][3] = 0.f;
#else
    __asm {
            mov dx, word ptr [esp + 8]
            mov eax, dword ptr [esp + 4]
            shr dx, 1
            and edx, 0xffff
            xor ecx, ecx
            mov dword ptr [esp + 8], edx
            mov dword ptr [eax + 4], ecx
            fild dword ptr [esp + 8]
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x10]
            fxch st(1)
            fmul qword ptr [IID_IDirectInputDevice2A+0x3c0]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x10]
            fld st(1)
            fld st(2)
            fxch st(1)
            fcos
            mov dword ptr [eax + 8], ecx
            mov dword ptr [eax + 0xc], ecx
            mov dword ptr [eax + 0x10], ecx
            mov dword ptr [eax + 0x18], ecx
            mov dword ptr [eax + 0x1c], ecx
            mov dword ptr [eax + 0x2c], 0xbf800000
            mov dword ptr [eax + 0x30], ecx
            mov dword ptr [eax + 0x34], ecx
            mov dword ptr [eax + 0x3c], ecx
            fxch st(1)
            fsin
            fxch st(1)
            fst dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 8]
            fdivp st(1), st(0)
            fld dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x10]
            fxch st(4)
            fstp dword ptr [esp + 8]
            fxch st(1)
            fdiv dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fdiv dword ptr [esp + 0xc]
            fxch st(3)
            fdiv dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x3c8]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x14]
            fxch st(6)
            _emit 0xd9
        _emit 0x18
            fxch st(1)
            fchs
            fxch st(1)
            fchs
            fxch st(5)
            fstp dword ptr [eax + 0x14]
            fxch st(2)
            fstp dword ptr [eax + 0x28]
            fstp dword ptr [eax + 0x38]
            fstp dword ptr [eax + 0x20]
            fxch st(1)
            fstp dword ptr [eax + 0x24]
            fstp st(0)
            ret 0x1c
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051e520
br_token C2_HOOK_STDCALL CameraToScreenMatrix4(br_matrix4* mat, br_actor* camera) {
    br_camera* camera_type;
    br_matrix34 mat34;

    camera_type = camera->type_data;
	switch (camera_type->type) {
	case BR_CAMERA_PARALLEL:
		if (camera_type->width == 0.f) {
			BrFailure("Parallel camera has zero width");
        }
		if (camera_type->height == 0.f) {
			BrFailure("Parallel camera has zero height");
        }
		if (camera_type->yon_z <= camera_type->hither_z) {
			BrFailure("Parallel camera has invalid yon and hither");
        }

		BrMatrix34Scale(&mat34,
            2.f / (camera_type->width * camera_type->aspect),
            2.f / camera_type->height,
            2.f / (camera_type->yon_z - camera_type->hither_z));
		BrMatrix34PreTranslate(&mat34,
            0.f,
            0.f,
            (camera_type->hither_z + camera_type->yon_z) / 2.f);
		BrMatrix34PostTranslate(&mat34,
            v1db.origin.v[0],
            v1db.origin.v[1],
            0.f);
		BrMatrix4Copy34(mat, &mat34);
		return BRT_PARALLEL;
	case BR_CAMERA_PERSPECTIVE_FOV:
		Matrix4PerspectiveNew(mat,
			camera_type->field_of_view,
            camera_type->aspect,
			-camera_type->hither_z,
            -camera_type->yon_z,
			v1db.origin.v[0],
            v1db.origin.v[1]);
		return BRT_PERSPECTIVE;
    case BR_CAMERA_PERSPECTIVE_WHD:
        return BRT_PERSPECTIVE;
	case BR_CAMERA_PARALLEL_OLD:
		if (camera_type->width == 0.f) {
            BrFailure("Parallel camera has zero width");
        }
        if (camera_type->height == 0.f) {
            BrFailure("Parallel camera has zero height");
        }
		if (camera_type->yon_z <= camera_type->hither_z) {
            BrFailure("Parallel camera has invalid yon and hither");
        }
		BrMatrix34Scale(&mat34,
			1.f / camera_type->width,
			1.f / camera_type->height,
			1.f / (camera_type->yon_z - camera_type->hither_z));
		BrMatrix34PreTranslate(&mat34,
            0.f,
            0.f,
            camera_type->hither_z);
		BrMatrix4Copy34(mat,&mat34);
		return BRT_PARALLEL;
	}
	return BR_NULL_TOKEN;
}

// FUNCTION: CARMA2_HW 0x0051e7d0
br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34(br_matrix34* m, br_actor* a, br_actor* b) {
    br_matrix34 mata;
    br_matrix34 matb;
    br_matrix34 matc;
    br_uint_8 at;
    br_uint_8 bt;

    if (a == b) {
        BrMatrix34Identity(m);
        return BR_TRANSFORM_IDENTITY;
    }

    if (a->parent == b) {
        BrTransformToMatrix34(m, &a->t);
        return a->t.type;
    }

    if (b->parent == a) {
        BrTransformToMatrix34(&matb, &b->t);

        if (BrTransformTypeIsLP(b->t.type))
            BrMatrix34LPInverse(m, &matb);
        else
            BrMatrix34Inverse(m, &matb);

        return b->t.type;
    }

    at = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mata);

    bt = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&matb);

    while (a && b && a != b) {

        if (a->depth > b->depth) {

            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mata, &a->t);
                at = BrTransformCombineTypes(at, a->t.type);
            }
            a = a->parent;

        } else if (b->depth > a->depth) {

            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&matb, &b->t);
                bt = BrTransformCombineTypes(bt, b->t.type);
            }
            b = b->parent;

        } else {

            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mata, &a->t);
                at = BrTransformCombineTypes(at, a->t.type);
            }
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&matb, &b->t);
                bt = BrTransformCombineTypes(bt, b->t.type);
            }
            b = b->parent;
            a = a->parent;
        }
    }

    if (bt == BR_TRANSFORM_IDENTITY) {
        BrMatrix34Copy(m, &mata);
        return at;
    }

    if (at == BR_TRANSFORM_IDENTITY) {
        if (BrTransformTypeIsLP(bt)) {
            BrMatrix34LPInverse(m, &matb);
        } else
            BrMatrix34Inverse(m, &matb);

        return bt;
    }

    if (BrTransformTypeIsLP(bt)) {
        BrMatrix34LPInverse(&matc, &matb);
    } else
        BrMatrix34Inverse(&matc, &matb);

    BrMatrix34Mul(m, &mata, &matc);

    return BrTransformCombineTypes(at, bt);
}

// FUNCTION: CARMA2_HW 0x0040c660
br_uint_8 C2_HOOK_CDECL BrActorToActorMatrix34_Legacy(br_matrix34* m, br_actor* a, br_actor* b) {
    br_uint_8 transform_type_combine[7][7] = {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1, 2, 2 },
        { 0, 1, 1, 1, 1, 3, 3 },
        { 0, 1, 1, 1, 1, 4, 4 },
        { 0, 1, 2, 3, 4, 5, 5 },
        { 0, 1, 2, 3, 4, 5, 6 },
    };
    br_matrix34 mat_a;
    br_matrix34 mat_b;
    br_matrix34 mat_c;
    br_uint_8 a_type;
    br_uint_8 b_type;

    if (a == b) {
        BrMatrix34Identity(m);
        return BR_TRANSFORM_IDENTITY;
    }
    if (a->parent == b) {
        BrTransformToMatrix34(m, &a->t);
        return (br_uint_8)a->t.type;
    }
    if (b->parent == a) {
        BrTransformToMatrix34(&mat_b, &b->t);
        if (b->t.type != BR_TRANSFORM_MATRIX34) {
            BrMatrix34LPInverse(m, &mat_b);
        } else {
            BrMatrix34Inverse(m, &mat_b);
        }
        return (br_uint_8)b->t.type;
    }

    a_type = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mat_a);
    b_type = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mat_b);

    while (a != NULL && b != NULL && a != b) {
        if (a->depth > b->depth) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_a, &a->t);
                a_type = transform_type_combine[a_type][a->t.type];
            }
            a = a->parent;
        } else if (b->depth > a->depth) {
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_b, &b->t);
                b_type = transform_type_combine[b_type][b->t.type];
            }
            b = b->parent;
        } else {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_a, &a->t);
                a_type = transform_type_combine[a_type][a->t.type];
            }
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mat_b, &b->t);
                b_type = transform_type_combine[b_type][b->t.type];
            }
            b = b->parent;
            a = a->parent;
        }
    }
    if (b_type == BR_TRANSFORM_IDENTITY) {
        BrMatrix34Copy(m, &mat_a);
        return a_type;
    }
    if (a_type == BR_TRANSFORM_IDENTITY) {
        if (b_type != BR_TRANSFORM_MATRIX34) {
            BrMatrix34LPInverse(m, &mat_b);
        } else {
            BrMatrix34Inverse(m, &mat_b);
        }
        return b_type;
    }
    if (b_type != BR_TRANSFORM_MATRIX34) {
        BrMatrix34LPInverse(&mat_c, &mat_b);
    } else {
        BrMatrix34Inverse(&mat_c, &mat_b);
    }
    BrMatrix34Mul(m, &mat_a, &mat_c);
    return transform_type_combine[a_type][b_type];
}

// FUNCTION: CARMA2_HW 0x0051eae0
void C2_HOOK_CDECL BrActorToScreenMatrix4(br_matrix4* m, br_actor* a, br_actor* camera) {
    br_matrix34 a2c;

    CameraToScreenMatrix4(m, camera);
    if (a != camera) {
        BrActorToActorMatrix34(&a2c, a, camera);
        BrMatrix4Pre34(m, &a2c);
    }
}

void C2_HOOK_CDECL BrMatrix34ApplyBounds(br_bounds* A, br_bounds* B, br_matrix34* C) {
    int i;
    int j;
    br_scalar a;
    br_scalar b;

    A->min.v[0] = A->max.v[0] = C->m[3][0];
    A->min.v[1] = A->max.v[1] = C->m[3][1];
    A->min.v[2] = A->max.v[2] = C->m[3][2];

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            a = C->m[j][i] * B->min.v[j];
            b = C->m[j][i] * B->max.v[j];
            if (a < b) {
                A->min.v[i] += a;
                A->max.v[i] += b;
            } else {
                A->min.v[i] += b;
                A->max.v[i] += a;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051ebc0
void C2_NAKED C2_HOOK_STDCALL ActorToBounds(br_bounds* dest, br_actor* ap, br_model* model) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* a;
    br_bounds new;
    br_matrix34 m2v;
    int i;

    if (ap->model != NULL) {
        model = ap->model;
    }

    m2v = v1db.model_to_view;
    BrMatrix34PreTransform(&v1db.model_to_view, &ap->t);

    if (ap->type == BR_ACTOR_MODEL) {
        BrMatrix34ApplyBounds(&new, &model->bounds, &v1db.model_to_view);

        for (i = 0; i < 3; i++) {
            if (new.min.v[i] < dest->min.v[i]) {
                dest->min.v[i] = new.min.v[i];
            }

            if (new.max.v[i] > dest->max.v[i]) {
                dest->max.v[i] = new.max.v[i];
            }
        }
    }

    for(a = ap->children; a != NULL; a = a->next) {
        ActorToBounds(dest, a, model);
    }

    v1db.model_to_view = m2v;
#else
    __asm {
    L0051ebc0:
            sub esp, 0x48
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x58]
            push esi
            push edi
            mov eax, dword ptr [ebp + 0x18]
            test eax, eax
            je L0051ebda
            mov ebx, eax
            mov dword ptr [esp + 0x64], ebx
            jmp L0051ebde
    L0051ebda:
            mov ebx, dword ptr [esp + 0x64]
    L0051ebde:
            lea eax, [ebp + 0x28]
            mov ecx, 0xc
            mov esi, offset v1db+0x60
            lea edi, [esp + 0x28]
            push eax
            push offset v1db+0x60
            rep movsd
            call BrMatrix34PreTransform
            mov al, byte ptr [ebp + 0x12]
            mov ebp, dword ptr [esp + 0x64]
            add esp, 8
            cmp al, 1
            jne L0051ecc6
            fld dword ptr [v1db+0x84]
            add ebx, 0x34
            xor ecx, ecx
            fst dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [v1db+0x88]
            fst dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x14]
            fld dword ptr [v1db+0x8c]
            fst dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x18]
    L0051ec3d:
            mov edx, ebx
            lea esi, [ecx + v1db+0x60]
            mov edi, 3
    L0051ec4a:
            _emit 0xd9
        _emit 0x06
            _emit 0xd9
        _emit 0x02
            fld dword ptr [edx + 0xc]
            fxch st(1)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x0e
            fld st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L0051ec64
            fxch st(1)
    L0051ec64:
            fadd dword ptr [esp + ecx + 0x10]
            add edx, 4
            add esi, 0xc
            dec edi
            fstp dword ptr [esp + ecx + 0x10]
            fadd dword ptr [esp + ecx + 0x1c]
            fstp dword ptr [esp + ecx + 0x1c]
            jne L0051ec4a
            add ecx, 4
            cmp ecx, 0xc
            jl L0051ec3d
            lea edx, [esp + 0x10]
            lea esi, [esp + 0x1c]
            sub edx, ebp
            mov ecx, ebp
            sub esi, ebp
            mov edi, 3
    L0051ec98:
            fld dword ptr [edx + ecx]
            _emit 0xd8
        _emit 0x19
            fnstsw ax
            test ah, 1
            je L0051eca9
            mov eax, dword ptr [edx + ecx]
            mov dword ptr [ecx], eax
    L0051eca9:
            fld dword ptr [esi + ecx]
            fcomp dword ptr [ecx + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L0051ecbc
            mov eax, dword ptr [esi + ecx]
            mov dword ptr [ecx + 0xc], eax
    L0051ecbc:
            add ecx, 4
            dec edi
            jne L0051ec98
            mov ebx, dword ptr [esp + 0x64]
    L0051ecc6:
            mov ecx, dword ptr [esp + 0x60]
            mov esi, dword ptr [ecx + 8]
            test esi, esi
            je L0051ecdf
    L0051ecd1:
            push ebx
            push esi
            push ebp
            call L0051ebc0
            mov esi, dword ptr [esi]
            test esi, esi
            jne L0051ecd1
    L0051ecdf:
            mov ecx, 0xc
            lea esi, [esp + 0x28]
            mov edi, offset v1db+0x60
            rep movsd
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x48
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051eb20
br_bounds* C2_HOOK_CDECL BrActorToBounds(br_bounds* b, br_actor* ap) {
    br_matrix34 m2v;
    br_model* model;
    br_actor* a;

    m2v = v1db.model_to_view;

    if (ap->model == NULL) {
        model = v1db.default_model;
    } else {
        model = ap->model;
    }

    BrMatrix34Identity(&v1db.model_to_view);

    if (ap->type == BR_ACTOR_MODEL) {
        *b = model->bounds;
    } else {
        b->min.v[0] = b->min.v[1] = b->min.v[2] = BR_SCALAR_MAX;
        b->max.v[0] = b->max.v[1] = b->max.v[2] = BR_SCALAR_MIN;
    }

    for (a = ap->children; a != NULL; a = a->next) {
        ActorToBounds(b, a, model);
    }

    v1db.model_to_view = m2v;
    return b;
}
