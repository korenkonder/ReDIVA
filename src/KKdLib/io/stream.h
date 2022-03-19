/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../default.h"
#include "../string.h"
#include "../half_t.h"
#include "../vector.h"

typedef enum stream_type {
    STREAM_NONE = 0,
    STREAM_FILE,
    STREAM_MEMORY,
} stream_type;

typedef struct stream {
    uint8_t buf[0x100];
    union io {
        FILE* stream;
        struct data {
            uint8_t* data;
            vector_old_uint8_t vec;
        } data;
    } io;
    ssize_t length;
    stream_type type;
    bool is_big_endian;
    vector_old_ssize_t position_stack;
} stream;

extern void io_open(stream* s);
extern void io_open(stream* s, const char* path, const char* mode);
extern void io_open(stream* s, const wchar_t* path, const wchar_t* mode);
extern void io_open(stream* s, const void* data, size_t length);
extern void io_copy(stream* s, void** data, size_t* length);
extern void io_align_read(stream* s, ssize_t align);
extern void io_align_write(stream* s, ssize_t align);
extern int io_flush(stream* s);
extern ssize_t io_get_length(stream* s);
extern ssize_t io_get_position(stream* s);
extern int32_t io_set_position(stream* s, ssize_t pos, int32_t seek);
extern int32_t io_position_push(stream* s, ssize_t pos, int32_t seek);
extern void io_position_pop(stream* s);
extern ssize_t io_read(stream* s, ssize_t count);
extern ssize_t io_read(stream* s, void* buf, ssize_t count);
extern ssize_t io_write(stream* s, ssize_t count);
extern ssize_t io_write(stream* s, void* buf, ssize_t count);
extern ssize_t io_write(stream* s, const void* buf, ssize_t count);
extern int32_t io_read_char(stream* s);
extern int32_t io_write_char(stream* s, char c);

extern int8_t io_read_int8_t(stream* s);
extern uint8_t io_read_uint8_t(stream* s);
extern void io_write_int8_t(stream* s, int8_t val);
extern void io_write_uint8_t(stream* s, uint8_t val);

extern void io_read_string(stream* s, string* str, size_t length);
extern void io_read_string(stream* s, std::string* str, size_t length);
extern void io_read_wstring(stream* s, wstring* str, size_t length);
extern void io_read_wstring(stream* s, std::wstring* str, size_t length);
extern void io_read_string_null_terminated(stream* s, string* str);
extern void io_read_string_null_terminated(stream* s, std::string* str);
extern void io_read_wstring_null_terminated(stream* s, wstring* str);
extern void io_read_wstring_null_terminated(stream* s, std::wstring* str);
extern void io_read_string_null_terminated_offset(stream* s, ssize_t offset, string* str);
extern void io_read_string_null_terminated_offset(stream* s, ssize_t offset, std::string* str);
extern void io_read_wstring_null_terminated_offset(stream* s, ssize_t offset, wstring* str);
extern void io_read_wstring_null_terminated_offset(stream* s, ssize_t offset, std::wstring* str);
extern char* io_read_utf8_string_null_terminated(stream* s);
extern wchar_t* io_read_utf16_string_null_terminated(stream* s);
extern char* io_read_utf8_string_null_terminated_length(stream* s, ssize_t* length);
extern wchar_t* io_read_utf16_string_null_terminated_length(stream* s, ssize_t* length);
extern char* io_read_utf8_string_null_terminated_offset(stream* s, ssize_t offset);
extern wchar_t* io_read_utf16_string_null_terminated_offset(stream* s, ssize_t offset);
extern char* io_read_utf8_string_null_terminated_offset_length(stream* s, ssize_t offset, ssize_t* length);
extern wchar_t* io_read_utf16_string_null_terminated_offset_length(stream* s, ssize_t offset, ssize_t* length);
extern void io_write_string(stream* s, string* str);
extern void io_write_string(stream* s, std::string* str);
extern void io_write_wstring(stream* s, wstring* str);
extern void io_write_wstring(stream* s, std::wstring* str);
extern void io_write_string_null_terminated(stream* s, string* str);
extern void io_write_string_null_terminated(stream* s, std::string* str);
extern void io_write_wstring_null_terminated(stream* s, wstring* str);
extern void io_write_wstring_null_terminated(stream* s, std::wstring* str);
extern void io_write_utf8_string(stream* s, char* str);
extern void io_write_utf8_string(stream* s, const char* str);
extern void io_write_utf16_string(stream* s, wchar_t* str);
extern void io_write_utf16_string(stream* s, const wchar_t* str);
extern void io_write_utf8_string_null_terminated(stream* s, char* str);
extern void io_write_utf8_string_null_terminated(stream* s, const char* str);
extern void io_write_utf16_string_null_terminated(stream* s, wchar_t* str);
extern void io_write_utf16_string_null_terminated(stream* s, const wchar_t* str);
extern ssize_t io_read_offset(stream* s, int32_t offset, bool is_x);
extern ssize_t io_read_offset_f2(stream* s, int32_t offset);
extern ssize_t io_read_offset_x(stream* s);
extern void io_write_offset(stream* s, ssize_t val, int32_t offset, bool is_x);
extern void io_write_offset_f2(stream* s, ssize_t val, int32_t offset);
extern void io_write_offset_x(stream* s, ssize_t val);
extern void io_free(stream* s);

#define io_read_write(t) \
extern t io_read_##t(stream* s); \
extern t io_read_##t##_stream_reverse_endianness(stream* s); \
extern t io_read_##t##_reverse_endianness(stream* s, bool big_endian); \
extern void io_write_##t(stream* s, t val); \
extern void io_write_##t##_stream_reverse_endianness(stream* s, t val); \
extern void io_write_##t##_reverse_endianness(stream* s, t val, bool big_endian);

#define io_read_write_func(t) \
t io_read_##t(stream* s) { \
    io_read(s, s->buf, sizeof(t)); \
    return *(t*)s->buf; \
} \
\
t io_read_##t##_stream_reverse_endianness(stream* s) { \
    io_read(s, s->buf, sizeof(t)); \
    t val; \
    if (s->is_big_endian) \
        val = load_reverse_endianness_##t(s->buf); \
    else \
        val = *(t*)s->buf; \
    return val; \
}\
\
t io_read_##t##_reverse_endianness(stream* s, bool big_endian) { \
    io_read(s, s->buf, sizeof(t)); \
    t val; \
    if (big_endian) \
        val = load_reverse_endianness_##t(s->buf); \
    else \
        val = *(t*)s->buf; \
    return val; \
}\
\
void io_write_##t(stream* s, t val) { \
    *(t*)s->buf = val; \
    io_write(s, s->buf, sizeof(t)); \
} \
\
void io_write_##t##_stream_reverse_endianness(stream* s, t val) { \
    if (s->is_big_endian) \
        store_reverse_endianness_##t(val, s->buf); \
    else \
        *(t*)s->buf = val; \
    io_write(s, s->buf, sizeof(t)); \
} \
\
void io_write_##t##_reverse_endianness(stream* s, t val, bool big_endian) { \
    if (big_endian) \
        store_reverse_endianness_##t(val, s->buf); \
    else \
        *(t*)s->buf = val; \
    io_write(s, s->buf, sizeof(t)); \
}

io_read_write(int16_t)
io_read_write(uint16_t)
io_read_write(int32_t)
io_read_write(uint32_t)
io_read_write(int64_t)
io_read_write(uint64_t)
io_read_write(float_t)
io_read_write(double_t)
io_read_write(half_t)
