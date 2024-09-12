/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "io/stream.hpp"
#include "prj/vector_pair.hpp"
#include "vec.hpp"

struct key_val_scope {
    std::string key;
    size_t index;
    size_t count;

    prj::vector_pair<uint64_t, size_t> key_hash_index;

    key_val_scope();
    ~key_val_scope();
};

struct key_val {
    key_val_scope* curr_scope;
    std::vector<key_val_scope> scope;
    ssize_t scope_size;
    std::string temp_key;
    prj::vector_pair<uint64_t, size_t> temp_key_hash_index;

    prj::vector_pair<const char*, size_t> key;
    prj::vector_pair<const char*, size_t> val;
    char* buf;

    key_val();
    ~key_val();

    void close_scope();
    void file_read(const char* path);
    void file_read(const wchar_t* path);
    bool has_key(const char* str);
    bool has_key(std::string& str);
    bool open_scope(const char* str);
    bool open_scope(std::string& str);
    bool open_scope_fmt(int32_t i);
    bool open_scope_fmt(uint32_t i);
    bool open_scope_fmt(_In_z_ _Printf_format_string_ const char* const fmt, ...);
    void parse(const void* data, size_t size);
    bool read(bool& value);
    bool read(float_t& value);
    bool read(int32_t& value);
    bool read(uint32_t& value);
    bool read(const char*& value);
    bool read(std::string& value);
    bool read(vec3& value);
    bool read(const char* key, bool& value);
    bool read(std::string& key, bool& value);
    bool read(const char* key, float_t& value);
    bool read(std::string& key, float_t& value);
    bool read(const char* key, int32_t& value);
    bool read(std::string& key, int32_t& value);
    bool read(const char* key, uint32_t& value);
    bool read(std::string& key, uint32_t& value);
    bool read(const char* key, const char*& value);
    bool read(std::string& key, const char*& value);
    bool read(const char* key, std::string& value);
    bool read(std::string& key, std::string& value);
    bool read(const char* key, vec3& value);
    bool read(std::string& key, vec3& value);
    bool read(const char* key0, const char* key1, bool& value);
    bool read(std::string& key0, std::string& key1, bool& value);
    bool read(const char* key0, const char* key1, float_t& value);
    bool read(std::string& key0, std::string& key1, float_t& value);
    bool read(const char* key0, const char* key1, int32_t& value);
    bool read(std::string& key0, std::string& key1, int32_t& value);
    bool read(const char* key0, const char* key1, uint32_t& value);
    bool read(std::string& key0, std::string& key1, uint32_t& value);
    bool read(const char* key0, const char* key1, const char*& value);
    bool read(std::string& key0, std::string& key1, const char*& value);
    bool read(const char* key0, const char* key1, std::string& value);
    bool read(std::string& key0, std::string& key1, std::string& value);
    bool read(const char* key0, const char* key1, vec3& value);
    bool read(std::string& key0, std::string& key1, vec3& value);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

struct key_val_out {
    std::string* curr_scope;
    std::vector<std::string> scope;
    std::string temp_key;

    key_val_out();
    ~key_val_out();

    void close_scope();
    void open_scope(std::string& str);
    void open_scope(const char* str);
    void open_scope_fmt(int32_t i);
    void open_scope_fmt(uint32_t i);
    void write(stream& s, bool value);
    void write(stream& s, float_t value);
    void write(stream& s, int32_t value);
    void write(stream& s, uint32_t value);
    void write(stream& s, const char* value);
    void write(stream& s, const std::string& value);
    void write(stream& s, const vec3& value);
    void write(stream& s, const char* key, bool value);
    void write(stream& s, std::string& key, bool value);
    void write(stream& s, const char* key, float_t value, const char* fmt = "%g");
    void write(stream& s, std::string& key, float_t value, const char* fmt = "%g");
    void write(stream& s, const char* key, int32_t value);
    void write(stream& s, std::string& key, int32_t value);
    void write(stream& s, const char* key, uint32_t value);
    void write(stream& s, std::string& key, uint32_t value);
    void write(stream& s, const char* key, const char* value);
    void write(stream& s, std::string& key, const char* value);
    void write(stream& s, const char* key, const std::string& value);
    void write(stream& s, std::string& key, const std::string& value);
    void write(stream& s, const char* key, const vec3& value);
    void write(stream& s, std::string& key, const vec3& value);

    static void get_lexicographic_order(std::vector<int32_t>& vec, int32_t length);
};
