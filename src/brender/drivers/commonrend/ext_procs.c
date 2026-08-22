#include "drv.h"
#include "brassert.h"
#include "commonrend.h"

void* BREND_FN(DevicePixelmap, GetGetProcAddress)(br_device_pixelmap* self) {
    UASSERT(self->use_type == BRT_NONE);
    return (void*)self->asFront.callbacks.get_proc_address;
}

void BREND_FN(DevicePixelmap, GetViewport)(br_device_pixelmap* self, int *x, int *y, float *width_multiplier, float *height_multiplier) {
    UASSERT(self->use_type == BRT_NONE);
    self->asFront.callbacks.get_viewport(x, y, width_multiplier, height_multiplier);
}

void BREND_FN(DevicePixelmap, SwapBuffers)(br_device_pixelmap* self) {
    UASSERT(self->use_type == BRT_NONE);
    self->asFront.callbacks.swap_buffers((br_pixelmap*)self);
}

void BREND_FN(DevicePixelmap, Free)(br_device_pixelmap* self) {
    UASSERT(self->use_type == BRT_NONE);
}
