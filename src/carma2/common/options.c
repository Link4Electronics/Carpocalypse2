#include "options.h"

#include "errors.h"
#include "globvars.h"

#include "drmem.h"

#include "flicplay.h"

#include "font.h"

#include "graphics.h"

#include "interface-graphics.h"

#include "intrface.h"

#include "network.h"

#include "platform.h"

extern void C2_HOOK_FASTCALL LoadKeyMapping(void);
extern void C2_HOOK_FASTCALL FUN_004b3ec0(void);
extern int gUNK_0065bd80;
extern int gUNK_0065bda0;
extern int gUNK_0065bdc0;
extern int gUNK_0065bde0;
extern int gUNK_0065be00;
extern int gUNK_0065be20;
extern int gUNK_0065be40;
extern int gUNK_0065be60;
extern int gUNK_0065be80;
extern int gUNK_0065bea0;
extern int gUNK_0065bec0;
extern int gUNK_0065bee0;
extern float gUNK_0058a6e0;

#include "loading.h"
#include "utility.h"

#include "brucetrk.h"

#include "depth.h"

#include "graphics1.h"

#include "sound.h"

#include "spark.h"

#include "world.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
// (options.h contents inlined in the merged headers)
#include "loading1.h"
#include "packfile.h"
#include "globvars.h"
extern void C2_HOOK_FASTCALL LoadKeyMapping(void);
// GLOBAL: CARMA2_HW 0x006869e0
extern int gBackup_key_mappings[4][77];
extern int gKey_map_index;
extern int gKey_mapping[77];
extern int gOrig_key_map_index;

// GLOBAL: CARMA2_HW 0x00688458
char* gKey_names[153];

// GLOBAL: CARMA2_HW 0x006918d0
char* gOptions_key_names[153];

// GLOBAL: CARMA2_HW 0x0065cd70
tInterface_spec gInterface_0065cd70;

// GLOBAL: CARMA2_HW 0x0065ca90
tInterface_spec gInterface_0065ca90;

// GLOBAL: CARMA2_HW 0x0065c780
tInterface_spec gInterface_0065c780;

// GLOBAL: CARMA2_HW 0x0065c068
tInterface_spec gInterface_0065c068;

// GLOBAL: CARMA2_HW 0x00691798
int gINT_00691798[77];

// GLOBAL: CARMA2_HW 0x0069178c
int gINT_0069178c;

// GLOBAL: CARMA2_HW 0x00691b34
int gINT_00691b34;

// GLOBAL: CARMA2_HW 0x00691b38
int gINT_00691b38;

extern int gUNK_00691788;
extern tU8* gUNK_006918cc;

// GLOBAL: CARMA2_HW 0x0058a6e4
const br_scalar gFloat_0058a6e4 = 0.2f;

// GLOBAL: CARMA2_HW 0x0058a6e8
const br_scalar gFloat_0058a6e8 = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a6ec
const br_scalar gFloat_0058a6ec = 0.5f;

// GLOBAL: CARMA2_HW 0x0058a6f0
const br_scalar gFloat_0058a6f0 = 0.25f;

// GLOBAL: CARMA2_HW 0x00688440
int gOrig_key_map_index;

// GLOBAL: CARMA2_HW 0x006869e0
int gBackup_key_mappings[4][77];


#include "errors.h"
#include "globvars.h"
#include "loading1.h"
#include "utility.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMemFree();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_HOOK_FASTCALL DisposeFont(int pFont_ID);
extern int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);
extern void C2_HOOK_FASTCALL FUN_004b4070(void);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_FASTCALL GetAccessoryRendering(void);
extern int C2_HOOK_FASTCALL GetCarSimplificationLevel(void);
extern tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);
extern int C2_HOOK_FASTCALL GetDepthCueingOn(void);
extern tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void);
extern tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void);
extern int C2_HOOK_FASTCALL GetSkyTextureOn(void);
extern int C2_HOOK_FASTCALL GetSmokeOn(void);
extern int C2_HOOK_FASTCALL GetSoundDetailLevel(void);
extern tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void);
extern br_scalar C2_HOOK_STDCALL GetYon(void);
extern br_scalar C2_HOOK_STDCALL GetYonFactor(void);
extern void C2_HOOK_FASTCALL LoadFont(int pFont_ID);
extern void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there);
extern int C2_HOOK_FASTCALL PDFileUnlock(const char* pThe_path);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PreloadBunchOfFlics(int pBunch_index);
extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);
extern void C2_HOOK_FASTCALL RunFlic(int pIndex);
extern void C2_HOOK_FASTCALL StripControls(char* pStr);
extern void C2_HOOK_FASTCALL UnlockBunchOfFlics(int pBunch_index);
extern void __cdecl _ftol();
extern tPath_name gApplication_path;
extern tFrontend_spec gFrontend_QUIT;
extern char gNext_crush_actor_id[];
extern char gPed_flags_0058f2e4[];
extern tProgram_state gProgram_state;
#endif
// FUNCTION: CARMA2_HW 0x004b39f0
void C2_NAKED C2_HOOK_FASTCALL DoOptions(void) {
#ifndef CARPOCALYPSE2_MATCHING

    // Relict from Carmageddon 1, unused here
    UNUSED();
    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x208
            mov edx, offset gInterface_0065cd70+0x194
            xor ecx, ecx
            push ebx
            push ebp
            push esi
            push edi
            call PrintMemoryDump
            mov ebp, 1
            mov ecx, ebp
            call PreloadBunchOfFlics
            mov edi, dword ptr [gProgram_state+0x1bb4]
            xor ebx, ebx
            xor eax, eax
            cmp edi, ebx
            setne al
            inc eax
            xor edx, edx
            push eax
            mov ecx, offset gInterface_0065cd70
            call DoInterfaceScreen
            mov edi, 3
            cmp eax, edi
            jge L004b3ea1
            sub eax, ebx
            je L004b3e7b
            dec eax
            je L004b3cba
            dec eax
            jne L004b3ea1
            mov ecx, dword ptr [gKey_map_index]
            mov esi, offset gKey_mapping
            mov dword ptr [esp + 0x10], ecx
            mov ecx, 0x4d
            mov edi, offset gINT_00691798
            push offset gFrontend_QUIT+0xbcd8
            rep movsd
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x11c]
            mov dword ptr [esp + 0x18], ebx
            mov dword ptr [gUNK_006918cc], 0x65ca90
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x118]
            call DRfopen
            mov ebp, eax
            cmp ebp, ebx
            jne L004b3ab2
            push 0x6a
            call FatalError
            add esp, 4
    L004b3ab2:
            mov ebx, offset gOptions_key_names
    L004b3ab7:
            push ebp
            mov edx, 0x100
            lea ecx, [esp + 0x1c]
            call PFfgets
            lea ecx, [esp + 0x18]
            call StripControls
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            push 0xfd
            repne scasb
            not ecx
            push ecx
            call BrMemAllocate
            mov edx, eax
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            mov dword ptr [ebx], edx
            repne scasb
            not ecx
            sub edi, ecx
            add ebx, 4
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            cmp ebx, offset gINT_00691b34
            rep movsb
            jl L004b3ab7
            mov ecx, ebp
            call PFfclose
            mov ecx, 9
            call LoadFont
            mov ecx, 0xb
            call LoadFont
            mov ecx, 0xa
            call LoadFont
            mov ecx, 0xc
            call LoadFont
            mov dword ptr [gINT_00691b34], 0xffffffff
            mov dword ptr [gINT_0069178c], 0x12
            mov dword ptr [gINT_00691b38], 8
    L004b3b67:
            mov ecx, dword ptr [esp + 0x14]
            xor edx, edx
            test ecx, ecx
            push 0
            mov ecx, offset gInterface_0065ca90
            setne dl
            call DoInterfaceScreen
            mov esi, eax
            test esi, esi
            jge L004b3ba6
            call FadePaletteDown
            xor edx, edx
            mov ecx, 0xe5
            call NetFullScreenMessage
            call FadePaletteDown
            test esi, esi
            mov dword ptr [esp + 0x14], 1
            jl L004b3b67
    L004b3ba6:
            je L004b3bc8
            mov ecx, 0x4d
            mov esi, offset gINT_00691798
            mov edi, offset gKey_mapping
            rep movsd
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [gKey_map_index], ecx
            jmp L004b3c61
    L004b3bc8:
            push offset gFrontend_QUIT+0xbcc8
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x1c]
            call PathCat
            mov dl, byte ptr [gKey_map_index]
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            add dl, 0x30
            repne scasb
            not ecx
            lea eax, [esp + 0x18]
            dec ecx
            sub eax, 5
            mov byte ptr [ecx + eax], dl
            lea ecx, [esp + 0x18]
            call PDFileUnlock
            mov edx, offset gFrontend_QUIT+0xbcc4
            lea ecx, [esp + 0x18]
            call DRfopen
            mov esi, eax
            test esi, esi
            jne L004b3c23
            push 9
            call FatalError
            add esp, 4
    L004b3c23:
            mov edi, offset gKey_mapping
    L004b3c28:
            mov ecx, dword ptr [edi]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push esi
            call fprintf
            add esp, 0xc
            push esi
            push 0xd
            call fputc
            add esp, 8
            push esi
            push 0xa
            call fputc
            add edi, 4
            add esp, 8
            cmp edi, offset gKey_mapping+0x134
            jl L004b3c28
            mov ecx, esi
            call PFfclose
    L004b3c61:
            mov ecx, 9
            call DisposeFont
            mov ecx, 0xb
            call DisposeFont
            mov ecx, 0xa
            call DisposeFont
            mov ecx, 0xc
            call DisposeFont
            call FUN_004b4070
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L004b3ca6
            call FadePaletteDown
            mov ebp, 1
            jmp L004b3ea1
    L004b3ca6:
            mov ecx, 0xab
            call RunFlic
            mov ebp, 1
            jmp L004b3ea1
    L004b3cba:
            mov ecx, 0xc
            mov dword ptr [gUNK_006918cc], 0x65c780
            mov dword ptr [gUNK_00691788], 0xffffffff
            call LoadFont
            call GetCarSimplificationLevel
            mov esi, 2
            cmp eax, esi
            jl L004b3ce7
            dec eax
    L004b3ce7:
            mov dword ptr [gUNK_0065bd80], eax
            call GetCarTexturingLevel
            sub eax, ebx
            je L004b3d0b
            dec eax
            je L004b3d03
            dec eax
            jne L004b3d11
            mov dword ptr [gUNK_0065bda0], ebx
            jmp L004b3d11
    L004b3d03:
            mov dword ptr [gUNK_0065bda0], ebp
            jmp L004b3d11
    L004b3d0b:
            mov dword ptr [gUNK_0065bda0], esi
    L004b3d11:
            call GetShadowLevel
            sub eax, ebx
            je L004b3d2d
            dec eax
            je L004b3d25
            mov dword ptr [gUNK_0065bdc0], ebx
            jmp L004b3d33
    L004b3d25:
            mov dword ptr [gUNK_0065bdc0], ebp
            jmp L004b3d33
    L004b3d2d:
            mov dword ptr [gUNK_0065bdc0], esi
    L004b3d33:
            call GetWallTexturingLevel
            sub eax, ebx
            je L004b3d52
            dec eax
            je L004b3d4a
            dec eax
            jne L004b3d58
            mov dword ptr [gUNK_0065bde0], ebx
            jmp L004b3d58
    L004b3d4a:
            mov dword ptr [gUNK_0065bde0], ebp
            jmp L004b3d58
    L004b3d52:
            mov dword ptr [gUNK_0065bde0], esi
    L004b3d58:
            call GetRoadTexturingLevel
            sub eax, ebx
            je L004b3d6c
            dec eax
            jne L004b3d72
            mov dword ptr [gUNK_0065be00], ebx
            jmp L004b3d72
    L004b3d6c:
            mov dword ptr [gUNK_0065be00], esi
    L004b3d72:
            call GetYon
            fld dword ptr [gUNK_0058a6e0]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fsub dword ptr [esp + 0x10]
            fmul dword ptr [gFloat_0058a6e4]
            call _ftol
            cmp eax, edi
            mov dword ptr [gUNK_0065be20], eax
            jle L004b3da1
            mov dword ptr [gUNK_0065be20], edi
    L004b3da1:
            call GetYonFactor
            fcom dword ptr [gFloat_0058a6e8]
            fnstsw ax
            test ah, 1
            jne L004b3dbd
            fstp st(0)
            mov dword ptr [gUNK_0065be40], ebx
            jmp L004b3ded
    L004b3dbd:
            fcom dword ptr [gFloat_0058a6ec]
            fnstsw ax
            test ah, 1
            jne L004b3dd4
            fstp st(0)
            mov dword ptr [gUNK_0065be40], ebp
            jmp L004b3ded
    L004b3dd4:
            fcomp dword ptr [gFloat_0058a6f0]
            mov dword ptr [gUNK_0065be40], esi
            fnstsw ax
            test ah, 1
            je L004b3ded
            mov dword ptr [gUNK_0065be40], edi
    L004b3ded:
            call GetSkyTextureOn
            neg eax
            sbb eax, eax
            inc eax
            mov dword ptr [gUNK_0065be60], eax
            call GetDepthCueingOn
            neg eax
            sbb eax, eax
            inc eax
            mov dword ptr [gUNK_0065be80], eax
            call GetAccessoryRendering
            neg eax
            sbb eax, eax
            inc eax
            mov dword ptr [gUNK_0065bea0], eax
            call GetSmokeOn
            neg eax
            sbb eax, eax
            inc eax
            mov dword ptr [gUNK_0065bec0], eax
            call GetSoundDetailLevel
            sub esi, eax
            xor edx, edx
            push ebx
            mov ecx, offset gInterface_0065c780
            mov dword ptr [gUNK_0065bee0], esi
            call DoInterfaceScreen
            test eax, eax
            jne L004b3e4c
            call FUN_004b3ec0
    L004b3e4c:
            cmp dword ptr [gProgram_state+0x28], ebx
            je L004b3e65
            call FadePaletteDown
            mov ecx, 0xc
            call DisposeFont
            jmp L004b3ea1
    L004b3e65:
            mov ecx, 0xa1
            call RunFlic
            mov ecx, 0xc
            call DisposeFont
            jmp L004b3ea1
    L004b3e7b:
            push ebx
            xor edx, edx
            mov ecx, offset gInterface_0065c068
            call DoInterfaceScreen
            cmp dword ptr [gProgram_state+0x28], ebx
            je L004b3e97
            call FadePaletteDown
            jmp L004b3ea1
    L004b3e97:
            mov ecx, 0x97
            call RunFlic
    L004b3ea1:
            mov ecx, ebp
            call UnlockBunchOfFlics
            mov edx, offset gInterface_0065cd70+0x17c
            xor ecx, ecx
            call PrintMemoryDump
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x208
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4020
void C2_HOOK_FASTCALL StripControls(char* pStr) {
    int i;
    int len;

    len = strlen(pStr);
    for (i = 0; i < len; i++) {
        if ((tU8)pStr[i] < 0x20 /* ' ' */) {
#ifdef CARPOCALYPSE2_FIX_BUGS
            memmove(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
#else
            memcpy(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
#endif
            len--;
#ifdef CARPOCALYPSE2_FIX_BUGS
            /* correctly handle stripping consecutive control characters */
            i--;
#endif
        }
    }
}

void C2_HOOK_FASTCALL LoadKeyNames(void) {
    int i;
    FILE* f;
    tPath_name the_path;
    char s[256];

    PathCat(the_path, gApplication_path, "KEYNAMES.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpenKeyNamesFile);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_names); i++) {
        PFfgets(s, sizeof(s), f);
        StripControls(s);
        gKey_names[i] = BrMemAllocate(strlen(s) + 1, kMem_misc);
        strcpy(gKey_names[i], s);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL DisposeKeyNames(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_names); i++) {
        BrMemFree(gKey_names[i]);
    }
}

// FUNCTION: CARMA2_HW 0x004b4070
void C2_NAKED C2_HOOK_FASTCALL FUN_004b4070(void) {
#ifndef CARPOCALYPSE2_MATCHING
    char** p;

    p = gKey_names;
    do {
        BrMemFree(*p);
        p++;
    } while ((int)p < (int)&gKey_names[CARPOCALYPSE2_ASIZE(gKey_names)]);
#else
    __asm {
            push esi
            mov esi, offset gOptions_key_names
    L004b4076:
            mov eax, dword ptr [esi]
            push eax
            call BrMemFree
            add esi, 4
            add esp, 4
            cmp esi, offset gINT_00691b34
            jl L004b4076
            pop esi
            ret
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL BackupKeyMappings(void) {
    int i;

    gOrig_key_map_index = gKey_map_index;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gBackup_key_mappings) != 4);
    C2_HOOK_BUG_ON(sizeof(gKey_mapping) != 4 * 77);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gBackup_key_mappings); i++) {

        gKey_map_index = i;
        LoadKeyMapping();
        memcpy(gBackup_key_mappings[i],
            gKey_mapping,
            sizeof(gKey_mapping));
    }
    gKey_map_index = gOrig_key_map_index;
    memcpy(gKey_mapping,
        gBackup_key_mappings[gKey_map_index],
        sizeof(gKey_mapping));
}

// GLOBAL: CARMA2_HW 0x0065bd80
int gUNK_0065bd80;

// GLOBAL: CARMA2_HW 0x0065bda0
int gUNK_0065bda0;

// GLOBAL: CARMA2_HW 0x0065bdc0
int gUNK_0065bdc0;

// GLOBAL: CARMA2_HW 0x0065bde0
int gUNK_0065bde0;

// GLOBAL: CARMA2_HW 0x0065be00
int gUNK_0065be00;

// GLOBAL: CARMA2_HW 0x0065be20
int gUNK_0065be20;

// GLOBAL: CARMA2_HW 0x0065be40
int gUNK_0065be40;

// GLOBAL: CARMA2_HW 0x0065be60
int gUNK_0065be60;

// GLOBAL: CARMA2_HW 0x0065be80
int gUNK_0065be80;

// GLOBAL: CARMA2_HW 0x0065bea0
int gUNK_0065bea0;

// GLOBAL: CARMA2_HW 0x0065bec0
int gUNK_0065bec0;

// GLOBAL: CARMA2_HW 0x0065bee0
int gUNK_0065bee0;

// GLOBAL: CARMA2_HW 0x0058a6f4
float gUNK_0058a6f4 = 5.0f;

// GLOBAL: CARMA2_HW 0x0058a6e0
float gUNK_0058a6e0 = 35.0f;

// C2PORT-NONMATCHING: best 85.17% | difftest PASS 2000 trials (spec difftest/specs/004b3ec0_FUN_004b3ec0.json) | reason regalloc-switch-chain
// FUNCTION: CARMA2_HW 0x004b3ec0
void C2_HOOK_FASTCALL FUN_004b3ec0(void) {
    int level;

    SetCarSimplificationLevel(gUNK_0065bd80 >= 2 ? gUNK_0065bd80 + 1 : gUNK_0065bd80);
    level = -1;
    switch (gUNK_0065bda0) {
    case 0: level = 2; break;
    case 1: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetCarTexturingLevel(level);
    }
    level = -1;
    switch (gUNK_0065bdc0) {
    case 0: level = 3; break;
    case 1: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetShadowLevel(level);
    }
    level = -1;
    switch (gUNK_0065bde0) {
    case 0: level = 2; break;
    case 1: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetWallTexturingLevel(level);
    }
    level = -1;
    switch (gUNK_0065be00) {
    case 0: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetRoadTexturingLevel(level);
    }
    SetYon(gUNK_0058a6e0 - (float)gUNK_0065be20 * gUNK_0058a6f4);
    switch (gUNK_0065be40) {
    case 0:
        SetYonFactor(1.0f);
        break;
    case 1:
        SetYonFactor(0.5f);
        break;
    case 2:
        SetYonFactor(0.25f);
        break;
    case 3:
        SetYonFactor(0.125f);
        break;
    default:
        break;
    }
    SetSkyTextureOn(1 - gUNK_0065be60);
    SetDepthCueingOn(1 - gUNK_0065be80);
    SetAccessoryRendering(1 - gUNK_0065bea0);
    SetSmokeOn(1 - gUNK_0065bec0);
    SetSoundDetailLevel(2 - gUNK_0065bee0);
    SaveOptions();
}
