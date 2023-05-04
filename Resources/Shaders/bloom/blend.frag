#version 330 core
out vec4 outColor;

in vec2 uv;

uniform sampler2D MainTex;
uniform sampler2D bloom;

uniform float exposure = 10;
uniform float temperature = 0; // Range ~[-1.67;1.67] works best
uniform float tint = 0; // Range ~[-1.67;1.67] works best
uniform float contrast = 1;
uniform float brightness = 0;
uniform float saturation = 1;
uniform float gamma = 2.2;

// for reinhard extended tonemapper
uniform float lum_white = 2;

float luminance(vec3 color) {
    return dot(color, vec3(0.299f, 0.587f, 0.114f));
}

vec3 saturate(vec3 color) {
    return clamp(color, 0, 1);
}

//https://docs.unity3d.com/Packages/com.unity.shadergraph@6.9/manual/White-Balance-Node.html
vec3 whiteBalance(vec3 col) {
    float t1 = temperature * 10.0f / 6.0f;
    float t2 = tint * 10.0f / 6.0f;

    float x = 0.31271 - t1 * (t1 < 0 ? 0.1 : 0.05);
    float standardIlluminantY = 2.87 * x - 3 * x * x - 0.27509507;
    float y = standardIlluminantY + t2 * 0.05;

    vec3 w1 = vec3(0.949237, 1.03542, 1.08728);

    float Y = 1;
    float X = Y * x / y;
    float Z = Y * (1 - x - y) / y;
    float L = 0.7328 * X + 0.4296 * Y - 0.1624 * Z;
    float M = -0.7036 * X + 1.6975 * Y + 0.0061 * Z;
    float S = 0.0030 * X + 0.0136 * Y + 0.9834 * Z;
    vec3 w2 = vec3(L, M, S);

    vec3 balance = vec3(w1.x / w2.x, w1.y / w2.y, w1.z / w2.z);

    mat3 LIN_2_LMS_MAT = mat3(
        3.90405e-1, 5.49941e-1, 8.92632e-3,
        7.08416e-2, 9.63172e-1, 1.35775e-3,
        2.31082e-2, 1.28021e-1, 9.36245e-1
    );

    mat3 LMS_2_LIN_MAT = mat3(
        2.85847e+0, -1.62879e+0, -2.48910e-2,
        -2.10182e-1,  1.15820e+0,  3.24281e-4,
        -4.18120e-2, -1.18169e-1,  1.06867e+0
    );

    vec3 lms = LIN_2_LMS_MAT * col;
    lms *= balance;
    return LMS_2_LIN_MAT * lms;
}

vec3 reinhard(vec3 color) {
    return color / (color + 1);
}

vec3 reinhardExtended(vec3 col) {
    float lum = luminance(col);
    float lout = (lum * (1 + lum / (lum_white * lum_white) )) / (1 + lum);
    return saturate(lout * col / lum);
}

vec3 narkowiczACES(vec3 col) {
    return (col*(2.51f*col+0.03f))/(col*(2.43f*col+0.59f)+0.14f);
}

vec3 gammaCorrection(vec3 col) {
    return pow(col, vec3(1.0 / gamma));
}

vec3 applyContrastAndBrightness(vec3 col) {
    return (col - 0.5) * contrast + 0.5 + brightness;
}

vec3 applySaturation(vec3 col) {
    float lum = luminance(col);
    return mix(vec3(lum), col, saturation);
}

void main() {
    vec3 hdrColor = texture(MainTex, uv).rgb;
    vec3 bloomColor = texture(bloom, uv).rgb;

    hdrColor += bloomColor;
    hdrColor = max(hdrColor,0);

    // exposure
    hdrColor *= exposure;

    // white balancing
    hdrColor = whiteBalance(hdrColor);
    hdrColor = max(hdrColor, 0);

    // contrast and brightness
    hdrColor = applyContrastAndBrightness(hdrColor);
    hdrColor = max(hdrColor, 0);

    // saturation
    hdrColor = applySaturation(hdrColor);
    hdrColor = max(hdrColor, 0);

    // tonemapping
    hdrColor = narkowiczACES(hdrColor);

    hdrColor = gammaCorrection(hdrColor);

    outColor = vec4(hdrColor, 1);
}
