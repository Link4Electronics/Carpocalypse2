#include "video.h"

#include "font.h"

#include "errors.h"

#include "globvars.h"
#include "grafdata.h"
#include "graphics.h"

#include "loading.h"

#include "movie.h"

#include "platform.h"
#include "utility.h"

#include <FileTypesAndCreators.h>
#include <GXMath.h>
#include <Movies.h>
#include <QTML.h>
#include <Script.h>
#include <TextUtils.h>

#include <string.h>

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006a0c24
extern int gQuickTime_initialized;
// GLOBAL: CARMA2_HW 0x006a23b8
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x006a0c50
undefined4 gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a0af0
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0af8
int gQuicktime_max_compression_size;

// GLOBAL: CARMA2_HW 0x006a0c30
Movie gQuicktime_movie;

// GLOBAL: CARMA2_HW 0x006a0c34
Handle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
Handle gQuicktime_image_description_handle;

// GLOBAL: CARMA2_HW 0x006a0c3c
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0c38
void* gQuicktime_compressed_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c44
br_uint_16* gQuicktime_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c48
br_uint_16* gQuicktime_frame_pixel_conversion_table;

// GLOBAL: CARMA2_HW 0x006a0af4
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0afc
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0c10
Track gQuicktime_track;

// GLOBAL: CARMA2_HW 0x006a0c2c
int gQuicktime_movie_counter;

// GLOBAL: CARMA2_HW 0x006a0b08
struct FSSpec gQt_file_spec;

// GLOBAL: CARMA2_HW 0x006a0c18
struct Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x0065ef2c
int gQuicktime_frame_flags = 1;

// GLOBAL: CARMA2_HW 0x0065ef34
undefined2 DAT_0065ef34;

// GLOBAL: CARMA2_HW 0x006a0c28
int gRecording_paused;

// GLOBAL: CARMA2_HW 0x006a0c54
tU32 gRecording_time;

// GLOBAL: CARMA2_HW 0x006baa28

// GLOBAL: CARMA2_HW 0x006baa28
int gRecording_mouse_disabled;


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern int C2_HOOK_CDECL FUN_0056a920();
extern OSErr C2_HOOK_CDECL FUN_0056abe0();
extern int C2_HOOK_CDECL FUN_0056b340();
extern short C2_HOOK_CDECL FUN_0056b440();
extern OSErr C2_HOOK_CDECL FUN_0056baa0();
extern Track C2_HOOK_CDECL FUN_0056c640();
extern Media C2_HOOK_CDECL FUN_0056c6e0();
extern int C2_HOOK_CDECL FUN_0056d7e0();
extern int C2_HOOK_CDECL FUN_0056d8a0();
extern short C2_HOOK_CDECL FUN_0056d8c0();
extern void* C2_HOOK_CDECL FUN_0056fde0();
extern int C2_HOOK_CDECL FUN_005705c0();
extern Handle C2_HOOK_CDECL FUN_00570600();
extern void C2_HOOK_CDECL FUN_005707e0();
extern void C2_HOOK_CDECL FUN_00570a40();
extern void C2_HOOK_CDECL FUN_00570ac0();
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern void C2_HOOK_FASTCALL PDFileDelete(const char* pPath, int pIgnore_read_only);
extern void C2_HOOK_FASTCALL PossibleLock(int pValue);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
extern void C2_HOOK_CDECL dr_dprintf();
extern tGraf_data* gCurrent_graf_data;
extern int gNbPixelBits;
extern int gQuickTime_compressor_code;
extern int gQuick_time_banner_number;
extern char gQuick_time_banner_texture_name[256];
extern char gQuick_time_temp_path[256];
extern br_pixelmap* gRender_palette;
#endif
// FUNCTION: CARMA2_HW 0x004e2100
int C2_HOOK_FASTCALL GetNextReplayRecordState(void) {
    return gNext_replay_record_state;
}

// FUNCTION: CARMA2_HW 0x004e2110
void C2_HOOK_FASTCALL KillStatusMessage(void) {
    gShow_status_message = 0;
}

// FUNCTION: CARMA2_HW 0x004e1400
void C2_NAKED C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int width;
    int height;
    int pixel_step;
    int top;
    int row;
    int col;

    (void)pFrame_pixmap_handle;
    if (gQuicktime_frame_pixel_conversion_table == NULL) {
        gQuicktime_frame_pixel_conversion_table = BrMemAllocate(0x20000, 0xfd);
        if (gNbPixelBits == 0x10) {
            i = 0;
            do {
                unsigned int value;
                br_uint_8 blue;

                value = ((unsigned int)i >> 1) & 0x7fe0;
                blue = (br_uint_8)i;
                value |= blue & 0x1f;
                i++;
                gQuicktime_frame_pixel_conversion_table[i - 1] =
                        ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            } while (i < 0x10000);
        } else {
            br_uint_32* palette;

            palette = gRender_palette->pixels;
            for (i = 0; i < 256; i++) {
                br_uint_32 colour;
                unsigned int value;

                colour = palette[i];
                value = (((colour >> 19) & 0x1f) << 5) | ((colour >> 11) & 0x1f);
                value = (value << 5) | ((colour >> 3) & 0x1f);
                gQuicktime_frame_pixel_conversion_table[i] =
                        ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            }
        }
    }

    PossibleLock(1);
    if (gNbPixelBits == 0x10) {
        width = gCurrent_graf_data->width / 2;
    } else {
        width = gCurrent_graf_data->width;
    }
    height = gCurrent_graf_data->height;
    if (gNbPixelBits == 0x10) {
        height /= 2;
    }

    if (gNbPixelBits == 0x10) {
        pixel_step = 2;
        if (pScreen->height == 200) {
            top = 20;
            height = 200;
            pixel_step = 1;
        } else {
            top = 0;
        }
        row = 0;
        if (height > 0) {
            int destination_offset;

            destination_offset = top * width * 2;
            do {
                if (width > 0) {
                    int source_x;
                    int destination_pixel;

                    source_x = 0;
                    destination_pixel = destination_offset / 2;
                    col = width;
                    do {
                        int source_pixel;

                        source_pixel = (pixel_step * row * (int)pScreen->row_bytes) / 2 + source_x;
                        gQuicktime_frame_pixels[destination_pixel] =
                                gQuicktime_frame_pixel_conversion_table[((br_uint_16*)pScreen->pixels)[source_pixel]];
                        destination_pixel++;
                        source_x += pixel_step;
                        col--;
                    } while (col != 0);
                }
                row++;
                destination_offset += width * 2;
            } while (row < height);
            PossibleUnlock(1);
            return;
        }
    } else {
        row = 0;
        do {
            if (width > 0) {
                col = 0;
                do {
                    int source_pixel;

                    source_pixel = row * (int)pScreen->row_bytes + col;
                    gQuicktime_frame_pixels[row * width + col] =
                            gQuicktime_frame_pixel_conversion_table[((br_uint_8*)pScreen->pixels)[source_pixel]];
                    col++;
                } while (col < width);
            }
            row++;
        } while (row < 200);
    }
    PossibleUnlock(1);
#else
    __asm {
            mov eax, dword ptr [gQuicktime_frame_pixel_conversion_table]
            sub esp, 0x14
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            jne L004e14b4
            push 0xfd
            push 0x20000
            call BrMemAllocate
            mov dword ptr [gQuicktime_frame_pixel_conversion_table], eax
            mov eax, dword ptr [gNbPixelBits]
            add esp, 8
            cmp eax, 0x10
            jne L004e1464
            xor ecx, ecx
    L004e1439:
            mov eax, ecx
            mov dl, cl
            shr eax, 1
            and eax, 0x7fe0
            and edx, 0x1f
            or eax, edx
            xor edx, edx
            mov dl, ah
            inc ecx
            mov dh, al
            mov eax, dword ptr [gQuicktime_frame_pixel_conversion_table]
            cmp ecx, 0x10000
            mov word ptr [eax + ecx*2 - 2], dx
            jl L004e1439
            jmp L004e14b4
    L004e1464:
            xor ebx, ebx
            xor esi, esi
    L004e1468:
            mov ecx, dword ptr [gRender_palette]
            add esi, 4
            add ebx, 2
            mov edx, dword ptr [ecx + 8]
            mov ecx, dword ptr [edx + esi - 4]
            mov eax, ecx
            mov edx, ecx
            shr eax, 0x13
            and eax, 0x1f
            shr edx, 0xb
            and edx, 0x1f
            shl eax, 5
            shr ecx, 3
            or eax, edx
            mov edx, dword ptr [gQuicktime_frame_pixel_conversion_table]
            and ecx, 0x1f
            shl eax, 5
            or eax, ecx
            xor ecx, ecx
            mov cl, ah
            cmp esi, 0x400
            mov ch, al
            mov word ptr [edx + ebx - 2], cx
            jl L004e1468
    L004e14b4:
            mov ecx, 1
            call PossibleLock
            mov esi, dword ptr [gNbPixelBits]
            mov ecx, dword ptr [gCurrent_graf_data]
            cmp esi, 0x10
            jne L004e14da
            mov eax, dword ptr [ecx]
            cdq
            sub eax, edx
            mov ebp, eax
            sar ebp, 1
            jmp L004e14dc
    L004e14da:
            mov ebp, dword ptr [ecx]
    L004e14dc:
            mov eax, dword ptr [ecx + 4]
            cmp esi, 0x10
            mov dword ptr [esp + 0x20], ebp
            jne L004e14ed
            cdq
            sub eax, edx
            sar eax, 1
    L004e14ed:
            cmp esi, 0x10
            mov dword ptr [esp + 0x14], eax
            mov ebx, 2
            jne L004e15bb
            cmp word ptr [edi + 0x36], 0xc8
            jne L004e151c
            mov eax, 0xc8
            mov ecx, 0x14
            mov dword ptr [esp + 0x14], eax
            mov ebx, 1
            jmp L004e151e
    L004e151c:
            xor ecx, ecx
    L004e151e:
            test eax, eax
            mov dword ptr [esp + 0x10], 0
            jle L004e1620
            imul ecx, ebp
            shl ecx, 1
            mov dword ptr [esp + 0x18], ecx
    L004e1537:
            test ebp, ebp
            jle L004e158e
            mov ecx, dword ptr [esp + 0x18]
            xor esi, esi
            mov dword ptr [esp + 0x1c], ebp
    L004e1545:
            mov eax, ebx
            xor ebp, ebp
            imul eax, dword ptr [esp + 0x10]
            movsx edx, word ptr [edi + 0x28]
            imul eax, edx
            cdq
            sub eax, edx
            mov edx, dword ptr [edi + 8]
            sar eax, 1
            add eax, esi
            add ecx, 2
            add esi, ebx
            mov bp, word ptr [edx + eax*2]
            mov eax, dword ptr [gQuicktime_frame_pixel_conversion_table]
            mov dx, word ptr [eax + ebp*2]
            mov eax, dword ptr [gQuicktime_frame_pixels]
            mov word ptr [eax + ecx - 2], dx
            mov eax, dword ptr [esp + 0x1c]
            dec eax
            mov dword ptr [esp + 0x1c], eax
            jne L004e1545
            mov eax, dword ptr [esp + 0x14]
            mov ebp, dword ptr [esp + 0x20]
    L004e158e:
            mov edx, dword ptr [esp + 0x10]
            mov esi, dword ptr [esp + 0x18]
            lea ecx, [ebp + ebp]
            inc edx
            add esi, ecx
            cmp edx, eax
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x18], esi
            jl L004e1537
            mov ecx, 1
            call PossibleUnlock
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004e15bb:
            xor eax, eax
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x18], eax
    L004e15c5:
            xor eax, eax
            test ebp, ebp
            jle L004e1601
            mov ecx, dword ptr [esp + 0x18]
    L004e15cf:
            movsx edx, word ptr [edi + 0x28]
            mov esi, dword ptr [esp + 0x10]
            xor ebx, ebx
            imul esi, edx
            mov edx, dword ptr [edi + 8]
            add esi, eax
            inc eax
            add ecx, 2
            mov bl, byte ptr [esi + edx]
            mov edx, dword ptr [gQuicktime_frame_pixel_conversion_table]
            mov esi, dword ptr [gQuicktime_frame_pixels]
            cmp eax, ebp
            mov dx, word ptr [edx + ebx*2]
            mov word ptr [esi + ecx - 2], dx
            jl L004e15cf
    L004e1601:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x18]
            lea eax, [ebp + ebp]
            inc ecx
            add edx, eax
            cmp ecx, 0xc8
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x18], edx
            jl L004e15c5
    L004e1620:
            mov ecx, 1
            call PossibleUnlock
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
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
            mov ax, word ptr [gQuicktime_movie_file]
            sub esp, 0x104
            test ax, ax
            je L004e165a
            push eax
            call FUN_0056baa0
            add esp, 4
    L004e165a:
            mov eax, dword ptr [gQuicktime_movie]
            push eax
            call FUN_0056abe0
            add esp, 4
            mov edx, 1
            mov ecx, offset gQuick_time_temp_path
            call PDFileDelete
            lea ecx, [esp]
            push offset gQuick_time_temp_path
            push offset DAT_0065ef34+0x5c
            push ecx
            call sprintf
            add esp, 0xc
            mov edx, 1
            lea ecx, [esp]
            call PDFileDelete
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            test eax, eax
            je L004e16b8
            push eax
            call FUN_00570ac0
            add esp, 4
            mov dword ptr [gQuicktime_frame_pixmap_handle], 0
    L004e16b8:
            mov eax, dword ptr [gQuicktime_image_description_handle]
            test eax, eax
            je L004e16d4
            push eax
            call FUN_00570ac0
            add esp, 4
            mov dword ptr [gQuicktime_image_description_handle], 0
    L004e16d4:
            mov eax, dword ptr [gQuicktime_compressed_frame_handle]
            test eax, eax
            je L004e16f0
            push eax
            call FUN_00570ac0
            add esp, 4
            mov dword ptr [gQuicktime_compressed_frame_handle], 0
    L004e16f0:
            add esp, 0x104
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

// FUNCTION: CARMA2_HW 0x004e2120
void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2) {
    gShow_status_message = 1;
    gStatus_message_1 = msg1;
    gStatus_message_2 = msg2;
}

// FUNCTION: CARMA2_HW 0x004e2190
void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    undefined4 state;

    state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = state;
}

void C2_HOOK_FASTCALL FreeOffQTshite(void) {
    tPath_name path;

    if (gQuicktime_movie_file != 0) {
        CloseMovieFile(gQuicktime_movie_file);
    }
    DisposeMovie(gQuicktime_movie);
    PDFileDelete(gQuick_time_temp_path, 1);
    sprintf(path, "%s.#Res", gQuick_time_temp_path);
    PDFileDelete(path, 1);
    if (gQuicktime_frame_pixmap_handle != NULL) {
        DisposeHandle(gQuicktime_frame_pixmap_handle);
        gQuicktime_frame_pixmap_handle = NULL;
    }
    if (gQuicktime_image_description_handle != NULL) {
        DisposeHandle(gQuicktime_image_description_handle);
        gQuicktime_image_description_handle = NULL;
    }
    if (gQuicktime_compressed_frame_handle != NULL) {
        DisposeHandle(gQuicktime_compressed_frame_handle);
        gQuicktime_compressed_frame_handle = NULL;
    }
}



// FUNCTION: CARMA2_HW 0x004e2140
int C2_HOOK_FASTCALL GetMovieWidth(void) {
    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
int C2_HOOK_FASTCALL GetMovieHeight(void) {
    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e0f20
OSErr C2_HOOK_FASTCALL InitMovie(void) {

    gQuicktime_rect.top = 0;
    gQuicktime_rect.left = 0;
    gQuicktime_rect.right = GetMovieWidth();
    gQuicktime_rect.bottom = GetMovieHeight();
    if (CreateMovieFile(&gQt_file_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie) != noErr) {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
        return CreateMovieFile(&gQt_file_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie);
    }
}

// FUNCTION: CARMA2_HW 0x004e0fe0
int C2_NAKED C2_HOOK_FASTCALL CreateMovie(void) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
    return 0;
#else
    __asm {
            push esi
            mov esi, dword ptr [gCurrent_graf_data]
            push edi
            mov edi, dword ptr [gNbPixelBits]
            cmp edi, 0x10
            jne L004e0fff
            mov eax, dword ptr [esi + 4]
            cdq
            sub eax, edx
            mov ecx, eax
            sar ecx, 1
            jmp L004e1002
    L004e0fff:
            mov ecx, dword ptr [esi + 4]
    L004e1002:
            cmp edi, 0x10
            jne L004e1012
            mov eax, dword ptr [esi]
            cdq
            sub eax, edx
            mov esi, eax
            sar esi, 1
            jmp L004e1014
    L004e1012:
            mov esi, dword ptr [esi]
    L004e1014:
            push 0x100
            lea eax, [ecx + ecx]
            push 1
            push eax
            call FUN_0056fde0
            add esp, 8
            lea ecx, [esi + esi]
            push eax
            push 1
            push ecx
            call FUN_0056fde0
            mov edx, dword ptr [gQuicktime_movie]
            add esp, 8
            push eax
            push edx
            call FUN_0056c640
            add esp, 0x10
            mov dword ptr [gQuicktime_track], eax
            call FUN_0056a920
            test ax, ax
            jne L004e124b
            mov eax, dword ptr [gQuicktime_track]
            push 0
            push 0
            push 0x3e8
            push 0x76696465
            push eax
            call FUN_0056c6e0
            add esp, 0x14
            mov dword ptr [gQuicktime_media], eax
            call FUN_0056a920
            test ax, ax
            jne L004e124b
            mov ecx, dword ptr [gQuicktime_media]
            push ecx
            call FUN_0056b340
            add esp, 4
            test ax, ax
            jne L004e124b
            push 0x32
            call FUN_00570600
            add esp, 4
            mov dword ptr [gQuicktime_frame_pixmap_handle], eax
            call FUN_005705c0
            test ax, ax
            jne L004e124b
            mov edx, dword ptr [gQuicktime_frame_pixmap_handle]
            mov ecx, 0xc
            xor eax, eax
            push offset gQuicktime_max_compression_size
            mov edi, dword ptr [edx]
            push -2
            rep stosd
            stosw
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            mov ecx, dword ptr [eax]
            mov word ptr [ecx + 0x1e], 0x10
            mov edx, dword ptr [gQuicktime_frame_pixmap_handle]
            mov eax, dword ptr [edx]
            mov word ptr [eax + 0x20], 0x10
            mov ecx, dword ptr [gQuicktime_frame_pixmap_handle]
            mov edx, dword ptr [ecx]
            mov word ptr [edx + 0x22], 3
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            mov ecx, dword ptr [eax]
            mov word ptr [ecx + 0x24], 5
            mov edx, dword ptr [gQuickTime_compressor_code]
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            push edx
            push 0x200
            push 0
            push offset gQuicktime_rect
            push eax
            call FUN_0056d7e0
            add esp, 0x1c
            test ax, ax
            jne L004e124b
            mov ecx, dword ptr [gQuicktime_max_compression_size]
            push ecx
            call FUN_00570600
            add esp, 4
            mov dword ptr [gQuicktime_compressed_frame_handle], eax
            call FUN_005705c0
            test ax, ax
            jne L004e124b
            mov edx, dword ptr [gQuicktime_compressed_frame_handle]
            push edx
            call FUN_00570a40
            mov eax, dword ptr [gQuicktime_compressed_frame_handle]
            add esp, 4
            push eax
            call FUN_005707e0
            mov ecx, dword ptr [gQuicktime_compressed_frame_handle]
            add esp, 4
            mov edx, dword ptr [ecx]
            push 4
            mov dword ptr [gQuicktime_compressed_frame_pixels], edx
            call FUN_00570600
            add esp, 4
            mov dword ptr [gQuicktime_image_description_handle], eax
            call FUN_005705c0
            test ax, ax
            jne L004e124b
            mov eax, dword ptr [gQuicktime_frame_pixels]
            test eax, eax
            jne L004e11f8
            mov edi, dword ptr [gNbPixelBits]
            mov esi, dword ptr [gCurrent_graf_data]
            cmp edi, 0x10
            jne L004e11c6
            mov eax, dword ptr [esi]
            cdq
            sub eax, edx
            mov ecx, eax
            sar ecx, 1
            jmp L004e11c8
    L004e11c6:
            mov ecx, dword ptr [esi]
    L004e11c8:
            mov eax, dword ptr [esi + 4]
            cmp edi, 0x10
            jne L004e11d5
            cdq
            sub eax, edx
            sar eax, 1
    L004e11d5:
            imul eax, ecx
            shl eax, 1
            push 0xfd
            push eax
            call BrMemAllocate
            add esp, 8
            mov dword ptr [gQuicktime_frame_pixels], eax
            test eax, eax
            jne L004e11f8
            mov ax, 1
            pop edi
            pop esi
            ret
    L004e11f8:
            mov eax, dword ptr [gQuicktime_image_description_handle]
            mov ecx, dword ptr [gQuickTime_compressor_code]
            mov dx, word ptr [DAT_0065ef34]
            push eax
            push 0xc0
            push 0
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            push 0xf
            push 0x200
            push 0x200
            push -2
            push ecx
            push edx
            push offset gQuicktime_rect
            push offset gQuicktime_rect
            push 0
            push eax
            push offset gQuicktime_image_sequence
            call FUN_0056d8a0
            add esp, 0x38
            test ax, ax
            jne L004e124b
            call L004e1250
    L004e124b:
            pop edi
            pop esi
            ret
            nop
            nop
    L004e1250:
            mov eax, dword ptr [gQuick_time_banner_number]
            sub esp, 8
            test eax, eax
            push ebx
            push esi
            push edi
            je L004e13e4
            mov ecx, offset gQuick_time_banner_texture_name
            call LoadPixelmap
            mov esi, eax
            test esi, esi
            je L004e13e4
            cmp word ptr [esi + 0x34], 0x140
            jne L004e1287
            cmp word ptr [esi + 0x36], 0xc8
            je L004e1294
    L004e1287:
            push offset DAT_0065ef34+0x3c
            call BrFailure
            add esp, 4
    L004e1294:
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            mov edx, dword ptr [gQuicktime_frame_pixels]
            mov ecx, dword ptr [eax]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [gNbPixelBits]
            cmp eax, 0x10
            jne L004e12bb
            mov eax, dword ptr [gCurrent_graf_data]
            mov eax, dword ptr [eax]
            cdq
            sub eax, edx
            sar eax, 1
            jmp L004e12c3
    L004e12bb:
            mov ecx, dword ptr [gCurrent_graf_data]
            mov eax, dword ptr [ecx]
    L004e12c3:
            lea edx, [eax + eax]
            mov eax, dword ptr [gQuicktime_frame_pixmap_handle]
            mov ecx, dword ptr [eax]
            mov word ptr [ecx + 4], dx
            mov ebx, dword ptr [gNbPixelBits]
            mov ecx, dword ptr [gCurrent_graf_data]
            cmp ebx, 0x10
            jne L004e12ed
            mov eax, dword ptr [ecx]
            cdq
            sub eax, edx
            mov edi, eax
            sar edi, 1
            jmp L004e12ef
    L004e12ed:
            mov edi, dword ptr [ecx]
    L004e12ef:
            mov eax, dword ptr [ecx + 4]
            cmp ebx, 0x10
            jne L004e12fc
            cdq
            sub eax, edx
            sar eax, 1
    L004e12fc:
            mov ecx, eax
            xor eax, eax
            imul ecx, edi
            mov edi, dword ptr [gQuicktime_frame_pixels]
            shl ecx, 1
            mov edx, ecx
            shr ecx, 2
            rep stosd
            mov ecx, edx
            xor edx, edx
            and ecx, 3
            rep stosb
            mov dx, word ptr [esi + 0x34]
            xor eax, eax
            mov al, byte ptr [esi + 0x2c]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x36]
            push eax
            push ecx
            push edx
            push offset DAT_0065ef34+0x14
            call dr_dprintf
            mov ecx, dword ptr [gQuicktime_frame_pixmap_handle]
            add esp, 0x10
            mov edx, esi
            call FUN_004e1400
            xor esi, esi
    L004e1349:
            mov edx, dword ptr [gQuicktime_compressed_frame_handle]
            lea eax, [esp + 0xf]
            push 0
            push eax
            mov eax, dword ptr [edx]
            mov edx, dword ptr [gQuicktime_image_sequence]
            lea ecx, [esp + 0x18]
            push ecx
            mov ecx, dword ptr [gQuicktime_frame_pixmap_handle]
            push eax
            push 0xc0
            push offset gQuicktime_rect
            push ecx
            push edx
            call FUN_0056d8c0
            add esp, 0x20
            test ax, ax
            jne L004e13da
            mov eax, dword ptr [gQuicktime_image_description_handle]
            mov edx, dword ptr [esp + 0x10]
            push 0
            push 0
            mov ecx, dword ptr [eax]
            push 1
            mov dword ptr [ecx + 0x2c], edx
            mov eax, dword ptr [gQuicktime_image_description_handle]
            mov edx, dword ptr [gQuicktime_compressed_frame_handle]
            push eax
            mov eax, dword ptr [eax]
            push 0x37
            mov ecx, dword ptr [eax + 0x2c]
            mov eax, dword ptr [gQuicktime_media]
            push ecx
            push 0
            push edx
            push eax
            call FUN_0056b440
            add esp, 0x24
            test ax, ax
            jne L004e13da
            inc esi
            cmp esi, 0x12
            jl L004e1349
            mov dword ptr [gQuicktime_frame_flags], 1
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 8
            ret
    L004e13da:
            movsx eax, ax
            pop edi
            pop esi
            pop ebx
            add esp, 8
            ret
    L004e13e4:
            pop edi
            pop esi
            mov dword ptr [gQuicktime_frame_flags], 1
            xor eax, eax
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e1c70
short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    int frame_flags;
    int pixmap_width;
    int frame_size;
    char codec_info;
    short result;

    if (gQuicktime_frame_flags != 0) {
        frame_flags = 0xc0;
        gQuicktime_frame_flags = 0;
    } else {
        frame_flags = 0x40;
    }
    PossibleLock(1);
    *(void**)(*gQuicktime_frame_pixmap_handle) = gQuicktime_frame_pixels;
    if (gNbPixelBits == 16) {
        pixmap_width = gCurrent_graf_data->width / 2;
    } else {
        pixmap_width = gCurrent_graf_data->width;
    }
    *(short*)(*gQuicktime_frame_pixmap_handle + 4) = pixmap_width * 2;
    FUN_004e1400(gQuicktime_frame_pixmap_handle, pScreen);
    result = FUN_0056d8c0(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle,
            &gQuicktime_rect, frame_flags, *gQuicktime_compressed_frame_handle,
            &frame_size, &codec_info, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    *(int*)(*gQuicktime_image_description_handle + 0x2c) = frame_size;
    result = FUN_0056b440(gQuicktime_media, gQuicktime_compressed_frame_handle, 0,
            *(int*)(*gQuicktime_image_description_handle + 0x2c),
            0x37, gQuicktime_image_description_handle, 1, 0, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    PossibleUnlock(1);
    return 0;
}

void C2_HOOK_FASTCALL TrapS3SoundOutput(void) {
}

void C2_HOOK_FASTCALL FreeS3SoundOutput(void) {
}

void C2_HOOK_FASTCALL WriteBannerFrame(void) {
    // Placeholder implementation - actual implementation should be in movie.c or another file
    // This is needed to resolve the undefined reference error
}

// FUNCTION: CARMA2_HW 0x004e1da0
int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap *pScreen, int pFrame_period) {
    tU32 remaining;

    switch (gNext_replay_record_state) {
    case 1:
        gNext_replay_record_state = 2;
        return 0;
    case 2:
        WaitFor(2000);
        if (InitMovie()) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            return 0;
        }
        gNext_replay_record_state = 3;
        ShowStatusMessage(eMiscString_initialising_movie, eMiscString_please_wait);
        return 0;
    case 3:
        if (CreateMovie()) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            return 0;
        }
        gNext_replay_record_state = 4;
        ShowStatusMessage(eMiscString_recording_movie, 0);
        TrapS3SoundOutput();
        return 0;
    case 4:
        if (!gRecording_paused) {
            remaining = gRecording_time + pFrame_period;
            gRecording_paused = 0;
            for (; remaining > 1000 / 18; remaining -= 1000 / 18) {
                if (GenerateOneMovieFrame(pScreen) != 0) {
                    gNext_replay_record_state = 10;
                    ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
                    return 0;
                }
            }
            gRecording_time = remaining;
            return 0;
        }
        break;
    case 5:
        gRecording_mouse_disabled = 0;
        return 0;
    case 10:
        WaitFor(2000);
        FreeOffQTshite();
        FreeS3SoundOutput();
        return 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e7350
void C2_HOOK_FASTCALL DrawStatusMessage(int pReplay_only) {
    int x;
    int y;

    if (gShow_status_message != 0) {
        if (gNext_replay_record_state == 4) {
            x = gCurrent_graf_data->field_0x350;
            y = gCurrent_graf_data->field_0x354;
            if (pReplay_only == 0) {
                DRPixelmapText(gBack_screen, x, y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
            }
        } else {
            x = gCurrent_graf_data->width / 2;
            y = gCurrent_graf_data->height / 2;
            DRPixelmapCentredText(gBack_screen, x, y - 0x40, &gFonts[4], GetMiscString(gStatus_message_1));
            if (gStatus_message_2 != 0) {
                DRPixelmapCentredText(gBack_screen, x, y - 0x20, &gFonts[6], GetMiscString(gStatus_message_2));
            }
        }
    }
}
