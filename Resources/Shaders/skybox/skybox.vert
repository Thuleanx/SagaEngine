#version 330 core
layout (location = 0) in vec3 pos;

out vec3 cubemap_uvw;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    cubemap_uvw = pos;
    gl_Position = projection * view * vec4(pos, 1.0);
}
