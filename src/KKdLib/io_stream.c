/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "io_stream.h"

static void io_get_length(stream* s) {
    if (s->io.stream) {
        size_t temp = _ftelli64(s->io.stream);
        _fseeki64(s->io.stream, 0, IO_SEEK_END);
        s->length = _ftelli64(s->io.stream);
        _fseeki64(s->io.stream, temp, IO_SEEK_SET);
    }
    else
        s->length = 0;
}

stream* io_open(char* path, char* mode) {
    stream* s = force_malloc(sizeof(stream));
    errno_t err = fopen_s((FILE**)&s->io.stream, path, mode);
    s->type = STREAM_FILE;
    io_get_length(s);
    return s;
}

stream* io_wopen(wchar_t* path, wchar_t* mode) {
    stream* s = force_malloc(sizeof(stream));
    errno_t err = _wfopen_s((FILE**)&s->io.stream, path, mode);
    s->type = STREAM_FILE;
    io_get_length(s);
    return s;
}

stream* io_open_memory(void* data, size_t length) {
    stream* s = force_malloc(sizeof(stream));
    vector_uint8_t_append(&s->io.data.vec, length);
    if (s->io.data.vec.begin && data)
        memcpy(s->io.data.vec.begin, data, length);
    s->io.data.data = s->io.data.vec.begin;
    s->io.data.vec.end = s->io.data.vec.begin + length;
    s->type = STREAM_MEMORY;
    s->length = length;
    return s;
}

void io_align(stream* s, size_t align) {
    size_t position;
    switch (s->type) {
    case STREAM_FILE:
        position = _ftelli64(s->io.stream);
        break;
    case STREAM_MEMORY:
        position = s->io.data.data - s->io.data.vec.begin;
        break;
    default:
        return;
    }

    size_t capacity;
    size_t temp_align = align - position % align;
    if (align != temp_align)
        switch (s->type) {
        case STREAM_FILE:
            _fseeki64(s->io.stream, position + temp_align, 0);
            break;
        case STREAM_MEMORY:
            capacity = s->io.data.vec.end - s->io.data.data;
            if (capacity < temp_align) {
                vector_uint8_t_append(&s->io.data.vec, temp_align);
                memset(s->io.data.vec.begin + position, 0, temp_align);
            }
            s->io.data.data = s->io.data.vec.begin + position + temp_align;
            if (s->io.data.vec.end < s->io.data.data)
                s->io.data.vec.end = s->io.data.data;
            break;
        }
}

int io_flush(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return fflush(s->io.stream);
    case STREAM_MEMORY:
        return 0;
    default:
        return 0;
    }
}

ssize_t io_get_position(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return _ftelli64(s->io.stream);
    case STREAM_MEMORY:
        return s->io.data.data - s->io.data.vec.begin;
    default:
        return IO_EOF;
    }
}

int32_t io_set_position(stream* s, ssize_t pos, int32_t seek) {
    ssize_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        return _fseeki64(s->io.stream, pos, seek);
    case STREAM_MEMORY:
        switch (seek) {
        case SEEK_SET:
            if (pos < 0)
                return IO_EOF;

            capacity = s->io.data.vec.capacity_end - s->io.data.vec.begin;
            if (capacity < pos)
                vector_uint8_t_append(&s->io.data.vec, pos - capacity);

            s->io.data.data = s->io.data.vec.begin + pos;
            if (s->io.data.data > s->io.data.vec.end) {
                capacity = s->io.data.data - s->io.data.vec.end;
                memset(s->io.data.vec.end, 0, capacity);
                s->io.data.vec.end += capacity;
            }
            return 0;
        case SEEK_CUR:
            if (pos > 0) {
                capacity = s->io.data.vec.capacity_end - s->io.data.data;
                if (capacity < pos) {
                    vector_uint8_t_append(&s->io.data.vec, pos - capacity);
                    s->io.data.vec.end = s->io.data.vec.capacity_end;
                    s->io.data.data = s->io.data.vec.capacity_end;
                }
                else
                    s->io.data.data += pos;
            }
            else if (pos < 0) {
                capacity = s->io.data.data - s->io.data.vec.begin;
                if (capacity < -pos)
                    return IO_EOF;
                else
                    s->io.data.data += pos;
            }
            return 0;
        case SEEK_END:
            if (pos < 0)
                return IO_EOF;

            capacity = s->io.data.vec.end - s->io.data.vec.begin;
            if (capacity < pos)
                return IO_EOF;

            s->io.data.data = s->io.data.vec.end - pos;
            return 0;
        }
        return IO_EOF;
    default:
        return IO_EOF;
    }
}

size_t io_read(stream* s, void* buf, size_t count) {
    size_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        return fread(buf, 1, count, s->io.stream);
    case STREAM_MEMORY:
        if (s->io.data.data >= s->io.data.vec.end)
            return IO_EOF;

        capacity = s->io.data.vec.end - s->io.data.data;
        if (capacity >= count)
            capacity = count;
        memcpy(buf, s->io.data.data, capacity);
        s->io.data.data += capacity;
        return capacity;
    default:
        return IO_EOF;
    }
}

size_t io_write(stream* s, void* buf, size_t count) {
    size_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        return fwrite(buf, 1, count, s->io.stream);
    case STREAM_MEMORY:
        capacity = s->io.data.vec.capacity_end - s->io.data.data;
        if (capacity < count) {
            size_t pos = s->io.data.data - s->io.data.vec.begin;
            vector_uint8_t_append(&s->io.data.vec, count + capacity);
            s->io.data.data = s->io.data.vec.begin + pos;
        }
        memcpy(s->io.data.data, buf, count);
        s->io.data.data += count;
        if (s->io.data.vec.end < s->io.data.data)
            s->io.data.vec.end = s->io.data.data;
        return count;
    default:
        return IO_EOF;
    }
}

int32_t io_read_char(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return fgetc(s->io.stream);
    case STREAM_MEMORY:
        if (s->io.data.data >= s->io.data.vec.end)
            return IO_EOF;
        return *s->io.data.data++;
    default:
        return IO_EOF;
    }
}

int32_t io_write_char(stream* s, char c) {
    size_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        return fputc(c, s->io.stream);
    case STREAM_MEMORY:
        capacity = s->io.data.vec.capacity_end - s->io.data.data;
        if (capacity < 1) {
            size_t pos = s->io.data.data - s->io.data.vec.begin;
            vector_uint8_t_append(&s->io.data.vec, 1 - capacity);
            s->io.data.data = s->io.data.vec.begin + pos;
        }
        *s->io.data.data++ = c;
        if (s->io.data.vec.end != s->io.data.data)
            s->io.data.vec.end = s->io.data.data;
        return 0;
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

void io_dispose(stream* s) {
    if (!s)
        return;

    switch (s->type) {
    case STREAM_FILE:
        if (s->io.stream) {
            fflush(s->io.stream);
            fclose(s->io.stream);
        }
        break;
    case STREAM_MEMORY:
        vector_uint8_t_free(&s->io.data.vec);
        break;
    }
    free(s);
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
        val = reverse_endianess_##t(val); \
    return val; \
}\
\
t io_read_##t##_reverse_endianess(stream* s, bool big_endian) { \
    io_read(s, s->buf, sizeof(t)); \
    t val = *(t*)s->buf; \
    if (big_endian) \
        val = reverse_endianess_##t(val); \
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
        val = reverse_endianess_##t(val); \
    *(t*)s->buf = val; \
    io_write(s, s->buf, sizeof(t)); \
} \
\
void io_write_##t##_reverse_endianess(stream* s, t val, bool big_endian) { \
    if (big_endian) \
        val = reverse_endianess_##t(val); \
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
