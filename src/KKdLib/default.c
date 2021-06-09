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

inline void* memcpy_malloc(void* src, size_t size) {
    void* dst = force_malloc(size);
    memcpy(dst, src, size);
    return dst;
}

inline int16_t load_reverse_endianness_int16_t(void* ptr) {
    return (int16_t)_byteswap_ushort(*(uint16_t*)ptr);
}

inline uint16_t load_reverse_endianness_uint16_t(void* ptr) {
    return (uint16_t)_byteswap_ushort(*(uint16_t*)ptr);
}

inline int32_t load_reverse_endianness_int32_t(void* ptr) {
    return (int32_t)_byteswap_ulong(*(uint32_t*)ptr);
}

inline uint32_t load_reverse_endianness_uint32_t(void* ptr) {
    return (uint32_t)_byteswap_ulong(*(uint32_t*)ptr);
}

inline int64_t load_reverse_endianness_int64_t(void* ptr) {
    return (int64_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline uint64_t load_reverse_endianness_uint64_t(void* ptr) {
    return (uint64_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline ssize_t load_reverse_endianness_ssize_t(void* ptr) {
    return (ssize_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline size_t load_reverse_endianness_size_t(void* ptr) {
    return (size_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline float_t load_reverse_endianness_float_t(float_t* ptr) {
    uint32_t v = (uint32_t)_byteswap_ulong(*(uint32_t*)ptr);
    return *(float_t*)&v;
}

inline double_t load_reverse_endianness_double_t(void* ptr) {
    uint64_t v = (uint64_t)_byteswap_uint64(*(uint64_t*)ptr);
    return *(double_t*)&v;
}

inline void store_reverse_endianness_int16_t(int16_t value, void* ptr) {
    *(int16_t*)ptr = (int16_t)_byteswap_ushort((uint16_t)value);
}

inline void store_reverse_endianness_uint16_t(uint16_t value, void* ptr) {
    *(uint16_t*)ptr = (uint16_t)_byteswap_ushort(value);
}

inline void store_reverse_endianness_int32_t(int32_t value, void* ptr) {
    *(int32_t*)ptr = (int32_t)_byteswap_ulong((uint32_t)value);
}

inline void store_reverse_endianness_uint32_t(uint32_t value, void* ptr) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong(value);
}

inline void store_reverse_endianness_int64_t(int64_t value, void* ptr) {
    *(int64_t*)ptr = (int64_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_uint64_t(uint64_t value, void* ptr) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64(value);
}

inline void store_reverse_endianness_ssize_t(ssize_t value, void* ptr) {
    *(ssize_t*)ptr = (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_size_t(size_t value, void* ptr) {
    *(size_t*)ptr = (size_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_float_t(float_t value, void* ptr) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong(*(uint32_t*)&value);
}

inline void store_reverse_endianness_double_t(double_t value, void* ptr) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64(*(uint64_t*)&value);
}

inline int16_t reverse_endianness_int16_t(int16_t value) {
    return (int16_t)_byteswap_ushort((uint16_t)value);
}

inline uint16_t reverse_endianness_uint16_t(uint16_t value) {
    return (uint16_t)_byteswap_ushort(value);
}

inline int32_t reverse_endianness_int32_t(int32_t value) {
    return (int32_t)_byteswap_ulong((uint32_t)value);
}

inline uint32_t reverse_endianness_uint32_t(uint32_t value) {
    return (uint32_t)_byteswap_ulong(value);
}

inline int64_t reverse_endianness_int64_t(int64_t value) {
    return (int64_t)_byteswap_uint64((uint64_t)value);
}

inline uint64_t reverse_endianness_uint64_t(uint64_t value) {
    return (uint64_t)_byteswap_uint64(value);
}

inline ssize_t reverse_endianness_ssize_t(ssize_t value) {
    return (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline size_t reverse_endianness_size_t(size_t value) {
    return (size_t)_byteswap_uint64((uint64_t)value);
}

inline float_t reverse_endianness_float_t(float_t value) {
    uint32_t v = (uint32_t)_byteswap_ulong(*(uint32_t*)&value);
    return *(float_t*)&v;
}

inline double_t reverse_endianness_double_t(double_t value) {
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
    wchar_t* t = force_malloc_s(wchar_t, len + 1);
    for (size_t i = 0; i < len; i++)
        t[i] = (wchar_t)s[i];
    return t;
}
