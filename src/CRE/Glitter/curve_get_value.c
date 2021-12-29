/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "curve.h"
#include "random.h"

static void glitter_curve_get_key_indexes(vector_glitter_curve_key* keys,
    float_t frame, size_t* curr, size_t* next);
static float_t glitter_curve_interpolate(GLT, glitter_curve* c, float_t frame,
    glitter_curve_key* curr, glitter_curve_key* next, glitter_key_type key_type, glitter_random* random);
static float_t glitter_curve_randomize(GLT,
    glitter_curve* c, float_t value, glitter_random* random);
static float_t glitter_curve_randomize_key(GLT,
    glitter_curve* c, glitter_curve_key* key, glitter_random* random);

bool glitter_curve_get_value(GLT, glitter_curve* c,
    float_t frame, float_t* value, int32_t random_value, glitter_random* random) {
    size_t keys_count;
    int32_t random_val;
    bool negate;
    float_t _value;

    keys_count = c->keys.end - c->keys.begin;
    if (!keys_count)
        return false;

    random_val = glitter_random_get_value(random);
    glitter_random_set_value(random, random_value);
    negate = c->flags & GLITTER_CURVE_NEGATE
        && glitter_random_get_int_min_max(GLT_VAL, random, 0, 0xFFFF) > 0x7FFF;
    if (c->flags & GLITTER_CURVE_STEP)
        glitter_random_set_value(random, random_val + 1);

    if (keys_count == 1) {
        glitter_curve_key* key = &c->keys.begin[keys_count - 1];
        _value = glitter_curve_randomize_key(GLT_VAL, c, key, random);
        goto End;
    }

    float_t start_time = (float_t)c->start_time;
    float_t end_time = (float_t)c->end_time;
    if (c->repeat && (start_time > frame || frame >= end_time)) {
        float_t t = (frame - start_time) / (end_time - start_time);
        t = t <= 0.0f ? (float_t)(int32_t)t - 1.0f : (float_t)(int32_t)t;
        frame -= t * (end_time - start_time);
    }

    if (end_time <= frame) {
        glitter_curve_key* key = &c->keys.begin[keys_count - 1];
        _value = glitter_curve_randomize_key(GLT_VAL, c, key, random);
    }
    else if (start_time > frame) {
        glitter_curve_key* key = &c->keys.begin[0];
        _value = glitter_curve_randomize_key(GLT_VAL, c, key, random);
    }
    else if (c->flags & GLITTER_CURVE_BAKED) {
        size_t key_index;
        if (frame >= end_time)
            key_index = keys_count - 1;
        else if (frame > start_time) {
            key_index = (size_t)frame - c->start_time;
            if (GLT_VAL == GLITTER_F2)
                key_index /= 2;

            if (key_index >= keys_count)
                key_index = keys_count - 1;
        }
        else
            key_index = 0;

        glitter_curve_key* key = &c->keys.begin[key_index];
        _value = glitter_curve_randomize_key(GLT_VAL, c, key, random);
    }
    else {
        size_t curr_key_index = 0;
        size_t next_key_index = 0;
        if (keys_count > 3)
            glitter_curve_get_key_indexes(&c->keys, frame, &curr_key_index, &next_key_index);
        else if (keys_count == 3 && frame >= c->keys.begin[1].frame) {
            curr_key_index = 1;
            next_key_index = 2;
        }
        else {
            curr_key_index = 0;
            next_key_index = 1;
        }
        glitter_curve_key* curr_key = &c->keys.begin[curr_key_index];
        glitter_curve_key* next_key = &c->keys.begin[next_key_index];
        _value = glitter_curve_interpolate(GLT_VAL, c, frame, curr_key, next_key, curr_key->type, random);
    }

End:
    _value = glitter_curve_randomize(GLT_VAL, c, _value, random);
    *value = negate ? -_value : _value;
    glitter_random_set_value(random, random_val + 1);
    return true;
}

static void glitter_curve_get_key_indexes(vector_glitter_curve_key* keys,
    float_t frame, size_t* curr, size_t* next) {
    size_t v4;
    glitter_curve_key* v6;
    size_t count;
    size_t v9;
    size_t v10;

    count = keys->end - keys->begin;
    if (count <= 1) {
        *curr = 0;
        *next = 0;
        return;
    }

    v4 = 0;
    v6 = keys->begin;
    v9 = count - 1;
    v10 = v9 / 2;
    while (v4 <= v9) {
        v10 = (v9 + v4) / 2;
        if (frame <= v6[(v10 + 1) % count].frame) {
            if (frame >= v6[v10].frame)
                goto LABEL_12;
            v9 = v10 - 1;
        }
        else
            v4 = v10 + 1;
    }

    if (frame > v6[v10].frame) {
    LABEL_12:
        *curr = v10;
        *next = v10 + 1;
    }
    else {
        *curr = v10 - 1;
        *next = v10;
    }
    *next %= count;
}

static float_t glitter_curve_interpolate(GLT, glitter_curve* c, float_t frame,
    glitter_curve_key* curr, glitter_curve_key* next, glitter_key_type key_type, glitter_random* random) {
    float_t next_val;
    float_t curr_val;
    float_t val;

    if (key_type == GLITTER_KEY_CONSTANT)
        return glitter_curve_randomize_key(GLT_VAL, c, curr, random);

    float_t df = frame - (float_t)curr->frame;
    float_t t = df / (float_t)(next->frame - curr->frame);
    if (key_type == GLITTER_KEY_HERMITE) {
        float_t t_1 = t - 1.0f;
        next_val = glitter_curve_randomize_key(GLT_VAL, c, next, random);
        curr_val = glitter_curve_randomize_key(GLT_VAL, c, curr, random);
        val = glitter_curve_randomize_key(GLT_VAL, c, curr, random);
        val += t * t * (3.0f - 2.0f * t) * (next_val - curr_val)
            + (t_1 * curr->tangent2 + t * next->tangent1) * df * t_1;
    }
    else {
        curr_val = glitter_curve_randomize_key(GLT_VAL, c, curr, random);
        next_val = glitter_curve_randomize_key(GLT_VAL, c, next, random);
        val = (1.0f - t) * curr_val + next_val * t;
    }
    return val;
}

static float_t glitter_curve_randomize(GLT,
    glitter_curve* c, float_t value, glitter_random* random) {
    float_t rand;

    if (~c->flags & GLITTER_CURVE_RANDOM_RANGE)
        return value;

    rand = glitter_random_get_float_min_max(GLT_VAL, random,
        c->flags & GLITTER_CURVE_RANDOM_RANGE_NEGATE ? -c->random_range : 0.0f, c->random_range);

    if (c->flags & GLITTER_CURVE_RANDOM_RANGE_MULT)
        if (GLT_VAL != GLITTER_FT)
            rand *= value * 0.01f;
        else
            rand *= value;
    return rand + value;
}

static float_t glitter_curve_randomize_key(GLT,
    glitter_curve* c, glitter_curve_key* key, glitter_random* random) {
    float_t rand;

    if (~c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
        return key->value;

    rand = glitter_random_get_float_min_max(GLT_VAL, random,
        c->flags & GLITTER_CURVE_RANDOM_RANGE_NEGATE ? -key->random_range : 0.0f, key->random_range);
    return rand + key->value;
}
