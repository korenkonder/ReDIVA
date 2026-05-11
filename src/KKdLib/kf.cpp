/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "kf.hpp"

void kft_check(void* src_key, kf_type src_type, void* dst_key, kf_type* dst_type) {
    switch (src_type) {
    case KEY_FRAME_TYPE_0: {
        kft0* sk = (kft0*)src_key;
        kft0* dk = (kft0*)dst_key;
        dk->frame = sk->frame;
        *dst_type = KEY_FRAME_TYPE_1;
    } break;
    case KEY_FRAME_TYPE_1: {
        kft1* sk = (kft1*)src_key;
        if (*(uint32_t*)&sk->value != 0) {
            kft1* dk = (kft1*)dst_key;
            dk->frame = sk->frame;
            dk->value = sk->value;
            *dst_type = KEY_FRAME_TYPE_1;
        }
        else {
            kft0* dk = (kft0*)dst_key;
            dk->frame = sk->frame;
            *dst_type = KEY_FRAME_TYPE_0;
        }
    } break;
    case KEY_FRAME_TYPE_2: {
        kft2* sk = (kft2*)src_key;
        if (*(uint32_t*)&sk->slope != 0) {
            kft2* dk = (kft2*)dst_key;
            dk->frame = sk->frame;
            dk->value = sk->value;
            dk->slope = sk->slope;
            *dst_type = KEY_FRAME_TYPE_2;
        }
        else if (*(uint32_t*)&sk->value != 0) {
            kft1* dk = (kft1*)dst_key;
            dk->frame = sk->frame;
            dk->value = sk->value;
            *dst_type = KEY_FRAME_TYPE_1;
        }
        else {
            kft0* dk = (kft0*)dst_key;
            dk->frame = sk->frame;
            *dst_type = KEY_FRAME_TYPE_0;
        }
    } break;
    case KEY_FRAME_TYPE_3: {
        kft3* sk = (kft3*)src_key;
        if (*(uint32_t*)&sk->l_slope != *(uint32_t*)&sk->r_slope) {
            kft3* dk = (kft3*)dst_key;
            dk->frame = sk->frame;
            dk->value = sk->value;
            dk->l_slope = sk->l_slope;
            dk->r_slope = sk->r_slope;
            *dst_type = KEY_FRAME_TYPE_3;
        }
        else if (*(uint32_t*)&sk->l_slope != 0) {
            kft2* dk = (kft2*)dst_key;
            dk->frame = sk->frame;
            dk->value = sk->value;
            dk->slope = sk->l_slope;
            *dst_type = KEY_FRAME_TYPE_2;
        }
        else if (*(uint32_t*)&sk->value != 0) {
            kft1* dk = (kft1*)dst_key;
            dk->frame = sk->frame;
            dk->value = sk->value;
            *dst_type = KEY_FRAME_TYPE_1;
        }
        else {
            kft0* dk = (kft0*)dst_key;
            dk->frame = sk->frame;
            *dst_type = KEY_FRAME_TYPE_0;
        }
    } break;
    }
}
