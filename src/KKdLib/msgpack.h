/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "char_buffer.h"

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

typedef struct msgpack {
    msgpack_type type;
    wchar_t_buffer name;
    union {
        char data[0x20];
        void* ptr;
    };
} msgpack;

#define CHECK_MSGPACK(a) sizeof(a) > 0x20
#define SELECT_MSGPACK(a, b) (CHECK_MSGPACK(a) ? (a*)(b)->ptr : (##a##*)(b)->data)

typedef struct msgpack_array {
    msgpack* data;
    size_t length;
    size_t fulllength;
} msgpack_array;

typedef struct msgpack_map {
    msgpack* data;
    size_t length;
    size_t fulllength;
} msgpack_map;

extern msgpack* msgpack_init_map(wchar_t* name);
extern msgpack* msgpack_init_array(wchar_t* name, size_t length);
extern msgpack* msgpack_init_null(wchar_t* name);
extern msgpack* msgpack_init_bool(wchar_t* name, bool val);
extern msgpack* msgpack_init_int8_t(wchar_t* name, int8_t val);
extern msgpack* msgpack_init_uint8_t(wchar_t* name, uint8_t val);
extern msgpack* msgpack_init_int16_t(wchar_t* name, int16_t val);
extern msgpack* msgpack_init_uint16_t(wchar_t* name, uint16_t val);
extern msgpack* msgpack_init_int32_t(wchar_t* name, int32_t val);
extern msgpack* msgpack_init_uint32_t(wchar_t* name, uint32_t val);
extern msgpack* msgpack_init_int64_t(wchar_t* name, int64_t val);
extern msgpack* msgpack_init_uint64_t(wchar_t* name, uint64_t val);
extern msgpack* msgpack_init_float_t(wchar_t* name, float_t val);
extern msgpack* msgpack_init_double_t(wchar_t* name, double_t val);
extern msgpack* msgpack_init_string(wchar_t* name, wchar_t* val);
extern bool msgpack_check_null(msgpack* msg);
extern bool msgpack_check_not_null(msgpack* msg);
extern msgpack* msgpack_get_by_index(msgpack* msg, size_t index);
extern void msgpack_set_by_index(msgpack* msg, msgpack* m, size_t index);
extern msgpack* msgpack_get_by_name(msgpack* msg, wchar_t* name);
extern void msgpack_set_by_name(msgpack* msg, msgpack* m);
extern void msgpack_append(msgpack* msg, msgpack* m);
extern void msgpack_append_bool(msgpack* msg, wchar_t* name, bool val);
extern void msgpack_append_int8_t(msgpack* msg, wchar_t* name, int8_t val);
extern void msgpack_append_uint8_t(msgpack* msg, wchar_t* name, uint8_t val);
extern void msgpack_append_int16_t(msgpack* msg, wchar_t* name, int16_t val);
extern void msgpack_append_uint16_t(msgpack* msg, wchar_t* name, uint16_t val);
extern void msgpack_append_int32_t(msgpack* msg, wchar_t* name, int32_t val);
extern void msgpack_append_uint32_t(msgpack* msg, wchar_t* name, uint32_t val);
extern void msgpack_append_int64_t(msgpack* msg, wchar_t* name, int64_t val);
extern void msgpack_append_uint64_t(msgpack* msg, wchar_t* name, uint64_t val);
extern void msgpack_append_float_t(msgpack* msg, wchar_t* name, float_t val);
extern void msgpack_append_double_t(msgpack* msg, wchar_t* name, double_t val);
extern void msgpack_append_string(msgpack* msg, wchar_t* name, wchar_t* val);
extern void msgpack_set_null(msgpack* msg, wchar_t* name);
extern void msgpack_set_array(msgpack* msg, wchar_t* name, msgpack_array* val);
extern void msgpack_set_map(msgpack* msg, wchar_t* name, msgpack_map* val);
extern void msgpack_set_map_empty(msgpack* msg, wchar_t* name);
extern void msgpack_set_bool(msgpack* msg, wchar_t* name, bool val);
extern void msgpack_set_int8_t(msgpack* msg, wchar_t* name, int8_t val);
extern void msgpack_set_uint8_t(msgpack* msg, wchar_t* name, uint8_t val);
extern void msgpack_set_int16_t(msgpack* msg, wchar_t* name, int16_t val);
extern void msgpack_set_uint16_t(msgpack* msg, wchar_t* name, uint16_t val);
extern void msgpack_set_int32_t(msgpack* msg, wchar_t* name, int32_t val);
extern void msgpack_set_uint32_t(msgpack* msg, wchar_t* name, uint32_t val);
extern void msgpack_set_int64_t(msgpack* msg, wchar_t* name, int64_t val);
extern void msgpack_set_uint64_t(msgpack* msg, wchar_t* name, uint64_t val);
extern void msgpack_set_float_t(msgpack* msg, wchar_t* name, float_t val);
extern void msgpack_set_double_t(msgpack* msg, wchar_t* name, double_t val);
extern void msgpack_set_string(msgpack* msg, wchar_t* name, wchar_t* val);
extern msgpack* msgpack_read(msgpack* msg, wchar_t* name);
extern bool msgpack_read_bool(msgpack* msg, wchar_t* name);
extern int8_t msgpack_read_int8_t(msgpack* msg, wchar_t* name);
extern uint8_t msgpack_read_uint8_t(msgpack* msg, wchar_t* name);
extern int16_t msgpack_read_int16_t(msgpack* msg, wchar_t* name);
extern uint16_t msgpack_read_uint16_t(msgpack* msg, wchar_t* name);
extern int32_t msgpack_read_int32_t(msgpack* msg, wchar_t* name);
extern uint32_t msgpack_read_uint32_t(msgpack* msg, wchar_t* name);
extern int64_t msgpack_read_int64_t(msgpack* msg, wchar_t* name);
extern uint64_t msgpack_read_uint64_t(msgpack* msg, wchar_t* name);
extern float_t msgpack_read_float_t(msgpack* msg, wchar_t* name);
extern double_t msgpack_read_double_t(msgpack* msg, wchar_t* name);
extern wchar_t* msgpack_read_string(msgpack* msg, wchar_t* name);
extern void msgpack_dispose(msgpack* msg);
