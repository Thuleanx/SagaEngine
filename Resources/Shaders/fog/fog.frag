#version 330 core
out vec4 outColor;

in vec2 uv;

uniform float near;
uniform float far;

uniform vec3 fogColor;
uniform float fogDensity;

uniform sampler2D MainTex;
uniform sampler2D DepthTex;

#include Resources/Shaders/_includes/depth.glsl
#include Resources/Shaders/_includes/fog.glsl

void main() {
    vec3 color = texture(MainTex, uv).rgb;
    float depth = texture(DepthTex, uv).r;

    depth = linearizePerspectiveDepth(depth) * far;
    float fogFactor = fogFactorByDepth(depth, fogDensity);
    outColor = vec4(calculateFog(color, fogColor, fogFactor), 1);
}
