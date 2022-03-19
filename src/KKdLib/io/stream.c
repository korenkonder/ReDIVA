/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stream.h"
#include <share.h>

void io_open(stream* s) {
    memset(s, 0, sizeof(stream));
    s->type = STREAM_MEMORY;
    s->length = 0;
}

void io_open(stream* s, const char* path, const char* mode) {
    memset(s, 0, sizeof(stream));
    if (!path || !mode)
        return;

    wchar_t* temp_path = utf8_to_utf16(path);
    wchar_t* temp_mode = utf8_to_utf16((char*)mode);
    s->io.stream = _wfsopen(temp_path, temp_mode, _SH_DENYNO);
    s->type = s->io.stream ? ferror(s->io.stream) ? STREAM_NONE : STREAM_FILE : STREAM_NONE;
    s->position_stack = vector_old_empty(ssize_t);
    io_get_length(s);
    free(temp_path);
    free(temp_mode);
}

void io_open(stream* s, const wchar_t* path, const wchar_t* mode) {
    memset(s, 0, sizeof(stream));
    if (!path || !mode)
        return;

    s->io.stream = _wfsopen(path, mode, _SH_DENYNO);
    s->type = s->io.stream ? ferror(s->io.stream) ? STREAM_NONE : STREAM_FILE : STREAM_NONE;
    s->position_stack = vector_old_empty(ssize_t);
    io_get_length(s);
}

void io_open(stream* s, const void* data, size_t length) {
    memset(s, 0, sizeof(stream));
    if (!data || !length) {
        s->type = STREAM_MEMORY;
        s->length = 0;
        return;
    }

    s->io.data.vec = vector_old_empty(uint8_t);
    vector_old_uint8_t_reserve(&s->io.data.vec, length);
    if (s->io.data.vec.begin && data)
        memcpy(s->io.data.vec.begin, data, length);
    s->io.data.data = s->io.data.vec.begin;
    s->io.data.vec.end = s->io.data.vec.begin + length;
    s->type = STREAM_MEMORY;
    s->position_stack = vector_old_empty(ssize_t);
    s->length = length;
}

void io_copy(stream* s, void** data, size_t* length) {
    if (!s || !data || !length || s->type != STREAM_MEMORY)
        return;

    *length = vector_old_length(s->io.data.vec);
    *data = force_malloc(*length);
    memcpy(*data, s->io.data.vec.begin, *length);
}

void io_align_read(stream* s, ssize_t align) {
    ssize_t position;
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

    ssize_t capacity;
    ssize_t temp_align = align - position % align;
    if (align != temp_align)
        switch (s->type) {
        case STREAM_FILE:
            _fseeki64(s->io.stream, position + temp_align, 0);
            break;
        case STREAM_MEMORY:
            capacity = s->io.data.vec.end - s->io.data.data;
            if (capacity < temp_align) {
                vector_old_uint8_t_reserve(&s->io.data.vec, temp_align);
                memset(s->io.data.vec.begin + position, 0, temp_align);
            }
            s->io.data.data = s->io.data.vec.begin + position + temp_align;
            if (s->io.data.vec.end < s->io.data.data)
                s->io.data.vec.end = s->io.data.data;
            break;
        }
}

void io_align_write(stream* s, ssize_t align) {
    ssize_t position;
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

    ssize_t capacity;
    ssize_t temp_align = align - position % align;
    if (align != temp_align)
        switch (s->type) {
        case STREAM_FILE: {
            memset(s->buf, 0, min(sizeof(s->buf), temp_align));
            ssize_t i = temp_align;
            while (i >= sizeof(s->buf)) {
                fwrite(s->buf, 1, sizeof(s->buf), s->io.stream);
                i -= sizeof(s->buf);
            }

            if (i > 0)
                fwrite(s->buf, 1, i, s->io.stream);
        } break;
        case STREAM_MEMORY:
            capacity = s->io.data.vec.end - s->io.data.data;
            if (capacity < temp_align) {
                vector_old_uint8_t_reserve(&s->io.data.vec, temp_align);
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

inline ssize_t io_get_length(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        if (s->io.stream) {
            size_t temp = _ftelli64(s->io.stream);
            _fseeki64(s->io.stream, 0, SEEK_END);
            s->length = _ftelli64(s->io.stream);
            _fseeki64(s->io.stream, temp, SEEK_SET);
        }
        else
            s->length = 0;
        break;
    case STREAM_MEMORY:
        s->length = vector_old_length(s->io.data.vec);
        break;
    default:
        s->length = 0;
        break;
    }
    return s->length;
}

inline ssize_t io_get_position(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return _ftelli64(s->io.stream);
    case STREAM_MEMORY:
        return s->io.data.data - s->io.data.vec.begin;
    default:
        return EOF;
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
                return EOF;

            capacity = vector_old_capacity(s->io.data.vec);
            if (capacity < pos)
                vector_old_uint8_t_reserve(&s->io.data.vec, pos - capacity);

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
                    vector_old_uint8_t_reserve(&s->io.data.vec, pos - capacity);
                    s->io.data.vec.end = s->io.data.vec.capacity_end;
                    s->io.data.data = s->io.data.vec.capacity_end;
                }
                else
                    s->io.data.data += pos;
            }
            else if (pos < 0) {
                capacity = s->io.data.data - s->io.data.vec.begin;
                if (capacity < -pos)
                    return EOF;
                else
                    s->io.data.data += pos;
            }
            return 0;
        case SEEK_END:
            if (pos < 0)
                return EOF;

            capacity = vector_old_length(s->io.data.vec);
            if (capacity < pos)
                return EOF;

            s->io.data.data = s->io.data.vec.end - pos;
            return 0;
        }
        return EOF;
    default:
        return EOF;
    }
}

inline int32_t io_position_push(stream* s, ssize_t pos, int32_t seek) {
    *vector_old_ssize_t_reserve_back(&s->position_stack) = io_get_position(s);
    return io_set_position(s, pos, seek);
}

inline void io_position_pop(stream* s) {
    if (vector_old_length(s->position_stack) < 1)
        return;

    ssize_t position = s->position_stack.end[-1];
    vector_old_ssize_t_pop_back(&s->position_stack, 0);
    io_set_position(s, position, SEEK_SET);
    io_flush(s);
}

inline ssize_t io_read(stream* s, ssize_t count) {
    return io_read(s, (void*)0, count);
}

ssize_t io_read(stream* s, void* buf, ssize_t count) {
    ssize_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        if (!buf) {
            ssize_t act_count = 0;
            while (count > 0) {
                act_count += fread(s->buf, 1, min(count, sizeof(s->buf)), s->io.stream);
                count -= sizeof(s->buf);
            }
            return act_count;
        }
        else
            return fread(buf, 1, count, s->io.stream);
    case STREAM_MEMORY:
        if (s->io.data.data >= s->io.data.vec.end)
            return EOF;

        capacity = s->io.data.vec.end - s->io.data.data;
        if (capacity >= count)
            capacity = count;
        if (buf)
            memcpy(buf, s->io.data.data, capacity);
        s->io.data.data += capacity;
        return capacity;
    default:
        return EOF;
    }
}

inline ssize_t io_write(stream* s, ssize_t count) {
    return io_write(s, (void*)0, count);
}

ssize_t io_write(stream* s, void* buf, ssize_t count) {
    ssize_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        if (!buf) {
            memset(s->buf, 0, sizeof(s->buf));
            ssize_t act_count = 0;
            while (count > 0) {
                act_count += fwrite(s->buf, 1, min(count, sizeof(s->buf)), s->io.stream);
                count -= sizeof(s->buf);
            }
            return act_count;
        }
        else
            return fwrite(buf, 1, count, s->io.stream);
    case STREAM_MEMORY:
        capacity = s->io.data.vec.capacity_end - s->io.data.data;
        if (capacity < count) {
            ssize_t pos = s->io.data.data - s->io.data.vec.begin;
            vector_old_uint8_t_reserve(&s->io.data.vec, count + capacity);
            s->io.data.data = s->io.data.vec.begin + pos;
        }
        if (buf)
            memcpy(s->io.data.data, buf, count);
        else
            memset(s->io.data.data, 0, count);
        s->io.data.data += count;
        if (s->io.data.vec.end < s->io.data.data)
            s->io.data.vec.end = s->io.data.data;
        return count;
    default:
        return EOF;
    }
}

inline ssize_t io_write(stream* s, const void* buf, ssize_t count) {
    return io_write(s, (void*)buf, count);
}

int32_t io_read_char(stream* s) {
    switch (s->type) {
    case STREAM_FILE:
        return fgetc(s->io.stream);
    case STREAM_MEMORY:
        if (s->io.data.data >= s->io.data.vec.end)
            return EOF;
        return *s->io.data.data++;
    default:
        return EOF;
    }
}

int32_t io_write_char(stream* s, char c) {
    ssize_t capacity;
    switch (s->type) {
    case STREAM_FILE:
        return fputc(c, s->io.stream);
    case STREAM_MEMORY:
        capacity = s->io.data.vec.capacity_end - s->io.data.data;
        if (capacity < 1) {
            ssize_t pos = s->io.data.data - s->io.data.vec.begin;
            vector_old_uint8_t_reserve(&s->io.data.vec, 1 - capacity);
            s->io.data.data = s->io.data.vec.begin + pos;
        }
        *s->io.data.data++ = c;
        if (s->io.data.vec.end < s->io.data.data)
            s->io.data.vec.end = s->io.data.data;
        return 0;
    default:
        return EOF;
    }
}

inline int8_t io_read_int8_t(stream* s) {
    int32_t c = io_read_char(s);
    if (c != EOF)
        return (int8_t)c;
    return 0;
}

inline uint8_t io_read_uint8_t(stream* s) {
    int32_t c = io_read_char(s);
    if (c != EOF)
        return (uint8_t)c;
    return 0;
}

inline void io_write_int8_t(stream* s, int8_t val) {
    io_write_char(s, (char)val);
}

inline void io_write_uint8_t(stream* s, uint8_t val) {
    io_write_char(s, (char)val);
}

inline void io_read_string(stream* s, string* str, size_t length) {
    string_init_length(str, length);
    char* temp = string_data(str);
    io_read(s, temp, length);
    temp[length] = 0;
}

inline void io_read_string(stream* s, std::string* str, size_t length) {
    char* temp = new char[length + 1];
    io_read(s, temp, length);
    temp[length] = 0;
    *str = std::string(temp, length);
    delete[] temp;
}

inline void io_read_wstring(stream* s, wstring* str, size_t length) {
    wstring_init_length(str, length);
    wchar_t* temp = wstring_data(str);
    io_read(s, temp, sizeof(wchar_t) * length);
    temp[length] = 0;
}

inline void io_read_wstring(stream* s, std::wstring* str, size_t length) {
    wchar_t* temp = new wchar_t[length + 1];
    io_read(s, temp, sizeof(wchar_t) * length);
    temp[length] = 0;
    *str = std::wstring(temp, length);
    delete[] temp;
}

inline void io_read_string_null_terminated(stream* s, string* str) {
    ssize_t offset = io_get_position(s);
    ssize_t length = 0;
    char* temp = io_read_utf8_string_null_terminated_offset_length(s, offset, &length);
    string_init_length(str, temp, length);
    free(temp);
}

inline void io_read_string_null_terminated(stream* s, std::string* str) {
    ssize_t offset = io_get_position(s);
    ssize_t length = 0;
    char* temp = io_read_utf8_string_null_terminated_offset_length(s, offset, &length);
    *str = std::string(temp, length);
    free(temp);
}

inline void io_read_wstring_null_terminated(stream* s, wstring* str) {
    ssize_t offset = io_get_position(s);
    ssize_t length = 0;
    wchar_t* temp = io_read_utf16_string_null_terminated_offset_length(s, offset, &length);
    wstring_init_length(str, temp, length);
    free(temp);
}

inline void io_read_wstring_null_terminated(stream* s, std::wstring* str) {
    ssize_t offset = io_get_position(s);
    ssize_t length = 0;
    wchar_t* temp = io_read_utf16_string_null_terminated_offset_length(s, offset, &length);
    *str = std::wstring(temp, length);
    free(temp);
}

inline void io_read_string_null_terminated_offset(stream* s,
    ssize_t offset, string* str) {
    if (offset) {
        ssize_t length = 0;
        char* temp = io_read_utf8_string_null_terminated_offset_length(s, offset, &length);
        string_init_length(str, temp, length);
        free(temp);
    }
    else
        *str = string_empty;
}

inline void io_read_string_null_terminated_offset(stream* s,
    ssize_t offset, std::string* str) {
    if (offset) {
        ssize_t length = 0;
        char* temp = io_read_utf8_string_null_terminated_offset_length(s, offset, &length);
        *str = std::string(temp, length);
        free(temp);
    }
    else
        *str = std::string();
}

inline void io_read_wstring_null_terminated_offset(stream* s,
    ssize_t offset, wstring* str) {
    if (offset) {
        ssize_t length = 0;
        wchar_t* temp = io_read_utf16_string_null_terminated_offset_length(s, offset, &length);
        wstring_init_length(str, temp, length);
        free(temp);
    }
    else
        *str = wstring_empty;
}

inline void io_read_wstring_null_terminated_offset(stream* s,
    ssize_t offset, std::wstring* str) {
    if (offset) {
        ssize_t length = 0;
        wchar_t* temp = io_read_utf16_string_null_terminated_offset_length(s, offset, &length);
        *str = std::wstring(temp, length);
        free(temp);
    }
    else
        *str = std::wstring();
}

inline char* io_read_utf8_string_null_terminated(stream* s) {
    ssize_t offset = io_get_position(s);
    ssize_t length = 0;
    return io_read_utf8_string_null_terminated_offset_length(s, offset, &length);
}

inline wchar_t* io_read_utf16_string_null_terminated(stream* s) {
    ssize_t offset = io_get_position(s);
    ssize_t length = 0;
    return io_read_utf16_string_null_terminated_offset_length(s, offset, &length);
}

inline char* io_read_utf8_string_null_terminated_length(stream* s, ssize_t* length) {
    ssize_t offset = io_get_position(s);
    return io_read_utf8_string_null_terminated_offset_length(s, offset, length);
}

inline wchar_t* io_read_utf16_string_null_terminated_length(stream* s, ssize_t* length) {
    ssize_t offset = io_get_position(s);
    return io_read_utf16_string_null_terminated_offset_length(s, offset, length);
}

inline char* io_read_utf8_string_null_terminated_offset(stream* s, ssize_t offset) {
    ssize_t length = 0;
    return io_read_utf8_string_null_terminated_offset_length(s, offset, &length);
}

inline wchar_t* io_read_utf16_string_null_terminated_offset(stream* s, ssize_t offset) {
    ssize_t length = 0;
    return io_read_utf16_string_null_terminated_offset_length(s, offset, &length);
}

char* io_read_utf8_string_null_terminated_offset_length(stream* s, ssize_t offset, ssize_t* length) {
    io_position_push(s, offset, SEEK_SET);

    size_t name_length = 0;
    int32_t c;
    while ((c = io_read_char(s)) != EOF && c != 0)
        name_length++;

    if (name_length == 0) {
        io_position_pop(s);
        *length = 0;
        return 0;
    }

    char* str = force_malloc_s(char, name_length + 1);
    io_set_position(s, offset, SEEK_SET);
    io_read(s, str, name_length);
    str[name_length] = 0;

    io_position_pop(s);
    *length = name_length;
    return str;
}

wchar_t* io_read_utf16_string_null_terminated_offset_length(stream* s, ssize_t offset, ssize_t* length) {
    io_position_push(s, offset, SEEK_SET);

    size_t name_length = 0;
    int32_t c0, c1;
    while ((c0 = io_read_char(s)) != EOF
        && (c1 = io_read_char(s)) != EOF
        && (((c0 & 0xFF) | ((c1 & 0xFF) << 8)) != 0))
        name_length++;

    if (name_length == 0) {
        io_position_pop(s);
        *length = 0;
        return 0;
    }

    wchar_t* str = force_malloc_s(wchar_t, name_length + 1);
    io_set_position(s, offset, SEEK_SET);
    io_read(s, str, sizeof(wchar_t) * name_length);
    str[name_length] = 0;

    io_position_pop(s);
    *length = name_length;
    return str;
}

inline void io_write_string(stream* s, string* str) {
    io_write(s, string_data(str), str->length);
}

inline void io_write_string(stream* s, std::string* str) {
    io_write(s, str->c_str(), str->size());
}

inline void io_write_wstring(stream* s, wstring* str) {
    io_write(s, wstring_data(str), sizeof(wchar_t) * str->length);
}

inline void io_write_wstring(stream* s, std::wstring* str) {
    io_write(s, str->c_str(), sizeof(wchar_t) * str->size());
}

inline void io_write_string_null_terminated(stream* s, string* str) {
    io_write(s, string_data(str), str->length);
    io_write_uint8_t(s, 0);
}

inline void io_write_string_null_terminated(stream* s, std::string* str) {
    io_write(s, str->c_str(), str->size());
    io_write_uint8_t(s, 0);
}

inline void io_write_wstring_null_terminated(stream* s, wstring* str) {
    io_write(s, wstring_data(str), sizeof(wchar_t) * str->length);
    io_write_uint16_t(s, 0);
}

inline void io_write_wstring_null_terminated(stream* s, std::wstring* str) {
    io_write(s, str->c_str(), sizeof(wchar_t) * str->size());
    io_write_uint16_t(s, 0);
}

inline void io_write_utf8_string(stream* s, char* str) {
    io_write(s, str, utf8_length(str));
}

inline void io_write_utf8_string(stream* s, const char* str) {
    io_write(s, str, utf8_length(str));
}

inline void io_write_utf16_string(stream* s, wchar_t* str) {
    io_write(s, str, sizeof(wchar_t) * utf16_length(str));
}

inline void io_write_utf16_string(stream* s, const wchar_t* str) {
    io_write(s, str, sizeof(wchar_t) * utf16_length(str));
}

inline void io_write_utf8_string_null_terminated(stream* s, char* str) {
    io_write(s, str, utf8_length(str));
    io_write_uint8_t(s, 0);
}

inline void io_write_utf8_string_null_terminated(stream* s, const char* str) {
    io_write(s, str, utf8_length(str));
    io_write_uint8_t(s, 0);
}

inline void io_write_utf16_string_null_terminated(stream* s, wchar_t* str) {
    io_write(s, str, sizeof(wchar_t) * utf16_length(str));
    io_write_uint16_t(s, 0);
}

inline void io_write_utf16_string_null_terminated(stream* s, const wchar_t* str) {
    io_write(s, str, sizeof(wchar_t) * utf16_length(str));
    io_write_uint16_t(s, 0);
}

inline ssize_t io_read_offset(stream* s, int32_t offset, bool is_x) {
    ssize_t val;
    if (!is_x) {
        val = io_read_int32_t_stream_reverse_endianness(s);
        if (val)
            val -= offset;
    }
    else {
        io_align_read(s, 0x08);
        val = io_read_int64_t_stream_reverse_endianness(s);
    }
    return val;
}

inline int64_t io_read_offset_f2(stream* s, int32_t offset) {
    ssize_t val = io_read_int32_t_stream_reverse_endianness(s);
    if (val)
        val -= offset;
    return val;
}

inline int64_t io_read_offset_x(stream* s) {
    io_align_read(s, 0x08);
    ssize_t val = io_read_int64_t_stream_reverse_endianness(s);
    return val;
}

inline void io_write_offset(stream* s, ssize_t val, int32_t offset, bool is_x) {
    if (!is_x) {
        if (val)
            val += offset;
        io_write_int32_t_stream_reverse_endianness(s, (int32_t)val);
    }
    else {
        io_align_write(s, 0x08);
        io_write_int64_t_stream_reverse_endianness(s, val);
    }
}

inline void io_write_offset_f2(stream* s, ssize_t val, int32_t offset) {
    if (val)
        val += offset;
    io_write_int32_t_stream_reverse_endianness(s, (int32_t)val);
}

inline void io_write_offset_x(stream* s, ssize_t val) {
    io_align_write(s, 0x08);
    io_write_int64_t_stream_reverse_endianness(s, val);
}

void io_free(stream* s) {
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
        vector_old_uint8_t_free(&s->io.data.vec, 0);
        break;
    }
    vector_old_ssize_t_free(&s->position_stack, 0);
    memset(s, 0, sizeof(stream));
}

io_read_write_func(int16_t)
io_read_write_func(uint16_t)
io_read_write_func(int32_t)
io_read_write_func(uint32_t)
io_read_write_func(int64_t)
io_read_write_func(uint64_t)
io_read_write_func(float_t)
io_read_write_func(double_t)
io_read_write_func(half_t)
