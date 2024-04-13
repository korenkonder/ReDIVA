/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage_modern.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../KKdLib/vec.hpp"
#include "rob/rob.hpp"
#include "effect.hpp"
#include "light_param.hpp"
#include "render_context.hpp"

namespace stage_detail {
    static void TaskStageModern_CtrlInner(stage_detail::TaskStageModern* a1);
    static void TaskStageModern_DispShadow(stage_detail::TaskStageModern* a1);
    static stage_modern* TaskStageModern_GetCurrentStage(stage_detail::TaskStageModern* a1);
    static stage_modern* TaskStageModern_GetStage(
        stage_detail::TaskStageModern* a1, const task_stage_modern_info stg_info);
    static task_stage_modern_info TaskStageModern_GetTaskStageModernInfo(
        stage_detail::TaskStageModern* a1, size_t index);
    static task_stage_modern_info TaskStageModern_GetCurrentStageInfo(stage_detail::TaskStageModern* a1);
    static void TaskStageModern_GetLoadedStageInfos(stage_detail::TaskStageModern* a1,
        std::vector<task_stage_modern_info>& vec);
    static void TaskStageModern_Load(stage_detail::TaskStageModern* a1);
    static bool TaskStageModern_LoadTask(stage_detail::TaskStageModern* a1, const char* name);
    static void TaskStageModern_Reset(stage_detail::TaskStageModern* a1);
    static void TaskStageModern_SetStage(
        stage_detail::TaskStageModern* a1, const task_stage_modern_info stg_info);
    static void TaskStageModern_TaskWindAppend(stage_detail::TaskStageModern* a1);
    static void TaskStageModern_Unload(stage_detail::TaskStageModern* a1);
}

static bool object_bounding_sphere_check_visibility_shadow(
    const obj_bounding_sphere* sphere, const mat4* view, const mat4* mat);
static bool object_bounding_sphere_check_visibility_shadow_chara(
    const obj_bounding_sphere* sphere, const mat4* view);
static bool object_bounding_sphere_check_visibility_shadow_stage(
    const obj_bounding_sphere* sphere, const mat4* view);

static bool stage_modern_ctrl(stage_modern* s, void* data, object_database* obj_db, texture_database* tex_db);
static void stage_modern_disp(stage_modern* s);
static void stage_modern_disp_shadow(stage_modern* s);
static void stage_modern_disp_shadow_object(object_info object, mat4* mat);
static void stage_modern_free(stage_modern* s);
static void stage_modern_load(stage_modern* s, void* data, object_database* obj_db, texture_database* tex_db);
static void stage_modern_reset(stage_modern* s);
static void stage_modern_set(stage_modern* s, stage_modern* other);
static void stage_modern_set_by_stage_hash(stage_modern* s, int32_t stage_hash,
    uint16_t stage_counter, stage_data_modern* stage_data);

stage_detail::TaskStageModern* task_stage_modern;

extern render_context* rctx_ptr;
extern bool task_stage_is_modern;

extern bool light_chara_ambient;
extern vec4 npr_cloth_spec_color;

static uint16_t stage_counter;

stage_modern::stage_modern() : counter(), state(), stage_data(),
stage_display(), ground(), sky(), auth_3d_loaded(), mat(), rot_y() {
    hash = hash_murmurhash_empty;
    obj_set_hash = hash_murmurhash_empty;
}

stage_detail::TaskStageModern::TaskStageModern() : state(), current_stage(), stage_display(), field_FB1(),
field_FB2(), field_FB3(), field_FB4(), mat(), field_FF8(), data(), obj_db(), tex_db(), stage_data() {

}

stage_detail::TaskStageModern:: ~TaskStageModern() {

}

bool stage_detail::TaskStageModern::init() {
    return true;
}

bool stage_detail::TaskStageModern::ctrl() {
    stage_detail::TaskStageModern_CtrlInner(this);

    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
        if (stages[i].hash != -1 && stages[i].hash != hash_murmurhash_empty
            && stage_modern_ctrl(&stages[i], data, obj_db, tex_db))
            break;
    return false;
}

bool stage_detail::TaskStageModern::dest() {
    stage_detail::TaskStageModern_Unload(this);
    if (state)
        return false;
    stage_detail::TaskStageModern_Reset(this);
    return true;
}

void stage_detail::TaskStageModern::disp() {
    if (state != 6 || !stage_display)
        return;

    stage_modern* s = stage_detail::TaskStageModern_GetCurrentStage(this);
    if (s)
        stage_modern_disp(s);
}

task_stage_modern_info::task_stage_modern_info() {
    load_index = -1;
    load_counter = 0;
}

task_stage_modern_info::task_stage_modern_info(int16_t load_index, uint16_t load_counter) {
    this->load_index = load_index;
    this->load_counter = load_counter;
}

bool task_stage_modern_info::check() const {
    return task_stage_modern_get_stage(*this) != 0;
}

uint32_t task_stage_modern_info::get_stage_hash() const {
    stage_modern* stg = task_stage_modern_get_stage(*this);
    if (stg)
        return stg->hash;
    return -1;
}

void task_stage_modern_info::set_ground(bool value) const {
    stage_modern* stg = task_stage_modern_get_stage(*this);
    if (stg)
        stg->ground = value;
}

void task_stage_modern_info::set_sky(bool value) const {
    stage_modern* stg = task_stage_modern_get_stage(*this);
    if (stg)
        stg->sky = value;
}

void task_stage_modern_info::set_stage() const {
    stage_detail::TaskStageModern_SetStage(task_stage_modern, *this);
}

void task_stage_modern_info::set_stage_display(bool value, bool effect_enable) const {
    stage_modern* stg = task_stage_modern_get_stage(*this);
    if (stg) {
        stg->stage_display = value;
        if (effect_enable)
            effect_manager_set_enable(value);
    }
}

void task_stage_modern_init() {
    task_stage_modern = new stage_detail::TaskStageModern;
}

bool task_stage_modern_check_not_loaded() {
    return task_stage_modern->load_stage_hashes.size() || task_stage_modern->state != 6;
}

bool task_stage_modern_check_task_ready() {
    return app::TaskWork::check_task_ready(task_stage_modern);
}

void task_stage_modern_current_set_ground(bool value) {
    task_stage_modern_info stg_info = task_stage_modern_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_ground(value);
}

void task_stage_modern_current_set_sky(bool value) {
    task_stage_modern_info stg_info = task_stage_modern_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_sky(value);
}

void task_stage_modern_current_set_stage_display(bool value, bool effect_enable) {
    task_stage_modern_info stg_info = task_stage_modern_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_stage_display(value, effect_enable);
}

void task_stage_modern_disp_shadow() {
    stage_detail::TaskStageModern_DispShadow(task_stage_modern);
}

stage_modern* task_stage_modern_get_current_stage() {
    return stage_detail::TaskStageModern_GetCurrentStage(task_stage_modern);
}

uint32_t task_stage_modern_get_current_stage_hash() {
    task_stage_modern_info stg_info = task_stage_modern_get_current_stage_info();
    if (stg_info.check())
        return stg_info.get_stage_hash();
    return hash_murmurhash_empty;
}

task_stage_modern_info task_stage_modern_get_current_stage_info() {
    return stage_detail::TaskStageModern_GetCurrentStageInfo(task_stage_modern);
}

void task_stage_modern_get_loaded_stage_infos(std::vector<task_stage_modern_info>& vec) {
    stage_detail::TaskStageModern_GetLoadedStageInfos(task_stage_modern, vec);
}

stage_modern* task_stage_modern_get_stage(const task_stage_modern_info stg_info) {
    return stage_detail::TaskStageModern_GetStage(task_stage_modern, stg_info);
}

bool task_stage_modern_load_task(const char* name) {
    task_stage_is_modern = true;
    return stage_detail::TaskStageModern_LoadTask(task_stage_modern, name);
}

void task_stage_modern_set_data(void* data,
    object_database* obj_db, texture_database* tex_db, stage_database* stage_data) {
    task_stage_modern->data = data;
    task_stage_modern->obj_db = obj_db;
    task_stage_modern->tex_db = tex_db;
    task_stage_modern->stage_data = stage_data;
}

void task_stage_modern_set_mat(const mat4& mat) {
    task_stage_modern->mat = mat;
}

void task_stage_modern_set_stage_hash(uint32_t stage_hash, stage_data_modern* stg_data) {
    std::vector<uint32_t> stage_hashes;
    stage_hashes.push_back(stage_hash);
    task_stage_modern->load_stage_hashes.insert(task_stage_modern->load_stage_hashes.end(),
        stage_hashes.begin(), stage_hashes.end());

    std::vector<stage_data_modern*> stages_data;
    stages_data.push_back(stg_data);
    task_stage_modern->load_stages_data.insert(task_stage_modern->load_stages_data.end(),
        stages_data.begin(), stages_data.end());
}

void task_stage_modern_set_stage_hashes(std::vector<uint32_t>& stage_hashes,
    std::vector<stage_data_modern*>& load_stage_data) {
    task_stage_modern->load_stage_hashes.insert(task_stage_modern->load_stage_hashes.end(),
        stage_hashes.begin(), stage_hashes.end());
    task_stage_modern->load_stages_data.insert(task_stage_modern->load_stages_data.end(),
        load_stage_data.begin(), load_stage_data.end());
}

bool task_stage_modern_unload_task() {
    return task_stage_modern->del();
}

void task_stage_modern_free() {
    delete task_stage_modern;
}

static void stage_detail::TaskStageModern_CtrlInner(stage_detail::TaskStageModern* a1) {
    if (a1->load_stage_hashes.size())
        if (!a1->state) {
            a1->state = 3;
            int32_t stage_count = (int32_t)a1->load_stage_hashes.size();
            stage_count = min_def(stage_count, TASK_STAGE_STAGE_COUNT);
            for (int32_t i = 0; i < stage_count; i++)
                if (a1->stages[i].hash == -1 || a1->stages[i].hash == hash_murmurhash_empty)
                    stage_modern_set_by_stage_hash(&a1->stages[i],
                        a1->load_stage_hashes[i], stage_counter++, a1->load_stages_data[i]);
            a1->stage_hashes = a1->load_stage_hashes;
            a1->load_stage_hashes.clear();
            a1->load_stages_data.clear();
        }
        else if (a1->state == 6) {
            stage_detail::TaskStageModern_Unload(a1);
            return;
        }

    if (a1->state >= 3 && a1->state <= 5)
        stage_detail::TaskStageModern_Load(a1);
    else if (a1->state >= 7 && a1->state <= 9)
        stage_detail::TaskStageModern_Unload(a1);
}

static void stage_detail::TaskStageModern_DispShadow(stage_detail::TaskStageModern* a1) {
    stage_modern* s = stage_detail::TaskStageModern_GetCurrentStage(a1);
    if (s)
        stage_modern_disp_shadow(s);
}

static stage_modern* stage_detail::TaskStageModern_GetCurrentStage(stage_detail::TaskStageModern* a1) {
    if (a1->current_stage >= 0 && a1->current_stage <= TASK_STAGE_STAGE_COUNT)
        return &a1->stages[a1->current_stage];
    return 0;
}

static stage_modern* stage_detail::TaskStageModern_GetStage(
    stage_detail::TaskStageModern* a1, const task_stage_modern_info stg_info) {
    if (stg_info.load_index >= 0 && stg_info.load_index <= TASK_STAGE_STAGE_COUNT) {
        stage_modern* s = &a1->stages[stg_info.load_index];
        if (s->hash != -1 && s->hash != hash_murmurhash_empty
            && s->counter == stg_info.load_counter)
            return s;
    }
    return 0;
}

static task_stage_modern_info stage_detail::TaskStageModern_GetTaskStageModernInfo(
    stage_detail::TaskStageModern* a1, size_t index) {
    if (index >= 0 && index < TASK_STAGE_STAGE_COUNT)
        return { (int16_t)index, a1->stages[index].counter };
    return {};
}

static task_stage_modern_info stage_detail::TaskStageModern_GetCurrentStageInfo(stage_detail::TaskStageModern* a1) {
    return stage_detail::TaskStageModern_GetTaskStageModernInfo(a1, a1->current_stage);
}

static void stage_detail::TaskStageModern_GetLoadedStageInfos(stage_detail::TaskStageModern* a1,
    std::vector<task_stage_modern_info>& vec) {
    vec.clear();
    vec.shrink_to_fit();
    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
        if (a1->stages[i].hash != -1 && a1->stages[i].hash != hash_murmurhash_empty)
            vec.push_back(stage_detail::TaskStageModern_GetTaskStageModernInfo(a1, i));
}

static void stage_detail::TaskStageModern_Load(stage_detail::TaskStageModern* a1) {
    switch (a1->state) {
    case 3: {
        //sub_140229F30("rom/STGTST_COLI.000.bin");
        light_param_data_storage_data_load_stages(a1->stage_hashes, a1->stage_data);
        a1->state = 4;
    } break;
    case 4: {
        if (light_param_data_storage_data_load_file()/* || sub_14022A380()*/)
            break;

        bool wait_load = false;
        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            if (a1->stages[i].hash != -1 && a1->stages[i].hash != hash_murmurhash_empty
                && a1->stages[i].state != 6) {
                wait_load = true;
                break;
            }

        if (!wait_load) {
            effect_manager_set_stage_hashes(a1->stage_hashes);
            effect_manager_set_enable(a1->stage_display);
            a1->state = 5;
            //sub_14064DC10();
        }
    } break;
    case 5: {
        if (effect_manager_load())
            break;

        std::vector<task_stage_modern_info> vec;
        stage_detail::TaskStageModern_GetLoadedStageInfos(a1, vec);
        if (vec.size())
            stage_detail::TaskStageModern_SetStage(a1, vec.front());
        a1->state = 6;
    } break;
    }
}

static bool stage_detail::TaskStageModern_LoadTask(stage_detail::TaskStageModern* a1, const char* name) {
    if (app::TaskWork::add_task(a1, name)) {
        stage_detail::TaskStageModern_Reset(a1);
        stage_detail::TaskStageModern_TaskWindAppend(a1);
        return false;
    }
    else {
        if (!app::TaskWork::has_task_dest(a1))
            a1->del();
        return true;
    }
}

static void stage_detail::TaskStageModern_Reset(stage_detail::TaskStageModern* a1) {
    a1->state = 0;
    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
       stage_modern_reset(&a1->stages[i]);
    a1->current_stage = -1;
    a1->stage_hashes.clear();
    a1->load_stage_hashes.clear();
    a1->load_stages_data.clear();
    a1->stage_display = true;
    a1->field_FB1 = true;
    a1->field_FB2 = true;
    a1->field_FB3 = true;
    a1->field_FB4 = true;
    a1->mat = mat4_identity;
    a1->field_FF8 = 0.0f;
}

static void stage_detail::TaskStageModern_SetStage(
    stage_detail::TaskStageModern* a1, const task_stage_modern_info stg_info) {
    int16_t load_index = stg_info.load_index;
    stage_modern* s = stage_detail::TaskStageModern_GetStage(a1, stg_info);
    stage_modern* curr = stage_detail::TaskStageModern_GetCurrentStage(a1);
    stage_modern_set(curr, s);
    a1->current_stage = load_index;
}

static void stage_detail::TaskStageModern_TaskWindAppend(stage_detail::TaskStageModern* a1) {
    app::TaskWork::add_task(task_wind, a1, "CHARA WIND");
}

static void stage_detail::TaskStageModern_Unload(stage_detail::TaskStageModern* a1) {
    if (!a1->state)
        return;

    if (a1->state == 3 || a1->state == 4) {
        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            stage_modern_free(&a1->stages[i]);
        a1->state = 9;
    }
    else if (a1->state == 5 || a1->state == 6) {
        if (a1->state == 6)
            stage_detail::TaskStageModern_SetStage(a1, {});

        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            stage_modern_free(&a1->stages[i]);
        a1->state = 7;
    }

    if (a1->state == 7) {
        effect_manager_dest();
        a1->state = 8;
    }
    else if (a1->state == 8) {
        if (!effect_manager_unload())
            a1->state = 9;
    }
    else if (a1->state == 9) {
        bool v5 = 0;
        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            if (a1->stages[i].state == 6) {
                v5 = 1;
                break;
            }

        if (!v5) {
            //sub_140228ED0();
            light_param_data_storage_data_free_file_handlers();
            a1->stage_hashes.clear();
            a1->state = 0;
        }
    }
}

static bool object_bounding_sphere_check_visibility_shadow(
    const obj_bounding_sphere* sphere, const mat4* view, const mat4* mat) {
    vec3 center;
    mat4_transform_point(mat, &sphere->center, &center);
    mat4_transform_point(view, &center, &center);
    float_t radius = sphere->radius;

    Shadow* shad = shadow_ptr_get();
    float_t shadow_range = shad->get_shadow_range();
    if ((center.x + radius) < -shadow_range
        || (center.x - radius) > shadow_range
        || (center.y + radius) < -shadow_range
        || (center.y - radius) > shadow_range
        || (center.z - radius) > -shad->z_near
        || (center.z + radius) < -shad->z_far)
        return false;
    return true;
}

static bool object_bounding_sphere_check_visibility_shadow_chara(
    const obj_bounding_sphere* sphere, const mat4* view) {
    mat4 mat;
    Shadow* shad = shadow_ptr_get();
    mat4_look_at(&shad->view_point[0], &shad->interest[0], &mat);
    return object_bounding_sphere_check_visibility_shadow(sphere, view, &mat);
}

static bool object_bounding_sphere_check_visibility_shadow_stage(
    const obj_bounding_sphere* sphere, const mat4* view) {
    mat4 mat;
    Shadow* shad = shadow_ptr_get();
    mat4_look_at(&shad->view_point[1], &shad->interest[1], &mat);
    return object_bounding_sphere_check_visibility_shadow(sphere, view, &mat);
}

static bool stage_modern_ctrl(stage_modern* s, void* data, object_database* obj_db, texture_database* tex_db) {
    if (s->state >= 1 && s->state <= 5) {
        stage_modern_load(s, data, obj_db, tex_db);
        return true;
    }
    else if (s->state >= 7 && s->state <= 9) {
        stage_modern_free(s);
        return true;
    }
    return false;
}

static void stage_modern_disp(stage_modern* s) {
    if (s->state != 6 || !s->stage_display)
        return;

    mdl::DispManager& disp_manager = *rctx_ptr->disp_manager;

    mat4 mat;
    mat4_rotate_y(s->rot_y, &mat);

    if (s->stage_data->object_ground.not_null_modern() && s->ground)
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_ground);

    if (s->stage_data->object_reflect.not_null_modern()) {
        disp_manager.set_obj_flags((mdl::ObjFlags)(mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFRACT));
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_reflect);
        disp_manager.set_obj_flags();
    }

    if (s->stage_data->object_refract.not_null_modern()) {
        disp_manager.set_obj_flags((mdl::ObjFlags)(mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFRACT));
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_refract);
        disp_manager.set_obj_flags();
    }

    if (s->stage_data->object_sky.not_null_modern() && s->sky) {
        mat4 t = s->mat;
        mat4_mul(&t, &mat, &t);
        disp_manager.entry_obj_by_object_info(&t, s->stage_data->object_sky);
    }
}

static void stage_modern_disp_shadow(stage_modern* s) {
    if (s->state != 6 || !s->stage_display)
        return;

    mat4 mat;
    mat4_rotate_y(s->rot_y, &mat);
    if (s->stage_data->object_shadow.not_null_modern())
        stage_modern_disp_shadow_object(s->stage_data->object_shadow, &mat);
}

static void stage_modern_disp_shadow_object(object_info object, mat4* mat) {
    mdl::DispManager& disp_manager = *rctx_ptr->disp_manager;

    for (int32_t i = SHADOW_CHARA; i < SHADOW_MAX; i++) {
        disp_manager.set_shadow_type((shadow_type_enum)i);
        disp_manager.set_culling_finc(i == SHADOW_CHARA
            ? object_bounding_sphere_check_visibility_shadow_chara
            : object_bounding_sphere_check_visibility_shadow_stage);
        disp_manager.set_obj_flags((mdl::ObjFlags)(mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_SHADOW_OBJECT));
        disp_manager.entry_obj_by_object_info(mat, object);
    }

    disp_manager.set_obj_flags();
    disp_manager.set_culling_finc();
    disp_manager.set_shadow_type();
}

static void stage_modern_free(stage_modern* s) {
    if (!s->state)
        return;

    if (s->state >= 1 && s->state <= 6)
        s->state = 9;

    if (s->state < 7 || s->state > 9)
        return;

    rndr::RenderManager* render_manager = rctx_ptr->render_manager;
    if (s->stage_data->render_texture != -1 && s->stage_data->render_texture != hash_murmurhash_empty)
        rctx_ptr->render.render_texture_free(
            texture_manager_get_texture(s->stage_data->render_texture), 0);

    if (s->stage_data->movie_texture != -1 && s->stage_data->movie_texture != hash_murmurhash_empty)
        rctx_ptr->render.movie_texture_free(
            texture_manager_get_texture(s->stage_data->movie_texture));

    render_manager->set_shadow_true();
    rctx_ptr->render.set_cam_blur(0);
    npr_cloth_spec_color.w = 1.0f;
    render_manager->field_31D = false;
    render_manager->field_31E = false;
    render_manager->field_31F = false;
    render_manager->field_320 = false;
    light_chara_ambient = false;

    if (s->auth_3d_loaded) {
        auth_3d_data_unload_category(s->stage_data->auth_3d_name_hash);
        s->auth_3d_loaded = false;
    }

    object_storage_unload_set(s->stage_data->hash);
    if (s->obj_set_hash != -1 && s->obj_set_hash != hash_murmurhash_empty)
        object_storage_unload_set(s->obj_set_hash);

    s->obj_set_hash = -1;
    s->hash = hash_murmurhash_empty;
    s->state = 0;
    s->counter = 0;
}

static void stage_modern_load(stage_modern* s, void* data, object_database* obj_db, texture_database* tex_db) {
    if (s->state == 1) {
        object_storage_load_set_hash(data, s->obj_set_hash);
        s->state = 2;
    }
    else if (s->state == 2) {
        if (s->obj_set_hash == -1 || s->obj_set_hash == hash_murmurhash_empty
            || !object_storage_load_obj_set_check_not_read(s->obj_set_hash, obj_db, tex_db))
            s->state = 3;
    }
    else if (s->state == 3) {
        object_storage_load_set_hash(data, s->stage_data->hash);

        auth_3d_data_load_category(data, s->stage_data->auth_3d_name.c_str(),
            s->stage_data->auth_3d_name_hash);
        s->auth_3d_loaded = true;
        s->state = 4;
    }
    else if (s->state == 4) {
        if (object_storage_load_obj_set_check_not_read(s->stage_data->hash, obj_db, tex_db)
            || !auth_3d_data_check_category_loaded(s->stage_data->auth_3d_name_hash))
            return;

        if (s->stage_data->render_texture != -1 && s->stage_data->render_texture != hash_murmurhash_empty)
            rctx_ptr->render.render_texture_set(
                texture_manager_get_texture(s->stage_data->render_texture), 0);

        if (s->stage_data->movie_texture != -1 && s->stage_data->movie_texture != hash_murmurhash_empty)
            rctx_ptr->render.movie_texture_set(
                texture_manager_get_texture(s->stage_data->movie_texture));
        s->state = 6;
    }
}

static void stage_modern_reset(stage_modern* s) {
    s->hash = hash_murmurhash_empty;
    s->counter = 0;
    s->state = 0;
    s->stage_data = 0;
    s->stage_display = true;
    s->ground = true;
    s->sky = true;
    s->auth_3d_loaded = 0;
    s->mat = mat4_identity;
    s->rot_y = 0.0;
    s->obj_set_hash = hash_murmurhash_empty;
}

static void stage_modern_set(stage_modern* s, stage_modern* other) {
    if (other && s == other)
        return;

    rndr::RenderManager* render_manager = rctx_ptr->render_manager;
    if (s) {
        //flt_14CC925C0 = 0.0;
        //flt_140CB3704 = -1001.0;
        render_manager->set_pass_sw(rndr::RND_PASSID_REFLECT, false);
        render_manager->set_reflect(false);
        render_manager->set_pass_sw(rndr::RND_PASSID_REFRACT, false);
        render_manager->set_refract(true);
        render_manager->field_31D = false;
        render_manager->field_31E = false;
        render_manager->field_31F = false;
        render_manager->field_320 = false;
        render_manager->set_shadow_true();
        rctx_ptr->render.set_cam_blur(0);
        npr_cloth_spec_color.w = 1.0f;
        render_manager->set_npr_param(0);
        light_chara_ambient = false;
    }

    if (other) {
        //if (stru_14CC92630.pv_id == 421)
            //render_manager->field_31F = true;
        //sub_14064DC10();
        effect_manager_set_current_stage_hash(other->hash);
    }
    else
        effect_manager_set_current_stage_hash(hash_murmurhash_empty);

    if (other)
        light_param_data_storage_data_set_stage(other->hash);
    else
        light_param_data_storage_data_set_default_light_param();

    effect_manager_set_frame(0);

    //sub_140344180(0);

    if (pv_osage_manager_array_get_disp() && other)
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            rob_chara* rob_chr = rob_chara_array_get(i);
            //if (rob_chr)
                //rob_chara_set_stage_data_ring(rob_chr, &other->index);
        }
}

static void stage_modern_set_by_stage_hash(stage_modern* s, int32_t stage_hash,
    uint16_t stage_counter, stage_data_modern* stage_data) {
    stage_modern_reset(s);
    s->hash = stage_hash;
    s->counter = stage_counter;
    s->state = 1;

    s->stage_data = stage_data;

    const char* name = s->stage_data->name.c_str();
    size_t name_len = s->stage_data->name.size();

    char set_name[11];
    set_name[0] = 0;
    if (!strncmp(name, "STGPV", min_def(name_len, 5))) {
        memcpy(set_name, name, 8);
        set_name[8] = 0;
    }
    else if (!strncmp(name, "STGD2PV", min_def(name_len, 7))) {
        memcpy(set_name, name, 10);
        set_name[10] = 0;
    }

    s->obj_set_hash = hash_murmurhash_empty;
    if (set_name[0])
        s->obj_set_hash = hash_utf8_murmurhash(set_name);
}
