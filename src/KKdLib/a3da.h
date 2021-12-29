/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "light_param/fog.h"
#include "light_param/light.h"
#include "kf.h"
#include "mat.h"
#include "string.h"
#include "vec.h"
#include "vector.h"

typedef enum a3da_ambient_flags {
    A3DA_AMBIENT_LIGHT_DIFFUSE     = 0x01,
    A3DA_AMBIENT_RIM_LIGHT_DIFFUSE = 0x02,
} a3da_ambient_flags;

typedef enum a3da_camera_auxiliary_flags {
    A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE = 0x01,
    A3DA_CAMERA_AUXILIARY_EXPOSURE      = 0x02,
    A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE = 0x04,
    A3DA_CAMERA_AUXILIARY_GAMMA         = 0x08,
    A3DA_CAMERA_AUXILIARY_GAMMA_RATE    = 0x10,
    A3DA_CAMERA_AUXILIARY_SATURATE      = 0x20,
} a3da_camera_auxiliary_flags;

typedef enum a3da_camera_root_view_point_flags {
    A3DA_CAMERA_ROOT_VIEW_POINT_FOV  = 0x01,
    A3DA_CAMERA_ROOT_VIEW_POINT_ROLL = 0x02,
} a3da_camera_root_view_point_flags;

typedef enum a3da_compress_f16 {
    A3DA_COMPRESS_F32F32F32F32 = 0,
    A3DA_COMPRESS_I16F16F32F32 = 1,
    A3DA_COMPRESS_I16F16F16F16 = 2,
} a3da_compress_f16;

typedef enum a3da_ep_type {
    A3DA_EP_NONE         = 0,
    A3DA_EP_LINEAR       = 1,
    A3DA_EP_CYCLE        = 2,
    A3DA_EP_CYCLE_OFFSET = 3,
} a3da_ep_type; // Pre/Post Infinity

typedef enum a3da_event_type {
    A3DA_EVENT_MISC = 0x00,
    A3DA_EVENT_FILT = 0x01,
    A3DA_EVENT_FX   = 0x02,
    A3DA_EVENT_SND  = 0x03,
    A3DA_EVENT_MOT  = 0x04,
    A3DA_EVENT_A2D  = 0x05,
} a3da_event_type;

typedef enum a3da_fog_flags {
    A3DA_FOG_COLOR   = 0x01,
    A3DA_FOG_DENSITY = 0x02,
    A3DA_FOG_END     = 0x04,
    A3DA_FOG_START   = 0x08,
} a3da_fog_flags;

typedef enum a3da_format {
    A3DA_FORMAT_DT  = 0,
    A3DA_FORMAT_F   = 1,
    A3DA_FORMAT_AFT = 2,
    A3DA_FORMAT_F2  = 3,
    A3DA_FORMAT_MGF = 4,
    A3DA_FORMAT_X   = 5,
    A3DA_FORMAT_XHD = 6,
} a3da_format;

typedef enum a3da_key_flags {
    A3DA_KEY_BIN_OFFSET = 0x01,
} a3da_key_flags;

typedef enum a3da_key_type {
    A3DA_KEY_NONE    = 0,
    A3DA_KEY_STATIC  = 1,
    A3DA_KEY_LINEAR  = 2,
    A3DA_KEY_HERMITE = 3,
    A3DA_KEY_HOLD    = 4,
} a3da_key_type;

typedef enum a3da_light_flags {
    A3DA_LIGHT_AMBIENT        = 0x0001,
    A3DA_LIGHT_CONE_ANGLE     = 0x0002,
    A3DA_LIGHT_CONSTANT       = 0x0004,
    A3DA_LIGHT_DIFFUSE        = 0x0008,
    A3DA_LIGHT_DROP_OFF       = 0x0010,
    A3DA_LIGHT_FAR            = 0x0020,
    A3DA_LIGHT_INTENSITY      = 0x0040,
    A3DA_LIGHT_LINEAR         = 0x0080,
    A3DA_LIGHT_POSITION       = 0x0100,
    A3DA_LIGHT_QUADRATIC      = 0x0200,
    A3DA_LIGHT_SPECULAR       = 0x0400,
    A3DA_LIGHT_SPOT_DIRECTION = 0x0800,
    A3DA_LIGHT_TONE_CURVE     = 0x1000,
} a3da_light_flags;

typedef enum a3da_material_list_flags {
    A3DA_MATERIAL_LIST_BLEND_COLOR    = 0x01,
    A3DA_MATERIAL_LIST_GLOW_INTENSITY = 0x02,
    A3DA_MATERIAL_LIST_INCANDESCENCE  = 0x04,
} a3da_material_list_flags;

typedef enum a3da_model_transform_flags {
    A3DA_MODEL_TRANSFORM_BIN_OFFSET = 0x01,
} a3da_model_transform_flags;

typedef enum a3da_object_node_flags {
    A3DA_OBJECT_NODE_JOINT_ORIENT = 0x01,
} a3da_object_node_flags;

typedef enum a3da_object_texture_transform_flags {
    A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U        = 0x001,
    A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V        = 0x002,
    A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U          = 0x004,
    A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V          = 0x008,
    A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U          = 0x010,
    A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V          = 0x020,
    A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE            = 0x040,
    A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME      = 0x080,
    A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U = 0x100,
    A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V = 0x200,
} a3da_object_texture_transform_flags;

typedef enum a3da_play_control_flags {
    A3DA_PLAY_CONTROL_DIV    = 0x01,
    A3DA_PLAY_CONTROL_OFFSET = 0x02,
} a3da_play_control_flags;

typedef enum a3da_post_process_flags {
    A3DA_POST_PROCESS_INTENSITY  = 0x01,
    A3DA_POST_PROCESS_LENS_FLARE = 0x02,
    A3DA_POST_PROCESS_LENS_GHOST = 0x04,
    A3DA_POST_PROCESS_LENS_SHAFT = 0x08,
    A3DA_POST_PROCESS_RADIUS     = 0x10,
    A3DA_POST_PROCESS_SCENE_FADE = 0x20,
} a3da_post_process_flags;

typedef enum a3da_rgba_flags {
    A3DA_RGBA_R = 0x01,
    A3DA_RGBA_G = 0x02,
    A3DA_RGBA_B = 0x04,
    A3DA_RGBA_A = 0x08,
} a3da_rgba_flags;

typedef struct a3da_ambient a3da_ambient;
typedef struct a3da_camera_auxiliary a3da_camera_auxiliary;
typedef struct a3da_camera_root a3da_camera_root;
typedef struct a3da_camera_root_view_point a3da_camera_root_view_point;
typedef struct a3da_chara a3da_chara;
typedef struct a3da_curve a3da_curve;
typedef struct a3da_dof a3da_dof;
typedef struct a3da_event a3da_event;
typedef struct a3da_fog a3da_fog;
typedef struct a3da_key a3da_key;
typedef struct a3da_light a3da_light;
typedef struct a3da_m_object_hrc a3da_m_object_hrc;
typedef struct a3da_material_list a3da_material_list;
typedef struct a3da_model_transform a3da_model_transform;
typedef struct a3da_object a3da_object;
typedef struct a3da_object_hrc a3da_object_hrc;
typedef struct a3da_object_instance a3da_object_instance;
typedef struct a3da_object_node a3da_object_node;
typedef struct a3da_object_texture_pattern a3da_object_texture_pattern;
typedef struct a3da_object_texture_transform a3da_object_texture_transform;
typedef struct a3da_play_control a3da_play_control;
typedef struct a3da_point a3da_point;
typedef struct a3da_post_process a3da_post_process;
typedef struct a3da_rgba a3da_rgba;
typedef struct a3da_vec3 a3da_vec3;

vector(a3da_ambient)
vector(a3da_model_transform)
vector(a3da_camera_root)
vector(a3da_chara)
vector(a3da_curve)
vector(a3da_event)
vector(a3da_fog)
vector(a3da_light)
vector(a3da_m_object_hrc)
vector(a3da_material_list)
vector(a3da_object)
vector(a3da_object_hrc)
vector(a3da_object_instance)
vector(a3da_object_node)
vector(a3da_object_texture_pattern)
vector(a3da_object_texture_transform)
vector(a3da_point)

struct a3da_key {
    a3da_key_flags flags;
    int32_t bin_offset;
    a3da_key_type type;
    a3da_ep_type ep_type_pre;
    a3da_ep_type ep_type_post;
    vector_kft3 keys;
    float_t max;
    bool raw_data;
    bool raw_data_binary;
    int32_t raw_data_value_list_size;
    int32_t raw_data_value_list_offset;
    float_t value;
};

struct a3da_rgba {
    a3da_rgba_flags flags;
    a3da_key r;
    a3da_key g;
    a3da_key b;
    a3da_key a;
};

struct a3da_vec3 {
    a3da_key x;
    a3da_key y;
    a3da_key z;
};

struct a3da_model_transform {
    a3da_model_transform_flags flags;
    int32_t bin_offset;
    a3da_vec3 rotation;
    a3da_vec3 scale;
    a3da_vec3 translation;
    a3da_key visibility;
};

struct a3da_ambient {
    a3da_ambient_flags flags;
    a3da_rgba light_diffuse;
    string name;
    a3da_rgba rim_light_diffuse;
};

struct a3da_camera_auxiliary {
    a3da_camera_auxiliary_flags flags;
    a3da_key auto_exposure;
    a3da_key exposure;
    a3da_key exposure_rate;
    a3da_key gamma;
    a3da_key gamma_rate;
    a3da_key saturate;
};

struct a3da_camera_root_view_point {
    a3da_camera_root_view_point_flags flags;
    float_t aspect;
    float_t camera_aperture_h;
    float_t camera_aperture_w;
    a3da_key focal_length;
    a3da_key fov;
    bool fov_is_horizontal;
    a3da_model_transform model_transform;
    a3da_key roll;
};

struct a3da_camera_root {
    a3da_model_transform interest;
    a3da_model_transform model_transform;
    a3da_camera_root_view_point view_point;
};

struct a3da_chara {
    a3da_model_transform model_transform;
    string name;
};

struct a3da_curve {
    a3da_key curve;
    string name;
};

struct a3da_dof {
    bool has_dof;
    a3da_model_transform model_transform;
};

struct a3da_event {
    float_t begin;
    float_t clip_begin;
    float_t clip_end;
    float_t end;
    string name;
    string param_1;
    string ref;
    float_t time_ref_scale;
    a3da_event_type type;
};

struct a3da_fog {
    a3da_fog_flags flags;
    a3da_rgba color;
    a3da_key density;
    a3da_key end;
    fog_id id;
    a3da_key start;
};

struct a3da_light {
    a3da_light_flags flags;
    a3da_rgba ambient;
    a3da_key cone_angle;
    a3da_key constant;
    a3da_rgba diffuse;
    a3da_key drop_off;
    a3da_key _far;
    light_id id;
    a3da_key intensity;
    a3da_key linear;
    a3da_model_transform position;
    a3da_key quadratic;
    a3da_rgba specular;
    a3da_model_transform spot_direction;
    a3da_rgba tone_curve;
    string type;
};

struct a3da_m_object_hrc {
    vector_a3da_object_instance instance;
    a3da_model_transform model_transform;
    string name;
    vector_a3da_object_node node;
};

struct a3da_material_list {
    a3da_material_list_flags flags;
    a3da_rgba blend_color;
    a3da_key glow_intensity;
    a3da_rgba incandescence;
    string name;
};

struct a3da_object {
    a3da_model_transform model_transform;
    string morph;
    float_t morph_offset;
    string name;
    string parent_name;
    string parent_node;
    string pattern;
    float_t pattern_offset;
    vector_a3da_object_texture_pattern texture_pattern;
    vector_a3da_object_texture_transform texture_transform;
    string uid_name;
};

struct a3da_object_hrc {
    string name;
    vector_a3da_object_node node;
    string parent_name;
    string parent_node;
    bool shadow;
    string uid_name;
};

struct a3da_object_instance {
    a3da_model_transform model_transform;
    string name;
    bool shadow;
    string uid_name;
};

struct a3da_object_node {
    a3da_object_node_flags flags;
    vec3 joint_orient;
    a3da_model_transform model_transform;
    string name;
    int32_t parent;
};

struct a3da_object_texture_pattern {
    string name;
    string pattern;
    float_t pattern_offset;
};

struct a3da_object_texture_transform {
    a3da_object_texture_transform_flags flags;
    a3da_key coverage_u;
    a3da_key coverage_v;
    string name;
    a3da_key offset_u;
    a3da_key offset_v;
    a3da_key repeat_u;
    a3da_key repeat_v;
    a3da_key rotate;
    a3da_key rotate_frame;
    a3da_key translate_frame_u;
    a3da_key translate_frame_v;
};

struct a3da_play_control {
    a3da_play_control_flags flags;
    float_t begin;
    int32_t div;
    float_t fps;
    float_t offset;
    float_t size;
};

struct a3da_point {
    a3da_model_transform model_transform;
    string name;
};

struct a3da_post_process {
    a3da_post_process_flags flags;
    a3da_rgba intensity;
    a3da_key lens_flare;
    a3da_key lens_ghost;
    a3da_key lens_shaft;
    a3da_rgba radius;
    a3da_rgba scene_fade;
};

typedef struct a3da {
    bool ready;
    bool compressed;
    a3da_format format;

    a3da_compress_f16 _compress_f16;
    string _file_name;
    string _property_version;
    string _converter_version;

    vector_a3da_ambient ambient;
    vector_string auth_2d;
    a3da_camera_auxiliary camera_auxiliary;
    vector_a3da_camera_root camera_root;
    vector_a3da_chara chara;
    vector_a3da_curve curve;
    a3da_dof dof;
    vector_a3da_event event;
    vector_a3da_fog fog;
    vector_a3da_light light;
    vector_a3da_m_object_hrc m_object_hrc;
    vector_string m_object_hrc_list;
    vector_a3da_material_list material_list;
    vector_string motion;
    vector_a3da_object object;
    vector_a3da_object_hrc object_hrc;
    vector_string object_hrc_list;
    vector_string object_list;
    a3da_play_control play_control;
    vector_a3da_point point;
    a3da_post_process post_process;
} a3da;

extern void a3da_init(a3da* a);
extern void a3da_read(a3da* a, char* path);
extern void a3da_wread(a3da* a, wchar_t* path);
extern void a3da_mread(a3da* a, void* data, size_t length);
extern void a3da_write(a3da* a, char* path);
extern void a3da_wwrite(a3da* a, wchar_t* path);
extern void a3da_mwrite(a3da* a, void** data, size_t* length);
extern bool a3da_load_file(void* data, char* path, char* file, uint32_t hash);
extern void a3da_free(a3da* a);

extern void a3da_key_free(a3da_key* k);
extern void a3da_rgba_free(a3da_rgba* rgba);
extern void a3da_vec3_free(a3da_vec3* vec);
extern void a3da_model_transform_free(a3da_model_transform* mt);

extern void a3da_ambient_free(a3da_ambient* a);
extern void a3da_camera_auxiliary_free(a3da_camera_auxiliary* ca);
extern void a3da_camera_root_free(a3da_camera_root* cr);
extern void a3da_camera_root_view_point_free(a3da_camera_root_view_point* crvp);
extern void a3da_chara_free(a3da_chara* c);
extern void a3da_curve_free(a3da_curve* c);
extern void a3da_dof_free(a3da_dof* d);
extern void a3da_event_free(a3da_event* e);
extern void a3da_fog_free(a3da_fog* f);
extern void a3da_light_free(a3da_light* l);
extern void a3da_m_object_hrc_free(a3da_m_object_hrc* moh);
extern void a3da_material_list_free(a3da_material_list* ml);
extern void a3da_object_free(a3da_object* o);
extern void a3da_object_hrc_free(a3da_object_hrc* oh);
extern void a3da_object_instance_free(a3da_object_instance* oi);
extern void a3da_object_node_free(a3da_object_node* on);
extern void a3da_object_texture_pattern_free(a3da_object_texture_pattern* otp);
extern void a3da_object_texture_transform_free(a3da_object_texture_transform* ott);
extern void a3da_point_free(a3da_point* p);
extern void a3da_post_process_free(a3da_post_process* pp);
