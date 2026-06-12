#version 330 core
#include "transform_utils.glsl"
#include "math_constants.glsl"
#include "lighting_utils.glsl"
#include "material.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform Material uMaterial;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTexCoord;
out Material vMaterial;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoord = aTexCoord;
    vMaterial = uMaterial;
    gl_Position = uProjection * uView * worldPos;
}
