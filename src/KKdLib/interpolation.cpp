/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "interpolation.hpp"

void interpolate_chs_reverse_value(float_t* arr, size_t length,
    float_t& t1, float_t& t2, size_t f1, size_t f2, size_t f) {
    t1 = 0.0f;
    t2 = 0.0f;

    if (!arr || length < 2 || f - f1 + 1 >= length || f < 1 || f < f1 || f + 2 > f2)
        return;

    float_t _t1 = (float_t)(f - f1) / (float_t)(f2 - f1);
    float_t _t2 = (float_t)(f - f1 + 1) / (float_t)(f2 - f1);
    float_t t1_2 = _t1 * _t1;
    float_t t2_2 = _t2 * _t2;
    float_t t1_3 = t1_2 * _t1;
    float_t t2_3 = t2_2 * _t2;
    float_t t1_23 = 3.0f * t1_2;
    float_t t2_23 = 3.0f * t2_2;
    float_t t1_32 = 2.0f * t1_3;
    float_t t2_32 = 2.0f * t2_3;

    float_t h00_1 = t1_32 - t1_23 + 1.0f;
    float_t h00_2 = t2_32 - t2_23 + 1.0f;
    float_t h01_1 = t1_23 - t1_32;
    float_t h01_2 = t2_23 - t2_32;
    float_t h10_1 = t1_3 - 2.0f * t1_2 + _t1;
    float_t h10_2 = t2_3 - 2.0f * t2_2 + _t2;
    float_t h11_1 = t1_3 - t1_2;
    float_t h11_2 = t2_3 - t2_2;

    float_t t1_t2_1 = (arr[f] - h00_1 * arr[f1] - h01_1 * arr[f2]);
    float_t t1_t2_2 = (arr[f + 1] - h00_2 * arr[f1] - h01_2 * arr[f2]);

    t1_t2_1 /= (t1_2 - _t1) * (t2_2 - _t2);
    t1_t2_2 /= (t1_2 - _t1) * (t2_2 - _t2);

    t1 = -h11_2 * t1_t2_1 + h11_1 * t1_t2_2;
    t2 = h10_2 * t1_t2_1 - h10_1 * t1_t2_2;
}

void interpolate_chs_reverse(float_t* arr, size_t length,
    float_t& t1, float_t& t2, size_t f1, size_t f2) {
    t1 = 0.0f;
    t2 = 0.0f;

    if (f2 - f1 - 2 < 1)
        return;

    float_t _t1 = 0.0f;
    float_t _t2 = 0.0f;
    double_t tt1 = 0.0;
    double_t tt2 = 0.0;
    for (size_t i = f1 + 1; i < f2 - 1; i++) {
        interpolate_chs_reverse_value(arr, length, _t1, _t2, f1, f2, i);
        tt1 += _t1;
        tt2 += _t2;
    }
    t1 = (float_t)(tt1 / (double_t)(f2 - f1 - 2));
    t2 = (float_t)(tt2 / (double_t)(f2 - f1 - 2));
}

int32_t interpolate_chs_reverse_sequence(std::vector<float_t>& values_src, std::vector<kft3>& values) {
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
        uint32_t val = *(uint32_t*)&values_src.data()[0];
        uint32_t* arr = (uint32_t*)&values_src.data()[1];
        for (size_t i = count - 1; i; i--)
            if (val != *arr++)
                break;

        if (arr == (uint32_t*)(values_src.data() + count))
            if (values_src[0] != 0.0f) {
                values.push_back({ 0, values_src[0] });
                return 1;
            }
            else
                return 0;
    }

    float_t* arr = values_src.data();

    const float_t reverse_bias = 0.0001f;
    const int32_t reverse_min_count = 4;

    float_t* a = arr;
    size_t left_count = count;
    int32_t frame = 0;
    int32_t prev_frame = 0;
    float_t t2_old = 0.0f;
    while (left_count > 0) {
        if (left_count < reverse_min_count) {
            if (left_count > 1) {
                values.push_back({ (float_t)frame, a[0], t2_old, 0.0f });
                for (size_t j = 1; j < left_count - 1; j++)
                    values.push_back({ (float_t)(int32_t)(frame + j), a[j] });
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

        int32_t c = 0;
        for (i = reverse_min_count - 1, i_prev = i; i < left_count; i++) {
            bool constant = true;
            for (size_t j = 1; j <= i; i++)
                if (!memcmp(&a[0], &a[j], sizeof(float_t))) {
                    constant = false;
                    break;
                }

            double_t tt1 = 0.0;
            double_t tt2 = 0.0;
            for (size_t j = 1; j < i; j++) {
                float_t _t1 = 0.0f;
                float_t _t2 = 0.0f;
                interpolate_chs_reverse_value(a, left_count, _t1, _t2, 0, i, j);
                tt1 += _t1;
                tt2 += _t2;
            }
            t1 = (float_t)(tt1 / (double_t)(i - 2));
            t2 = (float_t)(tt2 / (double_t)(i - 2));

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

            if (!has_error) {
                i_prev = i;
                t1_prev = t1;
                t2_prev = t2;
                has_prev_error = false;
                has_prev_succeded = true;
                if (i < left_count)
                    continue;
            }

            if (has_prev_succeded) {
                i = i_prev;
                t1 = t1_prev;
                t2 = t2_prev;
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
                    values.push_back({ (float_t)(int32_t)(frame + j), a[j] });
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

    values.push_back({ (float_t)(int32_t)(count - 1), arr[count - 1], t2_old, 0.0f });

    kft3* keys = values.data();
    size_t length = values.size();
    for (size_t i = 0; i < count; i++) {
        float_t frame = (float_t)(int32_t)i;

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
    return 2;
}
