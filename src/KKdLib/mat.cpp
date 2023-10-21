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

void mat3::set(const quat& in_q1) {
    float_t x = in_q1.x;
    float_t y = in_q1.y;
    float_t z = in_q1.z;
    float_t w = in_q1.w;
    float_t len = quat::length_squared(in_q1);
    len = len > 0.0f ? 2.0f / len : 0.0f;
    float_t xx = x * x * len;
    float_t xy = x * y * len;
    float_t xz = x * z * len;
    float_t yy = y * y * len;
    float_t zz = z * z * len;
    float_t yz = y * z * len;
    float_t wx = w * x * len;
    float_t wy = w * y * len;
    float_t wz = w * z * len;
    row0.x = 1.0f - zz - yy;
    row0.y = xy + wz;
    row1.z = xz - wy;
    row1.x = xy - wz;
    row1.y = 1.0f - zz - xx;
    row1.z = yz + wx;
    row2.x = xz + wy;
    row2.y = yz - wx;
    row2.z = 1.0f - yy - xx;
}

void mat3::set(const vec3& in_axis, const float_t in_angle) {
    set(in_axis, sinf(in_angle), cosf(in_angle));
}

void mat3::set(const vec3& in_axis, const float_t s, const float_t c) {
    float_t c_1 = 1.0f - c;

    vec3 axis = vec3::normalize(in_axis);
    vec3 axis_s = axis * s;

    vec3 temp;
    temp = axis * (axis.x * c_1);
    row0.x = temp.x + c;
    row1.x = temp.y - axis_s.z;
    row2.x = temp.z + axis_s.y;
    temp = axis * (axis.y * c_1);
    row0.y = temp.x + axis_s.z;
    row1.y = temp.y + c;
    row2.y = temp.z - axis_s.x;
    temp = axis * (axis.z * c_1);
    row0.z = temp.x - axis_s.y;
    row1.z = temp.y + axis_s.x;
    row2.z = temp.z + c;
}

void mat3::add(const float_t value) {
    row0 += value;
    row1 += value;
    row2 += value;
}

void mat3::add(const mat3& in_m1, const mat3& in_m2) {
    row0 = in_m1.row0 + in_m2.row0;
    row1 = in_m1.row1 + in_m2.row1;
    row2 = in_m1.row2 + in_m2.row2;
}

void mat3::sub(const float_t value) {
    row0 -= value;
    row1 -= value;
    row2 -= value;
}

void mat3::sub(const mat3& in_m1, const mat3& in_m2) {
    row0 = in_m1.row0 - in_m2.row0;
    row1 = in_m1.row1 - in_m2.row1;
    row2 = in_m1.row2 - in_m2.row2;
}

void mat3::mul(const float_t value) {
    row0 *= value;
    row1 *= value;
    row2 *= value;
}

void mat3::mul(const mat3& in_m1, const mat3& in_m2) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 yt;
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    xt0 = vec3::load_xmm(in_m1.row0);
    xt1 = vec3::load_xmm(in_m1.row1);
    xt2 = vec3::load_xmm(in_m1.row2);
    yt = vec3::load_xmm(in_m2.row0);
    t0 = _mm_mul_ps(xt0, _mm_shuffle_ps(yt, yt, 0x00));
    t1 = _mm_mul_ps(xt1, _mm_shuffle_ps(yt, yt, 0x55));
    t2 = _mm_mul_ps(xt2, _mm_shuffle_ps(yt, yt, 0xAA));
    row0 = vec3::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), t2));
    yt = vec3::load_xmm(in_m2.row1);
    t0 = _mm_mul_ps(xt0, _mm_shuffle_ps(yt, yt, 0x00));
    t1 = _mm_mul_ps(xt1, _mm_shuffle_ps(yt, yt, 0x55));
    t2 = _mm_mul_ps(xt2, _mm_shuffle_ps(yt, yt, 0xAA));
    row1 = vec3::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), t2));
    yt = vec3::load_xmm(in_m2.row2);
    t0 = _mm_mul_ps(xt0, _mm_shuffle_ps(yt, yt, 0x00));
    t1 = _mm_mul_ps(xt1, _mm_shuffle_ps(yt, yt, 0x55));
    t2 = _mm_mul_ps(xt2, _mm_shuffle_ps(yt, yt, 0xAA));
    row2 = vec3::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), t2));
}

void mat3::mul(const mat3& in_m1, const vec3& in_axis, const float_t in_angle) {
    quat q1 = quat(in_m1.row0.x, in_m1.row1.x, in_m1.row2.x, in_m1.row0.y,
        in_m1.row1.y, in_m1.row2.y, in_m1.row0.z, in_m1.row1.z, in_m1.row2.z);
    quat q2 = quat(in_axis, in_angle);
    set(quat::mul(q2, q1));
}

void mat3::transform_vector(const vec2& normal, vec2& normalOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    yt = vec2::load_xmm(normal);
    zt0 = _mm_mul_ps(vec3::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec3::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    normalOut = vec2::store_xmm(_mm_add_ps(zt0, zt1));
}

void mat3::transform_vector(const vec3& normal, vec3& normalOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    yt = vec3::load_xmm(normal);
    zt0 = _mm_mul_ps(vec3::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec3::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec3::load_xmm(row2), _mm_shuffle_ps(yt, yt, 0xAA));
    normalOut = vec3::store_xmm(_mm_add_ps(_mm_add_ps(zt0, zt1), zt2));
}

void mat3::inverse_transform_vector(const vec2& normal, vec2& normalOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec2::load_xmm(normal);
    zt = _mm_mul_ps(yt, vec3::load_xmm(row0));
    normalOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec3::load_xmm(row1));
    normalOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat3::inverse_transform_vector(const vec3& normal, vec3& normalOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec3::load_xmm(normal);
    zt = _mm_mul_ps(yt, vec3::load_xmm(row0));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec3::load_xmm(row1));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec3::load_xmm(row2));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat3::transpose(const mat3& in_m1) {
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    __m128 xt3;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    xt0 = vec3::load_xmm(in_m1.row0);
    xt1 = vec3::load_xmm(in_m1.row1);
    xt2 = vec3::load_xmm(in_m1.row2);
    xt3 = vec4::load_xmm(0.0f);
    yt0 = _mm_unpacklo_ps(xt0, xt1);
    yt1 = _mm_unpackhi_ps(xt0, xt1);
    yt2 = _mm_unpacklo_ps(xt2, xt3);
    yt3 = _mm_unpackhi_ps(xt2, xt3);
    row0 = vec3::store_xmm(_mm_movelh_ps(yt0, yt2));
    row1 = vec3::store_xmm(_mm_movehl_ps(yt2, yt0));
    row2 = vec3::store_xmm(_mm_movelh_ps(yt1, yt3));
}

void mat3::invert() {
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 wt;
    const vec3& xt0 = row0;
    const vec3& xt1 = row1;
    const vec3& xt2 = row2;
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
    wt = _mm_mul_ps(vec3::load_xmm(xt0), wt);
    wt = _mm_hadd_ps(wt, wt);
    wt = _mm_hadd_ps(wt, wt);
    if (_mm_cvtss_f32(wt) != 0.0f)
        wt = _mm_div_ss(_mm_set_ss(1.0f), wt);
    wt = _mm_shuffle_ps(wt, wt, 0);
    row0 = vec3::store_xmm(_mm_mul_ps(zt0, wt));
    row1 = vec3::store_xmm(_mm_mul_ps(zt1, wt));
    row2 = vec3::store_xmm(_mm_mul_ps(zt2, wt));
}

void mat3::invert_fast() {
    transpose();
}

void mat3::normalize() {
    float_t det = determinant();
    if (det != 0.0f)
        det = 1.0f / det;
    row0 *= det;
    row1 *= det;
    row2 *= det;
}

void mat3::normalize_rotation() {
    row0 = vec3::normalize(row0);
    row1 = vec3::normalize(row1);
    row2 = vec3::normalize(row2);
}

float_t mat3::determinant() const {
    const vec3& xt0 = row0;
    const vec3& xt1 = row1;
    const vec3& xt2 = row2;
    float_t b00 = xt0.x * xt1.y * xt2.z;
    float_t b01 = xt0.y * xt1.z * xt2.x;
    float_t b02 = xt0.z * xt1.x * xt2.y;
    float_t b03 = xt0.z * xt1.y * xt2.x;
    float_t b04 = xt0.x * xt1.z * xt2.y;
    float_t b05 = xt0.y * xt1.x * xt2.z;
    return b00 + b01 + b02 - b03 - b04 - b05;
}

void mat3::rotate_x(float_t rad) {
    rotate_x(sinf(rad), cosf(rad));
}

void mat3::rotate_y(float_t rad) {
    rotate_y(sinf(rad), cosf(rad));
}

void mat3::rotate_z(float_t rad) {
    rotate_z(sinf(rad), cosf(rad));
}

void mat3::rotate_x(float_t s, float_t c) {
    *this = mat3_identity;
    row1.y = c;
    row1.z = s;
    row2.y = -s;
    row2.z = c;
}

void mat3::rotate_y(float_t s, float_t c) {
    *this = mat3_identity;
    row0.x = c;
    row0.z = -s;
    row2.x = s;
    row2.z = c;
}

void mat3::rotate_z(float_t s, float_t c) {
    *this = mat3_identity;
    row0.x = c;
    row0.y = s;
    row1.x = -s;
    row1.y = c;
}

void mat3::rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat3_identity;
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat3::rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat3_identity;
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat3::rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat3_identity;
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat3::rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat3_identity;
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat3::rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat3_identity;
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat3::rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat3_identity;
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat3::mul_rotate_x(float_t rad) {
    mul_rotate_x(sinf(rad), cosf(rad));
}

void mat3::mul_rotate_y(float_t rad) {
    mul_rotate_y(sinf(rad), cosf(rad));
}

void mat3::mul_rotate_z(float_t rad) {
    mul_rotate_z(sinf(rad), cosf(rad));
}

void mat3::mul_rotate_x(float_t s, float_t c) {
    __m128 t1;
    __m128 t2;
    __m128 y1;
    __m128 y2;
    y1 = vec3::load_xmm(row1);
    y2 = vec3::load_xmm(row2);
    t1 = _mm_mul_ps(y1, vec4::load_xmm(c));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(s));
    row1 = vec3::store_xmm(_mm_add_ps(t1, t2));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(-s));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(c));
    row2 = vec3::store_xmm(_mm_add_ps(t1, t2));
}

void mat3::mul_rotate_y(float_t s, float_t c) {
    __m128 t0;
    __m128 t2;
    __m128 y0;
    __m128 y2;
    y0 = vec3::load_xmm(row0);
    y2 = vec3::load_xmm(row2);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(c));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(-s));
    row0 = vec3::store_xmm(_mm_add_ps(t0, t2));
    t0 = _mm_mul_ps(y0, vec4::load_xmm(s));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(c));
    row2 = vec3::store_xmm(_mm_add_ps(t0, t2));

}

void mat3::mul_rotate_z(float_t s, float_t c) {
    __m128 t0;
    __m128 t1;
    __m128 y0;
    __m128 y1;
    y0 = vec3::load_xmm(row0);
    y1 = vec3::load_xmm(row1);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(c));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(s));
    row0 = vec3::store_xmm(_mm_add_ps(t0, t1));
    t0 = _mm_mul_ps(y0, vec4::load_xmm(-s));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(c));
    row1 = vec3::store_xmm(_mm_add_ps(t0, t1));
}

void mat3::mul_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat3::mul_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat3::mul_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat3::mul_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat3::mul_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat3::mul_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat3::scale(float_t sx, float_t sy, float_t sz) {
    *this = mat3_identity;
    row0.x = sx;
    row1.y = sy;
    row2.z = sz;
}

void mat3::scale_x(float_t s) {
    *this = mat3_identity;
    row0.x = s;
}

void mat3::scale_y(float_t s) {
    *this = mat3_identity;
    row1.y = s;
}

void mat3::scale_z(float_t s) {
    *this = mat3_identity;
    row2.z = s;
}

void mat3::mul_scale(float_t sx, float_t sy, float_t sz) {
    row0 *= sx;
    row1 *= sy;
    row2 *= sz;
}

void mat3::mul_scale_x(float_t s) {
    row0 *= s;
}

void mat3::mul_scale_y(float_t s) {
    row1 *= s;
}

void mat3::mul_scale_z(float_t s) {
    row2 *= s;
}

void mat3::get_rotation(vec3& out_rad) const {
    if (-row0.z >= 1.0f)
        out_rad.y = (float_t)M_PI_2;
    else if (-row0.z <= -1.0f)
        out_rad.y = (float_t)-M_PI_2;
    else
        out_rad.y = asinf(-row0.z);

    if (fabsf(row0.z) < 0.99999899f) {
        out_rad.x = atan2f(row1.z, row2.z);
        out_rad.z = atan2f(row0.y, row0.x);
    }
    else {
        out_rad.x = 0.0f;
        out_rad.z = atan2f(row2.y, row1.y);
        if (row0.z > 0.0f)
            out_rad.z = -out_rad.z;
    }
}

void mat3::get_scale(vec3& out_s) const {
    out_s.x = vec3::length(row0);
    out_s.y = vec3::length(row1);
    out_s.z = vec3::length(row2);
}

float_t mat3::get_max_scale() const {
    mat3 mat;
    mat.transpose(*this);

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

void mat4::set(const quat& in_q1) {
    set_rotation(in_q1);
    row0.w = 0.0f;
    row1.w = 0.0f;
    row2.w = 0.0f;
    row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

static float_t vec3_angle_between_two_vectors(const vec3& in_v1, const vec3& in_v2) {
    vec3 z_t = vec3::cross(in_v1, in_v2);
    float_t v2 = vec3::length(z_t);
    float_t v3 = vec3::dot(in_v1, in_v2);
    return fabsf(atan2f(v2, v3));
}

void mat4::set(const vec3& in_v1, const vec3& in_v2) {
    *this = mat4_identity;
    if (in_v1 == in_v2 || fabsf(1.0f - vec3::dot(in_v1, in_v2)) <= 0.000001f)
        return;

    vec3 axis = vec3::cross(in_v1, in_v2);
    float_t axis_length = vec3::length(axis);
    if (axis_length > 0.000001f) {
        float_t angle = vec3_angle_between_two_vectors(in_v1, in_v2);
        if (axis_length != 0.0)
            axis *= 1.0f / axis_length;
        set(axis, angle);
    }
}

void mat4::set(const vec3& in_axis, const float_t in_angle) {
    set_rotation(in_axis, sinf(in_angle), cosf(in_angle));
    row0.w = 0.0f;
    row1.w = 0.0f;
    row2.w = 0.0f;
    row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void mat4::set(const vec3& in_axis, const float_t s, const float_t c) {
    set_rotation(in_axis, s, c);
    row0.w = 0.0f;
    row1.w = 0.0f;
    row2.w = 0.0f;
    row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void mat4::set_rotation(const quat& in_q1) {
    float_t x = in_q1.x;
    float_t y = in_q1.y;
    float_t z = in_q1.z;
    float_t w = in_q1.w;
    float_t len = quat::length_squared(in_q1);
    len = len > 0.0f ? 2.0f / len : 0.0f;
    float_t xx = x * x * len;
    float_t xy = x * y * len;
    float_t xz = x * z * len;
    float_t yy = y * y * len;
    float_t zz = z * z * len;
    float_t yz = y * z * len;
    float_t wx = w * x * len;
    float_t wy = w * y * len;
    float_t wz = w * z * len;
    row0.x = 1.0f - zz - yy;
    row0.y = xy + wz;
    row0.z = xz - wy;
    row1.x = xy - wz;
    row1.y = 1.0f - zz - xx;
    row1.z = yz + wx;
    row2.x = xz + wy;
    row2.y = yz - wx;
    row2.z = 1.0f - yy - xx;
}

void mat4::set_rotation(const vec3& in_axis, const float_t in_angle) {
    set_rotation(in_axis, sinf(in_angle), cosf(in_angle));
}

void mat4::set_rotation(const vec3& in_axis, const float_t s, const float_t c) {
    float_t c_1 = 1.0f - c;

    vec3 axis = vec3::normalize(in_axis);
    vec3 axis_s = axis * s;

    vec3 temp;
    temp = axis * (axis.x * c_1);
    row0.x = temp.x + c;
    row1.x = temp.y - axis_s.z;
    row2.x = temp.z + axis_s.y;
    temp = axis * (axis.y * c_1);
    row0.y = temp.x + axis_s.z;
    row1.y = temp.y + c;
    row2.y = temp.z - axis_s.x;
    temp = axis * (axis.z * c_1);
    row0.z = temp.x - axis_s.y;
    row1.z = temp.y + axis_s.x;
    row2.z = temp.z + c;
}

void mat4::add(const float_t value) {
    row0 += value;
    row1 += value;
    row2 += value;
    row3 += value;
}

void mat4::add(const mat4& in_m1, const mat4& in_m2) {
    row0 = in_m1.row0 + in_m2.row0;
    row1 = in_m1.row1 + in_m2.row1;
    row2 = in_m1.row2 + in_m2.row2;
    row3 = in_m1.row3 + in_m2.row3;
}

void mat4::sub(const float_t value) {
    row0 -= value;
    row1 -= value;
    row2 -= value;
    row3 -= value;
}

void mat4::sub(const mat4& in_m1, const mat4& in_m2) {
    row0 = in_m1.row0 - in_m2.row0;
    row1 = in_m1.row1 - in_m2.row1;
    row2 = in_m1.row2 - in_m2.row2;
    row3 = in_m1.row3 - in_m2.row3;
}

void mat4::mul(const float_t value) {
    row0 *= value;
    row1 *= value;
    row2 *= value;
    row3 *= value;
}

void mat4::mul(const mat4& in_m1, const mat4& in_m2) {
    __m128 t0;
    __m128 t1;
    __m128 t2;
    __m128 t3;
    __m128 xt;
    __m128 y0;
    __m128 y1;
    __m128 y2;
    __m128 y3;
    y0 = vec4::load_xmm(in_m2.row0);
    y1 = vec4::load_xmm(in_m2.row1);
    y2 = vec4::load_xmm(in_m2.row2);
    y3 = vec4::load_xmm(in_m2.row3);
    xt = vec4::load_xmm(in_m1.row0);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    row0 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
    xt = vec4::load_xmm(in_m1.row1);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    row1 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
    xt = vec4::load_xmm(in_m1.row2);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    row2 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
    xt = vec4::load_xmm(in_m1.row3);
    t0 = _mm_mul_ps(y0, _mm_shuffle_ps(xt, xt, 0x00));
    t1 = _mm_mul_ps(y1, _mm_shuffle_ps(xt, xt, 0x55));
    t2 = _mm_mul_ps(y2, _mm_shuffle_ps(xt, xt, 0xAA));
    t3 = _mm_mul_ps(y3, _mm_shuffle_ps(xt, xt, 0xFF));
    row3 = vec4::store_xmm(_mm_add_ps(_mm_add_ps(t0, t1), _mm_add_ps(t2, t3)));
}

void mat4::mul_rotation(const mat4& in_m1, const vec3& in_axis, const float_t in_angle) {
    quat q1 = quat(in_m1.row0.x, in_m1.row1.x, in_m1.row2.x, in_m1.row0.y,
        in_m1.row1.y, in_m1.row2.y, in_m1.row0.z, in_m1.row1.z, in_m1.row2.z);
    quat q2 = quat(in_axis, in_angle);
    set(quat::mul(q2, q1));
}

void mat4::transform_vector(const vec2& normal, vec2& normalOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    yt = vec2::load_xmm(normal);
    zt0 = _mm_mul_ps(vec4::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    normalOut = vec2::store_xmm(_mm_add_ps(zt0, zt1));
}

void mat4::transform_vector(const vec3& normal, vec3& normalOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    yt = vec3::load_xmm(normal);
    zt0 = _mm_mul_ps(vec4::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec4::load_xmm(row2), _mm_shuffle_ps(yt, yt, 0xAA));
    normalOut = vec3::store_xmm(_mm_add_ps(_mm_add_ps(zt0, zt1), zt2));
}

void mat4::transform_vector(const vec4& normal, vec4& normalOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;
    yt = vec4::load_xmm(normal);
    zt0 = _mm_mul_ps(vec4::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec4::load_xmm(row2), _mm_shuffle_ps(yt, yt, 0xAA));
    zt3 = _mm_mul_ps(vec4::load_xmm(row3), _mm_shuffle_ps(yt, yt, 0xFF));
    normalOut = vec4::store_xmm(_mm_add_ps(_mm_add_ps(zt0, zt1), _mm_add_ps(zt2, zt3)));
}

void mat4::transform_point(const vec2& point, vec2& pointOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    yt = vec2::load_xmm(point);
    zt0 = _mm_mul_ps(vec4::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = vec4::load_xmm(row3);
    pointOut = vec2::store_xmm(_mm_add_ps(_mm_add_ps(zt0, zt1), zt2));
}

void mat4::transform_point(const vec3& point, vec3& pointOut) const {
    __m128 yt;
    __m128 zt0;
    __m128 zt1;
    __m128 zt2;
    __m128 zt3;
    yt = vec3::load_xmm(point);
    zt0 = _mm_mul_ps(vec4::load_xmm(row0), _mm_shuffle_ps(yt, yt, 0x00));
    zt1 = _mm_mul_ps(vec4::load_xmm(row1), _mm_shuffle_ps(yt, yt, 0x55));
    zt2 = _mm_mul_ps(vec4::load_xmm(row2), _mm_shuffle_ps(yt, yt, 0xAA));
    zt3 = vec4::load_xmm(row3);
    pointOut = vec3::store_xmm(_mm_add_ps(_mm_add_ps(zt0, zt1), _mm_add_ps(zt2, zt3)));
}

void mat4::inverse_transform_vector(const vec2& normal, vec2& normalOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec2::load_xmm(normal);
    zt = _mm_mul_ps(yt, vec4::load_xmm(row0));
    normalOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row1));
    normalOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat4::inverse_transform_vector(const vec3& normal, vec3& normalOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec3::load_xmm(normal);
    zt = _mm_mul_ps(yt, vec4::load_xmm(row0));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row1));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row2));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat4::inverse_transform_vector(const vec4& normal, vec4& normalOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec4::load_xmm(normal);
    zt = _mm_mul_ps(yt, vec4::load_xmm(row0));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row1));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row2));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row3));
    zt = _mm_hadd_ps(zt, zt);
    normalOut.w = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat4::inverse_transform_point(const vec2& point, vec2& pointOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec2::load_xmm(point);
    yt = _mm_sub_ps(yt, vec4::load_xmm(row3));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row0));
    pointOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row1));
    pointOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat4::inverse_transform_point(const vec3& point, vec3& pointOut) const {
    __m128 yt;
    __m128 zt;
    yt = vec3::load_xmm(point);
    yt = _mm_sub_ps(yt, vec4::load_xmm(row3));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row0));
    zt = _mm_hadd_ps(zt, zt);
    pointOut.x = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row1));
    zt = _mm_hadd_ps(zt, zt);
    pointOut.y = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
    zt = _mm_mul_ps(yt, vec4::load_xmm(row2));
    zt = _mm_hadd_ps(zt, zt);
    pointOut.z = _mm_cvtss_f32(_mm_hadd_ps(zt, zt));
}

void mat4::transpose(const mat4& in_m1) {
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    __m128 xt3;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    xt0 = vec4::load_xmm(in_m1.row0);
    xt1 = vec4::load_xmm(in_m1.row1);
    xt2 = vec4::load_xmm(in_m1.row2);
    xt3 = vec4::load_xmm(in_m1.row3);
    yt0 = _mm_unpacklo_ps(xt0, xt1);
    yt1 = _mm_unpackhi_ps(xt0, xt1);
    yt2 = _mm_unpacklo_ps(xt2, xt3);
    yt3 = _mm_unpackhi_ps(xt2, xt3);
    row0 = vec4::store_xmm(_mm_movelh_ps(yt0, yt2));
    row1 = vec4::store_xmm(_mm_movehl_ps(yt2, yt0));
    row2 = vec4::store_xmm(_mm_movelh_ps(yt1, yt3));
    row3 = vec4::store_xmm(_mm_movehl_ps(yt3, yt1));
}

void mat4::invert() {
    static const __m128 xor0 = { -0.0f,  0.0f, -0.0f,  0.0f };
    static const __m128 xor1 = { 0.0f, -0.0f,  0.0f, -0.0f };

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
    xt0 = vec4::load_xmm(row0);
    xt1 = vec4::load_xmm(row1);
    xt2 = vec4::load_xmm(row2);
    xt3 = vec4::load_xmm(row3);
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
    row0 = vec4::store_xmm(_mm_mul_ps(zt0, wt));
    row1 = vec4::store_xmm(_mm_mul_ps(zt1, wt));
    row2 = vec4::store_xmm(_mm_mul_ps(zt2, wt));
    row3 = vec4::store_xmm(_mm_mul_ps(zt3, wt));
}

void mat4::invert_rotation() {
    mat3 mat;
    to_mat3(mat);
    mat.invert();
    rotation_from_mat3(mat);
}

void mat4::invert_fast() {
    inverse_transform_vector(*(vec3*)&row3, *(vec3*)&row3);
    invert_rotation_fast();
}

void mat4::invert_rotation_fast() {
    __m128 xt0;
    __m128 xt1;
    __m128 xt2;
    __m128 xt3;
    __m128 yt0;
    __m128 yt1;
    __m128 yt2;
    __m128 yt3;
    xt0 = vec3::load_xmm(*(vec3*)&row0);
    xt1 = vec3::load_xmm(*(vec3*)&row1);
    xt2 = vec3::load_xmm(*(vec3*)&row2);
    xt3 = vec4::load_xmm(0.0f);
    yt0 = _mm_unpacklo_ps(xt0, xt1);
    yt1 = _mm_unpackhi_ps(xt0, xt1);
    yt2 = _mm_unpacklo_ps(xt2, xt3);
    yt3 = _mm_unpackhi_ps(xt2, xt3);
    *(vec3*)&row0 = vec3::store_xmm(_mm_movelh_ps(yt0, yt2));
    *(vec3*)&row1 = vec3::store_xmm(_mm_movehl_ps(yt2, yt0));
    *(vec3*)&row2 = vec3::store_xmm(_mm_movelh_ps(yt1, yt3));
}

void mat4::normalize() {
    float_t det = determinant();
    if (det != 0.0f)
        det = 1.0f / det;
    row0 *= det;
    row1 *= det;
    row2 *= det;
    row3 *= det;
}

void mat4::normalize_rotation() {
    *(vec3*)&row0 = vec3::normalize(*(vec3*)&row0);
    *(vec3*)&row1 = vec3::normalize(*(vec3*)&row1);
    *(vec3*)&row2 = vec3::normalize(*(vec3*)&row2);
}

float_t mat4::determinant() const {
    const vec4& xt0 = row0;
    const vec4& xt1 = row1;
    const vec4& xt2 = row2;
    const vec4& xt3 = row3;
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

void mat4::rotate_x(float_t rad) {
    rotate_x(sinf(rad), cosf(rad));
}

void mat4::rotate_y(float_t rad) {
    rotate_y(sinf(rad), cosf(rad));
}

void mat4::rotate_z(float_t rad) {
    rotate_z(sinf(rad), cosf(rad));
}

void mat4::rotate_x(float_t s, float_t c) {
    *this = mat4_identity;
    row1.y = c;
    row1.z = s;
    row2.y = -s;
    row2.z = c;
}

void mat4::rotate_y(float_t s, float_t c) {
    *this = mat4_identity;
    row0.x = c;
    row0.z = -s;
    row2.x = s;
    row2.z = c;
}

void mat4::rotate_z(float_t s, float_t c) {
    *this = mat4_identity;
    row0.x = c;
    row0.y = s;
    row1.x = -s;
    row1.y = c;
}

void mat4::rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat4_identity;
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat4::rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat4_identity;
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat4::rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat4_identity;
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat4::rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat4_identity;
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat4::rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat4_identity;
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat4::rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z) {
    *this = mat4_identity;
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat4::mul_rotate_x(float_t rad) {
    mul_rotate_x(sinf(rad), cosf(rad));
}

void mat4::mul_rotate_y(float_t rad) {
    mul_rotate_y(sinf(rad), cosf(rad));
}

void mat4::mul_rotate_z(float_t rad) {
    mul_rotate_z(sinf(rad), cosf(rad));
}

void mat4::mul_rotate_x(float_t s, float_t c) {
    __m128 t1;
    __m128 t2;
    __m128 y1;
    __m128 y2;
    y1 = vec4::load_xmm(row1);
    y2 = vec4::load_xmm(row2);
    t1 = _mm_mul_ps(y1, vec4::load_xmm(c));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(s));
    row1 = vec4::store_xmm(_mm_add_ps(t1, t2));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(-s));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(c));
    row2 = vec4::store_xmm(_mm_add_ps(t1, t2));
}

void mat4::mul_rotate_y(float_t s, float_t c) {
    __m128 t0;
    __m128 t2;
    __m128 y0;
    __m128 y2;
    y0 = vec4::load_xmm(row0);
    y2 = vec4::load_xmm(row2);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(c));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(-s));
    row0 = vec4::store_xmm(_mm_add_ps(t0, t2));
    t0 = _mm_mul_ps(y0, vec4::load_xmm(s));
    t2 = _mm_mul_ps(y2, vec4::load_xmm(c));
    row2 = vec4::store_xmm(_mm_add_ps(t0, t2));
}

void mat4::mul_rotate_z(float_t s, float_t c) {
    __m128 t0;
    __m128 t1;
    __m128 y0;
    __m128 y1;
    y0 = vec4::load_xmm(row0);
    y1 = vec4::load_xmm(row1);
    t0 = _mm_mul_ps(y0, vec4::load_xmm(c));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(s));
    row0 = vec4::store_xmm(_mm_add_ps(t0, t1));
    t0 = _mm_mul_ps(y0, vec4::load_xmm(-s));
    t1 = _mm_mul_ps(y1, vec4::load_xmm(c));
    row1 = vec4::store_xmm(_mm_add_ps(t0, t1));
}

void mat4::mul_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat4::mul_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat4::mul_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_z(rad_z);
}

void mat4::mul_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_y != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat4::mul_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_x(rad_x);
    if (rad_x != 0.0f)
        mul_rotate_y(rad_y);
}

void mat4::mul_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z) {
    if (rad_z != 0.0f)
        mul_rotate_z(rad_z);
    if (rad_y != 0.0f)
        mul_rotate_y(rad_y);
    if (rad_x != 0.0f)
        mul_rotate_x(rad_x);
}

void mat4::scale(float_t sx, float_t sy, float_t sz) {
    *this = mat4_identity;
    row0.x = sx;
    row1.y = sy;
    row2.z = sz;
}

void mat4::scale(float_t sx, float_t sy, float_t sz, float_t sw) {
    *this = mat4_identity;
    row0.x = sx;
    row1.y = sy;
    row2.z = sz;
    row3.w = sw;
}

void mat4::scale_x(float_t s) {
    *this = mat4_identity;
    row0.x = s;
}

void mat4::scale_y(float_t s) {
    *this = mat4_identity;
    row1.y = s;
}

void mat4::scale_z(float_t s) {
    *this = mat4_identity;
    row2.z = s;
}

void mat4::scale_w(float_t s) {
    *this = mat4_identity;
    row3.w = s;
}

void mat4::mul_scale(float_t sx, float_t sy, float_t sz, float_t sw) {
    row0 *= sx;
    row1 *= sy;
    row2 *= sz;
    row3 *= sw;
}

void mat4::mul_scale_x(float_t s) {
    row0 *= s;
}

void mat4::mul_scale_y(float_t s) {
    row1 *= s;
}

void mat4::mul_scale_z(float_t s) {
    row2 *= s;
}

void mat4::mul_scale_w(float_t s) {
    row3 *= s;
}

void mat4::scale_rot(float_t sx, float_t sy, float_t sz) {
    if (sx != 1.0f || sy != 1.0f || sz != 1.0f) {
        *(vec3*)&row0 *= sx;
        *(vec3*)&row1 *= sy;
        *(vec3*)&row2 *= sz;
    }
}

void mat4::scale_x_rot(float_t s) {
    *(vec3*)&row0 *= s;
}

void mat4::scale_y_rot(float_t s) {
    *(vec3*)&row1 *= s;
}

void mat4::scale_z_rot(float_t s) {
    *(vec3*)&row2 *= s;
}

void mat4::translate(float_t tx, float_t ty, float_t tz) {
    *this = mat4_identity;
    row3.x = tx;
    row3.y = ty;
    row3.z = tz;
}

void mat4::translate_x(float_t t) {
    *this = mat4_identity;
    row3.x = t;
}

void mat4::translate_y(float_t t) {
    *this = mat4_identity;
    row3.y = t;
}

void mat4::translate_z(float_t t) {
    *this = mat4_identity;
    row3.z = t;
}

void mat4::mul_translate(float_t tx, float_t ty, float_t tz) {
    if (tx != 0.0f || ty != 0.0f || tz != 0.0f)
        *(vec3*)&row3 = vec3::store_xmm(vec4::load_xmm(row0 * tx + row1 * ty + row2 * tz + row3));
}

void mat4::mul_translate_x(float_t t) {
    if (t != 0.0f)
        *(vec3*)&row3 = vec3::store_xmm(vec4::load_xmm(row0 * t + row3));
}

void mat4::mul_translate_y(float_t t) {
    if (t != 0.0f)
        *(vec3*)&row3 = vec3::store_xmm(vec4::load_xmm(row1 * t + row3));
}

void mat4::mul_translate_z(float_t t) {
    if (t != 0.0f)
        *(vec3*)&row3 = vec3::store_xmm(vec4::load_xmm(row2 * t + row3));
}

void mat4::add_translate(float_t tx, float_t ty, float_t tz) {
    if (tx != 0.0f || ty != 0.0f || tz != 0.0f)
        row3 += vec4(tx, ty, tz, 0.0f);
}

void mat4::add_translate_x(float_t t) {
    if (t != 0.0f)
        row3.x += t;
}

void mat4::add_translate_y(float_t t) {
    if (t != 0.0f)
        row3.y += t;
}

void mat4::add_translate_z(float_t t) {
    if (t != 0.0f)
        row3.z += t;
}

void mat4::to_mat3(mat3& out_m) const {
    out_m.row0 = *(vec3*)&row0;
    out_m.row1 = *(vec3*)&row1;
    out_m.row2 = *(vec3*)&row2;
}

void mat4::to_mat3_inverse(mat3& out_m) const {
    mat4 mat;
    mat.invert(*this);
    out_m.row0 = *(vec3*)&mat.row0;
    out_m.row1 = *(vec3*)&mat.row1;
    out_m.row2 = *(vec3*)&mat.row2;
}

void mat4::from_mat3(const mat3& in_m1) {
    *(vec3*)&row0 = in_m1.row0;
    *(vec3*)&row1 = in_m1.row1;
    *(vec3*)&row2 = in_m1.row2;
}

void mat4::from_mat3_inverse(const mat3& in_m1) {
    mat3 mat;
    mat.invert(in_m1);
    *(vec3*)&row0 = mat.row0;
    *(vec3*)&row1 = mat.row1;
    *(vec3*)&row2 = mat.row2;
}

void mat4::rotation_from_mat3(const mat3& in_m1) {
    *this = mat4_identity;
    *(vec3*)&row0 = in_m1.row0;
    *(vec3*)&row1 = in_m1.row1;
    *(vec3*)&row2 = in_m1.row2;
}

void mat4::rotation_from_mat3_inverse(const mat3& in_m1) {
    mat3 mat;
    mat.invert(in_m1);
    *this = mat4_identity;
    *(vec3*)&row0 = mat.row0;
    *(vec3*)&row1 = mat.row1;
    *(vec3*)&row2 = mat.row2;
}

void mat4::clear_rot() {
    row0 = mat4_identity.row0;
    row1 = mat4_identity.row1;
    row2 = mat4_identity.row2;
}

void mat4::clear_trans() {
    row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void mat4::get_scale(vec3& out_s) const {
    out_s.x = vec4::length(row0);
    out_s.y = vec4::length(row1);
    out_s.z = vec4::length(row2);
}

void mat4::get_rotation(vec3& out_rad) const {
    if (-row0.z >= 1.0f)
        out_rad.y = (float_t)M_PI_2;
    else if (-row0.z <= -1.0f)
        out_rad.y = (float_t)-M_PI_2;
    else
        out_rad.y = asinf(-row0.z);

    if (fabsf(row0.z) < 0.99999899f) {
        out_rad.x = atan2f(row1.z, row2.z);
        out_rad.z = atan2f(row0.y, row0.x);
    }
    else {
        out_rad.x = 0.0f;
        out_rad.z = atan2f(row2.y, row1.y);
        if (row0.z > 0.0f)
            out_rad.z = -out_rad.z;
    }
}

void mat4::get_translation(vec3& out_t) const {
    out_t = *(vec3*)&row3;
}

float_t mat4::get_max_scale() const {
    mat4 mat;
    mat.transpose(*this);

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

void mat4::set_translation(const vec3& in_t) {
    *(vec3*)&row3 = in_t;
}

void mat4::blend(const mat4& in_m1, const mat4& in_m2, float_t blend) {
    quat q1 = quat(in_m1.row0.x, in_m1.row1.x, in_m1.row2.x, in_m1.row0.y,
        in_m1.row1.y, in_m1.row2.y, in_m1.row0.z, in_m1.row1.z, in_m1.row2.z);
    q1 = quat::normalize(q1);
    quat q2 = quat(in_m2.row0.x, in_m2.row1.x, in_m2.row2.x, in_m2.row0.y,
        in_m2.row1.y, in_m2.row2.y, in_m2.row0.z, in_m2.row1.z, in_m2.row2.z);
    q2 = quat::normalize(q2);

    vec3 t1;
    vec3 t2;
    in_m1.get_translation(t1);
    in_m2.get_translation(t2);

    quat q3 = quat::lerp(q1, q2, blend);
    vec3 t3 = vec3::lerp(t1, t2, blend);

    set(q3);
    set_translation(t3);
}

void mat4::blend_rotation(const mat4& in_m1, const mat4& in_m2, float_t blend) {
    quat q1 = quat(in_m1.row0.x, in_m1.row1.x, in_m1.row2.x, in_m1.row0.y,
        in_m1.row1.y, in_m1.row2.y, in_m1.row0.z, in_m1.row1.z, in_m1.row2.z);
    quat q2 = quat(in_m2.row0.x, in_m2.row1.x, in_m2.row2.x, in_m2.row0.y,
        in_m2.row1.y, in_m2.row2.y, in_m2.row0.z, in_m2.row1.z, in_m2.row2.z);
    set(quat::slerp(q1, q2, blend));
}

void mat4::lerp_rotation(const mat4& in_m1, const mat4& in_m2, float_t blend) {
    vec3 m0;
    vec3 m1;
    m0 = vec3::lerp(*(vec3*)&in_m1.row0, *(vec3*)&in_m2.row0, blend);
    m1 = vec3::lerp(*(vec3*)&in_m1.row1, *(vec3*)&in_m2.row1, blend);

    float_t m0_len_sq = vec3::length_squared(m0);
    float_t m1_len_sq = vec3::length_squared(m1);

    if (m0_len_sq <= 0.000001f || m1_len_sq <= 0.000001f) {
        *this = in_m2;
        return;
    }

    vec3 m2;
    m2 = vec3::cross(m0, m1);
    m1 = vec3::cross(m2, m0);

    float_t m2_len_sq;
    m1_len_sq = vec3::length_squared(m1);
    m2_len_sq = vec3::length_squared(m2);
    if (m2_len_sq <= 0.000001f || m1_len_sq <= 0.000001) {
        *this = in_m2;
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

    *this = mat4_identity;
    *(vec3*)&row0 = m0;
    *(vec3*)&row1 = m1;
    *(vec3*)&row2 = m2;
}

void mat4::frustrum(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far) {
    *this = mat4_null;
    row0.x = (float_t)((2.0 * z_near) / (right - left));
    row1.y = (float_t)((2.0 * z_near) / (top - bottom));
    row2.x = (float_t)((right + left) / (right - left));
    row2.y = (float_t)((top + bottom) / (top - bottom));
    row2.z = -(float_t)((z_far + z_near) / (z_far - z_near));
    row2.w = -1.0f;
    row3.z = -(float_t)((2.0 * z_far * z_near) / (z_far - z_near));
}

void mat4::ortho(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far) {
    *this = mat4_null;
    row0.x = (float_t)(2.0 / (right - left));
    row1.y = (float_t)(2.0 / (top - bottom));
    row2.z = (float_t)(-2.0 / (z_far - z_near));
    row3.x = -(float_t)((right + left) / (right - left));
    row3.y = -(float_t)((top + bottom) / (top - bottom));
    row3.z = -(float_t)((z_far + z_near) / (z_far - z_near));
    row3.w = 1.0f;
}

void mat4::persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far) {
    double_t tan_fov = tan(fov_y * 0.5);

    *this = mat4_null;
    row0.x = (float_t)(1.0 / (aspect * tan_fov));
    row1.y = (float_t)(1.0 / tan_fov);
    row2.z = -(float_t)((z_far + z_near) / (z_far - z_near));
    row2.w = -1.0f;
    row3.z = -(float_t)((2.0 * z_far * z_near) / (z_far - z_near));
}

void mat4::look_at(const vec3& eye, const vec3& target, const vec3& up) {
    vec3 x_axis, y_axis, z_axis;
    vec3 xyz;

    z_axis = vec3::normalize(eye - target);

    x_axis = vec3::normalize(vec3::cross(up, z_axis));
    if (vec3::length(x_axis) == 0.0f)
        x_axis = { 1.0f, 0.0f, 0.0f };

    y_axis = vec3::normalize(vec3::cross(z_axis, x_axis));

    xyz.x = vec3::dot(x_axis, eye);
    xyz.y = vec3::dot(y_axis, eye);
    xyz.z = vec3::dot(z_axis, eye);

    row0 = { x_axis.x, y_axis.x, z_axis.x, 0.0f };
    row1 = { x_axis.y, y_axis.y, z_axis.y, 0.0f };
    row2 = { x_axis.z, y_axis.z, z_axis.z, 0.0f };
    *(vec3*)&row3 = -xyz;
    row3.w = 1.0f;
}

void mat4::look_at(const vec3& eye, const vec3& target) {
    vec3 up = { 0.0f, 1.0f, 0.0f };
    vec3 dir = target - eye;
    if (vec3::length_squared(dir) <= 0.000001f) {
        up.x = 0.0f;
        up.y = 0.0f;
        if (dir.z < 0.0f)
            up.z = 1.0f;
        else
            up.z = -1.0f;
    }

    look_at(eye, target, up);
}
