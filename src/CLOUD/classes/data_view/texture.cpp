/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.hpp"
#include "../../../CRE/render_context.hpp"
#include "../imgui_helper.hpp"

struct data_view_texture {
    render_context* rctx;
};

extern int32_t width;
extern int32_t height;

const char* data_view_texture_window_title = "Texture##Data Viewer";

bool data_view_texture_init(class_data* data, render_context* rctx) {
    data->data = force_malloc(sizeof(data_view_texture));
    data_view_texture* data_view = (data_view_texture*)data->data;
    if (data_view) {
        data_view->rctx = rctx;
    }
    return true;
}

void data_view_texture_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 580.0f);
    float_t h = min((float_t)height, 480.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_view_texture_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    data_view_texture* data_view = (data_view_texture*)data->data;
    if (!data_view) {
        ImGui::End();
        return;
    }

    render_context* rctx = data_view->rctx;

    size_t count = texture_storage_get_texture_count();
    for (size_t i = 0; i < count; i++) {
        texture* tex = texture_storage_get_texture_by_index(i);
        if (tex->id.id)
            continue;

        ImGui::PushID(tex->id.index);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Init Count: %3d; Width: %5d; Height: %5d; Mipmap Count: %2d",
            tex->id.index, tex->init_count, tex->width, tex->height, tex->max_mipmap_level + 1);
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_texture_dispose(class_data* data) {
    data_view_texture* data_view = (data_view_texture*)data->data;
    if (data_view) {
    }
    free(data->data);
    return true;
}
