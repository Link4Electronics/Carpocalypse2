#ifndef BRGL_DISPLAY_H
#define BRGL_DISPLAY_H
/* Presentation geometry shared with the SDL platform; game coordinates stay 640x480. */
extern float gGLPresentationAspect;
extern int gGLWorldRendering;
extern int gGLFrameHasWorld;
extern int gGLWideBackdrop;
extern int gGLFrameHasWide;
void C2GLFitOutput(int width, int height, int* x, int* y, int* w, int* h);
void C2GLUIRect(int width, int height, int* x, int* y, int* w, int* h);
void C2GLMapMouse(int width, int height, float x, float y, float* gx, float* gy);
void C2GLScaleUIRect(int width, int height, int* x, int* y, int* w, int* h);
#ifdef __vita__
unsigned long long C2VitaGLTicksNS(void);
void C2VitaGLRecordOverlayCheck(int empty, unsigned long long elapsed_ns);
void C2VitaGLRecordOverlayRows(int rows);
void C2VitaGLRecordOverlayScope(void);
void C2VitaGLLogOverlayBenchmark(unsigned pixels, unsigned long long scalar_ns, unsigned long long vector_ns, int valid);
#endif
#endif
