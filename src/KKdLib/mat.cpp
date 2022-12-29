/*
    by korenkonder
    GitHub/GitLab: korenkonder
    Matrix Inverse algo: https://github.com/niswegmann/small-matrix-inverse
*/

#include "mat.hpp"

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

inline void mat3_add(const mat3* x, const mat3* y, mat3* z) {
    z->row0 = x->row0 + y->row0;
    z->row1 = x->row1 + y->row1;
    z->row2 = x->row2 + y->row2;
}

inline void mat3_sub(const mat3* x, const mat3* y, mat3* z) {
    z->row0 = x->row0 - y->row0;
    z->row1 = x->row1 - y->row1;
    z->row2 = x->row2 - y->row2;
}

inline void mat3_mult(const mat3* x, const mat3* y, mat3* z) {
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

inline void mat3_mult_vec(const mat3* x, const vec3* y, vec3* z) {
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

inline void mat3_mult_scalar(const mat3* x, float_t y, mat3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    yt = vec4::load_xmm(y);
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

inline void mat3_transpose(const mat3* x, mat3* z) {
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
    xt0 = vec3::load_xmm(x->row0);
    xt1 = vec3::load_xmm(x->row1);
    xt2 = vec3::load_xmm(x->row2);
    xt3 = vec4::load_xmm(0.0f);
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

void mat3_inverse(const mat3* x, mat3* z) {
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
    yt = vec3::load_xmm(xt0);
    wt = _mm_mul_ps(yt, wt);
    wt = _mm_hadd_ps(wt, wt);
    wt = _mm_hadd_ps(wt, wt);
    if (_mm_cvtss_f32(wt) != 0.0f)
        wt = _mm_div_ss(_mm_set_ss(1.0f), wt);
    wt = _mm_shuffle_ps(wt, wt, 0);
    yt = _mm_mul_ps(zt0, wt);
    z->row0 = *(vec3*)&yt;
    yt = _mm_mul_ps(zt1, wt);
    z->row1 = *(vec3*)&yt;
    yt = _mm_mul_ps(zt2, wt);
    z->row2 = *(vec3*)&yt;
}

inline void mat3_invtrans(const mat3* x, mat3* z) {
    mat3 t = *x;
    mat3_inverse(&t, &t);
    mat3_transpose(&t, &t);
    *z = t;
}

inline void mat3_inverse_normalized(const mat3* x, mat3* z) {
    mat3_transpose(x, z);
}

inline void mat3_normalize(const mat3* x, mat3* z) {
    __m128 det;
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    det = _mm_set_ss(mat3_determinant(x));
    if (_mm_cvtss_f32(det) != 0.0f)
        det = _mm_div_ss(_mm_set_ss(1.0f), det);
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

inline void mat3_normalize_rotation(const mat3* x, mat3* z) {
    z->row0 = vec3::normalize(x->row0);
    z->row1 = vec3::normalize(x->row1);
    z->row2 = vec3::normalize(x->row2);
}

inline float_t mat3_determinant(const mat3* x) {
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

inline void mat3_rotate_x_sin_cos(float_t sin_val, float_t cos_val, mat3* y) {
    mat3 yt;
    yt = mat3_identity;
    yt.row1.y = cos_val;
    yt.row1.z = sin_val;
    yt.row2.y = -sin_val;
    yt.row2.z = cos_val;
    *y = yt;
}

inline void mat3_rotate_y_sin_cos(float_t sin_val, float_t cos_val, mat3* y) {
    mat3 yt;
    yt = mat3_identity;
    yt.row0.x = cos_val;
    yt.row0.z = -sin_val;
    yt.row2.x = sin_val;
    yt.row2.z = cos_val;
    *y = yt;
}

inline void mat3_rotate_z_sin_cos(float_t sin_val, float_t cos_val, mat3* y) {
    mat3 yt;
    yt = mat3_identity;
    yt.row0.x = cos_val;
    yt.row0.y = sin_val;
    yt.row1.x = -sin_val;
    yt.row1.y = cos_val;
    *y = yt;
}

inline void mat3_rotate_x_mult_sin_cos(const mat3* x, float_t sin_val, float_t cos_val, mat3* z) {
    __m128 t1;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 zt;
    *(vec3*)&y0 = x->row0;
    *(vec3*)&y1 = x->row1;
    *(vec3*)&y2 = x->row2;
    z->row0 = *(vec3*)&y0;
    t1 = _mm_mul_ps(y1, vec4::load_xmm(cos_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(sin_val));
    zt = _mm_add_ps(t1, t2);
    z->row1 = *(vec3*)&zt;
    t1 = _mm_mul_ps(y1, vec4::load_xmm(-sin_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(cos_val));
    zt = _mm_add_ps(t1, t2);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_rotate_y_mult_sin_cos(const mat3* x, float_t sin_val, float_t cos_val, mat3* z) {
    __m128 t0;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 zt;
    *(vec3*)&y0 = x->row0;
    *(vec3*)&y1 = x->row1;
    *(vec3*)&y2 = x->row2;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(cos_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(-sin_val));
    zt = _mm_add_ps(t0, t2);
    z->row0 = *(vec3*)&zt;
    z->row1 = *(vec3*)&y1;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(sin_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(cos_val));
    zt = _mm_add_ps(t0, t2);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_rotate_z_mult_sin_cos(const mat3* x, float_t sin_val, float_t cos_val, mat3* z) {
    __m128 t0;
    __m128 t1;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 zt;
    *(vec3*)&y0 = x->row0;
    *(vec3*)&y1 = x->row1;
    *(vec3*)&y2 = x->row2;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(cos_val));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(sin_val));
    zt = _mm_add_ps(t0, t1);
    z->row0 = *(vec3*)&zt;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(-sin_val));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(cos_val));
    zt = _mm_add_ps(t0, t1);
    z->row1 = *(vec3*)&zt;
    z->row2 = *(vec3*)&y2;
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

inline void mat3_rotate_mult(const mat3* s, float_t x, float_t y, float_t z, mat3* d) {
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

inline void mat3_rotate_x_mult(const mat3* x, float_t y, mat3* z) {
    __m128 t1;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 zt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    *(vec3*)&y0 = x->row0;
    *(vec3*)&y1 = x->row1;
    *(vec3*)&y2 = x->row2;
    z->row0 = *(vec3*)&y0;
    t1 = _mm_mul_ps(y1, vec4::load_xmm(y_cos));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(y_sin));
    zt = _mm_add_ps(t1, t2);
    z->row1 = *(vec3*)&zt;
    t1 = _mm_mul_ps(y1, vec4::load_xmm(-y_sin));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(y_cos));
    zt = _mm_add_ps(t1, t2);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_rotate_y_mult(const mat3* x, float_t y, mat3* z) {
    __m128 t0;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 zt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    *(vec3*)&y0 = x->row0;
    *(vec3*)&y1 = x->row1;
    *(vec3*)&y2 = x->row2;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(y_cos));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(-y_sin));
    zt = _mm_add_ps(t0, t2);
    z->row0 = *(vec3*)&zt;
    z->row1 = *(vec3*)&y1;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(y_sin));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(y_cos));
    zt = _mm_add_ps(t0, t2);
    z->row2 = *(vec3*)&zt;
}

inline void mat3_rotate_z_mult(const mat3* x, float_t y, mat3* z) {
    __m128 t0;
    __m128 t1;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 zt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    *(vec3*)&y0 = x->row0;
    *(vec3*)&y1 = x->row1;
    *(vec3*)&y2 = x->row2;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(y_cos));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(y_sin));
    zt = _mm_add_ps(t0, t1);
    z->row0 = *(vec3*)&zt;
    t0 = _mm_mul_ps(y0, vec4::load_xmm(-y_sin));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(y_cos));
    zt = _mm_add_ps(t0, t1);
    z->row1 = *(vec3*)&zt;
    z->row2 = *(vec3*)&y2;
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

inline void mat3_scale_mult(const mat3* s, float_t x, float_t y, float_t z, mat3* d) {
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

inline void mat3_scale_x_mult(const mat3* x, float_t y, mat3* z) {
    mat3 yt;
    yt = mat3_identity;
    yt.row0.x = y;
    mat3_mult(x, &yt, z);
}

inline void mat3_scale_y_mult(const mat3* x, float_t y, mat3* z) {
    mat3 yt;
    yt = mat3_identity;
    yt.row1.y = y;
    mat3_mult(x, &yt, z);
}

inline void mat3_scale_z_mult(const mat3* x, float_t y, mat3* z) {
    mat3 yt;
    yt = mat3_identity;
    yt.row2.z = y;
    mat3_mult(x, &yt, z);
}

inline void mat3_from_quat(const quat* quat, mat3* mat) {
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
    len = quat::length_squared(*quat);
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

inline void mat3_from_axis_angle(const vec3* axis, float_t angle, mat3* mat) {
    quat quat;

    quat_from_axis_angle(axis, angle, &quat);
    mat3_from_quat(&quat, mat);
}

inline void mat3_from_mat4(const mat4* x, mat3* z) {
    z->row0 = *(vec3*)&x->row0;
    z->row1 = *(vec3*)&x->row1;
    z->row2 = *(vec3*)&x->row2;
}

inline void mat3_from_mat4_inverse(const mat4* x, mat3* z) {
    mat4 yt;

    mat4_inverse(x, &yt);
    z->row0 = *(vec3*)&yt.row0;
    z->row1 = *(vec3*)&yt.row1;
    z->row2 = *(vec3*)&yt.row2;
}

inline void mat3_get_rotation(const mat3* x, vec3* z) {
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

inline void mat3_get_scale(const mat3* x, vec3* z) {
    z->x = vec3::length(x->row0);
    z->y = vec3::length(x->row1);
    z->z = vec3::length(x->row2);
}

inline float_t mat3_get_max_scale(const mat3* x) {
    mat3 mat;
    mat3_transpose(x, &mat);

    float_t length;
    float_t max = 0.0f;
    length = vec3::length(mat.row0);
    if (max < length)
        max = length;
    length = vec3::length(mat.row1);
    if (max < length)
        max = length;
    length = vec3::length(mat.row2);
    if (max < length)
        max = length;
    return max;
}

inline void mat3_mult_axis_angle(const mat3* x, const vec3* axis, const float_t angle, mat3* z) {
    quat q1;
    quat q2;
    quat q3;
    quat_from_mat3(x->row0.x, x->row1.x, x->row2.x, x->row0.y,
        x->row1.y, x->row2.y, x->row0.z, x->row1.z, x->row2.z, &q1);
    quat_from_axis_angle(axis, angle, &q2);
    quat_mult(&q2, &q1, &q3);
    mat3_from_quat(&q3, z);
}

inline void mat4_add(const mat4* x, const mat4* y, mat4* z) {
    z->row0 = x->row0 + y->row0;
    z->row1 = x->row1 + y->row1;
    z->row2 = x->row2 + y->row2;
    z->row3 = x->row3 + y->row3;
}

inline void mat4_sub(const mat4* x, const mat4* y, mat4* z) {
    z->row0 = x->row0 - y->row0;
    z->row1 = x->row1 - y->row1;
    z->row2 = x->row2 - y->row2;
    z->row3 = x->row3 - y->row3;
}

inline void mat4_mult(const mat4* x, const mat4* y, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 t3;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = vec4::load_xmm(y->row0);
    y1 = vec4::load_xmm(y->row1);
    y2 = vec4::load_xmm(y->row2);
    y3 = vec4::load_xmm(y->row3);
    xt = vec4::load_xmm(x->row0);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row0 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
    xt = vec4::load_xmm(x->row1);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row1 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
    xt = vec4::load_xmm(x->row2);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row2 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
    xt = vec4::load_xmm(x->row3);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    z->row3 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
}

inline void mat4_mult_vec3(const mat4* x, const vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    *(vec3*)&yt = *y;
    zt0 = _mm_mul_ps(vec4::load_xmm(x->row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(x->row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec4::load_xmm(x->row2), _mm_shuffle_ps(yt, yt, 0xAA));
    zt = _mm_add_ps(_mm_add_ps(zt0, zt1), zt2);
    *z = *(vec3*)&zt;
}

inline void mat4_mult_vec3_inv(const mat4* x, const vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;
    yt = vec3::load_xmm(*y);
    zt = _mm_mul_ps(yt, vec4::load_xmm(x->row0));
    zt = _mm_hadd_ps(zt, zt);
    z->x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(x->row1));
    zt = _mm_hadd_ps(zt, zt);
    z->y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(x->row2));
    zt = _mm_hadd_ps(zt, zt);
    z->z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline void mat4_mult_vec3_trans(const mat4* x, const vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;
    *(vec3*)&yt = *y;
    zt0 = _mm_mul_ps(vec4::load_xmm(x->row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(x->row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec4::load_xmm(x->row2), _mm_shuffle_ps(yt, yt, 0xAA));
    zt3 = vec4::load_xmm(x->row3);
    zt = _mm_add_ps(_mm_add_ps(zt0, zt1), _mm_add_ps(zt2, zt3));
    *z = *(vec3*)&zt;
}

inline void mat4_mult_vec3_inv_trans(const mat4* x, const vec3* y, vec3* z) {
    __m128 yt;
    __m128 zt;
    yt = vec3::load_xmm(*y);
    yt = _mm_sub_ps(yt, vec4::load_xmm(x->row3));
    zt = _mm_mul_ps(yt, vec4::load_xmm(x->row0));
    zt = _mm_hadd_ps(zt, zt);
    z->x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(x->row1));
    zt = _mm_hadd_ps(zt, zt);
    z->y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(x->row2));
    zt = _mm_hadd_ps(zt, zt);
    z->z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

inline void mat4_mult_vec(const mat4* x, vec4* y, vec4* z) {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;
    yt = vec4::load_xmm(*y);
    zt0 = _mm_mul_ps(vec4::load_xmm(x->row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(x->row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec4::load_xmm(x->row2), _mm_shuffle_ps(yt, yt, 0xAA));
    zt3 = _mm_mul_ps(vec4::load_xmm(x->row3), _mm_shuffle_ps(yt, yt, 0xFF));
    *z = vec4::store_xmm(_mm_add_ps(_mm_add_ps(zt0, zt1), _mm_add_ps(zt2, zt3)));
}

inline void mat4_mult_scalar(const mat4* x, float_t y, mat4* z) {
    __m128 yt;
    yt = vec4::load_xmm(y);
    z->row0 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row0), yt));
    z->row1 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row1), yt));
    z->row2 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row2), yt));
    z->row3 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row3), yt));
}

inline void mat4_transpose(const mat4* x, mat4* z) {
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    __m128 xt3;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    xt0 = vec4::load_xmm(x->row0);
    xt1 = vec4::load_xmm(x->row1);
    xt2 = vec4::load_xmm(x->row2);
    xt3 = vec4::load_xmm(x->row3);
    yt0 = _mm_unpacklo_ps(xt0, xt1);
    yt1 = _mm_unpackhi_ps(xt0, xt1);
    yt2 = _mm_unpacklo_ps(xt2, xt3);
    yt3 = _mm_unpackhi_ps(xt2, xt3);
    z->row0 = vec4::store_xmm(_mm_movelh_ps(yt0, yt2));
    z->row1 = vec4::store_xmm(_mm_movehl_ps(yt2, yt0));
    z->row2 = vec4::store_xmm(_mm_movelh_ps(yt1, yt3));
    z->row3 = vec4::store_xmm(_mm_movehl_ps(yt3, yt1));
}

void mat4_inverse(const mat4* x, mat4* z) {
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
    xt0 = vec4::load_xmm(x->row0);
    xt1 = vec4::load_xmm(x->row1);
    xt2 = vec4::load_xmm(x->row2);
    xt3 = vec4::load_xmm(x->row3);
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
    if (_mm_cvtss_f32(wt) != 0.0f)
        wt = _mm_div_ss(_mm_set_ss(1.0f), wt);
    wt = _mm_shuffle_ps(wt, wt, 0);
    z->row0 = vec4::store_xmm(_mm_mul_ps(zt0, wt));
    z->row1 = vec4::store_xmm(_mm_mul_ps(zt1, wt));
    z->row2 = vec4::store_xmm(_mm_mul_ps(zt2, wt));
    z->row3 = vec4::store_xmm(_mm_mul_ps(zt3, wt));
}

inline void mat4_invtrans(const mat4* x, mat4* z) {
    mat4 t = *x;
    mat4_inverse(&t, &t);
    mat4_transpose(&t, &t);
    *z = t;
}

inline void mat4_invrot(const mat4* x, mat4* z) {
    mat3 yt;
    mat3_from_mat4(x, &yt);
    mat3_inverse(&yt, &yt);
    mat4_from_mat3(&yt, z);
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_inverse_normalized(const mat4* x, mat4* z) {
    mat3 yt;
    mat3_from_mat4(x, &yt);
    vec3 row3;
    row3.x = vec3::dot(*(vec3*)&x->row0, *(vec3*)&x->row3);
    row3.y = vec3::dot(*(vec3*)&x->row1, *(vec3*)&x->row3);
    row3.z = vec3::dot(*(vec3*)&x->row2, *(vec3*)&x->row3);
    mat3_transpose(&yt, &yt);
    mat4_from_mat3(&yt, z);
    *(vec3*)&z->row3 = -row3;
    z->row0.w = 0.0f;
    z->row1.w = 0.0f;
    z->row2.w = 0.0f;
    z->row3.w = 1.0f;
}

inline void mat4_invrot_normalized(const mat4* x, mat4* z) {
    mat3 yt;
    mat3_from_mat4(x, &yt);
    mat3_transpose(&yt, &yt);
    mat4_from_mat3(&yt, z);
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline void mat4_normalize(const mat4* x, mat4* z) {
    __m128 det;
    det = _mm_set_ss(mat4_determinant(x));
    if (_mm_cvtss_f32(det) != 0.0f)
        det = _mm_div_ss(_mm_set_ss(1.0f), det);
    det = _mm_shuffle_ps(det, det, 0);
    z->row0 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row0), det));
    z->row1 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row1), det));
    z->row2 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row2), det));
    z->row3 = vec4::store_xmm(_mm_mul_ps(vec4::load_xmm(x->row3), det));
}

inline void mat4_normalize_rotation(const mat4* x, mat4* z) {
    *(vec3*)&z->row0 = vec3::normalize(*(vec3*)&x->row0);
    *(vec3*)&z->row1 = vec3::normalize(*(vec3*)&x->row1);
    *(vec3*)&z->row2 = vec3::normalize(*(vec3*)&x->row2);
    z->row0.w = x->row0.w;
    z->row1.w = x->row1.w;
    z->row2.w = x->row2.w;
    z->row3 = x->row3;
}

inline float_t mat4_determinant(const mat4* x) {
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

inline void mat4_rotate_mult(const mat4* s, float_t x, float_t y, float_t z, mat4* d) {
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

inline void mat4_rotate_x_mult(const mat4* x, float_t y, mat4* z) {
    __m128 t1;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    y0 = vec4::load_xmm(x->row0);
    y1 = vec4::load_xmm(x->row1);
    y2 = vec4::load_xmm(x->row2);
    y3 = vec4::load_xmm(x->row3);
    z->row0 = vec4::store_xmm(y0);
    t1 = _mm_mul_ps(y1, vec4::load_xmm(y_cos));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(y_sin));
    z->row1 = vec4::store_xmm(_mm_add_ps(t1, t2));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(-y_sin));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(y_cos));
    z->row2 = vec4::store_xmm(_mm_add_ps(t1, t2));
    z->row3 = vec4::store_xmm(y3);
}

inline void mat4_rotate_y_mult(const mat4* x, float_t y, mat4* z) {
    __m128 t0;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    y0 = vec4::load_xmm(x->row0);
    y1 = vec4::load_xmm(x->row1);
    y2 = vec4::load_xmm(x->row2);
    y3 = vec4::load_xmm(x->row3);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(y_cos));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(-y_sin));
    z->row0 = vec4::store_xmm(_mm_add_ps(t0, t2));
    z->row1 = vec4::store_xmm(y1);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(y_sin));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(y_cos));
    z->row2 = vec4::store_xmm(_mm_add_ps(t0, t2));
    z->row3 = vec4::store_xmm(y3);
}

inline void mat4_rotate_z_mult(const mat4* x, float_t y, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    y0 = vec4::load_xmm(x->row0);
    y1 = vec4::load_xmm(x->row1);
    y2 = vec4::load_xmm(x->row2);
    y3 = vec4::load_xmm(x->row3);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(y_cos));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(y_sin));
    z->row0 = vec4::store_xmm(_mm_add_ps(t0, t1));
    t0 = _mm_mul_ps(y0, vec4::load_xmm(-y_sin));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(y_cos));
    z->row1 = vec4::store_xmm(_mm_add_ps(t0, t1));
    z->row2 = vec4::store_xmm(y2);
    z->row3 = vec4::store_xmm(y3);
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

inline void mat4_rotate_x_mult_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    __m128 t1;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = vec4::load_xmm(x->row0);
    y1 = vec4::load_xmm(x->row1);
    y2 = vec4::load_xmm(x->row2);
    y3 = vec4::load_xmm(x->row3);
    z->row0 = vec4::store_xmm(y0);
    t1 = _mm_mul_ps(y1, vec4::load_xmm(cos_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(sin_val));
    z->row1 = vec4::store_xmm(_mm_add_ps(t1, t2));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(-sin_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(cos_val));
    z->row2 = vec4::store_xmm(_mm_add_ps(t1, t2));
    z->row3 = vec4::store_xmm(y3);
}

inline void mat4_rotate_y_mult_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    __m128 t0;
    __m128 t2;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = vec4::load_xmm(x->row0);
    y1 = vec4::load_xmm(x->row1);
    y2 = vec4::load_xmm(x->row2);
    y3 = vec4::load_xmm(x->row3);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(cos_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(-sin_val));
    z->row0 = vec4::store_xmm(_mm_add_ps(t0, t2));
    z->row1 = vec4::store_xmm(y1);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(sin_val));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(cos_val));
    z->row2 = vec4::store_xmm(_mm_add_ps(t0, t2));
    z->row3 = vec4::store_xmm(y3);
}

inline void mat4_rotate_z_mult_sin_cos(const mat4* x, float_t sin_val, float_t cos_val, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = vec4::load_xmm(x->row0);
    y1 = vec4::load_xmm(x->row1);
    y2 = vec4::load_xmm(x->row2);
    y3 = vec4::load_xmm(x->row3);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(cos_val));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(sin_val));
    z->row0 = vec4::store_xmm(_mm_add_ps(t0, t1));
    t0 = _mm_mul_ps(y0, vec4::load_xmm(-sin_val));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(cos_val));
    z->row1 = vec4::store_xmm(_mm_add_ps(t0, t1));
    z->row2 = vec4::store_xmm(y2);
    z->row3 = vec4::store_xmm(y3);
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

inline void mat4_scale_mult(const mat4* s, float_t x, float_t y, float_t z, float_t w, mat4* d) {
    mat4 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f || w != 1.0f) {
        dt = mat4_identity;
        dt.row0.x = x;
        dt.row1.y = y;
        dt.row2.z = z;
        dt.row3.w = w;
        mat4_mult(s, &dt, d);
    }
    else if (s != d)
        *d = *s;
}

inline void mat4_scale_x_mult(const mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row0.x = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_y_mult(const mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row1.y = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_z_mult(const mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row2.z = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_w_mult(const mat4* x, float_t y, mat4* z) {
    mat4 yt;
    yt = mat4_identity;
    yt.row3.w = y;
    mat4_mult(x, &yt, z);
}

inline void mat4_scale_rot(const mat4* s, float_t x, float_t y, float_t z, mat4* d) {
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

inline void mat4_scale_x_rot(const mat4* x, float_t y, mat4* z) {
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

inline void mat4_scale_y_rot(const mat4* x, float_t y, mat4* z) {
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

inline void mat4_scale_z_rot(const mat4* x, float_t y, mat4* z) {
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

inline void mat4_translate_mult(const mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    __m128 yt;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    if (s != d)
        *d = *s;
    if (x != 0.0f || y != 0.0f || z != 0.0f) {
        yt0 = _mm_mul_ps(vec4::load_xmm(s->row0), vec4::load_xmm(x));
        yt1 = _mm_mul_ps(vec4::load_xmm(s->row1), vec4::load_xmm(y));
        yt2 = _mm_mul_ps(vec4::load_xmm(s->row2), vec4::load_xmm(z));
        yt3 = vec4::load_xmm(s->row3);
        yt = _mm_add_ps(_mm_add_ps(yt0, yt1), _mm_add_ps(yt2, yt3));
        *(vec3*)&d->row3 = vec3::store_xmm(yt);
    }
}

inline void mat4_translate_x_mult(const mat4* x, float_t y, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    if (x != z)
        *z = *x;
    if (y != 0.0f) {
        yt0 = vec4::load_xmm(x->row0);
        yt1 = vec4::load_xmm(x->row3);
        yt0 = _mm_add_ps(_mm_mul_ps(yt0, vec4::load_xmm(y)), yt1);
        *(vec3*)&z->row3 = vec3::store_xmm(yt0);
    }
}

inline void mat4_translate_y_mult(const mat4* x, float_t y, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    if (x != z)
        *z = *x;
    if (y != 0.0f) {
        yt0 = vec4::load_xmm(x->row1);
        yt1 = vec4::load_xmm(x->row3);
        yt0 = _mm_add_ps(_mm_mul_ps(yt0, vec4::load_xmm(y)), yt1);
        *(vec3*)&z->row3 = vec3::store_xmm(yt0);
    }
}

inline void mat4_translate_z_mult(const mat4* x, float_t y, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    if (x != z)
        *z = *x;
    if (y != 0.0f) {
        yt0 = vec4::load_xmm(x->row2);
        yt1 = vec4::load_xmm(x->row3);
        yt0 = _mm_add_ps(_mm_mul_ps(yt0, vec4::load_xmm(y)), yt1);
        *(vec3*)&z->row3 = vec3::store_xmm(yt0);
    }
}

inline void mat4_translate_add(const mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    if (s != d)
        *d = *s;
    if (x != 0.0f || y != 0.0f || z != 0.0f)
        d->row3 = vec4::store_xmm(_mm_add_ps(vec4::load_xmm(s->row3), vec4::load_xmm(vec4(x, y, z, 0.0f))));
}

inline void mat4_translate_x_add(const mat4* x, float_t y, mat4* z) {
    if (x != z)
        *z = *x;
    if (y != 0.0f)
        z->row3.x += y;
}

inline void mat4_translate_y_add(const mat4* x, float_t y, mat4* z) {
    if (x != z)
        *z = *x;
    if (y != 0.0f)
        z->row3.y += y;
}

inline void mat4_translate_z_add(const mat4* x, float_t y, mat4* z) {
    if (x != z)
        *z = *x;
    if (y != 0.0f)
        z->row3.z += y;
}

inline void mat4_from_quat(const quat* quat, mat4* mat) {
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
    len = quat::length_squared(*quat);
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

float_t vec3_angle_between_two_vectors(const vec3* x, const vec3* y) {
    vec3 z_t = vec3::cross(*x, *y);
    float_t v2 = vec3::length(z_t);
    float_t v3 = vec3::dot(*x, *y);
    return fabsf(atan2f(v2, v3));
}

void mat4_from_two_vectors(const vec3* x, const vec3* y, mat4* mat) {
    *mat = mat4_identity;
    if (x->x == y->x && y->y == x->y && y->z == x->z)
        return;

    if (fabsf(1.0f - vec3::dot(*x, *y)) <= 0.000001f)
        return;

    vec3 axis = vec3::cross(*x, *y);
    float_t axis_length = vec3::length(axis);
    if (axis_length > 0.000001f) {
        float_t angle = vec3_angle_between_two_vectors(x, y);
        if (axis_length != 0.0)
            axis *= 1.0f / axis_length;
        mat4_from_axis_angle(&axis, angle, mat);
    }
}

inline void mat4_from_axis_angle(const vec3* axis, float_t angle, mat4* mat) {
    float_t angle_sin;
    float_t angle_cos;
    float_t angle_cos_1;
    vec3 _axis;
    vec3 _axis_sin;
    vec3 temp;

    angle_sin = sinf(angle);
    angle_cos = cosf(angle);
    angle_cos_1 = 1.0f - angle_cos;

    _axis = vec3::normalize(*axis);

    _axis_sin = _axis * angle_sin;
    temp = _axis * (_axis.x * angle_cos_1);
    mat->row0.x = temp.x + angle_cos;
    mat->row1.x = temp.y - _axis_sin.z;
    mat->row2.x = temp.z + _axis_sin.y;
    temp = _axis * (_axis.y * angle_cos_1);
    mat->row0.y = temp.x + _axis_sin.z;
    mat->row1.y = temp.y + angle_cos;
    mat->row2.y = temp.z - _axis_sin.x;
    temp = _axis * (_axis.z * angle_cos_1);
    mat->row0.z = temp.x - _axis_sin.y;
    mat->row1.z = temp.y + _axis_sin.x;
    mat->row2.z = temp.z + angle_cos;
    mat->row0.w = 0.0f;
    mat->row1.w = 0.0f;
    mat->row2.w = 0.0f;
    mat->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

inline void mat4_from_mat3(const mat3* x, mat4* z) {
    *(vec3*)&z->row0 = x->row0;
    z->row0.w = 0.0f;
    *(vec3*)&z->row1 = x->row1;
    z->row1.w = 0.0f;
    *(vec3*)&z->row2 = x->row2;
    z->row2.w = 0.0f;
    z->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

inline void mat4_from_mat3_inverse(const mat3* x, mat4* z) {
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

inline void mat4_get_rotation(const mat4* x, vec3* z) {
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

inline void mat4_get_scale(const mat4* x, vec3* z) {
    z->x = vec4::length(x->row0);
    z->y = vec4::length(x->row1);
    z->z = vec4::length(x->row2);
}

inline void mat4_get_translation(const mat4* x, vec3* z) {
    *z = *(vec3*)&x->row3;
}

inline void mat4_set_translation(mat4* x, vec3* z) {
    *(vec3*)&x->row3 = *z;
}

inline void mat4_blend(const mat4* x, const mat4* y, mat4* z, float_t blend) {
    quat q0;
    quat q1;
    quat q2;

    quat_from_mat3(x->row0.x, x->row1.x, x->row2.x, x->row0.y,
        x->row1.y, x->row2.y, x->row0.z, x->row1.z, x->row2.z, &q0);
    quat_from_mat3(y->row0.x, y->row1.x, y->row2.x, y->row0.y,
        y->row1.y, y->row2.y, y->row0.z, y->row1.z, y->row2.z, &q1);

    vec3 t1;
    vec3 t2;
    vec3 t3;
    mat4_get_translation(x, &t1);
    mat4_get_translation(y, &t2);

    q2 = quat::slerp(q0, q1, blend);
    t3 = vec3::lerp(t1, t2, blend);

    mat4_from_quat(&q2, z);
    mat4_set_translation(z, &t3);
}

inline void mat4_blend_rotation(const mat4* x, const mat4* y, mat4* z, float_t blend) {
    quat q0;
    quat q1;
    quat q2;

    quat_from_mat3(x->row0.x, x->row1.x, x->row2.x, x->row0.y,
        x->row1.y, x->row2.y, x->row0.z, x->row1.z, x->row2.z, &q1);
    quat_from_mat3(y->row0.x, y->row1.x, y->row2.x, y->row0.y,
        y->row1.y, y->row2.y, y->row0.z, y->row1.z, y->row2.z, &q1);
    q2 = quat::slerp(q0, q1, blend);
    mat4_from_quat(&q2, z);
}

void mat4_lerp_rotation(const mat4* x, const mat4* y, mat4* z, float_t blend) {
    vec3 m0;
    vec3 m1;
    m0 = vec3::lerp(*(vec3*)&x->row0, *(vec3*)&y->row0, blend);
    m1 = vec3::lerp(*(vec3*)&x->row1, *(vec3*)&y->row1, blend);

    float_t m0_len_sq = vec3::length_squared(m0);
    float_t m1_len_sq = vec3::length_squared(m1);

    if (m0_len_sq <= 0.000001f || m1_len_sq <= 0.000001f) {
        *z = *y;
        return;
    }

    vec3 m2;
    m2 = vec3::cross(m0, m1);
    m1 = vec3::cross(m2, m0);

    float_t m2_len_sq;
    m1_len_sq = vec3::length_squared(m1);
    m2_len_sq = vec3::length_squared(m2);
    if (m2_len_sq <= 0.000001f || m1_len_sq <= 0.000001) {
        *z = *y;
        return;
    }

    float_t m0_len = sqrtf(m0_len_sq);
    if (m0_len != 0.0f)
        m0 *= 1.0f / m0_len;

    float_t m1_len = sqrtf(m1_len_sq);
    if (m1_len != 0.0f)
        m1 *= 1.0f / m1_len;

    float_t m2_len = sqrtf(m2_len_sq);
    if (m2_len != 0.0f)
        m2 *= 1.0f / m2_len;

    *z = mat4_identity;
    *(vec3*)&z->row0 = m0;
    *(vec3*)&z->row1 = m1;
    *(vec3*)&z->row2 = m2;
}

inline float_t mat4_get_max_scale(const mat4* x) {
    mat4 mat;
    mat4_transpose(x, &mat);

    float_t length;
    float_t max = 0.0f;
    length = vec3::length(*(vec3*)&mat.row0);
    if (max < length)
        max = length;
    length = vec3::length(*(vec3*)&mat.row1);
    if (max < length)
        max = length;
    length = vec3::length(*(vec3*)&mat.row2);
    if (max < length)
        max = length;
    return max;
}

inline void mat4_mult_axis_angle(const mat4* x, const vec3* axis, const float_t angle, mat4* z) {
    quat q1;
    quat q2;
    quat q3;
    float_t t0;
    float_t t1;
    float_t t2;
    vec4 t3;

    t0 = x->row0.w;
    t1 = x->row1.w;
    t2 = x->row2.w;
    t3 = x->row3;
    quat_from_mat3(x->row0.x, x->row1.x, x->row2.x, x->row0.y,
        x->row1.y, x->row2.y, x->row0.z, x->row1.z, x->row2.z, &q1);
    quat_from_axis_angle(axis, angle, &q2);
    quat_mult(&q2, &q1, &q3);
    mat4_from_quat(&q3, z);
    z->row0.w = t0;
    z->row1.w = t1;
    z->row2.w = t2;
    z->row3 = t3;
}

inline void mat4_frustrum(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* mat) {
    *mat = mat4_null;
    mat->row0.x = (float_t)((2.0 * z_near) / (right - left));
    mat->row1.y = (float_t)((2.0 * z_near) / (top - bottom));
    mat->row2.x = (float_t)((right + left) / (right - left));
    mat->row2.y = (float_t)((top + bottom) / (top - bottom));
    mat->row2.z = -(float_t)((z_far + z_near) / (z_far - z_near));
    mat->row2.w = -1.0f;
    mat->row3.z = -(float_t)((2.0 * z_far * z_near) / (z_far - z_near));
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

    *mat = mat4_null;
    mat->row0.x = (float_t)(1.0 / (aspect * tan_fov));
    mat->row1.y = (float_t)(1.0 / tan_fov);
    mat->row2.z = -(float_t)((z_far + z_near) / (z_far - z_near));
    mat->row2.w = -1.0f;
    mat->row3.z = -(float_t)((2.0 * z_far * z_near) / (z_far - z_near));
}

inline void mat4_look_at(const vec3* eye, const vec3* target, const vec3* up, mat4* mat) {
    vec3 x_axis, y_axis, z_axis;
    vec3 xyz;

    z_axis = vec3::normalize(*eye - *target);

    x_axis = vec3::normalize(vec3::cross(*up, z_axis));
    if (vec3::length(x_axis) == 0.0f)
        x_axis = { 1.0f, 0.0f, 0.0f };

    y_axis = vec3::normalize(vec3::cross(z_axis, x_axis));

    xyz.x = vec3::dot(x_axis, *eye);
    xyz.y = vec3::dot(y_axis, *eye);
    xyz.z = vec3::dot(z_axis, *eye);

    mat->row0 = { x_axis.x, y_axis.x, z_axis.x, 0.0f };
    mat->row1 = { x_axis.y, y_axis.y, z_axis.y, 0.0f };
    mat->row2 = { x_axis.z, y_axis.z, z_axis.z, 0.0f };
    *(vec3*)&mat->row3 = -xyz;
    mat->row3.w = 1.0f;
}

inline void mat4_look_at(const vec3* eye, const vec3* target, mat4* mat) {
    vec3 up = { 0.0f, 1.0f, 0.0f };
    vec3 dir;
    dir = *target - *eye;
    if (vec3::length_squared(dir) <= 0.000001f) {
        up.x = 0.0f;
        up.y = 0.0f;
        if (dir.z < 0.0f)
            up.z = 1.0f;
        else
            up.z = -1.0f;
    }

    mat4_look_at(eye, target, &up, mat);
}
