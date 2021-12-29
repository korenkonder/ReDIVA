/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"

typedef enum kf_type {
    KEY_FRAME_TYPE_0 = 0,
    KEY_FRAME_TYPE_1 = 1,
    KEY_FRAME_TYPE_2 = 2,
    KEY_FRAME_TYPE_3 = 3,
} kf_type;

typedef struct kft0 {
    float_t frame;
} kft0;

typedef struct kft1 {
    float_t frame;
    float_t value;
} kft1;

typedef struct kft2 {
    float_t frame;
    float_t value;
    float_t tangent;
} kft2;

typedef struct kft3 {
    float_t frame;
    float_t value;
    float_t tangent1;
    float_t tangent2;
} kft3;

vector(kft0)
vector(kft1)
vector(kft2)
vector(kft3)
vector_ptr(kft0)
vector_ptr(kft1)
vector_ptr(kft2)
vector_ptr(kft3)

extern void kft_check(void* src_key, kf_type src_type, void* dst_key, kf_type* dst_type);
