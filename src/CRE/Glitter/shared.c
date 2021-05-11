/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shared.h"

float_t frame_speed = 1.0f;
float_t target_fps = 60.0f;
extern double_t render_freq;

float_t get_frame_speed() {
    return (float_t)(clamp(target_fps / render_freq, 0.0, 1.0) * frame_speed);
}

void FASTCALL axis_angle_from_vectors(vec3* axis, float_t* angle, vec3* vec1, vec3* vec2) {
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

void FASTCALL mat3_mult_axis_angle(mat3* src, mat3* dst, vec3* axis, float_t angle) {
    quat q1;
    quat q2;
    quat q3;

    quat_from_mat3(src->row0.x, src->row0.y, src->row0.z, src->row1.x,
        src->row1.y, src->row1.z, src->row2.x, src->row2.y, src->row2.z, &q1);
    quat_from_axis_angle(axis, angle, &q2);
    q3.x = q1.x * q2.w - q1.y * q2.z + q1.z * q2.y + q1.w * q2.x;
    q3.y = q1.x * q2.z + q1.y * q2.w - q1.z * q2.x + q1.w * q2.y;
    q3.z = -q1.x * q2.y + q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    q3.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    mat3_from_quat(&q3, dst);
}

void FASTCALL mat4_mult_axis_angle(mat4* src, mat4* dst, vec3* axis, float_t angle) {
    quat q1;
    quat q2;
    quat q3;
    mat3 yt;

    quat_from_mat3(src->row0.x, src->row0.y, src->row0.z, src->row1.x,
        src->row1.y, src->row1.z, src->row2.x, src->row2.y, src->row2.z, &q1);
    quat_from_axis_angle(axis, angle, &q2);
    q3.x = q1.x * q2.w - q1.y * q2.z + q1.z * q2.y + q1.w * q2.x;
    q3.y = q1.x * q2.z + q1.y * q2.w - q1.z * q2.x + q1.w * q2.y;
    q3.z = -q1.x * q2.y + q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    q3.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    mat3_from_quat(&q3, &yt);
    *(vec3*)&dst->row0 = yt.row0;
    *(vec3*)&dst->row1 = yt.row1;
    *(vec3*)&dst->row2 = yt.row2;
    dst->row0.w = src->row0.w;
    dst->row1.w = src->row2.w;
    dst->row2.w = src->row3.w;
    dst->row3 = src->row3;
}
