/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../../CRE/Glitter/glitter.hpp"
#include "../../../CRE/render_context.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;
extern bool input_locked;

static const char* graphics_glitter_window_title = "Glitter##Graphics";

bool graphics_glitter_init(class_data* data, render_context* rctx) {
    graphics_glitter_dispose(data);
    return true;
}

void graphics_glitter_imgui(class_data* data) {
    float_t w = min((float_t)width / 4.0f, 148.0f);
    float_t h = min((float_t)height, 204.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_glitter_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    size_t ctrl;
    size_t disp;
    size_t total_ctrl = 0;
    size_t total_disp = 0;

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD);
    ImGui::Text("QUAD  (ctrl):%5lld", ctrl);
    ImGui::Text("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS);
    ImGui::Text("LOCUS (ctrl):%5lld", ctrl);
    ImGui::Text("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE);
    ImGui::Text("LINE  (ctrl):%5lld", ctrl);
    ImGui::Text("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH);
    disp = Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH);
    ImGui::Text("MESH  (ctrl):%5lld", ctrl);
    ImGui::Text("      (disp):%5lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ImGui::Text("TOTAL (ctrl):%5lld", total_ctrl);
    ImGui::Text("      (disp):%5lld", total_disp);

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_glitter_dispose(class_data* data) {
    return true;
}
