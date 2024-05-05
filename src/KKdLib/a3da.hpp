/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "light_param/fog.hpp"
#include "light_param/light.hpp"
#include "kf.hpp"
#include "vec.hpp"

enum a3da_ambient_flags {
    A3DA_AMBIENT_LIGHT_DIFFUSE     = 0x01,
    A3DA_AMBIENT_RIM_LIGHT_DIFFUSE = 0x02,
};

enum a3da_camera_auxiliary_flags {
    A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE = 0x01,
    A3DA_CAMERA_AUXILIARY_EXPOSURE      = 0x02,
    A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE = 0x04,
    A3DA_CAMERA_AUXILIARY_GAMMA         = 0x08,
    A3DA_CAMERA_AUXILIARY_GAMMA_RATE    = 0x10,
    A3DA_CAMERA_AUXILIARY_SATURATE      = 0x20,
};

enum a3da_camera_root_view_point_flags {
    A3DA_CAMERA_ROOT_VIEW_POINT_FOV  = 0x01,
    A3DA_CAMERA_ROOT_VIEW_POINT_ROLL = 0x02,
};

enum a3da_compress_f16 {
    A3DA_COMPRESS_F32F32F32F32 = 0,
    A3DA_COMPRESS_I16F16F32F32 = 1,
    A3DA_COMPRESS_I16F16F16F16 = 2,
};

enum a3da_ep_type {
    A3DA_EP_NONE         = 0,
    A3DA_EP_LINEAR       = 1,
    A3DA_EP_CYCLE        = 2,
    A3DA_EP_CYCLE_OFFSET = 3,
}; // Pre/Post Infinity

enum a3da_event_type {
    A3DA_EVENT_MISC = 0x00,
    A3DA_EVENT_FILT = 0x01,
    A3DA_EVENT_FX   = 0x02,
    A3DA_EVENT_SND  = 0x03,
    A3DA_EVENT_MOT  = 0x04,
    A3DA_EVENT_A2D  = 0x05,
};

enum a3da_fog_flags {
    A3DA_FOG_COLOR   = 0x01,
    A3DA_FOG_DENSITY = 0x02,
    A3DA_FOG_END     = 0x04,
    A3DA_FOG_START   = 0x08,
};

enum a3da_format {
    A3DA_FORMAT_DT  = 0,
    A3DA_FORMAT_F,
    A3DA_FORMAT_AFT,
    A3DA_FORMAT_F2,
    A3DA_FORMAT_MGF,
    A3DA_FORMAT_X,
    A3DA_FORMAT_XHD,
    A3DA_FORMAT_AFT_X_PACK,
};

enum a3da_key_flags {
    A3DA_KEY_BIN_OFFSET = 0x01,
};

enum a3da_key_type {
    A3DA_KEY_NONE    = 0,
    A3DA_KEY_STATIC  = 1,
    A3DA_KEY_LINEAR  = 2,
    A3DA_KEY_HERMITE = 3,
    A3DA_KEY_HOLD    = 4,
};

enum a3da_light_flags {
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
};

enum a3da_material_list_flags {
    A3DA_MATERIAL_LIST_BLEND_COLOR    = 0x01,
    A3DA_MATERIAL_LIST_GLOW_INTENSITY = 0x02,
    A3DA_MATERIAL_LIST_EMISSION       = 0x04,
};

enum a3da_model_transform_flags {
    A3DA_MODEL_TRANSFORM_BIN_OFFSET = 0x01,
};

enum a3da_object_node_flags {
    A3DA_OBJECT_NODE_JOINT_ORIENT = 0x01,
};

enum a3da_object_texture_transform_flags {
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
};

enum a3da_play_control_flags {
    A3DA_PLAY_CONTROL_DIV    = 0x01,
    A3DA_PLAY_CONTROL_OFFSET = 0x02,
};

enum a3da_post_process_flags {
    A3DA_POST_PROCESS_INTENSITY  = 0x01,
    A3DA_POST_PROCESS_LENS_FLARE = 0x02,
    A3DA_POST_PROCESS_LENS_GHOST = 0x04,
    A3DA_POST_PROCESS_LENS_SHAFT = 0x08,
    A3DA_POST_PROCESS_RADIUS     = 0x10,
    A3DA_POST_PROCESS_SCENE_FADE = 0x20,
};

enum a3da_rgba_flags {
    A3DA_RGBA_R = 0x01,
    A3DA_RGBA_G = 0x02,
    A3DA_RGBA_B = 0x04,
    A3DA_RGBA_A = 0x08,
};

struct a3da_ambient;
struct a3da_camera_auxiliary;
struct a3da_camera_root;
struct a3da_camera_root_view_point;
struct a3da_chara;
struct a3da_curve;
struct a3da_dof;
struct a3da_event;
struct a3da_fog;
struct a3da_key;
struct a3da_light;
struct a3da_m_object_hrc;
struct a3da_material_list;
struct a3da_model_transform;
struct a3da_object;
struct a3da_object_hrc;
struct a3da_object_instance;
struct a3da_object_node;
struct a3da_object_texture_pattern;
struct a3da_object_texture_transform;
struct a3da_play_control;
struct a3da_point;
struct a3da_post_process;
struct a3da_rgba;
struct a3da_vec3;

struct a3da_key {
    a3da_key_flags flags;
    int32_t bin_offset;
    a3da_key_type type;
    a3da_ep_type ep_type_pre;
    a3da_ep_type ep_type_post;
    std::vector<kft3> keys;
    float_t max_frame;
    bool raw_data;
    bool raw_data_binary;
    int32_t raw_data_value_list_size;
    int32_t raw_data_value_list_offset;
    float_t value;

    a3da_key();
    ~a3da_key();

    a3da_key& operator=(const a3da_key& k);
};

struct a3da_rgba {
    a3da_rgba_flags flags;
    a3da_key r;
    a3da_key g;
    a3da_key b;
    a3da_key a;

    a3da_rgba();
    ~a3da_rgba();
};

struct a3da_vec3 {
    a3da_key x;
    a3da_key y;
    a3da_key z;

    a3da_vec3();
    ~a3da_vec3();
};

struct a3da_model_transform {
    a3da_model_transform_flags flags;
    int32_t bin_offset;
    a3da_vec3 rotation;
    a3da_vec3 scale;
    a3da_vec3 translation;
    a3da_key visibility;

    a3da_model_transform();
    ~a3da_model_transform();
};

struct a3da_ambient {
    a3da_ambient_flags flags;
    a3da_rgba light_diffuse;
    std::string name;
    a3da_rgba rim_light_diffuse;

    a3da_ambient();
    ~a3da_ambient();
};

struct a3da_camera_auxiliary {
    a3da_camera_auxiliary_flags flags;
    a3da_key auto_exposure;
    a3da_key exposure;
    a3da_key exposure_rate;
    a3da_key gamma;
    a3da_key gamma_rate;
    a3da_key saturate;

    a3da_camera_auxiliary();
    ~a3da_camera_auxiliary();
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

    a3da_camera_root_view_point();
    ~a3da_camera_root_view_point();
};

struct a3da_camera_root {
    a3da_model_transform interest;
    a3da_model_transform model_transform;
    a3da_camera_root_view_point view_point;

    a3da_camera_root();
    ~a3da_camera_root();
};

struct a3da_chara {
    a3da_model_transform model_transform;
    std::string name;

    a3da_chara();
    ~a3da_chara();
};

struct a3da_curve {
    a3da_key curve;
    std::string name;

    a3da_curve();
    ~a3da_curve();
};

struct a3da_dof {
    bool has_dof;
    a3da_model_transform model_transform;

    a3da_dof();
    ~a3da_dof();
};

struct a3da_event {
    float_t begin;
    float_t clip_begin;
    float_t clip_end;
    float_t end;
    std::string name;
    std::string param1;
    std::string ref;
    float_t time_ref_scale;
    a3da_event_type type;

    a3da_event();
    ~a3da_event();
};

struct a3da_fog {
    a3da_fog_flags flags;
    a3da_rgba color;
    a3da_key density;
    a3da_key end;
    fog_id id;
    a3da_key start;

    a3da_fog();
    ~a3da_fog();
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
    std::string type;

    a3da_light();
    ~a3da_light();
};

struct a3da_m_object_hrc {
    std::vector<a3da_object_instance> instance;
    a3da_model_transform model_transform;
    std::string name;
    std::vector<a3da_object_node> node;

    a3da_m_object_hrc();
    ~a3da_m_object_hrc();
};

struct a3da_material_list {
    a3da_material_list_flags flags;
    a3da_rgba blend_color;
    a3da_key glow_intensity;
    a3da_rgba emission;
    std::string name;

    a3da_material_list();
    ~a3da_material_list();
};

struct a3da_object {
    a3da_model_transform model_transform;
    std::string morph;
    float_t morph_offset;
    std::string name;
    std::string parent_name;
    std::string parent_node;
    std::string pattern;
    float_t pattern_offset;
    std::vector<a3da_object_texture_pattern> texture_pattern;
    std::vector<a3da_object_texture_transform> texture_transform;
    std::string uid_name;

    a3da_object();
    ~a3da_object();
};

struct a3da_object_hrc {
    std::string name;
    std::vector<a3da_object_node> node;
    std::string parent_name;
    std::string parent_node;
    bool shadow;
    std::string uid_name;

    a3da_object_hrc();
    ~a3da_object_hrc();
};

struct a3da_object_instance {
    a3da_model_transform model_transform;
    std::string name;
    bool shadow;
    std::string uid_name;

    a3da_object_instance();
    ~a3da_object_instance();
};

struct a3da_object_node {
    a3da_object_node_flags flags;
    vec3 joint_orient;
    a3da_model_transform model_transform;
    std::string name;
    int32_t parent;

    a3da_object_node();
    ~a3da_object_node();
};

struct a3da_object_texture_pattern {
    std::string name;
    std::string pattern;
    float_t pattern_offset;

    a3da_object_texture_pattern();
    ~a3da_object_texture_pattern();
};

struct a3da_object_texture_transform {
    a3da_object_texture_transform_flags flags;
    a3da_key coverage_u;
    a3da_key coverage_v;
    std::string name;
    a3da_key offset_u;
    a3da_key offset_v;
    a3da_key repeat_u;
    a3da_key repeat_v;
    a3da_key rotate;
    a3da_key rotate_frame;
    a3da_key translate_frame_u;
    a3da_key translate_frame_v;

    a3da_object_texture_transform();
    ~a3da_object_texture_transform();
};

struct a3da_play_control {
    a3da_play_control_flags flags;
    float_t begin;
    int32_t div;
    float_t fps;
    float_t offset;
    float_t size;

    a3da_play_control();
    ~a3da_play_control();
};

struct a3da_point {
    a3da_model_transform model_transform;
    std::string name;

    a3da_point();
    ~a3da_point();
};

struct a3da_post_process {
    a3da_post_process_flags flags;
    a3da_rgba intensity;
    a3da_key lens_flare;
    a3da_key lens_ghost;
    a3da_key lens_shaft;
    a3da_rgba radius;
    a3da_rgba scene_fade;

    a3da_post_process();
    ~a3da_post_process();
};

struct a3da {
    bool ready;
    bool compressed;
    a3da_format format;

    a3da_compress_f16 _compress_f16;
    std::string _file_name;
    std::string _property_version;
    std::string _converter_version;

    std::vector<a3da_ambient> ambient;
    std::vector<std::string> auth_2d;
    a3da_camera_auxiliary camera_auxiliary;
    std::vector<a3da_camera_root> camera_root;
    std::vector<a3da_chara> chara;
    std::vector<a3da_curve> curve;
    a3da_dof dof;
    std::vector<a3da_event> event;
    std::vector<a3da_fog> fog;
    std::vector<a3da_light> light;
    std::vector<a3da_m_object_hrc> m_object_hrc;
    std::vector<std::string> m_object_hrc_list;
    std::vector<a3da_material_list> material_list;
    std::vector<std::string> motion;
    std::vector<a3da_object> object;
    std::vector<a3da_object_hrc> object_hrc;
    std::vector<std::string> object_hrc_list;
    std::vector<std::string> object_list;
    a3da_play_control play_control;
    std::vector<a3da_point> point;
    a3da_post_process post_process;

    a3da();
    ~a3da();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
