/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../hash.h"
#include "../str_utils.h"

vector_func(texture_info)

static void texture_database_classic_read_inner(texture_database* tex_db, stream* s);
static void texture_database_classic_write_inner(texture_database* tex_db, stream* s);
static void texture_database_modern_read_inner(texture_database* tex_db, stream* s, uint32_t header_length);
static void texture_database_modern_write_inner(texture_database* tex_db, stream* s);
static ssize_t texture_database_strings_get_string_offset(vector_string* vec,
    vector_ssize_t* vec_off, char* str);
static void texture_database_strings_push_back_check(vector_string* vec, char* str);

void texture_database_init(texture_database* tex_db) {
    memset(tex_db, 0, sizeof(texture_database));
}

void texture_database_read(texture_database* tex_db, char* path, bool modern) {
    if (!tex_db || !path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc(s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_mopen(&s_bin, data, s.length);
                texture_database_classic_read_inner(tex_db, &s_bin);
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
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')); {
                stream s_mtxi;
                io_mopen(&s_mtxi, st.data, st.length);
                s_mtxi.is_big_endian = st.header.use_big_endian;
                texture_database_modern_read_inner(tex_db, &s_mtxi, st.header.length);
                io_free(&s_mtxi);
            }
            f2_struct_free(&st);
        }
        free(path_txi);
    }
}

void texture_database_wread(texture_database* tex_db, wchar_t* path, bool modern) {
    if (!tex_db || !path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        if (path_wcheck_file_exists(path_bin)) {
            stream s;
            io_wopen(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc(s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_mopen(&s_bin, data, s.length);
                texture_database_classic_read_inner(tex_db, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_wadd(path, L".txi");
        if (path_wcheck_file_exists(path_txi)) {
            f2_struct st;
            f2_struct_wread(&st, path_txi);
            if (st.header.signature == reverse_endianness_uint32_t('MTXI')); {
                stream s_mtxi;
                io_mopen(&s_mtxi, st.data, st.length);
                s_mtxi.is_big_endian = st.header.use_big_endian;
                texture_database_modern_read_inner(tex_db, &s_mtxi, st.header.length);
                io_free(&s_mtxi);
            }
            f2_struct_free(&st);
        }
        free(path_txi);
    }
}

void texture_database_mread(texture_database* tex_db, void* data, size_t length, bool modern) {
    if (!tex_db || !data || !length)
        return;

    if (!modern) {
        stream s;
        io_mopen(&s, data, length);
        texture_database_classic_read_inner(tex_db, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        f2_struct_mread(&st, data, length);
        if (st.header.signature == reverse_endianness_uint32_t('MTXI')); {
            stream s_mtxi;
            io_mopen(&s_mtxi, st.data, st.length);
            s_mtxi.is_big_endian = st.header.use_big_endian;
            texture_database_modern_read_inner(tex_db, &s_mtxi, st.header.length);
            io_free(&s_mtxi);
        }
        f2_struct_free(&st);
    }
}

void texture_database_write(texture_database* tex_db, char* path) {
    if (!tex_db || !path || !tex_db->ready)
        return;

    if (!tex_db->modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            texture_database_classic_write_inner(tex_db, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_txi = str_utils_add(path, ".txi");
        stream s;
        io_open(&s, path_txi, "wb");
        if (s.io.stream)
            texture_database_modern_write_inner(tex_db, &s);
        io_free(&s);
        free(path_txi);
    }
}

void texture_database_wwrite(texture_database* tex_db, wchar_t* path) {
    if (!tex_db || !path || !tex_db->ready)
        return;

    if (!tex_db->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_wopen(&s, path_bin, L"wb");
        if (s.io.stream)
            texture_database_classic_write_inner(tex_db, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_txi = str_utils_wadd(path, L".txi");
        stream s;
        io_wopen(&s, path_txi, L"wb");
        if (s.io.stream)
            texture_database_modern_write_inner(tex_db, &s);
        io_free(&s);
        free(path_txi);
    }
}

void texture_database_mwrite(texture_database* tex_db, void** data, size_t* length) {
    if (!tex_db || !data || !tex_db->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    if (!tex_db->modern)
        texture_database_classic_write_inner(tex_db, &s);
    else
        texture_database_modern_write_inner(tex_db, &s);
    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool texture_database_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    texture_database* tex_db = data;
    texture_database_read(tex_db, string_data(&s), tex_db->modern);

    string_free(&s);
    return tex_db->ready;
}

void texture_database_merge_mdata(texture_database* tex_db,
    texture_database* base_tex_db, texture_database* mdata_tex_db) {
    if (!tex_db || !base_tex_db || !mdata_tex_db
        || !base_tex_db->ready || !mdata_tex_db->ready)
        return;

    vector_texture_info* texture = &tex_db->texture;
    vector_texture_info* base_texture = &base_tex_db->texture;
    vector_texture_info* mdata_texture = &mdata_tex_db->texture;

    int32_t count = (int32_t)(base_texture->end - base_texture->begin);
    vector_texture_info_reserve(texture, count);
    texture->end += count;

    for (int32_t i = 0; i < count; i++) {
        texture_info* b_info = &base_texture->begin[i];
        texture_info* info = &texture->begin[i];

        info->id = b_info->id;
        string_copy(&b_info->name, &info->name);
        info->name_hash = hash_fnv1a64m(string_data(&info->name), info->name.length);
    }

    int32_t mdata_count = (int32_t)(mdata_texture->end - mdata_texture->begin);
    for (int32_t i = 0; i < mdata_count; i++) {
        texture_info* m_info = &mdata_texture->begin[i];

        uint32_t id = m_info->id;

        texture_info* info = 0;
        for (info = texture->begin; info != texture->end; info++)
            if (id == info->id)
                break;

        if (info == texture->end)
            info = vector_texture_info_reserve_back(texture);

        info->id = m_info->id;
        string_replace(&m_info->name, &info->name);
        info->name_hash = hash_fnv1a64m(string_data(&info->name), info->name.length);
    }

    tex_db->ready = true;
}

void texture_database_split_mdata(texture_database* tex_db,
    texture_database* base_tex_db, texture_database* mdata_tex_db) {
    if (!tex_db || !base_tex_db || !mdata_tex_db
        || !tex_db->ready || !mdata_tex_db->ready)
        return;

}

uint32_t texture_database_get_texture_id(texture_database* tex_db, char* name) {
    if (!tex_db || !name)
        return -1;

    uint64_t name_hash = hash_fnv1a64m(name, utf8_length(name));

    for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++)
        if (name_hash == i->name_hash)
            return i->id;
    return -1;
}

char* texture_database_get_texture_name(texture_database* tex_db, uint32_t id) {
    if (!tex_db || id == -1)
        return 0;

    for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++)
        if (id == i->id)
            return string_data(&i->name);
    return 0;
}

void texture_database_free(texture_database* tex_db) {
    vector_texture_info_free(&tex_db->texture, texture_info_free);
}

void texture_info_free(texture_info* info) {
    string_free(&info->name);
}

static void texture_database_classic_read_inner(texture_database* tex_db, stream* s) {
    uint32_t texture_count = io_read_uint32_t(s);
    uint32_t textures_offset = io_read_uint32_t(s);

    vector_texture_info_reserve(&tex_db->texture, texture_count);
    tex_db->texture.end += texture_count;

    io_position_push(s, textures_offset, SEEK_SET);
    for (uint32_t i = 0; i < texture_count; i++) {
        texture_info* info = &tex_db->texture.begin[i];
        info->id = io_read_uint32_t(s);
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &info->name);
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

    uint32_t texture_count = (uint32_t)(tex_db->texture.end - tex_db->texture.begin);

    vector_ssize_t string_offsets = vector_empty(ssize_t);
    vector_ssize_t_reserve(&string_offsets, texture_count);

    for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++) {
        ssize_t off = io_get_position(s);
        io_write_string_null_terminated(s, &i->name);
        vector_ssize_t_push_back(&string_offsets, &off);
    }
    io_align_write(s, 0x20);

    ssize_t textures_offset = io_get_position(s);
    size_t off_idx = 0;
    for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++) {
        io_write_uint32_t(s, i->id);
        io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
    }
    io_align_write(s, 0x20);

    vector_ssize_t_free(&string_offsets, 0);

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, texture_count);
    io_write_uint32_t(s, (uint32_t)textures_offset);
    io_position_pop(s);
}

static void texture_database_modern_read_inner(texture_database* tex_db, stream* s, uint32_t header_length) {
    bool is_x = true;

    io_set_position(s, 0x04, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;

    io_set_position(s, 0x00, SEEK_SET);

    uint32_t texture_count = io_read_uint32_t_stream_reverse_endianness(s);
    ssize_t textures_offset = io_read_offset(s, header_length, is_x);

    vector_texture_info_reserve(&tex_db->texture, texture_count);
    tex_db->texture.end += texture_count;

    io_position_push(s, textures_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < texture_count; i++) {
            texture_info* info = &tex_db->texture.begin[i];
            info->id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &info->name);
        }
    else
        for (uint32_t i = 0; i < texture_count; i++) {
            texture_info* info = &tex_db->texture.begin[i];
            info->id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &info->name);
        }
    io_position_pop(s);

    tex_db->is_x = is_x;
    tex_db->modern = true;
    tex_db->ready = true;
}

static void texture_database_modern_write_inner(texture_database* tex_db, stream* s) {
    stream s_mtxi;
    io_mopen(&s_mtxi, 0, 0);
    uint32_t off;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;
    vector_size_t pof = vector_empty(size_t);

    bool is_x = tex_db->is_x;

    uint32_t texture_count = (uint32_t)(tex_db->texture.end - tex_db->texture.begin);

    if (!is_x) {
        ee = (enrs_entry){ 0, 2, 8, texture_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 8;
        off = align_val(off, 0x10);

        ee = (enrs_entry){ off, 2, 8, texture_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(texture_count * 16ULL);
    }
    else {
        texture_count++;
        ee = (enrs_entry){ 0, 2, 16, texture_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(texture_count * 16ULL);
        texture_count--;
    }

    io_write_int32_t(&s_mtxi, 0);
    io_write_offset(&s_mtxi, 0, 0x20, is_x);
    io_align_write(&s_mtxi, 0x10);

    ssize_t textures_offset = io_get_position(&s_mtxi);
    io_write(&s_mtxi, 0, texture_count * (is_x ? 0x10ULL : 0x08ULL));
    io_align_write(&s_mtxi, 0x10);

    vector_string strings = vector_empty(string);
    vector_ssize_t string_offsets = vector_empty(ssize_t);

    vector_string_reserve(&strings, texture_count);

    for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++)
        texture_database_strings_push_back_check(&strings, string_data(&i->name));

    for (string* i = strings.begin; i != &strings.end[-1]; i++) {
        char* i_str = string_data(i);
        for (string* j = i + 1; j != strings.end; j++)
            if (str_utils_compare(i_str, string_data(j)) > 0) {
                string temp = *i;
                *i = *j;
                *j = temp;
                i_str = string_data(i);
            }
    }

    vector_ssize_t_reserve(&string_offsets, strings.end - strings.begin);

    for (string* i = strings.begin; i != strings.end; i++) {
        *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_mtxi);
        io_write_string_null_terminated(&s_mtxi, i);
    }
    io_align_write(&s_mtxi, 0x10);

    io_position_push(&s_mtxi, 0x00, SEEK_SET);
    io_write_uint32_t(&s_mtxi, texture_count);
    io_write_offset_pof_add(&s_mtxi, textures_offset, 0x20, is_x, &pof);
    io_align_write(&s_mtxi, 0x10);

    if (!is_x)
        for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++) {
            io_write_uint32_t(&s_mtxi, i->id);
            ssize_t offset = texture_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->name));
            io_write_offset_f2_pof_add(&s_mtxi, offset, 0x20, &pof);
        }
    else
        for (texture_info* i = tex_db->texture.begin; i != tex_db->texture.end; i++) {
            io_write_uint32_t(&s_mtxi, i->id);
            ssize_t offset = texture_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->name));
            io_write_offset_x_pof_add(&s_mtxi, offset, &pof);
        }
    io_position_pop(&s_mtxi);

    vector_string_free(&strings, string_free);
    vector_ssize_t_free(&string_offsets, 0);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    io_align_write(&s_mtxi, 0x10);
    io_mcopy(&s_mtxi, &st.data, &st.length);
    io_free(&s_mtxi);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MTXI');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    f2_struct_swrite(&st, s, true, tex_db->is_x);
    f2_struct_free(&st);
}

inline static ssize_t texture_database_strings_get_string_offset(vector_string* vec,
    vector_ssize_t* vec_off, char* str) {
    size_t len = utf8_length(str);
    for (string* i = vec->begin; i != vec->end; i++)
        if (!memcmp(str, string_data(i), min(len, i->length) + 1))
            return vec_off->begin[i - vec->begin];
    return 0;
}

inline static void texture_database_strings_push_back_check(vector_string* vec, char* str) {
    size_t len = utf8_length(str);
    for (string* i = vec->begin; i != vec->end; i++)
        if (!memcmp(str, string_data(i), min(len, i->length) + 1))
            return;

    string* s = vector_string_reserve_back(vec);
    string_init_length(s, str, len);
}
