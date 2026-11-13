#include "loader.h"

#include "core/fw/diag.h"
#include "core/fw/file.h"
#include "core/fw/image.h"
#include "core/fw/resource.h"

#include "br_platform.h"

#define IMAGE_DOS_SIGNATURE                 0x5A4D      /* MZ */
#define IMAGE_NT_SIGNATURE                  0x00004550  /* PE00 */
#define IMAGE_FILE_MACHINE_I386              0x014c     /* Intel 386. */

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  /* Relocation info stripped from file. */
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  /* File is executable  (i.e. no unresolved external references). */
#define IMAGE_FILE_32BIT_MACHINE             0x0100  /* 32 bit word machine. */

#define DIRECTORY_EXPORT 0
#define DIRECTORY_IMPORT 1
#define DIRECTORY_BASERELOC 5

#define IMAGE_REL_BASED_ABSOLUTE              0
#define IMAGE_REL_BASED_HIGH                  1
#define IMAGE_REL_BASED_LOW                   2
#define IMAGE_REL_BASED_HIGHLOW               3

#define IMAGE_SECTION_FLAG_MEM_EXECUTE      0x20000000  /* Section is executable. */
#define IMAGE_SECTION_FLAG_MEM_READ         0x40000000  /* Section is readable. */
#define IMAGE_SECTION_FLAG_MEM_WRITE        0x80000000  /* Section is writeable. */

#define READ_BLOCK(var, fh) \
    do { \
        if (BrFileRead(&(var), 1, sizeof(var),(fh)) != sizeof(var)) { \
            return NULL; \
        }; \
    } while (0)
#define ENTRY_TYPE(e) ((e) >> 12)
#define ENTRY_OFFSET(e) ((e) & 0xfff)

#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern void C2_HOOK_CDECL BrFileAdvance();
extern void C2_HOOK_CDECL BrFileClose();
extern void* C2_HOOK_CDECL BrFileOpenRead();
extern int C2_HOOK_CDECL BrFileRead();
extern void* C2_HOOK_CDECL BrImageLookupName();
extern void* C2_HOOK_CDECL BrImageLookupOrdinal();
extern br_image* C2_HOOK_CDECL BrImageReference();
extern void* C2_HOOK_CDECL BrResAllocate();
extern char* C2_HOOK_CDECL BrResStrDup();
#endif
// FUNCTION: CARMA2_HW 0x00530e70
C2_NAKED br_image* C2_HOOK_STDCALL ImageLoad(const char* name) {
#ifndef CARPOCALYPSE2_MATCHING
    void* fh;
	int mode = BR_FS_MODE_BINARY;
	struct msdos_header dos_header;
	struct coff_header coff_header;
	struct nt_optional_header nt_header;
	struct section_header section_header;
	br_uint_32 pe;
	br_image* img;
	br_uint_8* arena_base;
	int arena_size, arena_align;
	int i;
	br_uint_32 offset;
	unsigned int n;

	fh = BrFileOpenRead(name, 0, NULL, &mode);
	if (fh == NULL) {
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(dos_header) != 0x40);
	READ_BLOCK(dos_header, fh);
	if (dos_header.magic != IMAGE_DOS_SIGNATURE) {
        return NULL;
    }
	if (dos_header.new_header_offset < sizeof(dos_header)) {
        return NULL;
    }

	BrFileAdvance(dos_header.new_header_offset - sizeof(dos_header), fh);

    C2_HOOK_BUG_ON(sizeof(pe) != 0x4);
	READ_BLOCK(pe, fh);
	if (pe != IMAGE_NT_SIGNATURE) {
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(coff_header) != 0x14);
	READ_BLOCK(coff_header, fh);
	if (coff_header.machine != IMAGE_FILE_MACHINE_I386) {
        return NULL;
    }
	if (coff_header.flags & IMAGE_FILE_RELOCS_STRIPPED) {
        return NULL;
    }
    if (!(coff_header.flags & IMAGE_FILE_EXECUTABLE_IMAGE)) {
        return NULL;
    }
	if (!(coff_header.flags & IMAGE_FILE_32BIT_MACHINE)) {
        return NULL;
    }
	if (coff_header.opt_header_size != sizeof(nt_header)) {
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(nt_header) != 0xe0);
	READ_BLOCK(nt_header, fh);

    C2_HOOK_BUG_ON(sizeof(br_image) != 0x40);
	img = BrResAllocate(NULL, sizeof(br_image), BR_MEMORY_IMAGE);

	img->type = BR_IMG_FRAMEWORK;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_BUG_ON(sizeof(br_image_section) != 0x18);
#endif
	img->sections = BrResAllocate(img, sizeof(br_image_section) * coff_header.n_sections, BR_MEMORY_IMAGE_SECTIONS);

	if (nt_header.section_alignment & (nt_header.section_alignment - 1)) {
        return NULL;
    }

	for (i = 0; i < coff_header.n_sections; i++) {
        C2_HOOK_BUG_ON(sizeof(section_header) != 0x28);
		READ_BLOCK(section_header, fh);

		img->sections[i].name		 = BrResStrDup(img, section_header.section_name);
		img->sections[i].mem_offset	 = section_header.rva;
		img->sections[i].mem_size	 = section_header.virtual_size;
		img->sections[i].data_size 	 = section_header.data_size;
		img->sections[i].data_offset = section_header.data_offset;
#ifdef CARPOCALYPSE2_FIX_BUGS
		img->sections[i].flags       = section_header.flags;  /* Added by carpocalypse2 */
#endif /* CARPOCALYPSE2_FIX_BUGS */
	}

	arena_size = nt_header.image_size;
	arena_align = nt_header.section_alignment;

	arena_base = BrResAllocate(img, arena_size + (arena_align - 1), BR_MEMORY_IMAGE_ARENA);
	arena_base = (br_uint_8*) (((uintptr_t)arena_base+arena_align - 1) & (~(arena_align - 1)));

    C2_HOOK_BUG_ON(sizeof(pe) + sizeof(coff_header) + sizeof(nt_header) != 0xf8);
    C2_HOOK_BUG_ON(sizeof(section_header) != 0x28);
	offset = dos_header.new_header_offset + sizeof(pe) + sizeof(coff_header) + sizeof(nt_header) + coff_header.n_sections * sizeof(section_header);

	for (i = 0; i < coff_header.n_sections; i++) {

		if (img->sections[i].data_offset == 0) {
            continue;
        }
        if (img->sections[i].data_size == 0) {
            continue;
        }
		if (offset > img->sections[i].data_offset) {
            return NULL;
        }
		BrFileAdvance(img->sections[i].data_offset - offset, fh);

		img->sections[i].base = arena_base + img->sections[i].mem_offset;

		if ((br_uint_32)BrFileRead(img->sections[i].base, 1, img->sections[i].data_size, fh) != img->sections[i].data_size) {
            return NULL;
        }

		offset = img->sections[i].data_offset + img->sections[i].data_size;
	}

	BrFileClose(fh);

	if (nt_header.directories[DIRECTORY_EXPORT].size != 0) {
		struct export_directory* ed;

		ed = (export_directory*)(arena_base + nt_header.directories[DIRECTORY_EXPORT].rva);

		img->identifier = BrResStrDup(img, (char*)(arena_base + ed->name));  /* BrResStrDup added by carpocalypse2 */
		img->ordinal_base = ed->ordinal_base;
		img->n_functions = ed->n_entries;
		img->functions = (void**)(arena_base + ed->export_table);
		img->n_names = ed->n_names;
		img->names = (const char**)(arena_base + ed->name_table);
		img->name_ordinals = (br_uint_16*)(arena_base + ed->ordinal_table);

		for (n = 0; n < img->n_functions; n++) {
            img->functions[n] = (void*)((br_uint_32)img->functions[n] + arena_base);
        }

		for (n = 0; n < img->n_names; n++) {
            img->names[n] = (char*)((br_uint_32)img->names[n] + arena_base);
        }
	}

	if (nt_header.directories[DIRECTORY_IMPORT].size != 0) {
		struct import_directory* id;
		void** at;
		br_uint_32* lt;
		br_image* import_img;

		n = 0;
		for (id = (import_directory*)(arena_base + nt_header.directories[DIRECTORY_IMPORT].rva); id->lookup_table != 0; id++) {
            n += 1;
        }

		img->imports = BrResAllocate(img, n * sizeof(br_image*), BR_MEMORY_IMAGE_NAMES);
		img->n_imports = n;

		for (n = 0, id = (import_directory*)(arena_base + nt_header.directories[DIRECTORY_IMPORT].rva); id->lookup_table != 0; id++, n++) {

			import_img = BrImageReference((char*)(id->name + arena_base));
			if (import_img == NULL) {
				BrFailure("Could not reference image %s", (char*)(id->name + arena_base));
				return NULL;
			}

			img->imports[n] = import_img;

			at = (void**)(arena_base + id->address_table);

			for (lt = (br_uint_32*)(arena_base + id->lookup_table); *lt != 0; lt++, at++) {
				if (*lt & 0x80000000) {
                    *at = BrImageLookupOrdinal(import_img, *lt & 0x7fffffff);
                }
                if (*at == NULL) {
                    BrFailure("Could not resolve imported ordinal %s:%d", (char *)(id->name + arena_base), *lt & 0x7fffffff);
                    return NULL;
                } else {
					*at = BrImageLookupName(import_img, (char*)(*lt + arena_base + 2), *((br_uint_16*)(*lt + arena_base)));
    				if (*at == NULL) {
                        BrFailure("Could not resolve imported symbol %s:%s (%d)",
		        			(char*)(id->name + arena_base),
	    					(char*)(*lt+arena_base + 2),
                            *((br_uint_16*)(*lt + arena_base)));
					    return NULL;
       				}
                }
			}
		}
	}

	if ((uintptr_t)arena_base != (uintptr_t)nt_header.image_base && nt_header.directories[DIRECTORY_BASERELOC].size != 0) {
		basereloc_header* header;
		br_uint_16* entry;
		br_uint_8* fixup;
		br_int_32 delta;
		br_int_16 delta_h, delta_l;

		offset = 0;
		delta = (uintptr_t)arena_base - nt_header.image_base;
		delta_h = (br_uint_16)(delta >> 16);
		delta_l = (br_uint_16)(delta & 0xFFFF);

		while (offset < nt_header.directories[DIRECTORY_BASERELOC].size) {
			header = (basereloc_header*)(nt_header.directories[DIRECTORY_BASERELOC].rva + arena_base + offset);
			offset += header->size;

            C2_HOOK_BUG_ON(sizeof(basereloc_header) != 8);

			for (n = 0, entry = (br_uint_16*)(header + 1); n < (header->size - sizeof(basereloc_header)) / sizeof(br_uint_16); n++, entry++) {

				fixup = ENTRY_OFFSET(*entry) + header->rva + arena_base;

				if (fixup >= arena_base + arena_size) {
                    return NULL;
                }

				switch (ENTRY_TYPE(*entry)) {
				case IMAGE_REL_BASED_ABSOLUTE:
					break;

				case IMAGE_REL_BASED_HIGH:
					*(br_int_16*)fixup += delta_h;
					break;

				case IMAGE_REL_BASED_LOW:
					*(br_int_16*)fixup += delta_l;
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					*(br_int_32*)fixup += delta;
					break;

				default:
					return NULL;
				}
			}
		}
	}

#ifdef BRENDER_FIX_BUGS
    /* Added by carpocalypse2: fixes DEP */
    for (i = 0; i < coff_header.n_sections; i++) {
        br_uint_32 map_flags = 0;
        if (img->sections[i].flags & IMAGE_SECTION_FLAG_MEM_EXECUTE) {
            map_flags |= kMemory_section_executable;
        }
        if (img->sections[i].flags & IMAGE_SECTION_FLAG_MEM_READ) {
            map_flags |= kMemory_section_read;
        }
        if (img->sections[i].flags & IMAGE_SECTION_FLAG_MEM_WRITE) {
            map_flags |= kMemory_section_write;
        }
        PDMapImageSection(img->sections[i].base, img->sections[i].mem_size, map_flags);
    }
#endif /* BRENDER_FIX_BUGS */

	return img;
#else
    __asm {
            sub esp, 0x17c
            mov ecx, dword ptr [esp + 0x180]
            push ebx
            push ebp
            push esi
            lea eax, [esp + 0x1c]
            push edi
            xor ebx, ebx
            push eax
            push ebx
            push ebx
            push ecx
            mov dword ptr [esp + 0x30], ebx
            call BrFileOpenRead
            mov edi, eax
            add esp, 0x10
            cmp edi, ebx
            mov dword ptr [esp + 0x10], edi
            jne L00530eb0
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530eb0:
            push edi
            push 0x40
            lea edx, [esp + 0x74]
            push 1
            push edx
            call BrFileRead
            add esp, 0x10
            cmp eax, 0x40
            je L00530ed6
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530ed6:
            cmp word ptr [esp + 0x6c], 0x5a4d
            je L00530eee
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530eee:
            mov eax, dword ptr [esp + 0xa8]
            cmp eax, 0x40
            jae L00530f09
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530f09:
            add eax, -0x40
            push edi
            push eax
            call BrFileAdvance
            add esp, 8
            lea eax, [esp + 0x24]
            push edi
            push 4
            push 1
            push eax
            call BrFileRead
            add esp, 0x10
            cmp eax, 4
            je L00530f3c
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530f3c:
            cmp dword ptr [esp + 0x24], 0x4550
            je L00530f55
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530f55:
            push edi
            push 0x14
            lea ecx, [esp + 0x34]
            push 1
            push ecx
            call BrFileRead
            add esp, 0x10
            cmp eax, 0x14
            je L00530f7b
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530f7b:
            cmp word ptr [esp + 0x2c], 0x14c
            je L00530f93
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530f93:
            mov eax, dword ptr [esp + 0x3e]
            test al, 1
            je L00530faa
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530faa:
            test al, 2
            jne L00530fbd
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530fbd:
            test ah, 1
            jne L00530fd1
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530fd1:
            cmp word ptr [esp + 0x3c], 0xe0
            je L00530fe9
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00530fe9:
            push edi
            push 0xe0
            lea edx, [esp + 0xb4]
            push 1
            push edx
            call BrFileRead
            add esp, 0x10
            cmp eax, 0xe0
            je L00531017
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00531017:
            push 0x1a
            push 0x40
            push ebx
            call BrResAllocate
            mov ebp, eax
            add esp, 0xc
            mov dword ptr [esp + 0x18], ebp
            mov dword ptr [ebp + 0xc], 1
            mov eax, dword ptr [esp + 0x2e]
            and eax, 0xffff
            push 0x1c
            lea eax, [eax + eax*2]
            shl eax, 3
            push eax
            push ebp
            call BrResAllocate
            mov dword ptr [ebp + 0x38], eax
            mov esi, dword ptr [esp + 0xd8]
            add esp, 0xc
            lea ecx, [esi - 1]
            test ecx, esi
            je L0053106c
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L0053106c:
            cmp word ptr [esp + 0x2e], 0
            jbe L005310ea
            xor esi, esi
    L00531076:
            push edi
            push 0x28
            lea edx, [esp + 0x4c]
            push 1
            push edx
            call BrFileRead
            add esp, 0x10
            cmp eax, 0x28
            jne L00531444
            lea eax, [esp + 0x44]
            push eax
            push ebp
            call BrResStrDup
            mov ecx, dword ptr [ebp + 0x38]
            add esp, 8
            inc ebx
            mov dword ptr [esi + ecx], eax
            mov edx, dword ptr [ebp + 0x38]
            mov eax, dword ptr [esp + 0x50]
            mov dword ptr [esi + edx + 8], eax
            mov ecx, dword ptr [ebp + 0x38]
            mov edx, dword ptr [esp + 0x4c]
            mov dword ptr [esi + ecx + 0xc], edx
            mov eax, dword ptr [ebp + 0x38]
            mov ecx, dword ptr [esp + 0x54]
            mov dword ptr [esi + eax + 0x14], ecx
            mov edx, dword ptr [ebp + 0x38]
            mov eax, dword ptr [esp + 0x58]
            mov dword ptr [esi + edx + 0x10], eax
            mov ecx, dword ptr [esp + 0x2e]
            and ecx, 0xffff
            add esi, 0x18
            cmp ebx, ecx
            jl L00531076
            mov esi, dword ptr [esp + 0xcc]
    L005310ea:
            mov eax, dword ptr [esp + 0xe4]
            push 0x1b
            mov dword ptr [esp + 0x2c], eax
            lea edx, [esi + eax - 1]
            push edx
            push ebp
            call BrResAllocate
            add esp, 0xc
            lea ebx, [esi + eax - 1]
            mov eax, dword ptr [esp + 0x2e]
            dec esi
            mov ecx, dword ptr [esp + 0xa8]
            mov dword ptr [esp + 0x14], 0
            not esi
            and ebx, esi
            and eax, 0xffff
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*8 + 0xf8]
            jle L005311c6
            mov edi, dword ptr [ebp + 0x38]
            xor esi, esi
    L0053113b:
            mov eax, dword ptr [esi + edi + 0x10]
            test eax, eax
            je L005311a4
            mov edx, dword ptr [esi + edi + 0x14]
            test edx, edx
            je L005311a4
            cmp ecx, eax
            ja L00531444
            mov edx, dword ptr [esp + 0x10]
            sub eax, ecx
            push edx
            push eax
            call BrFileAdvance
            mov eax, dword ptr [ebp + 0x38]
            add esp, 8
            add eax, esi
            mov ecx, dword ptr [eax + 8]
            add ecx, ebx
            mov dword ptr [eax + 4], ecx
            mov edi, dword ptr [ebp + 0x38]
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esi + edi + 0x14]
            push eax
            mov ecx, edx
            mov dword ptr [esp + 0x20], edx
            mov edx, dword ptr [esi + edi + 4]
            push ecx
            push 1
            push edx
            call BrFileRead
            mov edx, dword ptr [esp + 0x2c]
            add esp, 0x10
            cmp eax, edx
            jne L00531444
            mov ecx, dword ptr [esi + edi + 0x10]
            add ecx, edx
    L005311a4:
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x2e]
            inc eax
            and edx, 0xffff
            add esi, 0x18
            cmp eax, edx
            mov dword ptr [esp + 0x14], eax
            jl L0053113b
            mov edi, dword ptr [esp + 0x10]
    L005311c6:
            push edi
            call BrFileClose
            mov eax, dword ptr [esp + 0x114]
            add esp, 4
            test eax, eax
            je L00531251
            mov eax, dword ptr [esp + 0x10c]
            add eax, ebx
            mov ecx, dword ptr [eax + 0xc]
            add ecx, ebx
            mov dword ptr [ebp + 8], ecx
            mov edx, dword ptr [eax + 0x10]
            mov dword ptr [ebp + 0x14], edx
            mov ecx, dword ptr [eax + 0x14]
            mov dword ptr [ebp + 0x18], ecx
            mov edx, dword ptr [eax + 0x1c]
            add edx, ebx
            mov dword ptr [ebp + 0x1c], edx
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [ebp + 0x20], ecx
            mov edx, dword ptr [eax + 0x20]
            mov ecx, dword ptr [ebp + 0x18]
            add edx, ebx
            mov dword ptr [ebp + 0x24], edx
            mov eax, dword ptr [eax + 0x24]
            add eax, ebx
            mov dword ptr [ebp + 0x28], eax
            xor eax, eax
            test ecx, ecx
            jbe L00531233
    L0053121e:
            mov ecx, dword ptr [ebp + 0x1c]
            mov esi, dword ptr [ecx + eax*4]
            lea ecx, [ecx + eax*4]
            add esi, ebx
            inc eax
            mov dword ptr [ecx], esi
            mov ecx, dword ptr [ebp + 0x18]
            cmp eax, ecx
            jb L0053121e
    L00531233:
            mov ecx, dword ptr [ebp + 0x20]
            xor eax, eax
            test ecx, ecx
            jbe L00531251
    L0053123c:
            mov edx, dword ptr [ebp + 0x24]
            mov esi, dword ptr [edx + eax*4]
            lea ecx, [edx + eax*4]
            add esi, ebx
            inc eax
            mov dword ptr [ecx], esi
            mov ecx, dword ptr [ebp + 0x20]
            cmp eax, ecx
            jb L0053123c
    L00531251:
            mov eax, dword ptr [esp + 0x118]
            test eax, eax
            je L00531365
            mov eax, dword ptr [esp + 0x114]
            xor esi, esi
            add eax, ebx
            cmp dword ptr [eax], 0
            je L0053127b
    L00531270:
            mov ecx, dword ptr [eax + 0x14]
            add eax, 0x14
            inc esi
            test ecx, ecx
            jne L00531270
    L0053127b:
            lea ecx, [esi*4]
            push 0x1d
            push ecx
            push ebp
            call BrResAllocate
            mov dword ptr [ebp + 0x30], eax
            mov word ptr [ebp + 0x2c], si
            mov edx, dword ptr [esp + 0x120]
            add esp, 0xc
            mov eax, dword ptr [ebx + edx]
            lea esi, [ebx + edx]
            test eax, eax
            mov dword ptr [esp + 0x10], esi
            je L00531365
            mov dword ptr [esp + 0x14], 0
            jmp L005312bc
    L005312b8:
            mov esi, dword ptr [esp + 0x10]
    L005312bc:
            mov eax, dword ptr [esi + 0xc]
            add eax, ebx
            push eax
            call BrImageReference
            mov ebp, eax
            add esp, 4
            test ebp, ebp
            je L00531462
            mov ecx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [ecx + 0x30]
            mov dword ptr [eax + edx], ebp
            mov edi, dword ptr [esi + 0x10]
            mov esi, dword ptr [esi]
            add edi, ebx
            add esi, ebx
            mov eax, dword ptr [esi]
            test eax, eax
            je L00531342
    L005312f1:
            test eax, 0x80000000
            je L00531309
            and eax, 0x7fffffff
            push eax
            push ebp
            call BrImageLookupOrdinal
            add esp, 8
            mov dword ptr [edi], eax
    L00531309:
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [esi]
            test eax, eax
            je L00531484
            lea eax, [ecx + ebx]
            xor edx, edx
            mov dx, word ptr [eax]
            add eax, 2
            push edx
            push eax
            push ebp
            call BrImageLookupName
            add esp, 0xc
            mov dword ptr [edi], eax
            test eax, eax
            je L005314b1
            mov eax, dword ptr [esi + 4]
            add esi, 4
            add edi, 4
            test eax, eax
            jne L005312f1
    L00531342:
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x14]
            add eax, 0x14
            add edx, 4
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], edx
            cmp dword ptr [eax], 0
            jne L005312b8
            mov ebp, dword ptr [esp + 0x18]
    L00531365:
            mov edx, dword ptr [esp + 0xc8]
            cmp ebx, edx
            je L00531453
            mov edi, dword ptr [esp + 0x138]
            test edi, edi
            je L00531453
            mov eax, ebx
            xor ecx, ecx
            sub eax, edx
            mov dword ptr [esp + 0x18], eax
            sar eax, 0x10
            test edi, edi
            mov dword ptr [esp + 0x1c], eax
            jbe L00531453
    L0053139c:
            mov eax, dword ptr [esp + 0x134]
            lea edx, [ecx + ebx]
            add edx, eax
            mov dword ptr [esp + 0x14], 0
            mov eax, dword ptr [edx + 4]
            lea esi, [edx + 8]
            add ecx, eax
            add eax, -8
            test eax, 0xfffffffe
            mov dword ptr [esp + 0x10], ecx
            jbe L0053142d
            mov eax, dword ptr [esp + 0x28]
            lea edi, [eax + ebx]
    L005313cd:
            xor ecx, ecx
            mov cx, word ptr [esi]
            mov eax, ecx
            and eax, 0xfff
            add eax, dword ptr [edx]
            add eax, ebx
            cmp eax, edi
            jae L00531444
            shr ecx, 0xc
            cmp ecx, 5
            ja L00531444
            jmp dword ptr [ecx*4 + 0x5314e8]
            mov cx, word ptr [esp + 0x1c]
            add word ptr [eax], cx
            jmp L0053140a
            mov cx, word ptr [esp + 0x18]
            add word ptr [eax], cx
            jmp L0053140a
            mov ecx, dword ptr [esp + 0x18]
            add dword ptr [eax], ecx
    L0053140a:
            mov ecx, dword ptr [edx + 4]
            mov eax, dword ptr [esp + 0x14]
            sub ecx, 8
            inc eax
            shr ecx, 1
            add esi, 2
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jb L005313cd
            mov ecx, dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x138]
    L0053142d:
            cmp ecx, edi
            jb L0053139c
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00531444:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00531453:
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00531462:
            mov eax, dword ptr [esi + 0xc]
            add eax, ebx
            push eax
            push 0x66cae8
            call BrFailure
            add esp, 8
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L00531484:
            mov edx, dword ptr [esp + 0x10]
            and ecx, 0x7fffffff
            push ecx
            mov eax, dword ptr [edx + 0xc]
            add eax, ebx
            push eax
            push 0x66cb08
            call BrFailure
            add esp, 0xc
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
    L005314b1:
            mov ecx, dword ptr [esi]
            xor edx, edx
            mov dx, word ptr [ecx + ebx]
            lea eax, [ecx + ebx]
            add eax, 2
            push edx
            push eax
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax + 0xc]
            add ecx, ebx
            push ecx
            push 0x66cb34
            call BrFailure
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x17c
            ret 4
            nop
            _emit 0x0a
            _emit 0x14
            _emit 0x53
            _emit 0x00
            _emit 0xf0
            _emit 0x13
            _emit 0x53
            _emit 0x00
            _emit 0xfa
            _emit 0x13
            _emit 0x53
            _emit 0x00
            _emit 0x04
            _emit 0x14
            _emit 0x53
            _emit 0x00
            _emit 0x44
            _emit 0x14
            _emit 0x53
            _emit 0x00
            _emit 0x44
            _emit 0x14
            _emit 0x53
            _emit 0x00
    }
#endif
}
