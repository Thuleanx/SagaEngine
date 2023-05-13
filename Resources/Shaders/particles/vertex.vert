#version 330 core

// All in object space
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 inUV;

uniform mat4 mvp;

out vec2 uv;

void main() {
    gl_Position = mvp*vec4(pos, 1.0);
    uv = inUV;
}
