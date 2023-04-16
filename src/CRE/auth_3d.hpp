/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/database/texture.hpp"
#include "../KKdLib/a3da.hpp"
#include "../KKdLib/kf.hpp"
#include "frame_rate_control.hpp"
#include "render_context.hpp"

enum auth_3d_ambient_flags {
    AUTH_3D_AMBIENT_LIGHT_DIFFUSE     = 0x01,
    AUTH_3D_AMBIENT_RIM_LIGHT_DIFFUSE = 0x02,
};

enum auth_3d_camera_auxiliary_flags {
    AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE = 0x01,
    AUTH_3D_CAMERA_AUXILIARY_EXPOSURE      = 0x02,
    AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE = 0x04,
    AUTH_3D_CAMERA_AUXILIARY_GAMMA         = 0x08,
    AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE    = 0x10,
    AUTH_3D_CAMERA_AUXILIARY_SATURATE      = 0x20,
};

enum auth_3d_camera_root_view_point_flags {
    AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV  = 0x01,
    AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL = 0x02,
};

enum auth_3d_compress_f16 {
    AUTH_3D_COMPRESS_F32F32F32F32 = 0,
    AUTH_3D_COMPRESS_I16F16F32F32 = 1,
    AUTH_3D_COMPRESS_I16F16F16F16 = 2,
};

enum auth_3d_ep_type {
    AUTH_3D_EP_NONE         = 0,
    AUTH_3D_EP_LINEAR       = 1,
    AUTH_3D_EP_CYCLE        = 2,
    AUTH_3D_EP_CYCLE_OFFSET = 3,
}; // Pre/Post Infinity

enum auth_3d_fog_flags {
    AUTH_3D_FOG_COLOR   = 0x01,
    AUTH_3D_FOG_DENSITY = 0x02,
    AUTH_3D_FOG_END     = 0x04,
    AUTH_3D_FOG_START   = 0x08,
};

enum auth_3d_format {
    AUTH_3D_FORMAT_DT  = 0,
    AUTH_3D_FORMAT_F   = 1,
    AUTH_3D_FORMAT_FT  = 2,
    AUTH_3D_FORMAT_F2  = 3,
    AUTH_3D_FORMAT_MGF = 4,
    AUTH_3D_FORMAT_X   = 5,
    AUTH_3D_FORMAT_XHD = 6,
};

enum auth_3d_key_flags {
    AUTH_3D_KEY_BIN_OFFSET = 0x01,
};

enum auth_3d_key_type {
    AUTH_3D_KEY_NONE    = 0,
    AUTH_3D_KEY_STATIC  = 1,
    AUTH_3D_KEY_LINEAR  = 2,
    AUTH_3D_KEY_HERMITE = 3,
    AUTH_3D_KEY_HOLD    = 4,
};

enum auth_3d_light_flags {
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
};

enum auth_3d_material_list_flags {
    AUTH_3D_MATERIAL_LIST_BLEND_COLOR    = 0x01,
    AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY = 0x02,
    AUTH_3D_MATERIAL_LIST_EMISSION       = 0x04,
};

enum auth_3d_model_transform_flags {
    AUTH_3D_MODEL_TRANSFORM_BIN_OFFSET = 0x01,
};

enum auth_3d_object_node_flags {
    AUTH_3D_OBJECT_NODE_JOINT_ORIENT = 0x01,
};

enum auth_3d_object_texture_transform_flags {
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
};

enum auth_3d_play_control_flags {
    AUTH_3D_PLAY_CONTROL_DIV    = 0x01,
    AUTH_3D_PLAY_CONTROL_OFFSET = 0x02,
};

enum auth_3d_post_process_flags {
    AUTH_3D_POST_PROCESS_INTENSITY  = 0x01,
    AUTH_3D_POST_PROCESS_LENS_FLARE = 0x02,
    AUTH_3D_POST_PROCESS_LENS_GHOST = 0x04,
    AUTH_3D_POST_PROCESS_LENS_SHAFT = 0x08,
    AUTH_3D_POST_PROCESS_RADIUS     = 0x10,
    AUTH_3D_POST_PROCESS_SCENE_FADE = 0x20,
};

enum auth_3d_rgba_flags {
    AUTH_3D_RGBA_R = 0x01,
    AUTH_3D_RGBA_G = 0x02,
    AUTH_3D_RGBA_B = 0x04,
    AUTH_3D_RGBA_A = 0x08,
};

struct auth_3d;
struct auth_3d_ambient;
struct auth_3d_camera_auxiliary;
struct auth_3d_camera_root;
struct auth_3d_camera_root_view_point;
struct auth_3d_chara;
struct auth_3d_curve;
struct auth_3d_dof;
struct auth_3d_event;
struct auth_3d_fog;
struct auth_3d_key;
struct auth_3d_light;
struct auth_3d_m_object_hrc;
struct auth_3d_material_list;
struct auth_3d_model_transform;
struct auth_3d_object;
struct auth_3d_object_curve;
struct auth_3d_object_hrc;
struct auth_3d_object_instance;
struct auth_3d_object_model_transform;
struct auth_3d_object_node;
struct auth_3d_object_texture_pattern;
struct auth_3d_object_texture_transform;
struct auth_3d_play_control;
struct auth_3d_point;
struct auth_3d_post_process;
struct auth_3d_rgba;
struct auth_3d_vec3;

namespace auth_3d_detail {
    class Event {
    public:
        enum Type {
            MISC = 0x00,
            FILT = 0x01,
            FX   = 0x02,
            SND  = 0x03,
            MOT  = 0x04,
            A2D  = 0x05,
        };

        bool active;
        Type type;
        std::string name;
        float_t begin;
        float_t end;
        std::string param1;
        std::string ref;
        size_t index;

        Event(a3da_event* e);
        virtual ~Event();

        virtual void Load(auth_3d* auth);
        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx);
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx);
        virtual void Ctrl(auth_3d* auth, int32_t flags, render_context* rctx);
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx);
    };

    struct EventA2d : public Event {
    public:
        //std::pair<AetObjInitData, int32_t> aet_init_data;

        EventA2d(a3da_event* e);
        virtual ~EventA2d() override;

        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx) override;
    };

    struct EventFX : public Event {
    public:
        EventFX(a3da_event* e);
        virtual ~EventFX() override;

        virtual void Load(auth_3d* auth) override;
        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) override;
    };

    struct EventFXSmoothCut : public Event {
    public:
        bool cam;

        EventFXSmoothCut(a3da_event* e);
        virtual ~EventFXSmoothCut() override;

        virtual void Load(auth_3d* auth) override;
        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx) override;
    };

    struct EventFilterFade : public Event {
    public:
        enum Type {
            FADE_NONE = -1,
            FADE_OUT  = 0,
            FADE_IN   = 1,
        };

        Type type;
        vec3 color;

        EventFilterFade(a3da_event* e);
        virtual ~EventFilterFade() override;

        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx) override;
    };

    struct EventFilterTimeStop : public Event {
    public:
        EventFilterTimeStop(a3da_event* e);
        virtual ~EventFilterTimeStop() override;

        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx) override;
    };

    struct EventMot : public Event {
    public:
        EventMot(a3da_event* e);
        virtual ~EventMot() override;

        virtual void Load(auth_3d* auth) override;
        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx) override;
    };

    struct EventSnd : public Event {
    public:
        bool field_190;

        EventSnd(a3da_event* e);
        virtual ~EventSnd() override;

        virtual void Load(auth_3d* auth) override;
        virtual void Begin(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void End(auth_3d* auth, int32_t flags, render_context* rctx) override;
        virtual void Disp(auth_3d* auth, const mat4* mat, render_context* rctx) override;
    };
}

struct auth_3d_key {
    auth_3d_key_type type;
    float_t value;
    float_t max_frame;
    auth_3d_ep_type ep_type_pre;
    auth_3d_ep_type ep_type_post;
    float_t frame_delta;
    float_t value_delta;
    std::vector<kft3> keys_vec;
    size_t length;
    kft3* keys;

    float_t frame;
    float_t value_interp;
    float_t value_init;

    auth_3d_key();
    auth_3d_key(const auth_3d_key& k);
    ~auth_3d_key();

    float_t interpolate(float_t frame);
    void reset();

    auth_3d_key& operator=(const auth_3d_key& k);
};

struct auth_3d_rgba {
    auth_3d_rgba_flags flags;
    auth_3d_key r;
    auth_3d_key g;
    auth_3d_key b;
    auth_3d_key a;

    vec4 value;

    auth_3d_rgba();
    ~auth_3d_rgba();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_vec3 {
    auth_3d_key x;
    auth_3d_key y;
    auth_3d_key z;

    auth_3d_vec3();
    ~auth_3d_vec3();

    vec3 interpolate(float_t frame);
    void reset();
};

struct auth_3d_model_transform {
    mat4 mat;
    auth_3d_vec3 scale;
    auth_3d_vec3 rotation;
    auth_3d_vec3 translation;
    auth_3d_key visibility;

    vec3 scale_value;
    vec3 rotation_value;
    vec3 translation_value;
    bool visible;

    auth_3d_model_transform();
    ~auth_3d_model_transform();

    void interpolate(float_t frame);
    void reset();
    void set_mat(const mat4* parent_mat);
};

struct auth_3d_ambient {
    auth_3d_ambient_flags flags;
    auth_3d_rgba light_diffuse;
    std::string name;
    auth_3d_rgba rim_light_diffuse;

    auth_3d_ambient();
    ~auth_3d_ambient();

    void interpolate(float_t frame);
    void reset();
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

    auth_3d_camera_auxiliary();
    ~auth_3d_camera_auxiliary();

    void interpolate(float_t frame);
    void reset();
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

    auth_3d_camera_root_view_point();
    ~auth_3d_camera_root_view_point();

    void reset();
};

struct auth_3d_camera_root {
    auth_3d_model_transform interest;
    auth_3d_model_transform model_transform;
    auth_3d_camera_root_view_point view_point;

    float_t fov_value;
    vec3 interest_value;
    float_t roll_value;
    vec3 view_point_value;

    auth_3d_camera_root();
    ~auth_3d_camera_root();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_chara {
    auth_3d_model_transform model_transform;
    std::string name;
    int32_t index;

    auth_3d_chara();
    ~auth_3d_chara();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_curve {
    auth_3d_key curve;
    std::string name;
    float_t value;

    auth_3d_curve();
    ~auth_3d_curve();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_dof {
    bool has_dof;
    auth_3d_model_transform model_transform;

    auth_3d_dof();
    ~auth_3d_dof();

    void interpolate(float_t frame);
    void reset();
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

    vec4 color_init;
    float_t density_init;
    float_t end_init;
    float_t start_init;

    auth_3d_fog();
    ~auth_3d_fog();

    void interpolate(float_t frame);
    void reset();
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
    std::string type;

    float_t cone_angle_value;
    float_t constant_value;
    float_t drop_off_value;
    float_t far_value;
    float_t intensity_value;
    float_t linear_value;
    float_t quadratic_value;

    vec4 ambient_init;
    float_t cone_angle_init;
    float_t constant_init;
    vec4 diffuse_init;
    float_t drop_off_init;
    float_t far_init;
    float_t intensity_init;
    float_t linear_init;
    float_t quadratic_init;
    vec4 specular_init;
    light_tone_curve tone_curve_init;

    auth_3d_light();
    ~auth_3d_light();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_object_model_transform {
    mat4 mat;
    mat4 mat_inner;
    mat4 mat_rot;
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

    auth_3d_object_model_transform();
    ~auth_3d_object_model_transform();

    void interpolate(float_t frame);
    void mat_mult(const mat4* mat);
    void reset();
};

struct auth_3d_m_object_hrc {
    std::vector<auth_3d_object_instance> instance;
    auth_3d_object_model_transform model_transform;
    std::string name;
    std::vector<auth_3d_object_node> node;

    auth_3d_m_object_hrc();
    ~auth_3d_m_object_hrc();

    void interpolate(float_t frame);
    void list_ctrl(const mat4* parent_mat);
    void reset();
};

struct auth_3d_material_list {
    auth_3d_material_list_flags flags;
    auth_3d_rgba blend_color;
    auth_3d_key glow_intensity;
    auth_3d_rgba emission;
    std::string name;
    uint32_t hash;

    float_t glow_intensity_value;

    auth_3d_material_list();
    ~auth_3d_material_list();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_object_curve {
    auth_3d_curve* curve;
    std::string name;
    float_t frame_offset;
    float_t value;

    auth_3d_object_curve();
    ~auth_3d_object_curve();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_object {
    std::vector<auth_3d_object*> children_object;
    std::vector<auth_3d_object_hrc*> children_object_hrc;
    auth_3d_model_transform model_transform;
    auth_3d_object_curve morph;
    object_info object_info;
    uint32_t object_hash;
    std::string name;
    std::string parent_name;
    std::string parent_node;
    auth_3d_object_curve pattern;
    bool reflect;
    bool refract;
    std::vector<auth_3d_object_texture_pattern> texture_pattern;
    std::vector<auth_3d_object_texture_transform> texture_transform;
    std::string uid_name;

    auth_3d_object();
    ~auth_3d_object();

    void interpolate(float_t frame);
    void list_ctrl(const mat4* parent_mat);
    void reset();
};

struct auth_3d_object_hrc {
    std::vector<auth_3d_object*> children_object;
    std::vector<auth_3d_object_hrc*> children_object_hrc;
    std::vector<int32_t> children_object_parent_node;
    std::vector<int32_t> children_object_hrc_parent_node;
    std::vector<mat4> mats;
    std::string name;
    std::vector<auth_3d_object_node> node;
    object_info object_info;
    uint32_t object_hash;
    std::string parent_name;
    std::string parent_node;
    bool reflect;
    bool refract;
    bool shadow;
    std::string uid_name;

    auth_3d_object_hrc();
    ~auth_3d_object_hrc();

    int32_t get_node_index(const char* node);
    void interpolate(float_t frame);
    void list_ctrl(const mat4* parent_mat);
    void reset();
};

struct auth_3d_object_instance {
    std::vector<mat4> mats;
    auth_3d_object_model_transform model_transform;
    std::string name;
    std::vector<int32_t> object_bone_indices;
    object_info object_info;
    uint32_t object_hash;
    bool shadow;
    std::string uid_name;

    auth_3d_object_instance();
    ~auth_3d_object_instance();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_object_node {
    auth_3d_object_node_flags flags;
    int32_t bone_id;
    vec3 joint_orient;
    mat4 joint_orient_mat;
    mat4* mat;
    auth_3d_object_model_transform model_transform;
    std::string name;
    int32_t parent;

    auth_3d_object_node();
    ~auth_3d_object_node();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_object_texture_pattern {
    std::string name;
    auth_3d_object_curve pattern;
    int32_t texture_id;

    auth_3d_object_texture_pattern();
    ~auth_3d_object_texture_pattern();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_object_texture_transform {
    auth_3d_object_texture_transform_flags flags;
    auth_3d_key coverage_u;
    auth_3d_key coverage_v;
    mat4 mat;
    std::string name;
    auth_3d_key offset_u;
    auth_3d_key offset_v;
    auth_3d_key repeat_u;
    auth_3d_key repeat_v;
    auth_3d_key rotate;
    auth_3d_key rotate_frame;
    int32_t texture_id;
    auth_3d_key translate_frame_u;
    auth_3d_key translate_frame_v;

    auth_3d_object_texture_transform();
    ~auth_3d_object_texture_transform();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_play_control {
    auth_3d_play_control_flags flags;
    float_t begin;
    int32_t div;
    float_t fps;
    float_t offset;
    float_t size;

    auth_3d_play_control();
    ~auth_3d_play_control();

    void reset();
};

struct auth_3d_point {
    auth_3d_model_transform model_transform;
    std::string name;

    auth_3d_point();
    ~auth_3d_point();

    void interpolate(float_t frame);
    void reset();
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
    vec4 scene_fade_init;

    auth_3d_post_process();
    ~auth_3d_post_process();

    void interpolate(float_t frame);
    void reset();
};

struct auth_3d_time_event {
    float_t frame;
    int32_t type;
    size_t event_index;
};

struct auth_3d_id {
    int32_t id;

    inline auth_3d_id() {
        id = -1;
    }

    inline auth_3d_id(int32_t id) {
        this->id = id;
    }

    inline bool is_null() {
        return id == -1;
    }

    inline bool not_null() {
        return id != -1;
    }

    bool check_not_empty();
    bool check_loading();
    bool check_loaded();
    auth_3d* get_auth_3d();
    mat4* get_auth_3d_object_mat(size_t index, bool hrc, mat4* mat);
    int32_t get_chara_id();
    bool get_enable();
    bool get_ended();
    float_t get_frame();
    float_t get_frame_offset();
    float_t get_last_frame();
    bool get_left_right_reverse();
    bool get_paused();
    float_t get_play_control_begin();
    float_t get_play_control_size();
    bool get_repeat();
    int32_t get_uid();
    void read_file(auth_3d_database* auth_3d_db);
    void read_file_modern();
    void set_camera_root_update(bool value);
    void set_chara_id(int32_t value);
    void set_chara_item(bool value);
    void set_obj_flags_alpha(mdl::ObjFlags obj_flags, float_t alpha);
    void set_enable(bool value);
    void set_frame_rate(FrameRateControl* value);
    void set_last_frame(float_t value);
    void set_left_right_reverse(bool value);
    void set_mat(const mat4& value);
    void set_max_frame(float_t value);
    void set_paused(bool value);
    void set_pos(int32_t value);
    void set_repeat(bool value);
    void set_req_frame(float_t value);
    void set_shadow(bool value);
    void set_src_dst_chara(int32_t src_chara, int32_t dst_chara);
    void set_visibility(bool value);
    void unload_id(render_context* rctx);
};

struct auth_3d {
    int32_t uid;
    int32_t id;
    bool enable;
    bool camera_root_update;
    bool visible;
    bool repeat;
    bool ended;
    bool left_right_reverse;
    bool once;
    mat4 mat;
    float_t alpha;
    mdl::ObjFlags obj_flags;
    int32_t chara_id;
    bool chara_item;
    bool shadow;
    int32_t src_chara;
    int32_t dst_chara;
    int32_t pos;
    FrameRateControl* frame_rate;
    float_t frame;
    float_t req_frame;
    float_t max_frame;
    bool frame_changed;
    float_t frame_offset;
    float_t last_frame;
    bool paused;

    std::vector<auth_3d_ambient> ambient;
    std::vector<std::string> auth_2d;
    auth_3d_camera_auxiliary camera_auxiliary;
    std::vector<auth_3d_camera_root> camera_root;
    std::vector<auth_3d_chara> chara;
    std::vector<auth_3d_curve> curve;
    auth_3d_dof dof;
    std::vector<auth_3d_detail::Event*> event;
    std::vector<auth_3d_time_event> event_time;
    auth_3d_time_event* event_time_next;
    std::vector<auth_3d_fog> fog;
    std::vector<auth_3d_light> light;
    std::vector<auth_3d_m_object_hrc> m_object_hrc;
    std::vector<auth_3d_m_object_hrc*> m_object_hrc_list;
    std::vector<auth_3d_material_list> material_list;
    std::vector<std::string> motion;
    std::vector<auth_3d_object> object;
    std::vector<auth_3d_object_hrc> object_hrc;
    std::vector<auth_3d_object_hrc*> object_hrc_list;
    std::vector<auth_3d_object*> object_list;
    auth_3d_play_control play_control;
    std::vector<auth_3d_point> point;
    auth_3d_post_process post_process;
    int32_t state;

    std::string file_name;
    uint32_t hash;
    object_database* obj_db;
    texture_database* tex_db;

    auth_3d();
    ~auth_3d();

    void ctrl(render_context* rctx);
    void disp(render_context* rctx);
    void load(a3da* auth_file,
        object_database* obj_db, texture_database* tex_db);
    void load_from_farc(farc* f, const char* file,
        object_database* obj_db, texture_database* tex_db);
    void reset();
    void unload(render_context* rctx);
};

#define AUTH_3D_DATA_COUNT 0x100

struct auth_3d_farc {
    int32_t load_count;
    std::string name;
    int32_t state;
    std::string path;
    std::string file;
    p_file_handler file_handler;
    farc* farc;
    const void* data;
    size_t size;

    auth_3d_farc();
    ~auth_3d_farc();
};

struct auth_3d_uid_file {
    int32_t load_count;
    int32_t uid;
    std::string file_name;
    int32_t state;
    const char* name;
    auth_3d_farc* farc;

    auth_3d_uid_file();
    ~auth_3d_uid_file();
};

struct auth_3d_uid_file_modern {
    int32_t load_count;
    uint32_t hash;
    std::string file_name;
    int32_t state;
    std::string name;
    auth_3d_farc* farc;
    void* data;
    object_database* obj_db;
    texture_database* tex_db;

    auth_3d_uid_file_modern();
    ~auth_3d_uid_file_modern();
};

struct auth_3d_data_struct {
    std::vector<auth_3d_farc> farcs;
    std::map<uint32_t, auth_3d_farc> farcs_modern;
    std::vector<auth_3d_uid_file> uid_files;
    std::map<uint32_t, auth_3d_uid_file_modern> uid_files_modern;
    std::vector<int32_t> loaded_ids;
    auth_3d data[AUTH_3D_DATA_COUNT];

    auth_3d_data_struct();
    ~auth_3d_data_struct();
};

extern auth_3d_data_struct* auth_3d_data;

extern void auth_3d_data_init();
extern bool auth_3d_data_check_category_loaded(const char* category_name);
extern bool auth_3d_data_check_category_loaded(uint32_t category_hash);
extern int32_t auth_3d_data_get_auth_3d_id(const char* object_name);
extern int32_t auth_3d_data_get_auth_3d_id(object_info obj_info,
    int32_t* object_index, bool* hrc, int32_t instance = -1);
extern int32_t auth_3d_data_get_auth_3d_id(uint32_t file_name_hash, uint32_t object_hash,
    int32_t* object_index, bool* hrc, int32_t instance = -1);
void auth_3d_data_get_obj_sets_from_category(std::string& name, std::vector<uint32_t>& obj_sets,
    auth_3d_database* auth_3d_db, object_database* obj_db);
extern const char* auth_3d_data_get_uid_name(int32_t uid, auth_3d_database* auth_3d_db);
extern void auth_3d_data_load_auth_3d_db(auth_3d_database* auth_3d_db);
extern void auth_3d_data_load_category(const char* category_name, const char* mdata_dir = 0);
extern void auth_3d_data_load_category(void* data, const char* category_name, uint32_t category_hash);
extern auth_3d_id auth_3d_data_load_hash(uint32_t hash, void* data, object_database* obj_db, texture_database* tex_db);
extern auth_3d_id auth_3d_data_load_uid(int32_t uid, auth_3d_database* auth_3d_db);
extern void auth_3d_data_unload_category(const char* category_name);
extern void auth_3d_data_unload_category(uint32_t category_hash);
extern void auth_3d_data_free();

extern void task_auth_3d_init();
extern bool task_auth_3d_add_task();
extern bool task_auth_3d_check_task_ready();
extern bool task_auth_3d_del_task();
extern void task_auth_3d_free();
