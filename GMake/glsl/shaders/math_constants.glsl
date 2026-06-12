#ifndef MATH_CONSTANTS_GLSL
#define MATH_CONSTANTS_GLSL

const float PI = 3.14159265;
const float TWO_PI = 6.28318531;
const float HALF_PI = 1.57079633;
const float E = 2.71828183;
const float GOLDEN_RATIO = 1.61803399;

float radians(float deg) {
    return deg * PI / 180.0;
}

float degrees(float rad) {
    return rad * 180.0 / PI;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

#endif
