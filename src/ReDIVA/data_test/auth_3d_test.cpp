/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d_test.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/hash.hpp"
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../CRE/clear_color.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/task_effect.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"

static int auth_3d_test_window_uid_quicksort_compare_func(void const* src1, void const* src2);

extern int32_t width;
extern int32_t height;
extern float_t frame_speed;
extern render_context* rctx_ptr;

Auth3dTestTask* auth_3d_test_task;
Auth3dTestWindow* auth_3d_test_window;

Auth3dTestTask::Auth3dTestTask::Window::Window() {
    stage_link_change = true;
    obj_link = true;
}

Auth3dTestTask::Auth3dTestTask() {
    field_1C0 = 10;
    field_1C4 = 10;
    field_1C8 = 10;
    field_1CC = 10;
    field_1D0 = false;
    field_1D4 = 0;
    auth_3d_id = {};
    auth_3d_uid = -1;
    repeat = true;
    left_right_reverse = false;
    pos = false;
    field_1E3 = false;
    field_1E4 = 0;
    field_1E8 = false;
    field_1E9 = false;
    black_mask_listener = false;
    field_1EB = true;
    field_1EC = 1;
    effcmn_obj_set = -1;
    field_210 = 1;
    field_328 = false;
    field_329 = true;
    field_32A = true;
    stage_index = -1;
    load_stage_index = 0;
    trans_value = 0.0f;
    rot_y_value = 0.0f;
    field_388 = false;
    field_38C = 0;
    field_390 = false;
    field_394 = 0;
}

Auth3dTestTask:: ~Auth3dTestTask() {

}

bool Auth3dTestTask::Init() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    field_1D0 = false;
    field_1D4 = 0;
    auth_3d_id = {};
    auth_3d_uid = -1;
    repeat = true;
    left_right_reverse = false;
    pos = false;
    field_1E3 = false;
    field_1E4 = 0;
    field_1E8 = false;
    field_1E9 = false;
    black_mask_listener = false;
    field_1EC = 1;
    effcmn_obj_set = aft_obj_db->get_object_set_id("EFFCMN");
    field_210 = 1;
    field_328 = false;
    field_329 = true;
    field_32A = true;
    stage_index = -1;
    load_stage_index = 0;
    trans_value = 0.0f;
    rot_y_value = 0.0f;
    field_390 = true;
    field_394 = 0;
    category.clear();
    category.shrink_to_fit();
    load_category.clear();
    load_category.shrink_to_fit();
    obj_sets.clear();
    obj_sets.shrink_to_fit();

    rctx_ptr->render_manager.shadow_ptr->self_shadow = true;
    clear_color = { 0x99, 0x99, 0x99, 0x00 };
    task_stage_load_task("A3D_STAGE");
    object_storage_load_set(aft_data, aft_obj_db, effcmn_obj_set);
    return true;
}

bool Auth3dTestTask::Ctrl() {
    SetStage();
    SetAuth3dId();

    if (field_1EC == 1 && !object_storage_load_obj_set_check_not_read(effcmn_obj_set))
        field_1EC = 4;

    if (field_1D4 == 1 && auth_3d_id.check_loaded())
        field_1D4 = 2;

    if (field_1D4 == 2) {
        auth_3d_id.set_enable(true);
        auth_3d_id.set_paused(false);
        auth_3d_id.set_repeat(repeat);
        auth_3d_id.set_left_right_reverse(left_right_reverse);
        auth_3d_id.set_pos(pos ? 1 : 0);
        field_1D4 = 4;
    }

    if (auth_3d_id.check_not_empty()) {
        mat4 mat;
        mat4_translate(&trans_value, &mat);
        mat4_rotate_y_mult(&mat, rot_y_value * DEG_TO_RAD_FLOAT, &mat);
        auth_3d_id.set_mat(mat);
    }
    return false;
}

bool Auth3dTestTask::Dest() {
    auth_3d_id.unload_id(rctx_ptr);
    object_storage_unload_set(effcmn_obj_set);
    task_stage_unload_task();
    clear_color = color_black;
    rctx_ptr->render_manager.shadow_ptr->self_shadow = true;
    if (category.size())
        auth_3d_data_unload_category(category.c_str());
    category.clear();
    category.shrink_to_fit();
    load_category.clear();
    load_category.shrink_to_fit();
    for (uint32_t& i : obj_sets)
        object_storage_unload_set(i);
    obj_sets.clear();
    obj_sets.shrink_to_fit();
    return true;
}

void Auth3dTestTask::DispAuth3dChara(::auth_3d_id& id) {
    id.get_uid(); // ???
    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(0);
    if (!rob_itm_equip)
        return;

    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++)
        rob_itm_equip->set_disp((item_id)i, true);
}

void Auth3dTestTask::DispChara() {
    if (auth_3d_id.check_not_empty())
        Auth3dTestTask::DispAuth3dChara(auth_3d_id);
}

void Auth3dTestTask::SetAuth3dId() {
    if (task_stage_check_not_loaded() || auth_3d_uid == -1
        || !load_category.size() || auth_3d_uid == auth_3d_id.get_uid())
        return;

    if (load_category.compare(category)) {
        if (category.size()) {
            auth_3d_data_unload_category(category.c_str());
            for (uint32_t& i : obj_sets)
                object_storage_unload_set(i);
            obj_sets.clear();
        }
        auth_3d_data_load_category(load_category.c_str());
        category.assign(load_category);
        if (window.obj_link)
            field_394 = 1;
    }

    if (!auth_3d_data_check_category_loaded(category.c_str()))
        return;

    if (field_394 == 1) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;

        auth_3d_data_get_obj_sets_from_category(category, obj_sets, aft_auth_3d_db, aft_obj_db);

        for (uint32_t& i : obj_sets)
            object_storage_load_set(aft_data, aft_obj_db, i);
        field_394 = 2;
    }
    else if (field_394 == 2) {
        bool wait_load = false;
        for (uint32_t& i : obj_sets)
            if (object_storage_load_obj_set_check_not_read(i))
                wait_load = true;

        if (!wait_load)
            field_394 = 0;
    }

    if (!field_394) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

        auth_3d_id.unload_id(rctx_ptr);
        auth_3d_id = auth_3d_data_load_uid(auth_3d_uid, aft_auth_3d_db);
        if (auth_3d_id.check_not_empty()) {
            auth_3d_id.set_enable(false);
            auth_3d_id.read_file(aft_auth_3d_db);
            field_1D4 = 1;
        }
        auth_3d_uid = -1;
        return;
    }
}

void Auth3dTestTask::SetStage() {
    if (window.stage_link_change) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        const char* name = auth_3d_data_get_uid_name(auth_3d_uid, aft_auth_3d_db);
        size_t name_length = utf8_length(name);
        if (name && name_length >= 3) {
            size_t v5 = name_length - 2;
            const char* v6 = name;
            while (v5) {
                const char* v7 = (const char*)memchr(v6, 'S', v5);
                if (!v7)
                    break;

                if (memcmp(v7, "STG", min_def(v5, 3))) {
                    v5 += v6 - v7 - 1;
                    v6 = v7 + 1;
                    continue;
                }

                size_t v16 = v7 - name;
                if (v16 == -1 || v16 >= name_length || name_length == v16)
                    break;

                size_t v17 = name_length - v16;
                for (const char* i = name + v16; v17; ) {
                    const char* v20 = (const char*)memchr(i, '_', v17);
                    if (!v20)
                        break;

                    if (*v20 != '_') {
                        v17 += i - v20 - 1;
                        i = v20 + 1;
                        continue;
                    }

                    size_t v24 = v20 - name;
                    if (v24 != -1) {
                        std::string v30 = std::string(name + v16, v24 - v16);
                        int32_t stage_index = aft_stage_data->get_stage_index(v30.c_str());
                        if (this->stage_index != stage_index)
                            load_stage_index = stage_index;
                    }
                    break;
                }
                break;
            }
        }
    }

    if (load_stage_index != -1 && load_stage_index != stage_index) {
        task_stage_set_stage_index(load_stage_index);
        stage_index = load_stage_index;
        load_stage_index = -1;
    }
}

auth_3d_test_window_category::auth_3d_test_window_category() : name(), index() {

}

auth_3d_test_window_category::~auth_3d_test_window_category() {

}

Auth3dTestWindow::Auth3dTestWindow() {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    std::vector<auth_3d_database_category>& auth_3d_db_cat = aft_auth_3d_db->category;
    auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();

    category.resize(auth_3d_db_cat.size());
    for (auth_3d_database_category& i : auth_3d_db_cat) {
        auth_3d_test_window_category* cat = &category[&i - auth_3d_db_cat.data()];
        cat->name = &i.name;
        cat->index = -1;

        cat->uid.reserve(i.uid.size());
        for (int32_t& j : i.uid)
            if (uids[j].enabled)
                cat->uid.push_back({ &uids[j].name, uids[j].org_uid });

        if (cat->uid.size()) {
            quicksort_custom(cat->uid.data(), cat->uid.size(), sizeof(auth_3d_test_window_uid),
                auth_3d_test_window_uid_quicksort_compare_func);
            cat->index = 0;
        }
    }

    auth_3d_category_index = -1;
    auth_3d_category_index_prev = -1;
    auth_3d_index = -1;

    auth_3d_uid = -1;
    auth_3d_load = false;
    auth_3d_uid_load = false;

    if (category.size() > 0) {
        auth_3d_test_window_category* cat = &category.front();

        if (cat->uid.size()) {
            auth_3d_load = true;
            auth_3d_uid_load = true;
            auth_3d_uid = cat->uid.front().uid;
        }

        auth_3d_category_index = 0;
        auth_3d_category_index_prev = 0;
        auth_3d_index = cat->index;
    }

    stage.reserve(aft_stage_data->stage_data.size());
    for (stage_data& i : aft_stage_data->stage_data)
        stage.push_back(i.name.c_str());

    enable = true;
    frame = 0.0f;
    frame_changed = false;
    last_frame = 0.0f;
    paused = false;

    stg_auth_display = true;
    stg_display = true;
}

Auth3dTestWindow::~Auth3dTestWindow() {

}

bool Auth3dTestWindow::Init() {
    if (!first_show) {
        auth_3d_category_index = -1;
        auth_3d_category_index_prev = -1;
        auth_3d_index = -1;
        auth_3d_uid = -1;
    }

    enable = true;
    frame = 0.0f;
    frame_changed = false;
    last_frame = 0.0f;
    paused = false;

    stg_auth_display = true;
    stg_display = true;
    return true;
}

bool Auth3dTestWindow::Ctrl() {
    if (app::TaskWork::HasTaskCtrl(auth_3d_test_task)) {
        if (auth_3d_load) {
            auth_3d_test_task->load_category = category[auth_3d_category_index].name->c_str();
            auth_3d_load = false;
        }

        if (auth_3d_uid_load) {
            auth_3d_test_task->auth_3d_uid = auth_3d_uid;
            auth_3d_uid_load = false;
        }
    }
    task_stage_current_set_stage_display(stg_display, false);
    task_effect_parent_set_enable(stg_auth_display);
    return false;
}

bool Auth3dTestWindow::Dest() {
    return true;
}

void Auth3dTestWindow::Window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 280.0f);
    float_t h = min_def((float_t)height, 344.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    window_focus = false;
    if (!ImGui::Begin("Auth 3D Test##Data Test", 0, window_flags)) {
        ImGui::End();
        return;
    }

    auth_3d* auth = auth_3d_test_task->auth_3d_id.get_auth_3d();
    if (auth)
        last_frame = auth->play_control.size;

    if (auth_3d_test_task->auth_3d_id.check_not_empty()) {
        enable = auth_3d_test_task->auth_3d_id.get_enable();
        frame = auth_3d_test_task->auth_3d_id.get_frame();
        frame_changed = false;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();

    int32_t _auth_3d_category_index = auth_3d_category_index;

    ImGui::GetContentRegionAvailSetNextItemWidth();
    if (ImGui::BeginCombo("##Auth 3D Category Index", _auth_3d_category_index > -1
        ? category[_auth_3d_category_index].name->c_str() : "", 0)) {
        for (auth_3d_test_window_category& i : category) {
            int32_t auth_3d_category_idx = (int32_t)(&i - category.data());

            ImGui::PushID(&i);
            if (ImGui::Selectable(i.name->c_str(), _auth_3d_category_index == auth_3d_category_idx)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && _auth_3d_category_index != auth_3d_category_idx)) {
                auth_3d_category_index = -1;
                _auth_3d_category_index = auth_3d_category_idx;
            }
            ImGui::PopID();

            if (_auth_3d_category_index == auth_3d_category_idx)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= true;
        ImGui::EndCombo();
    }

    if (_auth_3d_category_index != auth_3d_category_index) {
        auth_3d_test_window_category* cat = &category[_auth_3d_category_index];

        if (cat->uid.size() > 0) {
            auth_3d_load = true;
            auth_3d_uid_load = true;
            auth_3d_uid = cat->uid[cat->index].uid;
        }
        else {
            auth_3d_load = false;
            auth_3d_uid_load = false;
            auth_3d_uid = -1;
        }
        auth_3d_category_index = _auth_3d_category_index;
        auth_3d_index = cat->index;
    }

    int32_t _auth_3d_index = auth_3d_index;

    ImGui::Text(" ID  ");

    ImGui::SameLine();

    ImGui::Checkbox("OBJ Link", &auth_3d_test_task->window.obj_link);

    ImGui::GetContentRegionAvailSetNextItemWidth();
    bool auth_3d_category_found = false;
    for (auth_3d_test_window_category& i : category) {
        if (auth_3d_category_index != &i - category.data())
            continue;

        if (ImGui::BeginCombo("##Auth 3D Index", _auth_3d_index > -1
            ? i.uid[_auth_3d_index].name->c_str() : "", 0)) {
            for (auth_3d_test_window_uid& j : i.uid) {
                int32_t auth_3d_idx = (int32_t)(&j - i.uid.data());

                ImGui::PushID(&j);
                if (ImGui::Selectable(j.name->c_str(), _auth_3d_index == auth_3d_idx)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && _auth_3d_index != auth_3d_idx)) {
                    auth_3d_index = -1;
                    _auth_3d_index = auth_3d_idx;
                }
                ImGui::PopID();

                if (_auth_3d_index == auth_3d_idx)
                    ImGui::SetItemDefaultFocus();
            }

            window_focus |= true;
            ImGui::EndCombo();
        }

        if (_auth_3d_index != auth_3d_index) {
            i.index = _auth_3d_index;
            auth_3d_uid_load = true;
            auth_3d_uid = i.uid[_auth_3d_index].uid;
            auth_3d_index = _auth_3d_index;
        }
        auth_3d_category_found = true;
        break;
    }

    if (!auth_3d_category_found && ImGui::BeginCombo("##Auth 3D Index", "", 0)) {
        window_focus |= true;
        ImGui::EndCombo();
    }

    float_t _frame = frame;
    ImGui::Text("frame[ 0,%4.0f)", last_frame);

    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::SliderFloatButton("##frame slider", &_frame, 1.0f, 0.0f, last_frame, 10.0f, "%5.0f", 0);
    if (ImGui::IsItemActivated())
        paused = true;

    if (ImGui::Button("|<<", { 32.0, 0.0f }))
        _frame = 0.0f;
    ImGui::SameLine();
    if (ImGui::Button(paused ? " > " : "||", { 32.0, 0.0f }))
        paused ^= true;
    ImGui::SameLine();
    if (ImGui::Button(">>|", { 32.0, 0.0f }))
        _frame = last_frame;
    ImGui::SameLine();
    if (ImGui::Checkbox("repeat", &auth_3d_test_task->repeat) && auth_3d_test_task->repeat && paused)
        paused = false;

    if (ImGui::Checkbox("Left Right Reverse", &auth_3d_test_task->left_right_reverse))
        _frame = 0.0f;

    ImGui::Checkbox("pos", &auth_3d_test_task->pos);

    if (_frame != frame) {
        frame_changed = true;
        frame = _frame;
    }

    ImGui::ColumnSliderFloatButton("transX", &auth_3d_test_task->trans_value.x,
        0.1f, -5.0f, 5.0f, 1.0f, "%.2f", 0);
    ImGui::ColumnSliderFloatButton("transZ", &auth_3d_test_task->trans_value.z,
        0.1f, -5.0f, 5.0f, 1.0f, "%.2f", 0);
    ImGui::ColumnSliderFloatButton("rotY", &auth_3d_test_task->rot_y_value,
        1.0f, -360.0f, 360.0f, 10.0f, "%.0f", 0);

    if (ImGui::Button("cam reset", { 72.0f, 0.0f }) && rctx_ptr && rctx_ptr->camera) {
        camera* cam = rctx_ptr->camera;
        cam->set_fov(32.2673416137695);
        cam->set_roll(0.0);
        cam->set_view_point({ 0.0f, 1.0f, 6.0f });
        cam->set_interest({ 0.0f, 1.0f, 0.0f });
    }

    ImGui::Separator();

    ImGui::ColumnSliderFloatButton("Frame Speed", &frame_speed, 0.01f, 0.0f, 3.0f, 0.1f, "%.2f", 0);

    window_focus |= ImGui::IsWindowFocused();
    ImGui::End();

    float_t x = w;
    float_t y = 0.0f;

    w = min_def((float_t)width, 200.0f);
    h = min_def((float_t)height, 124.0f);

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("A3D STAGE", 0, window_flags)) {
        ImGui::Checkbox("stage", &stg_display);
        ImGui::Checkbox("stg auth display", &stg_auth_display);

        int32_t stage_index = auth_3d_test_task->stage_index;

        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::BeginCombo("##Stage Index", stage_index > -1 ? stage[stage_index] : "", 0)) {
            for (const char*& i : stage) {
                int32_t stage_idx = (int32_t)(&i - stage.data());

                ImGui::PushID(i);
                if (ImGui::Selectable(i, stage_index == stage_idx)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && stage_index != stage_idx))
                    stage_index = stage_idx;
                ImGui::PopID();

                if (stage_index == stage_idx)
                    ImGui::SetItemDefaultFocus();
            }

            window_focus |= ImGui::IsWindowFocused();
            ImGui::EndCombo();
        }

        if (stage_index != auth_3d_test_task->stage_index)
            auth_3d_test_task->load_stage_index = stage_index;

        window_focus |= ImGui::IsWindowFocused();
    }

    ImGui::Checkbox("Link Stage Change", &auth_3d_test_task->window.stage_link_change);

    if (auth_3d_test_task->auth_3d_id.check_not_empty()) {
        auth_3d_test_task->auth_3d_id.set_enable(enable);
        auth_3d_test_task->auth_3d_id.set_repeat(auth_3d_test_task->repeat);
        auth_3d_test_task->auth_3d_id.set_left_right_reverse(auth_3d_test_task->left_right_reverse);
        auth_3d_test_task->auth_3d_id.set_pos(auth_3d_test_task->pos ? 1 : 0);
        if (frame_changed) {
            auth_3d_test_task->auth_3d_id.set_req_frame(frame);
            frame_changed = false;
        }
        auth_3d_test_task->auth_3d_id.set_paused(paused);
        auth_3d_test_task->auth_3d_id.set_frame_rate(0);
    }
    ImGui::End();
}

void auth_3d_test_task_init() {
    auth_3d_test_task = new Auth3dTestTask;
}

void auth_3d_test_task_disp_chara() {
    auth_3d_test_task->DispChara();
}

void auth_3d_test_task_free() {
    if (auth_3d_test_task) {
        delete auth_3d_test_task;
        auth_3d_test_task = 0;
    }
}

void auth_3d_test_window_init() {
    auth_3d_test_window = new Auth3dTestWindow;
}

void auth_3d_test_window_free() {
    if (auth_3d_test_window) {
        delete auth_3d_test_window;
        auth_3d_test_window = 0;
    }
}

static int auth_3d_test_window_uid_quicksort_compare_func(void const* src1, void const* src2) {
    std::string* str1 = ((auth_3d_test_window_uid*)src1)->name;
    std::string* str2 = ((auth_3d_test_window_uid*)src2)->name;
    return str_utils_compare_length(str1->c_str(), str1->size(), str2->c_str(), str2->size());
}
