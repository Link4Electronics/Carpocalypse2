/*
 * Private device driver structure
 */
#ifndef _DRV_H_
#define _DRV_H_

#if defined(_MSC_VER) && _MSC_VER <= 1929
#define alignas(X) _Alignas(X)
#else
#include <stdalign.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

#include "commonrend.h"

#if defined(BREND_DRIVER_GL)
#include "glad/glad.h"
#elif defined(BREND_DRIVER_SDL3GPUREND)
#include <SDL3/SDL_gpu.h>
#endif

#define BR_DEVICE_PRIVATE
#define BR_OUTPUT_FACILITY_PRIVATE
#define BR_DEVICE_PIXELMAP_PRIVATE
#define BR_RENDERER_FACILITY_PRIVATE
#define BR_BUFFER_STORED_PRIVATE
#define BR_GEOMETRY_V1_MODEL_PRIVATE
#define BR_GEOMETRY_STORED_PRIVATE
#define BR_RENDERER_STATE_STORED_PRIVATE
#define BR_RENDERER_PRIVATE
#define BR_DEVICE_CLUT_PRIVATE

#if defined(BREND_DRIVER_GL)
#define BR_GEOMETRY_V1_BUCKETS_PRIVATE
#endif

#include "brddi.h"
#if defined(BREND_DRIVER_GL)
#include "brglrend.h"
#else
#include "brsdl3gpurend.h"
#endif

#include "formats.h"
#include "pm.h"
#include "video.h"
#include "state.h"

#include "template.h"
#include "device.h"
#include "outfcty.h"
#include "devpixmp.h"
#include "rendfcty.h"
#include "renderer.h"
#include "sstate.h"
#include "sbuffer.h"
#include "gstored.h"
#include "gv1model.h"
#if defined(BREND_DRIVER_GL)
#include "gv1buckt.h"
#include "glassert.h"
#else
/* SDL3 GPU has no assert helpers yet. */
#endif
#include "devclut.h"
/* clang-format on */
/*
 * Macros that expand to the first two arguments of a template entry
 * Builtin or device token
 */
#define BRT(t) BRT_##t, 0
#define DEV(t) 0, #t

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#include "drv_ip.h"

#endif

#ifdef __cplusplus
};
#endif
#endif /* _DRV_H_ */
