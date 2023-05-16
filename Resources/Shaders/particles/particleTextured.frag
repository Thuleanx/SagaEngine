#version 330 core
out vec4 fragColor;

in vec2 uv;

uniform sampler2D MainTex;
uniform vec4 color;

void main() {
    fragColor = color * texture(MainTex, uv);
    if (fragColor.a < 0.7) discard;
}
