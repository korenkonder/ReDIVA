/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"
#include "../../../CRE/render_context.h"
#include "../imgui_helper.h"

typedef struct data_view_texture {
    render_context* rctx;
} data_view_texture;

extern int32_t width;
extern int32_t height;
extern bool input_locked;

const char* data_view_texture_window_title = "Texture##Data Viewer";

bool data_view_texture_init(class_data* data, render_context* rctx) {
    data->data = force_malloc(sizeof(data_view_texture));
    data_view_texture* data_view = data->data;
    if (data_view) {
        data_view->rctx = rctx;
    }
    return true;
}

void data_view_texture_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 480.0f);
    float_t h = min((float_t)height, 480.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_view_texture_window_title, &open, 0);
    if (!open) {
        data->flags |= CLASS_HIDE;
        goto End;
    }
    else if (collapsed)
        goto End;

    data_view_texture* data_view = data->data;
    if (!data_view)
        goto End;

    render_context* rctx = data_view->rctx;

    size_t count = texture_storage_get_texture_count();
    for (size_t i = 0; i < count; i++) {
        texture* tex = texture_storage_get_texture_by_index(i);
        if (texture_get_id(tex->id))
            continue;

        igPushID_Int((int32_t)i);
        char buf[0x1000];
        sprintf_s(buf, sizeof(buf), "ID: 0x%06X; Width: %5d; Height: %5d; Mipmap Count: %2d",
            texture_get_index(tex->id), tex->width, tex->height, tex->max_mipmap_level + 1);
        igSelectable_Bool(buf, false, 0, ImVec2_Empty);
        igPopID();
    }

    data->imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
}

void data_view_texture_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

bool data_view_texture_dispose(class_data* data) {
    data_view_texture* data_view = data->data;
    if (data_view) {
    }
    free(data->data);
    return true;
}
