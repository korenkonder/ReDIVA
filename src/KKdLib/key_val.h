/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "default.h"
#include "io/stream.h"
#include "vec.h"
#include "vector.h"

typedef struct key_val {
    vector_old_ptr_char key;
    vector_old_size_t key_len;
    vector_old_ptr_char val;
    vector_old_size_t val_len;
    char* buf;
    vector_old_uint64_t key_hash;
    vector_old_size_t key_index;
} key_val;

extern void key_val_init(key_val* kv, uint8_t* data, size_t length);
extern void key_val_file_read(key_val* kv, char* path);
extern void key_val_wfile_read(key_val* kv, wchar_t* path);
extern bool key_val_get_local_key_val(key_val* kv, char* str, key_val* lkv);
extern bool key_val_get_local_key_val(key_val* kv, const char* str, key_val* lkv);
extern bool key_val_has_key(key_val* kv, char* str);
extern bool key_val_read_bool(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, bool* value);
extern void key_val_write_bool(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, bool value);
extern bool key_val_read_float_t(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, float_t* value);
extern void key_val_write_float_t(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, float_t value);
extern bool key_val_read_int32_t(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, int32_t* value);
extern void key_val_write_int32_t(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, int32_t value);
extern bool key_val_read_uint32_t(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, uint32_t* value);
extern void key_val_write_uint32_t(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, uint32_t value);
extern bool key_val_read_string(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, string* value);
extern bool key_val_read_string(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, std::string* value);
extern bool key_val_read_string(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, char** value);
extern void key_val_write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, string* value);
extern void key_val_write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, std::string* value);
extern void key_val_write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, char* value);
extern void key_val_write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, const char* value);
extern bool key_val_read_vec3(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, vec3* value);
extern void key_val_write_vec3(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, vec3* value);
extern void key_val_free(key_val* kv);

extern void key_val_get_lexicographic_order(vector_old_int32_t* vec, int32_t length);
