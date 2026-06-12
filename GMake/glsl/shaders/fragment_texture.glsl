#version 330 core
#include "color_utils.glsl"

in vec2 vTexCoord;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    FragColor = applyGamma(texColor, 2.2);
}
