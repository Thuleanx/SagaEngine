#version 330 core

out vec4 outColor;

in vec3 cubemap_uvw;

uniform samplerCube Skybox;

void main() {
    outColor = texture(Skybox, cubemap_uvw);
}
