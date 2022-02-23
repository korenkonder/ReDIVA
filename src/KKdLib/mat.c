/*
    by korenkonder
    GitHub/GitLab: korenkonder
    Matrix Inverse algo: https://github.com/niswegmann/small-matrix-inverse
*/

#include "mat.h"

const mat3 mat3_identity = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
};

const mat3 mat3_null = {
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
};

const mat4 mat4_identity = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

const mat4 mat4_null = {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

const mat4u mat4u_identity = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

const mat4u mat4u_null = {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

vector_old_func(mat3)
vector_old_func(mat4)
vector_old_ptr_func(mat3)
vector_old_ptr_func(mat4)

// Crutch for vector_old_func definition

inline void mat3_add(mat3* x, mat3* y, mat3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    *(vec3*)&xt = x->row0;
    *(vec3*)&yt = y->row0;
    zt = _mm_add_ps(xt, yt);
    z->row0 = *(vec3*)&zt;
    *(vec3*)&xt = x->row1;
    *(vec3*)&yt = y->row1;
    zt = _mm_add_ps(xt, yt);
    z->row1 = *(vec3*)&zt;
    *(vec3*)&xt = x->row2;
    *(vec3*)&yt = y->row2;
    zt = _mm_add_ps(xt, yt);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_sub(mat3* x, mat3* y, mat3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    *(vec3*)&xt = x->row0;
    *(vec3*)&yt = y->row0;
    zt = _mm_sub_ps(xt, yt);
    z->row0 = *(vec3*)&zt;
    *(vec3*)&xt = x->row1;
    *(vec3*)&yt = y->row1;
    zt = _mm_sub_ps(xt, yt);
    z->row1 = *(vec3*)&zt;
    *(vec3*)&xt = x->row2;
    *(vec3*)&yt = y->row2;
    zt = _mm_sub_ps(xt, yt);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_mult(mat3* x, mat3* y, mat3* z) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 yt;
    __m128 zt;
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    *(vec3*)&xt0 = x->row0;
    *(vec3*)&xt1 = x->row1;
    *(vec3*)&xt2 = x->row2;
    *(vec3*)&yt = y->row0;
    t0 = _mm_mul_ps(xt0, _mm_shuffle_ps(yt, yt, 0x00));
    t1 = _mm_mul_ps(xt1, _mm_shuffle_ps(yt, yt, 0x55));
    t2 = _mm_mul_ps(xt2, _mm_shuffle_ps(yt, yt, 0xAA));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), t2);
    z->row0 = *(vec3*)&zt;
    *(vec3*)&yt = y->row1;
    t0 = _mm_mul_ps(xt0, _mm_shuffle_ps(yt, yt, 0x00));
    t1 = _mm_mul_ps(xt1, _mm_shuffle_ps(yt, yt, 0x55));
    t2 = _mm_mul_ps(xt2, _mm_shuffle_ps(yt, yt, 0xAA));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), t2);
    z->row1 = *(vec3*)&zt;
    *(vec3*)&yt = y->row2;
    t0 = _mm_mul_ps(xt0, _mm_shuffle_ps(yt, yt, 0x00));
    t1 = _mm_mul_ps(xt1, _mm_shuffle_ps(yt, yt, 0x55));
    t2 = _mm_mul_ps(xt2, _mm_shuffle_ps(yt, yt, 0xAA));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), t2);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_mult_vec(mat3* x, vec3* y, vec3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    *(vec3*)&yt = *y;
    *(vec3*)&xt = x->row0;
    zt0 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0x00));
    *(vec3*)&xt = x->row1;
    zt1 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0x55));
    *(vec3*)&xt = x->row2;
    zt2 = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0xAA));
    zt = _mm_add_ps(_mm_add_ps(zt0, zt1), zt2);
    *z = *(vec3*)&zt;
}

inline void mat3_mult_scalar(mat3* x, float_t y, mat3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    yt = _mm_set_ss(y);
    yt = _mm_shuffle_ps(yt, yt, 0);
    *(vec3*)&xt = x->row0;
    zt = _mm_mul_ps(xt, yt);
    z->row0 = *(vec3*)&zt;
    *(vec3*)&xt = x->row1;
    zt = _mm_mul_ps(xt, yt);
    z->row1 = *(vec3*)&zt;
    *(vec3*)&xt = x->row2;
    zt = _mm_mul_ps(xt, yt);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_transpose(mat3* x, mat3* z) {
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    __m128 xt3;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    *(vec3*)&xt0 = x->row0;
    *(vec3*)&xt1 = x->row1;
    *(vec3*)&xt2 = x->row2;
    xt0 = _mm_and_ps(xt0, vec4_mask_vec3.data);
    xt1 = _mm_and_ps(xt1, vec4_mask_vec3.data);
    xt2 = _mm_and_ps(xt2, vec4_mask_vec3.data);
    xt3 = vec4_null.data;
    yt0 = _mm_unpacklo_ps(xt0, xt1);
    yt1 = _mm_unpackhi_ps(xt0, xt1);
    yt2 = _mm_unpacklo_ps(xt2, xt3);
    yt3 = _mm_unpackhi_ps(xt2, xt3);
    zt0 = _mm_movelh_ps(yt0, yt2);
    zt1 = _mm_movehl_ps(yt2, yt0);
    zt2 = _mm_movelh_ps(yt1, yt3);
    z->row0 = *(vec3*)&zt0;
    z->row1 = *(vec3*)&zt1;
    z->row2 = *(vec3*)&zt2;
}

void mat3_inverse(mat3* x, mat3* z) {
    vec3 xt0;
    vec3 xt1;
    vec3 xt2;
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 wt;
    xt0 = x->row0;
    xt1 = x->row1;
    xt2 = x->row2;
    zt0 = _mm_sub_ps(
        _mm_mul_ps(
            _mm_set_ps(0.0f, xt0.y, xt0.z, xt1.y),
            _mm_set_ps(0.0f, xt1.z, xt2.y, xt2.z)
        ),
        _mm_mul_ps(
            _mm_set_ps(0.0f, xt0.z, xt0.y, xt1.z),
            _mm_set_ps(0.0f, xt1.y, xt2.z, xt2.y)
        )
    );
    zt1 = _mm_sub_ps(
        _mm_mul_ps(
            _mm_set_ps(0.0f, xt0.z, xt0.x, xt1.z),
            _mm_set_ps(0.0f, xt1.x, xt2.z, xt2.x)
        ),
        _mm_mul_ps(
            _mm_set_ps(0.0f, xt0.x, xt0.z, xt1.x),
            _mm_set_ps(0.0f, xt1.z, xt2.x, xt2.z)
        )
    );
    zt2 = _mm_sub_ps(
        _mm_mul_ps(
            _mm_set_ps(0.0f, xt0.x, xt0.y, xt1.x),
            _mm_set_ps(0.0f, xt1.y, xt2.x, xt2.y)
        ),
        _mm_mul_ps(
            _mm_set_ps(0.0f, xt0.y, xt0.x, xt1.y),
            _mm_set_ps(0.0f, xt1.x, xt2.y, xt2.x)
        )
    );

    wt = _mm_movelh_ps(_mm_unpacklo_ps(zt0, zt1), zt2);
    *(vec3*)&yt = xt0;
    yt = _mm_and_ps(yt, vec4_mask_vec3.data);
    wt = _mm_mul_ps(yt, wt);
    wt = _mm_hadd_ps(wt, wt);
    wt = _mm_hadd_ps(wt, wt);
    if (wt.m128_f32[0] != 0.0f)
        wt.m128_f32[0] = 1.0f / wt.m128_f32[0];
    wt = _mm_shuffle_ps(wt, wt, 0);
    yt = _mm_mul_ps(zt0, wt);
    z->row0 = *(vec3*)&yt;
    yt = _mm_mul_ps(zt1, wt);
    z->row1 = *(vec3*)&yt;
    yt = _mm_mul_ps(zt2, wt);
    z->row2 = *(vec3*)&yt;
}

inline void mat3_invtrans(mat3* x, mat3* z) {
    mat3 t = *x;
    mat3_inverse(&t, &t);
    mat3_transpose(&t, &t);
    *z = t;
}

inline void mat3_inverse_normalized(mat3* x, mat3* z) {
    mat3_transpose(x, z);
}

inline void mat3_normalize(mat3* x, mat3* z) {
    __m128 det;
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    det = _mm_set_ss(mat3_determinant(x));
    if (det.m128_f32[0] != 0.0f)
        det.m128_f32[0] = 1.0f / det.m128_f32[0];
    det = _mm_shuffle_ps(det, det, 0);
    *(vec3*)&xt0 = x->row0;
    *(vec3*)&xt1 = x->row1;
    *(vec3*)&xt2 = x->row2;
    xt0 = _mm_mul_ps(xt0, det);
    xt1 = _mm_mul_ps(xt1, det);
    xt2 = _mm_mul_ps(xt2, det);
    z->row0 = *(vec3*)&xt0;
    z->row1 = *(vec3*)&xt1;
    z->row2 = *(vec3*)&xt2;
}

inline void mat3_normalize_rotation(mat3* x, mat3* z) {
    __m128 xt;
    __m128 yt;
    *(vec3*)&xt = x->row0;
    xt = _mm_and_ps(xt, vec4_mask_vec3.data);
    yt = _mm_mul_ps(xt, xt);
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    z->row0 = *(vec3*)&xt;
    *(vec3*)&xt = x->row1;
    xt = _mm_and_ps(xt, vec4_mask_vec3.data);
    yt = _mm_mul_ps(xt, xt);
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    z->row1 = *(vec3*)&xt;
    *(vec3*)&xt = x->row2;
    xt = _mm_and_ps(xt, vec4_mask_vec3.data);
    yt = _mm_mul_ps(xt, xt);
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    z->row2 = *(vec3*)&xt;
}

inline float_t mat3_determinant(mat3* x) {
    vec3 xt0;
    vec3 xt1;
    vec3 xt2;
    xt0 = x->row0;
    xt1 = x->row1;
    xt2 = x->row2;
    float_t b00 = xt0.x * xt1.y * xt2.z;
    float_t b01 = xt0.y * xt1.z * xt2.x;
    float_t b02 = xt0.z * xt1.x * xt2.y;
    float_t b03 = xt0.z * xt1.y * xt2.x;
    float_t b04 = xt0.x * xt1.z * xt2.y;
    float_t b05 = xt0.y * xt1.x * xt2.z;
    return b00 + b01 + b02 - b03 - b04 - b05;
}

inline void mat3_rotate(float_t x, float_t y, float_t z, mat3* d) {
    mat3 dt;
    dt = mat3_identity;
    if (z != 0.0f)
        mat3_rotate_z(z, &dt);

    if (y != 0.0f)
        if (z != 0.0f)
            mat3_rotate_y_mult(&dt, y, &dt);
        else
            mat3_rotate_y(y, &dt);

    if (x != 0.0f)
        if (y != 0.0f || z != 0.0f)
            mat3_rotate_x_mult(&dt, x, &dt);
        else
            mat3_rotate_x(x, &dt);
    *d = dt;
}

inline void mat3_rotate_x(float_t x, mat3* y) {
    mat3 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    yt = mat3_identity;
    yt.row1.y = x_cos;
    yt.row1.z = x_sin;
    yt.row2.y = -x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

inline void mat3_rotate_y(float_t x, mat3* y) {
    mat3 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    yt = mat3_identity;
    yt.row0.x = x_cos;
    yt.row0.z = -x_sin;
    yt.row2.x = x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

inline void mat3_rotate_z(float_t x, mat3* y) {
    mat3 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    yt = mat3_identity;
    yt.row0.x = x_cos;
    yt.row0.y = x_sin;
    yt.row1.x = -x_sin;
    yt.row1.y = x_cos;
    *y = yt;
}

inline void mat3_rotate_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d) {
    mat3 dt;
    dt = *s;
    if (z != 0.0f)
        mat3_rotate_z_mult(&dt, z, &dt);
    if (y != 0.0f)
        mat3_rotate_y_mult(&dt, y, &dt);
    if (x != 0.0f)
        mat3_rotate_x_mult(&dt, x, &dt);
    *d = dt;
}

inline void mat3_rotate_x_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    yt = mat3_identity;
    yt.row1.y = y_cos;
    yt.row1.z = y_sin;
    yt.row2.y = -y_sin;
    yt.row2.z = y_cos;
    mat3_mult(&yt, x, z);
}

inline void mat3_rotate_y_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    yt = mat3_identity;
    yt.row0.x = y_cos;
    yt.row0.z = -y_sin;
    yt.row2.x = y_sin;
    yt.row2.z = y_cos;
    mat3_mult(&yt, x, z);
}

inline void mat3_rotate_z_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    yt = mat3_identity;
    yt.row0.x = y_cos;
    yt.row0.y = y_sin;
    yt.row1.x = -y_sin;
    yt.row1.y = y_cos;
    mat3_mult(&yt, x, z);
}

inline void mat3_scale(float_t x, float_t y, float_t z, mat3* d) {
    mat3 dt;
    dt = mat3_identity;
    if (x != 1.0f)
        dt.row0.x = x;
    if (y != 1.0f)
        dt.row1.y = y;
    if (z != 1.0f)
        dt.row2.z = z;
    *d = dt;
}

inline void mat3_scale_x(float_t x, mat3* y) {
    mat3 yt;
    yt = mat3_identity;
    yt.row0.x = x;
    *y = yt;
}

inline void mat3_scale_y(float_t x, mat3* y) {
    mat3 yt;
    yt = mat3_identity;
    yt.row1.y = x;
    *y = yt;
}

inline void mat3_scale_z(float_t x, mat3* y) {
    mat3 yt;
    yt = mat3_identity;
    yt.row2.z = x;
    *y = yt;
}

inline void mat3_scale_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d) {
    mat3 st;
    mat3 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f) {
        st = *s;
        dt = mat3_identity;
        dt.row0.x = x;
        dt.row1.y = y;
        dt.row2.z = z;
        mat3_mult(&st, &dt, &dt);
        *d = dt;
    }
    else
        *d = *s;
}

inline void mat3_scale_x_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    yt = mat3_identity;
    yt.row0.x = y;
    mat3_mult(x, &yt, z);
}

inline void mat3_scale_y_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    yt = mat3_identity;
    yt.row1.y = y;
    mat3_mult(x, &yt, z);
}

inline void mat3_scale_z_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    yt = mat3_identity;
    yt.row2.z = y;
    mat3_mult(x, &yt, z);
}

inline void mat3_from_quat(quat* quat, mat3* mat) {
    float_t y;
    float_t x;
    float_t z;
    float_t w;
    float_t len;
    float_t xy;
    float_t yy;
    float_t xx;
    float_t wx;
    float_t wy;
    float_t zz;
    float_t xz;
    float_t wz;
    float_t yz;

    x = quat->x;
    y = quat->y;
    z = quat->z;
    w = quat->w;
    vec4_length_squared(*quat, len);
    len = len > 0.0f ? 2.0f / len : 0.0f;
    xx = x * x * len;
    xy = x * y * len;
    xz = x * z * len;
    yy = y * y * len;
    zz = z * z * len;
    yz = y * z * len;
    wx = w * x * len;
    wy = w * y * len;
    wz = w * z * len;
    mat->row0.x = 1.0f - zz - yy;
    mat->row0.y = xy + wz;
    mat->row0.z = xz - wy;
    mat->row1.x = xy - wz;
    mat->row1.y = 1.0f - zz - xx;
    mat->row1.z = yz + wx;
    mat->row2.x = xz + wy;
    mat->row2.y = yz - wx;
    mat->row2.z = 1.0f - yy - xx;
}

inline void mat3_from_axis_angle(vec3* axis, float_t angle, mat3* mat) {
    quat quat;

    quat_from_axis_angle(axis, angle, &quat);
    mat3_from_quat(&quat, mat);
}

inline void mat3_from_mat4(mat4* x, mat3* z) {
    z->row0 = *(vec3*)&x->row0;
    z->row1 = *(vec3*)&x->row1;
    z->row2 = *(vec3*)&x->row2;
}

inline void mat3_from_mat4_inverse(mat4* x, mat3* z) {
    mat4 yt;

    mat4_inverse(x, &yt);
    z->row0 = *(vec3*)&yt.row0;
    z->row1 = *(vec3*)&yt.row1;
    z->row2 = *(vec3*)&yt.row2;
}

inline void mat3_get_rotation(mat3* x, vec3* z) {
    if (-x->row0.z >= 1.0f)
        z->y = (float_t)M_PI_2;
    else if (-x->row0.z <= -1.0f)
        z->y = (float_t)-M_PI_2;
    else
        z->y = asinf(-x->row0.z);
    if (fabs(x->row0.z) < 0.99999899f) {
        z->x = atan2f(x->row1.z, x->row2.z);
        z->z = atan2f(x->row0.y, x->row0.x);
    }
    else {
        z->x = 0.0f;
        z->z = atan2f(x->row2.y, x->row1.y);
        if (-x->row0.z < 0.0f)
            z->z = -z->z;
    }
}

inline void mat3_get_scale(mat3* x, vec3* z) {
    vec3_length(x->row0, z->x);
    vec3_length(x->row1, z->y);
    vec3_length(x->row2, z->z);
}

inline float_t mat3_get_max_scale(mat3* x) {
    mat3 mat;
    mat3_transpose(x, &mat);

    float_t length;
    float_t max = 0.0f;
    vec3_length(mat.row0, length);
    if (length > 0.0f)
        max = length;
    vec3_length(mat.row1, length);
    if (length > 0.0f)
        max = length;
    vec3_length(mat.row2, length);
    if (length > 0.0f)
        max = length;
    return max;
}

inline void mat4_add(mat4* x, mat4* y, mat4* z) {
    z->row0.data = _mm_add_ps(x->row0.data, y->row0.data);
    z->row1.data = _mm_add_ps(x->row1.data, y->row1.data);
    z->row2.data = _mm_add_ps(x->row2.data, y->row2.data);
    z->row3.data = _mm_add_ps(x->row3.data, y->row3.data);
}

inline void mat4_sub(mat4* x, mat4* y, mat4* z) {
    z->row0.data = _mm_sub_ps(x->row0.data, y->row0.data);
    z->row1.data = _mm_sub_ps(x->row1.data, y->row1.data);
    z->row2.data = _mm_sub_ps(x->row2.data, y->row2.data);
    z->row3.data = _mm_sub_ps(x->row3.data, y->row3.data);
}

inline void mat4_mult(mat4* x, mat4* y, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 t3;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = y->row0.data;
    y1 = y->row1.data;
    y2 = y->row2.data;
    y3 = y->row3.data;
    xt = x->row0.data;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row0.data = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    xt = x->row1.data;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row1.data = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    xt = x->row2.data;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row2.data = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    xt = x->row3.data;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row3.data = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
}

inline void mat4_mult_vec3(mat4* x, vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    *(vec3*)&yt = *y;
    zt0 = _mm_mul_ps(x->row0.data, _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(x->row1.data, _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(x->row2.data, _mm_shuffle_ps(yt, yt, 0xAA));
    zt = _mm_add_ps(_mm_add_ps(zt0, zt1), zt2);
    *z = *(vec3*)&zt;
}

inline void mat4_mult_vec3_inv(mat4* x, vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;

    *(vec3*)&yt = *y;
    yt = _mm_and_ps(yt, vec4_mask_vec3.data);
    zt = _mm_mul_ps(yt, x->row0.data);
    zt = _mm_hadd_ps(zt, zt);
    z->x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, x->row1.data);
    zt = _mm_hadd_ps(zt, zt);
    z->y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, x->row2.data);
    zt = _mm_hadd_ps(zt, zt);
    z->z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline void mat4_mult_vec3_trans(mat4* x, vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;
    *(vec3*)&yt = *y;
    zt0 = _mm_mul_ps(x->row0.data, _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(x->row1.data, _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(x->row2.data, _mm_shuffle_ps(yt, yt, 0xAA));
    zt3 = x->row3.data;
    zt = _mm_add_ps(_mm_add_ps(zt0, zt1), _mm_add_ps(zt2, zt3));
    *z = *(vec3*)&zt;
}

inline void mat4_mult_vec3_inv_trans(mat4* x, vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;

    *(vec3*)&yt = *y;
    yt = _mm_and_ps(yt, vec4_mask_vec3.data);
    yt = _mm_sub_ps(yt, x->row3.data);
    zt = _mm_mul_ps(yt, x->row0.data);
    zt = _mm_hadd_ps(zt, zt);
    z->x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, x->row1.data);
    zt = _mm_hadd_ps(zt, zt);
    z->y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, x->row2.data);
    zt = _mm_hadd_ps(zt, zt);
    z->z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline void mat4_mult_vec(mat4* x, vec4* y, vec4* z) {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;
    yt = y->data;
    zt0 = _mm_mul_ps(x->row0.data, _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(x->row1.data, _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(x->row2.data, _mm_shuffle_ps(yt, yt, 0xAA));
    zt3 = _mm_mul_ps(x->row3.data, _mm_shuffle_ps(yt, yt, 0xFF));
    z->data = _mm_add_ps(_mm_add_ps(zt0, zt1), _mm_add_ps(zt2, zt3));
}

inline void mat4_mult_scalar(mat4* x, float_t y, mat4* z) {
    __m128 yt;
    yt = _mm_set_ss(y);
    yt = _mm_shuffle_ps(yt, yt, 0);
    z->row0.data = _mm_mul_ps(x->row0.data, yt);
    z->row1.data = _mm_mul_ps(x->row1.data, yt);
    z->row2.data = _mm_mul_ps(x->row2.data, yt);
    z->row3.data = _mm_mul_ps(x->row3.data, yt);
}

inline void mat4_transpose(mat4* x, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    yt0 = _mm_unpacklo_ps(x->row0.data, x->row1.data);
    yt1 = _mm_unpackhi_ps(x->row0.data, x->row1.data);
    yt2 = _mm_unpacklo_ps(x->row2.data, x->row3.data);
    yt3 = _mm_unpackhi_ps(x->row2.data, x->row3.data);
    z->row0.data = _mm_movelh_ps(yt0, yt2);
    z->row1.data = _mm_movehl_ps(yt2, yt0);
    z->row2.data = _mm_movelh_ps(yt1, yt3);
    z->row3.data = _mm_movehl_ps(yt3, yt1);
}

inline void mat4_transpose(mat4* x, mat4u* z) {
    mat4 y;
    mat4_transpose(x, &y);
    *z = y;
}

inline void mat4_transpose(mat4u* x, mat4* z) {
    mat4 y = *x;
    mat4_transpose(&y, z);
}

inline void mat4_transpose(mat4u* x, mat4u* z) {
    mat4 y = *x;
    mat4_transpose(&y, &y);
    *z = y;
}

void mat4_inverse(mat4* x, mat4* z) {
    static const __m128 xor0 = { -0.0f,  0.0f, -0.0f,  0.0f };
    static const __m128 xor1 = {  0.0f, -0.0f,  0.0f, -0.0f };

    __m128 xt0, xt1, xt2, xt3;
    __m128 xt0x, xt1x, xt2x, xt3x;
    __m128 xt0y, xt1y, xt2y, xt3y;
    __m128 xt0z, xt1z, xt2z, xt3z;
    __m128 xt0w, xt1w, xt2w, xt3w;
    __m128 yt00, yt01, yt02, yt03, yt04, yt05, yt06, yt07, yt08, yt09, yt10, yt11;
    __m128 zt0, zt1, zt2, zt3;
    __m128 wt;
    __m128 wt0, wt1, wt2;
    __m128 t0, t1, t2, t3, t4, t5;
    xt0 = x->row0.data;
    xt1 = x->row1.data;
    xt2 = x->row2.data;
    xt3 = x->row3.data;
    xt0x = _mm_shuffle_ps(xt0, xt0, 0x00);
    xt0y = _mm_shuffle_ps(xt0, xt0, 0x55);
    xt0z = _mm_shuffle_ps(xt0, xt0, 0xAA);
    xt0w = _mm_shuffle_ps(xt0, xt0, 0xFF);
    xt1x = _mm_shuffle_ps(xt1, xt1, 0x00);
    xt1y = _mm_shuffle_ps(xt1, xt1, 0x55);
    xt1z = _mm_shuffle_ps(xt1, xt1, 0xAA);
    xt1w = _mm_shuffle_ps(xt1, xt1, 0xFF);
    xt2x = _mm_shuffle_ps(xt2, xt2, 0x00);
    xt2y = _mm_shuffle_ps(xt2, xt2, 0x55);
    xt2z = _mm_shuffle_ps(xt2, xt2, 0xAA);
    xt2w = _mm_shuffle_ps(xt2, xt2, 0xFF);
    xt3x = _mm_shuffle_ps(xt3, xt3, 0x00);
    xt3y = _mm_shuffle_ps(xt3, xt3, 0x55);
    xt3z = _mm_shuffle_ps(xt3, xt3, 0xAA);
    xt3w = _mm_shuffle_ps(xt3, xt3, 0xFF);
    yt00 = _mm_movelh_ps(_mm_unpacklo_ps(xt1y, xt0y), xt0y);
    yt01 = _mm_movelh_ps(xt2y, xt1y);
    yt02 = _mm_movelh_ps(xt3y, _mm_unpacklo_ps(xt3y, xt2y));
    yt03 = _mm_movelh_ps(xt2z, xt1z);
    yt04 = _mm_movelh_ps(xt3w, _mm_unpacklo_ps(xt3w, xt2w));
    yt05 = _mm_movelh_ps(xt2w, xt1w);
    yt06 = _mm_movelh_ps(_mm_unpacklo_ps(xt3z, xt3z), _mm_unpacklo_ps(xt3z, xt2z));
    yt07 = _mm_movelh_ps(_mm_unpacklo_ps(xt1z, xt0z), xt0z);
    yt08 = _mm_movelh_ps(_mm_unpacklo_ps(xt1w, xt0w), xt0w);
    yt09 = _mm_movelh_ps(_mm_unpacklo_ps(xt1x, xt0x), xt0x);
    yt10 = _mm_movelh_ps(xt2x, xt1x);
    yt11 = _mm_movelh_ps(xt3x, _mm_unpacklo_ps(xt3x, xt2x));

    t0 = _mm_sub_ps(_mm_mul_ps(yt03, yt04), _mm_mul_ps(yt05, yt06));
    t1 = _mm_sub_ps(_mm_mul_ps(yt07, yt04), _mm_mul_ps(yt08, yt06));
    t2 = _mm_sub_ps(_mm_mul_ps(yt07, yt05), _mm_mul_ps(yt08, yt03));

    t3 = _mm_xor_ps(yt09, xor0);
    t4 = _mm_xor_ps(yt10, xor1);
    t5 = _mm_xor_ps(yt11, xor0);

    wt0 = _mm_mul_ps(_mm_xor_ps(yt00, xor1), t0);
    wt1 = _mm_mul_ps(_mm_xor_ps(yt01, xor0), t1);
    wt2 = _mm_mul_ps(_mm_xor_ps(yt02, xor1), t2);
    zt0 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt0 = _mm_mul_ps(t3, t0);
    wt1 = _mm_mul_ps(t4, t1);
    wt2 = _mm_mul_ps(t5, t2);
    zt1 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt0 = _mm_mul_ps(_mm_xor_ps(yt09, xor1), _mm_sub_ps(_mm_mul_ps(yt01, yt04), _mm_mul_ps(yt05, yt02)));
    wt1 = _mm_mul_ps(_mm_xor_ps(yt10, xor0), _mm_sub_ps(_mm_mul_ps(yt00, yt04), _mm_mul_ps(yt08, yt02)));
    wt2 = _mm_mul_ps(_mm_xor_ps(yt11, xor1), _mm_sub_ps(_mm_mul_ps(yt00, yt05), _mm_mul_ps(yt08, yt01)));
    zt2 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt0 = _mm_mul_ps(t3, _mm_sub_ps(_mm_mul_ps(yt01, yt06), _mm_mul_ps(yt03, yt02)));
    wt1 = _mm_mul_ps(t4, _mm_sub_ps(_mm_mul_ps(yt00, yt06), _mm_mul_ps(yt07, yt02)));
    wt2 = _mm_mul_ps(t5, _mm_sub_ps(_mm_mul_ps(yt00, yt03), _mm_mul_ps(yt07, yt01)));
    zt3 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt = _mm_movelh_ps(_mm_unpacklo_ps(zt0, zt1), _mm_unpacklo_ps(zt2, zt3));
    wt = _mm_mul_ps(xt0, wt);
    wt = _mm_hadd_ps(wt, wt);
    wt = _mm_hadd_ps(wt, wt);
    if (wt.m128_f32[0] != 0.0f)
        wt.m128_f32[0] = 1.0f / wt.m128_f32[0];
    wt = _mm_shuffle_ps(wt, wt, 0);
    z->row0.data = _mm_mul_ps(zt0, wt);
    z->row1.data = _mm_mul_ps(zt1, wt);
    z->row2.data = _mm_mul_ps(zt2, wt);
    z->row3.data = _mm_mul_ps(zt3, wt);
}

inline void mat4_invtrans(mat4* x, mat4* z) {
    mat4 t = *x;
    mat4_inverse(&t, &t);
    mat4_transpose(&t, &t);
    *z = t;
}

inline void mat4_invrot(mat4* x, mat4* z) {
    mat3 yt;
    mat3_from_mat4(x, &yt);
    mat3_inverse(&yt, &yt);
    mat4_from_mat3(&yt, z);
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_inverse_normalized(mat4* x, mat4* z) {
    mat3 yt;
    mat3_from_mat4(x, &yt);
    vec3 row3;
    vec3_dot(*(vec3*)&x->row0, *(vec3*)&x->row3, row3.x);
    vec3_dot(*(vec3*)&x->row1, *(vec3*)&x->row3, row3.y);
    vec3_dot(*(vec3*)&x->row2, *(vec3*)&x->row3, row3.z);
    mat3_transpose(&yt, &yt);
    mat4_from_mat3(&yt, z);
    vec3_negate(row3, *(vec3*)&z->row3);
    z->row0.w = 0.0f;
    z->row1.w = 0.0f;
    z->row2.w = 0.0f;
    z->row3.w = 1.0f;
}

inline void mat4_invrot_normalized(mat4* x, mat4* z) {
    mat3 yt;
    mat3_from_mat4(x, &yt);
    mat3_transpose(&yt, &yt);
    mat4_from_mat3(&yt, z);
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_normalize(mat4* x, mat4* z) {
    __m128 det;
    det = _mm_set_ss(mat4_determinant(x));
    if (det.m128_f32[0] != 0.0f)
        det.m128_f32[0] = 1.0f / det.m128_f32[0];
    det = _mm_shuffle_ps(det, det, 0);
    z->row0.data = _mm_mul_ps(x->row0.data, det);
    z->row1.data = _mm_mul_ps(x->row1.data, det);
    z->row2.data = _mm_mul_ps(x->row2.data, det);
    z->row3.data = _mm_mul_ps(x->row3.data, det);
}

inline void mat4_normalize_rotation(mat4* x, mat4* z) {
    __m128 xt;
    __m128 yt;
    xt = _mm_and_ps(x->row0.data, vec4_mask_vec3.data);
    yt = _mm_mul_ps(xt, xt);
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    *(vec3*)&z->row0 = *(vec3*)&xt;
    xt = _mm_and_ps(x->row1.data, vec4_mask_vec3.data);
    yt = _mm_mul_ps(xt, xt);
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    *(vec3*)&z->row1 = *(vec3*)&xt;
    xt = _mm_and_ps(x->row2.data, vec4_mask_vec3.data);
    yt = _mm_mul_ps(xt, xt);
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    *(vec3*)&z->row2 = *(vec3*)&xt;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline float_t mat4_determinant(mat4* x) {
    vec4 xt0;
    vec4 xt1;
    vec4 xt2;
    vec4 xt3;
    xt0 = x->row0;
    xt1 = x->row1;
    xt2 = x->row2;
    xt3 = x->row3;
    float_t b00 = xt0.x * xt1.y - xt0.y * xt1.x;
    float_t b01 = xt0.x * xt1.z - xt0.z * xt1.x;
    float_t b02 = xt0.x * xt1.w - xt0.w * xt1.x;
    float_t b03 = xt0.y * xt1.z - xt0.z * xt1.y;
    float_t b04 = xt0.y * xt1.w - xt0.w * xt1.y;
    float_t b05 = xt0.z * xt1.w - xt0.w * xt1.z;
    float_t b06 = xt2.x * xt3.y - xt2.y * xt3.x;
    float_t b07 = xt2.x * xt3.z - xt2.z * xt3.x;
    float_t b08 = xt2.x * xt3.w - xt2.w * xt3.x;
    float_t b09 = xt2.y * xt3.z - xt2.z * xt3.y;
    float_t b10 = xt2.y * xt3.w - xt2.w * xt3.y;
    float_t b11 = xt2.z * xt3.w - xt2.w * xt3.z;
    return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
}

inline void mat4_rotate(float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = mat4_identity;
    if (z != 0.0f)
        mat4_rotate_z(z, &dt);

    if (y != 0.0f)
        if (z != 0.0f)
            mat4_rotate_y_mult(&dt, y, &dt);
        else
            mat4_rotate_y(y, &dt);

    if (x != 0.0f)
        if (y != 0.0f || z != 0.0f)
            mat4_rotate_x_mult(&dt, x, &dt);
        else
            mat4_rotate_x(x, &dt);
    *d = dt;
}

inline void mat4_rotate_x(float_t x, mat4* y) {
    mat4 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    yt = mat4_identity;
    yt.row1.y = x_cos;
    yt.row1.z = x_sin;
    yt.row2.y = -x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

inline void mat4_rotate_y(float_t x, mat4* y) {
    mat4 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    yt = mat4_identity;
    yt.row0.x = x_cos;
    yt.row0.z = -x_sin;
    yt.row2.x = x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

inline void mat4_rotate_z(float_t x, mat4* y) {
    mat4 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    yt = mat4_identity;
    yt.row0.x = x_cos;
    yt.row0.y = x_sin;
    yt.row1.x = -x_sin;
    yt.row1.y = x_cos;
    *y = yt;
}

inline void mat4_rotate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = *s;
    if (z != 0.0f)
        mat4_rotate_z_mult(&dt, z, &dt);
    if (y != 0.0f)
        mat4_rotate_y_mult(&dt, y, &dt);
    if (x != 0.0f)
        mat4_rotate_x_mult(&dt, x, &dt);
    *d = dt;
}

inline void mat4_rotate_x_mult(mat4* x, float_t y, mat4* z) {
    __m128 t1;
    __m128 t2;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    y0 = x->row0.data;
    y1 = x->row1.data;
    y2 = x->row2.data;
    y3 = x->row3.data;
    z->row0.data = y0;
    xt = _mm_set_ss(y_cos);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(y_sin);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row1.data = _mm_add_ps(t1, t2);
    xt = _mm_set_ss(-y_sin);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(y_cos);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row2.data = _mm_add_ps(t1, t2);
    z->row3.data = y3;
}

inline void mat4_rotate_y_mult(mat4* x, float_t y, mat4* z) {
    __m128 t0;
    __m128 t2;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    y0 = x->row0.data;
    y1 = x->row1.data;
    y2 = x->row2.data;
    y3 = x->row3.data;
    xt = _mm_set_ss(y_cos);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(-y_sin);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row0.data = _mm_add_ps(t0, t2);
    z->row1.data = y1;
    xt = _mm_set_ss(y_sin);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(y_cos);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row2.data = _mm_add_ps(t0, t2);
    z->row3.data = y3;
}

inline void mat4_rotate_z_mult(mat4* x, float_t y, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    y0 = x->row0.data;
    y1 = x->row1.data;
    y2 = x->row2.data;
    y3 = x->row3.data;
    xt = _mm_set_ss(y_cos);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(y_sin);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    z->row0.data = _mm_add_ps(t0, t1);
    xt = _mm_set_ss(-y_sin);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(y_cos);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    z->row1.data = _mm_add_ps(t0, t1);
    z->row2.data = y2;
    z->row3.data = y3;
}

inline void mat4_rotate_x_sin_cos(float_t sin_val, float_t cos_val, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row1.y = cos_val;
    yt.row1.z = sin_val;
    yt.row2.y = -sin_val;
    yt.row2.z = cos_val;
    *y = yt;
}

inline void mat4_rotate_y_sin_cos(float_t sin_val, float_t cos_val, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row0.x = cos_val;
    yt.row0.z = -sin_val;
    yt.row2.x = sin_val;
    yt.row2.z = cos_val;
    *y = yt;
}

inline void mat4_rotate_z_sin_cos(float_t sin_val, float_t cos_val, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row0.x = cos_val;
    yt.row0.y = sin_val;
    yt.row1.x = -sin_val;
    yt.row1.y = cos_val;
    *y = yt;
}

inline void mat4_rotate_x_mult_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    __m128 t1;
    __m128 t2;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = x->row0.data;
    y1 = x->row1.data;
    y2 = x->row2.data;
    y3 = x->row3.data;
    z->row0.data = y0;
    xt = _mm_set_ss(cos_val);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(sin_val);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row1.data = _mm_add_ps(t1, t2);
    xt = _mm_set_ss(-sin_val);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(cos_val);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row2.data = _mm_add_ps(t1, t2);
    z->row3.data = y3;
}

inline void mat4_rotate_y_mult_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    __m128 t0;
    __m128 t2;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = x->row0.data;
    y1 = x->row1.data;
    y2 = x->row2.data;
    y3 = x->row3.data;
    xt = _mm_set_ss(cos_val);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(-sin_val);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row0.data = _mm_add_ps(t0, t2);
    z->row1.data = y1;
    xt = _mm_set_ss(sin_val);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(cos_val);
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0x00));
    z->row2.data = _mm_add_ps(t0, t2);
    z->row3.data = y3;
}

inline void mat4_rotate_z_mult_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = x->row0.data;
    y1 = x->row1.data;
    y2 = x->row2.data;
    y3 = x->row3.data;
    xt = _mm_set_ss(cos_val);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(sin_val);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    z->row0.data = _mm_add_ps(t0, t1);
    xt = _mm_set_ss(-sin_val);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    xt = _mm_set_ss(cos_val);
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x00));
    z->row1.data = _mm_add_ps(t0, t1);
    z->row2.data = y2;
    z->row3.data = y3;
}

inline void mat4_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = *s;
    if (z != 0.0f)
        mat4_rot_z(&dt, z, &dt);
    if (y != 0.0f)
        mat4_rot_y(&dt, y, &dt);
    if (x != 0.0f)
        mat4_rot_x(&dt, x, &dt);
    *d = dt;
}

inline void mat4_rot_x(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    yt = mat3_identity;
    yt.row1.y = y_cos;
    yt.row1.z = y_sin;
    yt.row2.y = -y_sin;
    yt.row2.z = y_cos;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_rot_y(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    yt = mat3_identity;
    yt.row0.x = y_cos;
    yt.row0.z = -y_sin;
    yt.row2.x = y_sin;
    yt.row2.z = y_cos;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_rot_z(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    yt = mat3_identity;
    yt.row0.x = y_cos;
    yt.row0.y = y_sin;
    yt.row1.x = -y_sin;
    yt.row1.y = y_cos;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_rot_x_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    yt = mat3_identity;
    yt.row1.y = cos_val;
    yt.row1.z = sin_val;
    yt.row2.y = -sin_val;
    yt.row2.z = cos_val;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_rot_y_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    yt = mat3_identity;
    yt.row0.x = cos_val;
    yt.row0.z = -sin_val;
    yt.row2.x = sin_val;
    yt.row2.z = cos_val;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_rot_z_sin_cos(mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    yt = mat3_identity;
    yt.row0.x = cos_val;
    yt.row0.y = sin_val;
    yt.row1.x = -sin_val;
    yt.row1.y = cos_val;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_scale(float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = mat4_identity;
    if (x != 1.0f)
        dt.row0.x = x;
    if (y != 1.0f)
        dt.row1.y = y;
    if (z != 1.0f)
        dt.row2.z = z;
    *d = dt;
}

inline void mat4_scale_x(float_t x, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row0.x = x;
    *y = yt;
}

inline void mat4_scale_y(float_t x, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row1.y = x;
    *y = yt;
}

inline void mat4_scale_z(float_t x, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row2.z = x;
    *y = yt;
}

inline void mat4_scale_mult(mat4* s, float_t x, float_t y, float_t z, float_t w, mat4* d) {
    mat4 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f) {
        dt = mat4_identity;
        dt.row0.x = x;
        dt.row1.y = y;
        dt.row2.z = z;
        dt.row3.z = w;
        mat4_mult(s, &dt, d);
    }
    else if (s != d)
        *d = *s;
}

inline void mat4_scale_x_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row0.x = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_y_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row1.y = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_z_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row2.z = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_w_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row3.w = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat3 st;
    mat3 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f) {
        st.row0 = *(vec3*)&s->row0;
        st.row1 = *(vec3*)&s->row1;
        st.row2 = *(vec3*)&s->row2;
        dt = mat3_identity;
        dt.row0.x = x;
        dt.row1.y = y;
        dt.row2.z = z;
        mat3_mult(&st, &dt, &dt);
        *(vec3*)&d->row0 = dt.row0;
        *(vec3*)&d->row1 = dt.row1;
        *(vec3*)&d->row2 = dt.row2;
        d->row0.w = s->row0.w;
        d->row1.w = s->row1.w;
        d->row2.w = s->row2.w;
        d->row3 = s->row3;
    }
    else
        *d = *s;
}

inline void mat4_scale_x_rot(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    yt = mat3_identity;
    yt.row0.x = y;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_scale_y_rot(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    yt = mat3_identity;
    yt.row1.y = y;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_scale_z_rot(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    yt = mat3_identity;
    yt.row2.z = y;
    mat3_mult(&xt, &yt, &zt);
    *(vec3*)&z->row0 = zt.row0;
    *(vec3*)&z->row1 = zt.row1;
    *(vec3*)&z->row2 = zt.row2;
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_translate(float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = mat4_identity;
    dt.row3.x = x;
    dt.row3.y = y;
    dt.row3.z = z;
    *d = dt;
}

inline void mat4_translate_x(float_t x, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row3.x = x;
    *y = yt;
}

inline void mat4_translate_y(float_t x, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row3.y = x;
    *y = yt;
}

inline void mat4_translate_z(float_t x, mat4* y) {
    mat4 yt;
    yt = mat4_identity;
    yt.row3.z = x;
    *y = yt;
}

inline void mat4_translate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    __m128 yt4;
    if (s != d)
        *d = *s;
    if (x != 0.0f || y != 0.0f || z != 0.0f) {
        yt0 = s->row0.data;
        yt1 = s->row1.data;
        yt2 = s->row2.data;
        yt3 = s->row3.data;
        yt4 = _mm_set_ss(x);
        yt0 = _mm_mul_ps(yt0, _mm_shuffle_ps(yt4, yt4, 0));
        yt4 = _mm_set_ss(y);
        yt1 = _mm_mul_ps(yt1, _mm_shuffle_ps(yt4, yt4, 0));
        yt4 = _mm_set_ss(z);
        yt2 = _mm_mul_ps(yt2, _mm_shuffle_ps(yt4, yt4, 0));
        yt4 = _mm_add_ps(_mm_add_ps(yt0, yt1), _mm_add_ps(yt2, yt3));
        *(vec3*)&d->row3 = *(vec3*)&yt4;
    }
}

inline void mat4_translate_x_mult(mat4* x, float_t y, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    if (x != z)
        *z = *x;
    if (y != 0.0f) {
        yt0 = x->row0.data;
        yt1 = x->row3.data;
        yt2 = _mm_set_ss(y);
        yt0 = _mm_add_ps(_mm_mul_ps(yt0, _mm_shuffle_ps(yt2, yt2, 0)), yt1);
        *(vec3*)&z->row3 = *(vec3*)&yt0;
    }
}

inline void mat4_translate_y_mult(mat4* x, float_t y, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    if (x != z)
        *z = *x;
    if (y != 0.0f) {
        yt0 = x->row1.data;
        yt1 = x->row3.data;
        yt2 = _mm_set_ss(y);
        yt0 = _mm_add_ps(_mm_mul_ps(yt0, _mm_shuffle_ps(yt2, yt2, 0)), yt1);
        *(vec3*)&z->row3 = *(vec3*)&yt0;
    }
}

inline void mat4_translate_z_mult(mat4* x, float_t y, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    if (x != z)
        *z = *x;
    if (y != 0.0f) {
        yt0 = x->row2.data;
        yt1 = x->row3.data;
        yt2 = _mm_set_ss(y);
        yt0 = _mm_add_ps(_mm_mul_ps(yt0, _mm_shuffle_ps(yt2, yt2, 0)), yt1);
        *(vec3*)&z->row3 = *(vec3*)&yt0;
    }
}

inline void mat4_translate_add(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    if (s != d)
        *d = *s;
    if (x != 0.0f || y != 0.0f || z != 0.0f)
        d->row3.data = _mm_add_ps(s->row3.data, _mm_set_ps(x, y, z, 0.0f));
}

inline void mat4_translate_x_add(mat4* x, float_t y, mat4* z) {
    if (x != z)
        *z = *x;
    if (y != 0.0f)
        z->row3.x += y;
}

inline void mat4_translate_y_add(mat4* x, float_t y, mat4* z) {
    if (x != z)
        *z = *x;
    if (y != 0.0f)
        z->row3.y += y;
}

inline void mat4_translate_z_add(mat4* x, float_t y, mat4* z) {
    if (x != z)
        *z = *x;
    if (y != 0.0f)
        z->row3.z += y;
}

inline void mat4_from_quat(quat* quat, mat4* mat) {
    float_t y;
    float_t x;
    float_t z;
    float_t w;
    float_t len;
    float_t xy;
    float_t yy;
    float_t xx;
    float_t wx;
    float_t wy;
    float_t zz;
    float_t xz;
    float_t wz;
    float_t yz;

    x = quat->x;
    y = quat->y;
    z = quat->z;
    w = quat->w;
    vec4_length_squared(*quat, len);
    len = len > 0.0f ? 2.0f / len : 0.0f;
    xx = x * x * len;
    xy = x * y * len;
    xz = x * z * len;
    yy = y * y * len;
    zz = z * z * len;
    yz = y * z * len;
    wx = w * x * len;
    wy = w * y * len;
    wz = w * z * len;
    mat->row0.x = 1.0f - zz - yy;
    mat->row0.y = xy + wz;
    mat->row0.z = xz - wy;
    mat->row0.w = 0.0f;
    mat->row1.x = xy - wz;
    mat->row1.y = 1.0f - zz - xx;
    mat->row1.z = yz + wx;
    mat->row1.w = 0.0f;
    mat->row2.x = xz + wy;
    mat->row2.y = yz - wx;
    mat->row2.z = 1.0f - yy - xx;
    mat->row2.w = 0.0f;
    mat->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

float_t vec3_angle_between_two_vector_olds(vec3* x, vec3* y) {
    vec3 z_t;
    vec3_cross(*x, *y, z_t);
    float_t v2;
    vec3_length(z_t, v2);
    float_t v3;
    vec3_dot(*x, *y, v3);
    return fabsf(atan2f(v2, v3));
}

void mat4_from_two_vector_olds(vec3* x, vec3* y, mat4* mat) {
    *mat = mat4_identity;
    if (x->x == y->x && y->y == x->y && y->z == x->z)
        return;

    float_t v5;
    vec3_dot(*x, *y, v5);
    if (fabsf(1.0f - v5) <= 0.000001f)
        return;

    vec3 axis;
    vec3_cross(*x, *y, axis);
    float_t axis_length;
    vec3_length(axis, axis_length);
    if (axis_length > 0.000001f) {
        float_t angle = vec3_angle_between_two_vector_olds(x, y);
        if (axis_length != 0.0)
            vec3_mult_scalar(axis, 1.0f / axis_length, axis);
        mat4_from_axis_angle(&axis, angle, mat);
    }
}

inline void mat4_from_axis_angle(vec3* axis, float_t angle, mat4* mat) {
    float_t angle_sin;
    float_t angle_cos;
    float_t angle_cos_1;
    vec3 _axis;
    vec3 temp;

    angle_sin = sinf(angle);
    angle_cos = cosf(angle);
    angle_cos_1 = 1.0f - angle_cos;

    vec3_normalize(*axis, _axis);

    vec3_mult_scalar(_axis, _axis.x * angle_cos_1, temp);
    mat->row0.x = temp.x + angle_cos;
    mat->row0.y = temp.x - angle_sin * _axis.z;
    mat->row0.z = temp.x + angle_sin * _axis.y;
    vec3_mult_scalar(_axis, _axis.y * angle_cos_1, temp);
    mat->row1.x = temp.x + angle_sin * _axis.z;
    mat->row1.y = temp.y + angle_cos;
    mat->row1.z = temp.z - angle_sin * _axis.x;
    vec3_mult_scalar(_axis, _axis.z * angle_cos_1, temp);
    mat->row2.x = temp.x - angle_sin * _axis.y;
    mat->row2.y = temp.y + angle_sin * _axis.x;
    mat->row2.z = temp.z + angle_cos;
    mat->row0.w = 0.0f;
    mat->row1.w = 0.0f;
    mat->row2.w = 0.0f;
    mat->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

inline void mat4_from_mat3(mat3* x, mat4* z) {
    *(vec3*)&z->row0 = x->row0;
    z->row0.w = 0.0f;
    *(vec3*)&z->row1 = x->row1;
    z->row1.w = 0.0f;
    *(vec3*)&z->row2 = x->row2;
    z->row2.w = 0.0f;
    z->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

inline void mat4_from_mat3_inverse(mat3* x, mat4* z) {
    mat3 yt;

    mat3_inverse(x, &yt);
    *(vec3*)&z->row0 = yt.row0;
    z->row0.w = 0.0f;
    *(vec3*)&z->row1 = yt.row1;
    z->row1.w = 0.0f;
    *(vec3*)&z->row2 = yt.row2;
    z->row2.w = 0.0f;
    z->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

inline void mat4_clear_rot(mat4* x, mat4* z) {
    z->row0 = mat4_identity.row0;
    z->row1 = mat4_identity.row1;
    z->row2 = mat4_identity.row2;
    z->row3 = x->row3;
}

inline void mat4_clear_trans(mat4* x, mat4* z) {
    if (x != z) {
        z->row0 = x->row0;
        z->row1 = x->row1;
        z->row2 = x->row2;
    }
    z->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

inline void mat4_get_rotation(mat4* x, vec3* z) {
    if (-x->row0.z >= 1.0f)
        z->y = (float_t)M_PI_2;
    else if (-x->row0.z <= -1.0f)
        z->y = (float_t)-M_PI_2;
    else
        z->y = asinf(-x->row0.z);

    if (fabs(x->row0.z) < 0.99999899f) {
        z->x = atan2f(x->row1.z, x->row2.z);
        z->z = atan2f(x->row0.y, x->row0.x);
    }
    else {
        z->x = 0.0f;
        z->z = atan2f(x->row2.y, x->row1.y);
        if (x->row0.z > 0.0f)
            z->z = -z->z;
    }
}

inline void mat4_get_scale(mat4* x, vec3* z) {
    vec4_length(x->row0, z->x);
    vec4_length(x->row1, z->y);
    vec4_length(x->row2, z->z);
}

inline void mat4_get_translation(mat4* x, vec3* z) {
    *z = *(vec3*)&x->row3;
}

inline void mat4_set_translation(mat4* x, vec3* z) {
    *(vec3*)&x->row3 = *z;
}

inline void mat4_blend(mat4* x, mat4* y, mat4* z, float_t blend) {
    quat q1;
    quat q2;
    quat q3;

    quat_from_mat3(x->row0.x, x->row1.x, x->row2.x, x->row0.y,
        x->row1.y, x->row2.y, x->row0.z, x->row1.z, x->row2.z, &q1);
    quat_from_mat3(y->row0.x, y->row1.x, y->row2.x, y->row0.y,
        y->row1.y, y->row2.y, y->row0.z, y->row1.z, y->row2.z, &q1);

    vec3 t1;
    vec3 t2;
    vec3 t3;
    mat4_get_translation(x, &t1);
    mat4_get_translation(y, &t2);

    quat_slerp(&q3, &q1, &q2, blend);
    vec3_lerp_scalar(t1, t2, t3, blend);

    mat4_from_quat(&q3, z);
    mat4_set_translation(z, &t3);
}

inline void mat4_blend_rotation(mat4* x, mat4* y, mat4* z, float_t blend) {
    quat q0;
    quat q1;
    quat q2;

    quat_from_mat3(x->row0.x, x->row1.x, x->row2.x, x->row0.y,
        x->row1.y, x->row2.y, x->row0.z, x->row1.z, x->row2.z, &q1);
    quat_from_mat3(y->row0.x, y->row1.x, y->row2.x, y->row0.y,
        y->row1.y, y->row2.y, y->row0.z, y->row1.z, y->row2.z, &q1);
    quat_slerp(&q0, &q1, &q2, blend);
    mat4_from_quat(&q2, z);
}

void mat4_lerp_rotation(mat4* dst, mat4* src0, mat4* src1, float_t blend) {
    vec3 m0;
    vec3 m1;
    vec3_lerp_scalar(*(vec3*)&src0->row0, *(vec3*)&src1->row0, m0, blend);
    vec3_lerp_scalar(*(vec3*)&src0->row1, *(vec3*)&src1->row1, m1, blend);

    float_t m0_len_sq;
    float_t m1_len_sq;
    vec3_length_squared(m0, m0_len_sq);
    vec3_length_squared(m1, m1_len_sq);

    if (m0_len_sq <= 0.000001f || m1_len_sq <= 0.000001f) {
        *dst = *src1;
        return;
    }

    vec3 m2;
    vec3_cross(m0, m1, m2);
    vec3_cross(m2, m0, m1);

    float_t m2_len_sq;
    vec3_length_squared(m1, m1_len_sq);
    vec3_length_squared(m2, m2_len_sq);
    if (m2_len_sq <= 0.000001f || m1_len_sq <= 0.000001) {
        *dst = *src1;
        return;
    }

    float_t m0_len = sqrtf(m0_len_sq);
    if (m0_len != 0.0f)
        vec3_div_scalar(m0, 1.0f / m0_len, m0);

    float_t m1_len = sqrtf(m1_len_sq);
    if (m1_len != 0.0f)
        vec3_div_scalar(m1, 1.0f / m1_len, m1);

    float_t m2_len = sqrtf(m2_len_sq);
    if (m2_len != 0.0f)
        vec3_div_scalar(m2, 1.0f / m2_len, m2);

    *dst = mat4_identity;
    *(vec3*)&dst->row0 = m0;
    *(vec3*)&dst->row1 = m1;
    *(vec3*)&dst->row2 = m2;
}

inline float_t mat4_get_max_scale(mat4* x) {
    mat4 mat;
    mat4_transpose(x, &mat);

    float_t length;
    float_t max = 0.0f;
    vec3_length(*(vec3*)&mat.row0, length);
    if (length > 0.0f)
        max = length;
    vec3_length(*(vec3*)&mat.row1, length);
    if (length > 0.0f)
        max = length;
    vec3_length(*(vec3*)&mat.row2, length);
    if (length > 0.0f)
        max = length;
    return max;
}

inline void mat4_ortho(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* mat) {
    *mat = mat4_identity;
    mat->row0.x = (float_t)(2.0 / (right - left));
    mat->row1.y = (float_t)(2.0 / (top - bottom));
    mat->row2.z = (float_t)(-2.0 / (z_far - z_near));
    mat->row3.x = -(float_t)((right + left) / (right - left));
    mat->row3.y = -(float_t)((top + bottom) / (top - bottom));
    mat->row3.z = -(float_t)((z_far + z_near) / (z_far - z_near));
}

inline void mat4_persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far, mat4* mat) {
    double_t tan_fov = tan(fov_y * 0.5);

    double_t x = 1.0 / (aspect * tan_fov);
    double_t y = 1.0 / tan_fov;
    double_t d = -(2.0 * z_far * z_near) / (z_far - z_near);
    double_t e = -(z_far + z_near) / (z_far - z_near);

    *mat = mat4_null;
    mat->row0.x = (float_t)x;
    mat->row1.y = (float_t)y;
    mat->row2.z = (float_t)e;
    mat->row2.w = -1.0f;
    mat->row3.z = (float_t)d;
}

inline void mat4_look_at(vec3* eye, vec3* target, vec3* up, mat4* mat) {
    vec3 x_axis, y_axis, z_axis;
    float_t t;
    vec3 xyz;

    vec3_sub(*eye, *target, z_axis);
    vec3_normalize(z_axis, z_axis);

    vec3_cross(*up, z_axis, x_axis);
    vec3_normalize(x_axis, x_axis);
    vec3_length(x_axis, t);
    if (t == 0.0f)
        x_axis = { 1.0f, 0.0f, 0.0f };
    vec3_cross(z_axis, x_axis, y_axis);
    vec3_normalize(y_axis, y_axis);

    vec3_dot(x_axis, *eye, xyz.x);
    vec3_dot(y_axis, *eye, xyz.y);
    vec3_dot(z_axis, *eye, xyz.z);

    mat->row0 = { x_axis.x, y_axis.x, z_axis.x, 0.0f };
    mat->row1 = { x_axis.y, y_axis.y, z_axis.y, 0.0f };
    mat->row2 = { x_axis.z, y_axis.z, z_axis.z, 0.0f };
    vec3_negate(xyz, xyz);
    *(vec3*)&mat->row3 = xyz;
    mat->row3.w = 1.0f;
}

mat4::operator mat4u() const {
    mat4u m;
    m.row0 = row0;
    m.row1 = row1;
    m.row2 = row2;
    m.row3 = row3;
    return m;
}

mat4u::operator mat4() const {
    mat4 m;
    m.row0 = row0;
    m.row1 = row1;
    m.row2 = row2;
    m.row3 = row3;
    return m;
}
