/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <set>
#include "pv_game.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/data.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/pv_param.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/sort.hpp"
#include "imgui_helper.hpp"
#include "input.hpp"

enum dsc_ft_func {
    DSC_FT_END,
    DSC_FT_TIME,
    DSC_FT_MIKU_MOVE,
    DSC_FT_MIKU_ROT,
    DSC_FT_MIKU_DISP,
    DSC_FT_MIKU_SHADOW,
    DSC_FT_TARGET,
    DSC_FT_SET_MOTION,
    DSC_FT_SET_PLAYDATA,
    DSC_FT_EFFECT,
    DSC_FT_FADEIN_FIELD,
    DSC_FT_EFFECT_OFF,
    DSC_FT_SET_CAMERA,
    DSC_FT_DATA_CAMERA,
    DSC_FT_CHANGE_FIELD,
    DSC_FT_HIDE_FIELD,
    DSC_FT_MOVE_FIELD,
    DSC_FT_FADEOUT_FIELD,
    DSC_FT_EYE_ANIM,
    DSC_FT_MOUTH_ANIM,
    DSC_FT_HAND_ANIM,
    DSC_FT_LOOK_ANIM,
    DSC_FT_EXPRESSION,
    DSC_FT_LOOK_CAMERA,
    DSC_FT_LYRIC,
    DSC_FT_MUSIC_PLAY,
    DSC_FT_MODE_SELECT,
    DSC_FT_EDIT_MOTION,
    DSC_FT_BAR_TIME_SET,
    DSC_FT_SHADOWHEIGHT,
    DSC_FT_EDIT_FACE,
    DSC_FT_MOVE_CAMERA,
    DSC_FT_PV_END,
    DSC_FT_SHADOWPOS,
    DSC_FT_EDIT_LYRIC,
    DSC_FT_EDIT_TARGET,
    DSC_FT_EDIT_MOUTH,
    DSC_FT_SET_CHARA,
    DSC_FT_EDIT_MOVE,
    DSC_FT_EDIT_SHADOW,
    DSC_FT_EDIT_EYELID,
    DSC_FT_EDIT_EYE,
    DSC_FT_EDIT_ITEM,
    DSC_FT_EDIT_EFFECT,
    DSC_FT_EDIT_DISP,
    DSC_FT_EDIT_HAND_ANIM,
    DSC_FT_AIM,
    DSC_FT_HAND_ITEM,
    DSC_FT_EDIT_BLUSH,
    DSC_FT_NEAR_CLIP,
    DSC_FT_CLOTH_WET,
    DSC_FT_LIGHT_ROT,
    DSC_FT_SCENE_FADE,
    DSC_FT_TONE_TRANS,
    DSC_FT_SATURATE,
    DSC_FT_FADE_MODE,
    DSC_FT_AUTO_BLINK,
    DSC_FT_PARTS_DISP,
    DSC_FT_TARGET_FLYING_TIME,
    DSC_FT_CHARA_SIZE,
    DSC_FT_CHARA_HEIGHT_ADJUST,
    DSC_FT_ITEM_ANIM,
    DSC_FT_CHARA_POS_ADJUST,
    DSC_FT_SCENE_ROT,
    DSC_FT_EDIT_MOT_SMOOTH_LEN,
    DSC_FT_PV_BRANCH_MODE,
    DSC_FT_DATA_CAMERA_START,
    DSC_FT_MOVIE_PLAY,
    DSC_FT_MOVIE_DISP,
    DSC_FT_WIND,
    DSC_FT_OSAGE_STEP,
    DSC_FT_OSAGE_MV_CCL,
    DSC_FT_CHARA_COLOR,
    DSC_FT_SE_EFFECT,
    DSC_FT_EDIT_MOVE_XYZ,
    DSC_FT_EDIT_EYELID_ANIM,
    DSC_FT_EDIT_INSTRUMENT_ITEM,
    DSC_FT_EDIT_MOTION_LOOP,
    DSC_FT_EDIT_EXPRESSION,
    DSC_FT_EDIT_EYE_ANIM,
    DSC_FT_EDIT_MOUTH_ANIM,
    DSC_FT_EDIT_CAMERA,
    DSC_FT_EDIT_MODE_SELECT,
    DSC_FT_PV_END_FADEOUT,
    DSC_FT_TARGET_FLAG,
    DSC_FT_ITEM_ANIM_ATTACH,
    DSC_FT_SHADOW_RANGE,
    DSC_FT_HAND_SCALE,
    DSC_FT_LIGHT_POS,
    DSC_FT_FACE_TYPE,
    DSC_FT_SHADOW_CAST,
    DSC_FT_EDIT_MOTION_F,
    DSC_FT_FOG,
    DSC_FT_BLOOM,
    DSC_FT_COLOR_COLLE,
    DSC_FT_DOF,
    DSC_FT_CHARA_ALPHA,
    DSC_FT_AOTO_CAP,
    DSC_FT_MAN_CAP,
    DSC_FT_TOON,
    DSC_FT_SHIMMER,
    DSC_FT_ITEM_ALPHA,
    DSC_FT_MOVIE_CUT_CHG,
    DSC_FT_CHARA_LIGHT,
    DSC_FT_STAGE_LIGHT,
    DSC_FT_AGEAGE_CTRL,
    DSC_FT_MAX,
};

pv_game _pv_game;
TaskPvGame task_pv_game;

extern render_context* rctx_ptr;

pv_play_data_motion_data::pv_play_data_motion_data() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

pv_play_data_motion_data::~pv_play_data_motion_data() {

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
    set_item.clear();
    set_item.shrink_to_fit();
}

pv_play_data::pv_play_data() : rob_chr(), disp(), field_31(), field_34() {

}

pv_play_data::~pv_play_data() {

}

void pv_play_data::reset() {
    motion.clear();
    motion.shrink_to_fit();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    motion_data.reset();
}

pv_disp2d::pv_disp2d() : pv_id(), title_start_2d_field(), title_end_2d_field(),
title_start_2d_low_field(), title_end_2d_low_field(), title_start_3d_field(), title_end_3d_field(),
target_shadow_type(), pv_spr_set_id(), pv_aet_set_id(), pv_main_aet_id() {

}

pv_disp2d::~pv_disp2d() {

}

pv_game_dsc_data::pv_game_dsc_data() : field_0(), state(), play(), dsc_buffer(), dsc_buffer_counter(),
field_2BF30(), field_2BF38(), curr_time(), curr_time_float(), prev_time_float(), field_2BF50(), field_2BF58(),
field_2BF60(), field_2BF64(), field_2BF68(), chara_id(), pv_game()/*, field_2BF80(), field_2BF88()*/,
dsc_file_handler(), target_anim_fps(), anim_frame_speed(), target_flying_time(), time_signature(), dsc_time(),
field_2BFB0(), field_2BFB4(), field_2BFB8(), field_2BFBC(), field_2BFC0(), music_play(), field_2BFC4(), pv_end(),
fov(), min_dist(), field_2BFD4(), field_2BFD5(), data_camera(), change_field_branch_time(), scene_fade_enable(),
scene_fade_frame(), scene_fade_duration(), scene_fade_start_alpha(), scene_fade_end_alpha(), scene_fade_color(),
has_signature(), has_perf_id(), field_2C4E4(), field_2C500(), scene_rot_y(), scene_rot_mat(), field_2C54C(),
field_2C550(), branch_mode(), first_challenge_note(), last_challenge_note(), field_2C560() {

}

pv_game_dsc_data::~pv_game_dsc_data() {

}

pv_game_data_chara::pv_game_data_chara() : chara_index(), cos(), chara_id(), pv_data(), rob_chr() {

}

pv_game_data_chara::~pv_game_data_chara() {

}

pv_game_data_field::pv_game_data_field() : stage_index(), light_auth_3d_uid(), light_auth_3d_id(),
light_frame(), spr_set_back_id(), stage_flag(), npr_type(), cam_blur(), sdw_off() {

}

pv_game_data_field::~pv_game_data_field() {

}

struc_78::struc_78() : field_30(), field_34() {

}

struc_78::~struc_78() {

}

pv_game_data_chreff::pv_game_data_chreff() : src_chara_index(),
dst_chara_index(), src_auth_3d_uid(), dst_auth_3d_uid() {

}

pv_game_data_chreff::~pv_game_data_chreff() {

}

pv_effect_resource::pv_effect_resource() : emission() {

}

pv_effect_resource::~pv_effect_resource() {

}

pv_game_data::pv_game_data() : field_2CE98(), field_2CE9C(), field_2CF1C(), field_2CF20(), field_2CF24(),
field_2CF28(), field_2CF2C(), field_2CF30(), field_2CF34(), field_2CF38(), life_gauge(), score_final(),
challenge_time_total_bonus(), combo(), challenge_time_combo_count(), max_combo(), total_hit_count(), hit_count(),
current_reference_score(), target_count(), field_2CF84(), field_2CF88(), field_2CF8C(), score_slide_chain_bonus(),
slide_chain_length(), field_2CF98(), field_2CF9C(), field_2CFA0(), field_2CFA4(), pv(), field_2CFB0(), field_2CFB4(),
reference_score(), field_2CFBC(), field_2CFC0(), field_2CFC4(), field_2CFE0(), field_2CFE4(), notes_passed(),
field_2CFF0(), score_percentage(), score_percentage_clear(), score_percentage_border(), life_gauge_safety_time(),
life_gauge_border(), field_2D008(), field_2D00C(), no_fail(), field_2D00E(), field_2D00F(), challenge_time_start(),
challenge_time_end(), max_time(), max_time_float(), current_time_float(), current_time(), current_time_float_dup(),
field_2D03C(), score_hold_multi(), score_hold(), score_slide(), field_2D04C(), chance_time_ended(), pv_disp2d(),
life_gauge_final(), hit_border(), field_2D05D(), field_2D05E(), field_2D05F(), title_image_init(), field_2D061(),
field_2D062(), field_2D063(), has_auth_3d_frame(), has_light_frame(), field_2D066(), field_2D068(), field_2D069(),
field_2D06A(), field_2D06C(), field_index(), edit_effect_index(), slidertouch_counter(),
change_field_branch_success_counter(), field_2D080(), field_2D084(), life_gauge_bonus(), life_gauge_total_bonus(),
field_2D090(), field_2D091(), field_2D092(), field_2D093(), field_2D094(), field_2D095(), field_2D096(), success(),
field_2D098(), field_2D09C(), field_2D0A0(), pv_end_fadeout(), rival_percentage(), field_2D0A8(), field_2D0AC(),
field_2D0B0(), field_2D0BC(), field_2D0BD(), field_2D0BE(), field_2D0BF(), field_2D0C0(), field_2D0C4(),
field_2D7E8(), field_2D7EC(), field_2D808(), field_2D80C(), field_2D858(), camera_auth_3d_uid(), field_2D874(),
field_2D878(), field_2D87C(), current_field(), field_2D8A0(), field_2D8A4(), data_camera_id(),
field_2D954(), field_2DAC8(), field_2DACC(), field_2DB28(), field_2DB2C(), field_2DB34() {

}

pv_game_data::~pv_game_data() {

}

pv_game::pv_game() : loaded(), field_1(), field_2(), end_pv(), field_4(),
state(), field_C(), pv_id(), field_14(), modules(), items(), field_90() {

}

pv_game::~pv_game() {

}

void pv_game_init_data::reset() {
    pv_id = 0;
    difficulty = 0;
    field_C = 0;
    score_percentage_clear = 0;
    life_gauge_safety_time = 0;
    life_gauge_border = 0;
    field_18 = 0;
    for (int32_t& i : modules)
        i = 0;
    for (rob_chara_pv_data_item& i : items)
        i = {};
    for (struc_653& i : field_94)
        i = {};
}

TaskPvGame::InitData::InitData() : data(), field_190(), field_191(), no_fail(),
field_193(), field_194(), field_195(), field_196(), field_197(), field_198(), field_19C() {
    Reset();
}

TaskPvGame::InitData::~InitData() {

}

void TaskPvGame::InitData::Clear() {
    field_B0.clear();
    field_D0.clear();
    field_F0.clear();
    field_110.clear();
    field_130.clear();
    field_150.clear();
    field_170.clear();
}

void TaskPvGame::InitData::Reset() {
    data.reset();
    Clear();
    field_190 = false;
    field_191 = true;
    no_fail = false;
    field_193 = true;
    field_194 = false;
    field_195 = false;
    field_196 = false;
    field_197 = false;
    field_198 = false;
    field_19C = 0;
}

TaskPvGame::Data::Data() : field_0(), data(), field_190(), field_191(), no_fail(),
field_193(), field_194(), field_195(), field_196(), field_198() {
    Reset();
}

TaskPvGame::Data::~Data() {

}

void TaskPvGame::Data::Clear() {
    field_B0.clear();
    field_D0.clear();
    field_F0.clear();
    field_110.clear();
    field_130.clear();
    field_150.clear();
    field_170.clear();
}

void TaskPvGame::Data::Reset() {
    field_0 = 1;
    data.reset();
    Clear();
    field_190 = false;
    field_191 = true;
    no_fail = false;
    field_193 = true;
    field_194 = false;
    field_195 = false;
    field_196 = false;
    field_198 = 0;
}

TaskPvGame::TaskPvGame() {

}

TaskPvGame::~TaskPvGame() {

}

bool TaskPvGame::Init() {
    return true;
}

bool TaskPvGame::Ctrl() {
    return true;
    //return false;
}

bool TaskPvGame::Dest() {
    return true;
}

void TaskPvGame::Disp() {

}

bool task_pv_game_add_task(TaskPvGame::InitData* init_data) {
    task_pv_game.data.Reset();

    task_pv_game.data.field_0 = 1;
    task_pv_game.data.data = init_data->data;
    task_pv_game.data.field_B0 = init_data->field_B0;
    task_pv_game.data.field_D0 = init_data->field_D0;
    task_pv_game.data.field_F0 = init_data->field_F0;
    task_pv_game.data.field_110 = init_data->field_110;
    task_pv_game.data.field_130 = init_data->field_130;
    task_pv_game.data.field_150 = init_data->field_150;
    task_pv_game.data.field_170 = init_data->field_170;
    task_pv_game.data.field_190 = init_data->field_190;
    task_pv_game.data.field_191 = !init_data->field_195;
    task_pv_game.data.no_fail = init_data->no_fail;
    if (init_data->field_191) {
        task_pv_game.data.field_0 = 2;
        task_pv_game.data.field_193 = init_data->field_193;
    }
    else
        task_pv_game.data.field_193 = 1;
    task_pv_game.data.field_194 = init_data->field_195;
    task_pv_game.data.field_195 = init_data->field_196;
    task_pv_game.data.field_196 = init_data->field_197;
    task_pv_game.data.field_198 = init_data->field_19C;

    if (init_data->field_198)
        return app::TaskWork::AddTask(&task_pv_game, "PVGAME");
    else
        return app::TaskWork::AddTask(&task_pv_game, 0, "PVGAME");
}

bool task_pv_game_check_task_ready() {
    return app::TaskWork::CheckTaskReady(&task_pv_game);
}

bool task_pv_game_del_task() {
    if (!app::TaskWork::CheckTaskReady(&task_pv_game))
        return true;

    task_pv_game.DelTask();
    return false;
}
