#include "resource.h"


#include "core/fw/brlists.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/mem.h"
#include "core/fw/scratch.h"
#include "core/std/brstdlib.h"

#include <stdio.h>
#include "c2_stdlib.h"

#define RES_ALIGN 4
#define RESOURCE_SIZE(RES) (((RES)->size_l << 2) | ((RES)->size_m << 10) | ((RES)->size_h << 18))

void* ResToUserWithClass(resource_header* r, br_uint_8 class) {
    br_int_32 align;

    align = fw.resource_class_index[class]->alignment;
    if (align <= 0) {
        align = RES_ALIGN;
    }
    return (void*)(((br_size_t)r + sizeof(resource_header) + align - 1) & ~(align - 1));
}

void* C2_HOOK_CDECL ResToUser(resource_header* r) {

    return ResToUserWithClass(r, r->class);
}

resource_header* C2_HOOK_CDECL UserToRes(void* r) {
    br_uint_8* p = r;

    while (p[-1] == 0) {
        p--;
    }
#ifdef CARPOCALYPSE2_MATCHING
    p -= offsetof(struct resource_header, magic_num) + sizeof(((struct resource_header *)NULL)->magic_num);
#else
    p -= sizeof(resource_header);
#endif
#ifdef BRENDER_FIX_BUGS
    if (((resource_header*)p)->magic_num != 0xdeadbeef) {
        abort();
        // FIXME: add panic macro
        // LOG_PANIC("Bad resource header from user at %p. Was 0x%x", r, ((resource_header*)p)->magic_num);
    }
#endif
    return (resource_header*)p;
}

#ifdef CARPOCALYPSE2_MATCHING
extern br_order_table _BrDefaultOrderTable;
extern br_framework_state fw;
#endif
// FUNCTION: CARMA2_HW 0x005276c0
void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class) {
    resource_header* res;
    resource_header* parent;
    br_int_32 malign;
    br_int_32 calign;
    br_int_32 pad;
    br_int_32 actual_pad;

    size = (size + sizeof(resource_header) + RES_ALIGN - 1) & ~(RES_ALIGN - 1);

    malign = BrMemAlign(res_class);

    calign = fw.resource_class_index[res_class]->alignment;
    if (calign <= 0) {
        calign = RES_ALIGN;
    }

    pad = (~(malign - 1)) & (calign - 1);
    res = BrMemAllocate(pad + size, res_class);
    actual_pad = (((br_size_t)res + calign - 1) & (~(calign - 1))) - (br_size_t)res;
    if (actual_pad > pad) {
        BrFailure("Memory allocator broke alignment");
    }
    res->class = res_class;
    res->size_l = (br_uint_8)(size >> 2);
    res->size_m = (br_uint_8)(size >> 10);
    res->size_h = (br_uint_8)(size >> 18);
    BrSimpleNewList(&res->children);
    res->magic_num = 0xdeadbeef;
    res->magic_ptr = res;

    if (vparent != NULL) {
        // vparent points to a resource body, we track backwards to obtain its resource_header
        parent = UserToRes(vparent);
        BrSimpleAddHead(&parent->children, &res->node);
    }

#if 0
    if ((uintptr_t)UserToRes(ResToUser(res)) != (uintptr_t)res) {
        abort();
    }
#endif

    return ResToUser(res);
}

// FUNCTION: CARMA2_HW 0x005277f0
void C2_HOOK_STDCALL BrResInternalFree(resource_header* res, br_boolean callback) {
    void* r;
    int original_class;

    if (res->class == BR_MEMORY_FREE) {
        return;
    }

    original_class = res->class;
    r = ResToUser(res);
    res->class = BR_MEMORY_FREE;
    if (callback) {
        if (fw.resource_class_index[original_class]->free_cb != NULL) {
            fw.resource_class_index[original_class]->free_cb(r, original_class, RESOURCE_SIZE(res));
        }
    }

    while (res->children.head != NULL) {
        r = BrSimpleRemove(res->children.head);
        BrResInternalFree((resource_header*)r, 1);
    }
    if (res->node.prev != NULL) {
        BrSimpleRemove(&res->node);
    }
    res->magic_num = 1;
    res->magic_ptr = NULL;
    BrMemFree(res);
}

// FUNCTION: CARMA2_HW 0x005277c0
void C2_HOOK_CDECL BrResFree(void* vres) {

    BrResInternalFree(UserToRes(vres), 1);
}

// FUNCTION: CARMA2_HW 0x005278b0
void C2_HOOK_CDECL BrResFreeNoCallback(void* vres) {

    BrResInternalFree(UserToRes(vres), 0);
}

// FUNCTION: CARMA2_HW 0x005278e0
void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres) {
    resource_header* res;
    resource_header* parent;

    res = UserToRes(vres);
    parent = UserToRes(vparent);

    if (res->node.prev != NULL) {
        BrSimpleRemove(&res->node);
    }
    BrSimpleAddHead(&parent->children, &res->node);
    return vres;
}

// FUNCTION: CARMA2_HW 0x00527940
void* C2_HOOK_CDECL BrResRemove(void* vres) {
    resource_header* res;

    res = UserToRes(vres);
    BrSimpleRemove(&res->node);
    return vres;
}

// FUNCTION: CARMA2_HW 0x00527970
br_uint_8 C2_HOOK_CDECL BrResClass(void* vres) {
    resource_header* res;

    res = UserToRes(vres);
    return res->class;
}

// FUNCTION: CARMA2_HW 0x00527990
br_boolean C2_HOOK_CDECL BrResIsChild(void* vparent, void* vchild) {
    resource_header* parent;
    resource_header* child;
    resource_header* cp;

    parent = UserToRes(vparent);
    child = UserToRes(vchild);

    for (cp = (resource_header*)parent->children.head; cp != NULL; cp = (resource_header*)cp->node.next) {
        if (cp == child) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005279f0
br_uint_32 C2_HOOK_CDECL BrResSize(void* vres) {
    resource_header* res;

    C2_HOOK_BUG_ON(sizeof(resource_header) != 0x18);

    res = UserToRes(vres);
    return RESOURCE_SIZE(res) - sizeof(resource_header);
}

// FUNCTION: CARMA2_HW 0x00527ad0
br_uint_32 C2_NAKED C2_HOOK_CDECL ResSizeTotal(void* vres, br_uint_32* ptotal) {
#ifndef CARPOCALYPSE2_MATCHING

    *ptotal += BrResSize(vres);
    BrResChildEnum(vres, (br_resenum_cbfn*)ResSizeTotal, ptotal);
    return 0;
#else
    __asm {
    L00527ad0:
            push ebx
            push esi
            mov esi, dword ptr [esp + 0xc]
            push edi
            mov eax, esi
            mov dl, byte ptr [esi - 1]
            lea ecx, [esi - 1]
            test dl, dl
            jne L00527aef
    L00527ae3:
            mov eax, ecx
            mov bl, byte ptr [eax - 1]
            lea ecx, [eax - 1]
            test bl, bl
            je L00527ae3
    L00527aef:
            xor ecx, ecx
            xor ebx, ebx
            mov cl, byte ptr [eax - 0xb]
            mov bl, byte ptr [eax - 0xc]
            mov ch, byte ptr [eax - 0xa]
            mov edi, dword ptr [esp + 0x14]
            shl ecx, 8
            or ecx, ebx
            mov eax, esi
            sub ecx, 6
            shl ecx, 4
            add edi, ecx
            test dl, dl
            jne L00527b1b
    L00527b13:
            mov cl, byte ptr [eax - 2]
            dec eax
            test cl, cl
            je L00527b13
    L00527b1b:
            mov esi, dword ptr [eax - 0x10]
            test esi, esi
            je L00527b57
    L00527b22:
            xor edx, edx
            mov dl, byte ptr [esi + 0xf]
            mov eax, dword ptr [edx*4 + fw+0x18]
            mov eax, dword ptr [eax + 0x10]
            test eax, eax
            jg L00527b3a
            mov eax, 4
    L00527b3a:
            lea ecx, [eax + esi + 0x17]
            dec eax
            not eax
            and ecx, eax
            push edi
            push ecx
            call L00527ad0
            add esp, 8
            test eax, eax
            jne L00527b57
            mov esi, dword ptr [esi]
            test esi, esi
            jne L00527b22
    L00527b57:
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00527a30
br_uint_32 C2_NAKED C2_HOOK_CDECL BrResSizeTotal(void* vres) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 total;

    total = 0;
    ResSizeTotal(vres, &total);
    return total;
#else
    __asm {
            push ecx
            push ebx
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            mov dword ptr [esp + 0xc], 0
            mov eax, esi
            mov dl, byte ptr [esi - 1]
            lea ecx, [esi - 1]
            test dl, dl
            jne L00527a58
    L00527a4c:
            mov eax, ecx
            mov bl, byte ptr [eax - 1]
            lea ecx, [eax - 1]
            test bl, bl
            je L00527a4c
    L00527a58:
            xor ecx, ecx
            xor ebx, ebx
            mov cl, byte ptr [eax - 0xb]
            mov bl, byte ptr [eax - 0xc]
            mov ch, byte ptr [eax - 0xa]
            mov eax, esi
            shl ecx, 8
            or ecx, ebx
            sub ecx, 6
            shl ecx, 4
            test dl, dl
            lea edi, [esp + ecx + 0xc]
            jne L00527a82
    L00527a7a:
            mov cl, byte ptr [eax - 2]
            dec eax
            test cl, cl
            je L00527a7a
    L00527a82:
            mov esi, dword ptr [eax - 0x10]
            test esi, esi
            je L00527abe
    L00527a89:
            xor edx, edx
            mov dl, byte ptr [esi + 0xf]
            mov eax, dword ptr [edx*4 + fw+0x18]
            mov eax, dword ptr [eax + 0x10]
            test eax, eax
            jg L00527aa1
            mov eax, 4
    L00527aa1:
            lea ecx, [eax + esi + 0x17]
            dec eax
            not eax
            and ecx, eax
            push edi
            push ecx
            call ResSizeTotal
            add esp, 8
            test eax, eax
            jne L00527abe
            mov esi, dword ptr [esi]
            test esi, esi
            jne L00527a89
    L00527abe:
            mov eax, dword ptr [esp + 0xc]
            pop edi
            pop esi
            pop ebx
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00527b60
br_uint_32 C2_HOOK_CDECL BrResChildEnum(void* vres, br_resenum_cbfn* callback, void* arg) {
    resource_header* res;
    resource_header* rp;
    br_uint_32 r;

    res = UserToRes(vres);
    r = 0;
    for (rp = (resource_header*)res->children.head; rp != NULL; rp = (resource_header*)rp->node.next) {
        r = callback(ResToUser(rp), arg);
        if (r != 0) {
            break;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x00527bd0
br_uint_32 C2_HOOK_CDECL BrResCheck(void* vres, int no_tag) {
    resource_header* res;

    res = UserToRes(vres);
    if ((res->magic_ptr == res) && (res->magic_num == 0xdeadbeef)) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00527c10
char* C2_HOOK_CDECL BrResStrDup(void* vparent, const char* str) {
    int l;
    char* nstr;

    l = BrStrLen(str);
    nstr = (char*)BrResAllocate(vparent, l + 1, BR_MEMORY_STRING);
    BrStrCpy(nstr, str);
    return nstr;
}

void C2_HOOK_CDECL InternalResourceDump(resource_header* res, br_putline_cbfn* putline, void* arg, int level) {
    int i;
    char* cp;
    resource_header* child;
    br_resource_class* rclass;

    rclass = fw.resource_class_index[res->class];
    cp = BrScratchString();
    for (i = 0; i < level; i++) {
        *cp = ' ';
        cp++;
    }
    sprintf(cp, "%p [%s] size=%u", res, rclass->identifier, RESOURCE_SIZE(res));
    putline(BrScratchString(), arg);
    for (child = (resource_header*)res->children.head; child != NULL; child = (resource_header*)child->node.next) {
        InternalResourceDump(child, putline, arg, level + 1);
    }
}

// FUNCTION: CARMA2_HW 0x00527c50
void C2_HOOK_CDECL BrResDump(void* vres, br_putline_cbfn* putline, void* arg) {
    resource_header* res;

    (void) res;
#ifdef BR_FIX_BUGS
    res = UserToRes(vres);
    InternalResourceDump(res, putline, arg, 0);
#endif
}

// FUNCTION: CARMA2_HW 0x00527c60
C2_NAKED char* C2_HOOK_CDECL BrResClassIdentifier(br_uint_8 res_class) {
#ifndef CARPOCALYPSE2_MATCHING
    br_resource_class* rclass;

    rclass = fw.resource_class_index[res_class];
    if (rclass == NULL) {
        return "<NULL>";
    }
    return rclass->identifier;
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            and eax, 0xff
            mov eax, dword ptr [eax*4 + fw+0x18]
            test eax, eax
            je L00527c78
            mov eax, dword ptr [eax + 4]
            ret
    L00527c78:
            mov eax, offset _BrDefaultOrderTable+0x48
            ret
    }
#endif
}
