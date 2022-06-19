/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.hpp"
#include "../../../CRE/render_context.hpp"
#include "../imgui_helper.hpp"

struct data_view_object {
    render_context* rctx;
};

extern int32_t width;
extern int32_t height;

const char* data_view_object_window_title = "Object##Data Viewer";

bool data_view_object_init(class_data* data, render_context* rctx) {
    data->data = force_malloc(sizeof(data_view_object));
    data_view_object* data_view = (data_view_object*)data->data;
    if (data_view) {
        data_view->rctx = rctx;
    }
    return true;
}

void data_view_object_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min((float_t)width, 420.0f);
    float_t h = min((float_t)height, 480.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_view_object_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    data_view_object* data_view = (data_view_object*)data->data;
    if (!data_view) {
        ImGui::End();
        return;
    }

    render_context* rctx = data_view->rctx;

    size_t count = object_storage_get_obj_set_count();
    for (size_t i = 0; i < count; i++) {
        obj_set_handler* handler = object_storage_get_obj_set_handler_by_index(i);
        if (!handler || !handler->obj_set)
            continue;

        int32_t load_count = object_storage_get_obj_storage_load_count_by_index(i);

        ImGui::PushID(handler->set_id);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Load Count: %3d; Name: %s",
            handler->set_id, load_count, handler->name.c_str());
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_object_dispose(class_data* data) {
    data_view_object* data_view = (data_view_object*)data->data;
    if (data_view) {
    }
    free(data->data);
    return true;
}
