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

#define MAX_LIGHTS                   48 /* Must match up with BRender */
#define MAX_CLIP_PLANES              6

#define ENABLE_GAMMA_CORRECTION      0
#define ENABLE_SIMULATE_8BIT_COLOUR  0
#define ENABLE_SIMULATE_16BIT_COLOUR 0

#define UV_SOURCE_MODEL              0
#define UV_SOURCE_ENV_L              1
#define UV_SOURCE_ENV_I              2

// 3dfx driver uses a fog table. This approximates the behavior :/
#define FOGGING_EMULATE_3DFX_DENSITY_MULTIPLIER 80

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
layout(std140, set = 3, binding = 1) uniform br_scene_state
##else
layout(std140) uniform br_scene_state
##endif
{
    vec4 eye_view; /* Eye position in view-space */

    /* Field order matters: SDL3 GPU clips each uniform slot at 4096 bytes, so
     * everything read below lives at the front. The 4608-byte lights array
     * sits at the tail (this shader never references it). */
    vec4 clip_planes[MAX_CLIP_PLANES];
    uint num_clip_planes;

    float yon_z;
    uint num_lights;

    br_light lights[MAX_LIGHTS];
};
##ifdef SDL3GPU
layout(std140, set = 3, binding = 0) uniform br_model_state
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
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec4 colour;
##else
in vec4 position;
in vec2 uv;
in vec4 normal;
in vec4 colour;
##endif

##ifdef SDL3GPU
layout(location = 4) in vec3 rawPosition;
layout(location = 5) in vec3 rawNormal;
##else
in vec3 rawPosition;
in vec3 rawNormal;
##endif

##ifdef SDL3GPU
layout(location = 6) in vec3 v_frag_pos;
layout(location = 7) in float v_view_z;
##else
in vec3 v_frag_pos;
in float v_view_z;
##endif

##ifdef SDL3GPU
layout(location = 0) out vec4 fragColour;
##else
out vec4 fragColour;
##endif

##ifdef SDL3GPU
layout(set = 2, binding = 0) uniform sampler2D main_texture;
##else
uniform sampler2D main_texture;
##endif

vec3 adjustBrightness(in vec3 colour, in float brightness)
{
    return colour + brightness;
}

/* use values between -1 and 1 */
vec3 adjustContrast(in vec3 colour, in float contrast)
{
    return 0.5 + (1.0 + contrast) * (colour - 0.5);
}

/* use values between -1 and 1 */
vec3 adjustExposure(in vec3 colour, in float exposure)
{
    // return (1.0 + exposure) * colour;
    return vec3(1.0) - exp(-colour * exposure);
}

vec3 adjustSaturation(in vec3 colour, in float saturation)
{
    /* https://www.w3.org/TR/WCAG21/#dfn-relative-luminance */
    const vec3 luminosityFactor = vec3(0.2126, 0.7152, 0.0722);
    vec3 grayscale = vec3(dot(colour, luminosityFactor));

    return mix(grayscale, colour, 1.0 + saturation);
}


vec2 SurfaceMapEnvironment(in vec3 eye, in vec3 normal, in mat4 model_to_environment) {
    vec3 r;
    vec4 wr;
    float d, cu, cv;

    /*
     * Generate reflected vector
     */
    r = reflect(eye, normalize(normal));

    /*
     * Rotate vector into the environment frame.
     * This should be the identity matrix if no environment is set.
     */
    wr = model_to_environment * vec4(r, 0.0);
    vec3 wr2 = normalize(wr.xyz);

    /*
     * Convert vector to environment coordinates
     */
    cu = 0.5 + atan(wr2.x, -wr2.z) * 0.159154943091895; /* 1/(2*PI) */
    cv = 0.5 + -wr2.y * 0.5;

    return vec2(cu, cv);
}

vec2 SurfaceMap(in vec3 position, in vec3 normal, in vec2 uv)
{
    if(uv_source == UV_SOURCE_ENV_L) {
        /*
         * Generate U,V for environment assuming local eye.
         *
         * softrend/mapping.c - SurfaceMapEnvironmentLocal()
         */
        vec3 eye = normalize(eye_m.xyz - position);
        uv = SurfaceMapEnvironment(eye, normal, environment);
    } else if(uv_source == UV_SOURCE_ENV_I) {
        /*
         * Generate U,V for environment assuming infinite eye.
         *
         * softrend/mapping.c - SurfaceMapEnvironmentInfinite()
         */
        vec3 eye = normalize(eye_m.xyz);
        uv = SurfaceMapEnvironment(eye, normal, environment);
    } else {
        /* nop */
    }

    /* Apply the map transformation. */
    return (map_transform * vec4(uv, 1.0, 0.0)).xy;
}


void processClipPlanes() {
    for(uint i = 0u; i < num_clip_planes; i++) {
        // calculate signed plane-vertex distance
        float d = dot(clip_planes[i], projection_brender * model_view * vec4(rawPosition.xyz, 1));
        if (d < 0.0) {
            discard;
        }
    }
}

void doDistanceFog() {
    if (fog_enabled) {

        // Compute linear depth from view-space Z (passed as varying from vertex shader).
        // In view space, visible objects have z < 0, so negate to get positive depth.
        float linear_depth = -v_view_z;

        if (linear_depth < fog_min) {
            //return;
        }

        float linear_depth_normalized = linear_depth / yon_z;
        float density = 1/fog_max * FOGGING_EMULATE_3DFX_DENSITY_MULTIPLIER;
        float fogging_factor = 1.0 - exp(-density * linear_depth_normalized * linear_depth_normalized);
        fragColour.rgb = mix(fragColour.rgb, fog_colour.rgb, clamp(fogging_factor, 0.0, 1.0));
    }
}

void main() {
    vec4 textureColour;

    processClipPlanes();

    vec2 mappedUV = SurfaceMap(rawPosition, rawNormal, uv);

    if (disable_texture) {
        textureColour = surface_colour;
    } else {
        textureColour = texture(main_texture, mappedUV);
        // full black pixel is transparent
        if(!disable_colour_key && textureColour.rgb == vec3(0)) {
            discard;
        }
    }

    if (lighting == 1u) {
        fragColour.rgb = colour.rgb * textureColour.rgb;
    } else {
        fragColour.rgb = textureColour.rgb;
    }

    /* Perform gamma correction */
#if ENABLE_GAMMA_CORRECTION
    fragColour.rgb = pow(fragColour.rgb, vec3(1.0 / 1.2));
    // fragColour = adjustContrast(fragColour, 0.1);
    // fragColour = adjustExposure(fragColour, 2.0);
#endif

#if ENABLE_SIMULATE_8BIT_COLOUR
    fragColour = floor(fragColour.rgb * vec3(15.0)) / vec3(15.0);
#endif
#if ENABLE_SIMULATE_16BIT_COLOUR
    float r = floor(fragColour.r * 31.0) / 31.0;
    float g = floor(fragColour.g * 63.0) / 63.0;
    float b = floor(fragColour.b * 31.0) / 31.0;
    fragColour = vec3(r, g, b);
#endif

    // Apply alpha transparency
    fragColour.a = alpha;

    // Apply lighting
    if (lighting == 1u && prelit == 0u) {
        fragColour.rgb *= vec3(ka, ka, ka);
    }

    doDistanceFog();
}
