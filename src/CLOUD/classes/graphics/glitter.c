/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../../../CRE/render_context.h"
#include "../imgui_helper.h"

extern int32_t width;
extern int32_t height;
extern bool input_locked;

extern GPM;

static const char* graphics_glitter_window_title = "Glitter##Graphics";

bool graphics_glitter_init(class_data* data, render_context* rctx) {
    graphics_glitter_dispose(data);
    return true;
}

void graphics_glitter_imgui(class_data* data) {
    float_t w = min((float_t)width / 4.0f, 148.0f);
    float_t h = min((float_t)height, 204.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(graphics_glitter_window_title, &open, window_flags);
    if (!open) {
        data->flags |= CLASS_HIDE;
        goto End;
    }
    else if (collapsed)
        goto End;

    size_t ctrl;
    size_t disp;
    size_t total_ctrl = 0;
    size_t total_disp = 0;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_QUAD);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_QUAD);
    igText("QUAD  (ctrl):%5lld", ctrl);
    igText("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_LOCUS);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_LOCUS);
    igText("LOCUS (ctrl):%5lld", ctrl);
    igText("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_LINE);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_LINE);
    igText("LINE  (ctrl):%5lld", ctrl);
    igText("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_MESH);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_MESH);
    igText("MESH  (ctrl):%5lld", ctrl);
    igText("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    igText("TOTAL (ctrl):%5lld", total_ctrl);
    igText("      (disp):%5lld", total_disp);

    data->imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
}

void graphics_glitter_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

void graphics_glitter_render(class_data* data) {

}

bool graphics_glitter_dispose(class_data* data) {
    return true;
}
