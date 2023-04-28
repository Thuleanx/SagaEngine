#version 330 core

out vec4 FragColor;

in vec3 worldSpace_pos;
in vec3 worldSpace_norm;
in vec2 tex_coord;
in vec3 vertColor;
in vec4 screenPos;

uniform sampler2D shadowMap;

void main() {
    vec2 uv = (screenPos.xy/screenPos.w + 1) / 2;
    float depthValue = texture(shadowMap, uv).r;
    FragColor = vec4(vec3(depthValue), 1.0);
    /* FragColor = vec4(uv, 1.0, 1.0); */
}
