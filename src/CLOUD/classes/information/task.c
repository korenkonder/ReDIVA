/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.h"
#include "../../../KKdLib/sort.h"
#include "../../../KKdLib/str_utils.h"
#include "../../../CRE/task.h"
#include "../imgui_helper.h"

extern int32_t width;
extern int32_t height;
extern float_t task;

static const char* information_task_window_title = "Task##Information";

static int32_t task_sort_by_calc(void const* src1, void const* src2);
static int32_t task_sort_by_disp(void const* src1, void const* src2);
static int32_t task_sort_by_name(void const* src1, void const* src2);

bool information_task_init(class_data* data, render_context* rctx) {
    data->data = force_malloc_s(int32_t, 1);
    information_task_dispose(data);
    return true;
}

void information_task_imgui(class_data* data) {
    int32_t& task_sort = *(int32_t*)data->data;
    if (!data->data)
        return;

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

    std::vector<Task*> vec;
    for (int32_t i = 0; ; i++) {
        Task* t = TaskWork::GetTaskByIndex(i);
        if (!t)
            break;

        vec.push_back(t);
    }

    switch (task_sort) {
    case 2:
        quicksort_custom(vec.data(), vec.size(), sizeof(Task*), task_sort_by_name);
        break;
    case 3:
        quicksort_custom(vec.data(), vec.size(), sizeof(Task*), task_sort_by_calc);
        break;
    case 4:
        quicksort_custom(vec.data(), vec.size(), sizeof(Task*), task_sort_by_disp);
        break;
    }

    ImVec2 size = ImGui::GetContentRegionAvail();
    size.y -= font->FontSize + style.ItemSpacing.y;
    ImGui::BeginListBox("#Tasks", size);
    for (Task*& i : vec) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "%-32s % 7d(% 7d) % 7d(% 7d)",
            i->GetName(), i->GetCalcTime(), i->GetCalcTimeMax(), i->GetDispTime(), i->GetDispTimeMax());
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

    task_sort = task_sort_new;

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool information_task_dispose(class_data* data) {
    return true;
}

static int32_t task_sort_by_calc(void const* src1, void const* src2) {
    Task* t1 = *(Task**)src1;
    Task* t2 = *(Task**)src2;
    return t2->calc_time - t1->calc_time;
}

static int32_t task_sort_by_disp(void const* src1, void const* src2) {
    Task* t1 = *(Task**)src1;
    Task* t2 = *(Task**)src2;
    return t2->disp_time - t1->disp_time;
}

static int32_t task_sort_by_name(void const* src1, void const* src2) {
    Task* t1 = *(Task**)src1;
    Task* t2 = *(Task**)src2;
    return str_utils_compare(t1->name, t2->name);
}
