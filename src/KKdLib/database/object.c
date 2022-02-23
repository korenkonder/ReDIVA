/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../hash.h"
#include "../sort.h"
#include "../str_utils.h"

vector_old_func(object_info)
vector_old_func(object_info_data)
vector_old_func(object_set_info)

static void object_database_classic_read_inner(object_database* obj_db, stream* s);
static void object_database_classic_write_inner(object_database* obj_db, stream* s);
static void object_database_modern_read_inner(object_database* obj_db, stream* s, uint32_t header_length);
static void object_database_modern_write_inner(object_database* obj_db, stream* s);
static ssize_t object_database_strings_get_string_offset(vector_old_string* vec,
    vector_old_ssize_t* vec_off, char* str);
static void object_database_strings_push_back_check(vector_old_string* vec, char* str);

const object_info object_info_null = { (uint32_t)-1, (uint32_t)-1 };

void object_database_init(object_database* obj_db) {
    memset(obj_db, 0, sizeof(object_database));
}

void object_database_read(object_database* obj_db, char* path, bool modern) {
    if (!obj_db || !path)
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
                io_mopen(&s_bin, data, s.length);
                object_database_classic_read_inner(obj_db, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        if (path_check_file_exists(path_osi)) {
            f2_struct st;
            f2_struct_read(&st, path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                stream s_mosi;
                io_mopen(&s_mosi, st.data, st.length);
                s_mosi.is_big_endian = st.header.use_big_endian;
                object_database_modern_read_inner(obj_db, &s_mosi, st.header.length);
                io_free(&s_mosi);
            }
            f2_struct_free(&st);
        }
        free(path_osi);
    }
}

void object_database_wread(object_database* obj_db, wchar_t* path, bool modern) {
    if (!obj_db || !path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        if (path_wcheck_file_exists(path_bin)) {
            stream s;
            io_wopen(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_mopen(&s_bin, data, s.length);
                object_database_classic_read_inner(obj_db, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_wadd(path, L".osi");
        if (path_wcheck_file_exists(path_osi)) {
            f2_struct st;
            f2_struct_wread(&st, path_osi);
            if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
                stream s_mosi;
                io_mopen(&s_mosi, st.data, st.length);
                s_mosi.is_big_endian = st.header.use_big_endian;
                object_database_modern_read_inner(obj_db, &s_mosi, st.header.length);
                io_free(&s_mosi);
            }
            f2_struct_free(&st);
        }
        free(path_osi);
    }
}

void object_database_mread(object_database* obj_db, void* data, size_t length, bool modern) {
    if (!obj_db || !data || !length)
        return;

    if (!modern) {
        stream s;
        io_mopen(&s, data, length);
        object_database_classic_read_inner(obj_db, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        f2_struct_mread(&st, data, length);
        if (st.header.signature == reverse_endianness_uint32_t('MOSI')) {
            stream s_mosi;
            io_mopen(&s_mosi, st.data, st.length);
            s_mosi.is_big_endian = st.header.use_big_endian;
            object_database_modern_read_inner(obj_db, &s_mosi, st.header.length);
            io_free(&s_mosi);
        }
        f2_struct_free(&st);
    }
}

void object_database_write(object_database* obj_db, char* path) {
    if (!obj_db || !path || !obj_db->ready)
        return;

    if (!obj_db->modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            object_database_classic_write_inner(obj_db, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_osi = str_utils_add(path, ".osi");
        stream s;
        io_open(&s, path_osi, "wb");
        if (s.io.stream)
            object_database_modern_write_inner(obj_db, &s);
        io_free(&s);
        free(path_osi);
    }
}

void object_database_wwrite(object_database* obj_db, wchar_t* path) {
    if (!obj_db || !path || !obj_db->ready)
        return;

    if (!obj_db->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_wopen(&s, path_bin, L"wb");
        if (s.io.stream)
            object_database_classic_write_inner(obj_db, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_osi = str_utils_wadd(path, L".osi");
        stream s;
        io_wopen(&s, path_osi, L"wb");
        if (s.io.stream)
            object_database_modern_write_inner(obj_db, &s);
        io_free(&s);
        free(path_osi);
    }
}

void object_database_mwrite(object_database* obj_db, void** data, size_t* length) {
    if (!obj_db || !data || !obj_db->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    if (!obj_db->modern)
        object_database_classic_write_inner(obj_db, &s);
    else
        object_database_modern_write_inner(obj_db, &s);
    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool object_database_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    object_database* obj_db = (object_database*)data;
    object_database_read(obj_db, string_data(&s), obj_db->modern);

    string_free(&s);
    return obj_db->ready;
}

void object_database_merge_mdata(object_database* obj_db,
    object_database* base_obj_db, object_database* mdata_obj_db) {
    if (!obj_db || !base_obj_db || !mdata_obj_db
        || !base_obj_db->ready || !mdata_obj_db->ready)
        return;

    vector_old_string* bone_name = &obj_db->bone_name;
    vector_old_string* base_bone_name = &base_obj_db->bone_name;

    int32_t bone_count = (int32_t)vector_old_length(*base_bone_name);
    vector_old_string_reserve(bone_name, bone_count);
    bone_name->end += bone_count;

    for (int32_t i = 0; i < bone_count; i++)
        string_copy(&base_bone_name->begin[i], &bone_name->begin[i]);

    vector_old_object_set_info* object_set = &obj_db->object_set;
    vector_old_object_set_info* base_object_set = &base_obj_db->object_set;
    vector_old_object_set_info* mdata_object_set = &mdata_obj_db->object_set;

    int32_t count = (int32_t)vector_old_length(*base_object_set);
    vector_old_object_set_info_reserve(object_set, count);
    object_set->end += count;

    for (int32_t i = 0; i < count; i++) {
        object_set_info* b_set_info = &base_object_set->begin[i];
        object_set_info* set_info = &object_set->begin[i];

        string_copy(&b_set_info->name, &set_info->name);
        set_info->name_hash = hash_string_fnv1a64m(&set_info->name, false);
        set_info->id = b_set_info->id;
        string_copy(&b_set_info->object_file_name, &set_info->object_file_name);
        string_copy(&b_set_info->texture_file_name, &set_info->texture_file_name);
        string_copy(&b_set_info->archive_file_name, &set_info->archive_file_name);

        int32_t info_count = (int32_t)vector_old_length(b_set_info->object);
        vector_old_object_info_data_reserve(&set_info->object, info_count);
        set_info->object.end += info_count;

        for (int32_t j = 0; j < info_count; j++) {
            object_info_data* b_info = &b_set_info->object.begin[j];
            object_info_data* info = &set_info->object.begin[j];

            info->id = b_info->id;
            string_copy(&b_info->name, &info->name);
            info->name_hash_fnv1a64m = hash_string_fnv1a64m(&info->name, false);
            info->name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info->name, true);
            info->name_hash_murmurhash = hash_string_murmurhash(&info->name, 0, false);
        }
    }

    int32_t mdata_count = (int32_t)vector_old_length(*mdata_object_set);
    for (int32_t i = 0; i < mdata_count; i++) {
        object_set_info* m_set_info = &mdata_object_set->begin[i];

        uint64_t name_hash = m_set_info->name_hash;
        size_t name_len = m_set_info->name.length;

        object_set_info* set_info = 0;
        for (set_info = object_set->begin; set_info != object_set->end; set_info++)
            if (name_hash == set_info->name_hash)
                break;

        if (set_info == object_set->end)
            set_info = vector_old_object_set_info_reserve_back(object_set);

        string_replace(&m_set_info->name, &set_info->name);
        set_info->name_hash = hash_string_fnv1a64m(&set_info->name, false);
        set_info->id = m_set_info->id;
        string_replace(&m_set_info->object_file_name, &set_info->object_file_name);
        string_replace(&m_set_info->texture_file_name, &set_info->texture_file_name);
        string_replace(&m_set_info->archive_file_name, &set_info->archive_file_name);

        vector_old_object_info_data_clear(&set_info->object, object_info_data_free);
        int32_t info_count = (int32_t)vector_old_length(m_set_info->object);
        vector_old_object_info_data_reserve(&set_info->object, info_count);
        set_info->object.end += info_count;

        for (int32_t j = 0; j < info_count; j++) {
            object_info_data* m_info = &m_set_info->object.begin[j];
            object_info_data* info = &set_info->object.begin[j];

            info->id = m_info->id;
            string_copy(&m_info->name, &info->name);
            info->name_hash_fnv1a64m = hash_string_fnv1a64m(&info->name, false);
            info->name_hash_fnv1a64m_upper = hash_string_fnv1a64m(&info->name, true);
            info->name_hash_murmurhash = hash_string_murmurhash(&info->name, 0, false);
        }
    }

    obj_db->ready = true;
}

void object_database_split_mdata(object_database* obj_db,
    object_database* base_obj_db, object_database* mdata_obj_db) {
    if (!obj_db || !base_obj_db || !mdata_obj_db
        || !obj_db->ready || !base_obj_db->ready)
        return;
}

bool object_database_get_object_set_info(object_database* obj_db,
    char* name, object_set_info** set_info) {
    if (!set_info)
        return false;
    *set_info = 0;

    if (!obj_db || !name)
        return false;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        if (name_hash == i->name_hash) {
            *set_info = i;
            return true;
        }
    return false;
}

bool object_database_get_object_set_info(object_database* obj_db,
    const char* name, object_set_info** set_info) {
    if (!set_info)
        return false;
    *set_info = 0;

    if (!obj_db || !name)
        return false;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        if (name_hash == i->name_hash) {
            *set_info = i;
            return true;
        }
    return false;
}

bool object_database_get_object_set_info_by_set_id(object_database* obj_db,
    uint32_t set_id, object_set_info** set_info) {
    if (!set_info)
        return false;
    *set_info = 0;

    if (!obj_db || set_id == -1)
        return false;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        if (set_id == i->id) {
            *set_info = i;
            return true;
        }
    return false;
}

bool object_database_get_object_info_data(object_database* obj_db,
    char* name, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (!obj_db || !name)
        return false;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (name_hash == j->name_hash_fnv1a64m) {
                *info = j;
                return true;
            }
    return false;
}

bool object_database_get_object_info_data(object_database* obj_db,
    const char* name, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (!obj_db || !name)
        return false;

    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (name_hash == j->name_hash_fnv1a64m) {
                *info = j;
                return true;
            }
    return false;
}

bool object_database_get_object_info_data_by_fnv1a64m_hash(object_database* obj_db,
    uint64_t hash, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (!obj_db)
        return false;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (hash == j->name_hash_fnv1a64m) {
                *info = j;
                return true;
            }
    return false;
}

bool object_database_get_object_info_data_by_fnv1a64m_hash_upper(object_database* obj_db,
    uint64_t hash, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (!obj_db)
        return false;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (hash == j->name_hash_fnv1a64m_upper) {
                *info = j;
                return true;
            }
    return false;
}

bool object_database_get_object_info_data_by_murmurhash(object_database* obj_db,
    uint32_t hash, object_info_data** info) {
    if (!info)
        return false;
    *info = 0;

    if (!obj_db)
        return false;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (hash == j->name_hash_murmurhash) {
                *info = j;
                return true;
            }
    return false;
}
object_info object_database_get_object_info(object_database* obj_db, char* name) {
    if (!obj_db || !name)
        return object_info_null;

    size_t name_len = utf8_length(name);
    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return object_info_null;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (name_hash == j->name_hash_fnv1a64m)
                return { j->id, i->id };

    return object_info_null;
}

object_info object_database_get_object_info(object_database* obj_db, const char* name) {
    if (!obj_db || !name)
        return object_info_null;

    size_t name_len = utf8_length(name);
    uint64_t name_hash = hash_utf8_fnv1a64m(name, false);
    if (!str_utils_compare_length(name, name_len, "NULL", 5))
        return object_info_null;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            if (name_hash == j->name_hash_fnv1a64m)
                return { j->id, i->id };

    return object_info_null;
}

char* object_database_get_object_name(object_database* obj_db, object_info obj_info) {
    if (!obj_db || obj_info.is_null())
        return 0;

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        if (obj_info.set_id == i->id)
            for (object_info_data* j = i->object.begin; j != i->object.end; j++)
                if (obj_info.id == j->id)
                    return string_data(&j->name);

    return 0;
}

void object_database_free(object_database* obj_db) {
    vector_old_object_set_info_free(&obj_db->object_set, object_set_info_free);
}

void object_info_data_free(object_info_data* info) {
    string_free(&info->name);
}

void object_set_info_free(object_set_info* set_info) {
    string_free(&set_info->name);
    vector_old_object_info_data_free(&set_info->object, object_info_data_free);
    string_free(&set_info->object_file_name);
    string_free(&set_info->texture_file_name);
    string_free(&set_info->archive_file_name);
}

static void object_database_classic_read_inner(object_database* obj_db, stream* s) {
    uint32_t object_set_count = io_read_uint32_t(s);
    uint32_t max_object_set_id = io_read_uint32_t(s);
    uint32_t object_sets_offset = io_read_uint32_t(s);
    uint32_t object_count = io_read_uint32_t(s);
    uint32_t objects_offset = io_read_uint32_t(s);

    vector_old_object_set_info_reserve(&obj_db->object_set, object_set_count);
    obj_db->object_set.end += object_set_count;

    io_position_push(s, object_sets_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_set_count; i++) {
        object_set_info* set_info = &obj_db->object_set.begin[i];
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->name);
        set_info->id = io_read_uint32_t(s);
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->object_file_name);
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->texture_file_name);
        io_read_string_null_terminated_offset(s,
            io_read_uint32_t(s), &set_info->archive_file_name);
        io_read(s, 0x10);
    }
    io_position_pop(s);

    io_position_push(s, objects_offset, SEEK_SET);
    for (uint32_t i = 0; i < object_count; i++) {
        object_info_data info;
        info.id = io_read_uint16_t(s);
        uint32_t set_id = io_read_uint16_t(s);
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &info.name);

        if (info.id == 0xFFFF)
            info.id = -1;
        if (set_id == 0xFFFF)
            set_id = -1;

        bool found = false;
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set.begin[i];
            if (set_id != set_info->id)
                continue;

            vector_old_object_info_data_push_back(&set_info->object, &info);
            found = true;
            break;
        }

        if (!found)
            string_free(&info.name);
    }
    io_position_pop(s);

    obj_db->is_x = false;
    obj_db->modern = false;
    obj_db->ready = true;
}

static void object_database_classic_write_inner(object_database* obj_db, stream* s) {
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_write_uint32_t(s, 0x90669066);
    io_align_write(s, 0x20);

    uint32_t object_set_count = (uint32_t)vector_old_length(obj_db->object_set);

    vector_old_ssize_t string_offsets = vector_old_empty(ssize_t);
    vector_old_ssize_t_reserve(&string_offsets, object_set_count * 4ULL);

    uint32_t max_object_set_id = 0;
    uint32_t object_count = 0;
    ssize_t object_sets_offset = 0;
    size_t off_idx = 0;
    ssize_t objects_offset = 0;
    if (string_offsets.begin) {
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            *vector_old_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
            io_write_string_null_terminated(s, &i->name);
            *vector_old_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
            io_write_string_null_terminated(s, &i->object_file_name);
            *vector_old_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
            io_write_string_null_terminated(s, &i->texture_file_name);
            *vector_old_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
            io_write_string_null_terminated(s, &i->archive_file_name);
        }
        io_align_write(s, 0x20);

        object_sets_offset = io_get_position(s);
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
            io_write_uint32_t(s, i->id);
            io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
            io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
            io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
            io_write(s, 0x10);
            if (max_object_set_id < i->id)
                max_object_set_id = i->id;
            object_count += (uint32_t)vector_old_length(i->object);
        }

        vector_old_ssize_t_clear(&string_offsets, 0);
        vector_old_ssize_t_reserve(&string_offsets, object_count);

        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
            for (object_info_data* j = i->object.begin; j != i->object.end; j++) {
                *vector_old_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
                io_write_string_null_terminated(s, &j->name);
            }
        io_align_write(s, 0x20);

        objects_offset = io_get_position(s);
        off_idx = 0;
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            uint16_t set_id = (uint16_t)i->id;
            for (object_info_data* j = i->object.begin; j != i->object.end; j++) {
                io_write_uint16_t(s, (uint16_t)j->id);
                io_write_uint16_t(s, set_id);
                io_write_uint32_t(s, (uint32_t)string_offsets.begin[off_idx++]);
            }
        }
        io_align_write(s, 0x20);
    }

    vector_old_ssize_t_free(&string_offsets, 0);

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, object_set_count);
    io_write_uint32_t(s, max_object_set_id);
    io_write_uint32_t(s, (uint32_t)object_sets_offset);
    io_write_uint32_t(s, object_count);
    io_write_uint32_t(s, (uint32_t)objects_offset);
    io_write(s, 0x08);
    io_position_pop(s);
}

static void object_database_modern_read_inner(object_database* obj_db, stream* s, uint32_t header_length) {
    bool is_x = true;

    io_set_position(s, 0x0C, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;
    io_set_position(s, 0x14, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;

    io_set_position(s, 0x00, SEEK_SET);

    io_read_uint32_t_stream_reverse_endianness(s);
    uint32_t object_set_count = io_read_uint32_t_stream_reverse_endianness(s);
    uint32_t max_object_set_id = io_read_uint32_t_stream_reverse_endianness(s);
    ssize_t object_sets_offset = io_read_offset(s, header_length, is_x);
    uint32_t object_count = io_read_uint32_t_stream_reverse_endianness(s);
    ssize_t objects_offset = io_read_offset(s, header_length, is_x);

    vector_old_object_set_info_reserve(&obj_db->object_set, object_set_count);
    obj_db->object_set.end += object_set_count;

    io_position_push(s, object_sets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set.begin[i];
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->name);
            set_info->id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->object_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->texture_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_f2(s, header_length), &set_info->archive_file_name);
            io_read(s, 0x10);
        }
    else
        for (uint32_t i = 0; i < object_set_count; i++) {
            object_set_info* set_info = &obj_db->object_set.begin[i];
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->name);
            set_info->id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->object_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->texture_file_name);
            io_read_string_null_terminated_offset(s,
                io_read_offset_x(s), &set_info->archive_file_name);
            io_read(s, 0x10);
        }
    io_position_pop(s);

    io_position_push(s, objects_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < object_count; i++) {
            object_info_data info;
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            uint32_t set_id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &info.name);

            bool found = false;
            for (uint32_t i = 0; i < object_set_count; i++) {
                object_set_info* set_info = &obj_db->object_set.begin[i];
                if (set_id != set_info->id)
                    continue;

                vector_old_object_info_data_push_back(&set_info->object, &info);
                found = true;
                break;
            }

            if (!found)
                string_free(&info.name);
        }
    else
        for (uint32_t i = 0; i < object_count; i++) {
            object_info_data info;
            info.id = io_read_uint32_t_stream_reverse_endianness(s);
            uint32_t set_id = io_read_uint32_t_stream_reverse_endianness(s);
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &info.name);

            bool found = false;
            for (uint32_t i = 0; i < object_set_count; i++) {
                object_set_info* set_info = &obj_db->object_set.begin[i];
                if (set_id != set_info->id)
                    continue;

                vector_old_object_info_data_push_back(&set_info->object, &info);
                found = true;
                break;
            }

            if (!found)
                string_free(&info.name);
        }
    io_position_pop(s);

    obj_db->is_x = is_x;
    obj_db->modern = true;
    obj_db->ready = true;
}

static void object_database_modern_write_inner(object_database* obj_db, stream* s) {
    stream s_mosi;
    io_mopen(&s_mosi, 0, 0);
    uint32_t off;
    vector_old_enrs_entry e = vector_old_empty(enrs_entry);
    enrs_entry ee;
    vector_old_size_t pof = vector_old_empty(size_t);

    bool is_x = obj_db->is_x;

    uint32_t object_set_count = (uint32_t)vector_old_length(obj_db->object_set);

    uint32_t object_count = 0;
    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++)
        object_count += (uint32_t)vector_old_length(i->object);

    if (!is_x) {
        ee = { 0, 1, 1, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 6, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = 32;

        ee = { off, 1, 36, object_set_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 5, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_set_count * 36ULL);
        off = align_val(off, 0x10);

        ee = { off, 1, 12, object_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 3, ENRS_DWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_count * 12ULL);
        off = align_val(off, 0x10);
    }
    else {
        ee = { 0, 4, 48, 1, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 3, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 1, ENRS_QWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 1, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = 48;

        ee = { off, 3, 56, object_set_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_QWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 4, 3, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_set_count * 56ULL);
        off = align_val(off, 0x10);

        ee = { off, 2, 16, object_count, vector_old_empty(enrs_sub_entry) };
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 2, ENRS_DWORD);
        vector_old_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_QWORD);
        vector_old_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(object_count * 16ULL);
        off = align_val(off, 0x10);
    }

    io_write_int32_t(&s_mosi, 0);
    io_write_int32_t(&s_mosi, 0);
    io_write_offset(&s_mosi, 0, 0, is_x);
    io_write_int32_t(&s_mosi, 0);
    io_write_offset(&s_mosi, 0, 0, is_x);
    io_write(&s_mosi, 0x08);
    io_align_write(&s_mosi, 0x10);

    ssize_t object_sets_offset = io_get_position(&s_mosi);
    io_write(&s_mosi, object_set_count * (is_x ? 0x38ULL : 0x24ULL));
    io_align_write(&s_mosi, 0x10);

    ssize_t objects_offset = io_get_position(&s_mosi);
    io_write(&s_mosi, object_count * (is_x ? 0x10ULL : 0x0CULL));
    io_align_write(&s_mosi, 0x10);

    vector_old_string strings = vector_old_empty(string);
    vector_old_ssize_t string_offsets = vector_old_empty(ssize_t);

    vector_old_string_reserve(&strings, (ssize_t)object_set_count + object_count);

    for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
        object_database_strings_push_back_check(&strings, string_data(&i->name));
        object_database_strings_push_back_check(&strings, string_data(&i->object_file_name));
        object_database_strings_push_back_check(&strings, string_data(&i->texture_file_name));
        object_database_strings_push_back_check(&strings, string_data(&i->archive_file_name));
        for (object_info_data* j = i->object.begin; j != i->object.end; j++)
            object_database_strings_push_back_check(&strings, string_data(&j->name));
    }

    quicksort_string(strings.begin, vector_old_length(strings));
    vector_old_ssize_t_reserve(&string_offsets, vector_old_length(strings));
    for (string* i = strings.begin; i != strings.end; i++) {
        ssize_t off = io_get_position(&s_mosi);
        io_write_string_null_terminated(&s_mosi, i);
        vector_old_ssize_t_push_back(&string_offsets, &off);
    }
    io_align_write(&s_mosi, 0x10);

    io_position_push(&s_mosi, 0x00, SEEK_SET);
    io_write_uint32_t(&s_mosi, 0);
    io_write_uint32_t(&s_mosi, object_set_count);
    io_write_uint32_t(&s_mosi, 0);
    io_write_offset_pof_add(&s_mosi, object_sets_offset, 0x20, is_x, &pof);
    io_write_uint32_t(&s_mosi, object_count);
    io_write_offset_pof_add(&s_mosi, objects_offset, 0x20, is_x, &pof);
    io_write(&s_mosi, 0x08);
    io_align_write(&s_mosi, 0x10);

    if (!is_x)
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->name)), 0x20, &pof);
            io_write_uint32_t(&s_mosi, i->id);
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->object_file_name)), 0x20, &pof);
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->texture_file_name)), 0x20, &pof);
            io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->archive_file_name)), 0x20, &pof);
            io_write(&s_mosi, 0x10);
        }
    else
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->name)), &pof);
            io_write_uint32_t(&s_mosi, i->id);
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->object_file_name)), &pof);
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->texture_file_name)), &pof);
            io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->archive_file_name)), &pof);
            io_write(&s_mosi, 0x10);
        }
    io_align_write(&s_mosi, 0x10);

    if (!is_x)
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            uint32_t set_id = i->id;
            for (object_info_data* j = i->object.begin; j != i->object.end; j++) {
                io_write_uint32_t(&s_mosi, j->id);
                io_write_uint32_t(&s_mosi, set_id);
                io_write_offset_f2_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(&j->name)), 0x20, &pof);
            }
        }
    else
        for (object_set_info* i = obj_db->object_set.begin; i != obj_db->object_set.end; i++) {
            uint32_t set_id = i->id;
            for (object_info_data* j = i->object.begin; j != i->object.end; j++) {
                io_write_uint32_t(&s_mosi, j->id);
                io_write_uint32_t(&s_mosi, set_id);
                io_write_offset_x_pof_add(&s_mosi, object_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(&j->name)), &pof);
            }
        }
    io_position_pop(&s_mosi);

    vector_old_string_free(&strings, string_free);
    vector_old_ssize_t_free(&string_offsets, 0);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    io_align_write(&s_mosi, 0x10);
    io_mcopy(&s_mosi, &st.data, &st.length);
    io_free(&s_mosi);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSI');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    f2_struct_swrite(&st, s, true, is_x);
    f2_struct_free(&st);
}

inline static ssize_t object_database_strings_get_string_offset(vector_old_string* vec,
    vector_old_ssize_t* vec_off, char* str) {
    ssize_t len = utf8_length(str);
    for (string* i = vec->begin; i != vec->end; i++)
        if (!memcmp(str, string_data(i), min(len, i->length) + 1))
            return vec_off->begin[i - vec->begin];
    return 0;
}

inline static void object_database_strings_push_back_check(vector_old_string* vec, char* str) {
    ssize_t len = utf8_length(str);
    for (string* i = vec->begin; i != vec->end; i++)
        if (!memcmp(str, string_data(i), min(len, i->length) + 1))
            return;

    string* s = vector_old_string_reserve_back(vec);
    string_init_length(s, str, len);
}
