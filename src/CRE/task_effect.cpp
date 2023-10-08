/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_effect.hpp"
#include "../KKdLib/prj/algorithm.hpp"
#include "../KKdLib/hash.hpp"
#include "auth_3d.hpp"
#include "data.hpp"
#include "gl_state.hpp"
#include "random.hpp"
#include "render_context.hpp"
#include "render_manager.hpp"
#include "shader_ft.hpp"
#include "texture.hpp"

struct for_ring_vertex_data {
    vec2 position;
    vec4 color;
    float_t size;
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

struct leaf_particle_scene_shader_data {
    vec4 g_transform[4];
    vec4 g_view_pos;
    vec4 g_color;
    vec4 g_light_env_stage_diffuse;
    vec4 g_light_env_stage_specular;
    vec4 g_lit_dir;
    vec4 g_lit_luce;
};

struct leaf_particle_vertex_data {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
};

struct particle_rot_data {
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

struct particle_scene_shader_data {
    vec4 g_transform[4];
    vec4 g_view_pos;
    vec4 g_light_env_chara_diffuse;
    vec4 g_light_env_chara_specular;
};

struct particle_vertex_data {
    vec3 position;
    vec3 normal;
    vec4 color;
    vec2 texcoord;
};

struct particle_data {
    vec3 position;
    float_t size;
    float_t alpha;
    int32_t life_time;
    vec3 velocity;
    vec3 direction;

    particle_data();
};

struct rain_particle_scene_shader_data {
    vec4 g_view[4];
    vec4 g_proj[4];
    vec4 g_range_scale;
    vec4 g_range_offset;
};

struct rain_particle_batch_shader_data {
    vec4 g_pos_offset;
    vec4 g_tangent;
    vec4 g_color;
};

struct ripple_batch_shader_data {
    vec4 g_params;
};

struct ripple_scene_shader_data {
    vec4 g_transform;
    vec4 g_texcoord;
};

struct ripple_emit_scene_shader_data {
    vec4 g_size_in_projection;
    vec4 g_transform;
    vec4 g_framebuffer_size;
};

struct snow_particle_vertex_data {
    vec3 position;
    float_t size;
    float_t alpha;
};

struct snow_particle_gpu_vertex_data {
    vec3 position;
    float_t size;
};

struct snow_particle_scene_shader_data {
    vec4 g_transform[4];
    vec4 g_view_world_row2;
    vec4 g_size_in_projection;
    vec4 g_state_point_attenuation;
    vec4 g_range_scale;
    vec4 g_range_offset;
    vec4 g_framebuffer_size;
    vec4 g_near_far;
};

struct snow_particle_batch_shader_data {
    vec4 g_pos_offset;
    vec4 g_color;
    vec4i start_vertex_location;
};

struct star_catalog_scene_shader_data {
    vec4 g_transform[4];
};

struct star_catalog_batch_shader_data {
    vec4 g_size_in_projection;
    vec4 g_modifiers;
    vec4 g_thresholds;
};

struct star_catalog_milky_way {
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    uint16_t restart_index;
    int32_t idx_count;
    int32_t longitude_degs_10;
    int32_t latitude_degs_10;
    float_t longitude_offset_degs_10;
    float_t latitude_offset_degs_10;
    float_t latitude;
    float_t longitude;
    float_t uv_rec_scale_u;
    float_t uv_rec_scale_v;

    star_catalog_milky_way();
    ~star_catalog_milky_way();

    void create_buffers(int32_t subdivs, float_t uv_rec_scale_u, float_t uv_rec_scale_v,
        int32_t longitude_degs_10, int32_t latitude_degs_10,
        float_t longitude_offset_degs_10, float_t latitude_offset_degs_10);
    void create_default_sphere();
    void delete_buffers();
    void draw(const mat4& vp, const mat4& model, texture* tex, GL::UniformBuffer& scene_ubo);
    void reset();
};

struct star_catalog_vertex {
    vec3 position;
    vec2 texcoord;

    star_catalog_vertex();
};

struct stars_buffer_data {
    vec3 position;
    float_t size;
    vec4 color;

    stars_buffer_data();
};

struct star_catalog {
    GLuint vao;
    bool random;
    bool enable;
    stage_param_star* stage_param_data_ptr;
    stage_param_star stage_param_data;
    star_catalog_milky_way milky_way;
    int32_t star_count;
    int32_t star_b_count;
    p_file_handler file_handler;
    uint32_t star_tex;
    uint32_t star_b_tex;
    uint32_t milky_way_tex_id;
    GL::ShaderStorageBuffer stars_ssbo;
    GL::UniformBuffer scene_ubo;
    GL::UniformBuffer batch_ubo;

    star_catalog();
    ~star_catalog();

    void draw();
    void free();
    bool init();
    void set_stage_param_data(stage_param_star* value);

    static void parse_data(std::vector<stars_buffer_data>& vec, size_t data);
};

struct struc_608 {
    const stage_effects* stage_effects;
    const stage_effects_modern* stage_effects_modern;
    prj::vector_pair<TaskEffectType, TaskEffect*> field_10;

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
    prj::vector_pair<TaskEffectType, TaskEffect*> effects;
    prj::vector_pair<TaskEffectType, TaskEffect*> field_50;
    prj::vector_pair<int32_t, struc_608> field_68;
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

static void draw_ripple_emit(render_context* rctx, struc_101* data);

static void leaf_particle_init(bool change_stage = false);
static void leaf_particle_ctrl();
static int32_t leaf_particle_disp();
static void leaf_particle_free();

static void particle_init(vec3* offset);
static void particle_ctrl();
static int32_t particle_disp(particle_vertex_data* vtx_data, particle_rot_data* data, int32_t count);
static particle_rot_data* particle_emit();
static void particle_event(particle_event_data* event_data);
static void particle_kill(particle_rot_data* data);
static void particle_free();

static void rain_particle_init(bool change_stage = false);
static void rain_particle_ctrl();
static void rain_particle_free();

static void ripple_emit_init();
static void ripple_emit_free();

static void snow_particle_init(bool change_stage = false);
static void snow_particle_ctrl();
static void snow_particle_data_init();
static void snow_particle_data_emit_fallen(particle_data* data);
static void snow_particle_data_kill_fallen(particle_data* data, bool kill);
static void snow_particle_data_reset(particle_data* data);
static void snow_particle_data_free();
static particle_data* snow_particle_emit_fallen();
static vec3 snow_particle_get_random_velocity();
static void snow_particle_free();

static void sub_1403B6ED0(RenderTexture* a1, RenderTexture* a2, RenderTexture* a3, ripple_emit_params& params);
static void sub_1403B6F60(GLuint a1, GLuint a2, GLuint a3, ripple_emit_params& params);

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
static bool stage_param_data_leaf_set;

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
static GL::UniformBuffer leaf_particle_scene_ubo;
static const size_t leaf_ptcl_count = 0x800;

static stage_param_litproj* stage_param_data_litproj_current;
static bool stage_param_data_litproj_set;

static bool light_proj_enable;
static RenderTexture litproj_shadow[2];
static RenderTexture litproj_texture;

static bool particle_enable;
static float_t particle_delta_time;
static int32_t particle_index;
static int32_t particle_count;
static vec3 particle_wind;
static particle_rot_data* ptcl_data;
static GLuint ptcl_vao;
static GLuint ptcl_vbo;
static GL::UniformBuffer particle_scene_ubo;
static const size_t ptcl_count = 0x400;

static stage_param_rain* stage_param_data_rain_current;
static bool stage_param_data_rain_set;

static bool rain_particle_enable;
static float_t rain_particle_delta_frame;
static uint32_t rain_particle_tex_id;
static particle_data rain_ptcl_data[8];
static GLuint rain_vao;
static GL::ShaderStorageBuffer rain_ssbo;
static GL::UniformBuffer rain_particle_scene_ubo;
static GL::UniformBuffer rain_particle_batch_ubo;
static const size_t rain_ptcl_count = 0x8000;

static GLuint ripple_vao;
static GL::UniformBuffer ripple_batch_ubo;
static GL::UniformBuffer ripple_scene_ubo;

static ripple_emit* ripple_emit_data;

static GLuint ripple_emit_vao;
static GL::ShaderStorageBuffer ripple_emit_ssbo;
static GL::UniformBuffer ripple_emit_scene_ubo;

static stage_param_snow* stage_param_data_snow_current;
static bool stage_param_data_snow_set;

static bool snow_particle_enable;
static float_t snow_particle_delta_frame;
static float_t snow_particle_size_min;
static float_t snow_particle_size_mid;
static float_t snow_particle_size_max;
static int32_t snow_particle_fallen_index;
static int32_t snow_particle_fallen_count;
static uint32_t snow_particle_tex_id;
static particle_data* snow_ptcl_data;
static particle_data snow_ptcl_gpu[4];
static particle_data* snow_ptcl_fallen_data;
static GLuint snow_vao;
static GL::ShaderStorageBuffer snow_ssbo;
static GL::ShaderStorageBuffer snow_gpu_ssbo;
static GL::ShaderStorageBuffer snow_fallen_ssbo;
static GL::UniformBuffer snow_particle_scene_ubo;
static GL::UniformBuffer snow_particle_batch_ubo;
static const size_t snow_ptcl_count = 0x8000;
static const size_t snow_ptcl_fallen_count = 0x2000;

static star_catalog star_catalog_data;

static TaskEffect** task_effect_data_array[] = {
    (TaskEffect**)&task_effect_auth_3d,
    0,
    (TaskEffect**)&task_effect_leaf,
    0,
    (TaskEffect**)&task_effect_snow,
    0,
    (TaskEffect**)&task_effect_ripple,
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
    (TaskEffect**)&task_effect_star,
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
extern int32_t width;
extern int32_t height;

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
        DelTask();
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
        mat4_mul_rotate_x(&mat, sinf((float_t)((double_t)((float_t)(frame_int % 512)
            * (float_t)(2.0 / 512.0)) * M_PI)) * 0.015f, &mat);
        mat4_mul_rotate_z(&mat, sinf((float_t)((double_t)((float_t)(frame_int % 360)
            * (float_t)(2.0 / 360.0)) * M_PI)) * 0.015f, &mat);
        task_stage_set_mat(mat);
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
        const stage_data_modern* stg_data = stage_data->get_stage_data_modern(i);
        if (!stg_data || !stg_data->auth_3d_ids.size())
            continue;

        struc_621 v30;
        v30.stage_hash = i;

        if (stage.count != stage.max_count)
            for (uint32_t j : stg_data->auth_3d_ids) {
                if (stage.count == stage.max_count)
                    break;

                auth_3d_id id = auth_3d_data_load_hash(j, data, obj_db, tex_db);
                if (!id.check_not_empty())
                    break;

                id.read_file_modern();
                id.set_visibility(false);
                if (stage.count + 1 <= stage.max_count)
                    stage.auth_3d_ids_ptr[stage.count++] = id;
                v30.auth_3d_ids.push_back(id);
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

        const stage_data* stage_data = aft_stage_data->get_stage_data(i);
        if (!stage_data || !stage_data->auth_3d_ids.size())
            continue;

        struc_621 v30;
        v30.stage_index = i;

        if (stage.count != stage.max_count)
            for (int32_t j : stage_data->auth_3d_ids) {
                if (stage.count == stage.max_count)
                    break;

                auth_3d_id id = auth_3d_data_load_uid(j, aft_auth_3d_db);
                if (!id.check_not_empty())
                    break;

                id.read_file(aft_auth_3d_db);
                id.set_visibility(false);
                if (stage.count + 1 <= stage.max_count)
                    stage.auth_3d_ids_ptr[stage.count++] = id;
                v30.auth_3d_ids.push_back(id);
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
        id.unload(rctx_ptr);
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
field_124(), field_2B8(), field_2B9(), disp(), frame_rate_control(), vao() {
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

    for_ring_vertex_data* ptcl_vtx_data = (for_ring_vertex_data*)ssbo.MapMemory();
    if (!ptcl_vtx_data) {
        ssbo.UnmapMemory();
        num_vtx = 0;
        return;
    }

    vec4 color = this->color;
    for (int32_t i = num_ptcls; i > 0; i--, ptcl_data++, ptcl_vtx_data++) {
        float_t size = ptcl_data->size * (float_t)(1.0 / 256.0);
        color.w = ptcl_data->density * density;

        vec2 position;
        position.x = ptcl_data->position.x * (float_t)(1.0 / 8.0);
        position.y = ptcl_data->position.z * (float_t)(1.0 / 8.0);
        ptcl_vtx_data->position = position;
        ptcl_vtx_data->color = color;
        ptcl_vtx_data->size = size;
    }

    ssbo.UnmapMemory();

    num_vtx = (int32_t)(num_ptcls * 6LL);
}

void TaskEffectFogRing::Data::Ctrl() {
    if (!disp)
        return;

    CtrlInner(delta_frame * (float_t)(1.0 / 60.0));
    CalcVert();
    field_8 = 0;
}

void TaskEffectFogRing::Data::CtrlInner(float_t delta_time) {
    if (fabsf(delta_time) <= 0.000001f)
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

    ssbo.Destroy();

    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_PRE_PROCESS, false);
    rctx_ptr->render_manager.clear_pre_process(0);
    rctx_ptr->draw_state.set_fog_height(false);
}

void TaskEffectFogRing::Data::Disp() {
    if (enable && disp)
        rctx_ptr->disp_manager.entry_obj_user(&mat4_identity,
            (mdl::UserArgsFunc)draw_fog_particle, this, mdl::OBJ_TYPE_USER);
}

void TaskEffectFogRing::Data::Draw() {
    render_context* rctx = rctx_ptr;

    rctx->draw_state.set_fog_height(false);
    if (!enable || !disp)
        return;

    rctx->draw_state.set_fog_height(true);
    RenderTexture& rt = rctx->render_manager.get_render_texture(8);
    rt.Bind();
    glViewport(0, 0,
        rt.color_texture->get_width_align_mip_level(),
        rt.color_texture->get_height_align_mip_level());
    glClearColor(density_offset, density_offset, density_offset, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_USER))
        rctx->disp_manager.draw(mdl::OBJ_TYPE_USER);
    gl_state_bind_framebuffer(0);
    rctx->render_manager.set_effect_texture(rt.color_texture);
    gl_state_get_error();
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
    rctx_ptr->render_manager.clear_pre_process(0);
    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_PRE_PROCESS, false);
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

    current_stage_index = this->stage_indices.front();

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

    if (ptcl_data) {
        free(ptcl_data);
        ptcl_data = 0;
    }

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    ssbo.Destroy();

    ptcl_data = new (force_malloc<fog_ring_data>(max_ptcls)) fog_ring_data[max_ptcls];

    if (!vao)
        glGenVertexArrays(1, &vao);

    ssbo.Create(sizeof(for_ring_vertex_data) * max_ptcls);

    InitParticleData();

    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_PRE_PROCESS, true);
    rctx_ptr->render_manager.add_pre_process(0, TaskEffectFogRing::Data::DrawStatic, this);
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
    SetFrameRateControl();
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
        data.frame_rate_control = get_sys_frame_rate();
}

void TaskEffectFogRing::Reset() {
    data.Reset();
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
        SetFrameRateControl();
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

    int32_t stage_index = this->stage_indices.front();
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
        frame_rate_control = get_sys_frame_rate();
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
    delete rctx_ptr->litproj;
    rctx_ptr->litproj = 0;
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
    if (this->stage_indices.size())
        return;

    int32_t stage_index = this->stage_indices.front();
    current_stage_index = stage_index;
    stage_param_litproj* litproj = stage_param_data_litproj_storage_get_value(stage_index);
    if (litproj) {
        data_struct* aft_data = &data_list[DATA_AFT];
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        stage_param_data_litproj_current = litproj;

        light_proj* _litproj = new light_proj(width, height);
        rctx_ptr->litproj = _litproj;
        _litproj->enable = true;
        _litproj->texture_id = aft_tex_db->get_texture_id(litproj->tex_name.c_str());
        stage_param_data_litproj_set = true;
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
    SetFrameRateControl();
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
        frame_rate_control = get_sys_frame_rate();
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
        rain_particle_init();
        SetFrameRateControl();
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

    int32_t stage_index = this->stage_indices.front();
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
    if (found) {
        stage_param_data_rain_current = stage_param_data_rain_storage_get_value(value);
        rain_particle_init(true);
    }
}

void TaskEffectRain::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = get_sys_frame_rate();
}

void TaskEffectRain::Reset() {
    if (stage_param_data_rain_current) {
        rain_particle_init();
        SetFrameRateControl();
    }
}

ripple_emit_draw_data::ripple_emit_draw_data() : data() {

}

struc_192::struc_192() {
    index = -1;
}

struc_207::struc_207() {

}

ripple_emit_params::ripple_emit_params() {
    wake_attn = 0.56f;
    speed = 1.0f;
    field_8 = 0.0005f;
    field_C = 0.9f;
}

ripple_emit::ripple_emit() : delta_frame(), update(), rain_ripple_num(), rain_ripple_min_value(),
rain_ripple_max_value(), field_14(), emit_pos_scale(), emit_pos_ofs_x(), emit_pos_ofs_z(), ripple_tex_id(),
use_float_ripplemap(), field_30(), rob_emitter_size(), emitter_num(), emitter_size(), field_4C(), field_50(),
field_178(), field_2A0(), field_3C8(), field_4F0(), field_BB4(), counter(), field_BEC(), stage_set() {
    ground_y = -1001.0f;
    emitter_list = 0;
    current_stage_index = -1;
}

ripple_emit::~ripple_emit() {
    ground_y = -1001.0f;
}

void ripple_emit::add_draw_ripple_emit(struc_101* data) {
    if (data->count > 0)
        rctx_ptr->disp_manager.entry_obj_user(&mat4_identity,
            (mdl::UserArgsFunc)draw_ripple_emit, data, mdl::OBJ_TYPE_USER);
}

void ripple_emit::clear_tex() {
    vec4 clear_color;
    glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clear_color);

    for (int32_t i = 0, j = 5; i < 3; i++, j++) {
        RenderTexture* rt;
        float_t v5;
        if (use_float_ripplemap) {
            rt = &rctx_ptr->render_manager.get_render_texture(j - 3);
            v5 = -0.3f;
        }
        else {
            rt = &rctx_ptr->render_manager.get_render_texture(j);
            v5 = 0.5f;
        }

        glClearColor(0.0f, 0.0f, 0.0f, v5);
        rt->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_state_bind_framebuffer(0);
    }

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
}

void ripple_emit::ctrl() {
    if (delta_frame > 0.0f)
        update = true;
}

void ripple_emit::dest() {
    stage_param_data_ripple_storage_clear();
    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_PRE_PROCESS, false);
    rctx_ptr->render_manager.clear_pre_process(0);
    this->ground_y = -1001.0;
}

void ripple_emit::disp() {
    if (!stage_set)
        return;

    if (use_float_ripplemap) {
        if (field_30 == 60)
            sub_140358690();
        else
            sub_14035AED0();
        if (field_30 > 0)
            field_30--;
    }
    else {
        sub_14035AED0();
        sub_14035AAE0();
    }
}

void ripple_emit::draw() {
    if (!stage_set)
        return;

    gl_state_disable_cull_face();

    render_context* rctx = rctx_ptr;

    RenderTexture* rt[3];
    for (int32_t i = 0, j = 2; i < 3; i++, j++)
        rt[i] = &rctx->render_manager.get_render_texture(
            use_float_ripplemap ? j : (j + 3));

    if (update) {
        int32_t counter = this->counter + 1;
        if (counter >= 3)
            counter = 0;

        rt[(counter + 1) % 3]->Bind();

        GLint v43[4];
        glGetIntegerv(GL_VIEWPORT, v43);

        int32_t width = rt[0]->color_texture->width;
        int32_t height = rt[0]->color_texture->height;

        glViewport(1, 1, width - 2, height - 2);

        draw_pass_set_camera(rctx_ptr);
        glClear(GL_DEPTH_BUFFER_BIT);
        if (rctx->disp_manager.get_obj_count(mdl::OBJ_TYPE_USER)) {
            gl_state_active_bind_texture_2d(7, rt[counter % 3]->color_texture->tex);
            rctx->disp_manager.draw(mdl::OBJ_TYPE_USER, 0, true);
            gl_state_active_bind_texture_2d(7, 0);
        }

        gl_state_bind_framebuffer(0);

        params.field_8 = 0.0005f;
        params.field_C = 0.97f;
        if (field_30 > 0) {
            params.field_8 = 0.00005f;
            params.field_C = 0.999f;
        }

        sub_1403B6ED0(rt[(counter + 2) % 3], rt[(counter + 1) % 3], rt[counter % 3], params);

        glViewport(v43[0], v43[1], v43[2], v43[3]);

        sub_1403584A0(rt[(counter + 2) % 3]);

        this->counter = counter;
    }

    int32_t v11 = (counter + 2) % 3 + 2;
    if (!use_float_ripplemap)
        v11 += 3;

    rctx->render_manager.set_effect_texture(
        rctx->render_manager.get_render_texture(v11).color_texture);

    update = false;

    gl_state_enable_cull_face();
}

void ripple_emit::reset() {
    field_4F0 = 18;
    for (struc_207& i : field_4F4)
        for (int32_t j = 0; j < field_4F0; j++)
            i.field_0[j].trans = 0.0f;

    field_30 = 60;

    clear_tex();
}

void ripple_emit::set_stage_index(int32_t stage_index) {
    if (current_stage_index == stage_index)
        return;

    current_stage_index = stage_index;
    stage_set = false;
    if (stage_index == -1)
        return;

    for (int32_t& i : stage_indices) {
        if (i != stage_index)
            continue;

        stage_set = true;
        set_stage_param(stage_param_data_ripple_storage_get_value(i));
        clear_tex();
        break;
    }
}

void ripple_emit::set_stage_indices(std::vector<int32_t>& stage_indices) {
    static const int32_t dword_1409E5330[] = {
        0, 1, 2, 4, 10, 5, 12, 7, 14, 9, 21, 15, 23, 17, 25, 19, 26, 20
    };

    stage_set = false;
    current_stage_index = -1;
    this->stage_indices.clear();

    stage_param_data_ripple_storage_clear();
    for (int32_t& i : stage_indices) {
        stage_param_ripple ripple;
        if (task_effect_array_parse_stage_param_data_ripple(&ripple, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_ripple_storage_set_stage_data(i, &ripple);
        }
    }

    if (!this->stage_indices.size())
        return;

    current_stage_index = this->stage_indices.front();
    stage_param_ripple* ripple = stage_param_data_ripple_storage_get_value(this->stage_indices.front());
    if (!ripple)
        return;

    stage_set = true;
    set_stage_param(ripple);

    delta_frame = 1.0f;
    counter = 0;
    field_BEC = 0;

    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_PRE_PROCESS, true);
    rctx_ptr->render_manager.add_pre_process(0, ripple_emit::draw_static, this);

    field_4F0 = 18;
    for (struc_207& i : field_4F4)
        for (int32_t j = 0; j < field_4F0; j++) {
            i.field_0[j].index = dword_1409E5330[j];
            i.field_0[j].trans = 0.0f;
        }

    update = false;
    field_30 = 60;

    clear_tex();
}

void ripple_emit::set_stage_param(stage_param_ripple* ripple) {
    data_struct* aft_data = &data_list[DATA_AFT];
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

    params.wake_attn = ripple->wake_attn;
    params.speed = ripple->speed;
    rain_ripple_num = (int32_t)ripple->rain_ripple_num;
    rain_ripple_min_value = ripple->rain_ripple_min_value;
    rain_ripple_max_value = ripple->rain_ripple_max_value;
    ground_y = ripple->ground_y;
    emit_pos_scale = ripple->emit_pos_scale;
    emit_pos_ofs_x = ripple->emit_pos_ofs_x;
    emit_pos_ofs_z = ripple->emit_pos_ofs_z;
    ripple_tex_id = aft_tex_db->get_texture_id(ripple->ripple_tex_name.c_str());
    use_float_ripplemap = ripple->use_float_ripplemap;
    rob_emitter_size = ripple->rob_emitter_size;
    emitter_num = ripple->emitter_num;
    emitter_list = ripple->emitter_list.data();
    emitter_size = ripple->emitter_size;
}

void ripple_emit::draw_static(void* data) {
    ((ripple_emit*)data)->draw();
}

void ripple_emit::sub_1403584A0(RenderTexture* rt) {
    if (ripple_tex_id == -1)
        return;

    texture* ripple_tex = texture_storage_get_texture(ripple_tex_id);
    if (!ripple_tex)
        return;

    field_BB8.SetColorDepthTextures(ripple_tex->tex);
    field_BB8.Bind();

    image_filter_scale(rctx_ptr, ripple_tex->tex, rt->color_texture->tex, 1.0f);
    gl_state_bind_framebuffer(0);
}


void ripple_emit::sub_140358690() {
    ripple_emit_draw_data& v1 = field_178;
    v1.data.vertex = field_50.vertex;
    v1.data.color = field_50.color;

    for (int32_t i = 0; i < 16; i++) {
        v1.data.vertex[i].x = rand_state_array_get_float(4) * 1.8f - 0.9f;
        v1.data.vertex[i].y = rand_state_array_get_float(4) * 1.8f - 0.9f;
        rand_state_array_get_int(0x03, 0x07, 4);
        v1.data.vertex[i].z = (float_t)rand_state_array_get_int(0x20, 0xA0, 4);
    }

    v1.data.count = 16;
    v1.data.ripple_uniform = 1;
    v1.data.ripple_emit_uniform = 1;

    add_draw_ripple_emit(&v1.data);
}

void ripple_emit::sub_1403587C0(const vec3 a2, const vec3 a3, float_t a4, struc_101& a5, struc_101& a6) {
    vec3 v34 = a3 - a2;

    float_t v17 = ground_y - (a2.y - a4);
    int32_t v19 = 1 - (int32_t)(min_def(v17, a4) * -30.0f);
    int32_t v20 = v19 + 3;

    float_t v21 = sqrtf(v34.x * v34.x + v34.z * v34.z) * 0.8f;
    if (v21 < 0.03f)
        v21 = 0.0f;

    if (a3.y - a4 > ground_y) {
        v21 += (float_t)(v34.y * 3.0f);
        v20 += (int32_t)(float_t)(v34.y * 30.0f);
    }

    float_t v23 = sqrtf(v34.x * v34.x + v34.z * v34.z);
    if (v23 >= 0.01f)
        v34 *= 1.0f / v23;
    else
        v34 = 0.0f;

    float_t v24 = min_def(v21, 0.2f);
    if (v23 > 0.5f) {
        v23 = 0.01f;
        v24 = 0.0f;
    }

    int32_t v33 = v20 - (int32_t)(v23 * -20.0f);
    int32_t v37 = (int32_t)(a4 * 60.0f);
    int32_t v27 = v33 >= v37 ? v37 : v33;

    color4u8 v19a = { 0x08, 0x00, 0x00, (uint8_t)v19 };
    color4u8 v29 = { (uint8_t)v27, 0x00, 0x00, (uint8_t)(int32_t)(v24 * 1275.0f) };

    for (float_t i = 0.0f; i < v23; i += 0.03f) {
        if (v24 > 0.0f) {
            int32_t count = a6.count;
            if (count < 16) {
                a6.vertex[count].x = (a2.x - v34.x * 0.2f + i * v34.x) * emit_pos_scale;
                a6.vertex[count].y = 0.0;
                a6.vertex[count].z = (a2.z - v34.z * 0.2f + i * v34.z) * emit_pos_scale;
                a6.color[count] = v29;
                a6.count++;
            }
        }

        int32_t count = a5.count;
        if (count < 16) {
            a5.vertex[count].x = (a2.x + i * v34.x) * emit_pos_scale;
            a5.vertex[count].y = 0.0f;
            a5.vertex[count].z = (a2.z + i * v34.z) * emit_pos_scale;
            a6.color[count] = v19a;
            a5.count++;
        }
    }
}

void ripple_emit::sub_14035AAE0() {
    ripple_emit_draw_data& v1 = field_3C8;

    field_3C8.data.count = 0;
    field_3C8.data.vertex = field_3C8.vertex;
    field_3C8.data.color = field_3C8.color;
    field_3C8.data.size = emitter_size;

    if (!update)
        return;

    if (rain_ripple_num) {
        int32_t min_value = (int32_t)(rain_ripple_min_value * 127.0f);
        int32_t max_value = (int32_t)(rain_ripple_max_value * 127.0f - rain_ripple_min_value * 127.0f);

        max_value = max_def(max_value, 1);
        min_value = min_def(min_value, 126);

        for (int32_t i = 0; i < rain_ripple_num; i++) {
            v1.data.vertex[i].x = (float_t)(rand_state_array_get_int(4) % 1000) * 0.001f * 2.0f - 1.0f;
            v1.data.vertex[i].y = 0.0f;
            v1.data.vertex[i].y = (float_t)(rand_state_array_get_int(4) % 1000) * 0.001f * 2.0f - 1.0f;
            v1.data.color[i] = { 0x00, 0x00, 0x00,
                (uint8_t)(0x7F - rand_state_array_get_int(4) % max_value - min_value) };
            v1.data.count++;
        }
    }

    for (size_t i = 0; i < emitter_num; i++) {
        float_t v10 = emitter_list[i].z;
        v1.data.vertex[i].x = ((rand_state_array_get_float(4) - 0.5f)
            * v10 + emitter_list[i].x) * emit_pos_scale;
        v1.data.vertex[i].y = 0.0f;
        v1.data.vertex[i].z = ((rand_state_array_get_float(4) - 0.5f)
            * v10 + emitter_list[i].y) * emit_pos_scale;
        v1.data.color[i] = rand_state_array_get_float(4) < 0.5f
            ? color4u8(0x00, 0x00, 0x00, 0xFF) : color4u8(0x00);
        v1.data.count++;
    }

    v1.data.ripple_uniform = use_float_ripplemap ? 1 : 0;
    v1.data.ripple_emit_uniform = 0;

    add_draw_ripple_emit(&v1.data);
}

void ripple_emit::sub_14035AED0() {
    field_178.data.count = 0;
    field_178.data.vertex = field_178.vertex;
    field_178.data.color = field_178.color;
    field_178.data.size = rob_emitter_size;

    field_2A0.data.count = 0;
    field_2A0.data.vertex = field_2A0.vertex;
    field_2A0.data.color = field_2A0.color;
    field_2A0.data.size = rob_emitter_size;

    if (!update)
        return;

    ripple_emit_draw_data& v2 = field_178;
    ripple_emit_draw_data& v3 = field_2A0;

    int32_t chara_id = 0;
    for (struc_207& i : field_4F4) {
        if (!rob_chara_array_check_visibility(chara_id)) {
            chara_id++;
            continue;
        }

        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        if (!rob_chr) {
            chara_id++;
            continue;
        }

        for (int32_t j = 0; j < field_4F0; j++) {
            struc_192& v4 = i.field_0[j];
            vec3 trans = 0.0f;
            float_t scale = rob_chr->get_trans_scale(v4.index, trans);
            if (trans.y - ground_y < scale) {
                if (use_float_ripplemap)
                    sub_1403587C0(trans, v4.trans, scale, v2.data, v3.data);
                else if (v2.data.count < 16) {
                    v2.data.vertex[v2.data.count].x = ((rand_state_array_get_float(4) - 0.5f)
                        * 0.02f + trans.x) * emit_pos_scale + emit_pos_ofs_x;
                    v2.data.vertex[v2.data.count].y = trans.y;
                    v2.data.vertex[v2.data.count].z = ((rand_state_array_get_float(4) - 0.5f)
                        * 0.02f + trans.z) * emit_pos_scale + emit_pos_ofs_z;
                    v2.data.color[v2.data.count] = { 0x00, 0x00, 0x00, 0x00 };
                    v2.data.count++;
                }
            }
            v4.trans = trans;
        }

        chara_id++;
    }

    if (use_float_ripplemap) {
        v3.data.ripple_uniform = !!use_float_ripplemap;
        v3.data.ripple_emit_uniform = 1;
        add_draw_ripple_emit(&v3.data);
    }

    v2.data.ripple_uniform = !!use_float_ripplemap;
    v2.data.ripple_emit_uniform = 0;
    add_draw_ripple_emit(&v2.data);
}

TaskEffectRipple::TaskEffectRipple() : field_68(), frame_rate_control(), emit() {

}

TaskEffectRipple::~TaskEffectRipple() {

}

bool TaskEffectRipple::Init() {
    ripple_emit_init();
    //sub_1400DE640("TaskEffectRipple::init()\n");
    return true;
}

bool TaskEffectRipple::Ctrl() {
    emit->delta_frame = frame_rate_control->GetDeltaFrame();
    emit->ctrl();
    return false;
}

bool TaskEffectRipple::Dest() {
    emit->dest();
    //sub_1400DE640("TaskEffectRipple::dest()\n");
    ripple_emit_free();
    return true;
}

void TaskEffectRipple::Disp() {
    emit->disp();
}

void TaskEffectRipple::PreInit(int32_t stage_index) {

}

void TaskEffectRipple::SetStageIndices(std::vector<int32_t>& stage_indices) {
    SetFrameRateControl();
    emit = ripple_emit_data;
    emit->set_stage_indices(stage_indices);
}

void TaskEffectRipple::SetCurrentStageIndex(int32_t value) {
    emit->set_stage_index(value);
}

void TaskEffectRipple::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = get_sys_frame_rate();
}

void TaskEffectRipple::Reset() {
    if (!emit->use_float_ripplemap)
        ripple_emit_data->clear_tex();
    ripple_emit_data->reset();
}

TaskEffectSnow::TaskEffectSnow() : frame_rate_control() {
    current_stage_index = -1;
}

TaskEffectSnow::~TaskEffectSnow() {

}

bool TaskEffectSnow::Init() {
    if (stage_param_data_snow_current) {
        snow_particle_init();
        SetFrameRateControl();
    }
    return true;
}

bool TaskEffectSnow::Ctrl() {
    if (stage_param_data_snow_current) {
        snow_particle_delta_frame = frame_rate_control->GetDeltaFrame();
        snow_particle_ctrl();
    }
    return false;
}

bool TaskEffectSnow::Dest() {
    snow_particle_data_free();
    snow_particle_free();

    stage_param_data_snow_current = 0;
    stage_param_data_snow_storage_clear();
    stage_param_data_snow_set = 0;
    current_stage_index = -1;
    stage_indices.clear();
    return 1;
}

void TaskEffectSnow::Disp() {

}

void TaskEffectSnow::Basic() {

}

void TaskEffectSnow::PreInit(int32_t stage_index) {

}

void TaskEffectSnow::SetStageIndices(std::vector<int32_t>& stage_indices) {
    if (stage_param_data_snow_current)
        Dest();

    stage_param_data_snow_set = 0;
    current_stage_index = -1;
    this->stage_indices.clear();
    stage_param_data_snow_storage_clear();

    for (int32_t i : stage_indices) {
        stage_param_snow snow;
        if (task_effect_array_parse_stage_param_data_snow(&snow, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_snow_storage_set_stage_data(i, &snow);
        }
    }

    stage_param_data_snow_current = 0;
    if (!this->stage_indices.size())
        return;

    int32_t stage_index = this->stage_indices.front();
    current_stage_index = stage_index;
    stage_param_snow* snow = stage_param_data_snow_storage_get_value(stage_index);

    snow_particle_tex_id = -1;
    if (snow->tex_name.size()) {
        data_struct* aft_data = &data_list[DATA_AFT];
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        snow_particle_tex_id = aft_tex_db->get_texture_id(snow->tex_name.c_str());
    }

    if (snow) {
        stage_param_data_snow_current = snow;
        stage_param_data_snow_set = true;
    }
}

void TaskEffectSnow::SetEnable(bool value) {
    snow_particle_enable = value;
}

void TaskEffectSnow::SetCurrentStageIndex(int32_t value) {
    if (current_stage_index == value)
        return;

    current_stage_index = value;

    bool found = false;
    for (int32_t i : stage_indices)
        if (i == value) {
            found = true;
            break;
        }

    stage_param_data_snow_set = found;
    if (found) {
        stage_param_data_snow_current = stage_param_data_snow_storage_get_value(value);
        snow_particle_init(true);
    }
}

void TaskEffectSnow::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = get_sys_frame_rate();
}

void TaskEffectSnow::Reset() {
    if (stage_param_data_snow_current) {
        snow_particle_init();
        SetFrameRateControl();
    }
}

/*
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
    std::vector<vec4u8> field_60;
    struc_101 field_78;
    float_t ripple_emission;

    water_particle();
    ~water_particle();
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
*/

TaskEffectStar::TaskEffectStar() : frame_rate_control(), delta_frame() {
    current_stage_index = -1;
}

TaskEffectStar::~TaskEffectStar() {

}

bool TaskEffectStar::Init() {
    if (!star_catalog_data.stage_param_data_ptr)
        return true;
    else if (!star_catalog_data.init())
        return false;

    SetFrameRateControl(0);
    return true;
}

bool TaskEffectStar::Ctrl() {
    if (star_catalog_data.stage_param_data_ptr)
        delta_frame = frame_rate_control->GetDeltaFrame();
    return false;
}

bool TaskEffectStar::Dest() {
    star_catalog_data.free();
    stage_param_data_star_storage_clear();
    return true;
}

void TaskEffectStar::Disp() {

}

void TaskEffectStar::SetStageIndices(std::vector<int32_t>& stage_indices) {
    if (star_catalog_data.stage_param_data_ptr)
        Dest();

    current_stage_index = -1;
    this->stage_indices.clear();
    stage_param_data_star_storage_clear();
    delta_frame = 1.0f;
    for (int32_t i : stage_indices) {
        stage_param_star star;
        if (task_effect_array_parse_stage_param_data_star(&star, i)) {
            this->stage_indices.push_back(i);
            stage_param_data_star_storage_set_stage_data(i, &star);
        }
    }

    if (this->stage_indices.size()) {
        int32_t stage_index = this->stage_indices.front();
        current_stage_index = stage_index;
        stage_param_star* star = stage_param_data_star_storage_get_value(stage_index);

        if (star) {
            const char* dir = "./rom/";
            const char* farc_file;
            const char* file;

            if (star_catalog_data.random) {
                farc_file = "star_catalog_random.farc";
                file = "random.bin";
            }
            else {
                farc_file = "star_catalog_megastar2.farc";
                file = "megastar2.bin";
            }

            star_catalog_data.file_handler.read_file(&data_list[DATA_AFT], dir, farc_file, file, false);
            star_catalog_data.set_stage_param_data(star);
        }
    }
}

void TaskEffectStar::SetEnable(bool value) {
    star_catalog_data.enable = value;
}

void TaskEffectStar::SetCurrentStageIndex(int32_t value) {
    if (current_stage_index == value)
        return;

    current_stage_index = value;

    bool found = false;
    for (int32_t i : stage_indices)
        if (i == value) {
            found = true;
            break;
        }

    star_catalog_data.stage_param_data_ptr = 0;
    star_catalog_data.star_tex = -1;
    star_catalog_data.star_b_tex = -1;
    star_catalog_data.milky_way_tex_id = -1;
    if (found)
        star_catalog_data.set_stage_param_data(stage_param_data_star_storage_get_value(value));
}

void TaskEffectStar::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        frame_rate_control = value;
    else
        frame_rate_control = get_sys_frame_rate();
}

void TaskEffectStar::Reset() {

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

    const light_data& light_stage = rctx_ptr->light_set[LIGHT_SET_MAIN].lights[LIGHT_STAGE];

    leaf_particle_scene_shader_data shader_data = {};
    mat4 temp;
    mat4_transpose(&rctx_ptr->vp_mat, &temp);
    shader_data.g_transform[0] = temp.row0;
    shader_data.g_transform[1] = temp.row1;
    shader_data.g_transform[2] = temp.row2;
    shader_data.g_transform[3] = temp.row3;
    rctx_ptr->camera->get_view_point(shader_data.g_view_pos);
    shader_data.g_color = stage_param_data_leaf_current->color;
    light_stage.get_diffuse(shader_data.g_light_env_stage_diffuse);
    light_stage.get_specular(shader_data.g_light_env_stage_specular);
    shader_data.g_lit_dir = rctx_ptr->obj_scene.g_light_chara_dir;
    shader_data.g_lit_luce = rctx_ptr->obj_scene.g_light_chara_luce;
    leaf_particle_scene_ubo.WriteMemory(shader_data);

    gl_state_active_bind_texture_2d(0, tex->tex);
    shaders_ft.set(SHADER_FT_LEAF_PT);
    leaf_particle_scene_ubo.Bind(0);
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

    stage_param_rain* rain = stage_param_data_rain_current;
    vec3 range = rain->range;
    vec3 range_scale = rain->range;
    vec3 range_offset = rain->offset;
    range_offset.x -= range.x * 0.5f;
    range_offset.z -= range.z * 0.5f;

    rain_particle_scene_shader_data scene_shader_data = {};
    mat4 temp;
    mat4_transpose(&rctx_ptr->view_mat, &temp);
    scene_shader_data.g_view[0] = temp.row0;
    scene_shader_data.g_view[1] = temp.row1;
    scene_shader_data.g_view[2] = temp.row2;
    scene_shader_data.g_view[3] = temp.row3;
    mat4_transpose(&rctx_ptr->proj_mat, &temp);
    scene_shader_data.g_proj[0] = temp.row0;
    scene_shader_data.g_proj[1] = temp.row1;
    scene_shader_data.g_proj[2] = temp.row2;
    scene_shader_data.g_proj[3] = temp.row3;
    scene_shader_data.g_range_scale = { range_scale.x, range_scale.y, range_scale.z, 0.0f };
    scene_shader_data.g_range_offset = { range_offset.x, range_offset.y, range_offset.z, 0.0f };
    rain_particle_scene_ubo.WriteMemory(scene_shader_data);

    gl_state_enable_blend();
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_active_bind_texture_2d(0, tex->tex);
    shaders_ft.set(SHADER_FT_RAIN);

    float_t tangent_sign = -rain->psize.x;
    float_t tangent_size = -rain->psize.y * (float_t)(1.0 / 30.0);

    int32_t first = 0;
    int32_t count = min_def(rain->num_rain, (int32_t)rain_ptcl_count) / 2 / 4 * 6;

    vec4 color = rain->color;
    float_t color_a = color.w;
    gl_state_bind_vertex_array(rain_vao);
    rain_particle_scene_ubo.Bind(0);
    rain_particle_batch_ubo.Bind(1);
    rain_ssbo.Bind(0);
    for (int32_t i = 0; i < 8; i++, first += count) {
        particle_data& data = rain_ptcl_data[i];
        vec3 pos_offset = data.position / range;
        vec3 tangent = data.velocity * tangent_size;
        color.w = color_a * data.alpha;

        rain_particle_batch_shader_data batch_shader_data = {};
        batch_shader_data.g_pos_offset = { pos_offset.x, pos_offset.y, pos_offset.z, 0.075f };
        batch_shader_data.g_tangent = { tangent.x, tangent.y, tangent.z, tangent_sign };
        batch_shader_data.g_color = color;
        rain_particle_batch_ubo.WriteMemory(batch_shader_data);
        shaders_ft.draw_arrays(GL_TRIANGLES, first, count);
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

    const light_data& light_chara = rctx_ptr->light_set[LIGHT_SET_MAIN].lights[LIGHT_CHARA];

    particle_scene_shader_data shader_data = {};
    mat4 temp;
    mat4_transpose(&rctx_ptr->vp_mat, &temp);
    shader_data.g_transform[0] = temp.row0;
    shader_data.g_transform[1] = temp.row1;
    shader_data.g_transform[2] = temp.row2;
    shader_data.g_transform[3] = temp.row3;
    rctx_ptr->camera->get_view_point(shader_data.g_view_pos);
    light_chara.get_diffuse(shader_data.g_light_env_chara_diffuse);
    light_chara.get_specular(shader_data.g_light_env_chara_specular);
    particle_scene_ubo.WriteMemory(shader_data);

    shaders_ft.set(SHADER_FT_PARTICL);
    particle_scene_ubo.Bind(0);
    gl_state_bind_vertex_array(ptcl_vao);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, count);
    gl_state_bind_vertex_array(0);
}

void snow_particle_draw() {
    if (!stage_param_data_snow_current || !snow_particle_enable || !stage_param_data_snow_set)
        return;

    texture* tex = texture_storage_get_texture(snow_particle_tex_id);
    if (!tex)
        return;

    stage_param_snow* snow = stage_param_data_snow_current;
    draw_pass_set_camera(rctx_ptr);

    gl_state_enable_blend();
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(GL_FALSE);

    float_t point_attenuation = powf(tanf((float_t)rctx_ptr->camera->get_fov()
        * 0.5f * DEG_TO_RAD_FLOAT) * 3.4f, 2.0f) * 0.1f;

    snow_particle_scene_shader_data snow_scene = {};
    mat4 temp;
    mat4_transpose(&rctx_ptr->vp_mat, &temp);
    snow_scene.g_transform[0] = temp.row0;
    snow_scene.g_transform[1] = temp.row1;
    snow_scene.g_transform[2] = temp.row2;
    snow_scene.g_transform[3] = temp.row3;

    mat4_transpose(&rctx_ptr->view_mat, &temp);
    snow_scene.g_view_world_row2 = temp.row2;
    snow_scene.g_size_in_projection.x = 1.0f / (float_t)rctx_ptr->post_process.render_width;
    snow_scene.g_size_in_projection.y = 1.0f / (float_t)rctx_ptr->post_process.render_height;
    snow_scene.g_size_in_projection.z = snow_particle_size_min;
    snow_scene.g_size_in_projection.w = snow_particle_size_max;
    snow_scene.g_state_point_attenuation = { 0.0f, 0.0f, point_attenuation, 0.0f };
    snow_scene.g_range_scale.x = snow->range_gpu.x;
    snow_scene.g_range_scale.y = snow->range_gpu.y;
    snow_scene.g_range_scale.z = snow->range_gpu.z;
    snow_scene.g_range_offset.x = snow->offset_gpu.x - snow->range_gpu.x * 0.5f;
    snow_scene.g_range_offset.y = snow->offset_gpu.y;
    snow_scene.g_range_offset.z = snow->offset_gpu.z - snow->range_gpu.z * 0.5f;
    snow_particle_scene_ubo.WriteMemory(snow_scene);

    snow_particle_batch_shader_data snow_batch = {};
    snow_batch.g_color = snow->color;
    snow_batch.start_vertex_location.x = 0;
    snow_particle_batch_ubo.WriteMemory(snow_batch);

    gl_state_active_bind_texture_2d(0, tex->tex);
    gl_state_active_bind_texture_2d(1, rctx_ptr->post_process.rend_texture.depth_texture->tex);
    gl_state_bind_vertex_array(snow_vao);

    uniform_value[U_SNOW_PARTICLE] = 0;
    shaders_ft.set(SHADER_FT_SNOW_PT);
    snow_particle_scene_ubo.Bind(0);
    snow_particle_batch_ubo.Bind(1);

    snow_ssbo.Bind(0);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, snow->num_snow * 6);

    snow_fallen_ssbo.Bind(0);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, (GLsizei)(snow_ptcl_fallen_count * 6));

    uniform_value[U_SNOW_PARTICLE] = 1;
    shaders_ft.set(SHADER_FT_SNOW_PT);
    snow_particle_scene_ubo.Bind(0);
    snow_particle_batch_ubo.Bind(1);

    point_attenuation = powf(tanf((float_t)rctx_ptr->camera->get_fov()
        * 0.5f * DEG_TO_RAD_FLOAT) * 3.4f, 2.0f) * 0.06f;

    snow_scene.g_state_point_attenuation = { 0.0f, 0.0f, point_attenuation, 0.0f };
    snow_particle_scene_ubo.WriteMemory(snow_scene);

    snow_gpu_ssbo.Bind(0);

    int32_t count = snow->num_snow_gpu / 4;
    count = min_def(count, (int32_t)snow_ptcl_count / 4) * 6;

    int32_t first = 0;
    for (particle_data& i : snow_ptcl_gpu) {
        vec3 pos_offset = i.position / snow->range_gpu;
        vec4 color = snow->color;
        color.w *= i.alpha;

        snow_batch = {};
        snow_batch.g_pos_offset = { pos_offset.x, pos_offset.y, pos_offset.z, 0.0f };
        snow_batch.g_color = color;
        snow_batch.start_vertex_location.x = first;
        snow_particle_batch_ubo.WriteMemory(snow_batch);

        shaders_ft.draw_arrays(GL_TRIANGLES, 0, count);
        first += count;
    }

    gl_state_active_bind_texture_2d(1, 0);
    gl_state_active_bind_texture_2d(0, 0);

    gl_state_disable_depth_test();
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_disable_blend();
}

void star_catalog_draw() {
    star_catalog_data.draw();
}

void task_effect_init() {
    if (!task_effect_auth_3d)
        task_effect_auth_3d = new TaskEffectAuth3D;

    if (!task_effect_leaf)
        task_effect_leaf = new TaskEffectLeaf;

    if (!task_effect_snow)
        task_effect_snow = new TaskEffectSnow;

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

    if (!task_effect_star)
        task_effect_star = new TaskEffectStar;

    if (!task_effect_parent)
        task_effect_parent = new TaskEffectParent;

    if (!ripple_emit_data)
        ripple_emit_data = new ripple_emit;

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

    if (task_effect_snow) {
        delete task_effect_snow;
        task_effect_snow = 0;
    }

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

    if (ripple_emit_data) {
        delete ripple_emit_data;
        ripple_emit_data = 0;
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
        path.assign("rom/stage_param/");

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

particle_data::particle_data() : size(), alpha(), life_time() {

}

star_catalog_milky_way::star_catalog_milky_way() : vbo(), ebo(), vao(), restart_index(),
idx_count(), longitude_degs_10(), latitude_degs_10(), longitude_offset_degs_10(),
latitude_offset_degs_10(), latitude(), longitude(), uv_rec_scale_u(), uv_rec_scale_v() {
    reset();
}

star_catalog_milky_way::~star_catalog_milky_way() {

}

void star_catalog_milky_way::create_buffers(int32_t subdivs, float_t uv_rec_scale_u, float_t uv_rec_scale_v,
    int32_t longitude_degs_10, int32_t latitude_degs_10,
    float_t longitude_offset_degs_10, float_t latitude_offset_degs_10) {
    delete_buffers();

    const int32_t sectors_count = 2 * subdivs + 1;
    const int32_t vtx_count = (subdivs - 1) * sectors_count + 2;
    restart_index = (uint16_t)0xFFFFFFFF;

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);
    gl_state_bind_element_array_buffer(0);

    if (!vao)
        glGenVertexArrays(1, &vao);

    if (!vbo)
        glGenBuffers(1, &vbo);

    const float_t rec_longitude_degs_10 = 1.0f / (float_t)longitude_degs_10;
    const float_t rec_latitude_degs_10 = 1.0f / (float_t)latitude_degs_10;

    star_catalog_vertex* vtx_data = force_malloc<star_catalog_vertex>(vtx_count);
    vtx_data[0].position = { 0.0f, 1.0f, 0.0f };
    vtx_data[0].texcoord.x = 0.5f;
    vtx_data[0].texcoord.y = (latitude_offset_degs_10 + (1.0f / uv_rec_scale_v) * 90.0f) * rec_latitude_degs_10;
    size_t vtx = 1;
    if (subdivs - 1 > 0) {
        float_t rec_stack_step = 1.0f / (float_t)subdivs;
        float_t rec_sector_step = 1.0f / (float_t)(2 * subdivs);
        for (uint32_t i = subdivs - 1, j = 1; i; i--, j++) {
            float_t stack_angle = (float_t)M_PI_2 - (float_t)j * (float_t)M_PI * rec_stack_step;
            float_t xz = cosf(stack_angle);
            float_t y = sinf(stack_angle);
            float_t texcoord_y = (latitude_offset_degs_10 + stack_angle * (float_t)(1.0 / M_PI)
                * 180.0f * (float_t)(1.0 / uv_rec_scale_v)) * rec_latitude_degs_10;
            for (uint32_t k = sectors_count, l = 0; k; k--, l++, vtx++) {
                float_t sector_angle = (float_t)l * (float_t)(2.0 * M_PI) * rec_sector_step;
                vtx_data[vtx].position.x = sinf(sector_angle) * xz;
                vtx_data[vtx].position.y = y;
                vtx_data[vtx].position.z = cosf(sector_angle) * xz;
                vtx_data[vtx].texcoord.x = (longitude_offset_degs_10 + (float_t)l * rec_sector_step
                    * 360.0f * (1.0f / uv_rec_scale_u)) * rec_longitude_degs_10;
                vtx_data[vtx].texcoord.y = texcoord_y;
            }
        }
    }
    vtx_data[vtx].position = { 0.0f, -1.0f, 0.0f };
    vtx_data[vtx].texcoord.x = 1.0f;
    vtx_data[vtx].texcoord.y = (latitude_offset_degs_10 - (1.0f / uv_rec_scale_v) * 90.0f) * rec_latitude_degs_10;

    gl_state_bind_array_buffer(vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(star_catalog_vertex) * vtx_count), vtx_data, 0);
    else
        glBufferData(GL_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(star_catalog_vertex) * vtx_count), vtx_data, GL_STATIC_DRAW);
    free_def(vtx_data);

    static const GLsizei buffer_size = sizeof(star_catalog_vertex);

    gl_state_bind_vertex_array(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(star_catalog_vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(star_catalog_vertex, texcoord));

    const uint16_t first_vertex = 0;
    const uint16_t last_vertex = (uint16_t)(vtx_count - 1);
    uint16_t restart_index = this->restart_index;

    const int32_t ebo_count = (subdivs - 2) * (2 * sectors_count + 1) + 8 * sectors_count - 8;
    idx_count = ebo_count;
    uint16_t* ebo_data = force_malloc<uint16_t>(ebo_count);
    size_t idx = 0;
    for (uint32_t i = sectors_count - 1, j = 1; i; i--, j++, idx += 4) {
        ebo_data[idx + 0] = first_vertex;
        ebo_data[idx + 1] = (uint16_t)(j + 1);
        ebo_data[idx + 2] = (uint16_t)j;
        ebo_data[idx + 3] = restart_index;
    }

    uint16_t v37 = 1;
    for (uint32_t i = subdivs - 2; i; i--, idx++) {
        for (uint32_t j = sectors_count; j; j--, idx += 2, v37++) {
            ebo_data[idx + 0] = (uint16_t)(v37 + sectors_count);
            ebo_data[idx + 1] = (uint16_t)(v37);
        }
        ebo_data[idx] = restart_index;
    }

    for (uint32_t i = sectors_count - 1; i; i--, idx += 4, v37++) {
        ebo_data[idx + 0] = last_vertex;
        ebo_data[idx + 1] = (uint16_t)v37;
        ebo_data[idx + 2] = (uint16_t)(v37 + 1);
        ebo_data[idx + 3] = restart_index;
    }

    if (!ebo)
        glGenBuffers(1, &ebo);

    gl_state_bind_element_array_buffer(ebo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint16_t) * ebo_count), ebo_data, 0);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(sizeof(uint16_t) * ebo_count), ebo_data, GL_STATIC_DRAW);
    free_def(ebo_data);

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);
    gl_state_bind_element_array_buffer(0);
}

void star_catalog_milky_way::create_default_sphere() {
    create_buffers(16, uv_rec_scale_u, uv_rec_scale_v, longitude_degs_10, latitude_degs_10,
        longitude_offset_degs_10, latitude_offset_degs_10);
}

void star_catalog_milky_way::delete_buffers() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

void star_catalog_milky_way::draw(const mat4& vp, const mat4& mat, texture* tex, GL::UniformBuffer& scene_ubo) {
    if (!vao)
        return;

    mat4 latitude_mat;
    mat4 longitude_mat;
    mat4_rotate_x(latitude * DEG_TO_RAD_FLOAT, &latitude_mat);
    mat4_rotate_y(longitude * DEG_TO_RAD_FLOAT, &longitude_mat);

    mat4 model;
    mat4_mul(&latitude_mat, &longitude_mat, &model);

    const float_t pitch_forward = -0.50503153f;
    const float_t yaw_forward = 4.6496463f;
    const float_t pitch_up = 0.47347879f;
    const float_t yaw_up = 3.3660336f;

    vec3 forward;
    vec3 up;
    forward.x = sinf(yaw_forward) * cosf(pitch_forward);
    forward.y = sinf(pitch_forward);
    forward.z = cosf(yaw_forward) * cosf(pitch_forward);
    up.x = sinf(yaw_up) * cosf(pitch_up);
    up.y = sinf(pitch_up);
    up.z = cosf(yaw_up) * cosf(pitch_up);

    mat4 view = mat4_identity;
    *(vec3*)&view.row0 = vec3::cross(up, forward);
    *(vec3*)&view.row1 = up;
    *(vec3*)&view.row2 = forward;

    mat4_mul(&model, &view, &model);
    mat4_mul(&model, &mat, &model);

    star_catalog_scene_shader_data scene_shader_data = {};
    mat4 temp;
    mat4_mul(&model, &vp, &temp);
    mat4_transpose(&temp, &temp);
    scene_shader_data.g_transform[0] = temp.row0;
    scene_shader_data.g_transform[1] = temp.row1;
    scene_shader_data.g_transform[2] = temp.row2;
    scene_shader_data.g_transform[3] = temp.row3;
    scene_ubo.WriteMemory(scene_shader_data);

    gl_state_enable_cull_face();
    gl_state_disable_blend();
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_active_bind_texture_2d(0, tex->tex);
    gl_state_enable_primitive_restart();
    gl_state_set_primitive_restart_index(restart_index);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uniform_value[U_STAR] = 1;
    shaders_ft.set(SHADER_FT_STAR);

    gl_state_bind_vertex_array(vao);
    scene_ubo.Bind(0);
    shaders_ft.draw_elements(GL_TRIANGLE_STRIP, idx_count, GL_UNSIGNED_SHORT, 0);
    gl_state_bind_vertex_array(0);

    gl_state_active_bind_texture_2d(0, 0);
    gl_state_disable_primitive_restart();
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_enable_blend();
    gl_state_disable_cull_face();


}

void star_catalog_milky_way::reset() {
    longitude_degs_10 = 3600;
    latitude_degs_10 = 1800;
    longitude_offset_degs_10 = 1800.9501f;
    latitude_offset_degs_10 = 900.95001f;
    latitude = 0.0f;
    longitude = 0.0f;
    uv_rec_scale_u = -0.1f;
    uv_rec_scale_v = 0.1f;
}

star_catalog_vertex::star_catalog_vertex() {

}

star_catalog::star_catalog() : vao(), stage_param_data_ptr(), star_count(), star_b_count() {
    random = false;
    enable = true;
    star_tex = -1;
    star_b_tex = -1;
    milky_way_tex_id = -1;
}

star_catalog::~star_catalog() {
    free();
}

void star_catalog::draw() {
    if (!stage_param_data_ptr || !enable)
        return;

    texture* star_tex = texture_storage_get_texture(this->star_tex);
    texture* star_b_tex = texture_storage_get_texture(this->star_b_tex);
    if (!star_tex || !star_b_tex)
        return;

    mat4 model;
    stage_param_star::get_mat(model,
        stage_param_data.observer_north_latitude_deg * DEG_TO_RAD_FLOAT,
        stage_param_data.observer_east_longitude_deg * DEG_TO_RAD_FLOAT,
        stage_param_data.utc,
        stage_param_data.rotation_y_deg * DEG_TO_RAD_FLOAT);

    const mat4& view = rctx_ptr->view_mat;
    model.row3.x = vec3::dot(-*(vec3*)&view.row0, *(vec3*)&view.row3) + model.row3.x;
    model.row3.y = vec3::dot(-*(vec3*)&view.row1, *(vec3*)&view.row3) + model.row3.y;
    model.row3.z = vec3::dot(-*(vec3*)&view.row2, *(vec3*)&view.row3) + model.row3.z;

    mat4 proj = rctx_ptr->proj_mat;
    proj.row0.z = proj.row0.w;
    proj.row1.z = proj.row1.w;
    proj.row2.z = proj.row2.w;
    proj.row3.z = proj.row3.w;

    mat4 vp;
    mat4_mul(&rctx_ptr->view_mat, &proj, &vp);

    texture* milky_way_tex = texture_storage_get_texture(milky_way_tex_id);
    if (milky_way_tex)
        milky_way.draw(vp, model, milky_way_tex, scene_ubo);

    if (!vao)
        return;

    star_catalog_scene_shader_data scene_shader_data = {};
    mat4 temp;
    mat4_mul(&model, &vp, &temp);
    mat4_transpose(&temp, &temp);
    scene_shader_data.g_transform[0] = temp.row0;
    scene_shader_data.g_transform[1] = temp.row1;
    scene_shader_data.g_transform[2] = temp.row2;
    scene_shader_data.g_transform[3] = temp.row3;
    scene_ubo.WriteMemory(scene_shader_data);

    uniform_value[U_STAR] = 0;
    shaders_ft.set(SHADER_FT_STAR);
    gl_state_enable_blend();
    gl_state_set_blend_func(GL_ONE, GL_ONE);
    gl_state_enable_depth_test();
    gl_state_set_depth_mask(false);
    gl_state_bind_vertex_array(vao);
    for (int32_t i = 0; i < 2; i++) {
        stage_param_star_modifiers& modifiers = stage_param_data.modifiers[i];

        star_catalog_batch_shader_data batch_shader_data = {};
        batch_shader_data.g_size_in_projection = {
            (float_t)(1.0 / 1280.0), (float_t)(1.0 / 720.0), 0.0f, modifiers.size_max
        };
        batch_shader_data.g_modifiers = {
            modifiers.color_scale, modifiers.color_scale * modifiers.offset_scale,
            modifiers.pos_scale, modifiers.pos_scale * modifiers.offset_scale
        };
        batch_shader_data.g_thresholds = { modifiers.threshold * modifiers.pos_scale, 0.0f, 0.0f, 0.0f };
        batch_ubo.WriteMemory(batch_shader_data);

        scene_ubo.Bind(0);
        batch_ubo.Bind(1);
        stars_ssbo.Bind(0);
        if (i) {
            gl_state_active_bind_texture_2d(0, star_b_tex->tex);
            shaders_ft.draw_arrays(GL_TRIANGLES, 0, star_b_count * 6);
        }
        else {
            gl_state_active_bind_texture_2d(0, star_tex->tex);
            shaders_ft.draw_arrays(GL_TRIANGLES, 0, star_count * 6);
        }
    }
    gl_state_active_bind_texture_2d(0, 0);
    gl_state_bind_vertex_array(0);

}

void star_catalog::free() {
    stars_ssbo.Destroy();

    batch_ubo.Destroy();
    scene_ubo.Destroy();

    star_catalog_data.file_handler.reset();
    star_catalog_data.milky_way.delete_buffers();

    if (star_catalog_data.vao) {
        glDeleteVertexArrays(1, &star_catalog_data.vao);
        star_catalog_data.vao = 0;
    }

    star_catalog_data.stage_param_data_ptr = 0;
}

bool star_catalog::init() {
    if (file_handler.check_not_ready())
        return false;

    if (file_handler.get_data()) {
        std::vector<stars_buffer_data> vec;
        file_handler.get_size();
        const void* data = file_handler.get_data();
        star_catalog::parse_data(vec, (size_t)data);
        std::sort(vec.begin(), vec.end(), [](stars_buffer_data a, stars_buffer_data b) {
            return a.size > b.size;
        });

        stars_buffer_data* stars_data = vec.data();
        size_t length = vec.size();
        size_t temp;
        while (length > 0)
            if (stars_data[temp = length / 2].size <= 0.0f)
                length = temp;
            else {
                stars_data += temp + 1;
                length -= temp + 1;
            }

        if (stars_data != vec.data() + vec.size())
            vec.resize(stars_data - vec.data());

        stars_data = vec.data();
        length = vec.size();
        while (length > 0)
            if (stars_data[temp = length / 2].size <= 2.0f)
                length = temp;
            else {
                stars_data += temp + 1;
                length -= temp + 1;
            }

        if (stars_data != vec.data() + vec.size())
            star_catalog_data.star_b_count = (int32_t)(stars_data - vec.data());
        else
            star_catalog_data.star_b_count = (int32_t)vec.size();

        file_handler.reset();

        star_count = (int32_t)vec.size();

        stars_ssbo.Create(sizeof(stars_buffer_data) * vec.size(), vec.data());

        if (!vao)
            glGenVertexArrays(1, &vao);
    }

    milky_way.create_default_sphere();

    scene_ubo.Create(sizeof(star_catalog_scene_shader_data));
    batch_ubo.Create(sizeof(star_catalog_batch_shader_data));
    return true;
}

void star_catalog::set_stage_param_data(stage_param_star* value) {
    stage_param_data_ptr = value;
    if (value) {
        data_struct* aft_data = &data_list[DATA_AFT];
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

        stage_param_data = *value;
        stage_param_data.utc.get_current_time();
        stage_param_data.observer_north_latitude_deg = 43.065f;
        stage_param_data.observer_east_longitude_deg = 141.35f;

        star_tex = aft_tex_db->get_texture_id("F_DIVA_EFF00_HR_STAR");
        star_b_tex = aft_tex_db->get_texture_id("F_DIVA_EFF00_HR_STAR_B");
        milky_way_tex_id = aft_tex_db->get_texture_id(value->milky_way_texture_name.c_str());
    }
    else {
        star_tex = -1;
        star_b_tex = -1;
        milky_way_tex_id = -1;
    }
}

void star_catalog::parse_data(std::vector<stars_buffer_data>& vec, size_t data) {
    size_t count = *(int32_t*)data;
    vec.clear();
    vec.resize(count);
    if (count > 0)
        memcpy(vec.data(), (stars_buffer_data*)(data + 4), sizeof(stars_buffer_data) * count);
}

stars_buffer_data::stars_buffer_data() : size() {

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
            app::TaskWork::AddTask(t, name);
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
            i.second->DelTask();
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
        const ::stage_data_modern* stage_data = this->stage_data->get_stage_data_modern(i);
        if (!stage_data)
            continue;

        const stage_effects_modern* effects = stage_data->effects_init
            ? &stage_data->effects : &stage_effects_modern_default;

        field_68.push_back(i, struc_608(effects));

        for (uint32_t j : effects->field_0)
            if (j == hash_murmurhash_empty || j == hash_murmurhash_null || j == -1)
                break;
            else
                obj_set_ids.push_back(j);
    }

    prj::sort_unique(obj_set_ids);

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

    obj_set_ids.push_back(aft_obj_db->get_object_set_id("EFFCMN"));

    for (int32_t i : stage_indices) {
        const ::stage_data* stage_data = aft_stage_data->get_stage_data(i);
        if (!stage_data)
            continue;

        const stage_effects* effects = stage_data->effects_init
            ? &stage_data->effects : &stage_effects_default;

        field_68.push_back(i, struc_608(effects));

        for (int32_t j : effects->field_0)
            if (j == -1)
                break;
            else
                obj_set_ids.push_back(j);
    }

    prj::sort_unique(obj_set_ids);

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
    texture* tex = texture_storage_get_texture(data->tex_id);
    if (tex)
        gl_state_active_bind_texture_2d(0, tex->tex);
    gl_state_bind_vertex_array(data->vao);
    data->ssbo.Bind(0);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, data->num_vtx);
    gl_state_disable_blend();
}

static void draw_ripple_emit(render_context* rctx, struc_101* data) {
    gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

    {
        size_t count = data->count;
        vec3* vertex = data->vertex;
        color4u8* color = data->color;

        vec3* vtx_data = force_malloc<vec3>(count);

        for (size_t i = count; i; i--, vtx_data++, vertex++, color++)
            *vtx_data = { vertex->x, vertex->z, (float_t)color->a * (float_t)(1.0 / 255.0) };

        vtx_data -= count;

        ripple_emit_ssbo.WriteMemory(0, sizeof(vec3) * count, vtx_data);

        free_def(vtx_data);
    }

    int32_t size = (int32_t)(data->size + 0.5f);

    RenderTexture& rt = rctx->render_manager.get_render_texture(
        ripple_emit_data->use_float_ripplemap ? 2 : 5);
    int32_t width = rt.color_texture->width;
    int32_t height = rt.color_texture->height;

    ripple_emit_scene_shader_data shader_data = {};
    shader_data.g_size_in_projection = {
        (float_t)size / (float_t)width,
        (float_t)size / (float_t)height,
        0.0f, 0.0f
    };
    shader_data.g_transform.x = (float_t)width / (float_t)(width - 2);
    shader_data.g_transform.y = (float_t)height / (float_t)(height - 2);
    shader_data.g_transform.z = 0.0079498291f / (float_t)(width - 2);
    shader_data.g_transform.w = -0.0079498291f / (float_t)(height - 2);
    shader_data.g_framebuffer_size = {
        1.0f / (float_t)width,
        1.0f / (float_t)height,
        0.0f, 0.0f };
    ripple_emit_scene_ubo.WriteMemory(shader_data);

    uniform_value[U_RIPPLE] = data->ripple_uniform;
    uniform_value[U_RIPPLE_EMIT] = data->ripple_emit_uniform;

    gl_state_bind_vertex_array(ripple_emit_vao);
    shaders_ft.set(SHADER_FT_RIPEMIT);
    ripple_emit_scene_ubo.Bind(0);
    ripple_emit_ssbo.Bind(0);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, data->count * 6);

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

static void leaf_particle_init(bool change_stage) {
    leaf_particle_emit_timer = 0.0f;
    leaf_particle_emit_interval = stage_param_data_leaf_current->emit_interval;
    if (change_stage)
        return;

    leaf_particle_free();

    const size_t leaf_ptcl_vtx_count = leaf_ptcl_count * 0x08;

    leaf_ptcl_data = force_malloc<leaf_particle_data>(leaf_ptcl_count);

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
        (void*)offsetof(leaf_particle_vertex_data, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(leaf_particle_vertex_data, texcoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(leaf_particle_vertex_data, normal));

    size_t ebo_count = leaf_ptcl_vtx_count / 4 * 6;
    uint32_t* ebo_data = force_malloc<uint32_t>(ebo_count);
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

    leaf_particle_scene_ubo.Create(sizeof(leaf_particle_scene_shader_data));
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
    float_t delta_time = leaf->frame_speed_coef * (leaf_particle_delta_frame * (float_t)(1.0 / 60.0));

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

        mat3 mat;
        mat3_rotate_xyz(&data->rotation, &mat);

        vec3 t0;
        vec3 t1;
        vec3 t2;
        vec3 t3;
        mat3_transform_vector(&mat, &position[0], &t0);
        mat3_transform_vector(&mat, &position[1], &t1);
        mat3_transform_vector(&mat, &position[2], &t2);
        mat3_transform_vector(&mat, &position[3], &t3);
        vtx_data[0].position = pos + t0;
        vtx_data[1].position = pos + t1;
        vtx_data[2].position = pos + t2;
        vtx_data[3].position = pos + t3;

        mat3_transform_vector(&mat, &normal[0], &t0);
        mat3_transform_vector(&mat, &normal[1], &t1);
        mat3_transform_vector(&mat, &normal[2], &t2);
        mat3_transform_vector(&mat, &normal[3], &t3);
        vtx_data[0].normal = t0;
        vtx_data[1].normal = t1;
        vtx_data[2].normal = t2;
        vtx_data[3].normal = t3;
        data[0].normal = t0;

        if (split_tex) {
            const float_t u = i & 0x01 ? 1.0f : 0.0f;
            const float_t v = i & 0x02 ? 1.0f : 0.0f;

            float_t u0 = u * 0.5f;
            float_t v0 = v * 0.5f;
            float_t u1 = (u + 1.0f) * 0.5f;
            float_t v1 = (v + 1.0f) * 0.5f;

            vtx_data[0].texcoord = vec2(u0, v0);
            vtx_data[1].texcoord = vec2(u1, v0);
            vtx_data[2].texcoord = vec2(u1, v1);
            vtx_data[3].texcoord = vec2(u0, v1);
        }
        else {
            vtx_data[0].texcoord = 0.0f;
            vtx_data[1].texcoord = vec2(1.0f, 0.0f);
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

    leaf_particle_scene_ubo.Destroy();
}

static void particle_init(vec3* offset) {
    particle_free();

    const size_t ptcl_vtx_count = ptcl_count * 0x06;

    ptcl_data = force_malloc<particle_rot_data>(ptcl_count);

    particle_rot_data* data = ptcl_data;
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
        (void*)offsetof(particle_vertex_data, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, texcoord));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, buffer_size,
        (void*)offsetof(particle_vertex_data, normal));

    gl_state_bind_vertex_array(0);
    gl_state_bind_array_buffer(0);

    particle_scene_ubo.Create(sizeof(particle_scene_shader_data));
}

static void particle_ctrl() {
    if (!ptcl_data)
        return;

    vec3 wind = particle_wind;

    particle_rot_data* data = ptcl_data;
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

static int32_t particle_disp(particle_vertex_data* vtx_data, particle_rot_data* data, int32_t count) {
    int32_t vtx_count = 0;
    for (size_t i = 0; i < count; i++, data++) {
        if (!data->alive)
            continue;

        vec3 pos = data->position;

        mat3 mat;
        mat3_rotate_xyz(&data->rotation, &mat);

        float_t size = data->size;

        vec3 t0 = vec3( 0.0125f,  0.007216875f, 0.0f) * size;
        vec3 t1 = vec3(-0.0125f,  0.007216875f, 0.0f) * size;
        vec3 t2 = vec3( 0.0f   , -0.01443375f , 0.0f) * size;
        mat3_transform_vector(&mat, &t0, &t0);
        mat3_transform_vector(&mat, &t1, &t1);
        mat3_transform_vector(&mat, &t2, &t2);
        t0 += pos;
        t1 += pos;
        t2 += pos;

        vec3 normal = vec3(0.0f, 0.0f, 1.0f);
        mat3_transform_vector(&mat, &normal, &normal);
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

static particle_rot_data* particle_emit() {
    if (particle_count >= ptcl_count)
        return 0;

    for (int32_t i = 0; i < ptcl_count; i++) {
        particle_rot_data* data = &ptcl_data[particle_index++];
        if (particle_index >= ptcl_count)
            particle_index = 0;

        if (!data->alive) {
            *data = {};
            data->alive = true;
            particle_count++;
            return data;
        }
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
            particle_rot_data* data = particle_emit();
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
            particle_rot_data* data = particle_emit();
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

static void particle_kill(particle_rot_data* data) {
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

    particle_scene_ubo.Destroy();
}

static void rain_particle_init(bool change_stage) {
    vec3 velocity = stage_param_data_rain_current->velocity;
    vec3 vel_range = stage_param_data_rain_current->vel_range;

    for (int32_t i = 0; i < 8; i++) {
        particle_data& data = rain_ptcl_data[i];
        data.position.x = 0.0f;
        data.position.y = ((float_t)i * -12.5f) - 5.0f;
        data.position.z = 0.0f;
        data.size = 1.0f;
        data.alpha = 1.0f;
        data.life_time = 1;
        vec3 _velocity;
        _velocity.x = rand_state_array_get_float(4);
        _velocity.y = rand_state_array_get_float(4);
        _velocity.z = rand_state_array_get_float(4);
        data.velocity = (_velocity - 0.5f) * vel_range + velocity;
    }

    if (change_stage)
        return;

    rain_particle_free();

    if (!rain_vao)
        glGenVertexArrays(1, &rain_vao);

    vec3* vtx_data = force_malloc<vec3>(rain_ptcl_count);
    for (int32_t i = 0; i < rain_ptcl_count; i++) {
        vec3 position;
        position.x = rand_state_array_get_float(4);
        position.y = rand_state_array_get_float(4);
        position.z = rand_state_array_get_float(4);
        *vtx_data++ = position;
    }

    vtx_data -= rain_ptcl_count;

    rain_ssbo.Create(sizeof(vec3) * rain_ptcl_count, vtx_data);

    free_def(vtx_data);

    rain_particle_scene_ubo.Create(sizeof(rain_particle_scene_shader_data));
    rain_particle_batch_ubo.Create(sizeof(rain_particle_batch_shader_data));
}

static void rain_particle_ctrl() {
    if (!stage_param_data_rain_current)
        return;

    float_t delta_time = (float_t)(rain_particle_delta_frame * (1.0 / 60.0));

    for (int32_t i = 0; i < 8; i++) {
        particle_data& data = rain_ptcl_data[i];
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
    if (rain_vao) {
        glDeleteVertexArrays(1, &rain_vao);
        rain_vao = 0;
    }

    rain_ssbo.Destroy();

    rain_particle_scene_ubo.Destroy();
    rain_particle_batch_ubo.Destroy();
}

static void ripple_emit_init() {
    if (!ripple_vao)
        glGenVertexArrays(1, &ripple_vao);

    ripple_batch_ubo.Create(sizeof(ripple_batch_shader_data));
    ripple_scene_ubo.Create(sizeof(ripple_scene_shader_data));

    if (!ripple_emit_vao)
        glGenVertexArrays(1, &ripple_emit_vao);

    size_t max_count = 16;

    ripple_emit_ssbo.Create(sizeof(vec3) * max_count);

    ripple_emit_scene_ubo.Create(sizeof(ripple_emit_scene_shader_data));
}

static void ripple_emit_free() {
    if (ripple_vao) {
        glDeleteVertexArrays(1, &ripple_vao);
        ripple_vao = 0;
    }

    ripple_batch_ubo.Destroy();
    ripple_scene_ubo.Destroy();

    ripple_emit_ssbo.Destroy();

    if (ripple_emit_vao) {
        glDeleteVertexArrays(1, &ripple_emit_vao);
        ripple_emit_vao = 0;
    }

    ripple_emit_scene_ubo.Destroy();
}

static void snow_particle_init(bool change_stage) {
    stage_param_snow* snow = stage_param_data_snow_current;

    float_t colli_ground_y = stage_param_data_snow_current->colli_ground.y;

    snow_particle_fallen_count = 0;
    snow_particle_fallen_index = 0;

    int32_t render_height = rctx_ptr->post_process.render_height;
    float_t snow_particle_size = (float_t)render_height * (float_t)(1.0 / 720.0);
    snow_particle_size_min = snow_particle_size;
    snow_particle_size_mid = snow_particle_size * 31.0f;
    snow_particle_size_max = snow_particle_size * 63.0f;

    snow_particle_data_free();
    snow_particle_data_init();

    vec3 velocity = snow->velocity;
    vec3 vel_range = snow->vel_range;
    vec3 range = snow->range;
    vec3 offset = snow->offset;
    range.y = offset.y + range.y - max_def(offset.y, colli_ground_y);

    if (stage_param_data_snow_current->num_snow > 0) {
        particle_data* snow_ptcl = snow_ptcl_data;
        for (int32_t i = ((stage_param_data_snow_current->num_snow - 1) >> 1) + 1; i; i--, snow_ptcl += 2) {
            vec3 position;
            position.x = rand_state_array_get_float(4) - 0.5f;
            position.y = rand_state_array_get_float(4);
            position.z = rand_state_array_get_float(4) - 0.5f;
            snow_ptcl[0].position = position * range + offset;

            snow_ptcl[0].size = rand_state_array_get_float(4) * 0.4f + 0.3f;
            snow_ptcl[0].alpha = rand_state_array_get_float(4) * 0.5f + 0.4f;
            snow_ptcl[0].life_time = 1;

            vec3 _velocity;
            _velocity.x = rand_state_array_get_float(4);
            _velocity.y = rand_state_array_get_float(4);
            _velocity.z = rand_state_array_get_float(4);
            snow_ptcl[0].velocity = (_velocity - 0.5f) * vel_range + velocity;

            snow_ptcl[0].direction = 0.0f;

            snow_ptcl[1].size = ((rand_state_array_get_float(4) * 0.5f) + 0.3f) * snow_ptcl[0].size;
            snow_ptcl[1].alpha = (rand_state_array_get_float(4) + 1.0f) * 0.3f;

            _velocity.x = rand_state_array_get_float(4);
            _velocity.y = rand_state_array_get_float(4);
            _velocity.z = rand_state_array_get_float(4);
            snow_ptcl[1].velocity = (_velocity - 0.5f) * 0.035f * (snow_ptcl[1].size + snow_ptcl[0].size);

            snow_ptcl[0].size = snow_particle_size_mid * snow_ptcl[0].size;
            snow_ptcl[1].size = snow_particle_size_mid * snow_ptcl[1].size;
        }
    }

    for (int32_t i = 0; i < snow_ptcl_fallen_count; i++)
        snow_particle_data_kill_fallen(&snow_ptcl_fallen_data[i], false);

    for (particle_data& i : snow_ptcl_gpu) {
        i.position.x = 0.0f;
        i.position.y = (float_t)(&i - snow_ptcl_gpu) * -25.0f - 5.0f;
        i.position.z = 0.0f;
        i.alpha = 1.0;
        i.life_time = 1;

        vec3 _velocity;
        _velocity.x = rand_state_array_get_float(4);
        _velocity.y = rand_state_array_get_float(4);
        _velocity.z = rand_state_array_get_float(4);
        i.velocity = (_velocity - 0.5f) * vel_range + velocity;
        i.direction = 0.0f;
    }

    if (change_stage)
        return;

    snow_particle_free();

    if (!snow_vao)
        glGenVertexArrays(1, &snow_vao);

    snow_ssbo.Create(sizeof(snow_particle_vertex_data) * snow->num_snow);

    snow_particle_gpu_vertex_data* vtx_data = force_malloc<snow_particle_gpu_vertex_data>(snow_ptcl_count);

    for (int32_t i = 0; i < snow_ptcl_count; i++, vtx_data++) {
        vtx_data->position.x = rand_state_array_get_float(4);
        vtx_data->position.y = rand_state_array_get_float(4);
        vtx_data->position.z = rand_state_array_get_float(4);
        vtx_data->size = (rand_state_array_get_float(4) * 0.5f + 0.4f) * snow_particle_size_mid;
    }

    vtx_data -= snow_ptcl_count;

    snow_gpu_ssbo.Create(sizeof(snow_particle_gpu_vertex_data) * snow_ptcl_count, vtx_data);

    free_def(vtx_data);

    snow_fallen_ssbo.Create(sizeof(snow_particle_vertex_data) * snow_ptcl_fallen_count);

    snow_particle_scene_ubo.Create(sizeof(snow_particle_scene_shader_data));
    snow_particle_batch_ubo.Create(sizeof(snow_particle_batch_shader_data));
}

static void snow_particle_ctrl() {
    if (!stage_param_data_snow_current)
        return;

    stage_param_snow* snow = stage_param_data_snow_current;
    float_t colli_ground_y = snow->colli_ground.y + 0.03f;
    float_t offset_y = snow->offset.y;

    float_t delta_time = (float_t)(snow_particle_delta_frame * (1.0 / 60.0));

    vec4 _colli_ground_xz = {
        -snow->colli_ground.min_x,
        snow->colli_ground.max_x,
        -snow->colli_ground.min_z,
        snow->colli_ground.max_z
    };

    __m128 colli_ground_xz = vec4::load_xmm(_colli_ground_xz);

    if (snow->num_snow > 0) {
        particle_data* snow_ptcl = snow_ptcl_data;
        for (int32_t i = ((snow->num_snow - 1) >> 1) + 1; i; i--, snow_ptcl += 2) {
            vec3 direction;
            if (--snow_ptcl[0].life_time > 0)
                direction = snow_ptcl[0].direction;
            else {
                snow_ptcl[0].life_time = rand_state_array_get_int(15, 35, 4);
                direction = snow_particle_get_random_velocity();
                snow_ptcl[0].direction = direction;
            }

            vec3 velocity = (direction * delta_time + snow_ptcl[0].velocity) * 0.98f;
            vec3 position = velocity * delta_time + snow_ptcl[0].position;
            snow_ptcl[0].velocity = velocity;
            snow_ptcl[0].position = position;

            if (position.y < offset_y)
                snow_particle_data_reset(snow_ptcl);
            else if (colli_ground_y > position.y) {
                __m128 pos = vec3::load_xmm(position);
                if (!_mm_movemask_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(pos, pos, 0xA0),
                    vec4::load_xmm({ 1.0f, -1.0f, 1.0f, -1.0f })), colli_ground_xz))) {
                    snow_ptcl[0].position.y = colli_ground_y;
                    snow_particle_data_emit_fallen(snow_ptcl);
                    snow_particle_data_reset(snow_ptcl);
                }
            }

            snow_ptcl[1].position = snow_ptcl[0].position + snow_ptcl[1].velocity;
        }
    }

    for (particle_data& i : snow_ptcl_gpu) {
        vec3 direction;
        if (--i.life_time > 0)
            direction = i.direction;
        else {
            i.life_time = rand_state_array_get_int(0xFu, 0x23u, 4);
            direction = snow_particle_get_random_velocity();
            i.direction = direction;
        }

        vec3 velocity = (direction * delta_time + i.velocity) * 0.98f;
        vec3 position = velocity * delta_time + i.position;
        i.velocity = velocity;
        i.alpha = 1.0f;
        i.position = position;
        if (position.y > -1.0f)
            i.alpha = max_def(-position.y, 0.0f);
        else if (position.y < -99.0f) {
            i.alpha = max_def(position.y + 100.0f, 0.0f);
            if (position.y < -100.0f)
                i.position = 0.0f;
        }
    }

    particle_data* snow_ptcl_fallen = snow_ptcl_fallen_data;
    for (size_t i = snow->num_snow; i; i--, snow_ptcl_fallen++)
        if (snow_ptcl_fallen->size != 0.0f) {
            snow_ptcl_fallen->alpha *= 0.95f;
            if (snow_ptcl_fallen->alpha < 0.02f)
                snow_particle_data_kill_fallen(snow_ptcl_fallen, true);
        }

    snow_particle_vertex_data* vtx_data = (snow_particle_vertex_data*)snow_ssbo.MapMemory();
    if (vtx_data) {
        particle_data* snow_ptcl = snow_ptcl_data;
        for (int32_t i = snow->num_snow; i; i--, snow_ptcl++, vtx_data++) {
            vtx_data->position = snow_ptcl->position;
            vtx_data->size = snow_ptcl->size;
            vtx_data->alpha = snow_ptcl->alpha;
        }
    }
    snow_ssbo.UnmapMemory();

    snow_particle_vertex_data* fallen_vtx_data = (snow_particle_vertex_data*)snow_fallen_ssbo.MapMemory();
    if (fallen_vtx_data) {
        particle_data* snow_ptcl_fallen = snow_ptcl_fallen_data;
        for (size_t i = snow_ptcl_fallen_count; i; i--, snow_ptcl_fallen++, fallen_vtx_data++) {
            fallen_vtx_data->position = snow_ptcl_fallen->position;
            fallen_vtx_data->size = snow_ptcl_fallen->size;
            fallen_vtx_data->alpha = snow_ptcl_fallen->alpha;
        }
    }
    snow_fallen_ssbo.UnmapMemory();
}

static void snow_particle_data_init() {
    if (!snow_ptcl_data)
        snow_ptcl_data = force_malloc<particle_data>(stage_param_data_snow_current->num_snow);

    if (!snow_ptcl_fallen_data)
        snow_ptcl_fallen_data = force_malloc<particle_data>(snow_ptcl_fallen_count);
}

static void snow_particle_data_emit_fallen(particle_data* data) {
    for (int32_t i = 0; i < 6; i++) {
        particle_data* d = snow_particle_emit_fallen();
        if (!d)
            break;

        d->position.x = (rand_state_array_get_float(4) - 0.5f) * 0.05f + data->position.x;
        d->position.y = data->position.y;
        d->position.z = (rand_state_array_get_float(4) - 0.5f) * 0.05f + data->position.z;
        d->size = (rand_state_array_get_float(4) * 0.5f + 0.2f) * data->size;
        d->velocity = 0.0f;
        d->alpha = data->alpha * 0.60f;
    }
}

static void snow_particle_data_kill_fallen(particle_data* data, bool kill) {
    if (kill)
        --snow_particle_fallen_count;

    data->position = { 0.0f, -10.0f, 0.0f };
    data->size = 0.0f;
    data->alpha = 1.0f;
}

static void snow_particle_data_reset(particle_data* data) {
    stage_param_snow* snow = stage_param_data_snow_current;
    vec3 position;
    position.x = rand_state_array_get_float(4) - 0.5f;
    position.y = 1.0f;
    position.z = rand_state_array_get_float(4) - 0.5f;
    data->position = position * snow->range + snow->offset;

    data->life_time = 1;

    vec3 velocity;
    velocity.x = rand_state_array_get_float(4);
    velocity.y = rand_state_array_get_float(4);
    velocity.z = rand_state_array_get_float(4);
    data->velocity = (velocity - 0.5f) * snow->vel_range + snow->velocity;
    data->direction = 0.0f;
}

static void snow_particle_data_free() {
    if (snow_ptcl_data) {
        free(snow_ptcl_data);
        snow_ptcl_data = 0;
    }

    if (snow_ptcl_fallen_data) {
        free(snow_ptcl_fallen_data);
        snow_ptcl_fallen_data = 0;
    }
}

static particle_data* snow_particle_emit_fallen() {
    if (snow_particle_fallen_count >= snow_ptcl_fallen_count)
        return 0;

    for (int32_t i = 0; i < snow_ptcl_fallen_count; i++) {
        particle_data* data = &snow_ptcl_fallen_data[snow_particle_fallen_index++];
        if (snow_particle_fallen_index >= snow_ptcl_fallen_count)
            snow_particle_fallen_index = 0;

        if (data->size == 0.0f) {
            snow_particle_fallen_count++;
            return data;
        }
    }
    return 0;
}

static vec3 snow_particle_get_random_velocity() {
    stage_param_snow* snow = stage_param_data_snow_current;
    vec3 velocity;
    velocity.x = rand_state_array_get_float(4);
    velocity.y = rand_state_array_get_float(4);
    velocity.z = rand_state_array_get_float(4);
    return (velocity - 0.5f) * snow->vel_range + snow->velocity;
}

static void snow_particle_free() {
    if (snow_vao) {
        glDeleteVertexArrays(1, &snow_vao);
        snow_vao = 0;
    }

    snow_ssbo.Destroy();
    snow_gpu_ssbo.Destroy();
    snow_fallen_ssbo.Destroy();

    snow_particle_scene_ubo.Destroy();
    snow_particle_batch_ubo.Destroy();
}

static void sub_1403B6ED0(RenderTexture* a1, RenderTexture* a2, RenderTexture* a3, ripple_emit_params& params) {
    a1->Bind();
    if (a1->color_texture->internal_format == GL_RGBA32F
        || a1->color_texture->internal_format == GL_RGBA16F)
        uniform_value[U_RIPPLE] = 1;
    else
        uniform_value[U_RIPPLE] = 0;
    sub_1403B6F60(a1->color_texture->tex, a2->color_texture->tex, a3->color_texture->tex, params);
    gl_state_bind_framebuffer(0);
}

static void sub_1403B6F60(GLuint a1, GLuint a2, GLuint a3, ripple_emit_params& params) {
    if (!a1 || !a2 || !a3)
        return;

    texture_param tex_params[2];
    texture_params_get(0, 0, a1, &tex_params[0], a2, &tex_params[1]);

    int32_t width = tex_params[1].width;
    int32_t height = tex_params[1].height;

    GLint v43[4];
    glGetIntegerv(GL_VIEWPORT, v43);
    glViewport(1, 1, width - 2, height - 2);

    ripple_scene_shader_data ripple_scene = {};
    ripple_scene.g_transform = {
        params.speed / (float_t)width, params.speed / (float_t)height,
        (float_t)width / (float_t)(width - 2), (float_t)height / (float_t)(height - 2)
    };
    ripple_scene.g_texcoord = { 1.0f, 0.0f, 0.0f, 0.0f };
    ripple_scene_ubo.WriteMemory(ripple_scene);

    ripple_batch_shader_data ripple_batch = {};
    ripple_batch.g_params = { params.wake_attn, params.speed, params.field_8, params.field_C };
    ripple_batch_ubo.WriteMemory(ripple_batch);

    gl_state_bind_vertex_array(ripple_vao);
    shaders_ft.set(SHADER_FT_RIPPLE);
    ripple_scene_ubo.Bind(0);
    ripple_batch_ubo.Bind(1);
    gl_state_active_bind_texture_2d(0, a2);
    gl_state_active_bind_texture_2d(1, a3);
    shaders_ft.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    gl_state_active_bind_texture_2d(0, 0);
    gl_state_active_bind_texture_2d(1, 0);
    gl_state_active_texture(0);

    glViewport(v43[0], v43[1], v43[2], v43[3]);

    texture_params_restore(&tex_params[0], &tex_params[1]);
}
