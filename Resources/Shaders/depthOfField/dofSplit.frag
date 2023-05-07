#version 330 core

in vec2 uv;

uniform sampler2D MainTex;
uniform sampler2D DepthTex;

uniform float far;
uniform float near;

uniform float focusRange;
uniform float focusDistance;

layout (location = 0) out float coc;
layout (location = 1) out float cocNear;
layout (location = 2) out vec4 foregroundColor;
layout (location = 3) out vec4 backgroundColor;

float linearizePerspectiveDepth(float depth) {
    float fon = far/near;
    depth = (1 - fon) * depth + fon;
    depth = 1.0 / depth;
    return depth;
}

void main() {
    vec3 color = texture(MainTex, uv).rgb;
    float depth = texture(DepthTex, uv).r;
    depth = linearizePerspectiveDepth(depth) * far;

    coc = (depth - focusDistance) / focusRange;
    coc = clamp(coc, -1, 1);

    cocNear = -clamp(coc, -1, 0);

    foregroundColor = vec4(color, 1);
    backgroundColor = vec4(color * max(coc, 0), 1);
}
