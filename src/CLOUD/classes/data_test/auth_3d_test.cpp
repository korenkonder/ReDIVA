/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d_test.hpp"
#include "../../../KKdLib/io/path.hpp"
#include "../../../KKdLib/hash.hpp"
#include "../../../KKdLib/sort.hpp"
#include "../../../KKdLib/str_utils.hpp"
#include "../../../CRE/data.hpp"
#include "../../../CRE/render_context.hpp"
#include "../../../CRE/stage.hpp"
#include "../../input.hpp"
#include "../data_test.h"
#include "../imgui_helper.h"

struct data_test_auth_3d_test_uid {
    const char* name;
    int32_t uid;

    data_test_auth_3d_test_uid(const char* name, int32_t uid);
};

struct data_test_auth_3d_test_category {
    const char* name;
    int32_t index;
    std::vector<data_test_auth_3d_test_uid> uid;

    data_test_auth_3d_test_category();
    ~data_test_auth_3d_test_category();
};

class Auth3dTestWindow : public app::TaskWindow {
public:
    int32_t auth_3d_category_index;
    int32_t auth_3d_category_index_prev;
    int32_t auth_3d_index;

    int32_t auth_3d_uid;
    bool auth_3d_load;
    bool auth_3d_uid_load;

    bool enable;
    float_t frame;
    bool frame_changed;
    float_t last_frame;
    bool paused;

    std::vector<const char*> stage;
    std::vector<data_test_auth_3d_test_category> category;

    Auth3dTestWindow();
    virtual ~Auth3dTestWindow() override;

    virtual void Window() override;
};

extern int32_t width;
extern int32_t height;
extern float_t frame_speed;
extern render_context* rctx_ptr;
extern vec4u8 clear_color;

static const char* data_test_auth_3d_test_window_title = "Auth 3D Test##Data Test";

static Auth3dTestWindow* auth_3d_test_window;

static bool auth_3d_load();
static bool auth_3d_unload();
static int data_test_auth_3d_test_uid_quicksort_compare_func(void const* src1, void const* src2);

bool data_test_auth_3d_test_init(class_data* data, render_context* rctx) {
    return true;
}

void data_test_auth_3d_test_ctrl(class_data* data) {
    if (app::TaskWork::HasTaskCtrl(&auth_3d_test_task)) {
        if (auth_3d_test_window->auth_3d_load) {
            std::vector<data_test_auth_3d_test_category>& auth_3d_db_cat = auth_3d_test_window->category;
            int32_t auth_3d_category_index = auth_3d_test_window->auth_3d_category_index;
            auth_3d_test_task.load_category = auth_3d_db_cat[auth_3d_category_index].name;
            auth_3d_test_window->auth_3d_load = false;
        }

        if (auth_3d_test_window->auth_3d_uid_load) {
            auth_3d_test_task.auth_3d_uid = auth_3d_test_window->auth_3d_uid;
            auth_3d_test_window->auth_3d_uid_load = false;
        }
    }
}

bool data_test_auth_3d_test_hide(class_data* data) {
    auth_3d_test_task.SetDest();

    enum_and(data->flags, ~CLASS_HIDE);
    enum_or(data->flags, CLASS_HIDDEN);
    return true;
}

void data_test_auth_3d_test_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 280.0f);
    float_t h = min((float_t)height, 324.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_test_auth_3d_test_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    if (!auth_3d_test_window) {
        ImGui::End();
        return;
    }

    auth_3d* auth = auth_3d_data_get_auth_3d(auth_3d_test_task.auth_3d_id);
    if (auth)
        auth_3d_test_window->last_frame = auth->play_control.size;

    if (auth_3d_data_check_id_not_empty(&auth_3d_test_task.auth_3d_id)) {
        auth_3d_test_window->enable = auth_3d_data_get_enable(&auth_3d_test_task.auth_3d_id);
        auth_3d_test_window->frame = auth_3d_data_get_frame(&auth_3d_test_task.auth_3d_id);
        auth_3d_test_window->frame_changed = false;
    }

    std::vector<data_test_auth_3d_test_category>& auth_3d_db_cat = auth_3d_test_window->category;

    data_struct* aft_data = rctx_ptr->data;
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();

    int32_t auth_3d_category_index = auth_3d_test_window->auth_3d_category_index;

    imguiGetContentRegionAvailSetNextItemWidth();
    if (ImGui::BeginCombo("##Auth 3D Category Index", auth_3d_category_index > -1
        ? auth_3d_db_cat[auth_3d_category_index].name : "", 0)) {
        for (data_test_auth_3d_test_category& i : auth_3d_db_cat) {
            int32_t auth_3d_category_idx = (int32_t)(&i - auth_3d_db_cat.data());

            ImGui::PushID(&i);
            if (ImGui::Selectable(i.name, auth_3d_category_index == auth_3d_category_idx)
                || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                || (ImGui::IsItemFocused() && auth_3d_category_index != auth_3d_category_idx)) {
                auth_3d_category_index = auth_3d_category_idx;
            }
            ImGui::PopID();

            if (auth_3d_category_index == auth_3d_category_idx)
                ImGui::SetItemDefaultFocus();
        }

        data->imgui_focus |= true;
        ImGui::EndCombo();
    }

    if (auth_3d_category_index != auth_3d_test_window->auth_3d_category_index) {
        data_test_auth_3d_test_category* cat = &auth_3d_db_cat[auth_3d_category_index];

        if (cat->uid.size() > 0) {
            auth_3d_test_window->auth_3d_load = true;
            auth_3d_test_window->auth_3d_uid_load = true;
            auth_3d_test_window->auth_3d_uid = cat->uid[cat->index].uid;
        }
        else {
            auth_3d_test_window->auth_3d_load = false;
            auth_3d_test_window->auth_3d_uid_load = false;
            auth_3d_test_window->auth_3d_uid = -1;
        }
        auth_3d_test_window->auth_3d_category_index = auth_3d_category_index;
        auth_3d_test_window->auth_3d_index = cat->index;
    }

    int32_t auth_3d_index = auth_3d_test_window->auth_3d_index;

    ImGui::Text(" ID  ");

    ImGui::SameLine();

    ImGui::Checkbox("OBJ Link", &auth_3d_test_task.window.obj_link);

    imguiGetContentRegionAvailSetNextItemWidth();
    bool auth_3d_category_found = false;
    for (data_test_auth_3d_test_category& i : auth_3d_db_cat) {
        if (auth_3d_category_index != &i - auth_3d_db_cat.data())
            continue;

        if (ImGui::BeginCombo("##Auth 3D Index", auth_3d_index > -1
            ? i.uid[auth_3d_index].name : "", 0)) {
            for (data_test_auth_3d_test_uid& j : i.uid) {
                int32_t auth_3d_idx = (int32_t)(&j - i.uid.data());

                ImGui::PushID(&j);
                if (ImGui::Selectable(j.name, auth_3d_index == auth_3d_idx)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && auth_3d_index != auth_3d_idx)) {
                    auth_3d_test_window->auth_3d_index = -1;
                    auth_3d_index = auth_3d_idx;
                }
                ImGui::PopID();

                if (auth_3d_index == auth_3d_idx)
                    ImGui::SetItemDefaultFocus();
            }

            data->imgui_focus |= true;
            ImGui::EndCombo();
        }

        if (auth_3d_index != auth_3d_test_window->auth_3d_index) {
            i.index = auth_3d_index;
            auth_3d_test_window->auth_3d_uid_load = true;
            auth_3d_test_window->auth_3d_uid = i.uid[auth_3d_index].uid;
            auth_3d_test_window->auth_3d_index = auth_3d_index;
        }
        auth_3d_category_found = true;
        break;
    }

    if (!auth_3d_category_found && ImGui::BeginCombo("##Auth 3D Index", "", 0)) {
        data->imgui_focus |= true;
        ImGui::EndCombo();
    }

    float_t frame = auth_3d_test_window->frame;
    ImGui::Text("frame[ 0,%4.0f)", auth_3d_test_window->last_frame);

    imguiGetContentRegionAvailSetNextItemWidth();
    igSliderFloatButton("##frame slider", &frame, 1.0f,
        0.0f, auth_3d_test_window->last_frame, "%5.0f", 0);
    if (ImGui::IsItemActivated())
         auth_3d_test_window->paused = true;

    if (ImGui::Button("|<<", { 32.0, 0.0f }))
        frame = 0.0f;
    ImGui::SameLine();
    if (ImGui::Button( auth_3d_test_window->paused ? " > " : "||", { 32.0, 0.0f }))
         auth_3d_test_window->paused =  auth_3d_test_window->paused ? false : true;
    ImGui::SameLine();
    if (ImGui::Button(">>|", { 32.0, 0.0f }))
        frame = auth_3d_test_window->last_frame;
    ImGui::SameLine();
    if (ImGui::Checkbox("repeat", &auth_3d_test_task.repeat)
        && auth_3d_test_task.repeat &&  auth_3d_test_window->paused)
         auth_3d_test_window->paused = false;

    if (ImGui::Checkbox("Left Right Reverse", &auth_3d_test_task.left_right_reverse))
        frame = 0.0f;

    if (frame != auth_3d_test_window->frame) {
        auth_3d_test_window->frame_changed = true;
        auth_3d_test_window->frame = frame;
    }

    imguiColumnSliderFloat("transX", &auth_3d_test_task.trans_value.x, 0.1f, -5.0f, 5.0f, "%.2f", 0, true);
    imguiColumnSliderFloat("transZ", &auth_3d_test_task.trans_value.z, 0.1f, -5.0f, 5.0f, "%.2f", 0, true);
    imguiColumnSliderFloat("rotY", &auth_3d_test_task.rot_y_value, 1.0f, -360.0f, 360.0f, "%.0f", 0, true);

    if (ImGui::Button("cam reset", { 72.0f, 0.0f }) && rctx_ptr && rctx_ptr->camera) {
        camera* cam = rctx_ptr->camera;
        camera_set_fov(cam, 32.2673416137695);
        camera_set_roll(cam, 0.0);
        vec3 view_point = { 0.0f, 1.0f, 6.0f };
        camera_set_view_point(cam, &view_point);
        vec3 interest = { 0.0f, 1.0f, 0.0f };
        camera_set_interest(cam, &interest);
    }

    ImGui::Separator();

    imguiColumnSliderFloat("Frame Speed", &frame_speed, 0.01f, 0.0f, 3.0f, "%.2f", 0, true);

    data->imgui_focus |= ImGui::IsWindowFocused();
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
        stage* stg = task_stage_get_current_stage();

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

        int32_t stage_index = auth_3d_test_task.stage_index;

        imguiGetContentRegionAvailSetNextItemWidth();
        if (ImGui::BeginCombo("##Stage Index", stage_index > -1
            ? auth_3d_test_window->stage[stage_index] : "", 0)) {
            for (const char*& i : auth_3d_test_window->stage) {
                int32_t stage_idx = (int32_t)(&i - auth_3d_test_window->stage.data());

                ImGui::PushID(i);
                if (ImGui::Selectable(i, stage_index == stage_idx)
                    || imguiItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && stage_index != stage_idx))
                    stage_index = stage_idx;
                ImGui::PopID();

                if (stage_index == stage_idx)
                    ImGui::SetItemDefaultFocus();
            }

            data->imgui_focus |= ImGui::IsWindowFocused();
            ImGui::EndCombo();
        }

        if (stage_index != auth_3d_test_task.stage_index)
            auth_3d_test_task.load_stage_index = stage_index;

        data->imgui_focus |= ImGui::IsWindowFocused();
    }

    ImGui::Checkbox("Stage Link Change", &auth_3d_test_task.window.stage_link_change);

    if (auth_3d_data_check_id_not_empty(&auth_3d_test_task.auth_3d_id)) {
        auth_3d_data_set_enable(&auth_3d_test_task.auth_3d_id, auth_3d_test_window->enable);
        auth_3d_data_set_repeat(&auth_3d_test_task.auth_3d_id, auth_3d_test_task.repeat);
        auth_3d_data_set_left_right_reverse(&auth_3d_test_task.auth_3d_id, auth_3d_test_task.left_right_reverse);
        if (auth_3d_test_window->frame_changed) {
            auth_3d_data_set_req_frame(&auth_3d_test_task.auth_3d_id, auth_3d_test_window->frame);
            auth_3d_test_window->frame_changed = false;
        }
        auth_3d_data_set_paused(&auth_3d_test_task.auth_3d_id,  auth_3d_test_window->paused);
        auth_3d_data_set_frame_rate(&auth_3d_test_task.auth_3d_id, 0);
    }
    ImGui::End();
}

bool data_test_auth_3d_test_show(class_data* data) {
    auth_3d_load();

    if (!auth_3d_test_window)
        auth_3d_test_window = new Auth3dTestWindow;
    else {
        auth_3d_test_window->auth_3d_category_index = -1;
        auth_3d_test_window->auth_3d_category_index_prev = -1;
        auth_3d_test_window->auth_3d_index = -1;
        auth_3d_test_window->auth_3d_uid = -1;
    }

    auth_3d_test_window->enable = true;
    auth_3d_test_window->frame = 0.0f;
    auth_3d_test_window->frame_changed = false;
    auth_3d_test_window->last_frame = 0.0f;
    auth_3d_test_window->paused = false;
    return true;
}

bool data_test_auth_3d_test_dispose(class_data* data) {
    delete auth_3d_test_window;

    data->flags = (class_flags)(CLASS_HIDDEN | CLASS_DISPOSED);
    data->imgui_focus = false;
    return true;
}

static bool auth_3d_load() {
    app::TaskWork::AppendTask(&auth_3d_test_task, "AUTH3DTEST");
    return true;
}

static bool auth_3d_unload() {
    auth_3d_test_task.SetDest();

    classes_data* c = &data_test_classes[DATA_TEST_AUTH_3D_TEST];
    enum_or(c->data.flags, CLASS_HIDE);
    return true;
}

static int data_test_auth_3d_test_uid_quicksort_compare_func(void const* src1, void const* src2) {
    const char* str1 = ((data_test_auth_3d_test_uid*)src1)->name;
    const char* str2 = ((data_test_auth_3d_test_uid*)src2)->name;
    return str_utils_compare(str1, str2);
}

data_test_auth_3d_test_uid::data_test_auth_3d_test_uid(
    const char* name, int32_t uid) : name(name), uid(uid) {

}

data_test_auth_3d_test_category::data_test_auth_3d_test_category() : name(), index() {

}

data_test_auth_3d_test_category::~data_test_auth_3d_test_category() {

}

Auth3dTestWindow::Auth3dTestWindow() {
    data_struct* aft_data = rctx_ptr->data;
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    stage_database* aft_stage_data = &rctx_ptr->data->data_ft.stage_data;

    std::vector<auth_3d_database_category>& auth_3d_db_cat = aft_auth_3d_db->category;
    auth_3d_database_uid* uids = aft_auth_3d_db->uid.data();

    category.resize(auth_3d_db_cat.size());
    for (auth_3d_database_category& i : auth_3d_db_cat) {
        data_test_auth_3d_test_category* cat = &category[&i - auth_3d_db_cat.data()];
        cat->name = i.name.c_str();
        cat->index = -1;

        cat->uid.reserve(i.uid.size());
        for (int32_t& j : i.uid)
            if (uids[j].enabled)
                cat->uid.push_back({ uids[j].name.c_str(), uids[j].org_uid });

        if (cat->uid.size()) {
            quicksort_custom(cat->uid.data(), cat->uid.size(),
                sizeof(data_test_auth_3d_test_uid),
                data_test_auth_3d_test_uid_quicksort_compare_func);
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
        data_test_auth_3d_test_category* cat = &category.front();

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

void Auth3dTestWindow::Window() {

}
