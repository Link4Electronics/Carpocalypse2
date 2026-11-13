#include "pmfile.h"

#include "pmmem.h"
#include "pmdsptch.h"

#include "core/fw/datafile.h"
#include "core/fw/diag.h"
#include "core/fw/genfile.h"
#include "core/fw/resource.h"

#include "core/v1db/chunkids.h"
#include "core/v1db/stackids.h"

#include "c2_string.h"

#include <stddef.h>

// GLOBAL: CARMA2_HW 0x0066d7c8
br_file_enum_member pixelmap_type_FM[] = {
    { BR_PMT_INDEX_1, "BR_PMT_INDEX_1", },
    { BR_PMT_INDEX_2, "BR_PMT_INDEX_2", },
    { BR_PMT_INDEX_4, "BR_PMT_INDEX_4", },
    { BR_PMT_INDEX_8, "BR_PMT_INDEX_8", },
    { BR_PMT_RGB_555, "BR_PMT_RGB_555", },
    { BR_PMT_RGB_565, "BR_PMT_RGB_565", },
    { BR_PMT_RGB_888, "BR_PMT_RGB_888", },
    { BR_PMT_RGBX_888, "BR_PMT_RGBX_888", },
    { BR_PMT_RGBA_8888, "BR_PMT_RGBA_8888", },
    { BR_PMT_YUYV_8888, "BR_PMT_YUYV_8888", },
    { BR_PMT_YUV_888, "BR_PMT_YUV_888", },
    { BR_PMT_DEPTH_16, "BR_PMT_DEPTH_16", },
    { BR_PMT_DEPTH_32, "BR_PMT_DEPTH_32", },
    { BR_PMT_ALPHA_8, "BR_PMT_ALPHA_8", },
    { BR_PMT_INDEXA_88, "BR_PMT_INDEXA_88", },
};

// GLOBAL: CARMA2_HW 0x0066d840
br_file_enum pixelmap_type_F ={ BR_ASIZE(pixelmap_type_FM), pixelmap_type_FM };

// GLOBAL: CARMA2_HW 0x0066d848
br_file_struct_member br_old_pixelmap_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_pixelmap, type), "type", &pixelmap_type_F },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, row_bytes), "row_bytes", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, width), "width", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, height), "height", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_x), "origin_x", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_y), "origin_y", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_pixelmap, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x0066d8b8
br_file_struct_member br_pixelmap_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_pixelmap, type), "type", &pixelmap_type_F },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, row_bytes), "row_bytes", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, width), "width", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, height), "height", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_x), "origin_x", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_y), "origin_y", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, mip_offset), "mip_offset", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_pixelmap, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x0066d938
br_file_struct br_pixelmap_F = { "br_pixelmap", BR_ASIZE(br_pixelmap_FM), br_pixelmap_FM, sizeof(br_pixelmap) };

// GLOBAL: CARMA2_HW 0x0066d948
br_file_struct br_old_pixelmap_F = { "br_old_pixelmap", BR_ASIZE(br_old_pixelmap_FM), br_old_pixelmap_FM, sizeof(br_pixelmap) };

// GLOBAL: CARMA2_HW 0x0066d958
br_chunks_table_entry PixelmapLoadEntries[5] = {
    { CHUNKID_END, 0u, FopRead_END },
    { CHUNKID_PIXELMAP, 0u, FopRead_PIXELMAP },
    { CHUNKID_PIXELMAP_PIXELS, 0u, FopRead_PIXELS },
    { CHUNKID_PIXELMAP_ADD_MAP, 0u, FopRead_ADD_MAP },
    { CHUNKID_OLD_PIXELMAP, 0u, FopRead_OLD_PIXELMAP },
};
// GLOBAL: CARMA2_HW 0x0066d998
br_chunks_table PixelmapLoadTable = { BR_ASIZE(PixelmapLoadEntries), PixelmapLoadEntries };

int C2_HOOK_STDCALL FopWrite_PIXELMAP(br_datafile* df, br_pixelmap* pixelmap) {
    br_pixelmap pmap;

    memcpy(&pmap, pixelmap, sizeof(br_pixelmap));
    pmap.row_bytes = (pmTypeInfo[pmap.type].bits >> 3) * pmap.width;
    df->prims->chunk_write(df, CHUNKID_PIXELMAP, df->prims->struct_size(df, &br_pixelmap_F, &pmap));
    df->prims->struct_write(df, &br_pixelmap_F, &pmap);
    return 0;
}

#ifdef CARPOCALYPSE2_MATCHING
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectLock();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectUnlock();
extern void C2_HOOK_STDCALL DfClose(br_datafile* df);
extern br_datafile* C2_HOOK_STDCALL DfOpen(const char* name, int write, br_token scalar_type);
extern int C2_HOOK_STDCALL FopWrite_END(br_datafile* df);
extern int C2_HOOK_STDCALL FopWrite_FILE_INFO(br_datafile* df, br_uint_32 type);
extern pm_type_info pmTypeInfo[32];
#endif
// FUNCTION: CARMA2_HW 0x00538da0
int C2_HOOK_STDCALL FopRead_OLD_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;

    pp = (br_pixelmap*)DevicePixelmapMemAllocate(BR_PMAF_NO_PIXELS | BR_PMAF_INVERTED, 0, 0, 0, 2);
    df->res = pp;
    df->prims->struct_read(df, &br_old_pixelmap_F, pp);
    df->res = NULL;
    pp->row_bytes = (pmTypeInfo[pp->type].bits >> 3) * pp->width;
    DfPush(DF_PIXELMAP, pp, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538e00
int C2_HOOK_STDCALL FopRead_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;

    pp = (br_pixelmap*)DevicePixelmapMemAllocate(BR_PMAF_NO_PIXELS | BR_PMAF_INVERTED, 0, 0, 0, 2);
    df->res = pp;
    df->prims->struct_read(df, &br_pixelmap_F, pp);
    df->res = NULL;
    pp->row_bytes = (pmTypeInfo[pp->type].bits >> 3) * pp->width;
    DfPush(DF_PIXELMAP, pp, 1);
    return 0;
}

static int FopWrite_PIXELS(br_datafile* df, br_pixelmap* pixelmap) {
    int size = pmTypeInfo[pixelmap->type].file_size;
    int bytes = pmTypeInfo[pixelmap->type].bits >> 3;
    int block_count = (pixelmap->width * bytes) / size;
    char* pixels;

    pixels = (char*)pixelmap->pixels + pixelmap->base_y * pixelmap->row_bytes + pixelmap->base_x * pixelmap->width;
    if (pixelmap->mip_offset != 0) {
        int mipSize = 0;
        int currentMipSize;

        for (currentMipSize = pixelmap->width; currentMipSize != 0; currentMipSize >>= 1) {
            mipSize += currentMipSize * currentMipSize * bytes;
        }
        df->prims->chunk_write(df, CHUNKID_PIXELMAP_PIXELS, df->prims->block_size(df, pixels, mipSize / size, mipSize, 1, size));
        df->prims->block_write(df, pixels, mipSize, mipSize, 1, size);
    } else {
        df->prims->chunk_write(df, CHUNKID_PIXELMAP_PIXELS, df->prims->block_size(df, pixels, block_count, pixelmap->row_bytes, pixelmap->height, size));
        df->prims->block_write(df, pixels, block_count, pixelmap->row_bytes, pixelmap->height, size);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538e60
int C2_HOOK_STDCALL FopRead_PIXELS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    int icount;
    br_pixelmap* pp;
    int size;

    pp = DfTop(DF_PIXELMAP, NULL);
    size = pmTypeInfo[pp->type].file_size;
    df->res = pp;
    pp->pixels = df->prims->block_read(df, NULL, &icount, size, BR_MEMORY_PIXELS);
    pp->flags |= BR_PMF_LINEAR;
    df->res = NULL;
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ADD_MAP(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_PIXELMAP_ADD_MAP, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538ec0
int C2_HOOK_STDCALL FopRead_ADD_MAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;
    br_pixelmap* map;

    map = DfPop(DF_PIXELMAP, 0);
    pp = DfTop(DF_PIXELMAP, 0);
    pp->map = map;
    BrResAdd(pp, map);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538ef0
br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany(const char* filename, br_pixelmap** pixelmaps, br_uint_16 num) {
    br_datafile* df;
    int count;
    int r;

    df = DfOpen(filename, 0, BRT_FIXED);
    if (df == NULL) {
        return 0;
    }
    for (count = 0; count < num; ) {
        r = DfChunksInterpret(df, &PixelmapLoadTable);
        if (DfTopType() == DF_PIXELMAP) {
            pixelmaps[count++] = DfPop(DF_PIXELMAP, 0);
        }
        if (r == 0) {
            break;
        }
    }
    DfClose(df);
    return count;
}

// FUNCTION: CARMA2_HW 0x00539170
int C2_NAKED C2_HOOK_STDCALL WritePixelmap(br_pixelmap* pp, br_datafile* df) {
#ifndef CARPOCALYPSE2_MATCHING

    BrPixelmapDirectLock(pp, 1);
#ifdef BR_FIX_BUGS
    if (pp->pixels == NULL) {
        _BrAssert("pp->pixels", "pmfile.c", 291);
    }
#endif
    FopWrite_PIXELMAP(df, pp);
    if (pp->map != NULL) {
        WritePixelmap(pp->map, df);
        FopWrite_ADD_MAP(df);
    }
    FopWrite_PIXELS(df, pp);
    BrPixelmapDirectUnlock(pp);
    return 0;
#else
    __asm {
    L00539170:
            sub esp, 0x48
            push ebx
            mov ebx, dword ptr [esp + 0x50]
            push ebp
            push esi
            push edi
            push 1
            push ebx
            call BrPixelmapDirectLock
            mov ecx, 0x11
            mov esi, ebx
            lea edi, [esp + 0x1c]
            add esp, 8
            rep movsd
            mov eax, dword ptr [esp + 0x40]
            mov esi, dword ptr [esp + 0x60]
            and eax, 0xff
            lea edx, [esp + 0x14]
            mov edi, dword ptr [esi + 4]
            push edx
            mov cx, word ptr [eax*8 + pmTypeInfo]
            push offset br_pixelmap_F
            shr cx, 3
            imul ecx, dword ptr [esp + 0x50]
            push esi
            mov word ptr [esp + 0x48], cx
            call dword ptr [edi + 0x24]
            push eax
            push 0x3d
            push esi
            call dword ptr [edi + 8]
            mov eax, dword ptr [esi + 4]
            lea ecx, [esp + 0x14]
            push ecx
            push offset br_pixelmap_F
            push esi
            call dword ptr [eax + 0x1c]
            mov eax, dword ptr [ebx + 0x10]
            test eax, eax
            je L005391f8
            push esi
            push eax
            call L00539170
            mov edx, dword ptr [esi + 4]
            push 0
            push 0x22
            push esi
            call dword ptr [edx + 8]
    L005391f8:
            xor eax, eax
            xor ecx, ecx
            mov al, byte ptr [ebx + 0x2c]
            xor ebp, ebp
            shl eax, 3
            xor edx, edx
            xor edi, edi
            mov cx, word ptr [eax + pmTypeInfo]
            mov bp, word ptr [eax + pmTypeInfo+0x2]
            mov dx, word ptr [ebx + 0x34]
            mov di, word ptr [ebx + 0x32]
            shr ecx, 3
            mov eax, ecx
            mov dword ptr [esp + 0x60], edx
            imul eax, edx
            cdq
            idiv ebp
            movsx edx, word ptr [ebx + 0x28]
            imul edi, edx
            mov dword ptr [esp + 0x5c], eax
            xor eax, eax
            mov ax, word ptr [ebx + 0x30]
            imul eax, ecx
            add edi, eax
            mov eax, dword ptr [ebx + 8]
            add edi, eax
            cmp word ptr [ebx + 0x2a], 0
            je L005392be
            mov eax, dword ptr [esp + 0x60]
            mov dword ptr [esp + 0x5c], 0
            test eax, eax
            mov dword ptr [esp + 0x60], eax
            je L00539281
    L00539263:
            mov edx, eax
            imul edx, eax
            imul edx, ecx
            mov eax, dword ptr [esp + 0x5c]
            add eax, edx
            mov dword ptr [esp + 0x5c], eax
            mov eax, dword ptr [esp + 0x60]
            sar eax, 1
            mov dword ptr [esp + 0x60], eax
            jne L00539263
    L00539281:
            mov eax, dword ptr [esp + 0x5c]
            mov ecx, dword ptr [esi + 4]
            cdq
            idiv ebp
            mov edx, dword ptr [esp + 0x5c]
            push ebp
            push 1
            push edx
            mov dword ptr [esp + 0x6c], ecx
            push eax
            push edi
            push esi
            mov dword ptr [esp + 0x28], eax
            call dword ptr [ecx + 0x30]
            push eax
            push 0x21
            mov eax, dword ptr [esp + 0x68]
            push esi
            call dword ptr [eax + 8]
            mov ecx, dword ptr [esi + 4]
            push ebp
            mov edx, dword ptr [esp + 0x60]
            mov eax, dword ptr [esp + 0x14]
            push 1
            push edx
            push eax
            jmp L005392f8
    L005392be:
            xor ecx, ecx
            mov eax, dword ptr [esi + 4]
            mov cx, word ptr [ebx + 0x36]
            push ebp
            push ecx
            push edx
            mov edx, dword ptr [esp + 0x68]
            mov dword ptr [esp + 0x6c], eax
            push edx
            push edi
            push esi
            call dword ptr [eax + 0x30]
            push eax
            push 0x21
            mov eax, dword ptr [esp + 0x68]
            push esi
            call dword ptr [eax + 8]
            movsx eax, word ptr [ebx + 0x28]
            mov ecx, dword ptr [esi + 4]
            xor edx, edx
            mov dx, word ptr [ebx + 0x36]
            push ebp
            push edx
            mov edx, dword ptr [esp + 0x64]
            push eax
            push edx
    L005392f8:
            push edi
            push esi
            call dword ptr [ecx + 0x28]
            push ebx
            call BrPixelmapDirectUnlock
            add esp, 4
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x48
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00538f60
br_uint_32 C2_NAKED C2_HOOK_CDECL BrPixelmapSaveMany(const char* filename, br_pixelmap** pixelmaps, br_uint_16 num) {
#ifndef CARPOCALYPSE2_MATCHING
    br_datafile* df;
    int i;

    df = DfOpen(filename, 1, BRT_FIXED);
    if (df == NULL) {
        return 0;
    }
    FopWrite_FILE_INFO(df, 2);
    for (i = 0; i < num; i++) {
        WritePixelmap(pixelmaps[i], df);
        FopWrite_END(df);
    }
    DfClose(df);
    return num;
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            sub esp, 0x50
            push ebx
            push 0xb
            push 1
            push eax
            call DfOpen
            mov ebx, eax
            test ebx, ebx
            jne L00538f7d
            pop ebx
            add esp, 0x50
            ret
    L00538f7d:
            push esi
            push 2
            push ebx
            call FopWrite_FILE_INFO
            mov esi, dword ptr [esp + 0x64]
            and esi, 0xffff
            mov dword ptr [esp + 0x10], esi
            jle L0053915b
            mov ecx, dword ptr [esp + 0x60]
            push edi
            push ebp
            mov dword ptr [esp + 0x68], ecx
            mov dword ptr [esp + 0x10], esi
    L00538fa8:
            mov edx, dword ptr [esp + 0x68]
            push 1
            mov ebp, dword ptr [edx]
            push ebp
            call BrPixelmapDirectLock
            mov ecx, 0x11
            mov esi, ebp
            lea edi, [esp + 0x24]
            add esp, 8
            rep movsd
            mov eax, dword ptr [esp + 0x48]
            mov esi, dword ptr [ebx + 4]
            and eax, 0xff
            lea edx, [esp + 0x1c]
            push edx
            push offset br_pixelmap_F
            mov cx, word ptr [eax*8 + pmTypeInfo]
            push ebx
            shr cx, 3
            imul ecx, dword ptr [esp + 0x5c]
            mov word ptr [esp + 0x50], cx
            call dword ptr [esi + 0x24]
            push eax
            push 0x3d
            push ebx
            call dword ptr [esi + 8]
            mov eax, dword ptr [ebx + 4]
            lea ecx, [esp + 0x1c]
            push ecx
            push offset br_pixelmap_F
            push ebx
            call dword ptr [eax + 0x1c]
            mov eax, dword ptr [ebp + 0x10]
            test eax, eax
            je L00539027
            push ebx
            push eax
            call WritePixelmap
            mov edx, dword ptr [ebx + 4]
            push 0
            push 0x22
            push ebx
            call dword ptr [edx + 8]
    L00539027:
            xor eax, eax
            xor ecx, ecx
            mov al, byte ptr [ebp + 0x2c]
            xor edi, edi
            shl eax, 3
            xor edx, edx
            xor esi, esi
            mov cx, word ptr [eax + pmTypeInfo]
            mov di, word ptr [eax + pmTypeInfo+0x2]
            mov dx, word ptr [ebp + 0x34]
            mov si, word ptr [ebp + 0x32]
            shr ecx, 3
            mov eax, ecx
            mov dword ptr [esp + 0x6c], edx
            imul eax, edx
            cdq
            idiv edi
            movsx edx, word ptr [ebp + 0x28]
            imul esi, edx
            mov dword ptr [esp + 0x64], eax
            xor eax, eax
            mov ax, word ptr [ebp + 0x30]
            imul eax, ecx
            add esi, eax
            mov eax, dword ptr [ebp + 8]
            add esi, eax
            cmp word ptr [ebp + 0x2a], 0
            je L005390ed
            mov eax, dword ptr [esp + 0x6c]
            mov dword ptr [esp + 0x64], 0
            test eax, eax
            mov dword ptr [esp + 0x6c], eax
            je L005390b0
    L00539092:
            mov edx, eax
            imul edx, eax
            imul edx, ecx
            mov eax, dword ptr [esp + 0x64]
            add eax, edx
            mov dword ptr [esp + 0x64], eax
            mov eax, dword ptr [esp + 0x6c]
            sar eax, 1
            mov dword ptr [esp + 0x6c], eax
            jne L00539092
    L005390b0:
            mov eax, dword ptr [esp + 0x64]
            mov ecx, dword ptr [ebx + 4]
            cdq
            idiv edi
            mov edx, dword ptr [esp + 0x64]
            push edi
            push 1
            push edx
            mov dword ptr [esp + 0x78], ecx
            push eax
            push esi
            push ebx
            mov dword ptr [esp + 0x2c], eax
            call dword ptr [ecx + 0x30]
            push eax
            mov eax, dword ptr [esp + 0x70]
            push 0x21
            push ebx
            call dword ptr [eax + 8]
            mov edx, dword ptr [esp + 0x64]
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebx + 4]
            push edi
            push 1
            push edx
            push eax
            jmp L00539127
    L005390ed:
            xor ecx, ecx
            mov eax, dword ptr [ebx + 4]
            mov cx, word ptr [ebp + 0x36]
            push edi
            push ecx
            push edx
            mov edx, dword ptr [esp + 0x70]
            mov dword ptr [esp + 0x78], eax
            push edx
            push esi
            push ebx
            call dword ptr [eax + 0x30]
            push eax
            mov eax, dword ptr [esp + 0x70]
            push 0x21
            push ebx
            call dword ptr [eax + 8]
            movsx eax, word ptr [ebp + 0x28]
            mov ecx, dword ptr [ebx + 4]
            xor edx, edx
            mov dx, word ptr [ebp + 0x36]
            push edi
            push edx
            mov edx, dword ptr [esp + 0x6c]
            push eax
            push edx
    L00539127:
            push esi
            push ebx
            call dword ptr [ecx + 0x28]
            push ebp
            call BrPixelmapDirectUnlock
            add esp, 4
            push ebx
            call FopWrite_END
            mov ecx, dword ptr [esp + 0x68]
            mov eax, dword ptr [esp + 0x10]
            add ecx, 4
            dec eax
            mov dword ptr [esp + 0x68], ecx
            mov dword ptr [esp + 0x10], eax
            jne L00538fa8
            mov esi, dword ptr [esp + 0x18]
            pop ebp
            pop edi
    L0053915b:
            push ebx
            call DfClose
            mov eax, esi
            pop esi
            pop ebx
            add esp, 0x50
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00539320
br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(const char* filename) {
    br_pixelmap* ptr;

    if (BrPixelmapLoadMany(filename, &ptr, 1) == 1) {
        return ptr;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005393a0
br_uint_32 C2_NAKED C2_HOOK_CDECL BrPixelmapSave(const char* filename, br_pixelmap* ptr) {
#ifndef CARPOCALYPSE2_MATCHING

    return BrPixelmapSaveMany(filename, &ptr, 1);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            sub esp, 0x54
            push ebx
            push 0xb
            push 1
            push eax
            call DfOpen
            mov ebx, eax
            test ebx, ebx
            jne L005393bd
            pop ebx
            add esp, 0x54
            ret
    L005393bd:
            push edi
            push esi
            push ebp
            push 2
            push ebx
            call FopWrite_FILE_INFO
            lea ecx, [esp + 0x6c]
            mov dword ptr [esp + 0x18], 1
            mov dword ptr [esp + 0x14], ecx
    L005393d8:
            mov edx, dword ptr [esp + 0x14]
            push 1
            mov ebp, dword ptr [edx]
            push ebp
            call BrPixelmapDirectLock
            mov ecx, 0x11
            mov esi, ebp
            lea edi, [esp + 0x28]
            add esp, 8
            rep movsd
            mov eax, dword ptr [esp + 0x4c]
            mov esi, dword ptr [ebx + 4]
            and eax, 0xff
            lea edx, [esp + 0x20]
            push edx
            push offset br_pixelmap_F
            mov cx, word ptr [eax*8 + pmTypeInfo]
            push ebx
            shr cx, 3
            imul ecx, dword ptr [esp + 0x60]
            mov word ptr [esp + 0x54], cx
            call dword ptr [esi + 0x24]
            push eax
            push 0x3d
            push ebx
            call dword ptr [esi + 8]
            mov eax, dword ptr [ebx + 4]
            lea ecx, [esp + 0x20]
            push ecx
            push offset br_pixelmap_F
            push ebx
            call dword ptr [eax + 0x1c]
            mov eax, dword ptr [ebp + 0x10]
            test eax, eax
            je L00539457
            push ebx
            push eax
            call WritePixelmap
            mov edx, dword ptr [ebx + 4]
            push 0
            push 0x22
            push ebx
            call dword ptr [edx + 8]
    L00539457:
            xor eax, eax
            xor ecx, ecx
            mov al, byte ptr [ebp + 0x2c]
            xor edi, edi
            shl eax, 3
            xor edx, edx
            xor esi, esi
            mov cx, word ptr [eax + pmTypeInfo]
            mov di, word ptr [eax + pmTypeInfo+0x2]
            mov dx, word ptr [ebp + 0x34]
            mov si, word ptr [ebp + 0x32]
            shr ecx, 3
            mov eax, ecx
            mov dword ptr [esp + 0x10], edx
            imul eax, edx
            cdq
            idiv edi
            movsx edx, word ptr [ebp + 0x28]
            imul esi, edx
            mov dword ptr [esp + 0x68], eax
            xor eax, eax
            mov ax, word ptr [ebp + 0x30]
            imul eax, ecx
            add esi, eax
            mov eax, dword ptr [ebp + 8]
            add esi, eax
            cmp word ptr [ebp + 0x2a], 0
            je L0053951d
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x68], 0
            test eax, eax
            mov dword ptr [esp + 0x10], eax
            je L005394e0
    L005394c2:
            mov edx, eax
            imul edx, eax
            imul edx, ecx
            mov eax, dword ptr [esp + 0x68]
            add eax, edx
            mov dword ptr [esp + 0x68], eax
            mov eax, dword ptr [esp + 0x10]
            sar eax, 1
            mov dword ptr [esp + 0x10], eax
            jne L005394c2
    L005394e0:
            mov eax, dword ptr [esp + 0x68]
            mov ecx, dword ptr [ebx + 4]
            cdq
            idiv edi
            mov edx, dword ptr [esp + 0x68]
            push edi
            push 1
            push edx
            mov dword ptr [esp + 0x1c], ecx
            push eax
            push esi
            push ebx
            mov dword ptr [esp + 0x34], eax
            call dword ptr [ecx + 0x30]
            push eax
            mov eax, dword ptr [esp + 0x14]
            push 0x21
            push ebx
            call dword ptr [eax + 8]
            mov edx, dword ptr [esp + 0x68]
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [ebx + 4]
            push edi
            push 1
            push edx
            push eax
            jmp L00539557
    L0053951d:
            xor ecx, ecx
            mov eax, dword ptr [ebx + 4]
            mov cx, word ptr [ebp + 0x36]
            push edi
            push ecx
            push edx
            mov edx, dword ptr [esp + 0x74]
            mov dword ptr [esp + 0x1c], eax
            push edx
            push esi
            push ebx
            call dword ptr [eax + 0x30]
            push eax
            mov eax, dword ptr [esp + 0x14]
            push 0x21
            push ebx
            call dword ptr [eax + 8]
            movsx eax, word ptr [ebp + 0x28]
            mov ecx, dword ptr [ebx + 4]
            xor edx, edx
            mov dx, word ptr [ebp + 0x36]
            push edi
            push edx
            mov edx, dword ptr [esp + 0x70]
            push eax
            push edx
    L00539557:
            push esi
            push ebx
            call dword ptr [ecx + 0x28]
            push ebp
            call BrPixelmapDirectUnlock
            add esp, 4
            push ebx
            call FopWrite_END
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            add ecx, 4
            dec eax
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x18], eax
            jne L005393d8
            push ebx
            call DfClose
            pop ebp
            pop esi
            pop edi
            mov eax, 1
            pop ebx
            add esp, 0x54
            ret
    }
#endif
}
