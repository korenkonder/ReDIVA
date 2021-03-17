/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "curve.h"
#include "parse_file.h"
#include "random.h"

extern glitter_particle_manager* gpm;

glitter_curve* FASTCALL glitter_curve_init() {
    glitter_curve* c = force_malloc(sizeof(glitter_curve));
    vector_glitter_curve_key_expand(&c->keys, 0x80);
    return c;
}

void FASTCALL glitter_curve_dispose(glitter_curve* c) {
    vector_glitter_curve_key_clear(&c->keys);
    vector_glitter_curve_key_dispose(&c->keys);
    free(c);
}

float_t FASTCALL glitter_curve_apply_flag(glitter_curve* curve, float_t value) {
    float_t max_rand;

    if (curve->flags & GLITTER_CURVE_FLAG_RANDOMIZE) {
        max_rand = curve->max_rand;
        max_rand = glitter_random_get_float_clamp(curve->flags & GLITTER_CURVE_FLAG_NEGATIVE_RANDOM
            ? -max_rand : 0.0f, max_rand);

        if (curve->flags & GLITTER_CURVE_FLAG_MULT)
            max_rand *= value;
        return max_rand + value;
    }
    else
        return value;
}

void FASTCALL glitter_curve_get_key_indexes(vector_glitter_curve_key* a1, float_t a2, size_t* prev, size_t* curr) {
    size_t v4; // er10
    glitter_curve_key* v6; // rdi
    size_t count; // rbx
    size_t v9; // er9
    size_t v10; // er8

    *prev = 0;
    *curr = 0;
    count = a1->end - a1->begin;
    if (count <= 1)
        return;

    v4 = 0;
    v6 = a1->begin;
    v9 = count - 1;
    v10 = v9 / 2;
    while (v4 <= v9) {
        v10 = (v9 + v4) / 2;
        if (a2 <= v6[(v10 + 1) % count].frame) {
            if (a2 >= v6[v10].frame)
                goto LABEL_12;
            v9 = v10 - 1;
        }
        else
            v4 = v10 + 1;
    }

    if (a2 > v6[v10].frame) {
    LABEL_12:
        *prev = v10;
        *curr = v10 + 1;
    }
    else {
        *prev = v10 - 1;
        *curr = v10;
    }
    *curr %= count;
}

bool FASTCALL glitter_curve_get_value(glitter_curve* curv, float_t frame, float_t* value, int32_t random) {
    size_t keys_count;
    int32_t _random;
    bool negate;
    float_t _value;
    float_t t;
    size_t key;
    glitter_curve_key* prev_key;
    glitter_curve_key* curr_key;
    size_t curr_key_index;
    size_t prev_key_index;

    keys_count = curv->keys.end - curv->keys.begin;
    if (!keys_count)
        return false;

    _random = glitter_random_get();
    glitter_random_set(random);
    negate = curv->flags & GLITTER_CURVE_FLAG_NEGATE && glitter_random_get_int_clamp(0, 0xFFFF) > 0x7FFF;
    if (curv->flags & GLITTER_CURVE_FLAG_STEP)
        glitter_random_set(_random + 1);

    if (keys_count == 1) {
        _value = glitter_curve_randomize(curv, curv->keys.begin);
        _value = glitter_curve_apply_flag(curv, _value);
        *value = negate ? -_value : _value;
        glitter_random_set(_random + 1);
        return true;
    }

    if (curv->repeat && (curv->start_time > frame || frame >= curv->end_time)) {
        t = (frame - curv->start_time) / (curv->end_time - curv->start_time);
        t = t <= 0.0f ? (float_t)(int32_t)t - 1.0f : (float_t)(int32_t)t;
        frame -= t * (curv->end_time - curv->start_time);
    }

    if (curv->end_time <= frame)
        _value = glitter_curve_randomize(curv, &curv->keys.begin[keys_count - 1]);
    else if (curv->start_time > frame)
        _value = glitter_curve_randomize(curv, curv->keys.begin);
    else if (curv->flags & GLITTER_CURVE_FLAG_PLAIN) {
        key = 0;
        if (frame >= curv->end_time)
            key = keys_count - 1;
        else if (frame > curv->start_time) {
            key = (int64_t)((double_t)frame - curv->start_time);
            if (gpm->f2)
                key >>= 1;

            if (key >= keys_count)
                key = keys_count - 1;
        }
        _value = glitter_curve_randomize(curv, &curv->keys.begin[key]);
    }
    else {
        if (keys_count > 3)
            glitter_curve_get_key_indexes(&curv->keys, frame, &prev_key_index, &curr_key_index);
        else if (keys_count == 3 && frame >= curv->keys.begin[1].frame) {
            prev_key_index = 1;
            curr_key_index = 2;
        }
        else {
            prev_key_index = 0;
            curr_key_index = 1;
        }
        prev_key = &curv->keys.begin[prev_key_index];
        curr_key = &curv->keys.begin[curr_key_index];
        _value = glitter_curve_interpolate(curv, frame, prev_key, curr_key, prev_key->type);
    }
    _value = glitter_curve_apply_flag(curv, _value);
    *value = negate ? -_value : _value;
    glitter_random_set(_random + 1);
    return true;
}

float_t FASTCALL glitter_curve_interpolate(glitter_curve* a1, float_t frame,
    glitter_curve_key* a3, glitter_curve_key* a4, glitter_key_type type) {
    float_t v7;
    float_t v9;
    float_t v10;
    float_t t;
    float_t v15;
    float_t t_2;

    if (type == GLITTER_KEY_CONSTANT)
        return glitter_curve_randomize(a1, a3);

    v7 = a4->frame - a3->frame;
    t = (frame - a3->frame) / v7;
    if (type == GLITTER_KEY_HERMITE) {
        t_2 = (1.0f - t) * (1.0f - t);
        v9 = glitter_curve_randomize(a1, a4);
        v10 = glitter_curve_randomize(a1, a3);
        v15 = t_2 * (v9 - v10) * (1.0f + 2.0f * t) + ((t_2 * a3->tangent2 + t * (t - 1.0f) * a4->tangent1) * v7) * t;
        return glitter_curve_randomize(a1, a3) + v15;
    }
    else {
        v10 = glitter_curve_randomize(a1, a3);
        v9 = glitter_curve_randomize(a1, a4);
        return v9 * t + v10 * (1.0f - t);
    }
}

void FASTCALL glitter_curve_parse_file(glitter_file_reader* fr,
    f2_header* header, vector_ptr_glitter_curve* vec) {
    uint32_t* data; // rbx
    glitter_curve* curve; // rax
    uint32_t keys_count; // er14

    if (!header)
        return;

    data = glitter_parse_file_get_data_ptr(header);
    if (!data)
        return;

    curve = glitter_curve_init();
    curve->type = data[0];
    curve->repeat = data[1] != 0;
    curve->flags = data[2];
    if (curve->type <= GLITTER_CURVE_TRANSLATION_Z || curve->type >= GLITTER_CURVE_SCALE_X
        || header->version == 1)
        curve->max_rand = ((float_t*)data)[3];
    else
        curve->max_rand = 0.0f;
    keys_count = ((uint16_t*)data)[8];
    curve->start_time = (float_t)((uint16_t*)data)[9];
    curve->end_time = (float_t)((uint16_t*)data)[10];
    if (keys_count) {
        header = glitter_parse_file_get_sub_struct_ptr(header);
        if (header) {
            while (glitter_parse_file_reverse_signature_endianess(header) == 'ENRS') {
                header = glitter_parse_file_check_for_end_of_container(header);
                if (!header) {
                    vector_ptr_glitter_curve_append_element(vec, &curve);
                    return;
                }
            }

            if (header->data_size) {
                if (glitter_parse_file_reverse_signature_endianess(header) == 'KEYS') {
                    glitter_curve_unpack_file(fr, glitter_parse_file_get_data_ptr(header),
                        vec, curve, keys_count, header->version);
                }
            }
        }
    }
    vector_ptr_glitter_curve_append_element(vec, &curve);
}

float_t FASTCALL glitter_curve_randomize(glitter_curve* a1, glitter_curve_key* a2) {
    float_t min_value;
    float_t value;

    if (~a1->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
        return a2->value;

    min_value = a1->flags & GLITTER_CURVE_FLAG_NEGATIVE_RANDOM ? -a2->random_range : 0.0f;
    value = glitter_random_get_float_clamp(min_value, a2->random_range);

    if (a1->flags & GLITTER_CURVE_FLAG_MULT)
        return value * a2->value + a2->value;
    else
        return value + a2->value;
}

void FASTCALL glitter_curve_unpack_file(glitter_file_reader* fr, float_t* data,
    vector_ptr_glitter_curve* vec, glitter_curve* curve, uint32_t count, uint32_t keys_version) {
    int64_t v11;
    glitter_curve_key key;
    float_t scale;

    if (!data || !curve)
        return;

    data = data;
    key.type = GLITTER_KEY_CONSTANT;
    key.frame = 0.0f;
    key.value = 0.0f;
    key.tangent1 = 0.0f;
    key.tangent2 = 0.0f;
    key.random_range = 0.0f;

    scale = 1.0f;
    if (keys_version == 0 && curve->type >= GLITTER_CURVE_ROTATION_X && curve->type <= GLITTER_CURVE_ROTATION_Z)
        scale = (float_t)M_PI / 180.0f;
    else if (keys_version == 0 && curve->type >= GLITTER_CURVE_COLOR_R && curve->type <= GLITTER_CURVE_COLOR_A)
        scale = 1.0f / 255.0f;

    if (curve->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
        for (v11 = count; v11; v11--) {
            key.type = *(uint16_t*)data;
            key.frame = (float_t)((int16_t*)data)[1];
            if (key.type == GLITTER_KEY_HERMITE) {
                key.tangent1 = data[1] * scale;
                key.tangent2 = data[2] * scale;
                key.random_range = data[3] * scale;
                key.value = data[4] * scale;
                vector_glitter_curve_key_append_element(&curve->keys, &key);
                data += 5;
            }
            else {
                key.random_range = data[1] * scale;
                key.value = data[2] * scale;
                vector_glitter_curve_key_append_element(&curve->keys, &key);
                data += 3;
            }
        }
    else
        for (v11 = count; v11; v11--) {
            key.type = *(uint16_t*)data;
            key.frame = (float_t)((int16_t*)data)[1];
            if (key.type == GLITTER_KEY_HERMITE) {
                key.tangent1 = data[1] * scale;
                key.tangent2 = data[2] * scale;
                key.value = data[3] * scale;
                vector_glitter_curve_key_append_element(&curve->keys, &key);
                data += 4;
            }
            else {
                key.value = data[1] * scale;
                vector_glitter_curve_key_append_element(&curve->keys, &key);
                data += 2;
            }
        }
}
