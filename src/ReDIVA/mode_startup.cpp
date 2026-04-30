/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_startup.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/sound.hpp"
#include "data_initialize.hpp"
#include "input_state.hpp"
#include "mask_screen.hpp"
#include "mode.hpp"
#include "system_startup.hpp"
#include "test_mode.hpp"

static int32_t startup_error_code;

bool init_startup() {
    startup_error_code = false;
    return true;
}

bool ctrl_startup() {
    return false;
}

static void sub_14066C670() {

}

static uint32_t sub_14066D190() {
    return 0;
}

bool dest_startup() {
    if (get_mode_info_const()->next != MODE_APP_ERROR && !is_test_mode_start())
        sub_14066C670();

    if (is_test_mode_start())
        shift_next_mode(MODE_TEST_MODE);
    else {
        sub_14066D190();

        Mode state = get_mode_info_const()->next == MODE_TEST_MODE
            ? MODE_TEST_MODE : MODE_ADVERTISE;
        shift_next_mode(state);
    }
    return true;
}

bool init_data_initialize() {
    task_data_init_open();
    task_auth_3d_open();
    aet_manager_open();
    //no_good_open();
    return true;
}

bool ctrl_data_initialize() {
    return task_data_init_check_state();
}

bool dest_data_initialize() {
    task_data_init_close();
    return true;
}

bool init_system_startup() {
    task_system_startup_open();
    task_pv_db_open();

    /*if (!sub_14066E820())
        game_state_set_error_code(949);*/

    ScreenParam& screen_param = get_screen_param();

    //sub_14066D190();

    if (!is_test_mode_start()) {
        /*if (screen_param.mode != SCREEN_MODE_HD)
            game_state_set_error_code(910);*/

            /*if (sub_14066C9A0()) {
                sub_14066FA90();
                game_state_set_error_code(90);
            }*/
    }

    /*if (sub_1401E8A40())
        sub_1406A1DE0(task_touch_ptr[0], 0);*/

        /*if (sub_1401E89A0())
            task_aime_get()->sub_14017A890(9);*/

            //task_lamp_ctrl_get()->sub_1403C8B40();

            /*if (sub_1401E8A50())
                task_slider_control_get()->sub_140618A60();*/

                /*sub_1403D4760();*/

                /*const Wrap_collection* wrap_collection = wrap_collection_get();
                if (wrap_collection && wrap_collection->printer.get_value())
                    task_printer_get()->sub_1404C0F90();*/
    return true;
}

bool ctrl_system_startup() {
    return !!system_startup_ready;
}

bool dest_system_startup() {
    task_system_startup_close();
    //task_closing_ptr->open("CLOSING");
    //task_touch_area_ptr->open("TOUCH AREA", app::TASK_PRIO_HIGH);
    //sound_volume_get();
    //sub_140623810();
    //sub_1406A1F30();

    task_mask_screen_open();
    shift_next_mode_sub(MODE_SUB_WARNING);

    if (!is_test_mode_start()) {
        //sub_14066D190();

        /*uint8_t v2[4];
        net_alw_detail::context_ptr_get()->field_90(v2);
        if (!v2[1] && !v2[3]) {
            startup_error_code = 8005;
            shift_next_mode_sub(MODE_SUB_SYSTEM_STARTUP_ERROR);
            sub_140022740();
        }*/
    }
    return true;
}

bool init_system_startup_error() {
    //task_mode_app_error_open(startup_error_code);
    return true;
}

bool ctrl_system_startup_error() {
    const ModeInfo* mode_info = get_mode_info_const();
    if (mode_info->current || mode_info->sub_current != MODE_SUB_SYSTEM_STARTUP_ERROR)
        return false;

    const InputState* input_state = input_state_get(0);
    return input_state->CheckDown(INPUT_BUTTON_JVS_START) && input_state->CheckTapped(INPUT_BUTTON_JVS_TEST);
}

bool dest_system_startup_error() {
    //task_mode_app_error_close();
    shift_next_mode_sub(MODE_SUB_WARNING);
    return true;
}

bool init_warning() {
    /*if (sub_14066F660() && !is_test_mode_start() && !sub_14066D030())
        task_warning.open("WARNING");*/
    return true;
}

bool ctrl_warning() {
    return true;
    //return !task_warning.check_alive();
}

bool dest_warning() {
    //task_warning.close();
    return true;
}
