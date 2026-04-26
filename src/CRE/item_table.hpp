/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/prj/vector_pair_combine.hpp"
#include "file_handler.hpp"
#include "object.hpp"
#include "texture.hpp"

struct RobItemDataObj {
    object_info uid;
    ROB_PARTS_KIND replace_id;

    RobItemDataObj();
};

struct RobItemDataOfs {
    ROB_ITEM_EQUIP_SUB_ID equip_sub_id;
    uint32_t item_no;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    RobItemDataOfs();
};

struct RobItemDataTex {
    uint32_t org_uid;
    uint32_t chg_uid;

    RobItemDataTex();
};

struct RobItemDataColFlagMember {
    uint32_t is_available : 1;
    uint32_t reserve : 31;
};

union RobItemDataColFlag {
    uint32_t w;
    RobItemDataColFlagMember m;
};

struct RobItemDataCol {
    uint32_t tex_uid;
    RobItemDataColFlag flag;
    ImgfColorToneParam color;

    RobItemDataCol();
};

struct RobItemData {
    std::vector<RobItemDataObj> obj;
    std::vector<RobItemDataOfs> ofs;
    std::vector<RobItemDataTex> tex;
    std::vector<RobItemDataCol> col;

    RobItemData();
    ~RobItemData();
};

struct RobItemTableFlagMember {
    uint32_t is_dummy : 1;
    uint32_t is_present : 1;
    uint32_t is_texorg : 1;
    uint32_t is_objorg : 1;
    uint32_t reserve : 28;
};

union RobItemTableFlag {
    uint32_t w;
    RobItemTableFlagMember m;
};

struct RobItemTable {
    RobItemTableFlag flag;
    std::string name;
    std::vector<uint32_t> objset;
    ROB_ITEM_TYPE type;
    uint32_t attr;
    ROB_ITEM_EQUIP_DES_ID equip_des_id;
    ROB_ITEM_EQUIP_SUB_ID equip_sub_id;
    RobItemData data;
    uint32_t exclusion;
    uint32_t point;
    uint32_t org_itm;
    bool npr_flag;
    float_t face_depth;

    RobItemTable();
    ~RobItemTable();
};

struct RobItemEquip {
    uint32_t item_no[ROB_ITEM_EQUIP_SUB_ID_MAX];

    RobItemEquip();
};

struct item_table_dbgset {
    item_table_dbgset();
    ~item_table_dbgset();
};

struct RobItemHeader {
    prj::vector_pair_combine<uint32_t, RobItemTable> table;
    prj::vector_pair_combine<uint32_t, RobItemEquip> defset;
    prj::vector_pair_combine<std::string, RobItemEquip> dbgset;

    RobItemHeader();
    ~RobItemHeader();

    const RobItemTable* get_item(uint32_t item_no);
};

extern void item_table_handler_array_init();
extern bool item_table_handler_array_load();
extern void item_table_handler_array_read();
extern void item_table_handler_array_free();

extern const RobItemEquip* get_default_costume_data(CHARA_NUM cn, int32_t cos_id);
extern const RobItemHeader* get_rob_item_header(CHARA_NUM cn);
extern const RobItemTable* get_rob_item_table(CHARA_NUM cn, uint32_t item_no);
extern std::string get_rob_item_table_name(CHARA_NUM cn, uint32_t item_no);
extern const std::vector<uint32_t>* get_rob_item_table_objset(CHARA_NUM cn, uint32_t item_no);
