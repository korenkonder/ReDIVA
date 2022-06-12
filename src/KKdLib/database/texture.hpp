/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"

struct texture_info {
    std::string name;
    uint32_t name_hash;
    uint32_t id;

    texture_info();
    ~texture_info();
};

struct texture_database {
    bool ready;
    bool modern;
    bool is_x;

    std::vector<texture_info> texture;

    texture_database();
    ~texture_database();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    void merge_mdata(texture_database* base_tex_db,
        texture_database* mdata_tex_db);
    void split_mdata(texture_database* base_tex_db,
        texture_database* mdata_tex_db);

    uint32_t get_texture_id(const char* name);
    const char* get_texture_name(uint32_t id);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
