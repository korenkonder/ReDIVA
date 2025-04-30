/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vec.hpp"

const __m128 vec2_neg = { -0.0f, -0.0f,  0.0f,  0.0f };
const __m128 vec3_neg = { -0.0f, -0.0f, -0.0f,  0.0f };
const __m128 vec4_neg = { -0.0f, -0.0f, -0.0f, -0.0f };

const __m128d vec2d_neg = { -0.0, -0.0f };

const __m128i vec2i_abs = {
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0x00, (char)0x00, (char)0x00, (char)0x00,
    (char)0x00, (char)0x00, (char)0x00, (char)0x00,
};

const __m128i vec3i_abs = {
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0x00, (char)0x00, (char)0x00, (char)0x00,
};

const __m128i vec4i_abs = {
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
};

const __m128i vec2i64_abs = {
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0x7F,
};

void vec3::axis_angle_from_vectors(vec3& axis, float_t& angle, const vec3& vec1, const vec3& vec2) {
    axis = vec3::cross(vec1, vec2);
    angle = vec3::length(axis);

    if (angle >= 0.000001f)
        angle = asinf(clamp_def(angle, -1.0f, 1.0f));
    else {
        angle = 0.0f;
        axis = vec3::cross(vec3(0.0f, 1.0f, 0.0f), vec1);
        if (vec3::length(axis) < 0.000001f)
            axis = vec3::cross(vec3(0.0f, 0.0f, 1.0f), vec1);
    }

    if (vec3::dot(vec1, vec2) < 0.0f)
        angle = (float_t)M_PI - angle;
}
