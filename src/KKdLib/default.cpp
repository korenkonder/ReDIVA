/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "default.hpp"

void* force_malloc(size_t size) {
    if (!size)
        return 0;

    void* buf = 0;
    while (!buf)
        buf = malloc(size);
    memset(buf, 0, size);
    return buf;
}

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

inline void store_reverse_endianness_int16_t(int16_t value, void* ptr) {
    *(int16_t*)ptr = (int16_t)_byteswap_ushort((uint16_t)value);
}

inline void store_reverse_endianness_int16_t(uint16_t value, void* ptr) {
    *(int16_t*)ptr = (int16_t)_byteswap_ushort(value);
}

inline void store_reverse_endianness_uint16_t(int16_t value, void* ptr) {
    *(uint16_t*)ptr = (uint16_t)_byteswap_ushort((uint16_t)value);
}

inline void store_reverse_endianness_uint16_t(uint16_t value, void* ptr) {
    *(uint16_t*)ptr = (uint16_t)_byteswap_ushort(value);
}

inline void store_reverse_endianness_int32_t(int32_t value, void* ptr) {
    *(int32_t*)ptr = (int32_t)_byteswap_ulong((uint32_t)value);
}

inline void store_reverse_endianness_int32_t(uint32_t value, void* ptr) {
    *(int32_t*)ptr = (int32_t)_byteswap_ulong(value);
}

inline void store_reverse_endianness_uint32_t(int32_t value, void* ptr) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong((uint32_t)value);
}

inline void store_reverse_endianness_uint32_t(uint32_t value, void* ptr) {
    *(uint32_t*)ptr = (uint32_t)_byteswap_ulong(value);
}

inline void store_reverse_endianness_int64_t(int64_t value, void* ptr) {
    *(int64_t*)ptr = (int64_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_int64_t(uint64_t value, void* ptr) {
    *(int64_t*)ptr = (int64_t)_byteswap_uint64(value);
}

inline void store_reverse_endianness_uint64_t(int64_t value, void* ptr) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_uint64_t(uint64_t value, void* ptr) {
    *(uint64_t*)ptr = (uint64_t)_byteswap_uint64(value);
}

inline void store_reverse_endianness_ssize_t(ssize_t value, void* ptr) {
    *(ssize_t*)ptr = (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_ssize_t(size_t value, void* ptr) {
    *(ssize_t*)ptr = (ssize_t)_byteswap_uint64((uint64_t)value);
}

inline void store_reverse_endianness_size_t(ssize_t value, void* ptr) {
    *(size_t*)ptr = (size_t)_byteswap_uint64((uint64_t)value);
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

inline size_t utf8_length(const char* s) {
    if (!s)
        return 0;

    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

inline size_t utf16_length(const wchar_t* s) {
    if (!s)
        return 0;

    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

wchar_t* utf8_to_utf16(const char* s) {
    if (!s)
        return 0;

    uint32_t c = 0;
    size_t length = utf8_to_utf16_length(s);

    wchar_t* str = force_malloc_s(wchar_t, length + 1);
    if (!str)
        return 0;

    size_t j = 0;
    size_t l = 0;
    while (*s) {
        char t = *s++;
        if (~t & 0x80) {
            l = 0;
            str[j++] = t;
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
                str[j++] = c;
            else if (c >= 0x10000 && c <= 0x10FFFF) {
                c -= 0x10000;
                str[j++] = 0xD800 | ((c >> 10) & 0x3FF);
                str[j++] = 0xDC00 | (c & 0x3FF);
            }
            c = 0;
        }
    }
    str[length] = 0;
    return str;
}

char* utf16_to_utf8(const wchar_t* s) {
    if (!s)
        return 0;

    uint32_t c = 0;
    size_t length = utf16_to_utf8_length(s);

    char* str = force_malloc_s(char, length + 1);
    if (!str)
        return 0;

    size_t j = 0;
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
            str[j++] = (uint8_t)c;
        else if (c <= 0x7FF) {
            str[j++] = (uint8_t)(0xC0 | ((c >> 6) & 0x1F));
            str[j++] = (uint8_t)(0x80 | (c & 0x3F));
        }
        else if ((c >= 0x800 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF)) {
            str[j++] = (uint8_t)(0xE0 | ((c >> 12) & 0xF));
            str[j++] = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
            str[j++] = (uint8_t)(0x80 | (c & 0x3F));
        }
        else if (c >= 0x10000 && c <= 0x10FFFF) {
            str[j++] = (uint8_t)(0xF0 | ((c >> 18) & 0x7));
            str[j++] = (uint8_t)(0x80 | ((c >> 12) & 0x3F));
            str[j++] = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
            str[j++] = (uint8_t)(0x80 | (c & 0x3F));
        }
    }
    str[length] = 0;
    return str;
}

inline bool utf8_check_for_ascii_only(const char* s) {
    char c;
    while (c = *s++)
        if (c & 0x80)
            return false;
    return true;
}

size_t utf8_to_utf16_length(const char* s) {
    if (!s)
        return 0;

    uint32_t c = 0;
    size_t length = 0;
    size_t l = 0;

    while (*s) {
        char t = *s++;
        if (~t & 0x80) {
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

size_t utf16_to_utf8_length(const wchar_t* s) {
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
