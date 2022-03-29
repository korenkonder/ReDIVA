/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../KKdLib/default.h"
#include "../KKdLib/database/auth_3d.h"
#include "../KKdLib/database/bone.h"
#include "../KKdLib/database/motion.h"
#include "../KKdLib/database/object.h"
#include "../KKdLib/database/stage.h"
#include "../KKdLib/database/texture.h"
#include "../KKdLib/vec.h"

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

class data_struct_file {
public:
    std::string path;
    std::string name;

    data_struct_file();
    ~data_struct_file();
};

class data_struct_directory {
public:
    std::string path;
    std::string name;

    data_struct_directory();
    ~data_struct_directory();
};

class data_struct_path {
public:
    std::string path;
    std::vector<data_struct_directory> data;

    data_struct_path();
    ~data_struct_path();
};

#if defined(CRE_DEV) || defined(CLOUD_DEV)
class data_f2 {
public:
    bone_database bone_data;

    data_f2();
    ~data_f2();
};
#endif

class data_ft {
public:
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

    data_ft();
    ~data_ft();
};

#if defined(CRE_DEV) || defined(CLOUD_DEV)
class data_x {
public:
    bone_database bone_data;

    data_x();
    ~data_x();
};
#endif

class data_struct {
public:
    data_type type;
    bool ready;
    std::vector<data_struct_path> data_paths;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    std::vector<std::string> glitter_list_names;
    std::vector<uint64_t> glitter_list_fnv1a64m;
    std::vector<uint32_t> glitter_list_murmurhash;
#endif
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    data_f2 data_f2;
#endif
    data_ft data_ft;
#if defined(CRE_DEV) || defined(CLOUD_DEV)
    data_x data_x;
#endif

    data_struct();
    virtual ~data_struct();

    bool check_file_exists(const char* dir, const char* file);
    bool check_file_exists(const char* dir, uint32_t hash);
    void get_directory_files(const char* dir, std::vector<data_struct_file>* data_files);
    bool get_file(const char* dir, uint32_t hash, std::string* file);
    bool load_file(void* data, const char* dir, const char* file,
        bool (*load_func)(void* data, const char* path, const  char* file, uint32_t hash));
    bool load_file(void* data, const char* dir, uint32_t hash,
        bool (*load_func)(void* data, const char* path, const char* file, uint32_t hash));
};

extern data_struct* data_list;

#define DATA_LOAD_FILE_FUNC(f) 

extern void data_struct_init();
extern void data_struct_load(const char* path);
extern void data_struct_load(const wchar_t* path);
extern void data_struct_free();
