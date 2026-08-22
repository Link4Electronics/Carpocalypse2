##ifdef SDL3GPU
#version 450
##endif
##ifdef GL_ES
#version 300 es
precision mediump float;
precision mediump int;
precision lowp usampler2D;
##endif
##ifdef GL_CORE
#version 140
#extension GL_ARB_explicit_attrib_location:require
##endif

#define MAX_LIGHTS                      48 /* Must match up with BRender */
#define MAX_CLIP_PLANES                 6
#define SPECULARPOW_CUTOFF              0.6172
#define BR_SCALAR_EPSILON               1.192092896e-7f

#define DEBUG_DISABLE_LIGHTS            1
#define DEBUG_DISABLE_LIGHT_AMBIENT     0
#define DEBUG_DISABLE_LIGHT_DIRECTIONAL 0
#define DEBUG_DISABLE_LIGHT_POINT       0
#define DEBUG_DISABLE_LIGHT_POINTATTEN  0
#define DEBUG_DISABLE_LIGHT_SPOT        1
#define DEBUG_DISABLE_LIGHT_SPECULAR    0
#define ENABLE_PSX_SIMULATION           0

struct br_light
{
    vec4 position;    /* (X, Y, Z, 1) */
    vec4 direction;   /* (X, Y, Z, 0), normalised */
    vec4 half_;       /* (X, Y, Z, 0), normalised */
    vec4 colour;      /* (R, G, B, 0), normalised */
    vec4 iclq;        /* (intensity, constant, linear, attenutation) */
    vec2 spot_angles; /* (inner, outer), if (0.0, 0.0), then this is a point light. */
};

##ifdef SDL3GPU
layout(std140, set = 1, binding = 1) uniform br_scene_state
##else
layout(std140) uniform br_scene_state
##endif
{
    vec4 eye_view; /* Eye position in view-space */

    /* Field order matters: SDL3 GPU clips each uniform slot at 4096 bytes, so
     * everything read below lives at the front. The 4608-byte lights array
     * sits at the tail (vertex lighting is disabled by DEBUG_DISABLE_LIGHTS). */
    vec4 clip_planes[MAX_CLIP_PLANES];
    uint num_clip_planes;

    float yon_z;
    uint num_lights;

    br_light lights[MAX_LIGHTS];
};

##ifdef SDL3GPU
layout(std140, set = 1, binding = 0) uniform br_model_state
##else
layout(std140) uniform br_model_state
##endif
{
    mat4 model_view;
    mat4 projection;
    mat4 projection_brender;
    mat4 mvp;
    mat4 normal_matrix;
    mat4 environment;
    mat4 map_transform;
    vec4 surface_colour;
    vec4 clear_colour;
    vec4 eye_m; /* Eye position in model-space */

    float ka; /* Ambient mod */
    float ks; /* Specular mod (doesn't seem to be used by Croc) */
    float kd; /* Diffuse mod */
    float power;
    uint lighting; /* Is this surface lit? */
    int uv_source;
    bool disable_colour_key;
    bool disable_texture;
    bool fog_enabled;
    vec4 fog_colour;
    float fog_min;
    float fog_max;
    float alpha;
    uint prelit;
};

##ifdef SDL3GPU
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColour;
##else
in vec3 aPosition;
in vec2 aUV;
in vec3 aNormal;
in vec4 aColour;
##endif

##ifdef SDL3GPU
layout(location = 0) out vec4 position;
layout(location = 1) out vec2 uv;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 colour;
##else
out vec4 position;
out vec4 normal;
out vec2 uv;
out vec4 colour;
##endif

##ifdef SDL3GPU
layout(location = 4) out vec3 rawPosition;
layout(location = 5) out vec3 rawNormal;
##else
out vec3 rawPosition;
out vec3 rawNormal;
##endif

##ifdef SDL3GPU
layout(location = 6) out vec3 v_frag_pos;
layout(location = 7) out float v_view_z;
##else
out vec3 v_frag_pos;
out float v_view_z;
##endif

bool directLightExists;

#define SPECULAR_DOT()                    \
    {                                     \
        float rd = dot(dirn_norm, n) * 2.0; \
        vec4 r = n - rd;                  \
        r = r - dirn_norm;                \
                                          \
        _dot = dot(eye_view, r);          \
    }

#define SPECULAR_POWER(l) (_dot * (l)) / (power - (power * _dot) + _dot)

float calculateAttenuation(in br_light alp, in float dist)
{
    if (dist > alp.iclq.w)
        return 0.0;

    float attn;

    if (dist > alp.iclq.y)
        attn = (dist - alp.iclq.y) * alp.iclq.z;
    else
        attn = 0.0;

    return 1.0 - attn;
}

float shadingFilter(in float i)
{
/* Software shading emulation */
#if ENABLE_PSX_SIMULATION
    i = floor(i * 255.0) / 255.0;
#endif
    return i;
}

vec3 lightingColourAmbient(in vec4 p, in vec4 n, in br_light alp)
{
    return ka * alp.iclq.x * alp.colour.xyz;
}

vec3 lightingColourDirect(in vec4 p, in vec4 n, in br_light alp)
{
    /* Notes: '_dot' is 'intensity' */
    float _dot = max(dot(n, alp.direction), 0.0) * kd;

    /* Shading filters like 'toon' */
    _dot = shadingFilter(_dot);

    /*
     * -- Kludge to emulate the broken D3D lighting from v1.0 and v1.1
     * vec3 outColour = vec3((alp.colour.x + alp.colour.y + alp.colour.z) / 3.0) * _dot;
     */
    vec3 outColour = alp.colour.xyz * _dot;

#if !DEBUG_DISABLE_LIGHT_SPECULAR
    if (ks <= 0.0) {
        _dot = dot(n, alp.half_);

        if (_dot > SPECULARPOW_CUTOFF)
            outColour += SPECULAR_POWER(ks * alp.iclq.x);
    }
#endif
    return outColour;
}

vec3 lightingColourPoint(in vec4 p, in vec4 n, in br_light alp)
{
    float _dot;
    vec4 dirn, dirn_norm;

    /* Work out vector between point and light source */
    dirn = alp.position - p;
    dirn_norm = normalize(dirn);

    float dist = length(dirn);

    _dot = max(dot(n, dirn_norm), 0.0) * kd;

    /* Shading filters like 'toon' */
    _dot = shadingFilter(_dot);

    vec3 outColour = _dot * (alp.iclq.x * alp.colour.xyz);

#if !DEBUG_DISABLE_LIGHT_SPECULAR
    if (ks != 0.0) {
        /* Specular */
        SPECULAR_DOT();

        /* Phong lighting approximation from Gems IV pg. 385 */
        if (_dot > SPECULARPOW_CUTOFF)
            outColour += SPECULAR_POWER(ks);
    }
#endif
    return (outColour);
}

vec3 lightingColourPointAtten(in vec4 p, in vec4 n, in br_light alp)
{
    float _dot;
    vec4 dirn, dirn_norm;

    /* Work out vector between point and light source */
    dirn = alp.position - p;
    dirn_norm = normalize(dirn);
    _dot = ((max(dot(n, dirn_norm), 0.0) * kd));

    /* Shading filters like 'toon' */
    _dot = shadingFilter(_dot);

    float dist = length(dirn);
    float atten = calculateAttenuation(alp, dist);

    vec3 outColour = _dot * alp.colour.xyz;

#if !DEBUG_DISABLE_LIGHT_SPECULAR
    if (ks != 0.0) {
        /* Specular */
        SPECULAR_DOT();

        /* Phong lighting approximation from Gems IV pg. 385 */
        if (_dot > SPECULARPOW_CUTOFF)
            outColour += SPECULAR_POWER(ks);
    }
#endif
    return outColour * atten;
}

vec3 lightingColourSpot(in vec4 p, in vec4 n, in br_light alp)
{
    /* Croc doesn't use spot lights */
    return vec3(0);
}

vec3 lightingColourSpotAtten(in vec4 p, in vec4 n, in br_light alp)
{
    /* Croc doesn't use spot lights */
    return vec3(0);
}

vec4 fragmain()
{
    if (num_lights == 0u || lighting == 0u) {
        return surface_colour;
    }

#if DEBUG_DISABLE_LIGHTS
    return surface_colour;
#else

    vec3 _colour = surface_colour.xyz;

    vec3 lightColour = vec3(0.0);
    vec3 directLightColour = vec3(0.0);
    directLightExists = false;

    for (uint i = 0u; i < num_lights; ++i) {
#if !DEBUG_DISABLE_LIGHT_AMBIENT
        if(lights[i].colour.w != 0.0) {
            lightColour += lightingColourAmbient(position, normal, lights[i]);
            continue;
        }
#endif
        if (lights[i].position.w == 0.0) {
#if !DEBUG_DISABLE_LIGHT_DIRECTIONAL
            directLightExists = true;
            directLightColour += lightingColourDirect(position, normal, lights[i]);
#endif
        } else {
            if (lights[i].spot_angles == vec2(0.0, 0.0)) {
                if (lights[i].iclq.zw == vec2(0)) {
#if !DEBUG_DISABLE_LIGHT_POINT
                    lightColour += lightingColourPoint(position, normal, lights[i]);
#endif
                } else {
#if !DEBUG_DISABLE_LIGHT_POINTATTEN
                    lightColour += lightingColourPointAtten(position, normal, lights[i]);
#endif
                }
            } else {
#if !DEBUG_DISABLE_LIGHT_SPOT
                if (lights[i].iclq.zw == vec2(0))
                    lightColour += lightingColourSpot(position, normal, lights[i]);
                else
                    lightColour += lightingColourSpotAtten(position, normal, lights[i]);
#endif
            }
        }
    }

    lightColour += directLightColour;
    lightColour *= _colour;

    lightColour = clamp(lightColour, 0.0, 1.0);
    return vec4(lightColour, surface_colour.a);
#endif
}

#if ENABLE_PSX_SIMULATION
vec4 PSXify_pos(in vec4 vertex, in vec2 resolution)
{
    vec4 snappedPos = vertex;
    snappedPos.xyz = vertex.xyz / vertex.w;
    snappedPos.xy = floor(resolution.xy * snappedPos.xy) / resolution;
    snappedPos.xyz *= vertex.w;
    return snappedPos;
}
#endif

void main()
{
    v_frag_pos = vec3((projection_brender * model_view) * vec4(aPosition, 1.0));
    vec4 pos = vec4(aPosition, 1.0);

    position = model_view * pos;
    v_view_z = position.z;
    normal = vec4(normalize(mat3(normal_matrix) * aNormal), 0);
    uv = aUV;

    if (lighting == 1u) {
        if (prelit == 1u) {
            colour = vec4(aColour.rgb, 1);
        } else {
            colour = fragmain();
        }
    }

    rawPosition = aPosition;
    rawNormal   = aNormal;

    if (!directLightExists && num_lights > 0u && lighting == 1u) {
        colour += vec4(clear_colour.rgb, 0.0);
    }

#if ENABLE_PSX_SIMULATION
    pos = PSXify_pos(mvp * pos, vec2(200.0, 150.0));
#else
    pos = projection * model_view * pos;
#endif

##ifdef SDL3GPU
    // SDL3 GPU NDC has Z in [0, 1] (near=0) like D3D/Vulkan, but BRender's
    // projection is GL-style (NDC z [-1,1]). Replicate the GL depthRange remap:
    pos.z = (pos.z + pos.w) * 0.5;
##endif

    gl_Position = pos;
}
