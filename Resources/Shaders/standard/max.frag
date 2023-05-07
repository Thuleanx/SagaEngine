#version 330 core

in vec2 uv;
out float value;

const int MAX_KERNEL_SIZE = 16;
uniform sampler2D MainTex;
uniform int kernelSz;
uniform bool horizontal;

void main() {
    vec2 tex_offset = 1.0 / textureSize(MainTex, 0); // gets size of single texel
    value = texture(MainTex, uv).r;
    if(horizontal) {
        for(int i = 1; i <= kernelSz; ++i)
        {
            value = max(value, texture(MainTex, uv + vec2(tex_offset.x * i, 0.0)).r);
            value = max(value, texture(MainTex, uv - vec2(tex_offset.x * i, 0.0)).r);
        }
    } else {
        for(int i = 1; i <= kernelSz; ++i)
        {
            value = max(value, texture(MainTex, uv + vec2(0.0, tex_offset.y * i)).r);
            value = max(value, texture(MainTex, uv - vec2(0.0, tex_offset.y * i)).r);
        }
    }
}
