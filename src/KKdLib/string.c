/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "string.h"
#include "str_utils.h"

const string string_empty = {
    "",
    0,
    STRING_NULL_LENGTH,
};

const wstring wstring_empty = {
    L"",
    0,
    WSTRING_NULL_LENGTH,
};

vector_func(string)
vector_func(wstring)
vector_ptr_func(string)
vector_ptr_func(wstring)

static void string_reserve(string* s, ssize_t size);
static void wstring_reserve(wstring* s, ssize_t size);

inline void string_init(string* s) {
    string_init(s, (char*)0);
}

inline void string_init(string* s, char* str) {
    *s = string_empty;
    if (!str)
        return;

    ssize_t length = utf8_length(str);
    string_reserve(s, length);
    memmove(string_data(s), str, length);
    s->length = length;
    string_data(s)[s->length] = 0;
}

inline void string_init(string* s, const char* str) {
    string_init(s, (char*)str);
}

inline void string_init_length(string* s, ssize_t length) {
    string_init_length(s, (char*)0, length);
}

inline void string_init_length(string* s, char* str, ssize_t length) {
    *s = string_empty;
    if (!str && !length)
        return;

    string_reserve(s, length);
    if (str)
        memmove(string_data(s), str, length);
    else
        memset(string_data(s), 0, length);
    s->length = length;
    string_data(s)[s->length] = 0;
}

inline void string_init_length(string* s, const char* str, ssize_t length) {
    string_init_length(s, (char*)str, length);
}

inline char* string_data(string* s) {
    return s->capacity > STRING_NULL_LENGTH ? s->ptr : s->data;
}

inline void string_add(string* s, char* str) {
    if (!s || !str)
        return;

    ssize_t length = utf8_length(str);
    if (!length)
        return;

    string_reserve(s, length);
    memmove(string_data(s) + s->length, str, length);
    s->length += length;
    string_data(s)[s->length] = 0;
}

inline void string_add(string* s, const char* str) {
    string_add(s, (char*)str);
}

inline void string_add_length(string* s, char* str, ssize_t length) {
    if (!s || !length)
        return;

    string_reserve(s, length);
    if (str)
        memmove(string_data(s) + s->length, str, length);
    else
        memset(string_data(s) + s->length, 0, length);
    s->length += length;
    string_data(s)[s->length] = 0;
}

inline void string_add_length(string* s, const char* str, ssize_t length) {
    string_add_length(s, (char*)str, length);
}

inline void string_add_char(string* s, char c) {
    if (!s || !c)
        return;

    string_reserve(s, 1);
    string_data(s)[s->length++] = c;
    string_data(s)[s->length] = 0;
}

inline void string_copy(string* src, string* dst) {
    string_init_length(dst, string_data(src), src->length);
}

inline void string_copy_wstring(wstring* src, string* dst) {
    char* temp = utf16_to_utf8(wstring_data(src));
    string_init(dst, temp);
    free(temp);
}

inline void string_replace(string* src, string* dst) {
    string_free(dst);
    string_init_length(dst, string_data(src), src->length);
}

inline void string_replace_wstring(wstring* src, string* dst) {
    char* temp = utf16_to_utf8(wstring_data(src));
    string_free(dst);
    string_init(dst, temp);
    free(temp);
}

inline bool string_compare(string* str0, string* str1) {
    if (str0->length != str1->length)
        return false;
    return !memcmp(string_data(str0), string_data(str1),
        str0->length) ? true : false;
}

inline void string_free(string* s) {
    if (s->capacity > STRING_NULL_LENGTH)
        free(s->ptr);
    *s = string_empty;
}

inline wchar_t* wstring_data(wstring* s) {
    return s->capacity > WSTRING_NULL_LENGTH ? s->ptr : s->data;
}

inline void wstring_init(wstring* s) {
    wstring_init(s, (wchar_t*)0);
}

inline void wstring_init(wstring* s, wchar_t* str) {
    *s = wstring_empty;
    if (!str)
        return;

    ssize_t length = utf16_length(str);
    wstring_reserve(s, length);
    memmove(wstring_data(s), str, sizeof(wchar_t) * length);
    s->length = length;
    wstring_data(s)[s->length] = 0;
}

inline void wstring_init(wstring* s, const wchar_t* str) {
    wstring_init(s, (wchar_t*)str);
}

inline void wstring_init_length(wstring* s, ssize_t length) {
    wstring_init_length(s, (wchar_t*)0, length);
}

inline void wstring_init_length(wstring* s, wchar_t* str, ssize_t length) {
    *s = wstring_empty;
    if (!length)
        return;

    wstring_reserve(s, length);
    if (str)
        memmove(wstring_data(s), str, sizeof(wchar_t) * length);
    else
        memset(wstring_data(s), 0, sizeof(wchar_t) * length);
    s->length = length;
    wstring_data(s)[s->length] = 0;
}

inline void wstring_init_length(wstring* s, const wchar_t* str, ssize_t length) {
    wstring_init_length(s, (wchar_t*)str, length);
}

inline void wstring_add(wstring* s, wchar_t* str) {
    if (!s || !str)
        return;

    ssize_t length = utf16_length(str);
    if (!length)
        return;

    wstring_reserve(s, length);
    memmove(wstring_data(s) + s->length, str, sizeof(wchar_t) * length);
    s->length += length;
    wstring_data(s)[s->length] = 0;
}

inline void wstring_add(wstring* s, const wchar_t* str) {
    wstring_add(s, (wchar_t*)str);
}

inline void wstring_add_length(wstring* s, wchar_t* str, ssize_t length) {
    if (!s || !length)
        return;

    wstring_reserve(s, length);
    if (str)
        memmove(wstring_data(s) + s->length, str, sizeof(wchar_t) * length);
    else
        memset(wstring_data(s) + s->length, 0, sizeof(wchar_t) * length);
    s->length += length;
    wstring_data(s)[s->length] = 0;
}

inline void wstring_add_length(wstring* s, const wchar_t* str, ssize_t length) {
    wstring_add_length(s, (wchar_t*)str, length);
}

inline void wstring_add_char(wstring* s, wchar_t c) {
    if (!s || !c)
        return;

    wstring_reserve(s, 1);
    wstring_data(s)[s->length++] = c;
    wstring_data(s)[s->length] = 0;
}

inline void wstring_copy(wstring* src, wstring* dst) {
    wstring_init_length(dst, wstring_data(src), src->length);
}

inline void wstring_copy_string(string* src, wstring* dst) {
    wchar_t* temp = utf8_to_utf16(string_data(src));
    wstring_init(dst, temp);
    free(temp);
}

inline void wstring_replace(wstring* src, wstring* dst) {
    wstring_free(dst);
    wstring_init_length(dst, wstring_data(src), src->length);
}

inline void wstring_replace_string(string* src, wstring* dst) {
    wchar_t* temp = utf8_to_utf16(string_data(src));
    wstring_free(dst);
    wstring_init(dst, temp);
    free(temp);
}

inline bool wstring_compare(wstring* str0, wstring* str1) {
    if (str0->length != str1->length)
        return false;
    return !memcmp(wstring_data(str0), wstring_data(str1),
        sizeof(wchar_t) * str0->length) ? true : false;
}

inline void wstring_free(wstring* s) {
    if (s->capacity > WSTRING_NULL_LENGTH)
        free(s->ptr);
    *s = wstring_empty;
}

static void string_reserve(string* s, ssize_t size) {
    if (s->capacity - s->length >= size)
        return;

    ssize_t capacity = s->capacity;
    if ((ssize_t)(INT64_MAX / sizeof(char) - (capacity >> 1)) >= capacity)
        capacity = (capacity >> 1) + capacity;
    else
        capacity = 0;
    capacity = max(capacity, s->length + size);
    capacity |= STRING_NULL_LENGTH;

    char* src = string_data(s);
    char* dst;
    if (capacity > STRING_NULL_LENGTH)
        dst = force_malloc_s(char, capacity + 1);
    else
        dst = s->data;

    if (dst != src)
        memmove(dst, src, s->length + 1);

    if (capacity > STRING_NULL_LENGTH)
        s->ptr = dst;

    if (s->capacity > STRING_NULL_LENGTH)
        free(src);

    s->capacity = capacity;
}

static void wstring_reserve(wstring* s, ssize_t size) {
    if (s->capacity - s->length >= size)
        return;

    ssize_t capacity = s->capacity;
    if ((ssize_t)(INT64_MAX / sizeof(char) - (capacity >> 1)) >= capacity)
        capacity = (capacity >> 1) + capacity;
    else
        capacity = 0;
    capacity = max(capacity, s->length + size);
    capacity |= WSTRING_NULL_LENGTH;

    wchar_t* src = wstring_data(s);
    wchar_t* dst;
    if (capacity > STRING_NULL_LENGTH)
        dst = force_malloc_s(wchar_t, capacity + 1);
    else
        dst = s->data;

    if (dst != src)
        memmove(dst, src, sizeof(wchar_t) * (s->length + 1));

    if (capacity > STRING_NULL_LENGTH)
        s->ptr = dst;

    if (s->capacity > STRING_NULL_LENGTH)
        free(src);

    s->capacity = capacity;
}
