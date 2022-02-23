/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d_test.h"
#include "../../../KKdLib/io/path.h"
#include "../../../KKdLib/sort.h"
#include "../../../KKdLib/str_utils.h"
#include "../../../KKdLib/vector.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/effect_group.h"
#include "../../../CRE/Glitter/file_reader.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../../../CRE/Glitter/scene.h"
#include "../../../CRE/data.h"
#include "../../../CRE/render_context.h"
#include "../../../CRE/stage.h"
#include "../../input.h"
#include "../imgui_helper.h"

typedef struct data_test_auth_3d_test_uid {
    const char* name;
    uint32_t uid;
} data_test_auth_3d_test_uid;

vector_old(data_test_auth_3d_test_uid)

typedef struct data_test_auth_3d_test_category {
    const char* name;
    int32_t index;
    vector_old_data_test_auth_3d_test_uid uid;
} data_test_auth_3d_test_category;

vector_old(data_test_auth_3d_test_category)

typedef struct data_test_auth_3d_test {
    int32_t auth_3d_category_index;
    int32_t auth_3d_category_index_prev;
    int32_t auth_3d_index;
    int32_t stage_index;

    int32_t auth_3d_id;
    int32_t auth_3d_uid;
    bool auth_3d_load;
    bool auth_3d_uid_load;
    bool stage_load;

    vec2 a3d_stage_window_pos;

    render_context* rctx;

    bool enable;
    bool repeat;
    bool left_right_reverse;
    float_t frame;
    bool frame_changed;
    float_t last_frame;
    bool paused;
    float_t trans_x;
    float_t trans_z;
    float_t rot_y;
    vector_old_uint32_t obj_set_id;

    vector_old_ptr_char stage;
    vector_old_data_test_auth_3d_test_category category;
} data_test_auth_3d_test;

vector_old_func(data_test_auth_3d_test_category)
vector_old_func(data_test_auth_3d_test_uid)

extern int32_t width;
extern int32_t height;
extern float_t frame_speed;
extern stage stage_stgtst;
extern stage stage_test_data;

static const char* data_test_auth_3d_test_window_title = "Auth 3D Test##Data Test";

static void data_test_auth_3d_test_category_free(data_test_auth_3d_test_category* cat);
static void data_test_auth_3d_test_load_auth_file(data_test_auth_3d_test* auth_3d_test);
static int data_test_auth_3d_test_uid_quicksort_compare_func(void const* src1, void const* src2);

bool data_test_auth_3d_test_init(class_data* data, render_context* rctx) {
    bool ret = false;
    lock_trylock(&pv_lock);
    data->data = force_malloc_s(data_test_auth_3d_test, 1);

    data_test_auth_3d_test* auth_3d_test = (data_test_auth_3d_test*)data->data;
    if (auth_3d_test) {
        auth_3d_database* auth_3d_db = &rctx->data->data_ft.auth_3d_db;
        std::vector<auth_3d_database_category>& auth_3d_db_cat = auth_3d_db->category;
        auth_3d_database_uid* uids = auth_3d_db->uid.data();

        auth_3d_test->category = vector_old_empty(data_test_auth_3d_test_category);
        vector_old_data_test_auth_3d_test_category_reserve(&auth_3d_test->category, auth_3d_db_cat.size());
        for (auth_3d_database_category& i : auth_3d_db_cat) {
            data_test_auth_3d_test_category* cat
                = vector_old_data_test_auth_3d_test_category_reserve_back(&auth_3d_test->category);
            cat->name = i.name.c_str();
            cat->index = -1;
            cat->uid = vector_old_empty(data_test_auth_3d_test_uid);

            vector_old_data_test_auth_3d_test_uid_reserve(&cat->uid, i.uid.size());
            for (int32_t& j : i.uid) {
                if (!uids[j].enabled)
                    continue;

                data_test_auth_3d_test_uid uid;
                uid.name = uids[j].name.c_str();
                uid.uid = uids[j].org_uid;
                vector_old_data_test_auth_3d_test_uid_push_back(&cat->uid, &uid);
            }

            if (vector_old_length(cat->uid)) {
                quicksort_custom(cat->uid.begin, vector_old_length(cat->uid),
                    sizeof(data_test_auth_3d_test_uid),
                    data_test_auth_3d_test_uid_quicksort_compare_func);
                cat->index = 0;
            }
        }

        auth_3d_test->rctx = rctx;

        auth_3d_test->auth_3d_category_index = -1;
        auth_3d_test->auth_3d_category_index_prev = -1;
        auth_3d_test->auth_3d_index = -1;
        auth_3d_test->stage_index = -1;

        stage* stg = (stage*)rctx->stage;
        if (stg && !stg->modern && stg->stage_data) {
            stage_data* stg_data = stg->stage_data;
            data_struct* aft_data = rctx->data;
            stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

            for (stage_data* i = aft_stage_data->stage_data.begin; i != aft_stage_data->stage_data.end; i++)
                if (stg_data == i) {
                    auth_3d_test->stage_index = (int32_t)(i - aft_stage_data->stage_data.begin);
                    break;
                }
        }
        
        auth_3d_test->auth_3d_id = -1;
        auth_3d_test->auth_3d_uid = -1;
        auth_3d_test->auth_3d_load = false;
        auth_3d_test->auth_3d_uid_load = false;

        if (vector_old_length(auth_3d_test->category) > 0) {
            data_test_auth_3d_test_category* cat = &auth_3d_test->category.begin[0];

            if (vector_old_length(cat->uid)) {
                auth_3d_test->auth_3d_load = true;
                auth_3d_test->auth_3d_uid_load = true;
                auth_3d_test->auth_3d_uid = cat->uid.begin[0].uid;
            }

            auth_3d_test->auth_3d_category_index = 0;
            auth_3d_test->auth_3d_category_index_prev = 0;
            auth_3d_test->auth_3d_index = cat->index;
        }

        stage_database* aft_stage_data = &rctx->data->data_ft.stage_data;

        auth_3d_test->stage = vector_old_ptr_empty(char);
        vector_old_ptr_char_reserve(&auth_3d_test->stage, vector_old_length(aft_stage_data->stage_data));
        for (stage_data* i = aft_stage_data->stage_data.begin; i != aft_stage_data->stage_data.end; i++)
            *vector_old_ptr_char_reserve_back(&auth_3d_test->stage) = string_data(&i->name);

        auth_3d_test->a3d_stage_window_pos = { 200.0f, 100.0f };

        auth_3d_test->enable = true;
        auth_3d_test->repeat = true;
        auth_3d_test->left_right_reverse = false;
        auth_3d_test->frame = 0.0f;
        auth_3d_test->frame_changed = false;
        auth_3d_test->last_frame = 0.0f;
        auth_3d_test->paused = false;
        auth_3d_test->trans_x = 0.0f;
        auth_3d_test->trans_z = 0.0f;
        auth_3d_test->rot_y = 0.0f;
        auth_3d_test->obj_set_id = vector_old_empty(uint32_t);
    }
    ret = true;
    lock_unlock(&pv_lock);
    return ret;
}

void data_test_auth_3d_test_ctrl(class_data* data) {
    data_test_auth_3d_test* auth_3d_test = (data_test_auth_3d_test*)data->data;
    if (!auth_3d_test)
        return;

    if (auth_3d_test->auth_3d_uid_load && auth_3d_test->auth_3d_id != -1) {
        render_context* rctx = auth_3d_test->rctx;

        data_struct* aft_data = rctx->data;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();
        const char* uid_name = uids[auth_3d_test->auth_3d_uid].name.c_str();

        string name = string_empty;
        const char* s = 0;
        if (s = strchr(uid_name, '_'))
            string_init_length(&name, uid_name, s - uid_name);
        else
            string_init(&name, uid_name);

        for (stage_data* i = aft_stage_data->stage_data.begin;
            i != aft_stage_data->stage_data.end; i++)
            if (string_compare(&name, &i->name)) {
                int32_t stage_index = (int32_t)(i - aft_stage_data->stage_data.begin);
                if (stage_index != auth_3d_test->stage_index)
                    auth_3d_test->stage_load = true;
                auth_3d_test->stage_index = stage_index;
                break;
            }
        string_free(&name);
        auth_3d_data_unload_id(auth_3d_test->auth_3d_id, auth_3d_test->rctx);
        auth_3d_test->auth_3d_id = -1;
    }

    if (auth_3d_test->auth_3d_load) {
        render_context* rctx = auth_3d_test->rctx;
        if (auth_3d_test->auth_3d_category_index_prev != auth_3d_test->auth_3d_category_index) {
            data_test_auth_3d_test_category* cat_prev
                = &auth_3d_test->category.begin[auth_3d_test->auth_3d_category_index_prev];
            auth_3d_data_unload_category((char*)cat_prev->name);
        }

        {
            for (uint32_t* i = auth_3d_test->obj_set_id.begin; i != auth_3d_test->obj_set_id.end; i++)
                object_storage_delete_object_set(*i);
            vector_old_uint32_t_clear(&auth_3d_test->obj_set_id, 0);
        }

        data_test_auth_3d_test_category* cat
            = &auth_3d_test->category.begin[auth_3d_test->auth_3d_category_index];
        {
            object_database* obj_db = &rctx->data->data_ft.obj_db;

            const char* name_str = cat->name;
            size_t name_len = utf8_length(cat->name);

            object_set_info* set_info;
            if (!memcmp(name_str, "ITMPV", 5)
                && object_database_get_object_set_info(obj_db, name_str, &set_info)) {
                object_set_load_by_db_index(&set_info, rctx->data, obj_db, set_info->id);
                vector_old_uint32_t_push_back(&auth_3d_test->obj_set_id, &set_info->id);
            }

            if (!memcmp(name_str, "EFFCHRPV", 8)
                && object_database_get_object_set_info(obj_db, name_str, &set_info)) {
                object_set_load_by_db_index(&set_info, rctx->data, obj_db, set_info->id);
                vector_old_uint32_t_push_back(&auth_3d_test->obj_set_id, &set_info->id);
            }

            string obj = string_empty;
            for (size_t i = 0; i < name_len && name_len - i >= 8; i++)
                if (!memcmp(&name_str[i], "STGPV", 5)) {
                    string_free(&obj);
                    string_init_length(&obj, &name_str[i], 8);
                    break;
                }

            for (size_t i = 0; i < name_len && name_len - i >= 10; i++)
                if (!memcmp(&name_str[i], "STGD2PV", 7)) {
                    string_free(&obj);
                    string_init_length(&obj, &name_str[i], 10);
                    break;
                }

            if (object_database_get_object_set_info(obj_db, string_data(&obj), &set_info)) {
                object_set_load_by_db_index(&set_info, rctx->data, obj_db, set_info->id);
                vector_old_uint32_t_push_back(&auth_3d_test->obj_set_id, &set_info->id);
            }

            string_add_length(&obj, "HRC", 3);
            if (object_database_get_object_set_info(obj_db, string_data(&obj), &set_info)) {
                object_set_load_by_db_index(&set_info, rctx->data, obj_db, set_info->id);
                vector_old_uint32_t_push_back(&auth_3d_test->obj_set_id, &set_info->id);
            }
            string_free(&obj);
        }

        auth_3d_data_load_category((char*)cat->name);
        auth_3d_test->auth_3d_category_index_prev = auth_3d_test->auth_3d_category_index;
        auth_3d_test->auth_3d_load = false;
    }

    if (auth_3d_test->auth_3d_uid_load) {
        data_test_auth_3d_test_load_auth_file(auth_3d_test);
        auth_3d_test->auth_3d_uid_load = false;
    }

    if (auth_3d_test->stage_load) {
        render_context* rctx = auth_3d_test->rctx;

        data_struct* aft_data = rctx->data;
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;
        texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        stage_free(&stage_test_data, rctx);
        stage_init(&stage_test_data);
        stage_load(&stage_test_data, aft_data, aft_auth_3d_db,
            aft_obj_db, aft_tex_db, aft_stage_data,
            (char*)auth_3d_test->stage.begin[auth_3d_test->stage_index], rctx);
        stage_set(&stage_test_data, rctx);
        auth_3d_test->stage_load = false;
    }
}

bool data_test_auth_3d_test_hide(class_data* data) {
    data_test_auth_3d_test* auth_3d_test = (data_test_auth_3d_test*)data->data;
    if (!auth_3d_test)
        return true;

    lock_data_free(&stage_data_lock, (void(*)(void*))data_test_auth_3d_test_hide);

    render_context* rctx = auth_3d_test->rctx;

    for (uint32_t* i = auth_3d_test->obj_set_id.begin;
        i != auth_3d_test->obj_set_id.end; i++)
        object_storage_delete_object_set(*i);
    vector_old_uint32_t_free(&auth_3d_test->obj_set_id, 0);

    auth_3d_data_unload_id(auth_3d_test->auth_3d_id, auth_3d_test->rctx);

    auth_3d_test->auth_3d_category_index = -1;
    auth_3d_test->auth_3d_category_index_prev = -1;
    auth_3d_test->auth_3d_index = -1;
    auth_3d_test->auth_3d_uid = -1;

    enum_and(data->flags, ~CLASS_HIDE);
    enum_or(data->flags, CLASS_HIDDEN);
    return true;
}

void data_test_auth_3d_test_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 280.0f);
    float_t h = min((float_t)height, 316.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_test_auth_3d_test_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        igEnd();
        return;
    }
    else if (collapsed) {
        igEnd();
        return;
    }

    data_test_auth_3d_test* auth_3d_test = (data_test_auth_3d_test*)data->data;
    if (!auth_3d_test) {
        igEnd();
        return;
    }

    auth_3d* auth = auth_3d_data_get_auth_3d(auth_3d_test->auth_3d_id);
    auth_3d_test->last_frame = auth->play_control.size;

    if (auth_3d_data_check_id_not_empty(&auth_3d_test->auth_3d_id)) {
        auth_3d_test->enable = auth_3d_data_get_enable(&auth_3d_test->auth_3d_id);
        auth_3d_test->repeat = auth_3d_data_get_repeat(&auth_3d_test->auth_3d_id);
        auth_3d_test->left_right_reverse = auth_3d_data_get_left_right_reverse(&auth_3d_test->auth_3d_id);
        auth_3d_test->frame = auth_3d_data_get_frame(&auth_3d_test->auth_3d_id);
        auth_3d_test->frame_changed = false;
        auth_3d_test->paused = auth_3d_data_get_paused(&auth_3d_test->auth_3d_id);
    }

    vector_old_data_test_auth_3d_test_category* auth_3d_db_cat = &auth_3d_test->category;

    render_context* rctx = auth_3d_test->rctx;
    data_struct* aft_data = rctx->data;
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();

    int32_t auth_3d_category_index = auth_3d_test->auth_3d_category_index;

    imguiGetContentRegionAvailSetNextItemWidth();
    if (igBeginCombo("##Auth 3D Category Index", auth_3d_category_index > -1
        ? auth_3d_db_cat->begin[auth_3d_category_index].name : "", 0)) {
        for (data_test_auth_3d_test_category* i = auth_3d_db_cat->begin; i != auth_3d_db_cat->end; i++) {
            int32_t auth_3d_category_idx = (int32_t)(i - auth_3d_db_cat->begin);

            igPushID_Ptr(i);
            if (igSelectable_Bool(i->name,
                auth_3d_category_index == auth_3d_category_idx, 0, ImVec2_Empty)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (igIsItemFocused() && auth_3d_category_index != auth_3d_category_idx)) {
                auth_3d_test->auth_3d_category_index = -1;
                auth_3d_category_index = auth_3d_category_idx;
            }
            igPopID();

            if (auth_3d_category_index == auth_3d_category_idx)
                igSetItemDefaultFocus();
        }

        data->imgui_focus |= true;
        igEndCombo();
    }

    if (auth_3d_category_index != auth_3d_test->auth_3d_category_index) {
        data_test_auth_3d_test_category* cat = &auth_3d_db_cat->begin[auth_3d_category_index];

        if (vector_old_length(cat->uid) > 0) {
            auth_3d_test->auth_3d_load = true;
            auth_3d_test->auth_3d_uid_load = true;
            auth_3d_test->auth_3d_uid = cat->uid.begin[cat->index].uid;
        }
        else {
            auth_3d_test->auth_3d_load = false;
            auth_3d_test->auth_3d_uid_load = false;
            auth_3d_test->auth_3d_uid = -1;
        }
        auth_3d_test->auth_3d_category_index = auth_3d_category_index;
        auth_3d_test->auth_3d_index = cat->index;
    }

    int32_t auth_3d_index = auth_3d_test->auth_3d_index;

    igText(" ID  ");

    imguiGetContentRegionAvailSetNextItemWidth();
    bool auth_3d_category_found = false;
    for (data_test_auth_3d_test_category* i = auth_3d_db_cat->begin; i != auth_3d_db_cat->end; i++) {
        if (auth_3d_category_index != i - auth_3d_db_cat->begin)
            continue;

        if (igBeginCombo("##Auth 3D Index", auth_3d_index > -1
            ? i->uid.begin[auth_3d_index].name : "", 0)) {
            for (data_test_auth_3d_test_uid* j = i->uid.begin; j != i->uid.end; j++) {
                int32_t auth_3d_idx = (int32_t)(j - i->uid.begin);

                igPushID_Ptr(j);
                if (igSelectable_Bool(j->name,
                    auth_3d_index == auth_3d_idx, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && auth_3d_index != auth_3d_idx)) {
                    auth_3d_test->auth_3d_index = -1;
                    auth_3d_index = auth_3d_idx;
                }
                igPopID();

                if (auth_3d_index == auth_3d_idx)
                    igSetItemDefaultFocus();
            }

            data->imgui_focus |= true;
            igEndCombo();
        }

        if (auth_3d_index != auth_3d_test->auth_3d_index) {
            i->index = auth_3d_index;
            auth_3d_test->auth_3d_uid_load = true;
            auth_3d_test->auth_3d_uid = i->uid.begin[auth_3d_index].uid;
            auth_3d_test->auth_3d_index = auth_3d_index;
        }
        auth_3d_category_found = true;
        break;
    }

    if (!auth_3d_category_found && igBeginCombo("##Auth 3D Index", "", 0)) {
        data->imgui_focus |= true;
        igEndCombo();
    }

    float_t frame = auth_3d_test->frame;
    igText("frame[ 0,%4.0f)", auth_3d_test->last_frame);

    imguiGetContentRegionAvailSetNextItemWidth();
    igSliderFloatButton("##frame slider", &frame, 1.0f,
        0.0f, auth_3d_test->last_frame, "%5.0f", 0);
    if (igIsItemActivated())
        auth_3d_test->paused = true;

    if (igButton("|<<", { 32.0, 0.0f }))
        frame = 0.0f;
    igSameLine(0.0f, -1.0f);
    if (igButton(auth_3d_test->paused ? " > " : "||", { 32.0, 0.0f }))
        auth_3d_test->paused = auth_3d_test->paused ? false : true;
    igSameLine(0.0f, -1.0f);
    if (igButton(">>|", { 32.0, 0.0f }))
        frame = auth_3d_test->last_frame;
    igSameLine(0.0f, -1.0f);
    if (igCheckbox("repeat", &auth_3d_test->repeat) && auth_3d_test->repeat && auth_3d_test->paused)
        auth_3d_test->paused = false;

    if (igCheckbox("Left Right Reverse", &auth_3d_test->left_right_reverse))
        frame = 0.0f;

    if (frame != auth_3d_test->frame) {
        auth_3d_test->frame_changed = true;
        auth_3d_test->frame = frame;
    }

    imguiColumnSliderFloat("transX", &auth_3d_test->trans_x, 0.1f, -5.0f, 5.0f, "%.2f", 0, true);
    imguiColumnSliderFloat("transZ", &auth_3d_test->trans_z, 0.1f, -5.0f, 5.0f, "%.2f", 0, true);
    imguiColumnSliderFloat("rotY", &auth_3d_test->rot_y, 1.0f, -360.0f, 360.0f, "%.0f", 0, true);

    if (igButton("cam reset", { 72.0f, 0.0f })
        && auth_3d_test->rctx && auth_3d_test->rctx->camera) {
        camera* cam = auth_3d_test->rctx->camera;
        camera_set_fov(cam, 32.2673416137695);
        camera_set_roll(cam, 0.0);
        vec3 view_point = { 0.0f, 1.0f, 6.0f };
        camera_set_view_point(cam, &view_point);
        vec3 interest = { 0.0f, 1.0f, 0.0f };
        camera_set_interest(cam, &interest);
    }

    igSeparator();

    imguiColumnSliderFloat("Frame Speed", &frame_speed, 0.01f, 0.0f, 3.0f, "%.2f", 0, true);

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();

    float_t x = w;
    float_t y = 0.0f;

    w = min((float_t)width, auth_3d_test->a3d_stage_window_pos.x);
    h = min((float_t)height, auth_3d_test->a3d_stage_window_pos.y);

    igSetNextWindowPos({ x, y }, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (igBegin("A3D STAGE", 0, window_flags)) {
        stage* stg = (stage*)auth_3d_test->rctx->stage;
        if (!stg)
            if (stage_test_data.stage_data)
                stg = &stage_test_data;
            else
                stg = &stage_stgtst;

        igCheckbox("stage", &stg->display);
        igCheckbox("stg auth", &stg->effects);

        int32_t stage_index = auth_3d_test->stage_index;

        ImVec2 t;
        igGetContentRegionAvail(&t);
        igSetNextItemWidth(t.x);
        if (igBeginCombo("##Stage Index", stage_index > -1
            ? auth_3d_test->stage.begin[stage_index] : "", 0)) {
            for (char** i = auth_3d_test->stage.begin; i != auth_3d_test->stage.end; i++) {
                int32_t stage_idx = (int32_t)(i - auth_3d_test->stage.begin);

                igPushID_Ptr(i);
                if (igSelectable_Bool((char*)*i, stage_index == stage_idx, 0, ImVec2_Empty)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (igIsItemFocused() && stage_index != stage_idx)) {
                    auth_3d_test->stage_index = -1;
                    stage_index = stage_idx;
                }
                igPopID();

                if (stage_index == stage_idx)
                    igSetItemDefaultFocus();
            }

            data->imgui_focus |= igIsWindowFocused(0);
            igEndCombo();
        }

        if (stage_index != auth_3d_test->stage_index) {
            auth_3d_test->stage_load = true;
            auth_3d_test->stage_index = stage_index;
        }

        data->imgui_focus |= igIsWindowFocused(0);
    }

    if (auth_3d_data_check_id_not_empty(&auth_3d_test->auth_3d_id)) {
        auth_3d_data_set_enable(&auth_3d_test->auth_3d_id, auth_3d_test->enable);
        auth_3d_data_set_repeat(&auth_3d_test->auth_3d_id, auth_3d_test->repeat);
        auth_3d_data_set_left_right_reverse(&auth_3d_test->auth_3d_id, auth_3d_test->left_right_reverse);
        if (auth_3d_test->frame_changed) {
            auth_3d_data_set_req_frame(&auth_3d_test->auth_3d_id, auth_3d_test->frame);
            auth_3d_test->frame_changed = false;
        }
        auth_3d_data_set_paused(&auth_3d_test->auth_3d_id, auth_3d_test->paused);

        mat4 mat;
        mat4_translate(auth_3d_test->trans_x, 0.0f, auth_3d_test->trans_z, &mat);
        mat4_rotate_y_mult(&mat, auth_3d_test->rot_y * DEG_TO_RAD_FLOAT, &mat);
        auth_3d_data_set_mat(&auth_3d_test->auth_3d_id, &mat);
    }
    igEnd();
}

bool data_test_auth_3d_test_show(class_data* data) {
    data_test_auth_3d_test* auth_3d_test = (data_test_auth_3d_test*)data->data;
    if (!auth_3d_test)
        return false;

    bool ret = false;
    lock_trylock(&pv_lock);
    if (!lock_data_init(&stage_data_lock, &data->lock, data, (void(*)(void*))data_test_auth_3d_test_hide))
        goto End;

    ret = true;

    auth_3d_test->enable = true;
    auth_3d_test->repeat = true;
    auth_3d_test->left_right_reverse = false;
    auth_3d_test->frame = 0.0f;
    auth_3d_test->frame_changed = false;
    auth_3d_test->last_frame = 0.0f;
    auth_3d_test->paused = false;
    auth_3d_test->trans_x = 0.0f;
    auth_3d_test->trans_z = 0.0f;
    auth_3d_test->rot_y = 0.0f;

End:
    lock_unlock(&pv_lock);
    return ret;
}

bool data_test_auth_3d_test_dispose(class_data* data) {
    data_test_auth_3d_test* auth_3d_test = (data_test_auth_3d_test*)data->data;
    if (auth_3d_test) {
        memset(auth_3d_test->stage.begin, 0, sizeof(char*) * vector_old_length(auth_3d_test->stage));

        for (uint32_t* i = auth_3d_test->obj_set_id.begin;
            i != auth_3d_test->obj_set_id.end; i++)
            object_storage_delete_object_set(*i);
        vector_old_uint32_t_free(&auth_3d_test->obj_set_id, 0);

        auth_3d_data_unload_id(auth_3d_test->auth_3d_id, auth_3d_test->rctx);

        vector_old_data_test_auth_3d_test_category_free(&auth_3d_test->category,
            data_test_auth_3d_test_category_free);
        vector_old_ptr_char_free(&auth_3d_test->stage, 0);
    }
    free(data->data);
    return true;
}

static void data_test_auth_3d_test_category_free(data_test_auth_3d_test_category* cat) {
    vector_old_data_test_auth_3d_test_uid_free(&cat->uid, 0);
}

static void data_test_auth_3d_test_load_auth_file(data_test_auth_3d_test* auth_3d_test) {
    render_context* rctx = auth_3d_test->rctx;
    data_struct* aft_data = rctx->data;
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();
    const char* uid_name = uids[auth_3d_test->auth_3d_uid].name.c_str();

    auth_3d_test->auth_3d_id = auth_3d_data_load_uid(auth_3d_test->auth_3d_uid, aft_auth_3d_db);
    auth_3d* auth = auth_3d_data_get_auth_3d(auth_3d_test->auth_3d_id);
    if (!auth)
        return;

    auth_3d_data_read_file(&auth_3d_test->auth_3d_id, aft_auth_3d_db);

    auth_3d_test->enable = true;
    auth_3d_test->repeat = true;
    auth_3d_test->left_right_reverse = false;
    auth_3d_test->frame = 0.0f;
    auth_3d_test->frame_changed = false;
    auth_3d_test->last_frame = auth->play_control.size;
    auth_3d_test->paused = false;
    auth_3d_test->trans_x = 0.0f;
    auth_3d_test->trans_z = 0.0f;
    auth_3d_test->rot_y = 0.0f;
}

static int data_test_auth_3d_test_uid_quicksort_compare_func(void const* src1, void const* src2) {
    const char* str1 = ((data_test_auth_3d_test_uid*)src1)->name;
    const char* str2 = ((data_test_auth_3d_test_uid*)src2)->name;
    return str_utils_compare(str1, str2);
}
