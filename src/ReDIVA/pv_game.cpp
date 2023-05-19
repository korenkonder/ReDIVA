/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/rob/motion.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/rob/skin_param.hpp"
#include "../CRE/data.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/pv_param.hpp"
#include "../CRE/sound.hpp"
#include "../CRE/stage.hpp"
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

struct struc_14 {
    int32_t field_0;
    int32_t difficulty;
    int32_t edition;
    uint32_t pv;
    int8_t field_10;
    int8_t field_11;
    int8_t field_12;
    int8_t field_13;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    float_t field_20;
    int8_t field_24;
    int8_t field_25;
    int8_t field_26;
    int8_t field_27;
};

pv_game* _pv_game;
TaskPvGame* task_pv_game;
pv_game_music* pv_game_music_data;

struc_14 stru_140C94438;

extern render_context* rctx_ptr;

static pv_game_music* pv_game_music_get();

static struc_14* sub_14013C8C0();

pv_play_data_motion_data::pv_play_data_motion_data() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

pv_play_data_motion_data::~pv_play_data_motion_data() {

}

void pv_play_data_motion_data::clear() {
    set_motion.clear();
    set_motion.shrink_to_fit();
    set_item.clear();
    set_item.shrink_to_fit();
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

pv_data_camera::pv_data_camera() : enable(), time() {
    id = -1;
}

pv_game_pv_data::pv_game_pv_data() : field_0(), state(), play(), dsc_buffer(), dsc_buffer_counter(),
field_2BF30(), field_2BF38(), curr_time(), curr_time_float(), prev_time_float(), field_2BF50(), field_2BF58(),
field_2BF60(), field_2BF64(), field_2BF68(), chara_id(), pv_game()/*, field_2BF80()*/, music(),
dsc_file_handler(), target_anim_fps(), anim_frame_speed(), target_flying_time(), time_signature(), dsc_time(),
field_2BFB0(), field_2BFB4(), field_2BFB8(), field_2BFBC(), field_2BFC0(), music_play(), field_2BFC4(), pv_end(),
fov(), min_dist(), field_2BFD4(), field_2BFD5(), data_camera(), change_field_branch_time(), scene_fade_enable(),
scene_fade_frame(), scene_fade_duration(), scene_fade_start_alpha(), scene_fade_end_alpha(), scene_fade_color(),
has_signature(), has_perf_id(), field_2C4E4(), field_2C500(), scene_rot_y(), scene_rot_mat(), field_2C54C(),
field_2C550(), branch_mode(), first_challenge_note(), last_challenge_note(), field_2C560() {

}

pv_game_pv_data::~pv_game_pv_data() {

}

void pv_game_pv_data::reset() {
    pv_game = 0i64;
    //field_2BF80 = 0;
    music = 0i64;
    dsc_file_handler.reset();
    play = false;
    field_2BF50 = false;

    //if (sub_1400E7910(sub_14013C8C0()) >= 6)
    //    field_2BF60 = false;

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
    change_field_branch_time = -1i64;
    scene_fade_enable = false;
    scene_fade_frame = 0.0f;
    scene_fade_duration = 0.0f;
    scene_fade_start_alpha = 0.0f;
    scene_fade_end_alpha = 0.0f;
    scene_fade_color = 0.0f;

    reset_camera_post_process();

    field_2C4E8.clear();
    field_2C4E8.shrink_to_fit();
    field_2C500 = 0;
    field_0 = 0;
}

void pv_game_pv_data::reset_camera_post_process() {
    camera* cam = rctx_ptr->camera;
    //cam->set_ignore_min_dist(false);
    cam->set_max_distance(0.05);
    //cam->set_ignore_fov(0);
    cam->set_fov(32.2673416137695);

    post_process* pp = &rctx_ptr->post_process;
    pp->tone_map->set_scene_fade_color(0.0f);
    pp->tone_map->set_scene_fade_alpha(0.0f);
    pp->tone_map->set_scene_fade_alpha(0);
    pp->tone_map->set_tone_trans(0.0f, 1.0f);
    pp->tone_map->set_saturate_coeff(1.0f);

    shadow* shad = rctx_ptr->render_manager.shadow_ptr;
    if (shad)
        for (bool& i : shad->blur_filter_enable)
            i = true;

    branch_mode = false;

    for (pv_data_camera& i : data_camera)
        i = {};
}

pv_game_play_data::pv_game_play_data() : pv_set(), aet_ids(), aet_ids_enable(), field_1C8(), field_1D0(),
field_1D8(), field_1E0(), field_1E8(), field_1F0(), disp(), field_1FC(), field_204(), field_20C(),
frame_state(), field_218(), fade_begin(), fade_end(), fade(), skin_danger_frame(), field_2FC(),
chance_result(), chance_points(), field_310(), slide_points(), max_slide_points(), pv_spr_set_id(),
pv_aet_set_id(), pv_main_aet_id(), stage_index(), field_33C(), field_350(), field_354(), value_text_spr_index(),
not_clear(), spr_set_back_id(), loaded(), state(), field_36C(), score_preview_update(), field_374(),
field_378(), field_37C(), score_preview_speed(), score_preview(), value_text_display(), field_38C(),
value_text_index(), value_text_time_offset(), combo_state(), combo_count(), combo_disp_time(), field_3AC(),
field_3B0(), field_3B4(), field_3B8(), field_3BC(),  aix(), ogg(), field_3C4(), field_5E8(), field_5EC(),
option(), field_5F4(), lyric(), lyric_index(), field_64C(), field_650(), field_654(), field_658(), field_65C() {
    chance_points_pos = 0;
    slide_points_pos = 0;
    max_slide_points_pos = 0;
    reset();
    state = 0;
}

pv_game_play_data::~pv_game_play_data() {

}

void pv_game_play_data::reset() {
    loaded = false;
    score_preview_update = false;
    field_378 = false;
    score_preview_speed = 0.04f;
    field_36C = -1;
    combo_state = 4;
    pv_set = false;
    stage_index = 0;
    field_354 = false;
    not_clear = false;
    pv_spr_set_id = -1;
    pv_aet_set_id = -1;
    pv_main_aet_id = -1;

    sub_1401362C0();
}

void pv_game_play_data::sub_1401362C0() {
    for (int32_t i = 0; i < PV_GAME_AET_MAX; i++) {
        aet_ids[i] = 0;
        aet_ids_enable[i] = false;
    }

    for (int32_t i = 0; i < 2; i++) {
        field_1C8[i] = 0.0f;
        field_1D0[i] = 0.0f;
        field_1D8[i] = 0.0f;
        field_1E0[i] = 0.0f;
        field_1E8[i] = 0.0f;
        field_1F0[i] = 0.0f;
        disp[i] = false;
        field_1FC[i] = 0;
    }

    field_1C8[0] = 86.0f;
    field_1C8[1] = 138.0f;
    field_1D8[0] = 58.0f;
    field_1D8[1] = 138.0f;
    field_1E8[0] = 58.0f;
    field_1E8[1] = -1.0f;
    field_1F0[0] = 86.0f;
    field_1F0[1] = -1.0f;
    field_204[0] = 58.0f;
    field_204[1] = 138.0f;
    field_20C[0] = 86.0f;
    field_20C[1] = 138.0f;

    frame_state = 0;
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
    score_preview = 0;
    value_text_display = 0;
    field_38C = 0.0f;
    value_text_index = 0;
    value_text_time_offset = 0.0f;
    combo_count = 0;
    combo_disp_time = 0;
    draw_pos = 0.0f;
}

pv_game_chara::pv_game_chara() : chara_index(), cos(), chara_id(), pv_data(), rob_chr() {

}

pv_game_chara::~pv_game_chara() {

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

pv_game_data::pv_game_data() : field_2CE98(), field_2CE9C(), field_2CF1C(), field_2CF20(), field_2CF24(),
field_2CF28(), field_2CF2C(), field_2CF30(), field_2CF34(), field_2CF38(), life_gauge(), score_final(),
challenge_time_total_bonus(), combo(), challenge_time_combo_count(), max_combo(), total_hit_count(),
hit_count(), current_reference_score(), target_count(), field_2CF84(), field_2CF88(), field_2CF8C(),
score_slide_chain_bonus(), slide_chain_length(), field_2CF98(), field_2CF9C(), field_2CFA0(), field_2CFA4(),
pv(), field_2CFB0(), chance_point(), reference_score(), field_2CFBC(), field_2CFC0(), field_2CFC4(),
field_2CFE0(), field_2CFE4(), notes_passed(), field_2CFF0(), song_energy(), song_energy_base(),
song_energy_border(), life_gauge_safety_time(), life_gauge_border(), stage_index(), field_2D00C(),
no_fail(), changed_field(), challenge_time_start(), challenge_time_end(), max_time(), max_time_float(),
current_time_float(), current_time(), current_time_float_dup(), field_2D03C(), score_hold_multi(),
score_hold(), score_slide(), has_slide(), has_success_se(), pv_disp2d(), life_gauge_final(),
hit_border(), field_2D05D(), field_2D05E(), field_2D05F(), title_image_init(), field_2D061(),
field_2D062(), field_2D063(), has_auth_3d_frame(), has_light_frame(), has_aet_frame(), has_aet_list(),
field_index(), edit_effect_index(), slidertouch_counter(), change_field_branch_success_counter(),
field_2D080(), field_2D084(), life_gauge_bonus(), life_gauge_total_bonus(), field_2D090(), field_2D091(),
field_2D092(), field_2D093(), field_2D094(), field_2D095(), field_2D096(), success(), field_2D098(),
field_2D09C(), use_osage_play_data(), pv_end_fadeout(), rival_percentage(), field_2D0A8(), field_2D0AC(),
field_2D0B0(), field_2D0BC(), next_stage(), field_2D0BE(), field_2D0BF(), field_2D0C0(), field_2D0C4(),
field_2D7E8(), field_2D7EC(), field_2D808(), field_2D80C(), edit_effect(), camera_auth_3d_uid(),
field_2D874(), task_effect_init(), field_2D87C(), current_field(), field_2D8A0(), field_2D8A4(),
data_camera_id(), field_2D954(), field_2DAC8(), field_2DACC(), field_2DB28(), field_2DB2C(), field_2DB34() {
    field_2D00E = true;
}

pv_game_data::~pv_game_data() {

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
    data.play_data.option = 0.0f;

    data.field_2CF98 = 0;
    data.song_energy_base = 0.0f;
    data.life_gauge_safety_time = 0;
    data.life_gauge_border = 0;
    data.stage_index = 0;
    data.field_2D00C = true;
    data.challenge_time_start = -1;
    data.challenge_time_end = -1;
    data.field_2D061 = false;
    data.field_2D062 = false;
    data.field_2D063 = false;
    data.field_2D0BC = false;
    data.next_stage = false;
    data.field_2D0BE = false;
    data.field_2D0C0 = -1;
    data.no_fail = true;
    data.task_effect_init = false;

    data.effect_rs_list_hashes.reserve(0x40);
    data.effect_rs_list.reserve(0x40);
}

pv_game::~pv_game() {

}

void pv_game::ctrl() {

}

void pv_game::disp() {

}

std::string pv_game::get_effect_se_file_name() {
    pv_db_pv_difficulty* diff = data.pv->get_difficulty(
        sub_14013C8C0()->difficulty, sub_14013C8C0()->edition);
    if (diff)
        return diff->effect_se_file_name;
    return {};
}

void pv_game::load() {

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

    data.play_data.lyric_index = 0;
    data.play_data.lyrics_color = { 0xFF, 0xFF, 0xFF, 0xFF };

    memset(data.field_2CE9C, 0i64, sizeof(data.field_2CE9C));
    data.field_2CF1C = 0;
    data.field_2CF20 = 0;
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

    for (auto& i : data.chara) {
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

    data.edit_effect_spr_set_ids.clear();
    data.edit_effect_aet_ids.clear();
    data.edit_effect.field_0 = -1;
    data.edit_effect.field_4 = 1.0f;
    data.edit_effect.field_8 = 0.0f;
    data.edit_effect.field_C = 0.0f;
    data.edit_effect.field_10 = 1.0f;
    data.edit_effect.field_14 = false;

    data.current_field = 0;
    data.field_data.clear();

    data.field_2D8A0 = true;

    data.camera_auth_3d_uid = -1;
    data.campv_string.clear();
    data.campv_auth_3d_uids.clear();
    data.data_camera_id = 0;
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
    data.field_2CFBC = 0;
    data.field_2CFC0 = 0;
    data.field_2CFC8.clear();
    data.field_2CFE0 = 0;
    data.field_2D092 = data.field_2D00E;
    data.notes_passed = 0;
    data.field_2CFF0 = 0;
    data.song_energy = 0.0f;
    data.song_energy_border = 0.0f;
    data.max_time_float = 0.0f;
    data.current_time_float = 0.0f;
    data.max_time = 0;
    data.current_time = 0;
    data.current_time_float_dup = 0.0f;
    data.field_2D03C = 0.0f;
    data.score_hold_multi = 0;
    data.score_hold = 0;
    data.score_slide = 0;
    data.has_slide = false;
    data.has_success_se = false;
    data.pv_disp2d = 0;
    data.hit_border = false;
    data.field_2D05D = 0;
    data.field_2D05F = 1;
    data.title_image_init = false;
    data.field_index = 0;
    data.edit_effect_index = -1;
    data.slidertouch_counter = 0;
    data.change_field_branch_success_counter = 0;
    data.field_2D05E = 0;
    data.field_2D080 = 0;
    data.field_2D084 = 0;
    data.life_gauge_bonus = 0;
    data.life_gauge_total_bonus = 0;
    data.field_2D090 = 0;
    data.field_2D091 = 0;
    data.field_2D093 = 0;
    data.field_2D094 = 0;
    data.field_2D095 = false;
    data.field_2D096 = false;
    data.use_osage_play_data = false;
    data.pv_end_fadeout = false;
    data.field_2D098 = 0;
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
    data.play_data.field_5E8 = -1;
    data.field_2DB28 = 0;
    data.field_2DB2C = 0;
    data.play_data.aix = false;
    data.play_data.ogg = false;
    data.play_data.field_64C = 0;
    data.field_2CF30 = 4;
    data.field_2CF34 = 0;
    data.play_data.field_650 = 43;
    data.play_data.field_654 = 0.0f;
    data.field_2CF24 = 3;
    data.field_2CF28 = 0.0f;
    data.field_2CF38 = 0;
    data.play_data.field_658 = 0;
    data.play_data.field_65C = 0.0f;
    data.field_2CF2C = 0.0;
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
    data.pv = 0i64;
    data.score_slide_chain_bonus = 0;
    data.slide_chain_length = 0;
    data.field_2D0BC = false;
    data.next_stage = false;
    data.field_2D0BE = false;
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

    data.success = data.field_2D063;
    data.field_2CE98 = 0;
    data.field_2D874 = -1;
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
        int32_t frame_texture_index = 0;
        for (pv_db_pv_frame_texture& i : data.pv->frame_texture) {
            if (i.data.size()) {
                texture* tex = texture_storage_get_texture(aft_tex_db->get_texture_id(i.data.c_str()));
                //if (tex)
                //    post_process_free_frame_texture(&pp, frame_texture_index, tex);
            }
            frame_texture_index++;
        }

        if (data.field_2D0BE) {
            //post_process_reset_frame_textures(&pp);
            data.field_2D0BE = false;
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

    for (auto& i : data.chara) {
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

    //osage_play_data_manager_clear_data();

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
        for (int32_t& i : data.spr_set_back_ids)
            sprite_manager_unload_set(i, aft_spr_db);

    data.field_2D7E8 = false;
    data.spr_set_back_ids.clear();

    data.current_field = 0;
    data.field_data.clear();

    data.field_2D8A0 = true;

    for (auto& i : data.auth_3d)
        i.second.unload_id(rctx_ptr);

    for (auto& i : data.field_2DA08)
        for (auto& j : i.auth_3d_ids)
            j.unload_id(rctx_ptr);

    if (data.field_2DAC8)
        for (std::string& i : data.auth_3d_categories)
            auth_3d_data_unload_category(i.c_str());

    data.camera_auth_3d_uid = -1;
    data.campv_string.clear();
    data.campv_auth_3d_uids.clear();
    data.data_camera_id = 0;
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

    for (int32_t& i : data.edit_effect_aet_set_ids)
        sprite_manager_unload_set(i, aft_spr_db);
    data.edit_effect_aet_set_ids.clear();

    for (int32_t& i : data.edit_effect_spr_set_ids)
        aet_manager_unload_set(i, aft_aet_db);

    data.edit_effect_spr_set_ids.clear();
    data.edit_effect_aet_ids.clear();
    data.edit_effect.field_0 = -1;
    data.edit_effect.field_4 = 1.0f;
    data.edit_effect.field_8 = 0.0f;
    data.edit_effect.field_C = 0.0f;
    data.edit_effect.field_10 = 1.0f;
    data.edit_effect.field_14 = false;

    //pv_param::light_data_clear_data();
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
        TaskMovie* task_movie = task_movie_get(i16);
        if (!app::TaskWork::CheckTaskReady(task_movie))
            continue;

        sub_14041F4E0(task_movie);
        app::Task::DelTask(task_movie);
    }*/
    
    data.field_2D098 = 0;

    //stage_param_data_coli_data_clear();

    data.play_data.field_3B0 = -1;
    data.play_data.field_3B4 = -1;
    data.play_data.field_3B8 = -1;
    data.play_data.field_3BC = -1;

    //sub_14012BDA0(sub_14013C8F0());

    //data.play_data->unload();

    data.play_data.field_64C = 0;

    field_1 = false;
    field_2 = false;
    end_pv = false;

    //sub_14013C8C0()->field_26 = 0;

    /*struc_14* v85 = sub_14013C8C0();
    v85->field_14 = 0;
    v85->field_18 = 0;
    v85->field_1C = 0;
    v85->field_20 = 1.0;*/

    //sub_14013C8C0()->field_24 = false;
    //sub_14013C8C0()->field_25 = false;

    loaded = false;

    data.field_2CFC8.clear();
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

pv_game_init_data::pv_game_init_data() : pv_id(), difficulty(), field_C(), score_percentage_clear(),
life_gauge_safety_time(), life_gauge_border(), field_18(), modules(), items() {

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
    for (pv_game_item_mask& i : items_mask)
        i = {};
}

TaskPvGame::Args::Args() : init_data(), field_190(), field_191(), no_fail(),
field_193(), field_194(), field_195(), field_196(), field_197(), test_pv(), option() {
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
    field_191 = true;
    no_fail = false;
    field_193 = true;
    field_194 = false;
    field_195 = false;
    field_196 = false;
    field_197 = false;
    test_pv = false;
    option = 0;
}

TaskPvGame::Data::Data() : field_0(), init_data(), field_190(), field_191(), no_fail(),
field_193(), field_194(), field_195(), field_196(), option() {
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
    field_0 = 1;
    init_data.reset();
    Clear();
    field_190 = false;
    field_191 = true;
    no_fail = false;
    field_193 = true;
    field_194 = false;
    field_195 = false;
    field_196 = false;
    option = 0;
}

TaskPvGame::TaskPvGame() {

}

TaskPvGame::~TaskPvGame() {

}

bool TaskPvGame::Init() {
    //Load(data);
    //touch_util::touch_reaction_set_enable(0);
    return true;
}

bool TaskPvGame::Ctrl() {
    return false;
}

bool TaskPvGame::Dest() {
    return true;
}

void TaskPvGame::Disp() {

}

bool task_pv_game_add_task(TaskPvGame::Args& args) {
    return true;
    if (!task_pv_game)
        task_pv_game = new TaskPvGame;

    task_pv_game->data.field_0 = 1;
    task_pv_game->data.init_data = args.init_data;
    task_pv_game->data.se_name.assign(args.se_name);
    task_pv_game->data.slide_se_name.assign(args.slide_se_name);
    task_pv_game->data.chainslide_first_name.assign(args.chainslide_first_name);
    task_pv_game->data.chainslide_sub_name.assign(args.chainslide_sub_name);
    task_pv_game->data.chainslide_success_name.assign(args.chainslide_success_name);
    task_pv_game->data.chainslide_failure_name.assign(args.chainslide_failure_name);
    task_pv_game->data.slidertouch_name.assign(args.slidertouch_name);
    task_pv_game->data.field_190 = args.field_190;
    task_pv_game->data.field_191 = !args.field_195;
    task_pv_game->data.no_fail = args.no_fail;
    if (args.field_191) {
        task_pv_game->data.field_0 = 2;
        task_pv_game->data.field_193 = args.field_193;
    }
    else
        task_pv_game->data.field_193 = 1;
    task_pv_game->data.field_194 = args.field_195;
    task_pv_game->data.field_195 = args.field_196;
    task_pv_game->data.field_196 = args.field_197;
    task_pv_game->data.option = args.option;

    if (args.test_pv)
        return app::TaskWork::AddTask(task_pv_game, "PVGAME");
    else
        return app::TaskWork::AddTask(task_pv_game, 0, "PVGAME");
}

bool task_pv_game_check_task_ready() {
    return app::TaskWork::CheckTaskReady(task_pv_game);
}

bool task_pv_game_del_task() {
    return true;
    if (!app::TaskWork::CheckTaskReady(task_pv_game)) {
        delete task_pv_game;
        task_pv_game = 0;
        return true;
    }

    task_pv_game->DelTask();
    return false;
}

pv_game_music_args::pv_game_music_args() : type(), start(), field_2C() {

}

pv_game_music_args::~pv_game_music_args() {

}

pv_game_music_ogg::pv_game_music_ogg() : playback() {
    playback = ogg_playback_data_get(0);
}

pv_game_music_ogg::~pv_game_music_ogg() {
    if (playback) {
        playback->Reset();
        playback = 0;
    }
}

pv_game_music::pv_game_music() : flags(), pause(), channel_pair_volume(), fade_in(),
fade_out(), no_fade(), no_fade_remain(), fade_out_time_req(), fade_out_action_req(),
type(), start(), end(), play_on_end(), fade_in_time(), field_9C(), loaded(), ogg() {
    volume = 100;
    master_volume = 100;
    for (int32_t& i : channel_pair_volume)
        i = 100;
    fade_out_time = 3.0f;
}

pv_game_music::~pv_game_music() {

}

bool pv_game_music::check_args(int32_t type, std::string&& file_path, float_t start) {
    return args.type == type && !args.file_path.compare(file_path) && args.start == start;
}

void pv_game_music::ctrl(float_t delta_time) {
    if (no_fade && !pause) {
        no_fade_remain -= delta_time;
        if (no_fade_remain <= 0.0f) {
            fade_out.enable = true;
            fade_out.start = get_volume(0);
            fade_out.value = 0;
            fade_out.time = fade_out_time_req;
            fade_out.remain = fade_out_time_req;
            fade_out.action = fade_out_action_req;

            no_fade = false;
            no_fade_remain = 0.0f;
            fade_out_time_req = 0.0f;
            fade_out_action_req = PV_GAME_MUSIC_ACTION_NONE;
        }
    }

    if (fade_in.enable && !pause) {
        fade_in.remain -= delta_time;
        if (fade_in.remain > 0.0f) {
            int32_t value = fade_in.value;
            if (fade_in.time > 0.0f)
                value = (int32_t)((float_t)value - (float_t)(value
                    - fade_in.start) * (fade_in.remain / fade_in.time));
            set_volume_map(0, value);
        }
        else
            fade_in_end();
    }

    if (fade_out.enable && !pause) {
        fade_out.remain -= delta_time;
        if (fade_out.remain > 0.0f) {
            int32_t value = fade_out.value;
            if (fade_out.time > 0.0f)
                value = (int32_t)((float_t)value - (float_t)(value
                    - fade_out.start) * (fade_out.remain / fade_out.time));
            set_volume_map(0, value);
        }
        else
            fade_out_end();
    }

    if (flags & PV_GAME_MUSIC_OGG) {
        OggPlayback* playback = ogg->playback;
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            playback->Stop();
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            play_or_stop();
        OggPlayback::SetChannelPairVolumePan(playback);
    }
}

void pv_game_music::exclude_flags(pv_game_music_flags flags) {
    if ((this->flags & PV_GAME_MUSIC_OGG) && (flags & PV_GAME_MUSIC_OGG))
        ogg_free();
    enum_and(this->flags, ~flags);
}

void pv_game_music::fade_in_end() {
    if (!fade_in.enable)
        return;

    fade_in.enable = false;
    fade_in.remain = 0.0f;
    set_volume_map(0, fade_in.value);

    switch (fade_in.action) {
    case PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void pv_game_music::fade_out_end() {
    if (!fade_out.enable)
        return;

    fade_out.enable = false;
    fade_out.remain = 0.0f;
    set_volume_map(0, fade_out.value);

    switch (fade_out.action) {
    case PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void pv_game_music::file_load(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    this->type = type;
    this->file_path.assign(file_path);
    this->start = start;
    this->end = end;
    this->play_on_end = play_on_end;
    this->fade_out_time = fade_out_time;
    this->fade_in_time = fade_in_time;
    this->field_9C = a9;

    if (fade_in_time > 0.0f) {
        fade_in.start = 0;
        fade_in.value = 100;
        fade_in.time = fade_in_time;
        fade_in.remain = fade_in_time;
        fade_in.action = PV_GAME_MUSIC_ACTION_NONE;
        fade_in.enable = true;
    }

    if (end <= 0.0f) {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = 0.0f;
        fade_out.remain = 0.0f;
        fade_out.action = play_on_end
            ? PV_GAME_MUSIC_ACTION_PLAY : PV_GAME_MUSIC_ACTION_STOP;
    }
    else {
        no_fade = true;
        no_fade_remain = end + fade_in_time;
        fade_out_time_req = fade_out_time;
        fade_out_action_req = play_on_end
            ? PV_GAME_MUSIC_ACTION_PLAY : PV_GAME_MUSIC_ACTION_STOP;
    }
}

int32_t pv_game_music::get_master_volume(int32_t index) {
    if (!index)
        return master_volume;
    return 0;
}

int32_t pv_game_music::get_volume(int32_t index) {
    if (!index)
        return volume;
    return 0;
}

int32_t pv_game_music::include_flags(pv_game_music_flags flags) {
    if (!(this->flags & PV_GAME_MUSIC_OGG) && (flags & PV_GAME_MUSIC_OGG))
        ogg_init();
    enum_or(this->flags, flags);
    return 0;
}

int32_t pv_game_music::load(int32_t type, std::string&& file_path, bool wait_load, float_t time, bool a6) {
    if (type == 4 && (set_ogg_args(std::string(file_path), time, wait_load) < 0))
        return -6;

    reset();
    reset_args();
    set_args(type, std::string(file_path), time, a6);
    pause = true;
    return 0;
}

void pv_game_music::ogg_free() {
    if (ogg) {
        delete ogg;
        ogg = 0;
    }
}

int32_t pv_game_music::ogg_init() {
    if (ogg)
        return 0;

    ogg = new pv_game_music_ogg;
    return ogg->playback ? 0 : -1;
}

int32_t pv_game_music::ogg_load(std::string&& file_path, float_t start) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
    }

    playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    ogg->file_path.assign(file_path);
    loaded = true;
    return 0;
}

int32_t pv_game_music::ogg_reset() {
    sound_stream_array_reset();
    include_flags(PV_GAME_MUSIC_OGG);
    return 0;
}

int32_t pv_game_music::ogg_stop() {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->Stop();
    ogg->file_path.clear();
    loaded = false;
    return 0;
}

int32_t pv_game_music::play() {
    return play(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, false);
}

int32_t pv_game_music::play(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    set_volume_map(0, 100);
    if (type == 4 && ogg_load(std::string(file_path), start) < 0)
        return -7;

    reset();
    reset_args();
    file_load(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, a9);
    pause = false;
    return 0;
}

void pv_game_music::play_fade_in(int32_t type, std::string&& file_path, float_t start,
    float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10) {
    play(type, std::string(file_path), play_on_end, start, end, 0.0f, fade_out_time, a10);
}

int32_t pv_game_music::play_or_stop() {
    if (play_on_end)
        return play();
    else
        return stop();
}

void pv_game_music::reset() {
    fade_in = {};
    fade_out = {};
    no_fade = false;
    no_fade_remain = 0.0f;
    fade_out_time_req = 0.0f;
    fade_out_action_req = PV_GAME_MUSIC_ACTION_NONE;
    type = 0;
    file_path.clear();
    start = 0.0f;
    end = 0.0f;
    play_on_end = false;
    fade_in_time = 0.0f;
    fade_out_time = 3.0f;
    field_9C = false;
}

void pv_game_music::reset_args() {
    args.type = 0;
    args.file_path.clear();
    args.start = 0.0f;
    args.field_2C = false;
}

void pv_game_music::set_args(int32_t type, std::string&& file_path, float_t start, bool a5) {
    args.type = type;
    args.file_path.assign(file_path);
    args.start = start;
    args.field_2C = a5;
}

int32_t pv_game_music::set_channel_pair_volume(int32_t channel_pair, int32_t value) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetChannelPairVolume(channel_pair, ratio_to_db((float_t)value * 0.01f));
    return 0;
}

int32_t pv_game_music::set_channel_pair_volume_map(int32_t channel_pair, int32_t value) {
    if (channel_pair >= 0 && channel_pair <= 3) {
        set_channel_pair_volume(channel_pair, value);
        if (!channel_pair)
            set_channel_pair_volume_map(2, value);
        else if (channel_pair == 1)
            set_channel_pair_volume_map(3, value);
        channel_pair_volume[channel_pair] = value;
        return 0;
    }
    return -1;
}

int32_t pv_game_music::set_fade_out(float_t time, bool stop) {
    reset();
    if (time == 0.0f)
        pv_game_music::stop();
    else {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = time;
        fade_out.remain = time;
        fade_out.enable = true;
        fade_out.action = stop ? PV_GAME_MUSIC_ACTION_STOP
            : PV_GAME_MUSIC_ACTION_PAUSE;
    }
    return 0;
}

int32_t pv_game_music::set_master_volume(int32_t value) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetMasterVolume(ratio_to_db(
        (float_t)(value * get_master_volume(0) / 100) * 0.01f));
    return 0;
}

int32_t pv_game_music::set_ogg_args(std::string&& file_path, float_t start, bool wait_load) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
        playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PAUSE);
        ogg->file_path.assign(file_path);

        if (wait_load) {
            OggFileHandlerFileState file_state = playback->GetFileState();
            while (file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
                && file_state != OGG_FILE_HANDLER_FILE_STATE_STOPPED
                && file_state != OGG_FILE_HANDLER_FILE_STATE_MAX)
                file_state = playback->GetFileState();
        }
    }
    return 0;
}

int32_t pv_game_music::set_ogg_pause_state(uint8_t pause_state) {
    if ((flags & PV_GAME_MUSIC_OGG) == 0)
        return -2;

    OggPlayback* playback = ogg->playback;
    if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_PLAYING)
        playback->SetPauseState((OggFileHandlerPauseState)pause_state);
    return 0;
}

int32_t pv_game_music::set_pause(int32_t pause) {
    if ((flags & PV_GAME_MUSIC_OGG) != 0)
        set_ogg_pause_state(pause != 0);
    pause = pause == 1;
    return 0;
}

void pv_game_music::set_volume_map(int32_t index, int32_t value) {
    if (!index) {
        if (flags & PV_GAME_MUSIC_OGG)
            set_master_volume(value);
        volume = value;
    }
}

int32_t pv_game_music::stop() {
    if (flags & PV_GAME_MUSIC_OGG)
        ogg_stop();
    reset();
    reset_args();
    pause = false;
    return 0;
}

void pv_game_music::stop_reset_flags() {
    stop();
    exclude_flags(PV_GAME_MUSIC_ALL);
}

static pv_game_music* pv_game_music_get() {
    return pv_game_music_data;
}

static struc_14* sub_14013C8C0() {
    return &stru_140C94438;
}
