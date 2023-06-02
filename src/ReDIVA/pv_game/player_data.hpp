/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/prj/time.hpp"
#include <bitset>
#include <string>
#include <vector>

struct struc_765 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    int32_t field_14;

    struc_765();
};

struct struc_713 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
    struc_765 field_1C;
    struc_765 field_34;
    struc_765 field_4C;
    struc_765 field_64;
    struc_765 field_7C;
    struc_765 field_94;
    struc_765 field_AC;
    struc_765 field_C4;
    struc_765 field_DC;
    struc_765 field_F4;

    struc_713();

    void reset();
};

struct struc_761 {
    int32_t field_0;
    prj::time field_8;

    struc_761();
};

struct struc_762 {
    struc_761 field_0[10];

    struc_762();
};

struct struc_766 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;

    struc_766();
};

struct struc_767 {
    bool field_0[4];

    struc_767();
};

struct struc_763 {
    int32_t field_0;
    prj::time field_8;

    struc_763();
};

struct struc_764 {
    struc_763 field_0[10];

    struc_764();
};

struct struc_715 {
    int64_t field_0;
    int32_t field_8;
    prj::time field_10;

    struc_715();
};

struct struc_758 {
    int32_t pv_id;
    int32_t edition;
    int32_t field_8[6];
    int32_t field_20[24];
    int8_t field_80[6];
    int32_t field_88;
    int32_t field_8C;
    int32_t field_90;
    int32_t field_94;
    int32_t field_98;
    int32_t field_9C;
    int32_t field_A0;
    int32_t field_A4;
    int32_t field_A8;
    int32_t field_AC;
    int32_t field_B0;
    int32_t field_B4;
    int32_t field_B8;
    int8_t field_BC;
    int32_t field_C0;
    int32_t field_C4;
    int8_t field_C8;
    int32_t field_CC;
    int32_t field_D0;
    int32_t rival_percentage;
    int32_t field_D8;
    int8_t field_DC[4];
    int8_t field_E0[4];
};

struct struc_768 {
    int64_t field_0;
    std::string field_8;
    std::string field_28;

    struc_768();
    ~struc_768();
};

struct struc_769 {
    int64_t field_0;
    std::string field_8;

    struc_769();
    ~struc_769();
};

struct struc_770 {
    int32_t field_0;
    int32_t field_4;
    int64_t field_8;
    std::string field_10;
    std::string field_30;
    std::string field_50;
    std::string field_70;
    bool field_90;

    struc_770();
    ~struc_770();
};

struct struc_771 {
    int64_t field_0;
    std::string field_8;
    std::string field_28;
    std::string field_48;

    struc_771();
    ~struc_771();
};

struct struc_772 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;

    struc_772();
};

struct PlayerData {
    struct Contest {
        bool enable;
        bool field_1;
        int32_t field_4;
        int32_t field_8;
        bool field_C;
        struc_713 field_10[4];

        Contest();

        void Reset();
    };

    bool field_0;
    bool freeplay;
    int32_t card_type;
    std::string field_8;
    std::string field_28;
    std::string field_48;
    std::string field_68;
    std::string field_88;
    int32_t field_A8;
    std::string field_B0;
    int32_t play_data_id;
    int32_t accept_index;
    int32_t start_index;
    int32_t field_DC;
    std::string player_name;
    std::string level_name;
    int32_t level;
    int32_t level_plate_id;
    int32_t level_plate_effect;
    int32_t vocaloid_point;
    int32_t hp_vol;
    int8_t act_toggle;
    int32_t act_vol;
    int32_t act_slide_vol;
    std::bitset<1000> field_140;
    int32_t module_equip[6];
    int32_t module_equip_cmn[6];
    int32_t field_1F0[6];
    prj::time field_208;
    struc_762 field_210;
    bool use_pv_module_equip;
    bool ch_pv_module_equip;
    int32_t module_filter_kind;
    int64_t field_2B8[16];
    struc_766 field_338[6];
    struc_766 field_398[6];
    struc_772 field_3F8[6];
    struc_767 field_458[6];
    struc_767 field_470[6];
    struc_767 field_488[6];
    prj::time field_4A0;
    struc_764 field_4A8;
    uint32_t skin_equip;
    int32_t skin_equip_cmn;
    int8_t use_pv_skin_equip;
    int32_t btn_se_equip;
    int32_t btn_se_equip_cmn;
    bool use_pv_btn_se_equip;
    int32_t slide_se_equip;
    int32_t slide_se_equip_cmn;
    bool use_pv_slide_se_equip;
    int32_t chainslide_se_equip;
    int32_t chainslide_se_equip_cmn;
    bool use_pv_chainslide_se_equip;
    int32_t slidertouch_se_equip;
    int32_t slidertouch_se_equip_cmn;
    bool use_pv_slidertouch_se_equip;
    int32_t field_584;
    std::vector<void*> mylist[3];
    std::vector<struc_758> field_5D0[4];
    std::vector<void*> field_630;
    std::string field_648;
    int32_t field_668;
    int32_t field_66C;
    PlayerData::Contest contest;
    struc_715 field_AB0[25];
    int64_t field_D08;
    int32_t field_D10;
    int64_t field_D18;
    prj::time field_D20;
    std::vector<struc_771> field_D28;
    std::vector<struc_768> field_D40;
    std::vector<struc_768> field_D58;
    std::vector<struc_769> field_D70;
    prj::time field_D88;
    bool field_D90;
    int32_t clear_border;
    int64_t field_D98;
    int64_t field_DA0;
    int64_t field_DA8;
    int64_t field_DB0;
    int64_t field_DB8;
    int64_t field_DC0;
    int64_t field_DC8;
    int64_t field_DD0;
    int64_t field_DD8;
    int64_t field_DE0;
    int8_t field_DE8;
    int32_t field_DEC;
    int8_t field_DF0;
    int32_t field_DF4[3];
    int32_t field_E00[3];
    std::vector<struc_770> field_E10;
    int64_t field_E28;
    int32_t field_E30;
    bool field_E34;
    bool game_opts;
    bool field_E36;
    int64_t field_E38;
    int64_t field_E40[16];
    int64_t field_EC0[16];
    int32_t field_F40;

    PlayerData();
    ~PlayerData();

    void Reset();
};

extern PlayerData* player_data_array_get(uint32_t index);
