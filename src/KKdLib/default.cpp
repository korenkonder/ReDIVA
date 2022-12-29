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

std::wstring utf8_to_utf16(const std::string& s) {
    if (!s.size())
        return {};

    uint32_t c = 0;
    size_t length = utf8_to_utf16_length(s.c_str());

    std::wstring str;
    str.resize(length);

    char* _s = (char*)s.data();
    wchar_t* _str = (wchar_t*)str.data();

    size_t l = 0;
    while (*_s) {
        char t = *_s++;
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
    if (!s.size())
        return {};

    uint32_t c = 0;
    size_t length = utf16_to_utf8_length(s.c_str());

    std::string str;
    str.resize(length);

    wchar_t* _s = (wchar_t*)s.data();
    char* _str = (char*)str.data();

    while (*_s) {
        c = *_s++;
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
