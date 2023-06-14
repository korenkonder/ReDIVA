/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../dw.hpp"
#include "../task_window.hpp"

class DtmEqVs : public app::Task {
public:
    int32_t state;
    int32_t chara_id;
    ::chara_index chara_index;
    ::chara_index curr_chara_index;
    int32_t module_index;
    int32_t curr_module_index;
    int32_t field_80;

    DtmEqVs();
    virtual ~DtmEqVs() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;

    virtual bool AddTask(int32_t chara_id, ::chara_index chara_index);
    virtual bool DelTask();

    void SetCharaIndexModuleIndex(::chara_index chara_index, int32_t module_index);

    void sub_140261FF0();
    void sub_140262050();
};

class DataTestEquipDw : public dw::Shell {
public:
    struct Data {
        bool field_0;
        bool field_1;
        bool field_2;
        bool field_3;
        uint32_t field_4;
        item_cos_data* cos;
        chara_index chara_index;
        int32_t module_index;
        bool field_18;
        int field_1C;
        std::vector<uint32_t> field_20[25];
        std::map<object_info, item_id> field_278[25];
        uint32_t field_408[25];

        Data();
        ~Data();
    };

    int32_t chara_id;
    DataTestEquipDw::Data data[6];
    dw::Composite* field_1C28[25];
    dw::Label* field_1CF0[25];
    dw::ListBox* field_1DB8[25];
    dw::ListBox* field_1E80;
    __int64 field_1E88;
    __int64 field_1E90;
    __int64 field_1E98;
    __int64 field_1EA0;
    __int64 field_1EA8;
    __int64 field_1EB0;
    __int64 field_1EB8;
    __int64 field_1EC0;
    __int64 field_1EC8;
    __int64 field_1ED0;
    __int64 field_1ED8;
    __int64 field_1EE0;
    __int64 field_1EE8;
    __int64 field_1EF0;
    __int64 field_1EF8;
    __int64 field_1F00;
    __int64 field_1F08;
    __int64 field_1F10;
    __int64 field_1F18;
    __int64 field_1F20;
    __int64 field_1F28;
    __int64 field_1F30;
    __int64 field_1F38;
    __int64 field_1F40;
    __int64 field_1F48;
    __int64 field_1F50;
    __int64 field_1F58;
    __int64 field_1F60;
    __int64 field_1F68;
    __int64 field_1F70;
    __int64 field_1F78;
    __int64 field_1F80;
    __int64 field_1F88;
    __int64 field_1F90;
    __int64 field_1F98;
    __int64 field_1FA0;
    __int64 field_1FA8;
    __int64 field_1FB0;
    __int64 field_1FB8;
    __int64 field_1FC0;
    __int64 field_1FC8;
    __int64 field_1FD0;
    __int64 field_1FD8;
    __int64 field_1FE0;
    __int64 field_1FE8;
    __int64 field_1FF0;
    __int64 field_1FF8;
    __int64 field_2000;
    __int64 field_2008;
    __int64 field_2010;
    __int64 field_2018;
    __int64 field_2020;
    __int64 field_2028;
    __int64 field_2030;
    __int64 field_2038;
    __int64 field_2040;
    __int64 field_2048;
    __int64 field_2050;
    __int64 field_2058;
    __int64 field_2060;
    __int64 field_2068;
    __int64 field_2070;
    __int64 field_2078;
    __int64 field_2080;
    __int64 field_2088;
    __int64 field_2090;
    __int64 field_2098;
    __int64 field_20A0;
    __int64 field_20A8;
    __int64 field_20B0;
    __int64 field_20B8;
    __int64 field_20C0;
    __int64 field_20C8;
    __int64 field_20D0;
    __int64 field_20D8;
    __int64 field_20E0;
    __int64 field_20E8;
    __int64 field_20F0;
    __int64 field_20F8;
    __int64 field_2100;
    __int64 field_2108;
    __int64 field_2110;
    __int64 field_2118;
    __int64 field_2120;
    __int64 field_2128;
    __int64 field_2130;
    __int64 field_2138;
    __int64 field_2140;
    __int64 field_2148;
    __int64 field_2150;
    __int64 field_2158;
    __int64 field_2160;
    __int64 field_2168;
    __int64 field_2170;
    __int64 field_2178;
    __int64 field_2180;
    __int64 field_2188;
    __int64 field_2190;
    __int64 field_2198;
    __int64 field_21A0;
    __int64 field_21A8;
    __int64 field_21B0;
    __int64 field_21B8;
    __int64 field_21C0;
    __int64 field_21C8;
    __int64 field_21D0;
    __int64 field_21D8;
    __int64 field_21E0;
    __int64 field_21E8;
    __int64 field_21F0;
    __int64 field_21F8;
    __int64 field_2200;
    __int64 field_2208;
    __int64 field_2210;
    __int64 field_2218;
    __int64 field_2220;
    __int64 field_2228;
    __int64 field_2230;
    __int64 field_2238;
    __int64 field_2240;
    __int64 field_2248;
    __int64 field_2250;
    __int64 field_2258;
    __int64 field_2260;
    __int64 field_2268;
};

extern DtmEqVs* dtm_eq_vs_array;

extern void equip_test_init();
extern void equip_test_free();
