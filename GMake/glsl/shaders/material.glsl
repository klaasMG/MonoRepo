#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

struct Material {
    vec3 albedo;
    vec3 emissive;
    float metallic;
    float roughness;
    float ao;
};

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

#endif
