/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "str_utils.h"

bool str_utils_check_ends_with(char* str, char* mask) {
    if (!str || !mask)
        return false;

    size_t mask_len = utf8_length(mask);
    size_t len = utf8_length(str);
    char* t = str;
    while (t) {
        t = strstr(t, mask);
        if (t) {
            t += mask_len;
            if (t == str + len)
                return true;
        }
    }
    return false;
}

bool str_utils_wcheck_ends_with(wchar_t* str, wchar_t* mask) {
    if (!str || !mask)
        return false;

    size_t mask_len = utf16_length(mask);
    size_t len = utf16_length(str);
    wchar_t* t = str;
    while (t) {
        t = wcsstr(t, mask);
        if (t) {
            t += mask_len;
            if (t == str + len)
                return true;
        }
    }
    return false;
}

char* str_utils_get_next_int32_t(char* str, int32_t* value, char separator) {
    string s;
    str = str_utils_get_next_string(str, &s, separator);
    sscanf_s(string_data(&s), "%d", value);
    string_free(&s);
    return str;
}

wchar_t* str_utils_wget_next_int32_t(wchar_t* str, int32_t* value, wchar_t separator) {
    wstring s;
    str = str_utils_wget_next_string(str, &s, separator);
    swscanf_s(wstring_data(&s), L"%d", value);
    wstring_free(&s);
    return str;
}

char* str_utils_get_next_float_t(char* str, float_t* value, char separator) {
    string s;
    str = str_utils_get_next_string(str, &s, separator);
    sscanf_s(string_data(&s), "%f", value);
    string_free(&s);
    return str;
}

wchar_t* str_utils_wget_next_float_t(wchar_t* str, float_t* value, wchar_t separator) {
    wstring s;
    str = str_utils_wget_next_string(str, &s, separator);
    swscanf_s(wstring_data(&s), L"%f", value);
    wstring_free(&s);
    return str;
}

char* str_utils_get_next_string(char* str, string* value, char separator) {
    if (!str)
        return 0;

    string_init(value, 0);
    while (*str) {
        char c = *str++;
        if (c == separator)
            break;

        string_add_char(value, c);
    }
    return *str ? str : 0;
}

wchar_t* str_utils_wget_next_string(wchar_t* str, wstring* value, wchar_t separator) {
    wstring_init(value, 0);
    while (*str) {
        wchar_t c = *str++;
        if (c == separator)
            break;

        wstring_add_char(value, c);
    }
    return str;
}

char* str_utils_split_get_right(char* str, char split) {
    if (!str)
        return 0;

    char* t = strchr(str, split);
    if (!t)
        return str_utils_copy(str);
    t++;

    size_t len = utf8_length(t);
    char* p = force_malloc(len + 1);
    memcpy(p, t, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_get_right(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcschr(str, split);
    if (!t)
        return str_utils_wcopy(str);
    t++;

    size_t len = utf16_length(t);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, t, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_get_left(char* str, char split) {
    if (!str)
        return 0;

    char* t = strchr(str, split);

    size_t len = t ? t - str : utf8_length(str);
    char* p = force_malloc(len + 1);
    memcpy(p, str, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_get_left(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcschr(str, split);

    size_t len = t ? t - str : utf16_length(str);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, str, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_get_right_include(char* str, char split) {
    if (!str)
        return 0;

    char* t = strchr(str, split);
    if (!t)
        return str_utils_copy(str);

    size_t len = utf8_length(t);
    char* p = force_malloc(len + 1);
    memcpy(p, t, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_get_right_include(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcschr(str, split);
    if (!t)
        return str_utils_wcopy(str);

    size_t len = utf16_length(t);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, t, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_get_left_include(char* str, char split) {
    if (!str)
        return 0;

    char* t = strchr(str, split);
    t++;

    size_t len = t ? t - str : utf8_length(str);
    char* p = force_malloc(len + 1);
    memcpy(p, str, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_get_left_include(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcschr(str, split);
    t++;

    size_t len = t ? t - str : utf16_length(str);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, str, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_right_get_right(char* str, char split) {
    if (!str)
        return 0;

    char* t = strrchr(str, split);
    if (!t)
        return str_utils_copy(str);
    t++;

    size_t len = t - str;
    char* p = force_malloc(len + 1);
    memcpy(p, t, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_right_get_right(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, split);
    if (!t)
        return str_utils_wcopy(str);
    t++;

    size_t len = t - str;
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, t, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_right_get_left(char* str, char split) {
    if (!str)
        return 0;

    char* t = strrchr(str, split);

    size_t len = t ? t - str : utf8_length(str);
    char* p = force_malloc(len + 1);
    memcpy(p, str, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_right_get_left(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, split);

    size_t len = t ? t - str : utf16_length(str);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, str, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_right_get_right_include(char* str, char split) {
    if (!str)
        return 0;

    char* t = strrchr(str, split);
    if (!t)
        return str_utils_copy(str);

    size_t len = utf8_length(t);
    char* p = force_malloc(len + 1);
    memcpy(p, t, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_right_get_right_include(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, split);
    if (!t)
        return str_utils_wcopy(str);

    size_t len = utf16_length(t);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, t, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_right_get_left_include(char* str, char split) {
    if (!str)
        return 0;

    char* t = strrchr(str, split);
    if (t)
        t++;

    size_t len = t ? t - str : utf8_length(str);
    char* p = force_malloc(len + 1);
    memcpy(p, str, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_right_get_left_include(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, split);
    if (t)
        t++;

    size_t len = t ? t - str : utf16_length(str);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, str, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_get_extension(char* str) {
    if (!str)
        return 0;

    char* t = strrchr(str, '\\');
    return str_utils_split_right_get_right_include(t ? t + 1 : str, '.');
}

wchar_t* str_utils_wget_extension(wchar_t* str) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, L'\\');
    return str_utils_wsplit_right_get_right_include(t ? t + 1 : str, L'.');
}

char* str_utils_get_without_extension(char* str) {
    if (!str)
        return 0;

    char* t = strrchr(str, '\\');
    return str_utils_split_right_get_left(t ? t + 1 : str, '.');
}

wchar_t* str_utils_wget_without_extension(wchar_t* str) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, L'\\');
    return str_utils_wsplit_right_get_left(t ? t + 1 : str, L'.');
}

char* str_utils_add(char* str0, char* str1) {
    if (str0 && str1) {
        size_t str0_len = utf8_length(str0);
        size_t str1_len = utf8_length(str1);
        char* p = force_malloc(str0_len + str1_len + 1);
        memcpy(p, str0, str0_len + 1);
        memcpy(p + str0_len, str1, str1_len + 1);
        return p;
    }
    else if (str0)
        return str_utils_copy(str0);
    else if (str1)
        return str_utils_copy(str1);
    else
        return 0;
}

wchar_t* str_utils_wadd(wchar_t* str0, wchar_t* str1) {
    if (str0 && str1) {
        size_t str0_len = utf16_length(str0);
        size_t str1_len = utf16_length(str1);
        wchar_t* p = force_malloc_s(wchar_t, str0_len + str1_len + 1);
        memcpy(p, str0, sizeof(wchar_t) * (str0_len + 1));
        memcpy(p + str0_len, str1, sizeof(wchar_t) * (str1_len + 1));
        return p;
    }
    else if (str0)
        return str_utils_wcopy(str0);
    else if (str1)
        return str_utils_wcopy(str1);
    else
        return 0;
}

char* str_utils_copy(char* str) {
    if (!str)
        return 0;

    size_t len = utf8_length(str) + 1;
    char* p = force_malloc(len);
    memcpy(p, str, len);
    return p;
}

wchar_t* str_utils_wcopy(wchar_t* str) {
    if (!str)
        return 0;

    size_t len = utf16_length(str) + 1;
    wchar_t* p = force_malloc_s(wchar_t, len);
    memcpy(p, str, sizeof(wchar_t) * len);
    return p;
}

inline int32_t str_utils_compare(char* str0, char* str1) {
    int32_t diff = 0;;
    char c0;
    char c1;
    do
    {
        c0 = *str0++;
        c1 = *str1++;
        if (!c0 || !c1)
            return c0 - c1;
    } while (c0 == c1);
    return c0 - c1;
}

inline int32_t str_utils_wcompare(wchar_t* str0, wchar_t* str1) {
    int32_t diff = 0;;
    wchar_t c0;
    wchar_t c1;
    do
    {
        c0 = *str0++;
        c1 = *str1++;
        if (!c0 || !c1)
            return c0 - c1;
    } while (c0 == c1);
    return c0 - c1;
}

inline int32_t str_utils_compare_length(char* str0, size_t str0_len, char* str1, size_t str1_len) {
    if (!str0_len)
        return -*str1;
    else if (!str1_len)
        return *str0;

    int32_t diff = 0;;
    char c0;
    char c1;
    do
    {
        c0 = *str0++;
        c1 = *str1++;
        if (!c0 || !c1)
            return c0 - c1;
    } while (c0 == c1 && --str0_len && --str1_len);
    return c0 - c1;
}

inline int32_t str_utils_wcompare_length(wchar_t* str0, size_t str0_len, wchar_t* str1, size_t str1_len) {
    if (!str0_len)
        return -*str1;
    else if (!str1_len)
        return *str0;

    int32_t diff = 0;;
    wchar_t c0;
    wchar_t c1;
    do
    {
        c0 = *str0++;
        c1 = *str1++;
        if (!c0 || !c1)
            return c0 - c1;
    } while (c0 == c1 && --str0_len && --str1_len);
    return c0 - c1;
}

size_t str_utils_get_substring_offset(char* str0, size_t str0_len,
    size_t str0_off, char* str1, size_t str1_len) {
    if (!str1_len && str0_off <= str0_len)
        return str0_off;

    if (str0_off < str0_len && str1_len <= str0_len - str0_off) {
        size_t len = str0_len - str1_len - str0_off + 1;
        char* str = &str0[str0_off];
        for (; len; ) {
            char* s = memchr(str, *str1, len);
            if (!s)
                break;

            if (!str1_len || !memcmp(s, str1, str1_len))
                return s - str0;

            len += str - (s + 1);
            str = s + 1;
        }
    }
    return -1;
}

size_t str_utils_wget_substring_offset(wchar_t* str0, size_t str0_len,
    size_t str0_off, wchar_t* str1, size_t str1_len) {
    if (!str1_len && str0_off <= str0_len)
        return str0_off;

    if (str0_off < str0_len && str1_len <= str0_len - str0_off) {
        size_t len = str0_len - str1_len - str0_off + 1;
        wchar_t* str = &str0[str0_off];
        for (; len; ) {
            wchar_t* s = wmemchr(str, *str1, len);
            if (!s)
                break;

            if (!str1_len || !memcmp(s, str1, sizeof(wchar_t) * str1_len))
                return s - str0;

            len += str - (s + 1);
            str = s + 1;
        }
    }
    return -1;
}

bool str_utils_text_file_parse(void* data, size_t length,
    char** buf, char*** lines, size_t* count) {
    if (!data || !length || !buf || !lines || !count)
        return false;

    char* d = data;
    bool del = false;
    size_t c;
    *buf = 0;
    *lines = 0;
    *count = 0;
    if ((uint8_t)d[0] == 0x00)
        return false;
    else if (d[0] == 0xFF) {
        if (length == 1 || (uint8_t)d[1] != 0xFE || length == 2)
            return false;

        wchar_t* w_d = (wchar_t*)data + 1;
        d = utf16_to_utf8(w_d);
        length = utf8_length(d);
        del = true;
        goto decode_utf8_ansi;
    }
    else if ((uint8_t)d[0] == 0xFE) {
        if (length == 1 || (uint8_t)d[1] != 0xFF || length == 2)
            return false;

        length /= 2;
        wchar_t* w_d = (wchar_t*)data + 1;
        w_d = str_utils_wcopy(w_d);
        for (size_t i = 0; i < length; i++)
            w_d[i] = (wchar_t)reverse_endianness_uint16_t((uint16_t)w_d[i]);
        d = utf16_to_utf8(w_d);
        length = utf8_length(d);
        del = true;
        free(w_d);
        goto decode_utf8_ansi;
    }
    else if ((uint8_t)d[0] == 0xEF) {
        if (length == 1 || (uint8_t)d[1] != 0xBB || length == 2 || (uint8_t)d[2] != 0xBF || length == 3)
            return false;

        d += 3;
        length -= 3;
        goto decode_utf8_ansi;
    }
    else {
    decode_utf8_ansi:
        c = 1;
        bool lf;
        char ch;
        char* t;
        lf = false;
        t = d;
        ch = 0;

        size_t buf_len = length;
        for (size_t i = 0, l = 0, m = 0; i < length; i++) {
            ch = *t++;
            if (ch == '\r') {
                if (i + 1 < length && *t == '\n') {
                    i++;
                    t++;
                    l++;
                }
                lf = true;
            }
            else if (ch == '\n')
                lf = true;

            if (lf) {
                if (!l && c > 1)
                    buf_len--;
                c++;
                l = 0;
                m = 0;
                lf = false;
            }
            else {
                l++;
                m++;
            }
        }

        if (ch != '\r' && ch != '\n')
            buf_len++;

        lf = false;
        t = d;
        char* temp_buf = force_malloc(buf_len);
        char** temp_lines = force_malloc_s(char*, c);

        char* b = temp_buf;
        for (size_t i = 0, j = 0, l = 0, m = 0; j < c; i++) {
            ch = *t++;
            if (ch == '\r') {
                if (i + 1 < length && *t == '\n') {
                    i++;
                    t++;
                    l++;
                }
                lf = true;
            }
            else if (ch == '\n')
                lf = true;

            if (i >= length || lf) {
                temp_lines[j] = b;
                if (l) {
                    memcpy(b, d + i - l, m);
                    b[l] = 0;
                    b += l + 1;
                }
                else if (j)
                    temp_lines[j]--;
                else
                    *b++ = 0;
                j++;

                if (!lf)
                    break;

                l = 0;
                m = 0;
                lf = false;
            }
            else {
                l++;
                m++;
            }
        }

        *buf = temp_buf;
        *lines = temp_lines;
        *count = c;
    }

    if (del)
        free(d);
    return true;
}
