/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "background_color.h"
#include "../imgui_helper.h"

typedef struct background_color_struct {
    bool dispose;
    bool disposed;
    bool imgui_focus;
} background_color_struct;

extern int32_t width;
extern int32_t height;
extern bool input_locked;
extern vec3 back3d_color;
extern bool set_clear_color;

const char* background_color_window_title = "Background Color";

bool background_color_enabled = false;
static background_color_struct background_color;

void background_color_dispose() {
    background_color_enabled = false;
    background_color.imgui_focus = false;
    background_color.dispose = false;
    background_color.disposed = true;
}

void background_color_init() {
    if (background_color_enabled || background_color.dispose)
        background_color_dispose();

    background_color_enabled = true;
}

void background_color_imgui() {
    if (!background_color_enabled) {
        if (!background_color.disposed)
            background_color.dispose = true;
        return;
    }
    else if (background_color.disposed)
        background_color_enabled = true;

    float_t w = min((float_t)width / 4.0f, 360.0f);
    float_t h = min((float_t)height, 100.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    background_color.imgui_focus = false;
    if (!igBegin(background_color_window_title, &background_color_enabled, window_flags)) {
        background_color.dispose = true;
        igEnd();
        return;
    }

    ImVec2 temp;
    igGetContentRegionAvail(&temp);

    ImGuiColorEditFlags color_edit_flags = 0;
    color_edit_flags |= ImGuiColorEditFlags_NoLabel;
    color_edit_flags |= ImGuiColorEditFlags_NoSidePreview;
    color_edit_flags |= ImGuiColorEditFlags_NoDragDrop;

    igSetNextItemWidth(temp.x);
    igColorEdit3("##Background Color", (float_t*)&back3d_color, color_edit_flags);
    igCheckbox("Set Clear Color", &set_clear_color);

    if (imguiButton("Reset Color", ImVec2_Empty)) {
        back3d_color = (vec3){ 0.74117647f, 0.74117647f, 0.74117647f };
        set_clear_color = true;
    }

    background_color.imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

void background_color_input() {
    if (!background_color_enabled)
        return;

    input_locked |= background_color.imgui_focus;
}

void background_color_render() {
    if (background_color.dispose) {
        background_color_dispose();
        return;
    }
    else if (!background_color_enabled)
        return;
}
