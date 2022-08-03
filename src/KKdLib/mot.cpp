/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/stream.hpp"
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

mot_set::mot_set() : ready(), modern(), is_x() {

}

mot_set::~mot_set() {

}

void mot_set::pack_file(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
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

    stream s;
    s.open(data, size);
    if (!modern)
        mot_classic_read_inner(this, s);
    else
        mot_modern_read_inner(this, s);
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
    free(mh);

    ms->ready = true;
    ms->modern = false;
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
    free(mh);
}

static void mot_modern_read_inner(mot_set* ms, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('MOTC') || !st.data.size()) {
        ms->ready = false;
        ms->modern = false;
        ms->is_x = false;
        return;
    }

    stream s_motc;
    s_motc.open(st.data);
    s_motc.is_big_endian = st.header.use_big_endian;

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
    ms->is_x = is_x;
}

static void mot_modern_write_inner(mot_set* ms, stream& s) {
    stream s_motc;
    s_motc.open();

    bool is_x = ms->is_x;

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
        s_motc.write_uint16_t(m->info);
        s_motc.write_uint16_t(m->frame_count);

        mh.key_set_types_offset = s_motc.get_position();
        uint16_t key_set_type_buf = 0;
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_type type = m->key_set[j].type;
            if (type == MOT_KEY_SET_STATIC && m->key_set[j].values[0] == 0.0f)
                type = MOT_KEY_SET_NONE;

            key_set_type_buf |= ((uint16_t)type & 0x03) << (j % 8 * 2);

            if (j % 8 == 7) {
                s_motc.write_uint16_t(key_set_type_buf);
                key_set_type_buf = 0;
            }
        }

        if (m->key_set_count % 8 != 0)
            s_motc.write_uint16_t(key_set_type_buf);
        s_motc.align_write(0x04);

        mh.key_set_offset = s_motc.get_position();
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC) {
                if (mks->values[0] != 0.0f)
                    s.write_float_t(mks->values[0]);
            }
            else if (mks->type != MOT_KEY_SET_NONE) {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = mks->keys_count;
                mot_key_set_data_type data_type = mks->data_type;
                s_motc.write_uint16_t(keys_count);
                s_motc.write_uint16_t((uint16_t)data_type);

                uint8_t step = has_tangents ? 2 : 1;
                if (has_tangents) {
                    float_t* values = &mks->values[1];
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_float_t(*values);
                }

                float_t* values = mks->values.data();
                if (data_type == MOT_KEY_SET_DATA_F16)
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_half_t(float_to_half(*values));
                else
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        s_motc.write_float_t(*values);
                s_motc.align_write(0x04);

                int16_t* frames = (int16_t*)mks->frames.data();
                for (int32_t k = 0; k < keys_count; k++)
                    s_motc.write_int16_t(*frames++);
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
            s_motc.write_uint64_t(hash_string_murmurhash(m->bone_info[j].name));
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
        free(bone_info_offsets);

        s_motc.set_position(0x00, SEEK_SET);
        if (!is_x) {
            s_motc.write_uint64_t((uint64_t)mh.hash);
            s_motc.write_offset_f2(mh.name_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_count_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_types_offset, 0x40);
            s_motc.write_offset_f2(mh.key_set_offset, 0x40);
            s_motc.write_offset_f2(mh.bone_info_offset, 0x40);
            s_motc.write_offset_f2(mh.bone_hash_offset, 0x40);
            s_motc.write_int32_t(mh.bone_info_count);
        }
        else {
            s_motc.write_uint64_t((uint64_t)mh.hash);
            s_motc.write_offset_x(mh.name_offset);
            s_motc.write_offset_x(mh.key_set_count_offset);
            s_motc.write_offset_x(mh.key_set_types_offset);
            s_motc.write_offset_x(mh.key_set_offset);
            s_motc.write_offset_x(mh.bone_info_offset);
            s_motc.write_offset_x(mh.bone_hash_offset);
            s_motc.write_int32_t(mh.bone_info_count);
        }

        if (m->div_frames > 0) {
            s_motc.write_uint16_t(m->div_frames);
            s_motc.write_uint8_t(m->div_count);
        }
        else {
            s_motc.write_uint16_t(0);
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
    st.header.use_big_endian = false;
    st.header.use_section_size = true;
    st.header.murmurhash = murmurhash;
    st.header.inner_signature = 0xFF010008;

    st.write(s, true, is_x);
}
