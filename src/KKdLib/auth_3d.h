/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"
#include "kf.h"
#include "vec.h"

typedef enum auth_3d_compress_f16 {
    AUTH_3D_COMPRESS_F32F32F32F32 = 0,
    AUTH_3D_COMPRESS_I16F16F32F32 = 1,
    AUTH_3D_COMPRESS_I16F16F16F16 = 2,
} auth_3d_compress_f16;

typedef enum auth_3d_ep_type {
    AUTH_3D_EP_TYPE_NONE         = 0,
    AUTH_3D_EP_TYPE_LINEAR       = 1,
    AUTH_3D_EP_TYPE_CYCLE        = 2,
    AUTH_3D_EP_TYPE_CYCLE_OFFSET = 3,
} auth_3d_ep_type; // Pre/Post Infinity

typedef enum auth_3d_key_type {
    AUTH_3D_KEY_TYPE_NONE    = 0,
    AUTH_3D_KEY_TYPE_STATIC  = 1,
    AUTH_3D_KEY_TYPE_LINEAR  = 2,
    AUTH_3D_KEY_TYPE_HERMITE = 3,
    AUTH_3D_KEY_TYPE_HOLD    = 4,
} auth_3d_key_type;

typedef enum auth_3d_post_process_flags {
    AUTH_3D_POST_PROCESS_LENS_FLARE = 0x01,
    AUTH_3D_POST_PROCESS_LENS_SHAFT = 0x02,
    AUTH_3D_POST_PROCESS_LENS_GHOST = 0x04,
    AUTH_3D_POST_PROCESS_RADIUS     = 0x08,
    AUTH_3D_POST_PROCESS_INTENSITY  = 0x10,
    AUTH_3D_POST_PROCESS_SCENE_FADE = 0x20,
} auth_3d_post_process_flags;

typedef struct auth_3d_key_raw_data {
    int32_t key_type;
    char* value_type;
    char** value_list;
    int32_t value_list_size;
} auth_3d_key_raw_data;

typedef struct auth_3d_key_head {
    auth_3d_key_type type : 8;
    auth_3d_ep_type ep_type_pre : 4;
    auth_3d_ep_type ep_type_post : 4;
    uint32_t padding : 16;
    float_t value;
    float_t max;
    uint32_t length;
} auth_3d_key_head;

typedef struct auth_3d_key {
    auth_3d_key_type type;
    size_t length;
    size_t bin_offset;
    auth_3d_ep_type ep_type_pre;
    auth_3d_ep_type ep_type_post;
    float_t max;
    float_t value;
    auth_3d_key_raw_data raw_data;
    kft3* keys;
} auth_3d_key;

v3(auth_3d_key)
v4(auth_3d_key)
