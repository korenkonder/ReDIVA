/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.hpp"
#include "../f2/struct.hpp"
#include "../io/path.h"
#include "../io/stream.h"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.h"

static void texture_database_classic_read_inner(texture_database* tex_db, stream* s);
static void texture_database_classic_write_inner(texture_database* tex_db, stream* s);
static void texture_database_modern_read_inner(texture_database* tex_db, stream* s, uint32_t header_length);
static void texture_database_modern_write_inner(texture_database* tex_db, stream* s);
static int64_t texture_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, std::string& str);
static void texture_database_strings_push_back_check(std::vector<std::string>& vec, std::string& str);

texture_database::texture_database() : ready(), modern(), is_x() {

}

texture_database::~texture_database() {

}

void texture_database::read(const char* path, bool modern) {
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
                texture_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        char* path_txi = str_utils_add(path, ".txi");
        if (path_check_file_exists(path_txi)) {
            f2_struct st;
            st.read(path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
                stream s_mtxi;
                io_open(&s_mtxi, &st.data);
                s_mtxi.is_big_endian = st.header.use_big_endian;
                texture_database_modern_read_inner(this, &s_mtxi, st.header.length);
                io_free(&s_mtxi);
            }
        }
        free(path_txi);
    }
}

void texture_database::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                texture_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_add(path, L".txi");
        if (path_check_file_exists(path_txi)) {
            f2_struct st;
            st.read(path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
                stream s_mtxi;
                io_open(&s_mtxi, &st.data);
                s_mtxi.is_big_endian = st.header.use_big_endian;
                texture_database_modern_read_inner(this, &s_mtxi, st.header.length);
                io_free(&s_mtxi);
            }
        }
        free(path_txi);
    }
}

void texture_database::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        stream s;
        io_open(&s, data, size);
        texture_database_classic_read_inner(this, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
            stream s_mtxi;
            io_open(&s_mtxi, &st.data);
            s_mtxi.is_big_endian = st.header.use_big_endian;
            texture_database_modern_read_inner(this, &s_mtxi, st.header.length);
            io_free(&s_mtxi);
        }
    }
}

void texture_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            texture_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_txi = str_utils_add(path, ".txi");
        stream s;
        io_open(&s, path_txi, "wb");
        if (s.io.stream)
            texture_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_txi);
    }
}

void texture_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        stream s;
        io_open(&s, path_bin, L"wb");
        if (s.io.stream)
            texture_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_add(path, L".txi");
        stream s;
        io_open(&s, path_txi, L"wb");
        if (s.io.stream)
            texture_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_txi);
    }
}

void texture_database::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
    io_open(&s);
    if (!modern)
        texture_database_classic_write_inner(this, &s);
    else
        texture_database_modern_write_inner(this, &s);
    io_align_write(&s, 0x10);
    io_copy(&s, data, size);
    io_free(&s);
}

void texture_database::merge_mdata(texture_database* base_tex_db,
    texture_database* mdata_tex_db) {
    if (!base_tex_db || !mdata_tex_db
        || !base_tex_db->ready || !mdata_tex_db->ready)
        return;

    if (this != base_tex_db)
        texture = base_tex_db->texture;

    for (texture_info& i : mdata_tex_db->texture) {
        uint32_t id = i.id;

        texture_info* info = 0;
        for (texture_info& j : texture)
            if (id == j.id) {
                info = &j;
                break;
            }

        if (info)
            *info = i;
        else
            texture.push_back(i);
    }

    if (this != base_tex_db) {
        ready = true;
        modern = base_tex_db->modern;
        is_x = base_tex_db->is_x;
    }
}

void texture_database::split_mdata(texture_database* base_tex_db,
    texture_database* mdata_tex_db) {
    if (!base_tex_db || !mdata_tex_db
        || !ready || !mdata_tex_db->ready)
        return;

}

uint32_t texture_database::get_texture_id(const char* name) {
    if (!name)
        return -1;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (texture_info& i : texture)
        if (name_hash == i.name_hash)
            return i.id;
    return -1;
}

const char* texture_database::get_texture_name(uint32_t id) {
    if (id == -1)
        return 0;

    for (texture_info& i : texture)
        if (id == i.id)
            return i.name.c_str();
    return 0;
}

bool texture_database::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    texture_database* tex_db = (texture_database*)data;
    tex_db->read(s.c_str(), tex_db->modern);

    return tex_db->ready;
}

texture_info::texture_info() : name_hash(hash_murmurhash_empty), id((uint32_t)-1) {

}

texture_info::~texture_info() {

}

static void texture_database_classic_read_inner(texture_database* tex_db, stream* s) {
    uint32_t textures_count = io_read_uint32_t(s);
    uint32_t textures_offset = io_read_uint32_t(s);

    tex_db->texture.resize(textures_count);

    io_position_push(s, textures_offset, SEEK_SET);
    for (uint32_t i = 0; i < textures_count; i++) {
        texture_info& info = tex_db->texture[i];
        info.id = io_read_uint32_t(s);
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &info.name);
        info.name_hash = hash_string_murmurhash(&info.name);
    }
    io_position_pop(s);

    tex_db->is_x = false;
    tex_db->modern = false;
    tex_db->ready = true;
}

static void texture_database_classic_write_inner(texture_database* tex_db, stream* s) {
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_align_write(s, 0x20);

    uint32_t textures_count = (uint32_t)tex_db->texture.size();

    std::vector<int64_t> string_offsets;
    string_offsets.reserve(textures_count);

    for (texture_info& i : tex_db->texture) {
        string_offsets.push_back(io_get_position(s));
        io_write_string_null_terminated(s, &i.name);
    }
    io_align_write(s, 0x20);

    int64_t textures_offset = io_get_position(s);
    if (string_offsets.size()) {
        size_t off_idx = 0;
        for (texture_info& i : tex_db->texture) {
            io_write_uint32_t(s, i.id);
            io_write_uint32_t(s, (uint32_t)string_offsets[off_idx++]);
        }
    }
    io_align_write(s, 0x20);

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, textures_count);
    io_write_uint32_t(s, (uint32_t)textures_offset);
    io_position_pop(s);
}

static void texture_database_modern_read_inner(texture_database* tex_db, stream* s, uint32_t header_length) {
    bool is_x = true;

    io_set_position(s, 0x04, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;

    io_set_position(s, 0x00, SEEK_SET);

    uint32_t textures_count = io_read_uint32_t_stream_reverse_endianness(s);
    int64_t textures_offset = io_read_offset(s, header_length, is_x);

    tex_db->texture.resize(textures_count);

    io_position_push(s, textures_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < textures_count; i++) {
            texture_info& info = tex_db->texture[i];
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &info.name);
            info.name_hash = hash_string_murmurhash(&info.name);
        }
    else
        for (uint32_t i = 0; i < textures_count; i++) {
            texture_info& info = tex_db->texture[i];
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &info.name);
            info.name_hash = hash_string_murmurhash(&info.name);
        }
    io_position_pop(s);

    tex_db->is_x = is_x;
    tex_db->modern = true;
    tex_db->ready = true;
}

static void texture_database_modern_write_inner(texture_database* tex_db, stream* s) {
    stream s_mtxi;
    io_open(&s_mtxi);
    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    bool is_x = tex_db->is_x;

    uint32_t textures_count = (uint32_t)tex_db->texture.size();

    if (!is_x) {
        ee = { 0, 2, 8, textures_count };
        ee.append(0, 9, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 8;
        off = align_val(off, 0x10);

        ee = { off, 2, 8, textures_count };
        ee.append(0, 2, ENRS_DWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(textures_count * 16ULL);
    }
    else {
        textures_count++;
        ee = { 0, 2, 16, textures_count };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(textures_count * 16ULL);
        textures_count--;
    }

    io_write_int32_t(&s_mtxi, 0);
    io_write_offset(&s_mtxi, 0, 0x20, is_x);
    io_align_write(&s_mtxi, 0x10);

    int64_t textures_offset = io_get_position(&s_mtxi);
    io_write(&s_mtxi, textures_count * (is_x ? 0x10ULL : 0x08ULL));
    io_align_write(&s_mtxi, 0x10);

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(textures_count);

    for (texture_info& i : tex_db->texture)
        texture_database_strings_push_back_check(strings, i.name);

    quicksort_string(strings.data(), strings.size());
    string_offsets.reserve(strings.size());
    for (std::string& i : strings) {
        string_offsets.push_back(io_get_position(&s_mtxi));
        io_write_string_null_terminated(&s_mtxi, &i);
    }
    io_align_write(&s_mtxi, 0x10);

    io_position_push(&s_mtxi, 0x00, SEEK_SET);
    io_write_uint32_t(&s_mtxi, textures_count);
    io_write_offset_pof_add(&s_mtxi, textures_offset, 0x20, is_x, &pof);
    io_align_write(&s_mtxi, 0x10);

    if (!is_x)
        for (texture_info& i : tex_db->texture) {
            io_write_uint32_t(&s_mtxi, i.id);
            int64_t offset = texture_database_strings_get_string_offset(strings,
                string_offsets, i.name);
            io_write_offset_f2_pof_add(&s_mtxi, offset, 0x20, &pof);
        }
    else
        for (texture_info& i : tex_db->texture) {
            io_write_uint32_t(&s_mtxi, i.id);
            int64_t offset = texture_database_strings_get_string_offset(strings,
                string_offsets, i.name);
            io_write_offset_x_pof_add(&s_mtxi, offset, &pof);
        }
    io_position_pop(&s_mtxi);

    f2_struct st;
    io_align_write(&s_mtxi, 0x10);
    io_copy(&s_mtxi, &st.data);
    io_free(&s_mtxi);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MTXI');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    st.write(s, true, tex_db->is_x);
}

inline static int64_t texture_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, std::string& str) {
    for (std::string& i : vec)
        if (str == i)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static void texture_database_strings_push_back_check(std::vector<std::string>& vec, std::string& str) {
    for (std::string& i : vec)
        if (str == i)
            return;

    vec.push_back(str);
}
