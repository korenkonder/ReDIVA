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

    float_t df_1 = (float_t)(f - f1);
    float_t df_2 = (float_t)(f - f1 + 1);
    float_t _t1 = df_1 / (float_t)(f2 - f1);
    float_t _t2 = df_2 / (float_t)(f2 - f1);
    float_t t1_1 = _t1 - 1.0f;
    float_t t2_1 = _t2 - 1.0f;

    float_t t1_t2_1 = arr[f] - arr[f1] - _t1 * _t1 * (3.0f - 2.0f * _t1) * (arr[f2] - arr[f1]);
    float_t t1_t2_2 = arr[f + 1] - arr[f1] - _t2 * _t2 * (3.0f - 2.0f * _t2) * (arr[f2] - arr[f1]);
    t1_t2_1 /= df_1 * t1_1;
    t1_t2_2 /= df_2 * t2_1;

    t1 = (t1_t2_1 * _t2 - t1_t2_2 * _t1) / (_t1 - _t2);
    t2 = (-t1_t2_1 * t2_1 + t1_t2_2 * t1_1) / (_t1 - _t2);
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

void interpolate_chs_reverse_step_value(float_t* arr, size_t length,
    float_t& t1, float_t& t2, size_t f1, size_t f2, size_t f, uint8_t step) {
    t1 = 0.0f;
    t2 = 0.0f;

    if (!arr || length < 2 || f - f1 + 1 >= length || f < 1 || f < f1 || f + 2 > f2)
        return;

    float_t df_1 = (float_t)((f - f1) * step);
    float_t df_2 = (float_t)((f - f1 + 1) * step);
    float_t _t1 = df_1 / (float_t)((f2 - f1) * step);
    float_t _t2 = df_2 / (float_t)((f2 - f1) * step);
    float_t t1_1 = _t1 - 1.0f;
    float_t t2_1 = _t2 - 1.0f;

    float_t t1_t2_1 = arr[f] - arr[f1] - _t1 * _t1 * (3.0f - 2.0f * _t1) * (arr[f2] - arr[f1]);
    float_t t1_t2_2 = arr[f + 1] - arr[f1] - _t2 * _t2 * (3.0f - 2.0f * _t2) * (arr[f2] - arr[f1]);
    t1_t2_1 /= df_1 * t1_1;
    t1_t2_2 /= df_2 * t2_1;

    t1 = (t1_t2_1 * _t2 - t1_t2_2 * _t1) / (_t1 - _t2);
    t2 = (-t1_t2_1 * t2_1 + t1_t2_2 * t1_1) / (_t1 - _t2);
}

void interpolate_chs_reverse_step(float_t* arr, size_t length,
    float_t& t1, float_t& t2, size_t f1, size_t f2, uint8_t step) {
    t1 = 0.0f;
    t2 = 0.0f;

    if (f2 - f1 - 2 < 1)
        return;

    float_t _t1 = 0.0f;
    float_t _t2 = 0.0f;
    double_t tt1 = 0.0;
    double_t tt2 = 0.0;
    for (size_t i = f1 + 1; i < f2 - 1; i++) {
        interpolate_chs_reverse_step_value(arr, length, _t1, _t2, f1, f2, i, step);
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

    int32_t start_time = 0;
    int32_t end_time = (int32_t)(count - 1);

    float_t* arr = values_src.data();

    const float_t reverse_bias = 0.0001f;
    const int32_t reverse_min_count = 4;

    float_t* a = arr;
    size_t left_count = count;
    int32_t frame = start_time;
    int32_t prev_frame = start_time;
    float_t t2_old = 0.0f;
    while (left_count > 0) {
        if (left_count < reverse_min_count) {
            if (left_count > 1) {
                float_t _t2 = t2_old;
                for (size_t j = 0; j < left_count - 1; j++) {
                    values.push_back({ (float_t)(int32_t)(frame + j), a[j], _t2, 0.0f });
                    _t2 = 0.0f;
                }
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
                has_prev_succeded = true;
                has_prev_error = has_error;
                if (i < left_count)
                    continue;
            }

            if (has_prev_succeded) {
                i = i_prev;
                t1 = t1_prev;
                t2 = t2_prev;
                has_error = has_prev_error;
            }

            if (!has_error)
                c = (int32_t)i;
            else
                c = 1;

            if (has_error) {
                values.push_back({ (float_t)frame, a[0], t2_old, 0.0f });
                t2_old = 0.0f;
            }
            else {
                values.push_back({ (float_t)frame, a[0], t2_old, t1 });
                t2_old = t2;
            }
            has_prev_succeded = false;
            break;

            if (!has_error) {
                i_prev = i;
                t1_prev = t1;
                t2_prev = t2;
                has_prev_succeded = true;
                has_prev_error = has_error;
                if (i < left_count)
                    continue;
            }
            break;
        }

        if (has_prev_succeded) {
            if (has_error) {
                float_t _t2 = t2_old;
                for (size_t j = 0; j < c; j++) {
                    values.push_back({ (float_t)(frame + j), a[j], _t2, 0.0f });
                    _t2 = 0.0f;
                }
                t2_old = 0.0f;
            }
            else {
                values.push_back({ (float_t)frame, a[0], t2_old, t1_prev });
                t2_old = t2_prev;
            }
            c = (int32_t)i;
        }

        prev_frame = frame;
        frame += c;
        a += c;
        left_count -= c;
    }

    values.push_back({ (float_t)(int32_t)(start_time + (count - 1)), arr[count - 1], t2_old, 0.0f });

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
