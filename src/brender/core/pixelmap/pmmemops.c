#include "pmmemops.h"

#include "brender/br_defs.h"

#include "c2_stdlib.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0066fd0c
br_uint_8 g_startMasks[9] = {
    0xff, 0x7f, 0x3f, 0x1f,
    0x0f, 0x07, 0x03, 0x01,
    0x00,
};

// GLOBAL: CARMA2_HW 0x0066fd15
br_uint_8 g_endMasks[9] = {
    0x00,
    0x80, 0xc0, 0xe0, 0xf0,
    0xf8, 0xfc, 0xfe, 0xff,
};

#define WRITE_COLOUR_U8(DEST, COLOUR)               \
    do {                                            \
        *(br_uint_8*)(DEST) = (br_uint_8)(COLOUR);  \
    } while (0)
#define WRITE_COLOUR_U16(DEST, COLOUR)                  \
    do {                                                \
        *(br_uint_16*)(DEST) = (br_uint_16)(COLOUR);    \
    } while (0)
#define WRITE_COLOUR_U24(DEST, COLOUR)  \
    do {                                \
        ((br_uint_8*)(DEST))[0] = b0;   \
        ((br_uint_8*)(DEST))[1] = b1;   \
        ((br_uint_8*)(DEST))[2] = b2;   \
    } while (0)
#define WRITE_COLOUR_U32(DEST, COLOUR)                  \
    do {                                                \
        *(br_uint_32*)(DEST) = (br_uint_32)(COLOUR);    \
    } while (0)

#define COLOR_BITS_BYTE(SIZE, WRITE, BITS, DEST, SRC, COLOUR)   \
    do {                                                        \
        if ((BITS) & 0x80) {                                    \
            WRITE(((br_uint_8*)(DEST) + 0 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x40) {                                    \
            WRITE(((br_uint_8*)(DEST) + 1 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x20) {                                    \
            WRITE(((br_uint_8*)(DEST) + 2 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x10) {                                    \
            WRITE(((br_uint_8*)(DEST) + 3 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x08) {                                    \
            WRITE(((br_uint_8*)(DEST) + 4 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x04) {                                    \
            WRITE(((br_uint_8*)(DEST) + 5 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x02) {                                    \
            WRITE(((br_uint_8*)(DEST) + 6 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x01) {                                    \
            WRITE(((br_uint_8*)(DEST) + 7 * (SIZE)), COLOUR);   \
        }                                                       \
    } while (0)

#define COLOR_BITS_BLOCK(SIZE, WRITE, SRC, DEST, COLOUR)                                                    \
    do {                                                                                                    \
        br_uint_8* src_ptr = (SRC);                                                                         \
        void* dest_ptr = (DEST);                                                                            \
        for (; height != 0; height--, src_ptr += s_stride, dest_ptr = (br_uint_8*)dest_ptr + dest_stride) { \
            br_uint_32 remaining_row = row_bytes;                                                           \
            br_uint_8 bits = *src_ptr & start_mask;                                                         \
                                                                                                            \
            for (; remaining_row >= 0; remaining_row--) {                                                   \
                COLOR_BITS_BYTE((SIZE), WRITE, bits, dest_ptr, src_ptr, (COLOUR));                          \
                ++src_ptr;                                                                                  \
                dest_ptr = (br_uint_8*)dest_ptr + 8 * (SIZE);                                               \
                bits = *src_ptr;                                                                            \
                if (remaining_row == 0) {                                                                   \
                    bits &= end_mask;                                                                       \
                }                                                                                           \
            }                                                                                               \
        }                                                                                                   \
    } while (0)

#ifdef CARPOCALYPSE2_MATCHING
extern char gData_0066fd20[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_0066fd1e[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_0066fd1e[];
extern char gMatchData_0066fd20[];
#endif
// FUNCTION: CARMA2_HW 0x0053df30
void C2_NAKED C2_HOOK_CDECL pm_mem_copy_bits(void* dest, br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_8 start_mask;
    br_uint_8 end_mask;
    br_uint_32 row_bytes;

    start_mask = g_startMasks[start_bit];
    end_mask = g_endMasks[end_bit & 0x7];
    row_bytes = end_bit >> 3;

    if (row_bytes == 0) {
        switch (bpp) {
        case 1:
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(1, WRITE_COLOUR_U8, bits, dest, src, colour);
            }
            break;
        case 2:
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(2, WRITE_COLOUR_U16, bits, dest, src, colour);
            }
            break;
        case 3: {
            br_uint_8 b0 = (br_uint_8)((colour >>  0) & 0xff);
            br_uint_8 b1 = (br_uint_8)((colour >>  8) & 0xff);
            br_uint_8 b2 = (br_uint_8)((colour >> 16) & 0xff);
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(3, WRITE_COLOUR_U24, bits, dest, src, colour);
            }
            break;
        }
        case 4: {
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(4, WRITE_COLOUR_U32, bits, dest, src, colour);
            }
            break;
        }
        default:
            abort();
            break;
        }
    } else {
        switch (bpp) {
        case 1:
            COLOR_BITS_BLOCK(1, WRITE_COLOUR_U8, src, dest, colour);
            break;
        case 2:
            COLOR_BITS_BLOCK(2, WRITE_COLOUR_U16, src, dest, colour);
            break;
        case 3: {
            br_uint_8 b0 = (br_uint_8)((colour >>  0) & 0xff);
            br_uint_8 b1 = (br_uint_8)((colour >>  8) & 0xff);
            br_uint_8 b2 = (br_uint_8)((colour >> 16) & 0xff);
            COLOR_BITS_BLOCK(3, WRITE_COLOUR_U24, src, dest, colour);
            break;
        }
        case 4:
            COLOR_BITS_BLOCK(4, WRITE_COLOUR_U32, src, dest, colour);
            break;
        default:
            abort();
            break;
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov edi, dword ptr [ebp + 8]
            mov esi, dword ptr [ebp + 0x14]
            mov edx, dword ptr [ebp + 0x1c]
            mov ecx, dword ptr [ebp + 0x20]
            mov eax, ecx
            shr ecx, 3
            and eax, 7
            mov dl, byte ptr [edx + g_startMasks]
            mov dh, byte ptr [eax + g_endMasks]
            mov ebx, dword ptr [ebp + 0x2c]
            mov eax, dword ptr [ebp + 0x28]
            push ds
            mov ds, word ptr [ebp + 0xc]
            test ecx, ecx
            jne L0053df85
            dec eax
            je L0053dfa8
            dec eax
            je L0053dff9
            dec eax
            je L0053e062
            dec eax
            je L0053e0e8
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053df85:
            dec eax
            je L0053e139
            dec eax
            je L0053e19d
            dec eax
            je L0053e219
            dec eax
            je L0053e2b6
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053dfa8:
            and dl, dh
            mov ecx, dword ptr [ebp + 0x24]
    L0053dfad:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L0053dfb8
            mov byte ptr [edi], bl
    L0053dfb8:
            add al, al
            jae L0053dfbf
            mov byte ptr [edi + 1], bl
    L0053dfbf:
            add al, al
            jae L0053dfc6
            mov byte ptr [edi + 2], bl
    L0053dfc6:
            add al, al
            jae L0053dfcd
            mov byte ptr [edi + 3], bl
    L0053dfcd:
            add al, al
            jae L0053dfd4
            mov byte ptr [edi + 4], bl
    L0053dfd4:
            add al, al
            jae L0053dfdb
            mov byte ptr [edi + 5], bl
    L0053dfdb:
            add al, al
            jae L0053dfe2
            mov byte ptr [edi + 6], bl
    L0053dfe2:
            add al, al
            jae L0053dfe9
            mov byte ptr [edi + 7], bl
    L0053dfe9:
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec ecx
            jne L0053dfad
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053dff9:
            and dl, dh
            mov ecx, dword ptr [ebp + 0x24]
    L0053dffe:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L0053e00c
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], bh
    L0053e00c:
            add al, al
            jae L0053e016
            mov byte ptr [edi + 2], bl
            mov byte ptr [edi + 3], bh
    L0053e016:
            add al, al
            jae L0053e020
            mov byte ptr [edi + 4], bl
            mov byte ptr [edi + 5], bh
    L0053e020:
            add al, al
            jae L0053e02a
            mov byte ptr [edi + 6], bl
            mov byte ptr [edi + 7], bh
    L0053e02a:
            add al, al
            jae L0053e034
            mov byte ptr [edi + 8], bl
            mov byte ptr [edi + 9], bh
    L0053e034:
            add al, al
            jae L0053e03e
            mov byte ptr [edi + 0xa], bl
            mov byte ptr [edi + 0xb], bh
    L0053e03e:
            add al, al
            jae L0053e048
            mov byte ptr [edi + 0xc], bl
            mov byte ptr [edi + 0xd], bh
    L0053e048:
            add al, al
            jae L0053e052
            mov byte ptr [edi + 0xe], bl
            mov byte ptr [edi + 0xf], bh
    L0053e052:
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec ecx
            jne L0053dffe
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e062:
            mov eax, ebx
            shr eax, 8
            and dl, dh
            mov ecx, dword ptr [ebp + 0x24]
    L0053e06c:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L0053e07d
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], bh
            mov byte ptr [edi + 2], ah
    L0053e07d:
            add al, al
            jae L0053e08a
            mov byte ptr [edi + 3], bl
            mov byte ptr [edi + 4], bh
            mov byte ptr [edi + 5], ah
    L0053e08a:
            add al, al
            jae L0053e097
            mov byte ptr [edi + 6], bl
            mov byte ptr [edi + 7], bh
            mov byte ptr [edi + 8], ah
    L0053e097:
            add al, al
            jae L0053e0a4
            mov byte ptr [edi + 9], bl
            mov byte ptr [edi + 0xa], bh
            mov byte ptr [edi + 0xb], ah
    L0053e0a4:
            add al, al
            jae L0053e0b1
            mov byte ptr [edi + 0xc], bl
            mov byte ptr [edi + 0xd], bh
            mov byte ptr [edi + 0xe], ah
    L0053e0b1:
            add al, al
            jae L0053e0be
            mov byte ptr [edi + 0xf], bl
            mov byte ptr [edi + 0x10], bh
            mov byte ptr [edi + 0x11], ah
    L0053e0be:
            add al, al
            jae L0053e0cb
            mov byte ptr [edi + 0x12], bl
            mov byte ptr [edi + 0x13], bh
            mov byte ptr [edi + 0x14], ah
    L0053e0cb:
            add al, al
            jae L0053e0d8
            mov byte ptr [edi + 0x15], bl
            mov byte ptr [edi + 0x16], bh
            mov byte ptr [edi + 0x17], ah
    L0053e0d8:
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec ecx
            jne L0053e06c
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e0e8:
            and dl, dh
            mov ecx, dword ptr [ebp + 0x24]
    L0053e0ed:
            mov al, byte ptr ss:[esi]
            and al, dl
            add al, al
            jae L0053e0f8
            mov dword ptr [edi], ebx
    L0053e0f8:
            add al, al
            jae L0053e0ff
            mov dword ptr [edi + 4], ebx
    L0053e0ff:
            add al, al
            jae L0053e106
            mov dword ptr [edi + 8], ebx
    L0053e106:
            add al, al
            jae L0053e10d
            mov dword ptr [edi + 0xc], ebx
    L0053e10d:
            add al, al
            jae L0053e114
            mov dword ptr [edi + 0x10], ebx
    L0053e114:
            add al, al
            jae L0053e11b
            mov dword ptr [edi + 0x14], ebx
    L0053e11b:
            add al, al
            jae L0053e122
            mov dword ptr [edi + 0x18], ebx
    L0053e122:
            add al, al
            jae L0053e129
            mov dword ptr [edi + 0x1c], ebx
    L0053e129:
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec ecx
            jne L0053e0ed
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e139:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L0053e141:
            inc esi
    L0053e142:
            add al, al
            jae L0053e148
            mov byte ptr [edi], bl
    L0053e148:
            add al, al
            jae L0053e14f
            mov byte ptr [edi + 1], bl
    L0053e14f:
            add al, al
            jae L0053e156
            mov byte ptr [edi + 2], bl
    L0053e156:
            add al, al
            jae L0053e15d
            mov byte ptr [edi + 3], bl
    L0053e15d:
            add al, al
            jae L0053e164
            mov byte ptr [edi + 4], bl
    L0053e164:
            add al, al
            jae L0053e16b
            mov byte ptr [edi + 5], bl
    L0053e16b:
            add al, al
            jae L0053e172
            mov byte ptr [edi + 6], bl
    L0053e172:
            add al, al
            jae L0053e179
            mov byte ptr [edi + 7], bl
    L0053e179:
            mov al, byte ptr ss:[esi]
            add edi, 8
            dec ecx
            jg L0053e141
            js L0053e188
            and al, dh
            jmp L0053e142
    L0053e188:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec dword ptr [ebp + 0x24]
            jne L0053e139
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e19d:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L0053e1a5:
            inc esi
    L0053e1a6:
            add al, al
            jae L0053e1af
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], bh
    L0053e1af:
            add al, al
            jae L0053e1b9
            mov byte ptr [edi + 2], bl
            mov byte ptr [edi + 3], bh
    L0053e1b9:
            add al, al
            jae L0053e1c3
            mov byte ptr [edi + 4], bl
            mov byte ptr [edi + 5], bh
    L0053e1c3:
            add al, al
            jae L0053e1cd
            mov byte ptr [edi + 6], bl
            mov byte ptr [edi + 7], bh
    L0053e1cd:
            add al, al
            jae L0053e1d7
            mov byte ptr [edi + 8], bl
            mov byte ptr [edi + 9], bh
    L0053e1d7:
            add al, al
            jae L0053e1e1
            mov byte ptr [edi + 0xa], bl
            mov byte ptr [edi + 0xb], bh
    L0053e1e1:
            add al, al
            jae L0053e1eb
            mov byte ptr [edi + 0xc], bl
            mov byte ptr [edi + 0xd], bh
    L0053e1eb:
            add al, al
            jae L0053e1f5
            mov byte ptr [edi + 0xe], bl
            mov byte ptr [edi + 0xf], bh
    L0053e1f5:
            mov al, byte ptr ss:[esi]
            add edi, 0x10
            dec ecx
            jg L0053e1a5
            js L0053e204
            and al, dh
            jmp L0053e1a6
    L0053e204:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec dword ptr [ebp + 0x24]
            jne L0053e19d
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e219:
            mov eax, ebx
            shr eax, 8
    L0053e21e:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L0053e226:
            inc esi
    L0053e227:
            add al, al
            jae L0053e233
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], bh
            mov byte ptr [edi + 2], ah
    L0053e233:
            add al, al
            jae L0053e240
            mov byte ptr [edi + 3], bl
            mov byte ptr [edi + 4], bh
            mov byte ptr [edi + 5], ah
    L0053e240:
            add al, al
            jae L0053e24d
            mov byte ptr [edi + 6], bl
            mov byte ptr [edi + 7], bh
            mov byte ptr [edi + 8], ah
    L0053e24d:
            add al, al
            jae L0053e25a
            mov byte ptr [edi + 9], bl
            mov byte ptr [edi + 0xa], bh
            mov byte ptr [edi + 0xb], ah
    L0053e25a:
            add al, al
            jae L0053e267
            mov byte ptr [edi + 0xc], bl
            mov byte ptr [edi + 0xd], bh
            mov byte ptr [edi + 0xe], ah
    L0053e267:
            add al, al
            jae L0053e274
            mov byte ptr [edi + 0xf], bl
            mov byte ptr [edi + 0x10], bh
            mov byte ptr [edi + 0x11], ah
    L0053e274:
            add al, al
            jae L0053e281
            mov byte ptr [edi + 0x12], bl
            mov byte ptr [edi + 0x13], bh
            mov byte ptr [edi + 0x14], ah
    L0053e281:
            add al, al
            jae L0053e28e
            mov byte ptr [edi + 0x15], bl
            mov byte ptr [edi + 0x16], bh
            mov byte ptr [edi + 0x17], ah
    L0053e28e:
            mov al, byte ptr ss:[esi]
            add edi, 0x18
            dec ecx
            jg L0053e226
            js L0053e29d
            and al, dh
            jmp L0053e227
    L0053e29d:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec dword ptr [ebp + 0x24]
            jne L0053e21e
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e2b6:
            push ecx
            push esi
            push edi
            mov al, byte ptr ss:[esi]
            and al, dl
    L0053e2be:
            inc esi
    L0053e2bf:
            add al, al
            jae L0053e2c5
            mov dword ptr [edi], ebx
    L0053e2c5:
            add al, al
            jae L0053e2cc
            mov dword ptr [edi + 4], ebx
    L0053e2cc:
            add al, al
            jae L0053e2d3
            mov dword ptr [edi + 8], ebx
    L0053e2d3:
            add al, al
            jae L0053e2da
            mov dword ptr [edi + 0xc], ebx
    L0053e2da:
            add al, al
            jae L0053e2e1
            mov dword ptr [edi + 0x10], ebx
    L0053e2e1:
            add al, al
            jae L0053e2e8
            mov dword ptr [edi + 0x14], ebx
    L0053e2e8:
            add al, al
            jae L0053e2ef
            mov dword ptr [edi + 0x18], ebx
    L0053e2ef:
            add al, al
            jae L0053e2f6
            mov dword ptr [edi + 0x1c], ebx
    L0053e2f6:
            mov al, byte ptr ss:[esi]
            add edi, 0x20
            dec ecx
            jg L0053e2be
            js L0053e305
            and al, dh
            jmp L0053e2bf
    L0053e305:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp + 0x10]
            add esi, dword ptr [ebp + 0x18]
            dec dword ptr [ebp + 0x24]
            jne L0053e2b6
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov es, word ptr [ebp + 0xc]
            mov edi, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x18]
            mov ecx, dword ptr [ebp + 0x10]
            mov ebx, dword ptr [ebp + 0x14]
            cmp ebx, 4
            je L0053e393
            cmp ebx, 3
            je L0053e39b
            cmp ebx, 2
            je L0053e37b
            cmp ebx, 1
            je L0053e34b
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e34b:
            mov ah, al
            mov ebx, eax
            shl eax, 0x10
            mov al, bl
            mov ah, bl
    L0053e356:
            mov ebx, edi
            and ebx, 3
            je L0053e367
    L0053e35d:
            mov byte ptr es:[edi], al
            inc edi
            dec ecx
            je L0053e375
            dec ebx
            jne L0053e35d
    L0053e367:
            mov ebx, ecx
            shr ecx, 2
            rep stosd
            mov ecx, ebx
            and ecx, 3
            rep stosb
    L0053e375:
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e37b:
            mov ebx, eax
            shl eax, 0x10
            mov al, bl
            mov ah, bh
            shr ecx, 1
            rep stosd
            adc ecx, ecx
            rep stosw
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e393:
            rep stosd
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e39b:
            cmp al, ah
            jne L0053e3b2
            mov ebx, eax
            shr ebx, 8
            cmp bh, al
            jne L0053e3b2
            lea ecx, [ecx + ecx*2]
            rol eax, 8
            mov al, ah
            jmp L0053e356
    L0053e3b2:
            rol eax, 8
            mov edx, eax
            mov al, dh
            ror eax, 0x10
            mov ebx, edx
            rol ebx, 8
            mov bl, al
            mov dl, ah
            mov bh, ah
            rol eax, 8
            push ds
            mov ds, word ptr [ebp + 0xc]
            mov esi, ecx
            shr esi, 2
            je L0053e3e4
    L0053e3d6:
            mov dword ptr [edi], eax
            mov dword ptr [edi + 4], ebx
            mov dword ptr [edi + 8], edx
            add edi, 0xc
            dec esi
            jne L0053e3d6
    L0053e3e4:
            and ecx, 3
            je L0053e3f7
    L0053e3e9:
            mov byte ptr [edi], al
            mov byte ptr [edi + 1], ah
            mov byte ptr [edi + 2], bh
            add edi, 3
            dec ecx
            jne L0053e3e9
    L0053e3f7:
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov es, word ptr [ebp + 0xc]
            mov esi, dword ptr [ebp + 0x18]
            mov edi, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x20]
            mov ecx, dword ptr [ebp + 0x10]
            mov ebx, dword ptr [ebp + 0x1c]
            cmp ebx, 4
            je L0053e49a
            cmp ebx, 3
            je L0053e4b5
            cmp ebx, 2
            je L0053e473
            cmp ebx, 1
            je L0053e436
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e436:
            mov ah, al
            mov ebx, eax
            shl eax, 0x10
            mov al, bl
            mov ah, bl
    L0053e441:
            sub esi, ecx
            mov ebx, ecx
    L0053e445:
            mov ecx, ebx
            mov edx, edi
            and edx, 3
            je L0053e458
    L0053e44e:
            mov byte ptr es:[edi], al
            inc edi
            dec ecx
            je L0053e466
            dec edx
            jne L0053e44e
    L0053e458:
            mov edx, ecx
            shr ecx, 2
            rep stosd
            mov ecx, edx
            and ecx, 3
            rep stosb
    L0053e466:
            add edi, esi
            dec dword ptr [ebp + 0x14]
            jne L0053e445
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e473:
            mov ebx, eax
            shl eax, 0x10
            mov al, bl
            mov ah, bh
            sub esi, ecx
            sub esi, ecx
            mov ebx, ecx
    L0053e482:
            mov ecx, ebx
            shr ecx, 1
            rep stosd
            adc ecx, ecx
            rep stosw
            add edi, esi
            dec dword ptr [ebp + 0x14]
            jne L0053e482
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e49a:
            sub esi, ecx
            sub esi, ecx
            sub esi, ecx
            sub esi, ecx
            mov ebx, ecx
    L0053e4a4:
            mov ecx, ebx
            rep stosd
            add edi, esi
            dec dword ptr [ebp + 0x14]
            jne L0053e4a4
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e4b5:
            cmp al, ah
            jne L0053e4cf
            mov ebx, eax
            shr ebx, 8
            cmp bh, al
            jne L0053e4cf
            lea ecx, [ecx + ecx*2]
            rol eax, 8
            mov al, ah
            jmp L0053e441
    L0053e4cf:
            rol eax, 8
            mov edx, eax
            mov al, dh
            ror eax, 0x10
            mov ebx, edx
            rol ebx, 8
            mov bl, al
            mov dl, ah
            mov bh, ah
            rol eax, 8
            push ds
            mov ds, word ptr [ebp + 0xc]
    L0053e4ec:
            push ecx
            push edi
            mov esi, ecx
            shr esi, 2
            je L0053e503
    L0053e4f5:
            mov dword ptr [edi], eax
            mov dword ptr [edi + 4], ebx
            mov dword ptr [edi + 8], edx
            add edi, 0xc
            dec esi
            jne L0053e4f5
    L0053e503:
            and ecx, 3
            je L0053e516
    L0053e508:
            mov byte ptr [edi], al
            mov byte ptr [edi + 1], ah
            mov byte ptr [edi + 2], bh
            add edi, 3
            dec ecx
            jne L0053e508
    L0053e516:
            pop edi
            pop ecx
            add edi, dword ptr [ebp + 0x18]
            dec dword ptr [ebp + 0x14]
            jne L0053e4ec
            pop ds
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push ds
            push es
            mov ds, word ptr [ebp + 0x14]
            mov es, word ptr [ebp + 0xc]
            mov esi, dword ptr [ebp + 0x10]
            mov edi, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x28]
            mul dword ptr [ebp + 0x18]
            mov ecx, eax
    L0053e545:
            push ecx
            push esi
            push edi
            mov edx, edi
            and edx, 3
            je L0053e561
            neg edx
            add edx, 4
    L0053e554:
            mov al, byte ptr [esi]
            mov byte ptr es:[edi], al
            inc esi
            inc edi
            dec ecx
            je L0053e56f
            dec edx
            jne L0053e554
    L0053e561:
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L0053e56f:
            pop edi
            pop esi
            pop ecx
            add edi, dword ptr [ebp + 0x20]
            add esi, dword ptr [ebp + 0x24]
            dec dword ptr [ebp + 0x1c]
            jne L0053e545
            pop es
            pop ds
            pop edi
            pop esi
            pop ebx
            leave
            ret
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push ds
            push es
            mov ds, word ptr [ebp + 0x14]
            mov es, word ptr [ebp + 0xc]
            mov esi, dword ptr [ebp + 0x10]
            mov edi, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x1c]
            mul dword ptr [ebp + 0x18]
            mov ecx, eax
            mov edx, edi
            and edx, 3
            je L0053e5b4
    L0053e5a9:
            mov al, byte ptr [esi]
            mov byte ptr es:[edi], al
            inc esi
            inc edi
            dec ecx
            dec edx
            jne L0053e5a9
    L0053e5b4:
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep stosb
            pop es
            pop ds
            pop edi
            pop esi
            pop ebx
            leave
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053e686
void C2_NAKED C2_HOOK_CDECL pm_mem_fill_colour(br_uint_8 *dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (bpp) {
    case 1:
        memset(dest, colour, nbpixels);
        break;
    case 2: {
        br_uint_16 p = colour;
        int i;
        for (i = nbpixels; i != 0; i--, dest += 2) {
            *(br_uint_16*)dest = p;
        }
        break;
    }
    break;
    case 3: {
        int i;
        br_uint_8 b0, b1, b2;

        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);
        for (i = nbpixels; i != 0; i--, dest += 3) {
            dest[0] = b0;
            dest[1] = b1;
            dest[2] = b2;
        }
        break;
    }
    case 4: {
        int i;
        for (i = nbpixels; i != 0; i--, dest += 4) {
            *(br_uint_32*)dest = colour;
        }
        break;
    }
    default:
        abort();
        return;
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            add esp, -0xc
            push ebx
            push ecx
            push edx
            push edi
            push ds
            push es
            fnstcw word ptr [ebp - 0xa]
            fldcw word ptr [gMatchData_0066fd1e]
            mov es, word ptr [ebp + 0xc]
            mov ebx, dword ptr [ebp + 0x14]
            cmp ebx, 4
            je L0053e94c
            cmp ebx, 3
            je L0053e859
            cmp ebx, 2
            je L0053e792
            cmp ebx, 1
            je L0053e6cd
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053e6cd:
            mov eax, dword ptr [ebp + 0x18]
            mov edi, dword ptr [ebp + 8]
            mov ebx, eax
            mov ah, al
            shl eax, 0x10
            mov ecx, 8
            mov al, bl
            sub ecx, edi
            mov ah, bl
            and ecx, 7
            mov ebx, dword ptr [ebp + 0x10]
            je L0053e6f3
    L0053e6ed:
            sub ebx, ecx
            jle L0053e741
            rep stosb
    L0053e6f3:
            mov ecx, ebx
            and ebx, 7
            and ecx, 0xfffffff8
            je L0053e783
            mov edx, ecx
            and ecx, 0x38
            shr edx, 6
            add edi, ecx
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
            shr ecx, 1
            mov ds, word ptr [ebp + 0xc]
            jmp dword ptr cs:[ecx + 0x53e721]
            _emit 0x50
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x79
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x74
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x6f
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x6a
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x65
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x60
            _emit 0xe7
            _emit 0x53
            _emit 0x00
            _emit 0x5b
            _emit 0xe7
            _emit 0x53
            _emit 0x00
    L0053e741:
            add ecx, ebx
            rep stosb
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
            dec edx
            jl L0053e781
    L0053e753:
            lea edi, [edi + 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x38]
            fld st(0)
            fistp qword ptr [edi - 0x30]
            fld st(0)
            fistp qword ptr [edi - 0x28]
            fld st(0)
            fistp qword ptr [edi - 0x20]
            fld st(0)
            fistp qword ptr [edi - 0x18]
            fld st(0)
            fistp qword ptr [edi - 0x10]
            fld st(0)
            fistp qword ptr [edi - 8]
            dec edx
            jge L0053e753
    L0053e781:
            fstp st(0)
    L0053e783:
            mov ecx, ebx
            rep stosb
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053e792:
            mov eax, dword ptr [ebp + 0x18]
            mov edi, dword ptr [ebp + 8]
            mov ebx, eax
            mov ecx, 8
            shl eax, 0x10
            sub ecx, edi
            mov al, bl
            and ecx, 6
            shr ecx, 1
            mov ah, bh
            mov ebx, dword ptr [ebp + 0x10]
            je L0053e7b9
            sub ebx, ecx
            jle L0053e806
            rep stosw
    L0053e7b9:
            mov ecx, ebx
            and ebx, 3
            and ecx, 0xfffffffc
            je L0053e849
            mov edx, ecx
            and ecx, 0x1c
            shr edx, 5
            lea edi, [edi + ecx*2]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
            mov ds, word ptr [ebp + 0xc]
            jmp dword ptr cs:[ecx + 0x53e7e6]
            _emit 0x16
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x3f
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x3a
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x35
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x30
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x2b
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x26
            _emit 0xe8
            _emit 0x53
            _emit 0x00
            _emit 0x21
            _emit 0xe8
            _emit 0x53
            _emit 0x00
    L0053e806:
            add ecx, ebx
            rep stosw
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
            dec edx
            jl L0053e847
    L0053e819:
            lea edi, [edi + 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x38]
            fld st(0)
            fistp qword ptr [edi - 0x30]
            fld st(0)
            fistp qword ptr [edi - 0x28]
            fld st(0)
            fistp qword ptr [edi - 0x20]
            fld st(0)
            fistp qword ptr [edi - 0x18]
            fld st(0)
            fistp qword ptr [edi - 0x10]
            fld st(0)
            fistp qword ptr [edi - 8]
            dec edx
            jge L0053e819
    L0053e847:
            fstp st(0)
    L0053e849:
            mov ecx, ebx
            rep stosw
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053e859:
            mov eax, dword ptr [ebp + 0x18]
            mov edi, dword ptr [ebp + 8]
            mov ebx, eax
            mov ecx, 8
            ror ebx, 0x10
            sub ecx, edi
            cmp al, ah
            jne L0053e88c
            cmp bl, al
            jne L0053e88c
            rol eax, 8
            mov ebx, dword ptr [ebp + 0x10]
            mov al, ah
            and ecx, 7
            lea ebx, [ebx + ebx*2]
            je L0053e6f3
            jmp L0053e6ed
    L0053e88c:
            rol eax, 8
            mov bh, ah
            mov edx, eax
            mov al, bl
            rol edx, 8
            mov esi, dword ptr [ebp + 0x10]
            ror ebx, 0x10
            and ecx, 7
            mov dh, dl
            mov dl, bh
            mov ecx, dword ptr [ecx*4 + gMatchData_0066fd20]
            mov ds, word ptr [ebp + 0xc]
            sub esi, ecx
            jle L0053e92f
            dec ecx
            js L0053e8c5
    L0053e8b7:
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], dl
            mov byte ptr [edi + 2], al
            add edi, 3
            dec ecx
            jns L0053e8b7
    L0053e8c5:
            mov ecx, esi
            and esi, 0xf
            shr ecx, 4
            je L0053e913
            mov dword ptr [ebp - 8], edx
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], ebx
            fild qword ptr [ebp - 8]
            mov dword ptr [ebp - 8], ebx
            mov dword ptr [ebp - 4], edx
            fild qword ptr [ebp - 8]
    L0053e8ea:
            fld st(0)
            _emit 0xdf
        _emit 0x3f
            fld st(1)
            fistp qword ptr [edi + 8]
            fld st(2)
            fistp qword ptr [edi + 0x10]
            fld st(0)
            fistp qword ptr [edi + 0x18]
            fld st(1)
            fistp qword ptr [edi + 0x20]
            fld st(2)
            fistp qword ptr [edi + 0x28]
            lea edi, [edi + 0x30]
            dec ecx
            jne L0053e8ea
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L0053e913:
            dec esi
            js L0053e924
    L0053e916:
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], dl
            mov byte ptr [edi + 2], al
            add edi, 3
            dec esi
            jns L0053e916
    L0053e924:
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053e92f:
            add esi, ecx
            je L0053e924
    L0053e933:
            mov byte ptr [edi], bl
            mov byte ptr [edi + 1], dl
            mov byte ptr [edi + 2], al
            add edi, 3
            dec esi
            jne L0053e933
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053e94c:
            mov eax, dword ptr [ebp + 0x18]
            mov edi, dword ptr [ebp + 8]
            mov ebx, dword ptr [ebp + 0x10]
            test edi, 4
            je L0053e964
            mov dword ptr es:[edi], eax
            add edi, 4
            dec ebx
    L0053e964:
            mov ecx, ebx
            and ebx, 1
            and ecx, 0xfffffffe
            je L0053e9e1
            mov edx, ecx
            and ecx, 0xe
            shr edx, 4
            lea edi, [edi + ecx*4]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
            mov ds, word ptr [ebp + 0xc]
            jmp dword ptr cs:[ecx*2 + 0x53e98e]
            _emit 0xae
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xd7
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xd2
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xcd
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xc8
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xc3
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xbe
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            _emit 0xb9
            _emit 0xe9
            _emit 0x53
            _emit 0x00
            dec edx
            jl L0053e9df
    L0053e9b1:
            lea edi, [edi + 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x38]
            fld st(0)
            fistp qword ptr [edi - 0x30]
            fld st(0)
            fistp qword ptr [edi - 0x28]
            fld st(0)
            fistp qword ptr [edi - 0x20]
            fld st(0)
            fistp qword ptr [edi - 0x18]
            fld st(0)
            fistp qword ptr [edi - 0x10]
            fld st(0)
            fistp qword ptr [edi - 8]
            dec edx
            jge L0053e9b1
    L0053e9df:
            fstp st(0)
    L0053e9e1:
            test ebx, ebx
            je L0053e9e8
            mov dword ptr es:[edi], eax
    L0053e9e8:
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053e9f3
void C2_NAKED C2_HOOK_CDECL pm_mem_fill_colour_rect(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 x;
    br_uint_32 y;

    switch (bpp) {
    case 1:
        for (y = 0; y < height; y++, dest += stride) {
            memset(dest, colour, width);
        }
        break;
    case 2:
        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_16*)dest)[x] = colour;
            }
        }
        break;
    case 3: {
        br_uint_8 b0, b1, b2;

        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);

        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_8*)dest)[3 * x + 0] = b0;
                ((br_uint_8*)dest)[3 * x + 1] = b1;
                ((br_uint_8*)dest)[3 * x + 2] = b2;
            }
        }
        break;
    }
    case 4:
        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_32*)dest)[x] = colour;
            }
        }
        break;
    default:
        abort();
        return;
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            add esp, -0x1c
            push ebx
            push ecx
            push edx
            push esi
            push edi
            push ds
            push es
            fnstcw word ptr [ebp - 0xa]
            fldcw word ptr [gMatchData_0066fd1e]
            mov ds, word ptr [ebp + 0xc]
            mov es, word ptr [ebp + 0xc]
            mov ebx, dword ptr [ebp + 0x1c]
            cmp ebx, 4
            je L0053ed48
            cmp ebx, 3
            je L0053ec26
            cmp ebx, 2
            je L0053eb33
            cmp ebx, 1
            je L0053ea40
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053ea40:
            mov eax, dword ptr [ebp + 0x20]
            mov edi, dword ptr [ebp + 8]
            mov ebx, eax
            mov ah, al
            shl eax, 0x10
            mov ecx, 8
            mov al, bl
            sub ecx, edi
            mov ah, bl
            and ecx, 7
            mov dword ptr [ebp - 0x10], ecx
            mov ebx, dword ptr [ebp + 0x10]
    L0053ea61:
            sub ebx, ecx
            jle L0053eb12
            mov ecx, ebx
            and ebx, 7
            mov dword ptr [ebp - 0x1c], ebx
            and ecx, 0xfffffff8
            mov ebx, ecx
            and ecx, 0x38
            shr ebx, 6
            mov dword ptr [ebp - 0x14], ecx
            mov dword ptr [ebp - 0x18], ebx
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
    L0053ea91:
            push edi
            mov ecx, dword ptr [ebp - 0x10]
            rep stosb
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x18]
            add edi, ecx
            shr ecx, 1
            jmp dword ptr cs:[ecx + 0x53eaa8]
            _emit 0xc8
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xf1
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xec
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xe7
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xe2
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xdd
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xd8
            _emit 0xea
            _emit 0x53
            _emit 0x00
            _emit 0xd3
            _emit 0xea
            _emit 0x53
            _emit 0x00
            dec edx
            jl L0053eaf9
    L0053eacb:
            lea edi, [edi + 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x38]
            fld st(0)
            fistp qword ptr [edi - 0x30]
            fld st(0)
            fistp qword ptr [edi - 0x28]
            fld st(0)
            fistp qword ptr [edi - 0x20]
            fld st(0)
            fistp qword ptr [edi - 0x18]
            fld st(0)
            fistp qword ptr [edi - 0x10]
            fld st(0)
            fistp qword ptr [edi - 8]
            dec edx
            jge L0053eacb
    L0053eaf9:
            mov ecx, dword ptr [ebp - 0x1c]
            rep stosb
            pop edi
            add edi, esi
            dec ebx
            jne L0053ea91
            fstp st(0)
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053eb12:
            add ecx, ebx
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
            mov edx, ecx
    L0053eb1c:
            push edi
            rep stosb
            pop edi
            add edi, esi
            mov ecx, edx
            dec ebx
            jne L0053eb1c
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053eb33:
            mov eax, dword ptr [ebp + 0x20]
            mov edi, dword ptr [ebp + 8]
            mov ebx, eax
            mov ecx, 8
            shl eax, 0x10
            sub ecx, edi
            mov al, bl
            and ecx, 6
            mov ah, bh
            mov dword ptr [ebp - 0x10], ecx
            mov ebx, dword ptr [ebp + 0x10]
            sub ebx, ecx
            jle L0053ec04
            mov ecx, ebx
            and ebx, 3
            mov dword ptr [ebp - 0x1c], ebx
            and ecx, 0xfffffffc
            mov ebx, ecx
            and ecx, 0x1c
            shr ebx, 5
            mov dword ptr [ebp - 0x14], ecx
            mov dword ptr [ebp - 0x18], ebx
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
    L0053eb82:
            push edi
            mov ecx, dword ptr [ebp - 0x10]
            rep stosw
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x18]
            lea edi, [edi + ecx*2]
            jmp dword ptr cs:[ecx + 0x53eb99]
            _emit 0xb9
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xe2
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xdd
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xd8
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xd3
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xce
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xc9
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            _emit 0xc4
            _emit 0xeb
            _emit 0x53
            _emit 0x00
            dec edx
            jl L0053ebea
    L0053ebbc:
            lea edi, [edi + 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x38]
            fld st(0)
            fistp qword ptr [edi - 0x30]
            fld st(0)
            fistp qword ptr [edi - 0x28]
            fld st(0)
            fistp qword ptr [edi - 0x20]
            fld st(0)
            fistp qword ptr [edi - 0x18]
            fld st(0)
            fistp qword ptr [edi - 0x10]
            fld st(0)
            fistp qword ptr [edi - 8]
            dec edx
            jge L0053ebbc
    L0053ebea:
            mov ecx, dword ptr [ebp - 0x1c]
            rep stosw
            pop edi
            add edi, esi
            dec ebx
            jne L0053eb82
            fstp st(0)
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053ec04:
            add ecx, ebx
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
            mov edx, ecx
    L0053ec0e:
            push edi
            rep stosw
            pop edi
            add edi, esi
            mov ecx, edx
            dec ebx
            jne L0053ec0e
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053ec26:
            mov eax, dword ptr [ebp + 0x20]
            mov edi, dword ptr [ebp + 8]
            mov ebx, eax
            mov ecx, 8
            ror ebx, 0x10
            sub ecx, edi
            cmp al, ah
            jne L0053ec56
            cmp bl, al
            jne L0053ec56
            rol eax, 8
            mov ebx, dword ptr [ebp + 0x10]
            mov al, ah
            and ecx, 7
            lea ebx, [ebx + ebx*2]
            mov dword ptr [ebp - 0x10], ecx
            jmp L0053ea61
    L0053ec56:
            rol eax, 8
            mov bh, ah
            mov edx, eax
            mov al, bl
            rol edx, 8
            mov esi, dword ptr [ebp + 0x10]
            ror ebx, 0x10
            and ecx, 7
            mov dh, dl
            mov dl, bh
            mov ecx, dword ptr [ecx*4 + gMatchData_0066fd20]
            mov dword ptr [ebp - 0x10], ecx
            sub esi, ecx
            jle L0053ed1b
            mov ecx, esi
            and esi, 0xf
            shr ecx, 4
            mov dword ptr [ebp - 0x1c], esi
            mov dword ptr [ebp - 0x18], ecx
            mov dword ptr [ebp - 8], edx
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], ebx
            fild qword ptr [ebp - 8]
            mov dword ptr [ebp - 8], ebx
            mov dword ptr [ebp - 4], edx
            fild qword ptr [ebp - 8]
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
    L0053ecb0:
            push edi
            mov ecx, dword ptr [ebp - 0x10]
            dec ecx
            js L0053ecc5
    L0053ecb7:
            mov byte ptr [edi], ah
            mov byte ptr [edi + 1], dl
            mov byte ptr [edi + 2], al
            add edi, 3
            dec ecx
            jns L0053ecb7
    L0053ecc5:
            mov ecx, dword ptr [ebp - 0x18]
            test ecx, ecx
            je L0053ecef
    L0053eccc:
            fld st(0)
            _emit 0xdf
        _emit 0x3f
            fld st(1)
            fistp qword ptr [edi + 8]
            fld st(2)
            fistp qword ptr [edi + 0x10]
            fld st(0)
            fistp qword ptr [edi + 0x18]
            fld st(1)
            fistp qword ptr [edi + 0x20]
            fld st(2)
            fistp qword ptr [edi + 0x28]
            lea edi, [edi + 0x30]
            dec ecx
            jne L0053eccc
    L0053ecef:
            mov ecx, dword ptr [ebp - 0x1c]
            dec ecx
            js L0053ed03
    L0053ecf5:
            mov byte ptr [edi], ah
            mov byte ptr [edi + 1], dl
            mov byte ptr [edi + 2], al
            add edi, 3
            dec ecx
            jns L0053ecf5
    L0053ed03:
            pop edi
            add edi, esi
            dec ebx
            jne L0053ecb0
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L0053ed0f:
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053ed1b:
            add ecx, esi
            je L0053ed0f
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
    L0053ed25:
            push ecx
            push edi
    L0053ed27:
            mov byte ptr [edi], ah
            mov byte ptr [edi + 1], dl
            mov byte ptr [edi + 2], al
            add edi, 3
            dec ecx
            jne L0053ed27
            pop edi
            pop ecx
            add edi, esi
            dec ebx
            jne L0053ed25
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053ed48:
            mov eax, dword ptr [ebp + 0x20]
            mov edi, dword ptr [ebp + 8]
            mov ebx, edi
            mov ecx, dword ptr [ebp + 0x10]
            test ebx, 4
            je L0053ed5c
            dec ecx
    L0053ed5c:
            mov dword ptr [ebp - 0x1c], ecx
            and ecx, 0xfffffffe
            mov ebx, ecx
            and ecx, 0xe
            shr ebx, 4
            mov dword ptr [ebp - 0x14], ecx
            mov dword ptr [ebp - 0x18], ebx
            mov ebx, dword ptr [ebp + 0x14]
            mov esi, dword ptr [ebp + 0x18]
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], eax
            fild qword ptr [ebp - 8]
    L0053ed7f:
            push edi
            test edi, 4
            je L0053ed8e
            mov dword ptr es:[edi], eax
            add edi, 4
    L0053ed8e:
            mov ecx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x18]
            lea edi, [edi + ecx*4]
            jmp dword ptr cs:[ecx*2 + 0x53ed9f]
            _emit 0xbf
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xe8
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xe3
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xde
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xd9
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xd4
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xcf
            _emit 0xed
            _emit 0x53
            _emit 0x00
            _emit 0xca
            _emit 0xed
            _emit 0x53
            _emit 0x00
            dec edx
            jl L0053edf0
    L0053edc2:
            lea edi, [edi + 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x40]
            fld st(0)
            fistp qword ptr [edi - 0x38]
            fld st(0)
            fistp qword ptr [edi - 0x30]
            fld st(0)
            fistp qword ptr [edi - 0x28]
            fld st(0)
            fistp qword ptr [edi - 0x20]
            fld st(0)
            fistp qword ptr [edi - 0x18]
            fld st(0)
            fistp qword ptr [edi - 0x10]
            fld st(0)
            fistp qword ptr [edi - 8]
            dec edx
            jge L0053edc2
    L0053edf0:
            mov ecx, dword ptr [ebp - 0x1c]
            test ecx, 1
            je L0053edfe
            mov dword ptr es:[edi], eax
    L0053edfe:
            pop edi
            add edi, esi
            dec ebx
            jne L0053ed7f
            fstp st(0)
            fldcw word ptr [ebp - 0xa]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053ee16
void C2_NAKED C2_HOOK_CDECL pm_mem_copy_colour(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp) {
#ifndef CARPOCALYPSE2_MATCHING

    memcpy(dst, src, nbpixels * bpp);
#else
    __asm {
            push ebp
            mov ebp, esp
            add esp, -4
            push ebx
            push ecx
            push edx
            push esi
            push edi
            push ds
            push es
            fnstcw word ptr [ebp - 2]
            fldcw word ptr [g_endMasks+0x9]
            mov ds, word ptr [ebp + 0x14]
            mov es, word ptr [ebp + 0xc]
            mov esi, dword ptr [ebp + 0x10]
            mov edi, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x1c]
            mov ecx, 8
            mul dword ptr [ebp + 0x18]
            sub ecx, edi
            and ecx, 7
            je L0053ee56
            sub eax, ecx
            jle L0053eefd
            rep movsb
    L0053ee56:
            mov ebx, eax
            and eax, 7
            shr ebx, 3
            je L0053eeed
            mov ecx, ebx
            and ebx, 0x1f
            shr ecx, 5
            je L0053eedb
            add esi, 0x80
            add edi, 0x80
    L0053ee7a:
            mov edx, 4
    L0053ee7f:
            fild qword ptr [esi - 0x80]
            fild qword ptr [esi - 0x60]
            fild qword ptr [esi - 0x40]
            fild qword ptr [esi - 0x20]
            _emit 0xdf
        _emit 0x2e
            fild qword ptr [esi + 0x20]
            fild qword ptr [esi + 0x40]
            fild qword ptr [esi + 0x60]
            fxch st(1)
            fistp qword ptr es:[edi + 0x40]
            fistp qword ptr es:[edi + 0x60]
            fistp qword ptr es:[edi + 0x20]
            _emit 0x26
        _emit 0xdf
        _emit 0x3f
            fistp qword ptr es:[edi - 0x20]
            fistp qword ptr es:[edi - 0x40]
            fistp qword ptr es:[edi - 0x60]
            fistp qword ptr es:[edi - 0x80]
            lea esi, [esi + 8]
            lea edi, [edi + 8]
            dec edx
            jne L0053ee7f
            add esi, 0xe0
            add edi, 0xe0
            dec ecx
            jne L0053ee7a
            sub esi, 0x80
            sub edi, 0x80
    L0053eedb:
            test ebx, ebx
            je L0053eeed
    L0053eedf:
            _emit 0xdf
        _emit 0x2e
            _emit 0x26
        _emit 0xdf
        _emit 0x3f
            add esi, 8
            add edi, 8
            dec ebx
            jne L0053eedf
    L0053eeed:
            mov ecx, eax
            rep movsb
            fldcw word ptr [ebp - 2]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    L0053eefd:
            add ecx, eax
            rep movsb
            fldcw word ptr [ebp - 2]
            pop es
            pop ds
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            leave
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053ef0d
void C2_NAKED C2_HOOK_CDECL pm_mem_copy_colour_rowbyrow(br_uint_8* dest, br_uint_32 dest_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dest_stride, br_uint_32 src_stride, br_uint_32 bpp) {
#ifndef CARPOCALYPSE2_MATCHING

    for (; height != 0; height--, dest += dest_stride, src += src_stride) {
        memcpy(dest, src, width * bpp);
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            add esp, -0x14
            push ebx
            push esi
            push edi
            push ds
            push es
            fnstcw word ptr [ebp - 2]
            fldcw word ptr [g_endMasks+0x9]
            mov ds, word ptr [ebp + 0x14]
            mov es, word ptr [ebp + 0xc]
            mov esi, dword ptr [ebp + 0x10]
            mov edi, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x28]
            mov ecx, 8
            mul dword ptr [ebp + 0x18]
            sub ecx, edi
            and ecx, 7
            sub eax, ecx
            jle L0053f014
            mov ebx, eax
            and eax, 7
            shr ebx, 3
            mov dword ptr [ebp - 8], ecx
            mov ecx, ebx
            and ebx, 0x1f
            shr ecx, 5
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 0x10], ebx
            mov dword ptr [ebp - 0xc], ecx
            mov eax, dword ptr [ebp + 0x1c]
    L0053ef66:
            push esi
            push edi
            mov ecx, dword ptr [ebp - 8]
            rep movsb
            mov ecx, dword ptr [ebp - 0xc]
            mov ebx, dword ptr [ebp - 0x10]
            test ecx, ecx
            je L0053efe4
            add esi, 0x80
            add edi, 0x80
    L0053ef83:
            mov edx, 4
    L0053ef88:
            fild qword ptr [esi - 0x80]
            fild qword ptr [esi - 0x60]
            fild qword ptr [esi - 0x40]
            fild qword ptr [esi - 0x20]
            _emit 0xdf
        _emit 0x2e
            fild qword ptr [esi + 0x20]
            fild qword ptr [esi + 0x40]
            fild qword ptr [esi + 0x60]
            fxch st(1)
            fistp qword ptr es:[edi + 0x40]
            fistp qword ptr es:[edi + 0x60]
            fistp qword ptr es:[edi + 0x20]
            _emit 0x26
        _emit 0xdf
        _emit 0x3f
            fistp qword ptr es:[edi - 0x20]
            fistp qword ptr es:[edi - 0x40]
            fistp qword ptr es:[edi - 0x60]
            fistp qword ptr es:[edi - 0x80]
            lea esi, [esi + 8]
            lea edi, [edi + 8]
            dec edx
            jne L0053ef88
            add esi, 0xe0
            add edi, 0xe0
            dec ecx
            jne L0053ef83
            sub esi, 0x80
            sub edi, 0x80
    L0053efe4:
            test ebx, ebx
            je L0053eff6
    L0053efe8:
            _emit 0xdf
        _emit 0x2e
            _emit 0x26
        _emit 0xdf
        _emit 0x3f
            add esi, 8
            add edi, 8
            dec ebx
            jne L0053efe8
    L0053eff6:
            mov ecx, dword ptr [ebp - 0x14]
            rep movsb
            pop edi
            pop esi
            add esi, dword ptr [ebp + 0x24]
            add edi, dword ptr [ebp + 0x20]
            dec eax
            jne L0053ef66
    L0053f00a:
            fldcw word ptr [ebp - 2]
            pop es
            pop ds
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053f014:
            add ecx, eax
            mov eax, dword ptr [ebp + 0x1c]
            mov ebx, ecx
    L0053f01b:
            push esi
            push edi
            rep movsb
            pop edi
            pop esi
            add esi, dword ptr [ebp + 0x24]
            add edi, dword ptr [ebp + 0x20]
            mov ecx, ebx
            dec eax
            jne L0053f01b
            jmp L0053f00a
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053e5c9
void C2_NAKED C2_HOOK_CDECL pm_mem_set_colour(br_uint_8* dest, br_uint_32 dest_qual, br_uint_32 bpp, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING


    switch (bpp) {
    case 1:
        *(br_uint_8*)dest = colour;
        break;
    case 2:
        *(br_uint_16*)dest = colour;
        break;
    case 3:
        ((br_uint_8*)dest)[0] = (br_uint_8)((colour >>  0) & 0xff);
        ((br_uint_8*)dest)[1] = (br_uint_8)((colour >>  8) & 0xff);
        ((br_uint_8*)dest)[2] = (br_uint_8)((colour >> 16) & 0xff);
        break;
    case 4:
        *(br_uint_32*)dest = colour;
        break;
    default:
        abort();
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov eax, dword ptr [ebp + 0x14]
            mov ebx, dword ptr [ebp + 0x10]
            mov edi, dword ptr [ebp + 8]
            mov es, word ptr [ebp + 0xc]
            cmp ebx, 1
            je L0053e5f6
            cmp ebx, 2
            je L0053e5ff
            cmp ebx, 3
            je L0053e612
            cmp ebx, 4
            je L0053e609
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e5f6:
            mov byte ptr es:[edi], al
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e5ff:
            mov word ptr es:[edi], ax
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e609:
            mov dword ptr es:[edi], eax
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e612:
            mov byte ptr es:[edi], al
            mov byte ptr es:[edi + 1], ah
            shr eax, 8
            mov byte ptr es:[edi + 2], ah
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053e626
br_uint_32 C2_NAKED C2_HOOK_CDECL pm_mem_read_colour(void* src, br_uint_32 src_qual, br_uint_32 bpp) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (bpp) {
    case 1:
        return (br_uint_32)*(br_uint_8*)src;
    case 2:
        return (br_uint_32)*(br_uint_16*)src;
    case 3: {
        return BR_COLOUR_RGB(((br_uint_8*)src)[2], ((br_uint_8*)src)[1], ((br_uint_8*)src)[0]);
    }
    case 4:
        return (br_uint_32)*(br_uint_32*)src;
    default:
        abort();
        return 0;
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov ebx, dword ptr [ebp + 0x10]
            mov edi, dword ptr [ebp + 8]
            mov es, word ptr [ebp + 0xc]
            xor eax, eax
            cmp ebx, 1
            je L0053e652
            cmp ebx, 2
            je L0053e65b
            cmp ebx, 3
            je L0053e66e
            cmp ebx, 4
            je L0053e665
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e652:
            mov al, byte ptr es:[edi]
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e65b:
            mov ax, word ptr es:[edi]
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e665:
            mov eax, dword ptr es:[edi]
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    L0053e66e:
            mov ah, byte ptr es:[edi + 2]
            shl eax, 8
            mov ah, byte ptr es:[edi + 1]
            mov al, byte ptr es:[edi]
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    }
#endif
}
