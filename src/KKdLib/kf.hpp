/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"

enum kf_type {
    KEY_FRAME_TYPE_0 = 0,
    KEY_FRAME_TYPE_1 = 1,
    KEY_FRAME_TYPE_2 = 2,
    KEY_FRAME_TYPE_3 = 3,
};

struct kft0 {
    float_t frame;
};

struct kft1 {
    float_t frame;
    float_t value;
};

struct kft2 {
    float_t frame;
    float_t value;
    float_t slope;
};

struct kft3 {
    float_t frame;
    float_t value;
    float_t l_slope;
    float_t r_slope;
};

extern void kft_check(void* src_key, kf_type src_type, void* dst_key, kf_type* dst_type);
