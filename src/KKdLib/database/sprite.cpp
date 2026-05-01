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

static void sprite_database_file_classic_read_inner(SprDbFile* spr_db, stream& s);
static void sprite_database_file_classic_write_inner(SprDbFile* spr_db, stream& s);
static void sprite_database_file_modern_read_inner(SprDbFile* spr_db, stream& s, uint32_t header_length);
static void sprite_database_file_modern_write_inner(SprDbFile* spr_db, stream& s);
static int64_t sprite_database_file_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str);
static bool sprite_database_file_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str);

const SprDb::SprSet SprDb::s_err_set;
const SprDb::Spr SprDb::s_err_spr;

SprDbFile::Spr::Spr() : id(), texture() {
    uid = -1;
}

SprDbFile::Spr::~Spr() {

}

SprDbFile::SprSet::SprSet() {
    uid = -1;
    id = -1;
}

SprDbFile::SprSet::~SprSet() {

}

SprDbFile::SprDbFile() : ready(), modern(), big_endian(), is_x() {

}

SprDbFile::~SprDbFile() {

}

void SprDbFile::read(const char* path, bool modern) {
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

void SprDbFile::read(const wchar_t* path, bool modern) {
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

void SprDbFile::read(const void* data, size_t size, bool modern) {
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

void SprDbFile::write(const char* path) {
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

void SprDbFile::write(const wchar_t* path) {
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

void SprDbFile::write(void** data, size_t* size) {
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

bool SprDbFile::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    SprDbFile* spr_db = (SprDbFile*)data;
    spr_db->read(path.c_str(), spr_db->modern);

    return spr_db->ready;
}

SprDb::Spr::Spr() : reference() {
    uid = -1;
}

SprDb::Spr::~Spr() {

}

SprDb::SprSet::SprSet() {
    uid = -1;
    id = -1;
}

SprDb::SprSet::~SprSet() {

}

SprDb::SprDb() {

}

SprDb::~SprDb() {

}

void SprDb::add(SprDbFile* spr_db_file) {
    if (!spr_db_file || !spr_db_file->ready)
        return;

    size_t set_count = this->set_uid.size();

    set_uid.reserve(spr_db_file->set.size());
    set_idx.reserve(spr_db_file->set.size());
    set_name.reserve(spr_db_file->set.size());

    for (SprDbFile::SprSet& i : spr_db_file->set) {
        this->set.push_back({});
        SprDb::SprSet& set = this->set.back();
        set.uid = i.uid;
        set.name.assign(i.name);
        set.file.assign(i.file);
        set.id = (uint32_t)(set_count + i.id);

        set_uid.push_back(set.uid, &set);
        set_idx.push_back(set.id, &set);
        set_name.push_back(set.name, &set);

        spr_uid.reserve(i.sprite.size());
        spr_name.reserve(i.sprite.size());
        spr_idx.reserve(i.sprite.size());

        for (SprDbFile::Spr& j : i.sprite) {
            this->spr.push_back({});
            SprDb::Spr& spr = this->spr.back();
            spr.uid = j.uid;
            spr.name.assign(j.name);
            //spr.id = { j.id, (uint16_t)((j.texture ? 0x1000 : 0x0000) | set.id) };
            spr.id = { j.id, (uint16_t)((j.texture ? 0x4000 : 0x0000) | set.id) };
            spr.reference = 1;

            spr_uid.push_back(spr.uid, &spr);
            spr_name.push_back(spr.name, &spr);
            spr_idx.push_back(spr.id, &spr);
        }
    }

    set_uid.combine();
    set_idx.combine();
    set_name.combine();
    spr_uid.combine();
    spr_name.combine();
    spr_idx.combine();
}

void SprDb::clear() {
    set.clear();
    set_uid.clear();
    set_idx.clear();
    set_name.clear();
    spr.clear();
    spr_uid.clear();
    spr_name.clear();
    spr_idx.clear();
}

void SprDb::addSet(uint32_t set_uid, uint32_t id) {
    if (set_uid == -1 || set_uid == hash_murmurhash_empty)
        return;

    this->set.push_back({});
    SprDb::SprSet& set = this->set.back();
    set.uid = set_uid;
    set.id = id;

    this->set_uid.push_back(set.uid, &set);
    this->set_idx.push_back(set.id, &set);

    this->set_uid.combine();
    this->set_idx.combine();
}

void SprDb::freeSet(uint32_t set_uid, uint32_t id,
    const char* set_name, std::vector<uint32_t>& sprite_ids) {
    if (set_name && *set_name) {
        auto elem = this->set_name.find(set_name);
        if (elem != this->set_name.end())
            this->set_name.erase(elem);
    }

    if (id != -1) {
        auto elem = set_idx.find(id);
        if (elem != set_idx.end())
            set_idx.erase(elem);
    }

    if (set_uid != -1 && set_uid != hash_murmurhash_empty) {
        auto elem = this->set_uid.find(set_uid);
        if (elem != this->set_uid.end())
            this->set_uid.erase(elem);

        auto i_begin = set.begin();
        auto i_end = set.end();
        auto i = i_begin;
        while (i != i_end)
            if (i->uid == set_uid) {
                i = set.erase(i);
                i_end = set.end();
            }
            else
                i++;
    }

    for (auto& i : sprite_ids) {
        auto j_begin = spr.begin();
        auto j_end = spr.end();
        auto j = j_begin;
        while (j != j_end) {
            if (j->uid != i || --j->reference > 0) {
                j++;
                continue;
            }

            auto elem_uid = spr_uid.find(j->uid);
            if (elem_uid != spr_uid.end())
                spr_uid.erase(elem_uid);

            auto elem_idx = spr_idx.find(j->id);
            if (elem_idx != spr_idx.end())
                spr_idx.erase(elem_idx);

            auto elem_name = spr_name.find(j->name);
            if (elem_name != spr_name.end())
                spr_name.erase(elem_name);

            j = spr.erase(j);
        }
    }
}

void SprDb::parse(const SprDbFile::SprSet* set_file, std::vector<uint32_t>& sprite_ids) {
    if (!set_file)
        return;

    auto elem = set_uid.find(set_file->uid);
    if (elem == set_uid.end())
        return;

    uint16_t set_id = (uint16_t)elem->second->id;

    SprDb::SprSet* set = elem->second;
    set->name.assign(set_file->name);
    set->file.assign(set_file->file);

    this->set_name.push_back(set_file->name, elem->second);

    sprite_ids.reserve(set_file->sprite.size());
    for (const SprDbFile::Spr& i : set_file->sprite) {
        uint32_t uid = i.uid;
        sprite_ids.push_back(uid);

        auto j_begin = spr.begin();
        auto j_end = spr.end();
        auto j = j_begin;
        while (j != j_end)
            if (j->uid == uid)
                break;
            else
                j++;

        SprDb::Spr* spr;
        if (j == j_end) {
            this->spr.push_back({});
            spr = &this->spr.back();
        }
        else
            spr = &*j;

        spr->uid = uid;
        spr->name.assign(i.name);
        //spr->id = { i.id, (uint16_t)((i.texture ? 0x1000 : 0x0000) | set_id) };
        spr->id = { i.id, (uint16_t)((i.texture ? 0x4000 : 0x0000) | set_id) };
        spr->reference++;

        spr_uid.push_back(spr->uid, spr);
        spr_name.push_back(spr->name, spr);
        spr_idx.push_back(spr->id, spr);
    }

    this->set_name.combine();
    spr_uid.combine();
    spr_name.combine();
    spr_idx.combine();
}

uint32_t SprDb::getSetSize() const {
    return (uint32_t)set.size();
}

const char* SprDb::getSetNameFromUid(uint32_t uid) const {
    return getSetFromUid(uid).name.c_str();
}

const char* SprDb::getSetFileFromUid(uint32_t uid) const {
    return getSetFromUid(uid).file.c_str();
}

uint32_t SprDb::getSetIdFromUid(uint32_t uid) const {
    return getSetFromUid(uid).id;
}

uint32_t SprDb::getSetUidFromIdx(uint32_t idx) const {
    return set_name[idx].second->uid;
}

const char* SprDb::getSetNameFromIdx(uint32_t idx) const {
    return getSetFromIdx(idx).name.c_str();
}

const char* SprDb::getSetFileFromIdx(uint32_t idx) const {
    return getSetFromIdx(idx).file.c_str();
}

uint32_t SprDb::getSetUidFromName(const std::string& name) const {
    return getSetFromName(name).uid;
}

uint32_t SprDb::getSetIdFromName(const std::string& name) const {
    return getSetFromName(name).id;
}

uint32_t SprDb::getSetIdFromIdx(uint32_t idx) const {
    return getSetFromIdx(idx).id;
}

uint32_t SprDb::getSprIdFromUid(uint32_t uid) const {
    return getSprFromUid(uid).id.w;
}

uint32_t SprDb::getSprUidFromName(const std::string& name) const {
    return getSprFromName(name).uid;
}

uint32_t SprDb::getSprIdFromName(const std::string& name) const {
    return getSprFromName(name).id.w;
}

uint32_t SprDb::getSprUidFromIdx(uint32_t set_uid, uint32_t idx) const {
    return getSprFromIdx(set_uid, idx).uid;
}

uint32_t SprDb::getSprIdFromIdx(uint32_t set_uid, uint32_t idx) const {
    return getSprFromIdx(set_uid, idx).id.w;
}

uint32_t SprDb::getTexUidFromIdx(uint32_t set_uid, uint32_t idx) const {
    return getTexFromIdx(set_uid, idx).uid;
}

uint32_t SprDb::getTexIdFromIdx(uint32_t set_uid, uint32_t idx) const {
    return getTexFromIdx(set_uid, idx).id.w;
}

const SprDb::SprSet& SprDb::getSetFromUid(uint32_t uid) const {
    auto elem = set_uid.find(uid);
    if (elem != set_uid.end())
        return *elem->second;
    return s_err_set;
}

const SprDb::SprSet& SprDb::getSetFromIdx(uint32_t idx) const {
    auto elem = set_idx.find(idx);
    if (elem != set_idx.end())
        return *elem->second;
    return s_err_set;
}

const SprDb::SprSet& SprDb::getSetFromName(const std::string& name) const {
    auto elem = set_name.find(name);
    if (elem != set_name.end())
        return *elem->second;
    return s_err_set;
}

const SprDb::Spr& SprDb::getSprFromUid(uint32_t uid) const {
    auto elem = spr_uid.find(uid);
    if (elem != spr_uid.end())
        return *elem->second;
    return s_err_spr;
}

const SprDb::Spr& SprDb::getSprFromName(const std::string& name) const {
    auto elem = spr_name.find(std::string(name));
    if (elem != spr_name.end())
        return *elem->second;
    return s_err_spr;
}

const SprDb::Spr& SprDb::getSprFromIdx(uint32_t set_uid, uint32_t idx) const {
    auto elem = spr_idx.find({ (uint16_t)idx, (uint16_t)getSetIdFromUid(set_uid) });
    if (elem != spr_idx.end())
        return *elem->second;
    return s_err_spr;
}

const SprDb::Spr& SprDb::getTexFromIdx(uint32_t set_uid, uint32_t idx) const {
    //auto elem = spr_idx.find({ (uint16_t)idx, (uint16_t)(0x1000 | getSetIdFromUid(set_uid)) });
    auto elem = spr_idx.find({ (uint16_t)idx, (uint16_t)(0x4000 | getSetIdFromUid(set_uid)) });
    if (elem != spr_idx.end())
        return *elem->second;
    return s_err_spr;
}

static void sprite_database_file_classic_read_inner(SprDbFile* spr_db, stream& s) {
    uint32_t set_count = s.read_uint32_t();
    uint32_t set_offset = s.read_uint32_t();
    uint32_t spr_count = s.read_uint32_t();
    uint32_t spr_offset = s.read_uint32_t();

    spr_db->set.resize(set_count);

    SprDbFile::SprSet* spr_db_set = spr_db->set.data();

    s.position_push(set_offset, SEEK_SET);
    for (uint32_t i = 0; i < set_count; i++) {
        SprDbFile::SprSet& set = spr_db_set[i];
        set.uid = s.read_uint32_t();
        set.name = s.read_string_null_terminated_offset(s.read_uint32_t());
        set.file = s.read_string_null_terminated_offset(s.read_uint32_t());
        set.id = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(spr_offset, SEEK_SET);
    for (uint32_t i = 0; i < spr_count; i++) {
        uint32_t uid = s.read_uint32_t();
        uint32_t name_offset = s.read_uint32_t();
        uint32_t info = s.read_uint32_t();

        uint16_t id = (uint16_t)(info & 0xFFFF);
        uint16_t set_id = (uint16_t)((info >> 16) & 0x0FFF);
        bool texture = !!((info >> 16) & 0x1000);

        SprDbFile::SprSet& set = spr_db_set[set_id];

        set.sprite.push_back({});
        SprDbFile::Spr& spr = set.sprite.back();
        spr.uid = uid;
        spr.name.assign(s.read_string_null_terminated_offset(name_offset));
        spr.id = id;
        spr.texture = texture;
    }
    s.position_pop();

    spr_db->ready = true;
    spr_db->modern = false;
    spr_db->big_endian = false;
    spr_db->is_x = false;
}

static void sprite_database_file_classic_write_inner(SprDbFile* spr_db, stream& s) {
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.align_write(0x20);

    size_t set_count = spr_db->set.size();

    size_t spr_count = 0;
    for (SprDbFile::SprSet& i : spr_db->set)
        spr_count += i.sprite.size();

    int64_t spr_offset = s.get_position();
    s.write(0x0C * spr_count);
    s.align_write(0x20);

    int64_t set_offset = s.get_position();
    s.write(0x10 * spr_db->set.size());
    s.align_write(0x20);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(set_count + spr_count);
    string_offsets.reserve(set_count + spr_count);

    for (SprDbFile::SprSet& i : spr_db->set) {
        for (SprDbFile::Spr& j : i.sprite)
            if (j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }

        for (SprDbFile::Spr& j : i.sprite)
            if (!j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }
    }
    s.align_write(0x04);

    for (SprDbFile::SprSet& i : spr_db->set) {
        if (sprite_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.name);
        }

        if (sprite_database_file_strings_push_back_check(strings, i.file)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.file);
        }
    }
    s.align_write(0x04);

    s.position_push(spr_offset, SEEK_SET);
    for (SprDbFile::SprSet& i : spr_db->set) {
        uint16_t sprite_set_id = (uint16_t)(i.id & 0x0FFF);

        for (SprDbFile::Spr& j : i.sprite) {
            if (!j.texture)
                continue;

            s.write_uint32_t(j.uid);
            s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t(((0x1000 | sprite_set_id) << 16) | j.id);
        }

        for (SprDbFile::Spr& j : i.sprite) {
            if (j.texture)
                continue;

            s.write_uint32_t(j.uid);
            s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t((sprite_set_id << 16) | j.id);
        }
    }
    s.position_pop();

    s.position_push(set_offset, SEEK_SET);
    for (SprDbFile::SprSet& i : spr_db->set) {
        s.write_uint32_t(i.uid);
        s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
            string_offsets, i.name));
        s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
            string_offsets, i.file));
        s.write_uint32_t(i.id);
    }
    s.position_pop();

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t((uint32_t)set_count);
    s.write_uint32_t((uint32_t)set_offset);
    s.write_uint32_t((uint32_t)spr_count);
    s.write_uint32_t((uint32_t)spr_offset);
    s.position_pop();
}

static void sprite_database_file_modern_read_inner(SprDbFile* spr_db, stream& s, uint32_t header_length) {
    bool big_endian = s.big_endian;
    bool is_x = true;

    s.set_position(0x04, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    uint32_t set_count = s.read_uint32_t_reverse_endianness();
    int64_t set_offset = s.read_offset(header_length, is_x);
    uint32_t spr_count = s.read_uint32_t_reverse_endianness();
    int64_t spr_offset = s.read_offset(header_length, is_x);

    spr_db->set.resize(set_count);

    SprDbFile::SprSet* spr_db_set = spr_db->set.data();

    s.position_push(set_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < set_count; i++) {
            SprDbFile::SprSet& set = spr_db_set[i];
            set.uid = s.read_uint32_t_reverse_endianness();
            set.name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set.file = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set.id = s.read_uint32_t_reverse_endianness();
        }
    else
        for (uint32_t i = 0; i < set_count; i++) {
            SprDbFile::SprSet& set = spr_db_set[i];
            set.uid = s.read_uint32_t_reverse_endianness();
            set.name = s.read_string_null_terminated_offset(s.read_offset_x());
            set.file = s.read_string_null_terminated_offset(s.read_offset_x());
            set.id = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);
        }
    s.position_pop();

    s.position_push(spr_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < spr_count; i++) {
            uint32_t uid = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_f2(header_length);
            uint32_t info = s.read_uint32_t_reverse_endianness();

            uint16_t id = (uint16_t)(info & 0xFFFF);
            uint16_t set_id = (uint16_t)((info >> 16) & 0x0FFF);
            bool texture = !!((info >> 16) & 0x1000);

            SprDbFile::SprSet& set = spr_db_set[set_id];

            set.sprite.push_back({});
            SprDbFile::Spr& spr = set.sprite.back();
            spr.uid = uid;
            spr.name.assign(s.read_string_null_terminated_offset(name_offset));
            spr.id = id;
            spr.texture = texture;
        }
    else
        for (uint32_t i = 0; i < spr_count; i++) {
            uint32_t uid = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_x();
            uint32_t info = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);

            uint16_t id = (uint16_t)(info & 0xFFFF);
            uint16_t set_id = (uint16_t)((info >> 16) & 0x0FFF);
            bool texture = !!((info >> 16) & 0x1000);

            SprDbFile::SprSet& set = spr_db_set[set_id];

            set.sprite.push_back({});
            SprDbFile::Spr& spr = set.sprite.back();
            spr.uid = uid;
            spr.name.assign(s.read_string_null_terminated_offset(name_offset));
            spr.id = id;
            spr.texture = texture;
        }
    s.position_pop();

    spr_db->ready = true;
    spr_db->modern = true;
    spr_db->big_endian = big_endian;
    spr_db->is_x = is_x;
}

static void sprite_database_file_modern_write_inner(SprDbFile* spr_db, stream& s) {
    bool big_endian = spr_db->big_endian;
    bool is_x = spr_db->is_x;

    memory_stream s_spdb;
    s_spdb.open();
    s_spdb.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    size_t set_count = spr_db->set.size();

    size_t spr_count = 0;
    for (SprDbFile::SprSet& i : spr_db->set)
        spr_count += i.sprite.size();

    if (!is_x) {
        ee = { 0, 1, 16, 1 };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 16;
        off = align_val(off, 0x10);

        ee = { off, 1, 16, (uint32_t)set_count };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(set_count * 16ULL);
        off = align_val(off, 0x10);

        ee = { off, 1, 12, (uint32_t)spr_count };
        ee.append(0, 3, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(spr_count * 12ULL);
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

        ee = { off, 3, 32, (uint32_t)set_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 2, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(set_count * 32ULL);
        off = align_val(off, 0x10);

        ee = { off, 3, 24, (uint32_t)spr_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(spr_count * 24ULL);
        off = align_val(off, 0x10);
    }

    s_spdb.write(is_x ? 0x20 : 0x10);
    s_spdb.align_write(0x10);

    int64_t set_offset = s_spdb.get_position();
    s_spdb.write(set_count * (is_x ? 0x20ULL : 0x10ULL));
    s_spdb.align_write(0x10);

    int64_t spr_offset = s_spdb.get_position();
    s_spdb.write(spr_count * (is_x ? 0x18ULL : 0x0CULL));
    s_spdb.align_write(0x10);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(set_count + spr_count);
    string_offsets.reserve(set_count + spr_count);

    for (SprDbFile::SprSet& i : spr_db->set) {
        if (sprite_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s_spdb.get_position());
            s_spdb.write_string_null_terminated(i.name);
        }

        if (sprite_database_file_strings_push_back_check(strings, i.file)) {
            string_offsets.push_back(s_spdb.get_position());
            s_spdb.write_string_null_terminated(i.file);
        }
    }

    for (SprDbFile::SprSet& i : spr_db->set) {
        for (SprDbFile::Spr& j : i.sprite)
            if (j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s_spdb.get_position());
                s_spdb.write_string_null_terminated(j.name);
            }

        for (SprDbFile::Spr& j : i.sprite)
            if (!j.texture && sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s_spdb.get_position());
                s_spdb.write_string_null_terminated(j.name);
            }
    }
    s_spdb.align_write(0x10);

    s_spdb.position_push(0x00, SEEK_SET);
    s_spdb.write_uint32_t_reverse_endianness((uint32_t)set_count);
    io_write_offset_pof_add(s_spdb, set_offset, 0x20, is_x, &pof);
    s_spdb.write_uint32_t_reverse_endianness((uint32_t)spr_count);
    io_write_offset_pof_add(s_spdb, spr_offset, 0x20, is_x, &pof);
    s_spdb.position_pop();

    s_spdb.position_push(set_offset, SEEK_SET);
    if (!is_x)
        for (SprDbFile::SprSet& i : spr_db->set) {
            s_spdb.write_uint32_t_reverse_endianness(i.uid);
            io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.file), 0x20, &pof);
            s_spdb.write_uint32_t_reverse_endianness(i.id);
        }
    else
        for (SprDbFile::SprSet& i : spr_db->set) {
            s_spdb.write_uint32_t_reverse_endianness(i.uid);
            io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.file), &pof);
            s_spdb.write_uint32_t_reverse_endianness(i.id);
        }
    s_spdb.position_pop();

    s_spdb.position_push(spr_offset, SEEK_SET);
    for (SprDbFile::SprSet& i : spr_db->set) {
        uint16_t sprite_set_id = (uint16_t)(i.id & 0x0FFF);

        if (!is_x) {
            for (SprDbFile::Spr& j : i.sprite) {
                if (!j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.uid);
                io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_spdb.write_uint32_t_reverse_endianness(((0x1000 | sprite_set_id) << 16) | j.id);
            }

            for (SprDbFile::Spr& j : i.sprite) {
                if (j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.uid);
                io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_spdb.write_uint32_t_reverse_endianness((sprite_set_id << 16) | j.id);
            }
        }
        else {
            for (SprDbFile::Spr& j : i.sprite) {
                if (!j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.uid);
                io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_spdb.write_uint32_t_reverse_endianness(((0x1000 | sprite_set_id) << 16) | j.id);
                s_spdb.align_write(0x08);
            }

            for (SprDbFile::Spr& j : i.sprite) {
                if (j.texture)
                    continue;

                s_spdb.write_uint32_t_reverse_endianness(j.uid);
                io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_spdb.write_uint32_t_reverse_endianness((sprite_set_id << 16) | j.id);
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
