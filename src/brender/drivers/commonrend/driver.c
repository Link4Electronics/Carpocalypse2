#include "drv.h"

#if defined(BREND_DRIVER_GL)
br_token BRT_OPENGL_TEXTURE_U32 = BR_NULL_TOKEN;
#endif

br_device *BR_EXPORT BREND_FN(BrDrv1, Begin)(const char *arguments)
{
    br_device *dev = BREND_FN(Device, Allocate)(BREND_DRIVER_NAME, arguments);
    if(!dev)
        return NULL;

#if defined(BREND_DRIVER_GL)
    BRT_OPENGL_TEXTURE_U32 = BrTokenCreate("OPENGL_TEXTURE_U32", BRT_UINT_32);
#endif
    return dev;
}

#ifdef DEFINE_BR_ENTRY_POINT
br_device *BR_EXPORT BrDrv1Begin(const char *arguments)
{
    return BREND_FN(BrDrv1, Begin)(arguments);
}
#endif
