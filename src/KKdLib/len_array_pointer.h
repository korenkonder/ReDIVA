/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "len_array.h"

#define len_array_pointer(t) \
typedef struct len_array_pointer_##t { \
    t** data; \
    size_t length; \
    size_t fulllength; \
} len_array_pointer_##t; \
\
static const len_array_pointer_##t len_array_pointer_##t##_empty = { 0, 0, 0 }; \
\
extern void len_array_pointer_##t##_append_data(len_array_pointer_##t* buf, t* data); \
extern void len_array_pointer_##t##_free(len_array_pointer_##t* buf);

len_array_pointer(char)
len_array_pointer(wchar_t)

#define len_array_pointer_func(t) \
void len_array_pointer_##t##_append_data(len_array_pointer_##t* buf, t* data) { \
    size_t s = sizeof(t*); \
    len_array_pointer_##t b = *buf; \
    if (b.fulllength < b.length + 1) { \
        t** temp_buf = b.data; \
        b.data = force_malloc_s(s, b.fulllength + BUF_SIZE); \
        memcpy(b.data, temp_buf, b.length * s); \
        b.fulllength += BUF_SIZE; \
        free(temp_buf); \
    } \
    b.data[b.length] = data; \
    b.length++; \
    *buf = b; \
} \
\
void len_array_pointer_##t##_free(len_array_pointer_##t* buf) { \
    for (size_t i = 0; i < buf->length; i++) \
        free(buf->data[i]); \
    free(buf->data); \
    buf->length = buf->fulllength = 0; \
}
