/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"

struct texture_info_file {
    std::string name;
    uint32_t id;

    texture_info_file();
    ~texture_info_file();
};

struct texture_info {
    std::string name;
    uint32_t name_hash;
    uint32_t id;

    texture_info();
    ~texture_info();
};

struct texture_database_file {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<texture_info_file> texture;

    texture_database_file();
    virtual ~texture_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct texture_database {
    std::vector<texture_info> texture;

    texture_database();
    virtual ~texture_database();

    void add(texture_database_file* tex_db_file);

    uint32_t get_texture_id(const char* name);
    const char* get_texture_name(uint32_t id);
};
