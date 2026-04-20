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
    CN_MIKU   = 0x00,
    CN_RIN    = 0x01,
    CN_LEN    = 0x02,
    CN_LUKA   = 0x03,
    CN_NERU   = 0x04,
    CN_HAKU   = 0x05,
    CN_KAITO  = 0x06,
    CN_MEIKO  = 0x07,
    CN_SAKINE = 0x08,
    CN_TETO   = 0x09,
    CN_MAX    = 0x0A,
};

enum ROB_PARTS_KIND {
    RPK_NONE   = -1,
    RPK_BODY   = 0x00,
    RPK_ATAMA  = 0x01,
    RPK_KATA_R = 0x02,
    RPK_MUNE   = 0x03,
    RPK_KATA_L = 0x04,
    RPK_UDE_R  = 0x05,
    RPK_SENAKA = 0x06,
    RPK_UDE_L  = 0x07,
    RPK_HARA   = 0x08,
    RPK_KOSI   = 0x09,
    RPK_TE_R   = 0x0A,
    RPK_TE_L   = 0x0B,
    RPK_MOMO   = 0x0C,
    RPK_SUNE   = 0x0D,
    RPK_ASI    = 0x0E,
    RPK_ITEM01 = 0x0F,
    RPK_ITEM02 = 0x10,
    RPK_ITEM03 = 0x11,
    RPK_ITEM04 = 0x12,
    RPK_ITEM05 = 0x13,
    RPK_ITEM06 = 0x14,
    RPK_ITEM07 = 0x15,
    RPK_ITEM08 = 0x16,
    RPK_ITEM09 = 0x17,
    RPK_ITEM10 = 0x18,
    RPK_ITEM11 = 0x19,
    RPK_ITEM12 = 0x1A,
    RPK_ITEM13 = 0x1B,
    RPK_ITEM14 = 0x1C,
    RPK_ITEM15 = 0x1D,
    RPK_ITEM16 = 0x1E,
    RPK_MAX    = 0x1F,

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
};

enum item_sub_id {
    ITEM_SUB_NONE        = -1,
    ITEM_SUB_ZUJO        = 0x00,
    ITEM_SUB_KAMI        = 0x01,
    ITEM_SUB_HITAI       = 0x02,
    ITEM_SUB_ME          = 0x03,
    ITEM_SUB_MEGANE      = 0x04,
    ITEM_SUB_MIMI        = 0x05,
    ITEM_SUB_KUCHI       = 0x06,
    ITEM_SUB_MAKI        = 0x07,
    ITEM_SUB_KUBI        = 0x08,
    ITEM_SUB_INNER       = 0x09,
    ITEM_SUB_OUTER       = 0x0A,
    ITEM_SUB_KATA        = 0x0B,
    ITEM_SUB_U_UDE       = 0x0C,
    ITEM_SUB_L_UDE       = 0x0D,
    ITEM_SUB_TE          = 0x0E,
    ITEM_SUB_JOHA_MAE    = 0x0F,
    ITEM_SUB_JOHA_USHIRO = 0x10,
    ITEM_SUB_BELT        = 0x11,
    ITEM_SUB_KOSI        = 0x12,
    ITEM_SUB_PANTS       = 0x13,
    ITEM_SUB_ASI         = 0x14,
    ITEM_SUB_SUNE        = 0x15,
    ITEM_SUB_KUTSU       = 0x16,
    ITEM_SUB_HADA        = 0x17,
    ITEM_SUB_HEAD        = 0x18,
    ITEM_SUB_MAX         = 0x19,
};

struct itm_table_item_data_obj {
    std::string uid;
    ROB_PARTS_KIND rpk;

    itm_table_item_data_obj();
    ~itm_table_item_data_obj();
};

struct itm_table_item_data_ofs {
    item_sub_id sub_id;
    int32_t no;
    vec3 position;
    vec3 rotation;
    vec3 scale;

    itm_table_item_data_ofs();
};

struct itm_table_item_data_tex {
    std::string org;
    std::string chg;

    itm_table_item_data_tex();
    ~itm_table_item_data_tex();
};

struct itm_table_item_data_col {
    std::string tex;
    int32_t flag;
    color_tone col_tone;

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

struct itm_table_item {
    int32_t no;
    int32_t flag;
    std::string name;
    std::vector<std::string> objset;
    int32_t type;
    int32_t attr;
    int32_t des_id;
    item_sub_id sub_id;
    itm_table_item_data data;
    int32_t exclusion;
    int32_t point;
    int32_t org_itm;
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
extern const char* item_sub_id_get_name(item_sub_id sub_id);
