/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.h"
#include "f2/struct.h"
#include "io/path.h"
#include "io/stream.h"
#include "half_t.h"
#include "hash.h"
#include "str_utils.h"

typedef struct mot_header_classic {
    uint32_t key_set_count_offset;
    uint32_t key_set_types_offset;
    uint32_t key_set_offset;
    uint32_t bone_info_offset;
} mot_header_classic;

typedef struct mot_header_modern {
    uint32_t hash;
    int64_t name_offset;
    int64_t key_set_count_offset;
    int64_t key_set_types_offset;
    int64_t key_set_offset;
    int64_t bone_info_offset;
    int64_t bone_hash_offset;
    int32_t bone_info_count;
} mot_header_modern;

static bool mot_classic_read_inner(mot_set* ms, stream* s);
static void mot_classic_write_inner(mot_set* ms, stream* s);
static bool mot_modern_read_inner(mot_set* ms, stream* s);
static void mot_modern_write_inner(mot_set* ms, stream* s);

mot_set_farc::mot_set_farc() : ready(), modern() {

}

mot_set_farc::~mot_set_farc() {

}

void mot_set_farc::read(const char* path, bool modern) {
    if (!path)
        return;

    char* path_farc = str_utils_add(path, ".farc");
    if (path_check_file_exists(path_farc)) {
        farc f;
        f.read(path_farc, true, false);

        vec.reserve(f.files.size());
        if (!modern)
            for (farc_file& i : f.files) {
                if (!i.data || !i.size
                    || str_utils_compare_length(i.name.c_str(), i.name.size(), "mot_", 4)
                    || !str_utils_check_ends_with(i.name.c_str(), ".bin"))
                    continue;

                mot_set ms;

                stream s;
                io_open(&s, i.data, i.size);
                bool ret = mot_classic_read_inner(&ms, &s);
                io_free(&s);

                if (!ret)
                    continue;

                ms.name = std::string(i.name.c_str(), i.name.size() - 4);
                vec.push_back(ms);
                ready = true;
                modern = false;
            }
        else
            for (farc_file& i : f.files) {
                if (!i.data || !i.size
                    || !str_utils_check_ends_with(i.name.c_str(), ".mot"))
                    continue;

                mot_set ms;

                stream s;
                io_open(&s, i.data, i.size);
                bool ret = mot_classic_read_inner(&ms, &s);
                io_free(&s);

                if (!ret)
                    continue;

                ms.name = std::string(i.name.c_str(), i.name.size() - 4);
                vec.push_back(ms);
                ready = true;
                modern = true;
            }
    }
    free(path_farc);
}

void mot_set_farc::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    wchar_t* path_farc = str_utils_wadd(path, L".farc");
    if (path_check_file_exists(path_farc)) {
        farc f;
        f.read(path_farc, true, false);

        vec.reserve(f.files.size());
        if (!modern)
            for (farc_file& i : f.files) {
                if (!i.data || !i.size
                    || str_utils_compare_length(i.name.c_str(), i.name.size(), "mot_", 4)
                    || !str_utils_check_ends_with(i.name.c_str(), ".bin"))
                    continue;

                mot_set ms;

                stream s;
                io_open(&s, i.data, i.size);
                bool ret = mot_classic_read_inner(&ms, &s);
                io_free(&s);

                if (!ret)
                    continue;

                ms.name = std::string(i.name.c_str(), i.name.size() - 4);
                vec.push_back(ms);
                ready = true;
                modern = false;
            }
        else
            for (farc_file& i : f.files) {
                if (!i.data || !i.size
                    || !str_utils_check_ends_with(i.name.c_str(), ".mot"))
                    continue;

                mot_set ms;

                stream s;
                io_open(&s, i.data, i.size);
                bool ret = mot_classic_read_inner(&ms, &s);
                io_free(&s);

                if (!ret)
                    continue;

                ms.name = std::string(i.name.c_str(), i.name.size() - 4);
                vec.push_back(ms);
                ready = true;
                modern = true;
            }
    }
    free(path_farc);
}

void mot_set_farc::read(const void* data, size_t length, bool modern) {
    if (!data || !length)
        return;

    farc f;
    f.read(data, length, true);

    vec.reserve(f.files.size());
    if (!modern)
        for (farc_file& i : f.files) {
            if (!i.data || !i.size
                || str_utils_compare_length(i.name.c_str(), i.name.size(), "mot_", 4)
                || !str_utils_check_ends_with(i.name.c_str(), ".bin"))
                continue;

            mot_set ms;

            stream s;
            io_open(&s, i.data, i.size);
            bool ret = mot_classic_read_inner(&ms, &s);
            io_free(&s);

            if (!ret)
                continue;

            ms.name = std::string(i.name.c_str(), i.name.size() - 4);
            vec.push_back(ms);
            ready = true;
            modern = false;
        }
    else
        for (farc_file& i : f.files) {
            if (!i.data || !i.size
                || !str_utils_check_ends_with(i.name.c_str(), ".mot"))
                continue;

            mot_set ms;

            stream s;
            io_open(&s, i.data, i.size);
            bool ret = mot_classic_read_inner(&ms, &s);
            io_free(&s);

            if (!ret)
                continue;

            ms.name = std::string(i.name.c_str(), i.name.size() - 4);
            vec.push_back(ms);
            ready = true;
            modern = true;
        }
}

void mot_set_farc::write(const char* path, farc_compress_mode mode) {
    if (!path || !ready)
        return;

    char* path_farc = str_utils_add(path, ".farc");

    farc f;
    if (!modern)
        for (mot_set& i : vec) {
            farc_file ff;
            memset(&ff, 0, sizeof(farc_file));

            stream s;
            io_open(&s);
            mot_classic_write_inner(&i, &s);
            io_align_write(&s, 0x10);
            io_copy(&s, &ff.data, &ff.size);
            io_free(&s);

            ff.name = i.name;
            ff.name += ".bin";

            f.files.push_back(ff);
        }
    else
        for (mot_set& i : vec) {
            farc_file ff;
            memset(&ff, 0, sizeof(farc_file));

            stream s;
            io_open(&s);
            mot_modern_write_inner(&i, &s);
            io_align_write(&s, 0x10);
            io_copy(&s, &ff.data, &ff.size);
            io_free(&s);

            ff.name = i.name;
            ff.name += ".mot";

            f.files.push_back(ff);
        }
    f.write(path_farc, mode, false);
    free(path_farc);
}

void mot_set_farc::write(const wchar_t* path, farc_compress_mode mode) {
    if (!path || !ready)
        return;

    wchar_t* path_farc = str_utils_wadd(path, L".farc");

    farc f;
    if (!modern)
        for (mot_set& i : vec) {
            farc_file ff;

            stream s;
            io_open(&s);
            mot_classic_write_inner(&i, &s);
            io_align_write(&s, 0x10);
            io_copy(&s, &ff.data, &ff.size);
            io_free(&s);

            ff.name = i.name;
            ff.name += ".bin";

            f.files.push_back(ff);
        }
    else
        for (mot_set& i : vec) {
            farc_file ff;

            stream s;
            io_open(&s);
            mot_modern_write_inner(&i, &s);
            io_align_write(&s, 0x10);
            io_copy(&s, &ff.data, &ff.size);
            io_free(&s);

            ff.name = i.name;
            ff.name += ".mot";

            f.files.push_back(ff);
        }
    f.write(path_farc, mode, false);
    free(path_farc);
}

void mot_set_farc::write(void** data, size_t* length, farc_compress_mode mode) {
    if (!data || !ready)
        return;

    farc f;
    if (!modern)
        for (mot_set& i : vec) {
            farc_file ff;
            stream s;
            io_open(&s);
            mot_classic_write_inner(&i, &s);
            io_align_write(&s, 0x10);
            io_copy(&s, &ff.data, &ff.size);
            io_free(&s);

            ff.name = i.name;
            ff.name += ".bin";

            f.files.push_back(ff);
        }
    else
        for (mot_set& i : vec) {
            farc_file ff;
            memset(&ff, 0, sizeof(farc_file));

            stream s;
            io_open(&s);
            mot_modern_write_inner(&i, &s);
            io_align_write(&s, 0x10);
            io_copy(&s, &ff.data, &ff.size);
            io_free(&s);

            ff.name = i.name;
            ff.name += ".mot";

            f.files.push_back(ff);
        }
    f.write(data, length,  mode);
}

bool mot_set_farc::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    mot_set_farc* msf = (mot_set_farc*)data;
    msf->read(string_data(&s), msf->modern);

    string_free(&s);
    return msf->ready;
}

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

mot_set::mot_set() : is_x() {

}

mot_set::~mot_set() {

}

static bool mot_classic_read_inner(mot_set* ms, stream* s) {
    size_t count = 0;
    while (io_read_uint64_t(s) != 0) {
        io_read_uint64_t(s);
        count++;
    }

    if (count == 0) {
        ms->is_x = false;
        return false;
    }

    io_set_position(s, 0x00, SEEK_SET);
    ms->vec.resize(count);
    mot_header_classic* mh = force_malloc_s(mot_header_classic, count);
    for (size_t i = 0; i < count; i++) {
        mh[i].key_set_count_offset = io_read_uint32_t(s);
        mh[i].key_set_types_offset = io_read_uint32_t(s);
        mh[i].key_set_offset = io_read_uint32_t(s);
        mh[i].bone_info_offset = io_read_uint32_t(s);
    }

    for (size_t i = 0; i < count; i++) {
        mot_data* m = &ms->vec[i];

        io_set_position(s, mh[i].bone_info_offset, SEEK_SET);
        m->bone_info_count = 0;
        io_read_uint16_t(s);
        do
            m->bone_info_count++;
        while (io_read_uint16_t(s) != 0 && io_get_position(s) < s->length);

        io_set_position(s, mh[i].bone_info_offset, SEEK_SET);
        m->bone_info.resize(m->bone_info_count);
        for (size_t j = 0; j < m->bone_info_count; j++)
            m->bone_info[j].index = io_read_uint16_t(s);

        io_set_position(s, mh[i].key_set_count_offset, SEEK_SET);
        m->info = io_read_uint16_t(s);
        m->frame_count = io_read_uint16_t(s);

        m->key_set.resize(m->key_set_count);
        io_set_position(s, mh[i].key_set_types_offset, SEEK_SET);
        for (int32_t j = 0, b = 0; j < m->key_set_count; j++) {
            if (j % 8 == 0)
                b = io_read_uint16_t(s);

            m->key_set[j].type = (mot_key_set_type)((b >> (j % 8 * 2)) & 0x03);
        }

        io_set_position(s, mh[i].key_set_offset, SEEK_SET);
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
                mks->values[0] = io_read_float_t(s);
            }
            else {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = io_read_uint16_t(s);
                mks->keys_count = keys_count;

                mks->frames.resize(keys_count);
                mks->values.resize(has_tangents ? keys_count * 2ULL : keys_count);

                uint16_t* frames = mks->frames.data();
                for (int32_t k = 0; k < keys_count; k++)
                    *frames++ = io_read_uint16_t(s);
                io_align_read(s, 0x04);

                float_t* values = mks->values.data();
                if (!has_tangents)
                    for (int32_t k = 0; k < keys_count; k++)
                        *values++ = io_read_float_t(s);
                else
                    for (int32_t k = 0; k < keys_count; k++) {
                        *values++ = io_read_float_t(s);
                        *values++ = io_read_float_t(s);
                    }
            }
        }
    }
    free(mh);

    ms->is_x = false;
    return true;
}

static void mot_classic_write_inner(mot_set* ms, stream* s) {
    size_t count = ms->vec.size();
    io_set_position(s, count * 0x10 + 0x10, SEEK_SET);
    mot_header_classic* mh = force_malloc_s(mot_header_classic, count);
    for (size_t i = 0; i < count; i++) {
        mot_data* m = &ms->vec[i];

        mh[i].key_set_count_offset = (uint32_t)io_get_position(s);
        io_write_uint16_t(s, m->info);
        io_write_uint16_t(s, m->frame_count);

        mh[i].key_set_types_offset = (uint32_t)io_get_position(s);
        uint16_t key_set_type_buf = 0;
        for (int32_t j = 0; j < m->key_set_count; j++) {
            key_set_type_buf |= ((uint16_t)m->key_set[j].type & 0x03) << (j % 8 * 2);

            if (j % 8 == 7) {
                io_write_uint16_t(s, key_set_type_buf);
                key_set_type_buf = 0;
            }
        }

        if (m->key_set_count % 8 != 0)
            io_write_uint16_t(s, key_set_type_buf);
        io_align_write(s, 0x04);

        mh[i].key_set_offset = (uint32_t)io_get_position(s);
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC)
                io_write_float_t(s, mks->values[0]);
            else {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = mks->keys_count;
                io_write_uint16_t(s, keys_count);

                uint16_t* frames = mks->frames.data();
                for (int32_t k = 0; k < keys_count; k++)
                    io_write_uint16_t(s, *frames++);
                io_align_write(s, 0x04);

                float_t* values = mks->values.data();
                if (!has_tangents)
                    for (int32_t k = 0; k < keys_count; k++)
                        io_write_float_t(s, *values++);
                else
                    for (int32_t k = 0; k < keys_count; k++) {
                        io_write_float_t(s, *values++);
                        io_write_float_t(s, *values++);
                    }
            }
        }
        io_align_write(s, 0x04);

        mh[i].bone_info_offset = (uint32_t)io_get_position(s);
        if (m->bone_info_count)
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_uint16_t(s, m->bone_info[j].index);
        else
            io_write_uint16_t(s, 0x00);
        io_write_uint16_t(s, 0x00);
    }
    io_align_write(s, 0x04);

    io_set_position(s, 0x00, SEEK_SET);
    for (size_t i = 0; i < count; i++) {
        io_write_uint32_t(s, mh[i].key_set_count_offset);
        io_write_uint32_t(s, mh[i].key_set_types_offset);
        io_write_uint32_t(s, mh[i].key_set_offset);
        io_write_uint32_t(s, mh[i].bone_info_offset);
    }
    free(mh);
}

static bool mot_modern_read_inner(mot_set* ms, stream* s) {
    bool ret = false;
    f2_struct st;
    f2_struct_read(&st, s);
    if (st.header.signature != reverse_endianness_uint32_t('MOTC') || !st.data) {
        f2_struct_free(&st);
        return false;
    }

    stream s_motc;
    io_open(&s_motc, st.data, st.length);
    s_motc.is_big_endian = st.header.use_big_endian;

    io_set_position(&s_motc, 0x0C, SEEK_SET);
    bool is_x = io_read_uint32_t_stream_reverse_endianness(&s_motc) == 0;

    io_set_position(&s_motc, 0x00, SEEK_SET);
    ms->vec.resize(1);
    mot_header_modern mh;
    memset(&mh, 0, sizeof(mot_header_modern));
    if (!is_x) {
        mh.hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(&s_motc);
        mh.name_offset = io_read_offset_f2(&s_motc, st.header.length);
        mh.key_set_count_offset = io_read_offset_f2(&s_motc, st.header.length);
        mh.key_set_types_offset = io_read_offset_f2(&s_motc, st.header.length);
        mh.key_set_offset = io_read_offset_f2(&s_motc, st.header.length);
        mh.bone_info_offset = io_read_offset_f2(&s_motc, st.header.length);
        mh.bone_hash_offset = io_read_offset_f2(&s_motc, st.header.length);
        mh.bone_info_count = io_read_int32_t_stream_reverse_endianness(&s_motc);
    }
    else {
        mh.hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(&s_motc);
        mh.name_offset = io_read_offset_x(&s_motc);
        mh.key_set_count_offset = io_read_offset_x(&s_motc);
        mh.key_set_types_offset = io_read_offset_x(&s_motc);
        mh.key_set_offset = io_read_offset_x(&s_motc);
        mh.bone_info_offset = io_read_offset_x(&s_motc);
        mh.bone_hash_offset = io_read_offset_x(&s_motc);
        mh.bone_info_count = io_read_int32_t_stream_reverse_endianness(&s_motc);
    }

    mot_data* m = &ms->vec[0];
    m->div_frames = io_read_uint16_t_stream_reverse_endianness(&s_motc);;
    m->div_count = io_read_uint8_t(&s_motc);;

    io_read_string_null_terminated_offset(&s_motc, mh.name_offset, &m->name);

    io_set_position(&s_motc, mh.bone_info_offset, SEEK_SET);
    m->bone_info_count = mh.bone_info_count;
    m->bone_info.resize(m->bone_info_count);
    if (!is_x)
        for (size_t j = 0; j < mh.bone_info_count; j++)
            io_read_string_null_terminated_offset(&s_motc,
                io_read_offset_f2(&s_motc, st.header.length), &m->bone_info[j].name);
    else
        for (size_t j = 0; j < mh.bone_info_count; j++)
            io_read_string_null_terminated_offset(&s_motc,
                io_read_offset_x(&s_motc), &m->bone_info[j].name);

    io_set_position(&s_motc, mh.bone_hash_offset, SEEK_SET);
    for (size_t j = 0; j < mh.bone_info_count; j++)
        io_read_uint64_t_stream_reverse_endianness(&s_motc);

    io_set_position(&s_motc, mh.key_set_count_offset, SEEK_SET);
    m->info = io_read_uint16_t_stream_reverse_endianness(&s_motc);
    m->frame_count = io_read_uint16_t_stream_reverse_endianness(&s_motc);

    m->key_set.resize(m->key_set_count);
    io_set_position(&s_motc, mh.key_set_types_offset, SEEK_SET);
    for (int32_t j = 0, b = 0; j < m->key_set_count; j++) {
        if (j % 8 == 0)
            b = io_read_uint16_t_stream_reverse_endianness(&s_motc);

        m->key_set[j].type = (mot_key_set_type)((b >> (j % 8 * 2)) & 0x03);
    }

    io_set_position(&s_motc, mh.key_set_offset, SEEK_SET);
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
            mks->values[0] = io_read_float_t_stream_reverse_endianness(&s_motc);
        }
        else {
            bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
            uint16_t keys_count = io_read_uint16_t_stream_reverse_endianness(s);
            mot_key_set_data_type data_type
                = (mot_key_set_data_type)io_read_uint16_t_stream_reverse_endianness(&s_motc);
            mks->keys_count = keys_count;
            mks->data_type = data_type;

            mks->frames.resize(keys_count);
            mks->values.resize(has_tangents ? keys_count * 2ULL : keys_count);

            uint8_t step = has_tangents ? 2 : 1;
            if (has_tangents) {
                float_t* values = &mks->values[1];
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = io_read_float_t_stream_reverse_endianness(&s_motc);
            }

            float_t* values = mks->values.data();
            if (data_type == MOT_KEY_SET_DATA_F16)
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = half_to_float(io_read_half_t_stream_reverse_endianness(&s_motc));
            else
                for (int32_t k = 0; k < keys_count; k++, values += step)
                    *values = io_read_float_t_stream_reverse_endianness(&s_motc);
            io_align_read(&s_motc, 0x04);

            int16_t* frames = (int16_t*)mks->frames.data();
            for (int32_t k = 0; k < keys_count; k++)
                *frames++ = io_read_int16_t_stream_reverse_endianness(&s_motc);
            io_align_read(&s_motc, 0x04);
        }
    }

    m->murmurhash = st.header.murmurhash;

    io_free(&s_motc);
    f2_struct_free(&st);

    ms->is_x = is_x;
    return true;
}

static void mot_modern_write_inner(mot_set* ms, stream* s) {
    stream s_motc;
    io_open(&s_motc);
    uint32_t o;
    vector_old_enrs_entry e = vector_old_empty(enrs_entry);
    enrs_entry ee;
    vector_old_size_t pof = vector_old_empty(size_t);
    uint32_t murmurhash = 0;
    if (ms->vec.size() > 0) {
        mot_header_modern mh;
        memset(&mh, 0, sizeof(mot_header_modern));
        mot_data* m = &ms->vec[0];

        if (!ms->is_x) {
            ee = { 0, 3, 48, 1, vector_old_empty(enrs_sub_entry) };
            vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_QWORD);
            vector_old_enrs_sub_entry_append(&ee.sub, 0, 7, ENRS_DWORD);
            vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_WORD);
            vector_old_enrs_entry_push_back(&e, &ee);
            o = 48;
        }
        else {
            ee = { 0, 3, 64, 1, vector_old_empty(enrs_sub_entry) };
            vector_old_enrs_sub_entry_append(&ee.sub, 0, 7, ENRS_QWORD);
            vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
            vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_WORD);
            vector_old_enrs_entry_push_back(&e, &ee);
            o = 64;
        };

        ee = { o, 1, 4, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_WORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        o = 4;

        ee = { o, 1, (uint32_t)((m->key_set_count + 3ULL) / 4), 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, (uint32_t)((m->key_set_count + 7ULL) / 8), ENRS_WORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        o = (m->key_set_count + 3) / 4;
        o = align_val(o, 4);

        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC) {
                ee = { o, 1, 4, 1, vector_old_empty(enrs_sub_entry) };
                vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
                vector_old_enrs_entry_push_back(&e, &ee);
                o = 4;
            }
            else if (mks->type != MOT_KEY_SET_NONE) {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                if (has_tangents)
                    ee = { o, 1, 4, 1, vector_old_empty(enrs_sub_entry) };
                else
                    ee = { o, 1, 3, 1, vector_old_empty(enrs_sub_entry) };

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

                vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_WORD);
                if (has_tangents)
                    vector_old_enrs_sub_entry_append(&ee.sub, 0, keys_count, ENRS_DWORD);
                if (data_type == MOT_KEY_SET_DATA_F16) {
                    vector_old_enrs_sub_entry_append(&ee.sub, 0, keys_count, ENRS_WORD);
                    vector_old_enrs_sub_entry_append(&ee.sub, keys_count % 2 == 1 ? 2 : 0, keys_count, ENRS_WORD);
                }
                else {
                    vector_old_enrs_sub_entry_append(&ee.sub, 0, keys_count, ENRS_DWORD);
                    vector_old_enrs_sub_entry_append(&ee.sub, 0, keys_count, ENRS_WORD);
                }
                vector_old_enrs_entry_push_back(&e, &ee);
            }
        }
        o = align_val(o, 16);

        if (!ms->is_x) {
            ee = { o, 1, (uint32_t)(m->bone_info_count * 4ULL), 1, vector_old_empty(enrs_sub_entry) };
            vector_old_enrs_sub_entry_append(&ee.sub, 0, m->bone_info_count, ENRS_DWORD);
            vector_old_enrs_entry_push_back(&e, &ee);
            o = m->bone_info_count * 4;
        }
        else {
            ee = { o, 1, (uint32_t)(m->bone_info_count * 8ULL), 1, vector_old_empty(enrs_sub_entry) };
            vector_old_enrs_sub_entry_append(&ee.sub, 0, m->bone_info_count, ENRS_QWORD);
            vector_old_enrs_entry_push_back(&e, &ee);
            o = m->bone_info_count * 8;
        }
        o = align_val(o, 16);

        ee = { o, 1,(uint32_t)(m->bone_info_count * 8ULL), 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, m->bone_info_count, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        o = m->bone_info_count * 8;

        if (!ms->is_x) {
            io_write_uint64_t(&s_motc, 0);
            io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
            io_write_int32_t(&s_motc, 0);
            io_write_int32_t(&s_motc, 0);
        }
        else {
            io_write_uint64_t(&s_motc, 0);
            io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_write_offset_x_pof_add(&s_motc, 0, &pof);
            io_write_int32_t(&s_motc, 0);
        }
        io_write_uint16_t(&s_motc, 0);
        io_write_uint8_t(&s_motc, 0);
        io_align_write(&s_motc, 0x10);

        mh.hash = hash_string_murmurhash(&m->name, 0, false);

        mh.key_set_count_offset = io_get_position(&s_motc);
        io_write_uint16_t(&s_motc, m->info);
        io_write_uint16_t(&s_motc, m->frame_count);

        mh.key_set_types_offset = io_get_position(&s_motc);
        uint16_t key_set_type_buf = 0;
        for (int32_t j = 0; j < m->key_set_count; j++) {
            key_set_type_buf |= ((uint16_t)m->key_set[j].type & 0x03) << (j % 8 * 2);

            if (j % 8 == 7) {
                io_write_uint16_t(&s_motc, key_set_type_buf);
                key_set_type_buf = 0;
            }
        }

        if (m->key_set_count % 8 != 0)
            io_write_uint16_t(&s_motc, key_set_type_buf);
        io_align_write(&s_motc, 0x04);

        mh.key_set_offset = io_get_position(&s_motc);
        for (int32_t j = 0; j < m->key_set_count; j++) {
            mot_key_set_data* mks = &m->key_set[j];
            if (mks->type == MOT_KEY_SET_STATIC)
                io_write_float_t(&s_motc, mks->values[0]);
            else if (mks->type != MOT_KEY_SET_NONE) {
                bool has_tangents = mks->type != MOT_KEY_SET_HERMITE;
                uint16_t keys_count = mks->keys_count;
                mot_key_set_data_type data_type = mks->data_type;
                io_write_uint16_t(&s_motc, keys_count);
                io_write_uint16_t(&s_motc, (uint16_t)data_type);

                uint8_t step = has_tangents ? 2 : 1;
                if (has_tangents) {
                    float_t* values = &mks->values[1];
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        io_write_float_t(&s_motc, *values);
                }

                float_t* values = mks->values.data();
                if (data_type == MOT_KEY_SET_DATA_F16)
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        io_write_half_t(&s_motc, float_to_half(*values));
                else
                    for (int32_t k = 0; k < keys_count; k++, values += step)
                        io_write_float_t(&s_motc, *values);
                io_align_write(&s_motc, 0x04);

                int16_t* frames = (int16_t*)mks->frames.data();
                for (int32_t k = 0; k < keys_count; k++)
                    io_write_int16_t(&s_motc, *frames++);
                io_align_write(&s_motc, 0x04);
            }
        }
        io_align_write(&s_motc, 0x10);

        size_t* bone_info_offsets = force_malloc_s(size_t, m->bone_info_count);
        mh.bone_info_offset = io_get_position(&s_motc);
        if (!ms->is_x)
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_offset_f2_pof_add(&s_motc, 0, 0x40, &pof);
        else
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_offset_x_pof_add(&s_motc, 0, &pof);
        io_align_write(&s_motc, 0x10);

        mh.bone_hash_offset = io_get_position(&s_motc);
        for (int32_t j = 0; j < m->bone_info_count; j++)
            io_write_uint64_t(&s_motc, hash_string_murmurhash(&m->bone_info[j].name, 0, false));
        io_align_write(&s_motc, 0x10);

        mh.name_offset = io_get_position(&s_motc);
        io_write_string_null_terminated(&s_motc, &m->name);

        for (int32_t j = 0; j < m->bone_info_count; j++) {
            bone_info_offsets[j] = io_get_position(&s_motc);
            io_write_string_null_terminated(&s_motc, &m->bone_info[j].name);
        }
        io_align_write(&s_motc, 0x10);

        io_set_position(&s_motc, mh.bone_info_offset, SEEK_SET);
        mh.bone_info_count = m->bone_info_count;
        if (!ms->is_x)
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_offset_f2(&s_motc, bone_info_offsets[j], 0x40);
        else
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_offset_x(&s_motc, bone_info_offsets[j]);
        free(bone_info_offsets);

        io_set_position(&s_motc, 0x00, SEEK_SET);
        if (!ms->is_x) {
            io_write_uint64_t(&s_motc, (uint64_t)mh.hash);
            io_write_offset_f2(&s_motc, mh.name_offset, 0x40);
            io_write_offset_f2(&s_motc, mh.key_set_count_offset, 0x40);
            io_write_offset_f2(&s_motc, mh.key_set_types_offset, 0x40);
            io_write_offset_f2(&s_motc, mh.key_set_offset, 0x40);
            io_write_offset_f2(&s_motc, mh.bone_info_offset, 0x40);
            io_write_offset_f2(&s_motc, mh.bone_hash_offset, 0x40);
            io_write_int32_t(&s_motc, mh.bone_info_count);
        }
        else {
            io_write_uint64_t(&s_motc, (uint64_t)mh.hash);
            io_write_offset_x(&s_motc, mh.name_offset);
            io_write_offset_x(&s_motc, mh.key_set_count_offset);
            io_write_offset_x(&s_motc, mh.key_set_types_offset);
            io_write_offset_x(&s_motc, mh.key_set_offset);
            io_write_offset_x(&s_motc, mh.bone_info_offset);
            io_write_offset_x(&s_motc, mh.bone_hash_offset);
            io_write_int32_t(&s_motc, mh.bone_info_count);
        }

        if (m->div_frames > 0) {
            io_write_uint16_t(&s_motc, m->div_frames);
            io_write_uint8_t(&s_motc, m->div_count);
        }
        else {
            io_write_uint16_t(&s_motc, 0);
            io_write_uint8_t(&s_motc, 0);
        }

        murmurhash = m->murmurhash;
    }

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));

    io_align_write(&s_motc, 0x10);
    io_copy(&s_motc, &st.data, &st.length);
    io_free(&s_motc);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOTC');
    st.header.length = 0x40;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;
    st.header.murmurhash = murmurhash;
    st.header.inner_signature = 0xFF010008;

    f2_struct_write(&st, s, true, ms->is_x);
    f2_struct_free(&st);
}
