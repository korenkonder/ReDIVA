/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.hpp"
#include "../f2/struct.hpp"
#include "../io/path.hpp"
#include "../io/stream.hpp"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void object_database_classic_read_inner(object_database* obj_db, stream& s);
static void object_database_classic_write_inner(object_database* obj_db, stream& s);
static void object_database_modern_read_inner(object_database* obj_db, stream& s, uint32_t header_length);
static void object_database_modern_write_inner(object_database* obj_db, stream& s);
static int64_t object_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, std::string& str);
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
            s.open(path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                stream s_bin;
                s_bin.open(data, s.length);
                object_database_classic_read_inner(this, s_bin);
                free(data);
            }
        }
        free(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            st.read(path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                stream s_mosi;
                s_mosi.open(st.data);
                s_mosi.is_big_endian = st.header.use_big_endian;
                object_database_modern_read_inner(this, s_mosi, st.header.length);
            }
        }
        free(path_osi);
    }
}

void object_database::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            s.open(path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                stream s_bin;
                s_bin.open(data, s.length);
                object_database_classic_read_inner(this, s_bin);
                free(data);
            }
        }
        free(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_add(path, L".osi");
        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            st.read(path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                stream s_mosi;
                s_mosi.open(st.data);
                s_mosi.is_big_endian = st.header.use_big_endian;
                object_database_modern_read_inner(this, s_mosi, st.header.length);
            }
        }
        free(path_osi);
    }
}

void object_database::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        stream s;
        s.open(data, size);
        object_database_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
            stream s_mosi;
            s_mosi.open(st.data);
            s_mosi.is_big_endian = st.header.use_big_endian;
            object_database_modern_read_inner(this, s_mosi, st.header.length);
        }
    }
}

void object_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        s.open(path_bin, "wb");
        if (s.io.stream)
            object_database_classic_write_inner(this, s);
        free(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        stream s;
        s.open(path_osi, "wb");
        if (s.io.stream)
            object_database_modern_write_inner(this, s);
        free(path_osi);
    }
}

void object_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        stream s;
        s.open(path_bin, L"wb");
        if (s.io.stream)
            object_database_classic_write_inner(this, s);
        free(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_add(path, L".osi");
        stream s;
        s.open(path_osi, L"wb");
        if (s.io.stream)
            object_database_modern_write_inner(this, s);
        free(path_osi);
    }
}

void object_database::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
    s.open();
    if (!modern)
        object_database_classic_write_inner(this, s);
    else
        object_database_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

void object_database::merge_mdata(object_database* base_obj_db, object_database* mdata_obj_db) {
    if (!base_obj_db || !mdata_obj_db || !base_obj_db->ready || !mdata_obj_db->ready)
        return;

    if (this != base_obj_db)
        object_set = base_obj_db->object_set;

    for (object_set_info& i : mdata_obj_db->object_set) {
        uint32_t name_hash = i.name_hash;

        object_set_info* info = 0;
        for (object_set_info& j : object_set)
            if (name_hash == j.name_hash) {
                info = &j;
                break;
            }

        if (info)
            *info = i;
        else
            object_set.push_back(i);
    }

    if (this != base_obj_db) {
        ready = true;
        modern = base_obj_db->modern;
        is_x = base_obj_db->is_x;
    }
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

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (object_set_info& i : object_set)
        if (name_hash == i.name_hash) {
            *set_info = &i;
            return true;
        }
    return false;
}

bool object_database::get_object_set_info(uint32_t set_id, object_set_info** set_info) {
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

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (name_hash == j.name_hash_murmurhash) {
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

uint32_t object_database::get_object_set_id(const char* name) {
    if (!name)
        return (uint32_t)-1;

    size_t name_len = utf8_length(name);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return (uint32_t)-1;

    uint32_t name_hash = hash_utf8_murmurhash(name);
    if (name_hash == hash_murmurhash_empty)
        return (uint32_t)-1;

    for (object_set_info& i : object_set)
        if (name_hash == i.name_hash)
            return i.id;

    return (uint32_t)-1;
}

object_info object_database::get_object_info(const char* name) {
    if (!name)
        return object_info();

    size_t name_len = utf8_length(name);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return object_info();

    uint32_t name_hash = hash_utf8_murmurhash(name);
    if (name_hash == hash_murmurhash_empty)
        return object_info();

    for (object_set_info& i : object_set)
        for (object_info_data& j : i.object)
            if (name_hash == j.name_hash_murmurhash)
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

    std::string s = path + std::string(file, file_len);

    object_database* obj_db = (object_database*)data;
    obj_db->read(s.c_str(), obj_db->modern);

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

static void object_database_classic_read_inner(object_database* obj_db, stream& s) {
    uint32_t object_set_count = s.read_uint32_t();
    uint32_t max_object_set_id = s.read_uint32_t();
    uint32_t object_sets_offset = s.read_uint32_t();
    uint32_t object_count = s.read_uint32_t();
    uint32_t objects_offset = s.read_uint32_t();

    obj_db->object_set.resize(object_set_count);

    s.position_push(object_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_set_count; i++) {
        object_set_info* set_info = &obj_db->object_set[i];
        set_info->name = s.read_string_null_terminated_offset(s.read_uint32_t());
        set_info->name_hash = hash_string_murmurhash(&set_info->name);
        set_info->id = s.read_uint32_t();
        set_info->object_file_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        set_info->texture_file_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        set_info->archive_file_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        s.read(0x10);
    }
    s.position_pop();

    s.position_push(objects_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_count; i++) {
        object_info_data info;
        info.id = s.read_uint16_t();
        uint32_t set_id = s.read_uint16_t();
        info.name = s.read_string_null_terminated_offset(s.read_uint32_t());
        info.name_hash_fnv1a64m = hash_string_fnv1a64m(&info.name);
        info.name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info.name, true);
        info.name_hash_murmurhash = hash_string_murmurhash(&info.name);

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
    s.position_pop();

    obj_db->is_x = false;
    obj_db->modern = false;
    obj_db->ready = true;
}

static void object_database_classic_write_inner(object_database* obj_db, stream& s) {
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.align_write(0x20);

    uint32_t object_set_count = (uint32_t)obj_db->object_set.size();

    std::vector<int64_t> string_offsets;
    string_offsets.reserve(object_set_count * 4ULL);

    uint32_t max_object_set_id = 0;
    uint32_t object_count = 0;
    int64_t object_sets_offset = 0;
    size_t off_idx = 0;
    int64_t objects_offset = 0;
    if (string_offsets.size()) {
        for (object_set_info& i : obj_db->object_set) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.name);
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.object_file_name);
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.texture_file_name);
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.archive_file_name);
        }
        s.align_write(0x20);

        object_sets_offset = s.get_position();
        for (object_set_info& i : obj_db->object_set) {
            s.write_uint32_t((uint32_t)string_offsets[off_idx++]);
            s.write_uint32_t(i.id);
            s.write_uint32_t((uint32_t)string_offsets[off_idx++]);
            s.write_uint32_t((uint32_t)string_offsets[off_idx++]);
            s.write_uint32_t((uint32_t)string_offsets[off_idx++]);
            s.write(0x10);
            if (max_object_set_id < i.id)
                max_object_set_id = i.id;
            object_count += (uint32_t)i.object.size();
        }

        string_offsets.clear();
        string_offsets.reserve(object_count);

        for (object_set_info& i : obj_db->object_set)
            for (object_info_data& j : i.object) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }
        s.align_write(0x20);

        objects_offset = s.get_position();
        off_idx = 0;
        for (object_set_info& i : obj_db->object_set) {
            uint16_t set_id = (uint16_t)i.id;
            for (object_info_data& j : i.object) {
                s.write_uint16_t((uint16_t)j.id);
                s.write_uint16_t(set_id);
                s.write_uint32_t((uint32_t)string_offsets[off_idx++]);
            }
        }
        s.align_write(0x20);
    }

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(object_set_count);
    s.write_uint32_t(max_object_set_id);
    s.write_uint32_t((uint32_t)object_sets_offset);
    s.write_uint32_t(object_count);
    s.write_uint32_t((uint32_t)objects_offset);
    s.write(0x08);
    s.position_pop();
}

static void object_database_modern_read_inner(object_database* obj_db, stream& s, uint32_t header_length) {
    bool is_x = true;

    s.set_position(0x0C, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;
    s.set_position(0x14, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    s.read_uint32_t_reverse_endianness();
    uint32_t object_set_count = s.read_uint32_t_reverse_endianness();
    uint32_t max_object_set_id = s.read_uint32_t_reverse_endianness();
    int64_t object_sets_offset = s.read_offset(header_length, is_x);
    uint32_t object_count = s.read_uint32_t_reverse_endianness();
    int64_t objects_offset = s.read_offset(header_length, is_x);

    obj_db->object_set.resize(object_set_count);

    s.position_push(object_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set[i];
            set_info->name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set_info->name_hash = hash_string_murmurhash(&set_info->name);
            set_info->id = s.read_uint32_t_reverse_endianness();
            set_info->object_file_name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set_info->texture_file_name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set_info->archive_file_name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            s.read(0x10);
        }
    else
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set[i];
            set_info->name = s.read_string_null_terminated_offset(s.read_offset_x());
            set_info->name_hash = hash_string_murmurhash(&set_info->name);
            set_info->id = s.read_uint32_t_reverse_endianness();
            set_info->object_file_name = s.read_string_null_terminated_offset(s.read_offset_x());
            set_info->texture_file_name = s.read_string_null_terminated_offset(s.read_offset_x());
            set_info->archive_file_name = s.read_string_null_terminated_offset(s.read_offset_x());
            s.read(0x10);
        }
    s.position_pop();

    s.position_push(objects_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_count; i++) {
            object_info_data info;
            info.id = s.read_uint32_t_reverse_endianness();
            uint32_t set_id = s.read_uint32_t_reverse_endianness();
            info.name =  s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            info.name_hash_fnv1a64m = hash_string_fnv1a64m(&info.name);
            info.name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info.name, true);
            info.name_hash_murmurhash = hash_string_murmurhash(&info.name);

            for (uint32_t i = 0; i < object_set_count; i++)
                if (set_id == obj_db->object_set[i].id) {
                    obj_db->object_set[i].object.push_back(info);
                    break;
                }
        }
    else
        for (uint32_t i = 0; i < object_count; i++) {
            object_info_data info;
            info.id = s.read_uint32_t_reverse_endianness();
            uint32_t set_id = s.read_uint32_t_reverse_endianness();
            info.name = s.read_string_null_terminated_offset(s.read_offset_x());
            info.name_hash_fnv1a64m = hash_string_fnv1a64m(&info.name);
            info.name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info.name, true);
            info.name_hash_murmurhash = hash_string_murmurhash(&info.name);

            for (uint32_t i = 0; i < object_set_count; i++)
                if (set_id == obj_db->object_set[i].id) {
                    obj_db->object_set[i].object.push_back(info);
                    break;
                }
        }
    s.position_pop();

    obj_db->is_x = is_x;
    obj_db->modern = true;
    obj_db->ready = true;
}

static void object_database_modern_write_inner(object_database* obj_db, stream& s) {
    stream s_mosi;
    s_mosi.open();
    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    bool is_x = obj_db->is_x;

    uint32_t object_set_count = (uint32_t)obj_db->object_set.size();

    uint32_t object_count = 0;
    for (object_set_info& i : obj_db->object_set)
        object_count += (uint32_t)i.object.size();

    if (!is_x) {
        ee = { 0, 1, 1, 1 };
        ee.append(0, 6, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 32;

        ee = { off, 1, 36, object_set_count };
        ee.append(0, 5, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(object_set_count * 36ULL);
        off = align_val(off, 0x10);

        ee = { off, 1, 12, object_count };
        ee.append(0, 3, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(object_count * 12ULL);
        off = align_val(off, 0x10);
    }
    else {
        ee = { 0, 4, 48, 1 };
        ee.append(0, 3, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        e.vec.push_back(ee);
        off = 48;

        ee = { off, 3, 56, object_set_count };
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 3, ENRS_QWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(object_set_count * 56ULL);
        off = align_val(off, 0x10);

        ee = { off, 2, 16, object_count };
        ee.append(0, 2, ENRS_DWORD);
        ee.append(0, 1, ENRS_QWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(object_count * 16ULL);
        off = align_val(off, 0x10);
    }

    s_mosi.write_int32_t(0);
    s_mosi.write_int32_t(0);
    s_mosi.write_offset(0, 0, is_x);
    s_mosi.write_int32_t(0);
    s_mosi.write_offset(0, 0, is_x);
    s_mosi.write(0x08);
    s_mosi.align_write(0x10);

    int64_t object_sets_offset = s_mosi.get_position();
    s_mosi.write(object_set_count * (is_x ? 0x38ULL : 0x24ULL));
    s_mosi.align_write(0x10);

    int64_t objects_offset = s_mosi.get_position();
    s_mosi.write(object_count * (is_x ? 0x10ULL : 0x0CULL));
    s_mosi.align_write(0x10);

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve((int64_t)object_set_count + object_count);

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
        string_offsets.push_back(s_mosi.get_position());
        s_mosi.write_string_null_terminated(i);
    }
    s_mosi.align_write(0x10);

    s_mosi.position_push(0x00, SEEK_SET);
    s_mosi.write_uint32_t(0);
    s_mosi.write_uint32_t(object_set_count);
    s_mosi.write_uint32_t(0);
    io_write_offset_pof_add(s_mosi, object_sets_offset, 0x20, is_x, &pof);
    s_mosi.write_uint32_t(object_count);
    io_write_offset_pof_add(s_mosi, objects_offset, 0x20, is_x, &pof);
    s_mosi.write(0x08);
    s_mosi.align_write(0x10);

    if (!is_x)
        for (object_set_info& i : obj_db->object_set) {
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            s_mosi.write_uint32_t(i.id);
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.object_file_name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.texture_file_name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.archive_file_name), 0x20, &pof);
            s_mosi.write(0x10);
        }
    else
        for (object_set_info& i : obj_db->object_set) {
            io_write_offset_x_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            s_mosi.write_uint32_t(i.id);
            io_write_offset_x_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.object_file_name), &pof);
            io_write_offset_x_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.texture_file_name), &pof);
            io_write_offset_x_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.archive_file_name), &pof);
            s_mosi.write(0x10);
        }
    s_mosi.align_write(0x10);

    if (!is_x)
        for (object_set_info& i : obj_db->object_set) {
            uint32_t set_id = i.id;
            for (object_info_data& j : i.object) {
                s_mosi.write_uint32_t(j.id);
                s_mosi.write_uint32_t(set_id);
                io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
            }
        }
    else
        for (object_set_info& i : obj_db->object_set) {
            uint32_t set_id = i.id;
            for (object_info_data& j : i.object) {
                s_mosi.write_uint32_t(j.id);
                s_mosi.write_uint32_t(set_id);
                io_write_offset_x_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
            }
        }
    s_mosi.position_pop();

    f2_struct st;
    s_mosi.align_write(0x10);
    s_mosi.copy(st.data);
    s_mosi.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSI');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    st.write(s, true, is_x);
}

inline static int64_t object_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, std::string& str) {
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
