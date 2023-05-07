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

    vec3 color = actualColor;

    // do far coc blending
    color = mix(color, background, farCoc);

    // do near coc blending 
    color = mix(color, foreground.rgb, nearCoc);

    /* outColor = vec4(vec3(nearCoc, 0, 0), 1); */
    outColor = vec4(color, 1);
    /* outColor = vec4(background, 1); */
    /* outColor = vec4(vec3(nearCoc, 0, 0) + vec3(0,farCoc,0),1); */
}
