#include "sys_conf.h"

#include "core/fw/assocarr.h"
#include "core/fw/brprintf.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/host/hostcfg.h"
#include "core/std/brstdlib.h"

// GLOBAL: CARMA2_HW 0x0066c870
const br_token valid_system_config_tokens[] = {
    BRT_BRENDER_PATH_STR,
    BRT_BRENDER_DRIVERS_STR,
    BRT_DEFAULT_DEVICE_STR,
    BRT_BRENDER_DEVICES_STR,
    BR_NULL_TOKEN,
};

br_boolean C2_HOOK_STDCALL Is_Valid_Sys_Config_Token(br_token t) {
    const br_token *tokens;
    br_boolean found;

    tokens = valid_system_config_tokens;
    found = 0;
    for (; *tokens != BR_NULL_TOKEN; ) {
        if (t == *tokens++) {
            found = 1;
            break;
        }
    }
    return found;
}

#ifdef CARPOCALYPSE2_MATCHING
extern br_associative_array* C2_HOOK_CDECL BrAssociativeArrayAllocate();
extern br_error C2_HOOK_STDCALL BrAssociativeArrayQuery(br_associative_array* pArray, br_token t, br_value* pValue);
extern br_error C2_HOOK_STDCALL BrAssociativeArraySetEntry(br_associative_array* pArray, br_token t, br_value v);
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern char* C2_HOOK_CDECL BrGetEnv();
extern br_int_32 C2_HOOK_CDECL BrSprintf();
extern char* C2_HOOK_CDECL BrStrNCpy();
extern br_error C2_HOOK_CDECL BrSystemConfigQueryString();
extern const char* C2_HOOK_CDECL HostDefaultDevice();
extern br_error C2_HOOK_CDECL HostIniQuery();
extern br_boolean C2_HOOK_CDECL HostIniSectionExists();
extern br_error C2_HOOK_CDECL HostRegistryQuery();
extern br_framework_state fw;
#endif
// FUNCTION: CARMA2_HW 0x005308b0
br_boolean C2_HOOK_STDCALL LoadIniEntry(char* ini_file, char* section_name, br_token t, char* Entry) {
    br_uint_16 size;
    char temp[255];
    br_value v;

    HostIniQuery(ini_file, section_name, Entry, temp, sizeof(temp), &size);
    if (size != 0) {
        v.cstr = temp;
        BrAssociativeArraySetEntry(fw.sys_config, t, v);
        return 1;
    }
    return 0;
}

br_error C2_HOOK_STDCALL LoadIniConfig(char* ini_file, char* section_name) {

    if (!HostIniSectionExists(ini_file, section_name)) {
        return 0x1002;
    }
    LoadIniEntry(ini_file, section_name, BRT_BRENDER_PATH_STR, "BRENDER_PATH");
    LoadIniEntry(ini_file, section_name, BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS");
    LoadIniEntry(ini_file, section_name, BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES");
    LoadIniEntry(ini_file, section_name, BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX");
    LoadIniEntry(ini_file, section_name, BRT_DEFAULT_DEVICE_STR, "BR_DEFAULT_DEVICE");
    return 0;
}

// FUNCTION: CARMA2_HW 0x00530920
br_boolean C2_HOOK_STDCALL LoadRegistryEntry(char* Reg_Path, void* hKey, br_token t, char* Entry) {
    char Temp[255];
    br_uint_16 size;
    br_value v;
    br_error r;

    r = HostRegistryQuery(hKey, Reg_Path, Entry, Temp, sizeof(Temp), &size);
    if (r != 0) {
        return 0;
    }
    v.cstr = Temp;
    BrAssociativeArraySetEntry(fw.sys_config, t, v);
    return 1;
}

br_error C2_HOOK_STDCALL LoadRegistryConfig(char* Reg_Path, void* hKey) {

    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_BRENDER_PATH_STR, "BRENDER_PATH")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_DEFAULT_DEVICE_STR, "BR_DEFAULT_DEVICE")) {
        return 0x1002;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00530990
br_error C2_HOOK_STDCALL BrSetDefaultConfig(br_token t, char* Entry) {
    char Reg_Path[255];
    int v0;
    int v1;
    int v2;
    br_value v;

    v0 = 1;
    v1 = 3;
    v2 = 0;
    BrSprintf(Reg_Path, "%s\\%d.%d.%d", "SOFTWARE\\Argonaut\\BRender", v0, v1, v2);
    if (LoadRegistryEntry(Reg_Path, NULL, t, Entry)) {
        return 0;
    }

    if (LoadIniEntry("BRender.ini", "BRender", t, Entry) != 0) {
        return 0;
    }

    v.cstr = BrGetEnv(Entry);
    if (v.cstr != NULL) {
        BrAssociativeArraySetEntry(fw.sys_config, t, v);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00530460
br_error C2_NAKED C2_HOOK_CDECL BrSystemConfigBegin(void) {
#ifndef CARPOCALYPSE2_MATCHING
    char temp[255];
    br_value v;

    fw.sys_config = BrAssociativeArrayAllocate();
    if (fw.sys_config == NULL) {
        BrFailure("System config associative array allocate failed.\n");
        return 0x1002;
    }
    BrSetDefaultConfig(BRT_BRENDER_PATH_STR, "BRENDER_PATH");
    BrSetDefaultConfig(BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS");
    BrSetDefaultConfig(BRT_BRENDER_DRIVERS_STR, "BR_DRIVERS");
    BrSetDefaultConfig(BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES");
    BrSetDefaultConfig(BRT_BRENDER_DEVICES_STR, "BR_DEVICES");
    BrSetDefaultConfig(BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX");
    BrSetDefaultConfig(BRT_DEFAULT_DEVICE_STR, "BRENDER_DEFAULT_DEVICE");
    BrSetDefaultConfig(BRT_DEFAULT_DEVICE_STR, "BR_DEFAULT_DEVICE");
    BrSystemConfigQueryString(BRT_DEFAULT_DEVICE_STR, temp, sizeof(temp));
    if (temp[0] == '\0') {
        v.cstr = HostDefaultDevice();
        BrAssociativeArraySetEntry(fw.sys_config, BRT_DEFAULT_DEVICE_STR, v);
    }
    BrSystemConfigQueryString(BRT_BRENDER_PATH_STR, temp, sizeof(temp));
    if (temp[0] == '\0') {
        v.cstr = "dat;../dat;../../dat;../../../dat;../../../../dat";
        BrAssociativeArraySetEntry(fw.sys_config, BRT_BRENDER_PATH_STR, v);
    }
    return 0;
#else
    __asm {
            sub esp, 0x304
            call BrAssociativeArrayAllocate
            test eax, eax
            mov dword ptr [fw+0x468], eax
            jne L0053048d
            push offset valid_system_config_tokens+0xf8
            call BrFailure
            add esp, 4
            mov eax, 0x1002
            add esp, 0x304
            ret
    L0053048d:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea eax, [esp + 0x214]
            push offset valid_system_config_tokens+0xd8
            push eax
            call BrSprintf
            add esp, 0x18
            lea ecx, [esp + 2]
            lea edx, [esp + 4]
            lea eax, [esp + 0x204]
            push ecx
            push 0xff
            push edx
            push offset valid_system_config_tokens+0x12c
            push eax
            push 0
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L005304ea
            mov edx, dword ptr [fw+0x468]
            lea ecx, [esp + 4]
            push ecx
            push 0x1a3
            push edx
            jmp L00530524
    L005304ea:
            push offset valid_system_config_tokens+0x12c
            push 0x1a3
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L00530529
            push offset valid_system_config_tokens+0x12c
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L00530529
            push eax
            mov eax, dword ptr [fw+0x468]
            push 0x1a3
            push eax
    L00530524:
            call BrAssociativeArraySetEntry
    L00530529:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea ecx, [esp + 0x14]
            push offset valid_system_config_tokens+0xd8
            push ecx
            call BrSprintf
            add esp, 0x18
            lea edx, [esp + 2]
            lea eax, [esp + 0x204]
            lea ecx, [esp + 4]
            push edx
            push 0xff
            push eax
            push offset valid_system_config_tokens+0x13c
            push ecx
            push 0
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L00530585
            mov eax, dword ptr [fw+0x468]
            lea edx, [esp + 0x204]
            push edx
            push 0x1a4
            push eax
            jmp L005305c0
    L00530585:
            push offset valid_system_config_tokens+0x13c
            push 0x1a4
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L005305c5
            push offset valid_system_config_tokens+0x13c
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L005305c5
            mov ecx, dword ptr [fw+0x468]
            push eax
            push 0x1a4
            push ecx
    L005305c0:
            call BrAssociativeArraySetEntry
    L005305c5:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea edx, [esp + 0x14]
            push offset valid_system_config_tokens+0xd8
            push edx
            call BrSprintf
            add esp, 0x18
            lea eax, [esp + 4]
            push offset valid_system_config_tokens+0x14c
            push 0x1a4
            push 0
            push eax
            call LoadRegistryEntry
            test eax, eax
            jne L0053063c
            push offset valid_system_config_tokens+0x14c
            push 0x1a4
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L0053063c
            push offset valid_system_config_tokens+0x14c
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L0053063c
            mov ecx, dword ptr [fw+0x468]
            push eax
            push 0x1a4
            push ecx
            call BrAssociativeArraySetEntry
    L0053063c:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea edx, [esp + 0x14]
            push offset valid_system_config_tokens+0xd8
            push edx
            call BrSprintf
            add esp, 0x18
            lea eax, [esp + 4]
            push offset valid_system_config_tokens+0x158
            push 0x1a6
            push 0
            push eax
            call LoadRegistryEntry
            test eax, eax
            jne L005306b3
            push offset valid_system_config_tokens+0x158
            push 0x1a6
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L005306b3
            push offset valid_system_config_tokens+0x158
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L005306b3
            mov ecx, dword ptr [fw+0x468]
            push eax
            push 0x1a6
            push ecx
            call BrAssociativeArraySetEntry
    L005306b3:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea edx, [esp + 0x14]
            push offset valid_system_config_tokens+0xd8
            push edx
            call BrSprintf
            add esp, 0x18
            lea eax, [esp + 4]
            push offset valid_system_config_tokens+0x168
            push 0x1a6
            push 0
            push eax
            call LoadRegistryEntry
            test eax, eax
            jne L0053072a
            push offset valid_system_config_tokens+0x168
            push 0x1a6
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L0053072a
            push offset valid_system_config_tokens+0x168
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L0053072a
            mov ecx, dword ptr [fw+0x468]
            push eax
            push 0x1a6
            push ecx
            call BrAssociativeArraySetEntry
    L0053072a:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea edx, [esp + 0x14]
            push offset valid_system_config_tokens+0xd8
            push edx
            call BrSprintf
            add esp, 0x18
            lea eax, [esp + 4]
            push offset valid_system_config_tokens+0x174
            push 0x1a5
            push 0
            push eax
            call LoadRegistryEntry
            test eax, eax
            jne L005307a1
            push offset valid_system_config_tokens+0x174
            push 0x1a5
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L005307a1
            push offset valid_system_config_tokens+0x174
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L005307a1
            mov ecx, dword ptr [fw+0x468]
            push eax
            push 0x1a5
            push ecx
            call BrAssociativeArraySetEntry
    L005307a1:
            push 0
            push 3
            push 1
            push offset valid_system_config_tokens+0xbc
            lea edx, [esp + 0x14]
            push offset valid_system_config_tokens+0xd8
            push edx
            call BrSprintf
            add esp, 0x18
            lea eax, [esp + 4]
            push offset valid_system_config_tokens+0x184
            push 0x1a5
            push 0
            push eax
            call LoadRegistryEntry
            test eax, eax
            jne L00530818
            push offset valid_system_config_tokens+0x184
            push 0x1a5
            push offset valid_system_config_tokens+0xe4
            push offset valid_system_config_tokens+0xec
            call LoadIniEntry
            test eax, eax
            jne L00530818
            push offset valid_system_config_tokens+0x184
            call BrGetEnv
            add esp, 4
            test eax, eax
            je L00530818
            mov ecx, dword ptr [fw+0x468]
            push eax
            push 0x1a5
            push ecx
            call BrAssociativeArraySetEntry
    L00530818:
            push offset valid_system_config_tokens+0x19c
            push 0x1a5
            call BrSetDefaultConfig
            lea edx, [esp + 0x104]
            push 0xff
            push edx
            push 0x1a5
            call BrSystemConfigQueryString
            mov al, byte ptr [esp + 0x110]
            add esp, 0xc
            test al, al
            jne L00530862
            call HostDefaultDevice
            push eax
            mov eax, dword ptr [fw+0x468]
            push 0x1a5
            push eax
            call BrAssociativeArraySetEntry
    L00530862:
            lea ecx, [esp + 0x104]
            push 0xff
            push ecx
            push 0x1a3
            call BrSystemConfigQueryString
            mov al, byte ptr [esp + 0x110]
            add esp, 0xc
            test al, al
            jne L0053089d
            mov edx, dword ptr [fw+0x468]
            push offset valid_system_config_tokens+0x1b0
            push 0x1a3
            push edx
            call BrAssociativeArraySetEntry
    L0053089d:
            xor eax, eax
            add esp, 0x304
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00530a90
br_error C2_NAKED C2_HOOK_CDECL BrSystemConfigLoad(br_token t, char* Param1, void* Param2) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (t) {
    case BRT_REGISTRY_STR:
        return LoadRegistryConfig(Param1, Param2);
        break;
    case BRT_INI_STR:
        return LoadIniConfig(Param1, Param2);
        break;
    default:
        BrFailure("Invalid system config load token. Must be REGISTRY or INI");
        return 0x1002;
    }
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            sub esp, 0x104
            sub eax, 0x1a1
            push esi
            push edi
            je L00530c28
            dec eax
            je L00530ac5
            push offset valid_system_config_tokens+0x1e4
            call BrFailure
            add esp, 4
            mov eax, 0x1002
            pop edi
            pop esi
            add esp, 0x104
            ret
    L00530ac5:
            mov esi, dword ptr [esp + 0x118]
            mov edi, dword ptr [esp + 0x114]
            push esi
            push edi
            call HostIniSectionExists
            add esp, 8
            test eax, eax
            jne L00530aef
            mov eax, 0x1002
            pop edi
            pop esi
            add esp, 0x104
            ret
    L00530aef:
            lea eax, [esp + 0xa]
            lea ecx, [esp + 0xc]
            push eax
            push 0xff
            push ecx
            push offset valid_system_config_tokens+0x14
            push esi
            push edi
            call HostIniQuery
            add esp, 0x18
            cmp word ptr [esp + 0xa], 0
            je L00530b2a
            mov eax, dword ptr [fw+0x468]
            lea edx, [esp + 0xc]
            push edx
            push 0x1a3
            push eax
            call BrAssociativeArraySetEntry
    L00530b2a:
            lea ecx, [esp + 0xa]
            lea edx, [esp + 0xc]
            push ecx
            push 0xff
            push edx
            push offset valid_system_config_tokens+0x24
            push esi
            push edi
            call HostIniQuery
            add esp, 0x18
            cmp word ptr [esp + 0xa], 0
            je L00530b66
            mov ecx, dword ptr [fw+0x468]
            lea eax, [esp + 0xc]
            push eax
            push 0x1a4
            push ecx
            call BrAssociativeArraySetEntry
    L00530b66:
            lea edx, [esp + 0xa]
            lea eax, [esp + 0xc]
            push edx
            push 0xff
            push eax
            push offset valid_system_config_tokens+0x34
            push esi
            push edi
            call HostIniQuery
            add esp, 0x18
            cmp word ptr [esp + 0xa], 0
            je L00530ba2
            mov edx, dword ptr [fw+0x468]
            lea ecx, [esp + 0xc]
            push ecx
            push 0x1a6
            push edx
            call BrAssociativeArraySetEntry
    L00530ba2:
            lea eax, [esp + 0xa]
            lea ecx, [esp + 0xc]
            push eax
            push 0xff
            push ecx
            push offset valid_system_config_tokens+0x44
            push esi
            push edi
            call HostIniQuery
            add esp, 0x18
            cmp word ptr [esp + 0xa], 0
            je L00530bdd
            mov eax, dword ptr [fw+0x468]
            lea edx, [esp + 0xc]
            push edx
            push 0x1a5
            push eax
            call BrAssociativeArraySetEntry
    L00530bdd:
            lea ecx, [esp + 0xa]
            lea edx, [esp + 0xc]
            push ecx
            push 0xff
            push edx
            push offset valid_system_config_tokens+0x54
            push esi
            push edi
            call HostIniQuery
            add esp, 0x18
            cmp word ptr [esp + 0xa], 0
            je L00530d71
            mov ecx, dword ptr [fw+0x468]
            lea eax, [esp + 0xc]
            push eax
            push 0x1a5
            push ecx
            call BrAssociativeArraySetEntry
            xor eax, eax
            pop edi
            pop esi
            add esp, 0x104
            ret
    L00530c28:
            mov esi, dword ptr [esp + 0x114]
            mov edi, dword ptr [esp + 0x118]
            lea edx, [esp + 0xa]
            lea eax, [esp + 0xc]
            push edx
            push 0xff
            push eax
            push offset valid_system_config_tokens+0x68
            push esi
            push edi
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L00530d63
            mov edx, dword ptr [fw+0x468]
            lea ecx, [esp + 0xc]
            push ecx
            push 0x1a3
            push edx
            call BrAssociativeArraySetEntry
            lea eax, [esp + 0xa]
            lea ecx, [esp + 0xc]
            push eax
            push 0xff
            push ecx
            push offset valid_system_config_tokens+0x78
            push esi
            push edi
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L00530d63
            mov eax, dword ptr [fw+0x468]
            lea edx, [esp + 0xc]
            push edx
            push 0x1a4
            push eax
            call BrAssociativeArraySetEntry
            lea ecx, [esp + 0xa]
            lea edx, [esp + 0xc]
            push ecx
            push 0xff
            push edx
            push offset valid_system_config_tokens+0x88
            push esi
            push edi
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L00530d63
            mov ecx, dword ptr [fw+0x468]
            lea eax, [esp + 0xc]
            push eax
            push 0x1a6
            push ecx
            call BrAssociativeArraySetEntry
            lea edx, [esp + 0xa]
            lea eax, [esp + 0xc]
            push edx
            push 0xff
            push eax
            push offset valid_system_config_tokens+0x98
            push esi
            push edi
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L00530d63
            mov edx, dword ptr [fw+0x468]
            lea ecx, [esp + 0xc]
            push ecx
            push 0x1a5
            push edx
            call BrAssociativeArraySetEntry
            lea eax, [esp + 0xa]
            lea ecx, [esp + 0xc]
            push eax
            push 0xff
            push ecx
            push offset valid_system_config_tokens+0xa8
            push esi
            push edi
            call HostRegistryQuery
            add esp, 0x18
            test eax, eax
            jne L00530d63
            mov eax, dword ptr [fw+0x468]
            lea edx, [esp + 0xc]
            push edx
            push 0x1a5
            push eax
            call BrAssociativeArraySetEntry
            xor eax, eax
            pop edi
            pop esi
            add esp, 0x104
            ret
    L00530d63:
            mov eax, 0x1002
            pop edi
            pop esi
            add esp, 0x104
            ret
    L00530d71:
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x104
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00530d80
br_error C2_NAKED C2_HOOK_CDECL BrSystemConfigSetString(br_token t, const char* string) {
#ifndef CARPOCALYPSE2_MATCHING
    br_value v;

    if (!Is_Valid_Sys_Config_Token(t)) {
        BrFailure("Not a valid system configuration token.\n");
        return 0x1002;
    }
    v.cstr = string;
    return BrAssociativeArraySetEntry(fw.sys_config, t, v);
#else
    __asm {
            mov eax, dword ptr [valid_system_config_tokens]
            mov ecx, dword ptr [esp + 4]
            xor edx, edx
            push esi
            test eax, eax
            je L00530daa
            mov eax, offset valid_system_config_tokens
    L00530d95:
            mov esi, dword ptr [eax]
            add eax, 4
            cmp ecx, esi
            je L00530da5
            cmp dword ptr [eax], 0
            jne L00530d95
            jmp L00530daa
    L00530da5:
            mov edx, 1
    L00530daa:
            test edx, edx
            jne L00530dc2
            push offset valid_system_config_tokens+0x220
            call BrFailure
            add esp, 4
            mov eax, 0x1002
            pop esi
            ret
    L00530dc2:
            mov eax, dword ptr [esp + 0xc]
            push eax
            push ecx
            mov ecx, dword ptr [fw+0x468]
            push ecx
            call BrAssociativeArraySetEntry
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00530de0
br_error C2_NAKED C2_HOOK_CDECL BrSystemConfigQueryString(br_token t, char* string, int max_size) {
#ifndef CARPOCALYPSE2_MATCHING
    br_error r;
    br_value v;

    if (!Is_Valid_Sys_Config_Token(t)) {
        BrFailure("Not a valid system configuration token.\n");
        return 0x1002;
    }
    r = BrAssociativeArrayQuery(fw.sys_config, t, &v);
    if (r == 0) {
        if (v.cstr != NULL) {
            BrStrNCpy(string, v.cstr, max_size);
            return 0;
        } else {
            string[0] = '\0';
            return 0;
        }
    } else {
        string[0] = '\0';
        return r;
    }
#else
    __asm {
            mov eax, dword ptr [valid_system_config_tokens]
            mov ecx, dword ptr [esp + 4]
            xor edx, edx
            push esi
            test eax, eax
            je L00530e0a
            mov eax, offset valid_system_config_tokens
    L00530df5:
            mov esi, dword ptr [eax]
            add eax, 4
            cmp ecx, esi
            je L00530e05
            cmp dword ptr [eax], 0
            jne L00530df5
            jmp L00530e0a
    L00530e05:
            mov edx, 1
    L00530e0a:
            test edx, edx
            jne L00530e22
            push offset valid_system_config_tokens+0x24c
            call BrFailure
            add esp, 4
            mov eax, 0x1002
            pop esi
            ret
    L00530e22:
            lea eax, [esp + 8]
            push eax
            push ecx
            mov ecx, dword ptr [fw+0x468]
            push ecx
            call BrAssociativeArrayQuery
            mov esi, eax
            test esi, esi
            jne L00530e64
            mov eax, dword ptr [esp + 8]
            test eax, eax
            je L00530e59
            mov edx, dword ptr [esp + 0x10]
            push edx
            push eax
            mov eax, dword ptr [esp + 0x14]
            push eax
            call BrStrNCpy
            add esp, 0xc
            mov eax, esi
            pop esi
            ret
    L00530e59:
            mov ecx, dword ptr [esp + 0xc]
            mov eax, esi
            mov byte ptr [ecx], 0
            pop esi
            ret
    L00530e64:
            mov edx, dword ptr [esp + 0xc]
            mov eax, esi
            pop esi
            mov byte ptr [edx], 0
            ret
    }
#endif
}
