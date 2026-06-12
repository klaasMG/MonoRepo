#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 5) in vec3 aVelocity;
layout(location = 6) in float aLifetime;

uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTime;

out float vLifetime;

void main() {
    vec3 pos = aPos + aVelocity * uTime;
    vLifetime = aLifetime - uTime;
    gl_Position = uProjection * uView * vec4(pos, 1.0);
    gl_PointSize = max(1.0, vLifetime * 10.0);
}
