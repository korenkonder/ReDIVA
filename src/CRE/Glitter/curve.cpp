/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../KKdLib/interpolation.h"

namespace Glitter {
    Curve::Key::Key() : type(), frame(), value(), tangent1(), tangent2(), random_range() {

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
            t = t <= 0.0f ? (float_t)(int32_t)t - 1.0f : (float_t)(int32_t)t;
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

        float_t df = frame - (float_t)curr->frame;
        float_t t = df / (float_t)(next->frame - curr->frame);
        float_t val;
        if (key_type == KEY_HERMITE) {
            float_t t_1 = t - 1.0f;
            float_t next_val = F2RandomizeKey(GLT_VAL, next, random);
            float_t curr_val = F2RandomizeKey(GLT_VAL, curr, random);
            val = F2RandomizeKey(GLT_VAL, curr, random);
            val += t * t * (3.0f - 2.0f * t) * (next_val - curr_val)
                + (t_1 * curr->tangent2 + t * next->tangent1) * df * t_1;
        }
        else {
            float_t curr_val = F2RandomizeKey(GLT_VAL, curr, random);
            float_t next_val = F2RandomizeKey(GLT_VAL, next, random);
            val = (1.0f - t) * curr_val + next_val * t;
        }
        return val;
    }

    float_t Curve::F2Randomize(GLT, float_t value, Random* random) {
        float_t rand;

        if (~flags & CURVE_RANDOM_RANGE)
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
        if (~flags & CURVE_KEY_RANDOM_RANGE)
            return key->value;

        float_t rand = random->F2GetFloat(GLT_VAL, flags & CURVE_RANDOM_RANGE_NEGATE
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
    void Curve::Recalculate(GLT) {
        keys.clear();
        if (keys_rev.size() == 1)
            keys.push_back(keys_rev.data()[0]);
        else if (keys_rev.size() < 1)
            return;
        else if (~flags & CURVE_BAKED) {
            keys.insert(keys.end(), keys_rev.begin(), keys_rev.end());
            return;
        }

        bool curve_baked_half = false;
        if (GLT_VAL == Glitter::F2 || (GLT_VAL == Glitter::X && ~flags & CURVE_BAKED_FULL))
            curve_baked_half = true;

        ssize_t keys_count = keys_rev.size();
        size_t count = (size_t)end_time - start_time;
        if (curve_baked_half)
            count /= 2;
        count++;

        const uint8_t step = curve_baked_half ? 2 : 1;

        Curve::Key first_key = keys_rev.data()[0];
        Curve::Key last_key = keys_rev.data()[keys_count - 1];

        Curve::Key key;
        keys.reserve(count);
        for (size_t i = 0; i < count; i++) {
            int32_t frame = start_time + (int32_t)(i * step);

            if (frame <= first_key.frame) {
                key.type = KEY_CONSTANT;
                key.frame = frame;
                key.value = first_key.value;
                key.random_range = first_key.random_range;
                keys.push_back(key);
                continue;
            }
            else if (frame >= last_key.frame) {
                key.type = KEY_CONSTANT;
                key.frame = frame;
                key.value = last_key.value;
                key.random_range = last_key.random_range;
                keys.push_back(key);
                continue;
            }

            size_t key_idx = 0;
            size_t length = keys_count;
            size_t temp;
            while (length > 0)
                if (frame > keys_rev.data()[key_idx + (temp = length >> 1)].frame) {
                    key_idx += temp + 1;
                    length -= temp + 1;
                }
                else
                    length = temp;

            Curve::Key* curr_key = &keys_rev.data()[key_idx - 1];
            Curve::Key* next_key = &keys_rev.data()[key_idx];

            float_t val;
            float_t rand_range;
            if (curr_key->type == KEY_CONSTANT) {
                val = curr_key->value;
                rand_range = curr_key->random_range;
            }
            else if (curr_key->type == KEY_HERMITE) {
                val = interpolate_chs_value(curr_key->value, next_key->value,
                    curr_key->tangent2, next_key->tangent1,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
                rand_range = interpolate_chs_value(curr_key->random_range, next_key->random_range,
                    0.0f, 0.0f,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
                float_t val1 = interpolate_chs_value(curr_key->value + curr_key->random_range,
                    next_key->value + next_key->random_range,
                    curr_key->tangent2, next_key->tangent1,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
            }
            else {
                val = interpolate_linear_value(curr_key->value, next_key->value,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
                rand_range = interpolate_linear_value(curr_key->random_range, next_key->random_range,
                    (float_t)curr_key->frame, (float_t)next_key->frame, (float_t)frame);
            }

            key.type = KEY_CONSTANT;
            key.frame = frame;
            key.value = val;
            key.random_range = rand_range;
            keys.push_back(key);
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
            t = t <= 0.0f ? (float_t)(int32_t)t - 1.0f : (float_t)(int32_t)t;
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
                if (~flags & CURVE_BAKED_FULL)
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

    float_t Curve::XInterpolate(float_t frame,
        Curve::Key* curr, Curve::Key* next, KeyType key_type, Random* random) {
        if (key_type == KEY_CONSTANT)
            return XRandomizeKey(curr, random);

        float_t df = frame - (float_t)curr->frame;
        float_t t = df / (float_t)(next->frame - curr->frame);
        float_t val;
        if (key_type == KEY_HERMITE) {
            float_t t_1 = t - 1.0f;
            float_t next_val = XRandomizeKey(next, random);
            float_t curr_val = XRandomizeKey(curr, random);
            val = XRandomizeKey(curr, random);
            val += t * t * (3.0f - 2.0f * t) * (next_val - curr_val)
                + (t_1 * curr->tangent2 + t * next->tangent1) * df * t_1;
        }
        else {
            float_t curr_val = XRandomizeKey(curr, random);
            float_t next_val = XRandomizeKey(next, random);
            val = (1.0f - t) * curr_val + next_val;
        }
        return val;
    }

    float_t Curve::XRandomize(float_t value, Random* random) {
        if (~flags & CURVE_RANDOM_RANGE)
            return value;

        float_t rand = random->XGetFloat(flags & CURVE_RANDOM_RANGE_NEGATE
            ? -random_range : 0.0f, random_range);

        if (flags & CURVE_RANDOM_RANGE_MULT)
            rand *= value * 0.01f;
        return rand + value;
    }

    float_t Curve::XRandomizeKey(Curve::Key* key, Random* random) {
        if (~flags & CURVE_KEY_RANDOM_RANGE)
            return key->value;

        float_t rand = random->XGetFloat(flags & CURVE_RANDOM_RANGE_NEGATE
            ? -key->random_range : 0.0f, key->random_range);
        return rand + key->value;
    }
}
