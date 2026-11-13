#include "otable.h"

#include "custsupt.h"
#include "dbsetup.h"

#include "core/std/brstdlib.h"
#include "core/fw/resource.h"

#include "c2_stdlib.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrModelToScreenQuery();
extern char IID_IDirectInputDevice2A[];
extern void __cdecl _ftol();
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x00526300
br_order_table* C2_HOOK_CDECL BrZsOrderTableAllocate(br_uint_16 size, br_uint_32 flags, br_uint_16 type) {
    br_order_table* order_table;

    C2_HOOK_BUG_ON(sizeof(br_order_table) != 36);

    order_table = BrResAllocate(v1db.res, sizeof(br_order_table), BR_MEMORY_ORDER_TABLE);
    order_table->table = BrResAllocate(order_table, size * sizeof(br_primitive*), BR_MEMORY_ORDER_TABLE);
    order_table->size = size;
    order_table->next = NULL;
    order_table->flags = flags;
    order_table->type = type;
    order_table->visits = 0;
    order_table->min_z = 0.0f;
    order_table->max_z = 0.0f;
    order_table->sort_z = 0.0f;
    order_table->scale = 0.0f;
    return order_table;
}

// FUNCTION: CARMA2_HW 0x00526370
void C2_HOOK_CDECL BrZsOrderTableFree(br_order_table* order_table) {

    BrResFree(order_table);
}

// FUNCTION: CARMA2_HW 0x00526380
br_order_table* C2_HOOK_CDECL BrZsActorOrderTableSet(br_actor* actor, br_order_table* order_table) {

    actor->render_data = order_table;
    return order_table;
}

// FUNCTION: CARMA2_HW 0x00526390
br_order_table* C2_HOOK_CDECL BrZsActorOrderTableGet(br_actor* actor) {

    return actor->render_data;
}

// FUNCTION: CARMA2_HW 0x005263a0
br_order_table* C2_HOOK_CDECL BrZsOrderTableClear(br_order_table* order_table) {

    BrMemSet(order_table->table, 0, order_table->size * sizeof(br_primitive*));
    return order_table;
}

// FUNCTION: CARMA2_HW 0x005263c0
void C2_HOOK_CDECL BrZsOrderTablePrimitiveInsert(br_order_table* order_table, br_primitive* primitive, br_uint_16 bucket) {

    primitive->next = order_table->table[bucket];
    order_table->table[bucket] = primitive;
}

// FUNCTION: CARMA2_HW 0x005263f0
br_uint_16 C2_NAKED C2_HOOK_CDECL BrZsPrimitiveBucketSelect(br_scalar* z, br_uint_16 type, br_scalar min_z, br_scalar max_z, br_uint_16 size, br_uint_16 sort_type) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_16 bucket;
    br_scalar zprim;
    br_scalar range;
    br_scalar scale;

#ifdef BRENDER_FIX_BUGS
    zprim = 0.f;
#endif

    switch (type) {
    case BRT_POINT:
        zprim = z[0];
        break;
    case BRT_LINE:
        switch (sort_type) {
        case 1:
            zprim = (z[0] + z[1]) / 2.f;
            break;
        case 2:
            zprim = z[0];
            if (z[1] < zprim) {
                zprim = z[1];
            }
            break;
        case 3:
            zprim = z[0];
            if (zprim < z[1]) {
                zprim = z[1];
            }
            break;
        default:
            abort();
        }
    case BRT_TRIANGLE:
        switch (sort_type) {
        case 1:
            zprim = (z[0] + z[1] + z[3]) * (1.f / 3.f);
            break;
        case 2:
            zprim = z[0];
            if (z[1] < zprim) {
                zprim = z[1];
            }
            if (z[2] < zprim) {
                zprim = z[2];
            }
            break;
        case 3:
            zprim = z[0];
            if (zprim < z[1]) {
                zprim = z[1];
            }
            if (zprim < z[2]) {
                zprim = z[2];
            }
            break;
        default:
            abort();
        }
    default:
        abort();
    }
    range = max_z - min_z;
    if (range < 0.001f) {
        scale = 1000.f;
    } else {
        scale = size / range;
    }
    zprim -= min_z;
    if (zprim < 0.f) {
        zprim = 0.f;
    }
    bucket = (br_uint_16)(scale * zprim);
    if (bucket >= size) {
        bucket = size - 1;
    }
    return bucket;
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            push esi
            and eax, 0xffff
            sub eax, 0xd1
            je L005264d9
            sub eax, 0x39
            je L00526490
            dec eax
            jne L005264e1
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 8]
            and eax, 0xffff
            _emit 0xd9
        _emit 0x01
            dec eax
            je L00526480
            dec eax
            je L0052645c
            dec eax
            jne L005264e5
            fld dword ptr [ecx + 4]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L00526442
            fstp st(0)
            fld dword ptr [ecx + 4]
    L00526442:
            fld dword ptr [ecx + 8]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L005264e5
            fstp st(0)
            fld dword ptr [ecx + 8]
            jmp L005264e5
    L0052645c:
            fld dword ptr [ecx + 4]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L0052646d
            fstp st(0)
            fld dword ptr [ecx + 4]
    L0052646d:
            fld dword ptr [ecx + 8]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L005264e5
            fstp st(0)
            fld dword ptr [ecx + 8]
            jmp L005264e5
    L00526480:
            fld dword ptr [ecx + 8]
            fadd dword ptr [ecx + 4]
            faddp st(1), st(0)
            fmul dword ptr [IID_IDirectInputDevice2A+0x450]
            jmp L005264e5
    L00526490:
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 8]
            and eax, 0xffff
            _emit 0xd9
        _emit 0x01
            dec eax
            je L005264ce
            dec eax
            je L005264bb
            dec eax
            jne L005264e5
            fld dword ptr [ecx + 4]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L005264e5
            fstp st(0)
            fld dword ptr [ecx + 4]
            jmp L005264e5
    L005264bb:
            fld dword ptr [ecx + 4]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L005264e5
            fstp st(0)
            fld dword ptr [ecx + 4]
            jmp L005264e5
    L005264ce:
            fadd dword ptr [ecx + 4]
            fmul dword ptr [IID_IDirectInputDevice2A+0x454]
            jmp L005264e5
    L005264d9:
            mov eax, dword ptr [esp + 8]
            _emit 0xd9
        _emit 0x00
            jmp L005264e5
    L005264e1:
            fld dword ptr [esp + 8]
    L005264e5:
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x10]
            mov esi, dword ptr [esp + 0x18]
            fcom dword ptr [IID_IDirectInputDevice2A+0x458]
            fnstsw ax
            test ah, 0x41
            jne L00526518
            mov ecx, esi
            and ecx, 0xffff
            mov dword ptr [esp + 8], ecx
            fild dword ptr [esp + 8]
            fdiv st(0), st(1)
            fstp dword ptr [esp + 8]
            fstp st(0)
            jmp L00526522
    L00526518:
            fstp st(0)
            mov dword ptr [esp + 8], 0x447a0000
    L00526522:
            fsub dword ptr [esp + 0x10]
            fcom dword ptr [IID_IDirectInputDevice2A+0x460]
            fnstsw ax
            test ah, 1
            je L0052653b
            fstp st(0)
            fld dword ptr [IID_IDirectInputDevice2A+0x460]
    L0052653b:
            fld dword ptr [esp + 8]
            fmul st(0), st(1)
            call _ftol
            cmp ax, si
            fstp st(0)
            jb L00526550
            lea eax, [esi - 1]
    L00526550:
            pop esi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00526560
void C2_HOOK_CDECL BrZsOrderTablePrimaryEnable(br_order_table* order_table) {

    if (order_table == NULL) {
        order_table = v1db.default_order_table;
    }
    v1db.primary_order_table = order_table;
}

// FUNCTION: CARMA2_HW 0x00526580
void C2_HOOK_CDECL BrZsOrderTablePrimaryDisable(void) {

    v1db.primary_order_table = NULL;
}

// FUNCTION: CARMA2_HW 0x00526590
void C2_HOOK_STDCALL InsertOrderTableList(br_order_table* order_table) {
    br_order_table* previous_table;
    br_order_table* current_table;

    if (order_table == v1db.primary_order_table) {
        return;
    }

    if (v1db.order_table_list == NULL) {
        v1db.order_table_list = order_table;
        order_table->next = NULL;
        return;
    }
    if (v1db.order_table_list->sort_z < order_table->sort_z) {
        order_table->next = v1db.order_table_list;
        v1db.order_table_list = order_table;
        return;
    }
    current_table = v1db.order_table_list->next;
    previous_table = v1db.order_table_list;
    while (current_table != NULL && !(current_table->sort_z < order_table->sort_z)) {
        previous_table = current_table;
        current_table = current_table->next;
    }
    previous_table->next = order_table;
    order_table->next = current_table;
}

// FUNCTION: CARMA2_HW 0x00526600
void C2_NAKED C2_HOOK_STDCALL SetOrderTableBounds(br_bounds* bounds, br_order_table* order_table) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 i;
    br_scalar element;
    br_scalar min_z;
    br_scalar max_z;
    br_vector3* min;
    br_vector3* max;

    if((order_table->flags & 0x1) || ((order_table->flags & 0x2) && order_table->visits==1)) {
        min = &bounds->min;
        max = &bounds->max;
        BrModelToScreenQuery(&v1db.model_to_screen);
        min_z = v1db.model_to_screen.m[3][3];
        max_z = v1db.model_to_screen.m[3][3];

        for (i = 0; i < 3; i++) {
            element = v1db.model_to_screen.m[i][3];
            if (element > 0.0f) {
                max_z += element * max->v[i];
                min_z += element * min->v[i];
            } else {
                max_z += element * min->v[i];
                min_z += element * max->v[i];
            }
        }
        order_table->min_z = min_z;
        order_table->max_z = max_z;
    }
    SetOrderTableRange(order_table);
#else
    __asm {
            push ebx
            mov ebx, dword ptr [esp + 0xc]
            push esi
            push edi
            mov eax, dword ptr [ebx + 0x1c]
            test al, 1
            jne L00526619
            test al, 2
            je L0052667b
            cmp word ptr [ebx + 0x22], 1
            jne L0052667b
    L00526619:
            mov edi, dword ptr [esp + 0x10]
            push offset v1db+0x20
            lea esi, [edi + 0xc]
            call BrModelToScreenQuery
            fld dword ptr [v1db+0x5c]
            add esp, 4
            mov ecx, esi
            fld st(0)
            mov edx, offset v1db+0x2c
            sub edi, esi
    L0052663e:
            _emit 0xd9
        _emit 0x02
            fcom dword ptr [IID_IDirectInputDevice2A+0x460]
            fnstsw ax
            test ah, 0x41
            jne L00526654
            fld dword ptr [edi + ecx]
            _emit 0xd9
        _emit 0x01
            jmp L00526659
    L00526654:
            _emit 0xd9
        _emit 0x01
            fld dword ptr [edi + ecx]
    L00526659:
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            faddp st(4), st(0)
            add edx, 0x10
            add ecx, 4
            faddp st(2), st(0)
            cmp edx, offset v1db+0x5c
            fstp st(0)
            jb L0052663e
            fstp dword ptr [ebx + 0xc]
            fstp dword ptr [ebx + 0x10]
    L0052667b:
            fld dword ptr [ebx + 0x10]
            fsub dword ptr [ebx + 0xc]
            fst dword ptr [esp + 0x14]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x458]
            fnstsw ax
            test ah, 0x41
            jne L005266a9
            xor eax, eax
            mov ax, word ptr [ebx + 4]
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fdiv dword ptr [esp + 0x14]
            fstp dword ptr [ebx + 0x18]
            jmp L005266b0
    L005266a9:
            mov dword ptr [ebx + 0x18], 0x447a0000
    L005266b0:
            mov eax, dword ptr [ebx + 0x1c]
            test al, 8
            je L005266c3
            mov ecx, dword ptr [ebx + 0xc]
            mov dword ptr [ebx + 0x14], ecx
            pop edi
            pop esi
            pop ebx
            ret 8
    L005266c3:
            test al, 0x10
            je L005266d3
            mov edx, dword ptr [ebx + 0x10]
            mov dword ptr [ebx + 0x14], edx
            pop edi
            pop esi
            pop ebx
            ret 8
    L005266d3:
            test al, 0x20
            je L005266e7
            fld dword ptr [esp + 0x14]
            fmul dword ptr [IID_IDirectInputDevice2A+0x454]
            fadd dword ptr [ebx + 0xc]
            fstp dword ptr [ebx + 0x14]
    L005266e7:
            pop edi
            pop esi
            pop ebx
            ret 8
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005266f0
void C2_HOOK_STDCALL SetOrderTableRange(br_order_table* order_table) {
    br_scalar range;

    range = order_table->max_z - order_table->min_z;
    if (range > 0.001f) {
        order_table->scale = ((float) order_table->size) / range;
    } else {
        order_table->scale = 1000.0f;
    }
    if ((order_table->flags & 0x8) != 0) {
        order_table->sort_z = order_table->min_z;
    } else {
        if ((order_table->flags & 0x10) != 0){
            order_table->sort_z = order_table->max_z;
        } else {
            if ((order_table->flags & 0x20) != 0) {
                float scaled_range = 0.5f * range;

                order_table->sort_z = order_table->min_z + scaled_range;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00526770
void C2_HOOK_STDCALL RenderOrderTableList(void) {
    br_order_table* order_table;

    order_table = v1db.order_table_list;
    while (order_table != NULL) {
        ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table = order_table->next;
    }
}

// FUNCTION: CARMA2_HW 0x005267b0
void C2_HOOK_STDCALL RenderPrimaryOrderTable(void) {
    br_uint_16 m;
    br_uint_16 size;
    br_scalar bucket_size;
    br_scalar min_z;
    br_scalar max_z;
    br_primitive** bucket;
    br_order_table* order_table;

    if (v1db.primary_order_table->visits == 0) {
        RenderOrderTableList();
        return;
    }

    min_z = v1db.primary_order_table->min_z;
    max_z = v1db.primary_order_table->max_z;
    size = v1db.primary_order_table->size;
    bucket = v1db.primary_order_table->table + (size - 1);

    order_table = v1db.order_table_list;
    while (order_table!=NULL) {
        if (order_table->sort_z < max_z) {
            break;
        }
        ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table=order_table->next;
    }
    bucket_size = (float)(max_z - min_z) / (float)size;

    for (m = 0; m < size; m++, bucket--) {
        if (*bucket != NULL) {
            ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, bucket, 1);
        }
        max_z -= bucket_size;
        while (order_table != NULL) {
            if (order_table->sort_z < max_z) {
                break;
            }
            ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
            order_table->visits = 0;
            order_table=order_table->next;
        }
    }
    while (order_table != NULL) {
        ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table = order_table->next;
    }

	v1db.primary_order_table->visits=0;
}
