/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../CRE/auth_3d.hpp"
#include "../../CRE/module_table.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/task.hpp"
#include "../config.hpp"
#include "../task_window.hpp"
#include "pv_game_pv_data.hpp"
#include "pv_game_play_data.hpp"

enum hit_state {
    HIT_COOL = 0,
    HIT_FINE,
    HIT_SAFE,
    HIT_BAD,
    HIT_WRONG_COOL,
    HIT_WRONG_FINE,
    HIT_WRONG_SAFE,
    HIT_WRONG_BAD,
    HIT_WORST,
    HIT_COOL_DOUBLE,
    HIT_FINE_DOUBLE,
    HIT_SAFE_DOUBLE,
    HIT_BAD_DOUBLE,
    HIT_COOL_TRIPLE,
    HIT_FINE_TRIPLE,
    HIT_SAFE_TRIPLE,
    HIT_BAD_TRIPLE,
    HIT_COOL_QUADRIPLE,
    HIT_FINE_QUADRIPLE,
    HIT_SAFE_QUADRIPLE,
    HIT_BAD_QUADRIPLE,
    HIT_MAX,
};

struct pv_game_chara {
    chara_index chara_index;
    int32_t cos;
    int32_t chara_id;
    rob_chara_pv_data pv_data;
    rob_chara* rob_chr;
    std::vector<uint32_t> motion_ids;
    std::vector<uint32_t> motion_face_ids;

    pv_game_chara();
    ~pv_game_chara();

    bool check_chara();
    void reset();
};

struct pv_game_edit_effect_data {
    int32_t index;
    float_t speed;
    float_t time;
    float_t end_frame;
    float_t delta_time;
    bool loop;

    pv_game_edit_effect_data();

    void reset();
};

struct pv_game_edit_effect {
    std::vector<uint32_t> spr_set_ids;
    std::vector<uint32_t> aet_set_ids;
    std::vector<uint32_t> aet_ids;
    pv_game_edit_effect_data data;

    pv_game_edit_effect();
    ~pv_game_edit_effect();

    void reset();
    void unload();
};

struct pv_game_field {
    int32_t stage_index;
    std::vector<int32_t> auth_3d_uids;
    std::vector<auth_3d_id> auth_3d_ids;
    std::map<int32_t, int32_t> auth_3d_frame_list;
    int32_t light_auth_3d_uid;
    auth_3d_id light_auth_3d_id;
    int32_t light_frame;
    std::vector<std::string> aet_names[4];
    std::vector<std::string> aet_ids[4];
    std::map<std::pair<std::string, std::string>, int32_t> aet[4];
    uint32_t spr_set_back_id;
    bool stage_flag;
    int32_t npr_type;
    int32_t cam_blur;
    bool sdw_off;
    std::vector<uint64_t> play_eff_list;
    std::vector<uint64_t> stop_eff_list;
    std::vector<uint64_t> field_1A0;

    pv_game_field();
    ~pv_game_field();
};

struct struc_269 {
    prj::vector_pair<object_info, int32_t> data;
    std::vector<auth_3d_id> auth_3d_ids;
    int32_t field_30;
    int32_t field_34;

    struc_269();
    ~struc_269();
};

struct pv_game_chreff {
    chara_index src_chara_index;
    chara_index dst_chara_index;
    std::bitset<128> type;
    std::string src_auth_3d;
    std::string dst_auth_3d;
    std::string src_auth_3d_category;
    std::string dst_auth_3d_category;
    int32_t src_auth_3d_uid;
    int32_t dst_auth_3d_uid;

    pv_game_chreff();
    ~pv_game_chreff();
};

struct pv_effect_resource {
    std::string name;
    float_t emission;

    pv_effect_resource();
    pv_effect_resource(const std::string& name, float_t emission);
    ~pv_effect_resource();
};

struct pv_game_data {
    pv_game_pv_data pv_data;
    pv_game_play_data play_data;
    int32_t field_2CE98;
    int8_t field_2CE9C[128];
    int32_t field_2CF1C;
    bool field_2CF20;
    int32_t appear_state;
    float_t appear_time;
    float_t appear_duration;
    int32_t field_2CF30;
    int32_t field_2CF34;
    int32_t field_2CF38;
    int32_t life_gauge;
    int32_t score_final;
    int32_t challenge_time_total_bonus;
    int32_t combo;
    int32_t challenge_time_combo_count;
    int32_t max_combo;
    int32_t total_hit_count[5];
    int32_t hit_count[5];
    int32_t current_reference_score;
    int32_t target_count;
    int32_t field_2CF84;
    int32_t field_2CF88;
    float_t field_2CF8C;
    int32_t score_slide_chain_bonus;
    int32_t slide_chain_length;
    int32_t field_2CF98;
    int32_t field_2CF9C;
    int16_t field_2CFA0;
    int32_t field_2CFA4;
    const pv_db_pv* pv;
    int32_t field_2CFB0;
    int32_t chance_point;
    int32_t reference_score;
    int32_t reference_score_no_flag;
    int32_t reference_score_no_flag_life_bonus;
    std::vector<int32_t> target_reference_score;
    int32_t field_2CFE0;
    int32_t field_2CFE4;
    size_t notes_passed;
    int32_t field_2CFF0;
    float_t song_energy;
    float_t song_energy_base;
    float_t song_energy_border;
    int32_t life_gauge_safety_time;
    int32_t life_gauge_border;
    int32_t stage_index;
    bool music_play;
    bool no_fail;
    bool disp_lyrics;
    bool changed_field;
    int64_t challenge_time_start;
    int64_t challenge_time_end;
    int64_t max_time;
    float_t max_time_float;
    float_t current_time_float;
    int64_t current_time;
    float_t field_2D038;
    float_t field_2D03C;
    int32_t score_hold_multi;
    uint32_t score_hold;
    int32_t score_slide;
    bool has_slide;
    bool has_success_se;
    pv_disp2d* pv_disp2d;
    int32_t life_gauge_final;
    bool hit_border;
    bool field_2D05D;
    bool field_2D05E;
    bool start_fade;
    bool title_image_init;
    bool mute;
    bool ex_stage;
    bool play_success;
    bool has_auth_3d_frame;
    bool has_light_frame;
    bool has_aet_frame[4];
    bool has_aet_list[4];
    uint32_t field_index;
    int32_t edit_effect_index;
    int32_t slidertouch_counter;
    int32_t change_field_branch_success_counter;
    int32_t field_2D080;
    int32_t field_2D084;
    int32_t life_gauge_bonus;
    int32_t life_gauge_total_bonus;
    bool field_2D090;
    bool no_clear;
    bool disp_lyrics_now;
    bool field_2D093;
    bool field_2D094;
    bool field_2D095;
    bool field_2D096;
    bool success;
    int32_t title_image_state;
    int32_t field_2D09C;
    bool use_osage_play_data;
    bool pv_end_fadeout;
    float_t rival_percentage;
    int32_t field_2D0A8;
    int32_t field_2D0AC;
    int32_t field_2D0B0[3];
    bool field_2D0BC;
    bool next_stage;
    bool has_frame_texture;
    int8_t field_2D0BF;
    int32_t movie_index;
    int32_t field_2D0C4;
    pv_game_chara chara[ROB_CHARA_COUNT];
    std::vector<uint32_t> motion_set_ids;
    std::vector<uint32_t> motion_set_face_ids;
    std::vector<uint32_t> obj_set_itmpv;
    std::vector<uint32_t> obj_set_handitem;
    std::vector<uint32_t> chreff_auth_3d_obj_set_ids;
    std::vector<uint32_t> field_2D770;
    std::vector<uint32_t> stgpvhrc_obj_set_ids;
    std::vector<uint32_t> ex_song_ex_auth_obj_set_ids;
    std::vector<int32_t> stage_indices;
    std::vector<task_stage_info> stage_infos;
    bool field_2D7E8;
    int32_t field_2D7EC;
    std::vector<uint32_t> spr_set_back_ids;
    int32_t field_2D808;
    int32_t field_2D80C;
    pv_game_edit_effect edit_effect;
    uint32_t camera_auth_3d_uid;
    int32_t se_index;
    bool task_effect_init;
    int32_t field_2D87C;
    size_t current_field;
    std::vector<pv_game_field> field_data;
    bool enable_movie;
    int32_t field_2D8A4;
    std::map<int32_t, int64_t> auth_3d_time;
    std::map<int32_t, int64_t> light_time;
    std::map<std::pair<std::string, std::string>, int64_t> aet_time[4];
    std::string campv_string;
    std::vector<int32_t> campv_auth_3d_uids;
    std::map<int32_t, auth_3d_id> campv;
    int32_t campv_index;
    int32_t field_2D954;
    std::string itmpv_string;
    std::vector<int32_t> itmpv_auth_3d_uids;
    std::map<int32_t, std::pair<auth_3d_id, int64_t>> itmpv[ROB_CHARA_COUNT];
    std::vector<std::string> itmpv_uids;
    struc_269 field_2DA08[3];
    std::vector<int32_t> loaded_auth_3d_uids;
    bool field_2DAC8;
    int32_t field_2DACC;
    std::vector<std::string> auth_3d_categories;
    std::map<int32_t, auth_3d_id> auth_3d;
    std::vector<pv_game_chreff> chreff_auth_3d;
    std::vector<pv_game_chreff> chreff_auth_3d_obj;
    bool height_adjust;
    vec2 field_2DB2C;
    int32_t field_2DB34;
    time_struct field_2DB38;
    std::vector<uint64_t> effect_rs_list_hashes;
    std::vector<pv_effect_resource> effect_rs_list;

    pv_game_data();
    ~pv_game_data();

    void calculate_target_start_pos(vec2& pos, float_t angle, float_t distance, vec2& start_pos);
    int64_t get_max_time(int64_t* challenge_time_start, int64_t* challenge_time_end);
    void reset();

    bool sub_140119960(bool* has_target, int64_t* dsc_time, int32_t* target_count,
        int64_t* target_flying_time, float_t fade_time, bool* fade, dsc_target_ft* target_hist,
        int32_t* target_flag, bool* has_chance, bool* has_slide, int32_t* slide_chain_type,
        bool* slide_chain_start, int32_t* slide_chain_start_count, int32_t* slide_chain_cont_count,
        int32_t* slide_chain_no_end_count, bool* has_slide_chain, int64_t target_flying_time_hist);

    inline float_t get_anim_offset() {
        return pv->is_old_pv ? 1.0f : 0.0f;
    }
};

union pv_game_item {
    struct {
        int32_t head;
        int32_t face;
        int32_t chest;
        int32_t back;
    };
    int32_t arr[4];

    pv_game_item();
};

struct pv_game_item_mask {
    bool arr[4];

    pv_game_item_mask();
};

struct pv_game {
    bool loaded;
    bool field_1;
    bool field_2;
    bool end_pv;
    bool field_4;
    int32_t state;
    int32_t field_C;
    int32_t pv_id;
    bool field_14;
    int32_t modules[ROB_CHARA_COUNT];
    rob_chara_pv_data_item items[ROB_CHARA_COUNT];
    pv_game_item_mask items_mask[ROB_CHARA_COUNT];
    pv_game_data data;

    pv_game();
    ~pv_game();

    void calculate_life_gauge(hit_state hit_state, bool has_safety,
        bool challenge_time, bool slide, bool slide_chain, bool slide_chain_start, bool a8);
    int32_t calculate_life_gauge_border();
    int32_t calculate_reference_score();
    void calculate_score(int32_t score, bool continue_combo, int64_t& time,
        vec2& pos, int32_t multi_count, hit_state hit_state, bool challenge_time, bool slide,
        bool slide_chain, bool slide_chain_start, bool slide_chain_max, bool slide_chain_continues);
    void calculate_song_energy();
    void calculate_song_energy_border();
    void calculate_target_reference_score(int32_t* reference_score,
        int32_t* target_index, int32_t multi_count, bool slide_chain);
    void calculate_target_reference_combo_bonus(int32_t* reference_score, int32_t* target_index);
    void change_field(size_t field, ssize_t dsc_time, ssize_t curr_time);
    void check_auth_replace_by_module(const char* name, int32_t& uid);
    int32_t check_chrcam(const char* name, int32_t& uid);
    bool check_life_gauge_safety();
    int32_t ctrl(float_t delta_time, int64_t curr_time);
    void data_itmpv_disable();
    void disp();
    void disp_song_name();
    void edit_effect_ctrl(float_t delta_time);
    void edit_effect_set(int32_t index, float_t frame_speed, float_t delta_time);
    void end(bool a2, bool set_fade);
    float_t get_aet_frame_max_frame(int32_t aet_frame, int32_t aet_index, std::pair<std::string,
        std::string>& aet_name_id, int64_t dsc_time, int64_t curr_time, float_t* max_frame);
    void get_aet_frame_max_frame_by_name(float_t& frame,
        float_t& max_frame, std::pair<std::string, std::string>& aet_name_id);
    float_t get_aet_frame_max_frame_change_field(int32_t aet_index,
        std::pair<std::string, std::string>& aet_name_id, int64_t time);
    int64_t get_aet_time(int32_t aet_index, std::pair<std::string, std::string>& aet_name_id);
    float_t get_auth_3d_frame_max_frame(int32_t auth_3d_frame, int32_t auth_3d_uid,
        int64_t dsc_time, int64_t curr_time, uint8_t type, float_t* max_frame);
    float_t get_auth_3d_frame_max_frame_change_field(uint32_t auth_3d_uid, int64_t time, uint8_t type);
    auth_3d_id get_auth_3d_id(int32_t uid);
    int64_t get_auth_3d_time(int32_t uid, uint8_t type);
    uint32_t get_chreff_auth_3d_object_set(int32_t& uid);
    int64_t get_data_itmpv_time(int32_t chara_id, int32_t index);
    float_t get_data_rival_percentage();
    std::string get_effect_se_file_name();
    int32_t get_field_aet_frame_by_name_id(pv_game_field& field,
        int32_t aet_index, std::pair<std::string, std::string>& aet_name_id);
    int32_t get_field_aet_index_by_name_id(pv_game_field& field,
        int32_t aet_index, std::pair<std::string, std::string>& aet_name_id);
    int32_t get_field_auth_3d_frame(pv_game_field& field_data, int32_t uid, int32_t type);
    int32_t get_frame(int32_t frame, int64_t dsc_time, int64_t curr_time);
    int32_t get_frame_continue(int32_t frame);
    float_t get_life_gauge_safety_time();
    float_t get_next_aet_max_frame_change_field(int32_t aet_index,
        std::pair<std::string, std::string>& aet_name_id, int64_t dsc_time, int64_t curr_time);
    float_t get_next_aet_max_time_max_frame_change_field(int32_t aet_index,
        std::pair<std::string, std::string>& aet_name_id, int64_t dsc_time, int64_t* max_time,
        int64_t curr_time, prj::vector_pair<int32_t, int32_t>& change_field);
    float_t get_next_auth_3d_max_frame_change_field(uint32_t auth_3d_uid,
        int64_t dsc_time, int64_t curr_time, int32_t type);
    float_t get_next_auth_3d_max_time_max_frame_change_field(uint32_t auth_3d_uid, int64_t dsc_time,
        int64_t* max_time, int64_t curr_time, prj::vector_pair<int32_t, int32_t>& change_field, int32_t type);
    int32_t get_play_data_ex_song_index();
    std::string get_play_data_song_file_name();
    const pv_db_pv* get_pv_db_pv();
    float_t get_pv_hidden_timing();
    int32_t get_pv_high_speed_rate();
    std::string get_pv_movie_file_name(int32_t index);
    float_t get_pv_sudden_timing();
    std::string get_song_name();
    std::string get_song_file_name(int32_t& ex_song_index);
    task_stage_info get_stage_info(int32_t stage_index);
    bool is_play_data_option_3();
    void itmpv_reset();
    bool load();
    void play_se(int32_t index);
    void play_data_ctrl(float_t delta_time);
    void reset();
    void reset_appear();
    void reset_field();
    void set_chara_use_opd(bool value);
    void set_data_campv(int32_t type, int32_t index, float_t frame);
    void set_data_itmpv(int32_t chara_id, int32_t index, bool enable, int64_t time);
    void set_data_itmpv_chara_id(int32_t chara_id, int32_t index, bool attach);
    void set_data_itmpv_req_frame(int32_t chara_id, int32_t index, float_t value);
    void set_data_itmpv_max_frame(int32_t chara_id, float_t value);
    void set_data_itmpv_max_frame(int32_t chara_id, int32_t index, float_t value);
    void set_data_itmpv_visibility(int32_t chara_id, bool value);
    void set_data_itmpv_visibility(int32_t chara_id, int32_t index, bool value);
    void set_eyes_adjust(pv_game_chara* chr);
    void set_item(size_t performer, rob_chara_pv_data_item& value);
    void set_item_mask(size_t performer, size_t item, bool value);
    void set_lyric(int32_t lyric_index, color4u8 lyric_color);
    void set_module(size_t performer, int32_t module);
    void set_osage_init(const pv_game_chara& chr);
    bool set_pv_param_post_process_bloom_data(bool set, int32_t id, float_t duration);
    void set_pv_param_post_process_chara_alpha_data(int32_t chara_id,
        float_t alpha, int32_t type, float_t duration);
    void set_pv_param_post_process_chara_item_alpha_data(int32_t chara_id,
        float_t alpha, int32_t type, float_t duration);
    bool set_pv_param_post_process_color_correction_data(bool set, int32_t id, float_t duration);
    bool set_pv_param_post_process_dof_data(bool set, int32_t id, float_t duration);
    void title_image_ctrl(bool dont_wait);
    bool unload();

    void sub_1400FC500();
    void sub_1400FC6F0();
    int32_t sub_1400FC780(float_t delta_time);
    float_t sub_1400FCEB0();
    int32_t sub_1400FDD80();
    void sub_140104FB0();
    void sub_140106640();
    bool sub_14010EF00();
    void sub_14010F030();
    int32_t sub_14010F930();
    int32_t sub_140112C00(int32_t index);
    void sub_140113730();
    void sub_140115C10(int32_t chara_id, bool value);
    void sub_140115C90(int32_t chara_id, bool disp, int32_t index, float_t frame, float_t frame_speed);
    void sub_1401230A0();

    static void chara_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha);
    static void get_item_mask(pv_performer_type type,
        rob_chara_pv_data_item* src_item, pv_game_item_mask* src_mask,
        rob_chara_pv_data_item* dst_item, pv_game_item_mask* dst_mask);
};

struct pv_game_init_data {
    int32_t pv_id;
    pv_difficulty difficulty;
    pv_edition edition;
    int32_t score_percentage_clear;
    int32_t life_gauge_safety_time;
    int32_t life_gauge_border;
    int32_t stage_index;
    int32_t modules[ROB_CHARA_COUNT];
    rob_chara_pv_data_item items[ROB_CHARA_COUNT];
    pv_game_item_mask items_mask[ROB_CHARA_COUNT];

    pv_game_init_data();

    void reset();
};

struct struc_14 {
    int32_t type;
    pv_difficulty difficulty;
    pv_edition edition;
    int32_t pv_id;
    int8_t field_10;
    int8_t field_11;
    int8_t field_12;
    int8_t field_13;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    float_t field_20;
    bool field_24;
    bool field_25;
    bool field_26;

    struc_14();

    int32_t sub_1400E7910();
    bool sub_1400E7920();
    void sub_1400E79E0(int32_t value);
};

struct struc_677 {
    int32_t field_0;
    int32_t field_4;
    int32_t stage_index;
    int8_t field_C;
    int32_t field_10;
    bool no_fail;
    bool watch;
    bool success;
    bool field_17;
    int32_t field_18;
    int32_t field_1C;
    bool field_20;
    int32_t option;
};

struct struc_775 {
    int32_t pv_id;
    int32_t field_4;
    pv_difficulty difficulty;
    int32_t edition;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
    int8_t field_1C;
    int32_t score_percentage_clear;
    int32_t life_gauge_safety_time;
    int32_t life_gauge_border;
    int32_t field_2C;
    int8_t field_30;
    int32_t field_34;
    int32_t field_38;
    int8_t field_3C;
    bool ex_stage;
    int32_t another_song_index;
    int32_t field_44;
    int8_t field_48;
    int8_t field_49;
    int32_t field_4C[ROB_CHARA_COUNT];
    int32_t field_64[ROB_CHARA_COUNT];
    pv_game_item field_7C[ROB_CHARA_COUNT];
    pv_game_item_mask field_DC[ROB_CHARA_COUNT];
    int32_t field_F4;
    int32_t field_F8;

    struc_775();
};

struct struc_663 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
};

struct struc_664 {
    int8_t field_0;
    int8_t field_1;
    int8_t field_2;
    int8_t field_3;
};

struct struc_660 {
    int32_t field_0;
    int32_t field_4;
    std::vector<int32_t> field_8;
    int32_t field_20;
    int32_t field_24;
    std::string field_28;
    int32_t field_48;
    int32_t field_4C;

    struc_660();
    ~struc_660();
};

struct struc_661 {
    struc_660 field_0[2];

    struc_661();
    ~struc_661();
};

struct struc_662 {
    int32_t field_0;
    int32_t total_hit_count[5];
    int32_t hit_count[5];
    int32_t field_2C;
    int32_t field_30[5];
    int32_t field_44[5];
    int32_t max_combo;
    int32_t challenge_time_total_bonus;
    int32_t score_hold_multi;
    int32_t score_final;
    int32_t score_percentage;
    int32_t score_slide;
    bool field_70;
    int32_t has_slide;
    int32_t pv_game_state;
    int32_t field_7C;
    int32_t field_80;
    int32_t field_84;
    int32_t field_88;
    int32_t field_8C;
    int32_t field_90;
    int32_t field_94;
    int32_t field_98;
    int32_t field_9C;
    int32_t field_A0;
    int32_t field_A4;
    int32_t field_A8;
    int32_t field_AC[6];
    struc_663 field_C4[6];
    struc_664 field_124[6];
    int32_t field_13C;
    struc_661 field_140;
    int32_t field_1E0;
    int32_t field_1E4[3];
    bool next_stage;

    struc_662();
    ~struc_662();
};

struct struc_777 {
    int8_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
    std::string field_10;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    std::string field_40;
    int32_t field_60;
    int32_t field_64;

    struc_777();
    ~struc_777();
};

struct struc_778 {
    int32_t field_0;
    int32_t field_4;
    std::string field_8[2];

    struc_778();
    ~struc_778();
};

struct struc_780 {
    int32_t field_0;
    int32_t field_4;
    int8_t field_8;
};

struct struc_779 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    struc_780 field_C[5];
};

struct struc_716 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
    int8_t field_10;
    int8_t field_11;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
    int32_t field_28;
    struc_775 field_2C;
    struc_662 field_128;
    struc_777 field_320;
    struc_778 field_388;
    struc_778 field_3D0[3];
    struc_779 field_4A8[5];
    int64_t field_610;
    int64_t field_618;
    int64_t field_620;
    int64_t field_628;
    int64_t field_630;
    int64_t field_638;
    int64_t field_640;
    int8_t field_648;
    int8_t field_649;
    int64_t field_650;
    int64_t field_658;
    int64_t field_660;

    struc_716();
    ~struc_716();
};

struct struc_718 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
    int64_t field_20;
    int64_t field_28;
    int64_t field_30;
    int64_t field_38;
    int64_t field_40;
    int64_t field_48;
    int64_t field_50;
    int64_t field_58;
    int64_t field_60;
    int64_t field_68;
    int64_t field_70;
    int64_t field_78;
    int64_t field_80;
    int64_t field_88;
    int64_t field_90;
    int64_t field_98;
    int64_t field_A0;
    int64_t field_A8;
    int64_t field_B0;
    int64_t field_B8;
    int64_t field_C0;
    int64_t field_C8;
    int64_t field_D0;
    int64_t field_D8;
    int64_t field_E0;
    int64_t field_E8;
    int64_t field_F0;
    int64_t field_F8;
    int64_t field_100;
    int64_t field_108;
    int64_t field_110;
    int64_t field_118;
    int64_t field_120;
    int64_t field_128;
    int64_t field_130;
    int64_t field_138;
    int64_t field_140;
};

struct struc_717 {
    struc_677 field_0;
    struc_716 field_28[4];
    struc_718 field_19C8;

    struc_717();
    ~struc_717();
};

#if PV_DEBUG
class TaskPvGame : public app::TaskWindow {
#else
class TaskPvGame : public app::Task {
#endif
public:
    struct Args {
        pv_game_init_data init_data;
        std::string se_name;
        std::string slide_se_name;
        std::string chainslide_first_name;
        std::string chainslide_sub_name;
        std::string chainslide_success_name;
        std::string chainslide_failure_name;
        std::string slidertouch_name;
        bool field_190;
        bool watch;
        bool no_fail;
        bool disp_lyrics;
        bool field_194;
        bool mute;
        bool ex_stage;
        bool success;
        bool test_pv;
        int32_t option;

        Args();
        ~Args();

        void Clear();
        void Reset();
    };

    struct Data {
        int32_t type;
        pv_game_init_data init_data;
        std::string se_name;
        std::string slide_se_name;
        std::string chainslide_first_name;
        std::string chainslide_sub_name;
        std::string chainslide_success_name;
        std::string chainslide_failure_name;
        std::string slidertouch_name;
        bool field_190;
        bool music_play;
        bool no_fail;
        bool disp_lyrics;
        bool mute;
        bool ex_stage;
        bool success;
        int32_t option;

        Data();
        ~Data();

        void Clear();
        void Reset();
    } data;

#if PV_DEBUG
    bool pause;
    bool step_frame;
    bool is_paused;
#endif

    TaskPvGame();
    virtual ~TaskPvGame() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
#if PV_DEBUG
    virtual void basic() override;
    virtual void window() override;
#endif

    void load(TaskPvGame::Data& data);
    bool unload();
};

#if PV_DEBUG
class PVGameSelector : public app::TaskWindow {
public:
    int32_t pv_id;
    const pv_db_pv* pv;
    pv_difficulty difficulty;
    pv_edition edition;
    bool success;
    chara_index charas[ROB_CHARA_COUNT];
    int32_t modules[ROB_CHARA_COUNT];
    std::string module_names[ROB_CHARA_COUNT];
    bool start;
    bool exit;

    PVGameSelector();
    virtual ~PVGameSelector() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;

    void reset_chara();
};
#endif

extern float_t bar_time_set_to_target_flying_time(int32_t bpm, int32_t time_signature, int64_t* time_int = 0);
extern float_t dsc_time_to_frame(int64_t time);

extern bool pv_game_init();
extern pv_game* pv_game_get();
extern bool pv_game_free();

extern bool task_pv_game_add_task(TaskPvGame::Args& args);
extern bool task_pv_game_check_task_ready();
extern bool task_pv_game_del_task();

extern void task_pv_game_init_pv();
extern bool task_pv_game_init_demo_pv(int32_t pv_id, pv_difficulty difficulty, bool music_play);
extern void task_pv_game_init_test_pv();

#if PV_DEBUG
extern bool pv_game_selector_init();
extern PVGameSelector* pv_game_selector_get();
extern bool pv_game_selector_free();
#endif

extern struc_14* sub_14013C8C0();
extern struc_717* sub_14038BB30();
