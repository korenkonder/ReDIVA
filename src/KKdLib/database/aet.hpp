/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <string>
#include <vector>
#include "../default.hpp"
#include "../prj/vector_pair_combine.hpp"

struct aet_info {
    uint16_t index;
    uint16_t set_index;

    inline aet_info() {
        this->index = (uint16_t)-1;
        this->set_index = (uint16_t)-1;
    }

    inline aet_info(uint16_t index, uint16_t set_index) {
        this->index = index;
        this->set_index = set_index;
    }

    inline bool is_null() const {
        return index == (uint16_t)-1 && set_index == (uint16_t)-1;
    }

    inline bool not_null() const {
        return index != (uint16_t)-1 || set_index != (uint16_t)-1;
    }
};

inline bool operator >(const aet_info& left, const aet_info& right) {
    return *(uint32_t*)&left > *(uint32_t*)&right;
}

inline bool operator <(const aet_info& left, const aet_info& right) {
    return *(uint32_t*)&left < *(uint32_t*)&right;
}

inline bool operator >=(const aet_info& left, const aet_info& right) {
    return *(uint32_t*)&left >= *(uint32_t*)&right;
}

inline bool operator <=(const aet_info& left, const aet_info& right) {
    return *(uint32_t*)&left <= *(uint32_t*)&right;
}

inline bool operator ==(const aet_info& left, const aet_info& right) {
    return *(uint32_t*)&left == *(uint32_t*)&right;
}

inline bool operator !=(const aet_info& left, const aet_info& right) {
    return *(uint32_t*)&left != *(uint32_t*)&right;
}

struct aet_db_aet_file {
    uint32_t id;
    std::string name;
    uint16_t index;

    aet_db_aet_file();
    ~aet_db_aet_file();
};

struct aet_db_aet {
    uint32_t id;
    std::string name;
    aet_info info;
    int32_t load_count;

    aet_db_aet();
    ~aet_db_aet();
};

struct aet_db_aet_set_file {
    uint32_t id;
    std::string name;
    std::string file_name;
    std::vector<aet_db_aet_file> aet;
    uint32_t index;
    uint32_t sprite_set_id;

    aet_db_aet_set_file();
    ~aet_db_aet_set_file();
};

struct aet_db_aet_set {
    uint32_t id;
    std::string name;
    std::string file_name;
    uint32_t index;
    uint32_t sprite_set_id;

    aet_db_aet_set();
    ~aet_db_aet_set();
};

struct aet_database_file {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<aet_db_aet_set_file> aet_set;

    aet_database_file();
    ~aet_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct aet_database {
    std::list<aet_db_aet_set> aet_sets;
    prj::vector_pair_combine<uint32_t, aet_db_aet_set*> aet_set_ids;
    prj::vector_pair_combine<uint32_t, aet_db_aet_set*> aet_set_indices;
    prj::vector_pair_combine<std::string, aet_db_aet_set*> aet_set_names;
    std::list<aet_db_aet> aets;
    prj::vector_pair_combine<uint32_t, aet_db_aet*> aet_ids;
    prj::vector_pair_combine<std::string, aet_db_aet*> aet_names;
    prj::vector_pair_combine<aet_info, aet_db_aet*> aet_indices;

    aet_database();
    ~aet_database();

    void add(aet_database_file* aet_db_file);
    void clear();

    void add_aet_set(uint32_t set_id, uint32_t index);
    void parse(const aet_db_aet_set_file* set_file,
        std::string& set_name, std::vector<uint32_t>& aet_ids);
    void remove_aet_set(uint32_t set_id, uint32_t index,
        const char* set_name, std::vector<uint32_t>& aet_ids);

    const aet_db_aet_set* get_aet_set_by_name(const char* name) const;
    const aet_db_aet_set* get_aet_set_by_id(uint32_t set_id) const;
    const aet_db_aet_set* get_aet_set_by_index(uint32_t index) const;
    const aet_db_aet* get_aet_by_name(const char* name) const;
    const aet_db_aet* get_aet_by_id(uint32_t id) const;
    const aet_db_aet* get_aet_by_set_id_index(uint32_t set_id, uint32_t index) const;
    const char* get_aet_set_file_name(uint32_t set_id) const;
    uint32_t get_aet_set_id_by_name(const char* name) const;
    uint32_t get_aet_set_id_by_name_index(uint32_t index) const;
    const char* get_aet_set_name(uint32_t set_id) const;
    uint32_t get_aet_id_by_name(const char* name) const;
};
