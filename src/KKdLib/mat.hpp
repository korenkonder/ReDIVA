/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "vec.hpp"
#include "quat.hpp"

struct mat3 {
    vec3 row0;
    vec3 row1;
    vec3 row2;

    inline mat3() : row0(), row1(), row2() {

    }

    inline mat3(vec3 row0, vec3 row1, vec3 row2) :
        row0(row0), row1(row1), row2(row2) {

    }

    void set(const quat& in_q1);
    void set(const vec3& in_axis, const float_t in_angle);
    void set(const vec3& in_axis, const float_t s, const float_t c);

    void add(const float_t value);
    void add(const mat3& in_m1, const mat3& in_m2);
    void sub(const float_t value);
    void sub(const mat3& in_m1, const mat3& in_m2);
    void mul(const float_t value);
    void mul(const mat3& in_m1, const mat3& in_m2);
    void mul(const mat3& in_m1, const vec3& in_axis, const float_t in_angle);

    void transform_vector(const vec2& normal, vec2& normalOut) const;
    void transform_vector(const vec3& normal, vec3& normalOut) const;
    void inverse_transform_vector(const vec2& normal, vec2& normalOut) const;
    void inverse_transform_vector(const vec3& normal, vec3& normalOut) const;

    void transpose(const mat3& in_m1);
    void invert();
    void invert_fast();
    void normalize();
    void normalize_rotation();
    float_t determinant() const;

    void rotate_x(float_t rad);
    void rotate_y(float_t rad);
    void rotate_z(float_t rad);
    void rotate_x(float_t s, float_t c);
    void rotate_y(float_t s, float_t c);
    void rotate_z(float_t s, float_t c);
    void rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_x(float_t rad);
    void mul_rotate_y(float_t rad);
    void mul_rotate_z(float_t rad);
    void mul_rotate_x(float_t s, float_t c);
    void mul_rotate_y(float_t s, float_t c);
    void mul_rotate_z(float_t s, float_t c);
    void mul_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z);

    void scale(float_t sx, float_t sy, float_t sz);
    void scale_x(float_t s);
    void scale_y(float_t s);
    void scale_z(float_t s);
    void mul_scale(float_t sx, float_t sy, float_t sz);
    void mul_scale_x(float_t s);
    void mul_scale_y(float_t s);
    void mul_scale_z(float_t s);

    void get_rotation(vec3& out_rad) const;
    void get_scale(vec3& out_s) const;
    float_t get_max_scale() const;

    inline void transform_vector(vec2& normalInOut) const {
        transform_vector(normalInOut, normalInOut);
    }

    inline void transform_vector(vec3& normalInOut) const {
        transform_vector(normalInOut, normalInOut);
    }

    inline void inverse_transform_vector(vec2& normalInOut) const {
        inverse_transform_vector(normalInOut, normalInOut);
    }

    inline void inverse_transform_vector(vec3& normalInOut) const {
        inverse_transform_vector(normalInOut, normalInOut);
    }


    inline void transpose() {
        transpose(*this);
    }

    inline void invert(const mat3& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        invert();
    }

    inline void invert_fast(const mat3& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        invert_fast();
    }

    inline void normalize(const mat3& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        normalize();
    }

    inline void normalize_rotation(const mat3& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        normalize_rotation();
    }

    inline void rotate_xyz(const vec3& rad) {
        rotate_xyz(rad.x, rad.y, rad.z);
    }

    inline void rotate_xzy(const vec3& rad) {
        rotate_xzy(rad.x, rad.y, rad.z);
    }

    inline void rotate_yxz(const vec3& rad) {
        rotate_yxz(rad.x, rad.y, rad.z);
    }

    inline void rotate_yzx(const vec3& rad) {
        rotate_yzx(rad.x, rad.y, rad.z);
    }

    inline void rotate_zxy(const vec3& rad) {
        rotate_zxy(rad.x, rad.y, rad.z);
    }

    inline void rotate_zyx(const vec3& rad) {
        rotate_zyx(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_xyz(const vec3& rad) {
        mul_rotate_xyz(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_xzy(const vec3& rad) {
        mul_rotate_xzy(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_yxz(const vec3& rad) {
        mul_rotate_yxz(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_yzx(const vec3& rad) {
        mul_rotate_yzx(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_zxy(const vec3& rad) {
        mul_rotate_zxy(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_zyx(const vec3& rad) {
        mul_rotate_zyx(rad.x, rad.y, rad.z);
    }

    inline void scale(const vec3& s) {
        scale(s.x, s.y, s.z);
    }

    inline void mul_scale(float_t s) {
        mul_scale(s, s, s);
    }

    inline void mul_scale(const vec3& s) {
        mul_scale(s.x, s.y, s.z);
    }

    inline vec3 get_scale() const {
        vec3 out_s;
        get_scale(out_s);
        return out_s;
    }

    inline vec3 get_rotation() const {
        vec3 out_rad;
        get_rotation(out_rad);
        return out_rad;
    }
};

struct mat4 {
    vec4 row0;
    vec4 row1;
    vec4 row2;
    vec4 row3;

    inline mat4() : row0(), row1(), row2(), row3() {

    }

    inline mat4(vec4 row0, vec4 row1, vec4 row2, vec4 row3) :
        row0(row0), row1(row1), row2(row2), row3(row3) {

    }

    void set(const quat& in_q1);
    void set(const vec3& in_v1, const vec3& in_v2);
    void set(const vec3& in_axis, const float_t in_angle);
    void set(const vec3& in_axis, const float_t s, const float_t c);
    void set_rotation(const quat& in_q1);
    void set_rotation(const vec3& in_axis, const float_t in_angle);
    void set_rotation(const vec3& in_axis, const float_t s, const float_t c);

    void add(const float_t value);
    void add(const mat4& in_m1, const mat4& in_m2);
    void sub(const float_t value);
    void sub(const mat4& in_m1, const mat4& in_m2);
    void mul(const float_t value);
    void mul(const mat4& in_m1, const mat4& in_m2);
    void mul_rotation(const mat4& in_m1, const vec3& in_axis, const float_t in_angle);

    void transform_vector(const vec2& normal, vec2& normalOut) const;
    void transform_vector(const vec3& normal, vec3& normalOut) const;
    void transform_vector(const vec4& normal, vec4& normalOut) const;
    void transform_point(const vec2& point, vec2& pointOut) const;
    void transform_point(const vec3& point, vec3& pointOut) const;
    void inverse_transform_vector(const vec2& normal, vec2& normalOut) const;
    void inverse_transform_vector(const vec3& normal, vec3& normalOut) const;
    void inverse_transform_vector(const vec4& normal, vec4& normalOut) const;
    void inverse_transform_point(const vec2& point, vec2& pointOut) const;
    void inverse_transform_point(const vec3& point, vec3& pointOut) const;

    void transpose(const mat4& in_m1);
    void invert();
    void invert_rotation();
    void invert_fast();
    void invert_rotation_fast();
    void normalize();
    void normalize_rotation();
    float_t determinant() const;

    void rotate_x(float_t rad);
    void rotate_y(float_t rad);
    void rotate_z(float_t rad);
    void rotate_x(float_t s, float_t c);
    void rotate_y(float_t s, float_t c);
    void rotate_z(float_t s, float_t c);
    void rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z);
    void rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_x(float_t rad);
    void mul_rotate_y(float_t rad);
    void mul_rotate_z(float_t rad);
    void mul_rotate_x(float_t s, float_t c);
    void mul_rotate_y(float_t s, float_t c);
    void mul_rotate_z(float_t s, float_t c);
    void mul_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z);
    void mul_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z);

    void scale(float_t sx, float_t sy, float_t sz);
    void scale(float_t sx, float_t sy, float_t sz, float_t sw);
    void scale_x(float_t s);
    void scale_y(float_t s);
    void scale_z(float_t s);
    void scale_w(float_t s);
    void mul_scale(float_t sx, float_t sy, float_t sz, float_t sw);
    void mul_scale_x(float_t s);
    void mul_scale_y(float_t s);
    void mul_scale_z(float_t s);
    void mul_scale_w(float_t s);

    void scale_rot(float_t sx, float_t sy, float_t sz);
    void scale_x_rot(float_t s);
    void scale_y_rot(float_t s);
    void scale_z_rot(float_t s);

    void translate(float_t tx, float_t ty, float_t tz);
    void translate_x(float_t t);
    void translate_y(float_t t);
    void translate_z(float_t t);
    void mul_translate(float_t tx, float_t ty, float_t tz);
    void mul_translate_x(float_t t);
    void mul_translate_y(float_t t);
    void mul_translate_z(float_t t);
    void add_translate(float_t tx, float_t ty, float_t tz);
    void add_translate_x(float_t t);
    void add_translate_y(float_t t);
    void add_translate_z(float_t t);

    void to_mat3(mat3& out_m) const;
    void to_mat3_inverse(mat3& out_m) const;
    void from_mat3(const mat3& in_m1);
    void from_mat3_inverse(const mat3& in_m1);

    void rotation_from_mat3(const mat3& in_m1);
    void rotation_from_mat3_inverse(const mat3& in_m1);

    void clear_rot();
    void clear_trans();

    void get_scale(vec3& out_s) const;
    void get_rotation(vec3& out_rad) const;
    void get_translation(vec3& out_t) const;
    float_t get_max_scale() const;

    void set_translation(const vec3& in_t);

    void blend(const mat4& in_m1, const mat4& in_m2, float_t blend);
    void blend_rotation(const mat4& in_m1, const mat4& in_m2,  float_t blend);
    void lerp_rotation(const mat4& in_m1, const mat4& in_m2, float_t blend);

    void frustrum(double_t left, double_t right,
        double_t bottom, double_t top, double_t z_near, double_t z_far);
    void ortho(double_t left, double_t right,
        double_t bottom, double_t top, double_t z_near, double_t z_far);
    void persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far);
    void look_at(const vec3& eye, const vec3& target, const vec3& up);
    void look_at(const vec3& eye, const vec3& target);

    inline void transform_vector(vec2& normalInOut) const {
        transform_vector(normalInOut, normalInOut);
    }

    inline void transform_vector(vec3& normalInOut) const {
        transform_vector(normalInOut, normalInOut);
    }

    inline void transform_vector(vec4& normalInOut) const {
        transform_vector(normalInOut, normalInOut);
    }

    inline void transform_point(vec2& pointInOut) const {
        transform_point(pointInOut, pointInOut);
    }

    inline void transform_point(vec3& pointInOut) const {
        transform_point(pointInOut, pointInOut);
    }

    inline void inverse_transform_vector(vec2& normalInOut) const {
        inverse_transform_vector(normalInOut, normalInOut);
    }

    inline void inverse_transform_vector(vec3& normalInOut) const {
        inverse_transform_vector(normalInOut, normalInOut);
    }

    inline void inverse_transform_vector(vec4& normalInOut) const {
        inverse_transform_vector(normalInOut, normalInOut);
    }

    inline void inverse_transform_point(vec2& pointInOut) const {
        inverse_transform_point(pointInOut, pointInOut);
    }

    inline void inverse_transform_point(vec3& pointInOut) const {
        inverse_transform_point(pointInOut, pointInOut);
    }

    inline void transpose() {
        transpose(*this);
    }

    inline void invert(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        invert();
    }

    inline void invert_rotation(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        invert_rotation();
    }

    inline void invert_fast(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        invert_fast();
    }

    inline void invert_rotation_fast(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        invert_rotation_fast();
    }

    inline void normalize(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        normalize();
    }

    inline void normalize_rotation(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        normalize_rotation();
    }

    inline void rotate_xyz(const vec3& rad) {
        rotate_xyz(rad.x, rad.y, rad.z);
    }

    inline void rotate_xzy(const vec3& rad) {
        rotate_xzy(rad.x, rad.y, rad.z);
    }

    inline void rotate_yxz(const vec3& rad) {
        rotate_yxz(rad.x, rad.y, rad.z);
    }

    inline void rotate_yzx(const vec3& rad) {
        rotate_yzx(rad.x, rad.y, rad.z);
    }

    inline void rotate_zxy(const vec3& rad) {
        rotate_zxy(rad.x, rad.y, rad.z);
    }

    inline void rotate_zyx(const vec3& rad) {
        rotate_zyx(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_xyz(const vec3& rad) {
        mul_rotate_xyz(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_xzy(const vec3& rad) {
        mul_rotate_xzy(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_yxz(const vec3& rad) {
        mul_rotate_yxz(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_yzx(const vec3& rad) {
        mul_rotate_yzx(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_zxy(const vec3& rad) {
        mul_rotate_zxy(rad.x, rad.y, rad.z);
    }

    inline void mul_rotate_zyx(const vec3& rad) {
        mul_rotate_zyx(rad.x, rad.y, rad.z);
    }

    inline void scale(const vec3& s) {
        scale(s.x, s.y, s.z);
    }

    inline void mul_scale(float_t s) {
        mul_scale(s, s, s, s);
    }

    inline void mul_scale(const vec4& s) {
        mul_scale(s.x, s.y, s.z, s.w);
    }

    inline void scale_rot(const float_t s) {
        scale_rot(s, s, s);
    }

    inline void scale_rot(const vec3& s) {
        scale_rot(s.x, s.y, s.z);
    }

    inline void translate(const vec3& s) {
        translate(s.x, s.y, s.z);
    }

    inline void mul_translate(const vec3& t) {
        mul_translate(t.x, t.y, t.z);
    }

    inline void add_translate(const vec3& t) {
        add_translate(t.x, t.y, t.z);
    }

    inline void clear_rot(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        clear_rot();
    }

    inline void clear_trans(const mat4& in_m1) {
        if (this != &in_m1)
            *this = in_m1;
        clear_trans();
    }

    inline vec3 get_scale() const {
        vec3 out_s;
        get_scale(out_s);
        return out_s;
    }

    inline vec3 get_rotation() const {
        vec3 out_rad;
        get_rotation(out_rad);
        return out_rad;
    }

    inline vec3 get_translation() const {
        vec3 out_t;
        get_translation(out_t);
        return out_t;
    }
};

extern const mat3 mat3_identity;
extern const mat3 mat3_null;
extern const mat4 mat4_identity;
extern const mat4 mat4_null;

inline void mat3_set(const quat* in_q1, mat3* out_m) {
    out_m->set(*in_q1);
}

inline void mat3_set(const vec3* in_axis, const float_t in_angle, mat3* out_m) {
    out_m->set(*in_axis, in_angle);
}

inline void mat3_set(const vec3* in_axis, const float_t s, const float_t c, mat3* out_m) {
    out_m->set(*in_axis, s, c);
}

inline void mat3_add(const mat3* in_m1, const float_t value, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul(value);
}

inline void mat3_add(const mat3* in_m1, const mat3* in_m2, mat3* out_m) {
    out_m->add(*in_m1, *in_m2);
}

inline void mat3_sub(const mat3* in_m1, const float_t value, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul(value);
}

inline void mat3_sub(const mat3* in_m1, const mat3* in_m2, mat3* out_m) {
    out_m->sub(*in_m1, *in_m2);
}

inline void mat3_mul(const mat3* in_m1, const float_t value, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul(value);
}

inline void mat3_mul(const mat3* in_m1, const mat3* in_m2, mat3* out_m) {
    out_m->mul(*in_m1, *in_m2);
}

inline void mat3_mul(const mat3* in_m1, const vec3* in_axis, const float_t in_angle, mat3* out_m) {
    out_m->mul(*in_m1, *in_axis, in_angle);
}

inline void mat3_transform_vector(const mat3* in_m1, const vec2* normal, vec2* normalOut) {
    in_m1->transform_vector(*normal, *normalOut);
}

inline void mat3_transform_vector(const mat3* in_m1, const vec3* normal, vec3* normalOut) {
    in_m1->transform_vector(*normal, *normalOut);
}

inline void mat3_inverse_transform_vector(const mat3* in_m1, const vec2* normal, vec2* normalOut) {
    in_m1->inverse_transform_vector(*normal, *normalOut);
}

inline void mat3_inverse_transform_vector(const mat3* in_m1, const vec3* normal, vec3* normalOut) {
    in_m1->inverse_transform_vector(*normal, *normalOut);
}

inline void mat3_transpose(const mat3* in_m1, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->transpose();
}

inline void mat3_invert(const mat3* in_m1, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->invert();
}

inline void mat3_invert_fast(const mat3* in_m1, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->invert_fast();
}

inline void mat3_normalize(const mat3* in_m1, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->normalize();
}

inline void mat3_normalize_rotation(const mat3* in_m1, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->normalize_rotation();
}

inline float_t mat3_determinant(const mat3* in_m1) {
    return in_m1->determinant();
}

inline void mat3_rotate_x(float_t rad, mat3* out_m) {
    out_m->rotate_x(rad);
}

inline void mat3_rotate_y(float_t rad, mat3* out_m) {
    out_m->rotate_y(rad);
}

inline void mat3_rotate_z(float_t rad, mat3* out_m) {
    out_m->rotate_z(rad);
}

inline void mat3_rotate_x(float_t s, float_t c, mat3* out_m) {
    out_m->rotate_x(s, c);
}

inline void mat3_rotate_y(float_t s, float_t c, mat3* out_m) {
    out_m->rotate_y(s, c);
}

inline void mat3_rotate_z(float_t s, float_t c, mat3* out_m) {
    out_m->rotate_z(s, c);
}

inline void mat3_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    out_m->rotate_xyz(rad_x, rad_y, rad_z);
}

inline void mat3_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    out_m->rotate_xzy(rad_x, rad_y, rad_z);
}

inline void mat3_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    out_m->rotate_yxz(rad_x, rad_y, rad_z);
}

inline void mat3_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    out_m->rotate_yzx(rad_x, rad_y, rad_z);
}

inline void mat3_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    out_m->rotate_zxy(rad_x, rad_y, rad_z);
}

inline void mat3_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    out_m->rotate_zyx(rad_x, rad_y, rad_z);
}

inline void mat3_mul_rotate_x(const mat3* in_m1, float_t rad, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_x(rad);
}

inline void mat3_mul_rotate_y(const mat3* in_m1, float_t rad, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_y(rad);
}

inline void mat3_mul_rotate_z(const mat3* in_m1, float_t rad, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_z(rad);
}

inline void mat3_mul_rotate_x(const mat3* in_m1, float_t s, float_t c, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_x(s, c);
}

inline void mat3_mul_rotate_y(const mat3* in_m1, float_t s, float_t c, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_y(s, c);
}

inline void mat3_mul_rotate_z(const mat3* in_m1, float_t s, float_t c, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_z(s, c);
}

inline void mat3_mul_rotate_xyz(const mat3* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_xyz(rad_x, rad_y, rad_z);
}

inline void mat3_mul_rotate_xzy(const mat3* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_xzy(rad_x, rad_y, rad_z);
}

inline void mat3_mul_rotate_yxz(const mat3* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_yxz(rad_x, rad_y, rad_z);
}

inline void mat3_mul_rotate_yzx(const mat3* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_yzx(rad_x, rad_y, rad_z);
}

inline void mat3_mul_rotate_zxy(const mat3* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_zxy(rad_x, rad_y, rad_z);
}

inline void mat3_mul_rotate_zyx(const mat3* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_zyx(rad_x, rad_y, rad_z);
}

inline void mat3_scale(float_t sx, float_t sy, float_t sz, mat3* out_m) {
    out_m->scale(sx, sy, sz);
}

inline void mat3_scale_x(float_t s, mat3* out_m) {
    out_m->scale_x(s);
}

inline void mat3_scale_y(float_t s, mat3* out_m) {
    out_m->scale_y(s);
}

inline void mat3_scale_z(float_t s, mat3* out_m) {
    out_m->scale_z(s);
}

inline void mat3_mul_scale(const mat3* in_m1, float_t sx, float_t sy, float_t sz, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale(sx, sy, sz);
}

inline void mat3_mul_scale_x(const mat3* in_m1, float_t s, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_x(s);
}

inline void mat3_mul_scale_y(const mat3* in_m1, float_t s, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_y(s);
}

inline void mat3_mul_scale_z(const mat3* in_m1, float_t s, mat3* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_z(s);
}

inline void mat3_get_rotation(const mat3* in_m1, vec3* out_rad) {
    in_m1->get_rotation(*out_rad);
}

inline void mat3_get_scale(const mat3* in_m1, vec3* out_s) {
    in_m1->get_scale(*out_s);
}

inline float_t mat3_get_max_scale(const mat3* in_m1) {
    return in_m1->get_max_scale();
}

inline void mat4_set(const quat* in_q1, mat4* out_m) {
    out_m->set(*in_q1);
}

inline void mat4_set(const vec3* in_v1, const vec3* in_v2, mat4* out_m) {
    out_m->set(*in_v1, *in_v2);
}

inline void mat4_set(const vec3* in_axis, const float_t in_angle, mat4* out_m) {
    out_m->set(*in_axis, in_angle);
}

inline void mat4_set(const vec3* in_axis, const float_t s, const float_t c, mat4* out_m) {
    out_m->set(*in_axis, s, c);
}

inline void mat4_set_rotation(mat4* in_m1, const quat* in_q1) {
    in_m1->set_rotation(*in_q1);
}

inline void mat4_set_rotation(mat4* in_m1, const vec3* in_axis, const float_t in_angle) {
    in_m1->set_rotation(*in_axis, in_angle);
}

inline void mat4_set_rotation(mat4* in_m1, const vec3* in_axis, const float_t s, const float_t c) {
    in_m1->set_rotation(*in_axis, s, c);
}

inline void mat4_add(const mat4* in_m1, const float_t value, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->add(value);
}

inline void mat4_add(const mat4* in_m1, const mat4* in_m2, mat4* out_m) {
    out_m->add(*in_m1, *in_m2);
}

inline void mat4_sub(const mat4* in_m1, const float_t value, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->sub(value);
}

inline void mat4_sub(const mat4* in_m1, const mat4* in_m2, mat4* out_m) {
    out_m->sub(*in_m1, *in_m2);
}

inline void mat4_mul(const mat4* in_m1, const float_t value, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul(value);
}

inline void mat4_mul(const mat4* in_m1, const mat4* in_m2, mat4* out_m) {
    out_m->mul(*in_m1, *in_m2);
}

inline void mat4_mul_rotation(const mat4* in_m1, const vec3* in_axis, const float_t in_angle, mat4* out_m) {
    out_m->mul_rotation(*in_m1, *in_axis, in_angle);
}

inline void mat4_transform_vector(const mat4* in_m1, const vec2* normal, vec2* normalOut) {
    in_m1->transform_vector(*normal, *normalOut);
}

inline void mat4_transform_vector(const mat4* in_m1, const vec3* normal, vec3* normalOut) {
    in_m1->transform_vector(*normal, *normalOut);
}

inline void mat4_transform_vector(const mat4* in_m1, const vec4* normal, vec4* normalOut) {
    in_m1->transform_vector(*normal, *normalOut);
}

inline void mat4_transform_point(const mat4* in_m1, const vec2* point, vec2* pointOut) {
    in_m1->transform_point(*point, *pointOut);
}

inline void mat4_transform_point(const mat4* in_m1, const vec3* point, vec3* pointOut) {
    in_m1->transform_point(*point, *pointOut);
}

inline void mat4_inverse_transform_vector(const mat4* in_m1, const vec2* normal, vec2* normalOut) {
    in_m1->inverse_transform_vector(*normal, *normalOut);
}

inline void mat4_inverse_transform_vector(const mat4* in_m1, const vec3* normal, vec3* normalOut) {
    in_m1->inverse_transform_vector(*normal, *normalOut);
}

inline void mat4_inverse_transform_vector(const mat4* in_m1, const vec4* normal, vec4* normalOut) {
    in_m1->inverse_transform_vector(*normal, *normalOut);
}

inline void mat4_inverse_transform_point(const mat4* in_m1, const vec2* point, vec2* pointOut) {
    in_m1->inverse_transform_point(*point, *pointOut);
}

inline void mat4_inverse_transform_point(const mat4* in_m1, const vec3* point, vec3* pointOut) {
    in_m1->inverse_transform_point(*point, *pointOut);
}

inline void mat4_transpose(const mat4* in_m1, mat4* out_m) {
    out_m->transpose(*in_m1);
}

inline void mat4_invert(const mat4* in_m1, mat4* out_m) {
    out_m->invert(*in_m1);
}

inline void mat4_invert_rotation(const mat4* in_m1, mat4* out_m) {
    out_m->invert_rotation(*in_m1);
}

inline void mat4_invert_fast(const mat4* in_m1, mat4* out_m) {
    out_m->invert_fast(*in_m1);
}

inline void mat4_invert_rotation_fast(const mat4* in_m1, mat4* out_m) {
    out_m->invert_rotation_fast(*in_m1);
}

inline void mat4_normalize(const mat4* in_m1, mat4* out_m) {
    out_m->normalize(*in_m1);
}

inline void mat4_normalize_rotation(const mat4* in_m1, mat4* out_m) {
    out_m->normalize_rotation(*in_m1);
}

inline float_t mat4_determinant(const mat4* in_m1) {
    return in_m1->determinant();
}

inline void mat4_rotate_x(float_t rad, mat4* out_m) {
    out_m->rotate_x(rad);
}

inline void mat4_rotate_y(float_t rad, mat4* out_m) {
    out_m->rotate_y(rad);
}

inline void mat4_rotate_z(float_t rad, mat4* out_m) {
    out_m->rotate_z(rad);
}

inline void mat4_rotate_x(float_t s, float_t c, mat4* out_m) {
    out_m->rotate_x(s, c);
}

inline void mat4_rotate_y(float_t s, float_t c, mat4* out_m) {
    out_m->rotate_y(s, c);
}

inline void mat4_rotate_z(float_t s, float_t c, mat4* out_m) {
    out_m->rotate_z(s, c);
}

inline void mat4_rotate_xyz(float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    out_m->rotate_xyz(rad_x, rad_y, rad_z);
}

inline void mat4_rotate_xzy(float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    out_m->rotate_xzy(rad_x, rad_y, rad_z);
}

inline void mat4_rotate_yxz(float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    out_m->rotate_yxz(rad_x, rad_y, rad_z);
}

inline void mat4_rotate_yzx(float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    out_m->rotate_yzx(rad_x, rad_y, rad_z);
}

inline void mat4_rotate_zxy(float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    out_m->rotate_zxy(rad_x, rad_y, rad_z);
}

inline void mat4_rotate_zyx(float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    out_m->rotate_zyx(rad_x, rad_y, rad_z);
}

inline void mat4_mul_rotate_x(const mat4* in_m1, float_t rad, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_x(rad);
}

inline void mat4_mul_rotate_y(const mat4* in_m1, float_t rad, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_y(rad);
}

inline void mat4_mul_rotate_z(const mat4* in_m1, float_t rad, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_z(rad);
}

inline void mat4_mul_rotate_x(const mat4* in_m1, float_t s, float_t c, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_x(s, c);
}

inline void mat4_mul_rotate_y(const mat4* in_m1, float_t s, float_t c, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_y(s, c);
}

inline void mat4_mul_rotate_z(const mat4* in_m1, float_t s, float_t c, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_z(s, c);
}

inline void mat4_mul_rotate_xyz(const mat4* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_xyz(rad_x, rad_y, rad_z);
}

inline void mat4_mul_rotate_xzy(const mat4* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_xzy(rad_x, rad_y, rad_z);
}

inline void mat4_mul_rotate_yxz(const mat4* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_yxz(rad_x, rad_y, rad_z);
}

inline void mat4_mul_rotate_yzx(const mat4* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_yzx(rad_x, rad_y, rad_z);
}

inline void mat4_mul_rotate_zxy(const mat4* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_zxy(rad_x, rad_y, rad_z);
}

inline void mat4_mul_rotate_zyx(const mat4* in_m1, float_t rad_x, float_t rad_y, float_t rad_z, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_rotate_zyx(rad_x, rad_y, rad_z);
}

inline void mat4_scale(float_t sx, float_t sy, float_t sz, mat4* out_m) {
    out_m->scale(sx, sy, sz);
}

inline void mat4_scale_x(float_t s, mat4* out_m) {
    out_m->scale_x(s);
}

inline void mat4_scale_y(float_t s, mat4* out_m) {
    out_m->scale_y(s);
}

inline void mat4_scale_z(float_t s, mat4* out_m) {
    out_m->scale_z(s);
}

inline void mat4_mul_scale(const mat4* in_m1, float_t sx, float_t sy, float_t sz, float_t sw, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale(sx, sy, sz, sw);
}

inline void mat4_mul_scale_x(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_x(s);
}

inline void mat4_mul_scale_y(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_y(s);
}

inline void mat4_mul_scale_z(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_z(s);
}

inline void mat4_mul_scale_w(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_scale_w(s);
}

inline void mat4_scale_rot(const mat4* in_m1, float_t sx, float_t sy, float_t sz, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->scale_rot(sx, sy, sz);
}

inline void mat4_scale_x_rot(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->scale_x_rot(s);
}

inline void mat4_scale_y_rot(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->scale_y_rot(s);
}

inline void mat4_scale_z_rot(const mat4* in_m1, float_t s, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->scale_z_rot(s);
}

inline void mat4_translate(float_t tx, float_t ty, float_t tz, mat4* out_m) {
    out_m->translate(tx, ty, tz);
}

inline void mat4_translate_x(float_t t, mat4* out_m) {
    out_m->translate_x(t);
}

inline void mat4_translate_y(float_t t, mat4* out_m) {
    out_m->translate_y(t);
}

inline void mat4_translate_z(float_t t, mat4* out_m) {
    out_m->translate_z(t);
}

inline void mat4_mul_translate(const mat4* in_m1, float_t tx, float_t ty, float_t tz, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_translate(tx, ty, tz);
}

inline void mat4_mul_translate_x(const mat4* in_m1, float_t t, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_translate_x(t);
}

inline void mat4_mul_translate_y(const mat4* in_m1, float_t t, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_translate_y(t);
}

inline void mat4_mul_translate_z(const mat4* in_m1, float_t t, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->mul_translate_z(t);
}

inline void mat4_add_translate(const mat4* in_m1, float_t tx, float_t ty, float_t tz, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->add_translate(tx, ty, tz);
}

inline void mat4_add_translate_x(const mat4* in_m1, float_t t, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->add_translate_x(t);
}

inline void mat4_add_translate_y(const mat4* in_m1, float_t t, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->add_translate_y(t);
}

inline void mat4_add_translate_z(const mat4* in_m1, float_t t, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->add_translate_z(t);
}

inline void mat4_to_mat3(const mat4* in_m1, mat3* out_m) {
    in_m1->to_mat3(*out_m);
}

inline void mat4_to_mat3_inverse(const mat4* in_m1, mat3* out_m) {
    in_m1->to_mat3_inverse(*out_m);
}

inline void mat4_from_mat3(const mat3* in_m1, mat4* out_m) {
    out_m->from_mat3(*in_m1);
}

inline void mat4_from_mat3_inverse(const mat3* in_m1, mat4* out_m) {
    out_m->from_mat3_inverse(*in_m1);
}

inline void mat4_clear_rot(const mat4* in_m1, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->clear_rot();
}

inline void mat4_clear_trans(const mat4* in_m1, mat4* out_m) {
    if (out_m != in_m1)
        *out_m = *in_m1;
    out_m->clear_trans();
}

inline void mat4_get_scale(const mat4* in_m1, vec3* out_s) {
    in_m1->get_scale(*out_s);
}

inline void mat4_get_rotation(const mat4* in_m1, vec3* out_rad) {
    in_m1->get_rotation(*out_rad);
}

inline void mat4_get_translation(const mat4* in_m1, vec3* out_t) {
    in_m1->get_translation(*out_t);
}

inline void mat4_set_translation(mat4* in_m1, const vec3* in_t) {
    in_m1->set_translation(*in_t);
}

inline float_t mat4_get_max_scale(const mat4* in_m1) {
    return in_m1->get_max_scale();
}

inline void mat4_blend(const mat4* in_m1, const mat4* in_m2, mat4* out_m, float_t blend) {
    out_m->blend(*in_m1, *in_m2, blend);
}

inline void mat4_blend_rotation(const mat4* in_m1, const mat4* in_m2, mat4* out_m, float_t blend) {
    out_m->blend_rotation(*in_m1, *in_m2, blend);
}

inline void mat4_lerp_rotation(const mat4* in_m1, const mat4* in_m2, mat4* out_m, float_t blend) {
    out_m->lerp_rotation(*in_m1, *in_m2, blend);
}

inline void mat4_frustrum(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* out_m) {
    out_m->frustrum(left, right, bottom, top, z_near, z_far);
}

inline void mat4_ortho(double_t left, double_t right,
    double_t bottom, double_t top, double_t z_near, double_t z_far, mat4* out_m) {
    out_m->ortho(left, right, bottom, top, z_near, z_far);
}

inline void mat4_persp(double_t fov_y, double_t aspect, double_t z_near, double_t z_far, mat4* out_m) {
    out_m->persp(fov_y, aspect, z_near, z_far);
}

inline void mat4_look_at(const vec3* eye, const vec3* target, const vec3* up, mat4* out_m) {
    out_m->look_at(*eye, *target, *up);
}

inline void mat4_look_at(const vec3* eye, const vec3* target, mat4* out_m) {
    out_m->look_at(*eye, *target);
}

inline void mat3_rotate_xyz(const vec3* rad, mat3* out_m) {
    mat3_rotate_xyz(rad->x, rad->y, rad->z, out_m);
}

inline void mat3_rotate_xzy(const vec3* rad, mat3* out_m) {
    mat3_rotate_xzy(rad->x, rad->y, rad->z, out_m);
}

inline void mat3_rotate_yxz(const vec3* rad, mat3* out_m) {
    mat3_rotate_yxz(rad->x, rad->y, rad->z, out_m);
}

inline void mat3_rotate_yzx(const vec3* rad, mat3* out_m) {
    mat3_rotate_yzx(rad->x, rad->y, rad->z, out_m);
}

inline void mat3_rotate_zxy(const vec3* rad, mat3* out_m) {
    mat3_rotate_zxy(rad->x, rad->y, rad->z, out_m);
}

inline void mat3_rotate_zyx(const vec3* rad, mat3* out_m) {
    mat3_rotate_zyx(rad->x, rad->y, rad->z, out_m);
}

inline void mat3_mul_rotate_xyz(const mat3* in_m1, const vec3* rad, mat3* out_m) {
    mat3_mul_rotate_xyz(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat3_mul_rotate_xzy(const mat3* in_m1, const vec3* rad, mat3* out_m) {
    mat3_mul_rotate_xzy(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat3_mul_rotate_yxz(const mat3* in_m1, const vec3* rad, mat3* out_m) {
    mat3_mul_rotate_yxz(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat3_mul_rotate_yzx(const mat3* in_m1, const vec3* rad, mat3* out_m) {
    mat3_mul_rotate_yzx(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat3_mul_rotate_zxy(const mat3* in_m1, const vec3* rad, mat3* out_m) {
    mat3_mul_rotate_zxy(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat3_mul_rotate_zyx(const mat3* in_m1, const vec3* rad, mat3* out_m) {
    mat3_mul_rotate_zyx(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat3_scale(const vec3* s, mat3* out_m) {
    mat3_scale(s->x, s->y, s->z, out_m);
}

inline void mat3_mul_scale(const mat3* in_m1, float_t s, mat3* out_m) {
    mat3_mul_scale(in_m1, s, s, s, out_m);
}

inline void mat3_mul_scale(const mat3* in_m1, const vec3* s, mat3* out_m) {
    mat3_mul_scale(in_m1, s->x, s->y, s->z, out_m);
}

inline void mat4_rotate_xyz(const vec3* rad, mat4* out_m) {
    mat4_rotate_xyz(rad->x, rad->y, rad->z, out_m);
}

inline void mat4_rotate_xzy(const vec3* rad, mat4* out_m) {
    mat4_rotate_xzy(rad->x, rad->y, rad->z, out_m);
}

inline void mat4_rotate_yxz(const vec3* rad, mat4* out_m) {
    mat4_rotate_yxz(rad->x, rad->y, rad->z, out_m);
}

inline void mat4_rotate_yzx(const vec3* rad, mat4* out_m) {
    mat4_rotate_yzx(rad->x, rad->y, rad->z, out_m);
}

inline void mat4_rotate_zxy(const vec3* rad, mat4* out_m) {
    mat4_rotate_zxy(rad->x, rad->y, rad->z, out_m);
}

inline void mat4_rotate_zyx(const vec3* rad, mat4* out_m) {
    mat4_rotate_zyx(rad->x, rad->y, rad->z, out_m);
}

inline void mat4_mul_rotate_xyz(const mat4* in_m1, const vec3* rad, mat4* out_m) {
    mat4_mul_rotate_xyz(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat4_mul_rotate_xzy(const mat4* in_m1, const vec3* rad, mat4* out_m) {
    mat4_mul_rotate_xzy(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat4_mul_rotate_yxz(const mat4* in_m1, const vec3* rad, mat4* out_m) {
    mat4_mul_rotate_yxz(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat4_mul_rotate_yzx(const mat4* in_m1, const vec3* rad, mat4* out_m) {
    mat4_mul_rotate_yzx(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat4_mul_rotate_zxy(const mat4* in_m1, const vec3* rad, mat4* out_m) {
    mat4_mul_rotate_zxy(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat4_mul_rotate_zyx(const mat4* in_m1, const vec3* rad, mat4* out_m) {
    mat4_mul_rotate_zyx(in_m1, rad->x, rad->y, rad->z, out_m);
}

inline void mat4_scale(const vec3* s, mat4* out_m) {
    mat4_scale(s->x, s->y, s->z, out_m);
}

inline void mat4_mul_scale(const mat4* in_m1, float_t s, mat4* out_m) {
    mat4_mul_scale(in_m1, s, s, s, s, out_m);
}

inline void mat4_mul_scale(const mat4* in_m1, vec4* s, mat4* out_m) {
    mat4_mul_scale(in_m1, s->x, s->y, s->z, s->w, out_m);
}

inline void mat4_scale_rot(const mat4* in_m1, const float_t s, mat4* out_m) {
    mat4_scale_rot(in_m1, s, s, s, out_m);
}

inline void mat4_scale_rot(const mat4* in_m1, const vec3* s, mat4* out_m) {
    mat4_scale_rot(in_m1, s->x, s->y, s->z, out_m);
}

inline void mat4_translate(const vec3* s, mat4* out_m) {
    mat4_translate(s->x, s->y, s->z, out_m);
}

inline void mat4_mul_translate(const mat4* in_m1, const vec3* t, mat4* out_m) {
    mat4_mul_translate(in_m1, t->x, t->y, t->z, out_m);
}

inline void mat4_add_translate(const mat4* in_m1, const vec3* t, mat4* out_m) {
    mat4_add_translate(in_m1, t->x, t->y, t->z, out_m);
}
