/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "interpolation.hpp"

void interpolate_chs_reverse_value(const float_t* arr, const size_t length,
    float_t& t1, float_t& t2, const size_t f1, const size_t f2, const size_t f) {
    const vec2 t = ((float_t)(int64_t)(f - f1) + vec2(0.0f, 1.0f)) / (float_t)(int64_t)(f2 - f1);
    const vec2 t_2 = t * t;
    const vec2 t_3 = t_2 * t;
    const vec2 t_23 = 3.0f * t_2;
    const vec2 t_32 = 2.0f * t_3;

    const vec2 h00 = t_32 - t_23 + 1.0f;
    const vec2 h01 = t_23 - t_32;
    const vec2 h10 = t_3 - 2.0f * t_2 + t;
    const vec2 h11 = t_3 - t_2;

    const vec2 t1_t2 = *(vec2*)&arr[f] - h00 * arr[f1] - h01 * arr[f2];
    const float_t t_div = (t_2.x - t.x) * (t_2.y - t.y);
    const vec2 t1_t2a = t1_t2 / t_div;

    t1 = -h11.y * t1_t2a.x + h11.x * t1_t2a.y;
    t2 = h10.y * t1_t2a.x - h10.x * t1_t2a.y;
}

void interpolate_chs_reverse_value(const float_t* arr, const size_t length, float_t& t1a, float_t& t2a,
    float_t& t1b, float_t& t2b, float_t& t1c, float_t& t2c, const size_t f1, const size_t f2, const size_t f) {
    const vec4 t = ((float_t)(int64_t)(f - f1) + vec4(0.0f, 1.0f, 2.0f, 3.0f)) / (float_t)(int64_t)(f2 - f1);
    const vec4 t_2 = t * t;
    const vec4 t_3 = t_2 * t;
    const vec4 t_23 = 3.0f * t_2;
    const vec4 t_32 = 2.0f * t_3;

    const vec4 h00 = t_32 - t_23 + 1.0f;
    const vec4 h01 = t_23 - t_32;
    const vec4 h10 = t_3 - 2.0f * t_2 + t;
    const vec4 h11 = t_3 - t_2;

    const vec4 t1_t2 = *(vec4*)&arr[f] - h00 * arr[f1] - h01 * arr[f2];
    const vec3 t_div = (*(vec3*)&t_2.x - *(vec3*)&t.x) * (*(vec3*)&t_2.y - *(vec3*)&t.y);
    const vec3 t1_t2a = *(vec3*)&t1_t2.x / t_div;
    const vec3 t1_t2b = *(vec3*)&t1_t2.y / t_div;

    const vec3 t1 = -*(vec3*)&h11.y * t1_t2a + *(vec3*)&h11.x * t1_t2b;
    const vec3 t2 = *(vec3*)&h10.y * t1_t2a - *(vec3*)&h10.x * t1_t2b;

    t1a = t1.x;
    t2a = t2.x;
    t1b = t1.y;
    t2b = t2.y;
    t1c = t1.z;
    t2c = t2.z;
}

void interpolate_chs_reverse_value(const double_t* arr, const size_t length,
    double_t& t1, double_t& t2, const size_t f1, const size_t f2, const size_t f) {
    const vec2d t = ((double_t)(int64_t)(f - f1) + vec2d(0.0, 1.0)) / (double_t)(int64_t)(f2 - f1);
    const vec2d t_2 = t * t;
    const vec2d t_3 = t_2 * t;
    const vec2d t_23 = 3.0 * t_2;
    const vec2d t_32 = 2.0 * t_3;

    const vec2d h00 = t_32 - t_23 + 1.0;
    const vec2d h01 = t_23 - t_32;
    const vec2d h10 = t_3 - 2.0 * t_2 + t;
    const vec2d h11 = t_3 - t_2;

    const vec2d t1_t2 = *(vec2d*)&arr[f] - h00 * arr[f1] - h01 * arr[f2];
    const double_t t_div = (t_2.x - t.x) * (t_2.y - t.y);
    const vec2d t1_t2a = t1_t2 / t_div;

    t1 = -h11.y * t1_t2a.x + h11.x * t1_t2a.y;
    t2 = h10.y * t1_t2a.x - h10.x * t1_t2a.y;
}

void interpolate_chs_reverse(const float_t* arr, const size_t length,
    float_t& t1, float_t& t2, const size_t f1, const size_t f2) {
    t1 = 0.0f;
    t2 = 0.0f;

    if (f2 - f1 - 2 < 1)
        return;

    double_t tt1 = 0.0;
    double_t tt2 = 0.0;

    size_t i = f1 + 1;
    for (; i < f2 - 1 && i + 3 <= f2 - 1; i += 3) {
        float_t t1a = 0.0f;
        float_t t2a = 0.0f;
        float_t t1b = 0.0f;
        float_t t2b = 0.0f;
        float_t t1c = 0.0f;
        float_t t2c = 0.0f;
        interpolate_chs_reverse_value(arr, length, t1a, t2a, t1b, t2b, t1c, t2c, f1, f2, i);
        tt1 += t1a;
        tt1 += t2a;
        tt1 += t1b;
        tt1 += t2b;
        tt1 += t1c;
        tt1 += t2c;
    }

    for (; i < f2 - 1; i++) {
        float_t t1 = 0.0f;
        float_t t2 = 0.0f;
        interpolate_chs_reverse_value(arr, length, t1, t2, f1, f2, i);
        tt1 += t1;
        tt2 += t2;
    }

    t1 = (float_t)(tt1 / (double_t)(f2 - f1 - 2));
    t2 = (float_t)(tt2 / (double_t)(f2 - f1 - 2));
}

void interpolate_chs_reverse(const double_t* arr, const size_t length,
    double_t& t1, double_t& t2, const size_t f1, const size_t f2) {
    t1 = 0.0;
    t2 = 0.0;

    if (f2 - f1 - 2 < 1)
        return;

    double_t _t1 = 0.0;
    double_t _t2 = 0.0;
    double_t tt1 = 0.0;
    double_t tt2 = 0.0;
    for (size_t i = f1 + 1; i < f2 - 1; i++) {
        interpolate_chs_reverse_value(arr, length, _t1, _t2, f1, f2, i);
        tt1 += _t1;
        tt2 += _t2;
    }
    t1 = tt1 / (double_t)(f2 - f1 - 2);
    t2 = tt2 / (double_t)(f2 - f1 - 2);
}

int32_t interpolate_chs_reverse_sequence(const std::vector<float_t>& values_src,
    std::vector<kft3>& values, const bool fast) {
    size_t count = values_src.size();
    if (!count)
        return 0;
    else if (count == 1) {
        if (values_src[0] != 0.0f) {
            values.push_back({ 0, values_src[0] });
            return 1;
        }
        else
            return 0;
    }
    else {
        const float_t val = values_src.data()[0];
        const float_t* arr = &values_src.data()[1];

        bool constant = true;
        size_t i = count;
        if (constant) {
            const vec4 p1[8] = { val, val, val, val, val, val, val, val };
            for (size_t j = i / 32; j; j--, i -= 32)
                if (memcmp(p1, arr, sizeof(vec4) * 8)) {
                    constant = false;
                    break;
                }
                else
                    arr += 32;
        }

        if (constant) {
            const vec4 p1[4] = { val, val, val, val };
            for (size_t j = i / 16; j; j--, i -= 16)
                if (memcmp(p1, arr, sizeof(vec4) * 4)) {
                    constant = false;
                    break;
                }
                else
                    arr += 16;
        }

        if (constant) {
            const vec4 p1[2] = { val, val };
            for (size_t j = i / 8; j; j--, i -= 8)
                if (memcmp(p1, arr, sizeof(vec4) * 2)) {
                    constant = false;
                    break;
                }
                else
                    arr += 8;
        }

        if (constant) {
            const vec4 p1 = val;
            for (size_t j = i / 4; j; j--, i -= 4)
                if (memcmp(&p1, arr, sizeof(vec4))) {
                    constant = false;
                    break;
                }
                else
                    arr += 4;
        }

        if (constant) {
            const float_t p1 = val;
            for (size_t j = i; j; j--, i--)
                if (memcmp(&p1, arr, sizeof(float_t))) {
                    constant = false;
                    break;
                }
                else
                    arr++;
        }

        if (constant)
            if (values_src[0] != 0.0f) {
                values.push_back({ 0, values_src[0] });
                return 1;
            }
            else
                return 0;
    }

    const float_t* arr = values_src.data();

    const float_t reverse_bias = 0.0001f;
    const int32_t reverse_min_count = 4;

    const float_t* a = arr;
    size_t left_count = count;
    int32_t frame = 0;
    int32_t prev_frame = 0;
    float_t t2_old = 0.0f;
    while (left_count > 0) {
        if (left_count < reverse_min_count) {
            if (left_count > 1) {
                values.push_back({ (float_t)frame, a[0], t2_old, 0.0f });
                for (size_t j = 1; j < left_count - 1; j++)
                    values.push_back({ (float_t)(int64_t)(frame + j), a[j] });
                t2_old = 0.0f;
            }
            break;
        }

        size_t i = 0;
        size_t i_prev = 0;
        float_t t1 = 0.0f;
        float_t t2 = 0.0f;
        float_t t1_prev = 0.0f;
        float_t t2_prev = 0.0f;
        bool has_prev_succeded = false;
        bool has_error = false;
        bool has_prev_error = false;
        bool constant_prev = false;

        int32_t c = 0;
        for (i = reverse_min_count - 1, i_prev = i; i < left_count; i++) {
            bool constant = true;
            size_t j = 1;
            if (constant) {
                const vec4 p1[8] = { a[0], a[0], a[0], a[0], a[0], a[0], a[0], a[0] };
                for (; j <= i && j + 32 <= i; j += 32)
                    if (memcmp(p1, &a[j], sizeof(vec4) * 8)) {
                        constant = false;
                        break;
                    }
            }

            if (constant) {
                const vec4 p1[4] = { a[0], a[0], a[0], a[0] };
                for (; j <= i && j + 16 <= i; j += 16)
                    if (memcmp(p1, &a[j], sizeof(vec4) * 4)) {
                        constant = false;
                        break;
                    }
            }

            if (constant) {
                const vec4 p1[2] = { a[0], a[0] };
                for (; j <= i && j + 8 <= i; j += 8)
                    if (memcmp(p1, &a[j], sizeof(vec4) * 2)) {
                        constant = false;
                        break;
                    }
            }

            if (constant) {
                const vec4 p1 = a[0];
                for (; j <= i && j + 4 <= i; j += 4)
                    if (memcmp(&p1, &a[j], sizeof(vec4))) {
                        constant = false;
                        break;
                    }
            }

            if (constant) {
                const float_t p1 = a[0];
                for (; j <= i; j++)
                    if (memcmp(&p1, &a[j], sizeof(float_t))) {
                        constant = false;
                        break;
                    }
            }

            t1 = 0.0f;
            t2 = 0.0f;
            has_error = false;

            if (!constant) {
                if (!fast) {
                    double_t t1_accum = 0.0;
                    double_t t2_accum = 0.0;

                    size_t j = 1;
                    for (; j < i - 1 && j + 3 <= i - 1; j += 3) {
                        float_t t1a = 0.0f;
                        float_t t2a = 0.0f;
                        float_t t1b = 0.0f;
                        float_t t2b = 0.0f;
                        float_t t1c = 0.0f;
                        float_t t2c = 0.0f;
                        interpolate_chs_reverse_value(a, left_count, t1a, t2a, t1b, t2b, t1c, t2c, 0, i, j);
                        t1_accum += t1a;
                        t2_accum += t2a;
                        t1_accum += t1b;
                        t2_accum += t2b;
                        t1_accum += t1c;
                        t2_accum += t2c;
                    }

                    for (; j < i - 1; j++) {
                        float_t t1 = 0.0f;
                        float_t t2 = 0.0f;
                        interpolate_chs_reverse_value(a, left_count, t1, t2, 0, i, j);
                        t1_accum += t1;
                        t2_accum += t2;
                    }
                    t1 = (float_t)(t1_accum / (double_t)(i - 2));
                    t2 = (float_t)(t2_accum / (double_t)(i - 2));
                }
                else
                    interpolate_chs_reverse_value(a, left_count, t1, t2, 0, i, 1);

                has_error = false;
                for (size_t j = 1; j < i; j++) {
                    float_t val = interpolate_chs_value(a[0], a[i], t1, t2, 0.0f, (float_t)i, (float_t)j);
                    if (fabsf(val - a[j]) > reverse_bias) {
                        has_error = true;
                        break;
                    }
                }

                if (fabsf(t1) > 0.5f || fabsf(t2) > 0.5f)
                    has_error = true;
            }

            if (!has_error) {
                i_prev = i;
                t1_prev = t1;
                t2_prev = t2;
                constant_prev = constant;
                has_prev_error = false;
                has_prev_succeded = true;
                if (i < left_count)
                    continue;
            }

            if (has_prev_succeded) {
                i = i_prev;
                t1 = t1_prev;
                t2 = t2_prev;
                constant = constant_prev;
                has_error = false;
                has_prev_succeded = false;
            }

            if (!has_error) {
                if (constant) {
                    t1 = 0.0f;
                    t2 = 0.0f;
                }

                c = (int32_t)i;
                values.push_back({ (float_t)frame, a[0], t2_old, t1 });
                t2_old = t2;
                has_prev_error = false;
                break;
            }

            has_prev_error = true;
        }

        if (has_prev_succeded) {
            if (has_error) {
                values.push_back({ (float_t)frame, a[0], t2_old, 0.0f });
                for (size_t j = 1; j < c; j++)
                    values.push_back({ (float_t)(int64_t)(frame + j), a[j] });
                t2_old = 0.0f;
            }
            else {
                values.push_back({ (float_t)frame, a[0], t2_old, t1_prev });
                t2_old = t2_prev;
            }
            c = (int32_t)i;
        }
        else if (has_prev_error) {
            values.push_back({ (float_t)frame, a[0], t2_old, 0.0f });
            t2_old = 0.0f;
            c = 1;
        }

        prev_frame = frame;
        frame += c;
        a += c;
        left_count -= c;
    }

    values.push_back({ (float_t)(int64_t)(count - 1), arr[count - 1], t2_old, 0.0f });

    if (values.size() > 2) {
        kft3* keys = values.data();
        size_t length = values.size();
        for (size_t i = 0; i < length - 3; i++)
            if (*(uint32_t*)&keys[i + 0].value == *(uint32_t*)&keys[i + 1].value
                && *(uint32_t*)&keys[i + 1].value == *(uint32_t*)&keys[i + 2].value
                && *(uint32_t*)&keys[i + 0].tangent2 == 0
                && *(uint32_t*)&keys[i + 1].tangent1 == 0
                && *(uint32_t*)&keys[i + 1].tangent2 == 0
                && *(uint32_t*)&keys[i + 2].tangent1 == 0) {
                keys[i + 1].frame = keys[i + 2].frame;
                keys[i + 1].tangent2 = keys[i + 2].tangent2;
                values.erase(values.begin() + (i + 2));
                keys = values.data();
                length = values.size();
                if (length < 3)
                    break;
            }
    }

    kft3* keys = values.data();
    size_t length = values.size();
    for (size_t i = 0; i < count; i++) {
        float_t frame = (float_t)(int64_t)i;

        kft3* first_key = keys;
        kft3* key = keys;
        size_t _length = length;
        size_t temp;
        while (_length > 0)
            if (frame < key[temp = _length / 2].frame)
                _length = temp;
            else {
                key += temp + 1;
                _length -= temp + 1;
            }

        float_t val;
        if (key == first_key)
            val = first_key->value;
        else if (key == &first_key[length])
            val = key[-1].value;
        else
            val = interpolate_linear_value(key[-1].value, key[0].value,
                key[-1].frame, key[0].frame, frame);

        if (fabsf(val - arr[i]) > reverse_bias)
            return 3;
    }

    for (kft3& i : values) {
        i.tangent1 = 0.0f;
        i.tangent2 = 0.0f;
    }
    return 2;
}
