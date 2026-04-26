/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include "../default.hpp"
#include "../image.hpp"
#include "../vec.hpp"

enum CHARA_NUM {
    CN_NONE   = -1,

    CN_MIKU = 0,
    CN_RIN,
    CN_LEN,
    CN_LUKA,
    CN_NERU,
    CN_HAKU,
    CN_KAITO,
    CN_MEIKO,
    CN_SAKINE,
    CN_TETO,
    CN_MAX,
};

enum ROB_ITEM_ATTR {
    ROB_ITEM_ATTR_NONE = 0,

    ROB_ITEM_ATTR_OBJ     = 0x00000001,
    ROB_ITEM_ATTR_OFS     = 0x00000002,
    ROB_ITEM_ATTR_TEX     = 0x00000004,
    ROB_ITEM_ATTR_COL     = 0x00000008,
    ROB_ITEM_ATTR_HYOUTAN = 0x00000010,
    ROB_ITEM_ATTR_NUDE    = 0x00000020,
    ROB_ITEM_ATTR_HADA    = 0x00000040,
    ROB_ITEM_ATTR_NO_SDW  = 0x00000080,
    ROB_ITEM_ATTR_NO_REF  = 0x00000100,
    ROB_ITEM_ATTR_SYM     = 0x00000200,
    ROB_ITEM_ATTR_NO_MRG  = 0x00000400,
    ROB_ITEM_ATTR_HEAD    = 0x00000800,
    ROB_ITEM_ATTR_MAX     = 0x80000000,
};

enum ROB_ITEM_EQUIP_DES_ID {
    ROB_ITEM_EQUIP_DES_ID_NONE = -1,

    ROB_ITEM_EQUIP_DES_ID_ATAM = 0,
    ROB_ITEM_EQUIP_DES_ID_FACE,
    ROB_ITEM_EQUIP_DES_ID_JOHA,
    ROB_ITEM_EQUIP_DES_ID_KAHA,
    ROB_ITEM_EQUIP_DES_ID_HADA,
    ROB_ITEM_EQUIP_DES_ID_MAX,
};

enum ROB_ITEM_EQUIP_ID {
    ROB_ITEM_EQUIP_ID_NONE = -1,

    ROB_ITEM_EQUIP_ID_ATAM = 0,
    ROB_ITEM_EQUIP_ID_KAO,
    ROB_ITEM_EQUIP_ID_JOHA,
    ROB_ITEM_EQUIP_ID_UDE,
    ROB_ITEM_EQUIP_ID_KAHA,
    ROB_ITEM_EQUIP_ID_MAX,
};

enum ROB_ITEM_EQUIP_SUB_ID {
    ROB_ITEM_EQUIP_SUB_ID_NONE = -1,

    ROB_ITEM_EQUIP_SUB_ID_ZUJO = 0,
    ROB_ITEM_EQUIP_SUB_ID_KAMI,
    ROB_ITEM_EQUIP_SUB_ID_HITAI,
    ROB_ITEM_EQUIP_SUB_ID_ME,
    ROB_ITEM_EQUIP_SUB_ID_MEGANE,
    ROB_ITEM_EQUIP_SUB_ID_MIMI,
    ROB_ITEM_EQUIP_SUB_ID_KUCHI,
    ROB_ITEM_EQUIP_SUB_ID_MAKI,
    ROB_ITEM_EQUIP_SUB_ID_KUBI,
    ROB_ITEM_EQUIP_SUB_ID_INNER,
    ROB_ITEM_EQUIP_SUB_ID_OUTER,
    ROB_ITEM_EQUIP_SUB_ID_KATA,
    ROB_ITEM_EQUIP_SUB_ID_U_UDE,
    ROB_ITEM_EQUIP_SUB_ID_L_UDE,
    ROB_ITEM_EQUIP_SUB_ID_TE,
    ROB_ITEM_EQUIP_SUB_ID_JOHA_MAE,
    ROB_ITEM_EQUIP_SUB_ID_JOHA_USHIRO,
    ROB_ITEM_EQUIP_SUB_ID_BELT,
    ROB_ITEM_EQUIP_SUB_ID_KOSI,
    ROB_ITEM_EQUIP_SUB_ID_PANTS,
    ROB_ITEM_EQUIP_SUB_ID_ASI,
    ROB_ITEM_EQUIP_SUB_ID_SUNE,
    ROB_ITEM_EQUIP_SUB_ID_KUTSU,
    ROB_ITEM_EQUIP_SUB_ID_HADA,
    ROB_ITEM_EQUIP_SUB_ID_HEAD,
    ROB_ITEM_EQUIP_SUB_ID_MAX,
};

enum ROB_ITEM_TYPE {
    ROB_ITEM_TYPE_NONE = -1,

    ROB_ITEM_TYPE_EQUIP = 0,
    ROB_ITEM_TYPE_REPLACE,
    ROB_ITEM_TYPE_LOOKS,
    ROB_ITEM_TYPE_REM,
    ROB_ITEM_TYPE_MAX,
};

enum ROB_PARTS_KIND {
    RPK_NONE = -1,

    RPK_BODY = 0,
    RPK_ATAMA,
    RPK_KATA_R,
    RPK_MUNE,
    RPK_KATA_L,
    RPK_UDE_R,
    RPK_SENAKA,
    RPK_UDE_L,
    RPK_HARA,
    RPK_KOSI,
    RPK_TE_R,
    RPK_TE_L,
    RPK_MOMO,
    RPK_SUNE,
    RPK_ASI,
    RPK_ITEM01,
    RPK_ITEM02,
    RPK_ITEM03,
    RPK_ITEM04,
    RPK_ITEM05,
    RPK_ITEM06,
    RPK_ITEM07,
    RPK_ITEM08,
    RPK_ITEM09,
    RPK_ITEM10,
    RPK_ITEM11,
    RPK_ITEM12,
    RPK_ITEM13,
    RPK_ITEM14,
    RPK_ITEM15,
    RPK_ITEM16,
    RPK_MAX,

    RPK_TOP = RPK_BODY,

    RPK_1SKIN_BEGIN    = RPK_BODY,
    RPK_1SKIN_END      = RPK_BODY,
    RPK_1SKIN_MAX      = RPK_1SKIN_END - RPK_1SKIN_BEGIN + 1,

    RPK_BASE_BEGIN     = RPK_ATAMA,
    RPK_BASE_END       = RPK_ASI,
    RPK_BASE_MAX       = RPK_ASI - RPK_ATAMA + 1,

    RPK_ITEM_BEGIN     = RPK_ITEM01,
    RPK_ITEM_END       = RPK_ITEM16,
    RPK_ITEM_MAX       = RPK_ITEM_END - RPK_ITEM_BEGIN + 1,

    RPK_ITEM_ETC_BEGIN = RPK_ITEM09,
    RPK_ITEM_ETC_END   = RPK_ITEM16,
    RPK_ITEM_ETC_MAX   = RPK_ITEM_ETC_END - RPK_ITEM_ETC_BEGIN + 1,

    RPK_DISP_BEGIN     = RPK_ATAMA,
    RPK_DISP_END       = RPK_ITEM16,
    RPK_DISP_MAX       = RPK_DISP_END - RPK_DISP_BEGIN + 1,

    RPK_KAMI           = RPK_ITEM01,
    RPK_OUTER          = RPK_ITEM02,
    RPK_PANTS          = RPK_ITEM03,
    RPK_ZUJO           = RPK_ITEM04,
    RPK_MEGANE         = RPK_ITEM05,
    RPK_KUBI           = RPK_ITEM06,
    RPK_JOHA_USHIRO    = RPK_ITEM07,
    RPK_KUCHI          = RPK_ITEM08,

    RPK_SP_UDE_L = 100,
    RPK_SP_TE_L  = 101,
    RPK_SP_UDE_R = 102,
    RPK_SP_TE_R  = 103,
    RPK_SP_SUNE  = 104,
    RPK_SP_ASI   = 105,
};

struct itm_table_item_data_obj {
    std::string uid;
    ROB_PARTS_KIND rpk;

    itm_table_item_data_obj();
    ~itm_table_item_data_obj();
};

struct itm_table_item_data_ofs {
    ROB_ITEM_EQUIP_SUB_ID sub_id;
    uint32_t item_no;
    vec3 trans;
    vec3 rot;
    vec3 scale;

    itm_table_item_data_ofs();
};

struct itm_table_item_data_tex {
    std::string org;
    std::string chg;

    itm_table_item_data_tex();
    ~itm_table_item_data_tex();
};

struct itm_table_item_data_col_flag_member {
    uint32_t is_available : 1;
    uint32_t reserve : 31;
};

union itm_table_item_data_col_flag {
    uint32_t w;
    itm_table_item_data_col_flag_member m;
};

struct itm_table_item_data_col {
    std::string tex;
    itm_table_item_data_col_flag flag;
    ImgfColorToneParam color;

    itm_table_item_data_col();
    ~itm_table_item_data_col();
};

struct itm_table_item_data {
    std::vector<itm_table_item_data_obj> obj;
    std::vector<itm_table_item_data_ofs> ofs;
    std::vector<itm_table_item_data_tex> tex;
    std::vector<itm_table_item_data_col> col;

    itm_table_item_data();
    ~itm_table_item_data();
};

struct itm_table_item_flag_member {
    uint32_t is_dummy : 1;
    uint32_t is_present : 1;
    uint32_t is_texorg : 1;
    uint32_t is_objorg : 1;
    uint32_t reserve : 28;
};

union itm_table_item_flag {
    uint32_t w;
    itm_table_item_flag_member m;
};

struct itm_table_item {
    uint32_t no;
    itm_table_item_flag flag;
    std::string name;
    std::vector<std::string> objset;
    ROB_ITEM_TYPE type;
    uint32_t attr;
    ROB_ITEM_EQUIP_DES_ID equip_des_id;
    ROB_ITEM_EQUIP_SUB_ID equip_sub_id;
    itm_table_item_data data;
    uint32_t exclusion;
    uint32_t point;
    uint32_t org_itm;
    bool npr_flag;
    float_t face_depth;

    itm_table_item();
    ~itm_table_item();
};

struct itm_table_cos {
    int32_t id;
    std::vector<int32_t> item;

    itm_table_cos();
    ~itm_table_cos();
};

struct itm_table_dbgset {
    std::string name;
    std::vector<int32_t> item;

    itm_table_dbgset();
    ~itm_table_dbgset();
};

struct itm_table {
    bool ready;

    std::vector<itm_table_item> item;
    std::vector<itm_table_cos> cos;
    std::vector<itm_table_dbgset> dbgset;

    itm_table();
    ~itm_table();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

extern const char* rob_parts_kind_str[];
extern const char* rob_item_equip_sub_id_str[];

extern const char* get_char_id_str(CHARA_NUM cname);
extern const char* get_chara_name(CHARA_NUM cname);
extern const char* get_chara_name_face_mot(CHARA_NUM cname);
extern const char* get_chara_name_full(CHARA_NUM cname);
extern CHARA_NUM get_chara_num_from_char_id(const char* in_name);

extern const char* item_id_get_name(ROB_PARTS_KIND rpk);
extern const char* item_sub_id_get_name(ROB_ITEM_EQUIP_SUB_ID sub_id);
