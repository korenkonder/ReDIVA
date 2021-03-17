/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "utf8.h"

char* utf8_encode(wchar_t* s) {
    if (!s)
        return 0;

    wchar_t c = 0;
    size_t len = wcslen(s);
    size_t length = 0;
    for (size_t i = 0; i < len; i++) {
        c = s[i];
        if (c <= 0x7F)
            length++;
        else if (c <= 0x7FF)
            length += 2;
        else
            length += 3;
    }

    char* str = force_malloc(length + 1);
    for (size_t i = 0, j = 0; i < len; i++) {
        c = s[i];
        if (c <= 0x7F)
            str[j++] = (char)c;
        else if (c <= 0x7FF) {
            str[j++] = 0xC0 | ((c >> 6) & 0x1F);
            str[j++] = 0x80 | (c & 0x3F);
        }
        else {
            str[j++] = 0xE0 | ((c >> 12) & 0xF);
            str[j++] = 0x80 | ((c >> 6) & 0x3F);
            str[j++] = 0x80 | (c & 0x3F);
        }
    }
    str[length] = 0;
    return str;
}

wchar_t* utf8_decode(char* s) {
    if (!s)
        return 0;

    wchar_t c = 0;
    size_t len = strlen(s);
    size_t length = 0;
    size_t l = 0;

    for (size_t i = 0; i < len; i++) {
        char t = s[i];
        if (!(t & 0x80)) {
            l = 0;
            length++;
            continue;
        }
        else if ((t & 0xFC) == 0xF8)
            continue;
        else if ((t & 0xF8) == 0xF0)
            l = 3;
        else if ((t & 0xF0) == 0xE0)
            l = 2;
        else if ((t & 0xE0) == 0xC0)
            l = 1;
        else if ((t & 0xC0) == 0x80)
            l--;
        if (!l)
            length++;
    }

    wchar_t* str = force_malloc_s(sizeof(wchar_t), length + 1);
    for (size_t i = 0, j = 0; i < len; i++) {
        char t = s[i];
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
            str[j++] = c;
            c = 0;
        }
    }
    str[length] = 0;
    return str;
}
