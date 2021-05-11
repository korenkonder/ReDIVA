/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Code is from web. I don't really remember wnere I got it
*/

#include "sort.h"

#define RADIX 8

inline int32_t digit(int32_t n, size_t k, size_t m) {
    return (int32_t)(((ssize_t)n >> (RADIX * k) & (m - 1)));
}

void radix_sort(int32_t* array, size_t length) {
    const size_t k = (32 + RADIX - 1) / RADIX;
    const size_t m = (size_t)1 << RADIX;
    const size_t s = sizeof(int32_t);
    int32_t* l = array;
    int32_t* r = l + length;
    int32_t* b = force_malloc_s(s, length);
    int32_t* c = force_malloc_s(s, m);
    int32_t t;
    for (size_t i = 0; i < k; i++) {
        memset(c, 0, m * s);

        for (int32_t* j = l; j < r; j++)
            c[digit(*j, i, m)]++;

        t = c[0];
        for (size_t j = 1; j < m; j++)
            t = c[j] += t;

        for (int32_t* j = r - 1; j >= l; j--)
            b[--c[digit(*j, i, m)]] = *j;

        size_t cur = 0;
        for (int32_t* j = l; j < r; j++)
            *j = b[cur++];
    }
    free(b);
    free(c);
}
