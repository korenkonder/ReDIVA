/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.h"

inline float_t light_point_calculate_radius(vec3* color,
    float_t constant, float_t linear, float_t quadratic) {
    vec3 c = *color;
    float_t light_max = max(max(c.x, c.y), c.z);

    double_t t_sqrt = sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 1.0) * light_max));
    double_t radius = (-linear + t_sqrt) / (2.0 * quadratic);
    return (float_t)radius;
}
