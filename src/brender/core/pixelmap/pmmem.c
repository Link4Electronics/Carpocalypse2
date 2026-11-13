#include "pmmem.h"

#include "genclip.h"
#include "pmmemops.h"
#include "pmsetup.h"
#include "pmutils.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"
#include "core/pixelmap/pmgen.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0066ea98
br_tv_template_entry matchTemplateEntries[] = {
    { BRT_USE_T,            NULL,  0, 2, 3, 0, 0, },
    { BRT_PIXEL_TYPE_U8,    NULL,  4, 2, 3, 0, 0, },
    { BRT_PIXEL_BITS_I32,   NULL,  8, 2, 3, 0, 0, },
    { BRT_RENDERER_O,       NULL, 20, 2, 3, 0, 0, },
    { BRT_WIDTH_I32,        NULL, 12, 2, 3, 0, 0, },
    { BRT_HEIGHT_I32,       NULL, 16, 2, 3, 0, 0, }
};

// GLOBAL: CARMA2_HW 0x0058baf0
br_device_pixelmap_dispatch devicePixelmapDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_device_pixelmap_mem_free,
    (void*)_M_br_device_pixelmap_mem_identifier,
    (void*)_M_br_device_pixelmap_mem_type,
    (void*)_M_br_device_pixelmap_mem_isType,
    (void*)_M_br_device_pixelmap_mem_device,
    (void*)_M_br_device_pixelmap_mem_space,
    (void*)_M_br_device_pixelmap_mem_queryTemplate,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    (void*)_M_br_object_queryMany,
    (void*)_M_br_object_queryManySize,
    (void*)_M_br_object_queryAll,
    (void*)_M_br_object_queryAllSize,
    (void*)_M_br_device_pixelmap_mem_validSource,
    _M_br_device_pixelmap_mem_resize,
    _M_br_device_pixelmap_mem_match,
    _M_br_device_pixelmap_mem_allocateSub,
    _M_br_device_pixelmap_mem_copyTo,
    _M_br_device_pixelmap_mem_copyTo,
    _M_br_device_pixelmap_mem_copyFrom,
    _M_br_device_pixelmap_mem_fill,
    _M_br_device_pixelmap_gen_doubleBuffer,
    _M_br_device_pixelmap_gen_copyDirty,
    _M_br_device_pixelmap_gen_copyToDirty,
    _M_br_device_pixelmap_gen_copyFromDirty,
    _M_br_device_pixelmap_gen_fillDirty,
    _M_br_device_pixelmap_gen_doubleBufferDirty,
    _M_br_device_pixelmap_gen_rectangle,
    _M_br_device_pixelmap_gen_rectangle2,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleCopyFrom,
    _M_br_device_pixelmap_mem_rectangleStretchCopyTo,
    _M_br_device_pixelmap_mem_rectangleStretchCopyTo,
    _M_br_device_pixelmap_mem_rectangleStretchCopyFrom,
    _M_br_device_pixelmap_mem_rectangleFill,
    _M_br_device_pixelmap_mem_pixelSet,
    _M_br_device_pixelmap_mem_line,
    _M_br_device_pixelmap_mem_copyBits,
    (void*)_M_br_device_pixelmap_gen_text,
    (void*)_M_br_device_pixelmap_gen_textBounds,
    _M_br_device_pixelmap_mem_rowSize,
    (void*)_M_br_device_pixelmap_mem_rowSet,
    (void*)_M_br_device_pixelmap_mem_rowQuery,
    _M_br_device_pixelmap_mem_pixelQuery,
    _M_br_device_pixelmap_mem_pixelAddressQuery,
    _M_br_device_pixelmap_mem_pixelAddressSet,
    _M_br_device_pixelmap_mem_originSet,
    _M_br_device_pixelmap_mem_flush,
    _M_br_device_pixelmap_mem_synchronise,
    _M_br_device_pixelmap_mem_directLock,
    _M_br_device_pixelmap_mem_directUnlock,
};

// GLOBAL: CARMA2_HW 0x0058ba90
br_tv_template_entry devicePixelmapTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_pixelmap, identifier), 0x5, 0x3, 0, 0, },
    { BRT_WIDTH_I32, NULL, offsetof(br_pixelmap, width),            0x5, 0xd, 0, 0, },
    { BRT_HEIGHT_I32, NULL, offsetof(br_pixelmap, height),          0x5, 0xd, 0, 0, },
    { BRT_PIXEL_TYPE_U8, NULL, offsetof(br_pixelmap, type),         0x5, 0xc, 0, 0, },
};

// GLOBAL: CARMA2_HW 0x0058b990
pm_type_info pmTypeInfo[32] = {
    { 0x1,  0x1,  0x20, 0x1, },
    { 0x2,  0x1,  0x10, 0x1, },
    { 0x4,  0x1,  0x8,  0x1, },
    { 0x8,  0x1,  0x4,  0x1, },
    { 0x10, 0x2,  0x2,  0x2, },
    { 0x10, 0x2,  0x2,  0x2, },
    { 0x18, 0x3,  0x4,  0x2, },
    { 0x20, 0x4,  0x1,  0x2, },
    { 0x20, 0x4,  0x1,  0xa, },
    { 0x10, 0x1,  0x2,  0x10,},
    { 0x20, 0x1,  0x1,  0x10,},
    { 0x10, 0x2,  0x4,  0x4, },
    { 0x20, 0x4,  0x4,  0x4, },
    { 0x8,  0x1,  0x4,  0x8, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x8,  0x1,  0x4,  0x2, },
    { 0x8,  0x1,  0x4,  0x4, },
    { 0x20, 0x4,  0x1,  0xa, },
    { 0x4,  0x1,  0x8,  0x8, },
    { 0x8,  0x1,  0x4,  0x9, },
    { 0x8,  0x1,  0x4,  0x9, },
    { 0x10, 0x2,  0x2,  0x4, },
    { 0x20, 0x4,  0x1,  0x4, },
    { 0x10, 0x2,  0x2,  0x4, },
    { 0x10, 0x2,  0x2,  0x4, },
    { 0x10, 0x2,  0x2,  0xa, },
};

#ifdef CARPOCALYPSE2_MATCHING
extern br_clip_result C2_HOOK_CDECL PixelmapLineClip();
extern br_clip_result C2_HOOK_CDECL PixelmapRectangleClip();
extern br_clip_result C2_HOOK_CDECL PixelmapRectangleClipTwo();
extern void C2_HOOK_CDECL pm_mem_copy_colour();
extern void C2_HOOK_CDECL pm_mem_copy_colour_rowbyrow();
extern void C2_HOOK_CDECL pm_mem_fill_colour();
extern void C2_HOOK_CDECL pm_mem_fill_colour_rect();
extern void C2_HOOK_CDECL pm_mem_set_colour();
#endif
// FUNCTION: CARMA2_HW 0x00539620
br_device_pixelmap* C2_HOOK_STDCALL DevicePixelmapMemAllocate(br_uint_8 type, br_uint_16 w, br_uint_16 h, void* pixels, int flags) {
    br_device_pixelmap* pm;
    pm_type_info* tip;

    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);

    tip = &pmTypeInfo[type];
    pm = BrResAllocate(_pixelmap.res, sizeof(br_device_pixelmap), BR_MEMORY_PIXELMAP);
    pm->pm_type = type;
    pm->dispatch = &devicePixelmapDispatch;
    pm->pm_identifier = NULL;
    pm->pm_map = NULL;
    pm->pm_flags = BR_PMF_LINEAR;
    pm->pm_copy_function = BR_PMCOPY_NORMAL;
    pm->pm_base_x = 0;
    pm->pm_base_y = 0;
    pm->pm_width = w;
    pm->pm_height = h;
    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;

    pm->pm_row_bytes = tip->bits * tip->align * ((w + tip->align - 1) / tip->align) / 8;

    if ((8 * pm->pm_row_bytes % tip->bits) == 0) {
        pm->pm_flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    if (!(flags & BR_PMAF_NO_PIXELS)) {
        if (pixels != NULL) {
            pm->pm_pixels = pixels;
        } else {
            pm->pm_pixels = BrResAllocate(pm, pm->pm_height * pm->pm_row_bytes, BR_MEMORY_PIXELS);
        }
    }

     pm->pm_pixels_qualifier = GetSysQual();

    if (flags & BR_PMAF_INVERTED) {
        pm->pm_pixels = (char*)pm->pm_pixels + (pm->pm_height - 1) * pm->pm_row_bytes;
        pm->pm_row_bytes = -pm->pm_row_bytes;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00539730
void C2_HOOK_STDCALL CheckDispatch(br_device_pixelmap* pm) {
    if (pm->dispatch == NULL) {
        pm->dispatch = &devicePixelmapDispatch;
    }
}

// FUNCTION: CARMA2_HW 0x00539750
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_allocateSub(br_device_pixelmap* self, br_device_pixelmap** newpm, br_rectangle* rect) {
    br_device_pixelmap* pm;
    br_rectangle out;

    if (PixelmapRectangleClip(&out, rect, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 4098;
    }

    pm = (br_device_pixelmap*)BrResAllocate(_pixelmap.res, sizeof(br_pixelmap), BR_MEMORY_PIXELMAP);
    memcpy(pm, self, sizeof(br_pixelmap));
    pm->pm_base_x += out.x;
    pm->pm_base_y += out.y;
    pm->pm_width = out.w;
    pm->pm_height = out.h;
    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;
    pm->pm_stored = 0;
    pm->dispatch = &devicePixelmapDispatch;
    if (self->pm_width != pm->pm_width) {
        pm->pm_flags &= ~BR_PMF_LINEAR;
    }
    *newpm = pm;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005397f0
void C2_HOOK_CDECL _M_br_device_pixelmap_mem_free(br_device_pixelmap* self) {
    BrResFree(self);
}

// FUNCTION: CARMA2_HW 0x00539800
char* C2_HOOK_CDECL _M_br_device_pixelmap_mem_identifier(br_device_pixelmap* self) {

    return self->pm_identifier;
}

// FUNCTION: CARMA2_HW 0x00539810
br_token C2_HOOK_CDECL _M_br_device_pixelmap_mem_type(br_device_pixelmap* self) {

    return BRT_DEVICE_PIXELMAP;
}

// FUNCTION: CARMA2_HW 0x00539820
br_boolean C2_HOOK_CDECL _M_br_device_pixelmap_mem_isType(br_device_pixelmap* self, br_token t) {

    return t == BRT_DEVICE_PIXELMAP || t == BRT_OBJECT;
}

// FUNCTION: CARMA2_HW 0x00539840
br_device* C2_HOOK_CDECL _M_br_device_pixelmap_mem_device(br_device_pixelmap* self) {

    return NULL;
}

// FUNCTION: CARMA2_HW 0x00539850
br_int_32 C2_HOOK_CDECL _M_br_device_pixelmap_mem_space(br_device_pixelmap* self) {

    return BrResSizeTotal(self);
}

// FUNCTION: CARMA2_HW 0x00539860
br_tv_template* C2_HOOK_CDECL _M_br_device_pixelmap_mem_queryTemplate(br_device_pixelmap* self) {

    if (_pixelmap.device_pixelmap_template == NULL) {
        _pixelmap.device_pixelmap_template = BrTVTemplateAllocate(_pixelmap.res, devicePixelmapTemplateEntries, BR_ASIZE(devicePixelmapTemplateEntries));
    }
    return _pixelmap.device_pixelmap_template;
}

// FUNCTION: CARMA2_HW 0x00539890
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_validSource(br_device_pixelmap* self, br_boolean* b, br_object* h) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x005398a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_resize(br_device_pixelmap* self, br_int_32 width, br_int_32 height) {
    char* pixels;
    pm_type_info* tip;
    br_int_16 old_row_bytes;

    tip = &pmTypeInfo[self->pm_type];
    pixels = self->pm_pixels;
    if (self->pm_row_bytes < 0) {
        pixels += (self->pm_height - 1) * self->pm_row_bytes;
    }
    if (BrResIsChild(self, pixels)) {
        BrResFree(pixels);
    }
    self->pm_width = width;
    self->pm_height = height;
    old_row_bytes = self->pm_row_bytes;
    self->pm_row_bytes = tip->bits * tip->align * ((width + tip->align - 1) / tip->align) / 8;

    if (((8 * self->pm_row_bytes) % tip->bits) == 0) {
        self->pm_flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    self->pm_pixels = BrResAllocate(self, self->pm_row_bytes * self->pm_height, BR_MEMORY_PIXELS);
    self->pm_pixels_qualifier = GetSysQual();
    if (old_row_bytes < 0) {
        self->pm_pixels = (char *)self->pm_pixels + (self->pm_height - 1) * self->pm_row_bytes;
        self->pm_row_bytes = -self->pm_row_bytes;
    }
    return 0;
}

// IDA: br_error __cdecl _M_br_device_pixelmap_mem_match(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
// FUNCTION: CARMA2_HW 0x00539990
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_match(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {
    match_tokens mt = { BRT_NONE, 0 };
    br_int_32 count;
    br_device_pixelmap* pm;
    br_int_32 bytes;
    br_int_32 r;

    if (_pixelmap.pixelmap_match_template == NULL) {
        _pixelmap.pixelmap_match_template = BrTVTemplateAllocate(_pixelmap.res, matchTemplateEntries, BR_ASIZE(matchTemplateEntries));
        if (_pixelmap.pixelmap_match_template == NULL) {
            return 0x1002;
        }
    }
    mt.pixel_type = self->pm_type;
    mt.width = self->pm_width;
    mt.height = self->pm_height;
    BrTokenValueSetMany(&mt, &count, NULL, tv, _pixelmap.pixelmap_match_template);

    switch(mt.use) {
    case BRT_DEPTH:
        switch (mt.pixel_bits) {
        case 0:
        case 8:
            mt.pixel_type = BR_PMT_DEPTH_8;
            break;
        case 16:
            mt.pixel_type = BR_PMT_DEPTH_16;
            break;
        case 32:
            mt.pixel_type = BR_PMT_DEPTH_32;
            break;
        default:
            return 0x1002;
        }
        break;
    case BRT_CLONE:
    case BRT_OFFSCREEN:
    case BRT_HIDDEN:
    case BRT_HIDDEN_BUFFER:
    case BRT_NO_RENDER:
        break;
    default:
        return 0x1002;
    }
    if (mt.use == BRT_DEPTH) {
        pm = DevicePixelmapMemAllocate(mt.pixel_type, mt.width, mt.height, NULL, ((self->pm_row_bytes < 0) ? BR_PMAF_INVERTED : 0) | BR_PMAF_NO_PIXELS);
        bytes = pmTypeInfo[self->pm_type].bits >> 3;
        r = self->pm_row_bytes;
        if (r < 0) {
            r = -r;
        }
        r = (r + bytes - 1) / bytes;
        pm->pm_row_bytes = r * (pmTypeInfo[pm->pm_type].bits >> 3);

        pm->pm_pixels = BrResAllocate(pm, pm->pm_row_bytes * pm->pm_height, BR_MEMORY_PIXELS);

        if (pm->pm_width * (pmTypeInfo[pm->pm_type].bits >> 3) == pm->pm_row_bytes) {
            pm->pm_flags |= BR_PMF_LINEAR;
        } else {
            pm->pm_flags &= ~BR_PMF_LINEAR;
        }

        if (self->pm_row_bytes < 0) {
            pm->pm_pixels = (char *)pm->pm_pixels + pm->pm_row_bytes * (pm->pm_height - 1);
            pm->pm_row_bytes = -pm->pm_row_bytes;
        }
    } else {
        pm = DevicePixelmapMemAllocate(mt.pixel_type, mt.width, mt.height, NULL, (self->pm_row_bytes < 0) ? BR_PMAF_INVERTED : 0);
    }

    pm->pm_origin_x = self->pm_origin_x;
    pm->pm_origin_y = self->pm_origin_y;

    *newpm = pm;

    return 0;
}

// FUNCTION: CARMA2_HW 0x00539bf0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_int_8 bytes;

    bytes = pmTypeInfo[self->pm_type].bits / 8;
    if (self->pm_row_bytes == src->pm_row_bytes
        && (self->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)
        && (src->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) {
        if (self->pm_row_bytes > 0) {
            pm_mem_copy_colour(
                (br_uint_8*)self->pm_pixels + self->pm_base_y * self->pm_row_bytes + self->pm_base_x * bytes,
                self->pm_pixels_qualifier,
                (br_uint_8*)src->pm_pixels + src->pm_base_y * src->pm_row_bytes + src->pm_base_x * bytes,
                src->pm_pixels_qualifier,
                self->pm_height * self->pm_width,
                bytes);
        } else {
            pm_mem_copy_colour(
                (br_uint_8*)self->pm_pixels + (self->pm_base_y + (self->pm_height - 1)) * self->pm_row_bytes + self->pm_base_x * bytes,
                self->pm_pixels_qualifier,
                (br_uint_8*)src->pm_pixels + (src->pm_base_y + (src->pm_height - 1)) * src->pm_row_bytes + src->pm_base_x * bytes,
                src->pm_pixels_qualifier,
                self->pm_width * self->pm_height,
                bytes);
        }
    } else {
        if ((self->pm_row_bytes & 0x7) == 0) {
            pm_mem_copy_colour_rowbyrow(
                (br_uint_8*)self->pm_pixels, self->pm_pixels_qualifier,
                (br_uint_8*)src->pm_pixels, src->pm_pixels_qualifier,
                self->pm_width, self->pm_height,
                self->pm_row_bytes, src->pm_row_bytes, bytes);
        } else {
            br_uint_8 *s, *d;
            br_uint_32 y;

            s = src->pm_pixels;
            d = self->pm_pixels;
            for (y = 0; y < self->pm_height; y++) {
                pm_mem_copy_colour(
                    d,
                    self->pm_pixels_qualifier,
                    s,
                    src->pm_pixels_qualifier,
                    self->pm_width,
                    bytes);
                s += src->pm_row_bytes;
                d += self->pm_row_bytes;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00539dc0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyFrom(br_device_pixelmap* self, br_device_pixelmap* dest) {
    br_int_8 bytes;

    bytes = pmTypeInfo[self->pm_type].bits / 8;
    if (self->pm_row_bytes == dest->pm_row_bytes
        && (self->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == ((BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS))
        && (dest->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == ((BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS))) {
        if (self->pm_row_bytes > 0) {
            pm_mem_copy_colour(
                    (br_uint_8*)dest->pm_pixels + dest->pm_base_y * dest->pm_row_bytes + dest->pm_base_x * bytes,
                    dest->pm_pixels_qualifier,
                    (br_uint_8*)self->pm_pixels + self->pm_base_y * self->pm_row_bytes + self->pm_base_x * bytes,
                    self->pm_pixels_qualifier,
                    self->pm_height * self->pm_width,
                    bytes);
        } else {
            pm_mem_copy_colour(
                    (br_uint_8*)dest->pm_pixels + (dest->pm_base_y + (dest->pm_height - 1)) * dest->pm_row_bytes + dest->pm_base_x * bytes,
                    dest->pm_pixels_qualifier,
                    (br_uint_8*)self->pm_pixels + (self->pm_base_y + (self->pm_height - 1)) * self->pm_row_bytes + self->pm_base_x * bytes,
                    self->pm_pixels_qualifier,
                    self->pm_width * self->pm_height,
                    bytes);
        }
    } else {
        if ((dest->pm_row_bytes & 0x7) == 0) {
            pm_mem_copy_colour_rowbyrow(
                    (br_uint_8*)dest->pm_pixels, dest->pm_pixels_qualifier,
                    (br_uint_8*)self->pm_pixels, self->pm_pixels_qualifier,
                    self->pm_width, self->pm_height,
                    dest->pm_row_bytes, self->pm_row_bytes, bytes);
        } else {
            br_uint_8 *s, *d;
            br_uint_32 y;

            s = self->pm_pixels;
            d = dest->pm_pixels;

            for (y = 0; y < self->pm_height; y++) {
                pm_mem_copy_colour(
                        d,
                        dest->pm_pixels_qualifier,
                        s,
                        self->pm_pixels_qualifier,
                        self->pm_width,
                        bytes);
                s += self->pm_row_bytes;
                d += dest->pm_row_bytes;
            }
        }
    }
            return 0;
}

// FUNCTION: CARMA2_HW 0x00539f90
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_mem_fill(br_device_pixelmap* self, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING
    br_int_8 bytes;

    bytes = pmTypeInfo[self->pm_type].bits / 8;
    if ((self->pm_flags & (BR_PMF_ROW_WHOLEPIXELS | BR_PMF_LINEAR)) == (BR_PMF_ROW_WHOLEPIXELS | BR_PMF_LINEAR)) {
        if (self->pm_row_bytes > 0) {
            pm_mem_fill_colour((br_uint_8*)self->pm_pixels + self->pm_base_y * self->pm_row_bytes + self->pm_base_x * bytes, self->pm_pixels_qualifier,
                               self->pm_width * self->pm_height, bytes, colour);
        } else {
            pm_mem_fill_colour((br_uint_8*)self->pm_pixels + (self->pm_base_y + (self->pm_height - 1)) * self->pm_row_bytes + self->pm_base_x * bytes,
                               self->pm_pixels_qualifier, self->pm_width * self->pm_height, bytes, colour);
        }
    } else if ((self->pm_row_bytes & 7) == 0) {
        pm_mem_fill_colour_rect((br_uint_8*)self->pm_pixels + self->pm_base_y * self->pm_row_bytes + self->pm_base_x * bytes, self->pm_pixels_qualifier,
                                self->pm_width, self->pm_height, self->pm_row_bytes, bytes, colour);
    } else {
        br_uint_8 *d;
        br_uint_32 y;

        d = self->pm_pixels;

        for (y = 0; y < self->pm_height; y++) {
            pm_mem_fill_colour(
                d,
                self->pm_pixels_qualifier,
                self->pm_width,
                bytes,
                colour);

			d += self->pm_row_bytes;
        }
    }
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x10]
            xor eax, eax
            push edi
            mov al, byte ptr [esi + 0x2c]
            mov cl, byte ptr [esi + 0x2d]
            and cl, 6
            mov ax, word ptr [eax*8 + pmTypeInfo]
            shr eax, 3
            cmp cl, 6
            jne L0053a056
            mov dx, word ptr [esi + 0x28]
            test dx, dx
            jle L0053a009
            mov ecx, dword ptr [esp + 0x18]
            xor edi, edi
            mov di, word ptr [esi + 0x34]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x36]
            imul ecx, edi
            movsx eax, al
            push eax
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            mov edi, dword ptr [esi + 8]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            movsx edx, dx
            imul ecx, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            imul edx, eax
            add ecx, edx
            add ecx, edi
            push ecx
            call pm_mem_fill_colour
            add esp, 0x14
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0053a009:
            mov edi, dword ptr [esp + 0x18]
            movsx ecx, al
            push edi
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            xor edi, edi
            mov di, word ptr [esi + 0x34]
            push ecx
            imul edi, eax
            push edi
            mov edi, dword ptr [esi + 0xc]
            push edi
            xor edi, edi
            mov di, word ptr [esi + 0x32]
            movsx edx, dx
            lea eax, [edi + eax - 1]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            imul edx, ecx
            add eax, edx
            mov edx, dword ptr [esi + 8]
            add eax, edx
            push eax
            call pm_mem_fill_colour
            add esp, 0x14
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0053a056:
            mov cx, word ptr [esi + 0x28]
            test cl, 7
            jne L0053a0a7
            mov edx, dword ptr [esp + 0x18]
            mov edi, dword ptr [esi + 8]
            movsx eax, al
            movsx ecx, cx
            push edx
            xor edx, edx
            mov dx, word ptr [esi + 0x36]
            push eax
            push ecx
            push edx
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            push edx
            mov edx, dword ptr [esi + 0xc]
            push edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            imul edx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            imul eax, ecx
            add edx, eax
            add edx, edi
            push edx
            call pm_mem_fill_colour_rect
            add esp, 0x1c
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0053a0a7:
            mov edi, dword ptr [esi + 8]
            xor ebx, ebx
            cmp word ptr [esi + 0x36], bx
            jbe L0053a0e0
            movsx ebp, al
    L0053a0b5:
            mov ecx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esi + 0xc]
            xor edx, edx
            push ecx
            mov dx, word ptr [esi + 0x34]
            push ebp
            push edx
            push eax
            push edi
            call pm_mem_fill_colour
            movsx ecx, word ptr [esi + 0x28]
            xor edx, edx
            add esp, 0x14
            mov dx, word ptr [esi + 0x36]
            add edi, ecx
            inc ebx
            cmp ebx, edx
            jb L0053a0b5
    L0053a0e0:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053a0f0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053a100
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053a110
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053a120
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053a130
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053a140
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053a150
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r) {
#ifndef CARPOCALYPSE2_MATCHING
    int bytes;
    br_rectangle ar;
    br_point ap;

    if (PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap*)self, (br_pixelmap*)src) == BR_CLIP_REJECT) {
        return 0;
    }
    bytes = pmTypeInfo[self->pm_type].bits / 8;
    if (ar.w == self->pm_width) {
        if (self->pm_row_bytes == src->pm_row_bytes
            && (self->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)
            && (src->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) {
                if (self->pm_row_bytes > 0) {
                    pm_mem_copy_colour(
                        (br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y) * self->pm_row_bytes + (self->pm_base_x + ap.x) * bytes,
                        self->pm_pixels_qualifier,
                        (br_uint_8*)src->pm_pixels + (src->pm_base_y + ar.y) * src->pm_row_bytes + (src->pm_base_x + ar.x) * bytes,
                        src->pm_pixels_qualifier,
                        ar.w * ar.h,
                        bytes);
                    return 0;
                } else {
                    pm_mem_copy_colour(
                        (br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y - 1 + ar.h) * self->pm_row_bytes + (self->pm_base_x + ap.x) * bytes,
                        self->pm_pixels_qualifier,
                        (br_uint_8*)src->pm_pixels + (src->pm_base_y + ar.y - 1 + ar.h) * src->pm_row_bytes + (src->pm_base_x + ar.x) * bytes,
                        src->pm_pixels_qualifier,
                        ar.w * ar.h,
                        bytes);
                return 0;
            }
        }
    }
    if ((self->pm_row_bytes & 0x7) == 0) {
        pm_mem_copy_colour_rowbyrow(
            (br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y) * self->pm_row_bytes + (self->pm_base_x + ap.x) * bytes,
            self->pm_pixels_qualifier,
            (br_uint_8*)src->pm_pixels + (src->pm_base_y + ar.y) * src->pm_row_bytes + (src->pm_base_x + ar.x) * bytes,
            src->pm_pixels_qualifier,
            ar.w, ar.h,
            self->pm_row_bytes, src->pm_row_bytes,
            bytes);
        return 0;
    } else {
        int i;

        for (i = 0; i < ar.h; i++) {
            pm_mem_copy_colour(
                (br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y + i) * self->pm_row_bytes + (self->pm_base_x + ap.x) * bytes,
                self->pm_pixels_qualifier,
                (br_uint_8*)src->pm_pixels + (src->pm_base_y + ar.y + i) * src->pm_row_bytes + (src->pm_base_x + ar.x) * bytes,
                self->pm_pixels_qualifier,
                ar.w,
                bytes);
        }
        return 0;
    }
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0x10]
            sub esp, 0x18
            lea edx, [esp]
            push esi
            mov esi, dword ptr [esp + 0x28]
            push edi
            mov edi, dword ptr [esp + 0x24]
            push esi
            push edi
            push eax
            push ecx
            lea eax, [esp + 0x20]
            push edx
            push eax
            call PixelmapRectangleClipTwo
            add esp, 0x18
            test eax, eax
            jne L0053a185
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a185:
            xor ecx, ecx
            xor eax, eax
            mov cl, byte ptr [edi + 0x2c]
            xor edx, edx
            mov dx, word ptr [edi + 0x34]
            push ebp
            mov ax, word ptr [ecx*8 + pmTypeInfo]
            mov ecx, dword ptr [esp + 0x1c]
            shr eax, 3
            cmp ecx, edx
            push ebx
            mov dword ptr [esp + 0x34], eax
            jne L0053a2ca
            mov bp, word ptr [edi + 0x28]
            mov bx, word ptr [esi + 0x28]
            cmp bp, bx
            jne L0053a2ca
            mov dl, byte ptr [edi + 0x2d]
            and dl, 6
            cmp dl, 6
            jne L0053a2ca
            mov dl, byte ptr [esi + 0x2d]
            and dl, 6
            cmp dl, 6
            jne L0053a2ca
            test bp, bp
            push eax
            jle L0053a252
            imul ecx, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            xor edx, edx
            push ecx
            mov dx, word ptr [esi + 0x30]
            mov ecx, dword ptr [esp + 0x24]
            add edx, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            imul edx, eax
            add ecx, dword ptr [esp + 0x28]
            movsx ebx, bx
            imul ecx, ebx
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            add edx, ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x30]
            push edx
            mov edx, dword ptr [edi + 0xc]
            mov esi, dword ptr [edi + 8]
            push edx
            mov edx, dword ptr [esp + 0x24]
            add ecx, edx
            xor edx, edx
            mov dx, word ptr [edi + 0x32]
            imul ecx, eax
            add edx, dword ptr [esp + 0x28]
            movsx eax, bp
            imul edx, eax
            add ecx, edx
            add ecx, esi
            push ecx
            call pm_mem_copy_colour
            add esp, 0x18
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a252:
            mov edx, dword ptr [esp + 0x28]
            imul ecx, edx
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            add ecx, dword ptr [esp + 0x28]
            movsx ebx, bx
            lea ecx, [ecx + edx - 1]
            imul ecx, ebx
            xor ebx, ebx
            mov bx, word ptr [esi + 0x30]
            add ebx, dword ptr [esp + 0x24]
            imul ebx, eax
            add ecx, ebx
            mov ebx, dword ptr [esi + 8]
            add ecx, ebx
            mov esi, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [edi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x32]
            add ecx, esi
            mov esi, dword ptr [edi + 8]
            lea edx, [ecx + edx - 1]
            movsx ecx, bp
            mov ebp, dword ptr [esp + 0x24]
            imul edx, ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x30]
            add ecx, ebp
            imul ecx, eax
            add edx, ecx
            add edx, esi
            push edx
            call pm_mem_copy_colour
            add esp, 0x18
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a2ca:
            mov dx, word ptr [edi + 0x28]
            test dl, 7
            jne L0053a347
            movsx ebp, word ptr [esi + 0x28]
            mov ebx, dword ptr [esp + 0x24]
            push eax
            movsx edx, dx
            push ebp
            push edx
            push ebx
            mov ebx, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            add ecx, ebx
            xor ebx, ebx
            mov bx, word ptr [esi + 0x32]
            imul ecx, eax
            add ebx, dword ptr [esp + 0x34]
            imul ebx, ebp
            add ecx, ebx
            mov ebx, dword ptr [esi + 8]
            add ecx, ebx
            mov esi, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [edi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x30]
            mov ebp, dword ptr [esp + 0x34]
            add ecx, esi
            mov esi, dword ptr [edi + 8]
            imul ecx, eax
            xor eax, eax
            mov ax, word ptr [edi + 0x32]
            add eax, ebp
            imul eax, edx
            add ecx, eax
            add ecx, esi
            push ecx
            call pm_mem_copy_colour_rowbyrow
            add esp, 0x24
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a347:
            mov ebp, dword ptr [esp + 0x18]
            xor ebx, ebx
            mov bx, word ptr [esi + 0x30]
            mov dword ptr [esp + 0x2c], 0
            add ebx, ebp
            mov ebp, dword ptr [esp + 0x1c]
            imul ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add eax, ebp
            movsx ebp, word ptr [esi + 0x28]
            imul eax, ebp
            mov ebp, dword ptr [esi + 8]
            add ebx, eax
            mov eax, dword ptr [esp + 0x10]
            add ebx, ebp
            xor ebp, ebp
            mov bp, word ptr [edi + 0x30]
            add ebp, eax
            xor eax, eax
            mov ax, word ptr [edi + 0x32]
            imul ebp, dword ptr [esp + 0x34]
            add eax, dword ptr [esp + 0x14]
            movsx edx, dx
            imul eax, edx
            mov edx, dword ptr [edi + 8]
            add ebp, eax
            mov eax, dword ptr [esp + 0x24]
            add ebp, edx
            test eax, eax
            jbe L0053a3e3
            jmp L0053a3ae
    L0053a3aa:
            mov ecx, dword ptr [esp + 0x20]
    L0053a3ae:
            mov eax, dword ptr [esp + 0x34]
            mov edx, dword ptr [edi + 0xc]
            push eax
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push ecx
            push ebx
            push edx
            push ebp
            call pm_mem_copy_colour
            movsx eax, word ptr [esi + 0x28]
            movsx ecx, word ptr [edi + 0x28]
            add ebx, eax
            mov eax, dword ptr [esp + 0x44]
            add esp, 0x18
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x24]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x2c], eax
            jb L0053a3aa
    L0053a3e3:
            pop ebx
            pop ebp
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053a3f0
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleCopyFrom(br_device_pixelmap* self, br_point* p, br_device_pixelmap* dest, br_rectangle* r) {
#ifndef CARPOCALYPSE2_MATCHING
    int bytes;
    br_rectangle ar;
    br_point ap;

    if (PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap*)dest, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0;
    }
    bytes = pmTypeInfo[self->pm_type].bits / 8;
    if (ar.w == self->pm_width) {
        if (self->pm_row_bytes == dest->pm_row_bytes
            && (self->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)
            && (dest->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) {
                if (self->pm_row_bytes > 0) {
                    pm_mem_copy_colour(
                        (br_uint_8*)dest->pm_pixels + (dest->pm_base_y + ap.y) * dest->pm_row_bytes + (dest->pm_base_x + ap.x) * bytes,
                        dest->pm_pixels_qualifier,
                        (br_uint_8*)self->pm_pixels + (self->pm_base_y + ar.y) * self->pm_row_bytes + (self->pm_base_x + ar.x) * bytes,
                        self->pm_pixels_qualifier,
                        ar.w * ar.h,
                        bytes);
                    return 0;
                } else {
                    pm_mem_copy_colour(
                        (br_uint_8*)dest->pm_pixels + (dest->pm_base_y + ap.y - 1 + ar.h) * dest->pm_row_bytes + (dest->pm_base_x + ap.x) * bytes,
                        dest->pm_pixels_qualifier,
                        (br_uint_8*)self->pm_pixels + (self->pm_base_y + ar.y - 1 + ar.h) * self->pm_row_bytes + (self->pm_base_x + ar.x) * bytes,
                        self->pm_pixels_qualifier,
                        ar.w * ar.h,
                        bytes);
                return 0;
            }
        }
    }
    if ((dest->pm_row_bytes & 0x7) == 0) {
        pm_mem_copy_colour_rowbyrow(
            (br_uint_8*)dest->pm_pixels + (dest->pm_base_y + ap.y) * dest->pm_row_bytes + (dest->pm_base_x + ap.x) * bytes,
            dest->pm_pixels_qualifier,
            (br_uint_8*)self->pm_pixels + (self->pm_base_y + ar.y) * self->pm_row_bytes + (self->pm_base_x + ar.x) * bytes,
            self->pm_pixels_qualifier,
            ar.w, ar.h,
            dest->pm_row_bytes, self->pm_row_bytes,
            bytes);
        return 0;
    } else {
        int i;

        for (i = 0; i < ar.h; i++) {
            pm_mem_copy_colour(
                (br_uint_8*)dest->pm_pixels + (dest->pm_base_y + ap.y + i) * dest->pm_row_bytes + (dest->pm_base_x + ap.x) * bytes,
                dest->pm_pixels_qualifier,
                (br_uint_8*)self->pm_pixels + (self->pm_base_y + ar.y + i) * self->pm_row_bytes + (self->pm_base_x + ar.x) * bytes,
                self->pm_pixels_qualifier,
                ar.w,
                bytes);
        }
        return 0;
    }
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0x10]
            sub esp, 0x18
            lea edx, [esp]
            push esi
            mov esi, dword ptr [esp + 0x20]
            push edi
            mov edi, dword ptr [esp + 0x2c]
            push esi
            push edi
            push eax
            push ecx
            lea eax, [esp + 0x20]
            push edx
            push eax
            call PixelmapRectangleClipTwo
            add esp, 0x18
            test eax, eax
            jne L0053a425
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a425:
            xor ecx, ecx
            xor eax, eax
            mov cl, byte ptr [esi + 0x2c]
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            push ebp
            mov ax, word ptr [ecx*8 + pmTypeInfo]
            mov ecx, dword ptr [esp + 0x1c]
            shr eax, 3
            cmp ecx, edx
            push ebx
            mov dword ptr [esp + 0x2c], eax
            jne L0053a56c
            mov bp, word ptr [esi + 0x28]
            mov bx, word ptr [edi + 0x28]
            cmp bp, bx
            jne L0053a56c
            mov dl, byte ptr [esi + 0x2d]
            and dl, 6
            cmp dl, 6
            jne L0053a56c
            mov dl, byte ptr [edi + 0x2d]
            and dl, 6
            cmp dl, 6
            jne L0053a56c
            test bp, bp
            push eax
            jle L0053a4f4
            imul ecx, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            xor edx, edx
            push ecx
            mov dx, word ptr [esi + 0x30]
            mov ecx, dword ptr [esp + 0x24]
            add edx, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            imul edx, eax
            add ecx, dword ptr [esp + 0x28]
            movsx ebp, bp
            imul ecx, ebp
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            add edx, ecx
            mov ebp, dword ptr [esp + 0x20]
            push edx
            mov edx, dword ptr [edi + 0xc]
            xor ecx, ecx
            push edx
            mov cx, word ptr [edi + 0x30]
            mov edx, dword ptr [esp + 0x24]
            add ecx, edx
            xor edx, edx
            mov dx, word ptr [edi + 0x32]
            imul ecx, eax
            movsx eax, bx
            add edx, ebp
            mov esi, dword ptr [edi + 8]
            imul edx, eax
            add ecx, edx
            add ecx, esi
            push ecx
            call pm_mem_copy_colour
            add esp, 0x18
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a4f4:
            mov edx, dword ptr [esp + 0x28]
            imul ecx, edx
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            add ecx, dword ptr [esp + 0x28]
            movsx ebp, bp
            lea ecx, [ecx + edx - 1]
            imul ecx, ebp
            xor ebp, ebp
            mov bp, word ptr [esi + 0x30]
            add ebp, dword ptr [esp + 0x24]
            imul ebp, eax
            add ecx, ebp
            mov ebp, dword ptr [esi + 8]
            add ecx, ebp
            mov esi, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [edi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x32]
            mov ebp, dword ptr [esp + 0x24]
            add ecx, esi
            mov esi, dword ptr [edi + 8]
            lea edx, [ecx + edx - 1]
            movsx ecx, bx
            imul edx, ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x30]
            add ecx, ebp
            imul ecx, eax
            add edx, ecx
            add edx, esi
            push edx
            call pm_mem_copy_colour
            add esp, 0x18
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a56c:
            mov dx, word ptr [edi + 0x28]
            test dl, 7
            jne L0053a5e9
            movsx ebp, word ptr [esi + 0x28]
            mov ebx, dword ptr [esp + 0x24]
            push eax
            movsx edx, dx
            push ebp
            push edx
            push ebx
            mov ebx, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            add ecx, ebx
            xor ebx, ebx
            mov bx, word ptr [esi + 0x32]
            imul ecx, eax
            add ebx, dword ptr [esp + 0x34]
            imul ebx, ebp
            add ecx, ebx
            mov ebx, dword ptr [esi + 8]
            add ecx, ebx
            mov esi, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [edi + 0xc]
            push ecx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x30]
            mov ebp, dword ptr [esp + 0x34]
            add ecx, esi
            mov esi, dword ptr [edi + 8]
            imul ecx, eax
            xor eax, eax
            mov ax, word ptr [edi + 0x32]
            add eax, ebp
            imul eax, edx
            add ecx, eax
            add ecx, esi
            push ecx
            call pm_mem_copy_colour_rowbyrow
            add esp, 0x24
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x18
            ret
    L0053a5e9:
            mov ebp, dword ptr [esp + 0x10]
            xor ebx, ebx
            mov bx, word ptr [esi + 0x30]
            mov dword ptr [esp + 0x34], 0
            add ebx, ebp
            mov ebp, dword ptr [esp + 0x14]
            imul ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add eax, ebp
            movsx ebp, word ptr [esi + 0x28]
            imul eax, ebp
            mov ebp, dword ptr [esi + 8]
            add ebx, eax
            mov eax, dword ptr [esp + 0x18]
            add ebx, ebp
            xor ebp, ebp
            mov bp, word ptr [edi + 0x30]
            add ebp, eax
            xor eax, eax
            mov ax, word ptr [edi + 0x32]
            imul ebp, dword ptr [esp + 0x2c]
            add eax, dword ptr [esp + 0x1c]
            movsx edx, dx
            imul eax, edx
            mov edx, dword ptr [edi + 8]
            add ebp, eax
            mov eax, dword ptr [esp + 0x24]
            add ebp, edx
            test eax, eax
            jbe L0053a685
            jmp L0053a650
    L0053a64c:
            mov ecx, dword ptr [esp + 0x20]
    L0053a650:
            mov eax, dword ptr [esp + 0x2c]
            mov edx, dword ptr [edi + 0xc]
            push eax
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push ecx
            push ebx
            push edx
            push ebp
            call pm_mem_copy_colour
            movsx eax, word ptr [esi + 0x28]
            movsx ecx, word ptr [edi + 0x28]
            add ebx, eax
            mov eax, dword ptr [esp + 0x4c]
            add esp, 0x18
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x24]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x34], eax
            jb L0053a64c
    L0053a685:
            pop ebx
            pop ebp
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053a690
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleStretchCopyTo(br_device_pixelmap* self, br_rectangle* destinationRectangle, br_device_pixelmap* src, br_rectangle* sourceRectangle) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053a6a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleStretchCopyFrom(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0x1002;
}


// FUNCTION: CARMA2_HW 0x0053a6b0
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
#ifndef CARPOCALYPSE2_MATCHING
    br_rectangle arect;
    br_int_8 bytes;

    if (PixelmapRectangleClip(&arect, rect, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0;
    }

    bytes = pmTypeInfo[self->pm_type].bits >> 3;
    if (arect.w == self->pm_width
        && (self->pm_flags & (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) == (BR_PMF_LINEAR | BR_PMF_ROW_WHOLEPIXELS)) {
        if (self->pm_row_bytes > 0) {
            pm_mem_fill_colour(
                (br_uint_8*)self->pm_pixels + (self->pm_base_y + arect.y) * self->pm_row_bytes + (self->pm_base_x + arect.x) * bytes,
                self->pm_pixels_qualifier,
                arect.w * arect.h,
                bytes,
                colour);
            return 0;
        } else {
            pm_mem_fill_colour(
                (br_uint_8*)self->pm_pixels + (self->pm_base_y + (arect.y +  arect.h - 1)) * self->pm_row_bytes + (self->pm_base_x + arect.x) * bytes,
                self->pm_pixels_qualifier,
                arect.w * arect.h,
                bytes,
                colour);
            return 0;
        }
    }
    if ((self->pm_row_bytes & 7) == 0) {
        pm_mem_fill_colour_rect(
            (br_uint_8*)self->pm_pixels + (self->pm_base_y + arect.y) * self->pm_row_bytes + (self->pm_base_x + arect.x) * bytes,
            self->pm_pixels_qualifier,
            arect.w, arect.h,
            self->pm_row_bytes,
            bytes,
            colour);
            return 0;
    } else {
        br_uint_32 y;

        for (y = arect.y; y < (br_uint_32)(arect.y + arect.h); y++) {
            pm_mem_fill_colour(
                (br_uint_8*)self->pm_pixels + (self->pm_base_y + y) * self->pm_row_bytes + (self->pm_base_x + arect.x) * bytes,
                self->pm_pixels_qualifier,
                arect.w,
                bytes,
                colour);
        }
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            sub esp, 0x10
            lea ecx, [esp]
            push esi
            mov esi, dword ptr [esp + 0x18]
            push esi
            push eax
            push ecx
            call PixelmapRectangleClip
            add esp, 0xc
            test eax, eax
            jne L0053a6d4
            pop esi
            add esp, 0x10
            ret
    L0053a6d4:
            xor edx, edx
            mov eax, dword ptr [esp + 0xc]
            mov dl, byte ptr [esi + 0x2c]
            push edi
            push ebp
            push ebx
            mov cx, word ptr [edx*8 + pmTypeInfo]
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            shr ecx, 3
            cmp eax, edx
            jne L0053a7aa
            mov dl, byte ptr [esi + 0x2d]
            and dl, 6
            cmp dl, 6
            jne L0053a7aa
            mov dx, word ptr [esi + 0x28]
            mov edi, dword ptr [esp + 0x2c]
            movsx ecx, cl
            test dx, dx
            push edi
            push ecx
            jle L0053a75f
            imul eax, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x18]
            push eax
            mov eax, dword ptr [esi + 0xc]
            mov ebx, dword ptr [esp + 0x20]
            push eax
            xor eax, eax
            mov ax, word ptr [esi + 0x30]
            add eax, ebp
            imul eax, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            movsx edx, dx
            add ecx, ebx
            imul ecx, edx
            mov edx, dword ptr [esi + 8]
            add eax, ecx
            add eax, edx
            push eax
            call pm_mem_fill_colour
            add esp, 0x14
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x10
            ret
    L0053a75f:
            mov edi, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x1c]
            imul eax, edi
            push eax
            mov eax, dword ptr [esi + 0xc]
            push eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ebx, dword ptr [esp + 0x20]
            add eax, ebp
            movsx edx, dx
            lea eax, [eax + edi - 1]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add edx, ebx
            imul edx, ecx
            add eax, edx
            mov edx, dword ptr [esi + 8]
            add eax, edx
            push eax
            call pm_mem_fill_colour
            add esp, 0x14
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x10
            ret
    L0053a7aa:
            mov dx, word ptr [esi + 0x28]
            test dl, 7
            jne L0053a802
            mov edi, dword ptr [esp + 0x2c]
            mov ebp, dword ptr [esp + 0x10]
            movsx ecx, cl
            movsx edx, dx
            push edi
            mov edi, dword ptr [esp + 0x20]
            push ecx
            push edx
            push edi
            push eax
            mov eax, dword ptr [esi + 0xc]
            mov ebx, dword ptr [esp + 0x28]
            push eax
            xor eax, eax
            mov ax, word ptr [esi + 0x30]
            add eax, ebp
            imul eax, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            add ecx, ebx
            imul ecx, edx
            mov edx, dword ptr [esi + 8]
            add eax, ecx
            add eax, edx
            push eax
            call pm_mem_fill_colour_rect
            add esp, 0x1c
            xor eax, eax
            pop ebx
            pop ebp
            pop edi
            pop esi
            add esp, 0x10
            ret
    L0053a802:
            mov edi, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x1c]
            add edx, edi
            cmp edi, edx
            jae L0053a863
            mov ebp, dword ptr [esp + 0x2c]
            movsx ebx, cl
            jmp L0053a81d
    L0053a819:
            mov eax, dword ptr [esp + 0x18]
    L0053a81d:
            mov edx, dword ptr [esp + 0x10]
            push ebp
            push ebx
            push eax
            mov eax, dword ptr [esi + 0xc]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            push eax
            xor eax, eax
            add edx, ecx
            mov ax, word ptr [esi + 0x32]
            mov ecx, edi
            add ecx, eax
            imul edx, ebx
            movsx eax, word ptr [esi + 0x28]
            imul ecx, eax
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            add edx, ecx
            push edx
            call pm_mem_fill_colour
            mov ecx, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x28]
            add esp, 0x14
            inc edi
            add edx, ecx
            cmp edi, edx
            jb L0053a819
    L0053a863:
            pop ebx
            pop ebp
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053a870
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelSet(br_device_pixelmap* self, br_point* p, br_uint_32 colour) {
    br_point ap;
    br_int_8 bytes;

    if (PixelmapPointClip(&ap, p, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0;
    }
    bytes = pmTypeInfo[self->pm_type].bits / 8;
    pm_mem_set_colour((br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y) * self->pm_row_bytes + (self->pm_base_x + ap.x) * bytes, self->pm_pixels_qualifier, bytes, colour);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053a8f0
br_error C2_NAKED C2_HOOK_CDECL _M_br_device_pixelmap_mem_line(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour) {
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
    char* dest;
    int bytes;
    br_point as;
    br_point ae;
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
    int bytes;
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

    bytes = pmTypeInfo[self->pm_type].bits / 8;

    for (;;) {
        pm_mem_set_colour(
            (br_uint_8*)self->pm_pixels + (self->pm_base_y + p.y) * self->pm_row_bytes + (self->pm_base_x + p.x) * bytes,
            self->pm_pixels_qualifier,
            bytes,
            colour);
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
            sub esp, 0x2c
            mov ecx, dword ptr [esp + 0x34]
            lea edx, [esp + 0x1c]
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x3c]
            push edi
            push esi
            push eax
            push ecx
            lea eax, [esp + 0x40]
            push edx
            push eax
            call PixelmapLineClip
            add esp, 0x14
            test eax, eax
            je L0053bcb5
            mov edx, dword ptr [esp + 0x2c]
            mov ebx, dword ptr [esp + 0x34]
            xor ecx, ecx
            xor edi, edi
            mov cl, byte ptr [esi + 0x2c]
            mov ebp, dword ptr [esp + 0x30]
            mov di, word ptr [ecx*8 + pmTypeInfo]
            mov ecx, 0
            shr edi, 3
            sub edx, ebx
            setns cl
            dec ecx
            and ecx, 0xfffffffe
            inc ecx
            mov eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x18], eax
            sub ebp, edx
            mov edx, 0
            setns dl
            dec edx
            and edx, 0xfffffffe
            inc edx
            mov dword ptr [esp + 0x40], edx
            imul edx, ebp
            test eax, eax
            mov dword ptr [esp + 0x20], edx
            jne L0053a9be
            test edx, edx
            jne L0053a9be
            mov edx, dword ptr [esp + 0x4c]
            mov eax, dword ptr [esi + 0xc]
            push edx
            push edi
            movsx edx, word ptr [esi + 0x28]
            xor ecx, ecx
            push eax
            mov cx, word ptr [esi + 0x32]
            mov eax, dword ptr [esp + 0x44]
            add ecx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x30]
            imul ecx, edx
            add eax, ebx
            imul eax, edi
            mov edi, dword ptr [esi + 8]
            add ecx, eax
            add ecx, edi
            push ecx
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053a9be:
            sub ecx, dword ptr [esp + 0x40]
            neg ecx
            sbb ecx, ecx
            and ecx, 0xfffffffe
            inc ecx
            cmp edx, eax
            mov dword ptr [esp + 0x10], ecx
            jle L0053aa12
            mov ecx, dword ptr [esp + 0x38]
            mov ebp, dword ptr [esp + 0x2c]
            xor ebx, ecx
            xor eax, edx
            xor ecx, ebx
            xor edx, eax
            mov dword ptr [esp + 0x38], ecx
            xor ebx, ecx
            mov ecx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x34], ebx
            xor ebp, ecx
            mov dword ptr [esp + 0x20], edx
            xor ecx, ebp
            mov dword ptr [esp + 0x44], 1
            xor ebp, ecx
            xor eax, edx
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x2c], ebp
            mov dword ptr [esp + 0x18], eax
            jmp L0053aa1e
    L0053aa12:
            mov ebp, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x44], 0
    L0053aa1e:
            cmp ebx, ebp
            jle L0053aa38
            mov ecx, dword ptr [esp + 0x2c]
            mov ebp, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x48], ebx
            mov ebx, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x40], ecx
            jmp L0053aa4c
    L0053aa38:
            mov edx, dword ptr [esp + 0x2c]
            mov ebp, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x40], ebx
            mov ebx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x48], edx
    L0053aa4c:
            lea ecx, [eax - 1]
            mov eax, ecx
            cdq
            and edx, 3
            add eax, edx
            sar eax, 2
            mov dword ptr [esp + 0x24], eax
            mov eax, ecx
            cdq
            xor eax, edx
            sub eax, edx
            and eax, 3
            xor eax, edx
            sub eax, edx
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053aa97
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x40]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebp
            mov cx, word ptr [esi + 0x30]
            jmp L0053aab4
    L0053aa97:
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebp
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x40]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053aab4:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053aafa
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053ab17
    L0053aafa:
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053ab17:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x28]
            add eax, eax
            add esp, 0x10
            mov dword ptr [esp + 0x1c], eax
            sub eax, edx
            shl eax, 1
            mov dword ptr [esp + 0x28], eax
            jns L0053b22f
            mov ecx, dword ptr [esp + 0x1c]
            lea eax, [ecx + ecx]
            mov ecx, edx
            mov dword ptr [esp + 0x20], eax
            sub eax, ecx
            mov ecx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x18], eax
            test ecx, ecx
            jle L0053aefe
            mov dword ptr [esp + 0x24], ecx
    L0053ab75:
            mov ecx, dword ptr [esp + 0x40]
            mov edx, dword ptr [esp + 0x48]
            inc ecx
            dec edx
            test eax, eax
            mov dword ptr [esp + 0x40], ecx
            mov dword ptr [esp + 0x48], edx
            jge L0053ad38
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053abb2
            xor edx, edx
            mov eax, ecx
            mov dx, word ptr [esi + 0x32]
            movsx ecx, word ptr [esi + 0x28]
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053abc9
    L0053abb2:
            xor edx, edx
            mov eax, ebp
            mov dx, word ptr [esi + 0x32]
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053abc9:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053ac19
            mov eax, dword ptr [esp + 0x40]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            movsx edx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053ac42
    L0053ac19:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x40]
            mov ax, word ptr [esi + 0x32]
            inc ecx
            mov edx, eax
            mov eax, ebp
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053ac42:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053ac81
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053ac9e
    L0053ac81:
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053ac9e:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053acee
            mov eax, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            dec eax
            movsx edx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x48], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebx
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053ad17
    L0053acee:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x48]
            mov ax, word ptr [esi + 0x32]
            dec ecx
            mov edx, eax
            mov eax, ebx
            add eax, edx
            mov dword ptr [esp + 0x48], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053ad17:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x30]
            add esp, 0x10
            jmp L0053aee3
    L0053ad38:
            mov edx, dword ptr [esp + 0x1c]
            cmp eax, edx
            mov eax, dword ptr [esp + 0x44]
            jge L0053bcbf
            test eax, eax
            je L0053ad67
            xor edx, edx
            mov eax, ecx
            mov dx, word ptr [esi + 0x32]
            movsx ecx, word ptr [esi + 0x28]
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053ad7e
    L0053ad67:
            xor edx, edx
            mov eax, ebp
            mov dx, word ptr [esi + 0x32]
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053ad7e:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053add4
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x40]
            movsx edx, word ptr [esi + 0x28]
            add ebp, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053ae03
    L0053add4:
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x40]
            mov edx, eax
            mov eax, ebp
            inc ecx
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053ae03:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053ae42
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053ae5f
    L0053ae42:
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053ae5f:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053ae9a
            mov eax, dword ptr [esp + 0x48]
            mov ecx, dword ptr [esp + 0x10]
            dec eax
            sub ebx, ecx
            mov dword ptr [esp + 0x48], eax
            jmp L0053be14
    L0053ae9a:
            sub ebx, dword ptr [esp + 0x10]
    L0053ae9e:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x48]
            mov ax, word ptr [esi + 0x32]
            dec ecx
            mov edx, eax
            mov eax, ebx
            add eax, edx
            mov dword ptr [esp + 0x48], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053aec7:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x38]
            add esp, 0x10
    L0053aee3:
            mov ecx, dword ptr [esp + 0x18]
            add ecx, eax
            mov eax, dword ptr [esp + 0x24]
            dec eax
            mov dword ptr [esp + 0x18], ecx
            mov dword ptr [esp + 0x24], eax
            mov eax, ecx
            jne L0053ab75
    L0053aefe:
            mov ecx, dword ptr [esp + 0x14]
            test ecx, ecx
            je L0053bcb5
            test eax, eax
            jge L0053b071
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053af43
            mov eax, dword ptr [esp + 0x40]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            movsx edx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053af6c
    L0053af43:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x40]
            mov ax, word ptr [esi + 0x32]
            inc ecx
            mov edx, eax
            mov eax, ebp
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053af6c:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x24]
            add esp, 0x10
            cmp eax, 1
            jle L0053affa
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053afbe
            mov ecx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            movsx edx, word ptr [esi + 0x28]
            lea eax, [ecx + eax + 1]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            add ebp, ecx
            mov ecx, dword ptr [esi + 8]
            imul ebp, edi
            add eax, ebp
            add eax, ecx
            jmp L0053afe7
    L0053afbe:
            movsx eax, word ptr [esi + 0x28]
            xor edx, edx
            xor ecx, ecx
            mov dx, word ptr [esi + 0x32]
            mov cx, word ptr [esi + 0x30]
            add ebp, edx
            mov edx, dword ptr [esp + 0x40]
            imul ebp, eax
            lea eax, [edx + ecx + 1]
            imul eax, edi
            add ebp, eax
            mov eax, dword ptr [esi + 8]
            add ebp, eax
            mov eax, ebp
    L0053afe7:
            mov ecx, dword ptr [esp + 0x4c]
            mov edx, dword ptr [esi + 0xc]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
    L0053affa:
            cmp dword ptr [esp + 0x14], 2
            jle L0053bcb5
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b03e
            mov ecx, dword ptr [esp + 0x48]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            movsx edx, word ptr [esi + 0x28]
            lea eax, [ecx + eax - 1]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            add ebx, ecx
            mov ecx, dword ptr [esi + 8]
            imul ebx, edi
            add eax, ebx
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            jmp L0053bca7
    L0053b03e:
            movsx eax, word ptr [esi + 0x28]
            xor edx, edx
            xor ecx, ecx
            mov dx, word ptr [esi + 0x32]
            mov cx, word ptr [esi + 0x30]
            add ebx, edx
            mov edx, dword ptr [esp + 0x48]
            imul ebx, eax
            lea eax, [edx + ecx - 1]
            mov ecx, dword ptr [esp + 0x4c]
            imul eax, edi
            add ebx, eax
            mov eax, dword ptr [esi + 8]
            add ebx, eax
            push ecx
            mov eax, ebx
            jmp L0053bca7
    L0053b071:
            mov ecx, dword ptr [esp + 0x1c]
            cmp eax, ecx
            mov eax, dword ptr [esp + 0x44]
            jge L0053b0c3
            test eax, eax
            je L0053b0ad
            mov eax, dword ptr [esp + 0x40]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            movsx edx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b983
    L0053b0ad:
            mov ecx, dword ptr [esp + 0x40]
            inc ecx
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov dword ptr [esp + 0x40], ecx
            mov edx, eax
            jmp L0053b96b
    L0053b0c3:
            test eax, eax
            je L0053b0ef
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x40]
            movsx ecx, word ptr [esi + 0x28]
            add ebp, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            inc eax
            mov dword ptr [esp + 0x40], eax
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053b115
    L0053b0ef:
            mov ecx, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x40]
            mov eax, ebp
            add eax, edx
            inc ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dword ptr [esp + 0x40], ecx
            mov dx, word ptr [esi + 0x30]
    L0053b115:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x24]
            add esp, 0x10
            cmp eax, 1
            jle L0053b1aa
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b16e
            mov ecx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            movsx edx, word ptr [esi + 0x28]
            lea eax, [ecx + eax + 1]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            add ebp, ecx
            mov ecx, dword ptr [esi + 8]
            imul ebp, edi
            add eax, ebp
            add eax, ecx
            jmp L0053b197
    L0053b16e:
            movsx eax, word ptr [esi + 0x28]
            xor edx, edx
            xor ecx, ecx
            mov dx, word ptr [esi + 0x32]
            mov cx, word ptr [esi + 0x30]
            add ebp, edx
            mov edx, dword ptr [esp + 0x40]
            imul ebp, eax
            lea eax, [edx + ecx + 1]
            imul eax, edi
            add ebp, eax
            mov eax, dword ptr [esi + 8]
            add ebp, eax
            mov eax, ebp
    L0053b197:
            mov ecx, dword ptr [esp + 0x4c]
            mov edx, dword ptr [esi + 0xc]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
    L0053b1aa:
            cmp dword ptr [esp + 0x14], 2
            jle L0053bcb5
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b1e5
            mov ebp, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x48]
            xor eax, eax
            xor ecx, ecx
            mov ax, word ptr [esi + 0x30]
            mov cx, word ptr [esi + 0x32]
            sub ebx, ebp
            add ebx, eax
            lea eax, [edx + ecx - 1]
            movsx ecx, word ptr [esi + 0x28]
            imul ebx, edi
            imul eax, ecx
            jmp L0053b20b
    L0053b1e5:
            mov eax, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            sub ebx, eax
            movsx eax, word ptr [esi + 0x28]
            xor ecx, ecx
            add ebx, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, dword ptr [esp + 0x48]
            imul ebx, eax
            lea eax, [edx + ecx - 1]
            imul eax, edi
    L0053b20b:
            mov ecx, dword ptr [esp + 0x4c]
            mov edx, dword ptr [esi + 0xc]
            add ebx, eax
            mov eax, dword ptr [esi + 8]
            push ecx
            add ebx, eax
            push edi
            push edx
            push ebx
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053b22f:
            mov eax, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x18]
            sub eax, edx
            shl eax, 1
            mov ecx, eax
            mov dword ptr [esp + 0x1c], ecx
            lea eax, [ecx + ecx]
            mov dword ptr [esp + 0x20], eax
            add eax, edx
            mov edx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x18], eax
            test edx, edx
            jle L0053b797
            mov dword ptr [esp + 0x24], edx
            jmp L0053b264
    L0053b260:
            mov eax, dword ptr [esp + 0x18]
    L0053b264:
            mov ecx, dword ptr [esp + 0x40]
            mov edx, dword ptr [esp + 0x48]
            inc ecx
            dec edx
            test eax, eax
            mov dword ptr [esp + 0x40], ecx
            mov dword ptr [esp + 0x48], edx
            jle L0053b455
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b2a9
            mov edx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, edx
            mov edx, eax
            mov eax, ecx
            movsx ecx, word ptr [esi + 0x28]
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053b2c6
    L0053b2a9:
            mov eax, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            add ebp, eax
            mov eax, ebp
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053b2c6:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b31c
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x40]
            movsx edx, word ptr [esi + 0x28]
            add ebp, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b34b
    L0053b31c:
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x40]
            mov edx, eax
            mov eax, ebp
            inc ecx
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b34b:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            mov eax, dword ptr [esp + 0x10]
            je L0053b390
            movsx edx, word ptr [esi + 0x28]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053b3af
    L0053b390:
            movsx edx, word ptr [esi + 0x28]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053b3af:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b405
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x48]
            movsx edx, word ptr [esi + 0x28]
            sub ebx, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            dec eax
            mov dword ptr [esp + 0x48], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebx
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b434
    L0053b405:
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            sub ebx, ecx
            mov ecx, dword ptr [esp + 0x48]
            mov edx, eax
            mov eax, ebx
            dec ecx
            add eax, edx
            mov dword ptr [esp + 0x48], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b434:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x30]
            add esp, 0x10
            jmp L0053b776
    L0053b455:
            mov edx, dword ptr [esp + 0x1c]
            cmp eax, edx
            mov eax, dword ptr [esp + 0x44]
            jge L0053b5c0
            test eax, eax
            je L0053b484
            xor edx, edx
            mov eax, ecx
            mov dx, word ptr [esi + 0x32]
            movsx ecx, word ptr [esi + 0x28]
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053b49b
    L0053b484:
            xor edx, edx
            mov eax, ebp
            mov dx, word ptr [esi + 0x32]
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053b49b:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b4f1
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x40]
            movsx edx, word ptr [esi + 0x28]
            add ebp, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b520
    L0053b4f1:
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x40]
            mov edx, eax
            mov eax, ebp
            inc ecx
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b520:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b55f
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053b57c
    L0053b55f:
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053b57c:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b5b7
            mov eax, dword ptr [esp + 0x48]
            mov ecx, dword ptr [esp + 0x10]
            dec eax
            sub ebx, ecx
            mov dword ptr [esp + 0x48], eax
            jmp L0053b711
    L0053b5b7:
            sub ebx, dword ptr [esp + 0x10]
            jmp L0053b731
    L0053b5c0:
            test eax, eax
            je L0053b5e7
            mov edx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, edx
            mov edx, eax
            mov eax, ecx
            movsx ecx, word ptr [esi + 0x28]
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053b604
    L0053b5e7:
            mov eax, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            add ebp, eax
            mov eax, ebp
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053b604:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b654
            mov eax, dword ptr [esp + 0x40]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            movsx edx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b67d
    L0053b654:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x40]
            mov ax, word ptr [esi + 0x32]
            inc ecx
            mov edx, eax
            mov eax, ebp
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b67d:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            mov eax, dword ptr [esp + 0x10]
            je L0053b6c2
            movsx edx, word ptr [esi + 0x28]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053b6e1
    L0053b6c2:
            movsx edx, word ptr [esi + 0x28]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053b6e1:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053b731
            mov eax, dword ptr [esp + 0x48]
            dec eax
            mov dword ptr [esp + 0x48], eax
    L0053b711:
            movsx edx, word ptr [esi + 0x28]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            add eax, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            mov edx, ebx
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b75a
    L0053b731:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x48]
            mov ax, word ptr [esi + 0x32]
            dec ecx
            mov edx, eax
            mov eax, ebx
            add eax, edx
            mov dword ptr [esp + 0x48], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b75a:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x38]
            add esp, 0x10
    L0053b776:
            mov ecx, dword ptr [esp + 0x18]
            add ecx, eax
            mov eax, dword ptr [esp + 0x24]
            dec eax
            mov dword ptr [esp + 0x18], ecx
            mov dword ptr [esp + 0x24], eax
            jne L0053b260
            mov ecx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x18]
    L0053b797:
            mov edx, dword ptr [esp + 0x14]
            test edx, edx
            je L0053bcb5
            test eax, eax
            jle L0053b92a
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b7e2
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x40]
            movsx edx, word ptr [esi + 0x28]
            add ebp, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053b811
    L0053b7e2:
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x40]
            mov edx, eax
            mov eax, ebp
            inc ecx
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b811:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x24]
            add esp, 0x10
            cmp eax, 1
            jle L0053b8a5
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b864
            mov eax, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            add eax, ebp
            xor edx, edx
            add eax, ecx
            mov dx, word ptr [esi + 0x32]
            mov ecx, dword ptr [esp + 0x40]
            imul eax, edi
            lea edx, [ecx + edx + 1]
            movsx ecx, word ptr [esi + 0x28]
            imul edx, ecx
            add eax, edx
            jmp L0053b88d
    L0053b864:
            mov edx, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            lea eax, [edx + ebp]
            movsx edx, word ptr [esi + 0x28]
            add eax, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            mov edx, dword ptr [esp + 0x40]
            lea ecx, [edx + ecx + 1]
            imul ecx, edi
            add eax, ecx
    L0053b88d:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esp + 0x4c]
            add eax, ecx
            mov ecx, dword ptr [esi + 0xc]
            push edx
            push edi
            push ecx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
    L0053b8a5:
            cmp dword ptr [esp + 0x14], 2
            jle L0053bcb5
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b8e0
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x48]
            sub ebx, eax
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebx, edx
            imul ebx, edi
            lea edx, [ecx + eax - 1]
            movsx eax, word ptr [esi + 0x28]
            imul edx, eax
            jmp L0053b906
    L0053b8e0:
            mov ebp, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            sub ebx, ebp
            movsx edx, word ptr [esi + 0x28]
            xor eax, eax
            add ebx, ecx
            mov ax, word ptr [esi + 0x30]
            mov ecx, dword ptr [esp + 0x48]
            imul ebx, edx
            lea edx, [ecx + eax - 1]
            imul edx, edi
    L0053b906:
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [esi + 0xc]
            add ebx, edx
            add ebx, eax
            mov eax, dword ptr [esp + 0x4c]
            push eax
            push edi
            push ecx
            push ebx
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053b92a:
            cmp eax, ecx
            mov eax, dword ptr [esp + 0x44]
            jge L0053bab4
            test eax, eax
            je L0053b95c
            mov eax, dword ptr [esp + 0x40]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            inc eax
            movsx ecx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x40], eax
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053b97c
    L0053b95c:
            mov ecx, dword ptr [esp + 0x40]
            inc ecx
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            mov dword ptr [esp + 0x40], ecx
    L0053b96b:
            mov eax, ebp
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053b97c:
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053b983:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x24]
            add esp, 0x10
            cmp eax, 1
            jle L0053ba17
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053b9d6
            mov eax, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            add eax, ebp
            xor edx, edx
            add eax, ecx
            mov dx, word ptr [esi + 0x32]
            mov ecx, dword ptr [esp + 0x40]
            imul eax, edi
            lea edx, [ecx + edx + 1]
            movsx ecx, word ptr [esi + 0x28]
            imul edx, ecx
            add eax, edx
            jmp L0053b9ff
    L0053b9d6:
            mov edx, dword ptr [esp + 0x10]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            lea eax, [edx + ebp]
            movsx edx, word ptr [esi + 0x28]
            add eax, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            mov edx, dword ptr [esp + 0x40]
            lea ecx, [edx + ecx + 1]
            imul ecx, edi
            add eax, ecx
    L0053b9ff:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esp + 0x4c]
            add eax, ecx
            mov ecx, dword ptr [esi + 0xc]
            push edx
            push edi
            push ecx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
    L0053ba17:
            cmp dword ptr [esp + 0x14], 2
            jle L0053bcb5
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053ba6e
            mov eax, dword ptr [esp + 0x48]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            movsx ecx, word ptr [esi + 0x28]
            lea eax, [eax + edx - 1]
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            imul eax, ecx
            add ebx, edx
            mov ecx, dword ptr [esi + 8]
            imul ebx, edi
            mov edx, dword ptr [esp + 0x4c]
            add eax, ebx
            add eax, ecx
            mov ecx, dword ptr [esi + 0xc]
            push edx
            push edi
            push ecx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053ba6e:
            movsx ecx, word ptr [esi + 0x28]
            xor eax, eax
            xor edx, edx
            mov ax, word ptr [esi + 0x32]
            mov dx, word ptr [esi + 0x30]
            add ebx, eax
            mov eax, dword ptr [esp + 0x48]
            imul ebx, ecx
            lea ecx, [eax + edx - 1]
            mov eax, dword ptr [esi + 8]
            imul ecx, edi
            mov edx, dword ptr [esp + 0x4c]
            add ebx, ecx
            mov ecx, dword ptr [esi + 0xc]
            add ebx, eax
            push edx
            mov eax, ebx
            push edi
            push ecx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053bab4:
            test eax, eax
            je L0053bae0
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x40]
            movsx ecx, word ptr [esi + 0x28]
            add ebp, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            inc eax
            mov dword ptr [esp + 0x40], eax
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053bb06
    L0053bae0:
            mov ecx, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x40]
            mov eax, ebp
            add eax, edx
            inc ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dword ptr [esp + 0x40], ecx
            mov dx, word ptr [esi + 0x30]
    L0053bb06:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x24]
            add esp, 0x10
            cmp eax, 1
            jle L0053bb9d
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L0053bb5f
            mov ecx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            movsx edx, word ptr [esi + 0x28]
            lea eax, [ecx + eax + 1]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            add ebp, ecx
            mov ecx, dword ptr [esi + 8]
            imul ebp, edi
            add eax, ebp
            add eax, ecx
            jmp L0053bb88
    L0053bb5f:
            movsx eax, word ptr [esi + 0x28]
            xor edx, edx
            xor ecx, ecx
            mov dx, word ptr [esi + 0x32]
            mov cx, word ptr [esi + 0x30]
            add ebp, edx
            mov edx, dword ptr [esp + 0x40]
            imul ebp, eax
            lea eax, [edx + ecx + 1]
            imul eax, edi
            add ebp, eax
            mov eax, dword ptr [esi + 8]
            add ebp, eax
            mov eax, ebp
    L0053bb88:
            mov ebp, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esi + 0xc]
            push ebp
            push edi
            push ecx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
            jmp L0053bba1
    L0053bb9d:
            mov ebp, dword ptr [esp + 0x4c]
    L0053bba1:
            cmp dword ptr [esp + 0x14], 2
            jle L0053bcb5
            mov edx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x18]
            cmp eax, edx
            mov eax, dword ptr [esp + 0x44]
            jle L0053bc50
            test eax, eax
            je L0053bc0a
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x48]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x30]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            add ebx, eax
            imul ebx, edi
            lea eax, [edx + ecx - 1]
            push ebp
            movsx ecx, word ptr [esi + 0x28]
            imul eax, ecx
            mov ecx, dword ptr [esi + 0xc]
            add ebx, eax
            mov eax, dword ptr [esi + 8]
            push edi
            add ebx, eax
            push ecx
            push ebx
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053bc0a:
            mov ecx, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            sub ebx, ecx
            movsx eax, word ptr [esi + 0x28]
            xor ecx, ecx
            add ebx, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, dword ptr [esp + 0x48]
            imul ebx, eax
            lea eax, [edx + ecx - 1]
            mov ecx, dword ptr [esi + 0xc]
            imul eax, edi
            add ebx, eax
            mov eax, dword ptr [esi + 8]
            push ebp
            add ebx, eax
            push edi
            push ecx
            push ebx
            call pm_mem_set_colour
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0053bc50:
            test eax, eax
            je L0053bc7d
            mov eax, dword ptr [esp + 0x48]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            movsx ecx, word ptr [esi + 0x28]
            lea eax, [eax + edx - 1]
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            imul eax, ecx
            add ebx, edx
            mov ecx, dword ptr [esi + 8]
            imul ebx, edi
            add eax, ebx
            add eax, ecx
            jmp L0053bca6
    L0053bc7d:
            movsx ecx, word ptr [esi + 0x28]
            xor eax, eax
            xor edx, edx
            mov ax, word ptr [esi + 0x32]
            mov dx, word ptr [esi + 0x30]
            add ebx, eax
            mov eax, dword ptr [esp + 0x48]
            imul ebx, ecx
            lea ecx, [eax + edx - 1]
            mov eax, dword ptr [esi + 8]
            imul ecx, edi
            add ebx, ecx
            add ebx, eax
            mov eax, ebx
    L0053bca6:
            push ebp
    L0053bca7:
            mov edx, dword ptr [esi + 0xc]
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            add esp, 0x10
    L0053bcb5:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x2c
            ret
    L0053bcbf:
            test eax, eax
            je L0053bce6
            mov edx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            add ebp, edx
            mov edx, eax
            mov eax, ecx
            movsx ecx, word ptr [esi + 0x28]
            add eax, edx
            imul eax, ecx
            xor edx, edx
            mov ecx, ebp
            mov dx, word ptr [esi + 0x30]
            jmp L0053bd03
    L0053bce6:
            mov eax, dword ptr [esp + 0x10]
            xor edx, edx
            mov dx, word ptr [esi + 0x32]
            add ebp, eax
            mov eax, ebp
            add eax, edx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
    L0053bd03:
            add ecx, edx
            mov edx, dword ptr [esi + 0xc]
            imul ecx, edi
            add eax, ecx
            mov ecx, dword ptr [esi + 8]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053bd53
            mov eax, dword ptr [esp + 0x40]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            inc eax
            movsx edx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x40], eax
            add eax, ecx
            xor ecx, ecx
            imul eax, edx
            mov cx, word ptr [esi + 0x30]
            mov edx, ebp
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053bd7c
    L0053bd53:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x40]
            mov ax, word ptr [esi + 0x32]
            inc ecx
            mov edx, eax
            mov eax, ebp
            add eax, edx
            mov dword ptr [esp + 0x40], ecx
            movsx edx, word ptr [esi + 0x28]
            imul eax, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x30]
            add ecx, edx
            imul ecx, edi
            add eax, ecx
    L0053bd7c:
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            mov eax, dword ptr [esp + 0x10]
            je L0053bdc1
            movsx edx, word ptr [esi + 0x28]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x48]
            add eax, ecx
            imul eax, edx
            xor ecx, ecx
            mov edx, ebx
            mov cx, word ptr [esi + 0x30]
            jmp L0053bde0
    L0053bdc1:
            movsx edx, word ptr [esi + 0x28]
            sub ebx, eax
            xor eax, eax
            mov ax, word ptr [esi + 0x32]
            mov ecx, eax
            mov eax, ebx
            add eax, ecx
            imul eax, edx
            mov edx, dword ptr [esp + 0x48]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
    L0053bde0:
            add edx, ecx
            mov ecx, dword ptr [esi + 8]
            imul edx, edi
            add eax, edx
            mov edx, dword ptr [esi + 0xc]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x4c]
            push ecx
            push edi
            push edx
            push eax
            call pm_mem_set_colour
            mov eax, dword ptr [esp + 0x54]
            add esp, 0x10
            test eax, eax
            je L0053ae9e
            mov eax, dword ptr [esp + 0x48]
            dec eax
            mov dword ptr [esp + 0x48], eax
    L0053be14:
            movsx edx, word ptr [esi + 0x28]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x32]
            add eax, ecx
            xor ecx, ecx
            mov cx, word ptr [esi + 0x30]
            imul eax, edx
            mov edx, ebx
            add edx, ecx
            imul edx, edi
            add eax, edx
            jmp L0053aec7
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053be40
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
    int bytes;
    int bit;
    br_rectangle ar;
    br_point ap;

    if (PixelmapCopyBitsClip(&ar, &ap, bit_rect, point, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0;
    }

    bytes = pmTypeInfo[self->pm_type].bits / 8;
    bit = ar.x & 7;

    pm_mem_copy_bits(
        (br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y) * self->pm_row_bytes + (self->pm_base_x + ap.x + (ar.x & ~7) - bit) * bytes,
        self->pm_pixels_qualifier,
        self->pm_row_bytes,
        src + ar.y * s_stride,
        s_stride,
        bit,
        bit + ar.w,
        ar.h,
        bytes,
        colour);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053bf00
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelQuery(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p) {
    br_point ap;
    br_int_8 bytes;

    if (PixelmapPointClip(&ap, p, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0x1002;
    }
    bytes = pmTypeInfo[self->pm_type].bits / 8;
    *pcolour = pm_mem_read_colour((br_uint_8*)self->pm_pixels + (self->pm_base_y + ap.y) * self->pm_row_bytes + (self->pm_base_x + ap.x) * bytes, self->pm_pixels_qualifier, bytes);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053bf80
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelAddressQuery(br_device_pixelmap* self, void** pptr, br_uint_32* pqual, br_point* p) {
    br_int_8 bytes;
    br_point ap;

    if (PixelmapPointClip(&ap, p, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0x1002;
    }
    bytes = pmTypeInfo[self->pm_type].bits >> 3;
    if (pptr != NULL) {
        *pptr = (br_uint_8*)self->pm_pixels
            + (self->pm_base_y + ap.y) * self->pm_row_bytes
            + (self->pm_base_x + ap.x) * bytes;
    }
    if (pqual != NULL) {
        *pqual = self->pm_pixels_qualifier;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c010
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelAddressSet(br_device_pixelmap* self, void* ptr, br_uint_32* qual) {

    if (ptr != NULL) {
        self->pm_pixels = ptr;
    }
    if (qual != NULL) {
        self->pm_pixels_qualifier = *qual;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c030
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_originSet(br_device_pixelmap* self, br_point* p) {

    self->pm_origin_x = p->x;
    self->pm_origin_y = p->y;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c050
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rowSize(br_device_pixelmap* self, br_size_t* sizep) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c060
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rowQuery(br_device_pixelmap* self, void* buffer, br_size_t buffer_size, br_int_32 row) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c070
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rowSet(br_device_pixelmap* self, void* buffer, br_size_t buffer_size, br_int_32 row) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c080
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_flush(br_device_pixelmap* self) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c090
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_synchronise(br_device_pixelmap* self, br_token sync_type, br_boolean block) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c0a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_directLock(br_device_pixelmap* self, br_boolean block) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c0b0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_directUnlock(br_device_pixelmap* self) {

    return 0;
}
