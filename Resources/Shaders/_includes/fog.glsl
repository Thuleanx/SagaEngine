
float fogFactorByDepth(float depth, float density) {
    return pow(2, -depth*depth*density*density);
}

vec3 calculateFog(vec3 fragColor, vec3 fogColor, float fogFactor) {
    return mix(fogColor, fragColor, fogFactor);
}
