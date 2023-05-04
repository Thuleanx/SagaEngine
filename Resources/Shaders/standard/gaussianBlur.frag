#version 330 core

out vec4 color;
in vec2 uv;

uniform sampler2D MainTex;

const int KERNEL_SZ = 16;

uniform float bloomRadius = 3;
uniform bool horizontal;
uniform int kernelSz = 16; // actual kernel size
uniform float weight[16];

void main() {
    vec2 tex_offset = bloomRadius / textureSize(MainTex, 0); // gets size of single texel
    vec3 result = texture(MainTex, uv).rgb * weight[0]; // current fragment's contribution
    if(horizontal) {
        for(int i = 1; i < kernelSz; ++i)
        {
            result += texture(MainTex, uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(MainTex, uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < kernelSz; ++i)
        {
            result += texture(MainTex, uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(MainTex, uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    color = vec4(result, 1.0);
}
