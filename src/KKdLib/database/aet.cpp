/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "aet.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void aet_database_file_classic_read_inner(aet_database_file* aet_db, stream& s);
static void aet_database_file_classic_write_inner(aet_database_file* aet_db, stream& s);
static void aet_database_file_modern_read_inner(aet_database_file* aet_db, stream& s, uint32_t header_length);
static void aet_database_file_modern_write_inner(aet_database_file* aet_db, stream& s);
static int64_t aet_database_file_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, std::string& str);
static bool aet_database_file_strings_push_back_check(std::vector<string_hash>& vec, std::string& str);

const aet_db_aet_set aet_db_aet_set_null;
const aet_db_aet aet_db_aet_null;

aet_db_aet_file::aet_db_aet_file() {
    id = -1;
    index = -1;
}

aet_db_aet_file::~aet_db_aet_file() {

}

aet_db_aet::aet_db_aet() : load_count() {
    id = -1;
    name_hash = hash_murmurhash_empty;
}

aet_db_aet::~aet_db_aet() {

}

aet_db_aet_set_file::aet_db_aet_set_file() {
    id = -1;
    index = -1;
    sprite_set_id = -1;
}

aet_db_aet_set_file::~aet_db_aet_set_file() {

}

aet_db_aet_set::aet_db_aet_set() {
    id = -1;
    name_hash = hash_murmurhash_empty;
    file_name_hash = hash_murmurhash_empty;
    index = -1;
    sprite_set_id = -1;
}

aet_db_aet_set::~aet_db_aet_set() {

}

aet_database_file::aet_database_file() : ready(), modern(), big_endian(), is_x() {

}

aet_database_file::~aet_database_file() {

}

void aet_database_file::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, "rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                aet_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_aei = str_utils_add(path, ".aei");
        if (path_check_file_exists(path_aei)) {
            f2_struct st;
            st.read(path_aei);
            if (st.header.signature == reverse_endianness_uint32_t('AEDB')) {
                memory_stream s_aedb;
                s_aedb.open(st.data);
                s_aedb.big_endian = st.header.use_big_endian;
                aet_database_file_modern_read_inner(this, s_aedb, st.header.length);
            }
        }
        free_def(path_aei);
    }
}

void aet_database_file::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, L"rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                aet_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_aei = str_utils_add(path, L".aei");
        if (path_check_file_exists(path_aei)) {
            f2_struct st;
            st.read(path_aei);
            if (st.header.signature == reverse_endianness_uint32_t('AEDB')) {
                memory_stream s_aedb;
                s_aedb.open(st.data);
                s_aedb.big_endian = st.header.use_big_endian;
                aet_database_file_modern_read_inner(this, s_aedb, st.header.length);
            }
        }
        free_def(path_aei);
    }
}

void aet_database_file::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        aet_database_file_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('AEDB')) {
            memory_stream s_aedb;
            s_aedb.open(st.data);
            s_aedb.big_endian = st.header.use_big_endian;
            aet_database_file_modern_read_inner(this, s_aedb, st.header.length);
        }
    }
}

void aet_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            aet_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_aei = str_utils_add(path, ".aei");
        file_stream s;
        s.open(path_aei, "wb");
        if (s.check_not_null())
            aet_database_file_modern_write_inner(this, s);
        free_def(path_aei);
    }
}

void aet_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            aet_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_aei = str_utils_add(path, L".aei");
        file_stream s;
        s.open(path_aei, L"wb");
        if (s.check_not_null())
            aet_database_file_modern_write_inner(this, s);
        free_def(path_aei);
    }
}

void aet_database_file::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        aet_database_file_classic_write_inner(this, s);
    else
        aet_database_file_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool aet_database_file::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    aet_database_file* aet_db = (aet_database_file*)data;
    aet_db->read(s.c_str(), aet_db->modern);

    return aet_db->ready;
}

aet_database::aet_database() {

}

aet_database::~aet_database() {

}

void aet_database::add(aet_database_file* aet_db_file) {
    if (!aet_db_file || !aet_db_file->ready)
        return;

    size_t aet_sets_count = this->aet_set_ids.size();

    aet_set_ids.reserve(aet_db_file->aet_set.size());
    aet_set_indices.reserve(aet_db_file->aet_set.size());
    aet_set_names.reserve(aet_db_file->aet_set.size());

    for (aet_db_aet_set_file& i : aet_db_file->aet_set) {
        aet_sets.push_back({});
        aet_db_aet_set& aet_set = aet_sets.back();
        aet_set.id = i.id;
        aet_set.name.assign(i.name);
        aet_set.name_hash = hash_string_murmurhash(aet_set.name);
        aet_set.file_name.assign(i.file_name);
        aet_set.file_name_hash = hash_string_murmurhash(aet_set.file_name);
        aet_set.index = (uint32_t)(aet_sets_count + i.index);
        aet_set.sprite_set_id = i.sprite_set_id;

        aet_set_ids.push_back({ aet_set.id, &aet_set });
        aet_set_indices.push_back({ aet_set.index, &aet_set });
        aet_set_names.push_back({ aet_set.name, &aet_set });

        aet_ids.reserve(i.aet.size());
        aet_names.reserve(i.aet.size());
        aet_indices.reserve(i.aet.size());

        for (aet_db_aet_file& j : i.aet) {
            aets.push_back({});
            aet_db_aet& aet = aets.back();
            aet.id = j.id;
            aet.name.assign(j.name);
            aet.name_hash = hash_string_murmurhash(aet.name);
            aet.info = { j.index, (uint16_t)aet_set.index };
            aet.load_count = 1;

            aet_ids.push_back({ aet.id, &aet });
            aet_names.push_back({ aet.name, &aet });
            aet_indices.push_back({ aet.info, &aet });
        }
    }

    aet_set_ids.combine();
    aet_set_indices.combine();
    aet_set_names.combine();
    aet_ids.combine();
    aet_names.combine();
    aet_indices.combine();
}

void aet_database::clear() {
    aet_sets.clear();
    aet_set_ids.clear();
    aet_set_indices.clear();
    aet_set_names.clear();
    aets.clear();
    aet_ids.clear();
    aet_names.clear();
    aet_indices.clear();
}

void aet_database::add_aet_set(uint32_t set_id, uint32_t index) {
    if (set_id == -1 || set_id == hash_murmurhash_empty)
        return;

    aet_sets.push_back({});
    aet_db_aet_set& set = aet_sets.back();
    set.id = set_id;
    set.index = index;

    aet_set_indices.push_back({ set.index, &set });
    aet_set_ids.push_back({ set.id, &set });

    aet_set_indices.combine();
    aet_set_ids.combine();
}

void aet_database::parse(const aet_db_aet_set_file* set_file,
    std::string& set_name, std::vector<uint32_t>& aet_ids) {
    if (!set_file)
        return;

    auto elem = aet_set_ids.find(set_file->id);
    if (elem == aet_set_ids.end())
        return;

    uint16_t set_index = (uint16_t)elem->second->index;

    aet_set_names.push_back({ set_file->name, elem->second });

    aet_ids.reserve(set_file->aet.size());
    for (const aet_db_aet_file& i : set_file->aet) {
        uint32_t id = i.id;
        aet_ids.push_back(id);

        auto j_begin = aets.begin();
        auto j_end = aets.end();
        auto j = j_begin;
        while (j != j_end)
            if (j->id == id)
                break;

        aet_db_aet* aet;
        if (j == j_end) {
            aets.push_back({});
            aet = &aets.back();
        }
        else
            aet = &*j;

        aet->id = id;
        aet->name.assign(i.name);
        aet->name_hash = hash_string_murmurhash(aet->name);
        aet->info = { i.index, set_index };
        aet->load_count++;

        this->aet_ids.push_back({ aet->id, aet });
        aet_names.push_back({ aet->name, aet });
        aet_indices.push_back({ aet->info, aet });
    }

    aet_set_names.combine();
    this->aet_ids.combine();
    aet_names.combine();
    aet_indices.combine();
}

void aet_database::remove_aet_set(uint32_t set_id, uint32_t index,
    const char* set_name, std::vector<uint32_t>& aet_ids) {
    if (set_name && *set_name) {
        auto elem = aet_set_names.find(set_name);
        if (elem != aet_set_names.end())
            aet_set_names.erase(elem);
    }

    if (index != -1) {
        auto elem = aet_set_indices.find(index);
        if (elem != aet_set_indices.end())
            aet_set_indices.erase(elem);
    }

    if (set_id != -1 && set_id != hash_murmurhash_empty) {
        auto elem = aet_set_ids.find(set_id);
        if (elem != aet_set_ids.end())
            aet_set_ids.erase(elem);

        auto i_begin = aet_sets.begin();
        auto i_end = aet_sets.end();
        auto i = i_begin;
        while (i != i_end)
            if (i->id == set_id)
                i = aet_sets.erase(i);
            else
                i++;
    }

    for (auto& i : aet_ids) {
        auto j_begin = aets.begin();
        auto j_end = aets.end();
        auto j = j_begin;
        while (j != j_end) {
            if (j->id != i || --j->load_count > 0) {
                j++;
                continue;
            }

            auto elem_id = this->aet_ids.find(j->id);
            if (elem_id != this->aet_ids.end())
                this->aet_ids.erase(elem_id);

            auto elem_index = aet_indices.find(j->info);
            if (elem_index != aet_indices.end())
                aet_indices.erase(elem_index);

            auto elem_name = aet_names.find(j->name);
            if (elem_name != aet_names.end())
                aet_names.erase(elem_name);

            j = aets.erase(j);
        }
    }
}

const aet_db_aet_set* aet_database::get_aet_set_by_name(const char* name) const {
    auto elem = aet_set_names.find(std::string(name));
    if (elem != aet_set_names.end())
        return elem->second;
    return &aet_db_aet_set_null;
}

const aet_db_aet_set* aet_database::get_aet_set_by_id(uint32_t set_id) const {
    auto elem = aet_set_ids.find(set_id);
    if (elem != aet_set_ids.end())
        return elem->second;
    return &aet_db_aet_set_null;
}

const aet_db_aet_set* aet_database::get_aet_set_by_index(uint32_t index) const {
    auto elem = aet_set_indices.find(index);
    if (elem != aet_set_indices.end())
        return elem->second;
    return &aet_db_aet_set_null;
}

const aet_db_aet* aet_database::get_aet_by_name(const char* name) const {
    auto elem = aet_names.find(std::string(name));
    if (elem != aet_names.end())
        return elem->second;
    return &aet_db_aet_null;
}

const aet_db_aet* aet_database::get_aet_by_id(uint32_t id) const {
    auto elem = aet_ids.find(id);
    if (elem != aet_ids.end())
        return elem->second;
    return &aet_db_aet_null;
}

const aet_db_aet* aet_database::get_aet_by_set_id_index(uint32_t set_id, uint32_t index) const {
    auto elem = aet_indices.find({ (uint16_t)index, (uint16_t)get_aet_set_by_id(set_id)->index });
    if (elem != aet_indices.end())
        return elem->second;
    return &aet_db_aet_null;
}

const char* aet_database::get_aet_set_file_name(uint32_t set_id) const {
    return get_aet_set_by_id(set_id)->file_name.c_str();
}

uint32_t aet_database::get_aet_set_id_by_name(const char* name) const {
    return get_aet_set_by_name(name)->id;
}

uint32_t aet_database::get_aet_set_id_by_name_index(uint32_t index) const {
    return aet_set_names[index].second->id;
}

const char* aet_database::get_aet_set_name(uint32_t set_id) const {
    return get_aet_set_by_id(set_id)->name.c_str();
}

uint32_t aet_database::get_aet_id_by_name(const char* name) const {
    return get_aet_by_name(name)->id;
}

static void aet_database_file_classic_read_inner(aet_database_file* aet_db, stream& s) {
    uint32_t aet_sets_count = s.read_uint32_t();
    uint32_t aet_sets_offset = s.read_uint32_t();
    uint32_t aets_count = s.read_uint32_t();
    uint32_t aets_offset = s.read_uint32_t();

    aet_db->aet_set.resize(aet_sets_count);

    aet_db_aet_set_file* aet_db_aet_set = aet_db->aet_set.data();

    s.position_push(aet_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < aet_sets_count; i++) {
        aet_db_aet_set_file& aet_set = aet_db_aet_set[i];
        aet_set.id = s.read_uint32_t();
        aet_set.name = s.read_string_null_terminated_offset(s.read_uint32_t());
        aet_set.file_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        aet_set.index = s.read_uint32_t();
        aet_set.sprite_set_id = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(aets_offset, SEEK_SET);
    for (uint32_t i = 0; i < aets_count; i++) {
        uint32_t id = s.read_uint32_t();
        uint32_t name_offset = s.read_uint32_t();
        uint32_t info = s.read_uint32_t();

        uint16_t index = (uint16_t)(info & 0xFFFF);
        uint16_t set_index = (uint16_t)(info >> 16);

        aet_db_aet_set_file& aet_set = aet_db_aet_set[set_index];

        aet_set.aet.push_back({});
        aet_db_aet_file& aet = aet_set.aet.back();
        aet.id = id;
        aet.name.assign(s.read_string_null_terminated_offset(name_offset));
        aet.index = index;
    }
    s.position_pop();

    aet_db->ready = true;
    aet_db->modern = false;
    aet_db->big_endian = false;
    aet_db->is_x = false;
}

static void aet_database_file_classic_write_inner(aet_database_file* aet_db, stream& s) {
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.align_write(0x20);

    size_t aet_sets_count = aet_db->aet_set.size();

    size_t aets_count = 0;
    for (aet_db_aet_set_file& i : aet_db->aet_set)
        aets_count += i.aet.size();

    int64_t aets_offset = s.get_position();
    s.write(0x0C * aets_count);
    s.align_write(0x20);

    int64_t aet_sets_offset = s.get_position();
    s.write(0x10 * aet_db->aet_set.size());
    s.align_write(0x20);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(aet_sets_count + aets_count);

    for (aet_db_aet_set_file& i : aet_db->aet_set)
        for (aet_db_aet_file& j : i.aet)
            if (aet_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }
    s.align_write(0x04);

    for (aet_db_aet_set_file& i : aet_db->aet_set) {
        if (aet_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.name);
        }

        if (aet_database_file_strings_push_back_check(strings, i.file_name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.file_name);
        }
    }
    s.align_write(0x04);

    s.position_push(aets_offset, SEEK_SET);
    for (aet_db_aet_set_file& i : aet_db->aet_set) {
        uint16_t aet_set_index = (uint16_t)i.index;

        for (aet_db_aet_file& j : i.aet) {
            s.write_uint32_t(j.id);
            s.write_uint32_t((uint32_t)aet_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t((aet_set_index << 16) | j.index);
        }
    }
    s.position_pop();

    s.position_push(aet_sets_offset, SEEK_SET);
    for (aet_db_aet_set_file& i : aet_db->aet_set) {
        s.write_uint32_t(i.id);
        s.write_uint32_t((uint32_t)aet_database_file_strings_get_string_offset(strings,
            string_offsets, i.name));
        s.write_uint32_t((uint32_t)aet_database_file_strings_get_string_offset(strings,
            string_offsets, i.file_name));
        s.write_uint32_t(i.index);
        s.write_uint32_t(i.sprite_set_id);
    }
    s.position_pop();

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t((uint32_t)aet_sets_count);
    s.write_uint32_t((uint32_t)aet_sets_offset);
    s.write_uint32_t((uint32_t)aets_count);
    s.write_uint32_t((uint32_t)aets_offset);
    s.position_pop();
}

static void aet_database_file_modern_read_inner(aet_database_file* aet_db, stream& s, uint32_t header_length) {
    bool big_endian = s.big_endian;
    bool is_x = true;

    s.set_position(0x04, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    uint32_t aet_sets_count = s.read_uint32_t_reverse_endianness();
    int64_t aet_sets_offset = s.read_offset(header_length, is_x);
    uint32_t aets_count = s.read_uint32_t_reverse_endianness();
    int64_t aets_offset = s.read_offset(header_length, is_x);

    aet_db->aet_set.resize(aet_sets_count);

    aet_db_aet_set_file* aet_db_aet_set = aet_db->aet_set.data();

    s.position_push(aet_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < aet_sets_count; i++) {
            aet_db_aet_set_file& aet_set = aet_db_aet_set[i];
            aet_set.id = s.read_uint32_t_reverse_endianness();
            aet_set.name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            aet_set.file_name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            aet_set.index = s.read_uint32_t_reverse_endianness();
            aet_set.sprite_set_id = s.read_uint32_t_reverse_endianness();
        }
    else
        for (uint32_t i = 0; i < aet_sets_count; i++) {
            aet_db_aet_set_file& aet_set = aet_db_aet_set[i];
            aet_set.id = s.read_uint32_t_reverse_endianness();
            aet_set.name = s.read_string_null_terminated_offset(s.read_offset_x());
            aet_set.file_name = s.read_string_null_terminated_offset(s.read_offset_x());
            aet_set.index = s.read_uint32_t_reverse_endianness();
            aet_set.sprite_set_id = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);
        }
    s.position_pop();

    s.position_push(aets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < aets_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_f2(header_length);
            uint32_t info = s.read_uint32_t_reverse_endianness();

            uint16_t index = (uint16_t)(info & 0xFFFF);
            uint16_t set_index = (uint16_t)((info >> 16) & 0xFFFF);

            aet_db_aet_set_file& aet_set = aet_db_aet_set[set_index];

            aet_set.aet.push_back({});
            aet_db_aet_file& aet = aet_set.aet.back();
            aet.id = id;
            aet.name.assign(s.read_string_null_terminated_offset(name_offset));
            aet.index = index;
        }
    else
        for (uint32_t i = 0; i < aets_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_x();
            uint32_t info = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);

            uint16_t index = (uint16_t)(info & 0xFFFF);
            uint16_t set_index = (uint16_t)((info >> 16) & 0xFFFF);

            aet_db_aet_set_file& aet_set = aet_db_aet_set[set_index];

            aet_set.aet.push_back({});
            aet_db_aet_file& aet = aet_set.aet.back();
            aet.id = id;
            aet.name.assign(s.read_string_null_terminated_offset(name_offset));
            aet.index = index;
        }
    s.position_pop();

    aet_db->ready = true;
    aet_db->modern = true;
    aet_db->big_endian = big_endian;
    aet_db->is_x = is_x;
}

static void aet_database_file_modern_write_inner(aet_database_file* aet_db, stream& s) {
    bool big_endian = aet_db->big_endian;
    bool is_x = aet_db->is_x;

    memory_stream s_aedb;
    s_aedb.open();
    s_aedb.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    size_t aet_sets_count = aet_db->aet_set.size();

    size_t aets_count = 0;
    for (aet_db_aet_set_file& i : aet_db->aet_set)
        aets_count += i.aet.size();

    if (!is_x) {
        ee = { 0, 1, 16, 1 };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 16;
        off = align_val(off, 0x10);

        ee = { off, 1, 16, (uint32_t)aet_sets_count };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(aet_sets_count * 16ULL);
        off = align_val(off, 0x10);

        ee = { off, 1, 12, (uint32_t)aets_count };
        ee.append(0, 3, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(aets_count * 12ULL);
        off = align_val(off, 0x10);
    }
    else {
        ee = { 0, 4, 32, 1 };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        e.vec.push_back(ee);
        off = 32;
        off = align_val(off, 0x10);

        ee = { off, 3, 32, (uint32_t)aet_sets_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 2, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(aet_sets_count * 32ULL);
        off = align_val(off, 0x10);

        ee = { off, 3, 24, (uint32_t)aets_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(aets_count * 24ULL);
        off = align_val(off, 0x10);
    }

    s_aedb.write(is_x ? 0x20 : 0x10);
    s_aedb.align_write(0x10);

    int64_t aet_sets_offset = s_aedb.get_position();
    s_aedb.write(aet_sets_count * (is_x ? 0x20ULL : 0x10ULL));
    s_aedb.align_write(0x10);

    int64_t aets_offset = s_aedb.get_position();
    s_aedb.write(aets_count * (is_x ? 0x18ULL : 0x0CULL));
    s_aedb.align_write(0x10);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(aet_sets_count + aets_count);

    for (aet_db_aet_set_file& i : aet_db->aet_set) {
        if (aet_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s_aedb.get_position());
            s_aedb.write_string_null_terminated(i.name);
        }

        if (aet_database_file_strings_push_back_check(strings, i.file_name)) {
            string_offsets.push_back(s_aedb.get_position());
            s_aedb.write_string_null_terminated(i.file_name);
        }
    }

    for (aet_db_aet_set_file& i : aet_db->aet_set)
        for (aet_db_aet_file& j : i.aet)
            if (aet_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s_aedb.get_position());
                s_aedb.write_string_null_terminated(j.name);
            }
    s_aedb.align_write(0x10);

    s_aedb.position_push(0x00, SEEK_SET);
    s_aedb.write_uint32_t_reverse_endianness((uint32_t)aet_sets_count);
    io_write_offset_pof_add(s_aedb, aet_sets_offset, 0x20, is_x, &pof);
    s_aedb.write_uint32_t_reverse_endianness((uint32_t)aets_count);
    io_write_offset_pof_add(s_aedb, aets_offset, 0x20, is_x, &pof);
    s_aedb.position_pop();

    s_aedb.position_push(aet_sets_offset, SEEK_SET);
    if (!is_x)
        for (aet_db_aet_set_file& i : aet_db->aet_set) {
            s_aedb.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_f2_pof_add(s_aedb, aet_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_aedb, aet_database_file_strings_get_string_offset(strings,
                string_offsets, i.file_name), 0x20, &pof);
            s_aedb.write_uint32_t_reverse_endianness(i.index);
            s_aedb.write_uint32_t_reverse_endianness(i.sprite_set_id);
        }
    else
        for (aet_db_aet_set_file& i : aet_db->aet_set) {
            s_aedb.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_x_pof_add(s_aedb, aet_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            io_write_offset_x_pof_add(s_aedb, aet_database_file_strings_get_string_offset(strings,
                string_offsets, i.file_name), &pof);
            s_aedb.write_uint32_t_reverse_endianness(i.index);
            s_aedb.write_uint32_t_reverse_endianness(i.sprite_set_id);
        }
    s_aedb.position_pop();

    s_aedb.position_push(aets_offset, SEEK_SET);
    for (aet_db_aet_set_file& i : aet_db->aet_set) {
        uint16_t aet_set_index = (uint16_t)i.index;

        if (!is_x)
            for (aet_db_aet_file& j : i.aet) {
                s_aedb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_f2_pof_add(s_aedb, aet_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_aedb.write_uint32_t_reverse_endianness((aet_set_index << 16) | j.index);
            }
        else
            for (aet_db_aet_file& j : i.aet) {
                s_aedb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_x_pof_add(s_aedb, aet_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_aedb.write_uint32_t_reverse_endianness((aet_set_index << 16) | j.index);
                s_aedb.align_write(0x08);
            }
    }
    s_aedb.position_pop();

    f2_struct st;
    s_aedb.align_write(0x10);
    s_aedb.copy(st.data);
    s_aedb.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('AEDB');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;

    st.write(s, true, aet_db->is_x);
}

inline static int64_t aet_database_file_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool aet_database_file_strings_push_back_check(std::vector<string_hash>& vec, std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return false;

    vec.push_back(str);
    return true;
}
