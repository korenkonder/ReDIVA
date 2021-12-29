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
    if (ptr) \
        free(ptr); \
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

#define ptr_t(t) \
typedef t* ptr_##t;

#define pair(t0, t1) \
typedef struct pair_##t0##_##t1 { \
    t0 key; \
    t1 val; \
} pair_##t0##_##t1;

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define mult_min_max(a, b, c) (((a) >= 0) ? ((a) * (b)) : ((a) * (c)))

#define div_min_max(a, b, c) (((a) >= 0) ? ((a) / (b)) : ((a) / (c)))

#define clamp(a, b, c) (((((a) > (b)) ? (a) : (b)) < (c)) ? (((a) > (b)) ? (a) : (b)) : (c))

#define align_val(a, b) ((((a) + (b) - 1) / (b)) * (b))

#define align_val_divide(a, b, c) (((a) + (b) - 1) / (c))

#define BUF_SIZE 4096

#define RAD_TO_DEG (180.0 / M_PI)

#define DEG_TO_RAD (M_PI / 180.0)

#define RAD_TO_DEG_FLOAT ((float_t)(180.0 / M_PI))

#define DEG_TO_RAD_FLOAT ((float_t)(M_PI / 180.0))

#define ctgf(x) (1.0f / tanf(x))

#define ctghf(x) (1.0f / tanhf(x))

#define actgf(x) (1.0f / atanf(x))

#define actghf(x) (1.0f / atanhf(x))

#define ctg(x) (1.0 / tan(x))

#define ctgh(x) (1.0 / tanh(x))

#define actg(x) (1.0 / atan(x))

#define actgh(x) (1.0 / atanh(x))

#define lerp(x, y, blend) ((1.0f - (blend)) * (x) + (blend) * (y))

extern void* force_malloc(size_t size);
#define force_malloc_s(s, size) force_malloc(sizeof(s) * (size))

extern int16_t load_reverse_endianness_int16_t(void* ptr);
extern uint16_t load_reverse_endianness_uint16_t(void* ptr);
extern int32_t load_reverse_endianness_int32_t(void* ptr);
extern uint32_t load_reverse_endianness_uint32_t(void* ptr);
extern int64_t load_reverse_endianness_int64_t(void* ptr);
extern uint64_t load_reverse_endianness_uint64_t(void* ptr);
extern ssize_t load_reverse_endianness_ssize_t(void* ptr);
extern size_t load_reverse_endianness_size_t(void* ptr);
extern float_t load_reverse_endianness_float_t(void* ptr);
extern double_t load_reverse_endianness_double_t(void* ptr);
extern void store_reverse_endianness_int16_t(int16_t value, void* ptr);
extern void store_reverse_endianness_uint16_t(uint16_t value, void* ptr);
extern void store_reverse_endianness_int32_t(int32_t value, void* ptr);
extern void store_reverse_endianness_uint32_t(uint32_t value, void* ptr);
extern void store_reverse_endianness_int64_t(int64_t value, void* ptr);
extern void store_reverse_endianness_uint64_t(uint64_t value, void* ptr);
extern void store_reverse_endianness_ssize_t(ssize_t value, void* ptr);
extern void store_reverse_endianness_size_t(size_t value, void* ptr);
extern void store_reverse_endianness_float_t(float_t value, void* ptr);
extern void store_reverse_endianness_double_t(double_t value, void* ptr);
extern int16_t reverse_endianness_int16_t(int16_t value);
extern uint16_t reverse_endianness_uint16_t(uint16_t value);
extern int32_t reverse_endianness_int32_t(int32_t value);
extern uint32_t reverse_endianness_uint32_t(uint32_t value);
extern int64_t reverse_endianness_int64_t(int64_t value);
extern uint64_t reverse_endianness_uint64_t(uint64_t value);
extern ssize_t reverse_endianness_ssize_t(ssize_t value);
extern size_t reverse_endianness_size_t(size_t value);
extern float_t reverse_endianness_float_t(float_t value);
extern double_t reverse_endianness_double_t(double_t value);

extern size_t utf8_length(char* s);
extern size_t utf16_length(wchar_t* s);
extern wchar_t* utf8_to_utf16(char* s);
extern char* utf16_to_utf8(wchar_t* s);
extern bool utf8_check_for_ascii_only(char* s);
extern size_t utf8_to_utf16_length(char* s);
extern size_t utf16_to_utf8_length(wchar_t* s);
