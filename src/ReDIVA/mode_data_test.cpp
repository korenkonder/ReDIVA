/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_data_test.hpp"
#include "../CRE/clear_color.hpp"
#include "../CRE/render_context.hpp"
#include "../CRE/stage.hpp"
#include "data_edit/rob_chara_adjust.hpp"
#include "data_test/auth_2d_test.hpp"
#include "data_test/auth_3d_test.hpp"
#include "data_test/glitter_test.hpp"
#include "data_test/misc_test.hpp"
#include "data_test/motion_test.hpp"
#include "data_test/object_test.hpp"
#include "data_test/opd_test.hpp"
#include "data_test/rob_osage_test.hpp"
#include "data_test/selector.hpp"
#include "data_test/stage_test.hpp"
#include "mode.hpp"
#include "wait_screen.hpp"

static bool data_test_reset = false;

static Mode data_test_old_mode;

extern render_context* rctx_ptr;

bool init_data_test() {
    rctx_ptr->render_manager->set_multisample(false);
    rctx_ptr->render_manager->set_clear(true);
    data_test_old_mode = get_mode_info_const()->old;
    //task_information_ptr->sub_1403BB780(0);
    task_wait_screen_set_load_loop_none();
    task_rob_manager_open();
    //touch_util::touch_reaction_set_enable(false);
    //net_alw_detail::context_ptr_get()->field_C8(2);
    return true;
}

bool ctrl_data_test() {
    if (data_test_reset) {
        if (get_mode_info_const()->sub_current != MODE_SUB_DATA_TEST_MAIN)
            shift_next_mode_sub(MODE_SUB_DATA_TEST_MAIN);
        else
            shift_next_mode(data_test_old_mode);
    }
    return false;
}

bool dest_data_test() {
    if (!task_rob_manager_close())
        return false;

    rctx_ptr->render_manager->set_multisample(true);
    rctx_ptr->render_manager->set_clear(false);
    //touch_util::touch_reaction_set_enable(true);
    return true;
}

bool init_data_test_main_md() {
    data_test_sel->open("DATA_TEST_MAIN");
    return true;
}

bool ctrl_data_test_main_md() {
    int32_t state = data_test_sel_get_sub_state();
    if (state >= 0)
        shift_next_mode_sub((ModeSub)state);
    return false;
}

bool dest_data_test_main_md() {
    data_test_sel->close();
    return true;
}

bool init_data_test_misc_md() {
    task_data_test_misc->open("DATA_TEST_MISC");
    return true;
}

bool ctrl_data_test_misc_md() {
    return false;
}

bool dest_data_test_misc_md() {
    task_data_test_misc->close();
    return true;
}

bool init_data_test_obj_md() {
    task_data_test_obj->open("DATA_TEST_OBJ");
    task_stage_open("DATA_TEST_OBJ_STAGE");
    return true;
}

bool ctrl_data_test_obj_md() {
    return false;
}

bool dest_data_test_obj_md() {
    task_data_test_obj->close();
    task_stage_close();
    return true;
}

bool init_data_test_stg_md() {
    clear_color = 0xFF606060;

    camera* cam = rctx_ptr->camera;
    cam->set_view_point({ 0.0f, 0.88f, 4.3f });
    cam->set_interest({ 0.0f, 1.0f, 0.0f });

    dtm_stg->open("DATA_TEST_STAGE");
    dtw_stg_init();
    return true;
}

bool ctrl_data_test_stg_md() {
    return false;
}

bool dest_data_test_stg_md() {
    clear_color = 0xFF000000;

    dtm_stg_unload();
    dtw_stg_unload();
    return true;
}

bool init_data_test_mot_md() {
    data_test_mot->open("TASK_DATA_TEST_MOT");
    rob_osage_test->open("ROB_OSAGE_TEST");
    rob_chara_adjust->open("ROB_CHARA_ADJUST");
    dtm_stg_load(0);
    dtw_stg_load(true);
    return true;
}

bool ctrl_data_test_mot_md() {
    return false;
}

bool dest_data_test_mot_md() {
    data_test_mot->close();
    rob_osage_test->close();
    rob_chara_adjust->close();
    dtm_stg_unload();
    dtw_stg_unload();
    return true;
}

bool init_data_test_collision_md() {
    return true;
}

bool ctrl_data_test_collision_md() {
    return false;
}

bool dest_data_test_collision_md() {
    return true;
}

bool init_data_test_spr_md() {
    return true;
}

bool ctrl_data_test_spr_md() {
    return false;
}

bool dest_data_test_spr_md() {
    return true;
}

bool init_data_test_aet_md() {
    clear_color = 0xFF606060;
    dtm_aet_load();
    return true;
}

bool ctrl_data_test_aet_md() {
    return false;
}

bool dest_data_test_aet_md() {
    clear_color = 0xFF000000;
    dtm_aet_unload();
    return true;
}

bool init_data_test_auth_3d_md() {
    auth_3d_test_task->open("AUTH3DTEST");
    return true;
}

bool ctrl_data_test_auth_3d_md() {
    return false;
}

bool dest_data_test_auth_3d_md() {
    auth_3d_test_task->close();
    return true;
}

bool init_data_test_chr_md() {
    return true;
}

bool ctrl_data_test_chr_md() {
    return false;
}

bool dest_data_test_chr_md() {
    return true;
}

bool init_data_test_item_md() {
    return true;
}

bool ctrl_data_test_item_md() {
    return false;
}

bool dest_data_test_item_md() {
    return true;
}

bool init_data_test_perf_md() {
    return true;
}

bool ctrl_data_test_perf_md() {
    return false;
}

bool dest_data_test_perf_md() {
    return true;
}

bool init_data_test_pv_script_md() {
    return true;
}

bool ctrl_data_test_pv_script_md() {
    return false;
}

bool dest_data_test_pv_script_md() {
    return true;
}

bool init_data_test_print_md() {
    return true;
}

bool ctrl_data_test_print_md() {
    return false;
}

bool dest_data_test_print_md() {
    return true;
}

bool init_data_test_card_md() {
    return true;
}

bool ctrl_data_test_card_md() {
    return false;
}

bool dest_data_test_card_md() {
    return true;
}

bool init_data_test_opd_md() {
    task_rob_manager_close();
    return task_data_test_opd->open();
}

bool ctrl_data_test_opd_md() {
    return !task_data_test_opd->check_alive();
}

bool dest_data_test_opd_md() {
    task_rob_manager_open();
    task_data_test_opd->close();
    return true;
}

bool init_data_test_slider_md() {
    return true;
}

bool ctrl_data_test_slider_md() {
    return false;
}

bool dest_data_test_slider_md() {
    return true;
}

bool init_data_test_glitter_md() {
    task_data_test_glitter_particle->open("DATA_TEST_PARTICLE");
    dtm_stg_load(0);
    dtw_stg_load(true);
    return true;
}

bool ctrl_data_test_glitter_md() {
    return false;
}

bool dest_data_test_glitter_md() {
    dtw_stg_unload();
    dtm_stg_unload();
    //if (data_test_chr->check_alive())
    //    data_test_chr->close();
    task_data_test_glitter_particle->close();
    return true;
}

bool init_data_test_graphics_md() {
    return true;
}

bool ctrl_data_test_graphics_md() {
    return false;
}

bool dest_data_test_graphics_md() {
    return true;
}

bool init_data_test_collection_card_md() {
    return true;
}

bool ctrl_data_test_collection_card_md() {
    return false;
}

bool dest_data_test_collection_card_md() {
    return true;
}

bool init_data_test_mode_main_md() {
    return true;
}

bool ctrl_data_test_mode_main_md() {
    return false;
}

bool dest_data_test_mode_main_md() {
    return true;
}
