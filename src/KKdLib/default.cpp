/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "default.hpp"

static const uint8_t string_upper_to_lower_table[0x100] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static const uint8_t string_lower_to_upper_table[0x100] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
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
        c = (char)string_upper_to_lower_table[(uint8_t)c];
    return _str;
}

std::string string_to_upper(const std::string& str) {
    std::string _str(str);
    for (char& c : _str)
        c = (char)string_lower_to_upper_table[(uint8_t)c];
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
