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
#include "rob/rob.hpp"
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

struct particle_init_data {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    vec3 trans;
    float_t scale_y;
};

class TaskEffect : public app::Task {
public:
    TaskEffect();
    virtual ~TaskEffect() override;

    virtual void PreInit(int32_t stage_index);
    virtual void SetStageHashes(std::vector<uint32_t>& stage_hashes, void* data,
        object_database* obj_db, texture_database* tex_db, stage_database* stage_data); // Added
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices);
    virtual void SetFrame(int32_t value);
    virtual void Field_48();
    virtual void SetEnable(bool value);
    virtual void SetCurrentStageHash(uint32_t value); // Added
    virtual void SetCurrentStageIndex(int32_t value);
    virtual void SetFrameRateControl(FrameRateControl* value);
    virtual void Field_68();
    virtual void Reset();
    virtual void Event(int32_t event_type, void* data);
    virtual void Field_80();
    virtual void Field_88();
    virtual void Field_90();
    virtual void Field_98(int32_t a2, int32_t* a3);
    virtual void Field_A0(int32_t a2, int32_t* a3);
    virtual void Field_A8(int32_t a2, int8_t* a3);
};

struct struc_621 {
    uint32_t stage_hash; // Added
    int32_t stage_index;
    std::vector<auth_3d_id> auth_3d_ids;

    struc_621();
    ~struc_621();
};

struct TaskEffectAuth3D : public TaskEffect {
public:
    struct Stage {
        size_t count;
        size_t max_count;
        auth_3d_id* auth_3d_ids_ptr;
        auth_3d_id auth_3d_ids[TASK_STAGE_STAGE_COUNT];

        Stage();
    } stage;
    bool enable;
    std::vector<struc_621> field_120;
    uint32_t current_stage_hash; // Added
    int32_t current_stage_index;
    int32_t field_13C;
    std::vector<uint32_t> stage_hashes;
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
    virtual void SetStageHashes(std::vector<uint32_t>& stage_hashes, void* data,
        object_database* obj_db, texture_database* tex_db, stage_database* stage_data) override; // Added
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetFrame(int32_t value) override;
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageHash(uint32_t value) override; // Added
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;

    void ResetData();
    void SetVisibility(bool value);
};

struct TaskEffectFogAnim : public TaskEffect {
public:
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

    virtual void PreInit(int32_t stage_index) override;
    virtual void Reset() override;
};

struct fog_ring_data {
    vec3 position;
    vec3 direction;
    vec3 field_18;
    float_t size;
    float_t density;

    fog_ring_data();
};

struct particle_event_data {
    float_t type;
    float_t count;
    float_t size;
    vec3 trans;
    float_t force;

    particle_event_data();
};

struct point_particle_data {
    vec2 position;
    vec4 color;
};

struct struc_371 {
    int32_t field_0;
    vec3 position;
    float_t field_10;
    float_t field_14;
    vec3 direction;
    float_t field_24;

    struc_371();
};

struct struc_573 {
    int32_t chara_index;
    rob_bone_index bone_index;
    vec3 position;

    struc_573();
};


struct TaskEffectFogRing : public TaskEffect {
public:
    struct Data {
        bool enable;
        float_t delta_frame;
        bool field_8;
        float_t ring_size;
        vec3 wind_dir;
        int32_t tex_id;
        vec4 color;
        float_t ptcl_size;
        int32_t max_ptcls;
        int32_t num_ptcls;
        float_t density;
        float_t density_offset;
        fog_ring_data* ptcl_data;
        int32_t num_vtx;
        struc_573 field_5C[2][5];
        int32_t field_124;
        struc_371 field_128[10];
        int8_t field_2B8;
        int8_t field_2B9;
        bool disp;
        int32_t current_stage_index;
        std::vector<int32_t> stage_indices;
        FrameRateControl* frame_rate_control;
        GLuint vao;
        GLuint vbo;

        Data();
        ~Data();

        void CalcPtcl(float_t delta_time);
        void CalcVert();
        void Ctrl();
        void CtrlInner(float_t delta_time);
        void Dest();
        void Disp();
        void Draw();
        void InitParticleData();
        void Reset();
        void SetStageIndices(std::vector<int32_t>& stage_indices);

        static void DrawStatic(void* data);
        static float_t PtclRandom(float_t value);

        void sub_140347B40(float_t delta_time);

        static void sub_140347860(fog_ring_data* a1, int32_t a2, struc_371* a3, float_t a4);
    } data;

    TaskEffectFogRing();
    virtual ~TaskEffectFogRing() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;
};

struct TaskEffectLeaf : public TaskEffect {
public:
    FrameRateControl* frame_rate_control;
    int32_t current_stage_index;
    std::vector<int32_t> stage_indices;
    int32_t wait_frames;

    TaskEffectLeaf();
    virtual ~TaskEffectLeaf() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;
};

struct TaskEffectLitproj : public TaskEffect {
public:
    int32_t current_stage_index;
    std::vector<int32_t> stage_indices;
    int32_t wait_frames;
    vec4 diffuse;
    vec4 specular;
    int32_t frame;

    TaskEffectLitproj();
    virtual ~TaskEffectLitproj() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void Reset() override;
};

struct TaskEffectParticle : public TaskEffect {
public:
    FrameRateControl* frame_rate_control;
    int32_t current_stage_index;

    TaskEffectParticle();
    virtual ~TaskEffectParticle() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetEnable(bool value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;
    virtual void Event(int32_t event_type, void* data) override;
};

struct TaskEffectRain : public TaskEffect {
public:
    FrameRateControl* frame_rate_control;
    int32_t current_stage_index;
    std::vector<int32_t> stage_indices;

    TaskEffectRain();
    virtual ~TaskEffectRain() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;
};

struct color4u8 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct struc_101 {
    int32_t ripple_uniform;
    int32_t ripple_emit_uniform;
    int32_t count;
    vec3* vertex;
    color4u8* color;
    float_t size;
    int32_t field_24;
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
    vec3* emitter_list;
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
    float_t speed;
    float_t field_BF8;
    float_t field_BFC;
    int8_t field_C00;
    int32_t current_stage_index;
    std::vector<int32_t> stage_indices;
};

struct TaskEffectRipple : public TaskEffect {
public:
    int64_t field_68;
    FrameRateControl* frame_rate_control;
    ripple_emit* emit;

    TaskEffectRipple();
    virtual ~TaskEffectRipple() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    virtual void PreInit(int32_t stage_index) override;
    virtual void SetStageIndices(std::vector<int32_t>& stage_indices) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Reset() override;
};

struct TaskEffectSnow : public TaskEffect {
public:
    FrameRateControl* frame_rate_control;
    int32_t current_stage_index;
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
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
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
public:
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
        int32_t current_stage_index;
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
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

struct TaskEffectStar : public TaskEffect {
public:
    FrameRateControl* frame_rate_control;
    int32_t current_stage_index;
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
    virtual void SetEnable(bool value) override;
    virtual void SetCurrentStageIndex(int32_t value) override;
    virtual void SetFrameRateControl(FrameRateControl* value) override;
    virtual void Field_68() override;
    virtual void Reset() override;
    virtual void Field_80() override;
    virtual void Field_88() override;
    virtual void Field_90() override;
    virtual void Field_98(int32_t a2, int32_t* a3) override;
    virtual void Field_A0(int32_t a2, int32_t* a3) override;
    virtual void Field_A8(int32_t a2, int8_t* a3) override;
};

extern void leaf_particle_draw();
extern void rain_particle_draw();
extern void particle_draw();
extern void snow_particle_draw();

extern void task_effect_init();
extern void task_effect_free();

extern void task_effect_parent_event(TaskEffectType type, int32_t event_type, void* data);
extern void task_effect_parent_dest();
extern bool task_effect_parent_load();
extern void task_effect_parent_reset();
extern void task_effect_parent_set_current_stage_hash(uint32_t stage_hash);
extern void task_effect_parent_set_current_stage_index(int32_t stage_index);
extern void task_effect_parent_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data);
extern void task_effect_parent_set_enable(bool value);
extern void task_effect_parent_set_frame(int32_t value);
extern void task_effect_parent_set_frame_rate_control(FrameRateControl* value);
extern void task_effect_parent_set_stage_hashes(std::vector<uint32_t>& stage_hashes);
extern void task_effect_parent_set_stage_indices(std::vector<int32_t>& stage_indices);
extern bool task_effect_parent_unload();
