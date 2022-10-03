/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/memory_stream.hpp"
#include "interpolation.hpp"
#include "half_t.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

struct mot_header_classic {
    uint32_t key_set_count_offset;
    uint32_t key_set_types_offset;
    uint32_t key_set_offset;
    uint32_t bone_info_offset;
};

struct mot_header_modern {
    uint32_t hash;
    int64_t name_offset;
    int64_t key_set_count_offset;
    int64_t key_set_types_offset;
    int64_t key_set_offset;
    int64_t bone_info_offset;
    int64_t bone_hash_offset;
    int32_t bone_info_count;
};

static void mot_classic_read_inner(mot_set* ms, stream& s);
static void mot_classic_write_inner(mot_set* ms, stream& s);
static void mot_modern_read_inner(mot_set* ms, stream& s);
static void mot_modern_write_inner(mot_set* ms, stream& s);

mot_bone_info::mot_bone_info() : index() {

}

mot_bone_info::~mot_bone_info() {

}

mot_key_set_data::mot_key_set_data() : type(), frames(), values(), keys_count(), data_type() {

}

mot_key_set_data::~mot_key_set_data() {

}

mot_data::mot_data() : info(), frame_count(), bone_info_count(),
murmurhash(), div_frames(), div_count(), bone_info(), key_set() {

}

mot_data::~mot_data() {

}

mot_set::mot_set() : ready(), modern(), big_endian(), is_x() {

}

mot_set::~mot_set() {

}

void mot_set::pack_file(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        mot_classic_write_inner(this, s);
    else
        mot_modern_write_inner(this, s);
    s.copy(data, size);
}

void mot_set::unpack_file(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    if (!modern)
        mot_classic_read_inner(this, s);
    else
        mot_modern_read_inner(this, s);
}

inline static float_t interpolate_mot_value(float_t p1, float_t p2,
    float_t t1, float_t t2, float_t f1, float_t f2, float_t f) {
    float_t df = f - f1;
    float_t t = df / (f2 - f1);
    float_t t_1 = t - 1.0f;
    return (t_1 * t1 + t * t2) * t_1 * df + (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
}

inline static void interpolate_mot_reverse_value(float_t* arr, size_t length,
    float_t* t1, float_t* t2, size_t f1, size_t f2, size_t f) {
    *t2 = 0.0f;

    if (!arr || length < 2 || f - f1 + 1 >= length || f < 1 || f < f1 || f + 2 > f2)
        return;

    float_t df_1 = (float_t)(f - f1);
    float_t df_2 = (float_t)(f - f1 + 1);
    float_t _t1 = df_1 / (float_t)(f2 - f1);
    float_t _t2 = df_2 / (float_t)(f2 - f1);
    float_t t1_1 = _t1 - 1.0f;
    float_t t2_1 = _t2 - 1.0f;

    float_t t1_t2_1 = arr[f] - arr[f1] - (_t1 * 2.0f - 3.0f) * (_t1 * _t1) * (arr[f1] - arr[f2]);
    float_t t1_t2_2 = arr[f + 1] - arr[f1] - (_t2 * 2.0f - 3.0f) * (_t2 * _t2) * (arr[f1] - arr[f2]);
    t1_t2_1 /= df_1 * t1_1;
    t1_t2_2 /= df_2 * t2_1;

    *t1 = (t1_t2_1 * _t2 - t1_t2_2 * _t1) / (_t1 - _t2);
    *t2 = (-t1_t2_1 * t2_1 + t1_t2_2 * t1_1) / (_t1 - _t2);
}

inline static void mot_set_add_key(uint16_t frame, float_t v, float_t t,
    std::vector<uint16_t>& frames, std::vector<float_t>& values) {
    frames.push_back(frame);
    values.push_back(v);
    values.push_back(t);
}

inline static float_t mot_set_add_key(bool has_error, float_t* a, int32_t frame,
    size_t i, float_t t1, float_t t2, float_t t2_old,
    std::vector<uint16_t>& frames, std::vector<float_t>& values) {
    if (has_error) {
        float_t _t2 = t2_old;
        for (size_t j = 0; j < i; j++) {
            mot_set_add_key((uint16_t)(frame + j), a[j], _t2, frames, values);
            _t2 = 0.0f;
            mot_set_add_key((uint16_t)(frame + j), a[j], 0.0f, frames, values);
        }
        return 0.0f;
    }
    else {
        mot_set_add_key((uint16_t)frame, a[0], t2_old, frames, values);
        if (t2_old == 0.0f)
            mot_set_add_key((uint16_t)frame, a[0], t2_old, frames, values);
        if (t1 != t2_old) {
            mot_set_add_key((uint16_t)frame, a[0], t1, frames, values);
            if (t1 == 0.0f)
                mot_set_add_key((uint16_t)frame, a[0], t1, frames, values);
        }
        return t2;
    }
}

mot_key_set_type mot_set::fit_keys_into_curve(std::vector<float_t>& values_src,
    std::vector<uint16_t>& frames, std::vector<float_t>& values) {
    size_t count = values_src.size();
    if (!count)
        return MOT_KEY_SET_NONE;
    else if (count == 1) {
        if (values_src[0] != 0.0f) {
            values.push_back(values_src[0]);
            return MOT_KEY_SET_STATIC;
        }
        else
            return MOT_KEY_SET_NONE;
    }
    else {
        uint32_t val = *(uint32_t*)&values_src.data()[0];
        uint32_t* arr = (uint32_t*)&values_src.data()[1];
        for (size_t i = count - 1; i; i--)
            if (val != *arr++)
                break;

        if (arr == (uint32_t*)(values_src.data() + count))
            if (values_src[0] != 0.0f) {
                values.push_back(values_src[0]);
                return MOT_KEY_SET_STATIC;
            }
            else
                return MOT_KEY_SET_NONE;
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
            if (left_count > 1)
                t2_old = mot_set_add_key(true, a, frame, left_count - 1, 0.0f, 0.0f, t2_old, frames, values);
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
                interpolate_mot_reverse_value(a, left_count, &_t1, &_t2, 0, i, j);
                tt1 += _t1;
                tt2 += _t2;
            }
            t1 = (float_t)(tt1 / (double_t)(i - 2));
            t2 = (float_t)(tt2 / (double_t)(i - 2));

            has_error = false;
            for (size_t j = 1; j < i - 1; j++) {
                float_t val = interpolate_mot_value(a[0], a[i], t1, t2, 0.0f, (float_t)i, (float_t)j);
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

            t2_old = mot_set_add_key(has_error, a, frame, c, t1, t2, t2_old, frames, values);
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
            t2_old = mot_set_add_key(has_error, a, frame, c, t1_prev, t2_prev, t2_old, frames, values);
            c = (int32_t)i;
        }

        prev_frame = frame;
        frame += c;
        a += c;
        left_count -= c;
    }

    mot_set_add_key((uint16_t)(start_time + (count - 1)), arr[count - 1], t2_old, frames, values);
    if (t2_old != 0.0f)
        mot_set_add_key((uint16_t)(start_time + (count - 1)), arr[count - 1], 0.0f, frames, values);


    count = values.size() / 2;
    arr = values.data();
    for (size_t i = count; i; i--)
        if (*arr++ != 0.0f)
            break;

    if (arr != values.data() + count)
        return MOT_KEY_SET_HERMITE_TANGENT;

    float_t* arr_src = values.data();
    float_t* arr_dst = values.data();
    for (size_t i = count; i; i--) {
        *arr_dst = *arr_src;
        arr_src++;
        arr_dst += 2;
    }
    values.resize(count);
    return MOT_KEY_SET_HERMITE;
}

static void mot_classic_read_inner(mot_set* ms, stream& s) {
    size_t count = 0;
    while (s.read_uint64_t() != 0) {
        s.read_uint64_t();
        count++;
    }

    if (count == 0) {
        ms->ready = false;
        ms->modern = false;
        ms->big_endian = false;
        ms->is_x = false;
    }

    s.set_position(0x00, SEEK_SET);
    ms->vec.resize(count);
    mot_header_classic* mh = force_malloc_s(mot_header_classic, count);
    for (size_t i = 0; i < count; i++) {
        mh[i].key_set_count_offset = s.read_uint32_t();
        mh[i].key_set_types_offset = s.read_uint32_t();
        mh[i].key_set_offset = s.read_uint32_t();
        mh[i].bone_info_offset = s.read_uint32_t();
    }

    for (size_t i = 0; i < count; i++) {
        mot_data* m = &ms->vec[i];

        s.set_position(mh[i].bone_info_offset, SEEK_SET);
        m->bone_info_count = 0;
        s.read_uint16_t();
        do
            m->bone_info_count++;
        while (s.read_uint16_t() != 0 && s.get_position() < s.length);

        s.set_position(mh[i].bone_info_offset, SEEK_SET);
        m->bone_info.resize(m->bone_info_count);
        for (size_t j = 0; j < m->bone_info_count; j++)
            m->bone_info[j].index = s.read_uint16_t();

        s.set_position(mh[i].key_set_count_offset, SEEK_SET);
        m->info = s.read_uint16_t();
        m->frame_count = s.read_uint16_t();

        m->key_set.resize(m->key_set_count);
        s.set_position(mh[i].key_set_types_offset, SEEK_SET);
        for (int32_t j = 0, b = 0; j < m->key_set_count; j++) {
            if (j % 8 == 0)
                b = s.read_uint16_t();

            m->key_set[j].type = (mot_key_set_type)((b >> (j % 8 * 2)) & 0x03);
        }

        s.set_position(mh[i].key_set_offset, SEEK_SET);
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_NONE) {
                mks->keys_count = 0;
                mks->frames.resize(0);
                mks->values.resize(0);
            }
            else if (mks->type == MOT_KEY_SET_STATIC) {
                mks->keys_count = 1;
                mks->frames.resize(0);
                mks->values.resize(1);
                mks->values[0] = s.read_float_t();
            }
            else {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = s.read_uint16_t();
                mks->keys_count = keys_count;

                mks->frames.resize(keys_count);
                mks->values.resize(has_tangents ? keys_count * 2ULL : keys_count);

                uint16_t* frames = mks->frames.data();
                for (int32_t k = 0; k < keys_count; k++)
                    *frames++ = s.read_uint16_t();
                s.align_read(0x04);

                float_t* values = mks->values.data();
                if (!has_tangents)
                    for (int32_t k = 0; k < keys_count; k++)
                        *values++ = s.read_float_t();
                else
                    for (int32_t k = 0; k < keys_count; k++) {
                        *values++ = s.read_float_t();
                        *values++ = s.read_float_t();
                    }
            }
        }
    }
    free_def(mh);

    ms->ready = true;
    ms->modern = false;
    ms->big_endian = false;
    ms->is_x = false;
}

static void mot_classic_write_inner(mot_set* ms, stream& s) {
    size_t count = ms->vec.size();
    s.set_position(count * 0x10 + 0x10, SEEK_SET);
    mot_header_classic* mh = force_malloc_s(mot_header_classic, count);
    for (size_t i = 0; i < count; i++) {
        mot_data* m = &ms->vec[i];

        mh[i].key_set_count_offset = (uint32_t)s.get_position();
        s.write_uint16_t(m->info);
        s.write_uint16_t(m->frame_count);

        mh[i].key_set_types_offset = (uint32_t)s.get_position();
        uint16_t key_set_type_buf = 0;
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_type type = m->key_set[j].type;
            if (type == MOT_KEY_SET_STATIC && m->key_set[j].values[0] == 0.0f)
                type = MOT_KEY_SET_NONE;

            key_set_type_buf |= ((uint16_t)type & 0x03) << (j % 8 * 2);

            if (j % 8 == 7) {
                s.write_uint16_t(key_set_type_buf);
                key_set_type_buf = 0;
            }
        }

        if (m->key_set_count % 8 != 0)
            s.write_uint16_t(key_set_type_buf);
        s.align_write(0x04);

        mh[i].key_set_offset = (uint32_t)s.get_position();
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC) {
                if (mks->values[0] != 0.0f)
                    s.write_float_t(mks->values[0]);
            }
            else if (mks->type != MOT_KEY_SET_NONE) {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = mks->keys_count;
                s.write_uint16_t(keys_count);

                uint16_t* frames = mks->frames.data();
                s.write(frames, sizeof(uint16_t) * keys_count);
                s.align_write(0x04);

                float_t* values = mks->values.data();
                if (!has_tangents)
                    s.write(values, sizeof(float_t) * keys_count);
                else
                    s.write(values, sizeof(float_t) * keys_count * 2);
            }
        }
        s.align_write(0x04);

        mh[i].bone_info_offset = (uint32_t)s.get_position();
        if (m->bone_info_count)
            for (int32_t j = 0; j < m->bone_info_count; j++)
                s.write_uint16_t(m->bone_info[j].index);
        else
            s.write_uint16_t(0x00);
        s.write_uint16_t(0x00);
    }
    s.align_write(0x10);

    s.set_position(0x00, SEEK_SET);
    for (size_t i = 0; i < count; i++) {
        s.write_uint32_t(mh[i].key_set_count_offset);
        s.write_uint32_t(mh[i].key_set_types_offset);
        s.write_uint32_t(mh[i].key_set_offset);
        s.write_uint32_t(mh[i].bone_info_offset);
    }
    free_def(mh);
}

static void mot_modern_read_inner(mot_set* ms, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('MOTC') || !st.data.size()) {
        ms->ready = false;
        ms->modern = false;
        ms->big_endian = false;
        ms->is_x = false;
        return;
    }

    bool big_endian = st.header.use_big_endian;

    memory_stream s_motc;
    s_motc.open(st.data);
    s_motc.big_endian = big_endian;

    s_motc.set_position(0x0C, SEEK_SET);
    bool is_x = s_motc.read_uint32_t_reverse_endianness() == 0;

    s_motc.set_position(0x00, SEEK_SET);
    ms->vec.resize(1);
    mot_header_modern mh;
    memset(&mh, 0, sizeof(mot_header_modern));
    if (!is_x) {
        mh.hash = (uint32_t)s_motc.read_uint64_t_reverse_endianness();
        mh.name_offset = s_motc.read_offset_f2(st.header.length);
        mh.key_set_count_offset = s_motc.read_offset_f2(st.header.length);
        mh.key_set_types_offset = s_motc.read_offset_f2(st.header.length);
        mh.key_set_offset = s_motc.read_offset_f2(st.header.length);
        mh.bone_info_offset = s_motc.read_offset_f2(st.header.length);
        mh.bone_hash_offset = s_motc.read_offset_f2(st.header.length);
        mh.bone_info_count = s_motc.read_int32_t_reverse_endianness();
    }
    else {
        mh.hash = (uint32_t)s_motc.read_uint64_t_reverse_endianness();
        mh.name_offset = s_motc.read_offset_x();
        mh.key_set_count_offset = s_motc.read_offset_x();
        mh.key_set_types_offset = s_motc.read_offset_x();
        mh.key_set_offset = s_motc.read_offset_x();
        mh.bone_info_offset = s_motc.read_offset_x();
        mh.bone_hash_offset = s_motc.read_offset_x();
        mh.bone_info_count = s_motc.read_int32_t_reverse_endianness();
    }

    mot_data* m = &ms->vec[0];
    m->div_frames = s_motc.read_uint16_t_reverse_endianness();
    m->div_count = s_motc.read_uint8_t();

    m->name = s_motc.read_string_null_terminated_offset(mh.name_offset);

    s_motc.set_position(mh.bone_info_offset, SEEK_SET);
    m->bone_info_count = mh.bone_info_count;
    m->bone_info.resize(m->bone_info_count);
    if (!is_x)
        for (size_t j = 0; j < mh.bone_info_count; j++)
            m->bone_info[j].name = s_motc.read_string_null_terminated_offset(
                s_motc.read_offset_f2(st.header.length));
    else
        for (size_t j = 0; j < mh.bone_info_count; j++)
            m->bone_info[j].name = s_motc.read_string_null_terminated_offset(
                s_motc.read_offset_x());

    s_motc.set_position(mh.bone_hash_offset, SEEK_SET);
    for (size_t j = 0; j < mh.bone_info_count; j++)
        s_motc.read_uint64_t_reverse_endianness();

    s_motc.set_position(mh.key_set_count_offset, SEEK_SET);
    m->info = s_motc.read_uint16_t_reverse_endianness();
    m->frame_count = s_motc.read_uint16_t_reverse_endianness();

    m->key_set.resize(m->key_set_count);
    s_motc.set_position(mh.key_set_types_offset, SEEK_SET);
    for (int32_t j = 0, b = 0; j < m->key_set_count; j++) {
        if (j % 8 == 0)
            b = s_motc.read_uint16_t_reverse_endianness();

        m->key_set[j].type = (mot_key_set_type)((b >> (j % 8 * 2)) & 0x03);
    }

    s_motc.set_position(mh.key_set_offset, SEEK_SET);
    for (int32_t j = 0; j < m->key_set_count; j++) {
        mot_key_set_data* mks = &m->key_set[j];
        if (mks->type == MOT_KEY_SET_NONE) {
            mks->keys_count = 0;
            mks->frames.resize(0);
            mks->values.resize(0);
        }
        else if (mks->type == MOT_KEY_SET_STATIC) {
            mks->keys_count = 1;
            mks->frames.resize(0);
            mks->values.resize(1);
            mks->values[0] = s_motc.read_float_t_reverse_endianness();
        }
        else {
            bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
            uint16_t keys_count = s_motc.read_uint16_t_reverse_endianness();
            mot_key_set_data_type data_type
                = (mot_key_set_data_type)s_motc.read_uint16_t_reverse_endianness();
            mks->keys_count = keys_count;
            mks->data_type = data_type;

            mks->frames.resize(keys_count);
            mks->values.resize(has_tangents ? keys_count * 2ULL : keys_count);

            uint8_t step = has_tangents ? 2 : 1;
            if (has_tangents) {
                float_t* values = &mks->values[1];
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = s_motc.read_float_t_reverse_endianness();
            }

            float_t* values = mks->values.data();
            if (data_type == MOT_KEY_SET_DATA_F16)
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = half_to_float(s_motc.read_half_t_reverse_endianness());
            else
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = s_motc.read_float_t_reverse_endianness();
            s_motc.align_read(0x04);

            int16_t* frames = (int16_t*)mks->frames.data();
            for (int32_t k = 0; k < keys_count; k++)
                *frames++ = s_motc.read_int16_t_reverse_endianness();
            s_motc.align_read(0x04);
        }
    }

    m->murmurhash = st.header.murmurhash;

    ms->ready = true;
    ms->modern = true;
    ms->big_endian = big_endian;
    ms->is_x = is_x;
}

static void mot_modern_write_inner(mot_set* ms, stream& s) {
    bool big_endian = ms->big_endian;
    bool is_x = ms->is_x;

    memory_stream s_motc;
    s_motc.open();
    s_motc.big_endian = big_endian;

    uint32_t o;
    enrs e;
    enrs_entry ee;
    pof pof;
    uint32_t murmurhash = 0;
    if (ms->vec.size() > 0) {
        mot_header_modern mh;
        memset(&mh, 0, sizeof(mot_header_modern));
        mot_data* m = &ms->vec[0];

        if (!is_x) {
            ee = { 0, 3, 48, 1 };
            ee.append(0, 1, ENRS_QWORD);
            ee.append(0, 7, ENRS_DWORD);
            ee.append(0, 1, ENRS_WORD);
            e.vec.push_back(ee);
            o = 48;
        }
        else {
            ee = { 0, 3, 64, 1 };
            ee.append(0, 7, ENRS_QWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_WORD);
            e.vec.push_back(ee);
            o = 64;
        }

        ee = { o, 1, 4, 1 };
        ee.append(0, 2, ENRS_WORD);
        e.vec.push_back(ee);
        o = 4;

        ee = { o, 1, (uint32_t)((m->key_set_count + 3ULL) / 4), 1 };
        ee.append(0, (uint32_t)((m->key_set_count + 7ULL) / 8), ENRS_WORD);
        e.vec.push_back(ee);
        o = (m->key_set_count + 3) / 4;
        o = align_val(o, 4);

        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC) {
                if (mks->values[0] != 0.0f) {
                    ee = { o, 1, 4, 1 };
                    ee.append(0, 1, ENRS_DWORD);
                    e.vec.push_back(ee);
                    o = 4;
                }
            }
            else if (mks->type != MOT_KEY_SET_NONE) {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                if (has_tangents)
                    ee = { o, 1, 4, 1 };
                else
                    ee = { o, 1, 3, 1 };

                uint16_t keys_count = mks->keys_count;
                mot_key_set_data_type data_type = mks->data_type;
                o = 4;
                if (has_tangents)
                    o += keys_count * 4;

                if (data_type == MOT_KEY_SET_DATA_F16)
                    o += align_val(keys_count * 2, 4);
                else
                    o += keys_count * 4;
                o += align_val(keys_count * 2, 4);
                o = align_val(o, 4);
                ee.size = o;

                ee.append(0, 2, ENRS_WORD);
                if (has_tangents)
                    ee.append(0, keys_count, ENRS_DWORD);
                if (data_type == MOT_KEY_SET_DATA_F16) {
                    ee.append(0, keys_count, ENRS_WORD);
                    ee.append(keys_count % 2 == 1 ? 2u : 0u, keys_count, ENRS_WORD);
                }
                else {
                    ee.append(0, keys_count, ENRS_DWORD);
                    ee.append(0, keys_count, ENRS_WORD);
                }
                e.vec.push_back(ee);
            }
        }
        o = align_val(o, 16);

        if (!is_x) {
            ee = { o, 1, (uint32_t)(m->bone_info_count * 4ULL), 1 };
            ee.append(0, (uint32_t)m->bone_info_count, ENRS_DWORD);
            e.vec.push_back(ee);
            o = m->bone_info_count * 4;
        }
        else {
            ee = { o, 1, (uint32_t)(m->bone_info_count * 8ULL), 1 };
            ee.append(0, (uint32_t)m->bone_info_count, ENRS_QWORD);
            e.vec.push_back(ee);
            o = m->bone_info_count * 8;
        }
        o = align_val(o, 16);

        ee = { o, 1,(uint32_t)(m->bone_info_count * 8ULL), 1 };
        ee.append(0, (uint32_t)m->bone_info_count, ENRS_QWORD);
        e.vec.push_back(ee);
        o = m->bone_info_count * 8;

        if (!is_x) {
            s_motc.write_uint64_t(0);
            io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
            s_motc.write_int32_t(0);
            s_motc.write_int32_t(0);
        }
        else {
            s_motc.write_uint64_t(0);
            io_write_offset_x_pof_add(s_motc, 0, &pof);
            io_write_offset_x_pof_add(s_motc, 0, &pof);
            io_write_offset_x_pof_add(s_motc, 0, &pof);
            io_write_offset_x_pof_add(s_motc, 0, &pof);
            io_write_offset_x_pof_add(s_motc, 0, &pof);
            io_write_offset_x_pof_add(s_motc, 0, &pof);
            s_motc.write_int32_t(0);
        }
        s_motc.write_uint16_t(0);
        s_motc.write_uint8_t(0);
        s_motc.align_write(0x10);

        mh.hash = hash_string_murmurhash(m->name);

        mh.key_set_count_offset = s_motc.get_position();
        s_motc.write_uint16_t_reverse_endianness(m->info);
        s_motc.write_uint16_t_reverse_endianness(m->frame_count);

        mh.key_set_types_offset = s_motc.get_position();
        uint16_t key_set_type_buf = 0;
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_type type = m->key_set[j].type;
            if (type == MOT_KEY_SET_STATIC && m->key_set[j].values[0] == 0.0f)
                type = MOT_KEY_SET_NONE;

            key_set_type_buf |= ((uint16_t)type & 0x03) << (j % 8 * 2);

            if (j % 8 == 7) {
                s_motc.write_uint16_t_reverse_endianness(key_set_type_buf);
                key_set_type_buf = 0;
            }
        }

        if (m->key_set_count % 8 != 0)
            s_motc.write_uint16_t_reverse_endianness(key_set_type_buf);
        s_motc.align_write(0x04);

        mh.key_set_offset = s_motc.get_position();
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC) {
                if (mks->values[0] != 0.0f)
                    s.write_float_t_reverse_endianness(mks->values[0]);
            }
            else if (mks->type != MOT_KEY_SET_NONE) {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = mks->keys_count;
                mot_key_set_data_type data_type = mks->data_type;
                s_motc.write_uint16_t_reverse_endianness(keys_count);
                s_motc.write_uint16_t_reverse_endianness((uint16_t)data_type);

                uint8_t step = has_tangents ? 2 : 1;
                if (has_tangents) {
                    float_t* values = &mks->values[1];
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_float_t_reverse_endianness(*values);
                }

                float_t* values = mks->values.data();
                if (data_type == MOT_KEY_SET_DATA_F16)
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_half_t_reverse_endianness(float_to_half(*values));
                else
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_float_t_reverse_endianness(*values);
                s_motc.align_write(0x04);

                int16_t* frames = (int16_t*)mks->frames.data();
                for (int32_t k = 0; k < keys_count; k++)
                    s_motc.write_int16_t_reverse_endianness(*frames++);
                s_motc.align_write(0x04);
            }
        }
        s_motc.align_write(0x10);

        size_t* bone_info_offsets = force_malloc_s(size_t, m->bone_info_count);
        mh.bone_info_offset = s_motc.get_position();
        if (!is_x)
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
        else
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_offset_x_pof_add(s_motc, 0, &pof);
        s_motc.align_write(0x10);

        mh.bone_hash_offset = s_motc.get_position();
        for (int32_t j = 0; j < m->bone_info_count; j++)
            s_motc.write_uint64_t_reverse_endianness(hash_string_murmurhash(m->bone_info[j].name));
        s_motc.align_write(0x10);

        mh.name_offset = s_motc.get_position();
        s_motc.write_string_null_terminated(m->name);

        for (int32_t j = 0; j < m->bone_info_count; j++) {
            bone_info_offsets[j] = s_motc.get_position();
            s_motc.write_string_null_terminated(m->bone_info[j].name);
        }
        s_motc.align_write(0x10);

        s_motc.set_position(mh.bone_info_offset, SEEK_SET);
        mh.bone_info_count = m->bone_info_count;
        if (!is_x)
            for (int32_t j = 0; j < m->bone_info_count; j++)
                s_motc.write_offset_f2(bone_info_offsets[j], 0x40);
        else
            for (int32_t j = 0; j < m->bone_info_count; j++)
                s_motc.write_offset_x(bone_info_offsets[j]);
        free_def(bone_info_offsets);

        s_motc.set_position(0x00, SEEK_SET);
        if (!is_x) {
            s_motc.write_uint64_t_reverse_endianness((uint64_t)mh.hash);
            s_motc.write_offset_f2(mh.name_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_count_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_types_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_offset, 0x40);
            s_motc.write_offset_f2(mh.bone_info_offset, 0x40);
            s_motc.write_offset_f2(mh.bone_hash_offset, 0x40);
            s_motc.write_int32_t_reverse_endianness(mh.bone_info_count);
        }
        else {
            s_motc.write_uint64_t_reverse_endianness((uint64_t)mh.hash);
            s_motc.write_offset_x(mh.name_offset);
            s_motc.write_offset_x(mh.key_set_count_offset);
            s_motc.write_offset_x(mh.key_set_types_offset);
            s_motc.write_offset_x(mh.key_set_offset);
            s_motc.write_offset_x(mh.bone_info_offset);
            s_motc.write_offset_x(mh.bone_hash_offset);
            s_motc.write_int32_t_reverse_endianness(mh.bone_info_count);
        }

        if (m->div_frames > 0) {
            s_motc.write_uint16_t_reverse_endianness(m->div_frames);
            s_motc.write_uint8_t(m->div_count);
        }
        else {
            s_motc.write_uint16_t_reverse_endianness(0);
            s_motc.write_uint8_t(0);
        }

        murmurhash = m->murmurhash;
    }

    f2_struct st;
    s_motc.align_write(0x10);
    s_motc.copy(st.data);
    s_motc.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOTC');
    st.header.length = 0x40;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;
    st.header.murmurhash = murmurhash;
    st.header.inner_signature = 0xFF010008;

    st.write(s, true, is_x);
}
