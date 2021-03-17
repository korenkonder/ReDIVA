/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

#define hash(t) \
typedef struct hash_##t { \
    uint64_t hash; \
    t data; \
} hash_##t;

#define hash_ptr(t) \
typedef struct hash_ptr_##t { \
    uint64_t hash; \
    t* data; \
} hash_ptr_##t;

extern uint64_t hash_char(char* data);
extern uint64_t hash_wchar_t(wchar_t* data);
extern uint64_t hash_data(void* data, size_t length);
