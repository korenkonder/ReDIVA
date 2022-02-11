/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vector.h"

typedef struct motion_info {
    string name;
    uint64_t name_hash;
    uint32_t id;
} motion_info;

vector(motion_info)

typedef struct motion_set_info {
    string name;
    uint64_t name_hash;
    uint32_t id;
    vector_motion_info motion;
} motion_set_info;

vector(motion_set_info)

typedef struct motion_database {
    bool ready;

    vector_string bone_name;
    vector_motion_set_info motion_set;
} motion_database;

extern void motion_database_init(motion_database* mot_db);
extern void motion_database_read(motion_database* mot_db, char* path);
extern void motion_database_wread(motion_database* mot_db, wchar_t* path);
extern void motion_database_mread(motion_database* mot_db, void* data, size_t length);
extern void motion_database_write(motion_database* mot_db, char* path);
extern void motion_database_wwrite(motion_database* mot_db, wchar_t* path);
extern void motion_database_mwrite(motion_database* mot_db, void** data, size_t* length);
extern bool motion_database_load_file(void* data, char* path, char* file, uint32_t hash);
extern void motion_database_merge_mdata(motion_database* mot_db,
    motion_database* base_mot_db, motion_database* mdata_mot_db);
extern void motion_database_split_mdata(motion_database* mot_db,
    motion_database* base_mot_db, motion_database* mdata_mot_db);
extern motion_set_info* motion_database_get_motion_set_by_id(motion_database* mot_db, uint32_t id);
extern motion_set_info* motion_database_get_motion_set_by_name(motion_database* mot_db, char* name);
extern motion_set_info* motion_database_get_motion_set_by_name(motion_database* mot_db, const char* name);
extern uint32_t motion_database_get_motion_set_id(motion_database* mot_db, char* name);
extern uint32_t motion_database_get_motion_set_id(motion_database* mot_db, const char* name);
extern char* motion_database_get_motion_set_name(motion_database* mot_db, uint32_t id);
extern motion_info* motion_database_get_motion_by_id(motion_database* mot_db, uint32_t id);
extern motion_info* motion_database_get_motion_by_name(motion_database* mot_db, char* name);
extern motion_info* motion_database_get_motion_by_name(motion_database* mot_db, const char* name);
extern uint32_t motion_database_get_motion_id(motion_database* mot_db, char* name);
extern uint32_t motion_database_get_motion_id(motion_database* mot_db, const char* name);
extern char* motion_database_get_motion_name(motion_database* mot_db, uint32_t id);
extern void motion_database_free(motion_database* mot_db);

extern void motion_info_free(motion_info* info);
extern void motion_set_info_free(motion_set_info* set_info);
