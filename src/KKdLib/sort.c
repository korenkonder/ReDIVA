/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Code is from web. I don't really remember wnere I got it
*/

#include "sort.h"

#define RADIX_BASE 8
#define RADIX (1 << RADIX_BASE)

void radix_sort_int8_t(int8_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint8_t* a = (uint8_t*)arr;
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x80)
            a[i] |= 0x80;
        else
            a[i] = (uint8_t)(-(int8_t)a[i]);
    radix_sort_uint8_t(a, n);
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x80)
            a[i] &= ~0x80;
        else
            a[i] = (uint8_t)(-(int8_t)a[i]);
}

void radix_sort_uint8_t(uint8_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint8_t* o = force_malloc_s(uint8_t, n);
    size_t* c = force_malloc_s(size_t, RADIX);
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

    free(o);
    free(c);
}

void radix_sort_int16_t(int16_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint16_t* a = (uint16_t*)arr;
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x8000)
            a[i] |= 0x8000;
        else
            a[i] = (uint16_t)(-(int16_t)a[i]);
    radix_sort_uint16_t(a, n);
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x8000)
            a[i] &= ~0x8000;
        else
            a[i] = (uint16_t)(-(int16_t)a[i]);
}

void radix_sort_uint16_t(uint16_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint16_t* o = force_malloc_s(uint16_t, n);
    size_t* c = force_malloc_s(size_t, RADIX);
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

    free(o);
    free(c);
}

void radix_sort_int32_t(int32_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint32_t* a = (uint32_t*)arr;
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x80000000)
            a[i] |= 0x80000000;
        else
            a[i] = (uint32_t)(-(int32_t)a[i]);
    radix_sort_uint32_t(a, n);
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x80000000)
            a[i] &= ~0x80000000;
        else
            a[i] = (uint32_t)(-(int32_t)a[i]);
}

void radix_sort_uint32_t(uint32_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint32_t* o = force_malloc_s(uint32_t, n);
    size_t* c = force_malloc_s(size_t, RADIX);
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

    free(o);
    free(c);
}

void radix_sort_int64_t(int64_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint64_t* a = (uint64_t*)arr;
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x8000000000000000)
            a[i] |= 0x8000000000000000;
        else
            a[i] = (uint64_t)(-(int64_t)a[i]);
    radix_sort_uint64_t(a, n);
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x8000000000000000)
            a[i] &= ~0x8000000000000000;
        else
            a[i] = (uint64_t)(-(int64_t)a[i]);
}

void radix_sort_uint64_t(uint64_t* arr, size_t n) {
    if (n <= 1)
        return;

    uint64_t* o = force_malloc_s(uint64_t, n);
    size_t* c = force_malloc_s(size_t, RADIX);
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

    free(o);
    free(c);
}

void radix_sort_ssize_t(ssize_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t* a = (size_t*)arr;
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x8000000000000000)
            a[i] |= 0x8000000000000000;
        else
            a[i] = (size_t)(-(ssize_t)a[i]);
    radix_sort_size_t(a, n);
    for (size_t i = 0; i < n; i++)
        if (a[i] & 0x8000000000000000)
            a[i] &= ~0x8000000000000000;
        else
            a[i] = (size_t)(-(ssize_t)a[i]);
}

void radix_sort_size_t(size_t* arr, size_t n) {
    if (n <= 1)
        return;

    size_t* o = force_malloc_s(size_t, n);
    size_t* c = force_malloc_s(size_t, RADIX);
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

    free(o);
    free(c);
}
