#include "packfile.h"

#include "loading1.h"
#include "utility.h"
#include "errors.h"
#include "sound.h"
#include "platform.h"
#include "carpocalypse2_macros.h"

#include <string.h>
// GLOBAL: CARMA2_HW 0x00691b40
tTwatVfsMountPoint gTwatVfsMountPoints[5];

// GLOBAL: CARMA2_HW 0x00692070
tTwatVfsFile gTwatVfsFiles[51];

// GLOBAL: CARMA2_HW 0x006923a0
int gDisableTiffConversionStackPos;

// GLOBAL: CARMA2_HW 0x00692068
int gDisableTiffConversionStack[2];

// GLOBAL: CARMA2_HW 0x0068c724
int gDisableTiffConversion;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern int C2_HOOK_FASTCALL PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data);
extern void C2_HOOK_FASTCALL PDForEveryFile(const char* pThe_path, tPDForEveryFile_cbfn pAction_routine);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern char _fgets[];
extern char _ungetc[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char _rewind[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF);
extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size);
extern int gCancel_recover;
extern char gInterface_0065cd70[];
#endif
// FUNCTION: CARMA2_HW 0x004b4570
void C2_NAKED C2_HOOK_FASTCALL InitPackFiles(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsMountPoint) != 264);

    for (i = 1; i < (int)CARPOCALYPSE2_ASIZE(gTwatVfsFiles); i++) {
        gTwatVfsFiles[i].start = NULL;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTwatVfsMountPoints); i++) {
        gTwatVfsMountPoints[i].header = NULL;
    }

    gDisableTiffConversionStackPos = 0;
#else
    __asm {
            mov eax, offset gTwatVfsFiles+0x10
    L004b4575:
            mov dword ptr [eax], 0
            add eax, 0x10
            cmp eax, offset gDisableTiffConversionStackPos
            jl L004b4575
            mov eax, offset gTwatVfsMountPoints
    L004b458a:
            mov dword ptr [eax], 0
            add eax, 0x108
            cmp eax, offset gDisableTiffConversionStack
            jl L004b458a
            mov dword ptr [gDisableTiffConversionStackPos], 0
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

// FUNCTION: CARMA2_HW 0x004b45b0
tTWTVFS C2_NAKED C2_HOOK_FASTCALL OpenPackFile(const char* path) {
#ifndef CARPOCALYPSE2_MATCHING
    tPath_name twatFilePath;
    FILE* f;
    tTWTVFS twt;
    tU32 fileSize;
    tU8* fileData;
    int i;

    // file header must be 56 bytes for compatibility with .TWT files
    CARPOCALYPSE2_BUG_ON(sizeof(tTwatFileHeader) != 56);

    strcpy(twatFilePath, path);
    strcat(twatFilePath, ".TWT");

    f = fopen(twatFilePath, "rb");
#if defined(CARPOCALYPSE2_FIX_BUGS)
    if (f == NULL) {
        /* retail expects "<path>.TWT" (e.g. <root>/DATA.TWT); also accept
         * the pack inside its own directory (<root>/DATA/DATA.TWT). */
        const char* base = strrchr(path, '/');

        if (base != NULL && base[1] != '\0') {
            snprintf(twatFilePath, sizeof(twatFilePath), "%s/%s.TWT", path, base + 1);
            f = fopen(twatFilePath, "rb");
        }
    }
#endif
    if (f != NULL) {
        for (twt = 0; ; twt++) {
            if (twt >= (int)CARPOCALYPSE2_ASIZE(gTwatVfsMountPoints)) {
                fclose(f);
                return -1;
            }
            if (gTwatVfsMountPoints[twt].header == NULL) {
                break;
            }
        }
        strcpy(gTwatVfsMountPoints[twt].path, path);
        fileSize = ReadU32(f);
        rewind(f);

        gTwatVfsMountPoints[twt].header = BrMemAllocate(fileSize, kMem_packed_file);
        fread(gTwatVfsMountPoints[twt].header, fileSize, 1, f);
        SplungeSomeData(gTwatVfsMountPoints[twt].header, fileSize);
        gTwatVfsMountPoints[twt].data = (tU8*)gTwatVfsMountPoints[twt].header;
        fclose(f);

        fileData = (tU8*)&gTwatVfsMountPoints[twt].header->fileHeaders[gTwatVfsMountPoints[twt].header->nbFiles];
        for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
            gTwatVfsMountPoints[twt].header->fileHeaders[i].data_offset = (tU32)(fileData - (tU8*)gTwatVfsMountPoints[twt].header);
            fileData += (gTwatVfsMountPoints[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
        }
        return twt;
    }
    return -1;
#else
    __asm {
            sub esp, 0x108
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x14], ecx
            mov edi, ecx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x18]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x18]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            push offset gCancel_recover+0x1a4
            rep movsb
            mov edi, offset gInterface_0065cd70+0x1ac
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea eax, [esp + 0x1c]
            and ecx, 3
            push eax
            rep movsb
            call fopen
            mov ebp, eax
            add esp, 8
            test ebp, ebp
            je L004b4651
            xor ecx, ecx
            mov eax, offset gTwatVfsMountPoints
            mov dword ptr [esp + 0x10], ecx
    L004b4632:
            cmp dword ptr [eax], 0
            je L004b465f
            add eax, 0x108
            inc ecx
            cmp eax, offset gDisableTiffConversionStack
            jl L004b4632
            push ebp
            mov dword ptr [esp + 0x14], ecx
            call fclose
            add esp, 4
    L004b4651:
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret
    L004b465f:
            mov edi, dword ptr [esp + 0x14]
            mov ebx, ecx
            shl ebx, 5
            mov dword ptr [esp + 0x10], ecx
            add ebx, ecx
            or ecx, 0xffffffff
            xor eax, eax
            shl ebx, 3
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebx + gTwatVfsMountPoints+0x4]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, ebp
            call ReadU32
            mov esi, eax
            push ebp
            call rewind
            add esp, 4
            push 0xd0
            push esi
            call BrMemAllocate
            add esp, 8
            mov dword ptr [ebx + gTwatVfsMountPoints], eax
            push ebp
            push 1
            push esi
            push eax
            call fread
            mov ecx, dword ptr [ebx + gTwatVfsMountPoints]
            add esp, 0x10
            mov edx, esi
            call SplungeSomeData
            mov esi, dword ptr [ebx + gTwatVfsMountPoints]
            push ebp
            mov dword ptr [ebx + gTwatVfsMountPoints+0x104], esi
            call fclose
            mov eax, dword ptr [esi + 4]
            mov edx, dword ptr [ebx + gTwatVfsMountPoints]
            add esp, 4
            lea ecx, [eax*8]
            sub ecx, eax
            lea ecx, [edx + ecx*8 + 8]
            xor edx, edx
            test eax, eax
            jle L004b4720
            lea eax, [esi + 8]
    L004b4708:
            mov dword ptr [eax + 0x34], ecx
            mov edi, dword ptr [eax]
            add edi, 3
            add eax, 0x38
            and edi, 0xfffffffc
            add ecx, edi
            mov edi, dword ptr [esi + 4]
            inc edx
            cmp edx, edi
            jl L004b4708
    L004b4720:
            mov eax, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4730
void C2_HOOK_FASTCALL ClosePackFile(tTWTVFS twt) {
    if (twt >= 0) {
        BrMemFree(gTwatVfsMountPoints[twt].header);
        gTwatVfsMountPoints[twt].header = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004b4760
int C2_HOOK_FASTCALL PFfclose(FILE* pFile) {
    if ((uintptr_t)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        fclose(pFile);
    } else {
        gTwatVfsFiles[(uintptr_t)pFile].start = NULL;
    }
}

// Windows gets neither this fallback nor the dirent.h it is built on: MSVC
// has no dirent.h, and its filesystem is case-insensitive, so the exact-name
// fopen below already succeeds on retail case.
#if defined(CARPOCALYPSE2_FIX_BUGS) && !defined(_WIN32)
#include <dirent.h>

// Case-insensitive filesystem fallback helper. The retail game assumed a
// case-insensitive filesystem (Windows) and hard-codes paths such as
// ".../CARS/64X48X8/EAGLE3.TXT" while the data ships with lower-case names
// (".../CARS/64x48x8/eagle3.txt"). When an exact open fails on a case-sensitive
// host, resolve the path by matching every component case-insensitively.
static FILE* CI_FopenCaseInsensitive(const char* pPath, const char* mode) {
    char resolved[256];
    char comp[256];
    const char* p;
    size_t len;
    size_t comp_len;

    if (pPath == NULL || mode == NULL || pPath[0] == '\0') {
        return NULL;
    }

    resolved[0] = '\0';

    // Split the path into components and resolve them one directory at a time,
    // matching each component case-insensitively against the parent directory.
    p = pPath;
    while (p[0] == '/') {
        p++;
    }

    for (;;) {
        comp_len = strcspn(p, "/");
        if (comp_len >= sizeof(comp)) {
            return NULL;
        }
        memcpy(comp, p, comp_len);
        comp[comp_len] = '\0';

        {
            DIR* d = opendir(resolved[0] == '\0' ? "/" : resolved);
            struct dirent* e;
            int found = 0;
            if (d == NULL) {
                return NULL;
            }
            while ((e = readdir(d)) != NULL) {
                if (DRStricmp(e->d_name, comp) == 0) {
                    len = strlen(resolved);
                    if (len > 0 && resolved[len - 1] != '/') {
                        strcat(resolved, "/");
                    } else if (len == 0) {
                        strcat(resolved, "/");
                    }
                    strcat(resolved, e->d_name);
                    found = 1;
                    break;
                }
            }
            closedir(d);
            if (!found) {
                return NULL;
            }
        }

        p += comp_len;
        if (p[0] == '\0' || p[1] == '\0') {
            break;
        }
        if (p[0] == '/') {
            p++;
        }
    }

    return fopen(resolved, mode);
}
#endif

// FUNCTION: CARMA2_HW 0x004b4780
C2_NAKED FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode) {
#ifndef CARPOCALYPSE2_MATCHING
    int twt;
    int i;
    int file_index;
    size_t twt_path_len;

    for (twt = 0; twt < (int)CARPOCALYPSE2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header != NULL && strstr(pPath, gTwatVfsMountPoints[twt].path) == pPath) {
            twt_path_len = strlen(gTwatVfsMountPoints[twt].path);
            for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
                if (DRStricmp(gTwatVfsMountPoints[twt].header->fileHeaders[i].filename, &pPath[twt_path_len + 1]) == 0) {
                    tU8* data = gTwatVfsMountPoints[twt].data + gTwatVfsMountPoints[twt].header->fileHeaders[i].data_offset;
                    for (file_index = 1; file_index < (int)CARPOCALYPSE2_ASIZE(gTwatVfsFiles); file_index++) {
                        if (gTwatVfsFiles[file_index].start != NULL) {
                            continue;
                        }
                        gTwatVfsFiles[file_index].start = data;
                        gTwatVfsFiles[file_index].pos = data;
                        gTwatVfsFiles[file_index].end = data + gTwatVfsMountPoints[twt].header->fileHeaders[i].fileSize;
                        gTwatVfsFiles[file_index].error = 0;
                        return (FILE*)(file_index);
                    }
                    return NULL;
                }
            }
        }
    }
    {
        FILE* disk = fopen(pPath, mode);
        if (disk != NULL) {
            return disk;
        }
#if defined(CARPOCALYPSE2_FIX_BUGS) && !defined(_WIN32)
        return CI_FopenCaseInsensitive(pPath, mode);
#else
        return NULL;
#endif
    }
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            mov ebx, ecx
            push esi
            push edi
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], ebx
            mov esi, offset gTwatVfsMountPoints+0x104
    L004b4796:
            mov eax, dword ptr [esi - 0x104]
            test eax, eax
            je L004b47ef
            lea edi, [esi - 0x100]
            push edi
            push ebx
            call strstr
            add esp, 8
            cmp eax, ebx
            jne L004b47ef
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov eax, dword ptr [esi]
            xor edi, edi
            not ecx
            dec ecx
            lea ebp, [ecx + ebx + 1]
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            jle L004b47ef
            xor ebx, ebx
    L004b47cf:
            mov ecx, dword ptr [esi]
            mov edx, ebp
            lea ecx, [ecx + ebx + 0xc]
            call DRStricmp
            test eax, eax
            je L004b4813
            mov edx, dword ptr [esi]
            inc edi
            add ebx, 0x38
            cmp edi, dword ptr [edx + 4]
            jl L004b47cf
            mov ebx, dword ptr [esp + 0x10]
    L004b47ef:
            add esi, 0x108
            cmp esi, offset gTwatVfsFiles+0xfc
            jl L004b4796
            mov eax, dword ptr [esp + 0x14]
            push eax
            push ebx
            call fopen
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L004b4813:
            xor ecx, ecx
            mov eax, offset gTwatVfsFiles+0x10
    L004b481a:
            cmp dword ptr [eax], 0
            je L004b4834
            add eax, 0x10
            inc ecx
            cmp eax, offset gDisableTiffConversionStackPos
            jl L004b481a
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L004b4834:
            mov edx, dword ptr [esi]
            mov eax, edi
            shl eax, 3
            sub eax, edi
            pop edi
            mov esi, dword ptr [edx + eax*8 + 0x3c]
            lea edx, [edx + eax*8]
            mov eax, ecx
            shl eax, 4
            mov dword ptr [eax + gTwatVfsFiles+0x10], esi
            mov dword ptr [eax + gTwatVfsFiles+0x14], esi
            mov edx, dword ptr [edx + 8]
            mov esi, dword ptr [eax + gTwatVfsFiles+0x10]
            add edx, esi
            pop esi
            mov dword ptr [eax + gTwatVfsFiles+0x18], edx
            mov dword ptr [eax + gTwatVfsFiles+0x1c], 0
            pop ebp
            lea eax, [ecx + 1]
            pop ebx
            add esp, 8
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4880
int C2_NAKED C2_HOOK_FASTCALL PFfgetc(FILE* pFile) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;
    int result;

    if ((intptr_t)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fgetc(pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    if (twtFile->pos >= twtFile->end) {
        twtFile->error = -1;
        return EOF;
    }
    twtFile->error = 0;
    result = *(tS8*)twtFile->pos;
    twtFile->pos++;
    return result;
#else
    __asm {
            cmp ecx, 0x33
            jb L004b488f
            push ecx
            call fgetc
            add esp, 4
            ret
    L004b488f:
            shl ecx, 4
            mov edx, dword ptr [ecx + gTwatVfsFiles+0x4]
            mov eax, dword ptr [ecx + gTwatVfsFiles+0x8]
            lea ecx, [ecx + gTwatVfsFiles]
            cmp edx, eax
            jb L004b48af
            or eax, 0xffffffff
            mov dword ptr [ecx + 0xc], eax
            ret
    L004b48af:
            mov dword ptr [ecx + 0xc], 0
            movsx eax, byte ptr [edx]
            inc edx
            mov dword ptr [ecx + 4], edx
            ret
            nop
            nop
    }
#endif
}

// PFgetc

// FUNCTION: CARMA2_HW 0x004b4900
C2_NAKED char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;
    int c;
    char* writePtr;
    int i;

    if ((intptr_t)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fgets(buffer, size, pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    writePtr = buffer;
    for (i = 0; i < (int)size - 1; i++) {
        c = (int)*twtFile->pos++;
        if (c == -1) {
            *writePtr = '\0';
            twtFile->error = -1;
            return NULL;
        }
        *writePtr++ = c;
        if (c == '\n') {
            break;
        }
        if (twtFile->pos >= twtFile->end) {
            break;
        }
    }
    *writePtr = '\0';
    twtFile->error = 0;
    return buffer;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x10]
            cmp esi, 0x33
            push edi
            jb L004b491f
            push esi
            push edx
            push ecx
            call fgets
            add esp, 0xc
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    L004b491f:
            shl esi, 4
            lea eax, [edx - 1]
            xor ebp, ebp
            test eax, eax
            mov ebx, ecx
            lea edi, [esi + gTwatVfsFiles]
            jle L004b4986
    L004b4933:
            mov esi, dword ptr [edi + 4]
            movsx edx, byte ptr [esi]
            inc esi
            cmp edx, -1
            mov dword ptr [edi + 4], esi
            je L004b4973
            mov byte ptr [ebx], dl
            inc ebx
            cmp dl, 0xa
            je L004b4986
            mov edx, dword ptr [edi + 4]
            mov esi, dword ptr [edi + 8]
            cmp edx, esi
            sbb edx, edx
            inc edx
            mov dword ptr [esp + 0x14], edx
            jne L004b4986
            inc ebp
            cmp ebp, eax
            jl L004b4933
            mov byte ptr [ebx], 0
            mov dword ptr [edi + 0xc], 0
            mov eax, ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    L004b4973:
            mov byte ptr [ebx], 0
            mov dword ptr [edi + 0xc], 0xffffffff
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    L004b4986:
            mov byte ptr [ebx], 0
            mov dword ptr [edi + 0xc], 0
            pop edi
            pop esi
            pop ebp
            mov eax, ecx
            pop ebx
            ret 4
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

// FUNCTION: CARMA2_HW 0x004b49a0
int C2_NAKED C2_HOOK_FASTCALL PFungetc(int ch, FILE* pFile) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;

    if ((uintptr_t)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return ungetc(ch, pFile);
    }
    twtFile = &gTwatVfsFiles[(intptr_t)pFile];
    if (twtFile->pos > twtFile->start && ch != -1) {
        twtFile->pos--;
        *twtFile->pos = ch;
        twtFile->error = 0;
        return ch;
    } else {
        twtFile->error = -1;
        return -1;
    }
#else
    __asm {
            cmp edx, 0x33
            push esi
            mov eax, ecx
            jb L004b49b4
            push edx
            push eax
            call ungetc
            add esp, 8
            pop esi
            ret
    L004b49b4:
            shl edx, 4
            mov ecx, dword ptr [edx + gTwatVfsFiles+0x4]
            mov esi, dword ptr [edx + gTwatVfsFiles]
            lea edx, [edx + gTwatVfsFiles]
            cmp ecx, esi
            jbe L004b49e3
            cmp eax, -1
            je L004b49e3
            lea esi, [ecx - 1]
            mov dword ptr [edx + 4], esi
            mov byte ptr [esi], al
            mov dword ptr [edx + 0xc], 0
            pop esi
            ret
    L004b49e3:
            mov dword ptr [edx + 0xc], 0xffffffff
            or eax, 0xffffffff
            pop esi
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b49f0
br_size_t C2_NAKED C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f) {
#ifndef CARPOCALYPSE2_MATCHING
    int read_size;
    tU8 *pos;
    tU8 *end;

    if ((uintptr_t)f >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles) + 1) {
        return fread(buf, size, n, f);
    }
    read_size = size * n;
    end = gTwatVfsFiles[(uintptr_t)f].end;
    pos = gTwatVfsFiles[(uintptr_t)f].pos;
    if (end - pos >= read_size) {
        end = pos + read_size;
    } else {
        n = ((end - pos) / size) * size;
        read_size = size * n;//
    }
    memcpy(buf, gTwatVfsFiles[(uintptr_t)f].pos, read_size);
    gTwatVfsFiles[(uintptr_t)f].pos = end;
    gTwatVfsFiles[(uintptr_t)f].error = 0;
    return n;
#else
    __asm {
            push ecx
            mov eax, dword ptr [esp + 0xc]
            push ebx
            cmp eax, 0x33
            mov ebx, edx
            mov dword ptr [esp + 4], ecx
            jb L004b4a16
            push eax
            mov eax, dword ptr [esp + 0x10]
            push eax
            push ebx
            push ecx
            call fread
            add esp, 0x10
            pop ebx
            pop ecx
            ret 8
    L004b4a16:
            mov edx, dword ptr [esp + 0xc]
            push edi
            mov ecx, edx
            push esi
            shl eax, 4
            imul ecx, ebx
            mov edi, dword ptr [eax + gTwatVfsFiles+0x8]
            mov esi, dword ptr [eax + gTwatVfsFiles+0x4]
            push ebp
            lea ebp, [eax + gTwatVfsFiles]
            mov eax, edi
            sub eax, esi
            cmp eax, ecx
            jl L004b4a46
            mov eax, edx
            lea edx, [ecx + esi]
            jmp L004b4a54
    L004b4a46:
            xor edx, edx
            div ebx
            mov edx, edi
            imul eax, ebx
            mov ecx, eax
            imul ecx, ebx
    L004b4a54:
            mov edi, dword ptr [esp + 0x10]
            mov ebx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
            mov dword ptr [ebp + 4], edx
            mov dword ptr [ebp + 0xc], 0
            pop ebp
            pop esi
            pop edi
            pop ebx
            pop ecx
            ret 8
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

// FUNCTION: CARMA2_HW 0x004b4a80
int C2_HOOK_FASTCALL PFfwrite(const void* buf, br_size_t size, unsigned int n, void* f) {

    if ((uintptr_t)f >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fwrite(buf, size, n, f);
    }
    FatalError(kFatalError_WriteAttemptToPackedFile_S, "unknown");
}

// FUNCTION: CARMA2_HW 0x004b4b00
int C2_NAKED C2_HOOK_FASTCALL PFftell(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    int pos;

    if ((uintptr_t)pF >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return ftell(pF);
    }
    pos = gTwatVfsFiles[(uintptr_t)pF].pos - gTwatVfsFiles[(uintptr_t)pF].start;
    gTwatVfsFiles[(uintptr_t)pF].error = 0;
    return pos;
#else
    __asm {
            cmp ecx, 0x33
            jb L004b4b0f
            push ecx
            call ftell
            add esp, 4
            ret
    L004b4b0f:
            shl ecx, 4
            mov eax, dword ptr [ecx + gTwatVfsFiles+0x4]
            mov edx, dword ptr [ecx + gTwatVfsFiles]
            lea ecx, [ecx + gTwatVfsFiles]
            sub eax, edx
            mov dword ptr [ecx + 0xc], 0
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4b70
int C2_NAKED C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;
    tU8 *newpos;

    if ((intptr_t)pF >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fseek(pF, offset, whence);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pF];
    switch (whence) {
    case SEEK_SET:
        newpos = twtFile->start + offset;
        break;
    case SEEK_END:
        newpos = twtFile->end + offset;
        break;
    case SEEK_CUR:
        newpos = twtFile->pos + offset;
        break;
    }
    if (newpos >= twtFile->start && newpos <= twtFile->end) {
        twtFile->pos = newpos;
        twtFile->error = 0;
        return 0;
    } else {
        twtFile->error = -1;
        return -1;
    }
#else
    __asm {
            cmp ecx, 0x33
            jb L004b4b87
            mov eax, dword ptr [esp + 4]
            push eax
            push edx
            push ecx
            call fseek
            add esp, 0xc
            ret 4
    L004b4b87:
            mov eax, dword ptr [esp + 4]
            shl ecx, 4
            sub eax, 0
            lea ecx, [ecx + gTwatVfsFiles]
            je L004b4bad
            dec eax
            je L004b4ba6
            dec eax
            jne L004b4bb3
            mov eax, dword ptr [ecx + 8]
            add eax, edx
            jmp L004b4bb7
    L004b4ba6:
            mov eax, dword ptr [ecx + 4]
            add eax, edx
            jmp L004b4bb7
    L004b4bad:
            add edx, dword ptr [ecx]
            mov eax, edx
            jmp L004b4bb7
    L004b4bb3:
            mov eax, dword ptr [esp + 4]
    L004b4bb7:
            cmp eax, dword ptr [ecx]
            jb L004b4bcf
            cmp eax, dword ptr [ecx + 8]
            ja L004b4bcf
            mov dword ptr [ecx + 4], eax
            mov dword ptr [ecx + 0xc], 0
            xor eax, eax
            ret 4
    L004b4bcf:
            or eax, 0xffffffff
            mov dword ptr [ecx + 0xc], eax
            ret 4
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

// FUNCTION: CARMA2_HW 0x004b4be0
void C2_NAKED C2_HOOK_FASTCALL PFrewind(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;

    if ((int)pF >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        rewind(pF);
    } else {
        twtFile = &gTwatVfsFiles[(uintptr_t)pF];
        twtFile->error = 0;
        twtFile->pos = gTwatVfsFiles[(uintptr_t)pF].start;
    }
#else
    __asm {
            cmp ecx, 0x33
            jb L004b4bef
            push ecx
            call rewind
            add esp, 4
            ret
    L004b4bef:
            shl ecx, 4
            lea eax, [ecx + gTwatVfsFiles]
            mov ecx, dword ptr [ecx + gTwatVfsFiles]
            mov dword ptr [eax + 0xc], 0
            mov dword ptr [eax + 4], ecx
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

// FUNCTION: CARMA2_HW 0x004b4c10
int C2_NAKED C2_HOOK_FASTCALL PFfeof(FILE* pFile) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;

    if ((int)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return feof(pFile);
    } else {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        return twtFile->pos >= gTwatVfsFiles[(uintptr_t)pFile].end;
    }
#else
    __asm {
            cmp ecx, 0x33
            jb L004b4c1c
            mov eax, dword ptr [ecx + 0xc]
            and eax, 0x10
            ret
    L004b4c1c:
            shl ecx, 4
            mov edx, dword ptr [ecx + gTwatVfsFiles+0x8]
            lea eax, [ecx + gTwatVfsFiles]
            mov ecx, dword ptr [ecx + gTwatVfsFiles+0x4]
            cmp ecx, edx
            sbb eax, eax
            inc eax
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

// FUNCTION: CARMA2_HW 0x004b4c80
void C2_NAKED C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
#ifndef CARPOCALYPSE2_MATCHING
    int twt;
    int i;
    char buffer[256];

    for (twt = 0; twt < (int)CARPOCALYPSE2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header == NULL) {
            continue;
        }
        if (DRStricmp(pThe_path, gTwatVfsMountPoints[twt].path) != 0) {
            continue;
        }
        for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
            PathCat(buffer, pThe_path, gTwatVfsMountPoints[twt].header->fileHeaders[i].filename);
            pAction_routine(buffer);
        }
        return;
    }
    PDForEveryFile(pThe_path, pAction_routine);
#else
    __asm {
            sub esp, 0x104
            push ebx
            push ebp
            push esi
            mov ebx, edx
            push edi
            mov dword ptr [esp + 0x10], ebx
            mov ebp, ecx
            xor edi, edi
            mov esi, offset gTwatVfsMountPoints+0x4
    L004b4c99:
            mov eax, dword ptr [esi - 4]
            test eax, eax
            je L004b4cad
            mov edx, esi
            mov ecx, ebp
            call DRStricmp
            test eax, eax
            je L004b4cd0
    L004b4cad:
            add esi, 0x108
            inc edi
            cmp esi, offset gDisableTiffConversionStack+0x4
            jl L004b4c99
            mov edx, ebx
            mov ecx, ebp
            call PDForEveryFile
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x104
            ret
    L004b4cd0:
            mov eax, edi
            xor esi, esi
            shl eax, 5
            add eax, edi
            lea ebx, [eax*8 + gTwatVfsMountPoints]
            mov eax, dword ptr [eax*8 + gTwatVfsMountPoints+0x104]
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            jle L004b4d22
            xor edi, edi
    L004b4cf0:
            lea ecx, [edi + eax + 0xc]
            mov edx, ebp
            push ecx
            lea ecx, [esp + 0x18]
            call PathCat
            lea ecx, [esp + 0x14]
            call dword ptr [esp + 0x10]
            mov eax, dword ptr [ebx + 0x104]
            inc esi
            add edi, 0x38
            cmp esi, dword ptr [eax + 4]
            jl L004b4cf0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x104
            ret
    L004b4d22:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x104
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4d30
void C2_NAKED C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data) {
#ifndef CARPOCALYPSE2_MATCHING
    int twt;
    int i;
    tPath_name twt_filePath;

    for (twt = 0; twt < (int)CARPOCALYPSE2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header != NULL && DRStricmp(gTwatVfsMountPoints[twt].path, path) == 0) {
            for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
                PathCat(twt_filePath, path, gTwatVfsMountPoints[twt].header->fileHeaders[i].filename);
                pCallback(twt_filePath, data);
            }
            return;
        }
    }
    PDEnumPath(path, pCallback, data);
#else
    __asm {
            sub esp, 0x108
            push ebx
            push ebp
            push esi
            mov ebp, edx
            mov ebx, ecx
            push edi
            mov dword ptr [esp + 0x10], ebp
            mov dword ptr [esp + 0x14], ebx
            xor edi, edi
            mov esi, offset gTwatVfsMountPoints+0x4
    L004b4d4d:
            mov eax, dword ptr [esi - 4]
            test eax, eax
            je L004b4d61
            mov edx, esi
            mov ecx, ebx
            call DRStricmp
            test eax, eax
            je L004b4d8e
    L004b4d61:
            add esi, 0x108
            inc edi
            cmp esi, offset gDisableTiffConversionStack+0x4
            jl L004b4d4d
            mov edx, dword ptr [esp + 0x11c]
            mov ecx, ebx
            push edx
            mov edx, ebp
            call PDEnumPath
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 4
    L004b4d8e:
            mov eax, edi
            xor esi, esi
            shl eax, 5
            add eax, edi
            lea ebx, [eax*8 + gTwatVfsMountPoints]
            mov eax, dword ptr [eax*8 + gTwatVfsMountPoints+0x104]
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            jle L004b4de0
            mov ebp, dword ptr [esp + 0x11c]
            xor edi, edi
    L004b4db5:
            mov edx, dword ptr [esp + 0x14]
            lea ecx, [edi + eax + 0xc]
            push ecx
            lea ecx, [esp + 0x1c]
            call PathCat
            mov edx, ebp
            lea ecx, [esp + 0x18]
            call dword ptr [esp + 0x10]
            mov eax, dword ptr [ebx + 0x104]
            inc esi
            add edi, 0x38
            cmp esi, dword ptr [eax + 4]
            jl L004b4db5
    L004b4de0:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x108
            ret 4
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4df0
tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path) {
    tTWTVFS res;

    res = OpenPackFile(path);
    if (res >= 0) {
        gDisableTiffConversionStack[gDisableTiffConversionStackPos] = gDisableTiffConversion;
        gDisableTiffConversionStackPos++;
        gDisableTiffConversion = 1;
    }
    return res;
}

// FUNCTION: CARMA2_HW 0x004b4e20
void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt) {
    if (twt >= 0) {
        gDisableTiffConversionStackPos -= 1;
        gDisableTiffConversion = gDisableTiffConversionStack[gDisableTiffConversionStackPos];
        ClosePackFile(twt);
    }
}

// FUNCTION: CARMA2_HW 0x004b4e60
void C2_NAKED C2_HOOK_FASTCALL PackFileRevertTiffLoading(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int count;

    count = gDisableTiffConversionStackPos;
    if (count != 0) {
 // or > 0 if it is unsigned
        gDisableTiffConversionStack[count] = gDisableTiffConversion;
        gDisableTiffConversion = gDisableTiffConversionStack[count - 1];
    }
#else
    __asm {
            mov eax, dword ptr [gDisableTiffConversionStackPos]
            test eax, eax
            je L004b4e83
            mov ecx, dword ptr [gDisableTiffConversion]
            mov dword ptr [eax*4 + gDisableTiffConversionStack], ecx
            mov edx, dword ptr [eax*4 + gTwatVfsMountPoints+0x524]
            mov dword ptr [gDisableTiffConversion], edx
    L004b4e83:
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4e90
void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void) {
    if (gDisableTiffConversionStackPos != 0) {
        gDisableTiffConversion = gDisableTiffConversionStack[gDisableTiffConversionStackPos];
    }
}
// FUNCTION: CARMA2_HW 0x004b4eb0
void C2_HOOK_FASTCALL PackfileNoOp(void) {
}
