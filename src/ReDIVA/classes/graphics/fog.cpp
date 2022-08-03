/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../../../KKdLib/vec.hpp"
#include "../../../CRE/clear_color.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;

static const char* graphics_fog_window_title = "Fog##Graphics";

bool graphics_fog_init(class_data* data, render_context* rctx) {
    graphics_fog_dispose(data);
    data->data = rctx;
    return true;
}

void graphics_fog_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 200.0f);
    float_t h = min((float_t)height, 84.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_fog_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    render_context* rctx = (render_context*)data->data;
    fog* fog = rctx->fog;

    ImGuiColorEditFlags color_edit_flags = 0;
    color_edit_flags |= ImGuiColorEditFlags_NoLabel;
    color_edit_flags |= ImGuiColorEditFlags_NoSidePreview;
    color_edit_flags |= ImGuiColorEditFlags_NoDragDrop;

    if (ImGui::ButtonEnterKeyPressed("Reset"))
        light_param_data_storage_data_set_default_light_param(LIGHT_PARAM_DATA_STORAGE_FOG);

    ImGui::GetContentRegionAvailSetNextItemWidth();

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_fog_dispose(class_data* data) {
    return true;
}
