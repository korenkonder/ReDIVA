/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"

struct object_info {
    uint32_t id;
    uint32_t set_id;

    object_info();
    object_info(uint32_t id, uint32_t set_id);
    bool is_null();
    bool not_null();
};

inline bool object_info::is_null() {
    return id == (uint32_t)-1 && set_id == (uint32_t)-1;
}

inline bool object_info::not_null() {
    return id != (uint32_t)-1 || set_id != (uint32_t)-1;
}

inline bool operator >(const object_info& left, const object_info& right) {
    return left.set_id > right.set_id && left.id > right.id;
}

inline bool operator <(const object_info& left, const object_info& right) {
    return left.set_id < right.set_id&& left.id < right.id;
}

inline bool operator >=(const object_info& left, const object_info& right) {
    return left.set_id >= right.set_id && left.id >= right.id;
}

inline bool operator <=(const object_info& left, const object_info& right) {
    return left.set_id <= right.set_id && left.id <= right.id;
}

inline bool operator ==(const object_info& left, const object_info& right) {
    return left.set_id == right.set_id && left.id == right.id;
}

inline bool operator !=(const object_info& left, const object_info& right) {
    return left.set_id != right.set_id || left.id != right.id;
}

struct object_info_data_file {
    uint32_t id;
    std::string name;

    object_info_data_file();
    ~object_info_data_file();
};

struct object_set_info_file {
    std::string name;
    uint32_t id;
    std::string object_file_name;
    std::string texture_file_name;
    std::string archive_file_name;
    std::vector<object_info_data_file> object;

    object_set_info_file();
    ~object_set_info_file();
};

struct object_info_data {
    uint32_t id;
    std::string name;
    uint64_t name_hash_fnv1a64m;
    uint64_t name_hash_fnv1a64m_upper;
    uint32_t name_hash_murmurhash;

    object_info_data();
    ~object_info_data();
};

struct object_set_info {
    std::string name;
    uint32_t name_hash;
    uint32_t id;
    std::string object_file_name;
    std::string texture_file_name;
    std::string archive_file_name;
    std::vector<object_info_data> object;

    object_set_info();
    ~object_set_info();
};

struct object_database_file {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<object_set_info_file> object_set;

    object_database_file();
    virtual ~object_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct object_database {
    std::vector<object_set_info> object_set;

    object_database();
    virtual ~object_database();

    void add(object_database_file* obj_db_file);

    bool get_object_set_info(const char* name, object_set_info** set_info);
    bool get_object_set_info(uint32_t set_id, object_set_info** set_info);
    bool get_object_info_data(const char* name, object_info_data** info);
    bool get_object_info_data_by_fnv1a64m_hash(uint64_t hash, object_info_data** info);
    bool get_object_info_data_by_fnv1a64m_hash_upper(uint64_t hash, object_info_data** info);
    bool get_object_info_data_by_murmurhash(uint32_t hash, object_info_data** info);
    uint32_t get_object_set_id(const char* name);
    const char* get_object_set_name(uint32_t set_id);
    object_info get_object_info(const char* name);
    const char* get_object_name(object_info obj_info);
};
