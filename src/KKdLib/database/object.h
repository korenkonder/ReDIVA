/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.h"

typedef struct object_info {
    uint32_t id;
    uint32_t set_id;

    object_info();
    object_info(uint32_t id, uint32_t set_id);
    bool is_null();
    bool not_null();
} object_info;

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

class object_info_data {
public:
    uint32_t id;
    std::string name;
    uint64_t name_hash_fnv1a64m;
    uint64_t name_hash_fnv1a64m_upper;
    uint32_t name_hash_murmurhash;

    object_info_data();
    ~object_info_data();
};

class object_set_info {
public:
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

class object_database {
public:
    bool ready;
    bool modern;
    bool is_x;

    std::vector<object_set_info> object_set;

    object_database();
    ~object_database();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t length, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* length);

    void merge_mdata(object_database* base_obj_db, object_database* mdata_obj_db);
    void split_mdata(object_database* base_obj_db, object_database* mdata_obj_db);

    bool get_object_set_info(const char* name, object_set_info** set_info);
    bool get_object_set_info(uint32_t set_id, object_set_info** set_info);
    bool get_object_info_data(const char* name, object_info_data** info);
    bool get_object_info_data_by_fnv1a64m_hash(uint64_t hash, object_info_data** info);
    bool get_object_info_data_by_fnv1a64m_hash_upper(uint64_t hash, object_info_data** info);
    bool get_object_info_data_by_murmurhash(uint32_t hash, object_info_data** info);
    uint32_t get_object_set_id(const char* name);
    object_info get_object_info(const char* name);
    const char* get_object_name(object_info obj_info);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
