/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include "../default.h"
#include "../vec.h"

typedef enum itm_id {
    ITM_NONE        = -1,
    ITM_BODY        = 0x00,
    ITM_ATAMA       = 0x01,
    ITM_KATA_R      = 0x02,
    ITM_MUNE        = 0x03,
    ITM_KATA_L      = 0x04,
    ITM_UDE_R       = 0x05,
    ITM_SENAKA      = 0x06,
    ITM_UDE_L       = 0x07,
    ITM_HARA        = 0x08,
    ITM_KOSI        = 0x09,
    ITM_TE_R        = 0x0A,
    ITM_TE_L        = 0x0B,
    ITM_MOMO        = 0x0C,
    ITM_SUNE        = 0x0D,
    ITM_ASI         = 0x0E,
    ITM_KAMI        = 0x0F,
    ITM_OUTER       = 0x10,
    ITM_PANTS       = 0x11,
    ITM_ZUJO        = 0x12,
    ITM_MEGANE      = 0x13,
    ITM_KUBI        = 0x14,
    ITM_JOHA_USHIRO = 0x15,
    ITM_KUCHI       = 0x16,
    ITM_ITEM09      = 0x17,
    ITM_ITEM10      = 0x18,
    ITM_ITEM11      = 0x19,
    ITM_ITEM12      = 0x1A,
    ITM_ITEM13      = 0x1B,
    ITM_ITEM14      = 0x1C,
    ITM_ITEM15      = 0x1D,
    ITM_ITEM16      = 0x1E,
    ITM_MAX         = 0x1F,
} itm_id;

typedef enum itm_sub_id {
    ITM_SUB_NONE        = -1,
    ITM_SUB_ZUJO        = 0x00,
    ITM_SUB_KAMI        = 0x01,
    ITM_SUB_HITAI       = 0x02,
    ITM_SUB_ME          = 0x03,
    ITM_SUB_MEGANE      = 0x04,
    ITM_SUB_MIMI        = 0x05,
    ITM_SUB_KUCHI       = 0x06,
    ITM_SUB_MAKI        = 0x07,
    ITM_SUB_KUBI        = 0x08,
    ITM_SUB_INNER       = 0x09,
    ITM_SUB_OUTER       = 0x0A,
    ITM_SUB_JOHA_MAE    = 0x0B,
    ITM_SUB_JOHA_USHIRO = 0x0C,
    ITM_SUB_HADA        = 0x0D,
    ITM_SUB_KATA        = 0x0E,
    ITM_SUB_U_UDE       = 0x0F,
    ITM_SUB_L_UDE       = 0x10,
    ITM_SUB_TE          = 0x11,
    ITM_SUB_BELT        = 0x12,
    ITM_SUB_COSI        = 0x13,
    ITM_SUB_PANTS       = 0x14,
    ITM_SUB_ASI         = 0x15,
    ITM_SUB_SUNE        = 0x16,
    ITM_SUB_KUTSU       = 0x17,
    ITM_SUB_HEAD        = 0x18,
    ITM_SUB_MAX         = 0x19,
} itm_sub_id;

typedef struct itm_table_itm_data_obj {
    std::string uid;
    itm_id rpk;
} itm_table_itm_data_obj;

typedef struct itm_table_itm_data_ofs {
    itm_sub_id sub_id;
    int32_t no;
    vec3 position;
    vec3 rotation;
    vec3 scale;
} itm_table_itm_data_ofs;

typedef struct itm_table_itm_data_tex {
    std::string org;
    std::string chg;
} itm_table_itm_data_tex;

typedef struct itm_table_itm_data_col_data {
    vec3 blend;
    vec3 offset;
    float_t hue;
    float_t saturation;
    float_t value;
    float_t constrast;
    bool inverse;
} itm_table_itm_data_col_data;

typedef struct itm_table_itm_data_col {
    std::string tex;
    int32_t flag;
    itm_table_itm_data_col_data data;
} itm_table_itm_data_col;

class itm_table_itm_data {
public:
    std::vector<itm_table_itm_data_obj> obj;
    std::vector<itm_table_itm_data_ofs> ofs;
    std::vector<itm_table_itm_data_tex> tex;
    std::vector<itm_table_itm_data_col> col;

    itm_table_itm_data();
    ~itm_table_itm_data();
};

class itm_table_item {
public:
    int32_t flag;
    std::string name;
    std::vector<std::string> objset;
    int32_t type;
    int32_t attr;
    int32_t des_id;
    itm_sub_id sub_id;
    itm_table_itm_data data;
    int32_t exclusion_point;
    int32_t field_AC;
    int32_t org_itm;
    bool npr_flag;
    float_t face_depth;

    itm_table_item();
    ~itm_table_item();
};

class itm_table_cos {
public:
    int32_t id;
    std::vector<int32_t> item;

    itm_table_cos();
    ~itm_table_cos();
};

class itm_table_dbgset {
public:
    std::string name;
    std::vector<int32_t> item;

    itm_table_dbgset();
    ~itm_table_dbgset();
};

class itm_table {
public:
    bool ready;

    std::vector<std::pair<int32_t, itm_table_item>> item;
    std::vector<itm_table_cos> cos;
    std::vector<itm_table_dbgset> dbgset;

    itm_table();
    ~itm_table();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t length);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* length);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
