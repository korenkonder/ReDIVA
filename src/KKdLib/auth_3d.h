/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"
#include "kf.h"
#include "vec.h"

typedef enum auth_3d_ambient_flags {
    AUTH_3D_AMBIENT_LIGHT     = 0x1,
    AUTH_3D_AMBIENT_RIM_LIGHT = 0x2,
} auth_3d_ambient_flags;

typedef enum auth_3d_camera_auxiliary_flags {
    AUTH_3D_CAMERA_EXPOSURE      = 0x01,
    AUTH_3D_CAMERA_GAMMA         = 0x02,
    AUTH_3D_CAMERA_SATURATE      = 0x04,
    AUTH_3D_CAMERA_AUTO_EXPOSURE = 0x08,
    AUTH_3D_CAMERA_GAMMA_RATE    = 0x10,
} auth_3d_camera_auxiliary_flags;

typedef enum auth_3d_compress_f16 {
    AUTH_3D_COMPRESS_TYPE_0 = 0,
    AUTH_3D_COMPRESS_TYPE_1 = 1,
    AUTH_3D_COMPRESS_TYPE_2 = 2,
} auth_3d_compress_f16;

typedef enum auth_3d_ep_type {
    AUTH_3D_EP_TYPE_NONE         = 0,
    AUTH_3D_EP_TYPE_LINEAR       = 1,
    AUTH_3D_EP_TYPE_CYCLE        = 2,
    AUTH_3D_EP_TYPE_CYCLE_OFFSET = 3,
} auth_3d_ep_type; // Pre/Post Infinity

typedef enum auth_3d_fog_flags {
    AUTH_3D_FOG_DENSITY = 0x1,
    AUTH_3D_FOG_START   = 0x2,
    AUTH_3D_FOG_END     = 0x4,
    AUTH_3D_FOG_DIFFUSE = 0x8,
} auth_3d_fog_flags;

typedef enum auth_3d_key_type {
    AUTH_3D_KEY_TYPE_NONE    = 0,
    AUTH_3D_KEY_TYPE_STATIC  = 1,
    AUTH_3D_KEY_TYPE_LINEAR  = 2,
    AUTH_3D_KEY_TYPE_HERMITE = 3,
    AUTH_3D_KEY_TYPE_HOLD    = 4,
} auth_3d_key_type;

typedef enum auth_3d_light_flags {
    AUTH_3D_LIGHT_AMBIENT       = 0x1,
    AUTH_3D_LIGHT_DIFFUSE       = 0x2,
    AUTH_3D_LIGHT_SPECULAR      = 0x4,
    AUTH_3D_LIGHT_INCANDESCENCE = 0x8,
} auth_3d_light_flags;

typedef enum auth_3d_material_list_flags {
    AUTH_3D_MATERIAL_LIST_BLEND_COLOR   = 0b01,
    AUTH_3D_MATERIAL_LIST_INCANDESCENCE = 0b10,
} auth_3d_material_list_flags;

typedef enum auth_3d_object_tex_transform_flags {
    AUTH_3D_OBJECT_TEX_TRANSFORM_COVERAGE_U        = 0x001,
    AUTH_3D_OBJECT_TEX_TRANSFORM_COVERAGE_V        = 0x002,
    AUTH_3D_OBJECT_TEX_TRANSFORM_REPEAT_U          = 0x004,
    AUTH_3D_OBJECT_TEX_TRANSFORM_REPEAT_V          = 0x008,
    AUTH_3D_OBJECT_TEX_TRANSFORM_TRANSLATE_FRAME_U = 0x010,
    AUTH_3D_OBJECT_TEX_TRANSFORM_TRANSLATE_FRAME_V = 0x020,
    AUTH_3D_OBJECT_TEX_TRANSFORM_OFFSET_U          = 0x040,
    AUTH_3D_OBJECT_TEX_TRANSFORM_OFFSET_V          = 0x080,
    AUTH_3D_OBJECT_TEX_TRANSFORM_ROTATE_FRAME      = 0x100,
    AUTH_3D_OBJECT_TEX_TRANSFORM_ROTATE            = 0x200,
} auth_3d_object_tex_transform_flags;

typedef enum auth_3d_post_process_flags {
    AUTH_3D_POST_PROCESS_LENS_FLARE = 0x01,
    AUTH_3D_POST_PROCESS_LENS_SHAFT = 0x02,
    AUTH_3D_POST_PROCESS_LENS_GHOST = 0x04,
    AUTH_3D_POST_PROCESS_RADIUS     = 0x08,
    AUTH_3D_POST_PROCESS_INTENSITY  = 0x10,
    AUTH_3D_POST_PROCESS_SCENE_FADE = 0x20,
} auth_3d_post_process_flags;

typedef struct auth_3d_key_raw_data {
    int32_t KeyType;
    int32_t ValueListSize;
    char* ValueType;
    char** ValueList;
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

typedef struct auth_3d_model_transform {
    bool written;
    size_t bin_offset;
    vec3_auth_3d_key scale;
    vec3_auth_3d_key rot;
    vec3_auth_3d_key trans;
    auth_3d_key visibility;
} auth_3d_model_transform;

typedef struct auth_3d__ {
    auth_3d_compress_f16 compress_f16;
    char* file_name;
    char* property_version;
    char* converter_version;
} auth_3d__;

typedef struct auth_3d_ambient {
    auth_3d_ambient_flags flags;

    char* name;
    vec4_auth_3d_key light;
    vec4_auth_3d_key rim_light;
} auth_3d_ambient;

typedef struct auth_3d_camera_auxiliary {
    auth_3d_camera_auxiliary_flags flags;

    auth_3d_key exposure;
    auth_3d_key gamma;
    auth_3d_key saturate;
    auth_3d_key gamma_rate;
    auth_3d_key auto_exposure;
} auth_3d_camera_auxiliary;

typedef struct auth_3d_view_point {
    float_t aspect;
    float_t camera_aperture_height;
    float_t camera_aperture_width;
    auth_3d_key fov;
    bool fov_horizontal;
    auth_3d_key focal_length;
    bool has_fov;
    auth_3d_model_transform mt;
    auth_3d_key roll;
} auth_3d_view_point;

typedef struct auth_3d_camera_root {
    auth_3d_view_point view_point;
    auth_3d_model_transform mt;
    auth_3d_model_transform interest;
} auth_3d_camera_root;

typedef struct auth_3d_curve {
    char* name;
    auth_3d_key cv;
} auth_3d_curve;

typedef struct auth_3d_dof {
    bool has_dof;
    auth_3d_model_transform mt;
    char* name;
} auth_3d_dof;

typedef struct auth_3d_event {
    int32_t type;
    float_t end;
    float_t begin;
    float_t clip_end;
    float_t clip_begin;
    float_t time_ref_scale;
    char* name;
    char* param;
    char* ref;
} auth_3d_event;

typedef struct auth_3d_fog {
    auth_3d_fog_flags flags;

    vec4_auth_3d_key diffuse; // Diffuse
    auth_3d_key density;
    auth_3d_key end;
    int32_t id;
    auth_3d_key start;
} auth_3d_fog;

typedef struct auth_3d_light {
    auth_3d_light_flags flags;

    int32_t id;
    char* name;
    char* type;
    vec4_auth_3d_key ambient; // Ambient
    vec4_auth_3d_key diffuse; // Diffuse
    vec4_auth_3d_key specular; // Specular
    vec4_auth_3d_key incandescence; // Incandescence
    auth_3d_model_transform position;
    auth_3d_model_transform spot_direction;
} auth_3d_light;

typedef struct auth_3d_material_list {
    auth_3d_material_list_flags flags;

    vec4_auth_3d_key blend_color;
    auth_3d_key glow_intensity;
    char* hash_name;
    vec4_auth_3d_key incandescence;
    char* name;
} auth_3d_material_list;

typedef struct auth_3d_obj_instance {
    bool shadow;
    char* name;
    char* uid_name;
    auth_3d_model_transform mt;
} auth_3d_obj_instance;

typedef struct auth_3d_obj_node {
    int32_t parent;
    char* name;
    auth_3d_model_transform mt;
} auth_3d_obj_node;

vector(auth_3d_obj_instance)
vector(auth_3d_obj_node)

typedef struct auth_3d_m_object_hrc {
    char* name;
    vector_auth_3d_obj_node node;
    vec3 joint_orient;
    vector_auth_3d_obj_instance instance;
    auth_3d_model_transform mt;
} auth_3d_m_object_hrc;

typedef struct auth_3d_object_tex_pat {
    float_t pat_offset;
    char* pat;
    char* name;
} auth_3d_object_tex_pat;

typedef struct auth_3d_object_tex_transform {
    auth_3d_object_tex_transform_flags flags;

    auth_3d_key coverage_u;
    auth_3d_key coverage_v;
    char* name;
    auth_3d_key offset_u;
    auth_3d_key offset_v;
    auth_3d_key repeat_u;
    auth_3d_key repeat_v;
    auth_3d_key rotate;
    auth_3d_key rotate_frame;
    auth_3d_key translate_frame_u;
    auth_3d_key translate_frame_v;
} auth_3d_object_tex_transform;

vector(auth_3d_object_tex_pat)
vector(auth_3d_object_tex_transform)

typedef struct auth_3d_object {
    float_t morph_offset;

    char* morph;
    char* name;
    char* parent_name;
    char* uid_name;

    auth_3d_model_transform mt;

    vector_auth_3d_object_tex_pat tex_pat;
    vector_auth_3d_object_tex_transform tex_transform;
} auth_3d_object;

typedef struct auth_3d_object_hrc {
    bool shadow;

    char* name;
    char* uid_name;

    vec3 joint_orient;

    vector_auth_3d_obj_node node;
} auth_3d_object_hrc;

typedef struct auth_3d_play_control {
    float_t begin;
    float_t div;
    float_t fps;
    float_t offset;
    float_t size;
} auth_3d_play_control;

typedef struct auth_3d_post_process {
    auth_3d_post_process_flags flags;

    auth_3d_key lens_ghost;
    auth_3d_key lens_flare;
    auth_3d_key lens_shaft;

    vec4_auth_3d_key radius;       // Ambient
    vec4_auth_3d_key intensity;    // Diffuse
    vec4_auth_3d_key scene_fade;   // Specular
} auth_3d_post_process;

vector(auth_3d_fog)
vector(auth_3d_curve)
vector(auth_3d_event)
vector(auth_3d_light)
vector(auth_3d_object)
vector(auth_3d_ambient)
vector(auth_3d_object_hrc)
vector(auth_3d_camera_root)
vector(auth_3d_m_object_hrc)
vector(auth_3d_material_list)
vector(auth_3d_model_transform)

typedef struct auth_3d_data {
    bool ready;

    vector_ptr_char motion;
    vector_ptr_char object_list;
    vector_ptr_char object_hrc_list;
    vector_ptr_char m_object_hrc_list;

    auth_3d__ _;
    auth_3d_camera_auxiliary camera_auxiliary;
    auth_3d_dof dof;
    auth_3d_play_control play_control;
    auth_3d_post_process post_process;

    vector_auth_3d_fog fog;
    vector_auth_3d_curve curve;
    vector_auth_3d_event event;
    vector_auth_3d_light light;
    vector_auth_3d_object object;
    vector_auth_3d_ambient ambient;
    vector_auth_3d_object_hrc object_hrc;
    vector_auth_3d_camera_root camera_root;
    vector_auth_3d_m_object_hrc m_object_hrc;
    vector_auth_3d_material_list material_list;
    vector_auth_3d_model_transform chara;
    vector_auth_3d_model_transform point;
} auth_3d_data;
