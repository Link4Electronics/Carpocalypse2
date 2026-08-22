#ifndef SHADER_DATA_H_
#define SHADER_DATA_H_

/*
 * std140 payloads for the shared GLSL uniform blocks (drivers/commonrend).
 *
 * These structs must be byte-identical between the glrend and sdl3gpurend
 * backends — the shaders are shared, so the CPU payload has to match their
 * std140 layout on every backend. Keep the field order in lockstep with the
 * corresponding uniform block in the GLSL.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define BR_STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)

#pragma pack(push, 16)

/* std140-compatible light structure */
typedef struct shader_data_light {
    /* (X, Y, Z, T), if T == 0, direct, otherwise point/spot */
    alignas(16) br_vector4 position;
    /* (X, Y, Z, 0), normalised */
    alignas(16) br_vector4 direction;
    /* (X, Y, Z, 0), normalised */
    alignas(16) br_vector4 half;
    /* (R, G, B, 0) */
    alignas(16) br_vector4 colour;
    /* (intensity, constant, linear, attenutation) */
    alignas(16) br_vector4 iclq;
    /* (inner, outer), if (0.0, 0.0), then this is a point light. */
    alignas(16) br_vector2 spot_angles;

    /* Pad out the structure to maintain alignment. */
    alignas(4) float _pad0, _pad1;
} shader_data_light;
BR_STATIC_ASSERT(sizeof(shader_data_light) % 16 == 0, "shader_data_light is not aligned");

/* NOTE on field order: matches the shared GLSL block (drivers/commonrend).
 * SDL3 GPU's per-slot uniform data window is capped at 4096 bytes
 * (MAX_UBO_SECTION_SIZE in its backends), so every field the shaders actually
 * read must sit within the first 4096 bytes of the block. The lights array
 * (96 * 48 = 4608 bytes) therefore goes LAST: it is dead weight today (vertex
 * lighting is compiled out by DEBUG_DISABLE_LIGHTS and the fragment shader
 * never references it), and its tail is clipped by SDL3. */
typedef struct shader_data_scene {
    alignas(16) br_vector4 eye_view;
    alignas(16) br_vector4 clip_planes[BR_MAX_CLIP_PLANES];
    alignas(4) uint32_t num_clip_planes;
    alignas(4) float yon_z;
    alignas(4) uint32_t num_lights;
    alignas(16) shader_data_light lights[BR_MAX_LIGHTS];

} shader_data_scene;
BR_STATIC_ASSERT(sizeof(((shader_data_scene*)NULL)->lights) == sizeof(shader_data_light) * BR_MAX_LIGHTS,
    "std::array<shader_data_light> fucked up");

typedef struct shader_data_model {
    alignas(16) br_matrix4 model_view;
    alignas(16) br_matrix4 projection;
    alignas(16) br_matrix4 projection_brender;
    alignas(16) br_matrix4 mvp;
    alignas(16) br_matrix4 normal_matrix;
    alignas(16) br_matrix4 environment_matrix;
    alignas(16) br_matrix4 map_transform;
    alignas(16) br_vector4 surface_colour;
    alignas(16) br_vector4 clear_colour;
    alignas(16) br_vector4 eye_m;
    alignas(4) float ka;
    alignas(4) float ks;
    alignas(4) float kd;
    alignas(4) float power;
    alignas(4) uint32_t lighting;
    alignas(4) uint32_t uv_source;
    alignas(4) uint32_t disable_colour_key;
    alignas(4) uint32_t disable_texture;
    alignas(4) uint32_t fog_enabled;
    alignas(16) br_vector4 fog_colour;
    alignas(4) float fog_min;
    alignas(4) float fog_max;
    alignas(4) float alpha;
    alignas(4) uint32_t prelit;

} shader_data_model;

#pragma pack(pop)

#ifdef __cplusplus
};
#endif

#endif /* SHADER_DATA_H_ */
