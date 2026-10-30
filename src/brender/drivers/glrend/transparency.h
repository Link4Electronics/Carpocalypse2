#ifndef GL_TRANSPARENCY_H
#define GL_TRANSPARENCY_H
/* drv.h supplies the private driver types. */
br_boolean GLQueueTransparent(br_renderer* renderer, const struct v11group* group, const shader_data_model* model);
void GLFlushTransparent(br_renderer* renderer);
void GLFreeTransparent(br_renderer* renderer);
#endif
