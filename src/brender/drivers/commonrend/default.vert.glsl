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


layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aUV;

out vec3 colour;
out vec2 uv;

uniform mat4 uMVP;

void main()
{
	gl_Position = vec4(aPosition, 1.0);
	colour = aColour;
	uv = aUV;
}
