/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.hpp"
#include "../../../CRE/render_context.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;

const char* data_view_object_window_title = "Object##Data Viewer";

bool data_view_object_init(class_data* data, render_context* rctx) {
    data->data = rctx;
    return true;
}

void data_view_object_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 420.0f);
    float_t h = min_def((float_t)height, 480.0f);

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

    render_context* rctx = (render_context*)data->data;
    if (!rctx) {
        ImGui::End();
        return;
    }

    extern std::map<uint32_t, obj_set_handler> object_storage_data;
    for (auto& i : object_storage_data) {
        obj_set_handler* handler = &i.second;
        if (!handler->obj_set)
            continue;

        ImGui::PushID(handler->set_id);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Load Count: %3d; Name: %s",
            handler->set_id, handler->load_count, handler->name.c_str());
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    extern std::map<uint32_t, obj_set_handler> object_storage_data_modern;
    if (object_storage_data_modern.size())
        ImGui::Selectable("Modern:", false, ImGuiSelectableFlags_Disabled);

    for (auto& i : object_storage_data_modern) {
        obj_set_handler* handler = &i.second;
        if (!handler->obj_set)
            continue;

        ImGui::PushID(handler->set_id);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Load Count: %3d; Name: %s",
            handler->set_id, handler->load_count, handler->name.c_str());
        ImGui::Selectable(buf);
        ImGui::PopID();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_object_dispose(class_data* data) {
    data->data = 0;
    return true;
}
