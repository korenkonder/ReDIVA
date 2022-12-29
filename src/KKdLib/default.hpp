/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#define NOMINMAX
#define _USE_MATH_DEFINES
#include "types.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fileapi.h>
#include <string>

#pragma warning( push )
#pragma warning( disable: 26812 )

#define free_def(ptr) \
{ \
    if (ptr) \
        free(ptr); \
    (ptr) = 0; \
}

#define null(t) \
struct null_##t { \
    bool has_value; \
    t value; \
};

null(bool)
null(int8_t)
null(uint8_t)
null(int16_t)
null(uint16_t)
null(int32_t)
null(uint32_t)
null(int64_t)
null(uint64_t)
null(float_t)
null(double_t)
null(size_t)
null(ssize_t)

#define max_def(a, b) (((a) > (b)) ? (a) : (b))

#define min_def(a, b) (((a) < (b)) ? (a) : (b))

#define mult_min_max_def(a, b, c) (((a) >= 0) ? ((a) * (b)) : ((a) * (c)))

#define div_min_max_def(a, b, c) (((a) >= 0) ? ((a) / (b)) : ((a) / (c)))

#define clamp_def(a, b, c) (((((a) > (b)) ? (a) : (b)) < (c)) ? (((a) > (b)) ? (a) : (b)) : (c))

#define align_val(a, b) ((((a) + (b) - 1) / (b)) * (b))

#define align_val_divide(a, b, c) (((a) + (b) - 1) / (c))

#define BUF_SIZE 4096

#define RAD_TO_DEG (180.0 / M_PI)

#define DEG_TO_RAD (M_PI / 180.0)

#define RAD_TO_DEG_FLOAT ((float_t)(180.0 / M_PI))

#define DEG_TO_RAD_FLOAT ((float_t)(M_PI / 180.0))

#define ctgf(x) (1.0f / tanf(x))

#define ctghf(x) (1.0f / tanhf(x))

#define actgf(x) (1.0f / atanf(x))

#define actghf(x) (1.0f / atanhf(x))

#define ctg(x) (1.0 / tan(x))

#define ctgh(x) (1.0 / tanh(x))

#define actg(x) (1.0 / atan(x))

#define actgh(x) (1.0 / atanh(x))

#define lerp_def(x, y, blend) ((1.0f - (blend)) * (x) + (blend) * (y))

extern void* force_malloc(size_t size);
#define force_malloc_s(s, size) (s*)force_malloc(sizeof(s) * (size))

#define enum_or(s0, s1) \
(s0) = (decltype(s0))((int32_t)(s0) | (s1))

#define enum_xor(s0, s1) \
(s0) = (decltype(s0))((int32_t)(s0) ^ (s1))

#define enum_and(s0, s1) \
(s0) = (decltype(s0))((int32_t)(s0) & (s1))

#define enum_not(s0, s1) \
(s0) = (decltype(s0))(~(int32_t)(s0))

inline int16_t load_reverse_endianness_int16_t(const void* ptr) {
    return (int16_t)_byteswap_ushort(*(uint16_t*)ptr);
}

inline uint16_t load_reverse_endianness_uint16_t(const void* ptr) {
    return (uint16_t)_byteswap_ushort(*(uint16_t*)ptr);
}

inline int32_t load_reverse_endianness_int32_t(const void* ptr) {
    return (int32_t)_byteswap_ulong(*(uint32_t*)ptr);
}

inline uint32_t load_reverse_endianness_uint32_t(const void* ptr) {
    return (uint32_t)_byteswap_ulong(*(uint32_t*)ptr);
}

inline int64_t load_reverse_endianness_int64_t(const void* ptr) {
    return (int64_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline uint64_t load_reverse_endianness_uint64_t(const void* ptr) {
    return (uint64_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline ssize_t load_reverse_endianness_ssize_t(const void* ptr) {
    return (ssize_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline size_t load_reverse_endianness_size_t(const void* ptr) {
    return (size_t)_byteswap_uint64(*(uint64_t*)ptr);
}

inline float_t load_reverse_endianness_float_t(const void* ptr) {
    uint32_t v = (uint32_t)_byteswap_ulong(*(uint32_t*)ptr);
    return *(float_t*)&v;
}

inline double_t load_reverse_endianness_double_t(const void* ptr) {
    uint64_t v = (uint64_t)_byteswap_uint64(*(uint64_t*)ptr);
    return *(double_t*)&v;
}

inline void store_reverse_endianness_int16_t(void* ptr, int16_t value) {
    *(int16_t*)ptr = (int16_t)_byteswap_ushort((uint16_t)value);
}

inline void store_reverse_endianness_int16_t(void* ptr, uint16_t value) {
    *(int16_t*)ptr = (int16_t)_byteswap_ushort(value);
}

inline void store_reverse_endianness_uint16_t(void* ptr, int16_t value) {
    *(uint16_t*)ptr = (uint16_t)_byteswap_ushort((uint16_t)value);
}

inline void store_reverse_endianness_uint16_t(void* ptr, uint16_t value) {
    *(uint16_t*)ptr = (uint16_t)_byteswap_ushort(value);
}

inline void store_reverse_endianness_int32_t(void* ptr, int32_t value) {
    *(int32_t*)ptr = (int32_t)_byteswap_ulong((uint32_t)value);
}

inline void store_reverse_endianness_int32_t(void* ptr, uint32_t value) {
    *(int32_t*)ptr = (int32_t)_byteswap_ulong(value);
}

inline void store_reverse_endianness_uint32_t(void* ptr, int32_t value) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong((uint32_t)value);
}

inline void store_reverse_endianness_uint32_t(void* ptr, uint32_t value) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong(value);
}

inline void store_reverse_endianness_int64_t(void* ptr, int64_t value) {
    *(int64_t*)ptr = (int64_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_int64_t(void* ptr, uint64_t value) {
    *(int64_t*)ptr = (int64_t)_byteswap_uint64(value);
}

inline void store_reverse_endianness_uint64_t(void* ptr, int64_t value) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_uint64_t(void* ptr, uint64_t value) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64(value);
}

inline void store_reverse_endianness_ssize_t(void* ptr, ssize_t value) {
    *(ssize_t*)ptr = (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_ssize_t(void* ptr, size_t value) {
    *(ssize_t*)ptr = (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_size_t(void* ptr, ssize_t value) {
    *(size_t*)ptr = (size_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_size_t(void* ptr, size_t value) {
    *(size_t*)ptr = (size_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_float_t(void* ptr, float_t value) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong(*(uint32_t*)&value);
}

inline void store_reverse_endianness_double_t(void* ptr, double_t value) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64(*(uint64_t*)&value);
}

inline int16_t reverse_endianness_int16_t(int16_t value) {
    return (int16_t)_byteswap_ushort((uint16_t)value);
}

inline int16_t reverse_endianness_int16_t(uint16_t value) {
    return (int16_t)_byteswap_ushort(value);
}

inline uint16_t reverse_endianness_uint16_t(int16_t value) {
    return (uint16_t)_byteswap_ushort((uint16_t)value);
}

inline uint16_t reverse_endianness_uint16_t(uint16_t value) {
    return (uint16_t)_byteswap_ushort(value);
}

inline int32_t reverse_endianness_int32_t(int32_t value) {
    return (int32_t)_byteswap_ulong((uint32_t)value);
}

inline int32_t reverse_endianness_int32_t(uint32_t value) {
    return (int32_t)_byteswap_ulong(value);
}

inline uint32_t reverse_endianness_uint32_t(int32_t value) {
    return (uint32_t)_byteswap_ulong((uint32_t)value);
}

inline uint32_t reverse_endianness_uint32_t(uint32_t value) {
    return (uint32_t)_byteswap_ulong(value);
}

inline int64_t reverse_endianness_int64_t(int64_t value) {
    return (int64_t)_byteswap_uint64((uint64_t)value);
}

inline int64_t reverse_endianness_int64_t(uint64_t value) {
    return (int64_t)_byteswap_uint64(value);
}

inline uint64_t reverse_endianness_uint64_t(int64_t value) {
    return (uint64_t)_byteswap_uint64((uint64_t)value);
}

inline uint64_t reverse_endianness_uint64_t(uint64_t value) {
    return (uint64_t)_byteswap_uint64(value);
}

inline ssize_t reverse_endianness_ssize_t(ssize_t value) {
    return (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline ssize_t reverse_endianness_ssize_t(size_t value) {
    return (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline size_t reverse_endianness_size_t(ssize_t value) {
    return (size_t)_byteswap_uint64((uint64_t)value);
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

inline void printf_debug(const char* fmt, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
}

inline constexpr size_t utf8_length(const char* s) {
    if (!s)
        return 0;

    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

inline constexpr size_t utf16_length(const wchar_t* s) {
    if (!s)
        return 0;

    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

inline constexpr bool utf8_check_for_ascii_only(const char* s) {
    char c = 0;
    while (c = *s++)
        if (c & 0x80)
            return false;
    return true;
}

inline constexpr size_t utf8_to_utf16_length(const char* s) {
    if (!s)
        return 0;

    uint32_t c = 0;
    size_t length = 0;
    size_t l = 0;

    while (*s) {
        char t = *s++;
        if (!(t & 0x80)) {
            l = 0;
            length++;
            c = 0;
            continue;
        }
        else if ((t & 0xFC) == 0xF8)
            continue;
        else if ((t & 0xF8) == 0xF0) {
            c = t & 0x07;
            l = 3;
        }
        else if ((t & 0xF0) == 0xE0) {
            c = t & 0x0F;
            l = 2;
        }
        else if ((t & 0xE0) == 0xC0) {
            c = t & 0x1F;
            l = 1;
        }
        else if ((t & 0xC0) == 0x80) {
            c = (c << 6) | (t & 0x3F);
            l--;
        }

        if (!l) {
            if (c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF))
                length++;
            else if (c >= 0x10000 && c <= 0x10FFFF) {
                length++;
                length++;
            }
            c = 0;
        }
    }
    return length;
}

inline constexpr size_t utf16_to_utf8_length(const wchar_t* s) {
    uint32_t c = 0;
    size_t length = 0;
    while (*s) {
        c = *s++;
        if ((c & 0xFC00) == 0xD800) {
            if (!*s)
                break;

            wchar_t _c = *s++;
            if ((_c & 0xFC00) != 0xDC00)
                continue;

            c &= 0x3FF;
            c <<= 10;
            c |= _c & 0x3FF;
            c += 0x10000;
        }
        else if ((c & 0xFC00) == 0xDC00)
            continue;

        if (c <= 0x7F)
            length++;
        else if (c <= 0x7FF)
            length += 2;
        else if ((c >= 0x800 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF))
            length += 3;
        else if (c >= 0x10000 && c <= 0x10FFFF)
            length += 4;
    }
    return length;
}

extern wchar_t* utf8_to_utf16(const char* s);
extern char* utf16_to_utf8(const wchar_t* s);
extern std::wstring utf8_to_utf16(const std::string& s);
extern std::string utf16_to_utf8(const std::wstring& s);
