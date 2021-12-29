/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vector.h"

typedef enum auth_3d_database_uid_flags {
    AUTH_3D_DATABASE_UID_ORG_UID = 0x01,
    AUTH_3D_DATABASE_UID_SIZE    = 0x02,
} auth_3d_database_uid_flags;

typedef struct auth_3d_database_uid {
    bool enabled;
    string category;
    string name;
    int32_t org_uid;
    float_t size;
} auth_3d_database_uid;

typedef struct auth_3d_database_uid_file {
    auth_3d_database_uid_flags flags;
    string category;
    int32_t org_uid;
    float_t size;
    string value;
} auth_3d_database_uid_file;

typedef struct auth_3d_database_category {
    string name;
    vector_int32_t uid;
} auth_3d_database_category;

vector(auth_3d_database_category)

vector(auth_3d_database_uid)

typedef struct auth_3d_database {
    bool ready;

    vector_auth_3d_database_category category;
    vector_auth_3d_database_uid uid;
} auth_3d_database;

vector(auth_3d_database_uid_file)

typedef struct auth_3d_database_file {
    bool ready;

    vector_string category;
    vector_auth_3d_database_uid_file uid;
    int32_t uid_max;
} auth_3d_database_file;

extern void auth_3d_database_init(auth_3d_database* auth_3d_db);
extern void auth_3d_database_merge_mdata(auth_3d_database* auth_3d_db,
    auth_3d_database_file* base_auth_3d_db, auth_3d_database_file* mdata_auth_3d_db);
extern void auth_3d_database_split_mdata(auth_3d_database* auth_3d_db,
    auth_3d_database_file* base_auth_3d_db, auth_3d_database_file* mdata_auth_3d_db);
extern void auth_3d_database_free(auth_3d_database* auth_3d_db);

extern void auth_3d_database_file_init(auth_3d_database_file* auth_3d_db_file);
extern void auth_3d_database_file_read(auth_3d_database_file* auth_3d_db_file, char* path);
extern void auth_3d_database_file_wread(auth_3d_database_file* auth_3d_db_file, wchar_t* path);
extern void auth_3d_database_file_mread(auth_3d_database_file* auth_3d_db_file, void* data, size_t length);
extern void auth_3d_database_file_write(auth_3d_database_file* auth_3d_db_file, char* path);
extern void auth_3d_database_file_wwrite(auth_3d_database_file* auth_3d_d_fileb, wchar_t* path);
extern void auth_3d_database_file_mwrite(auth_3d_database_file* auth_3d_db_file, void** data, size_t* length);
extern bool auth_3d_database_file_load_file(void* data, char* path, char* file, uint32_t hash);
extern void auth_3d_database_file_free(auth_3d_database_file* auth_3d_db_file);

extern void auth_3d_database_category_free(auth_3d_database_category* cat);
extern void auth_3d_database_uid_free(auth_3d_database_uid* uid);
extern void auth_3d_database_uid_file_free(auth_3d_database_uid_file* uid);
