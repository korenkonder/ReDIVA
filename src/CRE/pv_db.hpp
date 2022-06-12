/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <bitset>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/key_val.hpp"
#include "rob.hpp"
#include "task.hpp"

enum pv_attribute_type {
    PV_ATTRIBUTE_ORIGINAL = 0x00,
    PV_ATTRIBUTE_EXTRA    = 0x01,
    PV_ATTRIBUTE_END      = 0x02,
};

enum pv_chreff_data_type {
    PV_CHREFF_DATA_AUTH3D     = 0x00,
    PV_CHREFF_DATA_AUTH3D_OBJ = 0x01,
};

enum pv_movie_surface {
    PV_MOVIE_SURFACE_BACK  = 0x00,
    PV_MOVIE_SURFACE_FRONT = 0x01,
};

enum pv_difficulty {
    PV_DIFFICULTY_EASY    = 0x00,
    PV_DIFFICULTY_NORMAL  = 0x01,
    PV_DIFFICULTY_HARD    = 0x02,
    PV_DIFFICULTY_EXTREME = 0x03,
    PV_DIFFICULTY_ENCORE  = 0x04,
    PV_DIFFICULTY_MAX     = 0x05,
};

enum pv_frame_texture_type {
    PV_FRAME_TEXTURE_PRE_PP  = 0x00,
    PV_FRAME_TEXTURE_POST_PP = 0x01,
    PV_FRAME_TEXTURE_FB      = 0x02,
};

enum pv_level {
    PV_LV_00_0 = 0x00,
    PV_LV_00_5 = 0x01,
    PV_LV_01_0 = 0x02,
    PV_LV_01_5 = 0x03,
    PV_LV_02_0 = 0x04,
    PV_LV_02_5 = 0x05,
    PV_LV_03_0 = 0x06,
    PV_LV_03_5 = 0x07,
    PV_LV_04_0 = 0x08,
    PV_LV_04_5 = 0x09,
    PV_LV_05_0 = 0x0A,
    PV_LV_05_5 = 0x0B,
    PV_LV_06_0 = 0x0C,
    PV_LV_06_5 = 0x0D,
    PV_LV_07_0 = 0x0E,
    PV_LV_07_5 = 0x0F,
    PV_LV_08_0 = 0x10,
    PV_LV_08_5 = 0x11,
    PV_LV_09_0 = 0x12,
    PV_LV_09_5 = 0x13,
    PV_LV_10_0 = 0x14,
    PV_LV_MAX  = 0x15,
};

enum pv_performer_size {
    PV_PERFORMER_NORMAL     = 0x00,
    PV_PERFORMER_PLAY_CHARA = 0x01,
    PV_PERFORMER_PV_CHARA   = 0x02,
    PV_PERFORMER_SHORT      = 0x03,
    PV_PERFORMER_TALL       = 0x04,
};

enum pv_performer_type {
    PV_PERFORMER_VOCAL          = 0x00,
    PV_PERFORMER_PSEUDO_DEFAULT = 0x01,
    PV_PERFORMER_PSEUDO_SAME    = 0x02,
    PV_PERFORMER_PSEUDO_SWIM    = 0x03,
    PV_PERFORMER_PSEUDO_SWIM_S  = 0x04,
    PV_PERFORMER_PSEUDO_MYCHARA = 0x05,
    PV_PERFORMER_GUEST          = 0x06,
};

struct pv_db_pv_lyric {
    int32_t index;
    std::string data;

    pv_db_pv_lyric();
    ~pv_db_pv_lyric();
};

struct pv_db_pv_sabi {
    float_t start_time;
    float_t play_time;
};

struct pv_db_pv_performer {
    pv_performer_type type;
    int32_t chara;
    int32_t costume;
    int32_t pv_costume[5];
    bool fixed;
    int32_t exclude;
    pv_performer_size size;
    int32_t pseudo_same_id;
    int32_t item[4];

    pv_db_pv_performer();

    void set_pv_costume(int32_t pv_costume, int32_t difficulty);
};

struct pv_db_pv_attribute {
    pv_attribute_type type;
    std::bitset<128> bitset;

    pv_db_pv_attribute();
    ~pv_db_pv_attribute();

    void reset();
};

struct pv_db_pv_motion {
    int32_t index;
    int32_t id;
    std::string name;

    pv_db_pv_motion();
    ~pv_db_pv_motion();
};

struct pv_db_pv_field {
    int32_t index;
    int32_t stage_index;
    int32_t ex_stage_index;
    std::vector<std::string> auth_3d_list;
    std::vector<std::string> ex_auth_3d_list;
    std::vector<int32_t> auth_3d_frame_list;
    std::string light;
    int32_t light_frame;
    bool light_frame_set;
    std::vector<std::string> aet[4];
    std::vector<std::string> aet_id[4];
    std::vector<int32_t> aet_frame[4];
    std::string spr_set_back;
    bool stage_flag;
    int32_t npr_type;
    int32_t cam_blur;
    bool sdw_off;
    std::vector<std::string> play_eff_list;
    std::vector<std::string> stop_eff_list;
    std::vector<std::string> effect_rs_list;
    std::vector<float_t> effect_emision_rs_list;

    pv_db_pv_field();
    ~pv_db_pv_field();

    void reset();
};

struct pv_db_pv_field_parent {
    std::vector<pv_db_pv_field> data;
    bool ex_stage_set;

    pv_db_pv_field_parent();
    ~pv_db_pv_field_parent();

    void reset();
};

struct pv_db_pv_item {
    int32_t index;
    std::string name;

    pv_db_pv_item();
    ~pv_db_pv_item();
};

struct pv_db_pv_hand_item {
    int32_t index;
    int32_t id;
    std::string name;

    pv_db_pv_hand_item();
    ~pv_db_pv_hand_item();
};

struct struc_426 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
};

struct pv_db_pv_edit_effect {
    int32_t index;
    std::string name;
    bool low_field;

    pv_db_pv_edit_effect();
    ~pv_db_pv_edit_effect();

    void reset();
};

struct pv_db_pv_title_image {
    float_t time;
    float_t end_time;
    std::string aet_name;

    pv_db_pv_title_image();
    ~pv_db_pv_title_image();

    void reset();
};

struct pv_db_pv_songinfo {
    std::string music;
    std::string lyrics;
    std::string arranger;
    std::string manipulator;
    std::string pv_editor;
    std::string guitar_player;
    std::pair<std::string, std::string> ex_info[4];

    pv_db_pv_songinfo();
    ~pv_db_pv_songinfo();

    void reset();
};

struct pv_db_pv_movie {
    int32_t index;
    std::string name;

    pv_db_pv_movie();
    ~pv_db_pv_movie();
};

struct pv_db_pv_difficulty {
    pv_difficulty difficulty;
    int32_t edition;
    pv_db_pv_attribute attribute;
    std::string script_file_name;
    pv_level level;
    int32_t level_sort_index;
    std::string se_name;
    std::string pvbranch_success_se_name;
    std::string slide_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    std::string chainslide_failure_name;
    std::string slidertouch_name;
    std::vector<pv_db_pv_motion> motion[ROB_CHARA_COUNT];
    pv_db_pv_field_parent field;
    std::vector<pv_db_pv_item> pv_item;
    std::vector<pv_db_pv_hand_item> hand_item;
    std::vector<struc_426> field_228;
    std::vector<void*> field_240;
    std::vector<void*> field_258;
    std::vector<pv_db_pv_edit_effect> edit_effect;
    pv_db_pv_title_image title_image;
    pv_db_pv_songinfo songinfo;
    std::vector<pv_db_pv_movie> movie_list;
    pv_movie_surface movie_surface;
    std::string effect_se_file_name;
    std::vector<std::string> effect_se_name_list;
    int32_t version;
    int32_t script_format;
    int32_t high_speed_rate;
    float_t hidden_timing;
    float_t sudden_timing;
    bool edit_chara_scale;

    pv_db_pv_difficulty();
    ~pv_db_pv_difficulty();

    void reset();
};

struct pv_db_pv_ex_song_ex_auth {
    std::string org_name;
    std::string name;

    pv_db_pv_ex_song_ex_auth();
    ~pv_db_pv_ex_song_ex_auth();
};

struct pv_db_pv_ex_song {
    int32_t chara[ROB_CHARA_COUNT];
    std::string file;
    std::string name;
    std::vector<pv_db_pv_ex_song_ex_auth> ex_auth;

    pv_db_pv_ex_song();
    ~pv_db_pv_ex_song();

    void reset();
};

struct pv_db_pv_ex_song_parent {
    int32_t chara_count;
    std::vector<pv_db_pv_ex_song> data;

    pv_db_pv_ex_song_parent();
    ~pv_db_pv_ex_song_parent();

    void reset();
};

struct pv_db_pv_mdata {
    bool flag;
    std::string dir;

    pv_db_pv_mdata();
    ~pv_db_pv_mdata();

    void reset();
};

struct pv_db_pv_osage_init {
    std::string motion;
    int32_t frame;
    int32_t stage;

    pv_db_pv_osage_init();
    ~pv_db_pv_osage_init();
};

struct pv_db_pv_stage_param {
    int32_t stage;
    int32_t mhd_id;
    std::string collision_file;
    std::string wind_file;

    pv_db_pv_stage_param();
    ~pv_db_pv_stage_param();
};

struct pv_db_pv_disp2d {
    std::string set_name;
    int32_t target_shadow_type;
    int32_t title_start_2d_field;
    int32_t title_end_2d_field;
    int32_t title_start_2d_low_field;
    int32_t title_end_2d_low_field;
    int32_t title_start_3d_field;
    int32_t title_end_3d_field;
    std::string title_2d_layer;

    pv_db_pv_disp2d();
    ~pv_db_pv_disp2d();

    void reset();
};

struct pv_db_pv_chreff_data {
    pv_chreff_data_type type;
    std::string name;

    pv_db_pv_chreff_data();
    ~pv_db_pv_chreff_data();
};

struct pv_db_pv_chreff {
    int32_t id;
    std::string name;
    std::vector<pv_db_pv_chreff_data> data;

    pv_db_pv_chreff();
    ~pv_db_pv_chreff();
};

struct pv_db_pv_chrcam {
    int32_t id;
    std::string chara;
    std::string org_name;
    std::string name;

    pv_db_pv_chrcam();
    ~pv_db_pv_chrcam();
};

struct pv_db_pv_chrmot {
    int32_t id;
    std::string chara;
    std::string org_name;
    std::string name;

    pv_db_pv_chrmot();
    ~pv_db_pv_chrmot();
};

struct pv_db_pv_eyes_rot_rate {
    float_t xp_rate;
    float_t xn_rate;
};

struct pv_db_pv_another_song {
    std::string name;
    std::string song_file_name;

    pv_db_pv_another_song();
    ~pv_db_pv_another_song();
};

struct pv_db_pv_frame_texture {
    std::string data;
    pv_frame_texture_type type;

    pv_db_pv_frame_texture();
    ~pv_db_pv_frame_texture();

    void reset();
};

struct pv_db_pv_auth_replace_by_module {
    int32_t id;
    int32_t module_id;
    std::string org_name;
    std::string name;

    pv_db_pv_auth_replace_by_module();
    ~pv_db_pv_auth_replace_by_module();
};

struct pv_db_pv {
    int32_t id;
    int32_t date;
    std::string song_name;
    std::string song_name_reading;
    int32_t remix_parent;
    int32_t bpm;
    std::string song_file_name;
    std::vector<pv_db_pv_lyric> lyric;
    pv_db_pv_sabi sabi;
    int32_t edit;
    bool disable_calc_motfrm_limit;
    std::vector<pv_db_pv_performer> performer;
    std::vector<pv_db_pv_difficulty> difficulty[PV_DIFFICULTY_MAX];
    pv_db_pv_ex_song_parent ex_song;
    pv_db_pv_mdata mdata;
    std::vector<pv_db_pv_osage_init> osage_init;
    std::vector<pv_db_pv_stage_param> stage_param;
    pv_db_pv_disp2d disp2d;
    bool use_osage_play_data;
    std::string pv_expression_file_name;
    std::vector<pv_db_pv_chreff> chreff;
    std::vector<pv_db_pv_chrcam> chrcam;
    std::vector<pv_db_pv_chrmot> chrmot;
    bool eyes_xrot_adjust;
    bool is_old_pv;
    eyes_base_adjust_type eyes_base_adjust_type;
    std::map<chara_index, pv_db_pv_eyes_rot_rate> eyes_rot_rate;
    std::vector<pv_db_pv_another_song> another_song;
    bool pre_play_script;
    pv_db_pv_frame_texture frame_texture[6];
    std::vector<pv_db_pv_auth_replace_by_module> auth_replace_by_module;

    pv_db_pv();
    ~pv_db_pv();

    pv_db_pv_difficulty* get_difficulty(int32_t difficulty, pv_attribute_type attribute_type);
    pv_db_pv_difficulty* get_difficulty(int32_t difficulty, int32_t edition);
    void reset();
};

namespace pv_db {
    class TaskPvDB : public app::Task {
    public:
        int32_t state;
        std::list<std::pair<std::string, std::string>> paths;
        p_file_handler file_handler;
        std::list<pv_db_pv> pv_data;
        bool reset;
        bool field_99;
        std::map<std::string, pv_performer_type> performer_types;
        std::map<std::string, pv_performer_size> performer_sizes;
        std::map<std::string, pv_chreff_data_type> chreff_data_types;
        std::map<std::string, pv_movie_surface> movie_surfaces;
        std::map<std::string, pv_level> pv_levels;

        TaskPvDB();
        virtual ~TaskPvDB() override;

        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;

        void InitChrEffDataTypes();
        void InitMovieSurfaces();
        void InitPerformerSizes();
        void InitPerformerTypes();
        void InitPvLevels();

        bool ParsePvDb(pv_db_pv* pv, key_val& kv, int32_t pv_id);
        bool ParsePvField(pv_db_pv* pv, key_val& kv, int32_t pv_id);
    };
}

extern bool task_pv_db_append_task();
extern void task_pv_db_free_task();
extern pv_db::TaskPvDB* task_pv_db_get();
extern uint32_t task_pv_db_get_paths_count();
extern pv_db_pv* task_pv_db_get_pv(int32_t pv_id);
extern pv_db_pv_difficulty* task_pv_db_get_pv_difficulty(int32_t pv_id,
    int32_t difficulty, pv_attribute_type attribute_type);
extern pv_db_pv_difficulty* task_pv_db_get_pv_difficulty(int32_t pv_id,
    int32_t difficulty, int32_t edition);
extern bool task_pv_db_is_paths_empty();
