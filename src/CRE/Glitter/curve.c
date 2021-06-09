/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "curve.h"
#include "random.h"
#include "../../KKdLib/interpolation.h"

static const float_t glitter_curve_baked_reverse_bias[] = {
    0.00001f,
    0.0001f,
    0.001f,
};

static const float_t glitter_curve_baked_half_reverse_bias[] = {
    0.000001f,
    0.00001f,
    0.0001f,
};

#ifdef CRE_DEV
static float_t FASTCALL glitter_curve_key_add_key(bool has_error,
    bool has_error_lerp, bool has_error_hermite, float_t* a, float_t* b, int32_t frame,
    const uint8_t step, size_t i, float_t t1, float_t t2, float_t t2_old,
    vector_glitter_curve_key* keys_rev);
#endif
static void FASTCALL glitter_curve_key_pack_file(GLT, f2_struct* st,
    glitter_curve* c, vector_glitter_curve_key* keys);
static void FASTCALL glitter_curve_key_unpack_file(GLT, f2_struct* st, glitter_curve* c, uint32_t count);
static void FASTCALL glitter_curve_pack_file(GLT, f2_struct* st, glitter_curve* c, size_t keys_count);
static void FASTCALL glitter_curve_unpack_file(GLT, void* data,
    glitter_curve* c, uint32_t version, bool use_big_endian, uint32_t* keys_count);

glitter_curve* FASTCALL glitter_curve_init(GLT) {
    glitter_curve* c = force_malloc(sizeof(glitter_curve));
    c->version = GLT_VAL == GLITTER_X ? 0x02 : 0x01;
    c->keys_version = GLT_VAL == GLITTER_X ? 0x03 : 0x02;
    return c;
}

#ifdef CRE_DEV
void FASTCALL glitter_curve_add_value(GLT, glitter_curve* curve, float_t val) {
    vector_glitter_curve_key* keys = &curve->keys_rev;
    for (glitter_curve_key* i = keys->begin; i != keys->end; i++)
        i->value += val;
    glitter_curve_recalculate(GLT_VAL, curve);
}
#endif

glitter_curve* FASTCALL glitter_curve_copy(glitter_curve* c) {
    if (!c)
        return 0;

    glitter_curve* cc = force_malloc(sizeof(glitter_curve));
    *cc = *c;

    cc->keys = (vector_glitter_curve_key){ 0, 0, 0 };
    vector_glitter_curve_key_append(&cc->keys, c->keys.end - c->keys.begin);
    vector_glitter_curve_key_insert_range(&cc->keys, 0, c->keys.begin, c->keys.end);
#ifdef CRE_DEV
    cc->keys_rev = (vector_glitter_curve_key){ 0, 0, 0 };
    vector_glitter_curve_key_append(&cc->keys_rev, c->keys_rev.end - c->keys_rev.begin);
    vector_glitter_curve_key_insert_range(&cc->keys_rev, 0, c->keys_rev.begin, c->keys_rev.end);
#endif
    return cc;
}

bool FASTCALL glitter_curve_parse_file(GLT, f2_struct* st, uint32_t version, glitter_curve** c) {
    f2_struct* i;
    uint32_t keys_count;

    if (!st || !st->header.data_size)
        return false;

    *c = glitter_curve_init(GLT_VAL);
    (*c)->version = st->header.version;
    glitter_curve_unpack_file(GLT_VAL, st->data, *c, version, st->header.use_big_endian, &keys_count);
    if (keys_count)
        for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
            if (!i->header.data_size)
                continue;

            if (i->header.signature == reverse_endianness_uint32_t('KEYS')) {
                glitter_curve_key_unpack_file(GLT_VAL, i, *c, keys_count);
                break;
            }
        }
    return true;
}

#ifdef CRE_DEV
void FASTCALL glitter_curve_recalculate(GLT, glitter_curve* curve) {
    int32_t start_time = curve->start_time;
    int32_t end_time = curve->end_time;

    vector_glitter_curve_key keys_rev = curve->keys_rev;
    vector_glitter_curve_key* keys = &curve->keys;
    vector_glitter_curve_key_clear(keys);
    if (keys_rev.end - keys_rev.begin == 1)
        vector_glitter_curve_key_push_back(keys, &keys_rev.begin[0]);
    else if (keys_rev.end - keys_rev.begin > 1) {
        if (~curve->flags & GLITTER_CURVE_BAKED) {
            vector_glitter_curve_key_insert_range(keys, 0, keys_rev.begin, keys_rev.end);
            return;
        }

        bool curve_baked_half = false;
        if (GLT_VAL == GLITTER_F2 || (GLT_VAL == GLITTER_X && ~curve->flags & GLITTER_CURVE_BAKED_FULL))
            curve_baked_half = true;

        ssize_t keys_count = keys_rev.end - keys_rev.begin;
        size_t count = (size_t)end_time - start_time;
        if (curve_baked_half)
            count /= 2;
        count++;

        const uint8_t step = curve_baked_half ? 2 : 1;

        glitter_curve_key first_key = keys_rev.begin[0];
        glitter_curve_key last_key = keys_rev.begin[keys_count - 1];

        glitter_curve_key key;
        memset(&key, 0, sizeof(glitter_curve_key));
        vector_glitter_curve_key_append(keys, count);
        for (size_t i = 0; i < count; i++) {
            int32_t frame = start_time + (int32_t)(i * step);

            if (frame <= first_key.frame) {
                key.type = GLITTER_KEY_CONSTANT;
                key.frame = frame;
                key.value = first_key.value;
                key.random_range = first_key.random_range;
                vector_glitter_curve_key_push_back(keys, &key);
                continue;
            }
            else if (frame >= last_key.frame) {
                key.type = GLITTER_KEY_CONSTANT;
                key.frame = frame;
                key.value = last_key.value;
                key.random_range = last_key.random_range;
                vector_glitter_curve_key_push_back(keys, &key);
                continue;
            }

            size_t key_idx = 0;
            size_t length = keys_count;
            size_t temp;
            while (length > 0)
                if (frame > keys_rev.begin[key_idx + (temp = length >> 1)].frame) {
                    key_idx += temp + 1;
                    length -= temp + 1;
                }
                else
                    length = temp;

            glitter_curve_key* curr_key = &keys_rev.begin[key_idx - 1];
            glitter_curve_key* next_key = &keys_rev.begin[key_idx];

            float_t val;
            float_t rand_range;
            if (curr_key->type == GLITTER_KEY_CONSTANT) {
                val = curr_key->value;
                rand_range = curr_key->random_range;
            }
            else if (curr_key->type == GLITTER_KEY_HERMITE) {
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

            key.type = GLITTER_KEY_CONSTANT;
            key.frame = frame;
            key.value = val;
            key.random_range = rand_range;
            vector_glitter_curve_key_push_back(keys, &key);
        }
    }
}
#endif

bool FASTCALL glitter_curve_unparse_file(GLT, f2_struct* st, glitter_curve* c) {
#ifndef CRE_DEV
    vector_glitter_curve_key keys = c->keys;
    if (keys.end - keys.begin < 1)
        return false;
#else
    vector_glitter_curve_key keys = c->keys;
    if (c->keys_rev.end - c->keys_rev.begin < 1)
        return false;

    c->keys = (vector_glitter_curve_key){ 0, 0, 0 };
    glitter_curve_recalculate(GLT_VAL, c);
    if (!c->keys.begin) {
        c->keys = keys;
        return false;
    }
#endif
    glitter_curve_pack_file(GLT_VAL, st, c, c->keys.end - c->keys.begin);

    f2_struct s;
    glitter_curve_key_pack_file(GLT_VAL, &s, c, &c->keys);
    vector_f2_struct_push_back(&st->sub_structs, &s);
#ifdef CRE_DEV
    vector_glitter_curve_key_free(&c->keys);
#endif
    c->keys = keys;
    return true;
}

void FASTCALL glitter_curve_dispose(glitter_curve* c) {
    vector_glitter_curve_key_free(&c->keys);
#ifdef CRE_DEV
    vector_glitter_curve_key_free(&c->keys_rev);
#endif
    free(c);
}

static void FASTCALL glitter_curve_key_pack_file(GLT, f2_struct* st,
    glitter_curve* c, vector_glitter_curve_key* keys) {
    size_t l;
    size_t d;
    glitter_key_type key_type;
    size_t count;

    memset(st, 0, sizeof(f2_struct));
    l = 0;

    uint32_t o;
    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    if (keys->begin == keys->end) {
        st->header.signature = reverse_endianness_uint32_t('KEYS');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = c->keys_version;
        return;
    }

    if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE) {
        key_type = keys->begin->type;
        if (key_type == GLITTER_KEY_HERMITE) {
            ee = (enrs_entry){ 0, 2, 20, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_TYPE_DWORD });
            l = 20;
        }
        else {
            ee = (enrs_entry){ 0, 2, 12, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_DWORD });
            l = 12;
        }

        count = 1;
        for (glitter_curve_key* i = keys->begin + 1; i != keys->end;
            i++, count++, l += key_type == GLITTER_KEY_HERMITE ? 20 : 12) {
            if (i->type == key_type)
                continue;

            if (count > 0) {
                ee.repeat_count = (uint32_t)count;
                vector_enrs_entry_push_back(&e, &ee);
            }

            count = 1;
            o = (uint32_t)((key_type == GLITTER_KEY_HERMITE ? 20 : 12) * count);
            if (i->type == GLITTER_KEY_HERMITE) {
                ee = (enrs_entry){ o, 2, 20, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_TYPE_DWORD });
            }
            else {
                ee = (enrs_entry){ o, 2, 12, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_DWORD });
            }
            key_type = i->type;
        }
    }
    else {
        key_type = keys->begin->type;
        if (key_type == GLITTER_KEY_HERMITE) {
            ee = (enrs_entry){ 0, 2, 16, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_DWORD });
            l = 16;
        }
        else {
            ee = (enrs_entry){ 0, 2, 8, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
            l = 8;
        }

        count = 1;
        for (glitter_curve_key* i = keys->begin + 1; i != keys->end;
            i++, count++, l += key_type == GLITTER_KEY_HERMITE ? 16 : 8) {
            if (i->type == key_type)
                continue;

            if (count > 0) {
                ee.repeat_count = (uint32_t)count;
                vector_enrs_entry_push_back(&e, &ee);
            }

            count = 1;
            o = (uint32_t)((key_type == GLITTER_KEY_HERMITE ? 16 : 8) * count);
            if (i->type == GLITTER_KEY_HERMITE) {
                ee = (enrs_entry){ o, 2, 16, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_DWORD });
            }
            else {
                ee = (enrs_entry){ o, 2, 8, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
            }
            key_type = i->type;
        }
    }

    if (count > 0) {
        ee.repeat_count = (uint32_t)count;
        vector_enrs_entry_push_back(&e, &ee);
    }

    float_t scale = 1.0f;
    if (c->keys_version == 0)
        switch (c->type) {
        case GLITTER_CURVE_ROTATION_X:
        case GLITTER_CURVE_ROTATION_Y:
        case GLITTER_CURVE_ROTATION_Z:
            scale = RAD_TO_DEG_FLOAT;
            break;
        case GLITTER_CURVE_COLOR_R:
        case GLITTER_CURVE_COLOR_G:
        case GLITTER_CURVE_COLOR_B:
        case GLITTER_CURVE_COLOR_A:
            scale = 255.0f;
            break;
        }

    l = align_val(l, 0x10);
    d = (size_t)force_malloc(l);
    st->length = l;
    st->data = (void*)d;
    st->enrs = e;

    if (scale == 1.0f)
        if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
            for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
                *(int16_t*)d = (int16_t)i->type;
                *(int16_t*)(d + 2) = (int16_t)i->frame;
                if (i->type == GLITTER_KEY_HERMITE) {
                    *(float_t*)(d + 4) = i->tangent1;
                    *(float_t*)(d + 8) = i->tangent2;
                    *(float_t*)(d + 12) = i->random_range;
                    *(float_t*)(d + 16) = i->value;
                    d += 20;
                }
                else {
                    *(float_t*)(d + 4) = i->random_range;
                    *(float_t*)(d + 8) = i->value;
                    d += 12;
                }
            }
        else
            for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
                *(int16_t*)d = (int16_t)i->type;
                *(int16_t*)(d + 2) = (int16_t)i->frame;
                if (i->type == GLITTER_KEY_HERMITE) {
                    *(float_t*)(d + 4) = i->tangent1;
                    *(float_t*)(d + 8) = i->tangent2;
                    *(float_t*)(d + 12) = i->value;
                    d += 16;
                }
                else {
                    *(float_t*)(d + 4) = i->value;
                    d += 8;
                }
            }
    else
        if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
            for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
                *(int16_t*)d = (int16_t)i->type;
                *(int16_t*)(d + 2) = (int16_t)i->frame;
                if (i->type == GLITTER_KEY_HERMITE) {
                    *(float_t*)(d + 4) = i->tangent1 * scale;
                    *(float_t*)(d + 8) = i->tangent2 * scale;
                    *(float_t*)(d + 12) = i->random_range * scale;
                    *(float_t*)(d + 16) = i->value * scale;
                    d += 20;
                }
                else {
                    *(float_t*)(d + 4) = i->random_range * scale;
                    *(float_t*)(d + 8) = i->value * scale;
                    d += 12;
                }
            }
        else
            for (glitter_curve_key* i = keys->begin; i != keys->end; i++) {
                *(int16_t*)d = (int16_t)i->type;
                *(int16_t*)(d + 2) = (int16_t)i->frame;
                if (i->type == GLITTER_KEY_HERMITE) {
                    *(float_t*)(d + 4) = i->tangent1 * scale;
                    *(float_t*)(d + 8) = i->tangent2 * scale;
                    *(float_t*)(d + 12) = i->value * scale;
                    d += 16;
                }
                else {
                    *(float_t*)(d + 4) = i->value * scale;
                    d += 8;
                }
            }

    st->header.signature = reverse_endianness_uint32_t('KEYS');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = c->keys_version;
}

static void FASTCALL glitter_curve_key_unpack_file(GLT,
    f2_struct* st, glitter_curve* c, uint32_t count) {
    size_t i;
    glitter_curve_key key;
    size_t d;
    float_t scale;

    if (!st || !st->header.data_size || count < 1)
        return;

    c->keys_version = st->header.version;
    d = (size_t)st->data;

    key.type = GLITTER_KEY_CONSTANT;
    key.frame = 0;
    key.value = 0.0f;
    key.tangent1 = 0.0f;
    key.tangent2 = 0.0f;
    key.random_range = 0.0f;

    vector_glitter_curve_key keys = { 0, 0, 0 };
    vector_glitter_curve_key_append(&keys, count);
    if (GLT_VAL == GLITTER_X && c->keys_version != 2) {
        if (st->header.use_big_endian)
            if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
                for (i = count; i; i--) {
                    key.type = load_reverse_endianness_int16_t((void*)d);
                    key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = load_reverse_endianness_float_t((void*)(d + 16));
                        key.tangent2 = load_reverse_endianness_float_t((void*)(d + 20));
                        key.random_range = load_reverse_endianness_float_t((void*)(d + 24));
                        key.value = load_reverse_endianness_float_t((void*)(d + 28));
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 32;
                    }
                    else {
                        key.random_range = load_reverse_endianness_float_t((void*)(d + 8));
                        key.value = load_reverse_endianness_float_t((void*)(d + 12));
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 16;
                    }
                }
            else
                for (i = count; i; i--) {
                    key.type = load_reverse_endianness_int16_t((void*)d);
                    key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                        key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                        key.value = load_reverse_endianness_float_t((void*)(d + 12));
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 16;
                    }
                    else {
                        key.value = load_reverse_endianness_float_t((void*)(d + 12));
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 16;
                    }
                }
        else
            if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
                for (i = count; i; i--) {
                    key.type = *(int16_t*)d;
                    key.frame = *(int16_t*)(d + 2);
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = *(float_t*)(d + 16);
                        key.tangent2 = *(float_t*)(d + 20);
                        key.random_range = *(float_t*)(d + 24);
                        key.value = *(float_t*)(d + 28);
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 32;
                    }
                    else {
                        key.random_range = *(float_t*)(d + 8);
                        key.value = *(float_t*)(d + 12);
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 16;
                    }
                }
            else
                for (i = count; i; i--) {
                    key.type = *(int16_t*)d;
                    key.frame = *(int16_t*)(d + 2);
                    if (key.type == GLITTER_KEY_HERMITE) {
                        key.tangent1 = *(float_t*)(d + 4);
                        key.tangent2 = *(float_t*)(d + 8);
                        key.value = *(float_t*)(d + 12);
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 16;
                    }
                    else {
                        key.value = *(float_t*)(d + 12);
                        vector_glitter_curve_key_push_back(&keys, &key);
                        d += 16;
                    }
                }
    }
    else {
        scale = 1.0f;
        if (c->keys_version == 0)
            switch (c->type) {
            case GLITTER_CURVE_ROTATION_X:
            case GLITTER_CURVE_ROTATION_Y:
            case GLITTER_CURVE_ROTATION_Z:
                scale = DEG_TO_RAD_FLOAT;
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
                if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
                    for (i = count; i; i--) {
                        key.type = load_reverse_endianness_int16_t((void*)d);
                        key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                            key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                            key.random_range = load_reverse_endianness_float_t((void*)(d + 12));
                            key.value = load_reverse_endianness_float_t((void*)(d + 16));
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 20;
                        }
                        else {
                            key.random_range = load_reverse_endianness_float_t((void*)(d + 4));
                            key.value = load_reverse_endianness_float_t((void*)(d + 8));
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 12;
                        }
                    }
                else
                    for (i = count; i; i--) {
                        key.type = load_reverse_endianness_int16_t((void*)d);
                        key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                            key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                            key.value = load_reverse_endianness_float_t((void*)(d + 12));
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 16;
                        }
                        else {
                            key.value = load_reverse_endianness_float_t((void*)(d + 4));
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 8;
                        }
                    }
            else
                if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
                    for (i = count; i; i--) {
                        key.type = load_reverse_endianness_int16_t((void*)d);
                        key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                            key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8)) * scale;
                            key.random_range = load_reverse_endianness_float_t((void*)(d + 12)) * scale;
                            key.value = load_reverse_endianness_float_t((void*)(d + 16)) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 20;
                        }
                        else {
                            key.random_range = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                            key.value = load_reverse_endianness_float_t((void*)(d + 8)) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 12;
                        }
                    }
                else
                    for (i = count; i; i--) {
                        key.type = load_reverse_endianness_int16_t((void*)d);
                        key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                            key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8)) * scale;
                            key.value = load_reverse_endianness_float_t((void*)(d + 12)) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 16;
                        }
                        else {
                            key.value = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 8;
                        }
                    }
        else
            if (scale == 1.0f)
                if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
                    for (i = count; i; i--) {
                        key.type = *(int16_t*)d;
                        key.frame = *(int16_t*)(d + 2);
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = *(float_t*)(d + 4);
                            key.tangent2 = *(float_t*)(d + 8);
                            key.random_range = *(float_t*)(d + 12);
                            key.value = *(float_t*)(d + 16);
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 20;
                        }
                        else {
                            key.random_range = *(float_t*)(d + 4);
                            key.value = *(float_t*)(d + 8);
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 12;
                        }
                    }
                else
                    for (i = count; i; i--) {
                        key.type = *(int16_t*)d;
                        key.frame = *(int16_t*)(d + 2);
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = *(float_t*)(d + 4);
                            key.tangent2 = *(float_t*)(d + 8);
                            key.value = *(float_t*)(d + 12);
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 16;
                        }
                        else {
                            key.value = *(float_t*)(d + 4);
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 8;
                        }
                    }
            else
                if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE)
                    for (i = count; i; i--) {
                        key.type = *(int16_t*)d;
                        key.frame = *(int16_t*)(d + 2);
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = *(float_t*)(d + 4) * scale;
                            key.tangent2 = *(float_t*)(d + 8) * scale;
                            key.random_range = *(float_t*)(d + 12) * scale;
                            key.value = *(float_t*)(d + 16) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 20;
                        }
                        else {
                            key.random_range = *(float_t*)(d + 4) * scale;
                            key.value = *(float_t*)(d + 8) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 12;
                        }
                    }
                else
                    for (i = count; i; i--) {
                        key.type = *(int16_t*)d;
                        key.frame = *(int16_t*)(d + 2);
                        if (key.type == GLITTER_KEY_HERMITE) {
                            key.tangent1 = *(float_t*)(d + 4) * scale;
                            key.tangent2 = *(float_t*)(d + 8) * scale;
                            key.value = *(float_t*)(d + 12) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 16;
                        }
                        else {
                            key.value = *(float_t*)(d + 4) * scale;
                            vector_glitter_curve_key_push_back(&keys, &key);
                            d += 8;
                        }
                    }
    }
    c->keys = keys;

    if (c->flags & GLITTER_CURVE_KEY_RANDOM_RANGE && keys.begin) {
        if (glt_type == GLITTER_X && c->flags & GLITTER_CURVE_BAKED)
            if (c->flags & GLITTER_CURVE_RANDOM_RANGE_NEGATE)
                for (i = 0; i < count; i++) {
                    double_t max = keys.begin[i].value;
                    double_t min = keys.begin[i].random_range;
                    if (*(uint64_t*)&min != *(uint64_t*)&max) {
                        keys.begin[i].value = (float_t)((min + max) * 0.5);
                        keys.begin[i].random_range = (float_t)(max - (min + max) * 0.5);
                    }
                    else
                        keys.begin[i].random_range = 0.0f;
                }
            else
                for (i = 0; i < count; i++) {
                    double_t max = keys.begin[i].value;
                    double_t min = keys.begin[i].random_range;
                    if (*(uint32_t*)&min != *(uint64_t*)&max) {
                        keys.begin[i].value = (float_t)min;
                        keys.begin[i].random_range = (float_t)(max - min);
                    }
                    else
                        keys.begin[i].random_range = 0.0f;
                }

        bool has_key_random_range = false;
        if (GLT_VAL == GLITTER_F2) {
            if (count > 1 && keys.begin[0].random_range != 0.0f
                && keys.begin[1].random_range != 0.0f)
                has_key_random_range = true;
            else if (count > 1 && keys.begin[0].random_range != 0.0f
                && keys.begin[1].random_range == 0.0f) {
                keys.begin[0].value += keys.begin[0].random_range * 10.0f;
                keys.begin[0].random_range = 0.0f;
            }

            if (count > 1 && keys.begin[count - 2].random_range != 0.0f
                && keys.begin[count - 1].random_range != 0.0f)
                    has_key_random_range = true;
            else if (count > 1 && keys.begin[count - 2].random_range == 0.0f
                && keys.begin[count - 1].random_range != 0.0f) {
                keys.begin[count - 1].value += keys.begin[count - 1].random_range * 10.0f;
                keys.begin[count - 1].random_range = 0.0f;
            }
        }

        if (!has_key_random_range)
            for (i = 0; i < count; i++)
                if (keys.begin[i].random_range != 0.0f) {
                    has_key_random_range = true;
                    break;
                }

        if (!has_key_random_range)
            c->flags &= ~GLITTER_CURVE_KEY_RANDOM_RANGE;
    }

#ifdef CRE_DEV
    c->keys_rev = (vector_glitter_curve_key){ 0, 0, 0 };
    if (~c->flags & GLITTER_CURVE_BAKED) {
        vector_glitter_curve_key_insert_range(&c->keys_rev, 0, keys.begin, keys.end);
        return;
    }
    else if (count == 1) {
        keys.begin[0].frame = c->start_time;
        vector_glitter_curve_key_push_back(&c->keys_rev, &keys.begin[0]);
        return;
    }

    bool curve_baked_half = false;
    if (GLT_VAL == GLITTER_F2 || (GLT_VAL == GLITTER_X && ~c->flags & GLITTER_CURVE_BAKED_FULL))
        curve_baked_half = true;

    const uint8_t step = curve_baked_half ? 2 : 1;

    int32_t start_time = c->start_time;
    int32_t end_time = c->start_time + (int32_t)((count - 1) * step);
    c->end_time = end_time;
    float_t* arr_a = force_malloc_s(float_t, count);
    float_t* arr_b = force_malloc_s(float_t, count);
    for (i = 0; i < count; i++) {
        keys.begin[i].frame = start_time + (int32_t)(i * step);
        arr_a[i] = keys.begin[i].value;
        arr_b[i] = keys.begin[i].random_range;
    }

    const float_t* reverse_bias = curve_baked_half
        ? glitter_curve_baked_half_reverse_bias : glitter_curve_baked_reverse_bias;
    const size_t reverse_min_count = curve_baked_half ? 5 : 4;

    vector_glitter_curve_key keys_rev = { 0, 0, 0 };
    float_t* a = arr_a;
    float_t* b = arr_b;
    size_t left_count = count;
    int32_t frame = start_time;
    int32_t prev_frame = start_time;
    float_t t2_old = 0.0f;
    float_t* val_arr = force_malloc_s(float_t, count);
    while (left_count > 0) {
        int32_t c = 0;
        if (left_count < reverse_min_count) {
            glitter_curve_key key;
            memset(&key, 0, sizeof(glitter_curve_key));
            if (left_count == 1) {
                key.type = GLITTER_KEY_CONSTANT;
                key.frame = frame;
                key.value = a[0];
                key.tangent1 = t2_old;
                key.tangent2 = 0.0f;
                key.random_range = b[0];
                vector_glitter_curve_key_push_back(&keys_rev, &key);
                t2_old = 0.0f;
            }
            else {
                bool has_error = false;
                bool has_error_lerp = false;
                for (size_t j = 1; j < left_count; j++) {
                    float_t val = lerp(a[0], a[left_count - 1], (float_t)j / (float_t)left_count);
                    if (fabsf(val - a[0]) > reverse_bias[0]) {
                        has_error = true;
                        if (fabsf(val - a[j]) > reverse_bias[1]) {
                            has_error_lerp = true;
                            break;
                        }
                    }
                }

                t2_old = glitter_curve_key_add_key(has_error, has_error_lerp, true,
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
        bool has_prev_error_hermite = false;
        for (i = reverse_min_count - 1; i < left_count; i++) {
            double_t tt1 = 0.0;
            double_t tt2 = 0.0;
            for (size_t j = 1; j < i - 1; j++) {
                float_t _t1 = 0.0f;
                float_t _t2 = 0.0f;
                interpolate_chs_reverse_step_value(a, left_count, &_t1, &_t2, 0, i, j, step);
                tt1 += _t1;
                tt2 += _t2;
            }
            t1 = (float_t)(tt1 / (double_t)(i - 2));
            t2 = (float_t)(tt2 / (double_t)(i - 2));

            constant = false;
            has_error = false;
            has_error_lerp = false;
            has_error_hermite = false;
            if (i == 3 && a[0] != a[1]) {
                if (a[0] != a[1] && a[1] == a[2]) {
                    has_error = true;
                    has_error_lerp = true;
                    has_error_hermite = true;
                }
                else if (fabsf(a[0] - a[1]) / fabsf(a[1] - a[2]) > 5.0f) {
                    has_error = true;
                    has_error_lerp = true;
                    has_error_hermite = true;
                }
            }

            if (!has_error_hermite)
                for (size_t j = 1; j < i; j++) {
                    float_t val = interpolate_chs_value(a[0], a[i], t1, t2, 0.0f,
                        (float_t)(i * step), (float_t)(j * step));
                    float_t val_lerp = lerp(a[0], a[i], (float_t)j / (float_t)i);
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
                    else
                        constant = true;
                }

            if (!has_error_hermite) {
                t1_prev = t1;
                t2_prev = t2;
                prev_constant = constant;
                has_prev_succeded = true;
                has_prev_error = has_error;
                has_prev_error_lerp = has_error_lerp;
                has_prev_error_hermite = has_error_hermite;
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
                has_error_hermite = has_prev_error_hermite;
            }
            else
                c = (int32_t)i;

            if (has_error_hermite)
                c = 1;

            t2_old = glitter_curve_key_add_key(has_error, has_error_lerp, has_error_hermite,
                a, b, frame, step, c, t1, t2, t2_old, &keys_rev);
            prev_constant = false;
            has_prev_succeded = false;
            break;
        }

        if (has_prev_succeded) {
            t2_old = glitter_curve_key_add_key(has_prev_error, has_prev_error_lerp, has_prev_error_hermite,
                a, b, frame, step, i, t1_prev, t2_prev, t2_old, &keys_rev);
            c = (int32_t)i;
        }
        prev_frame = frame;
        frame += c * step;
        a += c;
        b += c;
        left_count -= c;
    }
    free(val_arr);

    memset(&key, 0, sizeof(glitter_curve_key));
    key.type = GLITTER_KEY_CONSTANT;
    key.frame = start_time + (count - 1) * step;
    key.value = arr_a[count - 1];
    key.tangent1 = t2_old;
    key.tangent2 = 0.0f;
    key.random_range = arr_b[count - 1];
    vector_glitter_curve_key_push_back(&keys_rev, &key);
    free(arr_a);
    free(arr_b);
    c->keys_rev = keys_rev;

    glitter_curve_recalculate(GLT_VAL, c);
#endif
}

#ifdef CRE_DEV
static float_t FASTCALL glitter_curve_key_add_key(bool has_error,
    bool has_error_lerp, bool has_error_hermite, float_t* a, float_t* b, int32_t frame,
    const uint8_t step, size_t i, float_t t1, float_t t2, float_t t2_old,
    vector_glitter_curve_key* keys_rev) {
    glitter_curve_key key;
    memset(&key, 0, sizeof(glitter_curve_key));
    if (has_error && has_error_lerp && has_error_hermite) {
        float_t _t2 = t2_old;
        for (size_t j = 0; j < i; j++) {
            key.type = GLITTER_KEY_CONSTANT;
            key.frame = (int32_t)(frame + j * step);
            key.value = a[j];
            key.tangent1 = _t2;
            key.tangent2 = 0.0f;
            key.random_range = b[j];
            vector_glitter_curve_key_push_back(keys_rev, &key);
            _t2 = 0.0f;
        }
    }
    else if (has_error && has_error_lerp) {
        key.type = GLITTER_KEY_HERMITE;
        key.frame = frame;
        key.value = a[0];
        key.tangent1 = t2_old;
        key.tangent2 = t1;
        key.random_range = b[0];
        vector_glitter_curve_key_push_back(keys_rev, &key);
        return t2;
    }
    else if (has_error || (i > 1 && b[0] != b[1])) {
        key.type = GLITTER_KEY_LINEAR;
        key.frame = frame;
        key.value = a[0];
        key.tangent1 = 0.0f;
        key.tangent2 = 0.0f;
        key.random_range = b[0];
        vector_glitter_curve_key_push_back(keys_rev, &key);
    }
    else {
        key.type = GLITTER_KEY_CONSTANT;
        key.frame = frame;
        key.value = a[0];
        key.tangent1 = 0.0f;
        key.tangent2 = 0.0f;
        key.random_range = b[0];
        vector_glitter_curve_key_push_back(keys_rev, &key);
    }
    return 0.0f;
}
#endif

static void FASTCALL glitter_curve_pack_file(GLT,
    f2_struct* st, glitter_curve* c, size_t keys_count) {
    size_t l;
    size_t d;

    memset(st, 0, sizeof(f2_struct));
    l = 0;

    uint32_t o;
    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 2, 32, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_TYPE_DWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_WORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 32;

    d = (size_t)force_malloc(l);
    st->data = (void*)d;
    st->length = l;
    st->enrs = e;

    float_t random_range = c->random_range;
    if (c->version == 0)
        switch (c->type) {
        case GLITTER_CURVE_ROTATION_X:
        case GLITTER_CURVE_ROTATION_Y:
        case GLITTER_CURVE_ROTATION_Z:
            random_range = 0.0f;
            break;
        }

    *(uint32_t*)d = c->type;
    *(uint32_t*)(d + 4) = c->repeat ? 1 : 0;
    *(uint32_t*)(d + 8) = c->flags;
    *(float_t*)(d + 12) = random_range;
    *(int16_t*)(d + 16) = (int16_t)keys_count;
    *(int16_t*)(d + 18) = (int16_t)c->start_time;
    *(int16_t*)(d + 20) = (int16_t)c->end_time;

    st->header.signature = reverse_endianness_uint32_t('CURV');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = c->version;
}

static void FASTCALL glitter_curve_unpack_file(GLT, void* data,
    glitter_curve* c, uint32_t version, bool use_big_endian, uint32_t* keys_count) {
    size_t d;

    d = (size_t)data;
    if (GLT_VAL == GLITTER_X) {
        if (version == 1) {
            if (use_big_endian) {
                c->type = load_reverse_endianness_uint32_t((void*)d);
                c->repeat = load_reverse_endianness_uint32_t((void*)(d + 4)) != 0;
                c->flags = load_reverse_endianness_uint32_t((void*)(d + 8));
                c->random_range = load_reverse_endianness_float_t((void*)(d + 12));
                *keys_count = (uint32_t)load_reverse_endianness_int16_t((void*)(d + 16));
                c->start_time = load_reverse_endianness_int16_t((void*)(d + 18));
                c->end_time = load_reverse_endianness_int16_t((void*)(d + 20));
            }
            else {
                c->type = *(uint32_t*)d;
                c->repeat = *(uint32_t*)(d + 4) != 0;
                c->flags = *(uint32_t*)(d + 8);
                c->random_range = *(float_t*)(d + 12);
                *keys_count = (uint32_t) *(int16_t*)(d + 16);
                c->start_time = *(int16_t*)(d + 18);
                c->end_time = *(int16_t*)(d + 20);
            }
        }
        else {
            if (use_big_endian) {
                c->type = load_reverse_endianness_uint32_t((void*)d);
                c->repeat = load_reverse_endianness_uint32_t((void*)(d + 4)) != 0;
                c->flags = load_reverse_endianness_uint32_t((void*)(d + 8));
                c->random_range = load_reverse_endianness_float_t((void*)(d + 12));
                *keys_count = (uint32_t)load_reverse_endianness_uint16_t((void*)(d + 28));
                c->start_time = load_reverse_endianness_uint16_t((void*)(d + 30));
                c->end_time = load_reverse_endianness_uint16_t((void*)(d + 32));
            }
            else {
                c->type = *(uint32_t*)d;
                c->repeat = *(uint32_t*)(d + 4) != 0;
                c->flags = *(uint32_t*)(d + 8);
                c->random_range = *(float_t*)(d + 12);
                *keys_count = (uint32_t) *(int16_t*)(d + 28);
                c->start_time = *(int16_t*)(d + 30);
                c->end_time = *(int16_t*)(d + 32);
            }
        }
    }
    else {
        if (use_big_endian) {
            c->type = load_reverse_endianness_uint32_t((void*)d);
            c->repeat = load_reverse_endianness_uint32_t((void*)(d + 4)) != 0;
            c->flags = load_reverse_endianness_uint32_t((void*)(d + 8));
            c->random_range = load_reverse_endianness_float_t((void*)(d + 12));
            *keys_count = (uint32_t)load_reverse_endianness_uint16_t((void*)(d + 16));
            c->start_time = load_reverse_endianness_uint16_t((void*)(d + 18));
            c->end_time = load_reverse_endianness_uint16_t((void*)(d + 20));
        }
        else {
            c->type = *(uint32_t*)d;
            c->repeat = *(uint32_t*)(d + 4) != 0;
            c->flags = *(uint32_t*)(d + 8);
            c->random_range = *(float_t*)(d + 12);
            *keys_count = (uint32_t)*(int16_t*)(d + 16);
            c->start_time = *(int16_t*)(d + 18);
            c->end_time = *(int16_t*)(d + 20);
        }

        if (c->version == 0)
            switch (c->type) {
            case GLITTER_CURVE_ROTATION_X:
            case GLITTER_CURVE_ROTATION_Y:
            case GLITTER_CURVE_ROTATION_Z:
                c->random_range = 0.0f;
                break;
            }
    }
}
