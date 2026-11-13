#include "v1dbfile.h"

#include "actsupt.h"
#include "chunkids.h"
#include "dbsetup.h"
#include "matsupt.h"
#include "modsupt.h"
#include "regsupt.h"
#include "stackids.h"

#include "core/fw/brqsort.h"
#include "core/fw/datafile.h"
#include "core/fw/diag.h"
#include "core/fw/file.h"
#include "core/fw/genfile.h"
#include "core/fw/resource.h"
#include "core/fw/scratch.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include <stddef.h>

// GLOBAL: CARMA2_HW 0x00663490
br_file_struct_member br_vertex_FM[] = {
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, p.v[0]), "p.v[x]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, p.v[1]), "p.v[y]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, p.v[2]), "p.v[z]", NULL },
};

// GLOBAL: CARMA2_HW 0x006634c0
br_file_struct br_vertex_F = { "br_vertex", BR_ASIZE(br_vertex_FM), br_vertex_FM, sizeof(br_vertex) };

// GLOBAL: CARMA2_HW 0x006634d0
br_file_struct_member br_vertex_uv_FM[] = {
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, map.v[0]), "map.v[0]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, map.v[1]), "map.v[1]", NULL },
};

// GLOBAL: CARMA2_HW 0x006634f0
br_file_struct br_vertex_uv_F = { "br_vertex_uv", BR_ASIZE(br_vertex_uv_FM), br_vertex_uv_FM, sizeof(br_vertex) };

// GLOBAL: CARMA2_HW 0x00663500
br_file_struct_member br_old_vertex_uv_FM[] = {
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, p.v[0]), "p.v[X]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, p.v[1]), "p.v[Y]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, p.v[2]), "p.v[Z]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, map.v[0]), "map.v[0]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_vertex, map.v[1]), "map.v[1]", NULL },
};

// GLOBAL: CARMA2_HW 0x00663550
br_file_struct br_old_vertex_uv_F = { "br_old_vertex_uv", BR_ASIZE(br_old_vertex_uv_FM), br_old_vertex_uv_FM, sizeof(br_vertex) };

// GLOBAL: CARMA2_HW 0x00663560
br_file_struct_member br_face_FM[] = {
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices), "vertices[0]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[1]), "vertices[1]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[2]), "vertices[2]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, smoothing), "smoothing", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_face, flags), "flags", NULL },
};

// GLOBAL: CARMA2_HW 0x006635b0
br_file_struct br_face_F = { "br_face", BR_ASIZE(br_face_FM), br_face_FM, sizeof(br_face) };

// GLOBAL: CARMA2_HW 0x006635c0
br_file_struct_member br_old_face_1_FM[] = {
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[0]), "vertices[0]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[1]), "vertices[1]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[2]), "vertices[2]", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_face, smoothing), "smoothing", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_face, flags), "flags", NULL },
};

// GLOBAL: CARMA2_HW 0x00663610
br_file_struct br_old_face_1_F = { "br_old_face_1", BR_ASIZE(br_old_face_1_FM), br_old_face_1_FM, sizeof(br_face) };

// GLOBAL: CARMA2_HW 0x00663620
br_file_struct_member br_old_face_FM[] = {
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[0]), "vertices[0]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[1]), "vertices[1]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, vertices[2]), "vertices[2]", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_face, material), "material", NULL },
    { DF_TYPE_BR_UINT_32, offsetof(br_face, smoothing), "smoothing", NULL },
};

// GLOBAL: CARMA2_HW 0x00663670
br_file_struct br_old_face_F ={ "br_old_face", BR_ASIZE(br_old_face_FM), br_old_face_FM, sizeof(br_face) };

// GLOBAL: CARMA2_HW 0x00663680
br_file_struct_member br_model_FM[] = {
    { DF_TYPE_BR_UINT_16, offsetof(br_model, flags), "flags", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_model, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x006636a0
br_file_struct br_model_F ={ "br_model", BR_ASIZE(br_model_FM), br_model_FM, sizeof(br_model) };


// GLOBAL: CARMA2_HW 0x006636b0
br_file_struct_member br_old_model_1_FM[] = {
    { DF_TYPE_ASCIZ, offsetof(br_model, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x006636c0
br_file_struct br_old_model_1_F = { "br_old_model_1", 1, br_old_model_1_FM, sizeof(br_model) };

// GLOBAL: CARMA2_HW 0x006636d0
br_file_struct_member br_pivot_FM[] = {
    { DF_TYPE_BR_SCALAR, offsetof(br_model, pivot.v[0]), "pivot.v[X]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_model, pivot.v[1]), "pivot.v[Y]", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_model, pivot.v[2]), "pivot.v[Z]", NULL },
};

// GLOBAL: CARMA2_HW 0x00663700
br_file_struct br_pivot_F = { "br_pivot", 3, br_pivot_FM, sizeof(br_model) };

// GLOBAL: CARMA2_HW 0x00663710
br_file_struct_member br_material_old_FM[] = {
    { DF_TYPE_BR_COLOUR, offsetof(br_material, colour), "colour", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_material, opacity), "opacity", NULL },
    { DF_TYPE_BR_UFRACTION, offsetof(br_material, ka), "ka", NULL },
    { DF_TYPE_BR_UFRACTION, offsetof(br_material, kd), "kd", NULL },
    { DF_TYPE_BR_UFRACTION, offsetof(br_material, ks), "ks", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_material, power), "power", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_material, flags), "flags", NULL },
    { DF_TYPE_BR_VECTOR2, offsetof(br_material, map_transform.m[0]), "map_transform.m[0]", NULL },
    { DF_TYPE_BR_VECTOR2, offsetof(br_material, map_transform.m[1]), "map_transform.m[1]", NULL },
    { DF_TYPE_BR_VECTOR2, offsetof(br_material, map_transform.m[2]), "map_transform.m[2]", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_material, index_base), "index_base", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_material, index_range), "index_range", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_material, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x006637e0
br_file_struct br_material_old_F = { "br_material_old", BR_ASIZE(br_material_old_FM), br_material_old_FM, sizeof(br_material) };

// GLOBAL: CARMA2_HW 0x006637f0
br_file_struct_member br_material_FM[] = {
    { DF_TYPE_BR_COLOUR, offsetof(br_material, colour), "colour", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_material, opacity), "opacity", NULL },
    { DF_TYPE_BR_UFRACTION, offsetof(br_material, ka), "ka", NULL },
    { DF_TYPE_BR_UFRACTION, offsetof(br_material, kd), "kd", NULL },
    { DF_TYPE_BR_UFRACTION, offsetof(br_material, ks), "ks", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_material, power), "power", NULL },
    { DF_TYPE_BR_UINT_32, offsetof(br_material, flags), "flags", NULL },
    { DF_TYPE_BR_VECTOR2, offsetof(br_material, map_transform.m[0]), "map_transform.m[0]", NULL },
    { DF_TYPE_BR_VECTOR2, offsetof(br_material, map_transform.m[1]), "map_transform.m[1]", NULL },
    { DF_TYPE_BR_VECTOR2, offsetof(br_material, map_transform.m[2]), "map_transform.m[2]", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_material, index_base), "index_base", NULL },
    { DF_TYPE_BR_UINT_8, offsetof(br_material, index_range), "index_range", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_material, fog_min), "fog_min", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_material, fog_max), "fog_max", NULL },
    { DF_TYPE_BR_COLOUR, offsetof(br_material, fog_colour), "fog_colour", NULL },
    { DF_TYPE_BR_INT_32, offsetof(br_material, subdivide_tolerance), "subdivide_tolerance", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_material, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x00663900
br_file_struct br_material_F = { "br_material", BR_ASIZE(br_material_FM), br_material_FM, sizeof(br_material) };

// GLOBAL: CARMA2_HW 0x00663910
br_df_material_map_item MaterialMaps[] = {
    { CHUNKID_MATERIAL_COLOUR_MAP, offsetof(br_material, colour_map), 0 },
    { CHUNKID_MATERIAL_INDEX_BLEND, offsetof(br_material, index_blend), 1 },
    { CHUNKID_MATERIAL_INDEX_SHADE, offsetof(br_material, index_shade), 1 },
    { CHUNKID_MATERIAL_SCREENDOOR, offsetof(br_material, screendoor), 1 },
    { CHUNKID_MATERIAL_INDEX_FOG, offsetof(br_material, index_fog), 1 },
};

// GLOBAL: CARMA2_HW 0x00663950
br_file_enum_member actor_type_FM[] = {
    { 0, "BR_ACTOR_NONE" },
    { 1, "BR_ACTOR_MODEL" },
    { 2, "BR_ACTOR_LIGHT" },
    { 3, "BR_ACTOR_CAMERA" },
    { 4, "_BR_ACTOR_RESERVED" },
    { 5, "BR_ACTOR_BOUNDS" },
    { 6, "BR_ACTOR_BOUNDS_CORRECT" },
    { 7, "BR_ACTOR_CLIP_PLANE" },
};

// GLOBAL: CARMA2_HW 0x00663990
br_file_enum actor_type_F ={ BR_ASIZE(actor_type_FM), actor_type_FM };

// GLOBAL: CARMA2_HW 0x00663998
br_file_enum_member render_style_FM[] = {
    { 0, "BR_RSTYLE_DEFAULT"},
    { 1, "BR_RSTYLE_NONE"},
    { 2, "BR_RSTYLE_POINTS"},
    { 3, "BR_RSTYLE_EDGES"},
    { 4, "BR_RSTYLE_FACES"},
    { 5, "BR_RSTYLE_BOUNDING_POINTS"},
    { 6, "BR_RSTYLE_BOUNDING_EDGES"},
    { 7, "BR_RSTYLE_BOUNDING_FACES"},
};

// GLOBAL: CARMA2_HW 0x006639d8
br_file_enum render_style_F = { 8, render_style_FM };

// GLOBAL: CARMA2_HW 0x006639e0
br_file_struct_member br_actor_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_actor, type), "type", &actor_type_F },
    { DF_TYPE_ENUM_8, offsetof(br_actor, render_style), "render_style", &render_style_F },
    { DF_TYPE_ASCIZ, offsetof(br_actor, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x00663a10
br_file_struct br_actor_F = { "br_actor", BR_ASIZE(br_actor_FM), br_actor_FM, sizeof(br_actor) };

// GLOBAL: CARMA2_HW 0x00663a20
br_file_struct_member br_transform_matrix34_FM[] ={
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.mat.m[0]), "t.mat.m[0]", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.mat.m[1]), "t.mat.m[1]", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.mat.m[2]), "t.mat.m[2]", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.mat.m[3]), "t.mat.m[3]", NULL },
};

// GLOBAL: CARMA2_HW 0x00663a60
br_file_struct br_transform_matrix34_F ={ "br_transform_matrix34", BR_ASIZE(br_transform_matrix34_FM), br_transform_matrix34_FM, sizeof(br_transform) };

// GLOBAL: CARMA2_HW 0x00663a70
br_file_struct_member br_transform_quat_FM[] = {
    { DF_TYPE_BR_SCALAR, offsetof(br_transform, t.quat.q.x), "t.quat.q.x", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_transform, t.quat.q.y), "t.quat.q.y", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_transform, t.quat.q.z), "t.quat.q.z", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_transform, t.quat.q.w), "t.quat.q.w", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.quat.t), "t.quat.t", NULL },
};

// GLOBAL: CARMA2_HW 0x00663ac0
br_file_struct br_transform_quat_F = { "br_transform_quat", BR_ASIZE(br_transform_quat_FM), br_transform_quat_FM, sizeof(br_transform) };

// GLOBAL: CARMA2_HW 0x00663ad0
br_file_enum_member angle_order_FM[] = {
    { 0, "BR_EULER_XYZ_S" },
    { 8, "BR_EULER_XYX_S" },
    { 4, "BR_EULER_XZY_S" },
    { 12, "BR_EULER_XZX_S" },
    { 1, "BR_EULER_YZX_S" },
    { 9, "BR_EULER_YZY_S" },
    { 5, "BR_EULER_YXZ_S" },
    { 13, "BR_EULER_YXY_S" },
    { 2, "BR_EULER_ZXY_S" },
    { 10, "BR_EULER_ZXZ_S" },
    { 6, "BR_EULER_ZYX_S" },
    { 14, "BR_EULER_ZYZ_S" },
    { 16, "BR_EULER_ZYX_R" },
    { 24, "BR_EULER_XYX_R" },
    { 20, "BR_EULER_YZX_R" },
    { 28, "BR_EULER_XZX_R" },
    { 17, "BR_EULER_XZY_R" },
    { 25, "BR_EULER_YZY_R" },
    { 21, "BR_EULER_ZXY_R" },
    { 29, "BR_EULER_YXY_R" },
    { 18, "BR_EULER_YXZ_R" },
    { 26, "BR_EULER_ZXZ_R" },
    { 22, "BR_EULER_XYZ_R" },
    { 30, "BR_EULER_ZYZ_R" },
};

// GLOBAL: CARMA2_HW 0x00663b90
br_file_enum angle_order_F = { BR_ASIZE(angle_order_FM), angle_order_FM };

// GLOBAL: CARMA2_HW 0x00663b98
br_file_struct_member br_transform_euler_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_transform, t.euler.e.order), "t.euler.e.order", &angle_order_F },
    { DF_TYPE_BR_ANGLE, offsetof(br_transform, t.euler.e.a), "t.euler.e.a", NULL },
    { DF_TYPE_BR_ANGLE, offsetof(br_transform, t.euler.e.b), "t.euler.e.b", NULL },
    { DF_TYPE_BR_ANGLE, offsetof(br_transform, t.euler.e.c), "t.euler.e.c", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.euler.t), "t.euler.t", NULL },
};

// GLOBAL: CARMA2_HW 0x00663be8
br_file_struct br_transform_euler_F = { "br_transform_euler", BR_ASIZE(br_transform_euler_FM), br_transform_euler_FM, sizeof(br_transform) };

// GLOBAL: CARMA2_HW 0x00663bf8
br_file_struct_member br_transform_look_up_FM[] = {
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.look_up.look), "t.look_up.look", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.look_up.up), "t.look_up.up", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.look_up.t), "t.look_up.t", NULL },
};

// GLOBAL: CARMA2_HW 0x00663c28
br_file_struct br_transform_look_up_F = { "br_transform_look_up", BR_ASIZE(br_transform_look_up_FM), br_transform_look_up_FM, sizeof(br_transform) };

// GLOBAL: CARMA2_HW 0x00663c38
br_file_struct_member br_transform_translation_FM[] ={
    { DF_TYPE_BR_VECTOR3, offsetof(br_transform, t.translate.t), "t.translate.t", NULL },
};

// GLOBAL: CARMA2_HW 0x00663c48
br_file_struct br_transform_translation_F = { "br_transform_translation", BR_ASIZE(br_transform_translation_FM), br_transform_translation_FM, sizeof(br_transform) };

// GLOBAL: CARMA2_HW 0x00663c58
transform_type TransformTypes[] = {
    { CHUNKID_TRANSFORM_MAT34, &br_transform_matrix34_F },
    { CHUNKID_TRANSFORM_MAT34_LP, &br_transform_matrix34_F },
    { CHUNKID_TRANSFORM_QUAT, &br_transform_quat_F },
    { CHUNKID_TRANSFORM_EULER, &br_transform_euler_F },
    { CHUNKID_TRANSFORM_LOOK_UP, &br_transform_look_up_F },
    { CHUNKID_TRANSFORM_TRANSLATION, &br_transform_translation_F },
    { CHUNKID_TRANSFORM_IDENTITY, NULL },
};

// GLOBAL: CARMA2_HW 0x00663c90
br_file_struct_member br_bounds3_FM[] = {
    { DF_TYPE_BR_VECTOR3, offsetof(br_bounds3, min), "min", NULL },
    { DF_TYPE_BR_VECTOR3, offsetof(br_bounds3, max), "max", NULL },
};

// GLOBAL: CARMA2_HW 0x00663cb0
br_file_struct br_bounds3_F = { "br_bounds3", BR_ASIZE(br_bounds3_FM), br_bounds3_FM, sizeof(br_bounds3) };

// GLOBAL: CARMA2_HW 0x00663cc0
br_file_struct_member br_plane_FM[] ={
    { DF_TYPE_BR_VECTOR4, offsetof(br_vector4, v), "eqn", NULL },
};

// GLOBAL: CARMA2_HW 0x00663cd0
br_file_struct br_plane_F = { "br_plane", BR_ASIZE(br_plane_FM), br_plane_FM, sizeof(br_vector4) };

// GLOBAL: CARMA2_HW 0x00663ce0
br_file_enum_member light_type_FM[] = {
    { 0, "BR_LIGHT_POINT"},
    { 1, "BR_LIGHT_DIRECT"},
    { 2, "BR_LIGHT_SPOT"},
    { 4, "BR_LIGHT_VIEW|BR_LIGHT_POINT"},
    { 5, "BR_LIGHT_VIEW|BR_LIGHT_DIRECT"},
    { 6, "BR_LIGHT_VIEW|BR_LIGHT_SPOT"},
};

// GLOBAL: CARMA2_HW 0x00663d30
br_file_enum light_type_F = { BR_ASIZE(light_type_FM), light_type_FM };

// GLOBAL: CARMA2_HW 0x00663d38
br_file_struct_member br_light_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_light, type), "type", &light_type_F },
    { DF_TYPE_BR_COLOUR, offsetof(br_light, colour), "colour", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_light, attenuation_c), "attenuation_c", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_light, attenuation_l), "attenuation_l", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_light, attenuation_q), "attenuation_q", NULL },
    { DF_TYPE_BR_ANGLE, offsetof(br_light, cone_inner), "cone_inner", NULL },
    { DF_TYPE_BR_ANGLE, offsetof(br_light, cone_outer), "cone_outer", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_light, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x00663db8
br_file_struct br_light_F = { "br_light", BR_ASIZE(br_light_FM), br_light_FM, sizeof(br_light) };

// GLOBAL: CARMA2_HW 0x00663dc8
br_file_enum_member camera_type_FM[] = {
    { 0, "BR_CAMERA_PARALLEL"},
    { 1, "BR_CAMERA_PERSPECTIVE"},
};

// GLOBAL: CARMA2_HW 0x00663dd8
br_file_enum camera_type_F = { BR_ASIZE(camera_type_FM), camera_type_FM };

// GLOBAL: CARMA2_HW 0x00663de0
br_file_struct_member br_camera_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_camera, type), "type", &camera_type_F },
    { DF_TYPE_BR_ANGLE, offsetof(br_camera, field_of_view), "field_of_view", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_camera, hither_z), "hither_z", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_camera, yon_z), "yon_z", NULL },
    { DF_TYPE_BR_SCALAR, offsetof(br_camera, aspect), "aspect", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_camera, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x00663e40
br_file_struct br_camera_F = { "br_camera", BR_ASIZE(br_camera_FM), br_camera_FM, sizeof(br_camera) };

// GLOBAL: CARMA2_HW 0x00663e50
br_chunks_table_entry ModelLoadEntries[] = {
    { CHUNKID_END, 0u, FopRead_END },
    { CHUNKID_OLD_MATERIAL_INDEX, 0u, FopRead_OLD_MATERIAL_INDEX },
    { CHUNKID_OLD_VERTICES, 0u, FopRead_OLD_VERTICES },
    { CHUNKID_OLD_VERTICES_UV, 0u, FopRead_OLD_VERTICES_UV },
    { CHUNKID_OLD_FACES, 0u, FopRead_OLD_FACES },
    { CHUNKID_OLD_MODEL, 0u, FopRead_OLD_MODEL },
    { CHUNKID_OLD_FACES_1, 1u, FopRead_OLD_FACES_1 },
    { CHUNKID_OLD_MODEL_1, 0u, FopRead_OLD_MODEL_1 },
    { CHUNKID_MODEL, 0u, FopRead_MODEL },
    { CHUNKID_MATERIAL_INDEX, 1u, FopRead_MATERIAL_INDEX },
    { CHUNKID_MODEL_VERTICES, 1u, FopRead_VERTICES },
    { CHUNKID_MODEL_VERTEX_UV, 1u, FopRead_VERTEX_UV },
    { CHUNKID_MODEL_FACES, 1u, FopRead_FACES },
    { CHUNKID_MODEL_FACE_MATERIAL, 0u, FopRead_FACE_MATERIAL },
    { CHUNKID_MODEL_PIVOT, 0u, FopRead_PIVOT },
};

// GLOBAL: CARMA2_HW 0x00663f08
br_chunks_table ModelLoadTable = { BR_ASIZE(ModelLoadEntries), ModelLoadEntries };

// GLOBAL: CARMA2_HW 0x00663f10
br_chunks_table_entry ActorLoadEntries[] = {
    { CHUNKID_END, 0u, FopRead_END },
    { CHUNKID_ACTOR, 0u, FopRead_ACTOR },
    { CHUNKID_ACTOR_MODEL, 0u, FopRead_ACTOR_MODEL },
    { CHUNKID_ACTOR_TRANSFORM, 0u, FopRead_ACTOR_TRANSFORM },
    { CHUNKID_ACTOR_MATERIAL, 0u, FopRead_ACTOR_MATERIAL },
    { CHUNKID_ACTOR_LIGHT, 0u, FopRead_ACTOR_LIGHT },
    { CHUNKID_ACTOR_CAMERA, 0u, FopRead_ACTOR_CAMERA },
    { CHUNKID_ACTOR_BOUNDS, 0u, FopRead_ACTOR_BOUNDS },
    { CHUNKID_ACTOR_CLIP_PLANE, 0u, FopRead_ACTOR_CLIP_PLANE },
    { CHUNKID_ACTOR_ADD_CHILD, 0u, FopRead_ACTOR_ADD_CHILD },
    { CHUNKID_TRANSFORM_MAT34, 0u, FopRead_TRANSFORM },
    { CHUNKID_TRANSFORM_MAT34_LP, 0u, FopRead_TRANSFORM },
    { CHUNKID_TRANSFORM_QUAT, 0u, FopRead_TRANSFORM },
    { CHUNKID_TRANSFORM_EULER, 0u, FopRead_TRANSFORM },
    { CHUNKID_TRANSFORM_LOOK_UP, 0u, FopRead_TRANSFORM },
    { CHUNKID_TRANSFORM_TRANSLATION, 0u, FopRead_TRANSFORM },
    { CHUNKID_TRANSFORM_IDENTITY, 0u, FopRead_TRANSFORM },
    { CHUNKID_BOUNDS, 0u, FopRead_BOUNDS },
    { CHUNKID_LIGHT, 0u, FopRead_LIGHT },
    { CHUNKID_CAMERA, 0u, FopRead_CAMERA },
    { CHUNKID_PLANE, 0u, FopRead_PLANE },
};

// GLOBAL: CARMA2_HW 0x00664010
br_chunks_table ActorLoadTable = { BR_ASIZE(ActorLoadEntries), ActorLoadEntries };

// GLOBAL: CARMA2_HW 0x00664018
br_chunks_table_entry MaterialLoadEntries[] = {
    { CHUNKID_END, 0u, FopRead_END },
    { CHUNKID_MATERIAL_OLD, 0u, FopRead_MATERIAL_OLD },
    { CHUNKID_MATERIAL_COLOUR_MAP, 0u, FopRead_PIXELMAP_REF },
    { CHUNKID_MATERIAL_INDEX_BLEND, 0u, FopRead_PIXELMAP_REF },
    { CHUNKID_MATERIAL_INDEX_SHADE, 0u, FopRead_PIXELMAP_REF },
    { CHUNKID_MATERIAL_SCREENDOOR, 0u, FopRead_PIXELMAP_REF },
    { CHUNKID_MATERIAL_INDEX_FOG, 0u, FopRead_PIXELMAP_REF },
    { CHUNKID_MATERIAL, 0u, FopRead_MATERIAL },
};

// GLOBAL: CARMA2_HW 0x00664078
br_chunks_table MaterialLoadTable = { BR_ASIZE(MaterialLoadEntries), MaterialLoadEntries };

int C2_HOOK_STDCALL FopWrite_VERTICES(br_datafile* df, br_vertex* vertices, int nvertices) {

    df->prims->chunk_write(df, CHUNKID_MODEL_VERTICES, df->prims->struct_size(df, &br_vertex_F, NULL) * nvertices + df->prims->count_size(df));
    df->prims->count_write(df, nvertices);
    DfStructWriteArray(df, &br_vertex_F, vertices, nvertices);
    return 0;
}

#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern int C2_HOOK_CDECL BrFileRead();
extern br_pixelmap* C2_HOOK_CDECL BrMapFind();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern br_uint_32 C2_HOOK_CDECL BrModelCount();
extern br_uint_32 C2_HOOK_CDECL BrModelEnum();
extern void C2_HOOK_CDECL BrQsort();
extern void* C2_HOOK_CDECL BrResAllocate();
extern void C2_HOOK_CDECL BrResFree();
extern void* C2_HOOK_CDECL BrScratchAllocate();
extern void C2_HOOK_CDECL BrScratchFree();
extern br_pixelmap* C2_HOOK_CDECL BrTableFind();
extern void C2_HOOK_CDECL BrWarning();
extern void C2_HOOK_STDCALL DfClose(br_datafile* df);
extern br_datafile* C2_HOOK_STDCALL DfOpen(const char* name, int write, br_token scalar_type);
extern void* C2_HOOK_STDCALL DfPop(int type, int* countp);
extern void C2_HOOK_STDCALL DfPush(int type, void* value, unsigned int count);
extern br_uint_32 C2_HOOK_STDCALL DfStructReadArray(br_datafile* df, br_file_struct* str, void* base, int n);
extern br_uint_32 C2_HOOK_STDCALL DfStructWriteArray(br_datafile* df, br_file_struct* str, void* base, int n);
extern void* C2_HOOK_STDCALL DfTop(int type, int* countp);
extern int C2_HOOK_STDCALL FopWrite_END(br_datafile* df);
extern int C2_HOOK_STDCALL FopWrite_FILE_INFO(br_datafile* df, br_uint_32 type);
extern char IID_IDirectInputDevice2A[];
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x005232a0
int C2_HOOK_STDCALL FopRead_VERTICES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_model* mp;

    mp = DfTop(DF_MODEL, NULL);
    mp->vertices = BrResAllocate(mp, sizeof(br_vertex) * count, BR_MEMORY_VERTICES);
    DfStructReadArray(df, &br_vertex_F, mp->vertices, count);
    mp->nvertices = count;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005232f0
int C2_HOOK_STDCALL FopRead_OLD_VERTICES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    void *ptr;

    count = length / df->prims->struct_size(df, &br_vertex_F, NULL);
    ptr = BrResAllocate(v1db.res, count* sizeof(br_vertex), BR_MEMORY_VERTICES);
    DfStructReadArray(df, &br_vertex_F, ptr, count);
    DfPush(DF_VERTEX, ptr, count);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_VERTEX_UV(br_datafile* df, br_vertex* vertices, int nvertices) {

    df->prims->chunk_write(df, CHUNKID_MODEL_VERTEX_UV, df->prims->struct_size(df, &br_vertex_uv_F, NULL) * nvertices + df->prims->count_size(df));
    df->prims->count_write(df, nvertices);
    DfStructWriteArray(df, &br_vertex_uv_F, vertices, nvertices);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523350
int C2_HOOK_STDCALL FopRead_VERTEX_UV(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_model* mp;

    mp = DfTop(DF_MODEL, NULL);
    if (count > mp->nvertices) {
        BrFailure("Vertex UV: too many entries");
    }
    DfStructReadArray(df, &br_vertex_uv_F, mp->vertices, count);
    return 0;
}

// FUNCTION: CARMA2_HW 0x005233a0
int C2_HOOK_STDCALL FopRead_OLD_VERTICES_UV(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    void* ptr;

    count = length / df->prims->struct_size(df, &br_old_vertex_uv_F, NULL);
    ptr = BrResAllocate(v1db.res, count * sizeof(br_vertex), BR_MEMORY_VERTICES);
    DfStructReadArray(df, &br_old_vertex_uv_F, ptr, count);
    DfPush(DF_VERTEX, ptr, count);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523400
int C2_NAKED C2_HOOK_STDCALL FopRead_MATERIAL_INDEX(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    char name[256];
    br_material** mip;
    br_uint_32 i;

    mip = (br_material**)BrResAllocate(v1db.res, sizeof(intptr_t) * (count + 1), BR_MEMORY_MATERIAL_INDEX);
    mip[0] = NULL;
    for (i = 1; i < count + 1; i++) {
        df->prims->name_read(df, name);
        mip[i] = BrMaterialFind(name);
    }
    DfPush(DF_MATERIAL_INDEX, mip, count + 1);
    return 0;
#else
    __asm {
            sub esp, 0x104
            mov eax, dword ptr [esp + 0x114]
            mov edx, dword ptr [v1db+0x4b4]
            push ebp
            push 9
            lea ebp, [eax + 1]
            lea ecx, [ebp*4]
            push ecx
            push edx
            call BrResAllocate
            add esp, 0xc
            cmp ebp, 1
            mov dword ptr [esp + 4], eax
            mov dword ptr [eax], 0
            jbe L0052346d
            push edi
            mov edi, dword ptr [esp + 0x110]
            push esi
            push ebx
            lea esi, [eax + 4]
            lea ebx, [ebp - 1]
    L00523449:
            mov eax, dword ptr [edi + 4]
            lea ecx, [esp + 0x14]
            push ecx
            push edi
            call dword ptr [eax + 0x38]
            push eax
            call BrMaterialFind
            mov dword ptr [esi], eax
            add esp, 4
            add esi, 4
            dec ebx
            jne L00523449
            mov eax, dword ptr [esp + 0x10]
            pop ebx
            pop esi
            pop edi
    L0052346d:
            push ebp
            push eax
            push 5
            call DfPush
            xor eax, eax
            pop ebp
            add esp, 0x104
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

int C2_HOOK_STDCALL FopWrite_MATERIAL_INDEX(br_datafile* df, br_material** materials, int nmaterials) {
    int i;
    int s;

    s = df->prims->count_size(df);
    for (i = 0; i < nmaterials; i++) {
        s += df->prims->name_size(df, materials[i]->identifier);
    }
    df->prims->chunk_write(df, CHUNKID_MATERIAL_INDEX, s);
    df->prims->count_write(df, nmaterials);
    for (i = 0; i < nmaterials; i++) {
        df->prims->name_write(df, materials[i]->identifier);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523490
int C2_NAKED C2_HOOK_STDCALL FopRead_OLD_MATERIAL_INDEX(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    char* mblock;
    char* cp;
    int i;
    int num_materials;
    br_material** mip;

    mblock = BrScratchAllocate(length);
    if (BrFileRead(mblock, 1, length, df->h) != length) {
        BrFailure("could not read material index");
    }
    for (i = 0, cp = mblock, num_materials = 0; i < (int)length; i++) {
        if (*cp++ == '\0') {
            num_materials += 1;
        }
    }
    mip = BrResAllocate(v1db.res, num_materials * sizeof(*mip), BR_MEMORY_MATERIAL_INDEX);
    cp = mblock;
    for (i = 0; i < (int)length; i++) {
        mip[i] = BrMaterialFind(cp);
        while (*cp++ != '\0') {
        }
    }
    BrScratchFree(mblock);
    DfPush(DF_MATERIAL_INDEX, mip, count);
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x18]
            push edi
            push esi
            call BrScratchAllocate
            mov edi, eax
            mov eax, dword ptr [esp + 0x18]
            add esp, 4
            mov dword ptr [esp + 0x1c], edi
            mov ecx, dword ptr [eax]
            push ecx
            push esi
            push 1
            push edi
            call BrFileRead
            add esp, 0x10
            cmp eax, esi
            je L005234cb
            push offset MaterialLoadTable+0xb4
            call BrFailure
            add esp, 4
    L005234cb:
            xor ebp, ebp
            mov eax, edi
            test esi, esi
            jle L005234de
    L005234d3:
            mov dl, byte ptr [eax]
            inc eax
            test dl, dl
            jne L005234db
            inc ebp
    L005234db:
            dec esi
            jne L005234d3
    L005234de:
            mov ecx, dword ptr [v1db+0x4b4]
            lea eax, [ebp*4]
            push 9
            push eax
            push ecx
            call BrResAllocate
            add esp, 0xc
            mov dword ptr [esp + 0x14], eax
            test ebp, ebp
            mov esi, edi
            jle L00523528
            mov edi, eax
            mov ebx, ebp
    L00523505:
            push esi
            call BrMaterialFind
            mov dword ptr [edi], eax
            mov dl, byte ptr [esi]
            add esp, 4
            inc esi
            test dl, dl
            je L0052351e
    L00523517:
            mov al, byte ptr [esi]
            inc esi
            test al, al
            jne L00523517
    L0052351e:
            add edi, 4
            dec ebx
            jne L00523505
            mov edi, dword ptr [esp + 0x1c]
    L00523528:
            push edi
            call BrScratchFree
            mov ecx, dword ptr [esp + 0x18]
            add esp, 4
            push ebp
            push ecx
            push 5
            call DfPush
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00523550
int C2_HOOK_STDCALL FopRead_FACES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_model* mp;
    int i;

    C2_HOOK_BUG_ON(sizeof(br_face) != 40);
    C2_HOOK_ASSERT(count <= 0xffff);

    mp = DfTop(DF_MODEL, NULL);
    mp->faces = (br_face*)BrResAllocate(mp, sizeof(br_face) * count, BR_MEMORY_FACES);
    mp->nfaces = (br_uint_16)count;
    DfStructReadArray(df, &br_face_F, mp->faces, mp->nfaces);
    for (i = 0; i < mp->nfaces; i++) {
        if (mp->faces[i].smoothing == 0) {
            mp->faces[i].smoothing = -1;
        }
        // Differs slightly from DethRace here
    }
    return 0;
}

int C2_HOOK_STDCALL FopWrite_FACES(br_datafile* df, br_face* faces, int nfaces) {

    df->prims->chunk_write(df, CHUNKID_MODEL_FACES, df->prims->struct_size(df, &br_face_F, NULL) * nfaces + df->prims->count_size(df));
    df->prims->count_write(df, nfaces);
    DfStructWriteArray(df, &br_face_F, faces, nfaces);
    return 0;
}

// FUNCTION: CARMA2_HW 0x005235d0
int C2_HOOK_STDCALL FopRead_OLD_FACES_1(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_model* mp;
    int i;

    mp = DfTop(DF_MODEL, NULL);
    mp->faces = BrResAllocate(mp, count * sizeof(br_face), BR_MEMORY_FACES);
    mp->nfaces = count;
    DfStructReadArray(df, &br_old_face_1_F, mp->faces, (br_uint_16)count);
    for (i = 0; i < mp->nfaces; i++) {
        if (mp->faces[i].smoothing == 0) {
            mp->faces[i].smoothing = -1;
        } else {
            mp->faces[i].smoothing = 1 << ((mp->faces[i].smoothing - 1) % 16);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523670
int C2_NAKED C2_HOOK_STDCALL FopRead_OLD_FACES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    br_face* fp;
    br_material** mip;
    int mi_count;
    int i;
    void* ptr;

    ptr = DfPop(DF_VERTEX, (int*)&i);
    mip = DfPop(DF_MATERIAL_INDEX, &mi_count);
    DfPush(DF_VERTEX, ptr, i);
    mi_count = length / df->prims->struct_size(df, &br_old_face_F, NULL);
    fp = BrResAllocate(v1db.res, sizeof(br_face) * mi_count, BR_MEMORY_FACES);
    DfStructReadArray(df, &br_old_face_F, fp, mi_count);
    for (i = 0; i < mi_count; i++) {
        union u_mat_u16 {
            br_material mat;
            br_uint_16 u16;
        };
        fp[i].material = mip[((union u_mat_u16*)&fp[i].material)->u16];
        if (fp[i].smoothing == 0) {
            fp[i].smoothing = -1;
        }
    }
    BrResFree(mip);
    DfPush(DF_FACE, fp, mi_count);
    return 0;
#else
    __asm {
            push ecx
            push ebx
            mov ebx, dword ptr [esp + 0xc]
            push ebp
            push esi
            mov eax, dword ptr [ebx + 4]
            push edi
            push 0
            push offset br_old_face_F
            push ebx
            call dword ptr [eax + 0x24]
            mov ecx, eax
            xor edx, edx
            mov eax, dword ptr [esp + 0x20]
            div ecx
            lea edx, [esp + 0x18]
            push edx
            push 6
            mov edi, eax
            call DfPop
            mov esi, eax
            lea eax, [esp + 0x10]
            push eax
            push 5
            call DfPop
            mov ebp, eax
            mov ecx, dword ptr [esp + 0x18]
            push ecx
            push esi
            push 6
            call DfPush
            mov eax, dword ptr [v1db+0x4b4]
            lea edx, [edi + edi*4]
            shl edx, 3
            push 5
            push edx
            push eax
            call BrResAllocate
            add esp, 0xc
            mov esi, eax
            push edi
            push esi
            push offset br_old_face_F
            push ebx
            call DfStructReadArray
            xor eax, eax
            test edi, edi
            mov dword ptr [esp + 0x18], eax
            jbe L00523725
    L005236ec:
            lea ecx, [eax + eax*4]
            xor edx, edx
            mov dx, word ptr [esi + ecx*8 + 8]
            lea eax, [esi + ecx*8 + 8]
            mov ecx, dword ptr [ebp + edx*4]
            mov dword ptr [eax], ecx
            mov eax, dword ptr [esp + 0x18]
            lea edx, [eax + eax*4]
            cmp word ptr [esi + edx*8 + 6], 0
            lea ecx, [esi + edx*8 + 6]
            jne L0052371c
            mov word ptr [ecx], 0xffff
            mov eax, dword ptr [esp + 0x18]
    L0052371c:
            inc eax
            cmp eax, edi
            mov dword ptr [esp + 0x18], eax
            jb L005236ec
    L00523725:
            push ebp
            call BrResFree
            add esp, 4
            push edi
            push esi
            push 7
            call DfPush
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            pop ecx
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

int C2_HOOK_STDCALL FopWrite_FACE_MATERIAL(br_datafile* df, br_face* faces, int nfaces, br_material** mindex, int nmaterials) {
    br_uint_16* block;
    int i;
    int j;

    FopWrite_MATERIAL_INDEX(df, mindex, nmaterials);
    block = BrResAllocate(v1db.res, nfaces * sizeof(br_uint_16), BR_MEMORY_MATERIAL_INDEX);
    for (i = 0; i < nfaces; i++) {
        block[i] = 0;
        if (faces[i].material != NULL) {
            for (j = 0; j < nmaterials; j++) {
                if (faces[i].material == mindex[j]) {
                    block[i] = j + 1;
                    break;
                }
            }
        }
    }
    df->prims->chunk_write(df, CHUNKID_MODEL_FACE_MATERIAL, df->prims->block_size(df, block, nfaces, 0, 1, sizeof(br_uint_16)));
    df->prims->block_write(df, block, nfaces, 0, 1, sizeof(br_uint_16));
    BrResFree(block);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523750
int C2_HOOK_STDCALL FopRead_FACE_MATERIAL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_material** mindex;
    int nmaterials;
    br_model* mp;
    br_face* fp;
    br_uint_16* block;
    br_uint_16* ip;
    int block_count;
    int i;

    mindex = DfPop(DF_MATERIAL_INDEX, &nmaterials);
    mp = DfTop(DF_MODEL, NULL);

    block = BrScratchAllocate(length);
    block_count = mp->nfaces;
    block = df->prims->block_read(df, block, &block_count, sizeof(br_uint_16), 0);

    if (block_count > mp->nfaces) {
        BrFailure("Face Materials: too many entries");
    }

    fp = mp->faces;
    ip = block;
    for (i = 0; i < block_count; i++, fp++, ip++) {
        fp->material = (*ip < nmaterials) ? mindex[*ip] : NULL;
    }

    BrScratchFree(block);
    BrResFree(mindex);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523810
int C2_NAKED C2_HOOK_STDCALL FopRead_MODEL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* mp;

    mp = BrModelAllocate(NULL, 0, 0);
    mp->identifier = NULL;
    df->res = mp;
    df->prims->struct_read(df, &br_model_F, mp);
    df->res = NULL;
    mp->flags &= (BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_GENERATE_TAGS | BR_MODF_QUICK_UPDATE);
    DfPush(DF_MODEL, mp, 1);
    return 0;
#else
    __asm {
            push esi
            push edi
            push 0
            push 0
            push 0
            call BrModelAllocate
            mov edi, dword ptr [esp + 0x18]
            add esp, 0xc
            mov esi, eax
            mov eax, dword ptr [edi + 4]
            push esi
            push offset br_model_F
            push edi
            mov dword ptr [edi + 0xc], esi
            call dword ptr [eax + 0x20]
            push 1
            push esi
            mov dword ptr [edi + 0xc], 0
            and word ptr [esi + 0x20], 0x17
            push 8
            call DfPush
            pop edi
            xor eax, eax
            pop esi
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

int C2_HOOK_STDCALL FopWrite_MODEL(br_datafile* df, br_model* mp) {
    br_model temp_model;

    memcpy(&temp_model, mp, sizeof(br_model));
    df->prims->chunk_write(df, CHUNKID_MODEL, df->prims->struct_size(df, &br_model_F, mp));
    df->prims->struct_write(df, &br_model_F, mp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523860
int C2_NAKED C2_HOOK_STDCALL FopRead_OLD_MODEL_1(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* mp;

    mp = BrModelAllocate(NULL, 0, 0);
    df->res = mp;
    df->prims->struct_read(df, &br_old_model_1_F, mp);
    DfPush(DF_MODEL, df->res, 1);
    return 0;
#else
    __asm {
            push esi
            push edi
            push 0
            push 0
            push 0
            call BrModelAllocate
            mov edi, dword ptr [esp + 0x18]
            add esp, 0xc
            mov esi, eax
            mov eax, dword ptr [edi + 4]
            push esi
            push offset br_old_model_1_F
            push edi
            mov dword ptr [edi + 0xc], esi
            call dword ptr [eax + 0x20]
            push 1
            push esi
            push 8
            mov dword ptr [edi + 0xc], 0
            call DfPush
            pop edi
            xor eax, eax
            pop esi
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005238a0
int C2_HOOK_STDCALL FopRead_OLD_MODEL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_model* mp;
    int i;

    mp = BrModelAllocate(NULL, 0, 0);
    df->res = mp;
    df->prims->struct_read(df, &br_model_F, mp);
    df->res = NULL;
    mp->faces = DfPop(DF_FACE, &i);
    mp->nfaces = i;
    mp->vertices = DfPop(DF_VERTEX, &i);
    mp->nvertices = i;
    BrResAdd(mp, mp->faces);
    BrResAdd(mp, mp->vertices);
    mp->flags = mp->flags & (BR_STATE_SURFACE | BR_STATE_MATRIX | BR_STATE_ENABLE | BR_STATE_CLIP);
    DfPush(DF_MODEL, mp, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523930
int C2_HOOK_STDCALL FopRead_PIVOT(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_model* mp;

    mp = DfPop(DF_MODEL, NULL);
    df->res = mp;
    df->prims->struct_read(df, &br_pivot_F, mp);
    df->res = NULL;
    DfPush(DF_MODEL, mp, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523970
int C2_HOOK_STDCALL FopRead_MATERIAL_OLD(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_material* mp;
    mp = BrMaterialAllocate(NULL);
    df->res = mp;
    df->prims->struct_read(df, &br_material_old_F, mp);
    df->res = NULL;
    DfPush(DF_MATERIAL, mp, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x005239b0
int C2_HOOK_STDCALL FopRead_MATERIAL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_material* mp;

    mp = BrMaterialAllocate(NULL);
    df->res = mp;
    df->prims->struct_read(df, &br_material_F, mp);
    df->res = NULL;
    DfPush(DF_MATERIAL, mp, 1);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_MATERIAL(br_datafile* df, br_material* mp) {

    df->prims->chunk_write(df, CHUNKID_MATERIAL, df->prims->struct_size(df, &br_material_F, mp));
    df->prims->struct_write(df, &br_material_F, mp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x005239f0
int C2_NAKED C2_HOOK_STDCALL FopRead_PIXELMAP_REF(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap* pm;
    char name[256];
    char* mp;
    int i;

    for (i = 0; i < BR_ASIZE(MaterialMaps); i++) {
        if (id == MaterialMaps[i].id) {
            break;
        }
    }

    if (i >= BR_ASIZE(MaterialMaps)) {
        // FIXME: error message
        abort();
    }

    mp = DfTop(DF_MATERIAL, NULL);
    df->prims->name_read(df, name);
    if (MaterialMaps[i].table != 0) {
        pm = BrTableFind(name);
    } else {
        pm = BrMapFind(name);
    }
#ifndef CARPOCALYPSE2_MATCHING
    if (pm == NULL && MaterialMaps[i].table == 0) {
        /* Data packs reference map/texture names with a trailing ".PIX"
         * (e.g. "ROAD1.PIX"), while the loaded pixelmaps are registered under
         * their bare stem ("ROAD1"). Retry once with the extension stripped. */
        size_t ln = strlen(name);
        if (ln > 4 &&
            (name[ln - 4] == '.' && (name[ln - 3] | 0x20) == 'p' &&
             (name[ln - 2] | 0x20) == 'i' && (name[ln - 1] | 0x20) == 'x')) {
            char trimmed[256];
            memcpy(trimmed, name, ln - 4);
            trimmed[ln - 4] = '\0';
            pm = BrMapFind(trimmed);
        }
    }
#endif
    *(intptr_t*)(mp + MaterialMaps[i].offset) = (intptr_t)pm;
    return 0;
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            sub esp, 0x100
            mov eax, offset MaterialMaps
            push esi
            push edi
            xor esi, esi
    L00523a03:
            cmp ecx, dword ptr [eax]
            je L00523a12
            add eax, 0xc
            inc esi
            cmp eax, offset MaterialMaps+0x3c
            jb L00523a03
    L00523a12:
            push 0
            push 3
            call DfTop
            lea esi, [esi + esi*2]
            mov edi, eax
            shl esi, 2
            lea edx, [esp + 8]
            mov eax, dword ptr [esi + MaterialMaps+0x8]
            push edx
            test eax, eax
            mov eax, dword ptr [esp + 0x110]
            push eax
            mov ecx, dword ptr [eax + 4]
            je L00523a48
            call dword ptr [ecx + 0x38]
            push eax
            call BrTableFind
            jmp L00523a51
    L00523a48:
            call dword ptr [ecx + 0x38]
            push eax
            call BrMapFind
    L00523a51:
            mov ecx, dword ptr [esi + MaterialMaps+0x4]
            add esp, 4
            mov dword ptr [ecx + edi], eax
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x100
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

int C2_HOOK_STDCALL FopWrite_PIXELMAP_REF(br_datafile* df, int id, br_pixelmap* pixelmap) {

    df->prims->chunk_write(df, id, df->prims->name_size(df, pixelmap->identifier));
    df->prims->name_write(df, pixelmap->identifier);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR(br_datafile* df, br_actor* ap) {

    df->prims->chunk_write(df, CHUNKID_ACTOR, df->prims->struct_size(df, &br_actor_F, ap));
    df->prims->struct_write(df, &br_actor_F, ap);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523a70
int C2_HOOK_STDCALL FopRead_ACTOR(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* ap;

    ap = BrActorAllocate(BR_ACTOR_NONE, NULL);
    df->res = ap;
    df->prims->struct_read(df, &br_actor_F, ap);
    df->res = NULL;
    ap->t.type = BR_TRANSFORM_IDENTITY;
    DfPush(DF_ACTOR, ap, 1);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_MODEL(br_datafile* df, br_model* model) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_MODEL, df->prims->name_size(df, model->identifier));
    df->prims->name_write(df, model->identifier);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523ac0
int C2_HOOK_STDCALL FopRead_ACTOR_MODEL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    char name[256];
    br_actor* a;

    a = DfTop(DF_ACTOR, NULL);
    a->model = BrModelFind(df->prims->name_read(df, name));
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_MATERIAL(br_datafile* df, br_material* material) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_MATERIAL, df->prims->name_size(df, material->identifier));
    df->prims->name_write(df, material->identifier);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523b00
int C2_HOOK_STDCALL FopRead_ACTOR_MATERIAL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    char name[256];
    br_actor* a;

    a = DfTop(DF_ACTOR, NULL);
    a->material = BrMaterialFind(df->prims->name_read(df, name));
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_TRANSFORM(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_TRANSFORM, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523b40
int C2_HOOK_STDCALL FopRead_ACTOR_TRANSFORM(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* a;
    br_transform* tp;

    tp = DfPop(DF_TRANSFORM, NULL);
    a = DfTop(DF_ACTOR, NULL);
    memcpy(&a->t, tp, sizeof(br_transform));
    BrResFree(tp);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_LIGHT(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_LIGHT, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523b80
int C2_HOOK_STDCALL FopRead_ACTOR_LIGHT(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* a;
    br_light* lp;

    lp = DfPop(DF_LIGHT, NULL);
    a = DfTop(DF_ACTOR, NULL);
    a->type_data = lp;
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_CAMERA(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_CAMERA, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523ba0
int C2_HOOK_STDCALL FopRead_ACTOR_CAMERA(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* a;
    br_light* cp;

    cp = DfPop(DF_CAMERA, NULL);
    a = DfTop(DF_ACTOR, NULL);
    a->type_data = cp;
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_BOUNDS(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_BOUNDS, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523bc0
int C2_HOOK_STDCALL FopRead_ACTOR_BOUNDS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* a;
    br_bounds* bp;

    bp = DfPop(DF_BOUNDS, NULL);
    a = DfTop(DF_ACTOR, NULL);
    a->type_data = bp;
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_CLIP_PLANE(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_CLIP_PLANE, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523be0
int C2_HOOK_STDCALL FopRead_ACTOR_CLIP_PLANE(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* a;
    br_vector4* vp;

    vp = DfPop(DF_CLIP_PLANE, NULL);
    a = DfTop(DF_ACTOR, NULL);
    a->type_data = vp;
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ACTOR_ADD_CHILD(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_ACTOR_ADD_CHILD, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523c00
int C2_HOOK_STDCALL FopRead_ACTOR_ADD_CHILD(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_actor* a;
    br_actor* p;

    a = DfPop(DF_ACTOR, NULL);
    p = DfTop(DF_ACTOR, NULL);
    BrActorAdd(p, a);

    return 0;
}

int C2_HOOK_STDCALL FopWrite_TRANSFORM(br_datafile* df, br_transform* t) {
    transform_type* tt;

    tt = &TransformTypes[t->type];
    if (tt->fs == NULL) {
        df->prims->chunk_write(df, tt->id, 0);
    } else {
        df->prims->chunk_write(df, tt->id, df->prims->struct_size(df, tt->fs, t));
        df->prims->struct_size(df, tt->fs, &t);
        df->prims->struct_write(df, tt->fs, t);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523c30
int C2_NAKED C2_HOOK_STDCALL FopRead_TRANSFORM(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
#ifndef CARPOCALYPSE2_MATCHING
    int t;
    br_transform* tp;

    for (t = 0; t < BR_ASIZE(TransformTypes); t++) {
        if (id == TransformTypes[t].id) {
            break;
        }
    }
    if (t >= BR_ASIZE(TransformTypes)) {
        abort(); // FIXME: LOG_PANIC("transform type not found!");
    }
    tp = (br_transform*)BrResAllocate(v1db.res, sizeof(br_transform), BR_MEMORY_TRANSFORM);
    tp->type = t;
    df->res = tp;
    if (TransformTypes[t].fs) {
        df->prims->struct_read(df, TransformTypes[t].fs, tp);
    }
    df->res = NULL;
    DfPush(DF_TRANSFORM, tp, 1);
    return 0;
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            push ebx
            push esi
            push edi
            xor esi, esi
            mov eax, offset TransformTypes
    L00523c3e:
            cmp dword ptr [eax], ecx
            je L00523c4d
            add eax, 8
            inc esi
            cmp eax, offset br_bounds3_FM
            jb L00523c3e
    L00523c4d:
            mov eax, dword ptr [v1db+0x4b4]
            push 0x13
            push 0x34
            push eax
            call BrResAllocate
            mov ebx, dword ptr [esp + 0x1c]
            mov edi, eax
            add esp, 0xc
            mov word ptr [edi], si
            mov dword ptr [ebx + 0xc], edi
            mov esi, dword ptr [esi*8 + TransformTypes+0x4]
            test esi, esi
            je L00523c7f
            mov ecx, dword ptr [ebx + 4]
            push edi
            push esi
            push ebx
            call dword ptr [ecx + 0x20]
    L00523c7f:
            push 1
            push edi
            push 0x10
            mov dword ptr [ebx + 0xc], 0
            call DfPush
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

int C2_HOOK_STDCALL FopWrite_BOUNDS(br_datafile* df, br_bounds* bp) {

    df->prims->chunk_write(df, CHUNKID_BOUNDS, df->prims->struct_size(df, &br_bounds3_F, bp));
    df->prims->struct_write(df, &br_bounds3_F, bp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523ca0
int C2_HOOK_STDCALL FopRead_BOUNDS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_bounds3* bp;

    bp = BrResAllocate(v1db.res, sizeof(br_bounds3), BR_MEMORY_BOUNDS);
    df->res = bp;
    df->prims->struct_read(df, &br_bounds3_F, bp);
    df->res = NULL;
    DfPush(DF_BOUNDS, bp, 1);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_PLANE(br_datafile* df, br_vector4* pp) {

    df->prims->chunk_write(df, CHUNKID_PLANE, df->prims->struct_size(df, &br_plane_F, pp));
    df->prims->struct_write(df, &br_plane_F, pp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523cf0
int C2_HOOK_STDCALL FopRead_PLANE(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_vector4* pp;

    pp = BrResAllocate(v1db.res, sizeof(br_vector4), BR_MEMORY_CLIP_PLANE);
    df->res = pp;
    df->prims->struct_read(df, &br_plane_F, pp);
    df->res = NULL;
    DfPush(DF_CLIP_PLANE, pp, 1);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_LIGHT(br_datafile* df, br_light* lp) {

    df->prims->chunk_write(df, CHUNKID_LIGHT, df->prims->struct_size(df, &br_light_F, lp));
    df->prims->struct_write(df, &br_light_F, lp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523d40
int C2_HOOK_STDCALL FopRead_LIGHT(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_light* lp;

    lp = BrResAllocate(v1db.res, sizeof(br_light), BR_MEMORY_LIGHT);
    df->res = lp;
    df->prims->struct_read(df, &br_light_F, lp);
    df->res = NULL;
    DfPush(DF_LIGHT, lp, 1);
    return 0;
}

int C2_HOOK_STDCALL FopWrite_CAMERA(br_datafile* df, br_camera* cp) {

    df->prims->chunk_write(df, CHUNKID_CAMERA, df->prims->struct_size(df, &br_camera_F, cp));
    df->prims->struct_write(df, &br_camera_F, cp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523d90
int C2_HOOK_STDCALL FopRead_CAMERA(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_camera* cp;

    cp = BrResAllocate(v1db.res, sizeof(br_camera), BR_MEMORY_CAMERA);
    df->res = cp;
    df->prims->struct_read(df, &br_camera_F, cp);
    df->res = NULL;
    DfPush(DF_CAMERA, cp, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00523de0
br_uint_32 C2_HOOK_CDECL BrModelLoadMany(char* filename, br_model** models, br_uint_16 num) {
    int count;
    int r;
    br_datafile* df;

    df = DfOpen(filename, 0, BRT_FLOAT);
    if (df == NULL) {
        return 0;
    }
    count = 0;
    do {
        if (count >= num) {
            break;
        }
        r = DfChunksInterpret(df, &ModelLoadTable);
        if (DfTopType() == DF_MODEL) {
            models[count] = DfPop(DF_MODEL, NULL);
            count++;
        }
    } while (r != 0);
    DfClose(df);
    return count;
}

// FUNCTION: CARMA2_HW 0x00524310
int C2_HOOK_CDECL PtrCompare(void* a, void* b) {

    if (*(uintptr_t*)a > *(uintptr_t*)b) {
        return -1;
    }
    return *(uintptr_t*)a < *(uintptr_t*)b;
}

// FUNCTION: CARMA2_HW 0x00523ee0
br_uint_32 C2_NAKED C2_HOOK_CDECL WriteModel(br_model* mp, br_datafile* df) {
#ifndef CARPOCALYPSE2_MATCHING
    br_material** mindex;
    int nmaterials;
    int i;
    int has_uv;

    nmaterials = 0;
    if (mp->vertices == NULL || mp->faces == NULL) {
        if (mp->identifier != NULL) {
            BrWarning(mp->identifier);
        }
        BrWarning("Model must have vertex and face information to save.");
        BrWarning("This information may have neem stripped on addition to");
        BrWarning("the registry. To prevent this, set the BR_MODF_UPDATEABLE");
        BrFailure("flag before adding the model to the registry.");
    }
    mindex = BrResAllocate(v1db.res, mp->nfaces * sizeof(br_material*), BR_MEMORY_MATERIAL_INDEX);
    for (i = 0; i < mp->nfaces; i++) {
        mindex[i] = mp->faces[i].material;
    }
    BrQsort(mindex, mp->nfaces, sizeof(br_material*), (br_qsort_cbfn*)PtrCompare);
    nmaterials = 1;
    for (i = 1; i < mp->nfaces; i++) {
        if (mindex[nmaterials-1] == mindex[i]) {
            mindex[nmaterials] = mindex[i];
            nmaterials++;
        }
    }
    if (mindex[nmaterials-1] == NULL) {
        nmaterials = 0;
    }
    has_uv = 0;
    for (i = 0; i < mp->nvertices; i++) {
        if (mp->vertices[i].map.v[0] != 0.f || mp->vertices[i].map.v[1] != 0.f) {
            has_uv = 1;
            break;
        }
    }

    FopWrite_MODEL(df, mp);
    if (mp->nvertices != 0) {
        FopWrite_VERTICES(df, mp->vertices, mp->nvertices);
        if (has_uv != 0) {
            FopWrite_VERTEX_UV(df, mp->vertices, mp->nvertices);
        }
    }
    if (mp->nfaces != 0) {
        FopWrite_FACES(df, mp->faces, mp->nfaces);
        if (nmaterials != 0) {
            FopWrite_FACE_MATERIAL(df, mp->faces, mp->nfaces, mindex, nmaterials);
        }
    }
    FopWrite_END(df);
    BrResFree(mindex);
    return 0;
#else
    __asm {
            sub esp, 0x68
            xor eax, eax
            push ebx
            mov ebx, dword ptr [esp + 0x70]
            push esi
            push edi
            mov esi, dword ptr [ebx + 0xc]
            mov edi, dword ptr [ebx + 8]
            test esi, esi
            sete al
            xor ecx, ecx
            test edi, edi
            sete cl
            or eax, ecx
            je L00523f4f
            mov ebx, dword ptr [ebx + 4]
            test ebx, ebx
            je L00523f12
            push ebx
            call BrWarning
            add esp, 4
    L00523f12:
            push offset MaterialLoadTable+0x9f8
            call BrWarning
            add esp, 4
            push offset MaterialLoadTable+0xa30
            call BrWarning
            add esp, 4
            push offset MaterialLoadTable+0xa68
            call BrWarning
            add esp, 4
            push offset MaterialLoadTable+0xaa4
            call BrFailure
            add esp, 4
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x68
            ret
    L00523f4f:
            push ebp
            mov ebp, 1
            test esi, esi
            je L00524002
            mov eax, dword ptr [v1db+0x4b4]
            xor edx, edx
            mov dx, word ptr [ebx + 0x12]
            push 9
            shl edx, 2
            push edx
            push eax
            call BrResAllocate
            xor edx, edx
            add esp, 0xc
            cmp word ptr [ebx + 0x12], dx
            mov esi, eax
            mov dword ptr [esp + 0x10], esi
            jbe L00523fa4
            xor eax, eax
            mov ecx, esi
    L00523f89:
            mov edi, dword ptr [ebx + 0xc]
            inc edx
            add ecx, 4
            mov edi, dword ptr [eax + edi + 8]
            add eax, 0x28
            mov dword ptr [ecx - 4], edi
            xor edi, edi
            mov di, word ptr [ebx + 0x12]
            cmp edx, edi
            jl L00523f89
    L00523fa4:
            xor ecx, ecx
            push offset PtrCompare
            mov cx, word ptr [ebx + 0x12]
            push 4
            push ecx
            push esi
            call BrQsort
            add esp, 0x10
            cmp word ptr [ebx + 0x12], bp
            mov edi, ebp
            mov esi, ebp
            mov dword ptr [esp + 0x7c], edi
            jbe L00523ff1
            mov edx, dword ptr [esp + 0x10]
            add edx, 4
            mov ecx, edx
    L00523fd2:
            mov eax, dword ptr [ecx]
            cmp eax, dword ptr [ecx - 4]
            je L00523fdf
            mov dword ptr [edx], eax
            inc edi
            add edx, 4
    L00523fdf:
            xor eax, eax
            inc esi
            mov ax, word ptr [ebx + 0x12]
            add ecx, 4
            cmp esi, eax
            jl L00523fd2
            mov dword ptr [esp + 0x7c], edi
    L00523ff1:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ecx + edi*4 - 4]
            test eax, eax
            jne L00524002
            dec edi
            mov dword ptr [esp + 0x7c], edi
    L00524002:
            mov eax, dword ptr [ebx + 8]
            xor ecx, ecx
            mov cx, word ptr [ebx + 0x10]
            xor edx, edx
            cmp ecx, edx
            mov dword ptr [esp + 0x14], edx
            jle L00524045
            lea esi, [eax + 0x10]
    L00524018:
            fld dword ptr [esi - 4]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x444]
            fnstsw ax
            test ah, 0x40
            je L00524041
            _emit 0xd9
        _emit 0x06
            fcomp dword ptr [IID_IDirectInputDevice2A+0x444]
            fnstsw ax
            test ah, 0x40
            je L00524041
            inc edx
            add esi, 0x28
            cmp edx, ecx
            jl L00524018
            jmp L00524045
    L00524041:
            mov dword ptr [esp + 0x14], ebp
    L00524045:
            mov ecx, 0x17
            mov esi, ebx
            lea edi, [esp + 0x1c]
            lea edx, [esp + 0x1c]
            rep movsd
            mov esi, dword ptr [esp + 0x80]
            and word ptr [esp + 0x3c], 0x17
            push edx
            push offset br_model_F
            mov edi, dword ptr [esi + 4]
            push esi
            call dword ptr [edi + 0x24]
            push eax
            push 0x36
            push esi
            call dword ptr [edi + 8]
            mov eax, dword ptr [esi + 4]
            lea ecx, [esp + 0x1c]
            push ecx
            push offset br_model_F
            push esi
            call dword ptr [eax + 0x1c]
            mov ax, word ptr [ebx + 0x10]
            test ax, ax
            je L0052414c
            mov edx, dword ptr [ebx + 8]
            and eax, 0xffff
            mov edi, eax
            mov eax, dword ptr [esi + 4]
            push 0
            push offset br_vertex_F
            push esi
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x8c], eax
            call dword ptr [eax + 0x24]
            mov ebp, eax
            mov eax, dword ptr [esi + 4]
            push esi
            imul ebp, edi
            call dword ptr [eax + 0x18]
            mov ecx, dword ptr [esp + 0x80]
            add ebp, eax
            push ebp
            push 0x17
            push esi
            call dword ptr [ecx + 8]
            mov edx, dword ptr [esi + 4]
            push edi
            push esi
            call dword ptr [edx + 0x10]
            mov eax, dword ptr [esp + 0x18]
            push edi
            push eax
            push offset br_vertex_F
            push esi
            call DfStructWriteArray
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0052414c
            mov ecx, dword ptr [ebx + 8]
            mov eax, dword ptr [esi + 4]
            push 0
            xor edi, edi
            mov di, word ptr [ebx + 0x10]
            push offset br_vertex_uv_F
            push esi
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x8c], eax
            call dword ptr [eax + 0x24]
            mov edx, dword ptr [esi + 4]
            mov ebp, eax
            push esi
            imul ebp, edi
            call dword ptr [edx + 0x18]
            add ebp, eax
            mov eax, dword ptr [esp + 0x80]
            push ebp
            push 0x18
            push esi
            call dword ptr [eax + 8]
            mov ecx, dword ptr [esi + 4]
            push edi
            push esi
            call dword ptr [ecx + 0x10]
            mov edx, dword ptr [esp + 0x18]
            push edi
            push edx
            push offset br_vertex_uv_F
            push esi
            call DfStructWriteArray
    L0052414c:
            mov di, word ptr [ebx + 0x12]
            test di, di
            je L005242ed
            mov eax, dword ptr [ebx + 0xc]
            push 0
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esi + 4]
            push offset br_face_F
            push esi
            and edi, 0xffff
            mov dword ptr [esp + 0x8c], eax
            call dword ptr [eax + 0x24]
            mov ecx, dword ptr [esi + 4]
            mov ebp, eax
            push esi
            imul ebp, edi
            call dword ptr [ecx + 0x18]
            mov edx, dword ptr [esp + 0x80]
            add ebp, eax
            push ebp
            push 0x35
            push esi
            call dword ptr [edx + 8]
            mov eax, dword ptr [esi + 4]
            push edi
            push esi
            call dword ptr [eax + 0x10]
            mov ecx, dword ptr [esp + 0x18]
            push edi
            push ecx
            push offset br_face_F
            push esi
            call DfStructWriteArray
            mov edi, dword ptr [esp + 0x7c]
            test edi, edi
            je L005242ed
            mov edx, dword ptr [esi + 4]
            push esi
            call dword ptr [edx + 0x18]
            test edi, edi
            mov ebp, eax
            jle L005241ff
            mov eax, dword ptr [esp + 0x7c]
            mov edi, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x80], eax
    L005241d8:
            mov edx, dword ptr [edi]
            mov ecx, dword ptr [esi + 4]
            mov eax, dword ptr [edx + 4]
            push eax
            push esi
            call dword ptr [ecx + 0x3c]
            add ebp, eax
            mov eax, dword ptr [esp + 0x80]
            add edi, 4
            dec eax
            mov dword ptr [esp + 0x80], eax
            jne L005241d8
            mov edi, dword ptr [esp + 0x7c]
    L005241ff:
            mov ecx, dword ptr [esi + 4]
            push ebp
            push 0x16
            push esi
            call dword ptr [ecx + 8]
            mov edx, dword ptr [esi + 4]
            push edi
            push esi
            call dword ptr [edx + 0x10]
            test edi, edi
            jle L00524234
            mov edi, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x7c]
    L0052421d:
            mov ecx, dword ptr [edi]
            mov eax, dword ptr [esi + 4]
            mov edx, dword ptr [ecx + 4]
            push edx
            push esi
            call dword ptr [eax + 0x34]
            add edi, 4
            dec ebp
            jne L0052421d
            mov edi, dword ptr [esp + 0x7c]
    L00524234:
            mov ecx, dword ptr [v1db+0x4b4]
            xor eax, eax
            mov ax, word ptr [ebx + 0x12]
            mov ebp, dword ptr [ebx + 0xc]
            mov dword ptr [esp + 0x7c], eax
            add eax, eax
            push 9
            push eax
            push ecx
            call BrResAllocate
            mov ecx, dword ptr [esp + 0x88]
            add esp, 0xc
            test ecx, ecx
            mov dword ptr [esp + 0x14], eax
            mov ebx, eax
            jle L005242b7
            add ebp, 8
            mov dword ptr [esp + 0x80], ecx
    L00524270:
            mov word ptr [ebx], 0
            mov edx, dword ptr [ebp]
            test edx, edx
            je L00524298
            xor eax, eax
            test edi, edi
            jle L00524298
            mov ecx, dword ptr [esp + 0x10]
    L00524286:
            cmp dword ptr [ecx], edx
            je L00524294
            inc eax
            add ecx, 4
            cmp eax, edi
            jl L00524286
            jmp L00524298
    L00524294:
            inc eax
            mov word ptr [ebx], ax
    L00524298:
            mov eax, dword ptr [esp + 0x80]
            add ebp, 0x28
            add ebx, 2
            dec eax
            mov dword ptr [esp + 0x80], eax
            jne L00524270
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x7c]
    L005242b7:
            mov edi, dword ptr [esi + 4]
            push 2
            push 1
            push 0
            push ecx
            push eax
            push esi
            call dword ptr [edi + 0x30]
            push eax
            push 0x1a
            push esi
            call dword ptr [edi + 8]
            mov eax, dword ptr [esp + 0x7c]
            mov edi, dword ptr [esp + 0x14]
            mov edx, dword ptr [esi + 4]
            push 2
            push 1
            push 0
            push eax
            push edi
            push esi
            call dword ptr [edx + 0x28]
            push edi
            call BrResFree
            add esp, 4
    L005242ed:
            push esi
            call FopWrite_END
            mov ecx, dword ptr [esp + 0x10]
            push ecx
            call BrResFree
            add esp, 4
            xor eax, eax
            pop ebp
            pop edi
            pop esi
            pop ebx
            add esp, 0x68
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00523e50
br_uint_32 C2_NAKED C2_HOOK_CDECL BrModelSaveMany(char* filename, br_model** models, br_uint_16 num) {
#ifndef CARPOCALYPSE2_MATCHING
    br_datafile* df;
    int i;
    int m;

    df = DfOpen(filename, 1, BRT_FLOAT);
    if (df == NULL) {
        return 0;
    }
    FopWrite_FILE_INFO(df, FILE_TYPE_MODEL);
    if (models == NULL) {
        BrModelEnum(NULL, (br_model_enum_cbfn*)&WriteModel, df);
        m = BrModelCount(NULL);
    } else {
        for (i = 0; i < num; i++) {
            WriteModel(models[i], df);
        }
        m = num;
    }
    DfClose(df);
    return m;
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            push edi
            push 0xc
            push 1
            push eax
            call DfOpen
            mov edi, eax
            test edi, edi
            jne L00523e67
            pop edi
            ret
    L00523e67:
            push esi
            push 0xface
            push edi
            call FopWrite_FILE_INFO
            mov esi, dword ptr [esp + 0x10]
            test esi, esi
            je L00523eac
            push ebp
            mov ebp, dword ptr [esp + 0x18]
            and ebp, 0xffff
            jle L00523e9e
            push ebx
            mov ebx, ebp
    L00523e8b:
            mov ecx, dword ptr [esi]
            push edi
            push ecx
            call WriteModel
            add esp, 8
            add esi, 4
            dec ebx
            jne L00523e8b
            pop ebx
    L00523e9e:
            mov esi, ebp
            pop ebp
            push edi
            call DfClose
            mov eax, esi
            pop esi
            pop edi
            ret
    L00523eac:
            push edi
            push offset WriteModel
            push 0
            call BrModelEnum
            add esp, 0xc
            push 0
            call BrModelCount
            add esp, 4
            mov esi, eax
            push edi
            call DfClose
            mov eax, esi
            pop esi
            pop edi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00524330
br_uint_32 C2_HOOK_CDECL BrActorLoadMany(char* filename, br_actor** actors, br_uint_16 num) {
    br_datafile* df;
    int count;
    int r;

    df = DfOpen(filename, 0, BRT_FLOAT);
    if (df == NULL) {
        return 0;
    }

    count = 0;
    do {
        if (count >= num) {
            break;
        }
        r = DfChunksInterpret(df, &ActorLoadTable);
        if (DfTopType() == DF_ACTOR) {
            actors[count] = DfPop(DF_ACTOR, NULL);
            count++;
        }
    } while (r != 0);
    DfClose(df);
    return count;
}

// FUNCTION: CARMA2_HW 0x00524400
int C2_HOOK_STDCALL WriteActor(br_actor* a, br_datafile* df) {
    br_actor* ap;
    br_actor* last_ap;

    FopWrite_ACTOR(df, a);
    if (a->t.type != BR_TRANSFORM_IDENTITY) {
        FopWrite_TRANSFORM(df, &a->t);
        FopWrite_ACTOR_TRANSFORM(df);
    }
    if (a->material != NULL) {
        FopWrite_ACTOR_MATERIAL(df, a->material);
    }
    if (a->model != NULL) {
        FopWrite_ACTOR_MODEL(df, a->model);
    }
    if (a->type_data != NULL) {
        switch (a->type) {
        case BR_ACTOR_LIGHT:
            FopWrite_LIGHT(df, a->type_data);
            FopWrite_ACTOR_LIGHT(df);
            break;
        case BR_ACTOR_CAMERA:
            FopWrite_CAMERA(df, a->type_data);
            FopWrite_ACTOR_CAMERA(df);
            break;
        case BR_ACTOR_BOUNDS:
            FopWrite_BOUNDS(df, a->type_data);
            FopWrite_ACTOR_BOUNDS(df);
            break;
        case BR_ACTOR_CLIP_PLANE:
            FopWrite_PLANE(df, a->type_data);
            FopWrite_ACTOR_CLIP_PLANE(df);
            break;
        }
    }
    if (a->children != NULL){
        last_ap = a->children;
        while (last_ap->next != NULL) {
            last_ap = last_ap->next;
        }
        ap = last_ap;
        while (1) {
            WriteActor(ap, df);
            FopWrite_ACTOR_ADD_CHILD(df);
            if (a->children == ap) {
                break;
            }
            ap = (br_actor*)ap->prev;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005243a0
br_uint_32 C2_HOOK_CDECL BrActorSaveMany(char* filename, br_actor** actors, br_uint_16 num) {
    br_datafile* df;
    int i;

    df = DfOpen(filename, 1, BRT_FLOAT);
    if (df == NULL) {
        return 0;
    }
    FopWrite_FILE_INFO(df, FILE_TYPE_ACTORS);
    for (i = 0; i < num; i++) {
        WriteActor(actors[i], df);
        FopWrite_END(df);
    }
    DfClose(df);
    return num;
}

// FUNCTION: CARMA2_HW 0x00524600
br_uint_32 C2_HOOK_CDECL BrMaterialLoadMany(char* filename, br_material** materials, br_uint_16 num) {
    br_datafile* df;
    int count;
    int r;

    df = DfOpen(filename, 0, BRT_FLOAT);
    if (df == NULL) {
        return 0;
    }
    count = 0;
    do {
        if (count >= num) {
            break;
        }
        r = DfChunksInterpret(df, &MaterialLoadTable);
        if (DfTopType() == DF_MATERIAL) {
            materials[count] = DfPop(DF_MATERIAL, NULL);
            ++count;
        }
    } while (r != 0);
    DfClose(df);
    return count;
}

// FUNCTION: CARMA2_HW 0x005247f0
br_uint_32 C2_HOOK_CDECL WriteMaterial(br_material* mp, void* df) {

    FopWrite_MATERIAL(df, mp);
    if (mp->colour_map != NULL) {
        FopWrite_PIXELMAP_REF(df, CHUNKID_MATERIAL_COLOUR_MAP, mp->colour_map);
    }
    if (mp->index_shade != NULL) {
        FopWrite_PIXELMAP_REF(df, CHUNKID_MATERIAL_INDEX_SHADE, mp->index_shade);
    }
    if (mp->index_blend != NULL) {
        FopWrite_PIXELMAP_REF(df, CHUNKID_MATERIAL_INDEX_BLEND, mp->index_blend);
    }
    if (mp->screendoor != NULL) {
        FopWrite_PIXELMAP_REF(df, CHUNKID_MATERIAL_SCREENDOOR, mp->screendoor);
    }
    if (mp->index_fog != NULL) {
        FopWrite_PIXELMAP_REF(df, CHUNKID_MATERIAL_INDEX_FOG, mp->index_fog);
    }
    FopWrite_END(df);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00524670
br_uint_32 C2_HOOK_CDECL BrMaterialSaveMany(char* filename, br_material** materials, br_uint_16 num) {
    br_datafile* df;
    int i;
    int count;

    df = DfOpen(filename, 1, BRT_FLOAT);
    if (df == NULL) {
        return 0;
    }
#ifdef BRENDER_FIX_BUGS
    FopWrite_FILE_INFO(df, FILE_TYPE_MATERIAL);
#else
    FopWrite_FILE_INFO(df, FILE_TYPE_MATERIAL_OLD);
#endif
    if (materials == NULL) {
        BrMaterialEnum(NULL, WriteMaterial, df);
        count = BrMaterialCount(NULL);
    } else {
        for (i = 0; i < num; i++) {
            WriteMaterial(materials[i], df);
        }
        count = num;
    }
    DfClose(df);
    return count;
}

// FUNCTION: CARMA2_HW 0x005248e0
br_model* C2_HOOK_CDECL BrModelLoad(char* filename) {
    br_model* ptr;

    if (BrModelLoadMany(filename, &ptr, 1) == 1) {
        return ptr;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00524960
br_uint_32 C2_NAKED C2_HOOK_CDECL BrModelSave(char* filename, br_model* ptr) {
#ifndef CARPOCALYPSE2_MATCHING

    return BrModelSaveMany(filename, &ptr, 1);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            push edi
            push 0xc
            push 1
            push eax
            call DfOpen
            mov edi, eax
            test edi, edi
            jne L00524977
            pop edi
            ret
    L00524977:
            push esi
            push 0xface
            push edi
            call FopWrite_FILE_INFO
            lea ecx, [esp + 0x10]
            test ecx, ecx
            je L005249b8
            push ebx
            lea esi, [esp + 0x14]
            mov ebx, 1
    L00524995:
            mov edx, dword ptr [esi]
            push edi
            push edx
            call WriteModel
            add esp, 8
            add esi, 4
            dec ebx
            jne L00524995
            pop ebx
            mov esi, 1
            push edi
            call DfClose
            mov eax, esi
            pop esi
            pop edi
            ret
    L005249b8:
            push edi
            push offset WriteModel
            push 0
            call BrModelEnum
            add esp, 0xc
            push 0
            call BrModelCount
            add esp, 4
            mov esi, eax
            push edi
            call DfClose
            mov eax, esi
            pop esi
            pop edi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005249e0
br_material* C2_HOOK_CDECL BrMaterialLoad(char* filename) {
    br_material* ptr;

    if (BrMaterialLoadMany(filename, &ptr, 1) == 1) {
        return ptr;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00524a60
br_uint_32 C2_HOOK_CDECL BrMaterialSave(char* filename, br_material* ptr) {

    return BrMaterialSaveMany(filename, &ptr, 1);
}

// FUNCTION: CARMA2_HW 0x00524be0
br_actor* C2_HOOK_CDECL BrActorLoad(char* filename) {
    br_actor* ptr;

    if (BrActorLoadMany(filename, &ptr, 1) == 1) {
        return ptr;
    }
    // FIXME: LOG_DEBUG("loadmany fail");
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00524c60
br_uint_32 C2_HOOK_CDECL BrActorSave(char* filename, br_actor* ptr) {

    return BrActorSaveMany(filename, &ptr, 1);
}

// FUNCTION: CARMA2_HW 0x00524cb0
br_error C2_HOOK_CDECL BrModelFileCount(char* filename, br_uint_16* num) {
    br_datafile* df;

    df = DfOpen(filename, 0, BRT_FLOAT);
    if (df == NULL) {
        if (num == NULL) {
            return 0x1002;
        } else {
            *num = 0;
            return 0x1002;
        }
    }
    if (num == NULL) {
        return 0;
    }
    *num = 0;
    while (DfChunksInterpret(df, &ModelLoadTable) != 0) {
        if (DfTopType() == DF_MODEL) {
            *num += 1;
            DfPop(DF_MODEL, NULL);
        }
    }
    DfClose(df);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00524d30
br_error C2_HOOK_CDECL BrActorFileCount(char* filename, br_uint_16* num) {
    br_datafile* df;

    df = DfOpen(filename, 0, BRT_FLOAT);
    if (df == NULL) {
        if (num == NULL) {
            return 0x1002;
        } else {
            *num = 0;
            return 0x1002;
        }
    }
    if (num == NULL) {
        return 0;
    }
    *num = 0;
    while (DfChunksInterpret(df, &ActorLoadTable) != 0) {
        if (DfTopType() == DF_ACTOR) {
            *num += 1;
            DfPop(DF_ACTOR, NULL);
        }
    }
    DfClose(df);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00524db0
br_error C2_HOOK_CDECL BrMaterialFileCount(char* filename, br_uint_16* num) {
    br_datafile* df;

    df = DfOpen(filename, 0, BRT_FLOAT);
    if (df == NULL) {
        if (num == NULL) {
            return 0x1002;
        } else {
            *num = 0;
            return 0x1002;
        }
    }
    if (num == NULL) {
        return 0;
    }
    *num = 0;
    while (DfChunksInterpret(df, &MaterialLoadTable) != 0) {
        if (DfTopType() == DF_MATERIAL) {
            *num += 1;
            DfPop(DF_MATERIAL, NULL);
        }
    }
    DfClose(df);
    return 0;
}
