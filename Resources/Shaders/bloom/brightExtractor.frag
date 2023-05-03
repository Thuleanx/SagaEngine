#version 330 core

in vec2 uv;

uniform sampler2D MainTex;
uniform float threshold = 0.7;

/* layout (location = 1) out vec4 fragColor; */
layout (location = 0) out vec4 brightColor;

void main() {
    vec4 fragColor = texture(MainTex, uv);
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    brightColor = (brightness > threshold) ? vec4(fragColor.rgb, 1.0) : vec4(0);
    /* fragColor = vec4(uv, 0, 1); */
}
