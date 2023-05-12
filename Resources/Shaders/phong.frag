#version 330 core

const float tau = 6.283185307179586;

// Shadow data
const int KERNEL_SZ = 64;
uniform sampler2D shadowMap;
uniform float cameraNearFarDistance;
uniform vec2 kernel[KERNEL_SZ];

// Uniforms for shape information
in vec3 worldSpace_pos;
in vec3 worldSpace_norm;
in vec2 tex_coord;
in vec4 lightSpace_pos;

// Object Material Data
uniform int colorSource; // 0 = solid color (objColor), 1 = texture color (objTexture), 2 = per-vertex color (vertColor)
uniform vec3 objColor;
uniform sampler2D objTexture;
uniform vec2 objTexture_tiling;
uniform float shininess;
in vec3 vertColor;

// Camera uniform
uniform vec3 worldSpace_camPos;

// Global Data
uniform vec3 coeffs; // vec3(ka, kd, ks)

// Light Data
uniform int lightType[16]; // 0 = point light, 1 = directional light
uniform vec3 lightColor[16];
uniform vec3 lightFunction[16]; // Attenuation coefficients
uniform vec2 lightAngle[16];
uniform vec3 worldSpace_lightPos[16]; //Light Positions
uniform vec3 worldSpace_lightDir[16]; //Light Directions
uniform int numLights; // Max number of lights = 8

layout (location = 0) out vec4 fragColor;

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   

float goldNoise(in vec2 xy, in float seed){
    return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float shadow() {
    // we use lightSpace_pos to calculate this
    vec3 projCoords = lightSpace_pos.xyz / lightSpace_pos.w; // homogenize this position. Now the coordinates are in the range (-1, 1)
    projCoords = projCoords * 0.5 + 0.5;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float shadow = 0.0;

    // random rotation for the kernel
    float theta = rand(worldSpace_pos.xy) * tau;
    float cosRot = sin(theta);
    float sinRot = cos(theta);

    mat2 kernelRotation;
    kernelRotation[0] = vec2(cosRot, -sinRot);
    kernelRotation[1] = vec2(sinRot, cosRot);

    // loop through kernel
    for (int i = 0; i < KERNEL_SZ; i++) {
        // depth of object closest to the light
        float closestDepth = texture(shadowMap, projCoords.xy + kernelRotation * kernel[i] * texelSize * 0.5).r;
        shadow += currentDepth - 0.00001 > closestDepth ? 1.0 : 0.0;
    }

    shadow = shadow / KERNEL_SZ;

    // if outside of the orthographic projection box, then no shadow is casted onto the area, instead of 
    // it being all black
    if (projCoords.z > 1.0)
        shadow = 0.0;

    float surfaceAlignment = dot(normalize(worldSpace_norm), -worldSpace_lightDir[0]);
    // if surface is parallel then shadow = 1. This is hard to handle otherwise because of numerical accuracy
    return (abs(surfaceAlignment) < 0.0001) ? 1.0 : shadow;
}

vec3 getToLight(int lightIndex) {
    int LIGHT_POINT = 0;
    int LIGHT_DIRECTIONAL = 1;
    int LIGHT_SPOT = 2;

    if (lightType[lightIndex] == LIGHT_POINT) {
        return normalize(worldSpace_lightPos[lightIndex] - worldSpace_pos);
    }
    else if (lightType[lightIndex] == LIGHT_DIRECTIONAL) {
        return normalize(-worldSpace_lightDir[lightIndex]);
    }
    else if (lightType[lightIndex] == LIGHT_SPOT) {
        return normalize(worldSpace_lightPos[lightIndex] - worldSpace_pos);
    }

    return vec3(0);
}

float saturate(float a) {
	return clamp(a,0,1);
}

float attenuationFactor(int lightIndex) {
    int LIGHT_POINT = 0;
    int LIGHT_SPOT = 2;

    if (lightType[lightIndex] == LIGHT_POINT || lightType[lightIndex] == LIGHT_SPOT) {
        vec3 coeffs = lightFunction[lightIndex];
        float d = length(worldSpace_lightPos[lightIndex] - worldSpace_pos);
        return 1.0 / (coeffs.x + coeffs.y * d + coeffs.z * d * d);
    }

    return 1;
}

float falloff(int lightIndex) {
    int LIGHT_SPOT = 2;

    if (lightType[lightIndex] == LIGHT_SPOT) {
        vec3 dirToLight = getToLight(lightIndex);
        vec3 lightDir = normalize(worldSpace_lightDir[lightIndex]);

        float angle = abs(acos(dot(-dirToLight, lightDir)));

        float lightInnerAngle = lightAngle[lightIndex].x;
        float lightOuterAngle = lightAngle[lightIndex].y;

        float t = saturate((angle - lightInnerAngle) / (lightOuterAngle - lightInnerAngle));

        float falloff = -2*t*t*t + 3*t*t;

        return saturate(1-falloff); 
    }

    return 1;
}

float computeDiffuseIntensity(vec3 worldSpace_toLight) {
    // Dot product to get diffuse intensity
    float diffMult = saturate(dot(normalize(worldSpace_norm), worldSpace_toLight));
    return diffMult;
}

float computeSpecularIntensity(vec3 worldSpace_toLight, vec3 worldSpace_toEye) {
    // Guard against pow weirdness when exponent is 0
    if (shininess == 0) {
        return 0;
    }

/*     if (dot(worldSpace_toLight, normalize(worldSpace_norm)) < 0) { */
/*         return 0; */
/*     } */

    //reflect toLight
    vec3 worldSpace_toLightReflected = reflect(-worldSpace_toLight, normalize(worldSpace_norm));

    float specMult = saturate(dot(worldSpace_toLightReflected, worldSpace_toEye));

    //Compute specular intensity using toEye, reflected light, and shininess
    return pow(specMult, shininess);
}

void main() {
    // Declare ambient, diffuse, and specular terms
    vec3 ambi = vec3(coeffs.x);
    vec3 diff = vec3(0.0);
    vec3 spec = vec3(0.0);


    // Compute worldSpace_toEye Vector for specular intensity computation;
    vec3 worldSpace_toEye = normalize(worldSpace_camPos - worldSpace_pos);


    // Compute per-light diffuse and specular contribution
    for(int i = 0; i<numLights; i+= 1){
        // get direction vector to light based on light type
        vec3 worldSpace_toLight = getToLight(i);

        float diffuse_intensity = computeDiffuseIntensity(worldSpace_toLight);
        float specular_intensity = computeSpecularIntensity(worldSpace_toLight, worldSpace_toEye);

        float att = attenuationFactor(i) * falloff(i);

        diff = diff + diffuse_intensity * lightColor[i] * att;
        spec = spec + specular_intensity * lightColor[i] * att;
    }

    // Apply global coefficients and object color to the diffuse and specular components

    diff = diff * vec3(coeffs.y);
    spec = spec * vec3(coeffs.z);

    // Color generated only from light intensities and colors
    vec3 tempColor = clamp(ambi + (diff + spec) * (1 - shadow()), 0, 1);

    // Apply correct object color
    if (colorSource == 0 ) {
        fragColor = vec4(tempColor * objColor, 1.0);
    } 
    else if (colorSource == 1){
        fragColor = vec4(tempColor * vec3(texture(objTexture, tex_coord * objTexture_tiling)), 1.0);
    }
    else if (colorSource == 2) {
        fragColor = vec4(tempColor * vertColor, 1.0);
    }
    else{
        fragColor = vec4(tempColor, 1.0);
    }

    /* fragColor = vec4((normalize(worldSpace_norm) + 1) / 2,1); */
}
