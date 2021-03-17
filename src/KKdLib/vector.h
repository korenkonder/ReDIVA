/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

#define vector(t) \
typedef struct vector_##t { \
    t* begin; \
    t* end; \
    t* capacity_end; \
} vector_##t; \
\
extern void FASTCALL vector_##t##_push_back(vector_##t* vec, t* element); \
extern void FASTCALL vector_##t##_append(vector_##t* vec, uint64_t size); \
extern void FASTCALL vector_##t##_resize(vector_##t* vec, uint64_t size); \
extern void FASTCALL vector_##t##_erase(vector_##t* vec, uint64_t index); \
extern void FASTCALL vector_##t##_clear(vector_##t* vec); \
extern void FASTCALL vector_##t##_free(vector_##t* vec);

#define vector_ptr(t) \
typedef struct vector_ptr_##t { \
    t** begin; \
    t** end; \
    t** capacity_end; \
} vector_ptr_##t; \
\
extern void FASTCALL vector_ptr_##t##_push_back(vector_ptr_##t* vec, t** element); \
extern void FASTCALL vector_ptr_##t##_append(vector_ptr_##t* vec, uint64_t size); \
extern void FASTCALL vector_ptr_##t##_resize(vector_ptr_##t* vec, uint64_t size); \
extern void FASTCALL vector_ptr_##t##_erase(vector_ptr_##t* vec, uint64_t index); \
extern void FASTCALL vector_ptr_##t##_clear(vector_ptr_##t* vec, \
    void* FASTCALL dispose_func(void* data)); \
extern void FASTCALL vector_ptr_##t##_free(vector_ptr_##t* vec, \
    void* FASTCALL dispose_func(void* data));

#define vector_func(t) \
void FASTCALL vector_##t##_push_back(vector_##t* vec, t* element) { \
    int64_t old_length; \
\
    if (element >= vec->end || vec->begin > element) { \
        if (vec->end == vec->capacity_end) \
            vector_##t##_append(vec, 1); \
\
        if (vec->end) \
            *vec->end = *element; \
    } \
    else { \
        old_length = element - vec->begin; \
        if (vec->end == vec->capacity_end) \
            vector_##t##_append(vec, 1); \
\
        if (vec->end) \
            *vec->end = vec->begin[old_length]; \
    } \
    vec->end++; \
} \
\
void FASTCALL vector_##t##_append(vector_##t* vec, uint64_t size) { \
    int64_t length; \
    uint64_t capacity; \
    uint64_t temp_length; \
\
    if (vec->capacity_end - vec->end < (int64_t)size) { \
        length = vec->end - vec->begin; \
        capacity = vec->capacity_end - vec->begin; \
        if (UINT64_MAX / sizeof(t) - (capacity >> 1) >= capacity) \
            temp_length = (capacity >> 1) + capacity; \
        else \
            temp_length = 0; \
\
        if (temp_length < length + size) \
            temp_length = length + size; \
        vector_##t##_resize(vec, temp_length); \
    } \
} \
\
void FASTCALL vector_##t##_resize(vector_##t* vec, uint64_t size) { \
    t* temp; \
    int64_t length; \
\
    temp = 0; \
    if (size) \
        temp = force_malloc_s(sizeof(t), size); \
\
    if (!temp) \
        return; \
\
    memmove(temp, vec->begin, sizeof(t) * (vec->end - vec->begin)); \
    length = vec->end - vec->begin; \
    free(vec->begin); \
    vec->capacity_end = &temp[size]; \
    vec->end = &temp[length]; \
    vec->begin = temp; \
} \
\
void FASTCALL vector_##t##_erase(vector_##t* vec, uint64_t index) { \
    memmove(&vec->begin[index], &vec->begin[index + 1], sizeof(t*) * (vec->end - &vec->begin[index + 1])); \
    vec->end--; \
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
}

#define vector_ptr_func(t) \
void FASTCALL vector_ptr_##t##_push_back(vector_ptr_##t* vec, t** element) { \
    int64_t old_length; \
\
    if (element >= vec->end || vec->begin > element) { \
        if (vec->end == vec->capacity_end) \
            vector_ptr_##t##_append(vec, 1); \
\
        if (vec->end) \
            *vec->end = *element; \
    } \
    else { \
        old_length = element - vec->begin; \
        if (vec->end == vec->capacity_end) \
            vector_ptr_##t##_append(vec, 1); \
\
        if (vec->end) \
            *vec->end = vec->begin[old_length]; \
    } \
    vec->end++; \
} \
\
void FASTCALL vector_ptr_##t##_append(vector_ptr_##t* vec, uint64_t size) { \
    int64_t length; \
    uint64_t capacity; \
    uint64_t temp_length; \
\
    if (vec->capacity_end - vec->end < (int64_t)size) { \
        length = vec->end - vec->begin; \
        capacity = vec->capacity_end - vec->begin; \
        if (UINT64_MAX / sizeof(t*) - (capacity >> 1) >= capacity) \
            temp_length = (capacity >> 1) + capacity; \
        else \
            temp_length = 0; \
\
        if (temp_length < length + size) \
            temp_length = length + size; \
        vector_ptr_##t##_resize(vec, temp_length); \
    } \
} \
\
void FASTCALL vector_ptr_##t##_resize(vector_ptr_##t* vec, uint64_t size) { \
    t** temp; \
    int64_t length; \
\
    temp = 0; \
    if (size) \
        temp = force_malloc_s(sizeof(t*), size); \
\
    if (!temp) \
        return; \
\
    memmove(temp, vec->begin, sizeof(t*) * (vec->end - vec->begin)); \
    length = vec->end - vec->begin; \
    free(vec->begin); \
    vec->capacity_end = &temp[size]; \
    vec->end = &temp[length]; \
    vec->begin = temp; \
} \
\
void FASTCALL vector_ptr_##t##_erase(vector_ptr_##t* vec, uint64_t index) { \
    memmove(&vec->begin[index], &vec->begin[index + 1], sizeof(t**) * (vec->end - &vec->begin[index + 1])); \
    vec->end--; \
} \
\
void FASTCALL vector_ptr_##t##_clear(vector_ptr_##t* vec, \
    void* FASTCALL dispose_func(void* data)) { \
    t** i; \
\
    if (dispose_func) \
        for (i = vec->begin; i != vec->end; i++) \
            if (*i) { \
                dispose_func(*i); \
                *i = 0; \
            } \
    else \
        for (i = vec->begin; i != vec->end; i++) \
            free(*i); \
    vec->end = vec->begin; \
} \
\
void FASTCALL vector_ptr_##t##_free(vector_ptr_##t* vec, \
    void* FASTCALL dispose_func(void* data)) { \
    t** i; \
\
    if (dispose_func) \
        for (i = vec->begin; i != vec->end; i++) \
            if (*i) { \
                dispose_func(*i); \
                *i = 0; \
            } \
    else \
        for (i = vec->begin; i != vec->end; i++) \
            free(*i); \
\
    free(vec->begin); \
    vec->begin = 0; \
    vec->end = 0; \
    vec->capacity_end = 0; \
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
