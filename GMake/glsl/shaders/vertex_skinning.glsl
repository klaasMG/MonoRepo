#version 330 core
#include "transform_utils.glsl"
#include "common.glsl"

const int MAX_BONES = 64;

layout(location = 0) in vec3 aPos;
layout(location = 3) in ivec4 aBoneIds;
layout(location = 4) in vec4 aBoneWeights;

uniform mat4 uBones[MAX_BONES];
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vFragPos;

void main() {
    mat4 skinMatrix = mat4(0.0);
    for (int i = 0; i < 4; i++) {
        if (aBoneIds[i] >= 0) {
            skinMatrix += uBones[aBoneIds[i]] * aBoneWeights[i];
        }
    }
    vec4 pos = uModel * skinMatrix * vec4(aPos, 1.0);
    vFragPos = pos.xyz;
    gl_Position = uProjection * uView * pos;
}
