/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#define _USE_MATH_DEFINES
#include "math.h"
#include <math.h>

FORCE_INLINE float_t ctgf(float_t x) {
    return 1.0f / tanf(x);
}

FORCE_INLINE float_t ctghf(float_t x) {
    return 1.0f / tanhf(x);
}

FORCE_INLINE float_t actgf(float_t x) {
    return 1.0f / atanf(x);
}

FORCE_INLINE float_t actghf(float_t x) {
    return 1.0f / atanhf(x);
}

FORCE_INLINE double_t ctg(double_t x) {
    return 1.0 / tan(x);
}

FORCE_INLINE double_t ctgh(double_t x) {
    return 1.0 / tanh(x);
}

FORCE_INLINE double_t actg(double_t x) {
    return 1.0 / atan(x);
}

FORCE_INLINE double_t actgh(double_t x) {
    return 1.0 / atanh(x);
}

FORCE_INLINE float_t lerpf(float_t x, float_t y, float_t blend) {
    return x * (1.0f - blend) + y * blend;
}

FORCE_INLINE double_t lerp(double_t x, double_t y, double_t blend) {
    return x * (1.0 - blend) + y * blend;
}
