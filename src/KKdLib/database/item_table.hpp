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

enum chara_index {
    CHARA_NONE   = -1,
    CHARA_MIKU   = 0x00,
    CHARA_RIN    = 0x01,
    CHARA_LEN    = 0x02,
    CHARA_LUKA   = 0x03,
    CHARA_NERU   = 0x04,
    CHARA_HAKU   = 0x05,
    CHARA_KAITO  = 0x06,
    CHARA_MEIKO  = 0x07,
    CHARA_SAKINE = 0x08,
    CHARA_TETO   = 0x09,
    CHARA_MAX    = 0x0A,
};

enum item_id {
    ITEM_NONE        = -1,
    ITEM_BODY        = 0x00,
    ITEM_ATAMA       = 0x01,
    ITEM_KATA_R      = 0x02,
    ITEM_MUNE        = 0x03,
    ITEM_KATA_L      = 0x04,
    ITEM_UDE_R       = 0x05,
    ITEM_SENAKA      = 0x06,
    ITEM_UDE_L       = 0x07,
    ITEM_HARA        = 0x08,
    ITEM_KOSI        = 0x09,
    ITEM_TE_R        = 0x0A,
    ITEM_TE_L        = 0x0B,
    ITEM_MOMO        = 0x0C,
    ITEM_SUNE        = 0x0D,
    ITEM_ASI         = 0x0E,
    ITEM_KAMI        = 0x0F,
    ITEM_OUTER       = 0x10,
    ITEM_PANTS       = 0x11,
    ITEM_ZUJO        = 0x12,
    ITEM_MEGANE      = 0x13,
    ITEM_KUBI        = 0x14,
    ITEM_JOHA_USHIRO = 0x15,
    ITEM_KUCHI       = 0x16,
    ITEM_ITEM09      = 0x17,
    ITEM_ITEM10      = 0x18,
    ITEM_ITEM11      = 0x19,
    ITEM_ITEM12      = 0x1A,
    ITEM_ITEM13      = 0x1B,
    ITEM_ITEM14      = 0x1C,
    ITEM_ITEM15      = 0x1D,
    ITEM_ITEM16      = 0x1E,
    ITEM_MAX         = 0x1F,
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
    ITEM_SUB_JOHA_MAE    = 0x0B,
    ITEM_SUB_JOHA_USHIRO = 0x0C,
    ITEM_SUB_HADA        = 0x0D,
    ITEM_SUB_KATA        = 0x0E,
    ITEM_SUB_U_UDE       = 0x0F,
    ITEM_SUB_L_UDE       = 0x10,
    ITEM_SUB_TE          = 0x11,
    ITEM_SUB_BELT        = 0x12,
    ITEM_SUB_COSI        = 0x13,
    ITEM_SUB_PANTS       = 0x14,
    ITEM_SUB_ASI         = 0x15,
    ITEM_SUB_SUNE        = 0x16,
    ITEM_SUB_KUTSU       = 0x17,
    ITEM_SUB_HEAD        = 0x18,
    ITEM_SUB_MAX         = 0x19,
};

struct itm_table_item_data_obj {
    std::string uid;
    item_id rpk;

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
    virtual ~itm_table();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

extern const char* chara_auth_3d_names[];
extern const char* chara_face_mot_names[];
extern const char* chara_full_names[];
extern const char* chara_names[];

extern const char* chara_index_get_auth_3d_name(chara_index chara_index);
extern const char* chara_index_get_chara_name(chara_index chara_index);
extern const char* chara_index_get_face_mot_name(chara_index chara_index);
extern chara_index chara_index_get_from_chara_name(const char* str);
extern const char* chara_index_get_name(chara_index chara_index);
