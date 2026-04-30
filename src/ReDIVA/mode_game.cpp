/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_game.hpp"
#include "../CRE/render_context.hpp"
#include "pv_game/player_data.hpp"
#include "pv_game/pv_game.hpp"
#include "mask_screen.hpp"
#include "mode.hpp"
#include "test_mode.hpp"
#include "wait_screen.hpp"
#include "x_pv_game.hpp"

extern render_context* rctx_ptr;

#if PV_DEBUG
bool pv_x;
#endif
#if BAKE_X_PACK
bool pv_x_bake;
#endif

bool init_game() {
    //task_information_ptr->sub_1403BB780(4);
    //sub_14066C660();
    task_rob_manager_open();
    return true;
}

bool ctrl_game() {
    return false;
}

bool dest_game() {
    if (!task_pv_game_close() || !task_rob_manager_close())
        return false;

    //sub_1403935A0();
    //task_sel_ticket_close();
    //sel_vocal_change_get()->close();
    return true;
}

bool init_selector() {
#if PV_DEBUG
    if (!pv_x) {
        pv_game_selector_init();
        pv_game_selector_get()->open("PVGAME SELECTOR", app::TASK_PRIO_HIGH);
        return true;
    }
#endif

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (!baker) {
            x_pv_game_baker_init();
            x_pv_game_baker_get()->open("X PVGAME BAKER", app::TASK_PRIO_HIGH);
        }
        else
            baker->next = true;
        return true;
    }
#endif

    x_pv_game_selector_init();
    x_pv_game_selector_get()->open("X PVGAME SELECTOR", app::TASK_PRIO_HIGH);
    return true;

    /*task_information_ptr->sub_1403BB780(3);
    sub_1402B7880()->sub_1402BAE80();
    module_data_handler_data_add_all_modules();
    //sub_14022CE00()->sub_14022DC10();
    //sub_1404FF3D0();
    //sel_main_get()->open(false);
    //task_slider_control_get()->sub_140618980(1);
    return true;*/
}

bool ctrl_selector() {
#if PV_DEBUG
    if (!pv_x) {
        PVGameSelector* sel = pv_game_selector_get();
        if (sel->exit) {
            if (sel->start && pv_game_init()) {
                struc_717* v0 = sub_14038BB30();
                v0->field_0.stage_index = 0;

                struc_716* v2 = v0->get_stage();
                v2->field_2C.pv_id = sel->pv_id;
                v2->field_2C.difficulty = sel->difficulty;
                v2->field_2C.edition = sel->edition;
                v2->field_2C.score_percentage_clear = 5000;
                v2->field_2C.life_gauge_safety_time = 40;
                v2->field_2C.life_gauge_border = 30;

                for (int32_t i = 0; i < ROB_ID_MAX; i++) {
                    v2->field_2C.field_4C[i] = sel->modules[i];
                    v2->field_2C.field_64[i] = sel->modules[i];
                    v2->field_2C.field_7C[i] = {};
                    v2->field_2C.field_DC[i] = {};
                }

                v0->field_0.no_fail = true;
                v0->field_0.watch = true;
                v0->field_0.success = sel->success;

                PlayerData* player_data = player_data_array_get(0);
                player_data->field_0 = true;

                sub_14038BB30()->field_0.option = 0;

                task_pv_game_init_pv();
                shift_next_mode_sub(MODE_SUB_GAME_MAIN);
            }
            else
                shift_next_mode(MODE_ADVERTISE);
            return true;
        }
        return false;
    }
#endif

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (baker->wait)
            return false;

        const int32_t pv_ids[] = {
            801,
            802,
            803,
            804,
            805,
            806,
            807,
            808,
            809,
            810,
            811,
            812,
            813,
            814,
            815,
            816,
            817,
            818,
            819,
            820,
            821,
            822,
            823,
            824,
            825,
            826,
            827,
            828,
            829,
            830,
            831,
            832,
        };

        if (baker->start && x_pv_game_init()) {
            x_pv_game_get()->open("PVGAME", app::TASK_PRIO_HIGH);
            x_pv_game_get()->load(pv_ids[baker->index], pv_ids[baker->index] % 100, baker->charas, baker->modules);
            shift_next_mode_sub(MODE_SUB_GAME_MAIN);
        }
        else
            shift_next_mode(MODE_ADVERTISE);
        return true;
    }
#endif

    XPVGameSelector* sel = x_pv_game_selector_get();
    if (sel->exit) {
        if (sel->start && x_pv_game_init()) {
            x_pv_game_get()->open("PVGAME", app::TASK_PRIO_HIGH);
            x_pv_game_get()->load(sel->pv_id, sel->stage_id, sel->charas, sel->modules);
            shift_next_mode_sub(MODE_SUB_GAME_MAIN);
        }
        else
            shift_next_mode(MODE_ADVERTISE);
        return true;
    }
    return false;

    /*if (sel_main_get()->check_alive()) {
        if (sel_main_get()->field_7A && !task_pv_game_check_alive())
            task_pv_game_init_pv();
        return false;
    }
    else {
        if (sel_main_get()->field_79)
            shift_next_mode(MODE_ADVERTISE);
        else
            shift_next_mode_sub(MODE_SUB_GAME_MAIN);
        return true;
    }*/
}

bool dest_selector() {
#if PV_DEBUG
    if (!pv_x) {
        PVGameSelector* sel = pv_game_selector_get();
        if (sel->check_alive()) {
            sel->close();
            return false;
        }

        pv_game_selector_free();
        return true;
    }
#endif

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (baker && baker->start) {
            baker->start = false;
            return true;
        }
        else if (baker && !baker->exit)
            return false;
        else if (baker->check_alive()) {
            baker->close();
            return false;
        }

        x_pv_game_baker_free();
        return true;
    }
#endif

    XPVGameSelector* sel = x_pv_game_selector_get();
    if (sel->check_alive()) {
        sel->close();
        return false;
    }

    x_pv_game_selector_free();
    return true;

    /*if (sel_main_get()->check_alive()) {
        sel_main_get()->close();
        return false;
    }
    return true;*/
}

bool init_game_main() {
    sound_work_release_stream(0);
    set_etc_work(0);
    //task_information_ptr->sub_1403BB780(4);
    rctx_ptr->render_manager->set_multisample(false);
    return true;
}

bool ctrl_game_main() {
    switch (get_mode_info_const()->etc_work) {
    case 0:
        //if (task_wait_screen_get_started())
        //    break;

        set_etc_work(1);
        break;
    case 1:
        if (task_wait_screen_check_index_none()) {
            bool watch = sub_14038BB30()->field_0.watch;
            //sound_volume_get()->show_enable(true, watch);

            /*if (watch)
                task_photo_service_add();*/
                /*else {
                    if (!act_toggle_get()->check_alive())
                        act_toggle_get()->sub_14013CE40();
                    *(int32_t*)((size_t)act_toggle_get() + 0xA0) = 5;
                }*/

                //sound_volume_get()->field_6C = spr::SPR_PRIO_04;
                //sound_volume_get()->field_70 = spr::SPR_PRIO_05;
        }

#if PV_DEBUG
        if (!pv_x) {
            if (!task_pv_game_check_alive()) {
                if (!sub_14038BB30()->field_0.watch) {
                    //shift_next_mode_sub(MODE_SUB_STAGE_RESULT);
                    return true;
                }

                /*if (player_data_array_get(0)->field_0)
                    net_mgr::net_mgr_stage_result_open();*/

                set_etc_work(2);
            }
            break;
        }
#endif

        if (!x_pv_game_get()->check_alive())
            return true;
        break;
    case 2: {
        /*if (net_mgr::net_mgr_stage_result_check_alive())
            break;*/

            /*std::vector<texture*> textures;
            //sub_140490230(textures);
            if (textures.size() > 0)
                shift_next_mode_sub(MODE_SUB_SCREEN_SHOT_SEL);
            else {
                //task_photo_service_del();
                if (!sub_1403F6B00())
                    shift_next_mode_sub(MODE_SUB_GAME_OVER);
                else
                    shift_next_mode_sub(MODE_SUB_SCREEN_SHOT_RESULT);
            }*/
    } return true;
    }
    return false;
}

bool dest_game_main() {
    bool res;
#if PV_DEBUG
    if (!pv_x) {
        task_pv_game_close();
        res = pv_game_free();
    }
    else
        res = x_pv_game_free();
#else
    res = x_pv_game_free();
#endif
    if (!res)
        return false;

#if BAKE_X_PACK
    if (pv_x_bake) {
        XPVGameBaker* baker = x_pv_game_baker_get();
        if (baker)
            shift_next_mode_sub(MODE_SUB_SELECTOR);
    }
#endif

    task_mask_screen_fade_in(0.0f, 0);

    //sound_volume_get()->field_6C = spr::SPR_PRIO_09;
    //sound_volume_get()->field_70 = spr::SPR_PRIO_10;
    //*(int32_t*)((size_t)act_toggle_get() + 0xA0) = 10;
    //sound_volume_get()->show_enable(false, false);
    //act_toggle_get()->close();

    /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
    task_lamp_ctrl->field_74 = 1;
    task_lamp_ctrl->field_78 = color_white;*/

    //task_slider_control_get()->sub_140618980(1);

    rctx_ptr->render_manager->set_multisample(true);
    return true;
}

bool init_game_sel() {
    //task_information_ptr->sub_1403BB780(3);
    //sel_main_get()->open(true);
    return true;
}

bool ctrl_game_sel() {
    return true;
    /*if (sel_main_get()->check_alive()) {
        if (sel_main_get()->field_7A && !task_pv_game_check_alive())
            task_pv_game_init_pv();
        return false;
    }
    else {
        shift_next_mode_sub(MODE_SUB_GAME_MAIN);
        return true;
    }*/
}

bool dest_game_sel() {
    /*if (sel_main_get()->check_alive())
        sel_main_get()->close();*/
    return true;
}

bool init_stage_result() {
    //task_information_ptr->sub_1403BB780(3);
    //stage_result_main_get();
    //sub_140656320();
    return true;
}

bool ctrl_stage_result() {
    return true;
    /*if (stage_result_main_get()->check_alive())
        return false;

    if (sub_14038BB30()->field_0.no_fail) {
        shift_next_mode_sub(MODE_SUB_GAME_OVER);
        return true;
    }

    int32_tv2 = sub_14038BB30()->field_0.stage_index;
    sub_14038BB30();
    int32_t v3 = 2;//sub_14038AEE0();
    //if (player_data_array_get(0)->contest.enable)
    //    sub_14038BB30()->sub_14038D080(false);

    if (v2 >= v3 - 1)
        shift_next_mode_sub(MODE_SUB_GAME_OVER);
    else {
        struc_716* v7 = sub_14038BB30()->get_stage();
        if (v7->field_0 == -1 && !v7->field_128.next_stage) {
            shift_next_mode_sub(MODE_SUB_GAME_OVER);
        }
        else {
            sub_14038BB30()->cycle_state_index();
            shift_next_mode_sub(MODE_SUB_GAME_SEL);
        }
    }
    return true;*/
}

bool dest_stage_result() {
    //stage_result_main_get()->close();
    return true;
}

bool init_screen_shot_sel() {
    //task_information_ptr->sub_1403BB780(3);
    //sel_screen_shot_get();
    //sub_1405D7C60();
    return true;
}

bool ctrl_screen_shot_sel() {
    return true;
    /*if (sel_screen_shot_get()->check_alive())
        return false;

    shift_next_mode_sub(sub_1403F6B00()
        ? MODE_SUB_SCREEN_SHOT_RESULT : MODE_SUB_GAME_OVER);
    return true;*/
}

bool dest_screen_shot_sel() {
    //sel_screen_shot_get()->close();
    return true;
}

bool init_screen_shot_result() {
    //task_information_ptr->sub_1403BB780(3);
    //screen_shot_result_main_get();
    //sub_140559010();
    return true;
}

bool ctrl_screen_shot_result() {
    return true;
    /*if (screen_shot_result_main_get()->check_alive())
        return false;

    shift_next_mode_sub(MODE_SUB_GAME_OVER);
    return true;*/
}

bool dest_screen_shot_result() {
    //screen_shot_result_main_get()->close();
    return true;
}

bool init_game_over() {
    //task_information_ptr->sub_1403BB780(5);

    /*bool v0 = true;
    int32_t v1 = sub_14038BB30()->field_0.stage_index;
    sub_14038BB30();

    int32_t v2 = sub_14038AEE0() - 1;
    if (!sub_1402103A0())
        v0 = false;
    if (sub_14038BB30()->field_0.no_fail)
        v0 = false;
    if (!sub_1403933C0())
        v0 = false;

    if (v1 == v2) {
        if (sub_14038BB30()->get_stage()->field_0 != -1)
            v0 = false;
    }
    else if (v1 > v2)
        v0 = false;
    game_over_main_open(game_over_main_get(), v0);*/
    return true;
}

bool ctrl_game_over() {
    return true;
    /*if (game_over_main_get()->check_alive()) {
        struc_705* v2 = sub_14022EF20(0);
        if (v2 && sub_14022ED30(v2))
            *(uint8_t*)((size_t)game_over_main_get() + 0x8D) = 1;
    }
    else {
        if (*(uint8_t*)((size_t)game_over_main_get() + 0x8D))
            shift_next_mode_sub(MODE_SUB_GAME_SEL);
        else
            shift_next_mode(MODE_ADVERTISE);
    }
    return false;*/
}

bool dest_game_over() {
    //game_over_main_get()->close();
    return true;
}
