/*
 * Compatibility umbrella for drivers ported from upstream BRender
 * (drivers/commonrend, drivers/glrend, drivers/sdl3gpurend).
 *
 * Our core keeps the upstream DDI machinery but organises headers slightly
 * differently; this directory supplies the missing DDI pieces and this
 * umbrella so the ported drivers build unmodified.
 */
#ifndef _BRDDI_H_
#define _BRDDI_H_

#ifndef _BRENDER_H_
#include "brender.h"
#endif

#ifndef _POOL_H_
#include "pool.h"
#endif

#ifndef _BREXCEPT_H_
#include "brexcept.h"
#endif

#ifndef _REGISTER_H_
#include "register.h"
#endif

#ifndef _TOKEN_H_
#include "token.h"
#endif

#ifndef _TOKENMAP_H_
#include "tokenmap.h"
#endif

#ifndef _LEXER_H_
#include "lexer.h"
#endif

#ifndef _BRIMAGE_H_
#include "brimage.h"
#endif

#ifndef _DEV_OBJS_H_

struct v11model_f;
struct v11model_x;
struct v1model_f;
struct v1model_x;
struct brp_block;
union brp_vertex;
struct fmt_vertex;
#include "dev_objs.cgh"
#endif

/*
 * Pull in DDI prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _BRDDI_P_H_
#include "brddi_p.h"
#endif

#endif
#endif /* _BRDDI_H_ */
