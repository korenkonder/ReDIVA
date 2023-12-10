/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.hpp"
#include "../../../KKdLib/sort.hpp"
#include "../../../KKdLib/str_utils.hpp"
#include "../../../CRE/task.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;
extern float_t task;

static const char* information_task_window_title = "Task##Information";

static int32_t task_sort_by_calc(void const* src1, void const* src2);
static int32_t task_sort_by_disp(void const* src1, void const* src2);
static int32_t task_sort_by_name(void const* src1, void const* src2);

bool information_task_init(class_data* data, render_context* rctx) {
    data->data = force_malloc<int32_t>(2);
    information_task_dispose(data);
    return true;
}

void information_task_imgui(class_data* data) {
    if (!data->data)
        return;

    int32_t& task_sort = ((int32_t*)data->data)[0];
    bool& show_priority = *(bool*)&((int32_t*)data->data)[1];

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 496.0f;
    float_t h = 240.0f;

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(information_task_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    std::vector<app::Task*> vec;
    for (int32_t i = 0; ; i++) {
        app::Task* t = app::TaskWork::get_task_by_index(i);
        if (!t)
            break;

        vec.push_back(t);
    }

    switch (task_sort) {
    case 2:
        quicksort_custom(vec.data(), vec.size(), sizeof(app::Task*), task_sort_by_name);
        break;
    case 3:
        quicksort_custom(vec.data(), vec.size(), sizeof(app::Task*), task_sort_by_calc);
        break;
    case 4:
        quicksort_custom(vec.data(), vec.size(), sizeof(app::Task*), task_sort_by_disp);
        break;
    }

    ImVec2 size = ImGui::GetContentRegionAvail();
    size.y -= font->FontSize + style.ItemSpacing.y;
    ImGui::BeginListBox("#Tasks", size);
    char buf[0x100];
    if (show_priority)
        for (app::Task*& i : vec) {
            sprintf_s(buf, sizeof(buf), "%1d %-30s % 7d(% 7d) % 7d(% 7d)", i->priority,
                i->get_name(), i->get_calc_time(), i->get_calc_time_max(), i->get_disp_time(), i->get_disp_time_max());
            ImGui::Selectable(buf);
        }
    else
        for (app::Task*& i : vec) {
            sprintf_s(buf, sizeof(buf), "%-32s % 7d(% 7d) % 7d(% 7d)",
                i->get_name(), i->get_calc_time(), i->get_calc_time_max(), i->get_disp_time(), i->get_disp_time_max());
            ImGui::Selectable(buf);
        }
    ImGui::EndListBox();
    vec.clear();

    int32_t task_sort_new = task_sort;
    ImGui::Text("(%c)exec", task_sort == 0 ? '*' : ' ');
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        task_sort_new = 0;

    ImGui::SameLine();

    ImGui::Text("(%c)tree", task_sort == 1 ? '*' : ' ');
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        task_sort_new = 1;

    ImGui::SameLine();

    ImGui::Text("(%c)name", task_sort == 2 ? '*' : ' ');
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        task_sort_new = 2;

    ImGui::SameLine();

    ImGui::Text("(%c)calc", task_sort == 3 ? '*' : ' ');
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        task_sort_new = 3;

    ImGui::SameLine();

    ImGui::Text("(%c)disp", task_sort == 4 ? '*' : ' ');
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        task_sort_new = 4;

    ImGui::SameLine();

    bool text_tranparency = !show_priority;
    if (text_tranparency)
        ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 0.0f, 0.0f, 0.0f });
    ImGui::Text("(%c)show prio", show_priority ? '*' : ' ');
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        show_priority ^= true;
    if (text_tranparency)
        ImGui::PopStyleColor();

    task_sort = task_sort_new;

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool information_task_dispose(class_data* data) {
    return true;
}

static int32_t task_sort_by_calc(void const* src1, void const* src2) {
    app::Task* t1 = *(app::Task**)src1;
    app::Task* t2 = *(app::Task**)src2;
    return t2->calc_time - t1->calc_time;
}

static int32_t task_sort_by_disp(void const* src1, void const* src2) {
    app::Task* t1 = *(app::Task**)src1;
    app::Task* t2 = *(app::Task**)src2;
    return t2->disp_time - t1->disp_time;
}

static int32_t task_sort_by_name(void const* src1, void const* src2) {
    app::Task* t1 = *(app::Task**)src1;
    app::Task* t2 = *(app::Task**)src2;
    return str_utils_compare(t1->name, t2->name);
}
