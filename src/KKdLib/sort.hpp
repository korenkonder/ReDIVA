/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Code is from web. I don't really remember wnere I got it
*/

#pragma once

#include <string>
#include "default.hpp"
#include "hash.hpp"

typedef size_t(*radix_index_func)(void* data, size_t index);
typedef int(*quicksort_compare_func)(void const* src1, void const* src2);

#define radix_preprocess_int8_t(v) \
((int8_t)((uint8_t)(v) ^ 0x80u))

#define radix_postprocess_int8_t(v) \
((int8_t)((uint8_t)(v) ^ 0x80u))

#define radix_preprocess_int16_t(v) \
((int16_t)((uint16_t)(v) ^ 0x8000u))

#define radix_postprocess_int16_t(v) \
((int16_t)((uint16_t)(v) ^ 0x8000u))

#define radix_preprocess_int32_t(v) \
((int32_t)((uint32_t)(v) ^ 0x80000000u))

#define radix_postprocess_int32_t(v) \
((int32_t)((uint32_t)(v) ^ 0x80000000u))

#define radix_preprocess_int64_t(v) \
((int64_t)((uint64_t)(v) ^ 0x8000000000000000u))

#define radix_postprocess_int64_t(v) \
((int64_t)((uint64_t)(v) ^ 0x8000000000000000u))

#ifdef _WIN64
#define radix_preprocess_ssize_t(v) \
((ssize_t)((size_t)(v) ^ 0x8000000000000000u))

#define radix_postprocess_ssize_t(v) \
((ssize_t)((size_t)(v) ^ 0x8000000000000000u))
#else
#define radix_preprocess_ssize_t(v) \
((ssize_t)((size_t)(v) ^ 0x80000000u))

#define radix_postprocess_ssize_t(v) \
((ssize_t)((size_t)(v) ^ 0x80000000u))
#endif

extern void radix_sort_int8_t(int8_t* arr, size_t n);
extern void radix_sort_uint8_t(uint8_t* arr, size_t n);
extern void radix_sort_int16_t(int16_t* arr, size_t n);
extern void radix_sort_uint16_t(uint16_t* arr, size_t n);
extern void radix_sort_int32_t(int32_t* arr, size_t n);
extern void radix_sort_uint32_t(uint32_t* arr, size_t n);
extern void radix_sort_int64_t(int64_t* arr, size_t n);
extern void radix_sort_uint64_t(uint64_t* arr, size_t n);
extern void radix_sort_ssize_t(ssize_t* arr, size_t n);
extern void radix_sort_size_t(size_t* arr, size_t n);
extern void radix_sort_custom(void* arr, size_t n, size_t s, size_t idx_len, radix_index_func idx_func);

extern void quicksort_int8_t(int8_t* arr, size_t n);
extern void quicksort_uint8_t(uint8_t* arr, size_t n);
extern void quicksort_int16_t(int16_t* arr, size_t n);
extern void quicksort_uint16_t(uint16_t* arr, size_t n);
extern void quicksort_int32_t(int32_t* arr, size_t n);
extern void quicksort_uint32_t(uint32_t* arr, size_t n);
extern void quicksort_int64_t(int64_t* arr, size_t n);
extern void quicksort_uint64_t(uint64_t* arr, size_t n);
extern void quicksort_ssize_t(ssize_t* arr, size_t n);
extern void quicksort_size_t(size_t* arr, size_t n);
extern void quicksort_char_ptr(char** arr, size_t n);
extern void quicksort_wchar_t_ptr(wchar_t** arr, size_t n);
extern void quicksort_string(std::string* arr, size_t n);
extern void quicksort_wstring(std::wstring* arr, size_t n);
extern void quicksort_string_hash(string_hash* arr, size_t n);
extern void quicksort_custom(void* arr, size_t n, size_t s, quicksort_compare_func comp_finc);
