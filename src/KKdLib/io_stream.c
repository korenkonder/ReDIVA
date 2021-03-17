/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "io_stream.h"

static void io_get_length(stream* s) {
    if (s->io) {
        size_t temp = _ftelli64(s->io);
        _fseeki64(s->io, 0, IO_SEEK_END);
        s->length = _ftelli64(s->io);
        _fseeki64(s->io, temp, IO_SEEK_SET);
    }
}

stream* io_open(char* path, char* mode) {
    stream* s = force_malloc(sizeof(stream));
    errno_t err = fopen_s((FILE**)&s->io, path, mode);
    io_get_length(s);
    return s;
}

stream* io_wopen(wchar_t* path, wchar_t* mode) {
    stream* s = force_malloc(sizeof(stream));
    errno_t err = _wfopen_s((FILE**)&s->io, path, mode);
    io_get_length(s);
    return s;
}

void io_dispose(stream* s) {
    if (!s)
        return;

    if (s->io)
        switch (s->type) {
        case STREAM_FILE:
            fflush(s->io);
            fclose(s->io);
            break;
        }
    free(s);
}

void io_align(stream* s, size_t align) {
    size_t position;
    switch (s->type) {
    case STREAM_FILE:
        position = _ftelli64(s->io);
        break;
    default:
        return;
    }

    size_t temp_align = align - position % align;
    if (align != temp_align)
        switch (s->type) {
        case STREAM_FILE:
            _fseeki64(s->io, position + temp_align, 0);
            break;
        }
}

ssize_t io_get_position(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return _ftelli64(s->io);
    default:
        return IO_EOF;
    }
}

int32_t io_set_position(stream* s, size_t pos, int32_t seek) {
    switch (s->type) {
    case STREAM_FILE:
        return _fseeki64(s->io, pos, seek);
    default:
        return IO_EOF;
    }
}

size_t io_read(stream* s, void* buf, size_t count) {
    switch (s->type) {
    case STREAM_FILE:
        return fread(buf, 1, count, s->io);
    default:
        return IO_EOF;
    }
}

size_t io_write(stream* s, void* buf, size_t count) {
    switch (s->type) {
    case STREAM_FILE:
        return fwrite(buf, 1, count, s->io);
    default:
        return IO_EOF;
    }
}

int32_t io_read_char(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return fgetc(s->io);
    default:
        return IO_EOF;
    }
}

int32_t io_write_char(stream* s, char c) {
    switch (s->type) {
    case STREAM_FILE:
        return fputc(c, s->io);
    default:
        return IO_EOF;
    }
}

int8_t io_read_int8_t(stream* s) {
    char c = io_read_char(s);
    if (c != IO_EOF)
        return (int8_t)c;
    return 0;
}

uint8_t io_read_uint8_t(stream* s) {
    char c = io_read_char(s);
    if (c != IO_EOF)
        return (uint8_t)c;
    return 0;
}

void io_write_int8_t(stream* s, int8_t val) {
    io_write_char(s, (char)val);
}

void io_write_uint8_t(stream* s, uint8_t val) {
    io_write_char(s, (char)val);
}

#define io_read_write(t) \
t io_read_##t(stream* s) { \
    io_read(s, s->buf, sizeof(t)); \
    return *(t*)s->buf; \
} \
\
t io_read_##t##_stream_reverse_endianess(stream* s) { \
    io_read(s, s->buf, sizeof(t)); \
    t val = *(t*)s->buf; \
    if (s->is_big_endian) \
        reverse_endianess_##t(val); \
    return val; \
}\
\
t io_read_##t##_reverse_endianess(stream* s, bool big_endian) { \
    io_read(s, s->buf, sizeof(t)); \
    t val = *(t*)s->buf; \
    if (big_endian) \
        reverse_endianess_##t(val); \
    return val; \
}\
\
void io_write_##t(stream* s, t val) { \
    *(t*)s->buf = val; \
    io_write(s, s->buf, sizeof(t)); \
} \
\
void io_write_##t##_stream_reverse_endianess(stream* s, t val) { \
    if (s->is_big_endian) \
        reverse_endianess_##t(val); \
    *(t*)s->buf = val; \
    io_write(s, s->buf, sizeof(t)); \
} \
\
void io_write_##t##_reverse_endianess(stream* s, t val, bool big_endian) { \
    if (big_endian) \
        reverse_endianess_##t(val); \
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
#undef io_read_write
