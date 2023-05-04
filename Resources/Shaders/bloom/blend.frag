#version 330 core
out vec4 outColor;

in vec2 uv;

uniform sampler2D MainTex;
uniform sampler2D bloom;

uniform float exposure = 10;
uniform float gamma = 2.2;

// for reinhard extended tonemapper
uniform float lum_white = 2;

float luminance(vec3 color) {
    return dot(color, vec3(0.299f, 0.587f, 0.114f));
}

vec3 saturate(vec3 color) {
    return clamp(color, 0, 1);
}

vec3 reinhard(vec3 color) {
    return color / (color + 1);
}

vec3 reinhardExtended(vec3 col) {
    float lum = luminance(col);
    float lout = (lum * (1 + lum / (lum_white * lum_white) )) / (1 + lum);
    return saturate(lout * col / lum);
}

vec3 gammaCorrection(vec3 col) {
    return pow(col, vec3(1.0 / gamma));
}

void main() {
    vec3 hdrColor = texture(MainTex, uv).rgb;
    vec3 bloomColor = texture(bloom, uv).rgb;

    hdrColor += bloomColor;
    hdrColor = max(hdrColor,0);

    // exposure
    hdrColor *= exposure;

    // white balancing

    // tonemapping
    hdrColor = reinhardExtended(hdrColor);

    hdrColor = gammaCorrection(hdrColor);

    outColor = vec4(hdrColor, 1);
}
