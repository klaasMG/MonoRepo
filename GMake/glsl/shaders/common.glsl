#ifndef COMMON_GLSL
#define COMMON_GLSL

const float EPSILON = 1;

bool approxEqual(float a, float b) {
    return abs(a - b) < EPSILON;
}

#endif
