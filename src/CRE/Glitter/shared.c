/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shared.h"

void axis_angle_from_vectors(vec3* axis, float_t* angle, vec3* vec1, vec3* vec2) {
    float_t t;

    vec3_cross(*vec1, *vec2, *axis);
    vec3_length(*axis, *angle);
    if (*angle >= 0.000001f)
        *angle = asinf(min(*angle, 1.0f));
    else {
        *angle = 0.0f;
        axis->x = vec1->z;
        axis->y = 0.0f;
        axis->z = vec1->x;
        vec3_length(*axis, t);
        if (t < 0.000001f) {
            axis->x = -vec1->y;
            axis->y = vec1->x;
            axis->z = 0.0f;
        }
    }

    vec3_dot(*vec1, *vec2, t);
    if (t < 0.0f)
        *angle = (float_t)M_PI - *angle;
}

void mat3_mult_axis_angle(mat3* src, mat3* dst, vec3* axis, float_t angle) {
    quat q1;
    quat q2;
    quat q3;

    quat_from_mat3(src->row0.x, src->row1.x, src->row2.x, src->row0.y,
        src->row1.y, src->row2.y, src->row0.z, src->row1.z, src->row2.z, &q1);
    quat_from_axis_angle(axis, angle, &q2);
    quat_mult(&q2, &q1, &q3);
    mat3_from_quat(&q3, dst);
}

void mat4_mult_axis_angle(mat4* src, mat4* dst, vec3* axis, float_t angle) {
    quat q1;
    quat q2;
    quat q3;
    float_t t0;
    float_t t1;
    float_t t2;
    vec4 t3;

    t0 = src->row0.w;
    t1 = src->row1.w;
    t2 = src->row2.w;
    t3 = src->row3;
    quat_from_mat3(src->row0.x, src->row1.x, src->row2.x, src->row0.y,
        src->row1.y, src->row2.y, src->row0.z, src->row1.z, src->row2.z, &q1);
    quat_from_axis_angle(axis, angle, &q2);
    quat_mult(&q2, &q1, &q3);
    mat4_from_quat(&q3, dst);
    dst->row0.w = t0;
    dst->row1.w = t1;
    dst->row2.w = t2;
    dst->row3 = t3;
}
