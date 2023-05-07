#version 400 core

out vec4 outColor;
in vec2 uv;

uniform sampler2D MainTex;
uniform float blurRadius = 1.0;

// Circular Kernel from GPU Zen 'Practical Gather-based Bokeh Depth of Field' by Wojciech Sterna
const vec2 offsets[48] = vec2[48](
	2.0f * vec2(1.000000f, 0.000000f),
	2.0f * vec2(0.707107f, 0.707107f),
	2.0f * vec2(-0.000000f, 1.000000f),
	2.0f * vec2(-0.707107f, 0.707107f),
	2.0f * vec2(-1.000000f, -0.000000f),
	2.0f * vec2(-0.707106f, -0.707107f),
	2.0f * vec2(0.000000f, -1.000000f),
	2.0f * vec2(0.707107f, -0.707107f),
	4.0f * vec2(1.000000f, 0.000000f),
	4.0f * vec2(0.923880f, 0.382683f),
	4.0f * vec2(0.707107f, 0.707107f),
	4.0f * vec2(0.382683f, 0.923880f),
	4.0f * vec2(-0.000000f, 1.000000f),
	4.0f * vec2(-0.382684f, 0.923879f),
	4.0f * vec2(-0.707107f, 0.707107f),
	4.0f * vec2(-0.923880f, 0.382683f),
	4.0f * vec2(-1.000000f, -0.000000f),
	4.0f * vec2(-0.923879f, -0.382684f),
	4.0f * vec2(-0.707106f, -0.707107f),
	4.0f * vec2(-0.382683f, -0.923880f),
	4.0f * vec2(0.000000f, -1.000000f),
	4.0f * vec2(0.382684f, -0.923879f),
	4.0f * vec2(0.707107f, -0.707107f),
	4.0f * vec2(0.923880f, -0.382683f),
	6.0f * vec2(1.000000f, 0.000000f),
	6.0f * vec2(0.965926f, 0.258819f),
	6.0f * vec2(0.866025f, 0.500000f),
	6.0f * vec2(0.707107f, 0.707107f),
	6.0f * vec2(0.500000f, 0.866026f),
	6.0f * vec2(0.258819f, 0.965926f),
	6.0f * vec2(-0.000000f, 1.000000f),
	6.0f * vec2(-0.258819f, 0.965926f),
	6.0f * vec2(-0.500000f, 0.866025f),
	6.0f * vec2(-0.707107f, 0.707107f),
	6.0f * vec2(-0.866026f, 0.500000f),
	6.0f * vec2(-0.965926f, 0.258819f),
	6.0f * vec2(-1.000000f, -0.000000f),
	6.0f * vec2(-0.965926f, -0.258820f),
	6.0f * vec2(-0.866025f, -0.500000f),
	6.0f * vec2(-0.707106f, -0.707107f),
	6.0f * vec2(-0.499999f, -0.866026f),
	6.0f * vec2(-0.258819f, -0.965926f),
	6.0f * vec2(0.000000f, -1.000000f),
	6.0f * vec2(0.258819f, -0.965926f),
	6.0f * vec2(0.500000f, -0.866025f),
	6.0f * vec2(0.707107f, -0.707107f),
	6.0f * vec2(0.866026f, -0.499999f),
	6.0f * vec2(0.965926f, -0.258818f)
);
const int OFFSET_SZ = 48;

float luminance(vec3 color) {
    return dot(color, vec3(0.299f, 0.587f, 0.114f));
}

float karisWeight(vec3 color) {
    return 1.0 / (1.0 + luminance(color));
}

void main() {
    vec2 tex_offset = blurRadius / textureSize(MainTex, 0); // gets size of single texel

    float accumulatedWeight = 0;
    vec3 result = vec3(0);

    vec3 fragColor = texture(MainTex, uv).rgb;
    float fragWeight = karisWeight(fragColor);
    result += fragColor * fragWeight;
    accumulatedWeight += fragWeight;

    for (int i = 0; i < OFFSET_SZ; i++) {
        fragColor = texture(MainTex, uv + offsets[i] * tex_offset).rgb;
        fragWeight = karisWeight(fragColor);
        result += fragColor * fragWeight;
        accumulatedWeight += fragWeight;
    }

    result = result / (accumulatedWeight);

    outColor = vec4(result, 1);
}
