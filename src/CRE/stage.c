/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/str_utils.h"
#include "../KKdLib/vec.h"
#include "draw_task.h"
#include "light_param.h"
#include "render_context.h"
#include "rob.h"

namespace stage_detail {
    static void TaskStage_CtrlInner(stage_detail::TaskStage* a1);
    static void TaskStage_DispShadow(stage_detail::TaskStage* a1);
    static stage* TaskStage_GetCurrentStage(stage_detail::TaskStage* a1);
    static stage* TaskStage_GetStage(stage_detail::TaskStage* a1, task_stage_info stg_info);
    static void TaskStage_GetTaskStageInfo(stage_detail::TaskStage* a1,
        task_stage_info* stg_info, size_t index);
    static void TaskStage_GetCurrentStageInfo(stage_detail::TaskStage* a1,
        task_stage_info* stg_info);
    static void TaskStage_GetLoadedStageInfos(stage_detail::TaskStage* a1,
        std::vector<task_stage_info>* vec);
    static bool TaskStage_Load(stage_detail::TaskStage* a1, const char* name);
    static void TaskStage_Reset(stage_detail::TaskStage* a1);
    static void TaskStage_SetStage(stage_detail::TaskStage* a1, task_stage_info stg_info);
    static void TaskStage_TaskWindAppend(stage_detail::TaskStage* a1);
    static void TaskStage_Unload(stage_detail::TaskStage* a1);
}

static bool object_bounding_sphere_check_visibility_shadow(obj_bounding_sphere* sphere, camera* cam, mat4* mat);
static bool object_bounding_sphere_check_visibility_shadow_chara(obj_bounding_sphere* sphere, camera* cam);
static bool object_bounding_sphere_check_visibility_shadow_stage(obj_bounding_sphere* sphere, camera* cam);

static bool stage_ctrl(stage* s);
static void stage_disp(stage* s);
static void stage_disp_shadow(stage* s);
static void stage_disp_shadow_object(object_info object, mat4* mat);
static void stage_free(stage* s);
static void stage_load(stage* s);
static void stage_reset(stage* s);
static void stage_set(stage* s, stage* other);
static void stage_set_by_stage_index(stage* s, int32_t stage_index, uint16_t stage_counter);

static void task_stage_set_effect_display(task_stage_info* stg_info, bool value);
static void task_stage_set_ground(task_stage_info* stg_info, bool value);
static void task_stage_set_ring(task_stage_info* stg_info, bool value);
static void task_stage_set_sky(task_stage_info* stg_info, bool value);
static void task_stage_set_stage_display(task_stage_info* stg_info, bool value);

const task_stage_info task_stage_info_null = { -1, 0 };

stage_detail::TaskStage task_stage;
DtmStg dtm_stg;

extern render_context* rctx_ptr;

extern bool light_chara_ambient;
extern vec4 npr_spec_color;

static uint16_t stage_counter;

void dtm_stg_load(int32_t stage_index) {
    if (TaskWork::CheckTaskReady(&dtm_stg))
        return;

    if (TaskWork::CheckTaskReady(&dtm_stg)) {
        dtm_stg.stage_index = stage_index;
        dtm_stg.load_stage_index = stage_index;
    }
    TaskWork::AppendTask(&dtm_stg, "DATA_TEST_STAGE");
}

bool dtm_stg_unload() {
    return dtm_stg.SetDest();
}

bool task_stage_check_not_loaded() {
    return task_stage.load_stage_indices.size() || task_stage.state != 6;
}

void task_stage_current_set_effect_display(bool value) {
    task_stage_info stg_info;
    task_stage_get_current_stage_info(&stg_info);
    if (task_stage_has_stage_info(&stg_info))
        task_stage_set_effect_display(&stg_info, value);
}

void task_stage_current_set_ground(bool value) {
    task_stage_info stg_info;
    task_stage_get_current_stage_info(&stg_info);
    if (task_stage_has_stage_info(&stg_info))
        task_stage_set_ground(&stg_info, value);
}

void task_stage_current_set_ring(bool value) {
    task_stage_info stg_info;
    task_stage_get_current_stage_info(&stg_info);
    if (task_stage_has_stage_info(&stg_info))
        task_stage_set_ring(&stg_info, value);
}

void task_stage_current_set_sky(bool value) {
    task_stage_info stg_info;
    task_stage_get_current_stage_info(&stg_info);
    if (task_stage_has_stage_info(&stg_info))
        task_stage_set_sky(&stg_info, value);
}

void task_stage_current_set_stage_display(bool value) {
    task_stage_info stg_info;
    task_stage_get_current_stage_info(&stg_info);
    if (task_stage_has_stage_info(&stg_info))
        task_stage_set_stage_display(&stg_info, value);
}

void task_stage_disp_shadow() {
    stage_detail::TaskStage_DispShadow(&task_stage);
}

stage* task_stage_get_current_stage() {
    return stage_detail::TaskStage_GetCurrentStage(&task_stage);
}

void task_stage_get_current_stage_info(task_stage_info* stg_info) {
    stage_detail::TaskStage_GetCurrentStageInfo(&task_stage, stg_info);
}

void task_stage_get_loaded_stage_infos(std::vector<task_stage_info>* vec) {
    stage_detail::TaskStage_GetLoadedStageInfos(&task_stage, vec);
}

stage* task_stage_get_stage(task_stage_info stg_info) {
    return stage_detail::TaskStage_GetStage(&task_stage, stg_info);
}

bool task_stage_has_stage_info(task_stage_info* stg_info) {
    return task_stage_get_stage(*stg_info) != 0;
}

bool task_stage_load(char* name) {
    return stage_detail::TaskStage_Load(&task_stage, (const char*)name);
}

bool task_stage_load(const char* name) {
    return stage_detail::TaskStage_Load(&task_stage, name);
}

void task_stage_set_mat(mat4* mat) {
    task_stage.mat = *mat;
}

void task_stage_set_mat(mat4u* mat) {
    task_stage.mat = *mat;
}

void task_stage_set_stage(task_stage_info* stg_info) {
    stage_detail::TaskStage_SetStage(&task_stage, *stg_info);
}

void task_stage_set_stage_index(int32_t stage_index) {
    data_struct* data = rctx_ptr->data;
    stage_database* stage_data = &data->data_ft.stage_data;

    if (stage_index >= vector_old_length(stage_data->stage_data))
        return;

    std::vector<int32_t> stage_indices;
    stage_indices.push_back(stage_index);
    task_stage.load_stage_indices.insert(task_stage.load_stage_indices.end(),
        stage_indices.begin(), stage_indices.end());
}

void task_stage_set_stage_indices(std::vector<int32_t>* stage_indices) {
    task_stage.load_stage_indices.insert(task_stage.load_stage_indices.end(),
        stage_indices->begin(), stage_indices->end());
}

bool task_stage_unload() {
    return task_stage.SetDest();
}

static void stage_detail::TaskStage_CtrlInner(stage_detail::TaskStage* a1) {
    if (a1->load_stage_indices.size())
        if (!a1->state) {
            a1->state = 3;
            int32_t stage_count = (int32_t)a1->load_stage_indices.size();
            stage_count = min(stage_count, TASK_STAGE_STAGE_COUNT);
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

    if (a1->state == 3) {
        //sub_140229F30("rom/STGTST_COLI.000.bin");
        light_param_storage_data_load_stages(&a1->stage_indices);
        a1->state = 4;
    }
    else if (a1->state == 4 && !light_param_storage_data_load_file()/* && !sub_14022A380()*/) {
        bool v5 = 0;
        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            if (a1->stages[i].index != -1 && a1->stages[i].state != 6) {
                v5 = 1;
                break;
            }

        if (!v5) {
            //sub_140343B20(&a1->stage_indices);
            //task_stage_current_set_effect_display(a1->stage_display);
            a1->state = 5;
            //sub_14064DC10();
        }
    }
    else if (a1->state == 5/* && !sub_1403446E0()*/) {
        std::vector<task_stage_info> vec;
        stage_detail::TaskStage_GetLoadedStageInfos(a1, &vec);
        if (vec.size())
            stage_detail::TaskStage_SetStage(a1, vec[0]);
        a1->state = 6;
    }
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

static stage* stage_detail::TaskStage_GetStage(stage_detail::TaskStage* a1, task_stage_info stg_info) {
    if (stg_info.load_index >= 0 && stg_info.load_index <= TASK_STAGE_STAGE_COUNT) {
        stage* s = &a1->stages[stg_info.load_index];
        if (s->index != -1 && s->counter == stg_info.load_counter)
            return s;
    }
    return 0;
}

static void stage_detail::TaskStage_GetTaskStageInfo(stage_detail::TaskStage* a1,
    task_stage_info* stg_info, size_t index) {
    *stg_info = task_stage_info_null;
    if (index >= 0 && index < TASK_STAGE_STAGE_COUNT) {
        stg_info->load_index = (int16_t)index;
        stg_info->load_counter = a1->stages[index].counter;
    }
}

static void stage_detail::TaskStage_GetCurrentStageInfo(stage_detail::TaskStage* a1,
    task_stage_info* stg_info) {
    stage_detail::TaskStage_GetTaskStageInfo(a1, stg_info, a1->current_stage);
}

static void stage_detail::TaskStage_GetLoadedStageInfos(stage_detail::TaskStage* a1,
    std::vector<task_stage_info>* vec) {
    vec->clear();
    vec->shrink_to_fit();
    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
        if (a1->stages[i].index != -1) {
            task_stage_info stg_info;
            stage_detail::TaskStage_GetTaskStageInfo(a1, &stg_info, i);
            vec->push_back(stg_info);
        }
}

static bool stage_detail::TaskStage_Load(stage_detail::TaskStage* a1, const char* name) {
    if (TaskWork::AppendTask(a1, name)) {
        stage_detail::TaskStage_Reset(a1);
        stage_detail::TaskStage_TaskWindAppend(a1);
        return false;
    }
    else {
        if (!TaskWork::HasTaskDest(a1))
            a1->SetDest();
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
    a1->mat = mat4u_identity;
    a1->field_FF8 = 0.0f;
}

static void stage_detail::TaskStage_SetStage(stage_detail::TaskStage* a1, task_stage_info stg_info) {
    int16_t load_index = stg_info.load_index;
    stage* s = stage_detail::TaskStage_GetStage(a1, stg_info);
    stage* curr = stage_detail::TaskStage_GetCurrentStage(a1);
    stage_set(curr, s);
    a1->current_stage = load_index;
}

static void stage_detail::TaskStage_TaskWindAppend(stage_detail::TaskStage* a1) {
    TaskWork::AppendTask(&task_wind, a1, "CHARA WIND");
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
            stage_detail::TaskStage_SetStage(a1, task_stage_info_null);

        for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
            stage_free(&a1->stages[i]);
        a1->state = 7;
    }

    if (a1->state == 7) {
        //sub_140343C30();
        a1->state = 8;
    }
    else if (a1->state == 8) {
        //if (!sub_140344700())
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
            light_param_storage_data_free_file_handlers();
            a1->stage_indices.clear();
            a1->state = 0;
        }
    }
}

static bool object_bounding_sphere_check_visibility_shadow(obj_bounding_sphere* sphere, camera* cam, mat4* mat) {
    vec3 center;
    mat4_mult_vec3_trans(mat, &sphere->center, &center);
    mat4_mult_vec3_trans(&cam->view, &center, &center);
    float_t radius = sphere->radius;

    shadow* shad = rctx_ptr->draw_pass.shadow_ptr;
    float_t v9 = shad->field_170 * shad->field_174;
    if ((center.x + radius) < -v9
        || (center.x - radius) > v9
        || (center.y + radius) < -v9
        || (center.y - radius) > v9
        || (center.z - radius) > -shad->z_near
        || (center.z + radius) < -shad->z_far)
        return false;
    return true;
}

static bool object_bounding_sphere_check_visibility_shadow_chara(obj_bounding_sphere* sphere, camera* cam) {
    mat4 mat;
    shadow* shad = rctx_ptr->draw_pass.shadow_ptr;
    mat4_look_at(&shad->view_point[0], &shad->interest[0], &mat);
    return object_bounding_sphere_check_visibility_shadow(sphere, cam, &mat);
}

static bool object_bounding_sphere_check_visibility_shadow_stage(obj_bounding_sphere* sphere, camera* cam) {
    mat4 mat;
    shadow* shad = rctx_ptr->draw_pass.shadow_ptr;
    mat4_look_at(&shad->view_point[1], &shad->interest[1], &mat);
    return object_bounding_sphere_check_visibility_shadow(sphere, cam, &mat);
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

    for (int32_t& i : s->auth_3d_uids)
        auth_3d_data_set_visibility(&i, s->effect_display);
    return false;
}

static void stage_disp(stage* s) {
    if (s->state != 6 || !s->stage_display)
        return;

    object_data* object_data = &rctx_ptr->object_data;

    mat4 mat;
    mat4_rotate_y(s->rot_y, &mat);

    if (s->stage_data->object_ground.not_null() && s->ground)
        draw_task_add_draw_object_by_object_info(rctx_ptr, &mat, s->stage_data->object_ground, 0, 0, 0, 0, 0, 0);

    if (s->stage_data->object_ring.not_null() && s->ring)
        draw_task_add_draw_object_by_object_info(rctx_ptr, &mat, s->stage_data->object_ring, 0, 0, 0, 0, 0, 0);

    if (s->stage_data->object_reflect.not_null()) {
        object_data_set_draw_task_flags(object_data,
            (draw_task_flags)(DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT));
        draw_task_add_draw_object_by_object_info(rctx_ptr, &mat, s->stage_data->object_reflect, 0, 0, 0, 0, 0, 0);
        object_data_set_draw_task_flags(object_data, (draw_task_flags)0);
    }

    if (s->stage_data->object_refract.not_null()) {
        object_data_set_draw_task_flags(object_data,
            (draw_task_flags)(DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT));
        draw_task_add_draw_object_by_object_info(rctx_ptr, &mat, s->stage_data->object_refract, 0, 0, 0, 0, 0, 0);
        object_data_set_draw_task_flags(object_data, (draw_task_flags)0);
    }

    if (s->stage_data->object_sky.not_null() && s->sky) {
        mat4 t = s->mat;
        mat4_mult(&t, &mat, &t);
        draw_task_add_draw_object_by_object_info(rctx_ptr, &t, s->stage_data->object_sky, 0, 0, 0, 0, 0, 0);
    }

    if (s->stage_data->lens_flare_texture != -1 && s->lens_flare) {
        int32_t object_set_id = s->stage_data->object_set_id;
        post_process_struct* pp = &rctx_ptr->post_process;
        pp->lens_flare_texture = obj_database_get_obj_set_texture(
            object_set_id, s->stage_data->lens_flare_texture);
        pp->lens_shaft_texture = obj_database_get_obj_set_texture(
            object_set_id, s->stage_data->lens_shaft_texture);
        pp->lens_ghost_texture = obj_database_get_obj_set_texture(
            object_set_id, s->stage_data->lens_ghost_texture);
        pp->lens_flare_count = 16;
        pp->lens_shaft_inv_scale = s->stage_data->lens_shaft_inv_scale;
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
    object_data* object_data = &rctx_ptr->object_data;

    for (int32_t i = SHADOW_CHARA; i < SHADOW_MAX; i++) {
        object_data_set_shadow_type(object_data, (shadow_type_enum)i);
        object_data_set_object_bounding_sphere_check_func(object_data,
            i == SHADOW_CHARA ? object_bounding_sphere_check_visibility_shadow_chara : object_bounding_sphere_check_visibility_shadow_stage);
        object_data_set_draw_task_flags(object_data,
            (draw_task_flags)(DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_SHADOW_OBJECT));
        draw_task_add_draw_object_by_object_info(rctx_ptr, mat, object, 0, 0, 0, 0, 0, 0);
    }

    object_data_set_draw_task_flags(object_data, (draw_task_flags)0);
    object_data_set_object_bounding_sphere_check_func(object_data, 0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
}

static void stage_free(stage* s) {
    if (!s->state)
        return;

    if (s->state >= 1 && s->state <= 6)
        s->state = 9;

    if (s->state < 7 || s->state > 9)
        return;

    draw_pass* draw_pass = &rctx_ptr->draw_pass;
    if (s->stage_data->render_texture != -1) {
        post_process_render_texture_free(&rctx_ptr->post_process,
            texture_storage_get_texture(s->stage_data->render_texture), 0);
        s->stage_data->render_texture = -1;
    }

    if (s->stage_data->movie_texture != -1) {
        post_process_movie_texture_free(&rctx_ptr->post_process,
            texture_storage_get_texture(s->stage_data->movie_texture));
        s->stage_data->movie_texture = -1;
    }

    draw_pass->shadow = true;
    //rctx_ptr->post_process.field_14 = 0;
    //rctx_ptr->post_process.field_ED8 = 1;
    npr_spec_color.w = 1.0f;
    draw_pass->field_31D = 0;
    draw_pass->field_31E = 0;
    draw_pass->field_31F = false;
    draw_pass->field_320 = 0;
    light_chara_ambient = false;

    if (s->auth_3d_loaded) {
        auth_3d_data_unload_category(string_data(&s->stage_data->auth_3d_name));
        auth_3d_data_unload_category(string_data(&s->stage_data->name));
        s->auth_3d_loaded = false;
    }

    object_storage_unload_set(s->stage_data->object_set_id);
    if (s->obj_set >= 0)
        object_storage_unload_set(s->obj_set);

    s->obj_set = -1;
    s->index = -1;
    s->state = 0;
    s->counter = 0;
}

static void stage_load(stage* s) {
    if (s->state == 1) {
        data_struct* data = rctx_ptr->data;
        object_database* obj_db = &data->data_ft.obj_db;
        object_storage_load_set(data, obj_db, s->obj_set);
        s->state = 2;
    }
    else if (s->state == 2) {
        if (s->obj_set < 0 || !object_storage_load_obj_set_check_not_read(s->obj_set))
            s->state = 3;
    }
    else if (s->state == 3) {
        data_struct* data = rctx_ptr->data;
        object_database* obj_db = &data->data_ft.obj_db;
        object_storage_load_set(data, obj_db, s->stage_data->object_set_id);

        auth_3d_data_load_category(string_data(&s->stage_data->name));
        auth_3d_data_load_category(string_data(&s->stage_data->auth_3d_name));
        s->auth_3d_loaded = true;
        s->state = 4;
    }
    else if (s->state == 4
        && !object_storage_load_obj_set_check_not_read(s->stage_data->object_set_id)
        && auth_3d_data_check_category_loaded(string_data(&s->stage_data->name))
        && auth_3d_data_check_category_loaded(string_data(&s->stage_data->auth_3d_name))) {
        if (s->stage_data->render_texture != -1)
            post_process_render_texture_set(&rctx_ptr->post_process,
                texture_storage_get_texture(s->stage_data->render_texture), 0);

        if (s->stage_data->movie_texture != -1)
            post_process_movie_texture_set(&rctx_ptr->post_process,
                texture_storage_get_texture(s->stage_data->movie_texture));
        s->state = 6;

        data_struct* data = rctx_ptr->data;
        auth_3d_database* auth_3d_db = &data->data_ft.auth_3d_db;
        int32_t auth_3d_count = s->stage_data->auth_3d_count;
        int32_t* auth_3d_ids = s->stage_data->auth_3d_ids;
        for (int32_t i = 0; i < auth_3d_count; i++) {
            int32_t id = auth_3d_data_load_uid(auth_3d_ids[i], auth_3d_db);
            auth_3d_data_read_file(&id, auth_3d_db);
            s->auth_3d_uids.push_back(id);
        }

        for (int32_t& i : s->auth_3d_uids)
            auth_3d_data_set_visibility(&i, true);
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
    s->mat = mat4u_identity;
    s->rot_y = 0.0;
    s->obj_set = -1;

    for (int32_t& i : s->auth_3d_uids)
        auth_3d_data_unload_id(i, rctx_ptr);
    s->auth_3d_uids.clear();
    s->auth_3d_uids.shrink_to_fit();
    s->effect_display = true;
}

static void stage_set(stage* s, stage* other) {
    if (other && s == other)
        return;

    draw_pass* draw_pass = &rctx_ptr->draw_pass;
    if (s) {
        //flt_14CC925C0 = 0.0;
        //flt_140CB3704 = -1001.0;
        draw_pass->enable[DRAW_PASS_REFLECT] = false;
        draw_pass->reflect = false;
        draw_pass->enable[DRAW_PASS_REFRACT] = false;
        draw_pass->refract = true;
        draw_pass->field_31D = 0;
        draw_pass->field_31E = 0;
        draw_pass->field_31F = false;
        draw_pass->field_320 = 0;
        draw_pass->shadow = true;
        //rctx_ptr->post_process.field_14 = 0;
        //rctx_ptr->post_process.field_ED8 = 1;
        npr_spec_color.w = 1.0f;
        draw_pass->npr_param = 0;
        light_chara_ambient = false;
    }

    if (other) {
        rctx_ptr->chara_reflect = other->stage_data->reflect_type != STAGE_DATA_REFLECT_DISABLE;
        draw_pass->reflect_type = other->stage_data->reflect_type;
        rctx_ptr->chara_refract = other->stage_data->refract_enable;

        if (other->stage_data->reflect) {
            stage_data_reflect* reflect = &other->stage_data->reflect_data;
            draw_pass->enable[DRAW_PASS_REFLECT] = true;
            draw_pass->reflect_blur_num = reflect->blur_num;
            draw_pass->reflect_blur_filter = (blur_filter_mode)reflect->blur_filter;
            draw_pass->reflect = true;
            /*reflect_refract_resolution_mode reflect_resolution_mode = REFLECT_REFRACT_RESOLUTION_512x512;
            if (other->stage_data->reflect_type != STAGE_DATA_REFLECT_REFLECT_MAP)
                reflect_resolution_mode = (reflect_refract_resolution_mode)reflect->mode;
            draw_pass_data_set_reflect_resolution_mode(reflect_resolution_mode);*/
        }

        if (other->stage_data->refract) {
            stage_data_refract* refract = &other->stage_data->refract_data;
            draw_pass->enable[DRAW_PASS_REFRACT] = true;
            //draw_pass_data_set_refract_resolution_mode(refract->mode);
            draw_pass->refract = true;
        }

        if (other->stage_data->flags & STAGE_DATA_FLAG_1)
            draw_pass->field_31D = true;
        if (other->stage_data->flags & STAGE_DATA_FLAG_4)
            draw_pass->field_31E = true;
        if (other->stage_data->flags & STAGE_DATA_FLAG_8)
            draw_pass->field_320 = true;
        if (other->stage_data->flags & STAGE_DATA_LIGHT_CHARA_AMBIENT)
            light_chara_ambient = true;
        //if (stru_14CC92630.pv_id == 421)
            //draw_pass->field_31F = true;
        //sub_14064DC10();
        //sub_140344160(other->index);
    }
    //else
        //sub_140344160(-1);

    if (other)
        light_param_storage_data_set_stage(other->index);
    else
        light_param_storage_data_set_default_light_param();

    if (s)
        for (int32_t& i : s->auth_3d_uids) {
            auth_3d_data_set_req_frame(&i, 0.0f);
            auth_3d_data_set_visibility(&i, false);
        }

    if (other)
        for (int32_t& i : other->auth_3d_uids) {
            auth_3d_data_set_req_frame(&i, 0.0f);
            auth_3d_data_set_visibility(&i, true);
        }

    //sub_140344180(0);

    if (pv_osage_manager_array_ptr_get_disp() && other)
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            rob_chara* rob_chr = rob_chara_array_get(i);
            //if (rob_chr)
                //sub_1405559D0(rob_chr, &other->index);
        }
}

static void stage_set_by_stage_index(stage* s, int32_t stage_index, uint16_t stage_counter) {
    data_struct* data = rctx_ptr->data;
    object_database* obj_db = &data->data_ft.obj_db;
    stage_database* stage_data = &data->data_ft.stage_data;

    stage_reset(s);
    s->index = stage_index;
    s->counter = stage_counter;
    s->state = 1;

    if (stage_index >= vector_old_length(stage_data->stage_data)) {
        s->stage_data = 0;
        return;
    }
    
    s->stage_data = &stage_data->stage_data.begin[stage_index];

    char* name = string_data(&s->stage_data->name);
    size_t name_len = s->stage_data->name.length;

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
        s->obj_set = obj_db->get_object_set_id(set_name);
}

static void task_stage_set_effect_display(task_stage_info* stg_info, bool value) {
    stage* stg = task_stage_get_stage(*stg_info);
    if (stg)
        stg->effect_display = value;
}

static void task_stage_set_ground(task_stage_info* stg_info, bool value) {
    stage* stg = task_stage_get_stage(*stg_info);
    if (stg)
        stg->ground = value;
}

static void task_stage_set_ring(task_stage_info* stg_info, bool value) {
    stage* stg = task_stage_get_stage(*stg_info);
    if (stg)
        stg->ring = value;
}

static void task_stage_set_sky(task_stage_info* stg_info, bool value) {
    stage* stg = task_stage_get_stage(*stg_info);
    if (stg)
        stg->sky = value;
}

static void task_stage_set_stage_display(task_stage_info* stg_info, bool value) {
    stage* stg = task_stage_get_stage(*stg_info);
    if (stg)
        stg->stage_display = value;
}

stage::stage() : index(), counter(), state(), stage_data(), stage_display(), lens_flare(),
ground(), ring(), sky(), auth_3d_loaded(), mat(), rot_y(), obj_set(), effect_display() {

}

stage::~stage() {

}

stage_detail::TaskStage::TaskStage() : state(), current_stage(), stage_display(),
field_FB1(), field_FB2(), field_FB3(), field_FB4(), mat(), field_FF8() {

}

stage_detail::TaskStage:: ~TaskStage() {

}

bool stage_detail::TaskStage::Init() {
    return true;
}

bool stage_detail::TaskStage::Ctrl() {
    stage_detail::TaskStage_CtrlInner(this);

    for (int32_t i = 0; i < TASK_STAGE_STAGE_COUNT; i++)
        if (stages[i].index != -1 && stage_ctrl(&stages[i]))
            break;
    return false;
}

bool stage_detail::TaskStage::Dest() {
    stage_detail::TaskStage_Unload(this);
    if (state)
        return false;
    stage_detail::TaskStage_Reset(this);
    return true;
}

void stage_detail::TaskStage::Disp() {
    if (state != 6 || !stage_display)
        return;

    stage* s = stage_detail::TaskStage_GetCurrentStage(this);
    if (s)
        stage_disp(s);
}

DtmStg::DtmStg() : stage_index(), load_stage_index() {

}

DtmStg::~DtmStg() {

}

bool DtmStg::Init() {
    task_stage_load("DATA_TEST_STG_STAGE");
    task_stage_set_stage_index(stage_index);
    return true;
}

bool DtmStg::Ctrl() {
    if (task_stage_check_not_loaded())
        return false;

    if (load_stage_index != stage_index) {
        task_stage_set_stage_index(load_stage_index);
        stage_index = load_stage_index;
        return false;
    }
    return false;
}

bool DtmStg::Dest() {
    task_stage_unload();
    return true;
}
