#include "pmgen.h"

#include "genclip.h"
#include "pmdsptch.h"

#include "core/std/brstdlib.h"

#ifdef CARPOCALYPSE2_MATCHING
extern br_size_t C2_HOOK_CDECL BrStrLen();
extern br_clip_result C2_HOOK_CDECL PixelmapLineClip();
#endif
// FUNCTION: CARMA2_HW 0x0053c230
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_match(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c240
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copy(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle r;
    br_point p;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopy, 0x84);

    p.x = -self->pm_origin_x;
    p.y = -self->pm_origin_y;
    r.x = -src->pm_origin_x;
    r.y = -src->pm_origin_y;
    r.w = src->pm_width;
    r.h = src->pm_height;
    return self->dispatch->_rectangleCopy(self, &p, src, &r);
}

// FUNCTION: CARMA2_HW 0x0053c2b0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle r;
    br_point p;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopyTo, 0x88);

    p.x = -self->pm_origin_x;
    p.y = -self->pm_origin_y;
    r.x = -src->pm_origin_x;
    r.y = -src->pm_origin_y;
    r.w = src->pm_width;
    r.h = src->pm_height;
    return self->dispatch->_rectangleCopyTo(self, &p, src, &r);
}

// FUNCTION: CARMA2_HW 0x0053c320
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle r;
    br_point p;

    p.x = -self->pm_origin_x;
    p.y = -self->pm_origin_y;
    r.x = -src->pm_origin_x;
    r.y = -src->pm_origin_y;
    r.w = src->pm_width;
    r.h = src->pm_height;
    return self->dispatch->_rectangleCopyFrom(self, &p, src, &r);
}

// FUNCTION: CARMA2_HW 0x0053c390
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fill(br_device_pixelmap* self, br_uint_32 colour) {
    br_rectangle r;

    r.x = -self->pm_origin_x;
    r.y = -self->pm_origin_y;
    r.w = self->pm_width;
    r.h = self->pm_height;
    return self->dispatch->_rectangleFill(self, &r, colour);
}

// FUNCTION: CARMA2_HW 0x0053c3e0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle s;
    br_rectangle d;

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        return DispatchCopy(self,src);
    } else {
        s.x = -src->pm_origin_x;
        s.y = -src->pm_origin_y;
        s.w = src->pm_width;
        s.h = src->pm_height;
        d.x = -self->pm_origin_x;
        d.y = -self->pm_origin_y;
        d.w = self->pm_width;
        d.h = self->pm_height;
        return DispatchRectangleStretchCopy(self, &d, src, &s);
    }
}

// FUNCTION: CARMA2_HW 0x0053c480
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
    int i;
    br_point p;
    br_error e;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopy, 0x84);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _copy, 0x54);

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty[i].x;
            p.y = dirty[i].y;
            e = self->dispatch->_rectangleCopy(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_copy(self, src);
    }
}

// FUNCTION: CARMA2_HW 0x0053c500
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
    int i;
    br_point p;
    br_error e;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopyTo, 0x88);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _copyTo, 0x58);

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty[i].x;
            p.y = dirty[i].y;
            e = self->dispatch->_rectangleCopyTo(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_copyTo(self, src);
    }
}

// FUNCTION: CARMA2_HW 0x0053c580
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
    int i;
    br_point p;
    br_error e;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopyFrom, 0x8c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _copyFrom, 0x5c);

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty[i].x;
            p.y = dirty[i].y;
            e = self->dispatch->_rectangleCopyFrom(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_copyFrom(self, src);
    }
}

// FUNCTION: CARMA2_HW 0x0053c600
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {
    int i;
    br_error e;

    for (i = 0; i < num_rects; i++) {
        e = self->dispatch->_rectangleFill(self, &dirty[i], colour);
        if (e != 0) {
            return e;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c640
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
    int i;
    br_point p;
    br_error e;

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty[i].x;
            p.y = dirty[i].y;
            e = self->dispatch->_rectangleCopy(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_doubleBuffer(self, src);
    }
}

// FUNCTION: CARMA2_HW 0x0053c6c0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
    br_point tl;
    br_point tr;
    br_point bl;
    br_point br;
    br_error e;

    tl.x = bl.x = rect->x;
    tr.y = tl.y = rect->y;
    tr.x = br.x = rect->x + rect->w - 1;
    bl.y = br.y = rect->y + rect->h - 1;

    e = self->dispatch->_line(self, &tl, &tr, colour);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tl, &bl, colour);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &bl, &br, colour);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tr, &br, colour);
    return e;
}

// FUNCTION: CARMA2_HW 0x0053c780
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle2(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour_tl, br_uint_32 colour_br) {
    br_point tl;
    br_point tr;
    br_point bl;
    br_point br;
    br_error e;

    tl.x = bl.x = rect->x;
    tl.y = tr.y = rect->y;
    tr.x = br.x = rect->x + rect->w - 1;
    bl.y = br.y = rect->y + rect->h - 1;

    e = self->dispatch->_line(self, &tl, &tr, colour_tl);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tl, &bl, colour_tl);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &bl, &br, colour_br);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tr, &br, colour_br);
    return e;
}

// FUNCTION: CARMA2_HW 0x0053c840
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_gen_line(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING
#if 0
    int dx;
    int dy;
    int incr1;
    int incr2;
    int D;
    int x;
    int y;
    int xend;
    int c;
    int pixels_left;
    int x1;
    int y1;
    int sign_x;
    int sign_y;
    int step;
    int reverse;
    int i;
    br_point as;
    br_point ae;
    br_point p;
#else
    int err;
    int e2;
    int sx;
    int sy;
    int dx;
    int dy;
    br_point as;
    br_point ae;
    br_point p;
#endif

    if (PixelmapLineClip(&as, &ae, s, e, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0;
    }
    as.x -= self->pm_origin_x;
    as.y -= self->pm_origin_y;
    ae.x -= self->pm_origin_x;
    ae.y -= self->pm_origin_y;

    sx = as.x < ae.x ? 1 : -1;
    dx = (ae.x - as.x) * sx;
    sy = as.y < ae.y ? 1 : -1;
    dy = (ae.y - as.y) * sy;
    err = dx + dy;

    p.x = as.x;
    p.y = as.y;

    for (;;) {
        self->dispatch->_pixelSet(self, &p, colour);
        if (p.x == ae.x && p.y == ae.y) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            if (p.x == ae.x) {
                break;
            }
            err += dy;
            p.x += sx;
        }
        if (e2 <=  dx) {
            if (p.y == ae.y) {
                break;
            }
            err += dx;
            p.y += sy;
        }
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            sub esp, 0x38
            mov ecx, dword ptr [esp + 0x40]
            lea edx, [esp + 0x28]
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x48]
            push edi
            push esi
            push eax
            push ecx
            lea eax, [esp + 0x4c]
            push edx
            push eax
            call PixelmapLineClip
            add esp, 0x14
            test eax, eax
            je L0053d643
            movsx eax, word ptr [esi + 0x38]
            mov edx, dword ptr [esp + 0x40]
            mov edi, dword ptr [esp + 0x38]
            movsx ecx, word ptr [esi + 0x3a]
            mov ebp, dword ptr [esp + 0x44]
            sub edx, eax
            sub edi, eax
            mov eax, dword ptr [esp + 0x3c]
            sub ebp, ecx
            sub eax, ecx
            mov ecx, edi
            mov dword ptr [esp + 0x3c], eax
            sub ecx, edx
            mov eax, 0
            setns al
            dec eax
            mov dword ptr [esp + 0x38], edi
            mov edi, dword ptr [esp + 0x3c]
            and al, 0xfe
            inc eax
            mov dword ptr [esp + 0x44], ebp
            mov ebx, eax
            mov dword ptr [esp + 0x40], edx
            imul ebx, ecx
            sub edi, ebp
            mov ecx, 0
            setns cl
            dec ecx
            and ecx, 0xfffffffe
            inc ecx
            mov ebp, ecx
            imul ebp, edi
            test ebx, ebx
            jne L0053c8f5
            test ebp, ebp
            jne L0053c8f5
            mov edx, dword ptr [esp + 0x58]
            mov ecx, dword ptr [esi]
            lea eax, [esp + 0x40]
            push edx
            push eax
            push esi
            call dword ptr [ecx + 0xa0]
            add esp, 0xc
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    L0053c8f5:
            sub eax, ecx
            neg eax
            sbb eax, eax
            and al, 0xfe
            inc eax
            cmp ebp, ebx
            mov dword ptr [esp + 0x10], eax
            jle L0053c93e
            mov ecx, dword ptr [esp + 0x44]
            mov eax, dword ptr [esp + 0x38]
            mov edi, dword ptr [esp + 0x3c]
            xor edx, ecx
            xor eax, edi
            xor ecx, edx
            xor edi, eax
            xor ebx, ebp
            xor edx, ecx
            xor eax, edi
            xor ebp, ebx
            mov dword ptr [esp + 0x44], ecx
            mov dword ptr [esp + 0x40], edx
            mov dword ptr [esp + 0x3c], edi
            mov dword ptr [esp + 0x38], eax
            xor ebx, ebp
            mov dword ptr [esp + 0x14], 1
            jmp L0053c94e
    L0053c93e:
            mov eax, dword ptr [esp + 0x38]
            mov ecx, dword ptr [esp + 0x44]
            mov dword ptr [esp + 0x14], 0
    L0053c94e:
            cmp edx, eax
            jle L0053c962
            mov edi, eax
            mov eax, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x54], edx
            jmp L0053c970
    L0053c962:
            mov dword ptr [esp + 0x4c], ecx
            mov ecx, dword ptr [esp + 0x3c]
            mov edi, edx
            mov dword ptr [esp + 0x54], eax
    L0053c970:
            mov dword ptr [esp + 0x50], ecx
            lea ecx, [ebx - 1]
            mov eax, ecx
            cdq
            and edx, 3
            add eax, edx
            sar eax, 2
            mov dword ptr [esp + 0x28], eax
            mov eax, ecx
            cdq
            xor eax, edx
            sub eax, edx
            and eax, 3
            xor eax, edx
            sub eax, edx
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053c9ae
            mov edx, dword ptr [esp + 0x4c]
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x30], edx
            jmp L0053c9ba
    L0053c9ae:
            mov eax, dword ptr [esp + 0x4c]
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053c9ba:
            mov edx, dword ptr [esp + 0x58]
            mov ecx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push edx
            push eax
            push esi
            call dword ptr [ecx + 0xa0]
            mov eax, dword ptr [esp + 0x20]
            add esp, 0xc
            test eax, eax
            je L0053c9ea
            mov ecx, dword ptr [esp + 0x50]
            mov edx, dword ptr [esp + 0x54]
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edx
            jmp L0053c9fa
    L0053c9ea:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], ecx
    L0053c9fa:
            mov eax, dword ptr [esp + 0x58]
            mov edx, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push eax
            push ecx
            push esi
            call dword ptr [edx + 0xa0]
            lea eax, [ebp + ebp]
            add esp, 0xc
            mov dword ptr [esp + 0x20], eax
            sub eax, ebx
            shl eax, 1
            mov dword ptr [esp + 0x2c], eax
            jns L0053cfbf
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x28]
            lea ecx, [edx + edx]
            mov dword ptr [esp + 0x24], ecx
            sub ecx, ebx
            test eax, eax
            mov dword ptr [esp + 0x18], ecx
            jle L0053d64d
            mov ebx, dword ptr [esp + 0x58]
            mov ebp, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x28], eax
            jmp L0053ca55
    L0053ca51:
            mov ecx, dword ptr [esp + 0x18]
    L0053ca55:
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x4c]
            inc edi
            dec edx
            test ecx, ecx
            mov dword ptr [esp + 0x54], edx
            jge L0053cb4e
            test ebp, ebp
            je L0053ca79
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053ca81
    L0053ca79:
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053ca81:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053caa6
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], edi
            jmp L0053cab3
    L0053caa6:
            mov eax, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053cab3:
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebx
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cadb
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x54]
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], ecx
            jmp L0053caeb
    L0053cadb:
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], eax
    L0053caeb:
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebx
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cb18
            mov eax, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x54]
            dec eax
            mov dword ptr [esp + 0x54], eax
            mov dword ptr [esp + 0x34], eax
            jmp L0053cb2d
    L0053cb18:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x50]
            dec eax
            mov dword ptr [esp + 0x34], ecx
            mov dword ptr [esp + 0x54], eax
            mov dword ptr [esp + 0x30], eax
    L0053cb2d:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            mov ecx, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x24]
            add esp, 0xc
            add eax, ecx
            jmp L0053cd5a
    L0053cb4e:
            mov edx, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x18]
            cmp ecx, edx
            jge L0053cc58
            test ebp, ebp
            je L0053cb6c
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053cb74
    L0053cb6c:
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053cb74:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cba3
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            inc edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053cbba
    L0053cba3:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053cbba:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cbe2
            mov ecx, dword ptr [esp + 0x50]
            mov edx, dword ptr [esp + 0x54]
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edx
            jmp L0053cbf2
    L0053cbe2:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], ecx
    L0053cbf2:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cc25
            mov eax, dword ptr [esp + 0x50]
            mov edx, dword ptr [esp + 0x10]
            sub eax, edx
            mov dword ptr [esp + 0x50], eax
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x54]
            dec eax
            mov dword ptr [esp + 0x54], eax
            jmp L0053cc40
    L0053cc25:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x10]
            dec eax
            mov dword ptr [esp + 0x54], eax
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x50]
            sub eax, ecx
            mov dword ptr [esp + 0x50], eax
    L0053cc40:
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebx
            push edx
            push esi
            mov dword ptr [esp + 0x40], eax
            call dword ptr [ecx + 0xa0]
            jmp L0053cd4d
    L0053cc58:
            test ebp, ebp
            je L0053cc70
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x34], edi
            add eax, ecx
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            jmp L0053cc82
    L0053cc70:
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x30], edi
            add eax, ecx
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053cc82:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cca7
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], edi
            jmp L0053ccb4
    L0053cca7:
            mov eax, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053ccb4:
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebx
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cce2
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x10]
            sub eax, ecx
            mov dword ptr [esp + 0x50], eax
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x54]
            jmp L0053ccf8
    L0053cce2:
            mov ecx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x30], ecx
            mov ecx, dword ptr [esp + 0x10]
            sub eax, ecx
            mov dword ptr [esp + 0x50], eax
    L0053ccf8:
            mov edx, dword ptr [esi]
            mov dword ptr [esp + 0x34], eax
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053cd29
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x50]
            dec eax
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x54], eax
            mov dword ptr [esp + 0x34], eax
            jmp L0053cd3e
    L0053cd29:
            mov eax, dword ptr [esp + 0x54]
            mov edx, dword ptr [esp + 0x50]
            dec eax
            mov dword ptr [esp + 0x34], edx
            mov dword ptr [esp + 0x54], eax
            mov dword ptr [esp + 0x30], eax
    L0053cd3e:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
    L0053cd4d:
            mov edx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x24]
            add esp, 0xc
            add eax, edx
    L0053cd5a:
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x28]
            dec eax
            mov dword ptr [esp + 0x28], eax
            jne L0053ca51
            mov ecx, dword ptr [esp + 0x18]
    L0053cd71:
            mov edx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x4c]
            test edx, edx
            je L0053d643
            test ecx, ecx
            jge L0053ce2f
            test ebp, ebp
            je L0053cd98
            inc edi
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053cda1
    L0053cd98:
            inc edi
            mov dword ptr [esp + 0x34], eax
            mov dword ptr [esp + 0x30], edi
    L0053cda1:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            mov eax, dword ptr [esp + 0x28]
            add esp, 0xc
            cmp eax, 1
            jle L0053cdee
            test ebp, ebp
            je L0053cdcf
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], edi
            jmp L0053cddc
    L0053cdcf:
            mov eax, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053cddc:
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebx
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            add esp, 0xc
    L0053cdee:
            cmp dword ptr [esp + 0x1c], 2
            jle L0053d643
            test ebp, ebp
            je L0053ce16
            mov ecx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x50]
            dec ecx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], ecx
            mov ecx, dword ptr [esi]
            push ebx
            jmp L0053d634
    L0053ce16:
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esi]
            dec edx
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], eax
            push ebx
            jmp L0053d634
    L0053ce2f:
            cmp ecx, dword ptr [esp + 0x20]
            jge L0053cef9
            test ebp, ebp
            je L0053ce48
            inc edi
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053ce51
    L0053ce48:
            inc edi
            mov dword ptr [esp + 0x34], eax
            mov dword ptr [esp + 0x30], edi
    L0053ce51:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            mov eax, dword ptr [esp + 0x28]
            add esp, 0xc
            cmp eax, 1
            jle L0053ceaa
            test ebp, ebp
            je L0053ce85
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x4c]
            add edx, eax
            inc edi
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], edi
            jmp L0053ce98
    L0053ce85:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            add ecx, edx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], ecx
    L0053ce98:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
    L0053ceaa:
            cmp dword ptr [esp + 0x1c], 2
            jle L0053d643
            test ebp, ebp
            je L0053cecc
            mov eax, dword ptr [esp + 0x54]
            mov edx, dword ptr [esp + 0x50]
            dec eax
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], eax
            jmp L0053cedd
    L0053cecc:
            mov ecx, dword ptr [esp + 0x54]
            mov edx, dword ptr [esp + 0x50]
            dec ecx
            mov dword ptr [esp + 0x34], edx
            mov dword ptr [esp + 0x30], ecx
    L0053cedd:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    L0053cef9:
            test ebp, ebp
            je L0053cf12
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            inc edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053cf25
    L0053cf12:
            mov ecx, dword ptr [esp + 0x10]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053cf25:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            mov eax, dword ptr [esp + 0x28]
            add esp, 0xc
            cmp eax, 1
            jle L0053cf72
            test ebp, ebp
            je L0053cf53
            mov ecx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edi
            jmp L0053cf60
    L0053cf53:
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], edx
    L0053cf60:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
    L0053cf72:
            cmp dword ptr [esp + 0x1c], 2
            jle L0053d643
            test ebp, ebp
            je L0053cfa0
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x54]
            sub eax, ecx
            mov ecx, dword ptr [esi]
            dec edx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edx
            push ebx
            jmp L0053d634
    L0053cfa0:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x10]
            dec eax
            push ebx
            mov dword ptr [esp + 0x34], eax
            mov eax, dword ptr [esp + 0x54]
            sub eax, ecx
            mov ecx, dword ptr [esi]
            mov dword ptr [esp + 0x38], eax
            jmp L0053d634
    L0053cfbf:
            sub ebp, ebx
            shl ebp, 1
            mov dword ptr [esp + 0x20], ebp
            lea eax, [ebp + ebp]
            mov dword ptr [esp + 0x24], eax
            add eax, ebx
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x28]
            test eax, eax
            jle L0053d361
            mov dword ptr [esp + 0x28], eax
    L0053cfe5:
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x18]
            inc edi
            dec edx
            test eax, eax
            mov dword ptr [esp + 0x54], edx
            jle L0053d136
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053d01b
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            jmp L0053d031
    L0053d01b:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053d031:
            mov ebx, dword ptr [esp + 0x58]
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            mov ebp, dword ptr [esp + 0x20]
            add esp, 0xc
            test ebp, ebp
            je L0053d068
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            inc edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053d07f
    L0053d068:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053d07f:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053d0ad
            mov ebx, dword ptr [esp + 0x50]
            mov eax, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x54]
            sub ebx, eax
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x34], ebp
            jmp L0053d0c3
    L0053d0ad:
            mov ebx, dword ptr [esp + 0x50]
            mov eax, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x54]
            sub ebx, eax
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x34], ebx
    L0053d0c3:
            mov edx, dword ptr [esp + 0x58]
            mov ecx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push edx
            push eax
            push esi
            call dword ptr [ecx + 0xa0]
            mov eax, dword ptr [esp + 0x20]
            add esp, 0xc
            test eax, eax
            je L0053d0fa
            mov ecx, dword ptr [esp + 0x10]
            sub ebx, ecx
            dec ebp
            mov dword ptr [esp + 0x50], ebx
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x54], ebp
            mov dword ptr [esp + 0x34], ebp
            jmp L0053d111
    L0053d0fa:
            mov eax, dword ptr [esp + 0x10]
            dec ebp
            sub ebx, eax
            mov dword ptr [esp + 0x54], ebp
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x50], ebx
            mov dword ptr [esp + 0x34], ebx
    L0053d111:
            mov edx, dword ptr [esp + 0x58]
            mov ecx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push edx
            push eax
            push esi
            call dword ptr [ecx + 0xa0]
            mov ecx, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x24]
            add esp, 0xc
            add eax, ecx
            jmp L0053d34e
    L0053d136:
            mov ecx, dword ptr [esp + 0x20]
            cmp eax, ecx
            mov eax, dword ptr [esp + 0x14]
            jge L0053d23c
            test eax, eax
            je L0053d158
            mov edx, dword ptr [esp + 0x4c]
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x30], edx
            jmp L0053d164
    L0053d158:
            mov eax, dword ptr [esp + 0x4c]
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053d164:
            mov ebx, dword ptr [esp + 0x58]
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebx
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            mov ebp, dword ptr [esp + 0x20]
            add esp, 0xc
            test ebp, ebp
            je L0053d19b
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            inc edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053d1b2
    L0053d19b:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053d1b2:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053d1da
            mov ebp, dword ptr [esp + 0x50]
            mov ebx, dword ptr [esp + 0x54]
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x34], ebx
            jmp L0053d1ea
    L0053d1da:
            mov ebx, dword ptr [esp + 0x54]
            mov ebp, dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x34], ebp
    L0053d1ea:
            mov eax, dword ptr [esp + 0x58]
            mov edx, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push eax
            push ecx
            push esi
            call dword ptr [edx + 0xa0]
            mov eax, dword ptr [esp + 0x20]
            add esp, 0xc
            test eax, eax
            je L0053d220
            mov ecx, dword ptr [esp + 0x10]
            sub ebp, ecx
            dec ebx
            mov dword ptr [esp + 0x50], ebp
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x54], ebx
            jmp L0053d32a
    L0053d220:
            mov eax, dword ptr [esp + 0x10]
            dec ebx
            sub ebp, eax
            mov dword ptr [esp + 0x54], ebx
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x50], ebp
            mov dword ptr [esp + 0x34], ebp
            jmp L0053d32e
    L0053d23c:
            test eax, eax
            je L0053d258
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            jmp L0053d26e
    L0053d258:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053d26e:
            mov ebx, dword ptr [esp + 0x58]
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            mov ebp, dword ptr [esp + 0x20]
            add esp, 0xc
            test ebp, ebp
            je L0053d29b
            mov ecx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edi
            jmp L0053d2a8
    L0053d29b:
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], edx
    L0053d2a8:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebx
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            test ebp, ebp
            je L0053d2da
            mov ebx, dword ptr [esp + 0x50]
            mov eax, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x54]
            sub ebx, eax
            mov dword ptr [esp + 0x50], ebx
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x34], ebp
            jmp L0053d2f4
    L0053d2da:
            mov ebx, dword ptr [esp + 0x50]
            mov eax, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x54]
            sub ebx, eax
            mov dword ptr [esp + 0x30], ebp
            mov dword ptr [esp + 0x50], ebx
            mov dword ptr [esp + 0x34], ebx
    L0053d2f4:
            mov eax, dword ptr [esp + 0x58]
            mov edx, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push eax
            push ecx
            push esi
            call dword ptr [edx + 0xa0]
            mov eax, dword ptr [esp + 0x20]
            add esp, 0xc
            test eax, eax
            je L0053d321
            dec ebp
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x54], ebp
            mov dword ptr [esp + 0x34], ebp
            jmp L0053d32e
    L0053d321:
            dec ebp
            mov dword ptr [esp + 0x54], ebp
            mov dword ptr [esp + 0x30], ebp
    L0053d32a:
            mov dword ptr [esp + 0x34], ebx
    L0053d32e:
            mov eax, dword ptr [esp + 0x58]
            mov edx, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push eax
            push ecx
            push esi
            call dword ptr [edx + 0xa0]
            mov edx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x24]
            add esp, 0xc
            add eax, edx
    L0053d34e:
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x28]
            dec eax
            mov dword ptr [esp + 0x28], eax
            jne L0053cfe5
    L0053d361:
            mov eax, dword ptr [esp + 0x1c]
            test eax, eax
            je L0053d643
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            jle L0053d459
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053d39a
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            inc edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053d3b1
    L0053d39a:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053d3b1:
            mov ebp, dword ptr [esp + 0x58]
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebp
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            mov eax, dword ptr [esp + 0x28]
            add esp, 0xc
            cmp eax, 1
            jle L0053d416
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053d3ee
            mov ebx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x4c]
            inc edi
            lea eax, [ebx + edx]
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x30], eax
            jmp L0053d402
    L0053d3ee:
            mov ebx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x4c]
            inc edi
            lea edx, [ebx + ecx]
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], edx
    L0053d402:
            mov eax, dword ptr [esi]
            lea ecx, [esp + 0x30]
            push ebp
            push ecx
            push esi
            call dword ptr [eax + 0xa0]
            add esp, 0xc
            jmp L0053d41a
    L0053d416:
            mov ebx, dword ptr [esp + 0x10]
    L0053d41a:
            cmp dword ptr [esp + 0x1c], 2
            jle L0053d643
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053d445
            mov eax, dword ptr [esp + 0x50]
            mov edx, dword ptr [esp + 0x54]
            sub eax, ebx
            dec edx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edx
            jmp L0053d631
    L0053d445:
            mov eax, dword ptr [esp + 0x54]
            dec eax
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x50]
            sub eax, ebx
            jmp L0053d62d
    L0053d459:
            mov ecx, dword ptr [esp + 0x20]
            cmp eax, ecx
            mov eax, dword ptr [esp + 0x14]
            jge L0053d53b
            test eax, eax
            je L0053d47c
            mov eax, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053d489
    L0053d47c:
            mov ecx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], ecx
    L0053d489:
            mov ebx, dword ptr [esp + 0x58]
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            mov ebp, dword ptr [esp + 0x28]
            add esp, 0xc
            cmp ebp, 1
            jle L0053d4ea
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053d4c5
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x4c]
            add ecx, edx
            inc edi
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edi
            jmp L0053d4d8
    L0053d4c5:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x4c]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], eax
    L0053d4d8:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
    L0053d4ea:
            cmp ebp, 2
            jle L0053d643
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0053d50e
            mov edx, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x50]
            dec edx
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edx
            jmp L0053d51f
    L0053d50e:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x50]
            dec eax
            mov dword ptr [esp + 0x34], ecx
            mov dword ptr [esp + 0x30], eax
    L0053d51f:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    L0053d53b:
            test eax, eax
            je L0053d558
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            inc edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053d56f
    L0053d558:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x10]
            inc edi
            add eax, ecx
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x4c], eax
            mov dword ptr [esp + 0x34], eax
    L0053d56f:
            mov ebp, dword ptr [esp + 0x58]
            mov ecx, dword ptr [esi]
            lea edx, [esp + 0x30]
            push ebp
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            mov eax, dword ptr [esp + 0x28]
            mov ebx, dword ptr [esp + 0x20]
            add esp, 0xc
            cmp eax, 1
            jle L0053d5c4
            test ebx, ebx
            je L0053d5a5
            mov eax, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edi
            jmp L0053d5b2
    L0053d5a5:
            mov ecx, dword ptr [esp + 0x4c]
            inc edi
            mov dword ptr [esp + 0x30], edi
            mov dword ptr [esp + 0x34], ecx
    L0053d5b2:
            mov edx, dword ptr [esi]
            lea eax, [esp + 0x30]
            push ebp
            push eax
            push esi
            call dword ptr [edx + 0xa0]
            add esp, 0xc
    L0053d5c4:
            cmp dword ptr [esp + 0x1c], 2
            jle L0053d643
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x18]
            cmp eax, ecx
            jle L0053d609
            test ebx, ebx
            je L0053d5f4
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x54]
            sub eax, ecx
            dec edx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], edx
            jmp L0053d631
    L0053d5f4:
            mov eax, dword ptr [esp + 0x54]
            mov ecx, dword ptr [esp + 0x10]
            dec eax
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x50]
            sub eax, ecx
            jmp L0053d62d
    L0053d609:
            test ebx, ebx
            je L0053d620
            mov ecx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x50]
            dec ecx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x34], ecx
            jmp L0053d631
    L0053d620:
            mov edx, dword ptr [esp + 0x54]
            mov eax, dword ptr [esp + 0x50]
            dec edx
            mov dword ptr [esp + 0x30], edx
    L0053d62d:
            mov dword ptr [esp + 0x34], eax
    L0053d631:
            mov ecx, dword ptr [esi]
            push ebp
    L0053d634:
            lea edx, [esp + 0x34]
            push edx
            push esi
            call dword ptr [ecx + 0xa0]
            add esp, 0xc
    L0053d643:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x38
            ret
    L0053d64d:
            mov ebx, dword ptr [esp + 0x58]
            mov ebp, dword ptr [esp + 0x14]
            jmp L0053cd71
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053d660
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_gen_text(br_device_pixelmap* self, br_point* point, br_font* font, const char* text, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING
    br_rectangle r;
    br_point p;
    br_int_32 x;
    br_int_32 y;
    br_int_32 s_stride;

    x = point->x + self->pm_origin_x;
    y = point->y + self->pm_origin_y;

    if (y <= -font->glyph_y || y >= self->pm_height || x >= self->pm_width) {
        return 0;
    }

    p.x = point->x;
    p.y = point->y;

    r.x = 0;
    r.y = 0;
    r.h = font->glyph_y;

    if (font->flags & BR_FONTF_VARIABLE_WIDTH) {
        for (;*text != '\0'; text++) {
            r.w = font->width[*(br_uint_8*)text];
            s_stride = (r.w + 7) / 8;
            if (x + r.w > 0) {
                self->dispatch->_copyBits(self, &p, &font->glyphs[font->encoding[*(br_uint_8*)text]], s_stride, &r, colour);
            }
            x += r.w + 1;
            p.x += r.w + 1;
            if (x > self->pm_width) {
                break;
            }
        }
    } else {
        r.w = font->glyph_x;
        s_stride = (r.w + 7) / 8;
        for (; *text != '\0'; text++) {
            if (x + r.w > 0) {
                self->dispatch->_copyBits(self, &p, &font->glyphs[font->encoding[*(br_uint_8*)text]], s_stride, &r, colour);
            }
            x += r.w + 1;
            p.x += r.w  + 1;
            if (x > self->pm_width) {
                break;
            }
        }
    }
    return 0;
#else
    __asm {
            sub esp, 0x18
            mov edx, dword ptr [esp + 0x20]
            push ebx
            mov ecx, dword ptr [edx]
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x28]
            mov eax, dword ptr [edx + 4]
            push edi
            mov edi, dword ptr [esp + 0x34]
            movsx ebp, word ptr [esi + 0x38]
            add ebp, ecx
            movsx ecx, word ptr [esi + 0x3a]
            add ecx, eax
            xor eax, eax
            mov ax, word ptr [edi + 6]
            mov ebx, eax
            neg ebx
            cmp ecx, ebx
            jle L0053d7f8
            xor ebx, ebx
            mov bx, word ptr [esi + 0x36]
            cmp ecx, ebx
            jge L0053d7f8
            xor ecx, ecx
            mov cx, word ptr [esi + 0x34]
            cmp ebp, ecx
            jge L0053d7f8
            mov ecx, dword ptr [edx]
            mov edx, dword ptr [edx + 4]
            mov dword ptr [esp + 0x24], eax
            mov al, byte ptr [edi]
            mov dword ptr [esp + 0x10], ecx
            xor ecx, ecx
            test al, 1
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x18], ecx
            mov dword ptr [esp + 0x1c], ecx
            je L0053d76d
            mov eax, dword ptr [esp + 0x38]
            mov al, byte ptr [eax]
            test al, al
            je L0053d7f8
    L0053d6e5:
            mov ecx, dword ptr [edi + 0xc]
            movsx ebx, al
            movsx ecx, byte ptr [ebx + ecx]
            mov dword ptr [esp + 0x20], ecx
            lea eax, [ecx + 7]
            cdq
            and edx, 7
            add eax, edx
            lea edx, [ebp + ecx]
            sar eax, 3
            test edx, edx
            jle L0053d736
            mov edx, dword ptr [esp + 0x3c]
            mov ecx, dword ptr [esi]
            push edx
            lea edx, [esp + 0x1c]
            push edx
            push eax
            mov eax, dword ptr [edi + 0x10]
            xor edx, edx
            mov dx, word ptr [eax + ebx*2]
            mov ebx, dword ptr [edi + 0x14]
            add edx, ebx
            lea eax, [esp + 0x1c]
            push edx
            push eax
            push esi
            call dword ptr [ecx + 0xa8]
            mov ecx, dword ptr [esp + 0x38]
            add esp, 0x18
    L0053d736:
            mov edx, dword ptr [esp + 0x10]
            lea ebp, [ebp + ecx + 1]
            lea eax, [edx + ecx + 1]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x34]
            mov dword ptr [esp + 0x10], eax
            cmp ebp, ecx
            jg L0053d7f8
            mov eax, dword ptr [esp + 0x38]
            inc eax
            mov dword ptr [esp + 0x38], eax
            mov al, byte ptr [eax]
            test al, al
            jne L0053d6e5
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L0053d76d:
            xor ebx, ebx
            mov bx, word ptr [edi + 4]
            mov dword ptr [esp + 0x20], ebx
            lea eax, [ebx + 7]
            cdq
            and edx, 7
            add eax, edx
            mov edx, dword ptr [esp + 0x38]
            sar eax, 3
            mov cl, byte ptr [edx]
            mov dword ptr [esp + 0x2c], eax
            test cl, cl
            je L0053d7f8
    L0053d791:
            lea edx, [ebp + ebx]
            test edx, edx
            jle L0053d7cf
            mov ebx, dword ptr [esp + 0x3c]
            mov edx, dword ptr [esi]
            push ebx
            lea ebx, [esp + 0x1c]
            push ebx
            push eax
            movsx eax, cl
            mov ecx, dword ptr [edi + 0x10]
            xor ebx, ebx
            mov bx, word ptr [ecx + eax*2]
            mov ecx, dword ptr [edi + 0x14]
            add ebx, ecx
            lea eax, [esp + 0x1c]
            push ebx
            push eax
            push esi
            call dword ptr [edx + 0xa8]
            mov ebx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x44]
            add esp, 0x18
    L0053d7cf:
            mov ecx, dword ptr [esp + 0x10]
            lea ebp, [ebp + ebx + 1]
            lea edx, [ecx + ebx + 1]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x34]
            mov dword ptr [esp + 0x10], edx
            cmp ebp, ecx
            jg L0053d7f8
            mov ecx, dword ptr [esp + 0x38]
            inc ecx
            mov dword ptr [esp + 0x38], ecx
            mov cl, byte ptr [ecx]
            test cl, cl
            jne L0053d791
    L0053d7f8:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x18
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053d810
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_gen_textBounds(br_device_pixelmap* self, br_rectangle* rect, br_font* font, const char* text) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;

    rect->x = 0;
    rect->y = 0;
    rect->w = 0;
    rect->h = font->glyph_y;
    if (text == NULL) {
        return 0;
    }
    if (!(font->flags & BR_FONTF_VARIABLE_WIDTH)) {
        rect->w = BrStrLen(text) * (font->glyph_x + 1) - 1;
        return 0;
    }
    j = BrStrLen(text);
    for (i = 0; i < j; i++) {
        rect->w += font->width[(br_uint_8)text[i]] + 1;
    }
    rect->w -= 1;
    return 0;
#else
    __asm {
            push ebx
            mov ebx, dword ptr [esp + 0x10]
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            mov edi, dword ptr [esp + 0x1c]
            xor eax, eax
            mov dword ptr [esi], 0
            mov dword ptr [esi + 4], 0
            mov dword ptr [esi + 8], 0
            mov ax, word ptr [ebx + 6]
            test edi, edi
            mov dword ptr [esi + 0xc], eax
            je L0053d88d
            mov al, byte ptr [ebx]
            push edi
            test al, 1
            je L0053d877
            call BrStrLen
            add esp, 4
            test eax, eax
            jle L0053d86a
    L0053d853:
            movsx ecx, byte ptr [edi]
            mov edx, dword ptr [ebx + 0xc]
            movsx ecx, byte ptr [ecx + edx]
            mov edx, dword ptr [esi + 8]
            inc ecx
            add edx, ecx
            inc edi
            dec eax
            mov dword ptr [esi + 8], edx
            jne L0053d853
    L0053d86a:
            mov eax, dword ptr [esi + 8]
            dec eax
            mov dword ptr [esi + 8], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    L0053d877:
            call BrStrLen
            xor edx, edx
            add esp, 4
            mov dx, word ptr [ebx + 4]
            inc edx
            imul eax, edx
            dec eax
            mov dword ptr [esi + 8], eax
    L0053d88d:
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053d8a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
    br_int_32 x;
    br_int_32 y;
    br_point p;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _pixelSet, 0xa0);

    for (y = bit_rect->y; y < bit_rect->y + bit_rect->h; y++) {
        for (x = bit_rect->x; x < bit_rect->x + bit_rect->w; x++) {
            if (src[y * s_stride + x / 8] & (0x80 >> (x % 8))) {
                p.x = point->x + x;
                p.y = point->y + y;
                self->dispatch->_pixelSet(self, &p, colour);
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d970
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_flush(br_device_pixelmap* self) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d980
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_synchronise(br_device_pixelmap* self, br_token sync_type, br_boolean block) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d990
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directLock(br_device_pixelmap* self, br_boolean block) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d9a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directUnlock(br_device_pixelmap* self) {

    return 0;
}
