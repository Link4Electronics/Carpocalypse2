#include "loadtif.h"

#include "loading1.h"
#include "utility.h"
#include "packfile.h"
#include "globvars.h"
#include "world.h"

#ifndef _WIN32
extern const char* carpocalypse2_fix_path_case(const char* pPath);
#endif
#include "loadtif_zlib.h"
#include "platform.h"
#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include "c2_string.h"

#include <tiffio.h>

#include <sys/types.h>
#include <sys/stat.h>

static __inline br_pixelmap* C2_HOOK_FASTCALL LoadTiffUsingPalette(const char* pPath, br_pixelmap* pPalette, int pFlags, int* pError_code);
static __inline int BuildPathWithDirAndSuffix(char* pDest_dir_path, char* pDest_file_path, const char* pOld_path, const char* pNew_dirname, const char *pOld_suffix, const char* pNew_suffix);
static __inline int C2_HOOK_FASTCALL CheckPixOutdated(const char* pDirectory, const char* pStem, const char* pPix_path);

#ifdef CARPOCALYPSE2_MATCHING
extern br_filesystem* C2_HOOK_CDECL BrFilesystemSet();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad();
extern br_uint_32 C2_HOOK_CDECL BrPixelmapSave();
extern void* C2_HOOK_CDECL BrResAdd();
extern char* C2_HOOK_CDECL BrResStrDup();
extern br_pixelmap* C2_HOOK_FASTCALL CreatePalettePixelmapFromRGBChannels(br_uint_16* pRed_channel, br_uint_16* pGrn_channel, br_uint_16* pBlu_channel, int pRGB555);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern int C2_HOOK_FASTCALL DRstrrcasestr(int* pOffset, const char* pHaystack, int pLength_haystack, const char* pNeedle);
extern int C2_HOOK_FASTCALL FindPaletteColour(br_colour pRgb, br_pixelmap *pPalette);
extern int C2_HOOK_FASTCALL GetLastModificationTime(const char* path);
extern int C2_HOOK_FASTCALL IsValidFile(const char* path);
extern br_pixelmap* C2_HOOK_FASTCALL LoadTiff16bit(const char *pPath, int pFlags, int *pError_code);
extern br_pixelmap* C2_HOOK_FASTCALL LoadTiff8bit(const char *pPath, int pFlags, int *pError_code);
extern int C2_HOOK_FASTCALL PDmkdir(const char *pPath);
extern int C2_HOOK_FASTCALL ReadFileLink(char* pDest, const char* pPath);
extern int C2_HOOK_FASTCALL WriteFileLink(const char* pLink_dest, const char* pLink_source);
extern int gCancel_recover;
extern int gDRFont_to_polyfont_mapping[24];
extern const char* gDir_separator;
extern tFrontend_spec gFrontend_QUIT;
extern int gLast_key_down;
extern br_filesystem gZlib_filesystem;
#endif
// FUNCTION: CARMA2_HW 0x00485130
C2_NAKED br_pixelmap* C2_HOOK_FASTCALL LoadTiffPixelmap(const char* path, int flags) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_16 photometric;
    TIFF* tif;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_8* scanline_buffer;
    br_uint_32 width;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
    if (photometric == PHOTOMETRIC_RGB) {
        br_pixelmap* pm;
        br_uint_32 height;
        br_uint_16 samples_per_pixel;
        br_uint_16* pixels;
        br_uint_32 red;
        br_uint_32 grn;
        br_uint_32 blu;
        br_uint_32 alp;

        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
        pm = BrPixelmapAllocate((br_uint_8)(samples_per_pixel == 3 ? BR_PMT_RGB_565 : BR_PMT_RGBA_4444), width, height, NULL, 0);
        if (pm == NULL) {
            TIFFClose(tif);
            return NULL;
        }
        pixels = pm->pixels;
        scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
        if (scanline_buffer == NULL) {
            BrPixelmapFree(pm);
            TIFFClose(tif);
            return NULL;
        }
        for (y = 0; y < height && TIFFReadScanline(tif, scanline_buffer, y, 0) > 0; y++) {
            if (samples_per_pixel == 3) {
                for (x = 0; x < width; x++) {
                    red = scanline_buffer[3 * x + 0] >> 3;
                    grn = scanline_buffer[3 * x + 1] >> 2;
                    blu = scanline_buffer[3 * x + 2] >> 3;
                    pixels[(y * (int)pm->row_bytes >> 1) + x] = (red * 0x40 | grn) * 0x20 | blu;
                }
            } else {
                for (x = 0; x < width; x++) {
                    red = scanline_buffer[4 * x + 0] >> 4;
                    grn = scanline_buffer[4 * x + 1] >> 4;
                    blu = scanline_buffer[4 * x + 2] >> 4;
                    alp = scanline_buffer[4 * x + 3] >> 4;
                    pixels[(y * (int)pm->row_bytes >> 1) + x] = ((alp * 0x10 | red) * 0x10 | grn) * 0x10 | blu;
                }
            }
        }
        _TIFFfree(scanline_buffer);
        if (y != height) {
            BrPixelmapFree(pm);
            TIFFClose(tif);
            return NULL;
        }
        TIFFClose(tif);
        return pm;
    }
    if (photometric == PHOTOMETRIC_PALETTE) {
        br_pixelmap* pm;
        br_uint_32 height;
        br_uint_16 samples_per_pixel;
        br_uint_8* pixels;
        br_uint_16* red;
        br_uint_16* green;
        br_uint_16* blue;

        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
        pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
        if (pm == NULL) {
            TIFFClose(tif);
            return NULL;
        }
        pixels = pm->pixels;
        if (samples_per_pixel == 1) {
            for (y = 0; y < height; y++) {
                if (TIFFReadScanline(tif, pixels + y * pm->row_bytes, y, 0) <= 0) {
                    break;
                }
            }
        } else {
            scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
            if (scanline_buffer == NULL) {
                BrPixelmapFree(pm);
                TIFFClose(tif);
                return NULL;
            }
            for (y = 0; y < height; y++) {
                if (TIFFReadScanline(tif, scanline_buffer, y, 0) <= 0) {
                    break;
                }
                for (x = 0; x < width; x++) {
                    pixels[y * pm->row_bytes + x] = (scanline_buffer[x * samples_per_pixel + 1] < 0x80) ? 0 : scanline_buffer[x * samples_per_pixel];
                }
            }
            _TIFFfree(scanline_buffer);
        }
        if (y != height) {
            BrPixelmapFree(pm);
            TIFFClose(tif);
            return NULL;
        }
        TIFFGetField(tif, TIFFTAG_COLORMAP, &red, &green, &blue);
        pm->map = CreatePalettePixelmapFromRGBChannels(red, green, blue, flags);
        if (pm->map == NULL) {
            BrPixelmapFree(pm);
            TIFFClose(tif);
            return NULL;
        }
        pm->identifier = BrResStrDup(pm, "pixelmap");
        BrResAdd(pm, pm->map);
        TIFFClose(tif);
        return pm;
    }
    TIFFClose(tif);
    return NULL;
#else
    __asm {
            sub esp, 0x28
            mov dword ptr [esp + 0x10], edx
            push edi
            mov edx, offset gLast_key_down+0x30
            call TIFFOpen
            mov edi, eax
            test edi, edi
            mov dword ptr [esp + 0x10], edi
            jne L00485151
            pop edi
            add esp, 0x28
            ret
    L00485151:
            push esi
            push ebp
            lea eax, [esp + 0x12]
            push ebx
            push eax
            push 0x106
            push edi
            call TIFFGetField
            mov ax, word ptr [esp + 0x22]
            add esp, 0xc
            cmp ax, 2
            jne L0048536b
            lea ecx, [esp + 0x20]
            push ecx
            push 0x101
            push edi
            call TIFFGetField
            add esp, 0xc
            lea edx, [esp + 0x18]
            push edx
            push 0x100
            push edi
            call TIFFGetField
            add esp, 0xc
            lea eax, [esp + 0x10]
            push eax
            push 0x115
            push edi
            call TIFFGetFieldDefaulted
            mov ax, word ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x24]
            sub ax, 3
            add esp, 0xc
            neg ax
            sbb eax, eax
            push 0
            and eax, 0xd
            push 0
            push ecx
            add eax, 5
            push edx
            push eax
            call BrPixelmapAllocate
            mov ebx, eax
            add esp, 0x14
            test ebx, ebx
            mov dword ptr [esp + 0x24], ebx
            je L00485571
            mov ecx, dword ptr [ebx + 8]
            mov dword ptr [esp + 0x28], ecx
            mov ecx, edi
            call TIFFScanlineSize
            mov ecx, eax
            call _TIFFmalloc
            mov esi, eax
            test esi, esi
            mov dword ptr [esp + 0x2c], esi
            jne L00485225
            push ebx
            call BrPixelmapFree
            mov ecx, dword ptr [esp + 0x20]
            add esp, 4
            xor ebp, ebp
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L00485225:
            mov eax, dword ptr [esp + 0x20]
            xor ebx, ebx
            test eax, eax
            jbe L00485325
            jmp L00485239
    L00485235:
            mov edi, dword ptr [esp + 0x1c]
    L00485239:
            push 0
            push ebx
            mov edx, esi
            mov ecx, edi
            call TIFFReadScanline
            test eax, eax
            jle L00485325
            cmp word ptr [esp + 0x10], 3
            jne L004852b0
            mov eax, dword ptr [esp + 0x18]
            xor edi, edi
            test eax, eax
            jbe L00485318
            lea ecx, [esi + 2]
    L00485266:
            xor eax, eax
            xor edx, edx
            mov al, byte ptr [ecx - 2]
            mov dl, byte ptr [ecx - 1]
            shr eax, 3
            mov esi, edx
            xor edx, edx
            mov dl, byte ptr [ecx]
            add ecx, 3
            shr esi, 2
            shl eax, 6
            or eax, esi
            mov esi, ebx
            shr edx, 3
            shl eax, 5
            or eax, edx
            mov edx, dword ptr [esp + 0x24]
            movsx edx, word ptr [edx + 0x28]
            imul esi, edx
            mov edx, dword ptr [esp + 0x28]
            shr esi, 1
            add esi, edi
            inc edi
            mov word ptr [edx + esi*2], ax
            mov eax, dword ptr [esp + 0x18]
            cmp edi, eax
            jb L00485266
            jmp L00485314
    L004852b0:
            mov eax, dword ptr [esp + 0x18]
            xor ebp, ebp
            test eax, eax
            jbe L00485318
            lea ecx, [esi + 2]
    L004852bd:
            xor eax, eax
            xor edx, edx
            mov al, byte ptr [ecx - 1]
            mov dl, byte ptr [ecx - 2]
            mov esi, eax
            xor eax, eax
            mov al, byte ptr [ecx]
            add ecx, 4
            mov edi, eax
            xor eax, eax
            mov al, byte ptr [ecx - 3]
            shr eax, 4
            shr edx, 4
            shl eax, 4
            or eax, edx
            mov edx, dword ptr [esp + 0x24]
            shr esi, 4
            movsx edx, word ptr [edx + 0x28]
            shl eax, 4
            or eax, esi
            mov esi, ebx
            imul esi, edx
            mov edx, dword ptr [esp + 0x28]
            shr esi, 1
            shr edi, 4
            shl eax, 4
            add esi, ebp
            or eax, edi
            inc ebp
            mov word ptr [edx + esi*2], ax
            mov eax, dword ptr [esp + 0x18]
            cmp ebp, eax
            jb L004852bd
    L00485314:
            mov esi, dword ptr [esp + 0x2c]
    L00485318:
            mov eax, dword ptr [esp + 0x20]
            inc ebx
            cmp ebx, eax
            jb L00485235
    L00485325:
            mov ecx, esi
            call _TIFFfree
            cmp ebx, dword ptr [esp + 0x20]
            je L00485354
            mov eax, dword ptr [esp + 0x24]
            push eax
            call BrPixelmapFree
            mov ecx, dword ptr [esp + 0x20]
            add esp, 4
            xor ebp, ebp
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L00485354:
            mov ecx, dword ptr [esp + 0x1c]
            mov ebp, dword ptr [esp + 0x24]
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L0048536b:
            cmp ax, 3
            jne L00485571
            lea ecx, [esp + 0x10]
            push ecx
            push 0x101
            push edi
            call TIFFGetField
            add esp, 0xc
            lea edx, [esp + 0x24]
            push edx
            push 0x100
            push edi
            call TIFFGetField
            add esp, 0xc
            lea eax, [esp + 0x2c]
            push eax
            push 0x115
            push edi
            call TIFFGetFieldDefaulted
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x30]
            add esp, 0xc
            push 0
            push 0
            push ecx
            push edx
            push 3
            call BrPixelmapAllocate
            mov ebp, eax
            add esp, 0x14
            test ebp, ebp
            je L00485571
            mov esi, dword ptr [ebp + 8]
            cmp word ptr [esp + 0x2c], 1
            mov dword ptr [esp + 0x28], esi
            jne L0048541a
            mov eax, dword ptr [esp + 0x10]
            xor edi, edi
            test eax, eax
            jbe L004854bf
    L004853ed:
            movsx eax, word ptr [ebp + 0x28]
            mov edx, edi
            mov ecx, dword ptr [esp + 0x1c]
            imul edx, eax
            push 0
            push edi
            add edx, esi
            call TIFFReadScanline
            test eax, eax
            jle L004854bf
            mov eax, dword ptr [esp + 0x10]
            inc edi
            cmp edi, eax
            jb L004853ed
            jmp L004854bf
    L0048541a:
            mov ecx, edi
            call TIFFScanlineSize
            mov ecx, eax
            call _TIFFmalloc
            mov esi, eax
            test esi, esi
            jne L0048544c
            push ebp
            call BrPixelmapFree
            mov ecx, dword ptr [esp + 0x20]
            add esp, 4
            xor ebp, ebp
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L0048544c:
            mov eax, dword ptr [esp + 0x10]
            xor edi, edi
            test eax, eax
            jbe L004854b8
    L00485456:
            mov ecx, dword ptr [esp + 0x1c]
            push 0
            push edi
            mov edx, esi
            call TIFFReadScanline
            test eax, eax
            jle L004854b8
            mov ecx, dword ptr [esp + 0x24]
            xor eax, eax
            test ecx, ecx
            jbe L004854af
    L00485472:
            mov ecx, dword ptr [esp + 0x2c]
            and ecx, 0xffff
            imul ecx, eax
            lea edx, [ecx + esi]
            mov cl, byte ptr [ecx + esi + 1]
            cmp cl, 0x80
            jae L0048548f
            xor ecx, ecx
            jmp L00485493
    L0048548f:
            xor ecx, ecx
            mov cl, byte ptr [edx]
    L00485493:
            movsx edx, word ptr [ebp + 0x28]
            mov ebx, edi
            imul ebx, edx
            mov edx, dword ptr [esp + 0x28]
            add ebx, edx
            inc eax
            mov byte ptr [ebx + eax - 1], cl
            mov ecx, dword ptr [esp + 0x24]
            cmp eax, ecx
            jb L00485472
    L004854af:
            mov eax, dword ptr [esp + 0x10]
            inc edi
            cmp edi, eax
            jb L00485456
    L004854b8:
            mov ecx, esi
            call _TIFFfree
    L004854bf:
            cmp edi, dword ptr [esp + 0x10]
            je L004854e3
            push ebp
            call BrPixelmapFree
            mov ecx, dword ptr [esp + 0x20]
            add esp, 4
            xor ebp, ebp
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L004854e3:
            lea eax, [esp + 0x18]
            lea ecx, [esp + 0x30]
            push eax
            mov eax, dword ptr [esp + 0x20]
            lea edx, [esp + 0x38]
            push ecx
            push edx
            push 0x140
            push eax
            call TIFFGetField
            mov ecx, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x2c]
            add esp, 0x14
            push ecx
            mov ecx, dword ptr [esp + 0x38]
            push edx
            mov edx, dword ptr [esp + 0x38]
            call CreatePalettePixelmapFromRGBChannels
            test eax, eax
            mov dword ptr [ebp + 0x10], eax
            jne L00485540
            push ebp
            call BrPixelmapFree
            mov ecx, dword ptr [esp + 0x20]
            add esp, 4
            xor ebp, ebp
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L00485540:
            push offset gLast_key_down+0x24
            push ebp
            call BrResStrDup
            add esp, 8
            mov dword ptr [ebp + 4], eax
            mov eax, dword ptr [ebp + 0x10]
            push eax
            push ebp
            call BrResAdd
            mov ecx, dword ptr [esp + 0x24]
            add esp, 8
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    L00485571:
            mov ecx, dword ptr [esp + 0x1c]
            xor ebp, ebp
            call TIFFClose
            mov eax, ebp
            pop ebx
            pop ebp
            pop esi
            pop edi
            add esp, 0x28
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00485590
br_pixelmap* C2_HOOK_FASTCALL CreatePalettePixelmapFromRGBChannels(br_uint_16* pRed_channel, br_uint_16* pGrn_channel, br_uint_16* pBlu_channel, int pRGB555) {
    br_pixelmap* pm;
    int i;

    if (pRGB555) {
        br_uint_16* pixels;

        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (pm == NULL) {
            return NULL;
        }
        pixels = pm->pixels;
        for (i = 0; i < 256; i++) {
                pixels[i] = ((pRed_channel[i] & 0xf800) >> 0) | ((pGrn_channel[i] & 0xfc00) >> 5) | ((pBlu_channel[i] >> 11));
        }
    } else {
        br_uint_8* pixels;

        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        if (pm == NULL) {
            return NULL;
        }
        pixels = pm->pixels;

        pixels[0] = 0;
        pixels[1] = 0;
        pixels[2] = 0;
        for (i = 1; i < 256; i++) {
            pixels[4 * i + 0] = pBlu_channel[i] >> 8;
            pixels[4 * i + 1] = pGrn_channel[i] >> 8;
            pixels[4 * i + 2] = pRed_channel[i] >> 8;
            if (pixels[4 * i + 0] == 0 && pixels[4 * i + 1] == 0 && pixels[4 * i + 2] == 0) {
                pixels[4 * i + 0] = 1;
                pixels[4 * i + 1] = 1;
                pixels[4 * i + 2] = 1;
            }
        }
    }
    pm->identifier = BrResStrDup(pm, "palette");
    if (pm->identifier == NULL) {
        BrPixelmapFree(pm);
        return NULL;
    }
    return pm;
}

static br_pixelmap* C2_HOOK_FASTCALL LoadDefaultPalette(const char* pData_dir_path, int pFlags, int *pError_code) {
    int i;
    tPath_name path;
    br_uint_8 buffer[256 * 3];
    FILE* f;
    br_pixelmap* pm;

    sprintf(path, "%s%s%s%s%s", pData_dir_path, gDir_separator, "PALETTE", gDir_separator, "DEFAULT.ACT");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        goto error;
    }
    if (fread(buffer, CARPOCALYPSE2_ASIZE(buffer), 1, f) == 0) {
        goto error;
    }
    fclose(f);
    if (pFlags & kLoadTextureFlags_PalatteRGB555) {
        br_uint_16* pixel;

        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (pm == NULL) {
            *pError_code = 2;
            return NULL;
        }
        pixel = pm->pixels;
        for (i = 0; i < 256; i++) {
            pixel[i] = (br_uint_16)(((buffer[3 * i + 0] >> 3) << 11) | ((buffer[3 * i + 1] >> 2) << 5) | ((buffer[3 * i + 2] >> 3) << 0));
        }
    } else {
        br_uint_8* pixel;

        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            *pError_code = 2;
            return NULL;
        }
        pixel = pm->pixels;
        pixel[2] = 0;
        pixel[1] = 0;
        pixel[0] = 0;
        for (i = 1; i < 256; i++) {
            pixel[4 * i + 0] = buffer[3 * i + 2];
            pixel[4 * i + 1] = buffer[3 * i + 1];
            pixel[4 * i + 2] = buffer[3 * i + 0];
            if (pixel[4 * i + 0] == 0 && pixel[4 * i + 1] == 0 && pixel[4 * i + 2] == 0) {
                pixel[4 * i + 0] = 1;
                pixel[4 * i + 1] = 1;
                pixel[4 * i + 2] = 1;
            }
        }
    }
    pm->identifier = BrResStrDup(pm, "DEFAULT.ACT");
    if (pm->identifier == NULL) {
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }
    return pm;

error:
    *pError_code = 5;
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00485750
C2_NAKED br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex2(const char* pDirectory, const char* pFile_stem, br_pixelmap* pPalette, int pFlags, int* pError_code, int pTiff_palette) {
#ifndef CARPOCALYPSE2_MATCHING
    tPath_name tif_path;
    tPath_name zz_pix_path_target;
    tPath_name aa_pix_path;
    tPath_name save_dir_path;
    tPath_name save_pix_dir_path;
    int is_16bbp;
    int save_compressed;
    int pix_path_is_link;
    int tif_path_is_link;
    int pix_exists;
    int tif_exists;
    br_pixelmap* pm;
    br_pixelmap* pal;
    br_filesystem* original_filesystem;

    sprintf(tif_path, "%s%s%s%s%s%s", pDirectory, gDir_separator, pTiff_palette ? "TIFFX" : "TIFFRGB", gDir_separator, pFile_stem, ".TIF");
    is_16bbp = pFlags & kLoadTextureFlags_16bbp;
    save_compressed = pFlags & kLoadTextureFlags_SaveTextureCompressed;
    sprintf(aa_pix_path, "%s%s%s%s%s%s", pDirectory, gDir_separator, is_16bbp ? "PIX16" : "PIX8", gDir_separator, pFile_stem, ".PIX");
    tif_path_is_link = ReadFileLink(tif_path, tif_path);
    pix_path_is_link = ReadFileLink(zz_pix_path_target, aa_pix_path);
    tif_exists = IsValidFile(tif_path);
    pix_exists = IsValidFile(pix_path_is_link ? zz_pix_path_target : aa_pix_path);

    if (pTiff_palette && pix_exists && !tif_exists) {
        const char* pix_path = pix_path_is_link ? zz_pix_path_target : aa_pix_path;
        if (CheckPixOutdated(pDirectory, pFile_stem, pix_path)) {
            return 0;
        }
    }
    if (!tif_exists || ((is_16bbp || pTiff_palette) && !(pFlags & kLoadTextureFlags_ForceTiff) && pix_exists && (br_uint_32)GetLastModificationTime(pix_path_is_link ? zz_pix_path_target : aa_pix_path) >= (br_uint_32)GetLastModificationTime(tif_path))) {

        if (pix_exists && (is_16bbp || pTiff_palette)) {
            original_filesystem = BrFilesystemSet(&gZlib_filesystem);
            pm = BrPixelmapLoad(pix_path_is_link ? zz_pix_path_target : aa_pix_path);
            BrFilesystemSet(original_filesystem);
            *pError_code = (pm != NULL) ? 0 : 3;
            return pm;
        } else {
            *pError_code = 1;
            return NULL;
        }
    }

    if (pTiff_palette) {
        pm = LoadTiff8bit(tif_path, pFlags, pError_code);
    } else if (is_16bbp) {
        pm = LoadTiff16bit(tif_path, pFlags, pError_code);
    } else {
        pal = pPalette;
        if (pal == NULL) {
            pal = LoadDefaultPalette(pDirectory, pFlags, pError_code);
            if (pal == NULL) {
                return NULL;
            }
        }
        pm = LoadTiffUsingPalette(tif_path, pal, pFlags, pError_code);
        if (pm != NULL && (pFlags & kLoadTextureFlags_KeepShadeTable)) {
            pm->map = pal;
            if (pPalette == NULL) {
                BrResAdd(pm, pal);
            }
        } else {
            if (pPalette == NULL) {
                BrPixelmapFree(pal);
            }
        }
    }
    if (pm != NULL && (pFlags & kLoadTextureFlags_SaveBrenderTexture)) {
        if (!pix_path_is_link) {
            sprintf(save_pix_dir_path, "%s%s%s", pDirectory, gDir_separator, is_16bbp ? "PIX16" : "PIX8");
            PDmkdir(save_pix_dir_path);
            if (tif_path_is_link && !pix_exists) {
                int pos_suffix;
                int pos_filename_sep;
                int pos_dir_sep;
                int pos_endsep;

#pragma inline_depth(0)
                if (DRstrrcasestr(&pos_suffix, tif_path, strlen(tif_path), ".TIF")
                        && DRstrrcasestr(&pos_filename_sep, tif_path, pos_suffix, gDir_separator)
                        && DRstrrcasestr(&pos_dir_sep, tif_path, pos_filename_sep, gDir_separator)) {
#pragma inline_depth()
                    pos_endsep = pos_filename_sep + strlen(gDir_separator);
                    sprintf(save_dir_path, "%.*s%s%s", pos_dir_sep, tif_path, gDir_separator, is_16bbp ? "PIX16" : "PIX8");
                    sprintf(zz_pix_path_target, "%s%s%.*s%s", save_dir_path, gDir_separator, pos_suffix - pos_endsep, tif_path + pos_endsep, ".PIX");
                }
                PDmkdir(save_dir_path);
#pragma inline_depth(0)
                WriteFileLink(zz_pix_path_target, aa_pix_path);
#pragma inline_depth()
                pix_path_is_link = 1;
            }
        }
        if (!save_compressed) {
            original_filesystem = BrFilesystemSet(&gZlib_filesystem);
        }
        BrPixelmapSave(pix_path_is_link ? zz_pix_path_target : aa_pix_path, pm);
        if (!save_compressed) {
            BrFilesystemSet(original_filesystem);
        }
    }
    if (pm != NULL) {
        *pError_code = 0;
    }
    return pm;
#else
    __asm {
            sub esp, 0x930
            mov eax, dword ptr [esp + 0x940]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            test eax, eax
            mov esi, edx
            mov dword ptr [esp + 0x3c], edi
            mov ecx, offset gLast_key_down+0xa4
            jne L00485777
            mov ecx, offset gLast_key_down+0x9c
    L00485777:
            mov eax, dword ptr [gDir_separator]
            push offset gFrontend_QUIT+0xbbac
            push esi
            push eax
            push ecx
            push eax
            push edi
            lea eax, [esp + 0x58]
            push offset gLast_key_down+0x8c
            push eax
            call sprintf
            mov eax, dword ptr [esp + 0x968]
            add esp, 0x20
            and eax, 1
            mov ecx, offset gLast_key_down+0x84
            mov dword ptr [esp + 0x30], eax
            je L004857b2
            mov ecx, offset gLast_key_down+0x7c
    L004857b2:
            mov eax, dword ptr [gDir_separator]
            push offset gDRFont_to_polyfont_mapping+0x314
            push esi
            push eax
            push ecx
            push eax
            push edi
            lea ecx, [esp + 0x158]
            push offset gLast_key_down+0x8c
            push ecx
            call sprintf
            add esp, 0x20
            lea edx, [esp + 0x40]
            lea ecx, [esp + 0x40]
            call ReadFileLink
            lea edx, [esp + 0x140]
            lea ecx, [esp + 0x240]
            mov dword ptr [esp + 0x28], eax
            call ReadFileLink
            mov ebp, eax
            lea ecx, [esp + 0x40]
            mov dword ptr [esp + 0x38], ebp
            call IsValidFile
            test ebp, ebp
            mov ebx, eax
            lea ecx, [esp + 0x240]
            jne L0048581d
            lea ecx, [esp + 0x140]
    L0048581d:
            call IsValidFile
            mov dword ptr [esp + 0x24], eax
            mov eax, dword ptr [esp + 0x950]
            test eax, eax
            je L004858cf
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L004858cf
            test ebx, ebx
            jne L004858d7
            test ebp, ebp
            je L0048585a
            lea edx, [esp + 0x240]
            mov dword ptr [esp + 0x14], edx
            jmp L00485865
    L0048585a:
            lea eax, [esp + 0x140]
            mov dword ptr [esp + 0x14], eax
    L00485865:
            mov eax, dword ptr [gDir_separator]
            push offset gFrontend_QUIT+0xbbac
            push esi
            push eax
            push offset gLast_key_down+0x9c
            push eax
            push edi
            lea ecx, [esp + 0x358]
            push offset gLast_key_down+0x8c
            push ecx
            call sprintf
            add esp, 0x20
            lea edx, [esp + 0x340]
            lea ecx, [esp + 0x340]
            call ReadFileLink
            lea ecx, [esp + 0x340]
            call IsValidFile
            test eax, eax
            je L004858cf
            lea ecx, [esp + 0x340]
            call GetLastModificationTime
            mov ecx, dword ptr [esp + 0x14]
            mov esi, eax
            call GetLastModificationTime
            cmp esi, eax
            ja L00485fc3
    L004858cf:
            test ebx, ebx
            je L00485f42
    L004858d7:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            jne L004858ea
            mov eax, dword ptr [esp + 0x950]
            test eax, eax
            je L0048592a
    L004858ea:
            mov ebx, dword ptr [esp + 0x948]
            test bl, 0x20
            jne L00485931
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L00485931
            test ebp, ebp
            lea ecx, [esp + 0x240]
            jne L00485910
            lea ecx, [esp + 0x140]
    L00485910:
            call GetLastModificationTime
            lea ecx, [esp + 0x40]
            mov esi, eax
            call GetLastModificationTime
            cmp eax, esi
            jbe L00485f42
            jmp L00485931
    L0048592a:
            mov ebx, dword ptr [esp + 0x948]
    L00485931:
            mov eax, dword ptr [esp + 0x950]
            test eax, eax
            jne L00485d43
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            jne L00485d2e
            mov ebp, dword ptr [esp + 0x944]
            test ebp, ebp
            je L00485960
            mov dword ptr [esp + 0x10], ebp
            jmp L00485afe
    L00485960:
            mov eax, dword ptr [gDir_separator]
            push offset gLast_key_down+0x70
            push eax
            push offset gLast_key_down+0x68
            push eax
            push edi
            lea edx, [esp + 0x354]
            push offset gLast_key_down+0x5c
            push edx
            call sprintf
            add esp, 0x1c
            mov edx, offset gCancel_recover+0x1a4
            lea ecx, [esp + 0x340]
            call DRfopen
            mov esi, eax
            test esi, esi
            je L00485ae3
            push esi
            push 1
            lea eax, [esp + 0x648]
            push 0x300
            push eax
            call fread
            add esp, 0x10
            test eax, eax
            je L00485ae3
            push esi
            call fclose
            add esp, 4
            test bl, 0x10
            push 0
            push 0
            push 0x100
            push 1
            je L00485a43
            push 5
            call BrPixelmapAllocate
            mov ebp, eax
            add esp, 0x14
            test ebp, ebp
            jne L004859fd
            mov ecx, dword ptr [esp + 0x94c]
            mov dword ptr [ecx], 2
            jmp L00485af0
    L004859fd:
            mov ecx, dword ptr [ebp + 8]
            lea eax, [esp + 0x640]
            mov edi, 0x100
    L00485a0c:
            mov bl, byte ptr [eax]
            add ecx, 2
            movzx dx, byte ptr [eax + 1]
            and bl, 0xf8
            and edx, 0xfffc
            movzx si, bl
            mov bl, byte ptr [eax + 2]
            add eax, 3
            shl esi, 5
            shr bl, 3
            or edx, esi
            movzx si, bl
            shl edx, 3
            or edx, esi
            dec edi
            mov word ptr [ecx - 2], dx
            jne L00485a0c
            jmp L00485ab6
    L00485a43:
            push 7
            call BrPixelmapAllocate
            mov ebp, eax
            add esp, 0x14
            test ebp, ebp
            jne L00485a65
            mov eax, dword ptr [esp + 0x94c]
            mov dword ptr [eax], 2
            jmp L00485af0
    L00485a65:
            mov eax, dword ptr [ebp + 8]
            lea ecx, [esp + 0x644]
            mov esi, 0xff
            mov byte ptr [eax + 2], 0
            mov byte ptr [eax + 1], 0
            mov byte ptr [eax], 0
            add eax, 5
    L00485a82:
            mov dl, byte ptr [ecx + 1]
            mov byte ptr [eax - 1], dl
            mov dl, byte ptr [ecx]
            mov byte ptr [eax], dl
            mov dl, byte ptr [ecx - 1]
            mov byte ptr [eax + 1], dl
            mov bl, byte ptr [eax - 1]
            test bl, bl
            jne L00485aad
            cmp byte ptr [eax], 0
            jne L00485aad
            test dl, dl
            jne L00485aad
            mov byte ptr [eax + 1], 1
            mov byte ptr [eax], 1
            mov byte ptr [eax - 1], 1
    L00485aad:
            add ecx, 3
            add eax, 4
            dec esi
            jne L00485a82
    L00485ab6:
            push offset gLast_key_down+0x70
            push ebp
            call BrResStrDup
            add esp, 8
            mov dword ptr [ebp + 4], eax
            test eax, eax
            jne L00485af2
            push ebp
            call BrPixelmapFree
            mov eax, dword ptr [esp + 0x950]
            add esp, 4
            mov dword ptr [eax], 2
            jmp L00485af0
    L00485ae3:
            mov ecx, dword ptr [esp + 0x94c]
            mov dword ptr [ecx], 5
    L00485af0:
            xor ebp, ebp
    L00485af2:
            test ebp, ebp
            mov dword ptr [esp + 0x10], ebp
            je L00485fc3
    L00485afe:
            mov edx, offset gLast_key_down+0x30
            lea ecx, [esp + 0x40]
            call TIFFOpen
            mov edi, eax
            test edi, edi
            mov dword ptr [esp + 0x1c], edi
            jne L00485b2a
            mov edx, dword ptr [esp + 0x94c]
            xor ebx, ebx
            mov dword ptr [edx], 4
            jmp L00485cf0
    L00485b2a:
            lea eax, [esp + 0x14]
            push eax
            push 0x101
            push edi
            call TIFFGetField
            add esp, 0xc
            lea ecx, [esp + 0x18]
            push ecx
            push 0x100
            push edi
            call TIFFGetField
            add esp, 0xc
            lea edx, [esp + 0x36]
            push edx
            push 0x115
            push edi
            call TIFFGetFieldDefaulted
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x24]
            add esp, 0xc
            push 0
            push 0
            push eax
            push ecx
            push 3
            call BrPixelmapAllocate
            mov ebx, eax
            add esp, 0x14
            test ebx, ebx
            jne L00485b9a
            mov ecx, edi
            call TIFFClose
            mov edx, dword ptr [esp + 0x94c]
            mov dword ptr [edx], 2
            jmp L00485cf0
    L00485b9a:
            mov eax, dword ptr [ebx + 8]
            mov ecx, edi
            mov dword ptr [esp + 0x20], eax
            call TIFFScanlineSize
            mov ecx, eax
            call _TIFFmalloc
            mov esi, eax
            test esi, esi
            mov dword ptr [esp + 0x2c], esi
            jne L00485bdd
            mov ecx, edi
            call TIFFClose
            push ebx
            call BrPixelmapFree
            mov ecx, dword ptr [esp + 0x950]
            add esp, 4
            xor ebx, ebx
            mov dword ptr [ecx], 2
            jmp L00485cf0
    L00485bdd:
            mov eax, dword ptr [esp + 0x14]
            xor ebp, ebp
            test eax, eax
            jbe L00485cc0
    L00485beb:
            push 0
            push ebp
            mov edx, esi
            mov ecx, edi
            call TIFFReadScanline
            test eax, eax
            jle L00485cc0
            cmp word ptr [esp + 0x36], 3
            jne L00485c57
            mov eax, dword ptr [esp + 0x18]
            xor edi, edi
            test eax, eax
            jbe L00485cab
            mov edx, dword ptr [esp + 0x2c]
            lea esi, [edx + 2]
    L00485c1c:
            mov edx, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov ch, byte ptr [esi - 2]
            xor eax, eax
            mov cl, byte ptr [esi - 1]
            mov al, byte ptr [esi]
            shl ecx, 8
            or ecx, eax
            call FindPaletteColour
            movsx ecx, word ptr [ebx + 0x28]
            mov edx, ebp
            add esi, 3
            imul edx, ecx
            mov ecx, dword ptr [esp + 0x20]
            add edx, ecx
            inc edi
            mov byte ptr [edx + edi - 1], al
            mov eax, dword ptr [esp + 0x18]
            cmp edi, eax
            jb L00485c1c
            jmp L00485cab
    L00485c57:
            mov eax, dword ptr [esp + 0x18]
            xor esi, esi
            test eax, eax
            jbe L00485cab
            mov eax, dword ptr [esp + 0x2c]
            lea edi, [eax + 1]
    L00485c68:
            cmp byte ptr [edi + 2], 0x80
            jae L00485c72
            xor eax, eax
            jmp L00485c8c
    L00485c72:
            mov edx, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov ch, byte ptr [edi - 1]
            xor eax, eax
            mov cl, byte ptr [edi]
            mov al, byte ptr [edi + 1]
            shl ecx, 8
            or ecx, eax
            call FindPaletteColour
    L00485c8c:
            movsx ecx, word ptr [ebx + 0x28]
            mov edx, ebp
            add edi, 4
            imul edx, ecx
            mov ecx, dword ptr [esp + 0x20]
            add edx, ecx
            inc esi
            mov byte ptr [edx + esi - 1], al
            mov eax, dword ptr [esp + 0x18]
            cmp esi, eax
            jb L00485c68
    L00485cab:
            mov eax, dword ptr [esp + 0x14]
            mov edi, dword ptr [esp + 0x1c]
            mov esi, dword ptr [esp + 0x2c]
            inc ebp
            cmp ebp, eax
            jb L00485beb
    L00485cc0:
            mov ecx, esi
            call _TIFFfree
            mov ecx, edi
            call TIFFClose
            cmp ebp, dword ptr [esp + 0x14]
            je L00485cec
            push ebx
            call BrPixelmapFree
            mov eax, dword ptr [esp + 0x950]
            add esp, 4
            xor ebx, ebx
            mov dword ptr [eax], 2
    L00485cec:
            mov ebp, dword ptr [esp + 0x10]
    L00485cf0:
            test ebx, ebx
            je L00485d18
            test byte ptr [esp + 0x948], 8
            je L00485d18
            mov eax, dword ptr [esp + 0x944]
            mov dword ptr [ebx + 0x10], ebp
            test eax, eax
            jne L00485d58
            push ebp
            push ebx
            call BrResAdd
            add esp, 8
            jmp L00485d58
    L00485d18:
            mov eax, dword ptr [esp + 0x944]
            test eax, eax
            jne L00485d58
            push ebp
            call BrPixelmapFree
            add esp, 4
            jmp L00485d58
    L00485d2e:
            mov ecx, dword ptr [esp + 0x94c]
            mov edx, ebx
            push ecx
            lea ecx, [esp + 0x44]
            call LoadTiff16bit
            jmp L00485d56
    L00485d43:
            mov edx, dword ptr [esp + 0x94c]
            lea ecx, [esp + 0x40]
            push edx
            mov edx, ebx
            call LoadTiff8bit
    L00485d56:
            mov ebx, eax
    L00485d58:
            test ebx, ebx
            je L00485f33
            test byte ptr [esp + 0x948], 4
            je L00485f22
            mov ebp, dword ptr [esp + 0x38]
            test ebp, ebp
            jne L00485ed8
            mov esi, dword ptr [esp + 0x30]
            mov eax, offset gLast_key_down+0x84
            test esi, esi
            je L00485d8c
            mov eax, offset gLast_key_down+0x7c
    L00485d8c:
            mov ecx, dword ptr [esp + 0x3c]
            push eax
            mov eax, dword ptr [gDir_separator]
            lea edx, [esp + 0x544]
            push eax
            push ecx
            push offset gLast_key_down+0x54
            push edx
            call sprintf
            add esp, 0x14
            lea ecx, [esp + 0x540]
            call PDmkdir
            mov eax, dword ptr [esp + 0x28]
            test eax, eax
            je L00485ed8
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            jne L00485ed8
            test esi, esi
            mov ebp, offset gLast_key_down+0x84
            je L00485ddf
            mov ebp, offset gLast_key_down+0x7c
    L00485ddf:
            lea edi, [esp + 0x40]
            or ecx, 0xffffffff
            xor eax, eax
            push offset gFrontend_QUIT+0xbbac
            repne scasb
            not ecx
            dec ecx
            lea edx, [esp + 0x44]
            push ecx
            lea ecx, [esp + 0x28]
            call DRstrrcasestr
            test eax, eax
            je L00485eb4
            mov eax, dword ptr [gDir_separator]
            mov ecx, dword ptr [esp + 0x20]
            push eax
            push ecx
            lea edx, [esp + 0x48]
            lea ecx, [esp + 0x24]
            call DRstrrcasestr
            test eax, eax
            je L00485eb4
            mov edx, dword ptr [gDir_separator]
            mov eax, dword ptr [esp + 0x1c]
            push edx
            push eax
            lea edx, [esp + 0x48]
            lea ecx, [esp + 0x30]
            call DRstrrcasestr
            test eax, eax
            je L00485eb4
            mov edx, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            mov edi, edx
            xor eax, eax
            repne scasb
            not ecx
            mov edi, dword ptr [esp + 0x1c]
            dec ecx
            mov esi, ecx
            push ebp
            push edx
            mov edx, dword ptr [esp + 0x30]
            lea ecx, [esp + 0x48]
            lea eax, [esp + 0x448]
            push ecx
            push edx
            push offset gLast_key_down+0x48
            push eax
            add esi, edi
            call sprintf
            mov edx, dword ptr [esp + 0x38]
            mov eax, dword ptr [gDir_separator]
            add esp, 0x18
            lea ecx, [esp + esi + 0x40]
            sub edx, esi
            push offset gDRFont_to_polyfont_mapping+0x314
            push ecx
            push edx
            lea ecx, [esp + 0x44c]
            push eax
            push ecx
            lea edx, [esp + 0x254]
            push offset gLast_key_down+0x3c
            push edx
            call sprintf
            add esp, 0x1c
    L00485eb4:
            lea ecx, [esp + 0x440]
            call PDmkdir
            lea edx, [esp + 0x140]
            lea ecx, [esp + 0x240]
            call WriteFileLink
            mov ebp, 1
    L00485ed8:
            mov esi, dword ptr [esp + 0x948]
            and esi, 0x40
            jne L00485ef5
            push offset gZlib_filesystem
            call BrFilesystemSet
            add esp, 4
            mov edi, eax
            jmp L00485ef9
    L00485ef5:
            mov edi, dword ptr [esp + 0x28]
    L00485ef9:
            test ebp, ebp
            lea eax, [esp + 0x240]
            jne L00485f0b
            lea eax, [esp + 0x140]
    L00485f0b:
            push ebx
            push eax
            call BrPixelmapSave
            add esp, 8
            test esi, esi
            jne L00485f22
            push edi
            call BrFilesystemSet
            add esp, 4
    L00485f22:
            test ebx, ebx
            je L00485f33
            mov eax, dword ptr [esp + 0x94c]
            mov dword ptr [eax], 0
    L00485f33:
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x930
            ret 0x10
    L00485f42:
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L00485fb6
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            jne L00485f5d
            mov eax, dword ptr [esp + 0x950]
            test eax, eax
            je L00485fb6
    L00485f5d:
            push offset gZlib_filesystem
            call BrFilesystemSet
            add esp, 4
            mov edi, eax
            test ebp, ebp
            lea eax, [esp + 0x240]
            jne L00485f7e
            lea eax, [esp + 0x140]
    L00485f7e:
            push eax
            call BrPixelmapLoad
            add esp, 4
            mov esi, eax
            push edi
            call BrFilesystemSet
            mov edx, dword ptr [esp + 0x950]
            mov ecx, esi
            add esp, 4
            mov eax, esi
            neg ecx
            sbb ecx, ecx
            and ecx, 0xfffffffd
            add ecx, 3
            mov dword ptr [edx], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x930
            ret 0x10
    L00485fb6:
            mov eax, dword ptr [esp + 0x94c]
            mov dword ptr [eax], 1
    L00485fc3:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x930
            ret 0x10
    }
#endif
}

static __inline int C2_HOOK_FASTCALL CheckPixOutdated(const char* pDirectory, const char* pStem, const char* pPix_path) {
    tPath_name tif_path;

    sprintf(tif_path, "%s%s%s%s%s%s", pDirectory, gDir_separator, "TIFFRGB", gDir_separator, pStem, ".TIF");
    ReadFileLink(tif_path,tif_path);
    if (IsValidFile(tif_path) && (br_uint_32)GetLastModificationTime(tif_path) > (br_uint_32)GetLastModificationTime(pPix_path)) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004856c0
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* pDirectory, const char* pFile_stem, br_pixelmap* pPalette, int pFlags, int* pError_code) {
    br_pixelmap* texture;

    if ((pFlags & kLoadTextureFlags_16bbp) == kLoadTextureFlags_16bbp) {
        goto load_palette;
    }
    texture = LoadTiffTexture_Ex2(pDirectory, pFile_stem, pPalette, pFlags, pError_code, 1);
    if (texture != NULL) {
        goto identifier;
    }
load_palette:
    texture = LoadTiffTexture_Ex2(pDirectory, pFile_stem, pPalette, pFlags, pError_code, 0);
    if (texture == NULL) {
        goto return_texture;
    }
identifier:
    texture->identifier = BrResStrDup(texture, pFile_stem);
    if (texture->identifier == NULL) {
        BrPixelmapFree(texture);
        *pError_code = 2;
        return NULL;
    }
return_texture:
    return texture;
}

// FUNCTION: CARMA2_HW 0x004869e0
int C2_HOOK_FASTCALL ReadFileLink(char* pDest, const char* pPath) {
    FILE* f;
    char s1[256];
    char s2[256];
    int i;
    size_t len;
    char c;

    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return 0;
    }
    GetALineAndDontArgue(f, s1);
    s2[0] = '\0';
    if (strstr(s1, ".TIF") != NULL) {

        len = strlen(s1);
        for (i = 0; i < (int)len; i++) {
            c = s1[i];
            if (c == '\\') {
                c = *gDir_separator;
            }
            s2[i] = c;
        }
        s2[len] = '\0';
        PathCat(s1, gApplication_path, s2);
        strcpy(pDest, s1);
        fclose(f);
        return 1;
    } else if (strstr(s1, ".PIX") != NULL) {

        len = strlen(s1);
        for (i = 0; i < (int)len; i++) {
            c = s1[i];
            if (c == '\\') {
                c = *gDir_separator;
            }
            s2[i] = c;
        }
        s2[len] = '\0';
        PathCat(s1, gApplication_path, s2);
        strcpy(pDest, s1);
        fclose(f);
        return 1;
    } else {
        fclose(f);
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00486b20
int C2_HOOK_FASTCALL WriteFileLink(const char* pLink_dest, const char* pLink_source) {
    char* ptr;
    FILE* f;
    int i;
    char rel_dest[256];
    char s[256];

    i = 0;
    strcpy(s, pLink_dest);
    ptr = strstr(s, "DATA") + 5;
    while (*ptr != '\0') {
        if (*ptr == ':' || *ptr == '\\') {
            rel_dest[i++] = '\\';
        } else {
            rel_dest[i++] = *ptr;
        }
        ptr += 1;
    }
    rel_dest[i] = '\0';
    f = DRfopen(pLink_source, "wt");
    if (f != NULL) {
        fputs(rel_dest, f);
        fclose(f);
        return 0;
    } else {
        return 1;
    }
}

// PDmkdir

// FUNCTION: CARMA2_HW 0x00485fe0
br_pixelmap* C2_HOOK_FASTCALL LoadTiff8bit(const char *pPath, int pFlags, int *pError_code) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanline_buffer;
    br_uint_8* curSrcPos;
    br_uint_32 x;
    br_uint_8* pixels;
    br_uint_32 y;
    br_uint_16* colorMap_Red;
    br_uint_16* colorMap_Green;
    br_uint_16* colorMap_Blue;

    tif = TIFFOpen(pPath, "r");
    if (tif == NULL) {
        *pError_code = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *pError_code = 2;
        return NULL;
    }
    pixels = pm->pixels;
    if (samples_per_pixel == 1) {
        for (y = 0; y < height; y++) {
            if (TIFFReadScanline(tif, pixels + y * pm->row_bytes, y, 0) <= 0) {
                break;
            }
        }
    } else {
        scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
        if (scanline_buffer == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *pError_code = 2;
            return NULL;
        }
        for (y = 0; y < height; y++) {
            if (TIFFReadScanline(tif, scanline_buffer, y, 0) <= 0) {
                break;
            }
            for (x = 0; x < width; x++) {
                pixels[y * pm->row_bytes + x] = (scanline_buffer[x * samples_per_pixel + 1] < 0x80) ? 0 : scanline_buffer[x * samples_per_pixel];
            }
        }
        _TIFFfree(scanline_buffer);
    }
    if (y != height) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *pError_code = 4;
        return NULL;
    }
    if ((pFlags & kLoadTextureFlags_KeepShadeTable) != 0) {
        TIFFGetField(tif, TIFFTAG_COLORMAP, &colorMap_Red, &colorMap_Green, &colorMap_Blue);
        pm->map = CreatePalettePixelmapFromRGBChannels(colorMap_Red, colorMap_Green, colorMap_Blue, pFlags & kLoadTextureFlags_PalatteRGB555);
        if (pm->map == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *pError_code = 2;
            return NULL;
        }
        BrResAdd(pm, pm->map);
    }
    TIFFClose(tif);
    return pm;
}

// FUNCTION: CARMA2_HW 0x004864a0
br_pixelmap* C2_HOOK_FASTCALL LoadTiff16bit(const char *pPath, int pFlags, int *pError_code) {
    TIFF* tif;
    br_pixelmap* pm;
    int pixel_type;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanline_buffer;
    br_uint_16* pixels;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_32 red;
    br_uint_32 grn;
    br_uint_32 blu;
    br_uint_32 alp;
    br_uint_8* curSrcPos;

    tif = TIFFOpen(pPath, "r");
    if (tif == NULL) {
        *pError_code = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    if (samples_per_pixel == 3) {
        pixel_type = BR_PMT_RGB_565;
    } else {
        if (pFlags & kLoadTextureFlags_UseARGB1555) {
            pixel_type = BR_PMT_ARGB_1555;
        } else {
            pixel_type = BR_PMT_RGBA_4444;
        }
    }
    pm = BrPixelmapAllocate(pixel_type, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *pError_code = 2;
        return NULL;
    }
    pixels = pm->pixels;
    scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
    if (scanline_buffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }

    for (y = 0; y < height && TIFFReadScanline(tif, scanline_buffer, y, 0) > 0; y++) {

        if (samples_per_pixel == 3) {
            // RGB888 -> RGB565
            for (x = 0; x < width; x++) {
                red = scanline_buffer[3 * x + 0] >> 3;
                grn = scanline_buffer[3 * x + 1] >> 2;
                blu = scanline_buffer[3 * x + 2] >> 3;
                pixels[(y * (int)pm->row_bytes >> 1) + x] = (red * 0x40 | grn) * 0x20 | blu;
            }
        }
        else if (pFlags & kLoadTextureFlags_UseARGB1555) {
            // RGBA8888 -> ARGB1555
            for (x = 0; x < width; x++) {
                red = scanline_buffer[4 * x + 0] >> 3;
                grn = scanline_buffer[4 * x + 1] >> 3;
                blu = scanline_buffer[4 * x + 2] >> 3;
                alp = scanline_buffer[4 * x + 3] >= 0x80;
                pixels[(y * (int)pm->row_bytes >> 1) + x] = ((alp * 0x20 | red) * 0x20 | grn) * 0x20 | blu;
            }
        } else {
            // RGBA8888 -> ARGB4444
            for (x = 0; x < width; x++) {
                red = scanline_buffer[4 * x + 0] >> 4;
                grn = scanline_buffer[4 * x + 1] >> 4;
                blu = scanline_buffer[4 * x + 2] >> 4;
                alp = scanline_buffer[4 * x + 3] >> 4;
                pixels[(y * (int)pm->row_bytes >> 1) + x] = ((alp * 0x10 | red) * 0x10 | grn) * 0x10 | blu;
            }
        }
    }
    _TIFFfree(scanline_buffer);
    TIFFClose(tif);
    if (y != height) {
        BrPixelmapFree(pm);
        *pError_code = 4;
        return NULL;
    }
    return pm;
}

static __inline br_pixelmap* C2_HOOK_FASTCALL LoadTiffUsingPalette(const char* pPath, br_pixelmap* pPalette, int pFlags, int* pError_code) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanline_buffer;
    br_uint_32 x;
    br_uint_32 y;

    tif = TIFFOpen(pPath, "r");
    if (tif == NULL) {
        *pError_code = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *pError_code = 2;
        return NULL;
    }
    scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
    if (scanline_buffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }
    for (y = 0; y < height && TIFFReadScanline(tif, scanline_buffer, y, 0) > 0; y++) {
        br_uint_8* curSrcPos = scanline_buffer;

        if (samples_per_pixel == 3) {
            for (x = 0; x < width; x++) {
#pragma inline_depth(0)
                ((br_uint_8*)pm->pixels)[y * pm->row_bytes + x] = FindPaletteColour(BR_COLOUR_RGB(curSrcPos[0], curSrcPos[1], curSrcPos[2]), pPalette);
#pragma inline_depth()
                curSrcPos += 3;
            }
        } else {
            for (x = 0; x < width; x++) {
                if (curSrcPos[3] < 0x80) {
                    ((br_uint_8*)pm->pixels)[y * pm->row_bytes + x] = 0;
                } else {
#pragma inline_depth(0)
                    ((br_uint_8*)pm->pixels)[y * pm->row_bytes + x] = FindPaletteColour(BR_COLOUR_RGB(curSrcPos[0], curSrcPos[1], curSrcPos[2]), pPalette);
#pragma inline_depth()
                }
                curSrcPos += 4;
            }
        }
    }
    _TIFFfree(scanline_buffer);
    TIFFClose(tif);
    if (y != height) {
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00486240
int C2_HOOK_FASTCALL DRstrrcasestr(int* pOffset, const char* pHaystack, int pLength_haystack, const char* pNeedle) {
    int len_needle;
    int pos_haystack;

    len_needle = strlen(pNeedle);
    (void)strlen(pHaystack);
    if (pLength_haystack < len_needle) {
        return 0;
    } else {
        pos_haystack = pLength_haystack - len_needle;
        while (pos_haystack >= 0 && DRmemicmp(&pHaystack[pos_haystack], pNeedle, len_needle) != 0) {
            pos_haystack -= 1;
        }
        if (pos_haystack < 0) {
            return 0;
        }
        *pOffset = pos_haystack;
        return 1;
    }
}

static __inline int BuildPathWithDirAndSuffix(char* pDest_dir_path, char* pDest_file_path, const char* pOld_path, const char* pNew_dirname, const char *pOld_suffix, const char* pNew_suffix) {
    int pos_suffix;
    int pos_filename_sep;
    int pos_dir_sep;
    int pos_endsep;

    if (DRstrrcasestr(&pos_suffix, pOld_path, strlen(pOld_path), pOld_suffix)
            && DRstrrcasestr(&pos_filename_sep, pOld_path, pos_suffix, gDir_separator)
            && DRstrrcasestr(&pos_dir_sep, pOld_path, pos_filename_sep, gDir_separator)) {
        pos_endsep = pos_filename_sep + strlen(gDir_separator);
        sprintf(pDest_dir_path, "%.*s%s%s", pos_dir_sep, pOld_path, gDir_separator, pNew_dirname);
        sprintf(pDest_file_path, "%s%s%.*s%s", pDest_dir_path, gDir_separator, pos_suffix - pos_endsep, pOld_path + pos_endsep, pNew_suffix);
        return 1;
    } else {
        return 0;
    }
}

br_uint_32 C2_HOOK_FASTCALL RGB565Error(br_colour pColour, br_uint_16 pRGB565) {

    return CARPOCALYPSE2_SQR(((pColour >> 16) & 0xff) - ((pRGB565 >> 8) & 0xf8))
            + CARPOCALYPSE2_SQR(((pColour >> 8) & 0xff) - ((pRGB565 >> 3) & 0xfc))
            + CARPOCALYPSE2_SQR(((pColour >> 0) & 0xff) - ((pRGB565 & 0x1f) << 3));
}

int C2_HOOK_FASTCALL FindPaletteColourRGB565(br_colour pColour, br_pixelmap *pPalette) {
    br_uint_16 *pixel;
    int best_index;
    br_uint_32 best_error;
    br_uint_32 error;
    int i;

    pixel = pPalette->pixels;
    best_index = 1;
    best_error = RGB565Error(pColour, pixel[1]);
    for (i = 2; i < 256; i++) {

        error = RGB565Error(pColour, pixel[i]);
        if (error < best_error) {
            best_error = error;
            best_index = i;
        }
    }
    return best_index;
}

br_uint_32 C2_HOOK_FASTCALL XRGB8888Error(br_colour pColour, br_uint_32 pXRGB8888) {

    return CARPOCALYPSE2_SQR(((pColour >> 16) & 0xff) - ((pXRGB8888 >> 16)  & 0xff))
            + CARPOCALYPSE2_SQR(((pColour >> 8) & 0xff) - ((pXRGB8888 >> 8) & 0xff))
            + CARPOCALYPSE2_SQR(((pColour >> 0) & 0xff) - ((pXRGB8888 >> 0) & 0xff));
}

int C2_HOOK_FASTCALL FindPaletteColourXRGB8888(br_colour pColour, br_pixelmap *pPalette) {
    br_uint_32 *pixel;
    int best_index;
    br_uint_32 best_error;
    br_uint_32 error;
    int i;

    pixel = pPalette->pixels;
    best_index = 1;
    best_error = XRGB8888Error(pColour, pixel[1]);
    for (i = 2; i < 256; i++) {

        error = XRGB8888Error(pColour, pixel[i]);
        if (error < best_error) {
            best_error = error;
            best_index = i;
        }
    }
    return best_index;
}

// FUNCTION: CARMA2_HW 0x004862b0
int C2_HOOK_FASTCALL FindPaletteColour(br_colour pColour, br_pixelmap *pPalette) {

    if (pPalette->type == BR_PMT_RGB_565) {
        return FindPaletteColourRGB565(pColour, pPalette);
    } else {
        return FindPaletteColourXRGB8888(pColour, pPalette);
    }
}