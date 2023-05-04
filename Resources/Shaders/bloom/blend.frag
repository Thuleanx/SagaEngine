#version 330 core
out vec4 color;

in vec2 uv;

uniform sampler2D MainTex;
uniform sampler2D bloom;

uniform float exposure = 10;
uniform float gamma = 2.2;

void main() {
    vec3 hdrColor = texture(MainTex, uv).rgb;
    vec3 bloomColor = texture(bloom, uv).rgb * exposure;

    hdrColor += bloomColor;

    /* vec3 result = vec3(1.0) - exp(-hdrColor * exposure); */
    vec3 result = hdrColor / (hdrColor + 1);
    /* vec3 result = hdrColor; */

    result = pow(result, vec3(1.0 / gamma));
    color = vec4(result, 1);
}
