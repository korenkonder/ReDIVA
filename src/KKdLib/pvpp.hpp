/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "hash.hpp"
#include "vec.hpp"

enum pvpp_chara_id : int8_t {
    PVPP_CHARA_PNONE =   -1,
    PVPP_CHARA_1P    = 0x00,
    PVPP_CHARA_2P    = 0x01,
    PVPP_CHARA_3P    = 0x02,
    PVPP_CHARA_4P    = 0x03,
};

enum pvpp_chara_index : int8_t {
    PVPP_CHARA_NONE   =   -1,
    PVPP_CHARA_MIKU   = 0x00,
    PVPP_CHARA_RIN    = 0x01,
    PVPP_CHARA_LEN    = 0x02,
    PVPP_CHARA_LUKA   = 0x03,
    PVPP_CHARA_NERU   = 0x04,
    PVPP_CHARA_HAKU   = 0x05,
    PVPP_CHARA_KAITO  = 0x06,
    PVPP_CHARA_MEIKO  = 0x07,
    PVPP_CHARA_SAKINE = 0x08,
    PVPP_CHARA_TETO   = 0x09,
    PVPP_CHARA_EXTRA  = 0x0A,
};

enum pvpp_chara_item_type : int8_t {
    PVPP_CHARA_ITEM_NONE    =   -1,
    PVPP_CHARA_ITEM_OBJ     = 0x00,
    PVPP_CHARA_ITEM_OBJ_HRC = 0x01,
};

struct pvpp_chara_effect_auth_3d {
    string_hash auth_3d;
    string_hash object_set;
    bool has_object_set;
    uint8_t u00;
    uint8_t u01;
    uint8_t u02;
    uint8_t u03;
    uint8_t u04;
    uint8_t u05;
    uint8_t u06;
    uint8_t u07;

    pvpp_chara_effect_auth_3d();
    ~pvpp_chara_effect_auth_3d();
};

struct pvpp_chara_effect {
    pvpp_chara_index base_chara;
    pvpp_chara_id chara_id;
    std::vector<pvpp_chara_effect_auth_3d> auth_3d;

    pvpp_chara_effect();
    ~pvpp_chara_effect();
};

struct pvpp_chara_item {
    pvpp_chara_item_type type;
    string_hash auth_3d;
    std::string node;
    vec4 u18;

    pvpp_chara_item();
    ~pvpp_chara_item();
};

struct pvpp_glitter {
    string_hash name;
    std::string unk1;
    bool unk2;

    pvpp_glitter();
    ~pvpp_glitter();
};

struct pvpp_effect {
    std::vector<string_hash> auth_3d;
    pvpp_chara_id chara_id;
    std::vector<pvpp_glitter> glitter;

    pvpp_effect();
    ~pvpp_effect();
};

struct pvpp_chara {
    std::vector<string_hash> auth_3d;
    std::vector<pvpp_chara_item> item;
    pvpp_chara_effect chara_effect;
    bool chara_effect_init;
    std::vector<pvpp_glitter> glitter;
    std::vector<string_hash> motion;

    pvpp_chara();
    ~pvpp_chara();
};

struct pvpp {
    bool ready;
    bool big_endian;

    std::vector<pvpp_chara> chara;
    std::vector<pvpp_effect> effect;

    pvpp();
    ~pvpp();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

