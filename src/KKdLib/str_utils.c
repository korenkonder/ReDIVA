/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "str_utils.h"

bool str_utils_check_ends_with(char* str, char* mask) {
    if (!str || !mask)
        return false;

    size_t mask_len = strlen(mask);
    size_t len = strlen(str);
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

    size_t mask_len = wcslen(mask);
    size_t len = wcslen(str);
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

char* str_utils_split_get_right(char* str, char split) {
    if (!str)
        return 0;

    char* t = strchr(str, split);
    if (!t)
        return str_utils_copy(str);
    t++;

    size_t len = strlen(t);
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

    size_t len = wcslen(t);
    wchar_t* p = force_malloc_s(wchar_t, len + 1);
    memcpy(p, t, sizeof(wchar_t) * len);
    p[len] = 0;
    return p;
}

char* str_utils_split_get_left(char* str, char split) {
    if (!str)
        return 0;

    char* t = strchr(str, split);

    size_t len = t ? t - str : strlen(str);
    char* p = force_malloc(len + 1);
    memcpy(p, str, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_get_left(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcschr(str, split);

    size_t len = t ? t - str : wcslen(str);
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

    size_t len = strlen(t);
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

    size_t len = wcslen(t);
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

    size_t len = t ? t - str : strlen(str);
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

    size_t len = t ? t - str : wcslen(str);
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

    size_t len = t ? t - str : strlen(str);
    char* p = force_malloc(len + 1);
    memcpy(p, str, len);
    p[len] = 0;
    return p;
}

wchar_t* str_utils_wsplit_right_get_left(wchar_t* str, wchar_t split) {
    if (!str)
        return 0;

    wchar_t* t = wcsrchr(str, split);

    size_t len = t ? t - str : wcslen(str);
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

    size_t len = t - str + 1;
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

    size_t len = t - str + 1;
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

    size_t len = t ? t - str : strlen(str);
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

    size_t len = t ? t - str : wcslen(str);
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
        size_t str0_len = strlen(str0) + 1;
        size_t str1_len = strlen(str1) + 1;
        char* p = force_malloc(str0_len - 1 + str1_len);
        memcpy(p, str0, str0_len);
        memcpy(p + str0_len - 1, str1, str1_len);
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
        size_t str0_len = wcslen(str0) + 1;
        size_t str1_len = wcslen(str1) + 1;
        wchar_t* p = force_malloc_s(wchar_t, str0_len - 1 + str1_len);
        memcpy(p, str0, sizeof(wchar_t) * str0_len);
        memcpy(p + str0_len - 1, str1, sizeof(wchar_t) * str1_len);
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

    size_t len = strlen(str) + 1;
    char* p = force_malloc(len);
    memcpy(p, str, len);
    return p;
}

wchar_t* str_utils_wcopy(wchar_t* str) {
    if (!str)
        return 0;

    size_t len = wcslen(str) + 1;
    wchar_t* p = force_malloc_s(wchar_t, len);
    memcpy(p, str, sizeof(wchar_t) * len);
    return p;
}
