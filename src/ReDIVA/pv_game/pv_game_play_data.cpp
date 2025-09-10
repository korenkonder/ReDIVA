/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game_play_data.hpp"
#include "../../CRE/data.hpp"
#include "../print_work.hpp"
#include "game_2d.hpp"
#include "player_data.hpp"
#include "pv_game.hpp"
#include "target_pos.hpp"

const float_t percentage_clear_excellent[] = {
    80.0f, 85.0f, 90.0f, 95.0f, 97.0f
};

const float_t percentage_clear_great[] = {
    65.0f, 75.0f, 80.0f, 85.0f, 90.0f
};

const char* spr_black_fade[] = {
    "black_fade",
};

const char* spr_chance_frame_bottom[] = {
    "chance_frame_bottom",
};

const char* spr_chance_frame_top[] = {
    "chance_frame_top",
};

const char* spr_chance_point[] = {
    "chance_point_even",
    "chance_point_odd",
};

const char* spr_chance_txt[] = {
    "chance_start_txt",
    "chance_end_txt",
};

const char* spr_combo_cool_fine[] = {
    "combo_cool001",
    "combo_cool010",
    "combo_cool100",
    "combo_cool100_b",
    "combo_cool1000",
    "combo_fine001",
    "combo_fine010",
    "combo_fine100",
    "combo_fine100_b",
    "combo_fine1000",
};

const char* spr_combo_just_cool_fine[] = {
    "combo_just_cool001",
    "combo_just_cool010",
    "combo_just_cool100",
    "combo_just_cool100_b",
    "combo_just_cool1000",
    "combo_fine001",
    "combo_fine010",
    "combo_fine100",
    "combo_fine100_b",
    "combo_fine1000",
};

const char* spr_combo_tempo_cool_fine[] = {
    "combo_tempo_cool001",
    "combo_tempo_cool010",
    "combo_tempo_cool100",
    "combo_tempo_cool100_b",
    "combo_tempo_cool1000",
    "combo_tempo_fine001",
    "combo_tempo_fine010",
    "combo_tempo_fine100",
    "combo_tempo_fine100_b",
    "combo_tempo_fine1000",
};

const char* spr_demo_font[] = {
    "demo_font",
};

const char* spr_demo_logo[] = {
    "demo_logo",
};

const char* spr_failed_info[] = {
    "failed_info",
};

const char* spr_frame[] = {
    "frame_bottom",
    "frame_up",
    "frame_bottom_danger",
    "frame_up_danger",
};

const char* spr_level_info[] = {
    "level_info_easy",
    "level_info_normal",
    "level_info_hard",
    "level_info_extreme",
    "level_info_encore",
};

const char* spr_level_info_ctst[] = {
    "level_info_easy_ctst",
    "level_info_normal_ctst",
    "level_info_hard_ctst",
    "level_info_extreme_ctst",
    "level_info_extreme_ctst",
};

const char* spr_level_info_ctst_extra[] = {
    "level_info_easy_ctst",
    "level_info_normal_ctst",
    "level_info_hard_ctst",
    "level_info_extreme_extra_ctst",
    "level_info_extreme_ctst",
};

const char* spr_level_info_extra[] = {
    "level_info_easy",
    "level_info_normal",
    "level_info_hard",
    "level_info_extreme_extra",
    "level_info_encore",
};

const char* spr_level_info_insu[] = {
    "level_info_easy_insu",
    "level_info_normal_insu",
    "level_info_hard_insu",
    "level_info_extreme_insu",
    "level_info_extreme_insu",
};

const char* spr_level_info_insu_extra[] = {
    "level_info_easy_insu",
    "level_info_normal_insu",
    "level_info_hard_insu",
    "level_info_extreme_extra_insu",
    "level_info_extreme_insu",
};

const char* spr_max_slide_num[] = {
    "max_slide_num_00",
    "max_slide_num_01",
    "max_slide_num_02",
    "max_slide_num_03",
    "max_slide_num_04",
    "max_slide_num_05",
    "max_slide_num_06",
    "max_slide_num_07",
    "max_slide_num_08",
    "max_slide_num_09",
};

const char* spr_max_slide_num_plus[] = {
    "max_slide_num_plus",
};

const char* spr_max_slide_point[] = {
    "max_slide_point_even",
    "max_slide_point_odd",
};

const char* spr_next_info[] = {
    "next_info",
};

const char* spr_option_info_base[] = {
    "option_info_base",
};

const char* spr_option_info_hidden[] = {
    "option_info_hispeed_a",
    "option_info_hispeed_b",
};

const char* spr_option_info_hispeed[] = {
    "option_info_hidden_a",
    "option_info_hidden_b",
};

const char* spr_option_info_sudden[] = {
    "option_info_sudden_a",
    "option_info_sudden_b",
};

const char* spr_p_chance_point_num[] = {
    "p_chance_point_num01_c",
    "p_chance_point_num02_c",
    "p_chance_point_num03_c",
    "p_chance_point_num04_c",
    "p_chance_point_num05_c",
    "p_chance_point_num06_c",
    "p_chance_point_num07_c",
};

const int32_t spr_p_chance_point_num_spr[] = {
    391, 399, 400, 401, 402, 403, 404, 405, 406, 407, 392,
};

const char* spr_p_chance_rslt_num[] = {
    "p_chance_rslt_num01_c",
    "p_chance_rslt_num02_c",
    "p_chance_rslt_num03_c",
    "p_chance_rslt_num04_c",
    "p_chance_rslt_num05_c",
    "p_chance_rslt_num06_c",
};

const int32_t spr_p_chance_rslt_num_spr[] = {
    408, 409, 410, 411, 412, 413, 414, 415, 416, 417,
};

const char* spr_p_energy_num[] = {
    "p_energy_num01_c",
    "p_energy_num02_c",
    "p_energy_num03_c",
    "p_energy_num04_c",
    "p_energy_num05_c",
    "p_energy_num06_c",
    "p_energy_num07_c",
};

const int32_t spr_p_energy_num_spr[] = {
    1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 3092,
};

const int32_t spr_p_energy_reach_num_spr[] = {
    1546, 1547, 1548, 1549, 1550, 1551, 1552, 1553, 1554, 1555, 1556, 3093,
};

const char* spr_p_combo_num[] = {
    "p_combo_num001_c",
    "p_combo_num010_c",
    "p_combo_num100_c",
    "p_combo_num1000_c",
};

const int32_t spr_p_combo_num_spr[] = {
    3210, 3211, 3212, 3213, 3214, 3215, 3216, 3217, 3218, 3219,
};

const char* spr_p_score[] = {
    "p_score01_c",
    "p_score02_c",
    "p_score03_c",
    "p_score04_c",
    "p_score05_c",
    "p_score06_c",
    "p_score07_c",
};

const int32_t spr_p_score_spr[] = {
    477, 478, 479, 480, 481, 482, 483, 484, 485, 486,
};

const int32_t spr_p_slide_point_num_spr[] = {
    34076, 34077, 34078, 34079, 34080, 34081, 34082, 34083, 34084, 34085, 34086,
};

const char* spr_p_song_lt[] = {
    "p_song_lyric_lt",
    "p_song_title_lt",
};

const char* spr_song_energy_base[] = {
    "song_energy_base",
};

const char* spr_song_energy_base_reach[] = {
    "song_energy_base_reach",
};

const char* spr_song_energy_border[] = {
    "song_energy_border",
};

const char* spr_song_energy_border_excellent[] = {
    "song_energy_border_excellent",
};

const char* spr_song_energy_border_great[] = {
    "song_energy_border_great",
};

const char* spr_song_energy_border_rival[] = {
    "song_energy_border_rival",
};

const char* spr_song_energy_clear_txt[] = {
    "song_energy_clear_txt",
};

const char* spr_song_energy_edge_line02[] = {
    "song_energy_edge_line02",
};

const char* spr_song_energy_eff_normal[] = {
    "song_energy_eff_normal",
};

const char* spr_song_energy_eff_reach[] = {
    "song_energy_eff_reach",
};

const char* spr_song_energy_full[] = {
    "song_energy_full",
};

const char* spr_song_energy_normal[] = {
    "song_energy_normal",
};

const char* spr_song_energy_not_clear[] = {
    "song_energy_not_clear",
};

const char* spr_song_energy_not_clear_txt[] = {
    "song_energy_not_clear_txt",
};

const char* spr_song_energy_reach[] = {
    "song_energy_reach",
};

const char* spr_song_icon_loop[] = {
    "song_icon_loop",
};

const char* spr_stage_info[] = {
    "stage_info01",
    "stage_info02",
    "stage_info03",
    "stage_info04",
};

const char* spr_success_info[] = {
    "success_info",
};

const char* spr_value_text[] = {
    "value_text_cool04",
    "value_text_wrong01",
    "value_text_fine01",
    "value_text_wrong02",
    "value_text_safe",
    "value_text_wrong03",
    "value_text_sad",
    "value_text_wrong04",
    "value_text_worst",
};

const char* spr_value_text_early[] = {
    "value_text_cool01",
    "value_text_wrong01",
    "value_text_fine01",
    "value_text_wrong02",
    "value_text_safe",
    "value_text_wrong03",
    "value_text_sad",
    "value_text_wrong04",
    "value_text_worst",
};

const char* spr_value_text_late[] = {
    "value_text_cool03",
    "value_text_wrong01",
    "value_text_fine03",
    "value_text_wrong02",
    "value_text_safe",
    "value_text_wrong03",
    "value_text_sad",
    "value_text_wrong04",
    "value_text_worst",
};

const char* spr_white_fade[] = {
    "white_fade",
};

extern uint32_t aet_gam_cmn_set_id;
extern uint32_t spr_gam_cmn_set_id;

static void sub_14013AAE0(float_t pos_x, float_t pos_y, spr::SprPrio prio,
    const char* str, bool h_center, color4u8 color, rectangle* clip_rect = 0);

pv_disp2d::pv_disp2d() : pv_id(), title_start_2d_field(), title_end_2d_field(),
title_start_2d_low_field(), title_end_2d_low_field(), title_start_3d_field(), title_end_3d_field(),
target_shadow_type(), pv_spr_set_id(), pv_aet_set_id(), pv_aet_id() {

}

pv_disp2d::~pv_disp2d() {

}

pv_game_play_data::pv_game_play_data() : pv_set(), aet_ids(), aet_ids_enable(), frame_btm_pos(),
frame_btm_pos_diff(), frame_top_pos(), frame_top_pos_diff(), song_info_pos(), song_info_pos_diff(),
song_energy_pos(), song_energy_pos_diff(), frame_disp(), frame_state(), frame_top_pos_max(),
frame_btm_pos_max(), frame_danger_state(), field_218(), fade_begin(), fade_end(), fade(),
skin_danger_frame(), field_2FC(), chance_result(), chance_points(), field_310(), slide_points(),
max_slide_points(), pv_spr_set_id(), pv_aet_set_id(), pv_aet_id(), stage_index(), field_33C(),
field_350(), field_354(), value_text_spr_index(), not_clear(), spr_set_back_id(), loaded(),
state(), field_36C(), update(), field_374(), field_378(), field_37C(), score_speed(), score(),
value_text_display(), field_38C(), value_text_index(), value_text_time_offset(), combo_state(),
combo_count(), combo_disp_time(), field_3AC(), field_3B0(), field_3B4(), field_3B8(),
field_3BC(), aix(), ogg(), field_3C4(), ex_song_index(), field_5EC(), option(), field_5F4(),
lyric(), lyric_set(), field_64C(), field_650(), field_654(), field_658(), field_65C() {
    chance_points_pos = 0;
    slide_points_pos = 0;
    max_slide_points_pos = 0;
    reset();
    state = 0;
}

pv_game_play_data::~pv_game_play_data() {

}

void pv_game_play_data::combo_ctrl(float_t delta_time) {
    switch (combo_state) {
    case 0:
        combo_disp_time = 0.0f;
        combo_state = 1;
        break;
    case 1:
        if (combo_disp_time >= 0.05f) {
            combo_disp_time = 0.0f;
            combo_state = 2;
        }
        else
            combo_disp_time += delta_time;
        return;
    case 2:
        if (combo_disp_time >= 0.04f) {
            combo_disp_time = 0.0f;
            combo_state = 3;
        }
        else
            combo_disp_time += delta_time;
        return;
    case 3:
        if (combo_disp_time >= 0.2f) {
            combo_disp_time = 0.0f;
            combo_state = 4;
        }
        else
            combo_disp_time += delta_time;
        return;
    case 4:
    default:
        if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_COMBO]))
            aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_COMBO]);
        break;
    }

}

void pv_game_play_data::ctrl(float_t delta_time) {
    if (!update)
        return;

    combo_ctrl(delta_time);
    skin_danger_ctrl();

    if (field_378) {
        switch (field_374) {
        case 0:
            init_aet_chance_txt(true);
            field_374++;
            field_37C = 0.0f;
            break;
        case 1:
            if (field_37C >= 2.0f)
                field_374 = 2;
            break;
        case 2:
            if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_CHANCE_TXT])) {
                aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_CHANCE_TXT]);
                field_374++;
            }
            break;
        case 4:
            init_aet_chance_txt(false);
            field_374++;
            field_37C = 0.0f;
            break;
        case 5:
            if (field_37C >= 2.0f)
                field_374 = 6;
            break;
        case 6:
            if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_CHANCE_TXT])) {
                aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_CHANCE_TXT]);
                field_374 = 0;
                field_378 = false;
            }
            break;
        }

        field_37C += delta_time;
    }

    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_CHANCE_POINT]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_CHANCE_POINT]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_SLIDE_POINT]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_SLIDE_POINT]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_POINT]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_POINT]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_PLUS]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_PLUS]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_0]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_0]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_1]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_1]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_2]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_2]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_3]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_3]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_4]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_4]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_5]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_5]);
    if (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_6]))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_6]);

    if (aet_ids[PV_GAME_AET_VALUE_TEXT] && (aet_manager_get_obj_end(aet_ids[PV_GAME_AET_VALUE_TEXT])
        || aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_VALUE_TEXT]) > 60.0f))
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_VALUE_TEXT]);
}

void pv_game_play_data::disable_update() {
    if (loaded)
        update = false;
}

void pv_game_play_data::disp() {
    if (loaded && !field_354) {
        sub_1401349B0();
        field_354 = true;
    }

    disp_combo();
    disp_chance_point();
    disp_chance_txt();
    disp_slide_point();
    disp_max_slide_point();
    disp_score();
    disp_song_energy();
    disp_spr_set_back();
}

void pv_game_play_data::disp_chance_point() {
    if (!aet_ids[PV_GAME_AET_CHANCE_POINT])
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    int32_t digits = get_digit_count(chance_points, 6);

    int32_t chance_point;
    int32_t v5;
    if ((digits + 1) % 2) {
        chance_point = 1;
        v5 = 6 - digits;
    }
    else {
        chance_point = 0;
        v5 = 5 - digits;
    }

    aet_manager_init_aet_layout(&comp, 3, spr_chance_point[chance_point], (AetFlags)0, RESOLUTION_MODE_HD,
        0, aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_CHANCE_POINT]), aft_aet_db, aft_spr_db);

    vec2 pos;
    target_pos_scale_offset_apply(&chance_points_pos, &pos);

    const aet_layout_data* layout = comp.Find((&spr_p_chance_point_num[v5 / 2])[digits]);
    if (layout)
        aet_layout_data::put_sprite(392, spr::SPR_ATTR_CTR_CC, spr::SPR_PRIO_16, &pos, layout, aft_spr_db);

    AetComp::put_number_sprite(chance_points, 7, &comp, &spr_p_chance_point_num[v5 / 2],
        spr_p_chance_point_num_spr, spr::SPR_PRIO_16, &pos, 0, aft_spr_db);
}

void pv_game_play_data::disp_chance_points(int32_t chance_points, float_t pos_x, float_t pos_y) {
    pos_y = max_def(pos_y, 58.0f);

    chance_points = chance_points;
    chance_points_pos.x = pos_x;
    chance_points_pos.y = pos_y;
    init_aet_chance_point(chance_points, pos_x, pos_y);
}

void pv_game_play_data::disp_chance_txt() {
    if (!aet_ids[PV_GAME_AET_CHANCE_TXT] || !field_378 || field_374 < 4)
        return;

    int32_t digits = 6;
    while (digits) {
        if (digits > 1) {
            int32_t div = 10;
            int32_t v6 = digits - 2;
            while (v6) {
                div *= 10;
                v6--;
            }

            if (chance_result / div % 10)
                break;
        }
        else if (chance_result % 10)
            break;
        digits--;
    };

    if (!digits)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    vec2 pos = { ((float_t)(6 - digits) * field_350) * -0.5f, 0.0f };
    aet_manager_init_aet_layout(&comp, 3, spr_chance_txt[1], (AetFlags)0, RESOLUTION_MODE_HD,
        0, aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_CHANCE_TXT]), aft_aet_db, aft_spr_db);
    AetComp::put_number_sprite(chance_result, 6, &comp, spr_p_chance_rslt_num,
        spr_p_chance_rslt_num_spr, spr::SPR_PRIO_06, &pos, 0, aft_spr_db);
}

void pv_game_play_data::disp_combo() {
    if (!aet_ids[PV_GAME_AET_COMBO])
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    const char* name;
    if (value_text_time_offset < -(float_t)(1.0 / 120.0))
        name = spr_combo_tempo_cool_fine[value_text_spr_index];
    else if (value_text_time_offset > (float_t)(1.0 / 120.0))
        name = spr_combo_cool_fine[value_text_spr_index];
    else
        name = spr_combo_just_cool_fine[value_text_spr_index];

    AetComp comp;
    aet_manager_init_aet_layout(&comp, 3, name, (AetFlags)0, RESOLUTION_MODE_HD,
        0, aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_COMBO]), aft_aet_db, aft_spr_db);

    vec2 pos;
    target_pos_scale_offset_apply(&combo_pos, &pos);
    AetComp::put_number_sprite(combo_count, 4, &comp, spr_p_combo_num,
        spr_p_combo_num_spr, spr::SPR_PRIO_14, &pos, false, aft_spr_db);
}

void pv_game_play_data::disp_lyric(const char* str, bool h_center, color4u8 color) {
    if (loaded && str)
        sub_14013AAE0(song_txt[0].position.x, song_txt[0].position.y, spr::SPR_PRIO_17, str, h_center, color);
}

void pv_game_play_data::disp_max_slide_point() {
    if (!aet_ids[PV_GAME_AET_MAX_SLIDE_POINT])
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    int32_t digits = get_digit_count(max_slide_points, 6);

    int32_t max_slide_point;
    int32_t v5;
    if ((digits + 1) % 2) {
        max_slide_point = 1;
        v5 = 6 - digits;
    }
    else {
        max_slide_point = 0;
        v5 = 5 - digits;
    }

    AetComp comp;
    aet_manager_init_aet_layout(&comp, 3,
        spr_max_slide_point[max_slide_point], (AetFlags)0, RESOLUTION_MODE_HD, 0,
        aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_MAX_SLIDE_POINT]), aft_aet_db, aft_spr_db);

    vec2 pos;
    target_pos_scale_offset_apply(&this->max_slide_points_pos, &pos);

    const aet_layout_data* layout = comp.Find((&spr_p_chance_point_num[v5 / 2])[digits]);
    if (layout) {
        vec3 position;
        position.x = pos.x + layout->position.x;
        position.y = pos.y + layout->position.y;
        position.z = 0.0f;
        if (aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_PLUS])
            aet_manager_set_obj_position(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_PLUS], position);
    }

    int32_t max_slide_points = this->max_slide_points;
    for (int32_t i = 0; i < 7; i++) {
        max_slide_points /= 10;
        const aet_layout_data* layout = comp.Find((&spr_p_chance_point_num[v5 / 2])[i]);
        if (layout) {
            vec3 position;
            position.x = pos.x + layout->position.x;
            position.y = pos.y + layout->position.y;
            position.z = 0.0f;
            if (aet_ids[PV_GAME_AET_MAX_SLIDE_NUM + i])
                aet_manager_set_obj_position(aet_ids[PV_GAME_AET_MAX_SLIDE_NUM + i], position);
        }

        if (!max_slide_points)
            break;
    }
}

void pv_game_play_data::disp_score() {
    if (!(aet_ids[PV_GAME_AET_FRAME_BOTTOM] || aet_ids[PV_GAME_AET_BOTTOM_BACK]) || !frame_disp[0])
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    vec2 pos = { 0.0f, frame_btm_pos[0] };
    AetComp::put_number_sprite(score, 7, &comp, spr_p_score,
        spr_p_score_spr, spr::SPR_PRIO_11, &pos, true, aft_spr_db);
}

void pv_game_play_data::disp_slide_point() {
    if (!aet_ids[PV_GAME_AET_SLIDE_POINT])
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    int32_t digits = get_digit_count(slide_points, 6);

    int32_t chance_point;
    int32_t v5;
    if ((digits + 1) % 2) {
        chance_point = 1;
        v5 = 6 - digits;
    }
    else {
        chance_point = 0;
        v5 = 5 - digits;
    }

    aet_manager_init_aet_layout(&comp, 3, spr_chance_point[chance_point], (AetFlags)0, RESOLUTION_MODE_HD,
        0, aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_SLIDE_POINT]), aft_aet_db, aft_spr_db);

    vec2 pos;
    target_pos_scale_offset_apply(&slide_points_pos, &pos);
    const aet_layout_data* layout = comp.Find((&spr_p_chance_point_num[v5 / 2])[digits]);
    if (layout)
        aet_layout_data::put_sprite(34086, spr::SPR_ATTR_CTR_CC, spr::SPR_PRIO_16, &pos, layout, aft_spr_db);
    AetComp::put_number_sprite(slide_points, 7, &comp, &spr_p_chance_point_num[v5 / 2],
        spr_p_slide_point_num_spr, spr::SPR_PRIO_16, &pos, 0, aft_spr_db);
}

void pv_game_play_data::disp_slide_points(int32_t max_slide_point, float_t pos_x, float_t pos_y, bool max_slide) {
    pos_y = max_def(pos_y, 48.0f);

    if (max_slide) {
        max_slide_points_pos.x = pos_x;
        max_slide_points_pos.y = pos_y;
        max_slide_points = max_slide_point;
        init_aet_slide_max(max_slide_point, pos_x, pos_y);
    }
    else {
        slide_points_pos.x = pos_x;
        slide_points_pos.y = pos_y;
        slide_points = max_slide_point;
        init_aet_slide_point(max_slide_point, pos_x, pos_y);
    }
}

void pv_game_play_data::disp_song_energy() {
    if (!(aet_ids[PV_GAME_AET_FRAME_BOTTOM] || aet_ids[PV_GAME_AET_BOTTOM_BACK]) || !frame_disp[0])
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    pv_game* pv_game = pv_game_get();
    const int32_t* spr;
    if (pv_game->data.song_energy < pv_game->data.song_energy_base)
        spr = spr_p_energy_num_spr;
    else
        spr = spr_p_energy_reach_num_spr;

    int32_t song_energy = (int32_t)(pv_game->data.song_energy * 100.0f);
    vec2 pos = { 0.0f, song_energy_pos };
    AetComp::put_number_sprite(song_energy / 100, 3, &comp,
        &spr_p_energy_num[4], spr, spr::SPR_PRIO_16, &pos, 0, aft_spr_db);
    AetComp::put_number_sprite(song_energy % 100, 2, &comp,
        &spr_p_energy_num[1], spr, spr::SPR_PRIO_16, &pos, 1, aft_spr_db);

    const aet_layout_data* spr_p_energy_num_3_layout = comp.Find(spr_p_energy_num[3]);
    if (spr_p_energy_num_3_layout)
        aet_layout_data::put_sprite(spr[11], spr::SPR_ATTR_CTR_CC,
            spr::SPR_PRIO_16, &pos, spr_p_energy_num_3_layout, aft_spr_db);

    const aet_layout_data* spr_p_energy_num_0_layout = comp.Find(spr_p_energy_num[0]);
    if (spr_p_energy_num_0_layout)
        aet_layout_data::put_sprite(spr[10], spr::SPR_ATTR_CTR_CC,
            spr::SPR_PRIO_16, &pos, spr_p_energy_num_0_layout, aft_spr_db);
}

void pv_game_play_data::disp_song_name(const char* str) {
    if (!loaded || !str || !frame_disp[0])
        return;

    PrintWork print_work;
    font_info font(16);
    print_work.set_font(&font);

    bool clip = print_work.GetTextSize(str, str + utf8_length(str)).x > 768.0f;

    vec2 pos;
    pos.x = song_txt[1].position.x;
    pos.y = song_txt[1].position.y - song_info_pos;

    rectangle clip_rect;
    clip_rect.pos = pos;
    clip_rect.size = { 768.0f, font.glyph.y };

    sub_14013AAE0(pos.y, pos.y, spr::SPR_PRIO_16, str, 0, 0xFFFFFFFF, clip ? &clip_rect : 0);
}

void pv_game_play_data::disp_spr_set_back() {
    if (!loaded || spr_set_back_id == -1)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    spr::SprArgs args;
    args.id.id = spr_set_back_id;
    args.prio = spr::SPR_PRIO_01;
    args.index = 2;
    spr::put_sprite(args, aft_spr_db);
}

void pv_game_play_data::fade_begin_ctrl() {
    if (!loaded)
        return;

    if (!fade_begin) {
        set_aet_id_play(PV_GAME_AET_WHITE_FADE, true);
        fade_begin = true;
    }
}

bool pv_game_play_data::fade_end_ctrl() {
    if (!loaded)
        return false;

    if (!fade_end && aet_ids[PV_GAME_AET_WHITE_FADE]
        && aet_manager_get_obj_end(aet_ids[PV_GAME_AET_WHITE_FADE])) {
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_WHITE_FADE]);
        fade_end = true;
    }

    return fade_end;
}

void pv_game_play_data::frame_ctrl() {
    for (int32_t i = 0; i < 2; i++) {
        if (frame_state[i] == 3) {
            frame_btm_pos[i] = max_def(frame_btm_pos[i] - frame_btm_pos_diff[i], 0.0f);
            frame_top_pos[i] = max_def(frame_top_pos[i] - frame_top_pos_diff[i], 0.0f);

            if (frame_btm_pos[i] == 0.0f && frame_top_pos[i] == 0.0f)
                frame_state[i] = 1;
        }
        else if (frame_state[i] == 4) {
            frame_btm_pos[i] = min_def(frame_btm_pos[i] + frame_btm_pos_diff[i], frame_btm_pos_max[i]);
            frame_top_pos[i] = min_def(frame_top_pos[i] + frame_top_pos_diff[i], frame_top_pos_max[i]);

            if (frame_btm_pos[i] == frame_btm_pos_max[i] && frame_top_pos[i] == frame_top_pos_max[i])
                frame_state[i] = 2;
        }
    }

    if (song_info_pos_diff > 0.0f)
        song_info_pos -= song_info_pos_diff;
    song_info_pos = max_def(song_info_pos, 0.0f);

    if (song_energy_pos_diff > 0.0f)
        song_energy_pos -= song_energy_pos_diff;
    song_energy_pos = max_def(song_energy_pos, 0.0f);

    ui_set_pos();

    if (frame_state[0] == 1)
        field_218 = true;

    int32_t v13 = 0;
    bool v15 = false;
    if (!sub_14013C8C0()->sub_1400E7920() && frame_disp[0]) {
        if (frame_state[0] == 1 || !field_218)
            v15 = true;
        v13 = (int32_t)-song_info_pos;
    }

    task_game_2d_sub_140372670(v15);
    task_game_2d_sub_1403726D0(v13);
    task_game_2d_set_energy_unit_no_fail(pv_game_get()->data.no_fail);
}

#pragma warning(push)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
void pv_game_play_data::frame_set(bool disp, int32_t index, int32_t state, float_t duration) {
    if (index < 0 || index > 1)
        return;

    frame_disp [index] = disp;
    frame_state[index] = state;

    switch (index) {
    case 0:
        /*if (game_skin_get()->check_skin()) {
            set_aet_id_visible_enable(PV_GAME_AET_TOP_FRONT   , disp);
            set_aet_id_visible_enable(PV_GAME_AET_BOTTOM_FRONT, disp);
            set_aet_id_visible_enable(PV_GAME_AET_TOP_BACK    , disp);
            set_aet_id_visible_enable(PV_GAME_AET_BOTTOM_BACK , disp);
        }
        else */{
            set_aet_id_visible_enable(PV_GAME_AET_FRAME_BOTTOM, disp);
            set_aet_id_visible_enable(PV_GAME_AET_FRAME_TOP    , disp);
        }

        set_aet_id_visible_enable(PV_GAME_AET_0                      , disp);
        set_aet_id_visible_enable(PV_GAME_AET_1                      , disp);
        set_aet_id_visible_enable(PV_GAME_AET_SONG_ICON              , disp);
        set_aet_id_visible_enable(PV_GAME_AET_LEVEL_INFO             , disp);
        set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_BASE       , disp);
        set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_HISPEED    , disp);
        set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_HIDDEN     , disp);
        set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_SUDDEN     , disp);
        set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BASE       , disp);
        set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BASE_REACH , disp);
        set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_EDGE_LINE02, disp);
        set_aet_id_visible_enable(PV_GAME_AET_STAGE_INFO             , disp);

        set_song_energy(disp);
        break;
    case 1:
        set_aet_id_visible_enable(PV_GAME_AET_CHANCE_FRAME_BOTTOM, disp);
        set_aet_id_visible_enable(PV_GAME_AET_CHANCE_FRAME_TOP   , disp);
        break;
    }

    switch (state) {
    case 3:
        if (duration > 0.0f) {
            frame_btm_pos_diff[index] = (1.0f / duration) * frame_btm_pos_max[index];
            frame_top_pos_diff[index] = (1.0f / duration) * frame_top_pos_max[index];

            if (!index) {
                if (song_info_pos_diff < 0.0f)
                    song_info_pos_diff = (1.0f / duration) * frame_top_pos_max[index];
                if (song_energy_pos_diff < 0.0f)
                    song_energy_pos_diff = (1.0f / duration) * frame_btm_pos_max[0];
            }
        }
        else {
            frame_state[index] = 1;
            frame_btm_pos[index] = 0.0f;
            frame_top_pos[index] = 0.0f;
            field_218 = true;

            if (!index) {
                song_info_pos = 0.0f;
                song_energy_pos = 0.0f;
            }
        }
        break;
    case 4:
        if (duration > 0.0f) {
            frame_btm_pos_diff[index] = (1.0f / duration) * frame_btm_pos_max[index];
            frame_top_pos_diff[index] = (1.0f / duration) * frame_top_pos_max[index];
        }
        else {
            frame_state[index] = 2;
            frame_btm_pos[index] = frame_btm_pos_max[index];
            frame_top_pos[index] = frame_top_pos_max[index];
        }
        break;
    }

    ui_set_pos();
}
#pragma warning(pop)

void pv_game_play_data::free_aet(pv_aet aet, int32_t index) {
    if (index < 0 || index >= 8)
        return;

    switch (aet) {
    case PV_AET_FRONT:
        aet_manager_free_aet_object_reset(&aet_ids[index + PV_GAME_AET_FRONT]);
        break;
    case PV_AET_FRONT_LOW:
        aet_manager_free_aet_object_reset(&aet_ids[index + PV_GAME_AET_FRONT_LOW]);
        break;
    case PV_AET_FRONT_3D_SURF:
        aet_manager_free_aet_object_reset(&aet_ids[index + PV_GAME_AET_FRONT_3D_SURF]);
        break;
    case PV_AET_BACK:
        aet_manager_free_aet_object_reset(&aet_ids[index + PV_GAME_AET_BACK]);
        break;
    }
}

void pv_game_play_data::free_aet_title_image() {
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_TITLE_IMAGE]);
}

float_t pv_game_play_data::get_aet_frame(pv_aet aet, int32_t index) {
    if (index < 0 || index >= 8)
        return -1.0f;

    switch (aet) {
    case PV_AET_FRONT:
        return get_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_FRONT));
    case PV_AET_FRONT_LOW:
        return get_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_FRONT_LOW));
    case PV_AET_FRONT_3D_SURF:
        return get_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_FRONT_3D_SURF));
    case PV_AET_BACK:
        return get_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_BACK));
    }
    return -1.0f;
}

float_t pv_game_play_data::get_aet_id_frame(pv_game_aet aet_index) {
    if (aet_ids[aet_index])
        return aet_manager_get_obj_frame(aet_ids[aet_index]);
    return -1.0f;
}

bool pv_game_play_data::get_aet_next_info_disp() {
    return aet_ids[PV_GAME_AET_NEXT_INFO]
        && !aet_manager_get_obj_end(aet_ids[PV_GAME_AET_NEXT_INFO]);
}

int32_t pv_game_play_data::get_digit_count(int32_t score, int32_t digits) {
    if (score <= 0)
        return 1;

    int32_t _digits = 0;
    while (score > 0) {
        score /= 10;
        _digits++;
    }

    if (_digits < 1)
        return 1;

    if (_digits > digits)
        _digits = digits;
    return _digits;
}

void pv_game_play_data::init_aet(pv_aet aet, int32_t index, std::string&& layer_name, std::string&& aet_name,
    bool* not_init, float_t start_time, float_t end_time, FrameRateControl* frame_rate_control) {
    if (index < 0 || index >= 8)
        return;

    int32_t aet_id = pv_aet_id;
    if (aet_name.size()) {
        data_struct* aft_data = &data_list[DATA_AFT];
        aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

        aet_id = aft_aet_db->get_aet_by_name(aet_name.c_str())->id;
    }

    int32_t spr_index;
    pv_game_aet aet_index;
    spr::SprPrio prio;
    switch (aet) {
    case PV_AET_FRONT:
        spr_index = 0;
        aet_index = (pv_game_aet)(index + PV_GAME_AET_FRONT);
        prio = spr::SPR_PRIO_01;
        break;
    case PV_AET_FRONT_LOW:
        spr_index = 0;
        aet_index = (pv_game_aet)(index + PV_GAME_AET_FRONT_LOW);
        prio = spr::SPR_PRIO_00;
        break;
    case PV_AET_FRONT_3D_SURF:
        spr_index = 1;
        aet_index = (pv_game_aet)(index + PV_GAME_AET_FRONT_3D_SURF);
        prio = spr::SPR_PRIO_00;
        break;
    case PV_AET_BACK:
        spr_index = 2;
        aet_index = (pv_game_aet)(index + PV_GAME_AET_BACK);
        prio = spr::SPR_PRIO_01;
        break;
    default:
        return;
    }

    init_aet_id(aet_id, layer_name.c_str(), spr_index, aet_index,
        prio, not_init, start_time, end_time, frame_rate_control);

}

void pv_game_play_data::init_aet_black_fade() {
    fade = PV_GAME_AET_BLACK_FADE;
    reinit_aet_gam_cmn(PV_GAME_AET_BLACK_FADE, spr::SPR_PRIO_17, AET_PLAY_ONCE, spr_black_fade[0]);
    set_aet_id_frame(PV_GAME_AET_BLACK_FADE, 0.0f);
    set_aet_id_play(PV_GAME_AET_BLACK_FADE, false);
}

void pv_game_play_data::init_aet_chance_frame() {
    reinit_aet_gam_cmn(PV_GAME_AET_CHANCE_FRAME_BOTTOM, spr::SPR_PRIO_12, AET_LOOP, spr_chance_frame_bottom[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_CHANCE_FRAME_TOP   , spr::SPR_PRIO_12, AET_LOOP, spr_chance_frame_top   [0]);
}

void pv_game_play_data::init_aet_chance_point(int32_t chance_points, float_t pos_x, float_t pos_y) {
    vec2 pos = { pos_x, pos_y };
    target_pos_scale_offset_apply(&pos, &pos);
    reinit_aet_gam_cmn(PV_GAME_AET_CHANCE_POINT, spr::SPR_PRIO_15, AET_PLAY_ONCE,
        spr_chance_point[(get_digit_count(chance_points, 6) + 1) % 2 ? 1 : 0], &pos);
}

void pv_game_play_data::init_aet_chance_txt(bool end) {
    reinit_aet_gam_cmn(PV_GAME_AET_CHANCE_TXT, spr::SPR_PRIO_06, AET_PLAY_ONCE, spr_chance_txt[end ? 0 : 1]);
}

void pv_game_play_data::init_aet_combo(int32_t target_display_index,
    float_t pos_x, float_t pos_y, int32_t combo_count) {
    if (target_display_index < 0 || target_display_index >= 9 || combo_count < 0 || combo_count > 9999
        || combo_count > 0 && target_display_index != 0 && target_display_index != 2)
        return;

    pos_y = max_def(pos_y, 58.0f);

    combo_pos = { pos_x, pos_y };
    this->combo_count = combo_count;

    vec2 pos = { pos_x, pos_y };
    target_pos_scale_offset_apply(&pos, &pos);

    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_VALUE_TEXT]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_COMBO]);

    if (combo_count > 0) {
        if (combo_count < 10)
            value_text_spr_index = 0;
        else if (combo_count < 100)
            value_text_spr_index = 1;
        else if (combo_count < 1000)
            value_text_spr_index = combo_count == 50 * (combo_count / 50) ? 3 : 2;
        else
            value_text_spr_index = 4;

        if (target_display_index == 2)
            value_text_spr_index += 5;

        const char* name;
        if (value_text_time_offset < -(float_t)(1.0 / 120.0))
            name = spr_combo_tempo_cool_fine[value_text_spr_index];
        else if (value_text_time_offset > (float_t)(1.0 / 120.0))
            name = spr_combo_cool_fine[value_text_spr_index];
        else
            name = spr_combo_just_cool_fine[value_text_spr_index];
        aet_ids[PV_GAME_AET_COMBO] = init_aet_gam_cmn(PV_GAME_AET_COMBO,
            spr::SPR_PRIO_14, AET_PLAY_ONCE, name, &pos);
    }
    else {
        const char* name;
        if (value_text_time_offset < -(float_t)(1.0 / 120.0))
            name = spr_value_text_late[target_display_index];
        else if (value_text_time_offset > (float_t)(1.0 / 120.0))
            name = spr_value_text_early[target_display_index];
        else
            name = spr_value_text[target_display_index];
        aet_ids[PV_GAME_AET_VALUE_TEXT] = init_aet_gam_cmn(PV_GAME_AET_VALUE_TEXT,
            spr::SPR_PRIO_14, AET_PLAY_ONCE, name, &pos);
    }
}

void pv_game_play_data::init_aet_demo_font() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_DEMO_FONT]);
    aet_ids[PV_GAME_AET_DEMO_FONT] = aet_manager_init_aet_object(33, spr::SPR_PRIO_16,
        AET_PLAY_ONCE, spr_demo_font[0], 0, 0, 0, 0, -1.0f, -1.0f, 0, 0, aft_aet_db, aft_spr_db);
}

void pv_game_play_data::init_aet_demo_logo() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_DEMO_LOGO]);
    aet_ids[PV_GAME_AET_DEMO_LOGO] = aet_manager_init_aet_object(33, spr::SPR_PRIO_16,
        AET_LOOP, spr_demo_logo[0], 0, 0, 0, 0, -1.0f, -1.0f, 0, 0, aft_aet_db, aft_spr_db);
}

float_t pv_game_play_data::init_aet_edit_effect(int32_t aet_id, const char* name, bool loop, bool low_field) {
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_EDIT_EFFECT]);

    if (aet_id == -1)
        return 0.0f;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    std::vector<std::string> vec;
    aet_manager_get_scene_comp_layer_names(vec, aet_id, aft_aet_db);
    for (const std::string& i : vec)
        if (!i.compare("eff__f")) {
            name = i.c_str();
            break;
        }

#if PV_DEBUG
    FrameRateControl* frame_rate_control = get_sys_frame_rate();
#else
    FrameRateControl* frame_rate_control = 0;
#endif

    aet_ids[PV_GAME_AET_EDIT_EFFECT] = aet_manager_init_aet_object(aet_id,
        low_field ? spr::SPR_PRIO_00 : spr::SPR_PRIO_01, loop ? AET_LOOP : AET_PLAY_ONCE,
        name, 0, 0, 0, 0, -1.0f, -1.0f, 0, frame_rate_control, aft_aet_db, aft_spr_db);
    set_aet_id_play(PV_GAME_AET_EDIT_EFFECT, false);

    return aet_manager_get_scene_layer_end_time(aet_id, name, aft_aet_db);
}

void pv_game_play_data::init_aet_frame(bool danger) {
    if (frame_danger_state != (danger ? 2 : 1)) {
        data_struct* aft_data = &data_list[DATA_AFT];
        aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

        /*if (game_skin_get()->check_skin()) {
            aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_BOTTOM_FRONT]);
            aet_ids[PV_GAME_AET_BOTTOM_FRONT] = aet_manager_init_aet_object(game_skin_get()->get_skin_aet(),
                spr::SPR_PRIO_10, AET_LOOP, "bottom_front", 0, 0, 0, 0, -1.0f, -1.0f, 0, 0, aft_aet_db);
            aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_BOTTOM_BACK]);
            aet_ids[PV_GAME_AET_BOTTOM_BACK] = aet_manager_init_aet_object(game_skin_get()->get_skin_aet(),
                spr::SPR_PRIO_03, AET_LOOP, "bottom_back", 0, 0, 0, 0, -1.0f, -1.0f, 0, 0, aft_aet_db);
        }
        else */
            reinit_aet_gam_cmn(PV_GAME_AET_FRAME_BOTTOM, spr::SPR_PRIO_10, AET_LOOP, spr_frame[danger ? 2 : 0]);

        /*if (game_skin_get()->check_skin()) {
            aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_TOP_FRONT]);
            aet_ids[PV_GAME_AET_TOP_FRONT] = aet_manager_init_aet_object(game_skin_get()->get_skin_aet(),
                spr::SPR_PRIO_10, AET_LOOP, "top_front", 0, 0, 0, 0, -1.0f, -1.0f, 0, 0, aft_aet_db);
            aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_TOP_BACK]);
            aet_ids[PV_GAME_AET_TOP_BACK] = aet_manager_init_aet_object(game_skin_get()->get_skin_aet(),
                spr::SPR_PRIO_03, AET_LOOP, "top_back", 0, 0, 0, 0, -1.0f, -1.0f, 0, 0, aft_aet_db);
        }
        else */
            reinit_aet_gam_cmn(PV_GAME_AET_FRAME_TOP, spr::SPR_PRIO_10, AET_LOOP, spr_frame[danger ? 3 : 1]);

        frame_danger_state = danger ? 2 : 1;
    }

    /*if (game_skin_get()->check_skin()) {
        set_aet_id_visible_enable(PV_GAME_AET_BOTTOM_FRONT, frame_disp[0]);
        set_aet_id_visible_enable(PV_GAME_AET_BOTTOM_BACK , frame_disp[0]);
        set_aet_id_visible_enable(PV_GAME_AET_TOP_FRONT   , frame_disp[0]);
        set_aet_id_visible_enable(PV_GAME_AET_TOP_BACK    , frame_disp[0]);
    }
    else */{
        set_aet_id_visible_enable(PV_GAME_AET_FRAME_BOTTOM, frame_disp[0]);
        set_aet_id_visible_enable(PV_GAME_AET_FRAME_TOP   , frame_disp[0]);
    }
}

uint32_t pv_game_play_data::init_aet_gam_cmn(pv_game_aet aet_index, spr::SprPrio prio,
    AetFlags flags, const char* layer_name, const vec2* pos, const vec2* scale) {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    uint32_t aet_id = aet_manager_init_aet_object(3, prio, flags, layer_name, pos,
        0, 0, 0, -1.0f, -1.0f, scale, 0, aft_aet_db, aft_spr_db);
    aet_ids_enable[aet_index] = true;
    return aet_id;
}

void pv_game_play_data::init_aet_id(int32_t aet_id, const char* layer_name,
    int32_t index, pv_game_aet aet_index, spr::SprPrio prio, bool* not_init,
    float_t start_time, float_t end_time, FrameRateControl* frame_rate_control) {
    aet_manager_free_aet_object_reset(&aet_ids[aet_index]);
    if (!pv_set || !layer_name || !*layer_name)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    aet_ids[aet_index] = aet_manager_init_aet_object(aet_id, prio, AET_PLAY_ONCE, layer_name, 0,
        index, 0, 0, start_time, end_time, 0, frame_rate_control, aft_aet_db, aft_spr_db);
    if (!aet_ids[aet_index] && not_init)
        *not_init = true;
}

void pv_game_play_data::init_aet_level_info() {
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_LEVEL_INFO]);

    pv_difficulty difficulty = sub_14013C8C0()->difficulty;
    pv_edition edition = sub_14013C8C0()->edition;

    const pv_db_pv_difficulty* diff = pv_game_get()->data.pv->get_difficulty(difficulty, edition);
    if (!diff)
        return;

    bool extra = diff->attribute.type == PV_ATTRIBUTE_EXTRA;

    const char* name;
    if (player_data_array_get(0)->contest.enable)
        name = (extra ? spr_level_info_ctst_extra : spr_level_info_ctst)[difficulty];
    else if (pv_game_get()->data.no_fail)
        name = (extra ? spr_level_info_insu_extra : spr_level_info_insu)[difficulty];
    else
        name = (extra ? spr_level_info_extra : spr_level_info)[difficulty];

    aet_ids[PV_GAME_AET_LEVEL_INFO] = init_aet_gam_cmn(PV_GAME_AET_LEVEL_INFO, spr::SPR_PRIO_13, AET_LOOP, name);
    set_aet_id_visible_enable(PV_GAME_AET_LEVEL_INFO, frame_disp[0]);
}

void pv_game_play_data::init_aet_next_info() {
    reinit_aet_gam_cmn(PV_GAME_AET_NEXT_INFO, spr::SPR_PRIO_22, AET_PLAY_ONCE,
        pv_game_get()->data.next_stage ? spr_next_info[0] : spr_failed_info[0]);
}

void pv_game_play_data::init_aet_option_info() {
    if (!player_data_array_get(0)->field_0 || !player_data_array_get(0)->game_opts)
        return;

    int32_t option = pv_game_get()->data.play_data.option;
    if (!option)
        return;

    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_OPTION_INFO_BASE]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_OPTION_INFO_HISPEED]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_OPTION_INFO_HIDDEN]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_OPTION_INFO_SUDDEN]);
    aet_ids[PV_GAME_AET_OPTION_INFO_BASE   ] = init_aet_gam_cmn(PV_GAME_AET_OPTION_INFO_BASE,
        spr::SPR_PRIO_13, AET_PLAY_ONCE, spr_option_info_base   [0]);
    aet_ids[PV_GAME_AET_OPTION_INFO_HISPEED] = init_aet_gam_cmn(PV_GAME_AET_OPTION_INFO_HISPEED,
        spr::SPR_PRIO_13, AET_PLAY_ONCE, spr_option_info_hispeed[option != 1 ? 1 : 0]);
    aet_ids[PV_GAME_AET_OPTION_INFO_HIDDEN ] = init_aet_gam_cmn(PV_GAME_AET_OPTION_INFO_HIDDEN,
        spr::SPR_PRIO_13, AET_PLAY_ONCE, spr_option_info_hidden [option != 2 ? 1 : 0]);
    aet_ids[PV_GAME_AET_OPTION_INFO_SUDDEN ] = init_aet_gam_cmn(PV_GAME_AET_OPTION_INFO_SUDDEN,
        spr::SPR_PRIO_13, AET_PLAY_ONCE, spr_option_info_sudden [option != 3 ? 1 : 0]);
    set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_BASE   , frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_HISPEED, frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_HIDDEN , frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_OPTION_INFO_SUDDEN , frame_disp[0]);
}

void pv_game_play_data::init_aet_slide_max(int32_t slide_point, float_t pos_x, float_t pos_y) {
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_SLIDE_POINT]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_POINT]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_PLUS]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_0]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_1]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_2]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_3]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_4]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_5]);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_6]);

    vec2 v27 = { pos_x, pos_y };
    target_pos_scale_offset_apply(&v27, &v27);

    aet_ids[PV_GAME_AET_MAX_SLIDE_POINT] = init_aet_gam_cmn(PV_GAME_AET_MAX_SLIDE_POINT, spr::SPR_PRIO_15,
        AET_PLAY_ONCE, spr_max_slide_point[(get_digit_count(slide_point, 6) + 1) % 2 ? 1 : 0], &v27);

    int32_t v8 = get_digit_count(max_slide_points, 6);

    int32_t v10;
    int32_t v11;
    if ((v8 + 1) % 2) {
        v10 = 1;
        v11 = 6 - v8;
    }
    else {
        v10 = 0;
        v11 = 5 - v8;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    AetComp comp;
    aet_manager_init_aet_layout(&comp, 3, spr_max_slide_point[v10], (AetFlags)0, RESOLUTION_MODE_HD,
        0, aet_manager_get_obj_frame(aet_ids[PV_GAME_AET_MAX_SLIDE_POINT]), aft_aet_db, aft_spr_db);

    vec2 pos ;
    target_pos_scale_offset_apply(&max_slide_points_pos, &pos);

    const aet_layout_data* layout = comp.Find((&spr_p_chance_point_num[v11 / 2])[v8]);
    if (layout) {
        vec2 position = pos + *(vec2*)&layout->position;
        aet_ids[PV_GAME_AET_MAX_SLIDE_NUM_PLUS] = init_aet_gam_cmn(PV_GAME_AET_MAX_SLIDE_NUM_PLUS,
            spr::SPR_PRIO_15, AET_PLAY_ONCE, spr_max_slide_num_plus[0], &position);
    }

    for (int32_t i = 0; i < 7; i++) {
        int32_t digit = slide_point % 10;
        slide_point /= 10;

        const aet_layout_data* layout = comp.Find(spr_p_chance_point_num[i]);
        if (layout) {
            vec2 position = pos + *(vec2*)&layout->position;
            aet_ids[PV_GAME_AET_MAX_SLIDE_NUM + i] = init_aet_gam_cmn((pv_game_aet)(PV_GAME_AET_MAX_SLIDE_NUM + i),
                spr::SPR_PRIO_15, AET_PLAY_ONCE, spr_max_slide_num[digit], &position);
        }

        if (!slide_point)
            break;
    }
}

void pv_game_play_data::init_aet_slide_point(int32_t slide_point, float_t pos_x, float_t pos_y) {
    vec2 pos = { pos_x, pos_y };
    target_pos_scale_offset_apply(&pos, &pos);
    aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_SLIDE_POINT]);
    aet_ids[PV_GAME_AET_SLIDE_POINT] = init_aet_gam_cmn(PV_GAME_AET_SLIDE_POINT, spr::SPR_PRIO_15,
        AET_PLAY_ONCE, spr_chance_point[(get_digit_count(slide_point, 6) + 1) % 2 ? 1 : 0], &pos);
}

void pv_game_play_data::init_aet_song_energy() {
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_BASE,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_base[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_BASE_REACH,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_base_reach[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_NORMAL,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_normal[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_REACH,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_reach[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_FULL,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_full[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_EFF_NORMAL,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_eff_normal[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_EFF_REACH,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_eff_reach[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_CLEAR_TXT,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_clear_txt[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_BORDER_GREAT,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_border_great[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_border_excellent[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_BORDER_RIVAL,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_border_rival[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_EDGE_LINE02,
        spr::SPR_PRIO_15, AET_LOOP, spr_song_energy_edge_line02[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_NOT_CLEAR,
        spr::SPR_PRIO_16, AET_LOOP, spr_song_energy_not_clear[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_BORDER,
        spr::SPR_PRIO_16, AET_LOOP, spr_song_energy_border[0]);
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ENERGY_NOT_CLEAR_TXT,
        spr::SPR_PRIO_16, AET_LOOP, spr_song_energy_not_clear_txt[0]);

    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BASE            , frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BASE_REACH      , frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_CLEAR_TXT       , frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BORDER_GREAT    , frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT, frame_disp[0]);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_EDGE_LINE02     , frame_disp[0]);

    set_song_energy(frame_disp[0]);

    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_BASE            , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_CLEAR_TXT       , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_BORDER_GREAT    , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT, false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_EDGE_LINE02     , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_BASE_REACH      , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_NORMAL          , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_REACH           , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_EFF_NORMAL      , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_EFF_REACH       , false);
    set_aet_id_play(PV_GAME_AET_SONG_ENERGY_BORDER          , false);
}

void pv_game_play_data::init_aet_song_icon_loop() {
    reinit_aet_gam_cmn(PV_GAME_AET_SONG_ICON, spr::SPR_PRIO_13, AET_LOOP, spr_song_icon_loop[0]);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ICON, frame_disp[0]);
}

void pv_game_play_data::init_aet_stage_info() {
    if (stage_index < 0 || stage_index >= 4)
        return;

    reinit_aet_gam_cmn(PV_GAME_AET_STAGE_INFO, spr::SPR_PRIO_13, AET_LOOP, spr_stage_info[stage_index]);
    set_aet_id_visible_enable(PV_GAME_AET_STAGE_INFO, frame_disp[0]);
}

void pv_game_play_data::init_aet_success_info() {
    reinit_aet_gam_cmn(PV_GAME_AET_SUCCESS_INFO, spr::SPR_PRIO_02, (AetFlags)0, spr_success_info[0]);
}

void pv_game_play_data::init_aet_title_image(const char* name) {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

#if PV_DEBUG
    FrameRateControl* frame_rate_control = get_sys_frame_rate();
#else
    FrameRateControl* frame_rate_control = 0;
#endif

    aet_ids[PV_GAME_AET_TITLE_IMAGE] = aet_manager_init_aet_object(pv_aet_id, spr::SPR_PRIO_02,
        (AetFlags)0, name, 0, 0, 0, 0, -1.0f, -1.0f, 0, frame_rate_control, aft_aet_db, aft_spr_db);
}

void pv_game_play_data::init_aet_white_fade() {
    fade = PV_GAME_AET_WHITE_FADE;
    reinit_aet_gam_cmn(PV_GAME_AET_WHITE_FADE, spr::SPR_PRIO_17, AET_PLAY_ONCE, spr_white_fade[0]);
    set_aet_id_frame(PV_GAME_AET_WHITE_FADE, 0.0f);
    set_aet_id_play(PV_GAME_AET_WHITE_FADE, false);
}

bool pv_game_play_data::load_auth_2d(int32_t stage_index, pv_disp2d* disp2d) {
    if (!state) {
        reset();
        set_pv_disp2d(disp2d);
        set_stage_index(stage_index);
        update = true;
        read_auth_2d_data();
        state = 1;
    }
    else if (state == 1 && !load_auth_2d_data()) {
        sub_140135ED0();
        state = 0;
        loaded = 1;
        return true;
    }
    return false;
}

bool pv_game_play_data::load_auth_2d_data() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    if (pv_set) {
        if (pv_spr_set_id != -1 && sprite_manager_load_file(pv_spr_set_id, aft_spr_db))
            return true;
        if (pv_aet_set_id != -1 && aet_manager_load_file(pv_aet_set_id, aft_aet_db))
            return true;
    }

    if (sprite_manager_load_file(9, aft_spr_db) || aet_manager_load_file(3, aft_aet_db))
        return true;

    return false;//game_skin_get()->load_file();
}

void pv_game_play_data::read_auth_2d_data() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    sprite_manager_read_file(spr_gam_cmn_set_id, "", aft_data, aft_spr_db);
    aet_manager_read_file(aet_gam_cmn_set_id, "", aft_data, aft_aet_db);

    if (pv_set) {
        std::string mdata_dir;
        if (pv_game_get())
            mdata_dir.assign(pv_game_get()->data.pv->mdata.dir);

        if (pv_spr_set_id != -1)
            sprite_manager_read_file(pv_spr_set_id, mdata_dir, aft_data, aft_spr_db);

        if (pv_aet_set_id != -1)
            aet_manager_read_file(pv_aet_set_id, mdata_dir, aft_data, aft_aet_db);
    }

    //game_skin_get()->load();
}

void pv_game_play_data::reset() {
    loaded = false;
    update = false;
    field_378 = false;
    score_speed = 0.04f;
    field_36C = -1;
    combo_state = 4;
    pv_set = false;
    stage_index = 0;
    field_354 = false;
    not_clear = false;
    pv_spr_set_id = -1;
    pv_aet_set_id = -1;
    pv_aet_id = -1;

    sub_1401362C0();
}

void pv_game_play_data::set_aet_edit_effect_frame(float_t frame) {
    set_aet_id_frame(PV_GAME_AET_EDIT_EFFECT, frame);
}

void pv_game_play_data::set_aet_end_time(pv_aet aet, int32_t index, float_t value) {
    if (index < 0 || index >= 8)
        return;

    switch (aet) {
    case PV_AET_FRONT:
        set_aet_id_end_time((pv_game_aet)(index + PV_GAME_AET_FRONT), value);
        break;
    case PV_AET_FRONT_LOW:
        set_aet_id_end_time((pv_game_aet)(index + PV_GAME_AET_FRONT_LOW), value);
        break;
    case PV_AET_FRONT_3D_SURF:
        set_aet_id_end_time((pv_game_aet)(index + PV_GAME_AET_FRONT_3D_SURF), value);
        break;
    case PV_AET_BACK:
        set_aet_id_end_time((pv_game_aet)(index + PV_GAME_AET_BACK), value);
        break;
    }
}

void pv_game_play_data::set_aet_frame(pv_aet aet, int32_t index, float_t value) {
    if (index < 0 || index >= 8)
        return;

    switch (aet) {
    case PV_AET_FRONT:
        set_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_FRONT), value);
        break;
    case PV_AET_FRONT_LOW:
        set_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_FRONT_LOW), value);
        break;
    case PV_AET_FRONT_3D_SURF:
        set_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_FRONT_3D_SURF), value);
        break;
    case PV_AET_BACK:
        set_aet_id_frame((pv_game_aet)(index + PV_GAME_AET_BACK), value);
        break;
    }
}

void pv_game_play_data::set_aet_id_alpha(pv_game_aet aet_index, float_t value) {
    if (aet_ids[aet_index])
        aet_manager_set_obj_alpha(aet_ids[aet_index], value);
}

void pv_game_play_data::set_aet_id_end_time(pv_game_aet aet_index, float_t value) {
    if (aet_ids[aet_index])
        aet_manager_set_obj_end_time(aet_ids[aet_index], value);
}

void pv_game_play_data::set_aet_id_frame(pv_game_aet aet_index, float_t value) {
    if (aet_ids[aet_index])
        aet_manager_set_obj_frame(aet_ids[aet_index], value);
}

void pv_game_play_data::set_aet_id_play(pv_game_aet aet_index, bool value) {
    if (aet_ids[aet_index])
        aet_manager_set_obj_play(aet_ids[aet_index], value);
}

void pv_game_play_data::set_aet_id_position(pv_game_aet aet_index, const vec3& position) {
    if (aet_ids[aet_index])
        aet_manager_set_obj_position(aet_ids[aet_index], position);
}

void pv_game_play_data::set_aet_id_visible(pv_game_aet aet_index, bool value) {
    if (aet_ids[aet_index])
        aet_manager_set_obj_visible(aet_ids[aet_index], value);
}

void pv_game_play_data::set_aet_id_visible_enable(pv_game_aet aet_index, bool value) {
    aet_ids_enable[aet_index] = value;
    set_aet_id_visible(aet_index, value);
}

void pv_game_play_data::set_aet_song_energy() {
    pv_game* pv_game = pv_game_get();
    if (!loaded)
        return;

    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_BASE       , pv_game->data.song_energy_base);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_CLEAR_TXT  , pv_game->data.song_energy_base);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_EDGE_LINE02, pv_game->data.song_energy_base);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_BASE_REACH , pv_game->data.song_energy_base);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_NORMAL     , pv_game->data.song_energy);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_REACH      , pv_game->data.song_energy);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_EFF_NORMAL , pv_game->data.song_energy);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_EFF_REACH  , pv_game->data.song_energy);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_BORDER     , pv_game->data.song_energy_border);

    if (pv_game->data.song_energy_base <= 0.0f)
        return;

    float_t song_energy_base = pv_game->data.song_energy_base;
    float_t song_energy_border = pv_game->data.song_energy_border;

    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_BORDER_GREAT,
        1.0f / song_energy_base * get_percentage_clear_great() * song_energy_border);
    set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT,
        1.0f / song_energy_base * get_percentage_clear_excellent() * song_energy_border);
    if (pv_game->get_data_rival_percentage() > 0.0f) {
        float_t song_energy_rival
            = 1.0f / song_energy_base * pv_game->get_data_rival_percentage() * song_energy_border;
        song_energy_rival = min_def(song_energy_rival, 100.0f);
        set_aet_id_frame(PV_GAME_AET_SONG_ENERGY_BORDER_RIVAL, song_energy_rival);
    }
}

void pv_game_play_data::set_aet_visible(pv_aet aet, int32_t index, bool value) {
    if (index < 0 || index >= 8)
        return;

    switch (aet) {
    case PV_AET_FRONT:
        set_aet_id_visible((pv_game_aet)(index + PV_GAME_AET_FRONT), value);
        break;
    case PV_AET_FRONT_LOW:
        set_aet_id_visible((pv_game_aet)(index + PV_GAME_AET_FRONT_LOW), value);
        break;
    case PV_AET_FRONT_3D_SURF:
        set_aet_id_visible((pv_game_aet)(index + PV_GAME_AET_FRONT_3D_SURF), value);
        break;
    case PV_AET_BACK:
        set_aet_id_visible((pv_game_aet)(index + PV_GAME_AET_BACK), value);
        break;
    }
}

void pv_game_play_data::set_not_clear() {
    not_clear = true;
}

void pv_game_play_data::set_pv_disp2d(pv_disp2d* disp2d) {
    pv_set = false;
    if (!disp2d || disp2d->pv_id < 0
        || disp2d->pv_spr_set_id == -1
        || disp2d->pv_aet_set_id == -1
        || disp2d->pv_aet_id == -1)
        return;

    pv_set = true;
    pv_spr_set_id = disp2d->pv_spr_set_id;
    pv_aet_set_id = disp2d->pv_aet_set_id;
    pv_aet_id = disp2d->pv_aet_id;
}

void pv_game_play_data::set_song_energy(bool disp) {
    bool normal = false;
    bool reach = false;
    bool base_eff_reach = false;
    bool full = false;
    bool border = false;
    bool border_great = false;
    bool border_excellent = false;
    bool border_rival = false;
    bool not_clear = false;
    bool clear_txt = false;

    if (disp) {
        pv_game* pv_game = pv_game_get();
        if (pv_game->data.song_energy >= 100.0f)
            full = true;
        else if (pv_game->data.song_energy >= pv_game->data.song_energy_base) {
            reach = true;
            base_eff_reach = true;
        }
        else if (pv_game->data.song_energy >= pv_game->data.song_energy_border)
            reach = true;
        else
            normal = true;

        border = true;
        int32_t clear_border = player_data_array_get(0)->clear_border;
        border_great = !!(clear_border & 0x01);
        border_excellent = !!(clear_border & 0x02);
        border_rival = (clear_border & 0x04) && pv_game->get_data_rival_percentage() > 0.0f;
        not_clear = this->not_clear;
        clear_txt = true;
    }

    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_NORMAL          , normal);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_EFF_NORMAL      , normal);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_REACH           , reach);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BASE_REACH      , base_eff_reach);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_EFF_REACH       , base_eff_reach);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_FULL            , full);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BORDER          , border);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BORDER_GREAT    , border_great);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT, border_excellent);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_BORDER_RIVAL    , border_rival);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_NOT_CLEAR       , not_clear);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_NOT_CLEAR_TXT   , not_clear);
    set_aet_id_visible_enable(PV_GAME_AET_SONG_ENERGY_CLEAR_TXT       , clear_txt);
}

void pv_game_play_data::set_spr_set_back_id(int32_t value) {
    spr_set_back_id = value;
}

void pv_game_play_data::set_stage_index(int32_t value) {
    stage_index = value;
}

void pv_game_play_data::set_value_text(int32_t index, float_t time_offset) {
    value_text_time_offset = time_offset;
    value_text_index = index;
    value_text_display = true;
}

void pv_game_play_data::skin_danger_ctrl() {
    /*if (!game_skin_get()->check_skin())
        return;

    skin_danger_frame--;

    vec4 color = 1.0f;
    if (frame_danger_state == 2) {
        if (skin_danger_frame < 0)
            skin_danger_frame = 30;
        else if (skin_danger_frame < 15)
            color = { 1.0f, 0.6f, 0.7f, 1.0f };
    }

    aet_manager_set_obj_color(aet_ids[PV_GAME_AET_BOTTOM_BACK ], color);
    aet_manager_set_obj_color(aet_ids[PV_GAME_AET_BOTTOM_FRONT], color);
    aet_manager_set_obj_color(aet_ids[PV_GAME_AET_TOP_BACK    ], color);
    aet_manager_set_obj_color(aet_ids[PV_GAME_AET_TOP_FRONT   ], color);*/
}

void pv_game_play_data::score_update(uint32_t score_final, bool update) {
    if (!this->update && !update)
        return;

    if (score_final - score > 140)
        score += (int32_t)((float_t)(10 * (score_final - score)) * score_speed);
    else if (score_final - score > 20)
        score += 20;
    else
        score = score_final;
}

void pv_game_play_data::ui_set_disp() {
    for (int32_t i = PV_GAME_AET_0; i <= PV_GAME_AET_SONG_ICON; i++)
        set_aet_id_visible((pv_game_aet)i, aet_ids_enable[i]);
}

void pv_game_play_data::ui_set_pos() {
    const vec3        frame_btm_pos = { 0.0f,  this->frame_btm_pos[0], 0.0f };
    const vec3 chance_frame_btm_pos = { 0.0f,  this->frame_btm_pos[1], 0.0f };
    const vec3        frame_top_pos = { 0.0f, -this->frame_top_pos[0], 0.0f };
    const vec3 chance_frame_top_pos = { 0.0f, -this->frame_top_pos[1], 0.0f };
    const vec3        song_info_pos = { 0.0f, -this->   song_info_pos, 0.0f };
    const vec3      song_energy_pos = { 0.0f,  this-> song_energy_pos, 0.0f };

    /*if (game_skin_get()->check_skin()) {
        set_aet_id_position(PV_GAME_AET_BOTTOM_FRONT, frame_btm_pos);
        set_aet_id_position(PV_GAME_AET_BOTTOM_BACK , frame_btm_pos);
    }
    else*/
        set_aet_id_position(PV_GAME_AET_FRAME_BOTTOM, frame_btm_pos);

    set_aet_id_position(PV_GAME_AET_0                           , frame_btm_pos);
    set_aet_id_position(PV_GAME_AET_1                           , frame_btm_pos);
    set_aet_id_position(PV_GAME_AET_STAGE_INFO                  , frame_btm_pos);

    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_BASE            , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_BASE_REACH      , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_NORMAL          , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_REACH           , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_FULL            , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_EFF_NORMAL      , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_EFF_REACH       , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_CLEAR_TXT       , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_BORDER_GREAT    , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT, song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_BORDER_RIVAL    , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_EDGE_LINE02     , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_BORDER          , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_NOT_CLEAR       , song_energy_pos);
    set_aet_id_position(PV_GAME_AET_SONG_ENERGY_NOT_CLEAR_TXT   , song_energy_pos);

    /*if (game_skin_get()->check_skin()) {
        set_aet_id_position(PV_GAME_AET_TOP_FRONT, frame_top_pos);
        set_aet_id_position(PV_GAME_AET_TOP_BACK , frame_top_pos);
    }
    else*/
        set_aet_id_position(PV_GAME_AET_FRAME_TOP, frame_top_pos);

    set_aet_id_position(PV_GAME_AET_CHANCE_FRAME_BOTTOM, chance_frame_btm_pos);
    set_aet_id_position(PV_GAME_AET_CHANCE_FRAME_TOP   , chance_frame_top_pos);

    set_aet_id_position(PV_GAME_AET_SONG_ICON          , song_info_pos);
    set_aet_id_position(PV_GAME_AET_LEVEL_INFO         , song_info_pos);
    set_aet_id_position(PV_GAME_AET_OPTION_INFO_BASE   , song_info_pos);
    set_aet_id_position(PV_GAME_AET_OPTION_INFO_HISPEED, song_info_pos);
    set_aet_id_position(PV_GAME_AET_OPTION_INFO_HIDDEN , song_info_pos);
    set_aet_id_position(PV_GAME_AET_OPTION_INFO_SUDDEN , song_info_pos);
}

void pv_game_play_data::unload() {
    if (!loaded)
        return;

    task_game_2d_sub_140372670(false);
    task_game_2d_del_task();

    for (uint32_t& i : aet_ids)
        aet_manager_free_aet_object_reset(&i);

    unload_auth_2d();
    loaded = false;
}

void pv_game_play_data::unload_aet() {
    for (int32_t i = 0; i < 8; i++) {
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_FRONT]);
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_FRONT_LOW]);
        aet_manager_free_aet_object_reset(&aet_ids[PV_GAME_AET_BACK]);
    }
}

void pv_game_play_data::unload_auth_2d() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    unload_auth_2d_data();

    if (pv_set) {
        if (pv_spr_set_id != -1)
            sprite_manager_unload_set(pv_spr_set_id, aft_spr_db);
        if (pv_aet_set_id != -1)
            aet_manager_unload_set(pv_aet_set_id, aft_aet_db);
    }

    sprite_manager_unload_set(spr_gam_cmn_set_id, aft_spr_db);
    aet_manager_unload_set(aet_gam_cmn_set_id, aft_aet_db);

    //game_skin_get()->unload_skin();
}

void pv_game_play_data::unload_auth_2d_data() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

    if (pv_set && pv_aet_set_id != -1)
        aet_manager_free_aet_set_objects(pv_aet_set_id, aft_aet_db);

    aet_manager_free_aet_set_objects(3, aft_aet_db);
}

void pv_game_play_data::sub_140134670(int32_t chance_result) {
    if (!field_378)
        return;

    sub_140137F80(true, 1.0f);
    frame_set(true, 1, 4, 60.0f);

    this->chance_result = chance_result;
    field_37C = 0.0f;
    field_374 = 4;
}

void pv_game_play_data::sub_140134730() {
    sub_140137F80(false, 1.0f);
    frame_set(true, 1, 3, 60.0f);
    update = true;
    field_37C = 0.0f;
    field_374 = 0;
    field_378 = true;
}

void pv_game_play_data::sub_1401349B0() {
    sub_1401349C0(9);
}

void pv_game_play_data::sub_1401349C0(uint32_t set_id) {
    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    if (!sprite_manager_get_set_ready(aft_spr_db->get_spr_set_by_id(set_id)->index, aft_spr_db))
        return;

    uint32_t count = sprite_manager_get_set_texture_num(set_id, aft_spr_db);
    for (uint32_t i = 0; i < count; i++) {
        texture* tex = sprite_manager_get_spr_texture(
            aft_spr_db->get_tex_by_set_id_index(set_id, i)->id, aft_spr_db);
        if (!tex)
            continue;

        spr::SprArgs args;
        args.id.id = -1;
        args.prio = spr::SPR_PRIO_04;
        args.texture = tex;
        args.SetSpriteSize({ (float_t)tex->width, (float_t)tex->height });
        args.SetTexturePosSize(0.0f, 0.0f, (float_t)tex->width, (float_t)tex->height);
        spr::put_sprite(args, aft_spr_db);
    }
}

void pv_game_play_data::sub_140135ED0() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    AetArgs args;
    args.id.id = 3;
    args.mode = RESOLUTION_MODE_HD;
    args.spr_db = aft_spr_db;

    comp.Clear();
    aet_manager_init_aet_layout(&comp, args, aft_aet_db);

    for (int32_t i = 0; i < 2; i++) {
        const aet_layout_data* p_song_layout = comp.Find(spr_p_song_lt[i]);
        if (p_song_layout)
            song_txt[i] = *p_song_layout;
    }

    const aet_layout_data* p_chance_rslt_num_0_layout = comp.Find(spr_p_chance_rslt_num[0]);
    const aet_layout_data* p_chance_rslt_num_1_layout = comp.Find(spr_p_chance_rslt_num[1]);
    if (p_chance_rslt_num_0_layout && p_chance_rslt_num_1_layout)
        field_350 = p_chance_rslt_num_0_layout->position.x - p_chance_rslt_num_1_layout->position.x;

    if (sub_14013C8C0()->sub_1400E7910() == 3)
        init_aet_demo_logo();
    else {
        init_aet_frame(false);
        init_aet_chance_frame();
        init_aet_song_icon_loop();
        init_aet_song_energy();
        init_aet_level_info();
        init_aet_option_info();
        init_aet_stage_info();
        frame_set(false, 0, 0, 0.0f);
        frame_set(false, 1, 0, 0.0f);
    }

    /*if (game_skin_get()->check_skin()) {
        frame_top_pos_max[0] = 105.0f;
        frame_btm_pos_max[0] = 200.0f;
    }*/

    init_aet_white_fade();

    task_game_2d_add_task();
    task_game_2d_sub_140372670(false);
    task_game_2d_set_energy_unit_no_fail(pv_game_get()->data.no_fail);
}

void pv_game_play_data::sub_1401362C0() {
    for (int32_t i = 0; i < PV_GAME_AET_MAX; i++) {
        aet_ids[i] = 0;
        aet_ids_enable[i] = false;
    }

    for (int32_t i = 0; i < 2; i++) {
        frame_btm_pos[i] = 0.0f;
        frame_btm_pos_diff[i] = 0.0f;
        frame_top_pos[i] = 0.0f;
        frame_top_pos_diff[i] = 0.0f;
        frame_disp[i] = false;
        frame_state[i] = 0;
    }

    frame_btm_pos[0] = 86.0f;
    frame_btm_pos[1] = 138.0f;

    frame_top_pos[0] = 58.0f;
    frame_top_pos[1] = 138.0f;

    song_info_pos = 58.0f;
    song_info_pos_diff = -1.0f;

    song_energy_pos = 86.0f;
    song_energy_pos_diff = -1.0f;

    frame_top_pos_max[0] = 58.0f;
    frame_top_pos_max[1] = 138.0f;

    frame_btm_pos_max[0] = 86.0f;
    frame_btm_pos_max[1] = 138.0f;

    frame_danger_state = 0;
    field_218 = false;
    fade_begin = false;
    fade_end = false;
    fade = PV_GAME_AET_WHITE_FADE;
    skin_danger_frame = 0;
    field_2FC = 0;
    chance_result = 0;
    chance_points = 0;
    chance_points_pos = 0.0f;
    field_310 = 0;
    slide_points = 0;
    slide_points_pos = 0.0f;
    max_slide_points = 0;
    max_slide_points_pos = 0.0f;
    field_33C = 0;
    comp.Clear();
    field_350 = 0.0f;
    value_text_spr_index = 0;
    spr_set_back_id = -1;
    field_374 = 0;
    field_37C = 0.0f;
    score = 0;
    value_text_display = 0;
    field_38C = 0.0f;
    value_text_index = 0;
    value_text_time_offset = 0.0f;
    combo_count = 0;
    combo_disp_time = 0;
    combo_pos = 0.0f;
}

void pv_game_play_data::sub_140137BE0() {
    if (!loaded)
        return;

    field_36C = -1;
    update = false;
    field_378 = false;
    score_speed = 0.04f;
    combo_state = 4;

    for (uint32_t& i : aet_ids)
        aet_manager_free_aet_object_reset(&i);

    sub_1401362C0();
    sub_140135ED0();
}

void pv_game_play_data::sub_140137DD0(int32_t a2, float_t delta_time, vec2& pos, int32_t combo_count) {
    if (!update)
        return;

    if (field_36C != a2) {
        if (a2 > 3)
            init_aet_frame(false);
        else
            init_aet_frame(!pv_game_get()->data.no_fail);
        field_36C = a2;
    }

    switch (value_text_display) {
    case 1:
        init_aet_combo(value_text_index, pos.x, pos.y, combo_count);
        field_38C = 0.025f;
        value_text_display = 2;
        break;
    case 2:
        field_38C -= delta_time;
        if (field_38C <= 0.0f) {
            field_38C = 2.0f;
            value_text_display = 3;
        }
        break;
    case 3:
        field_38C -= delta_time;
        if (field_38C <= 0.0f)
            value_text_display = 0;
        break;
    }
    set_song_energy(frame_disp[0]);

    float_t alpha = field_33C % 60 < 30 ? 1.0f : 0.0f;
    set_aet_id_alpha(PV_GAME_AET_SONG_ENERGY_EFF_NORMAL, alpha);
    set_aet_id_alpha(PV_GAME_AET_SONG_ENERGY_EFF_REACH , alpha);
    set_aet_id_alpha(PV_GAME_AET_SONG_ENERGY_BASE_REACH, alpha);

    field_33C++;
}

void pv_game_play_data::sub_140137F80(bool update, float_t duration) {
    if (update) {
        frame_set(true, 0, 3, duration * 60.0f);
        this->update = true;
    }
    else {
        frame_set(true, 0, 4, duration * 60.0f);
        this->update = false;
    }
}

float_t get_percentage_clear_excellent() {
    return percentage_clear_excellent[sub_14013C8C0()->difficulty];
}

float_t get_percentage_clear_great() {
    return percentage_clear_great[sub_14013C8C0()->difficulty];
}

static void sub_14013AAE0(float_t pos_x, float_t pos_y, spr::SprPrio prio,
    const char* str, bool h_center, color4u8 color, rectangle* clip_rect) {
    if (!str)
        return;

    font_info font(16);
    font.set_glyph_size(24.0f, 24.0f);

    PrintWork print_work;
    print_work.set_font(&font);
    print_work.set_position(pos_x, pos_y);
    print_work.set_resolution_mode(RESOLUTION_MODE_HD);
    print_work.set_prio(prio);
    print_work.set_color(color);

    app::text_flags flags = (app::text_flags)((h_center
        ? app::TEXT_FLAG_ALIGN_FLAG_H_CENTER
        : app::TEXT_FLAG_ALIGN_FLAG_LEFT)
        | app::TEXT_FLAG_FONT);

    if (clip_rect) {
        print_work.clip = true;
        print_work.clip_data = *clip_rect;
    }

    print_work.PrintText(flags, str);
}
