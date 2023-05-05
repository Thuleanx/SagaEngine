#version 330 core
out vec4 outColor;

in vec2 uv;

uniform sampler2D MainTex;

void main() {
    vec3 color = texture(MainTex, uv).rgb;
    outColor = vec4(vec3(color), 1);
}
