#ifndef TRANSFORM_UTILS_GLSL
#define TRANSFORM_UTILS_GLSL

mat4 identity() {
    return mat4(1.0);
}

mat4 translate(vec3 offset) {
    mat4 m = identity();
    m[3] = vec4(offset, 1.0);
    return m;
}

mat4 scale(vec3 factor) {
    mat4 m = identity();
    m[0][0] = factor.x;
    m[1][1] = factor.y;
    m[2][2] = factor.z;
    return m;
}

mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1);
}

mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0, 0, 0, 0, 1);
}

mat4 rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

#endif
