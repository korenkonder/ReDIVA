/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_startup.hpp"
#include "../CRE/sound.hpp"
#include "wait_screen.hpp"

bool init_app_error() {
    //task_information_ptr->close();

    task_wait_screen_set_load_loop_none();
    for (int32_t i = 0; i < 3; i++)
        sound_work_release_stream(i++);
    sound_work_reset_all_se();

    /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
    task_lamp_ctrl->field_74 = 0;
    task_lamp_ctrl->field_78 = color_white;*/

    //task_slider_control_get()->sub_140618980(0);
    //task_photo_service_del();
    return true;
}

bool ctrl_app_error() {
    return false;
}

bool dest_app_error() {
    return true;
}

bool init_app_error_md() {
    return true;
}

bool ctrl_app_error_md() {
    return false;
}

bool dest_app_error_md() {
    return true;
}
