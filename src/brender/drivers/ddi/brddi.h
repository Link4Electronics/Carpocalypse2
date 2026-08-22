/*
 * Compatibility umbrella for drivers ported from upstream BRender
 * (drivers/commonrend, drivers/glrend, drivers/sdl3gpurend).
 *
 * Minimal shim: pulls in our public headers plus the small DDI pieces the
 * ported drivers need that our core does not provide under the same name.
 */
#ifndef _BRDDI_H_
#define _BRDDI_H_

#ifndef _BRENDER_H_
#include "brender.h"
#endif

#ifndef _POOL_H_
#include "pool.h"
#endif

#ifndef _BRASSERT_H_
#include "brassert.h"
#endif

#ifndef _BRHTON_H_
#include "brhton.h"
#endif

#endif /* _BRDDI_H_ */
