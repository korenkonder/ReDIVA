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

struct spr_info {
    uint16_t index;
    uint16_t set_index;

    inline spr_info() {
        this->index = (uint16_t)-1;
        this->set_index = (uint16_t)-1;
    }

    inline spr_info(uint16_t index, uint16_t set_index) {
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

constexpr bool operator==(const spr_info& left, const spr_info& right) {
    return *(uint32_t*)&left == *(uint32_t*)&right;
}

constexpr bool operator!=(const spr_info& left, const spr_info& right) {
    return !(left == right);
}

constexpr bool operator<(const spr_info& left, const spr_info& right) {
    return *(uint32_t*)&left < *(uint32_t*)&right;
}

constexpr bool operator>(const spr_info& left, const spr_info& right) {
    return right < left;
}

constexpr bool operator<=(const spr_info& left, const spr_info& right) {
    return !(right < left);
}

constexpr bool operator>=(const spr_info& left, const spr_info& right) {
    return !(left < right);
}

struct spr_db_spr_file {
    uint32_t id;
    std::string name;
    uint16_t index;
    bool texture;

    spr_db_spr_file();
    ~spr_db_spr_file();
};

struct spr_db_spr {
    uint32_t id;
    std::string name;
    spr_info info;
    int32_t load_count;

    spr_db_spr();
    ~spr_db_spr();
};

struct spr_db_spr_set_file {
    uint32_t id;
    std::string name;
    std::string file_name;
    std::vector<spr_db_spr_file> sprite;
    uint32_t index;

    spr_db_spr_set_file();
    ~spr_db_spr_set_file();
};

struct spr_db_spr_set {
    uint32_t id;
    std::string name;
    std::string file_name;
    uint32_t index;

    spr_db_spr_set();
    ~spr_db_spr_set();
};

struct sprite_database_file {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<spr_db_spr_set_file> sprite_set;

    sprite_database_file();
    ~sprite_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

struct sprite_database {
    std::list<spr_db_spr_set> spr_sets;
    prj::vector_pair_combine<uint32_t, spr_db_spr_set*> spr_set_ids;
    prj::vector_pair_combine<uint32_t, spr_db_spr_set*> spr_set_indices;
    prj::vector_pair_combine<std::string, spr_db_spr_set*> spr_set_names;
    std::list<spr_db_spr> sprs;
    prj::vector_pair_combine<uint32_t, spr_db_spr*> spr_ids;
    prj::vector_pair_combine<std::string, spr_db_spr*> spr_names;
    prj::vector_pair_combine<spr_info, spr_db_spr*> spr_indices;

    sprite_database();
    ~sprite_database();

    void add(sprite_database_file* spr_db_file);
    void clear();

    void add_spr_set(uint32_t set_id, uint32_t index);
    void parse(const spr_db_spr_set_file* set_file,
        std::string& set_name, std::vector<uint32_t>& sprite_ids);
    void remove_spr_set(uint32_t set_id, uint32_t index,
        const char* set_name, std::vector<uint32_t>& sprite_ids);

    const spr_db_spr_set* get_spr_set_by_name(const char* name) const;
    const spr_db_spr_set* get_spr_set_by_id(uint32_t set_id) const;
    const spr_db_spr_set* get_spr_set_by_index(uint32_t index) const;
    const spr_db_spr* get_spr_by_name(const char* name) const;
    const spr_db_spr* get_spr_by_id(uint32_t id) const;
    const spr_db_spr* get_spr_by_set_id_index(uint32_t set_id, uint32_t index) const;
    const spr_db_spr* get_tex_by_set_id_index(uint32_t set_id, uint32_t index) const;
    uint32_t get_spr_id_by_name(const char* name) const;
    const char* get_spr_set_file_name(uint32_t set_id) const;
    uint32_t get_spr_set_id_by_name(const char* name) const;
    uint32_t get_spr_set_id_by_name_index(uint32_t index) const;
    const char* get_spr_set_name(uint32_t set_id) const;
};
