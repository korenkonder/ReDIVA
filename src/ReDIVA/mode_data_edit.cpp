/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_data_edit.hpp"
#include "../CRE/render_context.hpp"
#include "data_edit/face_anim.hpp"
#include "data_edit/glitter_editor.hpp"
#include "data_edit/selector.hpp"
#include "mode.hpp"

extern render_context* rctx_ptr;

static bool data_edit_reset = false;
static Mode data_edit_old_mode;

bool init_data_edit() {
    rctx_ptr->render_manager->set_multisample(false);
    rctx_ptr->render_manager->set_clear(true);
    data_edit_old_mode = get_mode_info_const()->old;
    return true;
}

bool ctrl_data_edit() {
    if (data_edit_reset) {
        if (get_mode_info_const()->sub_current != MODE_SUB_DATA_EDIT_MAIN)
            shift_next_mode_sub(MODE_SUB_DATA_EDIT_MAIN);
        else
            shift_next_mode(data_edit_old_mode);
    }
    return false;
}

bool dest_data_edit() {
    rctx_ptr->render_manager->set_multisample(true);
    rctx_ptr->render_manager->set_clear(false);
    return true;
}

bool init_data_edit_main_md() { // Added
    data_edit_sel->open("DATA_TEST_EDIT");
    return true;
}

bool ctrl_data_edit_main_md() { // Added
    int32_t state = data_edit_sel_get_sub_state();
    if (state >= 0)
        shift_next_mode_sub((ModeSub)state);
    return false;
}

bool dest_data_edit_main_md() { // Added
    data_edit_sel->close();
    return true;
}

bool init_data_edit_glitter_editor_md() { // Added
    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 1.4f, 1.0f });
    cam->set_interest({ 0.0f, 1.4f, 0.0f });

    glitter_editor.open("GLITTER EDITOR", app::TASK_PRIO_HIGH);
    return true;
}

bool ctrl_data_edit_glitter_editor_md() { // Added
    return false;
}

bool dest_data_edit_glitter_editor_md() { // Added
    glitter_editor.app::Task::close();
    return true;
}

#if FACE_ANIM
bool init_data_edit_face_anim_md() { // Added
    face_anim.open("FACE ANIM", app::TASK_PRIO_HIGH);
    return true;
}

bool ctrl_data_edit_face_anim_md() { // Added
    return false;
}

bool dest_data_edit_face_anim_md() { // Added
    face_anim.close();
    return true;
}
#endif
