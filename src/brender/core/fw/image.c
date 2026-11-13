#include "image.h"

#include "loader.h"
#include "brlists.h"
#include "fwsetup.h"
#include "pattern.h"
#include "resource.h"
#include "scratch.h"

#include "core/host/himage.h"
#include "core/std/brstdlib.h"

#include "br_platform.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrAddHead();
extern br_boolean C2_HOOK_CDECL BrNamePatternMatch();
extern void* C2_HOOK_CDECL BrResAdd();
extern void* C2_HOOK_CDECL BrResAllocate();
extern char* C2_HOOK_CDECL BrResStrDup();
extern char* C2_HOOK_CDECL BrScratchString();
extern char* C2_HOOK_CDECL BrStrCat();
extern char* C2_HOOK_CDECL BrStrCpy();
extern int C2_HOOK_CDECL BrStrICmp();
extern char* C2_HOOK_CDECL BrStrRChr();
extern void* C2_HOOK_CDECL HostImageLoad();
extern br_image* C2_HOOK_STDCALL ImageLoad(const char* name);
extern br_framework_state fw;
extern char tokenTypes[];
#endif
// FUNCTION: CARMA2_HW 0x0052fed0
br_boolean C2_HOOK_CDECL BrImageAdd(br_image* img) {
    BrAddHead(&fw.images, &img->node);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052fef0
br_boolean C2_HOOK_CDECL BrImageRemove(br_image* img) {
    BrRemove(&img->node);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052ff10
br_image* C2_HOOK_CDECL BrImageFind(const char* pattern) {
    char* c;
    br_image* img;

    c = BrStrRChr(pattern, '.');
    if (c != NULL && (BrStrICmp(c, ".dll") == 0 || BrStrICmp(c, ".bdd") == 0 || BrStrICmp(c, ".bed") == 0)) {
        *c = '\0';
    }
    for (img = (br_image*)fw.images.head; img->node.next != NULL; img = (br_image*)img->node.next) {
        if (BrNamePatternMatch(pattern, img->identifier) != 0) {
            return img;
        }
    }
    return NULL;
}

br_image* C2_HOOK_STDCALL imageLoadHost(const char* name) {
    br_image* img;
    void* host_image;

    host_image = HostImageLoad(name);
    if (host_image != NULL) {
        img = BrResAllocate(NULL, sizeof(br_image), BR_MEMORY_IMAGE);
        img->identifier = BrResStrDup(img, name);
        img->type = 3;
        img->type_pointer = host_image;
    } else {
        img = NULL;
    }
    return img;
}

// FUNCTION: CARMA2_HW 0x0052ffa0
C2_NAKED br_image* C2_HOOK_CDECL BrImageReference(const char* name) {
#ifndef CARPOCALYPSE2_MATCHING
    const char* suffix;
    char* suffix2;
    char* scratch;
    br_image* img;

    scratch = BrScratchString();
    img = BrImageFind(name);
    if (img != NULL) {
        img->ref_count++;
        return img;
    }
    for (suffix = name; *suffix != '\0' && *suffix != '.'; suffix++) {
    }
    if (*suffix == '\0') {
        if (img == NULL) {
            BrStrCpy(scratch, name);
            BrStrCat(scratch, ".BDD");
            img = ImageLoad(scratch);
        }
        if (img == NULL) {
            img = imageLoadHost(name);
        }
        if (img == NULL) {
            BrStrCpy(scratch, name);
            BrStrCat(scratch, ".DLL");
            img = ImageLoad(scratch);
        }
    } else if (BrStrICmp(suffix, ".bdd") == 0 || BrStrICmp(suffix, ".bed") == 0) {
        if (img == NULL) {
            img = ImageLoad(name);
        }
        if (img == NULL) {
            img = imageLoadHost(name);
        }
    } else {
        if (img == NULL) {
            img = imageLoadHost(name);
        }
        if (img == NULL) {
            img = ImageLoad(name);
        }
    }

    if (img != NULL) {
        for (suffix2 = img->identifier; *suffix != '\0' && *suffix != '.'; suffix++) {
        }
        if (suffix2 != NULL) {
            *suffix2 = '\0';
        }
        BrResAdd(fw.res, img);
        BrAddHead(&fw.images, &img->node);
    }
    return img;
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            call BrScratchString
            mov edi, dword ptr [esp + 0x14]
            push 0x2e
            push edi
            mov ebp, eax
            call BrStrRChr
            mov esi, eax
            add esp, 8
            test esi, esi
            je L0052fff9
            push offset tokenTypes+0x1db4
            push esi
            call BrStrICmp
            add esp, 8
            test eax, eax
            je L0052fff6
            push offset tokenTypes+0x1dbc
            push esi
            call BrStrICmp
            add esp, 8
            test eax, eax
            je L0052fff6
            push offset tokenTypes+0x1dc4
            push esi
            call BrStrICmp
            add esp, 8
            test eax, eax
            jne L0052fff9
    L0052fff6:
            mov byte ptr [esi], 0
    L0052fff9:
            mov esi, dword ptr [fw+0x454]
            cmp dword ptr [esi], 0
            je L0053001c
    L00530004:
            mov eax, dword ptr [esi + 8]
            push eax
            push edi
            call BrNamePatternMatch
            add esp, 8
            test eax, eax
            jne L0053001e
            mov esi, dword ptr [esi]
            cmp dword ptr [esi], 0
            jne L00530004
    L0053001c:
            xor esi, esi
    L0053001e:
            test esi, esi
            je L00530030
            mov eax, dword ptr [esi + 0x10]
            inc eax
            mov dword ptr [esi + 0x10], eax
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00530030:
            mov al, byte ptr [edi]
            mov esi, edi
            test al, al
            je L00530044
    L00530038:
            cmp al, 0x2e
            je L00530044
            mov al, byte ptr [esi + 1]
            inc esi
            test al, al
            jne L00530038
    L00530044:
            cmp byte ptr [esi], 0
            jne L005300db
            push edi
            push ebp
            call BrStrCpy
            add esp, 8
            push offset tokenTypes+0x1dcc
            push ebp
            call BrStrCat
            add esp, 8
            push ebp
            call ImageLoad
            mov esi, eax
            test esi, esi
            jne L00530190
            push edi
            call HostImageLoad
            mov ebx, eax
            add esp, 4
            test ebx, ebx
            je L005300ac
            push 0x1a
            push 0x40
            push esi
            call BrResAllocate
            add esp, 0xc
            mov esi, eax
            push edi
            push esi
            call BrResStrDup
            add esp, 8
            mov dword ptr [esi + 8], eax
            mov dword ptr [esi + 0xc], 3
            mov dword ptr [esi + 0x3c], ebx
            jmp L005300ae
    L005300ac:
            xor esi, esi
    L005300ae:
            test esi, esi
            jne L00530190
            push edi
            push ebp
            call BrStrCpy
            add esp, 8
            push offset tokenTypes+0x1dd4
            push ebp
            call BrStrCat
            add esp, 8
            push ebp
            call ImageLoad
            mov esi, eax
            jmp L0053018c
    L005300db:
            push offset tokenTypes+0x1ddc
            push esi
            call BrStrICmp
            add esp, 8
            test eax, eax
            je L00530147
            push offset tokenTypes+0x1de4
            push esi
            call BrStrICmp
            add esp, 8
            test eax, eax
            je L00530147
            push edi
            call HostImageLoad
            mov ebx, eax
            add esp, 4
            test ebx, ebx
            je L00530137
            push 0x1a
            push 0x40
            push 0
            call BrResAllocate
            add esp, 0xc
            mov esi, eax
            push edi
            push esi
            call BrResStrDup
            add esp, 8
            mov dword ptr [esi + 8], eax
            mov dword ptr [esi + 0xc], 3
            mov dword ptr [esi + 0x3c], ebx
            jmp L00530139
    L00530137:
            xor esi, esi
    L00530139:
            test esi, esi
            jne L00530190
            push edi
            call ImageLoad
            mov esi, eax
            jmp L0053018c
    L00530147:
            push edi
            call ImageLoad
            mov esi, eax
            test esi, esi
            jne L00530190
            push edi
            call HostImageLoad
            mov ebx, eax
            add esp, 4
            test ebx, ebx
            je L0053018a
            push 0x1a
            push 0x40
            push esi
            call BrResAllocate
            add esp, 0xc
            mov esi, eax
            push edi
            push esi
            call BrResStrDup
            add esp, 8
            mov dword ptr [esi + 8], eax
            mov dword ptr [esi + 0xc], 3
            mov dword ptr [esi + 0x3c], ebx
            jmp L0053018c
    L0053018a:
            xor esi, esi
    L0053018c:
            test esi, esi
            je L005301ca
    L00530190:
            mov ecx, dword ptr [esi + 8]
            mov al, byte ptr [ecx]
            test al, al
            je L005301a5
    L00530199:
            cmp al, 0x2e
            je L005301a5
            mov al, byte ptr [ecx + 1]
            inc ecx
            test al, al
            jne L00530199
    L005301a5:
            test ecx, ecx
            je L005301ac
            mov byte ptr [ecx], 0
    L005301ac:
            mov ecx, dword ptr [fw+0x438]
            push esi
            push ecx
            call BrResAdd
            add esp, 8
            push esi
            push offset fw+0x454
            call BrAddHead
            add esp, 8
    L005301ca:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00530260
void* C2_HOOK_STDCALL imageLookupName(br_image* img, const char* name, br_uint_32 hint) {
    int c;
    int limit;
    int base;

    if (hint < img->n_names && BrStrCmp(name, img->names[hint]) == 0) {
        return img->functions[img->name_ordinals[hint]];
    }
    limit = img->n_names;
    base = 0;
    while (limit != 0) {
        c = BrStrCmp(name, img->names[base + limit / 2]);
        if (c < 0) {
            limit = limit / 2;
        } else if (c > 0) {
            base += limit / 2 + 1;
            limit = limit - (limit / 2 + 1);
        } else {
            return img->functions[img->name_ordinals[base + limit / 2]];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005301e0
void* C2_HOOK_CDECL BrImageLookupName(br_image* img, const char* name, br_uint_32 hint) {
    char* scratch;
    void* p;

    scratch = BrScratchString();
    if (img->type == 3) {
        return HostImageLookupName(img->type_pointer, name, hint);
    }
    p = imageLookupName(img, name, hint);
    if (p != NULL) {
        return p;
    }
    if (*name == '_') {
        p = imageLookupName(img, &name[1], hint);
        if (p != NULL) {
            return p;
        }
    }
    scratch[0] = '_';
    BrStrCpy(&scratch[1], name);
    return imageLookupName(img, scratch, hint);
}

// FUNCTION: CARMA2_HW 0x00530320
void* C2_HOOK_CDECL BrImageLookupOrdinal(br_image* img, br_uint_32 ordinal) {

    if (img->type == 3) {
        return HostImageLookupOrdinal(img->type_pointer, ordinal);
    }
    if (ordinal - img->ordinal_base > img->n_functions) {
        return NULL;
    }
    return img->functions[ordinal - img->ordinal_base];
}

// FUNCTION: CARMA2_HW 0x00530360
void C2_HOOK_CDECL BrImageDereference(br_image* image) {
    int i;
    image->ref_count--;

    if (image->ref_count > 0) {
        return;
    }
    switch (image->type) {
        case 2:
            return;
        case 3:
            HostImageUnload(image->type_pointer);
            break;
        case 1:
            break;
    }
#ifdef BRENDER_FIX_BUGS
    /* Added by carpocalypse2: fixes DEP */
    for (i = 0; i < image->n_sections; i++) {
        PDMapImageSection(image->sections[i].base, image->sections[i].mem_size,
                          kMemory_section_read | kMemory_section_write);
    }
#endif
    BrRemove(&image->node);
    BrResFree(image);
}

// FUNCTION: CARMA2_HW 0x005303a0
void C2_HOOK_CDECL BrImageFree(br_image* image) {
    int i;

    for (i = 0; i < image->n_imports; i++) {
        BrImageDereference(image->imports[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00530400
void C2_HOOK_CDECL _BrImageFree(void* res, br_uint_8 res_class, br_size_t size) {

    BrImageFree(res);
}
