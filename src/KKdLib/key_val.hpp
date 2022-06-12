/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "io/stream.hpp"
#include "vec.hpp"

struct key_val_hash_index_pair {
    uint64_t hash;
    size_t index;

    key_val_hash_index_pair();
    key_val_hash_index_pair(uint64_t hash, size_t index);
};

struct key_val_scope {
    std::string key;
    size_t index;
    size_t count;

    std::vector<key_val_hash_index_pair> key_hash_index;

    key_val_scope();
    ~key_val_scope();
};

struct key_val_pair {
    const char* str;
    size_t length;

    key_val_pair();
    key_val_pair(const char* str, size_t length);
    ~key_val_pair();
};

struct key_val {
    key_val_scope* curr_scope;
    std::vector<key_val_scope> scope;

    std::vector<key_val_pair> key;
    std::vector<key_val_pair> val;
    char* buf;

    key_val();
    ~key_val();

    void close_scope();
    void file_read(const char* path);
    void file_read(const wchar_t* path);
    bool has_key(const char* str);
    bool has_key(std::string& str);
    bool open_scope(std::string& str);
    bool open_scope(std::string&& str);
    bool open_scope(int32_t i);
    bool open_scope(uint32_t i);
    bool open_scope(const char* fmt, ...);
    void parse(const void* data, size_t size);
    bool read(bool& value);
    bool read(float_t& value);
    bool read(int32_t& value);
    bool read(uint32_t& value);
    bool read(const char*& value);
    bool read(std::string& value);
    bool read(vec3& value);
    bool read(std::string& key, bool& value);
    bool read(std::string&& key, bool& value);
    bool read(std::string& key, float_t& value);
    bool read(std::string&& key, float_t& value);
    bool read(std::string& key, int32_t& value);
    bool read(std::string&& key, int32_t& value);
    bool read(std::string& key, uint32_t& value);
    bool read(std::string&& key, uint32_t& value);
    bool read(std::string& key, const char*& value);
    bool read(std::string&& key, const char*& value);
    bool read(std::string& key, std::string& value);
    bool read(std::string&& key, std::string& value);
    bool read(std::string& key, vec3& value);
    bool read(std::string&& key, vec3& value);
    bool read(std::string& key0, std::string& key1, bool& value);
    bool read(std::string&& key0, std::string&& key1, bool& value);
    bool read(std::string& key0, std::string& key1, float_t& value);
    bool read(std::string&& key0, std::string&& key1, float_t& value);
    bool read(std::string& key0, std::string& key1, int32_t& value);
    bool read(std::string&& key0, std::string&& key1, int32_t& value);
    bool read(std::string& key0, std::string& key1, uint32_t& value);
    bool read(std::string&& key0, std::string&& key1, uint32_t& value);
    bool read(std::string& key0, std::string& key1, const char*& value);
    bool read(std::string&& key0, std::string&& key1, const char*& value);
    bool read(std::string& key0, std::string& key1, std::string& value);
    bool read(std::string&& key0, std::string&& key1, std::string& value);
    bool read(std::string& key0, std::string& key1, vec3& value);
    bool read(std::string&& key0, std::string&& key1, vec3& value);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct key_val_out {
    std::string* curr_scope;
    std::vector<std::string> scope;

    key_val_out();
    ~key_val_out();

    void close_scope();
    void open_scope(std::string& str);
    void open_scope(std::string&& str);
    void open_scope(int32_t i);
    void open_scope(uint32_t i);
    void write(stream& s, bool value);
    void write(stream& s, float_t value);
    void write(stream& s, int32_t value);
    void write(stream& s, uint32_t value);
    void write(stream& s, const char* value);
    void write(stream& s, std::string& value);
    void write(stream& s, vec3& value);
    void write(stream& s, vec3&& value);
    void write(stream& s, std::string& key, bool value);
    void write(stream& s, std::string&& key, bool value);
    void write(stream& s, std::string& key, float_t value);
    void write(stream& s, std::string&& key, float_t value);
    void write(stream& s, std::string& key, int32_t value);
    void write(stream& s, std::string&& key, int32_t value);
    void write(stream& s, std::string& key, uint32_t value);
    void write(stream& s, std::string&& key, uint32_t value);
    void write(stream& s, std::string& key, const char* value);
    void write(stream& s, std::string&& key, const char* value);
    void write(stream& s, std::string& key, std::string& value);
    void write(stream& s, std::string&& key, std::string& value);
    void write(stream& s, std::string& key, vec3& value);
    void write(stream& s, std::string&& key, vec3& value);
    void write(stream& s, std::string& key, vec3&& value);
    void write(stream& s, std::string&& key, vec3&& value);

    static void get_lexicographic_order(std::vector<int32_t>* vec, int32_t length);
};
