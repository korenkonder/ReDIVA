/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "vector.h"

typedef enum msgpack_type {
    MSGPACK_NONE = 0,
    MSGPACK_NULL,
    MSGPACK_BOOL,
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
} msgpack_type;

#pragma pack(push, 4)
typedef struct msgpack {
    union {
        uint8_t data[0x1C];
        void* ptr;
    };
    msgpack_type type;
    string name;
} msgpack;
#pragma pack(pop)

#define MSGPACK_CHECK(a) sizeof(a) > 0x1C
#define MSGPACK_SELECT(a, b) (MSGPACK_CHECK(a) ? (a*)(b).ptr : (##a##*)(b).data)
#define MSGPACK_SELECT_PTR(a, b) (MSGPACK_CHECK(a) ? (a*)(b)->ptr : (##a##*)(b)->data)

vector(msgpack)

typedef vector_msgpack msgpack_array;
typedef vector_msgpack msgpack_map;

extern void msgpack_init_map(msgpack* msg, char* name);
extern void msgpack_init_array(msgpack* msg, char* name, size_t length);
extern void msgpack_init_null(msgpack* msg, char* name);
extern void msgpack_init_bool(msgpack* msg, char* name, bool val);
extern void msgpack_init_int8_t(msgpack* msg, char* name, int8_t val);
extern void msgpack_init_uint8_t(msgpack* msg, char* name, uint8_t val);
extern void msgpack_init_int16_t(msgpack* msg, char* name, int16_t val);
extern void msgpack_init_uint16_t(msgpack* msg, char* name, uint16_t val);
extern void msgpack_init_int32_t(msgpack* msg, char* name, int32_t val);
extern void msgpack_init_uint32_t(msgpack* msg, char* name, uint32_t val);
extern void msgpack_init_int64_t(msgpack* msg, char* name, int64_t val);
extern void msgpack_init_uint64_t(msgpack* msg, char* name, uint64_t val);
extern void msgpack_init_float_t(msgpack* msg, char* name, float_t val);
extern void msgpack_init_double_t(msgpack* msg, char* name, double_t val);
extern void msgpack_init_utf8_string(msgpack* msg, char* name, char* val);
extern void msgpack_init_utf16_string(msgpack* msg, char* name, wchar_t* val);
extern void msgpack_init_string(msgpack* msg, char* name, string* val);
extern void msgpack_init_wstring(msgpack* msg, char* name, wstring* val);
extern bool msgpack_check_null(msgpack* msg);
extern bool msgpack_check_not_null(msgpack* msg);
extern msgpack* msgpack_get_by_index(msgpack* msg, size_t index);
extern void msgpack_set_by_index(msgpack* msg, msgpack* m, size_t index);
extern msgpack* msgpack_get_by_name(msgpack* msg, char* name);
extern void msgpack_set_by_name(msgpack* msg, msgpack* m);
extern void msgpack_append(msgpack* msg, msgpack* m);
extern void msgpack_append_bool(msgpack* msg, char* name, bool val);
extern void msgpack_append_int8_t(msgpack* msg, char* name, int8_t val);
extern void msgpack_append_uint8_t(msgpack* msg, char* name, uint8_t val);
extern void msgpack_append_int16_t(msgpack* msg, char* name, int16_t val);
extern void msgpack_append_uint16_t(msgpack* msg, char* name, uint16_t val);
extern void msgpack_append_int32_t(msgpack* msg, char* name, int32_t val);
extern void msgpack_append_uint32_t(msgpack* msg, char* name, uint32_t val);
extern void msgpack_append_int64_t(msgpack* msg, char* name, int64_t val);
extern void msgpack_append_uint64_t(msgpack* msg, char* name, uint64_t val);
extern void msgpack_append_float_t(msgpack* msg, char* name, float_t val);
extern void msgpack_append_double_t(msgpack* msg, char* name, double_t val);
extern void msgpack_append_utf8_string(msgpack* msg, char* name, char* val);
extern void msgpack_append_utf16_string(msgpack* msg, char* name, wchar_t* val);
extern void msgpack_append_string(msgpack* msg, char* name, string* val);
extern void msgpack_append_wstring(msgpack* msg, char* name, wstring* val);
extern void msgpack_set_null(msgpack* msg, char* name);
extern void msgpack_set_array(msgpack* msg, char* name, msgpack_array* val);
extern void msgpack_set_array_empty(msgpack* msg, char* name);
extern void msgpack_set_map(msgpack* msg, char* name, msgpack_map* val);
extern void msgpack_set_map_empty(msgpack* msg, char* name);
extern void msgpack_set_bool(msgpack* msg, char* name, bool val);
extern void msgpack_set_int8_t(msgpack* msg, char* name, int8_t val);
extern void msgpack_set_uint8_t(msgpack* msg, char* name, uint8_t val);
extern void msgpack_set_int16_t(msgpack* msg, char* name, int16_t val);
extern void msgpack_set_uint16_t(msgpack* msg, char* name, uint16_t val);
extern void msgpack_set_int32_t(msgpack* msg, char* name, int32_t val);
extern void msgpack_set_uint32_t(msgpack* msg, char* name, uint32_t val);
extern void msgpack_set_int64_t(msgpack* msg, char* name, int64_t val);
extern void msgpack_set_uint64_t(msgpack* msg, char* name, uint64_t val);
extern void msgpack_set_float_t(msgpack* msg, char* name, float_t val);
extern void msgpack_set_double_t(msgpack* msg, char* name, double_t val);
extern void msgpack_set_utf8_string(msgpack* msg, char* name, char* val);
extern void msgpack_set_utf16_string(msgpack* msg, char* name, wchar_t* val);
extern void msgpack_set_string(msgpack* msg, char* name, string* val);
extern void msgpack_set_wstring(msgpack* msg, char* name, wstring* val);
extern msgpack* msgpack_read(msgpack* msg, char* name);
extern bool msgpack_read_bool(msgpack* msg, char* name);
extern int8_t msgpack_read_int8_t(msgpack* msg, char* name);
extern uint8_t msgpack_read_uint8_t(msgpack* msg, char* name);
extern int16_t msgpack_read_int16_t(msgpack* msg, char* name);
extern uint16_t msgpack_read_uint16_t(msgpack* msg, char* name);
extern int32_t msgpack_read_int32_t(msgpack* msg, char* name);
extern uint32_t msgpack_read_uint32_t(msgpack* msg, char* name);
extern int64_t msgpack_read_int64_t(msgpack* msg, char* name);
extern uint64_t msgpack_read_uint64_t(msgpack* msg, char* name);
extern float_t msgpack_read_float_t(msgpack* msg, char* name);
extern double_t msgpack_read_double_t(msgpack* msg, char* name);
extern char* msgpack_read_utf8_string(msgpack* msg, char* name);
extern wchar_t* msgpack_read_utf16_string(msgpack* msg, char* name);
extern void msgpack_read_string(msgpack* msg, char* name, string* str);
extern void msgpack_read_wstring(msgpack* msg, char* name, wstring* str);
extern void msgpack_free(msgpack* msg);
