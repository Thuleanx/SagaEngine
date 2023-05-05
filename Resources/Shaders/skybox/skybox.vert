#version 330 core
layout (location = 0) in vec3 pos;

out vec3 cubemap_uvw;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    cubemap_uvw = pos;
    vec4 actualPos = projection * view * vec4(pos, 1.0);
    gl_Position = actualPos.xyww;
}
