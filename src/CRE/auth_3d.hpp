/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/auth_3d.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/database/texture.hpp"
#include "../KKdLib/a3da.hpp"
#include "../KKdLib/kf.hpp"
#include "prj/memory_manager.hpp"
#include "auth_2d.hpp"
#include "frame_rate_control.hpp"

#define AUTH_3D_DATA_COUNT 0x100

namespace auth_3d_detail {
    enum EventActionFlag {
        EAF_PASS          = 0x0001,
        EAF_JUMP_BACKWARD = 0x0002,
        EAF_JUMP_FORWARD  = 0x0004,
        EAF_DESTROY       = 0x8000,
    };

    enum EVENT_TYPE {
        EVENT_MISC = 0,
        EVENT_FLT,
        EVENT_FX,
        EVENT_SND,
        EVENT_MOT,
        EVENT_A2D,
    };

    // Pre/Post Infinity
    enum EP_TYPE {
        EP_TYPE_CONSTANT = 0,
        EP_TYPE_LINEAR,
        EP_TYPE_CYCLE,
        EP_TYPE_CYCLE_WITH_OFFSET,
    };

    enum FC_TYPE {
        FC_TYPE_STATIC_0 = 0,
        FC_TYPE_STATIC_DATA,
        FC_TYPE_LINEAR,
        FC_TYPE_HERMITE,
        FC_TYPE_NO_INTERPOLATION,
    };

    enum LoadState {
        S_BEFORE_LOADING = 0,
        S_NOW_LOADING,
        S_DONE,
    };
};

namespace mdl {
    enum ObjFlags : uint32_t;
}

struct CameraParam;

struct farc;
struct render_context;

namespace auth_3d_detail {
    class Ambient;
    class CameraAuxiliary;
    class CameraRoot;
    class CameraViewPoint;
    class Chara;
    class Color;
    class Curve;
    class CurveWithOffset;
    class DB;
    class Dof;
    class Event;
    class Fog;
    class Fcurve;
    class Fcurve3f;
    class FcurveColor4f;
    class Handle;
    class HierarchyObject;
    class Light;
    class MaterialList;
    class ModelTransform;
    class Motion;
    class MotionTransform;
    class MultiHierarchyObject;
    class Object;
    class ObjectInstance;
    class PlayControl;
    class Point;
    class PostProcess;
    class Scene;
    class SceneFile;
    class Texture;
    class TextureTransform;
    class TransformNode;
};

namespace a3d {
    class EventListener {
    public:
        EventListener();
        virtual ~EventListener();

        virtual void loadFinished(auth_3d_detail::Handle& in_handle) = 0;
        virtual void playStart(auth_3d_detail::Handle& in_handle) = 0;
        virtual void playFinished(auth_3d_detail::Handle& in_handle) = 0;
        virtual void playLoopJumpAfter(auth_3d_detail::Handle& in_handle) = 0;
        virtual void playLoopJumpBefore(auth_3d_detail::Handle& in_handle) = 0;
    };

    class EventAdapter : public EventListener {
    public:
        EventAdapter();
        virtual ~EventAdapter() override;

        virtual void loadFinished(auth_3d_detail::Handle& in_handle) override;
        virtual void playStart(auth_3d_detail::Handle& in_handle) override;
        virtual void playFinished(auth_3d_detail::Handle& in_handle) override;
        virtual void playLoopJumpAfter(auth_3d_detail::Handle& in_handle) override;
        virtual void playLoopJumpBefore(auth_3d_detail::Handle& in_handle) override;
    };
}

namespace auth_3d_detail {
    class Fcurve {
    public:
        struct KeyData {
            float_t frame;
            float_t value;
            float_t l_slope;
            float_t r_slope;

            const KeyData* next() const;
            KeyData* next();
            const KeyData* prev() const;
            KeyData* prev();

            static float_t S_calc(FC_TYPE in_type, float_t in_frame,
                const KeyData* curr_key, const KeyData* next_key);

            inline KeyData() : frame(), value(), l_slope(), r_slope() {}
            inline KeyData(float_t in_frame)
                : frame(in_frame), value(), l_slope(), r_slope() {}
            inline KeyData(float_t in_frame, float_t in_value)
                : frame(in_frame), value(in_value), l_slope(), r_slope() {}
            inline KeyData(float_t in_frame, float_t in_value, float_t in_slope)
                : frame(in_frame), value(in_value), l_slope(in_slope), r_slope(in_slope) {}
            inline KeyData(float_t in_frame, float_t in_value, float_t in_l_slope, float_t in_r_slope)
                : frame(in_frame), value(in_value), l_slope(in_l_slope), r_slope(in_r_slope) {}
            inline KeyData(const kft3& other)
                : frame(other.frame), value(other.value), l_slope(other.l_slope), r_slope(other.r_slope) {}

            inline operator kft3() const {
                return { frame, value, l_slope, r_slope };
            }
        };

        FC_TYPE m_type;
        float_t m_static_value;
        float_t m_max_frame;
        EP_TYPE m_ep_type_pre;
        EP_TYPE m_ep_type_post;
        float_t m_frame_delta;
        float_t m_value_delta;
        std::vector<KeyData> m_key_data_list;
        size_t m_key_data_size;
        const KeyData* m_key_data_ptr;

        // Added
        float_t m_frame;
        float_t m_value;

    private:
        void M_init();
        float_t M_get(float_t in_frame) const;
        void M_set_ep_type(EP_TYPE in_ep_type_pre, EP_TYPE in_ep_type_post);

    public:
        Fcurve();
        Fcurve(float_t in_value);
        Fcurve(const Fcurve& other);
        ~Fcurve();

        void init();
        void init(float_t in_value);
        float_t get(float_t in_frame);
        float_t operator()(float_t in_frame);
        const KeyData* find_key_data(float_t in_frame) const;
        float_t get_size() const;
        bool check_discontinuity(float_t in_frame) const;
        bool check_threshold_discontinuity(float_t in_frame, float_t in_threshold = 0.3f) const;

        bool load(const SceneFile& in_file, const a3d::Fcurve& in_fcv);
        void store(const SceneFile& in_file, a3d::Fcurve& out_fcv) const; // Added

        static bool check_ep_range(float_t in_frame_begin, float_t in_frame_end,
            float_t in_key_frame_begin, float_t in_key_frame_end);

        Fcurve& operator=(const Fcurve& other);
    };

    class Fcurve3f {
    public:
        Fcurve x;
        Fcurve y;
        Fcurve z;

        Fcurve3f();
        Fcurve3f(float_t in_value);
        Fcurve3f(const Fcurve3f& other);
        ~Fcurve3f();

        void init();
        void init(float_t in_value);
        vec3 get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::Fcurve3f& in_fcv3f);
        void store(const SceneFile& in_file, a3d::Fcurve3f& out_fcv3f) const; // Added

        Fcurve3f& operator=(const Fcurve3f& other);
    };

    class FcurveColor4f {
    public:
        Fcurve r;
        Fcurve g;
        Fcurve b;
        Fcurve a;
        bool has_r;
        bool has_g;
        bool has_b;
        bool has_a;

        FcurveColor4f();
        FcurveColor4f(const FcurveColor4f& other);
        ~FcurveColor4f();

        void init();
        vec4 get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::FcurveColor4f& in_fcv_col4f);
        void store(const SceneFile& in_file, a3d::FcurveColor4f& out_fcv_col4f) const; // Added

        FcurveColor4f& operator=(const FcurveColor4f& other);
    };

    class Color {
    public:
        FcurveColor4f fcurve_color;
        vec4 color;
        bool has_color[4];

        Color();
        Color(const Color& other);
        ~Color();

        void init();
        void get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::FcurveColor4f& in_fcv_col4f);
        void store(const SceneFile& in_file, a3d::FcurveColor4f& out_fcv_col4f) const; // Added

        Color& operator=(const Color& other);
    };

    class ModelTransform {
    public:
        mat4 matrix;
        Fcurve3f fcurve_scale;
        Fcurve3f fcurve_rotation;
        Fcurve3f fcurve_translation;
        Fcurve fcurve_visibility;

        vec3 scale;
        vec3 rotation;
        vec3 translation;
        bool visibility;

        ModelTransform();
        ModelTransform(const ModelTransform& other);
        ~ModelTransform();

        void init();
        void init_static();
        void get(float_t in_frame);
        const mat4& mul(const mat4& in_base_mat);
        const mat4& get_matrix();

        bool load(const SceneFile& in_file, const a3d::ModelTransform& in_mt);
        void store(const SceneFile& in_file, a3d::ModelTransform& out_mt) const; // Added

        ModelTransform& operator=(const ModelTransform& other);
    };

    // MGF
    class Ambient {
    public:
        enum {
            FLAG_LIGHT_DIFFUSE     = 0x01,
            FLAG_RIM_LIGHT_DIFFUSE = 0x02,
        };

        uint32_t flag;
        std::string name;
        Color light_diffuse;
        Color rim_light_diffuse;

        Ambient();
        Ambient(const Ambient& other);
        ~Ambient();

        void init();
        bool has_light_diffuse() const;
        bool has_rim_light_diffuse() const;
        void get(float_t in_frame);
        void ctrl(render_context* rctx);

        bool load(const SceneFile& in_file, const a3d::Ambient& in_a);
        void store(const SceneFile& in_file, a3d::Ambient& out_a) const; // Added

        Ambient& operator=(const Ambient& other);
    };

    class Auth2d {
    public:
        std::string name;

        Auth2d();
        Auth2d(const Auth2d& other);
        ~Auth2d();

        bool load(const SceneFile& in_file, const a3d::Auth2d& in_a2);
        void store(const SceneFile& in_file, a3d::Auth2d& out_a2) const; // Added

        Auth2d& operator=(const Auth2d& other);
    };

    class CameraAuxiliary {
    public:
        enum {
            FLAG_EXPOSURE      = 0x01,
            FLAG_EXPOSURE_RATE = 0x02, // F/F2/X/XHD
            FLAG_GAMMA         = 0x04,
            FLAG_GAMMA_RATE    = 0x08, // F/F2/X/XHD
            FLAG_SATURATE      = 0x10,
            FLAG_AUTO_EXPOSURE = 0x20,
        };

        uint32_t flag;
        Fcurve fcurve_exposure;
        Fcurve fcurve_exposure_rate; // F/F2/X/XHD
        Fcurve fcurve_gamma;
        Fcurve fcurve_gamma_rate; // F/F2/X/XHD
        Fcurve fcurve_saturate;
        Fcurve fcurve_auto_exposure;

        float_t exposure;
        float_t exposure_rate; // F/F2/X/XHD
        float_t gamma;
        float_t gamma_rate; // F/F2/X/XHD
        float_t saturate;
        float_t auto_exposure;

        CameraAuxiliary();
        CameraAuxiliary(const CameraAuxiliary& other);
        ~CameraAuxiliary();

        void init();
        bool has_exposure() const;
        bool has_exposure_rate() const; // F/F2/X/XHD
        bool has_gamma() const;
        bool has_gamma_rate() const; // F/F2/X/XHD
        bool has_saturate() const;
        bool has_auto_exposure() const;
        void get(float_t in_frame);
        void ctrl(Scene& in_scene, render_context* rctx);
        void dest(render_context* rctx);

        bool load(const SceneFile& in_file, const a3d::CameraAuxiliary& in_ca);
        void store(const SceneFile& in_file, a3d::CameraAuxiliary& out_ca) const; // Added

        CameraAuxiliary& operator=(const CameraAuxiliary& other);
    };

    class CameraInterest {
    public:
        ModelTransform model_transform;

        CameraInterest();
        CameraInterest(const CameraInterest& other);
        ~CameraInterest();

        void init();

        bool load(const SceneFile& in_file, const a3d::CameraInterest& in_ci);
        void store(const SceneFile& in_file, a3d::CameraInterest& in_ci) const; // Added

        CameraInterest& operator=(const CameraInterest& other);
    };

    class CameraViewPoint {
    public:
        ModelTransform model_transform;
        Fcurve roll;
        bool has_fov;
        bool fov_is_horizontal;
        Fcurve fov;
        float_t aspect;
        Fcurve focal_length;
        float_t camera_aperture_w;
        float_t camera_aperture_h;

        CameraViewPoint();
        CameraViewPoint(const CameraViewPoint& other);
        ~CameraViewPoint();

        void init();
        float_t get_v_fov(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::CameraViewPoint& in_cvp);
        void store(const SceneFile& in_file, a3d::CameraViewPoint& out_cvp) const; // Added

        CameraViewPoint& operator=(const CameraViewPoint& other);
    };

    class CameraRoot {
    public:
        ModelTransform model_transform;
        CameraInterest interest;
        CameraViewPoint view_point;

        CameraRoot();
        CameraRoot(const CameraRoot& other);
        ~CameraRoot();

        void init();
        CameraParam get(float_t in_frame, const Scene& in_scene, render_context* rctx);

        bool load(const SceneFile& in_file, const a3d::CameraRoot& in_cr);
        void store(const SceneFile& in_file, a3d::CameraRoot& out_cr) const; // Added

        CameraRoot& operator=(const CameraRoot& other);

    };

    class Chara {
    public:
        std::string name;
        ModelTransform model_transform;
        int32_t rob_id;

        Chara();
        Chara(const Chara& other);
        ~Chara();

        void get(float_t in_frame);
        void ctrl(Scene& in_scene);
        void disp(const Scene& in_scene, const mat4& in_base_mat, render_context* rctx);

        void set_disp_freeze(bool in_value) const;
        bool is_bind() const;

        bool load(const SceneFile& in_file, const a3d::Chara& in_c);
        void store(const SceneFile& in_file, a3d::Chara& out_c) const; // Added

        Chara& operator=(const Chara& other);
    };

    class Curve {
    public:
        Fcurve curve;
        std::string name;
        float_t value;

        Curve();
        Curve(const Curve& other);
        ~Curve();

        void get(float_t in_frame);
        Fcurve* M_get_fcurve();

        bool load(const SceneFile& in_file, const a3d::Curve& in_c);
        void store(const SceneFile& in_file, a3d::Curve& out_c) const; // Added

        Curve& operator=(const Curve& other);
    };

    class CurveWithOffset {
    public:
        Curve* fcurve_ptr;
        std::string name;
        float_t offset;
        float_t value;

        CurveWithOffset();
        CurveWithOffset(const CurveWithOffset& other);
        ~CurveWithOffset();

        void init();
        void bind(Curve& in_c);
        void get(float_t in_frame);
        bool is_valid() const;

        CurveWithOffset& operator=(const CurveWithOffset& other);
    };

    class Dof {
    public:
        bool has_dof;
        ModelTransform model_transform;

        Dof();
        Dof(const Dof& other);
        ~Dof();

        void init();
        void get(float_t in_frame);
        void ctrl(render_context* rctx);
        void dest(render_context* rctx);

        bool load(const SceneFile& in_file, const a3d::Dof& in_d);
        void store(const SceneFile& in_file, a3d::Dof& out_d) const; // Added

        Dof& operator=(const Dof& other);
    };

    class Event {
    public:
        bool active;
        EVENT_TYPE type;
        std::string name;
        size_t index;
        float_t begin;
        float_t end;
        std::string param1;
        std::string ref;
        size_t ref_index;

        Event(const a3d::Event& in_e);
        virtual ~Event();

        virtual void resolve_relation(const Scene& in_scene);
        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx);
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx);
        virtual void do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx);
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx);

        void store(const SceneFile& in_file, a3d::Event& out_e) const; // Added
    };

    class EventA2d : public Event {
    public:
        aet::IdHandle M_a2d_handle;

        EventA2d(const a3d::Event& in_e);
        virtual ~EventA2d() override;

        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) override;
    };

    class EventFX : public Event {
    public:
        EventFX(const a3d::Event& in_e);
        virtual ~EventFX() override;

        virtual void resolve_relation(const Scene& in_scene) override;
        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
    };

    class EventFXSmoothCut : public Event {
    public:
        bool is_camera;

        EventFXSmoothCut(const a3d::Event& in_e);
        virtual ~EventFXSmoothCut() override;

        virtual void resolve_relation(const Scene& in_scene);
        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) override;
    };

    class EventFilterFade : public Event {
    public:
        enum Type {
            FADE_NONE = -1,
            FADE_OUT = 0,
            FADE_IN = 1,
        };

        Type fade_type;
        vec3 fade_color;

        EventFilterFade(const a3d::Event& in_e);
        virtual ~EventFilterFade() override;

        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) override;
    };

    class EventFilterTimeStop : public Event {
    public:
        EventFilterTimeStop(const a3d::Event& in_e);
        virtual ~EventFilterTimeStop() override;

        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) override;
    };

    class EventMot : public Event {
    public:
        EventMot(const a3d::Event& in_e);
        virtual ~EventMot() override;

        virtual void resolve_relation(const Scene& in_scene) override;
        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) override;
    };

    class EventSnd : public Event {
    public:
        bool is_pseudo;

        EventSnd(const a3d::Event& in_e);
        virtual ~EventSnd() override;

        virtual void resolve_relation(const Scene& in_scene);
        virtual void entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) override;
        virtual void do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) override;
    };

    class Fog {
    public:
        enum {
            FLAG_DENSITY = 0x01,
            FLAG_START   = 0x02,
            FLAG_END     = 0x04,
            FLAG_COLOR   = 0x08,
        };

        uint32_t flag;
        uint32_t backup_flag;
        int32_t id;
        std::string name;

        Fcurve fcurve_density;
        Fcurve fcurve_start;
        Fcurve fcurve_end;

        float_t density;
        float_t start;
        float_t end;

        Color color;

        float_t backup_density;
        float_t backup_start;
        float_t backup_end;
        vec4 backup_color;

        Fog();
        Fog(const Fog& other);
        ~Fog();

        void init();
        void ctrl(render_context* rctx);
        void dest(render_context* rctx);
        bool has_density() const;
        bool has_start() const;
        bool has_end() const;
        bool has_color() const;
        void get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::Fog& in_f);
        void store(const SceneFile& in_file, a3d::Fog& out_f) const; // Added

        Fog& operator=(const Fog& other);
    };

    class HierarchyObject {
    public:
        std::vector<TransformNode> node_list;
        prj::sys_vector<mat4> mats;
        mat4* ex_node_mat;
        std::string name;
        std::string uid_name;
        object_info obj_uid;
        uint32_t obj_hash;
        bool is_reflect;
        bool is_refract;
        bool has_shadow;
        std::string parent_name;
        std::string parent_node;
        std::vector<Object*> child_object_list;
        std::vector<int32_t> child_object_list_parent_node;
        std::vector<HierarchyObject*> child_object_hrc_list;
        std::vector<int32_t> child_object_hrc_list_parent_node;

        HierarchyObject();
        HierarchyObject(const HierarchyObject& other);
        ~HierarchyObject();

        void init();
        void get(float_t in_frame);
        void ctrl(const mat4& in_base_mat);
        void disp(Scene& in_scene, render_context* rctx);

        void add_child_object(int32_t in_index, Object* in_obj);
        void add_child_object(int32_t in_index, HierarchyObject* in_obj);
        void assign_object_skin();
        void calc_matrix_hierarchy(const mat4& in_base_mat);
        bool replace_chara(int32_t in_cn_src, int32_t in_cn_dst, const object_database* obj_db);
        void check_rob_chara(const std::string& in_name);
        const mat4& get_root_matrix() const;
        void join(const HierarchyObject& in_other, float_t in_blend, float_t in_inv_blend);
        void join_lock(const HierarchyObject& in_other, float_t in_blend, float_t in_inv_blend);
        void lock_set();
        int32_t query_node_index(const char* in_name) const;
        void set_obj_uid(object_info objuid);

        bool load(const SceneFile& in_file,
            const a3d::ObjectHrc& in_oh, const object_database* obj_db);
        void store(const SceneFile& in_file, a3d::ObjectHrc& out_oh) const; // Added

        HierarchyObject& operator=(const HierarchyObject& other);
    };

    class Light {
    public:
        enum {
            FLAG_AMBIENT    = 0x001,
            FLAG_DIFFUSE    = 0x002,
            FLAG_SPECULAR   = 0x004,
            FLAG_TONE_CURVE = 0x008,

            // XHD
            FLAG_CONSTANT   = 0x010,
            FLAG_INTENSITY  = 0x020,
            FLAG_FAR        = 0x040,
            FLAG_LINEAR     = 0x080,
            FLAG_QUADRATIC  = 0x100,
            FLAG_DROP_OFF   = 0x200,
            FLAG_CONE_ANGLE = 0x400,
        };

        uint32_t flag;
        uint32_t backup_flag;
        int32_t id;
        std::string name;
        std::string type;

        ModelTransform position;
        ModelTransform spot_direction;
        Color ambient;
        Color diffuse;
        Color specular;
        Color tone_curve;

        // XHD
        Fcurve fcurve_constant;
        Fcurve fcurve_intensity;
        Fcurve fcurve_far;
        Fcurve fcurve_linear;
        Fcurve fcurve_quadratic;
        Fcurve fcurve_drop_off;
        Fcurve fcurve_cone_angle;

        // XHD
        float_t constant;
        float_t intensity;
        float_t far_; // MS...
        float_t linear;
        float_t quadratic;
        float_t drop_off;
        float_t cone_angle;

        vec4 backup_ambient;
        vec4 backup_diffuse;
        vec4 backup_specular;
        light_tone_curve backup_tone_curve;

        // XHD
        float_t backup_constant;
        float_t backup_intensity;
        float_t backup_far;
        float_t backup_linear;
        float_t backup_quadratic;
        float_t backup_drop_off;
        float_t backup_cone_angle;

        Light();
        Light(const Light& other);
        ~Light();

        void init();
        void ctrl(render_context* rctx);
        void dest(render_context* rctx);
        bool has_ambient() const;
        bool has_diffuse() const;
        bool has_specular() const;
        bool has_tone_curve() const;
        bool has_constant() const; // XHD
        bool has_intensity() const; // XHD
        bool has_far() const; // XHD
        bool has_linear() const; // XHD
        bool has_quadratic() const; // XHD
        bool has_drop_off() const; // XHD
        bool has_cone_angle() const; // XHD
        void get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::Light& in_l);
        void store(const SceneFile& in_file, a3d::Light& out_l) const; // Added

        Light& operator=(const Light& other);
    };

    // X/XHD
    class MaterialList {
    public:
        enum {
            FLAG_EMISSION       = 0x01,
            FLAG_BLEND_COLOR    = 0x02,
            FLAG_GLOW_INTENSITY = 0x04,
        };

        uint32_t flag;
        std::string name;
        uint32_t hash;

        Color emission;
        Color blend_color;
        Fcurve fcurve_glow_intensity;

        float_t glow_intensity;

        MaterialList();
        MaterialList(const MaterialList& other);
        ~MaterialList();

        void init();
        bool has_emission() const;
        bool has_blend_color() const;
        bool has_glow_intensity() const;
        void get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::MaterialList& in_ml);
        void store(const SceneFile& in_file, a3d::MaterialList& out_ml) const; // Added

        MaterialList& operator=(const MaterialList& other);
    };

    class Motion {
    public:
        std::string name;

        Motion();
        Motion(const Motion& other);
        ~Motion();

        bool load(const SceneFile& in_file, const a3d::Motion& in_m);
        void store(const SceneFile& in_file, a3d::Motion& out_m) const; // Added

        Motion& operator=(const Motion& other);
    };

    class MotionTransform {
    public:
        mat4 matrix;
        mat4 matrix_local;

        Fcurve3f fcurve_translation;
        Fcurve3f fcurve_rotation;
        Fcurve3f fcurve_scale;
        Fcurve fcurve_visibility;

        vec3 translation;
        vec3 rotation;
        vec3 scale;
        bool visibility;

        mat4 rot;

        vec3 trans_lock;
        mat4 rot_lock;
        vec3 scale_lock;

        float_t frame;
        bool update_rot;
        bool matrix_local_ready;
        bool has_scale;
        bool has_visibility;

        MotionTransform();
        MotionTransform(const MotionTransform& other);
        ~MotionTransform();

        void init();
        void init_static();
        void get(float_t in_frame);

        void calc_matrix(const mat4& in_mat);
        void calc_matrix_local();

        void check_scale();
        void check_visibility();

        void join(const MotionTransform& in_mt1,
            const MotionTransform& in_mt2, float_t in_inv_blend, float_t in_blend);
        void join(
            const MotionTransform& in_other, float_t in_inv_blend, float_t in_blend);
        void join_lock(const MotionTransform& in_mt1,
            const MotionTransform& in_mt2, float_t in_inv_blend, float_t in_blend);
        void join_lock(
            const MotionTransform& in_other, float_t in_inv_blend, float_t in_blend);

        void lock_set();

        void add(const MotionTransform& in_mt1, const MotionTransform& in_mt2);
        void sub(const MotionTransform& in_mt1, const MotionTransform& in_mt2);

        bool load(const SceneFile& in_file, const a3d::ModelTransform& in_mt);
        void store(const SceneFile& in_file, a3d::ModelTransform& out_mt) const; // Added

        MotionTransform& operator=(const MotionTransform& other);
    };

    class MultiHierarchyObject {
    public:
        std::string name;
        MotionTransform motion_transform;
        std::vector<TransformNode> node_list;
        std::vector<ObjectInstance> instance_list;

        MultiHierarchyObject();
        MultiHierarchyObject(const MultiHierarchyObject& other);
        ~MultiHierarchyObject();

        void init();
        void get(float_t in_frame);
        void ctrl(const mat4& in_base_mat);
        void disp(Scene& in_scene, render_context* rctx);

        void calc_matrix_hierarchy();
        void calc_matrix_local();
        const mat4& get_root_matrix() const;

        bool load(const SceneFile& in_file, const a3d::MObjectHrc& in_moh,
            const object_database* obj_db);
        void store(const SceneFile& in_file, a3d::MObjectHrc& out_moh) const; // Added

        MultiHierarchyObject& operator=(const MultiHierarchyObject& other);
    };

    class Object {
    public:
        ModelTransform model_transform;
        size_t index;
        std::string name;
        std::string uid_name;
        std::string parent_name;
        std::string parent_node;
        CurveWithOffset obj_pat;
        CurveWithOffset obj_morph;
        std::vector<Texture> texture_list;
        std::vector<TextureTransform> texture_transform_list;
        std::vector<Object*> child_object_list;
        std::vector<HierarchyObject*> child_object_hrc_list;
        object_info obj_uid;
        uint32_t obj_hash;
        bool is_reflect;
        bool is_refract;

        Object();
        Object(const Object& other);
        ~Object();

        void init();
        void get(float_t in_frame);
        void ctrl(const mat4& in_base_mat);
        void disp(Scene& in_scene, render_context* rctx);

        bool replace_chara(int32_t in_cn_src, int32_t in_cn_dst, const object_database* obj_db);
        void prepare_morph(const object_database* obj_db); // Added

        bool load(const SceneFile& in_file, const a3d::Object& in_o,
            const object_database* obj_db, const texture_database* tex_db);
        void store(const SceneFile& in_file, a3d::Object& out_o) const; // Added

        Object& operator=(const Object& other);
    };

    class ObjectInstance {
    public:
        std::string name;
        std::string uid_name;
        object_info obj_uid;
        uint32_t obj_hash;
        bool shadow;
        MotionTransform motion_transform;
        std::vector<int32_t> matrix_id;
        prj::sys_vector<mat4> mats;

        ObjectInstance();
        ObjectInstance(const ObjectInstance& other);
        ~ObjectInstance();

        void init();
        void get(float_t in_frame);

        bool load(const SceneFile& in_file, const a3d::ObjectInstance& in_oi,
            const MultiHierarchyObject& in_moh, const object_database* obj_db);
        void store(const SceneFile& in_file, a3d::ObjectInstance& out_oi,
            const MultiHierarchyObject& in_moh) const; // Added

        ObjectInstance& operator=(const ObjectInstance& other);
    };

    class PlayControl {
    public:
        enum {
            FLAG_DIV = 0x01, // F2/X/XHD
            FLAG_OFFSET = 0x02,
        };

        uint32_t flag;
        float_t begin;
        int32_t div; // F2/X/XHD
        float_t fps;
        float_t offset; // F2/X/XHD
        float_t size;

        PlayControl();
        PlayControl(const PlayControl& other);
        ~PlayControl();

        void init();

        bool load(const SceneFile& in_file, const a3d::PlayControl& in_pc);
        void store(const SceneFile& in_file, a3d::PlayControl& out_pc) const; // Added

        PlayControl& operator=(const PlayControl& other);
    };

    class Point {
    public:
        std::string name;
        ModelTransform model_transform;

        Point();
        Point(const Point& other);
        ~Point();

        void get(float_t frame);
        void disp(const Scene& in_scene, render_context* rctx);

        bool load(const SceneFile& in_file, const a3d::Point& in_p);
        void store(const SceneFile& in_file, a3d::Point& out_p) const; // Added

        Point& operator=(const Point& other);
    };

    class PostProcess {
    public:
        enum {
            FLAG_LENS_FLARE = 0x01,
            FLAG_LENS_SHAFT = 0x02,
            FLAG_LENS_GHOST = 0x04,
            FLAG_RADIUS     = 0x08,
            FLAG_INTENSITY  = 0x10,
            FLAG_SCENE_FADE = 0x20,
        };

        uint32_t flag;
        uint32_t backup_flag;
        Fcurve fcurve_lens_flare;
        Fcurve fcurve_lens_shaft;
        Fcurve fcurve_lens_ghost;
        Color radius;
        Color intensity;
        Color scene_fade;

        float_t backup_lens_flare;
        float_t backup_lens_shaft;
        float_t backup_lens_ghost;
        vec4 backup_radius;
        vec4 backup_intensity;
        vec4 backup_scene_fade;

        float_t lens_flare;
        float_t lens_shaft;
        float_t lens_ghost;

        PostProcess();
        PostProcess(const PostProcess& other);
        ~PostProcess();

        void init();
        bool has_lens() const;
        bool has_lens_flare() const;
        bool has_lens_shaft() const;
        bool has_lens_ghost() const;
        bool has_radius() const;
        bool has_intensity() const;
        bool has_scene_fade() const;
        void get(float_t in_frame);
        void ctrl(Scene& in_scene, render_context* rctx);
        void dest(render_context* rctx);

        bool load(const SceneFile& in_file, const a3d::PostProcess& in_pp);
        void store(const SceneFile& in_file, a3d::PostProcess& out_pp) const; // Added

        PostProcess& operator=(const PostProcess& other);
    };

    class Texture {
    public:
        std::string name;
        uint32_t tex_uid;
        CurveWithOffset tex_pat;

        Texture();
        Texture(const Texture& other);
        ~Texture();

        void init();
        void get(float_t in_frame);

        bool load(const SceneFile& in_file,
            const a3d::Texture& in_t, const texture_database* tex_db);
        void store(const SceneFile& in_file, a3d::Texture& out_t) const; // Added

        Texture& operator=(const Texture& other);
    };

    class TextureTransform {
    public:
        enum {
            FLAG_COVERAGE_U        = 0x001,
            FLAG_COVERAGE_V        = 0x002,
            FLAG_REPEAT_U          = 0x004,
            FLAG_REPEAT_V          = 0x008,
            FLAG_TRANSLATE_FRAME_U = 0x010,
            FLAG_TRANSLATE_FRAME_V = 0x020,
            FLAG_OFFSET_U          = 0x040,
            FLAG_OFFSET_V          = 0x080,
            FLAG_ROTATE_FRAME      = 0x100,
            FLAG_ROTATE            = 0x200,
        };

        uint32_t flag;
        uint32_t backup_flag;
        std::string name;
        uint32_t tex_uid;

        Fcurve fcurve_coverage_u;
        Fcurve fcurve_coverage_v;
        Fcurve fcurve_repeat_u;
        Fcurve fcurve_repeat_v;
        Fcurve fcurve_translate_frame_u;
        Fcurve fcurve_translate_frame_v;
        Fcurve fcurve_offset_u;
        Fcurve fcurve_offset_v;
        Fcurve fcurve_rotate_frame;
        Fcurve fcurve_rotate;

        mat4 mat;
        vec3 scale;
        vec3 repeat;
        vec3 rotate;
        vec3 translate_frame;

        TextureTransform();
        TextureTransform(const TextureTransform& other);
        ~TextureTransform();

        void init();
        bool has_coverage_u() const;
        bool has_coverage_v() const;
        bool has_repeat_u() const;
        bool has_repeat_v() const;
        bool has_translate_frame_u() const;
        bool has_translate_frame_v() const;
        bool has_offset_u() const;
        bool has_offset_v() const;
        bool has_rotate_frame() const;
        bool has_rotate() const;
        void get(float_t frame);
        void mul();

        bool load(const SceneFile& in_file,
            const a3d::TextureTransform& in_tt, const texture_database* tex_db);
        void store(const SceneFile& in_file, a3d::TextureTransform& out_tt) const; // Added

        TextureTransform& operator=(const TextureTransform& other);
    };

    struct TimeEvent {
        float_t frame;
        int32_t type;
        size_t event_index;
    };

    class TransformNode {
    public:
        enum {
            FLAG_JOINT_ORIENT = 0x01,
        };

        uint32_t flag;
        std::string name;
        int32_t node_index;
        int32_t parent_id;
        MotionTransform motion_transform;
        mat4* mat_ptr;
        vec3 joint_orient;
        mat4 joint_orient_mat;

        TransformNode();
        TransformNode(const TransformNode& other);
        ~TransformNode();

        void init();
        void get(float_t in_frame);

        void calc_matrix_local(const mat4& in_base_mat);

        bool load(const SceneFile& in_file, const a3d::ObjectNode& in_on);
        void store(const SceneFile& in_file, a3d::ObjectNode& out_on) const; // Added

        TransformNode& operator=(const TransformNode& other);
    };
}

namespace auth_3d_detail {
    class DB {
    public:

        DB();
        ~DB();
    };

    class CategoryData {
    public:
        int32_t ref_count;
        std::string category_name;
        LoadState load_state;
        std::string path;
        std::string file;
        p_file_handler file_handler;
        farc* ar;
        const void* raw_data;
        size_t raw_size;

    private:
        void M_load_req(const char* in_mdata_dir);
        void M_load_req_modern(void* data); // Added
        void M_free();

        static bool S_load_cb(CategoryData* data, const void* file_data, size_t file_size);

    public:
        CategoryData();
        CategoryData(const CategoryData& other);
        ~CategoryData();

        void load_req(const char* in_mdata_dir);
        void load_req_modern(void* data); // Added
        void free();
        std::tuple<size_t, const void*> get_file(const std::string& in_name);
        std::tuple<size_t, const void*, const char*> get_file(uint32_t in_hash); // Added
        bool load_is_done() const;

        CategoryData& operator=(const CategoryData& other);
    };

    class SceneFile {
    public:
        float_t frame_max;
        const void* raw_data;
        size_t raw_size;
        bool has_raw;
        a3d::Scene prop;
        bool prop_is_ready;

        SceneFile();
        SceneFile(const SceneFile& other);
        ~SceneFile();

        void load(const void* in_data, size_t in_size);
        void clear();

        SceneFile& operator=(const SceneFile& other);
    };

    class UidData {
    public:
        int32_t ref_count;
        int32_t uid;
        std::string file_name;
        LoadState load_state;
        SceneFile file;
        const char* name;
        CategoryData* category;

    private:
        void M_load_req(const auth_3d_database* auth_3d_db);
        void M_load_req_category();
        void M_free();
        void M_load_is_done();

    public:
        UidData();
        UidData(const UidData& other);
        ~UidData();

        void load_req(const auth_3d_database* auth_3d_db);
        void free();
        bool load_is_done();

        UidData& operator=(const UidData& other);
    };

    // Added
    class UidDataModern {
    public:
        int32_t ref_count;
        uint32_t hash;
        std::string file_name;
        LoadState load_state;
        SceneFile file;
        std::string name;
        CategoryData* category;
        void* data;
        const object_database* obj_db;
        const texture_database* tex_db;

    private:
        void M_load_req();
        void M_load_req_category();
        void M_free();
        void M_load_is_done();

    public:
        UidDataModern();
        UidDataModern(const UidDataModern& other);
        ~UidDataModern();

        void load_req();
        void free();
        bool load_is_done();

        UidDataModern& operator=(const UidDataModern& other);
    };

    struct ItemTexChange {
        uint32_t org_texid;
        uint32_t change_texid;

        inline ItemTexChange() : org_texid(-1), change_texid(-1) {

        }

        inline ItemTexChange(uint32_t in_org_texid, uint32_t in_change_texid)
            : org_texid(in_org_texid), change_texid(in_change_texid) {

        }
    };

    class Scene {
    public:
        int32_t M_uid;
        int32_t my_handle;
        bool M_is_enabled;
        bool M_camera_is_enabled;
        bool M_is_visible;
        bool M_is_looped;
        bool M_is_start;
        bool M_is_finished;
        bool M_is_reverse_side;
        bool M_is_auto_disable;
        mat4 base_matrix;
        float_t trnsl;
        mdl::ObjFlags trnsl_state;
        bool M_is_attribute_wall;
        std::map<uint32_t, uint32_t> item_attr_map;
        std::vector<ItemTexChange> item_texchg_list;
        int32_t M_assign_rob_id;
        object_info M_obj_uid;
        mat4* M_ex_node_mat;
        bool M_shadow;
        bool M_chara_item; // Added
        bool M_reflect; // Added
        int32_t M_cn_src;
        int32_t M_cn_dst;
        uint32_t M_debug;
        uint64_t M_data;
        FrameRateControl* M_frc;
        float_t M_frame;
        float_t frame_req;
        float_t frame_max;
        bool is_requested_frame;
        float_t M_frame_loop_begin;
        float_t M_frame_loop_end;
        float_t M_fov_adjust;
        const std::vector<HierarchyObject*>* object_hrc_blend_list;
        float_t object_hrc_blend;
        bool now_pause;

        std::vector<Event*> event_list;
        std::vector<TimeEvent> event_time_list;
        std::vector<TimeEvent>::iterator event_time_unread_ptr;
        PlayControl play_control;
        std::vector<Point> point_list;
        std::vector<Curve> curve_list;
        std::vector<CameraRoot> camera_root_list;
        CameraAuxiliary camera_auxiliary;
        std::vector<Light> light_list;
        std::vector<Ambient> ambient_list; // MGF
        std::vector<Fog> fog_list;
        PostProcess post_process;
        Dof dof;
        std::vector<Chara> chara_list;
        std::vector<Motion> motion_list;
        std::vector<Object> object_holder;
        std::vector<Object*> object_list;
        std::vector<HierarchyObject> object_hrc_holder;
        std::vector<HierarchyObject*> object_hrc_list;
        std::vector<MultiHierarchyObject> m_object_hrc_holder;
        std::vector<MultiHierarchyObject*> m_object_hrc_list;
        std::vector<std::string> adx_sound_list;
        std::vector<MaterialList> material_list; // X
        std::vector<std::string> object_name_list;
        std::vector<std::string> object_hrc_name_list;
        std::vector<std::string> m_object_hrc_name_list;
        std::vector<Auth2d> auth_2d_list;
        LoadState load_state;
        std::vector<a3d::EventListener*> my_event_listener;

        std::string file_name;
        uint32_t hash;
        const object_database* obj_db;
        const texture_database* tex_db;

    private:
        void M_load_exec();

    public:
        Handle to_handle();

        Scene();
        ~Scene();

        void init();
        void destroy(render_context* rctx);
        int32_t get_uid() const;
        void ctrl(render_context* rctx);
        void disp(render_context* rctx);
        void load_req(const auth_3d_database* auth_3d_db);
        void load_req_modern(); // Added
        void M_put_obj(const mat4& in_mat, object_info in_id, render_context* rctx) const;
        bool load_is_done() const;
        void add_event_listener(a3d::EventListener* in_evt_list);
        void remove_event_listener(a3d::EventListener* in_evt_list);
        void M_loadFromProperties(SceneFile& in_file,
            const object_database* in_obj_db, const texture_database* in_tex_db);
        void M_storeToProperties(SceneFile& in_file) const; // Added
        bool is_valid() const;
        void set_frame_rate_control(FrameRateControl* in_value);
        float_t get_frame_speed() const;
        void set_frame_req(float_t in_frame);
        void set_frame_max(float_t in_frame);
        float_t get_frame() const;
        float_t get_frame_size() const;
        float_t get_frame_fps() const;
        float_t get_frame_begin() const;
        void set_pause(bool in_value);
        bool get_pause() const;
        void set_enabled(bool in_value);
        bool get_enabled() const;
        void set_visible(bool in_value);
        bool get_visible() const;
        void set_looped(bool in_value);
        bool get_looped() const;
        void set_frame_loop_begin(float_t in_frame);
        void set_frame_loop_begin();
        float_t get_frame_loop_begin() const;
        void set_frame_loop_end(float_t in_frame);
        void set_frame_loop_end();
        float_t get_frame_loop_end() const;
        bool is_finished() const;
        void set_reverse_side(bool in_value);
        bool get_reverse_side() const;
        void set_auto_disable(bool in_value);
        bool get_auto_disable() const;
        void set_base_matrix(const mat4& in_value);
        const mat4& get_base_matrix() const;
        void set_trnsl(float_t in_value, mdl::ObjFlags in_state);
        float_t get_trnsl() const;
        void set_attribute_wall(bool in_value);
        bool get_attribute_wall() const;
        void set_item_attr(uint32_t in_id, uint32_t in_attr);
        uint32_t get_item_attr(uint32_t in_id) const;
        void set_item_texchange(const std::vector<ItemTexChange>& in_list);
        const std::vector<ItemTexChange>& get_item_texchange() const;
        size_t get_chara_size() const;
        size_t get_chara_index(const std::string& in_name) const;
        const std::string* chara_get_name(size_t in_index) const;
        void chara_bind_rob(size_t in_index, int32_t in_rob_id);
        size_t get_object_size() const;
        size_t get_object_index(const std::string& in_name) const;
        int32_t get_object_index(object_info in_obj_uid, int32_t in_instance = -1) const;
        int32_t get_object_index(uint32_t in_obj_hash, int32_t in_instance = -1) const;
        const std::string* object_get_name(size_t in_index) const;
        const mat4* get_object_matrix(size_t in_index) const;
        const mat4* get_object_matrix(size_t in_index, bool in_is_hrc) const;
        size_t get_object_hrc_size() const;
        size_t get_object_hrc_index(const std::string& in_name) const;
        int32_t get_object_hrc_index(object_info in_obj_uid, int32_t instance = -1) const;
        int32_t get_object_hrc_index(uint32_t in_obj_hash, int32_t instance = -1) const; // Added
        const std::string* object_hrc_get_name(size_t in_index) const;
        const mat4* get_object_hrc_matrix(size_t in_index) const;
        size_t get_m_object_hrc_size() const;
        size_t get_m_object_hrc_index(const std::string& in_name) const;
        const std::string* m_object_hrc_get_name(size_t in_index) const;
        void camera_set_enabled(bool in_value);
        bool camera_get_enabled() const;
        void camera_set_fov_adjust(float_t in_value);
        float_t camera_get_fov_adjust() const;
        size_t get_auth_2d_size() const;
        size_t get_auth_2d_index(const std::string& in_name) const;
        const std::string* auth_2d_get_name(size_t in_index) const;
        size_t get_point_index(const std::string& in_name) const;
        size_t get_curve_index(const std::string& in_name) const;
        void set_data(size_t);
        size_t get_data() const;
        void set_debug(uint32_t in_value);
        uint32_t get_debug() const;
        void assign_rob_id(int32_t in_value);
        int32_t get_assign_rob_id() const;
        void set_obj_uid(object_info in_value);
        void set_shadow(bool in_value);
        bool is_shadow() const;
        void replace_chara(int32_t in_cn_src, int32_t in_cn_dst);
        void set_frame_object_hrc(float_t in_frame);
        void set_object_hrc_blend(const Scene* in_scene, float_t in_value);
        void calc_matrix_hierarchy(const mat4& in_value);
        void calc_matrix_hierarchy_get();
        void calc_matrix_hierarchy_mul();
        void join();
        void join_lock();
        void set_ex_node_mat(mat4* in_value);
        bool is_ex_node_mat() const;
        void lock_list_set();
        void set_chara_item(bool in_value); // Added
        bool get_chara_item() const; // Added
        void set_reflect(bool in_value); // Added
        bool get_reflect() const; // Added
    };

    struct Auth3dManager {
        DB db;

        std::vector<CategoryData> category_data_list;
        std::unordered_map<uint32_t, CategoryData> category_data_list_modern;
        std::vector<UidData> uid_data_list;
        std::unordered_map<uint32_t, UidDataModern> uid_data_list_modern;
        std::vector<int32_t> handle_list;
        Scene scene_buffer[AUTH_3D_DATA_COUNT];

        Auth3dManager();
        ~Auth3dManager();

        UidData* get_uid_data(int32_t in_uid, const auth_3d_database* auth_3d_db);
        UidDataModern* get_uid_data_modern(uint32_t hash); // Added
        CategoryData* get_category_data(const char* in_name);
        CategoryData* get_category_data_modern(uint32_t in_hash); // Added
        void category_free(const char* in_name);
        void category_free(uint32_t in_hash); // Added
        bool category_load_is_done(const char* in_name);
        bool category_load_is_done(uint32_t in_hash); // Added
        void category_load_req(const char* in_name, const char* in_mdata_dir);
        void category_load_req(void* data, const char* in_name, uint32_t in_hash); // Added
        const mat4* get_object_matrix(int32_t in_handle, size_t in_index, bool in_is_hrc) const;
        const char* get_name_uid(int32_t in_uid, const auth_3d_database* auth_3d_db) const;
        Scene* get_scene(int32_t in_uid); // Added
        Scene* get_scene(uint32_t hash); // Added
        int32_t get_scene_object_index(object_info in_obj_uid,
            int32_t* out_index, bool* out_is_hrc, int32_t in_instance = -1);
        int32_t get_scene_object_index(uint32_t in_file_name_hash, uint32_t in_obj_hash,
            int32_t* out_index, bool* out_is_hrc, int32_t in_instance = -1); // Added
        bool uid_load_req(int32_t in_uid, const auth_3d_database* auth_3d_db);
        bool uid_load_req_modern(uint32_t hash); // Added
        void uid_free(int32_t in_uid, const auth_3d_database* auth_3d_db);
        void uid_free_modern(uint32_t hash); // Added
        bool init();
        bool ctrl();
        void disp();
        const Scene* M_get_work(int32_t in_handle) const;
        Scene* M_get_work(int32_t in_handle);
        const Scene* get_work(int32_t in_handle) const;
        Scene* get_work(int32_t in_handle);
        const Scene* get_work_enabled(int32_t in_handle) const;
        Scene* get_work_enabled(int32_t in_handle);
        //void dump_scene_buffer();
        int32_t create(int32_t uid, const auth_3d_database* auth_3d_db);
        int32_t create(uint32_t hash, void* data,
            const object_database* obj_db, const texture_database* tex_db); // Added
        void destroy(int32_t in_handle, render_context* rctx);
        int32_t get_assign_rob_id(int32_t in_handle) const;
    };

    class Handle {
    private:
        int32_t m_handle;

        Handle(int32_t in_handle) : m_handle(in_handle) {

        }

    public:
        inline Handle() : m_handle(-1) {

        }

        inline ~Handle() {

        }

        constexpr bool operator==(const Handle& other) const;
        constexpr bool operator<(const Handle& other) const;

        static Handle S_init(int32_t in_handle);

        int32_t get();
        static Handle create(int32_t uid, const auth_3d_database* auth_3d_db);
        static Handle create(uint32_t hash, void* data,
            const object_database* obj_db, const texture_database* tex_db); // Added
        void destroy(render_context* rctx);
        bool is_valid() const;
        void set_invalid();
        int32_t get_uid() const;
        void add_event_listener(a3d::EventListener* in_value);
        void remove_event_listener(a3d::EventListener* in_value);
        void load_req(const auth_3d_database* auth_3d_db);
        void load_req_modern(); // Added
        bool load_is_done() const;
        void set_frame_req(float_t in_frame);
        void set_frame_max(float_t in_frame);
        float_t get_frame() const;
        float_t get_frame_size() const;
        float_t get_frame_fps() const;
        float_t get_frame_begin() const;
        void set_frame_rate_control(FrameRateControl* in_value);
        void set_pause(bool in_value);
        bool get_pause() const;
        void set_enabled(bool in_value);
        bool get_enabled() const;
        void set_looped(bool in_value);
        bool get_looped() const;
        void set_frame_loop_begin();
        void set_frame_loop_begin(float_t in_frame);
        float_t get_frame_loop_begin() const;
        void set_frame_loop_end();
        void set_frame_loop_end(float_t in_frame);
        float_t get_frame_loop_end() const;
        bool is_finished() const;
        void set_auto_disable(bool in_value);
        bool get_auto_disable() const;
        void set_base_matrix(const mat4& in_value);
        const mat4& get_base_matrix() const;
        void set_reverse_side(bool in_value);
        bool get_reverse_side() const;
        size_t get_chara_size() const;
        size_t get_chara_index(const std::string& in_name) const;
        const std::string* chara_get_name(size_t in_index) const;
        void chara_bind_rob(size_t in_index, int32_t in_rob_id);
        size_t get_object_size() const;
        size_t get_object_index(const std::string& in_name) const;
        const std::string* object_get_name(size_t in_index) const;
        const mat4* get_object_matrix(size_t in_index) const;
        void set_visible(bool in_value);
        bool get_visible() const;
        void set_trnsl(float_t in_value, mdl::ObjFlags in_state);
        float_t get_trnsl() const;
        void set_attribute_wall(bool in_value);
        bool get_attribute_wall() const;
        void set_item_attr(uint32_t in_id, uint32_t in_attr);
        void set_item_texchange(const std::vector<ItemTexChange>& in_list);
        void camera_set_enabled(bool in_value);
        bool camera_get_enabled() const;
        void camera_set_fov_adjust(float_t in_value);
        float_t camera_get_fov_adjust() const;
        size_t get_auth_2d_size() const;
        size_t get_auth_2d_index(const std::string& in_name) const;
        const std::string* auth_2d_get_name(size_t in_index) const;
        void set_data(size_t in_value);
        size_t get_data() const;
        void set_debug(uint32_t in_value);
        uint32_t get_debug() const;
        void assign_rob_id(int32_t in_value);
        int32_t get_assign_rob_id() const;
        void set_obj_uid(object_info in_value);
        void set_shadow(bool in_value);
        bool is_shadow() const;
        void replace_chara(int32_t in_cn_src, int32_t in_cn_dst);
        void set_frame_object_hrc(float_t in_frame);
        void calc_matrix_hierarchy(const mat4& in_value);
        void join(int32_t in_handle, float_t in_blend);
        void join_lock(int32_t in_handle, float_t in_blend);
        void set_ex_node_mat(mat4* in_value);
        bool is_ex_node_mat() const;
        void lock_set();
        void set_chara_item(bool in_value); // Added
        bool get_chara_item() const; // Added
        void set_reflect(bool in_value); // Added
        bool get_reflect() const; // Added

        Scene* get_scene();
    };

    inline constexpr bool Handle::operator==(const Handle& other) const {
        return m_handle == other.m_handle;
    }

    inline constexpr bool Handle::operator<(const Handle& other) const {
        return m_handle < other.m_handle;
    }

    constexpr bool operator!=(const Handle& left, const Handle& right) {
        return !(left == right);
    }

    constexpr bool operator>(const Handle& left, const Handle& right) {
        return right < left;
    }

    constexpr bool operator<=(const Handle& left, const Handle& right) {
        return !(right < left);
    }

    constexpr bool operator>=(const Handle& left, const Handle& right) {
        return !(left < right);
    }

    extern Auth3dManager* g_manager;

    extern void category_free(const char* in_name);
    extern void category_free(uint32_t in_hash); // Added
    extern bool category_load_is_done(const char* in_name);
    extern bool category_load_is_done(uint32_t in_hash); // Added
    extern void category_load_req(const char* in_name);
    extern void category_load_req(const char* in_name, const char* in_mdata_dir);
    extern void category_load_req(void* data, const char* in_name, uint32_t in_hash); // Added
    extern Event* createEvent(const SceneFile& in_file, const a3d::Event& in_e);
    extern Event* createEventFX(const SceneFile& in_file, const a3d::Event& in_e);
    extern Event* createEventFilter(const SceneFile& in_file, const a3d::Event& in_e);
    extern const char* get_name_uid(int32_t in_uid, const auth_3d_database* auth_3d_db);
    extern const mat4* get_object_matrix(int32_t in_handle, size_t in_index, bool in_is_hrc);
    extern int32_t get_assign_rob_id(int32_t in_handle);
    extern Scene* get_scene(int32_t in_uid); // Added
    extern Scene* get_scene(uint32_t hash); // Added
    extern int32_t get_scene_object_index(object_info in_obj_uid,
        int32_t* out_index, bool* out_is_hrc, int32_t in_instance = -1);
    extern int32_t get_scene_object_index(uint32_t in_file_name_hash, uint32_t in_obj_hash,
        int32_t* out_index, bool* out_is_hrc, int32_t in_instance = -1); // Added

    // MGF
    inline bool Ambient::has_light_diffuse() const {
        return flag & FLAG_LIGHT_DIFFUSE;
    }

    // MGF
    inline bool Ambient::has_rim_light_diffuse() const {
        return flag & FLAG_RIM_LIGHT_DIFFUSE;
    }

    inline bool CameraAuxiliary::has_exposure() const {
        return flag & FLAG_EXPOSURE;
    }

    // F/F2/X/XHD
    inline bool CameraAuxiliary::has_exposure_rate() const {
        return flag & FLAG_EXPOSURE_RATE;
    }

    inline bool CameraAuxiliary::has_gamma() const {
        return flag & FLAG_GAMMA;
    }

    // F/F2/X/XHD
    inline bool CameraAuxiliary::has_gamma_rate() const {
        return flag & FLAG_GAMMA_RATE;
    }

    inline bool CameraAuxiliary::has_saturate() const {
        return flag & FLAG_SATURATE;
    }

    inline bool CameraAuxiliary::has_auto_exposure() const {
        return flag & FLAG_AUTO_EXPOSURE;
    }

    inline bool Fog::has_density() const {
        return flag & FLAG_DENSITY;
    }

    inline bool Fog::has_start() const {
        return flag & FLAG_START;
    }

    inline bool Fog::has_end() const {
        return flag & FLAG_END;
    }

    inline bool Fog::has_color() const {
        return flag & FLAG_COLOR;
    }

    inline bool Light::has_ambient() const {
        return flag & FLAG_AMBIENT;
    }

    inline bool Light::has_diffuse() const {
        return flag & FLAG_DIFFUSE;
    }

    inline bool Light::has_specular() const {
        return flag & FLAG_SPECULAR;
    }

    inline bool Light::has_tone_curve() const {
        return flag & FLAG_TONE_CURVE;
    }

    // XHD
    inline bool Light::has_constant() const {
        return flag & FLAG_CONSTANT;
    }

    // XHD
    inline bool Light::has_intensity() const {
        return flag & FLAG_INTENSITY;
    }

    // XHD
    inline bool Light::has_far() const {
        return flag & FLAG_FAR;
    }

    // XHD
    inline bool Light::has_linear() const {
        return flag & FLAG_LINEAR;
    }

    // XHD
    inline bool Light::has_quadratic() const {
        return flag & FLAG_QUADRATIC;
    }

    // XHD
    inline bool Light::has_drop_off() const {
        return flag & FLAG_DROP_OFF;
    }

    // XHD
    inline bool Light::has_cone_angle() const {
        return flag & FLAG_CONE_ANGLE;
    }

    // XHD
    inline bool MaterialList::has_emission() const {
        return flag & FLAG_EMISSION;
    }

    // XHD
    inline bool MaterialList::has_blend_color() const {
        return flag & FLAG_BLEND_COLOR;
    }

    // XHD
    inline bool MaterialList::has_glow_intensity() const {
        return flag & FLAG_GLOW_INTENSITY;
    }

    inline bool PostProcess::has_lens() const {
        return flag & (FLAG_LENS_GHOST | FLAG_LENS_SHAFT | FLAG_LENS_FLARE);
    }

    inline bool PostProcess::has_lens_flare() const {
        return flag & FLAG_LENS_FLARE;
    }

    inline bool PostProcess::has_lens_shaft() const {
        return flag & FLAG_LENS_SHAFT;
    }

    inline bool PostProcess::has_lens_ghost() const {
        return flag & FLAG_LENS_GHOST;
    }

    inline bool PostProcess::has_radius() const {
        return flag & FLAG_RADIUS;
    }

    inline bool PostProcess::has_intensity() const {
        return flag & FLAG_INTENSITY;
    }

    inline bool PostProcess::has_scene_fade() const {
        return flag & FLAG_SCENE_FADE;
    }

    inline bool TextureTransform::has_coverage_u() const {
        return flag & FLAG_COVERAGE_U;
    }

    inline bool TextureTransform::has_coverage_v() const {
        return flag & FLAG_COVERAGE_V;
    }

    inline bool TextureTransform::has_repeat_u() const {
        return flag & FLAG_REPEAT_U;
    }

    inline bool TextureTransform::has_repeat_v() const {
        return flag & FLAG_REPEAT_V;
    }

    inline bool TextureTransform::has_translate_frame_u() const {
        return flag & FLAG_TRANSLATE_FRAME_U;
    }

    inline bool TextureTransform::has_translate_frame_v() const {
        return flag & FLAG_TRANSLATE_FRAME_V;
    }

    inline bool TextureTransform::has_offset_u() const {
        return flag & FLAG_OFFSET_U;
    }

    inline bool TextureTransform::has_offset_v() const {
        return flag & FLAG_OFFSET_V;
    }

    inline bool TextureTransform::has_rotate_frame() const {
        return flag & FLAG_ROTATE_FRAME;
    }

    inline bool TextureTransform::has_rotate() const {
        return flag & FLAG_ROTATE;
    }
}

extern void auth_3d_data_init();
void auth_3d_data_get_obj_sets_from_category(std::string& name, std::vector<uint32_t>& obj_sets,
    const auth_3d_database* auth_3d_db, const object_database* obj_db);
extern void auth_3d_data_load_auth_3d_db(const auth_3d_database* auth_3d_db);
extern void auth_3d_data_free();

extern void task_auth_3d_init();
extern bool task_auth_3d_open();
extern bool task_auth_3d_check_alive();
extern bool task_auth_3d_close();
extern void task_auth_3d_free();
