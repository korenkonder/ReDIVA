/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../farc.h"
#include "../hash.h"
#include "../str_utils.h"

vector_func(motion_info)
vector_func(motion_set_info)

static void motion_database_read_inner(motion_database* mot_db, stream* s);
static void motion_database_write_inner(motion_database* mot_db, stream* s);

void motion_database_init(motion_database* mot_db) {
    memset(mot_db, 0, sizeof(motion_database));
}

void motion_database_read(motion_database* mot_db, char* path) {
    if (!mot_db || !path)
        return;

    char* path_farc = str_utils_add(path, ".farc");
    if (path_check_file_exists(path_farc)) {
        farc f;
        farc_init(&f);
        farc_read(&f, path_farc, true, false);

        farc_file* ff = farc_read_file(&f, "mot_db.bin");
        if (ff) {
            stream s;
            io_mopen(&s, ff->data, ff->size);
            motion_database_read_inner(mot_db, &s);
            io_free(&s);
        }
        farc_free(&f);
    }
    free(path_farc);
}

void motion_database_wread(motion_database* mot_db, wchar_t* path) {
    if (!mot_db || !path)
        return;

    wchar_t* path_farc = str_utils_wadd(path, L".farc");
    if (path_wcheck_file_exists(path_farc)) {
        farc f;
        farc_init(&f);
        farc_wread(&f, path_farc, true, false);

        farc_file* ff = farc_read_file(&f, "mot_db.bin");
        if (ff) {
            stream s;
            io_mopen(&s, ff->data, ff->size);
            motion_database_read_inner(mot_db, &s);
            io_free(&s);
        }
        farc_free(&f);
    }
    free(path_farc);
}

void motion_database_mread(motion_database* mot_db, void* data, size_t length) {
    if (!mot_db || !data || !length)
        return;

    stream s;
    io_mopen(&s, data, length);
    motion_database_read_inner(mot_db, &s);
    io_free(&s);
}

void motion_database_write(motion_database* mot_db, char* path) {
    if (!mot_db || !path || !mot_db->ready)
        return;

    farc f;
    farc_init(&f);

    farc_file* ff = vector_farc_file_reserve_back(&f.files);
    string_init_length(&ff->name, "mot_db.bin", 10);
    stream s;
    io_mopen(&s, 0, 0);
    motion_database_write_inner(mot_db, &s);
    io_mcopy(&s, &ff->data, &ff->size);
    io_free(&s);

    farc_write(&f, path, FARC_COMPRESS_FArc, false);
    farc_free(&f);
}

void motion_database_wwrite(motion_database* mot_db, wchar_t* path) {
    if (!mot_db || !path || !mot_db->ready)
        return;

    farc f;
    farc_init(&f);

    farc_file* ff = vector_farc_file_reserve_back(&f.files);
    string_init_length(&ff->name, "mot_db.bin", 10);
    stream s;
    io_mopen(&s, 0, 0);
    motion_database_write_inner(mot_db, &s);
    io_mcopy(&s, &ff->data, &ff->size);
    io_free(&s);

    farc_wwrite(&f, path, FARC_COMPRESS_FArC, false);
    farc_free(&f);
}

void motion_database_mwrite(motion_database* mot_db, void** data, size_t* length) {
    if (!mot_db || !data || !mot_db->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    motion_database_write_inner(mot_db, &s);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool motion_database_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    motion_database* mot_db = data;
    motion_database_read(mot_db, string_data(&s));

    string_free(&s);
    return mot_db->ready;
}

void motion_database_merge_mdata(motion_database* mot_db,
    motion_database* base_mot_db, motion_database* mdata_mot_db) {
    if (!mot_db || !base_mot_db || !mdata_mot_db
        || !base_mot_db->ready || !mdata_mot_db->ready)
        return;

    vector_string* bone_name = &mot_db->bone_name;
    vector_string* base_bone_name = &base_mot_db->bone_name;

    int32_t bone_count = (int32_t)(base_bone_name->end - base_bone_name->begin);
    vector_string_reserve(bone_name, bone_count);
    bone_name->end += bone_count;

    for (int32_t i = 0; i < bone_count; i++)
        string_copy(&base_bone_name->begin[i], &bone_name->begin[i]);

    vector_motion_set_info* motion_set = &mot_db->motion_set;
    vector_motion_set_info* base_motion_set = &base_mot_db->motion_set;
    vector_motion_set_info* mdata_motion_set = &mdata_mot_db->motion_set;

    int32_t count = (int32_t)(base_motion_set->end - base_motion_set->begin);
    vector_motion_set_info_reserve(motion_set, count);
    motion_set->end += count;

    for (int32_t i = 0; i < count; i++) {
        motion_set_info* b_set_info = &base_motion_set->begin[i];
        motion_set_info* set_info = &motion_set->begin[i];

        set_info->id = b_set_info->id;
        string_copy(&b_set_info->name, &set_info->name);
        set_info->name_hash = hash_fnv1a64m(string_data(&set_info->name), set_info->name.length);

        int32_t info_count = (int32_t)(b_set_info->motion.end - b_set_info->motion.begin);
        vector_motion_info_reserve(&set_info->motion, info_count);
        set_info->motion.end += info_count;

        for (int32_t j = 0; j < info_count; j++) {
            motion_info* b_info = &b_set_info->motion.begin[j];
            motion_info* info = &set_info->motion.begin[j];

            info->id = b_info->id;
            string_copy(&b_info->name, &info->name);
            info->name_hash = hash_fnv1a64m(string_data(&info->name), info->name.length);
        }
    }

    int32_t mdata_count = (int32_t)(mdata_motion_set->end - mdata_motion_set->begin);
    for (int32_t i = 0; i < mdata_count; i++) {
        motion_set_info* m_set_info = &mdata_motion_set->begin[i];

        char* name_str = string_data(&m_set_info->name);
        size_t name_len = m_set_info->name.length;

        motion_set_info* set_info = 0;
        for (set_info = motion_set->begin; set_info != motion_set->end; set_info++)
            if (!memcmp(name_str, string_data(&set_info->name), min(name_len, set_info->name.length) + 1))
                break;

        if (set_info == motion_set->end)
            set_info = vector_motion_set_info_reserve_back(motion_set);

        set_info->id = m_set_info->id;
        string_replace(&m_set_info->name, &set_info->name);
        set_info->name_hash = hash_fnv1a64m(string_data(&set_info->name), set_info->name.length);

        vector_motion_info_clear(&set_info->motion, motion_info_free);
        int32_t info_count = (int32_t)(m_set_info->motion.end - m_set_info->motion.begin);
        vector_motion_info_reserve(&set_info->motion, info_count);
        set_info->motion.end += info_count;

        for (int32_t j = 0; j < info_count; j++) {
            motion_info* m_info = &m_set_info->motion.begin[j];
            motion_info* info = &set_info->motion.begin[j];

            info->id = m_info->id;
            string_copy(&m_info->name, &info->name);
            info->name_hash = hash_fnv1a64m(string_data(&info->name), info->name.length);
        }
    }

    mot_db->ready = true;
}

void motion_database_split_mdata(motion_database* mot_db,
    motion_database* base_mot_db, motion_database* mdata_mot_db) {
    if (!mot_db || !base_mot_db || !mdata_mot_db
        || !mot_db->ready || !base_mot_db->ready)
        return;

}

motion_set_info* motion_database_get_motion_set_by_id(motion_database* mot_db, uint32_t id) {
    if (!mot_db || !id == -1)
        return 0;

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        if (id == i->id)
            return i;
    return 0;
}

motion_set_info* motion_database_get_motion_set_by_name(motion_database* mot_db, char* name) {
    if (!mot_db || !name)
        return 0;

    uint64_t name_hash = hash_fnv1a64m(name, utf8_length(name));

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        if (name_hash == i->name_hash)
            return i;
    return 0;
}

uint32_t motion_database_get_motion_set_id(motion_database* mot_db, char* name) {
    if (!mot_db || !name)
        return -1;

    uint64_t name_hash = hash_fnv1a64m(name, utf8_length(name));

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        if (name_hash == i->name_hash)
            return i->id;
    return -1;
}

char* motion_database_get_motion_set_name(motion_database* mot_db, uint32_t id) {
    if (!mot_db || id == -1)
        return 0;

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        if (id == i->id)
            return string_data(&i->name);
    return 0;
}

motion_info* motion_database_get_motion_by_id(motion_database* mot_db, uint32_t id) {
    if (!mot_db || id == -1)
        return 0;

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            if (id == j->id)
                return j;
    return 0;
}

motion_info* motion_database_get_motion_by_name(motion_database* mot_db, char* name) {
    if (!mot_db || !name)
        return 0;

    uint64_t name_hash = hash_fnv1a64m(name, utf8_length(name));

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            if (name_hash == j->name_hash)
                return j;
    return 0;
}

uint32_t motion_database_get_motion_id(motion_database* mot_db, char* name) {
    if (!mot_db || !name)
        return -1;

    uint64_t name_hash = hash_fnv1a64m(name, utf8_length(name));

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            if (name_hash == j->name_hash)
                return j->id;
    return -1;
}

char* motion_database_get_motion_name(motion_database* mot_db, uint32_t id) {
    if (!mot_db || id == -1)
        return 0;

    for (motion_set_info* i = mot_db->motion_set.begin; i != mot_db->motion_set.end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            if (id == j->id)
                return string_data(&j->name);
    return 0;
}

void motion_database_free(motion_database* mot_db) {
    vector_string_free(&mot_db->bone_name, string_free);
    vector_motion_set_info_free(&mot_db->motion_set, motion_set_info_free);
}

void motion_info_free(motion_info* info) {
    string_free(&info->name);
}

void motion_set_info_free(motion_set_info* set_info) {
    string_free(&set_info->name);
    vector_motion_info_free(&set_info->motion, motion_info_free);
}

static void motion_database_read_inner(motion_database* mot_db, stream* s) {
   if (io_read_uint32_t(s) != 0x01)
        return;

   uint32_t motion_sets_offset = io_read_uint32_t(s);
   uint32_t motion_set_ids_offset = io_read_uint32_t(s);
   uint32_t motion_set_count = io_read_uint32_t(s);
   uint32_t bone_name_offsets_offset = io_read_uint32_t(s);
   uint32_t bone_name_count = io_read_uint32_t(s);

   vector_motion_set_info_reserve(&mot_db->motion_set, motion_set_count);
   mot_db->motion_set.end += motion_set_count;

   io_position_push(s, motion_sets_offset, SEEK_SET);
   for (uint32_t i = 0; i < motion_set_count; i++) {
       motion_set_info* set_info = &mot_db->motion_set.begin[i];

       uint32_t name_offset = io_read_uint32_t(s);
       uint32_t motion_name_offsets_offset = io_read_uint32_t(s);
       uint32_t motion_count = io_read_uint32_t(s);
       uint32_t motion_ids_offset = io_read_uint32_t(s);

       io_read_string_null_terminated_offset(s, name_offset, &set_info->name);

       vector_motion_info_reserve(&set_info->motion, motion_count);
       set_info->motion.end += motion_count;

       io_position_push(s, motion_name_offsets_offset, SEEK_SET);
       for (uint32_t j = 0; j < motion_count; j++) {
           motion_info* motion = &set_info->motion.begin[j];
           io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &motion->name);
       }
       io_position_pop(s);

       io_position_push(s, motion_ids_offset, SEEK_SET);
       for (uint32_t j = 0; j < motion_count; j++) {
           motion_info* motion = &set_info->motion.begin[j];
           motion->id = io_read_uint32_t(s);
       }
       io_position_pop(s);
   }
   io_position_pop(s);

   io_position_push(s, motion_set_ids_offset, SEEK_SET);
   for (uint32_t i = 0; i < motion_set_count; i++) {
       motion_set_info* set_info = &mot_db->motion_set.begin[i];
       set_info->id = io_read_uint32_t(s);
   }
   io_position_pop(s);

   vector_string_reserve(&mot_db->bone_name, bone_name_count);
   mot_db->bone_name.end += bone_name_count;

   io_position_push(s, bone_name_offsets_offset, SEEK_SET);
   for (uint32_t i = 0; i < bone_name_count; i++) {
       string* bone_name = &mot_db->bone_name.begin[i];
       io_read_string_null_terminated_offset(s, io_read_uint32_t(s), bone_name);
   }
   io_position_pop(s);

   mot_db->ready = true;
}

static void motion_database_write_inner(motion_database* mot_db, stream* s) {
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_align_write(s, 0x40);

    vector_string* bone_name = &mot_db->bone_name;
    vector_motion_set_info* motion_set = &mot_db->motion_set;

    ssize_t bone_name_count = bone_name->end - bone_name->begin;
    ssize_t motion_set_count = motion_set->end - motion_set->begin;

    ssize_t motion_sets_offset = io_get_position(s);
    io_write(s, 0, 0x10 * motion_set_count + 0x10);
    io_align_write(s, 0x20);

    size_t size = 0;
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        size += i->name.length + 1;

    ssize_t motion_set_name_offset = io_get_position(s);
    io_write(s, 0, size);

    size = 0;
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            size += j->name.length + 1;

    ssize_t motion_name_offset = io_get_position(s);
    io_write(s, 0, size);
    io_align_write(s, 0x20);

    ssize_t motion_names_offset = io_get_position(s);
    size = 0;
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        size += 0x04 * (i->motion.end - i->motion.begin);
    io_write(s, 0, size);
    io_align_write(s, 0x20);

    ssize_t motion_ids_offset = io_get_position(s);
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            io_write_uint32_t(s, j->id);
    io_align_write(s, 0x20);

    ssize_t motion_set_ids_offset = io_get_position(s);
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        io_write_uint32_t(s, i->id);

    ssize_t bone_name_offset = io_get_position(s);
    size = 0;
    for (string* i = bone_name->begin; i != bone_name->end; i++)
        size += i->length + 1;
    io_write(s, 0, size);
    io_align_write(s, 0x20);

    io_position_push(s, motion_set_name_offset, SEEK_SET);
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        io_write_string_null_terminated(s, &i->name);
    io_position_pop(s);

    io_position_push(s, motion_name_offset, SEEK_SET);
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++)
            io_write_string_null_terminated(s, &j->name);
    io_position_pop(s);

    io_position_push(s, bone_name_offset, SEEK_SET);
    for (string* i = bone_name->begin; i != bone_name->end; i++)
        io_write_string_null_terminated(s, i);
    io_position_pop(s);

    ssize_t bone_name_offsets_offset = io_get_position(s);
    io_write(s, 0, 0x04 * bone_name_count);
    io_align_write(s, 0x20);

    io_position_push(s, motion_names_offset, SEEK_SET);
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++)
        for (motion_info* j = i->motion.begin; j != i->motion.end; j++) {
            io_write_uint32_t(s, (uint32_t)motion_name_offset);
            motion_name_offset += j->name.length + 1;
        }
    io_position_pop(s);

    io_position_push(s, motion_sets_offset, SEEK_SET);
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++) {
        io_write_uint32_t(s, (uint32_t)motion_set_name_offset);
        io_write_uint32_t(s, (uint32_t)motion_names_offset);
        io_write_uint32_t(s, (uint32_t)(i->motion.end - i->motion.begin));
        io_write_uint32_t(s, (uint32_t)motion_ids_offset);
        motion_set_name_offset += i->name.length + 1;
        motion_names_offset += 0x04 * (i->motion.end - i->motion.begin);
        motion_ids_offset += 0x04 * (i->motion.end - i->motion.begin);
    }
    io_write(s, 0, 0x10);
    io_position_pop(s);

    io_position_push(s, bone_name_offsets_offset, SEEK_SET);
    for (string* i = bone_name->begin; i != bone_name->end; i++) {
        io_write_uint32_t(s, (uint32_t)bone_name_offset);
        bone_name_offset += i->length + 1;
    }
    io_position_pop(s);

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, 0x01);
    io_write_uint32_t(s, (uint32_t)motion_sets_offset);
    io_write_uint32_t(s, (uint32_t)motion_set_ids_offset);
    io_write_uint32_t(s, (uint32_t)motion_set_count);
    io_write_uint32_t(s, (uint32_t)bone_name_offsets_offset);
    io_write_uint32_t(s, (uint32_t)bone_name_count);
    io_position_pop(s);
}
