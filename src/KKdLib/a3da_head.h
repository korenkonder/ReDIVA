/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "kf.h"
#include "auth_3d.h"

typedef struct a3da_head {
    auth_3d_key_type type;
    float_t value;
    float_t max_frames;
    auth_3d_ep_type ep_type_pre;
    auth_3d_ep_type ep_type_post;
    float_t frame_delta;
    float_t value_delta;
    int32_t padding;
    kft3* first_key;
    kft3* second_key;
    kft3* after_last_key;
    size_t length;
    kft3* keys; //Used only in-game and points to KFT3 Array
} auth_3d_struct;

extern auth_3d_struct* auth_3d_key_to_auth_3d_struct(auth_3d_key* k);
