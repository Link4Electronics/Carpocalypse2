/*
 * BRender 1.4 basics used by the ported drivers (drivers/commonrend, drivers/glrend)
 * that our decompiled core does not define. Port build only.
 */
#ifndef _BRENDER14_COMPAT_H_
#define _BRENDER14_COMPAT_H_

#include <stdint.h>

/* brerror.h */
#ifndef BRE_OK
#define BRE_OK 0x0
#endif
#ifndef BR_ERROR_MAKE
#define BR_ERROR_MAKE(klass, sub) ((klass) | (sub))
#endif
#ifndef BR_ECLASS_NONE
#define BR_ECLASS_NONE        0x0000
#endif
#ifndef BR_ECLASS_MISC
#define BR_ECLASS_MISC        0x1000
#endif
#ifndef BR_ECLASS_APPLICATION
#define BR_ECLASS_APPLICATION 0x2000
#endif
#ifndef BR_ECLASS_DATABASE
#define BR_ECLASS_DATABASE    0x3000
#endif
#ifndef BR_ECLASS_RENDERER
#define BR_ECLASS_RENDERER    0x4000
#endif
#ifndef BR_ECLASS_FILE
#define BR_ECLASS_FILE        0x5000
#endif
#ifndef BR_ECLASS_TOKEN
#define BR_ECLASS_TOKEN       0x6000
#endif
#ifndef BR_ECLASS_MATH
#define BR_ECLASS_MATH        0x7000
#endif
#ifndef BR_ECLASS_DOSIO
#define BR_ECLASS_DOSIO       0x8000
#endif
#ifndef BR_ECLASS_HOST
#define BR_ECLASS_HOST        0x9000
#endif
#ifndef BR_ECLASS_DEVICE
#define BR_ECLASS_DEVICE      0xA000
#endif
#ifndef BRE_UNKNOWN
#define BRE_UNKNOWN         BR_ERROR_MAKE(BR_ECLASS_MISC, 1)
#endif
#ifndef BRE_FAIL
#define BRE_FAIL            BR_ERROR_MAKE(BR_ECLASS_MISC, 2)
#endif
#ifndef BRE_NO_MEMORY
#define BRE_NO_MEMORY       BR_ERROR_MAKE(BR_ECLASS_MISC, 3)
#endif
#ifndef BRE_OVERFLOW
#define BRE_OVERFLOW        BR_ERROR_MAKE(BR_ECLASS_MISC, 4)
#endif
#ifndef BRE_UNDERFLOW
#define BRE_UNDERFLOW       BR_ERROR_MAKE(BR_ECLASS_MISC, 5)
#endif
#ifndef BRE_NOT_ACTIVE
#define BRE_NOT_ACTIVE      BR_ERROR_MAKE(BR_ECLASS_MISC, 6)
#endif
#ifndef BRE_ALLREADY_ACTIVE
#define BRE_ALLREADY_ACTIVE BR_ERROR_MAKE(BR_ECLASS_MISC, 7)
#endif
#ifndef BRE_UNSUPPORTED
#define BRE_UNSUPPORTED     BR_ERROR_MAKE(BR_ECLASS_MISC, 8)
#endif
#ifndef BRE_INVALID
#define BRE_INVALID         BR_ERROR_MAKE(BR_ECLASS_MISC, 9)
#endif
#ifndef BRE_RANGE
#define BRE_RANGE           BR_ERROR_MAKE(BR_ECLASS_MISC, 10)
#endif
#ifndef BRE_NO_SPACE
#define BRE_NO_SPACE        BR_ERROR_MAKE(BR_ECLASS_MISC, 11)
#endif
#ifndef BRE_PERMISSION
#define BRE_PERMISSION      BR_ERROR_MAKE(BR_ECLASS_MISC, 12)
#endif
#ifndef BRE_DEV_FAIL
#define BRE_DEV_FAIL                 BR_ERROR_MAKE(BR_ECLASS_DEVICE, 1) // All general device errors
#endif
#ifndef BRE_DEV_HARDWARE_NOT_PRESENT
#define BRE_DEV_HARDWARE_NOT_PRESENT BR_ERROR_MAKE(BR_ECLASS_DEVICE, 2) // Hardware not detected: usually device library failed init
#endif
#ifndef BRE_DEV_NO_MEMORY
#define BRE_DEV_NO_MEMORY            BR_ERROR_MAKE(BR_ECLASS_DEVICE, 3) // Driver failed in mem allocate
#endif
#ifndef BRE_DEV_NO_VIDEO_MEMORY
#define BRE_DEV_NO_VIDEO_MEMORY      BR_ERROR_MAKE(BR_ECLASS_DEVICE, 4) // Driver failed in video mem allocate
#endif
#ifndef BRE_DEV_UNSUPPORTED
#define BRE_DEV_UNSUPPORTED          BR_ERROR_MAKE(BR_ECLASS_DEVICE, 5) // Device does not support capabilities
#endif
#ifndef BRE_DEV_HARDWARE_SET
#define BRE_DEV_HARDWARE_SET         BR_ERROR_MAKE(BR_ECLASS_DEVICE, 6) // Driver failed some hardware setup call
#endif
#ifndef BRE_DEV_HARDWARE_NOT_READY
#define BRE_DEV_HARDWARE_NOT_READY   BR_ERROR_MAKE(BR_ECLASS_DEVICE, 7)
#endif
#ifndef BRE_DEV_NOT_FOUND
#define BRE_DEV_NOT_FOUND            BR_ERROR_MAKE(BR_ECLASS_DEVICE, 8) // Device driver not present (BRender should return this)
#endif
#ifndef BRE_DEV_REFUSED
#define BRE_DEV_REFUSED              BR_ERROR_MAKE(BR_ECLASS_DEVICE, 9)  // Some request refused
#endif
#ifndef BRE_DEV_LOCK_FAIL
#define BRE_DEV_LOCK_FAIL            BR_ERROR_MAKE(BR_ECLASS_DEVICE, 10) // Some locking call failed
#endif
#ifndef BRE_DEV_OUTPUT_UNAVAILABLE
#define BRE_DEV_OUTPUT_UNAVAILABLE   BR_ERROR_MAKE(BR_ECLASS_DEVICE, 11) // Output facility is not available
#endif
#ifndef BRE_DEV_ALREADY_INIT
#define BRE_DEV_ALREADY_INIT         BR_ERROR_MAKE(BR_ECLASS_DEVICE, 12) // Some facility is already initialised
#endif
#ifndef BRE_DEV_DIRECTDRAW_ERROR
#define BRE_DEV_DIRECTDRAW_ERROR     BR_ERROR_MAKE(BR_ECLASS_DEVICE, 13) // Direct Draw returned an error not covered by other codes
#endif

/* brlimits.h */
#ifndef BR_MAX_NAME
#define BR_MAX_NAME 256
#endif
#ifndef BR_MAX_LIGHTS
#define BR_MAX_LIGHTS 48
#endif
#ifndef BR_MAX_CLIP_PLANES
#define BR_MAX_CLIP_PLANES 6
#endif
#ifndef BR_MAX_HORIZON_PLANES
#define BR_MAX_HORIZON_PLANES 6
#endif
#ifndef BR_MAX_RESOURCE_CLASSES
#define BR_MAX_RESOURCE_CLASSES 256
#endif

/* brender.h / brddi.h version numbers */
#ifndef __BRENDER__
#define __BRENDER__ 0x010400
#endif
#ifndef __BRENDER_DDI__
#define __BRENDER_DDI__ 0x010400
#endif

/* pixelmap.h: no keyed-transparency flag in our 1.2-era pixelmaps (never set) */
#ifndef BR_PMF_KEYED_TRANSPARENCY
#define BR_PMF_KEYED_TRANSPARENCY 0
#endif

/* brddi.h: object helpers */
#ifndef ObjectDevice
#define ObjectDevice(o) ((struct br_device*)((br_object*)(o))->dispatch->_device((br_object*)(o)))
#endif

#ifndef BR_VERSION
#define BR_VERSION(major, minor, point) ((((major) & 0xFF) << 16) + (((minor) & 0xFF) << 8) + (((point) & 0xFF) << 0))
#endif

/* brddi.h object/container helpers (1.4 macros over the dispatch tables) */
#define ObjectIsType(o, t) (((br_object*)(o))->dispatch->_isType((br_object*)(o), (t)))
#define ObjectQueryMany(o, tv, e, es, pc) (((br_object*)(o))->dispatch->_queryMany((br_object*)(o), (tv), (e), (es), (pc)))
#define ObjectQueryManySize(o, ps, tv) (((br_object*)(o))->dispatch->_queryManySize((br_object*)(o), (ps), (tv)))
#define ObjectContainerAddFront(c, o) (((br_object_container*)(c))->dispatch->_addFront((br_object_container*)(c), (br_object*)(o)))
#define ObjectContainerRemove(c, o) (((br_object_container*)(c))->dispatch->_remove((br_object_container*)(c), (br_object*)(o)))
#define RendererStateDefault(r, m) (((br_renderer*)(r))->dispatch->_stateDefault((br_renderer*)(r), (m)))

/* brdiag.h */
#define BR_FATAL(s) BrFatal(__FILE__, __LINE__, s)
#define BR_FATAL0(s) BrFatal(__FILE__, __LINE__, s)
#define BR_FATAL1(s, a) BrFatal(__FILE__, __LINE__, s, a)
#define BR_FATAL2(s, a, b) BrFatal(__FILE__, __LINE__, s, a, b)
#define BR_ERROR(s) BrFailure(s)
#define BR_ERROR0(s) BrFailure(s)
#define BR_ERROR1(s, a) BrFailure(s, a)
#define BR_ERROR2(s, a, b) BrFailure(s, a, b)

/* brmem.h / vector helpers */
#include <string.h>
#define BrMemCpy(d, s, n) memcpy((d), (s), (n))
#define BrMemSet(d, c, n) memset((d), (c), (n))
#ifndef BR_VECTOR2
#define BR_VECTOR2(a, b) { a, b }
#endif
#define BrVector4Set(V, A, B, C, D) do { (V)->v[0] = (br_scalar)(A); (V)->v[1] = (br_scalar)(B); (V)->v[2] = (br_scalar)(C); (V)->v[3] = (br_scalar)(D); } while (0)
#define BrVector4Scale(V1, V2, S) do { (V1)->v[0] = (V2)->v[0] * (S); (V1)->v[1] = (V2)->v[1] * (S); (V1)->v[2] = (V2)->v[2] * (S); (V1)->v[3] = (V2)->v[3] * (S); } while (0)
#define BR_RCP(a) ((br_scalar)(1.0f / (a)))
/* 5-6-5 component values (glrend passes 31,0,31 for the magenta key) */
#define BR_COLOUR_565(r, g, b) ((br_uint_16)((((r) & 0x1f) << 11) | (((g) & 0x3f) << 5) | ((b) & 0x1f)))
#define BrPixelmapFlush(pm) (((br_device_pixelmap*)(pm))->dispatch->_flush((br_device_pixelmap*)(pm)))

/* implemented in drivers/glrend/compat/compat.c */
void C2_HOOK_CDECL BrMatrix4Copy23(br_matrix4* A, const br_matrix23* B);
void C2_HOOK_CDECL BrMatrix4Transpose(br_matrix4* A);
void C2_HOOK_CDECL BrVector4Normalise(br_vector4* v1, const br_vector4* v2);
br_boolean C2_HOOK_CDECL BrTokenValueComparePartial(br_token_value* tv1, br_token_value* tv2, const br_token* insignificant);
br_pool* C2_HOOK_CDECL BrPoolAllocate(int block_size, int chunk_size, br_uint_8 mem_type);
void C2_HOOK_CDECL BrPoolFree(br_pool* pool);
void* C2_HOOK_CDECL BrPoolBlockAllocate(br_pool* pool);
void C2_HOOK_CDECL BrPoolBlockFree(br_pool* pool, void* block);
void C2_HOOK_CDECL BrRendererFrameBegin(void);

/* compiler.h */
#ifndef BR_TRUE
#define BR_TRUE 1
#endif
#ifndef BR_FALSE
#define BR_FALSE 0
#endif
typedef uintptr_t br_uintptr_t;
typedef intptr_t br_intptr_t;
#ifndef BR_PUBLIC
#define BR_PUBLIC C2_HOOK_CDECL
#endif
#ifndef BR_EXPORT
#define BR_EXPORT C2_HOOK_CDECL
#endif
#ifndef BR_METHOD
#define BR_METHOD C2_HOOK_CDECL
#endif
#ifndef BR_CALLBACK
#define BR_CALLBACK C2_HOOK_CDECL
#endif
#ifndef BR_ASIZE
#define BR_ASIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif
#define BR_STR(s) #s
#define BR_CMETHOD_DECL(t, m)        BR_METHOD _M_##t##_##m
#define BR_CMETHOD_VA_DECL(t, m)     BR_METHOD _M_##t##_##m
#define BR_CMETHOD_PTR_DECL(t, m)    (BR_METHOD * _##m)
#define BR_CMETHOD_VA_PTR_DECL(t, m) (BR_METHOD * _##m)
#define BR_CMETHOD(t, m)             (_M_##t##_##m)
#define BR_CMETHOD_REF(t, m)         (_M_##t##_##m)
#define BR_CMETHOD_CALL(t, m, o)     (((t *)(o))->dispatch->_##m)
#define BR_CMETHOD_STR(t, m)         BR_STR(_M_##t##_##m)

/* pixelmap.h: our br_device_pixelmap / br_pixelmap layout after dispatch + pm_identifier */
#define BR_PIXELMAP_MEMBERS     void* pm_pixels;     br_uint_32 pm_pixels_qualifier;     br_pixelmap* pm_map;     br_colour_range pm_src_key;     br_colour_range pm_dst_key;     br_uint_32 pm_key;     br_int_16 pm_row_bytes;     br_int_16 pm_mip_offset;     br_uint_8 pm_type;     br_uint_8 pm_flags;     br_uint_16 pm_copy_function;     br_uint_16 pm_base_x;     br_uint_16 pm_base_y;     br_uint_16 pm_width;     br_uint_16 pm_height;     br_int_16 pm_origin_x;     br_int_16 pm_origin_y;     void* pm_user;     void* pm_stored;

/* BRender 1.4 pmfail.c: methods that always fail (our core has no "fail" pixelmap class) */
br_error C2_HOOK_CDECL BrCompatDevicePixelmapFail(void);
#define _M_br_device_pixelmap_fail_allocateSub ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copy ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copyBits ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copyDirty ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copyFrom ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copyFromDirty ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copyTo ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_copyToDirty ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_directLock ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_directUnlock ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_doubleBuffer ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_doubleBufferDirty ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_fill ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_fillDirty ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_flush ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_getControls ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_line ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_pixelAddressQuery ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_pixelAddressSet ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_pixelQuery ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_pixelSet ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangle ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangle2 ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangleCopyFrom ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangleCopyTo ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangleFill ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangleStretchCopyFrom ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rectangleStretchCopyTo ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rowQuery ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rowSet ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_rowSize ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_setControls ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_synchronise ((void*)BrCompatDevicePixelmapFail)
#define _M_br_device_pixelmap_fail_text ((void*)BrCompatDevicePixelmapFail)

#endif
