/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_effect.hpp"
#include "../KKdLib/hash.hpp"
#include "auth_3d.hpp"
#include "data.hpp"
#include "draw_task.hpp"
#include "random.hpp"
#include "render_context.hpp"
#include "shader_ft.hpp"
#include "stage_param.hpp"
#include "texture.hpp"

struct for_ring_vertex_data {
    vec2 position;
    vec4 color;
};

struct leaf_particle_data {
    vec3 position;
    vec3 direction;
    vec3 normal;
    vec3 rotation;
    vec3 rotation_add;
    float_t size;
    int32_t type;

    void init();
};

struct leaf_particle_vertex_data {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
};

struct particle_data {
    vec3 position;
    vec3 direction;
    vec3 normal;
    vec3 rotation;
    vec3 rotation_add;
    vec4 color;
    bool alive;
    float_t type;
    float_t life_time;
    float_t size;
};

struct particle_vertex_data {
    vec3 position;
    vec3 normal;
    vec4 color;
    vec2 texcoord;
};

struct rain_particle_data {
    vec3 position;
    vec2 texcoord;
    float_t alpha;
    int32_t type;
    int32_t field_1C;
    vec3 velocity;
    int32_t field_2C;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;

    rain_particle_data();
};

struct struc_608 {
    const stage_effects* stage_effects;
    const stage_effects_modern* stage_effects_modern;
    std::vector<std::pair<TaskEffectType, TaskEffect*>> field_10;

    struc_608();
    struc_608(const ::stage_effects* stage_effects);
    struc_608(const ::stage_effects_modern* stage_effects_modern);
    ~struc_608();
};

struct TaskEffectParent {
    uint32_t current_stage_hash; // Added
    int32_t current_stage_index;
    std::vector<uint32_t> stage_hashes; // Added
    std::vector<int32_t> stage_indices;
    std::vector<uint32_t> obj_set_ids;
    std::vector<std::pair<TaskEffectType, TaskEffect*>> effects;
    std::vector<std::pair<TaskEffectType, TaskEffect*>> field_50;
    std::vector<std::pair<int32_t, struc_608>> field_68;
    int32_t state;
    bool enable;
    bool modern; // Added

    void* data; // Added
    object_database* obj_db; // Added
    texture_database* tex_db; // Added
    stage_database* stage_data; // Added

    TaskEffectParent();
    virtual ~TaskEffectParent();

    std::pair<TaskEffectType, TaskEffect*> AddTaskEffectTypePtr(TaskEffectType type);
    int32_t CheckTaskEffectTypeLoaded(TaskEffectType type);
    void Event(TaskEffectType type, int32_t event_type, void* data);
    void Dest();
    bool Load();
    void Reset();
    void ResetData();
    void SetCurrentStageHash(uint32_t stage_hash); // Added
    void SetCurrentStageIndex(int32_t stage_index);
    void SetEnable(bool value);
    void SetFrame(int32_t value);
    void SetFrameRateControl(FrameRateControl* value);
    void SetStageHashes(std::vector<uint32_t>& stage_hashes); // Added
    void SetStageIndices(std::vector<int32_t>& stage_indices);
    bool Unload();
};

static TaskEffect* task_effect_array_get(TaskEffectType type, const char** name);
static std::string task_effect_array_get_stage_param_file_path(
    TaskEffectType type, int32_t stage_index, bool dev_ram, bool a4);
static bool task_effect_array_parse_stage_param_data_fog_ring(stage_param_fog_ring* fog_ring, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_leaf(stage_param_leaf* leaf, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_litproj(stage_param_litproj* litproj, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_rain(stage_param_rain* rain, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_ripple(stage_param_ripple* ripple, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_snow(stage_param_snow* snow, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_splash(stage_param_splash* splash, int32_t stage_index);
static bool task_effect_array_parse_stage_param_data_star(stage_param_star* star, int32_t stage_index);

static void draw_fog_particle(render_context* rctx, TaskEffectFogRing::Data* data);

static void leaf_particle_init(bool change_stage = false);
static void leaf_particle_ctrl();
static int32_t leaf_particle_disp();
static void leaf_particle_free();

static void litproj_textures_init();
static void litproj_textures_free();

static void particle_init(vec3* offset);
static void particle_ctrl();
static int32_t particle_disp(particle_vertex_data* vtx_data, particle_data* data, int32_t count);
static particle_data* particle_emit();
static void particle_event(particle_event_data* event_data);
static void particle_kill(particle_data* data);
static void particle_free();

static void rain_particle_init(bool change_stage);
static void rain_particle_ctrl();
static void rain_particle_free();

static TaskEffectAuth3D* task_effect_auth_3d;
static TaskEffectLeaf* task_effect_leaf;
static TaskEffectSnow* task_effect_snow;
static TaskEffectRipple* task_effect_ripple;
static TaskEffectRain* task_effect_rain;
static TaskEffectSplash* task_effect_splash;
static TaskEffectFogAnim* task_effect_fog_anim;
static TaskEffectFogRing* task_effect_fog_ring;
static TaskEffectParticle* task_effect_particle;
static TaskEffectLitproj* task_effect_litproj;
static TaskEffectStar* task_effect_star;

static TaskEffectParent* task_effect_parent;

static TaskEffectFogAnim::Data* task_effect_fog_anim_data;
static TaskEffectFogRing::Data* task_effect_fog_ring_data;
static TaskEffectSplash::Data* task_effect_splash_data;

static stage_param_leaf* stage_param_data_leaf_current;
static stage_param_litproj* stage_param_data_litproj_current;
static stage_param_rain* stage_param_data_rain_current;
static stage_param_snow* stage_param_data_snow_current;

static bool stage_param_data_leaf_set;
static bool stage_param_data_litproj_set;
static bool stage_param_data_rain_set;
static bool stage_param_data_snow_set;

static bool leaf_particle_enable;
static float_t leaf_particle_delta_frame;
static float_t leaf_particle_emit_interval;
static float_t leaf_particle_emit_timer;
static int32_t leaf_particle_num_ptcls;
static uint32_t leaf_particle_tex_id;
static leaf_particle_data* leaf_ptcl_data;
static GLuint leaf_ptcl_vao;
static GLuint leaf_ptcl_vbo;
static GLuint leaf_ptcl_ebo;
static const size_t leaf_ptcl_count = 0x800;

static bool light_proj_enable;
static render_texture litproj_shadow[2];
static render_texture litproj_texture;

static bool particle_enable;
static float_t particle_delta_time;
static int32_t particle_index;
static int32_t particle_count;
static vec3 particle_wind;
static particle_data* ptcl_data;
static GLuint ptcl_vao;
static GLuint ptcl_vbo;
static const size_t ptcl_count = 0x400;

static bool rain_particle_enable;
static float_t rain_particle_delta_frame;
static uint32_t rain_particle_tex_id;
static rain_particle_data rain_ptcl_data[8];
static GLuint rain_ptcl_vao;
static GLuint rain_ptcl_vbo;
static GLuint rain_ptcl_ebo;
static const size_t rain_ptcl_count = 0x8000;

static TaskEffect** task_effect_data_array[] = {
    (TaskEffect**)&task_effect_auth_3d,
    0,
    (TaskEffect**)&task_effect_leaf,
    0,
    0/*(TaskEffect**)&task_effect_snow*/,
    0,
    0/*(TaskEffect**)&task_effect_ripple*/,
    (TaskEffect**)&task_effect_rain,
    0,
    0,
    0,
    0,
    0/*(TaskEffect**)&task_effect_splash*/,
    0,
    (TaskEffect**)&task_effect_fog_anim,
    0,
    (TaskEffect**)&task_effect_fog_ring,
    0,
    (TaskEffect**)&task_effect_particle,
    (TaskEffect**)&task_effect_litproj,
    0/*(TaskEffect**)task_effect_star*/,
};

static const char* task_effect_name_array[] = {
    "EFFECT_AUTH3D",
    0,
    "EFFECT_LEAF",
    0,
    "EFFECT_SNOW",
    0,
    "EFFECT_RIPPLE",
    "EFFECT_RAIN",
    0,
    0,
    0,
    0,
    "EFFECT_SPLASH",
    0,
    "EFFECT_FOG_ANIM",
    0,
    "EFFECT_FOG_RING",
    0,
    "EFFECT_PARTICLE",
    "EFFECT_LITPROJ",
    "EFFECT_STAR",
};

extern render_context* rctx_ptr;

TaskEffect::TaskEffect() {

}

TaskEffect::~TaskEffect() {

}

void TaskEffect::PreInit(int32_t stage_index) {

}

void TaskEffect::SetStageHashes(std::vector<uint32_t>& stage_hashes, void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data) {

}

void TaskEffect::SetStageIndices(std::vector<int32_t>& stage_indices) {
    PreInit(stage_indices[0]);
}

void TaskEffect::SetFrame(int32_t value) {

}

void TaskEffect::Field_48() {

}

void TaskEffect::SetEnable(bool value) {

}

void TaskEffect::SetCurrentStageHash(uint32_t value) {

}

void TaskEffect::SetCurrentStageIndex(int32_t value) {

}

void TaskEffect::SetFrameRateControl(FrameRateControl* value) {

}

void TaskEffect::Field_68() {

}

void TaskEffect::Reset() {

}

void TaskEffect::Event(int32_t event_type, void* data) {

}

void TaskEffect::Field_80() {

}

void TaskEffect::Field_88() {

}

void TaskEffect::Field_90() {

}

void TaskEffect::Field_98(int32_t a2, int32_t* a3) {
    if (a3)
        *a3 = 0;
}

void TaskEffect::Field_A0(int32_t a2, int32_t* a3) {
    if (a3)
        *a3 = 0;
}

void TaskEffect::Field_A8(int32_t a2, int8_t* a3) {
    if (a3)
        *a3 = 0;
}

struc_621::struc_621() {
    stage_hash = hash_murmurhash_empty;
    stage_index = -1;
}

struc_621::~struc_621() {

}

TaskEffectAuth3D::Stage::Stage() : auth_3d_ids() {
    count = 0;
    max_count = TASK_STAGE_STAGE_COUNT;
    for (auth_3d_id& i : auth_3d_ids)
        i = -1;
    auth_3d_ids_ptr = auth_3d_ids;
}

TaskEffectAuth3D::TaskEffectAuth3D() : enable(),
field_13C(), field_158(), field_159(), field_15C(), frame() {
    current_stage_hash = hash_murmurhash_empty;
    current_stage_index = -1;
}

TaskEffectAuth3D::~TaskEffectAuth3D() {

}

bool TaskEffectAuth3D::Init() {
    if (!task_auth_3d_check_task_ready()) {
        SetDest();
        return true;
    }

    if (stage.count) {
        for (int32_t i = 0; i < stage.count; i++)
            if (!stage.auth_3d_ids_ptr[i].check_loaded())
                return false;
    }

    for (int32_t i = 0; i < stage.count; i++) {
        auth_3d_id& id = stage.auth_3d_ids_ptr[i];
        id.set_repeat(true);
        id.set_paused(false);
        id.set_enable(true);
    }

    //if (field_158)
    //    sub_14036D310(2, (__int64)sub_140345B70, (__int64)this);
    return true;
}

bool TaskEffectAuth3D::Ctrl() {
    if (field_158 && field_15C > 0)
        field_15C = 0;

    if (field_159) {
        int32_t frame_int;
        if (frame >= 0.0f) {
            frame_int = (int32_t)frame;
            frame += get_delta_frame();
            if (frame > (float_t)(512 * 360))
                frame = 0.0f;
        }
        else
            frame_int = 0;

        mat4 mat;
        mat4_translate(0.0f, -0.2f, 0.0f, &mat);
        mat4_rotate_x_mult(&mat, sinf((float_t)((double_t)((float_t)(frame_int % 512)
            * (float_t)(2.0 / 512.0)) * M_PI)) * 0.015f, &mat);
        mat4_rotate_z_mult(&mat, sinf((float_t)((double_t)((float_t)(frame_int % 360)
            * (float_t)(2.0 / 360.0)) * M_PI)) * 0.015f, &mat);
        task_stage_set_mat(&mat);
    }
    return false;
}

bool TaskEffectAuth3D::Dest() {
    //if (field_158)
    //    sub_14036D1E0(2, (__int64)sub_140345B70, (__int64)this);
    ResetData();
    return true;
}

void TaskEffectAuth3D::Disp() {

}

void TaskEffectAuth3D::PreInit(int32_t stage_index) {

}

void TaskEffectAuth3D::SetStageHashes(std::vector<uint32_t>& stage_hashes, void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data) {
    this->stage_hashes.assign(stage_hashes.begin(), stage_hashes.end());
    stage_indices.clear();
    for (uint32_t i : this->stage_hashes) {
        stage_data_modern* stg_data = stage_data->get_stage_data_modern(i);
        if (!stg_data || !stg_data->auth_3d_ids.size())
            continue;

        struc_621 v30;
        v30.stage_hash = i;

        if (stage.count != stage.max_count) {
            auth_3d_id id = {};
            for (uint32_t j : stg_data->auth_3d_ids) {
                id = {};
                if (stage.count == stage.max_count)
                    break;

                id = auth_3d_data_load_hash(j, data, obj_db, tex_db);
                if (id.check_not_empty())
                    break;
            }

            if (id.not_null()) {
                id.read_file_modern();
                id.set_visibility(false);
                if (stage.count + 1 <= stage.max_count)
                    stage.auth_3d_ids_ptr[stage.count++] = id;
                v30.auth_3d_ids.push_back(id);
            }
        }

        field_120.push_back(v30);
    }
}

void TaskEffectAuth3D::SetStageIndices(std::vector<int32_t>& stage_indices) {
    stage_hashes.clear();
    this->stage_indices.assign(stage_indices.begin(), stage_indices.end());
    for (int32_t i : this->stage_indices) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        stage_data* stage_data = aft_stage_data->get_stage_data(i);
        if (!stage_data || !stage_data->auth_3d_ids.size())
            continue;

        struc_621 v30;
        v30.stage_index = i;

        if (stage.count != stage.max_count) {
            auth_3d_id id = -1;
            for (int32_t j : stage_data->auth_3d_ids) {
                id = -1;
                if (stage.count == stage.max_count)
                    break;

                id = auth_3d_data_load_uid(j, aft_auth_3d_db);
                if (id.check_not_empty())
                    break;
            }

            if (id.not_null()) {
                id.read_file(aft_auth_3d_db);
                id.set_visibility(false);
                if (stage.count + 1 <= stage.max_count)
                    stage.auth_3d_ids_ptr[stage.count++] = id;
                v30.auth_3d_ids.push_back(id);
            }
        }

        field_120.push_back(v30);
    }
}

void TaskEffectAuth3D::SetFrame(int32_t value) {
    double_t frame = (double_t)value;
    for (int32_t i = 0; i < stage.count; i++) {
        auth_3d_id& id = stage.auth_3d_ids_ptr[i];
        float_t play_control_size = id.get_play_control_size();
        float_t req_frame;
        if (play_control_size != 0.0f)
            req_frame = (float_t)fmod(frame, play_control_size);
        else
            req_frame = 0.0f;
        id.set_req_frame(req_frame);
    }
}

void TaskEffectAuth3D::SetCurrentStageHash(uint32_t value) {
    if (current_stage_hash == value)
        return;

    SetVisibility(false);
    current_stage_hash = value;
    SetVisibility(enable);
}

void TaskEffectAuth3D::SetCurrentStageIndex(int32_t value) {
    if (current_stage_index == value)
        return;

    SetVisibility(false);
    current_stage_index = value;
    SetVisibility(enable);
}

void TaskEffectAuth3D::SetFrameRateControl(FrameRateControl* value) {
    for (int32_t i = 0; i < stage.count; i++) {
        auth_3d_id& id = stage.auth_3d_ids_ptr[i];
        id.set_frame_rate(value);
    }
}

void TaskEffectAuth3D::Reset() {
    for (int32_t i = 0; i < stage.count; i++) {
        auth_3d_id& id = stage.auth_3d_ids_ptr[i];
        id.set_enable(true);
        id.set_paused(false);
        id.set_req_frame(0.0f);
    }
}

void TaskEffectAuth3D::ResetData() {
    for (int32_t i = 0; i < stage.count; i++) {
        auth_3d_id& id = stage.auth_3d_ids_ptr[i];
        id.unload_id(rctx_ptr);
    }
    stage.count = 0;
    enable = false;

    field_120.clear();
    current_stage_hash = hash_murmurhash_empty;
    current_stage_index = -1;
    stage_indices.clear();
    field_158 = 0;
    field_159 = 0;
    field_15C = 0;
    frame = -1.0f;
}

void TaskEffectAuth3D::SetVisibility(bool value) {
    if (current_stage_index != -1)
        for (struc_621& i : field_120) {
            if (i.stage_index != current_stage_index)
                continue;

            for (auth_3d_id& j : i.auth_3d_ids)
                j.set_visibility(value);
            break;
        }
    else if (current_stage_hash != -1 && current_stage_hash
        != hash_murmurhash_empty && current_stage_hash != hash_murmurhash_null)
        for (struc_621& i : field_120) {
            if (i.stage_hash != current_stage_hash)
                continue;

            for (auth_3d_id& j : i.auth_3d_ids)
                j.set_visibility(value);
            break;
        }
}

void TaskEffectAuth3D::SetEnable(bool value) {
    enable = value;
    SetVisibility(value);
}

TaskEffectFogAnim::Data::Data() : field_0(), field_4(), field_8(), field_C(),
field_18(), field_24(), field_28(), field_2C(), field_30(), field_34() {
    Reset();
}

void TaskEffectFogAnim::Data::Ctrl() {
    if (!field_0 || !field_8)
        return;

    float_t v4 = field_C[0] * DEG_TO_RAD_FLOAT;
    float_t v7 = (sinf(v4 - 1.5f) + 1.0f) * 0.5f + field_34.x;
    v7 = min_def(v7, 1.0f) * field_24;
    float_t v8 = sinf(v4) * v7;
    float_t v9 = cosf(v4);

    vec4 color;
    color.x = (float_t)((v9 * v7) * 1.4022 + 1.0);
    color.y = (float_t)(1.0 - v8 * 0.345686 - ((double_t)v9 * v7) * 0.714486);
    color.z = (float_t)(v8 * 1.771 + 1.0);
    color.w = 1.0f;
    *(vec3*)&color = vec3::max(*(vec3*)&color, 0.0f);

    for (int32_t i = 0; i < 3; i++) {
        float_t v20 = (field_18[i] * 10.0f) + field_C[i];
        if (v20 > 360.0)
            v20 = fmodf(v20, 360.0);
        field_C[i] = v20;
    }

    fog& fog = rctx_ptr->fog[FOG_DEPTH];
    fog.set_color(color);

    *(vec3*)&color = (*(vec3*)&color + 1.0f) * 0.8f;

    light_set& light_set = rctx_ptr->light_set[LIGHT_SET_MAIN];
    light_set.lights[LIGHT_CHARA].set_diffuse(color);
    light_set.lights[LIGHT_CHARA].set_specular(color);
    light_set.lights[LIGHT_STAGE].set_diffuse(color);
    light_set.lights[LIGHT_STAGE].set_specular(color);
}

void TaskEffectFogAnim::Data::Reset() {
    field_0 = 0;
    field_4 = -1;
    field_8 = 1;
    field_C[0] = 0.0f;
    field_C[1] = 0.0f;
    field_C[2] = 0.0f;
    field_18[0] = 0.0f;
    field_18[1] = 0.0f;
    field_18[2] = 0.0f;
    field_24 = 0;
    field_28 = 0;
    field_2C = 0;
    field_30 = 0;
    field_34 = 1.0f;
}

TaskEffectFogAnim::TaskEffectFogAnim() {

}

TaskEffectFogAnim::~TaskEffectFogAnim() {

}

bool TaskEffectFogAnim::Init() {
    //sub_1400DE640("TaskEffectFogAnim::init()\n");
    return true;
}

bool TaskEffectFogAnim::Ctrl() {
    data.Ctrl();
    return false;
}

bool TaskEffectFogAnim::Dest() {
    data.Reset();
    task_effect_fog_anim_data = 0;
    //sub_1400DE640("TaskEffectFogAnim::dest()\n");
    return true;
}

void TaskEffectFogAnim::Disp() {

}

void TaskEffectFogAnim::PreInit(int32_t) {
    data.Reset();
    task_effect_fog_anim_data = &data;
    //sub_1400DE640("TaskEffectFogAnim::pre_init()\n");
}

void TaskEffectFogAnim::Reset() {
    if (data.field_0) {
        data.field_C[0] = 0.0f;
        data.field_C[1] = 0.0f;
        data.field_C[2] = 0.0f;
    }
}

fog_ring_data::fog_ring_data() : size(), density() {

}

particle_event_data::particle_event_data() : type(), count(), size(), force() {

}

struc_371::struc_371() : field_0(), field_10(), field_14(), field_24() {

}

struc_573::struc_573() : chara_index(), bone_index() {

}

TaskEffectFogRing::Data::Data() : enable(), delta_frame(), field_8(), ring_size(), tex_id(),
ptcl_size(), max_ptcls(), num_ptcls(), density(), density_offset(), ptcl_data(), num_vtx(),
field_124(), field_2B8(), field_2B9(), disp(), frame_rate_control(), vao(), vbo(), ebo() {
    current_stage_index = -1;
}

TaskEffectFogRing::Data::~Data() {

}

void TaskEffectFogRing::Data::CalcPtcl(float_t delta_time) {
    float_t delta_time1 = 1.0f * delta_time;
    float_t delta_time2 = 2.0f * delta_time;
    float_t delta_time3 = 3.0f * delta_time;

    fog_ring_data* ptcl_data = this->ptcl_data;
    float_t ring_size = (this->ring_size * 0.5f) + 1.0f;

    for (int32_t i = num_ptcls; i > 0; i--, ptcl_data++) {
        sub_140347860(ptcl_data, field_124, field_128, delta_time);

        float_t v12 = ptcl_data->field_18.x;
        float_t v13 = ptcl_data->field_18.z;

        float_t pos_x = ptcl_data->position.x;
        float_t pos_z = ptcl_data->position.z;

        pos_x = (v12 + ptcl_data->direction.x) * delta_time + pos_x;
        pos_z = (v13 + ptcl_data->direction.z) * delta_time + pos_z;

        if (pos_x < -ring_size)
            pos_x = ring_size;
        else if (pos_x > ring_size)
            pos_x = -ring_size;

        if (pos_z < -ring_size)
            pos_z = ring_size;
        else if (pos_z > ring_size)
            pos_z = -ring_size;

        ptcl_data->position.x = pos_x;
        ptcl_data->position.z = pos_z;

        bool v11 = false;
        if (fabsf(v12) > 0.00001f) {
            v11 = true;
            ptcl_data->field_18.x = v12 - v12 * delta_time3;
        }

        ptcl_data->field_18.y = 0.0f;

        if (fabsf(v13) > 0.00001f) {
            v11 = true;
            ptcl_data->field_18.z = v13 - v13 * delta_time3;
        }

        float_t v17;
        if (v11) {
            float_t v14 = ptcl_data->field_18.x;
            float_t v15 = ptcl_data->field_18.z;
            v17 = ptcl_data->density - v15 * v15 + v14 * v14 * delta_time1;
            v17 = max_def(v17, 0.0f);
        }
        else {
            v17 = delta_time2 + ptcl_data->density;
            v17 = min_def(v17, 1.0f);
        }


        ptcl_data->density = v17;
    }
}

void TaskEffectFogRing::Data::CalcVert() {
    float_t density = this->density;
    fog_ring_data* ptcl_data = this->ptcl_data;

    for_ring_vertex_data* ptcl_vtx_data;
    if (GLAD_GL_VERSION_4_5) {
        ptcl_vtx_data = (for_ring_vertex_data*)glMapNamedBuffer(vbo, GL_WRITE_ONLY);
        if (!ptcl_vtx_data) {
            glUnmapNamedBuffer(vbo);
            num_vtx = 0;
            return;
        }
    }
    else {
        gl_state_bind_array_buffer(vbo);
        ptcl_vtx_data = (for_ring_vertex_data*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (!ptcl_vtx_data) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            gl_state_bind_array_buffer(0);
            num_vtx = 0;
            return;
        }
    }

    vec4 color = this->color;
    for (int32_t i = num_ptcls; i > 0; i--, ptcl_data++, ptcl_vtx_data += 4) {
        float_t size = ptcl_data->size * (float_t)((1.0 / 128.0 + 1.0 / 256.0) / 2.0);
        color.w = ptcl_data->density * density;

        vec2 position;
        position.x = ptcl_data->position.x * (float_t)(1.0 / 8.0);
        position.y = ptcl_data->position.z * (float_t)(1.0 / 8.0);
        ptcl_vtx_data[0].position = position + vec2(-size,  size);
        ptcl_vtx_data[0].color = color;
        ptcl_vtx_data[1].position = position + vec2(-size, -size);
        ptcl_vtx_data[1].color = color;
        ptcl_vtx_data[2].position = position + vec2( size, -size);
        ptcl_vtx_data[2].color = color;
        ptcl_vtx_data[3].position = position + vec2( size,  size);
        ptcl_vtx_data[3].color = color;
    }

    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(vbo);
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    num_vtx = (int32_t)(num_ptcls * 4LL);
}

void TaskEffectFogRing::Data::Ctrl() {
    if (!disp)
        return;

    CtrlInner(delta_frame * (float_t)(1.0 / 60.0));
    CalcVert();
    field_8 = 0;
}

void TaskEffectFogRing::Data::CtrlInner(float_t delta_time) {
    if (fabs(delta_time) <= 0.000001f)
        return;

    sub_140347B40(delta_time);
    CalcPtcl(delta_time);
}

void TaskEffectFogRing::Data::Dest() {
    stage_param_data_fog_ring_storage_clear();

    if (ptcl_data) {
        free(ptcl_data);
        ptcl_data = 0;
    }

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }

    rctx_ptr->draw_pass.set_enable(DRAW_PASS_PRE_PROCESS, false);
    rctx_ptr->draw_pass.clear_preprocess(0);
    rctx_ptr->draw_state.set_fog_height(false);
}

void TaskEffectFogRing::Data::Disp() {
    if (enable && disp)
        draw_task_add_draw_preprocess(rctx_ptr, &mat4_identity,
            (void(*)(render_context * rctx, void* data))draw_fog_particle, this, DRAW_OBJECT_PREPROCESS);
}

void TaskEffectFogRing::Data::Draw() {
    render_context* rctx = rctx_ptr;

    rctx->draw_state.set_fog_height(false);
    if (!enable || !disp)
        return;

    rctx->draw_state.set_fog_height(true);
    render_texture& rt = rctx->draw_pass.get_render_texture(8);
    rt.bind();
    glViewport(0, 0,
        rt.color_texture->get_width_align_mip_level(),
        rt.color_texture->get_height_align_mip_level());
    glClearColor(density_offset, density_offset, density_offset, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (draw_task_get_count(rctx, DRAW_OBJECT_PREPROCESS))
        draw_task_draw_objects_by_type(rctx, DRAW_OBJECT_PREPROCESS, 0, 0, true);
    gl_state_bind_framebuffer(0);
    rctx->draw_pass.set_effect_texture(rt.color_texture);
    glGetError();
}

void TaskEffectFogRing::Data::InitParticleData() {
    fog_ring_data* ptcl_data = this->ptcl_data;
    if (!ptcl_data)
        return;

    float_t ptcl_size = this->ptcl_size;
    float_t ring_size = this->ring_size * 0.5f;
    float_t wind_dir_x = wind_dir.x;
    float_t wind_dir_z = wind_dir.z;
    for (int32_t i = max_ptcls; i > 0; i--, ptcl_data++) {
        float_t pos_x = PtclRandom(ring_size);
        float_t pos_z = PtclRandom(ring_size);
        float_t dir_x = PtclRandom(0.5f) + wind_dir_x;
        float_t dir_z = PtclRandom(0.5f) + wind_dir_z;
        float_t size = PtclRandom(3.0f);

        ptcl_data->position.x = pos_x;
        ptcl_data->position.y = 0.5f;
        ptcl_data->position.z = pos_z;
        ptcl_data->direction.x = dir_x;
        ptcl_data->direction.y = 0.0f;
        ptcl_data->direction.z = dir_z;
        ptcl_data->size = size + ptcl_size;
        ptcl_data->density = 1.0f;
    }
}

void TaskEffectFogRing::Data::Reset() {
    struc_573(*v2)[5] = field_5C;
    for (int32_t i = 0; i < 2; i++, v2++) {
        struc_573* v3 = *v2;
        for (int32_t j = 0; j < 5; j++, v3++) {
            v3->bone_index = (rob_bone_index)0;
            v3->position = 0.0f;
        }
    }

    InitParticleData();
    field_8 = true;
}

void TaskEffectFogRing::Data::SetStageIndices(std::vector<int32_t>& stage_indices) {
    rctx_ptr->draw_pass.clear_preprocess(0);
    rctx_ptr->draw_pass.set_enable(DRAW_PASS_PRE_PROCESS, false);
    disp = false;
    current_stage_index = -1;
    this->stage_indices.clear();
    stage_param_data_fog_ring_storage_clear();

    for (int32_t& i : stage_indices) {
        stage_param_fog_ring fog_ring;
        if (task_effect_array_parse_stage_param_data_fog_ring(&fog_ring, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_fog_ring_storage_set_stage_data(i, &fog_ring);
        }
    }

    enable = true;
    num_vtx = 0;
    field_2B8 = 0;
    field_2B9 = 0;
    delta_frame = 1.0f;
    field_8 = true;

    if (!this->stage_indices.size())
        return;

    current_stage_index = stage_indices.front();

    stage_param_fog_ring* fog_ring = stage_param_data_fog_ring_storage_get_value(current_stage_index);
    if (!fog_ring)
        return;

    disp = true;
    enable = true;
    ring_size = fog_ring->ring_size;
    wind_dir = fog_ring->wind_dir;
    tex_id = -1;
    if (fog_ring->tex_name.size()) {
        data_struct* aft_data = &data_list[DATA_AFT];
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        tex_id = aft_tex_db->get_texture_id(fog_ring->tex_name.c_str());
    }

    color = fog_ring->color;
    ptcl_size = fog_ring->ptcl_size;
    max_ptcls = 2000;
    num_ptcls = fog_ring->num_ptcls;
    density = fog_ring->density;
    density_offset = fog_ring->density_offset;

    struc_573(*v2)[5] = field_5C;
    for (int32_t i = 0; i < 2; i++, v2++) {
        struc_573* v3 = *v2;
        v3[0].chara_index = i;
        v3[0].bone_index = ROB_BONE_KL_TOE_R_WJ;
        v3[0].position = 0.0f;
        v3[1].chara_index = i;
        v3[1].bone_index = ROB_BONE_KL_TOE_L_WJ;
        v3[1].position = 0.0f;
        v3[2].chara_index = i;
        v3[2].bone_index = ROB_BONE_KL_TE_L_WJ;
        v3[2].position = 0.0f;
        v3[3].chara_index = i;
        v3[3].bone_index = ROB_BONE_KL_TE_R_WJ;
        v3[3].position = 0.0f;
        v3[4].chara_index = i;
        v3[4].bone_index = ROB_BONE_N_HARA_CP;
        v3[4].position = 0.0f;
    }

    const size_t max_ptcls_vtx_count = max_ptcls * 0x04ULL;

    ptcl_data = force_malloc_s(fog_ring_data, max_ptcls);
    ptcl_data = new (ptcl_data) fog_ring_data[max_ptcls];

    if (!vao)
        glGenVertexArrays(1, &vao);

    if (!vbo)
        glGenBuffers(1, &vbo);

    static const GLsizei buffer_size = sizeof(for_ring_vertex_data);

    gl_state_bind_array_buffer(vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * max_ptcls_vtx_count),
            0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
    else
        glBufferData(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * max_ptcls_vtx_count),
            0, GL_DYNAMIC_DRAW);

    gl_state_bind_vertex_array(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(for_ring_vertex_data, position)); // Pos
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(for_ring_vertex_data, color));    // Color0

    size_t ebo_count = max_ptcls_vtx_count / 4 * 6;
    uint32_t* ebo_data = force_malloc_s(uint32_t, ebo_count);
    for (size_t i = 0, j = 0; i < max_ptcls; i += 6, j += 4) {
        ebo_data[i + 0] = (uint32_t)(j + 0);
        ebo_data[i + 1] = (uint32_t)(j + 1);
        ebo_data[i + 2] = (uint32_t)(j + 2);
        ebo_data[i + 3] = (uint32_t)(j + 0);
        ebo_data[i + 4] = (uint32_t)(j + 2);
        ebo_data[i + 5] = (uint32_t)(j + 3);
    }

    if (!ebo)
        glGenBuffers(1, &ebo);

    gl_state_bind_element_array_buffer(ebo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint32_t) * ebo_count), ebo_data, 0);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint32_t) * ebo_count), ebo_data, GL_STATIC_DRAW);
    free_def(ebo_data);

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);
    gl_state_bind_element_array_buffer(0);

    InitParticleData();

    rctx_ptr->draw_pass.set_enable(DRAW_PASS_PRE_PROCESS, true);
    rctx_ptr->draw_pass.add_preprocess(0, TaskEffectFogRing::Data::DrawStatic, this);
}

void TaskEffectFogRing::Data::DrawStatic(void* data) {
    ((TaskEffectFogRing::Data*)data)->Draw();
}

float_t TaskEffectFogRing::Data::PtclRandom(float_t value) {
    return (float_t)(rand_state_array_get_int(4) % 10000) * 0.0001f * value * 2.0f - value;
}

void TaskEffectFogRing::Data::sub_140347B40(float_t delta_time) {
    field_124 = 0;

    struc_573(*v5)[5] = field_5C;
    for (int32_t i = 0; i < 2; i++, v5++) {
        if (!rob_chara_array_get(i) || !rob_chara_array_check_visibility(i))
            continue;

        rob_chara_bone_data* rob_bone_data = rob_chara_array_get_bone_data(i);
        if (!rob_bone_data)
            continue;

        struc_573* v8 = *v5;
        for (int32_t j = 0; j < 5; j++, v8++) {
            mat4* mat = rob_chara_bone_data_get_mats_mat(rob_bone_data, v8->bone_index);
            if (!mat)
                continue;

            vec3 trans;
            mat4_get_translation(mat, &trans);
            v8->position = trans;

            if (field_124 >= 10 || field_8)
                continue;

            vec3 v23 = (trans - v8->position) * (1.0f / delta_time);
            float_t v15 = vec3::length(v23);

            float_t v18;
            float_t v19;
            if (v8->bone_index) {
                if (v15 < 10.0f)
                    continue;

                if (v15 != 0.0f)
                    v23 *= 1.0f / v15;

                v18 = 0.01f * v15;
                v19 = 0.5f;
            }
            else {
                if (trans.y >= 0.2f || v8->position.y < 0.2f)
                    continue;

                v18 = 2.2f;
                v19 = 2.0f;
                v23 = { 0.0f, 1.0f, 0.0f };
            }

            struc_371& v20 = field_128[field_124++];
            v20.field_0 = 1;
            v20.position = trans;
            v20.field_10 = v19;
            v20.field_14 = v19 * v19;
            v20.direction = v23;
            v20.field_24 = v18;
        }
    }
}

void TaskEffectFogRing::Data::sub_140347860(fog_ring_data* a1, int32_t a2, struc_371* a3, float_t delta_time) {
    if (a2 <= 0)
        return;

    for (int32_t i = a2; i > 0; i--, a3++) {
        vec3 v32 = a1->position - a3->position;
        float_t v9 = a3->field_10 + 1.0f;
        float_t v10 = vec3::length_squared(v32);
        if (v10 > v9 * v9)
            continue;

        vec3 v33 = 0.0f;
        switch (a3->field_0) {
        case 0:
            v33 = a3->direction * a3->field_24;
            break;
        case 1:
            if (v10 < 0.00001f)
                v33 = vec3(1.0f, 0.0f, 1.0f) * a3->field_24;
            else {
                float_t v16 = sqrtf(v10);

                if (v16 != 0.0f)
                    v32 *= 1.0f / v16;

                float_t v17 = 1.0f / v10;
                v17 = min_def(v17, 1.0f);

                v33 = v32 * (v17 * a3->field_24);
            }
            break;
        }
        a1->field_18 = (v33 * delta_time * 60.0f) + a1->field_18;
    }
}

TaskEffectFogRing::TaskEffectFogRing() {

}

TaskEffectFogRing::~TaskEffectFogRing() {

}

bool TaskEffectFogRing::Init() {
    //sub_1400DE640("TaskEffectFogRing::init()\n");
    return true;
}

bool TaskEffectFogRing::Ctrl() {
    data.delta_frame = data.frame_rate_control->GetDeltaFrame();
    data.Ctrl();
    return 0;
}

bool TaskEffectFogRing::Dest() {
    data.Dest();
    task_effect_fog_ring_data = 0;
    //sub_1400DE640("TaskEffectFogRing::dest()\n");
    return 1;
}

void TaskEffectFogRing::Disp() {
    data.Disp();
}

void TaskEffectFogRing::PreInit(int32_t stage_index) {

}

void TaskEffectFogRing::SetStageIndices(std::vector<int32_t>& stage_indices) {
    SetFrameRateControl(0);
    task_effect_fog_ring_data = &data;
    data.SetStageIndices(stage_indices);
}

void TaskEffectFogRing::SetEnable(bool value) {
    data.enable = value;
}

void TaskEffectFogRing::SetCurrentStageIndex(int32_t value) {
    if (data.current_stage_index == value)
        return;

    data.current_stage_index = value;
    if (value != -1) {
        for (int32_t& i : data.stage_indices)
            if (i == value) {
                data.disp = true;
                return;
            }
    }
    data.disp = false;
}

void TaskEffectFogRing::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        data.frame_rate_control = value;
    else
        data.frame_rate_control = &sys_frame_rate;
}

void TaskEffectFogRing::Reset() {
    //sub_140349C30(&this->data);
}

TaskEffectLeaf::TaskEffectLeaf() : frame_rate_control(), wait_frames() {
    current_stage_index = -1;
}

TaskEffectLeaf:: ~TaskEffectLeaf() {

}

bool TaskEffectLeaf::Init() {
    if (stage_param_data_leaf_current) {
        leaf_particle_init();
        wait_frames = 2;
        SetFrameRateControl(0);
    }
    return true;
}

bool TaskEffectLeaf::Ctrl() {
    if (!stage_param_data_leaf_current)
        return false;

    leaf_particle_delta_frame = frame_rate_control->GetDeltaFrame();
    if (wait_frames > 0) {
        wait_frames--;
        return false;
    }
    leaf_particle_ctrl();
    return false;
}

bool TaskEffectLeaf::Dest() {
    leaf_particle_free();
    stage_param_data_leaf_current = 0;
    stage_param_data_leaf_storage_clear();
    return true;
}

void TaskEffectLeaf::Disp() {

}

void TaskEffectLeaf::PreInit(int32_t stage_index) {

}

void TaskEffectLeaf::SetStageIndices(std::vector<int32_t>& stage_indices) {
    if (stage_param_data_leaf_current)
        Dest();

    stage_param_data_leaf_set = 0;
    current_stage_index = -1;
    this->stage_indices.clear();
    stage_param_data_leaf_storage_clear();

    for (int32_t i : stage_indices) {
        stage_param_leaf leaf;
        if (task_effect_array_parse_stage_param_data_leaf(&leaf, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_leaf_storage_set_stage_data(i, &leaf);
        }
    }

    stage_param_data_leaf_current = 0;
    if (!this->stage_indices.size())
        return;

    int32_t stage_index = stage_indices.front();
    current_stage_index = stage_index;
    stage_param_leaf* leaf = stage_param_data_leaf_storage_get_value(stage_index);

    leaf_particle_tex_id = -1;
    if (leaf->tex_name.size()) {
        data_struct* aft_data = &data_list[DATA_AFT];
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        leaf_particle_tex_id = aft_tex_db->get_texture_id(leaf->tex_name.c_str());
    }

    if (leaf) {
        stage_param_data_leaf_current = leaf;
        stage_param_data_leaf_set = true;
    }
}

void TaskEffectLeaf::SetEnable(bool value) {
    leaf_particle_enable = value;
}

void TaskEffectLeaf::SetCurrentStageIndex(int32_t value) {
    if (current_stage_index == value)
        return;

    current_stage_index = value;

    bool found = false;
    for (int32_t i : stage_indices)
        if (i == value) {
            found = true;
            break;
        }

    stage_param_data_leaf_set = found;
    if (found) {
        stage_param_data_leaf_current = stage_param_data_leaf_storage_get_value(value);
        leaf_particle_init(true);
        wait_frames = 2;
    }
}

void TaskEffectLeaf::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = &sys_frame_rate;
}

void TaskEffectLeaf::Reset() {
    if (stage_param_data_leaf_current) {
        leaf_particle_init();
        wait_frames = 2;
    }
}

TaskEffectLitproj::TaskEffectLitproj() : wait_frames(), frame() {
    current_stage_index = -1;
}

TaskEffectLitproj:: ~TaskEffectLitproj() {

}

bool TaskEffectLitproj::Init() {
    if (stage_param_data_litproj_current) {
        wait_frames = 10;
        frame = 0;
    }
    return true;
}

bool TaskEffectLitproj::Ctrl() {
    if (!stage_param_data_litproj_current)
        return false;

    light_data& light = rctx_ptr->light_set[LIGHT_SET_MAIN].lights[LIGHT_PROJECTION];
    if (light.get_type() != LIGHT_SPOT)
        return false;

    if (wait_frames > 0) {
        if (!--wait_frames) {
            light.get_diffuse(diffuse);
            light.get_specular(specular);
        }
        return false;
    }

    if (--frame < 0)
        frame = 255;

    float_t v8 = (float_t)((float_t)frame * M_PI * (1.0 / 128.0));
    float_t v11 = (sinf(sinf(v8 * 27.0f) + v8 * 2.0f) + sinf(v8 * 23.0f)) * 0.5f;
    light.set_diffuse({ diffuse.x, diffuse.y * (v11 * 0.1f + 1.0f), diffuse.z, diffuse.w });
    light.set_specular({ specular.x, specular.y * (v11 * 0.15f + 1.0f), specular.z, specular.w });
    return false;
}

bool TaskEffectLitproj::Dest() {
    litproj_textures_free();
    stage_param_data_litproj_current = 0;
    stage_param_data_litproj_storage_clear();
    return true;
}

void TaskEffectLitproj::Disp() {

}

void TaskEffectLitproj::PreInit(int32_t stage_index) {

}

void TaskEffectLitproj::SetStageIndices(std::vector<int32_t>& stage_indices) {
    if (stage_param_data_litproj_current)
        Dest();

    stage_param_data_litproj_set = 0;
    current_stage_index = -1;
    this->stage_indices.clear();
    stage_param_data_litproj_storage_clear();

    for (int32_t i : stage_indices) {
        stage_param_litproj litproj;
        if (task_effect_array_parse_stage_param_data_litproj(&litproj, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_litproj_storage_set_stage_data(i, &litproj);
        }
    }

    stage_param_data_litproj_current = 0;
    if (stage_indices.size()) {
        int32_t stage_index = stage_indices.front();
        current_stage_index = stage_index;
        stage_param_litproj* litproj = stage_param_data_litproj_storage_get_value(stage_index);
        if (litproj) {
            stage_param_data_litproj_current = litproj;
            litproj_textures_init();
            stage_param_data_litproj_set = true;
        }
    }
}

void TaskEffectLitproj::SetEnable(bool value) {
    light_proj_enable = value;
}

void TaskEffectLitproj::SetCurrentStageIndex(int32_t value) {
    if (current_stage_index == value)
        return;

    current_stage_index = value;

    bool found = false;
    for (int32_t i : stage_indices)
        if (i == value) {
            found = true;
            break;
        }

    stage_param_data_litproj_set = found;
    if (found)
        stage_param_data_litproj_current = stage_param_data_litproj_storage_get_value(value);
}

void TaskEffectLitproj::Reset() {
    wait_frames = 10;
    frame = 0;
}

TaskEffectParticle::TaskEffectParticle() : frame_rate_control() {
    current_stage_index = -1;
}

TaskEffectParticle::~TaskEffectParticle() {

}

bool TaskEffectParticle::Init() {
    SetFrameRateControl(0);
    return true;
}

bool TaskEffectParticle::Ctrl() {
    particle_delta_time = frame_rate_control->GetDeltaFrame() * (float_t)(1.0 / 60.0);
    if (particle_count > 0)
        particle_ctrl();
    return false;
}

bool TaskEffectParticle::Dest() {
    particle_free();
    return true;
}

void TaskEffectParticle::Disp() {

}

void TaskEffectParticle::PreInit(int32_t stage_index) {
    current_stage_index = stage_index;
    //field_74 = 0;
    particle_init(0);
}

void TaskEffectParticle::SetEnable(bool value) {
    particle_enable = value;
}

void TaskEffectParticle::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = &sys_frame_rate;
}

void TaskEffectParticle::Reset() {
    if (ptcl_data)
        particle_init(0);
}

void TaskEffectParticle::Event(int32_t event_type, void* data) {
    if (event_type == 1)
        particle_event((particle_event_data*)data);
}

TaskEffectRain::TaskEffectRain() : frame_rate_control() {
    current_stage_index = -1;
}

TaskEffectRain:: ~TaskEffectRain() {

}

bool TaskEffectRain::Init() {
    if (stage_param_data_rain_current) {
        rain_particle_init(0);
        SetFrameRateControl(0);
    }
    return true;
}

bool TaskEffectRain::Ctrl() {
    if (stage_param_data_rain_current) {
        rain_particle_delta_frame = frame_rate_control->GetDeltaFrame();
        rain_particle_ctrl();
    }
    return false;
}

bool TaskEffectRain::Dest() {
    if (stage_param_data_rain_current) {
        rain_particle_free();
        rain_ptcl_vbo = 0;
        stage_param_data_rain_current = 0;
        stage_param_data_rain_storage_clear();
        stage_param_data_rain_set = false;
        current_stage_index = -1;
        stage_indices.clear();
    }
    return true;
}

void TaskEffectRain::Disp() {

}

void TaskEffectRain::PreInit(int32_t stage_index) {

}

void TaskEffectRain::SetStageIndices(std::vector<int32_t>& stage_indices) {
    if (stage_param_data_rain_current)
        Dest();

    stage_param_data_rain_set = false;
    current_stage_index = -1;
    this->stage_indices.clear();
    stage_param_data_rain_storage_clear();

    for (int32_t i : stage_indices) {
        stage_param_rain rain;
        if (task_effect_array_parse_stage_param_data_rain(&rain, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_rain_storage_set_stage_data(i, &rain);
        }
    }

    stage_param_data_rain_current = 0;
    if (!this->stage_indices.size())
        return;

    int32_t stage_index = stage_indices.front();
    current_stage_index = stage_index;
    stage_param_rain* rain = stage_param_data_rain_storage_get_value(stage_index);

    rain_particle_tex_id = -1;
    if (rain->tex_name.size()) {
        data_struct* aft_data = &data_list[DATA_AFT];
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        rain_particle_tex_id = aft_tex_db->get_texture_id(rain->tex_name.c_str());
    }

    if (rain) {
        stage_param_data_rain_current = rain;
        stage_param_data_rain_set = true;
    }
}

void TaskEffectRain::SetEnable(bool value) {
    rain_particle_enable = value;
}

void TaskEffectRain::SetCurrentStageIndex(int32_t value) {
    if (current_stage_index == value)
        return;

    current_stage_index = value;

    bool found = false;
    for (int32_t i : stage_indices)
        if (i == value) {
            found = true;
            break;
        }

    stage_param_data_rain_set = found;
    if (found)
        stage_param_data_rain_current = stage_param_data_rain_storage_get_value(value);
}

void TaskEffectRain::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = &sys_frame_rate;
}

void TaskEffectRain::Reset() {
    if (stage_param_data_rain_current) {
        rain_particle_init(0);
        SetFrameRateControl(0);
    }
}

TaskEffectRipple::TaskEffectRipple() : field_68(), frame_rate_control(), emit() {

}

TaskEffectRipple::~TaskEffectRipple() {

}

bool TaskEffectRipple::Init() {
    //sub_1400DE640("TaskEffectRipple::init()\n");
    return true;
}

bool TaskEffectRipple::Ctrl() {
    emit->delta_frame = frame_rate_control->GetDeltaFrame();
    //emit->ctrl();
    return false;
}

bool TaskEffectRipple::Dest() {
    //emit->dest();
    //sub_1400DE640("TaskEffectRipple::dest()\n");
    return true;
}

void TaskEffectRipple::Disp() {
    //emit->disp();
}

void TaskEffectRipple::PreInit(int32_t stage_index) {

}

void TaskEffectRipple::SetStageIndices(std::vector<int32_t>& stage_indices) {
    SetFrameRateControl(0);
    //emit = &ripple_emit_data;
    //emit->set_stage_indices(stage_indices);
}

void TaskEffectRipple::SetCurrentStageIndex(int32_t value) {
    //emit->set_stage_index(value);
}

void TaskEffectRipple::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = &sys_frame_rate;
}

void TaskEffectRipple::Reset() {
    //if (!emit->use_float_ripplemap)
    //    ripple_emit_data.clear_tex();
    //ripple_emit_data.reset();
}

void leaf_particle_draw() {
    if (!stage_param_data_leaf_current || !leaf_ptcl_data
        || !leaf_particle_enable || !stage_param_data_leaf_set)
        return;

    texture* tex = texture_storage_get_texture(leaf_particle_tex_id);
    if (!tex)
        return;

    int32_t count = leaf_particle_disp();
    if (!count)
        return;

    gl_state_active_bind_texture_2d(0, tex->tex);
    shaders_ft.set(SHADER_FT_LEAF_PT);
    shaders_ft.local_frag_set(0, stage_param_data_leaf_current->color);
    shaders_ft.state_material_set_shininess(false, 1.0f);
    gl_state_bind_vertex_array(leaf_ptcl_vao);
    shaders_ft.draw_elements(GL_TRIANGLES, count / 4 * 6, GL_UNSIGNED_INT, 0);
    gl_state_bind_vertex_array(0);
}

void rain_particle_draw() {
    if (!stage_param_data_rain_current || !rain_particle_enable || !stage_param_data_rain_set)
        return;

    texture* tex = texture_storage_get_texture(rain_particle_tex_id);
    if (!tex)
        return;

    gl_state_enable_blend();
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_active_bind_texture_2d(0, tex->tex);
    shaders_ft.set(SHADER_FT_RAIN);

    stage_param_rain* rain = stage_param_data_rain_current;
    float_t tangent_sign = -rain->psize.x;
    float_t tangent_size = -rain->psize.y * (float_t)(1.0 / 30.0);

    int32_t first = 0;
    int32_t count = min_def(rain->num_rain, rain_ptcl_count) / 2;

    vec3 range = rain->range;
    vec3 range_scale = rain->range;
    vec3 range_offset = rain->offset;
    range_offset.x -= range.x * 0.5f;
    range_offset.z -= range.z * 0.5f;
    vec4 color = rain->color;
    float_t color_a = color.w;
    shaders_ft.local_vert_set(1, range_offset.x, range_offset.y, range_offset.z, 0.0f);
    shaders_ft.local_vert_set(2, range_scale.x, range_scale.y, range_scale.z, 0.0f);
    gl_state_bind_vertex_array(rain_ptcl_vao);
    for (int32_t i = 0; i < 8; i++, first += count) {
        rain_particle_data& data = rain_ptcl_data[i];
        vec3 pos_offset = data.position / range;
        vec3 tangent = data.velocity * tangent_size;
        color.w = color_a * data.alpha;
        shaders_ft.local_vert_set(0, pos_offset.x, pos_offset.y, pos_offset.z, 0.075f);
        shaders_ft.local_vert_set(3, tangent.x, tangent.y, tangent.z, tangent_sign);
        shaders_ft.local_frag_set(0, color);
        shaders_ft.draw_range_elements(GL_TRIANGLES, first, count, count / 4 * 6, GL_UNSIGNED_INT, 0);
    }
    gl_state_bind_vertex_array(0);
    gl_state_active_bind_texture_2d(0, 0);
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_disable_blend();
}

void particle_draw() {
    if (!ptcl_data)
        return;

    particle_vertex_data* vtx_data;
    if (GLAD_GL_VERSION_4_5) {
        vtx_data = (particle_vertex_data*)glMapNamedBuffer(ptcl_vbo, GL_WRITE_ONLY);
        if (!vtx_data) {
            glUnmapNamedBuffer(ptcl_vbo);
            return;
        }
    }
    else {
        gl_state_bind_array_buffer(ptcl_vbo);
        vtx_data = (particle_vertex_data*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (!vtx_data) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            gl_state_bind_array_buffer(0);
            return;
        }
    }

    int32_t count = particle_disp(vtx_data, ptcl_data, ptcl_count);
    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(ptcl_vbo);
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    if (!count)
        return;

    gl_state_bind_vertex_array(ptcl_vao);
    shaders_ft.set(SHADER_FT_PARTICL);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, count);
    gl_state_bind_vertex_array(0);
}

void snow_particle_draw() {

}

void task_effect_init() {
    if (!task_effect_auth_3d)
        task_effect_auth_3d = new TaskEffectAuth3D;

    if (!task_effect_leaf)
        task_effect_leaf = new TaskEffectLeaf;

    /*if (!task_effect_snow)
        task_effect_snow = new TaskEffectSnow;*/

    if (!task_effect_ripple)
        task_effect_ripple = new TaskEffectRipple;

    if (!task_effect_rain)
        task_effect_rain = new TaskEffectRain;

    /*if (!task_effect_splash)
        task_effect_splash = new TaskEffectSplash;*/

    if (!task_effect_fog_anim)
        task_effect_fog_anim = new TaskEffectFogAnim;

    if (!task_effect_fog_ring)
        task_effect_fog_ring = new TaskEffectFogRing;

    if (!task_effect_particle)
        task_effect_particle = new TaskEffectParticle;

    if (!task_effect_litproj)
        task_effect_litproj = new TaskEffectLitproj;

    /*if (!task_effect_star)
        task_effect_star = new TaskEffectStar;*/

    if (!task_effect_parent)
        task_effect_parent = new TaskEffectParent;

    task_effect_fog_anim_data = 0;
    task_effect_fog_ring_data = 0;
    task_effect_splash_data = 0;
}

void task_effect_free() {
    if (task_effect_auth_3d) {
        delete task_effect_auth_3d;
        task_effect_auth_3d = 0;
    }

    if (task_effect_leaf) {
        delete task_effect_leaf;
        task_effect_leaf = 0;
    }

    /*if (task_effect_snow) {
        delete task_effect_snow;
        task_effect_snow = 0;
    }*/

    if (task_effect_ripple) {
        delete task_effect_ripple;
        task_effect_ripple = 0;
    }

    if (task_effect_rain) {
        delete task_effect_rain;
        task_effect_rain = 0;
    }

    /*if (task_effect_splash) {
        delete task_effect_splash;
        task_effect_splash = 0;
    }*/

    if (task_effect_fog_anim) {
        delete task_effect_fog_anim;
        task_effect_fog_anim = 0;
    }

    if (task_effect_fog_ring) {
        delete task_effect_fog_ring;
        task_effect_fog_ring = 0;
    }

    if (task_effect_particle) {
        delete task_effect_particle;
        task_effect_particle = 0;
    }

    if (task_effect_litproj) {
        delete task_effect_litproj;
        task_effect_litproj = 0;
    }

    /*if (task_effect_star) {
        delete task_effect_star;
        task_effect_star = 0;
    }*/

    if (task_effect_parent) {
        delete task_effect_parent;
        task_effect_parent = 0;
    }

    task_effect_fog_anim_data = 0;
    task_effect_fog_ring_data = 0;
    task_effect_splash_data = 0;
}

void task_effect_parent_event(TaskEffectType type, int32_t event_type, void* data) {
    task_effect_parent->Event(type, event_type, data);
}

void task_effect_parent_dest() {
    task_effect_parent->Dest();
}

bool task_effect_parent_load() {
    return task_effect_parent->Load();
}

void task_effect_parent_reset() {
    task_effect_parent->Reset();
}

void task_effect_parent_set_current_stage_hash(uint32_t stage_hash) {
    task_effect_parent->SetCurrentStageHash(stage_hash);
}

void task_effect_parent_set_current_stage_index(int32_t stage_index) {
    task_effect_parent->SetCurrentStageIndex(stage_index);
}

void task_effect_parent_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data) {
    task_effect_parent->data = data;
    task_effect_parent->obj_db = obj_db;
    task_effect_parent->tex_db = tex_db;
    task_effect_parent->stage_data = stage_data;
}

void task_effect_parent_set_enable(bool value) {
    task_effect_parent->SetEnable(value);
}

void task_effect_parent_set_frame(int32_t value) {
    task_effect_parent->SetFrame(value);
}

void task_effect_parent_set_frame_rate_control(FrameRateControl* value) {
    task_effect_parent->SetFrameRateControl(value);
}

void task_effect_parent_set_stage_hashes(std::vector<uint32_t>& stage_hashes) {
    task_effect_parent->SetStageHashes(stage_hashes);
}

void task_effect_parent_set_stage_indices(std::vector<int32_t>&stage_indices) {
    task_effect_parent->SetStageIndices(stage_indices);
}

bool task_effect_parent_unload() {
    return task_effect_parent->Unload();
}

static TaskEffect* task_effect_array_get(TaskEffectType type, const char** name) {
    if (type < TASK_EFFECT_AUTH_3D || type > TASK_EFFECT_STAR) {
        if (name)
            *name = 0;
        return 0;
    }

    if (name)
        *name = task_effect_name_array[type];

    TaskEffect** task = task_effect_data_array[type];
    if (task)
        return *task;
    return 0;
}

static std::string task_effect_array_get_stage_param_file_path(
    TaskEffectType type, int32_t stage_index, bool dev_ram, bool a4) {
    std::string path;
    if (dev_ram)
        path.assign("dev_ram/stage_param/");
    else
        path.assign("./rom/stage_param/");

    if (a4)
        return path;

    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    const char* stage_name = aft_stage_data->get_stage_name(stage_index);
    if (!stage_name)
        return {};

    const char* effect_name = 0;
    task_effect_array_get(type, &effect_name);
    if (!effect_name)
        return {};

    size_t path_len = path.size();
    path.append(effect_name + 7);
    path.append("_");
    path.append(stage_name + 3);
    size_t path_len_new = path.size();
    for (size_t i = path_len; i < path_len_new; i++) {
        char c = path[i];
        if (c >= 'A' && c <= 'Z')
            path[i] = c + 0x20;
    }
    path.append(".txt");
    return path;
}

static bool task_effect_array_parse_stage_param_data_fog_ring(stage_param_fog_ring* fog_ring, int32_t stage_index) {
    if (!fog_ring)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_FOG_RING, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(fog_ring, path.c_str(), stage_param_fog_ring::load_file);
    return fog_ring->ready;
}

static bool task_effect_array_parse_stage_param_data_leaf(stage_param_leaf* leaf, int32_t stage_index) {
    if (!leaf)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_LEAF, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(leaf, path.c_str(), stage_param_leaf::load_file);
    return leaf->ready;
}

static bool task_effect_array_parse_stage_param_data_litproj(stage_param_litproj* litproj, int32_t stage_index) {
    if (!litproj)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_LITPROJ, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(litproj, path.c_str(), stage_param_litproj::load_file);
    return litproj->ready;
}

static bool task_effect_array_parse_stage_param_data_rain(stage_param_rain* rain, int32_t stage_index) {
    if (!rain)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_RAIN, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(rain, path.c_str(), stage_param_rain::load_file);
    return rain->ready;
}

static bool task_effect_array_parse_stage_param_data_ripple(stage_param_ripple* ripple, int32_t stage_index) {
    if (!ripple)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_RIPPLE, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(ripple, path.c_str(), stage_param_ripple::load_file);
    return ripple->ready;
}

static bool task_effect_array_parse_stage_param_data_snow(stage_param_snow* snow, int32_t stage_index) {
    if (!snow)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_SNOW, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(snow, path.c_str(), stage_param_snow::load_file);
    return snow->ready;
}

static bool task_effect_array_parse_stage_param_data_splash(stage_param_splash* splash, int32_t stage_index) {
    if (!splash)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_SPLASH, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(splash, path.c_str(), stage_param_splash::load_file);
    return splash->ready;
}

static bool task_effect_array_parse_stage_param_data_star(stage_param_star* star, int32_t stage_index) {
    if (!star)
        return false;

    std::string path = task_effect_array_get_stage_param_file_path(TASK_EFFECT_STAR, stage_index, 0, 0);
    data_list[DATA_AFT].load_file(star, path.c_str(), stage_param_star::load_file);
    return star->ready;
}

void leaf_particle_data::init() {
    stage_param_leaf* leaf = stage_param_data_leaf_current;

    position.x = rand_state_array_get_float(4);
    position.y = rand_state_array_get_float(4);
    position.z = rand_state_array_get_float(4);
    position = (position * vec3(2.0f, 1.0f, 2.0f) - vec3(1.0f, 0.0f, 1.0f)) * leaf->range + leaf->offset;
    direction = vec3(0.0f, -1.0f, 0.0f);
    normal = vec3(0.0f, 0.0f, 1.0f);
    rotation.x = rand_state_array_get_float(4) * 6.28f;
    rotation.y = rand_state_array_get_float(4) * 6.28f;
    rotation.z = 0.0f;
    rotation_add.x = (rand_state_array_get_float(4) + 1.0f) * 3.0f;
    rotation_add.y = (rand_state_array_get_float(4) + 1.0f) * 3.0f;
    rotation_add.z = 10.0f;
    type = 1;
    size = leaf->psize;
}

rain_particle_data::rain_particle_data() : alpha(), type(), field_1C(),
field_2C(), field_30(), field_34(), field_38(), field_3C() {

}

struc_608::struc_608() : stage_effects(), stage_effects_modern() {

}

struc_608::struc_608(const ::stage_effects* stage_effects) : stage_effects_modern() {
    this->stage_effects = stage_effects;
}

struc_608::struc_608(const ::stage_effects_modern* stage_effects_modern) : stage_effects() {
    this->stage_effects_modern = stage_effects_modern;
}

struc_608::~struc_608() {

}

TaskEffectParent::TaskEffectParent() : state(), enable(),
modern(), data(), obj_db(), tex_db(), stage_data() {
    current_stage_hash = hash_murmurhash_empty;
    current_stage_index = -1;
}

TaskEffectParent::~TaskEffectParent() {

}

std::pair<TaskEffectType, TaskEffect*> TaskEffectParent::AddTaskEffectTypePtr(TaskEffectType type) {
    if (CheckTaskEffectTypeLoaded(type) < 0) {
        const char* name = 0;
        TaskEffect* t = task_effect_array_get(type, &name);
        if (t) {
            app::TaskWork::AppendTask(t, name);
            return { type, t };
        }
    }

    return { TASK_EFFECT_INVALID, 0 };
}

int32_t TaskEffectParent::CheckTaskEffectTypeLoaded(TaskEffectType type) {
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        if (i.first == type)
            return 0;
    return -1;
}

void TaskEffectParent::Event(TaskEffectType type, int32_t event_type, void* data) {
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        if (i.first == type)
            i.second->Event(event_type, data);
}

void TaskEffectParent::Dest() {
    if (state == 1)
        state = 4;
    else if (state >= 2 && state <= 3) {
        for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
            i.second->SetDest();
        state = 4;
    }
}

static const TaskEffectType dword_1409E3440[16] = {
    TASK_EFFECT_AUTH_3D,
    TASK_EFFECT_PARTICLE,
    TASK_EFFECT_INVALID,
};

bool TaskEffectParent::Load() {
    if (state == 1) {
        if (!modern) {
            bool wait_load = false;
            for (uint32_t i : obj_set_ids)
                if (object_storage_load_obj_set_check_not_read(i))
                    wait_load |= true;

            if (wait_load)
                return true;

            for (TaskEffectType i : dword_1409E3440) {
                if (i == TASK_EFFECT_INVALID)
                    break;

                std::pair<TaskEffectType, TaskEffect*> v57 = AddTaskEffectTypePtr(i);
                if (v57.second) {
                    v57.second->SetStageIndices(stage_indices);
                    v57.second->SetEnable(enable);
                    effects.push_back(v57);
                    field_50.push_back(v57);
                }
            }

            for (std::pair<int32_t, struc_608>& i : field_68) {
                const stage_effects* stage_effects = i.second.stage_effects;
                for (int32_t j : stage_effects->field_20) {
                    if (j == TASK_EFFECT_INVALID)
                        break;

                    std::pair<TaskEffectType, TaskEffect*> v57 = AddTaskEffectTypePtr((TaskEffectType)j);
                    if (v57.second) {
                        v57.second->SetStageIndices(stage_indices);
                        v57.second->SetEnable(enable);
                        effects.push_back(v57);
                        i.second.field_10.push_back(v57);
                    }
                }
            }
        }
        else {
            void* data = this->data;
            object_database* obj_db = this->obj_db;
            texture_database* tex_db = this->tex_db;
            stage_database* stage_data = this->stage_data;

            bool wait_load = false;
            for (uint32_t i : obj_set_ids)
                if (object_storage_load_obj_set_check_not_read(i, obj_db, tex_db))
                    wait_load = true;

            if (wait_load)
                return true;

            for (TaskEffectType i : dword_1409E3440) {
                if (i == TASK_EFFECT_INVALID)
                    break;

                std::pair<TaskEffectType, TaskEffect*> v57 = AddTaskEffectTypePtr(i);
                if (v57.second) {
                    v57.second->SetStageHashes(stage_hashes, data, obj_db, tex_db, stage_data);
                    v57.second->SetEnable(enable);
                    effects.push_back(v57);
                    field_50.push_back(v57);
                }
            }

            for (std::pair<int32_t, struc_608>& i : field_68) {
                const stage_effects_modern* stage_effects = i.second.stage_effects_modern;
                for (int32_t j : stage_effects->field_20) {
                    if (j == TASK_EFFECT_INVALID)
                        break;

                    std::pair<TaskEffectType, TaskEffect*> v57 = AddTaskEffectTypePtr((TaskEffectType)j);
                    if (v57.second) {
                        v57.second->SetStageHashes(stage_hashes, data, obj_db, tex_db, stage_data);
                        v57.second->SetEnable(enable);
                        effects.push_back(v57);
                        i.second.field_10.push_back(v57);
                    }
                }
            }
        }
        state = 2;
        return true;
    }
    else if (state != 2)
        return false;

    bool wait_load = false;
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        if (!app::TaskWork::CheckTaskCtrl(i.second)) {
            wait_load = true;
            break;
        }

    if (!wait_load) {
        for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
            i.second->SetEnable(enable);

        state = 3;
        return false;
    }
    return true;
}

void TaskEffectParent::Reset() {
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        i.second->Reset();
}

void TaskEffectParent::ResetData() {
    current_stage_hash = hash_murmurhash_empty;
    current_stage_index = -1;
    this->current_stage_index = -1;
    stage_hashes.clear();
    stage_indices.clear();
    obj_set_ids.clear();
    effects.clear();
    field_68.clear();
    enable = true;
    state = 0;
}

void TaskEffectParent::SetCurrentStageHash(uint32_t stage_hash) {
    if (current_stage_hash != stage_hash) {
        for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
            i.second->SetCurrentStageHash(stage_hash);
        current_stage_hash = stage_hash;
    }
}

void TaskEffectParent::SetCurrentStageIndex(int32_t stage_index) {
    if (current_stage_index != stage_index) {
        for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
            i.second->SetCurrentStageIndex(stage_index);
        current_stage_index = stage_index;
    }
}

void TaskEffectParent::SetEnable(bool value) {
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        i.second->SetEnable(value);
}

void TaskEffectParent::SetFrame(int32_t value) {
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        i.second->SetFrame(value);
}

void TaskEffectParent::SetFrameRateControl(FrameRateControl* value) {
    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        i.second->SetFrameRateControl(value);
}

void TaskEffectParent::SetStageHashes(std::vector<uint32_t>& stage_hashes) {
    this->stage_indices.clear();
    this->stage_hashes.assign(stage_hashes.begin(), stage_hashes.end());
    obj_set_ids.clear();
    modern = true;

    for (uint32_t i : stage_hashes) {
        ::stage_data_modern* stage_data = this->stage_data->get_stage_data_modern(i);
        if (!stage_data)
            continue;

        const stage_effects_modern* effects = stage_data->effects_init
            ? &stage_data->effects : &stage_effects_modern_default;

        field_68.push_back({ i, struc_608(effects) });

        for (uint32_t j : effects->field_0) {
            if (j == hash_murmurhash_empty || j == hash_murmurhash_null || j == -1)
                break;

            bool found = false;
            for (uint32_t k : obj_set_ids)
                if (k == j) {
                    found = true;
                    break;
                }

            if (!found)
                obj_set_ids.push_back(j);
        }
    }

    for (uint32_t i : obj_set_ids)
        object_storage_load_set_hash(data, i);
    state = 1;
}

void TaskEffectParent::SetStageIndices(std::vector<int32_t>& stage_indices) {
    this->stage_indices.assign(stage_indices.begin(), stage_indices.end());
    this->stage_hashes.clear();
    obj_set_ids.clear();
    modern = false;

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    for (int32_t i : stage_indices) {
        ::stage_data* stage_data = aft_stage_data->get_stage_data(i);
        if (!stage_data)
            continue;

        const stage_effects* effects = stage_data->effects_init
            ? &stage_data->effects : &stage_effects_default;

        field_68.push_back({ i, struc_608(effects) });

        for (int32_t j : effects->field_0) {
            if (j == -1)
                break;

            bool found = false;
            for (uint32_t k : obj_set_ids)
                if (k == j) {
                    found = true;
                    break;
                }

            if (!found)
                obj_set_ids.push_back(j);
        }
    }

    for (uint32_t i : obj_set_ids)
        object_storage_load_set(aft_data, aft_obj_db, i);
    state = 1;
}

bool TaskEffectParent::Unload() {
    if (state != 4) {
        ResetData();
        return false;
    }

    for (std::pair<TaskEffectType, TaskEffect*>& i : effects)
        if (app::TaskWork::CheckTaskReady(i.second))
            return true;

    for (uint32_t& i : obj_set_ids)
        object_storage_unload_set(i);

    ResetData();
    return false;
}

static void draw_fog_particle(render_context* rctx, TaskEffectFogRing::Data* data) {
    if (!data->num_vtx)
        return;

    shaders_ft.set(SHADER_FT_FOGPTCL);
    gl_state_enable_blend();
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    texture* tex = texture_storage_get_texture(data->tex_id);
    if (tex)
        gl_state_active_bind_texture_2d(0, tex->tex);
    gl_state_bind_vertex_array(data->vao);
    shaders_ft.draw_elements(GL_TRIANGLES, data->num_vtx / 4 * 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_PROGRAM_POINT_SIZE);
    gl_state_disable_blend();
}

static void leaf_particle_init(bool change_stage) {
    leaf_particle_emit_timer = 0.0f;
    leaf_particle_emit_interval = stage_param_data_leaf_current->emit_interval;
    if (change_stage)
        return;

    leaf_particle_free();

    const size_t leaf_ptcl_vtx_count = leaf_ptcl_count * 0x08;

    leaf_ptcl_data = force_malloc_s(leaf_particle_data, leaf_ptcl_count);

    leaf_particle_num_ptcls = stage_param_data_leaf_current->num_initial_ptcls;

    leaf_particle_data* data = leaf_ptcl_data;
    int32_t i = 0;
    for (; i < leaf_particle_num_ptcls; i++, data++)
        data->init();

    for (; i < leaf_ptcl_count; i++, data++)
        data->type = 0;

    if (!leaf_ptcl_vao)
        glGenVertexArrays(1, &leaf_ptcl_vao);

    if (!leaf_ptcl_vbo)
        glGenBuffers(1, &leaf_ptcl_vbo);

    static const GLsizei buffer_size = sizeof(leaf_particle_vertex_data);

    gl_state_bind_array_buffer(leaf_ptcl_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * leaf_ptcl_vtx_count),
            0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
    else
        glBufferData(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * leaf_ptcl_vtx_count),
            0, GL_DYNAMIC_DRAW);

    gl_state_bind_vertex_array(leaf_ptcl_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(leaf_particle_vertex_data, position)); // Pos
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(leaf_particle_vertex_data, normal));   // Normal
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(leaf_particle_vertex_data, texcoord)); // TexCoords

    size_t ebo_count = leaf_ptcl_vtx_count / 4 * 6;
    uint32_t* ebo_data = force_malloc_s(uint32_t, ebo_count);
    for (size_t i = 0, j = 0; i < ebo_count; i += 6, j += 4) {
        ebo_data[i + 0] = (uint32_t)(j + 0);
        ebo_data[i + 1] = (uint32_t)(j + 1);
        ebo_data[i + 2] = (uint32_t)(j + 2);
        ebo_data[i + 3] = (uint32_t)(j + 0);
        ebo_data[i + 4] = (uint32_t)(j + 2);
        ebo_data[i + 5] = (uint32_t)(j + 3);
    }

    if (!leaf_ptcl_ebo)
        glGenBuffers(1, &leaf_ptcl_ebo);

    gl_state_bind_element_array_buffer(leaf_ptcl_ebo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint32_t) * ebo_count), ebo_data, 0);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint32_t) * ebo_count), ebo_data, GL_STATIC_DRAW);
    free_def(ebo_data);

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);
    gl_state_bind_element_array_buffer(0);
}

static float_t sub_14034C960(vec3 x) {
    float_t t1 = vec3::length_squared(x);
    if (t1 <= 1.0e-30f)
        return 0.0f;
    float_t t2 = 1.0f / sqrtf(t1);
    return (t2 * 1.5f + t1 * t2 * t2 * t2 * -0.5f) * t1;
}

static void leaf_particle_ctrl() {
    if (!leaf_particle_enable || !stage_param_data_leaf_set || !leaf_ptcl_data)
        return;

    stage_param_leaf* leaf = stage_param_data_leaf_current;

    float_t off_y = leaf->offset.y;
    float_t delta_time = (float_t)(leaf->frame_speed_coef
        * (leaf_particle_delta_frame * (1.0 / 60.0)));

    leaf_particle_data* data = leaf_ptcl_data;
    if (leaf_particle_num_ptcls < 0x800) {
        int32_t v10 = 0;
        leaf_particle_emit_timer += delta_time;
        for (int32_t i = 0; i <= 100 && leaf_particle_emit_timer > leaf_particle_emit_interval; i++) {
            data[leaf_particle_num_ptcls++].init();
            v10++;
            leaf_particle_emit_interval += leaf->emit_interval * leaf->frame_speed_coef;
        }
    }

    vec3 wind = leaf->wind;
    stage_param_data_leaf_lie_plane_xz lie_plane_xz = leaf->lie_plane_xz;

    data = leaf_ptcl_data;
    for (int32_t i = 0; i < leaf_particle_num_ptcls; i++, data++) {
        if (data->type != 1)
            continue;

        vec3 direction = data->direction;
        float_t v19 = sub_14034C960(direction);
        vec3 direction_diff = wind - direction;
        vec3 v25 = vec3::dot(direction_diff * data->normal, vec3(1.0f))
            * data->normal * 35.0f + vec3(0.0f, -9.8f, 0.0f)
            + direction_diff * sub_14034C960(direction_diff) * 5.0f;
        direction += v25 * delta_time;
        data->direction = direction;
        data->position += direction * delta_time;
        data->rotation += data->rotation_add * v19 * delta_time;

        float_t pos_y = data->position.y;
        if (pos_y < off_y)
            data->type = 2;
        else if (pos_y < 0.01f && pos_y > -0.09f
            && (lie_plane_xz.min_x <= data->position.x && data->position.x <= lie_plane_xz.max_x)
            && (lie_plane_xz.min_z <= data->position.z && data->position.z <= lie_plane_xz.max_z)) {
            data->type = 2;
            data->position.y = 0.01f;
            data->rotation.x = (float_t)-M_PI_2;
            data->rotation.y = 0.0f;
        }
    }
}

static std::pair<vec3, float_t> sub_1406427A0(vec3 x, vec3 y) {
    float_t t1 = vec3::length_squared(x);
    if (t1 <= 1.0e-30f)
        return { y, 0.0f };

    float_t t2 = 1.0f / sqrtf(t1);
    float_t t3 = (t2 * 1.5f + t1 * t2 * t2 * t2 * -0.5f);
    return { t3 * x, t3 * t1 };
}

static int32_t leaf_particle_disp() {
    float_t size = leaf_ptcl_data[0].size;

    vec3 position[4];
    vec3 normal[4];
    position[0] = vec3(-0.5f, -0.5f, 0.0f) * size;
    position[1] = vec3( 0.5f, -0.5f, 0.2f) * size;
    position[2] = vec3( 0.5f,  0.5f, 0.0f) * size;
    position[3] = vec3(-0.5f,  0.5f, 0.2f) * size;
    normal[0] = sub_1406427A0(vec3( 0.0f,  0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)).first;
    normal[1] = sub_1406427A0(vec3(-0.4f,  0.4f, 1.0f), vec3(0.0f, 1.0f, 0.0f)).first;
    normal[2] = sub_1406427A0(vec3(-0.2f, -0.2f, 1.0f), vec3(0.0f, 1.0f, 0.0f)).first;
    normal[3] = sub_1406427A0(vec3( 0.4f, -0.4f, 1.0f), vec3(0.0f, 1.0f, 0.0f)).first;

    leaf_particle_vertex_data* vtx_data;
    if (GLAD_GL_VERSION_4_5) {
        vtx_data = (leaf_particle_vertex_data*)glMapNamedBuffer(leaf_ptcl_vbo, GL_WRITE_ONLY);
        if (!vtx_data) {
            glUnmapNamedBuffer(leaf_ptcl_vbo);
            return 0;
        }
    }
    else {
        gl_state_bind_array_buffer(leaf_ptcl_vbo);
        vtx_data = (leaf_particle_vertex_data*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (!vtx_data) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            gl_state_bind_array_buffer(0);
            return 0;
        }
    }

    int32_t vtx_count = 0;
    leaf_particle_data* data = leaf_ptcl_data;
    bool split_tex = stage_param_data_leaf_current->split_tex;
    for (int32_t i = 0; i < leaf_particle_num_ptcls; i++, data++) {
        if (!data->type)
            continue;

        vec3 pos = data->position;
        vec3 rot = data->rotation;

        mat3 mat;
        mat3_rotate_x(rot.x, &mat);
        mat3_rotate_y_mult(&mat, rot.y, &mat);
        mat3_rotate_z_mult(&mat, rot.z, &mat);

        vec3 t0;
        vec3 t1;
        vec3 t2;
        vec3 t3;
        mat3_mult_vec(&mat, &position[0], &t0);
        mat3_mult_vec(&mat, &position[1], &t1);
        mat3_mult_vec(&mat, &position[2], &t2);
        mat3_mult_vec(&mat, &position[3], &t3);
        vtx_data[0].position = pos + t0;
        vtx_data[1].position = pos + t1;
        vtx_data[2].position = pos + t2;
        vtx_data[3].position = pos + t3;

        mat3_mult_vec(&mat, &normal[0], &t0);
        mat3_mult_vec(&mat, &normal[1], &t1);
        mat3_mult_vec(&mat, &normal[2], &t2);
        mat3_mult_vec(&mat, &normal[3], &t3);
        vtx_data[0].normal = t0;
        vtx_data[1].normal = t1;
        vtx_data[2].normal = t2;
        vtx_data[3].normal = t3;
        data[0].normal = t0;

        if (split_tex) {
            float_t u = 0.0f;
            float_t v = 0.0f;
            if (i & 0x01)
                u = 1.0f;
            if (i & 0x02)
                v = 1.0f;

            float_t u0 = u * 0.5f;
            float_t v0 = v * 0.5f;
            float_t u1 = (u + 1.0f) * 0.5f;
            float_t v1 = (v + 1.0f) * 0.5f;

            vtx_data[0].texcoord.x = u0;
            vtx_data[0].texcoord.y = v0;
            vtx_data[1].texcoord.x = u1;
            vtx_data[1].texcoord.y = v0;
            vtx_data[2].texcoord.x = u1;
            vtx_data[2].texcoord.y = v1;
            vtx_data[3].texcoord.x = u0;
            vtx_data[3].texcoord.y = v1;
        }
        else {
            vtx_data[0].texcoord = 0.0f;
            vtx_data[1].texcoord = 1.0f;
            vtx_data[2].texcoord = 1.0f;
            vtx_data[3].texcoord = vec2(0.0f, 1.0f);
        }

        if (data->type == 1) {
            vtx_data[4] = vtx_data[0];
            vtx_data[5] = vtx_data[3];
            vtx_data[6] = vtx_data[2];
            vtx_data[7] = vtx_data[1];
            vtx_data[4].normal = -vtx_data[4].normal;
            vtx_data[5].normal = -vtx_data[5].normal;
            vtx_data[6].normal = -vtx_data[6].normal;
            vtx_data[7].normal = -vtx_data[7].normal;
            vtx_data += 8;
            vtx_count += 8;
        }
        else {
            vtx_data += 4;
            vtx_count += 4;
        }
    }

    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(leaf_ptcl_vbo);
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }
    return vtx_count;
}

static void leaf_particle_free() {
    if (leaf_ptcl_data) {
        free(leaf_ptcl_data);
        leaf_ptcl_data = 0;
    }

    if (leaf_ptcl_vao) {
        glDeleteVertexArrays(1, &leaf_ptcl_vao);
        leaf_ptcl_vao = 0;
    }

    if (leaf_ptcl_vbo) {
        glDeleteBuffers(1, &leaf_ptcl_vbo);
        leaf_ptcl_vbo = 0;
    }

    if (leaf_ptcl_ebo) {
        glDeleteBuffers(1, &leaf_ptcl_ebo);
        leaf_ptcl_ebo = 0;
    }
}

static void litproj_textures_init() {
    litproj_shadow[0].init(2048, 512, 0, GL_R32F, GL_DEPTH_COMPONENT24);
    litproj_shadow[1].init(2048, 512, 0, GL_R32F, GL_ZERO);
    litproj_texture.init(1280, 720, 0, GL_RGBA8, GL_DEPTH_COMPONENT24);
}

static void litproj_textures_free() {
    litproj_shadow[0].free();
    litproj_shadow[1].free();
    litproj_texture.free();
}

static void particle_init(vec3* offset) {
    if (ptcl_data)
        delete[] ptcl_data;

    const size_t ptcl_vtx_count = ptcl_count * 0x06;

    ptcl_data = force_malloc_s(particle_data, ptcl_count);

    particle_data* data = ptcl_data;
    for (size_t i = 0; i < ptcl_count; i++, data++)
        data->alive = false;

    particle_index = 0;
    particle_count = 0;

    if (offset)
        particle_wind = *offset;
    else
        particle_wind = 0.0f;

    if (!ptcl_vao)
        glGenVertexArrays(1, &ptcl_vao);

    if (!ptcl_vbo)
        glGenBuffers(1, &ptcl_vbo);

    static const GLsizei buffer_size = sizeof(particle_vertex_data);

    gl_state_bind_array_buffer(ptcl_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * ptcl_vtx_count),
            0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
    else
        glBufferData(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * ptcl_vtx_count),
            0, GL_DYNAMIC_DRAW);

    gl_state_bind_vertex_array(ptcl_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, position)); // Pos
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, normal));   // Normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, color));    // Color0
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, texcoord)); // TexCoords

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);
}

static void particle_ctrl() {
    if (!ptcl_data)
        return;

    vec3 wind = particle_wind;

    particle_data* data = ptcl_data;
    for (int32_t i = 0; i < ptcl_count; i++, data++) {
        if (!data->alive)
            continue;

        vec3 v6 = data->direction + (wind - data->direction + vec3(0.0f, -3.0f, 0.0f)) * particle_delta_time;
        std::pair<vec3, float_t> v7 = sub_1406427A0(v6, vec3(0.0f, 1.0f, 0.0f));
        float_t v8 = vec3::dot(v7.first * data->normal, vec3(1.0f)) * v7.second;
        vec3 v14 = v6 + (v8 * data->normal * -0.345f);
        data->direction = 0.97f * v14;
        data->position += v14 * particle_delta_time;
        data->rotation += data->rotation_add * v7.second * particle_delta_time;
        data->life_time -= particle_delta_time;
        if (data->position.y < 0.0f || data->life_time < 0.0f)
            particle_kill(data);
    }
}

static int32_t particle_disp(particle_vertex_data* vtx_data, particle_data* data, int32_t count) {
    int32_t vtx_count = 0;
    for (size_t i = 0; i < count; i++, data++) {
        if (!data->alive)
            continue;

        vec3 pos = data->position;
        vec3 rot = data->rotation;

        mat3 mat;
        mat3_rotate_x(rot.x, &mat);
        mat3_rotate_y_mult(&mat, rot.y, &mat);
        mat3_rotate_z_mult(&mat, rot.z, &mat);

        float_t size = data->size;

        vec3 t0 = vec3( 0.0125f,  0.007216875f, 0.0f) * size;
        vec3 t1 = vec3(-0.0125f,  0.007216875f, 0.0f) * size;
        vec3 t2 = vec3( 0.0f   , -0.01443375f , 0.0f) * size;
        mat3_mult_vec(&mat, &t0, &t0);
        mat3_mult_vec(&mat, &t1, &t1);
        mat3_mult_vec(&mat, &t2, &t2);
        t0 += pos;
        t1 += pos;
        t2 += pos;

        vec3 normal = vec3(0.0f, 0.0f, 1.0f);
        mat3_mult_vec(&mat, &normal, &normal);
        data->normal = normal;

        vec4 color = data->color;
        vtx_data[0].position = t0;
        vtx_data[1].position = t1;
        vtx_data[2].position = t2;
        vtx_data[0].normal = normal;
        vtx_data[1].normal = normal;
        vtx_data[2].normal = normal;
        vtx_data[0].color = color;
        vtx_data[1].color = color;
        vtx_data[2].color = color;
        vtx_data[0].texcoord = vec2(0.0f);
        vtx_data[1].texcoord = vec2(1.0f, 0.5f);
        vtx_data[2].texcoord = vec2(0.0f, 1.0f);

        normal = -normal;
        vtx_data[3].position = t2;
        vtx_data[4].position = t1;
        vtx_data[5].position = t0;
        vtx_data[3].normal = normal;
        vtx_data[4].normal = normal;
        vtx_data[5].normal = normal;
        vtx_data[3].color = color;
        vtx_data[4].color = color;
        vtx_data[5].color = color;
        vtx_data[3].texcoord = vec2(0.0f, 1.0f);
        vtx_data[4].texcoord = vec2(1.0f, 0.5f);
        vtx_data[5].texcoord = vec2(0.0f);
        vtx_data += 6;
        vtx_count += 6;
    }
    return vtx_count;
}

static particle_data* particle_emit() {
    if (particle_count >= ptcl_count)
        return 0;

    for (int32_t i = 0; i < ptcl_count; i++) {
        particle_data* data = &ptcl_data[particle_index++];
        if (particle_index >= ptcl_count)
            particle_index = 0;
        if (data->alive)
            continue;

        *data = {};
        data->alive = true;
        particle_count++;
        return data;
    }
    return 0;
}

static void particle_event(particle_event_data* event_data) {
    if (!ptcl_data)
        return;

    float_t type = event_data->type;
    int32_t count = (int32_t)event_data->count;
    float_t size = event_data->size;
    vec3 trans = event_data->trans;
    float_t force = event_data->force;

    if (type == 1.0f) {
        vec4 color[3];
        color[0] = vec4(1.0f, 0.2f, 0.2f, 1.0f);
        color[1] = vec4(1.0f, 1.0f, 0.4f, 1.0f);
        color[2] = vec4(1.0f, 0.6f, 0.0f, 1.0f);

        for (int32_t i = 0; i < count; i++) {
            particle_data* data = particle_emit();
            if (!data)
                break;

            data->position = trans;
            data->rotation.x = rand_state_array_get_float(4) * 6.28f;
            data->rotation.y = rand_state_array_get_float(4) * 6.28f;
            vec3 direction = vec3(cosf(data->rotation.x), 0.0f, sinf(data->rotation.x));
            data->direction = direction * (rand_state_array_get_float(4) * force);
            data->normal = vec3(0.0f, 0.0f, 1.0f);
            data->rotation_add.x = (rand_state_array_get_float(4) + 1.0f) * 5.0f;
            data->rotation_add.y = (rand_state_array_get_float(4) + 1.0f) * 2.5f;
            data->rotation_add.z = 20.0f;

            int32_t color_rand = rand_state_array_get_int(0, 2, 4);
            data->color = color[color_rand];
            data->type = type;
            data->life_time = 10.0f;
            data->size = size;
        }
    }
    else if (type == 2.0f) {
        float_t v13 = rand_state_array_get_float(4) * (float_t)(M_PI * 2.0);
        float_t v14 = rand_state_array_get_float(4) * (float_t)(144.0 * DEG_TO_RAD);
        float_t v15 = sinf(v14);

        vec3 direction;
        direction.x = sinf(v13) * v15;
        direction.y = cosf(v14);
        direction.z = cosf(v13) * v15;

        for (int32_t i = 0; i < count; i++) {
            particle_data* data = particle_emit();
            if (!data)
                break;

            data->position = trans;
            data->normal = vec3(0.0f, 0.0f, 1.0f);
            data->rotation.x = rand_state_array_get_float(4) * 6.28f;
            data->rotation.y = rand_state_array_get_float(4) * 6.28f;
            data->direction = direction * ((rand_state_array_get_float(4) + 1.0f) * force);
            data->rotation_add.x = (rand_state_array_get_float(4) + 1.0f) * 5.0f;
            data->rotation_add.y = (rand_state_array_get_float(4) + 1.0f) * 2.5f;
            data->rotation_add.z = 20.0f;
            data->color.x = 5.0;
            data->color.y = 4.5;
            data->color.z = 2.0;
            data->color.w = 1.0;
            data->type = type;
            data->life_time = 8.0f;
            data->size = size;
        }
    }
}

static void particle_kill(particle_data* data) {
    data->alive = false;
    particle_count--;
}

static void particle_free() {
    if (ptcl_data)
        delete[] ptcl_data;

    ptcl_data = 0;
    particle_index = 0;
    particle_count = 0;

    if (ptcl_vao) {
        glDeleteVertexArrays(1, &ptcl_vao);
        ptcl_vao = 0;
    }

    if (ptcl_vbo) {
        glDeleteBuffers(1, &ptcl_vbo);
        ptcl_vbo = 0;
    }
}

static void rain_particle_init(bool change_stage) {
    vec3 velocity = stage_param_data_rain_current->velocity;
    vec3 vel_range = stage_param_data_rain_current->vel_range;

    const size_t rain_ptcl_vtx_count = rain_ptcl_count * 0x04;

    for (int32_t i = 0; i < 8; i++) {
        rain_particle_data& data = rain_ptcl_data[i];
        data.position.x = 0.0f;
        data.position.y = ((float_t)i * -12.5f) - 5.0f;
        data.position.z = 0.0f;
        data.texcoord.x = 1.0f;
        data.alpha = 1.0f;
        data.type = 1;
        data.velocity.x = rand_state_array_get_float(4);
        data.velocity.y = rand_state_array_get_float(4);
        data.velocity.z = rand_state_array_get_float(4);
        data.velocity = (data.velocity - 0.5f) * vel_range + velocity;
        data.field_2C = 0;
        data.field_30 = 0;
        data.field_34 = 0;
        data.field_38 = 0;
        data.field_3C = 0;
    }

    if (change_stage)
        return;

    if (!rain_ptcl_vao)
        glGenVertexArrays(1, &rain_ptcl_vao);

    if (!rain_ptcl_vbo)
        glGenBuffers(1, &rain_ptcl_vbo);

    vec3* vtx_data = force_malloc_s(vec3, rain_ptcl_vtx_count);
    for (int32_t i = 0; i < rain_ptcl_count; i++, vtx_data += 4) {
        vec3 position;
        position.x = rand_state_array_get_float(4);
        position.y = rand_state_array_get_float(4);
        position.z = rand_state_array_get_float(4);
        vtx_data[0] = position;
        vtx_data[1] = position;
        vtx_data[2] = position;
        vtx_data[3] = position;
    }
    vtx_data -= rain_ptcl_vtx_count;

    static const GLsizei buffer_size = sizeof(vec3);

    gl_state_bind_array_buffer(rain_ptcl_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * rain_ptcl_vtx_count), vtx_data, 0);
    else
        glBufferData(GL_ARRAY_BUFFER,
            (GLsizeiptr)(buffer_size * rain_ptcl_vtx_count), vtx_data, GL_STATIC_DRAW);
    free_def(vtx_data);

    gl_state_bind_vertex_array(rain_ptcl_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size, 0); // Pos

    size_t ebo_count = rain_ptcl_vtx_count / 4 * 6;
    uint32_t* ebo_data = force_malloc_s(uint32_t, ebo_count);
    for (size_t i = 0, j = 0; i < ebo_count; i += 6, j += 4) {
        ebo_data[i + 0] = (uint32_t)(j + 0);
        ebo_data[i + 1] = (uint32_t)(j + 1);
        ebo_data[i + 2] = (uint32_t)(j + 2);
        ebo_data[i + 3] = (uint32_t)(j + 0);
        ebo_data[i + 4] = (uint32_t)(j + 2);
        ebo_data[i + 5] = (uint32_t)(j + 3);
    }

    if (!rain_ptcl_ebo)
        glGenBuffers(1, &rain_ptcl_ebo);

    gl_state_bind_element_array_buffer(rain_ptcl_ebo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint32_t) * ebo_count), ebo_data, 0);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint32_t) * ebo_count), ebo_data, GL_STATIC_DRAW);
    free_def(ebo_data);

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);
    gl_state_bind_element_array_buffer(0);
}

static void rain_particle_ctrl() {
    if (!stage_param_data_rain_current)
        return;

    float_t delta_time = (float_t)(rain_particle_delta_frame * (1.0 / 60.0));

    for (int32_t i = 0; i < 8; i++) {
        rain_particle_data& data = rain_ptcl_data[i];
        data.position += delta_time * data.velocity;
        data.alpha = 1.0f;

        float_t pos_y = data.position.y;
        if (pos_y > -1.0f)
            data.alpha = max_def(-pos_y, 0.0f);
        else if (pos_y < -99.0f) {
            data.alpha = max_def(pos_y + 100.0f, 0.0f);
            if (pos_y < -100.0f)
                data.position = 0.0f;
        }
    }
}

static void rain_particle_free() {
    if (rain_ptcl_vao) {
        glDeleteVertexArrays(1, &rain_ptcl_vao);
        rain_ptcl_vao = 0;
    }

    if (rain_ptcl_vbo) {
        glDeleteBuffers(1, &rain_ptcl_vbo);
        rain_ptcl_vbo = 0;
    }

    if (rain_ptcl_ebo) {
        glDeleteBuffers(1, &rain_ptcl_ebo);
        rain_ptcl_ebo = 0;
    }
}
