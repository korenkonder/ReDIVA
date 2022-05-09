/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.h"

struct pvsr_a3da {
    std::string name;
    uint32_t hash;

    pvsr_a3da();
    ~pvsr_a3da();
};

struct pvsr_aet {
    std::string name;
    std::uint32_t hash;
    float_t bright_scale;

    pvsr_aet();
    ~pvsr_aet();
};

struct pvsr_effect {
    std::string name;
    float_t emission;

    pvsr_effect();
    ~pvsr_effect();
};

struct pvsr_glitter {
    std::string name;
    uint8_t unk1;

    pvsr_glitter();
    ~pvsr_glitter();
};

struct pvsr_stage_effect {
    uint8_t bar_count;
    std::vector<pvsr_a3da> a3da;
    std::vector<pvsr_glitter> glitter;

    pvsr_stage_effect();
    ~pvsr_stage_effect();
};

struct pvsr_stage_effect_env_sub1 {
    std::string name;
    uint32_t hash;
    uint16_t unk2;

    pvsr_stage_effect_env_sub1();
    ~pvsr_stage_effect_env_sub1();
};

struct pvsr_stage_effect_env_sub2 {
    uint16_t u00;
    uint16_t u02;
    uint16_t u04;
    uint16_t u06;
    uint16_t u08;
    uint16_t u0a;
    uint16_t u0c;
};

struct pvsr_stage_effect_env {
    uint16_t u48;
    uint16_t u4a;
    uint16_t u4c;
    uint16_t u4e;
    uint16_t u50;
    uint16_t u52;
    uint16_t u54;
    uint16_t u56;
    uint16_t u58;
    uint16_t u5a;
    uint16_t u5c;
    uint16_t u5e;
    uint16_t u60;

    std::string set_name;
    std::vector<pvsr_aet> aet_front;
    std::vector<pvsr_aet> aet_front_low;
    std::vector<pvsr_aet> aet_back;
    pvsr_stage_effect_env_sub1 sub1_data;
    bool sub1_data_init;
    pvsr_stage_effect_env_sub2 sub2a_data;
    bool sub2a_data_init;
    pvsr_stage_effect_env_sub2 sub2b_data;
    bool sub2b_data_init;
    pvsr_stage_effect_env_sub2 sub2c_data;
    bool sub2c_data_init;
    pvsr_stage_effect_env_sub2 sub2d_data;
    bool sub2d_data_init;
    std::vector<pvsr_aet> unk03;
    std::vector<pvsr_aet> unk04;

    pvsr_stage_effect_env();
    ~pvsr_stage_effect_env();
};

#define PVSR_STAGE_CHANGE_EFFECT_COUNT 0x10

struct pvsr {
    bool ready;

    std::vector<pvsr_effect> effect;
    std::vector<std::string> emcs;
    pvsr_stage_effect stage_change_effect[PVSR_STAGE_CHANGE_EFFECT_COUNT][PVSR_STAGE_CHANGE_EFFECT_COUNT];
    bool stage_change_effect_init[PVSR_STAGE_CHANGE_EFFECT_COUNT][PVSR_STAGE_CHANGE_EFFECT_COUNT];
    std::vector<pvsr_stage_effect> stage_effect;
    std::vector<pvsr_stage_effect_env> stage_effect_env;

    pvsr();
    virtual ~pvsr();

    void read(const char* path);
    void read(const wchar_t* path);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
