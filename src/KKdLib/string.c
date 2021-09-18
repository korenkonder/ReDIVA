/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "string.h"
#include "str_utils.h"

inline void string_init(string* s, char* str) {
    if (!str) {
        s->data[0] = 0;
        s->length = 0;
        return;
    }

    s->length = utf8_length(str);
    if (s->length > STRING_LENGTH - 1) {
        s->ptr = force_malloc(s->length + 1);
        memcpy(s->ptr, str, s->length + 1);
    }
    else
        memcpy(s->data, str, s->length + 1);
}

inline char* string_data(string* s) {
    return s->length ? s->length > STRING_LENGTH - 1 ? s->ptr : s->data : 0;
}

inline void string_add(string* s, char* str) {
    char* t = str_utils_add(string_data(s), str);
    string_free(s);
    string_init(s, t);
    free(t);
}

inline void string_copy(string* src, string* dst) {
    string_init(dst, string_data(src));
}

inline void string_copy_wstring(wstring* src, string* dst) {
    char* temp = utf16_to_utf8(wstring_access(src));
    string_init(dst, temp);
    free(temp);
}

inline size_t string_length(string* s) {
    return s->length;
}

inline void string_free(string* s) {
    if (s->length > STRING_LENGTH - 1)
        free(s->ptr);
    s->data[0] = 0;
    s->length = 0;
}

inline wchar_t* wstring_access(wstring* s) {
    return  s->length ? s->length > WSTRING_LENGTH - 1 ? s->ptr : s->data : 0;
}

inline void wstring_init(wstring* s, wchar_t* str) {
    if (!str) {
        s->data[0] = 0;
        s->length = 0;
        return;
    }

    s->length = utf16_length(str);
    if (s->length > WSTRING_LENGTH - 1) {
        s->ptr = force_malloc_s(wchar_t, s->length + 1);
        memcpy(s->ptr, str, sizeof(wchar_t) * (s->length + 1));
    }
    else
        memcpy(s->data, str, sizeof(wchar_t) * (s->length + 1));
}

inline void wstring_add(wstring* s, wchar_t* str) {
    wchar_t* t = str_utils_wadd(wstring_access(s), str);
    wstring_free(s);
    wstring_init(s, t);
    free(t);
}

inline void wstring_copy(wstring* src, wstring* dst) {
    wstring_init(dst, wstring_access(src));
}

inline void wstring_copy_string(string* src, wstring* dst) {
    wchar_t* temp = utf8_to_utf16(string_data(src));
    wstring_init(dst, temp);
    free(temp);
}

inline size_t wstring_length(wstring* s) {
    return s->length;
}

inline void wstring_free(wstring* s) {
    if (s->length > WSTRING_LENGTH - 1)
        free(s->ptr);
    s->data[0] = 0;
    s->length = 0;
}
