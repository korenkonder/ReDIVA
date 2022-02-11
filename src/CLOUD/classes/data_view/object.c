/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.h"
#include "../../../CRE/render_context.h"
#include "../imgui_helper.h"

typedef struct data_view_object {
    render_context* rctx;
} data_view_object;

extern int32_t width;
extern int32_t height;
extern bool input_locked;

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
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 420.0f);
    float_t h = min((float_t)height, 480.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_view_object_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        igEnd();
        return;
    }
    else if (collapsed) {
        igEnd();
        return;
    }

    data_view_object* data_view = (data_view_object*)data->data;
    if (!data_view) {
        igEnd();
        return;
    }

    render_context* rctx = data_view->rctx;

    size_t count = object_storage_get_object_set_count();
    for (size_t i = 0; i < count; i++) {
        object_set* set = object_storage_get_object_set_by_index(i);
        int32_t load_count = object_storage_get_object_set_load_count_by_index(i);

        igPushID_Int(set->id);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%08X; Load Count: %3d; Name: %s",
            set->id, load_count, string_data(&set->name));
        igSelectable_Bool(buf, false, 0, ImVec2_Empty);
        igPopID();
    }

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

void data_view_object_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

bool data_view_object_dispose(class_data* data) {
    data_view_object* data_view = (data_view_object*)data->data;
    if (data_view) {
    }
    free(data->data);
    return true;
}
