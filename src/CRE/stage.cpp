/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../KKdLib/vec.hpp"
#include "rob/rob.hpp"
#include "effect.hpp"
#include "light_param.hpp"
#include "stage_param.hpp"
#include "render_context.hpp"

namespace stage_detail {
    static void TaskStage_CtrlInner(stage_detail::TaskStage* a1);
    static void TaskStage_DispShadow(stage_detail::TaskStage* a1);
    static stage* TaskStage_GetCurrentStage(stage_detail::TaskStage* a1);
    static stage* TaskStage_GetStage(stage_detail::TaskStage* a1, const task_stage_info stg_info);
    static task_stage_info TaskStage_GetTaskStageInfo(stage_detail::TaskStage* a1, size_t index);
    static task_stage_info TaskStage_GetCurrentStageInfo(stage_detail::TaskStage* a1);
    static void TaskStage_GetLoadedStageInfos(stage_detail::TaskStage* a1,
        std::vector<task_stage_info>& vec);
    static void TaskStage_Load(stage_detail::TaskStage* a1);
    static bool TaskStage_LoadTask(stage_detail::TaskStage* a1, const char* name);
    static void TaskStage_Reset(stage_detail::TaskStage* a1);
    static void TaskStage_SetStage(stage_detail::TaskStage* a1, const task_stage_info stg_info);
    static void TaskStage_TaskWindAppend(stage_detail::TaskStage* a1);
    static void TaskStage_Unload(stage_detail::TaskStage* a1);
}

static bool object_bounding_sphere_check_visibility_shadow(
    const obj_bounding_sphere* sphere, const mat4* view, const mat4* mat);
static bool object_bounding_sphere_check_visibility_shadow_chara(
    const obj_bounding_sphere* sphere, const mat4* view);
static bool object_bounding_sphere_check_visibility_shadow_stage(
    const obj_bounding_sphere* sphere, const mat4* view);

static bool stage_ctrl(stage* s);
static void stage_disp(stage* s);
static void stage_disp_shadow(stage* s);
static void stage_disp_shadow_object(object_info object, mat4* mat);
static void stage_free(stage* s);
static void stage_load(stage* s);
static void stage_reset(stage* s);
static void stage_set(stage* s, stage* other);
static void stage_set_by_stage_index(stage* s, int32_t stage_index, uint16_t stage_counter);

stage_detail::TaskStage* task_stage;

extern render_context* rctx_ptr;
extern bool task_stage_is_modern;

extern bool light_chara_ambient;
extern vec4 npr_cloth_spec_color;

static uint16_t stage_counter;

stage::stage() : index(), counter(), state(), stage_data(), stage_display(),
lens_flare(), ground(), ring(), sky(), auth_3d_loaded(), mat(), rot_y(), obj_set() {

}

stage_detail::TaskStage::TaskStage() : state(), current_stage(), stage_display(),
field_FB1(), field_FB2(), field_FB3(), field_FB4(), mat(), field_FF8() {

}

stage_detail::TaskStage:: ~TaskStage() {

}

bool stage_detail::TaskStage::init() {
    return true;
}

bool stage_detail::TaskStage::ctrl() {
    stage_detail::TaskStage_CtrlInner(this);

    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
        if (stages[i].index != -1 && stage_ctrl(&stages[i]))
            break;
    return false;
}

bool stage_detail::TaskStage::dest() {
    stage_detail::TaskStage_Unload(this);
    if (state)
        return false;
    stage_detail::TaskStage_Reset(this);
    return true;
}

void stage_detail::TaskStage::disp() {
    if (state != 6 || !stage_display)
        return;

    stage* s = stage_detail::TaskStage_GetCurrentStage(this);
    if (s)
        stage_disp(s);
}

task_stage_info::task_stage_info() {
    load_index = -1;
    load_counter = 0;
}

task_stage_info::task_stage_info(int16_t load_index, uint16_t load_counter) {
    this->load_index = load_index;
    this->load_counter = load_counter;
}

bool task_stage_info::check() const {
    return task_stage_get_stage(*this) != 0;
}

int32_t task_stage_info::get_stage_index() const {
    stage* stg = task_stage_get_stage(*this);
    if (stg)
        return stg->index;
    return -1;
}

void task_stage_info::set_ground(bool value) const {
    stage* stg = task_stage_get_stage(*this);
    if (stg)
        stg->ground = value;
}

void task_stage_info::set_ring(bool value) const {
    stage* stg = task_stage_get_stage(*this);
    if (stg)
        stg->ring = value;
}

void task_stage_info::set_sky(bool value) const {
    stage* stg = task_stage_get_stage(*this);
    if (stg)
        stg->sky = value;
}

void task_stage_info::set_stage() const {
    stage_detail::TaskStage_SetStage(task_stage, *this);
}

void task_stage_info::set_stage_display(bool value, bool effect_enable) const {
    stage* stg = task_stage_get_stage(*this);
    if (stg) {
        stg->stage_display = value;
        if (effect_enable)
            effect_manager_set_enable(value);
    }
}

void task_stage_init() {
    task_stage = new stage_detail::TaskStage;
}

bool task_stage_add_task(const char* name) {
    task_stage_is_modern = false;
    return stage_detail::TaskStage_LoadTask(task_stage, name);
}

bool task_stage_check_not_loaded() {
    return task_stage->load_stage_indices.size() || task_stage->state != 6;
}

void task_stage_current_set_ground(bool value) {
    task_stage_info stg_info = task_stage_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_ground(value);
}

void task_stage_current_set_ring(bool value) {
    task_stage_info stg_info = task_stage_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_ring(value);
}

void task_stage_current_set_sky(bool value) {
    task_stage_info stg_info = task_stage_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_sky(value);
}

void task_stage_current_set_stage_display(bool value, bool effect_enable) {
    task_stage_info stg_info = task_stage_get_current_stage_info();
    if (stg_info.check())
        stg_info.set_stage_display(value, effect_enable);
}

bool task_stage_del_task() {
    return task_stage->del();
}

void task_stage_disp_shadow() {
    stage_detail::TaskStage_DispShadow(task_stage);
}

stage* task_stage_get_current_stage() {
    return stage_detail::TaskStage_GetCurrentStage(task_stage);
}

int32_t task_stage_get_current_stage_index() {
    task_stage_info stg_info = task_stage_get_current_stage_info();
    if (stg_info.check())
        return stg_info.get_stage_index();
    return -1;
}

task_stage_info task_stage_get_current_stage_info() {
    return stage_detail::TaskStage_GetCurrentStageInfo(task_stage);
}

void task_stage_get_loaded_stage_infos(std::vector<task_stage_info>& vec) {
    stage_detail::TaskStage_GetLoadedStageInfos(task_stage, vec);
}

stage* task_stage_get_stage(const task_stage_info stg_info) {
    return stage_detail::TaskStage_GetStage(task_stage, stg_info);
}

void task_stage_set_mat(const mat4& mat) {
    task_stage->mat = mat;
}

void task_stage_set_stage_index(int32_t stage_index) {
    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    if (stage_index >= aft_stage_data->stage_data.size())
        return;

    std::vector<int32_t> stage_indices;
    stage_indices.push_back(stage_index);
    task_stage->load_stage_indices.insert(task_stage->load_stage_indices.end(),
        stage_indices.begin(), stage_indices.end());
}

void task_stage_set_stage_indices(const std::vector<int32_t>& stage_indices) {
    task_stage->load_stage_indices.insert(task_stage->load_stage_indices.end(),
        stage_indices.begin(), stage_indices.end());
}

void task_stage_free() {
    delete task_stage;
}

static void stage_detail::TaskStage_CtrlInner(stage_detail::TaskStage* a1) {
    if (a1->load_stage_indices.size())
        if (!a1->state) {
            a1->state = 3;
            int32_t stage_count = (int32_t)a1->load_stage_indices.size();
            stage_count = min_def(stage_count, TASK_STAGE_STAGE_COUNT);
            for (int32_t i = 0; i < stage_count; i++)
                if (a1->stages[i].index == -1)
                    stage_set_by_stage_index(&a1->stages[i], a1->load_stage_indices[i], stage_counter++);
            a1->stage_indices = a1->load_stage_indices;
            a1->load_stage_indices.clear();
        }
        else if (a1->state == 6) {
            stage_detail::TaskStage_Unload(a1);
            return;
        }

    if (a1->state >= 3 && a1->state <= 5)
        stage_detail::TaskStage_Load(a1);
    else if (a1->state >= 7 && a1->state <= 9)
        stage_detail::TaskStage_Unload(a1);
}

static void stage_detail::TaskStage_DispShadow(stage_detail::TaskStage* a1) {
    stage* s = stage_detail::TaskStage_GetCurrentStage(a1);
    if (s)
        stage_disp_shadow(s);
}

static stage* stage_detail::TaskStage_GetCurrentStage(stage_detail::TaskStage* a1) {
    if (a1->current_stage >= 0 && a1->current_stage <= TASK_STAGE_STAGE_COUNT)
        return &a1->stages[a1->current_stage];
    return 0;
}

static stage* stage_detail::TaskStage_GetStage(stage_detail::TaskStage* a1, const task_stage_info stg_info) {
    if (stg_info.load_index >= 0 && stg_info.load_index <= TASK_STAGE_STAGE_COUNT) {
        stage* s = &a1->stages[stg_info.load_index];
        if (s->index != -1 && s->counter == stg_info.load_counter)
            return s;
    }
    return 0;
}

static task_stage_info stage_detail::TaskStage_GetTaskStageInfo(stage_detail::TaskStage* a1, size_t index) {
    if (index >= 0 && index < TASK_STAGE_STAGE_COUNT)
        return { (int16_t)index, a1->stages[index].counter };
    return {};
}

static task_stage_info stage_detail::TaskStage_GetCurrentStageInfo(stage_detail::TaskStage* a1) {
    return stage_detail::TaskStage_GetTaskStageInfo(a1, a1->current_stage);
}

static void stage_detail::TaskStage_GetLoadedStageInfos(stage_detail::TaskStage* a1,
    std::vector<task_stage_info>& vec) {
    vec.clear();
    vec.shrink_to_fit();
    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
        if (a1->stages[i].index != -1)
            vec.push_back(stage_detail::TaskStage_GetTaskStageInfo(a1, i));
}

static void stage_detail::TaskStage_Load(stage_detail::TaskStage* a1) {
    switch (a1->state) {
    case 3: {
        //sub_140229F30("rom/STGTST_COLI.000.bin");
        light_param_data_storage_data_load_stages(a1->stage_indices);
        a1->state = 4;
    } break;
    case 4: {
        if (light_param_data_storage_data_load_file()/* || sub_14022A380()*/)
            break;

        bool wait_load = false;
        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            if (a1->stages[i].index != -1 && a1->stages[i].state != 6) {
                wait_load = true;
                break;
            }

        if (!wait_load) {
            effect_manager_set_stage_indices(a1->stage_indices);
            effect_manager_set_enable(a1->stage_display);
            a1->state = 5;
            //sub_14064DC10();
        }
    } break;
    case 5: {
        if (effect_manager_load())
            break;

            std::vector<task_stage_info> vec;
            stage_detail::TaskStage_GetLoadedStageInfos(a1, vec);
            if (vec.size())
                stage_detail::TaskStage_SetStage(a1, vec.front());
            a1->state = 6;
    } break;
    }
}

static bool stage_detail::TaskStage_LoadTask(stage_detail::TaskStage* a1, const char* name) {
    if (app::TaskWork::add_task(a1, name)) {
        stage_detail::TaskStage_Reset(a1);
        stage_detail::TaskStage_TaskWindAppend(a1);
        return false;
    }
    else {
        if (!app::TaskWork::has_task_dest(a1))
            a1->del();
        return true;
    }
}

static void stage_detail::TaskStage_Reset(stage_detail::TaskStage* a1) {
    a1->state = 0;
    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
       stage_reset(&a1->stages[i]);
    a1->current_stage = -1;
    a1->stage_indices.clear();
    a1->load_stage_indices.clear();
    a1->stage_display = true;
    a1->field_FB1 = true;
    a1->field_FB2 = true;
    a1->field_FB3 = true;
    a1->field_FB4 = true;
    a1->mat = mat4_identity;
    a1->field_FF8 = 0.0f;
}

static void stage_detail::TaskStage_SetStage(stage_detail::TaskStage* a1, const task_stage_info stg_info) {
    int16_t load_index = stg_info.load_index;
    stage* s = stage_detail::TaskStage_GetStage(a1, stg_info);
    stage* curr = stage_detail::TaskStage_GetCurrentStage(a1);
    stage_set(curr, s);
    a1->current_stage = load_index;
}

static void stage_detail::TaskStage_TaskWindAppend(stage_detail::TaskStage* a1) {
    app::TaskWork::add_task(task_wind, a1, "CHARA WIND");
}

static void stage_detail::TaskStage_Unload(stage_detail::TaskStage* a1) {
    if (!a1->state)
        return;

    if (a1->state == 3 || a1->state == 4) {
        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            stage_free(&a1->stages[i]);
        a1->state = 9;
    }
    else if (a1->state == 5 || a1->state == 6) {
        if (a1->state == 6)
            stage_detail::TaskStage_SetStage(a1, {});

        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            stage_free(&a1->stages[i]);
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
            a1->stage_indices.clear();
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

static bool stage_ctrl(stage* s) {
    if (s->state >= 1 && s->state <= 5) {
        stage_load(s);
        return true;
    }
    else if (s->state >= 7 && s->state <= 9) {
        stage_free(s);
        return true;
    }
    return false;
}

static void stage_disp(stage* s) {
    if (s->state != 6 || !s->stage_display)
        return;

    mdl::DispManager& disp_manager = *rctx_ptr->disp_manager;

    mat4 mat;
    mat4_rotate_y(s->rot_y, &mat);

    if (s->stage_data->object_ground.not_null() && s->ground)
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_ground);

    if (s->stage_data->object_ring.not_null() && s->ring)
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_ring);

    if (s->stage_data->object_reflect.not_null()) {
        disp_manager.set_obj_flags((mdl::ObjFlags)(mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFLECT));
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_reflect);
        disp_manager.set_obj_flags();
    }

    if (s->stage_data->object_refract.not_null()) {
        disp_manager.set_obj_flags((mdl::ObjFlags)(mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFRACT));
        disp_manager.entry_obj_by_object_info(&mat, s->stage_data->object_refract);
        disp_manager.set_obj_flags();
    }

    if (s->stage_data->object_sky.not_null() && s->sky) {
        mat4 t = s->mat;
        mat4_mul(&t, &mat, &t);
        disp_manager.entry_obj_by_object_info(&t, s->stage_data->object_sky);
    }

    if (s->stage_data->lens_flare_texture != -1 && s->lens_flare) {
        int32_t object_set_id = s->stage_data->object_set_id;
        rndr::Render* rend = &rctx_ptr->render;
        rend->lens_flare_texture = objset_info_storage_get_obj_set_texture(
            object_set_id, s->stage_data->lens_flare_texture);
        if (rend->lens_flare_texture) {
            rend->lens_shaft_texture = objset_info_storage_get_obj_set_texture(
                object_set_id, s->stage_data->lens_shaft_texture);
            rend->lens_ghost_texture = objset_info_storage_get_obj_set_texture(
                object_set_id, s->stage_data->lens_ghost_texture);
            rend->lens_ghost_count = 16;
            rend->lens_shaft_inv_scale = s->stage_data->lens_shaft_inv_scale;
        }
    }
}

static void stage_disp_shadow(stage* s) {
    if (s->state != 6 || !s->stage_display)
        return;

    mat4 mat;
    mat4_rotate_y(s->rot_y, &mat);
    if (s->stage_data->object_shadow.not_null())
        stage_disp_shadow_object(s->stage_data->object_shadow, &mat);
}

static void stage_disp_shadow_object(object_info object, mat4* mat) {
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

static void stage_free(stage* s) {
    if (!s->state)
        return;

    if (s->state >= 1 && s->state <= 6)
        s->state = 9;

    if (s->state < 7 || s->state > 9)
        return;

    rndr::RenderManager* render_manager = rctx_ptr->render_manager;
    if (s->stage_data->render_texture != -1)
        rctx_ptr->render.render_texture_free(
            texture_manager_get_texture(s->stage_data->render_texture), 0);

    if (s->stage_data->movie_texture != -1)
        rctx_ptr->render.movie_texture_free(
            texture_manager_get_texture(s->stage_data->movie_texture));

    render_manager->set_shadow_true();
    rctx_ptr->render.set_cam_blur(0);
    npr_cloth_spec_color.w = 1.0f;
    render_manager->reflect_texture_mask = false;
    render_manager->reflect_tone_curve = false;
    render_manager->field_31F = false;
    render_manager->light_stage_ambient = false;
    light_chara_ambient = false;

    if (s->auth_3d_loaded) {
        auth_3d_data_unload_category(s->stage_data->auth_3d_name.c_str());
        auth_3d_data_unload_category(s->stage_data->name.c_str());
        s->auth_3d_loaded = false;
    }

    objset_info_storage_unload_set(s->stage_data->object_set_id);
    if (s->obj_set != -1)
        objset_info_storage_unload_set(s->obj_set);

    s->obj_set = -1;
    s->index = -1;
    s->state = 0;
    s->counter = 0;
}

static void stage_load(stage* s) {
    if (s->state == 1) {
        data_struct* aft_data = &data_list[DATA_AFT];
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;
        objset_info_storage_load_set(aft_data, aft_obj_db, s->obj_set);
        s->state = 2;
    }
    else if (s->state == 2) {
        if (s->obj_set == -1 || !objset_info_storage_load_obj_set_check_not_read(s->obj_set))
            s->state = 3;
    }
    else if (s->state == 3) {
        data_struct* aft_data = &data_list[DATA_AFT];
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;
        objset_info_storage_load_set(aft_data, aft_obj_db, s->stage_data->object_set_id);

        auth_3d_data_load_category(s->stage_data->name.c_str());
        auth_3d_data_load_category(s->stage_data->auth_3d_name.c_str());
        s->auth_3d_loaded = true;
        s->state = 4;
    }
    else if (s->state == 4) {
        if (objset_info_storage_load_obj_set_check_not_read(s->stage_data->object_set_id)
            || !auth_3d_data_check_category_loaded(s->stage_data->name.c_str())
            || !auth_3d_data_check_category_loaded(s->stage_data->auth_3d_name.c_str()))
            return;

        if (s->stage_data->render_texture != -1)
            rctx_ptr->render.render_texture_set(
                texture_manager_get_texture(s->stage_data->render_texture), 0);

        if (s->stage_data->movie_texture != -1)
            rctx_ptr->render.movie_texture_set(
                texture_manager_get_texture(s->stage_data->movie_texture));
        s->state = 6;
    }
}

static void stage_reset(stage* s) {
    s->index = -1;
    s->counter = 0;
    s->state = 0;
    s->stage_data = 0;
    s->stage_display = true;
    s->lens_flare = true;
    s->ground = true;
    s->ring = true;
    s->sky = true;
    s->auth_3d_loaded = 0;
    s->mat = mat4_identity;
    s->rot_y = 0.0;
    s->obj_set = -1;
}

static void stage_set(stage* s, stage* other) {
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
        render_manager->reflect_texture_mask = false;
        render_manager->reflect_tone_curve = false;
        render_manager->field_31F = false;
        render_manager->light_stage_ambient = false;
        render_manager->set_shadow_true();
        rctx_ptr->render.set_cam_blur(0);
        npr_cloth_spec_color.w = 1.0f;
        render_manager->set_npr_param(0);
        light_chara_ambient = false;
    }

    if (other) {
        rctx_ptr->chara_reflect = other->stage_data->reflect_type != STAGE_DATA_REFLECT_DISABLE;
        render_manager->set_reflect_type(other->stage_data->reflect_type);
        rctx_ptr->chara_refract = other->stage_data->refract_enable;

        if (other->stage_data->reflect) {
            stage_data_reflect* reflect = &other->stage_data->reflect_data;
            render_manager->set_pass_sw(rndr::RND_PASSID_REFLECT, true);
            render_manager->set_reflect_blur(reflect->blur_num, (blur_filter_mode)reflect->blur_filter);
            render_manager->set_reflect(true);
            reflect_refract_resolution_mode reflect_resolution_mode = REFLECT_REFRACT_RESOLUTION_512x512;
            if (other->stage_data->reflect_type != STAGE_DATA_REFLECT_REFLECT_MAP)
                reflect_resolution_mode = (reflect_refract_resolution_mode)reflect->mode;
            render_manager->set_refract_resolution_mode(reflect_resolution_mode);
        }

        if (other->stage_data->refract) {
            stage_data_refract* refract = &other->stage_data->refract_data;
            render_manager->set_pass_sw(rndr::RND_PASSID_REFRACT, true);
            render_manager->set_refract_resolution_mode((reflect_refract_resolution_mode)refract->mode);
            render_manager->set_refract(true);
        }

        if (other->stage_data->flags & STAGE_DATA_REFLECT_TEXTURE_MASK)
            render_manager->reflect_texture_mask = true;
        if (other->stage_data->flags & STAGE_DATA_REFLECT_TONE_CURVE)
            render_manager->reflect_tone_curve = true;
        if (other->stage_data->flags & STAGE_DATA_LIGHT_STAGE_AMBIENT)
            render_manager->light_stage_ambient = true;
        if (other->stage_data->flags & STAGE_DATA_LIGHT_CHARA_AMBIENT)
            light_chara_ambient = true;
        if (stage_param_data_coli_data_get_pv_id() == 421)
            render_manager->field_31F = true;
        //sub_14064DC10();
        effect_manager_set_current_stage_index(other->index);
    }
    else
        effect_manager_set_current_stage_index(-1);

    if (other)
        light_param_data_storage_data_set_stage(other->index);
    else
        light_param_data_storage_data_set_default_light_param();

    effect_manager_set_frame(0);

    if (!pv_osage_manager_array_get_disp() && other)
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            rob_chara* rob_chr = rob_chara_array_get(i);
            if (rob_chr)
                rob_chr->set_stage_data_ring(other->index);
        }
}

static void stage_set_by_stage_index(stage* s, int32_t stage_index, uint16_t stage_counter) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    stage_reset(s);
    s->index = stage_index;
    s->counter = stage_counter;
    s->state = 1;

    if (stage_index >= aft_stage_data->stage_data.size()) {
        s->stage_data = 0;
        return;
    }

    s->stage_data = &aft_stage_data->stage_data[stage_index];

    const char* name = s->stage_data->name.c_str();
    size_t name_len = s->stage_data->name.size();

    char set_name[11];
    set_name[0] = 0;
    if (!str_utils_compare_length(name, name_len, "STGPV", 5)) {
        memcpy(set_name, name, 8);
        set_name[8] = 0;
    }
    else if (!str_utils_compare_length(name, name_len, "STGD2PV", 7)) {
        memcpy(set_name, name, 10);
        set_name[10] = 0;
    }

    s->obj_set = -1;
    if (set_name[0])
        s->obj_set = aft_obj_db->get_object_set_id(set_name);
}
