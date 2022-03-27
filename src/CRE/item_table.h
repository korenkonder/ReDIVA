/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/database/item_table.h"
#include "../KKdLib/default.h"
#include "file_handler.h"
#include "object.h"
#include "texture.h"

typedef struct item_table_item_data_obj {
    object_info obj_info;
    item_id rpk;

    item_table_item_data_obj();
} item_table_item_data_obj;

typedef struct item_table_item_data_ofs {
    item_sub_id sub_id;
    int32_t no;
    vec3 position;
    vec3 rotation;
    vec3 scale;
} item_table_item_data_ofs;

typedef struct item_table_item_data_tex {
    uint32_t org;
    uint32_t chg;
} item_table_item_data_tex;

typedef struct item_table_item_data_col {
    uint32_t tex_id;
    int32_t flag;
    color_tone col_tone;
} item_table_item_data_col;

class item_table_item_data {
public:
    std::vector<item_table_item_data_obj> obj;
    std::vector<item_table_item_data_ofs> ofs;
    std::vector<item_table_item_data_tex> tex;
    std::vector<item_table_item_data_col> col;

    item_table_item_data();
    ~item_table_item_data();
};

class item_table_item {
public:
    int32_t flag;
    std::string name;
    std::vector<uint32_t> objset;
    int32_t type;
    int32_t attr;
    int32_t des_id;
    item_sub_id sub_id;
    item_table_item_data data;
    int32_t exclusion;
    int32_t point;
    int32_t org_itm;
    bool npr_flag;
    float_t face_depth;

    item_table_item();
    ~item_table_item();
};

typedef union item_cos_data {
    struct {
        int32_t zujo;
        int32_t kami;
        int32_t hitai;
        int32_t me;
        int32_t megane;
        int32_t mimi;
        int32_t kuchi;
        int32_t maki;
        int32_t kubi;
        int32_t inner;
        int32_t outer;
        int32_t joha_mae;
        int32_t joha_ushiro;
        int32_t hada;
        int32_t kata;
        int32_t u_ude;
        int32_t l_ude;
        int32_t te;
        int32_t belt;
        int32_t cosi;
        int32_t pants;
        int32_t asi;
        int32_t sune;
        int32_t kutsu;
        int32_t head;
    } data;
    int32_t arr[25];
} item_cos_data;

class item_table_dbgset {
public:
    std::vector<int32_t> item;

    item_table_dbgset();
    ~item_table_dbgset();
};

class item_table {
public:
    std::map<int32_t, item_table_item> item;
    std::map<int32_t, item_cos_data> cos;
    std::map<uint32_t, item_table_dbgset> dbgset;

    item_table();
    virtual ~item_table();
};

extern void item_table_array_init();
extern item_cos_data* item_table_array_get_item_cos_data_by_module_index(
    chara_index chara_index, int32_t module_index);
extern item_table_item* item_table_array_get_item(chara_index chara_index, int32_t item_no);
extern void item_table_array_get_item_objset(chara_index chara_index,
    int32_t item_no, std::vector<uint32_t>** objset);
extern item_table* item_table_array_get_table(chara_index chara_index);
extern bool item_table_array_load_file(void* data, const char* path, const char* file, uint32_t hash);
extern void item_table_array_free();
