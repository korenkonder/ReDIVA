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
static int64_t sprite_database_file_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, std::string& str);
static bool sprite_database_file_strings_push_back_check(std::vector<string_hash>& vec, std::string& str);

sprite_info_file::sprite_info_file() : id(), index() {

}

sprite_info_file::~sprite_info_file() {

}

sprite_info::sprite_info() : id(), index() {
    name_hash = hash_murmurhash_empty;
}

sprite_info::~sprite_info() {

}

sprite_texture_info_file::sprite_texture_info_file() : id(), index() {

}

sprite_texture_info_file::~sprite_texture_info_file() {

}

sprite_texture_info::sprite_texture_info() : id(), index() {
    name_hash = hash_murmurhash_empty;
}

sprite_texture_info::~sprite_texture_info() {

}

sprite_set_file::sprite_set_file() : id(), index(){

}

sprite_set_file::~sprite_set_file() {

}

sprite_set::sprite_set() : id(), index() {
    name_hash = hash_murmurhash_empty;
    file_name_hash = hash_murmurhash_empty;
}

sprite_set::~sprite_set() {

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
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, "rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
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
        if (path_check_file_exists(path_spi)) {
            f2_struct st;
            st.read(path_spi);
            if (st.header.signature == reverse_endianness_uint32_t('SPDB')) {
                memory_stream s_spdb;
                s_spdb.open(st.data);
                s_spdb.big_endian = st.header.use_big_endian;
                sprite_database_file_modern_read_inner(this, s_spdb, st.header.length);
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
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, L"rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
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
        if (path_check_file_exists(path_spi)) {
            f2_struct st;
            st.read(path_spi);
            if (st.header.signature == reverse_endianness_uint32_t('SPDB')) {
                memory_stream s_spdb;
                s_spdb.open(st.data);
                s_spdb.big_endian = st.header.use_big_endian;
                sprite_database_file_modern_read_inner(this, s_spdb, st.header.length);
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
            s_spdb.big_endian = st.header.use_big_endian;
            sprite_database_file_modern_read_inner(this, s_spdb, st.header.length);
        }
    }
}

void sprite_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            sprite_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_spi = str_utils_add(path, ".spi");
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
        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            sprite_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_spi = str_utils_add(path, L".spi");
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

bool sprite_database_file::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    sprite_database_file* spr_db = (sprite_database_file*)data;
    spr_db->read(s.c_str(), spr_db->modern);

    return spr_db->ready;
}

sprite_database::sprite_database() {

}

sprite_database::~sprite_database() {

}

void sprite_database::add(sprite_database_file* spr_db_file) {
    if (!spr_db_file || !spr_db_file->ready)
        return;

    sprite_set_file* sprite_set = spr_db_file->sprite_set.data();

    size_t sprite_sets_count = this->sprite_set.size();

    this->sprite_set.reserve(spr_db_file->sprite_set.size());

    for (sprite_set_file& i : spr_db_file->sprite_set) {
        uint32_t id = i.id;

        ::sprite_set* set_info = 0;
        for (::sprite_set& j : this->sprite_set)
            if (id == j.id) {
                set_info = &j;
                break;
            }

        if (!set_info) {
            this->sprite_set.push_back({});
            set_info = &this->sprite_set.back();
        }

        set_info->id = i.id;
        set_info->name.assign(i.name);
        set_info->name_hash = hash_string_murmurhash(set_info->name);
        set_info->file_name.assign(i.file_name);
        set_info->file_name_hash = hash_string_murmurhash(set_info->file_name);

        set_info->sprite.clear();
        set_info->sprite.reserve(i.sprite.size());

        for (sprite_info_file& j : i.sprite) {
            set_info->sprite.push_back({});
            sprite_info& info = set_info->sprite.back();
            info.id = j.id;
            info.name.assign(j.name);
            info.name_hash = hash_string_murmurhash(info.name);
        }

        set_info->texture.clear();
        set_info->texture.reserve(i.texture.size());

        for (sprite_texture_info_file& j : i.texture) {
            set_info->texture.push_back({});
            sprite_texture_info& info = set_info->texture.back();
            info.id = j.id;
            info.name.assign(j.name);
            info.name_hash = hash_string_murmurhash(info.name);
        }

        set_info->index = (uint32_t)(sprite_sets_count + i.index);
    }
}

static void sprite_database_file_classic_read_inner(sprite_database_file* spr_db, stream& s) {
    uint32_t sprite_sets_count = s.read_uint32_t();
    uint32_t sprite_sets_offset = s.read_uint32_t();
    uint32_t sprites_count = s.read_uint32_t();
    uint32_t sprites_offset = s.read_uint32_t();

    spr_db->sprite_set.resize(sprite_sets_count);

    sprite_set_file* sprite_set = spr_db->sprite_set.data();

    s.position_push(sprite_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < sprite_sets_count; i++) {
        sprite_set_file& set_info = sprite_set[i];
        set_info.id = s.read_uint32_t();
        set_info.name = s.read_string_null_terminated_offset(s.read_uint32_t());
        set_info.file_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        set_info.index = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(sprites_offset, SEEK_SET);
    for (uint32_t i = 0; i < sprites_count; i++) {
        uint32_t id = s.read_uint32_t();
        uint32_t name_offset = s.read_uint32_t();
        uint32_t info = s.read_uint32_t();

        uint16_t index = (uint16_t)(info & 0xFFFF);
        uint16_t set_index = (uint16_t)((info >> 16) & 0x0FFF);

        sprite_set_file& set_info = sprite_set[set_index];

        if ((info >> 16) & 0x1000) {
            set_info.texture.push_back({});
            sprite_texture_info_file& info = set_info.texture.back();
            info.id = id;
            info.name.assign(s.read_string_null_terminated_offset(name_offset));
            info.index = index;
        }
        else {
            set_info.sprite.push_back({});
            sprite_info_file& info = set_info.sprite.back();
            info.id = id;
            info.name.assign(s.read_string_null_terminated_offset(name_offset));
            info.index = index;
        }
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
    for (sprite_set_file& i : spr_db->sprite_set)
        sprites_count += i.sprite.size() + i.texture.size();

    int64_t sprites_offset = s.get_position();
    s.write(0x0C * sprites_count);
    s.align_write(0x20);

    int64_t sprite_sets_offset = s.get_position();
    s.write(0x10 * spr_db->sprite_set.size());
    s.align_write(0x20);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(sprite_sets_count + sprites_count);

    for (sprite_set_file& i : spr_db->sprite_set) {
        for (sprite_texture_info_file& j : i.texture)
            if (sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }

        for (sprite_info_file& j : i.sprite)
            if (sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }
    }
    s.align_write(0x04);

    for (sprite_set_file& i : spr_db->sprite_set) {
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
    for (sprite_set_file& i : spr_db->sprite_set) {
        uint16_t sprite_set_index = (uint16_t)(i.index & 0x0FFF);

        for (sprite_texture_info_file& j : i.texture) {
            s.write_uint32_t(j.id);
            s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t(((0x1000 | sprite_set_index) << 16) | j.index);
        }

        for (sprite_info_file& j : i.sprite) {
            s.write_uint32_t(j.id);
            s.write_uint32_t((uint32_t)sprite_database_file_strings_get_string_offset(strings,
                string_offsets, j.name));
            s.write_uint32_t((sprite_set_index << 16) | j.index);
        }
    }
    s.position_pop();

    s.position_push(sprite_sets_offset, SEEK_SET);
    for (sprite_set_file& i : spr_db->sprite_set) {
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

    sprite_set_file* sprite_set = spr_db->sprite_set.data();

    s.position_push(sprite_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < sprite_sets_count; i++) {
            sprite_set_file& set_info = sprite_set[i];
            set_info.id = s.read_uint32_t_reverse_endianness();
            set_info.name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set_info.file_name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            set_info.index = s.read_uint32_t_reverse_endianness();
        }
    else
        for (uint32_t i = 0; i < sprite_sets_count; i++) {
            sprite_set_file& set_info = sprite_set[i];
            set_info.id = s.read_uint32_t_reverse_endianness();
            set_info.name = s.read_string_null_terminated_offset(s.read_offset_x());
            set_info.file_name = s.read_string_null_terminated_offset(s.read_offset_x());
            set_info.index = s.read_uint32_t_reverse_endianness();
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

            sprite_set_file& set_info = sprite_set[set_index];

            if ((info >> 16) & 0x1000) {
                set_info.texture.push_back({});
                sprite_texture_info_file& info = set_info.texture.back();
                info.id = id;
                info.name.assign(s.read_string_null_terminated_offset(name_offset));
                info.index = index;
            }
            else {
                set_info.sprite.push_back({});
                sprite_info_file& info = set_info.sprite.back();
                info.id = id;
                info.name.assign(s.read_string_null_terminated_offset(name_offset));
                info.index = index;
            }
        }
    else
        for (uint32_t i = 0; i < sprites_count; i++) {
            uint32_t id = s.read_uint32_t_reverse_endianness();
            int64_t name_offset = s.read_offset_x();
            uint32_t info = s.read_uint32_t_reverse_endianness();
            s.align_read(0x08);

            uint16_t index = (uint16_t)(info & 0xFFFF);
            uint16_t set_index = (uint16_t)((info >> 16) & 0x0FFF);

            sprite_set_file& set_info = sprite_set[set_index];

            if ((info >> 16) & 0x1000) {
                set_info.texture.push_back({});
                sprite_texture_info_file& info = set_info.texture.back();
                info.id = id;
                info.name.assign(s.read_string_null_terminated_offset(name_offset));
                info.index = index;
            }
            else {
                set_info.sprite.push_back({});
                sprite_info_file& info = set_info.sprite.back();
                info.id = id;
                info.name.assign(s.read_string_null_terminated_offset(name_offset));
                info.index = index;
            }
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
    for (sprite_set_file& i : spr_db->sprite_set)
        sprites_count += i.sprite.size() + i.texture.size();

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

    for (sprite_set_file& i : spr_db->sprite_set) {
        if (sprite_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s_spdb.get_position());
            s_spdb.write_string_null_terminated(i.name);
        }

        if (sprite_database_file_strings_push_back_check(strings, i.file_name)) {
            string_offsets.push_back(s_spdb.get_position());
            s_spdb.write_string_null_terminated(i.file_name);
        }
    }

    for (sprite_set_file& i : spr_db->sprite_set) {
        for (sprite_texture_info_file& j : i.texture)
            if (sprite_database_file_strings_push_back_check(strings, j.name)) {
                string_offsets.push_back(s_spdb.get_position());
                s_spdb.write_string_null_terminated(j.name);
            }

        for (sprite_info_file& j : i.sprite)
            if (sprite_database_file_strings_push_back_check(strings, j.name)) {
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
        for (sprite_set_file& i : spr_db->sprite_set) {
            s_spdb.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), 0x20, &pof);
            io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.file_name), 0x20, &pof);
            s_spdb.write_uint32_t_reverse_endianness(i.index);
        }
    else
        for (sprite_set_file& i : spr_db->sprite_set) {
            s_spdb.write_uint32_t_reverse_endianness(i.id);
            io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.name), &pof);
            io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                string_offsets, i.file_name), &pof);
            s_spdb.write_uint32_t_reverse_endianness(i.index);
        }
    s_spdb.position_pop();

    s_spdb.position_push(sprites_offset, SEEK_SET);
    for (sprite_set_file& i : spr_db->sprite_set) {
        uint16_t sprite_set_index = (uint16_t)(i.index & 0x0FFF);

        if (!is_x)
            for (sprite_texture_info_file& j : i.texture) {
                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_spdb.write_uint32_t_reverse_endianness(((0x1000 | sprite_set_index) << 16) | j.index);
            }
        else
            for (sprite_texture_info_file& j : i.texture) {
                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_spdb.write_uint32_t_reverse_endianness(((0x1000 | sprite_set_index) << 16) | j.index);
                s_spdb.align_write(0x08);
            }

        if (!is_x)
            for (sprite_info_file& j : i.sprite) {
                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_f2_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), 0x20, &pof);
                s_spdb.write_uint32_t_reverse_endianness((sprite_set_index << 16) | j.index);
            }
        else
            for (sprite_info_file& j : i.sprite) {
                s_spdb.write_uint32_t_reverse_endianness(j.id);
                io_write_offset_x_pof_add(s_spdb, sprite_database_file_strings_get_string_offset(strings,
                    string_offsets, j.name), &pof);
                s_spdb.write_uint32_t_reverse_endianness((sprite_set_index << 16) | j.index);
                s_spdb.align_write(0x08);
            }
    }
    s_spdb.position_pop();

    f2_struct st;
    s_spdb.align_write(0x10);
    s_spdb.copy(st.data);
    s_spdb.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('SPDB');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;

    st.write(s, true, spr_db->is_x);
}

inline static int64_t sprite_database_file_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool sprite_database_file_strings_push_back_check(std::vector<string_hash>& vec, std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return false;

    vec.push_back(str);
    return true;
}
