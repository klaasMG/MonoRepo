#version 330 core
#include "color_utils.glsl"
#include "common.glsl"

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 uColor;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    float height = (vPosition.y + 1.0) * 0.5;
    vec3 color = mix(uColor, uColor * 0.5, height);
    FragColor = applyGamma(vec4(color * (diff + 0.3), 1.0), 2.2);
}
