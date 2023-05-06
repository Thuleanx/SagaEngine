#version 330 core

in vec2 uv;

uniform float far;
uniform float near;

uniform sampler2D MainTex;
uniform sampler2D coc;
uniform sampler2D cocNear;
uniform sampler2D foregroundBlurred;
uniform sampler2D backgroundBlurred;

out vec4 outColor;

void main() {
    vec3 actualColor = texture(MainTex, uv).rgb;

    float farCoc = clamp(texture(coc, uv).r, 0, 1);
    float nearCoc = texture(cocNear, uv).r;

    vec3 foreground = texture(foregroundBlurred, uv).rgb;
    vec3 background = texture(backgroundBlurred, uv).rgb;

    // do near coc blending 
    vec3 color = mix(actualColor, foreground, -nearCoc);

    // do far coc blending
    color = mix(color, background, farCoc);

    outColor = vec4(color,1);
}
