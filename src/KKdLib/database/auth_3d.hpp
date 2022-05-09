/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.h"

enum auth_3d_database_uid_flags {
    AUTH_3D_DATABASE_UID_ORG_UID = 0x01,
    AUTH_3D_DATABASE_UID_SIZE    = 0x02,
};

struct auth_3d_database_uid {
    bool enabled;
    std::string category;
    std::string name;
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
    bool ready;

    std::vector<auth_3d_database_category> category;
    std::vector<auth_3d_database_uid> uid;

    auth_3d_database();
    ~auth_3d_database();

    void merge_mdata(auth_3d_database_file* base_auth_3d_db,
        auth_3d_database_file* mdata_auth_3d_db);
    void split_mdata(auth_3d_database_file* base_auth_3d_db,
        auth_3d_database_file* mdata_auth_3d_db);
};
