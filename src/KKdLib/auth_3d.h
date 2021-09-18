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
    AUTH_3D_EP_NONE         = 0,
    AUTH_3D_EP_LINEAR       = 1,
    AUTH_3D_EP_CYCLE        = 2,
    AUTH_3D_EP_CYCLE_OFFSET = 3,
} auth_3d_ep_type; // Pre/Post Infinity

typedef enum auth_3d_data_type {
    AUTH_3D_DATA_NONE    = 0,
    AUTH_3D_DATA_STATIC  = 1,
    AUTH_3D_DATA_LINEAR  = 2,
    AUTH_3D_DATA_HERMITE = 3,
    AUTH_3D_DATA_HOLD    = 4,
} auth_3d_data_type;

typedef enum auth_3d_camera_auxiliary_flags {
    AUTH_3D_CAMERA_EXPOSURE      = 0x01,
    AUTH_3D_CAMERA_GAMMA         = 0x02,
    AUTH_3D_CAMERA_SATURATE      = 0x04,
    AUTH_3D_CAMERA_AUTO_EXPOSURE = 0x08,
} auth_3d_camera_auxiliary_flags;

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

typedef struct auth_3d_comp_data_header {
    auth_3d_data_type type : 8;
    auth_3d_ep_type ep_type_pre : 4;
    auth_3d_ep_type ep_type_post : 4;
    uint32_t padding : 16;
    float_t value;
    float_t max;
    uint32_t length;
} auth_3d_comp_data_header;

typedef struct auth_3d_data {
    auth_3d_data_type type;
    size_t length;
    size_t bin_offset;
    auth_3d_ep_type ep_type_pre;
    auth_3d_ep_type ep_type_post;
    float_t max;
    float_t value;
    auth_3d_key_raw_data raw_data;
    kft3* keys;
} auth_3d_data;

typedef struct auth_3d_rgba_data {
    auth_3d_data r_data;
    auth_3d_data g_data;
    auth_3d_data b_data;
    auth_3d_data a_data;
    bool has_r;
    bool has_g;
    bool has_b;
    bool has_a;
} auth_3d_rgba_data;

typedef struct auth_3d_vec3_data {
    auth_3d_data x;
    auth_3d_data y;
    auth_3d_data z;
} auth_3d_vec3_data;

typedef struct auth_3d_model_transform_data {
    auth_3d_vec3_data scale_data;
    auth_3d_vec3_data rotation_data;
    auth_3d_vec3_data translation_data;
    auth_3d_data visibility_data;
    vec3 scale;
    vec3 rotation;
    vec3 translation;
    bool visibility;
} auth_3d_model_transform_data;

