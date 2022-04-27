/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "vector.h"

enum msgpack_type {
    MSGPACK_NONE = 0,
    MSGPACK_NULL,
    MSGPACK_bool,
    MSGPACK_INT8,
    MSGPACK_UINT8,
    MSGPACK_INT16,
    MSGPACK_UINT16,
    MSGPACK_INT32,
    MSGPACK_UINT32,
    MSGPACK_INT64,
    MSGPACK_UINT64,
    MSGPACK_FLOAT,
    MSGPACK_DOUBLE,
    MSGPACK_STRING,
    MSGPACK_ARRAY,
    MSGPACK_MAP,
};

#pragma pack(push, 4)
struct msgpack {
    union {
        uint8_t data[0x1C];
        void* ptr;
    };
    msgpack_type type;
    string name;
};
#pragma pack(pop)

#define MSGPACK_CHECK(a) sizeof(a) > 0x1C
#define MSGPACK_SELECT(a, b) (MSGPACK_CHECK(a) ? (a*)(b).ptr : (##a##*)(b).data)
#define MSGPACK_SELECT_PTR(a, b) (MSGPACK_CHECK(a) ? (a*)(b)->ptr : (##a##*)(b)->data)

vector_old(msgpack)

typedef vector_old_msgpack msgpack_array;
typedef vector_old_msgpack msgpack_map;

extern void msgpack_init_map(msgpack* msg, const char* name);
extern void msgpack_init_array(msgpack* msg, const char* name, size_t length);
extern void msgpack_init_null(msgpack* msg, const char* name);
extern void msgpack_init_bool(msgpack* msg, const char* name, bool val);
extern void msgpack_init_int8_t(msgpack* msg, const char* name, int8_t val);
extern void msgpack_init_uint8_t(msgpack* msg, const char* name, uint8_t val);
extern void msgpack_init_int16_t(msgpack* msg, const char* name, int16_t val);
extern void msgpack_init_uint16_t(msgpack* msg, const char* name, uint16_t val);
extern void msgpack_init_int32_t(msgpack* msg, const char* name, int32_t val);
extern void msgpack_init_uint32_t(msgpack* msg, const char* name, uint32_t val);
extern void msgpack_init_int64_t(msgpack* msg, const char* name, int64_t val);
extern void msgpack_init_uint64_t(msgpack* msg, const char* name, uint64_t val);
extern void msgpack_init_float_t(msgpack* msg, const char* name, float_t val);
extern void msgpack_init_double_t(msgpack* msg, const char* name, double_t val);
extern void msgpack_init_utf8_string(msgpack* msg, const char* name, const char* val);
extern void msgpack_init_utf16_string(msgpack* msg, const char* name, const wchar_t* val);
extern void msgpack_init_string(msgpack* msg, const char* name, string* val);
extern void msgpack_init_wstring(msgpack* msg, const char* name, wstring* val);
extern bool msgpack_check_null(msgpack* msg);
extern bool msgpack_check_not_null(msgpack* msg);
extern msgpack* msgpack_get_by_index(msgpack* msg, size_t index);
extern void msgpack_set_by_index(msgpack* msg, msgpack* m, size_t index);
extern msgpack* msgpack_get_by_name(msgpack* msg, const char* name);
extern void msgpack_set_by_name(msgpack* msg, msgpack* m);
extern void msgpack_append(msgpack* msg, msgpack* m);
extern void msgpack_append_bool(msgpack* msg, const char* name, bool val);
extern void msgpack_append_int8_t(msgpack* msg, const char* name, int8_t val);
extern void msgpack_append_uint8_t(msgpack* msg, const char* name, uint8_t val);
extern void msgpack_append_int16_t(msgpack* msg, const char* name, int16_t val);
extern void msgpack_append_uint16_t(msgpack* msg, const char* name, uint16_t val);
extern void msgpack_append_int32_t(msgpack* msg, const char* name, int32_t val);
extern void msgpack_append_uint32_t(msgpack* msg, const char* name, uint32_t val);
extern void msgpack_append_int64_t(msgpack* msg, const char* name, int64_t val);
extern void msgpack_append_uint64_t(msgpack* msg, const char* name, uint64_t val);
extern void msgpack_append_float_t(msgpack* msg, const char* name, float_t val);
extern void msgpack_append_double_t(msgpack* msg, const char* name, double_t val);
extern void msgpack_append_utf8_string(msgpack* msg, const char* name, const char* val);
extern void msgpack_append_utf16_string(msgpack* msg, const char* name, const wchar_t* val);
extern void msgpack_append_string(msgpack* msg, const char* name, string* val);
extern void msgpack_append_wstring(msgpack* msg, const char* name, wstring* val);
extern void msgpack_set_null(msgpack* msg, const char* name);
extern void msgpack_set_array(msgpack* msg, const char* name, msgpack_array* val);
extern void msgpack_set_array_empty(msgpack* msg, const char* name);
extern void msgpack_set_map(msgpack* msg, const char* name, msgpack_map* val);
extern void msgpack_set_map_empty(msgpack* msg, const char* name);
extern void msgpack_set_bool(msgpack* msg, const char* name, bool val);
extern void msgpack_set_int8_t(msgpack* msg, const char* name, int8_t val);
extern void msgpack_set_uint8_t(msgpack* msg, const char* name, uint8_t val);
extern void msgpack_set_int16_t(msgpack* msg, const char* name, int16_t val);
extern void msgpack_set_uint16_t(msgpack* msg, const char* name, uint16_t val);
extern void msgpack_set_int32_t(msgpack* msg, const char* name, int32_t val);
extern void msgpack_set_uint32_t(msgpack* msg, const char* name, uint32_t val);
extern void msgpack_set_int64_t(msgpack* msg, const char* name, int64_t val);
extern void msgpack_set_uint64_t(msgpack* msg, const char* name, uint64_t val);
extern void msgpack_set_float_t(msgpack* msg, const char* name, float_t val);
extern void msgpack_set_double_t(msgpack* msg, const char* name, double_t val);
extern void msgpack_set_utf8_string(msgpack* msg, const char* name, const char* val);
extern void msgpack_set_utf16_string(msgpack* msg, const char* name, const wchar_t* val);
extern void msgpack_set_string(msgpack* msg, const char* name, string* val);
extern void msgpack_set_wstring(msgpack* msg, const char* name, wstring* val);
extern msgpack* msgpack_read(msgpack* msg, const char* name);
extern bool msgpack_read_bool(msgpack* msg, const char* name);
extern int8_t msgpack_read_int8_t(msgpack* msg, const char* name);
extern uint8_t msgpack_read_uint8_t(msgpack* msg, const char* name);
extern int16_t msgpack_read_int16_t(msgpack* msg, const char* name);
extern uint16_t msgpack_read_uint16_t(msgpack* msg, const char* name);
extern int32_t msgpack_read_int32_t(msgpack* msg, const char* name);
extern uint32_t msgpack_read_uint32_t(msgpack* msg, const char* name);
extern int64_t msgpack_read_int64_t(msgpack* msg, const char* name);
extern uint64_t msgpack_read_uint64_t(msgpack* msg, const char* name);
extern float_t msgpack_read_float_t(msgpack* msg, const char* name);
extern double_t msgpack_read_double_t(msgpack* msg, const char* name);
extern char* msgpack_read_utf8_string(msgpack* msg, const char* name);
extern wchar_t* msgpack_read_utf16_string(msgpack* msg, const char* name);
extern void msgpack_read_string(msgpack* msg, const char* name, string* str);
extern void msgpack_read_wstring(msgpack* msg, const char* name, wstring* str);
extern void msgpack_free(msgpack* msg);
