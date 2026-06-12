#version 330 core
#include "lighting_utils.glsl"
#include "material.glsl"
#include "color_utils.glsl"

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoord;
in Material vMaterial;

uniform DirectionalLight uSun;
uniform PointLight uPointLights[4];
uniform vec3 uViewPos;
uniform int uNumPointLights;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vFragPos);
    vec3 result = vMaterial.emissive;

    vec3 L = normalize(-uSun.direction);
    float diff = diffuse(L, N);
    float spec = blinnPhong(L, N, V, mix(4.0, 256.0, 1.0 - vMaterial.roughness));
    result += (diff + spec) * uSun.color * uSun.intensity;

    for (int i = 0; i < uNumPointLights; i++) {
        vec3 lightVec = uPointLights[i].position - vFragPos;
        float dist = length(lightVec);
        L = lightVec / dist;
        diff = diffuse(L, N);
        float atten = attenuation(dist, uPointLights[i].range);
        result += diff * uPointLights[i].color * uPointLights[i].intensity * atten;
    }

    result *= vMaterial.albedo;
    vec3 color = mix(result, result * (1.0 - vMaterial.metallic), vMaterial.metallic);
    FragColor = applyGamma(vec4(color * vMaterial.ao, 1.0), 2.2);
}
