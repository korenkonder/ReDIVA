/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/database/object.h"
#include "../KKdLib/database/texture.h"
#include "../KKdLib/a3da.h"
#include "../KKdLib/kf.h"
#include "render_context.h"

typedef enum auth_3d_ambient_flags {
    AUTH_3D_AMBIENT_LIGHT_DIFFUSE     = 0x01,
    AUTH_3D_AMBIENT_RIM_LIGHT_DIFFUSE = 0x02,
} auth_3d_ambient_flags;

typedef enum auth_3d_camera_auxiliary_flags {
    AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE = 0x01,
    AUTH_3D_CAMERA_AUXILIARY_EXPOSURE      = 0x02,
    AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE = 0x04,
    AUTH_3D_CAMERA_AUXILIARY_GAMMA         = 0x08,
    AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE    = 0x10,
    AUTH_3D_CAMERA_AUXILIARY_SATURATE      = 0x20,
} auth_3d_camera_auxiliary_flags;

typedef enum auth_3d_camera_root_view_point_flags {
    AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV  = 0x01,
    AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL = 0x02,
} auth_3d_camera_root_view_point_flags;

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

typedef enum auth_3d_event_type {
    AUTH_3D_EVENT_MISC = 0x00,
    AUTH_3D_EVENT_FILT = 0x01,
    AUTH_3D_EVENT_FX   = 0x02,
    AUTH_3D_EVENT_SND  = 0x03,
    AUTH_3D_EVENT_MOT  = 0x04,
    AUTH_3D_EVENT_A2D  = 0x05,
} auth_3d_event_type;

typedef enum auth_3d_event_filter_fade_type {
    AUTH_3D_EVENT_FILTER_FADE_NONE = -1,
    AUTH_3D_EVENT_FILTER_FADE_OUT  = 0,
    AUTH_3D_EVENT_FILTER_FADE_IN   = 1,
} auth_3d_event_filter_fade_type;

typedef enum auth_3d_fog_flags {
    AUTH_3D_FOG_COLOR   = 0x01,
    AUTH_3D_FOG_DENSITY = 0x02,
    AUTH_3D_FOG_END     = 0x04,
    AUTH_3D_FOG_START   = 0x08,
} auth_3d_fog_flags;

typedef enum auth_3d_format {
    AUTH_3D_FORMAT_DT  = 0,
    AUTH_3D_FORMAT_F   = 1,
    AUTH_3D_FORMAT_FT  = 2,
    AUTH_3D_FORMAT_F2  = 3,
    AUTH_3D_FORMAT_MGF = 4,
    AUTH_3D_FORMAT_X   = 5,
    AUTH_3D_FORMAT_XHD = 6,
} auth_3d_format;

typedef enum auth_3d_key_flags {
    AUTH_3D_KEY_BIN_OFFSET = 0x01,
} auth_3d_key_flags;

typedef enum auth_3d_key_type {
    AUTH_3D_KEY_NONE    = 0,
    AUTH_3D_KEY_STATIC  = 1,
    AUTH_3D_KEY_LINEAR  = 2,
    AUTH_3D_KEY_HERMITE = 3,
    AUTH_3D_KEY_HOLD    = 4,
} auth_3d_key_type;

typedef enum auth_3d_light_flags {
    AUTH_3D_LIGHT_AMBIENT        = 0x0001,
    AUTH_3D_LIGHT_CONE_ANGLE     = 0x0002,
    AUTH_3D_LIGHT_CONSTANT       = 0x0004,
    AUTH_3D_LIGHT_DIFFUSE        = 0x0008,
    AUTH_3D_LIGHT_DROP_OFF       = 0x0010,
    AUTH_3D_LIGHT_FAR            = 0x0020,
    AUTH_3D_LIGHT_INTENSITY      = 0x0040,
    AUTH_3D_LIGHT_LINEAR         = 0x0080,
    AUTH_3D_LIGHT_POSITION       = 0x0100,
    AUTH_3D_LIGHT_QUADRATIC      = 0x0200,
    AUTH_3D_LIGHT_SPECULAR       = 0x0400,
    AUTH_3D_LIGHT_SPOT_DIRECTION = 0x0800,
    AUTH_3D_LIGHT_TONE_CURVE     = 0x1000,
} auth_3d_light_flags;

typedef enum auth_3d_material_list_flags {
    AUTH_3D_MATERIAL_LIST_BLEND_COLOR    = 0x01,
    AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY = 0x02,
    AUTH_3D_MATERIAL_LIST_INCANDESCENCE  = 0x04,
} auth_3d_material_list_flags;

typedef enum auth_3d_model_transform_flags {
    AUTH_3D_MODEL_TRANSFORM_BIN_OFFSET = 0x01,
} auth_3d_model_transform_flags;

typedef enum auth_3d_object_node_flags {
    AUTH_3D_OBJECT_NODE_JOINT_ORIENT = 0x01,
} auth_3d_object_node_flags;

typedef enum auth_3d_object_texture_transform_flags {
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U        = 0x001,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V        = 0x002,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U          = 0x004,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V          = 0x008,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U          = 0x010,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V          = 0x020,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE            = 0x040,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME      = 0x080,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U = 0x100,
    AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V = 0x200,
} auth_3d_object_texture_transform_flags;

typedef enum auth_3d_play_control_flags {
    AUTH_3D_PLAY_CONTROL_DIV    = 0x01,
    AUTH_3D_PLAY_CONTROL_OFFSET = 0x02,
} auth_3d_play_control_flags;

typedef enum auth_3d_post_process_flags {
    AUTH_3D_POST_PROCESS_INTENSITY  = 0x01,
    AUTH_3D_POST_PROCESS_LENS_FLARE = 0x02,
    AUTH_3D_POST_PROCESS_LENS_GHOST = 0x04,
    AUTH_3D_POST_PROCESS_LENS_SHAFT = 0x08,
    AUTH_3D_POST_PROCESS_RADIUS     = 0x10,
    AUTH_3D_POST_PROCESS_SCENE_FADE = 0x20,
} auth_3d_post_process_flags;

typedef enum auth_3d_rgba_flags {
    AUTH_3D_RGBA_R = 0x01,
    AUTH_3D_RGBA_G = 0x02,
    AUTH_3D_RGBA_B = 0x04,
    AUTH_3D_RGBA_A = 0x08,
} auth_3d_rgba_flags;

typedef struct auth_3d_ambient auth_3d_ambient;
typedef struct auth_3d_camera_auxiliary auth_3d_camera_auxiliary;
typedef struct auth_3d_camera_root auth_3d_camera_root;
typedef struct auth_3d_camera_root_view_point auth_3d_camera_root_view_point;
typedef struct auth_3d_chara auth_3d_chara;
typedef struct auth_3d_curve auth_3d_curve;
typedef struct auth_3d_dof auth_3d_dof;
typedef struct auth_3d_event auth_3d_event;
typedef struct auth_3d_fog auth_3d_fog;
typedef struct auth_3d_key auth_3d_key;
typedef struct auth_3d_light auth_3d_light;
typedef struct auth_3d_m_object_hrc auth_3d_m_object_hrc;
typedef struct auth_3d_material_list auth_3d_material_list;
typedef struct auth_3d_model_transform auth_3d_model_transform;
typedef struct auth_3d_object auth_3d_object;
typedef struct auth_3d_object_curve auth_3d_object_curve;
typedef struct auth_3d_object_hrc auth_3d_object_hrc;
typedef struct auth_3d_object_instance auth_3d_object_instance;
typedef struct auth_3d_object_model_transform auth_3d_object_model_transform;
typedef struct auth_3d_object_node auth_3d_object_node;
typedef struct auth_3d_object_texture_pattern auth_3d_object_texture_pattern;
typedef struct auth_3d_object_texture_transform auth_3d_object_texture_transform;
typedef struct auth_3d_play_control auth_3d_play_control;
typedef struct auth_3d_point auth_3d_point;
typedef struct auth_3d_post_process auth_3d_post_process;
typedef struct auth_3d_rgba auth_3d_rgba;
typedef struct auth_3d_vec3 auth_3d_vec3;

ptr_t(auth_3d_m_object_hrc)
ptr_t(auth_3d_object)
ptr_t(auth_3d_object_hrc)

vector(auth_3d_ambient)
vector(auth_3d_model_transform)
vector(auth_3d_camera_root)
vector(auth_3d_chara)
vector(auth_3d_curve)
vector(auth_3d_event)
vector(auth_3d_fog)
vector(auth_3d_light)
vector(auth_3d_m_object_hrc)
vector(auth_3d_material_list)
vector(auth_3d_object)
vector(auth_3d_object_hrc)
vector(auth_3d_object_instance)
vector(auth_3d_object_node)
vector(auth_3d_object_texture_pattern)
vector(auth_3d_object_texture_transform)
vector(auth_3d_point)
vector(ptr_auth_3d_m_object_hrc)
vector(ptr_auth_3d_object)
vector(ptr_auth_3d_object_hrc)

struct auth_3d_key {
    auth_3d_key_type type;
    float_t value;
    float_t max_frame;
    auth_3d_ep_type ep_type_pre;
    auth_3d_ep_type ep_type_post;
    float_t frame_delta;
    float_t value_delta;
    vector_kft3 keys_vec;
    size_t length;
    kft3* keys;

    float_t frame;
    float_t value_interp;
    float_t value_init;
};

struct auth_3d_rgba {
    auth_3d_rgba_flags flags;
    auth_3d_key r;
    auth_3d_key g;
    auth_3d_key b;
    auth_3d_key a;

    vec4u value;
};

struct auth_3d_vec3 {
    auth_3d_key x;
    auth_3d_key y;
    auth_3d_key z;
};

struct auth_3d_model_transform {
    mat4u mat;
    auth_3d_vec3 scale;
    auth_3d_vec3 rotation;
    auth_3d_vec3 translation;
    auth_3d_key visibility;

    vec3 scale_value;
    vec3 rotation_value;
    vec3 translation_value;
    bool visible;
};

struct auth_3d_ambient {
    auth_3d_ambient_flags flags;
    auth_3d_rgba light_diffuse;
    string name;
    auth_3d_rgba rim_light_diffuse;
};

struct auth_3d_camera_auxiliary {
    auth_3d_camera_auxiliary_flags flags;
    auth_3d_key auto_exposure;
    auth_3d_key exposure;
    auth_3d_key exposure_rate;
    auth_3d_key gamma;
    auth_3d_key gamma_rate;
    auth_3d_key saturate;

    float_t auto_exposure_value;
    float_t exposure_value;
    float_t exposure_rate_value;
    float_t gamma_value;
    float_t gamma_rate_value;
    float_t saturate_value;
};

struct auth_3d_camera_root_view_point {
    auth_3d_camera_root_view_point_flags flags;
    float_t aspect;
    float_t camera_aperture_h;
    float_t camera_aperture_w;
    auth_3d_key focal_length;
    auth_3d_key fov;
    bool fov_is_horizontal;
    auth_3d_model_transform model_transform;
    auth_3d_key roll;
};

struct auth_3d_camera_root {
    auth_3d_model_transform interest;
    auth_3d_model_transform model_transform;
    auth_3d_camera_root_view_point view_point;

    float_t fov_value;
    vec3 interest_value;
    float_t roll_value;
    vec3 view_point_value;
    bool fast_change;
    bool fast_change_hist0;
};

struct auth_3d_chara {
    auth_3d_model_transform model_transform;
    string name;
};

struct auth_3d_curve {
    auth_3d_key curve;
    string name;
    float_t value;
};

struct auth_3d_dof {
    bool has_dof;
    auth_3d_model_transform model_transform;
};

struct auth_3d_event {
    float_t begin;
    float_t clip_begin;
    float_t clip_end;
    float_t end;
    string name;
    string param_1;
    string ref;
    float_t time_ref_scale;
    auth_3d_event_type type;
};

struct auth_3d_fog {
    auth_3d_fog_flags flags;
    auth_3d_fog_flags flags_init;
    auth_3d_rgba color;
    auth_3d_key density;
    auth_3d_key end;
    fog_id id;
    auth_3d_key start;

    float_t density_value;
    float_t end_value;
    float_t start_value;

    vec4u color_init;
    float_t density_init;
    float_t end_init;
    float_t start_init;
};

struct auth_3d_light {
    auth_3d_light_flags flags;
    auth_3d_light_flags flags_init;
    auth_3d_rgba ambient;
    auth_3d_key cone_angle;
    auth_3d_key constant;
    auth_3d_rgba diffuse;
    auth_3d_key drop_off;
    auth_3d_key _far;
    light_id id;
    auth_3d_key intensity;
    auth_3d_key linear;
    auth_3d_model_transform position;
    auth_3d_key quadratic;
    auth_3d_rgba specular;
    auth_3d_model_transform spot_direction;
    auth_3d_rgba tone_curve;
    string type;

    float_t cone_angle_value;
    float_t constant_value;
    float_t drop_off_value;
    float_t far_value;
    float_t intensity_value;
    float_t linear_value;
    float_t quadratic_value;

    vec4u ambient_init;
    float_t cone_angle_init;
    float_t constant_init;
    vec4u diffuse_init;
    float_t drop_off_init;
    float_t far_init;
    float_t intensity_init;
    float_t linear_init;
    float_t quadratic_init;
    vec4u specular_init;
    vec3 tone_curve_init;
};

struct auth_3d_object_model_transform {
    mat4u mat;
    mat4u mat_inner;
    mat4u mat_rot;
    auth_3d_vec3 translation;
    auth_3d_vec3 rotation;
    auth_3d_vec3 scale;
    auth_3d_key visibility;

    vec3 translation_value;
    vec3 rotation_value;
    vec3 scale_value;
    bool visible;
    float_t frame;
    bool has_rotation;
    bool has_translation;
    bool has_scale;
    bool has_visibility;
};

struct auth_3d_m_object_hrc {
    vector_auth_3d_object_instance instance;
    auth_3d_object_model_transform model_transform;
    string name;
    vector_auth_3d_object_node node;
};

struct auth_3d_material_list {
    auth_3d_material_list_flags flags;
    auth_3d_rgba blend_color;
    auth_3d_key glow_intensity;
    auth_3d_rgba incandescence;
    string name;

    float_t glow_intensity_value;
};

struct auth_3d_object_curve {
    auth_3d_curve* curve;
    string name;
    float_t frame_offset;
    float_t value;
};

struct auth_3d_object {
    vector_ptr_auth_3d_object children_object;
    vector_ptr_auth_3d_object_hrc children_object_hrc;
    auth_3d_model_transform model_transform;
    auth_3d_object_curve morph;
    object_info object_info;
    uint32_t object_hash;
    string name;
    string parent_name;
    string parent_node;
    auth_3d_object_curve pattern;
    bool reflect;
    bool refract;
    vector_auth_3d_object_texture_pattern texture_pattern;
    vector_auth_3d_object_texture_transform texture_transform;
    string uid_name;
};

struct auth_3d_object_hrc {
    vector_ptr_auth_3d_object children_object;
    vector_ptr_auth_3d_object_hrc children_object_hrc;
    vector_int32_t childer_object_hrc_parent_node;
    vector_int32_t childer_object_parent_node;
    vector_mat4 mats;
    string name;
    vector_auth_3d_object_node node;
    object_info object_info;
    uint32_t object_hash;
    string parent_name;
    string parent_node;
    bool reflect;
    bool refract;
    bool shadow;
    string uid_name;
};

struct auth_3d_object_instance {
    vector_mat4 mats;
    auth_3d_object_model_transform model_transform;
    string name;
    vector_int32_t object_bone_indices;
    object_info object_info;
    uint32_t object_hash;
    bool shadow;
    string uid_name;
};

struct auth_3d_object_node {
    auth_3d_object_node_flags flags;
    int32_t bone_id;
    vec3 joint_orient;
    mat4u joint_orient_mat;
    mat4* mat;
    auth_3d_object_model_transform model_transform;
    string name;
    int32_t parent;
};

struct auth_3d_object_texture_pattern {
    string name;
    auth_3d_object_curve pattern;
    int32_t texture_id;
};

struct auth_3d_object_texture_transform {
    auth_3d_object_texture_transform_flags flags;
    auth_3d_key coverage_u;
    auth_3d_key coverage_v;
    mat4u mat;
    string name;
    auth_3d_key offset_u;
    auth_3d_key offset_v;
    auth_3d_key repeat_u;
    auth_3d_key repeat_v;
    auth_3d_key rotate;
    auth_3d_key rotate_frame;
    int32_t texture_id;
    auth_3d_key translate_frame_u;
    auth_3d_key translate_frame_v;
};

struct auth_3d_play_control {
    auth_3d_play_control_flags flags;
    float_t begin;
    int32_t div;
    float_t fps;
    float_t offset;
    float_t size;
};

struct auth_3d_point {
    auth_3d_model_transform model_transform;
    string name;
};

struct auth_3d_post_process {
    auth_3d_post_process_flags flags;
    auth_3d_post_process_flags flags_init;
    auth_3d_rgba intensity;
    auth_3d_key lens_flare;
    auth_3d_key lens_ghost;
    auth_3d_key lens_shaft;
    auth_3d_rgba radius;
    auth_3d_rgba scene_fade;

    float_t lens_flare_value;
    float_t lens_ghost_value;
    float_t lens_shaft_value;

    vec3 intensity_init;
    float_t lens_flare_init;
    float_t lens_ghost_init;
    float_t lens_shaft_init;
    vec3 radius_init;
    vec4u scene_fade_init;
};

typedef struct auth_3d {
    int32_t uid;
    int32_t id;
    bool enable;
    bool camera_root_update;
    bool visible;
    bool repeat;
    bool ended;
    bool left_right_reverse;
    bool once;

    float_t alpha;
    int32_t chara_id;
    frame_rate_control* frame_rate;
    float_t frame;
    float_t set_frame;
    float_t max_frame;
    bool frame_changed;
    float_t frame_offset;
    float_t last_frame;
    bool paused;

    vector_auth_3d_ambient ambient;
    vector_string auth_2d;
    auth_3d_camera_auxiliary camera_auxiliary;
    vector_auth_3d_camera_root camera_root;
    vector_auth_3d_chara chara;
    vector_auth_3d_curve curve;
    auth_3d_dof dof;
    vector_auth_3d_event event;
    vector_auth_3d_fog fog;
    vector_auth_3d_light light;
    vector_auth_3d_m_object_hrc m_object_hrc;
    vector_ptr_auth_3d_m_object_hrc m_object_hrc_list;
    vector_auth_3d_material_list material_list;
    vector_string motion;
    vector_auth_3d_object object;
    vector_auth_3d_object_hrc object_hrc;
    vector_ptr_auth_3d_object_hrc object_hrc_list;
    vector_ptr_auth_3d_object object_list;
    auth_3d_play_control play_control;
    vector_auth_3d_point point;
    auth_3d_post_process post_process;

    string file_name;
    int32_t frame_int;
    uint32_t hash;
} auth_3d;

#define AUTH_3D_DATA_COUNT 0x100

typedef struct auth_3d_data_struct {
    vector_int32_t loaded_ids;
    auth_3d data[AUTH_3D_DATA_COUNT];
} auth_3d_data_struct;

typedef struct auth_3d_farc {
    farc auth_3d_farc;
    vector_uint32_t obj_set_id;
} auth_3d_farc;

extern auth_3d_data_struct auth_3d_data;

extern void auth_3d_init(auth_3d* auth);
extern void auth_3d_data_set(auth_3d* auth, mat4* mat, render_context* rctx);
extern void auth_3d_time_step(auth_3d* auth, mat4* mat);
extern void auth_3d_load(auth_3d* auth, a3da* auth_file,
    object_database* obj_db, texture_database* tex_db);
extern void auth_3d_load_from_farc(auth_3d* auth, farc* f, char* file,
    object_database* obj_db, texture_database* tex_db);
extern void auth_3d_unload(auth_3d* auth, render_context* rctx);
extern void auth_3d_free(auth_3d* auth);

extern void auth_3d_farc_init(auth_3d_farc* f);
extern void auth_3d_farc_load(auth_3d_farc* f, void* data, char* name, object_database* obj_db);
extern void auth_3d_farc_free(auth_3d_farc* f);

extern void auth_3d_key_free(auth_3d_key* k);
extern void auth_3d_rgba_free(auth_3d_rgba* rgba);
extern void auth_3d_vec3_free(auth_3d_vec3* vec);
extern void auth_3d_model_transform_free(auth_3d_model_transform* mt);

extern void auth_3d_ambient_free(auth_3d_ambient* a);
extern void auth_3d_camera_auxiliary_free(auth_3d_camera_auxiliary* ca);
extern void auth_3d_camera_root_free(auth_3d_camera_root* cr);
extern void auth_3d_camera_root_view_point_free(auth_3d_camera_root_view_point* crvp);
extern void auth_3d_chara_free(auth_3d_chara* c);
extern void auth_3d_curve_free(auth_3d_curve* c);
extern void auth_3d_dof_free(auth_3d_dof* d);
extern void auth_3d_event_free(auth_3d_event* e);
extern void auth_3d_fog_free(auth_3d_fog* f);
extern void auth_3d_light_free(auth_3d_light* l);
extern void auth_3d_m_object_hrc_free(auth_3d_m_object_hrc* moh);
extern void auth_3d_material_list_free(auth_3d_material_list* ml);
extern void auth_3d_object_free(auth_3d_object* o);
extern void auth_3d_object_curve_free(auth_3d_object_curve* oc);
extern void auth_3d_object_hrc_free(auth_3d_object_hrc* oh);
extern void auth_3d_object_instance_free(auth_3d_object_instance* oi);
extern void auth_3d_object_model_transform_free(auth_3d_object_model_transform* omt);
extern void auth_3d_object_node_free(auth_3d_object_node* on);
extern void auth_3d_object_texture_pattern_free(auth_3d_object_texture_pattern* otp);
extern void auth_3d_object_texture_transform_free(auth_3d_object_texture_transform* ott);
extern void auth_3d_point_free(auth_3d_point* p);
extern void auth_3d_post_process_free(auth_3d_post_process* pp);

extern void auth_3d_data_init();
extern int32_t auth_3d_data_load_uid(int32_t uid, auth_3d_database* auth_3d_db);
extern auth_3d* auth_3d_data_get_auth_3d(int32_t id);
extern int32_t auth_3d_data_get_chara_id(int32_t id);
extern int32_t auth_3d_data_get_auth_3d_id_by_object_info(object_info obj_info,
    int32_t* object_index, bool* hrc, int32_t instance);
extern int32_t auth_3d_data_get_auth_3d_id_by_hash(uint32_t file_name_hash, uint32_t object_hash,
    int32_t* object_index, bool* hrc, int32_t instance);
extern mat4* auth_3d_data_struct_get_auth_3d_object_mat(int32_t id, size_t index, bool hrc, mat4* mat);
extern void auth_3d_data_unload_id(int32_t id, render_context* rctx);
extern void auth_3d_data_free();
