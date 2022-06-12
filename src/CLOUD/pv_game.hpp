/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(CLOUD_DEV)
#pragma once

#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "../KKdLib/dsc.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/rob.hpp"
#include "../CRE/stage.hpp"
#include "../CRE/task.hpp"
#include "classes.h"

struct pv_play_data_set_motion {
    float_t frame_speed;
    int32_t motion_id;
    float_t frame;
    float_t duration;
    bool field_10;
    MotionBlendType blend_type;
    bool disable_eye_motion;
    int32_t motion_index;
    int64_t dsc_time;
    float_t dsc_frame;
};

struct pv_play_data_motion {
    bool enable;
    int32_t motion_index;
    int64_t time;
};

struct dsc_set_item {
    int32_t time;
    int32_t item_index;
    int32_t pv_branch_mode;
};

struct dsc_set_motion {
    int32_t time;
    int32_t motion_index;
    int32_t pv_branch_mode;
};

struct struc_104 {
    rob_chara* rob_chr;
    float_t current_time;
    float_t duration;
    vec3 start_pos;
    vec3 end_pos;
    float_t start_rot;
    float_t end_rot;
    float_t mot_smooth_len;
    std::vector<dsc_set_motion> set_motion;
    std::vector<dsc_set_item> set_item;

    struc_104();
    ~struc_104();

    void reset();
};

struct pv_play_data {
    std::vector<pv_play_data_motion> motion;
    rob_chara* rob_chr;
    std::list<pv_play_data_set_motion> set_motion;
    bool disp;
    struc_104 field_38;

    pv_play_data();
    ~pv_play_data();

    void reset();
};

struct pv_disp2d {
    int32_t pv_id;
    int32_t title_start_2d_field;
    int32_t title_end_2d_field;
    int32_t title_start_2d_low_field;
    int32_t title_end_2d_low_field;
    int32_t title_start_3d_field;
    int32_t title_end_3d_field;
    bool target_shadow_type;
    int32_t pv_spr_set_id;
    int32_t pv_aet_set_id;
    int32_t pv_main_aet_id;
    std::string title_2d_layer;

    pv_disp2d();
    virtual ~pv_disp2d();
};

struct pv_game_data {
    //struc_490 field_0;
    //struc_155 field_2C838;
    int field_2CE98;
    char field_2CE9C[128];
    int field_2CF1C;
    char field_2CF20;
    int field_2CF24;
    float_t field_2CF28;
    float_t field_2CF2C;
    int field_2CF30;
    int field_2CF34;
    int field_2CF38;
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
    int field_2CF84;
    int field_2CF88;
    float_t field_2CF8C;
    int32_t score_slide_chain_bonus;
    int32_t slide_chain_length;
    int field_2CF98;
    int field_2CF9C;
    __int16 field_2CFA0;
    int field_2CFA4;
    pv_db_pv* pv;
    int field_2CFB0;
    int field_2CFB4;
    int32_t reference_score;
    int field_2CFBC;
    int field_2CFC0;
    int field_2CFC4;
    std::vector<int32_t> field_2CFC8;
    int field_2CFE0;
    int field_2CFE4;
    size_t notes_passed;
    int field_2CFF0;
    float_t score_percentage;
    float_t score_percentage_clear;
    float_t score_percentage_border;
    int32_t life_gauge_safety_time;
    int32_t life_gauge_border;
    int field_2D008;
    char field_2D00C;
    bool no_fail;
    char field_2D00E;
    bool field_2D00F;
    int64_t challenge_time_start;
    int64_t challenge_time_end;
    uint64_t max_time;
    float_t max_time_float;
    float_t current_time_float;
    uint64_t current_time;
    float_t current_time_float_dup;
    float_t field_2D03C;
    int32_t score_hold_multi;
    uint32_t score_hold;
    int32_t score_slide;
    uint8_t field_2D04C;
    uint8_t chance_time_endede;
    pv_disp2d* pv_disp2d;
    int32_t life_gauge_final;
    bool hit_border;
    char field_2D05D;
    char field_2D05E;
    char field_2D05F;
    char field_2D060;
    char field_2D061;
    char field_2D062;
    char field_2D063;
    bool field_2D064;
    char field_2D065;
    char field_2D066[2];
    char field_2D068;
    char field_2D069;
    char field_2D06A[2];
    int field_2D06C;
    uint32_t field_2D070;
    int field_2D074;
    int field_2D078;
    int field_2D07C;
    int field_2D080;
    int field_2D084;
    int32_t life_gauge_bonus;
    int32_t life_gauge_total_bonus;
    char field_2D090;
    char field_2D091;
    char field_2D092;
    char field_2D093;
    uint8_t field_2D094;
    char field_2D095;
    char field_2D096;
    bool success;
    int field_2D098;
    int field_2D09C;
    char field_2D0A0;
    bool pv_end_fadeout;
    float_t rival_percentage;
    int field_2D0A8;
    int field_2D0AC;
    int field_2D0B0[3];
    bool field_2D0BC;
    char field_2D0BD;
    char field_2D0BE;
    char field_2D0BF;
    int field_2D0C0;
    int field_2D0C4;
    //struc_154 chrcam_chrmot[6];
    std::vector<uint32_t> motion_set_ids;
    std::vector<uint32_t> motion_set_face_ids;
    std::vector<int32_t> obj_set_itmpv;
    std::vector<int32_t> obj_set_handitem;
    std::vector<int32_t> chreff_auth_3d_obj_set_ids;
    std::vector<int32_t> field_2D770;
    std::vector<int32_t> stgpvhrc_obj_set_ids;
    std::vector<int32_t> ex_song_ex_auth_obj_set_ids;
    std::vector<int32_t> stage_indices;
    std::vector<task_stage_info> stage_infos;
    char field_2D7E8;
    int field_2D7EC;
    std::vector<int32_t> field_2D7F0;
    int field_2D808;
    int field_2D80C;
    std::vector<int32_t> field_2D810;
    std::vector<int32_t> field_2D828;
    std::vector<int32_t> field_2D840;
    int field_2D858;
    float_t field_2D85C;
    float_t field_2D860;
    float_t field_2D864;
    float_t field_2D868;
    char field_2D86C;
    int camera_auth_3d_uid;
    int field_2D874;
    bool field_2D878;
    int field_2D87C;
    size_t current_field;
    //std::vector<pv_game_data_field_data> field_data;
    char field_2D8A0;
    int field_2D8A4;
    __int64 field_2D8A8;
    int field_2D8B0;
    int field_2D8B4;
    __int64 field_2D8B8;
    __int64 field_2D8C0;
    int field_2D8C8;
    int field_2D8CC;
    int field_2D8D0;
    int field_2D8D4;
    int field_2D8D8;
    int field_2D8DC;
    int field_2D8E0;
    int field_2D8E4;
    int field_2D8E8;
    int field_2D8EC;
    int field_2D8F0;
    int field_2D8F4;
    int field_2D8F8;
    int field_2D8FC;
    int field_2D900;
    int field_2D904;
    std::string campv_string;
    std::vector<int32_t> campv_auth_3d_uids;
    std::map<int32_t, int32_t> campv;
    int data_camera_id;
    int field_2D954;
    std::string itmpv_string;
    std::vector<int32_t> itmpv_auth_3d_uids;
    //std::map<int32_t, struc_644> itmpv[6];
    std::vector<std::string> field_2D9F0;
    //struc_78 field_2DA08[3];
    std::vector<int32_t> loaded_auth_3d_uids;
    char field_2DAC8;
    int field_2DACC;
    std::vector<std::string> auth_3d_categories;
    std::map<int32_t, int32_t> auth_3d;
    //std::vector<pv_game_data_chreff> chreff_auth_3d;
    //std::vector<pv_game_data_chreff> chreff_auth_3d_obj;
    char field_2DB28;
    vec2 field_2DB2C;
    int field_2DB34;
    time_struct field_2DB38;
    std::vector<uint64_t> field_2DB48;
    //std::vector<struc_12> field_2DB60;
};

class pv_game : public app::TaskWindow {
public:
    int32_t pv_id;

    std::vector<uint32_t> objset_load;

    int32_t state;
    int32_t frame;
    double_t frame_float;
    int64_t time;
    int32_t rob_chara_ids[ROB_CHARA_COUNT];

    std::string pv_category;
    std::string stage_category;
    std::string camera_category;

    std::vector<std::string> category_load;

    std::map<uint32_t, int32_t> pv_auth_3d_ids;
    int32_t light_auth_3d_id;
    int32_t camera_auth_3d_id;

    dsc dsc_m;
    dsc_data* dsc_data_ptr;
    dsc_data* dsc_data_ptr_end;

    bool play;
    bool success;
    int32_t chara_id;
    float_t target_anim_fps;
    float_t anim_frame_speed;
    int64_t dsc_time;
    bool pv_end;
    pv_play_data playdata[ROB_CHARA_COUNT];
    float_t scene_rot_y;
    mat4u scene_rot_mat;
    int32_t branch_mode;

    bool pause;
    bool step_frame;

    pv_game();
    virtual ~pv_game() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Window() override;

    void Load(int32_t pv_id, chara_index charas[6], int32_t modules[6]);
    void Unload();
};

extern pv_game pv_game_data;

#endif
