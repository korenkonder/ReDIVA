/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../CRE/rob/rob.hpp"
#include "../../KKdLib/dsc.hpp"

enum dsc_ft_func {
    DSC_FT_END = 0,
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

struct pv_play_data_set_motion {
    float_t frame_speed;
    uint32_t motion_id;
    float_t frame;
    float_t blend_duration;
    bool blend;
    MotionBlendType blend_type;
    bool disable_eye_motion;
    int32_t motion_index;
    int64_t dsc_time;
    float_t dsc_frame;

    pv_play_data_set_motion();

    void reset();
};

struct pv_play_data_motion {
    bool enable;
    int32_t motion_index;
    int64_t time;
};

struct pv_play_data_event {
    int32_t time;
    int32_t index;
    int32_t pv_branch_mode;
};

struct pv_play_data_motion_data {
    rob_chara* rob_chr;
    float_t current_time;
    float_t duration;
    vec3 start_pos;
    vec3 end_pos;
    float_t start_rot;
    float_t end_rot;
    float_t mot_smooth_len;
    std::vector<pv_play_data_event> set_motion;
    std::vector<pv_play_data_event> item_anim;

    pv_play_data_motion_data();
    ~pv_play_data_motion_data();

    void ctrl(float_t delta_time);
    void ctrl_inner();
    void clear();
    void reset();
    void set(rob_chara* rob_chr, float_t duration,
        vec3 start_pos, vec3 end_pos, float_t start_rot, float_t end_rot);
};

struct pv_play_data {
    std::vector<pv_play_data_motion> motion;
    rob_chara* rob_chr;
    std::list<pv_play_data_set_motion> set_motion;
    bool disp;
    bool shadow;
    pv_play_data_motion_data motion_data;

    pv_play_data();
    ~pv_play_data();

    pv_play_data_motion* get_motion(int32_t motion_index);
    void reset();
};

struct pv_data_camera {
    bool enable;
    int32_t index;
    int64_t time;

    pv_data_camera();
};

struct pv_scene_fade {
    bool enable;
    float_t time;
    float_t duration;
    float_t start_alpha;
    float_t end_alpha;
    vec3 color;

    pv_scene_fade();

    void ctrl(float_t delta_time);
    void reset();
};

struct pv_dsc_target {
    dsc_target_ft type;
    vec2 pos;
    vec2 start_pos;
    float_t amplitude;
    int32_t frequency;
    bool slide_chain_start;
    bool slide_chain_end;
    bool slide_chain_left;
    bool slide_chain_right;
    bool slide_chain;

    pv_dsc_target();
};

struct pv_dsc_target_group {
    int32_t target_count;
    pv_dsc_target targets[4];
    int32_t field_94;
    int64_t time_begin;
    int64_t time_end;
    bool slide_chain;

    pv_dsc_target_group();
};

struct struc_676 {
    bool field_0[ROB_CHARA_COUNT];
    pv_play_data_set_motion field_8[ROB_CHARA_COUNT];
    bool field_128[ROB_CHARA_COUNT];
    int32_t field_130[ROB_CHARA_COUNT];
    bool field_148[ROB_CHARA_COUNT];
    pv_play_data_set_motion field_150[ROB_CHARA_COUNT];
    float_t field_270[ROB_CHARA_COUNT];
    bool field_288[ROB_CHARA_COUNT];
    int32_t field_290[ROB_CHARA_COUNT];
    float_t field_2A8[ROB_CHARA_COUNT];
    bool field_2C0;
    int32_t field_2C4;
    int32_t field_2C8;
    float_t field_2CC;
    int64_t field_2D0;

    struc_676();

    void reset();
};

struct pv_game_pv_data {
    bool field_0;
    int32_t dsc_state;
    bool play;
    //uint32_t dsc_buffer[45000];
    //int32_t dsc_buffer_counter;
    dsc dsc;
    dsc_data* dsc_data_ptr;
    dsc_data* dsc_data_ptr_end;
    int64_t field_2BF30;
    int64_t field_2BF38;
    int64_t curr_time;
    float_t curr_time_float;
    float_t prev_time_float;
    bool field_2BF50;
    int64_t field_2BF58;
    bool field_2BF60;
    int32_t field_2BF64;
    int64_t field_2BF68;
    int32_t chara_id;
    struct pv_game* pv_game;
    //struc_162* field_2BF80;
    struct pv_game_music* music;
    p_file_handler dsc_file_handler;
    float_t measured_fps;
    float_t anim_frame_speed;
    float_t target_flying_time;
    int32_t time_signature;
    int64_t dsc_time;
    int32_t field_2BFB0;
    int32_t field_2BFB4;
    int32_t field_2BFB8;
    int32_t field_2BFBC;
    bool music_play;
    bool music_playing;
    int32_t field_2BFC4;
    bool pv_end;
    float_t fov;
    float_t min_dist;
    bool look_camera_enable;
    bool field_2BFD5;
    pv_play_data playdata[ROB_CHARA_COUNT];
    pv_data_camera data_camera[3];
    std::vector<int32_t> data_camera_branch_fail;
    std::vector<int32_t> data_camera_branch_success;
    std::vector<pv_data_set_motion> set_motion[ROB_CHARA_COUNT];
    prj::vector_pair<int32_t, int32_t> change_field_branch_fail;
    prj::vector_pair<int32_t, int32_t> change_field_branch_success;
    int64_t change_field_branch_time;
    pv_scene_fade scene_fade;
    bool has_signature;
    bool has_perf_id;
    int32_t targets_remaining;
    std::vector<pv_dsc_target_group> targets;
    size_t target_index;
    float_t scene_rot_y;
    mat4 scene_rot_mat;
    float_t field_2C54C;
    int32_t field_2C550;
    int32_t branch_mode;
    int32_t first_challenge_note;
    int32_t last_challenge_note;
    struc_676 field_2C560;

    pv_game_pv_data();
    ~pv_game_pv_data();

    int64_t ctrl(float_t delta_time, int64_t curr_time, bool fade_out);
    //void dsc_buffer_counter_set();
    //void dsc_buffer_set(const void* data, uint32_t size);
    bool dsc_ctrl(float_t delta_time, int64_t curr_time,
        float_t* dsc_time_offset, bool* music_play, bool fade_out, bool ignore_targets);
    void dsc_reset_position();
    //void dsc_get_start_position(int32_t* value);
    dsc_data* find_func(int32_t func_name, int32_t* time,
        int32_t* pv_branch_mode, dsc_data* start, dsc_data* end);
    void find_change_fields();
    void find_data_camera();
    void find_playdata_item_anim(int32_t chara_id);
    void find_playdata_set_motion(int32_t chara_id);
    void find_set_motion(const struct pv_db_pv_difficulty* diff);
    const std::vector<pv_data_set_motion>* get_set_motion(size_t chara_id);
    pv_dsc_target_group* get_target();
    int32_t get_target_target_count(size_t index, float_t time_offset, bool a4);
    void init(::pv_game* pv_game, bool music_play);
    bool load(std::string&& file_path, struct pv_game* pv_game, bool music_play);
    void reset();
    void reset_camera_post_process();
    void scene_fade_ctrl(float_t delta_time);
    void set_camera_max_frame(int64_t time);
    void set_item_anim_max_frame(int32_t chara_id, int32_t index, int64_t time);
    void set_motion_max_frame(int32_t chara_id, int32_t motion_index, int64_t time);
};
