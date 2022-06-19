/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "frame_rate_control.hpp"
#include "render_texture.hpp"
#include "stage.hpp"
#include "task.hpp"

enum TaskEffectType {
    TASK_EFFECT_INVALID  = -1,
    TASK_EFFECT_AUTH_3D  = 0x00,
    TASK_EFFECT_TYPE_1   = 0x01,
    TASK_EFFECT_LEAF     = 0x02,
    TASK_EFFECT_TYPE_3   = 0x03,
    TASK_EFFECT_SNOW     = 0x04,
    TASK_EFFECT_TYPE_5   = 0x05,
    TASK_EFFECT_RIPPLE   = 0x06,
    TASK_EFFECT_RAIN     = 0x07,
    TASK_EFFECT_TYPE_8   = 0x08,
    TASK_EFFECT_TYPE_9   = 0x09,
    TASK_EFFECT_TYPE_10  = 0x0A,
    TASK_EFFECT_TYPE_11  = 0x0B,
    TASK_EFFECT_SPLASH   = 0x0C,
    TASK_EFFECT_TYPE_13  = 0x0D,
    TASK_EFFECT_FOG_ANIM = 0x0E,
    TASK_EFFECT_TYPE_15  = 0x0F,
    TASK_EFFECT_FOG_RING = 0x10,
    TASK_EFFECT_TYPE_17  = 0x11,
    TASK_EFFECT_PARTICLE = 0x12,
    TASK_EFFECT_LITPROJ  = 0x13,
    TASK_EFFECT_STAR     = 0x14,
};

class TaskEffect : public app::Task {
public:
    TaskEffect();
    virtual ~TaskEffect() override;

    virtual void PreInit(int32_t stage_index);
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices);
    virtual void SetFrame(int32_t value);
    virtual void Field_48();
    virtual void SetEnable(bool);
    virtual void SetStageIndex(int32_t value);
    virtual void SetFrameRateControl(FrameRateControl* value);
    virtual void Field_68();
    virtual void Reset();
    virtual void Field_78();
    virtual void Field_80();
    virtual void Field_88();
    virtual void Field_90();
    virtual void Field_98(int32_t a2, int32_t* a3);
    virtual void Field_A0(int32_t a2, int32_t* a3);
    virtual void Field_A8(int32_t a2, int8_t* a3);
};

;

struct struc_621 {
    int32_t stage_index;
    std::vector<int32_t> auth_3d_ids;
};

struct TaskEffectAuth3D : public TaskEffect {
    struct Stage {
        size_t count;
        size_t max_count;
        int32_t* auth_3d_ids_ptr;
        int32_t auth_3d_ids[TASK_STAGE_STAGE_COUNT];

        Stage();
    } stage;
    bool enable;
    std::vector<struc_621> field_120;
    int32_t stage_index;
    int32_t field_13C;
    std::vector<int32_t> stage_indices;
    int8_t field_158;
    int8_t field_159;
    int32_t field_15C;
    float_t frame;

    TaskEffectAuth3D();
    virtual ~TaskEffectAuth3D() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void SetEnable(bool value) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;

    void ResetData();
    void SetVisibility(bool value);
};

struct TaskEffectFogAnim : public TaskEffect {
    struct Data {
        bool field_0;
        int32_t field_4;
        bool field_8;
        float_t field_C[3];
        float_t field_18[3];
        float_t field_24;
        int32_t field_28;
        int32_t field_2C;
        int32_t field_30;
        vec4 field_34;

        Data();

        void Ctrl();
        void Reset();
    } data;

    TaskEffectFogAnim();
    virtual ~TaskEffectFogAnim() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct point_particle_data {
    vec3 position;
    vec4 color;
    float_t size;
};

struct struc_371 {
    vec3 field_0;
    float_t field_C;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
};

struct TaskEffectFogRing : public TaskEffect {
    struct Data {
        bool enable;
        float_t delta_frame;
        int8_t field_8;
        float_t ring_size;
        vec3 wind_dir;
        int32_t tex_id;
        vec4 color;
        float_t point_size;
        int32_t max_ptcls;
        int32_t num_ptcls;
        float_t density;
        float_t density_offset;
        int32_t field_44;
        int64_t field_48;
        point_particle_data* data;
        int32_t count;
        int32_t field_5C;
        int32_t field_60;
        int32_t field_64;
        int32_t field_68;
        int32_t field_6C;
        int32_t field_70;
        int32_t field_74;
        int32_t field_78;
        int32_t field_7C;
        int32_t field_80;
        int32_t field_84;
        int32_t field_88;
        int32_t field_8C;
        int32_t field_90;
        int32_t field_94;
        int32_t field_98;
        int32_t field_9C;
        int32_t field_A0;
        int32_t field_A4;
        int32_t field_A8;
        int32_t field_AC;
        int32_t field_B0;
        int32_t field_B4;
        int32_t field_B8;
        int32_t field_BC;
        int32_t field_C0;
        int32_t field_C4;
        int32_t field_C8;
        int32_t field_CC;
        int32_t field_D0;
        int32_t field_D4;
        int32_t field_D8;
        int32_t field_DC;
        int32_t field_E0;
        int32_t field_E4;
        int32_t field_E8;
        int32_t field_EC;
        int32_t field_F0;
        int32_t field_F4;
        int32_t field_F8;
        int32_t field_FC;
        int32_t field_100;
        int32_t field_104;
        int32_t field_108;
        int32_t field_10C;
        int32_t field_110;
        int32_t field_114;
        int32_t field_118;
        int32_t field_11C;
        int32_t field_120;
        int32_t field_124;
        int8_t field_128[4];
        float_t field_12C[2];
        int32_t field_134;
        float_t field_138[2];
        struc_371 field_140[2];
        int32_t field_190;
        int32_t field_194;
        int32_t field_198;
        int32_t field_19C;
        int32_t field_1A0;
        int32_t field_1A4;
        int32_t field_1A8;
        int32_t field_1AC;
        int32_t field_1B0;
        int32_t field_1B4;
        int32_t field_1B8;
        int32_t field_1BC;
        int32_t field_1C0;
        int32_t field_1C4;
        int32_t field_1C8;
        int32_t field_1CC;
        int32_t field_1D0;
        int32_t field_1D4;
        int32_t field_1D8;
        int32_t field_1DC;
        int32_t field_1E0;
        int32_t field_1E4;
        int32_t field_1E8;
        int32_t field_1EC;
        int32_t field_1F0;
        int32_t field_1F4;
        int32_t field_1F8;
        int32_t field_1FC;
        int32_t field_200;
        int32_t field_204;
        int32_t field_208;
        int32_t field_20C;
        int32_t field_210;
        int32_t field_214;
        int32_t field_218;
        int32_t field_21C;
        int32_t field_220;
        int32_t field_224;
        int32_t field_228;
        int32_t field_22C;
        int32_t field_230;
        int32_t field_234;
        int32_t field_238;
        int32_t field_23C;
        int32_t field_240;
        int32_t field_244;
        int32_t field_248;
        int32_t field_24C;
        int32_t field_250;
        int32_t field_254;
        int32_t field_258;
        int32_t field_25C;
        int32_t field_260;
        int32_t field_264;
        int32_t field_268;
        int32_t field_26C;
        int32_t field_270;
        int32_t field_274;
        int32_t field_278;
        int32_t field_27C;
        int32_t field_280;
        int32_t field_284;
        int32_t field_288;
        int32_t field_28C;
        int32_t field_290;
        int32_t field_294;
        int32_t field_298;
        int32_t field_29C;
        int32_t field_2A0;
        int32_t field_2A4;
        int64_t field_2A8;
        int32_t field_2B0;
        int32_t field_2B4;
        int8_t field_2B8;
        int8_t field_2B9;
        bool disp;
        int32_t stage_index;
        std::vector<int32_t> stage_indices;
        FrameRateControl* frame_rate_control;
        int64_t field_2E0;

        Data();
        ~Data();

        void Ctrl();
        void SetStageIndices(std::vector<int32_t>& stage_indices);
    } data;

    TaskEffectFogRing();
    virtual ~TaskEffectFogRing() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;
};

struct TaskEffectLeaf : public TaskEffect {
    FrameRateControl* frame_rate_control;
    int32_t stage_index;
    std::vector<int32_t> stage_indices;
    int32_t field_90;
    int32_t field_94;
    int64_t field_98;

    TaskEffectLeaf();
    virtual ~TaskEffectLeaf() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct TaskEffectLitproj : public TaskEffect {
    int32_t stage_index;
    std::vector<int32_t> stage_indices;
    int32_t field_88;
    vec4 diffuse;
    vec4 specular;
    int32_t field_AC;

    TaskEffectLitproj();
    virtual ~TaskEffectLitproj() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct TaskEffectParticle : public TaskEffect {
    FrameRateControl* frame_rate_control;
    int32_t field_70;
    int32_t field_74;
    int64_t field_78;

    TaskEffectParticle();
    virtual ~TaskEffectParticle() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct TaskEffectRain : public TaskEffect {
    FrameRateControl* frame_rate_control;
    int32_t stage_index;
    std::vector<int32_t> stage_indices;

    TaskEffectRain();
    virtual ~TaskEffectRain() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct color4u8 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct  struc_101 {
    int32_t ripple_uniform;
    int32_t ripple_emit_uniform;
    int32_t count;
    int32_t field_C;
    vec3* vertex;
    color4u8* color;
    float_t size;
    int32_t field_24;
};

struct struc_202 {
    vec2 field_0;
    float_t field_8;
};

struct ripple_emit_draw_data {
    struc_101 data;
    vec3 vertex[16];
    color4u8 color[16];
};

struct struc_192 {
    int32_t field_0;
    vec3 field_4;
};

struct struc_207 {
    struc_192 field_0[18];
};

struct ripple_emit {
    float_t delta_frame;
    int8_t field_4;
    int32_t rain_ripple_num;
    float_t rain_ripple_min_value;
    float_t rain_ripple_max_value;
    int32_t field_14;
    float_t ground_y;
    float_t emit_pos_scale;
    float_t emit_pos_ofs_x;
    float_t emit_pos_ofs_z;
    int32_t ripple_tex_id;
    bool use_float_ripplemap;
    int32_t field_30;
    float_t rob_emitter_size;
    size_t emitter_num;
    struc_202* emitter_list;
    float_t emitter_size;
    int32_t field_4C;
    ripple_emit_draw_data field_50;
    ripple_emit_draw_data field_178;
    ripple_emit_draw_data field_2A0;
    ripple_emit_draw_data field_3C8;
    int32_t field_4F0;
    struc_207 field_4F4[6];
    int32_t field_BB4;
    render_texture field_BB8;
    int32_t field_BE8;
    int8_t field_BEC;
    float_t wake_attn;
    float speed;
    float field_BF8;
    float field_BFC;
    int8_t field_C00;
    int32_t stage_index;
    std::vector<int32_t> stage_indices;
};

struct TaskEffectRipple : public TaskEffect {
    int64_t field_68;
    FrameRateControl* frame_rate_control;
    ripple_emit* emit_struct;

    TaskEffectRipple();
    virtual ~TaskEffectRipple() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct TaskEffectSnow : public TaskEffect {
    FrameRateControl* frame_rate_control;
    int32_t stage_index;
    std::vector<int32_t> stage_indices;

    TaskEffectSnow();
    virtual ~TaskEffectSnow() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct struc_180 {
    uint32_t count;
    int32_t field_4;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
    int32_t field_20;
    int64_t field_28;
    int64_t field_30;
    int64_t field_38;
};

struct ParticleEmitter {
    struc_180* field_8;
    int64_t field_10;
    int64_t field_18;
    float_t field_20;
    int32_t field_24;
    float_t field_28;
    float_t particle_size;

    ParticleEmitter();
    virtual ~ParticleEmitter();

    virtual bool Field_8();
    virtual bool Field_10();
};

struct ParticleEmitterRob : ParticleEmitter {
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    int32_t field_40;
    int32_t field_44;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t emit_num;
    int32_t field_60;
    float_t emission_ratio_attn;
    float_t emission_velocity_scale;
    bool in_water;
    int8_t field_6D;
};

struct water_particle {
    struc_180* field_0;
    vec4 color;
    float_t particle_size;
    std::vector<point_particle_data> field_20;
    int32_t count;
    int32_t tex_id;
    bool blink;
    std::vector<vec3> field_48;
    std::vector<color4u8> field_60;
    struc_101 field_78;
    float_t ripple_emission;
};

struct ParticleDispObj {
    struc_180* field_8;
    object_info object;
    std::vector<mat4> instances_mat;

    ParticleDispObj();
    virtual ~ParticleDispObj();

    void Disp();
};

struct TaskEffectSplash : public TaskEffect {
    struct Data {
        struct Sub {
            int32_t field_0;
            struc_180* field_8;
            int32_t field_10;
            ParticleEmitterRob* field_18;
            int32_t field_20;
            water_particle* field_28;
            int8_t field_30;
            struc_180 field_38;
            int32_t field_78;
            ParticleEmitterRob* field_80;
            ParticleDispObj field_88;
            vec4 color;
            float_t particle_size;
            int32_t emit_num;
            float_t ripple_emission;
            float_t emission_ratio_attn;
            float_t emission_velocity_scale;
            int32_t splash_tex_id;
            object_info splash_obj_id;
            int8_t in_water;
            int8_t blink;
            int64_t field_E8;
            int64_t field_F0;
            int64_t field_F8;
        };

        int8_t field_0;
        int32_t field_4;
        int8_t field_8;
        Sub field_10;
        int8_t field_110;
        int32_t stage_index;
        std::vector<int32_t> stage_indices;
        FrameRateControl* frame_rate_control;
        int64_t field_138;
    };

    bool enable;
    Data data;

    TaskEffectSplash();
    virtual ~TaskEffectSplash() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct TaskEffectStar : public TaskEffect {
    FrameRateControl* frame_rate_control;
    int32_t stage_index;
    float_t delta_frame;
    std::vector<int32_t> stage_indices;

    TaskEffectStar();
    virtual ~TaskEffectStar() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void Field_48() override;
    virtual void SetEnable(bool) override;
    virtual void SetStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_78() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};
