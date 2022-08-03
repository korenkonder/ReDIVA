/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "frame_speed.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;
extern float_t frame_speed;

static const char* information_frame_speed_window_title = "FrameSpeedWindow##Information";

bool information_frame_speed_init(class_data* data, render_context* rctx) {
    information_frame_speed_dispose(data);
    return true;
}

void information_frame_speed_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 240.0f);
    float_t h = min((float_t)height, 58.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(information_frame_speed_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::ColumnSliderFloatButton("frame speed", &frame_speed, 0.01f, 0.0f, 3.0f, 0.1f, "%.2f", 0);

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool information_frame_speed_dispose(class_data* data) {
    return true;
}
