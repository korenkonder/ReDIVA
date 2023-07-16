/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game.hpp"
#include <iterator>
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/rob/motion.hpp"
#include "../../CRE/rob/skin_param.hpp"
#include "../../CRE/customize_item_table.hpp"
#include "../../CRE/hand_item.hpp"
#include "../../CRE/random.hpp"
#include "../../CRE/pv_expression.hpp"
#include "../../CRE/pv_param.hpp"
#include "../../CRE/task_effect.hpp"
#include "../../CRE/sound.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../game_state.hpp"
#include "../imgui_helper.hpp"
#include "../input.hpp"
#include "player_data.hpp"
#include "pv_game_camera.hpp"
#include "pv_game_music.hpp"
#include "target_pos.hpp"

class DivaPvFrameRate : public FrameRateControl {
public:
    DivaPvFrameRate();
    virtual ~DivaPvFrameRate() override;

    virtual float_t GetDeltaFrame() override;
};

struct pv_game_parent {
    int8_t field_0;
    int8_t field_1;
    uint8_t pv_state;
    bool playing;
    int8_t field_4;
    int8_t field_5;
    int8_t field_6;
    int8_t field_7;
    int64_t curr_time;
    float_t delta_time;
    int32_t state;
    void(*update_func)(pv_game_parent*);
    int8_t field_20;
    int8_t field_21;
    int8_t field_22;
    int8_t field_23;
    int8_t field_24;
    int8_t field_25;
    int8_t field_26;
    int8_t field_27;
    bool init_time;

    pv_game_parent();

    static void ctrl(pv_game_parent* pvgmp);
};

struct pv_game_time {
    int64_t curr_time;
    time_struct last_stop_time;
    bool add_last_stop_time;
    int64_t delta_time;
    time_struct current_time;
    bool add_current_time;

    pv_game_time();
};

struct pv_game_se_data {
    std::string se_name;
    std::string slide_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    std::string chainslide_failure_name;
    std::string slidertouch_name;

    pv_game_se_data();
    ~pv_game_se_data();
};

struct struc_674 {
    size_t performer;
    int32_t module;
    rob_chara_pv_data_item item;
    bool field_1C;
    const pv_db_pv* pv;

    struc_674();
};

struct struc_773 {
    int32_t field_0;
    bool field_4;
    int32_t field_8;
    bool field_C;
    int32_t field_10;
};

const int32_t life_gauge_table_array[PV_DIFFICULTY_MAX][HIT_MAX] = {
    { 3, 2, 1, -10,  0, -1,  -3, -15, -20, 3, 2, 1, -10, 3, 2, 1, -10, 3, 2, 1, -10 },
    { 2, 1, 0, -10,  0, -1,  -3, -15, -20, 2, 1, 0, -10, 2, 1, 0, -10, 2, 1, 0, -10 },
    { 2, 1, 0, -10, -3, -6,  -9, -15, -20, 2, 1, 0, -10, 2, 1, 0, -10, 2, 1, 0, -10 },
    { 2, 1, 0, -15, -6, -8, -10, -20, -25, 2, 1, 0, -15, 2, 1, 0, -15, 2, 1, 0, -15 },
    { 1, 1, 0, -15, -6, -8, -10, -20, -30, 1, 1, 0, -15, 1, 1, 0, -15, 1, 1, 0, -15 },
};

DivaPvFrameRate diva_pv_frame_rate;
SysFrameRate sys_frame_rate_array[3];
pv_game* pv_game_ptr;
pv_disp2d pv_disp2d_data;
TaskPvGame* task_pv_game;

#if PV_DEBUG
PVGameSelector* pv_game_selector_ptr;
#endif

struc_14 stru_140C94438;
struc_717 stru_141197E00;

pv_game_parent pv_game_parent_data;
pv_game_time pv_game_time_data;
int32_t pv_game_parent_state;

int32_t pv_game_state;

extern render_context* rctx_ptr;

static bool pv_game_parent_ctrl();
static void pv_game_parent_disp();

static void pv_game_time_pause();
static void pv_game_time_start();

static FrameRateControl* get_diva_pv_frame_rate();
static SysFrameRate* sys_frame_rate_get(int32_t chara_id);

static int32_t sub_1400FCFD0(pv_game_chara* arr, const pv_db_pv* pv);
static void sub_140105010(pv_game_chara* arr, size_t max_count, pv_game_item_mask* a3, struc_674& a4);

pv_game_chara::pv_game_chara() : chara_index(), cos(), chara_id(), pv_data(), rob_chr() {

}

pv_game_chara::~pv_game_chara() {

}

bool pv_game_chara::check_chara() {
    if (chara_id != -1)
        return !!rob_chr;
    return false;
}

void pv_game_chara::reset() {
    chara_index = CHARA_MAX;
    cos = -1;
    chara_id = -1;
    pv_data.reset();
    rob_chr = 0;
    motion_ids.clear();
    motion_face_ids.clear();
}

pv_game_edit_effect_data::pv_game_edit_effect_data() {
    index = -1;
    speed = 1.0f;
    time = 0.0f;
    end_frame = 0.0f;
    delta_time = 1.0f;
    loop = false;
}

void pv_game_edit_effect_data::reset() {
    index = -1;
    speed = 1.0f;
    time = 0.0f;
    end_frame = 0.0f;
    delta_time = 1.0f;
    loop = false;
}

pv_game_edit_effect::pv_game_edit_effect() {

}

pv_game_edit_effect::~pv_game_edit_effect() {

}

void pv_game_edit_effect::reset() {
    spr_set_ids.clear();
    aet_set_ids.clear();
    aet_ids.clear();
    data.reset();
}

void pv_game_edit_effect::unload() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    for (uint32_t& i : spr_set_ids)
        sprite_manager_unload_set(i, aft_spr_db);

    for (uint32_t& i : aet_set_ids)
        aet_manager_unload_set(i, aft_aet_db);

    reset();
}

pv_game_field::pv_game_field() : stage_index(), light_auth_3d_uid(), light_auth_3d_id(),
light_frame(), spr_set_back_id(), stage_flag(), npr_type(), cam_blur(), sdw_off() {

}

pv_game_field::~pv_game_field() {

}

struc_269::struc_269() : field_30(), field_34() {

}

struc_269::~struc_269() {

}

pv_game_chreff::pv_game_chreff() : src_chara_index(),
dst_chara_index(), src_auth_3d_uid(), dst_auth_3d_uid() {

}

pv_game_chreff::~pv_game_chreff() {

}

pv_effect_resource::pv_effect_resource() : emission() {

}

pv_effect_resource::~pv_effect_resource() {

}

pv_game_data::pv_game_data() : field_2CE98(), field_2CE9C(), field_2CF1C(), field_2CF20(), appear_state(),
appear_time(), appear_duration(), field_2CF30(), field_2CF34(), field_2CF38(), life_gauge(), score_final(),
challenge_time_total_bonus(), combo(), challenge_time_combo_count(), max_combo(), total_hit_count(), hit_count(),
current_reference_score(), target_count(), field_2CF84(), field_2CF88(), field_2CF8C(), score_slide_chain_bonus(),
slide_chain_length(), field_2CF98(), field_2CF9C(), field_2CFA0(), field_2CFA4(), pv(),field_2CFB0(),
chance_point(), reference_score(), reference_score_no_flag(), reference_score_no_flag_life_bonus(),
field_2CFE0(), field_2CFE4(), notes_passed(), field_2CFF0(), song_energy(), song_energy_base(),
song_energy_border(), life_gauge_safety_time(), life_gauge_border(), stage_index(), music_play(),
no_fail(), changed_field(), challenge_time_start(), challenge_time_end(), max_time(), max_time_float(),
current_time_float(), current_time(), field_2D038(), field_2D03C(), score_hold_multi(),
score_hold(), score_slide(), has_slide(), has_success_se(), pv_disp2d(), life_gauge_final(),
hit_border(), field_2D05D(), field_2D05E(), start_fade(), title_image_init(), mute(),
ex_stage(), play_success(), has_auth_3d_frame(), has_light_frame(), has_aet_frame(), has_aet_list(),
field_index(), edit_effect_index(), slidertouch_counter(), change_field_branch_success_counter(),
field_2D080(), field_2D084(), life_gauge_bonus(), life_gauge_total_bonus(), field_2D090(), no_clear(),
disp_lyrics_now(), field_2D093(), field_2D094(), field_2D095(), field_2D096(), success(), title_image_state(),
field_2D09C(), use_osage_play_data(), pv_end_fadeout(), rival_percentage(), field_2D0A8(), field_2D0AC(),
field_2D0B0(), field_2D0BC(), next_stage(), has_frame_texture(), field_2D0BF(), field_2D0C0(), field_2D0C4(),
field_2D7E8(), field_2D7EC(), field_2D808(), field_2D80C(), edit_effect(), camera_auth_3d_uid(),
se_index(), task_effect_init(), field_2D87C(), current_field(), field_2D8A0(), field_2D8A4(),
campv_index(), field_2D954(), field_2DAC8(), field_2DACC(), height_adjust(), field_2DB2C(), field_2DB34() {
    disp_lyrics = true;
}

pv_game_data::~pv_game_data() {

}

void pv_game_data::calculate_target_start_pos(vec2& pos, float_t angle, float_t distance, vec2& start_pos) {
    angle = ((angle >= 0.0f ? 180.0f : -180.0f) - angle) * DEG_TO_RAD_FLOAT;
    start_pos.x = sinf(angle) * distance + pos.x;
    start_pos.y = cosf(angle) * distance + pos.y;
}

int64_t pv_game_data::get_max_time(int64_t* challenge_time_start, int64_t* challenge_time_end) {
    if (!challenge_time_start || !challenge_time_end)
        return 0;

    *challenge_time_start = -1;
    *challenge_time_end = -1;

    int64_t dsc_time = 0;
    int64_t max_time = 0;
    while (pv_data.dsc_data_ptr != pv_data.dsc_data_ptr_end) {
        int32_t func = pv_data.dsc_data_ptr->func;
        uint32_t* data = pv_data.dsc.get_func_data(pv_data.dsc_data_ptr);

        if (func < 0 || func >= DSC_FT_MAX)
            goto END;

        switch (func) {
        case DSC_FT_END:
        case DSC_FT_PV_END:{
            max_time = dsc_time;
        } goto END;
        case DSC_FT_TIME: {
            dsc_time = 10000LL * (int32_t)data[0];
        } break;
        case DSC_FT_MODE_SELECT:
        case DSC_FT_EDIT_MODE_SELECT: {
            bool v11 = true;
            int32_t mode;
            if (func == DSC_FT_MODE_SELECT && pv_data.has_perf_id && sub_14013C8C0()->sub_1400E7910() < 4) {
                int32_t diff_bits = (int32_t)data[0];
                mode = (int32_t)data[1];
                v11 = !!(diff_bits & (1 << sub_14013C8C0()->difficulty));
            }
            else
                mode = (int32_t)data[0];

            if (v11 && (sub_14013C8C0()->difficulty == PV_DIFFICULTY_EASY
                || sub_14013C8C0()->difficulty == PV_DIFFICULTY_NORMAL))
                if (mode == 1)
                    *challenge_time_start = dsc_time;
                else if (mode == 3)
                    *challenge_time_end = dsc_time;
        } break;
        }

        pv_data.dsc_data_ptr++;
    }

END:
    pv_data.targets.clear();
    pv_data.targets.shrink_to_fit();
    pv_data.target_index = 0;
    return max_time;
}

void pv_game_data::reset() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    for (pv_play_data& i : pv_data.playdata) {
        rob_chara* rob_chr = i.rob_chr;
        if (rob_chr) {
            rob_chr->reset_data(&rob_chr->pv_data, aft_bone_data, aft_mot_db);
            rob_chr->set_hand_l_mottbl_motion(0, 194, 1.0, 0, 0.0, 0.0, 1.0, -1, 0.0, aft_mot_db);
            rob_chr->set_hand_r_mottbl_motion(0, 194, 1.0, 0, 0.0, 0.0, 1.0, -1, 0.0, aft_mot_db);
            rob_chr->set_visibility(true);
        }

        i.motion_data.start_pos = 0.0f;
        i.motion_data.end_pos = 0.0f;
        i.motion_data.start_rot = 0.0f;
        i.motion_data.end_rot = 0.0f;

        i.motion_data.rob_chr = 0;
        i.motion_data.current_time = 0.0f;
        i.motion_data.duration = 0.0f;
        i.set_motion.clear();
    }

    pv_data.dsc_reset_position();

    pv_data.curr_time = 0;
    pv_data.curr_time_float = 0.0f;
    pv_data.prev_time_float = 0.0f;
    pv_data.field_2BF30 = -1;
    pv_data.field_2BF38 = 0;
    pv_data.music_playing = false;
    pv_data.pv_game->data.play_data.lyric_color = 0xFFFFFFFF;
    pv_data.play = true;
    pv_data.field_2BF50 = 0;
    pv_data.field_2BFD5 = false;
    sub_14013C8C0()->field_26 = false;
    pv_data.field_2BF58 = -1;
    pv_data.field_2BFC4 = 2;

    pv_game_camera_reset();

    pv_data.fov = 32.2673416137695f;
    pv_data.min_dist = 0.05f;
    pv_data.scene_fade.reset();
    pv_data.pv_game->data_itmpv_disable();
    pv_data.reset_camera_post_process();

    pv_data.scene_rot_y = 0.0f;
    pv_data.scene_rot_mat = mat4_identity;
}

#pragma warning(push)
#pragma warning(disable: 6011)
#pragma warning(disable: 6385)
#pragma warning(disable: 28182)
bool pv_game_data::sub_140119960(bool* has_target, int64_t* dsc_time, int32_t* target_count,
    int64_t* target_flying_time, float_t fade_time, bool* start_fade, dsc_target_ft* target_hist,
    int32_t* target_flag, bool* has_chance, bool* has_slide, int32_t* slide_chain_type,
    bool* slide_chain_start, int32_t* slide_chain_start_count, int32_t* slide_chain_cont_count,
    int32_t* slide_chain_no_end_count, bool* has_slide_chain, int64_t target_flying_time_hist) {
    if (!has_target || !dsc_time || !target_count || !target_flying_time || !start_fade || !target_hist
        || !target_flag || !has_chance || !has_slide || !slide_chain_type)
        return true;

    *has_target = false;
    *target_flying_time = -1;
    *has_chance = false;
    *has_slide = false;
    *slide_chain_type = 0;

    bool end = false;
    int32_t fade_time_int = (int32_t)(fade_time * 1000000000.0f);

    uint32_t* target_data_offset[4] = {};

    int32_t _target_count = 0;
    while (pv_data.dsc_data_ptr != pv_data.dsc_data_ptr_end) {
        int32_t func = pv_data.dsc_data_ptr->func;
        uint32_t* data = pv_data.dsc.get_func_data(pv_data.dsc_data_ptr);

        if (func < 0 || func >= DSC_FT_MAX)
            goto END;

        switch (func) {
        case DSC_FT_END:
        case DSC_FT_PV_END:
            end = true;
            pv_data.dsc_data_ptr++;
            goto END;
        case DSC_FT_TIME: {
            if (_target_count > 0) {
                pv_data.dsc_data_ptr++;
                goto PROCESS_TARGET;
            }

            *dsc_time = 10000LL * (int32_t)data[0];
        } break;
        case DSC_FT_TARGET: {
            *has_target = true;
            target_data_offset[_target_count++] = data;

            int32_t target = (int32_t)data[0];

            switch (target) {
            case DSC_TARGET_FT_0B:
            case DSC_TARGET_FT_SLIDE_L:
            case DSC_TARGET_FT_SLIDE_R:
            case DSC_TARGET_FT_GREEN_SQUARE:
            case DSC_TARGET_FT_16:
            case DSC_TARGET_FT_SLIDE_L_CHANCE:
            case DSC_TARGET_FT_SLIDE_R_CHANCE:
                *has_slide = true;
                break;
            case DSC_TARGET_FT_0E:
                *slide_chain_type = 1;
                break;
            case DSC_TARGET_FT_SLIDE_CHAIN_L:
                *slide_chain_type = 2;
                break;
            case DSC_TARGET_FT_SLIDE_CHAIN_R:
                *slide_chain_type = 3;
                break;
            }

            switch (target) {
            case DSC_TARGET_FT_TRIANGLE_CHANCE:
            case DSC_TARGET_FT_CIRCLE_CHANCE:
            case DSC_TARGET_FT_CROSS_CHANCE:
            case DSC_TARGET_FT_SQUARE_CHANCE:
            case DSC_TARGET_FT_SLIDE_L_CHANCE:
            case DSC_TARGET_FT_SLIDE_R_CHANCE:
                *has_chance = true;
                break;
            }

            if (*dsc_time + *target_flying_time - 129999995 < fade_time_int)
                *start_fade = false;
        } break;
        case DSC_FT_LYRIC: {
            int32_t lyric_index = (int32_t)data[0];
            if (lyric_index > 0 && *dsc_time < fade_time_int)
                *start_fade = false;
        } break;
        case DSC_FT_BAR_TIME_SET: {
            int32_t bpm = (int32_t)data[0];
            int32_t time_signature = (int32_t)data[1] + 1;
            bar_time_set_to_target_flying_time(bpm, time_signature, target_flying_time);
        } break;
        case DSC_FT_TARGET_FLYING_TIME: {
            int32_t _target_flying_time = (int32_t)data[0];
            if (target_flying_time >= 0)
                *target_flying_time = 1000000LL * _target_flying_time;
        } break;
        case DSC_FT_TARGET_FLAG: {
            *target_flag = data[0];
        } break;
        }

        pv_data.dsc_data_ptr++;
    }

END:
    if (_target_count <= 0) {
        *target_count = _target_count;
        return end;
    }

PROCESS_TARGET:
    int32_t random_mask = 0x0F;
    int32_t random_count = 4;
    int32_t max_random_count = 0;
    for (int32_t i = 0; i < _target_count; i++) {
        dsc_target_ft type = (dsc_target_ft)target_data_offset[i][0];
        if (type < DSC_TARGET_FT_RANDOM || type > DSC_TARGET_FT_RANDOM_HOLD) {
            if (type >= DSC_TARGET_FT_TRIANGLE_HOLD && type <= DSC_TARGET_FT_SQUARE_HOLD)
                type = (dsc_target_ft)(type - 4);
            max_random_count--;
            random_mask &= ~(1 << type);
        }
        else
            random_count++;
    }

    if (random_count > 0) {
        int32_t v41 = 0;
        int32_t v42 = max_random_count - 1;
        for (int32_t i = 0; i < _target_count; i++) {
            if (!target_data_offset[i])
                continue;

            dsc_ft_func v44 = (dsc_ft_func)target_data_offset[i][0];
            if ((unsigned int)(v44 - 8) > 1)
                continue;

            int32_t v45 = rand_state_array_get_int(0, v42, 0);
            int32_t v46 = 1;
            int32_t v47 = 0;
            int32_t v48 = 0;
            for (; random_mask & (1 << v48) && v48 < 4; v46 <<= 1, v48++) {
                if (v47 == v45) {
                    v42--;
                    random_mask &= ~v46;
                }

                v47++;
            }

            if (v48 == 4)
                v48 = 0;
            if (v44 != DSC_TARGET_FT_RANDOM)
                v48 += 4;

            target_data_offset[i][0] = v48;
        }
    }

    for (int32_t i = 0; i < _target_count; i++)
        if (target_data_offset[i][0] == DSC_TARGET_FT_PREVIOUS)
            target_data_offset[i][0] = *target_hist != DSC_TARGET_FT_MAX ? *target_hist : DSC_TARGET_FT_CIRCLE;

    dsc_target_ft type = (dsc_target_ft)target_data_offset[_target_count - 1][0];
    if (type < DSC_TARGET_FT_RANDOM || type > DSC_TARGET_FT_PREVIOUS)
        *target_hist = type;
    else
        *target_hist = DSC_TARGET_FT_CIRCLE;

    if (*target_flying_time >= 0)
        target_flying_time_hist = *target_flying_time;

    pv_dsc_target_group next_target;
    next_target.time_begin = *dsc_time;
    next_target.time_end = *dsc_time + target_flying_time_hist;

    for (int32_t i = 0; i < _target_count; ) {
        uint32_t* data = target_data_offset[i];

        dsc_target_ft type = (dsc_target_ft)data[0];
        float_t pos_x = (float_t)(int32_t)data[1] * 0.001f;
        float_t pos_y = (float_t)(int32_t)data[2] * 0.001f;
        float_t angle = (float_t)(int32_t)data[3] * 0.001f;
        float_t distance = (float_t)(int32_t)data[4] * 0.001f;
        float_t amplitude = (float_t)(int32_t)data[5] * 0.001f;
        int32_t frequency = (int32_t)data[6];

        int32_t high_speed_rate = 1;
        if (pv_data.pv_game->data.play_data.option == 1) {
            distance *= (float_t)pv_data.pv_game->get_pv_high_speed_rate();
            high_speed_rate = pv_data.pv_game->get_pv_high_speed_rate();
        }

        next_target.targets[i].type = type;
        next_target.targets[i].pos.x = pos_x;
        next_target.targets[i].pos.y = pos_y;
        calculate_target_start_pos(next_target.targets[i].pos,
            angle, distance, next_target.targets[i].start_pos);
        next_target.targets[i].amplitude = amplitude;
        next_target.targets[i].frequency = high_speed_rate * frequency;
        next_target.target_count = ++i;
    }

    next_target.slide_chain = *slide_chain_type > 0;

    for (int32_t i = 0; i < next_target.target_count; i++) {
        pv_dsc_target& target = next_target.targets[i];
        target.slide_chain_start = false;
        target.slide_chain_end = false;
        target.slide_chain_left = false;
        target.slide_chain_right = false;
        target.slide_chain = next_target.slide_chain;
    }

    if (next_target.target_count > 0) {
        bool has_other = false;
        bool has_no_slide_chain = true;
        for (int32_t i = 0; i < next_target.target_count; i++) {
            dsc_target_ft type = next_target.targets[i].type;
            if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
                has_no_slide_chain = false;
            else
                has_other = true;
        }

        if (has_other && has_no_slide_chain)
            *has_slide_chain = true;
    }

    if ((*has_slide || *slide_chain_type > 0) && next_target.target_count > 1) {
        int32_t left_index = 0;
        int32_t right_index = 0;
        float_t pos_left = next_target.targets[0].pos.x;
        float_t pos_right = next_target.targets[0].pos.x;

        for (int32_t i = 1; i < next_target.target_count; i++) {
            if (pos_left > next_target.targets[i].pos.x) {
                left_index = i;
                pos_left = next_target.targets[i].pos.x;
            }

            if (pos_right < next_target.targets[i].pos.x) {
                right_index = i;
                pos_right = next_target.targets[i].pos.x;
            }
        }

        if (left_index != right_index) {
            next_target.targets[left_index].slide_chain_left = true;
            next_target.targets[right_index].slide_chain_right = true;
        }
    }

    size_t pv_target_count = pv_data.targets.size();

    bool slide_chain_end = false;
    if (*slide_chain_type > 0) {
        pv_dsc_target_group prev_target_1;
        pv_dsc_target_group prev_target_2;

        bool has_prev_target_1 = false;
        bool prev_target_1_has_same_slide_chain = false;

        if (pv_target_count > 0) {
            has_prev_target_1 = true;
            prev_target_1 = pv_data.targets[pv_target_count - 1];
            for (int32_t i = 0; i < next_target.target_count; i++) {
                dsc_target_ft type = next_target.targets[i].type;
                if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
                    if (type != prev_target_1.targets[i].type) {
                        prev_target_1_has_same_slide_chain = false;
                        break;
                    }
                    else
                        prev_target_1_has_same_slide_chain = true;
            }
        }

        bool has_prev_target_2 = false;
        bool prev_target_2_has_same_slide_chain = false;

        if (pv_target_count > 1) {
            has_prev_target_2 = true;
            prev_target_2 = pv_data.targets[pv_target_count - 2];
            for (int32_t i = 0; i < next_target.target_count; i++) {
                dsc_target_ft type = next_target.targets[i].type;
                if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
                    if (type != prev_target_2.targets[i].type) {
                        prev_target_2_has_same_slide_chain = false;
                        break;
                    }
                    else
                        prev_target_2_has_same_slide_chain = true;
            }
        }

        int32_t next_target_slide_chain_count = 0;
        for (int32_t i = 0; i < next_target.target_count; i++) {
            dsc_target_ft type = next_target.targets[i].type;
            if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
                next_target_slide_chain_count++;
        }

        bool prev_target_1_has_start = false;
        int32_t prev_target_1_slide_chain_count = 0;
        if (has_prev_target_1)
            for (int32_t i = 0; i < prev_target_1.target_count; i++) {
                dsc_target_ft type = prev_target_1.targets[i].type;
                if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R) {
                    if (prev_target_1.targets[i].slide_chain_start)
                        prev_target_1_has_start = true;
                    prev_target_1_slide_chain_count++;
                }
            }

        int32_t prev_target_2_slide_chain_count = 0;
        if (has_prev_target_2)
            for (int32_t i = 0; i < prev_target_2.target_count; i++) {
                dsc_target_ft type = prev_target_2.targets[i].type;
                if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
                    prev_target_2_slide_chain_count++;
            }

        float_t v139;
        float_t v140;
        if (has_prev_target_1
            && prev_target_1_has_same_slide_chain
            && next_target_slide_chain_count == prev_target_1_slide_chain_count
            && (!has_prev_target_2
                || !prev_target_2_has_same_slide_chain
                || next_target_slide_chain_count != prev_target_2_slide_chain_count
                || prev_target_1_has_start
                || (v139 = (float_t)(next_target.time_begin - prev_target_1.time_begin) * 0.000001f,
                     v140 = (float_t)(prev_target_1.time_begin - prev_target_2.time_begin) * 0.000001f,
                     (v139 >= (v140 * 0.5f) && v139 <= (v140 * 1.5f)))))
            slide_chain_end = false;
        else {
            slide_chain_end = true;
            for (int32_t i = 0; i < next_target.target_count; i++) {
                dsc_target_ft type = next_target.targets[i].type;
                if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
                    next_target.targets[i].slide_chain_start = true;
            }
        }
    }
    else
        slide_chain_end = true;

    if (pv_target_count && slide_chain_end) {
        pv_dsc_target_group prev_target = pv_data.targets[pv_target_count - 1];
        for (int32_t i = 0; i < prev_target.target_count; i++) {
            dsc_target_ft type = prev_target.targets[i].type;
            if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R) {
                if (!prev_target.targets[i].slide_chain_end)
                    (*slide_chain_no_end_count)++;

                pv_data.targets[pv_target_count - 1].targets[i].slide_chain_end = true;
            }
        }
    }

    *slide_chain_start = false;
    for (int32_t i = 0; i < next_target.target_count; i++) {
        dsc_target_ft type = next_target.targets[i].type;
        if (type >= DSC_TARGET_FT_0E && type <= DSC_TARGET_FT_SLIDE_CHAIN_R)
            if (next_target.targets[i].slide_chain_start) {
                *slide_chain_start = true;
                (*slide_chain_start_count)++;
            }
            else
                (*slide_chain_cont_count)++;
    }

    pv_data.targets.push_back(next_target);
    *target_count = _target_count;
    return end;
}
#pragma warning(pop)

pv_game_item::pv_game_item() {
    arr[0] = -1;
    arr[1] = -1;
    arr[2] = -1;
    arr[3] = -1;
}

pv_game_item_mask::pv_game_item_mask() {
    arr[0] = true;
    arr[1] = true;
    arr[2] = true;
    arr[3] = true;
}

pv_game::pv_game() : loaded(), field_1(), field_2(), end_pv(),
field_4(), state(), field_C(), pv_id(), field_14(), modules(), items() {
    reset();

    state = 0;
    field_C = -1;
    pv_id = -1;

    for (int32_t& i : modules)
        i = 0;

    data.play_data.field_5EC = 0;
    data.play_data.option = 0;

    data.field_2CF98 = 0;
    data.song_energy_base = 0.0f;
    data.life_gauge_safety_time = 0;
    data.life_gauge_border = 0;
    data.stage_index = 0;
    data.music_play = true;
    data.no_fail = false;
    data.challenge_time_start = -1;
    data.challenge_time_end = -1;
    data.mute = false;
    data.ex_stage = false;
    data.play_success = false;
    data.field_2D0BC = false;
    data.next_stage = false;
    data.has_frame_texture = false;
    data.field_2D0C0 = -1;
    data.no_fail = true;
    data.task_effect_init = false;

    data.effect_rs_list_hashes.reserve(0x40);
    data.effect_rs_list.reserve(0x40);
}

pv_game::~pv_game() {

}

void pv_game::calculate_life_gauge(hit_state hit_state, bool has_safety,
    bool challenge_time, bool slide, bool slide_chain, bool slide_chain_start, bool a8) {
    if (sub_14013C8C0()->sub_1400E7910() >= 6 || challenge_time)
        return;

    int32_t score = 0;
    if (!slide_chain || slide_chain_start || !a8)
        score = life_gauge_table_array[sub_14013C8C0()->difficulty][hit_state];

    if (!data.life_gauge && data.no_fail)
        score = 0;

    data.life_gauge += score;

    data.life_gauge = clamp_def(data.life_gauge, 0, 255);
    data.life_gauge_final = data.life_gauge;

    if (has_safety) {
        int32_t border = calculate_life_gauge_border();
        if (data.life_gauge <= border) {
            data.life_gauge = border;
            data.hit_border = true;
        }
    }
}

int32_t pv_game::calculate_life_gauge_border() {
    return 255 * data.life_gauge_border / 100;
}

int32_t pv_game::calculate_reference_score() {
    data.challenge_time_start = -1;
    data.challenge_time_end = -1;

    const int32_t life_gauge_cool = life_gauge_table_array[sub_14013C8C0()->difficulty][0];

    data.max_time = data.get_max_time(&data.challenge_time_start, &data.challenge_time_end);
    data.max_time_float = (float_t)data.max_time * 0.000000001f;

    data.pv_data.dsc_reset_position();
    data.target_reference_score.push_back(0);

    data.field_2CFE0 = 0;
    data.field_2CFF0 = 0;

    bool has_slide = false;
    bool has_success_se = false;

    int32_t reference_score = 0;
    int32_t reference_score_no_flag = 0;
    int32_t life_bonus = 0;

    int64_t dsc_time = 0;
    bool start_fade = true;
    dsc_target_ft target_hist = DSC_TARGET_FT_MAX;
    bool has_chance = false;
    int32_t slide_chain_start_count = 0;
    int32_t slide_chain_cont_count = 0;
    int32_t slide_chain_no_end_count = 0;
    int64_t target_flying_time_hist = -1;

    int32_t target = 0;
    int32_t life_gauge = 127;
    int32_t target_index = 0;
    int32_t target_index_no_flag = 0;
    int32_t target_index_chance = 0;
    int32_t target_index_chance_no_flag = 0;
    bool end;
    do {
        int32_t target_count = 0;
        int32_t target_flag = 0;
        int32_t slide_chain_type = 0;
        bool has_target = false;
        int64_t target_flying_time = -1;
        bool _has_slide = false;
        bool slide_chain_start = false;
        bool has_slide_chain = false;
        end = data.sub_140119960(&has_target, &dsc_time, &target_count, &target_flying_time, 3.0f,
            &start_fade, &target_hist, &target_flag, &has_chance, &_has_slide, &slide_chain_type,
            &slide_chain_start, &slide_chain_start_count, &slide_chain_cont_count,
            &slide_chain_no_end_count, &has_slide_chain, target_flying_time_hist);
        if (target_flying_time >= 0)
            target_flying_time_hist = target_flying_time;

        if (has_target) {
            target_count = max_def(target_count, 1);
            bool slide_chain = slide_chain_type > 0 && !slide_chain_start && has_slide_chain;

            calculate_target_reference_score(&reference_score,
                &target_index, target_count, slide_chain);
            calculate_target_reference_score(&reference_score_no_flag,
                &target_index_no_flag, target_count, slide_chain);

            target_flying_time_hist = max_def(target_flying_time_hist, 0);

            if (target_flying_time_hist + dsc_time >= data.challenge_time_start
                && target_flying_time_hist + dsc_time <= data.challenge_time_end) {
                if (!slide_chain) {
                    if (!(target_flag & 1))
                        calculate_target_reference_combo_bonus(
                            &reference_score, &target_index_chance);
                    calculate_target_reference_combo_bonus(
                        &reference_score_no_flag, &target_index_chance_no_flag);
                    data.field_2CFF0++;
                }

                if (data.pv_data.first_challenge_note < 0)
                    data.pv_data.first_challenge_note = target;
                data.pv_data.last_challenge_note = target;
            }
            else {
                if (life_gauge == 255)
                    life_bonus += 10;
                else {
                    if (slide_chain_type <= 0 || slide_chain_start)
                        life_gauge += life_gauge_cool;
                    life_gauge = min_def(life_gauge, 255);
                }
            }

            data.target_reference_score.push_back(reference_score);

            if (slide_chain_type > 0 && !slide_chain_start)
                data.field_2CFE0++;
            target++;
        }

        if (_has_slide || slide_chain_type > 0)
            has_slide = true;

        if (has_chance)
            has_success_se = true;
    } while (!end);

    if (has_slide)
        data.has_slide = true;

    if (has_success_se)
        data.has_success_se = true;

    data.pv_data.dsc_reset_position();

    data.start_fade = start_fade;
    data.notes_passed = 0;
    data.reference_score_no_flag = reference_score_no_flag;
    data.reference_score_no_flag_life_bonus = reference_score_no_flag + life_bonus;
    return reference_score;
}

void pv_game::calculate_score(int32_t score, bool continue_combo, int64_t& time,
    vec2& pos, int32_t multi_count, hit_state hit_state, bool challenge_time,
    bool slide, bool slide_chain, bool slide_chain_start, bool slide_chain_end, bool slide_chain_continues) {
    data.score_final += score;

    if (multi_count > 1)
        data.score_hold_multi += score;

    if (slide || slide_chain)
        data.score_slide += score;

    data.life_gauge_bonus = 0;

    if (!challenge_time) {
        if (data.life_gauge == 255 && hit_state <= HIT_COOL_QUADRIPLE && (0x22201 & (1 << hit_state)))
            data.life_gauge_bonus = 10;

        data.life_gauge_total_bonus += data.life_gauge_bonus;
        data.score_final += data.life_gauge_bonus;
    }

    bool _slide_chain = false;
    if (slide_chain && !slide_chain_start && slide_chain_continues)
        _slide_chain = true;
    else if (continue_combo) {
        data.combo = min_def(data.combo + 1, 9999);
        data.max_combo = max_def(data.max_combo, data.combo);
        data.score_final += 50 * min_def(data.combo / 10, 5);
    }
    else
        data.combo = 0;

    calculate_target_reference_score(&data.current_reference_score,
        &data.target_count, multi_count, _slide_chain);

    if (continue_combo && slide_chain && !slide_chain_start) {
        data.slide_chain_length = min_def(data.slide_chain_length + 1, 50);

        int32_t slide_points = 10 * data.slide_chain_length;
        if (slide_chain_end)
            slide_points += 1000;

        data.score_final += slide_points;
        data.score_slide_chain_bonus += slide_points;
        data.score_slide += slide_points;

        if (slide_points > 0)
            data.play_data.disp_slide_points(slide_points, pos.x, pos.y - -40.0f, slide_chain_end);
    }
    else
        data.slide_chain_length = 0;

    if (challenge_time && (!slide_chain || slide_chain_start || !slide_chain_continues)) {
        if (continue_combo)
            data.challenge_time_combo_count = min_def(data.challenge_time_combo_count + 1, 50);
        else
            data.challenge_time_combo_count = 0;

        int32_t chance_point = 10 * data.challenge_time_combo_count;
        data.score_final += chance_point;
        data.challenge_time_total_bonus += chance_point;

        if (chance_point) {
            data.play_data.disp_chance_points(chance_point, pos.x, pos.y);
            data.field_2CFB0 = 30;
            data.chance_point = chance_point;
        }

        calculate_target_reference_combo_bonus(&data.current_reference_score, &data.field_2CF84);
        data.field_2CF88++;
    }
}

void pv_game::calculate_song_energy() {
    if (data.reference_score <= 0) {
        data.song_energy = 0.0f;
        return;
    }

    static const float_t score_hold_mult[] = {
        400.0f, 200.0f, 50.0f, 20.0f, 0.0f
    };

    int32_t score = data.score_final - data.score_slide_chain_bonus - data.score_hold;
    score = max_def(score, 0);

    float_t song_energy;
    if (score == data.reference_score)
        song_energy = 100.0f;
    else
        song_energy = ((float_t)score * 100.0f) / (float_t)data.reference_score;

    float_t song_energy_hold = ((float_t)data.score_hold
        * score_hold_mult[sub_14013C8C0()->difficulty]) / (float_t)data.reference_score;
    data.song_energy = song_energy + min_def(song_energy_hold, 5.0f);
}

void pv_game::calculate_song_energy_border() {
    if (data.reference_score <= 0 || data.notes_passed >= data.target_reference_score.size()) {
        data.song_energy_border = 0.0f;
        return;
    }

    int32_t reference_score = data.target_reference_score[data.notes_passed];
    reference_score = min_def(reference_score, data.reference_score);

    if (reference_score != data.reference_score) {
        float_t song_energy_border = (float_t)reference_score
            * data.song_energy_base / (float_t)data.reference_score;
        if (song_energy_border <= data.song_energy_base)
            data.song_energy_border = song_energy_border;
        else
            data.song_energy_border = data.song_energy_base;
    }
    else
        data.song_energy_border = data.song_energy_base;
}

void pv_game::calculate_target_reference_score(int32_t* reference_score,
    int32_t* target_index, int32_t multi_count, bool slide_chain) {
    if (!reference_score || !target_index || multi_count <= 0)
        return;

    int32_t score;
    switch (multi_count) {
    case 1:
    default:
        score = 500;
        break;
    case 2:
        score = 1000;
        break;
    case 3:
        score = 1500;
        break;
    case 4:
        score = 2000;
        break;
    }
    *reference_score += score;

    if (!slide_chain) {
        int32_t tgt_idx = ++(*target_index) / 10;
        *reference_score += 50 * min_def(tgt_idx, 5);
    }
}

void pv_game::calculate_target_reference_combo_bonus(int32_t* reference_score, int32_t* target_index) {
    if (!reference_score || !target_index)
        return;

    int32_t tgt_idx = ++(*target_index);
    tgt_idx = min_def(tgt_idx, 5);
    *target_index = tgt_idx;
    *reference_score += 10 * tgt_idx;
}

void pv_game::change_field(size_t field, ssize_t dsc_time, ssize_t curr_time) {
    if (!field || field >= data.field_data.size())
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

    if (!data.task_effect_init && (dsc_time > -1 || curr_time > -1)) {
        rand_state_array_4_set_seed_1393939();
        task_effect_parent_reset();
        data.task_effect_init = true;
    }

    pv_game_field& curr_field_data = data.field_data[data.current_field];
    pv_game_field& next_field_data = data.field_data[field];

    task_stage_info stg_info = get_stage_info(next_field_data.stage_index);
    stg_info.set_stage();
    if (stg_info.check())
        stg_info.set_stage_display(next_field_data.stage_flag, true);
    else
        task_stage_current_set_stage_display(false, true);

    light_param_data_storage_data_set_pv_cut((int32_t)field);
    rctx_ptr->render_manager.set_npr_param(next_field_data.npr_type);
    //rctx_ptr->post_process.set_cam_blur(next_field_data.cam_blur);

    if (next_field_data.sdw_off)
        rctx_ptr->render_manager.set_shadow_false();
    else
        rctx_ptr->render_manager.set_shadow_true();

    bool disp_front = true;
    bool disp_front_low = true;

    if (sub_14013C8C0()->sub_1400E7910() == 3) {
        pv_disp2d* pv_disp2d = data.pv_disp2d;
        if (pv_disp2d) {
            if (pv_disp2d->title_start_2d_field >= 0 && pv_disp2d->title_end_2d_field >= 0
                && field >= pv_disp2d->title_start_2d_field)
                disp_front = field > pv_disp2d->title_end_2d_field;

            if (pv_disp2d->title_start_2d_low_field >= 0 && pv_disp2d->title_end_2d_low_field >= 0
                && field >= pv_disp2d->title_start_2d_low_field)
                disp_front_low = field > pv_disp2d->title_end_2d_low_field;
        }
    }

    bool not_init[4];
    for (int32_t i = 0; i < PV_AET_MAX; i++) {
        not_init[i] = false;

        pv_aet aet = (pv_aet)i;

        bool disp_aet = true;
        switch (aet) {
        case PV_AET_FRONT:
            disp_aet = disp_front;
            break;
        case PV_AET_FRONT_LOW:
            disp_aet = disp_front_low;
            break;
        case PV_AET_FRONT_3D_SURF:
        case PV_AET_BACK:
        default:
            disp_aet = true;
            break;
        }

        std::vector<float_t> v109;

        int32_t index = 0;
        for (const std::string& j : curr_field_data.aet_names[i]) {
            if (data.has_aet_list[i]) {
                float_t v26 = data.play_data.get_aet_frame(aet, index);
                v109.push_back(max_def(v26, 0.0f));
                data.play_data.free_aet(aet, index);
            }
            else {
                std::pair<std::string, std::string> aet_name_id;
                aet_name_id.second.assign(j);
                if (index < curr_field_data.aet_ids[i].size())
                    aet_name_id.first.assign(curr_field_data.aet_ids[i][index]);

                if (get_field_aet_index_by_name_id(next_field_data, aet, aet_name_id) < 0)
                    data.play_data.free_aet(aet, index);
            }

            index++;
        }

        index = 0;
        for (const std::string& j : next_field_data.aet_names[i]) {
            bool disp = disp_aet;
            if (sub_14013C8C0()->sub_1400E7910() == 3) {
                pv_disp2d* pv_disp2d = data.pv_disp2d;
                if (pv_disp2d && !pv_disp2d->title_2d_layer.compare(j))
                    disp = false;
            }

            std::pair<std::string, std::string> aet_name_id;
            aet_name_id.second.assign(j);
            if (index < next_field_data.aet_ids[i].size())
                aet_name_id.first.assign(next_field_data.aet_ids[i][index]);

            if (!aet_name_id.second.size()) {
                index++;
                continue;
            }

            int32_t aet_index = get_field_aet_index_by_name_id(curr_field_data, aet, aet_name_id);

            if (aet_index >= 0) {
                float_t end_time = -1.0f;
                float_t start_time = -1.0f;
#if PV_DEBUG
                FrameRateControl* frame_rate_control = &sys_frame_rate;
#else
                FrameRateControl* frame_rate_control = 0;
#endif

                if (data.has_aet_frame[i]) {
                    int32_t aet_frame = get_field_aet_frame_by_name_id(next_field_data, aet, aet_name_id);
                    start_time = get_aet_frame_max_frame(get_frame_continue(aet_frame),
                        aet, aet_name_id, dsc_time, curr_time, &end_time);
                    frame_rate_control = get_diva_pv_frame_rate();
                }

                if (!data.has_aet_frame[i] && !data.has_aet_list[i]
                    || (get_aet_frame_max_frame_by_name(start_time, end_time, aet_name_id), !data.has_aet_list[i])) {
                    if (data.has_aet_frame[i]) {
                        if (start_time >= 0.0f)
                            data.play_data.set_aet_frame(aet, index, start_time);
                        if (end_time < 0.0f) {
                            uint32_t aet_id = data.pv_disp2d->pv_aet_id;
                            if (aet_name_id.first.size())
                                aet_id = aft_aet_db->get_aet_by_name(aet_name_id.first.c_str())->id;
                            end_time = aet_manager_get_scene_layer_end_time(aet_id,
                                aet_name_id.second.c_str(), aft_aet_db);
                        }
                        data.play_data.set_aet_end_time(aet, index, end_time);
                    }
                }
                else {
                    float_t v64 = 0.0f;
                    if (aet_index < v109.size())
                        v64 = v109[aet_index];

                    if (start_time < 0.0f)
                        start_time = v64;

                    if (end_time >= 0.0f && start_time >= end_time)
                        end_time = -1.0f;

                    data.play_data.init_aet(aet, index,
                        std::string(aet_name_id.second), std::string(aet_name_id.first),
                        &not_init[aet], start_time, end_time, frame_rate_control);
                }
            }
            else {
                float_t end_time = -1.0f;
                float_t start_time = -1.0f;
#if PV_DEBUG
                FrameRateControl* frame_rate_control = &sys_frame_rate;
#else
                FrameRateControl* frame_rate_control = 0;
#endif

                if (data.has_aet_frame[i]) {
                    int32_t aet_frame = get_field_aet_frame_by_name_id(next_field_data, aet, aet_name_id);
                    start_time = get_aet_frame_max_frame(get_frame(aet_frame,
                        dsc_time, curr_time), aet, aet_name_id, dsc_time, curr_time, &end_time);

                    if (start_time < 0.0f)
                        start_time = 0.0f;

                    if (end_time >= 0.0f && start_time >= end_time)
                        end_time = -1.0f;

                    get_aet_frame_max_frame_by_name(start_time, end_time, aet_name_id);
                    frame_rate_control = get_diva_pv_frame_rate();
                }

                data.play_data.init_aet(aet, index,
                    std::string(aet_name_id.second), std::string(aet_name_id.first),
                    &not_init[aet], start_time, end_time, frame_rate_control);
            }

            data.play_data.set_aet_visible(aet, index, disp);
            index++;
        }
    }

    data.play_data.set_spr_set_back_id(next_field_data.spr_set_back_id);

    std::vector<auth_3d_id> auth_3d_end;
    std::set_difference(
        curr_field_data.auth_3d_ids.begin(), curr_field_data.auth_3d_ids.end(),
        next_field_data.auth_3d_ids.begin(), next_field_data.auth_3d_ids.end(), std::back_inserter(auth_3d_end));
    std::vector<auth_3d_id> auth_3d_start;
    std::set_difference(
        next_field_data.auth_3d_ids.begin(), next_field_data.auth_3d_ids.end(),
        curr_field_data.auth_3d_ids.begin(), curr_field_data.auth_3d_ids.end(), std::back_inserter(auth_3d_start));
    std::vector<auth_3d_id> auth_3d_cont;
    std::set_intersection(
        curr_field_data.auth_3d_ids.begin(), curr_field_data.auth_3d_ids.end(),
        next_field_data.auth_3d_ids.begin(), next_field_data.auth_3d_ids.end(), std::back_inserter(auth_3d_cont));

    for (auth_3d_id& i : auth_3d_end) {
        i.set_visibility(false);
        i.set_enable(false);
    }

    bool disp = true;
    if (sub_14013C8C0()->sub_1400E7910() == 3) {
        pv_disp2d* pv_disp2d = data.pv_disp2d;
        if (pv_disp2d && pv_disp2d->title_start_3d_field >= 0 && pv_disp2d->title_end_3d_field >= 0
            && field >= pv_disp2d->title_start_3d_field)
            disp = field > pv_disp2d->title_end_3d_field;
    }

    for (auth_3d_id& i : auth_3d_start) {
        float_t req_frame = 0.0f;
        float_t max_frame = -1.0f;

        if (data.has_auth_3d_frame) {
            int32_t auth_3d_frame = get_field_auth_3d_frame(next_field_data, i.get_uid(), 0);
            req_frame = pv_game::get_auth_3d_frame_max_frame(get_frame(auth_3d_frame,
                dsc_time, curr_time), i.get_uid(), dsc_time, curr_time, 0, &max_frame);
            req_frame = max_def(req_frame, 0.0f);
        }

        i.set_enable(true);
        i.set_req_frame(req_frame);
        i.set_max_frame(max_frame);
        i.set_paused(false);
        i.set_visibility(disp);
    }

    for (auth_3d_id& i : auth_3d_cont) {
        float_t max_frame = -1.0f;

        if (data.has_auth_3d_frame) {
            int32_t auth_3d_frame = get_field_auth_3d_frame(next_field_data, i.get_uid(), 0);
            float_t req_frame = get_auth_3d_frame_max_frame(get_frame_continue(auth_3d_frame),
                i.get_uid(), dsc_time, curr_time, 0, &max_frame);
            if (req_frame >= 0.0f)
                i.set_req_frame(req_frame);
        }

        i.set_max_frame(max_frame);
    }

    if (next_field_data.light_auth_3d_id == curr_field_data.light_auth_3d_id) {
        auth_3d_id& light_auth = next_field_data.light_auth_3d_id;
        if (light_auth.check_not_empty()) {
            float_t max_frame = -1.0f;

            if (data.has_light_frame) {
                int32_t light_frame = get_field_auth_3d_frame(next_field_data, light_auth.get_uid(), 1);
                float_t req_frame = get_auth_3d_frame_max_frame(get_frame_continue(light_frame),
                    light_auth.get_uid(), dsc_time, curr_time, 1, &max_frame);
                if (req_frame >= 0.0f)
                    light_auth.set_req_frame(req_frame);
            }

            light_auth.set_max_frame(max_frame);
        }
    }
    else {
        auth_3d_id& prev_light_auth = curr_field_data.light_auth_3d_id;
        if (prev_light_auth.check_not_empty()) {
            prev_light_auth.set_visibility(false);
            prev_light_auth.set_enable(false);
        }

        auth_3d_id& next_light_auth = next_field_data.light_auth_3d_id;
        if (next_light_auth.check_not_empty()) {
            float_t req_frame = 0.0f;
            float_t max_frame = -1.0f;

            if (data.has_light_frame) {
                int32_t light_frame = get_field_auth_3d_frame(next_field_data, next_light_auth.get_uid(), 1);
                req_frame = get_auth_3d_frame_max_frame(get_frame(light_frame,
                    dsc_time, curr_time), next_light_auth.get_uid(), dsc_time, curr_time, 1, &max_frame);
                req_frame = max_def(req_frame, 0.0f);
            }

            next_light_auth.set_enable(true);
            next_light_auth.set_req_frame(req_frame);
            next_light_auth.set_max_frame(req_frame);
            next_light_auth.set_paused(false);
            next_light_auth.set_visibility(true);
        }
    }

    for (uint64_t& i : curr_field_data.stop_eff_list)
        Glitter::glt_particle_manager->FreeSceneEffect(hash_fnv1a64m_empty, i);

    for (uint64_t& i : next_field_data.play_eff_list)
        Glitter::glt_particle_manager->LoadScene(hash_fnv1a64m_empty, i);

    data.current_field = field;
    data.changed_field = true;
}

void pv_game::check_auth_replace_by_module(const char* name, int32_t& uid) {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

    int32_t perf_count = (int32_t)data.pv->get_performer_count();
    for (int32_t i = 0; i < perf_count; i++)
        for (const pv_db_pv_auth_replace_by_module& j : data.pv->auth_replace_by_module)
            if (j.id == i && j.module_id == modules[i] && !j.org_name.compare(name)) {
                uid = aft_auth_3d_db->get_uid(j.name.c_str());
                return;
            }
}

int32_t pv_game::check_chrcam(const char* name, int32_t& uid) {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

    std::string _name;
    _name.assign(name);

    std::string campv_base;
    campv_base.assign(data.campv_string);
    campv_base.append("_BASE");

    std::string campv_parts;
    campv_parts.assign(data.campv_string);
    campv_parts.append("_PARTS");

    int32_t cam_index = 0;
    if (!campv_base.compare(_name))
        cam_index = 1;
    else if (!_name.find(campv_parts))
        cam_index = atoi(_name.c_str() + campv_parts.size());

    for (const pv_db_pv_chrcam& i : data.pv->chrcam)
        if (!i.org_name.compare(_name)
            && !i.chara.compare(chara_index_get_auth_3d_name(data.chara[i.id].chara_index))) {
            uid = aft_auth_3d_db->get_uid(i.name.c_str());
            return 1;
        }

    return cam_index;
}

bool pv_game::check_life_gauge_safety() {
    return get_life_gauge_safety_time() > 0.0f;
}

int32_t pv_game::ctrl(float_t delta_time, int64_t curr_time) {
    data.hit_border = false;
    data.life_gauge_bonus = 0;
    if (!loaded)
        return 0;

    data.play_data.fade_begin_ctrl();

    if (pv_osage_manager_array_get_disp()) {
        pv_osage_manager_array_set_not_reset_true();
        return 0;
    }

    data.changed_field = 0;
    if (data.play_data.field_64C) {
        if (!data.field_2D093)
            data.pv_data.ctrl(delta_time, curr_time, false);
        /*else
            for (int32_t i = 0; i < 2; i++)
                if (app::TaskWork::CheckTaskReady(task_movie_get(i)))
                    task_movie_get(i)->sub_14041F4E0();*/

        if (!sub_14013C8C0()->sub_1400E7920()) {
            hit_state hit_state = HIT_MAX;
            bool v94 = false;
            vec2 pos;
            pv_game_se_data se_data;
            se_data.se_name.assign(data.play_data.se_name);
            se_data.slide_name.assign(data.play_data.slide_name);
            se_data.chainslide_first_name.assign(data.play_data.chainslide_first_name);
            se_data.chainslide_sub_name.assign(data.play_data.chainslide_sub_name);
            se_data.chainslide_success_name.assign(data.play_data.chainslide_success_name);
            se_data.chainslide_failure_name.assign(data.play_data.chainslide_failure_name);
            se_data.slidertouch_name.assign(data.play_data.slidertouch_name);
            int32_t a7 = 0;
            int32_t multi_count = 0;
            bool slide = false;
            bool slide_chain = false;
            bool slide_chain_start = false;
            bool slide_chain_end = false;
            bool slide_chain_continue = false;
            bool success = false;
            float_t time_offset = 0.0f;
            int32_t target = -1;
            /*sub_14013C8F0()->sub_14012C300(delta_time, hit_state, v94, pos, se_data, a7, multi_count, 1,
                slide, slide_chain, slide_chain_start, slide_chain_end, slide_chain_continue,
                data.song_energy_base <= data.song_energy, success, 0, 0, data.changed_field, time_offset, target);*/
        }

        if (!data.field_2D096) {
            sub_1400FC500();
            data.field_2D096 = true;
        }

        data.play_data.fade_end_ctrl();
        data.play_data.frame_ctrl();
        data.play_data.ui_set_disp();
        data.play_data.set_aet_song_energy();
        data.play_data.score_update(data.score_final, true);

        return sub_1400FC780(delta_time);
    }

    if (end_pv) {
        end_pv = false;
        return 1;
    }

    hit_state hit_state = HIT_MAX;
    bool v94 = true;
    vec2 pos;
    int32_t a7 = 0;
    int32_t multi_count = 0;
    bool slide = false;
    bool slide_chain = false;
    bool slide_chain_start = false;
    bool slide_chain_end = false;
    bool slide_chain_continue = false;
    bool success = false;
    float_t time_offset = 0.0f;
    int32_t target = -1;

    data.current_time = data.pv_data.ctrl(delta_time, curr_time, true);
    data.current_time_float = (float_t)data.current_time * 0.000000001f;
    data.field_2D038 = data.current_time_float + data.field_2D03C;

    if (!sub_14013C8C0()->sub_1400E7920()) {
        /*TaskLampCtrl* task_lamp_ctrl = task_lamp_ctrl_get();
        task_lamp_ctrl->field_74 = 2;
        task_lamp_ctrl->field_78 = 0xFFFFFFFF;
        task_lamp_ctrl->field_7C = 0;
        task_slider_control_get()->sub_140618980(3);*/

        pv_game_se_data se_data;
        se_data.se_name.assign(data.play_data.se_name);
        se_data.slide_name.assign(data.play_data.slide_name);
        se_data.chainslide_first_name.assign(data.play_data.chainslide_first_name);
        se_data.chainslide_sub_name.assign(data.play_data.chainslide_sub_name);
        se_data.chainslide_success_name.assign(data.play_data.chainslide_success_name);
        se_data.chainslide_failure_name.assign(data.play_data.chainslide_failure_name);
        se_data.slidertouch_name.assign(data.play_data.slidertouch_name);

        /*sub_14013C8F0()->sub_14012C300(delta_time, hit_state, v94, pos, se_data, a7, multi_count, 0,
            slide, slide_chain, slide_chain_start, slide_chain_end, slide_chain_continue,
            data.song_energy_border <= data.song_energy, success, data.pv_data.sub_1400FDA40(),
            data.pv_data.target_index, data.changed_field, time_offset, target); */
        data.score_final += a7;
        data.score_hold += a7;
        data.score_hold_multi += a7;
    }
    /*else if (sub_14013C8C0()->sub_1400E7910() != 3)
        task_slider_control_get()->sub_140618980(1);*/

    bool challenge_time = hit_state != HIT_MAX
        && data.pv_data.first_challenge_note >= 0 && data.pv_data.last_challenge_note >= 0
        && target >= data.pv_data.first_challenge_note && target <= data.pv_data.last_challenge_note;

    if (data.field_2CFB0 > 0)
        data.field_2CFB0--;

    if (!data.field_2D0BC && slide_chain_end)
        data.field_2D0BC = true;

    if (sub_14013C8C0()->sub_1400E7920()) {
        if (sub_14013C8C0()->sub_1400E7910() == 3) {
            data.field_2CF8C -= delta_time;
            if (data.field_2CF8C <= 0.0f)
                end(true, true);
        }
        else if (sub_14010EF00()) {
            data.field_2D0A8 = sub_14010F930();
            end(true, true);
        }
    }
    else {
        /*if (v94)
            sub_140105690();*/

        struc_773 v103[HIT_MAX];
        v103[HIT_COOL          ] = {  500,  true, 0,  true, 0 };
        v103[HIT_FINE          ] = {  300,  true, 1,  true, 2 };
        v103[HIT_SAFE          ] = {  100, false, 2,  true, 4 };
        v103[HIT_BAD           ] = {   50, false, 3,  true, 6 };
        v103[HIT_WRONG_COOL    ] = {  250, false, 0, false, 1 };
        v103[HIT_WRONG_FINE    ] = {  150, false, 1, false, 3 };
        v103[HIT_WRONG_SAFE    ] = {   50, false, 2, false, 5 };
        v103[HIT_WRONG_BAD     ] = {   30, false, 3, false, 7 };
        v103[HIT_WORST         ] = {    0, false, 4,  true, 8 };
        v103[HIT_COOL_DOUBLE   ] = { 1000,  true, 0,  true, 0 };
        v103[HIT_FINE_DOUBLE   ] = {  600,  true, 1,  true, 2 };
        v103[HIT_SAFE_DOUBLE   ] = {  200, false, 2,  true, 4 };
        v103[HIT_BAD_DOUBLE    ] = {  100, false, 3,  true, 6 };
        v103[HIT_COOL_TRIPLE   ] = { 1500,  true, 0,  true, 0 };
        v103[HIT_FINE_TRIPLE   ] = {  900,  true, 1,  true, 2 };
        v103[HIT_SAFE_TRIPLE   ] = {  300, false, 2,  true, 4 };
        v103[HIT_BAD_TRIPLE    ] = {  150, false, 3,  true, 6 };
        v103[HIT_COOL_QUADRIPLE] = { 2000,  true, 0,  true, 0 };
        v103[HIT_FINE_QUADRIPLE] = { 1200,  true, 1,  true, 2 };
        v103[HIT_SAFE_QUADRIPLE] = {  400, false, 2,  true, 4 };
        v103[HIT_BAD_QUADRIPLE ] = {  200, false, 3,  true, 6 };

        if (hit_state != HIT_MAX) {
            int64_t time = 0;
            calculate_score(v103[hit_state].field_0, v103[hit_state].field_4, time, pos, multi_count,
                hit_state, challenge_time, slide, slide_chain, slide_chain_start, slide_chain_end, slide_chain_continue);
            calculate_life_gauge(hit_state, check_life_gauge_safety(), challenge_time,
                slide, slide_chain, slide_chain_start, slide_chain_continue);
            if (!slide_chain || slide_chain_start || !slide_chain_continue) {
                data.total_hit_count[v103[hit_state].field_8]++;
                if (v103[hit_state].field_C)
                    data.hit_count[v103[hit_state].field_8]++;
                data.play_data.set_value_text(v103[hit_state].field_10, time_offset);
            }
        }

        if (hit_state == HIT_WORST) {
            if (/*!sub_14012F9A0(&sub_14013C8F0()->field_0.field_12B9C)
                && */!slide_chain || slide_chain_start || !slide_chain_continue) {
                pv_game_music_get()->set_channel_pair_volume_map(1, 0);
                data.field_2D090 = true;
            }

            if (data.notes_passed < data.target_reference_score.size() - 1)
                data.notes_passed++;
        }
        else if (hit_state != HIT_MAX) {
            pv_game_music_get()->set_channel_pair_volume_map(1, 100);
            data.field_2D090 = false;

            if (data.notes_passed < data.target_reference_score.size() - 1)
                data.notes_passed++;
        }

        if (success && !data.success) {
            data.success = true;
            data.play_data.init_aet_success_info();
            if (/*sub_14013C8F0()->field_0.field_12A8D && */data.play_data.pvbranch_success_se_name.size())
                sound_work_play_se(2, data.play_data.pvbranch_success_se_name.c_str(), 1.0f);
        }

        bool end = false;
        if (sub_14010EF00()) {
            data.field_2D0A8 = sub_14010F930();
            if (data.field_2D0A8 == 2)
                end = true;
            else {
                data.life_gauge = 0;
                data.life_gauge_final = 0;
                data.no_clear = true;
            }
        }

        int32_t combo = data.combo;
        if (combo < 2 || slide_chain && !slide_chain_start && slide_chain_continue)
            combo = 0;

        if (hit_state != HIT_MAX)
            data.field_2DB2C = pos;

        data.play_data.sub_140137DD0(data.life_gauge / 16, delta_time, data.field_2DB2C, combo);

        if (end)
            pv_game::end(true, true);

        else if (data.life_gauge <= 0)
            if (data.no_clear || !data.no_fail)
                pv_game::end(false, false);
            else
                data.play_data.set_not_clear();

        calculate_song_energy();
        calculate_song_energy_border();
    }

    play_data_ctrl(delta_time);

    title_image_ctrl(false);
    data.play_data.set_aet_song_energy();

    if (sub_14013C8C0()->sub_1400E7910() == 3 && data.field_2CF8C <= 5.0f && !data.field_2D05D) {
        data.play_data.init_aet_demo_font();
        data.field_2D05D = true;
    }

    data.field_2D084 = data.field_2D080;
    data.field_2D080++;
    return 0;
}

void pv_game::data_itmpv_disable() {
    for (auto& i : data.itmpv)
        for (auto& j : i)
            j.second.first.set_enable(false);
}

void pv_game::disp() {
    sub_1400FC6F0();
    //sub_14013C8F0()->sub_14012A4E0();
}

void pv_game::disp_song_name() {
    if (!sub_14013C8C0()->sub_1400E7920())
        data.play_data.disp_song_name(data.play_data.song_name.c_str());
}

void pv_game::edit_effect_ctrl(float_t delta_time) {
    if (data.edit_effect.data.index < 0)
        return;

    if (!data.play_data.field_64C)
        data.edit_effect.data.delta_time = delta_time * data.edit_effect.data.speed;
    data.edit_effect.data.time += data.edit_effect.data.delta_time;

    float_t end_frame = data.edit_effect.data.end_frame;
    float_t frame = 0.0f;
    if (end_frame > 0.0f) {
        frame = get_target_anim_fps() * data.edit_effect.data.time;
        if (data.edit_effect.data.loop)
            frame = fmodf(frame, end_frame + 1.0f);
        else if (frame > end_frame)
            frame = end_frame;
    }

    data.play_data.set_aet_edit_effect_frame(frame);
}

void pv_game::edit_effect_set(int32_t index, float_t speed, float_t delta_time) {
    int32_t aet_id = -1;
    bool loop = false;
    bool low_field = false;
    if (index >= 0 && index < data.edit_effect.aet_ids.size()) {
        const pv_db_pv_difficulty* diff = data.pv->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff) {
            int32_t edit_effect_index = diff->edit_effect[index].index;
            if (edit_effect_index >= 0 && edit_effect_index <= 83) {
                static const bool edit_effect_loop[] = {
                     true,  true,  true,  true,  true,  true,  true,  true,
                     true,  true,  true,  true,  true,  true,  true,  true,
                     true,  true, false, false, false, false, false, false,
                    false, false,  true,  true,  true,  true,  true,  true,
                     true,  true,  true,  true,  true,  true,  true,  true,
                     true,  true,  true,  true,  true,  true,  true,  true,
                     true,  true,  true,  true,  true,  true,  true, false,
                     true,  true,  true,  true,  true, false,  true,  true,
                     true,  true,  true,  true,  true,  true,  true, false,
                    false, false, false, false, false, false, false, false,
                    false, false, false, false,
                };

                low_field = diff->edit_effect[index].low_field;
                aet_id = data.edit_effect.aet_ids[index];
                loop = edit_effect_loop[edit_effect_index];
            }
        }
    }

    if (index != data.edit_effect.data.index || !loop) {
        data.edit_effect.data.time = 0.0f;
        data.edit_effect.data.end_frame = data.play_data.init_aet_edit_effect(aet_id, "eff", loop, low_field);
    }

    data.edit_effect.data.speed = speed;
    data.edit_effect.data.index = index;
    data.edit_effect.data.loop = loop;
    data.edit_effect.data.delta_time = speed * delta_time;
}

void pv_game::end(bool a2, bool set_fade) {
    data.play_data.field_64C = true;
    data.field_2D095 = a2;

    if (set_fade)
        pv_game_music_get()->set_fade_out(0.75f, true);
    else
        pv_game_music_get()->stop();

    data.play_data.disable_update();

    if (sub_14013C8C0()->sub_1400E7920() || sub_14013C8C0()->sub_1400E7910() == 6)
        data.play_data.field_650 = 41;
    else {
        data.play_data.field_654 = 0.0f;
        data.play_data.field_658 = 0;
        data.play_data.field_65C = 0.0f;
        data.play_data.field_650 = a2 ? 0 : 27;

        sub_14010F030();
    }
}

float_t pv_game::get_aet_frame_max_frame(int32_t aet_frame, int32_t aet_index, std::pair<std::string,
    std::string>& aet_name_id, int64_t dsc_time, int64_t curr_time, float_t* max_frame) {
    if (!max_frame)
        return -1.0f;

    *max_frame = -1.0f;
    if (dsc_time < 0 || curr_time < 0)
        return 0.0f;

    if (!((aet_frame + 4) & -5)) {
        data.aet_time[aet_index].insert_or_assign(aet_name_id, dsc_time);

        if (!aet_frame) {
            *max_frame = get_aet_frame_max_frame_change_field(aet_index, aet_name_id, dsc_time);
            if (*max_frame < 0.0f)
                *max_frame = get_next_aet_max_frame_change_field(aet_index, aet_name_id, dsc_time, curr_time);
        }
        return 0.0f;
    }
    else if (!((aet_frame + 5) & -5)) {
        float_t frame = -1.0f;
        int64_t aet_time = get_aet_time(aet_index, aet_name_id);
        if (aet_time >= 0) {
            frame = dsc_time_to_frame(curr_time - aet_time);
            float_t dsc_frame = roundf(dsc_time_to_frame(dsc_time - aet_time));
            frame = max_def(frame, dsc_frame);
        }

        if (aet_frame == -1) {
            *max_frame = get_aet_frame_max_frame_change_field(aet_index, aet_name_id, dsc_time);
            if (*max_frame < 0.0f)
                *max_frame = get_next_aet_max_frame_change_field(aet_index, aet_name_id, dsc_time, curr_time);
        }
        return frame;
    }
    else if (!((aet_frame + 6) & -5) && aet_frame == -2)
        *max_frame = get_aet_frame_max_frame_change_field(aet_index, aet_name_id, dsc_time);
    return -1.0f;
}

void pv_game::get_aet_frame_max_frame_by_name(float_t& frame, float_t& max_frame, std::pair<std::string, std::string>& aet_name_id) {
    if (frame < 0.0f && max_frame < 0.0f || !data.pv_disp2d)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;

    uint32_t aet_id = data.pv_disp2d->pv_aet_id;
    if (aet_name_id.first.size())
        aet_id = data_list[DATA_AFT].data_ft.aet_db.get_aet_by_name(aet_name_id.first.c_str())->id;

    float_t start_frame = aet_manager_get_scene_layer_start_time(aet_id, aet_name_id.second.c_str(), aft_aet_db);
    float_t end_frame = aet_manager_get_scene_layer_end_time(aet_id, aet_name_id.second.c_str(), aft_aet_db);

    if (frame >= 0.0f) {
        frame += start_frame;
        if (end_frame >= 0.0f && frame > end_frame)
            frame = end_frame;
    }

    if (max_frame >= 0.0f) {
        max_frame += start_frame;
        if (end_frame >= 0.0f && max_frame > end_frame)
            max_frame = end_frame;
    }
}

float_t pv_game::get_aet_frame_max_frame_change_field(int32_t aet_index,
    std::pair<std::string, std::string>& aet_name_id, int64_t time) {
    if (time < 0)
        return -1.0f;

    prj::vector_pair<int32_t, int32_t>& change_field = data.pv_data.branch_mode != 2
        ? data.pv_data.change_field_branch_fail
        : data.pv_data.change_field_branch_success;
    if (time == 10000LL * change_field.back().first)
        return roundf(dsc_time_to_frame(data.max_time - get_aet_time(aet_index, aet_name_id))) - 1.0f;
    return -1.0f;
}

int64_t pv_game::get_aet_time(int32_t aet_index, std::pair<std::string, std::string>& aet_name_id) {
    auto elem = data.aet_time[aet_index].find(aet_name_id);
    if (elem != data.aet_time[aet_index].end())
        return elem->second;
    return -1;
}

float_t pv_game::get_auth_3d_frame_max_frame(int32_t auth_3d_frame, int32_t auth_3d_uid,
    int64_t dsc_time, int64_t curr_time, uint8_t type, float_t* max_frame) {
    if (!max_frame || type != 0 && type != 1)
        return -1.0f;

    *max_frame = -1.0f;
    if (dsc_time < 0 || curr_time < 0)
        return 0.0f;

    if (!((auth_3d_frame + 4) & ~4u)) {
        if (type)
            data.light_time.insert_or_assign(auth_3d_uid, dsc_time);
        else
            data.auth_3d_time.insert_or_assign(auth_3d_uid, dsc_time);

        if (!auth_3d_frame) {
            *max_frame = get_auth_3d_frame_max_frame_change_field(auth_3d_uid, dsc_time, type);
            if (*max_frame < 0.0f)
                *max_frame = get_next_auth_3d_max_frame_change_field(auth_3d_uid, dsc_time, curr_time, type);
        }
        return 0.0f;
    }
    else if (!((auth_3d_frame + 5) & ~4u)) {
        float_t frame = -1.0f;
        int64_t auth_3d_time = get_auth_3d_time(auth_3d_uid, type);
        if (auth_3d_time >= 0) {
            frame = dsc_time_to_frame(curr_time - auth_3d_time);
            float_t dsc_frame = roundf(dsc_time_to_frame(dsc_time - auth_3d_time));
            frame = max_def(frame, dsc_frame);
        }

        if (auth_3d_frame == -1) {
            *max_frame = get_auth_3d_frame_max_frame_change_field(auth_3d_uid, dsc_time, type);
            if (*max_frame < 0.0f)
                *max_frame = get_next_auth_3d_max_frame_change_field(auth_3d_uid, dsc_time, curr_time, type);
        }
        return frame;
    }
    else if (!((auth_3d_frame + 6) & ~4u) && auth_3d_frame == -2)
        *max_frame = get_auth_3d_frame_max_frame_change_field(auth_3d_uid, dsc_time, type);
    return -1.0f;
}

float_t pv_game::get_auth_3d_frame_max_frame_change_field(uint32_t auth_3d_uid, int64_t time, uint8_t type) {
    if (time < 0)
        return -1.0f;

    prj::vector_pair<int32_t, int32_t>& change_field = data.pv_data.branch_mode != 2
        ? data.pv_data.change_field_branch_fail
        : data.pv_data.change_field_branch_success;
    if (time == 10000LL * change_field.back().first)
        return roundf(dsc_time_to_frame(data.max_time - get_auth_3d_time(auth_3d_uid, type))) - 1.0f;
    return -1.0f;
}

auth_3d_id pv_game::get_auth_3d_id(int32_t uid) {
    auto elem = data.auth_3d.find(uid);
    if (elem != data.auth_3d.end())
        return elem->second;
    return {};
}

int64_t pv_game::get_auth_3d_time(int32_t uid, uint8_t type) {
    switch (type) {
    case 0: {
        auto elem = data.auth_3d_time.find(uid);
        if (elem != data.auth_3d_time.end())
            return elem->second;
    } break;
    case 1: {
        auto elem = data.light_time.find(uid);
        if (elem != data.light_time.end())
            return elem->second;
    } break;
    }
    return -1;
}

uint32_t pv_game::get_chreff_auth_3d_object_set(int32_t& uid) {
    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    int32_t obj_set = -1;
    for (pv_game_chreff& i : data.chreff_auth_3d) {
        if (i.src_auth_3d_uid == uid) {
            obj_set = aft_obj_db->get_object_set_id(i.dst_auth_3d_category.c_str());
            if (obj_set != -1) {
                uid = i.dst_auth_3d_uid;
                break;
            }

            obj_set = aft_obj_db->get_object_set_id(i.src_auth_3d_category.c_str());
            if (obj_set != -1)
                return obj_set;
        }
    }

    if (obj_set == -1 && uid >= 0 && uid < aft_auth_3d_db->uid.size()) {
        std::string& category = aft_auth_3d_db->uid[uid].category;
        if (!category.find("EFFCHRPV"))
            return aft_obj_db->get_object_set_id(category.c_str());
    }
    return obj_set;
}

int64_t pv_game::get_data_itmpv_time(int32_t chara_id, int32_t index) {
    auto elem = data.itmpv[chara_id].find(index);
    if (elem != data.itmpv[chara_id].end())
        return elem->second.second;
    return -1;
}

float_t pv_game::get_data_rival_percentage() {
    return data.rival_percentage;
}

std::string pv_game::get_effect_se_file_name() {
    const pv_db_pv_difficulty* diff = data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff)
        return diff->effect_se_file_name;
    return {};
}

int32_t pv_game::get_field_aet_frame_by_name_id(pv_game_field& field,
    int32_t aet_index, std::pair<std::string, std::string>& aet_name_id) {
    auto elem = field.aet[aet_index].find(aet_name_id);
    if (elem != field.aet[aet_index].end())
        return elem->second;
    return -1;
}

int32_t pv_game::get_field_aet_index_by_name_id(pv_game_field& field,
    int32_t aet_index, std::pair<std::string, std::string>& aet_name_id) {
    int32_t index = 0;
    for (std::string& i : field.aet_names[aet_index]) {
        std::string* id = 0;
        if (index < field.aet_ids[aet_index].size())
            id = &field.aet_ids[aet_index][index];
        if ((!id || !id->compare(aet_name_id.first)) && !i.compare(aet_name_id.second))
            return index;

        index++;
    }
    return -1;
}

int32_t pv_game::get_field_auth_3d_frame(pv_game_field& field, int32_t uid, int32_t type) {
    switch (type) {
    case 0: {
        auto elem = field.auth_3d_frame_list.find(uid);
        if (elem != field.auth_3d_frame_list.end())
            return elem->second;
    } break;
    case 1:
        return field.light_frame;
    }
    return -3;
}

int32_t pv_game::get_frame(int32_t frame, int64_t dsc_time, int64_t curr_time) {
    if (frame == -3)
        frame = 0;
    if (dsc_time >= 0 && curr_time >= 0) {
        if (frame == -2)
            return 0;
        else if (frame == -6)
            return -4;
    }
    return frame;
}

int32_t pv_game::get_frame_continue(int32_t frame) {
    if (frame == -3)
        frame = -2;
    return frame;
}

float_t pv_game::get_life_gauge_safety_time() {
    float_t time = (float_t)data.life_gauge_safety_time - data.current_time_float;
    return max_def(time, 0.0f);
}

float_t pv_game::get_next_aet_max_frame_change_field(int32_t aet_index, std::pair<std::string,
    std::string>& aet_name_id, int64_t dsc_time, int64_t curr_time) {
    if (dsc_time < 0)
        return -1.0f;

    int64_t max_time = -1;
    if (dsc_time > data.pv_data.change_field_branch_time) {
        prj::vector_pair<int32_t, int32_t>& change_field = data.pv_data.branch_mode != 2
            ? data.pv_data.change_field_branch_fail
            : data.pv_data.change_field_branch_success;
        return get_next_aet_max_time_max_frame_change_field(aet_index,
            aet_name_id, dsc_time, &max_time, curr_time, change_field);
    }

    float_t max_frame_fail = get_next_aet_max_time_max_frame_change_field(aet_index,
        aet_name_id, dsc_time, &max_time, curr_time, data.pv_data.change_field_branch_fail);
    if (max_frame_fail < 0.0f)
        return get_next_aet_max_time_max_frame_change_field(aet_index,
            aet_name_id, dsc_time, &max_time, curr_time, data.pv_data.change_field_branch_success);
    else if (max_time > data.pv_data.change_field_branch_time) {
        float_t max_frame_success = get_next_aet_max_time_max_frame_change_field(aet_index,
            aet_name_id, dsc_time, &max_time, curr_time, data.pv_data.change_field_branch_success);
        if (max_frame_success >= 0.0f && max_frame_success < max_frame_fail)
            return max_frame_success;
    }
    return max_frame_fail;
}

float_t pv_game::get_next_aet_max_time_max_frame_change_field(int32_t aet_index,
    std::pair<std::string, std::string>& aet_name_id, int64_t dsc_time, int64_t* max_time,
    int64_t curr_time, prj::vector_pair<int32_t, int32_t>& change_field) {
    if (!max_time)
        return -1.0f;

    for (auto& i : change_field) {
        int64_t time = 10000LL * i.first;
        if (time > dsc_time && i.second >= 0 && i.second < data.field_data.size()
            && ((get_field_aet_frame_by_name_id(data.field_data[i.second], aet_index, aet_name_id) + 5) & ~5u) == 0) {
            *max_time = time;
            return roundf(dsc_time_to_frame(time - get_aet_time(aet_index, aet_name_id))) - 1.0f;
        }
    }
    return -1.0f;
}

float_t pv_game::get_next_auth_3d_max_frame_change_field(uint32_t auth_3d_uid,
    int64_t dsc_time, int64_t curr_time, int32_t type) {
    if (dsc_time < 0)
        return -1.0f;

    int64_t max_time = -1;
    if (dsc_time > data.pv_data.change_field_branch_time) {
        prj::vector_pair<int32_t, int32_t>& change_field = data.pv_data.branch_mode != 2
            ? data.pv_data.change_field_branch_fail
            : data.pv_data.change_field_branch_success;
        return get_next_auth_3d_max_time_max_frame_change_field(auth_3d_uid,
            dsc_time, &max_time, curr_time, change_field, type);
    }

    float_t max_frame_fail = get_next_auth_3d_max_time_max_frame_change_field(auth_3d_uid,
        dsc_time, &max_time, curr_time, data.pv_data.change_field_branch_fail, type);
    if (max_frame_fail < 0.0f)
        return get_next_auth_3d_max_time_max_frame_change_field(auth_3d_uid,
            dsc_time, &max_time, curr_time, data.pv_data.change_field_branch_success, type);
    else if (max_time > data.pv_data.change_field_branch_time) {
        float_t max_frame_success = get_next_auth_3d_max_time_max_frame_change_field(auth_3d_uid,
            dsc_time, &max_time, curr_time, data.pv_data.change_field_branch_success, type);
        if (max_frame_success >= 0.0f && max_frame_success < max_frame_fail)
            return max_frame_success;
    }
    return max_frame_fail;
}

float_t pv_game::get_next_auth_3d_max_time_max_frame_change_field(uint32_t auth_3d_uid, int64_t dsc_time,
    int64_t* max_time, int64_t curr_time, prj::vector_pair<int32_t, int32_t>& change_field, int32_t type) {
    if (!max_time)
        return -1.0f;

    for (auto& i : change_field) {
        int64_t time = 10000LL * i.first;
        if (time > dsc_time && i.second >= 0 && i.second < data.field_data.size()
            && ((get_field_auth_3d_frame(data.field_data[i.second], auth_3d_uid, type) + 5) & -6) == 0) {
            *max_time = time;
            return roundf(dsc_time_to_frame(time - get_auth_3d_time(auth_3d_uid, type))) - 1.0f;
        }
    }
    return -1.0f;
}

int32_t pv_game::get_play_data_ex_song_index() {
    return data.play_data.ex_song_index;
}

std::string pv_game::get_play_data_song_file_name() {
    return data.play_data.song_file_name;
}

const pv_db_pv* pv_game::get_pv_db_pv() {
    return data.pv;
}

float_t pv_game::get_pv_hidden_timing() {
    const pv_db_pv_difficulty* diff = data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff)
        return diff->hidden_timing;
    return 0.3f;
}

int32_t pv_game::get_pv_high_speed_rate() {
    const pv_db_pv_difficulty* diff = data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff)
        return diff->high_speed_rate;
    return 1;
}

std::string pv_game::get_pv_movie_file_name(int32_t index) {
    const pv_db_pv_difficulty* diff = data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff && index >= 0 && index < diff->movie_list.size())
        return diff->movie_list[index].name;
    return {};
}

float_t pv_game::get_pv_sudden_timing() {
    const pv_db_pv_difficulty* diff = data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff)
        return diff->sudden_timing;
    return 0.6f;
}

std::string pv_game::get_song_name() {
    struc_717* v13 = sub_14038BB30();
    struc_716* v15;
    if (v13->field_0.stage_index >= 4)
        v15 = &v13->field_28[2];
    else
        v15 = &v13->field_28[v13->field_0.stage_index];

    const pv_db_pv* pv = data.pv;
    if (pv->another_song.size() && v15->field_2C.another_song_index < pv->another_song.size()) {
        const std::string& another_song_name = pv->another_song[v15->field_2C.another_song_index].name;
        if (another_song_name.size())
            return another_song_name;
    }

    if (pv->ex_song.data.size()) {
        if (data.play_data.ex_song_index <= 0) {
            int32_t ex_song_index = sub_1400FCFD0(data.chara, pv);
            if (ex_song_index >= 0 && ex_song_index < pv->ex_song.data.size())
                return pv->ex_song.data[ex_song_index].name;
        }
        else if (data.play_data.ex_song_index <= pv->ex_song.data.size())
            return pv->ex_song.data[data.play_data.ex_song_index - 1LL].name;
    }
    return pv->song_name;
}

std::string pv_game::get_song_file_name(int32_t& ex_song_index) {
    ex_song_index = -1;

    struc_717* v13 = sub_14038BB30();
    struc_716* v15;
    if (v13->field_0.stage_index >= 4)
        v15 = &v13->field_28[2];
    else
        v15 = &v13->field_28[v13->field_0.stage_index];

    ex_song_index = -1;
    const pv_db_pv* pv = data.pv;
    if (pv->another_song.size() && v15->field_2C.another_song_index < pv->another_song.size()) {
        const std::string& another_song_name = pv->another_song[v15->field_2C.another_song_index].name;
        if (another_song_name.size())
            return another_song_name;
    }

    if (pv->ex_song.data.size()) {
        if (ex_song_index < 0) {
            int32_t _ex_song_index = sub_1400FCFD0(data.chara, pv);
            if (_ex_song_index >= 0 && _ex_song_index < pv->ex_song.data.size()) {
                ex_song_index = _ex_song_index;
                return pv->ex_song.data[_ex_song_index].name;
            }
        }
        else if (ex_song_index > 0 && data.play_data.ex_song_index <= pv->ex_song.data.size()) {
            ex_song_index = data.play_data.ex_song_index - 1;
            return pv->ex_song.data[data.play_data.ex_song_index - 1LL].name;
        }
    }

    return data.pv->song_file_name;
}

task_stage_info pv_game::get_stage_info(int32_t stage_index) {
    if (data.stage_infos.size())
        for (const task_stage_info& i : data.stage_infos)
            if (i.check() && i.get_stage_index() == stage_index)
                return i;
    return {};
}

bool pv_game::is_play_data_option_3() {
    return data.play_data.option == 3;
}

void pv_game::itmpv_reset() {
    for (auto& i : data.itmpv)
        for (auto& j : i) {
            auth_3d_id& id = j.second.first;
            id.set_repeat(0);
            id.set_enable(0);
            id.set_camera_root_update(0);
            id.set_visibility(1);
            id.set_frame_rate(get_diva_pv_frame_rate());
            id.set_shadow(true);
            id.set_chara_id((int32_t)(&i - data.itmpv));
            j.second.second = -1;
        }
}

bool pv_game::load() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    switch (state) {
    case 0: {
        wave_audio_storage_clear();
        if (sub_14013C8C0()->sub_1400E7920()) {
            reset();
            state = 1;
        }
        else /*if (sub_14013C8F0()->sub_14012AA70())*/ {
            reset();
            state = 1;
        }
        /*else
            break;*/
    }
    case 1: {
        if (sub_14013C8C0()->sub_1400E7910() < 4) {
            pv_id = sub_14013C8C0()->pv;
            const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
            if (pv)
                data.pv = pv;
            else
                break;

            const pv_db_pv_disp2d* pv_disp2d;
            int32_t pv_disp2d_pv_id;
            if (pv) {
                pv_disp2d = &data.pv->disp2d;
                pv_disp2d_pv_id = pv->id;
            }
            else {
                pv_disp2d = 0;
                pv_disp2d_pv_id = pv_id;
            }

            pv_disp2d_data.pv_id = pv_disp2d_pv_id;
            pv_disp2d_data.pv_spr_set_id = -1;
            pv_disp2d_data.pv_aet_set_id = -1;
            pv_disp2d_data.pv_aet_id = -1;
            if (pv_disp2d) {
                if (pv_disp2d->set_name.size()) {
                    std::string spr_set_name("SPR_GAM_PV");
                    spr_set_name.append(pv_disp2d->set_name);

                    std::string aet_set_name("AET_GAM_PV");
                    aet_set_name.append(pv_disp2d->set_name);

                    std::string aet_name(aet_set_name);
                    aet_name.append("_MAIN");

                    pv_disp2d_data.pv_spr_set_id = aft_spr_db->get_spr_set_id_by_name(spr_set_name.c_str());
                    pv_disp2d_data.pv_aet_set_id = aft_aet_db->get_aet_set_id_by_name(aet_set_name.c_str());
                    pv_disp2d_data.pv_aet_id = aft_aet_db->get_aet_id_by_name(aet_name.c_str());
                }

                pv_disp2d_data.title_start_2d_field = pv_disp2d->title_start_2d_field;
                pv_disp2d_data.title_end_2d_field = pv_disp2d->title_end_2d_field;
                pv_disp2d_data.title_start_2d_low_field = pv_disp2d->title_start_2d_low_field;
                pv_disp2d_data.title_end_2d_low_field = pv_disp2d->title_end_2d_low_field;
                pv_disp2d_data.title_start_3d_field = pv_disp2d->title_start_3d_field;
                pv_disp2d_data.title_end_3d_field = pv_disp2d->title_end_3d_field;
                pv_disp2d_data.target_shadow_type = pv_disp2d->target_shadow_type == 1;
                pv_disp2d_data.title_2d_layer.assign(pv_disp2d->title_2d_layer);
            }

            data.pv_disp2d = &pv_disp2d_data;
            //sub_14013C8F0()->field_0.target_shadow_type = pv_disp2d_data.target_shadow_type;
        }
        else
            field_C = -1;

        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff) {
            data.play_data.se_name.assign(diff->se_name);
            data.play_data.pvbranch_success_se_name.assign(diff->pvbranch_success_se_name);
            data.play_data.slide_name.assign(diff->slide_name);
            data.play_data.chainslide_first_name.assign(diff->chainslide_first_name);
            data.play_data.chainslide_sub_name.assign(diff->chainslide_sub_name);
            data.play_data.chainslide_success_name.assign(diff->chainslide_success_name);
            data.play_data.chainslide_failure_name.assign(diff->chainslide_failure_name);
            data.play_data.slidertouch_name.assign(diff->slidertouch_name);
            if (diff->attribute.bitset.test(2))
                data.has_slide = true;
            if (data.play_data.pvbranch_success_se_name.size())
                data.has_success_se = true;
            if (data.play_data.other_se_name.size())
                data.play_data.se_name.assign(data.play_data.other_se_name);
            if (data.play_data.se_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.se_name);
            if (data.play_data.other_slide_name.size())
                data.play_data.slide_name.assign(data.play_data.other_slide_name);
            if (data.play_data.other_chainslide_first_name.size())
                data.play_data.chainslide_first_name.assign(data.play_data.other_chainslide_first_name);
            if (data.play_data.other_chainslide_sub_name.size())
                data.play_data.chainslide_sub_name.assign(data.play_data.other_chainslide_sub_name);
            if (data.play_data.other_chainslide_success_name.size())
                data.play_data.chainslide_success_name.assign(data.play_data.other_chainslide_success_name);
            if (data.play_data.other_chainslide_failure_name.size())
                data.play_data.chainslide_failure_name.assign(data.play_data.other_chainslide_failure_name);
            if (data.play_data.other_slidertouch_name.size())
                data.play_data.slidertouch_name.assign(data.play_data.other_slidertouch_name);
            if (data.play_data.pvbranch_success_se_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.pvbranch_success_se_name);
            if (data.play_data.slide_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.slide_name);
            if (data.play_data.chainslide_first_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.chainslide_first_name);
            if (data.play_data.chainslide_sub_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.chainslide_sub_name);
            if (data.play_data.chainslide_success_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.chainslide_success_name);
            if (data.play_data.chainslide_failure_name.size())
                wave_audio_storage_load_wave_audio(data.play_data.chainslide_failure_name);
        }

        PlayerData* player_data = player_data_array_get(0);
        for (auto& i : player_data->field_5D0[sub_14038BB30()->field_0.field_0])
            if (i.pv_id != pv_id || i.edition != sub_14013C8C0()->edition) {
                if (i.field_CC != -1 && i.rival_percentage > 0)
                    data.rival_percentage = (float_t)i.rival_percentage * 0.01f;
                break;
            }

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            pv_game_chara& j = data.chara[i];
            j.reset();
            j.pv_data.field_4 = false;
            j.pv_data.field_70 = player_data->field_66C;

            struc_674 v750;
            v750.performer = i;
            v750.module = modules[i];
            v750.item = items[i];
            v750.field_1C = player_data->field_0;
            v750.pv = data.pv;
            if (data.pv)
                sub_140105010(data.chara, ROB_CHARA_COUNT, items_mask, v750);

            if (data.height_adjust)
                j.pv_data.height_adjust = true;

            pv_performer_size size = data.pv ? data.pv->get_performer_size(i) : PV_PERFORMER_NORMAL;
            switch (size) {
            case PV_PERFORMER_NORMAL:
            default:
                j.pv_data.chara_size_index = 1;
                break;
            case PV_PERFORMER_PLAY_CHARA:
                j.pv_data.chara_size_index = chara_init_data_get_chara_size_index(j.chara_index);
                break;
            case PV_PERFORMER_PV_CHARA:
                j.pv_data.chara_size_index = chara_init_data_get_chara_size_index(data.pv->get_performer_chara(i));
                break;
            case PV_PERFORMER_SHORT:
                j.pv_data.chara_size_index = 2;
                break;
            case PV_PERFORMER_TALL:
                j.pv_data.chara_size_index = 0;
                break;
            }

            set_eyes_adjust(&j);
        }
        state = 6;
    } break;
    case 2: {
        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
         /*if (diff && diff->movie_list.size()) {
            resolution_struct* res_wind = res_window_get();
            resolution_struct* res_wind_int = res_window_internal_get();

            struc_15 v750;
            v750.field_0.field_0.x = 0.0f;
            v750.field_0.field_0.y = (float_t)(res_wind->height - res_wind_int->height) * 0.5f;
            v750.field_0.field_8.x = (float_t)res_wind_int->width;
            v750.field_0.field_8.y = (float_t)res_wind_int->height;
            v750.field_0.field_10 = res_wind->resolution_mode;
            v750.field_0.field_14 = -1.0f;
            v750.field_0.field_1C = diff->movie_surface == PV_MOVIE_SURFACE_FRONT ? 0 : 2;
            v750.field_20 = true;

            for (pv_db_pv_movie& i : diff->movie_list)
                if (i.name.size() && aft_data->check_file_exists(i.name.c_str())) {
                    task_movie_get(v43->index)->sub_14041F260(v750);
                    task_movie_get(v43->index)->sub_14041F0A0(i.name);
                    task_movie_get(v43->index)->sub_14041F240(0.0f);
                }
        }*/

        if (sub_14013C8C0()->sub_1400E7910() >= 4) {
            state = 12;
            for (pv_game_chara& i : data.chara) {
                if (!i.motion_ids.size())
                    continue;

                i.chara_id = rob_chara_array_init_chara_index(i.chara_index, i.pv_data, i.cos, true);
                if (i.chara_id != -1)
                    i.rob_chr = rob_chara_array_get(i.chara_id);
            }

            rctx_ptr->render_manager.set_effect_texture(0);
            state = 12;
            return false;
        }

        data.motion_set_ids.clear();
        data.motion_set_face_ids.clear();

        if (diff) {
            for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                pv_game_chara& chr = data.chara[i];
                data.motion_set_ids.reserve(diff->motion[i].size());
                data.motion_set_face_ids.reserve(diff->motion[i].size());

                int32_t motion_face_id_index = 0;
                for (const pv_db_pv_motion& j : diff->motion[i]) {
                    if (j.index < 0)
                        continue;

                    uint32_t motion_id = j.id;
                    if (data.pv)
                        motion_id = data.pv->get_chrmot_motion_id(i, chr.chara_index, j);

                    uint32_t motion_set_id = aft_mot_db->get_motion_set_id_by_motion_id(motion_id);
                    if (motion_set_id != -1) {
                        if (!prj::find(chr.motion_ids, motion_id))
                            chr.motion_ids.push_back(motion_id);
                        if (!prj::find(data.motion_set_ids, motion_set_id))
                            data.motion_set_ids.push_back(motion_set_id);
                    }

                    if (j.index <= 0)
                        continue;

                    std::string motion_face_name;
                    motion_face_name.assign(chara_index_get_face_mot_name(chr.chara_index));
                    motion_face_name.append("_FACE_");
                    motion_face_name.append(j.name);

                    uint32_t motion_face_id = aft_mot_db->get_motion_id(motion_face_name.c_str());
                    if (motion_face_id_index < 10)
                        chr.pv_data.motion_face_ids[motion_face_id_index++] = motion_face_id;

                    uint32_t motion_face_set_id = aft_mot_db->get_motion_set_id_by_motion_id(motion_face_id);
                    if (motion_face_set_id != -1 && motion_face_set_id != 2) {
                        if (!prj::find(chr.motion_face_ids, motion_face_id))
                            chr.motion_face_ids.push_back(motion_face_id);
                        if (!prj::find(data.motion_set_face_ids, motion_face_set_id))
                            data.motion_set_face_ids.push_back(motion_face_set_id);
                    }
                }
            }
        }

        for (uint32_t& i : data.motion_set_ids) {
            motion_set_load_motion(i, std::string(data.pv->mdata.dir), aft_mot_db);
            motion_set_load_mothead(i, std::string(data.pv->mdata.dir), aft_mot_db);
        }

        for (uint32_t& i : data.motion_set_face_ids)
            motion_set_load_motion(i, std::string(data.pv->mdata.dir), aft_mot_db);

        pv_expression_file_load(aft_data, data.pv->pv_expression_file_name.c_str());

        for (pv_game_chara& i : data.chara) {
            if (!i.motion_ids.size())
                continue;

            i.chara_id = rob_chara_array_init_chara_index(i.chara_index, i.pv_data, i.cos, true);
            if (i.chara_id != -1)
                i.rob_chr = rob_chara_array_get(i.chara_id);
        }

        rctx_ptr->render_manager.set_effect_texture(0);
        state = 3;
    } return false;
    case 3: {
        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        /*for (pv_db_pv_movie& i : diff->movie_list)
            if (app::TaskWork::CheckTaskReady(task_movie_get(i.index))
                && !task_movie_get(i.index)->sub_14041F0E0())
                return false;*/

        for (uint32_t& i : data.motion_set_ids)
            if (motion_storage_check_mot_file_not_ready(i)
                || mothead_storage_check_mhd_file_not_ready(i))
                return false;

        for (uint32_t& i : data.motion_set_face_ids)
            if (motion_storage_check_mot_file_not_ready(i))
                return false;

        if (data.pv->pv_expression_file_name.size()
            && pv_expression_file_check_not_ready(data.pv->pv_expression_file_name.c_str()))
            return false;

        data.pv_data.dsc_state = 0;
        data.field_2D094 = false;
        data.use_osage_play_data = false;
        if (get_pv_db_pv()->use_osage_play_data) {
            bool use_osage_play_data = true;
            for (uint32_t& i : data.motion_set_ids) {
                bool has_opd = false;
                const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
                while (*opd_motion_set_ids != -1) {
                    if (*opd_motion_set_ids == i)
                        has_opd = true;
                    opd_motion_set_ids++;
                }

                if (i != 2 && !has_opd)
                    use_osage_play_data = false;
            }

            if (use_osage_play_data)
                data.use_osage_play_data = true;
        }
        state = 4;
    } return false;
    case 4: {
        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff && !data.field_2D094) {
            if (sub_14013C8C0()->sub_1400E7910() < 4
                && !data.pv_data.load(std::string(diff->script_file_name), this, data.music_play))
                return false;

            data.field_2D094 = true;

            int32_t field_index = 0;
            for (pv_game_field& i : data.field_data) {
                bool found = false;
                for (auto& i : data.pv_data.change_field_branch_fail)
                    if (i.second == field_index) {
                        found = true;
                        break;
                    }

                if (!found)
                    for (auto& i : data.pv_data.change_field_branch_success)
                        if (i.second == field_index) {
                            found = true;
                            break;
                        }

                if (!found) {
                    field_index++;
                    continue;
                }


                for (auto& j : i.auth_3d_frame_list)
                    switch (j.second) {
                    case -6:
                        j.second = -2;
                        break;
                    case -5:
                        j.second = -1;
                        break;
                    case -4:
                        j.second = 0;
                        break;
                    }

                switch (i.light_frame) {
                case -6:
                    i.light_frame = -2;
                    break;
                case -5:
                    i.light_frame = -1;
                    break;
                case -4:
                    i.light_frame = 0;
                    break;
                }

                for (auto& j : i.aet)
                    for (auto& k : j)
                        switch (k.second) {
                        case -6:
                            k.second = -2;
                            break;
                        case -5:
                            k.second = -1;
                            break;
                        case -4:
                            k.second = 0;
                            break;
                        }

                field_index++;
            }
        }

        bool has_chara = false;
        for (pv_game_chara& i : data.chara)
            if (i.chara_id != -1 && i.rob_chr) {
                has_chara = true;
                if (!task_rob_manager_check_chara_loaded(i.rob_chr->chara_id))
                    return false;
            }

        if (has_chara) {
            task_rob_manager_hide_task();

            for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                pv_game_chara& chr = data.chara[i];
                if (chr.chara_id == -1 || !chr.rob_chr)
                    continue;

                for (int32_t j = 0; j < PV_PERFORMER_ITEM_MAX; j++) {
                    int32_t item_no = data.pv->get_performer_item(i, (pv_performer_item)j);
                    if (!item_no || item_no < 0 && !items_mask[i].arr[j])
                        chr.rob_chr->load_body_parts_object_info(
                            pv_performer_item_to_item_id((pv_performer_item)j),
                            {}, aft_bone_data, aft_data, aft_obj_db);
                }
                set_osage_init(chr);
            }
        }
        state = 5;
    } return false;
    case 5: {
        if (skin_param_manager_array_check_task_ready())
            return false;

        if (!data.use_osage_play_data)
            for (pv_game_chara& i : data.chara) {
                if (i.chara_id == -1 || !i.rob_chr)
                    continue;

                pv_osage_manager_array_reset(i.chara_id);
                const std::vector<pv_data_set_motion>* set_motion = data.pv_data.get_set_motion(i.chara_id);
                if (set_motion)
                    pv_osage_manager_array_set_pv_set_motion(i.chara_id, *set_motion);
                pv_osage_manager_array_set_pv_id(i.chara_id, data.pv->id, true);
            }
        state = 8;
    } return false;
    case 6: {
        rand_state_array_4_set_seed_1393939();
        if (task_stage_load_task("PV_STAGE"))
            return false;

        for (const pv_db_pv_chreff& i : data.pv->chreff) {
            chara_index src_chara_index = chara_index_get_from_chara_name(i.name.c_str());
            chara_index dst_chara_index = data.chara[i.id].chara_index;

            const std::string& src_name = i.name;
            std::string dst_name(chara_index_get_auth_3d_name(data.chara[i.id].chara_index));

            for (const pv_db_pv_chreff_data& j : i.data) {
                pv_game_chreff chreff;
                chreff.src_chara_index = src_chara_index;
                chreff.dst_chara_index = dst_chara_index;
                chreff.type = j.type;
                chreff.src_auth_3d.assign(j.name);
                chreff.dst_auth_3d.assign(j.name);

                if (j.type && src_chara_index != dst_chara_index) {
                    size_t pos = chreff.dst_auth_3d.find(src_name);
                    if (pos != -1)
                        chreff.dst_auth_3d.replace(pos, 3, dst_name);
                    else {
                        size_t pos = chreff.dst_auth_3d.find(dst_name);
                        if (pos != -1)
                            chreff.dst_auth_3d.replace(pos, 3, src_name);
                    }
                }

                if (j.type != PV_CHREFF_DATA_AUTH3D && j.type != PV_CHREFF_DATA_AUTH3D_OBJ
                    && !(j.type && src_chara_index == dst_chara_index))
                    continue;

                chreff.src_auth_3d_uid = aft_auth_3d_db->get_uid(chreff.src_auth_3d.c_str());
                chreff.dst_auth_3d_uid = aft_auth_3d_db->get_uid(chreff.dst_auth_3d.c_str());

                std::string& src_category = aft_auth_3d_db->uid[chreff.src_auth_3d_uid].category;
                if (src_category.size())
                    chreff.src_auth_3d_category.assign(src_category);

                std::string& dst_category = aft_auth_3d_db->uid[chreff.dst_auth_3d_uid].category;
                if (dst_category.size())
                    chreff.dst_auth_3d_category.assign(dst_category);

                if (chreff.dst_auth_3d_uid == -1 || !dst_category.size()) {
                    chreff.dst_chara_index = src_chara_index;
                    chreff.dst_auth_3d.assign(chreff.src_auth_3d);
                    chreff.dst_auth_3d_uid = chreff.src_auth_3d_uid;
                    chreff.dst_auth_3d_category.assign(chreff.src_auth_3d_category);

                    if (chreff.dst_auth_3d_uid == -1 || !src_category.size())
                        continue;
                }

                if (j.type == PV_CHREFF_DATA_AUTH3D_OBJ && src_chara_index != dst_chara_index) {
                    std::string dst_category(chreff.dst_auth_3d_category);
                    size_t pos = dst_category.find(src_name);
                    if (pos != -1) {
                        dst_category.replace(pos, 3, dst_name);
                        if (aft_obj_db->get_object_set_id(dst_category.c_str()) != -1) {
                            chreff.dst_auth_3d_category.assign(dst_category);
                            data.chreff_auth_3d_obj.push_back(chreff);
                        }
                    }
                    else {
                        size_t pos = dst_category.find(dst_name);
                        if (pos != -1) {
                            dst_category.replace(pos, 3, src_name);
                            if (aft_obj_db->get_object_set_id(dst_category.c_str()) != -1) {
                                chreff.src_chara_index = src_chara_index;
                                chreff.dst_chara_index = dst_chara_index;
                                chreff.dst_auth_3d_category.assign(dst_category);
                                data.chreff_auth_3d_obj.push_back(chreff);
                            }
                        }
                    }
                }

                data.chreff_auth_3d.push_back(chreff);
            }
        }

        data.chreff_auth_3d_obj_set_ids.clear();

        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff) {
            data.field_data.reserve(diff->field.data.size());
            light_param_data_storage_data_set_pv_id(sub_14013C8C0()->pv);

            for (const pv_db_pv_field& i : diff->field.data) {
                pv_game_field field;

                int32_t stage_index = i.stage_index;
                if (data.ex_stage && i.ex_stage_index != -1)
                    stage_index = i.ex_stage_index;

                if (stage_index != -1) {
                    field.stage_index = stage_index;
                    data.stage_indices.push_back(stage_index);
                }

                const std::vector<std::string>* auth_3d_list = &i.auth_3d_list;
                if (data.ex_stage && i.ex_auth_3d_list.size())
                    auth_3d_list = &i.ex_auth_3d_list;

                size_t auth_3d_list_index = 0;
                for (const std::string& j : *auth_3d_list) {
                    int32_t auth_3d_uid = aft_auth_3d_db->get_uid(j.c_str());
                    check_auth_replace_by_module(j.c_str(), auth_3d_uid);
                    if (auth_3d_uid != -1) {
                        int32_t frame = -3;
                        if (auth_3d_list_index < i.auth_3d_frame_list.size()) {
                            frame = i.auth_3d_frame_list[auth_3d_list_index];
                            data.has_auth_3d_frame = true;
                        }

                        int32_t chreff_auth_3d_obj_set_id = get_chreff_auth_3d_object_set(auth_3d_uid);
                        field.auth_3d_frame_list.insert_or_assign(auth_3d_uid, frame);
                        field.auth_3d_uids.push_back(auth_3d_uid);
                        data.loaded_auth_3d_uids.push_back(auth_3d_uid);

                        if (aft_auth_3d_db->uid[auth_3d_uid].category.size())
                            data.auth_3d_categories.push_back(aft_auth_3d_db->uid[auth_3d_uid].category);

                        if (chreff_auth_3d_obj_set_id != -1)
                            data.chreff_auth_3d_obj_set_ids.push_back(chreff_auth_3d_obj_set_id);
                        else {
                            size_t pos = j.find('_');
                            if (pos != -1) {
                                std::string stage_name = j.substr(0, pos);
                                uint32_t stage_index = aft_stage_data->get_stage_index(stage_name.c_str());
                                if (stage_index != -1)
                                    data.stage_indices.push_back(stage_index);
                            }
                        }
                    }
                    auth_3d_list_index++;
                }

                int32_t light_uid = aft_auth_3d_db->get_uid(i.light.c_str());
                check_auth_replace_by_module(i.light.c_str(), light_uid);

                if (light_uid != -1) {
                    field.light_auth_3d_uid = light_uid;
                    data.loaded_auth_3d_uids.push_back(light_uid);

                    field.light_frame = i.light_frame;
                    if (i.light_frame_set)
                        data.has_light_frame = true;

                    if (aft_auth_3d_db->uid[light_uid].category.size())
                        data.auth_3d_categories.push_back(aft_auth_3d_db->uid[light_uid].category);

                    size_t pos = i.light.find('_');
                    if (pos != -1) {
                        std::string stage_name = i.light.substr(0, pos);
                        uint32_t stage_index = aft_stage_data->get_stage_index(stage_name.c_str());
                        if (stage_index != -1)
                            data.stage_indices.push_back(stage_index);
                    }
                }

                if (i.spr_set_back.size()) {
                    uint32_t spr_set_id = aft_spr_db->get_spr_set_id_by_name(i.spr_set_back.c_str());
                    if (spr_set_id != -1) {
                        std::string spr_set_back_image(i.spr_set_back);
                        spr_set_back_image.append("_IMAGE");

                        uint32_t spr_id = aft_spr_db->get_spr_set_id_by_name(spr_set_back_image.c_str());
                        if (spr_id != -1) {
                            data.spr_set_back_ids.push_back(spr_set_id);
                            field.stage_index = aft_stage_data->get_stage_index("STGAETBACK");
                            data.stage_indices.push_back(field.stage_index);
                        }
                    }
                }

                for (int32_t j = 0; j < PV_AET_MAX; j++) {
                    const std::vector<std::string>& aet_name = i.aet.name[j];
                    const std::vector<std::string>& aet_id = i.aet.id[j];
                    const std::vector<int32_t>& aet_frame = i.aet.frame[j];

                    size_t aet_count = aet_name.size();
                    for (size_t k = 0; k < aet_count; k++) {
                        const std::string& name = aet_name[k];
                        field.aet_names[j].push_back(name);
                        if (k)
                            data.has_aet_list[k] = true;

                        std::string id;
                        if (k < aet_id.size())
                            id.assign(aet_id[k]);
                        field.aet_ids[j].push_back(id);

                        int32_t frame = -3;
                        if (k < aet_frame.size()) {
                            frame = aet_frame[k];
                            data.has_aet_frame[j] = true;
                        }

                        field.aet[j].insert_or_assign({ id, name }, frame);
                    }
                }

                if (Glitter::glt_particle_manager) {
                    int32_t emision_rs_list_index = 0;
                    for (const std::string& j : i.effect_rs_list) {
                        float_t emission = -1.0;
                        if (emision_rs_list_index < i.effect_emision_rs_list.size())
                            emission = i.effect_emision_rs_list[emision_rs_list_index++];

                        pv_effect_resource pv_eff;
                        pv_eff.name.assign(j);
                        pv_eff.emission = emission;
                        data.effect_rs_list.push_back(pv_eff);
                    }

                    for (const std::string& j : i.play_eff_list)
                        field.play_eff_list.push_back(Glitter::glt_particle_manager->CalculateHash(j.c_str()));

                    for (const std::string& j : i.stop_eff_list)
                        field.stop_eff_list.push_back(Glitter::glt_particle_manager->CalculateHash(j.c_str()));
                }

                field.stage_flag = i.stage_flag;
                field.cam_blur = i.cam_blur;
                field.npr_type = i.npr_type;
                field.sdw_off = i.sdw_off;
                data.field_data.push_back(field);
            }

            prj::sort_unique(data.chreff_auth_3d_obj_set_ids);
            for (uint32_t& i : data.chreff_auth_3d_obj_set_ids)
                object_storage_load_set(aft_data, aft_obj_db, i);

            char campv[0x40];
            sprintf_s(campv, sizeof(campv), "CAMPV%03d", get_pv_db_pv()->id);
            data.campv_string.assign(campv);

            aft_auth_3d_db->get_category_uids(data.campv_string.c_str(), data.campv_auth_3d_uids);
            if (data.campv_auth_3d_uids.size()) {
                data.auth_3d_categories.push_back(data.campv_string);

                data.loaded_auth_3d_uids.insert(data.loaded_auth_3d_uids.end(),
                    data.campv_auth_3d_uids.begin(), data.campv_auth_3d_uids.end());
            }

            char itmpv[0x40];
            sprintf_s(itmpv, sizeof(itmpv), "ITMPV%03d", get_pv_db_pv()->id);
            data.itmpv_string.assign(itmpv);

            aft_auth_3d_db->get_category_uids(data.itmpv_string.c_str(), data.itmpv_auth_3d_uids);
            if (data.itmpv_auth_3d_uids.size()) {
                data.auth_3d_categories.push_back(data.itmpv_string);
                for (int32_t& i : data.itmpv_auth_3d_uids)
                    check_auth_replace_by_module(auth_3d_data_get_uid_name(i, aft_auth_3d_db), i);

                data.loaded_auth_3d_uids.insert(data.loaded_auth_3d_uids.end(),
                    data.itmpv_auth_3d_uids.begin(), data.itmpv_auth_3d_uids.end());
            }

            bool v261 = false;
            for (int32_t i = 0; i < 3; i++) {
                v261 |= !!diff->field_228[i].size();
                for (auto& j : diff->field_228[i])
                    data.itmpv_uids.push_back(auth_3d_data_get_uid_name(j.second.second, aft_auth_3d_db));
            }

            if (v261)
                for (std::string& i : data.itmpv_uids)
                    data.auth_3d_categories.push_back(i);

            data.field_2D770.clear();
            for (int32_t i = 0; i < 3; i++)
                for (auto& j : diff->field_228[i])
                    data.field_2D770.push_back(j.second.first.set_id);

            prj::sort_unique(data.field_2D770);
            for (uint32_t& i : data.field_2D770)
                object_storage_load_set(aft_data, aft_obj_db, i);

            data.ex_song_ex_auth_obj_set_ids.clear();
            for (const pv_db_pv_ex_song& i : get_pv_db_pv()->ex_song.data)
                for (const pv_db_pv_ex_song_ex_auth& j : i.ex_auth) {
                    uint32_t uid = aft_auth_3d_db->get_uid(j.org_name.c_str());
                    data.loaded_auth_3d_uids.push_back(uid);
                    const char* category_name = aft_auth_3d_db->uid[uid].category.c_str();
                    if (category_name) {
                        uint32_t obj_set_id = aft_obj_db->get_object_set_id(category_name);
                        if (obj_set_id != -1)
                            data.ex_song_ex_auth_obj_set_ids.push_back(obj_set_id);
                    }
                }

            prj::sort_unique(data.ex_song_ex_auth_obj_set_ids);
            for (uint32_t& i : data.ex_song_ex_auth_obj_set_ids)
                object_storage_load_set(aft_data, aft_obj_db, i);

            prj::sort_unique(data.stage_indices);
            if (!data.stage_indices.size())
                data.stage_indices.push_back(0);

            //stage_param_data_coli_data_load(data.pv->id);
            task_stage_set_stage_indices(data.stage_indices);
            task_stage_current_set_stage_display(false, true);

            data.stgpvhrc_obj_set_ids.clear();
            for (int32_t& i : data.stage_indices) {
                std::string stage_name(aft_stage_data->get_stage_name(i));
                if (stage_name.find("STGPV"))
                    continue;

                stage_name.resize(8);
                stage_name.append("HRC");
                uint32_t obj_set_id = aft_obj_db->get_object_set_id(stage_name.c_str());
                if (obj_set_id != -1)
                    data.stgpvhrc_obj_set_ids.push_back(obj_set_id);
            }

            prj::sort_unique(data.stgpvhrc_obj_set_ids);
            for (uint32_t& i : data.stgpvhrc_obj_set_ids)
                object_storage_load_set(aft_data, aft_obj_db, i);

            data.has_frame_texture = false;
            for (const pv_db_pv_frame_texture& i : get_pv_db_pv()->frame_texture)
                if (i.data.size()) {
                    data.has_frame_texture = true;
                    rctx_ptr->post_process.frame_texture_reset();
                    break;
                }
        }
        state = 7;
    } return false;
    case 7: {
        if (task_stage_check_not_loaded())
            return false;

        for (uint32_t& i : data.chreff_auth_3d_obj_set_ids)
            if (object_storage_load_obj_set_check_not_read(i))
                return false;

        for (uint32_t& i : data.field_2D770)
            if (object_storage_load_obj_set_check_not_read(i))
                return false;

        for (uint32_t& i : data.ex_song_ex_auth_obj_set_ids)
            if (object_storage_load_obj_set_check_not_read(i))
                return false;

        state = 2;
    } break;
    case 8: {
        prj::sort_unique(data.spr_set_back_ids);
        for (uint32_t& i : data.spr_set_back_ids)
            sprite_manager_read_file(i, std::string(data.pv->mdata.dir), aft_data, aft_spr_db);
        data.field_2D7E8 = true;

        prj::sort_unique(data.auth_3d_categories);
        for (std::string& i : data.auth_3d_categories)
            auth_3d_data_load_category(i.c_str(), data.pv->mdata.dir.c_str());
        data.field_2DAC8 = true;

        prj::sort_unique(data.loaded_auth_3d_uids);
        for (int32_t& i : data.loaded_auth_3d_uids) {
            auth_3d_id id = auth_3d_data_load_uid(i, aft_auth_3d_db);
            if (!id.check_not_empty())
                continue;

            id.set_enable(false);
            id.set_repeat(true);
            id.set_paused(false);

            data.auth_3d.insert_or_assign(i, id);
        }

        for (pv_game_field& i : data.field_data) {
            for (int32_t& j : i.auth_3d_uids) {
                auth_3d_id id = get_auth_3d_id(j);
                if (!id.check_not_empty())
                    continue;

                for (pv_game_chreff& k : data.chreff_auth_3d_obj)
                    if (k.dst_auth_3d_uid == j) {
                        id.set_src_dst_chara(k.src_chara_index, k.dst_chara_index);
                        break;
                    }

                i.auth_3d_ids.push_back(id);
            }

            prj::sort(i.auth_3d_ids);

            auth_3d_id light_auth_3d_id = get_auth_3d_id(i.light_auth_3d_uid);
            if (light_auth_3d_id.check_not_empty())
                i.light_auth_3d_id = light_auth_3d_id;
            else
                i.light_auth_3d_id = {};
        }

        for (int32_t& i : data.campv_auth_3d_uids) {
            const char* name = auth_3d_data_get_uid_name(i, aft_auth_3d_db);
            if (!name)
                continue;

            int32_t cam_index = check_chrcam(name, i);
            if (!cam_index)
                continue;

            auto elem = data.auth_3d.find(i);
            if (elem != data.auth_3d.end()) {
                auth_3d_id& id = elem->second;
                id.set_camera_root_update(true);
                id.set_repeat(false);
                id.set_frame_rate(get_diva_pv_frame_rate());
                data.campv.insert_or_assign(cam_index, id);
            }
        }
        data.campv_index = 0;

        for (int32_t& i : data.itmpv_auth_3d_uids) {
            const char* name = auth_3d_data_get_uid_name(i, aft_auth_3d_db);
            if (!name)
                continue;

            std::string uid_name(name);

            char buf[0x200];
            int32_t chara_id = -1;
            for (int32_t j = 0; j < ROB_CHARA_COUNT; j++) {
                sprintf_s(buf, sizeof(buf), "%s_%dP", data.itmpv_string.c_str(), j + 1);
                if (!uid_name.compare(buf)) {
                    chara_id = j;
                    break;
                }
            }

            if (chara_id != -1)
                continue;

            int32_t itm_index = 1;
            std::string index_str = uid_name.substr(utf8_length(buf));
            if (!index_str.size() && index_str.size() == 4 && index_str[0] == '_'
                && isdigit(index_str[1]) && isdigit(index_str[2]) && isdigit(index_str[3]))
                itm_index = atoi(index_str.c_str() + 1);

            if (chara_id >= 0 && itm_index >= 0) {
                auto elem = data.auth_3d.find(i);
                if (elem != data.auth_3d.end())
                    data.itmpv[chara_id].insert_or_assign(itm_index,
                        std::pair<auth_3d_id, int64_t>(elem->second, -1));
            }
        }

        itmpv_reset();
        for (struc_269& i : data.field_2DA08) {
            for (auto& j : i.data) {
                auth_3d_id id = auth_3d_data_load_uid(j.second, aft_auth_3d_db);
                if (!id.check_not_empty())
                    continue;

                id.set_enable(false);
                id.set_repeat(true);
                id.set_paused(false);
                id.set_camera_root_update(false);
                //id.set_object_info(j.first);
                i.auth_3d_ids.push_back(id);
            }
        }

        data.edit_effect.reset();
        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff) {
            data.edit_effect.spr_set_ids.reserve(diff->edit_effect.size());
            data.edit_effect.aet_set_ids.reserve(diff->edit_effect.size());
            data.edit_effect.aet_ids.reserve(diff->edit_effect.size());

            for (const pv_db_pv_edit_effect& i : diff->edit_effect) {
                if (!i.name.size())
                    continue;

                std::string aet_set_name("AET_GAM_EFF");
                aet_set_name.append(i.name);

                std::string spr_set_name("SPR_GAM_EFF");
                spr_set_name.append(i.name);

                uint32_t aet_set_id = aft_aet_db->get_aet_set_id_by_name(aet_set_name.c_str());
                uint32_t spr_set_id = aft_spr_db->get_spr_set_id_by_name(spr_set_name.c_str());
                if (aet_set_id != -1 && spr_set_id != -1) {
                    std::string aet_name(aet_set_name);
                    aet_name.append("_MAIN");

                    uint32_t aet_id = aft_aet_db->get_aet_id_by_name(aet_name.c_str());
                    if (aet_id != -1) {
                        data.edit_effect.spr_set_ids.push_back(spr_set_id);
                        data.edit_effect.aet_set_ids.push_back(aet_set_id);
                        data.edit_effect.aet_ids.push_back(aet_id);
                    }
                }
            }

            for (uint32_t& i : data.edit_effect.spr_set_ids)
                sprite_manager_read_file(i, "", aft_data, aft_spr_db);

            for (uint32_t& i : data.edit_effect.aet_set_ids)
                aet_manager_read_file(i, "", aft_data, aft_aet_db);
        }

        data.obj_set_itmpv.clear();
        if (diff)
            for (const pv_db_pv_item& i : diff->pv_item)
                if (i.index > 0) {
                    object_info obj_info = aft_obj_db->get_object_info(i.name.c_str());
                    if (obj_info.not_null())
                        data.obj_set_itmpv.push_back(obj_info.set_id);
                }

        object_info itmpv_obj_info = aft_obj_db->get_object_info(data.itmpv_string.c_str());
        if (itmpv_obj_info.not_null())
            data.obj_set_itmpv.push_back(itmpv_obj_info.set_id);

        prj::sort_unique(data.obj_set_itmpv);
        for (uint32_t& i : data.obj_set_itmpv)
            object_storage_load_set(aft_data, aft_obj_db, i);

        data.obj_set_handitem.clear();
        if (diff) {
            std::vector<object_info> hand_obj_infos;
            hand_obj_infos.reserve(diff->hand_item.size() * 2);
            data.obj_set_handitem.reserve(diff->hand_item.size());
            for (const pv_db_pv_hand_item& i : diff->hand_item) {
                if (i.index < (data.height_adjust ? 0 : 1))
                    continue;

                const hand_item* hand_item = hand_item_handler_data_get_hand_item(i.id, (chara_index)-1);
                if (hand_item) {
                    hand_obj_infos.push_back(hand_item->obj_left);
                    hand_obj_infos.push_back(hand_item->obj_right);
                    if (hand_item->obj_left.not_null())
                        data.obj_set_handitem.push_back(hand_item->obj_left.set_id);
                    else if (hand_item->obj_right.not_null())
                        data.obj_set_handitem.push_back(hand_item->obj_right.set_id);
                }
            }

            prj::sort_unique(data.obj_set_handitem);
            for (uint32_t& i : data.obj_set_handitem)
                object_storage_load_set(aft_data, aft_obj_db, i);

            skin_param_storage_load(hand_obj_infos, aft_data, aft_obj_db);
        }

        pv_param::post_process_data_load_files(data.pv->id, std::string(data.pv->mdata.dir));
        pv_param::light_data_load_files(data.pv->id, std::string(data.pv->mdata.dir));
        state = 9;
    } break;
    case 9: {
        bool wait_load = task_stage_check_not_loaded();

        for (uint32_t& i : data.spr_set_back_ids)
            if (sprite_manager_load_file(i, aft_spr_db))
                wait_load |= true;

        for (std::string& i : data.auth_3d_categories)
            if (!auth_3d_data_check_category_loaded(i.c_str()))
                wait_load |= true;

        for (uint32_t& i : data.edit_effect.aet_set_ids)
            if (aet_manager_load_file(i, aft_aet_db))
                wait_load |= true;

        for (uint32_t& i : data.edit_effect.spr_set_ids)
            if (sprite_manager_load_file(i, aft_spr_db))
                wait_load |= true;

        for (uint32_t& i : data.obj_set_itmpv)
            if (object_storage_load_obj_set_check_not_read(i)) {
                wait_load |= true;
                break;
            }

        for (uint32_t& i : data.obj_set_itmpv)
            if (object_storage_load_obj_set_check_not_read(i)) {
                wait_load |= true;
                break;
            }

        for (uint32_t& i : data.obj_set_handitem)
            if (object_storage_load_obj_set_check_not_read(i)) {
                wait_load |= true;
                break;
            }


        for (uint32_t& i : data.stgpvhrc_obj_set_ids)
            if (object_storage_load_obj_set_check_not_read(i)) {
                wait_load |= true;
                break;
            }

        if (wait_load)
            break;

        if (data.use_osage_play_data) {
            for (pv_game_chara& i : data.chara)
                if (i.check_chara())
                    osage_play_data_manager_append_chara_motion_ids(i.rob_chr, i.motion_ids);
            osage_play_data_manager_add_task();
        }

        task_stage_get_loaded_stage_infos(data.stage_infos);
        state = 10;
    } break;
    case 10: {
        for (auto& i : data.auth_3d)
            i.second.read_file(aft_auth_3d_db);

        for (struc_269& i : data.field_2DA08)
            for (auth_3d_id& j : i.auth_3d_ids)
                j.read_file(aft_auth_3d_db);

        if (Glitter::glt_particle_manager) {
            Glitter::glt_particle_manager->counter = 0;
            for (pv_effect_resource& i : data.effect_rs_list) {
                uint64_t hash = Glitter::glt_particle_manager->LoadFile(Glitter::FT,
                    aft_data, i.name.c_str(), 0, i.emission, true, aft_obj_db);
                if (hash != hash_fnv1a64m_empty)
                    data.effect_rs_list_hashes.push_back(hash);
            }
        }
        state = 11;
    }
    case 11: {
        bool wait_load = false;

        for (auto& i : data.auth_3d)
            if (!i.second.check_loaded())
                wait_load |= true;

        for (struc_269& i : data.field_2DA08)
            for (auth_3d_id& j : i.auth_3d_ids)
                if (!j.check_loaded())
                    wait_load |= true;

        if (!wait_load)
            state = 12;
    } break;
    case 12: {
        post_process& pp = rctx_ptr->post_process;
        int32_t frame_texture_slot = 0;
        for (const pv_db_pv_frame_texture& i : data.pv->frame_texture) {
            texture* tex = texture_storage_get_texture(aft_tex_db->get_texture_id(i.data.c_str()));
            if (tex)
                pp.frame_texture_load(frame_texture_slot, (post_process_frame_texture_type)i.type, tex);
            frame_texture_slot++;
        }

        /*if (app::TaskWork::CheckTaskReady(sel_main_get()))
            return false;

        if (sub_14013C8C0()->sub_1400E7910() != 3 && get_pv_db_pv()->id != 999)
            data.play_data.ex_song_index = sel_vocal_change_get()->sub_140112C30();*/

        data.play_data.aix = false;
        data.play_data.ogg = false;

        data.play_data.song_name.clear();
        data.play_data.song_name.assign(get_song_name());

        int32_t ex_song_index = -1;
        data.play_data.song_file_name.assign(get_song_file_name(ex_song_index));

        std::string song_file_name = get_play_data_song_file_name();
        if (song_file_name.size() > 3 && aft_data->check_file_exists(song_file_name.c_str())
            && song_file_name.find(".ogg") == song_file_name.size() - 4)
            data.play_data.ogg = true;

        for (pv_game_chara& i : data.chara)
            if (i.check_chara() && !task_rob_manager_check_chara_loaded(i.chara_id))
                return false;

        pv_game_music_get()->ogg_reset();

        if (data.play_data.aix) {
            pv_game_music_get()->include_flags(PV_GAME_MUSIC_AIX);
            pv_game_music_get()->load(2, get_play_data_song_file_name(), 1, sub_1400FCEB0(), false);
        }
        else {
            pv_game_music_get()->include_flags(PV_GAME_MUSIC_OGG);
            pv_game_music_get()->load(4, get_play_data_song_file_name(), 1, sub_1400FCEB0(), false);
        }

        std::string effect_se_file_name = get_effect_se_file_name();
        if (effect_se_file_name.size())
            sound_work_read_farc(effect_se_file_name.c_str());

        if (data.play_data.slidertouch_name.size()) {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "rom/sound/%s.farc", data.play_data.slidertouch_name.c_str());
            sound_work_read_farc(buf);
        }

        if (ex_song_index >= 0 && ex_song_index < get_pv_db_pv()->ex_song.data.size())
            for (const pv_db_pv_ex_song_ex_auth& i : get_pv_db_pv()->ex_song.data[ex_song_index].ex_auth) {
                int32_t org_uid = aft_auth_3d_db->get_uid(i.org_name.c_str());
                int32_t uid = aft_auth_3d_db->get_uid(i.name.c_str());

                for (pv_game_field& j : data.field_data)
                    for (int32_t& k : j.auth_3d_uids)
                        if (k == org_uid) {
                            k = uid;
                            break;
                        }

                for (pv_game_field& j : data.field_data)
                    for (auth_3d_id& k : j.auth_3d_ids)
                        if (k.get_uid() == org_uid) {
                            auto elem = data.auth_3d.find(uid);
                            if (elem != data.auth_3d.end()) {
                                k = elem->second;
                                break;
                            }
                        }

                for (pv_game_field& j : data.field_data) {
                    auto elem = j.auth_3d_frame_list.find(org_uid);
                    if (elem != j.auth_3d_frame_list.end()) {
                        j.auth_3d_frame_list.insert({ uid, elem->second });
                        j.auth_3d_frame_list.erase(elem);
                    }
                }
            }
        state = 13;
    } break;
    case 13: {
        if (pv_osage_manager_array_get_disp() || osage_play_data_manager_check_task_ready())
            return false;

        std::string effect_se_file_name = get_effect_se_file_name();
        if (effect_se_file_name.size())
            if (sound_work_load_farc(effect_se_file_name.c_str()))
                return false;

        if (data.play_data.slidertouch_name.size()) {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "rom/sound/%s.farc", data.play_data.slidertouch_name.c_str());
            if (sound_work_load_farc(buf))
                return false;
        }

        set_chara_use_opd(true);
        task_rob_manager_run_task();

        for (pv_game_chara& i : data.chara)
            if (i.check_chara()) {
                i.rob_chr->set_hand_l_mottbl_motion(0, 194, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
                i.rob_chr->set_hand_r_mottbl_motion(0, 194, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
            }

        const pv_db_pv_difficulty* diff = get_pv_db_pv()->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff && !diff->effect_se_file_name.size())
            for (const std::string& i : diff->effect_se_name_list)
                wave_audio_storage_load_wave_audio(i);

        if (Glitter::glt_particle_manager)
            for (uint64_t& i : data.effect_rs_list_hashes)
                if (Glitter::glt_particle_manager->GetEffectsCount(i)
                    && !Glitter::glt_particle_manager->CheckNoFileReaders(i))
                    return false;
        state = 14;
    } break;
    case 14: {
        //sub_14013C8F0()->sub_14012BDA0();
        state = sub_14013C8C0()->sub_1400E7910() >= 4 ? 17 :15;
    } break;
    case 15: {
        state = 16;
    } break;
    case 16: {
        state = 17;
    } break;
    case 17: {
        data.life_gauge = 127;
        data.life_gauge_final = 127;
        data.play_data.state = 0;
        state = 18;
    } break;
    case 18: {
        if (!data.play_data.load_auth_2d(data.stage_index, data.pv_disp2d))
            return false;

        state = sub_14013C8C0()->sub_1400E7910() >= 4 ? 21 : 19;
    } return false;
    case 19: {
        state = sub_14013C8C0()->sub_1400E7910() != 3 ? 21 : 20;
    } break;
    case 20: {
        state = 21;
    } return false;
    case 21: {
        if (sub_14013C8C0()->sub_1400E7910() < 4) {
            data.reference_score = calculate_reference_score();
            calculate_song_energy();
            /*if (!sub_14013C8C0()->sub_1400E7920())
                sub_14013C8F0()->sub_14012AE60();*/
        }

        rctx_ptr->camera->reset();

        data.score_final = 0;
        data.challenge_time_total_bonus = 0;
        data.combo = 0;
        data.challenge_time_combo_count = 0;
        data.max_combo = 0;
        data.current_reference_score = 0;
        data.target_count = 0;
        data.field_2CF84 = 0;
        data.field_2CF88 = 0;
        data.score_slide_chain_bonus = 0;
        data.slide_chain_length = 0;
        data.field_2CF98 = 0;
        data.field_2D0BC = false;

        for (int32_t& i : data.total_hit_count)
            i = 0;

        for (int32_t& i : data.hit_count)
            i = 0;

        data.play_data.field_5F4 = 1.0f;

        memset(data.play_data.lyric, 0, sizeof(data.play_data.lyric));
        data.play_data.lyric_set = false;
        data.play_data.field_64C = false;

        memset(data.field_2CE9C, 0, sizeof(data.field_2CE9C));
        data.field_2CF1C = 0;
        data.field_2CF20 = false;
        data.field_2CF30 = 4;

        field_1 = false;
        field_2 = false;
        end_pv = false;

        for (pv_game_chara& i : data.chara)
            if (i.check_chara())
                i.rob_chr->set_visibility(true);

        field_4 = false;
        if (sub_14013C8C0()->sub_1400E7910() == 3)
            data.field_2CF8C = 30.0f;
        data.field_2CF9C = 0;
        data.field_2CFA0 = 0;
        data.field_2CFA4 = 0;
        sub_14013C8C0()->field_26 = false;
        data.title_image_init = false;
        data.field_index = 1;
        data.edit_effect_index = 0;
        data.slidertouch_counter = 0;
        data.change_field_branch_success_counter = 10;
        if (data.pv_data.field_0) {
            if (sub_14013C8C0()->sub_1400E7910() < 4)
                data.pv_data.get_target_target_count(0, 0.0f, true);
        }
        rctx_ptr->disp_manager.object_culling = false;
        state = 22;
    } break;
    case 22: {
        if (data.field_index >= data.field_data.size()
            && data.edit_effect_index >= data.edit_effect.aet_ids.size()
            && data.slidertouch_counter >= 32
            && data.change_field_branch_success_counter <= 0
            && data.title_image_init) {
            if (data.pv->pre_play_script) {
                data.change_field_branch_success_counter = 0;
                state = 23;
            }
            else
                state = 25;
            break;
        }

        float_t view_point_xz_pos_angle =
            (float_t)(36 * get_frame_counter() % 360) * DEG_TO_RAD_FLOAT;
        vec3 view_point;
        view_point.x = sinf(view_point_xz_pos_angle) * 40.0f;
        view_point.y = 0.0f;
        view_point.z = cosf(view_point_xz_pos_angle) * 40.0f;
        rctx_ptr->camera->set_view_point(view_point);
        rctx_ptr->camera->set_interest({ 0.0f, 0.0f, 0.0f });

        data.change_field_branch_success_counter--;
        if (data.field_index < data.field_data.size())
            change_field(data.field_index, -1, -1);

        while (data.field_index < data.field_data.size()) {
            int32_t stage_index = data.field_data[data.field_index].stage_index;
            if (++data.field_index >= data.field_data.size())
                break;
            else if (stage_index != data.field_data[data.field_index].stage_index)
                break;
        }

        if (data.edit_effect_index < data.edit_effect.aet_ids.size()) {
            edit_effect_set(data.edit_effect_index, 1.0f, (float_t)(1.0 / 60.0));
            data.edit_effect_index++;
        }

        if (!data.title_image_init) {
            data.title_image_init = true;
            title_image_ctrl(true);
        }

        if (data.slidertouch_counter < 32) {
            if (data.play_data.slidertouch_name.size()) {
                char buf[0x200];
                std::string name(buf, sprintf_s(buf, sizeof(buf), "%s_%02d",
                    data.play_data.slidertouch_name.c_str(), data.slidertouch_counter + 1));
                wave_audio_storage_load_wave_audio(name);
            }
            else {
                data.slidertouch_counter += 32;
            }
            data.slidertouch_counter++;
        }
    } break;
    case 23: {
        if (data.change_field_branch_success_counter >= data.pv_data.change_field_branch_success.size()) {
            state = 24;
            break;
        }

        int32_t curr_time = data.pv_data.change_field_branch_success
            [data.change_field_branch_success_counter].first;
        if (data.change_field_branch_success_counter > 0) {
            int32_t prev_time = data.pv_data.change_field_branch_success
                [data.change_field_branch_success_counter - 1LL].first;
            if (curr_time > prev_time)
                for (int64_t i = 60LL * ((int64_t)curr_time - prev_time) / 100000; i; i--)
                    Glitter::glt_particle_manager->CtrlScenes();
        }
        data.pv_data.ctrl((float_t)(1.0 / 60.0), 10000LL * curr_time, false);
        data.change_field_branch_success_counter++;
    } break;
    case 24: {
        data.reset();
        data.play_data.sub_140137BE0();
        set_lyric(-1, 0xFFFFFFFF);

        for (pv_game_chara& i : data.chara) {
            if (!i.check_chara())
                continue;

            rob_chara* rob_chr = i.rob_chr;
            rob_chr->set_visibility(true);
            if (rob_chr->check_for_ageageagain_module()) {
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
            }
            rob_chr->set_hand_l_mottbl_motion(0, 194, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
            rob_chr->set_hand_r_mottbl_motion(0, 194, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        }

        for (int32_t& i : data.campv_auth_3d_uids) {
            const char* name = auth_3d_data_get_uid_name(i, aft_auth_3d_db);
            if (!name || !check_chrcam(name, i))
                continue;

            auto elem = data.auth_3d.find(i);
            if (elem != data.auth_3d.end())
                elem->second.set_enable(false);
        }

        itmpv_reset();
        sub_140113730();
        data.current_field = 0;

        for (pv_game_field& i : data.field_data) {
            for (int32_t& j : i.auth_3d_uids) {
                auth_3d_id id = get_auth_3d_id(j);
                id.set_enable(false);
                id.set_visibility(false);
                id.set_paused(false);
                id.set_req_frame(0.0f);
            }

            if (i.light_auth_3d_id.check_not_empty()) {
                i.light_auth_3d_id.set_enable(false);
                i.light_auth_3d_id.set_visibility(false);
                i.light_auth_3d_id.set_paused(false);
                i.light_auth_3d_id.set_req_frame(0.0f);
            }
        }

        pv_expression_array_reset_data();
        state = 25;
    } break;
    case 25: {
        //sub_14013C8F0->sub_14012BDA0();
        rand_state_array_4_set_seed_1393939();
        reset_field();
        data.edit_effect.data.reset();
        data.play_data.init_aet_edit_effect(-1, "eff", false, false);
        data.play_data.free_aet_title_image();
        data.title_image_state = 0;
        Glitter::glt_particle_manager->counter = 0;
        rctx_ptr->disp_manager.object_culling = true;
        loaded = true;
        state = false;
        //sel_vocal_change_get()->DelTask();
        rctx_ptr->render_manager.set_effect_texture(0);
        pv_param_task::post_process_task_add_task();
    } return true;
    }
    return false;
}

void pv_game::play_se(int32_t index) {
    const pv_db_pv_difficulty* diff = data.pv->get_difficulty(sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff && diff->effect_se_file_name.size()
        && index >= 0 && index < diff->effect_se_name_list.size()
        && diff->effect_se_name_list[index].size()) {
        sound_work_play_se(1, diff->effect_se_name_list[index].c_str(), 1.0f);
        data.se_index = index;
    }
}

void pv_game::play_data_ctrl(float_t delta_time) {
    bool fade_end = data.play_data.fade_end_ctrl();
    data.play_data.frame_ctrl();
    data.play_data.ui_set_disp();

    switch (data.appear_state) {
    case 0:
        if (data.start_fade) {
            data.appear_state = 1;
            break;
        }

        if (sub_14013C8C0()->sub_1400E7920()) {
            data.appear_state = 4;
            data.appear_time = 0.0f;
            data.appear_duration = 0.0f;
        }
        else {
            data.appear_state = 3;
            data.appear_time = 0.0f;
            data.appear_duration = 0.0f;

            if (!data.pv_data.field_2BFD5)
                data.play_data.sub_140137F80(true, 0.0f);
        }
        break;
    case 1:
        if (!fade_end)
            return;

        if (sub_14013C8C0()->sub_1400E7920()) {
            data.appear_state = 4;
            data.appear_time = 0.0f;
            data.appear_duration = 0.0f;
        }
        else {
            data.appear_state = 2;
            data.appear_time = 0.0f;
            data.appear_duration = 2.0f;

            if (!data.pv_data.field_2BFD5)
                data.play_data.sub_140137F80(true, 2.0f);
        }
        break;
    case 2:
        data.appear_time += delta_time;
        if (data.appear_time >= data.appear_duration) {
            data.appear_state = 3;
            data.appear_time = 0.0f;
            data.appear_duration = 0.0f;
        }
        break;
    case 3:
        data.play_data.ctrl(delta_time);
        data.play_data.score_update(data.score_final, false);
        break;
    }
}

void pv_game::reset() {
    loaded = false;
    field_1 = false;
    field_2 = false;
    end_pv = false;
    field_4 = false;
    field_14 = false;

    data.play_data.field_5F4 = 1.0f;

    memset(data.play_data.lyric, 0, sizeof(data.play_data.lyric));
    data.play_data.lyric_set = false;
    data.play_data.lyric_color = 0xFFFFFFFF;

    memset(data.field_2CE9C, 0, sizeof(data.field_2CE9C));
    data.field_2CF1C = 0;
    data.field_2CF20 = false;
    data.field_2CF8C = 0.0f;
    data.field_2CF9C = 0;
    data.field_2CFA0 = 0;
    data.field_2CFA4 = 0;

    data.play_data.field_3B0 = -1;
    data.play_data.field_3B4 = -1;
    data.play_data.field_3B8 = -1;
    data.play_data.field_3BC = -1;

    pv_id = -1;

    data.field_2CFB0 = 0;
    data.chance_point = 0;

    for (pv_game_chara& i : data.chara) {
        i.chara_index = CHARA_MAX;
        i.cos = -1;
        i.chara_id = -1;
        i.pv_data = {};
        i.rob_chr = 0;
        i.motion_ids.clear();
        i.motion_face_ids.clear();
    }

    data.motion_set_ids.clear();
    data.motion_set_face_ids.clear();

    data.obj_set_itmpv.clear();
    data.obj_set_handitem.clear();
    data.chreff_auth_3d_obj_set_ids.clear();
    data.field_2D770.clear();
    data.stgpvhrc_obj_set_ids.clear();
    data.ex_song_ex_auth_obj_set_ids.clear();

    data.stage_indices.clear();
    data.stage_infos.clear();

    data.field_2D7E8 = false;
    data.spr_set_back_ids.clear();

    data.field_2D808 = -1;

    data.edit_effect.reset();

    data.current_field = 0;
    data.field_data.clear();

    data.field_2D8A0 = true;

    data.camera_auth_3d_uid = -1;
    data.campv_string.clear();
    data.campv_auth_3d_uids.clear();
    data.campv_index = 0;
    data.campv.clear();

    data.itmpv_string.clear();
    data.itmpv_auth_3d_uids.clear();
    for (auto& i : data.itmpv)
        i.clear();
    data.itmpv_uids.clear();

    for (auto& i : data.field_2DA08) {
        i.data.clear();
        i.auth_3d_ids.clear();
        i.field_30 = -1;
    }

    data.auth_3d.clear();
    data.field_2DAC8 = false;
    data.auth_3d_categories.clear();
    data.loaded_auth_3d_uids.clear();

    data.auth_3d_time.clear();
    data.light_time.clear();

    for (auto& i : data.aet_time)
        i.clear();

    data.chreff_auth_3d.clear();
    data.chreff_auth_3d_obj.clear();

    data.reference_score = 0;
    data.reference_score_no_flag = 0;
    data.reference_score_no_flag_life_bonus = 0;
    data.target_reference_score.clear();
    data.field_2CFE0 = 0;
    data.disp_lyrics_now = data.disp_lyrics;
    data.notes_passed = 0;
    data.field_2CFF0 = 0;
    data.song_energy = 0.0f;
    data.song_energy_border = 0.0f;
    data.max_time_float = 0.0f;
    data.current_time_float = 0.0f;
    data.max_time = 0;
    data.current_time = 0;
    data.field_2D038 = 0.0f;
    data.field_2D03C = 0.0f;
    data.score_hold_multi = 0;
    data.score_hold = 0;
    data.score_slide = 0;
    data.has_slide = false;
    data.has_success_se = false;
    data.pv_disp2d = 0;
    data.hit_border = false;
    data.field_2D05D = false;
    data.start_fade = true;
    data.title_image_init = false;
    data.field_index = 0;
    data.edit_effect_index = -1;
    data.slidertouch_counter = 0;
    data.change_field_branch_success_counter = 0;
    data.field_2D05E = false;
    data.field_2D080 = 0;
    data.field_2D084 = 0;
    data.life_gauge_bonus = 0;
    data.life_gauge_total_bonus = 0;
    data.field_2D090 = 0;
    data.no_clear = true;
    data.field_2D093 = false;
    data.field_2D094 = false;
    data.field_2D095 = false;
    data.field_2D096 = false;
    data.use_osage_play_data = false;
    data.pv_end_fadeout = false;
    data.title_image_state = 0;
    data.field_2D09C = 0;
    data.rival_percentage = 0.0f;
    data.play_data.se_name.clear();
    data.play_data.pvbranch_success_se_name.clear();
    data.play_data.slide_name.clear();
    data.play_data.chainslide_first_name.clear();
    data.play_data.chainslide_sub_name.clear();
    data.play_data.chainslide_success_name.clear();
    data.play_data.chainslide_failure_name.clear();
    data.play_data.slidertouch_name.clear();
    data.play_data.song_file_name.clear();
    data.play_data.ex_song_index = -1;
    data.height_adjust = false;
    data.field_2DB2C = 0;
    data.play_data.aix = false;
    data.play_data.ogg = false;
    data.play_data.field_64C = false;
    data.field_2CF30 = 4;
    data.field_2CF34 = 0;
    data.play_data.field_650 = 43;
    data.play_data.field_654 = 0.0f;
    data.appear_state = 3;
    data.appear_time = 0.0f;
    data.field_2CF38 = 0;
    data.play_data.field_658 = 0;
    data.play_data.field_65C = 0.0f;
    data.appear_duration = 0.0f;
    data.target_count = 0;
    data.field_2CF84 = 0;
    data.current_reference_score = 0;
    data.combo = 0;
    data.challenge_time_combo_count = 0;
    data.life_gauge = 127;
    data.score_final = 0;
    data.life_gauge_final = 127;
    data.max_combo = 0;
    data.challenge_time_total_bonus = 0;
    data.field_2CF88 = 0;
    data.pv = 0;
    data.score_slide_chain_bonus = 0;
    data.slide_chain_length = 0;
    data.field_2D0BC = false;
    data.next_stage = false;
    data.has_frame_texture = false;
    data.field_2D0C0 = -1;

    data.field_2CE98 = 0;

    for (int32_t& i : data.total_hit_count)
        i = 0;

    for (int32_t& i : data.hit_count)
        i = 0;

    data.changed_field = 0;

    data.has_auth_3d_frame = false;
    data.has_light_frame = false;

    for (bool& i : data.has_aet_frame)
        i = false;

    for (bool& i : data.has_aet_list)
        i = false;

    data.success = data.play_success;
    data.field_2CE98 = 0;
    data.se_index = -1;
    data.field_2DB34 = 0;
    data.field_2D0A8 = 0;
    data.field_2D0AC = 0;
    data.field_2D0B0[0] = 0;
    data.field_2D0B0[1] = 0;
    data.field_2D0B0[2] = 0;

    data.effect_rs_list.clear();
    data.effect_rs_list_hashes.clear();

    data.task_effect_init = false;
}

void pv_game::reset_appear() {
    data.appear_state = 0;
    data.appear_time = 0.0f;
    data.appear_duration = 0.0f;
}

void pv_game::reset_field() {
    task_stage_info().set_stage();

    if (data.current_field) {
        pv_game_field& curr_field_data = data.field_data[data.current_field];

        for (auth_3d_id& i : curr_field_data.auth_3d_ids)
            i.set_enable(false);

        if (curr_field_data.light_auth_3d_id.check_not_empty())
            curr_field_data.light_auth_3d_id.set_enable(false);

        data.current_field = 0;
    }

    data.play_data.unload_aet();
    data.play_data.set_spr_set_back_id(-1);

    data.auth_3d_time.clear();
    data.light_time.clear();

    for (auto& i : data.aet_time)
        i.clear();

    Glitter::glt_particle_manager->FreeScenes();
}

void pv_game::set_chara_use_opd(bool value) {
    if (!data.use_osage_play_data)
        return;

    for (pv_game_chara& i : data.chara)
        if (i.chara_id != -1 && i.rob_chr)
            i.rob_chr->set_use_opd(value);
}

void pv_game::set_data_campv(int32_t type, int32_t index, float_t frame) {
    auto elem = data.campv.find(index);
    if (elem == data.campv.end())
        return;

    int32_t campv_index = data.campv_index;
    if (campv_index) {
        auto elem = data.campv.find(campv_index);
        if (elem != data.campv.end())
            elem->second.set_enable(false);
    }
    data.campv_index = index;

    auth_3d_id& id = elem->second;
    id.set_enable(true);
    data.camera_auth_3d_uid = id.get_uid();
    if (type == 0 || type == 1) {
        id.set_req_frame(frame);
        id.set_paused(false);
    }
}

void pv_game::set_data_itmpv(int32_t chara_id, int32_t index, bool enable, int64_t time) {
    auto elem = data.itmpv[chara_id].find(index);
    if (elem == data.itmpv[chara_id].end())
        return;

    auth_3d_id& id = elem->second.first;
    if (enable) {
        id.set_enable(true);
        id.set_paused(false);
        id.set_req_frame(0.0f);
        elem->second.second = time;
    }
    else {
        id.set_enable(false);
        elem->second.second = -1;
    }
}

void pv_game::set_data_itmpv_chara_id(int32_t chara_id, int32_t index, bool attach) {
    auto elem = data.itmpv[chara_id].find(index);
    if (elem != data.itmpv[chara_id].end())
        elem->second.first.set_chara_id(attach ? chara_id : -1);
}

void pv_game::set_data_itmpv_req_frame(int32_t chara_id, int32_t index, float_t value) {
    auto elem = data.itmpv[chara_id].find(index);
    if (elem != data.itmpv[chara_id].end())
        elem->second.first.set_req_frame(value);
}

void pv_game::set_data_itmpv_max_frame(int32_t chara_id, float_t value) {
    for (auto& i : data.itmpv[chara_id])
        i.second.first.set_max_frame(value);
}

void pv_game::set_data_itmpv_max_frame(int32_t chara_id, int32_t index, float_t value) {
    auto elem = data.itmpv[chara_id].find(index);
    if (elem != data.itmpv[chara_id].end())
        elem->second.first.set_max_frame(value);
}

void pv_game::set_data_itmpv_visibility(int32_t chara_id, bool value) {
    for (auto& i : data.itmpv[chara_id])
        i.second.first.set_visibility(value);
}

void pv_game::set_data_itmpv_visibility(int32_t chara_id, int32_t index, bool value) {
    auto elem = data.itmpv[chara_id].find(index);
    if (elem != data.itmpv[chara_id].end())
        elem->second.first.set_visibility(value);
}

void pv_game::set_eyes_adjust(pv_game_chara* chr) {
    chr->pv_data.eyes_adjust.xrot_adjust = data.pv->eyes_xrot_adjust;
    if (data.pv->eyes_base_adjust_type != EYES_BASE_ADJUST_NONE)
        chr->pv_data.eyes_adjust.base_adjust = data.pv->eyes_base_adjust_type;

    auto elem = data.pv->eyes_rot_rate.find(chr->chara_index);
    if (elem != data.pv->eyes_rot_rate.end()) {
        chr->pv_data.eyes_adjust.neg = elem->second.xn_rate;
        chr->pv_data.eyes_adjust.pos = elem->second.xp_rate;
    }
}

void pv_game::set_item(size_t performer, rob_chara_pv_data_item& value) {
    items[performer] = value;
}

void pv_game::set_item_mask(size_t performer, size_t item, bool value) {
    items_mask[performer].arr[item] = value;
}

void pv_game::set_lyric(int32_t lyric_index, color4u8 lyric_color) {
    memset(data.play_data.lyric, 0, sizeof(data.play_data.lyric));
    data.play_data.lyric_set = false;

    data.play_data.lyric_color = sub_14013C8C0()->sub_1400E7910() < 4 ? lyric_color : 0xFFFFFFFF;

    if (lyric_index < 0)
        return;

    const pv_db_pv_lyric* lyric = 0;
    for (const pv_db_pv_lyric& i : data.pv->lyric)
        if (i.index == lyric_index) {
            lyric = &i;
            break;
        }
    data.play_data.lyric_set = !!lyric;

    if (lyric)
        strcpy_s(data.play_data.lyric, sizeof(data.play_data.lyric), lyric->data.c_str());
}

void pv_game::set_module(size_t performer, int32_t module) {
    modules[performer] = module;
}

void pv_game::set_osage_init(const pv_game_chara& chr) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const pv_db_pv* pv = data.pv;

    std::vector<osage_init_data> vec;
    vec.push_back(osage_init_data(chr.rob_chr));

    for (const uint32_t& i : chr.motion_ids) {
        osage_init_data osage_init;
        osage_init.rob_chr = chr.rob_chr;
        osage_init.pv_id = pv->id;
        osage_init.motion_id = i;
        osage_init.frame = -1;
        vec.push_back(osage_init);
    }

    const std::vector<pv_data_set_motion>* set_motion = data.pv_data.get_set_motion(chr.chara_id);
    if (set_motion)
        for (const pv_data_set_motion& i : *set_motion) {
            float_t frame = i.frame_stage_index.first;
            if (frame > 0.0f)
                frame = (float_t)(int32_t)(frame + 0.5f);
            else if (frame < 0.0f)
                frame = (float_t)(int32_t)(frame - 0.5f);

            osage_init_data osage_init;
            osage_init.rob_chr = chr.rob_chr;
            osage_init.pv_id = pv->id;
            osage_init.motion_id = i.motion_id;
            osage_init.frame = (int32_t)frame;
            vec.push_back(osage_init);
        }

    for (const pv_db_pv_osage_init& i : pv->osage_init) {
        uint32_t motion_id = aft_mot_db->get_motion_id(i.motion.c_str());
        if (motion_id == -1)
            continue;

        osage_init_data osage_init;
        osage_init.rob_chr = chr.rob_chr;
        osage_init.pv_id = pv->id;
        osage_init.motion_id = motion_id;
        osage_init.path.assign(pv->mdata.dir);
        osage_init.frame = i.frame;
        vec.push_back(osage_init);
    }

    skin_param_manager_add_task(chr.chara_id, vec);
}

bool pv_game::set_pv_param_post_process_bloom_data(bool set, int32_t id, float_t duration) {
    pv_param::bloom& bloom = pv_param::post_process_data_get_bloom_data(id);
    if (set)
        pv_param_task::post_process_task_set_bloom_data(bloom, duration);
    return true;
}

void pv_game::set_pv_param_post_process_chara_alpha_data(int32_t chara_id,
    float_t alpha, int32_t type, float_t duration) {
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
        pv_param_task::post_process_task_set_chara_alpha(
            chara_id, type, alpha, duration * 2.0f);
}

void pv_game::set_pv_param_post_process_chara_item_alpha_data(int32_t chara_id,
    float_t alpha, int32_t type, float_t duration) {
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
        pv_param_task::post_process_task_set_chara_item_alpha(
            chara_id, type, alpha, duration * 2.0f,
            pv_game::chara_item_alpha_callback, this);
}

bool pv_game::set_pv_param_post_process_color_correction_data(bool set, int32_t id, float_t duration) {
    pv_param::color_correction& color_correction = pv_param::post_process_data_get_color_correction_data(id);
    if (set)
        pv_param_task::post_process_task_set_color_correction_data(color_correction, duration);
    return true;
}

bool pv_game::set_pv_param_post_process_dof_data(bool set, int32_t id, float_t duration) {
    pv_param::dof& dof = pv_param::post_process_data_get_dof_data(id);
    if (set)
        pv_param_task::post_process_task_set_dof_data(dof, duration);
    return true;
}

void pv_game::title_image_ctrl(bool dont_wait) {
    const pv_db_pv* pv = data.pv;
    if (!pv)
        return;

    const pv_db_pv_difficulty* diff = pv->get_difficulty(sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (!diff)
        return;

    int32_t& state = data.title_image_state;
    switch (data.title_image_state) {
    case 0:
        if (sub_14013C8C0()->sub_1400E7910() == 3 || diff->title_image.time == -1.0) {
            state = 3;
            break;
        }

        if (dont_wait) {
            if (diff->title_image.aet_name.size())
                data.play_data.init_aet_title_image(diff->title_image.aet_name.c_str());
            state = 3;
            break;
        }
    case 1:
        if (data.current_time_float - diff->title_image.time < 0.0f)
            break;

        if (!diff->title_image.aet_name.size()) {
            state = 3;
            break;
        }

        data.play_data.init_aet_title_image(diff->title_image.aet_name.c_str());
        if (diff->title_image.end_time >= 0.0f)
            state = 2;
        else
            state = 3;
        break;
    case 2:
        if (diff->title_image.end_time <= data.current_time_float) {
            data.play_data.free_aet_title_image();
            state = 3;
        }
        break;
    }
}

bool pv_game::unload() {
    data_struct* aft_data = &data_list[DATA_AFT];
    aet_database* aft_aet_db = &aft_data->data_ft.aet_db;
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

    switch (data.field_2D09C) {
    case 0: {
        pv_game_music_get()->stop();

        /*for (int32_t i = 0; i < 2; i++)
            if (app::TaskWork::CheckTaskReady(task_movie_get()))
                sub_14041F4E0(task_movie_get());*/

        std::string effect_se_file_name = get_effect_se_file_name();
        if (effect_se_file_name.size())
            sound_work_unload_farc(effect_se_file_name.data());

        if (data.play_data.slidertouch_name.size()) {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "rom/sound/%s.farc", data.play_data.slidertouch_name.c_str());
            sound_work_unload_farc(buf);
        }

        wave_audio_storage_clear();

        if (data.play_data.aix)
            pv_game_music_get()->exclude_flags(PV_GAME_MUSIC_AIX);

        pv_game_music_get()->set_channel_pair_volume_map(0, 100);
        pv_game_music_get()->set_channel_pair_volume_map(1, 100);

        data.field_2CF30 = 4;

        post_process& pp = rctx_ptr->post_process;
        int32_t frame_texture_slot = 0;
        for (const pv_db_pv_frame_texture& i : data.pv->frame_texture) {
            if (i.data.size()) {
                texture* tex = texture_storage_get_texture(aft_tex_db->get_texture_id(i.data.c_str()));
                if (tex)
                    pp.frame_texture_unload(frame_texture_slot, tex);
            }
            frame_texture_slot++;
        }

        if (data.has_frame_texture) {
            rctx_ptr->post_process.frame_texture_free();
            data.has_frame_texture = false;
        }

        data.pv_data.reset();

        if (data.play_data.ogg)
            pv_game_music_get()->exclude_flags(PV_GAME_MUSIC_OGG);

        data.field_2D09C = 1;
    }
    case 1:
        pv_osage_manager_array_set_not_reset_true();
        if (pv_osage_manager_array_get_disp())
            return false;

        data.field_2D09C = 2;
        break;
    }

    for (pv_game_chara& i : data.chara) {
        if (i.chara_id != -1 && i.rob_chr) {
            skin_param_manager_reset(i.chara_id);
            rob_chara_array_free_chara_id(i.chara_id);
        }

        i.chara_index = CHARA_MAX;
        i.cos = -1;
        i.chara_id = -1;
        i.pv_data = {};
        i.rob_chr = 0;
        i.motion_ids.clear();
        i.motion_face_ids.clear();
    }

    osage_play_data_manager_reset();

    for (uint32_t& i: data.motion_set_ids) {
        motion_set_unload_motion(i);
        motion_set_unload_mothead(i);
    }
    data.motion_set_ids.clear();

    for (uint32_t& i : data.motion_set_face_ids)
        motion_set_unload_motion(i);
    data.motion_set_face_ids.clear();

    pv_expression_file_unload(data.pv->pv_expression_file_name.c_str());

    data.stage_indices.clear();
    data.stage_infos.clear();

    task_stage_unload_task();

    if (data.field_2D7E8)
        for (uint32_t& i : data.spr_set_back_ids)
            sprite_manager_unload_set(i, aft_spr_db);

    data.field_2D7E8 = false;
    data.spr_set_back_ids.clear();

    data.current_field = 0;
    data.field_data.clear();

    data.field_2D8A0 = true;

    for (auto& i : data.auth_3d)
        i.second.unload(rctx_ptr);

    for (auto& i : data.field_2DA08)
        for (auto& j : i.auth_3d_ids)
            j.unload(rctx_ptr);

    if (data.field_2DAC8)
        for (std::string& i : data.auth_3d_categories)
            auth_3d_data_unload_category(i.c_str());

    data.camera_auth_3d_uid = -1;
    data.campv_string.clear();
    data.campv_auth_3d_uids.clear();
    data.campv_index = 0;
    data.campv.clear();

    data.itmpv_string.clear();
    data.itmpv_auth_3d_uids.clear();
    for (auto& i : data.itmpv)
        i.clear();
    data.itmpv_uids.clear();

    for (auto& i : data.field_2DA08) {
        i.data.clear();
        i.auth_3d_ids.clear();
        i.field_30 = -1;
    }

    data.auth_3d.clear();
    data.field_2DAC8 = false;
    data.auth_3d_categories.clear();
    data.loaded_auth_3d_uids.clear();

    data.auth_3d_time.clear();
    data.light_time.clear();

    for (auto& i : data.aet_time)
        i.clear();

    data.chreff_auth_3d.clear();
    data.chreff_auth_3d_obj.clear();

    data.edit_effect.unload();

    pv_param::light_data_clear_data();
    pv_param::post_process_data_clear_data();

    for (uint32_t& i : data.obj_set_itmpv)
        object_storage_unload_set(i);
    data.obj_set_itmpv.clear();

    for (uint32_t& i : data.obj_set_handitem)
        object_storage_unload_set(i);
    data.obj_set_handitem.clear();

    skin_param_storage_reset();

    for (uint32_t& i : data.chreff_auth_3d_obj_set_ids)
        object_storage_unload_set(i);
    data.chreff_auth_3d_obj_set_ids.clear();

    for (uint32_t& i : data.field_2D770)
        object_storage_unload_set(i);
    data.field_2D770.clear();

    for (uint32_t& i : data.stgpvhrc_obj_set_ids)
        object_storage_unload_set(i);
    data.stgpvhrc_obj_set_ids.clear();

    for (uint32_t& i : data.ex_song_ex_auth_obj_set_ids)
        object_storage_unload_set(i);
    data.ex_song_ex_auth_obj_set_ids.clear();

    for (uint64_t& i : data.effect_rs_list_hashes)
        Glitter::glt_particle_manager->UnloadEffectGroup(i);
    data.effect_rs_list.clear();
    data.effect_rs_list_hashes.clear();

    /*for (int32_t i = 0; i < 2; i++) {
        if (!app::TaskWork::CheckTaskReady(task_movie_get(i)))
            continue;

        task_movie_get(i)->sub_14041F4E0();
        task_movie->DelTask();
    }*/

    data.title_image_state = 0;

    //stage_param_data_coli_data_clear();

    data.play_data.field_3B0 = -1;
    data.play_data.field_3B4 = -1;
    data.play_data.field_3B8 = -1;
    data.play_data.field_3BC = -1;

    //sub_14013C8F0()->sub_14012BDA0();

    data.play_data.unload();

    data.play_data.field_64C = false;

    field_1 = false;
    field_2 = false;
    end_pv = false;

    sub_14013C8C0()->field_26 = false;

    struc_14* v85 = sub_14013C8C0();
    v85->field_14 = 0;
    v85->field_18 = 0;
    v85->field_1C = 0;
    v85->field_20 = 1.0f;

    sub_14013C8C0()->field_24 = false;
    sub_14013C8C0()->field_25 = false;

    loaded = false;

    data.target_reference_score.clear();
    data.notes_passed = 0;
    data.field_2CFF0 = 0;
    data.field_2CFE0 = 0;
    data.field_2D0C0 = -1;

    light_param_data_storage_data_reset();

    post_process& pp = rctx_ptr->post_process;
    pp.tone_map->set_saturate_coeff(1.0f);
    pp.dof->set_dof_pv();
    pp.tone_map->set_scene_fade(0.0f);

    /*if (!sub_140192E20())
        post_process_set_taa(&pp, 1);*/

    //sub_1404A9480(&pp, 0, -1);

    rctx_ptr->disp_manager.object_culling = true;
    rctx_ptr->render_manager.shadow_ptr->range = 1.0f;
    rctx_ptr->render_manager.set_effect_texture(0);

    sound_work_reset_all_se();

    pv_param_task::post_process_task_del_task();

    data.current_time_float = 0.0f;
    data.current_time = 0;
    return true;
}

void pv_game::sub_1400FC500() {
    if (data.field_2D095) {
        if (data.hit_count[0] + data.hit_count[1] == data.target_count)
            data.field_2CE98 = 5;
        else if (data.song_energy >= get_percentage_clear_excellent())
            data.field_2CE98 = 4;
        else if (data.song_energy >= get_percentage_clear_great())
            data.field_2CE98 = 3;
        else if (data.song_energy > 40.0f || data.song_energy_base <= data.song_energy)
            data.field_2CE98 = 2;
        else
            data.field_2CE98 = 1;

        if (!data.life_gauge && data.no_fail)
            data.field_2CE98 = 0;
    }
    data.field_2CE98 = 0;

    /*if (!sub_14013C8C0()->sub_1400E7920() && sub_14013C8C0()->sub_1400E7910() != 6
        && (!data.field_2CE98 || data.song_energy_base > data.song_energy)
        && !data.no_fail && sub_14019C750() && sub_14038BB30()->field_0.stage_index < sub_14038AEE0() - 1)
        data.next_stage = true;*/
}

void pv_game::sub_1400FC6F0() {
    disp_song_name();

    if (!data.play_data.field_64C && sub_14013C8C0()->sub_1400E7910() != 3 && data.play_data.lyric_set && data.disp_lyrics_now) {
        bool h_center = sub_14013C8C0()->sub_1400E7910() == 2 || sub_14013C8C0()->sub_1400E7910() == 5;
        data.play_data.disp_lyric(data.play_data.lyric, h_center, this->data.play_data.lyric_color);
    }
}

int32_t pv_game::sub_1400FC780(float_t delta_time) {
    data.play_data.field_65C += delta_time;
    data.play_data.skin_danger_ctrl();

    switch (data.play_data.field_650) {
    case 0:
        /*if (!this->data.pv_end_fadeout)
            task_mask_screen_fade_out(0.75f, false);*/

        data.play_data.field_650 = 1;
        data.play_data.field_654 = 1.0f;
        data.play_data.field_65C = 0.0;
        return 0;
    case 1:
        if (data.play_data.field_654 * 0.95f > data.play_data.field_65C)
            return 0;

        data.field_2D093 = true;
        break;
    case 6:
        if (data.play_data.field_654 > data.play_data.field_65C)
            return 0;

        data.play_data.field_650 = 34;
        data.play_data.field_654 = 0.0f;
        data.play_data.field_65C = 0.0f;
        return 0;
    case 27:
        data.play_data.init_aet_next_info();

        data.play_data.field_650 = 28;
        data.play_data.field_654 = 1.0f;
        data.play_data.field_65C = 0.0f;
        return 0;
    case 28:
    case 29:
        if (data.pv_data.pv_end && !data.field_2D05E) {
            data.field_2D05E = true;
            //task_mask_screen_fade_out(0.75f, 0);
        }

        rctx_ptr->post_process.tone_map->set_exposure(0.0f);

        if (data.play_data.get_aet_next_info_disp())
            return 0;

        data.field_2D093 = true;
        if (data.field_2D05E)
            break;

        data.field_2D05E = true;
        //task_mask_screen_fade_out(0.0f, 0);
        break;
    case 31:
        if (data.play_data.field_654 > data.play_data.field_65C)
            return 0;

        data.play_data.field_650 = 32;
        data.play_data.field_654 = 1.0f;
        data.play_data.field_65C = 0.0f;
        return 0;
    case 41:
        /*if (!data.pv_end_fadeout)
            task_mask_screen_fade_out(1.0, 0);*/

        data.play_data.field_650 = 42;
        data.play_data.field_654 = 1.1f;
        data.play_data.field_65C = 0.0f;
        return 0;
    case 42:
        if (data.play_data.field_654 > data.play_data.field_65C)
            return 0;

        data.field_2D093 = true;
        if (sub_14013C8C0()->sub_1400E7910() == 3 && data.field_2CF8C > 0.0f)
            return 3;
        break;
    }
    return 1;
}

float_t pv_game::sub_1400FCEB0() {
    return max_def(data.play_data.field_5EC, 0.0f);
}

int32_t pv_game::sub_1400FDD80() {
    if (data.target_reference_score.size())
        return (int32_t)(data.target_reference_score.size() - data.field_2CFE0 - 1);
    return 0;
}

void pv_game::sub_140104FB0() {
    data.se_index = -1;
    data.success = data.play_success;
    data.field_2CE98 = 0;
    data.field_2DB34 = 0;
    data.field_2D0A8 = 0;
    data.field_2D0AC = 0;

    for (int32_t& i : data.field_2D0B0)
        i = 0;
}

void pv_game::sub_140106640() {
    pv_game_time_pause();

    pv_game_music_get()->stop();
    pv_game_music_get()->set_volume_map(0, 0);

    if (data.play_data.aix) {
        pv_game_music_get()->include_flags(PV_GAME_MUSIC_AIX);
        pv_game_music_get()->load(2, get_play_data_song_file_name(), true, sub_1400FCEB0(), false);
    }
    else {
        pv_game_music_get()->include_flags(PV_GAME_MUSIC_OGG);
        pv_game_music_get()->load(4, get_play_data_song_file_name(), true, sub_1400FCEB0(), false);
    }

    /*for (int32_t i = 0; i < 2; i++) {
        if (!app::TaskWork::CheckTaskReady(task_movie_get(i)))
            continue;

        task_movie_get(i)->sub_14041F4E0();
        std::string name = get_pv_movie_file_name(i);
        if (name.size()) {
            task_movie_get(i)->sub_14041F0A0(name);
            while (!task_movie_get(i)->sub_14041F0E0(v14));
        }
    }*/

    sub_140104FB0();

    //sub_14013C8F0()->sub_14012BDA0();

    data.disp_lyrics_now = data.disp_lyrics;

    data.play_data.sub_140137BE0();
    set_lyric(-1, 0xFFFFFFFF);
    data.reset();
    data.title_image_state = 0;
    reset_appear();

    data.life_gauge = 127;
    data.score_final = 0;
    data.challenge_time_total_bonus = 0;
    data.combo = 0;
    data.challenge_time_combo_count = 0;
    data.max_combo = 0;
    data.current_reference_score = 0;
    data.target_count = 0;
    data.field_2CF84 = 0;
    data.field_2CF88 = 0;
    data.score_slide_chain_bonus = 0;
    data.slide_chain_length = 0;
    data.field_2CF98 = 0;
    data.field_2D0BC = false;
    data.next_stage = false;
    data.has_frame_texture = false;
    data.field_2D0C0 = -1;

    for (int32_t& i : data.total_hit_count)
        i = 0;

    for (int32_t& i : data.hit_count)
        i = 0;

    data.play_data.field_5F4 = 1.0f;
    memset(data.play_data.lyric, 0, sizeof(data.play_data.lyric));
    data.play_data.lyric_set = false;

    data.field_2CF30 = 4;
    field_1 = false;
    field_2 = false;
    end_pv = false;
    field_4 = false;
    data.field_2D03C = data.field_2D038;
    data.field_2DB2C = 0;

    data_struct* aft_data = &data_list[DATA_AFT];
    auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

    for (int32_t& i : data.campv_auth_3d_uids) {
        int32_t uid = i;
        const char* name = auth_3d_data_get_uid_name(uid, aft_auth_3d_db);
        if (!name)
            continue;

        if (check_chrcam(name, uid)) {
            auto elem = data.auth_3d.find(uid);
            if (elem != data.auth_3d.end())
                elem->second.set_enable(false);
        }
    }

    itmpv_reset();
    sub_140113730();

    data.edit_effect.data.reset();

    data.field_2D093 = false;
    data.field_2D095 = false;
    data.field_2D096 = false;

    pv_expression_array_reset_data();

    data.pv_end_fadeout = false;

    pv_game_time_start();
}

bool pv_game::sub_14010EF00() {
    /*InputState* input_state = input_state_get(0);
    if (!sub_1401E8380(ub_1401E8950() + 1))
        return false;

    bool v5 = task_slider_control_get()->sub_140618C20(38)
        && input_state->sub_14018D480(10) && input_state->sub_14018D480(7);
    switch (data.field_2DB34) {
    case 0:
        if (v5 && input_state->sub_14018D510(2)) {
            data.field_2DB34 = 1;
            data.field_2DB38.get_timestamp();
            data.field_2D0AC = sub_14010F930();
        }
        break;
    case 1:
        if (!v5 || !input_state->sub_14018D480(2)) {
            sub_14010F030();
            data.field_2DB34 = 0;
            data.field_2D0AC = 0;
            return false;
        }

        if (data.field_2DB38.calc_time_int() > 1000000) {
            data.field_2D0AC = 0;
            return true;
        }
        break;
    }*/
    return false;
}

void pv_game::sub_14010F030() {
    if (data.field_2D0AC)
        data.field_2D0B0[data.field_2D0AC]++;
}

int32_t pv_game::sub_14010F930() {
    if (sub_14013C8C0()->sub_1400E7920() || data.life_gauge <= 0
        || data.notes_passed != data.target_reference_score.size() - 1)
        return 1;
    return 2;
}

int32_t pv_game::sub_140112C00(int32_t index) {
    if (index > 0 && index <= 2)
        return data.field_2D0B0[index];
    return 0;
}

void pv_game::sub_140113730() {
    for (struc_269& i : data.field_2DA08)
        for (auth_3d_id& j : i.auth_3d_ids)
            j.set_enable(false);
}

void pv_game::sub_140115C10(int32_t chara_id, bool value) {
    struc_269& v4 = data.field_2DA08[chara_id];
    int32_t v5 = v4.field_30;
    if (v5 >= 0 && v4.auth_3d_ids.size()) {
        auth_3d_id& id = v4.auth_3d_ids[v5];
        if (id.check_not_empty())
            id.set_visibility(value);
    }
}

void pv_game::sub_140115C90(int32_t chara_id, bool disp, int32_t index, float_t frame, float_t frame_speed) {
    if (chara_id < 0 || chara_id >= 3)
        return;

    struc_269& v19 = data.field_2DA08[chara_id];
    int32_t v11 = v19.field_30;
    if (v11 >= 0) {
        if (v11 >= v19.auth_3d_ids.size())
            return;

        auth_3d_id& id = v19.auth_3d_ids[v11];
        id.set_enable(false);
    }

    if (index >= 0) {
        if (index >= v19.auth_3d_ids.size())
            return;

        auth_3d_id& id = v19.auth_3d_ids[index];
        if (id.check_not_empty()) {
            id.set_enable(true);
            id.set_visibility(disp);
            id.set_paused(false);
            id.set_req_frame(frame);
            id.set_chara_id(chara_id);
            id.set_last_frame(id.get_play_control_size() - 1.0f);

            FrameRateControl* sys_frame_rate = sys_frame_rate_get(chara_id);
            sys_frame_rate->SetFrameSpeed(frame_speed);
            id.set_frame_rate(sys_frame_rate);
        }
    }
    v19.field_30 = index;
}

void pv_game::sub_1401230A0() {
    data.challenge_time_total_bonus = 0;
    data.challenge_time_combo_count = 0;
    data.field_2CF84 = 0;
    data.play_data.sub_140134730();
    sub_14013C8C0()->field_26 = true;
}

void pv_game::chara_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha) {
    if (!data || chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    mdl::ObjFlags flags;
    switch (type) {
    case 0:
    default:
        flags = mdl::OBJ_ALPHA_ORDER_1;
        break;
    case 1:
        flags = mdl::OBJ_ALPHA_ORDER_2;
        break;
    case 2:
        flags = mdl::OBJ_ALPHA_ORDER_3;
        break;
    }

    for (auto& i : ((pv_game*)data)->data.itmpv[chara_id])
        i.second.first.set_alpha_obj_flags(alpha, flags);
}

void pv_game::get_item_mask(pv_performer_type type,
    rob_chara_pv_data_item* src_item, pv_game_item_mask* src_mask,
    rob_chara_pv_data_item* dst_item, pv_game_item_mask* dst_mask) {
    if (!src_item || !src_mask || !dst_item || !dst_mask)
        return;

    if (type == PV_PERFORMER_PSEUDO_SAME) {
        *dst_item = *src_item;
        *dst_mask = *src_mask;
    }
    else {
        *dst_item = {};
        *dst_mask = {};
    }
}

pv_game_init_data::pv_game_init_data() : pv_id(), difficulty(), edition(), score_percentage_clear(),
life_gauge_safety_time(), life_gauge_border(), stage_index(), modules(), items() {

}

void pv_game_init_data::reset() {
    pv_id = 0;
    difficulty = PV_DIFFICULTY_EASY;
    edition = 0;
    score_percentage_clear = 0;
    life_gauge_safety_time = 0;
    life_gauge_border = 0;
    stage_index = 0;

    for (int32_t& i : modules)
        i = 0;

    for (rob_chara_pv_data_item& i : items)
        i = {};

    for (pv_game_item_mask& i : items_mask)
        i = {};
}

struc_14::struc_14() : edition(), pv(), field_10(), field_11(), field_12(),
field_13(), field_14(), field_18(), field_1C(), field_24(), field_25(), field_26() {
    type = 8;
    difficulty = PV_DIFFICULTY_NORMAL;
    field_20 = 1.0f;
}

int32_t struc_14::sub_1400E7910() {
    return type;
}

bool struc_14::sub_1400E7920() {
    return type == 2 || type == 3 || type == 5 || type == 7;
}

void struc_14::sub_1400E79E0(int32_t value) {
    type = value;
}

struc_775::struc_775() : pv_id(), field_4(), difficulty(), edition(), field_10(), field_14(),
field_18(), field_1C(), score_percentage_clear(), life_gauge_safety_time(), life_gauge_border(),
field_2C(), field_30(), field_34(), field_38(), field_3C(), ex_stage(), another_song_index(),
field_44(), field_48(), field_49(), field_4C(), field_64(), field_F4(), field_F8() {

}

struc_660::struc_660() : field_0(), field_4(), field_20(), field_24(), field_48(), field_4C() {

}

struc_660::~struc_660() {

}

struc_661::struc_661() {

}

struc_661::~struc_661() {

}

struc_662::struc_662() : field_0(), total_hit_count(), hit_count(), field_2C(), field_30(), field_44(),
max_combo(), challenge_time_total_bonus(), score_hold_multi(), score_final(), score_percentage(),
score_slide(), field_70(), has_slide(), pv_game_state(), field_7C(), field_80(), field_84(), field_88(),
field_8C(), field_90(), field_94(), field_98(), field_9C(), field_A0(), field_A4(), field_A8(),
field_AC(), field_C4(), field_124(), field_13C(), field_1E0(), field_1E4(), next_stage() {

}

struc_662::~struc_662() {

}

struc_777::struc_777() : field_0(), field_4(), field_8(), field_C(),
field_30(), field_34(), field_38(), field_3C(), field_60(), field_64() {

}

struc_777::~struc_777() {

}

struc_778::struc_778() : field_0(), field_4() {

}

struc_778::~struc_778() {

}

struc_716::struc_716() : field_0(), field_4(), field_8(), field_C(), field_10(), field_11(), field_14(), field_18(),
field_1C(), field_20(), field_24(), field_28(), field_2C(), field_4A8(), field_610(), field_618(), field_620(),
field_628(), field_630(), field_638(), field_640(), field_648(), field_649(), field_650(), field_658(), field_660() {

}

struc_716::~struc_716() {

}

struc_717::struc_717() : field_0(), field_19C8() {

}

struc_717::~struc_717() {

}

TaskPvGame::Args::Args() : init_data(), field_190(), watch(), no_fail(),
disp_lyrics(), field_194(), mute(), ex_stage(), success(), test_pv(), option() {
    Reset();
}

TaskPvGame::Args::~Args() {

}

void TaskPvGame::Args::Clear() {
    se_name.clear();
    slide_se_name.clear();
    chainslide_first_name.clear();
    chainslide_sub_name.clear();
    chainslide_success_name.clear();
    chainslide_failure_name.clear();
    slidertouch_name.clear();
}

void TaskPvGame::Args::Reset() {
    init_data.reset();
    Clear();
    field_190 = false;
    watch = true;
    no_fail = false;
    disp_lyrics = true;
    field_194 = false;
    mute = false;
    ex_stage = false;
    success = false;
    test_pv = false;
    option = 0;
}

TaskPvGame::Data::Data() : type(), init_data(), field_190(), music_play(),
no_fail(), disp_lyrics(), mute(), ex_stage(), success(), option() {
    Reset();
}

TaskPvGame::Data::~Data() {

}

void TaskPvGame::Data::Clear() {
    se_name.clear();
    slide_se_name.clear();
    chainslide_first_name.clear();
    chainslide_sub_name.clear();
    chainslide_success_name.clear();
    chainslide_failure_name.clear();
    slidertouch_name.clear();
}

void TaskPvGame::Data::Reset() {
    type = 1;
    init_data.reset();
    Clear();
    field_190 = false;
    music_play = true;
    no_fail = false;
    disp_lyrics = true;
    mute = false;
    ex_stage = false;
    success = false;
    option = 0;
}

#if PV_DEBUG
TaskPvGame::TaskPvGame() : pause(), step_frame(), is_paused() {
#else
TaskPvGame::TaskPvGame() {
#endif

}

TaskPvGame::~TaskPvGame() {

}

bool TaskPvGame::Init() {
#if PV_DEBUG
    pause = true;
    step_frame = false;
    is_paused = false;
#endif

    Load(data);
    //touch_util::touch_reaction_set_enable(false);
    return true;
}

bool TaskPvGame::Ctrl() {
    return !pv_game_parent_ctrl();
}

bool TaskPvGame::Dest() {
    if (!Unload())
        return false;

    extern float_t frame_speed;
    frame_speed = 1.0f;

    //touch_util::touch_reaction_set_enable(true);
    return true;
}

void TaskPvGame::Disp() {
    pv_game_parent_disp();
}

#if PV_DEBUG
void TaskPvGame::Window() {
    if (data.type != 2 || pv_game_parent_data.pv_state != 1)
        return;

    if (Input::IsKeyTapped(GLFW_KEY_K, GLFW_MOD_CONTROL))
        pv_game_ptr->end_pv = true;
    else if (Input::IsKeyTapped(GLFW_KEY_K))
        pause ^= true;

    if (Input::IsKeyTapped(GLFW_KEY_L)) {
        pause = true;
        step_frame = true;
    }

    if (!pause)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 240.0f;
    float_t h = 86.0f;

    extern int32_t width;
    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    bool collapsed = !ImGui::Begin("PV GAME", 0, window_flags);
    if (collapsed) {
        ImGui::End();
        return;
    }

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed(pause || (!pause && step_frame) ? "Play (K)" : "Pause (K)", { w, 0.0f }))
            pause ^= true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Step Frame (L)", { w, 0.0f })) {
            pause = true;
            step_frame = true;
        }

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Stop (Ctrl+K)", { w, 0.0f }))
            pv_game_ptr->end_pv = true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "%d/%d",
            (int32_t)dsc_time_to_frame(pv_game_get()->data.current_time),
            (int32_t)(pv_game_get()->data.current_time / 10000));
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::ButtonExEnterKeyPressed(buf, { w, 0.0f }, ImGuiButtonFlags_DontClosePopups
            | ImGuiButtonFlags_NoNavFocus | ImGuiButtonFlags_NoHoveredOnFocus);
        ImGui::PopStyleColor(3);
        ImGui::EndTable();
    }

    extern bool input_locked;
    input_locked |= ImGui::IsWindowFocused();
    ImGui::End();
}

void TaskPvGame::Basic() {
    if (data.type != 2 || pv_game_parent_data.pv_state != 1)
        return;

    if (step_frame)
        if (pause)
            pause = false;
        else {
            pause = true;
            step_frame = false;
        }

    if (pause && !is_paused) {
        pv_game_time_pause();

        extern float_t frame_speed;
        frame_speed = 0.0f;

        is_paused = true;
    }

    if (!pause && is_paused) {
        pv_game_time_start();

        extern float_t frame_speed;
        frame_speed = 1.0f;

        is_paused = false;
    }

    pv_game_music_get()->set_pause(pause ? 1 : 0);
    Glitter::glt_particle_manager->SetPause(pause);
}
#endif

void TaskPvGame::Load(TaskPvGame::Data& data) {
    pv_game_init();

    pv_game_parent_data.update_func = pv_game_parent::ctrl;
    pv_game_parent_data.pv_state = 0;
    pv_game_parent_data.playing = true;
    pv_game_parent_data.state = 0;
    pv_game_parent_data.init_time = false;

    pv_game_time_data.curr_time = 0;
    pv_game_time_data.last_stop_time.get_timestamp();
    pv_game_time_data.last_stop_time.get_timestamp();
    pv_game_time_data.add_last_stop_time = true;
    pv_game_time_data.delta_time = 0;
    pv_game_time_data.current_time.get_timestamp();
    pv_game_time_data.current_time.get_timestamp();
    pv_game_time_data.add_current_time = true;

    sub_14013C8C0()->sub_1400E79E0(data.type);
    sub_14013C8C0()->pv = data.init_data.pv_id;
    sub_14013C8C0()->difficulty = data.init_data.difficulty;
    sub_14013C8C0()->edition = data.init_data.edition;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        pv_game_ptr->set_module(i, data.init_data.modules[i]);
        pv_game_ptr->set_item(i, data.init_data.items[i]);
        for (int32_t j = 0; j < 4; j++)
            pv_game_ptr->set_item_mask(i, i, data.init_data.items_mask[i].arr[j]);
    }

    pv_game_ptr->data.music_play = data.music_play;
    pv_game_ptr->data.no_fail = data.no_fail;
    pv_game_ptr->data.disp_lyrics = data.disp_lyrics;
    pv_game_ptr->data.song_energy_base = (float_t)data.init_data.score_percentage_clear;
    pv_game_ptr->data.life_gauge_safety_time = data.init_data.life_gauge_safety_time;
    pv_game_ptr->data.life_gauge_border = data.init_data.life_gauge_border;
    pv_game_ptr->data.stage_index = data.init_data.stage_index;
    pv_game_ptr->data.play_data.other_se_name.assign(data.se_name);
    pv_game_ptr->data.play_data.other_slide_name.assign(data.slide_se_name);
    pv_game_ptr->data.play_data.other_chainslide_first_name.assign(data.chainslide_first_name);
    pv_game_ptr->data.play_data.other_chainslide_sub_name.assign(data.chainslide_sub_name);
    pv_game_ptr->data.play_data.other_chainslide_success_name.assign(data.chainslide_success_name);
    pv_game_ptr->data.play_data.other_chainslide_failure_name.assign(data.chainslide_failure_name);
    pv_game_ptr->data.play_data.other_slidertouch_name.assign(data.slidertouch_name);
    pv_game_ptr->data.mute = data.mute;
    pv_game_ptr->data.ex_stage = data.ex_stage;
    pv_game_ptr->data.play_success = data.success;
    pv_game_ptr->data.play_data.option = data.option;

    target_pos_scale_offset_get();
    pv_game_parent_state = 0;
}

bool TaskPvGame::Unload() {
    switch (pv_game_parent_state) {
    case 0:
        pv_game_parent_data.state = 0;
        pv_game_music_get()->stop();
        //sub_14013C8F0()->sub_14012B830();
        pv_game_parent_state = 1;
        break;
    case 1:
        break;
    default:
        return true;
    }

    if (!pv_game_ptr->unload())
        return false;

    pv_game_parent_state = 2;
    pv_game_music_get()->stop_reset_flags();
    pv_game_free();
    return true;
}

#if PV_DEBUG
PVGameSelector::PVGameSelector() : charas(), modules(), start(), exit() {
    pv_id = 600;
    pv = task_pv_db_get_pv(pv_id);
    difficulty = PV_DIFFICULTY_HARD;
    edition = 0;
    success = true;

    const prj::vector_pair_combine<int32_t, module>& modules = module_table_handler_data_get_modules();
    for (const auto& i : modules)
        modules_data[i.second.chara].push_back(&i.second);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        charas[i] = pv->get_performer_chara(i);
        this->modules[i] = 0;
        module_names[i].clear();

        int32_t cos = pv->get_performer_pv_costume(i, difficulty);
        for (const auto& j : modules_data[charas[i]])
            if (cos == j->cos) {
                this->modules[i] = j->id;
                module_names[i].assign(j->name);
                break;
            }
    }
}

PVGameSelector::~PVGameSelector() {

}

bool PVGameSelector::Init() {
    start = false;
    exit = false;
    return true;
}

bool PVGameSelector::Ctrl() {
    return false;
}

bool PVGameSelector::Dest() {
    return true;
}

void PVGameSelector::Window() {
    static const char* difficulty_names[] = {
        "EASY",
        "NORMAL",
        "HARD",
        "EXTREME",
        "ENCORE",
    };

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = 400.0f;
    float_t h = (float_t)height;
    h = min_def(h, 500.0f);

    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    window_focus = false;
    bool open = true;
    if (!ImGui::Begin("PV Game Selector", &open, window_flags)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        start = false;
        exit = true;
        ImGui::End();
        return;
    }

    pv_db::TaskPvDB* task_pv_db = task_pv_db_get();

    ImGui::StartPropertyColumn("PV");
    extern ImFont* imgui_font_arial;
    if (imgui_font_arial)
        ImGui::PushFont(imgui_font_arial);
    if (ImGui::BeginCombo("", pv->song_name.c_str(), 0)) {
        for (const pv_db_pv& i : task_pv_db->pv_data) {
            if (i.id == 999)
                continue;

            ImGui::PushID(&i);
            if (ImGui::Selectable(i.song_name.c_str(), pv_id == i.id)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && pv_id != i.id)) {
                pv_id = i.id;
                pv = &i;
                difficulty = PV_DIFFICULTY_HARD;
                edition = 0;
                success = true;

                for (int32_t j = 0; j < ROB_CHARA_COUNT; j++) {
                    charas[j] = pv->get_performer_chara(j);
                    this->modules[j] = 0;
                    module_names[j].clear();

                    int32_t cos = pv->get_performer_pv_costume(j, difficulty);
                    for (const auto& k : modules_data[charas[j]])
                        if (cos == k->cos) {
                            modules[j] = k->id;
                            module_names[j].assign(k->name);
                            break;
                        }
                }
            }
            ImGui::PopID();

            if (pv_id == i.id)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= true;
        ImGui::EndCombo();
    }
    if (imgui_font_arial)
        ImGui::PopFont();
    ImGui::EndPropertyColumn();

    ImGui::StartPropertyColumn("Difficulty");
    if (ImGui::BeginCombo("", difficulty_names[difficulty], 0)) {
        for (int32_t i = 0; i < PV_DIFFICULTY_MAX; i++) {
            if (!pv->difficulty[i].size())
                continue;

            if (ImGui::Selectable(difficulty_names[i], difficulty == i)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && difficulty != i)) {
                difficulty = (pv_difficulty)i;
                edition = 0;
                success = true;

                for (int32_t j = 0; j < ROB_CHARA_COUNT; j++) {
                    charas[j] = pv->get_performer_chara(j);
                    this->modules[j] = 0;
                    module_names[j].clear();

                    int32_t cos = pv->get_performer_pv_costume(j, difficulty);
                    for (const auto& k : modules_data[charas[j]])
                        if (cos == k->cos) {
                            modules[j] = k->id;
                            module_names[j].assign(k->name);
                            break;
                        }
                }
            }

            if (difficulty == i)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= true;
        ImGui::EndCombo();
    }
    ImGui::EndPropertyColumn();

    char buf[0x200];

    ImGui::StartPropertyColumn("Edition");
    sprintf_s(buf, sizeof(buf), "%d", edition);
    if (ImGui::BeginCombo("", buf, 0)) {
        for (const pv_db_pv_difficulty& i : pv->difficulty[difficulty]) {
            ImGui::PushID(&i);
            sprintf_s(buf, sizeof(buf), "%d", i.edition);
            if (ImGui::Selectable(buf, edition == i.edition)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && edition != i.edition)) {
                edition = i.edition;
                success = true;

                for (int32_t j = 0; j < ROB_CHARA_COUNT; j++) {
                    charas[j] = pv->get_performer_chara(j);
                    this->modules[j] = 0;
                    module_names[j].clear();

                    int32_t cos = pv->get_performer_pv_costume(j, difficulty);
                    for (const auto& k : modules_data[charas[j]])
                        if (cos == k->cos) {
                            modules[j] = k->id;
                            module_names[j].assign(k->name);
                            break;
                        }
                }
            }
            ImGui::PopID();

            if (edition == i.edition)
                ImGui::SetItemDefaultFocus();
        }

        window_focus |= true;
        ImGui::EndCombo();
    }
    ImGui::EndPropertyColumn();

    bool has_success = !!pv->get_difficulty(difficulty, edition)->pvbranch_success_se_name.size();
    ImGui::DisableElementPush(has_success);
    ImGui::Checkbox("Success", &success);
    ImGui::DisableElementPop(has_success);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        chara_index chara_old = charas[i];

        sprintf_s(buf, sizeof(buf), "Chara %dP", i + 1);
        ImGui::ColumnComboBox(buf, chara_full_names, CHARA_MAX,
            (int32_t*)&charas[i], 0, false, &window_focus);

        if (chara_old != charas[i]) {
            modules[i] = 0;
            module_names[i].clear();

            for (const auto& j : modules_data[charas[i]])
                if (modules[i] == j->cos) {
                    modules[i] = j->id;
                    module_names[i].assign(j->name);
                    break;
                }
        }
    }

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        sprintf_s(buf, sizeof(buf), "Module %dP", i + 1);

        ImGui::StartPropertyColumn(buf);
        extern ImFont* imgui_font_arial;
        if (imgui_font_arial)
            ImGui::PushFont(imgui_font_arial);
        if (ImGui::BeginCombo("", module_names[i].c_str(), 0)) {
            for (const auto& j : modules_data[charas[i]]) {
                if (j->cos == 499)
                    continue;

                ImGui::PushID(&j);
                if (ImGui::Selectable(j->name.c_str(), modules[i] == j->id)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && modules[i] != j->id)) {
                    modules[i] = j->id;
                    module_names[i].assign(j->name);
                }
                ImGui::PopID();

                if (modules[i] == j->id)
                    ImGui::SetItemDefaultFocus();
            }

            window_focus |= true;
            ImGui::EndCombo();
        }
        if (imgui_font_arial)
            ImGui::PopFont();
        ImGui::EndPropertyColumn();
    }

    if (ImGui::Button("Start")) {
        start = true;
        exit = true;
    }

    ImGui::End();
}
#endif

float_t bar_time_set_to_target_flying_time(int32_t bpm, int32_t time_signature, int64_t* time_int) {
    if (time_int)
        *time_int = (int64_t)time_signature * (int32_t)(float_t)((60.0 / (float_t)bpm) * 1000000000.0f);
    return time_signature * (60.0f / (float_t)bpm);
}

float_t dsc_time_to_frame(int64_t time) {
    return roundf((float_t)time / (float_t)1000000000 * 60.0f);
}

bool pv_game_init() {
    pv_game_music_init();

    if (!pv_game_ptr)
        pv_game_ptr = new pv_game;
    return true;
}

pv_game* pv_game_get() {
    return pv_game_ptr;
}

bool pv_game_free() {
    if (pv_game_ptr) {
        delete pv_game_ptr;
        pv_game_ptr = 0;
    }

    pv_game_music_free();
    return true;
}

bool task_pv_game_add_task(TaskPvGame::Args& args) {
    if (!task_pv_game)
        task_pv_game = new TaskPvGame;

    task_pv_game->data.type = 1;
    task_pv_game->data.init_data = args.init_data;
    task_pv_game->data.se_name.assign(args.se_name);
    task_pv_game->data.slide_se_name.assign(args.slide_se_name);
    task_pv_game->data.chainslide_first_name.assign(args.chainslide_first_name);
    task_pv_game->data.chainslide_sub_name.assign(args.chainslide_sub_name);
    task_pv_game->data.chainslide_success_name.assign(args.chainslide_success_name);
    task_pv_game->data.chainslide_failure_name.assign(args.chainslide_failure_name);
    task_pv_game->data.slidertouch_name.assign(args.slidertouch_name);
    task_pv_game->data.field_190 = args.field_190;
    task_pv_game->data.music_play = !args.mute;
    task_pv_game->data.no_fail = args.no_fail;
    if (args.watch) {
        task_pv_game->data.type = 2;
        task_pv_game->data.disp_lyrics = args.disp_lyrics;
    }
    else
        task_pv_game->data.disp_lyrics = true;
    task_pv_game->data.mute = args.mute;
    task_pv_game->data.ex_stage = args.ex_stage;
    task_pv_game->data.success = args.success;
    task_pv_game->data.option = args.option;

    if (args.test_pv)
        return app::TaskWork::AddTask(task_pv_game, "PVGAME", 0);
    else
        return app::TaskWork::AddTask(task_pv_game, 0, "PVGAME", 0);
}

bool task_pv_game_check_task_ready() {
    return app::TaskWork::CheckTaskReady(task_pv_game);
}

bool task_pv_game_del_task() {
    if (!app::TaskWork::CheckTaskReady(task_pv_game)) {
        delete task_pv_game;
        task_pv_game = 0;
        return true;
    }

    task_pv_game->DelTask();
    return false;
}

void task_pv_game_init_pv() {
    PlayerData* player_data = player_data_array_get(0);

    struc_717* v0 = sub_14038BB30();
    struc_716* v2;
    if (v0->field_0.stage_index >= 4)
        v2 = &v0->field_28[2];
    else
        v2 = &v0->field_28[v0->field_0.stage_index];

    int32_t stage_index = v0->field_0.stage_index;
    /*if (stage_index == sub_14038AEE0() - 1)
        stage_index = 2;*/

    TaskPvGame::Args args;
    args.init_data.pv_id = max_def(v2->field_2C.pv_id, 0);
    args.init_data.difficulty = v2->field_2C.difficulty;
    args.init_data.edition = max_def(v2->field_2C.edition, 0);
    args.init_data.score_percentage_clear = v2->field_2C.score_percentage_clear / 100;
    args.init_data.life_gauge_safety_time = v2->field_2C.life_gauge_safety_time;
    args.init_data.life_gauge_border = v2->field_2C.life_gauge_border;
    args.init_data.stage_index = stage_index;
    args.ex_stage = v2->field_2C.ex_stage;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        args.init_data.modules[i] = v2->field_2C.field_4C[i];
        for (int32_t j = 0; j < 4; j++) {
            int32_t item_no = 0;
            if (v2->field_2C.field_7C[i].arr[j] >= 0)
                item_no = customize_item_data_handler_data_get_customize_item_obj_id(
                    v2->field_2C.field_7C[i].arr[j]);
            args.init_data.items[i].arr[j] = item_no;
            args.init_data.items_mask[i].arr[j] = v2->field_2C.field_DC[i].arr[j];
        }
    }

    args.disp_lyrics = true;
    args.field_194 = false;

    /*if (player_data->btn_se_equip >= 0) {
        int64_t v21 = sub_140390240(player_data->btn_se_equip);
        if (v21)
            args.se_name.assign(*(std::string*)(v21 + 40));
    }*/

    /*if (player_data->slide_se_equip >= 0) {
        int64_t v23 = sub_140390240(player_data->slide_se_equip);
        if (v23)
            args.slide_se_name.assign(*(std::string*)(v23 + 40));
    }*/

    /*if (player_data->chainslide_se_equip >= 0) {
        struc_711* v25 = sub_140374F00(player_data->chainslide_se_equip);
        if (v25) {
            args.chainslide_first_name.assign(v25->first_name);
            args.chainslide_sub_name.assign(v25->sub_name);
            args.chainslide_success_name.assign(v25->success_name);
            args.chainslide_failure_name.assign(v25->failure_name);
        }
    }*/

    /*if (player_data->slidertouch_se_equip >= 0) {
        int64_t v28 = sub_14038ED90(player_data->slidertouch_se_equip);
        if (v28)
            args.slidertouch_name.assign(*(std::string*)(v28 + 40));
    }*/

    if (v0->field_0.no_fail) {
        args.no_fail = true;
        if (v0->field_0.watch) {
            args.field_190 = true;
            args.watch = true;
            if (v0->field_0.success)
                args.success = true;
        }
        else {
            args.field_190 = false;
            args.watch = false;
        }
    }
    else {
        args.no_fail = false;
        args.field_190 = false;
        args.watch = false;
    }

    args.option = sub_14038BB30()->field_0.option;
    task_pv_game_add_task(args);
}

bool task_pv_game_init_demo_pv(int32_t pv_id, pv_difficulty difficulty, bool music_play) {
    task_pv_game->data.Reset();

    task_pv_game->data.init_data.pv_id = pv_id;
    task_pv_game->data.init_data.difficulty = difficulty;
    task_pv_game->data.init_data.score_percentage_clear = 100;
    task_pv_game->data.init_data.life_gauge_safety_time = 5;
    task_pv_game->data.init_data.life_gauge_border = 20;
    task_pv_game->data.init_data.stage_index = 0;

    const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
    for (int32_t i = 0; i < 6 && (!pv || pv && i < pv->get_performer_count()); i++) {
        if (pv && pv->get_performer_fixed(i)) {
            module_data mdl;
            if (module_data_handler_data_get_module(pv->get_performer_chara(i),
                pv->get_performer_pv_costume(i, difficulty), mdl))
                task_pv_game->data.init_data.modules[i] = mdl.id;
            else
                task_pv_game->data.init_data.modules[i] = 0;
        }
        else
            task_pv_game->data.init_data.modules[i] = 0;

        for (int32_t& j : task_pv_game->data.init_data.items[i].arr)
            j = 0;

        for (bool& j : task_pv_game->data.init_data.items_mask[i].arr)
            j = true;
    }

    task_pv_game->data.type = 3;
    task_pv_game->data.field_190 = false;
    task_pv_game->data.music_play = music_play;
    task_pv_game->data.no_fail = false;
    task_pv_game->data.disp_lyrics = true;
    task_pv_game->data.success = false;

    task_pv_game->data.se_name.assign("");
    task_pv_game->data.slide_se_name.assign("");
    task_pv_game->data.chainslide_first_name.assign("");
    task_pv_game->data.chainslide_sub_name.assign("");
    task_pv_game->data.chainslide_success_name.assign("");
    task_pv_game->data.chainslide_failure_name.assign("");
    task_pv_game->data.slidertouch_name.assign("");

    task_pv_game->data.option = 0;

    return app::TaskWork::AddTask(task_pv_game, 0, "PVGAMEDEMO", 0);
}

void task_pv_game_init_test_pv() {
    if (test_mode_get())
        return;

    task_rob_manager_add_task();
    TaskPvGame::Args args;
    args.init_data.pv_id = 999;
    args.init_data.difficulty = PV_DIFFICULTY_NORMAL;
    args.init_data.edition = 0;
    args.init_data.score_percentage_clear = 50;
    args.init_data.life_gauge_safety_time = 40;
    args.init_data.life_gauge_border = 30;
    args.field_190 = false;
    args.watch = false;
    args.no_fail = false;
    args.disp_lyrics = true;
    args.field_194 = true;
    args.mute = true;
    task_pv_game_add_task(args);
}

#if PV_DEBUG
bool pv_game_selector_init() {
    if (!pv_game_selector_ptr)
        pv_game_selector_ptr = new PVGameSelector;
    return true;
}

PVGameSelector* pv_game_selector_get() {
    return pv_game_selector_ptr;
}

bool pv_game_selector_free() {
    if (pv_game_selector_ptr) {
        delete pv_game_selector_ptr;
        pv_game_selector_ptr = 0;
    }
    return true;
}
#endif

struc_14* sub_14013C8C0() {
    return &stru_140C94438;
}

struc_717* sub_14038BB30() {
    return &stru_141197E00;
}

DivaPvFrameRate::DivaPvFrameRate() {

}

DivaPvFrameRate::~DivaPvFrameRate() {

}

float_t DivaPvFrameRate::GetDeltaFrame() {
    return get_delta_frame() * get_anim_frame_speed() * frame_speed;
}

pv_game_parent::pv_game_parent() : field_0(), field_1(), pv_state(), playing(), field_4(),
field_5(), field_6(), field_7(), curr_time(), delta_time(), state(), update_func(), field_20(),
field_21(), field_22(), field_23(), field_24(), field_25(), field_26(), field_27(), init_time() {

}

void pv_game_parent::ctrl(pv_game_parent* pvgmp) {
    switch (pvgmp->pv_state) {
    case 0:
    default:
        switch (pvgmp->state) {
        case 0:
        case 5:
            pvgmp->state = 6;
            break;
        case 2:
            pvgmp->state = 3;
            break;
        case 3:
            pvgmp->state = 4;
            break;
        case 6:
            pv_game_time_pause();
            if (sub_14013C8C0()->sub_1400E7920()) {
                pv_game_ptr->state = 0;
                pvgmp->state = 8;
                break;
            }
            pvgmp->state = 7;
            break;
        case 7:
            pv_game_ptr->state = 0;
            pvgmp->state = 8;
            break;
        case 8:
            if (sub_14013C8C0()->sub_1400E7910() < 4 && pv_game_ptr->load())
                pvgmp->state = 9;
            break;
        case 9:
            if (sub_14013C8C0()->sub_1400E7910() == 3 || sub_14013C8C0()->sub_1400E7910() >= 6) {
                struc_14* v15 = sub_14013C8C0();
                v15->field_14 = 0;
                v15->field_18 = 0;
                v15->field_1C = 0;
                v15->field_20 = 1.0f;
                sub_14013C8C0()->sub_1400E7910();
                //task_wait_screen_set_load_end();
            }
            /*else
                task_wait_screen_set_load_end();*/

            pv_game_time_start();
            pvgmp->state = 10;
            break;
        case 10:
            /*if (sub_14013C8C0()->sub_1400E7910() == 3 || (sub_14013C8C0()->sub_1400E7910() >= 6)) {
                if (task_wait_screen_get_hide_text()) {
                    pv_game_ptr->reset_appear();
                    //task_wait_screen_set(0);
                    if (pv_game_ptr->data.field_2CF9C > 0) {
                        pv_game_time_pause();
                        pv_game_time_start();
                    }
                    pvgmp->state = 13;
                }
            }
            else if (task_wait_screen_get_hide_text()) {
                task_wait_screen_set_type_0();*/
            pvgmp->state = 11;
            //}
            break;
        case 11:
            pvgmp->state = 12;
            break;
        case 12:
            pv_game_ptr->reset_appear();
            if (pv_game_ptr->data.field_2CF9C > 0) {
                pv_game_time_pause();
                pv_game_time_start();
            }
            pvgmp->state = 13;
            break;
        default:
            if (pvgmp->state >= 13) {
                pv_game_state = 0;
                pvgmp->init_time = true;
                pvgmp->pv_state = 1;
            }
            break;
        }
        break;
    case 1:
        pv_game_state = pv_game_ptr->ctrl(pvgmp->delta_time, pvgmp->curr_time);
        if (pv_game_state) {
            sub_14013C8C0()->sub_1400E7910();
            pvgmp->pv_state = 2;
        }
        break;
    case 2:
        pvgmp->playing = false;
        /*if (pv_game_ptr->data.play_data.field_64C) {
            struc_662 v19;
            v19.field_0 = pv_game_ptr->data.field_2CE98;
            v19.total_hit_count[0] = pv_game_ptr->data.total_hit_count[0];
            v19.hit_count[0] = pv_game_ptr->data.hit_count[0];
            v19.total_hit_count[1] = pv_game_ptr->data.total_hit_count[1];
            v19.hit_count[1] = pv_game_ptr->data.hit_count[1];
            v19.total_hit_count[2] = pv_game_ptr->data.total_hit_count[2];
            v19.hit_count[2] = pv_game_ptr->data.hit_count[2];
            v19.total_hit_count[3] = pv_game_ptr->data.total_hit_count[3];
            v19.hit_count[3] = pv_game_ptr->data.hit_count[3];
            v19.total_hit_count[4] = pv_game_ptr->data.total_hit_count[4];
            v19.hit_count[4] = pv_game_ptr->data.hit_count[4];
            v19.field_2C = pv_game_ptr->sub_1400FDD80();
            v19.max_combo = pv_game_ptr->data.max_combo;
            v19.challenge_time_total_bonus = pv_game_ptr->data.challenge_time_total_bonus;
            v19.score_final = pv_game_ptr->data.score_final;
            v19.score_percentage = (int)(float)(pv_game_ptr->data.song_energy * 100.0);
            v19.score_hold_multi = pv_game_ptr->data.score_hold_multi;
            v19.score_slide = pv_game_ptr->data.score_slide;
            v19.field_70 = pv_game_ptr->data.field_2D0BC;
            v19.has_slide = pv_game_ptr->data.has_slide;
            v19.field_90 = task_photo_service_sub_1404901F0();
            v6 = 0i64;
            v19.pv_game_state = 0;
            if (pv_game_ptr->data.has_success_se)
                v19.pv_game_state = pv_game_ptr->data.success ? 2 : 1;
            v19.field_7C = pv_game_ptr->get_play_data_ex_song_index();
            v19.field_1E0 = pv_game_ptr->data.field_2D0A8;
            for (int32_t i = 0; i < 3; i++)
                v19.field_1E4[i] = pv_game_ptr->sub_140112C00(i);
            v19.next_stage = pv_game_ptr->data.next_stage;
            sub_14038BB30()->sub_14038BD60(&v19);
            /*if (sub_14038BB30()->field_0.field_15)
                task_photo_service_sub_140490050();
        }*/

        if (pv_game_ptr) {
            if (sub_14013C8C0()->sub_1400E7910() >= 4)
                pv_game_ptr->sub_140106640();
            else
                pv_game_ptr->unload();
        }

        sub_14013C8C0()->sub_1400E7910();
    }
}

pv_game_time::pv_game_time() : curr_time(), add_last_stop_time(), delta_time(), add_current_time() {
    last_stop_time.get_timestamp();
    add_last_stop_time = true;
}

pv_game_se_data::pv_game_se_data() {

}

pv_game_se_data::~pv_game_se_data() {

}

struc_674::struc_674() : performer(), module(), item(), field_1C(), pv() {

}

static bool pv_game_parent_ctrl() {
#if PV_DEBUG
    if (pv_game_parent_data.init_time) {
        if (task_pv_game->pause)
            pv_game_parent_data.init_time = false;

        pv_game_time_data.curr_time = 0;
        pv_game_time_data.last_stop_time.get_timestamp();
        pv_game_time_data.last_stop_time.get_timestamp();

        if (!task_pv_game->pause)
            pv_game_time_data.add_last_stop_time = true;

        pv_game_time_data.delta_time = 0;
        pv_game_time_data.current_time.get_timestamp();
        pv_game_time_data.current_time.get_timestamp();

        if (!task_pv_game->pause)
            pv_game_time_data.add_current_time = true;
    }
#else
    if (pv_game_parent_data.init_time) {
        pv_game_parent_data.init_time = false;

        pv_game_time_data.curr_time = 0;
        pv_game_time_data.last_stop_time.get_timestamp();
        pv_game_time_data.last_stop_time.get_timestamp();
        pv_game_time_data.add_last_stop_time = true;

        pv_game_time_data.delta_time = 0;
        pv_game_time_data.current_time.get_timestamp();
        pv_game_time_data.current_time.get_timestamp();
        pv_game_time_data.add_current_time = true;
    }
#endif

    int64_t curr_time = pv_game_time_data.curr_time;
    if (pv_game_time_data.add_last_stop_time)
        curr_time += pv_game_time_data.last_stop_time.calc_time_int();

    int64_t delta_time = pv_game_time_data.delta_time;
    if (pv_game_time_data.add_current_time)
        delta_time += pv_game_time_data.current_time.calc_time_int();

    pv_game_time_data.delta_time = 0;
    pv_game_time_data.current_time.get_timestamp();

    pv_game_parent_data.curr_time = 1000 * curr_time;
    pv_game_parent_data.delta_time = (float_t)(1000 * delta_time) * 0.000000001f;

    //sub_14013C8D0()->sub_140132C20(pv_game_parent_data.delta_time);

    if (pv_game_parent_data.playing && pv_game_parent_data.update_func)
        pv_game_parent_data.update_func(&pv_game_parent_data);

    pv_game_music_get()->ctrl(pv_game_parent_data.delta_time);

    return pv_game_parent_data.playing;
}

static void pv_game_parent_disp() {
    pv_game_ptr->data.play_data.disp();
    pv_game_ptr->disp();
}

static void pv_game_time_pause() {
    if (pv_game_time_data.add_last_stop_time)
        pv_game_time_data.curr_time += pv_game_time_data.last_stop_time.calc_time_int();
    pv_game_time_data.add_last_stop_time = false;

    if (pv_game_time_data.add_current_time)
        pv_game_time_data.delta_time += pv_game_time_data.current_time.calc_time_int();
    pv_game_time_data.add_current_time = false;
}

static void pv_game_time_start() {
    pv_game_time_data.last_stop_time.get_timestamp();
    pv_game_time_data.add_last_stop_time = true;

    pv_game_time_data.current_time.get_timestamp();
    pv_game_time_data.add_current_time = true;
}

static FrameRateControl* get_diva_pv_frame_rate() {
    return &diva_pv_frame_rate;
}

static SysFrameRate* sys_frame_rate_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= 3)
        chara_id = 0;
    return &sys_frame_rate_array[chara_id];
}

static int32_t sub_1400FCFD0(pv_game_chara* arr, const pv_db_pv* pv) {
    chara_index charas[ROB_CHARA_COUNT];
    for (chara_index& i : charas)
        i = CHARA_MAX;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        if (i < pv->ex_song.chara_count)
            charas[i] = arr[i].chara_index;

    for (const pv_db_pv_ex_song& i : pv->ex_song.data) {
        int32_t j = 0;
        for (; j < ROB_CHARA_COUNT; j++)
            if (charas[j] != CHARA_MAX && i.chara[j] != charas[j])
                break;

        if (j == ROB_CHARA_COUNT)
            return (int32_t)(&i - pv->ex_song.data.data());
    }
    return -1;
}

static void sub_140105010(pv_game_chara* arr, size_t max_count, pv_game_item_mask* a3, struc_674& a4) {
    int32_t performer = (int32_t)a4.performer;
    pv_game_chara* chr = &arr[performer];

    {
        module_data mdl;
        if (module_data_handler_data_get_module(a4.module, mdl)) {
            chr->chara_index = mdl.chara_index;
            chr->cos = mdl.cos;
            chr->pv_data.sleeve_l = mdl.sleeve_l;
            chr->pv_data.sleeve_r = mdl.sleeve_r;
        }
        else {
            chr->chara_index = CHARA_MIKU;
            chr->cos = 0;
        }
    }

    chr->pv_data.item = a4.item;

    /*for (int32_t i = 0; i < PV_PERFORMER_ITEM_MAX; i++)
        if (sub_140234860()->sub_140234460(chr->pv_data.item.arr[i]) < 0)
            chr->pv_data.item.arr[i] = 0;*/

    const pv_db_pv* pv = a4.pv;
    PlayerData* player_data = player_data_array_get(0);
    bool v14 = pv->get_performer_pseudo_fixed(chr->chara_index,
        performer, a4.field_1C, !player_data->field_F40);
    if (sub_14013C8C0()->sub_1400E7910() == 3 && !pv->get_performer_fixed(performer))
        v14 = false;

    ::chara_index chara_index = CHARA_MAX;
    int32_t costume;
    if (pv->is_performer_type_pseudo(performer)) {
        int32_t pseudo_performer = pv->get_performer_pseudo_same_id(performer);
        if (pseudo_performer >= 0) {
            pv_game_chara* pseudo_chr = &arr[pseudo_performer];
            if (!a4.field_1C && player_data->field_F40 && sub_14013C8C0()->sub_1400E7910() != 3
                || chr->chara_index != pseudo_chr->chara_index || pv->get_performer_fixed(performer)) {
                chara_index = pseudo_chr->chara_index;
                pv_performer_type type = pv->get_performer_type(performer);
                costume = pv_db_pv::get_pseudo_costume(type, pseudo_chr->chara_index, pseudo_chr->cos);

                pv_game::get_item_mask(type, &pseudo_chr->pv_data.item,
                    &a3[pseudo_performer], &chr->pv_data.item, &a3[performer]);
            }
        }
    }

    if (v14) {
        chara_index = pv->get_performer_chara(performer);
        costume = pv->get_performer_costume(performer);
        int32_t exclude = pv->get_performer_exclude(performer);
        if (exclude >= 0 && exclude < max_count && arr[exclude].chara_index == chara_index) {
            chara_index = pv->get_performer_chara(exclude);
            costume = pv->get_performer_costume(exclude);
        }
    }

    if (chara_index != CHARA_MAX) {
        module_data mdl;
        module_data_handler_data_get_module(chara_index, costume != -1 ? costume : 0, mdl);
        chr->chara_index = mdl.chara_index;
        chr->cos = mdl.cos;
    }

    pv_performer_item item = PV_PERFORMER_ITEM_ZUJO;
    for (int32_t i = 0; i < PV_PERFORMER_ITEM_MAX; i++) {
        int32_t item_no = pv->get_performer_item(performer, item);
        if (item_no == -2) {
            const item_cos_data* cos = item_table_handler_array_get_item_cos_data(chr->chara_index, chr->cos);
            if (!cos)
                continue;

            item_sub_id sub_id = pv_performer_item_to_item_sub_id(item);
            if (sub_id < 0 || sub_id >= ITEM_SUB_MAX)
                continue;

            chr->pv_data.item.arr[i] = cos->arr[sub_id];
            a3[performer].arr[i] = true;
        }
        else if (item_no > 0) {
            const item_table_item* itm = item_table_handler_array_get_item(chr->chara_index, item_no);
            if (itm && itm->sub_id == pv_performer_item_to_item_sub_id(item))
                chr->pv_data.item.arr[i] = item_no;
        }
    }
}
