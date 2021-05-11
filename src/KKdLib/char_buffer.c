/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "char_buffer.h"

void char_buffer_init(char_buffer* c, char* str) {
    memset(c, 0, sizeof(char_buffer));
    c->length = strlen(str);
    if (c->length > CHAR_BUFFER_LENGTH - 1) {
        c->ptr = force_malloc(c->length + 1);
        memcpy(c->ptr, str, c->length + 1);
    }
    else
        memcpy(c->data, str, c->length + 1);
}

inline char* char_buffer_select(char_buffer* c) {
    return c->length > CHAR_BUFFER_LENGTH - 1 ? c->ptr : c->data;
}

void char_buffer_dispose(char_buffer* c) {
    if (c->length > CHAR_BUFFER_LENGTH - 1)
        free(c->ptr);
    memset(c, 0, sizeof(char_buffer));
}

inline wchar_t* wchar_t_buffer_select(wchar_t_buffer* c) {
    return c->length > WCHAR_T_BUFFER_LENGTH - 1 ? c->ptr : c->data;
}

void wchar_t_buffer_init(wchar_t_buffer* c, wchar_t* str) {
    memset(c, 0, sizeof(wchar_t_buffer));
    c->length = wcslen(str);
    if (c->length > WCHAR_T_BUFFER_LENGTH - 1) {
        c->ptr = force_malloc_s(wchar_t, c->length + 1);
        memcpy(c->ptr, str, sizeof(wchar_t) * (c->length + 1));
    }
    else
        memcpy(c->data, str, sizeof(wchar_t) * (c->length + 1));
}

void wchar_t_buffer_dispose(wchar_t_buffer* c) {
    if (c->length > WCHAR_T_BUFFER_LENGTH - 1)
        free(c->ptr);
    memset(c, 0, sizeof(wchar_t_buffer));
}
