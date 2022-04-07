/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.h"

class motion_info {
public:
    std::string name;
    uint32_t name_hash;
    uint32_t id;

    motion_info();
    ~motion_info();
};

class motion_set_info {
public:
    std::string name;
    uint32_t name_hash;
    uint32_t id;
    std::vector<motion_info> motion;

    motion_set_info();
    ~motion_set_info();
};

class motion_database {
public:
    bool ready;

    std::vector<std::string> bone_name;
    std::vector<motion_set_info> motion_set;

    motion_database();
    ~motion_database();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t length);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* length);

    void merge_mdata(motion_database* base_mot_db, motion_database* mdata_mot_db);
    void split_mdata(motion_database* base_mot_db, motion_database* mdata_mot_db);

    motion_set_info* get_motion_set_by_id(uint32_t id);
    motion_set_info* get_motion_set_by_name(const char* name);
    uint32_t get_motion_set_id(const char* name);
    const char* get_motion_set_name(uint32_t id);
    motion_info* get_motion_by_id(uint32_t id);
    motion_info* get_motion_by_name(const char* name);
    uint32_t get_motion_id(const char* name);
    const char* get_motion_name(uint32_t id);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
