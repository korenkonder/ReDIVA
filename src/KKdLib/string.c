/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "string.h"

void string_init(string* c, char* str) {
    if (!str) {
        c->data[0] = 0;
        c->length = 0;
        return;
    }

    c->length = strlen(str);
    if (c->length > STRING_LENGTH - 1) {
        c->ptr = force_malloc(c->length + 1);
        memcpy(c->ptr, str, c->length + 1);
    }
    else
        memcpy(c->data, str, c->length + 1);
}

inline char* string_access(string* c) {
    return c->length > STRING_LENGTH - 1 ? c->ptr : c->data;
}

void string_dispose(string* c) {
    if (c->length > STRING_LENGTH - 1)
        free(c->ptr);
    c->data[0] = 0;
    c->length = 0;
}

inline wchar_t* wstring_access(wstring* c) {
    return c->length > WSTRING_LENGTH - 1 ? c->ptr : c->data;
}

void wstring_init(wstring* c, wchar_t* str) {
    if (!str) {
        c->data[0] = 0;
        c->length = 0;
        return;
    }

    c->length = wcslen(str);
    if (c->length > WSTRING_LENGTH - 1) {
        c->ptr = force_malloc_s(wchar_t, c->length + 1);
        memcpy(c->ptr, str, sizeof(wchar_t) * (c->length + 1));
    }
    else
        memcpy(c->data, str, sizeof(wchar_t) * (c->length + 1));
}

void wstring_dispose(wstring* c) {
    if (c->length > WSTRING_LENGTH - 1)
        free(c->ptr);
    c->data[0] = 0;
    c->length = 0;
}
