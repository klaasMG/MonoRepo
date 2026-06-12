#version 330 core

in float vLifetime;

uniform vec4 uColor;

out vec4 FragColor;

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float alpha = 1.0 - smoothstep(0.0, 1.0, length(circCoord));
    alpha *= smoothstep(0.0, 1.0, vLifetime / 5.0);
    if (alpha < 0.01) discard;
    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
