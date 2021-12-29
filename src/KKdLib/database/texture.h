/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vector.h"

typedef struct texture_info {
    string name;
    uint64_t name_hash;
    uint32_t id;
} texture_info;

vector(texture_info)

typedef struct texture_database {
    bool ready;
    bool modern;
    bool is_x;

    vector_texture_info texture;
} texture_database;

extern void texture_database_init(texture_database* tex_db);
extern void texture_database_read(texture_database* tex_db, char* path, bool modern);
extern void texture_database_wread(texture_database* tex_db, wchar_t* path, bool modern);
extern void texture_database_mread(texture_database* tex_db, void* data, size_t length, bool modern);
extern void texture_database_write(texture_database* tex_db, char* path);
extern void texture_database_wwrite(texture_database* tex_db, wchar_t* path);
extern void texture_database_mwrite(texture_database* tex_db, void** data, size_t* length);
extern bool texture_database_load_file(void* data, char* path, char* file, uint32_t hash);
extern void texture_database_merge_mdata(texture_database* tex_db,
    texture_database* base_tex_db, texture_database* mdata_tex_db);
extern void texture_database_split_mdata(texture_database* tex_db,
    texture_database* base_tex_db, texture_database* mdata_tex_db);
extern uint32_t texture_database_get_texture_id(texture_database* tex_db, char* name);
extern char* texture_database_get_texture_name(texture_database* tex_db, uint32_t id);
extern void texture_database_free(texture_database* tex_db);

extern void texture_info_free(texture_info* info);
