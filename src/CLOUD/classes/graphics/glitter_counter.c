/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter_counter.h"
#include "../../../CRE/Glitter/glitter.h"
#include "../../../CRE/Glitter/particle_manager.h"
#include "../imgui_helper.h"

typedef struct glitter_counter_struct {
    bool dispose;
    bool disposed;
    bool imgui_focus;
} glitter_counter_struct;

extern int32_t width;
extern int32_t height;
extern bool input_locked;

extern GPM;

const char* glitter_counter_window_title = "Glitter##Counter";

bool glitter_counter_enabled = false;
static glitter_counter_struct glitter_counter;

void glitter_counter_dispose() {
    glitter_counter_enabled = false;
    glitter_counter.imgui_focus = false;
    glitter_counter.dispose = false;
    glitter_counter.disposed = true;
}

void glitter_counter_init() {
    if (glitter_counter_enabled || glitter_counter.dispose)
        glitter_counter_dispose();

    glitter_counter_enabled = true;
}

void glitter_counter_imgui() {
    if (!glitter_counter_enabled) {
        if (!glitter_counter.disposed)
            glitter_counter.dispose = true;
        return;
    }
    else if (glitter_counter.disposed)
        glitter_counter_enabled = true;

    float_t w = min((float_t)width / 4.0f, 148.0f);
    float_t h = min((float_t)height, 204.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    glitter_counter.imgui_focus = false;
    if (!igBegin(glitter_counter_window_title, &glitter_counter_enabled, window_flags)) {
        glitter_counter.dispose = true;
        igEnd();
        return;
    }

    size_t ctrl;
    size_t disp;
    size_t total_ctrl = 0;
    size_t total_disp = 0;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_QUAD);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_QUAD);
    igText("QUAD  (ctrl):%6lld", ctrl);
    igText("      (disp):%6lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_LOCUS);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_LOCUS);
    igText("LOCUS (ctrl):%6lld", ctrl);
    igText("      (disp):%6lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_LINE);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_LINE);
    igText("LINE  (ctrl):%6lld", ctrl);
    igText("      (disp):%6lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    ctrl = glitter_particle_manager_get_ctrl_count(GPM_VAL, GLITTER_PARTICLE_MESH);
    disp = glitter_particle_manager_get_disp_count(GPM_VAL, GLITTER_PARTICLE_MESH);
    igText("MESH  (ctrl):%6lld", ctrl);
    igText("      (disp):%6lld", disp);
    total_ctrl += ctrl;
    total_disp += disp;

    igText("TOTAL (ctrl):%6lld", total_ctrl);
    igText("      (disp):%6lld", total_disp);

    glitter_counter.imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

void glitter_counter_input() {
    if (!glitter_counter_enabled)
        return;

    input_locked |= glitter_counter.imgui_focus;
}

void glitter_counter_render() {
    if (glitter_counter.dispose) {
        glitter_counter_dispose();
        return;
    }
    else if (!glitter_counter_enabled)
        return;
}
