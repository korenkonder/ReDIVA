/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../hash.h"
#include "../sort.h"
#include "../str_utils.h"

static void texture_database_classic_read_inner(texture_database* tex_db, stream* s);
static void texture_database_classic_write_inner(texture_database* tex_db, stream* s);
static void texture_database_modern_read_inner(texture_database* tex_db, stream* s, uint32_t header_length);
static void texture_database_modern_write_inner(texture_database* tex_db, stream* s);
static ssize_t texture_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<ssize_t>& vec_off, std::string& str);
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
            f2_struct_read(&st, path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
                stream s_mtxi;
                io_open(&s_mtxi, st.data, st.length);
                s_mtxi.is_big_endian = st.header.use_big_endian;
                texture_database_modern_read_inner(this, &s_mtxi, st.header.length);
                io_free(&s_mtxi);
            }
            f2_struct_free(&st);
        }
        free(path_txi);
    }
}

void texture_database::read(const wchar_t* path, bool modern) {
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
                texture_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_wadd(path, L".txi");
        if (path_check_file_exists(path_txi)) {
            f2_struct st;
            f2_struct_read(&st, path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
                stream s_mtxi;
                io_open(&s_mtxi, st.data, st.length);
                s_mtxi.is_big_endian = st.header.use_big_endian;
                texture_database_modern_read_inner(this, &s_mtxi, st.header.length);
                io_free(&s_mtxi);
            }
            f2_struct_free(&st);
        }
        free(path_txi);
    }
}

void texture_database::read(const void* data, size_t length, bool modern) {
    if (!data || !length)
        return;

    if (!modern) {
        stream s;
        io_open(&s, data, length);
        texture_database_classic_read_inner(this, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        f2_struct_read(&st, data, length);
        if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
            stream s_mtxi;
            io_open(&s_mtxi, st.data, st.length);
            s_mtxi.is_big_endian = st.header.use_big_endian;
            texture_database_modern_read_inner(this, &s_mtxi, st.header.length);
            io_free(&s_mtxi);
        }
        f2_struct_free(&st);
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
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_open(&s, path_bin, L"wb");
        if (s.io.stream)
            texture_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_wadd(path, L".txi");
        stream s;
        io_open(&s, path_txi, L"wb");
        if (s.io.stream)
            texture_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_txi);
    }
}

void texture_database::write(void** data, size_t* length) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    if (!modern)
        texture_database_classic_write_inner(this, &s);
    else
        texture_database_modern_write_inner(this, &s);
    io_align_write(&s, 0x10);
    io_copy(&s, data, length);
    io_free(&s);
}

void texture_database::merge_mdata(texture_database* base_tex_db,
    texture_database* mdata_tex_db) {
    if (!base_tex_db || !mdata_tex_db
        || !base_tex_db->ready || !mdata_tex_db->ready)
        return;

    texture.reserve(base_tex_db->texture.size());

    for (texture_info& i : base_tex_db->texture) {
        texture_info info;
        info.id = i.id;
        info.name = i.name;
        info.name_hash = hash_string_fnv1a64m(&i.name, false);
        texture.push_back(info);
    }

    for (texture_info& i : mdata_tex_db->texture) {
        uint32_t id = i.id;

        texture_info* info_ptr = 0;
        for (texture_info& j : texture)
            if (id == j.id) {
                info_ptr = &j;
                break;
            }

        texture_info info;
        info.id = i.id;
        info.name = i.name;
        info.name_hash = hash_string_fnv1a64m(&i.name, false);

        if (info_ptr)
            *info_ptr = info;
        else
            texture.push_back(info);
    }

    ready = true;
}

void texture_database::split_mdata(texture_database* base_tex_db,
    texture_database* mdata_tex_db) {
    if (!base_tex_db || !mdata_tex_db
        || !ready || !mdata_tex_db->ready)
        return;

}

uint32_t texture_database::get_texture_id(char* name) {
    if (!name)
        return -1;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (texture_info& i : texture)
        if (name_hash == i.name_hash)
            return i.id;
    return -1;
}

uint32_t texture_database::get_texture_id(const char* name) {
    if (!name)
        return -1;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

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

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    texture_database* tex_db = (texture_database*)data;
    tex_db->read(string_data(&s), tex_db->modern);

    string_free(&s);
    return tex_db->ready;
}


texture_info::texture_info() :  name_hash(hash_fnv1a64m_empty), id((uint32_t)-1) {

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

    vector_old_ssize_t string_offsets = vector_old_empty(ssize_t);
    vector_old_ssize_t_reserve(&string_offsets, textures_count);

    for (texture_info& i : tex_db->texture) {
        ssize_t off = io_get_position(s);
        io_write_string_null_terminated(s, &i.name);
        vector_old_ssize_t_push_back(&string_offsets, &off);
    }
    io_align_write(s, 0x20);

    ssize_t textures_offset = io_get_position(s);
    if (string_offsets.begin) {
        size_t off_idx = 0;
        for (texture_info& i : tex_db->texture) {
            io_write_uint32_t(s, i.id);
            io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
        }
    }
    io_align_write(s, 0x20);

    vector_old_ssize_t_free(&string_offsets, 0);

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
    ssize_t textures_offset = io_read_offset(s, header_length, is_x);

    tex_db->texture.resize(textures_count);

    io_position_push(s, textures_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < textures_count; i++) {
            texture_info& info = tex_db->texture[i];
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &info.name);
        }
    else
        for (uint32_t i = 0; i < textures_count; i++) {
            texture_info& info = tex_db->texture[i];
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &info.name);
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
    vector_old_enrs_entry e = vector_old_empty(enrs_entry);
    enrs_entry ee;
    vector_old_size_t pof = vector_old_empty(size_t);

    bool is_x = tex_db->is_x;

    uint32_t textures_count = (uint32_t)tex_db->texture.size();

    if (!is_x) {
        ee = { 0, 2, 8, textures_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 9, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = 8;
        off = align_val(off, 0x10);

        ee = { off, 2, 8, textures_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(textures_count * 16ULL);
    }
    else {
        textures_count++;
        ee = { 0, 2, 16, textures_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 1, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(textures_count * 16ULL);
        textures_count--;
    }

    io_write_int32_t(&s_mtxi, 0);
    io_write_offset(&s_mtxi, 0, 0x20, is_x);
    io_align_write(&s_mtxi, 0x10);

    ssize_t textures_offset = io_get_position(&s_mtxi);
    io_write(&s_mtxi, textures_count * (is_x ? 0x10ULL : 0x08ULL));
    io_align_write(&s_mtxi, 0x10);

    std::vector<std::string> strings;
    std::vector<ssize_t> string_offsets;

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
            ssize_t offset = texture_database_strings_get_string_offset(strings,
                string_offsets, i.name);
            io_write_offset_f2_pof_add(&s_mtxi, offset, 0x20, &pof);
        }
    else
        for (texture_info& i : tex_db->texture) {
            io_write_uint32_t(&s_mtxi, i.id);
            ssize_t offset = texture_database_strings_get_string_offset(strings,
                string_offsets, i.name);
            io_write_offset_x_pof_add(&s_mtxi, offset, &pof);
        }
    io_position_pop(&s_mtxi);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    io_align_write(&s_mtxi, 0x10);
    io_copy(&s_mtxi, &st.data, &st.length);
    io_free(&s_mtxi);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MTXI');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    f2_struct_write(&st, s, true, tex_db->is_x);
    f2_struct_free(&st);
}

inline static ssize_t texture_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<ssize_t>& vec_off, std::string& str) {
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
