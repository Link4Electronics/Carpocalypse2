#include "interface.h"
#include "interface-main.h"
#include "interface-network.h"
#include "globvars.h"
#include "c2_string.h"
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pButton_index);
extern void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend);
extern tFrontend_spec* gCurrent_frontend_spec;
extern tFrontend_spec gFrontend_MAIN;
extern tFrontend_brender_item gFrontend_brender_items[100];
extern tRace_list_spec gRace_list[100];
#endif
// FUNCTION: CARMA2_HW 0x004677d0
int C2_NAKED C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {

        if (pX >= gFrontend_brender_items[i].model->vertices[1].p.v[0]
                && pX <= gFrontend_brender_items[i].model->vertices[3].p.v[0]
                && pY >= -gFrontend_brender_items[i].model->vertices[0].p.v[1]
                && pY <= -gFrontend_brender_items[i].model->vertices[1].p.v[1]
                && gCurrent_frontend_spec->items[i].enabled > 0 /* default or disabled */
                && gCurrent_frontend_spec->items[i].visible) {
            return i;
                }
    }
    return -1;
#else
    __asm {
            push ecx
            push ebx
            mov ebx, dword ptr [ecx + 0x104]
            mov dword ptr [esp + 4], edx
            push esi
            xor edx, edx
            push edi
            test ebx, ebx
            jle L00467878
            fild dword ptr [esp + 0xc]
            mov eax, dword ptr [gCurrent_frontend_spec]
            mov edi, offset gFrontend_brender_items+0x4
            fstp dword ptr [esp + 0xc]
            lea ecx, [eax + 0x15c]
    L00467800:
            mov eax, dword ptr [edi]
            fld dword ptr [esp + 0xc]
            mov esi, dword ptr [eax + 8]
            fcomp dword ptr [esi + 0x28]
            fnstsw ax
            test ah, 1
            jne L00467857
            fld dword ptr [esi + 0x78]
            fld dword ptr [esp + 0xc]
            fcompp
            fnstsw ax
            test ah, 0x41
            je L00467857
            fild dword ptr [esp + 0x14]
            fld dword ptr [esi + 4]
            fchs
            fld st(1)
            fcompp
            fnstsw ax
            test ah, 1
            jne L00467855
            fld dword ptr [esi + 0x2c]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            je L00467857
            mov eax, dword ptr [ecx - 4]
            test eax, eax
            jle L00467857
            cmp dword ptr [ecx], 0
            jne L0046786f
            jmp L00467857
    L00467855:
            fstp st(0)
    L00467857:
            inc edx
            add edi, 0x2c
            add ecx, 0x158
            cmp edx, ebx
            jl L00467800
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebx
            pop ecx
            ret 4
    L0046786f:
            mov eax, edx
            pop edi
            pop esi
            pop ebx
            pop ecx
            ret 4
    L00467878:
            pop edi
            pop esi
            or eax, 0xffffffff
            pop ebx
            pop ecx
            ret 4
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

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend) {
    const tRace_list_spec* race_list_item;
    int i;

    race_list_item = &gRace_list[pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem];
    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; race_list_item++, i++) {

        strcpy(pFrontend->items[i].text, race_list_item->name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x00467a70
void C2_NAKED C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gCurrent_frontend_spec == &gFrontend_MAIN) {
        RefreshRacesScroller(pFrontend);
    }
    if (gCurrent_frontend_spec == &gFrontend_NETWORK) {
        RefreshNetRacesScroller(pFrontend);
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [gCurrent_frontend_spec]
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            cmp eax, offset gFrontend_MAIN
            push edi
            mov dword ptr [esp + 0x10], ebp
            jne L00467a8c
            call RefreshRacesScroller
    L00467a8c:
            cmp dword ptr [gCurrent_frontend_spec], 0x5b39b8
            jne L00467b21
            mov edx, dword ptr [ebp + 0x87d8]
            mov eax, dword ptr [ebp + 0x87d4]
            mov ecx, dword ptr [ebp + 0x87dc]
            sub eax, edx
            cmp edx, ecx
            jg L00467b11
            lea ecx, [eax + eax*8]
            shl ecx, 4
            add ecx, eax
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea ebx, [ecx*4 + gRace_list]
            lea eax, [eax + eax*2]
            lea ecx, [edx + eax*2]
            lea ebp, [ebp + ecx*8 + 0x189]
    L00467ad7:
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            add ebx, 0x244
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebp
            add ebp, 0x158
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            inc edx
            rep movsb
            mov ecx, dword ptr [esp + 0x10]
            cmp edx, dword ptr [ecx + 0x87dc]
            jle L00467ad7
            mov ebp, ecx
    L00467b11:
            mov ecx, ebp
            call FuckWithWidths
            xor edx, edx
            mov ecx, ebp
            call MungeButtonModels
    L00467b21:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    }
#endif
}