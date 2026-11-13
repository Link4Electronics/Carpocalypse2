#include "prepmap.h"

#include "dbsetup.h"
#ifndef CARPOCALYPSE2_MATCHING
#include "../fw/brprintf.h"
#endif

#ifdef CARPOCALYPSE2_MATCHING
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x00523160
void C2_NAKED C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags) {
#ifndef CARPOCALYPSE2_MATCHING

    BrBufferUpdate(map, BRT_COLOUR_MAP_O, flags);
#else
    __asm {
            sub esp, 0x18
            push esi
            mov esi, dword ptr [v1db+0xc]
            push edi
            xor edi, edi
            mov eax, 1
            cmp esi, edi
            mov dword ptr [esp + 8], 0x6f
            mov dword ptr [esp + 0xc], edi
            mov dword ptr [esp + 0x10], 0x6e
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x18], edi
            mov dword ptr [esp + 0x1c], edi
            je L005231db
            mov ecx, dword ptr [esp + 0x28]
            test ch, 0x80
            je L005231a3
            mov dword ptr [esp + 0xc], eax
    L005231a3:
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [edx + 0x40]
            lea ecx, [edx + 0x40]
            cmp eax, edi
            je L005231c6
            mov ecx, dword ptr [eax]
            lea esi, [esp + 8]
            push esi
            push edx
            push eax
            call dword ptr [ecx + 0x44]
            add esp, 0xc
            pop edi
            pop esi
            add esp, 0x18
            ret
    L005231c6:
            mov eax, dword ptr [esi]
            lea edi, [esp + 8]
            push edi
            push edx
            push 0xa5
            push ecx
            push esi
            call dword ptr [eax + 0x78]
            add esp, 0x14
    L005231db:
            pop edi
            pop esi
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005231f0
void C2_NAKED C2_HOOK_STDCALL BrBufferUpdate(br_pixelmap* pm, br_token use, br_uint_16 flags) {
#ifndef CARPOCALYPSE2_MATCHING

    br_token_value tv[] = {
        { BRT_PREFER_SHARE_B,    { 0 } },
        { BRT_CAN_SHARE_B,       { 1 } },
        { 0 },
    };

    if (v1db.renderer == NULL) {
        return;
    }

    if (flags & BR_MAPU_SHARED) {
        tv[0].v.b = 1;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _bufferStoredNew, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_buffer_stored_dispatch , _update, 0x44);

    if (pm->stored == NULL) {
        v1db.renderer->dispatch->_bufferStoredNew(v1db.renderer, &pm->stored, use, (br_device_pixelmap*)pm, tv);
    } else {
        ((br_buffer_stored*)pm->stored)->dispatch->_update((br_buffer_stored*)pm->stored, (br_device_pixelmap*)pm, tv);
    }
#else
    __asm {
            sub esp, 0x18
            push esi
            mov esi, dword ptr [v1db+0xc]
            push edi
            xor edi, edi
            mov eax, 1
            cmp esi, edi
            mov dword ptr [esp + 8], 0x6f
            mov dword ptr [esp + 0xc], edi
            mov dword ptr [esp + 0x10], 0x6e
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x18], edi
            mov dword ptr [esp + 0x1c], edi
            je L0052326d
            mov ecx, dword ptr [esp + 0x2c]
            test ch, 0x80
            je L00523233
            mov dword ptr [esp + 0xc], eax
    L00523233:
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [edx + 0x40]
            lea ecx, [edx + 0x40]
            cmp eax, edi
            je L00523258
            mov ecx, dword ptr [eax]
            lea esi, [esp + 8]
            push esi
            push edx
            push eax
            call dword ptr [ecx + 0x44]
            add esp, 0xc
            pop edi
            pop esi
            add esp, 0x18
            ret 0xc
    L00523258:
            mov eax, dword ptr [esi]
            lea edi, [esp + 8]
            push edi
            push edx
            mov edx, dword ptr [esp + 0x30]
            push edx
            push ecx
            push esi
            call dword ptr [eax + 0x78]
            add esp, 0x14
    L0052326d:
            pop edi
            pop esi
            add esp, 0x18
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00523280
void C2_HOOK_STDCALL BrBufferClear(br_pixelmap* pm) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_buffer_stored_dispatch , _free, 0x10);

    if (pm->stored != NULL) {
        ((br_buffer_stored*)pm->stored)->dispatch->_free((br_object*)pm->stored);
        pm->stored = NULL;
    }
}
