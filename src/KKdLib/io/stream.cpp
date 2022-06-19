/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stream.hpp"
#include <share.h>


stream::io::io() : stream(), data() {

}

stream::io::~io() {

}

bool stream::io::check_null() {
    return !stream && !data.vec.size();
}

bool stream::io::check_not_null() {
    return stream || data.vec.size();
}

void stream::io::close() {
    if (stream) {
        fflush(stream);
        fclose(stream);
        stream = 0;
    }

    data.vec.clear();
    data.vec.shrink_to_fit();
    data.data = data.vec.begin();
}

stream::stream() : buf(), length(), type(), is_big_endian() {

}

stream::~stream() {
    close();
}

void stream::open() {
    close();

    type = STREAM_MEMORY;
    length = 0;
}

void stream::open(const char* path, const char* mode) {
    close();

    if (!path || !mode)
        return;

    wchar_t* temp_path = utf8_to_utf16(path);
    wchar_t* temp_mode = utf8_to_utf16((char*)mode);
    io.stream = _wfsopen(temp_path, temp_mode, _SH_DENYNO);
    type = io.stream && !ferror(io.stream) ? STREAM_FILE : STREAM_NONE;
    get_length();
    free(temp_path);
    free(temp_mode);
}

void stream::open(const wchar_t* path, const wchar_t* mode) {
    close();

    if (!path || !mode)
        return;

    io.stream = _wfsopen(path, mode, _SH_DENYNO);
    type = io.stream && !ferror(io.stream) ? STREAM_FILE : STREAM_NONE;
    get_length();
}

void stream::open(const void* data, size_t size) {
    close();

    if (!data || !size) {
        type = STREAM_MEMORY;
        length = 0;
        return;
    }

    io.data.vec = {};
    io.data.vec.resize(size);
    if (io.data.vec.data() && data)
        memcpy(io.data.vec.data(), data, size);
    io.data.data = io.data.vec.begin();
    type = STREAM_MEMORY;
    length = size;
}

void stream::open(std::vector<uint8_t>& data) {
    close();

    if (!data.size()) {
        type = STREAM_MEMORY;
        length = 0;
        return;
    }

    io.data.vec = data;
    io.data.data = io.data.vec.begin();
    type = STREAM_MEMORY;
    length = data.size();
}

void stream::copy(void** data, size_t* size) {
    if (!this || !data || !size || type != STREAM_MEMORY)
        return;

    *size = io.data.vec.size();
    *data = force_malloc(*size);
    memcpy(*data, io.data.vec.data(), *size);
}

void stream::copy(std::vector<uint8_t>& data) {
    if (!this || type != STREAM_MEMORY)
        return;

    size_t length = io.data.vec.size();
    data.resize(length);
    memcpy(data.data(), io.data.vec.data(), length);
}

int stream::flush() {
    switch (type) {
    case STREAM_FILE:
        return fflush(io.stream);
    case STREAM_MEMORY:
        return 0;
    default:
        return 0;
    }
}

void stream::close() {
    if (!this)
        return;

    memset(buf, 0, sizeof(buf));
    io.close();
    length = 0;
    type = STREAM_NONE;
    is_big_endian = false;
    position_stack.clear();
    position_stack.shrink_to_fit();
}

int64_t stream::get_length() {
    switch (type) {
    case STREAM_FILE:
        if (io.stream) {
            size_t temp = _ftelli64(io.stream);
            _fseeki64(io.stream, 0, SEEK_END);
            length = _ftelli64(io.stream);
            _fseeki64(io.stream, temp, SEEK_SET);
        }
        else
            length = 0;
        break;
    case STREAM_MEMORY:
        length = io.data.vec.size();
        break;
    default:
        length = 0;
        break;
    }
    return length;
}

int64_t stream::get_position() {
    switch (type) {
    case STREAM_FILE:
        return _ftelli64(io.stream);
    case STREAM_MEMORY:
        return io.data.data - io.data.vec.begin();
    default:
        return EOF;
    }
}

int32_t stream::set_position(int64_t pos, int32_t seek) {
    switch (type) {
    case STREAM_FILE:
        return _fseeki64(io.stream, pos, seek);
    case STREAM_MEMORY:
        switch (seek) {
        case SEEK_SET: {
            if (pos < 0)
                return EOF;

            if (io.data.vec.size() < (size_t)pos) {
                size_t size = io.data.vec.size();
                io.data.vec.resize(pos);
                memset(io.data.vec.data() + size, 0, pos - size);
            }
            io.data.data = io.data.vec.begin() + pos;
        } return 0;
        case SEEK_CUR: {
            if (pos > 0) {
                size_t _pos = io.data.data - io.data.vec.begin();
                if (io.data.vec.size() < (size_t)(_pos + pos)) {
                    size_t size = io.data.vec.size();
                    io.data.vec.resize(_pos + pos);
                    memset(io.data.vec.data() + size, 0, _pos + pos - size);
                    io.data.data = io.data.vec.begin() + _pos;
                }
                io.data.data += pos;
            }
            else if (pos < 0) {
                if (io.data.data - io.data.vec.begin() < -pos)
                    return EOF;
                else
                    io.data.data += pos;
            }
        } return 0;
        case SEEK_END: {
            if (pos < 0)
                break;

            if (io.data.vec.size() < (size_t)pos)
                break;

            io.data.data = io.data.vec.end() - pos;
        } return 0;
        }
    }
    return EOF;
}

int32_t stream::position_push(int64_t pos, int32_t seek) {
    position_stack.push_back(get_position());
    return set_position(pos, seek);
}

void stream::position_pop() {
    if (position_stack.size() < 1)
        return;

    int64_t position = position_stack.back();
    position_stack.pop_back();
    set_position(position, SEEK_SET);
    flush();
}

void stream::align_read(int64_t align) {
    int64_t position;
    switch (type) {
    case STREAM_FILE:
        position = _ftelli64(io.stream);
        break;
    case STREAM_MEMORY:
        position = io.data.data - io.data.vec.begin();
        break;
    default:
        return;
    }

    size_t temp_align = align - position % align;
    if (align != temp_align)
        switch (type) {
        case STREAM_FILE:
            _fseeki64(io.stream, position + temp_align, 0);
            break;
        case STREAM_MEMORY: {
            size_t pos = io.data.data - io.data.vec.begin();
            if (io.data.vec.size() < (size_t)(pos + temp_align)) {
                size_t size = io.data.vec.size();
                io.data.vec.resize(pos + temp_align);
                io.data.data = io.data.vec.begin() + pos;
                memset(io.data.data._Ptr, 0, temp_align);
            }
            io.data.data += temp_align;
        } break;
        }
}

void stream::align_write(int64_t align) {
    int64_t position;
    switch (type) {
    case STREAM_FILE:
        position = _ftelli64(io.stream);
        break;
    case STREAM_MEMORY:
        position = io.data.data - io.data.vec.begin();
        break;
    default:
        return;
    }

    size_t temp_align = align - position % align;
    if (align != temp_align)
        switch (type) {
        case STREAM_FILE: {
            memset(buf, 0, min(sizeof(buf), temp_align));
            size_t i = temp_align;
            while (i >= sizeof(buf)) {
                fwrite(buf, 1, sizeof(buf), io.stream);
                i -= sizeof(buf);
            }

            if (i > 0)
                fwrite(buf, 1, i, io.stream);
        } break;
        case STREAM_MEMORY: {
            size_t pos = io.data.data - io.data.vec.begin();
            if (io.data.vec.size() < (size_t)(pos + temp_align)) {
                size_t size = io.data.vec.size();
                io.data.vec.resize(pos + temp_align);
                io.data.data = io.data.vec.begin() + pos;
                memset(io.data.data._Ptr, 0, temp_align);
            }
            io.data.data += temp_align;
        } break;
        }
}

int64_t stream::read(int64_t count) {
    return read((void*)0, count);
}

int64_t stream::read(void* buf, int64_t count) {
    switch (type) {
    case STREAM_FILE:
        if (!buf) {
            int64_t act_count = 0;
            while (count > 0) {
                act_count += fread(this->buf, 1, min(count, sizeof(this->buf)), io.stream);
                count -= sizeof(this->buf);
            }
            return act_count;
        }
        else
            return fread(buf, 1, count, io.stream);
    case STREAM_MEMORY: {
        if (io.data.data >= io.data.vec.end())
            return EOF;

        size_t _count = io.data.vec.end() - io.data.data;
        if (_count >= (size_t)count)
            _count = count;
        if (buf)
            memcpy(buf, io.data.data._Ptr, _count);
        io.data.data += _count;
        return _count;
    }
    default:
        return EOF;
    }
}

int64_t stream::write(int64_t count) {
    return write(0, count);
}

int64_t stream::write(const void* buf, int64_t count) {
    switch (type) {
    case STREAM_FILE:
        if (!buf) {
            memset(this->buf, 0, sizeof(this->buf));
            int64_t act_count = 0;
            while (count > 0) {
                act_count += fwrite(this->buf, 1, min(count, sizeof(this->buf)), io.stream);
                count -= sizeof(this->buf);
            }
            return act_count;
        }
        else
            return fwrite(buf, 1, count, io.stream);
    case STREAM_MEMORY: {
        size_t pos = io.data.data - io.data.vec.begin();
        if (io.data.vec.size() < (size_t)(pos + count)) {
            size_t size = io.data.vec.size();
            io.data.vec.resize(pos + count);
            io.data.data = io.data.vec.begin() + pos;
        }
        if (buf)
            memcpy(io.data.data._Ptr, buf, count);
        else
            memset(io.data.data._Ptr, 0, count);
        io.data.data += count;
        return count;
    }
    default:
        return EOF;
    }
}

int32_t stream::read_char() {
    switch (type) {
    case STREAM_FILE:
        return fgetc(io.stream);
    case STREAM_MEMORY:
        if (io.data.data >= io.data.vec.end())
            return EOF;
        return *(io.data.data++)._Ptr;
    default:
        return EOF;
    }
}

int32_t stream::write_char(char c) {
    switch (type) {
    case STREAM_FILE:
        return fputc(c, io.stream);
    case STREAM_MEMORY: {
        size_t pos = io.data.data - io.data.vec.begin();
        if (io.data.vec.size() < (size_t)(pos + 1)) {
            size_t size = io.data.vec.size();
            io.data.vec.resize(pos + 1);
            io.data.data = io.data.vec.begin() + pos;
        }
        *(io.data.data++)._Ptr = c;
        return 0;
    }
    default:
        return EOF;
    }
}

int8_t stream::read_int8_t() {
    int32_t c = read_char();
    if (c != EOF)
        return (int8_t)c;
    return 0;
}

uint8_t stream::read_uint8_t() {
    int32_t c = read_char();
    if (c != EOF)
        return (uint8_t)c;
    return 0;
}

void stream::write_int8_t(int8_t val) {
    stream::write_char((char)val);
}

void stream::write_uint8_t(uint8_t val) {
    stream::write_char((char)val);
}

std::string stream::read_string(size_t length) {
    char* temp = force_malloc_s(char, length + 1);
    read(temp, length);
    temp[length] = 0;
    std::string str = std::string(temp, length);
    free(temp);
    return str;
}

std::wstring stream::read_wstring(size_t length) {
    wchar_t* temp = force_malloc_s(wchar_t, length + 1);
    read(temp, sizeof(wchar_t) * length);
    temp[length] = 0;
    std::wstring str = std::wstring(temp, length);
    free(temp);
    return str;
}

std::string stream::read_string_null_terminated() {
    int64_t offset = get_position();
    int64_t length = 0;
    char* temp = read_utf8_string_null_terminated_offset_length(offset, &length);
    std::string str = std::string(temp, length);
    free(temp);
    return str;
}

std::wstring stream::read_wstring_null_terminated() {
    int64_t offset = get_position();
    int64_t length = 0;
    wchar_t* temp = read_utf16_string_null_terminated_offset_length(offset, &length);
    std::wstring str = std::wstring(temp, length);
    free(temp);
    return str;
}

std::string stream::read_string_null_terminated_offset(int64_t offset) {
    if (offset) {
        int64_t length = 0;
        char* temp = read_utf8_string_null_terminated_offset_length(offset, &length);
        std::string str = std::string(temp, length);
        free(temp);
        return str;
    }
    else {
        std::string str = {};
        return str;
    }
}

std::wstring stream::read_wstring_null_terminated_offset(int64_t offset) {
    if (offset) {
        int64_t length = 0;
        wchar_t* temp = read_utf16_string_null_terminated_offset_length(offset, &length);
        std::wstring str = std::wstring(temp, length);
        free(temp);
        return str;
    }
    else {
        std::wstring str = {};
        return str;
    }
}

char* stream::read_utf8_string_null_terminated() {
    int64_t offset = get_position();
    int64_t length = 0;
    return read_utf8_string_null_terminated_offset_length(offset, &length);
}

wchar_t* stream::read_utf16_string_null_terminated() {
    int64_t offset = get_position();
    int64_t length = 0;
    return read_utf16_string_null_terminated_offset_length( offset, &length);
}

char* stream::read_utf8_string_null_terminated_length(int64_t* length) {
    int64_t offset = get_position();
    return read_utf8_string_null_terminated_offset_length(offset, length);
}

wchar_t* stream::read_utf16_string_null_terminated_length(int64_t* length) {
    int64_t offset = get_position();
    return read_utf16_string_null_terminated_offset_length(offset, length);
}

char* stream::read_utf8_string_null_terminated_offset(int64_t offset) {
    int64_t length = 0;
    return read_utf8_string_null_terminated_offset_length(offset, &length);
}

wchar_t* stream::read_utf16_string_null_terminated_offset(int64_t offset) {
    int64_t length = 0;
    return read_utf16_string_null_terminated_offset_length(offset, &length);
}

char* stream::read_utf8_string_null_terminated_offset_length(int64_t offset, int64_t* length) {
    position_push(offset, SEEK_SET);

    size_t name_length = 0;
    int32_t c;
    while ((c = read_char()) != EOF && c != 0)
        name_length++;

    if (name_length == 0) {
        position_pop();
        *length = 0;
        return 0;
    }

    char* str = force_malloc_s(char, name_length + 1);
    set_position(offset, SEEK_SET);
    read(str, name_length);
    str[name_length] = 0;

    position_pop();
    *length = name_length;
    return str;
}

wchar_t* stream::read_utf16_string_null_terminated_offset_length(int64_t offset, int64_t* length) {
    position_push(offset, SEEK_SET);

    size_t name_length = 0;
    int32_t c0, c1;
    while ((c0 = read_char()) != EOF && (c1 = read_char()) != EOF
        && (((c0 & 0xFF) | ((c1 & 0xFF) << 8)) != 0))
        name_length++;

    if (name_length == 0) {
        position_pop();
        *length = 0;
        return 0;
    }

    wchar_t* str = force_malloc_s(wchar_t, name_length + 1);
    set_position(offset, SEEK_SET);
    read(str, sizeof(wchar_t) * name_length);
    str[name_length] = 0;

    position_pop();
    *length = name_length;
    return str;
}

int16_t stream::read_int16_t() {
    read(buf, sizeof(int16_t));
    return *(int16_t*)buf;
}

int16_t stream::read_int16_t_reverse_endianness() {
    read(buf, sizeof(int16_t));
    int16_t val;
    if (is_big_endian)
        val = load_reverse_endianness_int16_t(buf);
    else
        val = *(int16_t*)buf;
    return val;
}

int16_t stream::read_int16_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(int16_t));
    int16_t val;
    if (big_endian)
        val = load_reverse_endianness_int16_t(buf);
    else
        val = *(int16_t*)buf;
    return val;
}

void stream::write_int16_t(int16_t val) {
    *(int16_t*)buf = val;
    write(buf, sizeof(int16_t));
}

void stream::write_int16_t_reverse_endianness(int16_t val) {
    if (is_big_endian)
        store_reverse_endianness_int16_t(val, buf);
    else
        *(int16_t*)buf = val;
    write(buf, sizeof(int16_t));
}

void stream::write_int16_t_reverse_endianness(int16_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_int16_t(val, buf);
    else
        *(int16_t*)buf = val;
    write(buf, sizeof(int16_t));
}

uint16_t stream::read_uint16_t() {
    read(buf, sizeof(uint16_t));
    return *(uint16_t*)buf;
}

uint16_t stream::read_uint16_t_reverse_endianness() {
    read(buf, sizeof(uint16_t));
    uint16_t val;
    if (is_big_endian)
        val = load_reverse_endianness_uint16_t(buf);
    else
        val = *(uint16_t*)buf;
    return val;
}

uint16_t stream::read_uint16_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(uint16_t));
    uint16_t val;
    if (big_endian)
        val = load_reverse_endianness_uint16_t(buf);
    else
        val = *(uint16_t*)buf;
    return val;
}

void stream::write_uint16_t(uint16_t val) {
    *(uint16_t*)buf = val;
    write(buf, sizeof(uint16_t));
}

void stream::write_uint16_t_reverse_endianness(uint16_t val) {
    if (is_big_endian)
        store_reverse_endianness_uint16_t(val, buf);
    else
        *(uint16_t*)buf = val;
    write(buf, sizeof(uint16_t));
}

void stream::write_uint16_t_reverse_endianness(uint16_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_uint16_t(val, buf);
    else
        *(uint16_t*)buf = val;
    write(buf, sizeof(uint16_t));
}

int32_t stream::read_int32_t() {
    read(buf, sizeof(int32_t));
    return *(int32_t*)buf;
}

int32_t stream::read_int32_t_reverse_endianness() {
    read(buf, sizeof(int32_t));
    int32_t val;
    if (is_big_endian)
        val = load_reverse_endianness_int32_t(buf);
    else
        val = *(int32_t*)buf;
    return val;
}

int32_t stream::read_int32_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(int32_t));
    int32_t val;
    if (big_endian)
        val = load_reverse_endianness_int32_t(buf);
    else
        val = *(int32_t*)buf;
    return val;
}

void stream::write_int32_t(int32_t val) {
    *(int32_t*)buf = val;
    write(buf, sizeof(int32_t));
}

void stream::write_int32_t_reverse_endianness(int32_t val) {
    if (is_big_endian)
        store_reverse_endianness_int32_t(val, buf);
    else
        *(int32_t*)buf = val;
    write(buf, sizeof(int32_t));
}

void stream::write_int32_t_reverse_endianness(int32_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_int32_t(val, buf);
    else
        *(int32_t*)buf = val;
    write(buf, sizeof(int32_t));
}

uint32_t stream::read_uint32_t() {
    read(buf, sizeof(uint32_t));
    return *(uint32_t*)buf;
}

uint32_t stream::read_uint32_t_reverse_endianness() {
    read(buf, sizeof(uint32_t));
    uint32_t val;
    if (is_big_endian)
        val = load_reverse_endianness_uint32_t(buf);
    else
        val = *(uint32_t*)buf;
    return val;
}

uint32_t stream::read_uint32_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(uint32_t));
    uint32_t val;
    if (big_endian)
        val = load_reverse_endianness_uint32_t(buf);
    else
        val = *(uint32_t*)buf;
    return val;
}

void stream::write_uint32_t(uint32_t val) {
    *(uint32_t*)buf = val;
    write(buf, sizeof(uint32_t));
}

void stream::write_uint32_t_reverse_endianness(uint32_t val) {
    if (is_big_endian)
        store_reverse_endianness_uint32_t(val, buf);
    else
        *(uint32_t*)buf = val;
    write(buf, sizeof(uint32_t));
}

void stream::write_uint32_t_reverse_endianness(uint32_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_uint32_t(val, buf);
    else
        *(uint32_t*)buf = val;
    write(buf, sizeof(uint32_t));
}

int64_t stream::read_int64_t() {
    read(buf, sizeof(int64_t));
    return *(int64_t*)buf;
}

int64_t stream::read_int64_t_reverse_endianness() {
    read(buf, sizeof(int64_t));
    int64_t val;
    if (is_big_endian)
        val = load_reverse_endianness_int64_t(buf);
    else
        val = *(int64_t*)buf;
    return val;
}

int64_t stream::read_int64_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(int64_t));
    int64_t val;
    if (big_endian)
        val = load_reverse_endianness_int64_t(buf);
    else
        val = *(int64_t*)buf;
    return val;
}

void stream::write_int64_t(int64_t val) {
    *(int64_t*)buf = val;
    write(buf, sizeof(int64_t));
}

void stream::write_int64_t_reverse_endianness(int64_t val) {
    if (is_big_endian)
        store_reverse_endianness_int64_t(val, buf);
    else
        *(int64_t*)buf = val;
    write(buf, sizeof(int64_t));
}

void stream::write_int64_t_reverse_endianness(int64_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_int64_t(val, buf);
    else
        *(int64_t*)buf = val;
    write(buf, sizeof(int64_t));
}

uint64_t stream::read_uint64_t() {
    read(buf, sizeof(uint64_t));
    return *(uint64_t*)buf;
}

uint64_t stream::read_uint64_t_reverse_endianness() {
    read(buf, sizeof(uint64_t));
    uint64_t val;
    if (is_big_endian)
        val = load_reverse_endianness_uint64_t(buf);
    else
        val = *(uint64_t*)buf;
    return val;
}

uint64_t stream::read_uint64_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(uint64_t));
    uint64_t val;
    if (big_endian)
        val = load_reverse_endianness_uint64_t(buf);
    else
        val = *(uint64_t*)buf;
    return val;
}

void stream::write_uint64_t(uint64_t val) {
    *(uint64_t*)buf = val;
    write(buf, sizeof(uint64_t));
}

void stream::write_uint64_t_reverse_endianness(uint64_t val) {
    if (is_big_endian)
        store_reverse_endianness_uint64_t(val, buf);
    else
        *(uint64_t*)buf = val;
    write(buf, sizeof(uint64_t));
}

void stream::write_uint64_t_reverse_endianness(uint64_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_uint64_t(val, buf);
    else
        *(uint64_t*)buf = val;
    write(buf, sizeof(uint64_t));
}

half_t stream::read_half_t() {
    read(buf, sizeof(half_t));
    return *(half_t*)buf;
}

half_t stream::read_half_t_reverse_endianness() {
    read(buf, sizeof(half_t));
    half_t val;
    if (is_big_endian)
        val = load_reverse_endianness_half_t(buf);
    else
        val = *(half_t*)buf;
    return val;
}

half_t stream::read_half_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(half_t));
    half_t val;
    if (big_endian)
        val = load_reverse_endianness_half_t(buf);
    else
        val = *(half_t*)buf;
    return val;
}

void stream::write_half_t(half_t val) {
    *(half_t*)buf = val;
    write(buf, sizeof(half_t));
}

void stream::write_half_t_reverse_endianness(half_t val) {
    if (is_big_endian)
        store_reverse_endianness_half_t(val, buf);
    else
        *(half_t*)buf = val;
    write(buf, sizeof(half_t));
}

void stream::write_half_t_reverse_endianness(half_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_half_t(val, buf);
    else
        *(half_t*)buf = val;
    write(buf, sizeof(half_t));
}

float_t stream::read_float_t() {
    read(buf, sizeof(float_t));
    return *(float_t*)buf;
}

float_t stream::read_float_t_reverse_endianness() {
    read(buf, sizeof(float_t));
    float_t val;
    if (is_big_endian)
        val = load_reverse_endianness_float_t(buf);
    else
        val = *(float_t*)buf;
    return val;
}

float_t stream::read_float_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(float_t));
    float_t val;
    if (big_endian)
        val = load_reverse_endianness_float_t(buf);
    else
        val = *(float_t*)buf;
    return val;
}

void stream::write_float_t(float_t val) {
    *(float_t*)buf = val;
    write(buf, sizeof(float_t));
}

void stream::write_float_t_reverse_endianness(float_t val) {
    if (is_big_endian)
        store_reverse_endianness_float_t(val, buf);
    else
        *(float_t*)buf = val;
    write(buf, sizeof(float_t));
}

void stream::write_float_t_reverse_endianness(float_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_float_t(val, buf);
    else
        *(float_t*)buf = val;
    write(buf, sizeof(float_t));
}

double_t stream::read_double_t() {
    read(buf, sizeof(double_t));
    return *(double_t*)buf;
}

double_t stream::read_double_t_reverse_endianness() {
    read(buf, sizeof(double_t));
    double_t val;
    if (is_big_endian)
        val = load_reverse_endianness_double_t(buf);
    else
        val = *(double_t*)buf;
    return val;
}

double_t stream::read_double_t_reverse_endianness(bool big_endian) {
    read(buf, sizeof(double_t));
    double_t val;
    if (big_endian)
        val = load_reverse_endianness_double_t(buf);
    else
        val = *(double_t*)buf;
    return val;
}

void stream::write_double_t(double_t val) {
    *(double_t*)buf = val;
    write(buf, sizeof(double_t));
}

void stream::write_double_t_reverse_endianness(double_t val) {
    if (is_big_endian)
        store_reverse_endianness_double_t(val, buf);
    else
        *(double_t*)buf = val;
    write(buf, sizeof(double_t));
}

void stream::write_double_t_reverse_endianness(double_t val, bool big_endian) {
    if (big_endian)
        store_reverse_endianness_double_t(val, buf);
    else
        *(double_t*)buf = val;
    write(buf, sizeof(double_t));
}

void stream::write_string(std::string& str) {
    write(str.c_str(), str.size());
}

void stream::write_string(std::string&& str) {
    write_string(*(std::string*)&str);
}

void stream::write_wstring(std::wstring& str) {
    write(str.c_str(), sizeof(wchar_t) * str.size());
}

void stream::write_wstring(std::wstring&& str) {
    write_wstring(*(std::wstring*)&str);
}

void stream::write_string_null_terminated(std::string& str) {
    write(str.c_str(), str.size());
    write_uint8_t(0);
}

void stream::write_string_null_terminated(std::string&& str) {
    write_string_null_terminated(*(std::string*)&str);
}

void stream::write_wstring_null_terminated(std::wstring& str) {
    write(str.c_str(), sizeof(wchar_t) * str.size());
    write_uint16_t(0);
}

void stream::write_wstring_null_terminated(std::wstring&& str) {
    write_wstring_null_terminated(*(std::wstring*)&str);
}

void stream::write_utf8_string(const char* str) {
    write(str, utf8_length(str));
}

void stream::write_utf16_string(const wchar_t* str) {
    write(str, sizeof(wchar_t) * utf16_length(str));
}

void stream::write_utf8_string_null_terminated(const char* str) {
    write(str, utf8_length(str));
    write_uint8_t(0);
}

void stream::write_utf16_string_null_terminated(const wchar_t* str) {
    write(str, sizeof(wchar_t) * utf16_length(str));
    write_uint16_t(0);
}

int64_t stream::read_offset(int64_t offset, bool is_x) {
    int64_t val;
    if (!is_x) {
        val = read_uint32_t_reverse_endianness();
        if (val)
            val -= offset;
    }
    else {
        align_read(0x08);
        val = read_int64_t_reverse_endianness();
    }
    return val;
}

int64_t stream::read_offset_f2(int64_t offset) {
    int64_t val = read_uint32_t_reverse_endianness();
    if (val)
        val -= offset;
    return val;
}

int64_t stream::read_offset_x() {
    align_read(0x08);
    int64_t val = read_int64_t_reverse_endianness();
    return val;
}

void stream::write_offset(int64_t val, int64_t offset, bool is_x) {
    if (!is_x) {
        if (val)
            val += offset;
        write_uint32_t_reverse_endianness((uint32_t)val);
    }
    else {
        align_write(0x08);
        write_int64_t_reverse_endianness(val);
    }
}

void stream::write_offset_f2(int64_t val, int64_t offset) {
    if (val)
        val += offset;
    write_uint32_t_reverse_endianness((uint32_t)val);
}

void stream::write_offset_x(int64_t val) {
    align_write(0x08);
    write_int64_t_reverse_endianness(val);
}
