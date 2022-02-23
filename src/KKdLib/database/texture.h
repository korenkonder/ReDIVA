/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.h"

class texture_info {
public:
    std::string name;
    uint64_t name_hash;
    uint32_t id;

    texture_info();
    ~texture_info();
};

typedef struct texture_database {
public:
    bool ready;
    bool modern;
    bool is_x;

    std::vector<texture_info> texture;

    texture_database();
    ~texture_database();

    void read(char* path, bool modern);
    void read(wchar_t* path, bool modern);
    void read(void* data, size_t length, bool modern);
    void write(char* path);
    void write(wchar_t* path);
    void write(void** data, size_t* length);

    void merge_mdata(texture_database* base_tex_db,
        texture_database* mdata_tex_db);
    void split_mdata(texture_database* base_tex_db,
        texture_database* mdata_tex_db);

    uint32_t get_texture_id(char* name);
    uint32_t get_texture_id(const char* name);
    const char* get_texture_name(uint32_t id);

    static bool load_file(void* data, char* path, char* file, uint32_t hash);
} texture_database;
