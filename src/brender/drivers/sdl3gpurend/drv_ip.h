#ifndef _DRV_IP_H_
#define _DRV_IP_H_

#include "commonrend.h"

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * device.c
 */
br_device* DeviceSDL3GPURENDAllocate(const char* identifier, const char* arguments);

/*
 * rendfcty.c
 */
br_renderer_facility* RendererFacilitySDL3GPURENDInit(br_device* dev);

/*
 * outfcty.c
 */
br_output_facility* OutputFacilitySDL3GPURENDInit(br_device* dev, br_renderer_facility* rendfcty);

/*
 * devpmgpu.c
 */
br_device_pixelmap* DevicePixelmapSDL3GPURENDAllocateFront(br_device* dev, br_output_facility* outfcty, br_token_value* tv);

/*
 * renderer.c
 */
br_renderer* RendererSDL3GPURENDAllocate(br_device* device, br_renderer_facility* facility, br_device_pixelmap* dest);

/*
 * sstate.c
 */
br_renderer_state_stored* RendererStateStoredSDL3GPURENDAllocate(br_renderer* renderer, state_stack* base_state, br_uint_32 m,
    br_token_value* tv);

/*
 * state.c and friends
 */
void StateSDL3GPURENDInit(state_all* state, void* res);
void StateSDL3GPURENDInitMatrix(state_all* state);
void StateSDL3GPURENDInitCull(state_all* state);
void StateSDL3GPURENDInitClip(state_all* state);
void StateSDL3GPURENDInitSurface(state_all* state);
void StateSDL3GPURENDInitPrimitive(state_all* state);
void StateSDL3GPURENDInitOutput(state_all* state);
void StateSDL3GPURENDInitHidden(state_all* state);
void StateSDL3GPURENDInitLight(state_all* state);

struct br_tv_template* StateSDL3GPURENDGetStateTemplate(state_all* state, br_token part, br_int_32 index);

void StateSDL3GPURENDReset(state_cache* cache);

br_boolean StateSDL3GPURENDPush(state_all* state, uint32_t mask);
br_boolean StateSDL3GPURENDPop(state_all* state, uint32_t mask);
void StateSDL3GPURENDDefault(state_all* state, uint32_t mask);

void StateSDL3GPURENDUpdateScene(state_cache* cache, state_stack* state);
void StateSDL3GPURENDUpdateModel(state_cache* cache, state_matrix* matrix);
void StateSDL3GPURENDFillModel(state_stack* state, uint32_t states, shader_data_model* model);
void StateSDL3GPURENDFillModelTexture(state_stack* state, uint32_t states, shader_data_model* model,
    struct br_buffer_stored** colour_map, br_boolean* filter_linear, br_boolean* palette_dirty,
    const br_uint_32** palette_entries);
void StateSDL3GPURENDCopy(state_stack* dst, state_stack* src, uint32_t mask);

/*
 * v1model.c
 *
 * Applies the current material state into `model` (the shader UBO payload)
 * and, when a texture is active, fills `texture`/`sampler` with the SDL3 GPU
 * texture/sampler to bind on the fragment sampler slot used by the draw.
 * The draw tail (modelrender.c) uploads the UBO payload and issues the draw.
 */
void StoredSDL3GPURENDApplyProperties(HVIDEO hVideo, state_stack* state, uint32_t states, shader_data_model* model, br_buffer_stored* default_texture, struct SDL_GPUTexture** texture, struct SDL_GPUSampler** sampler);
br_boolean SDL3GPUREND_ComputeScreenAABB(const br_matrix4* mvp, struct v11group* gp, HVIDEO hVideo, br_device_pixelmap* colour_target, br_rectangle* out);

/*
 * gstored.c
 */
br_geometry_stored* GeometryStoredSDL3GPURENDAllocate(struct br_geometry_v1_model* gv1model, const char* id, struct br_renderer* r, struct v11model* model);
void StoredSDL3GPURENDRenderGroup(struct br_geometry_stored* self, struct br_renderer* renderer, struct sdl3_groupinfo* groupinfo);

/*
 * cache.c
 */

/*
 * sbuffer.c
 */
struct br_buffer_stored* BufferStoredSDL3GPURENDAllocate(struct br_renderer* renderer, br_token use, struct br_device_pixelmap* pm, br_token_value* tv);

/*
 * onscreen.c
 */
br_token SDL3GPURENDOnScreenCheck(br_renderer* self, const br_matrix4* model_to_screen, const br_bounds3_f* bounds);

/*
 * ext_procs.c
 */
void* BREND_FN(DevicePixelmap, GetGetProcAddress)(br_device_pixelmap* self);

void BREND_FN(DevicePixelmap, GetViewport)(br_device_pixelmap* self, int *x, int *y, float *width_multiplier, float *height_multiplier);

void BREND_FN(DevicePixelmap, SwapBuffers)(br_device_pixelmap* self);

void BREND_FN(DevicePixelmap, Free)(br_device_pixelmap* self);

/*
 * devclut.c
 */
struct br_device_clut* DeviceClutSDL3GPURENDAllocate(br_device* dev, char* identifier);

/*
 * Hijack nulldev's no-op implementations.
 */
br_geometry_lighting* GeometryLightingNullAllocate(br_renderer_facility* type, const char* id);
br_geometry_v1_model* BREND_FN(GeometryV1Model, Allocate)(br_renderer_facility* type, const char* id);

#ifdef __cplusplus
};
#endif

#endif
#endif /* _DRV_IP_H_ */
