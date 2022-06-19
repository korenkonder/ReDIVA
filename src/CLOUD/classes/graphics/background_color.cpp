/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "background_color.hpp"
#include "../../../KKdLib/vec.hpp"
#include "../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;
extern vec4u8 clear_color;
extern bool set_clear_color;

static const char* graphics_background_color_window_title = "Background Color##Graphics";

bool graphics_background_color_init(class_data* data, render_context* rctx) {
    graphics_background_color_dispose(data);
    return true;
}

void graphics_background_color_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 360.0f);
    float_t h = min((float_t)height, 100.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_background_color_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    ImGuiColorEditFlags color_edit_flags = 0;
    color_edit_flags |= ImGuiColorEditFlags_NoLabel;
    color_edit_flags |= ImGuiColorEditFlags_NoSidePreview;
    color_edit_flags |= ImGuiColorEditFlags_NoDragDrop;

    ImGui::GetContentRegionAvailSetNextItemWidth();
    vec4 _clear_color;
    vec4u8_to_vec4(clear_color, _clear_color);
    vec4_mult_scalar(_clear_color, (float_t)(1.0 / 255.0), _clear_color);
    if (ImGui::ColorEdit4("##Background Color", (float_t*)&_clear_color, color_edit_flags)) {
        vec4_clamp_scalar(_clear_color, 0.0f, 1.0f, _clear_color);
        vec4_mult_scalar(_clear_color, 255.0f, _clear_color);
        vec4_to_vec4u8(_clear_color, clear_color);
    }
    ImGui::Checkbox("Set Clear Color", &set_clear_color);

    if (ImGui::ButtonEnterKeyPressed("Reset Color")) {
        clear_color = { 0x60, 0x60, 0x60, 0xFF };
        set_clear_color = true;
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_background_color_dispose(class_data* data) {
    return true;
}
