#include "brgl_display.h"
float gGLPresentationAspect = 4.f / 3.f;
int gGLWorldRendering = 0;
int gGLFrameHasWorld = 0;
/* port: set while the frontend copies its full-screen backdrop; it is then drawn across the
 * whole display (uniform zoom, cropped top/bottom) instead of the 4:3 UI area */
int gGLWideBackdrop = 0;
int gGLFrameHasWide = 0;

void C2GLFitOutput(int width, int height, int* x, int* y, int* w, int* h) {
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    *x = *y = 0; *w = width; *h = height;
    /* Expand horizontally on wide displays; retain the vertical field of view.
     * A tall window gets bars above/below instead of a narrower/cropped world. */
    if (width * 3 < height * 4) { *h = width * 3 / 4; if (*h < 1) *h = 1; *y = (height - *h) / 2; }
}
void C2GLUIRect(int width, int height, int* x, int* y, int* w, int* h) {
    C2GLFitOutput(width, height, x, y, w, h);
    if (*w * 3 > *h * 4) { int fit = *h * 4 / 3; *x += (*w - fit) / 2; *w = fit; }
}
void C2GLMapMouse(int width, int height, float x, float y, float* gx, float* gy) {
    int ux,uy,uw,uh;
    C2GLUIRect(width,height,&ux,&uy,&uw,&uh);
    *gx = (x - ux) * 640.f / uw; *gy = (y - uy) * 480.f / uh;
}
void C2GLScaleUIRect(int width, int height, int* x, int* y, int* w, int* h) {
    float fraction;
    (void)height; (void)y; (void)h;
    if (gGLWorldRendering || gGLPresentationAspect <= 4.f / 3.f) return;
    fraction = (4.f / 3.f) / gGLPresentationAspect;
    *x = (int)(width * (1.f - fraction) * .5f + *x * fraction + .5f);
    *w = (int)(*w * fraction + .5f);
    if (*w < 1) *w = 1;
}
