#version 330 core
#include "color_utils.glsl"

in vec3 vColor;

out vec4 FragColor;

void main() {
    vec3 display = hsv2rgb(vec3(luminance(vColor), 0.5, 0.8));
    FragColor = vec4(display, 1.0);
}
