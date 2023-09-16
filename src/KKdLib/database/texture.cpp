/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void texture_database_file_classic_read_inner(texture_database_file* tex_db, stream& s);
static void texture_database_file_classic_write_inner(texture_database_file* tex_db, stream& s);
static void texture_database_file_modern_read_inner(texture_database_file* tex_db, stream& s, uint32_t header_length);
static void texture_database_file_modern_write_inner(texture_database_file* tex_db, stream& s);
static int64_t texture_database_file_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, std::string& str);
static void texture_database_file_strings_push_back_check(std::vector<string_hash>& vec, std::string& str);

texture_info_file::texture_info_file() {
    id = (uint32_t)-1;
}

texture_info_file::~texture_info_file() {

}

texture_info::texture_info() {
    name_hash = hash_murmurhash_empty;
    id = (uint32_t)-1;
}

texture_info::~texture_info() {

}

texture_database_file::texture_database_file() : ready(), modern(), big_endian(), is_x() {

}

texture_database_file::~texture_database_file() {

}

void texture_database_file::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, "rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc<uint8_t>(s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                texture_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_txi = str_utils_add(path, ".txi");
        if (path_check_file_exists(path_txi)) {
            f2_struct st;
            st.read(path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
                memory_stream s_mtxi;
                s_mtxi.open(st.data);
                s_mtxi.big_endian = st.header.use_big_endian;
                texture_database_file_modern_read_inner(this, s_mtxi, st.header.length);
            }
        }
        free_def(path_txi);
    }
}

void texture_database_file::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, L"rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc<uint8_t>(s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                texture_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_add(path, L".txi");
        if (path_check_file_exists(path_txi)) {
            f2_struct st;
            st.read(path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
                memory_stream s_mtxi;
                s_mtxi.open(st.data);
                s_mtxi.big_endian = st.header.use_big_endian;
                texture_database_file_modern_read_inner(this, s_mtxi, st.header.length);
            }
        }
        free_def(path_txi);
    }
}

void texture_database_file::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        texture_database_file_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('MTXI')) {
            memory_stream s_mtxi;
            s_mtxi.open(st.data);
            s_mtxi.big_endian = st.header.use_big_endian;
            texture_database_file_modern_read_inner(this, s_mtxi, st.header.length);
        }
    }
}

void texture_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            texture_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_txi = str_utils_add(path, ".txi");
        file_stream s;
        s.open(path_txi, "wb");
        if (s.check_not_null())
            texture_database_file_modern_write_inner(this, s);
        free_def(path_txi);
    }
}

void texture_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            texture_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_add(path, L".txi");
        file_stream s;
        s.open(path_txi, L"wb");
        if (s.check_not_null())
            texture_database_file_modern_write_inner(this, s);
        free_def(path_txi);
    }
}

void texture_database_file::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        texture_database_file_classic_write_inner(this, s);
    else
        texture_database_file_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool texture_database_file::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    texture_database_file* tex_db = (texture_database_file*)data;
    tex_db->read(s.c_str(), tex_db->modern);

    return tex_db->ready;
}

texture_database::texture_database() {

}

texture_database::~texture_database() {

}

void texture_database::add(texture_database_file* tex_db_file) {
    if (!tex_db_file || !tex_db_file->ready)
        return;

    texture.reserve(tex_db_file->texture.size());

    size_t src_size = texture.size();
    size_t dst_size = src_size;

    for (texture_info_file& i : tex_db_file->texture) {
        uint32_t id = i.id;

        texture_info* info = texture.data();
        if (info) {
            size_t length = src_size;
            size_t temp;
            while (length > 0)
                if (info[temp = length / 2].id >= id)
                    length /= 2;
                else {
                    info += temp + 1;
                    length -= temp + 1;
                }

            if (info == texture.data() + src_size || id != info->id)
                info = 0;
        }

        if (!info) {
            texture_info* j_begin = texture.data() + src_size;
            texture_info* j_end = texture.data() + dst_size;
            for (texture_info* j = j_begin; j != j_end; j++)
                if (id == j->id) {
                    info = j;
                    break;
                }
        }

        if (!info) {
            texture.push_back({});
            info = &texture.back();
            dst_size++;
        }

        info->id = i.id;
        info->name.assign(i.name);
        info->name_hash = hash_string_murmurhash(info->name);
    }

    sort();
}

static size_t texture_info_radix_index_func_id(texture_info* data, size_t index) {
    return data[index].id;
}

void texture_database::clear() {
    texture.clear();
    texture.shrink_to_fit();
}

void texture_database::sort() {
    radix_sort_custom(texture.data(), texture.size(), sizeof(texture_info),
        sizeof(uint32_t), (radix_index_func)texture_info_radix_index_func_id);
}

uint32_t texture_database::get_texture_id(const char* name) const {
    if (!name)
        return -1;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (const texture_info& i : texture)
        if (name_hash == i.name_hash)
            return i.id;
    return -1;
}

const char* texture_database::get_texture_name(uint32_t id) const {
    if (id == -1)
        return 0;

    for (const texture_info& i : texture)
        if (id == i.id)
            return i.name.c_str();
    return 0;
}

static void texture_database_file_classic_read_inner(texture_database_file* tex_db, stream& s) {
    uint32_t textures_count = s.read_uint32_t();
    uint32_t textures_offset = s.read_uint32_t();

    tex_db->texture.resize(textures_count);

    texture_info_file* texture = tex_db->texture.data();

    s.position_push(textures_offset, SEEK_SET);
    for (uint32_t i = 0; i < textures_count; i++) {
        texture_info_file& info = texture[i];
        info.id = s.read_uint32_t();
        info.name = s.read_string_null_terminated_offset(s.read_uint32_t());
    }
    s.position_pop();

    tex_db->ready = true;
    tex_db->modern = false;
    tex_db->big_endian = false;
    tex_db->is_x = false;
}

static void texture_database_file_classic_write_inner(texture_database_file* tex_db, stream& s) {
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.write_uint32_t(0x90669066);
    s.align_write(0x20);

    uint32_t textures_count = (uint32_t)tex_db->texture.size();

    std::vector<int64_t> string_offsets;
    string_offsets.reserve(textures_count);

    for (texture_info_file& i : tex_db->texture) {
        string_offsets.push_back(s.get_position());
        s.write_string_null_terminated(i.name);
    }
    s.align_write(0x20);

    int64_t textures_offset = s.get_position();
    if (string_offsets.size()) {
        size_t off_idx = 0;
        for (texture_info_file& i : tex_db->texture) {
            s.write_uint32_t(i.id);
            s.write_uint32_t((uint32_t)string_offsets[off_idx++]);
        }
    }
    s.align_write(0x20);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(textures_count);
    s.write_uint32_t((uint32_t)textures_offset);
    s.position_pop();
}

static void texture_database_file_modern_read_inner(texture_database_file* tex_db, stream& s, uint32_t header_length) {
    bool big_endian = s.big_endian;
    bool is_x = true;

    s.set_position(0x04, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    uint32_t textures_count = s.read_uint32_t_reverse_endianness();
    int64_t textures_offset = s.read_offset(header_length, is_x);

    tex_db->texture.resize(textures_count);

    texture_info_file* texture = tex_db->texture.data();

    s.position_push(textures_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < textures_count; i++) {
            texture_info_file& info = texture[i];
            info.id = s.read_uint32_t_reverse_endianness();
            info.name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
        }
    else
        for (uint32_t i = 0; i < textures_count; i++) {
            texture_info_file& info = texture[i];
            info.id = s.read_uint32_t_reverse_endianness();
            info.name = s.read_string_null_terminated_offset(s.read_offset_x());
        }
    s.position_pop();

    tex_db->ready = true;
    tex_db->modern = true;
    tex_db->big_endian = big_endian;
    tex_db->is_x = is_x;
}

static void texture_database_file_modern_write_inner(texture_database_file* tex_db, stream& s) {
    bool big_endian = tex_db->big_endian;
    bool is_x = tex_db->is_x;

    memory_stream s_mtxi;
    s_mtxi.open();
    s_mtxi.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    uint32_t textures_count = (uint32_t)tex_db->texture.size();

    if (!is_x) {
        ee = { 0, 1, 8, textures_count };
        ee.append(0, 2, ENRS_DWORD);
        e.vec.push_back(ee);
        off = 8;
        off = align_val(off, 0x10);

        ee = { off, 1, 8, textures_count };
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

    s_mtxi.write_int32_t_reverse_endianness(0);
    s_mtxi.write_offset(0, 0x20, is_x);
    s_mtxi.align_write(0x10);

    int64_t textures_offset = s_mtxi.get_position();
    s_mtxi.write(textures_count * (is_x ? 0x10ULL : 0x08ULL));
    s_mtxi.align_write(0x10);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(textures_count);

    for (texture_info_file& i : tex_db->texture)
        texture_database_file_strings_push_back_check(strings, i.name);

    quicksort_string_hash(strings.data(), strings.size());
    string_offsets.reserve(strings.size());
    for (string_hash& i : strings) {
        string_offsets.push_back(s_mtxi.get_position());
        s_mtxi.write_string_null_terminated(i.str);
    }
    s_mtxi.align_write(0x10);

    s_mtxi.position_push(0x00, SEEK_SET);
    s_mtxi.write_uint32_t_reverse_endianness(textures_count);
    io_write_offset_pof_add(s_mtxi, textures_offset, 0x20, is_x, &pof);
    s_mtxi.align_write(0x10);

    if (!is_x)
        for (texture_info_file& i : tex_db->texture) {
            s_mtxi.write_uint32_t_reverse_endianness(i.id);
            int64_t offset = texture_database_file_strings_get_string_offset(strings,
                string_offsets, i.name);
            io_write_offset_f2_pof_add(s_mtxi, offset, 0x20, &pof);
        }
    else
        for (texture_info_file& i : tex_db->texture) {
            s_mtxi.write_uint32_t_reverse_endianness(i.id);
            int64_t offset = texture_database_file_strings_get_string_offset(strings,
                string_offsets, i.name);
            io_write_offset_x_pof_add(s_mtxi, offset, &pof);
        }
    s_mtxi.position_pop();

    f2_struct st;
    s_mtxi.align_write(0x10);
    s_mtxi.copy(st.data);
    s_mtxi.close();

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MTXI');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;

    st.write(s, true, tex_db->is_x);
}

inline static int64_t texture_database_file_strings_get_string_offset(std::vector<string_hash>& vec,
    std::vector<int64_t>& vec_off, std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static void texture_database_file_strings_push_back_check(std::vector<string_hash>& vec, std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return;

    vec.push_back(str);
}
