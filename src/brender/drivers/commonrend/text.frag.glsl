##ifdef GL_ES
#version 300 es
precision mediump float;
precision mediump int;
##endif
##ifdef GL_CORE
#version 140
#extension GL_ARB_explicit_attrib_location:require
##endif
##ifdef SDL3GPU
#version 450
##endif

##ifdef SDL3GPU
layout (location = 0) in vec2 uv;
##else
in vec2 uv;
##endif

##ifdef SDL3GPU
layout(set = 2, binding = 0) uniform sampler2D uSampler;
layout(std140, set = 3, binding = 2) uniform text_data { vec4 uTextColour; };
##else
uniform sampler2D uSampler;
uniform vec4 uTextColour;
##endif

layout (location = 0) out vec4 mainColour;

void main()
{
    mainColour = vec4(uTextColour.rgb, texture(uSampler, uv).a * uTextColour.a);
}
