/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../hash.h"
#include "../sort.h"
#include "../str_utils.h"

static void object_database_classic_read_inner(object_database* obj_db, stream* s);
static void object_database_classic_write_inner(object_database* obj_db, stream* s);
static void object_database_modern_read_inner(object_database* obj_db, stream* s, uint32_t header_length);
static void object_database_modern_write_inner(object_database* obj_db, stream* s);
static ssize_t object_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<ssize_t>& vec_off, std::string& str);
static void object_database_strings_push_back_check(std::vector<std::string>& vec, std::string& str);

object_info::object_info() {
    this->id = (uint32_t)-1;
    this->set_id = (uint32_t)-1;
}

object_info::object_info(uint32_t id, uint32_t set_id) {
    this->id = id;
    this->set_id = set_id;
}

object_database::object_database() : ready(), modern(), is_x() {

}

object_database::~object_database() {

}

void object_database::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                object_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            f2_struct_read(&st, path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                stream s_mosi;
                io_open(&s_mosi, st.data, st.length);
                s_mosi.is_big_endian = st.header.use_big_endian;
                object_database_modern_read_inner(this, &s_mosi, st.header.length);
                io_free(&s_mosi);
            }
            f2_struct_free(&st);
        }
        free(path_osi);
    }
}

void object_database::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                object_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_wadd(path, L".osi");
        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            f2_struct_read(&st, path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                stream s_mosi;
                io_open(&s_mosi, st.data, st.length);
                s_mosi.is_big_endian = st.header.use_big_endian;
                object_database_modern_read_inner(this, &s_mosi, st.header.length);
                io_free(&s_mosi);
            }
            f2_struct_free(&st);
        }
        free(path_osi);
    }
}

void object_database::read(const void* data, size_t length, bool modern) {
    if (!data || !length)
        return;

    if (!modern) {
        stream s;
        io_open(&s, data, length);
        object_database_classic_read_inner(this, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        f2_struct_read(&st, data, length);
        if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
            stream s_mosi;
            io_open(&s_mosi, st.data, st.length);
            s_mosi.is_big_endian = st.header.use_big_endian;
            object_database_modern_read_inner(this, &s_mosi, st.header.length);
            io_free(&s_mosi);
        }
        f2_struct_free(&st);
    }
}

void object_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            object_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        stream s;
        io_open(&s, path_osi, "wb");
        if (s.io.stream)
            object_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_osi);
    }
}

void object_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_open(&s, path_bin, L"wb");
        if (s.io.stream)
            object_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_wadd(path, L".osi");
        stream s;
        io_open(&s, path_osi, L"wb");
        if (s.io.stream)
            object_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_osi);
    }
}

void object_database::write(void** data, size_t* length) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    if (!modern)
        object_database_classic_write_inner(this, &s);
    else
        object_database_modern_write_inner(this, &s);
    io_align_write(&s, 0x10);
    io_copy(&s, data, length);
    io_free(&s);
}

void object_database::merge_mdata(object_database* base_obj_db, object_database* mdata_obj_db) {
    if (!base_obj_db || !mdata_obj_db || !base_obj_db->ready || !mdata_obj_db->ready)
        return;

    bone_name.reserve(base_obj_db->bone_name.size());

    for (std::string& i : base_obj_db->bone_name)
        bone_name.push_back(i);

    std::vector<object_set_info>& base_object_set = base_obj_db->object_set;
    std::vector<object_set_info>& mdata_object_set = mdata_obj_db->object_set;

    int32_t count = (int32_t)base_object_set.size();
    object_set.resize(count);

    for (int32_t i = 0; i < count; i++) {
        object_set_info* b_set_info = &base_object_set[i];
        object_set_info* set_info = &object_set[i];

        set_info->name = b_set_info->name;
        set_info->name_hash = hash_string_fnv1a64m(&set_info->name, false);
        set_info->id = b_set_info->id;
        set_info->object_file_name = b_set_info->object_file_name;
        set_info->texture_file_name = b_set_info->texture_file_name;
        set_info->archive_file_name = b_set_info->archive_file_name;

        int32_t info_count = (int32_t)b_set_info->object.size();
        set_info->object.resize(info_count);

        for (int32_t j = 0; j < info_count; j++) {
            object_info_data* b_info = &b_set_info->object[j];
            object_info_data* info = &set_info->object[j];

            info->id = b_info->id;
            info->name = b_info->name;
            info->name_hash_fnv1a64m = hash_string_fnv1a64m(&info->name, false);
            info->name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info->name, true);
            info->name_hash_murmurhash = hash_string_murmurhash(&info->name, 0, false);
        }
    }

    int32_t mdata_count = (int32_t)mdata_object_set.size();
    for (int32_t i = 0; i < mdata_count; i++) {
        object_set_info* m_set_info = &mdata_object_set[i];

        uint64_t name_hash = m_set_info->name_hash;
        size_t name_len = m_set_info->name.size();

        object_set_info* info_ptr = 0;
        for (object_set_info& j : object_set)
            if (name_hash == j.name_hash) {
                info_ptr = &j;
                break;
            }

        object_set_info set_info;
        set_info.name = m_set_info->name;
        set_info.name_hash = hash_string_fnv1a64m(&set_info.name, false);
        set_info.id = m_set_info->id;
        set_info.object_file_name = m_set_info->object_file_name;
        set_info.texture_file_name = m_set_info->texture_file_name;
        set_info.archive_file_name = m_set_info->archive_file_name;

        int32_t info_count = (int32_t)m_set_info->object.size();
        set_info.object.resize(info_count);

        for (int32_t j = 0; j < info_count; j++) {
            object_info_data* m_info = &m_set_info->object[j];
            object_info_data* info = &set_info.object[j];

            info->id = m_info->id;
            info->name = m_info->name;
            info->name_hash_fnv1a64m = hash_string_fnv1a64m(&info->name, false);
            info->name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info->name, true);
            info->name_hash_murmurhash = hash_string_murmurhash(&info->name, 0, false);
        }

        if (info_ptr)
            *info_ptr = set_info;
        else
            object_set.push_back(set_info);
    }

    ready = true;
}

void object_database::split_mdata(object_database* base_obj_db, object_database* mdata_obj_db) {
    if (!base_obj_db || !mdata_obj_db || !ready || !base_obj_db->ready)
        return;
}

bool object_database::get_object_set_info(const char* name, object_set_info** set_info) {
    if (!set_info)
        return false;
    *set_info = 0;

    if (!name)
        return false;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (object_set_info& i : object_set)
        if (name_hash == i.name_hash) {
            *set_info = &i;
            return true;
        }
    return false;
}

bool object_database::get_object_set_info_by_set_id(uint32_t set_id, object_set_info** set_info) {
    if (!set_info)
        return false;
    *set_info = 0;

    if (set_id == -1)
        return false;

    for (object_set_info& i : object_set)
        if (set_id == i.id) {
            *set_info = &i;
            return true;
        }
    return false;
}

bool object_database::get_object_info_data(const char* name, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (!name)
        return false;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (name_hash == j.name_hash_fnv1a64m) {
                *info = &j;
                return true;
            }
    return false;
}

bool object_database::get_object_info_data_by_fnv1a64m_hash(uint64_t hash, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (hash == hash_fnv1a64m_empty)
        return false;

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (hash == j.name_hash_fnv1a64m) {
                *info = &j;
                return true;
            }
    return false;
}

bool object_database::get_object_info_data_by_fnv1a64m_hash_upper(uint64_t hash, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (hash == hash_fnv1a64m_empty)
        return false;

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (hash == j.name_hash_fnv1a64m_upper) {
                *info = &j;
                return true;
            }
    return false;
}

bool object_database::get_object_info_data_by_murmurhash(uint32_t hash, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (hash == hash_murmurhash_empty)
        return false;

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (hash == j.name_hash_murmurhash) {
                *info = &j;
                return true;
            }
    return false;
}

object_info object_database::get_object_info(const char* name) {
    if (!name)
        return object_info();

    size_t name_len = utf8_length(name);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return object_info();

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);
    if (name_hash == hash_murmurhash_empty)
        return object_info();

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (name_hash == j.name_hash_fnv1a64m)
                return { j.id, i.id };

    return object_info();
}

const char* object_database::get_object_name(object_info obj_info) {
    if (obj_info.is_null())
        return 0;

    for (object_set_info& i : object_set)
        if (obj_info.set_id == i.id)
            for (object_info_data& j : i.object)
                if (obj_info.id == j.id)
                    return j.name.c_str();

    return 0;
}

bool object_database::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    object_database* obj_db = (object_database*)data;
    obj_db->read(string_data(&s), obj_db->modern);

    string_free(&s);
    return obj_db->ready;
}

object_set_info::object_set_info() : name_hash(), id() {

}

object_set_info::~object_set_info() {

}

object_info_data::object_info_data() : id(), name_hash_fnv1a64m(),
name_hash_fnv1a64m_upper(), name_hash_murmurhash() {
}

object_info_data::~object_info_data() {

}

static void object_database_classic_read_inner(object_database* obj_db, stream* s) {
    uint32_t object_set_count = io_read_uint32_t(s);
    uint32_t max_object_set_id = io_read_uint32_t(s);
    uint32_t object_sets_offset = io_read_uint32_t(s);
    uint32_t object_count = io_read_uint32_t(s);
    uint32_t objects_offset = io_read_uint32_t(s);

    obj_db->object_set.resize(object_set_count);

    io_position_push(s, object_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_set_count; i++) {
        object_set_info* set_info = &obj_db->object_set[i];
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->name);
        set_info->id = io_read_uint32_t(s);
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->object_file_name);
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->texture_file_name);
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->archive_file_name);
        io_read(s, 0x10);
    }
    io_position_pop(s);

    io_position_push(s, objects_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_count; i++) {
        object_info_data info;
        info.id = io_read_uint16_t(s);
        uint32_t set_id = io_read_uint16_t(s);
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &info.name);

        if (info.id == 0xFFFF)
            info.id = -1;
        if (set_id == 0xFFFF)
            set_id = -1;

        for (uint32_t i = 0; i < object_set_count; i++)
            if (set_id == obj_db->object_set[i].id) {
                obj_db->object_set[i].object.push_back(info);
                break;
            }
    }
    io_position_pop(s);

    obj_db->is_x = false;
    obj_db->modern = false;
    obj_db->ready = true;
}

static void object_database_classic_write_inner(object_database* obj_db, stream* s) {
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_align_write(s, 0x20);

    uint32_t object_set_count = (uint32_t)obj_db->object_set.size();

    std::vector<ssize_t> string_offsets;
    string_offsets.reserve(object_set_count * 4ULL);

    uint32_t max_object_set_id = 0;
    uint32_t object_count = 0;
    ssize_t object_sets_offset = 0;
    size_t off_idx = 0;
    ssize_t objects_offset = 0;
    if (string_offsets.size()) {
        for (object_set_info& i : obj_db->object_set) {
            string_offsets.push_back(io_get_position(s));
            io_write_string_null_terminated(s, &i.name);
            string_offsets.push_back(io_get_position(s));
            io_write_string_null_terminated(s, &i.object_file_name);
            string_offsets.push_back(io_get_position(s));
            io_write_string_null_terminated(s, &i.texture_file_name);
            string_offsets.push_back(io_get_position(s));
            io_write_string_null_terminated(s, &i.archive_file_name);
        }
        io_align_write(s, 0x20);

        object_sets_offset = io_get_position(s);
        for (object_set_info& i : obj_db->object_set) {
            io_write_uint32_t(s, (uint32_t)string_offsets[off_idx++]);
            io_write_uint32_t(s, i.id);
            io_write_uint32_t(s, (uint32_t)string_offsets[off_idx++]);
            io_write_uint32_t(s, (uint32_t)string_offsets[off_idx++]);
            io_write_uint32_t(s, (uint32_t)string_offsets[off_idx++]);
            io_write(s, 0x10);
            if (max_object_set_id < i.id)
                max_object_set_id = i.id;
            object_count += (uint32_t)i.object.size();
        }

        string_offsets.clear();
        string_offsets.reserve(object_count);

        for (object_set_info& i : obj_db->object_set)
            for (object_info_data& j : i.object) {
                string_offsets.push_back(io_get_position(s));
                io_write_string_null_terminated(s, &j.name);
            }
        io_align_write(s, 0x20);

        objects_offset = io_get_position(s);
        off_idx = 0;
        for (object_set_info& i : obj_db->object_set) {
            uint16_t set_id = (uint16_t)i.id;
            for (object_info_data& j : i.object) {
                io_write_uint16_t(s, (uint16_t)j.id);
                io_write_uint16_t(s, set_id);
                io_write_uint32_t(s, (uint32_t)string_offsets[off_idx++]);
            }
        }
        io_align_write(s, 0x20);
    }

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, object_set_count);
    io_write_uint32_t(s, max_object_set_id);
    io_write_uint32_t(s, (uint32_t)object_sets_offset);
    io_write_uint32_t(s, object_count);
    io_write_uint32_t(s, (uint32_t)objects_offset);
    io_write(s, 0x08);
    io_position_pop(s);
}

static void object_database_modern_read_inner(object_database* obj_db, stream* s, uint32_t header_length) {
    bool is_x = true;

    io_set_position(s, 0x0C, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;
    io_set_position(s, 0x14, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;

    io_set_position(s, 0x00, SEEK_SET);

    io_read_uint32_t_stream_reverse_endianness(s);
    uint32_t object_set_count = io_read_uint32_t_stream_reverse_endianness(s);
    uint32_t max_object_set_id = io_read_uint32_t_stream_reverse_endianness(s);
    ssize_t object_sets_offset = io_read_offset(s, header_length, is_x);
    uint32_t object_count = io_read_uint32_t_stream_reverse_endianness(s);
    ssize_t objects_offset = io_read_offset(s, header_length, is_x);

    obj_db->object_set.resize(object_set_count);

    io_position_push(s, object_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set[i];
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->name);
            set_info->id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->object_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->texture_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->archive_file_name);
            io_read(s, 0x10);
        }
    else
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set[i];
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->name);
            set_info->id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->object_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->texture_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->archive_file_name);
            io_read(s, 0x10);
        }
    io_position_pop(s);

    io_position_push(s, objects_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_count; i++) {
            object_info_data info;
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            uint32_t set_id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &info.name);

            for (uint32_t i = 0; i < object_set_count; i++)
                if (set_id != obj_db->object_set[i].id) {
                    obj_db->object_set[i].object.push_back(info);
                    break;
                }
        }
    else
        for (uint32_t i = 0; i < object_count; i++) {
            object_info_data info;
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            uint32_t set_id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &info.name);

            for (uint32_t i = 0; i < object_set_count; i++)
                if (set_id != obj_db->object_set[i].id) {
                    obj_db->object_set[i].object.push_back(info);
                    break;
                }
        }
    io_position_pop(s);

    obj_db->is_x = is_x;
    obj_db->modern = true;
    obj_db->ready = true;
}

static void object_database_modern_write_inner(object_database* obj_db, stream* s) {
    stream s_mosi;
    io_open(&s_mosi);
    uint32_t off;
    vector_old_enrs_entry e = vector_old_empty(enrs_entry);
    enrs_entry ee;
    vector_old_size_t pof = vector_old_empty(size_t);

    bool is_x = obj_db->is_x;

    uint32_t object_set_count = (uint32_t)obj_db->object_set.size();

    uint32_t object_count = 0;
    for (object_set_info& i : obj_db->object_set)
        object_count += (uint32_t)i.object.size();

    if (!is_x) {
        ee = { 0, 1, 1, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 6, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = 32;

        ee = { off, 1, 36, object_set_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 5, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_set_count * 36ULL);
        off = align_val(off, 0x10);

        ee = { off, 1, 12, object_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 3, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_count * 12ULL);
        off = align_val(off, 0x10);
    }
    else {
        ee = { 0, 4, 48, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 3, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 1, ENRS_QWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 1, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = 48;

        ee = { off, 3, 56, object_set_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_QWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 3, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_set_count * 56ULL);
        off = align_val(off, 0x10);

        ee = { off, 2, 16, object_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_count * 16ULL);
        off = align_val(off, 0x10);
    }

    io_write_int32_t(&s_mosi, 0);
    io_write_int32_t(&s_mosi, 0);
    io_write_offset(&s_mosi, 0, 0, is_x);
    io_write_int32_t(&s_mosi, 0);
    io_write_offset(&s_mosi, 0, 0, is_x);
    io_write(&s_mosi, 0x08);
    io_align_write(&s_mosi, 0x10);

    ssize_t object_sets_offset = io_get_position(&s_mosi);
    io_write(&s_mosi, object_set_count * (is_x ? 0x38ULL : 0x24ULL));
    io_align_write(&s_mosi, 0x10);

    ssize_t objects_offset = io_get_position(&s_mosi);
    io_write(&s_mosi, object_count * (is_x ? 0x10ULL : 0x0CULL));
    io_align_write(&s_mosi, 0x10);

    std::vector<std::string> strings;
    std::vector<ssize_t> string_offsets;

    strings.reserve((ssize_t)object_set_count + object_count);

    for (object_set_info& i : obj_db->object_set) {
        object_database_strings_push_back_check(strings, i.name);
        object_database_strings_push_back_check(strings, i.object_file_name);
        object_database_strings_push_back_check(strings, i.texture_file_name);
        object_database_strings_push_back_check(strings, i.archive_file_name);
        for (object_info_data& j : i.object)
            object_database_strings_push_back_check(strings, j.name);
    }

    quicksort_string(strings.data(), strings.size());
    string_offsets.reserve(strings.size());
    for (std::string& i : strings) {
        string_offsets.push_back(io_get_position(&s_mosi));
        io_write_string_null_terminated(&s_mosi, &i);
    }
    io_align_write(&s_mosi, 0x10);

    io_position_push(&s_mosi, 0x00, SEEK_SET);
    io_write_uint32_t(&s_mosi, 0);
    io_write_uint32_t(&s_mosi, object_set_count);
    io_write_uint32_t(&s_mosi, 0);
    io_write_offset_pof_add(&s_mosi, object_sets_offset, 0x20, is_x, &pof);
    io_write_uint32_t(&s_mosi, object_count);
    io_write_offset_pof_add(&s_mosi, objects_offset, 0x20, is_x, &pof);
    io_write(&s_mosi, 0x08);
    io_align_write(&s_mosi, 0x10);

    if (!is_x)
        for (object_set_info& i : obj_db->object_set) {
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            io_write_uint32_t(&s_mosi, i.id);
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.object_file_name), 0x20, &pof);
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.texture_file_name), 0x20, &pof);
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.archive_file_name), 0x20, &pof);
            io_write(&s_mosi, 0x10);
        }
    else
        for (object_set_info& i : obj_db->object_set) {
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            io_write_uint32_t(&s_mosi, i.id);
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.object_file_name), &pof);
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.texture_file_name), &pof);
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.archive_file_name), &pof);
            io_write(&s_mosi, 0x10);
        }
    io_align_write(&s_mosi, 0x10);

    if (!is_x)
        for (object_set_info& i : obj_db->object_set) {
            uint32_t set_id = i.id;
            for (object_info_data& j : i.object) {
                io_write_uint32_t(&s_mosi, j.id);
                io_write_uint32_t(&s_mosi, set_id);
                io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
            }
        }
    else
        for (object_set_info& i : obj_db->object_set) {
            uint32_t set_id = i.id;
            for (object_info_data& j : i.object) {
                io_write_uint32_t(&s_mosi, j.id);
                io_write_uint32_t(&s_mosi, set_id);
                io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
            }
        }
    io_position_pop(&s_mosi);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    io_align_write(&s_mosi, 0x10);
    io_copy(&s_mosi, &st.data, &st.length);
    io_free(&s_mosi);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSI');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    f2_struct_write(&st, s, true, is_x);
    f2_struct_free(&st);
}

inline static ssize_t object_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<ssize_t>& vec_off, std::string& str) {
    for (std::string& i : vec)
        if (str == i)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static void object_database_strings_push_back_check(std::vector<std::string>& vec, std::string& str) {
    for (std::string& i : vec)
        if (str == i)
            return;

    vec.push_back(str);
}
