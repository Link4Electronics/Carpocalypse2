#include "genclip.h"

// FUNCTION: CARMA2_HW 0x0053d9b0
br_clip_result C2_HOOK_CDECL PixelmapPointClip(br_point* out, br_point* in, br_pixelmap* pm) {

    out->x = in->x + pm->origin_x;
    out->y = in->y + pm->origin_y;

    if (out->x >= 0 && out->y >= 0 && out->x < pm->width && out->y < pm->height) {
        return BR_CLIP_ACCEPT;
    }
    return BR_CLIP_REJECT;
}

// FUNCTION: CARMA2_HW 0x0053da00
br_clip_result C2_NAKED C2_HOOK_CDECL PixelmapLineClip(br_point* s_out, br_point* e_out, br_point* s_in, br_point* e_in, br_pixelmap* pm) {
#ifndef CARPOCALYPSE2_MATCHING
    int temp;
    br_int_32 w;
    br_int_32 h;
    br_int_32 x1;
    br_int_32 x2;
    br_int_32 y1;
    br_int_32 y2;

#define USCALE(arg,num,den) (((br_uint_32)(arg) * (br_uint_32)(num)) / (br_uint_32)(den))

    x1 = s_in->x + pm->origin_x;
    y1 = s_in->y + pm->origin_y;

    x2 = e_in->x + pm->origin_x;
    y2 = e_in->y + pm->origin_y;

    w = pm->width - 1;
    h = pm->height - 1;

    if (x1 > x2) {
        temp = x1;
        x1 = x2;
        x2 = temp;

        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (x2 < 0 || x1 > w) {
        return BR_CLIP_REJECT;
    }
    if (y1 < y2) {
        if (y2 < 0 || y1 > h) {
            return BR_CLIP_REJECT;
        }
        if (y1 < 0) {
            temp = USCALE(x2 - x1, 0 - y1, y2 - y1);
            x1 += temp;
            if (x1 > w) {
                return BR_CLIP_REJECT;
            }
            y1 = 0;
        }
        if (y2 > h) {
            temp = USCALE(x2 - x1, y2 - h, y2 - y1);
            x2 -= temp;
            if (x2 < 0) {
                return BR_CLIP_REJECT;
            }
            y2 = h;
        }
        if (x1 < 0) {
            temp = USCALE(y2 - y1, 0 - x1, x2 - x1);
            y1 += temp;
            x1 = 0;
        }
        if (x2 > w) {
            temp = USCALE(y2 - y1, x2 - w, x2 - x1);
            y2 -= temp;
            x2 = w;
        }
    } else {
        if (y1 < 0 || y2 > h) {
            return BR_CLIP_REJECT;
        }
        if (y1 > h) {
            temp = USCALE(x2 - x1, y1 - h, y1 - y2);
            x1 += temp;
            if (x1 > w) {
                return BR_CLIP_REJECT;
            }
            y1 = h;
        }
        if (y2 < 0) {
            temp = USCALE(x2 - x1, 0 - y2, y1 - y2);
            x2 -= temp;
            if (x2 < 0) {
                return BR_CLIP_REJECT;
            }
            y2 = 0;
        }
        if (x1 < 0) {
            temp = USCALE(y1 - y2, 0 - x1, x2 - x1);
            y1 -= temp;
            x1 = 0;
        }
        if (x2 > w) {
            temp = USCALE(y1 - y2, x2 - w, x2 - x1);
            y2 += temp;
            x2 = w;
        }
    }

    s_out->x = x1;
    s_out->y = y1;

    e_out->x = x2;
    e_out->y = y2;

    return BR_CLIP_PARTIAL;
#else
    __asm {
            mov eax, dword ptr [esp + 0x14]
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x18]
            push edi
            mov edi, dword ptr [esp + 0x20]
            movsx edx, word ptr [eax + 0x38]
            movsx ebp, word ptr [eax + 0x3a]
            mov ecx, dword ptr [esi]
            mov ebx, dword ptr [edi]
            mov esi, dword ptr [esi + 4]
            mov edi, dword ptr [edi + 4]
            add ecx, edx
            add ebx, edx
            add esi, ebp
            add edi, ebp
            xor edx, edx
            xor ebp, ebp
            mov dx, word ptr [eax + 0x34]
            mov bp, word ptr [eax + 0x36]
            dec edx
            dec ebp
            cmp ecx, ebx
            mov dword ptr [esp + 0x24], edx
            jle L0053da4c
            mov eax, ecx
            mov ecx, ebx
            mov ebx, eax
            mov eax, esi
            mov esi, edi
            mov edi, eax
    L0053da4c:
            test ebx, ebx
            jl L0053dba9
            cmp ecx, edx
            jg L0053dba9
            cmp esi, edi
            jge L0053db03
            test edi, edi
            jl L0053dba9
            cmp esi, ebp
            jg L0053dba9
            test esi, esi
            jge L0053daa3
            mov eax, ebx
            mov edx, edi
            sub eax, ecx
            sub edx, esi
            imul eax, esi
            mov dword ptr [esp + 0x1c], edx
            xor edx, edx
            mov esi, dword ptr [esp + 0x1c]
            neg eax
            div esi
            add ecx, eax
            mov eax, dword ptr [esp + 0x24]
            cmp ecx, eax
            jg L0053dba9
            xor esi, esi
            mov edx, eax
    L0053daa3:
            cmp edi, ebp
            jle L0053dac6
            mov eax, edi
            mov edx, ebx
            sub eax, ebp
            sub edx, ecx
            imul eax, edx
            sub edi, esi
            xor edx, edx
            div edi
            sub ebx, eax
            js L0053dba9
            mov edx, dword ptr [esp + 0x24]
            mov edi, ebp
    L0053dac6:
            test ecx, ecx
            jge L0053dae3
            mov eax, edi
            mov ebp, ebx
            sub eax, esi
            sub ebp, ecx
            imul eax, ecx
            neg eax
            xor edx, edx
            div ebp
            mov edx, dword ptr [esp + 0x24]
            add esi, eax
            xor ecx, ecx
    L0053dae3:
            cmp ebx, edx
            jle L0053db8d
            mov eax, ebx
            sub ebx, ecx
            sub eax, edx
            mov edx, edi
            sub edx, esi
            imul eax, edx
            xor edx, edx
            div ebx
            sub edi, eax
            jmp L0053db89
    L0053db03:
            test esi, esi
            jl L0053dba9
            cmp edi, ebp
            jg L0053dba9
            cmp esi, ebp
            jle L0053db36
            mov eax, esi
            mov edx, ebx
            sub eax, ebp
            sub edx, ecx
            imul eax, edx
            sub esi, edi
            xor edx, edx
            div esi
            add ecx, eax
            mov eax, dword ptr [esp + 0x24]
            cmp ecx, eax
            jg L0053dba9
            mov esi, ebp
            mov edx, eax
    L0053db36:
            test edi, edi
            jge L0053db55
            mov eax, ebx
            mov ebp, esi
            sub eax, ecx
            sub ebp, edi
            imul eax, edi
            neg eax
            xor edx, edx
            div ebp
            sub ebx, eax
            js L0053dba9
            mov edx, dword ptr [esp + 0x24]
            xor edi, edi
    L0053db55:
            test ecx, ecx
            jge L0053db72
            mov eax, esi
            mov ebp, ebx
            sub eax, edi
            sub ebp, ecx
            imul eax, ecx
            neg eax
            xor edx, edx
            div ebp
            mov edx, dword ptr [esp + 0x24]
            sub esi, eax
            xor ecx, ecx
    L0053db72:
            cmp ebx, edx
            jle L0053db8d
            mov eax, ebx
            sub ebx, ecx
            sub eax, edx
            mov edx, esi
            sub edx, edi
            imul eax, edx
            xor edx, edx
            div ebx
            add edi, eax
    L0053db89:
            mov ebx, dword ptr [esp + 0x24]
    L0053db8d:
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [eax], ecx
            mov dword ptr [eax + 4], esi
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [eax], ebx
            mov dword ptr [eax + 4], edi
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0053dba9:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053dbb0
br_clip_result C2_HOOK_CDECL PixelmapRectangleClip(br_rectangle* out, br_rectangle* in, br_pixelmap* pm) {

    out->x = pm->origin_x + in->x;
    out->y = pm->origin_y + in->y;
    out->w = in->w;
    out->h = in->h;

    if (out->x >= pm->width || out->y >= pm->height) {
        return BR_CLIP_REJECT;
    }
    if (out->x + out->w <= 0 || out->y + out->h <= 0) {
        return BR_CLIP_REJECT;
    }

    if (out->x < 0) {
        out->w += out->x;
        out->x = 0;
    }
    if (out->y < 0) {
        out->h += out->y;
        out->y = 0;
    }
    if (out->x + out->w > pm->width) {
        out->w = pm->width - out->x;
    }
    if (out->y + out->h > pm->height) {
        out->h = pm->height - out->y;
    }
    if (out->w == 0 || out->h == 0) {
        return BR_CLIP_REJECT;
    }
    return BR_CLIP_PARTIAL;
}

// FUNCTION: CARMA2_HW 0x0053dc80
br_clip_result C2_HOOK_CDECL PixelmapRectangleClipTwo(br_rectangle* r_out, br_point* p_out, br_rectangle* r_in, br_point* p_in, br_pixelmap* pm_dst, br_pixelmap* pm_src) {

    r_out->x = pm_src->origin_x + r_in->x;
    r_out->y = pm_src->origin_y + r_in->y;
    r_out->w = r_in->w;
    r_out->h = r_in->h;
    p_out->x = pm_dst->origin_x + p_in->x;
    p_out->y = pm_dst->origin_y + p_in->y;

    if (p_out->x >= pm_dst->width) {
        return BR_CLIP_REJECT;
    }
    if (p_out->y >= pm_dst->height) {
        return BR_CLIP_REJECT;
    }
    if (r_out->x >= pm_src->width) {
        return BR_CLIP_REJECT;
    }
    if (r_out->y >= pm_src->height) {
        return BR_CLIP_REJECT;
    }
    if (p_out->x + r_out->w <= 0) {
        return BR_CLIP_REJECT;
    }
    if (p_out->y + r_out->h <= 0) {
        return BR_CLIP_REJECT;
    }
    if (r_out->x + r_out->w <= 0) {
        return BR_CLIP_REJECT;
    }
    if (r_out->y + r_out->h <= 0) {
        return BR_CLIP_REJECT;
    }
    if (p_out->x < 0) {
        r_out->w += p_out->x;
        r_out->x -= p_out->x;
        p_out->x = 0;
    }
    if (p_out->y < 0) {
        r_out->h += p_out->y;
        r_out->y -= p_out->y;
        p_out->y = 0;
    }
    if (p_out->x + r_out->w > pm_dst->width) {
        r_out->w = pm_dst->width - p_out->x;
    }
    if (p_out->y + r_out->h > pm_dst->height) {
        r_out->h = pm_dst->height - p_out->y;
    }
    if (r_out->x < 0) {
        r_out->w += r_out->x;
        p_out->x -= r_out->x;
        r_out->x = 0;
    }
    if (r_out->y < 0) {
        r_out->h += r_out->y;
        p_out->y -= r_out->y;
        r_out->y = 0;
    }
    if (r_out->x + r_out->w > pm_src->width) {
        r_out->w = pm_src->width - r_out->x;
    }
    if (r_out->y + r_out->h > pm_src->height) {
        r_out->h = pm_src->height - r_out->y;
    }
    if (r_out->w == 0 || r_out->h == 0) {
        return BR_CLIP_REJECT;
    }
    return BR_CLIP_PARTIAL;
}

// FUNCTION: CARMA2_HW 0x0053de30
br_clip_result C2_HOOK_CDECL PixelmapCopyBitsClip(br_rectangle* r_out, br_point* p_out, br_rectangle* r_in, br_point* p_in, br_pixelmap* pm) {

    r_out->x = r_in->x;
    r_out->y = r_in->y;
    r_out->w = r_in->w;
    r_out->h = r_in->h;
    p_out->x = pm->origin_x + p_in->x;
    p_out->y = pm->origin_y + p_in->y;

    if (p_out->x >= pm->width) {
        return BR_CLIP_REJECT;
    }
    if (p_out->y >= pm->height) {
        return BR_CLIP_REJECT;
    }
    if (p_out->x + r_out->w <= 0) {
        return BR_CLIP_REJECT;
    }
    if (p_out->y + r_out->h <= 0) {
        return BR_CLIP_REJECT;
    }
    if (p_out->x < 0) {
        r_out->w += p_out->x;
        r_out->x -= p_out->x;
        p_out->x = 0;
    }
    if (p_out->y < 0) {
        r_out->h += p_out->y;
        r_out->y -= p_out->y;
        p_out->y = 0;
    }
    if (p_out->x + r_out->w > pm->width) {
        r_out->w = pm->width - p_out->x;
    }
    if (p_out->y + r_out->h > pm->height) {
        r_out->h = pm->height - p_out->y;
    }
    if (r_out->w == 0 || r_out->h == 0) {
        return BR_CLIP_REJECT;
    }
    return BR_CLIP_PARTIAL;
}
