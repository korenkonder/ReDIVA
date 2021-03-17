/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

#define CHAR_BUFFER_LENGTH 24
#define WCHAR_T_BUFFER_LENGTH 12

typedef struct char_buffer {
    union {
        char data[CHAR_BUFFER_LENGTH];
        char* ptr;
    };
    uint64_t length;
} char_buffer;

typedef struct wchar_t_buffer {
    union {
        wchar_t data[WCHAR_T_BUFFER_LENGTH];
        wchar_t* ptr;
    };
    uint64_t length;
} wchar_t_buffer;

extern void char_buffer_init(char_buffer* c, char* str);
extern void char_buffer_dispose(char_buffer* c);
extern void wchar_t_buffer_init(wchar_t_buffer* c, wchar_t* str);
extern void wchar_t_buffer_dispose(wchar_t_buffer* c);

static inline char* char_buffer_select(char_buffer* c) {
    return c->length > CHAR_BUFFER_LENGTH - 1 ? c->ptr : c->data;
}

static inline wchar_t* wchar_t_buffer_select(wchar_t_buffer* c) {
    return c->length > WCHAR_T_BUFFER_LENGTH - 1 ? c->ptr : c->data;
}
