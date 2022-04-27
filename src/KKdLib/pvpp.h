/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.h"
#include "vec.h"

enum pvpp_chara_index {
    PVPP_CHARA_1P = 0,
    PVPP_CHARA_2P = 1,
    PVPP_CHARA_3P = 2,
    PVPP_CHARA_4P = 3,
};

enum pvpp_chara_type {
    PVPP_CHARA_MIKU   = 0,
    PVPP_CHARA_RIN    = 1,
    PVPP_CHARA_LEN    = 2,
    PVPP_CHARA_LUKA   = 3,
    PVPP_CHARA_NERU   = 4,
    PVPP_CHARA_HAKU   = 5,
    PVPP_CHARA_KAITO  = 6,
    PVPP_CHARA_MEIKO  = 7,
    PVPP_CHARA_SAKINE = 8,
    PVPP_CHARA_TETO   = 9,
    PVPP_CHARA_EXTRA  = 10,
    PVPP_CHARA_STAGE  = 255,
};

struct pvpp_a3da {
    uint32_t hash;
    std::string name;

    pvpp_a3da();
    ~pvpp_a3da();
};

struct pvpp_chara_effect_a3da {
    pvpp_a3da a3da;
    uint8_t u00;
    uint8_t u01;
    uint8_t chara_index;
    uint8_t u03;
    uint8_t u04;
    uint8_t u05;
    uint8_t u06;
    uint8_t u07;

    pvpp_chara_effect_a3da();
    ~pvpp_chara_effect_a3da();
};

struct pvpp_chara_effect {
    pvpp_chara_type base_chara;
    pvpp_chara_index chara_index;
    std::vector<pvpp_chara_effect_a3da> effect_a3da;

    pvpp_chara_effect();
    ~pvpp_chara_effect();
};

struct pvpp_chara_item {
    std::vector<pvpp_a3da> a3da;
    std::string bone;
    vec4u u18;

    pvpp_chara_item();
    ~pvpp_chara_item();
};

struct pvpp_glitter {
    std::string name;
    std::string unk1;
    bool unk2;

    pvpp_glitter();
    ~pvpp_glitter();
};

struct pvpp_motion {
    uint32_t hash;
    std::string name;

    pvpp_motion();
    ~pvpp_motion();
};

struct pvpp_effect {
    std::vector<pvpp_a3da> a3da;
    pvpp_chara_type chara_index;
    std::vector<pvpp_glitter> glitter;

    pvpp_effect();
    ~pvpp_effect();
};

struct pvpp_chara {
    std::vector<pvpp_a3da> a3da;
    std::vector<pvpp_chara_item> item;
    std::vector<pvpp_chara_effect> chara_effect;
    std::vector<pvpp_glitter> glitter;
    std::vector<pvpp_motion> motion;

    pvpp_chara();
    ~pvpp_chara();
};

struct pvpp {
    bool ready;

    std::vector<pvpp_chara> chara;
    std::vector<pvpp_effect> effect;

    pvpp();
    virtual ~pvpp();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t length);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

