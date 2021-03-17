/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#define _USE_MATH_DEFINES
#include "types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fileapi.h>

#define free(ptr) \
{ \
    if (ptr) { \
        free(ptr); \
        (ptr) = 0; \
    } \
    else \
        (ptr) = 0; \
}

#define null(t) \
typedef struct null_##t { \
    bool has_value; \
    t value; \
} null_##t;

null(bool)
null(int8_t)
null(uint8_t)
null(int16_t)
null(uint16_t)
null(int32_t)
null(uint32_t)
null(int64_t)
null(uint64_t)
null(float_t)
null(double_t)
null(size_t)
null(ssize_t)

#define pair(t0, t1) \
typedef struct pair_##t0##_##t1 { \
    t0 key; \
    t1 val; \
} pair_##t0##_##t1;

#define pair_pointer0(t0, t1) \
typedef struct pair_pointer0_##t0##_##t1 { \
    t0* key; \
    t1 val; \
} pair_pointer0_##t0##_##t1;

#define pair_pointer1(t0, t1) \
typedef struct pair_pointer1_##t0##_##t1 { \
    t0 key; \
    t1* val; \
} pair_pointer1_##t0##_##t1;

#define pair_pointer(t0, t1) \
typedef struct pair_pointer_##t0##_##t1 { \
    t0* key; \
    t1* val; \
} pair_pointer_##t0##_##t1;

pair_pointer(char, char)

#define pointer_ptr(t) \
typedef struct pointer_ptr_##t { \
    int32_t offset; \
    t* value; \
} pointer_ptr_##t;

#define pointer(t) \
typedef struct pointer_##t { \
    int32_t offset; \
    t value; \
} pointer_##t;

#define count_pointer(t) \
typedef struct count_pointer_##t { \
    int32_t count; \
    int32_t offset; \
    t value; \
} count_pointer_##t;

typedef union sstring_union {
    char* ptr;
    char data[16];
} sstring_union;

typedef struct sstring {
    sstring_union data;
    uint64_t length;
    uint64_t max_length;
} sstring;

pointer_ptr(char)

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define clamp(a, b, c) (min(max(a, b), c))

#define align_val(a, b) (((a) % (b) == 0) ? (a) : ((a) + (b) - (a) % (b)))

#define align_val_divide(a, b, c) ((((a) % (b) == 0) ? (a) : ((a) + (b) - (a) % (b))) / (c))

#define BUF_SIZE 4096

extern void* force_malloc(size_t size);
extern void* force_malloc_s(size_t s, size_t size);
extern void char_append_data(char** buf, size_t * buf_len,
    size_t * act_buf_len, char* data, size_t length);
extern void char_append_data_s(size_t s, char** buf, size_t* buf_len,
    size_t* act_buf_len, char* data, size_t length);
extern int16_t reverse_endianess_int16_t(int16_t value);
extern uint16_t reverse_endianess_uint16_t(uint16_t value);
extern int32_t reverse_endianess_int32_t(int32_t value);
extern uint32_t reverse_endianess_uint32_t(uint32_t value);
extern int64_t reverse_endianess_int64_t(int64_t value);
extern uint64_t reverse_endianess_uint64_t(uint64_t value);
extern ssize_t reverse_endianess_ssize_t(ssize_t value);
extern size_t reverse_endianess_size_t(size_t value);
extern float_t reverse_endianess_float_t(float_t value);
extern double_t reverse_endianess_double_t(double_t value);
extern char* wchar_t_string_to_char_string(wchar_t* s);
extern wchar_t* char_string_to_wchar_t_string(char* s);
