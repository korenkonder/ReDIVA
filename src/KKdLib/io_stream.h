/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "half_t.h"
#include "vector.h"

#define IO_SEEK_SET 0
#define IO_SEEK_CUR 1
#define IO_SEEK_END 2
#define IO_EOF -1

typedef enum stream_type {
    STREAM_FILE   = 0,
    STREAM_MEMORY = 1,
} stream_type;

typedef struct stream {
    uint8_t buf[0x100];
    union io {
        void* stream;
        struct data {
            uint8_t* data;
            vector_uint8_t vec;
        } data;
    } io;
    size_t length;
    stream_type type;
    bool is_big_endian;
} stream;

extern stream* io_open(char* path, char* mode);
extern stream* io_wopen(wchar_t* path, wchar_t* mode);
extern stream* io_open_memory(void* data, size_t length);
extern void io_dispose(stream* s);
extern void io_align(stream* s, size_t align);
extern ssize_t io_get_position(stream* s);
extern int32_t io_set_position(stream* s, ssize_t pos, int32_t seek);
extern size_t io_read(stream* s, void* buf, size_t count);
extern size_t io_write(stream* s, void* buf, size_t count);
extern int32_t io_read_char(stream* s);
extern int32_t io_write_char(stream* s, char c);

int8_t io_read_int8_t(stream* s);
uint8_t io_read_uint8_t(stream* s);
void io_write_int8_t(stream* s, int8_t val);
void io_write_uint8_t(stream* s, uint8_t val);

#define io_read_write(t) \
extern t io_read_##t(stream* s); \
extern t io_read_##t##_stream_reverse_endianess(stream* s); \
extern t io_read_##t##_reverse_endianess(stream* s, bool big_endian); \
extern void io_write_##t(stream* s, t val); \
extern void io_write_##t##_stream_reverse_endianess(stream* s, t val); \
extern void io_write_##t##_reverse_endianess(stream* s, t val, bool big_endian);

io_read_write(int16_t)
io_read_write(uint16_t)
io_read_write(int32_t)
io_read_write(uint32_t)
io_read_write(int64_t)
io_read_write(uint64_t)
io_read_write(float_t)
io_read_write(double_t)
io_read_write(half_t)
#undef io_read_write
