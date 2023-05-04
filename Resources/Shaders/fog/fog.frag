#version 330 core
out vec4 outColor;

in vec2 uv;

uniform float near;
uniform float far;

uniform vec3 fogColor;
uniform float fogDensity;

uniform sampler2D MainTex;
uniform sampler2D DepthTex;

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

    float squaredDepth = (depth*fogDensity)*(depth*fogDensity);
    float fogFactor = pow(2, -squaredDepth);

    color = mix(fogColor, color, fogFactor);
    outColor = vec4(vec3(color), 1);
}
