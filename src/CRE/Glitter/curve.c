/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "curve.h"
#include "random.h"
#include "../../KKdLib/io_stream.h"

extern glitter_particle_manager* gpm;

static float_t FASTCALL glitter_curve_apply_max_rand(glitter_curve* c, float_t value);
static void FASTCALL glitter_curve_get_key_indexes(vector_glitter_curve_key* keys,
    float_t frame, size_t* curr, size_t* next);
static float_t FASTCALL glitter_curve_interpolate(glitter_curve* c, float_t frame,
    glitter_curve_key* curr, glitter_curve_key* next, glitter_key_type type);
static void FASTCALL glitter_curve_key_pack_file(f2_struct* st, glitter_curve* c, bool use_big_endian);
static void FASTCALL glitter_curve_key_unpack_file(f2_struct* st, glitter_curve* c, uint32_t count);
static void FASTCALL glitter_curve_pack_file(f2_struct* st,
    glitter_curve* c, bool use_big_endian, size_t keys_count);
static float_t FASTCALL glitter_curve_randomize(glitter_curve* c, glitter_curve_key* key);
static void FASTCALL glitter_curve_unpack_file(uint32_t* data,
    glitter_curve* c, bool use_big_endian, uint32_t* keys_count);

glitter_curve* FASTCALL glitter_curve_init() {
    glitter_curve* c = force_malloc(sizeof(glitter_curve));
    vector_glitter_curve_key_resize(&c->keys, 0x80);
    return c;
}

void FASTCALL glitter_curve_parse_file(f2_struct* st, vector_ptr_glitter_curve* vec) {
    f2_struct* i;
    glitter_curve* c;
    uint32_t keys_count;

    if (!st || !st->header.data_size)
        return;

    c = glitter_curve_init();
    c->version = st->header.version;
    glitter_curve_unpack_file(st->data, c, st->header.use_big_endian, &keys_count);
    if (keys_count)
        for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
            if (!i->header.data_size)
                continue;

            if (i->header.signature == 0x5359454B) {
                glitter_curve_key_unpack_file(i, c, keys_count);
                break;
            }
        }
    vector_ptr_glitter_curve_push_back(vec, &c);
}

void FASTCALL glitter_curve_unparse_file(f2_struct* st, glitter_curve* c, bool use_big_endian) {
    glitter_curve_pack_file(st, c, use_big_endian, c->keys.end - c->keys.begin);

    f2_struct* s = f2_struct_init();
    glitter_curve_key_pack_file(s, c, use_big_endian);
    vector_f2_struct_push_back(&st->sub_structs, s);
    free(s);
}

void FASTCALL glitter_curve_dispose(glitter_curve* c) {
    vector_glitter_curve_key_free(&c->keys);
    free(c);
}

static float_t FASTCALL glitter_curve_apply_max_rand(glitter_curve* c, float_t value) {
    float_t max_rand;

    if (c->flags & GLITTER_CURVE_FLAG_RANDOMIZE) {
        max_rand = c->max_rand;
        max_rand = glitter_random_get_float_clamp(c->flags & GLITTER_CURVE_FLAG_NEGATIVE_RANDOM
            ? -max_rand : 0.0f, max_rand);

        if (c->flags & GLITTER_CURVE_FLAG_MULT)
            max_rand *= value;
        return max_rand + value;
    }
    else
        return value;
}

static void FASTCALL glitter_curve_get_key_indexes(vector_glitter_curve_key* keys, float_t frame, size_t* curr, size_t* next) {
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

bool FASTCALL glitter_curve_get_value(glitter_curve* c, float_t frame, float_t* value, int32_t random) {
    size_t keys_count;
    int32_t _random;
    bool negate;
    float_t _value;
    float_t t;
    size_t key;
    glitter_curve_key* curr_key;
    glitter_curve_key* next_key;
    size_t next_key_index;
    size_t curr_key_index;

    keys_count = c->keys.end - c->keys.begin;
    if (!keys_count)
        return false;

    _random = glitter_random_get();
    glitter_random_set(random);
    negate = c->flags & GLITTER_CURVE_FLAG_NEGATE && glitter_random_get_int_clamp(0, 0xFFFF) > 0x7FFF;
    if (c->flags & GLITTER_CURVE_FLAG_STEP)
        glitter_random_set(_random + 1);

    if (keys_count == 1) {
        _value = glitter_curve_randomize(c, c->keys.begin);
        _value = glitter_curve_apply_max_rand(c, _value);
        *value = negate ? -_value : _value;
        glitter_random_set(_random + 1);
        return true;
    }

    if (c->repeat && (c->start_time > frame || frame >= c->end_time)) {
        t = (frame - c->start_time) / (c->end_time - c->start_time);
        t = t <= 0.0f ? (float_t)(int32_t)t - 1.0f : (float_t)(int32_t)t;
        frame -= t * (c->end_time - c->start_time);
    }

    if (c->end_time <= frame)
        _value = glitter_curve_randomize(c, &c->keys.begin[keys_count - 1]);
    else if (c->start_time > frame)
        _value = glitter_curve_randomize(c, c->keys.begin);
    else if (c->flags & GLITTER_CURVE_FLAG_PLAIN) {
        key = 0;
        if (frame >= c->end_time)
            key = keys_count - 1;
        else if (frame > c->start_time) {
            key = (size_t)((double_t)frame - c->start_time);
            if (gpm->f2)
                key /= 2;

            if (key >= keys_count)
                key = keys_count - 1;
        }
        _value = glitter_curve_randomize(c, &c->keys.begin[key]);
    }
    else {
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
        curr_key = &c->keys.begin[curr_key_index];
        next_key = &c->keys.begin[next_key_index];
        _value = glitter_curve_interpolate(c, frame, curr_key, next_key, curr_key->type);
    }
    _value = glitter_curve_apply_max_rand(c, _value);
    *value = negate ? -_value : _value;
    glitter_random_set(_random + 1);
    return true;
}

static float_t FASTCALL glitter_curve_interpolate(glitter_curve* c, float_t frame,
    glitter_curve_key* curr, glitter_curve_key* next, glitter_key_type type) {
    float_t df;
    float_t next_val;
    float_t curr_val;
    float_t t;
    float_t v15;
    float_t t_2;

    if (type == GLITTER_KEY_CONSTANT)
        return glitter_curve_randomize(c, curr);

    df = (float_t)(next->frame - curr->frame);
    t = (frame - curr->frame) / df;
    if (type == GLITTER_KEY_HERMITE) {
        t_2 = (1.0f - t) * (1.0f - t);
        next_val = glitter_curve_randomize(c, next);
        curr_val = glitter_curve_randomize(c, curr);
        v15 = t_2 * (next_val - curr_val) * (1.0f + 2.0f * t) + ((t_2 * curr->tangent2 + t * (t - 1.0f) * next->tangent1) * df) * t;
        return glitter_curve_randomize(c, curr) + v15;
    }
    else {
        curr_val = glitter_curve_randomize(c, curr);
        next_val = glitter_curve_randomize(c, next);
        return next_val * t + curr_val * (1.0f - t);
    }
}

static void FASTCALL glitter_curve_key_pack_file(f2_struct* st, glitter_curve* c, bool use_big_endian) {
    stream* s = io_open_memory(0, 0);
    io_set_position(s, 0, SEEK_SET);

    float_t scale = 1.0f;
    if (c->keys_version == 0)
        switch (c->type) {
        case GLITTER_CURVE_ROTATION_X:
        case GLITTER_CURVE_ROTATION_Y:
        case GLITTER_CURVE_ROTATION_Z:
            scale = (float_t)(180.0 / M_PI);
            break;
        case GLITTER_CURVE_COLOR_R:
        case GLITTER_CURVE_COLOR_G:
        case GLITTER_CURVE_COLOR_B:
        case GLITTER_CURVE_COLOR_A:
            scale = 255.0f;
            break;
        }

    glitter_curve_key* i;
    if (use_big_endian)
        if (scale == 1.0f)
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->type));
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->frame));
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent1));
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent2));
                    }
                    io_write_float_t(s, reverse_endianess_float_t(i->random_range));
                    io_write_float_t(s, reverse_endianess_float_t(i->value));
                }
            else
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->type));
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->frame));
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent1));
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent2));
                    }
                    io_write_float_t(s, reverse_endianess_float_t(i->value));
                }
        else
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->type));
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->frame));
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent1 * scale));
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent2 * scale));
                    }
                    io_write_float_t(s, reverse_endianess_float_t(i->random_range * scale));
                    io_write_float_t(s, reverse_endianess_float_t(i->value * scale));
                }
            else
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->type));
                    io_write_int16_t(s, reverse_endianess_int16_t((int16_t)i->frame));
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent1 * scale));
                        io_write_float_t(s, reverse_endianess_float_t(i->tangent2 * scale));
                    }
                    io_write_float_t(s, reverse_endianess_float_t(i->value* scale));
                }
    else
        if (scale == 1.0f)
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, (int16_t)i->type);
                    io_write_int16_t(s, (int16_t)i->frame);
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, i->tangent1);
                        io_write_float_t(s, i->tangent2);
                    }
                    io_write_float_t(s, i->random_range);
                    io_write_float_t(s, i->value);
                }
            else
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, (int16_t)i->type);
                    io_write_int16_t(s, (int16_t)i->frame);
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, i->tangent1);
                        io_write_float_t(s, i->tangent2);
                    }
                    io_write_float_t(s, i->value);
                }
        else
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, (int16_t)i->type);
                    io_write_int16_t(s, (int16_t)i->frame);
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, i->tangent1 * scale);
                        io_write_float_t(s, i->tangent2 * scale);
                    }
                    io_write_float_t(s, i->random_range * scale);
                    io_write_float_t(s, i->value * scale);
                }
            else
                for (i = c->keys.begin; i != c->keys.end; i++) {
                    io_write_int16_t(s, (int16_t)i->type);
                    io_write_int16_t(s, (int16_t)i->frame);
                    if (i->type == GLITTER_KEY_HERMITE) {
                        io_write_float_t(s, i->tangent1 * scale);
                        io_write_float_t(s, i->tangent2 * scale);
                    }
                    io_write_float_t(s, i->value * scale);
                }
    io_set_position(s, 0, SEEK_END);
    io_align(s, 0x10);

    st->length = s->io.data.vec.end - s->io.data.vec.begin;
    st->data = force_malloc(st->length);
    memcpy(st->data, s->io.data.vec.begin, st->length);
    io_dispose(s);

    st->header.signature = 0x5359454B;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    st->header.version = c->keys_version;
}

static void FASTCALL glitter_curve_key_unpack_file(f2_struct* st, glitter_curve* c, uint32_t count) {
    size_t i;
    glitter_curve_key key;
    float_t scale;
    float_t* data;

    if (!st || !st->header.data_size)
        return;

    c->keys_version = st->header.version;
    data = st->data;

    key.type = GLITTER_KEY_CONSTANT;
    key.frame = 0;
    key.value = 0.0f;
    key.tangent1 = 0.0f;
    key.tangent2 = 0.0f;
    key.random_range = 0.0f;

    scale = 1.0f;
    if (c->keys_version == 0)
        switch (c->type) {
        case GLITTER_CURVE_ROTATION_X:
        case GLITTER_CURVE_ROTATION_Y:
        case GLITTER_CURVE_ROTATION_Z:
            scale = (float_t)(M_PI / 180.0);
            break;
        case GLITTER_CURVE_COLOR_R:
        case GLITTER_CURVE_COLOR_G:
        case GLITTER_CURVE_COLOR_B:
        case GLITTER_CURVE_COLOR_A:
            scale = (float_t)(1.0 / 255.0);
            break;
        }

    if (st->header.use_big_endian)
        if (scale == 1.0f)
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = count; i; i--) {
                    key.type = reverse_endianess_int16_t(((int16_t*)data)[0]);
                    key.frame = reverse_endianess_int16_t(((int16_t*)data)[1]);
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = reverse_endianess_float_t(data[1]);
                        key.tangent2 = reverse_endianess_float_t(data[2]);
                        key.random_range = reverse_endianess_float_t(data[3]);
                        key.value = reverse_endianess_float_t(data[4]);
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 5;
                    }
                    else {
                        key.random_range = reverse_endianess_float_t(data[1]);
                        key.value = reverse_endianess_float_t(data[2]);
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 3;
                    }
                }
            else
                for (i = count; i; i--) {
                    key.type = reverse_endianess_int16_t(((int16_t*)data)[0]);
                    key.frame = reverse_endianess_int16_t(((int16_t*)data)[1]);
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = reverse_endianess_float_t(data[1]);
                        key.tangent2 = reverse_endianess_float_t(data[2]);
                        key.value = reverse_endianess_float_t(data[3]);
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 4;
                    }
                    else {
                        key.value = reverse_endianess_float_t(data[1]);
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 2;
                    }
                }
        else
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = count; i; i--) {
                    key.type = reverse_endianess_int16_t(((int16_t*)data)[0]);
                    key.frame = reverse_endianess_int16_t(((int16_t*)data)[1]);
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = reverse_endianess_float_t(data[1]) * scale;
                        key.tangent2 = reverse_endianess_float_t(data[2]) * scale;
                        key.random_range = reverse_endianess_float_t(data[3]) * scale;
                        key.value = reverse_endianess_float_t(data[4]) * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 5;
                    }
                    else {
                        key.random_range = reverse_endianess_float_t(data[1]) * scale;
                        key.value = reverse_endianess_float_t(data[2]) * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 3;
                    }
                }
            else
                for (i = count; i; i--) {
                    key.type = reverse_endianess_int16_t(((int16_t*)data)[0]);
                    key.frame = reverse_endianess_int16_t(((int16_t*)data)[1]);
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = reverse_endianess_float_t(data[1]) * scale;
                        key.tangent2 = reverse_endianess_float_t(data[2]) * scale;
                        key.value = reverse_endianess_float_t(data[3]) * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 4;
                    }
                    else {
                        key.value = reverse_endianess_float_t(data[1]) * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 2;
                    }
                }
    else
        if (scale == 1.0f)
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = count; i; i--) {
                    key.type = ((int16_t*)data)[0];
                    key.frame = ((int16_t*)data)[1];
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = data[1];
                        key.tangent2 = data[2];
                        key.random_range = data[3];
                        key.value = data[4];
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 5;
                    }
                    else {
                        key.random_range = data[1];
                        key.value = data[2];
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 3;
                    }
                }
            else
                for (i = count; i; i--) {
                    key.type = ((int16_t*)data)[0];
                    key.frame = ((int16_t*)data)[1];
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = data[1];
                        key.tangent2 = data[2];
                        key.value = data[3];
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 4;
                    }
                    else {
                        key.value = data[1];
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 2;
                    }
                }
        else
            if (c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
                for (i = count; i; i--) {
                    key.type = ((int16_t*)data)[0];
                    key.frame = ((int16_t*)data)[1];
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = data[1] * scale;
                        key.tangent2 = data[2] * scale;
                        key.random_range = data[3] * scale;
                        key.value = data[4] * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 5;
                    }
                    else {
                        key.random_range = data[1] * scale;
                        key.value = data[2] * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 3;
                    }
                }
            else
                for (i = count; i; i--) {
                    key.type = ((int16_t*)data)[0];
                    key.frame = ((int16_t*)data)[1];
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = data[1] * scale;
                        key.tangent2 = data[2] * scale;
                        key.value = data[3] * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 4;
                    }
                    else {
                        key.value = data[1] * scale;
                        vector_glitter_curve_key_push_back(&c->keys, &key);
                        data += 2;
                    }
                }
}

static void FASTCALL glitter_curve_pack_file(f2_struct* st,
    glitter_curve* c, bool use_big_endian, size_t keys_count) {
    uint32_t* d;

    memset(st, 0, sizeof(f2_struct));

    d = force_malloc(0x20);
    st->data = d;
    st->length = 0x20;

    if (use_big_endian) {
        d[0] = reverse_endianess_uint32_t(c->type);
        d[1] = reverse_endianess_uint32_t(c->repeat ? 1 : 0);
        d[2] = reverse_endianess_uint32_t(c->flags);
        if (c->version == 1)
            *(float_t*)&d[3] = reverse_endianess_float_t(c->max_rand);
        else
            switch (c->type) {
            case GLITTER_CURVE_ROTATION_X:
            case GLITTER_CURVE_ROTATION_Y:
            case GLITTER_CURVE_ROTATION_Z:
                *(float_t*)&d[3] = 0.0f;
                break;
            default:
                *(float_t*)&d[3] = reverse_endianess_float_t(c->max_rand);
                break;
            }
        ((uint16_t*)d)[8] = reverse_endianess_uint16_t((uint16_t)keys_count);
        ((uint16_t*)d)[9] = reverse_endianess_uint16_t((uint16_t)roundf(c->start_time));
        ((uint16_t*)d)[10] = reverse_endianess_uint16_t((uint16_t)roundf(c->end_time));
    }
    else {
        d[0] = c->type;
        d[1] = c->repeat ? 1 : 0;
        d[2] = c->flags;
        if (c->version == 1)
            *(float_t*)&d[3] = c->max_rand;
        else
            switch (c->type) {
            case GLITTER_CURVE_ROTATION_X:
            case GLITTER_CURVE_ROTATION_Y:
            case GLITTER_CURVE_ROTATION_Z:
                *(float_t*)&d[3] = 0.0f;
                break;
            default:
                *(float_t*)&d[3] = c->max_rand;
                break;
            }
        ((uint16_t*)d)[8] = (uint16_t)keys_count;
        ((uint16_t*)d)[9] = (uint16_t)roundf(c->start_time);
        ((uint16_t*)d)[10] = (uint16_t)roundf(c->end_time);
    }

    st->header.signature = 0x56525543;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    st->header.version = c->version;
}

static float_t FASTCALL glitter_curve_randomize(glitter_curve* c, glitter_curve_key* key) {
    float_t min_value;
    float_t value;

    if (~c->flags & GLITTER_CURVE_FLAG_RANDOM_RANGE)
        return key->value;

    min_value = c->flags & GLITTER_CURVE_FLAG_NEGATIVE_RANDOM ? -key->random_range : 0.0f;
    value = glitter_random_get_float_clamp(min_value, key->random_range);

    if (c->flags & GLITTER_CURVE_FLAG_MULT)
        value *= key->value;

    return value + key->value;
}

static void FASTCALL glitter_curve_unpack_file(uint32_t* data,
    glitter_curve* c, bool use_big_endian, uint32_t* keys_count) {
    if (use_big_endian) {
        c->type = reverse_endianess_uint32_t(data[0]);
        c->repeat = reverse_endianess_uint32_t(data[1]) != 0;
        c->flags = reverse_endianess_uint32_t(data[2]);
        if (c->version == 1)
            c->max_rand = reverse_endianess_float_t(*(float_t*)&data[3]);
        else
            switch (c->type) {
            case GLITTER_CURVE_ROTATION_X:
            case GLITTER_CURVE_ROTATION_Y:
            case GLITTER_CURVE_ROTATION_Z:
                c->max_rand = 0.0f;
                break;
            default:
                c->max_rand = reverse_endianess_float_t(*(float_t*)&data[3]);
                break;
            }
        *keys_count = reverse_endianess_uint16_t(((uint16_t*)data)[8]);
        c->start_time = (float_t)reverse_endianess_uint16_t(((uint16_t*)data)[9]);
        c->end_time = (float_t)reverse_endianess_uint16_t(((uint16_t*)data)[10]);
    }
    else {
        c->type = data[0];
        c->repeat = data[1] != 0;
        c->flags = data[2];
        if (c->version == 1)
            c->max_rand = *(float_t*)&data[3];
        else
            switch (c->type) {
            case GLITTER_CURVE_ROTATION_X:
            case GLITTER_CURVE_ROTATION_Y:
            case GLITTER_CURVE_ROTATION_Z:
                c->max_rand = 0.0f;
                break;
            default:
                c->max_rand = *(float_t*)&data[3];
                break;
            }
        *keys_count = ((uint16_t*)data)[8];
        c->start_time = (float_t)((uint16_t*)data)[9];
        c->end_time = (float_t)((uint16_t*)data)[10];
    }
}
