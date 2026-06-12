#version 330 core

in vec3 vTexCoords;

uniform samplerCube uSkybox;

out vec4 FragColor;

void main() {
    FragColor = texture(uSkybox, vTexCoords);
}
