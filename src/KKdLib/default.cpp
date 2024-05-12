/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "default.hpp"

const char string_upper_to_lower_table[0x100] = {
    (char)0x00, (char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05, (char)0x06, (char)0x07,
    (char)0x08, (char)0x09, (char)0x0A, (char)0x0B, (char)0x0C, (char)0x0D, (char)0x0E, (char)0x0F,
    (char)0x10, (char)0x11, (char)0x12, (char)0x13, (char)0x14, (char)0x15, (char)0x16, (char)0x17,
    (char)0x18, (char)0x19, (char)0x1A, (char)0x1B, (char)0x1C, (char)0x1D, (char)0x1E, (char)0x1F,
    (char)0x20, (char)0x21, (char)0x22, (char)0x23, (char)0x24, (char)0x25, (char)0x26, (char)0x27,
    (char)0x28, (char)0x29, (char)0x2A, (char)0x2B, (char)0x2C, (char)0x2D, (char)0x2E, (char)0x2F,
    (char)0x30, (char)0x31, (char)0x32, (char)0x33, (char)0x34, (char)0x35, (char)0x36, (char)0x37,
    (char)0x38, (char)0x39, (char)0x3A, (char)0x3B, (char)0x3C, (char)0x3D, (char)0x3E, (char)0x3F,
    (char)0x40, (char)0x61, (char)0x62, (char)0x63, (char)0x64, (char)0x65, (char)0x66, (char)0x67,
    (char)0x68, (char)0x69, (char)0x6A, (char)0x6B, (char)0x6C, (char)0x6D, (char)0x6E, (char)0x6F,
    (char)0x70, (char)0x71, (char)0x72, (char)0x73, (char)0x74, (char)0x75, (char)0x76, (char)0x77,
    (char)0x78, (char)0x79, (char)0x7A, (char)0x5B, (char)0x5C, (char)0x5D, (char)0x5E, (char)0x5F,
    (char)0x60, (char)0x61, (char)0x62, (char)0x63, (char)0x64, (char)0x65, (char)0x66, (char)0x67,
    (char)0x68, (char)0x69, (char)0x6A, (char)0x6B, (char)0x6C, (char)0x6D, (char)0x6E, (char)0x6F,
    (char)0x70, (char)0x71, (char)0x72, (char)0x73, (char)0x74, (char)0x75, (char)0x76, (char)0x77,
    (char)0x78, (char)0x79, (char)0x7A, (char)0x7B, (char)0x7C, (char)0x7D, (char)0x7E, (char)0x7F,
    (char)0x80, (char)0x81, (char)0x82, (char)0x83, (char)0x84, (char)0x85, (char)0x86, (char)0x87,
    (char)0x88, (char)0x89, (char)0x8A, (char)0x8B, (char)0x8C, (char)0x8D, (char)0x8E, (char)0x8F,
    (char)0x90, (char)0x91, (char)0x92, (char)0x93, (char)0x94, (char)0x95, (char)0x96, (char)0x97,
    (char)0x98, (char)0x99, (char)0x9A, (char)0x9B, (char)0x9C, (char)0x9D, (char)0x9E, (char)0x9F,
    (char)0xA0, (char)0xA1, (char)0xA2, (char)0xA3, (char)0xA4, (char)0xA5, (char)0xA6, (char)0xA7,
    (char)0xA8, (char)0xA9, (char)0xAA, (char)0xAB, (char)0xAC, (char)0xAD, (char)0xAE, (char)0xAF,
    (char)0xB0, (char)0xB1, (char)0xB2, (char)0xB3, (char)0xB4, (char)0xB5, (char)0xB6, (char)0xB7,
    (char)0xB8, (char)0xB9, (char)0xBA, (char)0xBB, (char)0xBC, (char)0xBD, (char)0xBE, (char)0xBF,
    (char)0xC0, (char)0xC1, (char)0xC2, (char)0xC3, (char)0xC4, (char)0xC5, (char)0xC6, (char)0xC7,
    (char)0xC8, (char)0xC9, (char)0xCA, (char)0xCB, (char)0xCC, (char)0xCD, (char)0xCE, (char)0xCF,
    (char)0xD0, (char)0xD1, (char)0xD2, (char)0xD3, (char)0xD4, (char)0xD5, (char)0xD6, (char)0xD7,
    (char)0xD8, (char)0xD9, (char)0xDA, (char)0xDB, (char)0xDC, (char)0xDD, (char)0xDE, (char)0xDF,
    (char)0xE0, (char)0xE1, (char)0xE2, (char)0xE3, (char)0xE4, (char)0xE5, (char)0xE6, (char)0xE7,
    (char)0xE8, (char)0xE9, (char)0xEA, (char)0xEB, (char)0xEC, (char)0xED, (char)0xEE, (char)0xEF,
    (char)0xF0, (char)0xF1, (char)0xF2, (char)0xF3, (char)0xF4, (char)0xF5, (char)0xF6, (char)0xF7,
    (char)0xF8, (char)0xF9, (char)0xFA, (char)0xFB, (char)0xFC, (char)0xFD, (char)0xFE, (char)0xFF,
};

const char string_lower_to_upper_table[0x100] = {
    (char)0x00, (char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05, (char)0x06, (char)0x07,
    (char)0x08, (char)0x09, (char)0x0A, (char)0x0B, (char)0x0C, (char)0x0D, (char)0x0E, (char)0x0F,
    (char)0x10, (char)0x11, (char)0x12, (char)0x13, (char)0x14, (char)0x15, (char)0x16, (char)0x17,
    (char)0x18, (char)0x19, (char)0x1A, (char)0x1B, (char)0x1C, (char)0x1D, (char)0x1E, (char)0x1F,
    (char)0x20, (char)0x21, (char)0x22, (char)0x23, (char)0x24, (char)0x25, (char)0x26, (char)0x27,
    (char)0x28, (char)0x29, (char)0x2A, (char)0x2B, (char)0x2C, (char)0x2D, (char)0x2E, (char)0x2F,
    (char)0x30, (char)0x31, (char)0x32, (char)0x33, (char)0x34, (char)0x35, (char)0x36, (char)0x37,
    (char)0x38, (char)0x39, (char)0x3A, (char)0x3B, (char)0x3C, (char)0x3D, (char)0x3E, (char)0x3F,
    (char)0x40, (char)0x41, (char)0x42, (char)0x43, (char)0x44, (char)0x45, (char)0x46, (char)0x47,
    (char)0x48, (char)0x49, (char)0x4A, (char)0x4B, (char)0x4C, (char)0x4D, (char)0x4E, (char)0x4F,
    (char)0x50, (char)0x51, (char)0x52, (char)0x53, (char)0x54, (char)0x55, (char)0x56, (char)0x57,
    (char)0x58, (char)0x59, (char)0x5A, (char)0x5B, (char)0x5C, (char)0x5D, (char)0x5E, (char)0x5F,
    (char)0x60, (char)0x41, (char)0x42, (char)0x43, (char)0x44, (char)0x45, (char)0x46, (char)0x47,
    (char)0x48, (char)0x49, (char)0x4A, (char)0x4B, (char)0x4C, (char)0x4D, (char)0x4E, (char)0x4F,
    (char)0x50, (char)0x51, (char)0x52, (char)0x53, (char)0x54, (char)0x55, (char)0x56, (char)0x57,
    (char)0x58, (char)0x59, (char)0x5A, (char)0x7B, (char)0x7C, (char)0x7D, (char)0x7E, (char)0x7F,
    (char)0x80, (char)0x81, (char)0x82, (char)0x83, (char)0x84, (char)0x85, (char)0x86, (char)0x87,
    (char)0x88, (char)0x89, (char)0x8A, (char)0x8B, (char)0x8C, (char)0x8D, (char)0x8E, (char)0x8F,
    (char)0x90, (char)0x91, (char)0x92, (char)0x93, (char)0x94, (char)0x95, (char)0x96, (char)0x97,
    (char)0x98, (char)0x99, (char)0x9A, (char)0x9B, (char)0x9C, (char)0x9D, (char)0x9E, (char)0x9F,
    (char)0xA0, (char)0xA1, (char)0xA2, (char)0xA3, (char)0xA4, (char)0xA5, (char)0xA6, (char)0xA7,
    (char)0xA8, (char)0xA9, (char)0xAA, (char)0xAB, (char)0xAC, (char)0xAD, (char)0xAE, (char)0xAF,
    (char)0xB0, (char)0xB1, (char)0xB2, (char)0xB3, (char)0xB4, (char)0xB5, (char)0xB6, (char)0xB7,
    (char)0xB8, (char)0xB9, (char)0xBA, (char)0xBB, (char)0xBC, (char)0xBD, (char)0xBE, (char)0xBF,
    (char)0xC0, (char)0xC1, (char)0xC2, (char)0xC3, (char)0xC4, (char)0xC5, (char)0xC6, (char)0xC7,
    (char)0xC8, (char)0xC9, (char)0xCA, (char)0xCB, (char)0xCC, (char)0xCD, (char)0xCE, (char)0xCF,
    (char)0xD0, (char)0xD1, (char)0xD2, (char)0xD3, (char)0xD4, (char)0xD5, (char)0xD6, (char)0xD7,
    (char)0xD8, (char)0xD9, (char)0xDA, (char)0xDB, (char)0xDC, (char)0xDD, (char)0xDE, (char)0xDF,
    (char)0xE0, (char)0xE1, (char)0xE2, (char)0xE3, (char)0xE4, (char)0xE5, (char)0xE6, (char)0xE7,
    (char)0xE8, (char)0xE9, (char)0xEA, (char)0xEB, (char)0xEC, (char)0xED, (char)0xEE, (char)0xEF,
    (char)0xF0, (char)0xF1, (char)0xF2, (char)0xF3, (char)0xF4, (char)0xF5, (char)0xF6, (char)0xF7,
    (char)0xF8, (char)0xF9, (char)0xFA, (char)0xFB, (char)0xFC, (char)0xFD, (char)0xFE, (char)0xFF,
};

void* force_malloc(size_t size) {
    if (!size)
        return 0;

    void* buf = 0;
    while (!buf)
        buf = malloc(size);
    memset(buf, 0, size);
    return buf;
}

wchar_t* utf8_to_utf16(const char* s) {
    if (!s)
        return 0;

    uint32_t c = 0;
    size_t length = utf8_to_utf16_length(s);

    wchar_t* str = force_malloc<wchar_t>(length + 1);
    if (!str)
        return 0;

    size_t j = 0;
    size_t l = 0;
    while (*s) {
        char t = *s++;
        if (!(t & 0x80)) {
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

wchar_t* utf8_to_utf16(const char* s, size_t length) {
    if (!s || !length)
        return 0;

    uint32_t c = 0;
    size_t _length = utf8_to_utf16_length(s);

    wchar_t* str = force_malloc<wchar_t>(_length + 1);
    if (!str)
        return 0;

    size_t j = 0;
    size_t l = 0;
    while (*s && length) {
        char t = *s++;
        length--;
        if (!(t & 0x80)) {
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
    str[_length] = 0;
    return str;
}

char* utf16_to_utf8(const wchar_t* s) {
    if (!s)
        return 0;

    uint32_t c = 0;
    size_t length = utf16_to_utf8_length(s);

    char* str = force_malloc<char>(length + 1);
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

char* utf16_to_utf8(const wchar_t* s, size_t length) {
    if (!s || !length)
        return 0;

    uint32_t c = 0;
    size_t _length = utf16_to_utf8_length(s);

    char* str = force_malloc<char>(_length + 1);
    if (!str)
        return 0;

    size_t j = 0;
    while (*s && length) {
        c = *s++;
        length--;
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
    str[_length] = 0;
    return str;
}

std::wstring utf8_to_utf16(const std::string& s) {
    size_t length = s.size();
    if (!length)
        return {};

    uint32_t c = 0;
    size_t _length = utf8_to_utf16_length(s.c_str());

    std::wstring str;
    str.resize(_length);

    char* _s = (char*)s.data();
    wchar_t* _str = (wchar_t*)str.data();

    size_t l = 0;
    while (*_s && length) {
        char t = *_s++;
        length--;
        if (!(t & 0x80)) {
            l = 0;
            *_str++ = t;
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
                *_str++ = c;
            else if (c >= 0x10000 && c <= 0x10FFFF) {
                c -= 0x10000;
                *_str++ = 0xD800 | ((c >> 10) & 0x3FF);
            }
            c = 0;
        }
    }
    *_str++ = 0;
    return str;
}

std::string utf16_to_utf8(const std::wstring& s) {
    size_t length = s.size();
    if (!length)
        return {};

    uint32_t c = 0;
    size_t _length = utf16_to_utf8_length(s.c_str());

    std::string str;
    str.resize(_length);

    wchar_t* _s = (wchar_t*)s.data();
    char* _str = (char*)str.data();

    while (*_s && length) {
        c = *_s++;
        length--;
        if ((c & 0xFC00) == 0xD800) {
            if (!*_s)
                break;

            wchar_t _c = *_s++;
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
            *_str++ = (uint8_t)c;
        else if (c <= 0x7FF) {
            *_str++ = (uint8_t)(0xC0 | ((c >> 6) & 0x1F));
            *_str++ = (uint8_t)(0x80 | (c & 0x3F));
        }
        else if ((c >= 0x800 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF)) {
            *_str++ = (uint8_t)(0xE0 | ((c >> 12) & 0xF));
            *_str++ = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
            *_str++ = (uint8_t)(0x80 | (c & 0x3F));
        }
        else if (c >= 0x10000 && c <= 0x10FFFF) {
            *_str++ = (uint8_t)(0xF0 | ((c >> 18) & 0x7));
            *_str++ = (uint8_t)(0x80 | ((c >> 12) & 0x3F));
            *_str++ = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
            *_str++ = (uint8_t)(0x80 | (c & 0x3F));
        }
    }
    *_str++ = 0;
    return str;
}

std::string string_to_lower(const std::string& str) {
    std::string _str(str);
    for (char& c : _str)
        c = string_upper_to_lower_table[c];
    return _str;
}

std::string string_to_upper(const std::string& str) {
    std::string _str(str);
    for (char& c : _str)
        c = string_lower_to_upper_table[c];
    return _str;
}

std::string sprintf_s_string(_In_z_ _Printf_format_string_ const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string buf = vsprintf_s_string(fmt, args);
    va_end(args);
    return buf;
}

std::wstring swprintf_s_string(_In_z_ _Printf_format_string_ const wchar_t* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::wstring buf = vswprintf_s_string(fmt, args);
    va_end(args);
    return buf;
}

std::string vsprintf_s_string(_In_z_ _Printf_format_string_ const char* const fmt, va_list args) {
    char _buf[0x100];
    int32_t len = vsprintf_s(_buf, sizeof(_buf), fmt, args);
    if (len < 0)
        return {};
    else if (len < sizeof(_buf))
        return std::string(_buf, len);

    std::string buf(len, 0);
    if (vsprintf_s((char*)buf.data(), buf.size(), fmt, args) != len)
        return {};
    return buf;
}

std::wstring vswprintf_s_string(_In_z_ _Printf_format_string_ const wchar_t* const fmt, va_list args) {
    wchar_t _buf[0x100];
    int32_t len = vswprintf_s(_buf, sizeof(_buf) / sizeof(wchar_t), fmt, args);
    if (len < 0)
        return {};
    else if (len < sizeof(_buf))
        return std::wstring(_buf, len);

    std::wstring buf(len, 0);
    if (vswprintf_s((wchar_t*)buf.data(), buf.size(), fmt, args) != len)
        return {};
    return buf;
}
