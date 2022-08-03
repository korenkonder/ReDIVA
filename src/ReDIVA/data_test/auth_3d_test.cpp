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
#include "../imgui_helper.hpp"
#include "../input.hpp"

extern int32_t width;
extern int32_t height;
extern float_t frame_speed;
extern render_context* rctx_ptr;

Auth3dTestWindow* auth_3d_test_window;

static int auth_3d_test_window_uid_quicksort_compare_func(void const* src1, void const* src2);

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
    return false;
}

bool Auth3dTestWindow::Dest() {
    return true;
}

void Auth3dTestWindow::Window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 280.0f);
    float_t h = min((float_t)height, 324.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    window_focus = false;
    if (!ImGui::Begin("Auth 3D Test##Data Test", 0, window_flags)) {
        ImGui::End();
        return;
    }

    auth_3d* auth = auth_3d_data_get_auth_3d(auth_3d_test_task->auth_3d_id);
    if (auth)
        last_frame = auth->play_control.size;

    if (auth_3d_data_check_id_not_empty(&auth_3d_test_task->auth_3d_id)) {
        enable = auth_3d_data_get_enable(&auth_3d_test_task->auth_3d_id);
        frame = auth_3d_data_get_frame(&auth_3d_test_task->auth_3d_id);
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
                || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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
                    || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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

    w = min((float_t)width, 200.0f);
    h = min((float_t)height, 124.0f);

    ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("A3D STAGE", 0, window_flags)) {
        ::stage* stg = task_stage_get_current_stage();

        if (stg) {
            ImGui::Checkbox("stage", &stg->stage_display);
            ImGui::Checkbox("stg auth display", &stg->effect_display);
        }
        else {
            bool stage = true;
            ImGui::Checkbox("stage", &stage);
            bool effects = true;
            ImGui::Checkbox("stg auth display", &effects);
        }

        int32_t stage_index = auth_3d_test_task->stage_index;

        ImGui::GetContentRegionAvailSetNextItemWidth();
        if (ImGui::BeginCombo("##Stage Index", stage_index > -1 ? stage[stage_index] : "", 0)) {
            for (const char*& i : stage) {
                int32_t stage_idx = (int32_t)(&i - stage.data());

                ImGui::PushID(i);
                if (ImGui::Selectable(i, stage_index == stage_idx)
                    || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
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

    ImGui::Checkbox("Stage Link Change", &auth_3d_test_task->window.stage_link_change);

    if (auth_3d_data_check_id_not_empty(&auth_3d_test_task->auth_3d_id)) {
        auth_3d_data_set_enable(&auth_3d_test_task->auth_3d_id, enable);
        auth_3d_data_set_repeat(&auth_3d_test_task->auth_3d_id, auth_3d_test_task->repeat);
        auth_3d_data_set_left_right_reverse(&auth_3d_test_task->auth_3d_id, auth_3d_test_task->left_right_reverse);
        if (frame_changed) {
            auth_3d_data_set_req_frame(&auth_3d_test_task->auth_3d_id, frame);
            frame_changed = false;
        }
        auth_3d_data_set_paused(&auth_3d_test_task->auth_3d_id, paused);
        auth_3d_data_set_frame_rate(&auth_3d_test_task->auth_3d_id, 0);
    }
    ImGui::End();
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
