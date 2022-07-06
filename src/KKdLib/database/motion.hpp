/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"

struct motion_info_file {
    std::string name;
    uint32_t id;

    motion_info_file();
    ~motion_info_file();
};

struct motion_set_info_file {
    std::string name;
    uint32_t id;
    std::vector<motion_info_file> motion;

    motion_set_info_file();
    ~motion_set_info_file();
};

struct motion_info {
    std::string name;
    uint32_t name_hash;
    uint32_t id;

    motion_info();
    ~motion_info();
};

struct motion_set_info {
    std::string name;
    uint32_t name_hash;
    uint32_t id;
    std::vector<motion_info> motion;

    motion_set_info();
    ~motion_set_info();
};

struct motion_database_file {
    bool ready;

    std::vector<std::string> bone_name;
    std::vector<motion_set_info_file> motion_set;

    motion_database_file();
    virtual ~motion_database_file();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct motion_database {
    std::vector<std::string> bone_name;
    std::vector<motion_set_info> motion_set;

    motion_database();
    virtual ~motion_database();

    void add(motion_database_file* mot_db_file);

    motion_set_info* get_motion_set_by_id(uint32_t id);
    motion_set_info* get_motion_set_by_name(const char* name);
    motion_set_info* get_motion_set_by_motion_id(uint32_t id);
    motion_set_info* get_motion_set_by_motion_name(const char* name);
    uint32_t get_motion_set_id_by_motion_id(uint32_t id);
    uint32_t get_motion_set_id_by_motion_name(const char* name);
    uint32_t get_motion_set_id(const char* name);
    const char* get_motion_set_name(uint32_t id);
    motion_info* get_motion_by_id(uint32_t id);
    motion_info* get_motion_by_name(const char* name);
    uint32_t get_motion_id(const char* name);
    const char* get_motion_name(uint32_t id);
};
