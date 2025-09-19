/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void object_database_file_classic_read_inner(object_database_file* obj_db, stream& s);
static void object_database_file_classic_write_inner(object_database_file* obj_db, stream& s);
static void object_database_file_modern_read_inner(object_database_file* obj_db, stream& s, uint32_t header_length);
static void object_database_file_modern_write_inner(object_database_file* obj_db, stream& s);
static int64_t object_database_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str);
static bool object_database_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str);

object_set_info_file::object_set_info_file() : id() {

}

object_set_info_file::~object_set_info_file() {

}

object_info_data_file::object_info_data_file() : id() {
}

object_info_data_file::~object_info_data_file() {

}

object_set_info::object_set_info() : name_hash(), id() {
    name_hash = hash_murmurhash_empty;
}

object_set_info::~object_set_info() {

}

object_info_data::object_info_data() : id() {
    name_hash_fnv1a64m = hash_fnv1a64m_empty;
    name_hash_fnv1a64m_upper = hash_fnv1a64m_empty;
    name_hash_murmurhash = hash_murmurhash_empty;
}

object_info_data::~object_info_data() {

}

object_database_file::object_database_file() : ready(), modern(), big_endian(), is_x() {

}

object_database_file::~object_database_file() {

}

void object_database_file::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (!path_bin)
            return;

        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, "rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc<uint8_t>(s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                object_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        if (!path_osi)
            return;

        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            st.read(path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                memory_stream s_mosi;
                s_mosi.open(st.data);
                s_mosi.big_endian = st.header.attrib.get_big_endian();
                object_database_file_modern_read_inner(this, s_mosi, st.header.get_length());
            }
        }
        free_def(path_osi);
    }
}

void object_database_file::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (!path_bin)
            return;

        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, L"rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc<uint8_t>(s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                object_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_add(path, L".osi");
        if (!path_osi)
            return;

        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            st.read(path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                memory_stream s_mosi;
                s_mosi.open(st.data);
                s_mosi.big_endian = st.header.attrib.get_big_endian();
                object_database_file_modern_read_inner(this, s_mosi, st.header.get_length());
            }
        }
        free_def(path_osi);
    }
}

void object_database_file::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        object_database_file_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
            memory_stream s_mosi;
            s_mosi.open(st.data);
            s_mosi.big_endian = st.header.attrib.get_big_endian();
            object_database_file_modern_read_inner(this, s_mosi, st.header.get_length());
        }
    }
}

void object_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (!path_bin)
            return;

        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            object_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        if (!path_osi)
            return;

        file_stream s;
        s.open(path_osi, "wb");
        if (s.check_not_null())
            object_database_file_modern_write_inner(this, s);
        free_def(path_osi);
    }
}

void object_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (!path_bin)
            return;

        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            object_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_add(path, L".osi");
        if (!path_osi)
            return;

        file_stream s;
        s.open(path_osi, L"wb");
        if (s.check_not_null())
            object_database_file_modern_write_inner(this, s);
        free_def(path_osi);
    }
}

void object_database_file::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        object_database_file_classic_write_inner(this, s);
    else
        object_database_file_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool object_database_file::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    object_database_file* obj_db = (object_database_file*)data;
    obj_db->read(path.c_str(), obj_db->modern);

    return obj_db->ready;
}

object_database::object_database() {

}

object_database::~object_database() {

}

void object_database::add(object_database_file* obj_db_file) {
    if (!obj_db_file || !obj_db_file->ready)
        return;

    object_set.reserve(obj_db_file->object_set.size());

    for (object_set_info_file& i : obj_db_file->object_set) {
        uint32_t name_hash = hash_string_murmurhash(i.name);

        object_set_info* set_info = 0;
        for (object_set_info& j : object_set)
            if (name_hash == j.name_hash) {
                set_info = &j;
                break;
            }

        if (!set_info) {
            object_set.push_back({});
            set_info = &object_set.back();
        }

        set_info->id = i.id;
        set_info->name.assign(i.name);
        set_info->name_hash = hash_string_murmurhash(i.name);
        set_info->object_file_name.assign(i.object_file_name);
        set_info->texture_file_name.assign(i.texture_file_name);
        set_info->archive_file_name.assign(i.archive_file_name);

        set_info->object.clear();
        set_info->object.reserve(i.object.size());

        for (object_info_data_file& j : i.object) {
            set_info->object.push_back({});
            object_info_data& info = set_info->object.back();
            info.id = j.id;
            info.name.assign(j.name);
            info.name_hash_fnv1a64m = hash_string_fnv1a64m(info.name);
            info.name_hash_fnv1a64m_upper = hash_string_fnv1a64m(info.name, true);
            info.name_hash_murmurhash = hash_string_murmurhash(info.name);
        }
    }

    update();
}

void object_database::clear() {
    object_set.clear();
    object_set.shrink_to_fit();
    obj_set_ids.clear();
    obj_set_ids.shrink_to_fit();
    obj_set_murmurhashes.clear();
    obj_set_murmurhashes.shrink_to_fit();
    obj_set_names.clear();
    obj_set_names.shrink_to_fit();
    obj_infos.clear();
    obj_infos.shrink_to_fit();
    obj_fnv1a64m_hashes.clear();
    obj_fnv1a64m_hashes.shrink_to_fit();
    obj_fnv1a64m_hashes_upper.clear();
    obj_fnv1a64m_hashes_upper.shrink_to_fit();
    obj_murmurhashes.clear();
    obj_murmurhashes.shrink_to_fit();
    obj_info_fnv1a64m_hashes.clear();
    obj_info_fnv1a64m_hashes.shrink_to_fit();
    obj_info_fnv1a64m_hashes_upper.clear();
    obj_info_fnv1a64m_hashes_upper.shrink_to_fit();
    obj_info_murmurhashes.clear();
    obj_info_murmurhashes.shrink_to_fit();
}

void object_database::update() {
    obj_set_ids.clear();
    obj_set_murmurhashes.clear();
    obj_set_names.clear();
    obj_infos.clear();
    obj_fnv1a64m_hashes.clear();
    obj_fnv1a64m_hashes_upper.clear();
    obj_murmurhashes.clear();
    obj_info_fnv1a64m_hashes.clear();
    obj_info_fnv1a64m_hashes_upper.clear();
    obj_info_murmurhashes.clear();

    obj_set_ids.reserve(object_set.size());
    obj_set_murmurhashes.reserve(object_set.size());
    obj_set_names.reserve(object_set.size());

    for (object_set_info& i : object_set) {
        obj_set_ids.push_back(i.id, &i);
        obj_set_murmurhashes.push_back(i.name_hash, &i);
        obj_set_names.push_back(i.name, &i);

        obj_infos.reserve(i.object.size());
        obj_fnv1a64m_hashes.reserve(i.object.size());
        obj_fnv1a64m_hashes_upper.reserve(i.object.size());
        obj_murmurhashes.reserve(i.object.size());
        obj_info_fnv1a64m_hashes.reserve(i.object.size());
        obj_info_fnv1a64m_hashes_upper.reserve(i.object.size());
        obj_info_murmurhashes.reserve(i.object.size());

        for (object_info_data& j : i.object) {
            obj_infos.push_back({ j.id, i.id }, &j);
            obj_fnv1a64m_hashes.push_back(j.name_hash_fnv1a64m, &j);
            obj_fnv1a64m_hashes_upper.push_back(j.name_hash_fnv1a64m_upper, &j);
            obj_murmurhashes.push_back(j.name_hash_murmurhash, &j);
            obj_info_fnv1a64m_hashes.push_back(j.name_hash_fnv1a64m, { j.id, i.id });
            obj_info_fnv1a64m_hashes_upper.push_back(j.name_hash_fnv1a64m_upper, { j.id, i.id });
            obj_info_murmurhashes.push_back(j.name_hash_murmurhash, { j.id, i.id });
        }
    }

    obj_set_ids.sort_unique();
    obj_set_murmurhashes.sort_unique();
    obj_set_names.sort_unique();
    obj_infos.sort_unique();
    obj_fnv1a64m_hashes.sort_unique();
    obj_fnv1a64m_hashes_upper.sort_unique();
    obj_murmurhashes.sort_unique();
    obj_info_fnv1a64m_hashes.sort_unique();
    obj_info_fnv1a64m_hashes_upper.sort_unique();
    obj_info_murmurhashes.sort_unique();
}

const object_set_info* object_database::get_object_set_info(const char* name) const {
    if (!name || !*name)
        return 0;

    auto elem = obj_set_murmurhashes.find(hash_utf8_murmurhash(name));
    if (elem != obj_set_murmurhashes.end())
        return elem->second;
    return 0;
}

const object_set_info* object_database::get_object_set_info(uint32_t set_id) const {
    if (set_id == -1 || set_id == hash_murmurhash_empty || set_id == hash_murmurhash_null)
        return 0;

    auto elem = obj_set_ids.find(set_id);
    if (elem != obj_set_ids.end())
        return elem->second;
    return 0;
}

const object_info_data* object_database::get_object_info_data(const char* name) const {
    if (!name || !*name)
        return 0;

    size_t name_len = utf8_length(name);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return 0;

    auto elem = obj_murmurhashes.find(hash_utf8_murmurhash(name));
    if (elem != obj_murmurhashes.end())
        return elem->second;
    return 0;
}

const object_info_data* object_database::get_object_info_data_by_fnv1a64m_hash(uint64_t hash) const {
    if (hash == -1 || hash == hash_fnv1a64m_empty)
        return 0;

    auto elem = obj_fnv1a64m_hashes.find(hash);
    if (elem != obj_fnv1a64m_hashes.end())
        return elem->second;
    return 0;
}

const object_info_data* object_database::get_object_info_data_by_fnv1a64m_hash_upper(uint64_t hash) const {
    if (hash == -1 || hash == hash_fnv1a64m_empty)
        return 0;

    auto elem = obj_fnv1a64m_hashes_upper.find(hash);
    if (elem != obj_fnv1a64m_hashes_upper.end())
        return elem->second;
    return 0;
}

const object_info_data* object_database::get_object_info_data_by_murmurhash(uint32_t hash) const {
    if (hash == -1 || hash == hash_murmurhash_empty || hash == hash_murmurhash_null)
        return 0;

    auto elem = obj_murmurhashes.find(hash);
    if (elem != obj_murmurhashes.end())
        return elem->second;
    return 0;
}

uint32_t object_database::get_object_set_id(const char* name) const {
    if (!name || !*name)
        return (uint32_t)-1;

    size_t name_len = utf8_length(name);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return (uint32_t)-1;

    auto elem = obj_set_murmurhashes.find(hash_utf8_murmurhash(name));
    if (elem != obj_set_murmurhashes.end())
        return elem->second->id;
    return (uint32_t)-1;
}

const char* object_database::get_object_set_name(uint32_t set_id) const {
    if (set_id == -1)
        return 0;

    auto elem = obj_set_ids.find(set_id);
    if (elem != obj_set_ids.end())
        return elem->second->name.c_str();
    return 0;
}

object_info object_database::get_object_info(const char* name) const {
    if (!name)
        return object_info();

    size_t name_len = utf8_length(name);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return object_info();

    auto elem = obj_info_murmurhashes.find(hash_utf8_murmurhash(name));
    if (elem != obj_info_murmurhashes.end())
        return elem->second;
    return object_info();
}

object_info object_database::get_object_info_by_fnv1a64m_hash(uint64_t hash) const {
    auto elem = obj_info_fnv1a64m_hashes.find(hash);
    if (elem != obj_info_fnv1a64m_hashes.end())
        return elem->second;
    return object_info();
}

object_info object_database::get_object_info_by_fnv1a64m_hash_upper(uint64_t hash) const {
    auto elem = obj_info_fnv1a64m_hashes_upper.find(hash);
    if (elem != obj_info_fnv1a64m_hashes_upper.end())
        return elem->second;
    return object_info();
}

object_info object_database::get_object_info_by_murmurhash(uint32_t hash) const {
    auto elem = obj_info_murmurhashes.find(hash);
    if (elem != obj_info_murmurhashes.end())
        return elem->second;
    return object_info();
}

const char* object_database::get_object_name(object_info obj_info) const {
    if (obj_info.is_null())
        return 0;

    auto elem = obj_infos.find(obj_info);
    if (elem != obj_infos.end())
        return elem->second->name.c_str();
    return 0;
}

uint32_t object_database::get_object_set_count() const {
    return (uint32_t)object_set.size();
}

uint32_t object_database::get_object_set_id(uint32_t set_index) const {
    return set_index < obj_set_names.size() ? (uint32_t)obj_set_names.data()[set_index].second->id : -1;
}

static void object_database_file_classic_read_inner(object_database_file* obj_db, stream& s) {
    uint32_t object_set_count = s.read_uint32_t();
    uint32_t max_object_set_id = s.read_uint32_t();
    uint32_t object_sets_offset = s.read_uint32_t();
    uint32_t object_count = s.read_uint32_t();
    uint32_t objects_offset = s.read_uint32_t();

    obj_db->object_set.resize(object_set_count);

    object_set_info_file* object_set = obj_db->object_set.data();

    s.position_push(object_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_set_count; i++) {
        object_set_info_file* set_info = &object_set[i];
        set_info->name.assign(s.read_string_null_terminated_offset(s.read_uint32_t()));
        set_info->id = s.read_uint32_t();
        set_info->object_file_name.assign(s.read_string_null_terminated_offset(s.read_uint32_t()));
        set_info->texture_file_name.assign(s.read_string_null_terminated_offset(s.read_uint32_t()));
        set_info->archive_file_name.assign(s.read_string_null_terminated_offset(s.read_uint32_t()));
        s.read(0x10);
    }
    s.position_pop();

    size_t* object_set_object_count = force_malloc<size_t>(object_set_count);
    s.position_push(objects_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_count; i++) {
        uint32_t id = s.read_uint16_t();
        uint32_t set_id = s.read_uint16_t();
        uint32_t name_offset = s.read_uint32_t();

        for (uint32_t j = 0; j < object_set_count; j++)
            if (set_id == object_set[j].id) {
                object_set_object_count[j]++;
                break;
            }
    }
    s.position_pop();

    for (uint32_t i = 0; i < object_set_count; i++)
        object_set[i].object.reserve(object_set_object_count[i]);
    free_def(object_set_object_count);

    s.position_push(objects_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_count; i++) {
        uint32_t id = s.read_uint16_t();
        uint32_t set_id = s.read_uint16_t();
        uint32_t name_offset = s.read_uint32_t();

        if (id == 0xFFFF)
            id = -1;
        if (set_id == 0xFFFF)
            set_id = -1;

        for (uint32_t j = 0; j < object_set_count; j++)
            if (set_id == object_set[j].id) {
                object_set[j].object.push_back({});
                object_info_data_file& info = object_set[j].object.back();
                info.id = id;
                info.name.assign(s.read_string_null_terminated_offset(name_offset));
                break;
            }
    }
    s.position_pop();

    obj_db->ready = true;
    obj_db->modern = false;
    obj_db->big_endian = false;
    obj_db->is_x = false;
}

static void object_database_file_classic_write_inner(object_database_file* obj_db, stream& s) {
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);

    uint32_t object_set_count = (uint32_t)obj_db->object_set.size();

    std::vector<int64_t> string_offsets;

    string_offsets.reserve((int64_t)object_set_count * 4);

    uint32_t max_object_set_id = 0;
    uint32_t object_count = 0;
    int64_t object_sets_offset = 0;
    size_t off_idx = 0;
    int64_t objects_offset = 0;
    if (object_set_count) {
        for (object_set_info_file& i : obj_db->object_set) {
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
        for (object_set_info_file& i : obj_db->object_set) {
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

        for (object_set_info_file& i : obj_db->object_set)
            for (object_info_data_file& j : i.object) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }
        s.align_write(0x20);

        objects_offset = s.get_position();
        off_idx = 0;
        for (object_set_info_file& i : obj_db->object_set) {
            uint16_t set_id = (uint16_t)i.id;
            for (object_info_data_file& j : i.object) {
                s.write_uint32_t((uint32_t)((set_id << 16) | (uint16_t)j.id));
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

static void object_database_file_modern_read_inner(object_database_file* obj_db, stream& s, uint32_t header_length) {
    bool big_endian = s.big_endian;
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

    object_set_info_file* object_set = obj_db->object_set.data();

    s.position_push(object_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info_file* set_info = &object_set[i];
            set_info->name.assign(s.read_string_null_terminated_offset(s.read_offset_f2(header_length)));
            set_info->id = s.read_uint32_t_reverse_endianness();
            set_info->object_file_name.assign(s.read_string_null_terminated_offset(s.read_offset_f2(header_length)));
            set_info->texture_file_name.assign(s.read_string_null_terminated_offset(s.read_offset_f2(header_length)));
            set_info->archive_file_name.assign(s.read_string_null_terminated_offset(s.read_offset_f2(header_length)));
            s.read(0x10);
        }
    else
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info_file* set_info = &object_set[i];
            set_info->name.assign(s.read_string_null_terminated_offset(s.read_offset_x()));
            set_info->id = s.read_uint32_t_reverse_endianness();
            set_info->object_file_name.assign(s.read_string_null_terminated_offset(s.read_offset_x()));
            set_info->texture_file_name.assign(s.read_string_null_terminated_offset(s.read_offset_x()));
            set_info->archive_file_name.assign(s.read_string_null_terminated_offset(s.read_offset_x()));
            s.read(0x10);
        }
    s.position_pop();

    size_t* object_set_object_count = force_malloc<size_t>(object_set_count);
    s.position_push(objects_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_count; i++) {
            uint32_t id = s.read_uint16_t();
            uint32_t set_id = s.read_uint16_t();
            int64_t name_offset = s.read_offset_f2(header_length);

            for (uint32_t j = 0; j < object_set_count; j++)
                if (set_id == object_set[j].id) {
                    object_set_object_count[j]++;
                    break;
                }
        }
    else
        for (uint32_t i = 0; i < object_count; i++) {
            uint32_t id = s.read_uint16_t();
            uint32_t set_id = s.read_uint16_t();
            int64_t name_offset = s.read_offset_x();

            for (uint32_t j = 0; j < object_set_count; j++)
                if (set_id == object_set[j].id) {
                    object_set_object_count[j]++;
                    break;
                }
        }
    s.position_pop();

    for (uint32_t i = 0; i < object_set_count; i++)
        object_set[i].object.reserve(object_set_object_count[i]);
    free_def(object_set_object_count);

    s.position_push(objects_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            uint32_t set_id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_f2(header_length);

            for (uint32_t j = 0; j < object_set_count; j++)
                if (set_id == object_set[j].id) {
                    object_set[j].object.push_back({});
                    object_info_data_file& info = object_set[j].object.back();
                    info.id = id;
                    info.name.assign(s.read_string_null_terminated_offset(name_offset));
                    break;
                }
        }
    else
        for (uint32_t i = 0; i < object_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            uint32_t set_id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_x();

            for (uint32_t j = 0; j < object_set_count; j++)
                if (set_id == object_set[j].id) {
                    object_set[j].object.push_back({});
                    object_info_data_file& info = object_set[j].object.back();
                    info.id = id;
                    info.name.assign(s.read_string_null_terminated_offset(name_offset));
                    break;
                }
        }
    s.position_pop();

    obj_db->ready = true;
    obj_db->modern = true;
    obj_db->big_endian = big_endian;
    obj_db->is_x = is_x;
}

static void object_database_file_modern_write_inner(object_database_file* obj_db, stream& s) {
    bool big_endian = obj_db->big_endian;
    bool is_x = obj_db->is_x;

    memory_stream s_mosi;
    s_mosi.open();
    s_mosi.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    uint32_t object_set_count = (uint32_t)obj_db->object_set.size();

    uint32_t object_count = 0;
    for (object_set_info_file& i : obj_db->object_set)
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

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve((int64_t)object_set_count + object_count);
    string_offsets.reserve((int64_t)object_set_count + object_count);

    for (object_set_info_file& i : obj_db->object_set) {
        object_database_strings_push_back_check(strings, i.name);
        object_database_strings_push_back_check(strings, i.object_file_name);
        object_database_strings_push_back_check(strings, i.texture_file_name);
        object_database_strings_push_back_check(strings, i.archive_file_name);
        for (object_info_data_file& j : i.object)
            object_database_strings_push_back_check(strings, j.name);
    }

    quicksort_string_hash(strings.data(), strings.size());
    string_offsets.reserve(strings.size());
    for (string_hash& i : strings) {
        string_offsets.push_back(s_mosi.get_position());
        s_mosi.write_string_null_terminated(i.str);
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
        for (object_set_info_file& i : obj_db->object_set) {
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            s_mosi.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.object_file_name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.texture_file_name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.archive_file_name), 0x20, &pof);
            s_mosi.write(0x10);
        }
    else
        for (object_set_info_file& i : obj_db->object_set) {
            io_write_offset_x_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            s_mosi.write_uint32_t_reverse_endianness(i.id);
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
        for (object_set_info_file& i : obj_db->object_set) {
            uint32_t set_id = i.id;
            for (object_info_data_file& j : i.object) {
                s_mosi.write_uint32_t_reverse_endianness(j.id);
                s_mosi.write_uint32_t_reverse_endianness(set_id);
                io_write_offset_f2_pof_add(s_mosi, object_database_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
            }
        }
    else
        for (object_set_info_file& i : obj_db->object_set) {
            uint32_t set_id = i.id;
            for (object_info_data_file& j : i.object) {
                s_mosi.write_uint32_t_reverse_endianness(j.id);
                s_mosi.write_uint32_t_reverse_endianness(set_id);
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

    new (&st.header) f2_header('MOSI');
    st.header.attrib.set_big_endian(big_endian);

    st.write(s, true, is_x);
}

inline static int64_t object_database_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (const string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool object_database_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (const string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return false;

    vec.push_back(str);
    return true;
}
