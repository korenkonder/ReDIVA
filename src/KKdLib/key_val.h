/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.h"
#include "io/stream.h"
#include "vec.h"
#include "vector.h"

struct key_val {
    std::vector<char*> key;
    std::vector<size_t> key_len;
    std::vector<char*> val;
    std::vector<size_t> val_len;
    char* buf;
    std::vector<uint64_t> key_hash;
    std::vector<size_t> key_index;

    key_val();
    ~key_val();

    void file_read(const char* path);
    void file_read(const wchar_t* path);
    bool get_local_key_val(const char* str, key_val* lkv);
    bool has_key(const char* str);
    void parse(uint8_t* data, size_t length);
    bool read_bool(char* buf, size_t offset, const char* str_add, size_t str_add_len, bool* value);
    bool read_float_t(char* buf, size_t offset, const char* str_add, size_t str_add_len, float_t* value);
    bool read_int32_t(char* buf, size_t offset, const char* str_add, size_t str_add_len, int32_t* value);
    bool read_uint32_t(char* buf, size_t offset, const char* str_add, size_t str_add_len, uint32_t* value);
    bool read_string(char* buf, size_t offset, const char* str_add, size_t str_add_len, string* value);
    bool read_string(char* buf, size_t offset, const char* str_add, size_t str_add_len, std::string* value);
    bool read_string(char* buf, size_t offset, const char* str_add, size_t str_add_len, const char** value);
    bool read_vec3(char* buf, size_t offset, const char* str_add, size_t str_add_len, vec3* value);

    static void get_lexicographic_order(std::vector<int32_t>* vec, int32_t length);
    static void write_bool(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, bool value);
    static void write_float_t(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, float_t value);
    static void write_int32_t(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, int32_t value);
    static void write_uint32_t(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, uint32_t value);
    static void write_string(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, string& value);
    static void write_string(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, std::string& value);
    static void write_string(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, const char* value);
    static void write_vec3(stream* s, char* buf,
        size_t offset, const char* str_add, size_t str_add_len, vec3& value);
};
