/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/aet.hpp"
#include "../KKdLib/database/auth_3d.hpp"
#include "../KKdLib/database/bone.hpp"
#include "../KKdLib/database/motion.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/database/sprite.hpp"
#include "../KKdLib/database/stage.hpp"
#include "../KKdLib/database/texture.hpp"
#include "../KKdLib/vec.hpp"

enum data_type {
    DATA_AFT = 0,
#if defined(CRE_DEV)
    DATA_F2LE,
    DATA_F2BE,
    DATA_FT,
    DATA_M39,
    DATA_VRFL,
    DATA_X,
    DATA_XHD,
#endif
    DATA_MAX,
};

struct data_struct_file {
    std::string path;
    std::string name;

    data_struct_file();
    ~data_struct_file();
};

struct data_struct_directory {
    std::string path;
    std::string name;

    data_struct_directory();
    ~data_struct_directory();
};

struct data_struct_path {
    std::string path;
    std::vector<data_struct_directory> data;

    data_struct_path();
    ~data_struct_path();
};

#if defined(CRE_DEV)
struct data_f2 {
    bone_database bone_data;

    data_f2();
    ~data_f2();
};
#endif

struct data_ft {
    aet_database aet_db;
    auth_3d_database auth_3d_db;
    bone_database bone_data;
    motion_database mot_db;
    object_database obj_db;
    sprite_database spr_db;
    stage_database stage_data;
    texture_database tex_db;

    data_ft();
    ~data_ft();
};

#if defined(CRE_DEV)
struct data_x {
    bone_database bone_data;

    data_x();
    ~data_x();
};
#endif

struct data_struct {
    data_type type;
    bool ready;
    std::vector<data_struct_path> data_paths;
#if defined(CRE_DEV)
    std::vector<std::string> glitter_list_names;
    prj::vector_pair<uint64_t, const char*> glitter_list_fnv1a64m;
    prj::vector_pair<uint64_t, const char*>  glitter_list_murmurhash;
#endif
#if defined(CRE_DEV)
    data_f2 data_f2;
#endif
    data_ft data_ft;
#if defined(CRE_DEV)
    data_x data_x;
#endif

    data_struct();
    ~data_struct();

    bool check_directory_exists(const char* dir);
    bool check_file_exists(const char* path);
    bool check_file_exists(const char* dir, const char* file);
    bool check_file_exists(const char* dir, uint32_t hash);
    std::vector<data_struct_file> get_directory_files(const char* dir);
    bool get_file(const char* dir, uint32_t hash, const char* ext, std::string& file);
    bool get_file_path(std::string& path);
    bool get_file_path(const char* dir, const char* file, std::string& path);
    bool load_file(void* data, const char* path,
        bool (*load_func)(void* data, const char* dir, const char* file, uint32_t hash));
    bool load_file(void* data, const char* dir, const char* file,
        bool (*load_func)(void* data, const char* dir, const char* file, uint32_t hash));
    bool load_file(void* data, const char* dir, uint32_t hash, const char* ext,
        bool (*load_func)(void* data, const char* dir, const char* file, uint32_t hash));
};

extern data_struct* data_list;

extern void data_struct_init();
extern void data_struct_load(const char* path);
extern void data_struct_load(const wchar_t* path);
extern void data_struct_load_db();
extern void data_struct_free();
