/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
#if defined(CRE_DEV)
    static float_t glitter_curve_add_key(bool has_error,
        bool has_error_lerp, bool has_error_hermite, float_t* a, float_t* b, int32_t frame,
        const uint8_t step, size_t i, float_t t1, float_t t2, float_t t2_old,
        std::vector<Curve::Key>* keys_rev);
#endif

    Curve::Key::Key() : type(), frame(), value(), tangent1(), tangent2(), random_range() {

    }

    Curve::Key::Key(KeyType type, int32_t frame, float_t value, float_t random_range) : tangent1(), tangent2() {
        this->type = type;
        this->frame = frame;
        this->value = value;
        this->random_range = random_range;
    }

    Curve::Key::Key(KeyType type, int32_t frame, float_t value,
        float_t tangent1, float_t tangent2, float_t random_range) {
        this->type = type;
        this->frame = frame;
        this->value = value;
        this->tangent1 = tangent1;
        this->tangent2 = tangent2;
        this->random_range = random_range;
    }

    Curve::Curve(GLT) : type(), repeat(),
        start_time(), end_time(), flags(), random_range() {
        version = GLT_VAL == Glitter::X ? 0x02 : 0x01;
        keys_version = GLT_VAL == Glitter::X ? 0x03 : 0x02;
        keys.reserve(0x80);
    }

    Curve::~Curve() {

    }

#if defined(CRE_DEV)
    void Curve::AddValue(GLT,  float_t val) {
        for (Curve::Key& i : keys_rev)
            i.value += val;
        Recalculate(GLT_VAL);
    }
#endif

    bool Curve::F2GetValue(GLT, float_t frame,
        float_t* value, int32_t random_value, Random* random) {
        size_t keys_count = keys.size();
        if (!keys_count)
            return false;

        int32_t random_val = random->GetValue();
        random->SetValue(random_value);
        bool negate = flags & CURVE_NEGATE
            && random->F2GetInt(GLT_VAL, 0, 0xFFFF) > 0x7FFF;
        if (flags & CURVE_STEP)
            random->SetValue(random_val + 1);

        float_t start_time;
        float_t end_time;
        float_t _value;
        if (keys_count == 1) {
            Curve::Key* key = &keys.data()[keys_count - 1];
            _value = F2RandomizeKey(GLT_VAL, key, random);
            goto End;
        }

        start_time = (float_t)this->start_time;
        end_time = (float_t)this->end_time;
        if (repeat && (start_time > frame || frame >= end_time)) {
            float_t t = (frame - start_time) / (end_time - start_time);
            if (t > 0.0f)
                t = (float_t)(int32_t)t;
            else if (t < 0.0f)
                t = (float_t)(int32_t)t - 1.0f;
            frame -= t * (end_time - start_time);
        }

        if (end_time <= frame) {
            Curve::Key* key = &keys.data()[keys_count - 1];
            _value = F2RandomizeKey(GLT_VAL, key, random);
        }
        else if (start_time > frame) {
            Curve::Key* key = &keys.data()[0];
            _value = F2RandomizeKey(GLT_VAL, key, random);
        }
        else if (flags & CURVE_BAKED) {
            size_t key_index;
            if (frame >= end_time)
                key_index = keys_count - 1;
            else if (frame > start_time) {
                key_index = (size_t)frame - this->start_time;
                if (GLT_VAL == Glitter::F2)
                    key_index /= 2;

                if (key_index >= keys_count)
                    key_index = keys_count - 1;
            }
            else
                key_index = 0;

            Curve::Key* key = &keys.data()[key_index];
            _value = F2RandomizeKey(GLT_VAL, key, random);
        }
        else {
            size_t curr_key_index = 0;
            size_t next_key_index = 0;
            if (keys_count > 3)
                GetKeyIndices(&keys, frame, &curr_key_index, &next_key_index);
            else if (keys_count == 3 && frame >= keys.data()[1].frame) {
                curr_key_index = 1;
                next_key_index = 2;
            }
            else {
                curr_key_index = 0;
                next_key_index = 1;
            }
            Curve::Key* curr_key = &keys.data()[curr_key_index];
            Curve::Key* next_key = &keys.data()[next_key_index];
            _value = F2Interpolate(GLT_VAL, frame, curr_key, next_key, curr_key->type, random);
        }

    End:
        _value = F2Randomize(GLT_VAL, _value, random);
        *value = negate ? -_value : _value;
        random->SetValue(random_val + 1);
        return true;
    }

    float_t Curve::F2Interpolate(GLT, float_t frame, Curve::Key* curr,
        Curve::Key* next, KeyType key_type, Random* random) {
        if (key_type == KEY_CONSTANT)
            return F2RandomizeKey(GLT_VAL, curr, random);
        else if (key_type == KEY_HERMITE)
            return F2InterpolateHermite(GLT_VAL, curr, next, frame, random);
        else
            return F2InterpolateLinear(GLT_VAL, curr, next, frame, random);
    }

    float_t Curve::F2InterpolateHermite(GLT, Glitter::Curve::Key* curr,
        Glitter::Curve::Key* next, float_t frame, Random* random) {
        float_t next_val = F2RandomizeKey(GLT_VAL, next, random);
        float_t curr_val = F2RandomizeKey(GLT_VAL, curr, random);
        return InterpolateHermite(F2RandomizeKey(GLT_VAL, curr, random),
            next_val - curr_val, curr->tangent2, next->tangent1,
            (float_t)curr->frame, (float_t)next->frame, frame);
    }
    
    float_t Curve::F2InterpolateLinear(GLT, Glitter::Curve::Key* curr,
        Glitter::Curve::Key* next, float_t frame, Random* random) {
        float_t df = (float_t)(next->frame - curr->frame);
        float_t t = (frame - (float_t)curr->frame) / (float_t)(next->frame - curr->frame);
        float_t curr_val = F2RandomizeKey(GLT_VAL, curr, random);
        float_t next_val = F2RandomizeKey(GLT_VAL, next, random);
        return curr_val * (1.0f - t) + next_val * t;
    }

    float_t Curve::F2Randomize(GLT, float_t value, Random* random) {
        float_t rand;

        if (!(flags & CURVE_RANDOM_RANGE))
            return value;

        rand = random->F2GetFloat(GLT_VAL, flags & CURVE_RANDOM_RANGE_NEGATE
            ? -random_range : 0.0f, random_range);

        if (flags & CURVE_RANDOM_RANGE_MULT)
            if (GLT_VAL != Glitter::FT)
                rand *= value * 0.01f;
            else
                rand *= value;
        return rand + value;
    }

    float_t Curve::F2RandomizeKey(GLT, Curve::Key* key, Random* random) {
        if (!(flags & CURVE_KEY_RANDOM_RANGE))
            return key->value;

        float_t rand = random->F2GetFloat(GLT_VAL, flags & CURVE_RANDOM_RANGE_NEGATE
            ? -key->random_range : 0.0f, key->random_range);
        return rand + key->value;
    }

#if defined(CRE_DEV)
    void Curve::FitKeysIntoCurve(GLT) {
        size_t count = keys.size();
        keys_rev = {};
        if (!(flags & CURVE_BAKED)) {
            keys_rev.insert(keys_rev.end(), keys.begin(), keys.end());
            return;
        }
        else if (count == 1) {
            keys.data()[0].frame = start_time;
            keys_rev.push_back(keys.data()[0]);
            return;
        }

        bool curve_baked_half = false;
        if (type == Glitter::F2 || (type == Glitter::X && !(flags & CURVE_BAKED_FULL)))
            curve_baked_half = true;

        const uint8_t step = curve_baked_half ? 2 : 1;

        end_time = start_time + (int32_t)((count - 1) * step);
        float_t* arr_a = force_malloc_s(float_t, count);
        float_t* arr_b = force_malloc_s(float_t, count);
        if (!arr_a || !arr_b) {
            free_def(arr_a);
            free_def(arr_b);
            if (!(flags & CURVE_BAKED)) {
                keys_rev.insert(keys_rev.end(), keys.begin(), keys.end());
                return;
            }
            else if (count == 1) {
                keys.data()[0].frame = start_time;
                keys_rev.push_back(keys.data()[0]);
                return;
            }
            else
                return;
        }

        Curve::Key* keys_data = keys.data();
        for (size_t i = 0; i < count; i++) {
            keys_data[i].frame = start_time + (int32_t)(i * step);
            arr_a[i] = keys_data[i].value;
            arr_b[i] = keys_data[i].random_range;
        }

        static const float_t curve_baked_reverse_bias[] = {
            0.00001f,
            0.0001f,
            0.001f,
        };

        static const float_t curve_baked_half_reverse_bias[] = {
            0.000001f,
            0.00001f,
            0.0001f,
        };

        const float_t* reverse_bias = curve_baked_half
            ? curve_baked_half_reverse_bias : curve_baked_reverse_bias;
        const size_t reverse_min_count = curve_baked_half ? 5 : 4;

        keys_rev.clear();
        float_t* a = arr_a;
        float_t* b = arr_b;
        size_t left_count = count;
        int32_t frame = start_time;
        int32_t prev_frame = start_time;
        float_t t2_old = 0.0f;
        while (left_count > 0) {
            if (left_count < reverse_min_count) {
                Curve::Key key;
                if (left_count == 1) {
                    keys_rev.push_back(Curve::Key(KEY_CONSTANT, frame, a[0], t2_old, 0.0f, b[0]));
                    t2_old = 0.0f;
                }
                else {
                    bool has_error = false;
                    bool has_error_lerp = false;
                    for (size_t j = 1; j < left_count; j++) {
                        float_t val = lerp_def(a[0], a[left_count - 1], (float_t)j / (float_t)left_count);
                        if (fabsf(val - a[0]) > reverse_bias[0]) {
                            has_error = true;
                            if (fabsf(val - a[j]) > reverse_bias[1]) {
                                has_error_lerp = true;
                                break;
                            }
                        }
                    }

                    t2_old = glitter_curve_add_key(has_error, has_error_lerp, true,
                        a, b, frame, step, left_count, 0.0f, 0.0f, t2_old, &keys_rev);
                }
                break;
            }

            float_t t1 = 0.0f;
            float_t t2 = 0.0f;
            float_t t1_prev = 0.0f;
            float_t t2_prev = 0.0f;
            bool has_prev_succeded = false;
            bool constant = false;
            bool prev_constant = false;
            bool has_error = false;
            bool has_error_lerp = false;
            bool has_error_hermite = false;
            bool has_prev_error = false;
            bool has_prev_error_lerp = false;

            int32_t c = 0;
            size_t i;
            for (i = reverse_min_count - 1; i < left_count; i++) {
                double_t t1_accum = 0.0;
                double_t t2_accum = 0.0;
                for (size_t j = 1; j < i - 1; j++) {
                    float_t _t1 = (float_t)(j * step) / (float_t)(i * step);
                    float_t _t2 = (float_t)((j + 1) * step) / (float_t)(i * step);
                    float_t t1_1 = _t1 - 1.0f;
                    float_t t2_1 = _t2 - 1.0f;

                    float_t t1_t2_1 = a[j] - a[0] - (_t1 * 2.0f - 3.0f) * (_t1 * _t1) * (a[0] - a[i]);
                    float_t t1_t2_2 = a[j + 1] - a[0] - (_t2 * 2.0f - 3.0f) * (_t2 * _t2) * (a[0] - a[i]);
                    t1_t2_1 /= t1_1 * _t1;
                    t1_t2_2 /= t2_1 * _t2;

                    float_t t1 = -t1_t2_1 * _t2 + t1_t2_2 * _t1;
                    float_t t2 = t1_t2_1 * t2_1 - t1_t2_2 * t1_1;

                    t1_accum += t1;
                    t2_accum += t2;
                }
                t1 = (float_t)(t1_accum / (double_t)(i - 2));
                t2 = (float_t)(t2_accum / (double_t)(i - 2));

                constant = true;
                has_error = false;
                has_error_lerp = false;
                has_error_hermite = false;
                for (size_t j = 1; j < i; j++) {
                    float_t val = InterpolateHermite(a[0], a[i] - a[0], t1, t2, 0.0f,
                        (float_t)(i * step), (float_t)(j * step));
                    float_t val_lerp = lerp_def(a[0], a[i], (float_t)j / (float_t)i);
                    if (fabsf(val - a[0]) > reverse_bias[0]) {
                        has_error = true;
                        constant = false;
                        if (fabsf(val_lerp - a[j]) > reverse_bias[1]) {
                            has_error_lerp = true;
                            if (fabsf(val - a[j]) > reverse_bias[2]) {
                                has_error_hermite = true;
                                break;
                            }
                        }
                    }
                }

                if (fabsf(t1) > 0.5f || fabsf(t2) > 0.5f)
                    has_error_hermite = true;

                if (!has_error_hermite) {
                    t1_prev = t1;
                    t2_prev = t2;
                    prev_constant = constant;
                    has_prev_succeded = true;
                    has_prev_error = has_error;
                    has_prev_error_lerp = has_error_lerp;
                    if (i < left_count)
                        continue;
                }

                if (has_prev_succeded) {
                    if (prev_constant && ((i < left_count - 1 && a[i] == a[i + 1]) || (frame + i == end_time)))
                        c = (int32_t)i;
                    else
                        c = (int32_t)(i - 1);
                    t1 = t1_prev;
                    t2 = t2_prev;
                    has_error = has_prev_error;
                    has_error_lerp = has_prev_error_lerp;
                    has_error_hermite = false;
                }
                else if (has_error_hermite)
                    c = 1;
                else
                    c = (int32_t)i;                

                t2_old = glitter_curve_add_key(has_error, has_error_lerp, has_error_hermite,
                    a, b, frame, step, c, t1, t2, t2_old, &keys_rev);
                prev_constant = false;
                has_prev_succeded = false;
                break;
            }

            if (has_prev_succeded) {
                t2_old = glitter_curve_add_key(has_prev_error, has_prev_error_lerp, false,
                    a, b, frame, step, i, t1_prev, t2_prev, t2_old, &keys_rev);
                c = (int32_t)i;
            }
            prev_frame = frame;
            frame += c * step;
            a += c;
            b += c;
            left_count -= c;
        }

        keys_rev.push_back(Curve::Key(KEY_CONSTANT, (int32_t)(start_time + (count - 1) * step),
            arr_a[count - 1], t2_old, 0.0f, arr_b[count - 1]));
        free_def(arr_a);
        free_def(arr_b);

        Recalculate(GLT_VAL);
    }

    void Curve::Recalculate(GLT) {
        keys.clear();
        if (keys_rev.size() == 1)
            keys.push_back(keys_rev.data()[0]);
        else if (keys_rev.size() < 1)
            return;
        else if (!(flags & CURVE_BAKED)) {
            keys.insert(keys.end(), keys_rev.begin(), keys_rev.end());
            return;
        }

        bool curve_baked_half = false;
        if (GLT_VAL == Glitter::F2 || (GLT_VAL == Glitter::X && !(flags & CURVE_BAKED_FULL)))
            curve_baked_half = true;

        ssize_t keys_count = keys_rev.size();
        size_t count = (size_t)end_time - start_time;
        if (curve_baked_half)
            count /= 2;
        count++;

        const uint8_t step = curve_baked_half ? 2 : 1;

        Curve::Key first_key = keys_rev.data()[0];
        Curve::Key last_key = keys_rev.data()[keys_count - 1];

        keys.reserve(count);
        for (size_t i = 0; i < count; i++) {
            int32_t frame = start_time + (int32_t)(i * step);

            if (frame <= first_key.frame) {
                keys.push_back(Curve::Key(KEY_CONSTANT, frame, first_key.value, first_key.random_range));
                continue;
            }
            else if (frame >= last_key.frame) {
                keys.push_back(Curve::Key(KEY_CONSTANT, frame, last_key.value, last_key.random_range));
                continue;
            }

            Curve::Key* key = keys_rev.data();

            size_t length = keys_count;
            size_t temp;
            while (length > 0)
                if (key[temp = length / 2].frame > frame)
                    length = temp;
                else {
                    key += temp + 1;
                    length -= temp + 1;
                }

            Curve::Key* curr_key = key - 1;
            Curve::Key* next_key = key;

            float_t val;
            float_t rand_range;
            if (curr_key->type == KEY_CONSTANT) {
                val = curr_key->value;
                rand_range = curr_key->random_range;
            }
            else if (curr_key->type == KEY_HERMITE) {
                val = InterpolateHermite(curr_key->value, next_key->value - curr_key->value,
                    curr_key->tangent2, next_key->tangent1,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
                rand_range = InterpolateHermite(curr_key->random_range,
                    next_key->random_range - curr_key->random_range, 0.0f, 0.0f,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
            }
            else {
                val = InterpolateLinear(curr_key->value, next_key->value,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
                rand_range = InterpolateLinear(curr_key->random_range, next_key->random_range,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
            }

            keys.push_back(Curve::Key(KEY_CONSTANT, frame, val, 0.0f, 0.0f, rand_range));
        }
    }
#endif

    bool Curve::XGetValue(float_t frame,
        float_t* value, int32_t random_value, Random* random) {
        size_t keys_count = keys.size();
        if (!keys_count)
            return false;

        int32_t random_val = random->GetValue();
        random->SetValue(random_value);
        bool negate = flags & CURVE_NEGATE && random->XGetInt(0, 0xFFFF) > 0x7FFF;
        if (flags & CURVE_STEP)
            random->SetValue(random_val + 1);

        float_t start_time;
        float_t end_time;
        float_t _value;
        if (keys_count == 1) {
            Curve::Key* curr_key = &keys.data()[keys_count - 1];
            _value = XRandomizeKey(curr_key, random);
            goto End;
        }

        start_time = (float_t)this->start_time;
        end_time = (float_t)this->end_time;
        if (repeat && (start_time > frame || frame >= end_time)) {
            float_t t = (frame - start_time) / (end_time - start_time);
            if (t > 0.0f)
                t = (float_t)(int32_t)t;
            else if (t < 0.0f)
                t = (float_t)(int32_t)t - 1.0f;
            frame -= t * (end_time - start_time);
        }

        if (end_time <= frame) {
            Curve::Key* key = &keys.data()[keys_count - 1];
            _value = XRandomizeKey(key, random);
        }
        else if (start_time > frame) {
            Curve::Key* key = &keys.data()[0];
            _value = XRandomizeKey(key, random);
        }
        else if (flags & CURVE_BAKED) {
            size_t key_index;
            if (frame >= end_time)
                key_index = keys_count - 1;
            else if (frame > start_time) {
                key_index = (size_t)frame - this->start_time;
                if (!(flags & CURVE_BAKED_FULL))
                    key_index /= 2;

                if (key_index >= keys_count)
                    key_index = keys_count - 1;
            }
            else
                key_index = 0;

            Curve::Key* key = &keys.data()[key_index];
            _value = XRandomizeKey(key, random);
        }
        else {
            size_t curr_key_index = 0;
            size_t next_key_index = 0;
            if (keys_count > 3)
                Curve::GetKeyIndices(&keys, frame, &curr_key_index, &next_key_index);
            else if (keys_count == 3 && frame >= keys.data()[1].frame) {
                curr_key_index = 1;
                next_key_index = 2;
            }
            else {
                curr_key_index = 0;
                next_key_index = 1;
            }
            Curve::Key* curr_key = &keys.data()[curr_key_index];
            Curve::Key* next_key = &keys.data()[next_key_index];
            _value = XInterpolate(frame, curr_key, next_key, curr_key->type, random);
        }

    End:
        _value = XRandomize(_value, random);
        *value = negate ? -_value : _value;
        random->SetValue(random_val + 1);
        return true;
    }

    float_t Curve::XInterpolate(float_t frame, Curve::Key* curr,
        Curve::Key* next, KeyType key_type, Random* random) {
        if (key_type == KEY_CONSTANT)
            return XRandomizeKey(curr, random);
        else if (key_type == KEY_HERMITE)
            return XInterpolateHermite(curr, next, frame, random);
        else
            return XInterpolateLinear(curr, next, frame, random);
    }

    float_t Curve::XInterpolateHermite(Glitter::Curve::Key* curr,
        Glitter::Curve::Key* next, float_t frame, Random* random) {
        float_t next_val = XRandomizeKey(next, random);
        float_t curr_val = XRandomizeKey(curr, random);
        return InterpolateHermite(XRandomizeKey(curr, random),
            next_val - curr_val, curr->tangent2, next->tangent1,
            (float_t)curr->frame, (float_t)next->frame, frame);
    }

    float_t Curve::XInterpolateLinear(Glitter::Curve::Key* curr,
        Glitter::Curve::Key* next, float_t frame, Random* random) {
        float_t df = (float_t)(next->frame - curr->frame);
        float_t t = (frame - (float_t)curr->frame) / (float_t)(next->frame - curr->frame);
        float_t curr_val = XRandomizeKey(curr, random);
        float_t next_val = XRandomizeKey(next, random);
        return curr_val * (1.0f - t) + next_val * t;
    }

    float_t Curve::XRandomize(float_t value, Random* random) {
        if (!(flags & CURVE_RANDOM_RANGE))
            return value;

        float_t rand = random->XGetFloat(flags & CURVE_RANDOM_RANGE_NEGATE
            ? -random_range : 0.0f, random_range);

        if (flags & CURVE_RANDOM_RANGE_MULT)
            rand *= value * 0.01f;
        return rand + value;
    }

    float_t Curve::XRandomizeKey(Curve::Key* key, Random* random) {
        if (!(flags & CURVE_KEY_RANDOM_RANGE))
            return key->value;

        float_t rand = random->XGetFloat(flags & CURVE_RANDOM_RANGE_NEGATE
            ? -key->random_range : 0.0f, key->random_range);
        return rand + key->value;
    }

    void Curve::GetKeyIndices(std::vector<Curve::Key>* keys,
        float_t frame, size_t* curr, size_t* next) {
        size_t count = keys->size();
        if (count <= 1) {
            *curr = 0;
            *next = 0;
            return;
        }

        size_t first_key = 0;
        Curve::Key* key = keys->data();
        size_t last_key = count - 1;
        size_t temp = last_key / 2;
        while (first_key <= last_key) {
            temp = (last_key + first_key) / 2;
            if (frame <= key[(temp + 1) % count].frame) {
                if (frame >= key[temp].frame)
                    goto NextKey;
                last_key = temp - 1;
            }
            else
                first_key = temp + 1;
        }

        if (frame > key[temp].frame) {
        NextKey:
            *curr = temp;
            *next = temp + 1;
        }
        else {
            *curr = temp - 1;
            *next = temp;
        }
        *next %= count;
    }

#if defined(CRE_DEV)
    static float_t glitter_curve_add_key(bool has_error,
        bool has_error_lerp, bool has_error_hermite, float_t* a, float_t* b, int32_t frame,
        const uint8_t step, size_t i, float_t t1, float_t t2, float_t t2_old,
        std::vector<Curve::Key>* keys_rev) {
        if (has_error && has_error_lerp && has_error_hermite) {
            keys_rev->reserve(i);
            keys_rev->push_back(Curve::Key(KEY_CONSTANT, frame, a[0], t2_old, 0.0f, b[0]));
            for (size_t j = 1; j < i; j++)
                keys_rev->push_back(Curve::Key(KEY_CONSTANT, (int32_t)(frame + j * step), a[j], b[j]));
        }
        else if (has_error && has_error_lerp) {
            keys_rev->push_back(Curve::Key(KEY_HERMITE, frame, a[0], t2_old, t1, b[0]));
            return t2;
        }
        else if (has_error || (i > 1 && b[0] != b[1]))
            keys_rev->push_back(Curve::Key(KEY_LINEAR, frame, a[0], b[0]));
        else
            keys_rev->push_back(Curve::Key(KEY_CONSTANT, frame, a[0], b[0]));
        return 0.0f;
    }
#endif
}
