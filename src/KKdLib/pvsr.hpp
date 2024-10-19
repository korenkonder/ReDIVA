/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "hash.hpp"

enum pvsr_auth_3d_flags : uint8_t {
    PVSR_AUTH_3D_REPEAT         = 0x01,
    PVSR_AUTH_3D_MOVIE_TEXTURE  = 0x02,
    PVSR_AUTH_3D_RENDER_TEXTURE = 0x04,
    PVSR_AUTH_3D_MAIN           = 0x08,
};

enum pvsr_glitter_flags : uint8_t {
    PVSR_GLITTER_FORCE_DISP = 0x01,
};

struct pvsr_auth_2d {
    string_hash name;
    float_t bright_scale;

    pvsr_auth_2d();
    ~pvsr_auth_2d();
};

struct pvsr_auth_3d {
    string_hash name;
    pvsr_auth_3d_flags flags;

    pvsr_auth_3d();
    ~pvsr_auth_3d();
};

struct pvsr_effect {
    string_hash name;
    float_t emission;

    pvsr_effect();
    ~pvsr_effect();
};

struct pvsr_glitter {
    string_hash name;
    int8_t fade_time;
    pvsr_glitter_flags flags;

    pvsr_glitter();
    ~pvsr_glitter();
};

struct pvsr_stage_change_effect {
    bool enable;
    int8_t bar_count;
    std::vector<pvsr_auth_3d> auth_3d;
    std::vector<pvsr_glitter> glitter;

    pvsr_stage_change_effect();
    ~pvsr_stage_change_effect();
};

struct pvsr_stage_effect {
    std::vector<pvsr_auth_3d> auth_3d;
    std::vector<pvsr_glitter> glitter;

    pvsr_stage_effect();
    ~pvsr_stage_effect();
};

struct pvsr_stage_effect_env_light {
    string_hash auth_3d_name;
    int16_t stage_light;

    pvsr_stage_effect_env_light();
    ~pvsr_stage_effect_env_light();
};

struct pvsr_stage_effect_env_post_process {
    int16_t index0;
    int16_t start0;
    int16_t trans0;
    int16_t index1;
    int16_t start1;
    int16_t trans1;
    int16_t index2;

    pvsr_stage_effect_env_post_process();
};

struct pvsr_stage_effect_env {
    int16_t chara_light;
    int16_t chara_shadow_light;
    int16_t shadow_light;
    int16_t item_light;
    int16_t common_light;
    int16_t u52;
    int16_t ibl_color_light;
    int16_t u56;
    int16_t cat;
    int16_t u5a;
    int16_t u5c;
    int16_t reflection;
    int16_t effect_light;

    std::string set_name;
    std::vector<pvsr_auth_2d> aet_front;
    std::vector<pvsr_auth_2d> aet_front_low;
    std::vector<pvsr_auth_2d> aet_back;
    pvsr_stage_effect_env_light light;
    bool light_init;
    pvsr_stage_effect_env_post_process fog;
    bool fog_init;
    pvsr_stage_effect_env_post_process bloom;
    bool bloom_init;
    pvsr_stage_effect_env_post_process cct;
    bool cct_init;
    pvsr_stage_effect_env_post_process shimmer;
    bool shimmer_init;
    std::vector<pvsr_auth_2d> unk03;
    std::vector<pvsr_auth_2d> unk04;

    pvsr_stage_effect_env();
    ~pvsr_stage_effect_env();
};

#define PVSR_STAGE_EFFECT_COUNT 0x10

struct pvsr {
    bool ready;
    bool big_endian;

    std::vector<pvsr_effect> effect;
    std::vector<std::string> emcs;
    pvsr_stage_change_effect stage_change_effect[PVSR_STAGE_EFFECT_COUNT][PVSR_STAGE_EFFECT_COUNT];
    std::vector<pvsr_stage_effect> stage_effect;
    std::vector<pvsr_stage_effect_env> stage_effect_env;

    pvsr();
    ~pvsr();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
