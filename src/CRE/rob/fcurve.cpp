/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fcurve.hpp"
#include "../../KKdLib/prj/prj_assert.hpp"
#include "../../KKdLib/mot.hpp"

// 0x14036F780
void Fcurve::clear_fcurve_key() {

}

// 0x14036F790
void Fcurve::dbg_print_fcurve() {
    FcurveKey* fck = fck_ptr;
    for (int32_t i = 0; i < fc_max; fck++, i++) {
        if (fck->kind == FC_STATIC_0)
            prj_tracef(" %d : FC_STATIC_0", i);
        else if (fck->kind == FC_STATIC_DATA)
            prj_tracef(" %d : FC_STATIC_DATA %f", i, *fck->val);
        else if (fck->kind == FC_SPLINE_SLOPE_0) {
            prj_tracef(" %d : FC_SPLINE_SLOPE_0 ", i);

            int32_t sum = max_def(fck->sum, 8);
            for (int32_t j = 0; j < sum; j++)
                prj_tracef("%f ", fck->val[j]);
        }
        else if (fck->kind == FC_SPLINE_DATA) {
            prj_tracef(" %d : FC_SPLINE_DATA ", i);

            int32_t sum = max_def(fck->sum, 4);
            for (int32_t j = 0; j < sum; j++)
                prj_tracef("%f %f ", fck->val[j * 2], fck->val[j * 2 + 1]);
        }
        prj_tracef("\n");
    }
    prj_tracef("\n");
}

// 0x14036F7B0
bool Fcurve::fcurve_init(const void* data) {
    frame_max = 0;
    if (!data)
        return 0;

    const mot_data* d = (const mot_data*)data;
    frame_max = d->frame_max;

    int32_t fc_max = d->key_set_count;
    bool motion_body_type = d->motion_body_type != 0;
    this->fc_max = (uint16_t)fc_max;
    key_num_type = d->info & 0x8000;

    const mot_key_set_data* key_set = d->key_set_array;
    FcurveKey* fck = fck_ptr;

    for (int32_t i = 0; i < fc_max; i++, fck++, key_set++)
        fck->kind = (FcurveKeyKind)key_set->type;

    fcurve_init_u16(d->key_set_array);
    return motion_body_type;
}

// 0x14040AB40
static uint32_t get_leading_zeros_count(uint16_t value) {
    uint32_t v3 = 16;
    if (value >> 8) {
        v3 = 8;
        value >>= 8;
    }

    if (value >> 4) {
        v3 -= 4;
        value >>= 4;
    }

    if (value >> 2) {
        v3 -= 2;
        value >>= 2;
    }

    if (value >> 1)
        value = 2;
    return v3 - value;
}

// 0x14036F8C0
void Fcurve::fcurve_init_u16(const void* data) {
    const mot_key_set_data* key_set = (const mot_key_set_data*)data;
    FcurveKey* fck = fck_ptr;
    for (int32_t i = 0; i < fc_max; i++, key_set++, fck++) {
        if (fck->kind == FC_STATIC_DATA)
            fck->val = key_set->values;
        else if (fck->kind != FC_STATIC_0) {
            uint16_t sum = key_set->keys_count;
            fck->num = key_set->frames;
            fck->val = key_set->values;

            fck->sum = sum;
            fck->cache_idx = 0;

            uint16_t last_idx = frame_max;
            if (sum > 1) {
                if (sum > last_idx)
                    sum = frame_max;
                last_idx = (16 - get_leading_zeros_count(sum)) * frame_max / sum;
            }
            fck->last_idx = last_idx;
        }
    }
}

// 0x4036F9A0
void Fcurve::get_fcurve_value(float_t frame, float_t* value,
    FcurveKey* fck, uint32_t in_fc_max, const struc_369* a6) {
    if (a6->field_0 == 4)
        frame = prj::floorf(frame);

    if (!in_fc_max)
        return;

    for (uint32_t i = in_fc_max; i; i--, fck++, value++) {
        FcurveKeyKind kind = fck->kind;
        const float_t* val = fck->val;
        if (kind == FC_STATIC_0) {
            *value = 0.0f;
            continue;
        }
        else if (kind == FC_STATIC_DATA) {
            *value = val[0];
            continue;
        }
        else if (kind != FC_SPLINE_SLOPE_0 && kind != FC_SPLINE_DATA) {
            *value = 0.0f;
            continue;
        }

        int32_t cache_idx = fck->cache_idx;
        int32_t sum = fck->sum;
        const uint16_t* num = fck->num;
        int32_t frame_int = (int32_t)frame;
        size_t key_index;
        if (cache_idx > sum
            || (frame_int > fck->last_idx + num[cache_idx])
            || cache_idx > 0 && frame_int < num[cache_idx - 1]) {
            const uint16_t* key = num;
            size_t length = sum;
            size_t temp;
            while (length > 0)
                if (key[temp = length / 2] > frame_int)
                    length /= 2;
                else {
                    key += temp + 1;
                    length -= temp + 1;
                }
            key_index = key - num;
        }
        else {
            const uint16_t* key = &num[cache_idx];
            for (const uint16_t* key_end = &num[sum]; key != key_end; key++)
                if (frame_int < *key)
                    break;
            key_index = key - num;
        }

        bool found = false;
        if (key_index < sum)
            for (; key_index < sum; key_index++)
                if ((float_t)num[key_index] >= frame) {
                    found = true;
                    break;
                }

        if (found) {
            float_t next_frame = (float_t)num[key_index];
            if (fabsf(next_frame - frame) > 0.000001f && key_index > 0) {
                float_t curr_frame = (float_t)num[key_index - 1];
                float_t df = frame - curr_frame;
                float_t t = df / (next_frame - curr_frame);
                if (kind == FC_SPLINE_SLOPE_0) {
                    val += key_index - 1;
                    float_t p1 = val[0];
                    float_t p2 = val[1];
                    *value = (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                else {
                    val += key_index * 2 - 2;
                    float_t p1 = val[0];
                    float_t p2 = val[2];
                    float_t t1 = val[1];
                    float_t t2 = val[3];
                    float_t t_1 = t - 1.0f;
                    *value = (t_1 * t1 + t * t2) * t_1 * df
                        + (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                fck->cache_idx = (int32_t)key_index;
                continue;
            }
        }

        if (key_index > 0)
            key_index--;
        if (kind == FC_SPLINE_SLOPE_0)
            *value = val[key_index];
        else
            *value = val[2 * key_index];
        fck->cache_idx = (int32_t)key_index;
    }
}

// 0x140370090
void Fcurve::get_fcurve_value_end(float_t* value) {
    FcurveKey* fck = fck_ptr;
    for (size_t i = 0; i < fc_max; i++, fck++, value++)
        switch (fck->kind) {
        case FC_STATIC_DATA:
            *value = *fck->val;
            break;
        case FC_SPLINE_SLOPE_0:
            *value = fck->val[fck->sum - 1];
            break;
        case FC_SPLINE_DATA:
            *value = fck->val[2 * fck->sum - 2];
            break;
        default:
            *value = 0.0f;
            break;
        }
}

// 0x140370110
void Fcurve::get_fcurve_value_start(float_t* value, uint8_t in_fc_offset, uint8_t in_fc_max) {
    FcurveKey* fck = &fck_ptr[in_fc_offset];
    for (size_t i = in_fc_offset; i < in_fc_max; i++, fck++, value++)
        *value = fck->kind ? *fck->val : 0.0f;
}

// Missing
void Fcurve::set_fck_ptr(FcurveKey* ptr) {
    fck_ptr = ptr;
}
