/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/database/auth_3d.h"
#include "../KKdLib/database/bone.h"
#include "../KKdLib/database/motion.h"
#include "../KKdLib/database/object.h"
#include "../KKdLib/database/stage.h"
#include "../KKdLib/database/texture.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/string.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"

typedef enum data_type {
    DATA_AFT = 0,
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    DATA_F2LE,
    DATA_F2BE,
    DATA_FT,
    DATA_M39,
    DATA_VRFL,
    DATA_X,
    DATA_XHD,
#endif
    DATA_MAX,
} data_type;

typedef struct data_struct_file {
    string path;
    string name;
} data_struct_file;

vector(data_struct_file)

typedef struct data_struct_directory {
    string path;
    string name;
} data_struct_directory;

vector(data_struct_directory)

typedef struct data_struct_path {
    string path;
    vector_data_struct_directory data;
} data_struct_path;

vector(data_struct_path)

#if defined(CRE_DEV) || defined(CLOUD_DEV)
typedef struct data_f2 {
    bone_database bone_data;
} data_f2;
#endif

typedef struct data_ft {
    auth_3d_database auth_3d_db;
    bone_database bone_data;
    motion_database mot_db;
    object_database obj_db;
    stage_database stage_data;
    texture_database tex_db;

    auth_3d_database_file base_auth_3d_db;
    motion_database base_mot_db;
    object_database base_obj_db;
    stage_database base_stage_data;
    texture_database base_tex_db;
} data_ft;

#if defined(CRE_DEV) || defined(CLOUD_DEV)
typedef struct data_x {
    bone_database bone_data;
} data_x;
#endif

typedef struct data_struct {
    data_type type;
    bool ready;
    vector_data_struct_path data_paths;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    vector_string glitter_list_names;
    union {
        vector_uint64_t glitter_list_fnv1a64m;
        vector_uint32_t glitter_list_murmurhash;
    };
#endif
    union {
#if defined(CRE_DEV) || defined(CLOUD_DEV)
        data_f2 data_f2;
#endif
        data_ft data_ft;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
        data_x data_x;
#endif
    };
} data_struct;

extern data_struct data_list[];

#define DATA_LOAD_FILE_FUNC(f) 

extern void data_struct_init();
extern void data_struct_load(char* path);
extern void data_struct_wload(wchar_t* path);
extern void data_struct_get_directory_files(data_struct* c, char* dir, vector_data_struct_file* data_files);
extern bool data_struct_load_file(data_struct* c, void* data, char* dir, char* file,
    bool (*load_func)(void* data, char* path, char* file, uint32_t hash));
extern bool data_struct_load_file_by_hash(data_struct* c, void* data, char* dir, uint32_t hash,
    bool (*load_func)(void* data, char* path, char* file, uint32_t hash));
extern void data_struct_free();

extern void data_struct_directory_free(data_struct_directory* data_dir);
extern void data_struct_path_free(data_struct_path* data_path);
extern void data_struct_file_free(data_struct_file* file);
