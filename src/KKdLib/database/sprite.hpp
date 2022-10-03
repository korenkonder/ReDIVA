/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include <string>
#include <vector>

struct sprite_info_file {
    uint32_t id;
    std::string name;
    uint16_t index;

    sprite_info_file();
    ~sprite_info_file();
};

struct sprite_info {
    uint32_t id;
    std::string name;
    uint32_t name_hash;
    uint16_t index;

    sprite_info();
    ~sprite_info();
};

struct sprite_texture_info_file {
    uint32_t id;
    std::string name;
    uint16_t index;

    sprite_texture_info_file();
    ~sprite_texture_info_file();
};

struct sprite_texture_info {
    uint32_t id;
    std::string name;
    uint32_t name_hash;
    uint16_t index;

    sprite_texture_info();
    ~sprite_texture_info();
};

struct sprite_set_file {
    uint32_t id;
    std::string name;
    std::string file_name;
    std::vector<sprite_info_file> sprite;
    std::vector<sprite_texture_info_file> texture;
    uint32_t index;

    sprite_set_file();
    ~sprite_set_file();
};

struct sprite_set {
    uint32_t id;
    std::string name;
    uint32_t name_hash;
    std::string file_name;
    uint32_t file_name_hash;
    std::vector<sprite_info> sprite;
    std::vector<sprite_texture_info> texture;
    uint32_t index;

    sprite_set();
    ~sprite_set();
};

struct sprite_database_file {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<sprite_set_file> sprite_set;

    sprite_database_file();
    virtual ~sprite_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct sprite_database {
    std::vector<sprite_set> sprite_set;

    sprite_database();
    virtual ~sprite_database();

    void add(sprite_database_file* spr_db_file);
};
