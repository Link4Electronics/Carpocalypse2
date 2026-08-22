#version 450

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

layout(location = 0) out vec2 fragUV;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    fragUV = vec2(aUV.x, 1.0 - aUV.y);
}
