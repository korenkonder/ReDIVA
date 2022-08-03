/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"

enum msgpack_type : uint32_t {
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
};

struct msgpack;

typedef std::vector<msgpack> msgpack_array;

union msgpack_data {
    uint8_t data[0x08];
    union {
        void* ptr;
        std::string* str;
        msgpack_array* arr;
    };
};

struct msgpack {
    msgpack_type type;
    std::string name;
    msgpack_data data;

    msgpack();
    msgpack(const msgpack& m);
    msgpack(const char* name);
    msgpack(const char* name, bool array, size_t length);
    msgpack(const char* name, bool array, msgpack_array& val);
    msgpack(const char* name, bool val);
    msgpack(const char* name, int8_t val);
    msgpack(const char* name, uint8_t val);
    msgpack(const char* name, int16_t val);
    msgpack(const char* name, uint16_t val);
    msgpack(const char* name, int32_t val);
    msgpack(const char* name, uint32_t val);
    msgpack(const char* name, int64_t val);
    msgpack(const char* name, uint64_t val);
    msgpack(const char* name, float_t val);
    msgpack(const char* name, double_t val);
    msgpack(const char* name, const char* val);
    msgpack(const char* name, const wchar_t* val);
    msgpack(const char* name, std::string& val);
    msgpack(const char* name, std::wstring& val);
    ~msgpack();

    bool check_null();
    bool check_not_null();
    msgpack* get_by_index(size_t index);
    void set_by_index(msgpack* m, size_t index);
    msgpack* get_by_name(const char* name);
    void set_by_name(msgpack* m);
    void append(msgpack* m);
    void append(msgpack& m);
    void append(msgpack&& m);
    msgpack* read(const char* name);
    bool read_bool(const char* name = 0);
    int8_t read_int8_t(const char* name = 0);
    uint8_t read_uint8_t(const char* name = 0);
    int16_t read_int16_t(const char* name = 0);
    uint16_t read_uint16_t(const char* name = 0);
    int32_t read_int32_t(const char* name = 0);
    uint32_t read_uint32_t(const char* name = 0);
    int64_t read_int64_t(const char* name = 0);
    uint64_t read_uint64_t(const char* name = 0);
    float_t read_float_t(const char* name = 0);
    double_t read_double_t(const char* name = 0);
    char* read_utf8_string(const char* name = 0);
    wchar_t* read_utf16_string(const char* name = 0);
    std::string read_string(const char* name = 0);
    std::wstring read_wstring(const char* name = 0);

    msgpack& operator=(const msgpack& m);
};

#define MSGPACK_CHECK(a) (sizeof(a) > 0x08)
#define MSGPACK_SELECT(a, b) (MSGPACK_CHECK(a) ? (a*)(b).data.ptr : (##a##*)(b).data.data)
#define MSGPACK_SELECT_PTR(a, b) (MSGPACK_CHECK(a) ? (a*)(b)->data.ptr : (##a##*)(b)->data.data)
