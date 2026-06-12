#version 330 core
#include "math_constants.glsl"
#include "noise_utils.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uDisplacement;

out vec3 vPosition;
out vec3 vNormal;

void main() {
    float n = noise(aPos.xz * 2.0);
    vec3 displaced = aPos + aNormal * uDisplacement * n;
    vPosition = (uModel * vec4(displaced, 1.0)).xyz;
    vNormal = mat3(uModel) * aNormal;
    gl_Position = uProjection * uView * vec4(vPosition, 1.0);
}
