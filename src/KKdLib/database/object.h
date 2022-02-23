/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vector.h"

typedef struct object_info {
    uint32_t id;
    uint32_t set_id;

    inline bool is_null() { return id == (uint32_t)-1 && set_id == (uint32_t)-1; }
    inline bool not_null() { return id != (uint32_t)-1 || set_id != (uint32_t)-1; }
    inline bool operator ==(object_info& right) { return id == right.id && set_id == right.set_id; }
    inline bool operator !=(object_info& right) { return id != right.id || set_id != right.set_id; }
} object_info;

vector_old(object_info)

typedef struct object_info_data {
    uint32_t id;
    string name;
    uint64_t name_hash_fnv1a64m;
    uint64_t name_hash_fnv1a64m_upper;
    uint32_t name_hash_murmurhash;
} object_info_data;

vector_old(object_info_data)

typedef struct object_set_info {
    string name;
    uint64_t name_hash;
    uint32_t id;
    string object_file_name;
    string texture_file_name;
    string archive_file_name;
    vector_old_object_info_data object;
} object_set_info;

vector_old(object_set_info)

typedef struct object_database {
    bool ready;
    bool modern;
    bool is_x;

    vector_old_string bone_name;
    vector_old_object_set_info object_set;
} object_database;

extern const object_info object_info_null;

extern void object_database_init(object_database* obj_db);
extern void object_database_read(object_database* obj_db, char* path, bool modern);
extern void object_database_wread(object_database* obj_db, wchar_t* path, bool modern);
extern void object_database_mread(object_database* obj_db, void* data, size_t length, bool modern);
extern void object_database_write(object_database* obj_db, char* path);
extern void object_database_wwrite(object_database* obj_db, wchar_t* path);
extern void object_database_mwrite(object_database* obj_db, void** data, size_t* length);
extern bool object_database_load_file(void* data, char* path, char* file, uint32_t hash);
extern void object_database_merge_mdata(object_database* obj_db,
    object_database* base_obj_db, object_database* mdata_obj_db);
extern void object_database_split_mdata(object_database* obj_db,
    object_database* base_obj_db, object_database* mdata_obj_db);
extern bool object_database_get_object_set_info(object_database* obj_db,
    char* name, object_set_info** set_info);
extern bool object_database_get_object_set_info(object_database* obj_db,
    const char* name, object_set_info** set_info);
extern bool object_database_get_object_set_info_by_set_id(object_database* obj_db,
    uint32_t set_id, object_set_info** set_info);
extern bool object_database_get_object_info_data(object_database* obj_db,
    char* name, object_info_data** info);
extern bool object_database_get_object_info_data(object_database* obj_db,
    const char* name, object_info_data** info);
extern bool object_database_get_object_info_data_by_fnv1a64m_hash(object_database* obj_db,
    uint64_t hash, object_info_data** info);
extern bool object_database_get_object_info_data_by_fnv1a64m_hash_upper(object_database* obj_db,
    uint64_t hash, object_info_data** info);
extern bool object_database_get_object_info_data_by_murmurhash(object_database* obj_db,
    uint32_t hash, object_info_data** info);
extern object_info object_database_get_object_info(object_database* obj_db, char* name);
extern object_info object_database_get_object_info(object_database* obj_db, const char* name);
extern char* object_database_get_object_name(object_database* obj_db, object_info obj_info);
extern void object_database_free(object_database* obj_db);

extern void object_info_data_free(object_info_data* info);
extern void object_set_info_free(object_set_info* set_info);
