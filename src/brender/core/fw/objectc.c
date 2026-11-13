#include "objectc.h"

#include "brender/brender.h"

#include "brlists.h"
#include "../std/brstdlib.h"
#include "fwsetup.h"
#include "pattern.h"
#include "tokenval.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrMemFree();
extern br_boolean C2_HOOK_CDECL BrNamePatternMatch();
#endif
// FUNCTION: CARMA2_HW 0x0052d190
void* C2_HOOK_CDECL BrObjectListAllocate(void* res) {
    object_list* hl;

    hl = BrResAllocate(res, sizeof(object_list), BR_MEMORY_OBJECT_LIST);
    BrSimpleNewList(&hl->l);
    return hl;
}

// FUNCTION: CARMA2_HW 0x0052d1c0
br_error C2_HOOK_CDECL _M_br_object_container_addFront(br_object_container* self, br_object* ph) {

    object_list* hl;
    object_list_entry* he;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_object_container_dispatch, _listQuery, 0x44);
    C2_HOOK_BUG_ON(sizeof(object_list_entry) != 0xc);

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    he = BrResAllocate(hl, sizeof(object_list_entry), BR_MEMORY_OBJECT_LIST_ENTRY);
    if (he == NULL) {
        return 0x1003;
    }
    he->h = ph;
    BrSimpleAddHead(&hl->l, &he->n);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052d210
br_error C2_HOOK_CDECL _M_br_object_container_remove(br_object_container* self, br_object* h) {
    object_list* hl;
    object_list_entry* he;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    for (he = (object_list_entry*)hl->l.head; he != NULL; he = (object_list_entry*)he->n.next) {
        if (he->h == h) {
            break;
        }
    }
    if (he == NULL || he->h != h) {
        return 0x1002;
    }
    BrSimpleRemove(&he->n);
    BrResFree(he);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052d260
br_error C2_HOOK_CDECL _M_br_object_container_removeFront(br_object_container* self, br_object** ph) {
    object_list* hl;
    object_list_entry* he;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    he = (object_list_entry*)BrSimpleRemHead(&hl->l);
    if (he == NULL) {
        *ph = NULL;
        return 0x1002;
    }
    *ph = he->h;
    BrResFree(he);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052d2b0
br_error C2_HOOK_CDECL _M_br_object_container_find(br_object_container* self, br_object** ph, br_token type, const char* pattern, br_token_value* tv) {
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    br_error r = 0x1002;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
#ifdef BRENDER_FIX_BUGS
    tvarg = NULL;
#endif
    if (tv != NULL) {
        tvarg = self->dispatch->_tokensMatchBegin(self, type, tv);
    }
    for (he = (object_list_entry*)hl->l.head; he != NULL; he = (object_list_entry*)he->n.next) {


        if ((type == BR_NULL_TOKEN || type == he->h->dispatch->_type(he->h))
                    && BrNamePatternMatch(pattern, he->h->dispatch->_identifier(he->h))) {
            if (tv != NULL && !self->dispatch->_tokensMatch(self, he->h, tvarg)) {
                continue;
            }
            *ph = he->h;
            r = 0;
        }
    }
    if (tv != NULL) {
        self->dispatch->_tokensMatchEnd(self, tvarg);
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0052d390
br_error C2_NAKED C2_HOOK_CDECL _M_br_object_container_findMany(br_object_container* self, br_object** objects, br_int_32 max_objects, br_int_32* num_objects, br_token type, const char* pattern, br_token_value* tv) {
#ifndef CARPOCALYPSE2_MATCHING
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    int n;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    tvarg = NULL;
    if (tv != NULL) {
        tvarg = self->dispatch->_tokensMatchBegin(self, type, tv);
    }
    n = 0;
    for (he = (object_list_entry*)hl->l.head; he != NULL; he = (object_list_entry*)he->n.next) {
        if (type != BR_NULL_TOKEN && he->h->dispatch->_type(he->h) != type) {
            continue;
        }
        if (!BrNamePatternMatch(pattern, he->h->dispatch->_identifier(he->h))) {
            continue;
        }
        if (tv != NULL && !self->dispatch->_tokensMatch(self, he->h, tvarg)) {
            continue;
        }
        if (n < max_objects) {
            objects[n] = he->h;
        }
        n++;
    }
    if (tv != NULL) {
        self->dispatch->_tokensMatchEnd(self, tvarg);
    }
    *num_objects = n;
    if (n > max_objects) {
        return 0x1004;
    }
    return 0;
#else
    __asm {
            push ecx
            push esi
            push edi
            mov edi, dword ptr [esp + 0x10]
            mov dword ptr [esp + 8], 0
            push edi
            mov eax, dword ptr [edi]
            call dword ptr [eax + 0x44]
            mov esi, eax
            add esp, 4
            test esi, esi
            jne L0052d3b7
            mov eax, 0x1002
            pop edi
            pop esi
            pop ecx
            ret
    L0052d3b7:
            push ebp
            mov ebp, dword ptr [esp + 0x24]
            push ebx
            mov ebx, dword ptr [esp + 0x30]
            test ebx, ebx
            je L0052d3d4
            mov ecx, dword ptr [edi]
            push ebx
            push ebp
            push edi
            call dword ptr [ecx + 0x48]
            add esp, 0xc
            mov dword ptr [esp + 0x18], eax
    L0052d3d4:
            mov esi, dword ptr [esi]
            test esi, esi
            je L0052d450
            mov ebx, dword ptr [esp + 0x1c]
    L0052d3de:
            test ebp, ebp
            je L0052d3f2
            mov eax, dword ptr [esi + 8]
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 0x18]
            add esp, 4
            cmp ebp, eax
            jne L0052d446
    L0052d3f2:
            mov eax, dword ptr [esi + 8]
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x14]
            mov edx, dword ptr [esp + 0x30]
            add esp, 4
            push eax
            push edx
            call BrNamePatternMatch
            add esp, 8
            test eax, eax
            je L0052d446
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L0052d42e
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [edi]
            push ecx
            push edx
            push edi
            call dword ptr [eax + 0x4c]
            add esp, 0xc
            test eax, eax
            je L0052d446
    L0052d42e:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x20]
            cmp eax, ecx
            jge L0052d442
            mov ecx, dword ptr [esi + 8]
            mov dword ptr [ebx], ecx
            add ebx, 4
    L0052d442:
            inc dword ptr [esp + 0x10]
    L0052d446:
            mov esi, dword ptr [esi]
            test esi, esi
            jne L0052d3de
            mov ebx, dword ptr [esp + 0x30]
    L0052d450:
            test ebx, ebx
            je L0052d462
            mov eax, dword ptr [esp + 0x18]
            mov edx, dword ptr [edi]
            push eax
            push edi
            call dword ptr [edx + 0x50]
            add esp, 8
    L0052d462:
            mov eax, dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x24]
            xor edx, edx
            cmp eax, edi
            pop ebx
            setg dl
            dec edx
            pop ebp
            and edx, 0xffffeffc
            mov dword ptr [ecx], eax
            add edx, 0x1004
            pop edi
            mov eax, edx
            pop esi
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052d490
br_error C2_HOOK_CDECL _M_br_object_container_count(br_object_container* self, br_uint_32* pcount, br_token type, const char* pattern, br_token_value* tv) {
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    int n;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    tvarg = NULL;
    if (tv != NULL) {
        tvarg = self->dispatch->_tokensMatchBegin(self, type, tv);
    }
    n = 0;
    for (he = (object_list_entry*)hl->l.head; he != NULL; he = (object_list_entry*)he->n.next) {
        if (type != BR_NULL_TOKEN && he->h->dispatch->_type(he->h) != type) {
            continue;
        }
        if (!BrNamePatternMatch(pattern, he->h->dispatch->_identifier(he->h))) {
            continue;
        }
        if (tv != NULL && !self->dispatch->_tokensMatch(self, he->h, tvarg)) {
            continue;
        }
        n++;
    }
    if (tv != NULL) {
        self->dispatch->_tokensMatchEnd(self, tvarg);
    }
    *pcount = n;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052d560
void* C2_HOOK_CDECL _M_br_object_container_tokensMatchBegin(br_object_container* self, br_token t, br_token_value* tv) {
    token_match* tm;
    br_int_32 i;

    C2_HOOK_BUG_ON(sizeof(token_match) != 20);

    if (tv == NULL) {
        return 0;
    }
    tm = BrResAllocate(fw.res, sizeof(token_match), BR_MEMORY_APPLICATION);
    tm->original = tv;
    for (i = 0; tv[i].t != BR_NULL_TOKEN; i++) {
    }
    tm->n = i + 1;
    tm->query = BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
    BrMemCpy(tm->query, tv, i * sizeof(br_token_value));
    return tm;
}

// FUNCTION: CARMA2_HW 0x0052d5e0
br_boolean C2_HOOK_CDECL _M_br_object_container_tokensMatch(br_object_container* self, br_object* h, void* arg) {
    token_match* tm;
    br_size_t s;
    br_int_32 n;

    tm = arg;
    if (arg == NULL) {
        return 1;
    }
    h->dispatch->_queryManySize(h, &s, tm->query);
    if (s > tm->extra_size) {
        if (tm->extra != NULL) {
            BrResFree(tm->extra);
        }
        tm->extra = BrResAllocate(tm, s, BR_MEMORY_APPLICATION);
        tm->extra_size = s;
    }
    h->dispatch->_queryMany(h, tm->query, tm->extra, tm->extra_size, &n);
    return BrTokenValueCompare(tm->original, tm->query);
}

// FUNCTION: CARMA2_HW 0x0052d670
void C2_HOOK_CDECL _M_br_object_container_tokensMatchEnd(br_object_container* self, void* arg) {

    if (arg != NULL) {
        BrResFree(arg);
    }
}

// FUNCTION: CARMA2_HW 0x0052d690
br_error C2_NAKED C2_HOOK_CDECL BrObjectContainerFree(br_object_container* self, br_token type, const char* pattern, br_token_value* tv) {
#ifndef CARPOCALYPSE2_MATCHING
    br_error r;
    br_object** handles;
    br_uint_32 count;
    br_int_32 n;
    br_int_32 i;

    r = self->dispatch->_count(self, &count, type, pattern, tv);
    if (r != 0) {
        return r;
    }
    handles = BrMemAllocate(count * sizeof(br_object*), BR_MEMORY_DRIVER);
    self->dispatch->_findMany(self, handles, count, &n, type, pattern, tv);
    for (i = 0; i < n; i++) {
        self->dispatch->_remove(self, handles[i]);
        handles[i]->dispatch->_free(handles[i]);
    }
    BrMemFree(handles);
    return 0;
#else
    __asm {
            push ebx
            mov ebx, dword ptr [esp + 0x10]
            push ebp
            mov ebp, dword ptr [esp + 0x10]
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            mov edi, dword ptr [esp + 0x20]
            mov eax, dword ptr [esi]
            push edi
            push ebx
            lea ecx, [esp + 0x1c]
            push ebp
            push ecx
            push esi
            call dword ptr [eax + 0x68]
            add esp, 0x14
            test eax, eax
            jne L0052d72e
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0052d72c
            lea edx, [eax*4]
            push 0x24
            push edx
            call BrMemAllocate
            add esp, 8
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x20]
            mov dword ptr [esp + 0x1c], eax
            push edi
            push ebx
            push ebp
            push edx
            mov edx, dword ptr [esp + 0x24]
            push edx
            push eax
            push esi
            call dword ptr [ecx + 0x64]
            mov eax, dword ptr [esp + 0x3c]
            add esp, 0x1c
            xor ebx, ebx
            test eax, eax
            jle L0052d71f
            mov edi, dword ptr [esp + 0x1c]
    L0052d6fc:
            mov ecx, dword ptr [edi]
            mov eax, dword ptr [esi]
            push ecx
            push esi
            call dword ptr [eax + 0x5c]
            mov eax, dword ptr [edi]
            add esp, 8
            mov edx, dword ptr [eax]
            push eax
            call dword ptr [edx + 0x10]
            mov eax, dword ptr [esp + 0x24]
            add esp, 4
            inc ebx
            add edi, 4
            cmp ebx, eax
            jl L0052d6fc
    L0052d71f:
            mov eax, dword ptr [esp + 0x1c]
            push eax
            call BrMemFree
            add esp, 4
    L0052d72c:
            xor eax, eax
    L0052d72e:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}
