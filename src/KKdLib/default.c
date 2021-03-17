/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "default.h"

void* force_malloc(size_t size) {
    void* buf = 0;
    while (!buf)
        buf = malloc(size);
    memset(buf, 0, size);
    return buf;
}

void* force_malloc_s(size_t s, size_t size) {
    void* buf = 0;
    while (!buf)
        buf = malloc(size * s);
    memset(buf, 0, size * s);
    return buf;
}

void char_append_data(char** buf, size_t* buf_len, size_t* act_buf_len, char* data, size_t length) {
    if (length < 1)
        return;

    if (*act_buf_len < *buf_len + length) {
        char* tempBuf = *buf;
        *act_buf_len = max(*act_buf_len * 2, BUF_SIZE);
        *buf = force_malloc(*act_buf_len);
        memcpy(*buf, tempBuf, *buf_len);
        free(tempBuf);
    }
    memcpy(*buf + *buf_len, data, length);
    *buf_len += length;
}

void char_append_data_s(size_t s, char** buf, size_t* buf_len, size_t* act_buf_len, char* data, size_t length) {
    if (length < 1)
        return;

    if (*act_buf_len < *buf_len + length) {
        char* tempBuf = *buf;
        *act_buf_len = max(*act_buf_len * 2 * s, max(s, BUF_SIZE)) / s;
        *buf = force_malloc_s(s, *act_buf_len);
        memcpy(*buf, tempBuf, *buf_len * s);
        free(tempBuf);
    }
    memcpy(*buf + *buf_len * s, data, length * s);
    *buf_len += length;
}

inline int16_t reverse_endianess_int16_t(int16_t value) {
    return (int16_t)_byteswap_ushort((uint16_t)value);
}

inline uint16_t reverse_endianess_uint16_t(uint16_t value) {
    return (uint16_t)_byteswap_ushort(value);
}

inline int32_t reverse_endianess_int32_t(int32_t value) {
    return (int32_t)_byteswap_ulong((uint32_t)value);
}

inline uint32_t reverse_endianess_uint32_t(uint32_t value) {
    return (uint32_t)_byteswap_ulong(value);
}

inline int64_t reverse_endianess_int64_t(int64_t value) {
    return (int64_t)_byteswap_uint64((uint64_t)value);
}

inline uint64_t reverse_endianess_uint64_t(uint64_t value) {
    return (uint64_t)_byteswap_uint64(value);
}

inline ssize_t reverse_endianess_ssize_t(ssize_t value) {
    return (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline size_t reverse_endianess_size_t(size_t value) {
    return (size_t)_byteswap_uint64((uint64_t)value);
}

inline float_t reverse_endianess_float_t(float_t value) {
    uint32_t v = (uint32_t)_byteswap_ulong(*(uint32_t*)&value);
    return *(float_t*)&v;
}

inline double_t reverse_endianess_double_t(double_t value) {
    uint64_t v = (uint64_t)_byteswap_uint64(*(uint64_t*)&value);
    return *(double_t*)&v;
}

char* wchar_t_string_to_char_string(wchar_t* s) {
    if (!s)
        return 0;

    size_t len = wcslen(s);
    char* t = force_malloc(len + 1);
    for (size_t i = 0; i < len; i++)
        t[i] = (char)s[i];
    return t;
}

wchar_t* char_string_to_wchar_t_string(char* s) {
    if (!s)
        return 0;

    size_t len = strlen(s);
    wchar_t* t = force_malloc_s(sizeof(wchar_t), len + 1);
    for (size_t i = 0; i < len; i++)
        t[i] = (wchar_t)s[i];
    return t;
}
