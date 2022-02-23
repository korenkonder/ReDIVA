/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "background_color.h"
#include "../../../KKdLib/vec.h"
#include "../imgui_helper.h"

extern int32_t width;
extern int32_t height;
extern vec3 back3d_color;
extern bool set_clear_color;

static const char* graphics_background_color_window_title = "Background Color##Graphics";

bool graphics_background_color_init(class_data* data, render_context* rctx) {
    graphics_background_color_dispose(data);
    return true;
}

void graphics_background_color_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 360.0f);
    float_t h = min((float_t)height, 100.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(graphics_background_color_window_title, &open, window_flags);
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
    igColorEdit3("##Background Color", (float_t*)&back3d_color, color_edit_flags);
    igCheckbox("Set Clear Color", &set_clear_color);

    if (imguiButton("Reset Color", ImVec2_Empty)) {
        back3d_color = { (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0), (float_t)(96.0 / 255.0) };
        set_clear_color = true;
    }

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

bool graphics_background_color_dispose(class_data* data) {
    return true;
}
