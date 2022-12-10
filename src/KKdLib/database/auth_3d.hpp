/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"

enum auth_3d_database_uid_flags {
    AUTH_3D_DATABASE_UID_ORG_UID = 0x01,
    AUTH_3D_DATABASE_UID_SIZE    = 0x02,
};

struct auth_3d_database_uid {
    bool enabled;
    std::string category;
    uint32_t category_hash;
    std::string name;
    uint32_t name_hash;
    int32_t org_uid;
    float_t size;

    auth_3d_database_uid();
    ~auth_3d_database_uid();
};

struct auth_3d_database_uid_file {
    auth_3d_database_uid_flags flags;
    std::string category;
    int32_t org_uid;
    float_t size;
    std::string value;

    auth_3d_database_uid_file();
    ~auth_3d_database_uid_file();
};

struct auth_3d_database_category {
    std::string name;
    uint32_t name_hash;
    std::vector<int32_t> uid;

    auth_3d_database_category();
    ~auth_3d_database_category();
};

struct auth_3d_database_file {
    bool ready;

    std::vector<std::string> category;
    std::vector<auth_3d_database_uid_file> uid;
    int32_t uid_max;

    auth_3d_database_file();
    ~auth_3d_database_file();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct auth_3d_database {
    std::vector<auth_3d_database_category> category;
    std::vector<auth_3d_database_uid> uid;

    auth_3d_database();
    ~auth_3d_database();

    void add(auth_3d_database_file* auth_3d_db_file, bool mdata);
    int32_t get_category_index(const char* name);
    void get_category_uids(const char* name, std::vector<int32_t>& uid);
    int32_t get_uid(const char* name);
};
