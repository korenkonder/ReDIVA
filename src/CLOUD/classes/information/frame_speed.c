/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "frame_speed.h"
#include "../../../KKdLib/vec.h"
#include "../imgui_helper.h"

extern int32_t width;
extern int32_t height;
extern bool input_locked;
extern float_t frame_speed;

static const char* information_frame_speed_window_title = "FrameSpeedWindow##Information";

bool information_frame_speed_init(class_data* data, render_context* rctx) {
    information_frame_speed_dispose(data);
    return true;
}

void information_frame_speed_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 240.0f);
    float_t h = min((float_t)height, 58.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(information_frame_speed_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        igEnd();
        return;
    }
    else if (collapsed) {
        igEnd();
        return;
    }

    ImGuiColorEditFlags color_edit_flags = 0;
    color_edit_flags |= ImGuiColorEditFlags_NoLabel;
    color_edit_flags |= ImGuiColorEditFlags_NoSidePreview;
    color_edit_flags |= ImGuiColorEditFlags_NoDragDrop;

    imguiGetContentRegionAvailSetNextItemWidth();
    imguiColumnSliderFloat("frame speed", &frame_speed, 0.01f, 0.0f, 3.0f, "%.2f", 0, true);

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

void information_frame_speed_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

void information_frame_speed_render(class_data* data) {

}

bool information_frame_speed_dispose(class_data* data) {
    return true;
}
