/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

#define len_array(t) \
typedef struct len_array_##t { \
    t* data; \
    size_t length; \
    size_t fulllength; \
} len_array_##t; \
\
static const len_array_##t len_array_##t##_empty = { 0, 0, 0 }; \
\
extern void len_array_##t##_append_data(len_array_##t* buf, t* data, size_t length); \
extern void len_array_##t##_free(len_array_##t* buf);

len_array(char)
len_array(wchar_t)
len_array(bool)
len_array(int8_t)
len_array(uint8_t)
len_array(int16_t)
len_array(uint16_t)
len_array(int32_t)
len_array(uint32_t)
len_array(int64_t)
len_array(uint64_t)
len_array(size_t)
len_array(pointer_ptr_char)

#define len_array_func(t) \
void len_array_##t##_append_data(len_array_##t* buf, t* data, size_t length) { \
    char_append_data_s(sizeof(t), (char**)&buf->data, &buf->length, &buf->fulllength, (char*)data, length); \
} \
\
void len_array_##t##_free(len_array_##t* buf) { \
    free(buf->data); \
    buf->length = buf->fulllength = 0; \
}
