#include "devsetup.h"

#include "devlist.h"
#include "tokenval.h"

#include "../core/std/brstdlib.h"

#include "../core/v1db/sys_conf.h"

#include <stdarg.h>
#include <stdio.h>

#include <stdio.h>
#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x006ad9ac
br_pixelmap* last_begin_screen;

#ifdef CARPOCALYPSE2_MATCHING
extern br_error C2_HOOK_CDECL BrDevAdd();
extern br_error C2_HOOK_CDECL BrDevBeginTV();
extern br_error C2_HOOK_CDECL BrDevCheckAdd();
extern br_error C2_HOOK_CDECL BrDevContainedFind();
extern br_error C2_HOOK_CDECL BrDevFind();
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern br_boolean C2_HOOK_CDECL BrIsSpace();
extern char* C2_HOOK_CDECL BrStrCpy();
extern int C2_HOOK_CDECL BrStrICmp();
extern br_size_t C2_HOOK_CDECL BrStrLen();
extern char* C2_HOOK_CDECL BrStrNCpy();
extern br_error C2_HOOK_CDECL BrStringToTokenValue();
extern br_error C2_HOOK_CDECL BrSystemConfigQueryString();
extern char Image_BRCORE1[];
#endif
// FUNCTION: CARMA2_HW 0x00528d50
br_pixelmap* C2_HOOK_CDECL BrDevLastBeginQuery(void) {

    return last_begin_screen;
}

// FUNCTION: CARMA2_HW 0x00528d60
void C2_HOOK_STDCALL BrDevLastBeginSet(br_pixelmap* pm) {

    last_begin_screen = pm;
}

// FUNCTION: CARMA2_HW 0x00528d70
br_error C2_NAKED C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, const char* setup_string, ...) {
#ifndef CARPOCALYPSE2_MATCHING
    va_list vl;
    br_uint_32 i;
    br_token_value tv[64];

    va_start(vl, setup_string);
    for (i = 0; ; i++) {
        if (i == BR_ASIZE(tv)) {
            BrFailure("Too many token value pairs");
        }
        tv[i].t = va_arg(vl, br_token);
        if (tv[i].t == BR_NULL_TOKEN) {
            break;
        }
        tv[i].v = va_arg(vl, br_value);
    }
    va_end(vl);
    tv[i].t = BR_NULL_TOKEN;
    tv[i].v.p = NULL;
    return BrDevBeginTV(ppm, setup_string, tv);
#else
    __asm {
            sub esp, 0x200
            push esi
            push edi
            lea esi, [esp + 0x214]
            xor edi, edi
    L00528d81:
            cmp edi, 0x40
            jne L00528d93
            push offset Image_BRCORE1+0xe98
            call BrFailure
            add esp, 4
    L00528d93:
            mov eax, dword ptr [esi]
            add esi, 4
            test eax, eax
            je L00528dad
            add esi, 4
            mov dword ptr [esp + edi*8 + 8], eax
            inc edi
            mov eax, dword ptr [esi - 4]
            mov dword ptr [esp + edi*8 + 4], eax
            jmp L00528d81
    L00528dad:
            mov edx, dword ptr [esp + 0x210]
            lea eax, [edi*8]
            lea ecx, [esp + 8]
            mov dword ptr [esp + eax + 8], 0
            mov dword ptr [esp + eax + 0xc], 0
            mov eax, dword ptr [esp + 0x20c]
            push ecx
            push edx
            push eax
            call BrDevBeginTV
            add esp, 0xc
            pop edi
            pop esi
            add esp, 0x200
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00528df0
br_error C2_HOOK_CDECL BrDevBegin(br_pixelmap** ppm, const char* setup_string) {

    return BrDevBeginTV(ppm, setup_string, NULL);
}

// FUNCTION: CARMA2_HW 0x00528e10
br_error C2_NAKED C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, const char* setup_string, br_token_value* setup_tv) {
#ifndef CARPOCALYPSE2_MATCHING
    char str[512];
    char* args;
    char* devname;
    char* devargs;
    char devices_str[512];
    char* device_name;
    char* device_args;
    char* next_device;
    br_token_value args_tv[64];
    br_token_value* tv;
    br_output_facility* output_facility;
    br_device* device;
    br_pixelmap* screen;
    int i;
    int n;
    br_error r;

    tv = NULL;
    if (setup_string == NULL) {
        BrSystemConfigQueryString(BRT_DEFAULT_DEVICE_STR, str, BR_ASIZE(str));
    } else {
        BrStrNCpy(str, setup_string, BR_ASIZE(str));
        str[BR_ASIZE(str) - 1] = '\0';
    }
    for (args = str; *args != '\0' && *args != ','; args++) {
    }
    if (*args != '\0') {
        *args = '\0';
        args++;
    }
    for (devname = str; BrIsSpace(*devname); devname++) {
    }
    n = BrStrLen(devname);
    /* Fix to avoid writing to negative indices */
    while (n != 0 && BrIsSpace(devname[n - 1])) {
        devname[n - 1] = '\0';
        n--;
    }
    if (n == 0 || (n == 1 && devname[0] == '*')) {
        if (*args != '\0' && BrStringToTokenValue(args_tv, sizeof(args_tv), args) == 0) {
            tv = args_tv;
        }
        if (setup_tv != NULL) {
            if (tv == NULL) {
                tv = setup_tv;
            } else {
                for (tv = setup_tv; tv->t != BR_NULL_TOKEN; tv++) {
                    for (i = 0; args_tv[i].t != BR_NULL_TOKEN; i++) {
                        if (args_tv[i].t == tv->t) {
                            break;
                        }
                    }
                    if (args_tv[i].t == BR_NULL_TOKEN) {
                        if (i == BR_ASIZE(args_tv) - 1) {
                            BrFailure("Too many token value pairs");
                        }
                        args_tv[i + 1].t = BR_NULL_TOKEN;
                        args_tv[i + 1].v.p = NULL;
                    }
                    args_tv[i] = *tv;
                }
                tv = args_tv;
            }
        }
        r = BrDevContainedFind((br_object **)&output_facility, BRT_OUTPUT_FACILITY, NULL, tv);
        if (r != 0) {
            return r;
        }
    } else {
        if (BrDevFind(&device, devname) != 0) {
            devargs = NULL;
            if (BrSystemConfigQueryString(BRT_BRENDER_DEVICES_STR, devices_str, BR_ASIZE(devices_str)) == 0) {
                for (device_name = devices_str; *device_name != '\0'; device_name = next_device) {
                    for (device_args = device_name; *device_args != '\0' && *device_args != ',' && *device_args != ';'; device_args++) {
                    }
                    if (*device_args == ',') {
                        *device_args = '\0';
                        device_args++;
                    }
                    for (next_device = device_args; *next_device != '\0' && *next_device != ';'; next_device++) {
                    }
                    if (*next_device != '\0') {
                        *next_device = '\0';
                        next_device++;
                    }
                    while (BrIsSpace(*device_name)) {
                        device_name++;
                    }
                    n = BrStrLen(device_name);
                    /* Fix to avoid writing to negative indices */
                    while (n != 0 && BrIsSpace(device_name[n - 1])) {
                        device_name[n - 1] = '\0';
                        n--;
                    }
                    if (BrStrICmp(devname, device_name) == 0) {
                        devargs = device_args;
                        break;
                    }
                }
            }

            r = BrDevAdd(&device, devname, devargs);
            if (r != 0) {
                return r;
            }
        }
        if(*args && BrStringToTokenValue(args_tv, sizeof(args_tv), args) == 0) {
            tv = args_tv;
        }
        if (setup_tv != NULL) {
            if (tv == NULL) {
                tv = setup_tv;
            } else {
                for (tv = setup_tv; tv->t != BR_NULL_TOKEN; tv++) {
                    for (i = 0; args_tv[i].t != BR_NULL_TOKEN; i++) {
                        if (args_tv[i].t == tv->t) {
                            break;
                        }
                    }
                    if (args_tv[i].t == BR_NULL_TOKEN) {
                        if (i == BR_ASIZE(args_tv) - 1) {
                            BrFailure("Too many token value pairs");
                        }
                        args_tv[i+1].t = BR_NULL_TOKEN;
                        args_tv[i+1].v.p = NULL;
                    }
                    args_tv[i] = *tv;
                }
                tv = args_tv;
            }
        }
        r = device->dispatch->_find((br_object_container*)device, (br_object**)&output_facility, BRT_OUTPUT_FACILITY, NULL, tv);
        if (r != 0) {
            return r;
        }
    }
    r = output_facility->dispatch->_pixelmapNew(output_facility, (br_device_pixelmap**)&screen, tv);
    if (r != 0) {
        return r;
    }
    BrDevLastBeginSet(screen);
    *ppm = screen;
    return 0;
#else
    __asm {
            sub esp, 0x620
            mov eax, dword ptr [esp + 0x628]
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            push 0x200
            jne L00528e49
            lea eax, [esp + 0x234]
            push eax
            push 0x1a5
            call BrSystemConfigQueryString
            add esp, 0xc
            jmp L00528e62
    L00528e49:
            lea ecx, [esp + 0x234]
            push eax
            push ecx
            call BrStrNCpy
            add esp, 0xc
            mov byte ptr [esp + 0x42f], 0
    L00528e62:
            mov cl, byte ptr [esp + 0x230]
            lea edi, [esp + 0x230]
            test cl, cl
            mov dword ptr [esp + 0x14], edi
            je L00528e8a
            mov al, cl
    L00528e7a:
            cmp al, 0x2c
            je L00528e86
            mov al, byte ptr [edi + 1]
            inc edi
            test al, al
            jne L00528e7a
    L00528e86:
            mov dword ptr [esp + 0x14], edi
    L00528e8a:
            cmp byte ptr [edi], 0
            je L00528e9e
            mov byte ptr [edi], 0
            mov cl, byte ptr [esp + 0x230]
            inc edi
            mov dword ptr [esp + 0x14], edi
    L00528e9e:
            movsx edx, cl
            lea ebp, [esp + 0x230]
            push edx
            mov dword ptr [esp + 0x24], ebp
            call BrIsSpace
            add esp, 4
            test eax, eax
            je L00528ecf
    L00528eb9:
            movsx eax, byte ptr [ebp + 1]
            inc ebp
            push eax
            call BrIsSpace
            add esp, 4
            test eax, eax
            jne L00528eb9
            mov dword ptr [esp + 0x20], ebp
    L00528ecf:
            push ebp
            call BrStrLen
            mov ebx, eax
            add esp, 4
            test ebx, ebx
            je L00528f18
            lea edi, [ebx + ebp]
            movsx ecx, byte ptr [edi - 1]
            lea esi, [edi - 1]
            push ecx
            call BrIsSpace
            add esp, 4
            test eax, eax
            je L00528f0e
    L00528ef5:
            dec ebx
            dec edi
            mov byte ptr [esi], 0
            movsx edx, byte ptr [edi - 1]
            lea esi, [edi - 1]
            push edx
            call BrIsSpace
            add esp, 4
            test eax, eax
            jne L00528ef5
    L00528f0e:
            mov esi, dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x14]
            jmp L00528f1c
    L00528f18:
            mov esi, dword ptr [esp + 0x10]
    L00528f1c:
            test ebx, ebx
            je L0052915e
            cmp ebx, 1
            jne L00528f33
            cmp byte ptr [ebp], 0x2a
            je L0052915e
    L00528f33:
            lea eax, [esp + 0x1c]
            push ebp
            push eax
            call BrDevFind
            add esp, 8
            test eax, eax
            je L0052907c
            lea ecx, [esp + 0x430]
            push 0x200
            push ecx
            push 0x1a6
            mov dword ptr [esp + 0x30], 0
            call BrSystemConfigQueryString
            add esp, 0xc
            test eax, eax
            jne L00529061
            mov al, byte ptr [esp + 0x430]
            lea esi, [esp + 0x430]
            test al, al
            mov dword ptr [esp + 0x18], esi
            je L00529061
    L00528f8d:
            mov al, byte ptr [esi]
            mov ebx, esi
            test al, al
            je L00528fa5
    L00528f95:
            cmp al, 0x2c
            je L00528fa5
            cmp al, 0x3b
            je L00528fa5
            mov al, byte ptr [ebx + 1]
            inc ebx
            test al, al
            jne L00528f95
    L00528fa5:
            cmp byte ptr [ebx], 0x2c
            jne L00528fae
            mov byte ptr [ebx], 0
            inc ebx
    L00528fae:
            mov al, byte ptr [ebx]
            mov edi, ebx
            test al, al
            je L00528fc2
    L00528fb6:
            cmp al, 0x3b
            je L00528fc2
            mov al, byte ptr [edi + 1]
            inc edi
            test al, al
            jne L00528fb6
    L00528fc2:
            cmp byte ptr [edi], 0
            je L00528fcb
            mov byte ptr [edi], 0
            inc edi
    L00528fcb:
            movsx edx, byte ptr [esi]
            push edx
            call BrIsSpace
            add esp, 4
            test eax, eax
            je L00528ff1
    L00528fdb:
            movsx eax, byte ptr [esi + 1]
            inc esi
            push eax
            call BrIsSpace
            add esp, 4
            test eax, eax
            jne L00528fdb
            mov dword ptr [esp + 0x18], esi
    L00528ff1:
            push esi
            call BrStrLen
            add esp, 4
            test eax, eax
            je L00529039
            mov ecx, dword ptr [esp + 0x18]
            lea ebp, [eax + ecx]
            movsx edx, byte ptr [ebp - 1]
            lea esi, [ebp - 1]
            push edx
            call BrIsSpace
            add esp, 4
            test eax, eax
            je L00529031
    L00529019:
            dec ebp
            mov byte ptr [esi], 0
            movsx eax, byte ptr [ebp - 1]
            lea esi, [ebp - 1]
            push eax
            call BrIsSpace
            add esp, 4
            test eax, eax
            jne L00529019
    L00529031:
            mov ebp, dword ptr [esp + 0x20]
            mov esi, dword ptr [esp + 0x18]
    L00529039:
            push esi
            push ebp
            call BrStrICmp
            add esp, 8
            test eax, eax
            je L00529059
            mov al, byte ptr [edi]
            mov esi, edi
            test al, al
            mov dword ptr [esp + 0x18], esi
            jne L00528f8d
            jmp L0052905d
    L00529059:
            mov dword ptr [esp + 0x24], ebx
    L0052905d:
            mov edi, dword ptr [esp + 0x14]
    L00529061:
            mov ecx, dword ptr [esp + 0x24]
            lea edx, [esp + 0x1c]
            push ecx
            push ebp
            push edx
            call BrDevAdd
            add esp, 0xc
            test eax, eax
            jne L00529256
    L0052907c:
            cmp byte ptr [edi], 0
            je L005290a0
            push edi
            lea eax, [esp + 0x34]
            push 0x200
            push eax
            call BrStringToTokenValue
            add esp, 0xc
            test eax, eax
            jne L005290a0
            lea ecx, [esp + 0x30]
            mov dword ptr [esp + 0x10], ecx
    L005290a0:
            mov eax, dword ptr [esp + 0x63c]
            test eax, eax
            je L00529130
            mov ecx, dword ptr [esp + 0x10]
            test ecx, ecx
            jne L005290bd
            mov dword ptr [esp + 0x10], eax
            jmp L00529130
    L005290bd:
            mov ecx, dword ptr [eax]
            xor ebx, ebx
            cmp ecx, ebx
            mov ebp, eax
            je L00529128
    L005290c7:
            mov eax, dword ptr [esp + 0x30]
            xor edx, edx
            cmp eax, ebx
            je L005290e7
            mov esi, dword ptr [ebp]
            lea ecx, [esp + 0x30]
    L005290d8:
            cmp eax, esi
            je L005290e7
            mov eax, dword ptr [ecx + 8]
            add ecx, 8
            inc edx
            cmp eax, ebx
            jne L005290d8
    L005290e7:
            lea edi, [edx*8]
            mov eax, dword ptr [esp + edi + 0x30]
            lea esi, [esp + edi + 0x30]
            cmp eax, ebx
            jne L00529115
            inc edx
            cmp edx, 0x40
            jne L0052910d
            push offset Image_BRCORE1+0xed0
            call BrFailure
            add esp, 4
    L0052910d:
            mov dword ptr [esp + edi + 0x38], ebx
            mov dword ptr [esp + edi + 0x3c], ebx
    L00529115:
            mov edx, dword ptr [ebp]
            mov eax, dword ptr [ebp + 4]
            add ebp, 8
            mov dword ptr [esi], edx
            mov dword ptr [esi + 4], eax
            cmp dword ptr [ebp], ebx
            jne L005290c7
    L00529128:
            lea ecx, [esp + 0x30]
            mov dword ptr [esp + 0x10], ecx
    L00529130:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x1c]
            push ecx
            push 0
            mov edx, dword ptr [eax]
            lea ecx, [esp + 0x30]
            push 0x27
            push ecx
            push eax
            call dword ptr [edx + 0x60]
            add esp, 0x14
            test eax, eax
            je L00529227
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x620
            ret
    L0052915e:
            cmp byte ptr [edi], 0
            je L0052917e
            push edi
            lea edx, [esp + 0x34]
            push 0x200
            push edx
            call BrStringToTokenValue
            add esp, 0xc
            test eax, eax
            jne L0052917e
            lea esi, [esp + 0x30]
    L0052917e:
            mov eax, dword ptr [esp + 0x63c]
            test eax, eax
            je L00529206
            test esi, esi
            jne L00529191
            mov esi, eax
            jmp L00529206
    L00529191:
            mov ecx, dword ptr [eax]
            xor ebx, ebx
            cmp ecx, ebx
            mov ebp, eax
            je L005291fc
    L0052919b:
            mov eax, dword ptr [esp + 0x30]
            xor edx, edx
            cmp eax, ebx
            je L005291bb
            mov esi, dword ptr [ebp]
            lea ecx, [esp + 0x30]
    L005291ac:
            cmp eax, esi
            je L005291bb
            mov eax, dword ptr [ecx + 8]
            add ecx, 8
            inc edx
            cmp eax, ebx
            jne L005291ac
    L005291bb:
            lea edi, [edx*8]
            mov eax, dword ptr [esp + edi + 0x30]
            lea esi, [esp + edi + 0x30]
            cmp eax, ebx
            jne L005291e9
            inc edx
            cmp edx, 0x40
            jne L005291e1
            push offset Image_BRCORE1+0xeb4
            call BrFailure
            add esp, 4
    L005291e1:
            mov dword ptr [esp + edi + 0x38], ebx
            mov dword ptr [esp + edi + 0x3c], ebx
    L005291e9:
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [ebp + 4]
            add ebp, 8
            mov dword ptr [esi], eax
            mov dword ptr [esi + 4], ecx
            cmp dword ptr [ebp], ebx
            jne L0052919b
    L005291fc:
            lea edx, [esp + 0x30]
            mov dword ptr [esp + 0x10], edx
            mov esi, edx
    L00529206:
            push esi
            push 0
            lea eax, [esp + 0x30]
            push 0x27
            push eax
            call BrDevContainedFind
            add esp, 0x10
            test eax, eax
            je L0052922b
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x620
            ret
    L00529227:
            mov esi, dword ptr [esp + 0x10]
    L0052922b:
            mov eax, dword ptr [esp + 0x28]
            lea edx, [esp + 0x2c]
            push esi
            push edx
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x70]
            add esp, 0xc
            test eax, eax
            jne L00529256
            mov eax, dword ptr [esp + 0x2c]
            mov ecx, dword ptr [esp + 0x634]
            mov dword ptr [last_begin_screen], eax
            mov dword ptr [ecx], eax
            xor eax, eax
    L00529256:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x620
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00529270
br_pixelmap* C2_HOOK_CDECL BrDevBeginOld(const char* setup_string) {
    br_pixelmap* s;

    if (BrDevLastBeginQuery() != NULL) {
        BrFailure("Device already active");
    }
    if (BrDevBeginTV(&s, setup_string, NULL) != 0) {
        BrFailure("Could not start driver");
    }
    return s;
}

// FUNCTION: CARMA2_HW 0x005292c0
void C2_HOOK_CDECL BrDevEndOld(void) {

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapFree(BrDevLastBeginQuery());
    }
    BrDevLastBeginSet(NULL);
}

// FUNCTION: CARMA2_HW 0x005292e0
void C2_HOOK_CDECL BrDevPaletteSetOld(br_pixelmap* pm) {

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapPaletteSet(BrDevLastBeginQuery(), pm);
    }
}

// FUNCTION: CARMA2_HW 0x00529300
void C2_HOOK_CDECL BrDevPaletteSetEntryOld(int i, br_colour colour) {

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapPaletteEntrySet(BrDevLastBeginQuery(), i, colour);
    }
}

// FUNCTION: CARMA2_HW 0x00529320
br_error C2_NAKED C2_HOOK_CDECL BrRendererFacilityFind(br_renderer_facility** prf, br_device_pixelmap* destination, br_token scalar_type) {
#ifndef CARPOCALYPSE2_MATCHING
    br_renderer_facility* renderer_facility;
    br_error r;
    br_output_facility* ot;
    char object_name[23] = "Default-Renderer-00000";
    char image_name[9] = "softrnd0";
    br_boolean scalar_is_valid;

    renderer_facility = NULL;
    scalar_is_valid = 0;
    switch (scalar_type) {
    case BRT_FIXED:
        BrStrCpy(&object_name[17], "Fixed");
        image_name[7] = 'x';
        scalar_is_valid = 1;
        break;
    case BRT_FLOAT:
        BrStrCpy(&object_name[17], "Float");
        image_name[7] = 'f';
        scalar_is_valid = 1;
        break;
    default:
        abort();
    }
    if (renderer_facility == NULL && destination != NULL) {
        destination->dispatch->_query((br_object*)destination, &renderer_facility, BRT_RENDERER_FACILITY_O);
    }
    if (renderer_facility == NULL && destination != NULL) {
        ot = NULL;
        r = destination->dispatch->_query((br_object*)destination, &ot, BRT_OUTPUT_FACILITY_O);
        if (r == 0 && ot != NULL) {
            ot->dispatch->_query((br_object*)ot, &renderer_facility, BRT_RENDERER_FACILITY_O);
        }
    }
    if (scalar_is_valid && renderer_facility == NULL) {
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, object_name, NULL);
    }
    if (renderer_facility == NULL) {
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, "Default-Renderer", NULL);
    }
    if (scalar_is_valid && renderer_facility == NULL) {
        BrDevCheckAdd(0, image_name, 0);
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, object_name, NULL);
    }
    if (renderer_facility == NULL) {
        BrDevCheckAdd(0, "softrend", 0);
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, "Default-Renderer", NULL);
    }
    if (renderer_facility == NULL) {
        return 0x1002;
    }
    *prf = renderer_facility;
    return 0;
#else
    __asm {
            sub esp, 0x28
            push esi
            push edi
            mov ecx, 5
            mov esi, offset Image_BRCORE1+0xf1c
            lea edi, [esp + 0x18]
            mov eax, dword ptr [Image_BRCORE1+0xf34]
            rep movsd
            mov ecx, dword ptr [Image_BRCORE1+0xf38]
            mov dl, byte ptr [Image_BRCORE1+0xf3c]
            movsw
            movsb
            mov dword ptr [esp + 0xc], eax
            mov eax, dword ptr [esp + 0x3c]
            xor edi, edi
            sub eax, 0xb
            mov dword ptr [esp + 8], 0
            mov dword ptr [esp + 0x10], ecx
            mov byte ptr [esp + 0x14], dl
            je L00529384
            dec eax
            jne L005293a0
            lea eax, [esp + 0x29]
            push offset Image_BRCORE1+0xf40
            push eax
            call BrStrCpy
            add esp, 8
            mov byte ptr [esp + 0x13], 0x66
            jmp L0052939b
    L00529384:
            lea ecx, [esp + 0x29]
            push offset Image_BRCORE1+0xf48
            push ecx
            call BrStrCpy
            add esp, 8
            mov byte ptr [esp + 0x13], 0x78
    L0052939b:
            mov edi, 1
    L005293a0:
            mov eax, dword ptr [esp + 8]
            mov esi, dword ptr [esp + 0x38]
            test eax, eax
            jne L005293c0
            test esi, esi
            je L005293c0
            mov edx, dword ptr [esi]
            lea eax, [esp + 8]
            push 0x6c
            push eax
            push esi
            call dword ptr [edx + 0x2c]
            add esp, 0xc
    L005293c0:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L00529400
            test esi, esi
            je L00529400
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x3c]
            push 0x5f
            push edx
            push esi
            mov dword ptr [esp + 0x48], 0
            call dword ptr [ecx + 0x2c]
            add esp, 0xc
            test eax, eax
            jne L00529400
            mov eax, dword ptr [esp + 0x3c]
            test eax, eax
            je L00529400
            mov ecx, dword ptr [eax]
            lea edx, [esp + 8]
            push 0x6c
            push edx
            push eax
            call dword ptr [ecx + 0x2c]
            add esp, 0xc
    L00529400:
            test edi, edi
            je L00529422
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L00529440
            lea eax, [esp + 0x18]
            push 0
            push eax
            lea ecx, [esp + 0x10]
            push 0x2a
            push ecx
            call BrDevContainedFind
            add esp, 0x10
    L00529422:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L00529440
            push 0
            push offset Image_BRCORE1+0xf50
            lea edx, [esp + 0x10]
            push 0x2a
            push edx
            call BrDevContainedFind
            add esp, 0x10
    L00529440:
            test edi, edi
            je L00529473
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L005294a2
            lea eax, [esp + 0xc]
            push 0
            push eax
            push 0
            call BrDevCheckAdd
            add esp, 0xc
            lea ecx, [esp + 0x18]
            lea edx, [esp + 8]
            push 0
            push ecx
            push 0x2a
            push edx
            call BrDevContainedFind
            add esp, 0x10
    L00529473:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L005294a2
            push 0
            push offset Image_BRCORE1+0xf64
            push 0
            call BrDevCheckAdd
            add esp, 0xc
            lea eax, [esp + 8]
            push 0
            push offset Image_BRCORE1+0xf70
            push 0x2a
            push eax
            call BrDevContainedFind
            add esp, 0x10
    L005294a2:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L005294b5
            mov eax, 0x1002
            pop edi
            pop esi
            add esp, 0x28
            ret
    L005294b5:
            mov ecx, dword ptr [esp + 0x34]
            pop edi
            pop esi
            mov dword ptr [ecx], eax
            xor eax, eax
            add esp, 0x28
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005294d0
br_error C2_NAKED C2_HOOK_CDECL BrPrimitiveLibraryFind(br_primitive_library** ppl, br_device_pixelmap* destination, br_token scalar_type) {
#ifndef CARPOCALYPSE2_MATCHING
    br_primitive_library* primitive_library;
    br_error r;
    br_output_facility* ot;
    char object_name[25] = "Default-Primitives-00000";
    char image_name[9] = "softprm0";
    br_boolean scalar_is_valid;

    primitive_library = NULL;
    scalar_is_valid = 0;
    switch (scalar_type) {
    case BRT_FIXED:
        BrStrCpy(&object_name[19], "Fixed");
        image_name[7] = 'x';
        scalar_is_valid = 1;
        break;
    case BRT_FLOAT:
        BrStrCpy(&object_name[19], "Float");
        image_name[7] = 'f';
        scalar_is_valid = 1;
        break;
    default:
        abort();
    }
    if (primitive_library == NULL && destination != NULL) {
        destination->dispatch->_query((br_object*)destination, &primitive_library, BRT_PRIMITIVE_LIBRARY_O);
    }
    if (primitive_library == NULL && destination != NULL) {
        ot = NULL;
        r = destination->dispatch->_query((br_object*)destination, &ot, BRT_OUTPUT_FACILITY_O);
        if (r == 0 && ot != NULL) {
            ot->dispatch->_query((br_object*)ot, &primitive_library, BRT_PRIMITIVE_LIBRARY_O);
        }
    }
    if (scalar_is_valid && primitive_library == NULL) {
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, object_name, NULL);
    }
    if (primitive_library == NULL) {
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, "Default-Primitives", NULL);
    }
    if (scalar_is_valid && primitive_library == NULL) {
        BrDevCheckAdd(0, image_name, 0);
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, object_name, NULL);
    }
    if (primitive_library == NULL) {
        BrDevCheckAdd(0, "softprim", 0);
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, "Default-Primitives", NULL);
    }
    if (primitive_library == NULL) {
        return 0x1002;
    }
    *ppl = primitive_library;
    return 0;
#else
    __asm {
            sub esp, 0x2c
            push esi
            push edi
            mov ecx, 6
            mov esi, offset Image_BRCORE1+0xf84
            lea edi, [esp + 0x18]
            mov eax, dword ptr [Image_BRCORE1+0xfa0]
            rep movsd
            mov ecx, dword ptr [Image_BRCORE1+0xfa4]
            mov dl, byte ptr [Image_BRCORE1+0xfa8]
            movsb
            mov dword ptr [esp + 0xc], eax
            mov eax, dword ptr [esp + 0x40]
            xor edi, edi
            sub eax, 0xb
            mov dword ptr [esp + 8], 0
            mov dword ptr [esp + 0x10], ecx
            mov byte ptr [esp + 0x14], dl
            je L00529532
            dec eax
            jne L0052954e
            lea eax, [esp + 0x2b]
            push offset Image_BRCORE1+0xfac
            push eax
            call BrStrCpy
            add esp, 8
            mov byte ptr [esp + 0x13], 0x66
            jmp L00529549
    L00529532:
            lea ecx, [esp + 0x2b]
            push offset Image_BRCORE1+0xfb4
            push ecx
            call BrStrCpy
            add esp, 8
            mov byte ptr [esp + 0x13], 0x78
    L00529549:
            mov edi, 1
    L0052954e:
            mov eax, dword ptr [esp + 8]
            mov esi, dword ptr [esp + 0x3c]
            test eax, eax
            jne L0052956e
            test esi, esi
            je L0052956e
            mov edx, dword ptr [esi]
            lea eax, [esp + 8]
            push 0x58
            push eax
            push esi
            call dword ptr [edx + 0x2c]
            add esp, 0xc
    L0052956e:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L005295ae
            test esi, esi
            je L005295ae
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x40]
            push 0x5f
            push edx
            push esi
            mov dword ptr [esp + 0x4c], 0
            call dword ptr [ecx + 0x2c]
            add esp, 0xc
            test eax, eax
            jne L005295ae
            mov eax, dword ptr [esp + 0x40]
            test eax, eax
            je L005295ae
            mov ecx, dword ptr [eax]
            lea edx, [esp + 8]
            push 0x58
            push edx
            push eax
            call dword ptr [ecx + 0x2c]
            add esp, 0xc
    L005295ae:
            test edi, edi
            je L005295d0
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L005295ee
            lea eax, [esp + 0x18]
            push 0
            push eax
            lea ecx, [esp + 0x10]
            push 0x34
            push ecx
            call BrDevContainedFind
            add esp, 0x10
    L005295d0:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L005295ee
            push 0
            push offset Image_BRCORE1+0xfbc
            lea edx, [esp + 0x10]
            push 0x34
            push edx
            call BrDevContainedFind
            add esp, 0x10
    L005295ee:
            test edi, edi
            je L00529621
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L00529650
            lea eax, [esp + 0xc]
            push 0
            push eax
            push 0
            call BrDevCheckAdd
            add esp, 0xc
            lea ecx, [esp + 0x18]
            lea edx, [esp + 8]
            push 0
            push ecx
            push 0x34
            push edx
            call BrDevContainedFind
            add esp, 0x10
    L00529621:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L00529650
            push 0
            push offset Image_BRCORE1+0xfd0
            push 0
            call BrDevCheckAdd
            add esp, 0xc
            lea eax, [esp + 8]
            push 0
            push offset Image_BRCORE1+0xfdc
            push 0x34
            push eax
            call BrDevContainedFind
            add esp, 0x10
    L00529650:
            mov eax, dword ptr [esp + 8]
            test eax, eax
            jne L00529663
            mov eax, 0x1002
            pop edi
            pop esi
            add esp, 0x2c
            ret
    L00529663:
            mov ecx, dword ptr [esp + 0x38]
            pop edi
            pop esi
            mov dword ptr [ecx], eax
            xor eax, eax
            add esp, 0x2c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00529680
br_error C2_NAKED C2_HOOK_CDECL BrGeometryFormatFind(br_geometry** pgf, br_renderer* renderer, br_renderer_facility* renderer_facility, br_token scalar_type, br_token format_type) {
#ifndef CARPOCALYPSE2_MATCHING
    br_error r;
    br_geometry* gf;
    char object_name[21] = "Default-Format-00000";
    char image_name[9] = "softfrm0";
    br_boolean scalar_is_valid;

    scalar_is_valid = 0;
    switch (scalar_type) {
    case BRT_FIXED:
        BrStrCpy(&object_name[15], "Fixed");
        image_name[7] = 'x';
        scalar_is_valid = 1;
        break;
    case BRT_FLOAT:
        BrStrCpy(&object_name[15], "Float");
        image_name[7] = 'f';
        scalar_is_valid = 1;
        break;
    default:
        abort();
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _query, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _find, 0x60);

    r = renderer->dispatch->_find((br_object_container*)renderer, (br_object**)&gf, format_type, NULL, NULL);
    if (r == 0) {
        *pgf = gf;
        return 0;
    }
    if (renderer_facility == NULL) {
        renderer->dispatch->_query((br_object*)renderer, (br_object**)&renderer_facility, BRT_RENDERER_FACILITY_O);
    }
    if (renderer_facility != NULL) {
        r = renderer_facility->dispatch->_find((br_object_container*)renderer_facility, (br_object**)&gf, format_type, 0, 0);
        if (r == 0) {
            *pgf = gf;
            return 0;
        }
    }
    if (scalar_is_valid) {
        r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
        if (r == 0) {
            *pgf = gf;
            return 0;
        }
    }
    r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
    if (r == 0) {
        *pgf = gf;
        return 0;
    }
    if (scalar_is_valid) {
        BrDevCheckAdd(0, image_name, 0);
        r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
        if (r == 0) {
            *pgf = gf;
            return 0;
        }
    }
    BrDevCheckAdd(0, "softform", 0);
    r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
    if (r == 0) {
        *pgf = gf;
        return 0;
    }
    return 0x1002;
#else
    __asm {
            sub esp, 0x24
            push ebx
            push esi
            push edi
            mov ecx, 5
            mov esi, offset Image_BRCORE1+0xff0
            lea edi, [esp + 0x18]
            rep movsd
            mov eax, dword ptr [Image_BRCORE1+0x1008]
            mov ecx, dword ptr [Image_BRCORE1+0x100c]
            mov dl, byte ptr [Image_BRCORE1+0x1010]
            mov dword ptr [esp + 0xc], eax
            mov eax, dword ptr [esp + 0x40]
            xor ebx, ebx
            sub eax, 0xb
            mov dword ptr [esp + 0x10], ecx
            movsb
            mov byte ptr [esp + 0x14], dl
            je L005296db
            dec eax
            jne L005296f7
            lea eax, [esp + 0x27]
            push offset Image_BRCORE1+0x1014
            push eax
            call BrStrCpy
            add esp, 8
            mov byte ptr [esp + 0x13], 0x66
            jmp L005296f2
    L005296db:
            lea ecx, [esp + 0x27]
            push offset Image_BRCORE1+0x101c
            push ecx
            call BrStrCpy
            add esp, 8
            mov byte ptr [esp + 0x13], 0x78
    L005296f2:
            mov ebx, 1
    L005296f7:
            mov edi, dword ptr [esp + 0x38]
            mov esi, dword ptr [esp + 0x44]
            push 0
            push 0
            mov edx, dword ptr [edi]
            lea eax, [esp + 0x48]
            push esi
            push eax
            push edi
            call dword ptr [edx + 0x60]
            add esp, 0x14
            test eax, eax
            jne L00529727
            mov ecx, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x40]
            mov dword ptr [ecx], edx
            pop edi
            pop esi
            pop ebx
            add esp, 0x24
            ret
    L00529727:
            mov eax, dword ptr [esp + 0x3c]
            test eax, eax
            jne L00529747
            mov eax, dword ptr [edi]
            lea ecx, [esp + 0x3c]
            push 0x6c
            push ecx
            push edi
            call dword ptr [eax + 0x2c]
            mov eax, dword ptr [esp + 0x48]
            add esp, 0xc
            test eax, eax
            je L00529771
    L00529747:
            mov edx, dword ptr [eax]
            push 0
            push 0
            lea ecx, [esp + 0x48]
            push esi
            push ecx
            push eax
            call dword ptr [edx + 0x60]
            add esp, 0x14
            test eax, eax
            jne L00529771
            mov edx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [edx], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x24
            ret
    L00529771:
            test ebx, ebx
            je L005297a1
            lea ecx, [esp + 0x18]
            push 0
            push ecx
            lea edx, [esp + 0x48]
            push esi
            push edx
            call BrDevContainedFind
            add esp, 0x10
            test eax, eax
            jne L005297a1
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [esp + 0x40]
            mov dword ptr [eax], ecx
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x24
            ret
    L005297a1:
            push 0
            push offset Image_BRCORE1+0x1024
            lea edx, [esp + 0x48]
            push esi
            push edx
            call BrDevContainedFind
            add esp, 0x10
            test eax, eax
            jne L005297cd
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [esp + 0x40]
            mov dword ptr [eax], ecx
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x24
            ret
    L005297cd:
            test ebx, ebx
            je L0052980e
            lea edx, [esp + 0xc]
            push 0
            push edx
            push 0
            call BrDevCheckAdd
            add esp, 0xc
            lea eax, [esp + 0x18]
            lea ecx, [esp + 0x40]
            push 0
            push eax
            push esi
            push ecx
            call BrDevContainedFind
            add esp, 0x10
            test eax, eax
            jne L0052980e
            mov edx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [edx], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x24
            ret
    L0052980e:
            push 0
            push offset Image_BRCORE1+0x1034
            push 0
            call BrDevCheckAdd
            add esp, 0xc
            lea ecx, [esp + 0x40]
            push 0
            push offset Image_BRCORE1+0x1040
            push esi
            push ecx
            call BrDevContainedFind
            add esp, 0x10
            test eax, eax
            jne L0052984b
            mov edx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [edx], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x24
            ret
    L0052984b:
            pop edi
            pop esi
            mov eax, 0x1002
            pop ebx
            add esp, 0x24
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
    }
#endif
}
