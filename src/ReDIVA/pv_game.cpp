/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/data.hpp"
#include "../CRE/object.hpp"
#include "../CRE/ogg_vorbis.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/pv_param.hpp"
#include "../CRE/sound.hpp"
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

enum pv_game_music_action {
    PV_GAME_MUSIC_ACTION_NONE = 0,
    PV_GAME_MUSIC_ACTION_STOP,
    PV_GAME_MUSIC_ACTION_PAUSE,
    PV_GAME_MUSIC_ACTION_PLAY,
    PV_GAME_MUSIC_ACTION_MAX,
};

enum pv_game_music_flags : uint8_t {
    PV_GAME_MUSIC_OGG = 0x04,
    PV_GAME_MUSIC_ALL = 0xFF,
};

struct pv_game_music_fade {
    int32_t start;
    int32_t value;
    float_t time;
    float_t remain;
    pv_game_music_action action;
    bool enable;

    void reset();
};

struct pv_game_music_args {
    int32_t type;
    std::string file_path;
    float_t start;
    bool field_2C;

    pv_game_music_args();
    ~pv_game_music_args();
};

struct pv_game_music_ogg {
    OggPlayback* playback;
    std::string file_path;

    pv_game_music_ogg();
    ~pv_game_music_ogg();
};

struct pv_game_music {
    pv_game_music_flags flags;
    bool pause;
    int32_t volume;
    int32_t master_volume;
    int32_t channel_pair_volume[4];
    pv_game_music_fade fade_in;
    pv_game_music_fade fade_out;
    bool no_fade;
    float_t no_fade_remain;
    float_t fade_out_time_req;
    pv_game_music_action fade_out_action_req;
    int32_t type;
    std::string file_path;
    float_t start;
    float_t end;
    bool play_on_end;
    float_t fade_in_time;
    float_t fade_out_time;
    bool field_9C;
    pv_game_music_args args;
    bool loaded;
    pv_game_music_ogg* ogg;

    pv_game_music();
    ~pv_game_music();

    bool check_args(int32_t type, std::string& file_path, float_t start);
    void ctrl(float_t delta_time);
    void exclude_flags(pv_game_music_flags flags);
    void fade_in_end();
    void fade_out_end();
    void file_load(int32_t type, std::string& file_path, bool play_on_end,
        float_t start, float end, float fade_in_time, float_t fade_out_time, bool a9);
    int32_t get_master_volume(int32_t index);
    int32_t get_volume(int32_t index);
    int32_t include_flags(pv_game_music_flags flags);
    int32_t load(int32_t type, std::string& file_path, bool wait_load, float_t time, bool a6);
    void ogg_free();
    int32_t ogg_init();
    int32_t ogg_load(std::string& file_path, float_t start);
    int32_t ogg_reset();
    int32_t ogg_stop();
    int32_t play();
    int32_t play(int32_t type, std::string& file_path, bool play_on_end,
        float_t start, float end, float fade_in_time, float_t fade_out_time, bool a9);
    void play(int32_t type, std::string& file_path, float_t start,
        float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10);
    int32_t play_or_stop();
    void reset();
    void reset_args();
    void set_args(int32_t type, std::string& file_path, float_t start, bool a5);
    int32_t set_channel_pair_volume(int32_t channel_pair, int32_t value);
    int32_t set_channel_pair_volume_map(int32_t channel_pair, int32_t value);
    int32_t set_fade_out(float_t time, uint8_t stop);
    int32_t set_master_volume(int32_t value);
    int32_t set_ogg_args(std::string& file_path, float_t start, bool wait_load);
    int32_t set_ogg_pause_state(uint8_t pause_state);
    int32_t set_pause(int32_t pause);
    void set_volume_map(int32_t index, int32_t value);
    int32_t stop();
    void stop_reset_flags();
};

pv_game* _pv_game;
TaskPvGame* task_pv_game;
pv_game_music* pv_game_music_data;

extern render_context* rctx_ptr;

static pv_game_music* pv_game_music_get();

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

pv_game_item_mask::pv_game_item_mask() {
    arr[0] = true;
    arr[1] = true;
    arr[2] = true;
    arr[3] = true;
}

pv_game::pv_game() : loaded(), field_1(), field_2(), end_pv(),
field_4(), state(), field_C(), pv_id(), field_14(), modules(), items() {

}

pv_game::~pv_game() {

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

void pv_game_music_fade::reset() {
    start = 0;
    value = 0;
    time = 0.0f;
    remain = 0.0f;
    action = PV_GAME_MUSIC_ACTION_NONE;
    enable = false;
}

pv_game_music_args::pv_game_music_args() : type(), start(), field_2C() {

}

pv_game_music_args::~pv_game_music_args() {

}

pv_game_music_ogg::pv_game_music_ogg() : playback() {

}

pv_game_music_ogg::~pv_game_music_ogg() {

}

pv_game_music::pv_game_music() : flags(), pause(), fade_in(), fade_out(), no_fade(),
no_fade_remain(), fade_out_time_req(), fade_out_action_req(), type(), start(), end(),
play_on_end(), fade_in_time(), fade_out_time(), field_9C(), loaded(), ogg() {
    volume = 100;
    master_volume = 100;
    channel_pair_volume[0] = 100;
    channel_pair_volume[1] = 100;
    channel_pair_volume[2] = 100;
    channel_pair_volume[3] = 100;
    reset();
}

pv_game_music::~pv_game_music() {

}

bool pv_game_music::check_args(int32_t type, std::string& file_path, float_t start) {
    bool ret = true;
    if (args.type != type)
        ret = false;
    if (args.file_path.compare(file_path))
        ret = false;
    if (args.start != start)
        ret = false;
    return ret;
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
                value = (int32_t)((float_t)value - (float_t)(value - fade_in.start) * (fade_in.remain / fade_in.time));
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
                value = (int32_t)((float_t)value - (float_t)(value - fade_out.start) * (fade_out.remain / fade_out.time));
            set_volume_map(0, value);
        }
        else
            fade_out_end();
    }

    if (flags & PV_GAME_MUSIC_OGG) {
        if (ogg->playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            ogg->playback->Stop();
        if (ogg->playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            play_or_stop();
        OggPlayback::SetChannelPairVolumePan(ogg->playback);
    }
}

void pv_game_music::exclude_flags(pv_game_music_flags flags) {
    if ((this->flags & PV_GAME_MUSIC_OGG) && (flags & PV_GAME_MUSIC_OGG))
        ogg_free();
    enum_and(flags, ~flags);
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
        set_pause(true);
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
        set_pause(true);
        break;
    case PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void pv_game_music::file_load(int32_t type, std::string& file_path, bool play_on_end,
    float_t start, float end, float fade_in_time, float_t fade_out_time, bool a9) {
    this->type = type;
    this->file_path.assign(file_path);
    this->start = start;
    this->end = end;
    this->play_on_end = play_on_end;
    this->fade_out_time = fade_out_time;
    this->fade_in_time = fade_in_time;
    field_9C = a9;

    if (fade_in_time > 0.0f) {
        fade_in.start = 0;
        fade_in.value = 100;
        fade_in.time = fade_in_time;
        fade_in.remain = fade_in_time;
        fade_in.enable = true;
        fade_in.action = PV_GAME_MUSIC_ACTION_NONE;
    }

    if (end <= 0.0f) {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = 0.0f;
        fade_out.remain = 0.0f;
        fade_out.action = play_on_end ? PV_GAME_MUSIC_ACTION_PLAY : PV_GAME_MUSIC_ACTION_STOP;
    }
    else {
        no_fade = true;
        no_fade_remain = end + fade_in_time;
        fade_out_time_req = fade_out_time;
        fade_out_action_req = play_on_end ? PV_GAME_MUSIC_ACTION_PLAY : PV_GAME_MUSIC_ACTION_STOP;
    }
}

int32_t pv_game_music::get_master_volume(int32_t index) {
    if (index)
        return 0;
    return master_volume;
}

int32_t pv_game_music::get_volume(int32_t index) {
    if (index)
        return 0;
    return volume;
}

int32_t pv_game_music::include_flags(pv_game_music_flags flags) {
    if (!(this->flags & PV_GAME_MUSIC_OGG) && (flags & PV_GAME_MUSIC_OGG))
        ogg_init();
    enum_or(this->flags, flags);
    return 0;
}

int32_t pv_game_music::load(int32_t type, std::string& file_path, bool wait_load, float_t time, bool a6) {
    if (type == 4 && (set_ogg_args(file_path, time, wait_load) < 0))
        return -6;

    reset();
    reset_args();
    set_args(type, file_path, time, a6);
    pause = true;
    return 0;
}

void pv_game_music::ogg_free() {
    if (!ogg)
        return;

    pv_game_music_ogg* ogg = this->ogg;
    if (ogg->playback) {
        ogg->playback->Reset();
        ogg->playback = 0;
    }
    delete ogg;
    this->ogg = 0;
}

int32_t pv_game_music::ogg_init() {
    if (ogg)
        return 0;

    ogg = new pv_game_music_ogg;
    ogg->playback = ogg_playback_data_get(0);
    if (ogg->playback)
        return 0;
    return -1;
}

int32_t pv_game_music::ogg_load(std::string& file_path, float_t start) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, file_path, start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);;
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
    return play(type, file_path, play_on_end, start, end, fade_in_time, fade_out_time, false);
}

int32_t pv_game_music::play(int32_t type, std::string& file_path, bool play_on_end,
    float_t start, float end, float fade_in_time, float_t fade_out_time, bool a9) {
    set_volume_map(0, 100);
    if (type == 4 && (ogg_load(file_path, start) < 0))
        return -7;

    reset();
    reset_args();
    file_load(type, file_path, play_on_end, start, end, fade_in_time, fade_out_time, a9);
    pause = false;
    return 0;
}

void pv_game_music::play(int32_t type, std::string& file_path, float_t start,
    float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10) {
    play(type, file_path, play_on_end, start, end, 0.0f, fade_out_time, a10);
}

int32_t pv_game_music::play_or_stop() {
    if (play_on_end)
        return play();
    else
        return stop();
}

void pv_game_music::reset() {
    fade_in.reset();
    fade_out.reset();
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

void pv_game_music::set_args(int32_t type, std::string& file_path, float_t start, bool a5) {
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
    if (channel_pair < 0 || channel_pair >= 4)
        return -1;

    set_channel_pair_volume(channel_pair, value);
    switch (channel_pair) {
    case 0:
        set_channel_pair_volume_map(2, value);
        break;
    case 1:
        set_channel_pair_volume_map(3, value);
        break;
    }
    channel_pair_volume[channel_pair] = value;
    return 0;
}

int32_t pv_game_music::set_fade_out(float_t time, uint8_t stop) {
    reset();
    if (time == 0.0f) {
        this->stop();
        return 0;
    }

    fade_out.start = get_volume(0);
    fade_out.value = 0;
    fade_out.time = time;
    fade_out.remain = time;
    fade_out.action = stop ? PV_GAME_MUSIC_ACTION_STOP : PV_GAME_MUSIC_ACTION_PAUSE;
    fade_out.enable = true;
    return 0;
}

int32_t pv_game_music::set_master_volume(int32_t value) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetMasterVolume(ratio_to_db((float)(value * get_master_volume(0) / 100) * 0.01f));
    return 0;
}

int32_t pv_game_music::set_ogg_args(std::string& file_path, float_t start, bool wait_load) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, file_path, start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
        playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PAUSE);
        ogg->file_path.assign(file_path);

        if (wait_load) {
            OggFileHandlerFileState file_state = playback->GetFileState();
            while (!(file_state == OGG_FILE_HANDLER_FILE_STATE_PLAYING
                    || file_state == OGG_FILE_HANDLER_FILE_STATE_STOPPED
                    || file_state == OGG_FILE_HANDLER_FILE_STATE_MAX))
                file_state = playback->GetFileState();
        }
    }
    return 0;
}

int32_t pv_game_music::set_ogg_pause_state(uint8_t pause_state) {
    if ((flags & PV_GAME_MUSIC_OGG) == 0)
        return -2;

    if (ogg->playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_PLAYING)
        ogg->playback->SetPauseState((OggFileHandlerPauseState)pause_state);
    return 0;
}

int32_t pv_game_music::set_pause(int32_t pause) {
    if ((flags & PV_GAME_MUSIC_OGG) != 0)
        set_ogg_pause_state(pause != 0);
    pause = pause == 1;
    return 0;
}

void pv_game_music::set_volume_map(int32_t index, int32_t value) {
    if (index)
        return;

    if (flags & PV_GAME_MUSIC_OGG)
        set_master_volume(value);
    volume = value;
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
