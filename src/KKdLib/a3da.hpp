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

namespace a3d {
    enum COMPRESS_TYPE {
        COMPRESS_F32F32F32F32 = 0,
        COMPRESS_I16F16F32F32,
        COMPRESS_I16F16F16F16,
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

    enum FORMAT {
        FORMAT_DT  = 0,
        FORMAT_F,
        FORMAT_AFT,
        FORMAT_F2,
        FORMAT_MGF,
        FORMAT_X,
        FORMAT_XHD,
        FORMAT_AFT_X_PACK,
    };

    enum FC_TYPE {
        FC_TYPE_STATIC_0 = 0,
        FC_TYPE_STATIC_DATA,
        FC_TYPE_LINEAR,
        FC_TYPE_HERMITE,
        FC_TYPE_NO_INTERPOLATION,
    };

    struct Ambient;
    struct CameraAuxiliary;
    struct CameraInterest;
    struct CameraRoot;
    struct CameraViewPoint;
    struct Chara;
    struct Curve;
    struct Dof;
    struct Event;
    struct Fcurve;
    struct Fcurve3f;
    struct FcurveColor4f;
    struct Fog;
    struct Light;
    struct MObjectHrc;
    struct MaterialList;
    struct ModelTransform;
    struct Object;
    struct ObjectHrc;
    struct ObjectInstance;
    struct ObjectNode;
    struct Texture;
    struct TextureTransform;
    struct PlayControl;
    struct Point;
    struct PostProcess;

    struct Fcurve {
        enum {
            BIN_OFFSET = 0x01,
        };

        uint32_t flag;
        int32_t bin_offset;
        FC_TYPE type;
        EP_TYPE ep_type_pre;
        EP_TYPE ep_type_post;
        std::vector<kft3> keys;
        float_t max_frame;
        bool raw_data;
        bool raw_data_binary;
        int32_t raw_data_value_list_size;
        int32_t raw_data_value_list_offset;
        float_t value;

        Fcurve();
        ~Fcurve();

        Fcurve& operator=(const Fcurve& other);
    };

    struct Fcurve3f {
        Fcurve x;
        Fcurve y;
        Fcurve z;

        Fcurve3f();
        ~Fcurve3f();
    };

    struct FcurveColor4f {
        Fcurve r;
        Fcurve g;
        Fcurve b;
        Fcurve a;
        bool has_r;
        bool has_g;
        bool has_b;
        bool has_a;

        FcurveColor4f();
        ~FcurveColor4f();
    };

    struct ModelTransform {
        enum {
            BIN_OFFSET = 0x01,
        };

        uint32_t flag;
        int32_t bin_offset;
        Fcurve3f rotation;
        Fcurve3f scale;
        Fcurve3f translation;
        Fcurve visibility;

        ModelTransform();
        ~ModelTransform();
    };

    // MGF
    struct Ambient {
        enum {
            FLAG_LIGHT_DIFFUSE     = 0x01,
            FLAG_RIM_LIGHT_DIFFUSE = 0x02,
        };

        uint32_t flag;
        FcurveColor4f light_diffuse;
        std::string name;
        FcurveColor4f rim_light_diffuse;

        Ambient();
        ~Ambient();
    };

    struct Auth2d {
        std::string name;

        Auth2d();
        ~Auth2d();
    };

    struct CameraAuxiliary {
        enum {
            FLAG_EXPOSURE      = 0x01,
            FLAG_EXPOSURE_RATE = 0x02, // F/F2/X
            FLAG_GAMMA         = 0x04,
            FLAG_GAMMA_RATE    = 0x08, // F/F2/X
            FLAG_SATURATE      = 0x10,
            FLAG_AUTO_EXPOSURE = 0x20,
        };

        uint32_t flag;
        Fcurve auto_exposure;
        Fcurve exposure;
        Fcurve exposure_rate; // F/F2/X
        Fcurve gamma;
        Fcurve gamma_rate; // F/F2/X
        Fcurve saturate;

        CameraAuxiliary();
        ~CameraAuxiliary();
    };

    struct CameraInterest {
        ModelTransform model_transform;

        CameraInterest();
        ~CameraInterest();
    };

    struct CameraViewPoint {
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
        ~CameraViewPoint();
    };

    struct CameraRoot {
        ModelTransform model_transform;
        CameraInterest interest;
        CameraViewPoint view_point;

        CameraRoot();
        ~CameraRoot();
    };

    struct Chara {
        ModelTransform model_transform;
        std::string name;

        Chara();
        ~Chara();
    };

    struct Curve {
        Fcurve fcurve;
        std::string name;

        Curve();
        ~Curve();
    };

    struct Dof {
        bool has_dof;
        ModelTransform model_transform;

        Dof();
        ~Dof();
    };

    struct Event {
        float_t begin;
        float_t clip_begin;
        float_t clip_end;
        float_t end;
        std::string name;
        std::string param1;
        std::string ref;
        float_t time_ref_scale;
        EVENT_TYPE type;

        Event();
        ~Event();
    };

    struct Fog {
        enum {
            FLAG_DENSITY = 0x01,
            FLAG_START   = 0x02,
            FLAG_END     = 0x04,
            FLAG_COLOR   = 0x08,
        };

        uint32_t flag;
        int32_t id;
        std::string name;
        Fcurve density;
        Fcurve start;
        Fcurve end;
        FcurveColor4f color;

        Fog();
        ~Fog();
    };

    struct Light {
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
        int32_t id;
        std::string name;
        std::string type;

        ModelTransform position;
        ModelTransform spot_direction;
        FcurveColor4f ambient;
        FcurveColor4f diffuse;
        FcurveColor4f specular;
        FcurveColor4f tone_curve;

        // XHD
        Fcurve constant;
        Fcurve intensity;
        Fcurve far_; // MS...
        Fcurve linear;
        Fcurve quadratic;
        Fcurve drop_off;
        Fcurve cone_angle;

        Light();
        ~Light();
    };

    struct MObjectHrc {
        std::vector<ObjectInstance> instance;
        ModelTransform model_transform;
        std::string name;
        std::vector<ObjectNode> node;

        MObjectHrc();
        ~MObjectHrc();
    };

    // X
    struct MaterialList {
        enum {
            FLAG_EMISSION       = 0x01,
            FLAG_BLEND_COLOR    = 0x02,
            FLAG_GLOW_INTENSITY = 0x04,
        };

        uint32_t flag;
        std::string name;
        FcurveColor4f emission;
        FcurveColor4f blend_color;
        Fcurve glow_intensity;

        MaterialList();
        ~MaterialList();
    };

    struct Motion {
        std::string name;

        Motion();
        ~Motion();
    };

    struct Object {
        std::string name;
        std::string uid_name;
        ModelTransform model_transform;
        std::string morph;
        float_t morph_offset;
        std::string pattern;
        float_t pattern_offset;
        std::string parent_name;
        std::string parent_node;
        std::vector<Texture> texture;
        std::vector<TextureTransform> texture_transform;

        Object();
        ~Object();
    };

    struct ObjectHrc {
        std::string name;
        std::vector<ObjectNode> node;
        std::string parent_name;
        std::string parent_node;
        bool shadow;
        std::string uid_name;

        ObjectHrc();
        ~ObjectHrc();
    };

    struct ObjectInstance {
        std::string name;
        std::string uid_name;
        bool shadow;
        ModelTransform model_transform;

        ObjectInstance();
        ~ObjectInstance();
    };

    struct ObjectNode {
        enum {
            FLAG_JOINT_ORIENT = 0x01,
        };

        uint32_t flag;
        std::string name;
        int32_t parent;
        ModelTransform model_transform;
        vec3 joint_orient;

        ObjectNode();
        ~ObjectNode();
    };

    struct PlayControl {
        enum {
            FLAG_DIV    = 0x01, // F2/X
            FLAG_OFFSET = 0x02,
        };

        uint32_t flag;
        float_t begin;
        int32_t div; // F2/X
        float_t fps;
        float_t offset; // F2/X/XHD
        float_t size;

        PlayControl();
        ~PlayControl();
    };

    struct Point {
        ModelTransform model_transform;
        std::string name;

        Point();
        ~Point();
    };

    struct PostProcess {
        enum {
            FLAG_LENS_FLARE = 0x01,
            FLAG_LENS_SHAFT = 0x02,
            FLAG_LENS_GHOST = 0x04,
            FLAG_RADIUS     = 0x08,
            FLAG_INTENSITY  = 0x10,
            FLAG_SCENE_FADE = 0x20,
        };

        uint32_t flag;
        FcurveColor4f intensity;
        Fcurve lens_flare;
        Fcurve lens_shaft;
        Fcurve lens_ghost;
        FcurveColor4f radius;
        FcurveColor4f scene_fade;

        PostProcess();
        ~PostProcess();
    };

    struct Texture {
        std::string name;
        std::string pattern;
        float_t pattern_offset;

        Texture();
        ~Texture();
    };

    struct TextureTransform {
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
        std::string name;
        Fcurve coverage_u;
        Fcurve coverage_v;
        Fcurve repeat_u;
        Fcurve repeat_v;
        Fcurve translate_frame_u;
        Fcurve translate_frame_v;
        Fcurve offset_u;
        Fcurve offset_v;
        Fcurve rotate_frame;
        Fcurve rotate;

        TextureTransform();
        ~TextureTransform();
    };

    struct Scene {
        bool ready;
        bool compressed;
        FORMAT format;

        COMPRESS_TYPE _compress_f16;
        std::string _file_name;
        std::string _property_version;
        std::string _converter_version;

        std::vector<Ambient> ambient; // MGF
        std::vector<Auth2d> auth_2d;
        CameraAuxiliary camera_auxiliary;
        std::vector<CameraRoot> camera_root;
        std::vector<Chara> chara;
        std::vector<Curve> curve;
        Dof dof;
        std::vector<Event> event;
        std::vector<Fog> fog;
        std::vector<Light> light;
        std::vector<MObjectHrc> m_object_hrc;
        std::vector<std::string> m_object_hrc_list;
        std::vector<MaterialList> material_list; // X
        std::vector<Motion> motion;
        std::vector<Object> object;
        std::vector<std::string> object_list;
        std::vector<ObjectHrc> object_hrc;
        std::vector<std::string> object_hrc_list;
        PlayControl play_control;
        std::vector<Point> point;
        PostProcess post_process;

        Scene();
        ~Scene();

        void read(const char* path);
        void read(const wchar_t* path);
        void read(const void* data, size_t size);
        void write(const char* path);
        void write(const wchar_t* path);
        void write(void** data, size_t* size);

        static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
    };
}
