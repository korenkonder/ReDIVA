/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"
#include "../half_t.hpp"

enum stream_type {
    STREAM_NONE = 0,
    STREAM_FILE,
    STREAM_MEMORY,
};

struct stream {

    uint8_t buf[0x100];
    struct io {
        FILE* stream;
        struct data {
            std::vector<uint8_t>::iterator data;
            std::vector<uint8_t> vec;
        } data;

        io();
        ~io();

        bool check_null();
        bool check_not_null();
        void close();
    } io;
    int64_t length;
    stream_type type;
    bool is_big_endian;
    std::vector<int64_t> position_stack;

    stream();
    ~stream();

    void open();
    void open(const char* path, const char* mode);
    void open(const wchar_t* path, const wchar_t* mode);
    void open(const void* data, size_t size);
    void open(std::vector<uint8_t>& data);
    void copy(void** data, size_t* size);
    void copy(std::vector<uint8_t>& data);
    int flush();
    void close();

    int64_t get_length();
    int64_t get_position();
    int32_t set_position(int64_t pos, int32_t seek);
    int32_t position_push(int64_t pos, int32_t seek);
    void position_pop();

    void align_read(int64_t align);
    void align_write(int64_t align);
    int64_t read(int64_t count);
    int64_t read(void* buf, int64_t count);
    int64_t write(int64_t count);
    int64_t write(const void* buf, int64_t count);
    int32_t read_char();
    int32_t write_char(char c);

    int8_t read_int8_t();
    uint8_t read_uint8_t();
    void write_int8_t(int8_t val);
    void write_uint8_t(uint8_t val);

    int16_t read_int16_t();
    int16_t read_int16_t_reverse_endianness();
    int16_t read_int16_t_reverse_endianness(bool big_endian);
    void write_int16_t(int16_t val);
    void write_int16_t_reverse_endianness(int16_t val);
    void write_int16_t_reverse_endianness(int16_t val, bool big_endian);

    uint16_t read_uint16_t();
    uint16_t read_uint16_t_reverse_endianness();
    uint16_t read_uint16_t_reverse_endianness(bool big_endian);
    void write_uint16_t(uint16_t val);
    void write_uint16_t_reverse_endianness(uint16_t val);
    void write_uint16_t_reverse_endianness(uint16_t val, bool big_endian);

    int32_t read_int32_t();
    int32_t read_int32_t_reverse_endianness();
    int32_t read_int32_t_reverse_endianness(bool big_endian);
    void write_int32_t(int32_t val);
    void write_int32_t_reverse_endianness(int32_t val);
    void write_int32_t_reverse_endianness(int32_t val, bool big_endian);

    uint32_t read_uint32_t();
    uint32_t read_uint32_t_reverse_endianness();
    uint32_t read_uint32_t_reverse_endianness(bool big_endian);
    void write_uint32_t(uint32_t val);
    void write_uint32_t_reverse_endianness(uint32_t val);
    void write_uint32_t_reverse_endianness(uint32_t val, bool big_endian);

    int64_t read_int64_t();
    int64_t read_int64_t_reverse_endianness();
    int64_t read_int64_t_reverse_endianness(bool big_endian);
    void write_int64_t(int64_t val);
    void write_int64_t_reverse_endianness(int64_t val);
    void write_int64_t_reverse_endianness(int64_t val, bool big_endian);

    uint64_t read_uint64_t();
    uint64_t read_uint64_t_reverse_endianness();
    uint64_t read_uint64_t_reverse_endianness(bool big_endian);
    void write_uint64_t(uint64_t val);
    void write_uint64_t_reverse_endianness(uint64_t val);
    void write_uint64_t_reverse_endianness(uint64_t val, bool big_endian);

    half_t read_half_t();
    half_t read_half_t_reverse_endianness();
    half_t read_half_t_reverse_endianness(bool big_endian);
    void write_half_t(half_t val);
    void write_half_t_reverse_endianness(half_t val);
    void write_half_t_reverse_endianness(half_t val, bool big_endian);

    float_t read_float_t();
    float_t read_float_t_reverse_endianness();
    float_t read_float_t_reverse_endianness(bool big_endian);
    void write_float_t(float_t val);
    void write_float_t_reverse_endianness(float_t val);
    void write_float_t_reverse_endianness(float_t val, bool big_endian);

    double_t read_double_t();
    double_t read_double_t_reverse_endianness();
    double_t read_double_t_reverse_endianness(bool big_endian);
    void write_double_t(double_t val);
    void write_double_t_reverse_endianness(double_t val);
    void write_double_t_reverse_endianness(double_t val, bool big_endian);

    std::string read_string(size_t length);
    std::wstring read_wstring(size_t length);
    std::string read_string_null_terminated();
    std::wstring read_wstring_null_terminated();
    std::string read_string_null_terminated_offset(int64_t offset);
    std::wstring read_wstring_null_terminated_offset(int64_t offset);
    char* read_utf8_string_null_terminated();
    wchar_t* read_utf16_string_null_terminated();
    char* read_utf8_string_null_terminated_length(int64_t* length);
    wchar_t* read_utf16_string_null_terminated_length(int64_t* length);
    char* read_utf8_string_null_terminated_offset(int64_t offset);
    wchar_t* read_utf16_string_null_terminated_offset(int64_t offset);
    char* read_utf8_string_null_terminated_offset_length(int64_t offset, int64_t* length);
    wchar_t* read_utf16_string_null_terminated_offset_length(int64_t offset, int64_t* length);

    void write_string(std::string& str);
    void write_string(std::string&& str);
    void write_wstring(std::wstring& str);
    void write_wstring(std::wstring&& str);
    void write_string_null_terminated(std::string& str);
    void write_string_null_terminated(std::string&& str);
    void write_wstring_null_terminated(std::wstring& str);
    void write_wstring_null_terminated(std::wstring&& str);
    void write_utf8_string(const char* str);
    void write_utf16_string(const wchar_t* str);
    void write_utf8_string_null_terminated(const char* str);
    void write_utf16_string_null_terminated(const wchar_t* str);

    int64_t read_offset(int64_t offset, bool is_x);
    int64_t read_offset_f2(int64_t offset);
    int64_t read_offset_x();
    void write_offset(int64_t val, int64_t offset, bool is_x);
    void write_offset_f2(int64_t val, int64_t offset);
    void write_offset_x(int64_t val);
};
