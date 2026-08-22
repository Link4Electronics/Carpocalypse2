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

in vec3 colour;
in vec2 uv;

uniform sampler2D uSampler;
uniform float uFlipVertically;
uniform int uDiscardPurplePixels;

layout (location = 0) out vec4 mainColour;

void main()
{
    mainColour = texture(uSampler, vec2(uv.x, abs(uFlipVertically - uv.y)));
    if (uDiscardPurplePixels == 1 && mainColour.rgb == vec3(1.0, 0.0, 1.0)) {
        discard;
    }
}
