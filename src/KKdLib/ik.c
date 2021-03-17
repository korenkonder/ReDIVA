/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "ik.h"

vec3 calculate_ik_angles(float_t a, float_t b, float_t c) {

    float_t a2 = a * a;
    float_t b2 = b * b;
    float_t c2 = c * c;
    float_t b3 = 2 * a * b;
    float_t c3 = 2 * a * c;

    vec3 v;
    v.x = acosf((a2 - b2 + c2) / c3);
    v.y = acosf((a2 + b2 - c2) / b3);
    v.z = (float_t)(M_PI - (double_t)v.x - (double_t)v.y);
    return v;
}
