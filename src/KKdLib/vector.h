/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "half_t.h"

#define vector_dispose_func(t) void(FASTCALL* dispose_func)(t* data)

#define vector(t) \
typedef struct vector_##t { \
    t* begin; \
    t* end; \
    t* capacity_end; \
} vector_##t; \
\
extern void FASTCALL vector_##t##_append(vector_##t* vec, ssize_t count); \
extern void FASTCALL vector_##t##_push_back(vector_##t* vec, t* val); \
extern void FASTCALL vector_##t##_pop_back(vector_##t* vec); \
extern void FASTCALL vector_##t##_insert(vector_##t* vec, ssize_t position, t* val); \
extern void FASTCALL vector_##t##_insert_range(vector_##t* vec, ssize_t position, t* first, t* last); \
extern void FASTCALL vector_##t##_erase(vector_##t* vec, ssize_t position); \
extern void FASTCALL vector_##t##_erase_range(vector_##t* vec, ssize_t first, ssize_t last); \
extern void FASTCALL vector_##t##_swap(vector_##t* vec1, vector_##t* vec2); \
extern void FASTCALL vector_##t##_clear(vector_##t* vec); \
extern void FASTCALL vector_##t##_free(vector_##t* vec);

#define vector_ptr(t) \
typedef struct vector_ptr_##t { \
    t** begin; \
    t** end; \
    t** capacity_end; \
} vector_ptr_##t; \
\
extern void FASTCALL vector_ptr_##t##_append(vector_ptr_##t* vec, ssize_t count); \
extern void FASTCALL vector_ptr_##t##_push_back(vector_ptr_##t* vec, t** val); \
extern void FASTCALL vector_ptr_##t##_pop_back(vector_ptr_##t* vec, vector_dispose_func(t)); \
extern void FASTCALL vector_ptr_##t##_insert(vector_ptr_##t* vec, ssize_t position, t** val); \
extern void FASTCALL vector_ptr_##t##_insert_range(vector_ptr_##t* vec, ssize_t position, t** first, t** last); \
extern void FASTCALL vector_ptr_##t##_erase(vector_ptr_##t* vec, \
    ssize_t position, vector_dispose_func(t)); \
extern void FASTCALL vector_ptr_##t##_erase_range(vector_ptr_##t* vec, \
    ssize_t first, ssize_t last, vector_dispose_func(t)); \
extern void FASTCALL vector_ptr_##t##_swap(vector_ptr_##t* vec1, vector_ptr_##t* vec2); \
extern void FASTCALL vector_ptr_##t##_clear(vector_ptr_##t* vec, vector_dispose_func(t)); \
extern void FASTCALL vector_ptr_##t##_free(vector_ptr_##t* vec, vector_dispose_func(t));

#define vector_func(t) \
void FASTCALL vector_##t##_push_back(vector_##t* vec, t* val) { \
    if (!vec || !val) \
        return; \
\
    if (vec->capacity_end - vec->end < 1) \
        vector_##t##_append(vec, 1); \
\
    *vec->end = *val; \
    vec->end++; \
} \
\
void FASTCALL vector_##t##_pop_back(vector_##t* vec) { \
    if (!vec) \
        return; \
\
    if (vec->end - vec->begin > 0) \
        vec->end--; \
} \
\
void FASTCALL vector_##t##_insert(vector_##t* vec, ssize_t position, t* val) { \
    if (!vec || !val || vec->end - vec->begin < position) \
        return; \
\
    if (vec->capacity_end - vec->end < 1) \
        vector_##t##_append(vec, 1); \
\
    t* t0 = vec->begin + position; \
    t* t1 = vec->begin + position + 1; \
    memmove(t1, t0, sizeof(t) * (vec->end - t0)); \
    *t0 = *val; \
    vec->end++; \
} \
\
void FASTCALL vector_##t##_insert_range(vector_##t* vec, ssize_t position, t* first, t* last) { \
    if (!vec || !first || !last || vec->end - vec->begin < position) \
        return; \
\
    ssize_t s = last - first; \
    if (s < 1) \
        return; \
\
    if (vec->capacity_end - vec->end < s) \
        vector_##t##_append(vec, s); \
    \
    t* t0 = vec->begin + position; \
    t* t1 = vec->begin + position + s; \
    memmove(t1, t0, sizeof(t) * (vec->end - t0)); \
    memmove(t0, first, sizeof(t) * s); \
    vec->end += s; \
} \
\
void FASTCALL vector_##t##_erase(vector_##t* vec, ssize_t position) { \
    if (!vec || vec->end - vec->begin <= position) \
        return; \
\
    t* t0 = vec->begin + position; \
    t* t1 = vec->begin + position + 1; \
    memmove(t0, t1, sizeof(t) * (vec->end - t1)); \
    vec->end--; \
} \
\
void FASTCALL vector_##t##_erase_range(vector_##t* vec, ssize_t first, ssize_t last) { \
    if (!vec || !last || last - first < 1) \
        return; \
\
    ssize_t s = min(last - first, (ssize_t)(vec->end - vec->begin)); \
    if (s < 1) \
        return; \
    \
    t* t0 = vec->begin + s; \
    t* t1 = vec->begin + s + 1; \
    memmove(t0, t1, sizeof(t) * (vec->end - t1)); \
    vec->end -= s; \
} \
\
void FASTCALL vector_##t##_swap(vector_##t* vec1, vector_##t* vec2) { \
    if (!vec1 || !vec2) \
        return; \
\
    vector_##t vec = *vec1; \
    *vec1 = *vec2; \
    *vec2 = vec; \
} \
\
void FASTCALL vector_##t##_clear(vector_##t* vec) { \
    vec->end = vec->begin; \
} \
\
void FASTCALL vector_##t##_free(vector_##t* vec) { \
    free(vec->begin); \
    vec->begin = 0; \
    vec->end = 0; \
    vec->capacity_end = 0; \
} \
\
void FASTCALL vector_##t##_append(vector_##t* vec, ssize_t count) { \
    if (vec->capacity_end - vec->end >= count) \
        return; \
\
    ssize_t length = vec->end - vec->begin; \
    ssize_t capacity = vec->capacity_end - vec->begin; \
    ssize_t _count; \
    if ((ssize_t)(INT64_MAX / sizeof(t) - (capacity >> 1)) >= capacity) \
        _count = (capacity >> 1) + capacity; \
    else \
        _count = 0; \
    count = max(_count, length + count); \
\
    t* temp = 0; \
    if (count) \
        temp = force_malloc_s(t, count); \
\
    if (!temp) \
        return; \
\
    memmove(temp, vec->begin, sizeof(t) * (vec->end - vec->begin)); \
    length = vec->end - vec->begin; \
    free(vec->begin); \
    vec->capacity_end = &temp[count]; \
    vec->end = &temp[length]; \
    vec->begin = temp; \
}

#define vector_ptr_func(t) \
void FASTCALL vector_ptr_##t##_push_back(vector_ptr_##t* vec, t** val) { \
    if (!vec || !val) \
        return; \
\
    if (vec->capacity_end - vec->end < 1) \
        vector_ptr_##t##_append(vec, 1); \
\
    *vec->end++ = *val; \
} \
\
void FASTCALL vector_ptr_##t##_pop_back(vector_ptr_##t* vec, vector_dispose_func(t)) { \
    if (!vec) \
        return; \
\
    if (vec->end - vec->begin > 0) { \
        if (dispose_func) \
            dispose_func(vec->end[1]); \
        else \
            free(vec->end[-1]); \
        vec->end--; \
    } \
} \
\
void FASTCALL vector_ptr_##t##_insert(vector_ptr_##t* vec, ssize_t position, t** val) { \
    if (!vec || !val || vec->end - vec->begin < position) \
        return; \
\
    if (vec->capacity_end - vec->end < 1) \
        vector_ptr_##t##_append(vec, 1); \
\
    t** t0 = vec->begin + position; \
    t** t1 = vec->begin + position + 1; \
    memmove(t1, t0, sizeof(t*) * (vec->end - t0)); \
    *t0 = *val; \
    vec->end++; \
} \
\
void FASTCALL vector_ptr_##t##_insert_range(vector_ptr_##t* vec, ssize_t position, t** first, t** last) { \
    if (!vec || !first || !last || vec->end - vec->begin < position) \
        return; \
\
    ssize_t s = last - first; \
    if (s < 1) \
        return; \
\
    if (vec->capacity_end - vec->end < s) \
        vector_ptr_##t##_append(vec, s); \
    \
    t** t0 = vec->begin + position; \
    t** t1 = vec->begin + position + s; \
    memmove(t1, t0, sizeof(t*) * (vec->end - t0)); \
    memmove(t0, first, sizeof(t*) * s); \
    vec->end += s; \
} \
\
void FASTCALL vector_ptr_##t##_erase(vector_ptr_##t* vec, \
    ssize_t position, vector_dispose_func(t)) { \
    if (!vec || vec->end - vec->begin <= position) \
        return; \
\
    t** t0 = vec->begin + position; \
    t** t1 = vec->begin + position + 1; \
    if (*t0) \
        if (dispose_func) \
            dispose_func(*t0); \
        else \
            free(*t0); \
\
    memmove(t0, t1, sizeof(t*) * (vec->end - t1)); \
    vec->end--; \
} \
\
void FASTCALL vector_ptr_##t##_erase_range(vector_ptr_##t* vec, \
    ssize_t first, ssize_t last, vector_dispose_func(t)) { \
    if (!vec || !last || last - first < 1) \
        return; \
\
    ssize_t s = min(last - first, (ssize_t)(vec->end - vec->begin)); \
    if (s < 1) \
        return; \
    \
    t** t0 = vec->begin + first; \
    t** t1 = vec->begin + first + s; \
    if (dispose_func) \
        for (ssize_t i = 0; i < s; i++) \
            if (t0[i]) { \
                dispose_func(t0[i]); \
                t0[i] = 0; \
            } \
    else \
        for (ssize_t i = 0; i < s; i++) \
            free(t0[i]); \
\
    memmove(t0, t1, sizeof(t*) * (vec->end - t1)); \
    vec->end -= s; \
} \
\
void FASTCALL vector_ptr_##t##_swap(vector_ptr_##t* vec1, vector_ptr_##t* vec2) { \
    if (!vec1 || !vec2) \
        return; \
\
    vector_ptr_##t vec = *vec1; \
    *vec1 = *vec2; \
    *vec2 = vec; \
} \
\
void FASTCALL vector_ptr_##t##_clear(vector_ptr_##t* vec, vector_dispose_func(t)) { \
    if (!vec) \
        return; \
\
    if (dispose_func) \
        for (t** i = vec->begin; i != vec->end; i++) \
            if (*i) { \
                dispose_func(*i); \
                *i = 0; \
            } \
    else \
        for (t** i = vec->begin; i != vec->end; i++) \
            free(*i); \
    vec->end = vec->begin; \
} \
\
void FASTCALL vector_ptr_##t##_free(vector_ptr_##t* vec, vector_dispose_func(t)) { \
    if (!vec) \
        return; \
\
    if (dispose_func) \
        for (t** i = vec->begin; i != vec->end; i++) \
            if (*i) { \
                dispose_func(*i); \
                *i = 0; \
            } \
    else \
        for (t** i = vec->begin; i != vec->end; i++) \
            free(*i); \
\
    free(vec->begin); \
    vec->begin = 0; \
    vec->end = 0; \
    vec->capacity_end = 0; \
} \
\
void FASTCALL vector_ptr_##t##_append(vector_ptr_##t* vec, ssize_t count) { \
    if (vec->capacity_end - vec->end >= count) \
        return; \
\
    ssize_t length = vec->end - vec->begin; \
    ssize_t capacity = vec->capacity_end - vec->begin; \
    ssize_t _count; \
    if ((ssize_t)(INT64_MAX / sizeof(t*) - (capacity >> 1)) >= capacity) \
        _count = (capacity >> 1) + capacity; \
    else \
        _count = 0; \
    count = max(_count, length + count); \
\
    t** temp = 0; \
    if (count) \
        temp = force_malloc_s(t*, count); \
\
    if (!temp) \
        return; \
\
    memmove(temp, vec->begin, sizeof(t*) * (vec->end - vec->begin)); \
    length = vec->end - vec->begin; \
    free(vec->begin); \
    vec->capacity_end = &temp[count]; \
    vec->end = &temp[length]; \
    vec->begin = temp; \
}

vector(char)
vector(wchar_t)
vector(bool)
vector(int8_t)
vector(uint8_t)
vector(int16_t)
vector(uint16_t)
vector(int32_t)
vector(uint32_t)
vector(int64_t)
vector(uint64_t)
vector(size_t)
vector(ssize_t)
vector(half_t)
vector(float_t)
vector(double_t)
vector_ptr(char)
vector_ptr(wchar_t)
vector_ptr(bool)
vector_ptr(int8_t)
vector_ptr(uint8_t)
vector_ptr(int16_t)
vector_ptr(uint16_t)
vector_ptr(int32_t)
vector_ptr(uint32_t)
vector_ptr(int64_t)
vector_ptr(uint64_t)
vector_ptr(size_t)
vector_ptr(ssize_t)
vector_ptr(half_t)
vector_ptr(float_t)
vector_ptr(double_t)
vector_ptr(void)
