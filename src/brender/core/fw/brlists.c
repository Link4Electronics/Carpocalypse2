#include "brlists.h"

#include <stdio.h>

#include <assert.h>

// FUNCTION: CARMA2_HW 0x00529960
void C2_HOOK_CDECL BrNewList(br_list* list) {

    list->head = (br_node*)&list->_null;
    list->_null = NULL;
    list->tail = (br_node*)list;
}

// FUNCTION: CARMA2_HW 0x00529980
void C2_HOOK_CDECL BrAddHead(br_list* list, br_node* node) {

    assert(node != NULL);
    assert(list != NULL);
    assert(list->head != NULL);

    node->prev = (br_node*)list;
    node->next = list->head;
    node->next->prev = node;
    list->head = node;
}

// FUNCTION: CARMA2_HW 0x005299a0
void C2_HOOK_CDECL BrAddTail(br_list* list, br_node* node) {

    node->next = (br_node*)&list->_null;
    node->prev = list->tail;
    node->prev->next = node;
    list->tail = node;
}

// FUNCTION: CARMA2_HW 0x005299c0
C2_NAKED br_node* C2_HOOK_CDECL BrRemHead(br_list* list) {
#ifndef CARPOCALYPSE2_MATCHING
    br_node* n;

    n = list->head;
    if (n == (br_node*)&list->_null) {
        return NULL;
    }
    list->head = n->next;
    n->next->prev = (br_node*)&list->head;
    return n;
#else
    __asm {
            mov ecx, dword ptr [esp + 4]
            mov eax, dword ptr [ecx]
            lea edx, [ecx + 4]
            cmp eax, edx
            jne L005299d0
            xor eax, eax
            ret
    L005299d0:
            mov edx, dword ptr [eax]
            mov dword ptr [ecx], edx
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

// FUNCTION: CARMA2_HW 0x005299e0
C2_NAKED br_node* C2_HOOK_CDECL BrRemTail(br_list* list) {
#ifndef CARPOCALYPSE2_MATCHING
    br_node* n;

    n = list->tail;
    if (n == (br_node*)&list->head) {
        return NULL;
    }
    list->tail = n->prev;
    n->prev->next = (br_node*)&list->_null;
    return n;
#else
    __asm {
            mov ecx, dword ptr [esp + 4]
            mov eax, dword ptr [ecx + 8]
            cmp eax, ecx
            jne L005299ee
            xor eax, eax
            ret
    L005299ee:
            mov edx, dword ptr [eax + 4]
            mov dword ptr [ecx + 8], edx
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

// FUNCTION: CARMA2_HW 0x00529a00
void C2_NAKED C2_HOOK_CDECL BrInsert(br_list* list, br_node* here, br_node* node) {
#ifndef CARPOCALYPSE2_MATCHING

    node->prev = here;
    node->next = here->next;
    here->next->prev = node;
    here->next = node;
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [eax]
            mov dword ptr [ecx + 4], eax
            mov dword ptr [ecx], edx
            mov edx, dword ptr [eax]
            mov dword ptr [edx + 4], ecx
            mov dword ptr [eax], ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00529a20
br_node* C2_HOOK_CDECL BrRemove(br_node* node) {

    node->next->prev = node->prev;
    node->prev->next = node->next;
    return node;
}

// FUNCTION: CARMA2_HW 0x00529a40
void C2_HOOK_CDECL BrSimpleNewList(br_simple_list* list) {

    list->head = NULL;
}

// FUNCTION: CARMA2_HW 0x00529a50
void C2_NAKED C2_HOOK_CDECL BrSimpleAddHead(br_simple_list* list, br_simple_node* node) {
#ifndef CARPOCALYPSE2_MATCHING

    node->next = list->head;
    node->prev = (br_simple_node**)&list->head;
    if (list->head != NULL) {
        list->head->prev = &node->next;
    }
    list->head = node;
#else
    __asm {
            mov edx, dword ptr [esp + 4]
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [edx]
            mov dword ptr [eax + 4], edx
            test ecx, ecx
            mov dword ptr [eax], ecx
            je L00529a66
            mov dword ptr [ecx + 4], eax
    L00529a66:
            mov dword ptr [edx], eax
            ret
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

// FUNCTION: CARMA2_HW 0x00529a70
br_simple_node* C2_HOOK_CDECL BrSimpleRemHead(br_simple_list* list) {
    br_simple_node* node;

    node = list->head;
    if (node != NULL) {
        BrSimpleRemove(node);
    }
    return node;
}

// FUNCTION: CARMA2_HW 0x00529aa0
void C2_NAKED C2_HOOK_CDECL BrSimpleInsert(br_simple_list* list, br_simple_node* here, br_simple_node* node) {
#ifndef CARPOCALYPSE2_MATCHING

    node->prev = &here->next;
    node->next = here->next;
    if (here->next != NULL) {
        here->next->prev = &node->next;
    }
    here->next = node;
#else
    __asm {
            mov edx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [edx]
            mov dword ptr [eax + 4], edx
            test ecx, ecx
            mov dword ptr [eax], ecx
            je L00529ab6
            mov dword ptr [ecx + 4], eax
    L00529ab6:
            mov dword ptr [edx], eax
            ret
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

// FUNCTION: CARMA2_HW 0x00529ac0
br_simple_node* C2_HOOK_CDECL BrSimpleRemove(br_simple_node* node) {

    *node->prev = node->next;
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}
