#include "devlist.h"

#include "fwsetup.h"
#include "image.h"
#include "pattern.h"
#include "resource.h"

#include "core/std/brstdlib.h"

#include "core/v1db/sys_conf.h"

br_error C2_HOOK_CDECL AddRequestedDrivers(void) {
    char devstr[256];

    if (!fw.bAlreadyLoadedDrivers) {
        BrSystemConfigQueryString(BRT_BRENDER_DRIVERS_STR, devstr, sizeof(devstr));
        if (devstr[0] != '\0') {
            BrDevAddConfig(devstr);
        }
        fw.bAlreadyLoadedDrivers = 1;
    }
    return 0;
}

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrImageDereference();
extern br_boolean C2_HOOK_CDECL BrNamePatternMatch();
extern br_error C2_HOOK_CDECL BrSystemConfigQueryString();
extern br_framework_state fw;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrResAllocate();
extern void C2_HOOK_CDECL BrResFree();
#endif
// FUNCTION: CARMA2_HW 0x00528490
br_error C2_NAKED C2_HOOK_STDCALL devAdd(br_device** pdev, br_device_begin_fn* dev_begin, const char* args, br_image* image) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_device* dev;
    br_open_device* new_slots;

    dev = dev_begin(args);
    if (dev == NULL) {
        return 0x1002;
    }
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev == NULL) {
            fw.dev_slots[i].dev = dev;
            fw.dev_slots[i].image = image;
            if (pdev != NULL) {
                *pdev = dev;
            }
            return 0;
        }
    }
    new_slots = BrResAllocate(dev, (fw.ndev_slots + 16) * sizeof(br_open_device), BR_MEMORY_DRIVER);
    if (new_slots == NULL) {
        dev->dispatch->_free((br_object*)dev);
        /* FIXME: also do BrImageDereference? */
        return 0x1003;
    }
    for (i = 0; i < fw.ndev_slots; i++) {
        new_slots[i] = fw.dev_slots[i];
    }
    BrResFree(fw.dev_slots);
    fw.dev_slots = new_slots;
    fw.dev_slots[fw.ndev_slots].dev = dev;
    fw.dev_slots[fw.ndev_slots].image = image;
    fw.ndev_slots += 16;
    if (pdev != NULL) {
        *pdev = dev;
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push esi
            push edi
            push eax
            call dword ptr [esp + 0x14]
            mov edi, eax
            add esp, 4
            test edi, edi
            jne L005284ae
            mov eax, 0x1002
            pop edi
            pop esi
            ret 0x10
    L005284ae:
            mov edx, dword ptr [fw+0x464]
            xor eax, eax
            test edx, edx
            jle L005284d3
            mov esi, dword ptr [fw+0x460]
            mov ecx, esi
    L005284c2:
            cmp dword ptr [ecx], 0
            je L00528577
            inc eax
            add ecx, 8
            cmp eax, edx
            jl L005284c2
    L005284d3:
            lea eax, [edx*8 + 0x80]
            push 0x24
            push eax
            push edi
            call BrResAllocate
            mov esi, eax
            add esp, 0xc
            test esi, esi
            jne L005284ff
            mov ecx, dword ptr [edi]
            push edi
            call dword ptr [ecx + 0x10]
            add esp, 4
            mov eax, 0x1003
            pop edi
            pop esi
            ret 0x10
    L005284ff:
            mov ecx, dword ptr [fw+0x464]
            xor eax, eax
            test ecx, ecx
            jle L0052852c
    L0052850b:
            mov edx, dword ptr [fw+0x460]
            lea ecx, [edx + eax*8]
            inc eax
            mov edx, dword ptr [ecx]
            mov dword ptr [esi + eax*8 - 8], edx
            mov ecx, dword ptr [ecx + 4]
            mov dword ptr [esi + eax*8 - 4], ecx
            mov ecx, dword ptr [fw+0x464]
            cmp eax, ecx
            jl L0052850b
    L0052852c:
            mov edx, dword ptr [fw+0x460]
            push edx
            call BrResFree
            mov eax, dword ptr [fw+0x464]
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [fw+0x460], esi
            add esp, 4
            mov dword ptr [esi + eax*8], edi
            mov ecx, dword ptr [fw+0x464]
            mov eax, dword ptr [esp + 0xc]
            mov dword ptr [esi + ecx*8 + 4], edx
            mov edx, dword ptr [fw+0x464]
            add edx, 0x10
            test eax, eax
            mov dword ptr [fw+0x464], edx
            je L00528570
            mov dword ptr [eax], edi
    L00528570:
            xor eax, eax
            pop edi
            pop esi
            ret 0x10
    L00528577:
            mov ecx, dword ptr [esp + 0x18]
            shl eax, 3
            mov dword ptr [esi + eax], edi
            mov edx, dword ptr [fw+0x460]
            mov dword ptr [edx + eax + 4], ecx
            mov eax, dword ptr [esp + 0xc]
            test eax, eax
            je L00528570
            mov dword ptr [eax], edi
            pop edi
            pop esi
            xor eax, eax
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00528430
br_error C2_HOOK_CDECL BrDevAdd(br_device** pdev, const char* image, const char* args) {
    br_image* dev_image;
    br_device_begin_fn* dev_begin;

    dev_image = BrImageReference(image);
    if (dev_image == NULL) {
        return 0x1002;
    }
    dev_begin = BrImageLookupName(dev_image, "BrDrv1Begin", 0);
    if (dev_begin == NULL) {
        BrImageDereference(dev_image);
        return 0x1002;
    }
    return devAdd(pdev, dev_begin, args, dev_image);
}

// FUNCTION: CARMA2_HW 0x005285a0
br_error C2_HOOK_CDECL BrDevAddStatic(br_device** pdev, br_device_begin_fn* dev_begin, const char* args) {
    return devAdd(pdev, dev_begin, args, NULL);
}

// FUNCTION: CARMA2_HW 0x005285c0
br_error C2_HOOK_CDECL BrDevCheckAdd(br_device** pdev, const char* name, const char* args) {

    if (BrDevFind(pdev, name) == 0) {
        return 0;
    }
    return BrDevAdd(pdev, name, args);
}

// FUNCTION: CARMA2_HW 0x005286d0
br_error C2_HOOK_CDECL BrDevAddConfig(const char* config) {
    const char* end;
    char* dev;
    char* arg;
    char tmp[512];
    int n;

    while (1) {
        for (end = config; *end != '\0' && *end != ';'; end++) {
        }
        n = end - config;
        if (n != 0) {
            BrMemCpy(tmp, config, n);
            tmp[n] = '\0';
            dev = tmp;
            for (arg = tmp; *arg != '\0' && *arg != ',' && *arg != ':'; arg++) {
            }
            if (*arg != '\0') {
                *arg++ = '\0';
            }
            while (BrIsSpace(*dev)) {
                dev++;
            }
            n = BrStrLen(dev);
            if (n != 0) {
                while (BrIsSpace(dev[n - 1])) {
                    dev[n - 1] = '\0';
                    n--;
                }
                BrDevAdd(NULL, dev, arg);
            }
        }
        if (*end == ';') {
            config = end + 1;
        } else {
            break;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528800
br_error C2_NAKED C2_HOOK_CDECL BrDevRemove(br_device* dev) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev == dev) {
            fw.dev_slots[i].dev->dispatch->_free((br_object*)fw.dev_slots[i].dev);
            if (fw.dev_slots[i].image != NULL) {
                BrImageDereference(fw.dev_slots[i].image);
            }
            fw.dev_slots[i].dev = NULL;
            fw.dev_slots[i].image = NULL;
        }
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [fw+0x464]
            push ebx
            push edi
            xor ebx, ebx
            xor edi, edi
            cmp eax, ebx
            jle L0052885b
            push esi
            mov esi, dword ptr [esp + 0x10]
    L00528814:
            mov eax, dword ptr [fw+0x460]
            cmp dword ptr [eax + edi*8], esi
            jne L00528850
            mov ecx, dword ptr [esi]
            push esi
            call dword ptr [ecx + 0x10]
            mov edx, dword ptr [fw+0x460]
            add esp, 4
            mov eax, dword ptr [edx + edi*8 + 4]
            cmp eax, ebx
            je L0052883e
            push eax
            call BrImageDereference
            add esp, 4
    L0052883e:
            mov eax, dword ptr [fw+0x460]
            mov dword ptr [eax + edi*8], ebx
            mov ecx, dword ptr [fw+0x460]
            mov dword ptr [ecx + edi*8 + 4], ebx
    L00528850:
            mov eax, dword ptr [fw+0x464]
            inc edi
            cmp edi, eax
            jl L00528814
            pop esi
    L0052885b:
            pop edi
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00528860
br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, const char* pattern) {
    int i;

    AddRequestedDrivers();

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, fw.dev_slots[i].dev->dispatch->_identifier((br_object*)fw.dev_slots[i].dev))) {
                if (pdev != NULL) {
                    *pdev = fw.dev_slots[i].dev;
                }
                return 0;
            }
        }
    }
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00528930
br_error C2_NAKED C2_HOOK_CDECL BrDevFindMany(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, const char* pattern) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int c;

    AddRequestedDrivers();

    c = 0;
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, fw.dev_slots[i].dev->dispatch->_identifier((br_object*)fw.dev_slots[i].dev))) {
                if (c >= max_devices) {
                    return 0x1004;
                }
                /* Order of operations differs from OG */
                devices[c] = fw.dev_slots[i].dev;
                c++;
            }
        }
    }
    if (ndevices != NULL) {
        *ndevices = c;
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [fw+0x46c]
            sub esp, 0x100
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L00528979
            lea eax, [esp + 0x10]
            push 0x100
            push eax
            push 0x1a4
            call BrSystemConfigQueryString
            mov al, byte ptr [esp + 0x1c]
            add esp, 0xc
            test al, al
            je L0052896f
            lea ecx, [esp + 0x10]
            push ecx
            call BrDevAddConfig
            add esp, 4
    L0052896f:
            mov dword ptr [fw+0x46c], 1
    L00528979:
            mov eax, dword ptr [fw+0x464]
            xor esi, esi
            xor ebx, ebx
            test eax, eax
            jle L005289db
            mov edi, dword ptr [esp + 0x114]
            mov ebp, dword ptr [esp + 0x11c]
    L00528994:
            mov edx, dword ptr [fw+0x460]
            mov eax, dword ptr [edx + esi*8]
            test eax, eax
            je L005289d1
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x14]
            mov edx, dword ptr [esp + 0x124]
            add esp, 4
            push eax
            push edx
            call BrNamePatternMatch
            add esp, 8
            test eax, eax
            je L005289d1
            inc ebx
            add edi, 4
            cmp ebx, ebp
            jge L005289f5
            mov eax, dword ptr [fw+0x460]
            mov ecx, dword ptr [eax + esi*8]
            mov dword ptr [edi], ecx
    L005289d1:
            mov eax, dword ptr [fw+0x464]
            inc esi
            cmp esi, eax
            jl L00528994
    L005289db:
            mov eax, dword ptr [esp + 0x118]
            test eax, eax
            je L005289e8
            mov dword ptr [eax], ebx
    L005289e8:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x100
            ret
    L005289f5:
            pop edi
            pop esi
            pop ebp
            mov eax, 0x1004
            pop ebx
            add esp, 0x100
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00528a10
br_error C2_HOOK_CDECL BrDevCount(br_int_32* ndevices, const char* pattern) {
    int i;
    int c;

    AddRequestedDrivers();

    c = 0;
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, fw.dev_slots[i].dev->dispatch->_identifier((br_object*)fw.dev_slots[i].dev))) {
                c++;
            }
        }
    }
    if (ndevices != NULL) {
        *ndevices = c;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528ac0
br_error C2_HOOK_CDECL BrDevContainedFind(br_object** ph, br_token type, const char* pattern, br_token_value* tv) {
    int i;

    AddRequestedDrivers();

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (fw.dev_slots[i].dev->dispatch->_find((br_object_container*)fw.dev_slots[i].dev, ph, type, pattern, tv) == 0) {
                return 0;
            }
        }
    }
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00528b90
br_error C2_HOOK_CDECL BrDevContainedFindMany(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, const char* pattern, br_token_value* tv) {
    int i;
    br_int_32 n;
    br_int_32 total;
    br_error r;

    total = 0;

    AddRequestedDrivers();

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            r = fw.dev_slots[i].dev->dispatch->_findMany((br_object_container*)fw.dev_slots[i].dev, objects, max_objects, &n, type, pattern, tv);
            if (r != 0) {
                return r;
            }
            objects += n;
            max_objects -= n;
            total += n;
        }
    }
    if (pnum_objects != NULL) {
        *pnum_objects = total;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528c80
br_error C2_NAKED C2_HOOK_CDECL BrDevContainedCount(br_int_32* pcount, br_token type, const char* pattern, br_token_value* tv) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_uint_32 n;
    br_int_32 total;
    br_error r;

    AddRequestedDrivers();

    total = 0;
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            r = fw.dev_slots[i].dev->dispatch->_count((br_object_container*)fw.dev_slots[i].dev, &n, type, pattern, tv);
            if (r != 0) {
                return r;
            }
            total += n;
        }
    }
    if (pcount != NULL) {
        *pcount = total;
    }
    return 0;
#else
    __asm {
            sub esp, 0x108
            mov eax, dword ptr [fw+0x46c]
            push ebx
            push ebp
            push esi
            xor esi, esi
            push edi
            cmp eax, esi
            mov dword ptr [esp + 0x10], esi
            jne L00528ccf
            lea eax, [esp + 0x18]
            push 0x100
            push eax
            push 0x1a4
            call BrSystemConfigQueryString
            mov al, byte ptr [esp + 0x24]
            add esp, 0xc
            test al, al
            je L00528cc5
            lea ecx, [esp + 0x18]
            push ecx
            call BrDevAddConfig
            add esp, 4
    L00528cc5:
            mov dword ptr [fw+0x46c], 1
    L00528ccf:
            cmp dword ptr [fw+0x464], esi
            jle L00528d26
            mov edi, dword ptr [esp + 0x128]
            mov ebx, dword ptr [esp + 0x124]
            mov ebp, dword ptr [esp + 0x120]
    L00528cec:
            mov edx, dword ptr [fw+0x460]
            mov eax, dword ptr [edx + esi*8]
            test eax, eax
            je L00528d1c
            mov ecx, dword ptr [eax]
            push edi
            push ebx
            lea edx, [esp + 0x1c]
            push ebp
            push edx
            push eax
            call dword ptr [ecx + 0x68]
            add esp, 0x14
            test eax, eax
            jne L00528d39
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x10]
            add ecx, eax
            mov dword ptr [esp + 0x10], ecx
    L00528d1c:
            mov eax, dword ptr [fw+0x464]
            inc esi
            cmp esi, eax
            jl L00528cec
    L00528d26:
            mov eax, dword ptr [esp + 0x11c]
            test eax, eax
            je L00528d37
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [eax], ecx
    L00528d37:
            xor eax, eax
    L00528d39:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret
    }
#endif
}
