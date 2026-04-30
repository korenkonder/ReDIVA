/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_test_mode.hpp"
#include "../CRE/sound.hpp"
#include "mask_screen.hpp"
#include "test_mode.hpp"
#include "wait_screen.hpp"

bool init_test_mode() {
    sys_am_set_test_mode_starting(true);
    sound_work_reset_all_se();

    if (!is_test_mode_start()) {
        //sub_1401F4440();
        //sub_1401E87E0();
    }

    sys_am_set_test_mode_starting(1);

    //task_information_ptr->sub_1403BB780(0);

    /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
    task_lamp_ctrl->field_74 = 0;
    task_lamp_ctrl->field_78 = color_white;
    task_lamp_ctrl->field_7C = 0;*/

    //task_slider_control_get()->sub_140618980(0);

    task_wait_screen_set_load_loop_none();
    task_mask_screen_fade_in(0.0f, 0);

    sound_work_reset_all_se();
    for (int32_t i = 0; i < 3; i++)
        sound_work_release_stream(i++);
    return true;
}

bool ctrl_test_mode() {
    return false;
}

bool dest_test_mode() {
    //sub_1401F4420();
    sys_am_set_test_mode_starting(false);
    return true;
}
