/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game_pv_data.hpp"
#include "../../CRE/rob/motion.hpp"
#include "../../CRE/pv_expression.hpp"
#include "../../CRE/pv_param.hpp"
#include "../dw_console.hpp"
#include "pv_game.hpp"
#include "pv_game_camera.hpp"
#include "pv_game_music.hpp"

struct struc_595 {
    const char* name;
    int32_t mottbl_index;
    int32_t type;
    int32_t field_10;
};

struct struc_595 stru_1409A0AA0[] = {
    { "通常", 0xA7, 0, 0 },
    { "カメラ目線", 0xA7, 1, 0 },
    { "上", 0xA9, 0, 0 },
    { "下", 0xAB, 0, 0 },
    { "左", 0xAD, 0, 0 },
    { "右", 0xAF, 0, 0 },
    { "右上", 0xB3, 0, 0 },
    { "右下", 0xB7, 0, 0 },
    { "左上", 0xB1, 0, 0 },
    { "左下", 0xB5, 0, 0 },
    { "上 →　下", 0xB9, 2, 8 },
    { "下 →　上", 0xB9, 3, 8 },
    { "左 →　右", 0xBB, 2, 8 },
    { "右 →　左", 0xBB, 3, 8 },
    { "左上 →　右下", 0xBD, 2, 8 },
    { "右上 →　左下", 0xBF, 2, 8 },
    { "右下 →　左上", 0xBD, 3, 8 },
    { "左下 →　右上", 0xBF, 3, 8 },
    { "中央 →　上", 0xA9, 2, 8 },
    { "中央 →　下", 0xAB, 2, 8 },
    { "中央 →　左", 0xAD, 2, 8 },
    { "中央 →　右", 0xAF, 2, 8 },
    { "中央 →　右上", 0xB3, 2, 8 },
    { "中央 →　右下", 0xB7, 2, 8 },
    { "中央 →　左上", 0xB1, 2, 8 },
    { "中央 →　左下", 0xB5, 2, 8 },
    { "上 →　中央", 0xA9, 3, 8 },
    { "下 →　中央", 0xAB, 3, 8 },
    { "左 →　中央", 0xAD, 3, 8 },
    { "右 →　中央", 0xAF, 3, 8 },
    { "右上 →　中央", 0xB3, 3, 8 },
    { "右下 →　中央", 0xB7, 3, 8 },
    { "左上 →　中央", 0xB1, 3, 8 },
    { "左下 →　中央", 0xB5, 3, 8 },
};

struct struc_595  stru_1409A0DD0[] = {
    { "上", 0xA8, 0, 0 },
    { "下", 0xAA, 0, 0 },
    { "右", 0xAE, 0, 0 },
    { "左", 0xAC, 0, 0 },
    { "右上", 0xB2, 0, 0 },
    { "左上", 0xB0, 0, 0 },
    { "右下", 0xB6, 0, 0 },
    { "左下", 0xB4, 0, 0 },
    { "カメラ目線", 0xA6, 1, 0 },
    { "通常", 0xA6, 0, 0 },
    { "通常", 0xA5, 0, 0 },
};

extern render_context* rctx_ptr;

extern bool light_chara_ambient;
extern vec4 npr_cloth_spec_color;

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t time);
static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t* time);

pv_play_data_set_motion::pv_play_data_set_motion() : frame_speed(), motion_id(),
frame(), blend_duration(), blend(), disable_eye_motion(), dsc_frame() {
    blend_type = MOTION_BLEND_CROSS;
    motion_index = -1;
    dsc_time = -1;
}

void pv_play_data_set_motion::reset() {
    frame_speed = 0.0f;
    motion_id = 0;
    frame = 0.0f;
    blend_duration = 0.0f;
    blend = false;
    blend_type = MOTION_BLEND_CROSS;
    disable_eye_motion = false;
    motion_index = -1;
    dsc_time = -1;
    dsc_frame = 0.0f;
}

pv_play_data_motion_data::pv_play_data_motion_data() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

pv_play_data_motion_data::~pv_play_data_motion_data() {

}

void pv_play_data_motion_data::ctrl(float_t delta_time) {
    if (!rob_chr || current_time > duration)
        return;

    current_time += delta_time;
    ctrl_inner();
}

void pv_play_data_motion_data::ctrl_inner() {
    if (!rob_chr)
        return;

    vec3 pos;
    float_t rot;
    if (current_time >= duration || duration <= 0.0f) {
        pos = end_pos;
        rot = end_rot;
    }
    else {
        float_t t = current_time / duration;
        pos = vec3::lerp(start_pos, end_pos, t);
        rot = lerp_def(start_rot, end_rot, t);
    }

    rob_chr->set_data_miku_rot_position(pos);
    rob_chr->set_data_miku_rot_rot_y_int16((int16_t)((rot * 32768.0f) * (float_t)(1.0 / 180.0)));
    rob_chr->set_osage_reset();

    if (rob_chr->check_for_ageageagain_module()) {
        if (fabsf(vec3::distance(start_pos, end_pos) <= 0.000001f && fabsf(end_rot - start_rot) <= 0.000001f)) {
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
        }
        rob_chara_age_age_array_set_step_full(rob_chr->chara_id, 1);
        rob_chara_age_age_array_set_step_full(rob_chr->chara_id, 2);
    }
}

void pv_play_data_motion_data::clear() {
    set_motion.clear();
    set_motion.shrink_to_fit();
    item_anim.clear();
    item_anim.shrink_to_fit();
}

void pv_play_data_motion_data::reset() {
    rob_chr = 0;
    current_time = 0.0f;
    duration = 0.0f;
    start_pos = 0.0f;
    end_pos = 0.0f;
    start_rot = 0.0f;
    end_rot = 0.0f;
    mot_smooth_len = 12.0f;
    set_motion.clear();
    set_motion.shrink_to_fit();
    item_anim.clear();
    item_anim.shrink_to_fit();
}

void pv_play_data_motion_data::set(rob_chara* rob_chr, float_t duration,
    vec3 start_pos, vec3 end_pos, float_t start_rot, float_t end_rot) {
    this->rob_chr = rob_chr;
    this->current_time = 0.0;
    this->duration = duration;
    this->start_pos = start_pos;
    this->end_pos = end_pos;

    while (true) {
        while (start_rot < -180.0f)
            start_rot += 360.0f;

        if (start_rot <= 180.0f)
            break;
        start_rot += -360.0f;
    }

    while (true) {
        while (end_rot < -180.0f)
            end_rot += 360.0f;

        if (end_rot <= 180.0f)
            break;
        end_rot += -360.0f;
    }

    if (end_rot < start_rot)
        end_rot += 360.0f;

    this->start_rot = start_rot;

    if (end_rot >= start_rot + 180.0f)
        end_rot += -360.0f;

    this->end_rot = end_rot;
}

pv_play_data::pv_play_data() : rob_chr(), disp(), shadow() {

}

pv_play_data::~pv_play_data() {

}

pv_play_data_motion* pv_play_data::get_motion(int32_t motion_index) {
    for (pv_play_data_motion& i : motion)
        if (i.motion_index == motion_index)
            return &i;
    return 0;
}

void pv_play_data::reset() {
    motion.clear();
    motion.shrink_to_fit();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    motion_data.reset();
}

pv_data_camera::pv_data_camera() : enable(), time() {
    index = -1;
}

pv_scene_fade::pv_scene_fade() : enable(), time(), duration(), start_alpha(), end_alpha() {

}

void pv_scene_fade::ctrl(float_t delta_time) {
    if (!enable)
        return;

    time += delta_time;

    float_t alpha;
    bool end;
    if (duration <= time || duration <= 0.0f) {
        alpha = this->end_alpha;
        end = true;
    }
    else {
        alpha = lerp_def(start_alpha, end_alpha, time / duration);
        end = false;
    }

    rctx_ptr->post_process.tone_map->set_scene_fade_color(color, 1);
    rctx_ptr->post_process.tone_map->set_scene_fade_alpha(alpha, 1);

    if (end)
        reset();
}

void pv_scene_fade::reset() {
    enable = false;
    time = 0.0f;
    duration = 0.0f;
    start_alpha = 0.0f;
    end_alpha = 0.0f;
    color = 0.0f;
}

pv_dsc_target::pv_dsc_target() : amplitude(), frequency(), slide_chain_start(),
slide_chain_end(), slide_chain_left(), slide_chain_right(), slide_chain() {
    type = DSC_TARGET_FT_MAX;
}

pv_dsc_target_group::pv_dsc_target_group() : target_count(), field_94(), slide_chain() {
    time_begin = -1;
    time_end = -1;
}

struc_676::struc_676() : field_0(), field_128(), field_130(), field_148(), field_270(), field_288(),
field_290(), field_2A8(), field_2C0(), field_2C4(), field_2C8(), field_2CC(), field_2D0() {

}

void struc_676::reset() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        field_0[i] = false;
        field_8[i].reset();
        field_128[i] = false;
        field_130[i] = 0;
        field_148[i] = false;
        field_150[i].reset();
        field_270[i] = 0.0f;
        field_288[i] = false;
        field_290[i] = 0;
        field_2A8[i] = 0.0f;
    }

    field_2C0 = false;
    field_2C4 = 0;
    field_2CC = 0.0f;
    field_2D0 = 0;
}

pv_game_pv_data::pv_game_pv_data() : field_0(), dsc_state(), play()/*, dsc_buffer(), dsc_buffer_counter()*/,
dsc_data_ptr(), dsc_data_ptr_end(), field_2BF38(), curr_time(), curr_time_float(), prev_time_float(),
field_2BF50(), field_2BF60(), field_2BF64(), field_2BF68(), chara_id(), pv_game()/*, field_2BF80()*/,
music(), dsc_file_handler(), dsc_time(), music_playing(), field_2BFC4(), pv_end(), look_camera_enable(),
field_2BFD5(), data_camera(), has_signature(), has_perf_id(), targets_remaining(), target_index(),
scene_rot_y(), field_2C550(), branch_mode(), last_challenge_note(), field_2C560() {
    field_2BF30 = -1;
    field_2BF58 = -1i64;
    field_2BFB0 = -1;
    field_2BFB4 = -1;
    field_2BFB8 = -1;
    field_2BFBC = -1;
    music_play = true;
    fov = 32.2673416137695f;
    min_dist = 0.05f;
    target_flying_time = -1.0f;
    target_anim_fps = 60.0f;
    anim_frame_speed = 1.0f;
    time_signature = -1;
    scene_rot_mat = mat4_identity;
    field_2C54C = 1000.0f;
    change_field_branch_time = -1;
    first_challenge_note = -1;

    for (pv_play_data& i : playdata)
        i.reset();
}

pv_game_pv_data::~pv_game_pv_data() {

}

int64_t pv_game_pv_data::ctrl(float_t delta_time, int64_t curr_time, bool a4) {
    if (!field_0)
        return 0;

    if (pv_game)
        pv_game->edit_effect_ctrl(delta_time);

    if (!play)
        return 0;

    for (pv_play_data& i : playdata)
        i.motion_data.ctrl(delta_time);

    pv_game_camera_ctrl(delta_time);

    scene_fade_ctrl(delta_time);

    if (field_2BF30 < 0)
        field_2BF30 = curr_time;

    this->curr_time = curr_time + field_2BF38 - field_2BF30;
    curr_time_float = (float_t)((double_t)this->curr_time * 0.000000001);

    /*for (int32_t i = 0; i < 2; i++)
        if (app::TaskWork::CheckTaskReady(task_movie_get(i))) {
            if (pv_game_get()->loaded)
                sub_14041F1B0(this->curr_time);
            break;
        }*/


    dsc_data* prev_dsc_data_ptr = dsc_data_ptr;
    targets_remaining = 0;

    float_t dsc_time_offset = 0.0f;
    while (dsc_data_ptr != dsc_data_ptr_end) {
        bool music_play = false;
        bool next = dsc_ctrl(delta_time, this->curr_time, &dsc_time_offset, &music_play, a4, false);

        if (prev_dsc_data_ptr != dsc_data_ptr)
            print_dsc_command(dsc, prev_dsc_data_ptr, dsc_time);

        prev_dsc_data_ptr = dsc_data_ptr;
        if (music_play) {
            music_play = false;
            prev_time_float = curr_time_float;
        }

        if (!next)
            break;
    }

    return this->curr_time;
}

/*void pv_game_pv_data::dsc_buffer_counter_set() {
    dsc_get_start_position(&dsc_buffer_counter);
}

void pv_game_pv_data::dsc_buffer_set(const void* data, uint32_t size) {
    if (data)
        memmove(this->dsc_buffer, data, size);
    has_signature = true;
    has_perf_id = true;
    dsc_buffer_counter_set();
}*/

bool pv_game_pv_data::dsc_ctrl(float_t delta_time, int64_t curr_time,
    float_t* dsc_time_offset, bool* music_play, bool a6, bool ignore_targets) {
    if (sub_14013C8C0()->sub_1400E7910() >= 4 && field_2BF68 <= curr_time)
        pv_game->set_lyric(-1, 0xFFFFFFFF);

    int32_t func = dsc_data_ptr->func;
    int32_t* data = dsc.get_func_data(dsc_data_ptr);

    if (branch_mode) {
        bool v18;
        if (!pv_game->loaded || pv_game->data.success)
            v18 = branch_mode == 2;
        else
            v18 = branch_mode == 1;

        if (!v18) {
            if (func < 0 || func >= DSC_FT_MAX) {
                //sub_140859DBC("PV Script Command Error\n");
                play = false;
                return false;
            }

            if (func != DSC_FT_TIME && func != DSC_FT_PV_END && func != DSC_FT_PV_BRANCH_MODE) {
                dsc_data_ptr++;
                return true;
            }
        }
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    pv_play_data* playdata = &this->playdata[chara_id];
    rob_chara* rob_chr = playdata->rob_chr;

    switch (func) {
    case DSC_FT_END: {
        play = false;
        return false;
    }
    case DSC_FT_TIME: {
        dsc_time = 10000LL * data[0];
        *dsc_time_offset = 0.0f;
        if (curr_time < dsc_time)
            return false;

        *dsc_time_offset = (float_t)((double_t)(curr_time - dsc_time) * 0.000000001);
    } break;
    case DSC_FT_MIKU_MOVE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        vec3 pos;
        pos = (float_t)data[1] * 0.001f;
        pos = (float_t)data[2] * 0.001f;
        pos = (float_t)data[3] * 0.001f;

        if (rob_chr) {
            pos *= (pv_game->data.pv->edit == 2 ? 1.0f : 0.227f);
            mat4_transform_point(&scene_rot_mat, &pos, &pos);
            rob_chr->set_data_miku_rot_position(pos);
            rob_chr->set_osage_reset();
        }
    } break;
    case DSC_FT_MIKU_ROT: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t rot = (float_t)data[1] * 0.001f;

        if (rob_chr) {
            rob_chr->set_data_miku_rot_rot_y_int16(
                (int16_t)((rot + scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0)));
            rob_chr->set_osage_reset();
        }
    } break;
    case DSC_FT_MIKU_DISP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t disp = data[1];
        playdata->disp = disp == 1;

        if (!rob_chr)
            break;

        if (disp == 1) {
            rob_chr->set_visibility(true);
            if (rob_chr->check_for_ageageagain_module()) {
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
            }

            pv_game->set_data_itmpv_visibility(chara_id, true);
            for (pv_play_data_set_motion& i : playdata->set_motion) {
                bool set = rob_chr->set_motion_id(i.motion_id, i.frame, i.blend_duration,
                    i.blend, 0, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(i.motion_id, i.dsc_frame);
                rob_chr->bone_data->set_disable_eye_motion(i.disable_eye_motion);
                rob_chr->set_motion_step(i.frame_speed);
                if (set)
                    pv_expression_array_set_motion(pv_game->data.pv->
                        pv_expression_file_name.c_str(), chara_id, i.motion_id);

                if (!pv_game->data.pv->disable_calc_motfrm_limit)
                    set_motion_max_frame(chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else {
            rob_chr->set_visibility(false);
            pv_game->set_data_itmpv_visibility(chara_id, false);
        }
    } break;
    case DSC_FT_MIKU_SHADOW: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_FT_TARGET: {
        if (sub_14013C8C0()->sub_1400E7920() || !a6 || targets_remaining > 0)
            targets_remaining = max_def(--targets_remaining, 0);
        else {
            targets_remaining = get_target_target_count(target_index, *dsc_time_offset, !ignore_targets);
            targets_remaining = max_def(targets_remaining, 0);
            target_index++;
        }
    } break;
    case DSC_FT_SET_MOTION: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t motion_index = data[1];
        int32_t blend_duration_int = data[2];
        int32_t frame_speed_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t frame_speed;
        if (frame_speed_int != -1) {
            frame_speed = (float_t)frame_speed_int * 0.001f;
            if (frame_speed < 0.0f)
                frame_speed = 0.0f;
        }
        else
            frame_speed = 1.0f;

        if (!rob_chr)
            break;

        float_t frame = 0.0f;
        float_t dsc_frame = 0.0f;

        bool blend = false;
        pv_play_data_motion* motion = playdata->get_motion(motion_index);
        if (motion && motion->enable && (!motion->motion_index || motion_index == motion->motion_index)) {
            if (dsc_time != motion->time) {
                frame = dsc_time_to_frame(curr_time - motion->time);
                dsc_frame = prj::roundf(dsc_time_to_frame(dsc_time - motion->time));
                if (frame < dsc_frame)
                    frame = dsc_frame;
                blend = curr_time > motion->time;
            }
            else
                frame = 0.0f;
        }

        const pv_db_pv_difficulty* diff = pv_game->data.pv->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (!diff)
            break;

        uint32_t motion_id = diff->get_motion_or_default(chara_id, motion_index).id;
        if (pv_game_get()->data.pv)
            motion_id = pv_game_get()->data.pv->get_chrmot_motion_id(rob_chr->chara_id,
                rob_chr->chara_index, diff->get_motion_or_default(chara_id, motion_index));

        if (!motion_index) {
            if (motion_id == -1) {
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(5);
                if (motion_id == -1)
                    motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
            }
        }
        else
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);

        if (blend)
            blend_duration = 0.0f;
        else
            blend_duration /= anim_frame_speed;

        frame_speed *= anim_frame_speed;

        pv_play_data_set_motion v518;
        v518.frame_speed = frame_speed;
        v518.motion_id = motion_id;
        v518.frame = frame;
        v518.blend_duration = blend_duration;
        v518.blend = blend;
        v518.blend_type = MOTION_BLEND_CROSS;
        v518.disable_eye_motion = true;
        v518.motion_index = motion_index;
        v518.dsc_time = motion ? motion->time : dsc_time;
        v518.dsc_frame = dsc_frame;

        field_2C560.field_0[chara_id] = true;
        field_2C560.field_8[chara_id] = v518;

        if (playdata->disp) {
            bool set = rob_chr->set_motion_id(motion_id, frame, blend_duration,
                blend, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_reset_data(motion_id, dsc_frame);
            rob_chr->set_motion_skin_param(motion_id, dsc_frame);
            rob_chr->bone_data->set_disable_eye_motion(true);
            rob_chr->set_motion_step(frame_speed);
            if (set)
                pv_expression_array_set_motion(pv_game->data.pv->
                    pv_expression_file_name.c_str(), chara_id, motion_id);

            if (!pv_game->data.pv->disable_calc_motfrm_limit)
                set_motion_max_frame(chara_id, motion_index, motion ? motion->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(v518);
            rob_chr->set_motion_step(frame_speed);
        }
    } break;
    case DSC_FT_SET_PLAYDATA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t motion_index = data[1];
        if (motion_index >= 0) {
            pv_play_data_motion* motion = playdata->get_motion(motion_index);
            if (motion) {
                motion->enable = true;
                motion->motion_index = motion_index;
                motion->time = dsc_time;
            }
            else
                playdata->motion.push_back({ true, motion_index, dsc_time });
        }
        else
            playdata->motion.clear();
    } break;
    case DSC_FT_EFFECT: {
    } break;
    case DSC_FT_FADEIN_FIELD: {
    } break;
    case DSC_FT_EFFECT_OFF: {
    } break;
    case DSC_FT_SET_CAMERA: {
    } break;
    case DSC_FT_DATA_CAMERA: {
        int32_t id = data[0];
        int32_t index = data[1];

        switch (id) {
        case 0: {
            if (field_2BF58 < 0)
                field_2BF58 = dsc_time;

            if (data_camera[0].index != index - 1) {
                data_camera[0].index = index - 1;
                data_camera[0].time = this->dsc_time;
                data_camera[0].enable = 1;
            }

            if (dsc_time == data_camera[0].time)
                curr_time = this->data_camera[0].time;

            float_t frame = dsc_time_to_frame(curr_time - data_camera[0].time);
            float_t dsc_frame = prj::roundf(dsc_time_to_frame(dsc_time - data_camera[0].time));
            if (frame < dsc_frame)
                frame = dsc_frame;

            pv_game->set_data_campv(0, index, frame);
            rctx_ptr->camera->set_fast_change_hist0(true);
            set_camera_max_frame(data_camera[0].time);
        } break;
        case 1: {
            if (data_camera[1].index != index - 1) {
                data_camera[1].index = index - 1;
                data_camera[1].time = this->dsc_time;
                data_camera[1].enable = 1;
            }

            if (dsc_time == data_camera[1].time)
                curr_time = data_camera[1].time;

            float_t frame = dsc_time_to_frame(curr_time - data_camera[1].time);
            float_t dsc_frame = prj::roundf(dsc_time_to_frame(dsc_time - data_camera[1].time));
            if (frame < dsc_frame)
                frame = dsc_frame;

            field_2C560.field_2C0 = true;
            field_2C560.field_2C4 = 1;
            field_2C560.field_2C8 = index;
            field_2C560.field_2CC = frame;
            field_2C560.field_2D0 = data_camera[1].time;

            pv_game->set_data_campv(1, index, frame);
            rctx_ptr->camera->set_fast_change_hist0(true);
            set_camera_max_frame(data_camera[1].time);
        } break;
        case 2: {
            pv_game->set_data_campv(2, index, 0.0f);
            rctx_ptr->camera->set_fast_change_hist0(true);
        } break;
        }
    } break;
    case DSC_FT_CHANGE_FIELD: {
        int32_t field_index = data[0];

        if (field_index > 0)
            pv_game->change_field(field_index, dsc_time, curr_time);
        else
            pv_game->reset_field();
    } break;
    case DSC_FT_HIDE_FIELD: {
    } break;
    case DSC_FT_MOVE_FIELD: {
    } break;
    case DSC_FT_FADEOUT_FIELD: {
    } break;
    case DSC_FT_EYE_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t v114 = data[0];
        int32_t blend_duration_int = data[1];

        float_t blend_duration;
        if (blend_duration_int == -1)
            blend_duration = 6.0f;
        else
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;

        if (rob_chr)
            rob_chr->set_eyelid_mottbl_motion_from_face(v114, blend_duration / anim_frame_speed,
                -1.0f, pv_game->data.get_anim_offset(), aft_mot_db);
    } break;
    case DSC_FT_MOUTH_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t mouth_anim_id = data[2];
        int32_t blend_duration_int = data[3];
        int32_t value_int = data[4];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (rob_chr) {
            int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
            if (pv_game->data.field_2D090 && mottbl_index != 144)
                value = 0.0f;

            rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value, 0, blend_duration / anim_frame_speed,
                0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), aft_mot_db);
        }
    } break;
    case DSC_FT_HAND_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t hand_index = data[1];
        int32_t hand_anim_id = data[2];
        int32_t blend_duration_int = data[3];
        int32_t value_int = data[4];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f * 60.0f;
            if (value < 0.0f)
                value = 0.0f;
        }
        else
            value = 1.0f;

        if (rob_chr) {
            int32_t mottbl_index = hand_anim_id_to_mottbl_index(hand_anim_id);
            switch (hand_index) {
            case 0:
                rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, value, 0, blend_duration / anim_frame_speed,
                    0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), aft_mot_db);
                break;
            case 1:
                rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, value, 0, blend_duration / anim_frame_speed,
                    0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), aft_mot_db);
                break;
            }
        }
    } break;
    case DSC_FT_LOOK_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t look_anim_id = data[1];
        int32_t blend_duration_int = data[2];
        int32_t value_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else {
            blend_duration = 6.0f;
        }

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (rob_chr) {
            int32_t mottbl_index = look_anim_id_to_mottbl_index(look_anim_id);
            rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value, mottbl_index == 224, blend_duration
                / anim_frame_speed, 0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), aft_mot_db);
        }
    } break;
    case DSC_FT_EXPRESSION: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t expression_id = data[1];
        int32_t blend_duration_int = data[2];
        int32_t value_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else {
            blend_duration = 6.0f;
        }

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (rob_chr) {
            int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
            bool v167 = dsc.signature >= 0x10000000 || pv_game->data.pv->edit != 1 && pv_game->data.pv->edit != 2;
            rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214 && mottbl_index <= 223,
                blend_duration / anim_frame_speed, 0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), v167, aft_mot_db);
        }
    } break;
    case DSC_FT_LOOK_CAMERA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t enable = data[1];
        int32_t blend_duration_int = data[2];
        int32_t head_rot_strength_int = data[3];
        int32_t eyes_rot_strength_int = data[4];

        if (!rob_chr)
            break;

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t head_rot_strength;
        if (head_rot_strength_int != -1) {
            head_rot_strength = (float_t)head_rot_strength_int * 0.001f;
            head_rot_strength = clamp_def(head_rot_strength, 0.0f, 1.0f);
        }
        else
            head_rot_strength = 1.0f;

        float_t eyes_rot_strength;
        if (eyes_rot_strength_int != -1) {
            eyes_rot_strength = (float_t)eyes_rot_strength_int * 0.001f;
            eyes_rot_strength = clamp_def(eyes_rot_strength, 0.0f, 1.0f);
        }
        else
            eyes_rot_strength = 1.0f;

        blend_duration /= anim_frame_speed;

        if (rob_chr) {
            look_camera_enable = enable == 1;
            if (pv_game->data.pv->is_old_pv)
                rob_chr->set_look_camera_old(look_camera_enable, head_rot_strength,
                    eyes_rot_strength, blend_duration, 0.25f, 2.0f);
            else
                rob_chr->set_look_camera_new(look_camera_enable, head_rot_strength,
                    eyes_rot_strength, blend_duration, 0.25f, 2.0f);
        }
    } break;
    case DSC_FT_LYRIC: {
        int32_t lyric_index = data[0];
        color4u8 color = *(uint32_t*)&data[1];
        color = { color.b, color.g, color.r, color.a };
        pv_game->set_lyric(lyric_index, color);
    } break;
    case DSC_FT_MUSIC_PLAY: {
        if (!pv_game_get()->loaded)
            break;

        if (pv_game->data.pv->song_file_name.size()) {
            music->play(4, pv_game->get_play_data_song_file_name(), false, 0.0f, 0.0f, 0.0f, 3.0f, false);

            int32_t v189 = 100;
            int32_t v192 = 100;
            if (sub_14013C8C0()->sub_1400E7910() == 2 && sub_14013C8C0()->field_25)
                v192 = 0;

            if (!this->music_play) {
                v192 = 0;
                v189 = 0;
            }

            music->set_channel_pair_volume_map(1, v192);
            music->set_channel_pair_volume_map(0, v189);
        }
        else {
            music->play_fade_in(2, pv_game->get_play_data_song_file_name(), 0.0f, -1.0f, false, true, 3.0f, false);
            music->set_volume_map(0, 100);
        }

        if (pv_game->field_1 || pv_game->field_2)
            music->set_pause(true);

        *music_play = true;
        this->music_playing = true;
    } break;
    case DSC_FT_MODE_SELECT:
    case DSC_FT_EDIT_MODE_SELECT: {
        int32_t diff_bits = data[0];
        int32_t mode = data[1];

        bool v3 = true;
        if (func == DSC_FT_MODE_SELECT && has_perf_id && sub_14013C8C0()->sub_1400E7910() < 4)
            v3 = !!(diff_bits & (1 << sub_14013C8C0()->difficulty));

        if (!sub_14013C8C0()->sub_1400E7920() && v3
            && (sub_14013C8C0()->difficulty == PV_DIFFICULTY_EASY
                || sub_14013C8C0()->difficulty == PV_DIFFICULTY_NORMAL)) {
            if (mode == 1)
                pv_game->sub_1401230A0();
            else if (mode == 3) {
                pv_game->data.play_data.sub_140134670(pv_game->data.challenge_time_total_bonus);
                sub_14013C8C0()->field_26 = false;
            }
            else
                field_2BFD5 = true;
        }
    } break;
    case DSC_FT_EDIT_MOTION:
    case DSC_FT_EDIT_MOTION_LOOP:
    case DSC_FT_EDIT_MOTION_F: {
        int32_t motion_index = data[0];
        float_t frame_speed = (float_t)data[1] * 0.001f;
        int32_t _blend_type = data[2];
        float_t value = (float_t)data[3] * 0.001f;

        if (!rob_chr) {
            playdata->motion_data.mot_smooth_len = 12.0f;
            break;
        }

        if (motion_index < 0)
            motion_index = 0;
        if (frame_speed < 0.0f)
            frame_speed = 1.0f;
        value = clamp_def(value, 0.0f, 1.0f);

        uint32_t motion_id;
        const pv_db_pv_difficulty* diff = pv_game->data.pv->get_difficulty(
            sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
        if (diff) {
            motion_id = diff->get_motion_or_default(chara_id, motion_index).id;
            if (pv_game_get()->data.pv)
                motion_id = pv_game_get()->data.pv->get_chrmot_motion_id(rob_chr->chara_id,
                    rob_chr->chara_index, diff->get_motion_or_default(chara_id, motion_index));

            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }
        else
            motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);

        frame_speed *= anim_frame_speed;

        float_t frame_count = motion_storage_get_mot_data_frame_count(motion_id, aft_mot_db) - 1.0f;
        float_t frame = frame_count * value;
        if (pv_game->data.use_osage_play_data && frame_count > 0.0f)
            frame = fmodf((dsc_time_to_frame(curr_time - dsc_time) * frame_speed)
                + frame_count * value, frame_count);

        float_t blend_duration = playdata->motion_data.mot_smooth_len / this->anim_frame_speed;
        MotionBlendType blend_type = MOTION_BLEND_CROSS;
        switch (_blend_type) {
        case 0:
        default:
            if (func == DSC_FT_EDIT_MOTION_LOOP)
                blend_type = MOTION_BLEND_FREEZE;
            blend_duration = 0.0f;
            break;
        case 1:
            break;
        case 2:
            blend_type = MOTION_BLEND_FREEZE;
            break;
        }

        bool blend = !(dsc_time / 10000) || (int32_t)(value * 1000.0f);

        pv_play_data_set_motion v518;
        v518.frame_speed = frame_speed;
        v518.motion_id = motion_id;
        v518.frame = frame;
        v518.blend_duration = blend_duration;
        v518.blend = blend;
        v518.blend_type = blend_type;
        v518.disable_eye_motion = false;
        v518.motion_index = -1;
        v518.dsc_time = dsc_time;
        v518.dsc_frame = frame;

        bool loop = true;
        if (func != DSC_FT_EDIT_MOTION_LOOP && func != DSC_FT_EDIT_MOTION_F)
            loop = false;

        if (playdata->disp || loop) {
            rob_chr->set_motion_id(motion_id, frame, blend_duration, blend, 0, blend_type, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_step(frame_speed);
            if (loop)
                rob_chr->set_motion_loop(0.0f, -1.0f, -1);

            field_2C560.field_148[chara_id] = true;
            field_2C560.field_150[chara_id] = v518;
            field_2C560.field_270[chara_id] = frame_count * value;
        }
        else {
            playdata->set_motion.push_back(v518);
            rob_chr->set_motion_step(frame_speed);
        }

        playdata->motion_data.mot_smooth_len = 12.0f;
    } break;
    case DSC_FT_BAR_TIME_SET: {
        int32_t bpm = data[0];
        int32_t time_signature = data[1] + 1;
        float_t target_flying_time = bar_time_set_to_target_flying_time(bpm, time_signature);
        //field_2BF80->field_0.target_flying_time = target_flying_time;
        this->target_flying_time = target_flying_time;
        this->time_signature = time_signature;
    } break;
    case DSC_FT_SHADOWHEIGHT: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_FT_EDIT_FACE:
    case DSC_FT_EDIT_EXPRESSION: {
        int32_t expression_id = data[0];
        float_t blend_duration = 0.0f;
        if (func == DSC_FT_EDIT_EXPRESSION)
            blend_duration = (float_t)data[1] * 0.001f * 60.0f;

        /*int32_t mouth_anim_id = -1;
        if (!has_perf_id)
            mouth_anim_id = data[1];*/

        if (rob_chr) {
            int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
            bool v236 = true;
            //if (has_perf_id)
                v236 = pv_game->data.pv->edit != 1 && pv_game->data.pv->edit != 2;
            rob_chr->set_face_mottbl_motion(0, mottbl_index, 1.0f, 0, blend_duration / anim_frame_speed,
                0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), v236, aft_mot_db);

            /*if (!has_perf_id) {
                int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
                float_t value = pv_game->data.field_2D090 && mottbl_index != 144 ? 0.0f : 1.0f;
                rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value, 0, target_anim_fps * 0.1,
                    0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), aft_mot_db);
            }*/
        }
    } break;
    case DSC_FT_MOVE_CAMERA:
    case DSC_FT_EDIT_CAMERA: {
        float_t duration = (float_t)data[0] * 0.001f;

        float_t v12 = pv_game->data.pv->edit == 2 ? 1.0f : 0.227f;
        float_t v14 = 1.0f / field_2C54C;

        pv_game_camera_dsc_data start;
        pv_game_camera_dsc_data end;
        start.view_point.x = (float_t)data[1] * v14 * v12;
        start.view_point.y = (float_t)data[2] * v14 * v12;
        start.view_point.z = (float_t)data[3] * v14 * v12;
        start.interest.x = (float_t)data[4] * v14 * v12;
        start.interest.y = (float_t)data[5] * v14 * v12;
        start.interest.z = (float_t)data[6] * v14 * v12;
        start.up_vec.x = (float_t)data[7] * v14;
        start.up_vec.y = (float_t)data[8] * v14;
        start.up_vec.z = (float_t)data[9] * v14;
        end.view_point.x = (float_t)data[10] * v14 * v12;
        end.view_point.y = (float_t)data[11] * v14 * v12;
        end.view_point.z = (float_t)data[12] * v14 * v12;
        end.interest.x = (float_t)data[13] * v14 * v12;
        end.interest.y = (float_t)data[14] * v14 * v12;
        end.interest.z = (float_t)data[15] * v14 * v12;
        end.up_vec.x = (float_t)data[16] * v14;
        end.up_vec.y = (float_t)data[17] * v14;
        end.up_vec.z = (float_t)data[18] * v14;

        mat4_transform_point(&scene_rot_mat, &start.view_point, &start.view_point);
        mat4_transform_point(&scene_rot_mat, &start.interest, &start.interest);
        mat4_transform_point(&scene_rot_mat, &start.up_vec, &start.up_vec);
        mat4_transform_point(&scene_rot_mat, &end.view_point, &end.view_point);
        mat4_transform_point(&scene_rot_mat, &end.interest, &end.interest);
        mat4_transform_point(&scene_rot_mat, &end.up_vec, &end.up_vec);

        float_t acceleration_1 = 0.0f;
        if (data[19] != -1)
            acceleration_1 = (float_t)data[19] * 0.001f;

        float_t acceleration_2 = 1.0f;
        if (data[20] != -1)
            acceleration_2 = (float_t)data[20] * 0.001f;

        int32_t follow_chara = 0;
        int32_t chara_id = 0;
        int32_t chara_follow_point = 0;
        if (func == DSC_FT_EDIT_CAMERA) {
            follow_chara = data[21];
            chara_id = data[22];
            chara_follow_point = data[23];
        }

        pv_game_camera_set_fov_min_dist(fov, min_dist);
        pv_game_camera_set_dsc_data(duration, start, end, acceleration_1, acceleration_2,
            follow_chara, chara_id, chara_follow_point, func == DSC_FT_EDIT_CAMERA);
        pv_game_camera_ctrl(0.0f);

        rctx_ptr->camera->set_fast_change_hist0(true);
    } break;
    case DSC_FT_PV_END: {
        dsc_data_ptr++;
        if (a6)
            pv_game->end(true, false);
        pv_end = true;
        return false;
    }
    case DSC_FT_SHADOWPOS: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_FT_EDIT_LYRIC: {
        int32_t lyric_index = data[0] - 1;
        field_2BF68 = curr_time + 1000000LL * data[1];
        pv_game->set_lyric(lyric_index, 0xFFFFFFFF);
    } break;
    case DSC_FT_EDIT_TARGET: {
    } break;
    case DSC_FT_EDIT_MOUTH:
    case DSC_FT_EDIT_MOUTH_ANIM: {
        int32_t mouth_anim_id = data[0];

        float_t blend_duration = 0.1f;
        if (func == DSC_FT_EDIT_MOUTH_ANIM)
            blend_duration = (float_t)data[1] * 0.001f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        blend_duration *= target_anim_fps;

        float_t value = pv_game->data.field_2D090 && mottbl_index != 144 ? 0.0f : 1.0f;
        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value, 0, blend_duration,
            0.0f, 1.0f, -1, pv_game->data.get_anim_offset(), aft_mot_db);
    } break;
    case DSC_FT_SET_CHARA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_FT_EDIT_MOVE: {
        float_t v9 = 1.0f / field_2C54C;
        float_t duration = (float_t)data[0] * v9;
        vec3 start_pos;
        start_pos.x = (float_t)data[1] * v9;
        start_pos.y = 0.0f;
        start_pos.z = (float_t)data[2] * v9;
        vec3 end_pos;
        end_pos.x = (float_t)data[3] * v9;
        end_pos.y = 0.0f;
        end_pos.z = (float_t)data[4] * v9;
        float_t start_rot = (float_t)data[5] * v9;
        float_t end_rot = (float_t)data[6] * v9;

        if (playdata) {
            float_t v5 = pv_game->data.pv->edit == 2 ? 1.0f : 0.227f;
            start_pos *= v5;
            end_pos *= v5;
            mat4_transform_point(&scene_rot_mat, &start_pos, &start_pos);
            mat4_transform_point(&scene_rot_mat, &end_pos, &end_pos);
            playdata->motion_data.set(playdata->rob_chr, duration,
                start_pos, end_pos, start_rot + scene_rot_y, end_rot + scene_rot_y);
            playdata->motion_data.ctrl(0.0f);
        }
    } break;
    case DSC_FT_EDIT_MOVE_XYZ: {
        float_t v9 = 1.0f / field_2C54C;
        float_t duration = (float_t)data[0] * v9;
        vec3 start_pos;
        start_pos.x = (float_t)data[1] * v9;
        start_pos.y = (float_t)data[2] * v9;
        start_pos.z = (float_t)data[3] * v9;
        vec3 end_pos;
        end_pos.x = (float_t)data[4] * v9;
        end_pos.y = (float_t)data[5] * v9;
        end_pos.z = (float_t)data[6] * v9;
        float_t start_rot = (float_t)data[7] * v9;
        float_t end_rot = (float_t)data[8] * v9;

        if (playdata) {
            float_t v5 = pv_game->data.pv->edit == 2 ? 1.0f : 0.227f;
            start_pos *= v5;
            end_pos *= v5;
            mat4_transform_point(&scene_rot_mat, &start_pos, &start_pos);
            mat4_transform_point(&scene_rot_mat, &end_pos, &end_pos);
            playdata->motion_data.set(playdata->rob_chr, duration,
                start_pos, end_pos, start_rot + scene_rot_y, end_rot + scene_rot_y);
            playdata->motion_data.ctrl(0.0f);
        }
    } break;
    case DSC_FT_EDIT_SHADOW: {
        int32_t enable = data[0];

        playdata->shadow = enable == 1;

        Shadow* shad = rctx_ptr->render_manager.shadow_ptr;
        if (shad)
            if (chara_id == 1)
                shad->blur_filter_enable[1] = playdata->shadow;
            else
                shad->blur_filter_enable[0] = playdata->shadow;
    } break;
    case DSC_FT_EDIT_EYELID: {
        int32_t v337 = data[0];

        if (!rob_chr)
            break;

        int32_t v339;
        switch (v337) {
        case 0:
            rob_chr->autoblink_enable();
            v339 = 2;
            break;
        case 1:
            rob_chr->autoblink_disable();
            v339 = 2;
            break;
        default:
            rob_chr->autoblink_disable();
            v339 = 1;
            break;
        }

        rob_chr->set_eyelid_mottbl_motion_from_face(v339,
            0.0f, -1.0f, pv_game->data.get_anim_offset(), aft_mot_db);
    } break;
    case DSC_FT_EDIT_EYE:
    case DSC_FT_EDIT_EYE_ANIM: {
        int32_t eye_anim_id = data[0];
        float_t v533 = (float_t)data[1] * 0.001f;

        float_t blend_duration = 0.0f;
        if (func == DSC_FT_EDIT_EYE_ANIM)
            blend_duration = (float_t)data[2] * 0.001f * 60.0f;
        if (eye_anim_id < 0 || eye_anim_id >= 34)
            eye_anim_id = 0;

        if (!rob_chr)
            break;

        blend_duration /= anim_frame_speed;

        struc_595& v355 = func == DSC_FT_EDIT_EYE_ANIM ? stru_1409A0DD0[eye_anim_id] : stru_1409A0AA0[eye_anim_id];

        int32_t mottbl_index = v355.mottbl_index;
        float_t _blend_duration = blend_duration;
        bool v358 = false;
        float_t value = 1.0f;
        float_t step = 1.0f;
        int32_t state = 0;
        int32_t v13 = -1;
        switch (v355.type) {
        case 1:
            v358 = true;
            _blend_duration = 0.0f;
            break;
        case 2: {
            state = 2;
            value = 0.0f;
            v13 = 0;
            float_t v363 = motion_storage_get_mot_data_frame_count(rob_chr->
                get_rob_cmn_mottbl_motion_id(mottbl_index), aft_mot_db) - 1.0f;
            step = v533 > 0.0f ? v363 / target_anim_fps * v533 : 1.0f;
        } break;
        case 3: {
            state = 4;
            v13 = 0;
            float_t v363 = motion_storage_get_mot_data_frame_count(rob_chr->
                get_rob_cmn_mottbl_motion_id(mottbl_index), aft_mot_db) - 1.0f;
            step = v533 > 0.0f ? v363 / target_anim_fps * v533 : 1.0f;
        } break;
        }

        rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value, state, _blend_duration, 0.0f,
            step, v13, pv_game->data.get_anim_offset(), aft_mot_db);

        if (pv_game->data.pv->is_old_pv)
            rob_chr->set_look_camera_old(v358, 0.0f, 1.0f, blend_duration, 1.0f, 2.0f);
        else
            rob_chr->set_look_camera_new(v358, 0.0f, 1.0f, blend_duration, 1.0f, 2.0f);
    } break;
    case DSC_FT_EDIT_ITEM: {
        int32_t index = data[0];

        if (rob_chr) {
            const pv_db_pv_difficulty* diff = pv_game->data.pv->get_difficulty(
                sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
            if (diff)
                rob_chr->set_hand_item(diff->get_hand_item_uid(index), 0.0f);
        }
    } break;
    case DSC_FT_EDIT_EFFECT: {
        int32_t index = data[0];
        float_t speed = (float_t)data[1] * 0.001f;

        pv_game->edit_effect_set(index, speed, delta_time);
    } break;
    case DSC_FT_EDIT_HAND_ANIM: {
    } break;
    case DSC_FT_EDIT_DISP: {
        int32_t disp = data[0];

        playdata->disp = disp == 1;
        if (rob_chr)
            if (disp == 1) {
                rob_chr->set_visibility(true);
                if (rob_chr->check_for_ageageagain_module()) {
                    rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
                    rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
                }

                for (pv_play_data_set_motion& i : playdata->set_motion) {
                    bool set = rob_chr->set_motion_id(i.motion_id, i.frame, i.blend_duration,
                        i.blend, false, i.blend_type, aft_bone_data, aft_mot_db);
                    rob_chr->bone_data->set_disable_eye_motion(i.disable_eye_motion);
                    rob_chr->set_motion_step(i.frame_speed);
                    if (set)
                        pv_expression_array_set_motion(pv_game->data.pv->
                            pv_expression_file_name.c_str(), chara_id, i.motion_id);
                }
                playdata->set_motion.clear();
            }
            else
                rob_chr->set_visibility(false);

        pv_game->sub_140115C10(chara_id, playdata->disp);
    } break;
    case DSC_FT_AIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_FT_HAND_ITEM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t hand_index = data[1];
        int32_t index = data[2];

        if (rob_chr) {
            const pv_db_pv_difficulty* diff = pv_game->data.pv->get_difficulty(
                sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
            if (diff)
                switch (hand_index) {
                case 0:
                    rob_chr->set_hand_item_l(diff->get_hand_item_uid(index));
                    break;
                case 1:
                    rob_chr->set_hand_item_r(diff->get_hand_item_uid(index));
                    break;
                }
        }
    } break;
    case DSC_FT_EDIT_BLUSH: {
        int32_t index = data[0];

        if (rob_chr)
            rob_chr->set_face_object_index(index >= 0 || index < 15 ? index : 0);
    } break;
    case DSC_FT_NEAR_CLIP: {
        float_t _min_dist = (float_t)data[0] * 0.001f;
        float_t _fov = (float_t)data[1] * 0.001f;

        camera* cam = rctx_ptr->camera;

        bool ignore_min_dist = true;
        if (_min_dist <= 0.0f) {
            _min_dist = 0.05f;
            ignore_min_dist = false;
        }

        bool ignore_fov = true;
        if (_fov <= 0.0f) {
            _fov = 32.2673416137695f;
            ignore_fov = false;
        }

        this->min_dist = _min_dist;
        cam->set_ignore_min_dist(false);
        cam->set_min_distance(_min_dist);
        cam->set_ignore_min_dist(ignore_min_dist);

        this->fov = _fov;
        cam->set_ignore_fov(false);
        cam->set_fov(_fov);
        cam->set_ignore_fov(ignore_fov);

        pv_game_camera_set_fov_min_dist(fov, min_dist);
    } break;
    case DSC_FT_CLOTH_WET: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t wet = (float_t)data[0] * 0.001f;

        wet = clamp_def(wet, 0.0f, 1.0f);

        if (rob_chr)
            rob_chr->item_equip->wet = wet;
    } break;
    case DSC_FT_LIGHT_ROT: {
    } break;
    case DSC_FT_SCENE_FADE: {
        scene_fade.duration = (float_t)data[0] * 0.001f;
        scene_fade.start_alpha = (float_t)data[1] * 0.001f;
        scene_fade.end_alpha = (float_t)data[2] * 0.001f;
        scene_fade.color.x = (float_t)data[3] * 0.001f;
        scene_fade.color.y = (float_t)data[4] * 0.001f;
        scene_fade.color.z = (float_t)data[5] * 0.001f;

        scene_fade.enable = true;
        scene_fade.time = 0.0f;
        scene_fade_ctrl(0.0f);
    } break;
    case DSC_FT_TONE_TRANS: {
        vec3 start;
        start.x = (float_t)data[0] * 0.001f;
        start.y = (float_t)data[1] * 0.001f;
        start.z = (float_t)data[2] * 0.001f;

        vec3 end;
        end.x = (float_t)data[3] * 0.001f;
        end.y = (float_t)data[4] * 0.001f;
        end.z = (float_t)data[5] * 0.001f;

        rctx_ptr->post_process.tone_map->set_tone_trans(start, end, 1);
    } break;
    case DSC_FT_SATURATE: {
        float_t saturate_coeff = (float_t)data[0] * 0.001f;

        rctx_ptr->post_process.tone_map->set_saturate_coeff(saturate_coeff, 1, false);
    } break;
    case DSC_FT_FADE_MODE: {
        int32_t blend_func = data[0];

        rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(blend_func, 1);
    } break;
    case DSC_FT_AUTO_BLINK: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t enable = data[0];

        if (rob_chr)
            if (enable)
                rob_chr->autoblink_enable();
            else
                rob_chr->autoblink_disable();
    } break;
    case DSC_FT_PARTS_DISP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t id = data[1];
        int32_t disp = data[2];

        if (rob_chr)
            rob_chr->set_parts_disp((item_id)id, disp == 1);
    } break;
    case DSC_FT_TARGET_FLYING_TIME: {
        /*float_t target_flying_time = (float_t)data[0] * 0.001f;

        if (target_flying_time >= 0.0f)
            field_2BF80->field_0.target_flying_time = target_flying_time;*/
    } break;
    case DSC_FT_CHARA_SIZE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t chara_size = data[1];
        if (!rob_chr)
            break;

        int32_t chara_size_index;
        if (chara_size == 0)
            chara_size_index = chara_init_data_get_chara_size_index(rob_chr->chara_index);
        else if (chara_size == 1)
            chara_size_index = chara_init_data_get_chara_size_index(pv_game->data.pv->get_performer_chara(chara_id));
        else if (chara_size == 2)
            chara_size_index = 1;
        else if (chara_size == 3) {
            chara_size_index = rob_chr->pv_data.chara_size_index;
            if (chara_size_index < 0 || chara_size_index > 4)
                break;
        }
        else {
            rob_chr->set_chara_size((float_t)chara_size / field_2C54C);
            break;
        }

        rob_chr->set_chara_size(chara_size_table_get_value(chara_size_index));
        rob_chr->set_chara_pos_adjust_y(chara_pos_adjust_y_table_get_value(chara_size_index));
        break;
    } break;
    case DSC_FT_CHARA_HEIGHT_ADJUST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t height_adjust = data[1];

        if (rob_chr)
            rob_chr->set_chara_height_adjust(height_adjust != 0);
    } break;
    case DSC_FT_ITEM_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t index = data[1];
        int32_t v290 = data[2];
        int32_t disp = data[3];

        if (index < 0)
            break;

        if (v290 == 2) {
            int64_t time = pv_game->get_data_itmpv_time(chara_id, index);
            if (time >= 0 && time <= dsc_time && time <= curr_time) {
                float_t frame = dsc_time_to_frame(curr_time - time);
                float_t dsc_frame = prj::roundf(dsc_time_to_frame(dsc_time - time));
                if (frame < dsc_frame)
                    frame = dsc_frame;
                pv_game->set_data_itmpv_req_frame(chara_id, index, frame);
                set_item_anim_max_frame(chara_id, index, time);
            }
        }
        else if (v290 >= 0) {
            pv_game->set_data_itmpv(chara_id, index, v290 != 0, this->dsc_time);
            set_item_anim_max_frame(chara_id, index, dsc_time);
        }

        if (disp >= 0)
            pv_game->set_data_itmpv_visibility(chara_id, index, disp != 0);
    } break;
    case DSC_FT_CHARA_POS_ADJUST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        vec3 pos;
        pos.x = (float_t)data[1] * 0.001f;
        pos.y = (float_t)data[2] * 0.001f;
        pos.z = (float_t)data[3] * 0.001f;
        if (!rob_chr)
            break;

        mat4_transform_point(&scene_rot_mat, &pos, &pos);
        rob_chr->set_chara_pos_adjust(pos);
        if (rob_chr->check_for_ageageagain_module()) {
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
        }
    } break;
    case DSC_FT_SCENE_ROT: {
        float_t scene_rot = (float_t)data[0] * 0.001f;

        scene_rot_y = scene_rot;
        mat4_rotate_y(scene_rot * DEG_TO_RAD_FLOAT, &scene_rot_mat);
    } break;
    case DSC_FT_EDIT_MOT_SMOOTH_LEN: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t value = data[1];

        if (value >= 0)
            playdata->motion_data.mot_smooth_len = (float_t)(value * 60) / field_2C54C;
        else
            playdata->motion_data.mot_smooth_len = 12.0f;
    } break;
    case DSC_FT_PV_BRANCH_MODE: {
        int32_t branch_mode = data[0];

        if (branch_mode >= 0 && branch_mode <= 2)
            this->branch_mode = branch_mode;
    } break;
    case DSC_FT_DATA_CAMERA_START: {
        int32_t camera_index = data[0];
        int32_t index = data[1];

        if (camera_index < 3) {
            data_camera[camera_index].enable = true;
            data_camera[camera_index].index = index;
            data_camera[camera_index].time = dsc_time;
        }
    } break;
    case DSC_FT_MOVIE_PLAY: {
        int32_t v408 = data[0];

        /*if (pv_game_get()->loaded && v408 && app::TaskWork::CheckTaskReady(task_movie_get(0))) {
            v410 = task_movie_get(0);
            task_movie_get(0)->sub_14041EF10();
            pv_game_get()->data.field_2D0C0 = 0;
            sub_14041F1D0(curr_time);
        }*/
    } break;
    case DSC_FT_MOVIE_DISP: {
        int32_t v412 = data[0];
        if (!pv_game_get()->loaded)
            break;

        /*for (int32_t i = 0; i < 2; i++) {
            if (!app::TaskWork::CheckTaskReady(task_movie_get(i)))
                continue;

            int32_t v415 = 0;
            if (pv_game_get()->data.field_2D0C0 == i) {
                if (pv_game->data.field_2D8A0)
                    switch (v412) {
                    case 1:
                        v415 = 1;
                        break;
                    case 2:
                        v415 = 2;
                        break;
                    }
            }
            task_movie_get(l)->field_88 = v415;
            break;
        }*/
    } break;
    case DSC_FT_WIND: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t wind_strength_kami = (float_t)data[1] * 0.001f;
        float_t wind_strength_outer = (float_t)data[2] * 0.001f;

        if (rob_chr)
            rob_chr->set_wind_strength(wind_strength_outer);
    } break;
    case DSC_FT_OSAGE_STEP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t osage_step_kami = (float_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)data[2] * 0.001f;

        if (rob_chr)
            rob_chr->set_osage_step(osage_step_outer);
    } break;
    case DSC_FT_OSAGE_MV_CCL: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t osage_mv_ccl_kami = (float_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)data[2] * 0.001f;

        if (rob_chr) {
            rob_chr->set_osage_move_cancel(1, osage_mv_ccl_kami);
            rob_chr->set_osage_move_cancel(2, osage_mv_ccl_outer);
        }
    } break;
    case DSC_FT_CHARA_COLOR: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t chara_color = data[1];

        if (rob_chr)
            rob_chr->set_chara_color(chara_color != 0);
    } break;
    case DSC_FT_SE_EFFECT: {
        int32_t index = data[0];

        if (pv_game_get()->loaded)
            pv_game->play_se(index);
    } break;
    case DSC_FT_EDIT_EYELID_ANIM: {
        int32_t v342 = data[0];
        float_t blend_duration = (float_t)data[1] * 0.001f * 60.0f;
        float_t value = (float_t)data[2] * 0.001f;

        if (!rob_chr)
            break;

        int32_t v346;
        switch (v342) {
        case 0:
            rob_chr->autoblink_disable();
            v346 = 2;
            break;
        case 1:
        default:
            rob_chr->autoblink_enable();
            v346 = 2;
            break;
        case 2:
            v346 = 1;
            break;
        case 3:
            v346 = 3;
            break;
        }

        rob_chr->set_eyelid_mottbl_motion_from_face(v346, blend_duration / anim_frame_speed,
            value, pv_game->data.get_anim_offset(), aft_mot_db);
    } break;
    case DSC_FT_EDIT_INSTRUMENT_ITEM: {
        int32_t index = data[0];
        float_t frame_speed = (float_t)data[1] * 0.001f;

        if (rob_chr) {
            field_2C560.field_288[chara_id] = index != -1;
            field_2C560.field_290[chara_id] = index;
            field_2C560.field_2A8[chara_id] = frame_speed;
            pv_game->sub_140115C90(chara_id, playdata->disp, index, rob_chr->get_frame(), frame_speed);
        }
    } break;
    case DSC_FT_PV_END_FADEOUT: {
        float_t duration = (float_t)data[0] * 0.001f;
        int32_t v437 = data[1];

        /*if (a6)
            task_mask_screen_fade_out(duration, v437 != 1);*/
        pv_game->data.pv_end_fadeout = true;
    } break;
    case DSC_FT_TARGET_FLAG: {
    } break;
    case DSC_FT_ITEM_ANIM_ATTACH: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t index = data[1];
        int32_t attach = data[2];

        if (index >= 0)
            pv_game->set_data_itmpv_chara_id(chara_id, index, attach == 1);
    } break;
    case DSC_FT_SHADOW_RANGE: {
        float_t range = (float_t)data[0] * 0.001f;

        rctx_ptr->render_manager.shadow_ptr->range = range;
    } break;
    case DSC_FT_HAND_SCALE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t hand = data[1];
        float_t scale = (float_t)data[2] * 0.001f;

        if (rob_chr)
            switch (hand) {
            case 0:
                rob_chr->set_right_hand_scale(scale);
                break;
            case 1:
                rob_chr->set_left_hand_scale(scale);
                break;
            }
    } break;
    case DSC_FT_LIGHT_POS: {
        vec3 pos;
        pos.x = (float_t)data[0] * 0.001f;
        pos.y = (float_t)data[1] * 0.001f;
        pos.z = (float_t)data[2] * 0.001f;
        int32_t index = data[3];

        if (index >= LIGHT_CHARA && index <= LIGHT_PROJECTION) {
            light_set* set_data = &rctx_ptr->light_set[LIGHT_SET_MAIN];
            set_data->lights[index].set_position(pos);
        }
    } break;
    case DSC_FT_FACE_TYPE: {
    } break;
    case DSC_FT_SHADOW_CAST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t shadow_cast = data[1];

        if (rob_chr)
            rob_chr->set_shadow_cast(shadow_cast == 1);
    } break;
    case DSC_FT_FOG: {
    } break;
    case DSC_FT_BLOOM: {
        int32_t id = data[0];
        float_t duration = (float_t)data[1];

        pv_game->set_pv_param_post_process_bloom_data(true, id, duration);
    } break;
    case DSC_FT_COLOR_COLLE: {
        int32_t set = data[0];
        int32_t id = data[1];
        float_t duration = (float_t)data[2];

        pv_game->set_pv_param_post_process_color_correction_data(set == 1, id, duration);
    } break;
    case DSC_FT_DOF: {
        int32_t set = data[0];
        int32_t id = data[1];
        float_t duration = (float_t)data[2];

        pv_game->set_pv_param_post_process_dof_data(set == 1, id, duration);
    } break;
    case DSC_FT_CHARA_ALPHA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t alpha = (float_t)data[1] * 0.001f;
        float_t duration = (float_t)data[2];
        int32_t type = data[3];

        pv_game->set_pv_param_post_process_chara_alpha_data(chara_id, alpha, type, duration);
    } break;
    case DSC_FT_AOTO_CAP: {
        int32_t enable = data[0];

        if (pv_game->data.pv)
            rctx_ptr->post_process.frame_texture_cont_capture_set(enable == 1);
    } break;
    case DSC_FT_MAN_CAP: {
        int32_t slot = data[0];

        if (pv_game->data.pv && slot >= 0 && slot <= 4)
            rctx_ptr->post_process.frame_texture_slot_capture_set(slot + 1);
    } break;
    case DSC_FT_TOON: {
        int32_t npr_param = data[0];
        int32_t value = data[1];

        rctx_ptr->render_manager.set_npr_param(npr_param);
        if (npr_param == 1)
            if (value > 0)
                npr_cloth_spec_color.w = (float_t)(value - 1) * 0.1f;
            else
                npr_cloth_spec_color.w = 1.0f;
    } break;
    case DSC_FT_SHIMMER: {
    } break;
    case DSC_FT_ITEM_ALPHA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t alpha = (float_t)data[1] * 0.001f;
        float_t duration = (float_t)data[2];
        int32_t type = data[3];

        pv_game->set_pv_param_post_process_chara_item_alpha_data(chara_id, alpha, type, duration);
    } break;
    case DSC_FT_MOVIE_CUT_CHG: {
        /*int32_t v486 = data[0];
        int32_t v487 = data[1];

        if (pv_game_get()->loaded) {
            for (int32_t m = 0; m < 2; m++)
                if (app::TaskWork::CheckTaskReady(task_movie_get(m)))
                    task_movie_get(m)->field_88 = 0;

            if (v486 != -1 && app::TaskWork::CheckTaskReady(task_movie_get(v486))) {
                task_movie_get(v486)->sub_14041EF10((__int64)v492);
                pv_game_get()->data.field_2D0C0 = v486;
                //sub_14041F210(curr_time);
                if (!pv_game->data.field_2D8A0)
                    v487 = 0;
                if (v487)
                    switch (v487) {
                    case 1:
                        v488 = 1;
                        break;
                    case 2:
                        v488 = 2;
                        break;
                    }
                task_movie_get(v486)->field_88 = v488;
            }
        }*/
    } break;
    case DSC_FT_CHARA_LIGHT: {
        int32_t id = data[0];
        float_t transition = (float_t)data[1];

        light_param_light_data& light_data = pv_param::light_data_get_chara_light_data(id);

        light_set* set_data = &rctx_ptr->light_set[LIGHT_SET_MAIN];
        ::light_data* light_chara = &set_data->lights[LIGHT_CHARA];

        light_chara->set_ambient(light_data.ambient);
        light_chara->set_diffuse(light_data.diffuse);
        light_chara->set_specular(light_data.specular);
        light_chara->set_position(light_data.position);
    } break;
    case DSC_FT_STAGE_LIGHT: {
        int32_t id = data[0];
        float_t transition = (float_t)data[1];

        light_param_light_data& light_data = pv_param::light_data_get_chara_light_data(id);

        light_set* set_data = &rctx_ptr->light_set[LIGHT_SET_MAIN];
        ::light_data* light_stage = &set_data->lights[LIGHT_STAGE];

        light_stage->set_ambient(light_data.ambient);
        light_stage->set_diffuse(light_data.diffuse);
        light_stage->set_specular(light_data.specular);
        light_stage->set_position(light_data.position);
    } break;
    case DSC_FT_AGEAGE_CTRL: {
        int32_t chara_id = data[0];
        int32_t parts = data[0];
        int32_t npr = data[2];
        int32_t rot_speed = data[3];
        int32_t skip = data[4];
        int32_t disp = data[5];

        switch (parts) {
        case 0:
            rob_chara_age_age_array_set_params(chara_id, 1, npr, rot_speed, skip, disp);
            rob_chara_age_age_array_set_params(chara_id, 2, npr, rot_speed, skip, disp);
            break;
        case 1:
            rob_chara_age_age_array_set_params(chara_id, 1, npr, rot_speed, skip, disp);
            break;
        case 2:
            rob_chara_age_age_array_set_params(chara_id, 2, npr, rot_speed, skip, disp);
            break;
        }
    } break;
    default: {
        //sub_140859DBC("PV Script Command Error\n");
        play = false;
    } return false;
    }

    dsc_data_ptr++;
    return true;
}

void pv_game_pv_data::dsc_reset_position() {
    //dsc_get_start_position(&dsc_buffer_counter);
    dsc_data_ptr = dsc.data.data();
    dsc_data_ptr_end = dsc.data.data() + dsc.data.size();
}

/*void pv_game_pv_data::dsc_get_start_position(int32_t* value) {
    if (value)
        *value = has_signature ? 1 : 0;
}*/

void pv_game_pv_data::find_change_fields() {
    change_field_branch_fail.clear();
    change_field_branch_success.clear();

    bool branch = false;
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find_func(DSC_FT_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);

        switch (pv_branch_mode) {
        case 0:
            change_field_branch_fail.push_back(time, data[0]);
            change_field_branch_success.push_back(time, data[0]);
            if (!branch)
                change_field_branch_time = 10000LL * time;
            break;
        case 1:
            change_field_branch_fail.push_back(time, data[0]);
            branch = true;
            break;
        case 2:
            change_field_branch_success.push_back(time, data[0]);
            branch = true;
            break;
        }

        prev_time = time;
        i++;
    }
}

dsc_data* pv_game_pv_data::find_func(int32_t func_name, int32_t* time,
    int32_t* pv_branch_mode, dsc_data* start, dsc_data* end) {
    if (!start || !end)
        return 0;

    int32_t _time = -1;
    for (dsc_data* i = start; i != end; i++)
        if (i->func == func_name) {
            if (time)
                *time = _time;
            return i;
        }
        else if (i->func == DSC_FT_END)
            break;
        else if (i->func == DSC_FT_TIME) {
            int32_t* data = dsc.get_func_data(i);
            _time = data[0];
        }
        else if (i->func == DSC_FT_PV_END)
            break;
        else if (i->func == DSC_FT_PV_BRANCH_MODE) {
            if (pv_branch_mode) {
                int32_t* data = dsc.get_func_data(i);
                *pv_branch_mode = data[0];
            }
        }
    return 0;
}

void pv_game_pv_data::find_data_camera() {
    data_camera_branch_fail.clear();
    data_camera_branch_success.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find_func(DSC_FT_DATA_CAMERA, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        int32_t stage_effect = data[0];

        switch (pv_branch_mode) {
        case 0:
            data_camera_branch_fail.push_back(time);
            data_camera_branch_success.push_back(time);
            break;
        case 1:
            data_camera_branch_fail.push_back(time);
            break;
        case 2:
            data_camera_branch_success.push_back(time);
            break;
        }

        prev_time = time;
        i++;
    }
}

void pv_game_pv_data::find_playdata_item_anim(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    std::vector<pv_play_data_event>& item_anim = playdata[chara_id].motion_data.item_anim;
    item_anim.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find_func(DSC_FT_ITEM_ANIM, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        if (chara_id == data[0] && data[2] == 2)
            item_anim.push_back({ time, data[1], pv_branch_mode });

        prev_time = time;
        i++;
    }
}

void pv_game_pv_data::find_playdata_set_motion(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    std::vector<pv_play_data_event>& set_motion = playdata[chara_id].motion_data.set_motion;
    set_motion.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find_func(DSC_FT_SET_MOTION, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        if (chara_id == data[0])
            set_motion.push_back({ time, data[1], pv_branch_mode });

        prev_time = time;
        i++;
    }
}

void pv_game_pv_data::find_set_motion(const pv_db_pv_difficulty* diff) {
    if (!diff)
        return;

    std::vector<pv_play_data_event> v94;
    std::vector<pv_play_data_event> v99[6];

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find_func(DSC_FT_SET_PLAYDATA, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        int32_t* data = dsc.get_func_data(i);

        int32_t chara_id = data[0];
        int32_t motion_index = data[1];

        v99[chara_id].push_back({ time, motion_index, pv_branch_mode });

        prev_time = time;
        i++;
    }

    pv_branch_mode = 0;
    time = -1;
    prev_time = -1;

    i = dsc.data.data();
    i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find_func(DSC_FT_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        int32_t* data = dsc.get_func_data(i);

        int32_t field = data[0];

        if (field >= 0 && field < diff->field.data.size())
            v94.push_back({ time, diff->field.data[field].stage_index, pv_branch_mode });

        prev_time = time;
        i++;
    }

    for (std::vector<pv_data_set_motion>& i : set_motion)
        i.clear();

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        std::vector<pv_data_set_motion>& set_motion = this->set_motion[i];
        pv_play_data& playdata = this->playdata[i];

        for (pv_play_data_event& j : playdata.motion_data.set_motion) {
            int32_t stage_index = 0;
            int32_t time = -1;

            pv_play_data_event* k_begin = v94.data() + v94.size();
            pv_play_data_event* k_end = v94.data();
            for (pv_play_data_event* k = k_begin; k != k_end; ) {
                k--;

                if (k->time <= j.time && k->pv_branch_mode == j.pv_branch_mode) {
                    stage_index = k->index;
                    break;
                }
            }

            pv_play_data_event* l_begin = v99[i].data() + v99[i].size();
            pv_play_data_event* l_end = v99[i].data();
            for (pv_play_data_event* l = l_begin; l != l_end; ) {
                l--;

                if (l->index == j.index && l->time <= j.time) {
                    time = l->time;
                    break;
                }
            }

            if (time < 0)
                time = j.time;

            float_t frame = prj::roundf(dsc_time_to_frame(((int64_t)j.time - time) * 10000));

            uint32_t motion_id = diff->get_motion_or_default(i, j.index).id;
            if (pv_game_get() && pv_game_get()->data.pv) {
                rob_chara* rob_chr = playdata.rob_chr;
                if (rob_chr)
                    motion_id = pv_game_get()->data.pv->get_chrmot_motion_id(rob_chr->chara_id,
                        rob_chr->chara_index, diff->get_motion_or_default(i, j.index));
            }

            if (motion_id != -1)
                set_motion.push_back({ motion_id, { frame, stage_index } });
        }
    }
}

const std::vector<pv_data_set_motion>* pv_game_pv_data::get_set_motion(size_t chara_id) {
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
        return &set_motion[chara_id];
    return 0;
}

pv_dsc_target_group* pv_game_pv_data::get_target() {
    if (target_index < targets.size())
        return &targets[target_index];
    return 0;
}

int32_t pv_game_pv_data::get_target_target_count(size_t index, float_t time_offset, bool a4) {
    if (index >= targets.size())
        return 0;

    /*if (a4)
        sub_14012B9A0(this->field_2BF80, &targets[index], dsc_time, index);*/
    return targets[index].target_count;
}

void pv_game_pv_data::init(::pv_game* pv_game, bool music_play) {
    this->pv_game = pv_game;
    //field_2BF80 = sub_14013C8F0();
    music = pv_game_music_get();
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();

    pv_expression_array_reset();

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        pv_game_chara& chr = pv_game->data.chara[i];
        if (chr.chara_id == -1 || !chr.rob_chr)
            continue;

        pv_play_data& playdata = this->playdata[i];
        playdata.reset();
        playdata.set_motion.clear();

        if (chr.chara_id != -1)
            playdata.rob_chr = rob_chara_array_get(chr.chara_id);

        if (playdata.rob_chr) {
            playdata.rob_chr->frame_speed = anim_frame_speed;
            playdata.rob_chr->data.motion.step_data.step = anim_frame_speed;
            pv_expression_array_set(i, playdata.rob_chr, anim_frame_speed);
        }

        find_playdata_set_motion(i);
        find_playdata_item_anim(i);
    }

    target_flying_time = -1.0f;
    time_signature = -1;
    field_2BFB0 = -1;
    field_2BFB4 = -1;
    field_2BFB8 = -1;
    field_2BFBC = -1;
    this->music_play = music_play;
    look_camera_enable = false;
    field_2BFD5 = false;
    pv_end = false;
    targets_remaining = 0;
    target_index = 0;
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4_identity;
    first_challenge_note = -1;
    last_challenge_note = -1;
    curr_time = 0;
    curr_time_float = 0.0f;
    prev_time_float = 0.0f;
    field_2BF30 = -1;
    field_2BF38 = 0;
    field_2BF68 = 0;
    music_playing = false;

    dsc_reset_position();

    play = true;
    field_2BF50 = false;
    field_2BF60 = false;
    field_2BF58 = -1;

    pv_game_camera_reset();

    fov = 32.2673416137695f;
    min_dist = 0.05f;

    scene_fade.reset();

    find_data_camera();
    find_change_fields();

    const pv_db_pv_difficulty* diff = pv_game->data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    find_set_motion(diff);

    Shadow* shad = rctx_ptr->render_manager.shadow_ptr;
    if (shad) {
        shad->blur_filter_enable[0] = true;
        shad->blur_filter_enable[1] = true;
    }

    pv_game->data.play_data.lyric_color = 0xFFFFFFFF;

    field_0 = true;
    chara_id = 0;

    field_2C560.reset();

    reset_camera_post_process();
}

bool pv_game_pv_data::load(std::string&& file_path, ::pv_game* pv_game, bool music_play) {
    switch (dsc_state) {
    case 0:
        if (dsc_file_handler.read_file(&data_list[DATA_AFT], file_path.c_str()))
            dsc_state = 1;
        break;
    case 1: {
        if (dsc_file_handler.check_not_ready())
            break;

        has_signature = false;
        has_perf_id = false;

        const void* data = dsc_file_handler.get_data();
        size_t size = dsc_file_handler.get_size();
        if (data && size && !(size % 4)) {
            uint32_t signature = *(uint32_t*)data;
            has_signature = signature >= 0x10000000;
            has_perf_id = has_signature && signature >= 0x10101514;
            dsc.parse(data, size, has_perf_id ? DSC_FT : DSC_AC120);
            if (!has_perf_id)
                dsc.convert(DSC_FT);
        }

        dsc_file_handler.reset();
        //dsc_process(pv_game);
        dsc_state = 2;
    } break;
    case 2:
        init(pv_game, music_play);
        dsc_state = 0;
        return true;
    }
    return false;
}

void pv_game_pv_data::reset() {
    pv_game = 0;
    //field_2BF80 = 0;
    music = 0;
    dsc_file_handler.reset();
    play = false;
    field_2BF50 = false;

    if (sub_14013C8C0()->sub_1400E7910() >= 6)
        field_2BF60 = false;

    for (pv_play_data& i : playdata) {
        i.rob_chr = 0;
        i.set_motion.clear();
        i.motion_data.clear();
        pv_expression_array_reset_motion(&i - playdata);
    }

    data_camera_branch_fail.clear();
    data_camera_branch_fail.shrink_to_fit();
    data_camera_branch_success.clear();
    data_camera_branch_success.shrink_to_fit();
    change_field_branch_fail.clear();
    change_field_branch_fail.shrink_to_fit();
    change_field_branch_success.clear();
    change_field_branch_success.shrink_to_fit();
    change_field_branch_time = -1;

    scene_fade.reset();

    reset_camera_post_process();

    targets.clear();
    targets.shrink_to_fit();
    target_index = 0;
    field_0 = 0;
}

void pv_game_pv_data::reset_camera_post_process() {
    camera* cam = rctx_ptr->camera;
    cam->set_ignore_min_dist(false);
    cam->set_min_distance(0.05f);
    cam->set_ignore_fov(false);
    cam->set_fov(32.2673416137695f);

    post_process* pp = &rctx_ptr->post_process;
    pp->tone_map->reset_scene_fade(1);
    pp->tone_map->reset_tone_trans(1);
    pp->tone_map->reset_saturate_coeff(1, 1);

    Shadow* shad = rctx_ptr->render_manager.shadow_ptr;
    if (shad)
        for (bool& i : shad->blur_filter_enable)
            i = true;

    branch_mode = false;

    for (pv_data_camera& i : data_camera)
        i = {};
}

void pv_game_pv_data::scene_fade_ctrl(float_t delta_time) {
    scene_fade.ctrl(delta_time);
}

void pv_game_pv_data::set_camera_max_frame(int64_t time) {
    std::vector<int32_t>& data_camera = branch_mode != 2
        ? data_camera_branch_fail
        : data_camera_branch_success;
    for (int32_t& i : data_camera) {
        if (10000LL * i <= dsc_time)
            continue;

        float_t max_frame = prj::roundf(dsc_time_to_frame(10000LL * i - time));
        if (pv_game->data.camera_auth_3d_uid != -1) {
            auth_3d_id id = pv_game->get_auth_3d_id(pv_game->data.camera_auth_3d_uid);
            if (id.check_not_empty())
                id.set_max_frame(max_frame - 1.0f);
        }
        break;
    }
}

void pv_game_pv_data::set_item_anim_max_frame(int32_t chara_id, int32_t index, int64_t time) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || index < 0)
        return;

    for (auto& i : playdata[chara_id].motion_data.item_anim)
        if (10000LL * i.time > dsc_time && i.index == index && (!branch_mode || branch_mode == i.pv_branch_mode)) {
            pv_game->set_data_itmpv_max_frame(chara_id, index, prj::roundf(dsc_time_to_frame(10000LL * i.time - time)) - 1.0f);
            break;
        }
}

void pv_game_pv_data::set_motion_max_frame(int32_t chara_id, int32_t motion_index, int64_t time) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || motion_index < 0)
        return;

    pv_play_data& playdata = this->playdata[chara_id];

    std::vector<pv_play_data_event>& set_motion = playdata.motion_data.set_motion;
    for (pv_play_data_event& i : set_motion)
        if (10000LL * i.time > dsc_time && i.index == motion_index
            && (!branch_mode || branch_mode == i.pv_branch_mode)) {
            playdata.rob_chr->bone_data->set_motion_max_frame(
                prj::roundf(dsc_time_to_frame(10000LL * i.time - time)) - 1.0f);
            break;
        }
}

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t time) {
    print_dsc_command(dsc, dsc_data_ptr, &time);
}

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t* time) {
    if (dsc_data_ptr->func < 0 || dsc_data_ptr->func >= DSC_FT_MAX) {
        dw_console_printf(DW_CONSOLE_PV_SCRIPT, "UNKNOWN command(%d)\n", dsc_data_ptr->func);
        return;
    }

    if (dsc_data_ptr->func == DSC_FT_TIME) {
        if (time)
            return;
    }
    else {
        if (time)
#pragma warning(suppress: 26451)
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%.3f:", (float_t)*time * 0.00001f);
    }

    dsc_get_func_length get_func_length = dsc.get_dsc_get_func_length();

    dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%s(", dsc_data_ptr->name);

    int32_t* data = dsc.get_func_data(dsc_data_ptr);

    int32_t length = get_func_length(dsc_data_ptr->func);
    for (int32_t i = 0; i < length; i++)
        if (i)
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, ", %d", ((int32_t*)data)[i]);
        else
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%d", ((int32_t*)data)[i]);

    dw_console_printf(DW_CONSOLE_PV_SCRIPT, ")\n");
}
