/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/memory_stream.hpp"
#include "half_t.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

struct mot_header_classic {
    uint32_t key_set_info_offset;
    uint32_t key_set_types_offset;
    uint32_t key_set_offset;
    uint32_t bone_info_offset;
};

struct mot_header_modern {
    uint32_t hash;
    int64_t name_offset;
    int64_t key_set_info_offset;
    int64_t key_set_types_offset;
    int64_t key_set_offset;
    int64_t bone_info_offset;
    int64_t bone_hash_offset;
    int32_t bone_info_count;
};

static void mot_classic_read_inner(mot_set* ms, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void mot_classic_write_inner(mot_set* ms, stream& s);
static void mot_modern_read_inner(mot_set* ms, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void mot_modern_write_inner(mot_set* ms, stream& s);

static const char* mot_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc);
static void mot_data_move_data(mot_data* mot_dst, const mot_data* mot_src,
    prj::shared_ptr<prj::stack_allocator> alloc);

static const char* mot_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset);

mot_bone_info::mot_bone_info() : name(), index() {

}

mot_key_set_data::mot_key_set_data() : type(), frames(), values(), keys_count(), data_type() {

}

mot_data::mot_data() : info(), frame_count(), bone_info_count(), murmurhash(),
div_frames(), div_count(), name(), bone_info_array(), key_set_array() {

}

mot_set::mot_set() : ready(), modern(), big_endian(), is_x(), name(), mot_data(), mot_num() {

}

void mot_set::move_data(mot_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!set_src->ready) {
        ready = false;
        modern = false;
        big_endian = false;
        is_x = false;
        return;
    }

    ready = true;
    modern = set_src->modern;
    big_endian = set_src->big_endian;
    is_x = set_src->is_x;

    name = mot_move_data_string(set_src->name, alloc);

    uint32_t mot_num = set_src->mot_num;
    ::mot_data* mot_data_src = set_src->mot_data;
    ::mot_data* mot_data_dst = alloc->allocate<::mot_data>(mot_num);

    for (uint32_t i = 0; i < mot_num; i++)
        mot_data_move_data(&mot_data_dst[i], &mot_data_src[i], alloc);

    this->mot_data = mot_data_dst;
    this->mot_num = mot_num;
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

void mot_set::unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    if (!modern)
        mot_classic_read_inner(this, alloc, s);
    else
        mot_modern_read_inner(this, alloc, s);
}

inline static float_t interpolate_mot_value(float_t p1, float_t p2,
    float_t t1, float_t t2, float_t f1, float_t f2, float_t f) {
    float_t df = f - f1;
    float_t t = df / (f2 - f1);
    float_t t_1 = t - 1.0f;
    return (t_1 * t1 + t * t2) * t_1 * df
        + (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
}

inline static void interpolate_mot_reverse_value(float_t* arr, size_t length,
    float_t& t1, float_t& t2, size_t f1, size_t f2, size_t f) {
    t1 = 0.0f;
    t2 = 0.0f;

    if (!arr || length < 2 || f - f1 + 1 >= length || f < 1 || f < f1 || f + 2 > f2)
        return;

    float_t _t1 = (float_t)(f - f1) / (float_t)(f2 - f1);
    float_t _t2 = (float_t)(f - f1 + 1) / (float_t)(f2 - f1);
    float_t t1_1 = _t1 - 1.0f;
    float_t t2_1 = _t2 - 1.0f;

    float_t t1_t2_1 = arr[f] - arr[f1] - (_t1 * 2.0f - 3.0f) * (_t1 * _t1) * (arr[f1] - arr[f2]);
    float_t t1_t2_2 = arr[f + 1] - arr[f1] - (_t2 * 2.0f - 3.0f) * (_t2 * _t2) * (arr[f1] - arr[f2]);
    t1_t2_1 /= t1_1 * _t1;
    t1_t2_2 /= t2_1 * _t2;

    t1 = -t1_t2_1 * _t2 + t1_t2_2 * _t1;
    t2 = t1_t2_1 * t2_1 - t1_t2_2 * t1_1;
}

inline static void mot_set_add_key(uint16_t frame, float_t v, float_t t,
    std::vector<uint16_t>& frames, std::vector<float_t>& values) {
    frames.push_back(frame);
    values.push_back(v);
    values.push_back(t);
}

inline static float_t mot_set_add_key(bool has_error, float_t* a, int32_t frame,
    size_t i, float_t t1, float_t t2, float_t t2_old,
    std::vector<uint16_t>& frames, std::vector<float_t>& values, bool fast = false) {
    if (has_error) {
        mot_set_add_key((uint16_t)frame, a[0], t2_old, frames, values);
        if (fabsf(t2) != 0.0f)
            mot_set_add_key((uint16_t)frame, a[0], 0.0f, frames, values);
        for (size_t j = 1; j < i; j++)
            mot_set_add_key((uint16_t)(frame + j), a[j], 0.0f, frames, values);
        return 0.0f;
    }
    else {
        const float_t reverse_bias = fast ? 0.0001f : 0.00001f;
        if (fabsf(t1 - t2_old) > reverse_bias) {
            mot_set_add_key((uint16_t)frame, a[0], t2_old, frames, values);
            mot_set_add_key((uint16_t)frame, a[0], t1, frames, values);
        }
        else
            mot_set_add_key((uint16_t)frame, a[0], (t1 + t2_old) * 0.5f, frames, values);
        return t2;
    }
}

mot_key_set_type mot_set::fit_keys_into_curve(std::vector<float_t>& values_src,
    std::vector<uint16_t>& frames, std::vector<float_t>& values, bool fast) {
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

    float_t* arr = values_src.data();

    const float_t reverse_bias = 0.0001f;
    const int32_t reverse_min_count = 4;

    mot_set_add_key(0, arr[0], 0.0f, frames, values);

    float_t* a = arr;
    size_t left_count = count;
    int32_t frame = 0;
    int32_t prev_frame = 0;
    float_t t2_old = 0.0f;
    while (left_count > 0) {
        if (left_count < reverse_min_count) {
            if (left_count > 1)
                t2_old = mot_set_add_key(true, a, frame,
                    left_count - 1, 0.0f, 0.0f, t2_old, frames, values);
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
        bool constant_prev = false;

        int32_t c = 0;
        for (i = reverse_min_count - 1, i_prev = i; i < left_count; i++) {
            bool constant = true;
            for (size_t j = 1; j <= i; j++)
                if (memcmp(&a[0], &a[j], sizeof(float_t))) {
                    constant = false;
                    break;
                }

            if (!fast) {
                double_t t1_accum = 0.0;
                double_t t2_accum = 0.0;
                for (size_t j = 1; j < i; j++) {
                    float_t t1 = 0.0f;
                    float_t t2 = 0.0f;
                    interpolate_mot_reverse_value(a, left_count, t1, t2, 0, i, j);
                    t1_accum += t1;
                    t2_accum += t2;
                }
                t1 = (float_t)(t1_accum / (double_t)(i - 2));
                t2 = (float_t)(t2_accum / (double_t)(i - 2));
            }
            else
                interpolate_mot_reverse_value(a, left_count, t1, t2, 0, i, 1);

            has_error = false;
            for (size_t j = 1; j < i; j++) {
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
                constant_prev = constant;
                has_prev_error = false;
                has_prev_succeded = true;
                if (i < left_count)
                    continue;
            }

            if (has_prev_succeded) {
                i = i_prev;
                t1 = t1_prev;
                t2 = t2_prev;
                constant = constant_prev;
                has_error = false;
                has_prev_succeded = false;
            }

            if (!has_error) {
                if (constant) {
                    t1 = 0.0f;
                    t2 = 0.0f;
                }

                c = (int32_t)i;
                t2_old = mot_set_add_key(has_error, a, frame, c, t1, t2, t2_old, frames, values, fast);
                has_prev_error = false;
                break;
            }

            has_prev_error = true;
        }

        if (has_prev_succeded) {
            t2_old = mot_set_add_key(has_error, a, frame, c, t1_prev, t2_prev, t2_old, frames, values, fast);
            c = (int32_t)i;
        }
        else if (has_prev_error) {
            t2_old = mot_set_add_key(has_error, a, frame, c, t1, t2, t2_old, frames, values, fast);
            c = 1;
        }

        prev_frame = frame;
        frame += c;
        a += c;
        left_count -= c;
    }

    if (frames.back() != (uint16_t)(count - 1))
        mot_set_add_key((uint16_t)(count - 1), arr[count - 1], t2_old, frames, values);
    mot_set_add_key((uint16_t)(count - 1), arr[count - 1], 0.0f, frames, values);


    count = frames.size();
    arr = values.data();
    for (size_t i = count; i; i--, arr += 2)
        if (arr[1] != 0.0f)
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

static void mot_classic_read_inner(mot_set* ms, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    size_t count = 0;
    while (s.read_uint64_t() != 0) {
        s.read_uint64_t();
        count++;
    }

    if (!count) {
        ms->ready = false;
        ms->modern = false;
        ms->big_endian = false;
        ms->is_x = false;
        return;
    }

    uint32_t mot_num = (uint32_t)count;
    ::mot_data* mot_data = alloc->allocate<::mot_data>(mot_num);
    ms->mot_data = mot_data;
    ms->mot_num = mot_num;

    s.set_position(0x00, SEEK_SET);
    mot_header_classic* mh = force_malloc_s(mot_header_classic, mot_num);
    for (uint32_t i = 0; i < mot_num; i++) {
        mh[i].key_set_info_offset = s.read_uint32_t();
        mh[i].key_set_types_offset = s.read_uint32_t();
        mh[i].key_set_offset = s.read_uint32_t();
        mh[i].bone_info_offset = s.read_uint32_t();
    }

    for (size_t i = 0; i < mot_num; i++) {
        ::mot_data* m = &ms->mot_data[i];

        s.set_position(mh[i].bone_info_offset, SEEK_SET);
        uint32_t bone_info_count = 0;
        s.read_uint16_t();
        do
            bone_info_count++;
        while (s.read_uint16_t() != 0 && s.get_position() < s.length);
        m->bone_info_count = bone_info_count;

        mot_bone_info* bone_info_array = alloc->allocate<mot_bone_info>(bone_info_count);
        m->bone_info_array = bone_info_array;

        s.set_position(mh[i].bone_info_offset, SEEK_SET);
        for (size_t j = 0; j < m->bone_info_count; j++)
            bone_info_array[j].index = s.read_uint16_t();

        s.set_position(mh[i].key_set_info_offset, SEEK_SET);
        m->info = s.read_uint16_t();
        m->frame_count = s.read_uint16_t();

        uint32_t key_set_count = m->key_set_count;
        mot_key_set_data* key_set_array = alloc->allocate<mot_key_set_data>(key_set_count);
        m->key_set_array = key_set_array;

        s.set_position(mh[i].key_set_types_offset, SEEK_SET);
        for (int32_t j = 0, b = 0; j < m->key_set_count; j++) {
            if (j % 8 == 0)
                b = s.read_uint16_t();

            key_set_array[j].type = (mot_key_set_type)((b >> (j % 8 * 2)) & 0x03);
        }

        s.set_position(mh[i].key_set_offset, SEEK_SET);
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* key_set = &key_set_array[j];
            if (key_set->type == MOT_KEY_SET_NONE) {
                key_set->keys_count = 0;
                key_set->frames = 0;
                key_set->values = 0;
            }
            else if (key_set->type == MOT_KEY_SET_STATIC) {
                key_set->keys_count = 1;
                key_set->frames = 0;
                key_set->values = alloc->allocate<float_t>(1);
                key_set->values[0] = s.read_float_t();
            }
            else {
                bool has_tangents = key_set->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = s.read_uint16_t();
                key_set->keys_count = keys_count;

                uint16_t* frames = alloc->allocate<uint16_t>(keys_count);
                float_t* values = alloc->allocate<float_t>(has_tangents ? keys_count * 2ULL : keys_count);
                key_set->frames = frames;
                key_set->values = values;

                s.read(frames, sizeof(uint16_t) * keys_count);
                s.align_read(0x04);

                if (!has_tangents)
                    s.read(values, sizeof(float_t) * keys_count);
                else
                    s.read(values, sizeof(float_t) * keys_count * 2ULL);
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
    ::mot_data* mot_data = ms->mot_data;
    uint32_t mot_num = ms->mot_num;

    s.set_position(mot_num * 0x10ULL + 0x10, SEEK_SET);
    mot_header_classic* mh = force_malloc_s(mot_header_classic, mot_num);
    for (size_t i = 0; i < mot_num; i++) {
        ::mot_data* m = &mot_data[i];

        mh[i].key_set_info_offset = (uint32_t)s.get_position();
        s.write_uint16_t(m->info);
        s.write_uint16_t(m->frame_count);

        uint16_t key_set_count = m->key_set_count;
        mot_key_set_data* key_set_array = m->key_set_array;

        mh[i].key_set_types_offset = (uint32_t)s.get_position();
        uint16_t key_set_type_buf = 0;
        for (uint32_t j = 0; j < key_set_count; j++) {
            mot_key_set_type type = key_set_array[j].type;
            if (type == MOT_KEY_SET_STATIC && key_set_array[j].values[0] == 0.0f)
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
        for (uint32_t j = 0; j < key_set_count; j++) {
            mot_key_set_data* key_set = &key_set_array[j];
            if (key_set->type == MOT_KEY_SET_STATIC) {
                if (key_set->values[0] != 0.0f)
                    s.write_float_t(key_set->values[0]);
            }
            else if (key_set->type != MOT_KEY_SET_NONE) {
                bool has_tangents = key_set->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = key_set->keys_count;
                s.write_uint16_t(keys_count);

                uint16_t* frames = key_set->frames;
                s.write(frames, sizeof(uint16_t) * keys_count);
                s.align_write(0x04);

                float_t* values = key_set->values;
                if (!has_tangents)
                    s.write(values, sizeof(float_t) * keys_count);
                else
                    s.write(values, sizeof(float_t) * keys_count * 2ULL);
            }
        }
        s.align_write(0x04);

        uint16_t bone_info_count = m->bone_info_count;
        mot_bone_info* bone_info_array = m->bone_info_array;

        mh[i].bone_info_offset = (uint32_t)s.get_position();
        if (m->bone_info_count)
            for (int32_t j = 0; j < bone_info_count; j++)
                s.write_uint16_t(bone_info_array[j].index);
        else
            s.write_uint16_t(0x00);
        s.write_uint16_t(0x00);
    }
    s.align_write(0x10);

    s.set_position(0x00, SEEK_SET);
    for (size_t i = 0; i < mot_num; i++) {
        s.write_uint32_t(mh[i].key_set_info_offset);
        s.write_uint32_t(mh[i].key_set_types_offset);
        s.write_uint32_t(mh[i].key_set_offset);
        s.write_uint32_t(mh[i].bone_info_offset);
    }
    free_def(mh);
}

static void mot_modern_read_inner(mot_set* ms, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('MOTC') || !st.data.size()) {
        ms->ready = false;
        ms->modern = false;
        ms->big_endian = false;
        ms->is_x = false;
        return;
    }

    uint32_t header_length = st.header.length;
    bool big_endian = st.header.use_big_endian;
    bool is_x = st.pof.shift_x;

    memory_stream s_motc;
    s_motc.open(st.data);
    s_motc.big_endian = big_endian;

    ::mot_data* mot_data = alloc->allocate<::mot_data>();
    ms->mot_data = mot_data;
    ms->mot_num = 1;

    s_motc.set_position(0x00, SEEK_SET);
    mot_header_modern mh = {};
    if (!is_x) {
        mh.hash = (uint32_t)s_motc.read_uint64_t_reverse_endianness();
        mh.name_offset = s_motc.read_offset_f2(header_length);
        mh.key_set_info_offset = s_motc.read_offset_f2(header_length);
        mh.key_set_types_offset = s_motc.read_offset_f2(header_length);
        mh.key_set_offset = s_motc.read_offset_f2(header_length);
        mh.bone_info_offset = s_motc.read_offset_f2(header_length);
        mh.bone_hash_offset = s_motc.read_offset_f2(header_length);
        mh.bone_info_count = s_motc.read_int32_t_reverse_endianness();
    }
    else {
        mh.hash = (uint32_t)s_motc.read_uint64_t_reverse_endianness();
        mh.name_offset = s_motc.read_offset_x();
        mh.key_set_info_offset = s_motc.read_offset_x();
        mh.key_set_types_offset = s_motc.read_offset_x();
        mh.key_set_offset = s_motc.read_offset_x();
        mh.bone_info_offset = s_motc.read_offset_x();
        mh.bone_hash_offset = s_motc.read_offset_x();
        mh.bone_info_count = s_motc.read_int32_t_reverse_endianness();
    }

    ::mot_data* m = &mot_data[0];
    m->div_frames = s_motc.read_uint16_t_reverse_endianness();
    m->div_count = s_motc.read_uint8_t();

    m->name = mot_read_utf8_string_null_terminated_offset(alloc, s_motc, mh.name_offset);

    uint32_t bone_info_count = mh.bone_info_count;
    mot_bone_info* bone_info_array = alloc->allocate<mot_bone_info>(bone_info_count);
    m->bone_info_array = bone_info_array;
    m->bone_info_count = bone_info_count;

    s_motc.set_position(mh.bone_info_offset, SEEK_SET);
    if (!is_x)
        for (size_t j = 0; j < mh.bone_info_count; j++)
            bone_info_array[j].name = mot_read_utf8_string_null_terminated_offset(alloc,
                s_motc, s_motc.read_offset_f2(header_length));
    else
        for (size_t j = 0; j < mh.bone_info_count; j++)
            bone_info_array[j].name = mot_read_utf8_string_null_terminated_offset(alloc,
                s_motc, s_motc.read_offset_x());

    s_motc.set_position(mh.bone_hash_offset, SEEK_SET);
    for (size_t j = 0; j < mh.bone_info_count; j++)
        s_motc.read_uint64_t_reverse_endianness();

    s_motc.set_position(mh.key_set_info_offset, SEEK_SET);
    m->info = s_motc.read_uint16_t_reverse_endianness();
    m->frame_count = s_motc.read_uint16_t_reverse_endianness();

    uint32_t key_set_count = m->key_set_count;
    mot_key_set_data* key_set_array = alloc->allocate<mot_key_set_data>(key_set_count);
    m->key_set_array = key_set_array;

    s_motc.set_position(mh.key_set_types_offset, SEEK_SET);
    for (uint32_t j = 0, b = 0; j < key_set_count; j++) {
        if (j % 8 == 0)
            b = s_motc.read_uint16_t_reverse_endianness();

        key_set_array[j].type = (mot_key_set_type)((b >> (j % 8 * 2)) & 0x03);
    }

    s_motc.set_position(mh.key_set_offset, SEEK_SET);
    for (uint32_t j = 0; j < key_set_count; j++) {
        mot_key_set_data* key_set = &key_set_array[j];
        if (key_set->type == MOT_KEY_SET_NONE) {
            key_set->keys_count = 0;
            key_set->frames = 0;
            key_set->values = 0;
        }
        else if (key_set->type == MOT_KEY_SET_STATIC) {
            key_set->keys_count = 1;
            key_set->frames = 0;
            key_set->values = alloc->allocate<float_t>(1);
            key_set->values[0] = s.read_float_t();
        }
        else {
            bool has_tangents = key_set->type != MOT_KEY_SET_HERMITE;
            uint16_t keys_count = s_motc.read_uint16_t_reverse_endianness();
            mot_key_set_data_type data_type
                = (mot_key_set_data_type)s_motc.read_uint16_t_reverse_endianness();
            key_set->keys_count = keys_count;
            key_set->data_type = data_type;

            int16_t* frames = alloc->allocate<int16_t>(keys_count);
            float_t* values = alloc->allocate<float_t>(has_tangents ? keys_count * 2ULL : keys_count);
            key_set->frames = (uint16_t*)frames;
            key_set->values = values;

            uint8_t step = has_tangents ? 2 : 1;
            if (has_tangents) {
                float_t* values = &key_set->values[1];
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = s_motc.read_float_t_reverse_endianness();
            }

            if (data_type == MOT_KEY_SET_DATA_F16)
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = half_to_float(s_motc.read_half_t_reverse_endianness());
            else
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = s_motc.read_float_t_reverse_endianness();
            s_motc.align_read(0x04);

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

    ::mot_data* mot_data = ms->mot_data;
    uint32_t mot_num = ms->mot_num;

    memory_stream s_motc;
    s_motc.open();
    s_motc.big_endian = big_endian;

    uint32_t o;
    enrs e;
    enrs_entry ee;
    pof pof;
    uint32_t murmurhash = 0;
    if (mot_num > 0) {
        mot_header_modern mh = {};
        ::mot_data* m = &mot_data[0];

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

        uint32_t key_set_count = m->key_set_count;
        uint16_t bone_info_count = m->bone_info_count;

        mot_key_set_data* key_set_array = m->key_set_array;

        ee = { o, 1, (uint32_t)((key_set_count + 3ULL) / 4), 1 };
        ee.append(0, (uint32_t)((key_set_count + 7ULL) / 8), ENRS_WORD);
        e.vec.push_back(ee);
        o = (key_set_count + 3) / 4;
        o = align_val(o, 4);

        for (uint32_t j = 0; j < key_set_count; j++) {
            mot_key_set_data* key_set = &key_set_array[j];
            if (key_set->type == MOT_KEY_SET_STATIC) {
                if (key_set->values[0] != 0.0f) {
                    ee = { o, 1, 4, 1 };
                    ee.append(0, 1, ENRS_DWORD);
                    e.vec.push_back(ee);
                    o = 4;
                }
            }
            else if (key_set->type != MOT_KEY_SET_NONE) {
                bool has_tangents = key_set->type != MOT_KEY_SET_HERMITE;
                if (has_tangents)
                    ee = { o, 1, 4, 1 };
                else
                    ee = { o, 1, 3, 1 };

                uint16_t keys_count = key_set->keys_count;
                mot_key_set_data_type data_type = key_set->data_type;
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
            ee = { o, 1, (uint32_t)(bone_info_count * 4ULL), 1 };
            ee.append(0, bone_info_count, ENRS_DWORD);
            e.vec.push_back(ee);
            o = m->bone_info_count * 4;
        }
        else {
            ee = { o, 1, (uint32_t)(bone_info_count * 8ULL), 1 };
            ee.append(0, bone_info_count, ENRS_QWORD);
            e.vec.push_back(ee);
            o = m->bone_info_count * 8;
        }
        o = align_val(o, 16);

        ee = { o, 1,(uint32_t)(bone_info_count * 8ULL), 1 };
        ee.append(0, bone_info_count, ENRS_QWORD);
        e.vec.push_back(ee);
        o = bone_info_count * 8;

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

        mh.key_set_info_offset = s_motc.get_position();
        s_motc.write_uint16_t_reverse_endianness(m->info);
        s_motc.write_uint16_t_reverse_endianness(m->frame_count);

        mh.key_set_types_offset = s_motc.get_position();
        uint16_t key_set_type_buf = 0;
        for (uint32_t j = 0; j < key_set_count; j++) {
            mot_key_set_type type = key_set_array[j].type;
            if (type == MOT_KEY_SET_STATIC && key_set_array[j].values[0] == 0.0f)
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
            mot_key_set_data* key_set = &key_set_array[j];
            if (key_set->type == MOT_KEY_SET_STATIC) {
                if (key_set->values[0] != 0.0f)
                    s.write_float_t_reverse_endianness(key_set->values[0]);
            }
            else if (key_set->type != MOT_KEY_SET_NONE) {
                bool has_tangents = key_set->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = key_set->keys_count;
                mot_key_set_data_type data_type = key_set->data_type;
                s_motc.write_uint16_t_reverse_endianness(keys_count);
                s_motc.write_uint16_t_reverse_endianness((uint16_t)data_type);

                uint8_t step = has_tangents ? 2 : 1;
                if (has_tangents) {
                    float_t* values = &key_set->values[1];
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_float_t_reverse_endianness(*values);
                }

                float_t* values = key_set->values;
                if (data_type == MOT_KEY_SET_DATA_F16)
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_half_t_reverse_endianness(float_to_half(*values));
                else
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_float_t_reverse_endianness(*values);
                s_motc.align_write(0x04);

                int16_t* frames = (int16_t*)key_set->frames;
                for (int32_t k = 0; k < keys_count; k++)
                    s_motc.write_int16_t_reverse_endianness(*frames++);
                s_motc.align_write(0x04);
            }
        }
        s_motc.align_write(0x10);

        mot_bone_info* bone_info_array = m->bone_info_array;

        size_t* bone_info_offsets = force_malloc_s(size_t, bone_info_count);
        mh.bone_info_offset = s_motc.get_position();
        if (!is_x)
            for (int32_t j = 0; j < bone_info_count; j++)
                io_write_offset_f2_pof_add(s_motc, 0, 0x40, &pof);
        else
            for (int32_t j = 0; j < bone_info_count; j++)
                io_write_offset_x_pof_add(s_motc, 0, &pof);
        s_motc.align_write(0x10);

        mh.bone_hash_offset = s_motc.get_position();
        for (int32_t j = 0; j < bone_info_count; j++)
            s_motc.write_uint64_t_reverse_endianness(hash_string_murmurhash(bone_info_array[j].name));
        s_motc.align_write(0x10);

        mh.name_offset = s_motc.get_position();
        s_motc.write_string_null_terminated(m->name);

        for (int32_t j = 0; j < bone_info_count; j++) {
            bone_info_offsets[j] = s_motc.get_position();
            s_motc.write_string_null_terminated(bone_info_array[j].name);
        }
        s_motc.align_write(0x10);

        s_motc.set_position(mh.bone_info_offset, SEEK_SET);
        mh.bone_info_count = bone_info_count;
        if (!is_x)
            for (int32_t j = 0; j < bone_info_count; j++)
                s_motc.write_offset_f2(bone_info_offsets[j], 0x40);
        else
            for (int32_t j = 0; j < bone_info_count; j++)
                s_motc.write_offset_x(bone_info_offsets[j]);
        free_def(bone_info_offsets);

        s_motc.set_position(0x00, SEEK_SET);
        if (!is_x) {
            s_motc.write_uint64_t_reverse_endianness((uint64_t)mh.hash);
            s_motc.write_offset_f2(mh.name_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_info_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_types_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_offset, 0x40);
            s_motc.write_offset_f2(mh.bone_info_offset, 0x40);
            s_motc.write_offset_f2(mh.bone_hash_offset, 0x40);
            s_motc.write_int32_t_reverse_endianness(mh.bone_info_count);
        }
        else {
            s_motc.write_uint64_t_reverse_endianness((uint64_t)mh.hash);
            s_motc.write_offset_x(mh.name_offset);
            s_motc.write_offset_x(mh.key_set_info_offset);
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

inline static const char* mot_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc) {
    if (str)
        return alloc->allocate<char>(str, utf8_length(str) + 1);
    return 0;
}

static void mot_data_move_data(mot_data* mot_dst, const mot_data* mot_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    mot_dst->info = mot_src->info;
    mot_dst->frame_count = mot_src->frame_count;
    mot_dst->bone_info_count = mot_src->bone_info_count;

    mot_dst->murmurhash = mot_src->murmurhash;
    mot_dst->div_frames = mot_src->div_frames;
    mot_dst->div_count = mot_src->div_count;

    mot_dst->name = mot_move_data_string(mot_src->name, alloc);

    uint32_t bone_info_count = mot_src->bone_info_count;
    mot_bone_info* bone_info_array_src = mot_src->bone_info_array;
    mot_bone_info* bone_info_array_dst = alloc->allocate<mot_bone_info>(bone_info_count);

    for (uint32_t i = 0; i < bone_info_count; i++) {
        mot_bone_info* bone_info_src = &bone_info_array_src[i];
        mot_bone_info* bone_info_dst = &bone_info_array_dst[i];
        bone_info_dst->name = mot_move_data_string(bone_info_src->name, alloc);
        bone_info_dst->index = bone_info_src->index;
    }

    mot_dst->bone_info_array = bone_info_array_dst;
    
    uint32_t key_set_count = mot_src->key_set_count;
    mot_key_set_data* key_set_array_src = mot_src->key_set_array;
    mot_key_set_data* key_set_array_dst = alloc->allocate<mot_key_set_data>(key_set_count);

    for (uint32_t i = 0; i < key_set_count; i++) {
        mot_key_set_data* key_set_src = &key_set_array_src[i];
        mot_key_set_data* key_set_dst = &key_set_array_dst[i];

        mot_key_set_type type = key_set_src->type;
        uint16_t keys_count = key_set_src->keys_count;

        switch (type) {
        case MOT_KEY_SET_NONE:
            key_set_dst->frames = 0;
            key_set_dst->values = 0;
            break;
        case MOT_KEY_SET_STATIC:
            key_set_dst->frames = 0;
            key_set_dst->values = alloc->allocate<float_t>(key_set_src->values, 1);
            break;
        case MOT_KEY_SET_HERMITE:
            key_set_dst->frames = alloc->allocate<uint16_t>(key_set_src->frames, keys_count);
            key_set_dst->values = alloc->allocate<float_t>(key_set_src->values, keys_count);
            break;
        case MOT_KEY_SET_HERMITE_TANGENT:
            key_set_dst->frames = alloc->allocate<uint16_t>(key_set_src->frames, keys_count);
            key_set_dst->values = alloc->allocate<float_t>(key_set_src->values, keys_count * 2ULL);
            break;
        }

        key_set_dst->type = type;
        key_set_dst->keys_count = keys_count;
        key_set_dst->data_type = key_set_src->data_type;
    }

    mot_dst->key_set_array = key_set_array_dst;
}

inline static const char* mot_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset) {
    size_t len = s.read_utf8_string_null_terminated_offset_length(offset);
    char* str = alloc->allocate<char>(len + 1);
    s.position_push(offset, SEEK_SET);
    s.read(str, len);
    s.position_pop();
    str[len] = 0;
    return str;
}
