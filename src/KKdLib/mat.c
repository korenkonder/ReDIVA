/*
    by korenkonder
    GitHub/GitLab: korenkonder
    Matrix Inverse algo: https://github.com/niswegmann/small-matrix-inverse
*/

#include "mat.h"

void FASTCALL mat3_identity(mat3* x) {
    *x = (mat3){
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
    };
}

void FASTCALL mat3_null(mat3* x) {
    *x = (mat3){
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
    };
}

void FASTCALL mat3_add(mat3* x, mat3* y, mat3* z) {
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

void FASTCALL mat3_sub(mat3* x, mat3* y, mat3* z) {
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

void FASTCALL mat3_mult(mat3* x, mat3* y, mat3* z) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 xt;
    __m128 zt;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    *(vec3*)&yt0 = y->row0;
    *(vec3*)&yt1 = y->row1;
    *(vec3*)&yt2 = y->row2;
    *(vec3*)&xt = x->row0;
    t0 = _mm_mul_ps(yt0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(yt1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(yt2, _mm_shuffle_ps(xt, xt, 0b10101010));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), t2);
    z->row0 = *(vec3*)&zt;
    *(vec3*)&xt = x->row1;
    t0 = _mm_mul_ps(yt0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(yt1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(yt2, _mm_shuffle_ps(xt, xt, 0b10101010));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), t2);
    z->row1 = *(vec3*)&zt;
    *(vec3*)&xt = x->row2;
    t0 = _mm_mul_ps(yt0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(yt1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(yt2, _mm_shuffle_ps(xt, xt, 0b10101010));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), t2);
    z->row2 = *(vec3*)&zt;
}

void FASTCALL mat3_mult_vec(mat3* x, vec3* y, vec3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    *(vec3*)&xt = x->row0;
    *(vec3*)&yt = *y;
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0b00000000));
    *(vec3*)&xt = x->row1;
    zt = _mm_add_ps(zt, _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0b01010101)));
    *(vec3*)&xt = x->row2;
    zt = _mm_add_ps(zt, _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0b10101010)));
    *z = *(vec3*)&zt;
}

void FASTCALL mat3_mult_scalar(mat3* x, float_t y, mat3* z) {
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

void FASTCALL mat3_transpose(mat3* x, mat3* z) {
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
    xt3 = (__m128){ .m128_f32 = { 0.0f, 0.0f, 0.0f, 0.0f } };
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

void FASTCALL mat3_inverse(mat3* x, mat3* z) {
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
    wt = _mm_mul_ps(yt, wt);
    wt.m128_f32[3] = 0.0f;
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

void FASTCALL mat3_invtrans(mat3* x, mat3* z) {
    mat3 t = *x;
    mat3_inverse(&t, &t);
    mat3_transpose(&t, &t);
    *z = t;
}

void FASTCALL mat3_normalize(mat3* x, mat3* z) {
    __m128 det;
    __m128 xt;
    det = _mm_set_ss(mat3_determinant(x));
    if (det.m128_f32[0] != 0.0f)
        det.m128_f32[0] = 1.0f / det.m128_f32[0];
    det = _mm_shuffle_ps(det, det, 0);
    *(vec3*)&xt = x->row0;
    xt = _mm_mul_ps(xt, det);
    z->row0 = *(vec3*)&xt;
    *(vec3*)&xt = x->row1;
    xt = _mm_mul_ps(xt, det);
    z->row1 = *(vec3*)&xt;
    *(vec3*)&xt = x->row2;
    xt = _mm_mul_ps(xt, det);
    z->row2 = *(vec3*)&xt;
}

void FASTCALL mat3_normalize_rotation(mat3* x, mat3* z) {
    __m128 xt;
    __m128 yt;
    *(vec3*)&xt = x->row0;
    yt = _mm_mul_ps(xt, xt);
    yt.m128_f32[3] = 0.0f;
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    z->row0 = *(vec3*)&xt;
    *(vec3*)&xt = x->row1;
    yt = _mm_mul_ps(xt, xt);
    yt.m128_f32[3] = 0.0f;
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    z->row1 = *(vec3*)&xt;
    *(vec3*)&xt = x->row2;
    yt = _mm_mul_ps(xt, xt);
    yt.m128_f32[3] = 0.0f;
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    z->row2 = *(vec3*)&xt;
}

float_t FASTCALL mat3_determinant(mat3* x) {
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

void FASTCALL mat3_rotate(float_t x, float_t y, float_t z, mat3* d) {
    mat3 dt;
    mat3_identity(&dt);
    if (x != 0.0f)
        mat3_rotate_x_mult(&dt, x, &dt);
    if (y != 0.0f)
        mat3_rotate_y_mult(&dt, y, &dt);
    if (z != 0.0f)
        mat3_rotate_z_mult(&dt, z, &dt);
    *d = dt;
}

void FASTCALL mat3_rotate_x(float_t x, mat3* y) {
    mat3 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    mat3_identity(&yt);
    yt.row1.y = x_cos;
    yt.row1.z = x_sin;
    yt.row2.y = -x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

void FASTCALL mat3_rotate_y(float_t x, mat3* y) {
    mat3 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    mat3_identity(&yt);
    yt.row0.x = x_cos;
    yt.row0.z = -x_sin;
    yt.row2.x = x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

void FASTCALL mat3_rotate_z(float_t x, mat3* y) {
    mat3 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    mat3_identity(&yt);
    yt.row0.x = x_cos;
    yt.row0.y = x_sin;
    yt.row1.x = -x_sin;
    yt.row1.y = x_cos;
    *y = yt;
}

void FASTCALL mat3_rotate_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d) {
    mat3 dt;
    dt = *s;
    if (x != 0.0f)
        mat3_rotate_x_mult(&dt, x, &dt);
    if (y != 0.0f)
        mat3_rotate_y_mult(&dt, y, &dt);
    if (z != 0.0f)
        mat3_rotate_z_mult(&dt, z, &dt);
    *d = dt;
}

void FASTCALL mat3_rotate_x_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat3_identity(&yt);
    yt.row1.y = y_cos;
    yt.row1.z = y_sin;
    yt.row2.y = -y_sin;
    yt.row2.z = y_cos;
    mat3_mult(x, &yt, z);
}

void FASTCALL mat3_rotate_y_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat3_identity(&yt);
    yt.row0.x = y_cos;
    yt.row0.z = -y_sin;
    yt.row2.x = y_sin;
    yt.row2.z = y_cos;
    mat3_mult(x, &yt, z);
}

void FASTCALL mat3_rotate_z_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat3_identity(&yt);
    yt.row0.x = y_cos;
    yt.row0.y = y_sin;
    yt.row1.x = -y_sin;
    yt.row1.y = y_cos;
    mat3_mult(x, &yt, z);
}

void FASTCALL mat3_scale(float_t x, float_t y, float_t z, mat3* d) {
    mat3 dt;
    mat3_identity(&dt);
    if (x != 1.0f)
        dt.row0.x = x;
    if (y != 1.0f)
        dt.row1.y = y;
    if (z != 1.0f)
        dt.row2.z = z;
    *d = dt;
}

void FASTCALL mat3_scale_x(float_t x, mat3* y) {
    mat3 yt;
    mat3_identity(&yt);
    yt.row0.x = x;
    *y = yt;
}

void FASTCALL mat3_scale_y(float_t x, mat3* y) {
    mat3 yt;
    mat3_identity(&yt);
    yt.row1.y = x;
    *y = yt;
}

void FASTCALL mat3_scale_z(float_t x, mat3* y) {
    mat3 yt;
    mat3_identity(&yt);
    yt.row2.z = x;
    *y = yt;
}

void FASTCALL mat3_scale_mult(mat3* s, float_t x, float_t y, float_t z, mat3* d) {
    mat3 st;
    mat3 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f) {
        st = *s;
        mat3_identity(&dt);
        dt.row0.x = x;
        dt.row1.y = y;
        dt.row2.z = z;
        mat3_mult(&st, &dt, &dt);
        *d = dt;
    }
    else
        *d = *s;
}

void FASTCALL mat3_scale_x_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    mat3_identity(&yt);
    yt.row0.x = y;
    mat3_mult(x, &yt, z);
}

void FASTCALL mat3_scale_y_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    mat3_identity(&yt);
    yt.row1.y = y;
    mat3_mult(x, &yt, z);
}

void FASTCALL mat3_scale_z_mult(mat3* x, float_t y, mat3* z) {
    mat3 yt;
    mat3_identity(&yt);
    yt.row2.z = y;
    mat3_mult(x, &yt, z);
}

void FASTCALL mat3_from_quat(quat* quat, mat3* mat) {
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
    len = quat_length_squared(quat);;
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

void FASTCALL mat4_identity(mat4* x) {
    *x = (mat4){
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    };
}

void FASTCALL mat4_null(mat4* x) {
    *x = (mat4){
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
    };
}

void FASTCALL mat4_add(mat4* x, mat4* y, mat4* z) {
    *(__m128*)& z->row0 = _mm_add_ps(*(__m128*) & x->row0, *(__m128*) & y->row0);
    *(__m128*)& z->row1 = _mm_add_ps(*(__m128*) & x->row1, *(__m128*) & y->row1);
    *(__m128*)& z->row2 = _mm_add_ps(*(__m128*) & x->row2, *(__m128*) & y->row2);
    *(__m128*)& z->row3 = _mm_add_ps(*(__m128*) & x->row3, *(__m128*) & y->row3);
}

void FASTCALL mat4_sub(mat4* x, mat4* y, mat4* z) {
    *(__m128*)& z->row0 = _mm_sub_ps(*(__m128*) & x->row0, *(__m128*) & y->row0);
    *(__m128*)& z->row1 = _mm_sub_ps(*(__m128*) & x->row1, *(__m128*) & y->row1);
    *(__m128*)& z->row2 = _mm_sub_ps(*(__m128*) & x->row2, *(__m128*) & y->row2);
    *(__m128*)& z->row3 = _mm_sub_ps(*(__m128*) & x->row3, *(__m128*) & y->row3);
}

void FASTCALL mat4_mult(mat4* x, mat4* y, mat4* z) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 t3;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    __m128 zt;
    *(vec4*)&y0 = y->row0;
    *(vec4*)&y1 = y->row1;
    *(vec4*)&y2 = y->row2;
    *(vec4*)&y3 = y->row3;
    *(vec4*)&xt = x->row0;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0b10101010));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0b11111111));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    z->row0 = *(vec4*)&zt;
    *(vec4*)&xt = x->row1;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0b10101010));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0b11111111));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    z->row1 = *(vec4*)&zt;
    *(vec4*)&xt = x->row2;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0b10101010));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0b11111111));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    z->row2 = *(vec4*)&zt;
    *(vec4*)&xt = x->row3;
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0b00000000));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0b01010101));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0b10101010));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0b11111111));
    zt = _mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3));
    z->row3 = *(vec4*)&zt;
}

void FASTCALL mat4_mult_vec3(mat4* x, vec3* y, vec3* z) {
    __m128 xt;
    __m128 yt;
    __m128 zt;
    *(vec3*)&xt = *(vec3*)&x->row0;
    *(vec3*)&yt = *y;
    zt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0b00000000));
    *(vec3*)&xt = *(vec3*)&x->row1;
    zt = _mm_add_ps(zt, _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0b01010101)));
    *(vec3*)&xt = *(vec3*)&x->row2;
    zt = _mm_add_ps(zt, _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0b10101010)));
    *z = *(vec3*)&zt;
}

void FASTCALL mat4_mult_vec(mat4* x, vec4* y, vec4* z) {
    __m128 yt;
    __m128 zt;
    *(vec4*)&yt = *y;
    zt = _mm_mul_ps(*(__m128*) & x->row0, _mm_shuffle_ps(yt, yt, 0b00000000));
    zt = _mm_add_ps(zt, _mm_mul_ps(*(__m128*) & x->row1, _mm_shuffle_ps(yt, yt, 0b01010101)));
    zt = _mm_add_ps(zt, _mm_mul_ps(*(__m128*) & x->row2, _mm_shuffle_ps(yt, yt, 0b10101010)));
    zt = _mm_add_ps(zt, _mm_mul_ps(*(__m128*) & x->row3, _mm_shuffle_ps(yt, yt, 0b11111111)));
    *z = *(vec4*)&zt;
}

void FASTCALL mat4_mult_scalar(mat4* x, float_t y, mat4* z) {
    __m128 yt;
    yt = _mm_set_ss(y);
    yt = _mm_shuffle_ps(yt, yt, 0);
    *(__m128*)& z->row0 = _mm_mul_ps(*(__m128*) & x->row0, yt);
    *(__m128*)& z->row1 = _mm_mul_ps(*(__m128*) & x->row1, yt);
    *(__m128*)& z->row2 = _mm_mul_ps(*(__m128*) & x->row2, yt);
    *(__m128*)& z->row3 = _mm_mul_ps(*(__m128*) & x->row3, yt);
}

void FASTCALL mat4_transpose(mat4* x, mat4* z) {
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    yt0 = _mm_unpacklo_ps(*(__m128*) & x->row0, *(__m128*) & x->row1);
    yt1 = _mm_unpackhi_ps(*(__m128*) & x->row0, *(__m128*) & x->row1);
    yt2 = _mm_unpacklo_ps(*(__m128*) & x->row2, *(__m128*) & x->row3);
    yt3 = _mm_unpackhi_ps(*(__m128*) & x->row2, *(__m128*) & x->row3);
    *(__m128*)& z->row0 = _mm_movelh_ps(yt0, yt2);
    *(__m128*)& z->row1 = _mm_movehl_ps(yt2, yt0);
    *(__m128*)& z->row2 = _mm_movelh_ps(yt1, yt3);
    *(__m128*)& z->row3 = _mm_movehl_ps(yt3, yt1);
}

void FASTCALL mat4_inverse(mat4* x, mat4* z) {
    static const __m128 xor0 = { .m128_u32 = { 0x80000000, 0x00000000, 0x80000000, 0x00000000 } };
    static const __m128 xor1 = { .m128_u32 = { 0x00000000, 0x80000000, 0x00000000, 0x80000000 } };

    vec4 xt0, xt1, xt2, xt3;
    __m128 yt00, yt01, yt02, yt03, yt04, yt05, yt06, yt07, yt08, yt09, yt10, yt11;
    __m128 zt0, zt1, zt2, zt3;
    __m128 wt;
    __m128 wt0, wt1, wt2;
    xt0 = x->row0;
    xt1 = x->row1;
    xt2 = x->row2;
    xt3 = x->row3;
    yt00 = _mm_set_ps(xt0.y, xt0.y, xt0.y, xt1.y);
    yt01 = _mm_set_ps(xt1.y, xt1.y, xt2.y, xt2.y);
    yt02 = _mm_set_ps(xt2.y, xt3.y, xt3.y, xt3.y);
    yt03 = _mm_set_ps(xt1.z, xt1.z, xt2.z, xt2.z);
    yt04 = _mm_set_ps(xt2.w, xt3.w, xt3.w, xt3.w);
    yt05 = _mm_set_ps(xt1.w, xt1.w, xt2.w, xt2.w);
    yt06 = _mm_set_ps(xt2.z, xt3.z, xt3.z, xt3.z);
    yt07 = _mm_set_ps(xt0.z, xt0.z, xt0.z, xt1.z);
    yt08 = _mm_set_ps(xt0.w, xt0.w, xt0.w, xt1.w);
    yt09 = _mm_set_ps(xt0.x, xt0.x, xt0.x, xt1.x);
    yt10 = _mm_set_ps(xt1.x, xt1.x, xt2.x, xt2.x);
    yt11 = _mm_set_ps(xt2.x, xt3.x, xt3.x, xt3.x);
    wt0 = _mm_mul_ps(_mm_xor_ps(yt00, xor1), _mm_sub_ps(_mm_mul_ps(yt03, yt04), _mm_mul_ps(yt05, yt06)));
    wt1 = _mm_mul_ps(_mm_xor_ps(yt01, xor0), _mm_sub_ps(_mm_mul_ps(yt07, yt04), _mm_mul_ps(yt08, yt06)));
    wt2 = _mm_mul_ps(_mm_xor_ps(yt02, xor1), _mm_sub_ps(_mm_mul_ps(yt07, yt05), _mm_mul_ps(yt08, yt03)));
    zt0 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt0 = _mm_mul_ps(_mm_xor_ps(yt09, xor0), _mm_sub_ps(_mm_mul_ps(yt03, yt04), _mm_mul_ps(yt05, yt06)));
    wt1 = _mm_mul_ps(_mm_xor_ps(yt10, xor1), _mm_sub_ps(_mm_mul_ps(yt07, yt04), _mm_mul_ps(yt08, yt06)));
    wt2 = _mm_mul_ps(_mm_xor_ps(yt11, xor0), _mm_sub_ps(_mm_mul_ps(yt07, yt05), _mm_mul_ps(yt08, yt03)));
    zt1 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt0 = _mm_mul_ps(_mm_xor_ps(yt09, xor1), _mm_sub_ps(_mm_mul_ps(yt01, yt04), _mm_mul_ps(yt05, yt02)));
    wt1 = _mm_mul_ps(_mm_xor_ps(yt10, xor0), _mm_sub_ps(_mm_mul_ps(yt00, yt04), _mm_mul_ps(yt08, yt02)));
    wt2 = _mm_mul_ps(_mm_xor_ps(yt11, xor1), _mm_sub_ps(_mm_mul_ps(yt00, yt05), _mm_mul_ps(yt08, yt01)));
    zt2 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt0 = _mm_mul_ps(_mm_xor_ps(yt09, xor0), _mm_sub_ps(_mm_mul_ps(yt01, yt06), _mm_mul_ps(yt03, yt02)));
    wt1 = _mm_mul_ps(_mm_xor_ps(yt10, xor1), _mm_sub_ps(_mm_mul_ps(yt00, yt06), _mm_mul_ps(yt07, yt02)));
    wt2 = _mm_mul_ps(_mm_xor_ps(yt11, xor0), _mm_sub_ps(_mm_mul_ps(yt00, yt03), _mm_mul_ps(yt07, yt01)));
    zt3 = _mm_add_ps(_mm_add_ps(wt0, wt1), wt2);

    wt = _mm_movelh_ps(_mm_unpacklo_ps(zt0, zt1), _mm_unpacklo_ps(zt2, zt3));
    wt = _mm_mul_ps(*(__m128*) & xt0, wt);
    wt = _mm_hadd_ps(wt, wt);
    wt = _mm_hadd_ps(wt, wt);
    if (wt.m128_f32[0] != 0.0f)
        wt.m128_f32[0] = 1.0f / wt.m128_f32[0];
    wt = _mm_shuffle_ps(wt, wt, 0);
    *(__m128*)& z->row0 = _mm_mul_ps(zt0, wt);
    *(__m128*)& z->row1 = _mm_mul_ps(zt1, wt);
    *(__m128*)& z->row2 = _mm_mul_ps(zt2, wt);
    *(__m128*)& z->row3 = _mm_mul_ps(zt3, wt);
}

void FASTCALL mat4_invtrans(mat4* x, mat4* z) {
    mat4 t = *x;
    mat4_inverse(&t, &t);
    mat4_transpose(&t, &t);
    *z = t;
}

void FASTCALL mat4_normalize(mat4* x, mat4* z) {
    __m128 det;
    det = _mm_set_ss(mat4_determinant(x));
    if (det.m128_f32[0] != 0.0f)
        det.m128_f32[0] = 1.0f / det.m128_f32[0];
    det = _mm_shuffle_ps(det, det, 0);
    *(__m128*)& z->row0 = _mm_mul_ps(*(__m128*) & x->row0, det);
    *(__m128*)& z->row1 = _mm_mul_ps(*(__m128*) & x->row1, det);
    *(__m128*)& z->row2 = _mm_mul_ps(*(__m128*) & x->row2, det);
    *(__m128*)& z->row3 = _mm_mul_ps(*(__m128*) & x->row3, det);
}

void FASTCALL mat4_normalize_rotation(mat4* x, mat4* z) {
    __m128 xt;
    __m128 yt;
    *(vec3*)&xt = *(vec3*)&x->row0;
    yt = _mm_mul_ps(xt, xt);
    yt.m128_f32[3] = 0.0f;
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    *(vec3*)&z->row0 = *(vec3*)&xt;
    *(vec3*)&xt = *(vec3*)&x->row1;
    yt = _mm_mul_ps(xt, xt);
    yt.m128_f32[3] = 0.0f;
    yt = _mm_hadd_ps(yt, yt);
    yt = _mm_sqrt_ss(_mm_hadd_ps(yt, yt));
    if (yt.m128_f32[0] != 0.0f)
        yt.m128_f32[0] = 1.0f / yt.m128_f32[0];
    xt = _mm_mul_ps(xt, _mm_shuffle_ps(yt, yt, 0));
    *(vec3*)&z->row1 = *(vec3*)&xt;
    *(vec3*)&xt = *(vec3*)&x->row2;
    yt = _mm_mul_ps(xt, xt);
    yt.m128_f32[3] = 0.0f;
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

float_t FASTCALL mat4_determinant(mat4* x) {
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

void FASTCALL mat4_rotate(float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    mat4_identity(&dt);
    if (x != 0.0f)
        mat4_rotate_x_mult(&dt, x, &dt);
    if (y != 0.0f)
        mat4_rotate_y_mult(&dt, y, &dt);
    if (z != 0.0f)
        mat4_rotate_z_mult(&dt, z, &dt);
    *d = dt;
}

void FASTCALL mat4_rotate_x(float_t x, mat4* y) {
    mat4 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    mat4_identity(&yt);
    yt.row1.y = x_cos;
    yt.row1.z = x_sin;
    yt.row2.y = -x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

void FASTCALL mat4_rotate_y(float_t x, mat4* y) {
    mat4 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    mat4_identity(&yt);
    yt.row0.x = x_cos;
    yt.row0.z = -x_sin;
    yt.row2.x = x_sin;
    yt.row2.z = x_cos;
    *y = yt;
}

void FASTCALL mat4_rotate_z(float_t x, mat4* y) {
    mat4 yt;
    float_t x_sin = sinf(x);
    float_t x_cos = cosf(x);
    mat4_identity(&yt);
    yt.row0.x = x_cos;
    yt.row0.y = x_sin;
    yt.row1.x = -x_sin;
    yt.row1.y = x_cos;
    *y = yt;
}

void FASTCALL mat4_rotate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = *s;
    if (x != 0.0f)
        mat4_rotate_x_mult(&dt, x, &dt);
    if (y != 0.0f)
        mat4_rotate_y_mult(&dt, y, &dt);
    if (z != 0.0f)
        mat4_rotate_z_mult(&dt, z, &dt);
    *d = dt;
}

void FASTCALL mat4_rotate_x_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat4_identity(&yt);
    yt.row1.y = y_cos;
    yt.row1.z = y_sin;
    yt.row2.y = -y_sin;
    yt.row2.z = y_cos;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_rotate_y_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat4_identity(&yt);
    yt.row0.x = y_cos;
    yt.row0.z = -y_sin;
    yt.row2.x = y_sin;
    yt.row2.z = y_cos;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_rotate_z_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat4_identity(&yt);
    yt.row0.x = y_cos;
    yt.row0.y = y_sin;
    yt.row1.x = -y_sin;
    yt.row1.y = y_cos;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    dt = *s;
    if (x != 0.0f)
        mat4_rot_x(&dt, x, &dt);
    if (y != 0.0f)
        mat4_rot_y(&dt, y, &dt);
    if (z != 0.0f)
        mat4_rot_z(&dt, z, &dt);
    *d = dt;
}

void FASTCALL mat4_rot_x(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat3_identity(&yt);
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

void FASTCALL mat4_rot_y(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat3_identity(&yt);
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

void FASTCALL mat4_rot_z(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    float_t y_sin = sinf(y);
    float_t y_cos = cosf(y);
    mat3_identity(&yt);
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

void FASTCALL mat4_scale(float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    mat4_identity(&dt);
    if (x != 1.0f)
        dt.row0.x = x;
    if (y != 1.0f)
        dt.row1.y = y;
    if (z != 1.0f)
        dt.row2.z = z;
    *d = dt;
}

void FASTCALL mat4_scale_x(float_t x, mat4* y) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row0.x = x;
    *y = yt;
}

void FASTCALL mat4_scale_y(float_t x, mat4* y) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row1.y = x;
    *y = yt;
}

void FASTCALL mat4_scale_z(float_t x, mat4* y) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row2.z = x;
    *y = yt;
}

void FASTCALL mat4_scale_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat4 st;
    mat4 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f) {
        st = *s;
        mat4_identity(&dt);
        dt.row0.x = x;
        dt.row1.y = y;
        dt.row2.z = z;
        mat4_mult(&st, &dt, &dt);
        *d = dt;
    }
    else
        *d = *s;
}

void FASTCALL mat4_scale_x_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row0.x = y;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_scale_y_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row1.y = y;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_scale_z_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row2.z = y;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_scale_rot(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat3 st;
    mat3 dt;
    if (x != 1.0f || y != 1.0f || z != 1.0f) {
        st.row0 = *(vec3*)&s->row0;
        st.row1 = *(vec3*)&s->row1;
        st.row2 = *(vec3*)&s->row2;
        mat3_identity(&dt);
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

void FASTCALL mat4_scale_x_rot(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    mat3_identity(&yt);
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

void FASTCALL mat4_scale_y_rot(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    mat3_identity(&yt);
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

void FASTCALL mat4_scale_z_rot(mat4* x, float_t y, mat4* z) {
    mat3 xt;
    mat3 yt;
    mat3 zt;
    xt.row0 = *(vec3*)&x->row0;
    xt.row1 = *(vec3*)&x->row1;
    xt.row2 = *(vec3*)&x->row2;
    mat3_identity(&yt);
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

void FASTCALL mat4_translate(float_t x, float_t y, float_t z, mat4* d) {
    mat4 dt;
    mat4_identity(&dt);
    dt.row3.x = x;
    dt.row3.y = y;
    dt.row3.z = z;
    *d = dt;
}

void FASTCALL mat4_translate_x(float_t x, mat4* y) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row3.x = x;
    *y = yt;
}

void FASTCALL mat4_translate_y(float_t x, mat4* y) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row3.y = x;
    *y = yt;
}

void FASTCALL mat4_translate_z(float_t x, mat4* y) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row3.z = x;
    *y = yt;
}

void FASTCALL mat4_translate_mult(mat4* s, float_t x, float_t y, float_t z, mat4* d) {
    mat4 st;
    mat4 dt;
    if (x != 0.0f || y != 0.0f || z != 0.0f) {
        st = *s;
        mat4_identity(&dt);
        dt.row3.x = x;
        dt.row3.y = y;
        dt.row3.z = z;
        mat4_mult(&dt, &st, &dt);
        *d = dt;
    }
    else
        *d = *s;
}

void FASTCALL mat4_translate_x_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row3.x = y;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_translate_y_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row3.y = y;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_translate_z_mult(mat4* x, float_t y, mat4* z) {
    mat4 yt;
    mat4_identity(&yt);
    yt.row3.z = y;
    mat4_mult(x, &yt, z);
}

void FASTCALL mat4_from_quat(quat* quat, mat4* mat) {
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
    len = quat_length_squared(quat);;
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
    mat->row3.x = 0.0f;
    mat->row3.y = 0.0f;
    mat->row3.z = 0.0f;
    mat->row3.w = 1.0f;
}
