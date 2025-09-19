/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sprite.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void sprite_database_file_classic_read_inner(sprite_database_file* spr_db, stream& s);
static void sprite_database_file_classic_write_inner(sprite_database_file* spr_db, stream& s);
static void sprite_database_file_modern_read_inner(sprite_database_file* spr_db, stream& s, uint32_t header_length);
static void sprite_database_file_modern_write_inner(sprite_database_file* spr_db, stream& s);
static int64_t sprite_database_file_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str);
static bool sprite_database_file_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str);

const spr_db_spr_set spr_db_spr_set_null;
const spr_db_spr spr_db_spr_null;

spr_db_spr_file::spr_db_spr_file() : index(), texture() {
    id = -1;
}

spr_db_spr_file::~spr_db_spr_file() {

}

spr_db_spr::spr_db_spr() : load_count() {
    id = -1;
}

spr_db_spr::~spr_db_spr() {

}

spr_db_spr_set_file::spr_db_spr_set_file() {
    id = -1;
    index = -1;
}

spr_db_spr_set_file::~spr_db_spr_set_file() {

}

spr_db_spr_set::spr_db_spr_set() {
    id = -1;
    index = -1;
}

spr_db_spr_set::~spr_db_spr_set() {

}

sprite_database_file::sprite_database_file() : ready(), modern(), big_endian(), is_x() {

}

sprite_database_file::~sprite_database_file() {

}

void sprite_database_file::read(const char* path, bool modern) {
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
                sprite_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_spi = str_utils_add(path, ".spi");
        if (!path_spi)
            return;

        if (path_check_file_exists(path_spi)) {
            f2_struct st;
            st.read(path_spi);
            if (st.header.signature == reverse_endianness_uint32_t('SPDB')) {
                memory_stream s_spdb;
                s_spdb.open(st.data);
                s_spdb.big_endian = st.header.attrib.get_big_endian();
                sprite_database_file_modern_read_inner(this, s_spdb, st.header.get_length());
            }
        }
        free_def(path_spi);
    }
}

void sprite_database_file::read(const wchar_t* path, bool modern) {
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
                sprite_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_spi = str_utils_add(path, L".spi");
        if (!path_spi)
            return;

        if (path_check_file_exists(path_spi)) {
            f2_struct st;
            st.read(path_spi);
            if (st.header.signature == reverse_endianness_uint32_t('SPDB')) {
                memory_stream s_spdb;
                s_spdb.open(st.data);
                s_spdb.big_endian = st.header.attrib.get_big_endian();
                sprite_database_file_modern_read_inner(this, s_spdb, st.header.get_length());
            }
        }
        free_def(path_spi);
    }
}

void sprite_database_file::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        sprite_database_file_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('SPDB')) {
            memory_stream s_spdb;
            s_spdb.open(st.data);
            s_spdb.big_endian = st.header.attrib.get_big_endian();
            sprite_database_file_modern_read_inner(this, s_spdb, st.header.get_length());
        }
    }
}

void sprite_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (!path_bin)
            return;

        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            sprite_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_spi = str_utils_add(path, ".spi");
        if (!path_spi)
            return;

        file_stream s;
        s.open(path_spi, "wb");
        if (s.check_not_null())
            sprite_database_file_modern_write_inner(this, s);
        free_def(path_spi);
    }
}

void sprite_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (!path_bin)
            return;

        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            sprite_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_spi = str_utils_add(path, L".spi");
        if (!path_spi)
            return;

        file_stream s;
        s.open(path_spi, L"wb");
        if (s.check_not_null())
            sprite_database_file_modern_write_inner(this, s);
        free_def(path_spi);
    }
}

void sprite_database_file::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        sprite_database_file_classic_write_inner(this, s);
    else
        sprite_database_file_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool sprite_database_file::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    sprite_database_file* spr_db = (sprite_database_file*)data;
    spr_db->read(path.c_str(), spr_db->modern);

    return spr_db->ready;
}

sprite_database::sprite_database() {

}

sprite_database::~sprite_database() {

}

void sprite_database::add(sprite_database_file* spr_db_file) {
    if (!spr_db_file || !spr_db_file->ready)
        return;

    size_t sprite_sets_count = this->spr_set_ids.size();

    spr_set_ids.reserve(spr_db_file->sprite_set.size());
    spr_set_indices.reserve(spr_db_file->sprite_set.size());
    spr_set_names.reserve(spr_db_file->sprite_set.size());

    for (spr_db_spr_set_file& i : spr_db_file->sprite_set) {
        spr_sets.push_back({});
        spr_db_spr_set& spr_set = spr_sets.back();
        spr_set.id = i.id;
        spr_set.name.assign(i.name);
        spr_set.file_name.assign(i.file_name);
        spr_set.index = (uint32_t)(sprite_sets_count + i.index);

        spr_set_ids.push_back(spr_set.id, &spr_set);
        spr_set_indices.push_back(spr_set.index, &spr_set);
        spr_set_names.push_back(spr_set.name, &spr_set);

        spr_ids.reserve(i.sprite.size());
        spr_names.reserve(i.sprite.size());
        spr_indices.reserve(i.sprite.size());

        for (spr_db_spr_file& j : i.sprite) {
            sprs.push_back({});
            spr_db_spr& spr = sprs.back();
            spr.id = j.id;
            spr.name.assign(j.name);
            //spr.info = { j.index, (uint16_t)((j.texture ? 0x1000 : 0x0000) | spr_set.index) };
            spr.info = { j.index, (uint16_t)((j.texture ? 0x4000 : 0x0000) | spr_set.index) };
            spr.load_count = 1;

            spr_ids.push_back(spr.id, &spr);
            spr_names.push_back(spr.name, &spr);
            spr_indices.push_back(spr.info, &spr);
        }
    }

    spr_set_ids.combine();
    spr_set_indices.combine();
    spr_set_names.combine();
    spr_ids.combine();
    spr_names.combine();
    spr_indices.combine();
}

void sprite_database::clear() {
    spr_sets.clear();
    spr_set_ids.clear();
    spr_set_indices.clear();
    spr_set_names.clear();
    sprs.clear();
    spr_ids.clear();
    spr_names.clear();
    spr_indices.clear();
}

void sprite_database::add_spr_set(uint32_t set_id, uint32_t index) {
    if (set_id == -1 || set_id == hash_murmurhash_empty)
        return;

    spr_sets.push_back({});
    spr_db_spr_set& set = spr_sets.back();
    set.id = set_id;
    set.index = index;

    spr_set_indices.push_back(set.index, &set);
    spr_set_ids.push_back(set.id, &set);

    spr_set_indices.combine();
    spr_set_ids.combine();
}

void sprite_database::parse(const spr_db_spr_set_file* set_file,
    std::string& set_name, std::vector<uint32_t>& sprite_ids) {
    if (!set_file)
        return;

    auto elem = spr_set_ids.find(set_file->id);
    if (elem == spr_set_ids.end())
        return;

    uint16_t set_index = (uint16_t)elem->second->index;

    spr_db_spr_set* spr_set = elem->second;
    spr_set->name.assign(set_file->name);
    spr_set->file_name.assign(set_file->file_name);

    spr_set_names.push_back(set_file->name, elem->second);

    sprite_ids.reserve(set_file->sprite.size());
    for (const spr_db_spr_file& i : set_file->sprite) {
        uint32_t id = i.id;
        sprite_ids.push_back(id);

        auto j_begin = sprs.begin();
        auto j_end = sprs.end();
        auto j = j_begin;
        while (j != j_end)
            if (j->id == id)
                break;
            else
                j++;

        spr_db_spr* spr;
        if (j == j_end) {
            sprs.push_back({});
            spr = &sprs.back();
        }
        else
            spr = &*j;

        spr->id = id;
        spr->name.assign(i.name);
        //spr->info = { i.index, (uint16_t)((i.texture ? 0x1000 : 0x0000) | set_index) };
        spr->info = { i.index, (uint16_t)((i.texture ? 0x4000 : 0x0000) | set_index) };
        spr->load_count++;

        spr_ids.push_back(spr->id, spr);
        spr_names.push_back(spr->name, spr);
        spr_indices.push_back(spr->info, spr);
    }

    spr_set_names.combine();
    spr_ids.combine();
    spr_names.combine();
    spr_indices.combine();
}

void sprite_database::remove_spr_set(uint32_t set_id, uint32_t index,
    const char* set_name, std::vector<uint32_t>& sprite_ids) {
    if (set_name && *set_name) {
        auto elem = spr_set_names.find(set_name);
        if (elem != spr_set_names.end())
            spr_set_names.erase(elem);
    }

    if (index != -1) {
        auto elem = spr_set_indices.find(index);
        if (elem != spr_set_indices.end())
            spr_set_indices.erase(elem);
    }

    if (set_id != -1 && set_id != hash_murmurhash_empty) {
        auto elem = spr_set_ids.find(set_id);
        if (elem != spr_set_ids.end())
            spr_set_ids.erase(elem);

        auto i_begin = spr_sets.begin();
        auto i_end = spr_sets.end();
        auto i = i_begin;
        while (i != i_end)
            if (i->id == set_id) {
                i = spr_sets.erase(i);
                i_end = spr_sets.end();
            }
            else
                i++;
    }

    for (auto& i : sprite_ids) {
        auto j_begin = sprs.begin();
        auto j_end = sprs.end();
        auto j = j_begin;
        while (j != j_end) {
            if (j->id != i || --j->load_count > 0) {
                j++;
                continue;
            }

            auto elem_id = spr_ids.find(j->id);
            if (elem_id != spr_ids.end())
                spr_ids.erase(elem_id);

            auto elem_index = spr_indices.find(j->info);
            if (elem_index != spr_indices.end())
                spr_indices.erase(elem_index);

            auto elem_name = spr_names.find(j->name);
            if (elem_name != spr_names.end())
                spr_names.erase(elem_name);

            j = sprs.erase(j);
        }
    }
}

const spr_db_spr_set* sprite_database::get_spr_set_by_name(const char* name) const {
    auto elem = spr_set_names.find(std::string(name));
    if (elem != spr_set_names.end())
        return elem->second;
    return &spr_db_spr_set_null;
}

const spr_db_spr_set* sprite_database::get_spr_set_by_id(uint32_t set_id) const {
    auto elem = spr_set_ids.find(set_id);
    if (elem != spr_set_ids.end())
        return elem->second;
    return &spr_db_spr_set_null;
}

const spr_db_spr_set* sprite_database::get_spr_set_by_index(uint32_t index) const {
    auto elem = spr_set_indices.find(index);
    if (elem != spr_set_indices.end())
        return elem->second;
    return &spr_db_spr_set_null;
}

const spr_db_spr* sprite_database::get_spr_by_name(const char* name) const {
    auto elem = spr_names.find(std::string(name));
    if (elem != spr_names.end())
        return elem->second;
    return &spr_db_spr_null;
}

const spr_db_spr* sprite_database::get_spr_by_id(uint32_t id) const {
    auto elem = spr_ids.find(id);
    if (elem != spr_ids.end())
        return elem->second;
    return &spr_db_spr_null;
}

const spr_db_spr* sprite_database::get_spr_by_set_id_index(uint32_t set_id, uint32_t index) const {
    auto elem = spr_indices.find({ (uint16_t)index, (uint16_t)get_spr_set_by_id(set_id)->index });
    if (elem != spr_indices.end())
        return elem->second;
    return &spr_db_spr_null;
}

const spr_db_spr* sprite_database::get_tex_by_set_id_index(uint32_t set_id, uint32_t index) const {
    //auto elem = spr_indices.find({ (uint16_t)index, (uint16_t)(0x1000 | get_spr_set_by_id(set_id)->index) });
    auto elem = spr_indices.find({ (uint16_t)index, (uint16_t)(0x4000 | get_spr_set_by_id(set_id)->index) });
    if (elem != spr_indices.end())
        return elem->second;
    return &spr_db_spr_null;
}

uint32_t sprite_database::get_spr_id_by_name(const char* name) const {
    return get_spr_by_name(name)->id;
}

const char* sprite_database::get_spr_set_file_name(uint32_t set_id) const {
    return get_spr_set_by_id(set_id)->file_name.c_str();
}

uint32_t sprite_database::get_spr_set_id_by_name(const char* name) const {
    return get_spr_set_by_name(name)->id;
}

uint32_t sprite_database::get_spr_set_id_by_name_index(uint32_t index) const {
    return spr_set_names[index].second->id;
}

const char* sprite_database::get_spr_set_name(uint32_t set_id) const {
    return get_spr_set_by_id(set_id)->name.c_str();
}

static void sprite_database_file_classic_read_inner(sprite_database_file* spr_db, stream& s) {
    uint32_t sprite_sets_count = s.read_uint32_t();
    uint32_t sprite_sets_offset = s.read_uint32_t();
    uint32_t sprites_count = s.read_uint32_t();
    uint32_t sprites_offset = s.read_uint32_t();

    spr_db->sprite_set.resize(sprite_sets_count);

    spr_db_spr_set_file* spr_db_spr_set = spr_db->sprite_set.data();

    s.position_push(sprite_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < sprite_sets_count; i++) {
        spr_db_spr_set_file& spr_set = spr_db_spr_set[i];
        spr_set.id = s.read_uint32_t();
        spr_set.name = s.read_string_null_terminated_offset(s.read_uint32_t());
        spr_set.file_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        spr_set.index = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(sprites_offset, SEEK_SET);
    for (uint32_t i = 0; i < sprites_count; i++) {
        uint32_t id = s.read_uint32_t();
        uint32_t name_offset = s.read_uint32_t();
        uint32_t info = s.read_uint32_t();

        uint16_t index = (uint16_t)(info & 0xFFFF);
        uint16_t set_index = (uint16_t)((info >> 16) & 0x0FFF);
        bool texture = !!((info >> 16) & 0x1000);

        spr_db_spr_set_file& spr_set = spr_db_spr_set[set_index];

        spr_set.sprite.push_back({});
        spr_db_spr_file& spr = spr_set.sprite.back();
        spr.id = id;
        spr.name.assign(s.read_string_null_terminated_offset(name_offset));
        spr.index = index;
        spr.texture = texture;
    }
    s.position_pop();

    spr_db->ready = true;
    spr_db->modern = false;
    spr_db->big_endian = false;
    spr_db->is_x = false;
}

static void sprite_database_file_classic_write_inner(sprite_database_file* spr_db, stream& s) {
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.align_write(0x20);

    size_t sprite_sets_count = spr_db->sprite_set.size();

    size_t sprites_count = 0;
    for (spr_db_spr_set_file& i : spr_db->sprite_set)
        sprites_count += i.sprite.size();

    int64_t sprites_offset = s.get_position();
    s.write(0x0C * sprites_count);
    s.align_write(0x20);

    int64_t sprite_sets_offset = s.get_position();
    s.write(0x10 * spr_db->sprite_set.size());
    s.align_write(0x20);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(sprite_sets_count + sprites_count);
    string_offsets.reserve(sprite_sets_count + sprites_count);

    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        for (spr_db_spr_file& j : i.sprite)
            if (j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }

        for (spr_db_spr_file& j : i.sprite)
            if (!j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }
    }
    s.align_write(0x04);

    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        if (sprite_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.name);
        }

        if (sprite_database_file_strings_push_back_check(strings, i.file_name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.file_name);
        }
    }
    s.align_write(0x04);

    s.position_push(sprites_offset, SEEK_SET);
    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        uint16_t sprite_set_index = (uint16_t)(i.index & 0x0FFF);

        for (spr_db_spr_file& j : i.sprite) {
            if (!j.texture)
                continue;

            s.write_uint32_t(j.id);
            s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t(((0x1000 | sprite_set_index) << 16) | j.index);
        }

        for (spr_db_spr_file& j : i.sprite) {
            if (j.texture)
                continue;

            s.write_uint32_t(j.id);
            s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t((sprite_set_index << 16) | j.index);
        }
    }
    s.position_pop();

    s.position_push(sprite_sets_offset, SEEK_SET);
    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        s.write_uint32_t(i.id);
        s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
            string_offsets, i.name));
        s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
            string_offsets, i.file_name));
        s.write_uint32_t(i.index);
    }
    s.position_pop();

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t((uint32_t)sprite_sets_count);
    s.write_uint32_t((uint32_t)sprite_sets_offset);
    s.write_uint32_t((uint32_t)sprites_count);
    s.write_uint32_t((uint32_t)sprites_offset);
    s.position_pop();
}

static void sprite_database_file_modern_read_inner(sprite_database_file* spr_db, stream& s, uint32_t header_length) {
    bool big_endian = s.big_endian;
    bool is_x = true;

    s.set_position(0x04, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    uint32_t sprite_sets_count = s.read_uint32_t_reverse_endianness();
    int64_t sprite_sets_offset = s.read_offset(header_length, is_x);
    uint32_t sprites_count = s.read_uint32_t_reverse_endianness();
    int64_t sprites_offset = s.read_offset(header_length, is_x);

    spr_db->sprite_set.resize(sprite_sets_count);

    spr_db_spr_set_file* spr_db_spr_set = spr_db->sprite_set.data();

    s.position_push(sprite_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < sprite_sets_count; i++) {
            spr_db_spr_set_file& spr_set = spr_db_spr_set[i];
            spr_set.id = s.read_uint32_t_reverse_endianness();
            spr_set.name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            spr_set.file_name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            spr_set.index = s.read_uint32_t_reverse_endianness();
        }
    else
        for (uint32_t i = 0; i < sprite_sets_count; i++) {
            spr_db_spr_set_file& spr_set = spr_db_spr_set[i];
            spr_set.id = s.read_uint32_t_reverse_endianness();
            spr_set.name = s.read_string_null_terminated_offset(s.read_offset_x());
            spr_set.file_name = s.read_string_null_terminated_offset(s.read_offset_x());
            spr_set.index = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);
        }
    s.position_pop();

    s.position_push(sprites_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < sprites_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_f2(header_length);
            uint32_t info = s.read_uint32_t_reverse_endianness();

            uint16_t index = (uint16_t)(info & 0xFFFF);
            uint16_t set_index = (uint16_t)((info >> 16) & 0x0FFF);
            bool texture = !!((info >> 16) & 0x1000);

            spr_db_spr_set_file& spr_set = spr_db_spr_set[set_index];

            spr_set.sprite.push_back({});
            spr_db_spr_file& spr = spr_set.sprite.back();
            spr.id = id;
            spr.name.assign(s.read_string_null_terminated_offset(name_offset));
            spr.index = index;
            spr.texture = texture;
        }
    else
        for (uint32_t i = 0; i < sprites_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_x();
            uint32_t info = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);

            uint16_t index = (uint16_t)(info & 0xFFFF);
            uint16_t set_index = (uint16_t)((info >> 16) & 0x0FFF);
            bool texture = !!((info >> 16) & 0x1000);

            spr_db_spr_set_file& spr_set = spr_db_spr_set[set_index];

            spr_set.sprite.push_back({});
            spr_db_spr_file& spr = spr_set.sprite.back();
            spr.id = id;
            spr.name.assign(s.read_string_null_terminated_offset(name_offset));
            spr.index = index;
            spr.texture = texture;
        }
    s.position_pop();

    spr_db->ready = true;
    spr_db->modern = true;
    spr_db->big_endian = big_endian;
    spr_db->is_x = is_x;
}

static void sprite_database_file_modern_write_inner(sprite_database_file* spr_db, stream& s) {
    bool big_endian = spr_db->big_endian;
    bool is_x = spr_db->is_x;

    memory_stream s_spdb;
    s_spdb.open();
    s_spdb.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    size_t sprite_sets_count = spr_db->sprite_set.size();

    size_t sprites_count = 0;
    for (spr_db_spr_set_file& i : spr_db->sprite_set)
        sprites_count += i.sprite.size();

    if (!is_x) {
        ee = { 0, 1, 16, 1 };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 16;
        off = align_val(off, 0x10);

        ee = { off, 1, 16, (uint32_t)sprite_sets_count };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(sprite_sets_count * 16ULL);
        off = align_val(off, 0x10);

        ee = { off, 1, 12, (uint32_t)sprites_count };
        ee.append(0, 3, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(sprites_count * 12ULL);
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

        ee = { off, 3, 32, (uint32_t)sprite_sets_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 2, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(sprite_sets_count * 32ULL);
        off = align_val(off, 0x10);

        ee = { off, 3, 24, (uint32_t)sprites_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(sprites_count * 24ULL);
        off = align_val(off, 0x10);
    }

    s_spdb.write(is_x ? 0x20 : 0x10);
    s_spdb.align_write(0x10);

    int64_t sprite_sets_offset = s_spdb.get_position();
    s_spdb.write(sprite_sets_count * (is_x ? 0x20ULL : 0x10ULL));
    s_spdb.align_write(0x10);

    int64_t sprites_offset = s_spdb.get_position();
    s_spdb.write(sprites_count * (is_x ? 0x18ULL : 0x0CULL));
    s_spdb.align_write(0x10);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(sprite_sets_count + sprites_count);
    string_offsets.reserve(sprite_sets_count + sprites_count);

    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        if (sprite_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s_spdb.get_position());
            s_spdb.write_string_null_terminated(i.name);
        }

        if (sprite_database_file_strings_push_back_check(strings, i.file_name)) {
            string_offsets.push_back(s_spdb.get_position());
            s_spdb.write_string_null_terminated(i.file_name);
        }
    }

    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        for (spr_db_spr_file& j : i.sprite)
            if (j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s_spdb.get_position());
                s_spdb.write_string_null_terminated(j.name);
            }

        for (spr_db_spr_file& j : i.sprite)
            if (!j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s_spdb.get_position());
                s_spdb.write_string_null_terminated(j.name);
            }
    }
    s_spdb.align_write(0x10);

    s_spdb.position_push(0x00, SEEK_SET);
    s_spdb.write_uint32_t_reverse_endianness((uint32_t)sprite_sets_count);
    io_write_offset_pof_add(s_spdb, sprite_sets_offset, 0x20, is_x, &pof);
    s_spdb.write_uint32_t_reverse_endianness((uint32_t)sprites_count);
    io_write_offset_pof_add(s_spdb, sprites_offset, 0x20, is_x, &pof);
    s_spdb.position_pop();

    s_spdb.position_push(sprite_sets_offset, SEEK_SET);
    if (!is_x)
        for (spr_db_spr_set_file& i : spr_db->sprite_set) {
            s_spdb.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.file_name), 0x20, &pof);
            s_spdb.write_uint32_t_reverse_endianness(i.index);
        }
    else
        for (spr_db_spr_set_file& i : spr_db->sprite_set) {
            s_spdb.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.file_name), &pof);
            s_spdb.write_uint32_t_reverse_endianness(i.index);
        }
    s_spdb.position_pop();

    s_spdb.position_push(sprites_offset, SEEK_SET);
    for (spr_db_spr_set_file& i : spr_db->sprite_set) {
        uint16_t sprite_set_index = (uint16_t)(i.index & 0x0FFF);

        if (!is_x) {
            for (spr_db_spr_file& j : i.sprite) {
                if (!j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_spdb.write_uint32_t_reverse_endianness(((0x1000 | sprite_set_index) << 16) | j.index);
            }

            for (spr_db_spr_file& j : i.sprite) {
                if (j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_spdb.write_uint32_t_reverse_endianness((sprite_set_index << 16) | j.index);
            }
        }
        else {
            for (spr_db_spr_file& j : i.sprite) {
                if (!j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_spdb.write_uint32_t_reverse_endianness(((0x1000 | sprite_set_index) << 16) | j.index);
                s_spdb.align_write(0x08);
            }

            for (spr_db_spr_file& j : i.sprite) {
                if (j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_spdb.write_uint32_t_reverse_endianness((sprite_set_index << 16) | j.index);
                s_spdb.align_write(0x08);
            }
        }
    }
    s_spdb.position_pop();

    f2_struct st;
    s_spdb.align_write(0x10);
    s_spdb.copy(st.data);
    s_spdb.close();

    st.enrs = e;
    st.pof = pof;

    new (&st.header) f2_header('SPDB');
    st.header.attrib.set_big_endian(big_endian);

    st.write(s, true, spr_db->is_x);
}

inline static int64_t sprite_database_file_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (const string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool sprite_database_file_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (const string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return false;

    vec.push_back(str);
    return true;
}
