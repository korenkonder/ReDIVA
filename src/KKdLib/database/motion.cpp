/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion.hpp"
#include "../io/path.hpp"
#include "../io/stream.hpp"
#include "../farc.hpp"
#include "../hash.hpp"
#include "../str_utils.hpp"

static void motion_database_read_inner(motion_database* mot_db, stream& s);
static void motion_database_write_inner(motion_database* mot_db, stream& s);

motion_database::motion_database() : ready() {

}

motion_database::~motion_database() {

}

void motion_database::read(const char* path) {
    if (!path)
        return;

    char* path_farc = str_utils_add(path, ".farc");
    if (path_check_file_exists(path_farc)) {
        farc f;
        f.read(path_farc, true, false);

        farc_file* ff = f.read_file("mot_db.bin");
        if (ff) {
            stream s;
            s.open(ff->data, ff->size);
            motion_database_read_inner(this, s);
        }
    }
    free(path_farc);
}

void motion_database::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_farc = str_utils_add(path, L".farc");
    if (path_check_file_exists(path_farc)) {
        farc f;
        f.read(path_farc, true, false);

        farc_file* ff = f.read_file("mot_db.bin");
        if (ff) {
            stream s;
            s.open(ff->data, ff->size);
            motion_database_read_inner(this, s);
        }
    }
    free(path_farc);
}

void motion_database::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    stream s;
    s.open(data, size);
    motion_database_read_inner(this, s);
}

void motion_database::write(const char* path) {
    if (!path || !ready)
        return;

    farc f;

    f.add_file("mot_db.bin");
    farc_file* ff = &f.files.back();

    stream s;
    s.open();
    motion_database_write_inner(this, s);
    s.copy(&ff->data, &ff->size);

    f.write(path, FARC_COMPRESS_FArC, false);
}

void motion_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    farc f;

    f.add_file("mot_db.bin");
    farc_file* ff = &f.files.back();

    stream s;
    s.open();
    motion_database_write_inner(this, s);
    s.copy(&ff->data, &ff->size);

    f.write(path, FARC_COMPRESS_FArC, false);
}

void motion_database::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
    s.open();
    motion_database_write_inner(this, s);
    s.copy(data, size);
}

void motion_database::merge_mdata(motion_database* base_mot_db,
    motion_database* mdata_mot_db) {
    if (!base_mot_db || !mdata_mot_db
        || !base_mot_db->ready || !mdata_mot_db->ready)
        return;

    bone_name = base_mot_db->bone_name;

    motion_set.reserve(base_mot_db->motion_set.size());

    for (motion_set_info& i : base_mot_db->motion_set) {
        motion_set_info set_info;
        set_info.id = i.id;
        set_info.name = i.name;
        set_info.name_hash = hash_string_murmurhash(&i.name);

        set_info.motion.reserve(i.motion.size());

        for (motion_info& j : i.motion) {
            motion_info info;
            info.id = j.id;
            info.name = j.name;
            info.name_hash = hash_string_murmurhash(&j.name);
            set_info.motion.push_back(info);
        }
        motion_set.push_back(set_info);
    }

    for (motion_set_info& i : mdata_mot_db->motion_set) {
        const char* name_str = i.name.c_str();
        size_t name_len = i.name.size();

        motion_set_info* set_info_ptr = 0;
        for (motion_set_info& j : motion_set)
            if (!memcmp(name_str, j.name.c_str(), min(name_len, j.name.size()) + 1)) {
                set_info_ptr = &j;
                break;
            }

        motion_set_info set_info;
        set_info.id = i.id;
        set_info.name = i.name;
        set_info.name_hash = hash_string_murmurhash(&i.name);

        set_info.motion.reserve(i.motion.size());

        for (motion_info& j : i.motion) {
            motion_info info;
            info.id = j.id;
            info.name = j.name;
            info.name_hash = hash_string_murmurhash(&j.name);
            set_info.motion.push_back(info);
        }

        if (set_info_ptr)
            *set_info_ptr = set_info;
        else
            motion_set.push_back(set_info);
    }

    ready = true;
}

void motion_database::split_mdata(motion_database* base_mot_db,
    motion_database* mdata_mot_db) {
    if (!base_mot_db || !mdata_mot_db
        || !ready || !base_mot_db->ready)
        return;

}

motion_set_info* motion_database::get_motion_set_by_id(uint32_t id) {
    if (id == -1)
        return 0;

    for (motion_set_info& i : motion_set)
        if (id == i.id)
            return &i;
    return 0;
}

motion_set_info* motion_database::get_motion_set_by_name(const char* name) {
    if (!name)
        return 0;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (motion_set_info& i : motion_set)
        if (name_hash == i.name_hash)
            return &i;
    return 0;
}

motion_set_info* motion_database::get_motion_set_by_motion_id(uint32_t id) {
    if (id == -1)
        return 0;

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (id == j.id)
                return &i;
    return 0;
}

motion_set_info* motion_database::get_motion_set_by_motion_name(const char* name) {
    if (!name)
        return 0;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (name_hash == j.name_hash)
                return &i;
    return 0;
}

uint32_t motion_database::get_motion_set_id_by_motion_id(uint32_t id) {
    if (id == -1)
        return -1;

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (id == j.id)
                return i.id;
    return -1;
}

uint32_t motion_database::get_motion_set_id_by_motion_name(const char* name) {
    if (!name)
        return -1;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (name_hash == j.name_hash)
                return i.id;
    return -1;
}

uint32_t motion_database::get_motion_set_id(const char* name) {
    if (!name)
        return -1;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (motion_set_info& i : motion_set)
        if (name_hash == i.name_hash)
            return i.id;
    return -1;
}

const char* motion_database::get_motion_set_name(uint32_t id) {
    if (id == -1)
        return 0;

    for (motion_set_info& i : motion_set)
        if (id == i.id)
            return i.name.c_str();
    return 0;
}

motion_info* motion_database::get_motion_by_id(uint32_t id) {
    if (id == -1)
        return 0;

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (id == j.id)
                return &j;
    return 0;
}

motion_info* motion_database::get_motion_by_name(const char* name) {
    if (!name)
        return 0;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (name_hash == j.name_hash)
                return &j;
    return 0;
}

uint32_t motion_database::get_motion_id(const char* name) {
    if (!name)
        return -1;

    uint32_t name_hash = hash_utf8_murmurhash(name);

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (name_hash == j.name_hash)
                return j.id;
    return -1;
}

const char* motion_database::get_motion_name(uint32_t id) {
    if (id == -1)
        return 0;

    for (motion_set_info& i : motion_set)
        for (motion_info& j : i.motion)
            if (id == j.id)
                return j.name.c_str();
    return 0;
}

bool motion_database::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    motion_database* mot_db = (motion_database*)data;
    mot_db->read(s.c_str());

    return mot_db->ready;
}

motion_info::motion_info() : name_hash(hash_murmurhash_empty), id((uint32_t)-1) {

}

motion_info::~motion_info() {

}

motion_set_info::motion_set_info() : name_hash(hash_murmurhash_empty), id((uint32_t)-1) {

}

motion_set_info::~motion_set_info() {

}

static void motion_database_read_inner(motion_database* mot_db, stream& s) {
   if (s.read_uint32_t() != 0x01)
        return;

   uint32_t motion_sets_offset = s.read_uint32_t();
   uint32_t motion_set_ids_offset = s.read_uint32_t();
   uint32_t motion_set_count = s.read_uint32_t();
   uint32_t bone_name_offsets_offset = s.read_uint32_t();
   uint32_t bone_name_count = s.read_uint32_t();

   mot_db->motion_set.resize(motion_set_count);

   s.position_push(motion_sets_offset, SEEK_SET);
   for (uint32_t i = 0; i < motion_set_count; i++) {
       motion_set_info& set_info = mot_db->motion_set[i];

       uint32_t name_offset = s.read_uint32_t();
       uint32_t motion_name_offsets_offset = s.read_uint32_t();
       uint32_t motion_count = s.read_uint32_t();
       uint32_t motion_ids_offset = s.read_uint32_t();

       set_info.name = s.read_string_null_terminated_offset(name_offset);
       set_info.name_hash = hash_string_murmurhash(&set_info.name);

       set_info.motion.resize(motion_count);

       s.position_push(motion_name_offsets_offset, SEEK_SET);
       for (uint32_t j = 0; j < motion_count; j++) {
           set_info.motion[j].name = s.read_string_null_terminated_offset(s.read_uint32_t());
           set_info.motion[j].name_hash = hash_string_murmurhash(&set_info.motion[j].name);
       }
       s.position_pop();

       s.position_push(motion_ids_offset, SEEK_SET);
       for (uint32_t j = 0; j < motion_count; j++)
           set_info.motion[j].id = s.read_uint32_t();
       s.position_pop();
   }
   s.position_pop();

   s.position_push(motion_set_ids_offset, SEEK_SET);
   for (uint32_t i = 0; i < motion_set_count; i++)
       mot_db->motion_set[i].id = s.read_uint32_t();
   s.position_pop();

   mot_db->bone_name.resize(bone_name_count);

   s.position_push(bone_name_offsets_offset, SEEK_SET);
   for (uint32_t i = 0; i < bone_name_count; i++)
       mot_db->bone_name[i] = s.read_string_null_terminated_offset(s.read_uint32_t());
   s.position_pop();

   mot_db->ready = true;
}

static void motion_database_write_inner(motion_database* mot_db, stream& s) {
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.align_write(0x40);

    size_t bone_name_count = mot_db->bone_name.size();
    size_t motion_set_count = mot_db->motion_set.size();

    int64_t motion_sets_offset = s.get_position();
    s.write(0x10 * motion_set_count + 0x10);
    s.align_write(0x20);

    size_t size = 0;
    for (motion_set_info& i : mot_db->motion_set)
        size += i.name.size() + 1;

    int64_t motion_set_name_offset = s.get_position();
    s.write(size);

    size = 0;
    for (motion_set_info& i : mot_db->motion_set)
        for (motion_info& j : i.motion)
            size += j.name.size() + 1;

    int64_t motion_name_offset = s.get_position();
    s.write(size);
    s.align_write(0x20);

    int64_t motion_names_offset = s.get_position();
    size = 0;
    for (motion_set_info& i : mot_db->motion_set)
        size += 0x04 * i.motion.size();
    s.write(size);
    s.align_write(0x20);

    int64_t motion_ids_offset = s.get_position();
    for (motion_set_info& i : mot_db->motion_set)
        for (motion_info& j : i.motion)
            s.write_uint32_t(j.id);
    s.align_write(0x20);

    int64_t motion_set_ids_offset = s.get_position();
    for (motion_set_info& i : mot_db->motion_set)
        s.write_uint32_t(i.id);

    int64_t bone_name_offset = s.get_position();
    size = 0;
    for (std::string& i : mot_db->bone_name)
        size += i.size() + 1;
    s.write(size);
    s.align_write(0x20);

    s.position_push(motion_set_name_offset, SEEK_SET);
    for (motion_set_info& i : mot_db->motion_set)
        s.write_string_null_terminated(i.name);
    s.position_pop();

    s.position_push(motion_name_offset, SEEK_SET);
    for (motion_set_info& i : mot_db->motion_set)
        for (motion_info& j : i.motion)
            s.write_string_null_terminated(j.name);
    s.position_pop();

    s.position_push(bone_name_offset, SEEK_SET);
    for (std::string& i : mot_db->bone_name)
        s.write_string_null_terminated(i);
    s.position_pop();

    int64_t bone_name_offsets_offset = s.get_position();
    s.write(0x04 * bone_name_count);
    s.align_write(0x20);

    s.position_push(motion_names_offset, SEEK_SET);
    for (motion_set_info& i : mot_db->motion_set)
        for (motion_info& j : i.motion) {
            s.write_uint32_t((uint32_t)motion_name_offset);
            motion_name_offset += j.name.size() + 1;
        }
    s.position_pop();

    s.position_push(motion_sets_offset, SEEK_SET);
    for (motion_set_info& i : mot_db->motion_set) {
        s.write_uint32_t((uint32_t)motion_set_name_offset);
        s.write_uint32_t((uint32_t)motion_names_offset);
        s.write_uint32_t((uint32_t)i.motion.size());
        s.write_uint32_t((uint32_t)motion_ids_offset);
        motion_set_name_offset += i.name.size() + 1;
        motion_names_offset += 0x04 * i.motion.size();
        motion_ids_offset += 0x04 * i.motion.size();
    }
    s.write(0x10);
    s.position_pop();

    s.position_push(bone_name_offsets_offset, SEEK_SET);
    for (std::string& i : mot_db->bone_name) {
        s.write_uint32_t((uint32_t)bone_name_offset);
        bone_name_offset += i.size() + 1;
    }
    s.position_pop();

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(0x01);
    s.write_uint32_t((uint32_t)motion_sets_offset);
    s.write_uint32_t((uint32_t)motion_set_ids_offset);
    s.write_uint32_t((uint32_t)motion_set_count);
    s.write_uint32_t((uint32_t)bone_name_offsets_offset);
    s.write_uint32_t((uint32_t)bone_name_count);
    s.position_pop();
}
