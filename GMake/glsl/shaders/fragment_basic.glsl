#version 330 core
#include "common.glsl"
#include "color_utils.glsl"

in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 uColor;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    vec3 result = uColor * diff;
    FragColor = applyGamma(vec4(result, 1.0), 2.2);
}
