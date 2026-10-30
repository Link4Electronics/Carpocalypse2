#ifndef _BRGLREND_H_
#define _BRGLREND_H_

#ifndef _BRENDER_H_
#error Please include brender.h first
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern br_token BRT_OPENGL_TEXTURE_U32;

/*
 * Platform callbacks handed to the front screen pixelmap via BRT_OPENGL_CALLBACKS_P
 * (the platform layer owns the window and the GL context).
 */
typedef void* br_device_gl_getprocaddress_cbfn(const char* name);
typedef void br_device_gl_getviewport_cbfn(int* x, int* y, float* width_multiplier, float* height_multiplier);
typedef void br_device_gl_swapbuffers_cbfn(br_pixelmap* pm);

typedef struct br_device_gl_callback_procs {
    br_device_gl_getprocaddress_cbfn* get_proc_address;
    br_device_gl_getviewport_cbfn* get_viewport;
    br_device_gl_swapbuffers_cbfn* swap_buffers;
} br_device_gl_callback_procs;

#ifdef __cplusplus
}
#endif

/*
 * Function prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _BRGLREND_P_H
#include "brglrend_p.h"
#endif

#endif /* _NO_PROTOTYPES */

#endif /* _BRGLREND_H_ */
