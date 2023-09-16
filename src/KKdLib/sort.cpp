/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Code is from web. I don't really remember wnere I got it
*/

#include "sort.hpp"
#include <stdlib.h>
#include "str_utils.hpp"

#define RADIX_BASE 8
#define RADIX (1 << RADIX_BASE)

static int quicksort_int8_t_compare(void const* src1, void const* src2);
static int quicksort_uint8_t_compare(void const* src1, void const* src2);
static int quicksort_int16_t_compare(void const* src1, void const* src2);
static int quicksort_uint16_t_compare(void const* src1, void const* src2);
static int quicksort_int32_t_compare(void const* src1, void const* src2);
static int quicksort_uint32_t_compare(void const* src1, void const* src2);
static int quicksort_int64_t_compare(void const* src1, void const* src2);
static int quicksort_uint64_t_compare(void const* src1, void const* src2);
static int quicksort_ssize_t_compare(void const* src1, void const* src2);
static int quicksort_size_t_compare(void const* src1, void const* src2);
static int quicksort_char_ptr_compare(void const* src1, void const* src2);
static int quicksort_wchar_t_ptr_compare(void const* src1, void const* src2);
static int quicksort_std_string_compare(void const* src1, void const* src2);
static int quicksort_std_wstring_compare(void const* src1, void const* src2);
static int quicksort_string_hash_compare(void const* src1, void const* src2);

void radix_sort_int8_t(int8_t* arr, size_t n) {
    if (n <= 1)
        return;

    for (size_t i = 0; i < n; i++)
        arr[i] = radix_preprocess_int8_t(arr[i]);
    radix_sort_uint8_t((uint8_t*)arr, n);
    for (size_t i = 0; i < n; i++)
        arr[i] = radix_postprocess_int8_t(arr[i]);
}

void radix_sort_uint8_t(uint8_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t c[RADIX];
    uint8_t* o = force_malloc<uint8_t>(n);
    uint8_t* org_arr = arr;

    for (size_t shift = 0, s = 0; shift < sizeof(uint8_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < n; i++)
            c[(arr[i] >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = n - 1; i >= 0; i--)
            o[--c[(arr[i] >> s) & (RADIX - 1)]] = arr[i];

        uint8_t* t = arr;
        arr = o;
        o = t;
    }

    if (org_arr == o) {
        uint8_t* t = arr;
        arr = o;
        o = t;

        memmove(arr, o, sizeof(uint8_t) * n);
    }

    free_def(o);
}

void radix_sort_int16_t(int16_t* arr, size_t n) {
    if (n <= 1)
        return;

    for (size_t i = 0; i < n; i++)
        arr[i] = radix_preprocess_int16_t(arr[i]);
    radix_sort_uint16_t((uint16_t*)arr, n);
    for (size_t i = 0; i < n; i++)
        arr[i] = radix_postprocess_int16_t(arr[i]);
}

void radix_sort_uint16_t(uint16_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t c[RADIX];
    uint16_t* o = force_malloc<uint16_t>(n);
    uint16_t* org_arr = arr;

    for (size_t shift = 0, s = 0; shift < sizeof(uint16_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < n; i++)
            c[(arr[i] >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = n - 1; i >= 0; i--)
            o[--c[(arr[i] >> s) & (RADIX - 1)]] = arr[i];

        uint16_t* t = arr;
        arr = o;
        o = t;
    }

    if (org_arr == o) {
        uint16_t* t = arr;
        arr = o;
        o = t;

        memmove(arr, o, sizeof(uint16_t) * n);
    }

    free_def(o);
}

void radix_sort_int32_t(int32_t* arr, size_t n) {
    if (n <= 1)
        return;

    for (size_t i = 0; i < n; i++)
        arr[i] = radix_preprocess_int32_t(arr[i]);
    radix_sort_uint32_t((uint32_t*)arr, n);
    for (size_t i = 0; i < n; i++)
        arr[i] = radix_postprocess_int32_t(arr[i]);
}

void radix_sort_uint32_t(uint32_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t c[RADIX];
    uint32_t* o = force_malloc<uint32_t>(n);
    uint32_t* org_arr = arr;

    for (size_t shift = 0, s = 0; shift < sizeof(uint32_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < n; i++)
            c[(arr[i] >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = n - 1; i >= 0; i--)
            o[--c[(arr[i] >> s) & (RADIX - 1)]] = arr[i];

        uint32_t* t = arr;
        arr = o;
        o = t;
    }

    if (org_arr == o) {
        uint32_t* t = arr;
        arr = o;
        o = t;

        memmove(arr, o, sizeof(uint32_t) * n);
    }

    free_def(o);
}

void radix_sort_int64_t(int64_t* arr, size_t n) {
    if (n <= 1)
        return;

    for (size_t i = 0; i < n; i++)
        arr[i] = radix_preprocess_int64_t(arr[i]);
    radix_sort_uint64_t((uint64_t*)arr, n);
    for (size_t i = 0; i < n; i++)
        arr[i] = radix_postprocess_int64_t(arr[i]);
}

void radix_sort_uint64_t(uint64_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t c[RADIX];
    uint64_t* o = force_malloc<uint64_t>(n);
    uint64_t* org_arr = arr;

    for (size_t shift = 0, s = 0; shift < sizeof(uint64_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < n; i++)
            c[(arr[i] >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = n - 1; i >= 0; i--)
            o[--c[(arr[i] >> s) & (RADIX - 1)]] = arr[i];

        uint64_t* t = arr;
        arr = o;
        o = t;
    }

    if (org_arr == o) {
        uint64_t* t = arr;
        arr = o;
        o = t;

        memmove(arr, o, sizeof(uint64_t) * n);
    }

    free_def(o);
}

void radix_sort_ssize_t(ssize_t* arr, size_t n) {
    if (n <= 1)
        return;

    for (size_t i = 0; i < n; i++)
        arr[i] = radix_preprocess_ssize_t(arr[i]);
    radix_sort_size_t((size_t*)arr, n);
    for (size_t i = 0; i < n; i++)
        arr[i] = radix_postprocess_ssize_t(arr[i]);
}

void radix_sort_size_t(size_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t c[RADIX];
    size_t* o = force_malloc<size_t>(n);
    size_t* org_arr = arr;

    for (size_t shift = 0, s = 0; shift < sizeof(size_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < n; i++)
            c[(arr[i] >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = n - 1; i >= 0; i--)
            o[--c[(arr[i] >> s) & (RADIX - 1)]] = arr[i];

        size_t* t = arr;
        arr = o;
        o = t;
    }

    if (org_arr == o) {
        size_t* t = arr;
        arr = o;
        o = t;

        memmove(arr, o, sizeof(size_t) * n);
    }

    free_def(o);
}

void radix_sort_custom(void* arr, size_t n, size_t size, size_t idx_len, radix_index_func idx_func) {
    if (n <= 1)
        return;

    size_t c[RADIX];
    uint8_t* o = force_malloc<uint8_t>(size * n);
    uint8_t* a = (uint8_t*)arr;
    void* org_arr = arr;

    for (size_t shift = 0, s = 0; shift < idx_len * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < n; i++)
            c[(idx_func(a, i) >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = n - 1; i >= 0; i--) {
            size_t index = --c[(idx_func(a, i) >> s) & (RADIX - 1)];
            memmove(o + size * index, a + size * i, size);
        }

        uint8_t* t = a;
        a = o;
        o = t;
    }

    if (org_arr == o) {
        uint8_t* t = a;
        a = o;
        o = t;

        memmove(a, o, size * n);
    }

    free_def(o);
}

void quicksort_int8_t(int8_t* arr, size_t n) {
    qsort(arr, n, sizeof(int8_t), quicksort_int8_t_compare);
}

void quicksort_uint8_t(uint8_t* arr, size_t n) {
    qsort(arr, n, sizeof(uint8_t), quicksort_uint8_t_compare);
}

void quicksort_int16_t(int16_t* arr, size_t n) {
    qsort(arr, n, sizeof(int16_t), quicksort_int16_t_compare);
}

void quicksort_uint16_t(uint16_t* arr, size_t n) {
    qsort(arr, n, sizeof(uint16_t), quicksort_uint16_t_compare);
}

void quicksort_int32_t(int32_t* arr, size_t n) {
    qsort(arr, n, sizeof(int32_t), quicksort_int32_t_compare);
}

void quicksort_uint32_t(uint32_t* arr, size_t n) {
    qsort(arr, n, sizeof(uint32_t), quicksort_uint32_t_compare);
}

void quicksort_int64_t(int64_t* arr, size_t n) {
    qsort(arr, n, sizeof(int64_t), quicksort_int64_t_compare);
}

void quicksort_uint64_t(uint64_t* arr, size_t n) {
    qsort(arr, n, sizeof(uint64_t), quicksort_uint64_t_compare);
}

void quicksort_ssize_t(ssize_t* arr, size_t n) {
    qsort(arr, n, sizeof(ssize_t), quicksort_ssize_t_compare);
}

void quicksort_size_t(size_t* arr, size_t n) {
    qsort(arr, n, sizeof(size_t), quicksort_size_t_compare);
}

void quicksort_char_ptr(char** arr, size_t n) {
    qsort(arr, n, sizeof(char*), quicksort_char_ptr_compare);
}

void quicksort_wchar_t_ptr(wchar_t** arr, size_t n) {
    qsort(arr, n, sizeof(wchar_t*), quicksort_wchar_t_ptr_compare);
}

void quicksort_string(std::string* arr, size_t n) {
    qsort(arr, n, sizeof(std::string), quicksort_std_string_compare);
}

void quicksort_wstring(std::wstring* arr, size_t n) {
    qsort(arr, n, sizeof(std::wstring), quicksort_std_wstring_compare);
}

void quicksort_string_hash(string_hash* arr, size_t n) {
    qsort(arr, n, sizeof(string_hash), quicksort_string_hash_compare);
}

void quicksort_custom(void* arr, size_t n, size_t s, quicksort_compare_func comp_finc) {
    qsort(arr, n, s, comp_finc);
}

static int quicksort_int8_t_compare(void const* src1, void const* src2) {
    int8_t s1 = *(int8_t*)src1;
    int8_t s2 = *(int8_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_uint8_t_compare(void const* src1, void const* src2) {
    uint8_t s1 = *(uint8_t*)src1;
    uint8_t s2 = *(uint8_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_int16_t_compare(void const* src1, void const* src2) {
    int16_t s1 = *(int16_t*)src1;
    int16_t s2 = *(int16_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_uint16_t_compare(void const* src1, void const* src2) {
    uint16_t s1 = *(uint16_t*)src1;
    uint16_t s2 = *(uint16_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_int32_t_compare(void const* src1, void const* src2) {
    int32_t s1 = *(int32_t*)src1;
    int32_t s2 = *(int32_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_uint32_t_compare(void const* src1, void const* src2) {
    uint32_t s1 = *(uint32_t*)src1;
    uint32_t s2 = *(uint32_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_int64_t_compare(void const* src1, void const* src2) {
    int64_t s1 = *(int64_t*)src1;
    int64_t s2 = *(int64_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_uint64_t_compare(void const* src1, void const* src2) {
    uint64_t s1 = *(uint64_t*)src1;
    uint64_t s2 = *(uint64_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_ssize_t_compare(void const* src1, void const* src2) {
    ssize_t s1 = *(ssize_t*)src1;
    ssize_t s2 = *(ssize_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_size_t_compare(void const* src1, void const* src2) {
    size_t s1 = *(ssize_t*)src1;
    size_t s2 = *(ssize_t*)src2;
    return s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
}

static int quicksort_char_ptr_compare(void const* src1, void const* src2) {
    char* str1 = *(char**)src1;
    char* str2 = *(char**)src2;
    return str_utils_compare(str1, str2);
}

static int quicksort_wchar_t_ptr_compare(void const* src1, void const* src2) {
    wchar_t* str1 = *(wchar_t**)src1;
    wchar_t* str2 = *(wchar_t**)src2;
    return str_utils_compare(str1, str2);
}

static int quicksort_std_string_compare(void const* src1, void const* src2) {
    std::string* str1 = (std::string*)src1;
    std::string* str2 = (std::string*)src2;
    return str1->compare(*str2);
}

static int quicksort_std_wstring_compare(void const* src1, void const* src2) {
    std::wstring* str1 = (std::wstring*)src1;
    std::wstring* str2 = (std::wstring*)src2;
    return str1->compare(*str2);
}

static int quicksort_string_hash_compare(void const* src1, void const* src2) {
    string_hash* str1 = (string_hash*)src1;
    string_hash* str2 = (string_hash*)src2;
    return str1->str.compare(str2->str);
}
