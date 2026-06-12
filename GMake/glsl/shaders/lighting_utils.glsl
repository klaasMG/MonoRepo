#ifndef LIGHTING_UTILS_GLSL
#define LIGHTING_UTILS_GLSL

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
};

float blinnPhong(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess) {
    vec3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfDir), 0.0);
    return pow(NdotH, shininess);
}

float diffuse(vec3 lightDir, vec3 normal) {
    return max(dot(normal, lightDir), 0.0);
}

float attenuation(float distance, float range) {
    return clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0);
}

#endif
