/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"
#include "../prj/vector_pair.hpp"
#include "../hash.hpp"

struct object_info {
    uint32_t id;
    uint32_t set_id;

    inline object_info() {
        this->id = (uint32_t)-1;
        this->set_id = (uint32_t)-1;
    }

    inline object_info(uint32_t id, uint32_t set_id) {
        this->id = id;
        this->set_id = set_id;
    }

    inline bool is_null() const {
        return id == (uint32_t)-1 && set_id == (uint32_t)-1;
    }

    inline bool not_null() const {
        return id != (uint32_t)-1 || set_id != (uint32_t)-1;
    }

    inline bool is_null_modern() const {
        return id == (uint32_t)-1 && set_id == (uint32_t)-1
            || id == hash_murmurhash_empty || id == hash_murmurhash_null
            || set_id == hash_murmurhash_empty || set_id == hash_murmurhash_null;
    }

    inline bool not_null_modern() const {
        return id != (uint32_t)-1 || set_id != (uint32_t)-1
            || (id != hash_murmurhash_empty && id != hash_murmurhash_null
                && set_id != hash_murmurhash_empty && set_id != hash_murmurhash_null);
    }
};

inline bool operator >(const object_info& left, const object_info& right) {
    return *(uint64_t*)&left > *(uint64_t*)&right;
}

inline bool operator <(const object_info& left, const object_info& right) {
    return *(uint64_t*)&left < *(uint64_t*)&right;
}

inline bool operator >=(const object_info& left, const object_info& right) {
    return *(uint64_t*)&left >= *(uint64_t*)&right;
}

inline bool operator <=(const object_info& left, const object_info& right) {
    return *(uint64_t*)&left <= *(uint64_t*)&right;
}

inline bool operator ==(const object_info& left, const object_info& right) {
    return *(uint64_t*)&left == *(uint64_t*)&right;
}

inline bool operator !=(const object_info& left, const object_info& right) {
    return *(uint64_t*)&left != *(uint64_t*)&right;
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
    ~object_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

struct object_database {
    std::vector<object_set_info> object_set;
    prj::vector_pair<uint32_t, object_set_info*> obj_set_ids;
    prj::vector_pair<uint32_t, object_set_info*> obj_set_murmurhashes;
    prj::vector_pair<object_info, object_info_data*> obj_infos;
    prj::vector_pair<uint64_t, object_info_data*> obj_fnv1a64m_hashes;
    prj::vector_pair<uint64_t, object_info_data*> obj_fnv1a64m_hashes_upper;
    prj::vector_pair<uint32_t, object_info_data*> obj_murmurhashes;
    prj::vector_pair<uint64_t, object_info> obj_info_fnv1a64m_hashes;
    prj::vector_pair<uint64_t, object_info> obj_info_fnv1a64m_hashes_upper;
    prj::vector_pair<uint32_t, object_info> obj_info_murmurhashes;

    object_database();
    ~object_database();

    void add(object_database_file* obj_db_file);
    void clear();
    void update();

    const object_set_info* get_object_set_info(const char* name) const;
    const object_set_info* get_object_set_info(uint32_t set_id) const;
    const object_info_data* get_object_info_data(const char* name) const;
    const object_info_data* get_object_info_data_by_fnv1a64m_hash(uint64_t hash) const;
    const object_info_data* get_object_info_data_by_fnv1a64m_hash_upper(uint64_t hash) const;
    const object_info_data* get_object_info_data_by_murmurhash(uint32_t hash) const;
    uint32_t get_object_set_id(const char* name) const;
    const char* get_object_set_name(uint32_t set_id) const;
    object_info get_object_info(const char* name) const;
    object_info get_object_info_by_fnv1a64m_hash(uint64_t hash) const;
    object_info get_object_info_by_fnv1a64m_hash_upper(uint64_t hash) const;
    object_info get_object_info_by_murmurhash(uint32_t hash) const;
    const char* get_object_name(object_info obj_info) const;
};
