/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mode_advertise.hpp"
#include "../CRE/customize_item_table.hpp"
#include "../CRE/data.hpp"
#include "../CRE/module_table.hpp"
#include "../CRE/render_context.hpp"
#include "../CRE/sound.hpp"
#include "pv_game/pv_game.hpp"
#include "am_data.hpp"
#include "input_state.hpp"
#include "mask_screen.hpp"
#include "mode.hpp"
#include "task_movie.hpp"
#include "wait_screen.hpp"

struct demo_mode {
    bool load;
    int32_t pv_state;
    int32_t index;
    int32_t difficulty_index;
    int32_t state;
};

extern render_context* rctx_ptr;

static demo_mode s_demo_mode;

static std::vector<std::pair<int32_t, pv_difficulty[2]>> stru_1411A12E0;
static std::vector<uint32_t> stru_1411A12F8;

static bool adv_load();
static void adv_read();
static void adv_unload();

static bool check_photo_mode_demo_movie_exists();

static void demo_mode_difficulty_flip();

bool init_advertise() {
    switch (get_mode_info_const()->etc_work) {
    case 0:
        //net_alw_detail::context_ptr_get()->field_168(0);
        //net_alw_detail::context_ptr_get()->field_C8(1);
        //sub_14066C680();
        //task_slider_control_get()->sub_140618840();
        //task_slider_control_get()->sub_140618860();

        adv_read();
        set_etc_work(1);

        //sub_1404A7370(player_data_array);
        //sub_14038AFC0(sub_14038BB30());

        if (get_mode_info_const()->current != get_mode_info_const()->old) {
            //sub_1403F4670();
            demo_mode_difficulty_flip();
        }

        /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
        task_lamp_ctrl->field_74 = 1;
        task_lamp_ctrl->field_78 = color_white;*/

        //task_slider_control_get()->sub_140618980(2);

        /*if (task_printer_get()->check_alive() && !wrap_collection_get()->printer.get_value())
            task_printer_get()->close();*/
        break;
    case 1:
        if (!adv_load())
            set_etc_work(2);
        break;
    case 2:
        //task_information_ptr->open("INFORMATION");

        task_wait_screen_open();
        //sub_1401F4440();
        //sub_1401E87E0();

        const float_t min_volume = get_min_speakers_volume();
        const float_t max_volume = get_max_speakers_volume();
        sound_work_set_speakers_volume((float_t)wrap_collection_get()->advertise_sound_volume.get_value()
            * (max_volume - min_volume) * 0.01f + min_volume);

        //sub_1402B7880()->sub_1402BAE80();
        module_data_handler_data_add_all_modules();
        customize_item_data_handler_data_add_all_customize_items();
        sound_work_reset_all_se();
        //adv_touch_get()->open(sub_1402103A0());
        //adv_festa_get()->open();
        //adv_noblesse_get()->open();
        //task_information_ptr->field_77 = false;
        //sub_1403F35A0();
        return true;
    }
    return false;
}

bool ctrl_advertise() {
    /*task_information_ptr->sub_1403BB780(2);

    if (sub_14066CC00()) {
        sub_14066FA90();
        game_state_set_error_code(91);
    }*/

    /*if (sub_1403BADE0())
        sub_1403BB7B0();*/

        //*(uint8_t*)((size_t)task_aime_get() + 0x15C) = is_mode_advertise_not_sub_demo();

        /*if (!sup_err_task_supplies_error_check_alive() && task_information_ptr->field_77
            && wrap_collection_get()->printer.get_value() && sub_140662AD0() != 3)
            task_information_ptr->field_77 = false;*/

    const InputState* input_state = input_state_get(0);
    bool v2 = false;//sub_1403F49C0();
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_START) && v2) {
        shift_next_mode(MODE_GAME);
        sound_work_play_se(0, "se_sy_01", 1.0);
    }

    /*adv_touch_get()->sub_14014ED60(true);
    if (!v2)
        adv_touch_get()->sub_14014ED60(false);

    adv_touch_get()->sub_14014EC20(v2);
    if (get_mode_info()->sub_current == MODE_SUB_TITLE)
        adv_touch_get()->sub_14014EC20(false);

    if (get_mode_info()->sub_current == MODE_SUB_CM && !*(uint8_t*)((size_t)adv_cm_get() + 0x108))
        adv_touch_get()->sub_14014EC20(false);

    if (get_mode_info()->sub_current == MODE_SUB_PHOTO_MODE_DEMO && !check_photo_mode_demo_movie_exists())
        adv_touch_get()->sub_14014EC20(false);

    if (input_state->CheckDown(INPUT_BUTTON_JVS_L) || input_state->CheckDown(INPUT_BUTTON_JVS_R))
        adv_touch_get()->sub_14014ED60(false);

    if (adv_touch_get()->field_70) {
        shift_next_mode(MODE_GAME);
        sound_work_play_se(0, "se_sy_01");
        adv_touch_get()->sub_14014EC00();
    }*/

    /*if (sub_14022EF20(0)->sub_14022ED30() && v2)
        shift_next_mode(MODE_GAME);*/
    return false;
}

bool dest_advertise() {
    //task_slider_control_get()->sub_140618840(v0);
    //adv_touch_get()->close();
    //adv_festa_get()->close();
    //adv_noblesse_get()->close();
    adv_unload();
    sound_work_set_speakers_volume(get_max_speakers_volume());
    //*(uint8_t*)((size_t)task_aime_get() + 0x15C) = 0;
    return true;
}

bool init_logo() {
    //task_information_ptr->sub_1403BB780(1);
    //task_adv_logo.open(0);
    //sub_1403F35A0();
    return true;
}

bool ctrl_logo() {
    return true;
    //*task_information_ptr->sub_1403BB780(1);
    //return !task_adv_logo.check_alive();
}

bool dest_logo() {
    //task_information_ptr->sub_1403BB780(1);
    //sub_14066E8D0();
    //task_adv_logo.close();
    return true;
}

bool init_rating() {
    //sub_1403F35A0();
    return true;
}

bool ctrl_rating() {
    return true;
    //return !task_adv_rating.check_alive();
}

bool dest_rating() {
    //task_adv_rating.close();
    return true;
}

bool init_demo() {
    rctx_ptr->render_manager->set_multisample(false);

    if (check_photo_mode_demo_movie_exists())
        s_demo_mode.state = 1;

    switch (s_demo_mode.state) {
    case 0: {
        ScreenParam& screen_param = get_screen_param();
        ScreenParam& render_screen_param = get_render_screen_param();
        get_screen_param();

        rectangle rect;
        rect.pos = { 0.0f, (float_t)(screen_param.height - render_screen_param.height) * 0.5f };
        rect.size = { (float_t)render_screen_param.width, (float_t)render_screen_param.height };

        TaskMovie::SprParams spr_params;
        spr_params.disp.rect = rect;
        spr_params.disp.screen_mode = screen_param.mode;
        spr_params.disp.scale = -1.0f;
        spr_params.disp.field_18 = 0;
        spr_params.disp.target = spr::SPR_TARGET_DEFAULT;
        spr_params.prio = spr::SPR_PRIO_07;
        task_movie_get(0)->Reset(spr_params);
        //task_movie_get(0)->Load("rom/movie/diva_adv.wmv");
    } break;
    case 1:
        task_wait_screen_set_load_loop_demo_load();
        s_demo_mode.load = false;
        task_rob_manager_open();
        s_demo_mode.pv_state = 0;
        break;
    }

    //sub_1403F35A0();
    return true;
}

bool ctrl_demo() {
    bool ret = false;
    bool v1 = false;
    /*if (sup_err_task_supplies_error_check_alive()) {
        if (!task_pv_game_check_alive())
            return true;

        v1 = true;
    }*/

    switch (s_demo_mode.state) {
    case 0:
        ret = !task_movie_get(0)->CheckState();
        break;
    case 1: {
        if (s_demo_mode.load) {
            if (!task_pv_game_check_alive())
                ret = true;
        }
        else if (!task_wait_screen_get_started()) {
            int32_t index = s_demo_mode.index;
            if (stru_1411A12F8.size()) {
                size_t size = stru_1411A12E0.size();
                if (size) {
                    uint32_t pv_index = stru_1411A12F8.data()[s_demo_mode.index];
                    if (pv_index < size) {
                        const std::pair<int32_t, pv_difficulty[2]>* elem = &stru_1411A12E0.data()[pv_index];
                        if (elem) {
                            pv_difficulty diff = elem->second[s_demo_mode.difficulty_index];
                            if (diff != PV_DIFFICULTY_MAX) {
                                if (!task_pv_game_init_demo_pv(elem->first, diff, true))
                                    task_wait_screen_set_load_loop_none();
                                goto Next;
                            }
                        }
                    }
                }
            }

            v1 = true;

        Next:
            s_demo_mode.index = ++index;

            if (index >= stru_1411A12E0.size()) {
                s_demo_mode.index = 0;
                demo_mode_difficulty_flip();
            }
            s_demo_mode.load = true;
        }
    } break;
    default:
        ret = 1;
        break;
    }

    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CIRCLE))
        v1 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CROSS))
        v1 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_TRIANGLE))
        v1 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_SQUARE))
        v1 = true;

    if (v1 && task_wait_screen_check_index_none())
        ret = true;
    return ret;
}

bool dest_demo() {
    //net_alw_detail::context_ptr_get()->field_C8(1);

    switch (s_demo_mode.state) {
    case 0:
        task_movie_get(0)->Unload();
        task_movie_get(0)->close();
        s_demo_mode.state = 1;
    case 1:
        switch (s_demo_mode.pv_state) {
        case 0:
        default:
            if (pv_game_get())
                pv_game_get()->unload();
            s_demo_mode.pv_state = 1;
            return false;
        case 1:
            if (task_pv_game_close() && task_rob_manager_close())
                s_demo_mode.pv_state = 2;
            return false;
        case 2:
            task_wait_screen_set_load_loop_none();
            task_mask_screen_fade_in(0.0f, 0);
            s_demo_mode.state = 0;
            break;
        }
        break;
    }

    rctx_ptr->render_manager->set_multisample(true);
    return true;
}

bool init_title() {
    //sub_140141F80(&task_adv_title, 0, false);
    //sub_1403F35A0();
    return true;
}

bool ctrl_title() {
    return true;
    //return !task_adv_title.check_alive();
}

bool dest_title() {
    //sub_14066E8D0();
    //task_adv_title.close();
    return true;
}

bool init_ranking() {
    //adv_rank_main_get();
    //sub_140143280();
    //sub_1403F35A0();
    return true;
}

bool ctrl_ranking() {
    return true;
    //return !adv_rank_main_get()->check_alive();
}

bool dest_ranking() {
    //sub_14066E8D0();
    //adv_rank_main_get()->close();
    return true;
}

bool init_score_ranking() {
    //adv_score_rank_main_get();
    //sub_140149010();
    //sub_1403F35A0();
    return true;
}

bool ctrl_score_ranking() {
    return true;
    //return !adv_score_rank_main_get()->check_alive();
}

bool dest_score_ranking() {
    //sub_14066E8D0();
    //adv_score_rank_main_get()->close();
    return true;
}

bool init_cm() {
    //adv_cm_get();
    //sub_14013F750();
    //sub_1403F35A0();
    return true;
}

bool ctrl_cm() {
    return true;
    /*bool v0 = false;
    bool v1 = false;//adv_cm_get()->field_68 == 3;
    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CIRCLE))
        v0 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CROSS))
        v0 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_TRIANGLE))
        v0 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_SQUARE))
        v0 = true;
    return v0 ? v0 : v1;*/
}

bool dest_cm() {
    //sub_14066E8D0();
    //adv_cm_get()->close();
    return true;
}

bool init_photo_mode_demo() {
    if (check_photo_mode_demo_movie_exists()) {
        ScreenParam& screen_param = get_screen_param();
        ScreenParam& render_screen_param = get_render_screen_param();

        rectangle rect;
        rect.pos = { 0.0f, (float_t)(screen_param.height - render_screen_param.height) * 0.5f };
        rect.size = { (float_t)render_screen_param.width, (float_t)render_screen_param.height };

        TaskMovie::SprParams spr_params;
        spr_params.disp.rect = rect;
        spr_params.disp.screen_mode = screen_param.mode;
        spr_params.disp.scale = -1.0f;
        spr_params.disp.field_18 = 0;
        spr_params.disp.target = spr::SPR_TARGET_DEFAULT;
        spr_params.prio = spr::SPR_PRIO_07;
        task_movie_get(0)->Reset(spr_params);
        //task_movie_get(0)->Load("rom/movie/diva_adv02.wmv");

        //adv_festa_get()->field_6C = true;
        //adv_noblesse_get()->sub_140142530(v6, true);
    }
    else {
        //adv_festa_get()->field_6C = false;
        //adv_noblesse_get()->sub_140142530(v6, false);
    }

    //sub_1403F35A0();
    return true;
}

bool ctrl_photo_mode_demo() {
    return true;
    /*if (!check_photo_mode_demo_movie_exists())
        return true;

    bool v2 = !task_movie_get(0)->CheckState();

    bool v4 = false;
    const InputState* input_state = input_state_get(0);
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CIRCLE))
        v4 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_CROSS))
        v4 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_TRIANGLE))
        v4 = true;
    if (input_state->CheckTapped(INPUT_BUTTON_JVS_SQUARE))
        v4 = true;
    if (v4 && task_wait_screen_check_index_none())
        v2 = true;
    return v2;*/
}

bool dest_photo_mode_demo() {
    if (check_photo_mode_demo_movie_exists()) {
        task_movie_get(0)->Unload();
        task_movie_get(0)->close();
    }
    return true;
}

static bool adv_load() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    if (sprite_manager_load_file(37, aft_spr_db) || aet_manager_load_file(30, aft_aet_db))
        return true;
    return sound_work_load_farc("rom/sound/se_adv.farc");
}

static void adv_read() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    sprite_manager_read_file(37, "", aft_data, aft_spr_db);
    aet_manager_read_file(30, "", aft_data, aft_aet_db);
    sound_work_read_farc("rom/sound/se_adv.farc");
}

static void adv_unload() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    sprite_manager_unload_set(37, aft_spr_db);
    aet_manager_unload_set(30, aft_aet_db);
    sound_work_unload_farc("rom/sound/se_adv.farc");
}

static bool check_photo_mode_demo_movie_exists() {
    if (wrap_collection_get()->printer.get_value())
        return data_list[DATA_AFT].check_file_exists("rom/movie/diva_adv02.wmv");
    return false;
}

static void demo_mode_difficulty_flip() {
    int32_t difficulty_index = s_demo_mode.difficulty_index + 1;
    if (difficulty_index < 0 || difficulty_index >= 1)
        difficulty_index = 0;
    s_demo_mode.difficulty_index = difficulty_index;
}
