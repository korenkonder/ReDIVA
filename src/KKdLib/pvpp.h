/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.h"
#include "vec.h"

typedef enum pvpp_chara_type {
    PVPP_CHARA_MIKU = 0,
    PVPP_CHARA_RIN = 1,
    PVPP_CHARA_LEN = 2,
    PVPP_CHARA_LUKA = 3,
    PVPP_CHARA_NERU = 4,
    PVPP_CHARA_HAKU = 5,
    PVPP_CHARA_KAITO = 6,
    PVPP_CHARA_MEIKO = 7,
    PVPP_CHARA_SAKINE = 8,
    PVPP_CHARA_TETO = 9,
    PVPP_CHARA_EXTRA = 10,
    PVPP_CHARA_STAGE = 255,
} pvpp_chara_type;

class pvpp_a3da {
public:
    uint32_t hash;
    std::string name;

    pvpp_a3da();
    ~pvpp_a3da();
};

class pvpp_chara_effect_a3da {
public:
    pvpp_a3da a3da;
    uint8_t u00;
    uint8_t u01;
    uint8_t u02;
    uint8_t u03;
    uint8_t u04;
    uint8_t u05;
    uint8_t u06;
    uint8_t u07;

    pvpp_chara_effect_a3da();
    ~pvpp_chara_effect_a3da();
};

class pvpp_chara_effect {
public:
    pvpp_chara_type base_chara;
    std::vector<pvpp_chara_effect_a3da> effect_a3da;

    pvpp_chara_effect();
    ~pvpp_chara_effect();
};

class pvpp_chara_item {
public:
    std::vector<pvpp_a3da> a3da;
    std::string bone;
    vec4u u18;

    pvpp_chara_item();
    ~pvpp_chara_item();
};

class pvpp_glitter {
public:
    std::string name;
    std::string unk1;
    bool unk2;

    pvpp_glitter();
    ~pvpp_glitter();
};

class pvpp_motion {
public:
    uint32_t hash;
    std::string name;

    pvpp_motion();
    ~pvpp_motion();
};

class pvpp_effect {
public:
    std::vector<pvpp_a3da> a3da;
    pvpp_chara_type chara_index;
    std::vector<pvpp_glitter> glitter;

    pvpp_effect();
    ~pvpp_effect();
};

class pvpp_chara {
public:
    std::vector<pvpp_a3da> a3da;
    std::vector<pvpp_chara_item> item;
    std::vector<pvpp_chara_effect> chara_effect;
    std::vector<pvpp_glitter> glitter;
    std::vector<pvpp_motion> motion;

    pvpp_chara();
    ~pvpp_chara();
};

class pvpp {
public:
    bool ready;

    std::vector<pvpp_chara> chara;
    std::vector<pvpp_effect> effect;

    pvpp();
    void read(char* path);
    void read(wchar_t* path);
    void read(void* data, size_t length);
    virtual ~pvpp();
};

extern bool pvpp_load_file(void* data, char* path, char* file, uint32_t hash);
