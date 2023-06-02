/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "player_data.hpp"

PlayerData player_data_array[2];

struc_765::struc_765() : field_10(), field_14() {
    field_0 = -1;
    field_4 = 5;
    field_8 = -1;
    field_C = -1;
}

struc_713::struc_713() : field_0(), field_4(), field_8(),
field_C(), field_10(), field_14(), field_18() {
    reset();
}

void struc_713::reset() {
    field_0 = -1;
    field_4 = -1;
    field_8 = -1;
    field_C = 0;
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
    field_1C = {};
    field_34 = {};
    field_4C = {};
    field_64 = {};
    field_7C = {};
    field_94 = {};
    field_AC = {};
    field_C4 = {};
    field_DC = {};
    field_F4 = {};
}

struc_761::struc_761() {
    field_0 = -1;
    field_8 = prj::time::get_default();
}

struc_762::struc_762() {

}

struc_766::struc_766() {
    field_0 = -1;
    field_4 = -1;
    field_8 = -1;
    field_C = -1;
}

struc_767::struc_767() : field_0() {
    for (bool& i : field_0)
        i = true;
}

struc_763::struc_763() {
    field_0 = -1;
    field_8 = prj::time::get_default();
}

struc_764::struc_764() {

}

struc_715::struc_715() {
    field_0 = -1;
    field_8 = -1;
    field_10 = prj::time::get_default();
}

struc_768::struc_768() : field_0() {

}

struc_768::~struc_768() {

}

struc_769::struc_769() : field_0() {

}

struc_769::~struc_769() {

}

struc_770::struc_770() : field_0(), field_4(), field_8(), field_90() {

}

struc_770::~struc_770() {

}

struc_771::struc_771() : field_0() {

}

struc_771::~struc_771() {

}

struc_772::struc_772() {
    field_0 = -999;
    field_4 = -999;
    field_8 = -999;
    field_C = -999;
}

PlayerData::Contest::Contest() : enable(), field_1(), field_4(), field_8(), field_C() {

}

void PlayerData::Contest::Reset() {
    enable = false;
    field_1 = false;
    field_4 = -1;
    field_8 = 0;
    field_C = false;

    for (struc_713& i : field_10)
        i.reset();
}

PlayerData::PlayerData() : field_0(), freeplay(), card_type(), field_A8(), play_data_id(), accept_index(),
start_index(), field_DC(), level(), level_plate_id(), level_plate_effect(), vocaloid_point(), hp_vol(), act_toggle(),
act_vol(), act_slide_vol(), module_equip(), module_equip_cmn(), field_1F0(), use_pv_module_equip(),
ch_pv_module_equip(), module_filter_kind(), field_2B8(), skin_equip(), skin_equip_cmn(),
use_pv_skin_equip(), btn_se_equip(), btn_se_equip_cmn(), use_pv_btn_se_equip(), slide_se_equip(),
slide_se_equip_cmn(), use_pv_slide_se_equip(), chainslide_se_equip(), chainslide_se_equip_cmn(),
use_pv_chainslide_se_equip(), slidertouch_se_equip(), slidertouch_se_equip_cmn(),use_pv_slidertouch_se_equip(),
field_584(), field_668(), field_66C(), field_D08(), field_D10(), field_D18(), field_D90(), clear_border(),
field_D98(), field_DA0(), field_DA8(), field_DB0(), field_DB8(), field_DC0(), field_DC8(), field_DD0(),
field_DD8(), field_DE0(), field_DE8(), field_DEC(), field_DF0(), field_DF4(), field_E00(), field_E28(),
field_E30(), field_E34(), game_opts(), field_E36(), field_E38(), field_E40(), field_EC0(), field_F40() {
    field_D08 = -1;
    field_D10 = -1;
    field_D18 = -1;

    Reset();
    return;
}

PlayerData::~PlayerData() {

}

void PlayerData::Reset() {
    field_0 = false;
    freeplay = false;
    card_type = 0;
    field_8.assign("0");
    field_28.assign("0");
    field_48.assign("0");
    field_68.assign("0");
    field_88.assign("0");
    field_A8 = -1;
    field_B0.assign("0");

    play_data_id = -1;
    accept_index = -1;
    start_index = -1;

    player_name.assign("ＮＯ－ＮＡＭＥ");
    level_name.assign("");

    level = 1;
    level_plate_id = 0;
    level_plate_effect = -1;
    vocaloid_point = 0;
    hp_vol = 0;
    act_toggle = 1;
    act_vol = 100;
    act_slide_vol = 100;

    field_140.reset();

    for (int32_t& i : module_equip)
        i = 0;

    for (int32_t& i : module_equip_cmn)
        i = 0;

    for (int32_t& i : field_1F0)
        i = 0;

    field_208 = prj::time::get_default();
    field_210 = {};

    use_pv_module_equip = false;
    ch_pv_module_equip = false;
    module_filter_kind = 0;

    for (int32_t& i : module_equip)
        i = 0;

    for (int32_t& i : module_equip_cmn)
        i = 0;

    for (int32_t& i : field_1F0)
        i = -999;

    for (int64_t& i : field_2B8)
        i = 0;

    for (struc_766& i : field_338)
        i = {};

    for (struc_766& i : field_398)
        i = {};

    for (struc_772& i : field_3F8)
        i = {};

    for (struc_767& i : field_458)
        i = {};

    for (struc_767& i : field_470)
        i = {};

    for (struc_767& i : field_488)
        i = {};

    field_4A0 = prj::time::get_default();
    field_4A8 = {};

    for (auto& i : field_5D0)
        i.clear();

    field_630.clear();

    skin_equip = 0;
    skin_equip_cmn = 0;
    use_pv_skin_equip = 0;
    btn_se_equip = -1;
    btn_se_equip_cmn = -1;
    use_pv_btn_se_equip = 0;
    slide_se_equip = -1;
    slide_se_equip_cmn = -1;
    use_pv_slide_se_equip = 0;
    chainslide_se_equip = -1;
    chainslide_se_equip_cmn = -1;
    use_pv_chainslide_se_equip = 0;
    slidertouch_se_equip = -1;
    slidertouch_se_equip_cmn = -1;
    use_pv_slidertouch_se_equip = 0;

    field_584 = 2;

    for (auto& i : mylist)
        i.clear();

    field_648.assign("************");
    field_668 = -1;
    field_66C = 0;
    contest.Reset();

    for (struc_715& i : field_AB0)
        i = {};

    field_D20 = prj::time::get_default();
    field_D28.clear();
    field_D40.clear();
    field_D58.clear();
    field_D70.clear();

    static prj::time start_of_2011 = prj::strptime("2011-01-01 00:00:00");
    field_D88 = start_of_2011;

    field_D90 = false;
    clear_border = 0;
    field_D98 = -1;
    field_DA0 = -1;
    field_DA8 = -1;
    field_DB0 = -1;
    field_DB8 = -1;
    field_DC0 = -1;
    field_DC8 = -1;
    field_DD0 = -1;
    field_DD8 = -1;
    field_DE0 = -1;
    field_DE8 = 0;
    field_DEC = -1;
    field_DF0 = 0;

    for (int32_t& i : field_DF4)
        i = -1;

    for (int32_t& i : field_E00)
        i = 0;

    field_E10.clear();
    if (field_E10.capacity() < 6)
        field_E10.reserve(6);

    field_E28 = 0;
    field_E30 = 0;
    field_E34 = false;
    game_opts = 0;
    field_E38 = 0;
    field_F40 = 2;
    field_E36 = true;

    for (int64_t& i : field_E40)
        i = 0;

    for (int64_t& i : field_EC0)
        i = 0;
}

PlayerData* player_data_array_get(uint32_t index) {
    return &player_data_array[index];
}
