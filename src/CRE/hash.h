/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef struct hash {
    uint64_t f;
    uint32_t m;
} hash;

#define hash_dat(t) \
typedef struct hash_##t { \
    hash hash; \
    t data; \
} hash_##t;

#define hash_ptr(t) \
typedef struct hash_ptr_##t { \
    hash hash; \
    t* data; \
} hash_ptr_##t;

#define HASH_COMPARE(a, b) \
(((a).f == (b).f) || ((a).m == (b).m))

extern hash hash_char(char* data);
extern hash hash_wchar_t(wchar_t* data);
extern hash hash_data(void* data, size_t length);
